
#pragma once

#include <boost/noncopyable.hpp>
#include <assert.h>

/// win32事件对象
class Win32Event : private boost::noncopyable
{
public:
	Win32Event() : m_handle(NULL)
	{
	}
	~Win32Event()
	{
		this->Close();
	}
	bool IsValid() const { return m_handle != NULL; }
	
	void Close()
	{
		if (IsValid())
		{
			::CloseHandle(m_handle);
			m_handle = NULL;
		}
	}

	BOOL Signal()
	{
		assert(IsValid());
		return ::SetEvent(m_handle);
	}
	BOOL Reset()
	{
		assert(IsValid());
		return ::ResetEvent(m_handle);
	}
	BOOL Pulse()
	{
		assert(IsValid());
		return ::PulseEvent(m_handle);
	}

	DWORD Wait()
	{
		return Wait(INFINITE);
	}
	DWORD Wait(DWORD milliseconds)
	{
		return ::WaitForSingleObject(m_handle, milliseconds);
	}
	DWORD GetState()
	{
		return this->Wait(0);
	}
	bool IsSignaled()
	{
		return GetState() == WAIT_OBJECT_0;
	}
	HANDLE GetHandle() const { return m_handle; }

protected:
	void Attach(HANDLE hEvent)
	{
		assert(!IsValid());
		m_handle = hEvent;
	}

private:
	HANDLE	m_handle;
};


/// 手动重置的win32事件对象
class ManualResetEvent : public Win32Event
{
public:
	explicit ManualResetEvent(BOOL initState = FALSE, LPCTSTR name = NULL)
	{
		Attach(::CreateEvent(NULL, TRUE, FALSE, name));
		assert(IsValid());
	}
};

/// 自动重置的win32事件对象
class AutoResetEvent : public Win32Event
{
public:
	explicit AutoResetEvent(BOOL initState = FALSE)
	{
		Attach(::CreateEvent(NULL, FALSE, FALSE, NULL));
		assert(IsValid());
	}
};



class WaitHandle
{
public:
	static DWORD WaitAny(const Win32Event* handles, DWORD count, DWORD milliseconds = INFINITE)
	{
		return ::WaitForMultipleObjects(count, reinterpret_cast<const HANDLE*>(handles), FALSE, milliseconds);
	}
};