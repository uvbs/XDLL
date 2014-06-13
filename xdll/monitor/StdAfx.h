// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TODO: reference additional headers your program requires here

#pragma warning(disable:4786)
#pragma warning(disable:4996)

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// #include <afxwin.h>         // MFC core and standard components
// #include <afxext.h>         // MFC extensions

#include <windows.h>
#include <tchar.h>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <time.h>
#include <tlhelp32.h>
#include <process.h>
#include <Psapi.h.>
#pragma comment(lib, "psapi")


using namespace std;

#include "DefConst.h"
#include "GeneralUtils/StrUtils.h"
#include "GeneralUtils/MyLog.h"
#include "GeneralUtils/PathAndDisk.h"
#include "GeneralUtils/RegAndProfile.h"
#include "GeneralUtils/NetUtils.h"
#include "GeneralUtils/Win32Thread.h"
#include "GeneralUtils/CrashHandler.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
