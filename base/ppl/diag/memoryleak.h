
#pragma once

/**
 * @file
 * @brief �ڴ�й©�ļ��
 * �÷���(��stdafx.h��)�������ļ���Ȼ���ڳ���Ŀ�ʼ������PPL_CHECK_MEMORY_LEAKE()
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

// ͨ���꽫new���¶��壬��Ҫ�Ȱ���һЩʹ�õ���operator new��in-place new��ͷ�ļ�
// �ŵ����ܹ���ȡ��׼ȷ��__FILE__��__LINE__��Ϣ��ȷ���Ƕ�һЩ�ǳ��淽ʽʹ��new������operator new�Ĵ�����Ӱ��

#    include <crtdbg.h>

#    include <new>
#    include <memory>
#    include <fstream>
#    include <xtree>

#    define PPL_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new PPL_DEBUG_NEW
#    pragma message("------redefine new as macro")

#  else

// ���ͨ��_CRTDBG_MAP_ALLOC�������ڴ�й©��飬��Ҫ�Ȱ���stdlib.h��malloc.h�Ȱ���malloc/calloc/free�Ⱥ���������ͷ�ļ�
// �ŵ��ǳ���malloc/calloc/free�ȱ�����Ϊ��֮�⣬�������Ĵ���û��ʲôӰ�죻ȱ����__FILE__��__LINE__��Ϣ��׼ȷ

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

