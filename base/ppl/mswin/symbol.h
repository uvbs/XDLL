
#pragma once


#include "SymbolEngine.h"




class ImageLoader
{
public:
	ImageLoader() : m_image(NULL) { }
	explicit ImageLoader(LPCTSTR path)
	{
		Open(path);
	}
	~ImageLoader()
	{
		Close();
	}


	void Close()
	{
		if (IsValid())
		{
			::ImageUnload(m_image);
			m_image = NULL;
		}
	}

	void Open(LPCTSTR path)
	{
		m_image = ::ImageLoad(const_cast<LPTSTR>(path), NULL);
	}
	bool IsValid() const
	{
		return m_image != NULL;
	}
private:
	PLOADED_IMAGE	m_image;
};


class SymbolModuleListener
{
public:
	virtual ~SymbolModuleListener() { }
	virtual BOOL OnEnumModuleSymbol(PTSTR name, DWORD address, ULONG size) { return FALSE; }
};



class SymbolViewer
{
public:
	SymbolViewer()
	{
		Init();
	}
	~SymbolViewer()
	{
		m_engine.SymCleanup();
	}

	void Init()
	{
		DWORD options = SymGetOptions();
		SymSetOptions(SYMOPT_LOAD_LINES | SymGetOptions());
		options = SymGetOptions();
		m_engine.SymInitialize(0, NULL, FALSE);
	}

	DWORD LoadModule(HANDLE hFile, PSTR ImageName, PSTR ModuleName, DWORD BaseOfDll, DWORD SizeOfDll)
	{
		return m_engine.SymLoadModule(hFile, ImageName, ModuleName, BaseOfDll, SizeOfDll);
	}
	BOOL UnloadModule(DWORD BaseOfDll)
	{
		return m_engine.SymUnloadModule(BaseOfDll);
	}
	BOOL EnumSymbol(DWORD baseaddr, PSYM_ENUMSYMBOLS_CALLBACK func, PVOID context)
	{
		return m_engine.SymEnumerateSymbols(baseaddr, func, context);
	}
	BOOL GetSymbol(DWORD addr, DWORD& displacement, IMAGEHLP_SYMBOL& info)
	{
		info.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		return m_engine.SymGetSymFromAddr(addr, &displacement, &info);
	}
	BOOL GetLine(DWORD addr, DWORD& displacement, IMAGEHLP_LINE& info)
	{
		return m_engine.SymGetLineFromAddr(addr, &displacement, &info);
	}

private:
	CSymbolEngine	m_engine;
};


class SymbolModule
{
public:
	explicit SymbolModule(SymbolViewer& viewer) : m_viewer(viewer), m_base(0)
	{
		SetListener(NULL);
	}
	~SymbolModule()
	{
		Close();
	}

	void Open(HANDLE hFile, PSTR ImageName, PSTR ModuleName, DWORD BaseOfDll, DWORD SizeOfDll)
	{
		Close();
		m_base = m_viewer.LoadModule(hFile, ImageName, ModuleName, BaseOfDll, SizeOfDll);
	}
	void Close()
	{
		if (IsValid())
		{
			m_viewer.UnloadModule(m_base);
			m_base = 0;
		}
	}

	DWORD GetBaseAddress() const { return m_base; }
	bool IsValid() const
	{
		return GetBaseAddress() != 0;
	}

	BOOL EnumSymbol()
	{
		assert(IsValid());
		return m_viewer.EnumSymbol(GetBaseAddress(), EnumSymbolsProc, this);
	}
	void SetListener(SymbolModuleListener* listener)
	{
		m_listener = (listener != NULL) ? listener : NullListener();
	}

protected:
	BOOL OnEnumSymbol(PTSTR name, DWORD address, ULONG size)
	{
		return m_listener->OnEnumModuleSymbol(name, address, size);
	}
	static SymbolModuleListener* NullListener()
	{
		static SymbolModuleListener listener;
		return &listener;
	}

private:
	static BOOL CALLBACK EnumSymbolsProc(PTSTR name, DWORD address, ULONG size, PVOID UserContext)
	{
		SymbolModule* pThis = static_cast<SymbolModule*>(UserContext);
		return (pThis != NULL) ? pThis->OnEnumSymbol(name, address, size) : FALSE;
	}


private:
	SymbolModuleListener*	m_listener;
	SymbolViewer&			m_viewer;
	DWORD	m_base;
};


