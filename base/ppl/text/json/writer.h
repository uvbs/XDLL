
#pragma once

#include <boost/noncopyable.hpp>
#include <ostream>

const char PPL_JSON_QUOTE = '\"';
const char PPL_JSON_COLON = ':';

class JsonWriter : private boost::noncopyable
{
public:
	explicit JsonWriter(ostream& os) : m_out(os)
	{

	}

	ostream& GetStream() { return m_out; }
	
	void WriteString(const string& name)
	{
		m_out << PPL_JSON_QUOTE << name << PPL_JSON_QUOTE;
	}
	void WriteComma()
	{
		m_out << ',';
	}
	void WriteColon()
	{
		m_out << PPL_JSON_COLON;
	}

	void WriteJsonString(const string& name, const string& val)
	{
		WriteString(name);
		m_out << PPL_JSON_COLON;
		WriteString(val);
	}

	void WriteJsonNumber(const string& name, const string& val)
	{
		WriteString(name);
		m_out << PPL_JSON_COLON;
		m_out << val;
	}

	void BeginWriteJsonObject()
	{
		m_out << '{';
	}
	void EndWriteJsonObject()
	{
		m_out << '}';
	}

	class WriterEntry : private boost::noncopyable
	{
	public:
		explicit WriterEntry(ostream& os, const string& beginTag, const string& endTag) : m_out(os), m_begin(beginTag), m_end(endTag)
		{
			m_out << m_begin;
		}
		~WriterEntry()
		{
			m_out << m_end;
		}
	private:
		ostream& m_out;
		string m_begin;
		string m_end;
	};
	class ObjectWriterEntry : public WriterEntry
	{
	public:
		explicit ObjectWriterEntry(ostream& os) : WriterEntry(os, "{", "}") { }
	};
	class ArrayWriterEntry : public WriterEntry
	{
	public:
		explicit ArrayWriterEntry(ostream& os) : WriterEntry(os, "[", "]") { }
	};

private:
	ostream& m_out;
	friend class ObjectWriter;
	friend class ArrayWriter;
};



