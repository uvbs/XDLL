//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"

#include "server.hpp"
#include <boost/bind.hpp>
#include "global.h"

namespace http {
	namespace server {

		server::server(boost::asio::io_service& io_service)
			: io_service_(io_service),
			acceptor_(io_service_),
			connection_manager_(io_service),
			new_connection_(new connection(io_service_, connection_manager_))
		{
			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
		}

		bool server::start()
		{
			//boost::asio::ip::tcp::resolver resolver(io_service_);
			//boost::asio::ip::tcp::resolver::query query("0.0.0.0", 8000);
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), g_tcp_port); // = *resolver.resolve(query);
			acceptor_.open(endpoint.protocol());
			acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			boost::system::error_code ec;
			acceptor_.bind(endpoint, ec);
			if (ec)
			{
				APP_ERROR("server::start() acceptor_.bind error ec=" << ec);
				return false;
			}
			acceptor_.listen();
			acceptor_.async_accept(new_connection_->socket(),
				boost::bind(&server::handle_accept, this,
				boost::asio::placeholders::error));

			connection_manager_.start_all();

			return true;
		}

		void server::stop()
		{
			// Post a call to the stop function so that server::stop() is safe to call
			// from any thread.
			io_service_.post(boost::bind(&server::handle_stop, this));
		}

		void server::handle_accept(const boost::system::error_code& e)
		{
			if (!e)
			{
				connection_manager_.start(new_connection_);
				new_connection_.reset(new connection(io_service_, connection_manager_));
				acceptor_.async_accept(new_connection_->socket(), boost::bind(&server::handle_accept, this, boost::asio::placeholders::error));
			}
		}

		void server::handle_stop()
		{
			// The server is stopped by cancelling all outstanding asynchronous
			// operations. Once all operations have finished the io_service::run() call
			// will exit.
			acceptor_.close();
			connection_manager_.stop_all();
		}

	} // namespace server
} // namespace http
