
#pragma once

#include <ppl/std/inttypes.h>
#include <ppl/util/macro.h>

#include <boost/noncopyable.hpp>


class FileFinder : private boost::noncopyable
{
public:
//	FileFinder() : m_handle(INVALID_HANDLE_VALUE), m_hasMore(false)
//	{
//	}
	explicit FileFinder(LPCTSTR filename) : m_handle(INVALID_HANDLE_VALUE), m_hasMore(false)
	{
		FILL_ZERO( m_item );
		this->Find(filename);
	}
	~FileFinder()
	{
		Close();
	}

	void Close()
	{
		if (IsValid())
		{
			::FindClose(m_handle);
			m_handle = INVALID_HANDLE_VALUE;
			m_hasMore = false;
		}
	}
	bool IsValid() const
	{
		return m_handle != INVALID_HANDLE_VALUE;
	}

	bool Find(LPCTSTR filename)
	{
		Close();
		m_handle = ::FindFirstFile(filename, &m_item);
		if (!IsValid())
			return false;
		m_hasMore = true;
		return true;
	}

	bool HasMore() const
	{
		return m_hasMore;
	}
	void MoveNext()
	{
		assert(HasMore());
		m_hasMore = (FALSE != ::FindNextFile(m_handle, &m_item));
	}
	const WIN32_FIND_DATA& Current() const
	{
		assert( m_hasMore );
		return m_item;
	}

	bool IsDirectory() const
	{
		return this->MatchFileAttribute( FILE_ATTRIBUTE_DIRECTORY );
	}
	bool IsNormal() const
	{
		return this->MatchFileAttribute( FILE_ATTRIBUTE_NORMAL );
	}

	bool MatchFileAttribute( DWORD attr ) const
	{
		return !!( this->Current().dwFileAttributes & attr );
	}

	UINT64 GetFileSize() const
	{
		return make_uint64( this->Current().nFileSizeHigh, this->Current().nFileSizeLow );
	}

	const TCHAR* GetFileName() const
	{
		return this->Current().cFileName;
	}


	static bool Exists( const TCHAR* path )
	{
		FileFinder finder( path );
		return finder.HasMore();
	}

	static bool DirectoryExists( const TCHAR* path )
	{
		FileFinder finder( path );
		if ( false == finder.HasMore() )
			return false;
		DWORD attr = finder.Current().dwFileAttributes;
		return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	static bool FileExists( const TCHAR* path )
	{
		FileFinder finder( path );
		if ( false == finder.HasMore() )
			return false;
		DWORD attr = finder.Current().dwFileAttributes;
		return (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}


private:
	HANDLE	m_handle;
	WIN32_FIND_DATA	m_item;
	bool	m_hasMore;
};

