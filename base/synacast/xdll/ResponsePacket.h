
#pragma once

#include <synacast/xdll/Protocol.h>
#include <ppl/std/inttypes.h>
#include <ppl/util/buffer.h>
#include <boost/noncopyable.hpp>
#include <string>
using std::string;


/// ����ͷ��
class ResponseHeader : public Serializable
{
public:
	/// ��һ������XDRequest����η���XDRequest�ļ��ʱ�䣬��λΪs
	UINT32 DelayTime;
	/// ��һ������XDRequest����η���XDRequest�ļ��ʱ�䣬��λΪs
	UINT8 TaskType;
	/// ������������ݵĳ���
	UINT16 TaskLength;

	ResponseHeader( ) : DelayTime( 0 ), TaskType( 0 ), TaskLength( 0 )
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
		return sizeof(UINT32) + sizeof(UINT8) + sizeof(UINT16);
	}
};


class ResponsePacketBuilder : private boost::noncopyable
{
public:
	enum { MAX_PACKET_SIZE = 16 * 1024 };
	ResponsePacketBuilder()
	{
		m_buffer.reserve( 4 * 1024 );
	}

	void Build( Serializable& body )
	{
		ResponseHeader header;
		header.Action = body.GetAction();
		header.Version = XD_PROTOCOL_VERSION;
		header.SequenceID = m_sequenceID.Get( sequenceID );
		size_t totalSize = header.GetObjectSize() + body.GetObjectSize();
		if ( totalSize > MAX_PACKET_SIZE )
		{
			throw std::overflow_error("packet body is too long");
		}
		m_buffer.resize( totalSize );
		PacketOutputStream os( m_buffer.data(), m_buffer.size() );
		os << header << body;
	}
	const BYTE* GetData() const { return m_buffer.data(); }
	size_t GetSize() const { return m_buffer.size(); }

private:
	byte_buffer m_buffer;
};

