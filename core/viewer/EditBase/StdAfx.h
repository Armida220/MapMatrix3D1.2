// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__039D274B_1388_45FE_8DCB_E03064EF9B39__INCLUDED_)
#define AFX_STDAFX_H__039D274B_1388_45FE_8DCB_E03064EF9B39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WINVER	0x500
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxtempl.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxpriv.h>
#include <afxext.h>

//#define USE_MEMPOOL

#pragma warning(disable:4244)
#pragma warning(disable:4800)
#pragma warning(disable:4267)
#pragma warning(disable:4996)

#include <stdlib.h>
#include <math.h>
#include <comdef.h>
#include <memory.h>
#include "SmartViewDef.h"
#include "EditbaseDef.h"
#include "SmartViewBaseType.h"
#include "Envelope.h"
#include <afxcontrolbars.h>

using namespace EditBase;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__039D274B_1388_45FE_8DCB_E03064EF9B39__INCLUDED_)
