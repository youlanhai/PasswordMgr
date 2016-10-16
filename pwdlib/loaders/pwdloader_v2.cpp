#include "pwdloader_v2.h"
#include "../util.h"
#include "../Pwd.h"
#include "../PwdStream.h"
#include "../PwdMgr.h"
#include "../pwdlog.h"
#include <cassert>

namespace pwd
{

    static const uint32_t EncryptMagic = 193578324;

    PwdLoaderV2::PwdLoaderV2(bool enableEncrypt)
        : enableEncrypt_(enableEncrypt)
    {

    }

    LoaderError PwdLoaderV2::load(PwdMgr &mgr, PwdStream &stream)
    {
        // read the header
        size_t headerLength = stream.loadStruct<uint16_t>();
        bool isEncrypt = stream.read() != 0;
        size_t dataLength = stream.loadStruct<uint32_t>();

        stream.skip(headerLength - (2 + 1 + 4));
        if (stream.remain() < dataLength)
        {
            return LoaderError::InvalidData;
        }

        if(isEncrypt && mgr.getEncryptKey().empty())
        {
            return LoaderError::EmptyPassword;
        }

        PwdStream ss;
        streambuffer &buffer = ss.steam();
        buffer.resize(dataLength);
        if(!stream.read(buffer.data(), dataLength))
        {
            return LoaderError::InvalidData;
        }
        assert(stream.empty());

        if(isEncrypt && !decryptData(buffer, mgr.getEncryptKey()))
        {
            return LoaderError::InvalidData;
        }

        uint32_t magic = ss.loadStruct<uint32_t>();
        if(magic != EncryptMagic)
        {
            return LoaderError::InvalidPassword;
        }

        pwdid idCounter = ss.loadStruct<uint32_t>();
        mgr.setIdCounter(idCounter);

        uint32_t len = ss.loadStruct<uint32_t>();

        for (uint32_t i = 0; i < len; ++i)
        {
			Pwd data;
            LoaderError ret = loadPwd(data, ss);
            if(ret != LoaderError::NoError)
            {
                return ret;
            }
            mgr.insert(data);
        }
        return LoaderError::NoError;
    }

    LoaderError PwdLoaderV2::save(const PwdMgr &mgr, PwdStream &stream)
    {
        // collect raw data.
        PwdStream ss;
        ss.saveStruct<uint32_t>(EncryptMagic);
        ss.saveStruct<uint32_t>(mgr.getIdCounter());
        ss.saveStruct<uint32_t>(mgr.count());
        for (const auto &pair : mgr)
        {
            LoaderError ret = savePwd(pair.second, ss);
            if(ret != LoaderError::NoError)
            {
                return ret;
            }
        }

		streambuffer buffer = ss.steam();
		buffer.resize(ss.offset());

        if(enableEncrypt_)
        {
            if(mgr.getEncryptKey().empty())
            {
                return LoaderError::EmptyPassword;
            }

            if(!encryptData(buffer, mgr.getEncryptKey()))
            {
                return LoaderError::FailedEncrypt;
            }
        }

        // write header
        size_t headerLength = 16;
        stream.saveStruct<uint16_t>(headerLength);
        stream.saveStruct<uchar>(enableEncrypt_);
        stream.saveStruct<uint32_t>(buffer.size());
        for(size_t i = 2 + 1 + 4; i < headerLength; ++i)
        {
            stream.append(0);
        }

        // write data
        stream.append(buffer.data(), buffer.size());
        return LoaderError::NoError;
    }

}
