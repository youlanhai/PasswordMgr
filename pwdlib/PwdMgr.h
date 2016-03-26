#pragma once
#include "Pwd.h"
#include <map>

namespace pwd
{
	typedef std::map<pwdid, Pwd> PwdMap;

	class PwdMgr
	{
	public:
		PwdMgr();
		~PwdMgr();

        void clear();

        bool load(const std::string & fname);
        bool save(const std::string & fname) const;

		bool load(PwdStream & stream);
		bool save(PwdStream & stream) const;

		//add one password data. and return the id of the data.
		pwdid add(const Pwd & data);
		void del(pwdid id);
		void modify(pwdid id, const Pwd & data);
		
		const Pwd & get(pwdid id) const;
		void searchByName(IdVector & ids, const pwdstring & name) const;
        void searchByPwd(IdVector & ids, const pwdstring & pwd) const;
		void searchByKeyword(IdVector & ids, const pwdstring & keyword) const;
		void searchByDesc(IdVector & ids, const pwdstring & desc) const;
		
        bool exist(pwdid id) const;
		inline size_t count() const { return pool_.size(); }

        inline PwdMap::const_iterator begin() const { return pool_.begin(); }
        inline PwdMap::const_iterator end() const { return pool_.end(); }

        pwdid getIdCounter() const { return idCounter_; }
        void setIdCounter(pwdid counter){ idCounter_ = counter; }

        void insert(const Pwd &data);

        void setEncryptKey(const std::string &key){ encryptKey_ = key; }
        const std::string& getEncryptKey() const { return encryptKey_; }

    private:
        inline pwdid allocateId(){ return idCounter_++;  }
        inline PwdMap::iterator find(pwdid id){ return pool_.find(id); }
		inline PwdMap::const_iterator find(pwdid id) const { return pool_.find(id); }

	private:
        pwdid       idCounter_;
        PwdMap      pool_;
        std::string encryptKey_;
	};

    uint32_t getVersion();

}
