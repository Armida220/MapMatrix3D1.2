#if !defined(AFX_DLGLAYERVISIBLE_H__117186DE_7648_4152_BCA0_A800DFEE9A4A__INCLUDED_)
#define AFX_DLGLAYERVISIBLE_H__117186DE_7648_4152_BCA0_A800DFEE9A4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlglayervisible.h : header file
//
#include "PropList.h"
#include "resource.h"
#include "EditBaseDoc.h"
#include "BaseView.h"
#include "LayersMgrDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgLayerVisible dialog

class CDlgLayerVisible : public CManageBaseDlg
{
// Construction
public:
	CDlgLayerVisible(CWnd* pParent = NULL);   // standard constructor

	virtual int GetManageType() { return ManageLayerVisible; } 

	void FillAllLayersList(CDlgDataSource *pDS = NULL);
	virtual void ModifyAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc ,_variant_t exData);
	virtual void ModifyCheckState(CLVLPropItem **pItems, int num, CLVLPropColumn* pCol, int col, _variant_t value);


// Dialog Data
	//{{AFX_DATA(CDlgLayerVisible)
	enum { IDD = IDD_LAYERLIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLayerVisible)
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
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CLVLPropList	m_wndPropListCtrl;
	CDlgDoc*		m_pDlgDoc;
public:
	CBaseView      *m_pBaseView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLAYERVISIBLE_H__117186DE_7648_4152_BCA0_A800DFEE9A4A__INCLUDED_)
