
#pragma once

#include <ppl/std/inttypes.h>
#include <ppl/util/macro.h>
#include <ppl/io/ByteOrder.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <string>
#include <assert.h>

/*
/// 输入流
class InputStream
{
public:
	InputStream() { }
	virtual ~InputStream() { }

	/// 流是否打开
	virtual bool IsOpen() const = 0;

	/// 关闭流
	virtual void Close() = 0;

	int Read()
	{
		BYTE val;
		size_t count = this->Read(&val, 1);
		if (count == 0)
			return -1;
		return val;
	}

	/// 读取数据
	size_t Read(void* buffer, size_t size) { return DoRead(buffer, size); }
	/// 读取指定长度的数据，如果读到的数据不够，则失败
	bool ReadN(void* buffer, size_t size) { return DoReadN(buffer, size); }

	/// 读取结构体数据
	template <typename StructT>
	bool ReadStruct(StructT& buffer)
	{
		return ReadN(&buffer, sizeof(StructT));
	}

protected:
	/// 实现读取操作
	virtual size_t DoRead(void* buffer, size_t size) = 0;
	/// 实现读取操作
	virtual bool DoReadN(void* buffer, size_t size) = 0;
};
*/



/*
class MemoryInputStream : private noncopyable
{
public:
	explicit MemoryInputStream(const void* buf, size_t size) : m_buffer(static_cast<const BYTE*>(buf)), m_capacity(capacity), m_size(0)
	{
		assert(m_buffer != NULL && m_capacity > 0);
		assert( ! ::IsBadWritePtr(m_buffer, m_capacity));
	}

	size_t capacity() const { return m_capacity; }
	size_t size() const { return m_size; }

	void check_available(size_t size)
	{
		if (m_size + size > m_capacity)
			throw std::overflow_error("");
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

	template <typename T, typename AllocT>
	void write_buffer(const basic_buffer<T, AllocT>& buf)
	{
		this->write_array(buf.data(), buf.size());
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

	void write_char(char val) { write_value(val); }
	void write_short(short val) { write_value(val); }
	void write_int(int val) { write_value(val); }
	void write_long(long val) { write_value(val); }
	void write_longlong(longlong val) { write_value(val); }

	void write_byte(BYTE val) { write_value(val); }
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

private:
	BYTE* m_buffer;
	size_t m_capacity;
	size_t m_size;
};
*/


class InputStream
{
public:
	InputStream() { }
	virtual ~InputStream() { }

	virtual bool read_n( void* buf, size_t size ) = 0;
	virtual bool read_byte( BYTE& val ) = 0;
	virtual size_t available() const = 0;
	virtual bool is_available( size_t size ) const
	{
		return this->available() >= size;
	}

	virtual bool ignore_n( size_t size ) = 0;
};


class MemoryInputStream : public InputStream, private boost::noncopyable
{
public:
	explicit MemoryInputStream(const void* buf, size_t size) : m_buffer(static_cast<const BYTE*>(buf)), m_size(size), m_position(0)
	{
		assert(m_buffer != NULL);
		//assert( ! ::IsBadReadPtr(m_buffer, m_size));
	}

	//size_t position() const { return m_position; }
	//size_t size() const { return m_size; }

	virtual bool read_n( void* buf, size_t size )
	{
		return this->read_bytes( buf, size );

	}
	virtual bool read_byte( BYTE& val )
	{
		if ( false == is_available( 1 ) )
			return false;
		val = m_buffer[m_position];
		++m_position;
		return true;
	}

	virtual size_t available() const
	{
		if ( m_size >= m_position )
			return m_size - m_position;
		return 0;
	}
	virtual bool is_available( size_t size ) const
	{
		return (m_position + size <= m_size);
	}

	virtual bool ignore_n( size_t size )
	{
		if ( false == is_available( size ) )
			return false;
		assert(size > 0);
		return true;
	}

	//void check_available(size_t size)
	//{
	//	assert( is_available( size ) );
	//}

	template <typename ValueT>
	bool read_value(ValueT& val)
	{
		if ( false == is_available( sizeof(ValueT) ) )
			return false;
		val = READ_MEMORY(m_buffer + m_position, ValueT);
		m_position += sizeof(ValueT);
		return true;
	}

	bool read_bytes(void* data, size_t size)
	{
		if ( false == is_available(size) )
			return false;
		if (size > 0)
		{
			memcpy(data, m_buffer + m_position, size);
			m_position += size;
		}
		return true;
	}

	//template <typename T, typename AllocT>
	//void write_buffer(const basic_buffer<T, AllocT>& buf)
	//{
	//	this->write_array(buf.data(), buf.size());
	//}


	template <typename ValueT>
	bool read_array(ValueT* vals, size_t count)
	{
		if ( 0 == count )
			return true;
		return read_bytes(vals, sizeof(ValueT) * count);
	}

	template <typename ValueT, typename AllocatorT>
	bool read_vector(size_t count, std::vector<ValueT, AllocatorT>& vals)
	{
		if ( 0 == count )
			return true;
		if ( false == is_available( sizeof(ValueT) * count ) )
		{
			assert(false);
			return false;
		}
		// 先检查数据是否充足，再进行resize操作，避免count过大，resize耗尽内存
		vals.resize( count );
		if ( read_array( &vals[0], count ) )
			return true;
		vals.clear();
		return false;
	}

	template <typename StructT>
	bool read_struct(StructT& val)
	{
		return read_bytes(&val, sizeof(StructT));
	}

	template <typename CharT, typename CharTraitsT, typename AllocatorT>
	bool read_string(size_t count, std::basic_string<CharT, CharTraitsT, AllocatorT>& s)
	{
		if ( 0 == count )
		{
			s.clear();
			return true;
		}
		// 先检查数据是否充足，再进行resize操作，避免count过大，resize耗尽内存
		if ( false == is_available( count * sizeof(CharT) ) )
		{
			assert(false);
			return false;
		}
		s.resize( count );
		if ( read_array( &s[0], count ) )
			return true;
		s.clear();
		return false;
	}

	bool read_char(char& val) { return read_value(val); }
	bool read_short(short& val) { return read_value(val); }
	bool read_int(int& val) { return read_value(val); }
	bool read_long(long& val) { return read_value(val); }
	bool read_longlong(longlong& val) { return read_value(val); }

	bool read_uchar(BYTE& val) { return read_value(val); }
	bool read_ushort(unsigned short& val) { return read_value(val); }
	bool read_uint(unsigned int& val) { return read_value(val); }
	bool read_ulong(unsigned long& val) { return read_value(val); }
	bool read_ulonglong(ulonglong& val) { return read_value(val); }

	//bool read_byte(BYTE& val) { return read_value(val); }
	bool read_word(unsigned short& val) { return read_value(val); }
	bool read_dword(unsigned long& val) { return read_value(val); }
	bool read_qword(ulonglong& val) { return read_value(val); }

	bool read_int8(signed char& val) { return read_value(val); }
	bool read_uint8(BYTE& val) { return read_value(val); }

	bool read_int16(short& val) { return read_value(val); }
	bool read_uint16(unsigned short& val) { return read_value(val); }

	bool read_int32(int& val) { return read_value(val); }
	bool read_uint32(unsigned int& val) { return read_value(val); }

	bool read_int64(longlong& val) { return read_value(val); }
	bool read_uint64(ulonglong& val) { return read_value(val); }

	template <typename ObjT>
	bool read_object( ObjT& obj )
	{
		return obj.read_object( *this );
	}

private:
	const BYTE* m_buffer;
	const size_t m_size;
	size_t m_position;
};



class DataInputStream : private boost::noncopyable
{
public:
	explicit DataInputStream( InputStream* is, bool bigEndian ) : m_in( is ), m_bigEndian( bigEndian ), m_good( is != NULL )
	{
	}

	typedef bool DataInputStream::*unspecified_bool_type;

	operator unspecified_bool_type() const
	{
		return m_good ? &DataInputStream::m_good : 0;
	}

	bool operator!() const
	{
		return ! m_good;
	}

	/// 所有的read操作，必须先检查m_good，如果m_good为false，读取失败
	bool good() const
	{
		return m_good;
	}

	void set_good()
	{
		m_good = true;
	}

	void attach( InputStream& is )
	{
		m_in = &is;
		m_good = true;
	}

	InputStream& get_input() const
	{
		return *m_in;
	}

	bool is_big_endian() const
	{
		return m_bigEndian;
	}

	size_t available() const
	{
		if ( false == m_good )
			return 0;
		return m_in->available();
	}
	bool is_available( size_t size ) const
	{
		if ( false == m_good )
			return false;
		return m_in->is_available( size );
	}

	bool ignore_n( size_t size )
	{
		if ( false == m_good )
			return false;
		m_good = m_in->ignore_n( size );
		return m_good;
	}


	/// try_read_n失败后，需设置m_good为false，供读取string/vector等对象时使用
	bool try_read_n( size_t size )
	{
		if ( is_available( size ) )
			return true;
		m_good = false;
		assert(false);
		return false;
	}

	bool read_byte( BYTE& val )
	{
		// 如果状态已经为bad，则为上次读取失败，不能再继续读
		if ( false == m_good )
			return false;
		m_good = m_in->read_byte( val );
		return m_good;
	}

	bool read_n( void* buf, size_t size )
	{
		// 如果状态已经为bad，则为上次读取失败，不能再继续读
		if ( false == m_good )
			return false;
		m_good = m_in->read_n( buf, size );
		return m_good;
	}

	bool read_string(size_t count, std::string& s)
	{
		// 先检查数据是否充足，再进行resize操作，避免count过大，resize耗尽内存
		// 并且如果m_good为false,即使count为0，也返回false，表示失败
		if ( try_read_n( count ) )
		{
			if ( 0 == count )
			{
				s.erase();
				return true;
			}
			s.resize( count );
			if ( read_n( &s[0], count ) )
				return true;
		}
		return false;
	}

	bool read_byte_vector(size_t count, std::vector<unsigned char>& s)
	{
		// 先检查数据是否充足，再进行resize操作，避免count过大，resize耗尽内存
		// 并且如果m_good为false,即使count为0，也返回false，表示失败
		if ( try_read_n( count ) )
		{
			if ( 0 == count )
			{
				s.clear();
				return true;
			}
			s.resize( count );
			if ( read_n( &s[0], count ) )
				return true;
		}
		return false;
	}

	bool read_char_vector(size_t count, std::vector<char>& s)
	{
		return read_byte_vector( count, reinterpret_cast<std::vector<unsigned char>&>( s ) );
	}

	bool read_uint8( BYTE& val )
	{
		return read_byte( val );
	}

	bool read_uint16( unsigned short& val )
	{
		if ( false == read_n( &val, 2 ) )
			return false;
		if ( m_bigEndian )
		{
			val = ByteOrder::reverse_uint16( val );
		}
		return true;
	}

	bool read_uint32( unsigned long& val )
	{
		if ( false == read_n( &val, 4 ) )
			return false;
		if ( m_bigEndian )
		{
			val = ByteOrder::reverse_uint32( val );
		}
		return true;
	}

	bool read_uint32( unsigned int& val )
	{
		if ( false == read_n( &val, 4 ) )
			return false;
		if ( m_bigEndian )
		{
			val = ByteOrder::reverse_uint32( val );
		}
		return true;
	}

	bool read_uint64( UINT64& val )
	{
		if ( false == read_n( &val, 8 ) )
			return false;
		if ( m_bigEndian )
		{
			val = ByteOrder::reverse_uint64( val );
		}
		return true;
	}

	bool read_int8( signed char& val )
	{
		return read_uint8( reinterpret_cast<BYTE&>( val ) );
	}

	bool read_int16( short& val )
	{
		return read_uint16( reinterpret_cast<unsigned short&>( val ) );
	}

	bool read_int32( long& val )
	{
		return read_uint32( reinterpret_cast<unsigned long&>( val ) );
	}

	bool read_int32( int& val )
	{
		return read_uint32( reinterpret_cast<unsigned int&>( val ) );
	}

	bool read_int64( INT64& val )
	{
		return read_uint64( reinterpret_cast<UINT64&>( val ) );
	}

private:
	InputStream* m_in;
	bool m_bigEndian;
	/// 状态是否为good，如果为false，表示上次读取已经失败，后续的读取都失败
	bool m_good;
};


inline DataInputStream& operator>>( DataInputStream& is, signed char& val )
{
	is.read_int8( val );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, char& val )
{
	is.read_int8( reinterpret_cast<signed char&>( val ) );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, BYTE& val )
{
	is.read_uint8( val );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, signed short& val )
{
	is.read_int16( val );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, unsigned short& val )
{
	is.read_uint16( val );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, int& val )
{
	is.read_int32( reinterpret_cast<long&>( val ) );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, unsigned int& val )
{
	is.read_uint32( reinterpret_cast<unsigned long&>( val ) );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, long& val )
{
	is.read_int32( val );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, unsigned long& val )
{
	is.read_uint32( val );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, INT64& val )
{
	is.read_int64( val );
	return is;
}

inline DataInputStream& operator>>( DataInputStream& is, UINT64& val )
{
	is.read_uint64( val );
	return is;
}

class MemoryDataInputStream : public DataInputStream
{
public:
	MemoryDataInputStream( const void* data, size_t size, bool bigEndian ) : DataInputStream( NULL, bigEndian ), m_mis( data, size )
	{
		this->attach( m_mis );
	}

private:
	MemoryInputStream m_mis;
};


