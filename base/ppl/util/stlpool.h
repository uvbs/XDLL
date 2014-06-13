
#pragma once

#include <string>

#include <map>
#include <set>
#include <vector>
#include <list>
#include <deque>

#include <algorithm>


namespace pool {


template<typename Key, typename T, typename Pred = std::less<Key> >
class map : public std::map<Key, T, Pred, pool_allocator<T> >
{
};


template<typename Key, typename T, typename Pred = std::less<Key> >
class multimap : public std::multimap<Key, T, Pred, pool_allocator<T> >
{
};


template<typename Key, typename Pred = std::less<Key> >
class set : public std::set<Key, Pred, pool_allocator<Key> >
{
};


template<typename T>
class vector : public std::vector<T, pool_allocator<T> >
{
};


template<typename T>
class list : public std::list<T, pool_allocator<T> >
{
};

template<typename T>
class deque : public std::deque<T, pool_allocator<T> >
{
};

typedef std::basic_string<char, std::char_traits<char>, pool_allocator<char> > string;
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, pool_allocator<wchar_t> > wstring;

typedef std::basic_string<BYTE, std::char_traits<BYTE>, pool_allocator<BYTE> > byte_string;

}


#if defined(_PPL_USE_POOL_ALLOCATOR_FOR_STL)

#pragma message("------use pool_allocator in stl containers")

using pool::map;
using pool::multimap;
using pool::set;
using pool::vector;
using pool::list;
using pool::deque;

#else

#pragma message("------do not use pool_allocator in stl containers")

using std::map;
using std::multimap;
using std::set;
using std::vector;
using std::list;
using std::deque;

#endif

typedef pool::string pool_string;
typedef pool::byte_string pool_byte_string;


#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif


using std::string;
using std::wstring;

using std::ostream;
using std::basic_string;
//using std::make_pair;
using std::pair;

// using std::set;
// using std::map;
// using std::vector;
// using std::list;
// using std::multimap;
// using std::string;
// using std::wstring;

using std::auto_ptr;

using std::endl;
using std::hex;
using std::dec;


using std::make_pair;
