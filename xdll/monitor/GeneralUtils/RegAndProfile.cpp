// 根据工程设置决定是否包含"stdafx.h"
#include "stdafx.h"

#include "RegAndProfile.h"


BOOL RegAndProfile::GetRegStr(
							  tstring strKeyName, 
							  tstring strValueName, 
							  tstring& strValue, 
							  DWORD dwMaxLength,
							  HKEY hKeyParent)
{
	BOOL bRet = TRUE;
	CRegKey rk;
	LONG lRet = rk.Open(hKeyParent, strKeyName.c_str());
	if (lRet != ERROR_SUCCESS)
		lRet = rk.Create(hKeyParent, strKeyName.c_str());

	dwMaxLength++;
	TCHAR* pBuf = new TCHAR[dwMaxLength];
	lRet = rk.QueryValue(pBuf, strValueName.c_str(), &dwMaxLength);
	if (lRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
	}
	else
	{
		strValue = tstring(pBuf);
	}
	rk.Close();
	delete [] pBuf;
	return bRet;
}

BOOL RegAndProfile::GetRegDWORD(tstring strKeyName, tstring strValueName, DWORD& dwValue, HKEY hKeyParent)
{
	BOOL bRet = TRUE;
	CRegKey rk;
	LONG lRet = rk.Open(hKeyParent, strKeyName.c_str());
	if (lRet != ERROR_SUCCESS)
		lRet = rk.Create(hKeyParent, strKeyName.c_str());

	lRet = rk.QueryValue(dwValue, strValueName.c_str());
	if (lRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
	}
	rk.Close();
	return bRet;
}



BOOL RegAndProfile::SetRegStr(tstring strKeyName, tstring strValueName, tstring strValue, HKEY hKeyParent)
{
	BOOL bRet = TRUE;
	CRegKey rk;
	LONG lRet = rk.Open(hKeyParent, strKeyName.c_str());
	if (lRet != ERROR_SUCCESS)
		lRet = rk.Create(hKeyParent, strKeyName.c_str());

	lRet = rk.SetValue(strValue.c_str(), strValueName.c_str());
	if (lRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
	}
	rk.Close();
	return bRet;
}

BOOL RegAndProfile::SetRegDWORD(tstring strKeyName, tstring strValueName, DWORD dwValue, HKEY hKeyParent)
{
	BOOL bRet = TRUE;
	CRegKey rk;
	LONG lRet = rk.Open(hKeyParent, strKeyName.c_str());
	if (lRet != ERROR_SUCCESS)
		lRet = rk.Create(hKeyParent, strKeyName.c_str());

	lRet = rk.SetValue(dwValue, strValueName.c_str());
	if (lRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
	}
	rk.Close();
	return bRet;
}

BOOL RegAndProfile::DelRegSubKey(tstring strKeyName, tstring strSubKey, HKEY hKeyParent)
{
	CRegKey rk;
	if (rk.Open(hKeyParent, strKeyName.c_str()) == ERROR_SUCCESS)
	{
		rk.DeleteSubKey(strSubKey.c_str());
		rk.Close();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL RegAndProfile::DelRegValue(tstring strKeyName, tstring strValueName, HKEY hKeyParent)
{
	CRegKey rk;
	if (rk.Open(hKeyParent, strKeyName.c_str()) == ERROR_SUCCESS)
	{
		rk.DeleteValue(strValueName.c_str());
		rk.Close();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

tstring RegAndProfile::GetINIStr(
								 tstring strAppName,
								 tstring strKeyName, 
								 tstring strDefault, 
								 tstring strFileFullPath,
								 DWORD dwMaxLength)
{
	dwMaxLength++;
	TCHAR* pBuf = new TCHAR[dwMaxLength];
	::GetPrivateProfileString(strAppName.c_str(), strKeyName.c_str(), strDefault.c_str(), pBuf, dwMaxLength, strFileFullPath.c_str());
	tstring s = pBuf;
	delete [] pBuf;
	return s;
}