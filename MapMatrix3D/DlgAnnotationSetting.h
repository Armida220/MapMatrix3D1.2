#if !defined(AFX_DLGANNOTATIONSETTING_H__AAA9E7FB_E2CB_4699_938B_232EC201E459__INCLUDED_)
#define AFX_DLGANNOTATIONSETTING_H__AAA9E7FB_E2CB_4699_938B_232EC201E459__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAnnotationSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAnnotationSetting dialog
#include "Symbol.h"
#include "UIFSimpleEx.h"

class CDlgAnnotationSetting : public CSonDialog
{
// Construction
public:
	CDlgAnnotationSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAnnotationSetting)
	enum { IDD = IDD_ANNOTATION_DIALOG };
	CComboBox	m_cFieldCombo;
	CComboBox	m_cFontName;
	CComboBox	m_cInclineType;
	CComboBox	m_cTextAligntype;
	CComboBox	m_cAnnoType;
	CComboBox	m_cPlaceType;
	float	m_lfCharHei;
	float	m_lfCharWidScale;
	float	m_lfCharIntvScale;
	int		m_nDigit;
	CString	m_strFontName;
	float	m_lfInclineAngle;
	float	m_lfLineSpaceScale;
	float	m_fXOff;
	float	m_fYOff;
	CString m_strText;
	CComboBox       m_cCoverType;
	float   m_fExtendDis;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAnnotationSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAnnotationSetting)
	afx_msg void OnChangeCharheiEdit();
	afx_msg void OnChangeCharintvEdit();
	afx_msg void OnChangeCharwidEdit();
	afx_msg void OnChangeDigitEdit();
	afx_msg void OnChangeInclineangleEdit();
	afx_msg void OnChangeLineintvEdit();
	afx_msg void OnChangePlacetypeEdit();
	afx_msg void OnChangeTextEdit();
	afx_msg void OnChangeXoffEdit();
	afx_msg void OnChangeYoffEdit();
	afx_msg void OnSelchangePlacetypeCombo();
	afx_msg void OnSelchangeAnnotypeCombo();
	afx_msg void OnSelchangeTextAligntypeCombo();
	afx_msg void OnSelchangeInclinetypeCombo();
	afx_msg void OnSelchangeFontNameCombo();
	afx_msg void OnSelchangeFieldCombo();
	afx_msg void OnChangeExtendDisEdit();
	afx_msg void OnSelchangeCovertypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	// tabÎªÊôÐÔ
	void SetMem(CAnnotation *pAnno, CValueTable &tab);
	void UpdataFieldComobo();
	
	
	CAnnotation *m_pAnno;
	int		m_nAnnoType;
	int		m_nPlaceType;

	CString m_strField;

	CStringArray m_arrFont;

	CValueTable m_annTab;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGANNOTATIONSETTING_H__AAA9E7FB_E2CB_4699_938B_232EC201E459__INCLUDED_)
