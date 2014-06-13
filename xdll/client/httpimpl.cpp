#include "StdAfx.h"
#include <string>
#include <vector>
#include "httpimpl.h"

class CHttpDownloadImpl :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CHttpDownloadImpl, &CLSID_NULL>,
	public IBindStatusCallback,
	public IHttpNegotiate,
	public HttpDownload
{
public:
	CHttpDownloadImpl()
	{
		m_dwBindVerb = BINDVERB_GET;
		m_dwBindFlag = BINDF_ASYNCHRONOUS|BINDF_ASYNCSTORAGE|BINDF_GETNEWESTVERSION|BINDF_NOWRITECACHE;
		m_listener = NULL;
		m_bAborted = false;
	}

	BEGIN_COM_MAP(CHttpDownloadImpl)
		COM_INTERFACE_ENTRY(IBindStatusCallback)
		COM_INTERFACE_ENTRY(IHttpNegotiate)
	END_COM_MAP()

	// interface implementation
public:
	// IHttpNegotiate
	STDMETHODIMP BeginningTransaction(LPCWSTR szURL, LPCWSTR szHeaders, DWORD dwReserved, LPWSTR *pszAdditionalHeaders)
	{
		wchar_t userAgentKey[] = L"User-Agent: ";
		wchar_t refererKey[] = L"Referer: ";

		// 计算长度
		int length = 0;
		if (!m_userAgent.empty())
			length += sizeof(userAgentKey) + m_userAgent.length()*sizeof(wchar_t) + 2*sizeof(wchar_t);
		if (!m_referer.empty())
			length += sizeof(refererKey) + m_referer.length()*sizeof(wchar_t) + 2*sizeof(wchar_t);
		length += sizeof(wchar_t);

		// 申请空间
		*pszAdditionalHeaders = (LPWSTR)CoTaskMemAlloc(length);
		if (!pszAdditionalHeaders)
		{
			return E_FAIL;
		}

		// 填充内容
		wcscpy(*pszAdditionalHeaders, L"");
		if (!m_userAgent.empty()) {
			wcscat(*pszAdditionalHeaders, userAgentKey);
			wcscat(*pszAdditionalHeaders, m_userAgent.c_str());
			wcscat(*pszAdditionalHeaders, L"\r\n");
		}
		if (!m_referer.empty()) {
			wcscat(*pszAdditionalHeaders, refererKey);
			wcscat(*pszAdditionalHeaders, m_referer.c_str());
			wcscat(*pszAdditionalHeaders, L"\r\n");
		}

		return S_OK;
	}
        
	STDMETHODIMP OnResponse(DWORD dwResponseCode, LPCWSTR szResponseHeaders, LPCWSTR szRequestHeaders, LPWSTR *pszAdditionalRequestHeaders)
	{
		return S_OK;
	}

public:
	// IBindStatusCallback
	STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding *pBinding)
	{
		m_spBinding = pBinding;
		if (m_bAborted) {
			return E_ABORT;
		}
		return S_OK;
	}
	STDMETHOD(GetPriority)(LONG *pnPriority)
	{
		if (!pnPriority) {
			return E_INVALIDARG;
		}
		*pnPriority = THREAD_PRIORITY_NORMAL;
		return S_OK;
	}
	STDMETHOD(OnLowResource)(DWORD reserved)
	{
		return S_OK;
	}
	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		if (m_bAborted) {
			return E_ABORT;
		}
		return S_OK;
	}
	STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError)
	{
		m_spBinding.Release();
		m_spBindCtx.Release();
		m_spMoniker.Release();
		if (!m_listener) {
			return S_OK;
		}
		if (SUCCEEDED(hresult)) {
			m_listener->OnDataReceive(m_responseData.begin(), m_responseData.size());
		} else {
			m_listener->OnDateReceiveFailed(hresult);
		}
		return S_OK;
	}
	STDMETHOD(GetBindInfo)(DWORD *pgrfBINDF, BINDINFO *pbindinfo)
	{
		if (!pbindinfo || pbindinfo->cbSize == 0 || !pgrfBINDF) {
			return E_INVALIDARG;
		}

		*pgrfBINDF = m_dwBindFlag;

		ULONG cbSize = pbindinfo->cbSize;
		memset(pbindinfo, 0, cbSize);
		pbindinfo->cbSize = cbSize;
		pbindinfo->dwBindVerb = m_dwBindVerb;

		if (m_dwBindVerb == BINDVERB_POST && m_sendData.size() > 0) {
			pbindinfo->stgmedData.tymed = TYMED_HGLOBAL;
			pbindinfo->stgmedData.hGlobal = GlobalAlloc(GMEM_FIXED, m_sendData.size());
			unsigned char* buffer = (unsigned char*)GlobalLock(pbindinfo->stgmedData.hGlobal);
			if (buffer) {
				memcpy(buffer, m_sendData.begin(), m_sendData.size());
				pbindinfo->cbstgmedData = m_sendData.size();
			}
			GlobalUnlock(pbindinfo->stgmedData.hGlobal);
		}
		return S_OK;
	}
	STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed)
	{
		HRESULT hr = S_OK;

		// get the stream passed
		if (grfBSCF & BSCF_FIRSTDATANOTIFICATION) {
			if (!m_spStream && pstgmed->tymed == TYMED_ISTREAM) {
				m_spStream = pstgmed->pstm;
			}
		}

		// read response data
		if (grfBSCF & BSCF_LASTDATANOTIFICATION) {
			STATSTG statstg;
			DWORD dwVal = STATFLAG_NONAME;
			if (m_listener && m_spStream->Stat(&statstg, dwVal) == S_OK)
			{
				m_responseData.resize(statstg.cbSize.QuadPart);
				ULONG ulRead = 0;
				m_spStream->Read(m_responseData.begin(), m_responseData.size(), &ulRead);
			}

			m_spStream.Release();
		}
		return m_bAborted ? E_ABORT : hr;
	}
	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown *punk)
	{
		return S_OK;
	}

	// HttpDownload
	bool StartDownload(LPCTSTR aUrl, DWORD aHttpVerb, const unsigned char* aSendData, size_t aDataSize)
	{
		m_dwBindVerb = aHttpVerb;
		m_sendData.assign((const char *)aSendData, aDataSize);

		CComBSTR bstrUrl;
		bstrUrl.Attach(A2BSTR(aUrl));
		return SUCCEEDED(StartAsyncDownload(bstrUrl));
	}
	void SetListener(HttpDownloadListener* listener)
	{
		m_listener = listener;
	}
	void Abort()
	{
		if (m_spBinding) {
			m_spBinding->Abort();
		}
		m_bAborted = true;
	}
	void Destroy()
	{
		Abort();
		m_listener = NULL;
		this->Release();
	}

	HRESULT _StartAsyncDownload(BSTR bstrURL, IUnknown* pUnkContainer, BOOL bRelative)
	{
		HRESULT hr = S_OK;
		CComQIPtr<IServiceProvider, &IID_IServiceProvider> spServiceProvider(pUnkContainer);
		CComPtr<IBindHost>	spBindHost;
		CComPtr<IStream>	spStream;
		if (spServiceProvider)
			spServiceProvider->QueryService(SID_IBindHost, IID_IBindHost, (void**)&spBindHost);
		
		if (spBindHost == NULL)
		{
			if (bRelative)
				return E_NOINTERFACE;  // relative asked for, but no IBindHost
			hr = CreateURLMoniker(NULL, bstrURL, &m_spMoniker);
			if (SUCCEEDED(hr))
				hr = CreateBindCtx(0, &m_spBindCtx);
			
			if (SUCCEEDED(hr))
				hr = RegisterBindStatusCallback(m_spBindCtx, static_cast<IBindStatusCallback*>(this), 0, 0L);
			else
				m_spMoniker.Release();
			
			if (SUCCEEDED(hr))
				hr = m_spMoniker->BindToStorage(m_spBindCtx, 0, IID_IStream, (void**)&spStream);
		}
		else
		{
			hr = CreateBindCtx(0, &m_spBindCtx);
			if (SUCCEEDED(hr))
				hr = RegisterBindStatusCallback(m_spBindCtx, static_cast<IBindStatusCallback*>(this), 0, 0L);
			
			if (SUCCEEDED(hr))
			{
				if (bRelative)
					hr = spBindHost->CreateMoniker(bstrURL, m_spBindCtx, &m_spMoniker, 0);
				else
					hr = CreateURLMoniker(NULL, bstrURL, &m_spMoniker);
			}
			
			if (SUCCEEDED(hr))
			{
				hr = spBindHost->MonikerBindToStorage(m_spMoniker, m_spBindCtx, static_cast<IBindStatusCallback*>(this), IID_IStream, (void**)&spStream);
				ATLTRACE2(atlTraceControls,2,_T("Bound"));
			}
		}
		return hr;
	}
	
	HRESULT StartAsyncDownload(BSTR bstrURL, IUnknown* pUnkContainer = NULL, BOOL bRelative = FALSE)
	{
		return  _StartAsyncDownload(bstrURL, pUnkContainer, bRelative);
	}

public:
	HttpDownloadListener* m_listener;
	DWORD m_dwBindVerb;
	DWORD m_dwBindFlag;
	CComPtr<IMoniker> m_spMoniker;
	CComPtr<IBindCtx> m_spBindCtx;
	CComPtr<IBinding> m_spBinding;
	CComPtr<IStream>  m_spStream;
	std::string m_sendData;
	std::wstring m_userAgent;
	std::wstring m_referer;
	std::vector<unsigned char> m_responseData;
	bool m_bAborted;
};

HttpDownload* HttpDownload::Create(LPCTSTR szUserAgent, LPCTSTR szReferer)
{
	CComObject<CHttpDownloadImpl>* pDownload = NULL;
	if (SUCCEEDED(CComObject<CHttpDownloadImpl>::CreateInstance(&pDownload))) {
		if (szUserAgent) {
			USES_CONVERSION;
			pDownload->m_userAgent = T2CW(szUserAgent);
		}
		if (szReferer) {
			USES_CONVERSION;
			pDownload->m_referer = T2CW(szReferer);
		}
		pDownload->AddRef();
		return pDownload;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT HttpDownloadToFile(LPCTSTR szURL, LPCTSTR szFileName, bool foreceReload, LPCTSTR szUserAgent, LPCTSTR szReferer)
{
	HttpDownload* httpDownload = NULL;
	CHttpDownloadImpl* httpImpl = NULL;

	if (foreceReload || szUserAgent) {
		httpDownload = HttpDownload::Create();
		if (!httpDownload) {
			return E_OUTOFMEMORY;
		}
		
		httpImpl = static_cast<CHttpDownloadImpl*>(httpDownload);
		if (foreceReload) {
			httpImpl->m_dwBindFlag = BINDF_GETNEWESTVERSION | BINDF_RESYNCHRONIZE | BINDF_PRAGMA_NO_CACHE;
		}
		if (szUserAgent) {
			USES_CONVERSION;
			httpImpl->m_userAgent = T2CW(szUserAgent);
		}
		if (szReferer) {
			USES_CONVERSION;
			httpImpl->m_referer = T2CW(szReferer);
		}
	}

	HRESULT hr = ::URLDownloadToFile(NULL, szURL, szFileName, 0, httpImpl ? static_cast<IBindStatusCallback*>(httpImpl) : NULL);

	if (httpDownload) {
		httpDownload->Destroy();
	}

	return hr;
}

HRESULT HttpDownloadToCacheFile(LPCTSTR szURL, LPTSTR szFileName, DWORD cchFileName, bool foreceReload, LPCTSTR szUserAgent, LPCTSTR szReferer)
{
	HttpDownload* httpDownload = NULL;
	CHttpDownloadImpl* httpImpl = NULL;
	
	if (foreceReload || szUserAgent) {
		httpDownload = HttpDownload::Create();
		if (!httpDownload) {
			return E_OUTOFMEMORY;
		}
		
		httpImpl = static_cast<CHttpDownloadImpl*>(httpDownload);
		if (foreceReload) {
			httpImpl->m_dwBindFlag = BINDF_GETNEWESTVERSION | BINDF_RESYNCHRONIZE | BINDF_PRAGMA_NO_CACHE;
		}
		if (szUserAgent) {
			USES_CONVERSION;
			httpImpl->m_userAgent = T2CW(szUserAgent);
		}
		if (szReferer) {
			USES_CONVERSION;
			httpImpl->m_referer = T2CW(szReferer);
		}
	}
	
	HRESULT hr = ::URLDownloadToCacheFile(NULL, szURL, szFileName, cchFileName, 0,  httpImpl ? static_cast<IBindStatusCallback*>(httpImpl) : NULL);

	if (httpDownload) {
		httpDownload->Destroy();
	}
	
	return hr;
}
