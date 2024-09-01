#include "MyString.h"
#include "ostream"
//#include "pch.h"

static const char* base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

//inline static char to_base64_char(unsigned short index) {
//	if (index >= 0 && index < 26)
//	{
//		return 'A' + index;
//	}
//	else if (index >= 26 && index < 52) {
//		return 'a' + index;
//	}
//	else if (index >= 52 && index < 62) {
//		return '0' + index;
//	}
//	else if (index == 62) {
//		return '+';
//	}
//	else if (index == 63) {
//		return '/';
//	}
//	return '\0';
//}

MyString::MyString() : length(0)
{
	this->context = new char[1];
	this->context[0] = '\0';
}

MyString::MyString(char* context, bool linked) {
	size_t length = 0;
	while (context[length++] != '\0');
	this->length = length - 1;
	if (linked)
	{
		this->context = context;
	}
	else {

		this->context = new char[length];
		for (size_t i = 0; i < length; i++)
		{
			this->context[i] = context[i];
		}
	}
}

MyString::MyString(const char* context) {
	size_t length = 0;
	while (context[length++] != '\0');

	this->length = length - 1;
	this->context = new char[length + 1];

	for (size_t i = 0; i < length + 1; i++)
	{
		this->context[i] = context[i];
	}

}

MyString::MyString(char* context, size_t length) : length(length) {
	this->context = new char[length + 1];
	this->context[length] = '\0';
	for (size_t i = 0; i < length; i++)
	{
		this->context[i] = context[i];
	}
}

MyString::MyString(size_t length) : length(length)
{
	char* context = new char[length + 1];
	context[length] = '\0';
	this->context = context;
}

inline bool MyString::is_base64(char c)
{

	return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '+' || c == '/');
}

MyString MyString::toBase64()
{
	size_t in_len = this->getLength();
	MyString result = "";
	char* bytes_to_encode = this->context;
	int i = 0;
	int j = 0;
	//char char_array_3[3];
	char char_array_4[5] = { '\0', '\0' ,'\0' ,'\0' ,'\0' };

	for (size_t i = 0; i < this->getLength(); i += 3)
	{
		char_array_4[0] = (this->getCharAt(i) & 0xfc) >> 2;
		char_array_4[1] = ((this->getCharAt(i) & 0x03) << 4) + ((this->getCharAt(i + 1) & 0xf0) >> 4);
		char_array_4[2] = ((this->getCharAt(i + 1) & 0x0f) << 2) + ((this->getCharAt(i + 2) & 0xc0) >> 6);
		char_array_4[3] = this->getCharAt(i + 2) & 0x3f;

		char_array_4[0] = base64_chars[char_array_4[0]];
		char_array_4[1] = base64_chars[char_array_4[1]];
		char_array_4[2] = this->getLength() - i == 1 ? '=' : base64_chars[char_array_4[2]];
		char_array_4[3] = this->getLength() - i == 1 || this->getLength() - i == 2 ? '=' : base64_chars[char_array_4[3]];

		result = result + char_array_4;
	}
	return result;
}

//MyString::~MyString() {
//    if (this->context) {
//        delete[] this->context;
//    }
//}


MyString MyString::operator+(const MyString& other)
{
	size_t resultLength = length + other.length;
	char* resultContext = new char[resultLength + 1];

	size_t i = 0;
	while (i < length)
	{
		resultContext[i] = context[i];
		i++;
	}

	while (i < resultLength + 1)
	{
		resultContext[i] = other.context[i - length];
		i++;
	}

	MyString* result = new MyString(resultContext, resultLength);
	return *result;
}

MyString MyString::operator+(const char* other) {
	MyString* temp = new MyString(other);

	return *this + *temp;
}

MyString MyString::operator=(const char* other) {
	delete this;
	MyString* resultObj = new MyString(other);
	return *resultObj;

}

MyString MyString::operator=(char* other) {
	delete this;
	MyString* resultObj = new MyString(other);
	return *resultObj;
}


std::vector<MyString*> MyString::getEachLine()
{
	std::vector<MyString*> output;
	size_t lastIndex = 0, iterator = 0;

	while (iterator < this->getLength())
	{
		if (this->getCharAt(iterator) == '\n')
		{
			output.push_back(new MyString(this->getSubString(lastIndex, iterator).getContext(), iterator - lastIndex));
			lastIndex = iterator + 1;
		}
		iterator++;
	}
	if (iterator != lastIndex)
	{
		output.push_back(new MyString(this->getSubString(lastIndex, iterator).getContext(), iterator - lastIndex));
	}
	return output;
}

//std::vector<MyString*> MyString::getEachLine(MyString custom) {
//	std::vector<MyString*> output;
//	size_t lastIndex = 0, iterator = 0;
//	MyString* temp = new MyString("");
//	char cs[2] = { '\0','\0' };
//	while (iterator < this->getLength())
//	{
//		if (temp == nullptr)
//		{
//			temp = new MyString("");
//		}
//		if (this->getCharAt(iterator) == '(' || this->getCharAt(iterator) == ')' || this->getCharAt(iterator) == '^' || this->getCharAt(iterator) == '!' ||
//			this->getCharAt(iterator) == '|' || this->getCharAt(iterator) == '&' || this->getCharAt(iterator) == '>' || this->getCharAt(iterator) == '<')
//		{
//			cs[0] = this->getCharAt(iterator);
//			*temp = *temp + this->getSubString(lastIndex, iterator) + "^";
//			lastIndex = iterator;
//		}else if (this->getCharAt(iterator) == '\n')
//		{
//			*temp = *temp + this->getSubString(lastIndex, iterator);
//
//			output.push_back(temp);
//			lastIndex = iterator + 1;
//			delete temp;
//			temp = nullptr;
//		}
//		iterator++;
//	}
//	if (iterator != lastIndex)
//	{
//		*temp = *temp + this->getSubString(lastIndex, iterator);
//		output.push_back(temp);
//	}
//	return output;
//}
//std::vector<MyString*> MyString::getEachLine(MyString input)
//{
//	return input.getEachLine();
//}
//
//std::vector<MyString*> MyString::getEachLine(FILE* input)
//{
//	std::vector<MyString*> output;
//	char buffer[1024];
//
//	while (fgets(buffer, sizeof(buffer), input) != NULL) {
//		output.push_back(new MyString(buffer));
//	}
//	return std::vector<MyString*>();
//}

MyString MyString::getSubString(size_t sIndex, size_t fIndex)
{
	if (sIndex > fIndex) // handle index swap
	{
		size_t swap = sIndex;
		sIndex = fIndex;
		fIndex = swap;
	}
	if (sIndex > this->getLength())
	{
		return MyString("");
	}
	fIndex = this->getLength() < fIndex ? this->getLength() : fIndex;  //Preventing index out of bound
	MyString* output = new MyString(fIndex - sIndex);
	for (size_t i = sIndex; i < fIndex; i++)
	{
		output->setCharAt(i - sIndex, this->getCharAt(i));
	}
	return *output;
}

char* MyString::getContext() {
	return this->context;
}

size_t MyString::getLength() {
	return this->length;
}

char MyString::getCharAt(size_t index)
{
	return index < this->length ? this->context[index] : '\0';
}

bool MyString::setCharAt(size_t index, char c)
{
	//if (index < 0)
	//{
	//	index += this->length;
	//}
	if (index < this->length)
	{
		if (c == '\0') {
			char* temp = new char[index + 1];
			for (size_t i = 0; i < index; i++)
			{
				temp[i] = this->context[i];
			}
			temp[index] = '\0';
			delete[] this->context;
			this->context = temp;
			this->length = index;
			return true;
		}
		else {
			this->context[index] = c;
			return this->context[index] == c;
		}
	}
	else if (index == this->length && c == '\0')	return  true;

	return false;
}
