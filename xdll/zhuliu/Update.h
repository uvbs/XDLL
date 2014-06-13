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
	// 模块名
	tstring strModuleName;
	
	// Section的个数
	UINT uSectionCount;
	
	
	
	// 匹配上的Section的索引(-1表示没有匹配上)
	int nMatchableSectionIndex;
	
	// 匹配上的Section的系数(0~100) 按此计算是否下载该模块
	UINT uRatio;

	// 是否下载(根据uRatio)
	BOOL bDownloadByRatio;
	
	// 匹配上的Section的强制模式(0 -> 完全强制   1 -> 强制   2 -> 不强制)
	UINT uForce;
	
	// 匹配上的Section的文件版本
	tstring strVersion;
	
	// 匹配上的Section的描述标题
	tstring strVersionDescTitle;
	// 匹配上的Section的描述
	tstring strVersionDesc;
	// 匹配上的Section的描述标题(EN)
	tstring strVersionDescTitle_En;
	// 匹配上的Section的描述(EN)
	tstring strVersionDesc_En;
	
	// 匹配上的Section的下载地址列表 用于服务器分流
	VECTOR_TSTRING fileUrlVector;
	
	// 匹配上的Section的下载地址列表中最后被选中的地址
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
	
	// 代理服务器地址
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
