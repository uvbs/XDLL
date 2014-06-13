
#pragma once


#include <ppl/config.h>


#if defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/file.h>

#elif defined(_PPL_PLATFORM_LINUX)

#include <ppl/posix/file.h>

#endif


