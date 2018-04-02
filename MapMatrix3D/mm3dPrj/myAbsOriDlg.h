#pragma once
#include "resource.h"
#include "simple_handler.h"
// CMyAbsOriDlg dialog

class CMyAbsOriDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMyAbsOriDlg)

public:
	CMyAbsOriDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMyAbsOriDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DOM };

	/*
	** brief 重新加载精度报告
	*/
	void reloadReportHtml(std::string reportStr);

	/*
	** brief 显示精度报告
	*/
	void showPrecision();

	/*
	** brief 回退到刺点对话框
	*/
	void backToAbsDlg();

	/*
	** brief 设置vm数据
	*/
	void setVmData(std::string vmDataStr);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

protected:
	/*
	**@brief 精度报告内容
	*/
	std::string mReportStr;

	/**
	* @brief m_bDrag 拖拽
	*/
	bool m_bDrag = false;

	/**
	* @brief m_oldPt 旧的鼠标点击点
	*/
	CPoint m_oldPt = CPoint(0, 0);

protected:
	/**
	* @brief getInstance 返回创建浏览器中需要的客户端单例
	*/
	CefRefPtr<SimpleHandler>& getInstance();

};
