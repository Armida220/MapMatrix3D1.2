// MM3DDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mm3dDll.h"
#include "MM3DDialog.h"
#include "afxdialogex.h"
#include "mm3d_clientHandler.h"
#include "ScriptExecutor.h"
#include "resource.h"
#include "IOsgDomGenerator.h"
#include <sstream> 
// CMM3DDialog dialog

using namespace std;


vector<string> split_black_space(string s){
	stringstream ss;
	ss << s;//存到字符流里  
	vector<string> vs;
	string word;
	while (ss >> word){//从字符流里读单词，会天然地过滤到多余的空格  
		vs.push_back(word);
	}
	return vs;
}

IMPLEMENT_DYNAMIC(CMM3DDialog, CDialogEx)

CMM3DDialog::CMM3DDialog(CString typeHtml, CWnd* pParent /*=NULL*/)
	: CDialogEx(CMM3DDialog::IDD, pParent),
	mTypeHtml(typeHtml)
{

}

CMM3DDialog::~CMM3DDialog()
{
}

void CMM3DDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMM3DDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMM3DDialog message handlers


BOOL CMM3DDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::SetWindowPos(this->m_hWnd, HWND_BOTTOM, 0, 0, 500, 400, SWP_NOZORDER);
	// TODO:  Add extra initialization here
	// TODO:  在此添加额外的初始化代码
	CefWindowInfo window_info;
	CRect rt;
	GetWindowRect(&rt);

	window_info.SetAsChild(this->GetSafeHwnd(), rt);
	
	CefBrowserSettings browser_settings;
	std::string url = "";
	vector<string> vecCoords;


	TCHAR exePath[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, exePath, _MAX_FNAME);
	TCHAR *pos = _tcsrchr(exePath, _T('\\'));
	if (pos)pos[1] = _T('\0');
	CString path = exePath;
	
	if (mTypeHtml == "1")
	{
		CString htmlFullPath = path + _T("/html/domDialog.html");
		url = htmlFullPath;
	}
	else if (mTypeHtml == "2")
	{
		CString htmlFullPath = path + _T("/html/dsmDialog.html");
		url = htmlFullPath;
	}
	else
	{
		string str = mTypeHtml;
		vector<string> strs = split_black_space(str);
		
		if (strs.size() > 0)
		{
			if (strs[0] == "3")
			{
				CString htmlFullPath = path + _T("/html/clipDialog.html");
				url = htmlFullPath;

				for (int i = 1; i < strs.size(); i++)
				{
					url += "?" + strs[i];
				}
			}
		}
	}

	CefRefPtr<CMm3dClientHandler> handler = new CMm3dClientHandler();
	bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), url, browser_settings, NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CefRefPtr<CMm3dClientHandler> CMM3DDialog::getClientHandler()
{
	CefRefPtr<CMm3dClientHandler> handler = CMm3dClientHandler::GetInstance();
	return handler;
}

void CMM3DDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
}


void CMM3DDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


