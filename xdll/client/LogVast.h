#pragma once

#include "logclient.h"
#include "logclient-source.h"
#include "logclient-error.h"

#define FILTER_SUB_URL "jp.as.pptv.com"

typedef INT  (__stdcall *SendLogAsyncIgnoreReply)(const char* pLogSource, const LogClient_LogItem* pMetaItems, const LogClient_LogItem* pItems);

class LogVast
{
public:
	static bool bNeedSendLog;	//��־�ķ�����Ϊ1/1000
	static HMODULE hModLogclient;
	static SendLogAsyncIgnoreReply pfnAsyncSendLog;
	static 	CString strJuid;	//�û�Ψһ��

	static BOOL LoadLogclientDll();
	
public:
	CString strOsVersion;	//����ϵͳ�汾��
	CString	Is64bit;		//0��ʾ32λ��1��ʾ64λ
	CString strFileVersion; //Tipsclient.dll�İ汾��
	CString	IsCanTipsRun;	//0��ʾ�����������������̣�1��ʾ����

	CString IsUDP;						//0��ʾhttp��ʽ����1��ʾudp
	CString IsDownloadVastxmlSucceed;	//0��ʾ��������ʧ�ܣ�1��ʾ�ɹ�
	CString IsSecure;					//0��ʾ���ü��ܣ�1��ʾδ���ܣ�-1��ʾ���ش���
	CString IsLoadVastxmlSucceed;		//0��ʾ���ü���ʧ�ܣ�1��ʾ�ɹ�
	CString IsVastxmlEmpty;				//0��ʾ���÷���Ϊ�գ�1��ʾ��Ϊ��

	CString strAdid;					//��浥��
	CString	nAdType;					//������ͣ�1:inline��2:wrapper��3:VastAdTagUrl��4:BackupAdList
	CString	IsAdEmpty;					//0��ʾ��Ad�ڵ�Ϊ�գ�1��ʾ��Ϊ��
	CString	existStartTrack;			//0��ʾ������start��⣬1��ʾ����
	CString existFirstQuartileTrack;	//0��ʾ������1/4��⣬1��ʾ����
	CString existMidpointTrack;			//0��ʾ������1/2��⣬1��ʾ����
	CString existThirdQuartileTrack;	//0��ʾ������3/4��⣬1��ʾ����
	CString existCompleteTrack;			//0��ʾ������Complete��⣬1��ʾ����
	CString existClickLink;				//0��ʾ������Click�����1��ʾ����
	CString existClickTrack;			//0��ʾ������Click��⣬1��ʾ����

	CString nTrackLinkType;				//������ͣ�1:start��2: 1/4��3: 1/2��4: 3/4��5:complete��6:Click�����7:Click��� 
public:
	LogVast() {};
	~LogVast() {};
public:
	void SendDllLoadLog();
	void SendConfigGetLog();
	void SendXmlParseLog();
	void SendLinkTrackLog();
};
