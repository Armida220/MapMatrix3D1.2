#if !defined(AFX_DLGPROCSYMLTSETTING_H__1035F505_3010_4511_9847_B823B255855E__INCLUDED_)
#define AFX_DLGPROCSYMLTSETTING_H__1035F505_3010_4511_9847_B823B255855E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgProcSymLTSetting.h : header file
//
#include "Symbol.h"
#include "UIFSimpleEx.h"
#include "SymbolLib.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgProcSymLTSetting dialog


//此对话框由粮仓、简单房屋、通车水闸、不通车水闸共用
class CDlgProcSymLTSetting : public CDialog
{
// Construction
public:
	CDlgProcSymLTSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProcSymLTSetting)
	enum { IDD = IDD_PROCSYM_LT_SETTING };
	CString	m_strName;
	float	m_fWidth;
	//}}AFX_DATA

	CProcSymbol_LT *m_pSymbol;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcSymLTSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProcSymLTSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnChangeWidthEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetMem(CProcSymbol_LT *pScale, ConfigLibItem m_config);
	
	ConfigLibItem m_config;
};


//此对话框由油罐使用
class CDlgScaleYouGuanSetting : public CDialog
{
	// Construction
public:
	CDlgScaleYouGuanSetting(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgProcSymLTSetting)
	enum { IDD = IDD_YOUGUAN_SETTING };
	CString	m_strName;
	float	m_fWidth;
	BOOL	m_bFilled;
	//}}AFX_DATA
	
	CScaleYouGuan *m_pSymbol;	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcSymLTSetting)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgProcSymLTSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnCheckFilled();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetMem(CScaleYouGuan *pScale, ConfigLibItem m_config);
	
	ConfigLibItem m_config;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROCSYMLTSETTING_H__1035F505_3010_4511_9847_B823B255855E__INCLUDED_)
