
#pragma once

#include <ppl/config.h>

#include <ppl/crypto/sha.h>



class HMACHashing
{
public:
	static const size_t HASH_SIZE = 20;
	bool HashData(const BYTE* btKey, DWORD dwKenLen, const BYTE* btIn, DWORD dwInLen, BYTE* btOut)
	{
		assert(dwKenLen <= 64);
		if ( dwKenLen > 64 )
			return false;

		const BYTE *btRealKey = btKey;
		DWORD dwRealKeyLen = dwKenLen;

		BYTE btInPad[64], btOutPad[64];
		{
			m_hasher.Clear();
			if ( false == m_hasher.Init() )
				return false;

			memset(btInPad, 0, 64);
			memset(btOutPad, 0, 64);
			memcpy(btInPad, btKey, dwKenLen);
			memcpy(btOutPad, btKey, dwKenLen);

			for (int i = 0; i < 64; i++)
			{
				btInPad[i] ^= 0x36;
				btOutPad[i] ^= 0x5c;
			}

			if ( false == m_hasher.AddData(btInPad, 64) )
				return false;

			if ( false == m_hasher.AddData(btIn, dwInLen) )
				return false;

			if ( false == m_hasher.GetResult(btOut) )
				return false;

		}

		{

			m_hasher.Clear();
			if ( false == m_hasher.Init() )
				return false;

			if ( false == m_hasher.AddData(btOutPad, 64) )
				return false;

			if ( false == m_hasher.AddData(btOut, HASH_SIZE) )
				return false;

			if ( false == m_hasher.GetResult(btOut) )
				return false;
		}
		return true;
	}

private:
	SHA1Hashing m_hasher;

};