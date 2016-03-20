#pragma once

#include "pwdconfig.h"

namespace pwd
{
	
	class PwdStream
	{
	public:
		PwdStream();
		~PwdStream();

        uchar read();
        bool read(void *dest, size_t len);

		void append(uchar ch);
        void append(const void *src, size_t leng);
        bool skip(size_t len);

        bool empty() const { return pos_ >= stream_.size(); }

        size_t remain() const { return stream_.size() - pos_; }
		
        streambuffer & steam(){ return stream_; }

	public:

		template<class T>
        bool loadStruct(T & t)
		{
            return read(&t, sizeof(t));
		}

		template<class T>
		T loadStruct()
		{
			T t;
            read(&t, sizeof(t));
			return t;
		}

		template<class T>
		void saveStruct(const T & t)
		{
            append(&t, sizeof(t));
		}

        bool loadString(std::string & str);
        void saveString(const std::string & str);

	private:
		streambuffer	stream_;
		size_t			pos_;
	};

}
