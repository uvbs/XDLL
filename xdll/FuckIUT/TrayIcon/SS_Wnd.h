// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_Wnd.h
//  AUTHOR:		Steve Schaneville
//  CREATED:	22 Jan 2002, 11:26
//
//  PURPOSE:	
//
//  Copyright (c) 2002
//
// ----------------------------------------------------------------------- //
#ifndef __SS_Wnd_h__
#define __SS_Wnd_h__

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#include <windows.h>
#include <TCHAR.h>

// ------------------[          Macros/Defines         ]------------------ //
#define SSWND_REGISTERED_NAME (_T("SS_Wnd"))

// ------------------[      Constants/Enumerations     ]------------------ //
// ------------------[       Forward Declarations      ]------------------ //
// ------------------[         Global Variables        ]------------------ //
// ------------------[         Global Functions        ]------------------ //
// ------------------[             Classes             ]------------------ //

// ----------------------------------------------------------------------- //
//  Class:			SS_Wnd
//  Author:			Steve Schaneville
//  Notes:			
// ----------------------------------------------------------------------- //
class SS_Wnd
{
public:

//    DECLARE_DYNAMIC(SS_Wnd);
        
    // construction, destruction, assignment, copy, initialization
	SS_Wnd                                          (HINSTANCE hInstance);
	virtual ~SS_Wnd                                 ();

	SS_Wnd					    		            (SS_Wnd& rhs);
	SS_Wnd&			        operator =		        (SS_Wnd& rhs);

protected:

    virtual VOID	        InitObject		        ();

public:

	// accessor functions
    VOID                    Instance                (HINSTANCE hInstance);
    const HINSTANCE         Instance                () const;
    VOID                    WindowHandle            (HWND hWnd);
    const HWND              WindowHandle            () const;

    // utilities
    VOID                    Create                  ();
    virtual LRESULT         WindowProc              (UINT message, 
                                                     WPARAM wParam, 
                                                     LPARAM lParam);
    virtual VOID            OnTimer                 (UINT uMsg,
                                                     UINT_PTR idEvent,
                                                     DWORD dwTime);
    virtual UINT_PTR        SetTimer                (UINT nIDEvent, 
                                                     UINT uElapse,
                                                     TIMERPROC lpTimerProc=NULL);
    virtual BOOL            KillTimer               (UINT nIDEvent);
    
protected:


    
private:

    VOID                    RegisterClass           ();

    static BOOL     m_bClassIsRegistered;
    HINSTANCE       m_hInstance;
    HWND            m_hWnd;

};


// ----------------------------------------------------------------------- //
//  SS_Wnd Inline Functions
// ----------------------------------------------------------------------- //

inline VOID SS_Wnd::Instance(HINSTANCE hInstance)
{ m_hInstance = hInstance; }

inline const HINSTANCE SS_Wnd::Instance() const
{ return m_hInstance; }

inline VOID SS_Wnd::WindowHandle(HWND hWnd)
{ m_hWnd = hWnd; }

inline const HWND SS_Wnd::WindowHandle() const
{ return m_hWnd; }


#endif // __SS_Wnd_h__
