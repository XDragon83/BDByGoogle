#pragma once

#include <vector>

class MyString
{
public:
	MyString();
	MyString(char* context, bool linked = 0);
	MyString(const char* contex);
	MyString(char* context, size_t length);
	MyString(size_t length);

	static inline bool is_base64(char c);

	MyString toBase64();
	MyString fromBase64();

	MyString operator+(const MyString& other);
	MyString operator+(const char* other);
	MyString operator=(const char* other);
	MyString operator=(char* other);

	std::vector<MyString*> getEachLine();
	std::vector<MyString*> getEachLine(MyString custom);
	//std::vector<MyString*> getEachLine(MyString input);
	//std::vector<MyString*> getEachLine(FILE* input);

	MyString getSubString(size_t sIndex, size_t fIndex);
	char* getContext();
	size_t getLength();
	int setContext();
	char getCharAt(size_t index);
	bool setCharAt(size_t index, char c);

	//~MyString();

private:
	char* context;
	size_t length;
};