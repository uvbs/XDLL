// FuckIUT.h : main header file for the FUCKIUT application
//

#if !defined(AFX_FUCKIUT_H__7F3E4E52_DD87_4825_83B1_79D8BD115C28__INCLUDED_)
#define AFX_FUCKIUT_H__7F3E4E52_DD87_4825_83B1_79D8BD115C28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "FuckIUTDlg.h"
#include "TrayIcon.h"

/////////////////////////////////////////////////////////////////////////////
// CFuckIUTApp:
// See FuckIUT.cpp for the implementation of this class
//

class CFuckIUTApp : public CWinApp
{
public:
	CFuckIUTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFuckIUTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFuckIUTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	TrayIcon* m_pTrayIcon;
	CMenu m_menuTrayIcon;
	CFuckIUTDlg* m_pFuckIUTDlg;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUCKIUT_H__7F3E4E52_DD87_4825_83B1_79D8BD115C28__INCLUDED_)
