#include "StdAfx.h"
#include "AdPolicy.h"
#include <ppl/util/log.h>
#include "tinyxml.h"
#include "TipsUtil.h"

const char* MakeToString(const char* ch);

BOOL AdPolicy::UpdatePolicyXml(LPCTSTR lpXmlUrl, LPCTSTR lpXmlPath)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(lpXmlPath, &wfd);
	BOOL bDownloadPolicyXml = FALSE;
	
	if (INVALID_HANDLE_VALUE == hFind)
	{
		bDownloadPolicyXml = TRUE;
	}
	else
	{
		time_t last_write_time = 0;
		TipsUtil::FileTimeToTime_t(wfd.ftLastWriteTime, &last_write_time);
		time_t current_time = time(0);
		
		DWORD dwExpiredTime = 86400;
#ifdef _DEBUG
		dwExpiredTime = 1;
#endif
		if (current_time - last_write_time > dwExpiredTime)	//单位：s，过期时间为1天(86400s)
			bDownloadPolicyXml = TRUE;
	}
	
	//如果Policy.xml不存在或过期，则重新下载
	if (bDownloadPolicyXml)
	{
		::DeleteUrlCacheEntry(lpXmlUrl);
		if(S_OK != ::URLDownloadToFile(NULL, lpXmlUrl, lpXmlPath, NULL, NULL) )
		{
			APP_EVENT("Download Policy.xml File Failed.");
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL AdPolicy::LoadPolicyXml(LPCTSTR lpXmlPath, CString& strParam)
{
	TiXmlDocument* PolicyDoucment = new TiXmlDocument();
	
	char szPolicyXmlPath[MAX_PATH] = {0};
	TipsUtil::TCHARtoCHAR(lpXmlPath, szPolicyXmlPath);
	
	if(!PolicyDoucment->LoadFile(szPolicyXmlPath))
	{
		APP_ERROR("Load Policy.xml Failed.");
		return FALSE;
	}
	
	TiXmlElement* rootElement = PolicyDoucment->RootElement();
	if (!rootElement)
		return FALSE;
	
	TiXmlElement* linearrulesElement = rootElement->FirstChildElement("LinearRules");
	if (linearrulesElement)
	{
		TiXmlElement* onstartElement = linearrulesElement->FirstChildElement("OnStart");
		if (onstartElement)
		{
			TiXmlElement* thenElement = onstartElement->FirstChildElement("Then");
			if (thenElement)
			{
				TiXmlElement* adElement = thenElement->FirstChildElement("Ad");
				if (adElement)
				{
					strParam = MakeToString(adElement->GetText());	//读取policy.xml中的参数
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}