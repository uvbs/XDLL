
#include "stdafx.h"


#include "net.h"
#include <ppl/net/inet.h>
#include <ppl/util/log.h>


class SocketImpl : public CAsyncSocket
{
public:
	explicit SocketImpl(SocketBase& owner) : m_owner(owner) { }
	virtual ~SocketImpl()
	{
		UTIL_ERROR("destruct socket impl");
	}

protected:
	virtual void OnAccept(int errcode)		{ m_owner.OnAccept(errcode); }
	virtual void OnConnect(int errcode)		{ m_owner.OnConnect(errcode); }
	virtual void OnClose(int errcode)		{ m_owner.OnClose(errcode); }
	virtual void OnReceive(int errcode)		{ m_owner.OnReceive(errcode); }
	virtual void OnSend(int errcode)		{ m_owner.OnSend(errcode); }

private:
	SocketBase&	m_owner;
};



SocketBase::SocketBase() : m_lastError(0)
{
	m_impl = new SocketImpl(*this);
}

SocketBase::~SocketBase()
{
	delete m_impl;
}

void SocketBase::DoClose()
{
	m_impl->Close();
}

SOCKET SocketBase::GetHandle() const
{
	return m_impl->m_hSocket;
}

int SocketBase::GetLastError()
{
	return m_impl->GetLastError();
}

bool SocketBase::IsValid() const
{
	return GetHandle() != INVALID_SOCKET;
}

InetSocketAddress SocketBase::GetLocalAddress() const
{
	InetSocketAddress addr;
	int addrlen = addr.GetAddressLength();
	VERIFY(m_impl->GetSockName(addr.GetAddress(), &addrlen));
	assert(addrlen == addr.GetAddressLength());
	return addr;
}


UDPSocket::UDPSocket()
{
}

UDPSocket::~UDPSocket()
{
	Close();
}

bool UDPSocket::Open(u_short port)
{
	assert(!IsValid());
	if (m_impl->Create(port, SOCK_DGRAM))
		return true;
	TRACE(TEXT("UDPSocket::Open(%d) failed. errcode=%d\n"), port, ::WSAGetLastError());
	return false;
}

bool UDPSocket::Send(const void* data, size_t size, u_long ip, u_short port)
{
	InetSocketAddress addr(ip, port);
	return Send(data, size, addr);
}

bool UDPSocket::Send(const void* data, size_t size, const InetSocketAddress& addr)
{
	int len = m_impl->SendTo(data, size, addr.GetAddress(), addr.GetAddressLength());
	if (len == -1 || len == 0)
	{
		UTIL_ERROR("UDPSocket::Send failed. " << make_pair(len, ::WSAGetLastError()));
		//assert(false);
		return false;
	}
	assert(len == size);
	return len == size;
}

void UDPSocket::OnSend(long errcode)
{
	if (errcode == 0)
	{
		GetListener()->OnSocketSend(this);
	}
	else
	{
		GetListener()->OnSocketSendFailed(this, errcode);
	}
}

void UDPSocket::OnReceive(long errcode)
{
	if (errcode != 0)
	{
		GetListener()->OnSocketReceiveFailed(this, errcode);
		return;
	}
	InetSocketAddress addr;
	int addrlen = addr.GetAddressLength();
	int len = m_impl->ReceiveFrom(m_buffer, MAX_PACKET_SIZE, addr.GetAddress(), &addrlen);
	if (len > 0)
	{
		assert(len <= MAX_PACKET_SIZE);
		assert(addrlen == addr.GetAddressLength());
		m_buffer[len] = '\0';
		GetListener()->OnSocketReceive(this, addr, m_buffer, len);
	}
	else
	{
		TRACE(TEXT("Error: ReceiveFrom %d\n"), WSAGetLastError());
	}
}





bool RawSocket::Create()
{
	assert(!IsValid());
	if (!m_impl->Socket(SOCK_RAW, FD_READ, IPPROTO_ICMP, AF_INET))
	{
		TRACE(TEXT("RawSocket::Open(%d) failed. errcode=%d\n"), 0, ::WSAGetLastError());
		return false;
	}
	return true;
}

bool RawSocket::SetTTL(int ttl)
{
	const int ip_ttl = 7;
	return m_impl->SetSockOpt(ip_ttl, reinterpret_cast<const char*>(&ttl), IPPROTO_IP) != FALSE;
}



TCPServerSocket::TCPServerSocket()
{
}

TCPServerSocket::~TCPServerSocket()
{
	Close();
}

bool TCPServerSocket::Open(u_short port)
{
	assert(!IsValid());
	if (!m_impl->Create(port))
	{
		TRACE(TEXT("TCPServerSocket::Open(%d) -- Create failed. errcode=%d\n"), port, ::WSAGetLastError());
		return false;
	}
	if (!m_impl->Listen())
	{
		TRACE(TEXT("TCPServerSocket::Open(%d) -- Listen failed. errcode=%d\n"), port, ::WSAGetLastError());
		return false;
	}
	return true;
}

void TCPServerSocket::OnAccept(long errcode)
{
	if (errcode == 0)
	{
		InetSocketAddress addr;
		int addrlen = addr.GetAddressLength();
		SOCKET hTemp = accept(m_impl->m_hSocket, addr.GetAddress(), &addrlen);
		assert(addrlen);
		if (hTemp != INVALID_SOCKET)
		{
			GetListener()->OnSocketAccept(this, new TCPClientSocket(hTemp), addr);
		}
		else
		{
			UTIL_ERROR("accept failed " << ::WSAGetLastError());
		}
	}
	else
	{
		GetListener()->OnSocketAcceptFailed(this, errcode);
	}
}




TCPClientSocket::TCPClientSocket()
{
}

TCPClientSocket::~TCPClientSocket()
{
	Close();
}


TCPClientSocket::TCPClientSocket(SOCKET sock)
{
	m_impl->Attach(sock);
}

bool TCPClientSocket::Connect(const InetSocketAddress& addr)
{
	Close();
	VERIFY(m_impl->Create());
	if (m_impl->Connect(addr.GetAddress(), addr.GetAddressLength()))
	{
		m_pendingConnectionCount++;
		return true;
	}
	if (::WSAGetLastError() == WSAEWOULDBLOCK)
	{
		m_pendingConnectionCount++;
		return true;
	}
	assert(false);
	return false;
}

bool TCPClientSocket::Connect(const char* ip, u_short port)
{
	return Connect(ResolveHostName(ip), port);
}

bool TCPClientSocket::Connect(u_long ip, u_short port)
{
	return Connect(InetSocketAddress(ip, port));
}

InetSocketAddress TCPClientSocket::GetRemoteAddress() const
{
	InetSocketAddress addr;
	int addrlen = addr.GetAddressLength();
	BOOL success = m_impl->GetPeerName(addr.GetAddress(), &addrlen);
	if (success)
	{
		assert(addrlen == addr.GetAddressLength());
	}
	return addr;
}

InetSocketAddress TCPClientSocket::GetLocalAddress() const
{
	InetSocketAddress addr;
	int addrlen = addr.GetAddressLength();
	VERIFY(m_impl->GetSockName(addr.GetAddress(), &addrlen));
	assert(addrlen == addr.GetAddressLength());
	return addr;
}


int TCPClientSocket::DoReceive(unsigned char* buffer, size_t size)
{
	return m_impl->Receive(buffer, size);
}

bool TCPClientSocket::DoSend(const void* data, size_t size)
{
	int len = m_impl->Send(data, size);
	if (len == -1 || len == 0)
	{
		int errcode = ::WSAGetLastError();
		if (errcode != WSAEWOULDBLOCK)
		{
			UTIL_ERROR("TCPClientSocket::DoSend failed. " << make_pair(len, ::WSAGetLastError()));
			return false;
		}
		else
		{
			assert(len == -1);
		}
	}
	else
	{
		assert(len == size);
	}
	return true;
}

void TCPClientSocket::OnConnect(long errcode)
{
	m_pendingConnectionCount--;
	if (errcode == 0)
	{
		GetListener()->OnSocketConnect(this);
	}
	else
	{
		GetListener()->OnSocketConnectFailed(this, errcode);
	}
}

void TCPClientSocket::OnClose(long errcode)
{
	m_pendingConnectionCount--;
	GetListener()->OnSocketClose(this, errcode);
}

void TCPClientSocket::OnSend(long errcode)
{
	if (errcode == 0)
	{
		GetListener()->OnSocketSend(this);
	}
	else
	{
		GetListener()->OnSocketSendFailed(this, errcode);
	}
}

void TCPClientSocket::OnReceive(long errcode)
{
	if (errcode == 0)
	{
		//if (!GetListener()->OnSocketReceive(this))
		{
			ReceiveAvailableData();
		}
	}
	else
	{
		GetListener()->OnSocketReceiveFailed(this, errcode);
	}
}

void TCPClientSocket::ReceiveAvailableData()
{
	const size_t max_size = 1024 * 8;
	unsigned char buf[max_size + 1];
	int len = Receive(buf, max_size);
	if (len > 0)
	{
		buf[len] = '\0';
		GetListener()->OnSocketReceive(this, buf, len);
		return;
	}
	GetListener()->OnSocketReceiveFailed(this, (len == 0) ? len : ::WSAGetLastError());
	Close();
	assert(false);
}

int TCPClientSocket::m_pendingConnectionCount = 0;

int TCPClientSocket::GetPendingConnectionCount()
{
	return 0;
	//return m_pendingConnectionCount;
}



PeerClientSocket::PeerClientSocket()
{
	Init();
}
PeerClientSocket::PeerClientSocket(SOCKET sock) : TCPClientSocket(sock)
{
	Init();
}

void PeerClientSocket::DoClose()
{
	TCPClientSocket::DoClose();
	Init();
}

int PeerClientSocket::DoReceive(unsigned char* buffer, size_t size)
{
	assert(false);
	return 0;
}
bool PeerClientSocket::DoSend(const void* data, size_t size)
{
	int len = m_impl->Send(&size, MAX_HEAD_SIZE);
	if (len == -1 || len == 0)
	{
		int errcode = ::WSAGetLastError();
		if (errcode != WSAEWOULDBLOCK)
		{
			UTIL_ERROR("PeerClientSocket::DoSend failed. " << make_pair(len, errcode));
			return false;
		}
	}
	assert(len == MAX_HEAD_SIZE);
	int len2 = m_impl->Send(data, size);
	if (len2 == -1 || len2 == 0)
	{
		int errcode = ::WSAGetLastError();
		if (errcode != WSAEWOULDBLOCK)
		{
			UTIL_ERROR("PeerClientSocket::DoSend2 failed. " << make_pair(len, errcode));
			return false;
		}
	}
	assert(len2 == size);
	return true;
}

void PeerClientSocket::Init()
{
	m_bodyLength = 0;
	m_headLength = 0;
	m_len = 0;
}
void PeerClientSocket::OnConnect(long errcode)
{
	Init();
	TCPClientSocket::OnConnect(errcode);
}
void PeerClientSocket::OnClose(long errcode)
{
	Init();
	TCPClientSocket::OnClose(errcode);
}
void PeerClientSocket::OnReceive(long errcode)
{
	if (IsHeadReceived())
	{
		ReceiveBody();
	}
	else
	{
		ReceiveHead();
	}
}

bool PeerClientSocket::IsHeadReceived() const
{
	assert(m_headLength <= MAX_HEAD_SIZE);
	return m_headLength == MAX_HEAD_SIZE;
}
void PeerClientSocket::ReceiveHead()
{
	char* buf = reinterpret_cast<char*>(&m_bodyLength);
	int maxLen = MAX_HEAD_SIZE - m_headLength;
	int len = m_impl->Receive(buf + m_headLength, maxLen);
	if (len < 0)
	{
		DWORD errcode = ::WSAGetLastError();
		GetListener()->OnSocketReceiveFailed(this, errcode);
		return;
	}
	if (len == 0)
	{
		assert(len == 0);
		return;
	}
	assert(len > 0 && len <= maxLen);
	m_headLength += len;
	PPL_LOG("PeerClientSocket::ReceiveHead " << make_pair(m_headLength, m_bodyLength));
	if (IsHeadReceived())
	{
		m_len = 0;
		if (m_bodyLength > MAX_PACKET_SIZE)
		{
			Close();
			PPL_LOG("PeerClientSocket::ReceiveHead m_bodyLength > MAX_PACKET_SIZE " << make_pair(m_bodyLength, int(MAX_PACKET_SIZE)));
			GetListener()->OnSocketReceiveFailed(this, -1);
			//assert(false);
		}
		else if (m_bodyLength == 0)
		{
			PPL_LOG("PeerClientSocket::ReceiveHead m_bodyLength == 0 ");
			Close();
			GetListener()->OnSocketReceiveFailed(this, -2);
			//assert(false);
		}
	}
}
void PeerClientSocket::ReceiveBody()
{
	assert(IsHeadReceived());
	assert(m_len < m_bodyLength);
	int maxLen = m_bodyLength - m_len;
	int len = m_impl->Receive(m_buffer + m_len, maxLen);
	if (len < 0)
	{
		DWORD errcode = ::WSAGetLastError();
		GetListener()->OnSocketReceiveFailed(this, errcode);
		return;
	}
	if (len == 0)
	{
		assert(len == 0);
		return;
	}
	assert(len > 0 && len <= maxLen);
	m_len += len;
	PPL_LOG("PeerClientSocket::ReceiveBody " << make_pair(m_len, m_bodyLength));
	if (m_len == m_bodyLength)
	{
		unsigned long bodyLength = m_bodyLength;
		Init();
		GetListener()->OnSocketReceive(this, m_buffer, bodyLength);
	}
}





#ifdef NEED_LOG

ostream& operator<<(ostream& os, const TCPClientSocket& sock)
{
	os << sock.GetRemoteAddress();
	return os;
}

#endif


