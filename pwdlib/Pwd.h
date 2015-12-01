#pragma once

#include "PwdStream.h"

namespace pwd
{
	class Pwd
	{
	public:
		Pwd();
		~Pwd();

		void load(PwdStream & stream);
		void save(PwdStream & stream) const;

		pwdid id_;
		pwdstring keyword_;
		pwdstring name_;
		pwdstring pwd_;
		pwdstring desc_;
	};

}