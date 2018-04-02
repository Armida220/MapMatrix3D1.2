// StereoView.h: interface for the CStereoView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOVIEW_H__83CBCC6D_5739_411D_8D71_47C2C76F2F9C__INCLUDED_)
#define AFX_STEREOVIEW_H__83CBCC6D_5739_411D_8D71_47C2C76F2F9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseView.h"
#include "DlgStretchImg.h "
#include "GeoBuilderPrj2.h "
#include "CoordCenter.h "
#include "RasterLayer.h"

// class CStereoView : public CBaseView  
// {
// public:
// 	CStereoView();
// 	virtual ~CStereoView();
// 
// };

#define TIMERID_AUTOMOVEIMAGE		2000

typedef struct
{
	PT_3D start;
	PT_3D end;
	double ang;
} LockLine; 

#define STEREOVIEW_INPUT_SYSMOUSE		1
#define STEREOVIEW_INPUT_3DMOUSE		2

struct ImgSaveData
{
public:
	ImgSaveData():rcSaveRectL(0,0,0,0),rcSaveRectR(0,0,0,0),
		nSaveRectType(0),pRectLData(0),pRectRData(0)
	{
	}
	CRect rcSaveRectL;
	CRect rcSaveRectR;
	int	  nSaveRectType;
	BYTE *pRectLData;
	BYTE *pRectRData;
	
	CDC dcMemRect;
	CBitmap bmpRect;
};

/*
 *	2008.01.17 将立体新方案（采用纹理方式实现立体漫游）添加进来
 *  为了减少对其他部分的影响，立体新方案和原来的方案是合并起来的，
 *  通过 m_bTextMode 来区分当前采用的方式；
 *  与原来的显示机制相比，纹理模式的立体视图有如下不同：
 *  1、影像的显示由纹理来完成；
 *  2、永久矢量的显示，采用 DrawCustomLayer 机制；
 *  2、永久矢量显示用的 CXVVectorLayer 对象被 CXVElementBank 对象替代；
 *  
 *  测标的显示采用两种方式，手轮脚盘运动的时候，通过DrawCustomLayer方式，
 *  鼠标运动的时候，仍然采用以前的方式；这两种方式通过 m_bMovableCursorInTextMode 区分；
 *  用户使用鼠标的时候，此值为TRUE，用户使用手轮脚盘的时候，此值为FALSE；
 */

class CStereoView : public CBaseView
{
	DECLARE_DYNCREATE(CStereoView)
protected:

	friend class CDlgDoc;
	//当前立体的像对文件路径
	CString m_strMdlImgFile[2];

	//当前立体的像对ID
	CString	m_strStereID;
 
	CRasterLayer m_ImgLayLeft;
	CRasterLayer m_ImgLayRight;

	CVariantDragLayer m_snapTipLayL;
	CVariantDragLayer m_snapTipLayR;

	//当前的三维鼠标测标点的大地坐标，
	//只在连续地使用三维鼠标作基于大地坐标的驱动时才用
	PT_3D m_gCur3DMousePoint;

	//当前的系统鼠标测标点的大地坐标，引入这个变量是要保存系统鼠标测标点，
	//因为snap可能会影响作业的测标点(m_gCurPoint)，
	PT_3D m_gCurMousePoint;

	//当前的三维鼠标测标点的客户坐标和系统鼠标测标点的客户坐标，
	//引入这个两个变量是为了让系统测标和三维鼠标分离，又不互相干扰；
	PT_4D m_cCur3DMousePoint;
	PT_4D m_cCurMousePoint;

	//中心测标模式下的测标定位点
	CPoint m_ptCenterPoint;
	//是否使用上述定位点作为测标定位点，如果否，则使用当前客户区中心作为定位点
	BOOL m_bUseCenterPoint;

	//当前使用中的测标输入类型; 
	//STEREOVIEW_INPUT_SYSMOUSE, 系统鼠标; 
	//STEREOVIEW_INPUT_3DMOUSE, 三维鼠标
	int  m_nInputType;

	//当前的测标模式
	DWORD m_dwModeCoord;

	//是否能正确地坐标转换
	BOOL m_bAutoMatch;
	BOOL m_bLoadConvert;
	int  m_nImageType;

	BOOL m_bLoadModel;
	BOOL m_bRealTimeEpipolar;

	//是否忽略垂直方向视差
	BOOL m_bIgnoreVParallax;

	BOOL m_bStereoMidButton;

	CoreObject	m_coreObj;
	//坐标转换器
	CCoordCenter *m_pConvert;

	//影像到大地坐标的仿射变换系数
	double m_trA[3],m_trB[3];
	double m_trZ;

	//高程步距
	double m_lfHeiStep;

	//是否显示矢量叠加层
	BOOL m_bViewVector;

	//越界警报
	BOOL m_bOutAlert;

	//右键弹出菜单的窗口句柄
	HWND m_hPopupWnd;
	
	//是否在测标移动的时候联动矢量
	BOOL m_bMoveDriveVector;
	
	//是否在鼠标移动的时候联动矢量
	BOOL m_bMouseDriveVector;

	//是否属于来自系统的移动消息
	BOOL m_bMoveFromSys;
	
	//是否在3D鼠标测图的时候废弃鼠标的消息
	BOOL m_bDisableMouseIn3DDraw;
	BOOL m_b3DDraw;
	CPoint m_ptLastMouse;

	//是否第一次加载立体模型；切换立体模型的时候，此值为 FALSE
	BOOL m_bFirstLoadModel;

	//影像旋转设置对话框
	CDlgStretchImg m_wndStretchDlg;

	BOOL m_bDriveOnlyCursor;

	BOOL m_bLockXY;

	// 支持在三维鼠标测图时锁定角度
	BOOL m_bLockAngle;
	LockLine m_LockLine;

	int m_nDragLineWid;

	//TURE -> 滚轮缩放立体
	BOOL m_bMouseWheelZoom;

	BOOL m_bFirstCheckStere;
protected:
	// 是否显示内节点
	GrBuffer *m_pBufVariantDrag;
	GrBuffer *m_pBufConstDrag;
//	BOOL m_bShowKeyPoint;

	BOOL m_bMouseNoHeiMode;

protected:
	//直接贴图的影像象素备份（如立体咬合的显示，立体咬合提示的显示）

	BOOL	 m_bUseStereoEvlp;
	CArray<PT_3D,PT_3D>    m_arrStereoBoundPts_forDisplayTrim;

	//影像变换导致的变换矩阵
	double m_lfStretchLMatrix[9], m_lfStretchRMatrix[9];

	//变换逆矩阵
	double m_lfStretchRLMatrix[9], m_lfStretchRRMatrix[9];

	BOOL    m_bAutoAdjustCoordMode;

	// 高程锁定模式下是否正在使用脚盘调整高程
	BOOL m_bDeviceModifyZProcessing;
	BOOL m_bModifyheightBy3DMouse;

	// 是否以当前测标为缩放中心
	BOOL m_bZoomWithCurrentMouse;

	// 扫描分辨率
	double m_lfScanSize;

	float m_fWheelSpeed;

public:	
	CCoordWnd GetCoordWnd();
	void SwitchModelForRefresh();

	CStereoView();
	virtual ~CStereoView();
	void DriveToXyz(PT_3D *pt, int coord_mode, BOOL bMoveImg = TRUE);
	virtual void StereoMove();

	void GetStereoBound(PT_3D* pts);
	
	PT_3D GetCrossPoint();
	CString	GetStereID();
	int GetImageType();
	BOOL GetRealTimeEpipolar();
protected:
	
	void ScrollForDrawBitmap(double dx,double dy,double dz = 0);
	virtual BOOL InitBmpLayer();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	
	virtual float ZoomSelect(float wantZoom);
	virtual void OnPreInnerCmd(UINT nInnerCmd);
	virtual void OnPostInnerCmd();
	
	virtual void ClientToImage(PT_4D *pt0, PT_4D *pt1);
	virtual void ImageToClient(PT_4D *pt0, PT_4D *pt1);
	virtual void GroundToImage(PT_3D *pt0, PT_4D *pt1);
	virtual void ImageToGround(PT_4D *pt0, PT_3D *pt1);

	virtual void GroundToVectorLay(PT_3D *pt0, PT_4D *pt1);

	void RotateCoordinate(double &x, double &y, const double *m);
//	BOOL Convert(Coordinate Input, Coordinate &Output);
	

	virtual void DrawAccuBox();
	virtual void EraseAccuBox();
	virtual void DrawAccuDrg();
	virtual void EraseAccuDrg();

	virtual void AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void AddConstDragLine(const GrBuffer *pBuf);

	virtual void UpdateConstDragLine();
	virtual void UpdateAccuDrawing();

	virtual void ClearAccuDrawing();
	virtual void ClearDragLine();

	virtual void InitUpdateLayersDepth();

	virtual void UpdateGridDrawing();

	virtual void AddObjtoVectLay(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround = TRUE, CVectorLayer *pLL = NULL, CVectorLayer *pLR = NULL);
	virtual void AddObjtoMarkVectLay(CFeature *pFtr);

	float GetActualImageParallax(float x, float y, int dir=0);
	BOOL LoadModel();
	
	
	void CheckCoordMode();
	BOOL IsDrawStreamline();
	void ModifyHeightWithUpdate(double dz);
	BOOL CheckStereoIsOpen(CString strStereID);
	void DisplayTrim(GrBuffer *pBuf);
	void UpdateVisible();
	
protected:
	BOOL Convert(Coordinate Input, Coordinate &Output);
	virtual BOOL ConvertFailed(Coordinate Input, Coordinate &Output);
	//重载函数
	virtual BOOL LoadDocData();
	virtual void ClearSnapDrawing();
	virtual void SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void SetConstDragLine(const GrBuffer *pBuf);

	
	virtual void UpdateSelDrawing(BOOL bAlways=FALSE);
	virtual void UpdateVariantDragLine();
	virtual void UpdateSnapDrawing();

	virtual void HiliteObject(CFeature* pFtr);
	virtual void UnHiliteObject(CFeature* pFtr);
	
	virtual void DelObjfromVectLay(LONG_PTR handle, CVectorLayer *pLL = NULL, CVectorLayer *pLR = NULL);

	virtual void PushViewPos();
	virtual void ActiveViewPos(ViewPos* pos);
	
	CSize GetDimension(BOOL bLeft=TRUE);

	void AdjustCellScale();

	void UpdateButtonStates();

	//判断立体是否可能会闪烁（原因通常是被遮住窗口有GDI绘图操作）
	//该函数，用来判断矢量窗口是否被立体窗口遮住
	BOOL StereoMaybeFlicker();

	// 移动当前测标位置影像到屏幕中心
	void AdjustViewWithCurrentMouse();

	void HideOutsideVertexes(GrBuffer2d *pBufL, GrBuffer2d *pBufR);

	//设置立体模型放大参数
	void SetStereoMagnity(double change,UINT Mode,BOOL bToCenter = TRUE);
	//{{AFX_MSG(CStereoView)
	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg void OnViewOverlay();
	afx_msg void OnHeightMode();
	afx_msg void OnHeightLock();
	afx_msg void OnAutoMode();
	afx_msg void OnManualMode();
	afx_msg void OnParaMode();
	afx_msg void OnShareMode();
	afx_msg void OnIndepMode();
	afx_msg void OnHParallaxLeft();
	afx_msg void OnHParallaxRight();
	afx_msg void OnVParallaxUp();
	afx_msg void OnVParallaxDown();
	afx_msg void OnHeightUpByStep();
	afx_msg void OnHeightDownByStep();
	afx_msg void OnHeightUp();
	afx_msg void OnHeightDown();
	afx_msg void OnSetXYZ();
	afx_msg void OnMoveCursorToCenter();
	afx_msg void OnMouseDrive();
	afx_msg void OnUpdateViewOverlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHeightMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHeightLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutoMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateParaMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateManualMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShareMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndepMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMouseDrive(CCmdUI* pCmdUI);
	afx_msg LRESULT OnDeviceLFootDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove2(WPARAM wParam, LPARAM lParam); //移动测标而不是影像
	afx_msg LRESULT OnDeviceRFootDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadModel(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	 
	afx_msg void OnHscrollLeft();
	afx_msg void OnHscrollRight();
	afx_msg void OnVscrollDown();
	afx_msg void OnVscrollUp();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnExportRaster();
	afx_msg void OnReset3DMouseParam();
	afx_msg void OnUpdateNoHatch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayOrder(CCmdUI* pCmdUI);
	afx_msg void OnShareSysMouse();
	afx_msg void OnUpdateShareSysMouse(CCmdUI* pCmdUI);
	afx_msg void OnReverseCrossColor();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void On3DMouseDriveCross();
//	afx_msg void OnUpdate3DMouseDriveCross(CCmdUI* pCmdUI);
	afx_msg void OnWsAsstereo();
	afx_msg void OnUpdateStereoFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnStereoNohatch();
	afx_msg void OnStereoReverseDisplayorder();
	afx_msg void OnUpdateStereoViewoverlay(CCmdUI* pCmdUI);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseNoHeiMode();
	afx_msg void OnUpdateMouseNoHeiMode(CCmdUI* pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnCtrlPtsBound();
	afx_msg void OnMagnifyHalf();
	afx_msg void OnMagnifyOne();
	afx_msg void OnMagnifyOneAndHalf();
	afx_msg void OnMagnifyTwo();
	afx_msg void OnMagnifyTwoAndHalf();
	afx_msg void OnMagnifyThree();
	afx_msg void OnMakeCheckPtSample();

	//}}AFX_MSG
	afx_msg LRESULT OnClientToGround(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGroundToClient(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnModifyMousepoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseLeave();
	afx_msg void OnStereoFullScreen();
	afx_msg LRESULT UpdateViewParams(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT LockAngle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	void ReadStereoBound_forDisplayTrim();
	void DefineAffineMatrix();

	void ManualLoadVect(Envelope evlp);

	//实时核线的坐标换算
	BOOL RealtimeConvertCoordLeft(double x0, double y0, double &x1, double &y1, BOOL bOrder);
	BOOL RealtimeConvertCoordRight(double x0, double y0, double &x1, double &y1, BOOL bOrder);
};

#endif // !defined(AFX_STEREOVIEW_H__83CBCC6D_5739_411D_8D71_47C2C76F2F9C__INCLUDED_)
