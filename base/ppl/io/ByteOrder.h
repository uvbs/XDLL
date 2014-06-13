
#pragma once

#include <ppl/std/inttypes.h>


class ByteOrder
{
public:
	static UINT16 reverse_uint16( UINT16 val )
	{
		UINT16 result;
		BYTE* dest = reinterpret_cast<BYTE*>( &result );
		const BYTE* src = reinterpret_cast<const BYTE*>( &val );
		dest[0] = src[1];
		dest[1] = src[0];
		return result;
	}

	static UINT32 reverse_uint32( UINT32 val )
	{
		UINT32 result;
		BYTE* dest = reinterpret_cast<BYTE*>( &result );
		const BYTE* src = reinterpret_cast<const BYTE*>( &val );
		dest[0] = src[3];
		dest[1] = src[2];
		dest[2] = src[1];
		dest[3] = src[0];
		return result;
	}

	static UINT64 reverse_uint64( UINT64 val )
	{
		UINT64 result;
		BYTE* dest = reinterpret_cast<BYTE*>( &result );
		const BYTE* src = reinterpret_cast<const BYTE*>( &val );
		dest[0] = src[7];
		dest[1] = src[6];
		dest[2] = src[5];
		dest[3] = src[4];
		dest[4] = src[3];
		dest[5] = src[2];
		dest[6] = src[1];
		dest[7] = src[0];
		return result;
	}

};