// DlgCelldefView.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgCelldefView.h"
#include "SmartViewFunctions.h"
#include "CollectionViewBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIP_SEARCH		StrFromResID(IDS_SEARCH_NAME)
#define TIMERID_CREATEIMAGE	1
#define TIMERID_PREVIEW		2

//extern void GDI_DrawGrBuffer2d(HDC hdc, const GrBuffer2d *pBuf, BOOL bUseClr, COLORREF clr, double m[9], CSize szDC, CRect rcView, COLORREF backCol);


/////////////////////////////////////////////////////////////////////////////
// CDlgCelldefView dialog


CDlgCelldefView::CDlgCelldefView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCelldefView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCelldefView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pLib = NULL;
}


void CDlgCelldefView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCelldefView)
	DDX_Control(pDX, IDC_LIST_SYMBOL, m_wndListSymbol);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCelldefView, CDialog)
	//{{AFX_MSG_MAP(CDlgCelldefView)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, OnChangeEditSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SYMBOL, OnItemchangedListSymbol)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCelldefView message handlers

void CDlgCelldefView::SetCellDefLib(CCellDefLib *pLib)
{
	m_pLib = pLib;
	FillSymbolList();
}

BOOL CDlgCelldefView::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTimer(TIMERID_CREATEIMAGE,50,NULL);
	SetTimer(TIMERID_PREVIEW,500,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCelldefView::OnItemchangedListSymbol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	OnStaticPreview();
	
	*pResult = 0;
}


void CDlgCelldefView::FillSymbolList()
{
	m_wndListSymbol.DeleteAllItems();
	m_listImages.DeleteImageList();
	m_arrIdxCreateFlag.RemoveAll();

	if( !m_pLib )return;
	
	int i, nSymbol = m_pLib->GetCellDefCount();
	
	CreateImageList(32,32,nSymbol);
	for( i=0; i<nSymbol; i++)
	{
		CellDef def = m_pLib->GetCellDef(i);
		m_wndListSymbol.InsertItem(i,def.m_name,i);
	}
	
	m_wndListSymbol.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

void CDlgCelldefView::OnChangeEditSearch() 
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if( !pWnd )return;
	
	CString strText, strItem, strItem2;
	pWnd->GetWindowText(strText);
	strText.Remove(' ');
	if( strText.IsEmpty() )return;

	strText.MakeUpper();

	int nCount = m_wndListSymbol.GetItemCount();
	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	int nsel = -1;
	if( pos )nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	
	for( int i=nsel+1; i<nsel+1+nCount; i++)
	{
		strItem = m_wndListSymbol.GetItemText(i%nCount,0);	
		strItem2 = strItem;
		strItem2.MakeUpper();
		if( strItem2.Find(strText,0)>=0 )
		{
			break;
		}
	}
	if( i<nCount )
	{
		m_wndListSymbol.EnsureVisible(i,FALSE);
		m_wndListSymbol.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	}
	
}

void CDlgCelldefView::OnSetfocusEditSearch() 
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if( !pWnd )return;
	
	CString strText;
	pWnd->GetWindowText(strText);
	if( strText==TIP_SEARCH )
	{
		pWnd->SetWindowText("");
	}
}

void CDlgCelldefView::OnKillfocusEditSearch() 
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if( !pWnd )return;
	
	CString strText;
	pWnd->GetWindowText(strText);
	if( strText.IsEmpty() )
	{
		pWnd->SetWindowText(TIP_SEARCH);
	}
}

void CDlgCelldefView::CreateImageList(int cx, int cy, int nsize)
{
	if( m_listImages.m_hImageList )
		return;
		
	m_listImages.Create(cx,cy,ILC_COLOR24,0,nsize+1);
	m_listImages.SetImageCount(nsize+1);
	m_wndListSymbol.SetImageList(&m_listImages,TVSIL_NORMAL);
	m_arrIdxCreateFlag.SetSize(nsize+1);
	
	GrBuffer2d buf;
	for( int i=0; i<nsize+1; i++)DrawImageItem(i,cx,cy,&buf);
	m_arrIdxCreateFlag[0] = 1;
	m_nImageWid = cx;  m_nImageHei = cy;
	m_nIdxToCreateImage = 1;
	
	SetTimer(TIMERID_CREATEIMAGE,50,NULL);
	SetTimer(TIMERID_PREVIEW,500,NULL);
}

void CDlgCelldefView::DrawSymbol(CellDef *pDef, GrBuffer2d *buf)
{	
	if( pDef->m_pgr )
	{
		buf->AddBuffer(pDef->m_pgr);
		buf->GetEnvelope();
		buf->SetAllColor(0);
	}
}

BOOL CDlgCelldefView::CreateImageItem(int idx)
{
	if( !m_pLib )return FALSE;

	CString strName = m_wndListSymbol.GetItemText(idx,0);
	CellDef def = m_pLib->GetCellDef(strName);

	GrBuffer2d buf;
	DrawSymbol(&def, &buf);
	DrawImageItem(idx,m_nImageWid,m_nImageHei,&buf);
	
	return TRUE;
}


void CDlgCelldefView::DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf)
{
	//创建内存设备
	CClientDC cdc(&m_wndListSymbol);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
	if( !hDC )return;
	
	HBITMAP hBmp = ::CreateCompatibleBitmap(cdc.m_hDC,cx,cy);
	if( !hBmp )
	{
		::DeleteDC(hDC);
		return;
	}
	
	CBitmap bm;
	bm.Attach(hBmp);
	
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
	
	CBrush br(RGB(255,255,255));
	::FillRect(hDC,CRect(0,0,cx,cy),(HBRUSH)br);
	
	CPen pen;
	pen.CreatePen(PS_SOLID,0,(COLORREF)0);
	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);
	
	//计算变换系数
	CRect rect(2,2,cx-4,cy-4);
	Envelope e = pBuf->GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);
	float xoff=0, yoff=0;
	if( scalex>scaley )
	{
		xoff = cx/2-((e.m_xh-e.m_xl)/2*scaley+rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy/2-((e.m_yh-e.m_yl)/2*scalex+rect.top);
	}

	double matrix[9] = {
		scalex,0,-e.m_xl*scalex+rect.left+xoff+0.5,
			0,-scalex,cy+e.m_yl*scalex-rect.top-yoff-0.5,
			0,0,1
	};

	GDI_DrawGrBuffer2d(hDC,pBuf,TRUE,0,matrix,CSize(cx,cy),CRect(0,0,cx,cy));
	
	::SelectObject(hDC,hOldBmp);
	
	m_listImages.Replace(idx,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
/*	
	//绘图
	COLORREF clr = 0;
	int wid = -1;
	const Graph2d* pGraph2d = pBuf->HeadGraph();
	while( pGraph2d ) 
	{
		switch(pGraph2d->type)
		{
		case 1:
			{
				GrPoint2d *pPoint = (GrPoint2d*)pGraph2d;
				if(pPoint == NULL)
					continue;
				
				break;
			}
		case 2:
			{
				GrLineString2d *pLineString = (GrLineString2d*)pGraph2d;
				if(pLineString == NULL)
					continue;
				GrVertex2d *pts = pLineString->ptlist.pts;
				for( int i=0; i<pLineString->ptlist.nuse; i++)
				{
					int x =(int)((pts[i].x-e.m_xl)*scalex+rect.left+xoff+0.5);
					int y =cy-(int)((pts[i].y-e.m_yl)*scalex+rect.top+yoff+0.5);
					
					//切换钢笔
					if( clr!=pLineString->color || wid!=pLineString->width )
					{
						::SelectObject(hDC,(HPEN)hOldPen);
						clr = pLineString->color;
						wid = pLineString->width;
						pen.DeleteObject();
						
						if( pLineString->width<=1 )
							pen.CreatePen(PS_SOLID,wid,clr);
						else
						{
							LOGBRUSH lbrush = {BS_SOLID,clr,0};
							pen.CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_FLAT,wid,&lbrush,0,NULL);
						}
						::SelectObject(hDC,(HPEN)pen);
					}
					
					//绘制直线
					if(pts[i].code==GRBUFFER_PTCODE_MOVETO )
						::MoveToEx(hDC, x, y, NULL);
					else
						::LineTo(hDC, x, y);
				}
								
				break;
			}
		case 3:
			{
				
				GrPolygon2d *pPolygon = (GrPolygon2d*)pGraph2d;
				if(pPolygon == NULL)
					continue;
				break;
			}
		case 4:
			{
				GrText2d *pText = (GrText2d*)pGraph2d;
				if(pText == NULL)
					continue;
				break;
			}
		default:
			pGraph2d = NULL;
			continue;
		}
		
		pGraph2d = pGraph2d->next;
	}
	
	::SelectObject(hDC,hOldPen);
	::SelectObject(hDC,hOldBmp);
	pen.DeleteObject();
	
	m_listImages.Replace(idx,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
*/
}

void CDlgCelldefView::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==TIMERID_CREATEIMAGE )
	{
		BOOL bKill = FALSE;
		if( !m_pLib )bKill = TRUE;
		else
		{
			int nsize = m_arrIdxCreateFlag.GetSize();
			
			//检查是否已经没有需要生成的图像了
			for( int i=m_nIdxToCreateImage; i<nsize+m_nIdxToCreateImage; i++)
			{
				if( m_arrIdxCreateFlag[(i%nsize)]==0 )break;
			}
			
			if( i>=nsize+m_nIdxToCreateImage )bKill = TRUE;
			else
			{
				i = (i%nsize);
				if( CreateImageItem(i) )
				{
					m_arrIdxCreateFlag[i] = 1;
					m_nIdxToCreateImage = i+1;
					
//					static int nCreateTimes = 0;
//					if( (nCreateTimes+1)%10==0 )
//						m_wndListSymbol.RedrawWindow();
//					nCreateTimes++;

					CRect rect;
					m_wndListSymbol.GetItemRect(i,&rect,LVIR_BOUNDS);
					m_wndListSymbol.RedrawWindow(&rect);
				}
			}
		}
		
	}
	else
	{
		OnStaticPreview();
	}
	
	CDialog::OnTimer(nIDEvent);
}


void CDlgCelldefView::DrawPreview(LPCTSTR strSymName)
{
	if( !m_pLib )return;
	
	CellDef def = m_pLib->GetCellDef(strSymName);

	CWnd *pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
	if( !pWnd )return;
	CRect rcView;
	pWnd->GetClientRect(&rcView);

	GrBuffer2d buf, *pBuf;
	pBuf = &buf;
	DrawSymbol(&def,&buf);

	//创建内存设备
	CClientDC cdc(pWnd);

	int cx = rcView.Width(), cy = rcView.Height();

/*	
	CBitmap bm;
	bm.Attach(hBmp);
	
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
*/	
	CBrush br(RGB(255,255,255));
	::FillRect(cdc.GetSafeHdc(),CRect(0,0,cx,cy),(HBRUSH)br);

	CPen pen;
	pen.CreatePen(PS_SOLID,0,(COLORREF)0);
//	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);
	
	//计算变换系数
	CRect rect(2,2,cx-4,cy-4);
	Envelope e = pBuf->GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);
	float xoff=0, yoff=0;
	if( scalex>scaley )
	{
		xoff = cx/2-((e.m_xh-e.m_xl)/2*scaley+rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy/2-((e.m_yh-e.m_yl)/2*scalex+rect.top);
	}

	double matrix[9] = {
		scalex,0,-e.m_xl*scalex+rect.left+xoff+0.5,
			0,-scalex,cy+e.m_yl*scalex-rect.top-yoff-0.5,
			0,0,1
	};

	GDI_DrawGrBuffer2d(cdc.GetSafeHdc(),pBuf,TRUE,0,matrix,rcView.Size(),rcView);

	CWnd *pWndSize = GetDlgItem(IDC_STATIC_SIZE);
	if( pWndSize )
	{
		CString strSize;
		strSize.Format("%.3fm X %.3fm",(e.m_xh-e.m_xl),(e.m_yh-e.m_yl));
		pWndSize->SetWindowText(strSize);
	}
}

void CDlgCelldefView::OnStaticPreview() 
{
	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos )return;
	
	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	CString name = m_wndListSymbol.GetItemText(nsel,0);
	
	DrawPreview(name);
	
	if( nsel<m_arrIdxCreateFlag.GetSize() )
	{
		CreateImageItem(nsel);
		m_wndListSymbol.RedrawItems(nsel,nsel+1);
	}
}
