
#pragma once

#include <synacast/asio/ioservice.h>
#include <synacast/asio/timer.h>
#include <ppl/net/socketfwd.h>
#include <ppl/net/inet.h>
#include <ppl/util/listener.h>
#include <ppl/util/memory.h>
#include <ppl/util/buffer.h>

#include <boost/noncopyable.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/asio/placeholders.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
using boost::shared_ptr;
using boost::shared_array;

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using boost::system::error_code;

#include <list>

typedef basic_buffer<unsigned char, malloc_alloc> byte_buffer;


class TCPSocketImpl;
class UDPSocketImpl;

inline udp::endpoint make_udp_endpoint( const InetSocketAddress& addr )
{
	udp::endpoint ep( boost::asio::ip::address_v4(addr.GetIPHostByteOrder()), addr.GetPort() );
	return ep;
}

inline tcp::endpoint make_tcp_endpoint( const InetSocketAddress& addr )
{
	tcp::endpoint ep( boost::asio::ip::address_v4(addr.GetIPHostByteOrder()), addr.GetPort() );
	return ep;
}

inline InetSocketAddress from_endpoint( const udp::endpoint& ep )
{
	InetSocketAddress addr( htonl( ep.address().to_v4().to_ulong() ), ep.port() );
	return addr;
}

inline InetSocketAddress from_endpoint( const tcp::endpoint& ep )
{
	InetSocketAddress addr( htonl( ep.address().to_v4().to_ulong() ), ep.port() );
	return addr;
}



struct shared_byte_buffer
{
	shared_array<BYTE> buffer;
	size_t size;

	shared_byte_buffer() : size(0) {}
	explicit shared_byte_buffer(const shared_array<BYTE>& buf, size_t len) : buffer(buf), size(len)
	{
		assert(buf);
		assert(len > 0);
	}
};
typedef std::list<shared_byte_buffer> shared_byte_buffer_queue;


class SocketBase : private boost::noncopyable
{

public:
	virtual ~SocketBase();

	int GetLastError() const
	{
		return m_LastError.value();
	}

	int LastError() const
	{
		return m_LastError.value();
	}


protected:
	mutable boost::system::error_code m_LastError;

};



class UDPSocket : public SocketBase, public Listenable<UDPSocketListener, TrivialUDPSocketListener>
{
public:

	struct UDPReceiveSession
	{
		byte_buffer buffer;
		udp::endpoint remote_endpoint;
	};

	typedef boost::shared_ptr<UDPReceiveSession> UDPReceiveSessionPtr;


	UDPSocket();
	virtual ~UDPSocket();


	InetSocketAddress GetLocalAddress() const;


	bool Close();

	bool Open( u_short port );

	bool Receive( size_t maxSize );

	/// 发送报文到指定的socket地址
	bool Send(const void* data, size_t size, const InetSocketAddress& addr);

	bool IsValid() const;

protected:
	void OnReceive(const boost::system::error_code& error, std::size_t bytes, UDPReceiveSessionPtr session);
	void OnSend(const boost::system::error_code& error, size_t bytes, shared_array<BYTE> buf);


protected:
	boost::shared_ptr<UDPSocketImpl> m_Impl;
};




class TCPClientSocket : public SocketBase, public Listenable<TCPClientSocketListener, TrivialTCPClientSocketListener>
{
public:
	TCPClientSocket();
	~TCPClientSocket();

	bool IsValid() const;

	//bool IsClosed() const
	//{
	//	return m_closed;
	//}

	bool Connect(u_long ip, u_short port);
	bool Connect(const InetSocketAddress& addr);

	virtual bool Close();

	bool Create();

	bool Send(const void* data, size_t size)
	{
		return this->DoSend(data, size);
	}

	virtual bool Receive();

	InetSocketAddress GetRemoteAddress() const;

	long GetSendingCount()
	{
		return 0;
	}

	static int GetPendingConnectionCount();

	void SetConnectTimeout(int timeout)
	{
		m_ConnectTimeout = timeout * 1000;
	}

	virtual void OnTimerElapsed(Timer* sender);

protected:
	virtual void OnConnect(const boost::system::error_code& error);
	void HandleConnect(const boost::system::error_code& error);

	virtual bool DoSend(const void* data, size_t size);

	void AsyncSend(const shared_array<BYTE>& buf, size_t size);
	void DoAsyncSend(const shared_byte_buffer& tcpdata);

	void HandleAsyncSend(const shared_array<BYTE>& buf, size_t size);

	void HandleTcpSend(const boost::system::error_code& err, size_t bytes_transferred);

	void OnConnectTimeout();
	void OnReceiveTimeout();

protected:
//	tcp::socket m_impl;
	boost::shared_ptr<TCPSocketImpl> m_Socket;

	//static int m_PendingConnections;

	OnceTimer m_ConnectTimer;
	UINT m_ConnectTimeout;

	OnceTimer m_ReceiveTimer;
	UINT m_ReceiveTimeout;

	friend class TCPServerSocket;

	shared_byte_buffer_queue m_SendQueue;

//	bool m_closed;
};


typedef boost::shared_ptr<TCPClientSocket> TCPClientSocketPtr;


