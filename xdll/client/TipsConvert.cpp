#include "StdAfx.h"
#include "TipsConvert.h"
#include "TipsUtil.h"
#include <vector>
#include <ppl/stl/strings.h>
#include <ppl/util/random.h>

#include <boost/lexical_cast.hpp>

string TipsConvert::UrlConvert(const string& url)
{
	string urlNew = url;
	int nPos;
	string sKey;
	// �����û��[area]
	sKey = "[area]";
	do 
	{
		nPos = urlNew.find(sKey);
		if (nPos == -1) break;
		string sReplace = boost::lexical_cast<string>(TipsUtil::GetAreaCode());
		urlNew.replace(nPos, sKey.size(), sReplace);
	} while(true);
	// �����û��[seconds1970]
	sKey = "[seconds1970]";
	do
	{
		nPos = urlNew.find(sKey);
		if (nPos == -1) break;
		string sReplace = boost::lexical_cast<string>(TipsUtil::GetSeconds1970());
		urlNew.replace(nPos, sKey.size(), sReplace);
	} while(true);

	// �����û��[number=min~max]
	sKey = "[number=";
	do 
	{
		nPos = urlNew.find(sKey);
		if (nPos == -1) break;

		int nEndPos = urlNew.find("]", nPos);
		if (nEndPos == -1)
		{	// ���������⣬ֱ���滻Ϊ��
			urlNew.replace(nPos, sKey.size(), "");
			continue;
		}
		int nLength = nEndPos-nPos+1;
		string sRandom = urlNew.substr(nPos+sKey.size(), nEndPos-nPos-sKey.size());

		vector<string> items;
		strings::split( std::back_inserter(items), sRandom, string("~") );
		if (items.size() < 2)
		{
			urlNew.replace(nPos, nLength, "0");
			continue;
		}
		try {
			int nMin = boost::lexical_cast<int>(items[0]);
			int nMax = boost::lexical_cast<int>(items[1]);
			if (nMin > nMax)
			{	// ��� nMin �� nMax ���򽻻�
				int temp = nMin;
				nMin = nMax;
				nMax = temp;
			}
			int nValue = nMin + RandomGenerator().NextIn(nMax-nMin);
			urlNew.replace(nPos, nLength, boost::lexical_cast<string>(nValue));
		}
		catch (...)
		{
			urlNew.replace(nPos, nLength, "0");
			continue;
		}

	} while (true);

	// �����û��[infiled=aaa|bbb|ccc|ddd|eee]
	sKey = "[infield=";
	do 
	{
		nPos = urlNew.find(sKey);
		if (nPos == -1) break;
		
		int nEndPos = urlNew.find("]", nPos);
		if (nEndPos == -1)
		{	// ���������⣬ֱ���滻Ϊ��
			urlNew.replace(nPos, sKey.size(), "");
			continue;
		}
		int nLength = nEndPos-nPos+1;
		string sRandom = urlNew.substr(nPos+sKey.size(), nEndPos-nPos-sKey.size());
		
		vector<string> items;
		strings::split( std::back_inserter(items), sRandom, string("|") );
		if (items.size() == 2)
		{
			urlNew.replace(nPos, nLength, "");
			continue;
		}
		
		int nIndex = RandomGenerator().NextIn(items.size());
		urlNew.replace(nPos, nLength, items[nIndex]);
	} while (true);

	return urlNew;
}