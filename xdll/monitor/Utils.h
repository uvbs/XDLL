#ifndef _UTILS_H_
#define _UTILS_H_

#include <windows.h>
#include <string>

using namespace std;

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

class CUtils
{
public:
	static BOOL IsScreenSaverRunning();
	
	static BOOL IsLocked();

	static void GetMemoryInfo(DWORD processID, DWORD& mem, DWORD& vmem);

	static DWORD GetPIDByName(tstring strProcessName);

	static VECTOR_TSTRING FindFileByExt(tstring strFolder, tstring strExe);

	static bool CheckSingleProcess(HANDLE& hMutex, const tstring& strMutexName);
	
	static void RunSecondProcess(HANDLE hMutex, const tstring& strCommandLine);

	static tstring GetFileVersionStr(const tstring& strFilePath);
	static INT64 GetFileVersionNumber(const tstring& strFilePath);

	// TRUE  pszVersion1 <  pszVersion2
	// FALSE pszVersion1 >= pszVersion2
	static BOOL CompareVersion(PCTSTR pszVersion1, PCTSTR pszVersion2);

	// [uMin, uMax]
	static UINT GetRandomNum(UINT uMin, UINT uMax);
	
	// 0 <= uNumOfHundred <= 100
	static BOOL GetBoolByProbability(UINT uNumOfHundred);

	static void KillAllProcessByName(tstring strModuleName);

	// for map, set...
	template <typename T, typename Container>
	static bool ContainsKey(const Container& container, const T& key)
	{
		return (container.find(key) != container.end());
	}

private:
	CUtils();
};

#endif