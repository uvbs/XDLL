// ExternalObject.cpp : implementation file
//

#include "stdafx.h"
#include "xdll.h"
#include "ExternalObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExternalObject

IMPLEMENT_DYNCREATE(CExternalObject, CCmdTarget)

CExternalObject::CExternalObject()
{
	EnableAutomation();
}

CExternalObject::~CExternalObject()
{
}


void CExternalObject::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CExternalObject, CCmdTarget)
	//{{AFX_MSG_MAP(CExternalObject)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CExternalObject, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CExternalObject)
	DISP_FUNCTION(CExternalObject, "GetCode", GetCode, VT_I4, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IExternalObject to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {E5E9BF00-A1AB-4B41-AFEF-9841A6F8F6BE}
static const IID IID_IExternalObject =
{ 0xe5e9bf00, 0xa1ab, 0x4b41, { 0xaf, 0xef, 0x98, 0x41, 0xa6, 0xf8, 0xf6, 0xbe } };

BEGIN_INTERFACE_MAP(CExternalObject, CCmdTarget)
	INTERFACE_PART(CExternalObject, IID_IExternalObject, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExternalObject message handlers


long CExternalObject::GetCode() 
{
	return 1;
}
