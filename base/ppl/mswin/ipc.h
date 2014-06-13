
#pragma once

#include <boost/noncopyable.hpp>

#include <tlhelp32.h>
#include <psapi.h>
#if defined(_WIN32_WCE)
//#pragma comment(lib, "toolhelp.lib")
#else
#pragma comment(lib, "psapi.lib")
#endif




/*
class ProcessEnum
{
public:
	enum { MAX_PROCESS_COUNT = 1024 };
	DWORD		m_ids[MAX_PROCESS_COUNT];
	DWORD		m_count;

	ProcessEnum() : m_count(0)
	{
		DWORD bytes = 0;
		if (!::EnumProcesses(m_ids, MAX_PROCESS_COUNT * sizeof(DWORD), &bytes))
		{
			APP_ERROR("EnumProcesses failed " << ::GetLastError());
			return;
		}
		assert(bytes % 4 == 0);
		m_count = bytes / 4;
	}

	DWORD GetCount() const
	{
		return m_count;
	}

	DWORD FindProcess(LPCTSTR name)
	{
		assert(m_count > 0);
		DWORD id = 0;
		for (DWORD i = 0; i < m_count; ++i)
		{
			ProcessObject obj(m_ids[i]);
			if (!obj.IsValid())
				continue;
			tstring path = obj.GetBaseName();
			if (stricmp(path.c_str(), name) == 0)
			{
				id = m_ids[i];
				break;
			}
		}
		return id;
	}
};
*/


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

/*
class ThreadObject
{
public:
	explicit ThreadObject(DWORD threadID, DWORD access = THREAD_SUSPEND_RESUME) : m_handle(NULL)
	{
		m_handle = ::OpenThread(access, FALSE, threadID);
	}
	~ThreadObject()
	{
		if (m_handle != NULL)
		{
			::CloseHandle(m_handle);
			m_handle = NULL;
		}
	}
	HANDLE GetHandle()
	{
		return m_handle;
	}
	void Suspend()
	{
		::SuspendThread(m_handle);
	}
private:
	HANDLE m_handle;
};
*/

/*
class FileFinder : private boost::noncopyable
{
public:
	FileFinder() : m_handle(INVALID_HANDLE_VALUE), m_hasMore(false)
	{
	}
	~FileFinder()
	{
		Close();
	}

	void Close()
	{
		if (IsValid())
		{
			::FindClose(m_handle);
			m_handle = INVALID_HANDLE_VALUE;
			m_hasMore = false;
		}
	}
	bool IsValid() const
	{
		return m_handle != INVALID_HANDLE_VALUE;
	}

	bool Find(LPCTSTR filename)
	{
		Close();
		m_handle = ::FindFirstFile(filename, &m_item);
		if (!IsValid())
			return false;
		m_hasMore = true;
		return true;
	}

	bool HasMore() const
	{
		return m_hasMore;
	}
	void MoveNext()
	{
		assert(HasMore());
		m_hasMore = (FALSE != ::FindNextFile(m_handle, &m_item));
	}
	const WIN32_FIND_DATA& Current() const
	{
		return m_item;
	}


	static bool Exists( const TCHAR* path )
	{
		FileFinder finder;
		return finder.Find( path );
	}

	static bool DirectoryExists( const TCHAR* path )
	{
		FileFinder finder;
		if ( false == finder.Find( path ) )
			return false;
		DWORD attr = finder.Current().dwFileAttributes;
		return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	static bool FileExists( const TCHAR* path )
	{
		FileFinder finder;
		if ( false == finder.Find( path ) )
			return false;
		DWORD attr = finder.Current().dwFileAttributes;
		return (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}


private:
	HANDLE	m_handle;
	WIN32_FIND_DATA	m_item;
	bool	m_hasMore;
};
*/

