// xdllhttpserver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "global.h"
#include "server.hpp"

boost::function0<void> console_ctrl_function;

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
	switch (ctrl_type)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		console_ctrl_function();
		return TRUE;
	default:
		return FALSE;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		LoadConfig();

		boost::asio::io_service io_service;

		APP_EVENT("Application Start");

		// Initialise server.
		http::server::server s(io_service);

		if ( ! s.start() )
		{
			APP_ERROR("Http Tips Server Start Error");
			return 0;
		}

		APP_ERROR("Application Start Succeed");

		// Set console control handler to allow server to be stopped.
		console_ctrl_function = boost::bind(&http::server::server::stop, &s);
		SetConsoleCtrlHandler(console_ctrl_handler, TRUE);

		// Run the server until stopped.
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}

