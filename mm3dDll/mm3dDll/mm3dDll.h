// mm3dDll.h : main header file for the mm3dDll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "IMm3dDLL.h"
#include "mm3d_app.h"

// Cmm3dDllApp
// See mm3dDll.cpp for the implementation of this class
//

class CMm3dDll : public IMm3dDLL
{
public:
	CMm3dDll();
	virtual ~CMm3dDll();

	virtual void showDlg();

private:

};



class Cmm3dDllApp : public CWinApp
{
public:
	Cmm3dDllApp();

// Overrides
protected:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

	BOOL m_bCEFInitialized;// «∑Ò≥ı ºªØ

	CefRefPtr<CMM3dApp> m_cefApp;
public:
	virtual int ExitInstance();
};
