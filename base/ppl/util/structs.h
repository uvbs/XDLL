
#pragma once

#include "./StaticAssert.h"



/// 带填料的结构体，PaddingT为填料类型，_PADDING_SIZE为填料数量
template <typename StructT, typename PaddingT, size_t _PADDING_SIZE>
class PaddedStruct
{
public:

	typedef StructT fixed_strcut;
	typedef PaddingT padding_type;
	enum { PADDING_SIZE = _PADDING_SIZE };

#pragma pack(push, 1)
	/// 包含变体部分的完整结构体
	struct struct_type : StructT
	{
		/// 填料，用于处理变体数据
		PaddingT Padding[_PADDING_SIZE];
	};
#pragma pack(pop)

//	BOOST_STATIC_ASSERT(sizeof(struct_type) == sizeof(StructT) + sizeof(PaddingT) * _PADDING_SIZE);

	/// 实际填料后的结构体定义
	typedef struct_type type;
};



