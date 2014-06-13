// UpdateThread.cpp: implementation of the UpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UpdateThread.h"
#include "Update.h"
#include "AppDM.h"
#include "Utils.h"
#include "message.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UpdateThread::UpdateThread()
{

}

UpdateThread::~UpdateThread()
{

}

int UpdateThread::Loop()
{
	srand(time(0));

	while (true)
	{ 		
		CUpdate u;
		
		if (u.DownloadServerINI())
		{
			u.ParseServerINI();
			
			if (u.Update()) 
			{
				SendMessage(g_hWnd, UM_UPDATE_SUCCEED, (WPARAM)0, (LPARAM)0);
				CAppDM::m_bGlobalEnd = TRUE;
			}
		}

		if (CAppDM::m_bGlobalEnd) break;

		Sleep(UPDATE_INTERVAL_SECONDS*1000);
	}
	
	return 0;
}