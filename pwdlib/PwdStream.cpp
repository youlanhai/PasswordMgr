#include "stdafx.h"
#include "PwdStream.h"

namespace pwd
{
	const size_t BasicSize = sizeof(void*) ;
	const size_t LimitSize = 1024 * 1024 * 1024;

	PwdStream::PwdStream()
		: pos_(0)
		, stream_(BasicSize)
	{
	}


	PwdStream::~PwdStream()
	{
	}

	uchar PwdStream::get()
	{
		if (empty()) return 0;
		return stream_[pos_++];
	}

	void PwdStream::append(uchar ch)
	{
		if (empty()) stream_.resize(pos_ + BasicSize);
		stream_[pos_++] = ch;
	}

	void PwdStream::gets(void *dest, size_t len)
	{
		if (len > LimitSize)
			throw(std::invalid_argument("the size is too large!"));

		if (stream_.size() - pos_ < len)
			throw(std::runtime_error("does't has enough stream!"));

		memcpy(dest, &stream_[pos_], len);
		pos_ += len;
	}

	void PwdStream::appends(const void *src, size_t len)
	{
		if (len > LimitSize)
			throw(std::invalid_argument("the size is too large!"));

		if (stream_.size() - pos_ < len)
			stream_.resize(pos_ + len + BasicSize);
		memcpy(&stream_[pos_], src, len);
		pos_ += len;
	}

	void PwdStream::skip(size_t len)
	{
		pos_ += len;

		if (pos_ >= stream_.size())
		{
			pos_ = stream_.size();
		}
	}
}