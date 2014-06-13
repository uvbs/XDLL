// monitor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "AppDM.h"
#include "Update.h"
#include "Utils.h"
#include "UpdateThread.h"
#include "message.h"
#include "DefConst.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;
HWND g_hWnd;											
TCHAR szTitle[MAX_LOADSTRING] = _T("Moniter_Class");		
TCHAR szWindowClass[MAX_LOADSTRING] = _T("Main Window");

#pragma comment(lib, "xdll_lib.lib")

extern "C" void WINAPI Startup();
extern "C" void WINAPI Cleanup();
extern "C" BOOL WINAPI Init(HINSTANCE hInstance);
extern "C" BOOL WINAPI Exit(HINSTANCE hInstance);

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);  

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;	//LoadIcon(hInstance, (LPCTSTR)IDI_MONITOR);
	wcex.hCursor		= NULL;	//LoadCursor(NULL, IDC_ARROW);;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;	//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	RegisterClassEx(&wcex);

	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_MINIMIZE, // WS_OVERLAPPEDWINDOW,
		0, 0, 100, 100, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
	{
		return FALSE;
	}

	//UpdateWindow(g_hWnd);
	//ShowWindow(g_hWnd, SW_NORMAL);

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MONITOR);

  	// single process
  	if (!CUtils::CheckSingleProcess(CAppDM::m_hMutex, MUTEX_NAME))
  		return 0;
  
  	// kill others
  	CUtils::KillAllProcessByName(PPLAP_EXE_FILE_NAME);
  
  	// init environment variable
  	CAppDM::Init();

	InitDefConst();
  	
  	// update thread
  	UpdateThread* pUpdateThread = new UpdateThread();
  	pUpdateThread->Start();

	Init(hInstance);

	Startup();

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	::TerminateProcess(::GetCurrentProcess(), 0);

	//Sleep(10*1000);

	Cleanup();

	Exit(hInstance);

	pUpdateThread->Stop(1000);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_ENDSESSION:
			PostQuitMessage(0);
			break;
		case UM_UPDATE_SUCCEED:
			// download succeed, so restart self
			CUtils::RunSecondProcess(CAppDM::m_hMutex, PathAndDisk::Path_Combine(CAppDM::m_strCurrentFolder, PPLAP_EXE_FILE_NAME));
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}