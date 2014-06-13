// FuckIUT.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FuckIUT.h"

#include <ppl/util/macro.h>
#include <ppl/mswin/ipc.h>
#include <ppl/mswin/module.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFuckIUTApp

BEGIN_MESSAGE_MAP(CFuckIUTApp, CWinApp)
	//{{AFX_MSG_MAP(CFuckIUTApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFuckIUTApp construction

CFuckIUTApp::CFuckIUTApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFuckIUTApp object

CFuckIUTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFuckIUTApp initialization

BOOL CFuckIUTApp::InitInstance()
{
	AfxEnableControlContainer();

	STARTUPINFO si; //一些必备参数设置
	memset(&si, 0, sizeof(STARTUPINFO)); 
	si.cb = sizeof(STARTUPINFO); 
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi; //必备参数设置结束

	string baseDir = Module().GetFileDirectory();
	string updateExeName = paths::combine(baseDir, _T("Update.exe"));

	if(!CreateProcess(updateExeName.c_str(),_T("/W1 1 /W2 2 /C"),NULL,FALSE,0,NULL,NULL,NULL,&si,&pi))
	{
		::OutputDebugString(_T("PFSDataActiver Create Process Update.exe Error"));
	}
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// menu
	m_menuTrayIcon.LoadMenu(IDR_MENU1);

	// TrayIcon
	m_pTrayIcon = new TrayIcon(AfxGetInstanceHandle(),3,9,NULL);
	m_pTrayIcon->LoadIcon(1, 0, IDR_MAINFRAME);
	m_pTrayIcon->Mode(1, SSTI_MODE_SHOWNORMAL);

	m_pTrayIcon->MapMessageToFunction(WM_LBUTTONDOWN,TrayIcon::OnMouseClickTI);
	m_pTrayIcon->MapMessageToFunction(WM_LBUTTONDBLCLK,TrayIcon::OnMouseDblClickTI);
	m_pTrayIcon->MapMessageToFunction(WM_RBUTTONDOWN,TrayIcon::OnMouseRightClickTI);
	
	m_pTrayIcon->ShowIcon(1);

	m_pFuckIUTDlg = new CFuckIUTDlg();
	m_pFuckIUTDlg->m_pTrayIcon = m_pTrayIcon;
	m_pFuckIUTDlg->Create(CFuckIUTDlg::IDD, NULL);
	m_pFuckIUTDlg->ShowWindow(SW_HIDE);
	m_pMainWnd = m_pFuckIUTDlg;

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}
