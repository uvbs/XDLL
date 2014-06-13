
#pragma once

#include <assert.h>

/**
 * @file
 * @brief com��صĺ�������
 */


/// �����Զ���ʼ��COM��
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

