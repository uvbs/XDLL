// TimerWindow.cpp : implementation file
//

#include "stdafx.h"
#include "TimerWindow.h"
#include <ppl/util/macro.h>
#include <boost/bind.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TimerWindow

TimerWindow::TimerWindow() : m_maxTimerID(1)
{
	this->CreateEx(0, AfxRegisterWndClass(0), TEXT("Timer Notification Sink"), WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL);
}

//bool TimerWindow::Create()
//{
//	assert(m_hWnd == NULL);
//	return this->CreateEx(0, TEXT("Message"), TEXT("Message"), 0, CRect(), NULL, 0) != FALSE;
//}

TimerWindow::~TimerWindow()
{
	STL_FOR_EACH(TimerCollection, m_timers, iter)
	{
		UINT timerID = iter->first;
		Timer* timer = iter->second;
		assert(timer->GetTimerID() == timerID);
		KillTimer(timerID);
	}
	m_timers.clear();
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(TimerWindow, CWnd)
	//{{AFX_MSG_MAP(TimerWindow)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TimerWindow message handlers

void TimerWindow::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
//	TRACE("OnTimer %d\n", nIDEvent);
	TimerCollection::iterator iter = m_timers.find(nIDEvent);
	if (iter == m_timers.end())
		return;
	Timer* timer = iter->second;
	timer->OnElapsed();
//	CWnd::OnTimer(nIDEvent);
}

void TimerWindow::ScheduleTimer(Timer& timer, UINT interval)
{
	UINT id = SetTimer(m_maxTimerID, interval, NULL);
	timer.Attach(id);
	m_timers[id] = &timer;
	++m_maxTimerID;
}

void TimerWindow::CancelTimer(Timer& timer)
{
	UINT timerID = timer.GetTimerID();
	BOOL success = KillTimer(timerID);
	assert(success);
	int count = m_timers.erase(timerID);
	assert(count == 1);
	timer.Attach(0);
}

void TimerWindow::Clear()
{
	this->DestroyWindow();
}

TimerWindow* TimerWindow::Instance()
{
	return g_tq;
}

TimerWindow* TimerWindow::g_tq = NULL;

void Timer::Start(UINT interval)
{
	assert(!IsStarted());
	Stop();
	DoStart(interval);
}

void Timer::DoStart(UINT interval)
{
	assert(interval > 0);
	assert(interval >= 50);
	m_interval = interval;
	TimerQueue::Instance()->ScheduleTimer(*this, interval);
}

void Timer::Stop()
{
	if (IsStarted())
	{
		TimerQueue::Instance()->CancelTimer(*this);
		m_timerID = 0;
	}
}

void Timer::OnElapsed()
{
	if (IsStarted())
	{
		++m_times;
		m_callback();
	}
}

void OnceTimer::OnElapsed()
{
	if (IsStarted())
	{
		Stop();
		m_callback();
	}
}

void TimerWindow::Cleanup()
{
	//Instance().reset();
	if(g_tq != NULL)
		delete g_tq;
}

void TimerWindow::Startup()
{
	//Instance().reset( new TimerWindow );
	g_tq = new TimerWindow();
}

