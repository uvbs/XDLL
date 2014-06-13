#include "stdafx.h"
#include "Task.h"
#include "strings.h"
#include "global.h"
#include <ppl/util/time.h>
#include "TimeControl.h"
#include <boost/algorithm/string/replace.hpp>

Task::Task()
: TaskType(1)
, DayDistCount(0)
, DayMTUDistCount(0)
, DayNowMTUCount(0)
, MTUCount(0)
, DayStartMTULimit(0)
, DayNowMTULimit(0)
, lpTaskInfo(NULL)
, MinSupportVersion(0)
, MinValue(0)
, MaxValue(0)
, TotalValue(0)
, TTEx(0)
, IsSupport64bit(0)
{
	for(int i=0; i<144; i++)
		DayLimitedValues[i] = 0;
};

Task::~Task()
{
	if(lpTaskInfo != NULL) delete lpTaskInfo;
}

Guid Task::GetGuidFormString(const string& line)
{
	vector<string> lines = splite(line, "\t");
	Guid guid;
	if(lines.size() == 0 )
		return Guid();
	if( ! guid.Parse(lines[0]) )
		return Guid();
	return guid;
}

int Task::GetNowLimit(long secondOfDay)
{
	int ret;
	int x = secondOfDay / (10*60);		// x 决定是第几个区间
	assert(x < 144);
	int y = secondOfDay % (10*60);		// y 决定是第几个区间 的 第几个位置
	
	if(x==0)
	{
		ret = y * DayLimitedValues[0] / (10*60);
	}
	else if(x >= 144)
	{
		ret = DayDistCount;
	}
	else
	{
		ret = DayLimitedValues[x-1] + (int)((double)(DayLimitedValues[x]-DayLimitedValues[x-1]) * (double)y / (double)(10*60));
	}
	return ret;
}

Task* Task::MakeFormString(const string& line)
{
	Task* task = new Task;
	if( false == task->UpdateFromString(line) )
	{
		delete task;
		return NULL;
	}
	return task;
}

bool Task::UpdateFromString(const string& line)
{
	vector<string> lines = splite(line, "\t");
	if(lines.size() < 7)
		return false;
	//	<TaskGUID>	<TaskType:[TaskGroupID:[TaskGroupRate]]>	<AddressControl>	<TimeControlType>	
	//	<ChannelID:ChannelID:*|RequestType:RequestType:*>	<DayDistCount>		<TaskString>
	//	< Version >	<probability>	<TTEx>	<Support>

	// <TaskGUID>
	Guid guid;
	if( ! guid.Parse(lines[0]) )
		return false;

	// <TaskType:[TaskGroupID:[TaskGroupRate]]>
	int taskType = 1;
	int taskGroupID = 0;
	double taskGroupRate = 0.0;
	vector<string> lines_1 = splite(lines[1], ":");
	if( lines_1.size() >= 1)
	{
		try { taskType = boost::lexical_cast<int>(lines_1[0]); } catch (...) { return false; }
	}
	if( lines_1.size() >= 2)
	{
		try { taskGroupID = boost::lexical_cast<int>(lines_1[1]); } catch (...) { return false; }
	}
	if( lines_1.size() >= 3)
	{
		try { taskGroupRate = boost::lexical_cast<double>(lines_1[2]); } catch (...) { return false; }
	}

	// <TimeControlType>
	int timeControlType = 0;
	try { timeControlType = boost::lexical_cast<int>(lines[3]); } catch (...) { return false; }
	// 读取时间模拟曲线，生成自己的分时明细
	if( g_timeControl.m_timeValues.find(timeControlType) == g_timeControl.m_timeValues.end() )
	{
		return false;
	}

	//<ChannelID:ChannelID:*|RequestType:RequestType:*>
	set<short> channel_set;
	set<short> request_type_set;
	vector<string> lines_2 = splite(lines[4], "|");
	if( lines_2.size() == 2 )
	{
		string channel_string = lines_2[0];
		string request_type_string = lines_2[1];
		if(channel_string != "*")
		{
			vector<string> channel_string_lines = splite(channel_string, ":");
			for(int i = 0; i < channel_string_lines.size(); i ++ )
			{
				try{ channel_set.insert( boost::lexical_cast<int>(channel_string_lines[i]) ); } catch(...) {};
			}
		}
		if(request_type_string != "*")
		{
			vector<string> request_type_string_lines = splite(request_type_string, ":");
			for(int j = 0; j < request_type_string_lines.size(); j ++)
			{
				try{ request_type_set.insert( boost::lexical_cast<int>(request_type_string_lines[j]) ); } catch(...) {};
			}
		}
	}
	else
	{
		channel_set.insert(0);
		request_type_set.insert(0);
	}


	// <DayDistCount>
	int dayDistCount = 0;
	try { dayDistCount = boost::lexical_cast<int>(lines[5]); } catch (...) { return false; }

	
	// <Version>
	int minSupportVersion = 0;
	if(lines.size() > 7)
	{
		try { minSupportVersion = boost::lexical_cast<u_int>(lines[7]); } catch (...) { return false; }
	}

	// <Probability>
	int minValue = 0;
	int maxValue = 0;
	int totalValue = 0;

	if(lines.size() > 8)
	{
		string probability_string = lines[8];
		vector<string> values =  splite(probability_string, "|");
		if(values.size() != 3) return false;
		try { minValue = boost::lexical_cast<u_int>(values[0]); } catch (...) { return false; }
		try { maxValue = boost::lexical_cast<u_int>(values[1]); } catch (...) { return false; }
		try { totalValue = boost::lexical_cast<u_int>(values[2]); } catch (...) { return false; }
	}

	// <TTEx>
	u_int ttex = 0;
	if (lines.size() > 9)
	{
		string ttex_string = lines[9];
		try { ttex = boost::lexical_cast<u_int>(ttex_string); } catch (...) { return false; }
	}

	// <Support>
	u_short isSupport64bit = 0;
	if (lines.size() > 10)
	{
		string support64bit_string = lines[10];
		try { isSupport64bit = boost::lexical_cast<u_short>(support64bit_string); } catch (...) { return false; }
	}

	// <TaskString>
	string taskString = lines[6];
	vector<string> task_elems = splite(taskString, "||||");
	TaskInfo* lpTaskInfo;

	if( taskType == XD_NORMAL_TASK )
	{   //Normal任务
		//<ClickRate>||||<BrowerRamdom>|||| <ShowUrl>||||<ShowRefer>||||<ClickUrl>|||| <ClickRefer>
		if(task_elems.size() != 6)	return false;
		int clickRate;
		try { clickRate = boost::lexical_cast<int>(task_elems[0]); } catch (...) { return false; }
		if( clickRate < 0 || clickRate > 100 ) return false;

		int browerRamdom = 0;
		try { browerRamdom = boost::lexical_cast<int>(task_elems[1]); } catch (...) { return false; }

		string showUrl = (task_elems[2] != "*") ? task_elems[2] : "";
		string showRefer = (task_elems[3] != "*") ? task_elems[3] : "";
		string clickUrl = (task_elems[4] != "*") ? task_elems[4] : "";
		string clickRefer = (task_elems[5] != "*") ? task_elems[5] : "";

		// 组装normal task
		NormalTaskInfo* lpNormalTaskInfo = new NormalTaskInfo;
		lpNormalTaskInfo->BrowserRandom = browerRamdom;
		lpNormalTaskInfo->ClickRate = clickRate;
		lpNormalTaskInfo->ShowUrl = showUrl;
		lpNormalTaskInfo->ShowReferer = showRefer;
		lpNormalTaskInfo->ClickUrl = clickUrl;
		lpNormalTaskInfo->ClickReferer = clickRefer;
		lpNormalTaskInfo->GroupID = taskGroupID;
		lpTaskInfo = lpNormalTaskInfo;
	} 
	else if( taskType == XD_MULTI_HOP_TASK )
	{   //MultiHop任务
		//<BrowerRamdom>||||< FirstUrl>||||< FirstRefer>||||<HopInfo>||||<Rule>||||<ClickIntervalMin>
		//||||<ClickIntervalMax>||||<TimeoutTime>||||<RemoveCookiesRate>||||<RemoveCookies>
		//||||<IsSecure>||||<IsOpenWindow>||||<AllowArea>||||<LoadFlash>
		//||||<RefererMode>
		//||||<UserAgent>
		//||||<DirectionalMode>||||<Directional>
		//||||<UrlFilterType>||||<UrlFilter>
		//||||<UaMinWidth>||||< UaMaxWidth >||||<UaMinHeight>||||<UaMaxHeight>
		//||||<Platform>
		if(task_elems.size() != 5 && task_elems.size() != 8 && task_elems.size() < 10)	return false;

		int browerRamdom = 0;
		try { browerRamdom = boost::lexical_cast<int>(task_elems[0]); } catch (...) { return false; }

		string firstUrl = (task_elems[1] != "*") ? task_elems[1] : "";
		string firstRefer = (task_elems[2] != "*") ? task_elems[2] : "";
		string hopInfo = task_elems[3];
		string ruleInfo = task_elems[4];
		boost::replace_all(ruleInfo, "\\n", "\n");
		boost::replace_all(ruleInfo, "\\r", "\r");

        u_int clickIntervalMin = 0;
        u_int clickIntervalMax = 0;
        u_int timeoutTime = 0;
        if( task_elems.size() >= 8 )
        {
            try { clickIntervalMin = boost::lexical_cast<u_int>(task_elems[5]); } catch (...) { return false; }
            try { clickIntervalMax = boost::lexical_cast<u_int>(task_elems[6]); } catch (...) { return false; }
            try { timeoutTime = boost::lexical_cast<u_int>(task_elems[7]); } catch (...) { return false; }
        }

		UINT removeCookiesRate = 0;
		string removeCookies = "";
		if (task_elems.size() >= 10)
		{
			try { removeCookiesRate = boost::lexical_cast<u_int>(task_elems[8]); } catch (...) {return false;}
			removeCookies = task_elems[9];
		}

		u_short is_secure = 0;
		if (task_elems.size() >= 11)
		{
			try { is_secure = boost::lexical_cast<u_short>(task_elems[10]); } catch (...) { return false; }
		}

		u_short is_openWindow = 0;
		if (task_elems.size() >= 12)
		{
			try { is_openWindow = boost::lexical_cast<u_short>(task_elems[11]); } catch (...) { return false; }
		}

		u_short is_allowArea = 0;
		if (task_elems.size() >= 13)
		{
			try { is_allowArea = boost::lexical_cast<u_short>(task_elems[12]); } catch (...) { return false; }
		}

		u_short is_loadFlash = 0;
		if (task_elems.size() >= 14)
		{
			try { is_loadFlash = boost::lexical_cast<u_short>(task_elems[13]); } catch (...) { return false; }
		}

		u_short refererMode = 0;
		if (task_elems.size() >= 15)
		{
			try { refererMode = boost::lexical_cast<u_short>(task_elems[14]); } catch (...) { return false; }
		}

		string userAgent;
		if (task_elems.size() >= 16)
		{
			if(task_elems[15] != "*") 
				userAgent=task_elems[15];
		}

		u_short directional_mode = 0;
		string directional;
		if (task_elems.size() >= 17)
		{
			try { directional_mode = boost::lexical_cast<u_short>(task_elems[16]); } catch (...) { return false; }
		}
		if (task_elems.size() >= 18)
		{
			if(task_elems[17] != "*") 
				directional = task_elems[17];
		}

		u_short urlfilter_type = 0;
		string urlfilter;
		if (task_elems.size() >= 19)
		{
			try { urlfilter_type = boost::lexical_cast<u_short>(task_elems[18]); } catch (...) { return false; }
		}
		if (task_elems.size() >= 20)
		{
			if(task_elems[19] != "*") 
				urlfilter = task_elems[19];
		}

		u_int remove_flash_cookie_rate = 0;
		string remove_flash_cookies;
		if (task_elems.size() >= 21)
		{
			try { remove_flash_cookie_rate = boost::lexical_cast<u_int>(task_elems[20]); } catch (...) { return false; }
		}
		if (task_elems.size() >= 22)
		{
			if (task_elems[21] != "*")
				remove_flash_cookies = task_elems[21];
		}

		u_int uaMinWidth = 0;
		u_int uaMaxWidth = 0;
		u_int uaMinHeight = 0;
		u_int uaMaxHeight = 0;
		if( task_elems.size() >= 26 )
		{
			try { uaMinWidth = boost::lexical_cast<u_int>(task_elems[22]); } catch (...) { return false; }
			try { uaMaxWidth = boost::lexical_cast<u_int>(task_elems[23]); } catch (...) { return false; }
			try { uaMinHeight = boost::lexical_cast<u_int>(task_elems[24]); } catch (...) { return false; }
			try { uaMaxHeight = boost::lexical_cast<u_int>(task_elems[25]); } catch (...) { return false; }
		}

		string platform;
		if (task_elems.size() >= 27) 
		{
			platform = task_elems[26];
		}

		// 发送多跳任务
		MultiHopTaskInfo* lpMultiHopTaskInfo = new MultiHopTaskInfo;
		//0
		lpMultiHopTaskInfo->BrowserRandom = browerRamdom;
		lpMultiHopTaskInfo->FirstUrl = firstUrl;
		lpMultiHopTaskInfo->FirstReferer = firstRefer;
		lpMultiHopTaskInfo->HopInfo = hopInfo;
		lpMultiHopTaskInfo->RuleInfo = ruleInfo;
		lpMultiHopTaskInfo->GroupID = taskGroupID;
        lpMultiHopTaskInfo->ClickIntervalMin = clickIntervalMin;
        lpMultiHopTaskInfo->ClickIntervalMax = clickIntervalMax;
        lpMultiHopTaskInfo->TimeoutTime = timeoutTime;
		lpMultiHopTaskInfo->RemoveCookiesRate = removeCookiesRate;
		//10
		lpMultiHopTaskInfo->RemoveCookies = removeCookies;
		lpMultiHopTaskInfo->IsSecure = is_secure;
		lpMultiHopTaskInfo->ServerTime = 0;
		lpMultiHopTaskInfo->IsOpenWindow = is_openWindow;
		lpMultiHopTaskInfo->AllowArea = is_allowArea;
		lpMultiHopTaskInfo->LoadFlash = is_loadFlash;
		lpMultiHopTaskInfo->RefererMode = refererMode;
		lpMultiHopTaskInfo->UserAgent = userAgent;
		lpMultiHopTaskInfo->DirectionalMode = directional_mode;
		lpMultiHopTaskInfo->Directional = directional;
		lpMultiHopTaskInfo->UrlFilterType = urlfilter_type;
		//20
		lpMultiHopTaskInfo->UrlFilter = urlfilter;
		lpMultiHopTaskInfo->RemoveFlashCookieRate = remove_flash_cookie_rate;
		lpMultiHopTaskInfo->RemoveFlashCookies = remove_flash_cookies;
		lpMultiHopTaskInfo->UaMinWidth = uaMinWidth;
		lpMultiHopTaskInfo->UaMaxWidth = uaMaxWidth;
		lpMultiHopTaskInfo->UaMinHeight = uaMinHeight;
		lpMultiHopTaskInfo->UaMaxHeight = uaMaxHeight;
		lpMultiHopTaskInfo->Platform = platform;
		lpTaskInfo = lpMultiHopTaskInfo;
	}
	else if (taskType == XD_IUT_TASK)
	{   //IUT任务
		//<NewProcessName>||||< MinRunDuration>||||<MaxRunDuration>
		if(task_elems.size() != 3)	return false;

		string	new_process_name = task_elems[0];
		u_int	min_run_duration = 600;
		u_int	max_run_duration = 1800;
		try { min_run_duration = boost::lexical_cast<u_int>(task_elems[1]); } catch (...) { return false; }
		try { max_run_duration = boost::lexical_cast<u_int>(task_elems[2]); } catch (...) { return false; }

		IUTTaskInfo* lpIUTTaskInfo = new IUTTaskInfo;
		lpIUTTaskInfo->NewProcessName = new_process_name;
		lpIUTTaskInfo->IUTProcessName = g_IUT_IUTProcessName;
		lpIUTTaskInfo->MinRunDuration = min_run_duration;
		lpIUTTaskInfo->MaxRunDuration = max_run_duration;
		lpIUTTaskInfo->GroupID = taskGroupID;
		lpTaskInfo = lpIUTTaskInfo;
	}
    else if( taskType == XD_EXE_TASK )
    {   // EXE任务
        //<OSVersion>||||<IsSecure>||||<IsOnce>||||<ExeFileLength>||||<ExeName>||||<ExeHttpUrl>||||<Forbiddens>
        if(task_elems.size() != 7)	return false;

        u_short	os_version = 0;
        u_short is_secure = 0;
        u_short is_once = 0;
        u_int exe_file_length = 0;
        try { os_version = boost::lexical_cast<u_short>(task_elems[0]); } catch (...) { return false; }
        try { is_secure = boost::lexical_cast<u_short>(task_elems[1]); } catch (...) { return false; }
        try { is_once = boost::lexical_cast<u_short>(task_elems[2]); } catch (...) { return false; }
        try { exe_file_length = boost::lexical_cast<u_int>(task_elems[3]); } catch (...) { return false; }
        string exe_name = task_elems[4];
        string exe_http_url = task_elems[5];
        string forbiddens = task_elems[6];

        EXETaskInfo* lpEXETaskInfo = new EXETaskInfo;
        lpEXETaskInfo->OSVersion = os_version;
        lpEXETaskInfo->IsSecure = is_secure;
        lpEXETaskInfo->IsOnce = is_once;
        lpEXETaskInfo->ExeFileLength = exe_file_length;
        lpEXETaskInfo->ExeName = exe_name;
        lpEXETaskInfo->ExeHttpUrl = exe_http_url;
        lpEXETaskInfo->Forbiddens = forbiddens;
        lpEXETaskInfo->GroupID = taskGroupID;
        lpTaskInfo = lpEXETaskInfo;
    }
    else if (taskType == XD_POST_TASK)
    {   //POST任务
        //<BrowerRamdom>||||<Url>||||<Refer>||||<Content>||||<ContentType >
        if(task_elems.size() != 5)	return false;
        int browerRamdom = 0;
        try { browerRamdom = boost::lexical_cast<int>(task_elems[0]); } catch (...) { return false; }

        string url = (task_elems[1] != "*") ? task_elems[1] : "";
        string refer = (task_elems[2] != "*") ? task_elems[2] : "";
        string content = (task_elems[3] != "*") ? task_elems[3] : "";
        string contentType = (task_elems[4] != "*") ? task_elems[4] : "";
        
        // 组装 post task
        PostTaskInfo* lpPostTaskInfo = new PostTaskInfo;
        lpPostTaskInfo->BrowserRandom = browerRamdom;
        lpPostTaskInfo->Url = url;
        lpPostTaskInfo->Referer = refer;
        lpPostTaskInfo->Content = content;
        lpPostTaskInfo->ContentType = contentType;
        lpPostTaskInfo->GroupID = taskGroupID;
        lpTaskInfo = lpPostTaskInfo;
    }
	else if( taskType == XD_UPDATE_TASK )
	{   //Update任务
		//<BeginVersion>||||<EndVersion>||||<DistVersion>||||<DistUrl>||||<DllFileLength>
		if(task_elems.size() < 5)	return false;

		string begin_version = task_elems[0];
		string end_version = task_elems[1];
		string dist_version = task_elems[2];
		string dist_url = task_elems[3];

		u_int dll_file_length = 0;
		try { dll_file_length = boost::lexical_cast<u_int>(task_elems[4]); } catch (...) { return false; }

		// 组装 update task
		UpdateTaskInfo* lpUpdateTaskInfo = new UpdateTaskInfo;
		lpUpdateTaskInfo->BeginVersion = begin_version;
		lpUpdateTaskInfo->EndVersion = end_version;
		lpUpdateTaskInfo->DistVersion = dist_version;
		lpUpdateTaskInfo->DistUrl = dist_url;
		lpUpdateTaskInfo->DllFileLength = dll_file_length;
		lpTaskInfo = lpUpdateTaskInfo;
	}
	else if(taskType == XD_REDIRECT_TASK)
	{	//Redirect任务
		//<DistServer>||||<DistServerPort>||||<IsAlways>
		if(task_elems.size() < 3)	return false;

		string dist_server = task_elems[0];

		u_short dist_server_port = 0;
		try { dist_server_port = boost::lexical_cast<u_short>(task_elems[1]); } catch (...) { return false; }

		u_short is_always = 0;
		try { is_always = boost::lexical_cast<u_short>(task_elems[2]); } catch (...) { return false; }

		// 组装 post task
		RedirectTaskInfo* lpRedirectTaskInfo = new RedirectTaskInfo;
		lpRedirectTaskInfo->DistServer = dist_server;
		lpRedirectTaskInfo->DistServerPort = dist_server_port;
		lpRedirectTaskInfo->IsAlways = is_always;
		lpTaskInfo = lpRedirectTaskInfo;
	}
	else if (taskType == XD_CONFIG_TASK)
	{	// Config 任务
		if(task_elems.size() < 2)	return false;

		u_short type = 0;
		try { type = boost::lexical_cast<u_short>(task_elems[0]); } catch (...) { return false; }

		string url = task_elems[1];
		if (url == "" || url == "*")
			return false;
		
		// 组装 config 任务
		ConfigTaskInfo* lpConfigTaskInfo = new ConfigTaskInfo;
		lpConfigTaskInfo->Url = url;
		lpConfigTaskInfo->Type = type;
		lpConfigTaskInfo->GroupID = taskGroupID;
		lpTaskInfo = lpConfigTaskInfo;
	}
	else if (taskType == XD_CLIENTSIM_TASK)
	{	// ClientSim 任务
		if (task_elems.size() < 3) return false;

		string adType = task_elems[0];	
		int maxl = 0;
		try { maxl = boost::lexical_cast<int>(task_elems[1]); } catch (...) { return false; }
		int maxc = 0;
		try { maxc = boost::lexical_cast<int>(task_elems[2]); } catch (...) { return false; }

		u_short isCommon = 0;
		if (task_elems.size() > 3)
		{
			try { isCommon = boost::lexical_cast<u_short>(task_elems[3]); } catch (...) { return false; }
		}

		ClientSimTaskInfo* lpClientSimTaskInfo = new ClientSimTaskInfo;
		lpClientSimTaskInfo->AdType = adType;
		lpClientSimTaskInfo->Maxl = maxl;
		lpClientSimTaskInfo->Maxc = maxc;
		lpClientSimTaskInfo->GroupID = taskGroupID;
		lpClientSimTaskInfo->IsCommon = isCommon;
		lpTaskInfo = lpClientSimTaskInfo;
	}
	else if (taskType == XD_VASTSIM_TASK)
	{	// VastSim 任务
		if (task_elems.size() < 3) return false;

		int isClosed = 0;
		try { isClosed = boost::lexical_cast<u_short>(task_elems[0]); } catch (...) { return false; }
		int maxl = 0;
		try { maxl = boost::lexical_cast<int>(task_elems[1]); } catch (...) { return false; }
		int maxc = 0;
		try { maxc = boost::lexical_cast<int>(task_elems[2]); } catch (...) { return false; }

		VASTSimTaskInfo* lpVastSimTaskInfo = new VASTSimTaskInfo;
		lpVastSimTaskInfo->IsClose = isClosed;
		lpVastSimTaskInfo->Maxl = maxl;
		lpVastSimTaskInfo->Maxc = maxc;
		lpVastSimTaskInfo->GroupID = taskGroupID;
		lpTaskInfo = lpVastSimTaskInfo;
	}
	else if (taskType == XD_VASTEXSIM_TASK)
	{	// VastExSim 任务
		if (task_elems.size() < 2) return false;

		u_short isSecure = 0;
		try { isSecure = boost::lexical_cast<u_short>(task_elems[0]); } catch (...) { return false; }

		string vast_url = task_elems[1];
		VASTExSimTaskInfo* lpVastExSimTaskInfo = new VASTExSimTaskInfo;
		lpVastExSimTaskInfo->IsSecure = isSecure;
		lpVastExSimTaskInfo->VastUrl = vast_url;
		lpVastExSimTaskInfo->GroupID = taskGroupID;
		lpTaskInfo = lpVastExSimTaskInfo;
	}
	else if (taskType == XD_ClIENTEXSIM_TASK)
	{	// VastExSim 任务
		if (task_elems.size() < 2) return false;

		u_short isSecure = 0;
		try { isSecure = boost::lexical_cast<u_short>(task_elems[0]); } catch (...) { return false; }

		string client_url = task_elems[1];
		ClientExSimTaskInfo* lpVastExSimTaskInfo = new ClientExSimTaskInfo;
		lpVastExSimTaskInfo->IsSecure = isSecure;
		lpVastExSimTaskInfo->ClientUrl = client_url;
		lpVastExSimTaskInfo->GroupID = taskGroupID;
		lpTaskInfo = lpVastExSimTaskInfo;
	}
	else
	{
		assert(0);
		return false;
	}

	int packetSize = lpTaskInfo->GetObjectSize() + PacketHeader::object_size + TaskHeader::object_size;
	int mtuCount = (packetSize % UDP_MTU)==0 ? packetSize/UDP_MTU : packetSize/UDP_MTU + 1;

	this->TaskGuid = guid;
	this->TaskType = taskType;
	this->TaskGroupID = taskGroupID;
	this->TaskGroupRate = taskGroupRate;
	this->DayDistCount = dayDistCount;
	this->DayMTUDistCount = MakeMTU(dayDistCount, mtuCount);
	this->MTUCount = mtuCount;
	this->ChannelSet = channel_set;
	this->RequestTypeSet = request_type_set;
	this->MinSupportVersion = minSupportVersion;
	this->MinValue = minValue;
	this->MaxValue = maxValue;
	this->TotalValue = totalValue;
	this->TTEx = ttex;
	this->IsSupport64bit = IsSupport64bit;

	if( this->lpTaskInfo != NULL ) delete this->lpTaskInfo;
	this->lpTaskInfo = lpTaskInfo;

	//加载时间控制
	TimeControlEntry& tce = g_timeControl.m_timeValues.find(timeControlType)->second;
	tce.MakeValue(this->DayMTUDistCount, this->DayLimitedValues);

	//<AddressControl> 加载地域控制
	string addressControl = lines[2];
	if( addressControl != "*" )
		this->LoadAddressListFromLine(addressControl);

	return true;
}

string Task::GeneralStatusString()
{
	ostringstream oss;
	oss << this->TaskGuid << "\t";
	oss << this->TaskType << "\t";
	oss << this->DayNowMTUCount << "\t";
	oss << this->DayStartMTULimit << "\t";
	oss << this->DayMTUDistCount << "\t";
	oss << this->DayNowMTULimit << "\t";
	oss << date_time::now().format_date();
	return oss.str();
}

int Task::GetCountRate()
{
	if (DayMTUDistCount == 0) return 0;
	return (int)( 100 * ((double)DayNowMTUCount / (double)DayMTUDistCount) );
}

int Task::GetCountSimRate()
{
	if (DayMTUDistCount == 0) return 0;
	return (int)( 100* ((double)(DayNowMTUCount+DayStartMTULimit) / (double) DayMTUDistCount) );
}

string Task::GeneralShowString()
{

	// 例如：
	// -9:({00000008-FFFF-FFFF-FFFF-FFFFFFFFFFF2}) Today: 1024765:25005341|44444444(1025349) [2%] [-40%]

	ostringstream oss;
	oss << "-" << TaskType << ":(";	
	oss << this->TaskGuid << ") ";

	int dayNowMTUCountSim = DayNowMTUCount+DayStartMTULimit;

	oss << "Today: "<<DayNowMTUCount<<":"<<dayNowMTUCountSim<<"|"<<DayMTUDistCount<<"("<<DayNowMTULimit<<") "
			<<"["<< this->GetCountRate() <<"%] ["<< this->GetCountSimRate() <<"%]";
	return oss.str();
}

int Task::MakeMTU(int count, int mtuCount)
{
	double recvRate = 1.0;
	for(int i = 0; i< mtuCount; i ++ )
		recvRate *= 0.9;
	return (int)(count/recvRate);
}

int Task::FromMTU(int count, int mtuCount)
{
	double recvRate = 1.0;
	for(int i = 0; i< mtuCount; i ++ )
		recvRate *= 0.9;
	return (int)(count*recvRate);
}

void Task::UserShow()
{
	// 例如：
	// -10:({FFFFFFFF-FFFF-FFFF-FFFF-FF400000000F})    T 3119281:388888888(8547866) [0%]

	int DayNowPrecent = DayMTUDistCount > 0 ? this->GetCountRate() : 0;
	printf("-%d:(%s)	T %d:%d(%d) [%d%%] \r\n", TaskType, this->TaskGuid.ToString().c_str(), 
		DayNowMTUCount, DayMTUDistCount, DayNowMTULimit, DayNowPrecent);
}

void Task::UserShowDetail()
{
	printf("-%d:(%s)	Today %d:%d(%d) [%d%%] \r\nGroup:%d:%f Channel:%d RequestType:%d\r\n", TaskType, this->TaskGuid.ToString().c_str(), 
		DayNowMTUCount, DayMTUDistCount, DayNowMTULimit, this->GetCountRate(), TaskGroupID, TaskGroupRate, ChannelSet.size(), RequestTypeSet.size());
}

void Task::LoadAddressListFromLine(const string& line)
{
	this->AddressList.clear();
	this->ForbiddenAddressList.clear();

	if( line.size() == 0 )
		return;

	string allow_address = "";
	string forbid_address = "";

	if(line.find('-') == string::npos)
		allow_address = line;
	else
	{
		vector<string> lines = splite(line,"-");
		if(lines.size() == 2)
		{
			allow_address = lines[0];
			forbid_address = lines[1];
		}
		else return;
	}

	do
	{
		if(forbid_address == "") break;
		vector<string> lines = splite(forbid_address,"|");
		for(vector<string>::iterator iter=lines.begin(); iter!=lines.end(); iter++)
		{
			string address_string=*iter;
			vector<string> strs=splite( address_string,":");
			if(strs.size() != 3)
				continue;
			try
			{
				NET_TYPE nt;
				nt.Country = boost::lexical_cast<u_short>(strs[0]);
				nt.Province = boost::lexical_cast<u_short>(strs[1]);
				nt.ISP = 0;
				nt.City = boost::lexical_cast<u_short>(strs[2]);
				this->ForbiddenAddressList.push_back(nt);
			}
			catch (...)
			{
				continue;
			}
		}
	} while(false);

	do
	{
		if(allow_address == "") break;
		vector<string> lines = splite(allow_address,"|");
		for(vector<string>::iterator iter=lines.begin(); iter!=lines.end(); iter++)
		{
			string address_string=*iter;
			vector<string> strs=splite( address_string,":");
			if(strs.size() != 3)
				continue;
			try
			{
				NET_TYPE nt;
				nt.Country = boost::lexical_cast<u_short>(strs[0]);
				nt.Province = boost::lexical_cast<u_short>(strs[1]);
				nt.ISP = 0;
				nt.City = boost::lexical_cast<u_short>(strs[2]);
				this->AddressList.push_back(nt);
			}
			catch (...)
			{
				continue;
			}
		}
	} while(false);
}

TaskStatus::TaskStatus()
: DayNowMTUCount(0)
{
}

bool TaskStatus::MakeFormString(const string& line, TaskStatus& taskStatus)
{
	vector<string> lines = splite(line, "\t");
	if(lines.size() != 3)
		return NULL;
	// <TaskGUID>	<DayNowMTUCount>	<Day >
	if( taskStatus.TaskGuid.Parse(lines[0]) == false )
		return false;
	try { taskStatus.DayNowMTUCount = boost::lexical_cast<int>(lines[1]); } catch (...) { return false; }
	taskStatus.Day = lines[2];
	return true;
}

bool TaskStatus::IsToday()
{
	return date_time::now().format_date() == this->Day;
}