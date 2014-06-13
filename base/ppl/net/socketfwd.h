
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


/// udp socket���¼�
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







/// TCPClientSocket���¼��ӿ�
class TCPClientSocketListener
{
public:
	virtual ~TCPClientSocketListener() { }

	/// ���ӳɹ�
	virtual void OnSocketConnect(TCPClientSocket* sender) { assert(false); }
	/// ����ʧ��
	virtual void OnSocketConnectFailed(TCPClientSocket* sender, long errcode) { assert(false); }
	/// socket�ر�
	virtual void OnSocketClose(TCPClientSocket* sender, long errcode) { }	// can be ignored

	/// ���յ�����
	virtual void OnSocketReceive(TCPClientSocket* sender, unsigned char* data, size_t size)  = 0;
	/// ��������ʧ��
	virtual void OnSocketReceiveFailed(TCPClientSocket* sender, long errcode) { assert(false); }

	/// �����������
	virtual void OnSocketSend(TCPClientSocket* sender) { }	// can be ignored
	/// ��������ʧ��
	virtual void OnSocketSendFailed(TCPClientSocket* sender, long errcode) { this->OnSocketReceiveFailed(sender, errcode); }
};




/// TCPServerSocket���¼��ӿ�
class TCPServerSocketListener
{
public:
	virtual ~TCPServerSocketListener() { }

	/// ���յ��µ�����
	virtual void OnSocketAccept(TCPServerSocket* sender, TCPClientSocket* client, const InetSocketAddress& addr) = 0;

	/// ����������ʧ��
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



/// udp socket���¼�
class TrivialUDPSocketListener : public UDPSocketListener
{
public:
	virtual void OnSocketReceive(UDPSocket* sender, const InetSocketAddress& addr, unsigned char* data, size_t size) { assert(false); }
	virtual void OnSocketReceiveFailed(UDPSocket* sender, long errcode) { assert(false); }
};

class TrivialTCPClientSocketListener : public TCPClientSocketListener
{
public:
	/// ���ӳɹ�
	virtual void OnSocketConnect(TCPClientSocket* sender) { assert(false); }
	/// ����ʧ��
	virtual void OnSocketConnectFailed(TCPClientSocket* sender, long errcode) { assert(false); }
	/// socket�ر�
	virtual void OnSocketClose(TCPClientSocket* sender, long errcode) { }	// can be ignored

	/// ���յ�����
	virtual void OnSocketReceive(TCPClientSocket* sender, unsigned char* data, size_t size) { assert(false); }
	/// ��������ʧ��
	virtual void OnSocketReceiveFailed(TCPClientSocket* sender, long errcode) { assert(false); }

	/// �����������
	virtual void OnSocketSend(TCPClientSocket* sender) { }	// can be ignored
	/// ��������ʧ��
	virtual void OnSocketSendFailed(TCPClientSocket* sender, long errcode) { assert(false); }
};



#ifdef NEED_LOG

#include <iosfwd>


std::ostream& operator<<(std::ostream& os, const TCPClientSocket& sock);

#endif

