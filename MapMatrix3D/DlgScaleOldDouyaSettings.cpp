// DlgScaleOldDouyaSettings.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "DlgScaleOldDouyaSettings.h"
#include "ExMessage.h"
#include "SmartViewFunctions.h"
#include "SilenceDDX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleOldDouyaSettings dialog


CDlgScaleOldDouyaSettings::CDlgScaleOldDouyaSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleOldDouyaSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleOldDouyaSettings)
	m_fInterval = 1.0f;
	m_fMaxToothLen = 2.0f;
	m_fWidth = 0.0f;
	m_fCycle = 3.0f;
	//}}AFX_DATA_INIT

	m_pSym = NULL;
}


void CDlgScaleOldDouyaSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleOldDouyaSettings)
	DDX_Text_Silence(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	DDX_Text_Silence(pDX, IDC_EDIT_MAXTOOTHLEN, m_fMaxToothLen);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Text_Silence(pDX, IDC_EDIT_CYCLE, m_fCycle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleOldDouyaSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleOldDouyaSettings)
	ON_EN_CHANGE(IDC_EDIT_CYCLE, OnChangeEditCycle)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, OnChangeEditInterval)
	ON_EN_CHANGE(IDC_EDIT_MAXTOOTHLEN, OnChangeEditMaxtoothlen)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleOldDouyaSettings message handlers

void CDlgScaleOldDouyaSettings::SetMem(CScaleOldDouya *pSym, ConfigLibItem config)
{
	m_pSym = pSym;
	m_config = config;

	if(m_pSym == NULL)
		return;

	m_fCycle = m_pSym->m_fCycle;
	m_fInterval = m_pSym->m_fInterval;
	m_fMaxToothLen = m_pSym->m_fMaxToothLen;
	m_fWidth = m_pSym->m_fWidth;
}



void CDlgScaleOldDouyaSettings::OnChangeEditCycle() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fCycle = m_fCycle;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
}

void CDlgScaleOldDouyaSettings::OnChangeEditInterval() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fInterval = m_fInterval;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
}

void CDlgScaleOldDouyaSettings::OnChangeEditMaxtoothlen() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fMaxToothLen = m_fMaxToothLen;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
}

void CDlgScaleOldDouyaSettings::OnChangeWidthEdit() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}
