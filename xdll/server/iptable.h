
#pragma once

#include <ppl/os/module.h>
#include <ppl/io/path.h>

/// ��������
#pragma pack(push, 1)
struct NET_TYPE
{
	/// ���һ����
	UINT16 Country;

	/// �����������ṩ��(telecom/cnc/edu...)
	UINT16 ISP;

	/// ʡ
	UINT16 Province;

	/// ��/��
	UINT16 City;

	/// ���
	void Clear()
	{
		this->Country = 0;
		this->ISP = 0;
		this->Province = 0;
		this->City = 0;
	}

	/// ת��Ϊ����
	UINT32 ToInteger() const
	{
		assert(Province < 50);
		assert(City < 200);
		assert(City < 500);
		return ISP * 100 * 1000 + Province * 1000 + City;
	}
};
#pragma pack(pop)

ostream& operator<<(ostream& os, const NET_TYPE& netType);
bool operator==(const NET_TYPE& val1, const NET_TYPE& val2);
bool Hit(const NET_TYPE& val1, const NET_TYPE& val2);

class IPTable
{
public:
	typedef int (*FUNC_Init)();
	typedef void (*FUNC_LocateIP)(UINT,WORD*,WORD*,WORD*,WORD*);

	IPTable() : m_funcLocateIP(NULL), m_funcInit(NULL) { }

	bool Load(const tstring& baseDir)
	{
		tstring dllPath = paths::combine( baseDir, _T("tpi.dll") );
		if (m_dll.Load(dllPath.c_str()))
		{
			m_funcLocateIP = reinterpret_cast<FUNC_LocateIP>(m_dll.GetExportItem("Locate"));
			m_funcInit = reinterpret_cast<FUNC_Init>(m_dll.GetExportItem("Init"));
			assert(m_funcLocateIP != NULL);
			return m_funcLocateIP != NULL;
		}
		return false;
	}

	int Init()
	{
		return m_funcInit();
	}

	NET_TYPE LocateIP(UINT ip)
	{
		NET_TYPE netType = { 0, 0, 0, 0 };
		if (m_funcLocateIP != NULL)
		{
			m_funcLocateIP(ip, &netType.Country, &netType.ISP, &netType.Province, &netType.City);
		}
		return netType;
	}

	bool IsValid() const
	{
		return m_dll.IsValid() && m_funcLocateIP != NULL;
	}

private:
	LoadableModule m_dll;
	FUNC_Init m_funcInit;
	FUNC_LocateIP m_funcLocateIP;
};

extern IPTable g_ipTable;

inline UINT GetNetTypeDistanceValue(int x, int y)
{
	if (x == 0 || y == 0)
		return 0;
	return x == y;
}


inline UINT CalcNetTypeDistance(const NET_TYPE& x, const NET_TYPE& y)
{
	UINT isp = GetNetTypeDistanceValue(x.ISP, y.ISP);
	UINT country = GetNetTypeDistanceValue(x.Country, y.Country);
	// �Ƿ���ͬһ���ҵ�ͬһʡ
	UINT province = GetNetTypeDistanceValue(x.Province, y.Province) && (x.Country == y.Country);
	// �Ƿ���ͬһ����ͬһʡ��ͬһ��
	UINT city = GetNetTypeDistanceValue(x.City, y.City) && (x.Country == y.Country) && (x.Province == y.Province);
	// isp��cityͬ����province��city��һ��
	return isp * 4 + city * 4 + province * 2 + country;
}

