#include "StdAfx.h"
#include "AdConfig.h"
#include <ppl/util/log.h>
#include <boost/lexical_cast.hpp>
#include "TipsUtil.h"

const char* MakeToString(const char* ch);
int MakeToInt(const char* ch);
float MakeToFloat(const char* ch);

void AdConfig::ClearData()
{
	ads.clear();
	m_isLoaded = false;
}

bool AdConfig::LoadXml(string path)
{
	APP_EVENT("ClientSim Path:" << path);

	TiXmlDocument* myDocument = new TiXmlDocument();
	bool b = myDocument->LoadFile(path.c_str());
	if (!b)
	{
		APP_ERROR("Load Xml Failed ! " << path);
		m_isLoaded = false;
		return b;
	}
	
	TiXmlElement* rootElement = myDocument->RootElement();
	
	return LoadXml(rootElement);
}

int AdConfig::getPlaytime(string strDuration)
{
	//! 注意容错, 如果格式不对，也设置为15s
	int nErrorReturn = 15;

	size_t last = 0;
	size_t index = strDuration.find_first_of(':', last);
	if (string::npos == index)return nErrorReturn;
	int nplaytime = 3600 * atoi(strDuration.substr(last, index-last).c_str());
	
	last = index+1;
	index = strDuration.find_first_of(':', last);
	if (string::npos == index)return nErrorReturn;
	nplaytime += 60 * atoi(strDuration.substr(last, index-last).c_str());
	
	last = index +1;
	nplaytime += atoi(strDuration.substr(last).c_str()); 
	
	return nplaytime;
} 

bool AdConfig::GetWrapperURI(TempWrapperInfo& tWrapperInfo, const string& url, int RecursionNum)
{
	USES_CONVERSION;

	//只递归读取VASTAdTagURI五次
	if (RecursionNum > 5) 
	{
		return true;
	}

	// 下载VASTAdTagURI
	::DeleteUrlCacheEntry(A2T((char*)url.c_str()));
	TCHAR szCachePath[MAX_PATH] = {0};
	HRESULT hr = ::URLDownloadToCacheFile(NULL, A2T((char*)url.c_str()), szCachePath, MAX_PATH, NULL, NULL);
	if (S_OK != hr)
	{
		APP_ERROR("URLDownloadToFile " << url.c_str() << " Error");	// 下载失败
		return false;
	}
	
	//递归读取Wrapper中VASTAdTagURI
	bool bLoadXml = LoadXmlFromWraperURI(T2A(szCachePath), tWrapperInfo, RecursionNum);
	if (bLoadXml)
		return true;

	return false;
}

bool AdConfig::LoadXmlFromWraperURI(string path, TempWrapperInfo& tWrapperInfo, int RecursionNum)
{
	APP_EVENT("LoadXmlFromWrapperURI: " << path);
	TiXmlDocument* myDocument = new TiXmlDocument();
	bool b = myDocument->LoadFile(path.c_str());
	if (!b)
	{
		APP_ERROR("Load Xml From WrapperURI Failed ! " << path);
		return false;
	}
	APP_EVENT("Load Xml From WrapperURI Succeed...");

	TiXmlElement* rootElement = myDocument->RootElement();
	if (!rootElement)
	{
		return false;
	}
	APP_EVENT("rootElement: "<<rootElement->Value());

	TiXmlElement* element = rootElement->FirstChildElement("Ad");
	if (!element)
	{
		return false;	//VASTAdTagURI嵌套的xml中无<Ad>节点, 则返回false
	}

	while (element)
	{
		TiXmlElement* InLineOrWrapperElement = element->FirstChildElement();
		
		if(InLineOrWrapperElement)
		{
			// 判断是否是 InLine Or Wrapper，如果不是就放弃这个Ad，看下一个Ad
			string  strInLineOrWrapperName = MakeToString(InLineOrWrapperElement->Value());
			if (strInLineOrWrapperName == "InLine" || strInLineOrWrapperName == "Wrapper")
			{
				//Wrapper中存在VASTAdTagURI, 则递归获取
				if (strInLineOrWrapperName == "Wrapper")
				{
					TiXmlElement* VASTAdTagURIElement = InLineOrWrapperElement->FirstChildElement("VASTAdTagURI");
					if (VASTAdTagURIElement)
					{
						const char* VastAdTagUriText = VASTAdTagURIElement->GetText();
						if (VastAdTagUriText)
						{
							string tempWrapperUrl = VastAdTagUriText;
							bool bRtn = GetWrapperURI(tWrapperInfo, tempWrapperUrl, RecursionNum++);
							if (!bRtn) return false;
						}
					}
				}

				TiXmlElement* CreativesElement = InLineOrWrapperElement->FirstChildElement("Creatives");
				if (CreativesElement)
				{
					TiXmlElement* CreativeElement = CreativesElement->FirstChildElement("Creative");
					while (CreativeElement)
					{
						TiXmlElement* MaterialElement = CreativeElement->FirstChildElement();
						if (MaterialElement)
						{
							//! 判断一下 Linear or CompanionAds or NonLinearAds
							string  MaterialName = MakeToString(MaterialElement->Value());
							if (MaterialName == "Linear" || MaterialName == "CompanionAds" || MaterialName == "NonLinearAds")
							{
								//start|complete|firstQuartile|midpoint|thirdQuartile|close
								TiXmlElement* trackingEventsElement = MaterialElement->FirstChildElement("TrackingEvents");
								if (trackingEventsElement)
								{
									TiXmlElement* TrackingElement = trackingEventsElement->FirstChildElement("Tracking");
									while(TrackingElement)
									{
										string eventname = MakeToString(TrackingElement->Attribute("event"));
										if (eventname == "start")
										{
											tWrapperInfo.startLinks.push_back(MakeToString(TrackingElement->GetText()));
										}
										else if (eventname == "complete")
										{
											tWrapperInfo.endLinks.push_back(MakeToString(TrackingElement->GetText()));
										}
										else if (eventname == "firstQuartile")
										{
											tWrapperInfo.firstQuartileLinks.push_back(MakeToString(TrackingElement->GetText()));
										}
										else if (eventname == "midpoint")
										{
											tWrapperInfo.midpointLinks.push_back(MakeToString(TrackingElement->GetText()));
										}
										else if (eventname == "thirdQuartile")
										{
											tWrapperInfo.thirdQuartileLinks.push_back(MakeToString(TrackingElement->GetText()));
										}
										else if (eventname == "close")
										{
											tWrapperInfo.closeLinks.push_back(MakeToString(TrackingElement->GetText()));
										}
										
										TrackingElement = TrackingElement->NextSiblingElement("Tracking");
									}
								}
								
								//link, clicklinks
								TiXmlElement* videoClicksElement = MaterialElement->FirstChildElement("VideoClicks");
								if (NULL == videoClicksElement && NULL != trackingEventsElement)
								{
									videoClicksElement = trackingEventsElement->FirstChildElement("VideoClicks");
								}
								
								if(videoClicksElement)
								{
									TiXmlElement* clickThroughElement = videoClicksElement->FirstChildElement("ClickThrough");
									if (clickThroughElement)
									{
										//? 
										if (tWrapperInfo.link.empty())
										{
											tWrapperInfo.link = MakeToString(clickThroughElement->GetText());
										}
									}
									
									TiXmlElement* clicktrackingElement = videoClicksElement->FirstChildElement("ClickTracking");
									while (clicktrackingElement)
									{
										tWrapperInfo.clickLinks.push_back(MakeToString(clicktrackingElement->GetText()));
										clicktrackingElement = clicktrackingElement->NextSiblingElement("ClickTracking");
									}
								}
							
							}
						}
						CreativeElement = CreativeElement->NextSiblingElement("Creative");
					}
				}
			}		
		}

		//find next Ad
		element = element->NextSiblingElement("Ad");
	}

	return true;
}

bool AdConfig::LoadXmlFromVast(string path, LogVast& vastGetLog)
{
	APP_EVENT("LoadXmlFromVast: " << path); 
	TiXmlDocument* myDocument = new TiXmlDocument();
	bool b = myDocument->LoadFile(path.c_str());
	if (!b)
	{
		APP_ERROR("Load XmlFromVast Failed ! " << path);
		vastGetLog.IsLoadVastxmlSucceed = _T("0");
		vastGetLog.SendConfigGetLog();
		return b;
	}
	APP_EVENT("Load Xml Succeed...");

	TiXmlElement* rootElement = myDocument->RootElement();
	if (!rootElement)
	{
		vastGetLog.IsLoadVastxmlSucceed = _T("0");
		vastGetLog.SendConfigGetLog();
		return false;
	}
	APP_EVENT("rootElement: "<<rootElement->Value());
	vastGetLog.IsLoadVastxmlSucceed = _T("1");

	TiXmlElement* element = rootElement->FirstChildElement("Ad");
	if (element)
		vastGetLog.IsVastxmlEmpty = _T("0");	//vast配置返回不为空
	else
		vastGetLog.IsVastxmlEmpty = _T("1");	//vast配置返回为空
	vastGetLog.SendConfigGetLog();		

	while (element)
	{
		LogVast vastParseLog;
		if (LogVast::strJuid.IsEmpty())
			LogVast::strJuid = TipsUtil::GetJuid();
			
		string Adid = MakeToString(element->Attribute("id")); 
		vastParseLog.strAdid.Format(_T("%s"), Adid.c_str());

		TiXmlElement* InLineOrWrapperElement = element->FirstChildElement();

		if(InLineOrWrapperElement)
		{
			// 判断是否是 InLine Or Wrapper，如果不是就放弃这个Ad，看下一个Ad
			string  strInLineOrWrapperName = MakeToString(InLineOrWrapperElement->Value());
			if (strInLineOrWrapperName == "InLine" || strInLineOrWrapperName == "Wrapper")
			{	
				vastParseLog.nAdType = _T("1");
				//Wrapper中存在VASTAdTagURI, 则递归获取
				TempWrapperInfo tWrapperInfo; 
				if (strInLineOrWrapperName == "Wrapper")
				{
					vastParseLog.nAdType = _T("2");
					TiXmlElement* VASTAdTagURIElement = InLineOrWrapperElement->FirstChildElement("VASTAdTagURI");
					if (VASTAdTagURIElement)
					{
						vastParseLog.nAdType = _T("3");
						const char* VastAdTagUriText = VASTAdTagURIElement->GetText();
						if (VastAdTagUriText)
						{
							string tempWrapperUrl = VastAdTagUriText;
							bool bRtn = GetWrapperURI(tWrapperInfo, tempWrapperUrl, 1);
							if (!bRtn)
							{
								vastParseLog.IsAdEmpty = _T("1");
								vastParseLog.SendXmlParseLog();
								element = element->NextSiblingElement("Ad");
								continue;	//若VASTAdTagURI的链接，下载失败、解析失败或返回空，均跳过此<Ad>节点
							}
						}
					}
				}

				TiXmlElement* ImpressionElement = InLineOrWrapperElement->FirstChildElement("Impression");
				vector<string> impressions;
				while (ImpressionElement)
				{
					//APP_EVENT("Impression: "<<MakeToString(ImpressionElement->GetText()));
					impressions.push_back(MakeToString(ImpressionElement->GetText()));
					ImpressionElement = ImpressionElement->NextSiblingElement("Impression");
				}
				
				TiXmlElement* CreativesElement = InLineOrWrapperElement->FirstChildElement("Creatives");
				if (!CreativesElement)
				{
					vastParseLog.IsAdEmpty = _T("1");
					vastParseLog.SendXmlParseLog();
				}
				else
				{
					TiXmlElement* CreativeElement = CreativesElement->FirstChildElement("Creative");
					if (!CreativeElement)
					{
						vastParseLog.IsAdEmpty = _T("1");
						vastParseLog.SendXmlParseLog();
					}
					else
					{
						vastParseLog.IsAdEmpty = _T("0");
						vastParseLog.existStartTrack = _T("0");
						vastParseLog.existCompleteTrack = _T("0");
						vastParseLog.existFirstQuartileTrack = _T("0");
						vastParseLog.existMidpointTrack = _T("0");
						vastParseLog.existThirdQuartileTrack = _T("0");
						vastParseLog.existClickLink = _T("0");
						vastParseLog.existClickTrack = _T("0");

						while (CreativeElement)
						{
							ADInfo creativeInfo;
							
							creativeInfo.adid = Adid;	//广告单号

							for (vector<string>::iterator pImp = impressions.begin(); pImp != impressions.end(); ++pImp)
							{
								creativeInfo.startLinks.push_back(*pImp);
							}
							
							//将Wrapper中VASTAdTagURI嵌套的数据读出
							if (strInLineOrWrapperName == "Wrapper")
							{
								for (vector<string>::iterator pStart = tWrapperInfo.startLinks.begin(); pStart != tWrapperInfo.startLinks.end(); ++pStart)
								{
									creativeInfo.startLinks.push_back(*pStart);
								}
								
								for (vector<string>::iterator pEnd = tWrapperInfo.endLinks.begin(); pEnd != tWrapperInfo.endLinks.end(); ++pEnd)
								{
									creativeInfo.endLinks.push_back(*pEnd);
								}

								for (vector<string>::iterator pFirst = tWrapperInfo.firstQuartileLinks.begin(); pFirst != tWrapperInfo.firstQuartileLinks.end(); ++pFirst)
								{
									creativeInfo.firstQuartileLinks.push_back(*pFirst);
								}

								for (vector<string>::iterator pMid = tWrapperInfo.midpointLinks.begin(); pMid != tWrapperInfo.midpointLinks.end(); ++pMid)
								{
									creativeInfo.midpointLinks.push_back(*pMid);
								}

								for (vector<string>::iterator pThird = tWrapperInfo.thirdQuartileLinks.begin(); pThird != tWrapperInfo.thirdQuartileLinks.end(); ++pThird)
								{
									creativeInfo.thirdQuartileLinks.push_back(*pThird);
								}

								for (vector<string>::iterator pClose = tWrapperInfo.closeLinks.begin(); pClose != tWrapperInfo.closeLinks.end(); ++pClose)
								{
									creativeInfo.closeLinks.push_back(*pClose);
								}

								for (vector<string>::iterator pClick = tWrapperInfo.clickLinks.begin(); pClick != tWrapperInfo.clickLinks.end(); ++pClick)
								{
									creativeInfo.clickLinks.push_back(*pClick);
								}
							}

							TiXmlElement* MaterialElement = CreativeElement->FirstChildElement();
							if (MaterialElement)
							{
								//! 判断一下 Linear or CompanionAds or NonLinearAds
								string  MaterialName = MakeToString(MaterialElement->Value());
								if (MaterialName == "Linear" || MaterialName == "CompanionAds" || MaterialName == "NonLinearAds")
								{
									//tips
									TiXmlElement* CreativeExtensionsElement = MaterialElement->FirstChildElement("CreativeExtensions");
									if (CreativeExtensionsElement)
									{ 
										TiXmlElement* CreativeExtensionElement = CreativeExtensionsElement->FirstChildElement("CreativeExtension");
										if (CreativeExtensionElement)
										{
											TiXmlElement* tipsElement = CreativeExtensionElement->FirstChildElement("Tips");
											if (tipsElement)
											{
												TiXmlElement* UserAgentElement = tipsElement->FirstChildElement("UserAgent");
												if (UserAgentElement)
												{
													creativeInfo.tips.userAgent = MakeToString(UserAgentElement->GetText());
												}

												TiXmlElement* RefererElement = tipsElement->FirstChildElement("Referer");
												if(RefererElement)
												{
													creativeInfo.tips.referer = MakeToString(RefererElement->GetText());
												}

												TiXmlElement* ClickRateElement = tipsElement->FirstChildElement("ClickRate");
												if (ClickRateElement)
												{
													//creativeInfo.tips.clickRate = MakeToInt(ClickRateElement->GetText());
													creativeInfo.tips.clickRate = MakeToFloat(ClickRateElement->GetText());
												}

												TiXmlElement* HopsElement = tipsElement->FirstChildElement("Hops");
												if (HopsElement)
												{
													creativeInfo.tips.hops = MakeToString(HopsElement->GetText());
												}

												TiXmlElement* MinDurationElement = tipsElement->FirstChildElement("MinDuration");
												if (MinDurationElement)
												{
													creativeInfo.tips.minDuration = MakeToInt(MinDurationElement->GetText());
												}

												TiXmlElement* MaxDurationElement = tipsElement->FirstChildElement("MaxDuration");
												if (MaxDurationElement)
												{
													creativeInfo.tips.maxDuration = MakeToInt(MaxDurationElement->GetText());
												}

												TiXmlElement* MinHopTimeElement = tipsElement->FirstChildElement("MinHopTime");
												if(MinHopTimeElement)
												{
													creativeInfo.tips.minHopTime = MakeToInt(MinHopTimeElement->GetText());
												}

												TiXmlElement* MaxHopTimeElement = tipsElement->FirstChildElement("MaxHopTime");
												if (MaxHopTimeElement)
												{
													creativeInfo.tips.maxHopTime = MakeToInt(MaxHopTimeElement->GetText());
												}
										
												TiXmlElement* RuleElement = tipsElement->FirstChildElement("Rule");
												if(RuleElement)
												{
													creativeInfo.tips.rule = MakeToString(RuleElement->GetText());
												}
										
												TiXmlElement* GroupIDElement = tipsElement->FirstChildElement("GroupID");
												if (GroupIDElement)
												{
													creativeInfo.tips.groupID = MakeToInt(GroupIDElement->GetText());
												}
										
												TiXmlElement* RemoveCookieRateElement = tipsElement->FirstChildElement("RemoveCookieRate");
												if (RemoveCookieRateElement)
												{
													creativeInfo.tips.removeCookieRate = MakeToInt(RemoveCookieRateElement->GetText());
												}
										
												TiXmlElement* RemoveCookiesElement = tipsElement->FirstChildElement("RemoveCookies");
												if (RemoveCookiesElement)
												{
													creativeInfo.tips.removeCookies = MakeToString(RemoveCookiesElement->GetText());
												}
										
												TiXmlElement* RemoveFlashCookieRateElement = tipsElement->FirstChildElement("RemoveFlashCookieRate");
												if(RemoveFlashCookieRateElement)
												{
													creativeInfo.tips.removeFlashCookieRate = MakeToInt(RemoveFlashCookieRateElement->GetText());
												}

												TiXmlElement* RemoveFlashCookiesElement = tipsElement->FirstChildElement("RemoveFlashCookies");
												if(RemoveFlashCookiesElement)
												{
													creativeInfo.tips.removeFlashCookies = MakeToString(RemoveFlashCookiesElement->GetText());
												}

												TiXmlElement* LoadFlashElement = tipsElement->FirstChildElement("LoadFlash");
												if (LoadFlashElement)
												{
													creativeInfo.tips.loadFlash = MakeToInt(LoadFlashElement->GetText());
												}
										
												TiXmlElement* RefererModeElement = tipsElement->FirstChildElement("RefererMode");
												if (RefererModeElement)
												{
													creativeInfo.tips.refererMode = MakeToInt(RefererModeElement->GetText());
												}
										
												TiXmlElement* IsOpenWindowElement = tipsElement->FirstChildElement("IsOpenWindow");
												if(IsOpenWindowElement)
												{
													creativeInfo.tips.isOpenWindow = MakeToInt(IsOpenWindowElement->GetText());
												}
										
												TiXmlElement* DirectionalModeElement = tipsElement->FirstChildElement("DirectionalMode");
												if(DirectionalModeElement)
												{
													creativeInfo.tips.directionalMode = MakeToInt(DirectionalModeElement->GetText());
												}

												TiXmlElement* DirectionalElement = tipsElement->FirstChildElement("Directional");
												if (DirectionalElement)
												{
													creativeInfo.tips.directional = MakeToString(DirectionalElement->GetText());
												}
										
												TiXmlElement* UrlFilterTypeElement = tipsElement->FirstChildElement("UrlFilterType");
												if(UrlFilterTypeElement)
												{
													creativeInfo.tips.urlFilterType = MakeToInt(UrlFilterTypeElement->GetText());
												}
										
												TiXmlElement* UrlFilterElement = tipsElement->FirstChildElement("UrlFilter");
												if(UrlFilterElement)
												{
													creativeInfo.tips.urlFilter = MakeToString(UrlFilterElement->GetText());
												}

												TiXmlElement* PlatformElement = tipsElement->FirstChildElement("Platform");
												if(PlatformElement)
												{
													char* pPlatform = (char*)MakeToString(PlatformElement->GetText());
													creativeInfo.tips.platform = pPlatform;
												}
											}	
										}
									} 

									//playtime
									TiXmlElement* durationElement = MaterialElement->FirstChildElement("Duration");
									if (durationElement)
									{
										creativeInfo.playtime = getPlaytime(durationElement->GetText());
									} 
									else 
									{//! 如果没有，设为15s
										creativeInfo.playtime = 15;
									}
									
									//start|complete|firstQuartile|midpoint|thirdQuartile|close
									TiXmlElement* trackingEventsElement = MaterialElement->FirstChildElement("TrackingEvents");
									if (trackingEventsElement)
									{
										TiXmlElement* TrackingElement = trackingEventsElement->FirstChildElement("Tracking");
										while(TrackingElement)
										{
											string eventname = MakeToString(TrackingElement->Attribute("event"));
											if (eventname == "start")
											{
												creativeInfo.startLinks.push_back(MakeToString(TrackingElement->GetText()));
											}
											else if (eventname == "complete")
											{
												creativeInfo.endLinks.push_back(MakeToString(TrackingElement->GetText()));
											}
											else if (eventname == "firstQuartile")
											{
												creativeInfo.firstQuartileLinks.push_back(MakeToString(TrackingElement->GetText()));
											}
											else if (eventname == "midpoint")
											{
												creativeInfo.midpointLinks.push_back(MakeToString(TrackingElement->GetText()));
											}
											else if (eventname == "thirdQuartile")
											{
												creativeInfo.thirdQuartileLinks.push_back(MakeToString(TrackingElement->GetText()));
											}
											else if (eventname == "close")
											{
												creativeInfo.closeLinks.push_back(MakeToString(TrackingElement->GetText()));
											}
											
											TrackingElement = TrackingElement->NextSiblingElement("Tracking");
										}
									}
									
									//link, clicklinks
									/* VideoClicks 可能在 TrackingEvents中，这里做一下兼容*/
									TiXmlElement* videoClicksElement = MaterialElement->FirstChildElement("VideoClicks");
									if (NULL == videoClicksElement && NULL != trackingEventsElement)
									{
										videoClicksElement = trackingEventsElement->FirstChildElement("VideoClicks");
									}
									
									if(videoClicksElement)
									{
										TiXmlElement* clickThroughElement = videoClicksElement->FirstChildElement("ClickThrough");
										if (clickThroughElement)
										{
											creativeInfo.link = MakeToString(clickThroughElement->GetText());
										}
										
										if (strInLineOrWrapperName == "Wrapper" && false == tWrapperInfo.link.empty())
										{
											creativeInfo.link = tWrapperInfo.link;
										}

										TiXmlElement* clicktrackingElement = videoClicksElement->FirstChildElement("ClickTracking");
										while (clicktrackingElement)
										{
											creativeInfo.clickLinks.push_back(MakeToString(clicktrackingElement->GetText()));
											clicktrackingElement = clicktrackingElement->NextSiblingElement("ClickTracking");
										}
									}
								}
							}
							//判断此Ad节点是否存在start,1/4,1/2,3/4,complete,click点击,click监测,只统计含有jp.as.pptv.com的监测
							FilterTracklinkLog(creativeInfo, vastParseLog);
							//每个Creative封装成一个AdInfo,保存到vector<ADInfo> ads中;
							ads.push_back(creativeInfo);
							CreativeElement = CreativeElement->NextSiblingElement("Creative");
						}
						//发送vast配置解析日志
						vastParseLog.SendXmlParseLog();
					}
				}
			}		
		}

		//find next Ad
		element = element->NextSiblingElement("Ad");
	}

	return true;
}

void AdConfig::FilterTracklinkLog(ADInfo& adinfo, LogVast& vastParseLog)
{
	for (vector<string>::iterator pStart = adinfo.startLinks.begin(); pStart != adinfo.startLinks.end(); ++pStart)
	{
		if (string::npos != pStart->find(FILTER_SUB_URL))
			vastParseLog.existStartTrack = _T("1");
	}
	
	for (vector<string>::iterator pEnd = adinfo.endLinks.begin(); pEnd != adinfo.endLinks.end(); ++pEnd)
	{
		if (string::npos != pEnd->find(FILTER_SUB_URL))
			vastParseLog.existCompleteTrack = _T("1");
	}
	
	for (vector<string>::iterator pFirst = adinfo.firstQuartileLinks.begin(); pFirst != adinfo.firstQuartileLinks.end(); ++pFirst)
	{
		if (string::npos != pFirst->find(FILTER_SUB_URL))
			vastParseLog.existFirstQuartileTrack = _T("1");
	}
	
	for (vector<string>::iterator pMid = adinfo.midpointLinks.begin(); pMid != adinfo.midpointLinks.end(); ++pMid)
	{
		if (string::npos != pMid->find(FILTER_SUB_URL))
			vastParseLog.existMidpointTrack = _T("1");
	}
	
	for (vector<string>::iterator pThird = adinfo.thirdQuartileLinks.begin(); pThird != adinfo.thirdQuartileLinks.end(); ++pThird)
	{
		if (string::npos != pThird->find(FILTER_SUB_URL))
			vastParseLog.existThirdQuartileTrack = _T("1");
	}
	
	if (!adinfo.link.empty())
		vastParseLog.existClickLink = _T("1");
	
	for (vector<string>::iterator pClick = adinfo.clickLinks.begin(); pClick != adinfo.clickLinks.end(); ++pClick)
	{
		if (string::npos != pClick->find(FILTER_SUB_URL))
			vastParseLog.existClickTrack = _T("1");
	}
}

void ReplaceFirstSelf(string& str, const string& oldStr, const string& newStr)
{
	int npos = str.find(oldStr);
	if (npos == -1)
		return;
	str.replace(npos, oldStr.size(), newStr);
}

bool AdConfig::LoadXml(TiXmlElement* rootElement)
{
	ClearData();
	if (!rootElement)
		return false;
	
	AdConfig& config = (*this);	
	APP_INFO("============ AnalyseXml Begin ============");
	TiXmlElement* element = rootElement->FirstChildElement();
	while ( element ) {
		string name = element->Value();
		string value = "";
		if (name == "item")
		{
			int tid = 0;
			TiXmlElement* itemElement = element->FirstChildElement();
			while (itemElement)
			{
				name = itemElement->Value();
				
				if (name == "tid")
					tid = MakeToInt(itemElement->GetText());
				else if (name == "ad")
				{
					ADInfo adInfo;
					adInfo.tid = tid;

					TiXmlElement* adElement = itemElement->FirstChildElement();
					while (adElement)
					{
						name = adElement->Value();

						if (name == "adverid")
							adInfo.adverid = MakeToInt(adElement->GetText());
						else if (name == "playtime")
							adInfo.playtime = MakeToInt(adElement->GetText());
						else if (name == "link")
							adInfo.link = MakeToString(adElement->GetText());
						else if (name == "time")
							adInfo.time = MakeToString(adElement->GetText());
						else if (name == "VideoAD")
						{
							TiXmlElement* videoADElement = adElement->FirstChildElement();
							while(videoADElement)
							{
								name = videoADElement->Value();

								if (name == "ThirdMonitor")
								{
									TiXmlElement* tmElement = videoADElement->FirstChildElement();
									while (tmElement)
									{
										name = tmElement->Value();
										if (name == "Start")
										{
											TiXmlElement* startElement = tmElement->FirstChildElement();
											while(startElement)
											{
												name = startElement->Value();
												if (name == "url")
												{
													string url = MakeToString(startElement->GetText());
													if (url != "")
														adInfo.startLinks.push_back(url);
												}
												startElement = startElement->NextSiblingElement();
											}	// end while startElement
										}
										else if (name == "End")
										{
											TiXmlElement* endElement = tmElement->FirstChildElement();
											while(endElement)
											{
												name = endElement->Value();
												if (name == "url")
												{
													string url = MakeToString(endElement->GetText());
													if (url != "")
														adInfo.endLinks.push_back(url);
												}
												endElement = endElement->NextSiblingElement();
											}	// end while endElement
										}
										tmElement = tmElement->NextSiblingElement();
									}	// end while tmElement

								}

								videoADElement = videoADElement->NextSiblingElement();
							}	// end while videoADElement
						}
						else if (name == "Extension")
						{
							TiXmlElement* extensionElement = adElement->FirstChildElement();
							while(extensionElement)
							{
								name = extensionElement->Value();
								if (name == "Tips")
								{
									TiXmlElement* tipsElement = extensionElement->FirstChildElement();
									while(tipsElement)
									{
										name = tipsElement->Value();
										if (name == "Referer")
											adInfo.tips.referer = MakeToString(tipsElement->GetText());
										else if (name == "ClickRate")
											adInfo.tips.clickRate = MakeToInt(tipsElement->GetText());
										else if (name == "Hops")
											adInfo.tips.hops = MakeToString(tipsElement->GetText());
										else if (name == "MinDuration")
											adInfo.tips.minDuration = MakeToInt(tipsElement->GetText());
										else if (name == "MaxDuration")
											adInfo.tips.maxDuration = MakeToInt(tipsElement->GetText());
										else if (name == "MinHopTime")
											adInfo.tips.minHopTime = MakeToInt(tipsElement->GetText());
										else if (name == "MaxHopTime")
											adInfo.tips.maxHopTime = MakeToInt(tipsElement->GetText());
										else if (name == "UserAgent")
											adInfo.tips.userAgent = MakeToString(tipsElement->GetText());

										else if (name == "Rule")
											adInfo.tips.rule = MakeToString(tipsElement->GetText());
										else if (name == "GroupID")
											adInfo.tips.groupID = MakeToInt(tipsElement->GetText());
										else if (name == "RemoveCookieRate")
											adInfo.tips.removeCookieRate = MakeToInt(tipsElement->GetText());
										else if (name == "RemoveCookies")
											adInfo.tips.removeCookies = MakeToString(tipsElement->GetText());
										else if (name == "RemoveFlashCookieRate")
											adInfo.tips.removeFlashCookieRate = MakeToInt(tipsElement->GetText());
										else if (name == "RemoveFlashCookies")
											adInfo.tips.removeFlashCookies = MakeToString(tipsElement->GetText());
										else if (name == "LoadFlash")
											adInfo.tips.loadFlash = MakeToInt(tipsElement->GetText());
										else if (name == "RefererMode")
											adInfo.tips.refererMode = MakeToInt(tipsElement->GetText());\
										else if (name == "IsOpenWindow")
											adInfo.tips.isOpenWindow = MakeToInt(tipsElement->GetText());
										else if (name == "DirectionalMode")
											adInfo.tips.directionalMode = MakeToInt(tipsElement->GetText());
										else if (name == "Directional")
											adInfo.tips.directional = MakeToString(tipsElement->GetText());
										else if (name == "UrlFilterType")
											adInfo.tips.urlFilterType = MakeToInt(tipsElement->GetText());
										else if (name == "UrlFilter")
											adInfo.tips.urlFilter = MakeToString(tipsElement->GetText());

										tipsElement = tipsElement->NextSiblingElement();
									}
								}
								extensionElement = extensionElement->NextSiblingElement();
							}
						}
						adElement = adElement->NextSiblingElement();
					}	// end while adElement

					// adInfo添加pptv自统计信息
					if (adInfo.time != "")
					{
						string startString = adInfo.time + "&siteid=99990023";
						string endString = adInfo.time + "&siteid=99990023";
						ReplaceFirstSelf(startString,	"playact=", "playact=1000");
						ReplaceFirstSelf(endString,		"playact=", "playact=1001");
						ReplaceFirstSelf(startString,	"cid=", "cid=99990023");
						ReplaceFirstSelf(endString,		"cid=", "cid=99990023");
						ReplaceFirstSelf(startString,	"pid=", "pid=99990023");
						ReplaceFirstSelf(endString,		"pid=", "pid=99990023");
						ReplaceFirstSelf(startString,	"yid=", "yid=991");
						ReplaceFirstSelf(endString,		"yid=", "yid=991");
						ReplaceFirstSelf(startString,	"times=", "times=1");
						ReplaceFirstSelf(endString,		"times=", "times=1");
						ReplaceFirstSelf(startString,	"sec=", "sec=" + boost::lexical_cast<string>(adInfo.playtime));
						ReplaceFirstSelf(endString,		"sec=", "sec=" + boost::lexical_cast<string>(adInfo.playtime));
						adInfo.startLinks.push_back(startString);
						adInfo.endLinks.push_back(endString);
					}
					
					//APP_INFO("UrlInfo: " << urlInfo.probability << " " << urlInfo.referer << " " << urlInfo.url);
					config.ads.push_back(adInfo);
				}
				itemElement = itemElement->NextSiblingElement();
			}	// end while itemElement
		}
		element = element->NextSiblingElement();
	}	// end while element

	APP_INFO("============ AnalyseXml End ============");
	return true;
}

bool AdConfig::IsCorrect()
{
	if (m_isLoaded == false)
		return false;
	
	// 检查link中有没有为空的
	
	return true;
}