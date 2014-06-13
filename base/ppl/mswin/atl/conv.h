
#pragma once

#include <ppl/util/macro.h>

#define _CONVERSION_DONT_USE_THREAD_LOCALE
#define _ATL_NO_EXCEPTIONS

#if _MSC_VER >= 1400
#include <atlexcept.h>
#else
#include <crtdbg.h>
#include <objbase.h>
#endif


#if _MSC_VER >= 1400
#include <atlchecked.h>
#endif

#include <atlconv.h>


#if _MSC_VER >= 1400
using namespace ATL;
#pragma comment(lib, PPL_LIB_FILE("atls"))
#else
#define CW2A(x) W2A(x)
#endif


/*

#ifdef _DEBUG
#pragma comment(lib, "atlsd.lib")
#else
#pragma comment(lib, "atls.lib")
#endif


#include <atlchecked.h>
#include <atlexcept.h>
#include <atlconv.h>
using namespace ATL;

*/


