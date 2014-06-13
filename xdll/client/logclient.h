#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LOGCLIENT_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LOGCLIENT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LOGCLIENT_EXPORTS
#define LOGCLIENT_API __declspec(dllexport)
#else
#define LOGCLIENT_API __declspec(dllimport)
#endif

#include "logclient-error.h"
#include "logclient-source.h"

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

typedef struct _LogClient_LogItem
{
	PCWSTR pName;
	PCWSTR pValue;
} LogClient_LogItem;

typedef void (__stdcall *LogClient_SendLogAsyncCallback)(void* pCallbackContext, INT resultError, DWORD httpStatusCode, DWORD winApiLastError);

// in following APIs, pMetaItems and pItems are ended with a item that has NULL pName/pValue.
// pMetaItems are first one or two fields that decide remaining fields in pItems,
// that means, for pMetaItems with same values, the pItems always has same amount of fields, field names and field meaning definition
// for winclient, usually pMetaItems contains 'Action' and 'A', others are in pItems.

extern "C" LOGCLIENT_API INT __stdcall LogClient_SendLogSync(const char* pLogSource, const LogClient_LogItem* pMetaItems, const LogClient_LogItem* pItems,
																						OUT DWORD* pHttpStatusCode, OUT DWORD* pWinApiLastError);

extern "C" LOGCLIENT_API INT __stdcall LogClient_SendLogAsyncIgnoreReply(const char* pLogSource, const LogClient_LogItem* pMetaItems, const LogClient_LogItem* pItems);

extern "C" LOGCLIENT_API INT __stdcall LogClient_SendLogAsync(const char* pLogSource, const LogClient_LogItem* pMetaItems, const LogClient_LogItem* pItems,
																						LogClient_SendLogAsyncCallback pCallbackFunc, void* pCallbackContext, OUT DWORD* pWinApiLastError);

extern "C" LOGCLIENT_API void __stdcall LogClient_FreeString(PCWSTR s);

extern "C" LOGCLIENT_API PCWSTR __stdcall LogClient_GetLogUrl(const char* pLogSource, const LogClient_LogItem* pMetaItems,
																						const LogClient_LogItem* pItems, OUT DWORD* pUrlCharCount);

// this api is provided for test equality of log client module and existing DAC log sending code
extern "C" LOGCLIENT_API PCWSTR __stdcall LogClient_GetLogUrlParamsPart(const char* pLogSource, const LogClient_LogItem* pMetaItems, 
																											const LogClient_LogItem* pItems, OUT DWORD* pUrlCharCount);
