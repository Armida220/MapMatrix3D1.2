// DlgDetectResult.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgDetectResult.h"
#include "editbasedoc.h"
#include "ExMessage.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDetectResult dialog


CDlgDetectResult::CDlgDetectResult(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDetectResult::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDetectResult)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDlgDetectResult::Add(CFeature *pFtr0,CFeature *pFtr1)
{
	item t;
	t.pFtr0 = pFtr0;
	t.pt = pFtr0->GetGeometry()->GetDataPoint(0);
	t.pFtr1 = pFtr1;
//	t.pt1 = pFtr1->GetGeometry()->GetDataPoint(0);
	m_arrRetObjs.Add(t);
}

void CDlgDetectResult::ClearData()
{
	m_arrRetObjs.RemoveAll();
	m_arrRetObjs.FreeExtra();
	m_gr.DeleteAll();
	m_nTimerState = 0;
}

void CDlgDetectResult::RefreshData()
{
	m_listResult.ResetContent();
	CString str;
	PT_3DEX tempPt;
	for (int i=0;i<m_arrRetObjs.GetSize();i++)
	{
		tempPt = m_arrRetObjs[i].pt;
		str.Format("Object%6d: %s (%lf, %lf, %lf)",i,StrFromResID(IDS_OVERLAPPOINT_POS),tempPt.x,tempPt.y,tempPt.z);
		int idx = m_listResult.AddString(str);
		m_listResult.SetItemData(idx, (DWORD_PTR)m_arrRetObjs[i].pFtr1);
	}	
}

void CDlgDetectResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDetectResult)
	DDX_Control(pDX, IDC_LIST_RESULTS, m_listResult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDetectResult, CDialog)
	//{{AFX_MSG_MAP(CDlgDetectResult)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_RESULTS, OnSelchangeListResults)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_RESULT_SELOBJ,OnSelObj)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedTipText)
	ON_COMMAND(ID_RESULT_MARKALL, OnResultMarkall)
	ON_COMMAND(ID_RESULT_UNSELALL, OnResultUnselall)
	ON_COMMAND(ID_RESULT_CLEAR, OnResultClear)
	ON_COMMAND(ID_RESULT_SELALL, OnResultSelall)
	ON_COMMAND(ID_RESULT_DELALLOBJ, OnResultDelall)
	ON_COMMAND(ID_RESULT_DELOBJ, OnResultDelobj)
	ON_COMMAND(ID_RESULT_MARKOBJ, OnResultMarkObj)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDetectResult message handlers

extern CDocument *GetCurDocument();

BOOL CDlgDetectResult::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	m_toolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_TOOLTIPS,
		CRect(0,0,0,0));
	m_toolBar.LoadToolBar(IDR_TOOLBAR_DETECT_RESULT);
	m_toolBar.ShowWindow(SW_SHOW);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,0);
	m_pDoc = (CDlgDoc*)GetCurDocument();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgDetectResult::OnNeedTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;
    pTTT->lpszText = MAKEINTRESOURCE(nID);
    pTTT->hinst = AfxGetResourceHandle();
    return(TRUE);	
}

int CDlgDetectResult::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

extern CFeature* MarkErrorPt(CDlgDoc *pDoc, PT_3D pt, double lfMarkWid);

void CDlgDetectResult::OnSelObj()
{	
	// Get the indexes of all the selected items.
	int nCount = m_listResult.GetSelCount();
	CArray<int,int> aryListBoxSel;
	CFeature *pFtr = NULL;
	aryListBoxSel.SetSize(nCount);
	m_listResult.GetSelItems(nCount, aryListBoxSel.GetData());
	for (int i=0;i<aryListBoxSel.GetSize();i++)
	{
		pFtr = (CFeature*)m_listResult.GetItemData(aryListBoxSel[i]);
		m_pDoc->SelectObj(FtrToHandle(pFtr));
	}
}

void CDlgDetectResult::OnResultMarkall() 
{
	//±‹√‚Œƒµµ÷∏’Îm_pDoc ß–ß
	if (m_pDoc==NULL)
	{
		return;
	}
	int num = m_arrRetObjs.GetSize();
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESSING));
	GProgressStart(num);
	for( int i=0; i<num; i++)
	{		
		MarkErrorPt(m_pDoc,m_arrRetObjs[i].pt,1);		
		GProgressStep();	
	}
	GProgressEnd();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESS_END));
	
	m_pDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CDlgDetectResult::OnResultUnselall() 
{
	//±‹√‚Œƒµµ÷∏’Îm_pDoc ß–ß
	if (m_pDoc==NULL)
	{
		return;
	}
	int num = m_arrRetObjs.GetSize();	

	for( int i=0; i<num; i++)
	{		
		m_pDoc->DeselectObj(FtrToHandle(m_arrRetObjs[i].pFtr1));
	}	
	m_pDoc->OnSelectChanged();
}

void CDlgDetectResult::OnResultClear() 
{	
	ClearData();
	RefreshData();
}

void CDlgDetectResult::OnResultSelall()
{
	//±‹√‚Œƒµµ÷∏’Îm_pDoc ß–ß
	if (m_pDoc==NULL)
	{
		return;
	}
	int num = m_arrRetObjs.GetSize();	
	
	for( int i=0; i<num; i++)
	{		
		m_pDoc->SelectObj(FtrToHandle(m_arrRetObjs[i].pFtr1));
	}	
	m_pDoc->OnSelectChanged();
}

void CDlgDetectResult::OnResultDelall()
{
	//±‹√‚Œƒµµ÷∏’Îm_pDoc ß–ß
	if (m_pDoc==NULL)
	{
		return;
	}
	int num = m_arrRetObjs.GetSize();	
	CUndoFtrs undo(m_pDoc,_T("ResultDelall"));
	for( int i=0; i<num; i++)
	{		
		m_pDoc->DeleteObject(FtrToHandle(m_arrRetObjs[i].pFtr1));
		undo.AddOldFeature(FtrToHandle(m_arrRetObjs[i].pFtr1));
	}
	undo.Commit();
	ClearData();
	RefreshData();
	m_pDoc->RefreshView();
}

void CDlgDetectResult::OnResultDelobj()
{
	if (m_pDoc==NULL)
	{
		return;
	}
	int nCount = m_listResult.GetSelCount();
	CArray<int,int> aryListBoxSel;
	CFeature *pFtr = NULL;
	aryListBoxSel.SetSize(nCount);
	m_listResult.GetSelItems(nCount, aryListBoxSel.GetData());
	CUndoFtrs undo(m_pDoc,_T("ResultDelobj"));
	for (int i=aryListBoxSel.GetSize()-1;i>=0;i--)
	{
		pFtr = (CFeature*)m_listResult.GetItemData(aryListBoxSel[i]);
		m_pDoc->DeleteObject(FtrToHandle(pFtr));
		undo.AddOldFeature(FtrToHandle(pFtr));
		m_listResult.DeleteString(aryListBoxSel[i]);
		for (int j=m_arrRetObjs.GetSize()-1;j>=0;j--)
		{
			if (m_arrRetObjs[j].pFtr1==pFtr)
			{
				m_arrRetObjs.RemoveAt(j);
				break;
			}
		}
	}
	undo.Commit();
	m_pDoc->RefreshView();

}

void CDlgDetectResult::OnResultMarkObj()
{
	if (m_pDoc==NULL)
	{
		return;
	}
	int nCount = m_listResult.GetSelCount();
	CArray<int,int> aryListBoxSel;
	CFeature *pFtr = NULL;
	aryListBoxSel.SetSize(nCount);
	m_listResult.GetSelItems(nCount, aryListBoxSel.GetData());
	for (int i=aryListBoxSel.GetSize()-1;i>=0;i--)
	{
		pFtr = (CFeature*)m_listResult.GetItemData(aryListBoxSel[i]);
		for (int j=m_arrRetObjs.GetSize()-1;j>=0;j--)
		{
			if (m_arrRetObjs[j].pFtr1==pFtr)
			{
				MarkErrorPt(m_pDoc,m_arrRetObjs[j].pt,1);
				break;
			}
		}					
	}
	m_pDoc->RefreshView();
}

void CDlgDetectResult::OnSelchangeListResults() 
{
	if (m_pDoc==NULL)
	{
		return;
	}
	KillTimer(1);
	m_nTimerState = 0;
	int nCount = m_listResult.GetSelCount();
	CArray<int,int> aryListBoxSel;
	CFeature *pFtr = NULL;
	aryListBoxSel.SetSize(nCount);
	m_listResult.GetSelItems(nCount, aryListBoxSel.GetData());
// 	m_gr.DeleteAll();
// 	for (int i=aryListBoxSel.GetSize()-1;i>=0;i--)
// 	{
// 		pFtr = (CFeature*)m_listResult.GetItemData(aryListBoxSel[i]);
// 		for (int j=m_arrRetObjs.GetSize()-1;j>=0;j--)
// 		{
// 			if (m_arrRetObjs[j].pFtr1==pFtr)
// 			{			
// 				PT_3D pt0;
// 				COPY_3DPT(pt0,m_arrRetObjs[j].pt);
// 				m_gr.BeginLineString(RGB(255,0,0),1);
// 				pt0.x = m_arrRetObjs[j].pt.x-2;  pt0.y = m_arrRetObjs[j].pt.y-2; 
// 				m_gr.MoveTo(&pt0);
// 				pt0.x = m_arrRetObjs[j].pt.x+2;  pt0.y = m_arrRetObjs[j].pt.y+2; 
// 				m_gr.LineTo(&pt0);
// 				m_gr.End();
// 				m_gr.BeginLineString(RGB(255,0,0),1);
// 				
// 				pt0.x = m_arrRetObjs[j].pt.x-2;  pt0.y = m_arrRetObjs[j].pt.y+2; 
// 				m_gr.MoveTo(&pt0);
// 				pt0.x = m_arrRetObjs[j].pt.x+2;  pt0.y = m_arrRetObjs[j].pt.y-2; 
// 				m_gr.LineTo(&pt0);
// 				m_gr.End();				
// 				break;
// 			}
// 		}
// 		
// 	}
// 	SetTimer(1,500,NULL);
	int idx = aryListBoxSel.GetSize()-1;
	pFtr = (CFeature*)m_listResult.GetItemData(aryListBoxSel[idx]);
	for (int j=m_arrRetObjs.GetSize()-1;j>=0;j--)
	{
		if (m_arrRetObjs[j].pFtr1==pFtr)
		{
			m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&(m_arrRetObjs[j].pt));
		}
	}	
	
}

void CDlgDetectResult::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==1 )
	{
		UpdateDrawing((m_nTimerState&1)==0);
		m_nTimerState++;
		
		if( m_nTimerState>=6 )KillTimer(1);
	}	
	CDialog::OnTimer(nIDEvent);
}

void CDlgDetectResult::OnCancel() 
{
	KillTimer(1);
	UpdateDrawing(FALSE);
	m_nTimerState = 0;
	ShowWindow(SW_HIDE);
}

void CDlgDetectResult::UpdateDrawing(BOOL bDraw)
{
	if( !m_pDoc )return;

	
	if( bDraw )
	{
		m_pDoc->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)&m_gr);
		m_pDoc->UpdateAllViews(NULL,hc_UpdateVariantDrag);
	}
	else
	{
		m_pDoc->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)NULL);
		m_pDoc->UpdateAllViews(NULL,hc_UpdateVariantDrag);
	}
}
