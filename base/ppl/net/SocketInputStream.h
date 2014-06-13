
#pragma once


#include "InputStream.h"

class SyncSocket;


/// socket������
class SocketInputStream : public InputStream
{
public:
	explicit SocketInputStream(SyncSocket& impl);

	virtual bool IsOpen() const;

	virtual void Close();

	/// ��ȡʵ�ֶ�д�����Ķ���
	SyncSocket& GetImpl() { return m_impl; }

protected:
	virtual size_t DoRead(void* buffer, size_t size);
	virtual bool DoReadN(void* buffer, size_t size);

protected:
	/// ʵ�ֶ�д�������ļ�����
	SyncSocket& m_impl;

	/// ������
	dynamic_buffer m_buffer;

	/// �������ĵ�ǰλ��
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
	// ʹ��8K�Ļ�������С
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



