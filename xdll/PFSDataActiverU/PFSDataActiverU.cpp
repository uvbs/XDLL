// PFSDataActiverU.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>

typedef void (CALLBACK* PFN_ProductUpdate_RD_XXXX)(HWND, HINSTANCE,LPSTR,int);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// ¼ÓÔØÉý¼¶Ä£¿é
	HMODULE	m_hModule1 = LoadLibrary(_T("ProductUpdate.dll"));
	if (!m_hModule1)
	{
		return 0;
	}
	else
	{
		PFN_ProductUpdate_RD_XXXX RD_XXXX = (PFN_ProductUpdate_RD_XXXX)GetProcAddress(m_hModule1,"RD_XXXX");
		if(!RD_XXXX)
		{
			FreeLibrary(m_hModule1);
		}
		else
		{
			RD_XXXX(::GetDesktopWindow(), hInstance, lpCmdLine, nCmdShow);
		}
	}

	return 0;
}



