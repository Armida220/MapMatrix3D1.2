#if !defined(AFX_DLGBATEXPORTMAPS_H__F048AED9_2DDD_4CDB_9100_15D1FFA5ABB4__INCLUDED_)
#define AFX_DLGBATEXPORTMAPS_H__F048AED9_2DDD_4CDB_9100_15D1FFA5ABB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBatExportMaps.h : header file
//

#include "Mapdecorator.h"
#include "DlgOpSettings.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBatExportMaps dialog

class CDlgBatExportMaps : public CDlgOpSettings
{
// Construction
public:
	CDlgBatExportMaps(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBatExportMaps)
	enum { IDD = IDD_BAT_EXPORTMAPS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	MapDecorate::CMapDecorator *m_pMapDecorator;
	BOOL m_bNew;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBatExportMaps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnMapDecorateSettings();
	// Generated message map functions
	//{{AFX_MSG(CDlgBatExportMaps)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBATEXPORTMAPS_H__F048AED9_2DDD_4CDB_9100_15D1FFA5ABB4__INCLUDED_)
