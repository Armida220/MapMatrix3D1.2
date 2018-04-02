// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1AEAACB1_FB94_40DA_A93B_11498B799E38__INCLUDED_)
#define AFX_STDAFX_H__1AEAACB1_FB94_40DA_A93B_11498B799E38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define _WIN32_WINNT		0x500
//#define WINVER				0x500

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS
/*#pragma warning(disable:4244)
#pragma warning(disable:4800)
#pragma warning(disable:4267)*/
#pragma warning(disable:4996)

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxtempl.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#include <stdlib.h>
#include <math.h>
#include <comdef.h>

//#define USE_MEMPOOL

#include "EditBaseDef.h"
#include "SmartViewDef.h"
#include "SmartViewBaseType.h"
#include "Envelope.h"
#include "FBHelperFunc.h"

using namespace EditBase;

#include "ThemidaSDK.h"

#undef UNICODE
#undef _UNICODE

	#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CEditBaseModule : public CComModule
{
public:
	LONG Unlock();
	LONG Lock();
	LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2);
	DWORD dwThreadID;
};
extern CEditBaseModule _Module;
#include <atlcom.h>

//#include "OdaToolkit.h"
//#import  "DWGdirectX.tlb" named_guids

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1AEAACB1_FB94_40DA_A93B_11498B799E38__INCLUDED_)
