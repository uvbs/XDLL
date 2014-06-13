#include "stdafx.h"
#include "HopXmlConfig.h"
#include <ppl/util/log.h>
#include <ppl/stl/strings.h>
#include <ppl/util/random.h>

bool HopStateInfo::HitUrl(const CComBSTR& bstrUrl)
{
	for(vector<CComBSTR>::iterator iter = hitMatches.begin(); iter != hitMatches.end(); iter ++)
	{
		if ((*iter).Length() == 0)
			continue;
		if (!StrStrIW(bstrUrl, *iter))
		{	// 找不到，则表示失败
			return false;
		}
	}

	for(iter = missMatches.begin(); iter != missMatches.end(); iter ++)
	{
		if ((*iter).Length() == 0)
			continue;
		if (StrStrIW(bstrUrl, *iter))
		{	// 找到了，则表示失败
			return false;
		}
	}
	return true;
}	

bool HopStateInfo::HitHop(int nHop)
{
	return minHop <= nHop && nHop < maxHop;
}

const char* MakeToString(const char* ch)
{
	if (ch) return ch;
	else return "";
}

int MakeToInt(const char* ch)
{
	if (ch) return atoi(ch);
	else return 0;
}

float MakeToFloat(const char* ch)
{
	if (ch) return atof(ch);
	else return 0;
}

void HopXmlConfig::ClearData()
{
	clearCookies.clear();
	hops.clear();
	states.clear();
	m_isLoaded = false;
}

bool HopXmlConfig::LoadXml(string path)
{
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

bool HopXmlConfig::LoadXml(TiXmlElement* rootElement)
{
	ClearData();
	if (!rootElement)
		return false;
	
	HopXmlConfig& config = (*this);	
	APP_INFO("============ AnalyseXml Begin ============");
	TiXmlElement* element = rootElement->FirstChildElement();
	while ( element ) {
		string name = element->Value();
		string value = MakeToString(element->GetText());
		APP_INFO(name << " : " << value);

		if (name == "duration")
			config.duration = atoi(value.c_str());
		else if (name == "loadFlash")
			config.loadFlash = atoi(value.c_str());
		else if (name == "isOpenWindow")
			config.isOpenWindow = atoi(value.c_str());
		else if (name == "Platform")
			config.XmlPlatform = value;
		else if (name == "Urls")
		{
			TiXmlElement* urlElement = element->FirstChildElement();
			while (urlElement)
			{
				name = urlElement->Value();
				if (name == "url")
				{
					UrlInfo urlInfo;
					urlInfo.probability = MakeToInt(urlElement->Attribute("probability"));
					urlInfo.method = MakeToInt(urlElement->Attribute("method"));
					urlInfo.referer = MakeToString(urlElement->Attribute("referer"));
					urlInfo.url = MakeToString(urlElement->GetText());
					APP_INFO("UrlInfo: " << urlInfo.probability << " " << urlInfo.referer << " " << urlInfo.url);
					config.urls.push_back(urlInfo);
				}
				urlElement = urlElement->NextSiblingElement();
			}
		}
		else if (name == "UserAgents")
		{
			TiXmlElement* urlElement = element->FirstChildElement();
			while (urlElement)
			{
				name = urlElement->Value();
				if (name == "ua")
				{
					UserAgentInfo userAgentInfo;
					userAgentInfo.probability = MakeToInt(urlElement->Attribute("probability"));
					userAgentInfo.ua = MakeToString(urlElement->GetText());
					APP_INFO("UserAgentInfo: " << userAgentInfo.probability << " " << userAgentInfo.ua);
					config.userAgents.push_back(userAgentInfo);
				}
				urlElement = urlElement->NextSiblingElement();
			}
		}
		else if (name == "ClearCookies")
		{
			TiXmlElement* clearCookieElement = element->FirstChildElement();
			while (clearCookieElement)
			{
				name = clearCookieElement->Value();
				if (name == "Do")
				{
					ClearCookieInfo clearCookieInfo;
					clearCookieInfo.probability = MakeToInt(clearCookieElement->Attribute("probability"));
					clearCookieInfo.method = MakeToInt(clearCookieElement->Attribute("method"));
					clearCookieInfo.interval = MakeToInt(clearCookieElement->Attribute("interval"));
					clearCookieInfo.cookie = MakeToString(clearCookieElement->GetText());
					config.clearCookies.push_back(clearCookieInfo);
					APP_INFO("ClearCookie: " << clearCookieInfo.probability << " " << clearCookieInfo.cookie);
				}
				clearCookieElement = clearCookieElement->NextSiblingElement();
			}
		}
		else if (name == "ClearFlashCookies")
		{
			TiXmlElement* clearFlashCookieElement = element->FirstChildElement();
			while (clearFlashCookieElement)
			{
				name = clearFlashCookieElement->Value();
				if (name == "Do")
				{
					ClearFlashCookieInfo clearFlashCookieInfo;
					clearFlashCookieInfo.probability = MakeToInt(clearFlashCookieElement->Attribute("probability"));
					clearFlashCookieInfo.cookie = MakeToString(clearFlashCookieElement->GetText());
					APP_INFO("ClearFlashCookie: " << clearFlashCookieInfo.probability << " " << clearFlashCookieInfo.cookie);
					config.clearFlashCookies.push_back(clearFlashCookieInfo);
				}
				clearFlashCookieElement = clearFlashCookieElement->NextSiblingElement();
			}
		}
		else if (name == "Hops")
		{
			TiXmlElement* hopElement = element->FirstChildElement();
			while (hopElement)
			{
				name = hopElement->Value();
				if (name == "Hop")
				{
					HopInfo hopInfo;
					hopInfo.probability = MakeToInt(hopElement->Attribute("probability"));
					hopInfo.minInterval = MakeToInt(hopElement->Attribute("minInterval"));
					hopInfo.maxInterval = MakeToInt(hopElement->Attribute("maxInterval"));
					config.hops.push_back(hopInfo);
					APP_INFO("Hop: " << hopInfo.probability << " " << hopInfo.minInterval << " " << hopInfo.maxInterval);
				}
				hopElement = hopElement->NextSiblingElement();
			}
		}
		else if (name == "HopStates")
		{
			TiXmlElement* stateElement = element->FirstChildElement();
			while (stateElement)
			{
				name = stateElement->Value();
				if (name == "HopState")
				{
					HopStateInfo hopStateInfo;

					string hit = MakeToString(stateElement->Attribute("hit"));
					if (hit != "")
					{
						hopStateInfo.hitMatches.clear();
						vector<string> hits;
						strings::split( std::back_inserter(hits), hit, string("||||") );
						for (int i = 0; i < hits.size(); i ++)
							hopStateInfo.hitMatches.push_back( CComBSTR(hits[i].c_str()) );
					}
		
					string miss = MakeToString(stateElement->Attribute("miss"));
					if (miss != "")
					{
						hopStateInfo.missMatches.clear();
						vector<string> misses;
						strings::split( std::back_inserter(misses), miss, string("||||") );
						for (int i = 0; i < misses.size(); i ++)
							hopStateInfo.missMatches.push_back( CComBSTR(misses[i].c_str()) );
					}

					hopStateInfo.minHop = MakeToInt(stateElement->Attribute("minHop"));
					hopStateInfo.maxHop = MakeToInt(stateElement->Attribute("maxHop"));
					hopStateInfo.clickMode = MakeToInt(stateElement->Attribute("clickMode"));

					APP_INFO("HopState: " << hit << " " << miss << " " << hopStateInfo.clickMode << " "
						<< hopStateInfo.minHop << " " << hopStateInfo.maxHop);

					TiXmlElement* stateItem = stateElement->FirstChildElement();
					while(stateItem)
					{
						name = stateItem->Value();
						if (name == "Script")
						{
							hopStateInfo.script = MakeToString(stateItem->GetText());
						}
						else if (name == "Inputs")
						{
							TiXmlElement* inputElement = stateItem->FirstChildElement();
							while (inputElement)
							{
								name = inputElement->Value();
								if (name == "Input")
								{
									InputInfo inputInfo;
									inputInfo.name = MakeToString(inputElement->Attribute("name"));
									inputInfo.value = MakeToString(inputElement->GetText());
									APP_INFO("\tInput: " << inputInfo.name << " " << inputInfo.value);
									hopStateInfo.inputs.push_back(inputInfo);
								}	// end Input
								inputElement = inputElement->NextSiblingElement();
							}
						} // end Inputs
						else if (name == "Clickes")
						{
							TiXmlElement* clickElement = stateItem->FirstChildElement();
							while (clickElement)
							{
								name = clickElement->Value();
								if (name == "Click")
								{
									ClickInfo clickInfo;
									clickInfo.minProbability = MakeToInt(clickElement->Attribute("minProbability"));
									clickInfo.maxProbability = MakeToInt(clickElement->Attribute("maxProbability"));
									clickInfo.priority = MakeToInt(clickElement->Attribute("priority"));
									clickInfo.begin = CComBSTR(MakeToString(clickElement->Attribute("begin")));		
									
									string exclude = MakeToString(stateElement->Attribute("exclude"));
									if (exclude != "")
									{
										clickInfo.exclude.clear();
										vector<string> excludes;
										strings::split( std::back_inserter(excludes), exclude, string("||||") );
										for (int i = 0; i < excludes.size(); i ++)
											clickInfo.exclude.push_back( CComBSTR(excludes[i].c_str()) );
									}

									string mids = MakeToString(clickElement->GetText());
									if (mids != "")
									{
										vector<string> midss; 
										strings::split( std::back_inserter(midss), mids, string("||||") );
										for (int i = 0; i < midss.size(); i ++)
											clickInfo.mids.push_back( CComBSTR(midss[i].c_str()) );
									}

									hopStateInfo.clickes.insert(std::make_pair(clickInfo.priority, clickInfo));
									APP_INFO("\tClick: " << clickInfo.minProbability << " " << clickInfo.maxProbability << " " << clickInfo.begin);
								}	// end Click
								clickElement = clickElement->NextSiblingElement();
							}	// end Clickes
						}
						stateItem = stateItem->NextSiblingElement();
					}	
					config.states.push_back(hopStateInfo);
				}	// end HopState
				stateElement = stateElement->NextSiblingElement();
			}	
		}	// end HopStates
		element = element->NextSiblingElement();
	}

	APP_INFO("============ AnalyseXml End ============");
	m_isLoaded = true;
	return true;
}

bool HopXmlConfig::IsCorrect()
{
	if (m_isLoaded == false)
		return false;

	// 检查Url中有没有为空的

	// 检查中停留市场是否大于10s

	// 检查 每个点击是否有数据

	return true;
}

int HopXmlConfig::GetUrlRamdom()
{
	vector<int> region;
	int n = urls.size();
	int sum = 0;
	for (int i = 0; i < n; i ++)
	{
		sum += urls[i].probability;
		region.push_back(sum);
	}
	assert(n == region.size());
	int random = RandomGenerator().NextIn(sum);

	for (i = 0; i < n; i ++)
	{
		if (random < region[i])
			return i;
	}
	return 0;
}

int HopXmlConfig::GetUserAgentRandom()
{
	vector<int> region;
	int n = userAgents.size();
	int sum = 0;
	for (int i = 0; i < n; i ++)
	{
		sum += userAgents[i].probability;
		region.push_back(sum);
	}
	assert(n == region.size());
	int random = RandomGenerator().NextIn(sum);
	
	for (i = 0; i < n; i ++)
	{
		if (random < region[i])
			return i;
	}
	return 0;
}