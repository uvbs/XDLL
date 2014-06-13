
#pragma once

/**
 * @file
 * @brief interlocked功能的封装类
 */


/// interlocked方式的同步机制
class Interlocked
{
public:
	/// 增加
	static long Increment(long& val)
	{
		return ::InterlockedIncrement(&val);
	}

	/// 缩减
	static long Decrement(long& val)
	{
		return ::InterlockedDecrement(&val);
	}
};


/// 同步保护的整数
class InterlockedInteger
{
public:
	InterlockedInteger() : m_val(0) { }

	long Increment() { return Interlocked::Increment(m_val); }
	long Decrement() { return Interlocked::Decrement(m_val); }

private:
	long m_val;
};





