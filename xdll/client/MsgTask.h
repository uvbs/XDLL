#pragma once
#include <synacast/xdll/PacketBuilder.h>
#include "Task.h"
#include "HopInfo.h"

//////////////////////////////////////////////////////////////////////////
// MSG线程,异步HTTP下载任务
// 用于HTTP统计
class AsyncHttpTask : public Task
{
public:
	AsyncHttpTask(string url, string userAgent, string referer)
		:m_url(url), m_userAgent(userAgent), m_referer(referer)
	{
	}
	virtual void Run();
	
protected:
	std::string  m_url;
	std::string  m_userAgent;
	std::string  m_referer;
};

//////////////////////////////////////////////////////////////////////////
// MSG线程任务,
class MultiHopTask : public Task
{
public:
	MultiHopTask(MultiHopTaskInfo& taskInfo) : m_taskInfo(taskInfo)
	{
	}
	
	virtual void Run();
	
protected:
	MultiHopTaskInfo m_taskInfo;
};

//////////////////////////////////////////////////////////////////////////
// MSG线程任务,
class XmlConfigMultiHopTask : public Task
{
public:
	XmlConfigMultiHopTask(MultiHopXmlConfigInfoPtr& xmlConfigInfoPtr) : m_xmlConfigInfoPtr(xmlConfigInfoPtr)
	{
	}
	
	virtual void Run();
	
protected:
	MultiHopXmlConfigInfoPtr m_xmlConfigInfoPtr;
};

//////////////////////////////////////////////////////////////////////////
// MSG线程任务,
class IniConfigTask : public Task
{
public:
	IniConfigTask(string url):m_decryptFilePath(url)
	{
	}

	virtual void Run();

protected:
	string m_decryptFilePath;
};