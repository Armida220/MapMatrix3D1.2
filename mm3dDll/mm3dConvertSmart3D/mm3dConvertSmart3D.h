
// mm3dConvertSmart3D.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cmm3dConvertSmart3DApp:
// See mm3dConvertSmart3D.cpp for the implementation of this class
//

class Cmm3dConvertSmart3DApp : public CWinApp
{
public:
	Cmm3dConvertSmart3DApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cmm3dConvertSmart3DApp theApp;