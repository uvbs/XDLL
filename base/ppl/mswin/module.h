
#pragma once

/**
 * @file
 * @brief dll��ģ����ص���
 */


#include <ppl/io/path.h>
#include <ppl/diag/trace.h>
#include <boost/noncopyable.hpp>

#include <ppl/mswin/windows.h>


/// ��װwin32ģ������Ĺ�����
class Module
{
public:
	explicit Module(HMODULE hModule = NULL) : m_handle(hModule)
	{
	}
	~Module()
	{
	}

	/// ���ݵ�ַ����ѯ�˵�ַ������ģ����������Ϊ������̬�����ĵ�ַ
	static HMODULE QueryModuleHandle(PVOID address)
	{
		MEMORY_BASIC_INFORMATION meminfo = { 0 };
		const size_t sizeof_meminfo = sizeof(MEMORY_BASIC_INFORMATION);
		if (sizeof_meminfo != ::VirtualQuery(address, &meminfo, sizeof_meminfo))
		{
			TRACE(TEXT("VirtualQuery failed.\n"));
			return NULL;
		}
		return static_cast<HMODULE>(meminfo.AllocationBase);
	}

	/// ��������Ӧ�ĵ�ַ������ģ��
	bool AttachAddress(PVOID address)
	{
		m_handle = QueryModuleHandle(address);
		if (m_handle != NULL)
		{
			assert( false == GetFilePath().empty() );
		}
		return this->IsValid();
	}

	/// ���ģ����
	HMODULE GetHandle() const { return m_handle; }

	/// ģ���Ƿ���Ч
	bool IsValid() const { return m_handle != NULL; }
	bool IsOpen() const { return m_handle != NULL; }

	/// ��ȡdllģ��ĵ�����
	FARPROC GetExportItem(LPCSTR name)
	{
		if (!IsValid())
			return NULL;
		return ::GetProcAddress(m_handle, name);
	}


	tstring GetFilePath() const
	{
		const size_t max_path_size = 4095;
		TCHAR pathstr[max_path_size + 1];
		DWORD len = ::GetModuleFileName( m_handle, pathstr, max_path_size );
		if ( 0 == len || len > max_path_size )
		{
			TRACE(_T("GetModuleFileName failed %p %lu %d\n"), m_handle, len, ::GetLastError());
			return tstring();
		}
		assert( '\0' == pathstr[len] );
		return tstring(pathstr, len);
	}

	/// ��ȡģ����ļ���(������·��)
	tstring GetFileName()
	{
		return paths::get_filename(GetFilePath());
	}
	tstring GetFileDirectory()
	{
		return paths::get_directory(GetFilePath());
	}
	tstring GetFileBaseName()
	{
		return paths::get_basename(GetFilePath());
	}

	tstring BuildLocalFilePath(const tstring& filename)
	{
		return paths::replace_filename( GetFilePath(), filename );
	}
	tstring BuildLocalMainFilePath(const tstring& extname)
	{
		return paths::replace_extension( GetFilePath(), extname );
	}

/*
#ifndef _PPL_NO_OLD_PATH
	/// ��ȡģ����ļ���(��������·��)
	bool GetFilePath(Path& result)
	{
		TCHAR path[Path::MAX_PATH_SIZE + 1] = { 0 };
		DWORD len = ::GetModuleFileName( m_handle, path, Path::MAX_PATH_SIZE );
		if (0 != len)
		{
			result.Assign(path);
			return true;
		}
		TRACE(TEXT("GetModuleFileName failed %p %d\n"), m_handle, ::GetLastError());
		result.Clear();
		assert(!"GetModuleFileName failed");
		return false;
	}

	/// ��ȡģ���·��
	bool GetFileDirectory(Path& result)
	{
		return GetFilePath(result) && result.RemoveFileSpec();
	}


	/// �����ģ��ͬһĿ¼���ļ�������·��(��: v:\\test.exe, ��Ӧ�Ľ��Ϊv:\\filename)
	bool BuildLocalFilePath(Path& result, LPCTSTR filename)
	{
		return GetFileDirectory(result) && result.Append(filename);
	}

	/// �����ģ��ͬһĿ¼, ��ͬ�ļ���, ��ͬ��չ���������ļ�·��(��: v:\\test.exe, ��Ӧ�Ľ��Ϊv:\\test.ext)
	bool BuildLocalMainFilePath(Path& result, LPCTSTR extName)
	{
		return GetFilePath(result) && result.RenameExtension(extName);
	}
#endif
*/

protected:
	/// ģ����
	HMODULE m_handle;
};


/// ��װ�ص�ģ��
class LoadableModule : public Module, private boost::noncopyable
{
public:
	LoadableModule()
	{
	}

	explicit LoadableModule(LPCTSTR filename)
	{
		m_handle = ::LoadLibrary(filename);
	}

	~LoadableModule()
	{
		Close();
	}
	
	bool Load(LPCTSTR filename)
	{
		assert(!IsValid());
		Close();
		m_handle = ::LoadLibrary(filename);
		return IsValid();
	}

	/// �ر�(�ͷ�)ģ��
	void Close()
	{
		if (IsValid())
		{
			::FreeLibrary(m_handle);
			m_handle = NULL;
		}
	}
};






