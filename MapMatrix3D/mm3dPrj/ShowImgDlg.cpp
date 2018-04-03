// ShowImgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShowImgDlg.h"
#include "afxdialogex.h"
#include "simple_handler.h"
#include "include/wrapper/cef_helpers.h"
#include <algorithm>
#include "Ori.h"
#include "OsgbView.h"
#include "PickPixelControl.h"
#include "MsgSender.h"
const int L_in_lc = 1;
const int R_in_lc = 2;
const int listCtlWidth = 150;

using namespace std;

/**
* @brief replace_all 将字符串中的所有出现的字符替换成另一个字符
* @param s 字符串
* @param t 要被替换的字符
* @param w 用来替换的字符
*/
void replace_all(std::string& s, std::string const & t, std::string const & w)
{
	string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != std::string::npos)
	{ // found   
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
}

/**
* @brief MBSCToCEF 将多字节情况下的url转换成可以被浏览器cef识别的url
* @param mbcsStr 多字节的url
* @return 返回可以被浏览器识别的url
*/
static char* MBSCToCEF(const char* mbcsStr)
{
	wchar_t*  wideStr;
	char*   utf8Str;
	int   charLen;

	charLen = MultiByteToWideChar(CP_UTF8, 0, mbcsStr, -1, NULL, 0);
	charLen = strlen(mbcsStr) + 1;
	wideStr = (wchar_t*)malloc(sizeof(wchar_t)*charLen);
	MultiByteToWideChar(CP_ACP, 0, mbcsStr, -1, wideStr, charLen);

	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);

	utf8Str = (char*)malloc(charLen);

	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);

	free(wideStr);
	return utf8Str;
}

//模板函数：将常用的数值类型转换为string类型变量（此方法具有普遍适用性）  
template <class Type>
string NumTostring(const Type num)
{
	stringstream ss;
	ss << num;

	string output;
	ss >> output;
	return output;
}



BEGIN_MESSAGE_MAP(CShowImgBar, CDockablePane)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickList2)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_NCPAINT()
	ON_WM_MOVE()
	ON_WM_MOVING()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

CShowImgBar::CShowImgBar() :m_pPPC(nullptr)
{
}

CShowImgBar::~CShowImgBar()
{
	if (m_pPPC)
		delete m_pPPC;
}

void CShowImgBar::showImgCtrl(std::vector<COri> & vecImg, int state, COsgbView * posgb)
{
	initBrowser();
	m_state = state;

	//根据影像名称创建控件，并显示影像
	int ctrlNum = vecImg.size();

	if (ctrlNum < 2)
	{
		return;
	}

	CRect rect;
	GetClientRect(&rect);

	int index = 0;
	int rcwidth = rect.Width();
	if (m_lc.GetItemCount() != 0)
	{
		for (int i = 0; i < m_lc.GetItemCount(); i++)
		{
			m_lc.SetItemData(i, 0);
		}
	}
	m_lc.DeleteAllItems();


	m_idxL = -1;
	m_idxR = -1;
	m_pPPC->setImage(vecImg);
	setCheck(state);
	for (index = 0; index < vecImg.size(); index++)
	{
		auto & img = vecImg[index];

		m_lc.InsertItem(index, img.getFileName(img.imgName).c_str());
		//char* finalUrl = COriToUrl(img, m_state, htmlName);
		m_lc.SetItemData(index, (DWORD_PTR)(&img));
	}
	Adjust();
	setL(0);
	setR(1);
	//if (!m_pPPC->isSetNotifyFun())
	{
		
		m_pPPC->setNotifyFun([posgb](bool b, std::string in, double ix, double iy){
			if (b)
			{
				posgb->reCalFront(in, ix, iy);
			}
			else
			{
				posgb->changeFakePt(in, ix, iy);
			}
		});
		m_pPPC->setCheckToUnCheck([posgb](){posgb->SendMessage(WM_BACK); });
	}
}


void CShowImgBar::initBrowserList()
{
//	CefRefPtr<SimpleHandler> handler = getInstance();

	CRect rect;
	GetClientRect(&rect);
	if (!m_pPPC)
	{
		m_pPPC = new CPickPixelControl(this);
		m_pPPC->Create(m_pPPC->IDD, this);
		m_pPPC->ShowWindow(true);
		m_pPPC->MoveWindow(rect.left, rect.top, rect.Width() - listCtlWidth, rect.Height());
	}

	m_lc.InsertColumn(0, "ID", LVCFMT_LEFT, 80);
	m_lc.InsertColumn(L_in_lc, "L", LVCFMT_LEFT, 20);
	m_lc.InsertColumn(R_in_lc, "R", LVCFMT_LEFT, 20);
	m_lc.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lc.ShowWindow(SW_HIDE);
}

void CShowImgBar::OnClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// 点击第一列
	if (pNMItemActivate->iSubItem == 0) return;
	// 该行已有勾选
	if (m_idxL == pNMItemActivate->iItem || m_idxR == pNMItemActivate->iItem) return;

	if (pNMItemActivate->iSubItem == L_in_lc)
	{
		setL(pNMItemActivate->iItem);
	}
	if (pNMItemActivate->iSubItem == R_in_lc)
	{
		setR(pNMItemActivate->iItem);
	}


	*pResult = 0;
}

void CShowImgBar::showImage(std::vector<COri>& vecOri, int state, COsgbView * posgb)
{
// 	char moduleName[_MAX_PATH] = { 0 };
// 	GetModuleFileName(NULL, moduleName, _MAX_FNAME);

	showImgCtrl(vecOri,  state, posgb);
}

void CShowImgBar::setCheck(int checkStatus)
{
	m_pPPC->m_L.SetState(checkStatus);
	m_pPPC->m_R.SetState(checkStatus);
}

void CShowImgBar::initBrowser()
{
	if (bInit == false)
	{
		initBrowserList();
	}
	bInit = true;
}

int CShowImgBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	RECT rc;
	if (!m_lc.Create(LVS_REPORT | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP,
		rc, this, IDC_LIST2))
		return -1;
	initBrowser();
	return 0;
}

void CShowImgBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	Adjust();
}

void CShowImgBar::OnMove(int x, int y)
{
	CDockablePane::OnMove(x, y);

	Adjust();
}

void CShowImgBar::OnMoving(UINT nSide, LPRECT lpRect)
{
	CDockablePane::OnMoving(nSide, lpRect);

	Adjust();
}

void CShowImgBar::Adjust()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rect;
	GetClientRect(rect);
	int cx = rect.Width();
	int cy = rect.Height();
	int num = 0;
	int width = cx;
	if (m_lc.GetItemCount() > 2)
	{
		width -= listCtlWidth;
		m_lc.ShowWindow(SW_SHOW);
		m_lc.MoveWindow(rect.right - listCtlWidth, 0, listCtlWidth, rect.Height());
	}
	else
	{
		m_lc.ShowWindow(SW_HIDE);
	}

	if (m_pPPC)
	{
		m_pPPC->MoveWindow(0, 0, width, cy);
	}
}



void CShowImgBar::setL(int idx)
{
	if (m_idxL == idx) return;
	if (m_idxL != -1) m_lc.SetItemText(m_idxL, L_in_lc, "");
	m_idxL = idx;
	m_lc.SetItemText(m_idxL, L_in_lc, _T("√"));
	if (m_pPPC) m_pPPC->m_L.SetIdx(idx);
}

void CShowImgBar::setR(int idx)
{
	if (m_idxR == idx) return;
	if (m_idxR != -1) m_lc.SetItemText(m_idxR, R_in_lc, "");
	m_idxR = idx;
	m_lc.SetItemText(m_idxR, R_in_lc, _T("√"));

	if (m_pPPC) m_pPPC->m_R.SetIdx(idx);

}

COsgbView* CShowImgBar::getOsgView()
{
	POSITION POS;
	POS = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate*   pDocTemplate = AfxGetApp()->GetNextDocTemplate(POS);
	POS = pDocTemplate->GetFirstDocPosition();
	CDocument*   pDoc = pDocTemplate->GetNextDoc(POS);
	POS = pDoc->GetFirstViewPosition();
	CView* pView;
	while (POS != NULL)
	{
		pView = (CView*)pDoc->GetNextView(POS);
		if (pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			COsgbView* pOsgView = DYNAMIC_DOWNCAST(COsgbView, pView);
			return pOsgView;
		}
	}

	return nullptr;
}



void CShowImgBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);

	COsgbView * posgb = getOsgView();
	if (posgb) posgb->UpdateImgVisible();
}
