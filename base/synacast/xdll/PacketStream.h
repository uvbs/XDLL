
#pragma once

#include <ppl/io/InputStream.h>
#include <ppl/io/OutputStream.h>


/// Э�鱨��ʹ�õ�������������ʹ��little-endian�������Ҫ�ı��ֽ��򣬾͸�����
class PacketInputStream : public MemoryDataInputStream
{
public:
	PacketInputStream( const void* data, size_t size ) : MemoryDataInputStream( data, size, false )
	{
	}
};

/// Э�鱨��ʹ�õ������������ʹ��little-endian�������Ҫ�ı��ֽ��򣬾͸�����
class PacketOutputStream : public MemoryDataOutputStream
{
public:
	PacketOutputStream( void* data, size_t size ) : MemoryDataOutputStream( data, size, false )
	{
	}
};


