
#pragma once

#include <ppl/text/base64.h>

class SynacastUrlEncoding
{
public:
	explicit SynacastUrlEncoding( const string& key ) : m_key( key )
	{
		assert(false == m_key.empty() );
	}

	string Encode(const string& url)
	{
		Base64Encoding::GetDelimiter() = "";
		const size_t PPL_KEY_LENGTH = m_key.size();
		string result;
		result.resize(url.size());
		for (size_t i = 0; i < url.size(); ++i)
		{
			size_t keyIndex = i % PPL_KEY_LENGTH;
			result[i] = url[i] + m_key[keyIndex];
		}
		string encodedResult = Base64Encoding::Encode(result);
		assert( encodedResult.size() == CalcEncodedLength( result.size() ) );
		return encodedResult;
	}

	string Decode(const string& url)
	{
		const size_t PPL_KEY_LENGTH = m_key.size();

		string url2 = Base64Encoding::Decode(url);
#ifdef _DEBUG
		Base64Encoding::GetDelimiter() = "";
		string url3 = Base64Encoding::Encode(url2);
		assert(url3 == url);
#endif
		string result;
		result.resize(url2.size());
		for (size_t i = 0; i < url2.size(); ++i)
		{
			size_t keyIndex = i % PPL_KEY_LENGTH;
			result[i] = url2[i] - m_key[keyIndex];
		}
		return result;
	}

	size_t CalcEncodedLength( size_t size )
	{
		if ( 0 == size )
			return 0;
		return ( size + 2 ) / 3 * 4;
	}

private:
	string m_key;
};
