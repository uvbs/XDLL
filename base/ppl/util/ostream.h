
#pragma once

/**
 * @file
 * @brief 一个简单的ostream和ostringstream实现(可供windows ce等平台使用)
 */



#include <assert.h>
#include <string>
#include <stdio.h>
//using namespace std;



namespace std {



template<class E, class T>
class basic_ostream
{
};


template<>
class basic_ostream<char, char_traits<char> >
{
public:
	int output(const char* format, ...);
	int outputv(const char* format, va_list args);
	virtual int write(const char* str, int count) = 0;

};

typedef basic_ostream<char, char_traits<char> > ostream;

class ostringstream : public ostream
{
public:
	const string& str() const { return m_str; }

	virtual int write(const char* str, int count);

private:
	string m_str;
};

class stringstream : public ostringstream
{
};

inline int ostream::output(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t count = this->outputv(format, args);
	va_end(args);
	return count;
}

inline int ostream::outputv(const char* format, va_list args)
{
	const size_t max_buffer_size = 2 * 1024;
	char str[max_buffer_size + 1];
	int count = vsnprintf(str, max_buffer_size, format, args);
	assert(count > 0);
	if (count > 0)
	{
		assert(count < max_buffer_size);
		this->write(str, count);
	}
	return count;
}

inline int ostringstream::write(const char* str, int count)
{
	assert(str != NULL);
	assert(count > 0);
	m_str.append(str, count);
	return count;
}



template<>
class basic_ostream<wchar_t, char_traits<wchar_t> >
{
public:
	int output(const wchar_t* format, ...);
	int outputv(const wchar_t* format, va_list args);
	virtual int write(const wchar_t* str, int count) = 0;
};

typedef basic_ostream<wchar_t, char_traits<wchar_t> > wostream;

class wostringstream : public wostream
{
public:
	const wstring& str() const { return m_str; }

	virtual int write(const wchar_t* str, int count);

private:
	wstring m_str;
};

class wstringstream : public wostringstream
{
};

inline int wostream::output(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t count = this->outputv(format, args);
	va_end(args);
	return count;
}

inline int wostream::outputv(const wchar_t* format, va_list args)
{
	const size_t max_buffer_size = 2 * 1024;
	wchar_t str[max_buffer_size + 1];
	int count = _vsnwprintf(str, max_buffer_size, format, args);
	assert(count > 0);
	if (count > 0)
	{
		assert(count < max_buffer_size);
		this->write(str, count);
	}
	return count;
}

inline int wostringstream::write(const wchar_t* str, int count)
{
	assert(str != NULL);
	assert(count > 0);
	m_str.append(str, count);
	return count;
}



inline ostream& __cdecl endl(ostream& os)
{
	os.write("\n", 1);
	return os;
}

inline wostream& __cdecl endl(wostream& os)
{
	os.write(L"\n", 1);
	return os;
}
inline ostream& __cdecl hex(ostream& os)
{
	return os;
}

inline ostream& __cdecl dec(ostream& os)
{
	return os;
}




template <typename T>
inline ostream& write_to_stream(ostream& os, T val, const char* format)
{
	os.output(format, val);
	return os;
}

inline ostream& operator<<(ostream& os, char val)
{
	return write_to_stream(os, val, "%c");
}

inline ostream& operator<<(ostream& os, int val)
{
	return write_to_stream(os, val, "%d");
}

inline ostream& operator<<(ostream& os, unsigned int val)
{
	return write_to_stream(os, val, "%u");
}

inline ostream& operator<<(ostream& os, short val)
{
	return write_to_stream(os, val, "%hd");
}

inline ostream& operator<<(ostream& os, unsigned short val)
{
	return write_to_stream(os, val, "%hu");
}

inline ostream& operator<<(ostream& os, long val)
{
	return write_to_stream(os, val, "%ld");
}

inline ostream& operator<<(ostream& os, unsigned long val)
{
	return write_to_stream(os, val, "%lu");
}

inline ostream& operator<<(ostream& os, const char* str)
{
	return write_to_stream(os, str, "%hs");
}

inline ostream& operator<<(ostream& os, const wchar_t* str)
{
	return write_to_stream(os, str, "%ls");
}

inline ostream& operator<<(ostream& os, const string& str)
{
	os.write(str.c_str(), str.size());
	return os;
}

inline ostream& operator<<(ostream& os, const wstring& str)
{
	return os << str.c_str();
}

inline ostream& operator<<(ostream& os, const void* p)
{
	return write_to_stream(os, p, "%p");
}

inline ostream& operator<<(ostream& os, float val)
{
	return write_to_stream(os, val, "%f");
}

inline ostream& operator<<(ostream& os, double val)
{
	return write_to_stream(os, val, "%f");
}

inline ostream& operator<<(ostream& os, ostream& (__cdecl *fun)(ostream&))
{
	return fun(os);
}

/*
inline ostream& operator<<(ostream& os, __int64 val)
{
	return write_to_stream(os, val, "%I64d");
}

inline ostream& operator<<(ostream& os, unsigned __int64 val)
{
	return write_to_stream(os, val, "%I64u");
}
*/




template <typename T>
inline wostream& write_to_stream(wostream& os, T val, const wchar_t* format)
{
	os.output(format, val);
	return os;
}

inline wostream& operator<<(wostream& os, char val)
{
	return write_to_stream(os, val, L"%c");
}

inline wostream& operator<<(wostream& os, int val)
{
	return write_to_stream(os, val, L"%d");
}

inline wostream& operator<<(wostream& os, unsigned int val)
{
	return write_to_stream(os, val, L"%u");
}

inline wostream& operator<<(wostream& os, short val)
{
	return write_to_stream(os, val, L"%hd");
}

inline wostream& operator<<(wostream& os, unsigned short val)
{
	return write_to_stream(os, val, L"%hu");
}

inline wostream& operator<<(wostream& os, long val)
{
	return write_to_stream(os, val, L"%ld");
}

inline wostream& operator<<(wostream& os, unsigned long val)
{
	return write_to_stream(os, val, L"%lu");
}

inline wostream& operator<<(wostream& os, const wchar_t* str)
{
	return write_to_stream(os, str, L"%hs");
}

inline wostream& operator<<(wostream& os, const char* str)
{
	return write_to_stream(os, str, L"%ls");
}

inline wostream& operator<<(wostream& os, const wstring& str)
{
	os.write(str.c_str(), str.size());
	return os;
}

inline wostream& operator<<(wostream& os, const string& str)
{
	return os << str.c_str();
}

inline wostream& operator<<(wostream& os, const void* p)
{
	return write_to_stream(os, p, L"%p");
}




/*
inline wostream& operator<<(wostream& os, __int64 val)
{
	return write_to_stream(os, val, L"%I64d");
}

inline wostream& operator<<(wostream& os, unsigned __int64 val)
{
	return write_to_stream(os, val, L"%I64u");
}
*/





#ifdef UNICODE
typedef wostream tostream;
typedef wostringstream tostringstream;
#else
typedef ostream tostream;
typedef ostringstream tostringstream;
#endif



}



using std::endl;


