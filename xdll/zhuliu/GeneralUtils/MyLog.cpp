// 根据工程设置决定是否包含"stdafx.h"
#include "stdafx.h"

#include "MyLog.h"
#include <time.h>
#include <locale.h>
#include <assert.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <shlobj.h>

#pragma comment(lib, "shlwapi.lib")


#define MAX_LOGLINE_LENGTH		2048
#define MAX_LOGNAME_LENGTH		30
#define MAX_TIMESTR_LENGTH		30

void MyLog::Init(bool bEnable, TCHAR* szLogName, TCHAR* szININame)
{
	m_bEnable = bEnable;
	if (!m_bEnable) return;

	// current folder
	TCHAR folder[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, m_AppFolder, MAX_PATH);
	PathRemoveFileSpec(m_AppFolder);

	// parse ini file
	if (szININame && szLogName)
	{
		TCHAR szINIPath[MAX_PATH] = { 0 };
		::PathCombine(szINIPath, m_AppFolder, szININame);
		_tcscat(szINIPath, _T(".ini"));

		if (::PathFileExists(szINIPath))
		{
			m_bEnable = (::GetPrivateProfileInt(szLogName, _T("EnableLog"), 0, szINIPath) != 0);
			if (!m_bEnable) return;
				
			TCHAR szLogType[MAX_PATH] = { 0 };
			::GetPrivateProfileString(szLogName, _T("LogType"), _T("stdout"), szLogType, MAX_PATH, szINIPath);
			if (_tcsicmp(_T("stdout"), szLogType) == 0) m_OutType = LOGOUT_STDOUT;
			else if (_tcsicmp(_T("file"), szLogType) == 0) m_OutType = LOGOUT_FILE;
			else m_OutType = LOGOUT_OUTPUTDEBUGSTRING;

			m_bWithSource = (::GetPrivateProfileInt(szLogName, _T("WithSource"), 0, szINIPath) != 0);
			m_bWithMillisecond = (::GetPrivateProfileInt(szLogName, _T("WithMillisecond"), 0, szINIPath) != 0);
		}
	}

	_tcscpy(m_LogName, szLogName);
	InitializeCriticalSection(&m_cs);
	_tsetlocale(LC_CTYPE, _T(""));
}

MyLog::MyLog()
{
	m_bEnable = false;
	m_bFileNameAlreadySet = false;
	m_bWithMillisecond = false;
	m_bWithSource = false;

	m_File = NULL;
	m_FileName[0] = _T('\0');
	m_AppFolder[0] = _T('\0');
	m_SourceFile[0] = _T('\0');
	m_LogName[0] = _T('\0');
	m_OutType = LOGOUT_OUTPUTDEBUGSTRING;
	m_LineNumber = 0;
}

MyLog::~MyLog()
{
	if (!m_bEnable) return;

	if (m_File != NULL) fclose(m_File);

	DeleteCriticalSection(&m_cs);
}

void MyLog::GetSysTimeStr(TCHAR* now, bool bWithMillisecond)
{
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	if (bWithMillisecond)
	{
		_stprintf(now, _T("%04u-%02u-%02u %02u:%02u:%02u.%03u"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	} 
	else
	{
		_stprintf(now, _T("%04u-%02u-%02u %02u:%02u:%02u"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	}
}

void MyLog::GetSysTimeStrForFileName(TCHAR* now)
{
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	_stprintf(now, _T("_%04u%02u%02u_%02u%02u%02u"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	_tcscat(now, _T(".log"));
}

void MyLog::SetFileName()
{
	if (!m_bEnable) return;
	if (m_File != NULL) fclose(m_File);

	// logfile path
	TCHAR logFilePath[MAX_PATH] = { 0 };
	_tcscpy(logFilePath, m_AppFolder);
	_tcscat(logFilePath, _T("\\Log\\"));
	if (!PathFileExists(logFilePath)) SHCreateDirectoryEx(NULL, logFilePath, NULL);
	_tcscpy(m_FileName, logFilePath);

	// LogName
	_tcscat(m_FileName, m_LogName);

	// Time
	TCHAR now[MAX_TIMESTR_LENGTH] = { 0 };
	GetSysTimeStrForFileName(now);
	_tcscat(m_FileName, now);

	//open the logfile...
	m_File = _tfopen(m_FileName, _T("at"));
	assert(m_File);

	m_bFileNameAlreadySet = true;
}

void MyLog::LogNow(TCHAR* LogString)
{
	if (!m_bEnable) return;

#ifdef _UNICODE
	LogNowW(LogString);
#else
	LogNowA(LogString);
#endif
}

void MyLog::LogNowA(char* LogString)
{
	if (!m_bEnable) return;

	if (m_OutType == LOGOUT_FILE)
	{
		//get the current date and time, and format it to the format we wanna use...
		TCHAR now[MAX_TIMESTR_LENGTH] = { 0 };
		GetSysTimeStr(now, m_bWithMillisecond);

		if (m_bWithSource)
		{
			#ifdef _UNICODE
						fprintf(m_File, "%S   %S,\t%d\t%s", now, m_SourceFile, m_LineNumber, LogString);
			#else
						fprintf(m_File, "%s   %s,\t%d\t%s", now, m_SourceFile, m_LineNumber, LogString);
			#endif
		}
		else
		{
			#ifdef _UNICODE
						fprintf(m_File, "%S   %s", now, LogString);
			#else
						fprintf(m_File, "%s   %s", now, LogString);
			#endif
		}

		fflush(m_File);
	}
	else if (m_OutType == LOGOUT_STDOUT)
	{
		AllocConsole();
		freopen("CONOUT$", "w+t", stdout);
		//freopen("CONIN$", "r+t", stdin);
		printf(LogString);
	}
	else
	{
		OutputDebugStringA(LogString);
	}
}

void MyLog::LogNowW(WCHAR* LogString)
{
	if (!m_bEnable) return;

	if (m_OutType == LOGOUT_FILE)
	{
		//get the current date and time, and format it to the format we wanna use...
		TCHAR now[MAX_TIMESTR_LENGTH] = { 0 };
		GetSysTimeStr(now, m_bWithMillisecond);

		if (m_bWithSource)
		{
			#ifdef _UNICODE
						fprintf(m_File, "%S   %S,\t%d\t%S", now, m_SourceFile, m_LineNumber, LogString);
			#else
						fprintf(m_File, "%s   %s,\t%d\t%S", now, m_SourceFile, m_LineNumber, LogString);
			#endif
		} 
		else
		{
			#ifdef _UNICODE
						fprintf(m_File, "%S   %S", now, LogString);
			#else
						fprintf(m_File, "%s   %S", now, LogString);
			#endif
		}
		
		fflush(m_File);
	}
	else if (m_OutType == LOGOUT_STDOUT)
	{
		AllocConsole();
		freopen("CONOUT$", "w+t", stdout);
		//freopen("CONIN$", "r+t", stdin);
		wprintf(LogString);
	}
	else
	{
		OutputDebugStringW(LogString);
	}
}

void MyLog::Log(TCHAR* format, ...)
{
	if (!m_bEnable) return;

	EnterCriticalSection(&m_cs);

	//never corrupt the last error value...
	DWORD LastError = GetLastError();

	if ((m_OutType == LOGOUT_FILE) && !m_bFileNameAlreadySet)
		SetFileName();

	//do the actual logging...
	TCHAR mid[MAX_LOGLINE_LENGTH] = { 0 };
	va_list args;
	va_start(args, format);
	_vsntprintf(mid, MAX_LOGLINE_LENGTH - 1, format, args);
	LogNow(mid);
	va_end(args);

	SetLastError(LastError);

	LeaveCriticalSection(&m_cs);
}

void MyLog::LogA(char* format, ...)
{
	if (!m_bEnable) return;

	EnterCriticalSection(&m_cs);

	//never corrupt the last error value...
	DWORD LastError = GetLastError();

	if ((m_OutType == LOGOUT_FILE) && !m_bFileNameAlreadySet)
		SetFileName();

	//do the actual logging...
	char mid[MAX_LOGLINE_LENGTH] = { 0 };
	va_list args;
	va_start(args, format);
	_vsnprintf(mid, MAX_LOGLINE_LENGTH - 1, format, args);
	LogNowA(mid);
	va_end(args);

	SetLastError(LastError);

	LeaveCriticalSection(&m_cs);
}

void MyLog::LogW(WCHAR* format, ...)
{
	if (!m_bEnable) return;

	EnterCriticalSection(&m_cs);

	//never corrupt the last error value...
	DWORD LastError = GetLastError();

	if ((m_OutType == LOGOUT_FILE) && !m_bFileNameAlreadySet)
		SetFileName();

	//do the actual logging...
	WCHAR mid[MAX_LOGLINE_LENGTH] = { 0 };
	va_list args;
	va_start(args, format);
	_vsnwprintf(mid, MAX_LOGLINE_LENGTH - 1, format, args);
	LogNowW(mid);
	va_end(args);

	SetLastError(LastError);

	LeaveCriticalSection(&m_cs);
}

void MyLog::SetSourceFileName(char* filename)
{
	if (!m_bEnable) return;

	char SourceFile[MAX_PATH] = { 0 };

	//strip the path from the filename...
	char* mid = filename + strlen(filename);
	while (mid > filename)
	{
		if (*(--mid) == '\\')
		{
			mid++;
			break;
		}
	}
	//store the filename...
	strcpy(SourceFile, mid);

#ifdef _UNICODE
	DWORD dwMinSize = MultiByteToWideChar(CP_ACP, 0, SourceFile, -1, NULL, 0);
	assert(dwMinSize <= MAX_PATH);

	MultiByteToWideChar(CP_ACP, 0, SourceFile, -1, m_SourceFile, dwMinSize);
#else
	strcpy(m_SourceFile, SourceFile);
#endif
}

void MyLog::SetOutType(LOGOUT outType)
{
	if (!m_bEnable) return;
	EnterCriticalSection(&m_cs);
	m_OutType = outType;
	LeaveCriticalSection(&m_cs);
}

void MyLog::SetWithSource(bool bWithSource)
{
	if (!m_bEnable) return;
	EnterCriticalSection(&m_cs);
	m_bWithSource = bWithSource;
	LeaveCriticalSection(&m_cs);
}

void MyLog::SetTimePrecision(bool bWithMillisecond)
{
	if (!m_bEnable) return;
	EnterCriticalSection(&m_cs);
	m_bWithMillisecond = bWithMillisecond;
	LeaveCriticalSection(&m_cs);
}

void MyLog::SetLineNumber(int line)
{
	if (!m_bEnable) return;
	m_LineNumber = line;
}

