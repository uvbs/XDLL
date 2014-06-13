
#pragma once


/**
 * @file
 * @brief tcp/ip中的工具类
 */


#include <ppl/config.h>
#include <ppl/std/tchar.h>
#include <ppl/util/macro.h>
#include <ppl/stl/numeric.h>
#include <ppl/util/log.h>
#include <ppl/text/conv.h>
#include <ppl/stl/string.h>
#include <assert.h>
#include <stddef.h>


#include <vector>
using std::vector;

#include <iosfwd>
using std::ostream;


#if defined(_PPL_PLATFORM_MSWIN)

#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif

#pragma comment(lib, "ws2_32.lib")

#else

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

inline int WSAGetLastError()
{
	return errno;
}

#endif



#ifdef NEED_LOG
#include <iosfwd>
using std::ostream;

#endif



class IPAddress;
class SocketAddress;
class InetSocketAddress;




#if defined(_PPL_PLATFORM_MSWIN)

class WinsockLib
{
public:
	WinsockLib()
	{
		WSADATA wsaData = { 0 };
		m_errcode = ::WSAStartup(MAKEWORD(1, 2), &wsaData);
	}
	~WinsockLib()
	{
		::WSACleanup();
	}

private:
	int m_errcode;
};

class SocketError
{
public:
	static int LastError()
	{
		return ::WSAGetLastError();
	}
};

#else

class SocketError
{
public:
	static int LastError()
	{
		return errno;
	}
};


#endif



/// 将ip地址格式化为字符串
tstring FormatIPAddress(u_long ip);

/// 解析域名
u_long ResolveHostName(const char* hostname);

/// 将socket地址格式化为字符串
string FormatSocketAddress(const sockaddr_in& addr);


struct win32_in_addr {
        union {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
};


/// 将ip地址格式化为字符串
inline int FormatIPAddress(u_long ip, char* buffer, size_t size)
{
	const win32_in_addr* addr = reinterpret_cast<const win32_in_addr*>(&ip);
	return snprintf(buffer, size, "%u.%u.%u.%u", addr->S_un.S_un_b.s_b1, addr->S_un.S_un_b.s_b2, addr->S_un.S_un_b.s_b3, addr->S_un.S_un_b.s_b4);
}

#if defined(_PPL_PLATFORM_MSWIN)

/// 将ip地址格式化为字符串
inline int FormatIPAddress(u_long ip, wchar_t* buffer, size_t size)
{
	const win32_in_addr* addr = reinterpret_cast<const win32_in_addr*>(&ip);
	return _snwprintf(buffer, size, L"%u.%u.%u.%u", addr->S_un.S_un_b.s_b1, addr->S_un.S_un_b.s_b2, addr->S_un.S_un_b.s_b3, addr->S_un.S_un_b.s_b4);
}

#endif



/// 从hostent结构体中加载ip列表
size_t LoadIPs(size_t maxCount, u_long ipArray[], const hostent& host);

/// 加载本地ip列表
size_t LoadLocalIPs(size_t maxCount, u_long ipArray[]);

/// 加载本地ip列表
void LoadLocalIPs(vector<u_long>& ipArray);



/// 翻转ip的字节序
u_long TransIP(u_long ip);

/// 判断一个IP是否为保留地址（内网地址）
bool IsPrivateIP(u_long ip);

/// 判断一组IP是否包含非保留地址（外网地址）
bool HasExternalIP(const vector<u_long>& ipArray);

/// 判断一组IP是否包含非保留地址（外网地址）
u_long FindExternalIP(const vector<u_long>& ipArray);

/// 寻找匹配的IP地址
u_long FindRightIP(const vector<u_long>& ipArray, u_long ip);

/// 寻找外网地址，如果没有找到，返回0
u_long FindRealExternalIP(const vector<u_long>& ipArray);


/// 判断一组IP是否包含非保留地址（外网地址）
bool HasExternalIP(const vector<UINT32>& ipArray);

/// 判断一组IP是否包含非保留地址（外网地址）
u_long FindExternalIP(const vector<UINT32>& ipArray);

/// 寻找匹配的IP地址
u_long FindRightIP(const vector<UINT32>& ipArray, u_long ip);

/// 寻找外网地址，如果没有找到，返回0
u_long FindRealExternalIP(const vector<UINT32>& ipArray);


ostream& operator<<(ostream& os, const sockaddr_in& addr);

ostream& operator<<(ostream& os, const in_addr& addr);

ostream& operator<<(ostream& os, const IPAddress& addr);

ostream& operator<<(ostream& os, const SocketAddress& addr);


inline bool CheckBitsValid( UINT val, UINT bits )
{
	UINT bitsVal = val & bits;
	return bitsVal != 0 && bitsVal != bits;
}

/// 169.254.x.x(dhcp失败的)
inline bool CheckIPIsDHCPFailed( UINT ip )
{
	const UINT ip_mask = 0x0000FEA9;
	return (ip & ip_mask) == ip_mask;
}

inline bool CheckIPValid( UINT ip )
{
	return CheckBitsValid( ip, 0x000000FF );
}


inline bool CheckIPFullyValid( UINT ip )
{
	return CheckIPValid( ip ) && false == CheckIPIsDHCPFailed( ip );
}


#pragma pack(push, 1)

struct IP_BYTES
{
	UINT8 Byte1;
	UINT8 Byte2;
	UINT8 Byte3;
	UINT8 Byte4;
};
#pragma pack(pop)



/// IP地址类
class IPAddress : public in_addr
{
public:
	IPAddress(u_long ip = INADDR_ANY)
	{
		this->s_addr = ip;
	}

	string ToAnsiString() const
	{
		return ::inet_ntoa(*this);
	}

	tstring ToString() const
	{
		const size_t max_buffer_size = 64;
		TCHAR buf[max_buffer_size + 1] = { 0 };
		::FormatIPAddress( this->s_addr, buf, max_buffer_size );
		return tstring(buf);
	}
	u_long GetAddress() const
	{
		return this->s_addr;
	}

	operator u_long() const
	{
		return GetAddress();
	}
};


/// socket地址的接口类
class SocketAddress
{
public:
	SocketAddress() { }
	virtual ~SocketAddress() { }

	/// 获取通用的sockaddr指针(只读)
	virtual const sockaddr* GetAddress() const = 0;

	/// 获取通用的sockaddr指针(可写)
	virtual sockaddr* GetAddress() = 0;

	/// 获取socket地址数据的长度
	virtual int GetAddressLength() const = 0;

	/// 转化为字符串
	virtual string ToString() const = 0;
};




/// ipv4中使用的Socket地址类 GetIP，GetPort返回的是host-byteorder－－遵照程序中的使用习惯
class InetSocketAddress : public SocketAddress
{
public:
	/// 复制对象
	InetSocketAddress(const sockaddr_in& addr) : m_addr(addr) { }

	/// 根据ip/port构造对象，ip为网络字节序，port为本机字节序
	explicit InetSocketAddress(u_long ip = 0, u_short port = 0)
	{
		m_addr.sin_family = AF_INET;
		FILL_ZERO(m_addr.sin_zero);
		this->Set(ip, port);
	}

	virtual const sockaddr* GetAddress() const { return GetRawSocketAddress(); }
	virtual sockaddr* GetAddress() { return GetRawSocketAddress(); }
	virtual int GetAddressLength() const { return sizeof(sockaddr_in); }
	virtual string ToString() const;

	bool Parse( const string& s )
	{
		size_t pos = s.find( ':' );
		if ( pos == string::npos )
			return false;
		u_short port = 0;
		if ( false == numeric<u_short>::try_parse( s.substr( pos + 1 ), port ) )
			return false;
		this->SetPort( port );
		this->SetIP( s.substr( 0, pos ).c_str() );
		if ( INADDR_NONE == this->GetRawIP() )
			return false;
		return true;
	}


	/// 设置ip地址和端口号
	void Set(u_long ip, u_short port)
	{
		SetIP(ip);
		SetPort(port);
	}

	/// 获取IP地址，返回的是本地字节序
	u_long GetIPHostByteOrder() const
	{
		return ntohl(this->GetRawIP());
	}


	/// 获取原始的IP地址，为network-byteorder
	u_long GetRawIP() const { return m_addr.sin_addr.s_addr; }

	/// 获取原始的端口号，为network-byteorder
	u_short GetRawPort() const { return m_addr.sin_port; }

	/// 获取IP地址，返回的是network-byteorder－－遵照程序中和一般的使用习惯
	u_long GetIP() const { return GetRawIP(); }

	/// 获取端口号，返回的是host-byteorder－－遵照程序中和一般的使用习惯
	u_short GetPort() const { return ntohs(GetRawPort()); }

	/// 设置端口号，port为主机字节序的端口值
	void SetPort(u_short port) { m_addr.sin_port = htons(port); }

	/// 设置IP，ip为网络字节序
	void SetIP(u_long ip)
	{
//		assert(ip != INADDR_NONE);
		m_addr.sin_addr.s_addr = ip;
	}

	/// 设置IP，参数为ip字符串
	void SetIP(const char* ip)
	{
		assert(ip != NULL);
		m_addr.sin_addr.s_addr = inet_addr(ip);
	}

	/// 获取原始地址数据
	const sockaddr_in& GetRawAddress() const { return m_addr; }

	/// 获取sockaddr对象
	const sockaddr* GetRawSocketAddress() const { return reinterpret_cast<const sockaddr*>(&m_addr); }

	/// 获取sockaddr对象
	sockaddr* GetRawSocketAddress() { return reinterpret_cast<sockaddr*>(&m_addr); }

private:
	/// 原始的地址数据
	sockaddr_in m_addr;
};

/*
inline bool operator==(const InetSocketAddressBase& x, const InetSocketAddressBase& y)
{
	return x.GetRawIP() == y.GetRawIP() && x.GetRawPort() == y.GetRawPort();
}
*/


inline bool operator==(const InetSocketAddress& x, const InetSocketAddress& y)
{
	return x.GetRawIP() == y.GetRawIP() && x.GetRawPort() == y.GetRawPort();
}

inline bool operator!=(const InetSocketAddress& x, const InetSocketAddress& y)
{
	return !(x == y);
}







inline u_long TransIP(u_long ip)
{
	win32_in_addr a;
	a.S_un.S_addr=ip;
	return   (( (u_long)a.S_un.S_un_b.s_b1 ) << 24) +
			 (( (u_long)a.S_un.S_un_b.s_b2 ) << 16) +
			 (( (u_long)a.S_un.S_un_b.s_b3 ) << 8)  +
			 ( (u_long)a.S_un.S_un_b.s_b4 ) ;
}

inline bool IsPrivateIP(u_long ip)
{
	win32_in_addr a;
	a.S_un.S_addr=ip;

	u_char b1 = a.S_un.S_un_b.s_b1;
	u_char b2 = a.S_un.S_un_b.s_b2;

	//10.x.x.x
	if (b1==10) return true;

	//172.16.x.x -- 172.31.x.x
	if ( (b1==172) && (16<=b2) && (b2<=31)  ) return true;

	//192.168.x.x
	if ( (b1==192) && (b2==168) ) return true;

	//169.254.x.x
	if ( (b1==169) && (b2==254) ) return true;

	return false;
}


inline bool HasExternalIP(const vector<u_long>& ipArray)
{
	for (size_t i = 0; i < ipArray.size(); ++i)
	{
		u_long ip = ipArray[i];
		if (!IsPrivateIP(ip))
			return true;
	}
	return false;
}

inline u_long FindRealExternalIP(const vector<u_long>& ipArray)
{
	for (size_t i = 0; i < ipArray.size(); ++i)
	{
		u_long ip = ipArray[i];
		if (!IsPrivateIP(ip))
			return ip;
	}
	return 0;
}

inline u_long FindExternalIP(const vector<u_long>& ipArray)
{
	for (size_t i = 0; i < ipArray.size(); ++i)
	{
		u_long ip = ipArray[i];
		if (!IsPrivateIP(ip))
			return ip;
	}
	if (ipArray.empty())
		return 0;
	return ipArray[0];
}

inline u_long FindRightIP(const vector<u_long>& ipArray, u_long ip)
{
	if ( ipArray.empty() )
	{
		assert( false );
		return 0;
	}
	u_long i;
	u_long min_value,min_index,value;

	min_value= TransIP(ip) ^ TransIP(ipArray[0]);
	min_index= 0;

	for (i=1; i<ipArray.size(); i++)
	{
		value= TransIP(ip) ^ TransIP(ipArray[i]);
		if (value < min_value)
		{
			min_value= value;
			min_index= i;
		}
	}

	return ipArray[min_index];
}



inline bool HasExternalIP(const vector<UINT32>& ipArray)
{
	return HasExternalIP(reinterpret_cast<const vector<u_long>&>(ipArray));
}

inline u_long FindExternalIP(const vector<UINT32>& ipArray)
{
	return FindExternalIP(reinterpret_cast<const vector<u_long>&>(ipArray));
}

inline u_long FindRightIP(const vector<UINT32>& ipArray, u_long ip)
{
	return FindRightIP(reinterpret_cast<const vector<u_long>&>(ipArray), ip);
}

/// 寻找外网地址，如果没有找到，返回0
inline u_long FindRealExternalIP(const vector<UINT32>& ipArray)
{
	return FindRealExternalIP(reinterpret_cast<const vector<u_long>&>(ipArray));
}




inline tstring FormatIPAddress(u_long ip)
{
	IPAddress ipaddr(ip);
	return ipaddr.ToString();
}

inline string AnsiFormatIPAddress(u_long ip)
{
	IPAddress ipaddr(ip);
	return ipaddr.ToAnsiString();
}

inline u_long ResolveHostName(const char* hostname)
{
	u_long ip = ::inet_addr(hostname);
	if (ip != INADDR_NONE)
		return ip;
	struct hostent* host = ::gethostbyname(hostname);
	if (host == NULL)
	{
		UTIL_ERROR("ResolveHostName: gethostbyname(" << hostname << ") failed, ErrorCode=" << SocketError::LastError());
		return INADDR_NONE;
	}
	const in_addr* addr = reinterpret_cast<const in_addr*>(host->h_addr_list[0]);
	return addr->s_addr;
}

inline u_long ResolveHostName(const string& hostname)
{
	return ResolveHostName(hostname.c_str());
}


#if defined(_PPL_PLATFORM_MSWIN)

inline u_long ResolveHostName(const wstring& hostname)
{
#if _MSC_VER < 1400
	USES_CONVERSION;
#endif
	return ResolveHostName(CW2A(hostname.c_str()));
}

#endif

inline string FormatSocketAddress(const sockaddr_in& addr)
{
	const int buffer_size = 255;
	char str[buffer_size + 1] = { 0 };
	snprintf(str, buffer_size, "%s:%hu", ::inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	return str;
}



inline ostream& operator<<(ostream& os, const sockaddr_in& addr)
{
	os << FormatSocketAddress(addr);
	return os;
}

inline ostream& operator<<(ostream& os, const in_addr& addr)
{
	os << ::inet_ntoa(addr);
	return os;
}



inline ostream& operator<<(ostream& os, const IPAddress& addr)
{
	const in_addr& rawAddr = addr;
	os << rawAddr;
	return os;
}

inline ostream& operator<<(ostream& os, const SocketAddress& addr)
{
	os << addr.ToString();
	return os;
}



inline string InetSocketAddress::ToString() const
{
	return FormatSocketAddress(GetRawAddress());
}


#ifdef _RUN_TEST


class InetTestCase : public TestCase
{
	virtual void DoRun()
	{
		u_long ip = inet_addr("192.168.0.1");
		u_long ip1 = inet_addr("192.168.0.113");
		u_long ip2 = inet_addr("202.168.0.1");
		vector<u_long> ipArray;
		ipArray.push_back(inet_addr("192.168.0.113"));
		ipArray.push_back(inet_addr("202.168.0.1"));

		assert(FindRightIP(ipArray, ip) == ip1);

		ip = inet_addr("58.19.11.33");

		ip = inet_addr("192.168.0.3");
		assert( IsPrivateIP( ip ) );
		ip = inet_addr("169.254.0.3");
		assert( IsPrivateIP( ip ) );
		ip = inet_addr("10.0.0.3");
		assert( IsPrivateIP( ip ) );
		ip = inet_addr("172.16.0.3");
		assert( IsPrivateIP( ip ) );
		ip = inet_addr("172.31.0.3");
		assert( IsPrivateIP( ip ) );
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(InetTestCase);

#endif

