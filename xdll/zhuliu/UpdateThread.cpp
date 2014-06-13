// UpdateThread.cpp: implementation of the UpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UpdateThread.h"
#include "Update.h"
#include "AppDM.h"
#include "Utils.h"

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
			
			if (u.Update()) CAppDM::m_bGlobalEnd = TRUE;
		}

		if (CAppDM::m_bGlobalEnd) break;

		Sleep(UPDATE_INTERVAL_SECONDS*1000);
	}
	
	return 0;
}