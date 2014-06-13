#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>

#include "tinyxml.h"
#include "LogVast.h"
using namespace std;

class ADInfoTips
{
public:
	float clickRate;	// ��Ӧ�ӿ��е� ClickRate
	string referer;		// ��Ӧ�ӿ��е� referer
	string hops;		// ��Ӧ�ӿ��е� Hops
	int minDuration;	// ��Ӧ�ӿ��е� MinDuration 
	int maxDuration;	// ��Ӧ�ӿ��е� MaxDuration
	int minHopTime;		// ��Ӧ�ӿ��е� MinHopTime
	int maxHopTime;		// ��Ӧ�ӿ��е� MaxHopTime
	string userAgent;	// ��Ӧ�ӿ��е� UseraAgent

	string rule;		
	int groupID;
	int removeCookieRate;
	string removeCookies;
	int removeFlashCookieRate;
	string removeFlashCookies;
	int loadFlash;
	int refererMode;
	int isOpenWindow;
	int directionalMode;
	string directional;
	int urlFilterType;
	string urlFilter;
	string platform;
//	float ctr;
	int uaMinWidth;
	int uaMaxWidth;
	int uaMinHeight;
	int uaMaxHeight;

	ADInfoTips() 
	{
#ifdef _DEBUG
		clickRate = 100;
#else
		clickRate = 2;
#endif
		
		referer = "";
		hops = "40|40|30|30";
		minDuration = 180;
		maxDuration = 240;
		minHopTime = 30;
		maxHopTime = 40;

		rule = "<A ";

		groupID = 0;
		removeCookieRate = 0;
		removeFlashCookieRate = 0;
		loadFlash = 0;
		refererMode = 1;
		isOpenWindow = 0;
		directionalMode = 0;
		urlFilterType = 0;
//		ctr = 0;
		uaMinWidth = 0;
		uaMaxWidth = 0;
		uaMinHeight = 0;
		uaMaxHeight = 0;
	}
};

class TempWrapperInfo
{
public:
	string link;					// ���������, ��ӦVAST��ClickThrough��ȡ������
	vector<string> startLinks;		// ��濪ʼ����ʱ����Ҫ�����ļ��
	vector<string> endLinks;		// ����������ʱ����Ҫ�����ļ��
	
	vector<string> firstQuartileLinks;	// ��沥��1/4ʱ�����ļ��
	vector<string> midpointLinks;		// ��沥��1/2ʱ�����ļ��
	vector<string> thirdQuartileLinks;	// ��沥��3/4ʱ�����ļ��
	vector<string> closeLinks;			// ��汻�����ر�ʱ�����ļ��
	vector<string> clickLinks;			// �����ʱ�����ļ�⣬��ӦVAST��ClickTracking
public:
	TempWrapperInfo(){}
	~TempWrapperInfo(){}
};

class ADInfo
{
public:
	int tid;
	int adverid;
	string adid;					// ��浥��			
	int playtime;					// ��λ����
	string link;					// ���������, ��ӦVAST��ClickThrough��ȡ������
	string time;

	// VAST���Ӳ���
	bool isClose;
	vector<string> startLinks;		// ��濪ʼ����ʱ����Ҫ�����ļ��
	vector<string> endLinks;		// ����������ʱ����Ҫ�����ļ��
	
	vector<string> firstQuartileLinks;	// ��沥��1/4ʱ�����ļ��
	vector<string> midpointLinks;		// ��沥��1/2ʱ�����ļ��
	vector<string> thirdQuartileLinks;	// ��沥��3/4ʱ�����ļ��
	vector<string> closeLinks;			// ��汻�����ر�ʱ�����ļ��
	vector<string> clickLinks;			// �����ʱ�����ļ�⣬��ӦVAST��ClickTracking

	// Tips�����߼�
	ADInfoTips tips;
public:
	ADInfo() : tid(0),adverid(0),playtime(0), isClose(false) {}
};

class AdConfig
{
public:
	vector<ADInfo> ads;
	
public:
	bool m_isLoaded;
public:
	AdConfig(){}
	bool LoadXml(string path);
	bool LoadXml(TiXmlElement* rootElement);
	int getPlaytime(string strDuration);	//��ù�沥��ʱ������ʽhh:mm:ss��ת��Ϊint
	bool LoadXmlFromVast(string path, LogVast& vastGetLog);
	void FilterTracklinkLog(ADInfo& adinfo, LogVast& vastParseLog);	//��־ֻͳ�ƺ�jp.as.pptv.com�ļ��
	void ClearData();
	bool IsCorrect();
	bool GetWrapperURI(TempWrapperInfo& tWrapperInfo, const string& url, int RecursionNum);
	bool LoadXmlFromWraperURI(string path, TempWrapperInfo& tWrapperInfo, int RecursionNum);	//��ȡWrapper��VASTAdTagURI
};
