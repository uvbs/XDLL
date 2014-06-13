
//#include "stdafx.h"

#include <synacast/asio/PeerClientSocket.h>
#include <synacast/asio/socketimpl.h>

#include <ppl/io/ByteOrder.h>

#include <boost/asio/error.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/regex.hpp>


size_t PeerClientSocket::max_peer_recv_length = 1024 * 1024;
bool PeerClientSocket::m_big_endian = false;


PeerClientSocket::PeerClientSocket(bool needCreate) : m_receiving( false ), m_length( 0 )
{
	if ( needCreate )
	{
		this->Create();
	}
}

bool PeerClientSocket::DoSend( const void* data, size_t size )
{
	if ( size >= max_peer_recv_length )
	{
		assert( false );
		return false;
	}
	shared_array<BYTE> buf( new BYTE[size + 4] );
	UINT32 packetSize = size;
	if ( m_big_endian )
	{
		packetSize = ByteOrder::reverse_uint32( size );
	}
	WRITE_MEMORY(buf.get(), packetSize, UINT);

	memcpy( buf.get() + 4, data, size );
	this->AsyncSend(buf, size + 4);
	return true;
}


bool PeerClientSocket::Receive()
{
	if ( m_receiving )
	{
		assert(false);
		return false;
	}
	TcpRecvLengthBuffer();
	return true;
}

void PeerClientSocket::TcpRecvLengthBuffer()
{
	assert( false == m_receiving );
	m_receiving = true;
	boost::asio::async_read(
		m_Socket->GetSocket(), 
		boost::asio::buffer( &m_length, 4 ),
		boost::asio::transfer_at_least(4),
		boost::bind( 
			&TCPSocketImpl::HandleLengthEvent, 
			m_Socket,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, 
			TCPSocketImpl::LengthEventHandler(boost::bind( &PeerClientSocket::HandleTcpRecvLengthBuffer, this, _1, _2 ))
		)
	);
}

void PeerClientSocket::TcpRecvDataBuffer()
{
	assert( m_receiving );

	//boost::shared_array<byte> data( new byte[length_] );
	//data_ = data;
	m_body.resize( m_length );

	boost::asio::async_read(
		m_Socket->GetSocket(), 
		boost::asio::buffer( m_body.data(), m_length ),
		boost::asio::transfer_at_least(m_length),
		boost::bind( 
			&TCPSocketImpl::HandleLengthEvent, 
			m_Socket,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, 
			TCPSocketImpl::LengthEventHandler(boost::bind( &PeerClientSocket::HandleTcpRecvDataBuffer, this, _1, _2 ))
		)
	);
}

void PeerClientSocket::HandleTcpRecvLengthBuffer(const boost::system::error_code& err, size_t bytes_transferred)
{
	if ( false == m_receiving || false == this->IsValid() )
		return;

	if( ! err )
	{
		assert(bytes_transferred == 4);
		if ( m_big_endian )
		{
			m_length = ByteOrder::reverse_uint32( m_length );
		}

		if( m_length > max_peer_recv_length )
		{
			// 向上汇报收包错误信息
			this->ResetState();
			if ( this->IsValid() )
			{
				Close();
				this->GetListener()->OnSocketReceiveFailed(this, -3);
			}
			return;
		}

		TcpRecvDataBuffer();
	}
	else
	{	// 向上汇报收包错误信息
		this->ResetState();
		if ( this->IsValid() )
		{
			Close();
			if( err == boost::asio::error::timed_out|| err == boost::asio::error::operation_aborted )
			{
				this->GetListener()->OnSocketReceiveFailed(this, -4);
			}
			else if( err == boost::asio::error::shut_down )
			{
				this->GetListener()->OnSocketReceiveFailed(this, -5);
			}
			else
			{
				this->GetListener()->OnSocketReceiveFailed(this, -2);
			}
		}
	}
}

void PeerClientSocket::HandleTcpRecvDataBuffer(const boost::system::error_code& err, size_t bytes_transferred)
{
	if ( false == m_receiving || false == this->IsValid() )
		return;


	if( ! err )
	{
		assert(bytes_transferred == m_length);

		APP_EVENT("PeerClientSocket::HandleTcpRecvDataBuffe ");
		// 向上汇报收包正确信息
		this->ResetState();
		if ( this->IsValid() )
		{
			this->GetListener()->OnSocketReceive(this, m_body.data(), m_length);
		}
	}
	else
	{	// 向上汇报收包错误信息
		this->ResetState();
		if ( this->IsValid() )
		{
			Close();
			if( err == boost::asio::error::timed_out || err == boost::asio::error::operation_aborted )
			{
				this->GetListener()->OnSocketReceiveFailed(this, -4);
			}
			else if( err == boost::asio::error::shut_down )
			{
				this->GetListener()->OnSocketReceiveFailed(this, -5);
			}
			else
			{
				this->GetListener()->OnSocketReceiveFailed(this, -1);
			}
		}
	}
}

bool PeerClientSocket::Close()
{
	this->ResetState();
	return TCPClientSocket::Close();
}

void PeerClientSocket::ResetState()
{
	m_receiving = false;
}

PeerClientSocket::~PeerClientSocket()
{
	TCPClientSocketListener* listener = this->GetListener();
	APP_EVENT("delete PeerClientSocket " << listener);
	this->Close();
}

void PeerClientSocket::SetMaxPeerReceivePacketLength( size_t len )
{
	max_peer_recv_length = len;
}

void PeerClientSocket::SetBigEndian( bool bigEndian )
{
	m_big_endian = bigEndian;
}

TCPServerSocket::TCPServerSocket() : acceptor_( IOService::Instance() )
{

}

InetSocketAddress TCPServerSocket::GetLocalAddress() const
{
	if ( false == acceptor_.is_open() )
		return InetSocketAddress();

	return from_endpoint( acceptor_.local_endpoint(m_LastError) );
}

bool TCPServerSocket::Open( u_short port )
{
	boost::system::error_code& err = m_LastError;
	this->acceptor_.open(boost::asio::ip::tcp::v4(), err);
	if ( err )
	{
		this->Close();
		assert(false);
		return false;
	}
	this->acceptor_.bind(
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), 
		err
		);
	if ( err )
	{
		//assert(false);
		this->Close();
		return false;
	}
	this->acceptor_.listen(0, err);
	if ( err )
	{
		this->Close();
		assert(false);
		return false;
	}
	this->StartAccept();
	return true;
}

bool TCPServerSocket::IsValid() const
{
	return this->acceptor_.is_open();
}

void TCPServerSocket::StartAccept()
{
	m_NewSocket.reset( this->CreateNewClient() );

	acceptor_.async_accept( 
		m_NewSocket->m_Socket->GetSocket(),
		m_NewSocket->m_Socket->GetRemoteEndpoint(),
		boost::bind(
		&TCPServerSocket::HandleAccept, 
		this->shared_from_this(), 
		boost::asio::placeholders::error
		)
		);
}

void TCPServerSocket::HandleAccept( const boost::system::error_code& err )
{
	if ( err )
	{
		m_NewSocket.reset();
	}
	else
	{
		TCPClientSocket* sock = m_NewSocket.release();
		this->GetListener()->OnSocketAccept( this, sock, from_endpoint(sock->m_Socket->GetRemoteEndpoint()));
	}
	if ( acceptor_.is_open() )
	{
		this->StartAccept();
	}
}

void TCPServerSocket::Close()
{
	this->acceptor_.cancel(m_LastError);
	this->acceptor_.close(m_LastError);
}

TCPServerSocket::~TCPServerSocket()
{
	this->Close();
}

PeerServerSocket::PeerServerSocket()
{

}

PeerServerSocket::~PeerServerSocket()
{

}

HTTPServerSocket::HTTPServerSocket()
{

}

HTTPServerSocket::~HTTPServerSocket()
{

}




void HTTPClientProxySocket::HttpRecv()
{
	boost::asio::async_read_until(
		m_Socket->GetSocket(), 
		request_, 
		boost::regex("\r\n\r\n"),
		boost::bind(
			&TCPSocketImpl::HandleLengthEvent, 
			m_Socket, 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, 
			TCPSocketImpl::LengthEventHandler(boost::bind( &HTTPClientProxySocket::HandleHttpHeader, this, _1, _2 ))
			)
		);
	m_ReceiveTimer.Start(this, this->m_ReceiveTimeout);
}

void HTTPClientProxySocket::HandleHttpHeader(const boost::system::error_code& err, size_t bytes_transferred)
{
	m_ReceiveTimer.Stop();
	if( ! err )
	{
		char* lpBuffer = new char[bytes_transferred];
		if ( ! lpBuffer)
			return;

		std::istream request_stream(&request_);
		request_stream.read(lpBuffer,bytes_transferred);
		if ( this->IsValid() )
		{
			this->GetListener()->OnSocketReceive(this, reinterpret_cast<BYTE*>(lpBuffer), bytes_transferred);
		}
	}
	else
	{
		if ( this->IsValid() )
		{
			Close();
			this->GetListener()->OnSocketReceiveFailed(this, err.value());
		}

	}
}

void HTTPClientProxySocket::HandleHttpTimeout(const boost::system::error_code& err)
{
	if( err != boost::asio::error::operation_aborted )
	{
		APP_EVENT("HttpServerSocket::HandleHttpTimeout ");
		this->Close();
	}
}


HTTPClientProxySocket::HTTPClientProxySocket() : request_(HTTP_HEADER_MAX_SIZE)
{

}

void HTTPClientProxySocket::OnTimerElapsed( Timer* sender )
{
	if ( &m_ReceiveTimer == sender )
	{
		if ( this->IsValid() )
		{
			this->GetListener()->OnSocketReceiveFailed(this, boost::asio::error::timed_out);
		}
	}
	else
	{
		TCPClientSocket::OnTimerElapsed(sender);
	}
}

HTTPClientProxySocket::~HTTPClientProxySocket()
{

}