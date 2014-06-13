
#pragma once

#include <boost/asio/io_service.hpp>
using boost::asio::io_service;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;


typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;

class IOService
{
public:
	static io_service& Instance()
	{
		return *InstancePtr();
	}
	static io_service_ptr InstancePtr()
	{
		return RefInstancePtr();
	}

	static io_service_ptr& RefInstancePtr()
	{
		static io_service_ptr theService( new io_service);
		return theService;
	}

};


