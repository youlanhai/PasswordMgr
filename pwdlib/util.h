#pragma once

#include "pwdconfig.h"

namespace pwd
{
    const pwdstring LetterS = "abcdefghijklmnopqrstuvwxyz";
    const pwdstring LetterB = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const pwdstring Digit = "0123456789";
    const pwdstring Special = "!@#$%^&*_";

    enum ChType
    {
        CT_None    = 0,
        CT_LetterS = 1,
        CT_LetterB = 2,
        CT_Digit   = 4,
        CT_Special = 8,
        CT_FORCE_WORD = 0x7fffff
    };

    //乱序排列字符串
    void disorder(pwdstring & pwd);

    //从source中随机抽取出n个字符
    void randomChoice(pwdstring & out, size_t n, const pwdstring & source);

    //生成n位随机密码。
    //source首选srcMust，如果srcMust位数不足，则从srcDefault中随机选择.
    //注意：srcMust和srcDefault不能同时为空。
    void genPwd(pwdstring & out, size_t n, const pwdstring & srcMust, const pwdstring & srcDefault);

    //根据类型，完全自由的生成密码。
    void genPwdByTypeRandom(pwdstring & out, size_t n, ChType type, const pwdstring & srcMust = EmptyStr);

    //根据类型，有约束的生成n位密码，保证每种类型至少存在一个字符。
    void genPwdByTypeStrict(pwdstring & out, size_t n, ChType type, const pwdstring & srcMust = EmptyStr);

    bool encryptData(streambuffer &inoutBuffer, const std::string &password);
    bool decryptData(streambuffer &inoutBuffer, const std::string &password);

    void test_util();

}//end namespace pwd
