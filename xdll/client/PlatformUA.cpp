#include "stdAfx.h"
#include "PlatformUA.h"
#include <ATLBASE.H>
#include "TipsUtil.h"
#include <fstream>
#include <synacast/util/SynacastUrlEncoding.h>
#include <sstream>
#include "AutoLocks.h"

const char* MakeToString(const char* ch);
int MakeToInt(const char* ch);

CriticalSection gSingletonCritical;  //单例的临界对象
CriticalSection gPlatformUaCritical; //读取UA数据临界对象

#define VERSION_INI_URL _T("http://fas.aplus.pptv.com/ua/uaversion.ini")
#define UA_XML_URL _T("http://fas.aplus.pptv.com/ua/uadetails_s.xml")

TypeOfUA::TypeOfUA(TypeOfUA &ua)
{
	map<string, vector<ClickData> >::iterator m_ip;
	for (m_ip = ua.vDevice.begin(); m_ip != ua.vDevice.end(); m_ip++)
	{
		vector<ClickData> vTemp;
		vector<ClickData>::iterator vIt;
		for (vIt = m_ip->second.begin(); vIt != m_ip->second.end(); vIt++)
		{
			vTemp.push_back(*vIt);
		}
		vDevice[m_ip->first] = vTemp;
	}
}

TypeOfUA& TypeOfUA::operator=(TypeOfUA& ua)
{
	if(this == &ua)return *this;

	map<string, vector<ClickData> >::iterator m_ip;
	for (m_ip = ua.vDevice.begin(); m_ip != ua.vDevice.end(); m_ip++)
	{
		vector<ClickData> vTemp;
		vector<ClickData>::iterator vIt;
		for (vIt = m_ip->second.begin(); vIt != m_ip->second.end(); vIt++)
		{
			vTemp.push_back(*vIt);
		}
		vDevice[m_ip->first] = vTemp;
	}

	return *this;
}


PlatformUA* PlatformUA::m_Instance = NULL;

int PlatformUA::uaMinWidth = 0;
int PlatformUA::uaMaxWidth = 0;
int PlatformUA::uaMinHeight = 0;
int PlatformUA::uaMaxHeight = 0;

PlatformUA* PlatformUA::GetInstance()
{
	if (NULL == m_Instance)
	{   
		Lock autolock(&gSingletonCritical);
		if (NULL == m_Instance)
		{
			m_Instance = new PlatformUA;
		}
	}
	return m_Instance;
}

ClickData& ValueClickData(ClickData& tmpClickdata,
					int ratio,
					int uaMinWidth,
					int uaMaxWidth,
					int uaMinHeight,
					int uaMaxHeight,
					string ua)
{
	tmpClickdata.ratio = ratio;
	tmpClickdata.uaMinWidth = uaMinWidth;
	tmpClickdata.uaMaxWidth = uaMaxWidth;
	tmpClickdata.uaMinHeight = uaMinHeight;
	tmpClickdata.uaMaxHeight = uaMaxHeight;
	tmpClickdata.ua = ua;
	
	return tmpClickdata;
}

void SetDefaultIphoneUaInfo(vector<ClickData>& vIphoneDevice)
{
	ClickData iphoneClickData;
	
	//iphone4
	vIphoneDevice.push_back(
		ValueClickData(iphoneClickData, 3734, 640, 640, 960, 960,
		"Mozilla/5.0 (iPhone; U; CPU iPhone OS 4_0 like Mac OS X; en-us) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8A293 Safari/6531.22.7")
	);
	
	//iphone5
	vIphoneDevice.push_back(
		ValueClickData(iphoneClickData, 3460, 640, 640, 1136, 1136,
		"Mozilla/5.0 (iPhone; CPU iPhone OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Mobile/10A403")
	);
	
	//iphone touch 4G
	vIphoneDevice.push_back(
		ValueClickData(iphoneClickData, 164, 640, 640, 960, 960,
		"Mozilla/5.0 (iPod; U; CPU iPhone OS 4_3_3 like Mac OS X; en-us) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8J2 Safari/6533.18.5")
	);
}

void SetDefaultIpadUaInfo(vector<ClickData>& vIpadDevice)
{
	ClickData ipadClickData;

	//ipad 2G
	vIpadDevice.push_back(
		ValueClickData(ipadClickData, 2002, 768, 768, 1024, 1024,
		"Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X; en-us) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9B176 Safari/7534.48.3")
	);

	//ipad 1G
	vIpadDevice.push_back(
		ValueClickData(ipadClickData, 496, 768, 768, 1024, 1024,
		"Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10")
	);
}

void SetDefaultAphoneUaInfo(vector<ClickData>& vAphoneDevice)
{
	ClickData aphoneClickData;
	
	//GT-I9300
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 1375, 720, 720, 1280, 1280,
		"Mozilla/5.0 (Linux; U; Android 4.0.4; en-gb; GT-I9300 Build/IMM76D) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30")
	);

	//GT-N7100
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 1136, 720, 720, 1280, 1280,
		"Mozilla/5.0 (Linux; U; Android 4.1.1; el-gr; SAMSUNG GT-N7100/N7100XXALIJ Build/JRO03C) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30")
	);

	//MI 1S
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 948, 480, 480, 854, 854,
		"Mozilla/5.0 (Linux; U; Android 4.1.2; en-us; MI 1S Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30 XiaoMi/MiuiBrowser/1.0")
	);

	//MI 2
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 857, 720, 720, 1280, 1280,
		"Mozilla/5.0 (Linux; U; Android 4.1.1; zh-cn; MI 2 Build/JRO03L) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30 XiaoMi/MiuiBrowser/1.0")
	);

	//GT-I9100
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 676, 480, 480, 800, 800,
		"Mozilla/5.0 (Linux; U; Android 4.0.3; fr-fr; GT-I9100 Samsung Galaxy S2 Build/IML74K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30")
	);

	//MI 2S
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 537, 720, 720, 1280, 1280,
		"Mozilla/5.0 (Linux; U; Android 4.1.1; zh-tw; MI 2S Build/JRO03L) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30 XiaoMi/MiuiBrowser/1.0")
	);

	//GT-S5830i
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 486, 320, 320, 480, 480,
		"Mozilla/5.0 (Linux; U; Android 2.3.6; hu-hu; SAMSUNG GT-S5830i/S5830iXXLB2 Build/GINGERBREAD) AppleWebKit/533.1 (KHTML like Gecko) Version/4.0 Mobile Safari/533.1")
	);

	//MI-ONE Plus
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 433, 480, 480, 854, 854,
		"Mozilla/5.0 (Linux; U; Android 2.3.5; zh-cn; MI-ONE Plus Build/GINGERBREAD) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1")
	);

	//GT-I9500
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 376, 1080, 1080, 1920, 1920,
		"Mozilla/5.0 (SAMSUNG; SAMSUNG-GT-i9500/1.0; U; Tizen/1.0 like Android; en-us) AppleWebKit/534.46 (KHTML, like Gecko) SLP Browser/1.0 Mobile")
	);

	//GT-N7000
	vAphoneDevice.push_back(
		ValueClickData(aphoneClickData, 342, 800, 800, 1280, 1280,
		"Mozilla/5.0 (Linux; U; Android 2.3.6; es-es; SAMSUNG GT-N7000/N7000XXLA6 Build/GINGERBREAD) AppleWebKit/533.1 (KHTML like Gecko) Version/4.0 Mobile Safari/533.1")
	);
}

void PlatformUA::DefaultUaInfo()
{
	m_ua.vDevice.clear();
	
	//默认的iphone设备
	vector<ClickData> vIphoneDevice;
	SetDefaultIphoneUaInfo(vIphoneDevice);
	m_ua.vDevice["iphone"] = vIphoneDevice;
	
	//默认的ipad设备
	vector<ClickData> vIpadDevice;
	SetDefaultIpadUaInfo(vIpadDevice);
	m_ua.vDevice["ipad"] = vIpadDevice;

	//默认的Aphone设备
	vector<ClickData> vAphoneDevice;
	SetDefaultAphoneUaInfo(vAphoneDevice);
	m_ua.vDevice["aphone"] = vAphoneDevice;
}

void PlatformUA::SetUaInfo(TypeOfUA& InputUA)
{
		Lock autolock(&gPlatformUaCritical);
		m_ua = InputUA;	
}

void PlatformUA::GetUaInfo(TypeOfUA& OutputUA)
{
		Lock autolock(&gPlatformUaCritical);
		OutputUA =  m_ua;
}

void PlatformUA::DeleteInstance()
{
	if (m_Instance != NULL)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
}

//下载最新的UaDetails.xml文件
BOOL PlatformUA::UpdateUaXml(LPCTSTR lpVersionIniPath, LPCTSTR lpUaXmlPath)
{
	//判断是否存在UaVersion.ini
	if (PathFileExists(lpVersionIniPath) && PathFileExists(lpUaXmlPath))
	{
		// 下载Ini文件到缓存
		::DeleteUrlCacheEntry(VERSION_INI_URL);
		TCHAR chNewIniCache[MAX_PATH] = {0};
		HRESULT hr = URLDownloadToCacheFile(NULL, VERSION_INI_URL, chNewIniCache, MAX_PATH, 0, NULL);
		if(FAILED(hr))
		{
			APP_EVENT("Download UaVersion.ini CacheFile Failed. ");
			return FALSE;
		}
		APP_EVENT("Download UaVersion.ini CacheFile Succeed. ");
		
		//对比线上版本
		int nNewVersion = ::GetPrivateProfileInt(_T("Data"), _T("Version"), 0, chNewIniCache);
		int nOldVersion = ::GetPrivateProfileInt(_T("Data"), _T("Version"), 0, lpVersionIniPath);
		if (nNewVersion <= nOldVersion)
		{
			//不需要更新xml
			return TRUE;
		}
		else
		{
			//替换老的ini文件
			if (!CopyFile(chNewIniCache, lpVersionIniPath, FALSE))
				return FALSE;	
		}
	}
	else
	{
		//更新UaVersion.ini
		::DeleteUrlCacheEntry(VERSION_INI_URL);
		if(S_OK != ::URLDownloadToFile(NULL, VERSION_INI_URL, lpVersionIniPath, NULL, NULL) )
		{
			APP_EVENT("Download VersionIni File Failed. ");
			return FALSE;
		}
		APP_EVENT("Download VersionIni File Succeed. ");
	}
	
	//更新UaDetails.xml
	::DeleteUrlCacheEntry(UA_XML_URL);
	if(S_OK != ::URLDownloadToFile(NULL, UA_XML_URL, lpUaXmlPath, NULL, NULL) )
	{
		APP_EVENT("Download UaDetails.xml File Failed. ");
		return FALSE;
	}
	APP_EVENT("Download UaDetails.xml File Succeed. ");
	return TRUE;
}

void PlatformUA::ConvertToWidth(ClickData& tmpClickData, const char* szWidth)
{
	if (!szWidth)
		return ;
	
	char buf[MAX_PATH] = {0};
	char* p = NULL;
	strcpy(buf, szWidth);
	
	p = strchr(buf, '|');
	
	if ( p == NULL)
	{
		tmpClickData.uaMinWidth = atoi(buf);
		tmpClickData.uaMaxWidth = atoi(buf);
	}
	else
	{
		tmpClickData.uaMinWidth = atoi(buf);
		tmpClickData.uaMaxWidth = atoi(p+1);
	}	
}

void PlatformUA::ConvertToHeight(ClickData& tmpClickData, const char* szHeight)
{
	if (!szHeight)
		return ;
	
	char buf[MAX_PATH] = {0};
	char* p = NULL;
	strcpy(buf, szHeight);
	
	p = strchr(buf, '|');
	
	if ( p == NULL)
	{
		tmpClickData.uaMinHeight = atoi(buf);
		tmpClickData.uaMaxHeight = atoi(buf);
	}
	else
	{
		tmpClickData.uaMinHeight = atoi(buf);
		tmpClickData.uaMaxHeight = atoi(p+1);
	}	
}

//加载UaDetails_s.xml
bool PlatformUA::LoadPlatformUa(LPCTSTR lpFilePath)
{
	TypeOfUA xmlUA;
	
    char szFilePath[MAX_PATH] = {0};
	TipsUtil::TCHARtoCHAR(lpFilePath, szFilePath);
	
	// 读取加密文件
	ifstream ifs;
	ifs.open(szFilePath);
	ostringstream oss;
	char ch;
	while(oss && ifs.get(ch)) oss.put(ch);
	string encryption_xml_string = oss.str();
	ifs.close();
	
	// 解密文件
	SynacastUrlEncoding encoding("19801219");
	string decryption_xml_string = encoding.Decode(encryption_xml_string);
//	cout<<decryption_xml_string.c_str()<<endl;

	// 加载xml
	TiXmlDocument* myDocument = new TiXmlDocument();
	myDocument->Parse(decryption_xml_string.c_str());
	if (myDocument->Error()) 
		return false;
	
	TiXmlElement* rootElement = myDocument->RootElement();
	if (!rootElement) return false;
	
	TiXmlElement* deviceElement = rootElement->FirstChildElement();
	while (deviceElement)
	{
		char* pDevicetype = (char*)MakeToString(deviceElement->Value());
		string strDeviceType = _strlwr(pDevicetype);
		
		vector<ClickData> vClickDatas;
		TiXmlElement* uaElement= deviceElement->FirstChildElement("ua");
		while(uaElement)
		{
			int nRatio = MakeToInt(uaElement->Attribute("ratio"));
			if (0 != nRatio)
			{
				ClickData tmpClickData;
				tmpClickData.ratio = nRatio;
				ConvertToWidth(tmpClickData, uaElement->Attribute("width"));
				ConvertToHeight(tmpClickData, uaElement->Attribute("height"));
				tmpClickData.ua = MakeToString(uaElement->GetText());
				vClickDatas.push_back(tmpClickData);
			}
			uaElement = uaElement->NextSiblingElement("ua");
		}
		
		xmlUA.vDevice[strDeviceType] = vClickDatas;
		
		deviceElement = deviceElement->NextSiblingElement();
	}
	
	SetUaInfo(xmlUA);

	return true;
}

//获取替换的UserAgent
string PlatformUA::GetNewUA(string strType)
{
	TypeOfUA tUAInfo;
	GetUaInfo(tUAInfo);	

	char* pType = _strlwr((char*)strType.c_str());
	map<string, vector<ClickData> >::iterator mit;	
	mit = tUAInfo.vDevice.find(pType);
	if (mit != tUAInfo.vDevice.end())
	{
		vector<ClickData>::iterator iter;		
		int total = 0;
		for(iter = mit->second.begin(); iter != mit->second.end(); iter++)
			total += (*iter).ratio;
		if (total <= 0)return "";
		
		srand(GetTickCount());
		int nRandom = rand()%total;
		int nIncrease = 0;	
		for (iter = mit->second.begin(); iter != mit->second.end(); iter++)
		{
			if (nRandom >= nIncrease && nRandom < nIncrease+(*iter).ratio )
			{
				uaMinWidth = (*iter).uaMinWidth;
				uaMaxWidth = (*iter).uaMaxWidth;
				uaMinHeight = (*iter).uaMinHeight;
				uaMaxHeight = (*iter).uaMaxHeight;
				return (*iter).ua;
			}
			nIncrease += (*iter).ratio;
		}	
	}
	
	return "";
}




