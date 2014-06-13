// HttpProtocol.cpp : Implementation of CXdllApp and DLL registration.

#include "stdafx.h"
#include "xdll.h"
#include "HttpProtocol.h"
#include "TipsClient.h"

/////////////////////////////////////////////////////////////////////////////
//
STDMETHODIMP CHttpProtocol::Read(void *pv,ULONG cb,ULONG *pcbRead)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpProtocol::Seek(LARGE_INTEGER dlibMove,DWORD dwOrigin,ULARGE_INTEGER *plibNewPosition)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpProtocol::LockRequest(DWORD dwOptions)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpProtocol::UnlockRequest(void)
{
	return E_NOTIMPL;
}


STDMETHODIMP CHttpProtocol::Start(LPCWSTR szUrl,IInternetProtocolSink *pOIProtSink,IInternetBindInfo *pOIBindInfo,DWORD grfPI,HANDLE_PTR dwReserved)
{
	m_spProtocolSink = pOIProtSink;
	m_spBindInfo = pOIBindInfo;

	TipsClient* client = AfxGetTipsClient();
	if (!client || client->CanDonwloadUrl(szUrl)) {
		return INET_E_USE_DEFAULT_PROTOCOLHANDLER;
	}

	return INET_E_INVALID_URL;
}

STDMETHODIMP CHttpProtocol::Continue(PROTOCOLDATA *pProtocolData)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpProtocol::Abort(HRESULT hrReason,DWORD dwOptions)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpProtocol::Terminate(DWORD dwOptions) 
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpProtocol::Suspend(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpProtocol::Resume( void)
{
	return E_NOTIMPL;
}


STDMETHODIMP CHttpProtocol::StartEx(IUri *pUri, IInternetProtocolSink *pOIProtSink,
									IInternetBindInfo *pOIBindInfo, DWORD grfPI,
									HANDLE_PTR dwReserved)
{
	if (!pUri) {
		return E_POINTER;
	}

	HRESULT hr = INET_E_INVALID_URL;
	CComPtr<IUri> spUri;
	spUri = pUri;
	CComBSTR url;
	if (FAILED(hr = spUri->GetAbsoluteUri(&url))) {
		return hr;
	}
	spUri.Release();

	hr = Start(url, pOIProtSink, pOIBindInfo, grfPI, dwReserved);

	return hr;
}
