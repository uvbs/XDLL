
#pragma once

#include <ppl/config.h>

#if defined(_PPL_PLATFORM_LINUX) || defined(_PPL_USE_ASIO)

#include <ppl/util/detail/IniFile.h>

#include <ppl/stl/strings.h>
#include <ppl/stl/numeric.h>
#include <ppl/io/fs.h>

class IniFile : private noncopyable
{
public:

	IniFile()
	{
	}
	~IniFile()
	{
	}


	void SetFileName(const string& filename)
	{
		m_filename = filename;
		if ( false == file_system::file_exists( m_filename.c_str() ) )
		{
			CIniFile::Create(m_filename.c_str());
		}
	}

	void SetSection(const string& section)
	{
		m_section = section;
	}

	string GetString(const string& key, LPCTSTR defaultValue)
	{
		CheckFileName();
		string s = CIniFile::GetValue(key, m_section, defaultValue, m_filename);
		return s;
	}
	bool SetString(const string& key, const tstring& value)
	{
		CheckFileName();
		return CIniFile::SetValue(key, value, m_section, m_filename);
	}




	/************************************************************************/
	/* integer                                                              */
	/************************************************************************/
	UINT GetInteger(const string& key, int defaultValue)
	{
		return this->GetInt(key, defaultValue);
	}
	UINT GetInt(const string& key, int defaultValue)
	{
		string s = this->GetString(key, "");
		if ( s.empty() )
			return defaultValue;
		return numeric<UINT>::parse(s, defaultValue);
	}
	bool SetInteger(const string& key, int value)
	{
		return this->SetInt(key, value);
	}
	bool SetInt(const string& key, int value)
	{
		string s = strings::format("%d", value);
		return this->SetString(key, s);
	}
	bool SetUnsignedInt(const string& key, unsigned int value)
	{
		string s = strings::format("%u", value);
		return this->SetString(key, s);
	}


	/************************************************************************/
	/* bool                                                                 */
	/************************************************************************/
	bool GetBool(const string& key, bool defaultValue)
	{
		return 0 != this->GetInt(key, defaultValue);
	}
	bool SetBool(const string& key, bool value)
	{
		return this->SetInt(key, value);
	}
	BOOL GetBOOL(const string& key, BOOL defaultValue)
	{
		UINT res = GetInt(key, defaultValue);
		return (0 != res) ? TRUE : FALSE;
	}
	bool SetBOOL(const string& key, BOOL value)
	{
		return this->SetInt(key, value);
	}


protected:
	/// 获得ini文件名字符串
	LPCTSTR GetFileName() const
	{
		return m_filename.c_str();
	}
	LPCTSTR GetSection() const
	{
		return m_section.c_str();
	}
	void CheckSection()
	{
		assert(!m_section.empty());
	}
	void CheckFileName()
	{
		assert(!m_filename.empty());
	}


private:
	string m_filename;
	string m_section;
};

#elif defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/ini.h>

#endif

