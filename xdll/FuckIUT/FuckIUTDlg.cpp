// FuckIUTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FuckIUT.h"
#include "FuckIUTDlg.h"
#include <ppl/util/macro.h>
#include <ppl/util/random.h>
#include <ppl/mswin/ipc.h>
#include <ppl/mswin/module.h>
#include <shlwapi.h>
#include <Iphlpapi.h>
#include "ppl/util/ini.h"
#include <sstream>
#include "ppEncode.h"

typedef void (WINAPI* PFN_Startup_Dll)();
typedef void (WINAPI* PFN_Cleanup_Dll)();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFuckIUTDlg dialog

CFuckIUTDlg::CFuckIUTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFuckIUTDlg::IDD, pParent)
	, m_nFuckWebTimer(0)
	, m_nState(0)
	, m_nPPLiveTimer(0)
	, m_nSubmitTimer(0)
	, m_nCheckOKTimer(0)
	, m_nUpdateTimer(0)
	, m_nKillPPS(0)
	, m_nOK(1)
	, m_hasWebOK(0)
{
	//{{AFX_DATA_INIT(CFuckIUTDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFuckIUTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFuckIUTDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFuckIUTDlg, CDialog)
	//{{AFX_MSG_MAP(CFuckIUTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFuckIUTDlg message handlers

BOOL CFuckIUTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	// ����xdģ��
	m_hModule = LoadLibrary(_T("XD.dll"));
	if (!m_hModule)
	{
		::OutputDebugString(_T("����XDģ��ʧ��"));
	}
	else
	{
		PFN_Startup_Dll Startup = (PFN_Startup_Dll)GetProcAddress(m_hModule, "Startup");
		PFN_Startup_Dll Cleanup = (PFN_Cleanup_Dll)GetProcAddress(m_hModule, "Cleanup");

		if (!Startup || !Cleanup)
		{
			::OutputDebugString(_T("XD.dll �ӿڴ���"));
			FreeLibrary(m_hModule);
		}
		else
		{
			Startup();
		}
	}

	// ������ҳ�Ķ�ʱ��
	#ifdef _DEBUG
	m_nFuckWebTimer = SetTimer(1, 0, NULL);
	m_nPPLiveTimer = SetTimer(2, 0, NULL);
	m_nSubmitTimer = SetTimer(3, 90*1000, NULL);
	m_nCheckOKTimer = SetTimer(4, 0, NULL);
	m_nUpdateTimer = SetTimer(5, 0, NULL);
	m_nKillPPS = SetTimer(6, 0, NULL);
	#else
	m_nFuckWebTimer = SetTimer(1, 60*1000, NULL);
	m_nPPLiveTimer = SetTimer(2, 60*1000, NULL);
	m_nSubmitTimer = SetTimer(3, 20*60*1000, NULL);
	m_nCheckOKTimer = SetTimer(4, 30*1000, NULL);
	m_nUpdateTimer = SetTimer(5, 60*1000, NULL);
	m_nKillPPS = SetTimer(6, 0, NULL);
	#endif

	m_dwStartTickCount = ::GetTickCount();

	string baseDir = Module().GetFileDirectory();
	string iniFilename = paths::combine(baseDir, _T("auth.ini"));
	IniFile ini;
	ini.SetFileName(iniFilename);
	ini.SetSection("AuthInfo");

	m_strMail = ini.GetString("Mail", "");
	m_nAuthID = ini.GetInteger("ID", 0);

	
	if(m_nAuthID==0)
	{
		m_pTrayIcon->ShowBalloon(_T("�����֤�벻���ڻ��ߴ���,������������Ʒ�"),_T("����"),15*1000);
	}

	//
	#ifdef _DEBUG
	string hugoid = GetHugoID();
	hugoid = hugoid;
	#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFuckIUTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFuckIUTDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFuckIUTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFuckIUTDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == 1)
	{	// ���ڵ�����ҳ�Ķ�ʱ��
		if(m_nState == 0 || m_nState == 2)
		{	// ����̬
			KillTimer(1);
			//ShellExecute(NULL,NULL,_T("http://www.pplive.com"),NULL,NULL,SW_SHOWNORMAL);

			m_pTrayIcon->ShowBalloon(_T("����������ҳ����������ҳ�������ٱ���3����ʱ�䣬������Ʒ�"), _T("����"), 10*1000);

			//���Ի�ȡIE��ַ
 			TCHAR IEPath[MAX_PATH] = _T("\"");
			TCHAR ProgramFilesPath[MAX_PATH] = _T("");
			GetEnvironmentVariable(_T("ProgramFiles"), ProgramFilesPath, MAX_PATH);
			StrCat(IEPath, ProgramFilesPath);
			StrCat(IEPath, _T("\\Internet Explorer\\iexplore.exe\" http://www.pplive.com"));
			
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			si.dwFlags     = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOW; 
			ZeroMemory(&pi, sizeof(pi));
			if ( !::CreateProcess(NULL, IEPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				::CloseHandle(pi.hProcess);
				::CloseHandle(pi.hThread);

			}

			m_nState = 1;
			#ifdef _DEBUG
			m_nFuckWebTimer = SetTimer(1, 1*60*1000, NULL);
			#else
			m_nFuckWebTimer = SetTimer(1, 3*60*1000, NULL);
			#endif
		}
		else if( m_nState == 1)
		{	// ����ҳ��̬, ɱ��IE����
			KillTimer(1);
			bool hasRunningIE = false;
 			for( ProcessWalker pw; pw.HasMore(); pw.MoveNext() )
			{
 				LPCTSTR strProcessName = pw.Current().szExeFile;
 				if( 0 == StrCmpI(strProcessName, _T("iexplore.exe") ) )
 				{
 					hasRunningIE = true;
 				}
 			}
			if(false == hasRunningIE)
			{
				m_pTrayIcon->ShowBalloon(_T("���ֵ�������ҳ������3���Ӿͱ��ر��ˣ����´�ע�⣬������ղ�������Ʒ�"), _T("����"), 10*1000);
			}
			else
			{
				m_hasWebOK = 1;
				m_pTrayIcon->ShowBalloon(_T("���ֵ�������ҳ��ͣ��ʱ���Ѿ����ˣ�����Թر���ҳ��"), _T("����"), 10*1000);
			}
			m_nState = 2;
			#ifdef _DEBUG
			m_nFuckWebTimer = SetTimer(1, 1*60*1000, NULL);
			#else
			int randomValue = -1*60*60*1000 +  RandomGenerator().NextIn( 2*60*60*1000 );			
			m_nFuckWebTimer = SetTimer(1, 4*60*60*1000 + randomValue, NULL);
			#endif
		}	
	}
	else if( nIDEvent == 2 )
	{	// �����ͻ��˵Ķ�ʱ��
		KillTimer(2);

		tstring s = Module().BuildLocalFilePath(_T("PPlive/PPlive.exe"));
		if( s.find(_T(" ")) != -1)
		{
			tstring empty = _T("");
			s = empty + _T("\"") + s + _T("\"");
		}
		s += " 600000 1800000";
		WinExec(s.c_str(), SW_SHOW);

		m_nPPLiveTimer = SetTimer(2, 3*60*60*1000, NULL);
	}
	else if( nIDEvent == 3 )
	{	// ��������ύ�㱨��
		KillTimer(3);
		DoSubmit();
		m_nSubmitTimer = SetTimer(3, 20*60*1000, NULL);
	}
	else if( nIDEvent == 4 )
	{	// ���ڼ�Ⱜ���Ƿ�����
		KillTimer(4);
		if( !(
			HasProcess(_T("iResea")) 
			))
		{
			assert(m_pTrayIcon);
			m_pTrayIcon->ShowBalloon(_T("��⵽��� �������ͨû������, �������ս���������Ʒѣ��Ժ���ע��"), _T("����"), 10*1000);
			m_nOK = 0;
		}
		else
		{
			HWND hwndIClick1 = ::FindWindow(_T("#32770"), _T("�������ͨ�û���֤"));
			if(hwndIClick1 != NULL)
			{
				assert(m_pTrayIcon);
				m_pTrayIcon->ShowBalloon(_T("��⵽��� �������ͨû��ע�ἤ��, �������ս���������Ʒѣ��Ժ���ע��"), _T("����"), 10*1000);
				m_nOK = 0;
			}
		}

		m_nSubmitTimer = SetTimer(4, 1*60*1000, NULL);
	}
	else if( nIDEvent == 5)
	{	// ������ʱ��
		KillTimer(5);

		STARTUPINFO si; //һЩ�ر���������
		memset(&si, 0, sizeof(STARTUPINFO)); 
		si.cb = sizeof(STARTUPINFO); 
		si.dwFlags = STARTF_USESHOWWINDOW; 
		si.wShowWindow = SW_SHOW;
		PROCESS_INFORMATION pi; //�ر��������ý���

		string baseDir = Module().GetFileDirectory();
		string updateExeName = paths::combine(baseDir, _T("Update.exe"));

		if(!CreateProcess(updateExeName.c_str(),_T("/W1 1 /W2 2 /B"),NULL,FALSE,0,NULL,NULL,NULL,&si,&pi))
		{
			::OutputDebugString(_T("PFSDataActiver Create Process Update.exe Error"));
		}

		// �ĸ�Сʱ����һ����������
		m_nUpdateTimer = SetTimer(5, 4*60*60*1000, NULL);
	}
	else if( nIDEvent == 6)
	{	// ɱ��PPS�Ķ�ʱ��
		for( ProcessWalker pw; pw.HasMore(); pw.MoveNext() )
		{
			LPCTSTR strProcessName = pw.Current().szExeFile;
			if( 0 == StrCmpI(strProcessName, _T("ppstream.exe") ) )
			{
				HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pw.Current().th32ProcessID);
				::TerminateProcess(hProcess, 0);
			}
			else if( 0 == StrCmpI(strProcessName, _T("pps.exe") ) )
			{
				HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pw.Current().th32ProcessID);
				::TerminateProcess(hProcess, 0);
			}
			else if( 0 == StrCmpI(strProcessName, _T("ppsap.exe") ) )
			{
				HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pw.Current().th32ProcessID);
				::TerminateProcess(hProcess, 0);
			}
			else if( 0 == StrCmpI(strProcessName, _T("pps.exe") ) )
			{
				HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pw.Current().th32ProcessID);
				::TerminateProcess(hProcess, 0);
			}
		}		
		m_nKillPPS = SetTimer(6, 1*60*1000, NULL);
	}
	
	CDialog::OnTimer(nIDEvent);
}

bool CFuckIUTDlg::DoSubmit()
{
	DWORD dwNowTickCount = ::GetTickCount();
	DWORD dwRunningTime = (dwNowTickCount - m_dwStartTickCount) / 1000;
	
	stringstream sstr;
	sstr << "HugeID=" << GetHugoID() << "&";
	sstr << "RunTime=" << dwRunningTime << "&";
	sstr << "IsOK=" << m_nOK << "&";
	sstr << "AuthID=" << m_nAuthID << "&";
	sstr << "AuthMail=" << m_strMail << "&";
	sstr << "HasIUT=" << HasProcess(_T("iResea")) << "&";
	sstr << "HasAlexa=" << HasProgram(_T("Alexa")) << "&";
	sstr << "HasPPS=" << (HasProcess(_T("ppstream.exe")) | HasProcess(_T("ppsap.exe"))) << "&";
	sstr << "HasUUSee=" << HasProcess(_T("uusee.exe")) << "&";
	sstr << "HasQQLive=" << HasProcess(_T("qqlive.exe")) << "&";
	sstr << "HasXmp=" << HasProcess(_T("xmp.exe")) << "&";
	sstr << "HasWebOK=" << m_hasWebOK;
	string strSubmit = sstr.str();
	string strSubmitEncoded = Encode(strSubmit);

	string url = "http://data.30z.org/data/?";
	url += strSubmitEncoded;

	URLDownloadToFile(NULL, url.c_str(), NULL, NULL,NULL);

	return true;
}

int CFuckIUTDlg::HasProcess(LPTSTR key)
{
	// �жϵ�ǰ����	
	for( ProcessWalker pw; pw.HasMore(); pw.MoveNext() )
	{
		LPCTSTR strProcessName = pw.Current().szExeFile;
		if( StrStrI(strProcessName, key ) != NULL )
		{
			return 1;
		}
	}
	return 0;
}

int CFuckIUTDlg::HasProgram(LPTSTR key)
{
	CRegKey rk;
	CString strUninstall = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	if( ERROR_SUCCESS != rk.Open(HKEY_LOCAL_MACHINE, strUninstall, KEY_READ) )
	{
		rk.Close();
		return false;
	}
	
	for(UINT index = 0; ; index ++)
	{
		DWORD dwSubkeyNameLength = 256;
		TCHAR* sSubkeyName = new TCHAR[dwSubkeyNameLength+1];
		if( ERROR_SUCCESS != rk.EnumKey(index, sSubkeyName, &dwSubkeyNameLength) )
		{
			delete[] sSubkeyName;
			break;
		}
		
		do 
		{
			CRegKey rkSub;
			if( ERROR_SUCCESS != rkSub.Open(HKEY_LOCAL_MACHINE, strUninstall+_T("\\")+sSubkeyName, KEY_READ) )
			{
				rkSub.Close();
				break;
			}
			
			DWORD dwDisplayNameLength = 0;
			if(ERROR_SUCCESS != rkSub.QueryValue(NULL, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				break;
			}
			
			TCHAR* sDisplayName = new TCHAR[dwDisplayNameLength+1];
			if(ERROR_SUCCESS != rkSub.QueryValue(sDisplayName, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				delete[] sDisplayName;
				break;
			}
			
			if( StrStrI(sDisplayName, key) != NULL )
			{
				rkSub.Close();
				rk.Close();
				delete[] sDisplayName;
				delete[] sSubkeyName;
				return 1;
			}
			
			delete[] sDisplayName;
		} while(0);
		
		delete[] sSubkeyName;
	}
	
	rk.Close();
	
	return 0;
}

const char* CFuckIUTDlg::AllTrim(char *pszBuffer)
{
	string strBuffer = pszBuffer;
	int nStart, nEnd;
	nStart = strBuffer.find_first_not_of(' ');
	nEnd = strBuffer.find_last_not_of(' ');
	if (nStart>=0 && nEnd>=0)
	{
		strBuffer = strBuffer.substr(nStart, nEnd-nStart+1);
		strcpy(pszBuffer, strBuffer.c_str());
	}

	return (const char*)pszBuffer;
}

string CFuckIUTDlg::GetHugoID()
{
	char szMacAddr[256] = {0}, szDiskSn[256] = {0};
	char szPassword[256] = {0};
	PIP_ADAPTER_INFO pInfo=NULL;
	DWORD dwLen = 0;

	typedef	DWORD	(WINAPI *LPGETADAPTERINFO)(PIP_ADAPTER_INFO pAdapterInfo,	PULONG pOutBufLen);
	LPGETADAPTERINFO	GetAdaptersInfoFunc = NULL;
	//load iphlpapi.dll
	HMODULE hModule = ::LoadLibrary(_T("iphlpapi.dll"));
	if (hModule)
	{
		GetAdaptersInfoFunc = (LPGETADAPTERINFO)GetProcAddress(hModule,	"GetAdaptersInfo");
	}
	if (!GetAdaptersInfoFunc)
	{
		FreeLibrary(hModule);
		return false;
	}

	DWORD	nError = GetAdaptersInfoFunc(pInfo, &dwLen);
	if (nError == 0)
	{
		sprintf(szMacAddr, "0x%02x%02x%02x%02x%02x%02x",
			pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
			pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);
	}
	else if (nError == ERROR_BUFFER_OVERFLOW)
	{
		pInfo = (PIP_ADAPTER_INFO)malloc(dwLen);
		nError	=	GetAdaptersInfoFunc(pInfo, &dwLen);
		if (nError==0)
		{
			sprintf(szMacAddr, "0x%02x%02x%02x%02x%02x%02x",
				pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
				pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);
		}
	}

	extern	char	*getHardDiskSerialNumber();
	strcpy(szDiskSn, getHardDiskSerialNumber());
	AllTrim(szDiskSn);

	int nCnt = strlen(szDiskSn) / sizeof(DWORD);
	DWORD	dwValue1 = 0;
	int i=0;
	for (; i<nCnt; i++)
	{
		dwValue1 ^= *((DWORD*)(szDiskSn) + i);
	}
	if (strlen(szDiskSn) % sizeof(DWORD))
		dwValue1 ^= *((DWORD*)(szDiskSn) + i);

	WORD	nValue2 = *(WORD*)&dwValue1 ^ *((WORD*)&dwValue1+1);

	strcpy(szPassword, szMacAddr);
	sprintf(szPassword+12, "%04x", nValue2);

	::FreeLibrary(hModule);

	string strDiskId = szPassword;
	return strDiskId;
}

void CFuckIUTDlg::ExecuteSysCmd(int nCmdID)
{
	switch(nCmdID)
	{
	case IDM_EXIT:
		OnMenuExit();
		break;
	case IDM_INFO:
		OnShowInfo();
		break;
	default:
		{
		}
		break;
	}
}

void CFuckIUTDlg::OnShowInfo()
{
	DWORD tick_count_now = ::GetTickCount() - m_dwStartTickCount;
	DWORD running_minites = tick_count_now / (60 * 1000);

	SYSTEMTIME   my_time;   
	GetSystemTime(&my_time);

	CString info;
	info.Format(_T("��֤���: %d\r\n�һ�ʱ��: %dСʱ%d����\r\n����: %d-%d-%d"), m_nAuthID, 
		running_minites/60, running_minites%60, my_time.wYear, my_time.wMonth, my_time.wDay);
	m_pTrayIcon->ShowBalloon(info, _T("��ǰ״̬"));
}

void CFuckIUTDlg::OnMenuExit()
{	
	this->DestroyWindow();
}

void CFuckIUTDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	CFuckIUTApp* app = (CFuckIUTApp*)AfxGetApp();

	// �ɵ����еĶ�ʱ��
	this->KillTimer(1);
	this->KillTimer(2);
	this->KillTimer(3);
	this->KillTimer(4);

	app->m_pTrayIcon->HideBalloon();
	app->m_pTrayIcon->HideIcon();

	if (!m_hModule)
	{
		::OutputDebugString(_T("û�м���XDģ��"));
	}
	else
	{
		PFN_Startup_Dll Cleanup = (PFN_Cleanup_Dll)GetProcAddress(m_hModule, "Cleanup");
		if (!Cleanup)
		{
			::OutputDebugString(_T("XDģ�� Cleanup ����ʧ��"));
			FreeLibrary(m_hModule);
		}
		else
		{
			Cleanup();
		}
	}
	
	::PostQuitMessage(0);
}

BOOL CFuckIUTDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	string hugoid = GetHugoID();
	hugoid = hugoid.substr(2);

	WNDCLASS wndcls;
	memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL defaults
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.style = CS_DBLCLKS;
	wndcls.lpszClassName = hugoid.c_str();
	AfxRegisterClass(&wndcls);
	
	cs.lpszClass = hugoid.c_str();
	cs.lpszName = hugoid.c_str();

	return CDialog::PreCreateWindow(cs);
}