#pragma once

class StringHelper
{
public:
	static wchar_t* getWChar(const char* str);
	static bool endWith(const wchar_t* str, const wchar_t* ending);
};