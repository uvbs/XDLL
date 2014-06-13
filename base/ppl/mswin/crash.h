
#pragma once

/* 使用方法
	CrashHandler::Install();
	或者定义全局的CrashHandlerInstaller对象
	memset(0, 0, 100); // 触发异常
*/


#include <ppl/mswin/version.h>

#include <ppl/mswin/ipc.h>
#include <ppl/mswin/module.h>
#include <ppl/io/stdfile.h>
#include <ppl/diag/trace.h>
#include <imagehlp.h>
#include <share.h>
#include <ppl/mswin/process.h>

#include <ppl/mswin/atl/conv.h>


class DbghelpModule : private noncopyable
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


	DbghelpModule() : m_dbghelp(_T("dbghelp.dll"))
	{
		if (!m_dbghelp.IsValid())
		{
			::OutputDebugString(TEXT("Load dbghelp.dll failed\n"));
		}
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
			::OutputDebugString(_T("DbghelpModule::SymInitialize, No SymInitialize\n"));
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
		func = reinterpret_cast<FuncT>(m_dbghelp.GetExportItem(name));
		if (func == NULL)
		{
			Tracer::Trace(TEXT("CrashHandler: RetrieveFunc failed %hs\n"), name);
		}
	}

	static DbghelpModule& GetInstance()
	{
		static DbghelpModule module;
		return module;
	}

	LoadableModule m_dbghelp;
	FUNC_SymInitialize	m_SymInitialize;
	FUNC_SymCleanup		m_SymCleanup;
	FUNC_StackWalk		m_StackWalk;
	FUNC_SymFunctionTableAccess		m_SymFunctionTableAccess;
	FUNC_SymGetModuleBase			m_SymGetModuleBase;
	FUNC_SymGetSymFromAddr			m_SymGetSymFromAddr;
};

class SymbolEngine
{
	HANDLE	m_hProcess;
public:
	SymbolEngine() : m_hProcess(NULL) { }

	BOOL SymInitialize(HANDLE hProcess , LPSTR UserSearchPath , BOOL fInvadeProcess)
	{
		m_hProcess = hProcess;
		if (DbghelpModule::SymInitialize(hProcess, UserSearchPath, fInvadeProcess))
			return TRUE;
		TCHAR msg[513];
		DWORD errcode = ::GetLastError();
		_sntprintf(msg, 512, _T("SymbolEngine::SymInitialize FAILED %d(%#lx)\n"), errcode, errcode);
		::OutputDebugString(msg);
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


/// 禁用crash处理器
const UINT CRASH_HANDLER_DISABLE = 0x01;

/// 处理掉这次错误，也就是不再继续搜索下一个CrashHandler
const UINT CRASH_HANDLER_SUPPRESS = 0x02;

/// 不显示对话框
const UINT CRASH_HANDLER_QUIET = 0x04;


const UINT CRASH_HANDLER_VERY_QUIET = CRASH_HANDLER_QUIET | CRASH_HANDLER_SUPPRESS;



class CrashHandler
{
public:
	CrashHandler() : m_file(NULL)
	{
		m_ExeFilename = Module().GetFilePath();

		Module module;
		module.AttachAddress(&CrashHandler::UnhandledExceptionFilter);
		tstring dllPath = module.GetFileDirectory();

		tstring pdbPath = dllPath;
		pdbPath += TEXT(";.;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;");

		USES_CONVERSION;
		m_IsValid = m_Engine.SymInitialize(::GetCurrentProcess(), T2A(const_cast<TCHAR*>(dllPath.c_str())), TRUE);
		if (!m_IsValid)
			return;

		tstring logfile = paths::combine(dllPath, _T("crash.log"));
		m_file = _tfsopen(logfile.c_str(), _T("a+t"), _SH_DENYWR);

		SYSTEMTIME st;
		::GetLocalTime(&st);
		LogMsg("Crash: %s\n", ::GetCommandLineA());
		LogMsg("Time : ");
		LogTime( st );
		LogMsg( "\n" );

		LogProcesses();
		LogModules();
	}
	~CrashHandler()
	{
		LogMsg("Crash End.\n\n\n");
		if (m_file != NULL)
		{
			fflush(m_file);
			fclose(m_file);
			m_file = NULL;
		}
		m_Engine.SymCleanup();
	}

	void LogTime( const SYSTEMTIME& st )
	{
		LogMsg("%04d-%02d-%02d %02d:%02d:%02d.%03d", 
			st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond, 
			st.wMilliseconds);
	}

	static CrashHandler& GetInstance()
	{
		static CrashHandler handler;
		return handler;
	}

	SymbolEngine& GetSymbolEngine()
	{
		return m_Engine;
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

	static UINT& GetMode()
	{
		static UINT mode = CRASH_HANDLER_QUIET;
		return mode;
	}

	static void SetMode(UINT mode)
	{
		GetMode() = mode;
	}

protected:
	static LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* excep)
	{
		LONG result = EXCEPTION_CONTINUE_SEARCH;
		__try
		{
			::OutputDebugString(_T("CrashHandler::UnhandledExceptionFilter\n"));
			CrashHandler& handler = GetInstance();
			if (handler.m_IsValid)
			{
				result = handler.Handle(excep);
			}
			else
			{
				::OutputDebugString(_T("CrashHandler is not valid. So continue normal processing.\n"));
				result = EXCEPTION_CONTINUE_SEARCH;
			}
			::OutputDebugString(_T("CrashHandler::UnhandledExceptionFilter OK.\n"));
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			::OutputDebugString(_T("CrashHandler::UnhandledExceptionFilter error \n"));
		}
		return result;
	}

	static bool IsFlagSet(UINT flag)
	{
		assert(flag > 0);
		UINT mode = GetMode();
		return (mode & flag) != 0;
	}

	LONG Handle(EXCEPTION_POINTERS* excep)
	{
		::OutputDebugString(_T("CrashHandler::Handle.\n"));
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
		PrintRegisters(excep->ContextRecord);
		PrintCallStack(excep);

		LogMsg("\n\n");
		::OutputDebugString(_T("CrashHandler::Handle complete.\n"));

		typedef HANDLE (WINAPI * FUNC_OpenThread)(DWORD, BOOL, DWORD);
		FUNC_OpenThread funcOpenThread = reinterpret_cast<FUNC_OpenThread>(::GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "OpenThread"));
		if (funcOpenThread != NULL)
		{
			DWORD pid = ::GetCurrentProcessId();
			for (ThreadWalker walker; walker.HasMore(); walker.MoveNext())
			{
				const THREADENTRY32& item = walker.Current();
				// 先排除其他的进程和本线程
				if (item.th32ThreadID == ::GetCurrentThreadId() || item.th32OwnerProcessID != pid)
					continue;
				HANDLE hThread = funcOpenThread(THREAD_SUSPEND_RESUME, FALSE, item.th32ThreadID);
				if (hThread != NULL)
				{
					//::OutputDebugString("suspend thread\n");
					::SuspendThread(hThread);
					::CloseHandle(hThread);
				}
			}
		}

		UINT mode = GetMode();
		bool isQuiet = IsFlagSet(CRASH_HANDLER_QUIET);
		bool isSuppressed = IsFlagSet(CRASH_HANDLER_SUPPRESS);

		if (!isQuiet)
		{
			::MessageBox(
				NULL, 
				_T("Application has encountered a problem and needs to close. We are sorry for the inconvenience.\n")
				_T("Error information has been recorded in file 'crash.log'."), 
				m_ExeFilename.c_str(), 
				MB_OK | MB_ICONERROR | MB_TOPMOST
			);
		}
		else
		{
			Tracer::Output(_T("Application crashed, but crash handler did not display.\n"));
		}
		
		return isSuppressed ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
	}

//	void PrintAddress(ostream& os, const char* name, const ADDRESS& addr)
//	{
//		const void* p = reinterpret_cast<const void*>(addr.Offset);
//		os << name << "\tAddress=0x" << p << ", Segment=" << addr.Segment << ", Mode=" << addr.Mode << endl;
//	}

	void PrintRegisters(PCONTEXT context)
	{
		LogMsg("eip = 0x%p  ebp = 0x%p  esp = 0x%p  eflags = 0x%p cflags = 0x%p\n", 
			context->Eip, context->Ebp, context->Esp, context->EFlags, context->ContextFlags);
		LogMsg("eax = 0x%p  ebx = 0x%p  ecx = 0x%p  edx = 0x%p  esi = 0x%p  edi = 0x%p\n", 
			context->Eax, context->Ebx, context->Ecx, context->Edx, context->Esi, context->Edi);
		LogMsg(" cs = 0x%p   ss = 0x%p   ds = 0x%p   es = 0x%p   fs = 0x%p   gs = 0x%p\n", 
			context->SegCs, context->SegSs, context->SegDs, context->SegEs, context->SegFs, context->SegGs);
	}

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

		tstring filename;
		Module module;
		module.AttachAddress(reinterpret_cast<void*>(addr.Offset));
		if (module.IsValid())
		{
			filename = module.GetFileName();
		}
		LogMsg("0x%p 0x%p  0x%08x  %2d  %-16s  %s\n", 
			addr.Offset, module.GetHandle(), displacement, addr.Mode, filename.c_str(), symbol->Name);
	}
	void PrintAddressHeader()
	{
		// 目前不打印头部信息, 避免外界了解过多程序细节
		LogMsg("Address     Displacement  Mode    Module       Name\n");
	}

	void PrintCallStack(EXCEPTION_POINTERS* excep)
	{
		SymbolEngine& engine = GetSymbolEngine();

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
		for (int stackFrames = 0; stackFrames < 500; ++stackFrames)
		{
			// 获取下一个栈帧
			if(!m_Engine.StackWalk(dwMachineType,hProcess,GetCurrentThread(),&sf,pContext))
				break;
			PrintAddress("AddrPC", sf.AddrPC);
	//		PrintAddress("  AddrReturn ", sf.AddrReturn);
	//		PrintAddress("\tAddrFrame", sf.AddrFrame);
	//		PrintAddress("\tAddrStack", sf.AddrStack);
		}
	}

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
	void LogModule(HMODULE hModule, LPCTSTR filename, DWORD size)
	{
		if (hModule != NULL)
		{
			DWORD filesize = 0;
			FileFinder finder;
			if (finder.Find(filename))
			{
				filesize = finder.Current().nFileSizeLow;
			}
			char verstr[256] = { 0 };
			FileVersionInfo versionInfo;
			if (versionInfo.Retrieve(filename))
			{
				const VS_FIXEDFILEINFO& ver = versionInfo.GetFixedInfo();
				_snprintf(verstr, 255, "%d.%d.%d.%d", 
					HIWORD(ver.dwFileVersionMS), 
					LOWORD(ver.dwFileVersionMS), 
					HIWORD(ver.dwFileVersionLS), 
					LOWORD(ver.dwFileVersionLS));
			}
			LogMsg("0x%p %8d %8d   %-18s %s\n", hModule, size, filesize, verstr, filename);
		}
	}
	void LogModule(LPCTSTR filename)
	{
		LogModule(::GetModuleHandle(filename), filename, 0);
	}
	void LogModules()
	{
		LogMsg( "Module List:\n" );
		TCHAR path[MAX_PATH * 4 + 1] = { 0 };
		for (ModuleWalker walker; walker.HasMore(); walker.MoveNext())
		{
			const MODULEENTRY32& item = walker.Current();
			assert((void*)item.hModule == item.modBaseAddr);
			// item.szExePath中可能是短路径
			bool success = (::GetLongPathName(item.szExePath, path, MAX_PATH * 4) > 0);
			LogModule(item.hModule, success ? path : item.szExePath, item.modBaseSize);
		}
	}
	void LogProcesses()
	{
		LogMsg( "Process List:\n" );
		for ( ProcessWalker walker; walker.HasMore(); walker.MoveNext() )
		{
			const PROCESSENTRY32& item = walker.Current();
			LogProcess( item );
		}
	}
	void LogProcess( const PROCESSENTRY32& item )
	{
		ProcessObject pobj( item.th32ProcessID, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ );
		if ( false == pobj.IsValid() )
			return;
		tstring exePath = pobj.GetPath();
		const DWORD max_size = MAX_PATH * 4;
		wchar_t path[max_size + 1] = { 0 };
		if ( FALSE == GetProcessCmdLine( pobj.GetHandle(), path, max_size ) )
		{
			path[0] = '\0';
		}
		SYSTEMTIME createtionTime = { 0 };
		FILETIME ft, ft2,ft3,ft4 = { 0 };
		if ( pobj.GetTimes( &ft, &ft2, &ft3, &ft4) )
		{
			FileTimeToSystemTime( &ft, &createtionTime );
		}
		else
		{
			TRACE( "get process times failed %d %p %p\n", GetLastError(), pobj.GetHandle(), pobj.GetID() );
		}
		LogTime( createtionTime );
		LogMsg( "  %-6d %-6d %-3d \t%-50s   \t%ls\n", item.th32ProcessID, item.th32ParentProcessID, item.cntThreads, exePath.c_str(), path );
	}

private:
	BOOL			m_IsValid;
	SymbolEngine	m_Engine;
	tstring			m_ExeFilename;
	FILE*			m_file;

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





