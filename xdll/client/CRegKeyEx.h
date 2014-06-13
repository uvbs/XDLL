#include <atlbase.h>

class CRegKeyEx : public CRegKey
{
public:
	// Enumerate the subkeys of the key.
	LONG EnumKey(DWORD iIndex, LPTSTR pszName, LPDWORD pnNameLength, FILETIME* pftLastWriteTime = NULL);
	LONG SetDWORDValue(LPCTSTR pszValueName, DWORD dwValue);
};

inline LONG CRegKeyEx::EnumKey(DWORD iIndex, LPTSTR pszName, LPDWORD pnNameLength, FILETIME* pftLastWriteTime)
{
	FILETIME ftLastWriteTime;

	if (pftLastWriteTime == NULL)
	{
		pftLastWriteTime = &ftLastWriteTime;
	}

	return ::RegEnumKeyEx(m_hKey, iIndex, pszName, pnNameLength, NULL, NULL, NULL, pftLastWriteTime);
}

inline LONG CRegKeyEx::SetDWORDValue(LPCTSTR pszValueName, DWORD dwValue)
{
	return ::RegSetValueEx(m_hKey, pszValueName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));
}