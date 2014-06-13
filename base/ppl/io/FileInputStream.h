
#pragma once


#include "InputStream.h"
class File;


/// �ļ�������
class FileInputStream : public InputStream
{
public:
	explicit FileInputStream(File& impl) : m_impl(impl) { }

	virtual bool IsOpen() const;

	virtual void Close();

	/// ��ȡʵ�ֶ�д�����Ķ���
	File& GetImpl() { return m_impl; }

protected:
	virtual size_t DoRead(void* buffer, size_t size);
	virtual bool DoReadN(void* buffer, size_t size);

protected:
	/// ʵ�ֶ�д�������ļ�����
	File& m_impl;
};






bool FileInputStream::IsOpen() const
{
	return m_impl.IsOpen();
}

void FileInputStream::Close()
{
	m_impl.Close();
}


size_t FileInputStream::DoRead(void* buffer, size_t size) { return m_impl.Read(buffer, size); }
bool FileInputStream::DoReadN(void* buffer, size_t size) { return m_impl.ReadN(buffer, size); }
