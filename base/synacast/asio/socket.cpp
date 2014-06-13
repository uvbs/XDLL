
//#include "stdafx.h"

#include <synacast/asio/socket.h>
#include <synacast/asio/socketimpl.h>

#include <ppl/util/log.h>
#include <ppl/diag/trace.h>

#include <boost/asio/write.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
using boost::make_tuple;

#include <boost/bind.hpp>


int TCPSocketImpl::m_PendingConnections = 0;


ostream& operator<<(ostream& os, const TCPClientSocket& sock)
{
	os << "TCPSocket: " << sock.GetRemoteAddress();
	return os;
}



bool UDPSocket::Open( u_short port )
{
	this->Close();

	udp::endpoint ep( udp::v4(), port );

	this->m_Impl->GetSocket().open( udp::v4(), m_LastError );
	if ( m_LastError )
		return false;
	this->m_Impl->GetSocket().bind( ep, m_LastError );
	if ( m_LastError )
		return false;
	boost::asio::detail::io_control::non_blocking_io iocmd(true);
	this->m_Impl->GetSocket().io_control(iocmd, m_LastError);
	return !m_LastError;
}

bool UDPSocket::Receive( size_t maxSize )
{
	UDPReceiveSessionPtr session( new UDPReceiveSession );
	session->buffer.resize( maxSize );

	m_Impl->GetSocket().async_receive_from(
		boost::asio::buffer(session->buffer.data(), session->buffer.size()), 
		session->remote_endpoint, 
		boost::bind(
			&UDPSocketImpl::HandleReceive, 
			m_Impl, 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, 
			UDPSocketImpl::LengthEventHandler( boost::bind(&UDPSocket::OnReceive, this, _1, _2, session) )
			)
		);
	return true;
}

bool UDPSocket::Send( const void* data, size_t size, const InetSocketAddress& addr )
{
	udp::endpoint ep = make_udp_endpoint( addr );
	m_Impl->GetSocket().async_send_to(
		boost::asio::buffer(data, size), 
		ep,
		boost::bind(
			&UDPSocketImpl::HandleSend, 
			m_Impl, 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
			)
		);

	return true;

	// error为0
//	if ( !m_LastError )
//		return true;
//
//#if defined(_PPL_PLATFORM_MSWIN)
//	if ( boost::asio::error::would_block == m_LastError )
//#elif defined(_PPL_PLATFORM_LINUX)
//	if ( boost::asio::error::try_again == m_LastError )
//#else
//#error "invalid platform for sendto"
//#endif
//	{
//		return true;
//	}
//	return false;
}

void UDPSocket::OnReceive( const error_code& error, std::size_t bytes, UDPReceiveSessionPtr session )
{
	if (!error || error == boost::asio::error::message_size)
	{
		assert( bytes > 0 );
		// 成功
		InetSocketAddress addr = from_endpoint( session->remote_endpoint );
		this->GetListener()->OnSocketReceive(this, addr, session->buffer.data(), bytes );
	}
	else
	{
		// 失败
		this->GetListener()->OnSocketReceiveFailed( this, error.value() );
	}
}

void UDPSocket::OnSend( const error_code& error, size_t bytes, shared_array<BYTE> buf )
{

}

UDPSocket::UDPSocket() : m_Impl(new UDPSocketImpl)
{
}
UDPSocket::~UDPSocket()
{
	this->Close();
	m_Impl->Close();
}

InetSocketAddress UDPSocket::GetLocalAddress() const
{
	if ( false == m_Impl->GetSocket().is_open() )
		return InetSocketAddress();

	return from_endpoint( m_Impl->GetSocket().local_endpoint(m_LastError) );
}

bool UDPSocket::IsValid() const
{
	return m_Impl->GetSocket().is_open();
}

bool UDPSocket::Close()
{
	m_Impl->GetSocket().cancel(m_LastError);
	m_Impl->GetSocket().close(m_LastError);
	assert( !m_LastError );
	return !m_LastError;
}
bool TCPClientSocket::Connect( u_long ip, u_short port )
{
	return this->Connect(InetSocketAddress(ip, port));
}
bool TCPClientSocket::Connect( const InetSocketAddress& addr )
{
	assert( this->IsValid() );
	tcp::endpoint ep = make_tcp_endpoint( addr );

	m_ConnectTimer.Start(m_ConnectTimeout);
	m_Socket->GetSocket().async_connect(
		ep,
		boost::bind( 
			&TCPSocketImpl::HandleConnectEvent,
			m_Socket, 
			boost::asio::placeholders::error, 
			TCPSocketImpl::EventHandler(boost::bind(&TCPClientSocket::HandleConnect, this, _1))
			)
		);

	TCPSocketImpl::IncPendingConnectionCount();

	return true;
}

void TCPClientSocket::HandleConnect( const error_code& error )
{
	this->OnConnect(error);
}

void TCPClientSocket::OnConnect( const error_code& error )
{
	m_ConnectTimer.Stop();
	//if ( this->IsClosed() )
	//{
	//	return;
	//}
	if ( !error )
	{
		this->GetListener()->OnSocketConnect( this );
	}
	else
	{
		this->GetListener()->OnSocketConnectFailed( this, error.value() );
	}
}

bool TCPClientSocket::DoSend(const void *data, std::size_t size)
{
	shared_array<BYTE> buf( new BYTE[size] );
	memcpy( buf.get(), data, size );
	this->AsyncSend(buf, size);
	return true;
}

// void TCPSocket::HandleTcpSend(const boost::asio::error& err, size_t bytes_transferred, MyBuffer b)
void TCPClientSocket::HandleTcpSend(const boost::system::error_code& err, size_t bytes_transferred)
{
	if ( m_SendQueue.empty() )
	{
		//TRACE("HandleTcpSend %lu %d\n", GetCurrentThreadId(), m_Socket->GetSocket().native());
		APP_ERROR("TCPClientSocket::HandleTcpSend invalid empty buffer " << make_tuple( err.value(), bytes_transferred, m_SendQueue.size() ));
		assert(false == m_Socket->GetSocket().is_open());
		return;
	}
	m_SendQueue.pop_front();
	if ( err )
	{
		// 发送失败
		APP_ERROR("TCPClientSocket::HandleTcpSend error " << make_tuple( err.value(), bytes_transferred, m_SendQueue.size() ));
		this->GetListener()->OnSocketReceiveFailed(this, err.value());
	}
	else
	{
		if ( m_SendQueue.empty() )
		{
			return;
		}
		else
		{
			this->DoAsyncSend(m_SendQueue.front());
		}
	}
}

void TCPClientSocket::HandleAsyncSend( const shared_array<BYTE>& buf, size_t size )
{
	//TRACE("HandleAsyncSend %lu\n", GetCurrentThreadId());
	shared_byte_buffer tcpdata(buf, size);
	m_SendQueue.push_back(tcpdata);
	if ( 1 == m_SendQueue.size() )
	{
		this->DoAsyncSend(tcpdata);
	}
}

void TCPClientSocket::AsyncSend( const shared_array<BYTE>& buf, size_t size )
{
	m_Socket->GetSocket().get_io_service().post( boost::bind( &TCPClientSocket::HandleAsyncSend, this, buf, size ) );
}

void TCPClientSocket::DoAsyncSend( const shared_byte_buffer& tcpdata )
{
	//printf("TCPClientSocket::AsyncSend %u\n", size);
	boost::asio::async_write(
		m_Socket->GetSocket(),
		boost::asio::buffer( tcpdata.buffer.get(), tcpdata.size ),
		boost::bind( 
			&TCPSocketImpl::HandleLengthEvent, 
			m_Socket, 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, 
			TCPSocketImpl::LengthEventHandler(boost::bind( &TCPClientSocket::HandleTcpSend, this, _1, _2 ))
			)
		//boost::bind(&on_tcp_send, buf, size, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred )
		);
}

InetSocketAddress TCPClientSocket::GetRemoteAddress() const
{
	boost::system::error_code err;
	boost::asio::ip::tcp::endpoint ep = this->m_Socket->GetSocket().remote_endpoint(err);
	if ( err )
		return InetSocketAddress();
	return from_endpoint(ep);
}

void TCPClientSocket::OnTimerElapsed( Timer* sender )
{
	if ( &m_ConnectTimer == sender )
	{
		this->OnConnect(boost::asio::error::timed_out);
	}
}

TCPClientSocket::TCPClientSocket() : m_ConnectTimeout( 60 * 1000 ), m_ReceiveTimeout( 60 * 1000 )
{
	m_Socket.reset( new TCPSocketImpl );
	m_ConnectTimer.SetListener( boost::bind( &TCPClientSocket::OnConnectTimeout, this ) );
	m_ReceiveTimer.SetListener( boost::bind( &TCPClientSocket::OnReceiveTimeout, this ) );
}

TCPClientSocket::~TCPClientSocket()
{
	//TRACE("TCPClientSocket::~TCPClientSocket() close %p %p\n", this, m_Socket);
	m_Socket->Close();
	this->Close();
}

bool TCPClientSocket::Close()
{
	//TRACE("TCPClientSocket::Close %p %p %u\n", this, m_Socket, m_SendQueue.size());
	m_SendQueue.clear();
	m_Socket->GetSocket().cancel(m_LastError);
	m_Socket->GetSocket().close(m_LastError);
//	m_closed = true;
	//error_code ec;
	//m_impl.cancel(ec);
	//m_impl.close(ec);
	//assert( !ec );
	//return !ec;
	return !m_LastError;
}

bool TCPClientSocket::Create()
{
//	TRACE("TCPClientSocket::Create %p %p\n", this, m_Socket);
	this->Close();
//	m_closed = false;
//	this->m_impl.open( tcp::v4(), m_LastError );
	this->m_Socket->GetSocket().open( tcp::v4(), m_LastError );
	return !m_LastError;
}

bool TCPClientSocket::Receive()
{
	assert(false);
	return false;
}

bool TCPClientSocket::IsValid() const
{
	return m_Socket->GetSocket().is_open();
}

int TCPClientSocket::GetPendingConnectionCount()
{
	return TCPSocketImpl::GetPendingConnectionCount();
}

void TCPClientSocket::OnConnectTimeout()
{
	this->OnConnect(boost::asio::error::timed_out);
}

void TCPClientSocket::OnReceiveTimeout()
{
}
SocketBase::~SocketBase()
{

}





