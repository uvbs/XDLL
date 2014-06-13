
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

	/// 启动服务器
	bool Start();
	/// 停止服务器
	void Stop();

	/// timer事件处理函数
	void OnAppTimer();
	void OnDayTimer();

	virtual void OnSocketReceive( UDPSocket* sender, const InetSocketAddress& sockAddr, BYTE* data, size_t size );
	virtual void OnSocketReceiveFailed( UDPSocket* sender, long errcode );

	void ReceiveOneUDP();

	void SendTask( UINT32 sequenceID, UINT32 delayTime, const TaskInfo& task, const InetSocketAddress& sockAddr );

	/// 处理收到的报文
	void HandlePacket( BYTE* data, size_t size, const InetSocketAddress& sockAddr );

	/// 处理收到的请求
	void HandleStatisticRequest( const PacketHeader& header, const InetSocketAddress& sockAddr, StatisticRequestInfo request );

public:
	void UserShow(vector<string> args);

private:
	/// 日志文件
	ofstream m_logFile;
	/// 上一次定时记录用于 天与天切换的 秒数
	long m_lastSecondOfDay;
	/// 报文组装器
	PacketBuilder m_builder;
	/// 用于跟客户端通信的udp socket对象
	UDPSocket m_udp;
	/// 周期性的timer
	PeriodicTimer m_timer;
	/// 每天一次的
	PeriodicTimer m_dayTimer;

	UINT m_TodayStartCount, m_TodayKeepaliveCount, m_TodayStopCount;
	UINT m_TotalStartCount, m_TotalKeepaliveCount, m_TotalStopCount;
};
