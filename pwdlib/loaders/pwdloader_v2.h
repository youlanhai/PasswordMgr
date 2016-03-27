#ifndef PWD_LOADER_V2_H
#define PWD_LOADER_V2_H

#include "pwdloader_v1.h"

namespace pwd
{
    class PwdLoaderV2 : public PwdLoaderV1
    {
        bool    enableEncrypt_;
    public:
        explicit PwdLoaderV2(bool enableEncrypt);

        virtual bool load(PwdMgr &mgr, PwdStream &stream) override;
        virtual bool save(const PwdMgr &mgr, PwdStream &stream) override;
    };
}

#endif //PWD_LOADER_V2_H
