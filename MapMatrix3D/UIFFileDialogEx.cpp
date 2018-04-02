// UIFFileDialogEx.cpp: implementation of the CFileDialogExEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "UIFFileDialogEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileDialogEx::CFileDialogEx(LPCTSTR lpszCaption, BOOL bOpenFileDialog,
	LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
	DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	if (lpszCaption)
	{
		strcpy(m_szTitle, lpszCaption);
		m_pOFN->lpstrTitle = m_szTitle;
	}

	if (lpszFileName)
	{
		strcpy(m_szInitDir, lpszFileName);
		TCHAR *pos = strrchr(m_szInitDir, '\\');
		if (pos > 0)*pos = 0;
		m_pOFN->lpstrInitialDir = m_szInitDir;
	}
}