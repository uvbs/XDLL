
#pragma once


/**
 * @file
 * @brief 一个简单的ostream实现(可供windows ce等平台使用)
 */


namespace std {
	using ::type_info;
}



/*
inline RPC_STATUS WINAPI UuidFromStringA(unsigned char* str, UUID* uuid)
{
	assert(!"not implemented");
	return RPC_S_ACCESS_DENIED;
}

inline RPC_STATUS RPC_ENTRY UuidFromStringW(unsigned short* str, UUID* uuid)
{
	assert(!"not implemented");
	return RPC_S_ACCESS_DENIED;
}

inline RPC_STATUS RPC_ENTRY UuidCreate(UUID* uuid)
{
	srand(GetTickCount());
	uuid->Data1 = rand();
	uuid->Data2 = rand();
	uuid->Data3 = rand();
	for (int i = 0; i < 8; ++i)
	{
		uuid->Data4[i] = rand();
	}
	return E_FAIL;
}

inline HRESULT CoCreateGuid(GUID* uuid)
{
#pragma message("invalid random guid")
	return E_FAIL;
}
*/

inline DWORD WINAPI GetPrivateProfileString(LPCTSTR section, LPCTSTR key, LPCTSTR defaultValue, LPTSTR val, DWORD size, LPCTSTR filename)
{
	if (defaultValue == NULL)
		return false;

	_tcsncpy(val, defaultValue, size);
	return _tcslen(defaultValue);
}

inline BOOL WINAPI WritePrivateProfileString(LPCTSTR section, LPCTSTR key, LPCTSTR val, LPCTSTR filename)
{
	return FALSE;
}

inline BOOL WINAPI GetPrivateProfileStruct(LPCTSTR section, LPCTSTR key, LPVOID val, UINT size, LPCTSTR filename)
{
	return FALSE;
}

inline BOOL WINAPI WritePrivateProfileStruct(LPCTSTR section, LPCTSTR key, LPVOID val, UINT size, LPCTSTR filename)
{
	return FALSE;
}

inline BOOL WINAPI GetPrivateProfileInt(LPCTSTR section, LPCTSTR key, INT defaultValue, LPCTSTR filename)
{
	return FALSE;
}


inline DWORD WINAPI GetConsoleTitle(LPTSTR lpConsoleTitle, DWORD nSize)
{
	return 0;
}

inline BOOL WINAPI AllocConsole()
{
	return FALSE;
}

inline FILE * __cdecl freopen(const char *, const char *, FILE *)
{
	return NULL;
}

inline FILE * __cdecl _tfsopen(const TCHAR* filename, const TCHAR* mode, int shareFlags)
{
	return _tfopen(filename, mode);
}

inline BOOL WINAPI FreeConsole()
{
	return FALSE;
}


#if _WIN32_WCE < 500 && defined(__AFX_H__)
// There is no new.h for WinCE, so we define a placement new here.
inline void operator delete(void* ptr, void* p)
{
}
#endif
