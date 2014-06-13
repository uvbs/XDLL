
#pragma once

#include <ppl/util/random.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <string>
using std::string;

class MultiHopInfo;
class MultiHopUrlInfo;
class MultiHopXmlConfigInfo;
typedef boost::shared_ptr<MultiHopInfo> MultiHopInfoPtr;
typedef boost::shared_ptr<MultiHopUrlInfo> MultiHopUrlInfoPtr;
typedef boost::shared_ptr<MultiHopXmlConfigInfo> MultiHopXmlConfigInfoPtr;

//typedef std::list< CAdapt<CComBSTR> > bstr_list;
#include "HopXmlConfig.h"

class TaskPatterns
{
public:
	CComBSTR begin;	// 标签开头必须满足条件，不能为空
	CComBSTR mid;	// 标签中间必须满足条件，可以为空, 如果为空, 则表示任意
	CComBSTR url;	// url中间必须满足条件，可以为空, 如果为空, 则表示任意
	CComBSTR url_only;		// url必须完全满足,才能点
	CComBSTR url_forbidden;	// 不能执行的url,末尾匹配，可以非空，为空表示不得执行
};

typedef std::list<TaskPatterns> TaskPatternsList;

class MultiHopInfo : public boost::enable_shared_from_this<MultiHopInfo>
{
public:
	enum MultiHopInfoMode
	{
		UrlMode = 1,
		XmlConfigMode = 2
	};
public:
	//窗体大小
	int UaMinWidth;
	int UaMaxWidth;
	int UaMinHeight;
	int UaMaxHeight;

	string Platform;

	MultiHopInfoMode Mode;
	MultiHopInfo(MultiHopInfoMode mode) : Mode(mode), UaMinWidth(0), UaMaxWidth(0), UaMinHeight(0), UaMaxHeight(0) {}
public:
	virtual int GetLoadFlash() = 0;
	virtual int GetIsOpenWindow() = 0;
};

class MultiHopUrlInfo : public MultiHopInfo
{
public:
	MultiHopUrlInfo() : MultiHopInfo(MultiHopInfo::UrlMode)
		, AllowArea(0), LoadFlash(0), IsOpenWindow(0)
		, IsReferedBrowser(false), DirectionalMode(0), UrlFilterType(0){}
public:
	string Url;
	string Referer;

	std::vector<int> AllClickRates;

	//bstr_list Patterns;
	TaskPatternsList Patterns;

	DWORD TimeOutTime;
	DWORD ClickIntervalMin;
	DWORD ClickIntervalMax;

	DWORD RemoveCookiesRate;
	std::list<string> RemoveCookiesList;
	DWORD RemoveFlashCookiesRate;
	std::list<string> RemoveFlashCookiesList;

	USHORT AllowArea;
	USHORT LoadFlash;
	USHORT IsOpenWindow;

	// 是否在执行Referer的浏览器控件内
	bool IsReferedBrowser;
	string UserAgent;

	//? 加一个platform

	// 定向控制
	USHORT DirectionalMode;
	std::vector<string> DirectionalsArray;

	// UrlFilter
	USHORT UrlFilterType;
	std::list<wstring> UrlFilterList;

	bool ParseHopInfo( const string& s );
	bool ParseRuleInfo( const string& s );
	bool ParseRemoveCookies(const string& s);
	bool ParseFlashRemoveCookies(const string& s);
	bool ParseDirectionals(const string& s);
	bool ParseUrlFilter(const string& s);

	virtual int GetLoadFlash() {return LoadFlash;};
	virtual int GetIsOpenWindow() {return IsOpenWindow;}
};


class MultiHopXmlConfigInfo : public MultiHopInfo, public HopXmlConfig
{
public:
	MultiHopXmlConfigInfo() : MultiHopInfo(MultiHopInfo::XmlConfigMode), IsReferedBrowser(false) {}

public:
	string Url;
	string Referer;
	string UserAgent;
	bool IsReferedBrowser;

public:
	virtual int GetLoadFlash() {return this->loadFlash;};
	virtual int GetIsOpenWindow() {return this->isOpenWindow;}
};

