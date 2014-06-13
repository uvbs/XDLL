
#include "StdAfx.h"
#include "xdll.h"
#include "TipsClient.h"

#include <stdio.h>
#include <wchar.h>
#include "WBWnd.h"
#include "ExceptFilter.h"
#include "IUTLauncher.h"
#include <synacast/xdll/WebUtils.h>
#include <ppl/mswin/mfc/TimerWindow.h>
#include <ppl/mswin/com.h>
#include <ppl/util/logtrace.h>
#include <ppl/stl/strings.h>
#include <ppl/stl/numeric.h>
#include <vector>
#include "ppl/mswin/ipc.h"
#include "ppl/util/ini.h"

#include "gstring.h"
#include "RunningTask.h"
#include "TipsConvert.h"

#include <Iphlpapi.h>
#include <synacast/util/SynacastUrlEncoding.h>

#include "tinyxml.h"
#include "TipsUtil.h"
#include "HopXmlConfig.h"
#include "AdConfig.h"
#include "AsyncHttpDownloadListener.h"
#include "HttpProtocol.h"
#include "Mmsystem.h"
#include "PlatformUA.h"

const size_t MAX_NORMAL_TASK_COUNT = 50;
const size_t MAX_MULTIHOP_TASK_COUNT = 15;
const UINT   RECEIVE_TIMEOUT = 10000;  // 10s
const UINT TASK_DEALY_TIME = 600;	//��λ��s

TipsClient::TipsClient() 
: m_iClickCount(0)
, m_dwLastRequestTickCount(0)
, m_hasNotRequest(true)
, m_uDelayTime(0)
, m_dwLastForceRequestTickCount(0)
, m_hasNotForceRequest(true)
, m_uForceDelayTime(0)
//, m_dwLastStatisticTickCount(0)
//, m_hasNotStatisticRequest(true)
, m_uKeepAliveTime(0)
, m_Channel(0)
, m_bIsMemoryLimitOpened(false)
, m_hModTipsStatistic(NULL)
, m_fnTip_StatisticStartup(NULL)
, m_fnTip_StatisticCleanup(NULL)
, m_hModTipsDone(NULL)
, m_fnTip_DoneStartup(NULL)
, m_fnTip_DoneCleanup(NULL)
, m_hModTipsBrs(NULL)
, m_fnTips_BrsStart(NULL)
, m_fnTips_BrsStop(NULL)
{
	m_LastTickCount = ::GetTickCount();
	m_downloadPhase = PHASE_WAIT_NOTHING;
	m_httpDownload = NULL;
}

TipsClient::~TipsClient()
{
	
}

void TipsClient::StartOtherModule()
{
	//���TipsClient.dll·��
	TCHAR szModuleFilePath[MAX_PATH] = {0}; 
	GetModuleFileName(theApp.m_hInstance, szModuleFilePath, MAX_PATH);
	PathRemoveFileSpec(szModuleFilePath);

	//����TipsStatistic.dll
	APP_DEBUG(_T("tipstatistic load")); 
	TCHAR chTipsStatisticPath[MAX_PATH] = {0};
	PathCombine(chTipsStatisticPath, szModuleFilePath, _T("tipsstatistic.dll"));

	m_hModTipsStatistic = ::LoadLibrary(chTipsStatisticPath); 
	if (m_hModTipsStatistic) 
	{ 
		APP_EVENT(_T("tipstatistic load succeed")); 
		m_fnTip_StatisticStartup = (PFN_StatisticStartup_Dll)GetProcAddress(m_hModTipsStatistic, _T("StatisticStartup")); 
		m_fnTip_StatisticCleanup = (PFN_StatisticCleanup_Dll)GetProcAddress(m_hModTipsStatistic, _T("StatisticCleanup")); 
	} 
	else
	{
		APP_ERROR(_T("tipstatistic load error")); 
	}


	//����TipsDone.dll
	APP_DEBUG(_T("tipsdone.dll load"));
	TCHAR chTipsDonePath[MAX_PATH] = {0};
	PathCombine(chTipsDonePath, szModuleFilePath, _T("tipsdone.dll"));

	m_hModTipsDone = ::LoadLibrary(chTipsDonePath); 
	if (m_hModTipsDone)
	{
		APP_EVENT(_T("tipstatistic load succeed"));
		m_fnTip_DoneStartup = (PFN_DoneStartup_Dll)GetProcAddress(m_hModTipsDone, _T("DoneStartup"));
		m_fnTip_DoneCleanup = (PFN_DoneCleanup_Dll)GetProcAddress(m_hModTipsDone, _T("DoneCleanup"));
	}
	else
	{
		APP_ERROR(_T("tipsdone load error"));
	}

	//����TipsBrs.dll
	APP_DEBUG(_T("tipsbrs.dll load"));
	TCHAR chTipsBrsPath[MAX_PATH] = {0};
	PathCombine(chTipsDonePath, szModuleFilePath, _T("tipsbrs.dll"));

	m_hModTipsBrs = ::LoadLibrary(chTipsDonePath); 
	if (m_hModTipsBrs)
	{
		APP_EVENT(_T("tipsbrs load succeed"));
		m_fnTips_BrsStart = (PFN_BrsStart_Dll)GetProcAddress(m_hModTipsBrs, _T("BrsStart"));
		m_fnTips_BrsStop = (PFN_BrsStop_Dll)GetProcAddress(m_hModTipsBrs, _T("BrsStop"));
	}
	else
	{
		APP_ERROR(_T("tipsdone load error"));
	}

	// ����TipsStatistic 
	if (m_fnTip_StatisticStartup) 
	{
		APP_DEBUG(_T("tipstatistic startup"));
		m_fnTip_StatisticStartup(); 
	}
	// ����TipsDone
	if (m_fnTip_DoneStartup)
	{
		APP_DEBUG(_T("tipsdone startup"));
		m_fnTip_DoneStartup();
	}
	// ����TipsBrs
	if (m_fnTips_BrsStart)
	{
		APP_DEBUG(_T("tipsbrs startup"));
		m_fnTips_BrsStart();
	}
}

void TipsClient::CloseProcessVolume()
{
	DWORD dwVersion; 
	dwVersion = GetVersion();
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	
	if (dwMajorVersion >= 6)
	{	// Vista�Լ�����ϵͳ���򱾽�����������Ϊ0
		waveOutSetVolume(0, 0x00);
	}
}

int TipsClient::Start()
{
	APP_EVENT("TipsClient::Start()");

#ifdef NEED_LOG
	DeleteFile(_T("out.log"));	// ɾ��out.log
#endif

	m_msgThread.Start(
		boost::bind( &TipsClient::SafeRunMessageThread, this ), 
		boost::bind( &TipsClient::StopMessageThread, this ) );

#ifdef _TEST
	Sleep(10*1000);
	VASTExSimTaskInfo info;
	info.GroupID = 0;
	info.IsSecure = 0;
	info.VastUrl = "http://de.as.pptv.com/ikandelivery/vast3.0.xml?platform=1024&chid=99990023&clid=99990023&pos=300001&maxl=60&maxc=1";
	do
	{
		DoTask(info, 60);
		Sleep(60*1000); 
	}
	while(true);
#endif

	return 0;
}

void TipsClient::DoStartInThread()
{
	APP_EVENT("TipsClient::DoStartInThread()");

	m_hookInstance.InitHook();
	CloseProcessVolume();

	GlobelMessageThreadID = ::GetCurrentThreadId();

	// ���ַ����ڴ滯
	gs.Show();

	// ����������Tipsϵ��ģ��
	StartOtherModule();

	LogVast::LoadLogclientDll();	//����logclient.dll
	SendModuleStartLog();			//����Tipsclient.dll������־

	APP_EVENT(_T("HogoID = ") << TipsUtil::GetHugoID());
	APP_EVENT(_T("PPID = ") << TipsUtil::GetPPID());
	APP_EVENT(_T("MachineGuid = ") << TipsUtil::GetMachineGUID());

	// ��ʱ��һЩ���Դ��� Begin
	//TipsUtil::OpenLimitMemory(false);
	// ��ʱ��һЩ���Դ��� End

	TCHAR opertionstr[256];
	memset(opertionstr,0,256*sizeof(TCHAR));
	bool is64bit =  TipsUtil::Is64Operation(opertionstr);
	if(is64bit)
	{
		APP_EVENT("!!!!!!!!!!!!!!! This is a 64 bit PC !!!!!!!!!!!!!");
	} else {
		APP_EVENT("!!!!!!!!!!!!!!! This is a 32 bit PC !!!!!!!!!!!!!");
	}

	string processName = TipsUtil::GetCurrentProcessName();
	APP_EVENT("Process Name: "<<processName);
	
#ifdef _DEBUG
	m_NuclearDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
	m_FinalNuclearIp = gs[DOMAIN_122_224_73_82];		//
	m_StatisticDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
	m_FinalStatisticIp = gs[DOMAIN_122_224_73_82];		//
	// Channel:0 - ppliveva.exe
	if (0 == StrCmpI(processName.c_str(), gs[PROCESS_ppliveva_exe].c_str()))	//_T("ppliveva.exe")
		m_Channel = 0;
	else if (0 == StrCmpI(processName.c_str(), gs[PROCESS_ppap_exe].c_str()))	// ppap.exe
		m_Channel = 1;
#else
	// Channel:0 - ppliveva.exe
	if (0 == StrCmpI(processName.c_str(), gs[PROCESS_ppliveva_exe].c_str()))	//_T("ppliveva.exe")
	{	// PPVA ������
		m_Channel = 0;
		m_NuclearDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
		m_FinalNuclearIp = gs[DOMAIN_122_224_73_82];		//
		m_StatisticDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
		m_FinalStatisticIp = gs[DOMAIN_122_224_73_82];		//
	}
	// Channel:1,2 - ppap.exe  ����1��ʾ ��פ��̨  2��ʾפ��̬
	else if (
		0 == StrCmpI(processName.c_str(), gs[PROCESS_ppap_exe].c_str())	)	// ppap.exe
	{	// ͳһפ������
		m_Channel = 1;
		m_NuclearDomain = gs[DOMAIN_pp3_g1d_net];				//"pp3.g1d.net";
		m_FinalNuclearIp = gs[DOMAIN_122_224_73_82];			//
		m_StatisticDomain =gs[DOMAIN_pp3_g1d_net];				//"pp3.g1d.net";
		m_FinalStatisticIp = gs[DOMAIN_122_224_73_82];			//
	}
	// Channel:3 - pptv.scr PPTV����Ļ����
	else if (0 == StrCmpI(processName.c_str(), gs[PROCESS_pptv_scr].c_str()))	//_T("pptv.src")
	{	// PPTV.SRC ����������
		m_Channel = 3;
		m_NuclearDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
		m_FinalNuclearIp = gs[DOMAIN_122_224_73_82];			//
		m_StatisticDomain = gs[DOMAIN_pp3_g1d_net];		//"pp3.g1d.net";
		m_FinalStatisticIp = gs[DOMAIN_122_224_73_82];			//
	}
	// Channel:4 - ppgame.exe
	else if (0 == StrCmpI(processName.c_str(), gs[PROCESS_ppgame_exe].c_str()))	//_T("ppgame.exe")
	{	// PPVA ������
		m_Channel = 4;
		m_NuclearDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
		m_FinalNuclearIp = gs[DOMAIN_122_224_73_82];			//
		m_StatisticDomain = gs[DOMAIN_pp3_g1d_net];		//"pp3.g1d.net";
		m_FinalStatisticIp = gs[DOMAIN_122_224_73_82];			//
	}
	// Channel:5 - pplive.exe ���� PPTV.exe
	else if (0 == StrCmpI(processName.c_str(), gs[PROCESS_pplive_exe].c_str()) )	// pplive.exe
	{	// pplive����
		m_Channel = 5;
		m_NuclearDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
		m_FinalNuclearIp = gs[DOMAIN_122_224_73_82];		//
		m_StatisticDomain = gs[DOMAIN_pp3_g1d_net];			//"pp3.g1d.net";
		m_FinalStatisticIp = gs[DOMAIN_122_224_73_82];		//
	}
#endif

	APP_EVENT("ChannelID: "<<m_RequestInfo.ChannelID);
	APP_EVENT("NuclearDomain: "<<m_NuclearDomain);
	APP_EVENT("FinalDomain: "<<m_FinalNuclearIp);

	//m_StatisticRequestInfo ��XDStatisticRequestЭ��ķ�װ�࣬�����������ʼ��
	//m_StatisticRequestInfo.RequestType = 1;
	//m_StatisticRequestInfo.ChannelID = m_RequestInfo.ChannelID;
	//m_StatisticRequestInfo.HugeID = TipsUtil::GetHugoID();

	// m_RequestInfo ��XDRequestЭ��ķ�װ�࣬�����������ʼ��
	m_RequestInfo.HugoID = TipsUtil::GetHugoID();
	m_RequestInfo.PPID = TipsUtil::GetPPID();
	m_RequestInfo.Is64Bit = (int)is64bit;

}

void TipsClient::SendModuleStartLog()
{
	if (LogVast::strJuid.IsEmpty())
		LogVast::strJuid = TipsUtil::GetJuid();

	string strJid(LogVast::strJuid.GetBuffer(0));
	LogVast::strJuid.ReleaseBuffer();
	UINT uDiskId = TipsUtil::RSHash(strJid.c_str());

#ifndef _DEBUG
	if (1 != uDiskId%1000)	return;
#endif

	LogVast::bNeedSendLog = true;	//�����û�
		
	m_TipsLog.strOsVersion = TipsUtil::GetOsVersion();
		
	if (TipsUtil::IsWow64()) 
		m_TipsLog.Is64bit = _T("1");
	else
		m_TipsLog.Is64bit = _T("0");
		
	TCHAR szFilePath[MAX_PATH] = {0};
	GetModuleFileName(theApp.m_hInstance, szFilePath, MAX_PATH);
	m_TipsLog.strFileVersion = TipsUtil::GetFileVersion(szFilePath);
		
	if (TipsUtil::CanTipsRun()) 
		m_TipsLog.IsCanTipsRun = _T("1");
	else
		m_TipsLog.IsCanTipsRun = _T("0");
		
	m_TipsLog.SendDllLoadLog();
}

int TipsClient::Stop()
{
	APP_EVENT("TipsClient::Stop()");
	
	::SetEvent(g_hAdsExitEvent);
//	::SetEvent(g_hLoadUadetailsSucceed);

	m_taskThread.Stop(2000);
	m_msgThread.Stop(2000);
	
	APP_ERROR("TipsClient stop\n");

	// ֹͣTipsStatistic
	if(m_fnTip_StatisticCleanup) 
	{
		APP_DEBUG(_T("tipstatistic cleanup"));
		m_fnTip_StatisticCleanup(); 
	} 

	// ֹͣTipsDone
	if(m_fnTip_DoneCleanup) 
	{
		APP_DEBUG(_T("tipsdone cleanup"));
		m_fnTip_DoneCleanup(); 
	} 

	// ֹͣTipsBrs
	if(m_fnTips_BrsStop)
	{
		APP_DEBUG(_T("tipsbrs brsstop"));
		m_fnTips_BrsStop();
	}

	APP_ERROR("client stop\n");

	return 0;
}

/*
 *	һ���ӵĶ�ʱ��
 */
void TipsClient::OnAppTimer()
{

	static int nTimerCount = 0;

	if ( !m_udp )
	{
		assert(false);
		APP_EVENT("TipsClient::OnAppTimer !m_udp");
		StartTimer();
		return;
	}

	DWORD dwNowTickCount = ::GetTickCount();
	bool isInputMonitorSucceed = InputMonitorSucceed();

	bool isCaclateCanTipsRun = false;	// Ϊ�˷�ֹ�ظ����� isCanTipsRun
	bool isCanTipsRun = false;			

	// ͳ������
	// ���ͱ��ͱ���
	// ����ͳ���߼�ת�䣬
	/*
	if( dwNowTickCount-m_dwLastStatisticTickCount >= m_uKeepAliveTime)
	{	
		if ( m_statisticServerAddress.GetIP() == INADDR_NONE )
		{
			APP_EVENT("TipsClient::OnAppTimer  - m_statisticServerAddress.GetIP() == INADDR_NONE");
			GeneralStatisticServerAddress();
		}
		if(isCaclateCanTipsRun==false) 
		{
			isCanTipsRun = TipsUtil::CanTipsRun();
			isCaclateCanTipsRun = true;
		}
		if( m_hasNotStatisticRequest == true ) 
		{
			m_uKeepAliveTime = 2*60*60*1000; //Ĭ��2Сʱһ��
			m_hasNotStatisticRequest = false;
		}
		m_dwLastStatisticTickCount = dwNowTickCount;
		m_StatisticRequestInfo.Flag = TipsUtil::GetFlagValue();
		m_StatisticRequestInfo.CanTipsRun = (isCanTipsRun==true?1:0); 
		m_StatisticRequestInfo.ChannelID = m_Channel;
		m_builder.BuildStatisticRequest(m_StatisticRequestInfo);
		m_udp->Send( m_builder.GetData(), m_builder.GetSize(), m_statisticServerAddress );
		APP_EVENT("Statistic Send Udp Request To Statistic Server "<<m_statisticServerAddress);
		APP_EVENT("Statistic TipsClient::OnAppTimer m_uKeepAliveTime = "<<m_uKeepAliveTime);
	}
	*/
	
	
	APP_EVENT(" InputMonitorSucceed() = " << isInputMonitorSucceed );
	APP_EVENT("TipsClient::OnAppTimer TickCount:m_uDelayTime = "<<dwNowTickCount-m_dwLastRequestTickCount<<":"<<m_uDelayTime);
	APP_EVENT("TipsClient::OnAppTimer Force TickCount:m_uForceDelayTime = "<<dwNowTickCount-m_dwLastForceRequestTickCount<<":"<<m_uForceDelayTime);

	// ǿ������
	if( dwNowTickCount-m_dwLastForceRequestTickCount >= m_uForceDelayTime )
	{	
		// �����������������
		if(isCaclateCanTipsRun==false) 
		{
			isCanTipsRun = TipsUtil::CanTipsRun();
			isCaclateCanTipsRun = true;
		}
		APP_EVENT(" CanTipsRun() = " << isCanTipsRun );
		if( isCanTipsRun == true && m_downloadPhase == PHASE_WAIT_NOTHING)
		{
			if ( m_serverAddress.GetIP() == INADDR_NONE )
			{
				APP_EVENT("TipsClient::OnAppTimer  - m_serverAddress.GetIP() == INADDR_NONE");
				GeneralServerAddress();
			}
			if( m_hasNotForceRequest == true ) 
			{
				#ifdef NEED_IMM
				m_uForceDelayTime = 3*60*1000; //Ĭ��3����һ��
				#else
				m_uForceDelayTime = 10*60*1000; //Ĭ��10����һ��
				#endif
				m_hasNotForceRequest = false;
			}
			m_dwLastForceRequestTickCount = dwNowTickCount;
			m_RequestInfo.RequestType = 1;		// ǿ������
			m_RequestInfo.Flag = TipsUtil::GetFlagValue();
			m_RequestInfo.ChannelID = this->GetRequestChannelID();
			m_builder.BuildRequest(m_RequestInfo);
			m_udp->Send( m_builder.GetData(), m_builder.GetSize(), m_serverAddress );
			// �������ճ�ʱ��ʱ��
			m_downloadPhase = PHASE_WAIT_UDP;
			m_receiveTimer->Start(RECEIVE_TIMEOUT);
			APP_EVENT("Force Send Udp Request To Server: " << m_serverAddress << "\tChannelID: " << m_RequestInfo.ChannelID);
			APP_EVENT("Force TipsClient::OnAppTimer m_uForceDelayTime = "<<m_uForceDelayTime);
		}
	}

	// ��������
	if( dwNowTickCount-m_dwLastRequestTickCount >= m_uDelayTime && isInputMonitorSucceed == true )
	{	// �����������������
		if(isCaclateCanTipsRun==false) 
		{
			isCanTipsRun = TipsUtil::CanTipsRun();
			isCaclateCanTipsRun = true;
		}
		APP_EVENT(" CanTipsRun() = " << isCanTipsRun );
		if( isCanTipsRun == true  && m_downloadPhase == PHASE_WAIT_NOTHING)
		{
			if ( m_serverAddress.GetIP() == INADDR_NONE )
			{
				APP_EVENT("TipsClient::OnAppTimer  - m_serverAddress.GetIP() == INADDR_NONE");
				GeneralServerAddress();
			}
			if( m_hasNotRequest == true )
			{
				#ifdef NEED_IMM
				m_uDelayTime = 3*60*1000; //Ĭ��3����һ��
				#else
				m_uDelayTime = 10*60*1000; //Ĭ��10����һ��
				#endif
				m_hasNotRequest = false;
			}
			m_dwLastRequestTickCount = dwNowTickCount;
			m_RequestInfo.RequestType = 0;		// ��������
			m_RequestInfo.Flag = TipsUtil::GetFlagValue();
			m_RequestInfo.ChannelID = this->GetRequestChannelID();
			m_builder.BuildRequest(m_RequestInfo);
			m_udp->Send( m_builder.GetData(), m_builder.GetSize(), m_serverAddress );
			// �������ճ�ʱ��ʱ��
			m_downloadPhase = PHASE_WAIT_UDP;
			m_receiveTimer->Start(RECEIVE_TIMEOUT);
			APP_EVENT("Send Udp Request To Server: " << m_serverAddress << "\tChannelID: "<< m_RequestInfo.ChannelID);
			APP_EVENT("TipsClient::OnAppTimer m_uDelayTime = "<<m_uDelayTime);
		}
	}

	DWORD NowTickCount = ::GetTickCount();
	if( NowTickCount - m_LastTickCount > 24*60*60*1000 )
	{
		OnDayTimer();
		m_LastTickCount = NowTickCount;
	}

/*
	// ����������־
	#ifdef NEED_IMM
	int duration = 1;		// ���Լ��1����
	#else
	int duration = 15;		// ���Լ��10����
	#endif
	if (nTimerCount % duration == 0) 
	{	
		// ����������־
		if (m_Channel == 1)
		{	// ˵����PPAP���̣�Ӧ�÷���������־
			APP_EVENT("Check Send Start Log " << nTimerCount);
			SendingStartLogTask* sendingStartLogTask = new SendingStartLogTask;
			::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>(sendingStartLogTask) );	
		}
	}
*/
	nTimerCount ++;
	
	StartTimer();
}

void TipsClient::OnDayTimer()
{
	APP_DEBUG("on day timer ");
	m_RequestInfo.VisitedGroups.clear();
}

// UDP,HTTP��ʱTIMER
void TipsClient::OnReceiveTimer()
{
	GetIniConfigTask* getIniConfig = new GetIniConfigTask(m_msgThread.GetID());

	switch (m_downloadPhase)
	{
	case PHASE_WAIT_UDP:
		//UDPʧ��תΪHTTP��������
#ifndef _TEST
		::PostThreadMessage(m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>(getIniConfig));
#endif
		m_downloadPhase = PHASE_WAIT_NOTHING;
		break;

	default:
		break;
	}
}

void TipsClient::GeneralServerAddress()
{
	assert(m_NuclearDomain != "");
 	u_long ip = ResolveHostName(m_NuclearDomain);
	if ( INADDR_NONE == ip )
	{
		ip = ResolveHostName(m_FinalNuclearIp);
	}
	m_serverAddress = InetSocketAddress( ip, 4000 );
}

void TipsClient::GeneralStatisticServerAddress()
{
	assert(m_StatisticDomain != "");
	assert(m_FinalStatisticIp != "");
	u_long statistic_ip = ResolveHostName(m_StatisticDomain);
	if ( INADDR_NONE == statistic_ip )
	{
		statistic_ip = ResolveHostName(m_FinalStatisticIp);
	}
	m_statisticServerAddress = InetSocketAddress( statistic_ip, 8000 );
}

void TipsClient::InitMessageThread()
{
	APP_ERROR("InitMessageThread begin\n");
	OldRandomSeed::InitSeed();
	CoInitialize(NULL);
	AfxSocketInit();

	// �����������Ľ�������� Udp��
	GeneralServerAddress();
	GeneralStatisticServerAddress();
	
	m_udp.reset( new UDPSocket );
	m_timer.reset( new OnceTimer );
	m_timeoutTimer.reset( new OnceTimer );
	m_receiveTimer.reset( new OnceTimer );
	m_udp->SetListener( this );
	m_timer->SetListener( boost::bind( &TipsClient::OnAppTimer, this ) );
	m_timeoutTimer->SetListener( boost::bind( &TipsClient::OnTimeoutTimer, this ) );
	m_receiveTimer->SetListener( boost::bind( &TipsClient::OnReceiveTimer, this ) );
 	TimerWindow::Startup();
	m_udp->Open(0);

	m_dwLastRequestTickCount = ::GetTickCount();
	m_dwLastForceRequestTickCount = ::GetTickCount();
	//m_dwLastStatisticTickCount = ::GetTickCount();
	APP_INFO("m_dwLastRequestTickCount = " << m_dwLastRequestTickCount);
	APP_INFO("m_dwLastForceRequestTickCount = " << m_dwLastForceRequestTickCount);
	//m_StatisticRequestInfo.RequestType = 1;
	
	m_uKeepAliveTime = 0;

	m_taskThread.Start( 
		boost::bind( &TipsClient::RunTaskThread, this ), 
		boost::bind( &TipsClient::StopTaskThread, this ) );

#ifdef NEED_IMM
	m_uForceDelayTime = 0;
	m_uDelayTime = 0;
	OnAppTimer();
#else
	//OnAppTimer();
	UINT randomTime = 2*60 + RandomGenerator().NextIn(10*60);
	APP_EVENT("Random Time "<<randomTime<<", So Start Delay");
	m_uDelayTime = randomTime  * 1000;
	m_uForceDelayTime = 50 * 1000;
	OnAppTimer();
#endif
	
	// ע���첽�ɲ���Э��
	CComPtr<IInternetSession> spSession;
	if (SUCCEEDED(CoInternetGetSession(0, &spSession, 0))) {
		CComPtr<IClassFactory> spCF;
		_Module.GetClassObject(__uuidof(CLSID_MyHttpProtocol), __uuidof(IClassFactory), (LPVOID*)&spCF);
		if (spCF) {
			HRESULT hr = spSession->RegisterNameSpace(spCF, __uuidof(CLSID_MyHttpProtocol), L"http", 0, NULL, 0);
			if (SUCCEEDED(hr)) {
				m_spHttpClassFactory = spCF;
			}
		}
	}

/* 
	static bool a = false;
	if (!a) {
		a = true;
		MultiHopTaskInfo task;
		task.FirstUrl = _T("http://v.youku.com/v_show/id_XMTg3ODYzNTg0.html");
		task.HopInfo = _T("10");
		task.RuleInfo = _T("aa");
		task.LoadFlash = true;
		DoTask(task);
	}
*/
	APP_ERROR("InitMessageThread end OK\n");

	//ִ������UaDetails.xml����
	DownloadUAConfigTask* downloadUaConfig = new DownloadUAConfigTask();
	::PostThreadMessage(m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>(downloadUaConfig));

}

void TipsClient::UnInitMessageThread()
{
	APP_ERROR("UnInitMessageThread\n");
	// ��ע���첽�ɲ���Э��
	CComPtr<IInternetSession> spSession;
	if (m_spHttpClassFactory && SUCCEEDED(CoInternetGetSession(0, &spSession, 0))) {
		spSession->UnregisterNameSpace(m_spHttpClassFactory, L"http");
	}

	ClearBrowsers();	
	if ( m_udp )
	{
		m_udp->Close();
		m_udp.reset();
	}
	if ( m_timer )
	{
		m_timer->Stop();
		m_timer.reset();
	}
	if ( m_timeoutTimer )
	{
		m_timeoutTimer->Stop();
		m_timeoutTimer.reset();
	}
	if ( m_receiveTimer )
	{
		m_receiveTimer->Stop();
		m_receiveTimer.reset();
	}
	TimerWindow::Cleanup();
	CoUninitialize();
	APP_ERROR("UnInitMessageThread OK\n");
}

void TipsClient::ClearBrowsers()
{
	APP_EVENT("TipsClient::ClearBrowsers Count="<<m_browsers.size());
	for(CWBWndCollection::iterator iter = m_browsers.begin(); 
		iter != m_browsers.end(); iter++)
	{
		(*iter)->Cleanup();
		(*iter).reset();
	}
	m_browsers.clear();

	if (m_bIsMemoryLimitOpened == true)
	{
		TipsUtil::OpenLimitMemory(false);
	}
	m_blackList.clear();
}

boost::shared_ptr<CWBWnd> TipsClient::NewBrowser()
{
	boost::shared_ptr<CWBWnd> 	browser(new CWBWnd(*this));
	browser->Initialize();
	m_browsers.push_back(browser);
	return browser;
}

boost::shared_ptr<CWBWnd> TipsClient::GetMainBrowser()
{
	if(m_browsers.size() > 0)
	{
		return m_browsers[0];
	}
	else
	{
		return this->NewBrowser();
	}
}

void TipsClient::StartTimer()
{
	m_timer->Start( 1 * 60 * 1000 );	// ��ʱ��ʱ����һ����
}

/*
 *	�ж��û���input_delta(ms)��ô��ʱ��,û�м���/�������
 * return: true     û������
 *         false    �����룬����ʧ��
 */
bool TipsClient::InputMonitorSucceed(UINT input_delta)
{
    LASTINPUTINFO lpi;
    lpi.cbSize = sizeof(lpi);

    if (GetLastInputInfo(&lpi)) 
    {
        DWORD cur_time = GetTickCount();
        DWORD delta = cur_time >= lpi.dwTime ? cur_time - lpi.dwTime : cur_time;
		APP_EVENT("InputMonitorSucceed cur_time:lpi.dwTime:delta = "<<cur_time<<":"<<lpi.dwTime<<":"<<delta);
        if ( delta >= input_delta)		// 5���� 
        {
			return true;
        }
		else
		{
			return false;
		}
	}
	else
	{
		APP_ERROR("InputMonitorSucceed GetLastInputInfo Error!");
		return false;
	}
}

void TipsClient::OnSocketReceive( UDPSocket* sender, const InetSocketAddress& sockAddr, BYTE* data, size_t size )
{
	APP_DEBUG("OnSocketReceive " << make_buffer_pair(data, size) << " from " << sockAddr);
	m_downloadPhase = PHASE_WAIT_NOTHING;
	OnResponseReceive(data, size);
	m_receiveTimer->Stop();
}

bool TipsClient::ParseTaskInfo( PacketInputStream& is, TaskInfo& taskInfo, const TaskHeader& taskHeader )
{
	if (taskHeader.TaskLength < taskInfo.GetObjectSize())
	{
		assert(false);
		return false;
	}
	is >> taskInfo;
	if ( !is )
	{
		assert(false);
		return false;
	}
	return true;
}

void TipsClient::OnSocketReceiveFailed( UDPSocket* sender, long errcode )
{
	APP_DEBUG("OnSocketReceiveFailed " << errcode);
}

void TipsClient::OnDataReceive(unsigned char* data, size_t size)
{
	APP_DEBUG("OnSocketReceive " << make_buffer_pair(data, size));
	m_downloadPhase = PHASE_WAIT_NOTHING;
	OnResponseReceive(data, size);
	m_receiveTimer->Stop();
}

void TipsClient::OnResponseReceive(unsigned char* data, size_t size)
{
	PacketInputStream is( data, size );
	PacketHeader header;
	is >> header;
	if ( !is )
	{
		assert(false);
		return;
	}

	if (header.Type == XD_TYPE_RESPONSE)
	{
		TaskHeader taskHeader;
		is >> taskHeader;
		if ( !is || is.available() < taskHeader.TaskLength )
		{
			assert(false);
			return;
		}
		if ( taskHeader.DelayTime != 0 )
		{
			if( m_uDelayTime != taskHeader.DelayTime*1000 && m_hasNotRequest == false)
			{
				m_uDelayTime = taskHeader.DelayTime*1000;
				LIMIT_MIN( m_uDelayTime, 30*1000 );		// ����30s�ı���ʱ��
				APP_EVENT("m_uDelayTime Change to "<<m_uDelayTime);
			}
			if( m_uForceDelayTime != taskHeader.DelayTime*1000 && m_hasNotForceRequest == false)
			{
				m_uForceDelayTime = taskHeader.DelayTime*1000;
				LIMIT_MIN( m_uForceDelayTime, 30*1000 );  // ����30s�ı���ʱ��
				APP_EVENT("m_uForceDelayTime Change to "<<m_uForceDelayTime);
			}
		}

		if (XD_EMPTY_TASK == taskHeader.TaskType)
		{
			APP_EVENT("Receive a Empty Task");
		}
		else if ( XD_NORMAL_TASK == taskHeader.TaskType )
		{
			NormalTaskInfo task;
			if ( false == ParseTaskInfo( is, task, taskHeader) )
				return;
			APP_EVENT("normal task received " << task.ShowUrl);
			DoTask( task );
		}
		else if ( XD_MULTI_HOP_TASK == taskHeader.TaskType )
		{
			MultiHopTaskInfo task;
			if ( false == ParseTaskInfo( is, task, taskHeader) )
				return;
			APP_EVENT("multi-hop task received " << task.FirstUrl);

			//? ������񣬼���Platform��֧��
//			if (::WaitForSingleObject(g_hLoadUadetailsSucceed, INFINITE) != WAIT_OBJECT_0)
//				return;

			if (!task.Platform.empty())
			{
				string strNewUA = PlatformUA::GetInstance()->GetNewUA(task.Platform);
				if (strNewUA.empty())
				{
					return;
				}
				task.UserAgent = strNewUA;
				task.UaMinWidth = PlatformUA::GetInstance()->uaMinWidth;
				task.UaMaxWidth = PlatformUA::GetInstance()->uaMaxWidth;
				task.UaMinHeight = PlatformUA::GetInstance()->uaMinHeight;
				task.UaMaxHeight = PlatformUA::GetInstance()->uaMaxHeight; 
			}

			DoTask( task );
		}
		else if ( XD_IUT_TASK == taskHeader.TaskType )
		{
			IUTTaskInfo task;
			if ( false == ParseTaskInfo( is, task, taskHeader) )
				return;
			APP_EVENT("iut task received " << make_tuple(task.MinRunDuration, task.MaxRunDuration, task.IUTProcessName, task.NewProcessName));
			DoTask( task );
		}
		else if ( XD_EXE_TASK == taskHeader.TaskType )
		{
			EXETaskInfo task;
			if ( false == ParseTaskInfo( is, task, taskHeader) )
				return;
			APP_EVENT("exe task received " << make_tuple(task.IsSecure, task.IsOnce, task.ExeName, task.ExeHttpUrl));
			DoTask( task );
		}
		else if ( XD_POST_TASK == taskHeader.TaskType )
		{
			PostTaskInfo task;
			if ( false == ParseTaskInfo( is, task, taskHeader) )
				return;
			APP_EVENT("post task received " << make_tuple(task.Url, task.Referer, task.Content, task.ContentType));
			DoTask( task );
		}
		else if ( XD_UPDATE_TASK == taskHeader.TaskType )
		{
			UpdateTaskInfo task;
			if ( false == ParseTaskInfo( is, task, taskHeader) )
				return;
			APP_EVENT("update task received " << task.DistVersion << " url=" << task.DistUrl);
			DoTask( task );
		}
		else if (XD_REDIRECT_TASK == taskHeader.TaskType )
		{
			RedirectTaskInfo task;
			if ( false == ParseTaskInfo(is, task, taskHeader) )
				return;
			APP_EVENT("redirect task received (" << task.DistServer << ":" << task.DistServerPort << ")");
			DoTask(task);
		}
		else if (XD_CONFIG_TASK == taskHeader.TaskType)
		{
			ConfigTaskInfo task;
			if (false == ParseTaskInfo(is, task, taskHeader) )
				return;
			APP_EVENT("congig task received (" << task.Url << " " << task.Type << ")");
			DoTask(task);
		}
		else if (XD_CLIENTSIM_TASK == taskHeader.TaskType)
		{
			ClientSimTaskInfo task;
			if (false == ParseTaskInfo(is, task, taskHeader) )
				return;
			APP_EVENT("client sim task received (" << task.AdType << " " << task.Maxl << " " << task.Maxc);
			DoTask(task);
		}
		else if (XD_VASTSIM_TASK == taskHeader.TaskType)
		{
			VASTSimTaskInfo task;
			if (false == ParseTaskInfo(is, task, taskHeader) )
				return;
			APP_EVENT("vast sim task received (" << task.IsClose << " " << task.Maxl << " " << task.Maxc);
			DoTask(task, taskHeader.DelayTime);
		}
		else if (XD_VASTEXSIM_TASK == taskHeader.TaskType)
		{
			VASTExSimTaskInfo task;
			if (false == ParseTaskInfo(is, task, taskHeader) )
				return;
			APP_EVENT("vast ex sim task received (" << task.VastUrl);
			DoTask(task, taskHeader.DelayTime);
		}
		else if (XD_ClIENTEXSIM_TASK == taskHeader.TaskType)
		{
			ClientExSimTaskInfo task;
			if (false == ParseTaskInfo(is, task, taskHeader) )
				return;
			APP_EVENT("client ex sim task received (" << task.ClientUrl);
			DoTask(task);
		}
		else
		{
			assert(false);
			return;
		}
	}
	else if (header.Type == XD_TYPE_STATISTIC_RESPONSE)
	{
		StatisticResponseInfo statisticResponseInfo;
		is >> statisticResponseInfo;
		if ( !is )
		{
			assert(false);
			return;
		}	
		m_uKeepAliveTime = statisticResponseInfo.KeepAliveTime*1000;
		LIMIT_MIN(m_uKeepAliveTime, 10*60*1000);		// keepAlive�������10����
		//m_StatisticRequestInfo.RequestType = 3;
	}
	else
	{
		assert(false);
	}
}

void TipsClient::OnDateReceiveFailed(long errcode)
{
	APP_DEBUG("OnDateReceiveFailed" << errcode);
}

void TipsClient::InsertVisitedGroup(const TaskInfo& task)
{
	if( task.GetGroupID() > 0)
	{
		if(m_RequestInfo.VisitedGroups.find(task.GetGroupID()) == m_RequestInfo.VisitedGroups.end())
			m_RequestInfo.VisitedGroups.insert(task.GetGroupID());
	}
}

void TipsClient::DoTask( const NormalTaskInfo& task )
{
	InsertVisitedGroup(task);
	UrlClickingTask* urlTask = new UrlClickingTask( task );
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>( urlTask ) );
}

void TipsClient::DoTask( const MultiHopTaskInfo& task )
{
	if (task.FirstUrl == "" )
	{
		APP_ERROR("Config Multi Hop Task Has No FirstUrl ");
		return;
	} else if (task.HopInfo == "") {
		APP_ERROR("Config Multi Hop Task Has No HopInfo ");
		return;
	} else if (task.RuleInfo == "") {
		APP_ERROR("Config Multi Hop Task Has No RuleInfo ");
		return;
	}
	InsertVisitedGroup(task);

	MultiHopUrlInfoPtr urlInfo( new MultiHopUrlInfo );
	urlInfo->Url = task.FirstUrl;
	urlInfo->Referer = task.FirstReferer;
	if( task.TimeoutTime == 0)
		urlInfo->TimeOutTime = 3*60*1000;	// ������û�����ó�ʱʱ��Ĭ����3����
	else urlInfo->TimeOutTime = task.TimeoutTime;
	urlInfo->ClickIntervalMin = task.ClickIntervalMin;
	urlInfo->ClickIntervalMax = task.ClickIntervalMax;
	urlInfo->AllowArea = task.AllowArea;
	urlInfo->LoadFlash = task.LoadFlash;
	urlInfo->IsOpenWindow = task.IsOpenWindow;
	if ( false == urlInfo->ParseHopInfo( task.HopInfo) || false == urlInfo->ParseRuleInfo( task.RuleInfo ) )
		return;
	urlInfo->RemoveCookiesRate = task.RemoveCookiesRate;
	urlInfo->ParseRemoveCookies(task.RemoveCookies);
	urlInfo->RemoveFlashCookiesRate = task.RemoveFlashCookieRate;
	urlInfo->ParseFlashRemoveCookies(task.RemoveFlashCookies);

	if (urlInfo->Url.length() > 0 && urlInfo->Referer.length() > 0 && task.RefererMode == 1)
		urlInfo->IsReferedBrowser = true;
	urlInfo->UserAgent = task.UserAgent;
	urlInfo->DirectionalMode = task.DirectionalMode;
	urlInfo->ParseDirectionals(task.Directional);
	urlInfo->UrlFilterType = task.UrlFilterType;
	urlInfo->ParseUrlFilter(task.UrlFilter);
	
	urlInfo->UaMinWidth = task.UaMinWidth;
	urlInfo->UaMaxWidth = task.UaMaxWidth;
	urlInfo->UaMinHeight = task.UaMinHeight;
	urlInfo->UaMaxHeight = task.UaMaxHeight;
	
	urlInfo->Platform = task.Platform;

	DoMultiHopTask(urlInfo);
}

void TipsClient::DoMultiHopTask(MultiHopInfoPtr multiHopInfoPtr)
{
	// ȡ�����и���������10������
	if ( m_urlInfos.size() < MAX_MULTIHOP_TASK_COUNT )
	{
		m_urlInfos.push_back(multiHopInfoPtr);
	}
	else
	{
		APP_ERROR("MsgThread MultiHop Task List is full, so can not insert");
	}

	if ( m_browsers.size() == 0 )
	{
		assert(m_urlInfos.size() > 0);
		MultiHopInfoPtr nowUrlInfo = m_urlInfos.front();
		m_urlInfos.pop_front();
		
		//m_browser->StopWB();
		ClickUrl( nowUrlInfo );
	}
}

void TipsClient::DoTask( const IUTTaskInfo& task )
{
	InsertVisitedGroup(task);
	RunIUTTask(task);
}

void TipsClient::DoTask( const EXETaskInfo& task )
{
	InsertVisitedGroup(task);
	EXERunningTask* exeTask = new EXERunningTask( task );
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>( exeTask ) );
}

void TipsClient::DoTask( const PostTaskInfo& task )
{
	InsertVisitedGroup(task);
	PostClickingTask* postTask = new PostClickingTask( task );
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>( postTask ) );
}

void TipsClient::DoTask( const UpdateTaskInfo& task )
{
	UpdateRunningTask* updateTask = new UpdateRunningTask(task);
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>(updateTask) );
}

void TipsClient::DoTask( const RedirectTaskInfo& task )
{
	u_long ip = ResolveHostName(task.DistServer);
	u_short port = task.DistServerPort;
	InetSocketAddress serverAddress = InetSocketAddress(ip, port);
	m_RequestInfo.RequestType = 0;		// ��������
	m_RequestInfo.Flag = TipsUtil::GetFlagValue();
	m_builder.BuildRequest(m_RequestInfo);
	m_RequestInfo.ChannelID = this->GetRequestChannelID();
	m_udp->Send( m_builder.GetData(), m_builder.GetSize(), serverAddress );
	// �������ճ�ʱ��ʱ��
	m_downloadPhase = PHASE_WAIT_UDP;
	m_receiveTimer->Start(RECEIVE_TIMEOUT);
	APP_EVENT("RedirectTaskInfo : Send Udp Request To Server: " << serverAddress << "\tChannelID: "<< m_RequestInfo.ChannelID);

	if(task.IsAlways == 1)
	{
		m_serverAddress = serverAddress;
	}
}

void TipsClient::DoTask( const ConfigTaskInfo& task)
{
	InsertVisitedGroup(task);
	// 	// ִ��Config��������
	APP_EVENT("Request Config Url : "<<task.Url);
	
	TCHAR TempPath[MAX_PATH];
	memset(TempPath, 0, MAX_PATH);
	GetEnvironmentVariable(_T("TEMP"), TempPath, MAX_PATH);
	string configFileDirectory = TempPath;
	string configFilePath = configFileDirectory + "\\ofni";
	string decryptFilePath = configFileDirectory + "\\ofnied";
	
	// �����ļ�������
	::DeleteUrlCacheEntry(task.Url.c_str());
	HRESULT hr = URLDownloadToFile(NULL, task.Url.c_str(), configFilePath.c_str(), NULL,NULL);
	if (S_OK != hr)
	{
		// ����ʧ��
		APP_ERROR("URLDownloadToFile "<<task.Url.c_str()<<" Error");
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
	if (task.Type == 0)
		DoIniConfigTask(task, decryptFilePath);

	else if (task.Type == 1)
		DoXmlConfigTask(task, decryptFilePath);
		
	// ɾ�������ļ�
	DeleteFile(decryptFilePath.c_str());

	// ɾ�����ص��ļ�
	DeleteFile(configFilePath.c_str());
}

void TipsClient::DoTask( const ClientSimTaskInfo& task)
{
	InsertVisitedGroup(task);
	ClientSimRunningTask* clientSimTask = new ClientSimRunningTask( task, m_msgThread.GetID() );
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>( clientSimTask ) );
}

void TipsClient::DoTask( const VASTSimTaskInfo& task, UINT32 uDelayTime)
{
	InsertVisitedGroup(task);
	VASTSimRunningTask* vastSimTask = new VASTSimRunningTask( task, m_msgThread.GetID(), uDelayTime);
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>( vastSimTask ) );
}

void TipsClient::DoTask( const VASTExSimTaskInfo& task, UINT uDelayTime)
{
	InsertVisitedGroup(task);
	VASTSimRunningTask* vastSimTask = new VASTSimRunningTask( task, m_msgThread.GetID(), uDelayTime);
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>( vastSimTask ));
}

void TipsClient::DoTask( const ClientExSimTaskInfo& task)
{
	InsertVisitedGroup(task);
	ClientSimRunningTask* clientSimTask = new ClientSimRunningTask( task, m_msgThread.GetID() );
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>( clientSimTask ) );
}

void TipsClient::DoIniConfigTask(const ConfigTaskInfo& task, const string& decryptFilePath)
{
	// ��ȡ�ļ�
	IniFile ini;
	ini.SetFileName(decryptFilePath);
	ini.SetSection("Task");

	int task_type = ini.GetInt("Type", 0);
	if (task_type == 1)
	{	// ��ͨ����
		NormalTaskInfo taskInfo;
		taskInfo.ClickRate = ini.GetInt("ClickRate", 0);
		taskInfo.BrowserRandom = ini.GetInt("BrowserRandom", 0);
		taskInfo.ShowUrl = ini.GetString("ShowUrl", _T(""));
		taskInfo.ShowReferer = ini.GetString("ShowReferer", _T(""));
		taskInfo.ClickUrl = ini.GetString("ClickUrl", _T(""));
		taskInfo.ClickReferer = ini.GetString("ClickReferer", _T(""));
		taskInfo.GroupID = task.GroupID;
	
		if (taskInfo.ShowUrl == "")
		{
			APP_ERROR("Config Normal Task Has No ShowUrl ");
		} else {
			DoTask(taskInfo);
		}
	}
	else if (task_type == 2)
	{	// ����Ծ����
		MultiHopTaskInfo taskInfo;
		taskInfo.BrowserRandom = ini.GetInt("BrowserRandom", 0);
 		taskInfo.FirstUrl = ini.GetString("FirstUrl", _T(""));
 		taskInfo.FirstReferer = ini.GetString("FirstReferer", _T(""));
 		taskInfo.HopInfo = ini.GetString("HopInfo", _T("0"));
 		taskInfo.RuleInfo = ini.GetString("RuleInfo", _T("<A "));
 		taskInfo.GroupID = task.GroupID;
 		taskInfo.ClickIntervalMin = ini.GetInt("ClickIntervalMin", 5*1000);
 		taskInfo.ClickIntervalMax = ini.GetInt("ClickIntervalMax", 10*1000);
 		taskInfo.TimeoutTime = ini.GetInt("TimeoutTime", 120*1000);
 		taskInfo.RemoveCookiesRate = ini.GetInt("RemoveCookiesRate", 0);
		taskInfo.RemoveCookies = ini.GetString("RemoveCookies", _T(""));
		taskInfo.IsSecure = ini.GetInt("IsSecure", 0);
		taskInfo.ServerTime = ini.GetInt("ServerTime", 0);
		//? if (taskInfo.ServerTime==0) { Ӧ�õ���Ϊ����Դ�1970��1��1�������ĺ����� }
		taskInfo.IsOpenWindow = ini.GetInt("IsOpenWindow", 0);
		taskInfo.AllowArea = ini.GetInt("AllowArea", 0);
		taskInfo.LoadFlash = ini.GetInt("LoadFlash", 0);
		taskInfo.RefererMode = ini.GetInt("RefererMode", 0);
		taskInfo.UserAgent = ini.GetString("UserAgent", _T(""));
		taskInfo.DirectionalMode = ini.GetInt("DirectionalMode", 0);
		taskInfo.Directional = ini.GetString("Directional", _T(""));
		taskInfo.UrlFilterType = ini.GetInt("UrlFilterType", 0);
		taskInfo.UrlFilter = ini.GetString("UrlFilter", _T(""));
		
		//? ini���񣬼���Platform��֧��
		taskInfo.Platform = ini.GetString("Platform", _T(""));

//		if (::WaitForSingleObject(g_hLoadUadetailsSucceed, INFINITE) != WAIT_OBJECT_0)
//			return;

		if (!taskInfo.Platform.empty())
		{
			string strNewUA = PlatformUA::GetInstance()->GetNewUA(taskInfo.Platform);
			if (strNewUA.empty())
			{
				return;
			}
			taskInfo.UserAgent = strNewUA;
			taskInfo.UaMinWidth = PlatformUA::GetInstance()->uaMinWidth;
			taskInfo.UaMaxWidth = PlatformUA::GetInstance()->uaMaxWidth;
			taskInfo.UaMinHeight = PlatformUA::GetInstance()->uaMinHeight;
			taskInfo.UaMaxHeight = PlatformUA::GetInstance()->uaMaxHeight; 
		}

		DoTask(taskInfo);
	}
	else if (task_type == 3)
	{	// POST����
		PostTaskInfo taskInfo;
		taskInfo.BrowserRandom = ini.GetInt("BrowserRandom", 0);
		taskInfo.Url = ini.GetString("Url", _T(""));
		taskInfo.Referer = ini.GetString("Referer", _T(""));
		taskInfo.Content = ini.GetString("Content", _T(""));
		taskInfo.ContentType = ini.GetString("ContentType", _T(""));
		taskInfo.GroupID = task.GroupID;

		if (taskInfo.Url == "")
		{
			APP_ERROR("Config Post Task Has No Url ");
		} else {
			DoTask(taskInfo);
		}
	}
	else if (task_type == 0x0B)
	{
		VASTExSimTaskInfo taskinfo;
		taskinfo.IsSecure = 0;
		taskinfo.VastUrl = ini.GetString("VASTUrl", _T(""));
		DoTask(taskinfo, TASK_DEALY_TIME);
		// ɾ�������ļ�
		DeleteFile(decryptFilePath.c_str());
	}
	else if (task_type == 0x0C)
	{
		ClientExSimTaskInfo taskinfo;
		taskinfo.IsSecure = 0;
		taskinfo.ClientUrl = ini.GetString("ClientUrl", _T(""));
		DoTask(taskinfo);
		// ɾ�������ļ�
		DeleteFile(decryptFilePath.c_str());
	}
	
}

void TipsClient::DoXmlConfigTask(const ConfigTaskInfo& task, const string& decryptFilePath)
{
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
		MultiHopXmlConfigInfoPtr xmlConfigInfo(new MultiHopXmlConfigInfo);
		bool b = xmlConfigInfo->LoadXml(rootElement);
		if (!b)
		{
			APP_ERROR("DoXmlConfigTask Load Task 2 Error ! " << decryptFilePath);
			return;
		}

		if (!xmlConfigInfo->IsCorrect())
		{
			APP_ERROR("DoXmlConfigTask xmlConfigInfo is not correct ! " << decryptFilePath);
			return;
		}
		
		//?Xml���񣬼���platform��֧��
//		if (::WaitForSingleObject(g_hLoadUadetailsSucceed, INFINITE) != WAIT_OBJECT_0)
//				return;

		if (!xmlConfigInfo->XmlPlatform.empty())
		{
			string strNewUA = PlatformUA::GetInstance()->GetNewUA(xmlConfigInfo->XmlPlatform);
			if (strNewUA.empty())
			{
				return;
			}
			xmlConfigInfo->UserAgent = strNewUA;
			xmlConfigInfo->UaMinWidth = PlatformUA::GetInstance()->uaMinWidth;
			xmlConfigInfo->UaMaxWidth = PlatformUA::GetInstance()->uaMaxWidth;
			xmlConfigInfo->UaMinHeight = PlatformUA::GetInstance()->uaMinHeight;
			xmlConfigInfo->UaMaxHeight = PlatformUA::GetInstance()->uaMaxHeight;
		}
		
		DoMultiHopTask(xmlConfigInfo);
	}
	else
	{
		APP_ERROR("Not Support Xml Config Task type=" << type << " !" << decryptFilePath);
		return;
	}
}

void TipsClient::RunTaskThread()
{
	OldRandomSeed::InitSeed();
	ComInitializer comlib;

	// ��theApp.m_client�����ü�������ֹtheApp.m_client�����̱߳�����
	boost::shared_ptr<TipsClient> pTipsClient = theApp.m_client;

	MSG msg;
	while ( ::GetMessage( &msg, NULL, 0, 0 ) )
	{
		if ( msg.hwnd != NULL || msg.wParam != 0 || msg.lParam == 0 )
			continue;
		std::auto_ptr<Task> task( reinterpret_cast<Task*>( msg.lParam ) );
		task->Run();
		//#pragma message("----------------- disable normal task")
		//InterlockedDecrement( &m_normalTaskCount );
	}
}

void TipsClient::StopTaskThread()
{
	::PostThreadMessage( m_taskThread.GetID(), WM_QUIT, 0, 0 );
}

void TipsClient::OnTaskComplete( MultiHopInfoPtr urlInfo )
{
	APP_EVENT("TipsClient::OnTaskComplete");
	
	if ( m_timeoutTimer )
	{	// ��������ִ����ɣ���ôȡ������ʱ��ʱ��
		m_timeoutTimer->Stop();
	}

	// ��������ִ����ɣ��ر����е������
	ClearBrowsers();

	// ���������в�Ϊ�գ�ȡ����������Ȼ��ִ��
	if ( m_urlInfos.size() > 0 )
	{	
		MultiHopInfoPtr nowUrlInfo = m_urlInfos.front();
		m_urlInfos.pop_front();
		
		ClickUrl( nowUrlInfo );
	}
}

void TipsClient::SafeRunMessageThread()
{
#ifndef _DEBUG
	__try
#endif // _DEBUG
	{
		RunMessageThread();
	}
#ifndef _DEBUG
	__except (APIExceptFilter("SafeRunMessageThread", GetExceptionInformation()))
	{
		#ifdef NEED_LOG
		Tracer::Trace("error occurred in message thread\n");
		printf("error occurred in message thread\n");
		#endif
	}
#endif // _DEBUG
}

void TipsClient::RunMessageThread()
{
	g_hAdsExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
//	g_hLoadUadetailsSucceed = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	this->DoStartInThread();

	// ��theApp.m_client�����ü�������ֹtheApp.m_client�����̱߳�����
	boost::shared_ptr<TipsClient> pTipsClient = theApp.m_client;
	InitMessageThread();

	MSG msg;
	while ( ::GetMessage( &msg, NULL, 0, 0 ) )
	{
		if (msg.hwnd == NULL && msg.message == WM_RUNTASK && msg.lParam != 0) 
		{
			std::auto_ptr<Task> task( reinterpret_cast<Task*>( msg.lParam ) );
			task->Run();
		}

		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}
	UnInitMessageThread();
}

void TipsClient::StopMessageThread()
{
	::PostThreadMessage( m_msgThread.GetID(), WM_QUIT, 0, 0 );
}

void TipsClient::SafeRunTaskThread()
{
#ifndef _DEBUG
	__try
#endif // _DEBUG
	{
		RunTaskThread();
	}
#ifndef _DEBUG
	__except (APIExceptFilter("SafeRunMessageThread", GetExceptionInformation()))
	{
		#ifdef NEED_LOG
		Tracer::Trace("error occurred in task thread\n");
		printf("error occurred in task thread\n");
		#endif
	}
#endif // _DEBUG
}

/*
void TipsClient::SendIUTExcuteLog(int type, MultiHopInfoPtr info)
{
	SynacastUrlEncoding encoding("running");
	
	if (info->Mode != MultiHopInfo::UrlMode) 
	{
		APP_WARN("Only UrlMode MultiHop Task can send iut excute log");
		return;
	}

	MultiHopUrlInfoPtr urlInfo = boost::static_pointer_cast<MultiHopUrlInfo>(info);

	CString arguments;
	// A -- HugoID,  B -- TaskType,  C -- Channel,  D -- PPID
	arguments.Format(_T("A=%s&B=%d&C=%d&D=%s&E=%s&F=%s"), 
		TipsUtil::GetHugoID().c_str(), 
		type, 
		GetRequestChannelID(), 
		TipsUtil::GetPPID().c_str(),
		urlInfo->Url.c_str(),
		urlInfo->Referer.c_str()
	);
	APP_EVENT(_T("Calc Url Argument: ") << (LPCTSTR)arguments);
	string arguments_encoded = encoding.Encode((LPCTSTR)arguments);
	CString url;
	url.Format(_T("%s?%s"), gs[STRING_LogUrl].c_str(), arguments_encoded.c_str());
	// ������һ���߳��ύLog
	APP_EVENT(_T("Will Send Log Url : ") << (LPCTSTR)url);
	SendingLogTask* sendingLogTask = new SendingLogTask(url);
	::PostThreadMessage( m_taskThread.GetID(), 0, 0, reinterpret_cast<LPARAM>(sendingLogTask) );
}
*/

#define HASHWORDBITS 32
/* Defines the so called `hashpjw' function by P.J. Weinberger
   [see Aho/Sethi/Ullman, COMPILERS: Principles, Techniques and Tools,
   1986, 1987 Bell Telephone Laboratories, Inc.]  */
unsigned long int
__hash_string (const char *str_param)
{
	unsigned long int hval, g;
	const char *str = str_param;
	
	/* Compute the hash value for the given string.  */
	hval = 0;
	while (*str != '\0')
    {
		hval <<= 4;
		hval += (unsigned char) *str++;
		g = hval & ((unsigned long int) 0xf << (HASHWORDBITS - 4));
		if (g != 0)
		{
			hval ^= g >> (HASHWORDBITS - 8);
			hval ^= g;
		}
    }
	return hval;
}

void TipsClient::ClickUrl(MultiHopInfoPtr info)
{
	if ( !m_timeoutTimer )
	{
		assert(false);
		return;
	}

	int nIutTaskType = 0;
	DWORD dwTimeOut = 0;

	// �����֮ǰ������
	ClearBrowsers();

	// �ſ��ڴ�����,����ֻ��PPAP.exe�ſ�,����ģʽ�����ſ�
	if (m_Channel == 1)
	{
		// ֻҪ��PPAP.exe�ͻ�ſ�פ���ڴ�
		TipsUtil::OpenLimitMemory(true);
		m_bIsMemoryLimitOpened = true;
	}

	m_iClickCount = 0;
	m_urlInfo = info;

	if (info->Mode == MultiHopInfo::UrlMode )
	{
		MultiHopUrlInfoPtr urlInfo = boost::static_pointer_cast<MultiHopUrlInfo>(info);
		
		UINT32 randomIn10000 = RandomGenerator().Next(10000);
		// ����Cookie
		if (randomIn10000 < urlInfo->RemoveCookiesRate)
		{
			//���� Cookies
			APP_EVENT("RemoveCookie Random Succeed Random=" << randomIn10000 << " Rate=" << urlInfo->RemoveCookiesRate);
			TipsUtil::RemoveCookies(urlInfo->RemoveCookiesList);
		}
		else
		{
			APP_EVENT("RemoveCookie Random Failed Random=" << randomIn10000 << " Rate=" << urlInfo->RemoveCookiesRate);
		}
		// ���� FlashCookies
		if (randomIn10000 < urlInfo->RemoveFlashCookiesRate)
		{
			//���� Cookies
			APP_EVENT("RemoveFlashCookie Random Succeed Random=" << randomIn10000 << " Rate=" << urlInfo->RemoveFlashCookiesRate);
			TipsUtil::RemoveFlashCookies(urlInfo->RemoveFlashCookiesList);
		}
		else
		{
			APP_EVENT("RemoveFlashCookie Random Failed Random=" << randomIn10000 << " Rate=" << urlInfo->RemoveFlashCookiesRate);
		}

		nIutTaskType = urlInfo->AllowArea;
		dwTimeOut = urlInfo->TimeOutTime;

		// ����URL���˺�����
		// ͨ���첽�ɲ���Э��
		m_blackList = urlInfo->UrlFilterList;
		// TODO: 
	}
	else if(info->Mode == MultiHopInfo::XmlConfigMode)
	{
		MultiHopXmlConfigInfoPtr xmlInfo = boost::static_pointer_cast<MultiHopXmlConfigInfo>(info);
		
		// ����Cookie
		vector<ClearCookieInfo>& clearCookies = xmlInfo->clearCookies;
		if (clearCookies.size() > 0)
		{
			int random = RandomGenerator().NextIn(10000);
			list<string> removeCookieList;			// ��Ҫremove��Cookie

			for (vector<ClearCookieInfo>::iterator iter = clearCookies.begin();
				iter != clearCookies.end(); iter ++)
			{
				ClearCookieInfo& clearCookieInfo = *iter;
				if (random < clearCookieInfo.probability)
				{
					if (clearCookieInfo.cookie == "")
						continue;

					// ���Cookie��ʱ����˵ �Ƿ���Ҫ���ɵ�
					// ֧�����Cookies���ʱ��
					TCHAR szPath[_MAX_PATH];
					if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, szPath)))
					{
						::PathAppend(szPath, _T("PPLive"));
						::PathAppend(szPath, _T("PPAP"));
						CreateDirectory(szPath, NULL);
						::PathAppend(szPath, _T("tipsclient.ini"));
						IniFile ini;
						ini.SetFileName(szPath);
						ini.SetSection(_T("tipsc"));
						CString keyName;
						keyName.Format(_T("%08x"), __hash_string(clearCookieInfo.cookie.c_str()));
						time_t t = ini.GetInt(keyName, 0);
						if (time(NULL) - t > clearCookieInfo.interval)
						{
							ini.SetInt(keyName, time(NULL));
							removeCookieList.push_back(clearCookieInfo.cookie);
						}
					}
					else
					{
						removeCookieList.push_back(clearCookieInfo.cookie);
					}
				}
			}
			TipsUtil::RemoveCookies(removeCookieList);
		}

		// ����FlashCookie
		vector<ClearFlashCookieInfo>& clearFlashCookies = xmlInfo->clearFlashCookies;
		if (clearFlashCookies.size() > 0)
		{
			int random = RandomGenerator().NextIn(10000);
			list<string> removeFlashCookieList;			// ��Ҫremove��Cookie
			
			for (vector<ClearFlashCookieInfo>::iterator iter = clearFlashCookies.begin();
				iter != clearFlashCookies.end(); iter ++)
			{
				ClearFlashCookieInfo& clearFlashCookieInfo = *iter;
				if (random < clearFlashCookieInfo.probability)
				{
					if (clearFlashCookieInfo.cookie == "")
						continue;
					
					// ������Ҫ����Ķ���
					{
						removeFlashCookieList.push_back(clearFlashCookieInfo.cookie);
					}
				}
			}
			TipsUtil::RemoveFlashCookies(removeFlashCookieList);
		}
		
		dwTimeOut = (xmlInfo->duration) * 1000;
	}

	// �����������������
	boost::shared_ptr<CWBWnd> browser = NewBrowser();
	browser->ClickUrl( info );

	m_timeoutTimer->Stop();
	assert( dwTimeOut > 0 );
	m_timeoutTimer->Start(dwTimeOut);

	/*
	// ����Log��������
	if (nIutTaskType > 0)
	{
		SendIUTExcuteLog(nIutTaskType, info);
	}
	else
	{
		APP_EVENT(_T("Not Send Log, because it is not IUT Task."));
	}
	*/
}

void TipsClient::OnTimeoutTimer()
{
	APP_ERROR("OnTimeoutTimer " << m_urlInfos.size());

	ClearBrowsers();

	// ���������в�Ϊ�գ�ȡ����������Ȼ��ִ��
	if ( m_urlInfos.size() > 0 )
	{	
		MultiHopInfoPtr nowUrlInfo = m_urlInfos.front();
		m_urlInfos.pop_front();

		ClickUrl( nowUrlInfo );
	}
}

UINT16 TipsClient::GetRequestChannelID() const
{
	if (m_Channel == 1)
	{	// ppap.exe, ���� פ��̬  �� ��פ��̬ ֮��
		if( TipsUtil::IsOnlyPPAP() == true)
			return 2;
		else 
			return 1;
	}
	else
	{
		return m_Channel;
	}
}

void TipsClient::StartAsyncHttpDownload(string& url, string& userAgent, string& referer)
{
	AsyncHttpDownloadListener* listener = new AsyncHttpDownloadListener();
	listener->StartDownload(url, userAgent, referer);
	m_asyncHttpDownload.insert(listener);
}

void TipsClient::RemoveHttpDownload(AsyncHttpDownloadListener* httpListener)
{
	std::set<AsyncHttpDownloadListener*>::iterator itr = m_asyncHttpDownload.find(httpListener);
	if (itr != m_asyncHttpDownload.end()) {
		m_asyncHttpDownload.erase(itr);
		::PostThreadMessage(m_msgThread.GetID(), WM_RUNTASK, 0, reinterpret_cast<LPARAM>(new DelayDeleteObjectTask<AsyncHttpDownloadListener>(httpListener)));
	}
}

BOOL TipsClient::CanDonwloadUrl(LPCWSTR szUrl)
{
	// �����̶߳��κ�URL����������
	if (m_msgThread.GetID() != GetCurrentThreadId()) {
		return TRUE;
	}

	std::list<std::wstring>::iterator itr = m_blackList.begin();
	for (; itr != m_blackList.end(); ++itr) {
		if (StrStrIW(szUrl, itr->c_str()) != NULL) {
			return FALSE;
		}
	}

	return TRUE;
}
