#pragma once


class CAppDM  
{
public:
	// ��ʵ�����
	static HANDLE m_hMutex;

	// ��������ʱ��
	static DWORD m_dwGlobalStartTickCount;

	// ָʾ�����Ƿ��˳�
	static volatile bool m_bGlobalEnd;

	// PPLIVEĿ¼
	//static tstring m_strPPLiveFolder;

	// PPVAĿ¼
	//static tstring m_strPPVAFolder;

	// ��ʱĿ¼
	static tstring m_strTmpFolder;
	static tstring m_strTmpFolder_INI;

	// ��ǰĿ¼
	static tstring m_strCurrentFolder;

	static void Init();

	//static void GetPPLiveFolder();

	//static void GetPPVAFolder();
 
	static void GetTmpFolder();

	static void GetCurrentFolder();
private:
	CAppDM();
};
