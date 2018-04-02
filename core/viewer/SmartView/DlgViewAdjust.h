#if !defined(AFX_DLGVIEWADJUST_H__B3A803F7_8592_46DA_9361_D04FF964C19C__INCLUDED_)
#define AFX_DLGVIEWADJUST_H__B3A803F7_8592_46DA_9361_D04FF964C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgViewAdjust.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgViewAdjust dialog


typedef LRESULT (CObject::*PFUNCALLBACK)(WPARAM wParam, LPARAM lParam);


class CDlgViewAdjust : public CDialog
{
// Construction
public:
	void SetService(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam);
	void SetService(CObject *pObj,PFUNCALLBACK func, WPARAM wParam, LPARAM lParam);
	void UpdateViewWnd();
	CDlgViewAdjust(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgViewAdjust)
	enum { IDD = IDD_DIALOG_ADJUST };
	int		m_brightness;
	int		m_contrast;
	int		m_brightness2;
	int		m_contrast2;

	BOOL	m_bLBlue;
	BOOL	m_bLGreen;
	BOOL	m_bLRed;
	BOOL	m_bRBlue;
	BOOL	m_bRGreen;
	BOOL	m_bRRed;
	
	BOOL	m_bLAutoHistogram;
	BOOL	m_bRAutoHistogram;

	BOOL	m_bSameRight;
	//}}AFX_DATA
	int		m_initb[2];
	int		m_initc[2];
	BOOL	m_bShowBand;
	BOOL	m_bShowRight;
private:
	MSG  m_updateMsg;
	BOOL m_bInitDialog;

	CObject *m_pObj;
	PFUNCALLBACK m_pFunc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgViewAdjust)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgViewAdjust)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinBrightness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinContrast(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditBrightness();
	afx_msg void OnChangeEditContrast();
	afx_msg void OnDeltaposSpinBrightness2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinContrast2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditBrightness2();
	afx_msg void OnChangeEditContrast2();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCheckSametoleft();
	afx_msg void OnCheckBank();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateSlider(UINT id, BOOL bSet=TRUE);
	void UpdateEdit(UINT id, BOOL bSet=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVIEWADJUST_H__B3A803F7_8592_46DA_9361_D04FF964C19C__INCLUDED_)
