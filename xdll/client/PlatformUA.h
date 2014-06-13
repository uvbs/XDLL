//#pragma once

#ifndef _PLATFORMUA_H_
#define _PLATFORMUA_H_

#include <string>
#include <vector>
#include <map>
#include <ShlObj.h>
#include <Shlwapi.h>
#include "tinyxml.h"
using namespace std;

struct ClickData
{
	int ratio;
	int uaMinWidth;
	int uaMaxWidth;
	int uaMinHeight;
	int uaMaxHeight; 
	string ua;
};

class TypeOfUA
{
public:
	TypeOfUA(){}
	~TypeOfUA(){}
	TypeOfUA(TypeOfUA &ua);
	TypeOfUA& operator=(TypeOfUA& ua);

public:
	map<string, vector<ClickData> > vDevice;
};


class PlatformUA
{
private:
	static PlatformUA* m_Instance;
	PlatformUA(){}
	~PlatformUA(){}

public:
	static PlatformUA* GetInstance();
	static void DeleteInstance();
	static int uaMinWidth;
	static int uaMaxWidth;
	static int uaMinHeight;
	static int uaMaxHeight;

public:
	TypeOfUA m_ua;
	void DefaultUaInfo();
	void SetUaInfo(TypeOfUA&);
	void GetUaInfo(TypeOfUA&);
	BOOL UpdateUaXml(LPCTSTR lpVersionIniPath, LPCTSTR lpUaXmlPath);	//�������µ�UaDetails.xml�ļ�
	void ConvertToWidth(ClickData& tmpClickData, const char* szWidth);
	void ConvertToHeight(ClickData& tmpClickData, const char* szHeight);
	bool LoadPlatformUa(LPCTSTR lpFilePath);							//����UaDetails.xml
	string GetNewUA(string strType);									//��ȡ�滻��UserAgent
};

#endif



