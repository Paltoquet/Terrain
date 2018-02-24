#pragma once

#include <string>

class StringHelper
{
public:
	static wchar_t* getWChar(const char* str);
	static bool endWith(const wchar_t* str, const wchar_t* ending);
	static bool ends_with(std::string const & value, std::string const & ending);
};