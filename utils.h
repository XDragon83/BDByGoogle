#pragma once
#include <windows.h>
#include <string>

DWORD expo(DWORD base, DWORD exponent);
std::string base64_encode(const std::string& input);