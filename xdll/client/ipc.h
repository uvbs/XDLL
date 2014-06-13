
#pragma once

#include <boost/noncopyable.hpp>

#include <tlhelp32.h>
#include <psapi.h>
#if defined(_WIN32_WCE)
//#pragma comment(lib, "toolhelp.lib")
#else
#pragma comment(lib, "psapi.lib")
#endif

struct KernelObjectHandleCloser
{
	static bool Close(HANDLE handle)
	{
		return ::CloseHandle(handle) != FALSE;
	}
};

template <typename HandleT, typename CloserT>
class HandleWrapper : private boost::noncopyable
{
public:
	HandleWrapper() : m_handle(NullHandle())
	{
	}
	~HandleWrapper()
	{
		Close();
	}

	HandleT GetHandle() const	{ return m_handle; }
	bool IsValid() const		{ return GetHandle() != NullHandle(); }
	void Close()
	{
		if (IsValid())
		{
			CloserT::Close(m_handle);
			m_handle = NullHandle();
		}
	}
	void Attach(HandleT handle)
	{
		Close();
		m_handle = handle;
	}

	static HandleT NullHandle()
	{
		return NULL;
	}

private:
	HandleT	m_handle;
};



class ToolhelpSnapshot : public HandleWrapper<HANDLE, KernelObjectHandleCloser>
{
public:
	void Open(DWORD flags, DWORD pid = 0)
	{
		Attach(::CreateToolhelp32Snapshot(flags, pid));
	}
	void OpenProcesses()
	{
		Open(TH32CS_SNAPPROCESS);
	}
	void OpenHeaps(DWORD pid)
	{
		Open(TH32CS_SNAPHEAPLIST, pid);
	}
};


class ProcessWalker : private boost::noncopyable
{
public:
	ProcessWalker()
	{
		FILL_ZERO(m_item);
		m_item.dwSize = sizeof(m_item);
		m_snapshot.OpenProcesses();
		assert(m_snapshot.IsValid());
		m_isValid = (::Process32First(m_snapshot.GetHandle(), &m_item) != FALSE);
	}

	bool HasMore() const
	{
		return m_isValid;
	}
	const PROCESSENTRY32& Current() const
	{
		return m_item;
	}
	void MoveNext()
	{
		assert(HasMore());
		m_isValid = (::Process32Next(m_snapshot.GetHandle(), &m_item) != FALSE);
	}

private:
	ToolhelpSnapshot	m_snapshot;
	PROCESSENTRY32		m_item;
	bool				m_isValid;
};



// 注意：此类使用Module32First系列API实现，不支持WinNT4
class ModuleWalker : private boost::noncopyable
{
public:
	explicit ModuleWalker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
	{
		FILL_ZERO(m_item);
		m_item.dwSize = sizeof(MODULEENTRY32);
		m_snapshot.Open(TH32CS_SNAPMODULE, pid);
		assert(m_snapshot.IsValid());
		m_isValid = (FALSE != ::Module32First(m_snapshot.GetHandle(), &m_item));
	}
	~ModuleWalker()
	{
	}

	bool HasMore() const
	{
		return m_isValid;
	}
	void MoveNext()
	{
		assert(HasMore());
		m_isValid = (FALSE != ::Module32Next(m_snapshot.GetHandle(), &m_item));
	}
	const MODULEENTRY32& Current() const
	{
		return m_item;
	}

private:
	ToolhelpSnapshot m_snapshot;
	MODULEENTRY32	m_item;
	bool	m_isValid;
};

// 注意：此类使用Module32First系列API实现，不支持WinNT4
class ThreadWalker : private boost::noncopyable
{
public:
	explicit ThreadWalker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
	{
		m_snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
		memset(&m_item, 0, sizeof(THREADENTRY32));
		m_item.dwSize = sizeof(THREADENTRY32);
		if (!IsSnapShotValid())
			return;
		m_isValid = (FALSE != ::Thread32First(m_snapShot, &m_item));
	}
	~ThreadWalker()
	{
		::CloseHandle(m_snapShot);
	}

	bool HasMore() const
	{
		return m_isValid;
	}
	void MoveNext()
	{
		assert(HasMore());
		m_isValid = (FALSE != ::Thread32Next(m_snapShot, &m_item));
	}
	const THREADENTRY32& Current() const
	{
		return m_item;
	}

protected:
	bool IsSnapShotValid() const
	{
		return m_snapShot != INVALID_HANDLE_VALUE;
	}

private:
	HANDLE	m_snapShot;
	THREADENTRY32	m_item;
	bool	m_isValid;
};