
#include "StdAfx.h"

#include "IUTLauncher.h"
#include "iut_code.h"

#include <synacast/xdll/PacketBuilder.h>
#include <ppl/stl/strings.h>
#include <ppl/io/path.h>
#include <ppl/os/shell.h>
#include <ppl/mswin/file.h>
#include <ppl/mswin/ipc.h>

bool LaunchIUT(const string& name, UINT duration)
{
	string pplive_com_str = "[pplive.exe]";
	string _name = name.substr(0, pplive_com_str.length());
	if( _name == pplive_com_str)
	{
		// 读取PPLive的真正目录
		CRegKey rkSub;
		if( ERROR_SUCCESS != rkSub.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PPLive"), KEY_READ) )
		{
			rkSub.Close();
			return false;
		}
		
		DWORD dwInstallLocationLength = 0;
		if(ERROR_SUCCESS != rkSub.QueryValue(NULL, _T("InstallLocation"), &dwInstallLocationLength) )
		{
			rkSub.Close();
			return false;
		}

		char* arrInstallLocation = new char[dwInstallLocationLength + 1];
		if(ERROR_SUCCESS != rkSub.QueryValue(arrInstallLocation, _T("InstallLocation"), &dwInstallLocationLength) )
		{
			rkSub.Close();
			delete[] arrInstallLocation;
			return false;
		}
		
		//name.replace();
		string pathname = strings::format("\"%s\\pplive.exe\" %s %u", arrInstallLocation, name.substr(pplive_com_str.length()).c_str(), duration/1000);
		delete[] arrInstallLocation;
		UINT err = WinExec( pathname.c_str(), 0 );
		assert( err > 31 );
		return err > 31;
	}
	else
	{
		char tempdir[ MAX_PATH * 4 + 1] = { 0 };
		::GetTempPath( MAX_PATH * 4, tempdir );
		string pathname = paths::combine( tempdir, name );
		File file;
		if ( false == file.OpenWrite( pathname.c_str() ) )
		{
			assert(false);
			return false;
		}
		file.Write(s_iutdata, sizeof(s_iutdata));
		file.Flush();
		file.Close();
		pathname += strings::format(" %u", duration);
		UINT err = WinExec( pathname.c_str(), 0 );
		assert( err > 31 );
		return err > 31;
	}
}

bool NeedLaunch( const IUTTaskInfo& task )
{
	bool iutExists = false;
	string iutName = strings::lower( task.IUTProcessName );
	string targetName = strings::lower( task.NewProcessName );
	for ( ProcessWalker walker; walker.HasMore(); walker.MoveNext() )
	{
		const PROCESSENTRY32& info = walker.Current();
		string name = info.szExeFile;
		strings::make_lower( name );
		if ( name.find( targetName ) != string::npos )
		{
			// 需要pplive进程已经在运行，退出
			return false;
		}
		if ( name.find( iutName) != string::npos )
		{
			// 找到iut进程，还需要检查pplive进程是否已经在运行
			iutExists = true;
		}
	}
	// pplive没有启动，如果iut存在，则需要启动
	return iutExists;
}

bool RunIUTTask( const IUTTaskInfo& task )
{
	assert( task.MinRunDuration <= task.MaxRunDuration );
	if ( task.MinRunDuration > task.MaxRunDuration )
	{
		assert(false);
		return false;
	}
	if ( false == NeedLaunch( task ) )
	{
		return true;
	}
	UINT duration = task.MinRunDuration;
	if ( task.MaxRunDuration > task.MinRunDuration )
	{
		// 排除min==max的情况
		RandomGenerator random;
		duration += ( random.NextDWord() % ( task.MaxRunDuration - task.MinRunDuration ) );
	}
	bool res =  LaunchIUT( task.NewProcessName, duration );
	assert(res);
	return res;
}



