// xdll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "xdll.h"

#include "TipsClient.h"
#include "TipsClientCsOperator.h"
#include "ExceptFilter.h"
#include <ppl/diag/trace.h>

#include <initguid.h>
#include "Xdll_i.c"
#include "gstring.h"
#include "HttpProtocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdio.h>

CXdllApp theApp;
CComModule _Module;
bool g_bIsExit = false;
bool g_bIsRunning = false;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_TipsClientCsOperator, CTipsClientCsOperator)
OBJECT_ENTRY(__uuidof(CLSID_MyHttpProtocol), CHttpProtocol)
END_OBJECT_MAP()

STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}
/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer(TRUE); //TRUE indicates that typelib is unreg'd
	return S_OK;
}


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CXdllApp

BEGIN_MESSAGE_MAP(CXdllApp, CWinApp)
	//{{AFX_MSG_MAP(CXdllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXdllApp construction

CXdllApp::CXdllApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXdllApp object

/////////////////////////////////////////////////////////////////////////////
// CXdllApp initialization

BOOL CXdllApp::InitInstance()
{
// 	if (!AfxSocketInit())
// 	{
// 		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
// 		return FALSE;
// 	}

	AfxEnableControlContainer();
	if (!InitATL())
		return FALSE;

	return TRUE;
}

int CXdllApp::ExitInstance() 
{
	#ifndef XDLL_LIB
	_Module.Term();
	#endif
	
	return CWinApp::ExitInstance();
}

int CXdllApp::Start()
{
	char module_path[MAX_PATH];
	DWORD dw  = ::GetModuleFileName(AfxGetInstanceHandle(), module_path, MAX_PATH);
	if( dw > 0 && GetLastError() != ERROR_INSUFFICIENT_BUFFER )
	{
		::LoadLibrary(module_path);
	}
	m_client.reset( new TipsClient );
	m_client->Start();

	return 0;
}

int CXdllApp::Stop()
{
	if ( m_client )
	{
		g_bIsExit = true;
		m_client->Stop();
		//m_client.reset();
	}
	return 0;
}

#ifdef NEED_LOG
void WINAPI DoCleanup();
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch (fdwCtrlType) 
	{ 
		// Handle the CTRL+C signal. 
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		{
			//APPF_EVENT("DoCleanup");
			DoCleanup();
			Sleep(1000);
			return FALSE;
		}
		default:
			return FALSE;
	}
}
#endif

void WINAPI DoStartup()
{
	__try
	{
 		#ifdef NEED_LOG
  		AllocConsole();		
  		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE); 
		freopen("CONOUT$","w+t",stdout); 
		freopen("CONIN$","r+t",stdin); 
		printf("PPAP DoStartup\n");
  		#endif
		theApp.Start();
	}
	__except (APIExceptFilter("Startup", GetExceptionInformation()))
	{
	}
}

void WINAPI DoCleanup()
{
#ifndef _DEBUG
	__try
#endif// _DEBUG
	{
		#ifdef NEED_LOG 
		printf("PPAP DoCleanup\n");
  		#endif
		theApp.Stop();
	}
#ifndef _DEBUG
	__except (APIExceptFilter("Cleanup", GetExceptionInformation()))
	{
	}
#endif // _DEBUG
}

extern "C" BOOL WINAPI Init(HINSTANCE hInstance)
{
 	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
 	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
 
 	theApp.InitInstance();

	return TRUE;
}

extern "C" BOOL WINAPI Exit(HINSTANCE hInstance)
{	
	theApp.ExitInstance();
	return FALSE;
}



extern "C" void WINAPI Startup()
{
	#ifdef NEED_LOG
	OutputDebugString(_T("XD:void WINAPI Startup()"));
	#endif
	//EnterCriticalSection(&_Module.m_csStaticDataInit);
	if( g_bIsRunning == false )
	{ 
		AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
		//::MessageBox(NULL, _T("Fuck"), _T("Fuck"), MB_OK); 
		DoStartup();
		g_bIsRunning = true;
	} 
	//LeaveCriticalSection(&_Module.m_csStaticDataInit);
}

extern "C" void WINAPI Cleanup()
{
	#ifdef NEED_LOG
	OutputDebugString(_T("XD:void WINAPI Cleanup()"));
	#endif
	//EnterCriticalSection(&_Module.m_csStaticDataInit);
	if( g_bIsRunning == true )
	{
 		AFX_MANAGE_STATE(AfxGetStaticModuleState());
 		DoCleanup();
		g_bIsRunning = false;
	}
	//LeaveCriticalSection(&_Module.m_csStaticDataInit);
}

extern "C" int WINAPI GetState()
{
	return 0;
}

BOOL CXdllApp::InitATL()
{
	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	return TRUE;
}

BOOL CXdllApp::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_ACTIVATE)
	{
		APP_EVENT("CXdllApp::PreTranslateMessage WM_ACTIVATE");
	}
	
	return CWinApp::PreTranslateMessage(pMsg);
}

TipsClient* AfxGetTipsClient()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CXdllApp* xdll = (CXdllApp*)AfxGetApp();
	if (xdll) {
		return xdll->m_client.get();
	}
	return NULL;
}