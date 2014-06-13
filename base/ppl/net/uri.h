
#pragma once

#include <map>
using std::map;

#include <boost/tuple/tuple.hpp>

#include <ppl/stl/strings.h>


namespace ppl {

typedef map<tstring, tstring> tstring_table;


class dictionary_output_iterator
{
public:
	explicit dictionary_output_iterator(tstring_table& params, TCHAR delimiter) : m_params(&params), m_delimiter(delimiter) { }

	dictionary_output_iterator& operator=(const tstring& queryItem)
	{
		tstring key, value;
		boost::tie(key, value) = strings::split_pair(queryItem, m_delimiter);
		(*m_params)[strings::lower(key)] = value;
		return (*this);
	}
	dictionary_output_iterator& operator*() { return (*this); }
	dictionary_output_iterator& operator++() { return (*this); }
	dictionary_output_iterator operator++(int) { return (*this); }
protected:
	tstring_table* m_params;
	TCHAR m_delimiter;
};


inline void parse_dictionary(const tstring& src, tstring_table& result, TCHAR primeDelimiter, TCHAR innerDelimiter)
{
	result.clear();
	dictionary_output_iterator outputer(result, innerDelimiter);
	strings::split(outputer, src, primeDelimiter);
}

inline void parse_uri_params(const tstring& src, tstring_table& result)
{
	parse_dictionary(src, result, TEXT('&'), TEXT('='));
}


class Uri
{
public:
	Uri()
	{
	}
	explicit Uri(const tstring& urlstr)
	{
		Parse(urlstr);
	}
	Uri(const tstring& scheme, const tstring& body) : m_scheme(scheme)
	{
		boost::tie(m_path, m_query) = strings::split_pair(body, TEXT('?'));
		ParseQuery();
	}
	Uri(const tstring& scheme, const tstring& path, const tstring& query) : m_scheme(scheme), m_path(path), m_query(query)
	{
		ParseQuery();
	}

	bool Parse(const tstring& urlstr)
	{
		tstring body;
		boost::tie(m_scheme, body) = strings::split_pair(urlstr, TEXT("://"));
		if (body.empty())
		{
			body = m_scheme;
			m_scheme.erase();
		}
		if (body.empty())
			return false;
		boost::tie(m_path, m_query) = strings::split_pair(body, TEXT('?'));
		ParseQuery();
		return true;
	}

	tstring GetParam(const tstring& name)
	{
		tstring result;
		tstring paramName = strings::lower(name);
		tstring_table::const_iterator iter = m_params.find(paramName);
		if (iter != m_params.end())
		{
			result = iter->second;
		}
		return result;
	}

	tstring GetScheme() const { return m_scheme; }
	tstring GetPath() const { return m_path; }
	tstring GetQuery() const { return m_query; }

protected:
	void ParseQuery()
	{
		Revise();
		parse_uri_params(m_query, m_params);
	}

	void Revise()
	{
		strings::make_lower(m_scheme);
		strings::make_lower(m_path);
		strings::make_lower(m_query);
	}

private:
	//tstring m_urlstr;
	//tstring m_body;

	tstring m_scheme;
	tstring m_path;
	tstring m_query;
	tstring_table m_params;
};

}


