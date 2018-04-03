// DlgChooseMP.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "editbasedoc.h"
#include "DlgChooseMP.h"
#include "UVSModify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDrawGraphStatic1::CDrawGraphStatic1()
{
	m_bIsExistForVecWin = false;
	m_lfMsOrgLogicX = 0;
	m_lfMsOrgLogicY = 0;
	m_lfMsOrgWorldX = 0;
	m_lfMsOrgWorldY = 0;
	m_msMove = NULL;
	//m_nRatioForZoom = 2;
}

CDrawGraphStatic1::~CDrawGraphStatic1()
{
}

BEGIN_MESSAGE_MAP(CDrawGraphStatic1, CStatic)
	//{{AFX_MSG_MAP(CDrawGraphStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CDrawGraphStatic1::Create(LPCTSTR lpszText, DWORD dwStyle,
	const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL ret = CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
	EnableToolTips(TRUE);

	return ret;
}

void CDrawGraphStatic1::SetDrawArea(const CRect &rect)
{
	m_rectDrawArea = rect;
}

void CDrawGraphStatic1::AddModelScope(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4, CString str)
{
	m_arrModelScopes.Add(ModelScope(p1, p2, p3, p4, str));
}

void CDrawGraphStatic1::ClearMSArray()
{
	m_arrModelScopes.RemoveAll();
	m_bIsExistForVecWin = false;
}

void CDrawGraphStatic1::SetCurrentVecWin(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4)
{
	m_msMain.p1 = p1;
	m_msMain.p2 = p2;
	m_msMain.p3 = p3;
	m_msMain.p4 = p4;
	m_bIsExistForVecWin = true;
}

void CDrawGraphStatic1::DrawScope(CDC *pDC, const ModelScope &ms)
{
	pDC->MoveTo(CPoint(ms.p1.x, ms.p1.y));
	pDC->LineTo(CPoint(ms.p2.x, ms.p2.y));
	pDC->LineTo(CPoint(ms.p3.x, ms.p3.y));
	pDC->LineTo(CPoint(ms.p4.x, ms.p4.y));
	pDC->LineTo(CPoint(ms.p1.x, ms.p1.y));
}

BOOL CDrawGraphStatic1::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDrawGraphStatic1::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	//填充背景
	CRect rect;
	GetClientRect(&rect);

	//帖按钮
	CDC MemDC0, MemDC1;
	MemDC0.CreateCompatibleDC(&dc);
	MemDC1.CreateCompatibleDC(&dc);

	CBitmap bmp2;
	bmp2.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* pOldBitmap1 = MemDC1.SelectObject(&bmp2);
	MemDC1.FillSolidRect(&rect, RGB(255, 255, 255));

	CDC &MemDC = MemDC1;

	/*CFont  font;
	font.CreatePointFont(66, "Arial");
	MemDC.SelectObject(font);*/

	//限定画图范围
	CRgn rgn;
	CRect rect1 = m_rectDrawArea;
	rect1.InflateRect(1, 1, 0, 1);
	rgn.CreateRectRgnIndirect(&rect1);
	MemDC.SelectClipRgn(&rgn);

	for(int i = 0; i < m_arrModelScopes.GetSize(); i++)
	{
		ModelScope ms = m_arrModelScopes.GetAt(i);

		int width = 0;
		int color = RGB(180, 180, 180);
		if (ms.flag==1)
		{
			width = 2;
			color = RGB(0, 0, 255);
		}
		CPen newPen(PS_SOLID, width, color);
		CPen *pOldPen = MemDC.SelectObject(&newPen);
		GeoToDev(&ms);
		DrawScope(&MemDC, ms);
		MemDC.SelectObject(pOldPen);
	}

	if (m_msMove)
	{
		ModelScope ms = *m_msMove;
		GeoToDev(&ms);
		MemDC.SetTextColor(RGB(255, 0, 255));
		MemDC.SetTextAlign(VTA_CENTER);
		int x = (ms.p1.x + ms.p2.x + ms.p3.x + ms.p4.x) / 4;
		int y = (ms.p1.y + ms.p2.y + ms.p3.y + ms.p4.y) / 4;
		MemDC.TextOut(x, y, m_msMove->name);
	}

	if (m_bIsExistForVecWin)
	{
		CPen newPen(PS_SOLID, 0, RGB(255, 0, 0));
		CPen *pOldPen = MemDC.SelectObject(&newPen);
		ModelScope ms = m_msMain;
		GeoToDev(&ms);
		DrawScope(&MemDC, ms);
		MemDC.SelectObject(pOldPen);

		MemDC.SetTextColor(RGB(255, 0, 0));
		MemDC.SetTextAlign(VTA_CENTER);
		int x = (ms.p1.x + ms.p2.x + ms.p3.x + ms.p4.x) / 4;
		int y = (ms.p1.y + ms.p2.y + ms.p3.y + ms.p4.y) / 4;
		MemDC.TextOut(x, y, StrFromResID(IDS_MAINDS));
	}

	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject(pOldBitmap1);
	MemDC.DeleteDC();
	dc.SelectClipRgn(NULL);
}

void CDrawGraphStatic1::GeoToDev(ModelScope *ms)
{
	(ms->p1).x = ((ms->p1).x - m_lfMsOrgWorldX) / m_lfRatioForCoorTrans + m_lfMsOrgLogicX;
	(ms->p1).y = -((ms->p1).y - m_lfMsOrgWorldY) / m_lfRatioForCoorTrans + m_lfMsOrgLogicY;
	(ms->p2).x = ((ms->p2).x - m_lfMsOrgWorldX) / m_lfRatioForCoorTrans + m_lfMsOrgLogicX;
	(ms->p2).y = -((ms->p2).y - m_lfMsOrgWorldY) / m_lfRatioForCoorTrans + m_lfMsOrgLogicY;
	(ms->p3).x = ((ms->p3).x - m_lfMsOrgWorldX) / m_lfRatioForCoorTrans + m_lfMsOrgLogicX;
	(ms->p3).y = -((ms->p3).y - m_lfMsOrgWorldY) / m_lfRatioForCoorTrans + m_lfMsOrgLogicY;
	(ms->p4).x = ((ms->p4).x - m_lfMsOrgWorldX) / m_lfRatioForCoorTrans + m_lfMsOrgLogicX;
	(ms->p4).y = -((ms->p4).y - m_lfMsOrgWorldY) / m_lfRatioForCoorTrans + m_lfMsOrgLogicY;
}

void CDrawGraphStatic1::GeoToDev(PT_2D *pt)
{
	pt->x = (pt->x - m_lfMsOrgWorldX) / m_lfRatioForCoorTrans + m_lfMsOrgLogicX;
	pt->y = -(pt->y - m_lfMsOrgWorldY) / m_lfRatioForCoorTrans + m_lfMsOrgLogicY;
}

void CDrawGraphStatic1::ShowWholeMap()
{
	double minx, miny, maxx, maxy, hratio, vratio;

	//捕捉光标，并使光标成沙漏等待光标
	//把最小值给非常大，把最大值给非常小，为了对变量进行初试化
	minx = INT_MAX; miny = INT_MAX; maxx = INT_MIN; maxy = INT_MIN;
	SetCapture();
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	for (int i = 0; i<m_arrModelScopes.GetSize(); i++)
	{
		double minX, minY, maxX, maxY;
		(m_arrModelScopes.GetAt(i)).GetRect(&minX, &minY, &maxX, &maxY);
		if (minx>minX)	minx = minX;
		if (maxx < maxX)	maxx = maxX;
		if (miny > minY)	miny = minY;
		if (maxy < maxY)	maxy = maxY;
	}

	//将光标变为箭头形，放弃捕捉的光标
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	ReleaseCapture();

	hratio = (maxx - minx) / m_rectDrawArea.Width();//横向的比例
	vratio = (maxy - miny) / m_rectDrawArea.Height();//纵向比例
	if (hratio > vratio)
		m_lfRatioForCoorTrans = hratio;  //取较大的一个作为比例尺
	else
		m_lfRatioForCoorTrans = vratio;

	//设置世界坐标的原点
	m_lfMsOrgWorldX = minx;
	m_lfMsOrgWorldY = maxy;

	//设置设备坐标的原点
	m_lfMsOrgLogicX = m_rectDrawArea.CenterPoint().x - (maxx - minx) / (2 * m_lfRatioForCoorTrans);
	m_lfMsOrgLogicY = m_rectDrawArea.CenterPoint().y - (maxy - miny) / (2 * m_lfRatioForCoorTrans);
}

ModelScope* CDrawGraphStatic1::GetNearestMsToPt(CPoint point)
{
	double dis = -1, tem;
	PT_3D pt, pts[4];
	pt.x = point.x;
	pt.y = point.y;

	for (int i = 0; i < m_arrModelScopes.GetSize(); i++)
	{
		ModelScope modelscope1 = m_arrModelScopes.GetAt(i);
		GeoToDev(&modelscope1);
		COPY_2DPT((pts[0]), modelscope1.p1);
		COPY_2DPT((pts[1]), modelscope1.p2);
		COPY_2DPT((pts[2]), modelscope1.p3);
		COPY_2DPT((pts[3]), modelscope1.p4);
		if (GraphAPI::GIsPtInRegion(pt, pts, 4) == 2)
		{
			tem = (pt.x - (pts[0].x + pts[1].x + pts[2].x + pts[3].x) / 4)*(pt.x - (pts[0].x + pts[1].x + pts[2].x + pts[3].x) / 4)
				+ (pt.y - (pts[0].y + pts[1].y + pts[2].y + pts[3].y) / 4)*(pt.y - (pts[0].y + pts[1].y + pts[2].y + pts[3].y) / 4);
			if (dis < 0 || tem < dis)
			{
				dis = tem;
				return &m_arrModelScopes.GetAt(i);
			}
		}
	}

	return NULL;
}

bool CDrawGraphStatic1::IsEqualForMS(const ModelScope &ms, const ModelScope &ms1)
{
	return (fabs(ms.p1.x - ms1.p1.x) < 1e-6&&fabs(ms.p1.y - ms1.p1.y) < 1e-6
		&&fabs(ms.p2.x - ms1.p2.x) < 1e-6&&fabs(ms.p2.y - ms1.p2.y) < 1e-6
		&&fabs(ms.p3.x - ms1.p3.x) < 1e-6&&fabs(ms.p3.y - ms1.p3.y) < 1e-6
		&&fabs(ms.p4.x - ms1.p4.x) < 1e-6&&fabs(ms.p4.y - ms1.p4.y) < 1e-6);
}

void CDrawGraphStatic1::OnMouseMove(UINT nFlags, CPoint point)
{
	PT_3D pt, pts[4];
	pt.x = point.x;
	pt.y = point.y;
	if (m_bIsExistForVecWin)
	{
		ModelScope ms = m_msMain;
		GeoToDev(&ms);
		COPY_2DPT((pts[0]), ms.p1);
		COPY_2DPT((pts[1]), ms.p2);
		COPY_2DPT((pts[2]), ms.p3);
		COPY_2DPT((pts[3]), ms.p4);
		if (GraphAPI::GIsPtInRegion(pt, pts, 4) == 2)
		{
			m_msMove = NULL;
			Invalidate();
			return;
		}
	}

	if (m_rectDrawArea.PtInRect(point))
	{
		ModelScope *ms = GetNearestMsToPt(point);
		if (ms != m_msMove)
		{
			m_msMove = ms;
			Invalidate();
		}
	}
}

void CDrawGraphStatic1::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetKeyState(VK_CONTROL) >= 0)
	{
		for (int i = 0; i < m_arrModelScopes.GetSize(); i++)
		{
			m_arrModelScopes.GetAt(i).flag = 0;
		}
	}

	PT_3D pt, pts[4];
	pt.x = point.x;
	pt.y = point.y;
	if (m_bIsExistForVecWin)
	{
		ModelScope ms = m_msMain;
		GeoToDev(&ms);
		COPY_2DPT((pts[0]), ms.p1);
		COPY_2DPT((pts[1]), ms.p2);
		COPY_2DPT((pts[2]), ms.p3);
		COPY_2DPT((pts[3]), ms.p4);
		if (GraphAPI::GIsPtInRegion(pt, pts, 4) == 2)
		{
			Invalidate();
			return;
		}
	}

	for (int i = 0; i < m_arrModelScopes.GetSize(); i++)
	{
		ModelScope ms = m_arrModelScopes.GetAt(i);
		GeoToDev(&ms);
		COPY_2DPT((pts[0]), ms.p1);
		COPY_2DPT((pts[1]), ms.p2);
		COPY_2DPT((pts[2]), ms.p3);
		COPY_2DPT((pts[3]), ms.p4);
		if (GraphAPI::GIsPtInRegion(pt, pts, 4) == 2)
		{
			if (ms.flag==1)  m_arrModelScopes.GetAt(i).flag = 0;
			else m_arrModelScopes.GetAt(i).flag = 1;
		}
	}
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseMP dialog

CDlgChooseMP::CDlgChooseMP(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChooseMP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChooseMP)
		// NOTE: the ClassWizard will add member initialization here
	m_bNearest = AfxGetApp()->GetProfileInt("FeatureOne", "MapbNearest", TRUE);
	//}}AFX_DATA_INIT
}


void CDlgChooseMP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChooseMP)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Check(pDX, IDC_CHECK1, m_bNearest);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChooseMP, CDialog)
	//{{AFX_MSG_MAP(CDlgChooseMP)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHECK1, OnCheckNearest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseMP message handlers

void CDlgChooseMP::OnOK()
{
	// TODO: Add extra validation here
	for (int i = 0; i < m_staticDrawGraph.m_arrModelScopes.GetSize(); i++)
	{
		ModelScope* ms = &m_staticDrawGraph.m_arrModelScopes.GetAt(i);
		if (ms && ms->flag == 1)
		{
			m_arrSelectedMaps.Add(ms->name);
		}
	}
	
	CDialog::OnOK();
}

int CDlgChooseMP::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_staticDrawGraph.Create(NULL, WS_CHILD | WS_VISIBLE | SS_BLACKRECT | SS_NOTIFY,
		rectDummy, this, 1))
	{
		TRACE0("Failed to create MapGraph\n");
		return -1;      // fail to create
	}

	AdjustLayout();
	return 0;
}

void CDlgChooseMP::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	int cyTlb = 25;
	CRect rectClient, rect;
	GetClientRect(&rectClient);

	m_staticDrawGraph.SetWindowPos(NULL,
		rectClient.left,
		rectClient.top,
		rectClient.Width(),
		rectClient.Height() - cyTlb,
		SWP_NOACTIVATE | SWP_NOZORDER);
	
	//确定画图区域
	m_staticDrawGraph.GetClientRect(&rect);
	CRect temp;
	temp.left = rect.left + 4;
	temp.right = rect.right - 4;
	temp.top = rect.top + 2;
	temp.bottom = rect.bottom - 4;
	m_staticDrawGraph.SetDrawArea(temp);
}

BOOL CDlgChooseMP::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!CUVSModify::IsDBConnected())
	{
		AfxMessageBox(IDS_NOT_CONN_UVS);
		OnCancel();
	}

	OnCheckNearest();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChooseMP::OnCheckNearest()
{
	UpdateData(TRUE);

	AfxGetApp()->WriteProfileInt("FeatureOne", "MapbNearest", m_bNearest);
	m_staticDrawGraph.ClearMSArray();

	Envelope e;
	BOOL bActiveDoc = FALSE;
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (pDoc)
	{
		CDataSourceEx *pDS = pDoc->GetDlgDataSource();
		if (pDS)
		{
			double zmin, zmax;
			PT_3D pts[4];
			pDS->GetBound(pts, &zmin, &zmax);
			m_staticDrawGraph.SetCurrentVecWin(pts[0], pts[1], pts[2], pts[3]);
			e.CreateFromPts(pts, 4);
			e.Inflate(10, 10, 0);
			bActiveDoc = TRUE;
		}
	}

	CStringArray mapsheet_list;
	CArray<double, double> bounds;
	CUVSModify::EnumerateMapsheet(mapsheet_list, bounds);
	for (int i = 0; i<mapsheet_list.GetSize(); ++i)
	{
		PT_2D pts1[4];
		Envelope e1;
		pts1[0].x = bounds[i * 12];        pts1[0].y = bounds[i * 12 + 1];
		pts1[1].x = bounds[i * 12 + 3]; pts1[1].y = bounds[i * 12 + 4];
		pts1[2].x = bounds[i * 12 + 6]; pts1[2].y = bounds[i * 12 + 7];
		pts1[3].x = bounds[i * 12 + 9]; pts1[3].y = bounds[i * 12 + 10];
		e1.CreateFromPts(pts1, 4);
		if(m_bNearest && bActiveDoc)
		{
			if(e.bIntersect(&e1))
			{
				m_staticDrawGraph.AddModelScope(pts1[0], pts1[1], pts1[2], pts1[3], mapsheet_list[i]);
			}
		}
		else
		{
			m_staticDrawGraph.AddModelScope(pts1[0], pts1[1], pts1[2], pts1[3], mapsheet_list[i]);
		}
	}
	m_staticDrawGraph.ShowWholeMap();
	m_staticDrawGraph.Invalidate();
}