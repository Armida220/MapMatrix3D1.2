#pragma once
#include "afxdockablepane.h"
#include "simple_handler.h"

// CProjectPane

/*
** 使用directUI 重写树状图
*/
class CDuiTreeView : public CDockablePane
{
	DECLARE_DYNAMIC(CDuiTreeView)

public:
	CDuiTreeView();
	virtual ~CDuiTreeView();

protected:
	DECLARE_MESSAGE_MAP()

	/*
	** 在创建过程中，使用浏览器占满整个客户端区域
	*/
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	/*
	** 改变窗口大小，重新调整对话框内容
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	/*
	** 去掉边框
	*/
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	/*
	** 显示窗口
	*/
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

protected:

	/**
	* @brief getInstance 返回创建浏览器中需要的客户端单例
	*/
	CefRefPtr<SimpleHandler>& getInstance();



public:
	afx_msg void OnPaint();
};


