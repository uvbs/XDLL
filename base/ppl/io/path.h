
#pragma once

/**
 * @file
 * @brief 路径相关操作
 */

#include <ppl/config.h>

#include <ppl/stl/strings.h>
#include <ppl/stl/stream.h>
#include <ppl/util/nonconstructable.h>
#include <ppl/std/tchar.h>

#include <boost/noncopyable.hpp>

#include <utility>
#include <assert.h>


const TCHAR PPL_PATH_BACKSLASH = _T('\\');
const TCHAR PPL_PATH_SLASH = _T('/');

#if defined(_PPL_PLATFORM_MSWIN)

const TCHAR PPL_PATH_SEPERATOR = PPL_PATH_BACKSLASH;

class path_traits
{
public:
	static TCHAR seperator()
	{
		return PPL_PATH_BACKSLASH;
	}
	static TCHAR extension_seperator()
	{
		return _T('.');
	}

	static bool is_seperator(TCHAR ch)
	{
		return PPL_PATH_BACKSLASH == ch || PPL_PATH_SLASH == ch;
	}

	static bool is_absolute(const tstring& path)
	{
		if ( path.empty() )
			return false;
		if ( is_seperator( path[0] ) )
		{
			// 如 \\abc\\d.txt 或者 /abc/d.txt
			return true;
		}
		if ( path.size() < 2 )
			return false;
		if ( _istalpha(path[0]) && ( _T(':') == path[1] ) )
		{
			if ( path.size() < 3 )
				return false;
			// 带驱动器的，例如 c:\\abc\\d.txt为绝对路径 c:abc\\d.txt为相对路径
			return is_seperator( path[2] );
		}
		return false;
	}
};


#elif defined(_PPL_PLATFORM_LINUX)

const TCHAR PPL_PATH_SEPERATOR = PPL_PATH_SLASH;

class path_traits
{
public:
	static TCHAR seperator()
	{
		return PPL_PATH_SLASH;
	}
	static TCHAR extension_seperator()
	{
		return _T('.');
	}

	static bool is_seperator(TCHAR ch)
	{
		return PPL_PATH_SLASH == ch;
	}

	static bool is_absolute(const tstring& path)
	{
		return ( false == path.empty() ) && is_seperator( path[0] );
	}
};

#endif


typedef std::pair<tstring, tstring> tstring_pair;

#pragma warning(push)
#pragma warning(disable:4624)

class paths : private nonconstructable
{
public:
	typedef path_traits traits_type;


	static tstring combine(const tstring& dir, const tstring& filename)
	{
		tstring newDir = add_trailing_seperator(dir);
		return newDir + filename;
	}

	static tstring_pair split_filename(const tstring& path)
	{
		if (path.empty())
		{
			assert(false);
			return std::make_pair( tstring(), tstring() );
		}
		assert( path.size() >= 1 );
		size_t pos = path.size() - 1;
		for ( ;; )
		{
			if ( path_traits::is_seperator( path[pos] ) )
			{
				// 返回的dir不包含末尾的seperator
				return strings::split_at( path, pos + 1 );
			}
			if ( 0 == pos )
			{
				break;
			}
			--pos;
		}
		return std::make_pair( tstring(), path );
	}

	static tstring_pair split_extension(const tstring& path)
	{
		if (path.empty())
		{
			return tstring_pair( tstring(), tstring() );
		}
		assert( path.size() >= 1 );
		size_t pos = path.size() - 1;
		for ( ;; )
		{
			if ( path_traits::extension_seperator() == path[pos] )
			{
				// 返回的ext包含开头的extension seperator
				return strings::split_at( path, pos );
			}
			if ( path_traits::is_seperator( path[pos] ) )
			{
				break;
			}
			if ( 0 == pos )
			{
				break;
			}
			--pos;
		}
		return std::make_pair( path, tstring() );
	}


	/// 获取文件名
	static tstring get_directory(const tstring& path)
	{
		return remove_trailing_seperator( split_filename(path).first );
	}

	/// 获取文件名
	static tstring remove_filename(const tstring& path)
	{
		return split_filename(path).first;
	}

	/// 获取文件名
	static tstring get_filename(const tstring& path)
	{
		return split_filename(path).second;
	}

	/// 去掉文件扩展名
	static tstring remove_extension(const tstring& path)
	{
		return split_extension(path).first;
	}

	/// 获取文件扩展名
	static tstring get_extension(const tstring& path)
	{
		tstring_pair res = split_extension(path);
		return res.second;
	}

	/// 获取文件扩展名
	static tstring get_basename(const tstring& path)
	{
		return remove_extension(get_filename(path));
	}


	static bool has_extension(const tstring& path)
	{
		return false == get_extension(path).empty();
	}


	/// 修改路径
	static tstring replace_directory(const tstring& path, const tstring& newDir)
	{
		tstring filename = get_filename(path);
		return combine(newDir, filename);
	}

	/// 修改文件名
	static tstring replace_filename(const tstring& path, const tstring& newFilename)
	{
		tstring dir = get_directory(path);
		return combine(dir, newFilename);
	}

	/// 修改文件的扩展名
	static tstring replace_extension(const tstring& path, const tstring& newExt)
	{
		tstring pathWithoutExt = remove_extension(path);
		return pathWithoutExt + newExt;
	}

	/// 修改文件名
	static tstring replace_basename(const tstring& path, const tstring& newBasename)
	{
		tstring_pair dirs = split_filename(path);
		tstring_pair exts = split_extension(dirs.second);
		return combine(dirs.first, newBasename + path_traits::extension_seperator() + exts.second);
	}


	static bool is_absolute(const tstring& path)
	{
		return path_traits::is_absolute( path );
	}

	static bool has_trailing_seperator(const tstring& path)
	{
		if (path.empty())
			return false;
		return path_traits::is_seperator( path[path.size() - 1] );
	}

	/// 在末尾增加反斜线
	static tstring add_trailing_seperator(const tstring& path)
	{
		if (has_trailing_seperator(path))
			return path;
		tstring res = path + path_traits::seperator();
		assert( has_trailing_seperator( res ) );
		return res;
	}

	/// 去掉末尾的反斜线
	static tstring remove_trailing_seperator(const tstring& path)
	{
		// 如果不是以sep结尾，直接返回
		if ( false == has_trailing_seperator(path) )
			return path;
		// 如果整个路径只有一个sep，就是根目录了，直接返回
		if ( path.size() == 1 )
			return path;
		assert ( false == path.empty() );
		tstring res = path.substr(0, path.size() - 1);
		assert( false == has_trailing_seperator( res ) );
		return res;
	}

};
#pragma warning(pop)



#ifdef _PPL_RUN_TESTS

#include <ppl/util/test.h>

class PathTestCase : public TestCase
{
public:
	virtual void DoRun()
	{
		cout << paths::split_filename(_T("/")) << std::endl;
		cout << paths::split_extension(_T("abc/c.txt")) << std::endl;
		cout << paths::split_extension(_T("abc/")) << std::endl;
		cout << paths::split_extension(_T("abc\\")) << std::endl;
		cout << paths::get_basename(_T("abc\\cde.txt\\")) << std::endl;
		cout << paths::get_directory(_T("abc\\cde.txt\\")) << std::endl;
		cout << paths::get_filename(_T("abc\\cde.txt\\")) << std::endl;
		cout << paths::get_basename(_T("abc\\cde.txt")) << std::endl;
		cout << paths::get_basename(_T("ab.txt")) << std::endl;

	}
};

#endif




