
#pragma once

/**
 * @file
 * @brief 内存泄漏的检查
 * 用法：(在stdafx.h中)包含此文件，然后在程序的开始处调用PPL_CHECK_MEMORY_LEAKE()
 */



#if defined(_DEBUG)

#pragma message("------ crt memory checking utilities")

#include <crtdbg.h>

class CrtDbgFlag
{
public:
	static void Set(int newFlag)
	{
		_CrtSetDbgFlag(newFlag);
	}
	static int Get()
	{
		return _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	}
	static void Enable(int flagBit)
	{
		Set(Get() | flagBit);
	}
	static void Disable(int flagBit)
	{
		Set(Get() & (~flagBit));
	}
	static void EnableLeakCheck()
	{
		Enable(_CRTDBG_LEAK_CHECK_DF);
	};
	static void EnableAlwaysCheck()
	{
		Enable(_CRTDBG_CHECK_ALWAYS_DF);
	}
};

#endif



#if defined(_DEBUG) && !defined(_PPL_DISABLE_MEMORY_LEAK_CHECKING)

#  pragma message("------enable memory leak checking")
#  define PPL_MEMORY_LEAK_CHECKING_ENABLED

#  if defined(_PPL_ENABLE_REDEFINE_NEW)

// 通过宏将new重新定义，需要先包含一些使用到了operator new和in-place new的头文件
// 优点是能够获取较准确的__FILE__和__LINE__信息，确定是对一些非常规方式使用new和重载operator new的代码有影响

#    include <crtdbg.h>

#    include <new>
#    include <memory>
#    include <fstream>
#    include <xtree>

#    define PPL_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new PPL_DEBUG_NEW
#    pragma message("------redefine new as macro")

#  else

// 如果通过_CRTDBG_MAP_ALLOC来启用内存泄漏检查，需要先包含stdlib.h和malloc.h等包含malloc/calloc/free等函数声明的头文件
// 优点是除了malloc/calloc/free等被定义为宏之外，对其它的代码没有什么影响；缺点是__FILE__和__LINE__信息不准确

#    include <stdlib.h>
#    include <malloc.h>

#    define _CRTDBG_MAP_ALLOC
#    include <crtdbg.h>

#  endif

#define PPL_CHECK_MEMORY_LEAK()		do { CrtDbgFlag::EnableLeakCheck(); } while (0)

#else
#define PPL_CHECK_MEMORY_LEAK()
#endif

#define PPL_CHECK_MEMORY_LEAKE() PPL_CHECK_MEMORY_LEAK()

class MemoryLeakChecker
{
public:
	MemoryLeakChecker()
	{
		PPL_CHECK_MEMORY_LEAK();
	}
private:
	MemoryLeakChecker(const MemoryLeakChecker&);
	void operator=(const MemoryLeakChecker&);
};

