#include "PwdStream.h"

namespace pwd
{
	const size_t BasicSize = sizeof(void*) ;
	const size_t LimitSize = 1024 * 1024 * 1024;

	PwdStream::PwdStream()
        : stream_(BasicSize)
        , pos_(0)
	{
	}


	PwdStream::~PwdStream()
	{
	}

    uchar PwdStream::read()
	{
        if (empty())
        {
            return 0;
        }
		return stream_[pos_++];
	}

    bool PwdStream::read(void *dest, size_t len)
    {
        if (pos_ + len > stream_.size())
        {
            pos_ = stream_.size();
            return false;
        }

        memcpy(dest, &stream_[pos_], len);
        pos_ += len;
        return true;
    }

	void PwdStream::append(uchar ch)
	{
        if (empty())
        {
            stream_.resize(pos_ + BasicSize);
        }
		stream_[pos_++] = ch;
	}

    void PwdStream::append(const void *src, size_t len)
	{
		if (len > LimitSize)
        {
            //throw(std::invalid_argument("the size is too large!"));
            return;
        }

		if (stream_.size() - pos_ < len)
        {
			stream_.resize(pos_ + len + BasicSize);
        }
		memcpy(&stream_[pos_], src, len);
		pos_ += len;
	}

    bool PwdStream::skip(size_t len)
	{
		pos_ += len;

		if (pos_ >= stream_.size())
		{
			pos_ = stream_.size();
            return false;
		}
        return true;
	}

    bool PwdStream::loadString(std::string & str)
    {
        uint16_t len;
        if(!loadStruct(len) || len > remain())
        {
            return false;
        }

        str.resize(len);
        if (len > 0)
        {
            return read(&str[0], len);
        }
        return true;
    }

    void PwdStream::saveString(const std::string &str)
    {
        uint16_t len = str.length();
        saveStruct(len);
        if (len > 0)
        {
            append(str.c_str(), len);
        }
    }
}
