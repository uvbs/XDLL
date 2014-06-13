#include "StdAfx.h"
#include "HookHelper.h"
#include "Xdll.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <MMSystem.h>
#include <dsound.h>
#pragma comment(lib, "dsound")
//#include <strsafe.h>

#include "ppl/util/log.h"
#include <Mmsystem.h>

#include "detours.h"
using namespace std;

#pragma comment(lib,"detours.lib")
#pragma warning(disable:4127)   // Many of our asserts are constants.

#define ASSERT_ALWAYS(x)   \
	do {                                                        \
	if (!(x)) {                                                 \
	AssertMessage(#x, __FILE__, __LINE__);              \
	DebugBreak();                                       \
	}                                                           \
	} while (0)

// #ifndef NDEBUG
// #define ASSERT(x)           ASSERT_ALWAYS(x)
// #else
// #define ASSERT(x)
// #endif

/*#define UNUSED(c)       (c) = (c)*/
#define ARRAYOF(x)      (sizeof(x)/sizeof(x[0]))

static HMODULE s_hInst = NULL;
static WCHAR s_wzDllPath[MAX_PATH];
extern int threadable;
static BOOL s_bLog = 1;
static LONG s_nTlsIndent = -1;
static LONG s_nTlsThread = -1;
static LONG s_nThreadCnt = 0;

BOOL (WINAPI * Real_PlaySound)(LPCSTR pszSound,  HMODULE hmod, DWORD fdwSound) = PlaySound;
BOOL (WINAPI * Real_PlaySoundW)(LPCWSTR pwszSound,  HMODULE hmod, DWORD fdwSound) = PlaySoundW;
MMRESULT (WINAPI * Real_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh) = waveOutWrite;
BOOL (WINAPI * Real_ShowWindow)(HWND hWnd, int nCmdShow) = ShowWindow;
HANDLE (WINAPI * Real_CreateFile)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFile;
HANDLE (WINAPI * Real_CreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;

HRESULT (WINAPI * Real_CoGetClassObject)(REFCLSID, DWORD, COSERVERINFO *, REFIID, LPVOID *) = (HRESULT (WINAPI *)(REFCLSID, DWORD, COSERVERINFO *, REFIID, LPVOID *))CoGetClassObject;
HRESULT (WINAPI * Real_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID *) = CoCreateInstance;

DWORD GlobelMessageThreadID = 0;
int g_screenWidth = 0;
int g_screenHeight = 0; 

//改变全屏分辨率的大小
INT (WINAPI * Real_GetDeviceCaps)(HDC hdc, int nIndex) = GetDeviceCaps;
INT WINAPI Mine_GetDeviceCaps(HDC hdc, int nIndex)
{
	if (HORZRES == nIndex)
	{
		if(g_screenWidth> 0)
			return g_screenWidth;
	}
	
	if (VERTRES == nIndex)
	{
		if (g_screenHeight > 0)
			return g_screenHeight;
	}
	
	return Real_GetDeviceCaps(hdc, nIndex);
}

BOOL WINAPI Mine_PlaySound(LPCSTR pszSound,  HMODULE hmod, DWORD fdwSound)
{
	APP_EVENT("Mine_PlaySound "<<pszSound<<" "<<fdwSound);
	return TRUE;
}

BOOL WINAPI Mine_PlaySoundW(LPCWSTR pwszSound,  HMODULE hmod, DWORD fdwSound)
{
	APP_EVENT("Mine_PlaySound "<<" "<<fdwSound);
	return TRUE;
}

MMRESULT WINAPI Mine_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	APP_EVENT("Mine_waveOutWrite");
	return 0;
}

BOOL WINAPI Mine_ShowWindow(HWND hWnd, int nCmdShow)
{
	if( nCmdShow != SW_HIDE)
	{
		TCHAR szWindowClassName[256];
		TCHAR szWindowCaption[256];
		GetClassName(hWnd, szWindowClassName, 256);
		GetWindowText(hWnd, szWindowCaption, 256);
		if( StrCmp(szWindowClassName, _T("#32770")) == 0 || StrCmp(szWindowClassName, _T("Internet Explorer_TridentDlgFrame")) == 0 )
		{
			APP_EVENT("Mine_ShowWindow "<<(UINT)hWnd<<" "<<nCmdShow<<""<<szWindowClassName<<" "<<szWindowCaption<<" | "<<nCmdShow);
			if(
				StrCmpI(szWindowCaption, _T("Internet Explorer")) == 0 ||
				StrCmpI(szWindowCaption, _T("添加收藏")) == 0 ||
				StrCmpI(szWindowCaption, _T("添加到收藏夹")) == 0 ||
				StrStrI(szWindowCaption, _T("收藏")) != NULL ||
				StrCmpI(szWindowCaption, _T("Add Favorite")) == 0 ||
				StrCmpI(szWindowCaption, _T("添加或更改主页")) == 0 ||
				StrCmpI(szWindowCaption, _T("Home Page")) == 0 ||
				StrCmpI(szWindowCaption, _T("安全信息")) == 0 ||
				StrCmpI(szWindowCaption, _T("安全警告")) == 0 ||
				StrCmpI(szWindowCaption, _T("Security Alert")) ==0 ||
				StrCmpI(szWindowCaption, _T("Security Information")) ==0 ||
				StrCmpI(szWindowCaption, _T("打印")) ==0 ||
				StrStrI(szWindowCaption, _T("Security")) != NULL ||
				StrStrI(szWindowCaption, _T("Print")) != NULL
			  )
			{
				APP_EVENT("Real_ShowWindow "<<(UINT)hWnd<<" SW_HIDE");
				return Real_ShowWindow(hWnd, SW_HIDE);
			}
		}
	}

	return Real_ShowWindow(hWnd, nCmdShow);
}

HANDLE WINAPI Mine_CreateFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
 	if( StrStrI(lpFileName, "History\\History.IE5\\") != NULL)
 	{
 		if( StrStrI(lpFileName, "History\\History.IE5\\index.dat") == NULL)
 		{
  			APP_EVENT("Mine_CreateFile "<<lpFileName);
			SetLastError(ERROR_ACCESS_DENIED);
  			return INVALID_HANDLE_VALUE;
 		}
 	}
	return Real_CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI Mine_CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
 	if( StrStrIW(lpFileName, L"History\\History.IE5\\") != NULL)
 	{
 		if( StrStrIW(lpFileName, L"History\\History.IE5\\index.dat") == NULL)
 		{
  			APP_EVENT("Mine_CreateFileW "<<(LPCSTR)CString(lpFileName) );
			SetLastError(ERROR_ACCESS_DENIED);
  			return INVALID_HANDLE_VALUE;
 		}
 	}
	return Real_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

MIDL_INTERFACE("EF0D1A14-1033-41A2-A589-240C01EDC078")
		PPP_OCX_GUID;
MIDL_INTERFACE("BCDE0395-E52F-467C-8E3D-C4579291692E")
		CLSID_MMDeviceEnumerator_GUID;

HRESULT WINAPI Mine_CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID * ppv)
{
  	if (::GetCurrentThreadId() == GlobelMessageThreadID && 
  		(__uuidof(PPP_OCX_GUID) == rclsid || __uuidof(CLSID_MMDeviceEnumerator_GUID) == rclsid))
  	{
  		return E_FAIL;
  	}
	return Real_CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

HRESULT WINAPI Mine_CoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, COSERVERINFO * pServerInfo, REFIID riid, LPVOID * ppv)
{
	if (::GetCurrentThreadId() == GlobelMessageThreadID && __uuidof(PPP_OCX_GUID) == rclsid)
	{
		APP_WARN("Mine_CoGetClassObject Meet PPP_OCX_GUID, so return E_FAIL");
		return E_FAIL;
	}
	return Real_CoGetClassObject(rclsid, dwClsContext, pServerInfo, riid, ppv);
}

HRESULT (WINAPI *Real_DirectSoundCreate)(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter) = DirectSoundCreate;
HRESULT WINAPI Mine_DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	if (::GetCurrentThreadId() == GlobelMessageThreadID)
	{
		return E_FAIL;
	}
	return Real_DirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter);
}

HRESULT (WINAPI *Real_DirectSoundCreate8)(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter) = DirectSoundCreate8;
HRESULT WINAPI Mine_DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
	if (::GetCurrentThreadId() == GlobelMessageThreadID)
	{
		return E_FAIL;
	}
	return Real_DirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
}


static HMODULE s_hDll;

HMODULE WINAPI Detoured()
{
	return s_hDll;
}

HookHelper::HookHelper(void)
{
}

HookHelper::~HookHelper(void)
{
}

void HookHelper::InitHook()
{
	 TCHAR opertionstr[256];
	 memset(opertionstr,0,256*sizeof(TCHAR));

	 if(Is64Operation(opertionstr))
	 {
		 printf("是64位\n");
	 }
	 else
	 {
		 printf("不是64位\n");
	 }

     HINSTANCE g_Instance=theApp.m_hInstance;

	 fflush(stdout);
	 Sleep(50);
	 Sleep(50);

	 fflush(stdout);
	 DetourRestoreAfterWith();
	 printf("DetourRestoreAfterWith\n");

	 fflush(stdout);
	 ProcessAttach(g_Instance);
	 printf("ProcessAttach\n");

	 fflush(stdout);
	 ThreadAttach(g_Instance);
	 printf("ThreadAttach\n");
 }






 BOOL HookHelper::Is64Operation(LPTSTR pszOS)
 {
	 return FALSE;
 }


 PCHAR HookHelper::DetRealName(PCHAR psz)
 {
	 PCHAR pszBeg = psz;

	 while (*psz) 
	 {
		 psz++;
	 }
	 while (psz > pszBeg &&((psz[-1] >= 'A' && psz[-1] <= 'Z') ||(psz[-1] >= 'a' && psz[-1] <= 'z') ||(psz[-1] >= '0' && psz[-1] <= '9'))) 
	 {
		 psz--;
	 }
	 return psz;
 }


 VOID HookHelper::DetAttach(PVOID *ppbReal, PVOID pbMine, PCHAR psz)
 {
	 LONG l = DetourAttach(ppbReal, pbMine);
	 if (l != 0) 
	 {

	 }
 }

 VOID HookHelper::DetDetach(PVOID *ppbReal, PVOID pbMine, PCHAR psz)
 {
	 LONG l = DetourDetach(ppbReal, pbMine);
	 if (l != 0) 
	 {

	 }
 }


#define ATTACH(x,y)   DetAttach(x,y,#x)
#define DETACH(x,y)   DetDetach(x,y,#x)

 LONG HookHelper::AttachDetours(VOID)
 {
	 DetourTransactionBegin();
	 DetourUpdateThread(GetCurrentThread());
	 
	 //ATTACH(&(PVOID&)Real_connect, Mine_connect);
	 ATTACH(&(PVOID&)Real_PlaySound, Mine_PlaySound);
	 ATTACH(&(PVOID&)Real_PlaySoundW, Mine_PlaySoundW);
	 ATTACH(&(PVOID&)Real_waveOutWrite, Mine_waveOutWrite);
	 ATTACH(&(PVOID&)Real_ShowWindow, Mine_ShowWindow);
	 ATTACH(&(PVOID&)Real_CreateFile, Mine_CreateFile);
	 ATTACH(&(PVOID&)Real_CreateFileW, Mine_CreateFileW);
	 ATTACH(&(PVOID&)Real_CoCreateInstance, Mine_CoCreateInstance);
	 ATTACH(&(PVOID&)Real_CoGetClassObject, Mine_CoGetClassObject);
	 ATTACH(&(PVOID&)Real_DirectSoundCreate, Mine_DirectSoundCreate);
	 ATTACH(&(PVOID&)Real_DirectSoundCreate8, Mine_DirectSoundCreate8);
	 ATTACH(&(PVOID&)Real_GetDeviceCaps, Mine_GetDeviceCaps);

	 return DetourTransactionCommit();
 }

 LONG HookHelper::DetachDetours(VOID)
 {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	//DETACH(&(PVOID&)Real_connect, Mine_connect);
	DETACH(&(PVOID&)Real_PlaySound, Mine_PlaySound);
	DETACH(&(PVOID&)Real_PlaySoundW, Mine_PlaySoundW);
	DETACH(&(PVOID&)Real_waveOutWrite, Mine_waveOutWrite);
	DETACH(&(PVOID&)Real_ShowWindow, Mine_ShowWindow);
	DETACH(&(PVOID&)Real_CreateFile, Mine_CreateFile);
	DETACH(&(PVOID&)Real_CreateFileW, Mine_CreateFileW);
	DETACH(&(PVOID&)Real_CoCreateInstance, Mine_CoCreateInstance);
	DETACH(&(PVOID&)Real_CoGetClassObject, Mine_CoGetClassObject);
	DETACH(&(PVOID&)Real_DirectSoundCreate, Mine_DirectSoundCreate);
	DETACH(&(PVOID&)Real_DirectSoundCreate8, Mine_DirectSoundCreate8);
	DETACH(&(PVOID&)Real_GetDeviceCaps, Mine_GetDeviceCaps);

	return DetourTransactionCommit();
 }


 BOOL HookHelper::ThreadAttach(HMODULE hDll)
 {
	 (void)hDll;

	 if (s_nTlsIndent >= 0)
	 {
		 TlsSetValue(s_nTlsIndent, (PVOID)0);
	 }
	 if (s_nTlsThread >= 0) 
	 {
		 LONG nThread = InterlockedIncrement(&s_nThreadCnt);
		 TlsSetValue(s_nTlsThread, (PVOID)(LONG_PTR)nThread);
	 }
	 return TRUE;
 }

 BOOL HookHelper::ThreadDetach(HMODULE hDll)
 {
	 (void)hDll;

	 if (s_nTlsIndent >= 0) 
	 {
		 TlsSetValue(s_nTlsIndent, (PVOID)0);
	 }
	 if (s_nTlsThread >= 0) 
	 {
		 TlsSetValue(s_nTlsThread, (PVOID)0);
	 }
	 return TRUE;
 }

 DWORD (WINAPI * Real_GetModuleFileNameW)(HMODULE a0, LPWSTR a1,	 DWORD a2)	 = GetModuleFileNameW;

 BOOL HookHelper::ProcessAttach(HMODULE hDll)
 {
	 s_bLog = FALSE;
	 s_nTlsIndent = TlsAlloc();
	 s_nTlsThread = TlsAlloc();

	 WCHAR wzExeName[MAX_PATH];
	 s_hInst = hDll;

	 Real_GetModuleFileNameW(hDll, s_wzDllPath, ARRAYOF(s_wzDllPath));
	 Real_GetModuleFileNameW(NULL, wzExeName, ARRAYOF(wzExeName));

	 LONG error = AttachDetours();
	 if (error != NO_ERROR) 
	 {

	 }

	 ThreadAttach(hDll);

	 s_bLog = TRUE;
	 return TRUE;
 }

 BOOL HookHelper::ProcessDetach(HMODULE hDll)
 {
	 ThreadDetach(hDll);
	 s_bLog = FALSE;

	 LONG error = DetachDetours();
	 if (error != NO_ERROR) 
	 {

	 }
	 if (s_nTlsIndent >= 0) 
	 {
		 TlsFree(s_nTlsIndent);
	 }

	 if (s_nTlsThread >= 0) 
	 {
		 TlsFree(s_nTlsThread);
	 }
	 return TRUE;
 }
