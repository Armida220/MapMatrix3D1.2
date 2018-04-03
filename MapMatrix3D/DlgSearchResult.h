#if !defined(AFX_DLGSEARCHRESULT_H__82B15C00_F63A_4EA4_B213_4190E36EF04C__INCLUDED_)
#define AFX_DLGSEARCHRESULT_H__82B15C00_F63A_4EA4_B213_4190E36EF04C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSearchResult.h : header file
//
#include "ConditionSelect.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSearchResult dialog

class CDlgSearchResult : public CDialog
{
// Construction
public:
	CDlgSearchResult(CWnd* pParent = NULL);   // standard constructor

	struct ResultState
	{
		ResultState(){
			nResultIdx = -1; nPointIdx = -1; bDeleted = 0;
		}
		int nResultIdx; //结果表中的第几项
		int nPointIdx;  //结果项中的第几个结果点？
		int bDeleted;   //当前结果点是否被删除？
	};
// Dialog Data
	//{{AFX_DATA(CDlgSearchResult)
	enum { IDD = IDD_SEARCH_RESULT };
	CComboBox	m_wndComboType;
	CTreeCtrl	m_wndResultTree;
	//}}AFX_DATA
	afx_msg LONG OnSetResult(WPARAM wParam,LPARAM lParam);
	void SelectLastResult();
	void SelectNextResult();
	void FillTree();
	CCPResultList m_lstResult;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSearchResult)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void UpdateDrawing(BOOL bDraw);
	BOOL GetSelItem(HTREEITEM& item, DWORD_PTR& data);
	CFeature *GetSelFtr(CFeature *pFtr=NULL, int *idx=NULL);
	void DeleteSiblingItem(HTREEITEM item, BOOL bDelParent=FALSE);
	CDocument *m_pDoc;

	//记录每个结果项的状态
	CArray<ResultState,ResultState> m_arrData;

	//记录每个类型对应的属性和值
	CArray<CP_ATTR,CP_ATTR> m_arrType;

	int m_nTimerState;
	GrBuffer m_gr;

	CToolBar m_wndTool;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgSearchResult)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnResultClear();
	afx_msg void OnResultDelallpt();
	afx_msg void OnResultDelobjpt();
	afx_msg void OnResultDelpt();
	afx_msg void OnResultMarkall();
	afx_msg void OnResultMarkobjpt();
	afx_msg void OnResultMarkpt();
	afx_msg void OnResultSelall();
	afx_msg void OnResultSelobj();
	afx_msg void OnResultUnselall();
	afx_msg void OnResultUnselobj();
	afx_msg BOOL OnNeedTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnSelchangeComboType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSEARCHRESULT_H__82B15C00_F63A_4EA4_B213_4190E36EF04C__INCLUDED_)
