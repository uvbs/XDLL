
#pragma once

#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>


class TCPSocketImpl : private boost::noncopyable, public boost::enable_shared_from_this<TCPSocketImpl>
{
public:
	typedef boost::function<void (const error_code&)> EventHandler;
	typedef boost::function<void (const error_code&, size_t)> LengthEventHandler;

	TCPSocketImpl() : m_impl( IOService::Instance() ), m_closed( false )//, m_test(false)
	{

	}

	static int GetPendingConnectionCount() { return m_PendingConnections; }
	static void IncPendingConnectionCount()
	{
		++m_PendingConnections;
	}

	tcp::socket& GetSocket() { return m_impl; }
	tcp::endpoint& GetRemoteEndpoint() { return m_remote_endpoint; }

	void HandleConnectEvent( const error_code& error, EventHandler handler )
	{
		assert(m_PendingConnections > 0);
		--m_PendingConnections;
//		TRACE("TCPSocketImpl::HandleEvent %p %d\n", this, m_closed);
		if ( m_closed )
			return;
		handler(error);
	}

	void HandleEvent( const error_code& error, EventHandler handler )
	{
//		TRACE("TCPSocketImpl::HandleEvent %p %d\n", this, m_closed);
		if ( m_closed )
			return;
		handler(error);
	}

	void HandleLengthEvent( const error_code& error, size_t bytes, LengthEventHandler handler )
	{
		//assert( false == m_test );
//		TRACE("TCPSocketImpl::HandleLengthEvent %p %d %d\n", this, m_closed, m_test);
		//m_test = true;
		if ( m_closed )
		{
			//m_test = false;
			return;
		}
		handler(error, bytes);
		//m_test = false;
	}

	void Close()
	{
		m_closed = true;
//		TRACE("TCPSocketImpl::Close %p\n", this);
	}

private:
	tcp::socket m_impl;
	tcp::endpoint m_remote_endpoint;
	bool m_closed;
	//bool m_test;

	static int m_PendingConnections;
};



class UDPSocketImpl : private boost::noncopyable, public boost::enable_shared_from_this<UDPSocketImpl>
{
public:
	typedef boost::function<void (const error_code&, size_t)> LengthEventHandler;

	UDPSocketImpl() : m_impl( IOService::Instance() ), m_closed( false )
	{

	}

	udp::socket& GetSocket() { return m_impl; }


	void HandleReceive( const error_code& error, size_t bytes, LengthEventHandler handler )
	{
		//		TRACE("TCPSocketImpl::HandleLengthEvent %p %d %d\n", this, m_closed, m_test);
		if ( m_closed )
		{
			return;
		}
		handler(error, bytes);
	}

	void HandleSend(const error_code& error, size_t bytes) {

	}

	void Close()
	{
		m_closed = true;
		//		TRACE("TCPSocketImpl::Close %p\n", this);
	}

private:
	udp::socket m_impl;
	bool m_closed;
};


