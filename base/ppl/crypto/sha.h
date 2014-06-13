
#pragma once


#include <ppl/config.h>

#if defined(_PPL_PLATFORM_MSWIN)

#include <atlcomcli.h>
#include <atlcrypt.h>
#include <ppl/mswin/atl/crypto/utils.h>

class SHA1Hashing
{
public:
	SHA1Hashing()
	{
		if (FAILED(m_Context.Initialize(PROV_RSA_FULL, NULL, NULL, 0)))
		{
			m_Context.InitCreateKeySet(PROV_RSA_FULL, NULL, NULL, 0);
		}
	}
	~SHA1Hashing()
	{
	}

	static const size_t HASH_SIZE = 20;

	bool Init()
	{
		HRESULT hr = m_HashAlgorithm.Initialize(m_Context);
		return SUCCEEDED(hr);
	}
	void Clear()
	{
		m_HashAlgorithm.Destroy();
	}
	bool AddData(const BYTE* data, size_t size)
	{
		HRESULT hr = m_HashAlgorithm.AddData(data, size);
		assert(SUCCEEDED(hr));
		return SUCCEEDED(hr);
	}
	bool GetResult(BYTE* data)
	{
		DWORD len = HASH_SIZE;
		HRESULT hr = m_HashAlgorithm.GetValue(data, &len);
		assert(SUCCEEDED(hr));
		return SUCCEEDED(hr);
	}

private:
	CCryptProv m_Context;
	CCryptSHA1Hash m_HashAlgorithm;
};


#elif defined(_PPL_PLATFORM_LINUX)

#include <openssl/sha.h>

class SHA1Hashing
{
public:
	SHA1Hashing()
	{
	}
	~SHA1Hashing()
	{
	}

	static const size_t HASH_SIZE = 20;

	bool Init()
	{
		SHA1_Init(&m_Context);
		return true;
	}
	void Clear()
	{
	}
	bool AddData(const BYTE* data, size_t size)
	{
		SHA1_Update(&m_Context, data, size);
		return true;
	}
	bool GetResult(BYTE* data)
	{
		SHA1_Final(data, &m_Context);
		return true;
	}

private:
	SHA_CTX m_Context;
};


#else

#error "invalid platofmr for sha1"

#endif



