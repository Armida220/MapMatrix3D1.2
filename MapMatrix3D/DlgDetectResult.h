#if !defined(AFX_DLGDETECTRESULT_H__9D8ADBE2_EC25_4E83_9E3E_2E71E739F5DC__INCLUDED_)
#define AFX_DLGDETECTRESULT_H__9D8ADBE2_EC25_4E83_9E3E_2E71E739F5DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDetectResult.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDetectResult dialog
class CDlgDoc;
class CDlgDetectResult : public CDialog
{
	struct item
	{
		CFeature* pFtr0;
//		PT_3DEX pt0;
		CFeature* pFtr1;
		PT_3DEX pt;
	};

// Construction
public:
	CDlgDetectResult(CWnd* pParent = NULL);   // standard constructor
	void Add(CFeature *pFtr0,CFeature *pFtr1);
	void ClearData();
	void RefreshData();
	CDlgDoc *m_pDoc;
// Dialog Data
	//{{AFX_DATA(CDlgDetectResult)
	enum { IDD = IDD_OVERLAPPOINT_RESULT };
	CListBox	m_listResult;
	//}}AFX_DATA
protected:
	CToolBar m_toolBar;
	
	CArray<item,item&> m_arrRetObjs;

	int m_nTimerState;
	GrBuffer m_gr;
private:
		void UpdateDrawing(BOOL bDraw);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDetectResult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnCancel();
	// Generated message map functions
	//{{AFX_MSG(CDlgDetectResult)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeListResults();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	afx_msg BOOL OnNeedTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnSelObj();
	afx_msg void OnResultMarkall();
	afx_msg void OnResultUnselall();
	afx_msg void OnResultClear();
	afx_msg void OnResultSelall();
	afx_msg void OnResultDelall();
	afx_msg void OnResultDelobj();
	afx_msg void OnResultMarkObj();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDETECTRESULT_H__9D8ADBE2_EC25_4E83_9E3E_2E71E739F5DC__INCLUDED_)
