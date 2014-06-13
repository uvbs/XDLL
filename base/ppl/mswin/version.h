
#pragma once


#pragma comment(lib, "version.lib")

#include <ppl/util/macro.h>
#include <ppl/stl/stream.h>


/// 文件的版本信息
class FileVersionInfo
{
public:
	FileVersionInfo();
	explicit FileVersionInfo(LPCTSTR filename);

	/// 获取指定文件的版本信息
	bool Retrieve(LPCTSTR filename);

	/// 获取基本信息
	const VS_FIXEDFILEINFO& GetFixedInfo() const
	{
		assert(!m_data.empty());
		return m_fixedInfo;
	}

private:
	/// 版本信息的原始数据
	tstring m_data;

	/// 基本的版本信息
	VS_FIXEDFILEINFO m_fixedInfo;
};



inline FileVersionInfo::FileVersionInfo()
{
	FILL_ZERO(m_fixedInfo);
}

inline FileVersionInfo::FileVersionInfo(LPCTSTR filename)
{
	Retrieve(filename);
}

inline bool FileVersionInfo::Retrieve(LPCTSTR filename)
{
	FILL_ZERO(m_fixedInfo);
	LPTSTR filepath = const_cast<LPTSTR>(filename);
	m_data.reserve(8 * 1024);
	DWORD dummyHandle = 0;
	DWORD size = ::GetFileVersionInfoSize(filepath, &dummyHandle);
	if (size <= 0)
	{
		UTIL_ERROR("FileVersionInfo::Retrieve: GetFileVersionInfoSize failed " << make_tuple(::GetLastError(), dummyHandle));
		return false;
	}
	m_data.resize(size);
	if (!::GetFileVersionInfo(filepath, 0, size, &m_data[0]))
	{
		UTIL_ERROR("FileVersionInfo::Retrieve: GetFileVersionInfo failed " << make_tuple(::GetLastError(), dummyHandle));
		return false;
	}
	void* buffer = NULL;
	UINT bufferLength = 0;
	const TCHAR* data = m_data.data();
	if (!::VerQueryValue(const_cast<TCHAR*>(data), TEXT("\\"), &buffer, &bufferLength))
	{
		UTIL_ERROR("FileVersionInfo::Retrieve: VerQueryValue failed " << make_tuple(::GetLastError(), dummyHandle));
		return false;
	}
	if (bufferLength < sizeof(VS_FIXEDFILEINFO))
	{
		UTIL_ERROR("FileVersionInfo::Retrieve: VerQueryValue failed buffer length error " << bufferLength);
		return false;
	}
	const VS_FIXEDFILEINFO* fixedInfo = static_cast<const VS_FIXEDFILEINFO*>(buffer);
	m_fixedInfo = *fixedInfo;
	return true;
}

