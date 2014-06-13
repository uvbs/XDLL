
#pragma once





inline CString FormatString(LPCTSTR format, ...)
{
	va_list args;
	va_start(args, format);
	CString str;
	str.FormatV(format, args);
	va_end(args);
	return str;
}


template <typename T>
inline CString FormatInteger(const T& val, LPCTSTR format = TEXT("%d"))
{
	return FormatString(format, val);
}

template <typename T>
inline CString FormatFloat(const T& val, LPCTSTR format = TEXT("%0.2f"))
{
	return FormatString(format, val);
}

template <typename T>
inline CString FormatFloatPercent(const T& val, LPCTSTR format = TEXT("%0.2f%%"))
{
	return FormatString(format, val);
}

template <typename T>
inline CString FormatUnsigned(const T& val, LPCTSTR format = TEXT("%u"))
{
	return FormatString(format, val);
}


template <typename T>
inline CString FormatPointer(const T& p, LPCTSTR format = TEXT("%p"))
{
	return FormatString(format, p);
}

template <typename T>
inline CString FormatHex(const T& p, LPCTSTR format = TEXT("%X"))
{
	return FormatString(format, p);
}

template <typename T>
inline CString FormatObject(const T& val, LPCTSTR format = TEXT("%s"))
{
	std::ostringstream os;
	os << val;
	CString str;
	USES_CONVERSION;
	str.Format(format, A2CT(os.str().c_str()));
	return str;
}


inline CString FormatUINT64(UINT64 val)
{
	return FormatString("%I64u", val);
}


/// 将dword格式化为x.x.x.x的字符串
inline CString FormatDotDWORD(DWORD val)
{
	WORD val1 = HIWORD(val);
	WORD val2 = LOWORD(val);
	return FormatString(TEXT("%u.%u.%u.%u"), HIBYTE(val1), LOBYTE(val1), HIBYTE(val2), LOBYTE(val2));
}



inline CString FormatTime(const SYSTEMTIME& systime)
{
	return FormatString(
			TEXT("%04hu-%02hu-%02hu %02hu:%02hu:%02hu.%03hu"), 
			systime.wYear, systime.wMonth, systime.wDay, 
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
}

inline CString GetLocalTimeString()
{
	SYSTEMTIME systime;
	::GetLocalTime(&systime);
	return FormatTime(systime);
}


