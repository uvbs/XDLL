#pragma once
#include "HopInfo.h"
#include "httpimpl.h"

class AsyncHttpDownloadListener : HttpDownloadListener
{
public:
	AsyncHttpDownloadListener()
	{
		m_httpDownload = NULL;
	}
	~AsyncHttpDownloadListener()
	{
		if (m_httpDownload) {
			m_httpDownload->Abort();
			m_httpDownload->Destroy();
			m_httpDownload = NULL;
		}
	}
	
	bool StartDownload(string& url, string& userAgent, string& referer)
	{
		if (m_httpDownload) {
			return false;
		}
		m_httpDownload = HttpDownload::Create(userAgent.c_str(), referer.c_str());
		if (m_httpDownload) {
			m_httpDownload->SetListener(this);
			return m_httpDownload->StartDownload(url.c_str(), BINDVERB_GET, NULL, 0);
		}
		return false;
	}
	virtual void OnDataReceive(unsigned char* data, size_t size);
	virtual void OnDateReceiveFailed(long errcode);
	
protected:
	HttpDownload* m_httpDownload;
};