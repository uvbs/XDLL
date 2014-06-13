
#pragma once

#include <boost/noncopyable.hpp>
#include <atlbase.h>

#include <wininet.h>
#include <afxinet.h>
#pragma comment(lib, "wininet.lib")

#include <atlcom.h>


class ATL_NO_VTABLE UrlDownloadCallback :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IBindStatusCallback, 
	public IHttpNegotiate
{

public:
	void SetUrl( const string& url )
	{
		m_url = url;
	}
	void SetReferer( const string& referer )
	{
		m_referer = referer;
	}
	void SetUserAgent( const string& userAgent )
	{
		m_userAgent = userAgent;
	}

BEGIN_COM_MAP(UrlDownloadCallback)
	COM_INTERFACE_ENTRY(IBindStatusCallback)
	COM_INTERFACE_ENTRY(IHttpNegotiate)
END_COM_MAP()

	UrlDownloadCallback()
	{
	}
	~UrlDownloadCallback()
	{
		ATLTRACE2(atlTraceControls,2,_T("~CBindStatusCallback\n"));
	}

	STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding *pBinding)
	{
		ATLTRACE2(atlTraceControls,2,_T("CBindStatusCallback::OnStartBinding\n"));
		return E_NOTIMPL;
	}

	STDMETHOD(GetPriority)(LONG *pnPriority)
	{
		ATLTRACE2(atlTraceControls,2,_T("CBindStatusCallback::GetPriority\n"));
		return E_NOTIMPL;
	}

	STDMETHOD(OnLowResource)(DWORD reserved)
	{
		ATLTRACE2(atlTraceControls,2,_T("CBindStatusCallback::OnLowResource\n"));
		return E_NOTIMPL;
	}

	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		ATLTRACE(_T("CBindStatusCallback::OnProgress %u %u %u %ls\n"), ulProgress, ulProgressMax, ulStatusCode, szStatusText);
		if ( BINDSTATUS_ENDDOWNLOADDATA == ulStatusCode )
		{
			BOOL res = ::DeleteUrlCacheEntry( m_url.c_str() );
			ATLTRACE(_T("CBindStatusCallback::OnProgress complete %hs %d\n"), m_url.c_str(), res);
		}
		return E_NOTIMPL;
	}

	STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError)
	{
		ATLTRACE2(atlTraceControls,2,_T("CBindStatusCallback::OnStopBinding\n"));
		return E_NOTIMPL;
	}

	STDMETHOD(GetBindInfo)(DWORD *pgrfBINDF, BINDINFO *pbindInfo)
	{
		ATLTRACE2(atlTraceControls,2,_T("CBindStatusCallback::GetBindInfo\n"));
		return E_NOTIMPL;
	}

	STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed)
	{
		ATLTRACE2(atlTraceControls,2,_T("CBindStatusCallback::OnDataAvailable\n"));
		return E_NOTIMPL;
	}

	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown *punk)
	{
		ATLTRACE2(atlTraceControls,2,_T("CBindStatusCallback::OnObjectAvailable\n"));
		return E_NOTIMPL;
	}

	STDMETHOD(BeginningTransaction)(LPCWSTR szURL, LPCWSTR szHeaders, DWORD dwReserved, LPWSTR *pszAdditionalHeaders)
	{
		if ( m_referer.empty() && m_userAgent.empty() )
			return S_OK;
		if (!pszAdditionalHeaders)
		{
			return E_POINTER;
		}

		*pszAdditionalHeaders = NULL;

		// This header is required when performing a POST operation
		std::wstring buf;
		buf.resize( m_referer.size() + m_userAgent.size() + 256 );
		int len = 0;
		if ( m_userAgent.empty() )
		{
			assert( !m_referer.empty() );
			len = _snwprintf( &buf[0], buf.size() - 1, L"Referer: %hs\r\n", m_referer.c_str() );
		}
		else if ( m_referer.empty() )
		{
			assert( !m_userAgent.empty() );
			len = _snwprintf( &buf[0], buf.size() - 1, L"User-Agent: %hs\r\n", m_userAgent.c_str() );
		}
		else
		{
			assert( !m_referer.empty() && !m_userAgent.empty() );
			len = _snwprintf( &buf[0], buf.size() - 1, L"Referer: %hs\r\nUser-Agent: %hs\r\n", m_referer.c_str(), m_userAgent.c_str() );
		}
		if ( len <= 0 )
			return E_INVALIDARG;
		buf.resize( len );

		LPWSTR wszAdditionalHeaders = (LPWSTR)CoTaskMemAlloc((buf.size()+1) *sizeof(WCHAR));
		if (!wszAdditionalHeaders)
		{
			return E_OUTOFMEMORY;
		}

		wcscpy(wszAdditionalHeaders, buf.c_str());
		*pszAdditionalHeaders = wszAdditionalHeaders;
		return S_OK;
	}
	STDMETHOD(OnResponse)(DWORD dwResponseCode, LPCWSTR szResponseHeaders, LPCWSTR szRequestHeaders, LPWSTR *pszAdditionalRequestHeaders)
	{
		return E_NOTIMPL;
	}

protected:
	CComPtr<IBinding> m_spBinding;
	std::string m_url;
	std::string m_referer;
	std::string m_userAgent;
};

const size_t NOPOS = -1;


//            synacast://www.pplive.com:554/xxxx/xxx/xx.exe?y1=a&y2=b#poiasdygopaisdugopasidjfopi
//                      |     domain   |   |        |      |         |
//                      |              |port        +-file-+         |
//                      |     host         |               |         |
//                                         | <--path---->  |<-param->|
//                                         | <---------request------>|
//                                                  |<--filerequest->|
//            <------refer---------------->|
//            <----------------fileurl-------------------->|
class Uri
{
public:
    Uri(const string &url)
        : url_(url),
        protcalpos_(NOPOS),
        host_start_pos_(NOPOS),
        host_end_pos_(NOPOS),
        port_start_pos_(NOPOS),
        path_start_pos_(NOPOS),
        file_start_pos_(NOPOS),
        param_start_pos_(NOPOS),
        param_end_pos_(NOPOS),
        domain_end_pos_(NOPOS),
        port_end_pos_(NOPOS),
        file_end_pos_(NOPOS),
        path_end_pos_(NOPOS),
        request_start_pos_(NOPOS),
        request_end_pos_(NOPOS)
    {
        parseurl();
    }

    Uri(const char *url)
        : url_(url),
        protcalpos_(NOPOS),
        host_start_pos_(NOPOS),
        host_end_pos_(NOPOS),
        port_start_pos_(NOPOS),
        path_start_pos_(NOPOS),
        file_start_pos_(NOPOS),
        param_start_pos_(NOPOS),
        param_end_pos_(NOPOS),
        domain_end_pos_(NOPOS),
        port_end_pos_(NOPOS),
        file_end_pos_(NOPOS),
        path_end_pos_(NOPOS),
        request_start_pos_(NOPOS),
        request_end_pos_(NOPOS)
    {
        parseurl();
    }

    bool replacepath(string file_name)
    {
        url_ = getreferer() + file_name;
        return true;
    }

    bool replacefile(string path_name)
    {
        if (file_start_pos_ != NOPOS)
        {
            url_ = url_.substr(0, file_start_pos_) + path_name;

            return true;
        }
        else
            return false;
    }

    string getfileurl() const
    {
        if (file_end_pos_ != NOPOS)
            return url_.substr(0, file_end_pos_);
        else
            return "";
    }

    string getrequest() const
    {
        if (request_start_pos_ != NOPOS)
            return url_.substr(request_start_pos_, request_end_pos_ - request_start_pos_);
        else
            return "";
    }

    string geturl() const
    {
        return url_;
    }

    string getprotocol() const
    {
        if(protcalpos_ != NOPOS)
            return url_.substr(0, protcalpos_);
        else
            return "http";
    }

    string getdomain() const
    {
        if(host_start_pos_!= NOPOS)
            return url_.substr(host_start_pos_, domain_end_pos_- host_start_pos_);
        else
            return "";
    }

    string getport() const
    {
        if(port_start_pos_ != NOPOS)
            return url_.substr(port_start_pos_, port_end_pos_ - port_start_pos_);
        else
            return "80";
    }

    string getpath() const
    {
        if(path_start_pos_ != NOPOS)
            return url_.substr(path_start_pos_, path_end_pos_ - path_start_pos_);
        else
            return "/";
    }

    string getfile() const
    {
        if(file_start_pos_ != NOPOS)
            return url_.substr(file_start_pos_, file_end_pos_ - file_start_pos_);
        else
            return "";
    }

    string getfilerequest() const
    {
        if(file_start_pos_ != NOPOS)
            return url_.substr(file_start_pos_, request_end_pos_ - file_start_pos_);
        else
            return "";
    }

    string getparameter(const string& key) const
    {
        string para = getparameter();
        size_t startp_, endp_;

        para = "&" + para;
        string key1 = "&" + key + "=";

        if((startp_ = para.find(key1)) == string::npos)
            return "";
        else
        {
            startp_ = para.find('=', startp_) + 1;
            endp_ = para.find('&', startp_);
            if (endp_ == string::npos)
                endp_ = para.length();
            return para.substr(startp_, endp_ - startp_);
        }
    }

    string getparameter() const
    {
        if(param_start_pos_ != NOPOS)
            return url_.substr(param_start_pos_, param_end_pos_ - param_start_pos_);
        else
            return "";
    }

    string gethost() const
    {
        if (host_start_pos_ != NOPOS)
            return url_.substr(host_start_pos_, host_end_pos_ - host_start_pos_);
        else
            return "";
    }

    string getreferer() const
    {
        if (host_end_pos_ != NOPOS)
            return url_.substr(0, host_end_pos_);
        else
            return "";
    }

private:
    void parseurl()
    {
        string::size_type beginpos = 0, endpos = 0;

        beginpos = url_.find(':', 0);
        endpos = url_.find('/', 0);
        if(beginpos != string::npos && beginpos < endpos && beginpos < url_.find('.', 0))
        {
            protcalpos_ = beginpos;
            beginpos = url_.find('/', endpos + 1) + 1;
            host_start_pos_ = beginpos;
        }
        else
        {
            host_start_pos_ = 0;
            beginpos = host_start_pos_;
        }

        string s = url_.substr(0, url_.find('/', beginpos));
        endpos = s.find(':', beginpos);
        if(endpos != string::npos)
        {
            port_start_pos_ = endpos + 1;
            beginpos = endpos;
            domain_end_pos_ = endpos;
        }

        endpos = url_.find('/', beginpos);
        if(endpos != string::npos)
        {
            if (domain_end_pos_ == NOPOS)
                domain_end_pos_ = endpos;
            if (port_start_pos_ != NOPOS)
                port_end_pos_ = endpos;
            path_start_pos_ = endpos;
            request_start_pos_ = endpos;
            host_end_pos_ = endpos;
        }
        else
        {
            url_ = url_ + '/';
            endpos = url_.find('/', beginpos);
            path_start_pos_ = endpos;
            if (domain_end_pos_ == NOPOS)
                domain_end_pos_ = url_.length() - 1;
            if (port_start_pos_ != NOPOS)
                port_end_pos_ = endpos;
            request_start_pos_ = endpos;
            host_end_pos_ = endpos;
        }

        beginpos = endpos;

        //endpos = url_.find('.', beginpos);
        //if(endpos != string::npos)
        //{
        //    beginpos = url_.rfind('/', endpos) + 1;
        //    file_start_pos_ = beginpos;
        //}

        beginpos = url_.find('?', beginpos);
        if(beginpos != string::npos)
        {
            param_start_pos_ = beginpos + 1;
            file_end_pos_ = beginpos;
            path_end_pos_ = beginpos;

            file_start_pos_= url_.rfind('/', beginpos) + 1;

        } else
        {
            file_end_pos_ = url_.length();
            path_end_pos_ = url_.length();

            file_start_pos_= url_.rfind('/', url_.length()) + 1;
        }

        beginpos = url_.rfind('#', url_.length() - 1);
        if(beginpos != string::npos)
        {
            request_end_pos_ = beginpos;
            param_end_pos_ = beginpos;
        } else
        {
            request_end_pos_ = url_.length();
            param_end_pos_ = url_.length();
        }
    }

private:
    string url_;
    size_t protcalpos_;
    size_t host_start_pos_;
    size_t host_end_pos_;
    size_t domain_end_pos_;
    size_t port_start_pos_;
    size_t port_end_pos_;
    size_t path_start_pos_;
    size_t path_end_pos_;
    size_t file_start_pos_;
    size_t file_end_pos_;
    size_t param_start_pos_;
    size_t param_end_pos_;
    size_t request_start_pos_;
    size_t request_end_pos_;
};

class WebUtils
{
public:
	/// 访问某个url，忽略内容
	static bool ClickUrl( const string& url )
	{
		TCHAR filename[MAX_PATH * 4 + 1] = { 0 };
		APP_EVENT("normal url download to file "<<url);
		BOOL res = ::DeleteUrlCacheEntry( url.c_str() );
		HRESULT hr = ::URLDownloadToCacheFileA( NULL, url.c_str(), filename, MAX_PATH * 4, 0, NULL );
		if ( FAILED(hr) )
			return false;
		return true;
	}

	/// 使用referer访问某个url，忽略内容
	static bool ClickUrl( const string& url, const string& referer )
	{
		CComObject<UrlDownloadCallback> *pbsc = NULL;
		APP_EVENT("normal url download to file "<<url<<"     referer:"<<referer);
		HRESULT hRes = CComObject<UrlDownloadCallback>::CreateInstance(&pbsc);
		if ( FAILED(hRes) )
		{
			assert( false );
			return ClickUrl( url );
		}
		pbsc->SetUrl( url );
		pbsc->SetReferer( referer );
		//pbsc->SetUserAgent( "NetMe" );
		TCHAR filename[MAX_PATH * 4 + 1] = { 0 };
		HRESULT hr = ::URLDownloadToCacheFileA( NULL, url.c_str(), filename, MAX_PATH * 4, 0, pbsc );
		if ( FAILED(hr) )
			return false;
		BOOL res = ::DeleteUrlCacheEntry( url.c_str() );
		APP_EVENT("normal url download succeed "<<url<<"     referer:"<<referer);
		//assert(res);
		return true;
	}

	static bool PostUrl( const string& url, const string& referer, const string& content, const string& contentType)
	{
		Uri uri(url);
		CInternetSession	session("My Session");   
		CHttpConnection*	pServer = NULL;
		CHttpFile*			pFile = NULL;   
		CString				ServerName = uri.getdomain().c_str();
        INTERNET_PORT		nPort = atoi(uri.getport().c_str());   
		DWORD				retcode;   

		try   
		{   
			pServer	= session.GetHttpConnection(ServerName,nPort);   
			pFile	= pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, uri.getrequest().c_str(), NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT);
			if(referer.size() > 0)
			{
				string headerReferer = "Referer: ";
				headerReferer.append(referer);
				pFile->AddRequestHeaders(headerReferer.c_str());
			}
			if(contentType.size() > 0)
			{
				string headerContentType = "Content-Type: ";
				headerContentType.append(contentType);
				pFile->AddRequestHeaders(headerContentType.c_str());
			}
			pFile->AddRequestHeaders("Accept: */*");
			pFile->SendRequest(NULL,0, (void *)content.c_str(), content.length());   
			pFile->QueryInfoStatusCode(retcode);   

			char   buf[10240];   
			memset(buf, 0, sizeof(buf));
			DWORD   dwread=pFile->Read(buf,10240); 
			APP_EVENT("WebUtils::PostUrl "<<buf);
		}
		catch(CInternetException* e){};
		delete pFile;  
		delete pServer;   
		session.Close();   

		return true;
	}
};








