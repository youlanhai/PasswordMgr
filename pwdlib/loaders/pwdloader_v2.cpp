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

    bool PwdLoaderV2::load(PwdMgr &mgr, PwdStream &stream)
    {
        // read the header
        size_t headerLength = stream.loadStruct<uint16_t>();
        bool isEncrypt = stream.read() != 0;
        size_t dataLength = stream.loadStruct<uint32_t>();

        stream.skip(headerLength - (2 + 1 + 4));
        if (stream.remain() < dataLength)
        {
            PWD_LOG_ERROR("Invalid data length.");
            return false;
        }

        PwdStream ss;
        streambuffer &buffer = ss.steam();
        buffer.resize(dataLength);
        if(!stream.read(buffer.data(), dataLength))
        {
            return false;
        }
        assert(stream.empty());

        if(isEncrypt && !decryptData(buffer, mgr.getEncryptKey()))
        {
            PWD_LOG_ERROR("Failed to init password key.");
            return false;
        }

        uint32_t magic = ss.loadStruct<uint32_t>();
        if(magic != EncryptMagic)
        {
            PWD_LOG_ERROR("Failed to decrypt data.");
            return false;
        }

        pwdid idCounter = ss.loadStruct<uint32_t>();
        mgr.setIdCounter(idCounter);

        uint32_t len = ss.loadStruct<uint32_t>();

        Pwd data;
        for (uint32_t i = 0; i < len; ++i)
        {
            if(!loadPwd(data, ss))
            {
                PWD_LOG_ERROR("Failed to load password info at index %d", (int)i);
                return false;
            }
            mgr.insert(data);
        }
        return true;
    }

    bool PwdLoaderV2::save(const PwdMgr &mgr, PwdStream &stream)
    {
        // collect raw data.
        PwdStream ss;
        ss.saveStruct<uint32_t>(EncryptMagic);
        ss.saveStruct<uint32_t>(mgr.getIdCounter());
        ss.saveStruct<uint32_t>(mgr.count());
        for (const auto &pair : mgr)
        {
            if(!savePwd(pair.second, ss))
            {
                return false;
            }
        }

        if(enableEncrypt_)
        {
            streambuffer &buffer = ss.steam();
            buffer.resize(ss.offset());

            if(!encryptData(buffer, mgr.getEncryptKey()))
            {
                PWD_LOG_ERROR("Failed to encrypt data.");
                return false;
            }
        }

        // write header
        size_t headerLength = 16;
        stream.saveStruct<uint16_t>(headerLength);
        stream.saveStruct<uchar>(enableEncrypt_);
        stream.saveStruct<uint32_t>(ss.offset());
        for(size_t i = 2 + 1 + 4; i < headerLength; ++i)
        {
            stream.append(0);
        }

        // write data
        stream.append(ss.begin(), ss.offset());
        return true;
    }

}
