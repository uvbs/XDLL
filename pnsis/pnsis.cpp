// pnsis.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "exdll.h"
#include <string>
#include <shlwapi.h>
#include <Iphlpapi.h>
using namespace std;

HMODULE g_hInstance = NULL;
HWND g_hwndParent = NULL;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls((HMODULE)hModule);
		g_hInstance = (HMODULE)hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}

string GetHugoIDString();

// extern "C" __declspec(dllexport) void AddEveryoneToAcl(HWND hwndParent, int string_size, 
// 										   char *variables, stack_t **stacktop)
// {
// 	EXDLL_INIT();
// 
// 	// note if you want parameters from the stack, pop them off in order.
// 	// i.e. if you are called via exdll::myFunction file.dat poop.dat
// 	// calling popstring() the first time would give you file.dat,
// 	// and the second time would give you poop.dat. 
// 	// you should empty the stack of your parameters, and ONLY your
// 	// parameters.
// 	char szFileName[_MAX_PATH];
// 	if (popstring(szFileName) != 0)
// 	{
// 		return;
// 	}
// 
// 	// nsis 使用 Pop 获得返回值
// 	pushstring("aaaaaaaaaaa");
// }

extern "C" __declspec(dllexport) void GetHugoID(HWND hwndParent, int string_size, 
										   char *variables, stack_t **stacktop)
{
	EXDLL_INIT();

	// nsis 使用 Pop 获得返回值
	pushstring((char*)GetHugoIDString().c_str());
}


const char* AllTrim(char *pszBuffer)
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

string GetHugoIDString()
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
		sprintf(szMacAddr, "%02x%02x%02x%02x%02x%02x",
			pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
			pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);
	}
	else if (nError == ERROR_BUFFER_OVERFLOW)
	{
		pInfo = (PIP_ADAPTER_INFO)malloc(dwLen);
		nError	=	GetAdaptersInfoFunc(pInfo, &dwLen);
		if (nError==0)
		{
			sprintf(szMacAddr, "%02x%02x%02x%02x%02x%02x",
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