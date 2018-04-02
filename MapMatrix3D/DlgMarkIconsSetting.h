#if !defined(AFX_DLGMARKICONSSETTING_H__625E2DBF_0BBA_488E_8CDD_E5EE98870532__INCLUDED_)
#define AFX_DLGMARKICONSSETTING_H__625E2DBF_0BBA_488E_8CDD_E5EE98870532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMarkIconsSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMarkIconsSetting dialog

class CDlgDoc;
class CChkResSettings;

class CDlgMarkIconsSetting : public CDialog
{
// Construction
public:
	CDlgMarkIconsSetting(CWnd* pParent = NULL);   // standard constructor

	virtual ~CDlgMarkIconsSetting();

// Dialog Data
	//{{AFX_DATA(CDlgMarkIconsSetting)
	enum { IDD = IDD_RESULTMARK_ICONSETTINGS };
	CListCtrl	m_wndListIcons;
	float	m_fSize;
	CListBox m_wndChkCmd;
	CListBox m_wndReasons;
	//}}AFX_DATA

	CMFCColorButton m_wndColor;

	long m_nScale;

	CDlgDoc *m_pDoc;
	CChkResSettings *m_pSettings;

	CChkResSettings *m_pSaveSettings;

	int m_nCurSelCmd, m_nCurSelReason;

protected:
	int m_nImageWid, m_nImageHei;
	CImageList m_listImages;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarkIconsSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void SetDoc(CDlgDoc *pDoc, CChkResSettings *pSettings);

// Implementation
protected:

	void SaveSomeParameters();

	void FillSymbolList();	
	void CreateImageList(int cx, int cy, int nsize);
	BOOL CreateImageItem(int idx);
	void DrawSymbol(CellDef *def, GrBuffer2d *buf);
	void DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf);

	// Generated message map functions
	//{{AFX_MSG(CDlgMarkIconsSetting)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnSet();
	afx_msg void OnAutoSet();
	afx_msg void OnAutoSetAll();
	afx_msg void OnTop();
	afx_msg void OnSelchangeChkCmd();
	afx_msg void OnSelchangeReasons();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMARKICONSSETTING_H__625E2DBF_0BBA_488E_8CDD_E5EE98870532__INCLUDED_)
