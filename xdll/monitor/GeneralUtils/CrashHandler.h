#ifndef _CRASH_HANDLER_H_
#define _CRASH_HANDLER_H_


#pragma warning(disable:4996)
#pragma warning(disable:4311)
#pragma warning(disable:4312)


/* 使用方法
	CrashHandler::Install();
	或者定义全局的CrashHandlerInstaller对象
	memset(0, 0, 100); // 触发异常
*/
#include <cassert>
#include <string>
#include <time.h>
#include <stdio.h>
#include <imagehlp.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")


using namespace std;

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

class CModuleVersion : public VS_FIXEDFILEINFO {
protected:
	BYTE* m_pVersionInfo;   // all version info

	struct TRANSLATION {
		WORD langID;         // language ID
		WORD charset;        // character set (code page)
	} m_translation;

public:
	CModuleVersion()
	{
		m_pVersionInfo = NULL;
	}
	virtual ~CModuleVersion()
	{
		delete [] m_pVersionInfo;
	}
	BOOL GetFileVersionInfo(LPCSTR moduleFileName)
	{
		m_translation.charset = 1252;    // default = ANSI code page
		memset((VS_FIXEDFILEINFO*)this, 0, sizeof(VS_FIXEDFILEINFO));

		// read file version info
		DWORD dwDummyHandle; // will always be set to zero
		DWORD len = GetFileVersionInfoSizeA(const_cast<LPSTR>(moduleFileName), &dwDummyHandle);
		if (len <= 0)
			return FALSE;

		m_pVersionInfo = new BYTE[len]; // allocate version info
		if (!::GetFileVersionInfoA(const_cast<LPSTR>(moduleFileName), 0, len, m_pVersionInfo))
			return FALSE;

		LPVOID lpvi;
		UINT iLen;
		if (!VerQueryValueA(m_pVersionInfo, "\\", &lpvi, &iLen))
			return FALSE;

		// copy fixed info to myself, which am derived from VS_FIXEDFILEINFO
		*(VS_FIXEDFILEINFO*)this = *(VS_FIXEDFILEINFO*)lpvi;

		// Get translation info
		if (VerQueryValueA(m_pVersionInfo,
			"\\VarFileInfo\\Translation", &lpvi, &iLen) && iLen >= 4) {
				m_translation = *(TRANSLATION*)lpvi;
				//TRACE("code page = %d\n", m_translation.charset);
		}

		return dwSignature == VS_FFI_SIGNATURE;
	}
};



namespace CrashUtility {


	inline string TStrToStr(const tstring& _src)
	{
#ifdef _UNICODE
		int nBufSize = ::WideCharToMultiByte(GetACP(), 0, _src.c_str(),-1, NULL, 0, 0, FALSE);

		char *szBuf = new char[nBufSize + 1];

		::WideCharToMultiByte(GetACP(), 0, _src.c_str(),-1, szBuf, nBufSize, 0, FALSE);

		string strRet(szBuf);

		delete []szBuf;
		szBuf = NULL;

		return strRet;
#else
	return _src;
#endif
	}

/// win32路径的操作类
class Path
{
public:
	Path()
	{
		Clear();
	}

	enum { MAX_PATH_SIZE = MAX_PATH * 4 };
	enum { MAX_BUFFER_SIZE = MAX_PATH_SIZE + 4 };

	string GetFileName() const
	{
		return string(::PathFindFileNameA(m_path));
	}
	string GetFileExtension() const
	{
		return string(::PathFindExtensionA(m_path));
	}

	bool AddBackslash()
	{
		return NULL != ::PathAddBackslashA(GetBuffer());
	}
	bool RemoveBackslash()
	{
		return NULL != ::PathRemoveBackslashA(GetBuffer());
	}
	/// 去掉文件名，只保留目录的路径
	bool RemoveFileSpec()
	{
		return FALSE != ::PathRemoveFileSpecA(GetBuffer());
	}
	/// 修改文件的扩展名
	bool RenameExtension(LPCSTR ext)
	{
		return FALSE != ::PathRenameExtensionA(GetBuffer(), ext);
	}
	/// 将路径dir和文件名file合并起来
	bool Combine(LPCSTR dir, LPCSTR file)
	{
		return NULL != ::PathCombineA(GetBuffer(), dir, file);
	}
	/// 将文件名file添加到原有的路径后
	bool Append(LPCSTR file)
	{
		Path tempPath;
		tempPath.Assign(*this);
		return Combine(tempPath, file);
	}
	void Assign(LPCSTR path)
	{
		strncpy(m_path, path, MAX_PATH_SIZE);
		m_path[MAX_PATH_SIZE] = '\0';
	}
	void Clear()
	{
		m_path[0] = '\0';
	}

	operator LPCSTR() const
	{
		return GetData();
	}

	LPSTR GetBuffer()
	{
		return m_path;
	}
	LPCSTR GetData() const
	{
		return m_path;
	}

private:
	char	m_path[MAX_BUFFER_SIZE];
};


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

	HMODULE GetHandle() const
	{
		return m_handle;
	}
	bool IsValid() const
	{
		return GetHandle() != NULL;
	}
	FARPROC GetExportItem(LPCSTR name)
	{
		if (!IsValid())
			return NULL;
		return ::GetProcAddress(GetHandle(), name);
	}
	bool GetFileName(Path& result)
	{
		DWORD len = ::GetModuleFileNameA( m_handle, result.GetBuffer(), Path::MAX_PATH_SIZE );
		if (0 != len)
			return true;
		result.Clear();
		assert(!"GetModuleFileNameA failed");
		return false;
	}
	string GetBaseName()
	{
		string result;
		Path path;
		if (GetFileName(path))
		{
			result = path.GetFileName();
		}
		return result;
	}
	/// 获取跟进程exe文件的完整路径--不包含文件名(如: v:\\test.exe, 对应的结果为v:\\)
	bool GetFileDirectory(Path& result)
	{
		return GetFileName(result) && result.RemoveFileSpec();
	}
	/// 获取跟进程exe同一目录下文件的完整路径(如: v:\\test.exe, 对应的结果为v:\\filename)
	bool BuildLocalFileName(Path& result, LPCSTR filename)
	{
		return GetFileDirectory(result) && result.Append(filename);
	}
	/// 获取跟进程exe同一目录, 相同文件名, 不同扩展名的完整文件路径(如: v:\\test.exe, 对应的结果为v:\\test.ext)
	bool BuildLocalMainFileName(Path& result, LPCSTR extName)
	{
		return GetFileName(result) && result.RenameExtension(extName);
	}

protected:
	HMODULE m_handle;


	//////////////////////////////////////////////////////////////////////////
	// 一些静态方法(deprecated，仅为兼容老的代码而保留)
	//////////////////////////////////////////////////////////////////////////

public:
	/// 获取跟进程exe文件的完整路径(如: v:\\test.exe)
	static bool GetDefaultModuleFileName(Path& result)
	{
		return Module().GetFileName(result);
	}
	/// 获取跟进程exe文件的完整路径--不包含文件名(如: v:\\test.exe, 对应的结果为v:\\)
	static bool GetDefaultModuleFilePath(Path& result)
	{
		return Module().GetFileDirectory(result);
	}
	/// 获取跟进程exe同一目录下文件的完整路径(如: v:\\test.exe, 对应的结果为v:\\filename)
	static bool GetLocalFileName(Path& result, LPCSTR filename)
	{
		return Module().BuildLocalFileName(result, filename);
	}
	/// 获取跟进程exe同一目录, 相同文件名, 不同扩展名的完整文件路径(如: v:\\test.exe, 对应的结果为v:\\test.ext)
	static bool GetLocalMainFile(Path& result, LPCSTR extName)
	{
		return Module().BuildLocalMainFileName(result, extName);
	}
};


/// 可装载的模块
class LoadableModule : public Module
{
public:
	LoadableModule()
	{
	}
	explicit LoadableModule(LPCSTR filename)
	{
		m_handle = ::LoadLibraryA(filename);
	}
	~LoadableModule()
	{
		Close();
	}

	void Close()
	{
		if (IsValid())
		{
			::FreeLibrary(m_handle);
			m_handle = NULL;
		}
	}
};



// 注意：此类使用Module32First系列API实现，不支持WinNT4
class ModuleWalker
{
public:
	explicit ModuleWalker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
	{
		m_snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		memset(&m_item, 0, sizeof(MODULEENTRY32));
		m_item.dwSize = sizeof(MODULEENTRY32);
		if (!IsSnapShotValid())
			return;
		m_isValid = (FALSE != ::Module32First(m_snapShot, &m_item));
	}
	~ModuleWalker()
	{
		::CloseHandle(m_snapShot);
	}

	bool HasMore() const
	{
		return m_isValid;
	}
	void MoveNext()
	{
		assert(HasMore());
		m_isValid = (FALSE != ::Module32Next(m_snapShot, &m_item));
	}
	const MODULEENTRY32& Current() const
	{
		return m_item;
	}

protected:
	bool IsSnapShotValid() const
	{
		return m_snapShot != INVALID_HANDLE_VALUE;
	}

private:
	HANDLE	m_snapShot;
	MODULEENTRY32	m_item;
	bool	m_isValid;
};

class FileFinder
{
public:
	explicit FileFinder(LPCSTR filename = NULL) : m_handle(INVALID_HANDLE_VALUE), m_hasMore(false)
	{
		if (filename != NULL)
		{
			Find(filename);
		}
	}
	~FileFinder()
	{
		Close();
	}

	void Close()
	{
		if (IsValid())
		{
			::CloseHandle(m_handle);
			m_handle = INVALID_HANDLE_VALUE;
			m_hasMore = false;
		}
	}
	bool IsValid() const
	{
		return m_handle != INVALID_HANDLE_VALUE;
	}

	void Find(LPCSTR filename)
	{
		Close();
		m_handle = ::FindFirstFileA(filename, &m_item);
		if (!IsValid())
			return;
		m_hasMore = true;
	}

	bool HasMore() const
	{
		return m_hasMore;
	}
	void MoveNext()
	{
		assert(HasMore());
		m_hasMore = (FALSE != ::FindNextFileA(m_handle, &m_item));
	}
	const WIN32_FIND_DATAA& Current() const
	{
		return m_item;
	}

private:
	HANDLE	m_handle;
	WIN32_FIND_DATAA	m_item;
	bool	m_hasMore;
};


}

class DbghelpModule : public CrashUtility::LoadableModule
{
public:
	typedef BOOL (__stdcall *FUNC_SymInitialize)(HANDLE, LPSTR, BOOL);
	typedef BOOL (__stdcall *FUNC_SymCleanup)(HANDLE);
	typedef BOOL (__stdcall *FUNC_StackWalk)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID, 
		PREAD_PROCESS_MEMORY_ROUTINE, 
		PFUNCTION_TABLE_ACCESS_ROUTINE, 
		PGET_MODULE_BASE_ROUTINE, 
		PTRANSLATE_ADDRESS_ROUTINE);
	typedef LPVOID (__stdcall *FUNC_SymFunctionTableAccess)(HANDLE, DWORD);
	typedef DWORD (__stdcall *FUNC_SymGetModuleBase)(HANDLE, DWORD);
	typedef BOOL (__stdcall *FUNC_SymGetSymFromAddr)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);


	DbghelpModule() : CrashUtility::LoadableModule("dbghelp.dll")
	{
		RetrieveFunc("SymInitialize", m_SymInitialize);
		RetrieveFunc("SymCleanup", m_SymCleanup);
		RetrieveFunc("StackWalk", m_StackWalk);
		RetrieveFunc("SymFunctionTableAccess", m_SymFunctionTableAccess);
		RetrieveFunc("SymGetModuleBase", m_SymGetModuleBase);
		RetrieveFunc("SymGetSymFromAddr", m_SymGetSymFromAddr);
	}


	static BOOL __stdcall SymInitialize(HANDLE hProcess, LPSTR UserSearchPath, BOOL fInvadeProcess)
	{
		FUNC_SymInitialize func = GetInstance().m_SymInitialize;
		if (func == NULL)
		{
			::OutputDebugStringA("DbghelpModule::SymInitialize, No SymInitialize\n");
			return FALSE;
		}
		return (*func)(hProcess, UserSearchPath, fInvadeProcess);
	}
	static BOOL __stdcall SymCleanup(HANDLE hProcess)
	{
		FUNC_SymCleanup func = GetInstance().m_SymCleanup;
		if (func == NULL)
			return FALSE;
		return (*func)(hProcess);
	}
	static BOOL __stdcall StackWalk(
		DWORD MachineType, HANDLE hProcess, HANDLE hThread, 
		LPSTACKFRAME StackFrame, LPVOID ContextRecord, 
		PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine, 
		PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine, 
		PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, 
		PTRANSLATE_ADDRESS_ROUTINE TranslateAddress)
	{
		FUNC_StackWalk func = GetInstance().m_StackWalk;
		if (func == NULL)
			return FALSE;
		return (*func)(MachineType, hProcess, hThread, StackFrame, ContextRecord, 
			ReadMemoryRoutine, FunctionTableAccessRoutine, 
			GetModuleBaseRoutine, TranslateAddress);
	}
	static BOOL __stdcall SymGetSymFromAddr(HANDLE hProcess, DWORD dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_SYMBOL Symbol)
	{
		FUNC_SymGetSymFromAddr func = GetInstance().m_SymGetSymFromAddr;
		if (func == NULL)
			return FALSE;
		return (*func)(hProcess, dwAddr, pdwDisplacement, Symbol);
	}
	static LPVOID __stdcall SymFunctionTableAccess(HANDLE hProcess, DWORD dwAddr)
	{
		FUNC_SymFunctionTableAccess func = GetInstance().m_SymFunctionTableAccess;
		if (func == NULL)
			return FALSE;
		return (*func)(hProcess, dwAddr);
	}
	static DWORD __stdcall SymGetModuleBase(HANDLE hProcess, DWORD dwAddr)
	{
		FUNC_SymGetModuleBase func = GetInstance().m_SymGetModuleBase;
		if (func == NULL)
			return 0;
		return (*func)(hProcess, dwAddr);
	}

private:
	template <typename FuncT>
	void RetrieveFunc(LPCSTR name, FuncT& func)
	{
		func = reinterpret_cast<FuncT>(GetExportItem(name));
	}

	static DbghelpModule& GetInstance()
	{
		static DbghelpModule module;
		return module;
	}

	FUNC_SymInitialize	m_SymInitialize;
	FUNC_SymCleanup		m_SymCleanup;
	FUNC_StackWalk		m_StackWalk;
	FUNC_SymFunctionTableAccess		m_SymFunctionTableAccess;
	FUNC_SymGetModuleBase			m_SymGetModuleBase;
	FUNC_SymGetSymFromAddr			m_SymGetSymFromAddr;
};

class CSymbolEngine
{
	CrashUtility::LoadableModule	m_dbghelp;
	HANDLE	m_hProcess;
public:
	CSymbolEngine() : m_dbghelp("dbghelp.dll"), m_hProcess(NULL) { }

	BOOL SymInitialize(HANDLE hProcess , LPSTR UserSearchPath , BOOL fInvadeProcess)
	{
		m_hProcess = hProcess;
		if (DbghelpModule::SymInitialize(hProcess, UserSearchPath, fInvadeProcess))
			return TRUE;
		char msg[513];
		DWORD errcode = ::GetLastError();
		_snprintf(msg, 512, "CSymbolEngine::SymInitialize FAILED %d(%#lx)\n", errcode, errcode);
		::OutputDebugStringA(msg);
		return FALSE;
	}
	BOOL SymCleanup()
	{
		return DbghelpModule::SymCleanup(m_hProcess);
	}
	BOOL StackWalk(DWORD MachineType, HANDLE hProcess, HANDLE hThread, 
		LPSTACKFRAME StackFrame, LPVOID ContextRecord)
	{
		return DbghelpModule::StackWalk(MachineType, hProcess, hThread, StackFrame, ContextRecord, 
			0, 
			DbghelpModule::SymFunctionTableAccess, 
			DbghelpModule::SymGetModuleBase, 
			0);
	}
	BOOL SymGetSymFromAddr(DWORD dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_SYMBOL Symbol)
	{
		return DbghelpModule::SymGetSymFromAddr(m_hProcess, dwAddr, pdwDisplacement, Symbol);
	}
};


class CrashHandler
{
	BOOL			m_IsValid;
	CSymbolEngine	m_Engine;
	string			m_ExeFilename;
	FILE*			m_file;

	void LogMsg(const char* format, ...)
	{
		va_list arglist;
		va_start(arglist, format);
		if (m_file == NULL)
			return;
		vfprintf(m_file, format, arglist);
		fflush(m_file);
		// 暂不输出到stderr,可能会出错?
		//vfprintf(stderr, format, arglist);
	}
	void LogModule(HMODULE hModule, LPCSTR filename, DWORD size)
	{
		if (hModule != NULL)
		{
			DWORD filesize = 0;
			CrashUtility::FileFinder finder(filename);
			if (finder.HasMore())
			{
				filesize = finder.Current().nFileSizeLow;
			}
			char verstr[256] = { 0 };
			CModuleVersion ver;
			if (ver.GetFileVersionInfo(filename))
			{
				_snprintf(verstr, 255, "%d.%d.%d.%d", 
					HIWORD(ver.dwFileVersionMS), 
					LOWORD(ver.dwFileVersionMS), 
					HIWORD(ver.dwFileVersionLS), 
					LOWORD(ver.dwFileVersionLS));
			}
			LogMsg("Module 0x%p %8d %8d   %-18s %s\n", hModule, size, filesize, verstr, filename);
		}
	}
	void LogModule(LPCSTR filename)
	{
		LogModule(::GetModuleHandleA(filename), filename, 0);
	}
	void LogModules()
	{
		char path[MAX_PATH * 4 + 1] = { 0 };
		for (CrashUtility::ModuleWalker walker; walker.HasMore(); walker.MoveNext())
		{
			const MODULEENTRY32& item = walker.Current();
			assert((void*)item.hModule == item.modBaseAddr);
			// item.szExePath中可能是短路径T2A((LPTSTR)item.szExePath)
			bool success = (::GetLongPathNameA(CrashUtility::TStrToStr(item.szExePath).c_str(), path, MAX_PATH * 4) > 0);
			LogModule(item.hModule, success ? path : CrashUtility::TStrToStr(item.szExePath).c_str(), item.modBaseSize);
		}
	}

public:
	CrashHandler() : m_file(NULL)
	{

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		
		//CreateProcess(
		//	NULL,					// name of executable module
		//	_T("./CrashReport.exe n"),			// command line string
		//	NULL,					// process attributes
		//	NULL,					// thread attributes
		//	FALSE,					// handle inheritance option
		//	0,						// creation flags
		//	NULL,					// new environment block
		//	NULL,					// current directory name
		//	&si,					// startup information
		//	&pi);

		//MessageBoxA(NULL, "A crash happened!", "sorry", MB_OK | MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);

		const DWORD max_path_length = MAX_PATH * 4;
		char path[max_path_length + 1] = { 0 };
		::GetModuleFileNameA(NULL, path, max_path_length);
		m_ExeFilename = path;
		HMODULE thisModule = GetModuleHandleFromAddress(reinterpret_cast<DWORD>(&CrashHandler::UnhandledExceptionFilter));
		::GetModuleFileNameA(thisModule, path, max_path_length);
		::PathRemoveFileSpecA(path);

		char pdbPath[max_path_length + 1] = { 0 };
		lstrcpyA(pdbPath, path);
		lstrcatA(pdbPath, ";.;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;");
		m_IsValid = m_Engine.SymInitialize(::GetCurrentProcess(), pdbPath, TRUE);
		if (!m_IsValid)
			return;

		char logfile[max_path_length + 1] = { 0 };
		lstrcpyA(logfile, path);
		if (!::PathAppendA(logfile, "crash.log"))
		{
			::OutputDebugStringA("get log file name failed.\n");
			lstrcpyA(logfile, "crash.log");
		}
		m_file = fopen(logfile, "a+t");

		SYSTEMTIME st;
		::GetLocalTime(&st);
		LogMsg("Crash: %s\n", m_ExeFilename.c_str());
		LogMsg("Time : %04d-%02d-%02d %02d:%02d:%02d.%03d\n", 
			st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond, 
			st.wMilliseconds);

		LogModules();
	}
	~CrashHandler()
	{
		LogMsg("Crash End.\n\n\n\n");
		if (m_file != NULL)
		{
			fflush(m_file);
			fclose(m_file);
			m_file = NULL;
		}
		m_Engine.SymCleanup();
	}

	CSymbolEngine& GetSymbolEngine()
	{
		return m_Engine;
	}
	

	static BOOL& IsEnabled()
	{
		static BOOL s_enabled = TRUE;
		return s_enabled;
	}
	static void Disable()
	{
		IsEnabled() = FALSE;
	}

	static LPTOP_LEVEL_EXCEPTION_FILTER Install()
	{
		static bool isNotInstalled = true;
		LPTOP_LEVEL_EXCEPTION_FILTER oldHandler = NULL;
		if (isNotInstalled)
		{
			oldHandler = ::SetUnhandledExceptionFilter(UnhandledExceptionFilter);
			isNotInstalled = false;
		}
		return oldHandler;
	}
	static void Uninstall(LPTOP_LEVEL_EXCEPTION_FILTER oldHandler)
	{
		::SetUnhandledExceptionFilter(oldHandler);
	}

protected:
	static LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* excep)
	{
		
		LONG result = EXCEPTION_CONTINUE_SEARCH;
		__try
		{
			::OutputDebugStringA("CrashHandler::UnhandledExceptionFilter\n");
			CrashHandler& handler = GetInstance();
			if (handler.m_IsValid)
			{
				result = handler.Handle(excep);
			}
			else
			{
				::OutputDebugStringA("CrashHandler is not valid. So continue normal processing.");
				result = EXCEPTION_CONTINUE_SEARCH;
			}
		}
		__finally
		{
		}
	//	return result;
		return EXCEPTION_EXECUTE_HANDLER;
	}

	LONG Handle(EXCEPTION_POINTERS* excep)
	{
		::OutputDebugStringA("CrashHandler::Handle.\n");
		const EXCEPTION_RECORD* info = excep->ExceptionRecord;
		UINT_PTR args[2] = { 0, 0 };
		if (info->NumberParameters > 0)
		{
			args[0] = info->ExceptionInformation[0];
			if (info->NumberParameters > 1)
				args[1] = info->ExceptionInformation[1];
		}
		LogMsg("ExceptionInfo: Code = 0x%p, Flags = 0x%p, Address = 0x%p, Args =[ %d, 0x%p ]\n", 
			info->ExceptionCode, info->ExceptionFlags, info->ExceptionAddress, args[0], args[1]);
		PrintCallStack(excep);

		LogMsg("\n");
		::OutputDebugStringA("CrashHandler::Handle complete.\n");
/*
		char appname[MAX_PATH + 1] = { 0 };
		GetModuleBaseFileName(NULL, NULL, )
		char msg[4097] = { 0 };
		wsprintf(
			msg, 
			"%s has encountered a problem and needs to close. We are sorry for the inconvenience.\n"
			"Error information has been recorded in file 'crash.log'.", 
			);
*/
		if (IsEnabled())
		{
			/*
			::MessageBoxA(
				NULL, 
				"Application has encountered a problem and needs to close. We are sorry for the inconvenience.\n"
				"Error information has been recorded in file 'crash.log'.", 
				m_ExeFilename.c_str(), 
				MB_OK | MB_ICONERROR | MB_TOPMOST
			);
			*/
		}
		else
		{
			::OutputDebugStringA("Application crashed, but crash handler did not display.\n");
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}

	static CrashHandler& GetInstance()
	{
		static CrashHandler handler;
		return handler;
	}

//	void PrintAddress(ostream& os, const char* name, const ADDRESS& addr)
//	{
//		const void* p = reinterpret_cast<const void*>(addr.Offset);
//		os << name << "\tAddress=0x" << p << ", Segment=" << addr.Segment << ", Mode=" << addr.Mode << endl;
//	}

	void PrintAddress(const char* name, const ADDRESS& addr)
	{
		const DWORD max_size = 1024;
		char buf[max_size + 1] = { 0 };	//buffer used by IMAGEHLP_SYMBOL
		IMAGEHLP_SYMBOL* symbol = reinterpret_cast<IMAGEHLP_SYMBOL*>(buf);
		symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		symbol->MaxNameLength = max_size - sizeof(IMAGEHLP_SYMBOL);
		symbol->Address = addr.Offset;
		DWORD displacement = 0;
		const char* symName = "[null]";
		if (m_Engine.SymGetSymFromAddr(addr.Offset, &displacement, symbol))
			symName = symbol->Name;

		char moduleFilePath[max_size + 1] = "";
		LPCSTR moduleName = moduleFilePath;
		DWORD moduleStart = GetModuleFromAddress(addr.Offset, moduleFilePath, max_size);
		if (moduleStart > 0)
		{
			moduleName = ::PathFindFileNameA(moduleFilePath);
		}
		LogMsg("0x%p 0x%p  0x%08x  %2d  %-16s  %s\n", 
			addr.Offset, moduleStart, displacement, addr.Mode, moduleName, symbol->Name);
	}
	void PrintAddressHeader()
	{
		LogMsg("Address     Displacement  Mode    Module       Name\n");
	}

	void PrintCallStack(EXCEPTION_POINTERS* excep)
	{
		CSymbolEngine& engine = GetSymbolEngine();

		CONTEXT* pContext = excep->ContextRecord;
//		m_Log << "Crashed at 0x" << excep->ExceptionRecord->ExceptionAddress 
//			<< ", ExceptionCode=0x" << hex << excep->ExceptionRecord->ExceptionCode << endl;
		HANDLE hProcess = ::GetCurrentProcess();
		STACKFRAME sf;
		memset(&sf,0,sizeof(sf));

		// 初始化stackframe结构
		sf.AddrPC.Offset = pContext->Eip;
		sf.AddrPC.Mode = AddrModeFlat;
		sf.AddrStack.Offset = pContext->Esp;
		sf.AddrStack.Mode = AddrModeFlat;
		sf.AddrFrame.Offset = pContext->Ebp;
		sf.AddrFrame.Mode = AddrModeFlat;
		DWORD dwMachineType = IMAGE_FILE_MACHINE_I386;
		int i = 0;
		int nCount = 0;
		while(1)
		{
			if (nCount > 100)
			{
				break;
			}
			// 获取下一个栈帧
			if(!m_Engine.StackWalk(dwMachineType,hProcess,GetCurrentThread(),&sf,pContext))
				break;
			PrintAddress("AddrPC", sf.AddrPC);
	//		PrintAddress("  AddrReturn ", sf.AddrReturn);
	//		PrintAddress("\tAddrFrame", sf.AddrFrame);
	//		PrintAddress("\tAddrStack", sf.AddrStack);
			nCount++;
		}
	}

public:
	static HMODULE GetModuleHandleFromAddress(DWORD address)
	{
		return GetModuleHandleFromAddress(reinterpret_cast<LPVOID>(address));
	}
	static HMODULE GetModuleHandleFromAddress(PVOID address)
	{
		MEMORY_BASIC_INFORMATION meminfo = { 0 };
		const size_t sizeof_meminfo = sizeof(MEMORY_BASIC_INFORMATION);
		if (sizeof_meminfo != ::VirtualQuery(address, &meminfo, sizeof_meminfo))
		{
			::OutputDebugStringA("VirtualQuery failed.\n");
			return NULL;
		}
		return static_cast<HMODULE>(meminfo.AllocationBase);
	}

	static DWORD GetModuleFromAddress(DWORD address, char* fileName, DWORD size)
	{
		HMODULE hModule = GetModuleHandleFromAddress(address);
		if (hModule == NULL)
			return 0;
		char filePath[MAX_PATH * 4 * 1] = { 0 };
		DWORD len = ::GetModuleFileNameA(hModule, filePath, MAX_PATH * 4);
		if (len > 0)
		{
			bool success = (::GetLongPathNameA(filePath, fileName, size) > 0);
			if (!success)
			{
				strncpy(fileName, filePath, size - 1);
			}
		}
		return reinterpret_cast<DWORD>(hModule);
	}
};



/// crash handler的自动安装器
class CrashHandlerInstaller
{
public:
	CrashHandlerInstaller()
	{
		m_oldHandler = CrashHandler::Install();
	}
	~CrashHandlerInstaller()
	{
		CrashHandler::Uninstall(m_oldHandler);
	}

private:
	/// 原来的处理器
	LPTOP_LEVEL_EXCEPTION_FILTER m_oldHandler;
};




#endif      // _CRASHHANDLER_H


