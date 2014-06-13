
#pragma once


#include <ppl/io/stdfile.h>
#include <ppl/mswin/module.h>
#include <ppl/io/path.h>


class Logger : private noncopyable
{
public:
	Logger()
	{
		Path path;
		Module().BuildLocalMainFilePath(path, TEXT(".log"));
		bool res = Open(path);
	}
	virtual ~Logger()
	{
	}

	bool Open(const TCHAR* filename)
	{
		return m_file.OpenBinary(filename, _SH_DENYWR);
	}
	void log(const char* str)
	{
		if (!m_file.IsOpen())
			return;
		this->LogHeader();
		m_file.Write(str);
		m_file.Write("\r\n");
		puts(str);
		m_file.Flush();
	}
	void logf(const char* format, ...)
	{
		if (!m_file.IsOpen())
			return;
		this->LogHeader();
		va_list args;
		va_start(args, format);
		m_file.WriteV(format, args);
		m_file.Write("\r\n");
		vprintf(format, args);
		va_end(args);
		m_file.Flush();
	}

	static Logger& Instance()
	{
		static Logger logger;
		return logger;
	}

protected:
	void LogHeader()
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		m_file.WriteF("%04d-%02d-%02d %02d:%02d:%02d.%03d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}

private:
	StdFileWriter m_file;
};


#ifndef _PPL_DISABLE_FILE_LOG_IMPL

inline void log_impl(unsigned long type, unsigned long level, const char* cText)
{
	Logger::Instance().log(cText);
}

#endif



