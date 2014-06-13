
#pragma once


#include <synacast/util/commanddispatcher.h>
#include <ppl/io/readline.h>
#include <ppl/stl/strings.h>
#include <ppl/util/macro.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <map>

#include <vector>
#include <string>


using namespace std;

typedef boost::function<void (vector<string>)> command_callback_type;

class command_line_loop
{
public:
	typedef std::map<string, command_callback_type> callback_collection;

	command_line_loop()
	{
		this->register_command( "help", boost::bind( &command_line_loop::on_help, this ) );
	}

	void run(command_dispatcher& cmddisp)
	{
		on_help();
		for ( ;; )
		{
			string line = LineReader::ReadLine();
			vector<string> args0;
			vector<string> args;

			strings::split(std::back_inserter(args0), line, ' ');
			if ( args0.empty() )
			{
				continue;
			}
			args.reserve(args0.size());
			for ( size_t index = 0; index < args0.size(); ++index )
			{
				string s = strings::trim(args0[index]);
				if ( false == s.empty() )
				{
					args.push_back( s );
				}
			}
			if ( args.empty() )
			{
				printf("error : no command\n");
				continue;
			}
			string command = args[0];
			//printf("%s\n", command.c_str());
			if( command == "exit" )
			{
				//MainThread::IOS().post(boost::bind(&UdpTrackerModule::Stop, UdpTrackerModule::Inst()));
				//DataThread::IOS().post(boost::bind(&DataWriter::Stop, DataWriter::Inst()));
				break;
			}
			callback_collection::const_iterator iter = m_callbacks.find( command );
			if ( iter != m_callbacks.end() )
			{
				cmddisp.post_command( boost::bind( iter->second, args ) );
			}
			else
			{
				printf("unrecognized command : %s\n", command.c_str());
			}
		}
	}


	bool register_command( const string& cmd, command_callback_type callback )
	{
		if ( cmd.empty() )
		{
			assert( false );
			return false;
		}
		if ( "exit" == cmd )
		{
			assert( false );
			return false;
		}
		m_callbacks[cmd] = callback;
		return true;
	}
	bool unregister_command( const string& cmd )
	{
		if ( cmd.empty() )
		{
			assert( false );
			return false;
		}
		if ( "exit" == cmd )
		{
			assert( false );
			return false;
		}
		m_callbacks.erase(cmd);
		return true;
	}
	void on_help()
	{
		printf("error : no command\n");
		return;
		puts("Command: (case sentive)");
		puts("  exit");
		STL_FOR_EACH_CONST( callback_collection, m_callbacks, iter )
		{
			printf("  %s\n", iter->first.c_str());
		}
	}

private:
	callback_collection m_callbacks;
};




