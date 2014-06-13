//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"

#include "connection_manager.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include "global.h"

namespace http {
	namespace server {

		connection_manager::connection_manager(boost::asio::io_service& io_service)
			: order(0), io_service_(io_service), 
			udp_(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
			, remote_endpoint_(boost::asio::ip::address::from_string(g_remote_ip), g_remote_port)
			, timer_(io_service_)
		{
		}

		void connection_manager::start_all()
		{
			APP_EVENT("connection_manager::start_all");

			// create udp module
			for (int i = 0; i < 40; i ++)
			{
				udp_.async_receive_from(
					boost::asio::buffer(data_, 2048), receive_endpoint_,
					boost::bind(&connection_manager::handle_receive_from, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}

			// create clear timer
			timer_.expires_from_now(boost::posix_time::seconds(g_elsped_second));
			timer_.async_wait(boost::bind(&connection_manager::handle_elapsed, this, boost::asio::placeholders::error));
		}

		void connection_manager::start(connection_ptr c)
		{
			order ++;
			if (connections_.find(order) == connections_.end())
			{
				//  Í£Ö¹µô
			}
			connections_.insert(std::make_pair(order, c));
			c->order_ = order;
			c->start();
		}

		void connection_manager::stop(connection_ptr c)
		{
			int order = c->order_;
			connections_.erase(order);
			c->stop();
		}

		void connection_manager::stop_all()
		{
			APP_EVENT("connection_manager::stop_all");
			/*std::for_each(connections_.begin(), connections_.end(),
				boost::bind(&connection::stop, _1));*/

			for (std::map<int, connection_ptr>::iterator iter = connections_.begin(); iter != connections_.end(); iter ++)
			{
				connection_ptr c = iter->second;
				c->stop();
			}
			connections_.clear();

			// stop clear timer
			timer_.cancel();

			// stop udp module
			udp_.cancel();
		}

		/// Do Request
		void connection_manager::do_request(connection_ptr c)
		{
			// change the Seques SequenceID
			c->old_sequence_id_ = *(u_int*)(c->request_.body+3);
			c->send_udp_tick_ = ::GetTickCount();
			*(u_int*)(c->request_.body+3) = c->order_;

			APP_EVENT("connection_manager::do_reques ");

			// send udp packet
			udp_.async_send_to(
				boost::asio::buffer(c->request_.body, c->request_.length), remote_endpoint_,
				boost::bind(&connection_manager::handle_send_to, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}

		void connection_manager::handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
		{
			APP_EVENT("connection_manager::handle_receive_from ");
			if (!error && bytes_recvd > 0)
			{
				if (bytes_recvd < 8)
				{
					APP_ERROR("connection_manager::handle_receive_from error bytes_recvd is to short (" << bytes_recvd << ")");
					goto receive_next;
				}

				u_int order = *(u_int*)(data_+3);
				std::map<int, connection_ptr>::iterator iter = connections_.find(order);
				if (iter == connections_.end())
				{
					// ´íÎó
					goto receive_next;
				}
				
				connection_ptr c = iter->second;

				*(u_int*)(data_+3) = c->old_sequence_id_;

				c->reply_.set_body(data_, bytes_recvd);
				c->do_response();
			}
			else
			{
				APP_ERROR("connection_manager::handle_receive_from error " << error);
			}

			receive_next:
			udp_.async_receive_from(
				boost::asio::buffer(data_, 2048), receive_endpoint_,
				boost::bind(&connection_manager::handle_receive_from, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}

		void connection_manager::handle_send_to(const boost::system::error_code& error, size_t bytes_sent)
		{
			if (!error)
			{

			}
			else
			{
				APP_ERROR("connection_manager::handle_send_to error " << error);
			}
		}

		void connection_manager::handle_elapsed(const boost::system::error_code& error)
		{
			if(!error)
			{
				// clear timeout session
				int count = 0;
				u_int tick = ::GetTickCount();
				for (std::map<int, connection_ptr>::iterator iter = connections_.begin(); iter != connections_.end();)
				{
					connection_ptr c = iter->second;
					if (c->send_udp_tick_ + g_clear_second*1000 < tick)
					{
						APP_EVENT("erase connection " << c->order_);
						c->stop();
						connections_.erase(iter++);
						count ++;
					}
					else
					{
						iter ++;
					}
				}
				APP_EVENT("connection_manager::handle_elapsed all clear " << count << " connection.");
				
				timer_.expires_from_now(boost::posix_time::seconds(1));
				timer_.async_wait(boost::bind(&connection_manager::handle_elapsed, this, boost::asio::placeholders::error));
			}
			else
			{
				APP_ERROR("connection_manager::handle_elapsed error "<<error);
			}
		}

	} // namespace server
} // namespace http
