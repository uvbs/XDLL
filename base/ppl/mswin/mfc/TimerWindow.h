#if !defined(AFX_TIMERWINDOW_H__985C20CC_9F3B_4F9B_ABD4_EBF4AA8057B2__INCLUDED_)
#define AFX_TIMERWINDOW_H__985C20CC_9F3B_4F9B_ABD4_EBF4AA8057B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimerWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TimerWindow window


#pragma warning(disable: 4786)
#include "timer.h"
#include <map>
#include <boost/shared_ptr.hpp>


class TimerWindow;
typedef TimerWindow TimerQueue;
class Timer;



typedef std::map<UINT, Timer*>	TimerCollection;

class TimerWindow : public CWnd
{
// Construction
public:
	TimerWindow();

	/// 进行清理操作
	void Clear();

	//bool Create();

// Attributes
public:
protected:
	TimerCollection	m_timers;
	UINT		m_maxTimerID;
	static TimerWindow* g_tq;

// Operations
public:
	void ScheduleTimer(Timer& timer, UINT interval);
	void CancelTimer(Timer& timer);

	static TimerWindow* Instance();
	static void Startup();
	static void Cleanup();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TimerWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~TimerWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(TimerWindow)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMERWINDOW_H__985C20CC_9F3B_4F9B_ABD4_EBF4AA8057B2__INCLUDED_)
