// DlgBatExportMaps.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgBatExportMaps.h"
#include "MapDecorateDlg.h"
#include "editbasedoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBatExportMaps dialog


CDlgBatExportMaps::CDlgBatExportMaps(CWnd* pParent /*=NULL*/)
	: CDlgOpSettings(CDlgBatExportMaps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBatExportMaps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pMapDecorator = NULL;
	m_bNew = FALSE;
}


void CDlgBatExportMaps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBatExportMaps)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Check(pDX, IDC_CHECK_GB_NEW, m_bNew);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBatExportMaps, CDialog)
	//{{AFX_MSG_MAP(CDlgBatExportMaps)
	ON_BN_CLICKED(IDC_MAPDECORATE_SETTINGS,OnMapDecorateSettings)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBatExportMaps message handlers


void CDlgBatExportMaps::OnMapDecorateSettings()
{
	UpdateData(TRUE);
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc) return;

	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if(!pDS) return;
	long scale = pDS->GetScale();

	m_pMapDecorator->m_bNew = m_bNew;
	m_pMapDecorator->InitData(pDS);
	
	m_pMapDecorator->LoadParams(scale);
	m_pMapDecorator->LoadDatas(NULL);
	
	Envelope e = pDS->GetBound();
	m_pMapDecorator->CalcMapNumberAndNearmap(pDS,PT_3D(e.CenterX(),e.CenterY(),0));

	if( scale < 5000 && FALSE == m_bNew )
	{//500-1000-2000¾É°æ
		CMapDecorateDlg_2KO dlg;
		
		dlg.m_bMapDecorateSettings = TRUE;
		dlg.m_pMapDecorator = m_pMapDecorator;

		m_pMapDecorator->SaveParamsMem();
		if( dlg.DoModal()!=IDOK )
		{
			m_pMapDecorator->RestoreParamsMem();
		}
		else
		{
			m_pMapDecorator->SaveDatas(NULL);
		}
	}
	else if( (5000==scale || 10000==scale) && TRUE==m_bNew )	
	{//5000-10000ÐÂ°æ
		CMapDecorateDlg_5KN dlg;
		
		dlg.m_bMapDecorateSettings = TRUE;
		dlg.m_pMapDecorator = m_pMapDecorator;
		
		m_pMapDecorator->SaveParamsMem();
		if( dlg.DoModal()!=IDOK )
		{
			m_pMapDecorator->RestoreParamsMem();
		}
		else
		{
			m_pMapDecorator->SaveDatas(NULL);
		}
	}
	else if( (25000==scale || 50000==scale || 100000 == scale) && FALSE==m_bNew )	
	{//25000-50000¾É°æ
		CMapDecorateDlg_25KO dlg;
		
		dlg.m_bMapDecorateSettings = TRUE;
		dlg.m_pMapDecorator = m_pMapDecorator;
		
		m_pMapDecorator->SaveParamsMem();
		if( dlg.DoModal()!=IDOK )
		{
			m_pMapDecorator->RestoreParamsMem();
		}
		else
		{
			m_pMapDecorator->SaveDatas(NULL);
		}
	}
	else if( (25000==scale || 50000==scale || 100000 == scale) && TRUE==m_bNew )	
	{//25000-50000ÐÂ°æ
		CMapDecorateDlg_25KN dlg;
		
		dlg.m_bMapDecorateSettings = TRUE;
		dlg.m_pMapDecorator = m_pMapDecorator;
		
		m_pMapDecorator->SaveParamsMem();
		if( dlg.DoModal()!=IDOK )
		{
			m_pMapDecorator->RestoreParamsMem();
		}
		else
		{
			m_pMapDecorator->SaveDatas(NULL);
		}
	}
}

