// VectorView_new.h: interface for the CVectorView_new_new class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORVIEW_NEW_H__5B2B5B73_801E_42D1_B634_2318500613D8__INCLUDED_)
#define AFX_VECTORVIEW_NEW_H__5B2B5B73_801E_42D1_B634_2318500613D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseView.h"
#include "RasterLayer.h"
#include "MutiRasterLayer.h "
#include "ViewImgPosition.h"

#define STEREOVIEW_INPUT_SYSMOUSE		1
#define STEREOVIEW_INPUT_3DMOUSE		2

class CVectorView_new;

//增加访问osgbview
class COsgbView;

class CVectorViewCenterPoint
{
public:
	CVectorViewCenterPoint(CVectorView_new *p);
	~CVectorViewCenterPoint();
	void Save();
	void Restore();

private:
	CVectorView_new *pView;
	PT_3D ptCenter;
};

class CDlgDoc;
class CVectorView_new : public CBaseView
{
	friend class CVectorViewCenterPoint;
	friend class CDlgDoc;

protected: // create from serialization only
	CVectorView_new();
	DECLARE_DYNCREATE(CVectorView_new)

// Attributes
public:
	CDlgDoc* GetDocument();
	virtual CWorker *GetWorker();	
	void ViewRotateAsStereo();

	HBITMAP GetViewBitmap(int nWidthDest, int nHeightDest);
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
	void DriveToXyz(PT_3D *pt, BOOL bMoveImg=FALSE, BOOL bMoveToCenterIfNeed = TRUE);
	virtual BOOL InitBmpLayer();
	virtual BOOL LoadDocData();
	virtual CCoordWnd  GetCoordWnd();
	virtual ~CVectorView_new();
	PT_3D GetCrossPoint();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
protected:	

	virtual void PushViewPos();
	virtual void ActiveViewPos(ViewPos* pos);

	void AddObjtoVectLay2(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround, BOOL bUpperMost=FALSE);
	virtual void AddObjtoMarkVectLay(CFeature * pFtr);
	virtual void UpdateSelDrawing(BOOL bAlways =FALSE );

	virtual void ViewExternal();
	virtual void SetDSVisibleState(int idx,BOOL bVisible);
	virtual void ViewLocal();
	virtual void ViewAll();
	virtual void ViewHideAll();

	virtual void OnChangeCoordSys(BOOL bJustScroll);
	virtual void OnPreInnerCmd(UINT nInnerCmd);
public:
	void UpdateOverlayImage(CArray<ViewImgPosition,ViewImgPosition>& img_pos);
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
	afx_msg void OnUpdateViewOverlay(CCmdUI* pCmdUI);
	afx_msg void OnWsRefImage();
	afx_msg void OnViewKeypoint();
	afx_msg void OnMarkReffile();
	afx_msg void OnUpdateMarkReffile(CCmdUI* pCmdUI);
	afx_msg void OnFillColor();
	afx_msg void OnUpdateFillColor(CCmdUI* pCmdUI);
	afx_msg void OnShowMapGrid();
	afx_msg void OnUpdateMapGrid(CCmdUI* pCmdUI);
	afx_msg void OnShowMapName();
	afx_msg void OnUpdateMapName(CCmdUI* pCmdUI);
	afx_msg void OnShowLineWidth();
	afx_msg void OnUpdateLineWidth(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnFilePrintPreview();
	afx_msg void OnFilePrint();
	afx_msg LRESULT OnDeviceLFootDown(WPARAM nCode, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove(WPARAM nCode, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove2(WPARAM nCode, LPARAM lParam);
	afx_msg LRESULT OnDeviceRFootDown(WPARAM nCode, LPARAM lParam);
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
	afx_msg LRESULT  OnZoomHotKey(WPARAM  wParam, LPARAM  lParam);

	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	//	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	/*增加osgb视图, 增加by曾进翀 2017/7/25*/
	afx_msg void OnOsgbview();


	DECLARE_MESSAGE_MAP()
public:

	COsgbView* getOsgbView();
	
	void GetViewImagePositions(CArray<ViewImgPosition,ViewImgPosition>& img_pos)
	{
		img_pos.Copy(m_arrImgPositions);
	}
	PT_3D* GetptBounds()
	{
		return m_ptBounds;
	}
    void RefreshImageOverlay();
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

	void ReadRefImageInfo();
	void SaveRefImageInfo();

	CRasterLayer *GetRasterLayer(LPCTSTR fileName, int &index);
	CRasterLayer *LoadRasterLayer(LPCTSTR fileName);
	void SetImagePosition(int index);
	void ScrollForDrawBitmap(double dx,double dy,double dz );
	Envelope GetImgLayBound();

	void Callback_DrawFeature(CDlgDataSource *pDS,CFtrLayer *pLayer,CFeature *pFtr, int nSymbolizedFlag, GrBuffer *pBuf);
	BOOL Callback_CheckLayerVisible(CFtrLayer *pLayer);
	void AdjustCellScale();

	/**
	* @brief UpdateSnapDrawing 更新捕捉
	*/
	virtual void UpdateSnapDrawing();

	/**
	* @brief isVectorViewActive 当前视图是否激活状态
	*/
	bool isVectorViewActive();

	void ShowMapGrid();
	//叠加影像的旋转角度

	BOOL IsDrawStreamline();
	int m_nRasterAng;
	
	CMenu m_muRasterRotate;
	
	//边界显示色
	COLORREF m_clrBound;

	//背景显示色
	COLORREF m_clrBack;
	
	//边界角点
	PT_3D m_ptBounds[4];  //这些点都是基于什么样的坐标系？
	
	//用于记录旋转的鼠标点
	CPoint m_ptViewRotateStart;
	CPoint m_ptViewRotateCenter;
	BOOL   m_bViewRotate;
	
	//大地坐标系的原点，当前视图的旋转角, 仿射系数
	double	m_xgoff,m_ygoff/*,m_lfAngle*/;
	
	GrBuffer m_grCoordSys;
	
	CArray<CRasterLayer*,CRasterLayer*> m_arrPImgLays;
	CArray<ViewImgPosition,ViewImgPosition> m_arrImgPositions;
	BOOL m_bOverlayImgLay;
	
	CMarkVectLayer m_markLayL2;
	
	CRealtimeVectLayer m_vectLayL2;

	double m_lfImgMatrix[16];
	double m_lfImgRMatrix[16];

	//是否基于影像来建立矢量视图；
	//如果是，叠加影像时，影像总是正立、朝上、等缩放比，而将矢量变形以套合影像；
	//如果否，叠加影像时，矢量总是正立、朝上、等缩放比；而将影像变形以套合矢量；
	BOOL m_bViewBaseOnImage;
	
	//手工加载矢量到立体时的起点
	BOOL m_bStartLoadVect;
	PT_3D m_ptStartLoadVect;
	
	//显示高程谱
	BOOL m_bDisplayByHeight;

	//是否显示填充颜色
	BOOL m_bFillColor;
	
	//分色显示等高线
	BOOL m_bDisplayContoursSpecially;
	
	BOOL m_bViewVector;

	BOOL m_bDriveOnlyStereo;
	
	BOOL m_bDriveOnlyCursor;

	//是否在鼠标移动的时候联动其它矢量
	BOOL m_bMouseDriveVector;

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

	BOOL m_bMarkReffile;

	BOOL m_bImageZoomNotLinear;

	BOOL m_bUseAnyZoomrate;

	//显示图幅信息
	BOOL m_ShowMapGrid;
	BOOL m_ShowMapName;
	BOOL mapgrid_showed;
	double m_fWid;
	double m_fHei;
private:
//	float  m_fOldDrawCellAngle;
	float  m_fOldDrawCellKX;
	float  m_fOldDrawCellKY;
	CRect m_rcTemp;//主要用于打印预览隐藏主窗口前，保存客户区的范围矩形
	C2DPrintGDIDrawingContext *m_pPrintContext;
	CCoordSys *m_pTempCoordSys;
	CCoordSys *m_pComCoordSys;
	int m_nPrintScope; //打印范围0 工作区 1 影像范围 2 当前窗口区域
	int m_nPrintScale;  //打印比例尺0 适应到纸张 1 按成图比例尺

	float	m_fPrintMarginLeft;
	float	m_fPrintMarginRight;
	float	m_fPrintMarginTop;
	float	m_fPrintMarginBottom;
	float	m_fPrintExtension;

	CMutiRasterLayer m_MutiImgLay;
	HDC m_hTempMemDC ;
	HBITMAP m_hTempMemBmp;
	HBITMAP m_hOldBmp;
public:

	virtual void ScrollView(int dx, int dy, int dp, int dq);
	virtual void Scroll(int dx, int dy, int dp, int dq = 0, int step = 6);
	virtual void ZoomChange(CPoint point, double change, BOOL bToCenter = TRUE);
	virtual void ZoomNext(CPoint point, int dir, BOOL bToCenter = TRUE);
	void UpdateMove(bool withScale = false);
};

class CVectorCellView_new : CVectorView_new
{
	DECLARE_DYNCREATE(CVectorCellView_new)

public:

	CDlgCellDoc* GetDocument();
	virtual CWorker *GetWorker();

protected:
	virtual void UpdateGridDrawing();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectorCellView_new)
public:
	//##ModelId=41466B7B039D
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

#ifndef _DEBUG  // debug version in EditBaseView.cpp
inline CDlgDoc* CVectorView_new::GetDocument()
   { return (CDlgDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VECTORVIEW_NEW_H__5B2B5B73_801E_42D1_B634_2318500613D8__INCLUDED_)
