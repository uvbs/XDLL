
#pragma once

#include <ppl/stl/strings.h>
#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <ppl/stl/stlutils.h>

class ProgramOptions
{
public:
	ProgramOptions()
	{

	}

	void Load(int argc, const TCHAR* const * args)
	{
		m_params.clear();
		m_params.reserve(argc);
		for (int i = 0; i < argc; ++i)
		{
			tstring str(args[i]);
			if (!str.empty())
			{
				m_params.push_back(str);
			}
		}
	}

	void Load(const tstring& arg)
	{
		m_params.clear();
		strings::split(std::back_inserter(m_params), arg, ' ');
	}

	bool HasParam(const tstring& name) const
	{
		return std::find(m_params.begin(), m_params.end(), name) != m_params.end();
	}
	tstring GetParam(const tstring& name, const tstring& defval = tstring()) const
	{
		std::vector<tstring>::const_iterator pos = std::find(m_params.begin(), m_params.end(), name);
		if (pos != m_params.end())
		{
			++pos;
			if (pos != m_params.end())
			{
				return *pos;
			}
		}
		return defval;
	}


private:
	std::vector<tstring> m_params;
};


class StringTable
{
public:
	StringTable()
	{

	}

	void Load(const tstring& s, TCHAR mainDelimiter, TCHAR subDelimiter)
	{
		m_params.clear();
		std::list<string> strs;
		strings::split(std::back_inserter(strs), s, mainDelimiter);
		for ( std::list<string>::const_iterator iter = strs.begin(); iter != strs.end(); ++iter )
		{
			const tstring& itemstr = *iter;
			if ( itemstr.empty() )
				continue;
			std::pair<tstring, tstring> res = strings::split_pair(itemstr, subDelimiter);
			if ( res.first.empty() || res.second.empty() )
				continue;
			m_params[res.first] = res.second;
		}
	}

	bool HasParam(const tstring& name) const
	{
		return containers::contains(m_params, name);
	}
	tstring GetParam(const tstring& name, const tstring& defval = tstring()) const
	{
		std::map<tstring, tstring>::const_iterator iter = m_params.find(name);
		if ( iter == m_params.end() )
			return defval;
		return iter->second;
	}


private:
	std::map<tstring, tstring> m_params;
};


