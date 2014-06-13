
#pragma once

/*

#include <ppl/util/inttypes.h>



#ifdef NEED_LOG

namespace std
{

inline ostream& operator<<(ostream& os, unsigned char val)
{
	os << static_cast<size_t>(val);
	return os;
}

#if defined(_WIN32_WCE) || _MSC_VER >= 1400
#pragma message("------output wstring to ostream")
inline ostream& operator<<(ostream& os, const wstring& s)
{
	return os << s.c_str();
}
#endif

}

#endif




inline void put_longlong(ostream& os, longlong val, bool isSigned)
{
	const char* fmt_signed = "%I64d";
	const char* fmt_unsigned = "%I64u";
	const char* fmt = isSigned ? fmt_signed : fmt_unsigned;
	const size_t max_size = 128;
	char buf[max_size + 1]; // 足够大的缓冲区
	_snprintf(buf, max_size, fmt, val);
	os << buf;
}

#if _MSC_VER < 1400

#pragma message("------add support to ostream output for 64 bit integer")

namespace std {

inline ostream& operator<<(ostream& os, longlong val)
{
	put_longlong(os, val, true);
	return os;
}

inline ostream& operator<<(ostream& os, ulonglong val)
{
	put_longlong(os, val, false);
	return os;
}

}

#endif




#ifdef NEED_LOG

#if defined(BOOST_TUPLE_HPP)

#  if defined(_WIN32_WCE)
#    define BOOST_NO_STD_LOCALE
#  endif

//#  include <boost/tuple/tuple_io.hpp>

#else

#pragma message("------ raw make_tuple is deprecated")

template <typename T1, typename T2, typename T3>
struct triple
{
	T1 first;
	T2 second;
	T3 third;

	triple(const T1& a, const T2& b, const T3& c) : first(a), second(b), third(c) { }
};


template <typename T1, typename T2, typename T3>
triple<T1, T2, T3> make_triple(const T1& a, const T2& b, const T3& c)
{
	return triple<T1, T2, T3>(a, b, c);
}



/// 构造pair
template <typename T1, typename T2>
inline pair<T1, T2> make_tuple(const T1& x, const T2& y)
{
	return std::make_pair(x, y);
}

/// 构造triple
template <typename T1, typename T2, typename T3>
inline triple<T1, T2, T3> make_tuple(const T1& x, const T2& y, const T3& z)
{
	return make_triple(x, y, z);
}




template <typename T1, typename T2, typename T3>
inline ostream& operator<<(ostream& os, const triple<T1, T2, T3>& val)
{
	os << "(" << val.first << "," << val.second << "," << val.third << ")";
	return os;
}

#endif


template <typename T1, typename T2>
inline ostream& operator<<(ostream& os, const pair<T1, T2>& val)
{
	os << "(" << val.first << "," << val.second << ")";
	return os;
}


inline pair<const void*, size_t> make_buffer_pair(const void* data, size_t size)
{
	return std::make_pair(data, size);
}


#endif

*/

