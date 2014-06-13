
#pragma once

#include <ppl/config.h>

#if defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/module.h>

#elif defined(_PPL_PLATFORM_LINUX)

#include <ppl/posix/module.h>

#else

#error no module.h

#endif
