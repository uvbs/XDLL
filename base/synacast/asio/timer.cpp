
//#include "stdafx.h"

#include <synacast/asio/timer.h>

#include <ppl/util/log.h>

#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/noncopyable.hpp>


class TimerImpl : private boost::noncopyable, public boost::enable_shared_from_this<TimerImpl>
{
public:
	typedef boost::function<void ()> CallbackType;

	explicit TimerImpl(boost::asio::io_service& inIOService, CallbackType callback) 
		: m_impl( inIOService )
		, m_callback( callback )
		, m_detached( false )
		, m_started( false )
	{
		APP_ERROR("TimerImpl::TimerImpl " << this);
	}

	~TimerImpl()
	{
		assert(false == m_started);
		assert(m_detached);
		this->Stop();
		APP_ERROR("TimerImpl::~TimerImpl " << this);
	}

	void StartFromNow(UINT elapse)
	{
		assert(false == this->IsStarted());
		APP_ERROR("TimerImpl::StartFromNow " << make_tuple(this, m_started, m_detached, elapse));
		m_impl.expires_from_now(my_time_duration_type(elapse), m_lasterror);
		this->AsyncWait();
	}
	void StartAt(const my_timer_type::time_type& expireTime)
	{
		assert(false == this->IsStarted());
		APP_ERROR("TimerImpl::StartAt " << make_tuple(this, m_started, m_detached));
		m_impl.expires_at(expireTime, m_lasterror);
		this->AsyncWait();
	}
	void Stop()
	{
		APP_ERROR("TimerImpl::Stop " << make_tuple(this, m_started, m_detached));
		if ( m_started )
		{
			m_impl.cancel(m_lasterror);
			m_started = false;
		}
	}

	void Detach()
	{
		APP_ERROR("TimerImpl::Close " << make_tuple(this, m_started, m_detached));
		m_detached = true;
	}

	bool IsStarted() const { return m_started; }

protected:
	void AsyncWait()
	{
		m_impl.async_wait(
			boost::bind(
			&TimerImpl::OnTimerMessage, 
			this->shared_from_this(), 
			boost::asio::placeholders::error
			)
			);
		m_started = true;
	}
	void OnTimerMessage( const boost::system::error_code& err )
	{
		APP_ERROR("TimerImpl::OnTimerMessage " << make_tuple(this, m_started, m_detached, err.value()));
		if ( m_detached )
		{
			assert(false == m_started);
			return;
		}
		if ( err )
		{
			assert( boost::asio::error::operation_aborted == err );
			return;
		}
		if ( false == m_started )
		{
			//assert( m_started );
			return;
		}
		m_started = false;
		m_callback();
	}

protected:
	//tick_count_timer m_impl;
	my_timer_type m_impl;
	CallbackType m_callback;
	boost::system::error_code m_lasterror;
	bool m_started;
	bool m_detached;
};



Timer::Timer(boost::asio::io_service& inIOSerivce)
{
	m_impl.reset( new TimerImpl(inIOSerivce, TimerImpl::CallbackType(boost::bind(&Timer::OnTimer, this))) );
	this->SetListener(NULL);
}

Timer::~Timer()
{
	m_impl->Detach();
}


bool Timer::Start( UINT elapse )
{
	//assert( false == this->IsStarted() );
	this->Stop();
	return DoStart(elapse);
}

void Timer::Stop()
{
	this->DoStop();
}


//void Timer::OnTimerMessage( const error_code& err )
//{
//	if( err == boost::asio::error::operation_aborted )
//		return;
//	this->OnTimer();
//}


OnceTimer::OnceTimer(boost::asio::io_service& inIOSerivce)
: Timer(inIOSerivce)
{
}

OnceTimer::~OnceTimer()
{
	this->Stop();
	assert( false == this->IsStarted() );
}

bool OnceTimer::DoStart( UINT elapse )
{
//	TRACE("OnceTimer::Start %p %d\n", this, elapse);
	this->m_impl->StartFromNow(elapse);
	return true;
}

void OnceTimer::DoStop()
{
	//	TRACE("OnceTimer::DoStop %p %d\n", this, this->m_Started);
	this->m_impl->Stop();
}

bool OnceTimer::IsStarted() const
{
	return m_impl->IsStarted();
}

void OnceTimer::OnTimer()
{
	//	TRACE("Timer::OnTimer %p \n", this);
	this->m_callback();
}



PeriodicTimer::PeriodicTimer(boost::asio::io_service& inIOSerivce)
	: Timer(inIOSerivce), m_started(false), m_times( 0 ), m_interval( 0 )
{
}

PeriodicTimer::~PeriodicTimer()
{
	this->Stop();
	assert( false == this->IsStarted() );
}

bool PeriodicTimer::DoStart( UINT elapse )
{
	assert(elapse >= 50 );
//	TRACE("PeriodicTimer::Start %p %d\n", this, elapse);
//	m_starttime = my_timer_type::traits_type::now();
	m_firetime = my_timer_type::traits_type::now();
	m_interval = elapse;
	m_times = 0;
	this->DoStartOnce();
	m_started = true;
	return true;
}

void PeriodicTimer::DoStop()
{
	m_started = false;
	this->m_impl->Stop();
}

bool PeriodicTimer::IsStarted() const
{
	return m_started;
}

void PeriodicTimer::DoStartOnce()
{
	m_firetime = my_timer_type::traits_type::add(m_firetime, my_time_duration_type(m_interval));
	this->m_impl->StartAt(m_firetime);
}

void PeriodicTimer::OnTimer()
{
	++m_times;
	//	TRACE("Timer::OnTimer %p \n", this);
	assert(m_started);
	if ( m_started )
	{
		this->DoStartOnce();
		this->m_callback();
	}
}

bool CImmediatePeriodicTimer::DoStart(UINT elapse)
{
	m_firetime = my_timer_type::traits_type::now();
	m_interval = elapse;
	m_times = 0;
	this->m_impl->StartAt(m_firetime);
	m_started = true;
	return true;
}



