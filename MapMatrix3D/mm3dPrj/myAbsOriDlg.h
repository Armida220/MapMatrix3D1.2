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
	** brief ���¼��ؾ��ȱ���
	*/
	void reloadReportHtml(std::string reportStr);

	/*
	** brief ��ʾ���ȱ���
	*/
	void showPrecision();

	/*
	** brief ���˵��̵�Ի���
	*/
	void backToAbsDlg();

	/*
	** brief ����vm����
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
	**@brief ���ȱ�������
	*/
	std::string mReportStr;

	/**
	* @brief m_bDrag ��ק
	*/
	bool m_bDrag = false;

	/**
	* @brief m_oldPt �ɵ��������
	*/
	CPoint m_oldPt = CPoint(0, 0);

protected:
	/**
	* @brief getInstance ���ش������������Ҫ�Ŀͻ��˵���
	*/
	CefRefPtr<SimpleHandler>& getInstance();

};
