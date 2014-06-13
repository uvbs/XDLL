#include "stdafx.h"
#include "Address.h"
#include "strings.h"

#include <boost/algorithm/string.hpp>



void Address::LoadFromLine(const string& line)
{
	vector<string> lines = splite( line,"|");
	for(vector<string>::iterator iter=lines.begin(); iter!=lines.end(); iter++)
	{
		string address_string=*iter;
		vector<string> strs=splite( line,":");
		if(strs.size() != 3)
			continue;
		try
		{
			u_short controy = boost::lexical_cast<u_short>(strs[0]);
			u_short province = boost::lexical_cast<u_short>(strs[1]);
			u_short city = boost::lexical_cast<u_short>(strs[2]);
		}
		catch (...)
		{
			continue;
		}
	}
}