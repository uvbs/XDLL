#include "StdAfx.h"
#include "TipsUtil.h"
#include "urlconvert.h"

#include <ppl/util/log.h>
#include <ppl/concurrent/runnable.h>
#include <ppl/util/logtrace.h>
#include <ppl/stl/strings.h>
#include <ppl/stl/numeric.h>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <ppl/mswin/ipc.h>
#include <ppl/mswin/module.h>
#include "shlwapi.h"
#include "Urlmon.h"
#include "gstring.h"
#include "CRegKeyEx.h"

#include <Iphlpapi.h>
#include <time.h>
#include <synacast/util/SynacastUrlEncoding.h>


bool TipsUtil::IsOnlyPPAP()
{
	for( ProcessWalker pw; pw.HasMore(); pw.MoveNext() )
	{
		LPCTSTR strProcessName = pw.Current().szExeFile;
		if( StrStrI(strProcessName, gs[PROCESS_PPLive_exe].c_str() ) != NULL 
			|| StrStrI(strProcessName, gs[PROCESS_ppliveva_exe].c_str() ) != NULL 
			|| StrStrI(strProcessName, gs[PROCESS_PPLite_exe].c_str() ) != NULL		
			)
		{
			APP_ERROR("PPAP.exe is not Only Process !");
			return false;
		}
	}
	return true;
}

// void RemoveCookiesFromPath(CString& strPath, const std::list<string>& removeCookiesList)
// {
// 	CFileFind ff;
// 	BOOL bFind = ff.FindFile(strPath + "\\*.*");
// 	
// 	while(bFind)
// 	{
// 		bFind = ff.FindNextFile();
// 		
// 		if (ff.IsDots())
// 			continue;
// 		
// 		if (ff.IsDirectory())
// 		{
// 			//APP_EVENT("Find Directory: " << (LPCTSTR)ff.GetFilePath());
// 			RemoveCookiesFromPath(ff.GetFilePath(), removeCookiesList);
// 		}
// 		else
// 		{
// 			CString strFileName = ff.GetFileName();
// 			
// 			//APP_EVENT("Find Cookie File: " << (LPCTSTR)ff.GetFilePath());
// 			
// 			if(strFileName.Find(".txt") == -1)
// 				continue;
// 			bool is_find = false;
// 			for(std::list<string>::const_iterator iter = removeCookiesList.begin();
// 			iter != removeCookiesList.end(); iter ++)
// 			{
// 				const string& filter = (*iter);
// 				if( strFileName.Find(filter.c_str()) >= 0 )
// 				{
// 					is_find = true;
// 					APP_EVENT("Find Cookie File Hited: " << (LPCTSTR)ff.GetFilePath());
// 					break;
// 				}
// 			}
// 			
// 			if( is_find == true)
// 			{
// 				// 删除文件
// 				CFile temp;
// 				temp.Remove( (LPCTSTR)ff.GetFilePath() );
// 			}
// 		}
// 	}
// }

CString GetIECookiePath()
{
	// 检查操作系统版本号
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	
	TCHAR UserProfile[MAX_PATH];
	memset(UserProfile, 0, MAX_PATH);
	GetEnvironmentVariable(_T("USERPROFILE"), UserProfile, MAX_PATH);
	CString ieCookiePath = UserProfile;
	
	if(osvi.dwMajorVersion == 5)
	{
		ieCookiePath += _T("\\Cookies");
	}
	else if(osvi.dwMajorVersion == 6)
	{
		ieCookiePath += _T("\\AppData\\Roaming\\Microsoft\\Windows\\Cookies");
	}
	return ieCookiePath;
}

BOOL RemoveCookie(int nMode, LPCTSTR lpDomain);
BOOL RemoveFlashCookie(LPCTSTR lpDomain, int method = 0);

// 其中method表示删除Cookie的方式,0表示域名必须完全匹配才能删除,1表示域名部分匹配就能删除
void TipsUtil::RemoveCookies(const std::list<string>& removeCookiesList, int method)
{
	// removeCookiesList 删除这个URL的所有cookies
	// 全匹配
	// Vista,Win7 清除 Cookie
	//  CString ie_path = GetIECookiePath();	
	//  RemoveCookiesFromPath(ie_path, removeCookiesList);
	std::list<string>::const_iterator itr = removeCookiesList.begin();
	for (; itr != removeCookiesList.end(); itr++) {
		RemoveCookie(method, itr->c_str());
	}
}

///
// method == 0 method == 1 是全匹配删除
void TipsUtil::RemoveFlashCookies(const std::list<string>& removeFlashCookiesList, int method)
{
	std::list<string>::const_iterator itr = removeFlashCookiesList.begin();
	for (; itr != removeFlashCookiesList.end(); itr++) {
		RemoveFlashCookie(itr->c_str(), method);
	}
}

bool TipsUtil::CheckInstallSoftware()							// 检查安装软件
{
	// 判断安装项
	CRegKeyEx rk;
	
	CString strUninstall = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	if( ERROR_SUCCESS != rk.Open(HKEY_LOCAL_MACHINE, strUninstall, KEY_READ) )
	{
		APP_ERROR("Open Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall Error");
		rk.Close();
		return false;
	}
	
	for(u_int index = 0; ; index ++)
	{
		DWORD dwSubkeyNameLength = 256;
		TCHAR* sSubkeyName = new TCHAR[dwSubkeyNameLength+1];
		if( ERROR_SUCCESS != rk.EnumKey(index, sSubkeyName, &dwSubkeyNameLength) )
		{
			delete[] sSubkeyName;
			break;
		}
		
		do 
		{
			CRegKey rkSub;
			if( ERROR_SUCCESS != rkSub.Open(HKEY_LOCAL_MACHINE, strUninstall+_T("\\")+sSubkeyName, KEY_READ) )
			{
				rkSub.Close();
				break;
			}
			
			DWORD dwDisplayNameLength = 0;
			if(ERROR_SUCCESS != rkSub.QueryValue(NULL, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				break;
			}
			
			TCHAR* sDisplayName = new TCHAR[dwDisplayNameLength+1];
			if(ERROR_SUCCESS != rkSub.QueryValue(sDisplayName, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				delete[] sDisplayName;
				break;
			}
			
			if( StrStrI(sDisplayName, gs[DEV_Visual_Assist].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_Wireshark].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_WinPcap].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_Sniffer].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_Borland].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_Delphi].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_HTTP_Analyzer].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_IEWatch].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_NetLimiter].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_NetPeeker].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_Socket_Moniter].c_str() ) != NULL
				|| StrStrI(sDisplayName, gs[DEV_Visual_Studio].c_str() ) != NULL 
				|| StrStrI(sDisplayName, gs[DEV_Fiddler].c_str() ) != NULL
				)
			{
				rkSub.Close();
				rk.Close();
				delete[] sDisplayName;
				delete[] sSubkeyName;
				APP_ERROR("Intsall Software Error !");
				return false;
			}
			
			delete[] sDisplayName;
		} while(0);
		
		delete[] sSubkeyName;
	}
	rk.Close();

	return true;
}

bool TipsUtil::CheckCurrentProcess()							// 检查当前正在运行的进程
{
	// 判断当前进程	
	for( ProcessWalker pw; pw.HasMore(); pw.MoveNext() )
	{
		LPCTSTR strProcessName = pw.Current().szExeFile;
		if( StrStrI(strProcessName, gs[DEVP_SPYXX_EXE].c_str() ) != NULL 
			|| StrStrI(strProcessName, gs[DEVP_NPGUI_EXE].c_str() ) != NULL 
			|| StrStrI(strProcessName, gs[DEVP_NetPeeker_exe].c_str() ) != NULL 
			|| StrStrI(strProcessName, gs[DEVP_MSDEV_EXE].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_devenv_exe].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_procexp_exe].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_Filemon_exe].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_HttpAnalyzer].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_SOCKMON_EXE].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_WINIPMON_EXE].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_Fiddler_exe].c_str() ) != NULL
			// 			|| StrStrI(strProcessName, gs[DEVP_ppstream_exe].c_str() ) != NULL
			// 			|| StrStrI(strProcessName, gs[DEVP_ppsap_exe].c_str() ) != NULL
			// 			|| StrStrI(strProcessName, gs[DEVP_PPSGame_exe].c_str() ) != NULL
			|| StrStrI(strProcessName, gs[DEVP_CPal_exe].c_str() ) != NULL
			)
		{
			APP_ERROR("Running Process Error !");
			return false;
		}	
	}
	return true;
}

bool TipsUtil::CheckBrowserScriptDebugOff()
{
	// 检查内嵌浏览器是否 关闭调试
	CRegKey rk;

	// 检查 HCUK\Software\Microsoft\Internet Explorer\Main 是否存在
	if( ERROR_SUCCESS != rk.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main"), KEY_READ) )
	{
		APP_ERROR("Read HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Main");
		rk.Close();
		return false;
	}
	
	// 检查 Disable Script Debugger 是否存在
	DWORD dwValue = 0;
	if( ERROR_SUCCESS != rk.QueryValue(NULL, _T("Disable Script Debugger"), &dwValue) )
	{
		APP_ERROR("QueryValue \"Disable Script Debugger\" Error 1");
		rk.Close();
		return false;
	}
	
	TCHAR* sValue = new TCHAR[dwValue];
	if( ERROR_SUCCESS != rk.QueryValue(sValue, _T("Disable Script Debugger"), &dwValue) )
	{
		APP_ERROR("QueryValue \"Disable Script Debugger\" Error 2");
		rk.Close();
		delete[] sValue;
		return false;
	}
	rk.Close();
	
	// 检查 Disable Script Debugger 的值是否关闭
	TCHAR sDefaulttValue[] = _T("yes");
	if( 0 != _tcsicmp(sValue, sDefaulttValue) )
	{
		APP_ERROR("Register Disable Script Debugger is No");
		delete[] sValue;
		return false;
	}
	
	delete[] sValue;
	return true;
}

bool TipsUtil::CanTipsRun()
{
	#ifndef NEED_IMM
	// 判断安装项
	if (CheckInstallSoftware() == false)
	{
		APP_EVENT(_T("CheckInstallSoftware() false, so CanTipsRun is false"));
		return false;
	}

	if (CheckCurrentProcess() == false)
	{
		APP_EVENT(_T("CheckCurrentProcess() false, so CanTipsRun is false"));
		return false;
	}
	#endif
	
	// 判断IE浏览器调试
// 	if (!CheckBrowserScriptDebugOff())
// 	{
// 		APP_EVENT(_T("CheckBrowserScriptDebugOff false, so CanTipsRun is false"));
// 		return false;
// 	}
	
	return true;
}


string TipsUtil::GetCurrentProcessName()
{
	Module module;
	return module.GetFileName();		  
}

bool TipsUtil::HasProcess(string iutNameMore)
{
	string iutName = strings::lower( iutNameMore );
	for ( ProcessWalker walker; walker.HasMore(); walker.MoveNext() )
	{
		const PROCESSENTRY32& info = walker.Current();
		string name = info.szExeFile;
		strings::make_lower( name );
		if ( name.find( iutName ) != string::npos )
		{
			return true;
		}
	}
	return false;
}

UINT32 TipsUtil::GetFlagValue()
{
	UINT32 flag = 0;
	if( HasProcess(gs[PROCESS_iresea].c_str()) == true)
		flag = flag | 0x00000001;
	if( HasProcess(gs[PROCESS_PPStream].c_str()) == true)
		flag = flag | 0x00000002;
	if( HasProcess(gs[PROCESS_PPSAP].c_str()) == true)
		flag = flag | 0x00000004;
	
	CRegKeyEx rk;
	CString strUninstall = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	if( ERROR_SUCCESS != rk.Open(HKEY_LOCAL_MACHINE, strUninstall, KEY_READ) )
	{
		APP_ERROR("Open Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall Error");
		rk.Close();
		return flag;
	}
	
	for(u_int index = 0; ; index ++)
	{
		DWORD dwSubkeyNameLength = 256;
		TCHAR* sSubkeyName = new TCHAR[dwSubkeyNameLength+1];
		if( ERROR_SUCCESS != rk.EnumKey(index, sSubkeyName, &dwSubkeyNameLength) )
		{
			delete[] sSubkeyName;
			break;
		}
		
		do 
		{
			CRegKey rkSub;
			if( ERROR_SUCCESS != rkSub.Open(HKEY_LOCAL_MACHINE, strUninstall+_T("\\")+sSubkeyName, KEY_READ) )
			{
				rkSub.Close();
				break;
			}
			
			DWORD dwDisplayNameLength = 0;
			if(ERROR_SUCCESS != rkSub.QueryValue(NULL, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				break;
			}
			
			TCHAR* sDisplayName = new TCHAR[dwDisplayNameLength+1];
			if(ERROR_SUCCESS != rkSub.QueryValue(sDisplayName, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				delete[] sDisplayName;
				break;
			}
			
			if( StrStrI(sDisplayName, gs[PROCESS_PPSWangLuoDianShi].c_str()) != NULL ||
				StrStrI(sDisplayName, gs[PROCESS_PPStream].c_str()) != NULL)
			{
				flag = flag | 0x40000000;
			}
			
			delete[] sDisplayName;
		} while(0);
		
		delete[] sSubkeyName;
	}
	
	rk.Close();
	
	return flag;
}

string TipsUtil::GetMachineGUID()
{
	CRegKey rkSub;
	if( ERROR_SUCCESS != rkSub.Open(HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Cryptography"), KEY_READ | KEY_WOW64_64KEY) )
	{
		rkSub.Close();
		return "";
	}
	
	DWORD dwGUIDLength = MAX_PATH;
	TCHAR szGUID[MAX_PATH];
	if(ERROR_SUCCESS != rkSub.QueryValue(szGUID, _T("MachineGuid"), &dwGUIDLength) )
	{
		rkSub.Close();
		return "";
	}
	
	rkSub.Close();
	
	return szGUID;
}


string TipsUtil::GetPPID()
{
	CRegKey rkSub;
	if( ERROR_SUCCESS != rkSub.Open(HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), KEY_READ | KEY_WOW64_64KEY) )
	{
		rkSub.Close();
		return "";
	}

	DWORD dwPPIDLength = MAX_PATH;
	TCHAR szPPID[MAX_PATH];
	if(ERROR_SUCCESS != rkSub.QueryValue(szPPID, _T("Ppid"), &dwPPIDLength) )
	{
		rkSub.Close();
		return "";
	}

	rkSub.Close();
	
	return szPPID;
}

string TipsUtil::GetHugoID()
{
	char	szMacAddr[256] = {0}, szDiskSn[256] = {0};
	char	szPassword[256] = {0};
	PIP_ADAPTER_INFO pInfo=NULL;
	DWORD			dwLen = 0;
	
	typedef	DWORD	(WINAPI *LPGETADAPTERINFO)(PIP_ADAPTER_INFO pAdapterInfo,	PULONG pOutBufLen);
	LPGETADAPTERINFO	GetAdaptersInfoFunc = NULL;
	//load iphlpapi.dll
	HMODULE hModule = ::LoadLibrary(_T("iphlpapi.dll"));
	if (hModule)
	{
		GetAdaptersInfoFunc = (LPGETADAPTERINFO)GetProcAddress(hModule,	"GetAdaptersInfo");
	}
	if (!GetAdaptersInfoFunc)
	{
		FreeLibrary(hModule);
		return false;
	}
	
	DWORD	nError = GetAdaptersInfoFunc(pInfo, &dwLen);
	if (nError == 0)
	{
		sprintf(szMacAddr, "0x%02x%02x%02x%02x%02x%02x",
			pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
			pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);
	}
	else if (nError == ERROR_BUFFER_OVERFLOW)
	{
		pInfo = (PIP_ADAPTER_INFO)malloc(dwLen);
		nError	=	GetAdaptersInfoFunc(pInfo, &dwLen);
		if (nError==0)
		{
			sprintf(szMacAddr, "0x%02x%02x%02x%02x%02x%02x",
				pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
				pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);
		}
	}
	
	extern	char	*getHardDiskSerialNumber();
	strcpy(szDiskSn, getHardDiskSerialNumber());
	AllTrim(szDiskSn);
	
	int nCnt = strlen(szDiskSn) / sizeof(DWORD);
	DWORD	dwValue1 = 0;
	int i=0;
	for (; i<nCnt; i++)
	{
		dwValue1 ^= *((DWORD*)(szDiskSn) + i);
	}
	if (strlen(szDiskSn) % sizeof(DWORD))
		dwValue1 ^= *((DWORD*)(szDiskSn) + i);
	
	WORD	nValue2 = *(WORD*)&dwValue1 ^ *((WORD*)&dwValue1+1);
	
	strcpy(szPassword, szMacAddr);
	sprintf(szPassword+12, "%04x", nValue2);
	
	::FreeLibrary(hModule);
	
	string strDiskId = szPassword;
	return strDiskId;
}

const char* TipsUtil::AllTrim(char *pszBuffer)
{
	string strBuffer = pszBuffer;
	int nStart, nEnd;
	nStart = strBuffer.find_first_not_of(' ');
	nEnd = strBuffer.find_last_not_of(' ');
	if (nStart>=0 && nEnd>=0)
	{
		strBuffer = strBuffer.substr(nStart, nEnd-nStart+1);
		strcpy(pszBuffer, strBuffer.c_str());
	}
	
	return (const char*)pszBuffer;
}



bool TipsUtil::IsComputerSafe()
{
	// 检查进程项
	for( ProcessWalker pw; pw.HasMore(); pw.MoveNext() )
	{
		LPCTSTR strProcessName = pw.Current().szExeFile;
		if( // 瑞星
			StrStrI(strProcessName, gs[SAFEP_RavCopy_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_RavTask_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_rsnetsvr_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_RSTray_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_scanfrm_exe].c_str()) != NULL ||
			// 卡巴司机
			StrStrI(strProcessName, gs[SAFEP_avp_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_KVMonXP_kxp].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_KVSrvXP_exe].c_str()) != NULL ||
			// 360
			StrStrI(strProcessName, gs[SAFEP_360tray_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_360Safe_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_360rp_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_360sd_exe].c_str()) != NULL ||
			// Norton
			StrStrI(strProcessName, gs[SAFEP_AluSchedulerSvc_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_ccSvcHst_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_symlcsvc_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_comHost_exe].c_str()) != NULL ||
			// 金山
			StrStrI(strProcessName, gs[SAFEP_kavstart_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_kavbackreport_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_kissvc_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_kav32_exe].c_str()) != NULL ||
			// 天网防火墙
			StrStrI(strProcessName, gs[SAFEP_pfw_exe].c_str()) != NULL ||
			// McAfee
			StrStrI(strProcessName, gs[SAFEP_McNASvc_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_McfSrv_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_McSACore_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_McProxy_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_mcagent_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_mcmscsvc_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_mcshell_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_McSchield_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_mcsysmon_exe].c_str()) != NULL ||
			// NOD32
			StrStrI(strProcessName, gs[SAFEP_nod32kui_exe].c_str()) != NULL ||
			StrStrI(strProcessName, gs[SAFEP_nod32krn_exe].c_str()) != NULL )
		{
			return true;
		}
	}

	// 检查安装卸载项
	CRegKeyEx rk;
	CString strUninstall = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	if( ERROR_SUCCESS != rk.Open(HKEY_LOCAL_MACHINE, strUninstall, KEY_READ) )
	{
		APP_ERROR("Open Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall Error");
		rk.Close();
		return true;
	}

	for(u_int index = 0; ; index ++)
	{
		DWORD dwSubkeyNameLength = 256;
		TCHAR* sSubkeyName = new TCHAR[dwSubkeyNameLength+1];
		if( ERROR_SUCCESS != rk.EnumKey(index, sSubkeyName, &dwSubkeyNameLength) )
		{
			delete[] sSubkeyName;
			break;
		}

		do 
		{
			CRegKey rkSub;
			if( ERROR_SUCCESS != rkSub.Open(HKEY_LOCAL_MACHINE, strUninstall+_T("\\")+sSubkeyName, KEY_READ) )
			{
				rkSub.Close();
				break;
			}
			
			DWORD dwDisplayNameLength = 0;
			if(ERROR_SUCCESS != rkSub.QueryValue(NULL, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				break;
			}

			TCHAR* sDisplayName = new TCHAR[dwDisplayNameLength+1];
			if(ERROR_SUCCESS != rkSub.QueryValue(sDisplayName, _T("DisplayName"), &dwDisplayNameLength) )
			{
				rkSub.Close();
				delete[] sDisplayName;
				break;
			}

			if( StrStrI(sDisplayName, gs[SAFE_RuiXin].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_KaBaSiJi].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_JIangMing].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_360AnQuanWeiShi].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_360ShaDu].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_Norton].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_Symantec].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_JinShanDuBa].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_JinShanWangBiao].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_TianWangFangHuoQiang].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_McAfee].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_NOD32].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_ESET].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_MumaQingDaoFu].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_MuMaKeXin].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_EYiDAShi].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_WeiDianZhuDongFangYu].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_ETrust].c_str()) != NULL
				|| StrStrI(sDisplayName, gs[SAFE_AVG].c_str()) != NULL )
			{
				rkSub.Close();
				rk.Close();
				delete[] sDisplayName;
				delete[] sSubkeyName;
				APP_ERROR("Intsall Software Error !");
				return true;
			}

			delete[] sDisplayName;
		} while(0);

		delete[] sSubkeyName;
	}

	rk.Close();

	return false;
}

bool TipsUtil::HasProcessRunning(std::list<string> items)
{
	// 判断当前进程	
	for( ProcessWalker pw; pw.HasMore(); pw.MoveNext() )
	{
		LPCTSTR strProcessName = pw.Current().szExeFile;
		for(std::list<string>::iterator iter = items.begin();
			iter != items.end(); iter++ )
		{
			if( StrStrI(strProcessName, (*iter).c_str()) != NULL )
				return true;
		}
	}
	return false;
}

string TipsUtil::GetExeFileFolder()
{
	TCHAR pszCachePath[MAX_PATH] = { 0 };
	string ret;

	//通过注册表获取
	CRegKeyEx key;
	if (key.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), KEY_READ|KEY_QUERY_VALUE) == ERROR_SUCCESS)
	{
		DWORD	dwCount = MAX_PATH;
		LONG lRes = key.QueryValue(pszCachePath, _T("Common AppData"), &dwCount);
		if (lRes != ERROR_SUCCESS)
		{
			return "";
		}
		key.Close();
		
		ret.append(pszCachePath);
		ret.append("\\Tips\\");
		
		if( !PathIsDirectory(ret.c_str()))
		{	//不存在 ${Common AppData}/Tips 目录，需要创建
			if( FALSE == CreateDirectory(ret.c_str(), NULL) )
			{
				return "";
			};
		}
		return ret;
	}
	else
	{
		return "";
	}
}


void TipsUtil::RunExeFile(string str)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	
	TCHAR processName[MAX_PATH];
	strcpy(processName, str.c_str());
	
	if( !CreateProcess( NULL,   // No module name (use command line). 
		processName,			// Command line. 
		NULL,					// Process handle not inheritable. 
		NULL,					// Thread handle not inheritable. 
		FALSE,					// Set handle inheritance to FALSE. 
		0,						// No creation flags. 
		NULL,					// Use parent's environment block. 
		NULL,					// Use parent's starting directory. 
		&si,					// Pointer to STARTUPINFO structure.
		&pi )					// Pointer to PROCESS_INFORMATION structure.
		) 
	{
		APP_EVENT( "CreateProcess failed "<<GetLastError() );
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return;
	}	
	
	WaitForSingleObject( pi.hProcess, INFINITE );
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
}

void TipsUtil::OpenLimitMemory(bool bIsOpened)
{
	CRegKeyEx rk;
	if (ERROR_SUCCESS != rk.Open(HKEY_CURRENT_USER, _T("Software\\PPLive"), KEY_WRITE))
	{
		APP_ERROR("Open HKEY_CURRENT_USER\\Software\\PPLive Error");
		rk.Close();
		return;
	}

	DWORD v = 200;
	if (bIsOpened == false)
		v = 50;

	if (ERROR_SUCCESS != rk.SetDWORDValue(_T("memorysize"), v))
	{
		APP_ERROR("Set DWORD Value HKEY_CURRENT_USER\\Software\\PPLive\\memorysize Error");
		rk.Close();
		return;
	}

	rk.Close();
}

int TipsUtil::GetAreaCode()
{
	CRegKey rk;
	
	// 检查 HKCU\Software\PPlive 是否存在
	if( ERROR_SUCCESS != rk.Open(HKEY_CURRENT_USER,_T("SOFTWARE\\PPLive"), KEY_READ) )
	{
		APP_ERROR("Read HKEY_CURRENT_USER\\SOFTWARE\\PPLive");
		rk.Close();
		return false;
	}
	
	// 检查 citycode 是否存在
	TCHAR sValue[MAX_PATH] = {0};
	DWORD dwValue = MAX_PATH;
	if( ERROR_SUCCESS != rk.QueryValue(sValue, _T("citycode"), &dwValue) )
	{
		APP_ERROR("QueryValue \"citycode\" Error 1");
		rk.Close();
		return false;
	}
	sValue[dwValue] = '\0';
	rk.Close();
	return atoi(sValue);
}

DWORD TipsUtil::GetSeconds1970()
{
	time_t rawtime; 
	time(&rawtime);
	return (DWORD)rawtime;
}

void TipsUtil::FileTimeToTime_t(FILETIME ft, time_t *t)
{
	LONGLONG  ll;  
	
	ULARGE_INTEGER ui;  
	ui.LowPart = ft.dwLowDateTime;  
	ui.HighPart = ft.dwHighDateTime;  
	
	ll = (ft.dwHighDateTime << 32)  +  ft.dwLowDateTime;  
	
	*t =  ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);  
}

string TipsUtil::DecodeFile(const string& beforeFile, const string& tempName)
{
	TCHAR TempPath[MAX_PATH];
	memset(TempPath, 0, MAX_PATH);
	GetEnvironmentVariable(_T("TEMP"), TempPath, MAX_PATH);
	
	string decryptFilePath = TempPath;
	decryptFilePath = decryptFilePath + "\\" + tempName;

	// 读取解密文件
	ifstream ifs;
	ifs.open(beforeFile.c_str());
	ostringstream oss;
	char ch;
	while(oss && ifs.get(ch)) oss.put(ch);
	string before_content_string = oss.str();
	ifs.close();
	
	// 解密文件
	APP_EVENT("Before Decrypt Config String Length is \r\n"<<before_content_string.size());
	SynacastUrlEncoding encoding("19801219");
	string decrypt_content_string = encoding.Decode(before_content_string);
	APP_EVENT("After Decrypt Config String Length is \r\n"<<decrypt_content_string.size());
	
	// 将解密后内容写回文件
	ofstream ofs;
	ofs.open(decryptFilePath.c_str());
	ofs << decrypt_content_string;
	ofs.close();

	return decryptFilePath;
}

bool TipsUtil::WCHARtoTCHAR(LPCWSTR lpcwszStr, LPTSTR lptszStr)
{
#ifdef _UNICODE
	size_t count = wcslen(lpcwszStr);
	if (count > MAX_PATH - 1)
	{
		count = MAX_PATH - 1;
	}
	wcsncpy(lptszStr, lpcwszStr, count);
#else
	DWORD dwMinSize = WideCharToMultiByte(CP_ACP, NULL, lpcwszStr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, lpcwszStr, -1, lptszStr, dwMinSize, NULL, NULL);
#endif
	return true;
}

bool TipsUtil::TCHARtoCHAR(LPCTSTR lpctszStr, LPSTR lpszStr)
{
#ifdef _UNICODE 
	DWORD dwMinSize = WideCharToMultiByte(CP_ACP, NULL, lpctszStr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, lpctszStr, -1, lpszStr, dwMinSize, NULL, NULL);
#else
	size_t count = strlen(lpctszStr);
	if (count > MAX_PATH - 1)
	{
		count = MAX_PATH - 1;
	}
	strncpy(lpszStr, lpctszStr, count);
#endif
	return true;
}

//获得MAC
string TipsUtil::GetMacAddr()
{
	char szMacAddr[256] = "";
	
	PIP_ADAPTER_INFO pInfo=NULL;
	DWORD dwLen = 0;
	BOOL  bGetMacAddr = FALSE;
	
	typedef DWORD (WINAPI *LPGETADAPTERINFO)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
	LPGETADAPTERINFO   GetAdaptersInfoFunc = NULL;
	
	//load iphlpapi.dll
	HMODULE hModule = ::LoadLibrary(_T("iphlpapi.dll"));
	if (!hModule)
		return szMacAddr;
	
	GetAdaptersInfoFunc = (LPGETADAPTERINFO)GetProcAddress(hModule, "GetAdaptersInfo");
	
	if (GetAdaptersInfoFunc)
	{
		DWORD  nError = GetAdaptersInfoFunc(pInfo, &dwLen);
		
		if (nError == 0)
		{
			sprintf(szMacAddr, "%02x%02x%02x%02x%02x%02x",
				pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
				pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);
		}
		else if (nError == ERROR_BUFFER_OVERFLOW)
		{
			PIP_ADAPTER_INFO pAdapter = (PIP_ADAPTER_INFO)malloc(dwLen);
			pInfo = pAdapter;
			nError =   GetAdaptersInfoFunc(pInfo, &dwLen);
			if (nError==0)
			{
				while(pInfo)
				{
					if(pInfo->Address[0] || pInfo->Address[1] || pInfo->Address[2] || pInfo->Address[3] || pInfo->Address[4] ||pInfo->Address[5])
					{
						sprintf(szMacAddr, "%02x%02x%02x%02x%02x%02x",
							pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
							pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);
						break;
					}
					else
					{
						pInfo = pInfo->Next;
					}
				}
			}
			if(pAdapter)free(pAdapter);
		}
	}
	
	if (hModule)
		FreeLibrary(hModule);
	
	return _strupr(szMacAddr);
}

//获得注册表中加密后的用户登录信息
string TipsUtil::GetUserProfile()
{
	CRegKey rkSub;
	if( ERROR_SUCCESS != rkSub.Open(HKEY_CURRENT_USER, 
		_T("SOFTWARE\\PPLive\\passport"), KEY_READ | KEY_WOW64_64KEY) )
	{
		rkSub.Close();
		return "";
	}
	DWORD dwUserProfileLength = MAX_PATH;
	TCHAR chUserProfile[MAX_PATH];
	if(ERROR_SUCCESS != rkSub.QueryValue(chUserProfile, _T("userprofile2"), &dwUserProfileLength) )
	{
		rkSub.Close();
		return "";
	}
	rkSub.Close();
	
	char szUserProfile[MAX_PATH] = {0};
	TCHARtoCHAR(chUserProfile, szUserProfile);
	
	return szUserProfile;
}

//获得PPTV用户登录账号
string TipsUtil::GetUserName()
{
	string strUserProfile = GetUserProfile();
	
	if (strUserProfile.empty())
	{
		return "";
	}
	
	string strLogInfo = Decode(strUserProfile);
	
	//截取用户名
	size_t found = strLogInfo.find_first_of("&");
	if (string::npos == found)
	{
		return "";
	}
	string strAccount = strLogInfo.substr(0, found);
	
	return strAccount;
}

//获得屏幕分辨率
string TipsUtil::GetResolution()
{
	int width = GetSystemMetrics ( SM_CXSCREEN ); 
	int height= GetSystemMetrics ( SM_CYSCREEN ); 
	
	char strWidth[MAX_PATH] = {0}; 
	_itoa(width, strWidth, 10);
	char strHeight[MAX_PATH] = {0};
	_itoa(height, strHeight, 10);
	
	string strResolution = strWidth;
	strResolution += " * ";
	strResolution += strHeight;
	
	return strResolution;	
}

//获得flashplayer版本号
string TipsUtil::GetFlashVer()
{
	HKEY hFlashPlayer = NULL;
	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Macromedia\\FlashPlayer"), 0, KEY_READ, &hFlashPlayer))
	{
		return "";
	}
	
	DWORD dwType = REG_SZ;
	TCHAR chFlashVer[MAX_PATH] = {0};
	DWORD dwFlashVer = MAX_PATH;
	if(ERROR_SUCCESS != RegQueryValueEx(hFlashPlayer, 
		_T("CurrentVersion"), 0, &dwType, (LPBYTE)chFlashVer, &dwFlashVer))
	{
		return "";
	} 
	
	RegCloseKey(hFlashPlayer);
	
	char szFlashVer[MAX_PATH] = {0};
	TCHARtoCHAR(chFlashVer, szFlashVer);
	string strCurrentVer = szFlashVer;
	size_t found = strCurrentVer.find_first_of(',');
	while (found != string::npos)
	{
		strCurrentVer[found] = '.';
		found = strCurrentVer.find_first_of(',', found+1);
	}
	
	return strCurrentVer;
}

//获得操作系统默认语言
string TipsUtil::GetSysLID()
{
	LANGID lid = GetSystemDefaultLangID();
	char szSysLangID[MAX_PATH] = {0};
	sprintf(szSysLangID, "0x%x", lid);
	
	return szSysLangID;
}

BOOL TipsUtil::PP_GetAllUserAppData(LPTSTR pszCachePath
									/*A pointer to a null-terminated string of length MAX_PATH which will receive the path*/
									)
{
	//通过注册表获取
	BOOL bGetFromReg = FALSE;
	
	//通过SHELL获取
	HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, pszCachePath);
	if (FAILED(hr))
	{
		//通过环境变量获取"ALLUSERSPROFILE"
		DWORD  dwRet = GetEnvironmentVariable(_T("ALLUSERSPROFILE"), pszCachePath, MAX_PATH);
		if (dwRet)
		{
			PathAppend(pszCachePath, _T("Application Data"));
		}
		else
		{        //通过环境变量获取"ProgramFiles"
			dwRet = GetEnvironmentVariable(_T("ProgramFiles"), pszCachePath, MAX_PATH);
		}
		
		if (dwRet == 0)
		{
			HKEY hShell = NULL;
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), 0,KEY_READ|KEY_QUERY_VALUE, &hShell) == ERROR_SUCCESS)
			{
				DWORD dwType = REG_SZ;
				DWORD   dwCount = MAX_PATH;
				LONG lRes = RegQueryValueEx(hShell, _T("Common AppData"), 0, &dwType, (LPBYTE)pszCachePath, &dwCount);
				if (lRes == ERROR_SUCCESS)
				{
					bGetFromReg = TRUE;
				}
				RegCloseKey(hShell);
			}
		}
	}
	else
	{
		bGetFromReg = TRUE;
	}
	
	return bGetFromReg;
}

UINT TipsUtil::RSHash(const char *str)
{
	unsigned int b = 378551 ;
	unsigned int a = 63689 ;
	unsigned int hash = 0 ;
	
	while (*str)
	{
		hash = hash*a+(*str++);
		a *= b;
	} 
	
	return (hash & 0x7FFFFFFF);
}

BOOL TipsUtil::IsWow64() 
{ 
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
	LPFN_ISWOW64PROCESS fnIsWow64Process; 
	BOOL bIsWow64 = FALSE; 
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(_T("kernel32")),"IsWow64Process"); 
	if (NULL != fnIsWow64Process) 
	{ 
		fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
	} 
	return bIsWow64; 
} 

CString TipsUtil::GetJuid()
{
	TCHAR chUilibDLLPath[MAX_PATH] = {0};
	GetModuleFileName(theApp.m_hInstance, chUilibDLLPath, MAX_PATH);
	PathRemoveFileSpec(chUilibDLLPath);
	PathAppend(chUilibDLLPath, _T("uilib.dll"));
	
	WCHAR wszDiskId[256] = L"";
	HMODULE hModule = ::LoadLibrary(chUilibDLLPath);
	if (hModule)
	{
		typedef LPWSTR (__stdcall *LPGETDISKID)( LPWSTR pszDiskID, LPDWORD pdwCount);
		LPGETDISKID  pGetDiskId = (LPGETDISKID)GetProcAddress(hModule, "PP_GetDiskId");
		if (pGetDiskId)
		{
			DWORD dwSize = 256;
			pGetDiskId(wszDiskId, &dwSize);
		}
	}

	return wszDiskId;
}

CString TipsUtil::GetOsVersion()
{
	DWORD dwVersion = GetVersion();
	DWORD MajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD MinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	CString strOsVersion;
	strOsVersion.Format(_T("%d.%d"), MajorVersion, MinorVersion);
	return strOsVersion; 
}

CString TipsUtil::GetFileVersion(LPTSTR szFilePath)
{
	CString strFileVersion;
	
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;
	WORD nProdVersion[4];
	
	dwVerInfoSize = GetFileVersionInfoSize(szFilePath, &dwVerHnd);
	if (!dwVerInfoSize)
	{
		APP_ERROR("GetFileVersionInfoSize Error!!!");
		return strFileVersion;
	}
	// If we were able to get the information, process it:
	HANDLE  hMem;
	LPVOID  lpvMem;
	unsigned int uInfoSize = 0;
	
	hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
	lpvMem = GlobalLock(hMem);
	GetFileVersionInfo(szFilePath, dwVerHnd, dwVerInfoSize, lpvMem);
	
	BOOL b = ::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);
	if (FALSE == b)
	{
		APP_ERROR("VerQueryValue Error !!!");
		GlobalUnlock(hMem);
		GlobalFree(hMem);
		return strFileVersion;
	}
	
	// Product version from the FILEVERSION of the version info resource 
	nProdVersion[0] = HIWORD(pFileInfo->dwProductVersionMS); 
	nProdVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
	nProdVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
	nProdVersion[3] = LOWORD(pFileInfo->dwProductVersionLS); 
	
	GlobalUnlock(hMem);
	GlobalFree(hMem);
	
	strFileVersion.Format(_T("%d.%d.%d.%d"), nProdVersion[0], nProdVersion[1], nProdVersion[2], nProdVersion[3]);
	return strFileVersion;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

bool TipsUtil::Is64Operation(LPTSTR pszOS)
{
#define BUFSIZE 256

#define PRODUCT_UNDEFINED                       0x00000000

#define PRODUCT_ULTIMATE                        0x00000001
#define PRODUCT_HOME_BASIC                      0x00000002
#define PRODUCT_HOME_PREMIUM                    0x00000003
#define PRODUCT_ENTERPRISE                      0x00000004
#define PRODUCT_HOME_BASIC_N                    0x00000005
#define PRODUCT_BUSINESS                        0x00000006
#define PRODUCT_STANDARD_SERVER                 0x00000007
#define PRODUCT_DATACENTER_SERVER               0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER            0x00000009
#define PRODUCT_ENTERPRISE_SERVER               0x0000000A
#define PRODUCT_STARTER                         0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE          0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE            0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE          0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64          0x0000000F
#define PRODUCT_BUSINESS_N                      0x00000010
#define PRODUCT_WEB_SERVER                      0x00000011
#define PRODUCT_CLUSTER_SERVER                  0x00000012
#define PRODUCT_HOME_SERVER                     0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER          0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER         0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER        0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER       0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS        0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM    0x00000019

#define PRODUCT_UNLICENSED                      0xABCDABCD
#define PROCESSOR_ARCHITECTURE_AMD64 9
#define SM_SERVERR2 89
#define VER_SUITE_COMPUTE_SERVER 0x00004000
#define VER_SUITE_STORAGE_SERVER 0x00002000
#define VER_SUITE_BLADE 0x00000400
#define VER_SUITE_PERSONAL 0x00000200

	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		return true;

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	pGNSI = (PGNSI) GetProcAddress(
		GetModuleHandle(TEXT("kernel32.dll")), 
		"GetNativeSystemInfo");
	if(NULL != pGNSI)
		pGNSI(&si);
	else GetSystemInfo(&si);

	if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && 
		osvi.dwMajorVersion > 4 )
	{
		StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft "));

		// Test for the specific product.

		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Vista "));
			else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 " ));

			pGPI = (PGPI) GetProcAddress(
				GetModuleHandle(TEXT("kernel32.dll")), 
				"GetProductInfo");

			pGPI( 6, 0, 0, 0, &dwType);

			switch( dwType )
			{
			case PRODUCT_ULTIMATE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate Edition" ));
				break;
			case PRODUCT_HOME_PREMIUM:
				StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium Edition" ));
				break;
			case PRODUCT_HOME_BASIC:
				StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic Edition" ));
				break;
			case PRODUCT_ENTERPRISE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
				break;
			case PRODUCT_BUSINESS:
				StringCchCat(pszOS, BUFSIZE, TEXT("Business Edition" ));
				break;
			case PRODUCT_STARTER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Starter Edition" ));
				break;
			case PRODUCT_CLUSTER_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Cluster Server Edition" ));
				break;
			case PRODUCT_DATACENTER_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition" ));
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition (core installation)" ));
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition (core installation)" ));
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition for Itanium-based Systems" ));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server" ));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium Edition" ));
				break;
			case PRODUCT_STANDARD_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition" ));
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition (core installation)" ));
				break;
			case PRODUCT_WEB_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Web Server Edition" ));
				break;
			}
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				StringCchCat(pszOS, BUFSIZE, TEXT( ", 64-bit" ));
				return true; 
			}
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
			{
				StringCchCat(pszOS, BUFSIZE, TEXT(", 32-bit"));
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if( GetSystemMetrics(SM_SERVERR2) )
				StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Server 2003 R2, "));
			else if ( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER )
				StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Storage Server 2003"));
			else if( osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				StringCchCat(pszOS, BUFSIZE, TEXT( "Windows XP Professional x64 Edition"));
			}
			else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003, "));

			// Test for the server type.
			if ( osvi.wProductType != VER_NT_WORKSTATION )
			{
				if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition for Itanium-based Systems" ));
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition for Itanium-based Systems" ));
				}

				else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter x64 Edition" ));
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise x64 Edition" ));
					else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard x64 Edition" ));
				}

				else
				{
					if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Compute Cluster Edition" ));
					else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition" ));
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition" ));
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
						StringCchCat(pszOS, BUFSIZE, TEXT( "Web Edition" ));
					else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard Edition" ));
				}
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP "));
			if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
				StringCchCat(pszOS, BUFSIZE, TEXT( "Home Edition" ));
			else StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			StringCchCat(pszOS, BUFSIZE, TEXT("Windows 2000 "));

			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
			}
			else 
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Server" ));
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					StringCchCat(pszOS, BUFSIZE, TEXT( "Advanced Server" ));
				else StringCchCat(pszOS, BUFSIZE, TEXT( "Server" ));
			}
		}

		// Include service pack (if any) and build number.

		if( _tcslen(osvi.szCSDVersion) > 0 )
		{
			StringCchCat(pszOS, BUFSIZE, TEXT(" ") );
			StringCchCat(pszOS, BUFSIZE, osvi.szCSDVersion);
		}

		TCHAR buf[80];

		StringCchPrintf( buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
		StringCchCat(pszOS, BUFSIZE, buf);

		return false; 
	}

	else
	{  
		//printf( "This sample does not support this version of Windows.\n");
		return true;
	}
}

