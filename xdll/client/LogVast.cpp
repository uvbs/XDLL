#include "StdAfx.h"
#include "LogVast.h"
#include <ATLBASE.H>
#include "xdll.h"
#include <ppl/util/log.h>

bool LogVast::bNeedSendLog = false;
HMODULE LogVast::hModLogclient = NULL;
SendLogAsyncIgnoreReply  LogVast::pfnAsyncSendLog = NULL;
CString LogVast::strJuid = _T("");

BOOL LogVast::LoadLogclientDll() 
{
	APP_DEBUG(_T("logclient.dll load"));
	
	TCHAR szLogclientPath[MAX_PATH] = {0}; 
	GetModuleFileName(theApp.m_hInstance, szLogclientPath, MAX_PATH);
	PathRemoveFileSpec(szLogclientPath);
	PathAppend(szLogclientPath, _T("logclient.dll"));
	
	hModLogclient = ::LoadLibrary(szLogclientPath); 
	if (hModLogclient)
	{
		APP_EVENT(_T("logclient.dll load succeed"));
		pfnAsyncSendLog = (SendLogAsyncIgnoreReply)GetProcAddress(hModLogclient, _T("LogClient_SendLogAsyncIgnoreReply"));
		if (pfnAsyncSendLog)
		{
			APP_ERROR(_T("logclient.dll load succeed"));
			return TRUE;
		}
	}
	
	APP_ERROR(_T("logclient.dll load error"));
	return FALSE;
}

void LogVast::SendDllLoadLog()
{
	if (!bNeedSendLog) return;
	if (!pfnAsyncSendLog) return;

	USES_CONVERSION;
	LogClient_LogItem pStartMeta[2]; 
	pStartMeta[0].pName = L"Action";
	pStartMeta[0].pValue = L"3";
	pStartMeta[1].pName = NULL;
	pStartMeta[1].pValue = NULL;
	
	LogClient_LogItem pStartItems[6];
	pStartItems[0].pName = L"ev";
	pStartItems[0].pValue = L"101";
	pStartItems[1].pName = L"os";	//操作系统版本号
	pStartItems[1].pValue = T2W(strOsVersion);
	pStartItems[2].pName = L"bit";	//0表示32位，1表示64位
	pStartItems[2].pValue = T2W(Is64bit);
	pStartItems[3].pName = L"ver";	//Tipsclient.dll的版本号
	pStartItems[3].pValue = T2W(strFileVersion);
	pStartItems[4].pName = L"chk";	//0表示不存在屏蔽软件或进程，1表示存在
	pStartItems[4].pValue = T2W(IsCanTipsRun);
	pStartItems[5].pName = NULL;
	pStartItems[5].pValue = NULL;
	
	pfnAsyncSendLog(LogClientSource_Ppap, pStartMeta, pStartItems);	//http://ppap.synacast.com/1.html?
}

void LogVast::SendConfigGetLog()
{
	if (!bNeedSendLog) return;
	if (!pfnAsyncSendLog) return;

	USES_CONVERSION;
	LogClient_LogItem pGetMeta[2]; 
	pGetMeta[0].pName = L"Action";
	pGetMeta[0].pValue = L"3";
	pGetMeta[1].pName = NULL;
	pGetMeta[1].pValue = NULL;
	
	LogClient_LogItem pGetItems[8];
	pGetItems[0].pName = L"ev";
	pGetItems[0].pValue = L"103";
	pGetItems[1].pName = L"jid";
	pGetItems[1].pValue = T2W(strJuid);
	pGetItems[2].pName = L"udp";
	pGetItems[2].pValue = T2W(IsUDP);
	pGetItems[3].pName = L"dwn";
	pGetItems[3].pValue = T2W(IsDownloadVastxmlSucceed);
	pGetItems[4].pName = L"scr";
	pGetItems[4].pValue = T2W(IsSecure);
	pGetItems[5].pName = L"ldx";
	pGetItems[5].pValue = T2W(IsLoadVastxmlSucceed);
	pGetItems[6].pName = L"xil";
	pGetItems[6].pValue = T2W(IsVastxmlEmpty);
	pGetItems[7].pName = NULL;
	pGetItems[7].pValue = NULL;
	
	pfnAsyncSendLog(LogClientSource_Ppap, pGetMeta, pGetItems); //http://ppap.synacast.com/1.html?
}

void LogVast::SendXmlParseLog()
{
	if (!bNeedSendLog) return;
	if (!pfnAsyncSendLog) return;

	USES_CONVERSION;
	LogClient_LogItem pParseMeta[2]; 
	pParseMeta[0].pName = L"Action";
	pParseMeta[0].pValue = L"3";
	pParseMeta[1].pName = NULL;
	pParseMeta[1].pValue = NULL;
	
	LogClient_LogItem pParseItems[13];
	pParseItems[0].pName = L"ev";
	pParseItems[0].pValue = L"104";
	pParseItems[1].pName = L"jid";
	pParseItems[1].pValue = T2W(strJuid);
	pParseItems[2].pName = L"aid";
	pParseItems[2].pValue = T2W(strAdid);
	pParseItems[3].pName = L"aty";
	pParseItems[3].pValue = T2W(nAdType);
	pParseItems[4].pName = L"ail";
	pParseItems[4].pValue = T2W(IsAdEmpty);
	pParseItems[5].pName = L"sta";
	pParseItems[5].pValue = T2W(existStartTrack);
	pParseItems[6].pName = L"qrt";
	pParseItems[6].pValue = T2W(existFirstQuartileTrack);
	pParseItems[7].pName = L"mid";
	pParseItems[7].pValue = T2W(existMidpointTrack);
	pParseItems[8].pName = L"trd";
	pParseItems[8].pValue = T2W(existThirdQuartileTrack);
	pParseItems[9].pName = L"end";
	pParseItems[9].pValue = T2W(existCompleteTrack);
	pParseItems[10].pName = L"clk";
	pParseItems[10].pValue = T2W(existClickLink);
	pParseItems[11].pName = L"clm";
	pParseItems[11].pValue = T2W(existClickTrack);
	pParseItems[12].pName = NULL;
	pParseItems[12].pValue = NULL;
	
	pfnAsyncSendLog(LogClientSource_Ppap, pParseMeta, pParseItems); //http://ppap.synacast.com/1.html?
}

void LogVast::SendLinkTrackLog()
{
	if (!bNeedSendLog) return;
	if (!pfnAsyncSendLog) return;

	USES_CONVERSION;
	LogClient_LogItem pTrackMeta[2]; 
	pTrackMeta[0].pName = L"Action";
	pTrackMeta[0].pValue = L"3";
	pTrackMeta[1].pName = NULL;
	pTrackMeta[1].pValue = NULL;
	
	LogClient_LogItem pTrackItems[5];
	pTrackItems[0].pName = L"ev";
	pTrackItems[0].pValue = L"105";
	pTrackItems[1].pName = L"jid";
	pTrackItems[1].pValue = T2W(strJuid);
	pTrackItems[2].pName = L"aid";
	pTrackItems[2].pValue = T2W(strAdid);
	pTrackItems[3].pName = L"uty";
	pTrackItems[3].pValue = T2W(nTrackLinkType);
	pTrackItems[4].pName = NULL;
	pTrackItems[4].pValue = NULL;
	
	pfnAsyncSendLog(LogClientSource_Ppap, pTrackMeta, pTrackItems); //http://ppap.synacast.com/1.html?
}
