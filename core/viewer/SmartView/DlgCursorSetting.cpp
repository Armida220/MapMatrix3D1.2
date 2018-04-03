#include "stdafx.h"
#include "DlgCursorSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CBmpListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CBitmap *pBmp = (CBitmap*)(lpDrawItemStruct->itemData);

	if( lpDrawItemStruct->itemID==LB_ERR )return;
	
	if( lpDrawItemStruct->itemAction&(ODA_DRAWENTIRE | ODA_SELECT))
	{
		DrawItem(pDC,pBmp,lpDrawItemStruct->rcItem,lpDrawItemStruct->itemState& ODS_SELECTED);
	}
	else if( lpDrawItemStruct->itemAction&ODA_FOCUS )
	{
		DrawItem(pDC,pBmp,lpDrawItemStruct->rcItem,lpDrawItemStruct->itemState&ODS_FOCUS);
	}
}

void CBmpListBox::MeasureItem(LPMEASUREITEMSTRUCT lpDrawItemStruct )
{
	lpDrawItemStruct-> itemHeight = m_nHeight;
}


void CBmpListBox::DrawItem(CDC *pDC, CBitmap *pBmp, CRect rect, BOOL bSelect)
{
	BITMAP info;
	pBmp->GetBitmap(&info);
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dcMem.SelectObject(pBmp);
	
	pDC->FillSolidRect(rect,RGB(0,0,0));
	if( bSelect )
	{
		pDC->BitBlt( rect.left+2+10, rect.top+2+(rect.Height()-info.bmHeight)/2 ,
					info.bmWidth-2,info.bmHeight-2,
					&dcMem,0,0,SRCCOPY);

		pDC->DrawEdge(&rect,BDR_RAISEDINNER,BF_RECT);
	}
	else
	{
		pDC->BitBlt( rect.left+10,rect.top+(rect.Height()-info.bmHeight)/2,
					 info.bmWidth,info.bmHeight,
					 &dcMem,0,0,SRCCOPY);

	}
	dcMem.SelectObject(pOldBmp);
	dcMem.DeleteDC();
}



IMPLEMENT_DYNCREATE(CDlgCursorSetting, CDialog)

CDlgCursorSetting::CDlgCursorSetting() : CDialog(CDlgCursorSetting::IDD)
{
	m_pCursor = NULL;
	m_nCursorIdx = 0;
	m_wndColorBtn.SetColor(RGB(255,255,255));

	for( int i=0; i<sizeof(m_bmpItems)/sizeof(m_bmpItems[0]); i++)
	{
		m_bmpItems[i].LoadBitmap(IDB_BITMAP_CURSOR);
	}
}

CDlgCursorSetting::~CDlgCursorSetting()
{
}

void CDlgCursorSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCursorSetting)
	DDX_Control(pDX, IDC_COLOR_CURSOR, m_wndColorBtn);
	DDX_Control(pDX, IDC_LIST_CURSORS, m_wndCursorList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCursorSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgCursorSetting)
	ON_LBN_SELCHANGE(IDC_LIST_CURSORS, OnSelchangeList)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCursorSetting message handlers

BOOL CDlgCursorSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( m_pCursor==NULL )
		return TRUE;

	int maxHeight = 30;

	//初始化对话框变量
	m_nCursorIdx  = m_pCursor->GetCursorType();
	m_wndColorBtn.SetColor(m_pCursor->GetCursorColor());
	
	//创建绘图环境
	CDC dcMem;
	CClientDC dc(this);
	CDC *pDC = &dc;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp=(CBitmap*)dcMem.SelectObject(&(m_bmpItems[0]));

	CDrawingContext *pNewDC = new C2DGDIDrawingContext;
	if( !pNewDC )return FALSE;

	pNewDC->CreateContext((HDC)dcMem);

	CDrawingContext *pOldDC = m_pCursor->GetContext();
	m_pCursor->SetContext(pNewDC);

	//准备光标
	BOOL bSelMarkVisible = m_pCursor->IsSelMarkVisible();
	BOOL bCrossVisible = m_pCursor->IsCrossVisible();
	BOOL bAttribute = m_pCursor->GetAttribute();
	m_pCursor->EnableSelMarkVisible(FALSE);
	m_pCursor->EnableCrossVisible(TRUE);
	m_pCursor->ModifyAttribute(CDrawingLayer::attrVisible,0);

	float ptOldx = m_pCursor->GetPos(0), ptOldy = m_pCursor->GetPos(1);

	//依次绘制各个光标
	BITMAP info;
	for (int i=0;i<m_pCursor->GetCursorCount();i++)
	{
		m_bmpItems[i].GetBitmap(&info);
		if( maxHeight<info.bmHeight )maxHeight = info.bmHeight;

		dcMem.SelectObject(&(m_bmpItems[i]));
		dcMem.FillSolidRect(0,0,info.bmWidth,info.bmHeight,0);

		pNewDC->BeginDrawing();
		pNewDC->SetDCSize(CSize(info.bmWidth,info.bmHeight));
		pNewDC->SetViewRect(CRect(0,0,info.bmWidth,info.bmHeight));
		m_pCursor->SetPos(info.bmWidth*0.5+0.5,info.bmHeight*0.5+0.5);

		m_pCursor->SetCursorType(i);
		m_pCursor->SetCursorColor(RGB(255,255,255));
		m_pCursor->Draw();
		
		pNewDC->EndDrawing();
	}
	m_pCursor->EnableSelMarkVisible(bSelMarkVisible);
	m_pCursor->EnableCrossVisible(bCrossVisible);
	m_pCursor->SetPos(ptOldx,ptOldy);
	m_pCursor->SetAttribute(bAttribute);

	m_pCursor->SetContext(pOldDC);
	delete pNewDC;

	dcMem.SelectObject(pOldBmp);

	//填充列表
	m_wndCursorList.SetMyHeight(maxHeight);
	m_wndCursorList.SetItemHeight(0,maxHeight);
	m_wndCursorList.SetColumnWidth(100);
	for( i=0; i<m_pCursor->GetCursorCount(); i++)
		m_wndCursorList.AddString((LPCTSTR)&m_bmpItems[i]);
	
	m_wndCursorList.SetCurSel(m_nCursorIdx);
	
	UpdateData(FALSE);
	return TRUE;
}


void CDlgCursorSetting::OnSelchangeList() 
{
	m_nCursorIdx = m_wndCursorList.GetCurSel();
}

void CDlgCursorSetting::OnPaint() 
{
	CPaintDC dc(this);
}


BOOL CDlgCursorSetting::SetCursor(CCursorLayer *pCursor)
{
    m_pCursor = pCursor;
	if (m_pCursor)
	{
		m_nCursorIdx  = m_pCursor->GetCursorType();
		m_wndColorBtn.SetColor(m_pCursor->GetCursorColor());
	}

	return TRUE;
}

void CDlgCursorSetting::OnOK() 
{
	if( m_pCursor )
	{
		m_pCursor->SetCursorType(m_nCursorIdx);
		m_pCursor->SetCursorColor(m_wndColorBtn.GetColor());
	}
	CDialog::OnOK();
}

		