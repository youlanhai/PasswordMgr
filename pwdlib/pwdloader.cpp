#include "pwdloader.h"
#include "Pwd.h"
#include "PwdStream.h"

#include <iconv.h>

#define PWD_RETURN_FALSE(RET) if(!(RET)) return false

namespace pwd
{
    typedef std::basic_string<int16_t> utf16;
    bool utf16_to_utf8(std::string &output, const utf16 &input)
    {
        if(input.empty())
        {
            output.clear();
            return true;
        }

        char *pInput = (char*)input.c_str();
        size_t inputLeft = input.size() * sizeof(utf16::value_type);

        size_t outputLeft = input.size() * 4;
        output.resize(outputLeft);
        char *pOutput = (char*)output.data();

        iconv_t cd = iconv_open("utf-8", "utf-16le");
        if(cd == iconv_t(-1))
        {
            return false;
        }

        if(iconv(cd, &pInput, &inputLeft, &pOutput, &outputLeft) == size_t(-1))
        {
            output.clear();
            iconv_close(cd);
            return false;
        }

        output.resize(output.size() - outputLeft);
        iconv_close(cd);
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


    PwdLoader::PwdLoader()
    {

    }

    PwdLoader::~PwdLoader()
    {

    }


    bool PwdLoaderV0::load(Pwd &info, PwdStream &stream)
    {
        PWD_RETURN_FALSE(stream.loadStruct<uint32_t>(info.id_));

        utf16 keyword, name, pwd, desc;
        PWD_RETURN_FALSE(read_utf16(keyword, stream));
        PWD_RETURN_FALSE(read_utf16(name, stream));
        PWD_RETURN_FALSE(read_utf16(pwd, stream));
        PWD_RETURN_FALSE(read_utf16(desc, stream));

        PWD_RETURN_FALSE(utf16_to_utf8(info.keyword_, keyword));
        PWD_RETURN_FALSE(utf16_to_utf8(info.name_, name));
        PWD_RETURN_FALSE(utf16_to_utf8(info.pwd_, pwd));
        PWD_RETURN_FALSE(utf16_to_utf8(info.desc_, desc));
        return true;
    }

    bool PwdLoaderV0::save(const Pwd &info, PwdStream &stream)
    {
        return false;
    }


    bool PwdLoaderV1::load(Pwd &info, PwdStream &stream)
    {
        PWD_RETURN_FALSE(stream.loadStruct<uint32_t>(info.id_));
        PWD_RETURN_FALSE(stream.loadString(info.keyword_));
        PWD_RETURN_FALSE(stream.loadString(info.name_));
        PWD_RETURN_FALSE(stream.loadString(info.pwd_));
        PWD_RETURN_FALSE(stream.loadString(info.desc_));
        return true;
    }

    bool PwdLoaderV1::save(const Pwd &info, PwdStream &stream)
    {
        stream.saveStruct<uint32_t>(info.id_);
        stream.saveString(info.keyword_);
        stream.saveString(info.name_);
        stream.saveString(info.pwd_);
        stream.saveString(info.desc_);
        return true;
    }

    PwdLoader* createLoader(uint32_t version)
    {
        if(version == 0)
        {
            return new PwdLoaderV0();
        }
        else if(version == 1)
        {
            return new PwdLoaderV1();
        }
        return NULL;
    }
}
