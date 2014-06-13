
#pragma once


inline ostream& operator<<(ostream& os, const wstring& s)
{
	return os << s.c_str();
}