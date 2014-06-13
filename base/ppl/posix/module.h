


#pragma once

/**
* @file
* @brief dll��ģ����ص���
*/


#include <ppl/io/path.h>
#include <ppl/diag/trace.h>
#include <dlfcn.h>



/// ��װwin32ģ������Ĺ�����
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

	/// ���ģ����
	HMODULE GetHandle() const { return m_handle; }

	/// ģ���Ƿ���Ч
	bool IsValid() const { return m_handle != NULL; }

	/// ��ȡdllģ��ĵ�����
	void* GetExportItem(LPCSTR name)
	{
		if (!IsValid())
			return NULL;
		return ::dlsym(GetHandle(), name);
	}



protected:
	/// ģ����
	HMODULE m_handle;
};


/// ��װ�ص�ģ��
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

	/// �ر�(�ͷ�)ģ��
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






