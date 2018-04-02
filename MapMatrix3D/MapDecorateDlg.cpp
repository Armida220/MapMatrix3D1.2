// MapDecorateDlg.cpp: implementation of the CMapDecorateDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Editbase.h"
#include "MapDecorateDlg.h"
#include "..\CORE\viewer\EditBase\res\resource.h"
#include "Functions_temp.h"
#include "EditbaseDoc.h"

using namespace MapDecorate;

/////////////////////////////////////////////////////////////////////////////
// CStaticEx
IMPLEMENT_DYNAMIC(CStaticEx, CStatic)

CStaticEx::CStaticEx()
{
	m_clrHotText=RGB(255,255,0);  
	m_clrBorder=m_clrText=RGB(255,0,0);
	m_clrBack = ::GetSysColor(COLOR_3DFACE);    
	m_brBkgnd.CreateSolidBrush(m_clrBack);    
//	m_bHotTrack = FALSE;    
	m_bHover = FALSE;
	clicked=FALSE;
}

CStaticEx::~CStaticEx()
{
}


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	//{{AFX_MSG_MAP(CStaticEx)
	ON_WM_PAINT()
		ON_WM_MOUSEMOVE()  
		ON_WM_MOUSELEAVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticEx message handlers



void CStaticEx::SetTextColor(COLORREF clrText)
{     
	m_clrText = clrText;    
    this->Invalidate(); 
}

void CStaticEx::SetHotTextColor(COLORREF clrHotText)
{     
	m_clrHotText = clrHotText;  
	this->Invalidate(); 
}

void CStaticEx::SetBgColor(COLORREF clrBack)
{     
	m_clrBack = clrBack;    
	this->Invalidate(); 
}

void CStaticEx::SetBorderColor(COLORREF clrBorder)
{
	m_clrBorder=clrBorder;
	this->Invalidate();	

}

void CStaticEx::PreSubclassWindow()
{     
	DWORD dwStyle = this->GetStyle();    
	::SetWindowLong(this->GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);    
	CStatic::PreSubclassWindow(); 
}

void CStaticEx::OnMouseMove(UINT nFlags, CPoint point)
{     
	TRACKMOUSEEVENT tme;    
	tme.cbSize = sizeof(tme);    
	tme.dwFlags = TME_HOVER | TME_LEAVE;    
	tme.hwndTrack = m_hWnd;    
	tme.dwHoverTime = HOVER_DEFAULT;    
	_TrackMouseEvent(&tme);    
	CRect rect;        
	GetClientRect(rect);        
	if (rect.PtInRect(point))       
	{
		m_bHover=TRUE;            
		m_clrBorder=m_clrText=m_clrHotText;
		this->Invalidate(); 
	}    
	   
	CStatic::OnMouseMove(nFlags, point); 
}

void CStaticEx::OnMouseLeave()
{     
	m_bHover=FALSE; 
	
	this->Invalidate();    
	::ReleaseCapture();
}


void CStaticEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(clicked)
		m_clrText=m_clrBorder=RGB(169,169,169);
	else
		m_clrText=m_clrBorder=RGB(255,0,0);

	if( m_bHover )
		m_clrText=m_clrBorder=RGB(255,255,0);
	
	// TODO: Add your message handler code here
	dc.SetBkMode(TRANSPARENT);
	CFont *pOldFont = dc.SelectObject(GetFont());

	CPen pen(PS_SOLID,1,m_clrBorder);
	CPen *pOldPen = dc.SelectObject(&pen);
	CBrush brush(m_clrBack);
	CBrush *pOldBrush = dc.SelectObject(&brush);
	
	//draw border
	CRect rect;
	this->GetClientRect(&rect);
	dc.RoundRect(&rect,CPoint(0,0));
	//draw text
	CString str;
	this->GetWindowText(str);
	dc.SetTextColor(m_clrText);

	if( rect.Width()>rect.Height() && str.GetLength()<14 )
		dc.DrawText(str,&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_WORDBREAK);
	else
		dc.DrawText(str,&rect,DT_CENTER|DT_WORDBREAK);

	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	// Do not call CStatic::OnPaint() for painting messages
}



/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg dialog

CMapDecorateDlg::CMapDecorateDlg(UINT nDlgID, CWnd* pParent /*=NULL*/)
	: CDialog(nDlgID, pParent)
{	
	//{{AFX_DATA_INIT(CMapDecorateDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nCurItemSel = -1;

	m_bClickList = FALSE;
	m_pCmd = NULL;

	bMagNor=bGrid=bInnerBound=bOutBound=FALSE;

	m_pMapDecorator = NULL;
	m_bMapDecorateSettings = FALSE;
}

void CMapDecorateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapDecorateDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMapDecorateDlg, CDialog)
	//{{AFX_MSG_MAP(CMapDecorateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg message handlers

CString MyGetWindowText(CStaticEx &item)
{
	CString strText;
	item.GetWindowText(strText);
	return strText;
}


BOOL CALLBACK MoveChildWindows(HWND hwnd, LPARAM lParam)
{
	CRect rcWnd;
	CPoint dpt = *(CPoint*)lParam;
	GetWindowRect(hwnd,&rcWnd);

	CWnd::FromHandle(GetParent(hwnd))->ScreenToClient(&rcWnd);

	rcWnd.OffsetRect(dpt);

	MoveWindow(hwnd,rcWnd.left,rcWnd.top,rcWnd.Width(),rcWnd.Height(),FALSE);

	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMapDecorateDlg::OnPaint() 
{
 //	UpdateData(FALSE);
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rect;
		
		m_staticNearMap.GetWindowRect(&rect);
		ScreenToClient(rect);
		int i,j;

		CPen pen;

		COLORREF color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("Grid"))==0 )
			color = RGB(169,169,169);
		if( bGrid )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,1,color);
		CPen *pOldPen = dc.SelectObject(&pen);

		// draw grid
		for(i=rect_outbound.top;i<rect_outbound.bottom;i+=30)
		{
			dc.MoveTo(CPoint(rect_outbound.left,i));
			dc.LineTo(CPoint(rect_outbound.right,i));
		}
		for(j=rect_outbound.left;j<rect_outbound.right;j+=30)
		{
			dc.MoveTo(CPoint(j,rect_outbound.top));
			dc.LineTo(j,rect_outbound.bottom);
		}
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw out rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("OuterBound"))==0 )
			color = RGB(169,169,169);
		if( bOutBound )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,3,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_outbound.left,rect_outbound.top));
		dc.LineTo(CPoint(rect_outbound.left,rect_outbound.bottom));
		dc.LineTo(CPoint(rect_outbound.right,rect_outbound.bottom));
		dc.LineTo(CPoint(rect_outbound.right,rect_outbound.top));
		dc.LineTo(CPoint(rect_outbound.left,rect_outbound.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw inner rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("InnerBound"))==0 )
			color = RGB(169,169,169);
		if( bInnerBound )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,2,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_innerbound.left,rect_innerbound.top));
		dc.LineTo(CPoint(rect_innerbound.left,rect_innerbound.bottom));
		dc.LineTo(CPoint(rect_innerbound.right,rect_innerbound.bottom));
		dc.LineTo(CPoint(rect_innerbound.right,rect_innerbound.top));
		dc.LineTo(CPoint(rect_innerbound.left,rect_innerbound.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw the magline line
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("MagLine"))==0 )
			color = RGB(169,169,169);
		if( bMagNor )
			color = RGB(255,255,0);

		pen.CreatePen(PS_DOT,1,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_magnor.CenterPoint().x,rect_outbound.top));
		dc.LineTo(CPoint(rect_magnor.CenterPoint().x,rect_outbound.bottom));
		dc.SelectObject(pOldPen);

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMapDecorateDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CMapDecorateDlg::OnDblClickMapList()
{
	OnSelchangeMapList();
}

void CMapDecorateDlg::OnSelchangeMapList() 
{
	// TODO: Add your control notification handler code here
	int index=m_ctlCheckList.GetCurSel();
	int check=m_ctlCheckList.GetCheck(index);

	SaveCurrent();

	m_bClickList = TRUE;

	ClickStatic(index);

	CString id = m_arrObjIDStrings[index];
	if( id.CompareNoCase("InnerBound")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("OuterBound")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("Grid")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("Magline")==0 )
	{
		InvalidateRect(&rect_outbound);
	}
	
	m_nCurItemSel = index;

	m_bClickList = FALSE;
	  
}

int CMapDecorateDlg::GetObjIndex(LPCTSTR id)
{
	for( int i=0; i<m_arrObjIDStrings.GetSize(); i++)
	{
		if( m_arrObjIDStrings[i].CompareNoCase(id)==0 )
			return i;
	}

	return -1;
}


CUIParam *CMapDecorateDlg::CreateNewUIParam(int index)
{
	CUIParam *pParam = new CUIParam();
	if( !pParam )return NULL;

	m_pMapDecorator->GetObj(m_arrObjIDStrings[index])->GetUIParams_data(pParam);

	return pParam;
}

void CMapDecorateDlg::SaveCurrent() 
{
	if( m_nCurItemSel<0 )
		return;
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	int index=m_nCurItemSel;
	int check = m_ctlCheckList.GetCheck(index);

	CValueTable tab;
	tab.BeginAddValueItem();
	m_wndPropList.GetCmdParams(tab);
	tab.EndAddValueItem();

	m_pMapDecorator->GetObj(m_arrObjIDStrings[index])->SetData(tab);
	m_pMapDecorator->GetObj(m_arrObjIDStrings[index])->m_bUsed = (check!=0);	
}

void CMapDecorateDlg::OnOK()
{
	m_nCurItemSel = m_ctlCheckList.GetCurSel();
	SaveCurrent();

	m_pMapDecorator->m_nMapType = m_nMapType;
	m_pMapDecorator->m_nMapBorderType = m_nMapBorderType;
	m_pMapDecorator->m_fMapWid = m_fMapWid;
	m_pMapDecorator->m_fMapHei = m_fMapHei;

	CName *pName = (CName*)m_pMapDecorator->GetObj("Name");
	strncpy(pName->name, m_strMapName, sizeof(pName->name) - 1);

	CNumber *pNumber = (CNumber*)m_pMapDecorator->GetObj("Number");
	strncpy(pNumber->number, m_strMapNum, sizeof(pNumber->number) - 1);

	m_pMapDecorator->m_bCutStripNum = m_bCutStripNO;

	CValueTable tab;
	tab.BeginAddValueItem();
	tab.AddValue("Command", &CVariantEx((_variant_t)"OK"));
	tab.EndAddValueItem();
	if (m_pCmd)m_pCmd->SetParams(tab);

	//参数存入xml
	m_pMapDecorator->SaveDatas(NULL);

	CDialog::OnOK();
}

void CMapDecorateDlg::OnCancel()
{
	CValueTable tab;
	tab.BeginAddValueItem();
	tab.AddValue("Command",&CVariantEx((_variant_t)"Cancel"));	
	tab.EndAddValueItem();
	if( m_pCmd )m_pCmd->SetParams(tab);

	CDialog::OnCancel();
}



void CMapDecorateDlg::OnChangeNumEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CMapDecorateDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT tme;    
	tme.cbSize = sizeof(tme);    
	tme.dwFlags = TME_HOVER | TME_LEAVE;    
	tme.hwndTrack = m_hWnd;    
	tme.dwHoverTime = HOVER_DEFAULT;    
	_TrackMouseEvent(&tme);

	BOOL bGrid0=bGrid;
	BOOL bOutBound0=bOutBound;
	BOOL bInnerBound0=bInnerBound;
	BOOL bMagNor0=bMagNor;

	bMagNor=bGrid=bInnerBound=bOutBound=FALSE;

	//point in the bouter_bound area_desc
	if(rect_outbound.PtInRect(point))
	{
		//point in the binner_bound area_desc
		if(rect_innerbound.PtInRect(point))
		{
			//point in the bgrid area_desc
			if(rect_grid.PtInRect(point))
			{
				if(rect_magnor.PtInRect(point))
				{
					bMagNor = TRUE;					
				}
				else
				{
					bGrid = TRUE;
				}
			}
			else
			{
				bInnerBound=TRUE;
			}
				
		}
		else 
		{
			bOutBound=TRUE;			
		}
	}

	if(bGrid0==bGrid && bOutBound0==bOutBound && bInnerBound0==bInnerBound && bMagNor0==bMagNor )
		return;
	
	CRect rect2 = rect_outbound;
	rect2.InflateRect(CSize(2,2));

	InvalidateRect(&rect2);

	CDialog::OnMouseMove(nFlags, point);
}

void CMapDecorateDlg::OnMouseLeave()
{
	CRect trect;
	m_staticArea.GetWindowRect(&trect);
	ScreenToClient(&trect);
	CPoint point;
	::GetCursorPos(&point);
	this->ScreenToClient(&point);

	//bMagNor=bGrid=bInnerBound=bOutBound=FALSE;
}

void CMapDecorateDlg::OnStaticButton()
{
	const MSG *pMsg = GetCurrentMessage();
	int id = pMsg->wParam;
	for( int i=0; i<m_arrCtrlIDs.GetSize(); i++)
	{
		if( id==m_arrCtrlIDs[i] )
		{
			SaveCurrent();
			ClickStatic(i);
			return;
		}
	}
}

void CMapDecorateDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(rect_outbound.PtInRect(point))
	{
		if(rect_innerbound.PtInRect(point))
		{
			if(rect_grid.PtInRect(point))
			{
				if(rect_magnor.PtInRect(point))
				{
					m_ctlCheckList.SetCurSel(GetObjIndex("magLine"));
				}
				else
				{
					m_ctlCheckList.SetCurSel(GetObjIndex("Grid"));
				}
			}
			else
			{
				m_ctlCheckList.SetCurSel(GetObjIndex("InnerBound"));
			}
		}
		else 
		{
			m_ctlCheckList.SetCurSel(GetObjIndex("OuterBound"));
		}

		CRect rect = rect_outbound;
		rect.InflateRect(2,2);
		
		InvalidateRect(rect);
		
		UpdatePropList();
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}


void CMapDecorateDlg::UpdatePropList()
{
	int i = m_ctlCheckList.GetCurSel();
	if( i<0 )
		return;

	CUIParam *pNewParam = CreateNewUIParam(i);
	if( !pNewParam )
		return;
	
	m_wndPropList.LoadParams(pNewParam);
	m_wndPropList.ShowParams(pNewParam);
	
	if( m_ctlCheckList.GetCheck(i) )
	{
		m_wndPropList.EnableWindow(TRUE);
	}
	else
	{
		m_wndPropList.EnableWindow(FALSE);
	}

	delete pNewParam;
}

void CMapDecorateDlg::ClickStatic(int i)
{
	if( i<0 )
		return;

	{
		BOOL bModifyCheck = FALSE;
		if( m_bClickList )
		{
			if( i==m_nCurItemSel )
			{
				bModifyCheck = TRUE;
			}
		}
		else
		{
		}

		if( bModifyCheck )
		{
			int check=m_ctlCheckList.GetCheck(i);
			
			m_ctlCheckList.SetCheck(i,1-check);
			
			m_arrPCtrls[i]->clicked = check;
			m_arrPCtrls[i]->Invalidate();
		}
	}

	m_ctlCheckList.SetCurSel(i);
	m_nCurItemSel = i;

	UpdatePropList();
}


void CMapDecorateDlg::OnSetParam()
{
	SaveCurrent();
	
	//记住图号的整数小数位参数，后面检查它们是否发生了变化
	CNumber *pNumber = (CNumber*)m_pMapDecorator->GetObj("Number");
	int int_num = pNumber->int_num;
	int float_num = pNumber->float_num;

	CMapDecorateParamDlg dlg(AfxGetMainWnd());
	dlg.m_pMapDecorator = m_pMapDecorator;

	CString curSelName;
	m_ctlCheckList.GetText(m_nCurItemSel, curSelName);
	for (int i = 0; i < m_pMapDecorator->m_arrPObjs.GetSize(); i++)
	{
		if (curSelName == m_pMapDecorator->m_arrPObjs[i]->Name())
		{
			dlg.m_nCurItemSel = i;
			break;
		}
	}

	m_pMapDecorator->SaveParamsMem();

	if( dlg.DoModal()!=IDOK )
	{
		m_pMapDecorator->RestoreParamsMem();
		return;
	}

	if( int_num!=pNumber->int_num || float_num!=pNumber->float_num )
	{
		if( m_pMapDecorator->m_pDS )
			m_pMapDecorator->InitData(m_pMapDecorator->m_pDS);
		else if( m_pMapDecorator->m_pDoc )
			m_pMapDecorator->InitData(m_pMapDecorator->m_pDoc->GetDlgDataSource());
		else
			m_pMapDecorator->InitData(GetActiveDlgDoc()->GetDlgDataSource());
	}

	m_strMapNum = pNumber->number;
	UpdateData(FALSE);
}



void CMapDecorateDlg::OnSelectMapBorder()
{
	CWnd *pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
	if( pWnd )
	{
		CString text;
		pWnd->GetWindowText(text);
		if( text.Find("OK")>=0 )
		{
			text = text.Left(text.GetLength()-3);
			pWnd->SetWindowText(text);
		}
	}

	UpdateData(FALSE);

	CValueTable tab;
	tab.BeginAddValueItem();
	if( m_nMapBorderType==0 )
		tab.AddValue("Command",&CVariantEx((_variant_t)"SelectBound"));
	else if( m_nMapBorderType==1 )
		tab.AddValue("Command",&CVariantEx((_variant_t)"SelectCorner"));

	tab.EndAddValueItem();
	if( m_pCmd )m_pCmd->SetParams(tab);
}


void CMapDecorateDlg::OnSetCoordSys()
{
	m_pMapDecorator->m_tm.ShowDatum();
}


void CMapDecorateDlg::OnSelchangeMaptype()
{
	UpdateData(TRUE);
	if( m_nMapType==TK_MAPTYPE_RECT5050 )
	{
		m_fMapWid = 50;
		m_fMapHei = 50;
	}
	else if( m_nMapType==TK_MAPTYPE_RECT5040 )
	{
		m_fMapWid = 50;
		m_fMapHei = 40;
	}
	else if( m_nMapType==TK_MAPTYPE_RECT4040 )
	{
		m_fMapWid = 40;
		m_fMapHei = 40;
	}
	
	BOOL bEnable = TRUE;
	if( m_nMapType==TK_MAPTYPE_STD )
	{
		bEnable = FALSE;
	}
	
	CWnd *pWnd = GetDlgItem(IDC_EDIT_WID);
	if( pWnd )
	{
		pWnd->EnableWindow(bEnable);
	}
	pWnd = GetDlgItem(IDC_EDIT_HEI);
	if( pWnd )
	{
		pWnd->EnableWindow(bEnable);
	}

	UpdateData(FALSE);
}


void CMapDecorateDlg::OnSelchangeMapBorder()
{
	UpdateData(TRUE);

	m_pMapDecorator->m_nMapBorderType = m_nMapBorderType;

	BOOL bEnable = TRUE;
	if( m_nMapBorderType==TK_MAPBORDER_MAPNUM )
	{
		bEnable = FALSE;
	}

	CWnd *pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
	if( pWnd )
	{
		pWnd->EnableWindow(bEnable);
	}

	pWnd = GetDlgItem(IDC_SET_COORDSYS);
	if( pWnd )
	{
		pWnd->EnableWindow(!bEnable);
	}

	UpdateData(FALSE);
}


void CMapDecorateDlg::SetBound(PT_3D pts[4])
{
	memcpy(m_pMapDecorator->m_SelectedBoundPts,pts,sizeof(PT_3D)*4);

	CWnd *pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
	if( pWnd )
	{
		CString text;
		pWnd->GetWindowText(text);
		if( text.Find("OK")<0 )
		{
			text += ":OK";
			pWnd->SetWindowText(text);
		}
	}
}


void CMapDecorateDlg::SetBoundCorner(PT_3D pt)
{
	m_pMapDecorator->m_SelectBoundCorner = pt;
	
	CWnd *pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
	if( pWnd )
	{
		CString text;
		pWnd->GetWindowText(text);
		if( text.Find("OK")>=0 )
		{
			text += ":OK";
			pWnd->SetWindowText(text);
		}
	}
}


void CMapDecorateDlg::OnSelectAll()
{
	for(int i=0;i<m_ctlCheckList.GetCount(); i++)
		m_ctlCheckList.SetCheck( i, 1);

	for(i=0;i<m_arrPCtrls.GetSize(); i++)
		m_arrPCtrls[i]->clicked = FALSE;

	for(i=0;i<m_pMapDecorator->m_arrPObjs.GetSize(); i++)
		m_pMapDecorator->m_arrPObjs[i]->m_bUsed = TRUE;	

	Invalidate(FALSE);
}


void CMapDecorateDlg::OnSelectNone()
{
	for(int i=0;i<m_ctlCheckList.GetCount(); i++)
		m_ctlCheckList.SetCheck( i, 0);
	
	for(i=0;i<m_arrPCtrls.GetSize(); i++)
		m_arrPCtrls[i]->clicked = TRUE;
	
	for(i=0;i<m_pMapDecorator->m_arrPObjs.GetSize(); i++)
		m_pMapDecorator->m_arrPObjs[i]->m_bUsed = FALSE;	

	Invalidate(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CMapDecorateParamDlg dialog

CMapDecorateParamDlg::CMapDecorateParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapDecorateParamDlg::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(CMapDecorateParamDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nCurItemSel = 0;
	m_pMapDecorator = NULL;
}

void CMapDecorateParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapDecorateParamDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MAP_LIST, m_ctlCheckList);
}

BEGIN_MESSAGE_MAP(CMapDecorateParamDlg, CDialog)
	//{{AFX_MSG_MAP(CMapDecorateParamDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_MAP_LIST, OnSelchangeMapList)
	ON_BN_CLICKED(IDC_SAVE_DEFAULT, OnSaveDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDecorateParamDlg message handlers


BOOL CMapDecorateParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	m_ctlCheckList.ResetContent();

	for(int i=0;i<m_pMapDecorator->m_arrPObjs.GetSize(); i++)
	{
		m_ctlCheckList.AddString(m_pMapDecorator->m_arrPObjs[i]->Name());
	}

	CWnd *pWnd = GetDlgItem(IDC_STATIC_PARAM);
	if( pWnd )
	{
		CRect rcPos;
		pWnd->GetWindowRect(rcPos);

		ScreenToClient(&rcPos);

		if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rcPos, this, 2))
		{
		}

		m_wndPropList.SetWindowPos(NULL,rcPos.left,rcPos.top,
			rcPos.Width(),rcPos.Height(),0);

		m_wndPropList.EnableHeaderCtrl(TRUE,StrFromResID(IDS_PARAM_NAME),StrFromResID(IDS_VALUE));
	}

	m_ctlCheckList.SetCurSel(m_nCurItemSel);
	
	UpdatePropList();

	
	CString title0;
	CString path = m_pMapDecorator->m_strSavePath;
	
	int pos;
	CString path1 = path;
	for( i=0; i<3; i++)
	{
		pos = path1.ReverseFind('\\');
		if( pos>0 )
		{
			path1 = path1.Left(pos);
		}
		else
		{
			break;
		}
	}
	
	if( i>=3 )
	{
		path = path.Mid(pos+1);
	}
	
	GetWindowText(title0);
	
	title0 = title0 + "(" + path + ")";
	SetWindowText(title0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMapDecorateParamDlg::OnPaint() 
{
 //	UpdateData(FALSE);
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rect;
	
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMapDecorateParamDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CMapDecorateParamDlg::OnSelchangeMapList() 
{
	// TODO: Add your control notification handler code here
	int index=m_ctlCheckList.GetCurSel();

	SaveCurrent();	
	
	UpdatePropList();
	
	m_nCurItemSel = index;
	  
}



CUIParam *CMapDecorateParamDlg::CreateNewUIParam(int index)
{
	if( index<0 )
		return NULL;

	CUIParam *pParam = new CUIParam();
	if( !pParam )return NULL;

	m_pMapDecorator->m_arrPObjs[index]->GetUIParams(pParam);

	return pParam;
}

void CMapDecorateParamDlg::SaveCurrent() 
{
	if( m_nCurItemSel<0 )
		return;
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int index=m_nCurItemSel;

	CValueTable tab;
	tab.BeginAddValueItem();
	m_wndPropList.GetCmdParams(tab);
	tab.EndAddValueItem();

	m_pMapDecorator->m_arrPObjs[index]->SetParams(tab);	
}

void CMapDecorateParamDlg::OnOK()
{
	m_nCurItemSel = m_ctlCheckList.GetCurSel();
	SaveCurrent();
	CDialog::OnOK();
}




void CMapDecorateParamDlg::UpdatePropList()
{
	int i = m_ctlCheckList.GetCurSel();
	if( i<0 )
		return;

	CUIParam *pNewParam = CreateNewUIParam(i);
	if( !pNewParam )
		return;
	
	m_wndPropList.LoadParams(pNewParam);
	m_wndPropList.ShowParams(pNewParam);

	delete pNewParam;
}


void CMapDecorateParamDlg::OnSaveDefault()
{
	SaveCurrent();
	m_pMapDecorator->SaveParams(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_5KN dialog

CMapDecorateDlg_5KN::CMapDecorateDlg_5KN(CWnd* pParent /*=NULL*/)
	: CMapDecorateDlg(CMapDecorateDlg_5KN::IDD, pParent)
{
}


void CMapDecorateDlg_5KN::DoDataExchange(CDataExchange* pDX)
{
	CMapDecorateDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapDecorateDlg_5KN)
	DDX_Control(pDX, IDC_NOTE_STATIC, m_staticNote);
	DDX_Control(pDX, IDC_DATADESC_STATIC, m_staticDataDesc);
	DDX_Control(pDX, IDC_MAKER_BOTTOM_STATIC, m_staticMakerBottom);
	DDX_Control(pDX, IDC_SECRET_STATIC, m_staticSecret);
	DDX_Control(pDX, IDC_SCALE_STATIC, m_staticScale);
	DDX_Control(pDX, IDC_NEARMAP_STATIC, m_staticNearMap);
	DDX_Control(pDX, IDC_AREA_STATIC, m_staticArea);
	DDX_Control(pDX, IDC_NUM_STATIC, m_staticNum);
	DDX_Control(pDX, IDC_STATIC_LBNAME, m_staticLBName);
	DDX_Control(pDX, IDC_STATIC_LTNAME, m_staticLTName);
	DDX_Control(pDX, IDC_STATIC_RBNAME, m_staticRBName);
	DDX_Control(pDX, IDC_STATIC_RTNAME, m_staticRTName);
	DDX_Control(pDX, IDC_STATIC_FIGURE, m_staticFigure);
	DDX_Control(pDX, IDC_STATIC_RULER, m_staticRuler);
	DDX_Control(pDX, IDC_DIGITAL_SCALE_STATIC, m_staticDigitalScale);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MAP_LIST, m_ctlCheckList);
	DDX_Control(pDX, IDC_NAME_STATIC, m_staticName);
	DDX_CBIndex(pDX, IDC_COMBO_MAPTYPE, m_nMapType);
	DDX_CBIndex(pDX, IDC_COMBO_MAPBORDER, m_nMapBorderType);
	DDX_Text(pDX, IDC_EDIT_WID, m_fMapWid);
	DDX_Text(pDX, IDC_EDIT_HEI, m_fMapHei);
	DDX_Text(pDX, IDC_EDIT_MAPNUM, m_strMapNum);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	DDX_Check(pDX, IDC_CHECK_CUT_STRIPNO, m_bCutStripNO);
}

BEGIN_MESSAGE_MAP(CMapDecorateDlg_5KN, CMapDecorateDlg)
	//{{AFX_MSG_MAP(CMapDecorateDlg_5KN)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_MAP_LIST, OnSelchangeMapList)
	ON_LBN_DBLCLK(IDC_MAP_LIST, OnDblClickMapList)
	ON_EN_CHANGE(IDC_NUM_EDIT, OnChangeNumEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SET_PARAM, OnSetParam)
	ON_BN_CLICKED(IDC_SELECT_MAPBORDER, OnSelectMapBorder)
	ON_BN_CLICKED(IDC_SET_COORDSYS, OnSetCoordSys)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPTYPE, OnSelchangeMaptype)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPBORDER, OnSelchangeMapBorder)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_SELECT_NONE, OnSelectNone)

	ON_BN_CLICKED(IDC_NAME_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NUM_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_AREA_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NEARMAP_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SCALE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SECRET_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_MAKER_BOTTOM_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_DATADESC_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NOTE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_FIGURE, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LBNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RBNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RULER, OnStaticButton)
	ON_BN_CLICKED(IDC_DIGITAL_SCALE_STATIC, OnStaticButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_5KN message handlers

BOOL CMapDecorateDlg_5KN::OnInitDialog()
{
	CMapDecorateDlg::OnInitDialog();

	// TODO: Add extra initialization here

	m_ctlCheckList.ResetContent();
	m_ctlCheckList.SetCheckStyle( BS_3STATE );

	m_arrObjIDStrings.Add("Name");
	m_arrObjIDStrings.Add("Number");
	m_arrObjIDStrings.Add("OuterBound");
	m_arrObjIDStrings.Add("InnerBound");
	m_arrObjIDStrings.Add("Grid");
	m_arrObjIDStrings.Add("AreaDesc");
	m_arrObjIDStrings.Add("AdminRegion_NearMap");
	m_arrObjIDStrings.Add("Secret");
	m_arrObjIDStrings.Add("Scale_5KN");
	m_arrObjIDStrings.Add("Ruler");
	m_arrObjIDStrings.Add("Figure");
	m_arrObjIDStrings.Add("DataDesc_5KN");
	m_arrObjIDStrings.Add("Note");
	m_arrObjIDStrings.Add("MakerBottom");
	m_arrObjIDStrings.Add("LTName");
	m_arrObjIDStrings.Add("LBName");
	m_arrObjIDStrings.Add("RTName");
	m_arrObjIDStrings.Add("RBName");
	m_arrObjIDStrings.Add("DigitalScale");

	m_arrPCtrls.Add(&m_staticName);
	m_arrPCtrls.Add(&m_staticNum);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticArea);
	m_arrPCtrls.Add(&m_staticNearMap);
	m_arrPCtrls.Add(&m_staticSecret);
	m_arrPCtrls.Add(&m_staticScale);
	m_arrPCtrls.Add(&m_staticRuler);
	m_arrPCtrls.Add(&m_staticFigure);
	m_arrPCtrls.Add(&m_staticDataDesc);
	m_arrPCtrls.Add(&m_staticNote);
	m_arrPCtrls.Add(&m_staticMakerBottom);
	m_arrPCtrls.Add(&m_staticLTName);
	m_arrPCtrls.Add(&m_staticLBName);
	m_arrPCtrls.Add(&m_staticRTName);
	m_arrPCtrls.Add(&m_staticRBName);
	m_arrPCtrls.Add(&m_staticDigitalScale);

	m_arrCtrlIDs.Add(IDC_NAME_STATIC);
	m_arrCtrlIDs.Add(IDC_NUM_STATIC);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(IDC_AREA_STATIC);
	m_arrCtrlIDs.Add(IDC_NEARMAP_STATIC);
	m_arrCtrlIDs.Add(IDC_SECRET_STATIC);
	m_arrCtrlIDs.Add(IDC_SCALE_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_RULER);
	m_arrCtrlIDs.Add(IDC_STATIC_FIGURE);
	m_arrCtrlIDs.Add(IDC_DATADESC_STATIC);
	m_arrCtrlIDs.Add(IDC_NOTE_STATIC);
	m_arrCtrlIDs.Add(IDC_MAKER_BOTTOM_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_LTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_LBNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RBNAME);
	m_arrCtrlIDs.Add(IDC_DIGITAL_SCALE_STATIC);


	for(int i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_arrPCtrls[i]->clicked = m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed?0:1;
	}

	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.AddString(m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->Name());
		m_ctlCheckList.SetCheck(i, 1);
	}
	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.SetCheck(i,m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed);
	}

	m_ctlCheckList.SetCurSel(0);
	m_staticName.SetTextColor(RGB(255,0,0));
	m_staticNum.SetTextColor(RGB(255,0,0));
	m_staticArea.SetTextColor(RGB(255,0,0));
	m_staticNearMap.SetTextColor(RGB(255,0,0));

	CRect rect;
	m_staticNearMap.GetWindowRect(&rect);
	ScreenToClient(&rect);

	//bouter_bound area_desc
	CWnd *pWnd = GetDlgItem(IDC_STATIC_OUTRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_outbound);
		ScreenToClient(&rect_outbound);
	}

	//binner_bound area_desc
	pWnd = GetDlgItem(IDC_STATIC_INRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_innerbound);
		ScreenToClient(&rect_innerbound);
	}

	rect_grid = rect_innerbound;
	rect_grid.DeflateRect(CSize(10,10)); //直接坐标网在图上每隔10cm绘一条

	//magline area_desc
	pWnd = GetDlgItem(IDC_STATIC_LINE);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_magnor);
		ScreenToClient(&rect_magnor);

		rect_magnor.InflateRect(CSize(10,0));
	}

	pWnd = GetDlgItem(IDC_STATIC_PARAM);
	if( pWnd )
	{
		CRect rcPos;
		pWnd->GetWindowRect(rcPos);

		ScreenToClient(&rcPos);

		if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rcPos, this, 2))
		{
		}

		m_wndPropList.SetWindowPos(NULL,rcPos.left,rcPos.top,
			rcPos.Width(),rcPos.Height(),0);

		m_wndPropList.EnableHeaderCtrl(TRUE,StrFromResID(IDS_COND_COL_FIELD),StrFromResID(IDS_COND_COL_VALUE));
	}
	
	m_nMapType = m_pMapDecorator->m_nMapType;
	m_nMapBorderType = m_pMapDecorator->m_nMapBorderType;
	m_fMapWid = m_pMapDecorator->m_fMapWid;
	m_fMapHei = m_pMapDecorator->m_fMapHei;

	m_strMapName = ((CName*)m_pMapDecorator->GetObj("Name"))->name;
	m_strMapNum = ((CNumber*)m_pMapDecorator->GetObj("Number"))->number;
	m_bCutStripNO = m_pMapDecorator->m_bCutStripNum;

	UpdateData(FALSE);
	
	OnSelchangeMapBorder();
	OnSelchangeMapList();
	OnSelchangeMaptype();

	m_nCurItemSel = 0;

	if( m_bMapDecorateSettings )
	{
		pWnd = GetDlgItem(IDC_COMBO_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNUM);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNAME);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_COMBO_MAPTYPE);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_WID);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_HEI);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_2KO dialog

CMapDecorateDlg_2KO::CMapDecorateDlg_2KO(CWnd* pParent /*=NULL*/)
: CMapDecorateDlg(CMapDecorateDlg_2KO::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(CMapDecorateDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

void CMapDecorateDlg_2KO::DoDataExchange(CDataExchange* pDX)
{
	CMapDecorateDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapDecorateDlg)
	DDX_Control(pDX, IDC_NOTE_STATIC, m_staticNote);
	DDX_Control(pDX, IDC_DATADESC_STATIC, m_staticDataDesc);
	DDX_Control(pDX, IDC_MAKER_RIGHT_STATIC, m_staticMakerRight);
	DDX_Control(pDX, IDC_MAKER_LEFT_STATIC, m_staticMakerLeft);
	DDX_Control(pDX, IDC_SECRET_STATIC, m_staticSecret);
//	DDX_Control(pDX, IDC_NORTH_STATIC, m_staticNorth);
	DDX_Control(pDX, IDC_SCALE_STATIC, m_staticScale);
	DDX_Control(pDX, IDC_NEARMAP_STATIC, m_staticNearMap);
	DDX_Control(pDX, IDC_AREA_STATIC, m_staticArea);
	DDX_Control(pDX, IDC_NUM_STATIC, m_staticNum);
	DDX_Control(pDX, IDC_STATIC_LBNAME, m_staticLBName);
	DDX_Control(pDX, IDC_STATIC_LTNAME, m_staticLTName);
	DDX_Control(pDX, IDC_STATIC_RBNAME, m_staticRBName);
	DDX_Control(pDX, IDC_STATIC_RTNAME, m_staticRTName);
	DDX_Control(pDX, IDC_STATIC_FIGURE, m_staticFigure);
//	DDX_Control(pDX, IDC_STATIC_RULER, m_staticRuler);
	DDX_Control(pDX, IDC_STATIC_CHECKMAN, m_staticCheckMan);
	DDX_Control(pDX, IDC_MAKER_RIGHT_STATIC, m_staticMakerRight);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MAP_LIST, m_ctlCheckList);
	DDX_Control(pDX, IDC_NAME_STATIC, m_staticName);
	DDX_CBIndex(pDX, IDC_COMBO_MAPTYPE, m_nMapType);
	DDX_CBIndex(pDX, IDC_COMBO_MAPBORDER, m_nMapBorderType);
	DDX_Text(pDX, IDC_EDIT_WID, m_fMapWid);
	DDX_Text(pDX, IDC_EDIT_HEI, m_fMapHei);
	DDX_Text(pDX, IDC_EDIT_MAPNUM, m_strMapNum);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	DDX_Check(pDX, IDC_CHECK_CUT_STRIPNO, m_bCutStripNO);
}

BEGIN_MESSAGE_MAP(CMapDecorateDlg_2KO, CMapDecorateDlg)
	//{{AFX_MSG_MAP(CMapDecorateDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_MAP_LIST, OnSelchangeMapList)
	ON_LBN_DBLCLK(IDC_MAP_LIST, OnDblClickMapList)
	ON_EN_CHANGE(IDC_NUM_EDIT, OnChangeNumEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SET_PARAM, OnSetParam)
	ON_BN_CLICKED(IDC_SELECT_MAPBORDER, OnSelectMapBorder)
	ON_BN_CLICKED(IDC_SET_COORDSYS, OnSetCoordSys)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPTYPE, OnSelchangeMaptype)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPBORDER, OnSelchangeMapBorder)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_SELECT_NONE, OnSelectNone)

	ON_BN_CLICKED(IDC_NAME_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NUM_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_AREA_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NEARMAP_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SCALE_STATIC, OnStaticButton)
//	ON_BN_CLICKED(IDC_NORTH_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SECRET_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_MAKER_LEFT_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_MAKER_RIGHT_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_DATADESC_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NOTE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_FIGURE, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LBNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RBNAME, OnStaticButton)
//	ON_BN_CLICKED(IDC_STATIC_RULER, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_CHECKMAN, OnStaticButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_2KO message handlers

BOOL CMapDecorateDlg_2KO::OnInitDialog()
{
	CMapDecorateDlg::OnInitDialog();

	// TODO: Add extra initialization here

	m_ctlCheckList.ResetContent();
	m_ctlCheckList.SetCheckStyle( BS_3STATE );

	m_arrObjIDStrings.Add("Name");
	m_arrObjIDStrings.Add("Number");
	m_arrObjIDStrings.Add("OuterBound");
	m_arrObjIDStrings.Add("InnerBound");
	m_arrObjIDStrings.Add("Grid");
	m_arrObjIDStrings.Add("AreaDesc");
	m_arrObjIDStrings.Add("Nearmap");
	m_arrObjIDStrings.Add("Secret");
	m_arrObjIDStrings.Add("Scale");
//	m_arrObjIDStrings.Add("Ruler");
	m_arrObjIDStrings.Add("Figure");
//	m_arrObjIDStrings.Add("NorthPointer");
	m_arrObjIDStrings.Add("Checkman");
	m_arrObjIDStrings.Add("DataDesc");
	m_arrObjIDStrings.Add("Note");
	m_arrObjIDStrings.Add("MakerLeft");
	m_arrObjIDStrings.Add("MakerRight");
	m_arrObjIDStrings.Add("LTName");
	m_arrObjIDStrings.Add("LBName");
	m_arrObjIDStrings.Add("RTName");
	m_arrObjIDStrings.Add("RBName");

	m_arrPCtrls.Add(&m_staticName);
	m_arrPCtrls.Add(&m_staticNum);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticArea);
	m_arrPCtrls.Add(&m_staticNearMap);
	m_arrPCtrls.Add(&m_staticSecret);
	m_arrPCtrls.Add(&m_staticScale);
//	m_arrPCtrls.Add(&m_staticRuler);
	m_arrPCtrls.Add(&m_staticFigure);
//	m_arrPCtrls.Add(&m_staticNorth);
	m_arrPCtrls.Add(&m_staticCheckMan);
	m_arrPCtrls.Add(&m_staticDataDesc);
	m_arrPCtrls.Add(&m_staticNote);
	m_arrPCtrls.Add(&m_staticMakerLeft);
	m_arrPCtrls.Add(&m_staticMakerRight);
	m_arrPCtrls.Add(&m_staticLTName);
	m_arrPCtrls.Add(&m_staticLBName);
	m_arrPCtrls.Add(&m_staticRTName);
	m_arrPCtrls.Add(&m_staticRBName);

	m_arrCtrlIDs.Add(IDC_NAME_STATIC);
	m_arrCtrlIDs.Add(IDC_NUM_STATIC);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(IDC_AREA_STATIC);
	m_arrCtrlIDs.Add(IDC_NEARMAP_STATIC);
	m_arrCtrlIDs.Add(IDC_SECRET_STATIC);
	m_arrCtrlIDs.Add(IDC_SCALE_STATIC);
//	m_arrCtrlIDs.Add(IDC_STATIC_RULER);
	m_arrCtrlIDs.Add(IDC_STATIC_FIGURE);
//	m_arrCtrlIDs.Add(IDC_NORTH_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_CHECKMAN);
	m_arrCtrlIDs.Add(IDC_DATADESC_STATIC);
	m_arrCtrlIDs.Add(IDC_NOTE_STATIC);
	m_arrCtrlIDs.Add(IDC_MAKER_LEFT_STATIC);
	m_arrCtrlIDs.Add(IDC_MAKER_RIGHT_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_LTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_LBNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RBNAME);


	for(int i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_arrPCtrls[i]->clicked = m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed?0:1;
	}

	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.AddString(m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->Name());
		m_ctlCheckList.SetCheck(i, 1);
	}
	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.SetCheck(i,m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed);
	}

	m_ctlCheckList.SetCurSel(0);
	m_staticName.SetTextColor(RGB(255,0,0));
	m_staticNum.SetTextColor(RGB(255,0,0));
	m_staticArea.SetTextColor(RGB(255,0,0));
	m_staticNearMap.SetTextColor(RGB(255,0,0));

	CRect rect;
	m_staticNearMap.GetWindowRect(&rect);
	ScreenToClient(&rect);

	//bouter_bound area_desc
	CWnd *pWnd = GetDlgItem(IDC_STATIC_OUTRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_outbound);
		ScreenToClient(&rect_outbound);
	}

	//binner_bound area_desc
	pWnd = GetDlgItem(IDC_STATIC_INRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_innerbound);
		ScreenToClient(&rect_innerbound);
	}

	rect_grid = rect_innerbound;
	rect_grid.DeflateRect(CSize(10,10));

	//magline area_desc
	pWnd = GetDlgItem(IDC_STATIC_LINE);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_magnor);
		ScreenToClient(&rect_magnor);

		rect_magnor.InflateRect(CSize(10,0));
	}

	pWnd = GetDlgItem(IDC_STATIC_PARAM);
	if( pWnd )
	{
		CRect rcPos;
		pWnd->GetWindowRect(rcPos);

		ScreenToClient(&rcPos);

		if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rcPos, this, 2))
		{
		}

		m_wndPropList.SetWindowPos(NULL,rcPos.left,rcPos.top,
			rcPos.Width(),rcPos.Height(),0);

		m_wndPropList.EnableHeaderCtrl(TRUE,StrFromResID(IDS_COND_COL_FIELD),StrFromResID(IDS_COND_COL_VALUE));
	}
	
	m_nMapType = m_pMapDecorator->m_nMapType;
	m_nMapBorderType = m_pMapDecorator->m_nMapBorderType;
	m_fMapWid = m_pMapDecorator->m_fMapWid;
	m_fMapHei = m_pMapDecorator->m_fMapHei;

	m_strMapName = ((CName*)m_pMapDecorator->GetObj("Name"))->name;
	m_strMapNum = ((CNumber*)m_pMapDecorator->GetObj("Number"))->number;
	m_bCutStripNO = m_pMapDecorator->m_bCutStripNum;

	UpdateData(FALSE);
	
	OnSelchangeMapBorder();
	OnSelchangeMapList();
	OnSelchangeMaptype();

	m_nCurItemSel = 0;

	if( m_bMapDecorateSettings )
	{
		pWnd = GetDlgItem(IDC_COMBO_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNUM);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNAME);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_COMBO_MAPTYPE);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_WID);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_HEI);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_25KN dialog

CMapDecorateDlg_25KN::CMapDecorateDlg_25KN(CWnd* pParent /*=NULL*/)
	: CMapDecorateDlg(CMapDecorateDlg_25KN::IDD, pParent)
{	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nCurItemSel = -1;

	m_bClickList = FALSE;
	m_pCmd = NULL;

	bOBD=bMagNor=bGrid=bInnerBound=bOutBound=FALSE;

	m_pMapDecorator = NULL;
	m_bMapDecorateSettings = FALSE;
}


void CMapDecorateDlg_25KN::DoDataExchange(CDataExchange* pDX)
{
	CMapDecorateDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapDecorateDlg_5KN)
	DDX_Control(pDX, IDC_NOTE_STATIC, m_staticNote);
	DDX_Control(pDX, IDC_DATADESC_STATIC, m_staticDataDesc);
	DDX_Control(pDX, IDC_MAKER_BOTTOM_STATIC, m_staticMakerBottom);
	DDX_Control(pDX, IDC_SECRET_STATIC, m_staticSecret);
	DDX_Control(pDX, IDC_NORTH_STATIC, m_staticNorth);
	DDX_Control(pDX, IDC_SCALE_STATIC, m_staticScale);
	DDX_Control(pDX, IDC_NEARMAP_STATIC, m_staticNearMap);
	DDX_Control(pDX, IDC_AREA_STATIC, m_staticArea);
	DDX_Control(pDX, IDC_NUM_STATIC, m_staticNum);
	DDX_Control(pDX, IDC_STATIC_LBNAME, m_staticLBName);
	DDX_Control(pDX, IDC_STATIC_LTNAME, m_staticLTName);
	DDX_Control(pDX, IDC_STATIC_RBNAME, m_staticRBName);
	DDX_Control(pDX, IDC_STATIC_RTNAME, m_staticRTName);
	DDX_Control(pDX, IDC_STATIC_FIGURE, m_staticFigure);
	DDX_Control(pDX, IDC_STATIC_RULER, m_staticRuler);
	DDX_Control(pDX, IDC_DIGITAL_SCALE_STATIC, m_staticDigitalScale);
	DDX_Control(pDX, IDC_STATIC_LINE, m_staticMagline);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MAP_LIST, m_ctlCheckList);
	DDX_Control(pDX, IDC_NAME_STATIC, m_staticName);
	DDX_CBIndex(pDX, IDC_COMBO_MAPTYPE, m_nMapType);
	DDX_CBIndex(pDX, IDC_COMBO_MAPBORDER, m_nMapBorderType);
	DDX_Text(pDX, IDC_EDIT_WID, m_fMapWid);
	DDX_Text(pDX, IDC_EDIT_HEI, m_fMapHei);
	DDX_Text(pDX, IDC_EDIT_MAPNUM, m_strMapNum);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	DDX_Check(pDX, IDC_CHECK_CUT_STRIPNO, m_bCutStripNO);
}

BEGIN_MESSAGE_MAP(CMapDecorateDlg_25KN, CMapDecorateDlg)
	//{{AFX_MSG_MAP(CMapDecorateDlg_25KN)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_MAP_LIST, OnSelchangeMapList)
	ON_LBN_DBLCLK(IDC_MAP_LIST, OnDblClickMapList)
	ON_EN_CHANGE(IDC_NUM_EDIT, OnChangeNumEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SET_PARAM, OnSetParam)
	ON_BN_CLICKED(IDC_SELECT_MAPBORDER, OnSelectMapBorder)
	ON_BN_CLICKED(IDC_SET_COORDSYS, OnSetCoordSys)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPTYPE, OnSelchangeMaptype)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPBORDER, OnSelchangeMapBorder)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_SELECT_NONE, OnSelectNone)

	ON_BN_CLICKED(IDC_NAME_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NUM_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_AREA_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NEARMAP_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SCALE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NORTH_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SECRET_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_MAKER_BOTTOM_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_DATADESC_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NOTE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_FIGURE, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LBNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RBNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RULER, OnStaticButton)
	ON_BN_CLICKED(IDC_DIGITAL_SCALE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LINE, OnStaticButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_25KN message handlers

BOOL CMapDecorateDlg_25KN::OnInitDialog()
{
	CMapDecorateDlg::OnInitDialog();

	// TODO: Add extra initialization here

	m_ctlCheckList.ResetContent();
	m_ctlCheckList.SetCheckStyle( BS_3STATE );

	m_arrObjIDStrings.Add("Name");
	m_arrObjIDStrings.Add("Number");
	m_arrObjIDStrings.Add("OuterBound");
	m_arrObjIDStrings.Add("OuterBoundDecorator");
	m_arrObjIDStrings.Add("InnerBound");
	m_arrObjIDStrings.Add("Grid_New");
	m_arrObjIDStrings.Add("AreaDesc");
	m_arrObjIDStrings.Add("AdminRegion_NearMap");
	m_arrObjIDStrings.Add("Secret");
	m_arrObjIDStrings.Add("Scale_25KN");
	m_arrObjIDStrings.Add("Ruler_25KN");
	m_arrObjIDStrings.Add("Figure");
	m_arrObjIDStrings.Add("NorthPointer_25KN");
	m_arrObjIDStrings.Add("DataDesc_5KN");
	m_arrObjIDStrings.Add("Note");
	m_arrObjIDStrings.Add("MakerBottom");
	m_arrObjIDStrings.Add("LTName");
	m_arrObjIDStrings.Add("LBName");
	m_arrObjIDStrings.Add("RTName");
	m_arrObjIDStrings.Add("RBName");
	m_arrObjIDStrings.Add("DigitalScale");
	m_arrObjIDStrings.Add("Magline_25KN");

	m_arrPCtrls.Add(&m_staticName);
	m_arrPCtrls.Add(&m_staticNum);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticArea);
	m_arrPCtrls.Add(&m_staticNearMap);
	m_arrPCtrls.Add(&m_staticSecret);
	m_arrPCtrls.Add(&m_staticScale);
	m_arrPCtrls.Add(&m_staticRuler);
	m_arrPCtrls.Add(&m_staticFigure);
	m_arrPCtrls.Add(&m_staticNorth);
	m_arrPCtrls.Add(&m_staticDataDesc);
	m_arrPCtrls.Add(&m_staticNote);
	m_arrPCtrls.Add(&m_staticMakerBottom);
	m_arrPCtrls.Add(&m_staticLTName);
	m_arrPCtrls.Add(&m_staticLBName);
	m_arrPCtrls.Add(&m_staticRTName);
	m_arrPCtrls.Add(&m_staticRBName);
	m_arrPCtrls.Add(&m_staticDigitalScale);
	m_arrPCtrls.Add(&m_staticMagline);

	m_arrCtrlIDs.Add(IDC_NAME_STATIC);
	m_arrCtrlIDs.Add(IDC_NUM_STATIC);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(IDC_AREA_STATIC);
	m_arrCtrlIDs.Add(IDC_NEARMAP_STATIC);
	m_arrCtrlIDs.Add(IDC_SECRET_STATIC);
	m_arrCtrlIDs.Add(IDC_SCALE_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_RULER);
	m_arrCtrlIDs.Add(IDC_STATIC_FIGURE);
	m_arrCtrlIDs.Add(IDC_NORTH_STATIC);
	m_arrCtrlIDs.Add(IDC_DATADESC_STATIC);
	m_arrCtrlIDs.Add(IDC_NOTE_STATIC);
	m_arrCtrlIDs.Add(IDC_MAKER_BOTTOM_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_LTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_LBNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RBNAME);
	m_arrCtrlIDs.Add(IDC_DIGITAL_SCALE_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_LINE);


	for(int i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_arrPCtrls[i]->clicked = m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed?0:1;
	}

	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.AddString(m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->Name());
		m_ctlCheckList.SetCheck(i, 1);
	}
	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.SetCheck(i,m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed);
	}

	m_ctlCheckList.SetCurSel(0);
	m_staticName.SetTextColor(RGB(255,0,0));
	m_staticNum.SetTextColor(RGB(255,0,0));
	m_staticArea.SetTextColor(RGB(255,0,0));
	m_staticNearMap.SetTextColor(RGB(255,0,0));

	CRect rect;
	m_staticNearMap.GetWindowRect(&rect);
	ScreenToClient(&rect);

	//bouter_bound area_desc
	CWnd *pWnd = GetDlgItem(IDC_STATIC_OUTRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_outbound);
		ScreenToClient(&rect_outbound);
	}

	//bouter_bound_decorator area_desc
	pWnd = GetDlgItem(IDC_STATIC_OBDRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_outbound_decorator);
		ScreenToClient(&rect_outbound_decorator);
	}

	//binner_bound area_desc
	pWnd = GetDlgItem(IDC_STATIC_INRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_innerbound);
		ScreenToClient(&rect_innerbound);
	}

	rect_grid = rect_innerbound;
	rect_grid.DeflateRect(CSize(10,10)); //直接坐标网在图上每隔10cm绘一条

	//magline area_desc
	pWnd = GetDlgItem(IDC_STATIC_LINE);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_magnor);
		ScreenToClient(&rect_magnor);

		rect_magnor.InflateRect(CSize(10,0));
	}

	pWnd = GetDlgItem(IDC_STATIC_PARAM);
	if( pWnd )
	{
		CRect rcPos;
		pWnd->GetWindowRect(rcPos);

		ScreenToClient(&rcPos);

		if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rcPos, this, 2))
		{
		}

		m_wndPropList.SetWindowPos(NULL,rcPos.left,rcPos.top,
			rcPos.Width(),rcPos.Height(),0);

		m_wndPropList.EnableHeaderCtrl(TRUE,StrFromResID(IDS_COND_COL_FIELD),StrFromResID(IDS_COND_COL_VALUE));
	}
	
	m_nMapType = m_pMapDecorator->m_nMapType;
	m_nMapBorderType = m_pMapDecorator->m_nMapBorderType;
	m_fMapWid = m_pMapDecorator->m_fMapWid;
	m_fMapHei = m_pMapDecorator->m_fMapHei;

	m_strMapName = ((CName*)m_pMapDecorator->GetObj("Name"))->name;
	m_strMapNum = ((CNumber*)m_pMapDecorator->GetObj("Number"))->number;
	m_bCutStripNO = m_pMapDecorator->m_bCutStripNum;

	UpdateData(FALSE);
	
	OnSelchangeMapBorder();
	OnSelchangeMapList();
	OnSelchangeMaptype();

	m_nCurItemSel = 0;

	if( m_bMapDecorateSettings )
	{
		pWnd = GetDlgItem(IDC_COMBO_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNUM);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNAME);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_COMBO_MAPTYPE);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_WID);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_HEI);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMapDecorateDlg_25KN::OnPaint() 
{
 //	UpdateData(FALSE);
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rect;
		
		m_staticNearMap.GetWindowRect(&rect);
		ScreenToClient(rect);
		int i,j;

		CPen pen;

		COLORREF color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("Grid"))==0 )
			color = RGB(169,169,169);
		if( bGrid )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,1,color);
		CPen *pOldPen = dc.SelectObject(&pen);

		// draw grid
		for(i=rect_outbound.top;i<rect_outbound.bottom;i+=30)
		{
			dc.MoveTo(CPoint(rect_outbound.left,i));
			dc.LineTo(CPoint(rect_outbound.right,i));
		}
		for(j=rect_outbound.left;j<rect_outbound.right;j+=30)
		{
			dc.MoveTo(CPoint(j,rect_outbound.top));
			dc.LineTo(j,rect_outbound.bottom);
		}
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw out rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("OuterBound"))==0 )
			color = RGB(169,169,169);
		if( bOutBound )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,3,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_outbound.left,rect_outbound.top));
		dc.LineTo(CPoint(rect_outbound.left,rect_outbound.bottom));
		dc.LineTo(CPoint(rect_outbound.right,rect_outbound.bottom));
		dc.LineTo(CPoint(rect_outbound.right,rect_outbound.top));
		dc.LineTo(CPoint(rect_outbound.left,rect_outbound.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw outbound decorator rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("OuterBoundDecorator"))==0 )
			color = RGB(169,169,169);
		if( bOBD )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,1,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_outbound_decorator.left,rect_outbound_decorator.top));
		dc.LineTo(CPoint(rect_outbound_decorator.left,rect_outbound_decorator.bottom));
		dc.LineTo(CPoint(rect_outbound_decorator.right,rect_outbound_decorator.bottom));
		dc.LineTo(CPoint(rect_outbound_decorator.right,rect_outbound_decorator.top));
		dc.LineTo(CPoint(rect_outbound_decorator.left,rect_outbound_decorator.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw inner rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("InnerBound"))==0 )
			color = RGB(169,169,169);
		if( bInnerBound )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,2,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_innerbound.left,rect_innerbound.top));
		dc.LineTo(CPoint(rect_innerbound.left,rect_innerbound.bottom));
		dc.LineTo(CPoint(rect_innerbound.right,rect_innerbound.bottom));
		dc.LineTo(CPoint(rect_innerbound.right,rect_innerbound.top));
		dc.LineTo(CPoint(rect_innerbound.left,rect_innerbound.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw the magline line
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("Magline_25KN"))==0 )
			color = RGB(169,169,169);
		if( bMagNor )
			color = RGB(255,255,0);

		pen.CreatePen(PS_DOT,1,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_magnor.CenterPoint().x,rect_outbound.top));
		dc.LineTo(CPoint(rect_magnor.CenterPoint().x,rect_outbound.bottom));
		dc.SelectObject(pOldPen);

		CDialog::OnPaint();
	}
}

void CMapDecorateDlg_25KN::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(rect_outbound.PtInRect(point))
	{
		if(rect_outbound_decorator.PtInRect(point))
		{
			if(rect_innerbound.PtInRect(point))
			{
				if(rect_grid.PtInRect(point))
				{
					if(rect_magnor.PtInRect(point))
					{
						m_ctlCheckList.SetCurSel(GetObjIndex("Magline_25KN"));
					}
					else
					{
						m_ctlCheckList.SetCurSel(GetObjIndex("Grid"));
					}
				}
				else
				{
					m_ctlCheckList.SetCurSel(GetObjIndex("InnerBound"));
				}
			}
			else 
			{
				m_ctlCheckList.SetCurSel(GetObjIndex("OuterBoundDecorator"));
			}
		}
		else 
		{
			m_ctlCheckList.SetCurSel(GetObjIndex("OuterBound"));
		}

		CRect rect = rect_outbound;
		rect.InflateRect(2,2);
		
		InvalidateRect(rect);
		
		UpdatePropList();
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CMapDecorateDlg_25KN::OnSelchangeMapList() 
{
	// TODO: Add your control notification handler code here
	int index=m_ctlCheckList.GetCurSel();
	int check=m_ctlCheckList.GetCheck(index);

	SaveCurrent();

	m_bClickList = TRUE;

	ClickStatic(index);

	CString id = m_arrObjIDStrings[index];
	if( id.CompareNoCase("InnerBound")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("OuterBound")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("Grid")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("Magline_25KN")==0 )
	{
		InvalidateRect(&rect_outbound);
	}
	else if( id.CompareNoCase("OuterBoundDecorator")==0 )
	{
		Invalidate();
	}
	
	m_nCurItemSel = index;

	m_bClickList = FALSE;
	  
}

void CMapDecorateDlg_25KN::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT tme;    
	tme.cbSize = sizeof(tme);    
	tme.dwFlags = TME_HOVER | TME_LEAVE;    
	tme.hwndTrack = m_hWnd;    
	tme.dwHoverTime = HOVER_DEFAULT;    
	_TrackMouseEvent(&tme);

	BOOL bGrid0=bGrid;
	BOOL bOutBound0=bOutBound;
	BOOL bInnerBound0=bInnerBound;
	BOOL bMagNor0=bMagNor;
	BOOL bOBD0=bOBD;

	bOBD=bMagNor=bGrid=bInnerBound=bOutBound=FALSE;

	//point in the bouter_bound area_desc
	if(rect_outbound.PtInRect(point))
	{
		if(rect_outbound_decorator.PtInRect(point))
		{
			//point in the binner_bound area_desc
			if(rect_innerbound.PtInRect(point))
			{
				//point in the bgrid area_desc
				if(rect_grid.PtInRect(point))
				{
					if(rect_magnor.PtInRect(point))
					{
						bMagNor = TRUE;					
					}
					else
					{
						bGrid = TRUE;
					}
				}
				else
				{
					bInnerBound=TRUE;
				}
				
			}
			else 
			{
				bOBD=TRUE;			
			}
		}
		else 
		{
			bOutBound=TRUE;			
		}
	}

	if(bGrid0==bGrid && bOutBound0==bOutBound && bInnerBound0==bInnerBound && bMagNor0==bMagNor && bOBD0==bOBD )
		return;
	
	CRect rect2 = rect_outbound;
	rect2.InflateRect(CSize(2,2));

	InvalidateRect(&rect2);

	CDialog::OnMouseMove(nFlags, point);
}




/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_25KO dialog

CMapDecorateDlg_25KO::CMapDecorateDlg_25KO(CWnd* pParent /*=NULL*/)
	: CMapDecorateDlg(CMapDecorateDlg_25KO::IDD, pParent)
{	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nCurItemSel = -1;

	m_bClickList = FALSE;
	m_pCmd = NULL;

	bOBD=bMagNor=bGrid=bInnerBound=bOutBound=FALSE;

	m_pMapDecorator = NULL;
	m_bMapDecorateSettings = FALSE;
}


void CMapDecorateDlg_25KO::DoDataExchange(CDataExchange* pDX)
{
	CMapDecorateDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapDecorateDlg_5KN)
	DDX_Control(pDX, IDC_NOTE_STATIC, m_staticNote);
	DDX_Control(pDX, IDC_DATADESC_STATIC, m_staticDataDesc);
	DDX_Control(pDX, IDC_MAKER_LEFT_STATIC, m_staticMakerLeft);
	DDX_Control(pDX, IDC_SECRET_STATIC, m_staticSecret);
	DDX_Control(pDX, IDC_NORTH_STATIC, m_staticNorth);
	DDX_Control(pDX, IDC_SCALE_STATIC, m_staticScale);
	DDX_Control(pDX, IDC_NEARMAP_STATIC, m_staticNearMap);
	DDX_Control(pDX, IDC_AREA_STATIC, m_staticArea);
	DDX_Control(pDX, IDC_NUM_STATIC, m_staticNum);
	DDX_Control(pDX, IDC_STATIC_LBNAME, m_staticLBName);
	DDX_Control(pDX, IDC_STATIC_LTNAME, m_staticLTName);
	DDX_Control(pDX, IDC_STATIC_RBNAME, m_staticRBName);
	DDX_Control(pDX, IDC_STATIC_RTNAME, m_staticRTName);
	DDX_Control(pDX, IDC_STATIC_FIGURE, m_staticFigure);
	DDX_Control(pDX, IDC_STATIC_RULER, m_staticRuler);
	DDX_Control(pDX, IDC_STATIC_LINE, m_staticMagline);
	DDX_Control(pDX, IDC_ADMINREGION_STATIC, m_staticAdminRegion);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MAP_LIST, m_ctlCheckList);
	DDX_Control(pDX, IDC_NAME_STATIC, m_staticName);
	DDX_CBIndex(pDX, IDC_COMBO_MAPTYPE, m_nMapType);
	DDX_CBIndex(pDX, IDC_COMBO_MAPBORDER, m_nMapBorderType);
	DDX_Text(pDX, IDC_EDIT_WID, m_fMapWid);
	DDX_Text(pDX, IDC_EDIT_HEI, m_fMapHei);
	DDX_Text(pDX, IDC_EDIT_MAPNUM, m_strMapNum);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	DDX_Check(pDX, IDC_CHECK_CUT_STRIPNO, m_bCutStripNO);
}

BEGIN_MESSAGE_MAP(CMapDecorateDlg_25KO, CMapDecorateDlg)
	//{{AFX_MSG_MAP(CMapDecorateDlg_25KO)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_MAP_LIST, OnSelchangeMapList)
	ON_LBN_DBLCLK(IDC_MAP_LIST, OnDblClickMapList)
	ON_EN_CHANGE(IDC_NUM_EDIT, OnChangeNumEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SET_PARAM, OnSetParam)
	ON_BN_CLICKED(IDC_SELECT_MAPBORDER, OnSelectMapBorder)
	ON_BN_CLICKED(IDC_SET_COORDSYS, OnSetCoordSys)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPTYPE, OnSelchangeMaptype)
	ON_CBN_SELCHANGE(IDC_COMBO_MAPBORDER, OnSelchangeMapBorder)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_SELECT_NONE, OnSelectNone)

	ON_BN_CLICKED(IDC_NAME_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NUM_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_AREA_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NEARMAP_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SCALE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NORTH_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_SECRET_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_MAKER_LEFT_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_DATADESC_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_NOTE_STATIC, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_FIGURE, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LBNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RTNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RBNAME, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_RULER, OnStaticButton)
	ON_BN_CLICKED(IDC_STATIC_LINE, OnStaticButton)
	ON_BN_CLICKED(IDC_ADMINREGION_STATIC, OnStaticButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDecorateDlg_25KO message handlers

BOOL CMapDecorateDlg_25KO::OnInitDialog()
{
	CMapDecorateDlg::OnInitDialog();

	// TODO: Add extra initialization here

	m_ctlCheckList.ResetContent();
	m_ctlCheckList.SetCheckStyle( BS_3STATE );

	m_arrObjIDStrings.Add("Name");
	m_arrObjIDStrings.Add("Number");
	m_arrObjIDStrings.Add("OuterBound");
	m_arrObjIDStrings.Add("OuterBoundDecorator");
	m_arrObjIDStrings.Add("InnerBound");
	m_arrObjIDStrings.Add("Grid_New");
	m_arrObjIDStrings.Add("AreaDesc");
	m_arrObjIDStrings.Add("NearMap");
	m_arrObjIDStrings.Add("Secret");
	m_arrObjIDStrings.Add("Scale_25KN");
	m_arrObjIDStrings.Add("Ruler_25KN");
	m_arrObjIDStrings.Add("Figure");
	m_arrObjIDStrings.Add("NorthPointer_25KN");
	m_arrObjIDStrings.Add("DataDesc");
	m_arrObjIDStrings.Add("Note");
	m_arrObjIDStrings.Add("MakerLeft");
	m_arrObjIDStrings.Add("LTName");
	m_arrObjIDStrings.Add("LBName");
	m_arrObjIDStrings.Add("RTName");
	m_arrObjIDStrings.Add("RBName");
	m_arrObjIDStrings.Add("Magline_25KN");
	m_arrObjIDStrings.Add("AdminRegion");

	m_arrPCtrls.Add(&m_staticName);
	m_arrPCtrls.Add(&m_staticNum);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticDummy);
	m_arrPCtrls.Add(&m_staticArea);
	m_arrPCtrls.Add(&m_staticNearMap);
	m_arrPCtrls.Add(&m_staticSecret);
	m_arrPCtrls.Add(&m_staticScale);
	m_arrPCtrls.Add(&m_staticRuler);
	m_arrPCtrls.Add(&m_staticFigure);
	m_arrPCtrls.Add(&m_staticNorth);
	m_arrPCtrls.Add(&m_staticDataDesc);
	m_arrPCtrls.Add(&m_staticNote);
	m_arrPCtrls.Add(&m_staticMakerLeft);
	m_arrPCtrls.Add(&m_staticLTName);
	m_arrPCtrls.Add(&m_staticLBName);
	m_arrPCtrls.Add(&m_staticRTName);
	m_arrPCtrls.Add(&m_staticRBName);
	m_arrPCtrls.Add(&m_staticMagline);
	m_arrPCtrls.Add(&m_staticAdminRegion);

	m_arrCtrlIDs.Add(IDC_NAME_STATIC);
	m_arrCtrlIDs.Add(IDC_NUM_STATIC);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(0);
	m_arrCtrlIDs.Add(IDC_AREA_STATIC);
	m_arrCtrlIDs.Add(IDC_NEARMAP_STATIC);
	m_arrCtrlIDs.Add(IDC_SECRET_STATIC);
	m_arrCtrlIDs.Add(IDC_SCALE_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_RULER);
	m_arrCtrlIDs.Add(IDC_STATIC_FIGURE);
	m_arrCtrlIDs.Add(IDC_NORTH_STATIC);
	m_arrCtrlIDs.Add(IDC_DATADESC_STATIC);
	m_arrCtrlIDs.Add(IDC_NOTE_STATIC);
	m_arrCtrlIDs.Add(IDC_MAKER_LEFT_STATIC);
	m_arrCtrlIDs.Add(IDC_STATIC_LTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_LBNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RTNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_RBNAME);
	m_arrCtrlIDs.Add(IDC_STATIC_LINE);
	m_arrCtrlIDs.Add(IDC_ADMINREGION_STATIC);


	for(int i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_arrPCtrls[i]->clicked = m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed?0:1;
	}

	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.AddString(m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->Name());
		m_ctlCheckList.SetCheck(i, 1);
	}
	
	for(i=0;i<m_arrObjIDStrings.GetSize(); i++)
	{
		m_ctlCheckList.SetCheck(i,m_pMapDecorator->GetObj(m_arrObjIDStrings[i])->m_bUsed);
	}

	m_ctlCheckList.SetCurSel(0);
	m_staticName.SetTextColor(RGB(255,0,0));
	m_staticNum.SetTextColor(RGB(255,0,0));
	m_staticArea.SetTextColor(RGB(255,0,0));
	m_staticNearMap.SetTextColor(RGB(255,0,0));

	CRect rect;
	m_staticNearMap.GetWindowRect(&rect);
	ScreenToClient(&rect);

	//bouter_bound area_desc
	CWnd *pWnd = GetDlgItem(IDC_STATIC_OUTRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_outbound);
		ScreenToClient(&rect_outbound);
	}

	//bouter_bound_decorator area_desc
	pWnd = GetDlgItem(IDC_STATIC_OBDRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_outbound_decorator);
		ScreenToClient(&rect_outbound_decorator);
	}

	//binner_bound area_desc
	pWnd = GetDlgItem(IDC_STATIC_INRECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_innerbound);
		ScreenToClient(&rect_innerbound);
	}

	rect_grid = rect_innerbound;
	rect_grid.DeflateRect(CSize(10,10)); //直接坐标网在图上每隔10cm绘一条

	//magline area_desc
	pWnd = GetDlgItem(IDC_STATIC_LINE);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rect_magnor);
		ScreenToClient(&rect_magnor);

		rect_magnor.InflateRect(CSize(10,0));
	}

	pWnd = GetDlgItem(IDC_STATIC_PARAM);
	if( pWnd )
	{
		CRect rcPos;
		pWnd->GetWindowRect(rcPos);

		ScreenToClient(&rcPos);

		if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rcPos, this, 2))
		{
		}

		m_wndPropList.SetWindowPos(NULL,rcPos.left,rcPos.top,
			rcPos.Width(),rcPos.Height(),0);

		m_wndPropList.EnableHeaderCtrl(TRUE,StrFromResID(IDS_COND_COL_FIELD),StrFromResID(IDS_COND_COL_VALUE));
	}
	
	m_nMapType = m_pMapDecorator->m_nMapType;
	m_nMapBorderType = m_pMapDecorator->m_nMapBorderType;
	m_fMapWid = m_pMapDecorator->m_fMapWid;
	m_fMapHei = m_pMapDecorator->m_fMapHei;

	m_strMapName = ((CName*)m_pMapDecorator->GetObj("Name"))->name;
	m_strMapNum = ((CNumber*)m_pMapDecorator->GetObj("Number"))->number;
	m_bCutStripNO = m_pMapDecorator->m_bCutStripNum;

	UpdateData(FALSE);
	
	OnSelchangeMapBorder();
	OnSelchangeMapList();
	OnSelchangeMaptype();

	m_nCurItemSel = 0;

	if( m_bMapDecorateSettings )
	{
		pWnd = GetDlgItem(IDC_COMBO_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_SELECT_MAPBORDER);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNUM);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_MAPNAME);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_COMBO_MAPTYPE);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_WID);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_HEI);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMapDecorateDlg_25KO::OnPaint() 
{
 //	UpdateData(FALSE);
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rect;
		
		m_staticNearMap.GetWindowRect(&rect);
		ScreenToClient(rect);
		int i,j;

		CPen pen;

		COLORREF color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("Grid"))==0 )
			color = RGB(169,169,169);
		if( bGrid )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,1,color);
		CPen *pOldPen = dc.SelectObject(&pen);

		// draw grid
		for(i=rect_outbound.top;i<rect_outbound.bottom;i+=30)
		{
			dc.MoveTo(CPoint(rect_outbound.left,i));
			dc.LineTo(CPoint(rect_outbound.right,i));
		}
		for(j=rect_outbound.left;j<rect_outbound.right;j+=30)
		{
			dc.MoveTo(CPoint(j,rect_outbound.top));
			dc.LineTo(j,rect_outbound.bottom);
		}
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw out rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("OuterBound"))==0 )
			color = RGB(169,169,169);
		if( bOutBound )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,3,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_outbound.left,rect_outbound.top));
		dc.LineTo(CPoint(rect_outbound.left,rect_outbound.bottom));
		dc.LineTo(CPoint(rect_outbound.right,rect_outbound.bottom));
		dc.LineTo(CPoint(rect_outbound.right,rect_outbound.top));
		dc.LineTo(CPoint(rect_outbound.left,rect_outbound.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw outbound decorator rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("OuterBoundDecorator"))==0 )
			color = RGB(169,169,169);
		if( bOBD )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,1,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_outbound_decorator.left,rect_outbound_decorator.top));
		dc.LineTo(CPoint(rect_outbound_decorator.left,rect_outbound_decorator.bottom));
		dc.LineTo(CPoint(rect_outbound_decorator.right,rect_outbound_decorator.bottom));
		dc.LineTo(CPoint(rect_outbound_decorator.right,rect_outbound_decorator.top));
		dc.LineTo(CPoint(rect_outbound_decorator.left,rect_outbound_decorator.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw inner rectangle
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("InnerBound"))==0 )
			color = RGB(169,169,169);
		if( bInnerBound )
			color = RGB(255,255,0);

		pen.CreatePen(PS_SOLID,2,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_innerbound.left,rect_innerbound.top));
		dc.LineTo(CPoint(rect_innerbound.left,rect_innerbound.bottom));
		dc.LineTo(CPoint(rect_innerbound.right,rect_innerbound.bottom));
		dc.LineTo(CPoint(rect_innerbound.right,rect_innerbound.top));
		dc.LineTo(CPoint(rect_innerbound.left,rect_innerbound.top));
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

		//draw the magline line
		color = RGB(255,0,0);
		if( m_ctlCheckList.GetCheck(GetObjIndex("Magline_25KN"))==0 )
			color = RGB(169,169,169);
		if( bMagNor )
			color = RGB(255,255,0);

		pen.CreatePen(PS_DOT,1,color);

		pOldPen = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(rect_magnor.CenterPoint().x,rect_outbound.top));
		dc.LineTo(CPoint(rect_magnor.CenterPoint().x,rect_outbound.bottom));
		dc.SelectObject(pOldPen);

		CDialog::OnPaint();
	}
}

void CMapDecorateDlg_25KO::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(rect_outbound.PtInRect(point))
	{
		if(rect_outbound_decorator.PtInRect(point))
		{
			if(rect_innerbound.PtInRect(point))
			{
				if(rect_grid.PtInRect(point))
				{
					if(rect_magnor.PtInRect(point))
					{
						m_ctlCheckList.SetCurSel(GetObjIndex("Magline_25KN"));
					}
					else
					{
						m_ctlCheckList.SetCurSel(GetObjIndex("Grid"));
					}
				}
				else
				{
					m_ctlCheckList.SetCurSel(GetObjIndex("InnerBound"));
				}
			}
			else 
			{
				m_ctlCheckList.SetCurSel(GetObjIndex("OuterBoundDecorator"));
			}
		}
		else 
		{
			m_ctlCheckList.SetCurSel(GetObjIndex("OuterBound"));
		}

		CRect rect = rect_outbound;
		rect.InflateRect(2,2);
		
		InvalidateRect(rect);
		
		UpdatePropList();
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CMapDecorateDlg_25KO::OnSelchangeMapList() 
{
	// TODO: Add your control notification handler code here
	int index=m_ctlCheckList.GetCurSel();
	int check=m_ctlCheckList.GetCheck(index);

	SaveCurrent();

	m_bClickList = TRUE;

	ClickStatic(index);

	CString id = m_arrObjIDStrings[index];
	if( id.CompareNoCase("InnerBound")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("OuterBound")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("Grid")==0 )
	{
		Invalidate();
	}
	else if( id.CompareNoCase("Magline_25KN")==0 )
	{
		InvalidateRect(&rect_outbound);
	}
	else if( id.CompareNoCase("OuterBoundDecorator")==0 )
	{
		Invalidate();
	}
	
	m_nCurItemSel = index;

	m_bClickList = FALSE;
	  
}

void CMapDecorateDlg_25KO::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT tme;    
	tme.cbSize = sizeof(tme);    
	tme.dwFlags = TME_HOVER | TME_LEAVE;    
	tme.hwndTrack = m_hWnd;    
	tme.dwHoverTime = HOVER_DEFAULT;    
	_TrackMouseEvent(&tme);

	BOOL bGrid0=bGrid;
	BOOL bOutBound0=bOutBound;
	BOOL bInnerBound0=bInnerBound;
	BOOL bMagNor0=bMagNor;
	BOOL bOBD0=bOBD;

	bOBD=bMagNor=bGrid=bInnerBound=bOutBound=FALSE;

	//point in the bouter_bound area_desc
	if(rect_outbound.PtInRect(point))
	{
		if(rect_outbound_decorator.PtInRect(point))
		{
			//point in the binner_bound area_desc
			if(rect_innerbound.PtInRect(point))
			{
				//point in the bgrid area_desc
				if(rect_grid.PtInRect(point))
				{
					if(rect_magnor.PtInRect(point))
					{
						bMagNor = TRUE;					
					}
					else
					{
						bGrid = TRUE;
					}
				}
				else
				{
					bInnerBound=TRUE;
				}
				
			}
			else 
			{
				bOBD=TRUE;			
			}
		}
		else 
		{
			bOutBound=TRUE;			
		}
	}

	if(bGrid0==bGrid && bOutBound0==bOutBound && bInnerBound0==bInnerBound && bMagNor0==bMagNor && bOBD0==bOBD )
		return;
	
	CRect rect2 = rect_outbound;
	rect2.InflateRect(CSize(2,2));

	InvalidateRect(&rect2);

	CDialog::OnMouseMove(nFlags, point);
}