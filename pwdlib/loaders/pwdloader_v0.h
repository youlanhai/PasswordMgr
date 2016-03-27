#ifndef PWD_LOADER_V0_H
#define PWD_LOADER_V0_H

#include "../pwdloader.h"

namespace pwd
{
    class PwdLoaderV0 : public PwdLoader
    {
    public:
        virtual LoaderError loadPwd(Pwd &info, PwdStream &stream) override;
        virtual LoaderError savePwd(const Pwd &info, PwdStream &stream) override;
    };
}

#endif //PWD_LOADER_V0_H
