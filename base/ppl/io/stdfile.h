
#pragma once

#include <ppl/std/inttypes.h>
#include <ppl/stl/string.h>
#include <ppl/config.h>
#include <ppl/util/log.h>
#include <boost/noncopyable.hpp>

#include <stdarg.h>

#if defined(_PPL_PLATFORM_MSWIN)
#include <share.h>
#else
inline FILE* _tfsopen(const char* fname, const char* mode, int shareFlag)
{
	return fopen(fname, mode);
}

inline FILE* _fsopen(const char* fname, const char* mode, int shareFlag)
{
	return fopen(fname, mode);
}

#define _SH_DENYRW      0x10    /* deny read/write mode */
#define _SH_DENYWR      0x20    /* deny write mode */
#define _SH_DENYRD      0x30    /* deny read mode */
#define _SH_DENYNO      0x40    /* deny none mode */
#define _SH_SECURE      0x80    /* secure mode */

#endif


/// ��׼C���ļ�������װ��
class StdFile : private boost::noncopyable
{
public:
	StdFile() : m_handle(NULL)
	{
	}
	~StdFile()
	{
		Close();
	}

	/// ��ȡ�ļ����
	FILE* GetHandle() const { return m_handle; }

#if defined(_PPL_PLATFORM_MSWIN)
	longlong GetSize() const
	{
		fpos_t pos = 0;
		if ( 0 == fgetpos( m_handle, &pos ) )
			return pos;
		assert(false);
		return 0;
	}
#elif defined(_PPL_PLATFORM_LINUX)
	ulonglong GetSize() const
	{
		fpos_t pos = { 0 };
		if ( 0 == fgetpos( m_handle, &pos ) )
			return pos.__pos;
		assert(false);
		return 0;
	}
#else
#error "invalid platform for fgetpos"
#endif


	/// �ļ�����Ƿ���Ч
	bool IsOpen() const { return m_handle != NULL; }

	/// �ر��ļ�
	void Close();

	/// �Ƿ����ļ�ĩβ
	bool IsEOF() const;

	bool OpenDescriptor( int fd, const char* mode = "r" )
	{
		assert(!IsOpen());
		this->Close();
		m_handle = fdopen( fd, mode );
		if (IsOpen())
			return true;
		UTIL_DEBUG("fdopen failed " << " with " << fd << " for mode " << mode);
		return false;
	}

	/// ���ļ�
	bool Open(const char* path, const char* mode = "r", int shareFlag = _SH_DENYNO);

#if defined(_PPL_PLATFORM_MSWIN)
	bool Open(const wchar_t* path, const wchar_t* mode = L"r", int shareFlag = _SH_DENYNO);
#endif

	int ReadByte()
	{
		return fgetc(m_handle);
	}

	int WriteByte( int ch )
	{
		return fputc(ch, m_handle);
	}

	/// ��ȡ����
	size_t Read(void* buf, size_t size);

	/// ��ȡ�е�������
	bool ReadLine(char* buf, int size);

	/// ��ȡ�е��ַ���
	bool ReadLine(string& line);


	/// ��ȡ�е�������
	bool ReadLine(wchar_t* buf, int size);

	/// ��ȡ�е��ַ���
	bool ReadLine(wstring& line);


	/// д�뻺��������
	size_t Write(const void* data, size_t size);

	/// д���ַ���
	bool Write(const char* str);

	/// ���������ĸ�ʽ��д��
	int WriteV(const char* format, va_list argptr);

	/// ��ʽ����д��
	int WriteF(const char* format, ...);

	/// д��ṹ������
	template <typename StructT>
	bool WriteStruct(const StructT& buffer)
	{
		size_t size = Write(&buffer, sizeof(StructT));
		return size == sizeof(StructT);
	}

	/// ��ȡ�ṹ������
	template <typename StructT>
	bool ReadStruct(StructT& buffer)
	{
		size_t size = Read(&buffer, sizeof(StructT));
		return size == sizeof(StructT);
	}

	bool Seek(long offset, int origin = SEEK_SET)
	{
		return 0 == fseek(m_handle, offset, origin);
	}

	/// ˢ�»�����
	bool Flush();

	/// �Ƿ�ʧ��
	bool IsFailed();

private:
	/// �ļ����
	FILE* m_handle;
};


/// ������ļ��ı�׼C�ļ�������װ��
class StdFileReader : public StdFile
{
public:
	/// �Զ����Ʒ�ʽ��
	bool OpenBinary(const char* path, int shareFlag = _SH_DENYNO);

	/// ���ı���ʽ��
	bool OpenText(const char* path, int shareFlag = _SH_DENYNO);

#if defined(_PPL_PLATFORM_MSWIN)
	bool OpenBinary(const wchar_t* path, int shareFlag = _SH_DENYNO);
	bool OpenText(const wchar_t* path, int shareFlag = _SH_DENYNO);
#endif
};


/// ����д�ļ��ı�׼C�ļ�������װ��
class StdFileWriter : public StdFile
{
public:
	StdFileWriter() { }
	~StdFileWriter();

	void Close();

	/// �Զ����Ʒ�ʽ��
	bool OpenBinary(const char* path, int shareFlag = _SH_DENYWR);

	/// ���ı���ʽ��
	bool OpenText(const char* path, int shareFlag = _SH_DENYWR);

	/// д���ļ�
	static bool WriteBinary(const char* path, const void* data, size_t size);

#if defined(_PPL_PLATFORM_MSWIN)
	bool OpenBinary(const wchar_t* path, int shareFlag = _SH_DENYWR);
	bool OpenText(const wchar_t* path, int shareFlag = _SH_DENYWR);
	static bool WriteBinary(const wchar_t* path, const void* data, size_t size);
#endif

};






inline bool StdFile::Flush()
{
	if (!IsOpen())
	{
		return false;
	}
	return EOF != fflush(m_handle);
}

inline bool StdFile::IsFailed()
{
	assert(IsOpen());
	return ferror(m_handle) != 0;
}
inline int StdFile::WriteV(const char* format, va_list argptr)
{
	assert(format != NULL);
	assert(IsOpen());
	//if (!IsOpen())
	//{
	//	return 0;
	//}
	return vfprintf(m_handle, format, argptr);
}
inline bool StdFile::Write(const char* str)
{
	assert(str != NULL);
	assert(IsOpen());
	//if (!IsOpen())
	//{
	//	return false;
	//}
	return EOF != fputs(str, m_handle);
}
inline size_t StdFile::Write(const void* data, size_t size)
{
	assert(data != NULL && size > 0);
	assert(IsOpen());
	//if (!IsOpen())
	//{
	//	return 0;
	//}
	return fwrite(data, 1, size, m_handle);
}
inline size_t StdFile::Read(void* buf, size_t size)
{
	assert(buf != NULL && size > 0);
	assert(IsOpen());
	//if (!IsOpen())
	//{
	//	return 0;
	//}
	return fread(buf, 1, size, m_handle);
}
inline bool StdFile::IsEOF() const
{
	return 0 != feof(m_handle);
}

inline void StdFile::Close()
{
	if (IsOpen())
	{
		fclose(m_handle);
		m_handle = NULL;
	}
}

inline bool StdFile::Open(const char* path, const char* mode, int shareFlag)
{
	assert(!IsOpen());
	this->Close();
	m_handle = _fsopen(path, mode, shareFlag);
	if (IsOpen())
		return true;
	UTIL_DEBUG("fopen failed " << " with " << path << " for mode " << mode);
	return false;
}

inline bool StdFile::ReadLine(char* buf, int size)
{
	assert(buf != NULL && size > 0);
	assert(IsOpen());
	//if (!IsOpen())
	//	return false;
	if (fgets(buf, size, m_handle) != NULL)
		return true;
	if (IsFailed())
	{
		UTIL_DEBUG("fgets failed ");
	}
	return false;
}

inline bool StdFile::ReadLine(string& line)
{
	assert(IsOpen());
	//if (!IsOpen())
	//	return false;
	const size_t max_size = 1024 * 4;
	char str[max_size + 1] = { 0 };
	if (fgets(str, max_size, m_handle) == NULL)
	{
		if (IsFailed())
		{
			UTIL_DEBUG("fgets failed");
		}
		return false;
	}
	line = str;
	return true;
}

inline bool StdFile::ReadLine(wstring& line)
{
	assert(IsOpen());
	//if (!IsOpen())
	//	return false;
	const size_t max_size = 1024;
	wchar_t str[max_size + 1] = { 0 };
	if (fgetws(str, max_size, m_handle) == NULL)
	{
		if (IsFailed())
		{
			UTIL_DEBUG("fgets failed");
		}
		return false;
	}
	line = str;
	return true;
}

inline int StdFile::WriteF(const char* format, ...)
{
	assert(format != NULL);
	va_list(args);
	va_start(args, format);
	int count = WriteV(format, args);
	va_end(args);
	return count;
}


inline bool StdFile::ReadLine(wchar_t* buf, int size)
{
	assert(buf != NULL && size > 0);
	assert(IsOpen());
	//if (!IsOpen())
	//	return false;
	if (fgetws(buf, size, m_handle) != NULL)
		return true;
	if (IsFailed())
	{
		UTIL_DEBUG("fgets failed ");
	}
	return false;
}



inline bool StdFileReader::OpenBinary(const char* path, int shareFlag)
{
	return this->Open(path, "rb", shareFlag);
}

inline bool StdFileReader::OpenText(const char* path, int shareFlag)
{
	return this->Open(path, "r", shareFlag);
}








inline bool StdFileWriter::OpenBinary(const char* path, int shareFlag)
{
	return this->Open(path, "wb", shareFlag);
}

inline bool StdFileWriter::OpenText(const char* path, int shareFlag)
{
	return this->Open(path, "w", shareFlag);
}





inline StdFileWriter::~StdFileWriter()
{
	Close();
}

inline void StdFileWriter::Close()
{
	if (IsOpen())
	{
		Flush();
		StdFile::Close();
	}
}


inline bool StdFileWriter::WriteBinary(const char* path, const void* data, size_t size)
{
	StdFileWriter file;
	if (!file.OpenBinary(path, _SH_DENYRW))
		return false;
	if (file.Write(data, size) != size)
		return false;
	file.Flush();
	return true;
}




#if defined(_PPL_PLATFORM_MSWIN)
inline bool StdFile::Open(const wchar_t* path, const wchar_t* mode, int shareFlag)
{
	assert(!IsOpen());
	this->Close();
	m_handle = _wfsopen(path, mode, shareFlag);
	if (IsOpen())
		return true;
	UTIL_DEBUG("fopen failed " << " with " << path << " for mode " << mode);
	return false;
}


inline bool StdFileReader::OpenBinary(const wchar_t* path, int shareFlag)
{
	return this->Open(path, L"rb", shareFlag);
}

inline bool StdFileReader::OpenText(const wchar_t* path, int shareFlag)
{
	return this->Open(path, L"r", shareFlag);
}


inline bool StdFileWriter::OpenBinary(const wchar_t* path, int shareFlag)
{
	return this->Open(path, L"wb", shareFlag);
}

inline bool StdFileWriter::OpenText(const wchar_t* path, int shareFlag)
{
	return this->Open(path, L"w", shareFlag);
}

inline bool StdFileWriter::WriteBinary(const wchar_t* path, const void* data, size_t size)
{
	StdFileWriter file;
	if (!file.OpenBinary(path, _SH_DENYRW))
		return false;
	if (file.Write(data, size) != size)
		return false;
	file.Flush();
	return true;
}

#endif




