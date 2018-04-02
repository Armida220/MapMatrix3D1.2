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

//���ӷ���osgbview
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

	/*����osgb��ͼ, ����by������ 2017/7/25*/
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
	* @brief UpdateSnapDrawing ���²�׽
	*/
	virtual void UpdateSnapDrawing();

	/**
	* @brief isVectorViewActive ��ǰ��ͼ�Ƿ񼤻�״̬
	*/
	bool isVectorViewActive();

	void ShowMapGrid();
	//����Ӱ�����ת�Ƕ�

	BOOL IsDrawStreamline();
	int m_nRasterAng;
	
	CMenu m_muRasterRotate;
	
	//�߽���ʾɫ
	COLORREF m_clrBound;

	//������ʾɫ
	COLORREF m_clrBack;
	
	//�߽�ǵ�
	PT_3D m_ptBounds[4];  //��Щ�㶼�ǻ���ʲô��������ϵ��
	
	//���ڼ�¼��ת������
	CPoint m_ptViewRotateStart;
	CPoint m_ptViewRotateCenter;
	BOOL   m_bViewRotate;
	
	//�������ϵ��ԭ�㣬��ǰ��ͼ����ת��, ����ϵ��
	double	m_xgoff,m_ygoff/*,m_lfAngle*/;
	
	GrBuffer m_grCoordSys;
	
	CArray<CRasterLayer*,CRasterLayer*> m_arrPImgLays;
	CArray<ViewImgPosition,ViewImgPosition> m_arrImgPositions;
	BOOL m_bOverlayImgLay;
	
	CMarkVectLayer m_markLayL2;
	
	CRealtimeVectLayer m_vectLayL2;

	double m_lfImgMatrix[16];
	double m_lfImgRMatrix[16];

	//�Ƿ����Ӱ��������ʸ����ͼ��
	//����ǣ�����Ӱ��ʱ��Ӱ���������������ϡ������űȣ�����ʸ���������׺�Ӱ��
	//����񣬵���Ӱ��ʱ��ʸ���������������ϡ������űȣ�����Ӱ��������׺�ʸ����
	BOOL m_bViewBaseOnImage;
	
	//�ֹ�����ʸ��������ʱ�����
	BOOL m_bStartLoadVect;
	PT_3D m_ptStartLoadVect;
	
	//��ʾ�߳���
	BOOL m_bDisplayByHeight;

	//�Ƿ���ʾ�����ɫ
	BOOL m_bFillColor;
	
	//��ɫ��ʾ�ȸ���
	BOOL m_bDisplayContoursSpecially;
	
	BOOL m_bViewVector;

	BOOL m_bDriveOnlyStereo;
	
	BOOL m_bDriveOnlyCursor;

	//�Ƿ�������ƶ���ʱ����������ʸ��
	BOOL m_bMouseDriveVector;

	//�Ƿ���3D����ͼ��ʱ�����������Ϣ
	BOOL m_bDisableMouseIn3DDraw;
	BOOL m_b3DDraw;
	CPoint m_ptLastMouse;
	//��ǰʹ���еĲ����������; 
	//STEREOVIEW_INPUT_SYSMOUSE, ϵͳ���; 
	//STEREOVIEW_INPUT_3DMOUSE, ��ά���
	int  m_nInputType;
	PT_3D m_gCur3DMousePoint;

	//���ӵ� DEM
	CDSM		m_dem;
	CString			m_strDemPath;

	CCoordSys *m_pRealCS;	

	BOOL m_bMarkReffile;

	BOOL m_bImageZoomNotLinear;

	BOOL m_bUseAnyZoomrate;

	//��ʾͼ����Ϣ
	BOOL m_ShowMapGrid;
	BOOL m_ShowMapName;
	BOOL mapgrid_showed;
	double m_fWid;
	double m_fHei;
private:
//	float  m_fOldDrawCellAngle;
	float  m_fOldDrawCellKX;
	float  m_fOldDrawCellKY;
	CRect m_rcTemp;//��Ҫ���ڴ�ӡԤ������������ǰ������ͻ����ķ�Χ����
	C2DPrintGDIDrawingContext *m_pPrintContext;
	CCoordSys *m_pTempCoordSys;
	CCoordSys *m_pComCoordSys;
	int m_nPrintScope; //��ӡ��Χ0 ������ 1 Ӱ��Χ 2 ��ǰ��������
	int m_nPrintScale;  //��ӡ������0 ��Ӧ��ֽ�� 1 ����ͼ������

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
