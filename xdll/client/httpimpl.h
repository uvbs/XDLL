#ifndef _HTTPIMPL_H_
#define _HTTPIMPL_H_

class __declspec(novtable) HttpDownloadListener
{
public:
	virtual ~HttpDownloadListener() { }
	
	virtual void OnDataReceive(unsigned char* data, size_t size) = 0;
	virtual void OnDateReceiveFailed(long errcode) = 0;
};

class HttpDownload
{
public:
	static HttpDownload* Create(LPCTSTR szUserAgent = NULL, LPCTSTR szReferer = NULL);
	virtual bool StartDownload(LPCTSTR aUrl, DWORD aHttpVerb, const unsigned char* aSendData, size_t aDataSize) = 0;
	virtual void SetListener(HttpDownloadListener* listener) = 0;
	virtual void Abort() = 0;
	virtual void Destroy() = 0;
};

HRESULT HttpDownloadToFile(LPCTSTR szURL, LPCTSTR szFileName, bool foreceReload, LPCTSTR szUserAgent = NULL, LPCTSTR szReferer = NULL);
HRESULT HttpDownloadToCacheFile(LPCTSTR szURL, LPTSTR szFileName, DWORD cchFileName, bool foreceReload, LPCTSTR szUserAgent = NULL, LPCTSTR szReferer = NULL);

#endif // _HTTPIMPL_H_

