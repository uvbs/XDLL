
#pragma once


/**
 * @file
 * @brief 随机数生成器
 */

#include <ppl/config.h>

#include <time.h>
#include <ppl/stl/string.h>
#include <ppl/diag/trace.h>
#include <ppl/util/log.h>
#include <ppl/util/macro.h>
#include <ppl/util/TimeCounter.h>
#include <ppl/diag/trace.h>

#include <boost/noncopyable.hpp>


#define _PPL_NO_USE_RAND_S

#ifndef _PPL_NO_USE_RAND_S

#if !defined(_CRT_RAND_S)
#define _CRT_RAND_S
//#error _CRT_RAND_S is not defined, rand_s is unavailable
#endif

#include <stdlib.h>

class SecureRandomGeneratorImpl : private noncopyable
{
public:
	bool IsSecureRandOK()
	{
		OSVERSIONINFO verinfo = { 0 };
		verinfo.dwOSVersionInfoSize = sizeof( verinfo );
		if ( FALSE == ::GetVersionEx( &verinfo ) )
			return false;
		return verinfo.dwMajorVersion > 5 || ( verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion >= 1 );
	}
	unsigned int Next()
	{
		static const bool is_rand_s_ok = IsSecureRandOK();
		if ( false == is_rand_s_ok )
		{
			WORD low = static_cast<WORD>( ::rand() );
			WORD high = static_cast<WORD>( ::rand() );
			return MAKE_DWORD( low, high );
		}

		unsigned int val = 0;
		errno_t errcode = ::rand_s(&val);
		if ( errcode != 0 || val == 0 )
		{
			val = rand();
			assert(false);
		}
		return val;
	}
	DWORD NextDWord()
	{
		return this->Next();
	}
	WORD NextWord()
	{
		return static_cast<WORD>( this->Next() );
	}
};


#endif


class OldRandomSeed : private boost::noncopyable
{
public:
	OldRandomSeed()
	{
		InitSeed();
	}

	static void InitSeed( unsigned seed )
	{
		::srand( seed );
		TRACE("set initial random seed %d %d\n", seed, ::rand());
	}
	static void InitSeed()
	{
#ifdef _PPL_PLATFORM_MSWIN
		UINT32 seed = TimeCounter::GetSystemCount32();
#else
		time_t seed = ::time(NULL);
#endif
		InitSeed(seed);
	}
};

/// 随机数生成器
class OldRandomGeneratorImpl : private boost::noncopyable
{
public:
	OldRandomGeneratorImpl()
	{
		static OldRandomSeed randomSeed;
	}
	explicit OldRandomGeneratorImpl(unsigned int seed)
	{
		SetSeed(seed);
	}

	void SetSeed(unsigned int seed)
	{
		assert(false);
		::srand(seed);
	}

	unsigned int Next()
	{
		return NextDWord();
	}

	DWORD NextDWord()
	{
		WORD low = this->NextWord();
		WORD high = this->NextWord();
		return MAKE_DWORD( low, high );
	}
	WORD NextWord()
	{
		int val = ::rand();
		return static_cast<unsigned int>( val );
	}

};


template <typename ImplT>
class RandomGeneratorT : private boost::noncopyable
{
public:
	typedef ImplT implementation_type;

	RandomGeneratorT()
	{
	}

	unsigned int NextValue()
	{
		return m_impl.Next();
	}
	unsigned int Next()
	{
		return m_impl.Next();
	}


	unsigned int Next(unsigned int maxValue)
	{
		return this->NextIn(maxValue);
	}

	unsigned int NextIn(unsigned int maxValue)
	{
		assert(maxValue > 0);
		return m_impl.Next() % maxValue;
	}
	unsigned char NextByte()
	{
		return static_cast<unsigned char>( m_impl.Next() );
	}
	unsigned short NextWord()
	{
		return m_impl.NextWord();
	}
	DWORD NextDWord()
	{
		return m_impl.NextDWord();
	}

	inline std::string RandomString(size_t size)
	{
		assert(size < SHRT_MAX);
		std::string str(size, '0');
		for (size_t i = 0; i < size; ++i)
		{
			str[i] = NextByte();
		}
		return str;
	}

	inline std::string RandomSizedString(size_t size)
	{
		assert(size < SHRT_MAX);
		return RandomString(NextWord((int)size));
	}

private:
	ImplT m_impl;
};


#ifdef _PPL_NO_USE_RAND_S

typedef RandomGeneratorT<OldRandomGeneratorImpl> RandomGenerator;
#pragma message("------ use old random generator")

#else

typedef RandomGeneratorT<SecureRandomGeneratorImpl> RandomGenerator;
#pragma message("------ use secure random generator")

#endif

typedef RandomGenerator Random;



