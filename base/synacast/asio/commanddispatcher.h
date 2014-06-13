
#pragma once

#include <synacast/util/commanddispatcher.h>
#include <synacast/asio/ioservice.h>

class IOServiceCommandDispatcher : public CommandDispatcher
{
public:
	IOServiceCommandDispatcher()
	{

	}

	virtual void PostCommand(boost::function<void ()> fun)
	{
		IOService::Instance().post(fun);
	}
};


class io_service_command_dispatcher : public command_dispatcher
{
public:
	io_service_command_dispatcher()
	{

	}

	virtual void post_command(boost::function<void ()> fun)
	{
		IOService::Instance().post(fun);
	}
};
