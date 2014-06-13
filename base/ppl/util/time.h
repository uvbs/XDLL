
#pragma once

#include <ppl/stl/strings.h>
#include <ppl/std/inttypes.h>
#include <ppl/util/macro.h>
#include <ppl/stl/strings.h>

#include <sys/timeb.h>


/// 一天有多少个100ns
const UINT64 nanoseconds_100_per_day = 24ULL * 60 * 60 * 1000 * 1000 * 10;



//! Class to provide simple basic formatting rules
template<class charT>
class simple_date_time_format {
public:
	//! Char to sep?
	static charT year_sep_char()
	{
		return '-';
	}
	//! char between year-month
	static charT month_sep_char()
	{
		return '-';
	}
	//! Char to separate month-day
	static charT day_sep_char()
	{
		return '-';
	}
	//! char between date-hours
	static charT hour_sep_char()
	{
		return ' ';
	}
	//! char between hour and minute
	static charT minute_sep_char()
	{
		return ':';
	}
	//! char for second
	static charT second_sep_char()
	{
		return ':';
	}

	//! char for second
	static charT millisecond_sep_char()
	{
		return '.';
	}

};




class date_time
{
public:
	date_time()
	{
		FILL_ZERO( m_date );
		FILL_ZERO( m_time );
	}

	int year() const
	{
		return m_date.tm_year + 1900;
	}
	int month() const
	{
		return m_date.tm_mon + 1;
	}
	int day() const
	{
		return m_date.tm_mday;
	}
	int hour() const
	{
		return m_date.tm_hour;
	}
	int minute() const
	{
		return m_date.tm_min;
	}
	int second() const
	{
		return m_date.tm_sec;
	}
	int millisecond() const
	{
		return m_time.millitm;
	}

	string str() const
	{
		return format( "%04u-%02u-%02u %02u:%02u:%02u.%03u" );
	}

	string format( const char* formatStr ) const
	{
		return strings::format( 
			formatStr, 
			year(), 
			month(), 
			day(), 
			hour(), 
			minute(), 
			second(), 
			millisecond() );
	}

	string format_date() const
	{
		return strings::format( "%04u-%02u-%02u", year(), month(), day() );
	}

	bool parse_date( const char* s )
	{
		int year = 0;
		int month = 0;
		int day = 0;
		int len = sscanf( s, "%4u-%2u-%2u", &year, &month, &day );
		date_time dt;
		if ( 3 != len )
			return false;
		FILL_ZERO( m_date );
		FILL_ZERO( m_time );
		m_date.tm_year = year - 1900;
		m_date.tm_mon = month - 1;
		m_date.tm_mday = day;
		m_time.time = mktime( &m_date );
		return true;
	}

	static date_time now()
	{
		date_time dt;
		ftime( &dt.m_time );
		tm* t = localtime( &dt.m_time.time );
		if ( t != NULL )
		{
			dt.m_date = *t;
		}
		return dt;
	}

private:
	struct tm m_date;
	struct timeb m_time;
};


/*
class times
{
public:
	static const char* time_format_string()
	{
		return "%04hu-%02hu-%02hu %02hu:%02hu:%02hu.%03hu";
	}
	static bool write_time_string( const SYSTEMTIME& systime, FILE* fout )
	{
		return 0 != fprintf( 
			fout, 
			time_format_string(), 
			systime.wYear, systime.wMonth, systime.wDay, 
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds );
	}
	static string FormatTime(const SYSTEMTIME& systime)
	{
		return strings::format(
			time_format_string(), 
			systime.wYear, systime.wMonth, systime.wDay, 
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
	}

	static string GetLocalTimeString()
	{
		SYSTEMTIME systime;
		::GetLocalTime(&systime);
		return FormatTime(systime);
	}


	static UINT64 current_filetime()
	{
		FILETIME ft = { 0 };
		SYSTEMTIME systime = { 0 };
		::GetLocalTime( &systime );
		if ( FALSE == ::SystemTimeToFileTime( &systime, &ft ) )
			return 0;
		return make_uint64( ft.dwHighDateTime, ft.dwLowDateTime );
	}
};
*/

