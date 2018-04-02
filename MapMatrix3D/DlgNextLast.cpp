// DlgNextLast.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgNextLast.h"
#include "EditBaseDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNextLast dialog


CDlgNextLast::CDlgNextLast(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNextLast::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNextLast)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDoc = NULL;
}


void CDlgNextLast::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNextLast)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNextLast, CDialog)
	//{{AFX_MSG_MAP(CDlgNextLast)
	ON_BN_CLICKED(IDC_BUTTON_LAST, OnButtonLast)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNextLast message handlers

void CDlgNextLast::Init(CDlgDoc *pDoc)
{
	m_pDoc = pDoc;
	m_nCurIndex = 0;
}

void CDlgNextLast::OnButtonLast() 
{
	if( !m_pDoc )
		return;
	
	CSelection *pSel = m_pDoc->GetSelection();
	int num = 0;
	const FTR_HANDLE *ftrs = pSel->GetSelectedObjs(num);
	if( num<=0 )
		return;
	
	if( m_nCurIndex>=num )
		m_nCurIndex = 0;
	else if( m_nCurIndex<0 )
		m_nCurIndex = 0;
	
	m_nCurIndex = ((m_nCurIndex+num-1)%num);
	CFeature *pFtr = HandleToFtr(ftrs[m_nCurIndex]);
	if( !pFtr )
		return;
	
	PT_3DEX pt0 = pFtr->GetGeometry()->GetDataPoint(0);
	
	m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
}

void CDlgNextLast::OnButtonNext() 
{
	if( !m_pDoc )
		return;

	CSelection *pSel = m_pDoc->GetSelection();
	int num = 0;
	const FTR_HANDLE *ftrs = pSel->GetSelectedObjs(num);
	if( num<=0 )
		return;

	if( m_nCurIndex>=num )
		m_nCurIndex = 0;
	else if( m_nCurIndex<0 )
		m_nCurIndex = 0;

	m_nCurIndex = ((m_nCurIndex+1)%num);
	CFeature *pFtr = HandleToFtr(ftrs[m_nCurIndex]);
	if( !pFtr )
		return;

	PT_3DEX pt0 = pFtr->GetGeometry()->GetDataPoint(0);

	m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
}


/////////////////////////////////////////////////////////////////////////////
// CDlgLocateFtrs dialog

#include "DlgSelectLayer.h"

CDlgLocateFtrs::CDlgLocateFtrs(CWnd* pParent /*=NULL*/)
: CDialog(CDlgLocateFtrs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNextLast)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_pDoc = NULL;
}


void CDlgLocateFtrs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNextLast)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT1, m_strLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLocateFtrs, CDialog)
//{{AFX_MSG_MAP(CDlgNextLast)
ON_BN_CLICKED(IDC_BUTTON1, OnButtonBrowse)
ON_BN_CLICKED(IDC_BUTTON2, OnButtonLast)
ON_BN_CLICKED(IDC_BUTTON3, OnButtonNext)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLocateFtrs message handlers

void CDlgLocateFtrs::Init(CDlgDoc *pDoc)
{
	m_pDoc = pDoc;
	m_nCurIndex = 0;
}

void CDlgLocateFtrs::OnButtonBrowse() 
{
	if( !m_pDoc ) return;

	CDlgSelectFtrLayer dlg;
	dlg.m_pDS = m_pDoc->GetDlgDataSource();	
	dlg.m_bUsed = TRUE;
	dlg.m_bLocal = TRUE;
	dlg.m_bNotLocal = TRUE;
	dlg.m_strLayer = m_strLayer;
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	m_strLayer = dlg.m_SingleLayer;
	UpdateData(FALSE);

	OnButtonNext();
}

void CDlgLocateFtrs::OnButtonLast() 
{
	if( !m_pDoc )
		return;

	m_arrFtrs.RemoveAll();
	CFtrLayer *pLayer=NULL;
	CDlgDataSource *pData = m_pDoc->GetDlgDataSource();
	pLayer=pData->GetFtrLayer((LPCTSTR)m_strLayer);
	if(pLayer && pLayer->IsVisible())
	{
		int nObj = pLayer->GetObjectCount();
		for(int i=0; i<nObj; i++)
		{
			CFeature* pFtr=pLayer->GetObject(i);
			if( pFtr && pFtr->IsVisible())
			{
				m_arrFtrs.Add(pFtr);
			}
		}
	}
	int num = m_arrFtrs.GetSize();
	if(num==0)  return;
	if( m_nCurIndex>=num )
		m_nCurIndex = 0;
	else if( m_nCurIndex<0 )
		m_nCurIndex = 0;
	
	m_nCurIndex = ((m_nCurIndex+num-1)%num);
	CFeature *pFtr = m_arrFtrs[m_nCurIndex];
	if( !pFtr )
		return;
	
	PT_3DEX pt0 = pFtr->GetGeometry()->GetDataPoint(0);
	
	m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
}

void CDlgLocateFtrs::OnButtonNext() 
{
	if( !m_pDoc )
		return;

	m_arrFtrs.RemoveAll();
	CFtrLayer *pLayer=NULL;
	CDlgDataSource *pData = m_pDoc->GetDlgDataSource();
	pLayer=pData->GetFtrLayer((LPCTSTR)m_strLayer);
	if(pLayer && pLayer->IsVisible())
	{
		int nObj = pLayer->GetObjectCount();
		for(int i=0; i<nObj; i++)
		{
			CFeature* pFtr=pLayer->GetObject(i);
			if( pFtr && pFtr->IsVisible())
			{
				m_arrFtrs.Add(pFtr);
			}
		}
	}
	int num = m_arrFtrs.GetSize();
	if(num==0)  return;
	if( m_nCurIndex>=num )
		m_nCurIndex = 0;
	else if( m_nCurIndex<0 )
		m_nCurIndex = 0;
	
	m_nCurIndex = ((m_nCurIndex+1)%num);
	CFeature *pFtr = m_arrFtrs[m_nCurIndex];
	if( !pFtr )
		return;
	
	PT_3DEX pt0 = pFtr->GetGeometry()->GetDataPoint(0);
	
	m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
}

