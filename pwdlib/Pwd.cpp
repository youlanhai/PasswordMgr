#include "Pwd.h"

namespace pwd
{
	Pwd::Pwd()
		: id_(0)
	{
	}


	Pwd::~Pwd()
	{
	}

	void Pwd::load(PwdStream & stream)
	{
		stream.loadStruct(id_);
		stream.loadString(keyword_);
		stream.loadString(name_);
		stream.loadString(pwd_);
		stream.loadString(desc_);
	}

	void Pwd::save(PwdStream & stream) const
	{
		stream.saveStruct(id_);
		stream.saveString(keyword_);
		stream.saveString(name_);
		stream.saveString(pwd_);
		stream.saveString(desc_);
	}
}
