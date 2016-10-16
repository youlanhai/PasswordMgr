#include "pwdloader_v0.h"
#include "../util.h"
#include "../Pwd.h"
#include "../PwdStream.h"

#include <codecvt>
#include <locale>
#include <string>

#define PWD_RETURN_FAILED(EXP) PWD_RETURN(EXP, LoaderError::InvalidData)

namespace pwd
{
    typedef std::u16string utf16;
    bool utf16_to_utf8(std::string &output, const utf16 &input)
    {
#if _MSC_VER == 1900
        std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> converter;
        const int16_t *p = reinterpret_cast<const int16_t*>(input.c_str());
        output = converter.to_bytes(p, p + input.size());
#else
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        output = converter.to_bytes(input);
#endif
        return true;
    }

    bool read_utf16(utf16 &output, PwdStream &stream)
    {
        uint32_t size;
        if(!stream.loadStruct(size) || size > stream.remain())
        {
            return false;
        }

        output.resize(size);
        if(size > 0)
        {
            return stream.read(&output[0], size * sizeof(utf16::value_type));
        }
        return true;
    }

    LoaderError PwdLoaderV0::loadPwd(Pwd &info, PwdStream &stream)
    {
        PWD_RETURN_FAILED(stream.loadStruct<uint32_t>(info.id_));

        utf16 keyword, name, pwd, desc;
        PWD_RETURN_FAILED(read_utf16(keyword, stream));
        PWD_RETURN_FAILED(read_utf16(name, stream));
        PWD_RETURN_FAILED(read_utf16(pwd, stream));
        PWD_RETURN_FAILED(read_utf16(desc, stream));

        PWD_RETURN_FAILED(utf16_to_utf8(info.keyword_, keyword));
        PWD_RETURN_FAILED(utf16_to_utf8(info.name_, name));
        PWD_RETURN_FAILED(utf16_to_utf8(info.pwd_, pwd));
        PWD_RETURN_FAILED(utf16_to_utf8(info.desc_, desc));
        return LoaderError::NoError;
    }

    LoaderError PwdLoaderV0::savePwd(const Pwd & /*info*/, PwdStream & /*stream*/)
    {
        return LoaderError::UnsupportedVersion;
    }
}
