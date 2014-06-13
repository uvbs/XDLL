
#pragma once

#include <synacast/asio/socket.h>
#include <boost/asio/streambuf.hpp>
#include <memory>
#include <boost/enable_shared_from_this.hpp>


class PeerClientSocket : public TCPClientSocket
{
public:
	explicit PeerClientSocket(bool needCreate = true);
	~PeerClientSocket();

	static void SetMaxPeerReceivePacketLength(size_t len);
	static void SetBigEndian( bool bigEndian );

	virtual bool Close();

public:
	virtual bool Receive();

protected:
	virtual bool DoSend(const void* data, size_t size);

protected:
	void TcpRecvLengthBuffer();
	void TcpRecvDataBuffer();
	void HandleTcpRecvLengthBuffer(const boost::system::error_code& error, size_t bytes_transferred);
	void HandleTcpRecvDataBuffer(const boost::system::error_code& error, size_t bytes_transferred);
	void HandleTcpRecvTimeout(const boost::system::error_code& err);

	void ResetState();


protected:
	static size_t max_peer_recv_length;
	static bool m_big_endian;

	u_int m_length;
	byte_buffer m_body;

	bool m_receiving;

};

class HTTPClientProxySocket : public TCPClientSocket
{
public:
	const static size_t HTTP_HEADER_MAX_SIZE = 10*1024;

	explicit HTTPClientProxySocket();
	virtual ~HTTPClientProxySocket();

public:
	virtual bool Receive()
	{
		this->HttpRecv();
		return true;
	}
	void HttpRecv();

	virtual void OnTimerElapsed(Timer* sender);
protected:
	void HandleHttpHeader(const boost::system::error_code& err, size_t bytes_transferred);
	void HandleHttpTimeout(const boost::system::error_code& err);

protected:
	boost::asio::streambuf request_;

};

class TCPServerSocket : public SocketBase, public Listenable<TCPServerSocketListener, TrivialTCPServerSocketListener>, public boost::enable_shared_from_this<TCPServerSocket>
{
public:
	TCPServerSocket();
	virtual ~TCPServerSocket();

	InetSocketAddress GetLocalAddress() const;

	bool Open( u_short port );

	bool IsValid() const;

	void Close();

protected:
	void StartAccept();
	void HandleAccept(const boost::system::error_code& err);
	virtual TCPClientSocket* CreateNewClient() = 0;

protected:
	boost::asio::ip::tcp::acceptor acceptor_;
	std::auto_ptr<TCPClientSocket> m_NewSocket;
	tcp::endpoint m_RemoteAddress;

};

class PeerServerSocket : public TCPServerSocket
{
public:
	PeerServerSocket();
	virtual ~PeerServerSocket();

protected:
	virtual TCPClientSocket* CreateNewClient()
	{
		return new PeerClientSocket(false);
	}
};



class HTTPServerSocket : public TCPServerSocket
{
public:
	HTTPServerSocket();
	virtual ~HTTPServerSocket();

protected:
	virtual TCPClientSocket* CreateNewClient()
	{
		return new HTTPClientProxySocket;
	}
};



