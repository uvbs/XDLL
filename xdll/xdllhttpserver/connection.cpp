//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
	namespace server {

		connection::connection(boost::asio::io_service& io_service,
			connection_manager& manager)
			: socket_(io_service),
			connection_manager_(manager),
			order_(0),
			send_udp_tick_(0),
			old_sequence_id_(0)
		{
		}

		boost::asio::ip::tcp::socket& connection::socket()
		{
			return socket_;
		}

		void connection::start()
		{
			socket_.async_read_some(boost::asio::buffer(buffer_),
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void connection::stop()
		{
			socket_.close();
		}

		void connection::handle_read(const boost::system::error_code& e,
			std::size_t bytes_transferred)
		{
			if (!e)
			{
				boost::tribool result;
				boost::tie(result, boost::tuples::ignore) = request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);

				if (result)
				{
					// 已经读取完成
					//request_handler_.handle_request(request_, reply_);

					if (request_.length < 8)
					{
						// 不正确，直接返回错误
						reply_ = reply::stock_reply(reply::forbidden);
						boost::asio::async_write(socket_, reply_.to_buffers(),
							boost::bind(&connection::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
						return;
					}

					connection_manager_.do_request(shared_from_this());
				}
				else if (!result)
				{
					// 不正确，直接返回错误
					reply_ = reply::stock_reply(reply::bad_request);
					boost::asio::async_write(socket_, reply_.to_buffers(),
						boost::bind(&connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error));
				}
				else
				{
					// 还未读完
					socket_.async_read_some(boost::asio::buffer(buffer_),
						boost::bind(&connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				}
			}
			else if (e != boost::asio::error::operation_aborted)
			{
				connection_manager_.stop(shared_from_this());
			}
		}

		void connection::do_response()
		{
			reply_.status = reply::ok;
			reply_.headers.resize(2);
			reply_.headers[0].name = "Content-Length";
			reply_.headers[0].value = boost::lexical_cast<std::string>(reply_.length);
			reply_.headers[1].name = "Content-Type";
			reply_.headers[1].value = "application/octet-stream";
			boost::asio::async_write(socket_, reply_.to_buffers_with_body(),
				boost::bind(&connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error));
		}

		void connection::handle_write(const boost::system::error_code& e)
		{
			if (!e)
			{
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			}

			if (e != boost::asio::error::operation_aborted)
			{
				connection_manager_.stop(shared_from_this());
			}
		}

	} // namespace server
} // namespace http
