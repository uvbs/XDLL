
#pragma once

#include <ppl/net/inet.h>

#include <ppl/config.h>


#if defined(_PPL_PLATFORM_MSWIN)

#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif

#pragma comment(lib, "ws2_32.lib")

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


struct MIB_IPADDRTABLE_DATA : public MIB_IPADDRTABLE
{
	char Padding[10 * 1024];
};

#else

#endif




/// 获取本机IP(如有多个，取第一个)
u_long GetLocalIP();

inline u_long GetLocalIP()
{
	char hostname[513] = { 0 };
	int res = ::gethostname(hostname, 512);
	assert(res == 0);
	return ResolveHostName(hostname);
}


inline size_t LoadIPs(size_t maxCount, u_long ipArray[], const hostent& host)
{
	size_t count = 0;
	for (int index = 0; host.h_addr_list[index] != NULL; index++)
	{
		if (count >= maxCount)
			break;
		const in_addr* addr = reinterpret_cast<const in_addr*>(host.h_addr_list[index]);
		ipArray[count++] = addr->s_addr;
	}
	for (size_t i = 0; i< count; i++)
	{
		for (size_t j = i; j< count; j++)
		{
			if (ipArray[i] < ipArray[j])
			{
				std::swap(ipArray[i], ipArray[j]);
			}
		}
	}
	return count;
}

inline size_t LoadLocalIPs(size_t maxCount, u_long ipArray[])
{
	char hostName[256] = { 0 };	// 存储主机名
	if (gethostname(hostName, 255) != 0)		//正确的情况
	{
		UTIL_ERROR("CIPTable::LoadLocal: gethostname failed, ErrorCode=" << SocketError::LastError());
		return 0;
	}
	UTIL_ERROR("CIPTable::LoadLocal: gethostname=" << hostName);
	struct hostent* host = gethostbyname(hostName);
	if (host == NULL)
	{
		UTIL_ERROR("CIPTable::LoadLocal: gethostbyname(" << hostName << ") failed, ErrorCode=" << SocketError::LastError());
		return 0;
	}
	size_t count = LoadIPs(maxCount, ipArray, *host);
	if (count == 0)
	{
		UTIL_ERROR("CIPTable::LoadLocal: No hostent found.");
		return 0;
	}
	return count;
}

inline void LoadLocalIPs(vector<u_long>& ipArray)
{
	const int max_count = 32;
	ipArray.clear();
	ipArray.resize(max_count);
	size_t count = LoadLocalIPs(max_count, &ipArray[0]);
	assert(count <= max_count);
	if (count < max_count)
	{
		ipArray.resize(count);
	}
}


#ifdef _PPL_PLATFORM_MSWIN

class IPArrayLoader
{
public:

	static UINT LoadLocalIPsFromIPTable(DWORD ipArray[], size_t maxIPCount)
	{
		ULONG bufsize = sizeof(MIB_IPADDRTABLE_DATA);
		MIB_IPADDRTABLE_DATA buf;
		memset(&buf, 0, sizeof(MIB_IPADDRTABLE_DATA));
		DWORD errcode = GetIpAddrTable(&buf, &bufsize, FALSE);
		if ( 0 != errcode )
		{
			//printf("GetIpAddrTable failed %d\n", errcode);
			return 0;
		}
		//printf("GetIpAddrTable ok. %d\n", buf.dwNumEntries);

		//if (InitWinsock() < 0)
		//	return -2;

		if (bufsize > sizeof(MIB_IPADDRTABLE_DATA))
		{
			//printf("invalid bufsize %d\n", bufsize);
			return 0;
		}

		UINT ipCount = min(maxIPCount, buf.dwNumEntries);
		UINT realIPCount = 0;
		for ( DWORD index = 0; index < ipCount; ++index )
		{
			if (realIPCount >= maxIPCount)
				break;
			const MIB_IPADDRROW& iprow = buf.table[index];
			if (iprow.dwAddr != 0 && iprow.dwAddr != 0x0100007f)
			{
				ipArray[realIPCount++] = iprow.dwAddr;
			}
		}
		return realIPCount;
	}


	static UINT LoadLocalIPsFromHostEntry(DWORD ipArray[], const hostent& host, size_t maxIPCount)
	{
		UINT count = 0;
		for (int index = 0; host.h_addr_list[index] != NULL; index++)
		{
			if (count >= maxIPCount)
				break;
			const in_addr* addr = reinterpret_cast<const in_addr*>(host.h_addr_list[index]);
			ipArray[count++] = addr->s_addr;
		}
		//for (int i = 0; i< count; i++)
		//{
		//	for (int j = i; j< count; j++)
		//	{
		//		if (ipArray[i] < ipArray[j])
		//		{
		//			std::swap(ipArray[i], ipArray[j]);
		//		}
		//	}
		//}
		return count;
	}

	static UINT DoLoadLocalIPs(DWORD ipArray[], size_t maxIPCount)
	{
		char hostName[128];	// 存储主机名
		if (gethostname(hostName, 128) != 0)		//正确的情况
		{
			APP_ERROR("CIPTable::LoadLocal: gethostname failed, ErrorCode=" << ::WSAGetLastError());
			return 0;
		}
		APP_ERROR("CIPTable::LoadLocal: gethostname=" << hostName);
		struct hostent* host = gethostbyname(hostName);
		if (host == NULL)
		{
			APP_ERROR("CIPTable::LoadLocal: gethostbyname(" << hostName << ") failed, ErrorCode=" << ::WSAGetLastError());
			return 0;
		}
		UINT count = LoadLocalIPsFromHostEntry(ipArray, *host, maxIPCount);
		if (count == 0)
		{
			APP_ERROR("CIPTable::LoadLocal: No hostent found.");
			return 0;
		}
		return count;
	}


	static UINT LoadLocalIPs(DWORD ipArray[], size_t maxIPCount)
	{
		UINT count = DoLoadLocalIPs(ipArray, maxIPCount);
		if (count > 0)
			return count;
		assert(false);
		count = LoadLocalIPsFromIPTable(ipArray, maxIPCount);
		if (count == 0)
		{
			APP_ERROR("CIPTable::LoadLocal: GetIpAddrTable failed.");
			assert(false);
			return 0;
		}
		return count;
	}

	static DWORD GetFirstLocalIP()
	{
		DWORD ipArray[64 + 1] = { 0 };
		if ( 0 == LoadLocalIPs(ipArray, 64) )
		{
			assert( false );
			return 0;
		}
		return ipArray[0];
	}
};

#elif defined(_PPL_PLATFORM_LINUX)

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
//#include <net/if.h>
#include <ifaddrs.h>
#include <errno.h>


class IPArrayLoader
{
public:
	static size_t LoadLocalIPs(u_long ipArray[], size_t maxIPCount)
	{
		struct ifaddrs *interfaceArray = NULL, *tempIfAddr = NULL;
		void *tempAddrPtr = NULL;
		int rc = 0;

		rc = getifaddrs(&interfaceArray);  /* retrieve the current interfaces */
		if (rc != 0)
		{
			//printf("getifaddrs() failed with errno =  %d %s \n",errno, strerror(errno));
			return 0;
		}
		size_t ipIndex = 0;
		for(tempIfAddr = interfaceArray; tempIfAddr != NULL; tempIfAddr = tempIfAddr->ifa_next)
		{
			if (string(tempIfAddr->ifa_name) == "lo")
			{
				continue;
			}
			if ( NULL == tempIfAddr->ifa_addr )
			{
				continue;
			}
			if ( AF_INET != tempIfAddr->ifa_addr->sa_family )
			{
				continue;
			}
			if ( ipIndex >= maxIPCount )
				break;
			const sockaddr_in& sa = *(sockaddr_in*)tempIfAddr->ifa_addr;
			ipArray[ipIndex++] = sa.sin_addr.s_addr;
		}

		return ipIndex;
	}

	static u_long GetFirstLocalIP()
	{
		u_long ipArray[10] = { 0 };
		if ( 0 == LoadLocalIPs( ipArray, 9 ) )
			return 0;
		return ipArray[0];
	}
};


#endif
