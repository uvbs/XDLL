
#pragma once

#include <boost/shared_ptr.hpp>
#include <assert.h>
#include <stddef.h>

class SocketAddress;
class InetSocketAddress;

class UDPSocket;
class TCPClientSocket;
class TCPServerSocket;

typedef boost::shared_ptr<TCPClientSocket> TCPClientSocketPtr;


/// udp socket的事件
class UDPSocketListener
{
public:
	virtual ~UDPSocketListener() { }

	virtual void OnSocketReceive(UDPSocket* sender, const InetSocketAddress& addr, unsigned char* data, size_t size) = 0;
	virtual void OnSocketReceiveFailed(UDPSocket* sender, long errcode) = 0;

	virtual void OnSocketSend(UDPSocket* sender) { }	// can be ignored
	virtual void OnSocketSendFailed(UDPSocket* sender, long errcode) { assert(false); }

	virtual void OnSocketClose(UDPSocket* sender, long errcode) { }	// can be ignored
};







/// TCPClientSocket的事件接口
class TCPClientSocketListener
{
public:
	virtual ~TCPClientSocketListener() { }

	/// 连接成功
	virtual void OnSocketConnect(TCPClientSocket* sender) { assert(false); }
	/// 连接失败
	virtual void OnSocketConnectFailed(TCPClientSocket* sender, long errcode) { assert(false); }
	/// socket关闭
	virtual void OnSocketClose(TCPClientSocket* sender, long errcode) { }	// can be ignored

	/// 接收到数据
	virtual void OnSocketReceive(TCPClientSocket* sender, unsigned char* data, size_t size)  = 0;
	/// 接收数据失败
	virtual void OnSocketReceiveFailed(TCPClientSocket* sender, long errcode) { assert(false); }

	/// 发送数据完毕
	virtual void OnSocketSend(TCPClientSocket* sender) { }	// can be ignored
	/// 发送数据失败
	virtual void OnSocketSendFailed(TCPClientSocket* sender, long errcode) { this->OnSocketReceiveFailed(sender, errcode); }
};




/// TCPServerSocket的事件接口
class TCPServerSocketListener
{
public:
	virtual ~TCPServerSocketListener() { }

	/// 接收到新的连接
	virtual void OnSocketAccept(TCPServerSocket* sender, TCPClientSocket* client, const InetSocketAddress& addr) = 0;

	/// 接收新连接失败
	virtual void OnSocketAcceptFailed(TCPServerSocket* sender, long errcode) { }	// can be ignored
};


class TrivialTCPServerSocketListener : public TCPServerSocketListener
{
public:
	virtual void OnSocketAccept(TCPServerSocket* sender, TCPClientSocket* client, const InetSocketAddress& addr)
	{
		assert(false);
	}
};



/// udp socket的事件
class TrivialUDPSocketListener : public UDPSocketListener
{
public:
	virtual void OnSocketReceive(UDPSocket* sender, const InetSocketAddress& addr, unsigned char* data, size_t size) { assert(false); }
	virtual void OnSocketReceiveFailed(UDPSocket* sender, long errcode) { assert(false); }
};

class TrivialTCPClientSocketListener : public TCPClientSocketListener
{
public:
	/// 连接成功
	virtual void OnSocketConnect(TCPClientSocket* sender) { assert(false); }
	/// 连接失败
	virtual void OnSocketConnectFailed(TCPClientSocket* sender, long errcode) { assert(false); }
	/// socket关闭
	virtual void OnSocketClose(TCPClientSocket* sender, long errcode) { }	// can be ignored

	/// 接收到数据
	virtual void OnSocketReceive(TCPClientSocket* sender, unsigned char* data, size_t size) { assert(false); }
	/// 接收数据失败
	virtual void OnSocketReceiveFailed(TCPClientSocket* sender, long errcode) { assert(false); }

	/// 发送数据完毕
	virtual void OnSocketSend(TCPClientSocket* sender) { }	// can be ignored
	/// 发送数据失败
	virtual void OnSocketSendFailed(TCPClientSocket* sender, long errcode) { assert(false); }
};



#ifdef NEED_LOG

#include <iosfwd>


std::ostream& operator<<(std::ostream& os, const TCPClientSocket& sock);

#endif

