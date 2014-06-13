#include "StdAfx.h"
#include "HopInfo.h"
#include <ppl/stl/strings.h>
#include <ppl/stl/numeric.h>

bool MultiHopUrlInfo::ParseHopInfo(const string& s)
{
	AllClickRates.clear();
	if ( s.empty() )
		return true;
	std::list<string> items;
	strings::split( std::back_inserter( items ), s, '|' );
	STL_FOR_EACH_CONST( std::list<string>, items, iter )
	{
		int val = 0;
		if ( false == numeric<int>::try_parse( *iter, val ) )
		{
			assert(false);
			return false;
		}
		AllClickRates.push_back( val );
	}
	return true;
}

bool MultiHopUrlInfo::ParseRuleInfo( const string& s )
{
	Patterns.clear();
	if ( s.empty() )
		return true;
	std::list<string> items;
	strings::split( std::back_inserter( items ), s, string("!!!!") );
	STL_FOR_EACH_CONST( std::list<string>, items, iter )
	{
		const string& item = *iter;
		
		std::vector<string> elems;
		strings::split( std::back_inserter( elems ), item, string("$$") );
		
		if( elems.size() == 0 )
			continue;
		
		TaskPatterns p;
		assert( elems.size() >= 1);
		p.begin = CComBSTR( elems[0].c_str() );
		if( elems.size() >= 2)
			p.mid = CComBSTR( elems[1].c_str() );
		if( elems.size() >= 3)
			p.url = CComBSTR( elems[2].c_str() );	
		if( elems.size() >= 4)
			p.url_only = CComBSTR( elems[3].c_str() );
		if( elems.size() >= 5)
			p.url_forbidden = CComBSTR( elems[4].c_str() );
		
		Patterns.push_back( p );
	}
	return true;
}

bool MultiHopUrlInfo::ParseRemoveCookies(const string& s)
{
	RemoveCookiesList.clear();
	if (s.empty() )
		return true;
	strings::split( std::back_inserter(RemoveCookiesList), s, string("!!!!") );
	return true;
}

bool MultiHopUrlInfo::ParseFlashRemoveCookies(const string& s)
{
	RemoveFlashCookiesList.clear();
	if (s.empty() )
		return true;
	strings::split( std::back_inserter(RemoveFlashCookiesList), s, string("!!!!") );
	return true;
}

bool MultiHopUrlInfo::ParseDirectionals(const string& s)
{
	DirectionalsArray.clear();
	if (s.empty() )
		return true;
	strings::split( std::back_inserter(DirectionalsArray), s, string("!!!!") );
	STL_FOR_EACH(std::vector<string>, DirectionalsArray, iter)
	{
		if (*iter == "*") *iter = "";
	}
	return true;
}

bool MultiHopUrlInfo::ParseUrlFilter(const string& s)
{
	UrlFilterList.clear();
	if (s.empty())
		return true;

	USES_CONVERSION;
	wstring ws = A2W(s.c_str());
	strings::split( std::back_inserter(UrlFilterList), ws, wstring(L"|"));
	return true;
}