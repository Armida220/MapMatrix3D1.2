#if !defined(AFX_DLGMAKECHECKPTSAMPLEBATCH_H__11E32DE2_045C_411A_98DE_E2BAD50DE3EF__INCLUDED_)
#define AFX_DLGMAKECHECKPTSAMPLEBATCH_H__11E32DE2_045C_411A_98DE_E2BAD50DE3EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMakeCheckPtSample.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMakeCheckPtSampleBatch dialog

class CDlgMakeCheckPtSampleBatch : public CDialog
{
// Construction
public:
	CDlgMakeCheckPtSampleBatch(CWnd* pParent = NULL);   // standard constructor

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
		int		m_nRotationAngle;
		CString	m_strImagePath;
		CString	m_strPtID;
		
		COLORREF m_clrFont;
		COLORREF m_clrCross;
		BOOL	m_bPointNumFromFile;
		CString	m_strPointFilePath;
		CString	m_strProjectFilePath;
	};

// Dialog Data
	//{{AFX_DATA(CDlgMakeCheckPtSampleBatch)
	enum { IDD = IDD_MAKE_CHECKPTSAMPLE_BATCH };
	int		m_nCrossWidth;
	CString	m_strImageID;
	int		m_nImageWidth;
	int		m_nImageHigh;
	int		m_nFontSize;
	int     m_nImgSoure;
	int		m_nRotationAngle;
	CString	m_strNoteText;
	CString	m_strImagePath;
	CString	m_strPtID;
	CString	m_strPointFilePath;
	CString	m_strProjectFilePath;
	//}}AFX_DATA

	COLORREF m_clrFont;
	COLORREF m_clrCross;
	CString  m_strFontName;
	float m_lfRotationAngle;//Ðý×ª½Ç¶È

	CComboBox m_wndComboFont;
	CComboBox m_wndComboRotation;
	CMFCColorButton m_wndFontColor;
	CMFCColorButton m_wndCrossColor;
	BOOL m_bPointNumFromFile;

	void IncreasePtID();
	void UpdatePointFilePathState();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMakeCheckPtSampleBatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMakeCheckPtSampleBatch)
	afx_msg void OnButtonBrowse();
	afx_msg void OnPointFileBrowse();
	afx_msg void OnProjectFileBrowse();
	afx_msg void OnCheckPointNumFromFile();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAKECHECKPTSAMPLEBATCH_H__11E32DE2_045C_411A_98DE_E2BAD50DE3EF__INCLUDED_)
