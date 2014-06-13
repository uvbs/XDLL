
#pragma once

#include <ppl/config.h>

#include <boost/noncopyable.hpp>



#if defined(_PPL_PLATFORM_MSWIN)

class Process : private boost::noncopyable
{
public:
	static DWORD CurrentProcessID()
	{
		return ::GetCurrentProcessId();
	}
	static DWORD CurrentThreadID()
	{
		return ::GetCurrentThreadId();
	}
};


#elif defined(_PPL_PLATFORM_LINUX)

class Process : private boost::noncopyable
{
public:
	static pid_t CurrentProcessID()
	{
		return ::getpid();
	}
};


#endif
