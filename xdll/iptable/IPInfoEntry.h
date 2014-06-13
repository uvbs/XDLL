
#pragma once


/// ip表中的一项
struct CITY_IP_INFO
{
	/// 起始ip
	UINT32 StartIP;

	/// 结束ip
	UINT32 EndIP;

	/// 所属isp，或者是教育网的某个地址
	WORD ISP;

	/// 所在省
	WORD Province;

	/// 所在城市
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


