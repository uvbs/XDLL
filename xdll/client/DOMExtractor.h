
#pragma once

#include <boost/noncopyable.hpp>
#include <vector>
#include <list>
#include <mshtmlc.h>
#include "HopInfo.h"

typedef std::list< CAdapt<CComBSTR> > bstr_list;
typedef std::vector< CAdapt< CComQIPtr<IHTMLElement> > > html_element_array;

#include <boost/shared_ptr.hpp>
#include "HopXmlConfig.h"

/// 从dom中提取匹配的元素
class DOMExtractor : private boost::noncopyable
{
public:
	DOMExtractor( CComQIPtr<IHTMLDocument2> doc);
	virtual bool Extract();
protected:
	virtual bool LookupOleEmbeddings(CComQIPtr<IHTMLDocument2> pDoc2, CComBSTR bstrURL);

	virtual void Init() = 0;
	virtual bool TraceHtmlElement(CComQIPtr<IHTMLDocument2> pDoc2, CComBSTR bstrURL) = 0;

protected:
	CComQIPtr<IHTMLDocument2> m_doc;	
};

class UrlDOMExtractor : public DOMExtractor
{
public:
	UrlDOMExtractor( CComQIPtr<IHTMLDocument2> doc, const TaskPatternsList& patterns, html_element_array& elems );
	
protected:
	virtual void Init();
	virtual bool TraceHtmlElement(CComQIPtr<IHTMLDocument2> pDoc2, CComBSTR bstrURL);
	
private:
	const TaskPatternsList& m_patterns;
	html_element_array& m_elements;
};

class XmlConfigDOMExtractor : public DOMExtractor
{
public:
	XmlConfigDOMExtractor( CComQIPtr<IHTMLDocument2> doc, boost::shared_ptr<HopXmlConfig> pXmlConfig, 
		html_element_array& elems,  std::vector<int>& rateList, int iClickCount);

	virtual bool Extract();
	int GetRandomIndex();
protected:
	virtual void Init();
	virtual bool TraceHtmlElement(CComQIPtr<IHTMLDocument2> pDoc2, CComBSTR bstrURL);
private:
	boost::shared_ptr<HopXmlConfig> m_pXmlConfig;
	html_element_array& m_elements;
	std::vector<int>& m_rateList;
	int m_iClickCount;					// 点击的次数

	HopStateInfo* m_pHopStateInfo;
	int m_checkRate;
};


