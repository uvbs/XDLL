#ifndef _MY_LOG_H_
#define _MY_LOG_H_

#include <stdio.h>
#include <windows.h>

enum LOGOUT
{
	LOGOUT_FILE,
	LOGOUT_STDOUT,
	LOGOUT_OUTPUTDEBUGSTRING
};

class MyLog
{
protected:
	CRITICAL_SECTION m_cs;
	FILE* m_File;
	TCHAR m_FileName[MAX_PATH];
	TCHAR m_AppFolder[MAX_PATH];
	TCHAR m_SourceFile[MAX_PATH];
	TCHAR m_LogName[MAX_PATH];
	LOGOUT m_OutType;
	int m_LineNumber;
	bool m_bEnable;
	bool m_bFileNameAlreadySet;
	bool m_bWithMillisecond;
	bool m_bWithSource;
	void LogNow(TCHAR* LogString);
	void LogNowA(char* LogString);
	void LogNowW(WCHAR* LogString);
	void SetFileName();
	void GetSysTimeStr(TCHAR* now, bool bWithMillisecond);
	void GetSysTimeStrForFileName(TCHAR* now);

public:
	MyLog();
	~MyLog();
	void Init(bool bEnable, TCHAR* szLogName, TCHAR* szININame);
	void SetLineNumber(int line);
	void SetSourceFileName(char* filename);
	void SetOutType(LOGOUT outType);
	void SetWithSource(bool bWithSource);
	void SetTimePrecision(bool bWithMillisecond);
	void Log(TCHAR* format, ...);
	void LogA(char* format, ...);
	void LogW(WCHAR* format, ...);


	// 如果使用自定义的log对象 则无法使用预设宏 也无法得到源文件和代码行信息
	// 调用举例: 
	// MyLog g_MyLogger;
	// g_MyLogger.Init(true, _T("ObjectInfo"), _T("MyApp"));
	// g_MyLogger.SetTimePrecision(true);
	// g_MyLogger.SetOutType(LOGOUT_FILE);
	// g_MyLogger.LogA("Hello!");
};

#endif





