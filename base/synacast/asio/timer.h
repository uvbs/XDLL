
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


	/// ������ʱ��
	bool Start(UINT elapse);

	/// ֹͣ��ȡ����ʱ��
	void Stop();

	/// �Ƿ�������
	virtual bool IsStarted() const = 0;

protected:
	/// ������ʱ��
	virtual bool DoStart(UINT elapse) = 0;

	/// ֹͣ��ȡ����ʱ��
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

	/// �Ƿ�������
	virtual bool IsStarted() const;

protected:
	/// ������ʱ��
	virtual bool DoStart(UINT elapse);

	/// ֹͣ��ȡ����ʱ��
	virtual void DoStop();

	virtual void OnTimer();

private:
};

class PeriodicTimer : public Timer
{
public:
	explicit PeriodicTimer(boost::asio::io_service& inIOSerivce = IOService::Instance());
	virtual ~PeriodicTimer();

	/// �Ƿ�������
	virtual bool IsStarted() const;

	UINT GetInterval() const { return m_interval; }

	/// ��ȡ��ʱ���¼������Ĵ���
	UINT GetTimes() const { return m_times; }

	/// ��ȡ��ʱ��������ʱ��
	UINT64 GetElapsedTime() const { return static_cast<UINT64>(m_times) * m_interval; }

protected:
	/// ������ʱ��
	virtual bool DoStart(UINT elapse);

	/// ֹͣ��ȡ����ʱ��
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


