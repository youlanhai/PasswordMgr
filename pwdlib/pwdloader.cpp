#include "pwdloader.h"
#include "Pwd.h"
#include "PwdStream.h"
#include "PwdMgr.h"

namespace pwd
{
    PwdLoader::PwdLoader()
    {

    }

    PwdLoader::~PwdLoader()
    {

    }

    bool PwdLoader::load(PwdMgr &mgr, PwdStream &stream)
    {
        stream.skip(16); //reserve 16 bytes
        if (stream.empty())
        {
            return false;
        }

        pwdid idCounter;
        stream.loadStruct<uint32_t>(idCounter);
        mgr.setIdCounter(idCounter);

        uint32_t len;
        stream.loadStruct<uint32_t>(len);

        Pwd data;
        for (uint32_t i = 0; i < len; ++i)
        {
            if(!loadPwd(data, stream))
            {
                return false;
            }
            mgr.insert(data);
        }
        return true;
    }

    bool PwdLoader::save(const PwdMgr &mgr, PwdStream &stream)
    {
        //reserve 16 bytes
        for (size_t i = 0; i < 16; ++i)
        {
            stream.append(0);
        }

        stream.saveStruct<uint32_t>(mgr.getIdCounter());
        stream.saveStruct<uint32_t>(mgr.count());

        for (const auto &pair : mgr)
        {
            if(!savePwd(pair.second, stream))
            {
                return false;
            }
        }
        return true;
    }
}
