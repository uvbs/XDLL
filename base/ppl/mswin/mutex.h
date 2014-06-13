
#pragma once

#include <boost/noncopyable.hpp>


class CriticalSection : private boost::noncopyable
{
private:
	CRITICAL_SECTION	m_cs;

public:
	CriticalSection()
	{
		::InitializeCriticalSection(&m_cs);
	}
	~CriticalSection()
	{
		::DeleteCriticalSection(&m_cs);
	}

	bool Enter()
	{
		//__try
		{
			::EnterCriticalSection(&m_cs);
		}
		//__except (EXCEPTION_EXECUTE_HANDLER)
		//{
		//	return false;
		//}
		return true;
	}
	bool Leave()
	{
		::LeaveCriticalSection(&m_cs);
		return true;
	}

	bool Lock()
	{
		return Enter();
	}
	bool Unlock()
	{
		return Leave();
	}
};


class Mutex : private boost::noncopyable
{
public:
	Mutex()
	{
		m_handle = ::CreateMutex(NULL, FALSE, NULL);
	}
	~Mutex()
	{
		Close();
	}

	bool IsValid() const
	{
		return m_handle != NULL;
	}
	void Close()
	{
		if (IsValid())
		{
			::CloseHandle(m_handle);
			m_handle = NULL;
		}
	}

	bool Acquire()
	{
		return WAIT_OBJECT_0 == ::WaitForSingleObject(m_handle, INFINITE);
	}
	bool Release()
	{
		return FALSE != ::ReleaseMutex(m_handle);
	}

	bool Lock()
	{
		return Acquire();
	}
	bool Unlock()
	{
		return Release();
	}

private:
	HANDLE	m_handle;
};


typedef CriticalSection LightMutex;
typedef CriticalSection LightweightMutex;




