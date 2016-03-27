#include "pwdloader_v0.h"
#include "../util.h"
#include "../Pwd.h"
#include "../PwdStream.h"

namespace pwd
{
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

    bool PwdLoaderV0::loadPwd(Pwd &info, PwdStream &stream)
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

    bool PwdLoaderV0::savePwd(const Pwd & /*info*/, PwdStream & /*stream*/)
    {
        return false;
    }
}
