// iptable.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "iptable.h"
#include <ppl/mswin/Module.h>
#include <ppl/io/fs.h>

//#ifdef _DEBUG
//#pragma comment(linker, "/EXPORT:RunTests=_RunTests@16")
//#endif

vector<string> splite(const string& input, const string& spliter)
{
	vector<string> ret_strs;;
	string s = input;
	size_t spliter_length = spliter.size();
	while(true)
	{
		size_t pos = s.find(spliter);
		if( pos == string::npos )
		{
			ret_strs.push_back(s);
			break;
		}
		ret_strs.push_back(s.substr(0, pos));
		pos += spliter_length;
		s = s.substr(pos);
	}
	return ret_strs;
}


/// ip表，用来查询ip所在的区域和isp
class IPTable
{
public:
	IPTable() { }

	/// 初始化
	int Init();

	/// 查找ip，如果没有找到，则返回的结构体中字段都为0
	void Find(UINT ip, WORD* p_country, WORD* p_isp, WORD* p_province, WORD* p_city) const;

private:
	map<u_int, NET_TYPE> ip_map;
};

int IPTable::Init()
{
	string tpi_config_file = Module().GetFileDirectory() + _T("\\tpi_config.txt");

	file_status fs;
	if( false == fs.retrieve(tpi_config_file) )
	{
		printf("tpi_config.txt is not existed\r\n");
		return 0;
	}

	std::ifstream ifs(tpi_config_file.c_str());
	string line;

	// 添加和更新任务
	while( std::getline(ifs, line) )
	{
		vector<string> lines = splite(line, "\t");
		if(lines.size() < 5 )
			continue;
		u_int begin_ip, end_ip;
		WORD country_id, province_id, city_id;
		try { 
			begin_ip = boost::lexical_cast<u_int>(lines[0]); 
			end_ip = boost::lexical_cast<u_int>(lines[1]); 
			country_id = boost::lexical_cast<WORD>(lines[2]);
			province_id = boost::lexical_cast<WORD>(lines[3]);
			city_id = boost::lexical_cast<WORD>(lines[4]);

			ip_map[end_ip] = NET_TYPE(country_id, province_id, city_id);
			//printf("%d %d %d %d %d\n", begin_ip, end_ip, country_id, province_id, city_id);
		} 
		catch (...) 
		{ 
			printf("Read %s error\n", line);
			continue;
		}
	}

	ifs.close();

	printf("All IP Segment Count %d\r\n", ip_map.size() );

	return 1;
}


inline void IPTable::Find(UINT netIP, WORD* p_country, WORD* p_isp, WORD* p_province, WORD* p_city) const
{
	UINT hostIP = ntohl(netIP);
	//assert(hostIP != netIP || hostIP == 0);

	map<u_int, NET_TYPE>::const_iterator iter1 = ip_map.lower_bound(hostIP);
	if (iter1 != ip_map.end())
	{
		const NET_TYPE& ip_net_type = iter1->second;
		*p_country = ip_net_type.Country;
		*p_isp = ip_net_type.ISP;
		*p_province = ip_net_type.Province;
		*p_city = ip_net_type.City;
	}
	else
	{
		*p_country = 0;
		*p_isp = 0;
		*p_province = 0;
		*p_city = 0;
	}
}

static IPTable s_ipTable;

IPTABLE_API int Init()
{
	return s_ipTable.Init();
}


IPTABLE_API void Locate(UINT ip, WORD* p_country, WORD* p_isp, WORD* p_province, WORD* p_city)
{
	s_ipTable.Find(ip, p_country, p_isp, p_province, p_city);
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}





