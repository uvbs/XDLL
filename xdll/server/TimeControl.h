#pragma once

struct TimeControlEntry
{
	int TimeControlType;
	int TimeValues[144];
	int Sum;

	TimeControlEntry() : TimeControlType(0), Sum(0) { for(int i=0; i<144; i++) TimeValues[i]=0; }
	void MakeValue(int dayDistCount,int values[]);
};

class TimeControl : private boost::noncopyable
{
public:
	map<int,TimeControlEntry> m_timeValues;

	void Load();
	bool MakeValue(int timeControlType,int dayDistCount,int values[]);
};

extern TimeControl g_timeControl;