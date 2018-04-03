// SceneViewEx.h: interface for the CSceneViewEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEVIEWEX_H__E87E15D1_E876_48A1_878D_424D12794BC3__INCLUDED_)
#define AFX_SCENEVIEWEX_H__E87E15D1_E876_48A1_878D_424D12794BC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneView.h"
#include "CursorLayer.h"

MyNameSpaceBegin

class EXPORT_SMARTVIEW CSceneViewEx : public CSceneView  
{
	DECLARE_DYNCREATE(CSceneViewEx)
public:
	CSceneViewEx();
	virtual ~CSceneViewEx();

public:
	virtual void DoPaint(const CRect *pRcPaint, int whichbuf, BOOL bIncludeNotErasable=TRUE, BOOL bIncludeErasable=TRUE);
	virtual void OnInitialUpdate();
	//基于客户坐标的测标位置
	void SetCrossPos(float x,float y, float xr);
	void GetCrossPos(float *x,float *y, float *xr);
	void UpdateCross();
	void ClearCross();

	int	 SetCrossType(int index);
	COLORREF SetCrossColor(COLORREF color);
	void SetCrossSelMarkWid(int wid);
	void ResetCursorDrawing();

	COLORREF GetCrossColor();
	int GetCrossSelMarkWid();
	int GetCrossType();
	
	void EnableCrossVisible(BOOL bEnable);
	void EnableCrossSelMarkVisible(BOOL bEnable);
	void EnableCrossCrossVisible(BOOL bEnable);
	void EnableHardCross(BOOL bEnable);
	void EnableSysCross(BOOL bEnable);

	BOOL IsCrossVisible();
	BOOL IsCrossSelMarkVisible();
	BOOL IsCrossCrossVisible();
	BOOL IsHardCross(){
		return m_bHardCross;
	}
	BOOL IsSysCross(){
		return m_bSysCross;
	}

	void DrawCrossOverlay();

public:
	//{{AFX_MSG(CSceneViewEx)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnHandMove();
	afx_msg void OnZoomRect();
	afx_msg void OnUpdateHandMove(CCmdUI *pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSettingCursor();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
		
	DECLARE_MESSAGE_MAP()

protected:
	CPoint m_ptLBtnDn; //拖动影像时的起点
	HCURSOR m_hSaveCursor; //在内部命令中设置光标后，保存的原来的光标
	BOOL m_bUseSaveCursor; //标志是否有保存光标
	CRect m_rcZoomDrag; //拉框放大的矩形

	CCursorLayer m_cursorL, m_cursorR;
	BOOL m_bHardCross, m_bSysCross;
};

MyNameSpaceEnd

#endif // !defined(AFX_SCENEVIEWEX_H__E87E15D1_E876_48A1_878D_424D12794BC3__INCLUDED_)
