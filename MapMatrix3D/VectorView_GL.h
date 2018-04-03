// VectorView_Print.h: interface for the CVectorView_GL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORVIEW_PRINT_H__BB89F3B6_D68E_482A_92DF_E5C077CBDEAA__INCLUDED_)
#define AFX_VECTORVIEW_PRINT_H__BB89F3B6_D68E_482A_92DF_E5C077CBDEAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseView.h"
#include "RasterLayer.h"
#include "MutiRasterLayer.h "

#define STEREOVIEW_INPUT_SYSMOUSE		1
#define STEREOVIEW_INPUT_3DMOUSE		2

class CVectorView_GL;
class CVectorViewCenterPoint1
{
public:
	CVectorViewCenterPoint1(CVectorView_GL *p);
	~CVectorViewCenterPoint1();
	void Save();
	void Restore();
	
private:
	CVectorView_GL *pView;
	PT_3D ptCenter;
};

//多影像视图，暂未使用
class CDlgDoc;
class CVectorView_GL : public CBaseView  
{
	struct ImgPosition
	{		
		double lfImgLayOX, lfImgLayOY;
		double lfImgMatrix[4];
		double lfPixelSizeX, lfPixelSizeY;
		int nPixelBase;
	};
	struct ImgPosItem
	{
		ImgPosItem(){
			memset(&pos,0,sizeof(pos));
			pos.lfImgMatrix[0]	= pos.lfImgMatrix[3] = 1.0;
		}
		CString fileName;
		ImgPosition pos;
	};
	friend class CVectorViewCenterPoint1;

protected: // create from serialization only
	CVectorView_GL();
	DECLARE_DYNCREATE(CVectorView_GL)

// Attributes
public:
	CDlgDoc* GetDocument();
	virtual CWorker *GetWorker();	
	void ViewRotateAsStereo();

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectorView_new)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void UpdateConstDragLine();
	virtual void AddConstDragLine(const GrBuffer *pBuf);
	void DriveToXyz(PT_3D *pt, BOOL bMoveImg=FALSE);
	virtual BOOL InitBmpLayer();
	virtual BOOL LoadDocData();
	virtual CCoordWnd  GetCoordWnd();
	virtual ~CVectorView_GL();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:	

	virtual void PushViewPos();
	virtual void ActiveViewPos(ViewPos* pos);
// Generated message map functions
protected:
	//{{AFX_MSG(CVectorView_new)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnOverlayRaster();
	afx_msg void OnRefdem();
	afx_msg void OnUnrefdem();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMoveCursorToCenter();
	afx_msg void OnViewRotateAsStereo();
	afx_msg void OnMeasureImagePosition();	
	afx_msg void OnUpdateViewRotateAsStereo(CCmdUI* pCmdUI);
	afx_msg void OnManulLoadVect();
	afx_msg void OnUpdateManulLoadVect(CCmdUI* pCmdUI);
	afx_msg void OnExportRaster();
	afx_msg void OnViewOverlay();
	//}}AFX_MSG
	afx_msg void OnFilePrintPreview();
	afx_msg void OnFilePrint();
	afx_msg LRESULT OnDeviceLFootDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceRFootDown(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseLeave();
	afx_msg void OnSetXYZ();
	afx_msg void OnDisplayByheight();
	afx_msg void OnUpdateDisplayByheight(CCmdUI* pCmdUI);
	afx_msg void OnDisplayContoursSpecially();
	afx_msg void OnUpdateDisplayContoursSpecially(CCmdUI* pCmdUI);
	afx_msg LRESULT OnMoveCursorToCenter2(WPARAM, LPARAM);
	afx_msg void OnViewReset();
	afx_msg void OnRotateRaster();
	afx_msg void OnUpdateRotateRaster(CCmdUI* pCmdUI);
	afx_msg void OnWsAsstereo();
	void RotateRaster(int ang);
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnRefresh();
	DECLARE_MESSAGE_MAP()

protected:
	float ZoomSelect(float wantZoom);
	void CalcImgSysParams(PT_3D *ptsBound);
	BOOL PrePrint(CDC* pDC, CPrintInfo* pInfo);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void GroundToVectorLay(PT_3D *pt0, PT_4D *pt1);
	virtual void ImageToGround(PT_4D *pt0, PT_3D *pt1);
	virtual void ImageToClient(PT_4D *pt0, PT_4D *pt1);
	virtual void GroundToImage(PT_3D *pt0, PT_4D *pt1);
	virtual void ClientToImage(PT_4D *pt0, PT_4D *pt1);
	virtual void UpdateGridDrawing();
	void DrawBkgnd(GrBuffer *buf);

	void SetImagePosition();
	void RefreshImageOverlay();
	void ScrollForDrawBitmap(double dx,double dy,double dz );

	//叠加影像的旋转角度

	BOOL IsDrawStreamline();
	int m_nRasterAng;
	
	CMenu m_muRasterRotate;
	
	//边界显示色
	COLORREF m_clrBound;
	
	//边界角点
	PT_3D m_ptBounds[4];  //这些点都是基于什么样的坐标系？
	
	//用于记录旋转的鼠标点
	CPoint m_ptViewRotateStart;
	CPoint m_ptViewRotateCenter;
	BOOL   m_bViewRotate;
	
	//大地坐标系的原点，当前视图的旋转角, 仿射系数
	double	m_xgoff,m_ygoff/*,m_lfAngle*/;
	
	GrBuffer m_grCoordSys;
	
	
	BOOL m_bOverlayImgLay;
	int m_nMainImageIdx;

	CMutiRasterLayer m_mutiImgLay;//辅助影像
	CArray<ImgPosItem,ImgPosItem> m_arrImgPos;//辅助影像的位置信息

	double m_lfImgMatrix[16];//与主影像相关
	double m_lfImgRMatrix[16];//与主影像相关

	//是否基于影像来建立矢量视图；
	//如果是，叠加影像时，影像总是正立、朝上、等缩放比，而将矢量变形以套合影像；
	//如果否，叠加影像时，矢量总是正立、朝上、等缩放比；而将影像变形以套合矢量；
	BOOL m_bViewBaseOnImage;
	
	//手工加载矢量到立体时的起点
	BOOL m_bStartLoadVect;
	PT_3D m_ptStartLoadVect;
	
	//显示高程谱
	BOOL m_bDisplayByHeight;
	
	//分色显示等高线
	BOOL m_bDisplayContoursSpecially;
	
	BOOL m_bViewVector;

	BOOL m_bDriveOnlyStereo;
	
	BOOL m_bDriveOnlyCursor;

	//是否在3D鼠标测图的时候废弃鼠标的消息
	BOOL m_bDisableMouseIn3DDraw;
	BOOL m_b3DDraw;
	CPoint m_ptLastMouse;
	//当前使用中的测标输入类型; 
	//STEREOVIEW_INPUT_SYSMOUSE, 系统鼠标; 
	//STEREOVIEW_INPUT_3DMOUSE, 三维鼠标
	int  m_nInputType;
	PT_3D m_gCur3DMousePoint;

	//叠加的 DEM
	CDSM		m_dem;
	CString			m_strDemPath;
	
	CCoordSys *m_pRealCS;	
private:
	//在叠加影像时保存缩放旧值
//	float  m_fOldDrawCellAngle;
	float  m_fOldDrawCellKX;
	float  m_fOldDrawCellKY;

	CRect m_rcTemp;//主要用于打印预览隐藏主窗口前，保存客户区的范围矩形
	CPrintingDrawingContext *m_pPrintContext;
	CCoordSys *m_pTempCoordSys;
	CCoordSys *m_pComCoordSys;
	int m_nPrintScope; //打印范围0 工作区 1 影像范围 2 当前窗口区域
	int m_nPrintScale;  //打印比例尺0 适应到纸张 1 按成图比例尺

};

#ifndef _DEBUG  // debug version in EditBaseView.cpp
inline CDlgDoc* CVectorView_GL::GetDocument()
{ return (CDlgDoc*)m_pDocument; }
#endif

#endif // !defined(AFX_VECTORVIEW_PRINT_H__BB89F3B6_D68E_482A_92DF_E5C077CBDEAA__INCLUDED_)
