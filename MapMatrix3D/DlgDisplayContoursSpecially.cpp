// DlgDisplayContoursSpecially.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgDisplayContoursSpecially.h"
#include "RegDef.h"
#include "RegDef2.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGPATH_VIEWCONTOURS_COLOR	"Config\\ViewContoursColor"
/////////////////////////////////////////////////////////////////////////////
// CDlgDisplayContoursSpecially dialog


CDlgDisplayContoursSpecially::CDlgDisplayContoursSpecially(CWnd* pParent /*=NULL*/)
: CDialog(CDlgDisplayContoursSpecially::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDisplayContoursSpecially)
	m_fInterval = GetProfileDouble(REGPATH_CONFIG,REGITEM_HEISTEP,5.0);
	m_nInterNum = 4;
	//}}AFX_DATA_INIT
	
	m_clr0 = RGB(255,0,0);
	m_clr1 = RGB(255,255,0);
	m_clr2 = RGB(0,255,255);
	m_clr3 = RGB(0,255,0);
	m_clr4 = RGB(0,0,255);
}


void CDlgDisplayContoursSpecially::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDisplayContoursSpecially)
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	DDX_Text(pDX, IDC_EDIT_INTERNUM, m_nInterNum);
	DDX_Control( pDX, IDC_BUTTON_INDEX_COLOR,m_btnColor0);
	DDX_Control( pDX, IDC_BUTTON_INTER_COLOR1,m_btnColor1);	
	DDX_Control( pDX, IDC_BUTTON_INTER_COLOR2,m_btnColor2);	
	DDX_Control( pDX, IDC_BUTTON_INTER_COLOR3,m_btnColor3);	
	DDX_Control( pDX, IDC_BUTTON_INTER_COLOR4,m_btnColor4);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDisplayContoursSpecially, CDialog)
//{{AFX_MSG_MAP(CDlgDisplayContoursSpecially)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDisplayContoursSpecially message handlers

BOOL CDlgDisplayContoursSpecially::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	m_clr0 = pApp->GetProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color0",m_clr0);
	m_clr1 = pApp->GetProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color1",m_clr1);
	m_clr2 = pApp->GetProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color2",m_clr2);
	m_clr3 = pApp->GetProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color3",m_clr3);
	m_clr4 = pApp->GetProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color4",m_clr4);
	
	// TODO: Add extra initialization here
	m_btnColor0.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_btnColor1.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_btnColor2.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_btnColor3.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_btnColor4.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	
	m_btnColor0.SetColor(m_clr0);
	m_btnColor1.SetColor(m_clr1);
	m_btnColor2.SetColor(m_clr2);
	m_btnColor3.SetColor(m_clr3);
	m_btnColor4.SetColor(m_clr4);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDisplayContoursSpecially::OnOK() 
{
	UpdateData(TRUE);
	
	// TODO: Add extra validation here
	m_clr0 = m_btnColor0.GetColor();
	m_clr1 = m_btnColor1.GetColor();
	m_clr2 = m_btnColor2.GetColor();
	m_clr3 = m_btnColor3.GetColor();
	m_clr4 = m_btnColor4.GetColor();
	
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	pApp->WriteProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color0",m_clr0);	
	pApp->WriteProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color1",m_clr1);
	pApp->WriteProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color2",m_clr2);
	pApp->WriteProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color3",m_clr3);
	pApp->WriteProfileInt(REGPATH_VIEWCONTOURS_COLOR,"Color4",m_clr4);
	
	CDialog::OnOK();
}
