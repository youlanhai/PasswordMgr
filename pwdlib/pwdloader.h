#ifndef PWDLOADER_H
#define PWDLOADER_H

#include "pwdconfig.h"

namespace pwd
{
    class Pwd;
    class PwdMgr;
    class PwdStream;

    class PwdLoader
    {
    public:
        PwdLoader();
        virtual ~PwdLoader();

        virtual LoaderError load(PwdMgr &mgr, PwdStream &stream);
        virtual LoaderError save(const PwdMgr &mgr, PwdStream &stream);

        virtual LoaderError loadPwd(Pwd &info, PwdStream &stream) = 0;
        virtual LoaderError savePwd(const Pwd &info, PwdStream &stream) = 0;
    };

    PwdLoader* createLoader(uint32_t version);
}

#endif // PWDLOADER_H
