//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "header.hpp"

namespace http {
	namespace server {

		/// A request received from a client.
		struct request
		{
			std::string method;
			std::string uri;
			int http_version_major;
			int http_version_minor;
			std::vector<header> headers;

			int length;
			char body[1024];
			int index;

			request()
				: http_version_minor(0), http_version_major(0), length(0), index(0)
			{
			}

			inline bool parse_header()
			{
				for(std::vector<header>::iterator iter = headers.begin();
					iter != headers.end(); iter ++)
				{
					header& h = *iter;
					if (h.name == "Content-Length")
					{
						length = boost::lexical_cast<int>(h.value);
						return true;
					}
				}
				return false;
			}
		};

	} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP
