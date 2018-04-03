#pragma once

#include <string>
#include "PickPixelVirtualControl.h"
#include "VirtualWindowResourceManager.h"
#include "resource.h"
// CPickPixelContral 对话框

class CPickPixelControl : public CDialogEx
{
	DECLARE_DYNAMIC(CPickPixelControl)
public:
	CPickPixelControl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPickPixelControl();
	
	enum { IDD = IDD_PICKPIXELCCONTROL };

public:
	void setImage(std::vector<COri>& vecOri);
	void setNotifyFun(const notifyFun & fun);
	void setCheckToUnCheck(const callbackFun & fun);
	bool isSetNotifyFun();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CPickPixelVirtualControl m_L;
	CPickPixelVirtualControl m_R;
	CVirtualWindowResourceManager m_manage;
private:
	bool needRepaint;
	bool bLoading;
	CPoint lastPoint;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
