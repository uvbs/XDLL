
#pragma once

/**
 * @file
 * @brief ����guid��less<>�ػ�������GUIDΪkey��map��setʹ��
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


