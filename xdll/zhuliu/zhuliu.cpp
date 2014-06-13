// zhuliu.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "AppDM.h"
#include "Update.h"
#include "Utils.h"
#include "UpdateThread.h"

using namespace std;

#pragma comment(lib, "xdll_lib.lib")

extern "C" void WINAPI Startup();
extern "C" void WINAPI Cleanup();
extern "C" void WINAPI Init();

#define MAX_LOADSTRING 100

HINSTANCE hInst;												
TCHAR szTitle[MAX_LOADSTRING] = _T("Moniter_Class");		
TCHAR szWindowClass[MAX_LOADSTRING] = _T("Main Window");

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL; //LoadIcon(hInstance, (LPCTSTR)IDI_MONITOR);
	wcex.hCursor		= NULL; //LoadCursor(NULL, IDC_ARROW);;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL; //LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	RegisterClassEx(&wcex);

	HWND hWnd;
	hWnd = CreateWindow(szWindowClass, szTitle, WS_MINIMIZE,
		0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

//	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MONITOR);

	::AllocConsole();
	freopen("CONOUT$","w",stdout);

 	// single process
 	if (!CUtils::CheckSingleProcess(CAppDM::m_hMutex, MUTEX_NAME))
 		return 0;
 
 	// kill others
 	CUtils::KillAllProcessByName(PPLAP_EXE_FILE_NAME);
 
 	// init environment variable
 	CAppDM::Init();
 	
 	// update thread
 	UpdateThread* pUpdateThread = new UpdateThread();
 	pUpdateThread->Start();

	Init();

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

	Cleanup();

	// wait all subthread exit normally
	// kill the timeout-subthread
	pUpdateThread->Stop(300);

	// at this point, all user-cleanup work have been done
	// so we can run the second process to kill this process
	//CUtils::RunSecondProcess(CAppDM::m_hMutex, PathAndDisk::Path_Combine(CAppDM::m_strCurrentFolder, PPLAP_EXE_FILE_NAME));

	// should never arrive here...
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_ENDSESSION:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

