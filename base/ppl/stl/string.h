
#pragma once

#include <ppl/config.h>


#include <string>

#if defined(_PPL_PLATFORM_MSWIN)
#include <tchar.h>
#endif


using std::string;
using std::wstring;



#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif



