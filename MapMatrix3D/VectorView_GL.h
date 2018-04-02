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

//��Ӱ����ͼ����δʹ��
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

	//����Ӱ�����ת�Ƕ�

	BOOL IsDrawStreamline();
	int m_nRasterAng;
	
	CMenu m_muRasterRotate;
	
	//�߽���ʾɫ
	COLORREF m_clrBound;
	
	//�߽�ǵ�
	PT_3D m_ptBounds[4];  //��Щ�㶼�ǻ���ʲô��������ϵ��
	
	//���ڼ�¼��ת������
	CPoint m_ptViewRotateStart;
	CPoint m_ptViewRotateCenter;
	BOOL   m_bViewRotate;
	
	//�������ϵ��ԭ�㣬��ǰ��ͼ����ת��, ����ϵ��
	double	m_xgoff,m_ygoff/*,m_lfAngle*/;
	
	GrBuffer m_grCoordSys;
	
	
	BOOL m_bOverlayImgLay;
	int m_nMainImageIdx;

	CMutiRasterLayer m_mutiImgLay;//����Ӱ��
	CArray<ImgPosItem,ImgPosItem> m_arrImgPos;//����Ӱ���λ����Ϣ

	double m_lfImgMatrix[16];//����Ӱ�����
	double m_lfImgRMatrix[16];//����Ӱ�����

	//�Ƿ����Ӱ��������ʸ����ͼ��
	//����ǣ�����Ӱ��ʱ��Ӱ���������������ϡ������űȣ�����ʸ���������׺�Ӱ��
	//����񣬵���Ӱ��ʱ��ʸ���������������ϡ������űȣ�����Ӱ��������׺�ʸ����
	BOOL m_bViewBaseOnImage;
	
	//�ֹ�����ʸ��������ʱ�����
	BOOL m_bStartLoadVect;
	PT_3D m_ptStartLoadVect;
	
	//��ʾ�߳���
	BOOL m_bDisplayByHeight;
	
	//��ɫ��ʾ�ȸ���
	BOOL m_bDisplayContoursSpecially;
	
	BOOL m_bViewVector;

	BOOL m_bDriveOnlyStereo;
	
	BOOL m_bDriveOnlyCursor;

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
private:
	//�ڵ���Ӱ��ʱ�������ž�ֵ
//	float  m_fOldDrawCellAngle;
	float  m_fOldDrawCellKX;
	float  m_fOldDrawCellKY;

	CRect m_rcTemp;//��Ҫ���ڴ�ӡԤ������������ǰ������ͻ����ķ�Χ����
	CPrintingDrawingContext *m_pPrintContext;
	CCoordSys *m_pTempCoordSys;
	CCoordSys *m_pComCoordSys;
	int m_nPrintScope; //��ӡ��Χ0 ������ 1 Ӱ��Χ 2 ��ǰ��������
	int m_nPrintScale;  //��ӡ������0 ��Ӧ��ֽ�� 1 ����ͼ������

};

#ifndef _DEBUG  // debug version in EditBaseView.cpp
inline CDlgDoc* CVectorView_GL::GetDocument()
{ return (CDlgDoc*)m_pDocument; }
#endif

#endif // !defined(AFX_VECTORVIEW_PRINT_H__BB89F3B6_D68E_482A_92DF_E5C077CBDEAA__INCLUDED_)
