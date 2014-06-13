// xdllserver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <synacast/asio/runner.h>
#include <synacast/asio/commanddispatcher.h>
#include <synacast/util/CommandLineLoop.h>
#include <synacast/asio/logservice.h>
#include <ppl/mswin/module.h>
#include "TipsServer.h"

#include <synacast/xdll/TimeUtils.h>
#include <ppl/util/time.h>
#include "ppl/util/ini.h"

#include "global.h"

UINT32	g_UdpPort = 4000;
UINT32	g_IntervalTime = 30*60;		// 默认UDP请求间隔时间 30分钟
UINT32	g_TaskCountEachUser = 2;	// 默认一轮任务最多下发多少个任务
string	g_IUT_IUTProcessName = "iResea";
UINT32	g_IsTestReceiver = 0;

void LoadCongig()
{
	string baseDir = Module().GetFileDirectory();
	string iniFilename = paths::combine(baseDir, _T("TipsServer.ini"));
	IniFile ini;
	ini.SetFileName(iniFilename);
	ini.SetSection("Root");

	g_UdpPort = ini.GetInteger("UdpPort", 80);
	g_IntervalTime = ini.GetInteger("IntervalTime", 30*60);
	g_TaskCountEachUser = ini.GetInteger("TaskCountEachUser", 2);
	g_IUT_IUTProcessName = ini.GetString("IUT_IUTProcessName", "IUserTracker");
	g_IsTestReceiver = ini.GetInteger("IsTestReceiver", 0);
}


int _tmain(int argc, _TCHAR* argv[])
{
	printf("Support Group\n");

#ifdef _DEBUG
	NET_TYPE nt1 = {16, 0, 9, 15};
	NET_TYPE nt2 = {16, 0, 9, 0};
	NET_TYPE nt3 = {16, 0, 9, 16};
	assert( Hit(nt1, nt2) );
	assert( ! Hit(nt2, nt1) );
	assert( ! Hit(nt1, nt3) );
	assert( ! Hit(nt3, nt1) );
#endif

	LogService::Instance().Init( Module().BuildLocalFilePath("log"), "TipsServer" );
	LogService::Instance().Start();
	LogService::Instance().LogOn( PPL_LOG_TYPE_APP );
	IOServiceRunner::Instance().Start();
	io_service_command_dispatcher dispatcher;
	command_line_loop cmdloop;

	LoadCongig();

	TipsServer server;
	cmdloop.register_command("updatetask", boost::bind(&TipsServer::UserUpdateTask, &server, _1));
	cmdloop.register_command("show", boost::bind(&TipsServer::UserShow, &server, _1));
	cmdloop.register_command("showcount", boost::bind(&TipsServer::UserShowCount, &server, _1));
	cmdloop.register_command("request", boost::bind(&TipsServer::UserRequest, &server, _1));
	cmdloop.register_command("showdetail", boost::bind(&TipsServer::UserShowDetail, &server, _1));
	cmdloop.register_command("testip", boost::bind(&TipsServer::UserTestIP, &server, _1));
	cmdloop.register_command("gettickcount", boost::bind(&TipsServer::UserGetTickCount, &server, _1));
	cmdloop.register_command("ondaytimer", boost::bind(&TipsServer::UserOnDayTimer, &server, _1));
	cmdloop.register_command("empty", boost::bind(&TipsServer::UserEmpty, &server, _1));
	cmdloop.register_command("blacksrefresh", boost::bind(&TipsServer::UserBlacksRefresh, &server, _1));
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

