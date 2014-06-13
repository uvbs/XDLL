#ifndef __IUri_FWD_DEFINED__
#define __IUri_FWD_DEFINED__
typedef interface IUri IUri;
#endif 	/* __IUri_FWD_DEFINED__ */

#ifndef __IUri_INTERFACE_DEFINED__
#define __IUri_INTERFACE_DEFINED__

/* interface IUri */
/* [unique][uuid][object] */ 

typedef /* [public][public][public][public][public][helpstring] */ 
enum __MIDL_IUri_0001
    {	Uri_PROPERTY_ABSOLUTE_URI	= 0,
	Uri_PROPERTY_STRING_START	= Uri_PROPERTY_ABSOLUTE_URI,
	Uri_PROPERTY_AUTHORITY	= ( Uri_PROPERTY_STRING_START + 1 ) ,
	Uri_PROPERTY_DISPLAY_URI	= ( Uri_PROPERTY_AUTHORITY + 1 ) ,
	Uri_PROPERTY_DOMAIN	= ( Uri_PROPERTY_DISPLAY_URI + 1 ) ,
	Uri_PROPERTY_EXTENSION	= ( Uri_PROPERTY_DOMAIN + 1 ) ,
	Uri_PROPERTY_FRAGMENT	= ( Uri_PROPERTY_EXTENSION + 1 ) ,
	Uri_PROPERTY_HOST	= ( Uri_PROPERTY_FRAGMENT + 1 ) ,
	Uri_PROPERTY_PASSWORD	= ( Uri_PROPERTY_HOST + 1 ) ,
	Uri_PROPERTY_PATH	= ( Uri_PROPERTY_PASSWORD + 1 ) ,
	Uri_PROPERTY_PATH_AND_QUERY	= ( Uri_PROPERTY_PATH + 1 ) ,
	Uri_PROPERTY_QUERY	= ( Uri_PROPERTY_PATH_AND_QUERY + 1 ) ,
	Uri_PROPERTY_RAW_URI	= ( Uri_PROPERTY_QUERY + 1 ) ,
	Uri_PROPERTY_SCHEME_NAME	= ( Uri_PROPERTY_RAW_URI + 1 ) ,
	Uri_PROPERTY_USER_INFO	= ( Uri_PROPERTY_SCHEME_NAME + 1 ) ,
	Uri_PROPERTY_USER_NAME	= ( Uri_PROPERTY_USER_INFO + 1 ) ,
	Uri_PROPERTY_STRING_LAST	= Uri_PROPERTY_USER_NAME,
	Uri_PROPERTY_HOST_TYPE	= ( Uri_PROPERTY_STRING_LAST + 1 ) ,
	Uri_PROPERTY_DWORD_START	= Uri_PROPERTY_HOST_TYPE,
	Uri_PROPERTY_PORT	= ( Uri_PROPERTY_DWORD_START + 1 ) ,
	Uri_PROPERTY_SCHEME	= ( Uri_PROPERTY_PORT + 1 ) ,
	Uri_PROPERTY_ZONE	= ( Uri_PROPERTY_SCHEME + 1 ) ,
	Uri_PROPERTY_DWORD_LAST	= Uri_PROPERTY_ZONE
    } 	Uri_PROPERTY;

typedef /* [public][helpstring] */ 
enum __MIDL_IUri_0002
    {	Uri_HOST_UNKNOWN	= 0,
	Uri_HOST_DNS	= ( Uri_HOST_UNKNOWN + 1 ) ,
	Uri_HOST_IPV4	= ( Uri_HOST_DNS + 1 ) ,
	Uri_HOST_IPV6	= ( Uri_HOST_IPV4 + 1 ) ,
	Uri_HOST_IDN	= ( Uri_HOST_IPV6 + 1 ) 
    } 	Uri_HOST_TYPE;


EXTERN_C const IID IID_IUri;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A39EE748-6A27-4817-A6F2-13914BEF5890")
    IUri : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPropertyBSTR( 
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */ BSTR *pbstrProperty,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPropertyLength( 
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */ DWORD *pcchProperty,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPropertyDWORD( 
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */ DWORD *pdwProperty,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE HasProperty( 
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */ BOOL *pfHasProperty) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetAbsoluteUri( 
            /* [out] */ BSTR *pbstrAbsoluteUri) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetAuthority( 
            /* [out] */ BSTR *pbstrAuthority) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDisplayUri( 
            /* [out] */ BSTR *pbstrDisplayString) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDomain( 
            /* [out] */ BSTR *pbstrDomain) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetExtension( 
            /* [out] */ BSTR *pbstrExtension) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFragment( 
            /* [out] */  BSTR *pbstrFragment) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetHost( 
            /* [out] */  BSTR *pbstrHost) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPassword( 
            /* [out] */  BSTR *pbstrPassword) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPath( 
            /* [out] */  BSTR *pbstrPath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPathAndQuery( 
            /* [out] */  BSTR *pbstrPathAndQuery) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetQuery( 
            /* [out] */  BSTR *pbstrQuery) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetRawUri( 
            /* [out] */  BSTR *pbstrRawUri) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSchemeName( 
            /* [out] */  BSTR *pbstrSchemeName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetUserInfo( 
            /* [out] */  BSTR *pbstrUserInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetUserName( 
            /* [out] */  BSTR *pbstrUserName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetHostType( 
            /* [out] */  DWORD *pdwHostType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPort( 
            /* [out] */  DWORD *pdwPort) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetScheme( 
            /* [out] */  DWORD *pdwScheme) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetZone( 
            /* [out] */  DWORD *pdwZone) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetProperties( 
            /* [out] */  LPDWORD pdwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsEqual( 
            /* [in] */  IUri *pUri,
            /* [out] */  BOOL *pfEqual) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUriVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUri * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUri * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUri * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPropertyBSTR )( 
            IUri * This,
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */  BSTR *pbstrProperty,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPropertyLength )( 
            IUri * This,
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */  DWORD *pcchProperty,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPropertyDWORD )( 
            IUri * This,
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */  DWORD *pdwProperty,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *HasProperty )( 
            IUri * This,
            /* [range][in] */ Uri_PROPERTY uriProp,
            /* [out] */  BOOL *pfHasProperty);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAbsoluteUri )( 
            IUri * This,
            /* [out] */  BSTR *pbstrAbsoluteUri);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAuthority )( 
            IUri * This,
            /* [out] */  BSTR *pbstrAuthority);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDisplayUri )( 
            IUri * This,
            /* [out] */  BSTR *pbstrDisplayString);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDomain )( 
            IUri * This,
            /* [out] */  BSTR *pbstrDomain);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetExtension )( 
            IUri * This,
            /* [out] */  BSTR *pbstrExtension);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetFragment )( 
            IUri * This,
            /* [out] */  BSTR *pbstrFragment);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetHost )( 
            IUri * This,
            /* [out] */  BSTR *pbstrHost);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPassword )( 
            IUri * This,
            /* [out] */  BSTR *pbstrPassword);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            IUri * This,
            /* [out] */  BSTR *pbstrPath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPathAndQuery )( 
            IUri * This,
            /* [out] */  BSTR *pbstrPathAndQuery);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetQuery )( 
            IUri * This,
            /* [out] */  BSTR *pbstrQuery);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetRawUri )( 
            IUri * This,
            /* [out] */  BSTR *pbstrRawUri);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSchemeName )( 
            IUri * This,
            /* [out] */  BSTR *pbstrSchemeName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetUserInfo )( 
            IUri * This,
            /* [out] */  BSTR *pbstrUserInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetUserName )( 
            IUri * This,
            /* [out] */  BSTR *pbstrUserName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetHostType )( 
            IUri * This,
            /* [out] */  DWORD *pdwHostType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPort )( 
            IUri * This,
            /* [out] */  DWORD *pdwPort);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetScheme )( 
            IUri * This,
            /* [out] */  DWORD *pdwScheme);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetZone )( 
            IUri * This,
            /* [out] */  DWORD *pdwZone);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProperties )( 
            IUri * This,
            /* [out] */  LPDWORD pdwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsEqual )( 
            IUri * This,
            /* [in] */  IUri *pUri,
            /* [out] */  BOOL *pfEqual);
        
        END_INTERFACE
    } IUriVtbl;

    interface IUri
    {
        CONST_VTBL struct IUriVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUri_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUri_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUri_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUri_GetPropertyBSTR(This,uriProp,pbstrProperty,dwFlags)	\
    ( (This)->lpVtbl -> GetPropertyBSTR(This,uriProp,pbstrProperty,dwFlags) ) 

#define IUri_GetPropertyLength(This,uriProp,pcchProperty,dwFlags)	\
    ( (This)->lpVtbl -> GetPropertyLength(This,uriProp,pcchProperty,dwFlags) ) 

#define IUri_GetPropertyDWORD(This,uriProp,pdwProperty,dwFlags)	\
    ( (This)->lpVtbl -> GetPropertyDWORD(This,uriProp,pdwProperty,dwFlags) ) 

#define IUri_HasProperty(This,uriProp,pfHasProperty)	\
    ( (This)->lpVtbl -> HasProperty(This,uriProp,pfHasProperty) ) 

#define IUri_GetAbsoluteUri(This,pbstrAbsoluteUri)	\
    ( (This)->lpVtbl -> GetAbsoluteUri(This,pbstrAbsoluteUri) ) 

#define IUri_GetAuthority(This,pbstrAuthority)	\
    ( (This)->lpVtbl -> GetAuthority(This,pbstrAuthority) ) 

#define IUri_GetDisplayUri(This,pbstrDisplayString)	\
    ( (This)->lpVtbl -> GetDisplayUri(This,pbstrDisplayString) ) 

#define IUri_GetDomain(This,pbstrDomain)	\
    ( (This)->lpVtbl -> GetDomain(This,pbstrDomain) ) 

#define IUri_GetExtension(This,pbstrExtension)	\
    ( (This)->lpVtbl -> GetExtension(This,pbstrExtension) ) 

#define IUri_GetFragment(This,pbstrFragment)	\
    ( (This)->lpVtbl -> GetFragment(This,pbstrFragment) ) 

#define IUri_GetHost(This,pbstrHost)	\
    ( (This)->lpVtbl -> GetHost(This,pbstrHost) ) 

#define IUri_GetPassword(This,pbstrPassword)	\
    ( (This)->lpVtbl -> GetPassword(This,pbstrPassword) ) 

#define IUri_GetPath(This,pbstrPath)	\
    ( (This)->lpVtbl -> GetPath(This,pbstrPath) ) 

#define IUri_GetPathAndQuery(This,pbstrPathAndQuery)	\
    ( (This)->lpVtbl -> GetPathAndQuery(This,pbstrPathAndQuery) ) 

#define IUri_GetQuery(This,pbstrQuery)	\
    ( (This)->lpVtbl -> GetQuery(This,pbstrQuery) ) 

#define IUri_GetRawUri(This,pbstrRawUri)	\
    ( (This)->lpVtbl -> GetRawUri(This,pbstrRawUri) ) 

#define IUri_GetSchemeName(This,pbstrSchemeName)	\
    ( (This)->lpVtbl -> GetSchemeName(This,pbstrSchemeName) ) 

#define IUri_GetUserInfo(This,pbstrUserInfo)	\
    ( (This)->lpVtbl -> GetUserInfo(This,pbstrUserInfo) ) 

#define IUri_GetUserName(This,pbstrUserName)	\
    ( (This)->lpVtbl -> GetUserName(This,pbstrUserName) ) 

#define IUri_GetHostType(This,pdwHostType)	\
    ( (This)->lpVtbl -> GetHostType(This,pdwHostType) ) 

#define IUri_GetPort(This,pdwPort)	\
    ( (This)->lpVtbl -> GetPort(This,pdwPort) ) 

#define IUri_GetScheme(This,pdwScheme)	\
    ( (This)->lpVtbl -> GetScheme(This,pdwScheme) ) 

#define IUri_GetZone(This,pdwZone)	\
    ( (This)->lpVtbl -> GetZone(This,pdwZone) ) 

#define IUri_GetProperties(This,pdwFlags)	\
    ( (This)->lpVtbl -> GetProperties(This,pdwFlags) ) 

#define IUri_IsEqual(This,pUri,pfEqual)	\
    ( (This)->lpVtbl -> IsEqual(This,pUri,pfEqual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUri_INTERFACE_DEFINED__ */















#ifndef __IInternetProtocolEx_FWD_DEFINED__
#define __IInternetProtocolEx_FWD_DEFINED__
typedef interface IInternetProtocolEx IInternetProtocolEx;
#endif 	/* __IInternetProtocolEx_FWD_DEFINED__ */

#ifndef _LPIINTERNETPROTOCOLEX_DEFINED
#define _LPIINTERNETPROTOCOLEX_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0025_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0025_v0_0_s_ifspec;

#ifndef __IInternetProtocolEx_INTERFACE_DEFINED__
#define __IInternetProtocolEx_INTERFACE_DEFINED__

/* interface IInternetProtocolEx */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IInternetProtocolEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7A98E66-1010-492c-A1C8-C809E1F75905")
    IInternetProtocolEx : public IInternetProtocol
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE StartEx( 
            /* [in] */ IUri *pUri,
            /* [in] */ IInternetProtocolSink *pOIProtSink,
            /* [in] */ IInternetBindInfo *pOIBindInfo,
            /* [in] */ DWORD grfPI,
            /* [in] */ HANDLE_PTR dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetProtocolExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetProtocolEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetProtocolEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetProtocolEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            IInternetProtocolEx * This,
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IInternetProtocolSink *pOIProtSink,
            /* [in] */ IInternetBindInfo *pOIBindInfo,
            /* [in] */ DWORD grfPI,
            /* [in] */ HANDLE_PTR dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *Continue )( 
            IInternetProtocolEx * This,
            /* [in] */ PROTOCOLDATA *pProtocolData);
        
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IInternetProtocolEx * This,
            /* [in] */ HRESULT hrReason,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            IInternetProtocolEx * This,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IInternetProtocolEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *Resume )( 
            IInternetProtocolEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *Read )( 
            IInternetProtocolEx * This,
            /* [length_is][size_is][out][in] */ void *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG *pcbRead);
        
        HRESULT ( STDMETHODCALLTYPE *Seek )( 
            IInternetProtocolEx * This,
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [out] */ ULARGE_INTEGER *plibNewPosition);
        
        HRESULT ( STDMETHODCALLTYPE *LockRequest )( 
            IInternetProtocolEx * This,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *UnlockRequest )( 
            IInternetProtocolEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *StartEx )( 
            IInternetProtocolEx * This,
            /* [in] */ IUri *pUri,
            /* [in] */ IInternetProtocolSink *pOIProtSink,
            /* [in] */ IInternetBindInfo *pOIBindInfo,
            /* [in] */ DWORD grfPI,
            /* [in] */ HANDLE_PTR dwReserved);
        
        END_INTERFACE
    } IInternetProtocolExVtbl;

    interface IInternetProtocolEx
    {
        CONST_VTBL struct IInternetProtocolExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetProtocolEx_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInternetProtocolEx_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInternetProtocolEx_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInternetProtocolEx_Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)	\
    ( (This)->lpVtbl -> Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved) ) 

#define IInternetProtocolEx_Continue(This,pProtocolData)	\
    ( (This)->lpVtbl -> Continue(This,pProtocolData) ) 

#define IInternetProtocolEx_Abort(This,hrReason,dwOptions)	\
    ( (This)->lpVtbl -> Abort(This,hrReason,dwOptions) ) 

#define IInternetProtocolEx_Terminate(This,dwOptions)	\
    ( (This)->lpVtbl -> Terminate(This,dwOptions) ) 

#define IInternetProtocolEx_Suspend(This)	\
    ( (This)->lpVtbl -> Suspend(This) ) 

#define IInternetProtocolEx_Resume(This)	\
    ( (This)->lpVtbl -> Resume(This) ) 


#define IInternetProtocolEx_Read(This,pv,cb,pcbRead)	\
    ( (This)->lpVtbl -> Read(This,pv,cb,pcbRead) ) 

#define IInternetProtocolEx_Seek(This,dlibMove,dwOrigin,plibNewPosition)	\
    ( (This)->lpVtbl -> Seek(This,dlibMove,dwOrigin,plibNewPosition) ) 

#define IInternetProtocolEx_LockRequest(This,dwOptions)	\
    ( (This)->lpVtbl -> LockRequest(This,dwOptions) ) 

#define IInternetProtocolEx_UnlockRequest(This)	\
    ( (This)->lpVtbl -> UnlockRequest(This) ) 


#define IInternetProtocolEx_StartEx(This,pUri,pOIProtSink,pOIBindInfo,grfPI,dwReserved)	\
    ( (This)->lpVtbl -> StartEx(This,pUri,pOIProtSink,pOIBindInfo,grfPI,dwReserved) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */
#endif 	/* __IInternetProtocolEx_INTERFACE_DEFINED__ */
#endif
