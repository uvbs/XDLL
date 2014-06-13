#pragma once

extern IPTable g_ipTable;

class Address
{
public:
	static void LoadFromLine(const string& line);
};