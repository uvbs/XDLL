
#pragma once

#include <boost/noncopyable.hpp>
#include <synacast/asio/timer.h>
#include <synacast/asio/socket.h>
#include <synacast/xdll/PacketBuilder.h>
#include "ppl/util/GuidLess.h"
#include <fstream>

class TipsStatistic : private boost::noncopyable, public UDPSocketListener
{
public:
	TipsStatistic();
	~TipsStatistic();

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
	void HandleStatisticRequest( const PacketHeader& header, const InetSocketAddress& sockAddr, StatisticRequestInfo request );

public:
	void UserShow(vector<string> args);

private:
	/// ��־�ļ�
	ofstream m_logFile;
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

	UINT m_TodayStartCount, m_TodayKeepaliveCount, m_TodayStopCount;
	UINT m_TotalStartCount, m_TotalKeepaliveCount, m_TotalStopCount;
};
