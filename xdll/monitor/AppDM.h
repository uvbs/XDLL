#pragma once


class CAppDM  
{
public:
	// 单实例句柄
	static HANDLE m_hMutex;

	// 程序启动时间
	static DWORD m_dwGlobalStartTickCount;

	// 指示程序是否退出
	static volatile bool m_bGlobalEnd;

	// PPLIVE目录
	//static tstring m_strPPLiveFolder;

	// PPVA目录
	//static tstring m_strPPVAFolder;

	// 临时目录
	static tstring m_strTmpFolder;
	static tstring m_strTmpFolder_INI;

	// 当前目录
	static tstring m_strCurrentFolder;

	static void Init();

	//static void GetPPLiveFolder();

	//static void GetPPVAFolder();
 
	static void GetTmpFolder();

	static void GetCurrentFolder();
private:
	CAppDM();
};
