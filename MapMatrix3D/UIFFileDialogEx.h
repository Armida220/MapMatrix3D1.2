// UIFFileDialogEx.h: interface for the CFileDialogExEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFFILEDIALOGEX_H__99C240DB_3424_40A3_93EA_69E2CDB4138B__INCLUDED_)
#define AFX_UIFFILEDIALOGEX_H__99C240DB_3424_40A3_93EA_69E2CDB4138B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileDialogEx : public CFileDialog
{
public:
	CFileDialogEx(LPCTSTR lpszCaption,
		BOOL bOpenFileDialog = TRUE,
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

private:
	TCHAR m_szInitDir[_MAX_PATH];
	TCHAR m_szTitle[128];
};

#endif // !defined(AFX_UIFFILEDIALOGEX_H__99C240DB_3424_40A3_93EA_69E2CDB4138B__INCLUDED_)
