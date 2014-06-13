#include "StdAfx.h"
#include "UrlEditor.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "CRegKeyEx.h"
#include <vector>
#include <assert.h>

string UrlEditor::UrlEdit(const string& str)
{
	string url = str;

	// �滻pp:����ַ���
	boost::replace_all(url, "[[[pp:area]]]", GetCityCode());

// 	// �滻rnd:����ַ���
// 	string url_temp = url;
// 	string begin_tag = "[[[";
// 	string end_tag = "]]]";
// 	while (true)
// 	{
// 		int index = boost::find_first(url_temp, begin_tag);
// 		if (index == string::npos)
// 			break;
// 		int index2 = boost::find_first(url_temp, end_tag);
// 		if (index == string::npos)
// 		{	// �쳣����Ӧ�ó��� ֻ��[[[ ��û��]]]�����
// 			url = url_temp.substr(0, index);
// 			break;
// 		}
// 		if( index2 < index1 )
// 		{	// �쳣����Ӧ�ó��� ֻ��]]] ��[[[ ǰ������
// 			url = url_temp.substr(0, index2);
// 			break;
// 		}
// 		// ��������ַ���
// 		string determiner = url_temp.substr(index, index2 - index + end_tag.length());		//�����ַ���
// 		string result = "";			// ���ӱ��滻���ַ���
// 
// 		assert(index2 >= index +  begin_tag.length());
// 		string mini_determiner = url_temp.substr(index + begin_tag.length(), index2 - index - begin_tag.length());
// 		url_temp = url_temp.substr(index2 + end_tag.length());
// 
// 		string k = "";
// 		string v = "";
// 		int equal_index = boost::find_first(mini_determiner, "=");
// 		if(equal_index == string::npos)
// 			k = mini_determiner;
// 		else
// 		{
// 			k = mini_determiner.substr(0, equal_index);
// 			v = mini_determiner.substr(equal_index+1);
// 		}
// 
// 		if (k == "rnd:random")
// 		{
// 			result = GetRndRamdom(v);
// 		}
// 		else
// 		{	// δ֪���ݣ�ֱ�ӱ��滻��
// 			result = "0";
// 		}
// 
// 		//�滻����
// 		boost::replace_first(url, determiner, result);
// 	}

	return url;
}

string UrlEditor::GetCityCode()
{
	TCHAR pszCityCode[MAX_PATH] = { 0 };
	
	//ͨ��ע����ȡ
	CRegKeyEx key;
	if (key.Open(HKEY_CURRENT_USER,_T("SOFTWARE\\PPLive"), KEY_READ|KEY_QUERY_VALUE) == ERROR_SUCCESS)
	{
		DWORD	dwCount = MAX_PATH;
		LONG lRes = key.QueryValue(pszCityCode, _T("citycode"), &dwCount);
		if (lRes != ERROR_SUCCESS)
		{
			return "1";
		}
		string ret = pszCityCode;
		return ret;
	}
	else
	{
		return "1";
	}
}

string UrlEditor::GetRndRamdom(const string& v)
{
	return "";
}