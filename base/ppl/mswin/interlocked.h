
#pragma once

/**
 * @file
 * @brief interlocked���ܵķ�װ��
 */


/// interlocked��ʽ��ͬ������
class Interlocked
{
public:
	/// ����
	static long Increment(long& val)
	{
		return ::InterlockedIncrement(&val);
	}

	/// ����
	static long Decrement(long& val)
	{
		return ::InterlockedDecrement(&val);
	}
};


/// ͬ������������
class InterlockedInteger
{
public:
	InterlockedInteger() : m_val(0) { }

	long Increment() { return Interlocked::Increment(m_val); }
	long Decrement() { return Interlocked::Decrement(m_val); }

private:
	long m_val;
};





