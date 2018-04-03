#pragma once
#include "afxdockablepane.h"
#include "simple_handler.h"
#include <memory>
//#include "EmptyDlg.h"
//
//// CDuiTitlePane
//class CEmptyDlg;

class CDuiTitlePane : public CDockablePane
{
	DECLARE_DYNAMIC(CDuiTitlePane)

public:
	CDuiTitlePane();
	virtual ~CDuiTitlePane();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	/*
	** 改变窗口大小，重新调整对话框内容
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	/*
	** 重绘造成无边框效果
	*/
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	/*
	** 重绘鼠标左击
	*/
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnPaint();
protected:
	/**
	* @brief getInstance 返回创建浏览器中需要的客户端单例
	*/
	CefRefPtr<SimpleHandler>& getInstance();

protected:
	/**
	* @brief m_EmptyDlg 空对话框
	*/
	/*CEmptyDlg m_EmptyDlg;*/

	//std::shared_ptr<CEmptyDlg> m_EmptyDlg;

	/**
	* @brief m_bDrag 拖拽
	*/
	bool m_bDrag = false;

	/**
	* @brief m_oldPt 旧的鼠标点击点
	*/
	CPoint m_oldPt = CPoint(0, 0);
};


