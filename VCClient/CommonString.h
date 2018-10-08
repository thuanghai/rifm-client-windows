#pragma once
#include <string>
#include <Windows.h>

class CommonString
{
public:
	CommonString();
	~CommonString();

	static std::wstring ANSIToUnicode(const std::string &str);
	static std::string UnicodeToANSI(const std::wstring &str);

	static std::wstring UTF8ToUnicode(const std::string &str);
	static std::string UnicodeToUTF8(const std::wstring &str);
};

