
#pragma once

#include <ppl/config.h>

#include <ppl/stl/string.h>
#include <ppl/std/tchar.h>

#include <utility>
using std::pair;
using std::make_pair;

//#include <tchar.h>
#include <assert.h>

#include <algorithm>

#include <stdarg.h>

#include <iosfwd>
#include <sstream>


//using std::ostringstream;
//using std::wostringstream;

#ifdef UNICODE

typedef std::wostream tostream;
typedef std::wistream tistream;

typedef std::wostringstream tostringstream;
typedef std::wistringstream tistringstream;

typedef std::wofstream tofstream;
typedef std::wifstream tifstream;

#else

typedef std::ostream tostream;
typedef std::istream tistream;

typedef std::ostringstream tostringstream;
typedef std::istringstream tistringstream;

typedef std::ofstream tofstream;
typedef std::ifstream tifstream;

#endif




/// 字符串工具类
class strings
{
public:
	/// 不分大小写的比较std::string
	static bool equals_ignore_case(const string& s1, const string& s2)
	{
		return iequals(s1, s2);
	}
	/// 不分大小写的比较std::string
	static bool iequals(const string& s1, const string& s2)
	{
		return (s1.size() == s2.size()) && (_strnicmp(s1.c_str(), s2.c_str(), s1.size()) == 0);
	}

	/// 大写
	template <typename CharT, typename TraitsT, typename AllocatorT>
	static void make_upper(std::basic_string<CharT, TraitsT, AllocatorT>& s)
	{
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	}

	/// 小写
	template <typename CharT, typename TraitsT, typename AllocatorT>
	static void make_lower(std::basic_string<CharT, TraitsT, AllocatorT>& s)
	{
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	}

	/// 大写
	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::basic_string<CharT, TraitsT, AllocatorT> upper(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
	{
		std::basic_string<CharT, TraitsT, AllocatorT> result = s;
		make_upper(result);
		return result;
	}

	/// 小写
	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::basic_string<CharT, TraitsT, AllocatorT> lower(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
	{
		std::basic_string<CharT, TraitsT, AllocatorT> result = s;
		make_lower(result);
		return result;
	}



	template <typename CharT, typename TraitsT, typename AllocatorT, typename OutputIteratorT>
	static void split(OutputIteratorT output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, CharT delimiter)
	{
		typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
		size_t startPos = 0;
		size_t pos;
		string_type str;
		for (;;)
		{
			pos = src.find(delimiter, startPos);
			str = src.substr(startPos, pos - startPos);
			*output++ = str;
			if (pos == string_type::npos)
				break;
			startPos = pos + 1;
		}
	}

	template <typename CharT, typename TraitsT, typename AllocatorT, typename OutputIteratorT>
	static void split(OutputIteratorT output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter)
	{
		typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
		size_t startPos = 0;
		size_t pos;
		string_type str;
		for (;;)
		{
			pos = src.find(delimiter, startPos);
			str = src.substr(startPos, pos - startPos);
			*output++ = str;
			if (pos == string_type::npos)
				break;
			startPos = pos + delimiter.size();
		}
	}

	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
		split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, CharT delimiter)
	{
		typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
		size_t pos = src.find(delimiter, 0);
		if (pos == string_type::npos)
			return std::make_pair(src, string_type());
		assert(pos + 1 <= src.size());
		return std::make_pair(src.substr(0, pos), src.substr(pos + 1));
	}

	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
		split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter)
	{
		typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
		size_t pos = src.find(delimiter, 0);
		if (pos == string_type::npos)
			return std::make_pair(src, string_type());
		assert(pos + delimiter.size() <= src.size());
		return std::make_pair(src.substr(0, pos), src.substr(pos + delimiter.size()));
	}

	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
		split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const CharT* delimiter)
	{
		return split_pair(src, delimiter, TraitsT::length(delimiter));
	}

	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
		split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const CharT* delimiter, size_t delimiterSize)
	{
		typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
		size_t pos = src.find(delimiter, 0, delimiterSize);
		if (pos == string_type::npos)
			return std::make_pair(src, string_type());
		assert(pos + delimiterSize <= src.size());
		return std::make_pair(src.substr(0, pos), src.substr(pos + delimiterSize));
	}


	/// 在指定的位置分割字符串，指定位置对应的字符包含在second中
	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
		split_at(const std::basic_string<CharT, TraitsT, AllocatorT>& src, size_t pos)
	{
		assert(pos <= src.size());
		return std::make_pair(src.substr(0, pos), src.substr(pos));
	}

	/// 在指定的位置分割字符串，指定位置对应的字符不被返回
	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
		split_on(const std::basic_string<CharT, TraitsT, AllocatorT>& src, size_t pos)
	{
		assert(pos < src.size());
		return std::make_pair(src.substr(0, pos), src.substr(pos + 1));
	}


/*	template <typename CharT, typename TraitsT, typename AllocatorT, typename InputIteratorT>
	static std::basic_string<CharT, TraitsT, AllocatorT> join(InputIteratorT begin, InputIteratorT end, char delimiter)
	{
		assert(!"not implemented");
		return "";
	}*/

	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::basic_string<CharT, TraitsT, AllocatorT> trim(const std::basic_string<CharT, TraitsT, AllocatorT>& str, const CharT* delimiter)
	{
		std::basic_string<CharT, TraitsT, AllocatorT> s = str;
		assert(TraitsT::length(delimiter) > 0);
		s.erase(0, s.find_first_not_of(delimiter));
		s.erase(s.find_last_not_of(delimiter) + 1);
		return s;
	}

	template <typename AllocatorT>
	static std::basic_string<char, std::char_traits<char>, AllocatorT> trim(const std::basic_string<char, std::char_traits<char>, AllocatorT>& str)
	{
		return strings::trim(str, " ");
	}

	template <typename AllocatorT>
	static std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocatorT> trim(const std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocatorT>& str)
	{
		return strings::trim(str, L" ");
	}

	template <typename T>
	static string format_object(const T& obj)
	{
		std::ostringstream oss;
		oss << obj;
		return oss.str();
	}


	static string format(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		string str = vformat(format, args);
		va_end(args);
		return str;
	}
	static string vformat(const char* format, va_list argList)
	{
		const size_t max_size = 1024;
		char str[max_size + 1];
		str[max_size] = 0;
		int count = vsnprintf(str, max_size, format, argList);
		if (count < 0)
		{
			assert(false);
			return string();
		}
		assert(count < max_size);
		return string(str, count);
	}

#ifndef _PPL_PLATFORM_LINUX
	static wstring format(const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);
		wstring str = vformat(format, args);
		va_end(args);
		return str;
	}
	static wstring vformat(const wchar_t* format, va_list argList)
	{
		const size_t max_size = 1024;
		wchar_t str[max_size + 1];
		str[max_size] = 0;
		int count = _vsnwprintf(str, max_size, format, argList);
		if (count < 0)
		{
			assert(false);
			return wstring();
		}
		assert(count < max_size);
		return wstring(str, count);
	}
#endif

	template <typename CharT, typename TraitsT, typename AllocatorT>
	static bool is_upper(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
	{
		for (size_t i = 0; i < str.size(); ++i)
		{
			if (!::isupper(str[i]))
				return false;
		}
		return true;
	}

};



