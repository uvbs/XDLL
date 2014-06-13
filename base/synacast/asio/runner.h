
#pragma once

#include <synacast/asio/ioservice.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <ppl/boost/thread.h>
#include <ppl/util/random.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

using boost::asio::io_service;


class IOServiceRunner : private boost::noncopyable
{
public:
	static IOServiceRunner& Instance()
	{
		static IOServiceRunner runner(IOService::InstancePtr());
		return runner;
	}

	explicit IOServiceRunner() : m_ioservice(new io_service) { }
	explicit IOServiceRunner(io_service_ptr ioservice) : m_ioservice(ioservice) { }

	io_service& GetService()
	{
		return *m_ioservice;
	}

	io_service_ptr GetServicePtr()
	{
		return m_ioservice;
	}

	void Start()
	{
		assert(!m_thread);
		m_work.reset(new io_service::work(this->GetService()));
		m_thread.reset(new boost::thread(boost::bind(&IOServiceRunner::Run, this)));
	}
	void Stop()
	{
		this->Stop(1000);
	}
	void Stop(UINT milliseconds)
	{
		if ( !m_thread )
			return;
		assert(m_work);
		assert(m_thread);
		m_work.reset();
		m_ioservice->stop();
		if ( m_thread )
		{
			if ( false == m_thread->timed_join(boost::posix_time::milliseconds(milliseconds)) )
			{
				Threads::Kill(*m_thread, 321);
			}
			m_thread.reset();
		}
	}
	bool Join()
	{
		if ( ! m_thread )
			return false;
		m_thread->join();
		return true;
	}


private:
	void Run()
	{
		OldRandomSeed setRandomSeed;
		printf("run io service %lu 0x%p\n", ::GetCurrentThreadId(), this);
		m_ioservice->run();
	}

private:
	io_service_ptr	m_ioservice;
	boost::shared_ptr<io_service::work> m_work;
	boost::shared_ptr<boost::thread> m_thread;
};

class IOServiceRunnerMT : public IOServiceRunner
{
public:
	IOServiceRunnerMT() : IOServiceRunner(io_service_ptr(new io_service))
	{
	}
};



