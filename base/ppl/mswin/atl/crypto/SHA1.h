
#pragma once


#include <atlcomcli.h>
#include <atlcrypt.h>
#include "utils.h"




class CSHA1  
{
public:
	CSHA1() 
	{
		if (FAILED(m_Context.Initialize(PROV_RSA_FULL, NULL, NULL, 0)))
		{
			m_Context.InitCreateKeySet(PROV_RSA_FULL, NULL, NULL, 0);
		}
	}
	~CSHA1() {}

	static const int HASHSIZE = 20;

	bool HashData(BYTE *btIn, DWORD dwInLen, BYTE *btOut)
	{
		HRESULT hr;
		if (m_sha1.GetHandle())
		{
			m_sha1.Destroy();
		}

		hr = m_sha1.Initialize(m_Context);
		ON_ERROR_RETURN(FAILED(hr), false);

		hr = m_sha1.AddData(btIn, dwInLen);
		ON_ERROR_RETURN(FAILED(hr), false);

		DWORD dwHashLen = HASHSIZE;
		hr = m_sha1.GetValue(btOut, &dwHashLen);
		ON_ERROR_RETURN(FAILED(hr), false);

		return true;
	}

private:
	CCryptProv m_Context;
	CCryptSHA1Hash m_sha1;
};