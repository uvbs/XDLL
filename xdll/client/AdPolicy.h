#ifndef _ADPOLICY_H_
#define _ADPOLICY_H_


class AdPolicy
{
public:
	AdPolicy(){};
	~AdPolicy(){};
public:
	BOOL UpdatePolicyXml(LPCTSTR lpXmlUrl, LPCTSTR lpXmlPath);
	BOOL LoadPolicyXml(LPCTSTR lpXmlPath, CString& strParam);
};

#endif
