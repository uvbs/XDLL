#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>

#include "tinyxml.h"

using namespace std;

class UrlInfo
{
public:
	int probability;
	int method;
	string referer;
	string url;
public:
	UrlInfo() : method(0) {}
};

class UserAgentInfo
{
public:
	int probability;
	string ua;
public:
	UserAgentInfo() : probability(0) {}
};

class ClearCookieInfo
{
public:
	int probability;
	int method;
	u_int interval;
	string cookie;
public:
	ClearCookieInfo() : probability(0), method(0), interval(0) {}
};

class ClearFlashCookieInfo
{
public:
	int probability;
	string cookie;
public:
	ClearFlashCookieInfo() :  probability(0) {}
};

class HopInfo
{
public:
	int probability;
	int minInterval;
	int maxInterval;
public:
	HopInfo() : probability(100), minInterval(5), maxInterval(10) {}
};

class InputInfo
{
public:
	string name;
	string value;
};

class ClickInfo
{
public:
	int minProbability;
	int maxProbability;
	int priority;
	CComBSTR begin;
	vector<CComBSTR> mids;
	vector<CComBSTR> exclude;
public:
	ClickInfo() : minProbability(0), maxProbability(0) {}
};

class HopStateInfo
{
public:
	vector<CComBSTR> hitMatches;
	vector<CComBSTR> missMatches;
	int minHop;
	int maxHop;
	int clickMode;
	string script;							//发下执行的脚本
	vector<InputInfo> inputs;
	multimap<int, ClickInfo> clickes;		//Key: priority  Value: ClickInfo
public:
	HopStateInfo() : minHop(0), maxHop(0), clickMode(0) {}

	// 检查Url是否满足状态条件
	bool HitUrl(const CComBSTR& url);
	// 检查条数是否满足状态条件
	bool HitHop(int nHop);
};

class HopXmlConfig
{
public:
	vector<UrlInfo> urls;
	int duration;
	int loadFlash;
	int isOpenWindow;
	string XmlPlatform;
	vector<UserAgentInfo> userAgents;
	vector<ClearCookieInfo> clearCookies;
	vector<ClearFlashCookieInfo> clearFlashCookies;
	vector<HopInfo> hops;
	vector<HopStateInfo> states;
public:
	bool m_isLoaded;
public:
	HopXmlConfig() : duration(180), loadFlash(0), m_isLoaded(false) {}
	bool LoadXml(string path);
	bool LoadXml(TiXmlElement* rootElement);
	void ClearData();
	bool IsCorrect();

	int GetUrlRamdom();
	int GetUserAgentRandom();
};