
#pragma once


#include "InputStream.h"

class SyncSocket;


/// socket输入流
class SocketInputStream : public InputStream
{
public:
	explicit SocketInputStream(SyncSocket& impl);

	virtual bool IsOpen() const;

	virtual void Close();

	/// 获取实现读写操作的对象
	SyncSocket& GetImpl() { return m_impl; }

protected:
	virtual size_t DoRead(void* buffer, size_t size);
	virtual bool DoReadN(void* buffer, size_t size);

protected:
	/// 实现读写操作的文件对象
	SyncSocket& m_impl;

	/// 缓冲区
	dynamic_buffer m_buffer;

	/// 缓冲区的当前位置
	size_t m_pos;
};




#include "SyncSocket.h"


const size_t MAX_BUFFER_SIZE = 1024 * 8;

/*
invariant:
m_pos < m_buffer.size() or (m_pos == 0 and m_buffer.size() == 0)
*/


SocketInputStream::SocketInputStream(SyncSocket& impl) : m_impl(impl), m_pos(0)
{
	// 使用8K的缓冲区大小
	m_buffer.reserve(MAX_BUFFER_SIZE);
}


bool SocketInputStream::IsOpen() const
{
	return m_impl.IsOpen();
}

void SocketInputStream::Close()
{
	m_impl.Close();
}


size_t SocketInputStream::DoRead(void* buffer, size_t size)
{
//	if (m_pos >= m_buffer.size())
	{
		return m_impl.Receive(buffer, size);
	}
}

bool SocketInputStream::DoReadN(void* buffer, size_t size)
{
	return m_impl.ReceiveN(buffer, size);
}



