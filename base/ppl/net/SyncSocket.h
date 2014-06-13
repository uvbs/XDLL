
#pragma once

#include <boost/noncopyable.hpp>
#include 

class SocketAddress;


/// 表示socket操作出错
class SocketException : public Exception
{
public:
	explicit SocketException(int errcode) : m_errcode(errcode) { }
	virtual ~SocketException() { }

	/// 获取错误码
	int GetErrorCode() const { return m_errcode; }

private:
	/// socket错误码
	int m_errcode;
};

/// socket已被正常的关闭
class SocketClosedException : public SocketException
{
public:
	SocketClosedException() : SocketException(0) { }
};


/// win32的文件操作类(使用操作系统的接口)
class SyncSocket : private noncopyable
{
public:
	SyncSocket() : m_handle(INVALID_SOCKET)
	{
	}
	~SyncSocket()
	{
		Close();
	}

	/// 连接指定的地址
	void Connect(const SocketAddress& addr);

	/// 连接指定的地址
	void Connect(const char* host, u_short port);

	/// 获取句柄
	SOCKET GetHandle() const { return m_handle; }

	/// 句柄是否有效
	bool IsValid() const { return m_handle != INVALID_SOCKET; }
	/// 句柄是否打开
	bool IsOpen() const { return IsValid(); }

	/// 关闭socket
	void Close();

	/// 接收数据
	size_t Receive(void* buffer, size_t size);

	/// 接收指定长度的数据
	bool ReceiveN(void* buffer, size_t size);

	/// 发送数据
	size_t Send(const void* data, size_t size);
	/// 将指定的数据全部发送出去
	void SendN(const void* data, size_t size);


	/// 设置socket选项信息
	bool SetOption(int level, int opt, const void* data, int size);
	/// 设置整数型的选项信息
	bool SetIntegerOption(int level, int opt, int val);

	/// 设置接收的超时时间
	bool SetReceiveTimeout(int timeout);
	/// 设置发送的超时时间
	bool SetSendTimeout(int timeout);

private:
	/// socket句柄
	SOCKET m_handle;
};




#include "util/inet.h"



/// 检查一般socket操作的结果
inline void CheckSocketResult(int socketResult)
{
	if (socketResult == SOCKET_ERROR)
	{
		int errcode = ::WSAGetLastError();
		UTIL_ERROR("CheckSocketResult error " << make_tuple(socketResult, errcode));
		throw SocketException(errcode);
	}
	assert(socketResult == 0);
}

/// 检查send/recv的结果
inline void CheckSocketIOResult(int socketResult)
{
	if (socketResult > 0)
		return;
	if (socketResult == 0)
	{
		UTIL_ERROR("CheckSocketIOResult socket closed " << socketResult);
		throw SocketClosedException();
	}
	CheckSocketResult(socketResult);
}



inline void SyncSocket::Close()
{
	if (IsOpen())
	{
		int result = ::closesocket(m_handle);
		m_handle = INVALID_SOCKET;
		if (SOCKET_ERROR == result)
		{
			UTIL_ERROR("File::Close CloseHandle failed " << ::WSAGetLastError());
		//	assert(false);
		}
	}
}

inline void SyncSocket::Connect(const SocketAddress& addr)
{
	assert(!IsOpen());
	Close();
	m_handle = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_handle == INVALID_SOCKET)
	{
		int errcode = ::WSAGetLastError();
		UTIL_ERROR("SyncSocket::Connect create socket failed " << errcode);
		assert(false);
		throw SocketException(errcode);
	}
	UTIL_EVENT("SyncSocket::Connect connect to " << addr);
	CheckSocketResult(::connect(m_handle, addr.GetAddress(), addr.GetAddressLength()));
	UTIL_EVENT("SyncSocket::Connect connect succeeded " << addr);
}

inline void SyncSocket::Connect(const char* host, u_short port)
{
	assert(host != NULL && port != 0);
	u_long ip = ResolveHostName(host);
	if (ip == INADDR_NONE)
	{
		int errcode = ::WSAGetLastError();
		UTIL_ERROR("SyncSocket::Connect ResolveHostName " << make_tuple(host, port, errcode));
		throw SocketException(errcode);
	}
	Connect(InetSocketAddress(ip, port));
}

inline size_t SyncSocket::Receive(void* buffer, size_t size)
{
	assert(buffer != NULL && size > 0);
//	assert(IsOpen());
	int bytes = ::recv(m_handle, static_cast<char*>(buffer), size, 0);
	CheckSocketIOResult(bytes);
	assert(bytes > 0 && bytes <= size);
	return bytes;
}

inline bool SyncSocket::ReceiveN(void* buffer, size_t size)
{
	assert(buffer != NULL && size > 0);
//	assert(IsOpen());
	char* buf = static_cast<char*>(buffer);
	size_t totalBytes = 0;
	while (totalBytes < size)
	{
		size_t bytes = Receive(buf + totalBytes, size - totalBytes);
		totalBytes += bytes;
	}
	return true;
}

inline size_t SyncSocket::Send(const void* data, size_t size)
{
	assert(data != NULL && size > 0);
//	assert(IsOpen());
	int bytes = ::send(m_handle, static_cast<const char*>(data), size, 0);
	CheckSocketIOResult(bytes);
	assert (bytes > 0 && bytes <= size);
	return bytes;
}

inline void SyncSocket::SendN(const void* data, size_t size)
{
	assert(data != NULL && size > 0);
//	assert(IsOpen());
	const char* databuf = static_cast<const char*>(data);
	size_t totalBytes = 0;
	while (totalBytes < size)
	{
		size_t bytes = Send(databuf + totalBytes, size - totalBytes);
		totalBytes += bytes;
	}
}

inline bool SyncSocket::SetOption(int level, int opt, const void* data, int size)
{
	assert(data != NULL && size > 0);
	assert(IsOpen());
	if (SOCKET_ERROR == ::setsockopt(m_handle, level, opt, static_cast<const char*>(data), size))
	{
		UTIL_ERROR("SyncSocket::SetOption setsockopt failed " << ::WSAGetLastError());
		return false;
	}
	return true;
}

inline bool SyncSocket::SetIntegerOption(int level, int opt, int val)
{
	return SetOption(level, opt, &val, sizeof(int));
}

inline bool SyncSocket::SetReceiveTimeout(int timeout)
{
	return SetIntegerOption(SOL_SOCKET, SO_RCVTIMEO, timeout);
}

inline bool SyncSocket::SetSendTimeout(int timeout)
{
	return SetIntegerOption(SOL_SOCKET, SO_SNDTIMEO, timeout);
}


/*
#ifdef _PPL_RUN_TEST

class FileTestCase : public TestCase
{
	void DoRun()
	{
		File file;
		file.OpenForRead("f:\\synet.txt");
		char buffer[2045];
		file.Read(buffer, 2044);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(FileTestCase);

#endif
*/

