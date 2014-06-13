
#pragma once



class CryptProvider
{
public:
	CryptProvider() : m_handle(NULL), m_isOpen(false)
	{
	}
	~CryptProvider()
	{
		Close();
	}

	HCRYPTPROV GetHandle() const { return m_handle; }
	bool IsOpen() const { return m_isOpen; }
	void Close()
	{
		if (m_isOpen)
		{
			BOOL success = ::CryptReleaseContext(m_handle, 0);
			m_handle = NULL;
			m_isOpen = false;
			assert(success);
		}
	}

	bool Open(DWORD type, DWORD flags)
	{
		Close();
		BOOL success = ::CryptAcquireContext(&m_handle, NULL, NULL, type, flags);
		if (!success)
		{
			DWORD errcode = ::GetLastError();
			if (errcode == NTE_BAD_KEYSET)
			{
				success = ::CryptAcquireContext(&m_handle, NULL, NULL, type, flags | CRYPT_NEWKEYSET);
				assert(success);
			}
		}
		m_isOpen = (success != FALSE);
		return m_isOpen;
	}

private:
	HCRYPTPROV m_handle;
	bool m_isOpen;
};


class Hashing
{
public:
	Hashing() : m_handle(NULL), m_isOpen(false)
	{
	}
	~Hashing()
	{
		Close();
	}

	bool IsOpen() const { return m_isOpen; }
	void Close()
	{
		if (m_isOpen)
		{
			BOOL success = ::CryptDestroyHash(m_handle);
			m_isOpen = true;
			m_handle = NULL;
			assert(success);
		}
	}

	bool Open(const CryptProvider& provider, ALG_ID algID, HCRYPTKEY key)
	{
		assert(provider.IsOpen());
		Close();
		BOOL success = ::CryptCreateHash(provider.GetHandle(), algID, key, 0, &m_handle);
		m_isOpen = (success != FALSE);
		return m_isOpen;
	}

	bool OpenMD5(const CryptProvider& provider)
	{
		return Open(provider, CALG_MD5, NULL);
	}
	bool OpenSHA(const CryptProvider& provider)
	{
		return Open(provider, CALG_SHA, NULL);
	}

	bool HashData(const void* data, size_t size)
	{
		assert(IsOpen());
		BOOL success = ::CryptHashData(m_handle, static_cast<const BYTE*>(data), size, 0);
		assert(success);
		return success != FALSE;
	}
	bool HashData(const string& data)
	{
		return HashData(data.data(), data.size());
	}
	bool GetParam(DWORD param, BYTE* data, DWORD* len)
	{
		assert(IsOpen());
		BOOL success = ::CryptGetHashParam(m_handle, param, data, len, 0);
		assert(success);
		return success != FALSE;
	}
	size_t GetDigestSize()
	{
		DWORD size = 0;
		DWORD sizeLen = sizeof(DWORD);
		bool success = GetParam(HP_HASHSIZE, reinterpret_cast<BYTE*>(&size), &sizeLen);
		return success ? size : 0;
	}
	string GetDigest()
	{
		string digest;
		size_t size = GetDigestSize();
		if (size > 0)
		{
			digest.resize(size);
			DWORD digestLen = size;
			bool success = GetParam(HP_HASHVAL, reinterpret_cast<BYTE*>(&digest[0]), &digestLen);
			if (!success)
				digest.erase();
		}
		return digest;
	}

	static string MD5(const void* data, size_t size)
	{
		CryptProvider provider;
		provider.Open(PROV_DSS, CRYPT_VERIFYCONTEXT);
		Hashing hashing;
		hashing.OpenMD5(provider);
		hashing.HashData(data, size);
		string digest = hashing.GetDigest();
		assert(digest.size() == 16);
		return digest;
	}
	static string SHA(const void* data, size_t size)
	{
		CryptProvider provider;
		provider.Open(PROV_DSS, CRYPT_VERIFYCONTEXT);
		Hashing hashing;
		hashing.OpenSHA(provider);
		hashing.HashData(data, size);
		string digest = hashing.GetDigest();
		assert(digest.size() == 20);
		return digest;
	}
	static string MD5(const string& data)
	{
		return MD5(data.data(), data.size());
	}
	static string SHA(const string& data)
	{
		return SHA(data.data(), data.size());
	}

private:
	HCRYPTHASH m_handle;
	bool m_isOpen;
};



