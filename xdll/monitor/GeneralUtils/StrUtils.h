#ifndef _STR_UTILS_H_
#define _STR_UTILS_H_

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

#include <windows.h>
#include <string>
#include <vector>
#include <tchar.h>
#include <assert.h>

using namespace std;

typedef vector<tstring>	VECTOR_TSTRING;
typedef vector<string>	VECTOR_STRING;
typedef vector<wstring> VECTOR_WSTRING;
typedef vector<int>		VECTOR_INT;

class StrUtils
{
public:
	static bool StartsWith(const tstring& strIn, const tstring& strMatch);
	static bool StartsWithA(const string& strIn, const string& strMatch);
	static bool StartsWithW(const wstring& strIn, const wstring& strMatch);

	static bool EndsWith(const tstring& strIn, const tstring& strMatch);
	static bool EndsWithA(const string& strIn, const string& strMatch);
	static bool EndsWithW(const wstring& strIn, const wstring& strMatch);

	static tstring SubEnd(const tstring& strIn, int n);
	static string SubEndA(const string& strIn, int n);
	static wstring SubEndW(const wstring& strIn, int n);

	static int IndexOfFirst(const tstring& strIn, const tstring& strMatch);
	static int IndexOfFirstA(const string& strIn, const string& strMatch);
	static int IndexOfFirstW(const wstring& strIn, const wstring& strMatch);

	static int IndexOfLast(const tstring& strIn, const tstring& strMatch);
	static int IndexOfLastA(const string& strIn, const string& strMatch);
	static int IndexOfLastW(const wstring& strIn, const wstring& strMatch);

	static int SplitString(const tstring& strIn, const tstring& strDelimiter, VECTOR_TSTRING& ret);
	static int SplitStringA(const string& strIn, const string& strDelimiter, VECTOR_STRING& ret);
	static int SplitStringW(const wstring& strIn, const wstring& strDelimiter, VECTOR_WSTRING& ret);

	static INT64 GetINT64ByStr(const tstring& strIn, int nRadix = 10);
	static INT64 GetINT64ByStrA(const string& strIn, int nRadix = 10);
	static INT64 GetINT64ByStrW(const wstring& strIn, int nRadix = 10);

	static tstring GetStrByINT64(INT64 i, int nRadix = 10, bool bFormatHex = true);
	static string GetStrByINT64A(INT64 i, int nRadix = 10, bool bFormatHex = true);
	static wstring GetStrByINT64W(INT64 i, int nRadix = 10, bool bFormatHex = true);

	static tstring Trim(const tstring& strIn);
	static string TrimA(const string& strIn);
	static wstring TrimW(const wstring& strIn);

	static tstring TrimLeft(const tstring& strIn);
	static string TrimLeftA(const string& strIn);
	static wstring TrimLeftW(const wstring& strIn);

	static tstring TrimRigth(const tstring& strIn);
	static string TrimRigthA(const string& strIn);
	static wstring TrimRigthW(const wstring& strIn);

	static tstring Replace(const tstring& strIn, const tstring& strOld, const tstring& strNew);
	static string ReplaceA(const string& strIn, const string& strOld, const string& strNew);
	static wstring ReplaceW(const wstring& strIn, const wstring& strOld, const wstring& strNew);

	static tstring GetFixedString(const tstring& strSource, const tstring& strFill, int nTotalLen, int nEndLen);
	static string GetFixedStringA(const string& strSource, const string& strFill, int nTotalLen, int nEndLen);
	static wstring GetFixedStringW(const wstring& strSource, const wstring& strFill, int nTotalLen, int nEndLen);

	static string WStrToStr(const wstring& strIn);
	static wstring StrToWStr(const string& strIn);
	static string WStrToUTF8(const wstring& strIn);
	static wstring UTF8ToWStr(const string& strIn);
	static string StrToUTF8(const string& strIn);
	static string UTF8ToStr(const string& strIn);

	static tstring ToUpper(const tstring& strIn);
	static string ToUpperA(const string& strIn);
	static wstring ToUpperW(const wstring& strIn);

	static tstring ToLower(const tstring& strIn);
	static string ToLowerA(const string& strIn);
	static wstring ToLowerW(const wstring& strIn);

	static bool CompareNoCase(const tstring& strIn1, const tstring& strIn2);
	static bool CompareNoCaseA(const string& strIn1, const string& strIn2);
	static bool CompareNoCaseW(const wstring& strIn1, const wstring& strIn2);

	static tstring Format(TCHAR* format, ...);
	static string FormatA(char* format, ...);
	static wstring FormatW(WCHAR* format, ...);

	static string UrlEncoder(string str);
	static string UrlDecoder(string str);

#ifdef __AFXWIN_H__
	static bool StartsWith(const CString& strIn, const CString& strMatch);
	static bool EndsWith(const CString& strIn, const CString& strMatch);
	static CString SubEnd(const CString& strIn, int n);
	static int IndexOfFirst(const CString& strIn, const CString& strMatch);
	static int IndexOfLast(const CString& strIn, const CString& strMatch);
	static CString Replace(const CString& strIn, const CString& strOld, const CString& strNew);
	static CString GetFixedString(const CString& strSource, const CString& strFill, int nTotalLen, int nEndLen);
#endif


private:
	StrUtils();
};

#endif