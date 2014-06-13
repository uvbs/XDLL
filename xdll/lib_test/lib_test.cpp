// lib_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lib_test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

//CWinApp theApp;

using namespace std;

#pragma comment(lib, "xdll_lib.lib")

extern "C" void WINAPI Startup();
extern "C" void WINAPI Cleanup();

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CString strHello;
		strHello.LoadString(IDS_HELLO);
		cout << (LPCTSTR)strHello << endl;
	}

	Startup();

	Sleep(10*60*1000);

	Cleanup();

	return nRetCode;
}


