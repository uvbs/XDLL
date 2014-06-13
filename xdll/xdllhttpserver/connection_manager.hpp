//
// connection_manager.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include "connection.hpp"


namespace http {
	namespace server {

		/// Manages open connections so that they may be cleanly stopped when the server
		/// needs to shut down.
		class connection_manager
			: private boost::noncopyable
		{
		public:
			///construct
			connection_manager(boost::asio::io_service& io_service);

			/// start the module
			void start_all();

			/// Stop all connections.
			void stop_all();

			/// Add the specified connection to the manager and start it.
			void start(connection_ptr c);

			/// Do Request
			void do_request(connection_ptr c);

			/// Stop the specified connection.
			void stop(connection_ptr c);

		protected:
			void handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd);
			void handle_send_to(const boost::system::error_code& error, size_t bytes_sent);
			void handle_elapsed(const boost::system::error_code& error);

		private:
			/// Send Order ID
			int order;
			/// The managed connections.
			std::map<int, connection_ptr> connections_;
			
			///
			boost::asio::io_service& io_service_;

			/// timer
			boost::asio::deadline_timer timer_;

			/// udp
			boost::asio::ip::udp::socket udp_;
			boost::asio::ip::udp::endpoint remote_endpoint_;
			boost::asio::ip::udp::endpoint receive_endpoint_;
			char data_[2048];
		};

	} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_MANAGER_HPP
