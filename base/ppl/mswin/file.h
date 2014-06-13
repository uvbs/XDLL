
#pragma once

#include <ppl/util/log.h>
#include <ppl/stl/stream.h>
#include <boost/noncopyable.hpp>


/// win32的文件操作类(使用操作系统的接口)
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

	/// 获取文件句柄
	HANDLE GetHandle() const { return m_handle; }

	/// 文件句柄是否有效
	bool IsOpen() const { return m_handle != INVALID_HANDLE_VALUE; }

	/// 关闭文件
	void Close();

	/// 是否到了文件末尾
	bool IsEOF() const;

	/// 打开文件
	bool Open(LPCTSTR path, DWORD access, DWORD shareMode, DWORD creationDisposition);

	/// 打开文件供读取
	bool OpenRead(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE, DWORD creationDisposition = OPEN_EXISTING);

	/// 打开文件供写入
	bool OpenWrite(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ, DWORD creationDisposition = OPEN_ALWAYS);

	/// 打开文件供追加数据
	bool OpenAppend(LPCTSTR path, DWORD shareMode = 0, DWORD creationDisposition = OPEN_ALWAYS);

	/// 读取数据
	size_t Read(void* buffer, size_t size);
	/// 读取指定长度的数据，如果读到的数据不够，则失败
	bool ReadN(void* buffer, size_t size)
	{
		size_t len = Read(buffer, size);
		assert(len <= size);
		return len == size;
	}

	/// 写入缓冲区数据
	size_t Write(const void* data, size_t size);

	/// 写入结构体数据
	template <typename StructT>
	bool WriteStruct(const StructT& buffer)
	{
		size_t size = Write(&buffer, sizeof(StructT));
		assert(size <= sizeof(StructT));
		return size == sizeof(StructT);
	}

	/// 读取结构体数据
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


	/// 可变参数的写数据
	bool WriteV(const TCHAR* format, va_list args);

	/// 可变参数的写数据
	bool WriteF(const TCHAR* format, ...);

	/// 刷新缓冲区
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
	/// 文件句柄
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


