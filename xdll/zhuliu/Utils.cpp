#include "stdafx.h"
#include "Utils.h"

#pragma comment(lib, "version.lib")

BOOL CUtils::IsScreenSaverRunning()
{
	BOOL bIsRun = FALSE;
	::SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &bIsRun, 0);
	return bIsRun;
}

BOOL CUtils::IsLocked()
{
	HDESK hDesk = OpenInputDesktop(0, FALSE, DESKTOP_ENUMERATE | DESKTOP_READOBJECTS);
	return !hDesk;
}

void CUtils::GetMemoryInfo(DWORD processID, DWORD& mem, DWORD& vmem)
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

	
    // Print information about the memory usage of the process.
	
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (NULL == hProcess)
        return;
	
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
		mem = pmc.WorkingSetSize;
		vmem = pmc.PagefileUsage;

			/*
        printf( "\tPageFaultCount:\t\t\t%08d\n", pmc.PageFaultCount );
        printf( "\tPeakWorkingSetSize:\t\t%08d\n", 
			pmc.PeakWorkingSetSize );
        printf( "\tWorkingSetSize:\t\t\t%08d\n", pmc.WorkingSetSize );
        printf( "\tQuotaPeakPagedPoolUsage:\t%08d\n", 
			pmc.QuotaPeakPagedPoolUsage );
        printf( "\tQuotaPagedPoolUsage:\t\t%08d\n", 
			pmc.QuotaPagedPoolUsage );
        printf( "\tQuotaPeakNonPagedPoolUsage:\t%08d\n", 
			pmc.QuotaPeakNonPagedPoolUsage );
        printf( "\tQuotaNonPagedPoolUsage:\t\t%08d\n", 
			pmc.QuotaNonPagedPoolUsage );
        printf( "\tPagefileUsage:\t\t\t%08d\n", pmc.PagefileUsage ); 
        printf( "\tPeakPagefileUsage:\t\t%08d\n", 
			pmc.PeakPagefileUsage );
			*/
    }
	
    CloseHandle(hProcess);
}

DWORD CUtils::GetPIDByName(tstring strProcessName)
{
	DWORD PID = 0;
	HANDLE snapshothandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);   
	PROCESSENTRY32 processentry;   
	processentry.dwSize = sizeof(PROCESSENTRY32);   
	BOOL finded = Process32First(snapshothandle, &processentry);
	while (finded != NULL)   
	{
		if (StrUtils::CompareNoCase(strProcessName, processentry.szExeFile))
		{
			PID = processentry.th32ProcessID;
			break;
		}
		finded = Process32Next(snapshothandle, &processentry);   
	}   
	CloseHandle(snapshothandle);
	return PID;
}

VECTOR_TSTRING CUtils::FindFileByExt(tstring strFolder, tstring strExe)
{
	VECTOR_TSTRING vt;
	HANDLE hFind; 
	WIN32_FIND_DATA fd; 
	
	tstring strSearch = StrUtils::Format(_T("%s\\*%s"), strFolder.c_str(), strExe.c_str()); 
	hFind = FindFirstFile(strSearch.c_str(), &fd); 
	if (hFind != INVALID_HANDLE_VALUE) 
	{ 
		do 
		{ 
			tstring strFileName = StrUtils::Format(_T("%s\\%s"), strFolder.c_str(), fd.cFileName); 
			vt.push_back(strFileName);
		}
		while(FindNextFile(hFind, &fd)); 
	} 
	FindClose(hFind);
	return vt;
}

bool CUtils::CheckSingleProcess(HANDLE& hMutex, const tstring& strMutexName)
{
	hMutex = CreateMutex(NULL, FALSE, strMutexName.c_str());
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		::CloseHandle(hMutex);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CUtils::RunSecondProcess(HANDLE hMutex, const tstring& strCommandLine)
{
	::ReleaseMutex(hMutex);
	::CloseHandle(hMutex);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	if(CreateProcess( 
		NULL,			  // No module name (use command line). 
		(LPTSTR)strCommandLine.c_str(),   // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi)              // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		//WaitForSingleObject(pi.hProcess, INFINITE);
		//CloseHandle(pi.hProcess);
		//CloseHandle(pi.hThread);
	}
}

UINT CUtils::GetRandomNum(UINT uMin, UINT uMax)
{
	assert(uMin <= uMax);
	assert(uMax <= RAND_MAX);
	return rand()%(++uMax-uMin)+uMin;
}

BOOL CUtils::GetBoolByProbability(UINT uNumOfHundred)
{
	assert(uNumOfHundred >= 0 && uNumOfHundred <= 100);
	return GetRandomNum(1, 100) <= uNumOfHundred;
}

// TRUE  pszVersion1 <  pszVersion2
// FALSE pszVersion1 >= pszVersion2
BOOL CUtils::CompareVersion(PCTSTR pszVersion1, PCTSTR pszVersion2)
{
	LPTSTR lpStop1 = NULL,lpStop2=NULL;
	ULONG ulVer1,ulVer2;
	ulVer1 = _tcstoul(pszVersion1,&lpStop1,10);
	ulVer2 = _tcstoul(pszVersion2,&lpStop2,10);
	if (ulVer1 < ulVer2) // 主版本号
	{
		return TRUE;
	}
	else if (ulVer1 > ulVer2)
	{
		return FALSE;
	}
	pszVersion1 = ++lpStop1;
	pszVersion2 = ++lpStop2;
	ulVer1 = _tcstoul(pszVersion1,&lpStop1,10);
	ulVer2 = _tcstoul(pszVersion2,&lpStop2,10);
	if (ulVer1 < ulVer2) // 次版本号
	{
		return TRUE;
	}
	else if (ulVer1 > ulVer2)
	{
		return FALSE;
	}
	
	pszVersion1 = ++lpStop1;
	pszVersion2 = ++lpStop2;
	ulVer1 = _tcstoul(pszVersion1,&lpStop1,10);
	ulVer2 = _tcstoul(pszVersion2,&lpStop2,10);
	if (ulVer1 < ulVer2) // 小版本号
	{
		return TRUE;
	}
	else if (ulVer1 > ulVer2)
	{
		return FALSE;
	}
	
	pszVersion1 = ++lpStop1;
	pszVersion2 = ++lpStop2;
	ulVer1 = _tcstoul(pszVersion1,&lpStop1,10);
	ulVer2 = _tcstoul(pszVersion2,&lpStop2,10);
	if (ulVer1 < ulVer2) // P2P 核心库版本号
	{
		return TRUE;
	}
	else if (ulVer1 > ulVer2)
	{
		return FALSE;
	}
	return FALSE;
}

tstring CUtils::GetFileVersionStr(const tstring& strFilePath)
{
	INT64 i = GetFileVersionNumber(strFilePath);
	int i1 = i/100000000;
	int i2 = (i%100000000)/1000000;
	int i3 = ((i%100000000)%1000000)/10000;
	int i4 = ((i%100000000)%1000000)%10000;
	return StrUtils::Format(_T("%d.%d.%d.%d"), i1, i2, i3, i4);
}

INT64 CUtils::GetFileVersionNumber(const tstring& strFilePath)
{
	DWORD dwHandle;   
	UINT cbVer;   
	VS_FIXEDFILEINFO *lpVer;   
    int dwLen = GetFileVersionInfoSize(strFilePath.c_str(), &dwHandle);   
    void *temp = malloc(dwLen);   
    
	int ysuccess = GetFileVersionInfo(strFilePath.c_str(), dwHandle, dwLen, temp);   
	int zsuccess = VerQueryValue(temp, TEXT("\\"), (void **)&lpVer, &cbVer);
	
    INT64 i1 = HIWORD(lpVer->dwProductVersionMS);
	INT64 i2 = LOWORD(lpVer->dwProductVersionMS);
	INT64 i3 = HIWORD(lpVer->dwProductVersionLS);
	INT64 i4 = LOWORD(lpVer->dwProductVersionLS);
	
	free(temp);

	return i1*100000000 + i2*1000000 + i3*10000 + i4;
}

void CUtils::KillAllProcessByName(tstring strModuleName)
{
	DWORD dwOwnID = GetCurrentProcessId();
	
	HANDLE snapshothandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);   
	PROCESSENTRY32 processentry;
	processentry.dwSize = sizeof(PROCESSENTRY32);   
	BOOL finded = Process32First(snapshothandle, &processentry);   
	while (finded != NULL)   
	{
		if (StrUtils::CompareNoCase(processentry.szExeFile, strModuleName))
		{
			DWORD PID = processentry.th32ProcessID;
			if (dwOwnID != PID)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}
		finded = Process32Next(snapshothandle, &processentry);   
	}   
	CloseHandle(snapshothandle);
}