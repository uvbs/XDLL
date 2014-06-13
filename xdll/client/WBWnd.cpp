// WBWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Xdll.h"
#include "WBWnd.h"
#include "DOMExtractor.h"
#include <ppl/util/macro.h>
#include <ppl/util/log.h>
#include <vector>
#include <string>
#include <ppl/stl/stream.h>
#include <windows.h>
#include <Mmsystem.h>

#include "TipsClient.h"
using std::make_pair;
#include "gstring.h"
#include "TipsConvert.h"
#include "PlatformUA.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWBWnd

CWBWnd::CWBWnd(TipsClient& tipClient)
: m_tipsClient(tipClient)
, m_nDDTimer(0)
, m_nClickTimer(0)
, m_rx(0)
, m_ry(0)
{
}

CWBWnd::~CWBWnd()
{
}

BEGIN_MESSAGE_MAP(CWBWnd, CWnd)
	//{{AFX_MSG_MAP(CWBWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CWBWnd, CWnd)
ON_EVENT(CWBWnd, IDC_WEB, 250 /* BeforeNavigate2 */, OnBeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
ON_EVENT(CWBWnd, IDC_WEB, 259 /* DocumentComplete */, OnDocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
ON_EVENT(CWBWnd, IDC_WEB, 273 /* NewWindow3 */, OnNewWindow3Explorer1, VTS_PDISPATCH VTS_PBOOL  VTS_I4 VTS_BSTR VTS_BSTR)
ON_EVENT(CWBWnd, IDC_WEB, 251 /* NewWindow2 */, OnNewWindow2Explorer1, VTS_PDISPATCH VTS_PBOOL)
ON_EVENT(CWBWnd, IDC_WEB, 106 /* DownloadBegin */, OnDownloadBeginExplorer1, VTS_NONE)
ON_EVENT(CWBWnd, IDC_WEB, 270 /* FileDownload */, OnFileDownloadExplorer1, VTS_BOOL VTS_PBOOL)
ON_EVENT(CWBWnd, IDC_WEB, 252 /* NavigateComplete2 */, OnNavigateComplete2Explorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWBWnd message handlers
const int CWBWnd::MIN_BROWSER_WIDTH = 800;
const int CWBWnd::MIN_BROWSER_HEIGHT = 600;

int CWBWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

int CWBWnd::SetXmlWidth(int UaMinWidth, int UaMaxWidth)
{
	int nXmlWidth = 0;

	if (0 > UaMaxWidth - UaMinWidth)
	{
		nXmlWidth = 0;
	}
	else if (0 == UaMaxWidth - UaMinWidth)
	{
		nXmlWidth = UaMinWidth;
	}
	else 
	{
		srand(GetTickCount());
		nXmlWidth = UaMinWidth + rand()%(UaMaxWidth - UaMinWidth);
	}

	return nXmlWidth;
}

int CWBWnd::SetXmlHeight(int UaMinHeight, int UaMaxHeight)
{
	int  nXmlHeight = 0;

	if ( 0 > UaMaxHeight - UaMinHeight)
	{
		nXmlHeight = 0;
	}
	else if (0 == UaMaxHeight - UaMinHeight)
	{
		nXmlHeight = UaMinHeight;
	}
	else
	{
		srand(GetTickCount());
		nXmlHeight = UaMinHeight + rand()%(UaMaxHeight-UaMinHeight);
	}

	return nXmlHeight;
}

int CWBWnd::SetRandomWidth()
{
	int nRandomWidth = MIN_BROWSER_WIDTH;
	
	if (m_sx > MIN_BROWSER_WIDTH)
	{
		srand(GetTickCount());
		nRandomWidth = MIN_BROWSER_WIDTH + rand()%(m_sx - MIN_BROWSER_WIDTH);
	}
	
	return nRandomWidth;
}

int CWBWnd::SetRandomHeight()
{
	int nRandomHeight = MIN_BROWSER_HEIGHT;

	if (m_sy > MIN_BROWSER_HEIGHT) 
	{
		srand(GetTickCount());
		nRandomHeight = MIN_BROWSER_HEIGHT + rand()%(m_sy - MIN_BROWSER_HEIGHT);
	}

	return nRandomHeight;
}

BOOL	CWBWnd::Initialize()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString	strMyClass = AfxRegisterWndClass(
							CS_VREDRAW | CS_HREDRAW,
							::LoadCursor(NULL, IDC_ARROW),
							(HBRUSH) ::GetStockObject(WHITE_BRUSH),
							::LoadIcon(NULL, IDI_APPLICATION));

	//获取屏幕分辨率
	m_sx = ::GetSystemMetrics(SM_CXSCREEN);
	m_sy = ::GetSystemMetrics(SM_CYSCREEN);
	
	//设置移动端分辨率，在HOOK中修改
	MultiHopInfoPtr urlInfo = m_tipsClient.GetUrlInfo();
	if (NULL != urlInfo.get())
	{
		m_rx = SetXmlWidth(urlInfo->UaMinWidth, urlInfo->UaMaxWidth);
		m_ry = SetXmlHeight(urlInfo->UaMinHeight, urlInfo->UaMaxHeight);
		g_screenWidth = m_rx;
		g_screenHeight = m_ry;
	}

	//如果platform未设置，则随机Windows PC端的浏览器大小
	if (m_rx <= 0 || m_ry <= 0)
	{
		m_rx = SetRandomWidth();
		m_ry = SetRandomHeight();
		g_screenWidth = 0;
		g_screenHeight = 0;
	}

 	CRect rect(0, 0, 0, 0);

	if (!CWnd::CreateEx(0, strMyClass, _T("WBWnd"), WS_OVERLAPPEDWINDOW, rect, 
		NULL, 0, NULL ))
		return FALSE;

	CreateWB();

	assert(::IsWindow(m_hWnd));

#ifdef _SHOW_BROWSER
	//this->ShowWindow(SW_SHOWNORMAL);
	//ShowWindow(SW_SHOW);
	::SetWindowPos(m_hWnd, HWND_TOP, 100, 100, m_rx, m_ry, SWP_SHOWWINDOW);
#else 
	//ShowWindow(SW_HIDE);
	::SetWindowPos(m_hWnd, HWND_BOTTOM, m_sx*2, m_sy*2, m_rx, m_ry, SWP_HIDEWINDOW);
#endif

	return TRUE;
}

BOOL	CWBWnd::CreateWB()
{
	if (m_WB.m_hWnd)
	{
		assert( ::IsWindow( m_WB.m_hWnd ) );
		return TRUE;
	}

	APP_EVENT("CWBWnd::CreateWB m_WB.m_hWnd="<<m_WB.m_hWnd);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect rect(0, 0, 0, 0);

 	if (!m_WB.Create(_T(""), _T("WB"), WS_CHILD|WS_VISIBLE, 
 			rect, this, IDC_WEB, NULL))
 		return FALSE;
	//if (!m_WB.Create(0, _T("TipsClient"), WS_CHILD|WS_VISIBLE, 
	//		rect, this, IDC_WEB, NULL))
	//	return FALSE;

	assert( ::IsWindow( m_WB.m_hWnd ) );

	TRY
	{
		m_WB.SetSilent(TRUE);

		//定制IE执行脚本样式
		CComQIPtr<IOleClientSite>	spOleCs;
		CComQIPtr<IOleObject>		spOleOb;
		LPDISPATCH	lpDisp = m_WB.GetApplication();
		if (lpDisp)
		{
			spOleOb = lpDisp;
			lpDisp->Release();
		}
		if (spOleOb)
		{
			CTipsClientCsOperator::CreateInstance(&m_spOleCs);
			if (m_spOleCs)
			{
				CComQIPtr<IOleClientSite>	spOleCs = m_spOleCs;
				spOleOb->SetClientSite(spOleCs);
				return TRUE;
			}
		}
	}
	CATCH_ALL(e)
	{
		assert(false);
		e->Delete();
	}
	END_CATCH_ALL

 	if (m_WB.m_hWnd)
 	{
 		m_WB.DestroyWindow();
 	}
	return FALSE;
}

void	CWBWnd::OnBeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel)
{
	TRY
	{
		if ( !URL || URL->vt != VT_BSTR || !URL->bstrVal )
		{
			assert(false);
			(*Cancel) = TRUE;
			return;
		}

		CComBSTR httpPattern = "http://";
		CComBSTR httpsPattern = "https://";
		CComBSTR httpAboutBlank = "about:blank";
		CComBSTR httpJavascript = "javascript:";
		CComBSTR httpUrlStrVal = URL->bstrVal;
		if (
			(httpUrlStrVal.Length()>=httpPattern.Length() && 0 == _wcsnicmp(httpUrlStrVal, httpPattern, httpPattern.Length())) ||
			(httpUrlStrVal.Length()>=httpsPattern.Length() && 0 == _wcsnicmp(httpUrlStrVal, httpsPattern, httpsPattern.Length())) ||
			(httpUrlStrVal.Length()>=httpAboutBlank.Length() && 0 == _wcsnicmp(httpUrlStrVal, httpAboutBlank, httpAboutBlank.Length())) || 
			(httpUrlStrVal.Length()>=httpJavascript.Length() && 0 == _wcsnicmp(httpUrlStrVal, httpJavascript, httpJavascript.Length()))
		)
		{
		}
		else
		{
			APP_EVENT("OnBeforeNavigate2 Not HTTP Request So Cancel "<<(LPCTSTR)CString(httpUrlStrVal));
			(*Cancel) = TRUE;
		}

		return;
	}
	CATCH_ALL(e)
	{
		assert(false);
		e->Delete();
	}
	END_CATCH_ALL
}

void	CWBWnd::OnNewWindow2Explorer1(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel)
{
	//这里可能有问题!!!!
	APP_EVENT("OnNewWindow2");
	if(g_bIsExit == true)
	{
		*Cancel = TRUE;
		return;
	}
}

void	CWBWnd::OnNewWindow3Explorer1(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel,  DWORD dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl)
{
	// 这里可能有问题!!!!
	APP_EVENT("OnNewWindow3 "<<bstrUrl);
	*ppDisp = m_WB.GetApplication();

	TRY
	{
		int isOpenWindow = m_tipsClient.GetUrlInfo()->GetIsOpenWindow();
		if ( isOpenWindow == 1)
		{	// 表示需要弹出窗口
			APP_EVENT("Because isOpenWindow == 1,so create new browser");
			boost::shared_ptr<CWBWnd> newCWBWnd = m_tipsClient.NewBrowser();
			newCWBWnd->EnadbleConfigs();
			*ppDisp = newCWBWnd->m_WB.GetApplication();
		}
		else
		{	// 不弹出窗口，还是用原来的窗口接管
			APP_EVENT("Because isOpenWindow != 1,so use old browser");
			
			// 在当前浏览器窗口中打开
			//IUnknown* pUnk = m_WB.GetControlUnknown(); 
			//if(pUnk) 
			//{ 
			//	IDispatch* pDisp = NULL; 
			//	HRESULT hr = pUnk-> QueryInterface(IID_IDispatch,(void**)&pDisp); 
			//	if(SUCCEEDED(hr)&&pDisp) 
			//		*ppDisp = pDisp; 
			//} 
			//*Cancel   =   false;  
			*ppDisp = m_WB.GetApplication();
			m_WB.Navigate(bstrUrl, NULL, NULL, NULL, NULL);
		}
	}
	CATCH_ALL(e)
	{
		assert(false);
		e->Delete();
		return;
	}
	END_CATCH_ALL
}

void	CWBWnd::OnNavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
	if ( !URL || URL->vt != VT_BSTR || !URL->bstrVal )
	{
		return;
	}

	TRY
	{
		APP_EVENT("OnNavigateComplete "<<(LPCTSTR)CString(URL->bstrVal) );
		LPDISPATCH lpDisp = m_WB.GetDocument();
		CComQIPtr<IHTMLDocument2>	spDoc2(lpDisp);
		SAFE_RELEASE(lpDisp);
		if (!spDoc2)
		{
			return;
		}

		HRESULT hr;
		CComQIPtr<IHTMLWindow2> spWindows;
		hr = spDoc2->get_parentWindow(&spWindows);
		if( FAILED(hr) )
		{
			APP_ERROR("IHTMLWindow2 get_parentWindow Error");
		}
 		VARIANT v;
 		hr = spWindows->execScript(CComBSTR(gs[SCRIPT_1].c_str()), CComBSTR(_T("javascript")), &v);
 		if( FAILED(hr) ) { APP_ERROR("spWindows execScript alert Error"); }
 		hr = spWindows->execScript(CComBSTR(gs[SCRIPT_2].c_str()), CComBSTR(_T("javascript")), &v);
 		if( FAILED(hr) ) { APP_ERROR("spWindows execScript confirm Error"); }
 		hr = spWindows->execScript(CComBSTR(gs[SCRIPT_3].c_str()), CComBSTR(_T("javascript")), &v);
 		if( FAILED(hr) ) { APP_ERROR("spWindows execScript prompt Error"); }
 		hr = spWindows->execScript(CComBSTR(gs[SCRIPT_4].c_str()), CComBSTR(_T("javascript")), &v);
 		if( FAILED(hr) ) { APP_ERROR("spWindows execScript showModalDialog Error"); }
 		hr = spWindows->execScript(CComBSTR(gs[SCRIPT_5].c_str()), CComBSTR(_T("javascript")), &v);
 		if( FAILED(hr) ) { APP_ERROR("spWindows execScript showModelessDialog Error"); }
		hr = spWindows->execScript(CComBSTR(gs[SCRIPT_6].c_str()), CComBSTR(_T("javascript")), &v);
		if( FAILED(hr) ) { APP_ERROR("spWindows execScript print Error"); }

		CComQIPtr<IDispatch> lpMainDisp = m_WB.GetControlUnknown();
		if( pDisp == lpMainDisp )
		{
			// 是主文档
			m_nDDTimer = SetTimer(1, 30*1000, NULL);
			m_mainUrl = CString(URL->bstrVal);
		}

		SAFE_RELEASE(lpDisp);

		APP_EVENT("OnNavigateComplete END "<<(LPCTSTR)CString(URL->bstrVal) );
	}
	CATCH_ALL(e)
	{
		assert(false);
		e->Delete();
	}
	END_CATCH_ALL
}

//////////////////////////////////////////////////////////////////////////
// LogicalDocumentComplete 只得是人为 认为的 页面DOM加载完成
//     在这里具体指得是  真的DocumentComplete 或者 30s超时到了
void	CWBWnd::LogicalDocumentComplete(CString url)
{
	APP_EVENT("LogicalDocumentComplete "<<(LPCTSTR)url);
	TRY {
		DWORD clickIntervalMin = 10*1000;
		DWORD clickIntervalMax = 30*1000;

		MultiHopInfoPtr info = m_tipsClient.GetUrlInfo();
		if (info->Mode == MultiHopInfo::UrlMode)
		{
			MultiHopUrlInfoPtr urlInfo = boost::static_pointer_cast<MultiHopUrlInfo>(info);
			if (urlInfo->IsReferedBrowser == true)
			{	// 在执行Referfer的浏览空间中, 直接点击，不要顾虑
				DoDirectClick(urlInfo->Url);
				urlInfo->IsReferedBrowser = false;
				return;
			}

			clickIntervalMin = urlInfo->ClickIntervalMin;
			clickIntervalMax = urlInfo->ClickIntervalMax;

			// 如果没有配置任何条数，则不跳
			if (urlInfo->AllClickRates.empty())
			{
				APP_EVENT("#### Click rate is 0");
				return;
			}
			// 如果点击次数已经用满
			if (m_tipsClient.GetClickCount() >= urlInfo->AllClickRates.size())
			{
				APP_EVENT("#### Click Count is full");
				return;
			}
		} 
		else if (info->Mode == MultiHopInfo::XmlConfigMode)
		{
			MultiHopXmlConfigInfoPtr xmlInfo = boost::static_pointer_cast<MultiHopXmlConfigInfo>(info);
			if (xmlInfo->IsReferedBrowser == true)
			{	// 在执行Referfer的浏览空间中, 直接点击，不要顾虑
				DoDirectClick(xmlInfo->Url);
				xmlInfo->IsReferedBrowser = false;
				return;
			}

			int iClickCount = m_tipsClient.GetClickCount();
			
			// 如果没有配置任何条数，则不跳
			if (xmlInfo->hops.size() == 0)
			{
				APP_EVENT("#### HopXmlConfig Config Zero Hop");
				return;
			}
			if (iClickCount >= xmlInfo->hops.size() )
			{
				APP_EVENT("#### Click Count is full");
				return;
			}
			clickIntervalMin = xmlInfo->hops[iClickCount].minInterval * 1000;
			clickIntervalMax = xmlInfo->hops[iClickCount].maxInterval * 1000;
		}

		LIMIT_MIN(clickIntervalMax, clickIntervalMin);
		DWORD clickInterval = clickIntervalMin;
		if( clickIntervalMax - clickIntervalMin > 0)
		{
			RandomGenerator random;
			clickInterval = clickIntervalMin + random.NextIn(clickIntervalMax-clickIntervalMin);
		}

		if(clickInterval == 0)
		{
			AnalysisAndClick(url);
		}
		else
		{
			m_nClickTimer = SetTimer(2, clickInterval, NULL);
		}
	}
	CATCH_ALL(e)
	{
		assert(false);
		e->Delete();
		APP_EVENT("LogicalDocumentComplete CATCH_ALL "<<(LPCTSTR)url);
	}
	END_CATCH_ALL
}

//////////////////////////////////////////////////////////////////////////
// 在Referer的执行空间中，强制点击
bool	CWBWnd::DoDirectClick(const string& url)
{
	APP_EVENT("DoDirectClick " << url);
	LPDISPATCH lpDisp = m_WB.GetDocument();
	CComQIPtr<IHTMLDocument2>	spDoc2(lpDisp);
	SAFE_RELEASE(lpDisp);
	if ( !spDoc2 )
	{
		APP_ERROR("DoDirectClick Error, because get_Document error");
		return false;
	}
	
	//get body element
	CComPtr<IHTMLElement>	spBodyElement;
	if ( !(SUCCEEDED(spDoc2->get_body(&spBodyElement)) && spBodyElement) )
	{
		APP_ERROR("DoDirectClick Error, because get_body error");
		return false;
	}
				
	//create new 'a' element
	CComPtr<IHTMLElement>	spHackElement;
	if ( !(SUCCEEDED(spDoc2->createElement(CComBSTR(_T("a")), &spHackElement)) && spHackElement) )
	{
		APP_ERROR("DoDirectClick Error, because createElement <A> error");
		return false;
	}

	spHackElement->setAttribute(CComBSTR(_T("href")), CComVariant(url.c_str()));
	spHackElement->put_innerText(CComBSTR(_T("click url")));
					
	//add new element to end of the 'body'
	CComQIPtr<IHTMLDOMNode>	spBodyNode = spBodyElement;
	if (spBodyNode)
	{
		CComQIPtr<IHTMLDOMNode>	spNewNode = spHackElement;
		CComPtr<IHTMLDOMNode>	spRetNode;
		spBodyNode->appendChild(spNewNode, &spRetNode);
	}

	//do click
	HRESULT hr = spHackElement->click();
	if (FAILED(hr))
	{
		APP_ERROR("DoDirectClick Error, because click <A> failed " << hr);
		return false;
	}

	APP_EVENT("DoDirectClick Succeed.");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 分析并点击
void	CWBWnd::AnalysisAndClick(CString url)
{
	APP_EVENT("AnalysisAndClick Begin "<<(LPCTSTR)url);
	TRY 
	{
		LPDISPATCH lpDisp = m_WB.GetDocument();
		CComQIPtr<IHTMLDocument2>	spDoc2(lpDisp);
		SAFE_RELEASE(lpDisp);
		if (!spDoc2)
		{
			APP_EVENT("Get spDoc2 false, so return");
			return;
		}

		//顶层文档准备就绪，可以开始查询对应URL的点击信息
		if ( !IsToplevelDocumentReady( spDoc2 ) )
		{	// 说明不是HTML
			APP_EVENT("IsToplevelDocumentReady false, so return");
			//return;
		}
		//m_timeoutTimer.Stop();

		MultiHopInfoPtr info = m_tipsClient.GetUrlInfo();
		if (info->Mode == MultiHopInfo::UrlMode)
		{
			MultiHopUrlInfoPtr urlInfo = boost::static_pointer_cast<MultiHopUrlInfo>(info);
			int iClickCount = m_tipsClient.GetClickCount();
			// 如果没有点击概率队列
			if ( urlInfo->AllClickRates.empty() )
			{
				APP_EVENT("#### Click rate is 0");
				return;
			}
			// 如果点击次数已经用满
			if (iClickCount >= urlInfo->AllClickRates.size())
			{
				APP_EVENT("#### Click Count is full");
				return;
			}
			
			// 检查点击率
			int clickRate = urlInfo->AllClickRates.at(iClickCount);
			LIMIT_MIN(clickRate, 0);
			LIMIT_MAX(clickRate, 100);
			int randomValue = RandomGenerator().NextIn( 100 );
			if ( randomValue > clickRate )
			{
				APP_EVENT("document complete click random failed "<<clickRate<<"-"<<randomValue);
				return;
			}

			// 检查是否为定向点击
			TaskPatternsList patterns = urlInfo->Patterns;
			if (urlInfo->DirectionalMode != 0 
				&& iClickCount < urlInfo->DirectionalsArray.size())
			{
				string href = urlInfo->DirectionalsArray[m_tipsClient.GetClickCount()];
				if (href != "" && href != "*")
				{	// 说明定位到了href, 则
					APP_EVENT("Will Do Redirectional url = " << href);
					if (urlInfo->DirectionalMode == 2)
					{	// 说明是自然定向
						TaskPatterns pattern;
						pattern.begin = CComBSTR("<A ");
						pattern.mid = CComBSTR(href.c_str());
						patterns.clear();
						patterns.push_back(pattern);
					}
					else if (urlInfo->DirectionalMode == 1)
					{	// 说明是强制定向
						DoDirectClick(href);
						m_tipsClient.AddClickCount();
						return;
					}
				}
			}
					
			// 可以点击率
			html_element_array elems;
			UrlDOMExtractor extractor( spDoc2, patterns, elems );
			extractor.Extract();
			m_tipsClient.AddClickCount();
			if ( elems.empty() )
			{
				APP_EVENT("#### elems is null so no one can be clicked");
				return;
			}
			APP_EVENT("#### "<<elems.size()<<" elems can be clicked");
			size_t index = RandomGenerator().NextIn( elems.size() );
			APP_EVENT("#### "<<m_tipsClient.GetClickCount()<<" Click elem "<<index<<" in "<<(LPCTSTR)url);
			elems[index].m_T->click();
		}
		else if (info->Mode == MultiHopInfo::XmlConfigMode)
		{
			MultiHopXmlConfigInfoPtr xmlInfo = boost::static_pointer_cast<MultiHopXmlConfigInfo>(info);
			int iClickCount = m_tipsClient.GetClickCount();
			// 如果没有点击概率队列
			if ( xmlInfo->hops.empty() )
			{
				APP_EVENT("#### Click rate is 0");
				return;
			}
			// 如果点击次数已经用满
			if (iClickCount >= xmlInfo->hops.size())
			{
				APP_EVENT("#### Click Count is full");
				return;
			}
			int clickRate = xmlInfo->hops[iClickCount].probability;
			int randomValue = RandomGenerator().NextIn( 100 );
			if ( randomValue > clickRate )
			{
				APP_EVENT("document complete click random failed "<<clickRate<<"-"<<randomValue);
				return;
			}
			// 可以点击了
 			html_element_array elems;
			vector<int> rates;
 			XmlConfigDOMExtractor extractor(spDoc2, xmlInfo, elems, rates, iClickCount);
 			extractor.Extract();
			m_tipsClient.AddClickCount();
 			if ( elems.empty() )
 			{
 				APP_EVENT("#### elems is null so no one can be clicked");
 				return;
 			}
 			APP_EVENT("#### "<<elems.size()<<" elems can be clicked");
 			size_t index = extractor.GetRandomIndex();
 			APP_EVENT("#### "<<m_tipsClient.GetClickCount()<<" Click elem "<<index<<" in "<<(LPCTSTR)url);
 			elems[index].m_T->click();
		}
		APP_EVENT("AnalysisAndClick End "<<(LPCTSTR)url);
	}
	CATCH_ALL(e)
	{
		assert(false);
		e->Delete();
		APP_EVENT("AnalysisAndClick CATCH_ALL "<<(LPCTSTR)url);
	}
	END_CATCH_ALL
}

void	CWBWnd::OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL)
{
	if ( !URL || URL->vt != VT_BSTR || !URL->bstrVal )
	{
		return;
	}

	TRY
	{
		CString url(URL->bstrVal);
		APP_EVENT("OnDocumentComplete "<<(LPCTSTR)url);

		CComQIPtr<IDispatch> lpDisp = m_WB.GetControlUnknown();
		if( pDisp == lpDisp )
		{
			KillTimer(1);
			LogicalDocumentComplete(url);
		}
	}
	CATCH_ALL(e)
	{
		assert(false);
		e->Delete();
	}
	END_CATCH_ALL
}

void CWBWnd::OnDownloadBeginExplorer1() 
{
	
}

void CWBWnd::OnFileDownloadExplorer1(BOOL ActiveDocument, BOOL FAR* Cancel) 
{
	APP_EVENT("OnFileDownload So Cancel "<<ActiveDocument);
	(*Cancel) = TRUE;
}

void CWBWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	if (m_WB.m_hWnd)
	{
		m_WB.DestroyWindow();
	}
}

BOOL	CWBWnd::IsToplevelDocumentReady( CComQIPtr<IHTMLDocument2> spDoc2 )
{
	CComBSTR	bstrState;
	spDoc2->get_readyState(&bstrState);
	if (bstrState == L"complete")
	{
		return TRUE;
	}
	return FALSE;
}

void CWBWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
// #ifndef _SHOW_BROWSER
// 	if (bShow)
// 	{
// 		DestroyWindow();
// 	}
// 
// #endif
}

void CWBWnd::OnSize( UINT nType, int cx, int cy)
{
	APP_EVENT("CWBWnd::OnSize  "<<"cx="<<cx<<", cy="<<cy);
	m_WB.SetWindowPos(NULL, 0, 0, cx, cy, 0);	
}

void CWBWnd::Cleanup()
{
	if (m_WB.m_hWnd)
	{
		KillTimer(1);
		KillTimer(2);

		StopWB();
	}
	m_WB.DestroyWindow();

	if ( m_hWnd )
	{
		DestroyWindow();
	}
}

bool CWBWnd::ClickUrl( MultiHopInfoPtr info )
{
	string url = "";
	string referer = "";
	string userAgent = "";
	bool alwaysOpenWindow = false;
	if (info->Mode == MultiHopInfo::UrlMode)
	{
		MultiHopUrlInfoPtr urlInfo = boost::static_pointer_cast<MultiHopUrlInfo>(info);
		urlInfo->Url = TipsConvert::UrlConvert(urlInfo->Url);
		urlInfo->Referer = TipsConvert::UrlConvert(urlInfo->Referer);
		urlInfo->UserAgent = TipsConvert::UrlConvert(urlInfo->UserAgent);
		url = urlInfo->Url;
		referer = urlInfo->Referer;
		userAgent = urlInfo->UserAgent;
		if (m_spOleCs) {
			m_spOleCs->put_loadFlash(urlInfo->LoadFlash ? VARIANT_TRUE : VARIANT_FALSE);
			m_spOleCs->put_userAgent(CComBSTR(userAgent.c_str()));
		}

		// 在执行RefererdBrowser里 表示先要Load,Referer指定的页面，然后点击到真正需要执行的页面去
		if (urlInfo->IsReferedBrowser)
		{
			url = referer;
			referer = "";
		}
	} 
	else if (info->Mode == MultiHopInfo::XmlConfigMode)
	{
		MultiHopXmlConfigInfoPtr xmlInfo = boost::static_pointer_cast<MultiHopXmlConfigInfo>(info);

		int i = xmlInfo->GetUrlRamdom();
		if (i < 0 || i >= xmlInfo->urls.size())
		{
			return false;
		}

		const UrlInfo& urlInfo = xmlInfo->urls[i];
		xmlInfo->Url = TipsConvert::UrlConvert(urlInfo.url);
		xmlInfo->Referer = TipsConvert::UrlConvert(urlInfo.referer);

		if (urlInfo.method == 0)
		{	// 执行方法是0 表示传统方式 执行Referer篡改
			url = xmlInfo->Url;
			referer = xmlInfo->Referer;
		}
		else if (urlInfo.method == 1)
		{	// 方式是1 表示仿真方式 执行Referer篡改
			if (xmlInfo->Referer == "") {
				url = xmlInfo->Url;
			} else {
				// 将配置中的referer作为主Referer
				url = xmlInfo->Referer;
				// 将跳转率的 最前面 增加一个100%的点击
				HopInfo hopInfo;
				hopInfo.probability = 100;
				hopInfo.minInterval = 3;
				hopInfo.maxInterval = 5;
				xmlInfo->hops.insert(xmlInfo->hops.begin(), hopInfo);
				// 将状态表 的最前面 增加一个往urlInfo.url跳转的状态
				HopStateInfo hopStateInfo;
				hopStateInfo.clickMode = 0;
				hopStateInfo.minHop = 0;
				hopStateInfo.maxHop = 1;
				hopStateInfo.hitMatches.push_back(CComBSTR(xmlInfo->Referer.c_str()));
				ClickInfo clickInfo;
				clickInfo.begin = CComBSTR(_T("<A "));
				clickInfo.minProbability = 0;
				clickInfo.maxProbability = 100;
				clickInfo.priority = 0;
				clickInfo.mids.push_back(CComBSTR(xmlInfo->Url.c_str()));
				hopStateInfo.clickes.insert(std::make_pair(0, clickInfo));
				xmlInfo->states.insert(xmlInfo->states.begin(), hopStateInfo);
			}
		}
		else if (urlInfo.method == 2)
		{	// 方式是1 表示Hank 执行Referer篡改
			if (xmlInfo->Referer == "") {
				url = xmlInfo->Url;
			} else {
				xmlInfo->IsReferedBrowser = true;
				url = xmlInfo->Referer;
			}
		}

		// 是否一直打开窗口
		if (xmlInfo->loadFlash == 1)
			alwaysOpenWindow = true;

		// UserAgent
		if (!xmlInfo->XmlPlatform.empty())
		{
			userAgent = xmlInfo->UserAgent;
		}
		else if(xmlInfo->userAgents.size() > 0)
		{
			i = xmlInfo->GetUserAgentRandom();
			if (i < 0 || i >= xmlInfo->userAgents.size())
			{
				return false;
			}
			xmlInfo->UserAgent = xmlInfo->userAgents[i].ua;
			userAgent = xmlInfo->UserAgent;
		}

		if (m_spOleCs) {
			m_spOleCs->put_loadFlash(xmlInfo->loadFlash ? VARIANT_TRUE : VARIANT_FALSE);
			m_spOleCs->put_userAgent(CComBSTR(userAgent.c_str()));
		}
	}

	if (url == "")
	{
		APP_ERROR("NavigateUrl Error Because url is Empty");
		return false;
	}

	APP_EVENT("NavigateUrl after stopwb " << std::make_pair(url, referer) );
	if ( false == NavigateUrl(url, referer, userAgent, alwaysOpenWindow) )
	{
		APP_ERROR("NavigateUrl Error " << url);
		return false;
	}
	return true;
}


void CWBWnd::EnadbleConfigs()
{
	MultiHopInfoPtr info = m_tipsClient.GetUrlInfo();
	if (info->Mode == MultiHopInfo::UrlMode)
	{
		MultiHopUrlInfoPtr urlInfo = boost::static_pointer_cast<MultiHopUrlInfo>(info);
		if (m_spOleCs) {
			m_spOleCs->put_loadFlash(urlInfo->LoadFlash ? VARIANT_TRUE : VARIANT_FALSE);
			m_spOleCs->put_userAgent(CComBSTR(urlInfo->UserAgent.c_str()));
		}
	}
	else if (info->Mode == MultiHopInfo::XmlConfigMode)
	{
		MultiHopXmlConfigInfoPtr xmlInfo = boost::static_pointer_cast<MultiHopXmlConfigInfo>(info);
		if (m_spOleCs) {
			m_spOleCs->put_loadFlash(xmlInfo->loadFlash ? VARIANT_TRUE : VARIANT_FALSE);
			m_spOleCs->put_userAgent(CComBSTR(xmlInfo->UserAgent.c_str()));
		}
	}
}

void CWBWnd::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == 1)
	{	// 对应 m_nDDTimer
		APP_EVENT("30s has not OnDocumnetComplete, so Do LogicalDocumentComplete");
		KillTimer(1);
		LogicalDocumentComplete(m_mainUrl);
	}
	else if( nIDEvent == 2)
	{	// 对应 m_nClickTimer
		APP_EVENT("Clict Interval Arrived, So AnalysisAndClick");
		KillTimer(2);
		AnalysisAndClick(m_mainUrl);
	}
	CWnd::OnTimer(nIDEvent);
}

void CWBWnd::OnTaskTimeout()
{
	MultiHopInfoPtr info = m_tipsClient.GetUrlInfo();
	if (info->Mode == MultiHopInfo::UrlMode)
	{
		MultiHopUrlInfoPtr urlInfo = boost::static_pointer_cast<MultiHopUrlInfo>(info);
		APP_ERROR("OnTaskTimeout " << make_tuple(urlInfo->Url, urlInfo->AllClickRates.size()));
	}
	else if (info->Mode == MultiHopInfo::XmlConfigMode)
	{
		
	}
}

bool CWBWnd::StopWB()
{
	TRY
	{
		m_WB.Stop();
		//m_WB.Quit();
	}
	CATCH_ALL(e)
	{
		//assert(false);
		//e->Delete();
		return false;
	}
	END_CATCH_ALL
	return true;	
}

bool CWBWnd::NavigateUrl(const string& urlstr, const string& referer, const string& userAgent, bool alwaysOpenWindow)
{
	//	m_referer = CComBSTR(("Referer: " + referer + "\r\n").c_str());		
	//	m_userAgent = CComBSTR("User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.6) Gecko/2009011913 Firefox/3.0.6\r\n");
	//	m_userAgent = CComBSTR("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1) ; .NET CLR 1.1.4322; .NET CLR 2.0.50727)\r\n");	
	//	APP_ERROR("NavigateUrl " << make_pair(urlstr, referer));
	if (!CreateWB())
	{
		APP_ERROR("NavigateUrl: failed to create wb" << std::make_pair(urlstr, referer));
		return false;
	}

	StopWB();

	string headers;
	if ( referer.size() > 0 )
	{
		headers	+= "Referer: ";
		headers += referer;
		headers += "\r\n";
	}
	if ( userAgent.size() > 0 )
	{
		headers	+= "User-Agent: ";
		headers += userAgent;
		headers += "\r\n";
	}
	CComVariant varHeaders(headers.c_str());

	int nFlags = 0;
 	if (alwaysOpenWindow == true)
	{
 		nFlags |= navOpenInNewWindow;
	}
	CComVariant varFlags(nFlags);

	LPCTSTR strTargetFrameName = _T("_top");
 	if (alwaysOpenWindow == true)
		strTargetFrameName = _T("_blank");
	CComVariant varTargetFrameName(strTargetFrameName);

//? 这里奇怪，如果加了 TRY-CATCH 有时候反而会挂，但是如果不加TRY-CATCH 就一定不会挂
//	TRY
	{
 		//m_WB.Navigate(urlstr.c_str(), &varFlags, &varTargetFrameName, NULL, headers.empty() ? NULL : &varHeaders);
		m_WB.Navigate(urlstr.c_str(), &varFlags, &varTargetFrameName, NULL, &varHeaders);
	}
// 	CATCH_ALL(e)
// 	{
// 		assert(false);
// 		e->Delete();
// 		return false;
// 	}
// 	END_CATCH_ALL
	
	return true;
}

void CWBWnd::OnTaskComplete()
{
}

LRESULT CWBWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_ACTIVATE )
	{
		APP_EVENT("CWBWnd::WindowProc WM_ACTIVATE "<<(UINT)wParam<<" "<<(UINT)lParam);
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

BOOL CWBWnd::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_ACTIVATE )
	{
		APP_EVENT("CWBWnd::PreTranslateMessage WM_ACTIVATE");
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}
