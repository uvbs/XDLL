#pragma once

#include "ppl/util/guid.h"
#include <synacast/xdll/PacketBuilder.h>
#include "iptable.h"

const int UDP_MTU = 1440;

struct Task : private boost::noncopyable
{
	Guid	TaskGuid;				// 任务的Guid
	int		TaskType;				// 任务的类型
	int		TaskGroupID;			// 任务的分组ID
	double	TaskGroupRate;

	int		DayDistCount;			// 没有经过MTU换算的 当日目标量
	int		DayMTUDistCount;		// 经过了MTU换算的 当日目标量
									// 所谓MTU换算，就根据包的大小计算丢包率，最终得到的应该下发的数量
									// 所以看MTU的参数就可以了

	int		DayNowMTUCount;			// 当前已经下发的任务数量
	int		DayStartMTULimit;		// 程序启动的时候计算的 DayStartMTULimit，如果0点启动，这个值就是0
	int		DayNowMTULimit;			// 当前当天的限制值，严格控制，DayNowMTUCount < DayNowMTULimited

	int		DayLimitedValues[144];	// 限制值数组，每10分钟一个值
	int		MTUCount;				// 报文被分成几段啊
	TaskInfo*	lpTaskInfo;
	vector<NET_TYPE> AddressList;				// 只允许投放的地域
	vector<NET_TYPE> ForbiddenAddressList;		// 不允许投放的地域
	set<short>	ChannelSet;
	set<short>	RequestTypeSet;
	u_int		MinSupportVersion;				// 最小支持的协议编号
	u_int		MinValue;						// 概率支持最小值指定
	u_int		MaxValue;						// 概率支持最大值指定
	u_int		TotalValue;						// 总值制定
	u_int		TTEx;							// 是否特殊任务
	u_short		IsSupport64bit;					// 是否支持64操作系统

private:
	Task();
public:
	~Task();

public:
	static Guid GetGuidFormString(const string& line);
	static Task* MakeFormString(const string& line);
	bool UpdateFromString(const string& line);
	string GeneralStatusString();
	string GeneralShowString();
	static int MakeMTU(int count, int mtuCount);
	static int FromMTU(int count, int mtuCount);
	void UserShow();
	void UserShowDetail();
	int GetNowLimit(long secondOfDay);
	void LoadAddressListFromLine(const string& line);

	int GetCountRate();
	int GetCountSimRate();
};

struct TaskStatus
{
	Guid TaskGuid;
	int DayNowMTUCount;
	string Day;	
	TaskStatus();
	bool IsToday();
	static bool MakeFormString(const string& line, TaskStatus& taskStatus);
};