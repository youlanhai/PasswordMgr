#ifndef PWD_LOADER_V0_H
#define PWD_LOADER_V0_H

#include "../pwdloader.h"

namespace pwd
{
    class PwdLoaderV0 : public PwdLoader
    {
    public:
        virtual bool loadPwd(Pwd &info, PwdStream &stream) override;
        virtual bool savePwd(const Pwd &info, PwdStream &stream) override;
    };
}

#endif //PWD_LOADER_V0_H
