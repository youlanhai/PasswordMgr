#include "pwdloader.h"
#include "Pwd.h"
#include "PwdStream.h"
#include "PwdMgr.h"
#include "pwdlog.h"
#include "util.h"

#include <cassert>
#include <iconv.h>

#define PWD_RETURN_FALSE(RET) if(!(RET)) return false


namespace pwd
{
    static const uint32_t EncryptMagic = 193578324;

    bool read_utf16(utf16 &output, PwdStream &stream)
    {
        uint32_t size;
        if(!stream.loadStruct(size) || size > stream.remain())
        {
            return false;
        }

        output.resize(size);
        if(size > 0)
        {
            return stream.read(&output[0], size * sizeof(utf16::value_type));
        }
        return true;
    }


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

    bool PwdLoaderV0::loadPwd(Pwd &info, PwdStream &stream)
    {
        PWD_RETURN_FALSE(stream.loadStruct<uint32_t>(info.id_));

        utf16 keyword, name, pwd, desc;
        PWD_RETURN_FALSE(read_utf16(keyword, stream));
        PWD_RETURN_FALSE(read_utf16(name, stream));
        PWD_RETURN_FALSE(read_utf16(pwd, stream));
        PWD_RETURN_FALSE(read_utf16(desc, stream));

        PWD_RETURN_FALSE(utf16_to_utf8(info.keyword_, keyword));
        PWD_RETURN_FALSE(utf16_to_utf8(info.name_, name));
        PWD_RETURN_FALSE(utf16_to_utf8(info.pwd_, pwd));
        PWD_RETURN_FALSE(utf16_to_utf8(info.desc_, desc));
        return true;
    }

    bool PwdLoaderV0::savePwd(const Pwd & /*info*/, PwdStream & /*stream*/)
    {
        return false;
    }


    bool PwdLoaderV1::loadPwd(Pwd &info, PwdStream &stream)
    {
        PWD_RETURN_FALSE(stream.loadStruct<uint32_t>(info.id_));
        PWD_RETURN_FALSE(stream.loadString(info.keyword_));
        PWD_RETURN_FALSE(stream.loadString(info.name_));
        PWD_RETURN_FALSE(stream.loadString(info.pwd_));
        PWD_RETURN_FALSE(stream.loadString(info.desc_));
        return true;
    }

    bool PwdLoaderV1::savePwd(const Pwd &info, PwdStream &stream)
    {
        stream.saveStruct<uint32_t>(info.id_);
        stream.saveString(info.keyword_);
        stream.saveString(info.name_);
        stream.saveString(info.pwd_);
        stream.saveString(info.desc_);
        return true;
    }

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
