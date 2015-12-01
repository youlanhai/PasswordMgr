// pwdgen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../PwdLib/util.h"

const std::wstring HelpText = L"genterate password randomly.\n\n"
    L"pwdgen length [/m][must string][[/all]|[/s][/b][/d][/p]][/f]\n\n"
    L"length: the length of the password.\n"
    L"/m: the password must contain this string.\n"
    L"/s: use small letter.\n"
    L"/b: use big letter.\n"
    L"/d: use digital.\n"
    L"/p: use special character\n"
    L"/all: use both /s /b /d /p,this flag is default.\n"
    L"/f: random freely.";

bool existFlag(int argc, _TCHAR* argv [], _TCHAR* flag)
{
    for (int i = 1; i < argc; ++i)
    {
        if (_wcsicmp(argv[i], flag) == 0) return true;
    }

    return false;
}

int findValue(int argc, _TCHAR* argv [], _TCHAR* flag)
{
    for (int i = 1; i < argc; ++i)
    {
        if (_wcsicmp(argv[i], flag) == 0)
        {
            if (i + 1 < argc) return i + 1;
            else return -1;
        }
    }

    return -1;
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc <= 1)
    {
        std::wcout << HelpText << std::endl;
        return 0;
    }

    int length = _wtoi(argv[1]);
    if (length <= 0)
    {
        std::wcout << L"length must not ben 0!" << std::endl;
        return 0;
    }

    std::wstring strMust;
    int pos = findValue(argc, argv, L"/m");
    if (pos > 0)
    {
        strMust = argv[pos];
        if (strMust.empty())
        {
            std::wcout << L"the 'must string' must not ben null!" << std::endl;
            return 0;
        }
    }

    int type = pwd::CT_None;
    if (existFlag(argc, argv, L"/all"))
        type |= pwd::CT_FORCE_WORD;
    else
    {
        if (existFlag(argc, argv, L"/s")) type |= pwd::CT_LetterS;
        if (existFlag(argc, argv, L"/b")) type |= pwd::CT_LetterB;
        if (existFlag(argc, argv, L"/d")) type |= pwd::CT_Digit;
        if (existFlag(argc, argv, L"/p")) type |= pwd::CT_Special;

        if (strMust.empty() && type == pwd::CT_None)
        {
            type = pwd::CT_FORCE_WORD;
        }
    }

    srand(time(NULL));

    std::wstring password;

    try
    {
        if (existFlag(argc, argv, L"/f"))
            pwd::genPwdByTypeRandom(password, length, (pwd::ChType)type, strMust);
        else
            pwd::genPwdByTypeStrict(password, length, (pwd::ChType)type, strMust);
    }
    catch (std::exception e)
    {
        std::wcout << L"error:" << e.what() << std::endl;
        return 0;
    }

    std::wcout << L"pwd:" << password << std::endl;

	return 0;
}

