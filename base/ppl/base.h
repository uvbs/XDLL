
#pragma once

/**
 * @file
 * @brief 包含常用的头文件
 */


// c/c++库

#pragma warning(disable: 4786)

#include <ppl/diag/memoryleak.h>


#include <algorithm>

#include <assert.h>
#include <tchar.h>


//using namespace std;

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


//using namespace std;


#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif



#include <ppl/util/stream.h>



//#include <atlconv.h>


#include <boost/static_assert.hpp>

#include <ppl/diag/trace.h>

#include <ppl/util/noncopyable.h>
#include <ppl/util/macro.h>

#include <ppl/util/inttypes.h>

#include <ppl/util/log.h>
#include <ppl/util/test.h>



