#include "stdafx.h"
#include "strings.h"

vector<string> splite(const string& input, const string& spliter)
{
	vector<string> ret_strs;;
	string s = input;
	size_t spliter_length = spliter.size();
	while(true)
	{
		size_t pos = s.find(spliter);
		if( pos == string::npos )
		{
			ret_strs.push_back(s);
			break;
		}
		ret_strs.push_back(s.substr(0, pos));
		pos += spliter_length;
		s = s.substr(pos);
	}
	return ret_strs;
}