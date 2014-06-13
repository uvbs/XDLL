#ifndef _PATH_AND_DISK_H_
#define _PATH_AND_DISK_H_

#include "StrUtils.h"
#include <map>
#include <winioctl.h>

using namespace std;


class PathAndDisk
{
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

	struct LargerNum
	{
		bool operator()(const ULARGE_INTEGER s1, const ULARGE_INTEGER s2) const
		{
			return (s1.QuadPart > s2.QuadPart);
		}
	};

	// retrieve the properties of a storage device or adapter. 
	typedef enum _STORAGE_PROPERTY_ID {
		StorageDeviceProperty = 0,
		StorageAdapterProperty,
		StorageDeviceIdProperty

	} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

	// retrieve the properties of a storage device or adapter. 
	typedef enum _STORAGE_QUERY_TYPE {
		PropertyStandardQuery = 0,
		PropertyExistsQuery,
		PropertyMaskQuery,
		PropertyQueryMaxDefined

	} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

	// retrieve the properties of a storage device or adapter. 
	typedef struct _STORAGE_PROPERTY_QUERY {
		STORAGE_PROPERTY_ID  PropertyId;
		STORAGE_QUERY_TYPE  QueryType;
		UCHAR  AdditionalParameters[1];

	} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

	// retrieve the storage device descriptor data for a device. 
	typedef struct _STORAGE_DEVICE_DESCRIPTOR {
		ULONG  Version;
		ULONG  Size;
		UCHAR  DeviceType;
		UCHAR  DeviceTypeModifier;
		BOOLEAN  RemovableMedia;
		BOOLEAN  CommandQueueing;
		ULONG  VendorIdOffset;
		ULONG  ProductIdOffset;
		ULONG  ProductRevisionOffset;
		ULONG  SerialNumberOffset;
		STORAGE_BUS_TYPE  BusType;
		ULONG  RawPropertiesLength;
		UCHAR  RawDeviceProperties[1];

	} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

	typedef map<ULARGE_INTEGER, tstring, LargerNum> MAP_LGDISK;

public:
	// 如果目录末尾没有反斜杠 则加上
	static tstring Path_AddBackslash(const tstring& strFolder);

	// 如果目录末尾有反斜杠 则去掉
	static tstring Path_RemoveBackslash(const tstring& strFolder);

	// 将目录和文件名合并成完整路径
	static tstring Path_Combine(const tstring& strFolder, const tstring& strFileName);
	
	// 根据完整路径 得到目录
	static tstring Path_GetFolder(const tstring& strFileFullPath);

	// 根据完整路径或URL 得到文件名
	static tstring Path_GetFileNameFromPathOrUrl(const tstring& strFileFullPath);
	
	// 根据完整文件名 得到文件名和扩展名
	static void Path_GetFileTitleAndExtName(const tstring& strFileName, tstring& strFileTitle, tstring& strExtName, bool hasDot);
	
	// 文件或目录是否存在
	static bool Path_FileOrFolderExist(const tstring& strFileFullPath);

	// 创建目录
	static void Path_CreateFolder(const tstring& strFolder);

	// 删除空目录
	static bool Path_RemoveFolder(const tstring& strFolder);

	// 验证文件名是否合法(仅从名称上验证 不考虑文件是否已经存在等情况)
	static bool Path_IsFileNameGood(const tstring& strFileName);

	// 验证目录是否合法(即能否在本机建立这个目录 不考虑目录已经存在的情况)
	static bool Path_IsFolderGood(const tstring& strFolder);

	// 得到合法的文件名(替换非法字符)
	static tstring Path_GetValidFileName(const tstring& strFileName, const tstring& strInstead);

	// 根据给定的完整路径 得到唯一的文件名
	// strFileFullPath -> c:\1.txt 
	// (假设已经存在c:\1.txt)
	// ret -> 1[1].txt
	static tstring Path_GetUniqueFileName(const tstring& strFileFullPath);
	
	// 得到所有分区
	static VECTOR_TSTRING Disk_GetAllDiskPartition();

	// 得到某一分区的剩余空间
	static bool Disk_GetFreeSpace(const tstring& strDisk, UINT64 *pFreeBytes);

	// 获得剩余空间最大的分区
	static bool Disk_GetMaxFreeDisk(tstring& strDisk);

	// 获得剩余空间最大的分区 非系统盘优先
	static bool Disk_GetMaxFreeDisk_NonSys(tstring& strDisk);

	// 是否是本地分区
	static bool IsHardDisk(const tstring& strDisk);

	// 是否是系统分区
	static bool IsSystemDisk(const tstring& strDisk);

	// 是否是USB分区
	static bool IsDiskUsb(const tstring& strDisk);

	// 得到所有本地磁盘分区及其剩余空间大小
	static bool GetAllDiskFreeSpace(MAP_LGDISK& mapLGDisk);

#ifdef __AFXWIN_H__
	static CString Path_AddBackslash(const CString& strFolder);
	static CString Path_RemoveBackslash(const CString& strFolder);
	static CString Path_Combine(const CString& strFolder, const CString& strFileName);
	static CString Path_GetFolder(const CString& strFileFullPath);
	static CString Path_GetFileNameFromPathOrUrl(const CString& strFileFullPath);
	static void Path_GetFileTitleAndExtName(const CString& strFileName, CString& strFileTitle, CString& strExtName, bool hasDot);
	static bool Path_FileOrFolderExist(const CString& strFileFullPath);
	static void Path_CreateFolder(const CString& strFolder);
	static bool Path_RemoveFolder(const CString& strFolder);
	static bool Path_IsFileNameGood(const CString& strFileName);
	static bool Path_IsFolderGood(const CString& strFolder);
	static CString Path_GetValidFileName(const CString& strFileName, const CString& strInstead);
	static CString Path_GetUniqueFileName(const CString& strFileFullPath);
	static bool Disk_GetFreeSpace(const CString& strDisk, UINT64 *pFreeBytes);
	static bool Disk_GetMaxFreeDisk(CString& strDisk);
	static bool Disk_GetMaxFreeDisk_NonSys(CString& strDisk);
#endif

private:
	// 验证文件名(不包括扩展名)是否是以[n]结尾
	// 如是 得到[n]之前的字符串和n 返回true
	// 否则返回false
	static bool Fn_VerifyFileName(const tstring& strFileName, tstring& s, int& n);

	// 验证一个字符是否是非法字符
	static bool IsCharValid(const tstring& strChar);

	// 所有非法字符
	static const tstring m_SpecialCharacters[];
	
private:
	PathAndDisk();
};

#endif
