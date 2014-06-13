#pragma once
#include "CryptImportPlainTextKey.h"

class CTripleDES  
{
public:
	CTripleDES(const BYTE btKey[24], const BYTE btIV[8]) 
	{
		if (FAILED(m_Context.Initialize(PROV_RSA_FULL, NULL, NULL, 0)))
		{
			m_Context.InitCreateKeySet(PROV_RSA_FULL, NULL, NULL, 0);
		}

		m_Key.ImportPlainText(m_Context, btKey, 24, CALG_3DES);
		m_Key.SetIV((BYTE*) btIV);
	};

	~CTripleDES() {};

	bool Decrypt(BYTE *pbtData, DWORD *pdwLen)
	{
		if (!m_Key.GetHandle())
		{
			return false;
		}

		return CryptDecrypt(m_Key.GetHandle(), 0, TRUE, 0, pbtData, pdwLen) == TRUE;
	}

	bool Encrypt(BYTE *pbtData, DWORD *pdwLen, DWORD dwBufLen)
	{
		if (!m_Key.GetHandle())
		{
			return false;
		}

		return CryptEncrypt(m_Key.GetHandle(), NULL, TRUE, 0, pbtData, pdwLen, dwBufLen) == TRUE;
	}
private:
	CCryptProv m_Context;
	CCryptImportPlainTextKey m_Key;
};