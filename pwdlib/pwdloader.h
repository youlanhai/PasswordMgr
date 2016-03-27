#ifndef PWDLOADER_H
#define PWDLOADER_H

#include <cstdint>

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

        virtual bool load(PwdMgr &mgr, PwdStream &stream);
        virtual bool save(const PwdMgr &mgr, PwdStream &stream);

        virtual bool loadPwd(Pwd &info, PwdStream &stream) = 0;
        virtual bool savePwd(const Pwd &info, PwdStream &stream) = 0;
    };




    PwdLoader* createLoader(uint32_t version);
}

#endif // PWDLOADER_H
