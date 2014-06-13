// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_Wnd.cpp
//  AUTHOR:		Steve Schaneville
//  CREATED:	22 Jan 2002, 11:26
//
//  PURPOSE:	
//
//  Copyright (c) 2002
//
// ----------------------------------------------------------------------- //

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#include "stdafx.h"
#include "SS_Wnd.h"
//#include "resource.h"
#include <list>
using namespace std;

// ------------------[      Macros/Constants/Types     ]------------------ //

typedef struct SSWndHandleMapEntry
{
    HWND    hWnd;
    SS_Wnd* pWnd;
} SSWND_HANDLE_MAP_ENTRY;

typedef list<SSWndHandleMapEntry> HandleMap;
typedef list<SSWndHandleMapEntry>::iterator HandleMapIterator;

// ------------------[         Global Variables        ]------------------ //

// the instance handle
HINSTANCE g_hInstance = NULL;

// Static variable to keep track of whether or not we've already registered 
// the SS_Wnd class.
BOOL SS_Wnd::m_bClassIsRegistered = FALSE;

// Mapping table to help us send the callback messages to the proper class instance.
HandleMap g_HandleMap;

// ------------------[         Global Functions        ]------------------ //

// look up the pWnd based on the hWnd
SS_Wnd* FindWindowInstance(HWND hWnd)
{
    SS_Wnd* pWnd = NULL;
    HandleMapIterator it = g_HandleMap.begin();
    while( it != g_HandleMap.end() )
    {
        if( it->hWnd == hWnd )
            return it->pWnd;
        
        it++;
    }

    return NULL;
}

// The global callback message handling function for SS_Wnd.
LRESULT CALLBACK SSWndWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDCLASS wndClass;
    ::GetClassInfo(g_hInstance, SSWND_REGISTERED_NAME, &wndClass);
    if( !_tcscmp(wndClass.lpszClassName, SSWND_REGISTERED_NAME) )
    {
        SS_Wnd* pWnd = FindWindowInstance(hWnd);
        if( pWnd )
            return pWnd->WindowProc(message, wParam, lParam);        
    }

    return 1;
}

// The global callback timer handling function.
VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    SS_Wnd* pWnd = FindWindowInstance(hWnd);
    if( pWnd )
        pWnd->OnTimer(uMsg, idEvent, dwTime);
}

// ------------------[    Class Function Definitions   ]------------------ //

// Standard Constructor
SS_Wnd::SS_Wnd(HINSTANCE hInstance)
{
    InitObject();
    Instance(hInstance);
    RegisterClass();
}

// Standard Destructor
SS_Wnd::~SS_Wnd()
{
}

// Copy Constructor
SS_Wnd::SS_Wnd( SS_Wnd& rhs )
{
    InitObject();
    *this = rhs;
}

// Assignment Operator
SS_Wnd& SS_Wnd::operator = ( SS_Wnd& rhs )
{
    InitObject();

    return *this;
}

// Object Initialization
VOID SS_Wnd::InitObject()
{
    m_hInstance = NULL;
    m_hWnd = NULL;
}

VOID SS_Wnd::RegisterClass()
{
    if( !m_bClassIsRegistered )
    {
        WNDCLASSEX wce;
        
        wce.cbSize = sizeof(wce);
        wce.style = NULL;
        wce.lpfnWndProc = SSWndWindowProc;
        wce.cbClsExtra = 0;
        wce.cbWndExtra = 0;
        wce.hInstance = Instance();
        wce.hIcon = NULL;
        wce.hIconSm = NULL;
        wce.hCursor = NULL; // LoadCursor(Instance(), MAKEINTRESOURCE(IDC_POINTER));
        wce.hbrBackground = NULL;
        wce.lpszMenuName = NULL;
        wce.lpszClassName = SSWND_REGISTERED_NAME;
        
        ::RegisterClassEx(&wce);

        m_bClassIsRegistered = TRUE;
        g_hInstance = m_hInstance;
    }
}

VOID SS_Wnd::Create()
{
    // create the window
    HWND hWnd = CreateWindowEx( NULL,
                                SSWND_REGISTERED_NAME,
                                _T(""),
                                NULL,
                                0,
                                0,
                                0,
                                0,
                                NULL,
                                NULL,
                                Instance(),
                                NULL );

    // store the handle in the classes member function
    WindowHandle(hWnd);

    // save the handle into the handle map for callback messages
    SSWndHandleMapEntry hme;
    hme.hWnd = hWnd;
    hme.pWnd = this;
    g_HandleMap.push_front(hme);
}

LRESULT SS_Wnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch( message )
    {
        case WM_TIMER:
            OnTimer(message, (UINT_PTR)wParam, (DWORD)lParam);
    	break;

        default:
        {
        }
    }

    return 1;
}

VOID SS_Wnd::OnTimer(UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
}

UINT_PTR SS_Wnd::SetTimer(UINT nIDEvent, UINT uElapse, TIMERPROC lpTimerProc)
{
    if( NULL == lpTimerProc )
        lpTimerProc = TimerProc;

    return ::SetTimer(m_hWnd, nIDEvent, uElapse, lpTimerProc);
}

BOOL SS_Wnd::KillTimer(UINT nIDEvent)
{
    return ::KillTimer(m_hWnd, nIDEvent);
}
