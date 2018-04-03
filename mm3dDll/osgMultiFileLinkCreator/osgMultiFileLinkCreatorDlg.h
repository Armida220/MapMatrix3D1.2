
// osgMultiFileLinkCreatorDlg.h : 头文件
//

#pragma once
#include <string>
#include "afxcmn.h"
#include <vector>
#include <string>
#include "afxwin.h"


// CosgMultiFileLinkCreatorDlg 对话框
class CosgMultiFileLinkCreatorDlg : public CDialogEx
{
// 构造
public:
	CosgMultiFileLinkCreatorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_OSGMULTIFILELINKCREATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	CFont m_font;
	CFont m_font1;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void SelectDir(std::string& path);
	void StartcreateLinkForMultiFile();
	void createMultiFileLink(void* ptr);
	void createLink();
public:
	afx_msg void OnBnClickedButtonSelectDir();
	CString m_path;
	CProgressCtrl m_progress;
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CButton m_EXIT;
	afx_msg void OnBnClickedButtonExit1();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};

//自定义索引生成类
class COsgMultiFileLinkCreator 
{
public:
	void createLinkForMultiFile();

	void createLink();

	CosgMultiFileLinkCreatorDlg* linkDlg;
protected:
	static void createMultiFileLink(void* ptr);
	void StartcreateLinkForMultiFile();
	void SearchFile(std::string dir, std::vector<std::string>* vecStrFileName);
protected:
	
	std::string strFilePath;
};