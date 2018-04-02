// EditBaseView.h : interface of the CEditBaseView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_INTERACTIVEVIEW_HEAD_INCLUDED_)
#define _INTERACTIVEVIEW_HEAD_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneViewEx.h"
#include "CoordWnd.h"
#include "Worker.h"
#include "VectorLayer.h"
#include "TmpGraphLayer.h"

#define UPDATEREGION_BACKGROUND	1050
#define UPDATEREGION_OBJECT		1100
#define UPDATEREGION_HILITE		1200
#define UPDATEREGION_CONSTDRAG	1550
#define UPDATEREGION_VARIANTDRAG 1700

#define HANDLE_CONSTDRAG		-3

MyNameSpaceBegin

class EXPORT_EDITBASE CUpdateFlag
{
public:
	CUpdateFlag(){
		m_bEnabled = FALSE;
		m_nFlag = INT_MAX;
	}
	~CUpdateFlag(){
	}
	void Enable(BOOL bEnabled){
		m_bEnabled = bEnabled;
		ClearFlag();
	}
	BOOL IsEnabled(){
		return m_bEnabled;
	}
	void UpdateFlag(int nFlag){
		if( m_bEnabled && m_nFlag>nFlag )
			m_nFlag = nFlag;
	}
	void ClearFlag(){
		m_nFlag = INT_MAX;
	}
	int GetFlag(){
		return m_nFlag;
	}
protected:
	BOOL m_bEnabled;
	int m_nFlag;
};

class GrBuffer;

class EXPORT_EDITBASE CInteractiveView : public CSceneViewEx
{
	
protected:
	CConstDragLayer m_constDragL, m_constDragR;
	CAddConstDragLayer m_addconstDragL, m_addconstDragR;
	CVariantDragLayer m_variantDragL, m_variantDragR;

	BOOL m_bConstDragChanged;

	COLORREF m_clrHilite;
	COLORREF m_clrDragLine;

	double m_matrix[16], m_rmatrix[16];

	PT_3D m_gCurPoint;

	CCoordSys m_ViewCS;
	CCoordSys m_SearchCS;

protected: // create from serialization only
	CInteractiveView();
	DECLARE_DYNCREATE(CInteractiveView)

// Attributes
public:
	virtual CWorker *GetWorker();
	PT_3D GetCurPoint() const { return m_gCurPoint; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditBaseView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInteractiveView();

protected:	
	virtual void ClientToGround(PT_4D *pt0, PT_3D *pt1);
	virtual void GroundToClient(PT_3D *pt0, PT_4D *pt1);
	virtual void GroundToVectorLay(PT_3D *pt0, PT_4D *pt1);

	virtual CCoordWnd GetCoordWnd();
	virtual void ZoomCustom(CPoint point,float change, BOOL bToCenter=TRUE);
	virtual void ScrollView(int dx, int dy, int dp, int dq);
	virtual CSize CalcViewSize();

protected:

	void DrawGrBuffer(CDC *pDC, const GrBuffer *pBuf, CCoordSys *pCS, COLORREF color, BOOL bColorUsed, int mode);

	virtual void SetConstDragLine(const GrBuffer *pBuf);
	virtual void SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void ClearDragLine();

	// 绘制地物动态线
	virtual void AddConstDragLine(const GrBuffer *pBuf);
	
	// 绘制地物动态线

	virtual void UpdateConstDragLine();
	virtual void UpdateVariantDragLine();

	virtual void HiliteObject(CGeometry *pobj);

	void SetCursorType(long type);

	virtual void ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer2d *outL, GrBuffer2d *outR);
	void ConvertGrBufferToClient(const GrBuffer *in, GrBuffer2d *outL, GrBuffer2d *outR);
	
// Generated message map functions
protected:
	//{{AFX_MSG(CEditBaseView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelect();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnClientToGround(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGroundToClient(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

MyNameSpaceEnd

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_INTERACTIVEVIEW_HEAD_INCLUDED_)
