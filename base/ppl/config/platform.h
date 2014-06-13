
#pragma once


#if defined(linux) || defined(__linux) || defined(__linux__)

#include <ppl/config/platform/linux.h>

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#include <ppl/config/platform/mswin.h>

#else

#error invalid platform

#endif
