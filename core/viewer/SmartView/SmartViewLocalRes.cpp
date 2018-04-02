// SmartViewLocalRes.cpp: implementation of the CSmartViewLocalRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmartViewLocalRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern AFX_EXTENSION_MODULE SmartViewDLL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmartViewLocalRes::CSmartViewLocalRes()
{
	if( AfxGetApp()==NULL )
	{
		AfxSetResourceHandle(SmartViewDLL.hModule);
		return;
	}

	m_hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(SmartViewDLL.hModule);

}

CSmartViewLocalRes::~CSmartViewLocalRes()
{
	if( AfxGetApp()==NULL )
		return;

	AfxSetResourceHandle(m_hInstOld);
}
