// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E5BF3501_2E0B_4534_80F7_DECD3D9CDEFD__INCLUDED_)
#define AFX_STDAFX_H__E5BF3501_2E0B_4534_80F7_DECD3D9CDEFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <winsock2.H>
#pragma comment(lib, "ws2_32.lib")

// TODO: reference additional headers your program requires here

#pragma warning(disable: 4786)

#include <map>
#include <set>
#include <cassert>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
using namespace std;




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E5BF3501_2E0B_4534_80F7_DECD3D9CDEFD__INCLUDED_)
