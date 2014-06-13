
#pragma once

#include <ppl/util/log.h>
#include <ppl/stl/string.h>

#include <boost/noncopyable.hpp>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")


#define ProcessBasicInformation 0

typedef struct
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct
{
    ULONG          AllocationSize;
    ULONG          ActualSize;
    ULONG          Flags;
    ULONG          Unknown1;
    UNICODE_STRING Unknown2;
    HANDLE         InputHandle;
    HANDLE         OutputHandle;
    HANDLE         ErrorHandle;
    UNICODE_STRING CurrentDirectory;
    HANDLE         CurrentDirectoryHandle;
    UNICODE_STRING SearchPaths;
    UNICODE_STRING ApplicationName;
    UNICODE_STRING CommandLine;
    PVOID          EnvironmentBlock;
    ULONG          Unknown[9];
    UNICODE_STRING Unknown3;
    UNICODE_STRING Unknown4;
    UNICODE_STRING Unknown5;
    UNICODE_STRING Unknown6;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;

typedef struct
{
    ULONG               AllocationSize;
    ULONG               Unknown1;
    HINSTANCE           ProcessHinstance;
    PVOID               ListDlls;
    PPROCESS_PARAMETERS ProcessParameters;
    ULONG               Unknown2;
    HANDLE              Heap;
} PEB, *PPEB;

typedef struct
{
    DWORD ExitStatus;
    PPEB  PebBaseAddress;
    DWORD AffinityMask;
    DWORD BasePriority;
    ULONG UniqueProcessId;
    ULONG InheritedFromUniqueProcessId;
}   PROCESS_BASIC_INFORMATION;


// ntdll!NtQueryInformationProcess (NT specific!)
//
// The function copies the process information of the
// specified type into a buffer
//
// NTSYSAPI
// NTSTATUS
// NTAPI
// NtQueryInformationProcess(
//    IN HANDLE ProcessHandle,              // handle to process
//    IN PROCESSINFOCLASS InformationClass, // information type
//    OUT PVOID ProcessInformation,         // pointer to buffer
//    IN ULONG ProcessInformationLength,    // buffer size in bytes
//    OUT PULONG ReturnLength OPTIONAL      // pointer to a 32-bit
//                                          // variable that receives
//                                          // the number of bytes
//                                          // written to the buffer
// );
typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);


//PROCNTQSIP NtQueryInformationProcess;

BOOL GetProcessCmdLine(HANDLE hProcess,LPWSTR wBuf,DWORD dwBufLen);


inline BOOL GetProcessCmdLine(HANDLE hProcess,LPWSTR wBuf,DWORD dwBufLen)
{
    LONG                      status;
    PROCESS_BASIC_INFORMATION pbi;
    PEB                       Peb;
    PROCESS_PARAMETERS        ProcParam;
    DWORD                     dwDummy;
    DWORD                     dwSize;
    LPVOID                    lpAddress;
    BOOL                      bRet = FALSE;

	static PROCNTQSIP funcNtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
                                            GetModuleHandle(_T("ntdll")),
                                            "NtQueryInformationProcess"
                                            );
    if (!funcNtQueryInformationProcess)
       return FALSE;

    // Get process handle
	assert( hProcess );

    // Retrieve information
    status = funcNtQueryInformationProcess( hProcess,
                                        ProcessBasicInformation,
                                        (PVOID)&pbi,
                                        sizeof(PROCESS_BASIC_INFORMATION),
                                        NULL
                                      );


    if (status)
       goto cleanup;

    if (!ReadProcessMemory( hProcess,
                            pbi.PebBaseAddress,
                            &Peb,
                            sizeof(PEB),
                            &dwDummy
                          )
       )
       goto cleanup;

    if (!ReadProcessMemory( hProcess,
                            Peb.ProcessParameters,
                            &ProcParam,
                            sizeof(PROCESS_PARAMETERS),
                            &dwDummy
                          )
       )
       goto cleanup;

    lpAddress = ProcParam.CommandLine.Buffer;
    dwSize = ProcParam.CommandLine.Length;

    if (dwBufLen<dwSize)
       goto cleanup;

    if (!ReadProcessMemory( hProcess,
                            lpAddress,
                            wBuf,
                            dwSize,
                            &dwDummy
                          )
       )
       goto cleanup;


    bRet = TRUE;

cleanup:
    return bRet;
}






class ProcessObject : private boost::noncopyable
{
public:
	ProcessObject() : m_handle(NULL), m_id(static_cast<DWORD>( -1 ))
	{
	}
	explicit ProcessObject(DWORD id, DWORD access = PROCESS_ALL_ACCESS) : m_handle(::GetCurrentProcess()), m_id(::GetCurrentProcessId())
	{
		Open(id, access);
	}
	~ProcessObject()
	{
		Close();
	}

	void Close()
	{
		if (IsValid())
		{
			::CloseHandle(m_handle);
			m_handle = NULL;
			m_id = static_cast<DWORD>( -1 );
		}
	}
	bool IsValid() const
	{
		return m_handle != NULL;
	}

	bool Open(DWORD id, DWORD access = PROCESS_ALL_ACCESS)
	{
		Close();
		m_handle = ::OpenProcess(access, FALSE, id);
		if (IsValid())
		{
			m_id = id;
			//TRACE( "open process %d 0x%p\n", m_id, m_handle );
			return true;
		}
		APP_ERROR("OpenProcess " << id << " failed, error code=" << ::GetLastError());
		return false;
	}

	DWORD GetExitCode() const
	{
		DWORD exitCode = 0;
		BOOL success = ::GetExitCodeProcess(m_handle, &exitCode);
		assert(success);
		return exitCode;
	}

	bool IsAlive() const
	{
		return GetExitCode() == STILL_ACTIVE;
	}

	bool GetTimes( FILETIME* creationTime, FILETIME* exitTime, FILETIME* kernelTime, FILETIME* userTime )
	{
		return FALSE != ::GetProcessTimes( m_handle, creationTime, exitTime, kernelTime, userTime );
	}

#if !defined(_WIN32_WCE)
	tstring GetPath() const
	{
		TCHAR path[MAX_PATH * 4 + 4] = { 0 };
		DWORD len = ::GetModuleFileNameEx(m_handle, NULL, path, MAX_PATH * 4);
		int errcode = ::GetLastError();
		//assert(len > 0);
		return tstring(path, len);
	}

	tstring GetBaseName() const
	{
		TCHAR path[MAX_PATH * 4 + 4] = { 0 };
		DWORD len = ::GetModuleBaseName(m_handle, NULL, path, MAX_PATH * 4);
		assert(len > 0);
		return tstring(path, len);
	}
#endif

	/// »ñÈ¡¾ä±ú
	HANDLE GetHandle() const { return m_handle; }
	DWORD GetID() const { return m_id; }

private:
	HANDLE	m_handle;
	DWORD	m_id;
};



class Process : private boost::noncopyable
{
public:
	static bool Create(STARTUPINFO& startupInfo, PROCESS_INFORMATION& processInfo, LPCTSTR appname, LPTSTR cmdline, DWORD flags)
	{
		BOOL res = ::CreateProcess(appname, cmdline, NULL, NULL, FALSE, flags, NULL, NULL, &startupInfo, &processInfo);
		return (FALSE != res);
	}
};

