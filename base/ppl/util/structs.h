
#pragma once

#include "./StaticAssert.h"



/// �����ϵĽṹ�壬PaddingTΪ�������ͣ�_PADDING_SIZEΪ��������
template <typename StructT, typename PaddingT, size_t _PADDING_SIZE>
class PaddedStruct
{
public:

	typedef StructT fixed_strcut;
	typedef PaddingT padding_type;
	enum { PADDING_SIZE = _PADDING_SIZE };

#pragma pack(push, 1)
	/// �������岿�ֵ������ṹ��
	struct struct_type : StructT
	{
		/// ���ϣ����ڴ����������
		PaddingT Padding[_PADDING_SIZE];
	};
#pragma pack(pop)

//	BOOST_STATIC_ASSERT(sizeof(struct_type) == sizeof(StructT) + sizeof(PaddingT) * _PADDING_SIZE);

	/// ʵ�����Ϻ�Ľṹ�嶨��
	typedef struct_type type;
};



