#if !defined(AFX_BASEVIEW_H__0BC4029B_0890_4E52_B74C_4DE1444767D9__INCLUDED_)
#define AFX_BASEVIEW_H__0BC4029B_0890_4E52_B74C_4DE1444767D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseView.h : header file
//

#include "InteractiveView.h"
#include "vectorLayer.h"
#include "editbasedoc.h"

#include "MarkVectLayer.h"
#include "RealtimeVectLayer.h"

#define TIMERID_UPDATEVIEW				1000
#define TIMERID_SIMPLIFY_UPDATEVIEW		1001


#define HANDLE_CONSTDRAG		-3
#define HANDLE_GRID				-4
#define HANDLE_BOUND			-5
#define HANDLE_GRAPH			-100
#define HANDLE_SUSPT			-101
#define HANDLE_PSEPT			-102


/////////////////////////////////////////////////////////////////////////////
// CBaseView view
class CDlgDoc;
class CBaseView : public CInteractiveView
{
public:
	//����ڴ������ϵ�ı����ͽǶ�
	struct GScaleAngle
	{
		GScaleAngle(){
			bInited = FALSE;
			lfGKX = 1.0; lfGKY = 1.0; lfAngle = 0;
		}
		BOOL bInited;
		double lfGKX, lfGKY;
		double lfAngle;
	};
protected:
	CBaseView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBaseView)

	BOOL m_bAccuLeave;

	CVectorLayer m_vectLayL;
	CVectorLayer m_vectLayR;

	CVectorLayer m_markLayL;
	CVectorLayer m_markLayR;

	CVariantDragLayer m_accurboxLayL;
	CVariantDragLayer m_accurboxLayR;

	CVariantDragLayer m_snapLayL;
	CVariantDragLayer m_snapLayR;

	//�Ƿ���Ż���ʾ
	BOOL m_bSymbolize;

	//�Ե���Ż����߷��Ż�������Ż��ֱ����
	BOOL m_bSymbolizePoint, m_bSymbolizeCurve, m_bSymbolizeSurface;

	// ������ɫ���
	BOOL m_bSymbolscolorByLayer;

	COLORREF m_clrSnap;

	BOOL m_bShowSnap;

	int m_nDragLineWid;

	// ��������ɫ
	COLORREF m_clrFtrPtColor;

	//������ʾ->����ʾΪ�����ɫ��͸����
	int m_Transparency;

	//ʸ�����Ƿ����ѹ���Ż�
	BOOL m_bVectLayCompress;

	BOOL m_bShowKeyPoint;
	//�Ƿ���ʾҧ�ϵ�
	BOOL m_bShowSnapPt;
	
	//��̬���Ƿ��ǻ��ڴ������
	BOOL m_bDrawGrdDrag, m_bEraseGrdDrag;
	
	//ҧ����
	SNAPITEM m_itemCurSnap;
	SNAPITEM m_itemOldSnap;
	PT_4D m_ptCurSnapClt, m_ptOldSnapClt;
	
	GScaleAngle m_gsa;
	VIEW_TYPE m_ViewType;

	GRIDParam m_sGridParam;

	//�Ƿ�����Ҽ��л��ɱ�״̬
	BOOL m_bRCLKSwitch;
	double m_lfMatrixLic[9], m_lfRMatrixLic[9];
    BOOL  m_bShowSusPoint;
	BOOL m_bShowPseudoNode;

	//��ǰ��������1m��Ӧ��������
	double m_lfMapScaleX;
	double m_lfMapScaleY;

	//�Ҽ��˵�
	CMenu m_menuRBD;

public:
	CMap<CFtrLayer*,CFtrLayer*,int,int> m_mapLayerVisible;
public:

// Operations
	virtual CWorker * GetWorker();
	CDlgDoc* GetDocument();
	virtual GScaleAngle GetViewScaleAngle() { return m_gsa; }

	//��ȡ���㣬�����ܵ�ҧ�ϡ���ȷ��ͼ��Ӱ�죬����������ʹ�õ����겻һ����ͬ
	virtual PT_3D GetCrossPoint();

	void AccuRightAngleAdjust(PT_3D &pt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseView)
	public:
	virtual void OnInitialUpdate();

	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBaseView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	virtual CCoordWnd GetCoordWnd();
protected:
	void RegisterPlugsInCmds();
	void RegisterPlugsInSelectCmds();

	BOOL CanSymbolized(CFeature *pFtr);

	virtual void ViewExternal();
	virtual void SetDSVisibleState(int idx,BOOL bVisible);
	virtual void ViewLocal();
	virtual void ViewAll();
	virtual void ViewHideAll();
	virtual void UpdateGridDrawing();
public:
	Envelope EnvelopeFromPtAndRadius(PT_4D cltpt, double r);
//protected:
	virtual void ClearAccuDrawing();
	virtual void ClearSnapDrawing();
	virtual void UpdateAccuDrawing();
	virtual void UpdateSnapDrawing();
	virtual void UpdateSelDrawing(BOOL bAlways=FALSE);
	virtual void UnHiliteObject(CFeature *pFtr);
	virtual void HiliteObject(CFeature *pFtr);	
	virtual void DrawTmptoVectLay(const GrBuffer *pBuf, CVectorLayer *pLL=NULL, CVectorLayer *pLR=NULL);
	virtual void ZoomChange(CPoint point,float change, BOOL bToCenter = TRUE);
	virtual void GroundToClient(PT_3D *pt0, PT_4D *pt1);
	virtual void ClientToGround(PT_4D *pt0, PT_3D *pt1);

	virtual void DelObjfromVectLay(LONG_PTR handle, CVectorLayer *pLL = NULL, CVectorLayer *pLR = NULL);
	virtual void AddObjtoMarkVectLay(CFeature * pFtr);
	virtual void GroundToVectorLay(PT_3D *pt0, PT_4D *pt1);
	virtual void AddObjtoVectLay(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround = TRUE, CVectorLayer *pLL = NULL, CVectorLayer *pLR = NULL);
	virtual BOOL InitBmpLayer();
	virtual BOOL LoadDocData();
	virtual void OnPreInnerCmd(UINT nInnerCmd);
	virtual void OnPostInnerCmd();
	virtual void ImageToGround(PT_4D *pt0, PT_3D *pt1);
	virtual void GroundToImage(PT_3D *pt0, PT_4D *pt1);
	virtual void ImageToClient(PT_4D *pt0, PT_4D *pt1);
	virtual void ClientToImage(PT_4D *pt0, PT_4D *pt1);
	virtual void ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer2d *outL, GrBuffer2d *outR);

	virtual void OnChangeCoordSys(BOOL bJustScroll);

	//{{AFX_MSG(CBaseView)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSnapClick();
	afx_msg void OnSelectAll();
	afx_msg void OnViewKeypoint();
	afx_msg void OnUpdateViewKeypoint(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewTypeAll();	
	afx_msg void OnViewTypeLocal();	
	afx_msg void OnViewTypeExternal();	
	afx_msg void OnViewTypeAllorlocal();
	afx_msg void OnViewTypeShoworhide();
	afx_msg void OnUpdateViewType(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSymbolscolorByLayer();
	afx_msg void OnLayerVisible();
	//}}AFX_MSG
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHscrollLeft();
	afx_msg void OnHscrollRight();
	afx_msg void OnVscrollDown();
	afx_msg void OnVscrollUp();
	afx_msg void OnSwitchmodel();
	afx_msg void OnMouseLeave();
	afx_msg void OnCommandType(UINT nID);
	afx_msg void OnUpdateCommandType(CCmdUI* pCmdUI);
	afx_msg void OnState_CurveType(UINT nID);
	afx_msg void OnUpdateState_CurveType(CCmdUI* pCmdUI);
#ifndef _NOT_USE_DPWPACK
	afx_msg void OnTestCreateContourCreate();
#endif
	afx_msg void OnTestCreateContourRemove();
	afx_msg void OnRefreshVectoryDisplayOrder();
	afx_msg void OnPlugCommandType(UINT nID);
	afx_msg void OnUpdatePlugCommandType(CCmdUI* pCmdUI);
	afx_msg void OnPlugFuncType(UINT nID);
	afx_msg void OnUpdatePlugFuncType(CCmdUI* pCmdUI);
	afx_msg void OnSelectPolygon();
	afx_msg void OnUpdateSelectPolygon(CCmdUI* pCmdUI);
	afx_msg void OnSelectRect();
	afx_msg void OnUpdateSelectRect(CCmdUI* pCmdUI);
	afx_msg void OnViewSymbolized();
	afx_msg void OnUpdateViewSymbolized(CCmdUI* pCmdUI);
	afx_msg void OnCommandOne();
	afx_msg void OnDestroy();
	afx_msg void OnSwitchSymbolize();
	afx_msg void OnSwitchSymbolizePoint();
	afx_msg void OnSwitchSymbolizeLine();
	afx_msg void OnSwitchSymbolizeSurface();
	afx_msg void OnSwitchSymbolizeAnnotation();
	afx_msg void OnUpdateSwitchSymbolizePoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwitchSymbolizeLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwitchSymbolizeSurface(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwitchSymbolizeAnnotation(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASEVIEW_H__0BC4029B_0890_4E52_B74C_4DE1444767D9__INCLUDED_)
