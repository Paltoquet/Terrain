#include "stdafx.h"
#include <Utils\StringHelper.h>
#include <stdlib.h>
#include <string>

using namespace::std;

wchar_t* StringHelper::getWChar(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];

	size_t outSize;
	mbstowcs_s(&outSize, wc, cSize, c, cSize-1);

	return wc;
}

bool StringHelper::endWith(const wchar_t* str, const wchar_t* ending)
{
	wstring ws1(str);
	wstring ws2(ending);
	string tmp(ws1.begin(), ws1.end());
	string end(ws2.begin(), ws2.end());
	if (end.size() > tmp.size()) {
		return false;
	}
	else {
		return std::equal(end.rbegin(), end.rend(), tmp.rbegin());
	}
}