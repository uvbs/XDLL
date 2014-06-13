
#pragma once

#include <boost/thread.hpp>

#include <ppl/config.h>

class Threads
{
public:
	static void Kill(boost::thread& t, int exitCode)
	{
#if defined(_PPL_PLATFORM_MSWIN)
		::TerminateThread(t.native_handle(), exitCode);
#elif defined(_PPL_PLATFORM_LINUX)
		::pthread_kill(t.native_handle(), NULL);
#else
#error no thread killer
#endif

	}
};