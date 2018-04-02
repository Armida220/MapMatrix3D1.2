#if !defined(AFX_SCENEVIEW_H__12B690F7_D261_44FB_A88A_296ECCBBB274__INCLUDED_)
#define AFX_SCENEVIEW_H__12B690F7_D261_44FB_A88A_296ECCBBB274__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneView.h : header file
//

#include "XVCtrlBar.h"
#include "LongScrollCtrl.h"
#include "DrawingLayerManager.h"
#include <afxmt.h>

#define TIMER_ID_PLAN_BGLOAD				1
#define TIMER_ID_BGLOAD					2
#define TIMER_ID_DELAYVIEW				3

#define IDC_BUTTON_REFRESH              1005
#define IDC_BUTTON_HANDMOVE             1006
#define IDC_BUTTON_ZOOMIN               1007
#define IDC_BUTTON_ZOOMOUT              1008
#define IDC_BUTTON_STEREO               1009
#define IDC_BUTTON_ADJUST               1010
#define IDC_BUTTON_CURSOR               1017
#define IDC_BUTTON_SYSCURSOR			1020
#define IDC_BUTTON_ZOOMFIT              1037
#define IDC_BUTTON_ZOOMRECT             1038
#define IDC_COMBO_ZOOM					1039

#define WM_MYTIMER						(WM_USER+201)

#define FCCM_VIEWCLEAR		WM_USER+3000

MyNameSpaceBegin


class EXPORT_SMARTVIEW CMyTimer
{
public:
	struct TimerItem
	{
		HWND hWnd;
		UINT nIDEvent,uElapse,nMsg;
	};
	CArray<TimerItem,TimerItem> m_arrItems;
	CArray<DWORD,DWORD> m_arrTimes;

	HANDLE m_hThread;
	CEvent m_eventClose;
	BOOL m_bProcessMsgs[100];

	CMyTimer();
	~CMyTimer();

	void SetTimer(HWND hWnd, UINT nIDEvent, UINT uElapse, UINT nMsg);
	void StartTimer();
	void CloseTimer();
	static DWORD WINAPI ThreadFunc(LPVOID pParam);

	void OnFinishMsg(int index);
};



class CStereoDrawingContext;

/////////////////////////////////////////////////////////////////////////////
// CSceneView view

class EXPORT_SMARTVIEW CSceneView : public CView
{
protected:
	CSceneView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSceneView)

// Attributes
public:
	struct ViewPos
	{
		double matrix_left[16];
		double matrix_right[16];
		float  zoom;
		PT_3D pt;
	};

// Operations
public:
	void	SetCtrlPos(int x, int y, int cx, int cy);
	void	SetButtonState(int id, int add, int del);
	UINT	GetButtonState(int id);
	void	AddButton(int id, HBITMAP hBmp, LPCTSTR tips);
	void	ZoomFit(CSize size);
	void	ZoomOut();
	void	ZoomIn();
	virtual void	Scroll(int dx,int dy,int dp,int dq=0, int step=6);
	inline double GetZoomRate(){	return m_fZoomRate; }
	//by shy
    virtual void StereoMove(){}
	void ShowOrHideScrollBar(BOOL bShow);
protected:
	//pRcPaint可见窗口的某个区域，
	BOOL DrawRectBitmap(HBITMAP hBmp,CRect bmpRect, CRect *pRcPaint, CPtrArray& layersL,CPtrArray& layersR, int whichbuf);
	virtual void OnPreInnerCmd(UINT nInnerCmd);
	virtual void OnPostInnerCmd();
	virtual CSize CalcViewSize();
	virtual double ZoomSelect(double wantZoom);
	virtual void ZoomChange(CPoint point,double change, BOOL bToCenter=TRUE);
	double	GetComboNextZoom(int dir);
	virtual void ZoomNext(CPoint point,int dir, BOOL bToCenter=TRUE);
	virtual void RecalcZoomRateList();
	void UpdateComboZoom();

	void	RecalcScrollBar(BOOL breset=TRUE);

	virtual void DoPaint(const CRect *pRcPaint, int whichbuf, BOOL bIncludeNotErasable = TRUE, BOOL bIncludeErasable = TRUE);

	CPoint GetScrollPos();

	virtual void RawScroll(int dx, int dy, int dp, int dq, int step=6);

	void RefreshDrawingLayers();
	void UpdateDrawingLayers(CDrawingLayer *pAfterLayerL, CDrawingLayer *pAfterLayerR);
	void FinishUpdateDrawing();

	BOOL CheckCacheReady();

	void ReadAdjustParams();
	void SaveAdjustParams();
	void SetAdjustParams();

	int GetViewMargin(){
		return m_nViewMargin;
	}

	BOOL IsShutterStereo(){
		return (m_pContext->GetDisplayMode()==CStereoDrawingContext::modeShutterStereo);
	}

	BOOL IsRGStereo(){
		return (m_pContext->GetDisplayMode()==CStereoDrawingContext::modeRGStereo);
	}

	BOOL IsSplit(){
		return (m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit);
	}

	BOOL IsPolarized(){
		return (m_pContext->GetDisplayMode()==CStereoDrawingContext::modeInterleavedStereo);
	}

	BOOL IsSingle(){
		return (m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSingle);
	}

	int GetStereoParallax(int type=0){
		return (type==0?m_nHorzParallax:m_nVertParallax);
	}

	int GetBestCacheTypeForLayer(CDrawingLayer *pLayer);

	void UpdateWindow_ForceDraw();

	virtual void OnChangeCoordSys(BOOL bJustScroll);

	LRESULT ClearData(WPARAM wp, LPARAM lp);

private:
	void UpdateDrawingLayersByIndex(int nIdxLeft, int nIdxRight);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneView)
protected:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSceneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void PushViewPos();
	virtual void ActiveViewPos(ViewPos* pos);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSceneView)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRefresh();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnZoomFit();
	afx_msg void OnDisplay();
	afx_msg void OnZoom();
	afx_msg LRESULT OnZoomChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnComboZoomChange();
	afx_msg void OnInverseStereo();
	afx_msg void OnAdjust();
	afx_msg LRESULT OnAdjustUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewUndo();
	afx_msg void OnViewRedo();
	afx_msg LRESULT OnMyTimer(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CArray<int,int>	m_cmdUseFlags;
	CXVCtrlBar      m_ctrlBarHorz; //视图的水平工具条（包括滚动条）
	CXVCtrlBar      m_ctrlBarVert; //视图的垂直工具条（包括滚动条）
	UINT			m_nInnerCmd; //用户激发的视图工具条的当前按钮命令
	CUIntArray      m_arrSaveInnerCmds;
	
	CArray<double,double> m_arrCanZoomRate; //可以支持的缩放比率

	// 是否支持滑块缩放
	BOOL			m_bSliderZoom;

	TCHAR			m_strRegSection[256];
	
	CLongScrollCtrl	m_ScrollCtrl; //滚动条
	
	double			m_fZoomRate; //放大比率
	int				m_nViewMargin; //视图相对影像的扩展宽度
	PT_2D			m_ptScrollOrigin; //滚动条的原点(0,0)的大地坐标值

	//左右片的绘图层管理者
	CDrawingLayerManager m_laymgrLeft,m_laymgrRight;

	int				m_nUpdateIdxLeft, m_nUpdateIdxRight;

	//绘制环境对象
	HDC				m_hViewDC;
	CStereoDrawingContext *m_pContext;

	//坐标系换算矩阵
	CCoordSys *m_pLeftCS, *m_pRightCS;

	CMenu			m_muDisplay;

	BOOL			m_bRightSametoLeft;

	BOOL m_bDisplayOrder;

	BOOL			m_bTextureForVect;

	CArray<ViewPos*,ViewPos*> m_arrPViewPos;

	int				m_nCurViewPos;
	float			m_fViewPosDx,m_fViewPosDy,m_fViewPosDp,m_fViewPosDq;

	CMyTimer		m_timer;
private:
	int				m_nHorzParallax; //左右视差
	int				m_nVertParallax; //上下视差

	BOOL			m_bCacheReady;

	int				m_nAdjustParams[6];

	CDialog     *m_pDummyDialog;
	BOOL			m_bIsSelfSizing;
};


MyNameSpaceEnd

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEVIEW_H__12B690F7_D261_44FB_A88A_296ECCBBB274__INCLUDED_)
