
#pragma once

#include <ppl/config.h>

#if defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/shell.h>

#elif defined(_PPL_PLATFORM_LINUX)

#include <ppl/posix/shell.h>

#else

#error "invalid platform"

#endif



