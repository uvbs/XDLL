
#pragma once

#include <assert.h>

/**
 * @file
 * @brief com相关的函数和类
 */


/// 负责自动初始化COM库
class ComInitializer
{
public:
	ComInitializer()
	{
		HRESULT hr = ::CoInitialize(NULL);
		assert(hr == S_OK);
	}
	~ComInitializer()
	{
		::CoUninitialize();
	}
};

