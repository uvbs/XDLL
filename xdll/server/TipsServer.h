
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
	/// 任务列表
	map<Guid,Task*> m_taskMap;
	/// Task.txt 的上次更新时间
	time_t m_taskTxtLastModifyTime;

	// 黑名单IP整数段
	vector<pair<u_int, u_int> > m_blackIpSegs;

	// 空任务
	EmptyTaskInfo m_emptyTaskInfo;
	
	// 空任务次数
	longlong m_emptyCount;
	longlong m_errorCount;
	longlong m_otherCount;
	longlong m_failedCount;

	longlong m_receiveCount;
	longlong m_handelCount;
	longlong m_sendCount;
};
