
#pragma once

#include <ppl/util/listener.h>
#include <ppl/net/socketfwd.h>
#include <ppl/stl/string.h>
#include <boost/noncopyable.hpp>


class SocketImpl;


class SocketBase : private boost::noncopyable
{
public:
	SocketBase();
	virtual ~SocketBase();

	bool IsValid() const;
	void Close()
	{
		DoClose();
	}
	SOCKET GetHandle() const;

	InetSocketAddress GetLocalAddress() const;

	int GetLastError();


protected:
	virtual void OnAccept(long errcode) { }
	virtual void OnConnect(long errcode) { }
	virtual void OnClose(long errcode) { }
	virtual void OnReceive(long errcode) { }
	virtual void OnSend(long errcode) { }

	virtual void DoClose();

	long LastError() const { return m_lastError; }

protected:
	friend class SocketImpl;
	SocketImpl*	m_impl;
	long m_lastError;
};

class UDPSocket : public SocketBase, public Listenable<UDPSocketListener, TrivialUDPSocketListener>
{
public:
	UDPSocket();
	virtual ~UDPSocket();

	bool Open(u_short port);
	bool Send(const void* data, size_t size, u_long ip, u_short port);
	bool Send(const void* data, size_t size, const InetSocketAddress& addr);
	bool Receive() { return true; }
	bool Receive(size_t size) { return true; }
protected:
	virtual void OnReceive(long errcode);
	virtual void OnSend(long errcode);

private:
	enum { MAX_PACKET_SIZE = 64 * 1024 };
	unsigned char m_buffer[MAX_PACKET_SIZE + 1];
};


class RawSocket : public UDPSocket
{
public:
	bool Create();

	bool SetTTL(int ttl);

private:
	using UDPSocket::Open;
};


class TCPServerSocket : public SocketBase, public Listenable<TCPServerSocketListener, TrivialTCPServerSocketListener>
{
public:
	TCPServerSocket();
	virtual ~TCPServerSocket();

	bool Open(u_short port);

protected:
	virtual void OnAccept(long errcode);
};


typedef TCPServerSocket PeerServerSocket;


class TCPClientSocket : public SocketBase, public Listenable<TCPClientSocketListener, TrivialTCPClientSocketListener>
{
public:
	TCPClientSocket();
	explicit TCPClientSocket(SOCKET sock);
	virtual ~TCPClientSocket();

	virtual bool Connect(const InetSocketAddress& addr);
	bool Connect(const char* ip, u_short port);
	bool Connect(u_long ip, u_short port);
	bool Send(const void* data, size_t size)
	{
		return DoSend(data, size);
	}
	bool Send(const char* data)
	{
		return Send(data, strlen(data));
	}
	bool Send(const string& data)
	{
		return Send(data.data(), data.size());
	}
	InetSocketAddress GetRemoteAddress() const;
	InetSocketAddress GetLocalAddress() const;

	bool Receive()
	{
		return true;
	}

	void SetConnectTimeout(int timeout) { }
	static int GetPendingConnectionCount();

protected:
	int Receive(unsigned char* buffer, size_t size)
	{
		return DoReceive(buffer, size);
	}
	void ReceiveAvailableData();

protected:
	virtual int DoReceive(unsigned char* buffer, size_t size);
	virtual bool DoSend(const void* data, size_t size);

	virtual void OnConnect(long errcode);
	virtual void OnClose(long errcode);
	virtual void OnSend(long errcode);
	virtual void OnReceive(long errcode);

protected:
	static int m_pendingConnectionCount;
};



class PeerClientSocket : public TCPClientSocket
{
public:
	PeerClientSocket();
	explicit PeerClientSocket(SOCKET sock);

protected:
	virtual int DoReceive(unsigned char* buffer, size_t size);
	virtual bool DoSend(const void* data, size_t size);

	void Init();
	virtual void OnConnect(long errcode);
	virtual void OnClose(long errcode);
	virtual void OnReceive(long errcode);

	bool IsHeadReceived() const;
	void ReceiveHead();
	void ReceiveBody();

	virtual void DoClose();

private:
	enum { MAX_HEAD_SIZE = sizeof(unsigned long), MAX_PACKET_SIZE = 64 * 1024 };
	unsigned long	m_bodyLength;
	unsigned char	m_headLength;
	size_t	m_len;
	unsigned char m_buffer[MAX_PACKET_SIZE + 1];
};


typedef TCPServerSocket HTTPServerSocket;
typedef TCPClientSocket HTTPClientSocket;
typedef TCPClientSocket HTTPClientProxySocket;

