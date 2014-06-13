
#pragma once


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IPTABLE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IPTABLE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef IPTABLE_EXPORTS
#define IPTABLE_API __declspec(dllexport)
#else
#define IPTABLE_API __declspec(dllimport)
#endif

#include <string>
using namespace std;
//#include "../../base/util/inttypes.h"
//#include "../../base/util/macro.h"
//#include "../../base/protocol/types.h"

#pragma pack(push, 1)
/// ip地址对应的网络类型
/// 网络类型
struct NET_TYPE
{
	/// 国家或地区
	UINT16 Country;

	/// 互联网服务提供商(telecom/cnc/edu...)
	UINT16 ISP;

	/// 省
	UINT16 Province;

	/// 市/县
	UINT16 City;

	NET_TYPE() : Country(0), ISP(0), Province(0), City(0) {}
	NET_TYPE(UINT16 _Country, UINT16 _Province, UINT16 _City) : Country(_Country), ISP(0), Province(_Province), City(_City) {}

	/// 清空
	void Clear()
	{
		this->Country = 0;
		this->ISP = 0;
		this->Province = 0;
		this->City = 0;
	}

	/// 转化为整数
	UINT32 ToInteger() const
	{
		assert(Province < 50);
		assert(City < 200);
		assert(City < 500);
		return ISP * 100 * 1000 + Province * 1000 + City;
	}
};

#pragma pack(pop)

extern "C" IPTABLE_API int Init();

extern "C" IPTABLE_API void Locate(UINT ip, WORD* p_country, WORD* p_isp, WORD* p_province, WORD* p_city);

#ifdef _DEBUG
extern "C" void CALLBACK RunTests(
  HWND hwnd,        // handle to owner window
  HINSTANCE hinst,  // instance handle for the DLL
  LPTSTR lpCmdLine, // string the DLL will parse
  int nCmdShow      // show state
);
#endif


