#ifndef _REG_AND_PROFILE_H_
#define _REG_AND_PROFILE_H_

#include <windows.h>
#include <string>
#include <tchar.h>
#include <atlbase.h>

using namespace std;

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

#define DEFAULT_KEY_PARENT		HKEY_CURRENT_USER

class RegAndProfile
{
public:
	static BOOL GetRegStr(
		tstring strKeyName, 
		tstring strValueName, 
		tstring& strValue, 
		DWORD dwMaxLength = 260, 
		HKEY hKeyParent = DEFAULT_KEY_PARENT);
	static BOOL GetRegDWORD(tstring strKeyName, tstring strValueName, DWORD& dwValue, HKEY hKeyParent = DEFAULT_KEY_PARENT);
	
	static BOOL SetRegStr(tstring strKeyName, tstring strValueName, tstring strValue, HKEY hKeyParent = DEFAULT_KEY_PARENT);
	static BOOL SetRegDWORD(tstring strKeyName, tstring strValueName, DWORD dwValue, HKEY hKeyParent = DEFAULT_KEY_PARENT);
	
	static BOOL DelRegSubKey(tstring strKeyName, tstring strSubKey, HKEY hKeyParent = DEFAULT_KEY_PARENT);
	static BOOL DelRegValue(tstring strKeyName, tstring strValueName, HKEY hKeyParent = DEFAULT_KEY_PARENT);

	static tstring GetINIStr(
		tstring strAppName,
		tstring strKeyName, 
		tstring strDefault, 
		tstring strFileFullPath,
		DWORD dwSize = 260);
private:
	RegAndProfile();
};

#endif