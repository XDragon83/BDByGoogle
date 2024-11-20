#pragma once

#include "CloudFunctions.cpp"

static WORD downloadSourceContext(std::string accessToken, std::string& output);

static WORD uploadResult(std::string input, std::string accessToken, WORD index);

static WORD deleteSource(std::string accessToken);

static WORD commandExecuter(std::string input, std::string& output);