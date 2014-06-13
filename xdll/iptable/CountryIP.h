
#pragma once


struct COUNTRY_IP_INFO
{
	UINT StartIP;
	UINT EndIP;
	UINT Country;
};




const size_t COUNTRY_IP_COUNT = 46985;
const size_t COUNTRY_COUNT = 214;

extern const COUNTRY_IP_INFO COUNTRY_IP_LIST[];



#ifdef _DEBUG
extern const char* COUNTRY_ARRAY[];
#endif


