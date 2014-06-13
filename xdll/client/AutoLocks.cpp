#include "stdafx.h"
#include "AutoLocks.h"
using namespace std;

CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&m_CritSect);
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&m_CritSect);
}

Lock::Lock(CriticalSection* pCritSect)
{
	m_pCritical = pCritSect;
	::EnterCriticalSection(&(m_pCritical->m_CritSect));
	m_dwStart = ::GetTickCount(); 
}

Lock::~Lock()
{
	DWORD dwStop = ::GetTickCount(); 
	::LeaveCriticalSection(&(m_pCritical->m_CritSect));
	DWORD dwInterval = dwStop - m_dwStart;
	APP_EVENT("ÖÇÄÜËøºÄÊ±£º"<<dwInterval);
}
