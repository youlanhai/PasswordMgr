#include "pwdloader_v1.h"
#include "../Pwd.h"
#include "../PwdStream.h"

namespace pwd
{

    bool PwdLoaderV1::loadPwd(Pwd &info, PwdStream &stream)
    {
        PWD_RETURN_FALSE(stream.loadStruct<uint32_t>(info.id_));
        PWD_RETURN_FALSE(stream.loadString(info.keyword_));
        PWD_RETURN_FALSE(stream.loadString(info.name_));
        PWD_RETURN_FALSE(stream.loadString(info.pwd_));
        PWD_RETURN_FALSE(stream.loadString(info.desc_));
        return true;
    }

    bool PwdLoaderV1::savePwd(const Pwd &info, PwdStream &stream)
    {
        stream.saveStruct<uint32_t>(info.id_);
        stream.saveString(info.keyword_);
        stream.saveString(info.name_);
        stream.saveString(info.pwd_);
        stream.saveString(info.desc_);
        return true;
    }
}
