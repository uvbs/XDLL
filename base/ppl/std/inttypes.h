
#pragma once

#include <ppl/config.h>

/**
 * @file
 * @brief 基本的整数类型定义
 */




typedef unsigned char		UINT8;
typedef unsigned short		UINT16;



// UINT32和UINT64已经定义在basestd.h中
//typedef unsigned int		UINT32;
//typedef unsigned __int64	UINT64;



#if defined(_PPL_PLATFORM_MSWIN)

typedef signed char			INT8;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef __int64 longlong;
typedef unsigned __int64 ulonglong;

inline ulonglong make_uint64( DWORD high, DWORD low )
{
	ULARGE_INTEGER val;
	val.HighPart = high;
	val.LowPart = low;
	return val.QuadPart;
}

#elif defined(_PPL_PLATFORM_LINUX)
#include <time.h>
#include <iostream>
#include <string>

using namespace std;
typedef long long longlong;
typedef unsigned long long ulonglong;

typedef unsigned int UINT;
typedef unsigned int UINT32;

typedef short INT16;
typedef int INT32;
typedef longlong INT64;
typedef ulonglong UINT64;

typedef UINT8 BYTE;
typedef UINT16 WORD;
typedef unsigned long DWORD;


typedef longlong LONGLONG;
typedef ulonglong ULONGLONG;

typedef long LONG;
typedef unsigned long ULONG;

typedef short SHORT;
typedef unsigned short USHORT;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER *PULARGE_INTEGER;

typedef DWORD DWORD_PTR;



typedef void* HWND;

typedef void* HANDLE;


typedef int BOOL;
const int TRUE = 1;
const int FALSE = 0;



typedef LONG HRESULT;



#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)

#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                                ((HRESULT)0x00000001L)


#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))




#ifndef _SYSTEMTIME_
#define _SYSTEMTIME_
typedef struct _SYSTEMTIME
    {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
    } 	SYSTEMTIME;

typedef struct _SYSTEMTIME *PSYSTEMTIME;

typedef struct _SYSTEMTIME *LPSYSTEMTIME;

#endif // !_SYSTEMTIME



inline bool GetLocalTime(SYSTEMTIME* st)
{
	time_t t = time(NULL);
	tm* ptm = localtime(&t);
	if ( NULL == ptm )
		return false;
	st->wYear = ptm->tm_year;
	st->wMonth = ptm->tm_mon;
	st->wDayOfWeek = ptm->tm_wday;
	st->wDay = ptm->tm_mday;
	st->wHour = ptm->tm_hour;
	st->wMinute = ptm->tm_min;
	st->wSecond = ptm->tm_sec;
	st->wMilliseconds = 0;
	return true;
}



#define FAILED(hr)  ((HRESULT)(hr) < 0)

typedef int INT;


const size_t MAX_PATH = 256;



/*
#if defined(UNICODE)
#define _T(x) L##x
typedef wchar_t TCHAR;

#else
#define _T(x) x
typedef char TCHAR;
#endif

typedef const char* LPCSTR;
typedef char* LPSTR;

typedef const TCHAR* LPCTSTR;
typedef TCHAR* LPTSTR;


#define TEXT(x) _T(x)

#define _ttoi atoi
#define _tcscat strcat
#define _tcscpy strcpy

# define _strnicmp strncasecmp
#define _vsnprintf vsnprintf
#define _snprintf snprintf
*/



inline bool IsBadReadPtr(const void* p, size_t size)
{
	return NULL == p || 0 == size;
}

inline bool IsBadWritePtr(void* p, size_t size)
{
	return NULL == p || 0 == size;
}


inline int lstrlen(const char* s)
{
	return strlen(s);
}

typedef void* LPVOID;
typedef const void *				LPCVOID;

typedef void* HANDLE;

#define INVALID_HANDLE_VALUE		((HANDLE)(-1))

/*

typedef struct _COORD { 
  SHORT X; 
  SHORT Y; 
} COORD; 

inline BOOL SetConsoleCursorPosition(
  HANDLE hConsoleOutput,  // handle to screen buffer
  COORD dwCursorPosition  // new cursor coordinates
)
{
	return FALSE;
}

*/


#define STD_OUTPUT_HANDLE   ((DWORD)-11)

#define WM_USER                         0x0400



/*
inline BOOL CopyFile(
  LPCTSTR lpExistingFileName, // name of an existing file
  LPCTSTR lpNewFileName,      // name of new file
  BOOL bFailIfExists          // operation if file exists
)
{
	return false;
}

inline HANDLE GetStdHandle(
  DWORD nStdHandle   // input, output, or error device
)
{
	return INVALID_HANDLE_VALUE;
}
*/


inline void OutputDebugStringA(const char* sz)
{
	printf("%s", sz);
}
inline void OutputDebugStringW(const wchar_t* sz)
{
	printf("%ls", sz);
}

inline void OutputDebugString(const char* sz)
{
	OutputDebugStringA(sz);
}
inline void OutputDebugString(const wchar_t* sz)
{
	OutputDebugStringW(sz);
}

inline long InterlockedDecrement(long* val)
{
	--(*val);
	return *val;
}

inline long InterlockedIncrement(long* val)
{
	++(*val);
	return *val;
}


#endif




