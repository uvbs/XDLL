#include "stdafx.h"
#include "TimeControl.h"

#include <ppl/mswin/module.h>
#include <ppl/mswin/FileFinder.h>
#include <ppl/io/path.h>
#include "ppl/util/log.h"

TimeControl g_timeControl;

void TimeControlEntry::MakeValue(int dayDistCount,int values[])
{
	long long beforeSumTimeValue = 0;
	//int values2[144];
	for( int i = 0; i < 144; i ++)
	{
		beforeSumTimeValue += (long long)TimeValues[i];
		values[i] = (int)((long long)beforeSumTimeValue*(long long)dayDistCount / (long long)Sum);
		//values2[i] = values[i];
	}
}

void TimeControl::Load()
{
	string searchPattern = Module().BuildLocalFilePath(_T("TimeControl\\*.txt"));

	for ( FileFinder ff( searchPattern.c_str() ); ff.HasMore(); ff.MoveNext() )
	{
		string filename = ff.Current().cFileName;
		string basename = paths::get_basename(filename);
		int timeControlType;
		try { timeControlType = boost::lexical_cast<int>(basename); } catch (...){ APP_ERROR("TimeControl::Load Error "<<filename); continue; }

		string fullfilename = Module().BuildLocalFilePath(string("TimeControl\\")+filename);

		// 读取文件
		std::ifstream ifs(fullfilename.c_str());
		string line;
		int i = 0, v = 0;
		TimeControlEntry tce;

		tce.TimeControlType = timeControlType;
		// 添加和更新任务
		while( std::getline(ifs, line) && i < 144 )
		{
			// 读取成功
			try { v = boost::lexical_cast<int>(line); } catch (...){ APP_ERROR("TimeControl::Load Error "<<filename); continue; }
			tce.TimeValues[i++] = v;
			tce.Sum += v;
		}
		if( tce.Sum == 0 ) tce.Sum = 1;

		m_timeValues[timeControlType] = tce;
	}
}

bool TimeControl::MakeValue(int timeControlType,int dayDistCount,int values[])
{
	map<int,TimeControlEntry>::iterator iter = m_timeValues.find(timeControlType);
	if( iter == m_timeValues.end() )
		return false;
	iter->second.MakeValue(dayDistCount, values);
	return true;
}