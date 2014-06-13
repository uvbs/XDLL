
#pragma once

#include <boost/noncopyable.hpp>
#include <synacast/asio/timer.h>
#include <synacast/asio/socket.h>
#include <synacast/xdll/PacketBuilder.h>
#include "Task.h"
#include "ppl/util/GuidLess.h"

class TipsServer : private boost::noncopyable, public UDPSocketListener
{
public:
	TipsServer();
	~TipsServer();

	/// ����������
	bool Start();
	/// ֹͣ������
	void Stop();

	/// timer�¼�������
	void OnAppTimer();
	void OnDayTimer();

	virtual void OnSocketReceive( UDPSocket* sender, const InetSocketAddress& sockAddr, BYTE* data, size_t size );
	virtual void OnSocketReceiveFailed( UDPSocket* sender, long errcode );

	void ReceiveOneUDP();
	void SendTask( UINT32 sequenceID, UINT32 delayTime, const TaskInfo& task, const InetSocketAddress& sockAddr );

	/// �����յ��ı���
	void HandlePacket( BYTE* data, size_t size, const InetSocketAddress& sockAddr );

	/// �����յ�������
	void HandleRequest( const PacketHeader& header, const InetSocketAddress& sockAddr, RequestInfo request );

protected:
	void LoadBlackIpSegs();

protected:
	void LoadFromTaskFile();
	void LoadFromStatusFile();
	void SaveToStatusFile();
	void SaveToShowFile();
	void SaveToNormalStatusFile();
	void SaveToNormalShowFile();

    string GetTaskFilePath();
	string GetNormalTaskFilePath();

public:
	void UserUpdateTask(vector<string> args);
	void UserShow(vector<string> args);
	void UserShowCount(vector<string> args);
	void UserRequest(vector<string> args);
	void UserShowDetail(vector<string> args);
	void UserTestIP(vector<string> args);
	void UserGetTickCount(vector<string> args);
	void UserOnDayTimer(vector<string> args);
	void UserEmpty(vector<string> args);
	void UserBlacksRefresh(vector<string> args);

private:
	/// ��һ�ζ�ʱ��¼���� �������л��� ����
	long m_lastSecondOfDay;
	/// ������װ��
	PacketBuilder m_builder;
	/// ���ڸ��ͻ���ͨ�ŵ�udp socket����
	UDPSocket m_udp;
	/// �����Ե�timer
	PeriodicTimer m_timer;
	/// ÿ��һ�ε�
	PeriodicTimer m_dayTimer;
	/// �����б�
	map<Guid,Task*> m_taskMap;
	/// Task.txt ���ϴθ���ʱ��
	time_t m_taskTxtLastModifyTime;

	// ������IP������
	vector<pair<u_int, u_int> > m_blackIpSegs;

	// ������
	EmptyTaskInfo m_emptyTaskInfo;
	
	// ���������
	longlong m_emptyCount;
	longlong m_errorCount;
	longlong m_otherCount;
	longlong m_failedCount;

	longlong m_receiveCount;
	longlong m_handelCount;
	longlong m_sendCount;
};
