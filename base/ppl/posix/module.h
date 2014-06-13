


#pragma once

/**
* @file
* @brief dll和模块相关的类
*/


#include <ppl/io/path.h>
#include <ppl/diag/trace.h>
#include <dlfcn.h>



/// 封装win32模块操作的工具类
class Module
{
public:
	typedef void* HMODULE;

	explicit Module(HMODULE hModule = NULL) : m_handle(hModule)
	{
	}
	~Module()
	{
	}

	/// 获得模块句柄
	HMODULE GetHandle() const { return m_handle; }

	/// 模块是否有效
	bool IsValid() const { return m_handle != NULL; }

	/// 获取dll模块的导出项
	void* GetExportItem(LPCSTR name)
	{
		if (!IsValid())
			return NULL;
		return ::dlsym(GetHandle(), name);
	}



protected:
	/// 模块句柄
	HMODULE m_handle;
};


/// 可装载的模块
class LoadableModule : public Module, private noncopyable
{
public:
	LoadableModule()
	{
	}

	explicit LoadableModule(LPCTSTR filename)
	{
		m_handle = ::dlopen(filename, RTLD_NOW);
	}

	~LoadableModule()
	{
		Close();
	}

	bool Load(LPCTSTR filename)
	{
		assert(!IsValid());
		Close();
		m_handle = ::dlopen(filename, RTLD_NOW);
		return IsValid();
	}

	/// 关闭(释放)模块
	void Close()
	{
		if (IsValid())
		{
			int err = ::dlclose(m_handle);
			m_handle = NULL;
			assert(0 == err);
		}
	}
};






