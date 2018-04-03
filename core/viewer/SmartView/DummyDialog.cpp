// DummyDialog.cpp: implementation of the CDummyDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DummyDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDummyDialog, CDialog)
	//{{AFX_MSG_MAP(CDummyDialog)
	ON_MESSAGE(WM_THREADMSG,OnThreadMsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDummyDialog::Create2(HINSTANCE hInst, UINT nIDTemplate, CWnd* pParentWnd)
{
	if( AfxGetApp()!=NULL )
	{
		HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(nIDTemplate), RT_DIALOG);
		HGLOBAL hTemplate = LoadResource(hInst, hResource);

		LPCDLGTEMPLATE lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);

		HWND hWnd = ::CreateDialogIndirect(hInst, lpDialogTemplate,
				pParentWnd->GetSafeHwnd(), (DLGPROC)::DefWindowProc);

		DWORD err = GetLastError();

		UnlockResource(hTemplate);

		FreeResource(hTemplate);

		SubclassWindow(hWnd);
	}
	else
	{
		return Create(nIDTemplate, pParentWnd);
	}

	return TRUE;
}