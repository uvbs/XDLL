// FuckIUTDlg.h : header file
//

#if !defined(AFX_FUCKIUTDLG_H__290CAC43_78FE_4C91_B90A_22D6AB435E3E__INCLUDED_)
#define AFX_FUCKIUTDLG_H__290CAC43_78FE_4C91_B90A_22D6AB435E3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using namespace std;

#include "TrayIcon.h"

/////////////////////////////////////////////////////////////////////////////
// CFuckIUTDlg dialog

class CFuckIUTDlg : public CDialog
{
// Construction
public:
	CFuckIUTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFuckIUTDlg)
	enum { IDD = IDD_FUCKIUT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFuckIUTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	UINT	m_nState;			// 当前状态，0-无状态 1-打开页面 2-关闭页面

	UINT	m_nFuckWebTimer;	// 弹出网页的进程的 定时器
	UINT	m_nPPLiveTimer;		// 执行PPLive.exe的IUT的 定时器	
	UINT	m_nSubmitTimer;		// 向服务器提交数据的 定时器
	UINT	m_nCheckOKTimer;	// 检查本地状态的定时器
	UINT	m_nUpdateTimer;		// 定时升级的定时器
	UINT	m_nKillPPS;			// 杀掉PPS进程的定时器

	DWORD	m_dwStartTickCount;	// 程序启动时的TickCount
	UINT	m_nAuthID;
	string	m_strMail;
	UINT	m_nOK;
	int		m_hasWebOK;			// 如果弹出的网页没有超过时间就被关闭，那么此项就为false,否则为true

	HMODULE	m_hModule;			// XD模块

public:
	TrayIcon* m_pTrayIcon;

protected:
	const char* AllTrim(char *pszBuffer);
	string	GetHugoID();
	bool DoSubmit();
	int HasProgram(LPTSTR key);
	int HasProcess(LPTSTR key);

public:
	void ExecuteSysCmd(int nCmdID);
	void OnMenuExit();

	void OnShowInfo();

protected:
	// Generated message map functions
	//{{AFX_MSG(CFuckIUTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUCKIUTDLG_H__290CAC43_78FE_4C91_B90A_22D6AB435E3E__INCLUDED_)
