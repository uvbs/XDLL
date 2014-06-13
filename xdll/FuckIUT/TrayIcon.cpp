#include "stdafx.h"
#include "TrayIcon.h"
#include "FuckIUT.h"

LRESULT TrayIcon::OnMouseClickTI(WPARAM wParam, LPARAM lParam)
{
	
	return 0;
}

LRESULT TrayIcon::OnMouseDblClickTI(WPARAM wParam, LPARAM lParam)
{
	CFuckIUTApp* ppliveVaApp = (CFuckIUTApp*) AfxGetApp();
	if ( ppliveVaApp != NULL )
	{
		ppliveVaApp->m_pFuckIUTDlg->OnShowInfo();
	}
	return 0;
}

LRESULT TrayIcon::OnMouseRightClickTI(WPARAM wParam, LPARAM lParam)
{
	CFuckIUTApp* ppliveVaApp = (CFuckIUTApp*) AfxGetApp();
	if ( ppliveVaApp != NULL )
	{
		CMenu* trayMenu = ppliveVaApp->m_menuTrayIcon.GetSubMenu(0);
		if (trayMenu)
		{
			POINT pt;
			GetCursorPos(&pt);

			CFuckIUTDlg* mainWnd = (CFuckIUTDlg*) AfxGetMainWnd();
			::SetForegroundWindow(mainWnd->m_hWnd);
			int nRetCmd = trayMenu->TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTALIGN|TPM_RETURNCMD, pt.x, pt.y, ppliveVaApp->m_pMainWnd);
			//::PostMessage(mainWnd->m_hWnd, WM_NULL, 0, 0);
			DWORD dwError = GetLastError();
			if (nRetCmd)
			{
				ppliveVaApp->m_pFuckIUTDlg->ExecuteSysCmd(nRetCmd);
			}
		}
	}
	return 0;
}