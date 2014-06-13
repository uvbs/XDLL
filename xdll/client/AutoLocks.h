#ifndef _AUTOLOCKS_H_
#define _AUTOLOCKS_H_

#include <Windows.h>
#include <ppl/util/log.h>

class CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();
	CRITICAL_SECTION m_CritSect;
};


class Lock
{
public:
	DWORD m_dwStart;
	Lock(CriticalSection* m_pCritSect);
	~Lock();
private:
	CriticalSection* m_pCritical;
};

#endif