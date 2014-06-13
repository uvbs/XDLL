#include "stdafx.h"
#include "AppDM.h"
#include "Utils.h"

HANDLE CAppDM::m_hMutex;
volatile bool CAppDM::m_bGlobalEnd;
DWORD CAppDM::m_dwGlobalStartTickCount;
tstring CAppDM::m_strTmpFolder;
tstring CAppDM::m_strTmpFolder_INI;
tstring CAppDM::m_strCurrentFolder;

void CAppDM::Init()
{
	m_bGlobalEnd = FALSE;
	m_dwGlobalStartTickCount = ::GetTickCount();
 	GetTmpFolder();
	GetCurrentFolder();

	// init log
	bool bEnableLog = false;
#ifdef _DEBUG
	bEnableLog = true;
#endif
	//g_log.Init(bEnableLog, _T("pplap"), NULL);
	//g_log.SetOutType(LOGOUT_FILE);
	//g_log.LogA("New start!\n");

	// delete .(old) file
	VECTOR_TSTRING vt = CUtils::FindFileByExt(m_strCurrentFolder, _T(".(old)"));
	for (size_t i = 0; i < vt.size(); i++)
	{
		if (::DeleteFile(vt[i].c_str()) == 0)
		{
			//g_log.LogA("DeleteFile error code = %d\n", ::GetLastError());
		}
	}
}
 
void CAppDM::GetTmpFolder()
{
	// m_strTmpFolder
	TCHAR szTmp[_MAX_PATH] = { 0 };
	if (!GetTempPath(MAX_PATH, szTmp))
	{
		GetModuleFileName(NULL, szTmp, _MAX_PATH);
		PathRemoveFileSpec(szTmp);
		m_strTmpFolder = szTmp;
	}
	else
	{
		m_strTmpFolder = szTmp;
	}
	m_strTmpFolder_INI = PathAndDisk::Path_Combine(CAppDM::m_strTmpFolder, SERVER_INI_FILE_NAME);
}

void CAppDM::GetCurrentFolder()
{
	DWORD PID = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	TCHAR path[MAX_PATH + 1] = { 0 };
	GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH + 1);
	CloseHandle(hProcess);
	m_strCurrentFolder = PathAndDisk::Path_GetFolder(path);
}