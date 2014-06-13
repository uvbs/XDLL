#pragma once

#include "ppl/util/guid.h"
#include <synacast/xdll/PacketBuilder.h>
#include "iptable.h"

const int UDP_MTU = 1440;

struct Task : private boost::noncopyable
{
	Guid	TaskGuid;				// �����Guid
	int		TaskType;				// ���������
	int		TaskGroupID;			// ����ķ���ID
	double	TaskGroupRate;

	int		DayDistCount;			// û�о���MTU����� ����Ŀ����
	int		DayMTUDistCount;		// ������MTU����� ����Ŀ����
									// ��νMTU���㣬�͸��ݰ��Ĵ�С���㶪���ʣ����յõ���Ӧ���·�������
									// ���Կ�MTU�Ĳ����Ϳ�����

	int		DayNowMTUCount;			// ��ǰ�Ѿ��·�����������
	int		DayStartMTULimit;		// ����������ʱ������ DayStartMTULimit�����0�����������ֵ����0
	int		DayNowMTULimit;			// ��ǰ���������ֵ���ϸ���ƣ�DayNowMTUCount < DayNowMTULimited

	int		DayLimitedValues[144];	// ����ֵ���飬ÿ10����һ��ֵ
	int		MTUCount;				// ���ı��ֳɼ��ΰ�
	TaskInfo*	lpTaskInfo;
	vector<NET_TYPE> AddressList;				// ֻ����Ͷ�ŵĵ���
	vector<NET_TYPE> ForbiddenAddressList;		// ������Ͷ�ŵĵ���
	set<short>	ChannelSet;
	set<short>	RequestTypeSet;
	u_int		MinSupportVersion;				// ��С֧�ֵ�Э����
	u_int		MinValue;						// ����֧����Сֵָ��
	u_int		MaxValue;						// ����֧�����ֵָ��
	u_int		TotalValue;						// ��ֵ�ƶ�
	u_int		TTEx;							// �Ƿ���������
	u_short		IsSupport64bit;					// �Ƿ�֧��64����ϵͳ

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