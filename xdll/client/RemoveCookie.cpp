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

//���ָ���ļ��Ƿ����:
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

//ƥ��ָ�������ɹ���TXT�ļ���ɾ֮
BOOL MatchDelCookieTxt(int nMode, LPCTSTR szSpecifiedDomain, LPCTSTR szCookieName, LPCTSTR szCookiePath)
{
	USES_CONVERSION;

	if (!CheckFileExist(szCookiePath))
	{
		return TRUE;
	}

	//ָ��������
	string strSpecifiedDomain = T2A((LPTSTR)szSpecifiedDomain);

	//��ͨ��API��ȡ��szCookieName���磺Administration@pptv.com/,��ȡ@��/֮�����������
	string strCookieName = T2A((LPTSTR)szCookieName);
	size_t found = strCookieName.find_last_of("@");
	if (string::npos == found)
	{// found == string::npos  û���ҵ�@��ô��?
		return FALSE;
	}
	// ȥ��ĩβ�ġ�/��
	string strCookieDomain = strCookieName.substr(found+1);
	string::iterator it = strCookieDomain.end()-1;
	strCookieDomain.erase(it);


	BOOL willDeleteCookie = FALSE;

	if (1 == nMode)
	{//ɾ������ƥ��������HttpCookie
		if ( NULL != StrStrIA(strCookieDomain.c_str(), strSpecifiedDomain.c_str()) )
			willDeleteCookie = TRUE;
	} 
	else
	{//ɾ����ȫƥ��������HttpCookie
		if ( 0 == lstrcmpiA(strCookieDomain.c_str(), strSpecifiedDomain.c_str()) )
			willDeleteCookie = TRUE;
	}

	if (willDeleteCookie == FALSE)
		return FALSE;


	if (!DeleteFile(szCookiePath))
	{
		//ɾ��szCookieName�ļ�ʧ��!!!
		return FALSE;
	}
	
	return TRUE;
}

//ɾ��ָ��������HttpCookie, nMode = 0Ϊ��ȫƥ��ɾ����nMode = 1Ϊ����ƥ��ɾ��
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


//ɾ��ָ���ļ���
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
		//APP_EVENT( "DeleteDir(), FindFirstFileʧ��!" );
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
				//APP_EVENT( "DeleteDir(), DeleteFileʧ��!" );
				return FALSE;
			}
		}
		else
		{
			BOOL bDelDir = DeleteDir(chPath);
			if (!bDelDir)
			{
				//APP_EVENT( "DeleteDir(), DeleteDirʧ��!" );
				return FALSE;
			}
		}
		memset(chPath, 0, MAX_PATH);
	}while(FindNextFile(hfind, &finddata));
	
	FindClose(hfind);
	
	if(!RemoveDirectory(szPath))
	{
		//APP_EVENT( "DeleteDir(), RemoveDirectoryʧ��!" << T2A(szPath) );
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
		//APP_ERROR( "CleanFlashCookie(), FindFirstFileʧ��!" );
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
			if (0 == nMethod) //ɾ������ָ������������FlashCookie
			{
				if( NULL != StrStrI(FindFileData.cFileName, szDomain) )
				{
					BOOL bDelDir = DeleteDir(chPath);
					if (!bDelDir)
					{
						//APP_EVENT( "CleanFlashCookie(), DeleteDirʧ��!" << T2A(chPath) );
						return FALSE;
					}
					//APP_EVENT( "CleanFlashCookie(), DeleteDir�ɹ�..." << T2A(chPath) );
				}
				else
				{
					if ( !CleanFlashCookie(nMethod, szDomain, chPath) )
					{
						//APP_EVENT("CleanFlashCookie(), �ݹ�CleanFlashCookieʧ��!");
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
						//APP_EVENT( "CleanFlashCookie(), DeleteDirʧ��!" << T2A(chPath) );
						return FALSE;
					}
					//APP_EVENT( "CleanFlashCookie(), DeleteDir�ɹ�..." << T2A(chPath) );
				}
				else
				{
					if ( !CleanFlashCookie(nMethod, szDomain, chPath) )
					{
						//APP_EVENT( "CleanFlashCookie(), �ݹ�CleanFlashCookieʧ��!" );
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

//ָ��������FlashCookie, method=1 ȫƥ��ɾ��
BOOL RemoveFlashCookie(LPCTSTR lpDomain, int method)
{
	TCHAR chFlashCookiePath[MAX_PATH] = {0};
	::SHGetSpecialFolderPath(NULL, chFlashCookiePath, CSIDL_APPDATA, 0);
	PathAppend(chFlashCookiePath, _T("Macromedia\\Flash Player\\#SharedObjects"));
	//APP_EVENT( "FlashCookie�Ĵ��·��" << T2A(chFlashCookiePath) );
	
	if (!CleanFlashCookie(method, lpDomain, chFlashCookiePath))
	{
		//APP_ERROR( "RemoveFlashCookie(), ���FlashCookieʧ��!" );
	}
	
	return FALSE;
}

/*
int _tmain(int argc, _TCHAR* argv[])
{
	RemoveCookie(1, _T("sogou.com")); //1,��ʾɾ������sogou.com������cookie
	RemoveCookie(0,_T("pptv.com"));   //0,��ʾɾ����ȫƥ��pptv.com������cookie
	return 0;
}
*/
