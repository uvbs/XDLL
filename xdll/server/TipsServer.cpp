#include "stdafx.h"
#include "TipsServer.h"
#include "ppl/mswin/Module.h"
#include <ppl/util/guid.h>
#include <ppl/util/random.h>
#include <ppl/io/fs.h>
#include "TimeControl.h"
#include "synacast/xdll/TimeUtils.h"
#include "Address.h"
#include "global.h"
#include "strings.h"
#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>
#include <time.h>
#include <fstream>

const size_t max_udp_recv_packet_size = 4000;

TipsServer::TipsServer()
: m_lastSecondOfDay(0), m_emptyCount(0), m_errorCount(0), m_otherCount(0)
, m_receiveCount(0), m_handelCount(0), m_sendCount(0), m_failedCount(0)
{
	m_timer.SetListener( boost::bind( &TipsServer::OnAppTimer, this ) );
	m_dayTimer.SetListener( boost::bind( &TipsServer::OnDayTimer, this ) );
	m_udp.SetListener( this );
}

TipsServer::~TipsServer()
{
}

void TipsServer::OnAppTimer()
{
	//APP_DEBUG("on app timer " << m_timer.GetTimes());
	// 此定时器，每一分钟都会触发

	// 2min 触发一次
	// 更新任务任务文件
	if( m_timer.GetTimes() % (1*60) == 0 )
	{	
		string task_file = GetTaskFilePath() + _T("Task.txt");

		file_status fs;
		if( false == fs.retrieve(task_file) )
		{
			APP_ERROR("Task.txt is not existed");
			return;
		}
		if( m_taskTxtLastModifyTime != fs.last_modify_time() )
		{	// Task.txt文件更新过，重新加载
			LoadFromTaskFile();
			SaveToShowFile();
		}
	}

	long secondOfDay = TimeUtils::GetSecondOfDay();
	if( secondOfDay < m_lastSecondOfDay )
	{
		OnDayTimer();
	}
	m_lastSecondOfDay = secondOfDay;

	// 每一秒触发一次，更新DayNowMTULimit
	if( m_timer.GetTimes() % (1) == 0 )
	{
		for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter++)
		{
			Task* task = iter->second;
			assert(task != NULL);
			
			task->DayNowMTULimit = task->GetNowLimit(secondOfDay) - task->DayStartMTULimit;
		}
	}
	
	// 30sec 触发
	// 将任务执行情况输出到Status.txt文件中
	if( m_timer.GetTimes() % (30) == 0 )
	{
		SaveToStatusFile();
		SaveToShowFile();
	}
}

void TipsServer::OnDayTimer()
{
	APP_DEBUG("on day timer " << m_timer.GetTimes());

	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter++)
	{
		Task* task = iter->second;
		assert(task != NULL);
		task->DayNowMTUCount = 0;
		task->DayNowMTULimit = 0;

		long secondOfDay = TimeUtils::GetSecondOfDay();
		task->DayStartMTULimit = task->GetNowLimit(secondOfDay);
	}

	m_emptyCount = 0;
	m_errorCount = 0;
	m_otherCount = 0;
	m_failedCount = 0;

	m_receiveCount = 0;
	m_handelCount = 0;
	m_sendCount = 0;
}

bool TipsServer::Start()
{
	time_t ltime;
	time(&ltime);
	APP_EVENT("Start " << ltime);

	g_ipTable.Load(Module().GetFileDirectory());
	g_ipTable.Init();
	//APP_INFO( "TPIDemo " << g_ipTable.LocateIP(1234567) );

	g_timeControl.Load();
	LoadBlackIpSegs();

	LoadFromTaskFile();
	SaveToStatusFile();
	SaveToShowFile();

	// 启动程序要Load上次退出前的状态文件
	//LoadFromStatusFile();

	// 检查是否是天际切换
	long secondOfDay = TimeUtils::GetSecondOfDay();
	if( secondOfDay < m_lastSecondOfDay)
	{
		OnDayTimer();
	}
	m_lastSecondOfDay = secondOfDay;

	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter++)
	{
		Task* task = iter->second;
		assert(task != NULL);

		task->DayStartMTULimit = task->GetNowLimit(secondOfDay);
	}

	m_timer.Start( 1*1000 );

	if ( false == m_udp.Open( g_UdpPort ) )
	{
		APP_ERROR("failed to open udp port " << g_UdpPort);
		return false;
	}
	for ( size_t index = 0; index < 100; ++index )
	{
		m_udp.Receive( max_udp_recv_packet_size );
	}

	APP_INFO("Tips Server Start Succeed");
	return true;
}

void TipsServer::Stop()
{
	m_timer.Stop();
	m_udp.Close();
}

void TipsServer::OnSocketReceive( UDPSocket* sender, const InetSocketAddress& sockAddr, BYTE* data, size_t size )
{
	//APP_DEBUG("udp packet receive from " << sockAddr << " with buffer " << make_buffer_pair( data, size ));
	ReceiveOneUDP();
	HandlePacket( data, size, sockAddr );
}

void TipsServer::OnSocketReceiveFailed( UDPSocket* sender, long errcode )
{
	//APP_DEBUG("udp packet receive failed with errcode=" << errcode);
	m_failedCount ++;
	ReceiveOneUDP();
}


void TipsServer::ReceiveOneUDP()
{
	while ( false == m_udp.Receive( max_udp_recv_packet_size ) )
	{
	}
}

void TipsServer::SendTask( UINT32 sequenceID, UINT32 delayTime, const TaskInfo& task, const InetSocketAddress& sockAddr )
{
	m_sendCount ++;
	m_builder.BuildResonse( sequenceID, delayTime, task );
	m_udp.Send( m_builder.GetData(), m_builder.GetSize(), sockAddr );
}

void TipsServer::LoadBlackIpSegs()
{
	string baseDir = Module().GetFileDirectory();
	string blacksFilename = paths::combine(baseDir, _T("blacks.txt"));
	
	fstream ifs(blacksFilename.c_str());
	if (!ifs)
	{
		APP_ERROR("Open Blacks File Error.");
		return;
	}
	string line;
	
	// 获得白名单
	while( std::getline(ifs, line) )
	{
		boost::trim(line);

		vector<string> items = splite(line, "\t");
		if (items.size() != 2) continue;
		
		u_int beginIp = inet_addr(items[0].c_str());
		if (beginIp == 0) {
			APP_ERROR("BeginIP Error " << line);
			continue;
		}
		
		u_int endIp = inet_addr(items[1].c_str());
		if (endIp == 0) {
			APP_ERROR("EndIP Error " << line);
			continue;
		}

		beginIp = ntohl(beginIp);
		endIp = ntohl(endIp);
		m_blackIpSegs.push_back(make_pair(beginIp, endIp) );
		APP_INFO("Load Black Ip Seg: " << beginIp << "-" << endIp << " from " << line);
	}

	ifs.close();
}

void TipsServer::HandlePacket( BYTE* data, size_t size, const InetSocketAddress& sockAddr )
{
	PacketInputStream is( data, size );
	PacketHeader header;
	if ( is >> header )
	{
		// 解析头部成功
		// 请求只有头部
		if ( XD_TYPE_REQUEST == header.Type )
		{
			RequestInfo request;
			is >> request;
			HandleRequest( header, sockAddr, request);
		}
		else
		{
			//APP_ERROR("invalid request packet type: " << make_tuple(header.Type, header.Version, header.SequenceID));
			m_otherCount ++;

		}
	}
	else
	{
		APP_ERROR("invalid request packet: " << boost::make_tuple(header.Type, header.Version, header.SequenceID));
		m_errorCount ++;
	}
}

void TipsServer::HandleRequest( const PacketHeader& header, const InetSocketAddress& sockAddr, RequestInfo request)
{
	m_receiveCount ++;

	if (g_IsTestReceiver == 1)
	{
		//m_emptyCount ++;
		//SendTask( header.SequenceID, g_IntervalTime, m_emptyTaskInfo, sockAddr );
		//m_handelCount ++;
		return;
	}

	// 协议比对
	if( header.Version < 2)
	{
		//APP_DEBUG("return because header.Version < 2");
		return;
	}

	if(header.Version < 5 && request.ChannelID == 1)
	{
		//APP_DEBUG("return because header.Version = " <<  header.Version << "&& request.ChannelID == " << request.ChannelID);
		return;
	}

	u_int ip = sockAddr.GetIPHostByteOrder();
	for(int i = 0; i < m_blackIpSegs.size(); i ++) {
		if (ip >= m_blackIpSegs[i].first && ip < m_blackIpSegs[i].second)
		{	// 黑名单命中,什么都不返回
			//APP_EVENT("black hit : " << ip << " (" << m_blackIpSegs[i].first << "," << m_blackIpSegs[i].second << ")");
			return;
		}
	}

	int count = 0;
	// 普通任务
	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); 
		iter != m_taskMap.end() && count<g_TaskCountEachUser; 
		iter++)
	{
		Task* task = iter->second;
		assert(task != NULL);

		// 时间控制
		if( task->DayNowMTUCount >= task->DayNowMTULimit )
			continue;
		
		// 地域控制
		NET_TYPE nt = g_ipTable.LocateIP(sockAddr.GetIP());
		vector<NET_TYPE>& addressList = task->AddressList;

		// 协议版本限制
		if( header.Version < task->MinSupportVersion)
			continue;

		// 64位机控制
		// 完全放开64为操作系统限制
		//if (task->IsSupport64bit == 0 && request.Is64Bit == 1)
		//{	// 如果 任务本身禁止64位系统 并且 请求是64为机器，则直接跳过此任务
		//	continue;
		//}

		// 概率控制
		string hugo_id = request.HugoID;
		if( task->TotalValue > 0)
		{
			if( hugo_id == "")
			{	// HugoID不存在则用ID判断
				u_int ipValue  = sockAddr.GetIP() % task->TotalValue;
				if( ipValue < task->MinValue || ipValue > task->MaxValue )
				{	// 概率控制不命中
					continue;
				}
			}
			else
			{	// HugoID存在,则用华哥ID判断
				boost::hash<std::string> string_hash;
				u_int hugoValue  = string_hash(hugo_id) % task->TotalValue;
				if( hugoValue < task->MinValue || hugoValue > task->MaxValue )
				{	// 概率控制不命中
					continue;
				}
			}	
		}

		//  如果 没有Hit了列表 则跳过
		if(addressList.size() > 0)
		{
			bool hit = false;
			for(vector<NET_TYPE>::iterator iter=addressList.begin(); iter!=addressList.end(); iter++)
			{
				if( true == Hit(nt, *iter) )
				{
					hit = true;
					break;
				}
			}
			if( false == hit ) continue;
		}

		// 如果 Hit了列表  则跳过
		vector<NET_TYPE>& forbidden_addressList = task->ForbiddenAddressList;
		if(forbidden_addressList.size() > 0)
		{
			bool hit = false;
			for(vector<NET_TYPE>::iterator iter=forbidden_addressList.begin(); iter!=forbidden_addressList.end(); iter++)
			{
				if( true == Hit(nt, *iter) )
				{
					hit = true;
					break;
				}
			}
			if( true == hit ) continue;
		}


		if( task->ChannelSet.size() > 0)
		{	// 检查是否有渠道设置
			if( task->ChannelSet.find(request.ChannelID) == task->ChannelSet.end() )
			{	// 渠道非法
				continue;
			}
		}

		if( task->RequestTypeSet.size() > 0)
		{	// 检查是否有请求方式设置
			if( task->RequestTypeSet.find(request.RequestType) == task->RequestTypeSet.end() )
			{	// 请求方式非法
				continue;
			}
		}

		// 是否已经做过了
		if( task->TaskGroupID != 0 )
		{
			if( request.VisitedGroups.find(task->TaskGroupID) != request.VisitedGroups.end() )
			{	// 已经做过了
				int randomValue = RandomGenerator().NextIn( 100000 );
				if ( randomValue >= 100000*task->TaskGroupRate )
				{	// 没有转到 已做过并且重复下发几率
					continue;
				}
			}
		}

		if( task->TaskType == 3)
		{	// IUT定量任务
			//if( !(request.Flag & 0x00000001) )
			//{	// 不符合IUT定量条件
			continue;
			//}
		}
	
		if( task->TaskType == 2)
		{
			MultiHopTaskInfo* lpTaskInfo = (MultiHopTaskInfo*)task->lpTaskInfo;
			// 计算从1970年1月1日 到现在的 秒数
			if (lpTaskInfo->IsSecure == 1)
			{
				time_t ltime;
				time(&ltime);
				lpTaskInfo->ServerTime = (UINT32)ltime;
			}
		}
		
		SendTask( header.SequenceID, g_IntervalTime, *(task->lpTaskInfo), sockAddr );
		task->DayNowMTUCount ++;
		count ++;
	}

	if (count == 0)
	{	// 没有发送正确的任务，则发送空任务
		m_emptyCount ++;
		SendTask( header.SequenceID, g_IntervalTime, m_emptyTaskInfo, sockAddr );
	}

	m_handelCount ++;
}

void TipsServer::LoadFromTaskFile()
{
	string task_file = GetTaskFilePath() + _T("Task.txt");

	file_status fs;
	if( false == fs.retrieve(task_file) )
	{
		APP_ERROR("Task.txt is not existed");
		return;
	}
	m_taskTxtLastModifyTime = fs.last_modify_time();

	std::ifstream ifs(task_file.c_str());
	string line;
	set<Guid> succeededTaskGuids;

	// 添加和更新任务
	while( std::getline(ifs, line) )
	{
		boost::trim(line);

		if( boost::starts_with(line, "#"))
		{
			// 注释
			continue;
		}

		if( line.length() == 0 || line == "")
		{
			// 空行
			continue;
		}
		
		// 读取成功
		Guid taskGuid = Task::GetGuidFormString(line);
		if( taskGuid == Guid() )
		{
			// 失败
			APP_ERROR("LoadFromTaskFile Error 1(Guid失败): "<<line);
			continue;
		}

		map<Guid, Task*>::iterator iter = m_taskMap.find(taskGuid);
		if(iter == m_taskMap.end())
		{	// 还不存在
			Task* task = Task::MakeFormString(line);
			if( task == NULL )
			{
				// 读取失败
				APP_ERROR("LoadFromTaskFile Error 2(读取失败): "<<line);
				continue;
			}
			m_taskMap[taskGuid] = task;
			// 新增任务，更新DayStartMTULimit;
			long secondOfDay = TimeUtils::GetSecondOfDay();
			task->DayStartMTULimit = task->GetNowLimit(secondOfDay);
			succeededTaskGuids.insert(taskGuid);
			//APP_INFO("LoadFromTaskFile Succed(New Task): "<<taskGuid);
		}
		else
		{	// 已经存在
			Task* task = iter->second;
			assert( task != NULL );
			if( false == task->UpdateFromString(line) )
			{
				APP_ERROR("LoadFromTaskFile Error 3(读取失败): "<<line);
				continue;
			}
			if(succeededTaskGuids.find(taskGuid) != succeededTaskGuids.end())
			{
				APP_ERROR("LoadFromTaskFile Error 4(重复添加): "<<line);
				continue;
			}

			succeededTaskGuids.insert(taskGuid);
			//APP_INFO("LoadFromTaskFile Succed(Update Task): "<<taskGuid);
		}
	}

	ifs.close();

	// 删除旧任务
	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end();)
	{
		Task* task = iter->second;
		assert(task != NULL);
		if( succeededTaskGuids.find(task->TaskGuid) == succeededTaskGuids.end() )
		{	
			m_taskMap.erase(iter ++);
			//APP_INFO("LoadFromTaskFile Succed(Delete Task): "<<task->TaskGuid);
			delete task;
		}
		else
		{
			iter ++;
		}
	}

	APP_INFO("LoadFromTaskFile Succed");
}

void TipsServer::SaveToStatusFile()
{
	string status_file = GetTaskFilePath() + _T("Status.txt");

	std::ofstream ofs(status_file.c_str());

	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter ++)
	{
		Task* task = iter->second;
		assert(task != NULL);
		string line = task->GeneralStatusString();
		ofs << line << std::endl;
	}

	ofs.close();
}

void TipsServer::SaveToShowFile()
{
	string status_file = GetTaskFilePath() + _T("Show.txt");

	std::ofstream ofs(status_file.c_str());

	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter++)
	{
		Task* task = iter->second;
		assert(task != NULL);
		string line = task->GeneralShowString();
		ofs << line << std::endl;
	}

	ofs.close();
}

void TipsServer::UserUpdateTask(vector<string> args)
{
	printf("User Update Task Begin\r\n");
	LoadFromTaskFile();
	SaveToShowFile();
	printf("User Update Task End\r\n");
}

void TipsServer::UserEmpty(vector<string> args)
{
	printf("Empty Count %d\r\n", m_emptyCount);
	printf("Error Count %d\r\n", m_errorCount);
	printf("Other Count %d\r\n", m_otherCount);
	printf("Failed Count %d\r\n", m_failedCount);
	printf("---------------------------------------\r\n");
	printf("Receive Count %d\r\n", m_receiveCount);
	printf("Handle Count %d\r\n", m_handelCount);
	printf("Send Count %d\r\n", m_sendCount);
}

void TipsServer::UserShow(vector<string> args)
{
	printf("User Show Begin =======================\r\n");
	printf("there are %d taskes\r\n", m_taskMap.size());
	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter++)
	{
		Task* task = iter->second;
		assert(task != NULL);
		task->UserShow();
	}
	printf("User Show End =======================\r\n");
}

void TipsServer::UserShowCount(vector<string> args)
{
	printf("there are %d taskes\r\n", m_taskMap.size());
}

void TipsServer::UserShowDetail(vector<string> args)
{
	printf("User Show Detail Begin =======================\r\n");
	printf("there are %d taskes\r\n", m_taskMap.size());
	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter++)
	{
		Task* task = iter->second;
		assert(task != NULL);
		task->UserShowDetail();
	}
	printf("User Show End =======================\r\n");
}

void TipsServer::UserTestIP(vector<string> args)
{
	printf("User Test Ip Begin =======================\r\n");
	if(args.size() > 1)
	{
		printf("ip = %s\r\n", args[1].c_str());
		NET_TYPE nt = g_ipTable.LocateIP( inet_addr(args[1].c_str()) );
		printf("NET_TYPE = %d|%d|%d\n", nt.Country, nt.Province, nt.City);
	}
	
	printf("User Test Ip End =======================\r\n");
}

void TipsServer::UserRequest(vector<string> args)
{
	printf("UserRequest\r\n");

	int send_count=0, task_count=0;

	u_int ip_uint = 0;
	NET_TYPE nt;
	string hugo_id = "";
	if( args.size() > 1)
	{
		ip_uint = inet_addr(args[1].c_str());
		nt = g_ipTable.LocateIP(ip_uint);
		APP_INFO("IP "<<args[1]<<" "<<nt<<"\r\n");
	}
	if( args.size() > 2)
	{
		hugo_id = args[2];
		APP_INFO("HugoID "<<args[2]<<"\r\n");
	}

	for(map<Guid, Task*>::iterator iter=m_taskMap.begin(); iter != m_taskMap.end(); iter++)
	{
		Task* task = iter->second;
		assert(task != NULL);

		task_count ++;

		// 时间控制
		if( task->DayNowMTUCount > task->DayNowMTULimit )
			continue;

		// 概率控制
		if( task->TotalValue > 0)
		{
			if( hugo_id == "")
			{	// HugoID不存在则用ID判断
				u_int ipValue  = ip_uint % task->TotalValue;
				if( ipValue < task->MinValue || ipValue > task->MaxValue )
				{	// 概率控制不命中
					continue;
				}
			}
			else
			{	// HugoID存在,则用华哥ID判断
				boost::hash<std::string> string_hash;
				u_int hugoValue  = string_hash(hugo_id) % task->TotalValue;
				if( hugoValue < task->MinValue || hugoValue > task->MaxValue )
				{	// 概率控制不命中
					continue;
				}
			}	
		}

		// 地域控制
		if( args.size() > 1)
		{
			nt = g_ipTable.LocateIP( inet_addr(args[1].c_str()) );
			vector<NET_TYPE>& addressList = task->AddressList;
			if(addressList.size() > 0)
			{
				bool hit = false;
				for(vector<NET_TYPE>::iterator iter=addressList.begin(); iter!=addressList.end(); iter++)
				{
					if( true == Hit(nt, *iter) )
					{
						hit = true;
						break;
					}
				}
				if( false == hit ) continue;
			}
			vector<NET_TYPE>& forbidden_addressList = task->ForbiddenAddressList;
			if(forbidden_addressList.size() > 0)
			{
				bool hit = false;
				for(vector<NET_TYPE>::iterator iter=forbidden_addressList.begin(); iter!=forbidden_addressList.end(); iter++)
				{
					if( true == Hit(nt, *iter) )
					{
						hit = true;
						break;
					}
				}
				if( true == hit ) continue;
			}
		}

		task->DayNowMTUCount ++;
		send_count ++;

		task->UserShow();
	}
	printf("TipsServer::UserRequest (Send:Task)=(%d/%d)\r\n",send_count,task_count);
}

void TipsServer::UserGetTickCount(vector<string> args)
{
	printf("User GetTickCount Begin =======================\r\n");
	printf("GetTickCount %u=%u days : %u : left %u days\n", ::GetTickCount(), ::GetTickCount()/1000/60/60/24, -1, ((u_int)-1 - ::GetTickCount())/1000/60/60/24);
	printf("User GetTickCount End =======================\r\n");
}

void TipsServer::UserOnDayTimer(vector<string> args)
{
	printf("User OnDayTimer Begin =======================\r\n");
	this->OnDayTimer();
	printf("User OnDayTimer End =======================\r\n");
}

void  TipsServer::UserBlacksRefresh(vector<string> args)
{
	printf("User UserBlacksRefresh Begin =======================\r\n");
	this->m_blackIpSegs.clear();
	LoadBlackIpSegs();
	printf("User UserBlacksRefresh End =======================\r\n");
}

string TipsServer::GetTaskFilePath()
{
	return Module().BuildLocalFilePath(_T("Task/"));
}