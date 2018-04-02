#pragma once
#include "afxdockablepane.h"
#include "simple_handler.h"

// CProjectPane

/*
** ʹ��directUI ��д��״ͼ
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
	** �ڴ��������У�ʹ�������ռ�������ͻ�������
	*/
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	/*
	** �ı䴰�ڴ�С�����µ����Ի�������
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	/*
	** ȥ���߿�
	*/
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	/*
	** ��ʾ����
	*/
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

protected:

	/**
	* @brief getInstance ���ش������������Ҫ�Ŀͻ��˵���
	*/
	CefRefPtr<SimpleHandler>& getInstance();



public:
	afx_msg void OnPaint();
};


