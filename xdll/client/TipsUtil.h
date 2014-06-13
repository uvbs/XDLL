#pragma once

#include <list>
#include <string>
#include "xdll.h"
using namespace std;

class TipsUtil
{
public:
	static bool IsComputerSafe();								// �������Ƿ��а�ȫ����
	static bool HasProcessRunning(std::list<string> items);		// �������Ƿ���������ָ������
	static string GetExeFileFolder();							// ���Exe�ļ���·����
	static bool Is64Operation(LPTSTR pszOS);					// ����ǲ���64��
	
	static void RunExeFile(string str);							// ִ��һ��Exe�ļ�
	static string GetHugoID();
	static string GetMachineGUID();
	static string GetPPID();
	static const char* AllTrim(char *pszBuffer);

	static string GetCurrentProcessName();						// ��õ�ǰ�Ľ��̵Ľ�����
	static UINT32 GetFlagValue();								// �����Ҫ���������Flagֵ
	static bool HasProcess(string iutName);						// �������Ʋ��� �Ƿ�����������

	static bool CanTipsRun();

	static bool CheckInstallSoftware();							// ��鰲װ�� ������� ���� ץ�����
	static bool CheckCurrentProcess();							// ��鵱ǰ�������еĽ��� �Ƿ��� �������� �� ץ������
	static bool CheckBrowserScriptDebugOff();					// �����Ƕ������Ƿ� �رյ���
	
	static void RemoveCookies(const std::list<string>& removeCookiesList, int method = 1);		// ɾ��IE��Cookie
	static void RemoveFlashCookies(const std::list<string>& removeCookiesList, int method = 0);

	static bool IsOnlyPPAP();									// ����ǲ���PPAP.exe��פ��̬

	static void OpenLimitMemory(bool bIsOpened);				// �������߹ر��ڴ�����

	static int GetAreaCode();
	static DWORD GetSeconds1970();
	static void FileTimeToTime_t(FILETIME ft, time_t *t);

	static string DecodeFile(const string& beforeFile, const string& tempName);
	
	static bool WCHARtoTCHAR(LPCWSTR lpcwszStr, LPTSTR lptszStr);
	static bool TCHARtoCHAR(LPCTSTR lpctszStr, LPSTR lpszStr);

	static string GetMacAddr();					//��û���MAC��ַ
	static string GetUserProfile();				//���ע����м��ܺ���û���¼��Ϣ
	static string GetUserName();				//���PPTV�û���¼�˺�
	static string GetResolution();				//�����Ļ�ֱ���
	static string GetFlashVer();				//���flashplayer�汾��
	static string GetSysLID();			//��ò���ϵͳĬ������

	static BOOL PP_GetAllUserAppData(LPTSTR pszCachePath);	//A pointer to a null-terminated string of length MAX_PATH which will receive the path
	
	static UINT RSHash(const char *str);				//�ַ�����ϣ
	static BOOL IsWow64();								//���ϵͳ�ǲ���64λ
	static CString GetJuid();
	static CString GetOsVersion();						//��ȡ����ϵͳ�汾��
	static CString GetFileVersion(LPTSTR szFilePath);	//����ļ��İ汾��

};