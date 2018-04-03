// DlgScaleXiepo.cpp : implementation file
//

#include "stdafx.h"
#include "DlgScaleXiepo.h"
#include "Editbase.h"
#include "ExMessage.h"
#include "SmartViewFunctions.h"
#include "SilenceDDX.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleXiepo dialog


CDlgScaleXiepo::CDlgScaleXiepo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleXiepo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleXiepo)
	m_bFasten = FALSE;
	m_fInterval = 0.0f;
	m_fPointSize = 0.0f;
	m_fToothLen = 0.0f;
	m_fWidth = 0.0f;
	m_fPointInterval = 0.0f;
	m_bAverageDraw = FALSE;
	m_fBaseYOffset = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgScaleXiepo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleXiepo)
	DDX_Check(pDX, IDC_CHECK_FASTEN, m_bFasten);
	DDX_Check(pDX, IDC_CHECK_AVERAGEDRAW, m_bAverageDraw);
	DDX_Text_Silence(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	DDX_Text_Silence(pDX, IDC_EDIT_POINTSIZE, m_fPointSize);
	DDX_Text_Silence(pDX, IDC_EDIT_POINTINTERVAL, m_fPointInterval);
	DDX_Text_Silence(pDX, IDC_EDIT_TOOTHLEN, m_fToothLen);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Text_Silence(pDX, IDC_BASEYOFFSET_EDIT, m_fBaseYOffset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleXiepo, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleXiepo)
	ON_BN_CLICKED(IDC_CHECK_FASTEN, OnCheckFasten)
	ON_BN_CLICKED(IDC_CHECK_AVERAGEDRAW, OnCheckAverageDraw)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, OnChangeEditInterval)
	ON_EN_CHANGE(IDC_EDIT_POINTSIZE, OnChangeEditPointsize)
	ON_EN_CHANGE(IDC_EDIT_TOOTHLEN, OnChangeEditToothlen)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_EN_CHANGE(IDC_BASEYOFFSET_EDIT, OnChangeBaseYOffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleXiepo message handlers


void CDlgScaleXiepo::SetMem(CScaleXiepo *pSym, ConfigLibItem config)
{
	m_pSym = pSym;
	m_config = config;

	if(m_pSym == NULL)
		return;

	m_fInterval = m_pSym->m_fInterval;
	m_fToothLen = m_pSym->m_fToothLen;
	m_bFasten = m_pSym->m_bFasten;
	m_fPointSize = m_pSym->m_fPointSize;
	m_fPointInterval = m_pSym->m_fPointInterval;
	m_bAverageDraw = m_pSym->m_bAverageDraw;
	m_fBaseYOffset = m_pSym->m_fBaseYOffset;
}


void CDlgScaleXiepo::OnCheckFasten() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_bFasten = m_bFasten;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
}

void CDlgScaleXiepo::OnChangeEditInterval() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fInterval = m_fInterval;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
}

void CDlgScaleXiepo::OnChangeEditPointsize() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fPointSize = m_fPointSize;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
}


void CDlgScaleXiepo::OnChangeEditPointinterval() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fPointInterval = m_fPointInterval;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
}

void CDlgScaleXiepo::OnChangeEditToothlen() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fToothLen = m_fToothLen;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
	
}

void CDlgScaleXiepo::OnChangeWidthEdit() 
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);	
	
}

void CDlgScaleXiepo::OnCheckAverageDraw()
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_bAverageDraw = m_bAverageDraw;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}



void CDlgScaleXiepo::OnChangeBaseYOffset()
{
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fBaseYOffset = m_fBaseYOffset;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}