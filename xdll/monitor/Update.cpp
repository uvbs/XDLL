#include "stdafx.h"
#include "Update.h"
#include "AppDM.h"
#include "Utils.h"

CUpdate::CUpdate()
{

}

CUpdate::~CUpdate()
{

}

BOOL CUpdate::DownloadServerINI()
{
	return NetUtils::DownloadFile_(SERVER_INI_URL, _T(""), CAppDM::m_strTmpFolder_INI);
}

void CUpdate::ParseServerINI()
{
	tstring strINIFullPath = CAppDM::m_strTmpFolder_INI;

	// 得到Module列表 和 每个Module的基本信息
	tstring strModules = RegAndProfile::GetINIStr(_T("Full"), _T("ModuleList"), PPLAP_EXE_FILE_NAME, strINIFullPath);
	VECTOR_TSTRING vt;
	StrUtils::SplitString(strModules, _T(","), vt);
	for (int i = 0; i < vt.size(); i++)
	{
		UINT uSectionCount = ::GetPrivateProfileInt(vt[i].c_str(), _T("SectionCount"), 0, strINIFullPath.c_str());
		ModuleInfo_Server mis;
		mis.Reset(vt[i], uSectionCount);
		m_serverINI.misVector.push_back(mis);
	}

	// 得到每个Module的详细信息
	VECTOR_MIS& vmis = m_serverINI.misVector;

	tstring strLocalVer = CUtils::GetFileVersionStr(PathAndDisk::Path_Combine(CAppDM::m_strCurrentFolder, PPLAP_EXE_FILE_NAME));

	// 遍历每个Module
	for (i = 0; i < vmis.size(); i++)
	{
		ModuleInfo_Server& mis = vmis[i];

		// 遍历这个Module的每一个Section
		// 如果匹配上某一个Section 则break
		for (int j = 0; j < mis.uSectionCount; j++)
		{
			tstring strSection = StrUtils::Format(_T("%s%u"), mis.strModuleName.c_str(), j);
			tstring strMin = RegAndProfile::GetINIStr(strSection, _T("min"), _T("0.0.0.0"),  strINIFullPath);
			tstring strMax = RegAndProfile::GetINIStr(strSection, _T("max"), _T("0.0.0.0"), strINIFullPath);

			// strMin <= strLocalVer < strMax
			if (!CUtils::CompareVersion(strLocalVer.c_str(), strMin.c_str()) && 
				CUtils::CompareVersion(strLocalVer.c_str(), strMax.c_str()))
			{
				// 匹配成功
				UINT uForce = ::GetPrivateProfileInt(strSection.c_str(), _T("Force"), 0, strINIFullPath.c_str());
				UINT uRatio = ::GetPrivateProfileInt(strSection.c_str(), _T("Ratio"), 0, strINIFullPath.c_str());
				tstring strVersion = RegAndProfile::GetINIStr(strSection, _T("Version"), _T("0.0.0.0"), strINIFullPath);
				tstring strVersionDesc = RegAndProfile::GetINIStr(strSection, _T("VersionDesc"), _T(""), strINIFullPath, 2048);
				tstring strVersionDesc_En = RegAndProfile::GetINIStr(strSection, _T("VersionDesc_En"), _T(""), strINIFullPath, 2048);
				tstring strVersionDescTitle = RegAndProfile::GetINIStr(strSection, _T("VersionDescTitle"), _T(""), strINIFullPath);
				tstring strVersionDescTitle_En = RegAndProfile::GetINIStr(strSection, _T("VersionDescTitle_En"), _T(""), strINIFullPath);
				UINT uFileCount = ::GetPrivateProfileInt(strSection.c_str(), _T("FileCount"), 0, strINIFullPath.c_str());
				for (int k = 0; k < uFileCount; ++k)
				{
					tstring strKey = StrUtils::Format(_T("File%u"), k);
					tstring strFileUrl = RegAndProfile::GetINIStr(strSection, strKey, _T(""), strINIFullPath, 1024);
					if (strFileUrl.size() > 0)
					{
						mis.fileUrlVector.push_back(strFileUrl);
					}
				}
				
				mis.uRatio = uRatio;
				mis.uForce = uForce;
				mis.strVersion = strVersion;
				mis.strVersionDesc = strVersionDesc;
				mis.strVersionDesc_En = strVersionDesc_En;
				mis.strVersionDescTitle = strVersionDescTitle;
				mis.strVersionDescTitle_En = strVersionDescTitle_En;
				mis.nMatchableSectionIndex = j;

				// 以平均概率从fileUrlVector中得到FileUrl
				int x = mis.fileUrlVector.size();
				if (x > 0)
				{
					x = CUtils::GetRandomNum(0, x - 1);
					mis.strFileUrl = mis.fileUrlVector[x];
				}
				// 根据uRatio计算一个概率 决定是否下载
				if (mis.uRatio > 100) mis.uRatio = 100;
				mis.bDownloadByRatio = CUtils::GetBoolByProbability(mis.uRatio);

				break;
			}
		}
	}

	// debug info
	/*
	for (i = 0; i < vmis.size(); i++)
	{
		g_log.Log(_T("serverINI.misVector[%u]: \nModuleName=%s, \nSectionCount=%u, \nRatio=%u, \nForce=%u, \nbDownloadByRatio=%d, \nVersion=%s, \nMatchableSectionIndex=%d, \nFileUrl=%s, \n"),
			i, 
			vmis[i].strModuleName.c_str(),
			vmis[i].uSectionCount,
			vmis[i].uRatio,
			vmis[i].uForce,
			vmis[i].bDownloadByRatio,
			vmis[i].strVersion.c_str(),
			vmis[i].nMatchableSectionIndex,
			vmis[i].strFileUrl.c_str());
	}
	g_log.Log(_T("\n"));
	*/
}

BOOL CUpdate::Update()
{
	VECTOR_MIS& vmis = m_serverINI.misVector;

	if (vmis.size() == 0) return FALSE;

	for (int i = 0; i < vmis.size(); i++)
	{
		ModuleInfo_Server& mis = vmis[i];

		if (mis.nMatchableSectionIndex == -1 || !mis.bDownloadByRatio) return FALSE;
		
		tstring strSrcPath = PathAndDisk::Path_Combine(CAppDM::m_strTmpFolder, PPLAP_EXE_FILE_NAME);
		tstring strObjPath = PathAndDisk::Path_Combine(CAppDM::m_strCurrentFolder, PPLAP_EXE_FILE_NAME);
		tstring strOldObjPath = strObjPath + _T(".(old)");
		
		//g_log.Log(_T("\nWill download: %s\n"), PPLAP_EXE_FILE_NAME);
		
		// 下载目标文件到临时目录
		if (NetUtils::DownloadFile_(mis.strFileUrl, m_serverINI.strProxy, strSrcPath))
		{
			// 删除当前目录下的old文件(如果存在)
			if (::PathFileExists(strOldObjPath.c_str()))
				::DeleteFile(strOldObjPath.c_str());
			
			// 将当前目录下的原文件改名
			MoveFileEx(strObjPath.c_str(), strOldObjPath.c_str(), MOVEFILE_WRITE_THROUGH);
			
			// 从临时目录拷贝文件到当前目录
			::CopyFile(strSrcPath.c_str(), strObjPath.c_str(), FALSE);

			return TRUE;
		}
		else
		{
			//g_log.Log(_T("Failed to download %s\n!"), PPLAP_EXE_FILE_NAME);
			return FALSE;
		}
	}

	return FALSE;
}