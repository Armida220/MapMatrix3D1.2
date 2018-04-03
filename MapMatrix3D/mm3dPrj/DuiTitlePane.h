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
	** �ı䴰�ڴ�С�����µ����Ի�������
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	/*
	** �ػ�����ޱ߿�Ч��
	*/
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	/*
	** �ػ�������
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
	* @brief getInstance ���ش������������Ҫ�Ŀͻ��˵���
	*/
	CefRefPtr<SimpleHandler>& getInstance();

protected:
	/**
	* @brief m_EmptyDlg �նԻ���
	*/
	/*CEmptyDlg m_EmptyDlg;*/

	//std::shared_ptr<CEmptyDlg> m_EmptyDlg;

	/**
	* @brief m_bDrag ��ק
	*/
	bool m_bDrag = false;

	/**
	* @brief m_oldPt �ɵ��������
	*/
	CPoint m_oldPt = CPoint(0, 0);
};


