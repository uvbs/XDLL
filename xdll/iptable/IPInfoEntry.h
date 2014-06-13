
#pragma once


/// ip���е�һ��
struct CITY_IP_INFO
{
	/// ��ʼip
	UINT32 StartIP;

	/// ����ip
	UINT32 EndIP;

	/// ����isp�������ǽ�������ĳ����ַ
	WORD ISP;

	/// ����ʡ
	WORD Province;

	/// ���ڳ���
	WORD City;
};



const size_t CITY_IP_COUNT = 17195;
const size_t IP_ISP_COUNT = 320;
const size_t IP_PROVINCE_COUNT = 36;

extern const CITY_IP_INFO CITY_IP_LIST[];

#ifdef _DEBUG
extern const char* ISP_ARRAY[];
extern const char* PROVINCE_ARRAY[];
#endif


