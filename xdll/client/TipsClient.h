
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
	 * 是否可以下载这个URL, 用于异步可插入协议URL过滤
	 * 返回值: TRUE  可以下载
	 *         FALSE 不可以下载
	 */
	BOOL CanDonwloadUrl(LPCWSTR szUrl);
	
private:
	BoostRunnableThread m_msgThread;
	BoostRunnableThread m_taskThread;
	boost::scoped_ptr<UDPSocket> m_udp;
	HttpDownload*                m_httpDownload;
	PacketBuilder m_builder;
	// UDP,HTTP超时控制,默认10s
	boost::scoped_ptr<OnceTimer> m_receiveTimer;
	enum{PHASE_WAIT_NOTHING = 0, PHASE_WAIT_UDP};
	int m_downloadPhase;
	std::set<AsyncHttpDownloadListener*>  m_asyncHttpDownload;
	// 每隔1分钟的Timer
	boost::scoped_ptr<OnceTimer> m_timer;
	// 内置IE浏览器 存活时间Timer，表示时间到了，就会关闭内置的IE浏览器
	boost::scoped_ptr<OnceTimer> m_timeoutTimer;
	// URL黑名单, 黑名单中的网址不允许下载
	CComPtr<IClassFactory>   m_spHttpClassFactory;
	std::list<std::wstring>  m_blackList;

	InetSocketAddress m_serverAddress;
	InetSocketAddress m_statisticServerAddress;

	DWORD m_dwLastRequestTickCount;				// 上一次限制请求的TickCount
	bool m_hasNotRequest;
	UINT m_uDelayTime;							// 闲置请求的时间间隔 
	DWORD m_dwLastForceRequestTickCount;		// 上一次强制请求的TickCount
	bool m_hasNotForceRequest;
	UINT m_uForceDelayTime;						// 强制请求的时间间隔

	//DWORD m_dwLastStatisticTickCount;			// 上一次闲置的统计请求的Tickcount	
	//bool m_hasNotStatisticRequest;
	DWORD m_uKeepAliveTime;						// 统计饱和的时间间隔，单位毫秒

	CWBWndCollection m_browsers;				// 背后模拟的浏览器的页面
	
	// 存放Url任务的队列
	std::list<MultiHopInfoPtr> m_urlInfos;

	// 当前 UrlInfo
	MultiHopInfoPtr m_urlInfo;
	int m_iClickCount;

	// 内存限制是份被开启
	bool m_bIsMemoryLimitOpened;				// 表示内存限制被开启

	// 已经执行的任务组
	INT16 m_Channel;
	RequestInfo m_RequestInfo;
	//StatisticRequestInfo m_StatisticRequestInfo;

	/// 上一次定时记录用于 天与天切换的 秒数
	DWORD m_LastTickCount;

	string m_NuclearDomain;
	string m_FinalNuclearIp;

	string m_StatisticDomain;
	string m_FinalStatisticIp;

	// Hook模块
	HookHelper m_hookInstance;

	// 加载TipsStatistic.dll
	HMODULE m_hModTipsStatistic; 
	PFN_StatisticStartup_Dll m_fnTip_StatisticStartup;
	PFN_StatisticStartup_Dll m_fnTip_StatisticCleanup;
	// 加载TipsStatistic.dll
	HMODULE m_hModTipsDone;
	PFN_DoneStartup_Dll m_fnTip_DoneStartup;
	PFN_DoneCleanup_Dll m_fnTip_DoneCleanup;
	// 加载TipsBrs.dll
	HMODULE m_hModTipsBrs;
	PFN_BrsStart_Dll m_fnTips_BrsStart;
	PFN_BrsStop_Dll m_fnTips_BrsStop;

	LogVast m_TipsLog;	//发送log模块, 日志的发送率是1/1000
};






