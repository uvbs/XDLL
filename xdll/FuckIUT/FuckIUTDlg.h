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
	UINT	m_nState;			// ��ǰ״̬��0-��״̬ 1-��ҳ�� 2-�ر�ҳ��

	UINT	m_nFuckWebTimer;	// ������ҳ�Ľ��̵� ��ʱ��
	UINT	m_nPPLiveTimer;		// ִ��PPLive.exe��IUT�� ��ʱ��	
	UINT	m_nSubmitTimer;		// ��������ύ���ݵ� ��ʱ��
	UINT	m_nCheckOKTimer;	// ��鱾��״̬�Ķ�ʱ��
	UINT	m_nUpdateTimer;		// ��ʱ�����Ķ�ʱ��
	UINT	m_nKillPPS;			// ɱ��PPS���̵Ķ�ʱ��

	DWORD	m_dwStartTickCount;	// ��������ʱ��TickCount
	UINT	m_nAuthID;
	string	m_strMail;
	UINT	m_nOK;
	int		m_hasWebOK;			// �����������ҳû�г���ʱ��ͱ��رգ���ô�����Ϊfalse,����Ϊtrue

	HMODULE	m_hModule;			// XDģ��

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
