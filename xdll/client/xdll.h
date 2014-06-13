// xdll.h : main header file for the XDLL DLL
//

#if !defined(AFX_XDLL_H__EEFFBD83_E822_4498_8475_7244432261A3__INCLUDED_)
#define AFX_XDLL_H__EEFFBD83_E822_4498_8475_7244432261A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include <boost/shared_ptr.hpp>

#include "HookHelper.h"

extern bool g_bIsExit;

class TipsClient;


/////////////////////////////////////////////////////////////////////////////
// CXdllApp
// See xdll.cpp for the implementation of this class
//

class CXdllApp : public CWinApp
{
public:
	CXdllApp();

	int Start();
	int Stop();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXdllApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXdllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL InitATL();

public:
	boost::shared_ptr<TipsClient> m_client;
};

extern CXdllApp theApp;


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XDLL_H__EEFFBD83_E822_4498_8475_7244432261A3__INCLUDED_)
