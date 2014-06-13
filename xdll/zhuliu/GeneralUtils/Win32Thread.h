#pragma once

class noncopyable
{
public:
	noncopyable() { }
	~noncopyable() { }
private:
	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&);
};

class ManualResetEvent : private noncopyable
{
public:
	explicit ManualResetEvent(BOOL initState = FALSE)
	{
		m_handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	~ManualResetEvent()
	{
		if (IsValid())
		{
			::CloseHandle(m_handle);
			m_handle = NULL;
		}
	}
	bool IsValid() const { return m_handle != NULL; }

	BOOL Signal(){return ::SetEvent(m_handle);}

	BOOL Reset(){return ::ResetEvent(m_handle);	}

	BOOL Pulse(){return ::PulseEvent(m_handle);	}

	DWORD Wait(DWORD milliseconds = INFINITE)
	{return ::WaitForSingleObject(m_handle, milliseconds);}

private:
	HANDLE	m_handle;
};

class Win32Thread : private noncopyable
{
public:
	Win32Thread(): m_handle(NULL), m_id(0){}

	virtual ~Win32Thread(){	Close();}

	struct ThreadParam
	{
		Win32Thread* Target;
		ManualResetEvent Notifier;
		LPVOID lpParam;
	};

	void Start(LPVOID lpParam = NULL)	{DoStart(lpParam);}

	void Win32Thread::Stop(DWORD milliseconds)
	{
		if (!IsAlive())
			return;
		DoStop(milliseconds);
	}

	HANDLE GetHandle() const { return m_handle; }

	DWORD GetID() const { return m_id; }

	bool IsValid() const { return m_handle != NULL; }

	bool IsAlive() const { return IsValid() && GetExitCode() == STILL_ACTIVE; }

	void Attach(HANDLE handle, DWORD id)
	{
		m_handle = handle;
		m_id = id;
	}


	DWORD Win32Thread::GetExitCode() const
	{
		DWORD code = 0;
		BOOL res = ::GetExitCodeThread(m_handle, &code);
		return code;
	}

	bool Win32Thread::Join(DWORD milliseconds)
	{
		bool res = true;
		if (IsAlive())
		{
			DWORD waitResult = ::WaitForSingleObject(m_handle, milliseconds);
			res = (waitResult == WAIT_OBJECT_0);
		}
		return res;
	}

	bool Win32Thread::Join(){return Join(INFINITE);	}

	void Win32Thread::Close()
	{
		if (!IsValid())
			return;
		::CloseHandle(m_handle);
		m_handle = NULL;
		m_id = 0;
	}


	void Win32Thread::Kill()
	{
		if (IsAlive())
		{
			::TerminateThread(m_handle, 110);
		}
	}

protected:

	virtual void CreateThread(ThreadParam& param);

	virtual void DoStart(LPVOID lpParam);

	virtual void DoStop(DWORD milliseconds);

	static unsigned int __stdcall ThreadEntryPoint(void* param);

protected:
	virtual void Run( LPVOID lpParam) { }
	virtual void RequestStop() { }

private:
	HANDLE m_handle;     /// 线程句柄
	unsigned	m_id;    /// 线程ID

};

inline void Win32Thread::DoStart(LPVOID lpParam)
{
	Stop(INFINITE);
	Close();
	ThreadParam param;
	param.Target = this;
	param.lpParam = lpParam;
	CreateThread(param);
	param.Notifier.Wait();
}

inline unsigned int Win32Thread::ThreadEntryPoint(void* param)
{
	if (param == NULL)	return 0;
	ThreadParam* tp = static_cast<ThreadParam*>(param);
	Win32Thread* target = tp->Target;
	void* lpParam = tp->lpParam;
	tp->Notifier.Signal();
	target->Run(lpParam);
	return 0;
}

inline void Win32Thread::CreateThread(ThreadParam& param)
{
	unsigned int id = 0;
	HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ThreadEntryPoint, &param, 0, &id));
	Attach(hThread, id);
}

inline void Win32Thread::DoStop(DWORD milliseconds)
{
	RequestStop();
	if (!Join(milliseconds))
	{
		Kill();
	}
}

