#if !defined(AFX_DLGSELECTLAYER_H__8E8144F8_F03E_4CC8_8EA2_C2BA047B6D16__INCLUDED_)
#define AFX_DLGSELECTLAYER_H__8E8144F8_F03E_4CC8_8EA2_C2BA047B6D16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectLayer.h : header file
//

#include "EditbaseDoc.h"
#include "ColoredListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectLayer dialog

//用来方便地选择方案中的图层

class CDlgSelectLayer : public CDialog
{
// Construction
public:
	CDlgSelectLayer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectLayer)
	enum { IDD = IDD_LAYERCODE_SELECT };
	CListBox	m_wndLayers;
	//}}AFX_DATA

	CDlgDoc *m_pDoc;
	CString m_strLayer;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillLayers();

	CStringArray m_arrLayerNames;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgSelectLayer)
	afx_msg void OnChangeEditCode();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListLayers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//用来方便地选择矢量文件中的图层

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFtrLayer dialog
#define  LAYER_SEL_MODE_SINGLESEL     0
#define  LAYER_SEL_MODE_MUTISEL       1
class CDlgSelectFtrLayer : public CDialog
{
	// Construction
public:
	CDlgSelectFtrLayer(CWnd* pParent = NULL,int nMode = LAYER_SEL_MODE_SINGLESEL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgSelectFtrLayer)
	enum { IDD = IDD_SELECTLAYERS };
	CColoredListCtrl	m_wndLayers;
	CString	m_strLayer;
	BOOL	m_bArea;
	BOOL	m_bLine;
	BOOL	m_bLocal;
	BOOL	m_bNotLocal;
	BOOL	m_bPoint;
	BOOL	m_bText;
	BOOL	m_bUsed;
	CString	m_strLayers;
	CString m_SingleLayer;
	//}}AFX_DATA
	
	CDlgDataSource *m_pDS;
	CFtrLayer *m_pRetLayer;
	CComboBox m_wndEdit;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectFtrLayer)
public:
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	void FillLayers();

	void UpdateLayerListDisplay();

	int m_nSelMode;

//	CString m_strLayers;

	BOOL m_bEditChanged;
	int m_nTimeElapes;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgSelectFtrLayer)
	virtual void OnOK();
	afx_msg void OnChangeEditLayername();
	afx_msg void OnDblclkListLayers(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeLayername();
	afx_msg void OnButtonAdd();
	afx_msg void OnChangeEditLayers();
	afx_msg void OnButtonClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTLAYER_H__8E8144F8_F03E_4CC8_8EA2_C2BA047B6D16__INCLUDED_)
