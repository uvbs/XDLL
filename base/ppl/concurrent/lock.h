
#pragma once

#include <ppl/config.h>


#if defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/mutex.h>
#include <boost/noncopyable.hpp>



/// 局部的自动锁
template <typename LockT>
class scoped_lock : private boost::noncopyable
{
public:
	explicit scoped_lock(LockT& lock) : m_lock(lock)
	{
		m_lock.Lock();
	}
	~scoped_lock()
	{
		m_lock.Unlock();
	}
private:
	/// 同步对象
	LockT& m_lock;
};

typedef scoped_lock<LightMutex> AutoLightMutex;
typedef scoped_lock<LightweightMutex> AutoLightweightMutex;
typedef scoped_lock<Mutex> AutoMutex;


class RWLock : private boost::noncopyable
{
public:
	RWLock() 
	{
		_readcount = 0; 
	}
	void LockRead()
	{
		AutoLightMutex writeLock(_writer);
		AutoLightMutex mutexLock(_mutex);
		if( ++_readcount == 1 )
			_reader.Lock();
	}
	void UnlockRead()
	{
		AutoLightMutex lock(_mutex);
		if( --_readcount == 0 )
			_reader.Unlock();
	}
	void LockWrite()
	{
		_writer.Lock();
		_reader.Lock();
	}
	void UnlockWrite()
	{
		_reader.Unlock();
		_writer.Unlock();
	}	
private:
	LightMutex _reader;
	LightMutex _writer;
	LightMutex _mutex;
	int _readcount;
};


class AutoReadLock : private boost::noncopyable
{
public:
	explicit AutoReadLock(RWLock& lock) : m_lock(lock) { m_lock.LockRead(); }
	~AutoReadLock() { m_lock.UnlockRead(); }

private:
	RWLock& m_lock;
};

class AutoWriteLock : private boost::noncopyable
{
public:
	explicit AutoWriteLock(RWLock& lock) : m_lock(lock) { m_lock.LockWrite(); }
	~AutoWriteLock() { m_lock.UnlockWrite(); }

private:
	RWLock& m_lock;
};


#else

#include <boost/detail/lightweight_mutex.hpp>

using boost::detail::lightweight_mutex;

typedef boost::detail::lightweight_mutex LightMutex;
typedef boost::detail::lightweight_mutex LightweightMutex;

#endif


