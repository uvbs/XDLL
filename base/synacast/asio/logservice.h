
#pragma once

#include <ppl/config.h>

#include <synacast/asio/runner.h>
#include <synacast/asio/timer.h>

#include <ppl/io/stdfile.h>
#include <ppl/io/path.h>
#include <ppl/io/fs.h>
#include <ppl/os/module.h>
#include <ppl/stl/stlutils.h>
#include <ppl/util/TimeCounter.h>
#include <ppl/util/time.h>
#include <ppl/std/inttypes.h>
#include <ppl/util/log.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using boost::asio::io_service;

#include <fstream>
#include <assert.h>

#if defined(_PPL_PLATFORM_LINUX)
#include <ext/hash_map>
using __gnu_cxx::hash_map;
#else
#include <hash_map>
using stdext::hash_map;
#endif

class LogService : private boost::noncopyable
{
protected:
	LogService() 
		: m_ioServiceRunner( new IOServiceRunner( boost::shared_ptr<io_service>( new io_service ) ) )
		, m_logLevel( 0)
		, m_dirty( false ) 
	{
		m_maxLogSize = 512 * 1024 * 1024;
		m_timer.reset( new PeriodicTimer( m_ioServiceRunner->GetService() ) );
		m_timer->SetListener( boost::bind( &LogService::OnTimer, this ) );
	}

public:
	typedef hash_map<ULONG, string> value_string_table;

	static LogService& Instance()
	{
		static LogService theLog;
		return theLog;
	}

	/// 非线程安全，请在启动日志模块之前调用
	void Init( const string& logDir, const string& logName )
	{
		m_dir = logDir;
		m_name = logName;
		file_system::ensure_directory_exists( m_dir.c_str() );
		//assert( file_system::directory_exists( m_dir.c_str() ) );
	}


	bool Start()
	{
		if ( m_dir.empty() || m_name.empty() )
		{
			assert( false );
			return false;
		}
		if ( false == OpenLog( date_time::now() ) )
		{
			return false;
		}
		m_ioServiceRunner->Start();
		RegisterLevel( _PPL_INFO, "INFO" );
		RegisterLevel( _PPL_EVENT, "EVENT" );
		RegisterLevel( _PPL_WARN, "WARN" );
		RegisterLevel( _PPL_ERROR, "ERROR" );
		RegisterLevel( _PPL_DEBUG, "DEBUG" );
		RegisterType( PPL_LOG_TYPE_APP, "APP" );
		RegisterType( PPL_LOG_TYPE_UTIL, "UTIL" );
		RegisterType( PPL_LOG_TYPE_LOGCORE, "LOG" );
		m_timer->Start( 3000 );
		return true;
	}
	void Stop()
	{
		//m_fout.Close();
		m_timer->Stop();
		m_ioServiceRunner->Stop();
	}

	void RegisterType( ULONG typeval, const string& typestr )
	{
		m_ioServiceRunner->GetService().post( boost::bind( &LogService::OnRegisterType, this, typeval, typestr) );
	}

	void RegisterLevel( ULONG levelval, const string& levelstr )
	{
		m_ioServiceRunner->GetService().post( boost::bind( &LogService::OnRegisterLevel, this, levelval, levelstr) );
	}

	void SetLogLevel( ULONG logLevel )
	{
		m_logLevel = logLevel;
	}
	void LogOn( ULONG logType )
	{
		this->SetLogType( logType, true );
	}
	void LogOff( ULONG logType )
	{
		this->SetLogType( logType, false );
	}

	void SetLogType( ULONG type, bool isLogOn )
	{
		m_ioServiceRunner->GetService().post( boost::bind( &LogService::OnSetLogType, this, type, isLogOn ) );
	}

	void PostLog( unsigned long type, unsigned long level, string text )
	{
		SYSTEMTIME sysTime;
		::GetLocalTime( &sysTime );
		m_ioServiceRunner->GetService().post( boost::bind( &LogService::OnLog, this, sysTime, type, level, text ) );
	}

	bool CanLog( unsigned long type, unsigned long level )
	{
		//if ( false == m_fout.IsOpen() )
		//	return false;
		if ( level < m_logLevel )
			return false;
		return containers::contains( m_allowedTypes, type );
	}


protected:
	void OnLog( SYSTEMTIME sysTime, unsigned long type, unsigned long level, string text )
	{
		//if ( false == m_fout.IsOpen() )
		//	return;
		if ( level < m_logLevel )
			return;
		if ( false == containers::contains( m_allowedTypes, type ) )
			return;

		date_time dt = date_time::now();
		string timestr = dt.str();
		string typestr = GetValueString( type, m_typeStrings );
		string levelstr = GetValueString( level, m_levelStrings );
		Output( stdout, timestr, typestr, levelstr, text );
		m_dirty = true;
		//if ( m_fout.IsOpen() )
		//{
		//	Output( m_fout.GetHandle(), timestr, typestr, levelstr, text );
		//	if ( m_lastFlushTime.GetElapsed32() > 5 * 1000 )
		//	{
		//		// 1分钟flush一次
		//		m_dirty = false;
		//		m_fout.Flush();
		//		m_lastFlushTime.Sync();
		//		if ( m_fout.GetSize() > m_maxLogSize )
		//		{
		//			this->OpenLog( dt );
		//		}
		//	}
		//}
	}
	void Output( FILE* fp, const string& timestr, const string& type, const string& level, const string& text )
	{
		fprintf( fp, "%s %-5s %5s %s\n", timestr.c_str(), type.c_str(), level.c_str(), text.c_str() );
	}

	void OnTimer()
	{
		if ( m_dirty )
		{
			m_dirty = false;
			//m_fout.Flush();
			m_lastFlushTime.Sync();
		}
	}

	bool OpenLog( const date_time& dt )
	{
		m_dirty = false;
		string timestr = dt.format( "_%04u%02u%02u_%02u%02u%02u" );
		string filename = m_name + timestr + ".log";
		string filepath = paths::combine( m_dir, filename );
		//m_fout.Close();
		//if ( false == m_fout.OpenText(filepath.c_str()) )
		//{
		//	printf("failed to open log file %s\n", filepath.c_str());
		//	//assert(false);
		//	return false;
		//}
		return true;
	}

	void OnSetLogType( ULONG type, bool isLogOn )
	{
		if ( isLogOn )
		{
			m_allowedTypes.insert( type );
		}
		else
		{
			m_allowedTypes.erase( type );
		}
	}

	void OnRegisterType( ULONG typeval, string typestr )
	{
		m_typeStrings[ typeval ] = typestr;
	}

	void OnRegisterLevel( ULONG levelval, string levelstr )
	{
		m_levelStrings[ levelval ] = levelstr;
	}

	static string GetValueString( ULONG val, const value_string_table& strs )
	{
		value_string_table::const_iterator iter = strs.find( val );
		if ( iter != strs.end() )
			return iter->second;
		return strings::format( "%lu", val );
	}

private:
	boost::shared_ptr<IOServiceRunner> m_ioServiceRunner;
	value_string_table m_typeStrings;
	value_string_table m_levelStrings;
	//StdFileWriter m_fout;
	TimeCounter m_lastFlushTime;
	ULONG m_logLevel;
	std::set<ULONG> m_allowedTypes;

	string m_dir;
	string m_name;
	size_t m_maxLogSize;
	bool m_dirty;

	boost::shared_ptr<PeriodicTimer> m_timer;
};


inline void log_impl(unsigned long type, unsigned long level, const string& text)
{
	LogService::Instance().PostLog( type, level, text );
}

