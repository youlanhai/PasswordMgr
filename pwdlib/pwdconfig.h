#pragma once

#include <string>
#include <vector>

#define PWD_STR(STR) STR

namespace pwd
{
    typedef uint32_t    pwdid;
    typedef char        pwdchar;
    typedef std::string pwdstring;
    typedef std::vector<pwdid> IdVector;

    typedef unsigned char uchar;
    typedef std::vector<uchar> streambuffer;

    const pwdid MaxId = 0x7fffffff;
    const pwdstring EmptyStr = PWD_STR("");


    enum class LoaderError
    {
        NoError,
        FailedOpenFile,
        FailedReadFile,
        FailedWriteFile,
        UnsupportedVersion,
        InvalidData,
        EmptyPassword,
        InvalidPassword,
        FailedEncrypt,
        FailedDecrypt,
    };

}

#define PWD_RETURN(EXP, RET) if(!(EXP)) return RET;
