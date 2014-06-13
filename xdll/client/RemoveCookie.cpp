// RemoveCookie.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <atlbase.h>
#include <string>
#include <Wininet.h>
using namespace std;
#pragma comment(lib, "Wininet.lib")

#include <ppl/util/log.h>

//检查指定文件是否存在:
BOOL CheckFileExist(LPCTSTR FilePath)
{
	WIN32_FIND_DATA wfd;
	BOOL bValue = FALSE;
	HANDLE hFind = FindFirstFile(FilePath, &wfd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		bValue = TRUE;
	} 
	FindClose(hFind);
	return bValue;
}

//匹配指定域名成功的TXT文件，删之
BOOL MatchDelCookieTxt(int nMode, LPCTSTR szSpecifiedDomain, LPCTSTR szCookieName, LPCTSTR szCookiePath)
{
	USES_CONVERSION;

	if (!CheckFileExist(szCookiePath))
	{
		return TRUE;
	}

	//指定的域名
	string strSpecifiedDomain = T2A((LPTSTR)szSpecifiedDomain);

	//对通过API获取的szCookieName，如：Administration@pptv.com/,截取@和/之间的域名部分
	string strCookieName = T2A((LPTSTR)szCookieName);
	size_t found = strCookieName.find_last_of("@");
	if (string::npos == found)
	{// found == string::npos  没有找到@怎么办?
		return FALSE;
	}
	// 去掉末尾的“/”
	string strCookieDomain = strCookieName.substr(found+1);
	string::iterator it = strCookieDomain.end()-1;
	strCookieDomain.erase(it);


	BOOL willDeleteCookie = FALSE;

	if (1 == nMode)
	{//删除包含匹配域名的HttpCookie
		if ( NULL != StrStrIA(strCookieDomain.c_str(), strSpecifiedDomain.c_str()) )
			willDeleteCookie = TRUE;
	} 
	else
	{//删除完全匹配域名的HttpCookie
		if ( 0 == lstrcmpiA(strCookieDomain.c_str(), strSpecifiedDomain.c_str()) )
			willDeleteCookie = TRUE;
	}

	if (willDeleteCookie == FALSE)
		return FALSE;


	if (!DeleteFile(szCookiePath))
	{
		//删除szCookieName文件失败!!!
		return FALSE;
	}
	
	return TRUE;
}

//删除指定域名的HttpCookie, nMode = 0为完全匹配删除，nMode = 1为包含匹配删除
BOOL RemoveCookie(int nMode, LPCTSTR lpDomain)
{
	USES_CONVERSION;
	HANDLE hCacheEnumHandle = NULL;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;
	
    char c[1024] = {0};
	lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) c;
	//lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwSize];
	DWORD dwSize = 1024;
	lpCacheEntry->dwStructSize = dwSize;
	
	// URL search pattern (1st parameter) options are:  NULL ("*.*"), "cookie:" or "visited:".
	hCacheEnumHandle = FindFirstUrlCacheEntry(_T("cookie:"), lpCacheEntry, &dwSize);
	
	if(hCacheEnumHandle != NULL)
	{
		APP_EVENT( T2A(lpCacheEntry->lpszSourceUrlName)<<", "<<T2A(lpCacheEntry->lpszLocalFileName) );
		MatchDelCookieTxt(nMode, lpDomain, lpCacheEntry->lpszSourceUrlName, lpCacheEntry->lpszLocalFileName);
	}
	else
	{
		//APP_ERROR( "FindFirstUrlCacheEntry fails!!!" );
		return FALSE;
	}
	
	do 
	{
		dwSize = 1024;
		lpCacheEntry->dwStructSize = dwSize;
		// Notice that return values of FindNextUrlCacheEntry (BOOL) and 
		// FindFirstUrlCacheEntry (HANDLE) are different.
		if (FindNextUrlCacheEntry(hCacheEnumHandle, lpCacheEntry, &dwSize))
		{
			APP_EVENT( T2A(lpCacheEntry->lpszSourceUrlName)<<", "<<T2A(lpCacheEntry->lpszLocalFileName) );
			MatchDelCookieTxt(nMode, lpDomain, lpCacheEntry->lpszSourceUrlName, lpCacheEntry->lpszLocalFileName);
		}
		else
		{
			FindCloseUrlCache(hCacheEnumHandle);
			if( ERROR_NO_MORE_ITEMS == GetLastError() )
			{
				return TRUE; 
			}
			else
			{
				//APP_ERROR( "FindNextUrlCacheEntry fails!!!" );
				return FALSE;
			}
		}
	} while (TRUE);

	return FALSE;
}


//删除指定文件夹
BOOL DeleteDir(LPCTSTR szPath)
{
	USES_CONVERSION;

	WIN32_FIND_DATA finddata;
	HANDLE hfind;
	TCHAR chPath[MAX_PATH] = {0};
	
	TCHAR *lpStr = StrCpy(chPath, szPath);
	PathAppend(lpStr, _T("*"));
	
	hfind=FindFirstFile(chPath, &finddata);
	if(hfind==INVALID_HANDLE_VALUE)
	{
		//APP_EVENT( "DeleteDir(), FindFirstFile失败!" );
		return FALSE;
	}
	
	memset(chPath, 0, MAX_PATH);
	do
	{
		lpStr = StrCpy(chPath, szPath);
		PathAppend(lpStr, finddata.cFileName);
		
		if( 0 == StrCmpI(finddata.cFileName, _T("."))  
			|| 0 == StrCmpI(finddata.cFileName, _T("..")) 
			|| 0 == StrCmpI(finddata.cFileName, _T("desktop.ini")) )
		{	
			continue;
		}
		
		if( (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0 )
		{
			if (!DeleteFile(chPath))
			{
				//APP_EVENT( "DeleteDir(), DeleteFile失败!" );
				return FALSE;
			}
		}
		else
		{
			BOOL bDelDir = DeleteDir(chPath);
			if (!bDelDir)
			{
				//APP_EVENT( "DeleteDir(), DeleteDir失败!" );
				return FALSE;
			}
		}
		memset(chPath, 0, MAX_PATH);
	}while(FindNextFile(hfind, &finddata));
	
	FindClose(hfind);
	
	if(!RemoveDirectory(szPath))
	{
		//APP_EVENT( "DeleteDir(), RemoveDirectory失败!" << T2A(szPath) );
		return FALSE;
	} 
	
	return TRUE;	
}

BOOL CleanFlashCookie(int nMethod, LPCTSTR szDomain, LPCTSTR lPath)  
{  
	USES_CONVERSION;
	
	TCHAR chPath[MAX_PATH] = {0};  
	TCHAR *lpStr = StrCpy(chPath, lPath);
	PathAppend(lpStr, _T("*"));
	
	WIN32_FIND_DATA FindFileData; 
	HANDLE hFind=::FindFirstFile(chPath, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
	{
		//APP_ERROR( "CleanFlashCookie(), FindFirstFile失败!" );
		return FALSE;
	}
	
	memset(chPath, 0, MAX_PATH);
	do 
	{
		if ( 0 == StrCmpI(FindFileData.cFileName, _T("."))  
			|| 0 == StrCmpI(FindFileData.cFileName, _T("..")) )
		{
			memset(chPath, 0, MAX_PATH);
			continue;
		}
		
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			lpStr = StrCpy(chPath, lPath);
			PathAppend(lpStr, FindFileData.cFileName);
			if (0 == nMethod) //删除包含指定部分域名的FlashCookie
			{
				if( NULL != StrStrI(FindFileData.cFileName, szDomain) )
				{
					BOOL bDelDir = DeleteDir(chPath);
					if (!bDelDir)
					{
						//APP_EVENT( "CleanFlashCookie(), DeleteDir失败!" << T2A(chPath) );
						return FALSE;
					}
					//APP_EVENT( "CleanFlashCookie(), DeleteDir成功..." << T2A(chPath) );
				}
				else
				{
					if ( !CleanFlashCookie(nMethod, szDomain, chPath) )
					{
						//APP_EVENT("CleanFlashCookie(), 递归CleanFlashCookie失败!");
						return FALSE;
					}
				}
			}
			else
			{
				if( 0 == StrCmpI(FindFileData.cFileName, szDomain) )
				{
					BOOL bDelDir = DeleteDir(chPath);
					if (!bDelDir)
					{
						//APP_EVENT( "CleanFlashCookie(), DeleteDir失败!" << T2A(chPath) );
						return FALSE;
					}
					//APP_EVENT( "CleanFlashCookie(), DeleteDir成功..." << T2A(chPath) );
				}
				else
				{
					if ( !CleanFlashCookie(nMethod, szDomain, chPath) )
					{
						//APP_EVENT( "CleanFlashCookie(), 递归CleanFlashCookie失败!" );
						return FALSE;
					}
				}
			}
		}
		memset(chPath, 0, MAX_PATH);
	}while(FindNextFile(hFind,  &FindFileData)); 
	
	FindClose(hFind);  
	return TRUE;
}

//指定域名的FlashCookie, method=1 全匹配删除
BOOL RemoveFlashCookie(LPCTSTR lpDomain, int method)
{
	TCHAR chFlashCookiePath[MAX_PATH] = {0};
	::SHGetSpecialFolderPath(NULL, chFlashCookiePath, CSIDL_APPDATA, 0);
	PathAppend(chFlashCookiePath, _T("Macromedia\\Flash Player\\#SharedObjects"));
	//APP_EVENT( "FlashCookie的存放路径" << T2A(chFlashCookiePath) );
	
	if (!CleanFlashCookie(method, lpDomain, chFlashCookiePath))
	{
		//APP_ERROR( "RemoveFlashCookie(), 清除FlashCookie失败!" );
	}
	
	return FALSE;
}

/*
int _tmain(int argc, _TCHAR* argv[])
{
	RemoveCookie(1, _T("sogou.com")); //1,表示删除包含sogou.com域名的cookie
	RemoveCookie(0,_T("pptv.com"));   //0,表示删除完全匹配pptv.com域名的cookie
	return 0;
}
*/
