
#pragma once

#include <ppl/concurrent/thread.h>
#include <boost/function.hpp>


class BoostRunnableThread : public Thread
{
public:
	typedef boost::function0<void> RunnerFunctor;
	typedef boost::function0<void> StopperFunctor;

	BoostRunnableThread() { }
	~BoostRunnableThread()
	{
		this->Stop(500);
	}

	void Init( RunnerFunctor runner, StopperFunctor stopper )
	{
		m_runner = runner;
		m_stopper = stopper;
	}

	void Start( RunnerFunctor runner, StopperFunctor stopper )
	{
		this->Stop( 10 );
		this->Init( runner, stopper );
		Thread::Start();
	}

protected:
	virtual void Run()
	{
		assert( false == m_runner.empty() );
		m_runner();
	}
	virtual void RequestStop()
	{
		if ( false == m_stopper.empty() )
		{
			m_stopper();
		}
	}

private:
	RunnerFunctor m_runner;
	StopperFunctor m_stopper;
};


