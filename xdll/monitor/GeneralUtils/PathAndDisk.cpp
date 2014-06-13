// 根据工程设置决定是否包含"stdafx.h"
#include "stdafx.h"
#include "PathAndDisk.h"
#include <Shlwapi.h>
#include <shlobj.h >

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")


const tstring PathAndDisk::m_SpecialCharacters[] = 
{	
	tstring(_T("\\")), tstring(_T("/")), tstring(_T(":")), tstring(_T("*")), tstring(_T("?")),
	tstring(_T("\"")), tstring(_T("<")), tstring(_T(">")), tstring(_T("|"))
};


#ifdef __AFXWIN_H__
CString PathAndDisk::Path_AddBackslash(const CString& strFolder)
{
	tstring ret = Path_AddBackslash(tstring(strFolder));
	return CString(ret.c_str());
}
#endif
tstring PathAndDisk::Path_AddBackslash(const tstring& strFolder)
{
	TCHAR tPath[MAX_PATH + 1] = { 0 };
	_tcscpy(tPath, strFolder.c_str());
	::PathAddBackslash(tPath);
	return tstring(tPath);
}


#ifdef __AFXWIN_H__
CString PathAndDisk::Path_RemoveBackslash(const CString& strFolder)
{
	tstring ret = Path_RemoveBackslash(tstring(strFolder));
	return CString(ret.c_str());
}
#endif
tstring PathAndDisk::Path_RemoveBackslash(const tstring& strFolder)
{
	TCHAR tPath[MAX_PATH + 1] = { 0 };
	_tcscpy(tPath, strFolder.c_str());
	::PathRemoveBackslash(tPath);
	return tstring(tPath);
}


#ifdef __AFXWIN_H__
CString PathAndDisk::Path_Combine(const CString& strFolder, const CString& strFileName)
{
	tstring ret = Path_Combine(tstring(strFolder), tstring(strFileName));
	return CString(ret.c_str());
}
#endif
tstring PathAndDisk::Path_Combine(const tstring& strFolder, const tstring& strFileName)
{
	TCHAR tPath1[MAX_PATH + 1] = { 0 };
	TCHAR tPath2[MAX_PATH + 1] = { 0 };
	TCHAR tPath3[MAX_PATH + 1] = { 0 };
	_tcscpy(tPath1, strFolder.c_str());
	_tcscpy(tPath2, strFileName.c_str());
	::PathCombine(tPath3, tPath1, tPath2);
	return tstring(tPath3);
}


#ifdef __AFXWIN_H__
CString PathAndDisk::Path_GetFolder(const CString& strFileFullPath)
{
	tstring ret = Path_GetFolder(tstring(strFileFullPath));
	return CString(ret.c_str());
}
#endif
tstring PathAndDisk::Path_GetFolder(const tstring& strFileFullPath)
{
	TCHAR tPath[MAX_PATH + 1] = { 0 };
	_tcscpy(tPath, strFileFullPath.c_str());
	::PathRemoveFileSpec(tPath);
	return tstring(tPath);
}


#ifdef __AFXWIN_H__
CString PathAndDisk::Path_GetFileNameFromPathOrUrl(const CString& strFileFullPath)
{
	tstring ret = Path_GetFileNameFromPathOrUrl(tstring(strFileFullPath));
	return CString(ret.c_str());
}
#endif
tstring PathAndDisk::Path_GetFileNameFromPathOrUrl(const tstring& strFileFullPath)
{
	tstring s(_T(""));
	int n = StrUtils::IndexOfLast(strFileFullPath, tstring(_T("\\")));
	if (n == -1)
	{
		n = StrUtils::IndexOfLast(strFileFullPath, tstring(_T("/")));
		if (n == -1)
		{
			return s;
		}
	}

	if ((int)strFileFullPath.size() > n + 1)
	{
		s = strFileFullPath.substr(n + 1);
	}
	return s;
}


#ifdef __AFXWIN_H__
void PathAndDisk::Path_GetFileTitleAndExtName(const CString& strFileName, CString& strFileTitle, CString& strExtName, bool hasDot)
{
	tstring strFileTitle1(strFileTitle);
	tstring strExtName1(strExtName);
	Path_GetFileTitleAndExtName(tstring(strFileName), strFileTitle1, strExtName1, hasDot);
	strFileTitle = CString(strFileTitle1.c_str());
	strExtName = CString(strExtName1.c_str());
}
#endif
void PathAndDisk::Path_GetFileTitleAndExtName(const tstring& strFileName, tstring& strFileTitle, tstring& strExtName, bool hasDot)
{
	int n = StrUtils::IndexOfLast(strFileName, tstring(_T(".")));
	int n2 = (int)strFileName.size();
	if (n == -1)
	{
		strFileTitle = strFileName;
		strExtName = _T("");
	} 
	else
	{
		strFileTitle = strFileName.substr(0, n);
		strExtName = hasDot ? strFileName.substr(n, n2 - n) : strFileName.substr(n + 1, n2 - n - 1);
	}
}


#ifdef __AFXWIN_H__
bool PathAndDisk::Path_FileOrFolderExist(const CString& strFileFullPath)
{
	return Path_FileOrFolderExist(tstring(strFileFullPath));
}
#endif
bool PathAndDisk::Path_FileOrFolderExist(const tstring& strFileFullPath)
{
	return PathFileExists(strFileFullPath.c_str()) != 0;
}


#ifdef __AFXWIN_H__
void PathAndDisk::Path_CreateFolder(const CString& strFolder)
{
	Path_CreateFolder(tstring(strFolder));
} 
#endif
void PathAndDisk::Path_CreateFolder(const tstring& strFolder)
{
	SHCreateDirectoryEx(NULL, strFolder.c_str(), NULL);
}


#ifdef __AFXWIN_H__
bool PathAndDisk::Path_RemoveFolder(const CString& strFolder)
{
	return Path_RemoveFolder(tstring(strFolder));
} 
#endif
bool PathAndDisk::Path_RemoveFolder(const tstring& strFolder)
{
	return ::RemoveDirectory(strFolder.c_str()) ? true : false;
}


#ifdef __AFXWIN_H__
bool PathAndDisk::Path_IsFileNameGood(const CString& strFileName)
{
	return Path_IsFileNameGood(tstring(strFileName));
}
#endif
bool PathAndDisk::Path_IsFileNameGood(const tstring& strFileName)
{
	if (strFileName.size() == 0)
	{
		return false;
	}
	if (strFileName.substr(0, 1) == tstring(_T(" ")))
	{
		return false;
	}
	for (tstring::const_iterator it = strFileName.begin(); it != strFileName.end(); it++)
	{
		tstring s;
		s = *it;
		if (!IsCharValid(s))
			return false;
	}
	return true;
}


#ifdef __AFXWIN_H__
bool PathAndDisk::Path_IsFolderGood(const CString& strFolder)
{
	return Path_IsFolderGood(tstring(strFolder));
}
#endif
bool PathAndDisk::Path_IsFolderGood(const tstring& strFolder)
{
	if (strFolder.size() < 2)
	{
		return false;
	}
	else
	{
		VECTOR_TSTRING vt = Disk_GetAllDiskPartition();
		if (strFolder.size() == 2)
		{
			// 根目录
			tstring strDisk = strFolder.substr(0, 2);
			for (VECTOR_TSTRING::iterator it = vt.begin(); it != vt.end(); it++)
			{
				tstring validDisk = it->substr(0, 2);
				if (StrUtils::ToUpper(strDisk) == StrUtils::ToUpper(validDisk))
				{
					return true;
				}
			}
		}
		else if (strFolder.size() == 3)
		{
			// 根目录
			tstring strDisk = strFolder.substr(0, 3);
			for (VECTOR_TSTRING::iterator it = vt.begin(); it != vt.end(); it++)
			{
				tstring validDisk = it->substr(0, 3);
				if (StrUtils::ToUpper(strDisk) == StrUtils::ToUpper(validDisk))
				{
					return true;
				}
			}
		}
		else
		{
			// 多级目录
			tstring strDisk = strFolder.substr(0, 3);
			for (VECTOR_TSTRING::iterator it = vt.begin(); it != vt.end(); it++)
			{
				tstring validDisk = it->substr(0, 3);
				if (StrUtils::ToUpper(strDisk) == StrUtils::ToUpper(validDisk))
				{
					// 盘符合法 接下来验证每一层目录名是否合法
					tstring strOther = strFolder.substr(3);
					VECTOR_TSTRING vtOther;
					StrUtils::SplitString(strOther, tstring(_T("\\")), vtOther);
					for (size_t i = 0; i < vtOther.size(); i++)
					{
						if (!Path_IsFileNameGood(vtOther[i]))
							return false;
					}
					return true;
				}
			}
		}
	}
	return false;
}


#ifdef __AFXWIN_H__
CString PathAndDisk::Path_GetValidFileName(const CString& strFileName, const CString& strInstead)
{
	tstring ret = Path_GetValidFileName(tstring(strFileName), tstring(strInstead));
	return CString(ret.c_str());
}
#endif
tstring PathAndDisk::Path_GetValidFileName(const tstring& strFileName, const tstring& strInstead)
{
	tstring ret(strFileName);
	int n = sizeof(m_SpecialCharacters) / sizeof(tstring);
	for (int i = 0; i < n; i++)
	{
		ret = StrUtils::Replace(ret, m_SpecialCharacters[i], strInstead);
	}
	return ret;
}


#ifdef __AFXWIN_H__
CString PathAndDisk::Path_GetUniqueFileName(const CString& strFileFullPath)
{
	tstring ret = Path_GetUniqueFileName(tstring(strFileFullPath));
	return CString(ret.c_str());
}
#endif
tstring PathAndDisk::Path_GetUniqueFileName(const tstring& strFileFullPath)
{
	tstring strFileName = Path_GetFileNameFromPathOrUrl(strFileFullPath);
	tstring strFolder = Path_GetFolder(strFileFullPath);
	int i = 1;
	while (Path_FileOrFolderExist(Path_Combine(strFolder, strFileName)))
	{
		tstring strFileTitle(_T(""));
		tstring strExtName(_T(""));
		Path_GetFileTitleAndExtName(strFileName, strFileTitle, strExtName, true);

		tstring s(_T(""));
		int n = 0;
		tstring s1 = tstring(_T("[")) + StrUtils::GetStrByINT64(i) + tstring(_T("]")) + strExtName;
		if (Fn_VerifyFileName(strFileTitle, s, n))
		{
			strFileName = s + s1;
		}
		else
		{
			strFileName = strFileTitle + s1;
		}
		i++;
	}
	return strFileName;
}


#ifdef __AFXWIN_H__
bool PathAndDisk::Disk_GetFreeSpace(const CString& strDisk, UINT64 *pFreeBytes)
{
	return Disk_GetFreeSpace(tstring(strDisk), pFreeBytes);
}
#endif
bool PathAndDisk::Disk_GetFreeSpace(const tstring& strDisk, UINT64 *pFreeBytes)
{
	if (strDisk.size() < 2)
	{
		return false;
	}

	tstring root = strDisk.substr(0, 2);
	if (!PathFileExists(root.c_str()))
	{
		return false;
	}

	ULARGE_INTEGER uliUserFree,uliTotal,uliRealFree;
	GetDiskFreeSpaceEx(root.c_str(), &uliUserFree, &uliTotal, &uliRealFree);

	*pFreeBytes = uliRealFree.QuadPart;
	return true;
}


#ifdef __AFXWIN_H__
bool PathAndDisk::Disk_GetMaxFreeDisk(CString& strDisk)
{
	tstring strDisk1(strDisk);
	bool ret = Disk_GetMaxFreeDisk(strDisk1);
	strDisk = CString(strDisk1.c_str());
	return ret;
}
#endif
bool PathAndDisk::Disk_GetMaxFreeDisk(tstring& strDisk)
{
	MAP_LGDISK mapLGDisk;
	if (!GetAllDiskFreeSpace(mapLGDisk))
	{
		return false;
	}

	if (mapLGDisk.size() > 0)
	{
		MAP_LGDISK::iterator it = mapLGDisk.begin();
		strDisk = it->second;
		return true;
	} 
	else
	{
		return false;
	}
}


#ifdef __AFXWIN_H__
bool PathAndDisk::Disk_GetMaxFreeDisk_NonSys(CString& strDisk)
{
	tstring strDisk1(strDisk);
	bool ret = Disk_GetMaxFreeDisk_NonSys(strDisk1);
	strDisk = CString(strDisk1.c_str());
	return ret;
}
#endif
bool PathAndDisk::Disk_GetMaxFreeDisk_NonSys(tstring& strDisk)
{
	MAP_LGDISK mapLGDisk;
	if (!GetAllDiskFreeSpace(mapLGDisk))
	{
		return false;
	}

	if (mapLGDisk.size() > 0)
	{
		MAP_LGDISK::iterator it = mapLGDisk.begin();
		if (mapLGDisk.size() == 1)
		{
			strDisk = it->second;
			return true;
		}
		else
		{
			while (it != mapLGDisk.end())
			{
				if (!IsSystemDisk(it->second.c_str()))
				{
					strDisk = it->second;
					return true;
				}
				it++;
			}
		}
	} 
	else
	{
		return false;
	}

	return true;
}


VECTOR_TSTRING PathAndDisk::Disk_GetAllDiskPartition()
{
	MAP_LGDISK mapLGDisk;
	GetAllDiskFreeSpace(mapLGDisk);

	VECTOR_TSTRING vectorDisk;
	for (MAP_LGDISK::iterator it = mapLGDisk.begin(); it != mapLGDisk.end(); it++)
	{
		vectorDisk.push_back(it->second);
	}

	return vectorDisk;
}


bool PathAndDisk::IsHardDisk(const tstring& strDisk)
{  
	if(strDisk.size() < 3)
	{
		return false;
	}
	TCHAR pDisk[4];
	memcpy(pDisk, strDisk.c_str(), sizeof(TCHAR)*3);
	pDisk[3] = 0;
	if((GetDriveType(pDisk) != DRIVE_FIXED)|| (IsDiskUsb(pDisk)))
	{
		return false;
	}
	//由于A、B盘为软驱，将其忽略过
	if (*pDisk == _T('A') || *pDisk == _T('a') || *pDisk == _T('B') || *pDisk == _T('b'))
	{
		return false;
	}
	return true;
}


bool PathAndDisk::IsSystemDisk(const tstring& strDisk)
{
	TCHAR pDisk[MAX_PATH] = { 0 };
	_tcscpy(pDisk, strDisk.c_str());

	if (pDisk == NULL)
		return false;

	TCHAR strSysDir[MAX_PATH];
	ZeroMemory(strSysDir, MAX_PATH*sizeof(TCHAR));
	GetWindowsDirectory(strSysDir, MAX_PATH);

	//查看是否是系统盘
	if (strSysDir[0] == (pDisk[0]) || (strSysDir[0] + 32 == pDisk[0]) || (strSysDir[0] - 32 == pDisk[0]))
	{
		return true;
	}
	return false;
}


bool PathAndDisk::IsDiskUsb(const tstring& strDisk)
{
	TCHAR pDisk[MAX_PATH] = { 0 };
	_tcscpy(pDisk, strDisk.c_str());

	if (pDisk == NULL)
		return false;

	TCHAR strPath[20];
	ZeroMemory(strPath, 20*sizeof(TCHAR));
	_stprintf(strPath, _T("\\\\?\\%c:"), *pDisk);
	HANDLE hDisk = CreateFile(
		strPath, 
		GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, NULL, NULL);

	if (hDisk == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwOutBytes;
	BOOL bResult;

	STORAGE_PROPERTY_QUERY Query;
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;	
	pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
	pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

	bResult = ::DeviceIoControl(hDisk,
		IOCTL_STORAGE_QUERY_PROPERTY, // info of device property
		&Query, sizeof(STORAGE_PROPERTY_QUERY), // input data buffer
		pDevDesc, pDevDesc->Size, // output data buffer
		&dwOutBytes, // out's length
		(LPOVERLAPPED)NULL); 

	CloseHandle(hDisk);
	if (pDevDesc->BusType == BusTypeUsb)
	{
		delete[] pDevDesc;
		return true;
	}
	if (pDevDesc->BusType == BusType1394)
	{
		delete[] pDevDesc;
		return true;
	}

	delete[] pDevDesc;
	return false;
}


bool PathAndDisk::GetAllDiskFreeSpace(MAP_LGDISK& mapLGDisk)
{
	//GetLogicalDriveStrings,WINAPI，用来枚举逻辑盘符，如果两个参数都为NULL，
	//返回值则为逻辑盘符占用空间的大小(为N*(sizeof("c:\ ")) + 1)
	DWORD dwSize = GetLogicalDriveStrings(NULL, NULL);
	if (dwSize == NULL)
	{
		return false;
	}
	TCHAR *disk = new TCHAR[dwSize];
	assert(disk != NULL);
	ZeroMemory(disk, dwSize);
	GetLogicalDriveStrings(dwSize, (TCHAR*)disk);

	TCHAR *pDisk = disk;
	ULARGE_INTEGER ulDiskFreeSize;
	for (; (*pDisk != NULL) && (*(pDisk +1) == _T(':')); pDisk += 4)
	{
		//判断是否是本地硬盘
		if(!IsHardDisk(pDisk))
		{
			continue;
		}
		ZeroMemory(&ulDiskFreeSize, sizeof(ULARGE_INTEGER));
		//GetDiskFreeSpaceEx,WINAPI,正在枚举的逻辑分区的大小
		if(!GetDiskFreeSpaceEx(pDisk, &ulDiskFreeSize, NULL,NULL))
		{
			//光驱是不能获取大小的
			continue;
		}
		else
		{
			mapLGDisk[ulDiskFreeSize] = tstring(pDisk);
		}
	}
	delete [] disk;
	return true;
}








// private

bool PathAndDisk::Fn_VerifyFileName(const tstring& strFileName, tstring& s, int& n)
{
	int n1 = StrUtils::IndexOfLast(strFileName, tstring(_T("[")));
	int n2 = StrUtils::IndexOfLast(strFileName, tstring(_T("]")));
	if (n1 == -1 || n2 == -1 || n1 > n2 || n1 + 1 == n2 || n2 + 1 != strFileName.size())
	{
		return false;
	}

	s = strFileName.substr(0, n1);
	tstring strNum = strFileName.substr(n1 + 1, n2 - n1 - 1);
#ifdef _UNICODE
	swscanf(strNum.c_str(), _T("%d"), &n);
#else
	sscanf(strNum.c_str(), _T("%d"), &n);
#endif
	return true;
}

bool PathAndDisk::IsCharValid(const tstring& strChar)
{
	int n = sizeof(m_SpecialCharacters) / sizeof(tstring);
	for (int i = 0; i < n; i++)
	{
		if (m_SpecialCharacters[i] == strChar)
		{
			return false;
		}
	}
	return true;
}
