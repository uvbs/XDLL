
#pragma once

#include <ppl/std/inttypes.h>
#include <ppl/io/ByteOrder.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <string>
#include <assert.h>
using std::string;


/*
class OutputStream
{
public:
	virtual ~OutputStream() { }

	/// 读取数据
	size_t write(const void* buffer, size_t size) { return do_write(buffer, size); }

	/// 读取结构体数据
	template <typename StructT>
	void write_struct(const StructT& buffer)
	{
		return write(&buffer, sizeof(StructT)) == sizeof(StructT);
	}
	/// 读取结构体数据
	template <typename ValueT>
	void write_value(ValueT val)
	{
		return write(&val, sizeof(ValueT)) == sizeof(ValueT);
	}

protected:
	/// 实现读取操作
	virtual size_t do_write(const void* buffer, size_t size) = 0;
};
*/


class OutputStream
{
public:
	OutputStream() { }
	virtual ~OutputStream() { }

	virtual void write_n( const void* data, size_t size ) = 0;
	virtual void write_byte( BYTE val ) = 0;

	virtual size_t get_written() const = 0;
};





class MemoryOutputStream : public OutputStream, private boost::noncopyable
{
public:
	explicit MemoryOutputStream(void* buf, size_t capacity) : m_buffer(static_cast<BYTE*>(buf)), m_capacity(capacity), m_size(0)
	{
		assert(m_buffer != NULL && m_capacity > 0);
		assert( ! ::IsBadWritePtr(m_buffer, m_capacity));
	}

	size_t capacity() const { return m_capacity; }
	size_t size() const { return m_size; }

	virtual void write_n( const void* data, size_t size )
	{
		this->write_bytes( data, size );
	}

	virtual void write_byte( BYTE val )
	{
		check_available( 1 );
		m_buffer[m_size] = val;
		m_size += 1;
	}

	virtual size_t get_written() const
	{
		return m_size;
	}

	void check_available(size_t size)
	{
		if ( false == this->is_available(size) )
			throw std::overflow_error("memory output stream overflow");
	}

	bool is_available(size_t size)
	{
		return (m_size + size <= m_capacity);
	}

	template <typename ValueT>
	void write_value(ValueT val)
	{
		check_available(sizeof(ValueT));
		WRITE_MEMORY(m_buffer + m_size, val, ValueT);
		m_size += sizeof(ValueT);
	}

	void write_bytes(const void* data, size_t size)
	{
		check_available(size);
		if (size > 0)
		{
			memcpy(m_buffer + m_size, data, size);
			m_size += size;
		}
	}


	template <typename ValueT>
	void write_array(const ValueT* vals, size_t count)
	{
		write_bytes(vals, sizeof(ValueT) * count);
	}

	template <typename ValueT, typename AllocatorT>
	void write_vector(const std::vector<ValueT, AllocatorT>& vals)
	{
		if (!vals.empty())
		{
			write_array(&vals[0], vals.size());
		}
	}

	template <typename StructT>
	void write_struct(const StructT& val)
	{
		write_bytes(&val, sizeof(StructT));
	}

	template <typename CharT, typename CharTraitsT, typename AllocatorT>
	void write_string(const std::basic_string<CharT, CharTraitsT, AllocatorT>& s)
	{
		this->write_bytes( s.data(), s.size() * sizeof( CharT ) );
	}

	void write_string(const char* s)
	{
		this->write_bytes( s, strlen(s) );
	}

	void write_char(char val) { write_value(val); }
	void write_short(short val) { write_value(val); }
	void write_int(int val) { write_value(val); }
	void write_long(long val) { write_value(val); }
	void write_longlong(longlong val) { write_value(val); }

	//void write_byte(BYTE val) { write_value(val); }
	void write_word(unsigned short val) { write_value(val); }
	void write_dword(unsigned long val) { write_value(val); }
	void write_qword(ulonglong val) { write_value(val); }

	void write_int8(signed char val) { write_value(val); }
	void write_uint8(BYTE val) { write_value(val); }

	void write_int16(short val) { write_value(val); }
	void write_uint16(unsigned short val) { write_value(val); }

	void write_int32(long val) { write_value(val); }
	void write_uint32(unsigned long val) { write_value(val); }

	void write_int64(longlong val) { write_value(val); }
	void write_uint64(ulonglong val) { write_value(val); }

	template <typename ObjT>
	void write_object( const ObjT& obj )
	{
		obj.write_object( *this );
	}

private:
	BYTE* m_buffer;
	size_t m_capacity;
	size_t m_size;
};






class DataOutputStream : private boost::noncopyable
{
public:
	explicit DataOutputStream( OutputStream* os, bool bigEndian ) : m_out( os ), m_bigEndian( bigEndian )//, m_good( true )
	{
	}

	void attach( OutputStream& out )
	{
		m_out = &out;
	}

	OutputStream& get_output() const
	{
		return *m_out;
	}

	bool is_big_endian() const
	{
		return m_bigEndian;
	}

	size_t get_written() const
	{
		return m_out->get_written();
	}

	//typedef bool DataOutputStream::*unspecified_bool_type;

	//operator unspecified_bool_type() const
	//{
	//	return m_good ? &DataOutputStream::m_good : 0;
	//}


	void write_byte( BYTE val )
	{
		m_out->write_byte( val );
	}

	void write_n( const void* buf, size_t size )
	{
		m_out->write_n( buf, size );
	}

	void write_string( const string& s )
	{
		this->write_n( s.data(), s.size() );
	}

	void write_uint8( BYTE val )
	{
		write_byte( val );
	}

	void write_uint16( unsigned short val )
	{
		if ( m_bigEndian )
		{
			val = ByteOrder::reverse_uint16( val );
		}
		write_n( &val, 2 );
	}

	void write_uint32( unsigned long val )
	{
		if ( m_bigEndian )
		{
			val = ByteOrder::reverse_uint32( val );
		}
		write_n( &val, 4 );
	}

	void write_uint64( UINT64 val )
	{
		if ( m_bigEndian )
		{
			val = ByteOrder::reverse_uint64( val );
		}
		write_n( &val, 8 );
	}

	void write_int8( signed char val )
	{
		write_uint8( static_cast<BYTE>( val ) );
	}

	void write_int16( short val )
	{
		write_uint16( static_cast<unsigned short>( val ) );
	}

	void write_int32( long val )
	{
		write_uint32( static_cast<unsigned long>( val ) );
	}

	void write_int64( INT64 val )
	{
		write_uint64( static_cast<UINT64>( val ) );
	}


private:
	OutputStream* m_out;
	bool m_bigEndian;
	//bool m_good;
};


inline DataOutputStream& operator<<( DataOutputStream& os, signed char val )
{
	os.write_int8( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, char val )
{
	os.write_int8( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, BYTE val )
{
	os.write_uint8( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, signed short val )
{
	os.write_int16( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, unsigned short val )
{
	os.write_uint16( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, int val )
{
	os.write_int32( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, unsigned int val )
{
	os.write_uint32( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, long val )
{
	os.write_int32( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, unsigned long val )
{
	os.write_uint32( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, INT64 val )
{
	os.write_int64( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, UINT64 val )
{
	os.write_uint64( val );
	return os;
}

inline DataOutputStream& operator<<( DataOutputStream& os, const string& s )
{
	os.write_string( s );
	return os;
}


class MemoryDataOutputStream : public DataOutputStream
{
public:
	MemoryDataOutputStream( void* data, size_t size, bool bigEndian ) : DataOutputStream( NULL, bigEndian ), m_mis( data, size )
	{
		this->attach( m_mis );
	}

private:
	MemoryOutputStream m_mis;
};





