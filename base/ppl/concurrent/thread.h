
#pragma once

#include <ppl/config.h>

#include <ppl/concurrent/lock.h>

#include <boost/scoped_ptr.hpp>



#if defined(_PPL_PLATFORM_LINUX) || defined(_PPL_USE_ASIO)


#include <boost/thread.hpp>

#include <ppl/boost/thread.h>


class Thread : private noncopyable
{
public:
	Thread()
	{
	}
	~Thread()
	{
		assert(false == IsStarted());
	}

	void Start()
	{
		assert(false == IsStarted());
		m_thread.reset( new boost::thread( boost::bind( &Thread::Run, this ) ) );
	}
	void Stop(UINT milliseconds)
	{
		if ( IsStarted() )
		{
			this->RequestStop();
			if ( false == this->Join( milliseconds ) )
			{
				this->Kill();
			}
			m_thread.reset();
		}
	}
	void Stop()
	{
		this->Stop(0xFFFFFFFF);
	}

	bool Join(UINT milliseconds)
	{
		assert(m_thread);
		if ( !m_thread )
			return true;
		return m_thread->timed_join(boost::posix_time::milliseconds(milliseconds));
	}

	void Kill()
	{
		if ( m_thread )
		{
			Threads::Kill(*m_thread, 321);
		}
	}

	bool IsStarted() const
	{
		return m_thread;
	}
	bool IsAlive() const
	{
		if ( !m_thread )
			return false;
		return 0 == pthread_kill( m_thread->native_handle(), 0 );
	}


protected:
	virtual void Run() = 0;
	virtual void RequestStop() = 0;


private:
	boost::scoped_ptr<boost::thread> m_thread;
};

#elif defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/thread.h>


#endif



