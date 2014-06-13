#pragma once

#include <list>
#include <string>
#include "xdll.h"
using namespace std;

class TipsUtil
{
public:
	static bool IsComputerSafe();								// 检测机器是否有安全防护
	static bool HasProcessRunning(std::list<string> items);		// 检测机器是否正在运行指定程序
	static string GetExeFileFolder();							// 获得Exe文件的路径名
	static bool Is64Operation(LPTSTR pszOS);					// 检测是不是64机
	
	static void RunExeFile(string str);							// 执行一个Exe文件
	static string GetHugoID();
	static string GetMachineGUID();
	static string GetPPID();
	static const char* AllTrim(char *pszBuffer);

	static string GetCurrentProcessName();						// 获得当前的进程的进程名
	static UINT32 GetFlagValue();								// 获得需要发出请求的Flag值
	static bool HasProcess(string iutName);						// 根据名称查找 是否存在这个进程

	static bool CanTipsRun();

	static bool CheckInstallSoftware();							// 检查安装有 开发软件 或者 抓包软件
	static bool CheckCurrentProcess();							// 检查当前正在运行的进程 是否有 开发进程 和 抓包进程
	static bool CheckBrowserScriptDebugOff();					// 检查内嵌浏览器是否 关闭调试
	
	static void RemoveCookies(const std::list<string>& removeCookiesList, int method = 1);		// 删除IE的Cookie
	static void RemoveFlashCookies(const std::list<string>& removeCookiesList, int method = 0);

	static bool IsOnlyPPAP();									// 检查是不是PPAP.exe的驻留态

	static void OpenLimitMemory(bool bIsOpened);				// 开启或者关闭内存限制

	static int GetAreaCode();
	static DWORD GetSeconds1970();
	static void FileTimeToTime_t(FILETIME ft, time_t *t);

	static string DecodeFile(const string& beforeFile, const string& tempName);
	
	static bool WCHARtoTCHAR(LPCWSTR lpcwszStr, LPTSTR lptszStr);
	static bool TCHARtoCHAR(LPCTSTR lpctszStr, LPSTR lpszStr);

	static string GetMacAddr();					//获得机器MAC地址
	static string GetUserProfile();				//获得注册表中加密后的用户登录信息
	static string GetUserName();				//获得PPTV用户登录账号
	static string GetResolution();				//获得屏幕分辨率
	static string GetFlashVer();				//获得flashplayer版本号
	static string GetSysLID();			//获得操作系统默认语言

	static BOOL PP_GetAllUserAppData(LPTSTR pszCachePath);	//A pointer to a null-terminated string of length MAX_PATH which will receive the path
	
	static UINT RSHash(const char *str);				//字符串哈希
	static BOOL IsWow64();								//检测系统是不是64位
	static CString GetJuid();
	static CString GetOsVersion();						//获取操作系统版本号
	static CString GetFileVersion(LPTSTR szFilePath);	//获得文件的版本号

};