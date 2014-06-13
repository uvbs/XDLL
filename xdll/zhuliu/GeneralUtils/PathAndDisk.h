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
	// ���Ŀ¼ĩβû�з�б�� �����
	static tstring Path_AddBackslash(const tstring& strFolder);

	// ���Ŀ¼ĩβ�з�б�� ��ȥ��
	static tstring Path_RemoveBackslash(const tstring& strFolder);

	// ��Ŀ¼���ļ����ϲ�������·��
	static tstring Path_Combine(const tstring& strFolder, const tstring& strFileName);
	
	// ��������·�� �õ�Ŀ¼
	static tstring Path_GetFolder(const tstring& strFileFullPath);

	// ��������·����URL �õ��ļ���
	static tstring Path_GetFileNameFromPathOrUrl(const tstring& strFileFullPath);
	
	// ���������ļ��� �õ��ļ�������չ��
	static void Path_GetFileTitleAndExtName(const tstring& strFileName, tstring& strFileTitle, tstring& strExtName, bool hasDot);
	
	// �ļ���Ŀ¼�Ƿ����
	static bool Path_FileOrFolderExist(const tstring& strFileFullPath);

	// ����Ŀ¼
	static void Path_CreateFolder(const tstring& strFolder);

	// ɾ����Ŀ¼
	static bool Path_RemoveFolder(const tstring& strFolder);

	// ��֤�ļ����Ƿ�Ϸ�(������������֤ �������ļ��Ƿ��Ѿ����ڵ����)
	static bool Path_IsFileNameGood(const tstring& strFileName);

	// ��֤Ŀ¼�Ƿ�Ϸ�(���ܷ��ڱ����������Ŀ¼ ������Ŀ¼�Ѿ����ڵ����)
	static bool Path_IsFolderGood(const tstring& strFolder);

	// �õ��Ϸ����ļ���(�滻�Ƿ��ַ�)
	static tstring Path_GetValidFileName(const tstring& strFileName, const tstring& strInstead);

	// ���ݸ���������·�� �õ�Ψһ���ļ���
	// strFileFullPath -> c:\1.txt 
	// (�����Ѿ�����c:\1.txt)
	// ret -> 1[1].txt
	static tstring Path_GetUniqueFileName(const tstring& strFileFullPath);
	
	// �õ����з���
	static VECTOR_TSTRING Disk_GetAllDiskPartition();

	// �õ�ĳһ������ʣ��ռ�
	static bool Disk_GetFreeSpace(const tstring& strDisk, UINT64 *pFreeBytes);

	// ���ʣ��ռ����ķ���
	static bool Disk_GetMaxFreeDisk(tstring& strDisk);

	// ���ʣ��ռ����ķ��� ��ϵͳ������
	static bool Disk_GetMaxFreeDisk_NonSys(tstring& strDisk);

	// �Ƿ��Ǳ��ط���
	static bool IsHardDisk(const tstring& strDisk);

	// �Ƿ���ϵͳ����
	static bool IsSystemDisk(const tstring& strDisk);

	// �Ƿ���USB����
	static bool IsDiskUsb(const tstring& strDisk);

	// �õ����б��ش��̷�������ʣ��ռ��С
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
	// ��֤�ļ���(��������չ��)�Ƿ�����[n]��β
	// ���� �õ�[n]֮ǰ���ַ�����n ����true
	// ���򷵻�false
	static bool Fn_VerifyFileName(const tstring& strFileName, tstring& s, int& n);

	// ��֤һ���ַ��Ƿ��ǷǷ��ַ�
	static bool IsCharValid(const tstring& strChar);

	// ���зǷ��ַ�
	static const tstring m_SpecialCharacters[];
	
private:
	PathAndDisk();
};

#endif
