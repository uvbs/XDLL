// HttpProtocol.h: Definition of the HttpProtocol class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPPROTOCOL_H__B07508D2_BB21_4254_A070_CF4BCD33FB1A__INCLUDED_)
#define AFX_HTTPPROTOCOL_H__B07508D2_BB21_4254_A070_CF4BCD33FB1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// HttpProtocol


class CHttpProtocol : 
	public CComObjectRoot,
	public CComCoClass<CHttpProtocol,&__uuidof(CLSID_MyHttpProtocol)>,
	public IInternetProtocolEx
{
public:
	CHttpProtocol()
	{
	}

BEGIN_COM_MAP(CHttpProtocol)
	COM_INTERFACE_ENTRY(IInternetProtocolEx)
	COM_INTERFACE_ENTRY(IInternetProtocol)
	COM_INTERFACE_ENTRY(IInternetProtocolRoot)
END_COM_MAP()

DECLARE_NO_REGISTRY()

	// IInternetProtocol
public:
	HRESULT STDMETHODCALLTYPE Read(void *pv,ULONG cb,ULONG *pcbRead);
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,DWORD dwOrigin,ULARGE_INTEGER *plibNewPosition);
	HRESULT STDMETHODCALLTYPE LockRequest(DWORD dwOptions);
	HRESULT STDMETHODCALLTYPE UnlockRequest(void);

	// IInternetProtocolRoot
public:
	HRESULT STDMETHODCALLTYPE Start(LPCWSTR szUrl,IInternetProtocolSink *pOIProtSink,IInternetBindInfo *pOIBindInfo,DWORD grfPI,HANDLE_PTR dwReserved);
	HRESULT STDMETHODCALLTYPE Continue(PROTOCOLDATA *pProtocolData);
	HRESULT STDMETHODCALLTYPE Abort(HRESULT hrReason,DWORD dwOptions);
	HRESULT STDMETHODCALLTYPE Terminate(DWORD dwOptions) ;
	HRESULT STDMETHODCALLTYPE Suspend(void);
	HRESULT STDMETHODCALLTYPE Resume(void);

	// IInternetProtocolEx
public:
	HRESULT STDMETHODCALLTYPE StartEx(IUri *pUri,IInternetProtocolSink *pOIProtSink,IInternetBindInfo *pOIBindInfo,DWORD grfPI,HANDLE_PTR dwReserved);

protected:
	CComPtr<IInternetProtocolSink> m_spProtocolSink;
	CComPtr<IInternetBindInfo>     m_spBindInfo;

};

#endif // !defined(AFX_HTTPPROTOCOL_H__B07508D2_BB21_4254_A070_CF4BCD33FB1A__INCLUDED_)
