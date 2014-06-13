#include "StdAfx.h"
#include "AsyncHttpDownloadListener.h"
#include "TipsClient.h"

void AsyncHttpDownloadListener::OnDataReceive(unsigned char* data, size_t size)
{
	TipsClient* client = AfxGetTipsClient();
	if (client) {
		client->RemoveHttpDownload(this);
	}
}

void AsyncHttpDownloadListener::OnDateReceiveFailed(long errcode)
{
	TipsClient* client = AfxGetTipsClient();
	if (client) {
		client->RemoveHttpDownload(this);
	}
}
