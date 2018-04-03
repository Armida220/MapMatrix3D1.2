// OsgbVecView.cpp : implementation file
//

#include "stdafx.h"
#include "OsgbDoc.h"
#include "OsgbVecView.h"
#include "OsgbView.h"
#include "ChildFrm.h"
#include "qcomm.h"
#include "ExMessage.h"
#include "osgdef.h"
#include "ICallBack.h"
#include "DlgCommand.h"
#include "glew.h"
#include "GrBuffer.h"
#include "OverrideRealtimeVectLayer.h"
#include "MsgSender.h"

using namespace std;
using namespace osgCall;


/**
* @brief bIsCatch 全局变量，因为osgbvecview不能定义成员变量，所有这里使用全局变量
*/
bool bIsCatch = false;

/**
* @brief minZoomScale 最小缩放比例
*/
const double minZoomScale = 0.016;

/**
* @brief bShowImg 显示影像
*/
bool bShowImg = false;

/**
* @brief registerCmd 注册命令
* @param pWorker 命令的工作者
* @param id  命令的id号
*/
template<typename T>
void registerCmd(CWorker* pWorker, int id)
{
	pWorker->RegisterCommand(id, T::Create);
}

// COsgbVecView

void COsgbVecView::leftClk(PT_3D *pt, double* screen2world)
{
	//如果捕捉成功的话，使用鼠标移动获得的捕捉点，否则使用三维场景过来的点
	if (!bIsCatch)
	{
		m_gCurPoint = *pt;
	}

	if (bShowImg)
	{
		COsgbView* pView = getOsgbView();

		if (pView != nullptr)
		{
			pView->SendMessage(WM_SHOW_IMG, (WPARAM)&m_gCurPoint, 1);
		}

	}

	PT_4D cltpt;
	COsgbVecView::GroundToClient(pt, &cltpt);

	CPoint cp(cltpt.x, cltpt.y);

	UINT flag = MK_LBUTTON;
	if (GetKeyState(VK_CONTROL) < 0)
		flag |= MK_CONTROL;
	if (GetKeyState(VK_SHIFT) < 0)
		flag |= MK_SHIFT;
	CVectorView_new::OnLButtonDown(flag, cp);

//	DriveToXyz(pt, TRUE);
	UpdateConstDragLine();
}

void COsgbVecView::leftDBClk(PT_3D *pt)
{
	if (!bIsCatch)
	{
		m_gCurPoint = *pt;
	}
	PT_4D cltpt;
	COsgbVecView::GroundToClient(pt, &cltpt);

	CPoint cp(cltpt.x, cltpt.y);

	UINT flag = MK_LBUTTON;
	if (GetKeyState(VK_CONTROL) < 0)
		flag |= MK_CONTROL;
	if (GetKeyState(VK_SHIFT) < 0)
		flag |= MK_SHIFT;
	CVectorView_new::OnLButtonDblClk(flag, cp);
}

void COsgbVecView::rightClk(PT_3D *pt)
{
	if (!bIsCatch)
	{
		m_gCurPoint = *pt;
	}

	PT_4D cltpt;
	COsgbVecView::GroundToClient(pt, &cltpt);
	CPoint cp(cltpt.x, cltpt.y);
	OnRButtonDown(2, cp);

	//DriveToXyz(pt, TRUE);
	UpdateConstDragLine();
}

void COsgbVecView::mouseMove(PT_3D *pt)
{
	m_gCurPoint = *pt;

	PT_4D cltpt;
	COsgbVecView::GroundToClient(pt, &cltpt);
	CPoint cp(cltpt.x, cltpt.y);
	OnMouseMove(1, cp);

}

void COsgbVecView::changeCurPt(PT_3D *pt)
{
	//
	m_gCurPoint = *pt;

	PT_4D cltpt;
	COsgbVecView::GroundToClient(pt, &cltpt);

	CPoint cp(cltpt.x, cltpt.y);
	CVectorView_new::OnLButtonDown(1, cp);

	UpdateConstDragLine();
}

void COsgbVecView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CVectorView_new::OnUpdate(pSender, lHint, pHint);
}

// COsgbVecView drawing

BEGIN_MESSAGE_MAP(COsgbVecView, CVectorView_new)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

bool COsgbVecView::isHasOsgView()
{
	bool bIsFind = false;

	POSITION pos = m_pDocument->GetFirstViewPosition();
	while (pos)
	{
		CView* pOsgView = m_pDocument->GetNextView(pos);
		if (pOsgView && pOsgView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			bIsFind = true;
		}
	}

	return bIsFind;
}

std::vector<double> COsgbVecView::getLinePick(const double x, const double y)
{
	std::vector<double> linePick;
	POSITION pos = m_pDocument->GetFirstViewPosition();
	while (pos)
	{
		CView* pView = m_pDocument->GetNextView(pos);
		if (pView && pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			COsgbView* pOsgbView = dynamic_cast<COsgbView*> (pView);
			linePick = pOsgbView->getLinePick(x, y);
		}
	}

	return linePick;
}

void COsgbVecView::OnMouseMove(UINT nFlags, CPoint point)
{
	mouseMv(nFlags, point);

	CDlgDoc *pDoc = GetDocument();
	pDoc->UpdateAllOtherViews(hc_SetCrossPos, (CObject*)&m_gCurPoint);
}

void COsgbVecView::mouseMv(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	static TRACKMOUSEEVENT Track;
	Track.cbSize = sizeof(TRACKMOUSEEVENT);
	Track.dwFlags = TME_LEAVE;
	Track.hwndTrack = m_hWnd;
	Track.dwHoverTime = HOVER_DEFAULT;
	_TrackMouseEvent(&Track);

	//SetFocus();

	//	static CPoint ptOld(-1,-1);

	//曾进翀 去掉过滤坏信号， 这是因为这部分会导致三维视图动线不能及时刷新
	//过滤鼠标的坏信号(原因很复杂，可能是硬件也可能是其他软件造成的)
	//if (m_ptLastMouse == point)
	//{
	//	return;
	//}

	if (m_nInnerCmd == ID_VIEW_ROTATE);
	// 	{
	// 		if( m_bViewRotate )
	// 		{
	// 			CRect rcClient;
	// 			GetClientRect(&rcClient);
	// 			CPoint cpt = rcClient.CenterPoint();
	// 			
	// 			//旋转矩形框
	// 			double ang = m_lfAngle + GGetAngle(cpt.x,rcClient.bottom-cpt.y,point.x,rcClient.bottom-point.y)	- 
	// 				GGetAngle(cpt.x,rcClient.bottom-cpt.y,m_ptViewRotateStart.x,rcClient.bottom-m_ptViewRotateStart.y);
	// 			
	// 			GrBuffer buf;
	// 			buf.EnableWidFromGrd(FALSE);
	// 			buf.SetWidth(2);
	// 			DrawViewRotate(&buf,cpt,cpt.x<cpt.y?cpt.x:cpt.y,-ang);
	// 			
	// 			SetVariantDragLine(&buf,FALSE);
	// 			UpdateVariantDragLine();
	// 		}
	// 		CSceneViewEx::OnMouseMove(nFlags,point);
	// 		return;
	// 	}
	else if (m_nInnerCmd == ID_TOOL_MANUALLOADVECT)
	{
		if (m_bStartLoadVect)
		{
			GrBuffer buf;

			PT_4D pt;
			PT_3D pt3d, ptt;
			pt.x = point.x; pt.y = point.y;
			ClientToGround(&pt, &pt3d);
			buf.BeginLineString(0, 0);

			buf.MoveTo(&m_ptStartLoadVect);
			ptt.x = pt3d.x, ptt.y = m_ptStartLoadVect.y, ptt.z = 0;
			buf.LineTo(&ptt);
			buf.LineTo(&pt3d);
			ptt.x = m_ptStartLoadVect.x, ptt.y = pt3d.y, ptt.z = 0;
			buf.LineTo(&ptt);
			buf.LineTo(&m_ptStartLoadVect);

			buf.End();
			SetVariantDragLine(&buf);
			UpdateVariantDragLine();
			FinishUpdateDrawing();
		}
		CSceneViewEx::OnMouseMove(nFlags, point);
		return;
	}
	else if (m_nInnerCmd != 0)
	{
		CSceneViewEx::OnMouseMove(nFlags, point);
		return;
	}

	if (IsDrawStreamline())
	{
		CInteractiveView::OnMouseMove(nFlags, point);

		return;
	}

	if (m_bAccuLeave)
	{
		//避免绘制遮挡测标的窗口的问题
		// 		EnableCrossVisible(FALSE);
		// 		ResetCursorDrawing();
		// 		EnableCrossVisible(TRUE);
		m_bAccuLeave = FALSE;
		if (GetDocument()->m_accuBox.IsOpen())
		{
			GetDocument()->m_accuBox.SetReDrawFlag(TRUE);
		}
	}

	SetCrossPos(point.x + 0.5, point.y + 0.5, point.x + 0.5);
	UpdateCross();

	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;

	PT_4D pt;
	COsgbVecView::GroundToClient(&m_gCurPoint, &pt);

	//曾进翀, 解决动线无法跟上的问题
	pt.x = point.x + 0.5; pt.y = point.y + 0.5;
	//ClientToGround(&pt, &m_gCurPoint);

	{
		PT_4D pt2 = pt;
		PT_3D testpt;
		ClientToGround(&pt2, &testpt);
		COsgbVecView::GroundToClient(&testpt, &pt2);
	}

	if (!m_strDemPath.IsEmpty())
		m_gCurPoint.z = m_dem.GetZ(m_gCurPoint.x, m_gCurPoint.y);

	m_ptLastMouse = point;

	// 	if( IsDrawStreamline() )
	// 	{
	// 		CInteractiveView::OnMouseMove(nFlags,point);
	// 	
	// 		return;
	// 	}

	if (m_bMouseDriveVector)
	{
		CDlgDoc *pDoc = GetDocument();
		pDoc->UpdateAllOtherViews(hc_SetCrossPos, (CObject*)&m_gCurPoint);
	}

	//CBaseView::OnMouseMove(nFlags, point);

	baseViewMv(nFlags, point);
}

void COsgbVecView::baseViewMv(UINT nFlags, CPoint point)
{
	COsgbView* osgbView = getOsgbView();

	if (osgbView == nullptr)
	{
		return;
	}

	//如果有内部命令(比如拖动影像)正在操作，应该优先执行内部命令
	if (m_nInnerCmd != 0)
	{
		CSceneViewEx::OnMouseMove(nFlags, point);
		return;
	}

	CDlgDoc *pDoc = GetDocument();
	pDoc->SetDataQueryType(m_bSymbolize);

	if (!pDoc->GetDlgDataSource())return;

	PT_3D pt3d;
	PT_4D cltpt;
	pt3d = m_gCurPoint;

	pDoc->m_snap.m_bWithSymbol = m_bSymbolize;
	//获取上一个点
	PT_3D lastPt;
	PT_3D *pLastPt = NULL;
	int lastNum = 0;
	CCommand* pCurCmd = pDoc->GetCurrentCommand();
	DrawingInfo Info;
	if (pCurCmd/*&&pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand))*/)
	{
		Info = (/*(CDrawCommand*)*/pCurCmd)->GetCurDrawingInfo();
		lastNum = Info.GetLastPts(pLastPt);
	}

	double r = GetDocument()->m_snap.GetSnapRadius() * 1 / getScale() * 1.5;
	Envelope e;
	e.CreateFromPtAndRadius(pt3d, r);

	static CCoordSys *pCS = new CCoordSys;
	//判断是否咬中
	//首先判断自身对象
	if (pDoc->m_snap.bOpen() && pDoc->m_snap.bSnapSelf())
	{
		pDoc->m_snap.PreSnap(e, pLastPt, lastNum, &m_gCurPoint, pCS, Info.GetDrawGeo());

	}

	if (pDoc->m_snap.bOpen() && pDoc->m_snap.DoSnap(e, pLastPt, lastNum, &m_gCurPoint, pCS))
	{
		m_itemCurSnap = pDoc->m_snap.GetFirstSnapResult();

		if (m_itemCurSnap.IsValid())
		{
			UpdateSnapDrawing1();
			m_gCurPoint = m_itemCurSnap.pt;
		}
		else
		{
			ClearSnapDrawing1();
		}

	}
	//没有咬中，再看是否可以利用accubox
	else
	{
		ClearSnapDrawing1();
	}

	if (pDoc->m_accuBox.IsOpen())
	{
		pt3d = m_gCurPoint;
		if (!pDoc->m_accuBox.IsActive())pDoc->m_accuBox.SetReDrawFlag(TRUE);

		//将咬合的最近点和精确绘图的轴线吸附或者方向锁定求交
		BOOL bWithSnap = FALSE;
		ACCU_DATA data;
		pDoc->m_accuBox.OnUpdateData(3, (LPARAM)&data);
		int  nDir = 0, status = pDoc->m_accuBox.GetStatus();
		pDoc->m_accuBox.GetLockedDirs(nDir);

		if (pDoc->m_accuBox.IsWaitDir())
		{

			Envelope et = EnvelopeFromPtAndRadius(pt3d, r);
			//PT_3D searchPt;
			//GetCoordWnd().m_pSearchCS->COsgbVecView::GroundToClient(&pt3d, &searchPt);
			CFeature *pFtr = pDoc->GetDataQuery()->FindNearestObject(pt3d, 0, pCS);

			PT_3D ptline0(0, 0, 0), ptline1(0, 0, 0);
			if (pFtr)
			{
				if (!pFtr->GetGeometry()->FindNearestBaseLine(pt3d, et, pCS, &ptline0, &ptline1, NULL))
				{
					ptline0.x = ptline0.y = ptline0.z = 0;
					ptline1.x = ptline1.y = ptline1.z = 0;
				}
			}

			pDoc->m_accuBox.MoveDir(ptline0, ptline1);
		}
		else
		{
			//将捕捉到的最近点对应的基线线段作为延长线方向，临时添加到精确绘图中
			if (pDoc->m_snap.bOpen() && m_itemCurSnap.nSnapMode == CSnap::modeNearPoint &&
				nDir < 2)
			{
				Envelope et = EnvelopeFromPtAndRadius(pt3d, r);

				PT_3D ptline0(0, 0, 0), ptline1(0, 0, 0);
				if (m_itemCurSnap.pFtr)
				{
					if (m_itemCurSnap.pFtr->GetGeometry()->FindNearestBaseLine(pt3d, et, pCS, &ptline0, &ptline1, NULL))
					{
						pDoc->m_accuBox.LockDir(ACCU_DIR::extension);
						pDoc->m_accuBox.MoveDir(ptline0, ptline1);
						bWithSnap = TRUE;
					}
				}
			}
		}

		pDoc->m_accuBox.MoveDataPt(GetCoordWnd(), pt3d);

		//如果采用捕捉的最近点处理的结果与不采用捕捉的最近点处理的结果差别超出了捕捉范围，
		//就不要采用捕捉的最近点
		if (bWithSnap)
		{
			//去除当前临时加的延长线方向
			pDoc->m_accuBox.UnlockDir(-1);

			//备份当前的状态数据
			ACCU_DATA data1;
			pDoc->m_accuBox.OnUpdateData(3, (LPARAM)&data1);

			//恢复到以前的状态数据
			pDoc->m_accuBox.OnUpdateData(0, (LPARAM)&data);

			//按照不采用捕捉的最近点处理，得到结果 pt3d2
			PT_3D pt3d2 = m_gCurPoint;
			pDoc->m_accuBox.MoveDataPt(GetCoordWnd(), pt3d2);

			//比较前后差距
			double dis = sqrt((pt3d.x - pt3d2.x)*(pt3d.x - pt3d2.x) +
				(pt3d.y - pt3d2.y)*(pt3d.y - pt3d2.y));

			//超出了，获得不采用捕捉的最近点处理的结果
			if (dis > (e.m_xh - e.m_xl) / 2)
				pt3d = pt3d2;
			//没有超出，获得采用捕捉的最近点处理的结果，恢复到当前状态数据
			else
			{
				pDoc->m_accuBox.OnUpdateData(0, (LPARAM)&data1);
			}
		}

		/*COsgbVecView::GroundToClient(&pt3d, &cltpt);*/
		UpdateSnapDrawing1();
		if (m_bAccuLeave)
		{
			ClearSnapDrawing1();
		}
		// 		else
		// 		{
		// 			ClearAccuDrawing();
		// 			OnPreUpdateRegion(NULL,UPDATEREGION_ACCUDRG);
		// 			EraseAccuDrg();
		// 			DrawAccuDrg();
		// 			OnPostUpdateRegion(NULL,UPDATEREGION_ACCUDRG);
		// 		}

		m_gCurPoint = pt3d;
	}

	//显示当前测标点的坐标
	AfxGetMainWnd()->SendMessage(FCCM_SHOW_XYZ, 0, LPARAM(&m_gCurPoint));

	interMouseMove(nFlags, point);
}

void COsgbVecView::interMouseMove(UINT nFlags, CPoint point)
{
	if (m_nInnerCmd != 0)
	{
		CSceneViewEx::OnMouseMove(nFlags, point);
		return;
	}

	CDlgDoc* dlgDoc = GetDocument();
	COsgbDoc* osgDoc = DYNAMIC_DOWNCAST(COsgbDoc, dlgDoc);

	if (osgDoc == nullptr)
	{
		return;
	}

	osgDoc->SetCoordWnd(GetCoordWnd());
	osgDoc->subMouseMove(m_gCurPoint, nFlags);
	UpdateVariantDragLine();
	UpdateConstDragLine();
	FinishUpdateDrawing();
	CSceneViewEx::OnMouseMove(nFlags, point);
}

void COsgbVecView::UpdateSnapDrawing1()
{
	CVectorView_new::UpdateSnapDrawing();

	bIsCatch = true;
}

void COsgbVecView::ClearSnapDrawing1()
{
	CVectorView_new::ClearSnapDrawing();

	bIsCatch = false;
}

COsgbView* COsgbVecView::getInstanceOfOsgbView()
{
	static COsgbView* pOsgbView = getOsgbView();
	return pOsgbView;
}

COsgbView* COsgbVecView::getOsgbView()
{
	POSITION pos = m_pDocument->GetFirstViewPosition();
	while (pos)
	{
		CView* pView = m_pDocument->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			COsgbView* pOsgbView = static_cast<COsgbView*>(pView);
			return pOsgbView;
		}
	}

	return nullptr;
}

PT_3D COsgbVecView::getGroundPt()
{
	return m_gCurPoint;
}

GrBuffer* COsgbVecView::AddObj(CFeature* pFtr)
{
	CDlgDoc* dlg = GetDocument();
	CDlgDataSource* pDS = dlg->GetDlgDataSource();
	CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);

	if (!pLayer)
		return NULL;

	BOOL bEnable;
	COLORREF monoClr;
	pDS->GetMonoColor(&bEnable, &monoClr);
	GrBuffer* buf = new GrBuffer;
	
	Callback_DrawFeature(pDS, pLayer, pFtr, 1, buf);
	return buf;
// 	if (CanSymbolized(pFtr) && pLayer->IsSymbolized())
// 	{
// 		if (pDS)
// 		{
// 			pDS->DrawFeature(pFtr, buf, pFtr->GetGeometry()->IsSymbolizeText(), m_gsa.lfAngle);
// 		}
// 	}
// 	else
// 	{
// 		if (pDS)
// 		{
// 			pFtr->Draw(buf, pDS->GetSymbolDrawScale());
// 
// 			if (bEnable)
// 			{
// 				buf->SetAllColor(monoClr);
// 			}
// 			else if (pFtr->GetGeometry()->GetColor() == COLOUR_BYLAYER || m_bSymbolscolorByLayer)
// 			{
// 				buf->SetAllColor(pLayer->GetColor());
// 			}
// 			if (pFtr->GetGeometry()->GetClassType() == CLS_GEOTEXT)
// 			{
// 				//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
// 			}
// 		}
// 	}
// 
// 	return buf;
}

COLORREF COsgbVecView::getDragLineClr()
{
	return m_clrDragLine;
}

bool COsgbVecView::getIsSymbolize()
{
	return m_bSymbolize;
}

vector<GrBuffer*> COsgbVecView::getRenderBufs()
{
	COverrideRealtimeVectLayer* vecLayer = DYNAMIC_DOWNCAST(COverrideRealtimeVectLayer, &m_vectLayL2);
	std::vector<GrBuffer*> bufs = vecLayer->getRenderBufs();
	return bufs;
}

void COsgbVecView::DriveToXyz(PT_3D *pt, BOOL bMoveImg)
{
	PT_4D cltpt, new_gpt, new_cltpt;
	COsgbVecView::GroundToClient(pt, &cltpt);

	CRect rect;
	GetClientRect(&rect);
	int w = rect.Width(), h = rect.Height();

	if (bMoveImg || cltpt.x<w / 10 || cltpt.x>w * 9 / 10 || cltpt.y<h / 10 || cltpt.y>h * 9 / 10)
	{
//		SetCrossPos(w*0.5 + 0.5, h*0.5 + 0.5, w*0.5 + 0.5);
		Scroll(w*0.5 - cltpt.x, h*0.5 - cltpt.y, 0);

//		UpdateCross();
	}
	else
	{
//		SetCrossPos(cltpt.x, cltpt.y, cltpt.x);
		CString str1;
		str1.Format("%f,%f,%f", cltpt.x, cltpt.y, cltpt.x);

		UpdateCross();
		FinishUpdateDrawing();
	}

// 	m_gCurPoint = *pt;
// 	m_gCur3DMousePoint = m_gCurPoint;
	GetDocument()->SetCoordWnd(GetCoordWnd());

	//显示当前测标点的坐标
//	AfxGetMainWnd()->SendMessage(FCCM_SHOW_XYZ, 0, LPARAM(&m_gCurPoint));
}

void COsgbVecView::adjustScale(double x, double y)
{
	COsgbView* pView = getOsgbView();

	if (pView)
	{
		double scale = pView->getScale();
		double vecScale = getScale();
		
		//相差比例
		double ratio = scale / vecScale;

		//调整后二维视图的比例
		double adjustScale = m_fZoomRate * ratio / 2.0;
		
		//进行比例选择
		zoomToScale(x, y, adjustScale);
	}
}

void COsgbVecView::zoomToScale(double x, double y, double scale)
{
	double zoom = GetZoomRate();
	if (scale < minZoomScale) return;
	if (scale < zoom)
	{
		double lz = 0;
		while (scale < zoom)
		{
			CSceneView::ZoomNext(CPoint(x, y), 0);
			zoom = GetZoomRate();

			if (zoom < minZoomScale || (lz == zoom))
			{
				break;
			}
			lz = zoom;
		}

		//回去一下
		CSceneView::ZoomNext(CPoint(x, y), 1);
	}
	else
	{
		double lz = 0;
		while (scale > zoom)
		{
			CSceneView::ZoomNext(CPoint(x, y), 1);
			zoom = GetZoomRate();
			if (lz == zoom)
			{
				break;
			}
			lz = zoom;
		}

		//回去一下
		CSceneView::ZoomNext(CPoint(x, y), 0);
	}

	PT_3D pt(x, y, 0);
	DriveToXyz(&pt, TRUE);

	Invalidate(TRUE);

}

double COsgbVecView::getScale()
{
	CRect rect;
	GetClientRect(rect);

	PT_4D left(0, 0, 0);
	PT_3D grdleft;
	ClientToGround(&left, &grdleft);

	PT_4D right(rect.Width(), 0, 0);
	PT_3D grdright;
	ClientToGround(&right, &grdright);

	//二维视图地面宽度
	double grdWidth = grdright.x - grdleft.x;

	//二维视图windows宽度
	double winWidth = rect.Width();

	//二维视图比例
	double vecScale = winWidth / grdWidth;

	return vecScale;
}

bool COsgbVecView::changeShowImg()
{
	bShowImg = !bShowImg;

	return bShowImg;
}

bool COsgbVecView::isShowImg()
{
	return bShowImg;
}

bool COsgbVecView::isNoHatch()
{
	return m_pContext->m_bNoHatch;
}

void COsgbVecView::GroundToClient(PT_3D *pt0, PT_4D *pt1)
{
	CVectorView_new::GroundToClient(pt0, pt1);
}

void COsgbVecView::updataBound(double l, double r, double t, double b)
{
	//Envelope evlp;

	PT_3D pt[4] = {
		PT_3D(l, t, 0),
		PT_3D(l, b, 0),
		PT_3D(r, b, 0),
		PT_3D(r, t, 0)
	};
	//evlp.CreateFromPts(pt, 4);
	GetWorker()->UpdateView(0, hc_SetMapArea, (LPARAM)pt);

}
