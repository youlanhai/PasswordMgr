#include "PwdMgr.h"

#include <cassert>

#include "pwdloader.h"
#include "pwdlog.h"

namespace pwd
{
    const uint32_t MagicNum = 'x' | 'p' << 8 | 'w' << 16 | 'd' << 24;
    const uint32_t PwdVersion = 1;

    
    uint32_t getVersion()
    {
        return PwdVersion;
    }

    static long flength(FILE* pFile)
    {
        assert(pFile != NULL);

        long cur = ftell(pFile);
        fseek(pFile, 0, SEEK_END);
        long length = ftell(pFile);
        fseek(pFile, cur, SEEK_SET);
        return length;
    }

    static bool matchstr(const pwdstring & src, const pwdstring & arg)
    {
        if(src.length() < arg.length()) return false;

        size_t maxMatch = src.length() - arg.length() + 1;
        for(size_t i = 0; i < maxMatch; ++i)
        {
            bool matched = true;

            for(size_t j = i, k = 0; k < arg.length(); ++j, ++k)
            {
                if(src[j] == arg[k])
                    continue;

                if(tolower(src[j]) == tolower(arg[k]))
                    continue;

                matched = false;
                break;
            }

            if(matched) return true;
        }

        return false;
    }


#define PWD_SEARCH_BY(VEC, KEY, ARG) \
	for (PwdMap::const_iterator it = pool_.begin(); it != pool_.end(); ++it)\
	{									    \
        if (matchstr(it->second.KEY, ARG))	\
			VEC.push_back(it->first);	    \
	}

    ////////////////////////////////////////////////////////////////////

	PwdMgr::PwdMgr()
		: idCounter_(10000)
	{
	}


	PwdMgr::~PwdMgr()
	{
	}

    void PwdMgr::clear()
    {
        pool_.clear();
        idCounter_ = 10000;
    }

	bool PwdMgr::load(PwdStream & stream)
	{
        clear();

        uint32_t magic, version;
		stream.loadStruct(magic);
        stream.loadStruct(version);

        if (magic != MagicNum)
        {
            PWD_LOG_ERROR("Invalid data format.");
            return false;
        }

        stream.skip(16); //reserve 16 bytes
        if (stream.empty())
        {
            return false;
        }

        PwdLoader *loader = createLoader(version);
        if(loader == nullptr)
        {
            PWD_LOG_ERROR("Unsupported file version '%u'", version);
            return false;
        }

		stream.loadStruct(idCounter_);

        uint32_t len;
		stream.loadStruct(len);

		Pwd data;
        bool ok = true;
		for (size_t i = 0; i < len; ++i)
		{
            if(!loader->load(data, stream))
            {
                ok = false;
                break;
            }
			pool_[data.id_] = data;
		}
        delete loader;

        return ok;
	}

	bool PwdMgr::save(PwdStream & stream) const
	{
        stream.saveStruct<uint32_t>(MagicNum);
        stream.saveStruct<uint32_t>(PwdVersion);
		
        //reserve 16 bytes
        for (size_t i = 0; i < 16; ++i)
        {
            stream.append(0);
        }

        stream.saveStruct<uint32_t>(idCounter_);
        stream.saveStruct<uint32_t>(pool_.size());
		
        PwdLoader *loader = createLoader(getVersion());
        if(!loader)
        {
            PWD_LOG_ERROR("Unsupported file version '%u'", getVersion());
            return false;
        }

        bool ok = true;
		for (PwdMap::const_iterator it = pool_.begin(); it != pool_.end(); ++it)
		{
            if(!loader->save(it->second, stream))
            {
                ok = false;
                break;
            }
		}
        delete loader;
        return ok;
	}

	pwdid PwdMgr::add(const Pwd & data)
	{
		pwdid id = allocateId();

		std::pair<pwdid, Pwd> temp(id, data);
		temp.second.id_ = id; //modify the id
		pool_.insert(temp);

        PWD_LOG_INFO("Add new data [%u]'%s'", id, data.keyword_.c_str());
		return id;
	}

	void PwdMgr::del(pwdid id)
	{
		PwdMap::iterator it = find(id);
		if (it == pool_.end())
        {
            PWD_LOG_ERROR("Delete failed, the pwdid [%u] was not found!", id);
            return;
        }

		pool_.erase(it);
	}

	void PwdMgr::modify(pwdid id, const Pwd & data)
	{
		PwdMap::iterator it = find(id);
		if (it == pool_.end())
        {
            PWD_LOG_ERROR("Modify failed, the pwdid [%u] was not found!", id);
            return;
        }

		it->second = data;
		it->second.id_ = id;//the id must not be changed.
        PWD_LOG_DEBUG("Modify [%u]'%s'", id, data.keyword_.c_str());
	}

    bool PwdMgr::exist(pwdid id) const
    {
        PwdMap::const_iterator it = find(id);
		return it != pool_.end();
    }

	const Pwd & PwdMgr::get(pwdid id) const
	{
		PwdMap::const_iterator it = find(id);
		if (it == pool_.end())
        {
            PWD_LOG_ERROR("the pwdid [%u] was not found!", id);

            static Pwd pwd;
            pwd.id_ = 0;
            return pwd;
        }

		return it->second;
	}

	void PwdMgr::searchByName(IdVector & ids, const pwdstring & name) const
	{
		PWD_SEARCH_BY(ids, name_, name)
	}

    void PwdMgr::searchByPwd(IdVector & ids, const pwdstring & pwd) const
    {
        PWD_SEARCH_BY(ids, pwd_, pwd);
    }

	void PwdMgr::searchByKeyword(IdVector & ids, const pwdstring & keyword) const
	{
		PWD_SEARCH_BY(ids, keyword_, keyword)
	}

	void PwdMgr::searchByDesc(IdVector & ids, const pwdstring & desc) const
	{
		PWD_SEARCH_BY(ids, desc_, desc)
	}

    
    bool PwdMgr::load(const std::string & fname)
    {
        FILE* pFile = fopen(fname.c_str(), "rb");
        if (!pFile)
        {
            PWD_LOG_ERROR("open file %s failed!", fname.c_str());
            return false;
        }

        pwd::PwdStream stream;
        pwd::streambuffer & buffer = stream.steam();

        long len = flength(pFile);
        buffer.resize(len);
        if (len <= 0 || fread(&buffer[0], len, 1, pFile) != 1)
        {
            PWD_LOG_ERROR("read file %s failed!", fname.c_str());
            fclose(pFile);
            return false;
        }
        fclose(pFile);

        if (!load(stream))
        {
            PWD_LOG_ERROR("load stream failed!");
            return false;
        }

        PWD_LOG_DEBUG("succed load pwd data %s", fname.c_str());
        return true;
    }

    bool PwdMgr::save(const std::string & fname) const
    {
        FILE* pFile = fopen(fname.c_str(), "wb");
        if (!pFile)
        {
            PWD_LOG_ERROR("open file %s failed!", fname.c_str());
            return false;
        }

        pwd::PwdStream stream;
        if (!save(stream))
        {
            PWD_LOG_ERROR("save stream failed!");
            fclose(pFile);
            return false;
        }

        pwd::streambuffer & buffer = stream.steam();
        size_t len = buffer.size();

        if (fwrite(buffer.data(), len, 1, pFile) != 1)
        {
            PWD_LOG_ERROR("write file failed!");
            fclose(pFile);
            return false;
        }
        fclose(pFile);

        PWD_LOG_DEBUG("succed save pwd data %s", fname.c_str());
        return true;
    }

}
