
#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp" 
#include <sstream>


class TimeUtils
{
public:
	static long GetSecondOfDay()
	{
		return boost::posix_time::second_clock::local_time().time_of_day().total_seconds();
	}

	static string GetTodayShortString()
	{
		stringstream sstr;
		boost::gregorian::date d = boost::posix_time::second_clock::local_time().date();
		boost::gregorian::date::ymd_type ymd = d.year_month_day();
		sstr << (int)ymd.year << "-" << (int)ymd.month << "-" << (int)ymd.day;
		return sstr.str();
	}
};



