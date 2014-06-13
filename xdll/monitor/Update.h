#pragma once

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <tchar.h>
#include "DefConst.h"

using namespace std;

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

typedef map<tstring, tstring>		MAP_TSTR_TSTR;
typedef vector<tstring>				VECTOR_TSTRING;

struct ModuleInfo_Server
{
	// ģ����
	tstring strModuleName;
	
	// Section�ĸ���
	UINT uSectionCount;
	
	
	
	// ƥ���ϵ�Section������(-1��ʾû��ƥ����)
	int nMatchableSectionIndex;
	
	// ƥ���ϵ�Section��ϵ��(0~100) ���˼����Ƿ����ظ�ģ��
	UINT uRatio;

	// �Ƿ�����(����uRatio)
	BOOL bDownloadByRatio;
	
	// ƥ���ϵ�Section��ǿ��ģʽ(0 -> ��ȫǿ��   1 -> ǿ��   2 -> ��ǿ��)
	UINT uForce;
	
	// ƥ���ϵ�Section���ļ��汾
	tstring strVersion;
	
	// ƥ���ϵ�Section����������
	tstring strVersionDescTitle;
	// ƥ���ϵ�Section������
	tstring strVersionDesc;
	// ƥ���ϵ�Section����������(EN)
	tstring strVersionDescTitle_En;
	// ƥ���ϵ�Section������(EN)
	tstring strVersionDesc_En;
	
	// ƥ���ϵ�Section�����ص�ַ�б� ���ڷ���������
	VECTOR_TSTRING fileUrlVector;
	
	// ƥ���ϵ�Section�����ص�ַ�б������ѡ�еĵ�ַ
	tstring strFileUrl;
	
	ModuleInfo_Server()
	{
		Reset();
	}
	
	void Reset(
		tstring _strModuleName = PPLAP_EXE_FILE_NAME, 
		UINT _uSectionCount = 0,
		UINT _uRatio = 0,
		UINT _uForce = 0,
		BOOL _bDownloadByRatio = TRUE,
		tstring _strVersion = _T("0.0.0.0"),
		int _nMatchableSectionIndex = -1)
	{
		strModuleName = _strModuleName;
		uSectionCount = _uSectionCount;
		uForce = _uForce;
		uRatio = _uRatio;
		bDownloadByRatio = _bDownloadByRatio;
		strVersion = _strVersion;
		nMatchableSectionIndex = _nMatchableSectionIndex;
		strFileUrl = _T("");
		strVersionDescTitle = _T("");
		strVersionDesc = _T("");
		strVersionDescTitle_En = _T("");
		strVersionDesc_En = _T("");
	}
};
typedef vector<ModuleInfo_Server>		VECTOR_MIS;

struct ServerINI
{
	// 0 -> http
	// 1 -> p2p
	UINT uProtocolType;
	
	// �����������ַ
	tstring strProxy;
	
	VECTOR_MIS misVector;
	ServerINI()
	{
		uProtocolType = 0;
		strProxy = _T("");
	}
};

class CUpdate  
{
public:
	CUpdate();
	virtual ~CUpdate();

	BOOL DownloadServerINI();

	void ParseServerINI();

	BOOL Update();

	ServerINI m_serverINI;
};
