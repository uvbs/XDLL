
#pragma once

#include <ppl/io/InputStream.h>
#include <ppl/io/OutputStream.h>


/// 协议报文使用的数据输入流，使用little-endian，如果需要改变字节序，就改这里
class PacketInputStream : public MemoryDataInputStream
{
public:
	PacketInputStream( const void* data, size_t size ) : MemoryDataInputStream( data, size, false )
	{
	}
};

/// 协议报文使用的数据输出流，使用little-endian，如果需要改变字节序，就改这里
class PacketOutputStream : public MemoryDataOutputStream
{
public:
	PacketOutputStream( void* data, size_t size ) : MemoryDataOutputStream( data, size, false )
	{
	}
};


