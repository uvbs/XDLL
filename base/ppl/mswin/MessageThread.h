
#pragma once

#include <ppl/mswin/thread.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

class WindowsMessageThread : public Thread
{
public:
	typedef boost::function0<void> OperationType;

	WindowsMessageThread()
	{
	}
	~WindowsMessageThread()
	{
		this->Stop();
	}

	void SetInitializer(OperationType initOP, OperationType uninitOP)
	{
		m_init = initOP;
		m_uninit = uninitOP;
	}

protected:
	virtual void Run()
	{
		if ( false == m_init.empty() )
		{
			m_init();
		}
		MSG msg;
		while ( ::GetMessage( &msg, NULL, 0, 0 ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
		if ( false == m_uninit.empty() )
		{
			m_uninit();
		}
	}

	virtual void RequestStop()
	{
		::PostThreadMessage( GetID(), WM_QUIT, 0, 0 );
	}

private:
	OperationType m_init;
	OperationType m_uninit;
};


