
#pragma once

#include <ppl/config.h>
#include <ppl/stl/string.h>
#include <ppl/std/tchar.h>


template <typename T>
struct numerics_traits;


template <>
struct numerics_traits<short>
{
	typedef int value_type;

	static const char* ansi_format_string()
	{
		return "%hd";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%hd";
	}
};


template <>
struct numerics_traits<unsigned short>
{
	typedef unsigned int value_type;

	static const char* ansi_format_string()
	{
		return "%hu";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%hu";
	}
};


template <>
struct numerics_traits<int>
{
	typedef int value_type;

	static const char* ansi_format_string()
	{
		return "%d";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%d";
	}
};


template <>
struct numerics_traits<unsigned int>
{
	typedef unsigned int value_type;

	static const char* ansi_format_string()
	{
		return "%u";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%u";
	}
};


template <>
struct numerics_traits<long>
{
	typedef long value_type;

	static const char* ansi_format_string()
	{
		return "%ld";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%ld";
	}
};


template <>
struct numerics_traits<unsigned long>
{
	typedef unsigned long value_type;

	static const char* ansi_format_string()
	{
		return "%lu";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%lu";
	}
};



#if defined(_MSC_VER) && _MSC_VER <= 1200

template <>
struct numerics_traits<__int64>
{
	typedef __int64 value_type;

	static const char* ansi_format_string()
	{
		return "%I64d";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%I64d";
	}
};


template <>
struct numerics_traits<unsigned __int64>
{
	typedef unsigned __int64 value_type;

	static const char* ansi_format_string()
	{
		return "%I64u";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%I64u";
	}
};

#else

template <>
struct numerics_traits<long long>
{
	typedef long long value_type;

	static const char* ansi_format_string()
	{
		return "%lld";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%lld";
	}
};


template <>
struct numerics_traits<unsigned long long>
{
	typedef unsigned long long value_type;

	static const char* ansi_format_string()
	{
		return "%llu";
	}
	static const wchar_t* wide_format_string()
	{
		return L"%llu";
	}
};

#endif


template <>
struct numerics_traits<double>
{
	typedef double value_type;

	static const char* ansi_format_string()
	{
		return "%f";
	}
	static const wchar_t* wide_format_string()
	{
		return L"f";
	}
};


template <>
struct numerics_traits<float> : public numerics_traits<double>
{
	typedef float value_type;
};




class numerics
{
public:
	/*
	static int parse_int(const string& s, int defaultVal)
	{
		return parse_int(s.c_str(), defaultVal);
	}
	static int parse_int(const char* s, int defaultVal)
	{
		return parse(s, defaultVal, "%d");
	}

	static int parse_uint(const string& s, unsigned int defaultVal)
	{
		return parse_uint(s.c_str(), defaultVal);
	}
	static int parse_uint(const char* s, unsigned int defaultVal)
	{
		return parse(s, defaultVal, "%u");
	}

	static int parse_int(const wstring& s, int defaultVal)
	{
		return parse_int(s.c_str(), defaultVal);
	}
	static int parse_int(const wchar_t* s, int defaultVal)
	{
		return parse(s, defaultVal, L"%d");
	}

	static int parse_uint(const wstring& s, unsigned int defaultVal)
	{
		return parse_uint(s.c_str(), defaultVal);
	}
	static int parse_uint(const wchar_t* s, unsigned int defaultVal)
	{
		return parse(s, defaultVal, L"%u");
	}
	*/


	template <typename T>
	static T parse(const char* s, T defaultVal, const char* formatString)
	{
		T val = T();
		if ( 1 != sscanf(s, formatString, &val) )
			return defaultVal;
		return val;
	}

	template <typename T>
	static T parse(const wchar_t* s, T defaultVal, const wchar_t* formatString)
	{
		T val = T();
		if ( 1 != swscanf(s, formatString, &val) )
			return defaultVal;
		return val;
	}

	template <typename T>
	static T parse(const string& s, T defaultVal, const char* formatString)
	{
		return parse(s.c_str(), defaultVal, formatString);
	}
	template <typename T>
	static T parse(const wstring& s, T defaultVal, const wchar_t* formatString)
	{
		return parse(s.c_str(), defaultVal, formatString);
	}


	/*
	template <typename T>
	static bool try_parse(const char* s, T& outval)
	{
		return try_parse(s, outval, numerics_traits<T>::ansi_format_string());
	}

	template <typename T>
	static bool try_parse(const string& s, T& outval)
	{
		return try_parse(s, outval, numerics_traits<T>::ansi_format_string());
	}

	template <typename T, typename TraitsT>
	static bool try_parse(const wchar_t* s, T& outval)
	{
		return try_parse(s, outval, numerics_traits<T>::wide_format_string());
	}

	template <typename T, typename TraitsT>
	static bool try_parse(const wstring& s, T& outval, TraitsT traits)
	{
		return try_parse(s, outval, numerics_traits<T>::wide_format_string());
	}
	*/



	template <typename T>
	static bool try_parse(const char* s, T& outval, const char* formatString)
	{
		T val = T();
		if ( 1 != sscanf(s, formatString, &val) )
			return false;
		outval = val;
		return true;
	}

	template <typename T>
	static bool try_parse(const string& s, T& outval, const char* formatString)
	{
		return try_parse(s.c_str(), outval, formatString);
	}

	template <typename T>
	static bool try_parse(const wchar_t* s, T& outval, const wchar_t* formatString)
	{
		T val = T();
		if ( 1 != swscanf(s, formatString, &val) )
			return false;
		outval = val;
		return true;
	}

	template <typename T>
	static bool try_parse(const wstring& s, T& outval, const wchar_t* formatString)
	{
		return try_parse(s.c_str(), outval, formatString);
	}



/*
	static string format_uint(unsigned int val)
	{
		return numerics::format(val, "%u");
	}
	static string format_int(int val)
	{
		return numerics::format(val, "%d");
	}
*/


	template <typename T>
	static string format(T val, const char* formatString)
	{
		const size_t max_buffer_size = 63;
		char buf[max_buffer_size + 1];
		buf[max_buffer_size] = '\0';
		int len = snprintf(buf, max_buffer_size, formatString, val);
		if ( len <= 0 || len > max_buffer_size )
			return string();
		return string(buf, len);
	}

#if defined(_PPL_PLATFORM_MSWIN)
	template <typename T>
	static wstring wformat(T val, const wchar_t* formatString)
	{
		const size_t max_buffer_size = 63;
		wchar_t buf[max_buffer_size + 1];
		buf[max_buffer_size] = '\0';
		int len = snwprintf(buf, max_buffer_size, formatString, val);
		if ( len <= 0 || len > max_buffer_size )
			return wstring();
		return wstring(buf, len);
	}
#endif

};



template <typename ValueT>
class numeric
{
public:
	typedef numerics_traits<ValueT> traits_type;
	typedef typename traits_type::value_type value_type;

	static ValueT parse(const string& s, ValueT defaultVal)
	{
		return numerics::parse(s, defaultVal, traits_type::ansi_format_string());
	}
	static ValueT parse(const char* s, ValueT defaultVal)
	{
		return numerics::parse(s, defaultVal, traits_type::ansi_format_string());
	}

	static ValueT parse(const wstring& s, ValueT defaultVal)
	{
		return numerics::parse(s, defaultVal, traits_type::wide_format_string());
	}
	static ValueT parse(const wchar_t* s, ValueT defaultVal)
	{
		return numerics::parse(s, defaultVal, traits_type::wide_format_string());
	}


	static bool try_parse(const char* s, ValueT& outval)
	{
		return numerics::try_parse(s, outval, traits_type::ansi_format_string());
	}

	static bool try_parse(const string& s, ValueT& outval)
	{
		return numerics::try_parse(s, outval, traits_type::ansi_format_string());
	}

	static bool try_parse(const wchar_t* s, ValueT& outval)
	{
		return numerics::try_parse(s, outval, traits_type::wide_format_string());
	}

	static bool try_parse(const wstring& s, ValueT& outval)
	{
		return numerics::try_parse(s, outval, traits_type::wide_format_string());
	}


	static string format(ValueT val)
	{
		return numerics::format(val, traits_type::ansi_format_string());
	}
#if defined(_PPL_PLATFORM_MSWIN)
	static wstring wformat(ValueT val)
	{
		return numerics::wformat(val, traits_type::wide_format_string());
	}
#endif
};






