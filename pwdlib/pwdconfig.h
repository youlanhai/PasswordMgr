#pragma once

#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <algorithm>

namespace pwd
{
	typedef size_t pwdid;
    typedef wchar_t pwdchar;
	typedef std::wstring pwdstring;
	typedef std::vector<pwdid> IdVector;

	typedef unsigned char uchar;
	typedef std::vector<uchar> streambuffer;

	const pwdid MaxId = 0x7fffffff;
    const pwdstring EmptyStr = L"";
}