
#pragma once

#include <ppl/io/InputStream.h>
#include <ppl/io/OutputStream.h>

#include <boost/noncopyable.hpp>

#include <string>
using std::string;



class packet_string_reader
{
private:
	const packet_string_reader& operator=( const packet_string_reader& );
public:
	string& data;
	explicit packet_string_reader( string& s ) : data( s ) { }
};

class packet_string_writer
{
private:
	const packet_string_writer& operator=( const packet_string_writer& );
public:
	const string& data;
	explicit packet_string_writer( const string& s ) : data( s ) { }
};



inline DataInputStream& operator>>( DataInputStream& is, const packet_string_reader& reader )
{
	reader.data.erase();
	UINT16 len = 0;
	if ( is >> len )
	{
		is.read_string( len, reader.data );
	}
	return is;
}

inline DataOutputStream& operator<<( DataOutputStream& os, const packet_string_writer& writer )
{
	os.write_uint16( writer.data.size() );
	os.write_string( writer.data );
	return os;
}


template <typename T>
class packet_vector_reader
{
private:

	const packet_vector_reader& operator=( const packet_vector_reader& );
public:
	std::vector<T>& items;
	explicit packet_vector_reader( std::vector<T>& d ) : items( d ) { }
};

template <typename T>
class packet_vector_writer
{
private:

	const packet_vector_writer& operator=( const packet_vector_writer& );
public:
	const std::vector<T>& items;
	explicit packet_vector_writer( const std::vector<T>& d ) : items( d ) { }
};



template <typename T>
inline DataInputStream& operator>>( DataInputStream& is, const packet_vector_reader<T>& reader )
{
	reader.items.clear();
	UINT16 len = 0;
	if ( is >> len )
	{
		T item;
		for ( size_t index = 0; index < len; ++index )
		{
			if ( is >> item )
			{
				reader.items.push_back( item );
			}
			else
			{
				break;
			}
		}
	}
	return is;
}

template <typename T>
inline DataOutputStream& operator<<( DataOutputStream& os, const packet_vector_writer<T>& writer )
{
	os.write_uint16( writer.items.size() );
	for ( size_t index = 0; index < writer.items.size(); ++index )
	{
		os << writer.items[index];
	}
	return os;
}



template <>
inline DataOutputStream& operator<<( DataOutputStream& os, const packet_vector_writer<unsigned char>& writer )
{
	os.write_uint16( writer.items.size() );
	os.write_n( &writer.items[0], writer.items.size() );
	return os;
}

template <>
inline DataInputStream& operator>>( DataInputStream& is, const packet_vector_reader<unsigned char>& reader )
{
	reader.items.clear();
	UINT16 len = 0;
	if ( is >> len )
	{
		if ( is.try_read_n( len ) )
		{
			if ( len > 0 )
			{
				reader.items.resize( len );
				is.read_n( &reader.items[0], len );
			}
			else
			{
				reader.items.clear();
			}
		}
	}
	return is;
}

template <>
inline DataOutputStream& operator<<( DataOutputStream& os, const packet_vector_writer<char>& writer )
{
	return os << reinterpret_cast<const packet_vector_writer<unsigned char>&>( writer );
}

template <>
inline DataInputStream& operator>>( DataInputStream& is, const packet_vector_reader<char>& reader )
{
	return is >> reinterpret_cast<const packet_vector_reader<unsigned char>&>( reader );
}



inline packet_string_reader make_packet_reader( string& data )
{
	return packet_string_reader( data );
}

inline packet_string_writer make_packet_writer( const string& data )
{
	return packet_string_writer( data );
}

template <typename T>
inline packet_vector_reader<T> make_packet_reader( std::vector<T>& data )
{
	return packet_vector_reader<T>( data );
}

template <typename T>
inline packet_vector_writer<T> make_packet_writer( const std::vector<T>& data )
{
	return packet_vector_writer<T>( data );
}


