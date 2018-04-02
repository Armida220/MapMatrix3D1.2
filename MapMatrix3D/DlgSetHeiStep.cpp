// DlgSetHeiStep.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSetHeiStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetHeiStep dialog


CDlgSetHeiStep::CDlgSetHeiStep(CWnd* pParent /*=NULL*/)
: CDialog(CDlgSetHeiStep::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetHeiStep)
	m_fHeiStep = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgSetHeiStep::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetHeiStep)
	DDX_Text(pDX, IDC_SHS_EDIT_HEISTEP, m_fHeiStep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetHeiStep, CDialog)
//{{AFX_MSG_MAP(CDlgSetHeiStep)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetHeiStep message handlers
