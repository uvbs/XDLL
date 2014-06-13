
#pragma once

#define _PPL_USES_TICK_COUNTER64



/**
 * @file
 * @brief 基础工具类－－时间计数
 */


class TickCounter64
{
public:
	TickCounter64()
	{
		m_count32 = ::GetTickCount();
		m_count64 = m_count32;
	}

	/// 获取当前的时间计数
	UINT64 GetCount()
	{
		DWORD newCount32 = ::GetTickCount();
		DWORD diff = newCount32 - m_count32;
		m_count32 = newCount32;
		m_count64 += diff;
		return m_count64;
	}


private:
	UINT64 m_count64;
	DWORD m_count32;
};

//#define _PPL_USES_TICK_COUNTER64


/// 基于GetTickCount的计时器(单位：毫秒)
class TimeCounter
{
public:
//#if defined(_PPL_USES_TICK_COUNTER64)
	typedef UINT64 count_value_type;
//#else
	//typedef DWORD count_value_type;
//#endif

	static UINT64 GetSystemCount()
	{
		static TickCounter64 tickCounter;
		return tickCounter.GetCount();
	}

	static DWORD GetSystemCount32()
	{
		return ::GetTickCount();
	}

	TimeCounter()
	{
		m_count = GetRealtimeCount();
	}
	explicit TimeCounter(UINT64 count) : m_count(count)
	{
	}

	/// 同步计时
	void Sync()
	{
		m_count = GetRealtimeCount();
	}
	/// 获取从上次计时开始的时间
	UINT64 GetElapsed() const
	{
		return GetRealtimeCount() - m_count;
	}
	/// 获取从上次计时开始的时间
	DWORD GetElapsed32() const
	{
		return static_cast<DWORD>( GetRealtimeCount() - m_count );
	}

	/// 获取所记录的时间
	UINT64 GetCount() const
	{
		return m_count;
	}
	/// 自动转换成DWORD
	operator UINT64() const
	{
		return GetCount();
	}

//#if defined(_PPL_USES_TICK_COUNTER64)
#pragma message("------ use simulated 64-bit GetTickCount")
	/// 获取当前的时间
	UINT64 GetRealtimeCount() const
	{
		return m_tick.GetCount();
	}
//#else
	//DWORD GetTimeCount()
	//{
	//	return ::GetTickCount();
	//}
//#endif

private:
	/// 所记录的时间
	UINT64 m_count;
	mutable TickCounter64 m_tick;
};







