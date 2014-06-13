
#pragma once

/**
 * @file
 * @brief dll和模块相关的类
 */


#include <ppl/io/path.h>
#include <ppl/diag/trace.h>
#include <boost/noncopyable.hpp>

#include <ppl/mswin/windows.h>


/// 封装win32模块操作的工具类
class Module
{
public:
	explicit Module(HMODULE hModule = NULL) : m_handle(hModule)
	{
	}
	~Module()
	{
	}

	/// 根据地址来查询此地址所属的模块句柄，参数为函数或静态变量的地址
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

	/// 关联到对应的地址所属的模块
	bool AttachAddress(PVOID address)
	{
		m_handle = QueryModuleHandle(address);
		if (m_handle != NULL)
		{
			assert( false == GetFilePath().empty() );
		}
		return this->IsValid();
	}

	/// 获得模块句柄
	HMODULE GetHandle() const { return m_handle; }

	/// 模块是否有效
	bool IsValid() const { return m_handle != NULL; }
	bool IsOpen() const { return m_handle != NULL; }

	/// 获取dll模块的导出项
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

	/// 获取模块的文件名(不包含路径)
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
	/// 获取模块的文件名(包含完整路径)
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

	/// 获取模块的路径
	bool GetFileDirectory(Path& result)
	{
		return GetFilePath(result) && result.RemoveFileSpec();
	}


	/// 构造跟模块同一目录下文件的完整路径(如: v:\\test.exe, 对应的结果为v:\\filename)
	bool BuildLocalFilePath(Path& result, LPCTSTR filename)
	{
		return GetFileDirectory(result) && result.Append(filename);
	}

	/// 构造跟模块同一目录, 相同文件名, 不同扩展名的完整文件路径(如: v:\\test.exe, 对应的结果为v:\\test.ext)
	bool BuildLocalMainFilePath(Path& result, LPCTSTR extName)
	{
		return GetFilePath(result) && result.RenameExtension(extName);
	}
#endif
*/

protected:
	/// 模块句柄
	HMODULE m_handle;
};


/// 可装载的模块
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

	/// 关闭(释放)模块
	void Close()
	{
		if (IsValid())
		{
			::FreeLibrary(m_handle);
			m_handle = NULL;
		}
	}
};






