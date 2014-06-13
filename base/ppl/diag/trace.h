
#pragma once

#include <ppl/config.h>

#include <ppl/std/tchar.h>
#include <assert.h>
#include <ppl/stl/string.h>
#include <ppl/std/tchar.h>

#include <stdarg.h>



class Tracer
{
public:
#ifdef _PPL_PLATFORM_MSWIN
	static void Output(const wchar_t* str)
	{
		assert(str != NULL);
		::OutputDebugStringW(str);
	}

	static void Trace(const wchar_t* formatString, ...)
	{
		va_list args;
		va_start(args, formatString);

		const size_t max_size = 4096;
		int nBuf;
		wchar_t szBuffer[max_size + 1] = { 0 };

		nBuf = _vsnwprintf(szBuffer, max_size, formatString, args);

		// was there an error? was the expanded string too long?
		assert(nBuf >= 0);

		Output(szBuffer);

		va_end(args);
	}
#endif

	static void Output(const char* str)
	{
		assert(str != NULL);
		::OutputDebugStringA(str);
	}

	static void Trace(const char* formatString, ...)
	{
		va_list args;
		va_start(args, formatString);

		const size_t max_size = 4096;
		int nBuf;
		char szBuffer[max_size + 1] = { 0 };

		nBuf = vsnprintf(szBuffer, max_size, formatString, args);

		// was there an error? was the expanded string too long?
		assert(nBuf >= 0);

		Output(szBuffer);

		va_end(args);
	}
};



#ifdef _DEBUG
#  define PPLTRACE		::Tracer::Trace
#  define PPLTRACE0		::Tracer::Output
#else
#  define PPLTRACE		1 ? (void)0 : ::Tracer::Trace
#  define PPLTRACE0		1 ? (void)0 : ::Tracer::Output
#endif


#ifndef TRACE
#define TRACE PPLTRACE
#endif

#ifndef TRACE0
#define TRACE0 PPLTRACE0
#endif

#define TRACEOUT	::Tracer::Trace



class ScopedTracer
{
public:
	explicit ScopedTracer(const tstring& str) : m_str(str)
	{
		Tracer::Trace(_T("%s:  Begin\n"), m_str.c_str());
	}
	~ScopedTracer()
	{
		Tracer::Trace(_T("%s:  End\n"), m_str.c_str());
	}
private:
	tstring	m_str;
};


#define TRACE_SCOPE(str)	ScopedTracer _scopedTracer(str)






