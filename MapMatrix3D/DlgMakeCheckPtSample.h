#if !defined(AFX_DLGMAKECHECKPTSAMPLE_H__11E32DE2_044C_411A_98DE_E2BAD50DE3EF__INCLUDED_)
#define AFX_DLGMAKECHECKPTSAMPLE_H__11E32DE2_044C_411A_98DE_E2BAD50DE3EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMakeCheckPtSample.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMakeCheckPtSample dialog

class CDlgMakeCheckPtSample : public CDialog
{
// Construction
public:
	CDlgMakeCheckPtSample(CWnd* pParent = NULL);   // standard constructor

	struct TempSaveData
	{
		TempSaveData()
		{
			bInited = FALSE;
		}
		BOOL    bInited;

		int		m_nCrossWidth;
		CString	m_strImageID;
		int		m_nImageWidth;
		int		m_nImageHigh;
		int		m_nFontSize;
		int     m_nImgSoure;
		CString	m_strNoteText;
		CString	m_strImagePath;
		CString	m_strPtID;
		
		COLORREF m_clrFont;
		COLORREF m_clrCross;
	};

// Dialog Data
	//{{AFX_DATA(CDlgMakeCheckPtSample)
	enum { IDD = IDD_MAKE_CHECKPTSAMPLE };
	int		m_nCrossWidth;
	CString	m_strImageID;
	int		m_nImageWidth;
	int		m_nImageHigh;
	int		m_nFontSize;
	int     m_nImgSoure;
	CString	m_strNoteText;
	CString	m_strImagePath;
	CString	m_strPtID;
	//}}AFX_DATA

	COLORREF m_clrFont;
	COLORREF m_clrCross;
	CString  m_strFontName;

	CComboBox m_wndComboFont;
	CMFCColorButton m_wndFontColor;
	CMFCColorButton m_wndCrossColor;

	void IncreasePtID();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMakeCheckPtSample)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMakeCheckPtSample)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAKECHECKPTSAMPLE_H__11E32DE2_044C_411A_98DE_E2BAD50DE3EF__INCLUDED_)
