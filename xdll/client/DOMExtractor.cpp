
#include "StdAfx.h"
#include "DOMExtractor.h"
#include <ppl/util/macro.h>
#include <ppl/util/log.h>
#include <assert.h>
#include <ppl/util/random.h>

DOMExtractor::DOMExtractor( CComQIPtr<IHTMLDocument2> doc) 
	: m_doc(doc)
{
}

UrlDOMExtractor::UrlDOMExtractor( CComQIPtr<IHTMLDocument2> doc, const TaskPatternsList& patterns, html_element_array& elems ) 
	: DOMExtractor(doc), m_patterns(patterns), m_elements(elems)
{
	
}

XmlConfigDOMExtractor::XmlConfigDOMExtractor( CComQIPtr<IHTMLDocument2> doc, boost::shared_ptr<HopXmlConfig> pXmlConfig, 
		html_element_array& elems,  std::vector<int>& rateList, int iClickCount )
	: DOMExtractor(doc), m_pXmlConfig(pXmlConfig), m_elements(elems), m_rateList(rateList), m_iClickCount(iClickCount)
	, m_pHopStateInfo(NULL), m_checkRate(0)
{
	
}

//////////////////////////////////////////////////////////////////////////
/// DOMExtractor
//////////////////////////////////////////////////////////////////////////

bool DOMExtractor::Extract()
{	
	APP_EVENT(_T("DOMExtractor::Extract() Begin"));
	Init();

	CComBSTR	bstrURL;
	if (FAILED(m_doc->get_URL(&bstrURL)))
		return FALSE;
	
	bool result = LookupOleEmbeddings(m_doc, bstrURL);
	APP_EVENT(_T("DOMExtractor::Extract() End"));
	return result;
}

bool DOMExtractor::LookupOleEmbeddings(CComQIPtr<IHTMLDocument2> pDoc2, CComBSTR bstrURL)
{
	if (TraceHtmlElement(pDoc2, bstrURL))
		return true;
	
	CComQIPtr<IOleContainer> spContainer = pDoc2;
	if (!spContainer)
		return false;
	
	long nEmbeddingCnt = 0;
	CComPtr<IEnumUnknown> spEnumUnk;
	if (SUCCEEDED(spContainer->EnumObjects(OLECONTF_EMBEDDINGS, &spEnumUnk)) && spEnumUnk)
	{
		spEnumUnk->Reset();
		CComPtr<IUnknown> spUnk;
		ULONG ulFetched;
		while (spEnumUnk->Next(1, &spUnk, &ulFetched) == S_OK)
		{
			CComQIPtr<IWebBrowser2>	spWebbrowser = spUnk;
			CComQIPtr<IHTMLDocument2>	spDoc2;
			CComBSTR	bstrURLTemp;
			if (spWebbrowser)
			{
				CComPtr<IDispatch>	spDisp;
				spWebbrowser->get_Document(&spDisp);
				spDoc2 = spDisp;
				if (spDoc2)
				{
					if (SUCCEEDED(spDoc2->get_URL(&bstrURLTemp)) && _wcsicmp(bstrURLTemp, L"about:blank") == 0)
					{
						goto _CONTINUE;
					}
				}
				else
				{
					goto _CONTINUE;
				}
				
				nEmbeddingCnt++;
			}
			else
			{
				goto _CONTINUE;
			}
			
			if (LookupOleEmbeddings(spDoc2, bstrURL))
				return true;
			
_CONTINUE:
			spUnk.Release();
		}
		
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
/// UrlDOMExtractor
//////////////////////////////////////////////////////////////////////////

void UrlDOMExtractor::Init()
{
	m_elements.clear();
	m_elements.reserve(32);
}

bool UrlDOMExtractor::TraceHtmlElement(CComQIPtr<IHTMLDocument2> pDoc, CComBSTR bstrURL)
{
	CComPtr<IHTMLElementCollection>	spHtmlConnection;

	if (pDoc && SUCCEEDED(pDoc->get_all(&spHtmlConnection)) && spHtmlConnection)
	{
		BOOL	bPrintElement = FALSE;
		//CComBSTR	bstrURL;
		//if (FAILED(pDoc->get_URL(&bstrURL)))
		//	return FALSE;
		//TRACE(_T("frame url:%ls\n"), bstrURL);

		/*  调试代码
		string strFrameURL = "http://www1.leeuu.com/ad/001.htm?src=pplive&ext=000001";
		if (!strnicmp(W2A(bstrURL), strFrameURL.c_str(), strFrameURL.length()))
			bPrintElement = true;
			*/

		long num = 0;
		spHtmlConnection->get_length(&num);
		for (long i=0; i<num; i++)
		{
			CComQIPtr<IHTMLElement>	spElement;
			CComPtr<IDispatch> pDisptch;
			CComVariant	vtName = i;
			CComVariant vtIndex = i;
			CComBSTR bstrValue;
			// 								VARIANT index;
			// 								index.vt = VT_I4;
			// 								index.intVal = 1;
			TRY
			{
				spHtmlConnection->item(vtName, vtIndex, &pDisptch);
				if (pDisptch)
				{
					spElement = pDisptch;

					if (SUCCEEDED(spElement->get_outerHTML(&bstrValue)))
					{
						#if defined(_DEBUG) && 0
						CComVariant urlstr;
						CComDispatchDriver disp(pDisptch);
						if ( SUCCEEDED(disp.GetPropertyByName(L"href", &urlstr)) && VT_BSTR == urlstr.vt && urlstr.bstrVal != NULL )
						{
							TRACE("href %ls  %ls\n", bstrURL.m_str, urlstr.bstrVal );
						}
						#endif
						
						//打印out.log
						#ifdef NEED_LOG
						if(SysStringLen(bstrValue) < 400*MAX_PATH)
						{
							char outerHTML[400 * MAX_PATH] = {0};
							DWORD dwSize = WideCharToMultiByte(CP_ACP, NULL, bstrValue, -1, NULL, 0, NULL, NULL);
							WideCharToMultiByte(CP_ACP, NULL, bstrValue, -1, outerHTML, dwSize, NULL, NULL);				
							APPF_EVENT(outerHTML);
						}
						#endif

						if (bPrintElement)
						{
							//USES_CONVERSION;
							//TRACE(_T("BBBBBBBBBBBB    "));
							//TRACE(_T("%ls"), bstrValue);
							//TRACE(_T("EEEEEEEEEEEE    "));
						}

						//APPF_EVENT("!!!!!!!! "<<(LPCSTR)CString(bstrURL)<<"\r\n"<<(LPCTSTR)CString(bstrValue));
						
						//如果获取的outerHTML开头有回车换行符，则移除
						CString strTemp = bstrValue;
						if (!strTemp.IsEmpty())	
						{
							if(_T('\r') == strTemp.GetAt(0) || _T('\n') == strTemp.GetAt(0))
							{
								strTemp.TrimLeft();
								bstrValue.Empty();
								bstrValue = strTemp;
							}
						}

						//STL_FOR_EACH_CONST( bstr_list, m_patterns, iter )
						STL_FOR_EACH_CONST( TaskPatternsList, m_patterns, iter )
						{
							//const CComBSTR& pattern = *iter;
							const TaskPatterns& pattern = *iter;

							if( pattern.url_only.Length() > 0 )
							{
								if( 0 != _wcsicmp(bstrURL, pattern.url_only) )
								{
									TRACE("Url Only is not right\n");
									continue;
								}
							}

							if( pattern.url.Length() > 0 )
							{
								if( NULL == StrStrIW(bstrURL, pattern.url))
								{
									TRACE("Url is not valueable\n");
									continue;
								}
							}

							if( pattern.url_forbidden.Length() > 0 )
							{
								if( 0 == _wcsicmp(bstrURL, pattern.url_forbidden) )
								{
									TRACE("Url Forbidden\n");
									continue;
								}
							}

							if( pattern.mid.Length() == 0 )
							{
								//APPF_EVENT("!!!!!!!! "<<(LPCTSTR)CString(bstrValue));
								//APPF_EVENT("######## "<<(LPCTSTR)CString(pattern.begin));
								if (bstrValue.Length()>=pattern.begin.Length() && !_wcsnicmp(bstrValue, pattern.begin, pattern.begin.Length()))	
								{
									//TRACE("add html elem %d \n", i );
									m_elements.push_back( spElement );
									//APPF_EVENT("!!!!!!!!!!!!!!!!!! "<<m_elements.size()<<": "<<(LPCSTR)CString(pattern.begin)<<"$$"<<(LPCSTR)CString(pattern.mid)<<"$$"<<(LPCSTR)CString(pattern.url));
									//APPF_EVENT("$$$$$$$$$$$$$$$$$$ "<<(LPCSTR)CString(bstrURL)<<"\r\n"<<(LPCTSTR)CString(bstrValue));
								}
							}
							else
							{
								if (bstrValue.Length()>=pattern.begin.Length() && !_wcsnicmp(bstrValue, pattern.begin, pattern.begin.Length())
									&& NULL != StrStrIW(bstrValue, pattern.mid) )	
								{
									//TRACE("add html elem %d \n", i );
									m_elements.push_back( spElement );
									//APPF_EVENT("!!!!!!!!!!!!!!!!!! "<<m_elements.size()<<": "<<(LPCSTR)CString(pattern.begin)<<"$$"<<(LPCSTR)CString(pattern.mid)<<"$$"<<(LPCSTR)CString(pattern.url));
									//APPF_EVENT("################## "<<(LPCSTR)CString(bstrURL)<<"\r\n"<<(LPCTSTR)CString(bstrValue));
								}
							}
						}
					}
				}
			}
			CATCH_ALL(e)
			{
				assert(false);
				TRACE("error occurred in TraceHtmlElement\n");
				e->Delete();
			}
			END_CATCH_ALL
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
/// XmlConfigDOMExtractor
//////////////////////////////////////////////////////////////////////////

void XmlConfigDOMExtractor::Init()
{
	m_elements.clear();
	m_elements.reserve(32);
	m_rateList.clear();
	
	m_pHopStateInfo = NULL;
	m_checkRate = 0;
}

bool XmlConfigDOMExtractor::Extract()
{	
	APP_EVENT(_T("XmlConfigDOMExtractor::Extract() Begin"));
	Init();
	
	CComBSTR	bstrURL;
	if (FAILED(m_doc->get_URL(&bstrURL)))
		return FALSE;

	if (!m_pXmlConfig)
		return FALSE;

	bool isHit = false;
	for (vector<HopStateInfo>::iterator iter = m_pXmlConfig->states.begin();
		iter != m_pXmlConfig->states.end(); iter ++)
	{
		HopStateInfo& hopStateInfo = *iter;

		// 检查状态匹配
		isHit = hopStateInfo.HitHop(m_iClickCount) && hopStateInfo.HitUrl(bstrURL);
		if (isHit)
		{
			m_pHopStateInfo = &hopStateInfo;
			break;
		}
	}

	if (isHit == false)
	{
		APP_ERROR("!!!! No HopState Hit " << bstrURL);
		return FALSE;
	}

	// 检查脚本，如果有的话则，执行脚本
	if (m_pHopStateInfo->script != "")
	{
		// 执行脚本
		HRESULT hr;
		CComQIPtr<IHTMLWindow2> spWindows;
		hr = m_doc->get_parentWindow(&spWindows);
		APP_INFO("Script: " << m_pHopStateInfo->script);
		if( FAILED(hr) )
		{
			APP_ERROR("RunScript: IHTMLWindow2 get_parentWindow Error");
		} else {
 			VARIANT v;
			hr = spWindows->execScript(CComBSTR(m_pHopStateInfo->script.c_str()), CComBSTR(_T("javascript")), &v);
 			if( FAILED(hr) ) { APP_ERROR("RunScript: spWindows Run Script Error"); }
			else { APP_EVENT("RunScript: spWindows Run Script Succeed"); }
		}
	}

	m_checkRate = RandomGenerator().NextIn(100);

	APP_EVENT("m_checkRate = " << m_checkRate);
	
	bool result = LookupOleEmbeddings(m_doc, bstrURL);

	if (m_elements.size() == 0)
	{
		APP_EVENT(_T("XmlConfigDOMExtractor::Extract() End"));
		return result;
	}

	// 检查元素坐标位置
// 	long left = 0, top = 0, width = 0, height = 0;
// 	spElement->get_offsetLeft(&left);
// 	spElement->get_offsetTop(&top);
// 	spElement->get_offsetWidth(&width);
// 	spElement->get_offsetHeight(&height);
// 	
// 	APP_EVENT("spElement (left,top)=("<<left<<","<<top<<") (width,height)=("<<width<<","<<height<<")");


	// 设置概率
	assert(m_pHopStateInfo);

	int n = m_elements.size();
	m_rateList.clear();
	m_rateList.reserve(n);
	if (m_pHopStateInfo->clickMode == 1)
	{	// 顺序有限分配 点击概率
		for (int i = n; i > 0; i --)
			m_rateList.push_back(i);
	}
	else
	{	// 平均分配
		for (int i = n; i > 0; i --)
			m_rateList.push_back(1);
	}

	assert(m_elements.size() == m_rateList.size());
	APP_EVENT(_T("XmlConfigDOMExtractor::Extract() End"));

	return result;
}

bool XmlConfigDOMExtractor::TraceHtmlElement(CComQIPtr<IHTMLDocument2> pDoc, CComBSTR bstrURL)
{
	assert (m_pHopStateInfo != NULL);

	CComPtr<IHTMLElementCollection>	spHtmlConnection;

	map<int, html_element_array> htmlElementsMap;

	if (pDoc && SUCCEEDED(pDoc->get_all(&spHtmlConnection)) && spHtmlConnection)
	{
		BOOL	bPrintElement = FALSE;

		long num = 0;
		spHtmlConnection->get_length(&num);

		APP_EVENT("TraceHtmlElement Elements " << num);
		for (long i=0; i<num; i++)
		{
			CComQIPtr<IHTMLElement>	spElement;
			CComPtr<IDispatch> pDisptch;
			CComVariant	vtName = i;
			CComVariant vtIndex = i;
			CComBSTR bstrValue;
			TRY
			{
				spHtmlConnection->item(vtName, vtIndex, &pDisptch);
				if (pDisptch)
				{
					spElement = pDisptch;

					if (SUCCEEDED(spElement->get_outerHTML(&bstrValue)))
					{
						#if defined(_DEBUG) && 0
						CComVariant urlstr;
						CComDispatchDriver disp(pDisptch);
						if ( SUCCEEDED(disp.GetPropertyByName(L"href", &urlstr)) && VT_BSTR == urlstr.vt && urlstr.bstrVal != NULL )
						{
							TRACE("href %ls  %ls\n", bstrURL.m_str, urlstr.bstrVal );
						}
						#endif
						
						int lastPriority = 0;
						// 遍历Click序列
						for (multimap<int, ClickInfo>::iterator iter = m_pHopStateInfo->clickes.begin();
							iter != m_pHopStateInfo->clickes.end(); iter ++)
						{
							const ClickInfo& clickInfo = iter->second;
						
							// 检查随机数
							if (!(clickInfo.minProbability <= m_checkRate && m_checkRate < clickInfo.maxProbability))
							{	// 点击率不再范围，则不点击
								continue;
							}

							assert( !(clickInfo.begin.Length() == 0 && clickInfo.mids.size() == 0 && clickInfo.exclude.size() == 0) );
							if (clickInfo.begin.Length() == 0 && clickInfo.mids.size() == 0 && clickInfo.exclude.size() == 0)
								continue;

							// 检查起始字符串
							if (clickInfo.begin.Length() > 0)
							{
								if ( !(bstrValue.Length() >= clickInfo.begin.Length()) )
									continue;
								if( !(0 == _wcsnicmp(bstrValue, clickInfo.begin, clickInfo.begin.Length())) )
								{
									continue;
								}
							}

							// 检查中间字符串
							if (clickInfo.mids.size() > 0)
							{
								bool bMidFind = true;
								STL_FOR_EACH_CONST(vector<CComBSTR>, clickInfo.mids, iter2)
								{
									//const string& mid = *iter2;
									CComBSTR mid = *iter2;
									if ( !StrStrIW(bstrValue, mid) )
									{	// 不匹配，则加入
										bMidFind = false;
										break;
									}
								}
								if (!bMidFind)
								{
									// 不匹配
									continue;
								}
							}

							// 检查exlude字符串
							if (clickInfo.exclude.size() > 0)
							{
								bool bExcludeFind = true;
								STL_FOR_EACH_CONST(vector<CComBSTR>, clickInfo.exclude, iter2)
								{
									//const string& mid = *iter2;
									CComBSTR exclu = *iter2;
									if ( StrStrIW(bstrValue, exclu) )
									{	// 匹配,则不加入
										bExcludeFind = false;
										break;
									}
								}
								if (!bExcludeFind)
								{
									// 不匹配
									continue;
								}
							}
							
							map<int, html_element_array>::iterator htmlElemIter = htmlElementsMap.find(clickInfo.priority);
							if (htmlElemIter == htmlElementsMap.end())
							{
								html_element_array htmlElements;
								htmlElementsMap[clickInfo.priority] = htmlElements;
								htmlElemIter = htmlElementsMap.find(clickInfo.priority);
								assert(htmlElemIter != htmlElementsMap.end());
							}

							htmlElemIter->second.push_back(spElement);
							
							//m_elements.push_back( spElement );
						}	// 遍历click序列 结束
					}
				}
			}
			CATCH_ALL(e)
			{
				assert(false);
				TRACE("error occurred in TraceHtmlElement\n");
				e->Delete();
			}
			END_CATCH_ALL
		}	// 遍历 html element 结束

		for (map<int, html_element_array>::iterator htmlElemIter = htmlElementsMap.begin();
		htmlElemIter != htmlElementsMap.end(); htmlElemIter++)
		{
			const html_element_array& htmlElements = htmlElemIter->second;
			if (htmlElements.size() > 0)
			{
				for (html_element_array::const_iterator elemIter = htmlElements.begin();
				elemIter != htmlElements.end(); elemIter ++)
				{
					m_elements.push_back(*elemIter);
				}
				APP_EVENT("Priority " << htmlElemIter->first << " has " << m_elements.size() << " element, so return");
				break;		// 优先级更低的就不用再考虑了
			}
			
			APP_EVENT("Priority " << htmlElemIter->first << " has zero element, so search next priority");
		}
	}	// end if

	return true;
}

int XmlConfigDOMExtractor::GetRandomIndex()
{
	if (m_rateList.size() == 0)
		return 0;
	assert(m_elements.size() == m_rateList.size());
	int n = m_rateList.size();
	int sum = 0;
	vector<int> m_calcRateList;
	m_calcRateList.reserve(n);
	for (int i = 0; i < n; i ++)
	{
		sum += m_rateList[i];
		m_calcRateList.push_back(sum);
	}
	assert(m_calcRateList.size() == m_rateList.size());

	int random = RandomGenerator().NextIn(sum);
	for (i = 0; i < n; i ++)
	{
		if (random < m_calcRateList[i])
			return i;
	}
	return 0;
}