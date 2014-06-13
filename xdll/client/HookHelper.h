#pragma once

extern DWORD GlobelMessageThreadID;
extern int g_screenWidth;
extern int g_screenHeight; 

class HookHelper
{
public:
	HookHelper(void);
	~HookHelper(void);

	void InitHook();

	BOOL Is64Operation(LPTSTR pszOS);
	PCHAR DetRealName(PCHAR psz);
	VOID DetAttach(PVOID *ppbReal, PVOID pbMine, PCHAR psz);
	VOID DetDetach(PVOID *ppbReal, PVOID pbMine, PCHAR psz);
	LONG AttachDetours(VOID);
	LONG DetachDetours(VOID);
	BOOL ThreadAttach(HMODULE hDll);
	BOOL ThreadDetach(HMODULE hDll);
	BOOL ProcessAttach(HMODULE hDll);
	BOOL ProcessDetach(HMODULE hDll);
};
