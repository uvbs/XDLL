
#pragma once

#include <ppl/config.h>

#include <ppl/std/tchar.h>


#if defined(_PPL_PLATFORM_MSWIN )

#include <ppl/mswin/atl/conv.h>

#elif defined(_PPL_PLATFORM_LINUX)

#define CW2A(x) ( x )
#define CT2A(x) ( x )

#endif


