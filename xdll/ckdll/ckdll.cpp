// ckdll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ckdll.h"

#include <ppl/stl/strings.h>
#include <ppl/mswin/file.h>
#include <ppl/mswin/ipc.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


extern "C" CKDLL_API int Check(void)
{
	bool iutExists = false;
	string iutName = "iresea";
	for ( ProcessWalker walker; walker.HasMore(); walker.MoveNext() )
	{
		const PROCESSENTRY32& info = walker.Current();
		string name = info.szExeFile;
		strings::make_lower( name );
		if ( name.find( iutName) != string::npos )
		{
			// �ҵ�iut���̣�����Ҫ���pplive�����Ƿ��Ѿ�������
			iutExists = true;
			break;
		}
	}
	// ppliveû�����������iut���ڣ�����Ҫ����
	return (iutExists == true) ? 1 : 0;
}


