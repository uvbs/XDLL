
#pragma once

/**
 * @file
 * @brief 包含guid的less<>特化，供以GUID为key的map和set使用
 */

#include <ppl/config.h>
#include <ppl/util/GuidLess.h>

#include <xhash>

namespace stdext
{

inline size_t hash_value(const GUID& val)
{
	unsigned long a = val.Data1;
	unsigned short b = val.Data2, c = val.Data3;
	unsigned char f = val.Data4[2], k = val.Data4[7];
	__asm
	{
		movzx eax, b
			shl eax, 10h
			or ax, c
			xor eax, a
			movzx edx, f
			shl edx, 18h
			or cl, k
			xor eax, edx
	}
	//Counting ((a ^ (b << 0x10) | c) ^ ((f << 0x18) | k))
	// return value is in eax now, so no return here
}

}


//#if defined(_PPL_PLATFORM_MSWIN)
//
//namespace stdext
//{
//	class GUIDHash : public hash_compare<GUID>
//	{
//	public:
//		bool operator()(const GUID &val1, const GUID &val2) const
//		{
//			return memcmp(&val1, &val2, sizeof(GUID)) != 0;
//		}
//
//		size_t operator()(const GUID &val) const
//		{
//			unsigned long a = val.Data1;
//			unsigned short b = val.Data2, c = val.Data3;
//			unsigned char f = val.Data4[2], k = val.Data4[7];
//			__asm
//			{
//				movzx eax, b
//					shl eax, 10h
//					or ax, c
//					xor eax, a
//					movzx edx, f
//					shl edx, 18h
//					or cl, k
//					xor eax, edx
//			}	//Counting ((a ^ (b << 0x10) | c) ^ ((f << 0x18) | k))
//			// return value is in eax now, so no return here
//		}
//	};
//}
//
//#elif defined(_PPL_PLATFORM_LINUX)
//
//namespace __gnu_cxx
//{
//	template<>
//	struct hash<GUID>
//	{
//	public:
////		bool operator()(const GUID &val1, const GUID &val2) const
////		{
////			return memcmp(&val1, &val2, sizeof(GUID)) != 0;
////		}
//
//		size_t operator()(const GUID &val) const
//		{
//			unsigned long a = val.Data1;
//			unsigned short b = val.Data2, c = val.Data3;
//			unsigned char f = val.Data4[2], k = val.Data4[7];
///*			__asm
//			{
//				movzx eax, b
//					shl eax, 10h
//					or ax, c
//					xor eax, a
//					movzx edx, f
//					shl edx, 18h
//					or cl, k
//					xor eax, edx
//			}	//Counting ((a ^ (b << 0x10) | c) ^ ((f << 0x18) | k))
//			// return value is in eax now, so no return here
//*/
//			return ((a ^ (b << 0x10) | c) ^ ((f << 0x18) | k));
//		}
//	};
//
//	template <typename T>
//	struct hash<T*>
//	{
//		size_t operator()(T* __x) const { return reinterpret_cast<size_t>( __x ); }
//	};
//
//	typedef hash<GUID> GUIDHash;
//}
//
//#endif
//
