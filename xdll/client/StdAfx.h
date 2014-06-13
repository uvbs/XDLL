// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__73EFD76E_3410_47E6_8234_AB1D6AA03F0B__INCLUDED_)
#define AFX_STDAFX_H__73EFD76E_3410_47E6_8234_AB1D6AA03F0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#	define NEED_LOG
#	define NEED_IMM
#endif


// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0500		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0500	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning( disable : 4503) 
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


// #ifndef _AFX_NO_DB_SUPPORT
// #include <afxdb.h>			// MFC ODBC database classes
// #endif // _AFX_NO_DB_SUPPORT
// 
// #ifndef _AFX_NO_DAO_SUPPORT
// #include <afxdao.h>			// MFC DAO database classes
// #endif // _AFX_NO_DAO_SUPPORT
// 
// #include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
// #ifndef _AFX_NO_AFXCMN_SUPPORT
// #include <afxcmn.h>			// MFC support for Windows Common Controls
// #endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions


#include <wininet.h>
//#include <winuser.h>
//#include <windows.h>

#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <mshtmlc.h>
#include <ShlObj.h>
#include <UrlMon.h>
#include "urlmon2.h"

#include <list>
#include <vector>
#include <map>

class TipsClient;
TipsClient* AfxGetTipsClient();
const UINT   WM_RUNTASK   = WM_USER + 0xCC;

MIDL_INTERFACE("fd2955f9-6e1d-44a9-9182-61dcdb6487c0")
CLSID_MyHttpProtocol;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__73EFD76E_3410_47E6_8234_AB1D6AA03F0B__INCLUDED_)
