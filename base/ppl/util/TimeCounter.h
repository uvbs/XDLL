
#pragma once

#include <ppl/config.h>

#if defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/TimeCounter.h>

#elif defined(_PPL_PLATFORM_POSIX)

#include <ppl/posix/TimeCounter.h>

#else

#error unsupported platform for TimeCounter

#endif




class TimeSpan
{
public:
	enum 
	{ 
		TICKS_PER_SECOND = 1000, 
		TICKS_PER_MINUTE = 1000 * 60, 
		TICKS_PER_HOUR = 1000 * 60 * 60, 
		TICKS_PER_DAY = 1000 * 60 * 60 * 24, 
	};

	explicit TimeSpan( UINT64 diff = 0 ) : m_Diff( diff ) { }

	UINT64 GetTotalTicks() const { return m_Diff; }
	UINT64 GetTotalDays() const { return m_Diff / TICKS_PER_DAY; }
	UINT64 GetTotalHours() const { return m_Diff / TICKS_PER_HOUR; }
	UINT64 GetTotalMinutes() const { return m_Diff / TICKS_PER_MINUTE; }
	UINT64 GetTotalSeconds() const { return m_Diff / TICKS_PER_SECOND; }
	UINT64 GetTotalMilliseconds() const { return m_Diff; }

	UINT GetDays() const { return static_cast<UINT>( this->GetTotalDays() ); }

	UINT GetHours() const
	{
		UINT oddTicks = static_cast<UINT>( m_Diff % TICKS_PER_DAY );
		return oddTicks / TICKS_PER_HOUR;
	}
	UINT GetMinutes() const
	{
		UINT oddTicks = static_cast<UINT>( m_Diff % TICKS_PER_HOUR );
		return oddTicks / TICKS_PER_MINUTE;
	}
	UINT GetSeconds() const
	{
		UINT oddTicks = static_cast<UINT>( m_Diff % TICKS_PER_MINUTE );
		return oddTicks / TICKS_PER_SECOND;
	}
	UINT GetMilliseconds() const
	{
		UINT oddTicks = static_cast<UINT>( m_Diff % TICKS_PER_SECOND );
		return oddTicks;
	}

private:
	UINT64 m_Diff;
};


