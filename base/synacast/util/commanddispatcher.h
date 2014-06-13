
#pragma once

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>


class CommandDispatcher : private boost::noncopyable
{
public:
	CommandDispatcher() { }
	virtual ~CommandDispatcher() { }

	virtual void PostCommand(boost::function<void ()> fun) = 0;
};




class command_dispatcher : private boost::noncopyable
{
public:
	command_dispatcher() { }
	virtual ~command_dispatcher() { }

	virtual void post_command( boost::function<void ()> fun ) = 0;
};


