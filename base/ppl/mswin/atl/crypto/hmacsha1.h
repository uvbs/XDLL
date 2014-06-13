#pragma once

#include <atlcomcli.h>
#include <atlcrypt.h>
#include "utils.h"


class CHMACSHA1  
{
public:
	CHMACSHA1() 
	{
		if (FAILED(m_Context.Initialize(PROV_RSA_FULL, NULL, NULL, 0)))
		{
			m_Context.InitCreateKeySet(PROV_RSA_FULL, NULL, NULL, 0);
		}
	}
	~CHMACSHA1() {}

	static const int HASHSIZE = 20;

	bool HashData(const BYTE* btKey, DWORD dwKenLen, const BYTE* btIn, DWORD dwInLen, BYTE* btOut)
	{
		const BYTE *btRealKey = btKey;
		DWORD dwRealKeyLen = dwKenLen;
		HRESULT hr;

		if (m_HashAlgorithm.GetHandle())
		{
			m_HashAlgorithm.Destroy();
		}
		hr = m_HashAlgorithm.Initialize(m_Context);
		ON_ERROR_RETURN(FAILED(hr), false);

		if (dwKenLen > 64)
		{
			BYTE* tempKey = new BYTE[HASHSIZE];
			ON_ERROR_RETURN(!tempKey, false);

			hr = m_HashAlgorithm.AddData(btKey, dwKenLen);
			ON_ERROR_RETURN(FAILED(hr), false);

			DWORD dwBufferLen = HASHSIZE;
			hr = m_HashAlgorithm.GetValue(tempKey, &dwBufferLen);
			ON_ERROR_RETURN(FAILED(hr), false);

			dwRealKeyLen = HASHSIZE;

			m_HashAlgorithm.Destroy();
			hr = m_HashAlgorithm.Initialize(m_Context);
			ON_ERROR_RETURN(FAILED(hr), false);

			btRealKey = tempKey;
		}

		BYTE btInPad[64], btOutPad[64];
		memset(btInPad, 0, 64);
		memset(btOutPad, 0, 64);
		memcpy(btInPad, btKey, dwKenLen);
		memcpy(btOutPad, btKey, dwKenLen);

		for (int i = 0; i < 64; i++)
		{
			btInPad[i] ^= 0x36;
			btOutPad[i] ^= 0x5c;
		}

		hr = m_HashAlgorithm.AddData(btInPad, 64);
		ON_ERROR_RETURN(FAILED(hr), false);

		hr = m_HashAlgorithm.AddData(btIn, dwInLen);
		ON_ERROR_RETURN(FAILED(hr), false);

		DWORD dwBufferLen = HASHSIZE;
		hr = m_HashAlgorithm.GetValue(btOut, &dwBufferLen);
		ON_ERROR_RETURN(FAILED(hr), false);

		m_HashAlgorithm.Destroy();
		hr = m_HashAlgorithm.Initialize(m_Context);
		ON_ERROR_RETURN(FAILED(hr), false);

		hr = m_HashAlgorithm.AddData(btOutPad, 64);
		ON_ERROR_RETURN(FAILED(hr), false);

		hr = m_HashAlgorithm.AddData(btOut, HASHSIZE);
		ON_ERROR_RETURN(FAILED(hr), false);

		hr = m_HashAlgorithm.GetValue(btOut, &dwBufferLen);
		ON_ERROR_RETURN(FAILED(hr), false);

		if (btKey != btRealKey)
		{
			delete[] btRealKey;
		}

		return true;
	}

private:
	CCryptProv m_Context;
	CCryptSHA1Hash m_HashAlgorithm;
};