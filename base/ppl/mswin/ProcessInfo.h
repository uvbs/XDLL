
#pragma once

#include "NtQuery.h"
//#pragma comment(lib, "ntdll.lib")

typedef struct
{
   DWORD Filler[4];
   DWORD InfoBlockAddress;
} __PEB;

typedef struct 
{
   DWORD Filler[17];
   DWORD wszCmdLineAddress;
} __INFOBLOCK;


inline BOOL GetProcessCmdLine(HANDLE hProcess, LPWSTR szCmdLine, DWORD Size)
{
// sanity checks
   assert(hProcess != NULL);
   assert(szCmdLine != NULL);
   assert(Size > 0);
   if ((hProcess == NULL) || (szCmdLine == NULL) || (Size == 0))
      return(FALSE);

// 0. get the Process Environment Block address
   int   iReturn = 1;
   DWORD dwSize;


   PROCESS_BASIC_INFORMATION  pbi;
   pbi.PebBaseAddress = (_PEB*)0x7ffdf000;
// the PEB is always at address 0x7ffdf000 but here is the "right"
// way to get it
   //iReturn = 
   //   ::NtQueryInformationProcess(
   //      hProcess, 
   //      ProcessBasicInformation, 
   //      &pbi, 
   //      sizeof(pbi), 
   //      &dwSize
   //      );


// NtQueryInformationProcess returns a negative value if it fails
   if (iReturn >= 0) 
   {
   // 1. find the Process Environment Block 
      __PEB PEB;
      if (!::ReadProcessMemory(hProcess, pbi.PebBaseAddress, &PEB, sizeof(PEB), &dwSize))
      {
		  // call GetLastError() to know why
		  TRACE("GetProcessCmdLine error 21 %d\n", ::GetLastError());
         return(FALSE);
      }
	  TRACE("GetProcessCmdLine 1 ok 0x%p\n", PEB.InfoBlockAddress);

   // 2. from this PEB, get the address of the block containing a pointer to the CmdLine
      __INFOBLOCK Block;
      if (!::ReadProcessMemory(hProcess, (LPVOID)PEB.InfoBlockAddress, &Block, sizeof(Block), &dwSize))
      {
		  // call GetLastError() to know why
		  TRACE("GetProcessCmdLine error 22 %d\n", ::GetLastError());
         return(FALSE);
	  }
	  TRACE("GetProcessCmdLine 2 ok 0x%p\n", Block.wszCmdLineAddress);

   // 3. get the CmdLine
	  wchar_t wszCmdLine[_MAX_PATH * 4 + 1] = { 0 };
      if (!::ReadProcessMemory(hProcess, (LPVOID)Block.wszCmdLineAddress, wszCmdLine, _MAX_PATH * 4 * sizeof(wchar_t), &dwSize))
      {
		  // call GetLastError() to know why
		  TRACE("GetProcessCmdLine error 23 %d\n", ::GetLastError());
         return(FALSE);
	  }
	  TRACE("GetProcessCmdLine 3 ok %s\n", wszCmdLine);

   // 4. skip the application pathname
   //    it can be empty, "c:\...\app.exe" or c:\...\app.exe 
/*      wchar_t* pPos = wszCmdLine; 
      if (*pPos != L'\0')
      {
         if (*pPos == L'"')
         {
         // find the next " character
            pPos = wcschr(&pPos[1], L'"');
         }
         else
         {
         // find the next SPACE character
            pPos = wcschr(&pPos[1], L' ');
         }

      // skip it
         if (pPos != NULL)
            pPos++;
      }

   // copy it back
      if (pPos != NULL)
      {
         if (*pPos != L'\0')
         {
            wcsncpy(szCmdLine, pPos, Size);
         }
         else
            szCmdLine[0] = L'\0';
      }
      else
         szCmdLine[0] = L'\0';
   }  
   else
   {
      TRACE("Impossible to get command line for process handle 0x%x...\n", hProcess);
      return(FALSE);
   }*/
	  wcsncpy( szCmdLine, wszCmdLine, Size );

   }
   return(TRUE);
}


inline BOOL GetProcessCmdLine(DWORD PID, LPWSTR szCmdLine, DWORD Size)
{
// sanity checks
   if ((PID <= 0) || (szCmdLine == NULL))
   {
	   TRACE("GetProcessCmdLine error 1 %d\n", PID);
      return(FALSE);
   }

// check if we can get information for this process 
   HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
   if (hProcess == NULL)
   {
	   TRACE("GetProcessCmdLine error 2 %d\n", PID);
      return(FALSE);
   }

   BOOL bReturn = GetProcessCmdLine(hProcess, szCmdLine, Size);

// don't forget to release the process handle
   ::CloseHandle(hProcess);

   return(bReturn);
}


