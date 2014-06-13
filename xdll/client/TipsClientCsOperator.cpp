// TipsClientCsOperator.cpp : Implementation of CTipsClientCsOperator
#include "stdafx.h"
#include "Xdll.h"
#include "TipsClientCsOperator.h"
#include <mshtmdid.h>
#include <mshtmHst.h>

/////////////////////////////////////////////////////////////////////////////
// CTipsClientCsOperator

STDMETHODIMP   CTipsClientCsOperator::Invoke(DISPID   dispidMember,   REFIID   riid,   LCID   lcid,   WORD   wFlags,  
									   DISPPARAMS*   pDispParams,   VARIANT*   pvarResult,  
									   EXCEPINFO*     pExcepInfo,     UINT*   puArgErr)  
{
	switch (dispidMember)
	{
	case DISPID_AMBIENT_DLCONTROL: 
		{
			pvarResult->vt = VT_I4;
			LONG lResult = DLCTL_SILENT | DLCTL_DLIMAGES | DLCTL_NO_JAVA | DLCTL_NO_BEHAVIORS;
 			if (!m_bLoadActiveX)
 				lResult |= DLCTL_NO_RUNACTIVEXCTLS | DLCTL_NO_DLACTIVEXCTLS;
			pvarResult->lVal = lResult;
		}
		break;
	case DISPID_AMBIENT_USERAGENT:
		{
			if (m_strUserAgent.Length() > 0)
			{
				//USES_CONVERSION;
				//CString strUserAgent("MyWebBrowserHost");
				pvarResult->vt = VT_BSTR;
				m_strUserAgent.CopyTo(&pvarResult->bstrVal);
				//pvarResult->bstrVal = ::SysAllocString(T2OLE(strUserAgent));
			}
		}
		break;
	default:
		break;
	}
	
	return 0;
}


// *** IOleCommandTarget ***
HRESULT	STDMETHODCALLTYPE	CTipsClientCsOperator::Exec( 
				/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,
				/* [in] */ DWORD nCmdID,
				/* [in] */ DWORD nCmdexecopt,
				/* [unique][in] */ VARIANT __RPC_FAR *pvaIn,
				/* [unique][out][in] */ VARIANT __RPC_FAR *pvaOut)
{
    HRESULT hr = S_OK;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{
		switch (nCmdID) 
		{
		case OLECMDID_SHOWSCRIPTERROR:
			{
				(*pvaOut).vt = VT_BOOL;
				// Continue running scripts on the page. but do not show error message box
				(*pvaOut).boolVal = VARIANT_TRUE;			
				break;
			}
		default:
            hr = OLECMDERR_E_NOTSUPPORTED;
            break;
		}
	}
	else
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return (hr);
}

HRESULT	STDMETHODCALLTYPE	CTipsClientCsOperator::QueryStatus(
				/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,
				/* [in] */ ULONG cCmds,
				/* [out][in][size_is] */ OLECMD __RPC_FAR prgCmds[],
				/* [unique][out][in] */ OLECMDTEXT __RPC_FAR *pCmdText)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::get_loadFlash(VARIANT_BOOL *aLoadFlash)
{
	if (!aLoadFlash) {
		return E_POINTER;
	}
	if (m_bLoadActiveX) {
		*aLoadFlash = VARIANT_TRUE;
	} else {
		*aLoadFlash = VARIANT_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CTipsClientCsOperator::put_loadFlash(VARIANT_BOOL aLoadFlash)
{
	m_bLoadActiveX = (aLoadFlash != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CTipsClientCsOperator::put_userAgent(BSTR strUserAgent)
{
	m_strUserAgent = strUserAgent;
	return S_OK;
}

STDMETHODIMP CTipsClientCsOperator::ShowContextMenu(DWORD dwID, POINT *ppt,
													IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget,
				  IOleInPlaceFrame *pFrame,
				  IOleInPlaceUIWindow *pDoc)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::HideUI( void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::UpdateUI( void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::GetExternal(IDispatch **ppDispatch)
{
	if (!m_spExternal) {
		CExternalObject* p = static_cast<CExternalObject*>(CExternalObject::CreateObject());
		m_spExternal.Attach(p->GetIDispatch(FALSE));
	}
	if (m_spExternal) {
		return m_spExternal.CopyTo(ppDispatch);
	}
    return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTipsClientCsOperator::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return E_NOTIMPL;
}
