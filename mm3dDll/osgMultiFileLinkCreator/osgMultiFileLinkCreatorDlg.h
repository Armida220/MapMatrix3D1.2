
// osgMultiFileLinkCreatorDlg.h : ͷ�ļ�
//

#pragma once
#include <string>
#include "afxcmn.h"
#include <vector>
#include <string>
#include "afxwin.h"


// CosgMultiFileLinkCreatorDlg �Ի���
class CosgMultiFileLinkCreatorDlg : public CDialogEx
{
// ����
public:
	CosgMultiFileLinkCreatorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_OSGMULTIFILELINKCREATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	CFont m_font;
	CFont m_font1;
	// ���ɵ���Ϣӳ�亯��
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

//�Զ�������������
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