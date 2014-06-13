
#pragma once

#include <ppl/config.h>

#include <synacast/asio/ioservice.h>

#include <ppl/std/inttypes.h>

#include <ppl/util/timerfwd.h>
#include <ppl/util/listener.h>

//using boost::asio::deadline_timer;
//using boost::posix_time::milliseconds;

using boost::system::error_code;

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/placeholders.hpp>


#if defined(_PPL_PLATFORM_MSWIN) && !defined(_PPL_ASIO_USE_POSIX_TIMER)

#include <synacast/asio/ticktimer.h>

typedef tick_count_timer my_timer_type;
typedef tick_count_traits::duration_type my_time_duration_type;

#else

#include <boost/asio/deadline_timer.hpp>

typedef boost::asio::deadline_timer my_timer_type;
typedef boost::posix_time::milliseconds my_time_duration_type;

#endif

class TimerImpl;






class Timer : public TimerBase//, public Listenable<TimerListener, TrivialTimerListener>
{
public:
	explicit Timer(boost::asio::io_service& inIOSerivce);
	virtual ~Timer();


	/// 启动定时器
	bool Start(UINT elapse);

	/// 停止或取消定时器
	void Stop();

	/// 是否已启动
	virtual bool IsStarted() const = 0;

protected:
	/// 启动定时器
	virtual bool DoStart(UINT elapse) = 0;

	/// 停止或取消定时器
	virtual void DoStop() = 0;

	virtual void OnTimer() = 0;

protected:
	boost::shared_ptr<TimerImpl> m_impl;
};

class OnceTimer : public Timer
{
public:
	explicit OnceTimer(boost::asio::io_service& inIOSerivce = IOService::Instance());
	virtual ~OnceTimer();

	/// 是否已启动
	virtual bool IsStarted() const;

protected:
	/// 启动定时器
	virtual bool DoStart(UINT elapse);

	/// 停止或取消定时器
	virtual void DoStop();

	virtual void OnTimer();

private:
};

class PeriodicTimer : public Timer
{
public:
	explicit PeriodicTimer(boost::asio::io_service& inIOSerivce = IOService::Instance());
	virtual ~PeriodicTimer();

	/// 是否已启动
	virtual bool IsStarted() const;

	UINT GetInterval() const { return m_interval; }

	/// 获取定时器事件触发的次数
	UINT GetTimes() const { return m_times; }

	/// 获取定时器的流逝时间
	UINT64 GetElapsedTime() const { return static_cast<UINT64>(m_times) * m_interval; }

protected:
	/// 启动定时器
	virtual bool DoStart(UINT elapse);

	/// 停止或取消定时器
	virtual void DoStop();

	virtual void OnTimer();

	void DoStartOnce();

protected:
//	my_timer_type::time_type m_starttime;
	my_timer_type::time_type m_firetime;
	UINT m_interval;
	UINT m_times;
	bool m_started;
};

class CImmediatePeriodicTimer : public PeriodicTimer
{
public:
	explicit CImmediatePeriodicTimer(boost::asio::io_service& inIOSerivce = IOService::Instance())
		:PeriodicTimer(inIOSerivce)
	{

	}
	virtual ~CImmediatePeriodicTimer(){};
	virtual bool DoStart(UINT elapse);
protected:
private:
};


