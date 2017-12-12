#include "stdafx.h"
#include <Utils\StringHelper.h>
#include <stdlib.h>

wchar_t* StringHelper::getWChar(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];

	size_t outSize;
	mbstowcs_s(&outSize, wc, cSize, c, cSize-1);

	return wc;
}