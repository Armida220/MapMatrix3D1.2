#if !defined(AFX_DLGANNOTIP_H__89A81F01_81CD_4E87_8F87_C6A20032F6C2__INCLUDED_)
#define AFX_DLGANNOTIP_H__89A81F01_81CD_4E87_8F87_C6A20032F6C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAnnoTip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAnnoTip dialog

class CStringScoreMap
{
public:
	struct SCORE{
		CString item;
		float scroe;
	};
public:
	BOOL GetScore(CString item, float& scroe);
	void SetOrAddScore(CString item, float scroe);
	void DeleteScore(CString item);
	void DeleteAll();
private:
	CArray<SCORE,SCORE> arr;
};

class CDlgAnnoTip : public CDialog
{
// Construction
public:
	CDlgAnnoTip(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAnnoTip();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Dialog Data
	//{{AFX_DATA(CDlgAnnoTip)
	enum { IDD = IDD_ANNOTIP };
	CTreeCtrl	m_treeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAnnoTip)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Load();
	void ExpandAll();
	//void Save();

//	afx_msg void OnClose();
	afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnExecuteItem();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();

protected:
	CString m_filepath;
	CStringScoreMap m_map;

	// Generated message map functions
	//{{AFX_MSG(CDlgAnnoTip)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGANNOTIP_H__89A81F01_81CD_4E87_8F87_C6A20032F6C2__INCLUDED_)
