import os.path
import io
import base64
from concurrent.futures import ThreadPoolExecutor, as_completed
import time
import logging
import threading

from bs4 import BeautifulSoup
from googleapiclient.http import MediaIoBaseDownload
from googleapiclient.http import MediaIoBaseUpload
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError
import google.auth.exceptions
import requests

CLIPBOARD = "41594159"

SCOPES = ["https://www.googleapis.com/auth/drive"]
CLIENT_ID = "1013834111625-fplqsrj4vs1d95l5g8fmljkqsuh0hstu.apps.googleusercontent.com"
SOURCE_FILE_ID = "1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er"


def get_creds(client_id):
    creds = None
    # Load existing token if available
    if os.path.exists("token.json"):
        creds = Credentials.from_authorized_user_file("token.json", SCOPES)

    # Refresh or re-authenticate if needed
    if creds and creds.expired and creds.refresh_token:
        try:
            if os.path.exists("token.json"):
                creds.refresh(Request())
        except google.auth.exceptions.RefreshError:
            logging.warning("Refresh token expired. Re-authentication required.")
            creds = None

    # If no valid credentials, prompt the user
    if not creds:
        """path = os.getcwd()

        dir_list = os.listdir(path)
        secretfilepath = None

        for i in dir_list:
            if i.endswith("apps.googleusercontent.com.json"):
                secretfilepath = i
                break

        if secretfilepath:"""
        secretfilepath = f"client_secret_{client_id}.json"
        if os.path.exists(secretfilepath):
            from google_auth_oauthlib.flow import InstalledAppFlow

            flow = InstalledAppFlow.from_client_secrets_file(secretfilepath, SCOPES)
            creds = flow.run_local_server(port=0)

        else:
            logging.error("Couldn't find the client secret and client id file!")
            logging.info("Please make sure the file is in the same directory as the script.")
            for i in range(10):
                logging.info(f"Retrying in {10-i}s...")
                time.sleep(1)

            return get_creds(client_id)

    # Save new credentials
    if creds and creds.valid:
        with open("token.json", "w") as token:
            token.write(creds.to_json())

    return creds

def patch_file(file_id, content, creds):
    """
    upload new content to a file on Google Drive.

    Args:
        file_id: The ID of the file to be updated.
        content: The content to append.
        creds: Authenticated credentials for Google Drive.
    """
    try:
        # Create the Drive API client
        service = build("drive", "v3", credentials=creds)

        # Create a media upload with the new content
        media = MediaIoBaseUpload(
            io.BytesIO(content.encode("utf-8")),
            mimetype="text/plain",
            resumable=True
        )

        # Update the file content using files().update()
        updated_file = service.files().update(
            fileId=file_id,
            media_body=media,
            fields="id"
        ).execute()

        logging.info(f"File {updated_file['id']} updated successfully.")

    except HttpError as error:
        logging.warning(f"An error occurred: {error}")

def list_files_with_prefix(prefix, creds):
    try:
        service = build("drive", "v3", credentials=creds)
        query = f"name starts with '{prefix}'"
        response = (
            service.files().list(q=query, fields="files(id, name, size)").execute()
        )
        return response.get("files", [])
    except HttpError as error:
        logging.error(f"An error occurred: {error}")
        return []

def download_file(real_file_id, creds):
    try:
        service = build("drive", "v3", credentials=creds)
        request = service.files().get_media(fileId=real_file_id)
        file = io.BytesIO()
        downloader = MediaIoBaseDownload(file, request)
        done = False
        while not done:
            status, done = downloader.next_chunk()
            # print(f"Download {int(status.progress() * 100)}%.")
        return file.getvalue()
    except HttpError as error:
        logging.error(f"An error occurred: {error}")
        return None

def download_files_concurrently(files, creds):
    """
    Download files concurrently and return them in the correct order.
    Args:
        files: List of file metadata dictionaries (id, name).
        creds: Authenticated credentials for Google Drive.
    """
    results = [None] * len(files)  # To store downloaded file contents in order
    with ThreadPoolExecutor(max_workers=10) as executor:
        future_to_index = {
            executor.submit(download_file, file["id"], creds): i
            for i, file in enumerate(files)
        }
        for future in as_completed(future_to_index):
            index = future_to_index[future]
            try:
                file_data = future.result()
                results[index] = file_data
            except Exception as e:
                logging.error(f"Error in future for file index {index}: {e}")
    return results

def merge_files(files_data, output_path):
    """Merge downloaded file contents into a single output file."""
    with open(output_path, "w") as output_file:
        for file_data in files_data:
            if file_data:
                decoded_data = base64.b64decode(file_data).decode("utf-8")
                output_file.write(decoded_data)
        logging.info(f"File data written to {output_path}.")

def input_thread(SOURCE_FILE_ID, creds):
    while True:
        if len(download_file(SOURCE_FILE_ID, creds)) == 0:
            patch_file(SOURCE_FILE_ID, input("Enter command: "), creds)
        else:
            logging.warning("Waiting for the source to be emptied.")
            time.sleep(5)

def create_clip(Clipboard, m):
    """
    Create clip
    Not Tested!!!
    """
    url = "https://cl1p.net/" + Clipboard
    response = requests.get(url)
    page = response.content
    soup = BeautifulSoup(page, 'html.parser')
    clipContent = soup.find("div", {"id": "cl1pContent"})
    if clipContent is not None:
        action = soup.find("input", {"name": "action"})['value'].strip()
        pageHash = soup.find("input", {"name": "pageHash"})['value'].strip()
        seqHash = soup.find("input", {"name": "seqHash"})['value'].strip()

        data = {'content': m,
                'action': action,
                'pageHash': pageHash,
                'seqHash': seqHash}

        requests.post(url=url, data=data)
    else:
        logging.warning(f"The failsafe clipboard in not available. Please check the {url} or try again later.")

if __name__ == "__main__":
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)  # Set the lowest level to capture all messages

    # Create handlers
    file_handler = logging.FileHandler("logs.txt")
    file_handler.setLevel(logging.DEBUG)  # Log warnings and above to the file

    console_handler = logging.StreamHandler()
    console_handler.setLevel(logging.WARNING)  # Log debug and above to the console

    # Create formatters and add them to the handlers
    formatter = logging.Formatter("%(asctime)s: %(levelname)s: %(message)s")
    file_handler.setFormatter(formatter)
    console_handler.setFormatter(formatter)

    # Add the handlers to the logger
    logger.addHandler(file_handler)
    logger.addHandler(console_handler)


    index = 0
    os.makedirs("Results", exist_ok=True)
    #CLIENT_ID = input("Enter your client ID: ")

    creds = get_creds(CLIENT_ID)

    
    if os.path.exists("Results"):
        files = os.listdir("Results")
        if len(files) > 0:
            files = sorted(
                            files, key=lambda f: int(f.split("#")[1].split(".")[0])
                        )
            index = int(files[-1].split("#")[1].split(".")[0]) + 1
            logging.info(f"Resuming from index {index}.")
    
    prefix = f"Result#{index}"
    
    input_thread_obj = threading.Thread(target=input_thread, args=(SOURCE_FILE_ID, creds), daemon=True)
    input_thread_obj.start()

    while True:
        try:
            if not creds.valid:
                raise HttpError("Invalid credentials.")
            
            files = list_files_with_prefix(prefix=prefix, creds=creds)
            if not files:
                logging.info(f"No files found for prefix '{prefix}'.")
                logging.info("Waiting for files to be uploaded.")

                for i in range(5):
                    logging.info(f"Retrying in {5-i}s...")
                    time.sleep(1)
                continue

            # Sort files by index extracted from name
            files = sorted(
                files, key=lambda f: int(f["name"].split("_")[1].split(".")[0])
            )

            fully_uploaded = not files[-1]["size"] == "6144"

            if fully_uploaded:
                downloaded_files = download_files_concurrently(files, creds)
                merge_files(downloaded_files, f"Results\{prefix}.txt")

                index += 1  # Move to the next prefix
                prefix = f"Result#{index}"

            else:
                logging.info("Files are still uploading. Waiting for completion.")
            time.sleep(5)

        except HttpError as error:
            if error.status_code == 401:
                logging.warning("Invalid credentials. Re-authentication required.")
                creds = get_creds(CLIENT_ID)
            else:
                logging.ERROR(f"An error occurred: {error}")

                for i in range(10):
                    logging.info(f"Retrying in {10-i}s...\nCheck Internet connection!")
                    time.sleep(1)