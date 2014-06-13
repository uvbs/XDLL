
#pragma once

#include <ppl/stl/string.h>
#include <ppl/stl/numeric.h>
#include <vector>
#include <algorithm>


class AppParams
{
public:
	bool Load(int argc, TCHAR* argv[])
	{
		m_params.clear();
		if (argc < 1)
			return false;
		m_params.clear();
		m_params.reserve(argc - 1);
		for (int i = 1; i < argc; ++i)
		{
			m_params.push_back(tstring(argv[i]));
		}
		return true;
	}
	bool HasParam( const tstring& name ) const
	{
		return std::find(m_params.begin(), m_params.end(), name) != m_params.end();
	}
	tstring GetParam( const tstring& name ) const
	{
		tstring result;
		std::vector<tstring>::const_iterator pos = std::find(m_params.begin(), m_params.end(), name);
		if (pos != m_params.end())
		{
			++pos;
			if (pos != m_params.end())
			{
				result = *pos;
			}
		}
		return result;
	}


	template <typename ValueT>
	ValueT GetNumericParam( const tstring& name, ValueT defaultValue ) const
	{
		ValueT val;
		if ( TryGetNumericParam( name, val ) )
			return val;
		return defaultValue;
	}
	template <typename ValueT>
	bool TryGetNumericParam( const tstring& name, ValueT& val ) const
	{
		tstring s = GetParam( name );
		if ( s.empty() )
			return false;
		return numeric<ValueT>::try_parse( s, val );
	}

private:
	std::vector<tstring> m_params;
};


