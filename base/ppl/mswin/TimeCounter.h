
#pragma once

#define _PPL_USES_TICK_COUNTER64



/**
 * @file
 * @brief ���������࣭��ʱ�����
 */


class TickCounter64
{
public:
	TickCounter64()
	{
		m_count32 = ::GetTickCount();
		m_count64 = m_count32;
	}

	/// ��ȡ��ǰ��ʱ�����
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


/// ����GetTickCount�ļ�ʱ��(��λ������)
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

	/// ͬ����ʱ
	void Sync()
	{
		m_count = GetRealtimeCount();
	}
	/// ��ȡ���ϴμ�ʱ��ʼ��ʱ��
	UINT64 GetElapsed() const
	{
		return GetRealtimeCount() - m_count;
	}
	/// ��ȡ���ϴμ�ʱ��ʼ��ʱ��
	DWORD GetElapsed32() const
	{
		return static_cast<DWORD>( GetRealtimeCount() - m_count );
	}

	/// ��ȡ����¼��ʱ��
	UINT64 GetCount() const
	{
		return m_count;
	}
	/// �Զ�ת����DWORD
	operator UINT64() const
	{
		return GetCount();
	}

//#if defined(_PPL_USES_TICK_COUNTER64)
#pragma message("------ use simulated 64-bit GetTickCount")
	/// ��ȡ��ǰ��ʱ��
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
	/// ����¼��ʱ��
	UINT64 m_count;
	mutable TickCounter64 m_tick;
};







