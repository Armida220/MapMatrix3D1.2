#if !defined(AFX_CDlgSetColor_H__87738D4E_8FD8_4822_9D1C_A4F3279AA0C7__INCLUDED_)
#define AFX_CDlgSetColor_H__87738D4E_8FD8_4822_9D1C_A4F3279AA0C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgSetColor.h : header file
//

#include "EditBase.h"
#include "GridCtrl_src/GridCtrl.h"


class CDlgDoc;
class CDlgDataSource;

namespace EditBase{

class CAttributesSource;

}

/////////////////////////////////////////////////////////////////////////////
// CDlgSetColor dialog

class CDlgSetColor : public CDialog
{
// Construction
public:
	CDlgSetColor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetColor)
	enum { IDD = IDD_SETCOLOR_BYFIELD };
	CButton	m_button3;
	CComboBox	m_combo;
	CButton	m_button2;
	CButton	m_button1;
	CGridCtrl m_gridCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetColor)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Init(CDlgDoc* pDoc);

private:
	CDlgDataSource *m_pDS;
	CAttributesSource *m_pXDS;
	CStringArray m_arrFields;
	CStringArray m_arrVals;
	CFtrArray m_ftrs;
	CArray<int,int> m_defaultColors;
	CArray<int,int> m_arrColors;
	CDlgDoc *m_pDoc;

	void ShowField(int index);
	void SetGridReadOnly(int row, int col)
	{
		int nState = m_gridCtrl.GetItemState(row, col);
		m_gridCtrl.SetItemState(row, col, nState|GVIS_READONLY);
	}

	afx_msg void OnOK(){}//屏蔽回车关对话框的功能
	afx_msg BOOL PreTranslateMessage(MSG* pMsg) 
	{
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
			return TRUE;
		return CDialog::PreTranslateMessage(pMsg);
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDlgSetColor_H__87738D4E_8FD8_4822_9D1C_A4F3279AA0C7__INCLUDED_)
