
#pragma once

#include <ppl/mswin/event.h>
#include <ppl/util/log.h>
#include <ppl/diag/trace.h>

#include <boost/noncopyable.hpp>


/// win32平台的线程类(使用_beginthreadex来创建线程)
class ThreadBase : private boost::noncopyable
{
public:
	ThreadBase();
	virtual ~ThreadBase();

//	struct ThreadParam
//	{
//		Thread* Target;
//		ManualResetEvent Notifier;
//	};

	/// 启动线程
	bool Start()
	{
		return DoStart();
	}
	/// 停止线程
	void Stop(DWORD milliseconds = INFINITE);

	HANDLE GetHandle() const { return m_handle; }
	DWORD GetID() const { return m_id; }
	bool IsValid() const { return m_handle != NULL; }

	bool IsAlive() const { return IsValid() && GetExitCode() == STILL_ACTIVE; }

	void Attach(HANDLE handle, DWORD id)
	{
		assert(!IsValid());
		m_handle = handle;
		m_id = id;
	}

	DWORD GetExitCode() const;

	bool Join(DWORD milliseconds);

	bool Join();

	void Close();

	void Kill();

protected:
	void SafeRun();
	LONG MyExceptFilter(PEXCEPTION_POINTERS excepInfo);

	virtual bool CreateThread() = 0;

	virtual bool DoStart();
	virtual void DoStop(DWORD milliseconds);

protected:
	static void RunThread(void* param);
	virtual void Run() = 0;
	virtual void RequestStop() = 0;

private:
	/// 线程句柄
	HANDLE m_handle;
	/// 线程ID
	unsigned long m_id;

	/// 用于启动线程的事件对象
	ManualResetEvent m_startEvent;
};


#if defined(_AFX)
class AfxThread : public ThreadBase
{
public:
	AfxThread()
	{

	}
	virtual ~AfxThread()
	{
	}

protected:
	virtual bool CreateThread()
	{
		unsigned int id = 0;
		CWinThread* threadobj = AfxBeginThread(&AfxThread::ThreadEntryPoint, this);
		if (threadobj == NULL)
		{
			UTIL_ERROR("AfxThread::CreateThread AfxBeginThread failed " << ::GetLastError());
			assert(false);
			return false;
		}
		assert(threadobj->m_hThread != NULL && threadobj->m_nThreadID != 0);
		HANDLE hThread = NULL;
		BOOL res = ::DuplicateHandle( GetCurrentProcess(), threadobj->m_hThread, GetCurrentProcess(), &hThread, THREAD_ALL_ACCESS, FALSE, DUPLICATE_SAME_ACCESS );
		assert( res );
		Attach(hThread, threadobj->m_nThreadID);
		return true;
	}
	static unsigned int __cdecl ThreadEntryPoint(void* param)
	{
		ThreadBase::RunThread(param);
		return 0;
	}

};

#elif !defined(_WIN32_WCE)

class Win32Thread : public ThreadBase
{
protected:
	virtual bool CreateThread()
	{
		unsigned int id = 0;
		HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &Win32Thread::ThreadEntryPoint, this, 0, &id));
		if (hThread == NULL)
		{
			UTIL_ERROR("Win32Thread::CreateThread _beginthreadex failed " << errno);
			assert(false);
			return false;
		}
		assert(id != 0);
		Attach(hThread, id);
		return true;
	}
	static unsigned int __stdcall ThreadEntryPoint(void* param)
	{
		ThreadBase::RunThread(param);
		return 0;
	}
};

#endif

class WinCEThread : public ThreadBase
{
protected:
	virtual bool CreateThread()
	{
		unsigned long id = 0;
		HANDLE hThread = ::CreateThread(NULL, 0, &WinCEThread::ThreadEntryPoint, this, 0, &id);
		if (hThread == NULL)
		{
			UTIL_ERROR("ThreadBase::CreateThread _beginthreadex failed");
			assert(false);
			return false;
		}
		assert(id != 0);
		Attach(hThread, id);
		return true;
	}
	static unsigned long __stdcall ThreadEntryPoint(void* param)
	{
		ThreadBase::RunThread(param);
		return 0;
	}
};


/// 可运行的对象
class Runnable : private boost::noncopyable
{
public:
	virtual ~Runnable() { }

	virtual void RunRunnable() = 0;
	virtual void RequestStopRunnable() { }
};


/// 空的可运行对象(什么都不做)
class TrivialRunnable : public Runnable
{
public:
	virtual void RunRunnable() { }

	static Runnable& Instance()
	{
		static TrivialRunnable runnable;
		return runnable;
	}
};


#if defined(_AFX)
class Thread : public AfxThread
{
};

#elif !defined(_WIN32_WCE)
class Thread : public Win32Thread
{
};
#else
class Thread : public WinCEThread
{
};
#endif


/// 调用Runnable对象的线程
class RunnableThread : public Thread
{
public:
	RunnableThread() : m_runnable(&TrivialRunnable::Instance()) { }
	~RunnableThread() { Stop(); }

	bool Start(Runnable& runnable);

protected:
	virtual void Run() { m_runnable->RunRunnable(); }
	virtual void RequestStop() { m_runnable->RequestStopRunnable(); }

private:
	Runnable* m_runnable;
};




#if !defined(_WIN32_WCE)
#include <process.h>
#endif



inline ThreadBase::ThreadBase() : m_handle(NULL), m_id(0)
{
}
inline ThreadBase::~ThreadBase()
{
	assert(!IsAlive());
	Close();
}


inline void ThreadBase::Stop(DWORD milliseconds)
{
	if (!IsAlive())
		return;
	DoStop(milliseconds);
}

inline void ThreadBase::DoStop(DWORD milliseconds)
{
	assert (IsAlive());
	RequestStop();
	if (!Join(milliseconds))
	{
		UTIL_ERROR("ThreadBase::DoStop failed to join, then kill " << make_tuple(milliseconds, ::GetLastError()));
		Kill();
	}
}

inline bool ThreadBase::DoStart()
{
	assert(!this->IsAlive());
	//assert(!this->IsValid());
	Stop();
	Close();
	if (!CreateThread())
		return false;
	//m_startEvent.Wait();
	return true;
}


inline DWORD ThreadBase::GetExitCode() const
{
	DWORD code = 0;
	BOOL res = ::GetExitCodeThread(m_handle, &code);
	assert(res);
	return code;
}

inline bool ThreadBase::Join(DWORD milliseconds)
{
	bool res = true;
	if (IsAlive())
	{
		DWORD waitResult = ::WaitForSingleObject(m_handle, milliseconds);
		res = (waitResult == WAIT_OBJECT_0);
	}
	return res;
}

inline bool ThreadBase::Join()
{
	return Join(INFINITE);
}

inline void ThreadBase::Close()
{
	assert(!IsAlive());
	if (!IsValid())
		return;
	if (!::CloseHandle(m_handle))
	{
		UTIL_ERROR("ThreadBase::Close CloseHandle failed " << make_tuple(m_handle, ::GetLastError()));
		assert(false);
	}
	m_handle = NULL;
	m_id = 0;
}

inline void ThreadBase::Kill()
{
	if (IsAlive())
	{
		BOOL success = ::TerminateThread(m_handle, 110);
		TRACE("ThreadBase::Kill 0x%p %lu %d\n", m_handle, m_id, success);
		assert(success);
	}
}

inline void ThreadBase::RunThread(void* param)
{
	if (param == NULL)
	{
		assert(false);
		return;
	}
//	ThreadParam* tp = static_cast<ThreadParam*>(param);
//	ThreadBase* target = tp->Target;
	ThreadBase* target = static_cast<ThreadBase*>(param);
	//target->m_startEvent.Signal();
	//target->m_startEvent.Close();
	target->Run();
}

inline void ThreadBase::SafeRun()
{
	__try
	{
		Run();
	}
	__except (MyExceptFilter(GetExceptionInformation()))
	{
	}
}

inline LONG ThreadBase::MyExceptFilter(PEXCEPTION_POINTERS excepInfo)
{
	::OutputDebugString(TEXT("error occured in thread\n"));
	return EXCEPTION_CONTINUE_SEARCH;
}





inline bool RunnableThread::Start(Runnable& runnable)
{
	assert(!IsAlive());
	Stop();
	m_runnable = &runnable;
	return ThreadBase::Start();
}




