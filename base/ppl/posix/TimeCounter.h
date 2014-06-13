
#pragma once

#include <sys/time.h>
#include<time.h>


inline UINT64 GetTickCount64()
{
	struct timespec t = { 0 };
	int res = clock_gettime(CLOCK_MONOTONIC, &t);
	assert( 0 == res );
	double val = t.tv_sec * 1000.0 + t.tv_nsec * 0.000001;
	return static_cast<UINT64>( val );
}


/*
inline UINT32 GetTickCount()
{
	static UINT64 initialTick64 = GetTickCount64();
	UINT64 currentTick64 = GetTickCount64();
	assert(currentTick64 >= initialTick64);
	UINT64 diff = currentTick64 - initialTick64;
	assert(diff < INT_MAX);
	return static_cast<UINT32>( diff );
}
*/

inline UINT32 GetTickCount()
{
	UINT64 ticks = GetTickCount64();
	return static_cast<UINT32>( ticks );
}


/// 基于GetTickCount的计时器(单位：毫秒)
class TimeCounter
{
public:
	typedef UINT64 count_value_type;

	TimeCounter()
	{
		m_count = GetSystemCount();
	}
	explicit TimeCounter(count_value_type count) : m_count(count)
	{
	}

	static UINT64 GetSystemCount()
	{
		return GetTickCount64();
	}
	static UINT32 GetSystemCount32()
	{
		return static_cast<UINT32>( GetTickCount64() );
	}


	/// 同步计时
	void Sync()
	{
		m_count = GetSystemCount();
	}
	/// 获取从上次计时开始的时间
	count_value_type GetElapsed() const
	{
		return GetSystemCount() - m_count;
	}
	DWORD GetElapsed32() const
	{
		return static_cast<DWORD>( this->GetElapsed() );
	}

	/// 获取所记录的时间
	count_value_type GetCount() const
	{
		return m_count;
	}
	/// 自动转换成DWORD
	operator count_value_type() const
	{
		return GetCount();
	}


#pragma message("------ use simulated 64-bit GetTickCount on posix platform")
	/// 获取当前的时间
	UINT64 GetRealtimeCount()
	{
		return GetSystemCount();
	}

private:
	/// 所记录的时间
	count_value_type m_count;
};






