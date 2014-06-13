#if !defined(AFX_WBWND_H__53B23237_17FA_4390_8BC4_EC7688B63A5A__INCLUDED_)
#define AFX_WBWND_H__53B23237_17FA_4390_8BC4_EC7688B63A5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WBWnd.h : header file
//

#include <mshtmlc.h>
#include "webbrowser2.h"
//#include "AdSocket.h"
#include "TipsClientCsOperator.h"
#include <boost/function.hpp>
#include <list>
#include <set>
#include <map>
#include <string>
using std::string;

#include "HopInfo.h"
#include <ppl/mswin/mfc/timer.h>

/////////////////////////////////////////////////////////////////////////////
// CWBWnd window
#define IDC_WEB	123
#define MAX_REQUEST_BUFFER 4096
#define MAX_ANSWER_BUFFER  4096
#define MAX_QUERYURL_TIMES	3
#define MAX_CLICK_QUEUE_NUM 100

#pragma pack(push, 1)
typedef struct tagAdRequest
{
	long nCode;
	long nSize;
	char cData[1];
} AdRequest, *LPAdRequest;

typedef struct tagAdAnswer
{
	long nCode;
	long nSize;
	char cData[1];
} AdAnswer, *LPAdAnswer;
#pragma pack(pop)

#define TARGET_HOSTNAME	_T("h.g1d.net");
#define TARGET_PORT		8888

enum REQUEST_CODE
{
	RQ_SLEEP = 0xf1,
	RQ_QUERYURL = 0xFF
};

enum ANSWER_CODE
{
	AS_SHOWAD = 0xae,
	AS_WAIT = 0xaf,
	AS_NAVURL = 0xbb,
	AS_OVER = 0xbc
};


typedef boost::function1<void, MultiHopInfoPtr> WebNavigateCompleteCallback;

inline string from_bstr( BSTR s )
{
	CString s1(s);
	return string( s1, s1.GetLength() );
}


class CVideoAd;
class CWBWnd : public CWnd
{
// Construction
public:
	CWBWnd(TipsClient& tipClient);

	void SetCallback( WebNavigateCompleteCallback callback )
	{
		m_callback = callback;
	}

	void OnTaskTimeout( );

	//const std::list<int>& GetHops() const { return m_clickRates; }

// Attributes
public:
	enum
	{
		IDC_TIMER_WEB_TIMEOUT = 101, 
	};

// Operations
public:
	BOOL	Initialize();
	int		SetXmlWidth(int UaMinWidth, int UaMaxWidth);
	int		SetXmlHeight(int UaMinHeight, int UaMaxHeight);
	int		SetRandomWidth();
	int		SetRandomHeight();
	void	Cleanup();

	void OnTaskComplete();

	bool ClickUrl(MultiHopInfoPtr info);
	bool NavigateUrl(const string& urlstr, const string& referer, const string& userAgent, bool alwaysOpenWindow);
	bool StopWB();

	void EnadbleConfigs();

	afx_msg void OnBeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg void OnNewWindow2Explorer1(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel);
	afx_msg void OnNewWindow3Explorer1(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel,  DWORD dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl);
	afx_msg void OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL);
	afx_msg void OnDownloadBeginExplorer1();
	afx_msg void OnFileDownloadExplorer1(BOOL ActiveDocument, BOOL FAR* Cancel);
	afx_msg void OnNavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT FAR* URL);

protected:
	BOOL	IsToplevelDocumentReady( CComQIPtr<IHTMLDocument2> doc );
	BOOL	LookupOleEmbeddings(CComQIPtr<IHTMLDocument2> pDoc2);
	BOOL	TraceHtmlElement(CComQIPtr<IHTMLDocument2> pDoc);
	void	CurrentAdComplete();

	void	LogicalDocumentComplete(CString url);
	void	AnalysisAndClick(CString url);

	bool	DoDirectClick(const string& url);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWBWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWBWnd();

protected:
	BOOL	CreateWB();

protected:
	CWebBrowser2	m_WB;
	CComPtr<ITipsClientCsOperator>	m_spOleCs;
	WebNavigateCompleteCallback m_callback;
	TipsClient& m_tipsClient;
	UINT	m_nDDTimer;			// 用来 定时解析长期无法OnDocumentComplete的页面
	UINT	m_nClickTimer;		// 用来 控制曝光到点击的时间的时间
	CString m_mainUrl;

	int		m_sx;		
	int		m_sy;	//屏幕分辨率	
	int		m_rx;
	int		m_ry;	//设置分辨率
	static const int MIN_BROWSER_WIDTH;
	static const int MIN_BROWSER_HEIGHT;

	// Generated message map functions
protected:
	//{{AFX_MSG(CWBWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WBWND_H__53B23237_17FA_4390_8BC4_EC7688B63A5A__INCLUDED_)
