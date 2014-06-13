// TipsClientCsOperator.h : Declaration of the CTipsClientCsOperator

#ifndef __TIPSCLIENTCSOPERATOR_H_
#define __TIPSCLIENTCSOPERATOR_H_

#include "resource.h"       // main symbols

#include "Xdll_i.h"
#include <MsHtmHst.h>
#include "ExternalObject.h"
/////////////////////////////////////////////////////////////////////////////
// CTipsClientCsOperator
class ATL_NO_VTABLE CTipsClientCsOperator : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTipsClientCsOperator, &CLSID_TipsClientCsOperator>,
	public IOleClientSite,
	public IDispatchImpl<ITipsClientCsOperator, &IID_ITipsClientCsOperator, &LIBID_XdllLib>,
	public IOleCommandTarget,
	public IDocHostUIHandler
{
public:
	CTipsClientCsOperator(bool bLoadActiveX = false) : m_bLoadActiveX(bLoadActiveX)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TIPSCLIENTCSOPERATOR)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTipsClientCsOperator)
	COM_INTERFACE_ENTRY(ITipsClientCsOperator)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IOleClientSite)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY(IDocHostUIHandler)
END_COM_MAP()

// ITipsClientCsOperator
public:
	STDMETHOD(get_loadFlash)(VARIANT_BOOL *aLoadFlash);
	STDMETHOD(put_loadFlash)(VARIANT_BOOL aLoadFlash);
	STDMETHOD(put_userAgent)(BSTR strUserAgent);

public:
	STDMETHOD(Invoke)(DISPID   dispidMember,REFIID   riid,   LCID   lcid,   WORD   wFlags,  
		DISPPARAMS   *   pdispparams,   VARIANT   *   pvarResult,  
		EXCEPINFO   *   pexcepinfo,   UINT   *   puArgErr);  
	
	//   IOleClientSite   methods  
	STDMETHOD(SaveObject)(void)   {return E_NOTIMPL;}  
	STDMETHOD(GetMoniker)(DWORD dwAssign,DWORD dwWhichMoniker,IMoniker** ppmk)  {return E_NOTIMPL;}  
	STDMETHOD(GetContainer)(IOleContainer** ppContainer) {return E_NOTIMPL;}  	
	STDMETHOD(ShowObject)(void) {return E_NOTIMPL;}  
	STDMETHOD(OnShowWindow)(BOOL fShow) {return E_NOTIMPL;}  
	STDMETHOD(RequestNewObjectLayout)(void) {return E_NOTIMPL;}   

	// IDocHostUIHandler
public:
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);	
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget,
		IOleInPlaceFrame *pFrame,
		IOleInPlaceUIWindow *pDoc);	
	STDMETHOD(HideUI)( void);	
	STDMETHOD(UpdateUI)( void);	
	STDMETHOD(EnableModeless)(BOOL fEnable);	
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate);	
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);	
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);	
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);	
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);	
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);	
	STDMETHOD(GetExternal)(IDispatch **ppDispatch);	
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);	
	STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);

public:
	// *** IOleCommandTarget ***
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);

protected:
	bool m_bLoadActiveX;
	CComBSTR m_strUserAgent;
	CComPtr<IDispatch> m_spExternal;
};

#endif //__TIPSCLIENTCSOPERATOR_H_
