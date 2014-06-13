
#pragma once

#include <boost/noncopyable.hpp>

/// http应答信息分析器
class HTTPInformation : private boost::noncopyable
{
public:
	explicit HTTPInformation(const char* data, size_t size) : m_data(data), m_size(size)
	{
		assert(m_data != NULL && m_size > 0);
	}

	/// 找到内容部分的位置
	int FindBody() const;

	/// 提取内容数据
	string ExtractBody() const;

	string ReadLine() const;


private:
	/// http应答信息的缓冲区
	const char* m_data;

	/// http应答信息的长度
	size_t m_size;
};


class HTTPResponse : public HTTPInformation
{
public:
	explicit HTTPResponse(const char* data, size_t size) : HTTPInformation(data, size)
	{

	}
};

class HTTPRequest : public HTTPInformation
{
public:
	explicit HTTPRequest(const char* data, size_t size) : HTTPInformation(data, size)
	{

	}

	string GetRequestUrl() const;
};




inline int HTTPInformation::FindBody() const
{
	const char* crlf2 = "\r\n\r\n";
	const size_t crlf2_size = 4;
	const char* pos = std::search(m_data, m_data + m_size, crlf2, crlf2 + crlf2_size);
	if (pos == m_data + m_size)
		return -1;
	return pos - m_data + crlf2_size;
}

inline string HTTPInformation::ExtractBody() const
{
	int pos = FindBody();
	if (pos < 0)
		return "";
	assert( static_cast<size_t>( pos ) <= m_size );
	return string(m_data + pos, m_data + m_size);
}

inline string HTTPInformation::ReadLine() const
{
	const char* crlf = "\r\n";
	const size_t crlf_size = 2;
	const char* pos = std::search(m_data, m_data + m_size, crlf, crlf + crlf_size);
	if (pos == m_data + m_size)
		return string();
	pos += crlf_size;
	return string(m_data, pos);

}

inline string HTTPRequest::GetRequestUrl() const
{
	string firstLine = this->ReadLine();
	if ( firstLine.empty() )
		return string();
	const char* tag1 = "GET ";
	const size_t tag1_size = 4;
	const char* tag2 = " HTTP/";
	size_t pos = firstLine.find( tag1 );
	if ( pos == string::npos )
		return string();
	pos += tag1_size;
	size_t pos2 = firstLine.find( tag2, pos );
	if ( pos2 == string::npos )
		return string();
	return firstLine.substr( pos, pos2 - pos );
}

