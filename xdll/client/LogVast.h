#pragma once

#include "logclient.h"
#include "logclient-source.h"
#include "logclient-error.h"

#define FILTER_SUB_URL "jp.as.pptv.com"

typedef INT  (__stdcall *SendLogAsyncIgnoreReply)(const char* pLogSource, const LogClient_LogItem* pMetaItems, const LogClient_LogItem* pItems);

class LogVast
{
public:
	static bool bNeedSendLog;	//日志的发送率为1/1000
	static HMODULE hModLogclient;
	static SendLogAsyncIgnoreReply pfnAsyncSendLog;
	static 	CString strJuid;	//用户唯一码

	static BOOL LoadLogclientDll();
	
public:
	CString strOsVersion;	//操作系统版本号
	CString	Is64bit;		//0表示32位，1表示64位
	CString strFileVersion; //Tipsclient.dll的版本号
	CString	IsCanTipsRun;	//0表示不存在屏蔽软件或进程，1表示存在

	CString IsUDP;						//0表示http方式请求，1表示udp
	CString IsDownloadVastxmlSucceed;	//0表示配置下载失败，1表示成功
	CString IsSecure;					//0表示配置加密，1表示未加密，-1表示返回错误
	CString IsLoadVastxmlSucceed;		//0表示配置加载失败，1表示成功
	CString IsVastxmlEmpty;				//0表示配置返回为空，1表示不为空

	CString strAdid;					//广告单号
	CString	nAdType;					//广告类型：1:inline；2:wrapper；3:VastAdTagUrl；4:BackupAdList
	CString	IsAdEmpty;					//0表示此Ad节点为空，1表示不为空
	CString	existStartTrack;			//0表示不存在start监测，1表示存在
	CString existFirstQuartileTrack;	//0表示不存在1/4监测，1表示存在
	CString existMidpointTrack;			//0表示不存在1/2监测，1表示存在
	CString existThirdQuartileTrack;	//0表示不存在3/4监测，1表示存在
	CString existCompleteTrack;			//0表示不存在Complete监测，1表示存在
	CString existClickLink;				//0表示不存在Click点击，1表示存在
	CString existClickTrack;			//0表示不存在Click监测，1表示存在

	CString nTrackLinkType;				//监测类型：1:start；2: 1/4；3: 1/2；4: 3/4；5:complete；6:Click点击；7:Click监测 
public:
	LogVast() {};
	~LogVast() {};
public:
	void SendDllLoadLog();
	void SendConfigGetLog();
	void SendXmlParseLog();
	void SendLinkTrackLog();
};
