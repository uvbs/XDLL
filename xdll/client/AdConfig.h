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
	float clickRate;	// 对应接口中的 ClickRate
	string referer;		// 对应接口中的 referer
	string hops;		// 对应接口中的 Hops
	int minDuration;	// 对应接口中的 MinDuration 
	int maxDuration;	// 对应接口中的 MaxDuration
	int minHopTime;		// 对应接口中的 MinHopTime
	int maxHopTime;		// 对应接口中的 MaxHopTime
	string userAgent;	// 对应接口中的 UseraAgent

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
	string link;					// 广告点击链接, 对应VAST的ClickThrough，取最里层的
	vector<string> startLinks;		// 广告开始播放时，需要发出的监测
	vector<string> endLinks;		// 广告结束播放时，需要发出的监测
	
	vector<string> firstQuartileLinks;	// 广告播放1/4时发出的监测
	vector<string> midpointLinks;		// 广告播放1/2时发出的监测
	vector<string> thirdQuartileLinks;	// 广告播放3/4时发出的监测
	vector<string> closeLinks;			// 广告被主动关闭时发出的检测
	vector<string> clickLinks;			// 广告点击时发出的监测，对应VAST的ClickTracking
public:
	TempWrapperInfo(){}
	~TempWrapperInfo(){}
};

class ADInfo
{
public:
	int tid;
	int adverid;
	string adid;					// 广告单号			
	int playtime;					// 单位：秒
	string link;					// 广告点击链接, 对应VAST的ClickThrough，取最里层的
	string time;

	// VAST增加部分
	bool isClose;
	vector<string> startLinks;		// 广告开始播放时，需要发出的监测
	vector<string> endLinks;		// 广告结束播放时，需要发出的监测
	
	vector<string> firstQuartileLinks;	// 广告播放1/4时发出的监测
	vector<string> midpointLinks;		// 广告播放1/2时发出的监测
	vector<string> thirdQuartileLinks;	// 广告播放3/4时发出的监测
	vector<string> closeLinks;			// 广告被主动关闭时发出的检测
	vector<string> clickLinks;			// 广告点击时发出的监测，对应VAST的ClickTracking

	// Tips控制逻辑
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
	int getPlaytime(string strDuration);	//获得广告播放时长，格式hh:mm:ss，转换为int
	bool LoadXmlFromVast(string path, LogVast& vastGetLog);
	void FilterTracklinkLog(ADInfo& adinfo, LogVast& vastParseLog);	//日志只统计含jp.as.pptv.com的监测
	void ClearData();
	bool IsCorrect();
	bool GetWrapperURI(TempWrapperInfo& tWrapperInfo, const string& url, int RecursionNum);
	bool LoadXmlFromWraperURI(string path, TempWrapperInfo& tWrapperInfo, int RecursionNum);	//读取Wrapper中VASTAdTagURI
};
