// 根据工程设置决定是否包含"stdafx.h"
#include "stdafx.h"

#include "StrUtils.h"
#include <algorithm>

#pragma warning(disable:4267)


#ifdef __AFXWIN_H__
bool StrUtils::StartsWith(const CString& strIn, const CString& strMatch)
{
	tstring strIn1(strIn);
	tstring strMatch1(strMatch);
	return StartsWith(strIn1, strMatch1);
}
#endif
bool StrUtils::StartsWith(const tstring& strIn, const tstring& strMatch)
{
#ifndef _UNICODE
	return StartsWithA(strIn, strMatch);
#else
	return StartsWithW(strIn, strMatch);
#endif
}
bool StrUtils::StartsWithA(const string& strIn, const string& strMatch)
{
	return strIn.compare(0, strMatch.size(), strMatch) == 0;
}
bool StrUtils::StartsWithW(const wstring& strIn, const wstring& strMatch)
{
	return strIn.compare(0, strMatch.size(), strMatch) == 0;
}

#ifdef __AFXWIN_H__
bool StrUtils::EndsWith(const CString& strIn, const CString& strMatch)
{
	tstring strIn1(strIn);
	tstring strMatch1(strMatch);
	return EndsWith(strIn1, strMatch1);
}
#endif
bool StrUtils::EndsWith(const tstring& strIn, const tstring& strMatch)
{
#ifndef _UNICODE
	return EndsWithA(strIn, strMatch);
#else
	return EndsWithW(strIn, strMatch);
#endif
}
bool StrUtils::EndsWithA(const string& strIn, const string& strMatch)
{
	if (strMatch.size() > strIn.size())
		return false;
	return strIn.compare(strIn.size() - strMatch.size(), strMatch.size(), strMatch) == 0;
}
bool StrUtils::EndsWithW(const wstring& strIn, const wstring& strMatch)
{
	if (strMatch.size() > strIn.size())
		return false;
	return strIn.compare(strIn.size() - strMatch.size(), strMatch.size(), strMatch) == 0;
}

#ifdef __AFXWIN_H__
CString StrUtils::SubEnd(const CString& strIn, int n)
{
	tstring strIn1(strIn);
	return CString(SubEnd(strIn1, n).c_str());
}
#endif
tstring StrUtils::SubEnd(const tstring& strIn, int n)
{
#ifndef _UNICODE
	return SubEndA(strIn, n);
#else
	return SubEndW(strIn, n);
#endif
}
string StrUtils::SubEndA(const string& strIn, int n)
{
	if (n >= (int)strIn.size())
	{
		return strIn;
	}
	else
	{
		return strIn.substr(strIn.size() - n);
	}
}
wstring StrUtils::SubEndW(const wstring& strIn, int n)
{
	if (n >= (int)strIn.size())
	{
		return strIn;
	}
	else
	{
		return strIn.substr(strIn.size() - n);
	}
}

#ifdef __AFXWIN_H__
int StrUtils::IndexOfFirst(const CString& strIn, const CString& strMatch)
{
	tstring strIn1(strIn);
	tstring strMatch1(strMatch);
	return IndexOfFirst(strIn1, strMatch1);
}
#endif
int StrUtils::IndexOfFirst(const tstring& strIn, const tstring& strMatch)
{
#ifndef _UNICODE
	return IndexOfFirstA(strIn, strMatch);
#else
	return IndexOfFirstW(strIn, strMatch);
#endif
}
int StrUtils::IndexOfFirstA(const string& strIn, const string& strMatch)
{
	return strIn.find(strMatch, 0);
}
int StrUtils::IndexOfFirstW(const wstring& strIn, const wstring& strMatch)
{
	return strIn.find(strMatch, 0);
}

#ifdef __AFXWIN_H__
int StrUtils::IndexOfLast(const CString& strIn, const CString& strMatch)
{
	tstring strIn1(strIn);
	tstring strMatch1(strMatch);
	return IndexOfLast(strIn1, strMatch1);
}
#endif
int StrUtils::IndexOfLast(const tstring& strIn, const tstring& strMatch)
{
#ifndef _UNICODE
	return IndexOfLastA(strIn, strMatch);
#else
	return IndexOfLastW(strIn, strMatch);
#endif
}
int StrUtils::IndexOfLastA(const string& strIn, const string& strMatch)
{
	return strIn.rfind(strMatch, strIn.size() - 1);
}
int StrUtils::IndexOfLastW(const wstring& strIn, const wstring& strMatch)
{
	return strIn.rfind(strMatch, strIn.size() - 1);
}

// 此函数如果要处理中文 则必须用UNICODE字符串!
int StrUtils::SplitString(const tstring& strIn, const tstring& strDelimiter, VECTOR_TSTRING& ret)
{
#ifndef _UNICODE
	return SplitStringA(strIn, strDelimiter, ret);
#else
	return SplitStringW(strIn, strDelimiter, ret);
#endif
}
int StrUtils::SplitStringA(const string& strIn, const string& strDelimiter, VECTOR_STRING& ret)
{
	ret.clear();

	int iPos = 0;
	int newPos = -1;
	int delimiterLength = strDelimiter.size();
	int strInLength = strIn.size();

	if (delimiterLength == 0 || strInLength == 0)
		return 0;

	VECTOR_INT positions;

	newPos = strIn.find(strDelimiter, 0);

	if (newPos < 0)
	{
		ret.push_back(strIn);
		return 1;
	}

	int numFound = 0;

	while (newPos >= iPos)
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = strIn.find(strDelimiter, iPos + delimiterLength);
	}

	for (size_t i = 0; i <= positions.size(); ++i)
	{
		string s("");
		if (i == 0) 
		{ 
			s = strIn.substr(i, positions[i]);
		}
		else
		{
			int offset = positions[i-1] + delimiterLength;
			if (offset < strInLength)
			{
				if (i == positions.size())
				{
					s = strIn.substr(offset);
				}
				else
				{
					s = strIn.substr(offset, positions[i] - positions[i-1] - delimiterLength);
				}
			}
		}

		if (s.size() > 0)
		{
			ret.push_back(s);
		}
	}
	return numFound;
}
int StrUtils::SplitStringW(const wstring& strIn, const wstring& strDelimiter, VECTOR_WSTRING& ret)
{
	ret.clear();

	int iPos = 0;
	int newPos = -1;
	int delimiterLength = strDelimiter.size();
	int strInLength = strIn.size();

	if (delimiterLength == 0 || strInLength == 0)
		return 0;

	VECTOR_INT positions;

	newPos = strIn.find(strDelimiter, 0);

	if (newPos < 0)
	{
		ret.push_back(strIn);
		return 1;
	}

	int numFound = 0;

	while (newPos >= iPos)
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = strIn.find(strDelimiter, iPos + delimiterLength);
	}

	for (size_t i = 0; i <= positions.size(); ++i)
	{
		wstring s(L"");
		if (i == 0) 
		{ 
			s = strIn.substr(i, positions[i]);
		}
		else
		{
			int offset = positions[i-1] + delimiterLength;
			if (offset < strInLength)
			{
				if (i == positions.size())
				{
					s = strIn.substr(offset);
				}
				else
				{
					s = strIn.substr(offset, positions[i] - positions[i-1] - delimiterLength);
				}
			}
		}

		if (s.size() > 0)
		{
			ret.push_back(s);
		}
	}
	return numFound;
}

INT64 StrUtils::GetINT64ByStr(const tstring& strIn, int nRadix)
{
#ifndef _UNICODE
	return GetINT64ByStrA(strIn, nRadix);
#else
	return GetINT64ByStrW(strIn, nRadix);
#endif
}
INT64 StrUtils::GetINT64ByStrA(const string& strIn, int nRadix)
{
	if (nRadix != 10 && nRadix != 16)
	{
		// error
		return 0;
	}

	INT64 i = 0;
	int flag = 0;

	if (nRadix == 10)
	{
		flag = sscanf(strIn.c_str(), "%I64d", &i);
	} 
	else
	{
		flag = sscanf(strIn.c_str(), "%I64x", &i);
	}

	if (flag <= 0)
	{
		// error
		i = 0;
	}

	return i;
}
INT64 StrUtils::GetINT64ByStrW(const wstring& strIn, int nRadix)
{
	if (nRadix != 10 && nRadix != 16)
	{
		// error
		return 0;
	}

	INT64 i = 0;
	int flag = 0;

	if (nRadix == 10)
	{
		flag = swscanf(strIn.c_str(), L"%I64d", &i);
	} 
	else
	{
		flag = swscanf(strIn.c_str(), L"%I64x", &i);
	}

	if (flag <= 0)
	{
		// error
		i = 0;
	}

	return i;
}

tstring StrUtils::GetStrByINT64(INT64 i, int nRadix, bool bFormatHex)
{
#ifndef _UNICODE
	return GetStrByINT64A(i, nRadix, bFormatHex);
#else
	return GetStrByINT64W(i, nRadix, bFormatHex);
#endif
}
string StrUtils::GetStrByINT64A(INT64 i, int nRadix, bool bFormatHex)
{
#define MAX_LENGTH 50

	char buf[MAX_LENGTH] = { 0 };
	if (_i64toa(i, buf, nRadix) == 0)
	{
		string s(buf);
		if ((nRadix == 16) && bFormatHex && (i <= 0xFFFFFFFF))
		{
			sprintf(buf, "0x%08X", i);
			s = buf;
		}
		return s;
	}
	else
	{
		// error
		return string("");
	}
}
wstring StrUtils::GetStrByINT64W(INT64 i, int nRadix, bool bFormatHex)
{
#define MAX_LENGTH 50

	WCHAR buf[MAX_LENGTH] = { 0 };
	if (_i64tow(i, buf, nRadix) == 0)
	{
		wstring s(buf);
		if ((nRadix == 16) && bFormatHex && (i <= 0xFFFFFFFF))
		{
			swprintf(buf, L"0x%08X", i);
			s = buf;
		}
		return s;
	}
	else
	{
		// error
		return wstring(L"");
	}
}

tstring StrUtils::Trim(const tstring& strIn)
{
#ifndef _UNICODE
	return TrimA(strIn);
#else
	return TrimW(strIn);
#endif
}
string StrUtils::TrimA(const string& strIn)
{
	string s(strIn);
	s.erase(0, s.find_first_not_of(" \r\n"));
	s.erase(s.find_last_not_of(" \r\n") + 1);
	return s;
}
wstring StrUtils::TrimW(const wstring& strIn)
{
	wstring s(strIn);
	s.erase(0, s.find_first_not_of(L" \r\n"));
	s.erase(s.find_last_not_of(L" \r\n") + 1);
	return s;
}

tstring StrUtils::TrimLeft(const tstring& strIn)
{
#ifndef _UNICODE
	return TrimLeftA(strIn);
#else
	return TrimLeftW(strIn);
#endif
}
string StrUtils::TrimLeftA(const string& strIn)
{
	string s(strIn);
	s.erase(0, s.find_first_not_of(" \r\n"));
	return s;
}
wstring StrUtils::TrimLeftW(const wstring& strIn)
{
	wstring s(strIn);
	s.erase(0, s.find_first_not_of(L" \r\n"));
	return s;
}

tstring StrUtils::TrimRigth(const tstring& strIn)
{
#ifndef _UNICODE
	return TrimRigthA(strIn);
#else
	return TrimRigthW(strIn);
#endif
}
string StrUtils::TrimRigthA(const string& strIn)
{
	string s(strIn);
	s.erase(s.find_last_not_of(" \r\n") + 1);
	return s;
}
wstring StrUtils::TrimRigthW(const wstring& strIn)
{
	wstring s(strIn);
	s.erase(s.find_last_not_of(L" \r\n") + 1);
	return s;
}

#ifdef __AFXWIN_H__
CString StrUtils::Replace(const CString& strIn, const CString& strOld, const CString& strNew)
{
	tstring strIn1(strIn);
	tstring strOld1(strOld);
	tstring strNew1(strNew);
	return CString(Replace(strIn1, strOld1, strNew1).c_str());
}
#endif
tstring StrUtils::Replace(const tstring& strIn, const tstring& strOld, const tstring& strNew)
{
#ifndef _UNICODE
	return ReplaceA(strIn, strOld, strNew);
#else
	return ReplaceW(strIn, strOld, strNew);
#endif
}
string StrUtils::ReplaceA(const string& strIn, const string& strOld, const string& strNew)
{
	string s(strIn);
	while ((int)s.find(strOld) != string::npos)
	{
		s.replace(s.find(strOld), strOld.size(), strNew);
	}
	return s;
}
wstring StrUtils::ReplaceW(const wstring& strIn, const wstring& strOld, const wstring& strNew)
{
	wstring s(strIn);
	while ((int)s.find(strOld) != wstring::npos)
	{
		s.replace(s.find(strOld), strOld.size(), strNew);
	}
	return s;
}

// strSource    -> http://www.baidu.com/img/baidu.gif
// strFill      -> ...
// nTotalLen    -> 10
// nEndLen      -> 3
// ret          -> http...gif
#ifdef __AFXWIN_H__
CString StrUtils::GetFixedString(const CString& strSource, const CString& strFill, int nTotalLen, int nEndLen)
{
	tstring strSource1(strSource);
	tstring strFill1(strFill);
	return CString(GetFixedString(strSource1, strFill1, nTotalLen, nEndLen).c_str());
}
#endif
tstring StrUtils::GetFixedString(const tstring& strSource, const tstring& strFill, int nTotalLen, int nEndLen)
{
#ifndef _UNICODE
	return GetFixedStringA(strSource, strFill, nTotalLen, nEndLen);
#else
	return GetFixedStringW(strSource, strFill, nTotalLen, nEndLen);
#endif
}
string StrUtils::GetFixedStringA(const string& strSource, const string& strFill, int nTotalLen, int nEndLen)
{
	string strRet("");

	if ((int)strSource.size() <= nTotalLen)
	{
		strRet = strSource;
		return strRet;
	}

	string strStart = strSource.substr(0, nTotalLen - nEndLen - strFill.size());
	string strEnd = strSource.substr(strSource.size() - nEndLen, nEndLen);

	strRet = strStart + strFill + strEnd;
	return strRet;
}
wstring StrUtils::GetFixedStringW(const wstring& strSource, const wstring& strFill, int nTotalLen, int nEndLen)
{
	wstring strRet(L"");

	if ((int)strSource.size() <= nTotalLen)
	{
		strRet = strSource;
		return strRet;
	}

	wstring strStart = strSource.substr(0, nTotalLen - nEndLen - strFill.size());
	wstring strEnd = strSource.substr(strSource.size() - nEndLen, nEndLen);

	strRet = strStart + strFill + strEnd;
	return strRet;
}

string StrUtils::WStrToStr(const wstring& strIn)
{
	int nBufSize = ::WideCharToMultiByte(GetACP(), 0, strIn.c_str(), -1, NULL, 0, 0, FALSE);

	char* szBuf = new char[nBufSize];

	::WideCharToMultiByte(GetACP(), 0, strIn.c_str(), -1, szBuf, nBufSize, 0, FALSE);

	string strRet(szBuf);

	delete [] szBuf;
	szBuf = NULL;

	return strRet;
}
wstring StrUtils::StrToWStr(const string& strIn)
{
	int nBufSize = ::MultiByteToWideChar(GetACP(), 0, strIn.c_str(), -1, NULL, 0); 

	wchar_t* wsBuf = new wchar_t[nBufSize];

	::MultiByteToWideChar(GetACP(), 0, strIn.c_str(), -1, wsBuf, nBufSize); 

	wstring wstrRet(wsBuf);

	delete [] wsBuf;
	wsBuf = NULL;

	return wstrRet;
}

string StrUtils::WStrToUTF8(const wstring& strIn)
{
	int nBufSize = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), -1, NULL, 0, 0, FALSE);

	char* szBuf = new char[nBufSize];

	::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), -1, szBuf, nBufSize, 0, FALSE);

	string strRet(szBuf);

	delete [] szBuf;
	szBuf = NULL;

	return strRet;
}
wstring StrUtils::UTF8ToWStr(const string& strIn)
{
	int nBufSize = ::MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, NULL, 0); 

	wchar_t* wsBuf = new wchar_t[nBufSize];

	::MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, wsBuf, nBufSize); 

	wstring wstrRet(wsBuf);

	delete [] wsBuf;
	wsBuf = NULL;

	return wstrRet;
}

string StrUtils::StrToUTF8(const string& strIn)
{
	wstring strW = StrToWStr(strIn);
	return WStrToUTF8(strW);
}
string StrUtils::UTF8ToStr(const string& strIn)
{
	wstring strW = UTF8ToWStr(strIn);
	return WStrToStr(strW);
}

tstring StrUtils::ToUpper(const tstring& strIn)
{
#ifndef _UNICODE
	return ToUpperA(strIn);
#else
	return ToUpperW(strIn);
#endif
}
string StrUtils::ToUpperA(const string& strIn)
{
	string s(strIn);
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}
wstring StrUtils::ToUpperW(const wstring& strIn)
{
	wstring s(strIn);
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

tstring StrUtils::ToLower(const tstring& strIn)
{
#ifndef _UNICODE
	return ToLowerA(strIn);
#else
	return ToLowerW(strIn);
#endif
}
string StrUtils::ToLowerA(const string& strIn)
{
	string s(strIn);
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}
wstring StrUtils::ToLowerW(const wstring& strIn)
{
	wstring s(strIn);
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

bool StrUtils::CompareNoCase(const tstring& strIn1, const tstring& strIn2)
{
#ifndef _UNICODE
	return CompareNoCaseA(strIn1, strIn2);
#else
	return CompareNoCaseW(strIn1, strIn2);
#endif
}
bool StrUtils::CompareNoCaseA(const string& strIn1, const string& strIn2)
{
	return ToLowerA(strIn1) == ToLowerA(strIn2);
}
bool StrUtils::CompareNoCaseW(const wstring& strIn1, const wstring& strIn2)
{
	return ToLowerW(strIn1) == ToLowerW(strIn2);
}

// the total length must be less than 2047
tstring StrUtils::Format(TCHAR* format, ...)
{
	TCHAR mid[2048] = { 0 };
	va_list args;
	va_start(args, format);
	_vsntprintf(mid, 2048 - 1, format, args);
	va_end(args);
	return tstring(mid);
}
string StrUtils::FormatA(char* format, ...)
{
	char mid[2048] = { 0 };
	va_list args;
	va_start(args, format);
	_vsnprintf(mid, 2048 - 1, format, args);
	va_end(args);
	return string(mid);
}
wstring StrUtils::FormatW(WCHAR* format, ...)
{
	WCHAR mid[2048] = { 0 };
	va_list args;
	va_start(args, format);
	_vsnwprintf(mid, 2048 - 1, format, args);
	va_end(args);
	return wstring(mid);
}







inline unsigned char ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55 : x + 48; 
}

inline unsigned char FromHex(unsigned char x) 
{ 
	unsigned char y;
	if( x >= 'A' && x <= 'Z' ) y = x - 'A' + 10;
	else if( x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if( x >= '0' && x <= '9') y = x - '0';
	else assert( 0 );
	return y;
}

string StrUtils::UrlEncoder(string str)
{
	string strTemp = "";
	size_t length = str.length();
	for( size_t i = 0; i < length; i ++ )
	{
		if( (str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') )
			strTemp += str[i];
		else if( str[i] == ' ' ) strTemp += "%20";
		else if( str[i] == '.') strTemp += '.';
		else if( str[i] == '/') strTemp += '/';
		else if( str[i] == '-') strTemp += '-';
		else if( str[i] == ':') strTemp += ':';
		else
		{
			strTemp += '%';
			strTemp += ToHex( (unsigned char)str[i] >> 4 );
			strTemp += ToHex( (unsigned char)str[i] % 16 );
		}
	}
	return strTemp;
}

string StrUtils::UrlDecoder(string str)
{
	string strTemp = "";
	size_t length = str.length();
	for( size_t i = 0; i < length; i ++ )
	{
		if( str[i] == '+') strTemp += ' ';
		else if( str[i] == '%')
		{
			assert( i + 2 < length );
			unsigned char hign = FromHex( (unsigned char)str[++i] );
			unsigned char low = FromHex( (unsigned char)str[++i] );
			strTemp += hign*16 + low;
		} else strTemp += str[i];
	}
	return strTemp;
}