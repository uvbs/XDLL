
#include "stdafx.h"
#include "TipsStatistic.h"
#include "ppl/mswin/Module.h"
#include <ppl/util/guid.h>
#include <ppl/io/fs.h>
#include "synacast/xdll/TimeUtils.h"
#include "global.h"
#include <boost/algorithm/string.hpp>

const size_t max_udp_recv_packet_size = 4000;

TipsStatistic::TipsStatistic()
: m_lastSecondOfDay(0)
, m_TodayStartCount(0), m_TodayKeepaliveCount(0), m_TodayStopCount(0)
, m_TotalStartCount(0), m_TotalKeepaliveCount(0), m_TotalStopCount(0)
{
	m_timer.SetListener( boost::bind( &TipsStatistic::OnAppTimer, this ) );
	m_dayTimer.SetListener( boost::bind( &TipsStatistic::OnDayTimer, this ) );
	m_udp.SetListener( this );
}

TipsStatistic::~TipsStatistic()
{
}

void TipsStatistic::OnAppTimer()
{
	//APP_DEBUG("on app timer " << m_timer.GetTimes());
	// 此定时器，每一分钟都会触发

	if(m_logFile.is_open())
	{
		m_logFile.flush();
	}

	long secondOfDay = TimeUtils::GetSecondOfDay();
	if( secondOfDay < m_lastSecondOfDay )
	{
		OnDayTimer();
	}
	m_lastSecondOfDay = secondOfDay;
}

void TipsStatistic::OnDayTimer()
{
	APP_DEBUG("on day timer " << m_timer.GetTimes());

	m_TodayStartCount = 0;
	m_TodayKeepaliveCount = 0;
	m_TodayStopCount = 0;
	// 切换文件操作

	if( m_logFile.is_open() )
	{
		m_logFile.close();
		APP_EVENT("m_logFile.close()");
	}
	string log_file = Module().BuildLocalFilePath(_T("slog\\"));//
	log_file += "log_" + TimeUtils::GetTodayShortString() + ".log";
	m_logFile.open(log_file.c_str(), ios_base::app);
	if( !m_logFile )
	{
		APP_EVENT("Open Log File Failed "<<log_file);
	}
	else
	{
		APP_EVENT("Open Log File Succeed "<<log_file);
	}
}

bool TipsStatistic::Start()
{
	if ( false == m_udp.Open( g_UdpPort ) )
	{
		APP_ERROR("failed to open udp port " << g_UdpPort);
		return false;
	}
	for ( size_t index = 0; index < 100; ++index )
	{
		m_udp.Receive( max_udp_recv_packet_size );
	}
	
	// 检查是否是天际切换
	long secondOfDay = TimeUtils::GetSecondOfDay();
	//if( secondOfDay < m_lastSecondOfDay)
	//{
		OnDayTimer();
	//}
	m_lastSecondOfDay = secondOfDay;

	m_timer.Start( 1*1000 );
	return true;
}

void TipsStatistic::Stop()
{
	m_timer.Stop();
	m_udp.Close();
}

void TipsStatistic::OnSocketReceive( UDPSocket* sender, const InetSocketAddress& sockAddr, BYTE* data, size_t size )
{
	//APP_DEBUG("udp packet receive from " << sockAddr << " with buffer " << make_buffer_pair( data, size ));
	ReceiveOneUDP();
	HandlePacket( data, size, sockAddr );
}

void TipsStatistic::OnSocketReceiveFailed( UDPSocket* sender, long errcode )
{
	//APP_DEBUG("udp packet receive failed with errcode=" << errcode);
	ReceiveOneUDP();
}


void TipsStatistic::ReceiveOneUDP()
{
	while ( false == m_udp.Receive( max_udp_recv_packet_size ) )
	{
	}
}

void TipsStatistic::SendTask( UINT32 sequenceID, UINT32 delayTime, const TaskInfo& task, const InetSocketAddress& sockAddr )
{
	m_builder.BuildResonse( sequenceID, delayTime, task );
	m_udp.Send( m_builder.GetData(), m_builder.GetSize(), sockAddr );
}

void TipsStatistic::HandlePacket( BYTE* data, size_t size, const InetSocketAddress& sockAddr )
{
	PacketInputStream is( data, size );
	PacketHeader header;
	if ( is >> header )
	{
		// 解析头部成功
		// 请求只有头部
		if ( XD_TYPE_STATISTIC_REQUEST == header.Type )
		{
			StatisticRequestInfo statistic_request;
			is >> statistic_request;
			HandleStatisticRequest( header, sockAddr, statistic_request);
		}
		else
		{
			//APP_ERROR("invalid request packet type: " << make_tuple(header.Type, header.Version, header.SequenceID));
		}
	}
	else
	{
		APP_ERROR("invalid request packet: " << make_tuple(header.Type, header.Version, header.SequenceID));
	}
}

void TipsStatistic::HandleStatisticRequest( const PacketHeader& header, const InetSocketAddress& sockAddr, StatisticRequestInfo statistic_request)
{
	switch(statistic_request.RequestType)
	{
	case 1:
		m_TodayStartCount ++;
		m_TotalStartCount ++;
		break;
	case 2:
		m_TodayStopCount ++;
		m_TotalStopCount ++;
		break;
	case 3:
		m_TodayKeepaliveCount ++;
		m_TodayKeepaliveCount ++;
		break;
	default:
		break;
	}

	// 写Log
	//APP_EVENT("ChannelID:"<<statistic_request.ChannelID<<" RequestType:"<<statistic_request.RequestType
    //    <<" IUT:" << (int)(statistic_request.Flag&0x01)<<" HugoID:"<<statistic_request.HugeID<<hex
    //    <<" Process:PPStream:"<<(int)(statistic_request.Flag&0x02)<<" Process:PPSAP:"<<(int)(statistic_request.Flag&0x04)
    //    <<" Install:PPS:" << (int)(statistic_request.Flag&0x40000000)<<oct);

	if( m_logFile.is_open() )
	{
		m_logFile 
			<< statistic_request.HugeID << "\t"
			<< statistic_request.ChannelID << "\t" 
			<< statistic_request.RequestType << "\t" 
			<< statistic_request.CanTipsRun << "\t"
			<< statistic_request.Flag << "\t"
			<< sockAddr.ToString() << "\r\n";
	}

	StatisticResponseInfo statistic_response_info;
	statistic_response_info.KeepAliveTime = g_IntervalTime;
	m_builder.BuildStatisticResponse(header.SequenceID, statistic_response_info);
	m_udp.Send(m_builder.GetData(), m_builder.GetSize(), sockAddr);
}

void TipsStatistic::UserShow(vector<string> args)
{
	printf("User Show Begin =======================\r\n");
	printf("【Today】 Start:%u\tStop:%u\tKeepalive:%u\r\n", m_TodayStartCount, m_TodayStopCount, m_TodayKeepaliveCount);
	printf("【Total】 Start:%u\tStop:%u\tKeepalive:%u\r\n", m_TotalStartCount, m_TotalStopCount, m_TotalKeepaliveCount);
	printf("User Show End =======================\r\n");
}