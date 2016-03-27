#include "pwdloader_v0.h"
#include "pwdloader_v1.h"
#include "pwdloader_v2.h"

namespace pwd
{
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
        else if(version == 2)
        {
            return new PwdLoaderV2(true);
        }
        return nullptr;
    }
}
