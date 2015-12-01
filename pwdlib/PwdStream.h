#pragma once

namespace pwd
{
	
	class PwdStream
	{
	public:
		PwdStream();
		~PwdStream();

		uchar get();
		void append(uchar ch);

		void gets(void *dest, size_t len);
		void appends(const void *src, size_t leng);
		void skip(size_t len);

		inline bool empty() const { return pos_ >= stream_.size(); }
		
        streambuffer & steam(){ return stream_; }

	public:

		template<class T>
		void loadStruct(T & t)
		{
			gets(&t, sizeof(t));
		}

		template<class T>
		T loadStruct()
		{
			T t;
			loadStruct(t);
			return t;
		}

		template<class T>
		void saveStruct(const T & t)
		{
			appends(&t, sizeof(t));
		}

		void loadString(pwdstring & str)
		{
			size_t len;
			loadStruct(len);
			str.resize(len);
			if (len > 0) gets(&str[0], len * sizeof(str[0]));
		}

		void saveString(const pwdstring & str)
		{
			size_t len = str.length();
			saveStruct(len);
			if (len > 0) appends(str.c_str(), len * sizeof(str[0]));
		}

	private:
		streambuffer	stream_;
		size_t			pos_;
	};

}