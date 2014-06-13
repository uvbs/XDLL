// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_TrayIcon.h
//  AUTHOR:		Steve Schaneville
//  CREATED:	22 Jan 2002, 09:23
//
//  PURPOSE:	Easily create an animated icon in the system tray.
//
//  Copyright (c) 2002
//
// ----------------------------------------------------------------------- //
#ifndef __SS_TrayIcon_h__
#define __SS_TrayIcon_h__

// ------------------[       Pre-Include Defines       ]------------------ //

// uncomment the following line (_WIN32_IE) if you are compiling for Win2000
// or later, as it allows for larger tool tips.  I couldn't figure out how to 
// do this at runtime because the system includes use this value too.

#ifndef _WIN32_IE
#define _WIN32_IE 0x0501
#endif

// ------------------[          Include Files          ]------------------ //
#include <windows.h>
#include <assert.h>
#include "SS_Wnd.h"

#if !defined NUMELMS
  #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#endif

#ifndef _LIB
#if defined(UNICODE)
//#pragma comment(lib,"SS_TrayIconU.lib")
#else
//#pragma comment(lib,"SS_TrayIcon.lib")
#endif
#endif
// ------------------[          Macros/Defines         ]------------------ //
#ifndef ASSERT
#define ASSERT assert
#endif

// the blink rate for icons.  This is used if the user does not call
// SS_TrayIcon::SetAnimateSpeed(...)
#define SSTI_DEFAULT_BLINK_RATE 500

// define a function pointer for windows messages.
typedef LRESULT (CALLBACK *SS_FnMessageMap)(WPARAM wParam, LPARAM lParam);

// ------------------[      Constants/Enumerations     ]------------------ //

// The tray icon's windows message ID
#define SSTI_CALLBACK_MESSAGE (WM_USER+3678) // Just picked an odd number so 
                                              // it's less likely to clash 
                                              // with someone else's

// various named timers
typedef enum SsTiTimer
{
    SSTI_TIMER_DBLCLICK,
    SSTI_TIMER_ANIMATE,
    SSTI_TIMER_COUNT        // this *must* be the last item
    
} SSTI_TIMER, *PSSTI_TIMER;

// The display mode options.
typedef enum SsTiMode
{
    SSTI_MODE_DEFAULT,
    SSTI_MODE_SHOWNORMAL,
    SSTI_MODE_HIDE,
    SSTI_MODE_BLINK,
    SSTI_MODE_ANIMATE,
    SSTI_MODE_COUNT      // this *must* be the last item

} SSTI_MODE, *PSSTI_MODE;

// An icon and its associated information.
typedef struct SSTiIcon
{
    SSTiIcon()
    {
        hIcon = NULL;
        nTime = 1000;
    }

    HICON       hIcon;
    INT         nTime; // display length in milliseconds

} SSTI_ICON, *PSSTI_ICON;

// A list of icons.  The list can be used for creating a small animation in 
// the system tray
typedef struct SSTiIconSet
{
    SSTiIconSet() 
    {
        pIcon = NULL;
        nMode = SSTI_MODE_SHOWNORMAL;
        nCurrentIcon = 0;
        nLastIcon = 0;
        ZeroMemory(szToolTip, sizeof(szToolTip));
    }

    SSTiIcon*   pIcon;
    SSTI_MODE   nMode;
    INT         nCurrentIcon;
    INT         nLastIcon;
#if(_WIN32_IE>=0x0500) // starting with Win2000, we have larger tooltips.
    TCHAR       szToolTip[128];       
#else
    TCHAR       szToolTip[64];       
#endif
    
} SSTI_ICONSET, *PSSTI_ICONSET;

// ------------------[       Forward Declarations      ]------------------ //
class SS_TrayIcon;

// ------------------[         Global Variables        ]------------------ //
// ------------------[         Global Functions        ]------------------ //
// ------------------[             Classes             ]------------------ //

// ----------------------------------------------------------------------- //
//  Class:			SS_TrayIconWnd
//  Author:			Steve Schaneville
//  Notes:			
// ----------------------------------------------------------------------- //
class SS_TrayIconWnd : public SS_Wnd
{
public:

    // construction, destruction, assignment, copy, initialization
	SS_TrayIconWnd(HINSTANCE hInstance);
	virtual ~SS_TrayIconWnd();

protected:

public:

	// accessor functions
    SS_TrayIcon*            TrayIcon                ();
    VOID                    TrayIcon                (SS_TrayIcon* pTI);

    // utilities
    virtual LRESULT         WindowProc              (UINT message, 
                                                     WPARAM wParam, 
                                                     LPARAM lParam);
    virtual VOID            OnTimer                 (UINT uMsg,
                                                     UINT_PTR idEvent,
                                                     DWORD dwTime);
    

protected:

private:

    SS_TrayIcon*    m_pTrayIcon;

};


// ----------------------------------------------------------------------- //
//  SS_TrayIconWnd Inline Functions
// ----------------------------------------------------------------------- //

inline SS_TrayIcon* SS_TrayIconWnd::TrayIcon()
{ return m_pTrayIcon; }

inline VOID SS_TrayIconWnd::TrayIcon(SS_TrayIcon* pTI)
{ m_pTrayIcon = pTI; }


// ----------------------------------------------------------------------- //
//  Class:			SS_TrayIcon
//  Author:			Steve Schaneville
//  Notes:			
// ----------------------------------------------------------------------- //
class SS_TrayIcon
{
public:

    // construction, destruction
	SS_TrayIcon                                         (HINSTANCE hInstance,
                                                         INT nMaxIconSets=1, 
                                                         INT nMaxIconsPerSet=1,
                                                         HWND hWnd=NULL);
    virtual ~SS_TrayIcon                                ();

protected:

    // initialization
    virtual VOID	            InitObject		        ();

public:

    // accessor functions
    const BOOL                  CurrentlyVisible        () const;
    VOID                        CurrentlyVisible        (BOOL bCurrentlyVisible);
    const INT                   CurrentSet              () const;
    VOID                        CurrentSet              (INT nCurrentSet);
    
    // functions to change the icon behavior
    const SsTiMode              Mode                    (INT nIconSet) const;
    VOID                        Mode                    (INT nIconSet,
                                                         SsTiMode nMode);
    
    HICON                       LoadIcon                (INT nIconSet,
                                                         INT nIndex,
                                                         UINT nResourceID,
                                                         INT nDisplayTime=SSTI_DEFAULT_BLINK_RATE);

    VOID                        SetAnimateSpeed         (INT nIconSet,
                                                         INT nMilliseconds);

    VOID                        ToolTip                 (INT nIconSet, TCHAR* szToolTip);
    const TCHAR*                ToolTip                 (INT nIconSet) const;
    
	VOID                        ShowBalloon				(LPCTSTR lpszInfo,
														 LPCTSTR lpszInfoTitle = NULL,  
														 UINT	 nTimeOut = 3000
														);
	VOID                        HideBalloon             ();
    VOID                        ShowMenuAtMouse         (UINT nResourceID, HWND hWnd) const;
    
    // show or hide the icon
    VOID                        HideIcon                ();
    INT                         ShowIcon                (INT nIconSet=-1,
                                                         SsTiMode nMode=SSTI_MODE_DEFAULT);
    
    // allows user to map TrayIcon windows messages to user-defined functions
    VOID                        MapMessageToFunction    (UINT message, 
                                                         SS_FnMessageMap pFn);

    // in case the use handles the tray icon's windows messages himself
    VOID                        OnTimer                 (UINT idEvent);

protected:

    // helper utilities
    HICON                       CreateBlankIcon         ();

    // accessor functions
    const SSTI_ICONSET          IconSet                 (INT nSetID) const;
    PSSTI_ICONSET               IconSetNC               ();
    const INT                   MaxIconSets             () const;
    VOID                        MaxIconSets             (INT nMaxIconSets);
    const INT                   MaxIconsPerSet          () const;
    VOID                        MaxIconsPerSet          (INT nMaxIconsPerSet);
    const BOOL                  UsingSSWnd              () const;
    VOID                        UsingSSWnd              (BOOL bUsingSSWnd);
    const HICON                 BlankIcon               () const;
    VOID                        BlankIcon               (HICON hIcon);
    VOID                        IncrementIcon           (INT nIconSet);
    VOID                        DecrementIcon           (INT nIconSet);
    
    const HINSTANCE             Instance                () const;
    VOID                        Instance                (HINSTANCE hInstance);
    const HWND                  WindowHandle            () const;
    VOID                        WindowHandle            (HWND hWnd);
    SS_TrayIconWnd*             SSWnd                   ();
    VOID                        SSWnd                   (SS_TrayIconWnd* pWnd);
    
    
private:

    PSSTI_ICONSET               m_pIconSet;
    INT                         m_nMaxIconSets;
    INT                         m_nMaxIconsPerSet;

    BOOL                        m_bCurrentlyVisible;
    INT                         m_nCurrentSet;

    HICON                       m_hBlankIcon;

    HINSTANCE                   m_hInstance;
    HWND                        m_hWnd;
    
    SS_TrayIconWnd*             m_pSSWnd;
    BOOL                        m_bUsingSSWnd;
    
};


// ----------------------------------------------------------------------- //
//  SS_TrayIcon Inline Functions
// ----------------------------------------------------------------------- //

inline const SSTI_ICONSET SS_TrayIcon::IconSet(INT nSetID) const
{ return m_pIconSet[nSetID]; }

inline PSSTI_ICONSET SS_TrayIcon::IconSetNC()
{ return m_pIconSet; }

inline const BOOL SS_TrayIcon::CurrentlyVisible() const
{ return m_bCurrentlyVisible; }

inline VOID SS_TrayIcon::CurrentlyVisible(BOOL bCurrentlyVisible)
{ m_bCurrentlyVisible = bCurrentlyVisible; }

inline const INT SS_TrayIcon::CurrentSet() const
{ return m_nCurrentSet; }

inline VOID SS_TrayIcon::CurrentSet(INT nCurrentSet)
{ m_nCurrentSet = nCurrentSet; }

inline const INT SS_TrayIcon::MaxIconSets() const
{ return m_nMaxIconSets; }

inline VOID SS_TrayIcon::MaxIconSets(INT nMaxIconSets)
{ m_nMaxIconSets = nMaxIconSets; }

inline const INT SS_TrayIcon::MaxIconsPerSet() const
{ return m_nMaxIconsPerSet; }

inline VOID SS_TrayIcon::MaxIconsPerSet(INT nMaxIconsPerSet)
{ m_nMaxIconsPerSet = nMaxIconsPerSet; }

inline VOID SS_TrayIcon::Instance(HINSTANCE hInstance)
{ m_hInstance = hInstance; }

inline const HINSTANCE SS_TrayIcon::Instance() const
{ return m_hInstance; }

inline VOID SS_TrayIcon::WindowHandle(HWND hWnd)
{ m_hWnd = hWnd; }

inline const HWND SS_TrayIcon::WindowHandle() const
{ return m_hWnd; }

inline VOID SS_TrayIcon::UsingSSWnd(BOOL bUsingSSWnd)
{ m_bUsingSSWnd = bUsingSSWnd; }

inline const BOOL SS_TrayIcon::UsingSSWnd() const
{ return m_bUsingSSWnd; }

inline VOID SS_TrayIcon::BlankIcon(HICON hIcon)
{ m_hBlankIcon = hIcon; }

inline const HICON SS_TrayIcon::BlankIcon() const
{ return m_hBlankIcon; }

inline VOID SS_TrayIcon::SSWnd(SS_TrayIconWnd* pWnd)
{ m_pSSWnd = pWnd; }
 
inline SS_TrayIconWnd* SS_TrayIcon::SSWnd()
{ return m_pSSWnd; }



#endif // __SS_TrayIcon_h__
