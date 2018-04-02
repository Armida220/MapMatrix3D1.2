// DlgCulvertSurfaceSetting.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCulvertSurfaceSetting.h"
#include "editbase.h"
#include "ExMessage.h"
#include "DlgCellDefLinetypeView.h"
#include "SmartViewFunctions.h"
#include "SilenceDDX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCulvertSurfaceSetting dialog


CDlgCulvertSurfaceSetting::CDlgCulvertSurfaceSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCulvertSurfaceSetting::IDD, pParent)
{
	m_LineType = "";
	m_LineWidth = 0;
	m_pPara1 = NULL;
	m_pPara2 = NULL;
}


void CDlgCulvertSurfaceSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCulvertSurfaceSetting)
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_LineWidth);
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_LineType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCulvertSurfaceSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgCulvertSurfaceSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCulvertSurfaceSetting message handlers

void CDlgCulvertSurfaceSetting::SetMem(CCulvertSurface1Symbol *pPara, ConfigLibItem config)
{
	m_pPara1 = pPara;
	m_pPara2 = NULL;
	m_config = config;
	
	if(m_pPara1==NULL || m_config.pLinetypeLib==NULL)
		return;
	
	m_LineWidth = m_pPara1->m_fWidth;
	m_LineType = m_pPara1->m_strLinetypeName;
}

void CDlgCulvertSurfaceSetting::SetMem(CCulvertSurface2Symbol *pPara, ConfigLibItem config)
{
	m_pPara2 = pPara;
	m_pPara1 = NULL;
	m_config = config;
	
	if(m_pPara2==NULL || m_config.pLinetypeLib==NULL)
		return;
	
	m_LineWidth = m_pPara2->m_fWidth;
	m_LineType = m_pPara2->m_strLinetypeName;
}

void CDlgCulvertSurfaceSetting::OnPreviewButton()
{
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);
	
	CDlgCellDefLinetypeView dlg;
	
	CSwitchScale scale(m_config.GetScale());
	// 	dlg.SetLineTypeLib(m_pBaseLib);
	dlg.SetShowMode(FALSE,TRUE,1,m_LineType);
	// 	dlg.SetScale(m_dScale);
	dlg.SetConfig(m_config);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_LineType = dlg.GetName();
	if(m_pPara1)
		m_pPara1->m_strLinetypeName = dlg.GetName();
	else if(m_pPara2)
		m_pPara2->m_strLinetypeName = dlg.GetName();

	UpdateData(FALSE);
		
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
}

void CDlgCulvertSurfaceSetting::OnChangeWidthEdit()
{
	UpdateData(TRUE);

	if(m_pPara1)
		m_pPara1->m_fWidth = m_LineWidth;
	else if(m_pPara2)
		m_pPara2->m_fWidth = m_LineWidth;
	else
		return;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCulvertSurfaceSetting::OnChangeLinetypenameEdit()
{
	UpdateData(TRUE);

	if(m_pPara1)
		m_pPara1->m_strLinetypeName = m_LineType;
	else if(m_pPara2)
		m_pPara2->m_strLinetypeName = m_LineType;
	else
		return;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}