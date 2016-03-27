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

    LoaderError PwdLoader::load(PwdMgr &mgr, PwdStream &stream)
    {
        stream.skip(16); //reserve 16 bytes
        if (stream.empty())
        {
            return LoaderError::InvalidData;
        }

        pwdid idCounter;
        stream.loadStruct<uint32_t>(idCounter);
        mgr.setIdCounter(idCounter);

        uint32_t len;
        stream.loadStruct<uint32_t>(len);

        Pwd data;
        for (uint32_t i = 0; i < len; ++i)
        {
            LoaderError ret = loadPwd(data, stream);
            if(ret != LoaderError::NoError)
            {
                return ret;
            }
            mgr.insert(data);
        }
        return LoaderError::NoError;
    }

    LoaderError PwdLoader::save(const PwdMgr &mgr, PwdStream &stream)
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
            LoaderError ret = savePwd(pair.second, stream);
            if(ret != LoaderError::NoError)
            {
                return ret;
            }
        }
        return LoaderError::NoError;
    }
}
