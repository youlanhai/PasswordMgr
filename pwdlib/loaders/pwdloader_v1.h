#ifndef PWD_LOADER_V1_H
#define PWD_LOADER_V1_H

#include "../pwdloader.h"

namespace pwd
{

    class PwdLoaderV1 : public PwdLoader
    {
    public:
        virtual bool loadPwd(Pwd &info, PwdStream &stream) override;
        virtual bool savePwd(const Pwd &info, PwdStream &stream) override;
    };
}
#endif //PWD_LOADER_V1_H
