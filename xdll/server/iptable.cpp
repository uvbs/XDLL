#include "stdafx.h"
#include "iptable.h"

IPTable g_ipTable;

ostream& operator<<(ostream& os, const NET_TYPE& netType)
{
	return os << netType.ISP << ":" << netType.Country << ":" << netType.Province << ":" << netType.City;
}

bool operator==(const NET_TYPE& val1, const NET_TYPE& val2)
{
	return val1.Country == val2.Country && val1.ISP == val2.ISP && 
		val1.Province == val2.Province && val1.City == val2.City;
}

bool Hit(const NET_TYPE& val1, const NET_TYPE& val2)
{
	bool ret = 
		(val1.Country==val2.Country || val2.Country==0) &&
		(val1.Province==val2.Province || val2.Province==0) &&
		(val1.City==val2.City || val2.City==0);
	return ret;
}