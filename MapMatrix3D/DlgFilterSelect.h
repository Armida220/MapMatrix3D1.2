#if !defined(AFX_DLGFILTERSELECT_H__42848F7E_ED5B_48FA_8748_5964FA6190FB__INCLUDED_)
#define AFX_DLGFILTERSELECT_H__42848F7E_ED5B_48FA_8748_5964FA6190FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFilterSelect.h : header file
//

#include "UIFPropListExPlus.h"

#include "UIFPropEx.h"
#include "QueryMenu.h"

#define FIELDNAME_LINETYPE		"LineType"
#define FIELDNAME_EQUALZ		"EqualZ"
#define FIELDNAME_CLOSED		"Closed"
#define FIELDNAME_CLOCKWISE		"Clockwise"
#define FIELDNAME_HEIGHT		"Height"
#define FIELDNAME_POINTNUM		"PointNum"
#define FIELDNAME_LENGTH		"Length"
#define FIELDNAME_AREA			"Area"
#define FIELDNAME_OTHERCONDS	"OtherConds"
#define FIELDNAME_RANGE			"Range"
#define FIELDNAME_REVERSE		"Reverse"

class CUIFConditionsProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFConditionsProp)
		
public:
	CUIFConditionsProp(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFConditionsProp();
	
	virtual void OnClickButton(CPoint point);

	CStringArray m_arrFields;
	CondMenu m_condMenu;
};

class CDlgDoc;

/////////////////////////////////////////////////////////////////////////////
// CDlgFilterSelect dialog

class CDlgFilterSelect : public CDialog
{
// Construction
public:
	CDlgFilterSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFilterSelect)
	enum { IDD = IDD_FILTERSELECT };
	BOOL	m_bArea;
	BOOL	m_bDirPoint;
	BOOL	m_bDLine;
	BOOL	m_bInverse;
	BOOL	m_bLine;
	BOOL	m_bParallel;
	BOOL	m_bPoint;
	BOOL	m_bText;
	BOOL    m_bAreaPoint;
	//}}AFX_DATA

	BOOL	m_nRange;

	CUIFPropListEx m_wndPropList;
	CStringArray m_names;

	CValueTable m_tab;
	CondMenu m_condMenu;
	CDlgDoc *m_pDoc;

	// 用于 OnSize 中调整位置时使用
	CRect m_rcStaticRect, m_rcClient;

	void RefreshProps();
	void OnDo();
	void OnUnDo();
	void OnClearSettings();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFilterSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void FillPropList();
	void GetPropValues(CValueTable& tab);

	void DeleteFieldFromArray(CStringArray& a1, CStringArray& a2, LPCTSTR field);

	void GetColors(CArray<COLORREF,COLORREF>& arrColors);
	void GetFtrCodes(CStringArray& names);
	void GetGroupNames(CStringArray& names);
	void GetTextContents(CStringArray& names);
	void GetTextFonts(CStringArray& names);
	void GetSymbolNames(CStringArray& names);

	// Generated message map functions
	//{{AFX_MSG(CDlgFilterSelect)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckPoint();
	afx_msg void OnCheckLine();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnPropertyChanged (WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFILTERSELECT_H__42848F7E_ED5B_48FA_8748_5964FA6190FB__INCLUDED_)
