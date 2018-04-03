#if !defined(AFX_DLGPLUGINSMANAGE_H__117186DE_7648_4152_BCA0_A800DFEE9A4A__INCLUDED_)
#define AFX_DLGPLUGINSMANAGE_H__117186DE_7648_4152_BCA0_A800DFEE9A4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPluginsManage.h : header file
//
//#include "PropList.h"
#include "resource.h"
#include "EditBaseDoc.h"
#include "BaseView.h"
#include "MyPropList.h"
using namespace MyList;
/////////////////////////////////////////////////////////////////////////////
// CDlgPluginsManage dialog
#define FIELDNAME_PLUGINNAME		"PluginName"
#define FIELDNAME_PLUGINUSEState    "PluginUseState"

class CDlgPluginsManage : public CDialog
{
// Construction
public:
	CDlgPluginsManage(CWnd* pParent = NULL);   // standard constructor

	void FillPluginsList();

// Dialog Data
	//{{AFX_DATA(CDlgPluginsManage)
	enum { IDD = IDD_PLUGINS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPluginsManage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AdjustLayout();

	// Generated message map functions
	//{{AFX_MSG(CDlgLayerVisible)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg LRESULT OnSelChange(WPARAM   wParam,LPARAM   lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CLVLPropList0	m_wndPluginsList;
	CDlgDoc*		m_pDlgDoc;
public:
	CBaseView      *m_pBaseView;
protected:
	PlugItem	*m_pPlugItem;
	int			m_PlugSize;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPLUGINSMANAGE_H__117186DE_7648_4152_BCA0_A800DFEE9A4A__INCLUDED_)
