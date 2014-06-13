// xdllserver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <synacast/asio/runner.h>
#include <synacast/asio/commanddispatcher.h>
#include <synacast/util/CommandLineLoop.h>
#include <synacast/asio/logservice.h>
#include <ppl/mswin/module.h>
#include "TipsStatistic.h"

#include <synacast/xdll/TimeUtils.h>
#include <ppl/util/time.h>
#include "ppl/util/ini.h"

#include "global.h"

UINT32	g_UdpPort = 8000;
UINT32	g_IntervalTime = 2*60*60;		// 默认UDP请求间隔时间 2小时

void LoadCongig()
{
	string baseDir = Module().GetFileDirectory();
	string iniFilename = paths::combine(baseDir, _T("TipsStatistic.ini"));
	IniFile ini;
	ini.SetFileName(iniFilename);
	ini.SetSection("Root");

	g_UdpPort = ini.GetInteger("UdpPort", 8000);
	g_IntervalTime = ini.GetInteger("IntervalTime", 2*60*60);
}


int _tmain(int argc, _TCHAR* argv[])
{
	printf("TipsStatistic\n");

	LogService::Instance().Init( Module().BuildLocalFilePath("log"), "TipsStatistic" );
	LogService::Instance().Start();
	LogService::Instance().LogOn( PPL_LOG_TYPE_APP );
	IOServiceRunner::Instance().Start();
	io_service_command_dispatcher dispatcher;
	command_line_loop cmdloop;

	LoadCongig();

	TipsStatistic server;
	cmdloop.register_command("show", boost::bind(&TipsStatistic::UserShow, &server, _1));
	if ( false == server.Start() )
	{
		APP_ERROR("failed to start server");
		return -1;
	}
	cmdloop.run( dispatcher );
	printf("exit...\n");
	server.Stop();
	IOServiceRunner::Instance().Stop(1000);
	LogService::Instance().Stop();
	return 0;
}