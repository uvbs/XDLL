
#pragma once

//#include <ppl/io/stdfile.h>
#include <ppl/io/file.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>


class hashing : private boost::noncopyable
{
public:
	template <typename HasherT>
	static bool hash_file_with( const TCHAR* filename, BYTE* outbuf, BYTE* tempbuf, size_t blockSize )
	{
#if 0
		StdFileReader fin;
		if ( false == fin.OpenBinary( filename, _SH_DENYWR ) )
#else
		File fin;
		if ( false == fin.OpenRead( filename, FILE_SHARE_READ ) )
#endif
		{
//			assert(false);
			return false;
		}
		HasherT hasher;
		hasher.Init();
		for ( ;; )
		{
			size_t len = fin.Read( tempbuf, blockSize );
			if ( 0 == len )
			{
				// ∂¡ÕÍ¡À
				break;
			}
			hasher.AddData( tempbuf, len );
		}
		hasher.GetResult(outbuf);
		return true;
	}

	template <typename HasherT>
	static bool hash_file( const TCHAR* filename, BYTE* outbuf, size_t blockSize )
	{
		assert( blockSize > 0 );
		boost::scoped_array<BYTE> buf( new BYTE[blockSize] );
		return hash_file_with<HasherT>( filename, outbuf, buf.get(), blockSize );
	}
};

