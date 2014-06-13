
#pragma once

#include <ppl/util/log.h>
#include <ppl/stl/stream.h>
#include <boost/noncopyable.hpp>


/// win32���ļ�������(ʹ�ò���ϵͳ�Ľӿ�)
class File : private boost::noncopyable
{
public:
	explicit File(HANDLE hFile = INVALID_HANDLE_VALUE) : m_handle(hFile)
	{
	}
	~File()
	{
		Close();
	}

	/// ��ȡ�ļ����
	HANDLE GetHandle() const { return m_handle; }

	/// �ļ�����Ƿ���Ч
	bool IsOpen() const { return m_handle != INVALID_HANDLE_VALUE; }

	/// �ر��ļ�
	void Close();

	/// �Ƿ����ļ�ĩβ
	bool IsEOF() const;

	/// ���ļ�
	bool Open(LPCTSTR path, DWORD access, DWORD shareMode, DWORD creationDisposition);

	/// ���ļ�����ȡ
	bool OpenRead(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE, DWORD creationDisposition = OPEN_EXISTING);

	/// ���ļ���д��
	bool OpenWrite(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ, DWORD creationDisposition = OPEN_ALWAYS);

	/// ���ļ���׷������
	bool OpenAppend(LPCTSTR path, DWORD shareMode = 0, DWORD creationDisposition = OPEN_ALWAYS);

	/// ��ȡ����
	size_t Read(void* buffer, size_t size);
	/// ��ȡָ�����ȵ����ݣ�������������ݲ�������ʧ��
	bool ReadN(void* buffer, size_t size)
	{
		size_t len = Read(buffer, size);
		assert(len <= size);
		return len == size;
	}

	/// д�뻺��������
	size_t Write(const void* data, size_t size);

	/// д��ṹ������
	template <typename StructT>
	bool WriteStruct(const StructT& buffer)
	{
		size_t size = Write(&buffer, sizeof(StructT));
		assert(size <= sizeof(StructT));
		return size == sizeof(StructT);
	}

	/// ��ȡ�ṹ������
	template <typename StructT>
	bool ReadStruct(StructT& buffer)
	{
		return ReadN(&buffer, sizeof(StructT));
	}

	unsigned char ReadByte()
	{
		unsigned char ch = 0;
		size_t bytes = Read(&ch, 1);
		assert(bytes == 1);
		return ch;
	}


	/// �ɱ������д����
	bool WriteV(const TCHAR* format, va_list args);

	/// �ɱ������д����
	bool WriteF(const TCHAR* format, ...);

	/// ˢ�»�����
	bool Flush();

	bool Seek(LONG distance, DWORD moveMethod);
	bool SeekToBegin(LONG distance = 0);
	bool SeekToEnd(LONG distance = 0);
	bool SeekToCurrent(LONG distance);

	DWORD GetSize()
	{
		return ::GetFileSize(m_handle, NULL);
	}

	INT64 GetSize64()
	{
		LARGE_INTEGER size;
		size.QuadPart = 0;
		if ( ::GetFileSizeEx( m_handle, &size ) )
		{
			assert( size.QuadPart >= 0 );
			return size.QuadPart;
		}
		assert(false);
		return 0;
	}

private:
	/// �ļ����
	HANDLE m_handle;
};



#ifndef INVALID_SET_FILE_POINTER
const DWORD INVALID_SET_FILE_POINTER = -1;
#endif


inline void File::Close()
{
	if (IsOpen())
	{
		BOOL success = ::CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
		if (!success)
		{
			UTIL_ERROR("File::Close CloseHandle failed " << ::GetLastError());
			assert(false);
		}
	}
}

inline bool File::Open(LPCTSTR path, DWORD access, DWORD shareMode, DWORD creationDisposition)
{
	assert(!IsOpen());
	Close();
	m_handle = ::CreateFile(path, access, shareMode, NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	if (IsOpen())
		return true;
	UTIL_ERROR("CreateFile failed " << make_tuple(path, ::GetLastError()));
	return false;
}

inline bool File::OpenRead(LPCTSTR path, DWORD shareMode, DWORD creationDisposition)
{
	return this->Open(path, GENERIC_READ, shareMode, creationDisposition);
}

inline bool File::OpenWrite(LPCTSTR path, DWORD shareMode, DWORD creationDisposition)
{
	return this->Open(path, GENERIC_WRITE, shareMode, creationDisposition);
}

inline bool File::OpenAppend(LPCTSTR path, DWORD shareMode, DWORD creationDisposition)
{
	return OpenWrite(path, shareMode, creationDisposition) && SeekToEnd();
}

inline size_t File::Read(void* buffer, size_t size)
{
	assert(buffer != NULL && size > 0);
	assert(IsOpen());
	DWORD bytes = 0;
	if (::ReadFile(m_handle, buffer, size, &bytes, NULL))
	{
		//assert(bytes > 0);
		return bytes;
	}
	UTIL_ERROR("File::Read ReadFile failed " << make_tuple(size, bytes, ::GetLastError()));
	return 0;
}

inline bool File::Seek(LONG distance, DWORD moveMethod)
{
	assert(IsOpen());
	DWORD result = ::SetFilePointer(m_handle, distance, NULL, moveMethod);
	if (result == INVALID_SET_FILE_POINTER)
	{
		DWORD errcode = ::GetLastError();
		UTIL_ERROR("File::Seek SetFilePointer failed " << make_tuple(result, errcode) << make_tuple(distance, moveMethod));
		assert(errcode != ERROR_SUCCESS);
		return false;
	}
	UTIL_ERROR("File::Seek SetFilePointer succeeded " << make_tuple(result, distance, moveMethod));
	return true;
}

inline bool File::SeekToBegin(LONG distance)
{
	return Seek(distance, FILE_BEGIN);
}

inline bool File::SeekToEnd(LONG distance)
{
	return Seek(distance, FILE_END);
}

inline bool File::SeekToCurrent(LONG distance)
{
	return Seek(distance, FILE_CURRENT);
}

inline size_t File::Write(const void* data, size_t size)
{
	DWORD count = 0;
	if (!::WriteFile(GetHandle(), data, size, &count, NULL))
		return 0;
	return count;
}

inline bool File::WriteV(const TCHAR* format, va_list args)
{
	const int max_size = 8 * 1024;
	TCHAR buffer[max_size + 1];
	buffer[max_size] = '\0';
	int count = _vsntprintf(buffer, max_size, format, args);
	if (count <= 0)
	{
		assert(false);
		return false;
	}
	return Write(buffer, count) == static_cast<size_t>( count );
}

inline bool File::WriteF(const TCHAR* format, ...)
{
	va_list args;
	va_start(args, format);
	bool success = WriteV(format, args);
	va_end(args);
	return success;
}


inline bool File::Flush()
{
	return ::FlushFileBuffers(GetHandle()) != FALSE;
}




/*
#ifdef _PPL_RUN_TEST

class FileTestCase : public TestCase
{
	void DoRun()
	{
		File file;
		file.OpenReader("f:\\synet.txt");
		char buffer[2045];
		file.Read(buffer, 2044);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(FileTestCase);

#endif
*/


