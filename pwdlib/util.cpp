#include "stdafx.h"
#include "util.h"
#include <cstdlib>

namespace pwd
{
    void disorder(pwdstring & pwd)
    {
        size_t n = pwd.size();
        if (n == 0) return;

        for (size_t i = 0; i < n - 1; ++i)
        {
            int k = i + rand() % (n - i);
            std::swap(pwd[i], pwd[k]);
        }
    }

    void randomChoice(pwdstring & out, size_t n, const pwdstring & source)
    {
        out.clear();
        if (n == 0) return;

        size_t srcLen = source.size();
        if (srcLen == 0) return;

        for (size_t i = 0; i < n; ++i)
        {
            out.push_back(source[rand() % srcLen]);
        }
    }

    void genPwd(pwdstring & out, size_t n, const pwdstring & srcMust, const pwdstring & srcDefault)
    {
        if (n == 0)
        {
            throw(std::invalid_argument("the num must not be 0!"));
        }
        if (srcMust.empty() && srcDefault.empty())
        {
            throw(std::invalid_argument("srcMust and srcDefault, at least one is not empty!"));
        }

        out.clear();

        if (srcMust.size() > n)
        {
            randomChoice(out, n, srcMust);
        }
        else if (srcMust.size() < n)
        {
            if (!srcDefault.empty())
                randomChoice(out, n - srcMust.size(), srcDefault);
            else
                randomChoice(out, n - srcMust.size(), srcMust);
            out += srcMust;
        }
        else
        {
            out = srcMust;
        }

        disorder(out);
    }

    void genPwdByTypeRandom(pwdstring & out, size_t n, ChType type, const pwdstring & srcMust)
    {
        if (n == 0)
        {
            throw(std::invalid_argument("the num must not be 0!"));
        }

        pwdstring source;

        if (type & CT_LetterS) source += LetterS;
        if (type & CT_LetterB) source += LetterB;
        if (type & CT_Digit) source += Digit;
        if (type & CT_Special) source += Special;

        if (source.empty() && srcMust.empty())
        {
            throw(std::invalid_argument("the type and srcMust, at least one must not be none!"));
        }

        disorder(source);
        genPwd(out, n, srcMust, source);
    }

    void genPwdByTypeStrict(pwdstring & out, size_t n, ChType type, const pwdstring & srcMust)
    {
        if (n == 0)
        {
            throw(std::invalid_argument("the num must not be 0!"));
        }

        pwdstring source;
        pwdstring temp;

        if (type & CT_LetterS)
        {
            randomChoice(temp, 1, LetterS);
            source += temp;
        }

        if (type & CT_LetterB)
        {
            randomChoice(temp, 1, LetterB);
            source += temp;
        }

        if (type & CT_Digit)
        {
            randomChoice(temp, 1, Digit);
            source += temp;
        }

        if (type & CT_Special)
        {
            randomChoice(temp, 1, Special);
            source += temp;
        }

        if (source.empty() && srcMust.empty())
        {
            throw(std::invalid_argument("the type and srcMust, at least one must not be none!"));
        }

        source += srcMust;//source与srcMust同级。
        disorder(source);

        if (source.size() > n)//srcMust太多，则从中随机选择。
        {
            randomChoice(out, n, source);
        }
        else if (source.size() < n)
        {
            genPwdByTypeRandom(out, n, type, source);
        }
        else
        {
            out = source;
        }
    }
}