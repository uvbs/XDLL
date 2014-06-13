
#pragma once

/**
 * @file
 * @brief 包含guid的less<>特化，供以GUID为key的map和set使用
 */


#include <ppl/util/guid.h>



namespace std {

template<>
struct less<GUID> : public binary_function<GUID, GUID, bool>
{
	bool operator()(const GUID& x, const GUID& y) const
	{
		return memcmp(&x, &y, sizeof(GUID)) < 0;
	}
};

}


