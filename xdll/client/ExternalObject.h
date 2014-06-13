#if !defined(AFX_EXTERNALOBJECT_H__0DE3B0C9_4686_428C_84C5_5BF8AC385C5E__INCLUDED_)
#define AFX_EXTERNALOBJECT_H__0DE3B0C9_4686_428C_84C5_5BF8AC385C5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExternalObject.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CExternalObject command target

class CExternalObject : public CCmdTarget
{
	DECLARE_DYNCREATE(CExternalObject)

	CExternalObject();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExternalObject)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CExternalObject();

	// Generated message map functions
	//{{AFX_MSG(CExternalObject)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CExternalObject)
	afx_msg long GetCode();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTERNALOBJECT_H__0DE3B0C9_4686_428C_84C5_5BF8AC385C5E__INCLUDED_)
