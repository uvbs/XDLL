
#pragma once

#include <ppl/util/listener.h>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include <assert.h>


class Timer;
typedef boost::function0<void> TimerCallbackType;


/// 定时器的事件接口
class TimerListener
{
public:
	virtual ~TimerListener() { }

	/// Timer到期
	virtual void OnTimerElapsed(Timer* sender) = 0;
};



/// 定时器的事件接口
class TrivialTimerListener : public TimerListener
{
public:
	virtual void OnTimerElapsed(Timer* sender) { assert(!"Unhandled Timer Message."); }
};


PPL_DEFINE_LISTENABLE(TimerListenable, TimerListener, TrivialTimerListener);



class TimerBase : private boost::noncopyable
{
public:
	TimerBase()
	{
		m_callback = TimerCallbackType(&TimerBase::DefaultHandler);
	}
	virtual ~TimerBase() { }

	void SetListener(TimerCallbackType listener)
	{
		m_callback = listener;
	}

protected:
	static void DefaultHandler()
	{
		assert(!"Unhandled Timer Message.");
	}

protected:
	TimerCallbackType m_callback;
};
