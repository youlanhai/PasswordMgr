#include "stdafx.h"
#include "PwdMgr.h"
#include <iostream>

namespace pwd
{
	const size_t MagicNum = 'x' | 'p' << 8 | 'w' << 16 | 'd' << 24;
	const size_t PwdVersion = 0;

    
    size_t getVersion()
    {
        return PwdVersion;
    }

    
    static FILE* openFile(const std::wstring & fname, const std::wstring & mode)
    {
        FILE * pFile = NULL;
        _wfopen_s(&pFile, fname.c_str(), mode.c_str());

        return pFile;
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

		size_t magic, version;
		stream.loadStruct(magic);
		if (magic != MagicNum)
		{
			return false;
		}

		stream.loadStruct(version);
		if (version != PwdVersion)
		{
			return false;
		}

		stream.skip(16); //reserve 16 bytes
		if (stream.empty())
		{
			return false;
		}

		stream.loadStruct(idCounter_);

		size_t len;
		stream.loadStruct(len);

		Pwd data;
		for (size_t i = 0; i < len; ++i)
		{
			data.load(stream);
			pool_[data.id_] = data;
		}

		stream.loadStruct(magic);
		if (magic != MagicNum)
		{
            clear();
			return false;
		}

		return true;
	}

	bool PwdMgr::save(PwdStream & stream) const
	{
		stream.saveStruct(MagicNum);
		stream.saveStruct(PwdVersion);
		
        //reserve 16 bytes
        for (size_t i = 0; i < 16; ++i)
        {
            stream.append(0);
        }

		stream.saveStruct(idCounter_);
		stream.saveStruct(pool_.size());
		
		for (PwdMap::const_iterator it = pool_.begin(); it != pool_.end(); ++it)
		{
			it->second.save(stream);
		}

		stream.saveStruct(MagicNum);

		return true;
	}

	pwdid PwdMgr::add(const Pwd & data)
	{
		pwdid id = allocateId();

		std::pair<pwdid, Pwd> temp(id, data);
		temp.second.id_ = id; //modify the id
		pool_.insert(temp);

		return id;
	}

	void PwdMgr::del(pwdid id)
	{
		PwdMap::iterator it = find(id);
		if (it == pool_.end())
			throw(std::runtime_error("the pwdid was not found!"));

		pool_.erase(it);
	}
	void PwdMgr::modify(pwdid id, const Pwd & data)
	{
		PwdMap::iterator it = find(id);
		if (it == pool_.end())
			throw(std::runtime_error("the pwdid was not found!"));

		it->second = data;
		it->second.id_ = id;//the id must not be changed.
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
			throw(std::runtime_error("the pwdid was not found!"));

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

    
    bool PwdMgr::load(const std::wstring & fname)
    {
        FILE* pFile = openFile(fname, L"rb");
        if (!pFile)
        {
            std::wcout << L"open file " << fname << " failed!" << std::endl;
            return false;
        }

        pwd::PwdStream stream;
        pwd::streambuffer & buffer = stream.steam();

        long len = flength(pFile);
        buffer.resize(len);
        if (fread(&buffer[0], len, 1, pFile) != 1)
        {
            std::wcout << L"read file failed!" << std::endl;
            fclose(pFile);
            return false;
        }
        fclose(pFile);

        if (!load(stream))
        {
            std::wcout << L"load stream failed!" << std::endl;

            return false;
        }

        std::wcout << L"succed load pwd data '" << fname << "'." << std::endl;
        return true;
    }

    bool PwdMgr::save(const std::wstring & fname) const
    {
        FILE* pFile = openFile(fname, L"wb");
        if (!pFile)
        {
            std::wcout << L"open file " << fname << " failed!" << std::endl;
            return false;
        }

        pwd::PwdStream stream;
        if (!save(stream))
        {
            std::wcout << L"save stream failed!" << std::endl;
            fclose(pFile);
            return false;
        }

        pwd::streambuffer & buffer = stream.steam();
        size_t len = buffer.size();

        if (fwrite(&buffer[0], len, 1, pFile) != 1)
        {
            std::wcout << L"write file failed!" << std::endl;
            fclose(pFile);
            return false;
        }
        fclose(pFile);

        std::wcout << L"succed save pwd data '" << fname << "'." << std::endl;
        return true;
    }


}