// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0BDFA35D_A1F4_4DB0_91E2_D57606ED4C83__INCLUDED_)
#define AFX_STDAFX_H__0BDFA35D_A1F4_4DB0_91E2_D57606ED4C83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

#include <Aclapi.h>
#pragma comment(lib, "Advapi32")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")
#include <TlHelp32.h>
// TODO: reference additional headers your program requires here

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0500		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0500	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#include <atlbase.h>


#include <atlbase.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0BDFA35D_A1F4_4DB0_91E2_D57606ED4C83__INCLUDED_)
