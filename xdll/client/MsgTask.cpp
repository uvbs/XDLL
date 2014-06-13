#include "StdAfx.h"
#include "TipsClient.h"
#include "MsgTask.h"

void AsyncHttpTask::Run()
{
	TipsClient* client = AfxGetTipsClient();
	if (client) {
		client->StartAsyncHttpDownload(m_url, m_userAgent, m_referer);
	}
}

void MultiHopTask::Run()
{
	TipsClient* client = AfxGetTipsClient();
	if (client) {
		client->DoTask(m_taskInfo);
	}
}

void XmlConfigMultiHopTask::Run()
{
	TipsClient* client = AfxGetTipsClient();
	if (client) {
		//To-do
		client->DoMultiHopTask(m_xmlConfigInfoPtr);
	}
}

void IniConfigTask::Run()
{
	TipsClient* client = AfxGetTipsClient();
	if (client)
	{
		ConfigTaskInfo noInfo;
		client->DoIniConfigTask(noInfo, m_decryptFilePath);
	}
};