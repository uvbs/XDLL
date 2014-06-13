#include "stdafx.h"
#include "ppl/mswin/module.h"
#include "ppl/util/ini.h"

#include "global.h"

int	g_tcp_port;
string	g_remote_ip;
int	g_remote_port;
int	g_clear_second;
int g_elsped_second;

void LoadConfig()
{
	string base_dir = Module().GetFileDirectory();
	string ini_filename = paths::combine(base_dir, _T("HttpServer.ini"));
	IniFile ini;
	ini.SetFileName(ini_filename);
	ini.SetSection("root");

	g_tcp_port = ini.GetInteger("tcp_port", 80);
	g_remote_ip = ini.GetString("remote_ip", "127.0.0.1");
	g_remote_port = ini.GetInteger("remote_port", 4000);
	g_clear_second = ini.GetInteger("clear_second", 15);
	g_elsped_second = ini.GetInteger("elsped_second", 1);
}
