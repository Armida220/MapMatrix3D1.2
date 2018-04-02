#pragma once
#include "resource.h"
#include <vector>
#include <string>
//#include "include/cef_command_line.h"
#include "Ori.h"
#include "afxcmn.h"
// CShowImgDlg dialog
class CPickPixelControl;
class COsgbView;
class CShowImgBar : public CDockablePane
{
public:
	CShowImgBar();
	~CShowImgBar();
protected:
	void showImgCtrl(std::vector<COri> & vecImg,
		int state, COsgbView * posgb);

	/**
	* @brief getInstance 返回创建浏览器中需要的客户端单例
	*/
	//CefRefPtr<SimpleHandler>& getInstance();

	/**
	* @brief initBrowserList 初始化浏览器列表
	*/
	void initBrowserList();

	CListCtrl m_lc;
	afx_msg void OnClickList2(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
	CPickPixelControl * m_pPPC;
public:
	void showImage(std::vector<COri>& vecOri, int state, COsgbView * posgb);

	/**
	* @brief setCheck 设置checkbox控件的状态
	* @param checkStatus checkbox的控件的状态
	*/
	void setCheck(int checkStatus);
	int lIdx() const { return m_idxL; }
	int rIdx() const { return m_idxR; }
	void initBrowser();
	int getState() { return m_state; }
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
protected:
	CBrush m_brush;
	void Adjust();
	/**
	* @brief m_rect 显示对话框的windows上的区域
	*/
	CRect m_rect;

	/**
	* @brief bInit 浏览器是否已经初始化了
	*/
	bool bInit = false;

	/**
	* @brief m_idxL 左侧浏览器显示的索引
	*/
	int m_idxL;
	/**
	* @brief m_idxR 右侧浏览器显示的索引
	*/
	int m_idxR;

	void setL(int idx);
	void setR(int idx);
//	std::string getUrl(int idx);

	int m_state;
//	std::string m_htmlName;

	COsgbView* getOsgView();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
