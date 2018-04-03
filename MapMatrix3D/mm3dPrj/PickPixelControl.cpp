// PickPixelContral.cpp : 实现文件
//

#include "stdafx.h"
#include "PickPixelControl.h"
#include "afxdialogex.h"
// CPickPixelContral 对话框

IMPLEMENT_DYNAMIC(CPickPixelControl, CDialogEx)

CPickPixelControl::CPickPixelControl(CWnd* pParent /*=NULL*/)
: CDialogEx(CPickPixelControl::IDD, pParent),
m_L(0, 0, 50, 100, 0),
m_R(50, 0, 50, 100, 0),
bLoading(true)
{
	EnableD2DSupport();
}

CPickPixelControl::~CPickPixelControl()
{
}

void CPickPixelControl::setImage(std::vector<COri>& vecOri)
{
	m_manage.setVecOri(vecOri);
	m_L.SetIdx(0);
	m_R.SetIdx(1);
}

void CPickPixelControl::setNotifyFun(const notifyFun & fun)
{
	m_L.SetNotifyFun(fun);
	m_R.SetNotifyFun(fun);
}

void CPickPixelControl::setCheckToUnCheck(const callbackFun & fun)
{
	m_L.setCheckToUncheckByClickFun(fun);
	m_R.setCheckToUncheckByClickFun(fun);
}

bool CPickPixelControl::isSetNotifyFun()
{
	return m_L.IsSetNotifyFun() && m_R.IsSetNotifyFun();
}

void CPickPixelControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPickPixelControl, CDialogEx)
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CPickPixelControl::OnDraw2d)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CPickPixelContral 消息处理程序
LRESULT CPickPixelControl::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);

	CRect rect;
	GetClientRect(rect);
	m_L.Paint(pRenderTarget);
	m_R.Paint(pRenderTarget);
	return 0;
}




BOOL CPickPixelControl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_manage.Init(GetRenderTarget());

	m_L.SetManager(&m_manage);
	m_L.SetIdx(-1);
	m_R.SetManager(&m_manage);
	m_R.SetIdx(-1);
	SetTimer(1, 33, NULL);
	return TRUE; 
	
}


void CPickPixelControl::OnTimer(UINT_PTR nIDEvent)
{
	
	Invalidate();
	CDialogEx::OnTimer(nIDEvent);
}



void CPickPixelControl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(rect);
	needRepaint = true;

	m_L.ResizeByParentRect(rect);
	m_R.ResizeByParentRect(rect);
	// TODO:  在此处添加消息处理程序代码
}


void CPickPixelControl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_L.MouseMove(point))
		needRepaint = true;
	if (m_R.MouseMove(point))
		needRepaint = true;
	if (nFlags & MK_MBUTTON)
	{
		double dx = lastPoint.x - point.x;
		double dy = lastPoint.y - point.y;
		m_L.Move(dx, dy);
		m_R.Move(dx, dy);
	}
	lastPoint = point;
	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CPickPixelControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CPoint npt = pt;
	ScreenToClient(&npt);
	m_L.Zoom(zDelta > 0, npt);
	m_R.Zoom(zDelta > 0, npt);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CPickPixelControl::OnLButtonDown(UINT nFlags, CPoint point)
{


	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPickPixelControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_L.Click(point);
	m_R.Click(point);
	CDialogEx::OnLButtonUp(nFlags, point);
}
