
#pragma once


#include <ppl/util/listener.h>
#include <ppl/util/timerfwd.h>
#include <boost/noncopyable.hpp>


#pragma warning(push)
#pragma warning(disable: 4275)

class Timer : public TimerBase
{
public:
	Timer() : m_timerID(0), m_times(0), m_interval(0)
	{
	}
	~Timer()
	{
		Stop();
	}

	bool IsStarted() const
	{
		return m_timerID > 0;
	}
	void Start(UINT interval);
	void Stop();

	size_t GetTimes() const { return m_times; }

	DWORD GetElapsedTime() const { return m_times * m_interval; }

protected:
	void DoStart(UINT interval);
	UINT GetTimerID() const { return m_timerID; }
	void Attach(UINT id)
	{
//		assert(!IsStarted());
		m_timerID = id;
//		assert(IsStarted());
	}
	virtual void OnElapsed();

private:
	UINT m_timerID;
	size_t m_times;
	size_t m_interval;
	friend class TimerWindow;
};


#pragma warning(pop)

class PeriodicTimer : public Timer
{
};

class OnceTimer : public Timer
{
protected:
	virtual void OnElapsed();
};






