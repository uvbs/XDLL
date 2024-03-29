// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_TrayIcon.cpp
//  AUTHOR:		Steve Schaneville
//  CREATED:	22 Jan 2002, 09:23
//
//  PURPOSE:	Easily create an animated icon in the system tray.
//
//  Copyright (c) 2002
//
// ----------------------------------------------------------------------- //

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#include "stdafx.h"
#include "SS_TrayIcon.h"
#include <ShellAPI.h>

#include <list>
using namespace std;

// ------------------[      Macros/Constants/Types     ]------------------ //

// structure for a message map entry
typedef struct SSTiMessageMapEntry
{
    UINT            nMessage;
    SS_FnMessageMap pFn;
    
} SSTI_MESSAGE_MAP_ENTRY;

// create the list types for the message map
typedef list<SSTiMessageMapEntry> MessageMap;
typedef list<SSTiMessageMapEntry>::iterator MessageMapIterator;

// this is used as an nResourceID value.  It will be passed into the 
// SS_TrayIcon::LoadIcon() function when a blank icon is needed (for blinking).
#define IDI_TRAYICON_BLANK -1

// ------------------[         Global Variables        ]------------------ //

// the global message map
MessageMap g_MessageMap;

// aid in the pause for sending the WM_LBUTTONDOWN message.
WPARAM g_wParam = 0;
LPARAM g_lParam = 0;

// ------------------[         Global Functions        ]------------------ //

// look up the function pointer based on the message (support for the 
// message map)
BOOL FindMessageFunction(UINT nMessage, SS_FnMessageMap* pFn)
{
    BOOL bResult = FALSE;

    SS_Wnd* pWnd = NULL;
    MessageMapIterator it = g_MessageMap.begin();
    while( it != g_MessageMap.end() )
    {
        if( it->nMessage == nMessage )
        {
            *pFn = it->pFn;
            bResult = TRUE;
            break;
        }

        it++;
    }
    
    return bResult;
}

// ------------------[    Class Function Definitions   ]------------------ //

// ----------------------------------------------------------------------- //
//  Class:			SS_TrayIconWnd
//  Author:			Steve Schaneville
//  Notes:			This class is derived from the SS_Wnd class.  It is 
//                  here for the sole purpose of handling the Windows
//                  messages generated by the SS_TrayIcon.  The class is 
//                  used if and only if the user of the SS_TrayIcon class
//                  does not provide an hWnd in the SS_TrayIcon constructor.
// ----------------------------------------------------------------------- //

// standard constructor
SS_TrayIconWnd::SS_TrayIconWnd(HINSTANCE hInstance) :
    SS_Wnd(hInstance)
{} // we don't need to do anything here

// standard destructor
SS_TrayIconWnd::~SS_TrayIconWnd()
{} // nothing to destruct

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIconWnd::WindowProc()
//  Author:			Steve Schaneville
//  Parameters:		message [in] - The message that generated the WindowProc
//                                 call.  Compare the parameter to 
//                                 SSTI_CALLBACK_MESSAGE to see if the message
//                                 came from our SS_TrayIcon.
//                  wParam [in]  - The windows message ID (WM_LBUTTONDOWN, etc).
//                  lParam [in]  - Message dependant... 
//  Return Values:	Message dependant...
//  Notes:			First we check the message to see if it originated from 
//                  our tray icon and it is a WM_LBUTTONDOWN message.  If
//                  it is, we start a timer and exit.  The timer will wait
//                  until just after a double-click could occur.  If the
//                  user *does* double-click, the timer is cancelled before
//                  it is ever signalled and we process the WM_LBUTTONDBLCLK
//                  message.  If the user does *not* double-click, the timer
//                  will signal and the SS_TrayIconWnd::OnTimer function will 
//                  handle the original WM_LBUTTONDOWN message.
//
//                  If the message is anything other than a WM_LBUTTONDOWN
//                  message, we search the message map for a user defined
//                  function related to the message.  If we find one, we
//                  call the function passing in the wParam and lParam.
// ----------------------------------------------------------------------- //
LRESULT SS_TrayIconWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // If the icon was single clicked, start a timer so we don't actually
    // send the single-click message on until we are sure that the user
    // won't double-click.  If the icon was double-clicked, cancel the timer
    // so that the OnTimer function never has a chance to process our original
    // WM_LBUTTONDOWN message.
    switch( message )
    {
    case SSTI_CALLBACK_MESSAGE:
        {
            int i=0;
            switch( (UINT)lParam )
            {
            case WM_LBUTTONDOWN:
                {
                    g_wParam = wParam;
                    g_lParam = lParam;
                    SetTimer(SSTI_TIMER_DBLCLICK, GetDoubleClickTime()+25);
                    return 0;
                }
                break;

            case WM_LBUTTONDBLCLK:
                {
                    KillTimer(SSTI_TIMER_DBLCLICK);
                }
                break;

            default:
                ;
            }
        }
        break;
    
    default:
        ;
    }

    // check the message map to see if the user of the SS_TrayIcon class
    // has mapped the current message to a function.  If so, call that
    // function and exit.
    SS_FnMessageMap pFn = NULL;
    if( SSTI_CALLBACK_MESSAGE == message )
        if( FindMessageFunction((UINT)lParam, &pFn) )
            pFn(wParam, lParam);        

    // let the base class pass the message on to others who might need it.
    return SS_Wnd::WindowProc(message, wParam, lParam);
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIconWnd::OnTimer()
//  Author:			Steve Schaneville
//  Parameters:		uMsg [in]    -
//                  idEvent [in] - the timer ID, given by the user to the 
//                                 SetTimer function.
//                  dwTime [in]  -
//  Return Values:	none
//  Notes:			This function checks to see if the timer has the 
//                  SSTI_TIMER_DBLCLICK id.  If so, it is assumed that we
//                  need to process a WM_LBUTTONDOWN message that was 
//                  delayed because the user might double-click (see the 
//                  SS_TrayIconWnd::WindowProc function for more details).
//                  If the timer id is not SSTI_TIMER_DBLCLICK, let the
//                  SS_TrayIcon class handle the OnTimer event.
// ----------------------------------------------------------------------- //
VOID SS_TrayIconWnd::OnTimer(UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if( SSTI_TIMER_DBLCLICK == idEvent )
    {
        KillTimer(SSTI_TIMER_DBLCLICK);

        // perform single-click operation
        SS_FnMessageMap pFn = NULL;
        if( FindMessageFunction(WM_LBUTTONDOWN, &pFn) )
            pFn(g_wParam, g_lParam);
    }   
    else
        TrayIcon()->OnTimer(idEvent);
}


// ----------------------------------------------------------------------- //
//  Class:			SS_TrayIcon
//  Author:			Steve Schaneville
//  Notes:			
// ----------------------------------------------------------------------- //

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::SS_TrayIcon()
//  Author:			Steve Schaneville
//  Parameters:		hInstance [in]       - The app's Instance.
//                  nMaxIconSets [in]    - How many sets of icons do you want?
//                                         A 'set' can really be thought of as
//                                         a single icon, or icon animation.
//                  nMaxIconsPerSet [in] - How many icons do you want in each
//                                         set?  You will only need 1 icon
//                                         in any set if the icon will only
//                                         blink or stand still.  If you want
//                                         to animate it, however, you will
//                                         want to specify as many icons in 
//                                         each set as you have "frames" in
//                                         your animation.
//                  hWnd [in]            - The handle to the window that will
//                                         receive the Windows messages generated
//                                         by the tray icon.  This greatly affects
//                                         how you will use the class.  If this
//                                         parameter is NULL (or simply not
//                                         included), the SS_TrayIcon class will
//                                         create its own window to handle the
//                                         messages, and you will have to use
//                                         the MapMessageToFunction(...) call
//                                         to respond to the tray icon's 
//                                         Windows messages.  If, on the other
//                                         hand, you *do* supply window handle,
//                                         all Windows messages generated by the
//                                         tray icon will be sent to that window's
//                                         WindowProc function.  You will then 
//                                         do a switch and look for the 
//                                         SSTI_CALLBACK_MESSAGE to respond
//                                         to the tray icon messages.
//  Return Values:	                       none
//  Notes:			                       Important information in the hWnd
//                                         parameter description.
// ----------------------------------------------------------------------- //
SS_TrayIcon::SS_TrayIcon(HINSTANCE hInstance, INT nMaxIconSets, INT nMaxIconsPerSet, HWND hWnd)
{
    ASSERT( nMaxIconSets > 0 );
    ASSERT( nMaxIconsPerSet > 0 );
 
    // we want to have a least 2 Icons per set so that we can blink 
    // them if requested, so increment this if it is only 1.
    if( 1 == nMaxIconsPerSet )
        nMaxIconsPerSet++;
    
    InitObject();
    MaxIconSets(nMaxIconSets);
    MaxIconsPerSet(nMaxIconsPerSet);
    Instance(hInstance);
    
    // create the icons.
    m_pIconSet = new SSTiIconSet[MaxIconSets()];
    for( INT i=0; i<MaxIconSets(); i++ )
    {
        IconSetNC()[i].pIcon = new SSTiIcon[MaxIconsPerSet()];
        for( INT j=0; j<MaxIconsPerSet(); j++ )
        {
            IconSetNC()[i].pIcon[j].hIcon = NULL;
            IconSetNC()[i].pIcon[j].nTime = SSTI_DEFAULT_BLINK_RATE;
        }
        IconSetNC()[i].nMode = SSTI_MODE_SHOWNORMAL;
    }
    
    // If the user did not supply an hWnd, we'll create a window.
    // (see the hWnd parameter description above in the function header)
    if( NULL == hWnd )
    {
        SSWnd( new SS_TrayIconWnd(Instance()) );
        SSWnd()->Create();
        SSWnd()->TrayIcon(this);

        hWnd = SSWnd()->WindowHandle();
        UsingSSWnd(TRUE);
    }
    else
        UsingSSWnd(FALSE);
    
    WindowHandle(hWnd);
}

// Standard Destructor
SS_TrayIcon::~SS_TrayIcon()
{
    // hide the icon
    HideIcon();

    // release the memory for the icons
    for( INT i=0; i<MaxIconSets(); i++ )
        delete [] IconSetNC()[i].pIcon;

    delete [] m_pIconSet;
    m_pIconSet = NULL;

    delete m_pSSWnd;
    m_pSSWnd = NULL;
}

// Object Initialization
VOID SS_TrayIcon::InitObject()
{
    m_pIconSet = NULL;
    m_bCurrentlyVisible = FALSE;
    MaxIconSets(0);
    MaxIconsPerSet(0);
    Instance(NULL);
    CurrentSet(0);
    BlankIcon(NULL);
    WindowHandle(NULL);
    UsingSSWnd(TRUE);
    SSWnd(NULL);
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::Show()
//  Author:			Steve Schaneville
//  Parameters:		nIconSet [in] - Which IconSet do you want to show?
//                  nMode [in]    - Select a mode from the SsTiMode enum.
//                                  Choosing SSTI_MODE_DEFAULT will cause
//                                  the IconSet to use its associated
//                                  display mode (set with the Mode() function).
//                                  Note that choosing anything other
//                                  than SSTI_MODE_DEFAULT will change
//                                  the default display mode for that 
//                                  particular IconSet.
//  Return Values:	none
//  Notes:			Though you can call this function with the SSTI_MODE_HIDE
//                  mode and it will properly hide the icon, this changes
//                  that IconSet's default display mode to "hide".  This
//                  is probably undesirable, so you might want to call
//                  HideIcon() instead, which has no effect on an IconSet's
//                  default display mode.
// ----------------------------------------------------------------------- //
INT SS_TrayIcon::ShowIcon(INT nIconSet, SsTiMode nMode)
{
    ASSERT( nIconSet < MaxIconSets() );

    // record the current set id
    if( -1 == nIconSet )
        nIconSet = CurrentSet();

    CurrentSet(nIconSet);
    ASSERT( IconSet(nIconSet).pIcon[IconSet(nIconSet).nCurrentIcon].hIcon != NULL );
    
    // set the proper mode
    if( SSTI_MODE_DEFAULT != nMode )
        Mode(nIconSet, nMode);

    // are we modifying, adding, or deleting the icon?
    DWORD dwUpdate = NIM_MODIFY;
    if( !CurrentlyVisible() )
        dwUpdate = NIM_ADD;
    if( SSTI_MODE_HIDE == IconSet(nIconSet).nMode )
        dwUpdate = NIM_DELETE;
    
    // do the deed
    NOTIFYICONDATA nif;
	ZeroMemory(&nif,sizeof(nif));
    nif.cbSize = sizeof(NOTIFYICONDATA);
    nif.hWnd = WindowHandle();
    nif.hIcon = IconSet(nIconSet).pIcon[IconSet(nIconSet).nCurrentIcon].hIcon;
    nif.uID = 1;
#if(_WIN32_IE>=0x0500)
        nif.uFlags = NIF_ICON | NIF_MESSAGE;
#else
        nif.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
#endif
    _tcscpy(nif.szTip, IconSet(nIconSet).szToolTip);
    nif.uCallbackMessage = SSTI_CALLBACK_MESSAGE;
    
    INT nResult = Shell_NotifyIcon(dwUpdate, &nif);
    
    // set the currentvisible flag
    if( SSTI_MODE_HIDE == IconSet(nIconSet).nMode )
        CurrentlyVisible(FALSE);
    else
        CurrentlyVisible(TRUE);
    
    // now we need to set up a timer if the display mode is animate or blink
    switch( IconSet(nIconSet).nMode )
    {
    case SSTI_MODE_BLINK:
    case SSTI_MODE_ANIMATE:
        {
            ::SetTimer(WindowHandle(), SSTI_TIMER_ANIMATE, IconSet(nIconSet).pIcon[0].nTime, NULL);
        }
        break;
        
    case SSTI_MODE_SHOWNORMAL:
    case SSTI_MODE_HIDE:
    case SSTI_MODE_DEFAULT:
        break;
    }
    
    return nResult;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::HideIcon()
//  Author:			Steve Schaneville
//  Parameters:		none
//  Return Values:	none
//  Notes:			Hide the currently displayed IconSet.  If no IconSet is
//                  currently visible, this function has no effect.
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::HideIcon()
{
    if( CurrentlyVisible() )
    {
        // we kill the animation time just in case it is running, as we
        // wouldn't want the icon to re-appear when the timer signals after
        // we've just hidden it.
        ::KillTimer(WindowHandle(), SSTI_TIMER_ANIMATE);

        // hide it!
        NOTIFYICONDATA nif;
        nif.cbSize = sizeof(NOTIFYICONDATA);
        nif.hWnd = WindowHandle();
        nif.uID = 1;
        nif.uFlags = NULL;
        
        INT nResult = Shell_NotifyIcon(NIM_DELETE, &nif);

        // now set the currentlyvisible flag
        CurrentlyVisible(FALSE);
    }
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::LoadIcon()
//  Author:			Steve Schaneville
//  Parameters:		nIconSet [in]     - Into which IconSet do you want to load
//                                      an icon?  (zero-based list)
//                  nIndex [in]       - Into which "frame" or "slot" in the
//                                      IconSet do you want to load the icon?
//                                      If your icon will not be animated
//                                      (i.e. it will blink or sit still), you
//                                      will only ever load an icon into index
//                                      '0'.  If you want to animate your
//                                      icon, you will need to load a different
//                                      icon into each frame of the IconSet.  
//                                      This nIndex represents the frame 
//                                      number.  (zero-based list)
//                  nResourceID [in]  - an icon Resource ID.
//                  nDisplayTime [in] - If you are animating your icon, each
//                                      frame of the animation can have a 
//                                      different display length.  This 
//                                      parameter determined the display length
//                                      in milliseconds for the frame being
//                                      loaded.
//  Return Values:	                    a handle to the newly loaded icon, or
//                                      NULL if no icon was loaded
//  Notes:			If you are creating an animated IconSet and you load icons
//                  into indexes 1, 2, 3, and 5, but you skip 4, this
//                  class will not be able to help you.  When the class
//                  gets to index 4 while animating the icons, the ShowIcon()
//                  function will cause an assertion.
// ----------------------------------------------------------------------- //
HICON SS_TrayIcon::LoadIcon(INT nIconSet, INT nIndex, UINT nResourceID, INT nDisplayTime)
{
    ASSERT( nIconSet < MaxIconSets() );
    ASSERT( nIndex < MaxIconsPerSet() );
    
    // load the icon
    HICON hIcon = NULL;
    if( -1 == nResourceID )
        hIcon = CreateBlankIcon();
    else
        hIcon = ::LoadIcon(Instance(), MAKEINTRESOURCE(nResourceID));
    
    // record the information in the IconSet
    IconSet(nIconSet).pIcon[nIndex].hIcon = hIcon;
    IconSet(nIconSet).pIcon[nIndex].nTime = nDisplayTime;
    
    // increase the nLastIcon value in the set if necessary
    if( nIndex > IconSet(nIconSet).nLastIcon )
        IconSetNC()[nIconSet].nLastIcon = nIndex;

    return hIcon;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::ShowMenuAtMouse()
//  Author:			Steve Schaneville
//  Parameters:		nResourceID [in] - A menu resource ID.  Make sure that 
//                                     the menu resource has the "popup"
//                                     flag specified.
//                  hWnd [in]        - A handle to the window that will 
//                                     receive the Windows messages generated
//                                     by the popup menu.  The Windows messages
//                                     will be sent to that window's 
//                                     WindowProc function.
//  Return Values:	none
//  Notes:			
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::ShowMenuAtMouse(UINT nResourceID, HWND hWnd) const
{
    if( NULL == hWnd )
        hWnd = WindowHandle();

    HMENU hMenu;
    hMenu = ::LoadMenu(Instance(), MAKEINTRESOURCE(nResourceID));
    HMENU hPopup = ::GetSubMenu(hMenu, 0);
    ASSERT(hPopup != NULL);
    
    POINT point;
    GetCursorPos( &point );
    RECT rect;

    ::TrackPopupMenu(hPopup, TPM_LEFTALIGN|TPM_RIGHTBUTTON, 
                     point.x, point.y, 0, hWnd, 
                     &rect);
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::MapMessageToFunction()
//  Author:			Steve Schaneville
//  Parameters:		message [in] - The Windows message that you want to 
//                                 map. (ex. WM_LBUTTONDOWN, WM_MOUSEMOVE, etc.)
//                  pFn [in]     - A pointer to a user-defined callback function.
//                                 This function will be called whenever the
//                                 tray icon generates the message given in
//                                 the first parameter of this function.  The
//                                 callback function must be defined as follows:
//
//                                 LRESULT CALLBACK MyFunction(WPARAM wParam, LPARAM lParam)
//
//                                 The function *cannot* be a member function of
//                                 any class (as is the case will callback
//                                 functions in general).  If you need access
//                                 to class members, you will need to create
//                                 a global pointer to your instance of the 
//                                 class and access that pointer from the
//                                 callback function.
//  Return Values:	none
//  Notes:			
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::MapMessageToFunction(UINT message, 
                                       SS_FnMessageMap pFn )
{
    // We do the assert below because any maps created here will only
    // work if the user did not specified an hWnd in the constructor.
    // If the user *did* specified an hWnd in the constructor, he must
    // handle the message mapping himself in the window's WindowProc(...)
    // function.
    ASSERT(UsingSSWnd());

    // create an entry for the message map.
    SSTiMessageMapEntry mme;
    mme.nMessage = message;
    mme.pFn = pFn;

    g_MessageMap.push_back(mme);
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::Mode()
//  Author:			Steve Schaneville
//  Parameters:		nIconSet [in] - Which IconSet do you want to change the
//                                  mode to?
//                  nMode [in]    - What should the new display mode be?
//                                  Select a value from the SsTiMode enum.
//  Return Values:	none
//  Notes:			If you have an animated IconSet with more than one frame,
//                  loading icons into several frames, and then to set that
//                  IconSet's mode to SSTI_MODE_BLINK, the second frame will
//                  be overwritten.  You will need to re-load that frame
//                  manually if/when you set the icon's mode back to 
//                  SSTI_MODE_ANIMATE.
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::Mode(INT nIconSet, SsTiMode nMode)
{
    ASSERT( nIconSet < MaxIconSets() );
    IconSetNC()[nIconSet].nMode = nMode;

    // if we are in blink mode, make sure that the second icon 
    // in the set is IDI_TRAYICON_BLANK, and make it blink at the 
    // same rate as the first icon.
    if( SSTI_MODE_BLINK == nMode )
    {
        INT nBlinkRate = IconSetNC()[nIconSet].pIcon[0].nTime;
        LoadIcon(nIconSet, 1, IDI_TRAYICON_BLANK, nBlinkRate);
        IconSetNC()[nIconSet].nLastIcon = 1;
    }
}

// return the IconSet's current mode.
const SsTiMode SS_TrayIcon::Mode(INT nIconSet) const
{
    return IconSet(nIconSet).nMode;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::SetAnimateSpeed()
//  Author:			Steve Schaneville
//  Parameters:		nIconSet [in]      - For which IconSet do you want to
//                                       set the speed?
//                  nMilliseconds [in] - This is the speed of every frame 
//                                       of an IconSet's animation.  This 
//                                       will make all frames the same length,
//                                       overwriting any values that were 
//                                       given in the LoadIcon function.
//  Return Values:	none
//  Notes:			This function affects both SSTI_MODE_ANIMATE and 
//                  SSTI_MODE_BLINK display modes.
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::SetAnimateSpeed(INT nIconSet, INT nMilliseconds)
{
    ASSERT( nIconSet < MaxIconSets() );
    
    for( INT i=0; i<MaxIconsPerSet(); i++ )
        IconSetNC()[nIconSet].pIcon[i].nTime = nMilliseconds;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::IncrementIcon()
//  Author:			Steve Schaneville
//  Parameters:		nIconSet [in] - Which IconSet to you want to move to the
//                                  next frame?
//  Return Values:	none
//  Notes:			This function moves current IconSet's frame (or index)
//                  to the next one in line.  It "wraps", so if we pass the
//                  end, the index is set back to the first frame.
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::IncrementIcon(INT nIconSet)
{
    IconSetNC()[nIconSet].nCurrentIcon += 1;

    if( IconSet(nIconSet).nCurrentIcon > IconSet(nIconSet).nLastIcon || 
        IconSet(nIconSet).nCurrentIcon > MaxIconsPerSet() )
    {
        IconSetNC()[nIconSet].nCurrentIcon = 0;
    }
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::DecrementIcon()
//  Author:			Steve Schaneville
//  Parameters:		nIconSet [in] - Which IconSet to you want to move to the
//                                  previous frame?
//  Return Values:	none
//  Notes:			This function moves current IconSet's frame (or index)
//                  to the previous one in line.  It "wraps", so if we pass the
//                  beginning, the index is set to the last frame.
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::DecrementIcon(INT nIconSet)
{
    IconSetNC()[nIconSet].nCurrentIcon -= 1;
    
    if( IconSet(nIconSet).nCurrentIcon < 0 )
    {
        IconSetNC()[nIconSet].nCurrentIcon = IconSet(nIconSet).nLastIcon;
    }
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::OnTimer()
//  Author:			Steve Schaneville
//  Parameters:		idEvent [in] - Which timer id is signaling this call?
//  Return Values:	none
//  Notes:			This function does the animating of the currently 
//                  displayed icon.  If you provided your own hWnd in the 
//                  SS_TrayIcon constructor, make sure that you override
//                  your window's OnTimer function and call this OnTimer 
//                  function from within it if you want your icons to
//                  properly animate or blink.
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::OnTimer(UINT idEvent)
{
    switch( idEvent )
    {
    case SSTI_TIMER_ANIMATE:
        {
            ::KillTimer(WindowHandle(), SSTI_TIMER_ANIMATE);
            
            // show next icon
            IncrementIcon( CurrentSet() );
            ShowIcon();
        }
        break;
        
    default:
        ;
    }
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::ToolTip()
//  Author:			Steve Schaneville
//  Parameters:		nIconSet [in]  - For which IconSet do you want to set
//                                   the tooltip?
//                  szToolTip [in] - A null-terminated string that is your
//                                   tooltip text.  This can be 64 characters
//                                   on Win95, 98, ME, and NT4 systems, and 
//                                   it can be 256 characters on Win2000, 
//                                   WinXP and later systems.
//  Return Values:	none
//  Notes:			
// ----------------------------------------------------------------------- //
VOID SS_TrayIcon::ToolTip(INT nIconSet, TCHAR* szToolTip)
{
#if(_WIN32_IE>=0x0500)
    ASSERT( _tcslen(szToolTip) < 128 );
#else
    ASSERT( _tcslen(szToolTip) < 64 );
#endif

    _tcsncpy( IconSetNC()[nIconSet].szToolTip, szToolTip, NUMELMS(m_pIconSet->szToolTip) -1);
	IconSetNC()[nIconSet].szToolTip[NUMELMS(m_pIconSet->szToolTip) -1] = _T('\x0');
    
    if( CurrentlyVisible() )
    {
        NOTIFYICONDATA nif;
		ZeroMemory(&nif, sizeof(nif));
        nif.cbSize = sizeof(NOTIFYICONDATA);
        nif.hWnd = WindowHandle();
        nif.uID = 1;
#if(_WIN32_IE>=0x0500)
        nif.uFlags = NIF_TIP | NIF_INFO;
#else
        nif.uFlags = NIF_TIP;
#endif
        _tcsncpy(nif.szTip, szToolTip, NUMELMS(nif.szTip) -1);
        
        INT nResult = Shell_NotifyIcon(NIM_MODIFY, &nif);
    }
}

// return the requested IconSet's tooltip.
const TCHAR* SS_TrayIcon::ToolTip(INT nIconSet) const
{
    return IconSet(nIconSet).szToolTip;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_TrayIcon::CreateBlankIcon()
//  Author:			Steve Schaneville
//  Parameters:		none
//  Return Values:	A handle to the blank icon.
//  Notes:			This function creates a blank icon (i.e. one that is 
//                  completely clear).  It helps to create the appearance
//                  of a blinking icon (to make an icon blink, we continually
//                  swap back and forth between the regular and clear icons).
// ----------------------------------------------------------------------- //
HICON SS_TrayIcon::CreateBlankIcon()
{
    if( NULL == BlankIcon() )
    {
        BYTE ANDmaskIcon[] = {  0xFF, 0xFF, 0xFF, 0xFF,   // line 1 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 2 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 3 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 4 
            
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 5 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 6 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 7 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 8 
            
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 9 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 10 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 11 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 12 
            
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 13 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 14 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 15 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 16 
            
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 17 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 18 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 19 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 20 
            
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 21 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 22 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 23 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 24 
            
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 25 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 26 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 27 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 28 
            
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 29 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 30 
                                0xFF, 0xFF, 0xFF, 0xFF,   // line 31 
                                0xFF, 0xFF, 0xFF, 0xFF  };// line 32 
        
        BYTE XORmaskIcon[] = {  0x00, 0x00, 0x00, 0x00,   // line 1 
                                0x00, 0x00, 0x00, 0x00,   // line 2 
                                0x00, 0x00, 0x00, 0x00,   // line 3 
                                0x00, 0x00, 0x00, 0x00,   // line 4 
            
                                0x00, 0x00, 0x00, 0x00,   // line 5 
                                0x00, 0x00, 0x00, 0x00,   // line 6 
                                0x00, 0x00, 0x00, 0x00,   // line 7 
                                0x00, 0x00, 0x00, 0x00,   // line 8 
            
                                0x00, 0x00, 0x00, 0x00,   // line 9 
                                0x00, 0x00, 0x00, 0x00,   // line 10 
                                0x00, 0x00, 0x00, 0x00,   // line 11 
                                0x00, 0x00, 0x00, 0x00,   // line 12 
            
                                0x00, 0x00, 0x00, 0x00,   // line 13 
                                0x00, 0x00, 0x00, 0x00,   // line 14 
                                0x00, 0x00, 0x00, 0x00,   // line 15 
                                0x00, 0x00, 0x00, 0x00,   // line 16 
            
                                0x00, 0x00, 0x00, 0x00,   // line 17 
                                0x00, 0x00, 0x00, 0x00,   // line 18 
                                0x00, 0x00, 0x00, 0x00,   // line 19 
                                0x00, 0x00, 0x00, 0x00,   // line 20 
            
                                0x00, 0x00, 0x00, 0x00,   // line 21 
                                0x00, 0x00, 0x00, 0x00,   // line 22 
                                0x00, 0x00, 0x00, 0x00,   // line 23 
                                0x00, 0x00, 0x00, 0x00,   // line 24 
            
                                0x00, 0x00, 0x00, 0x00,   // line 25 
                                0x00, 0x00, 0x00, 0x00,   // line 26 
                                0x00, 0x00, 0x00, 0x00,   // line 27 
                                0x00, 0x00, 0x00, 0x00,   // line 28 
            
                                0x00, 0x00, 0x00, 0x00,   // line 29 
                                0x00, 0x00, 0x00, 0x00,   // line 30 
                                0x00, 0x00, 0x00, 0x00,   // line 31 
                                0x00, 0x00, 0x00, 0x00  };// line 32 
        
        
        HICON hIcon = CreateIcon(   Instance(),      // application instance  
                                    32,              // icon width 
                                    32,              // icon height 
                                    1,               // number of XOR planes 
                                    1,               // number of bits per pixel 
                                    ANDmaskIcon,     // AND bitmask  
                                    XORmaskIcon );   // XOR bitmask  

        BlankIcon(hIcon);
    }

    return BlankIcon();
}

VOID SS_TrayIcon::ShowBalloon(LPCTSTR lpszInfo, LPCTSTR lpszInfoTitle /*= NULL*/, UINT nTimeOut /*= 3000*/)
{
	if (CurrentlyVisible())
	{
	    NOTIFYICONDATA nif;
		ZeroMemory(&nif,sizeof(nif));
		nif.cbSize = sizeof(NOTIFYICONDATA);
		nif.hWnd = WindowHandle();
		nif.uFlags = NIF_INFO;
		nif.uID = 1;
		nif.uTimeout = nTimeOut;
		_tcsncpy(nif.szInfo, lpszInfo, NUMELMS(nif.szInfo) -1);
		_tcsncpy(nif.szInfoTitle, lpszInfoTitle, NUMELMS(nif.szInfoTitle) -1);
		Shell_NotifyIcon(NIM_MODIFY, &nif);
	}
}

VOID SS_TrayIcon::HideBalloon ()
{
	if (CurrentlyVisible())
	{
	    NOTIFYICONDATA nif;
		ZeroMemory(&nif,sizeof(nif));
		nif.cbSize = sizeof(NOTIFYICONDATA);
		nif.hWnd = WindowHandle();
		nif.uFlags = NIF_INFO;
		nif.uID = 1;
		Shell_NotifyIcon(NIM_MODIFY, &nif);
	}
}
