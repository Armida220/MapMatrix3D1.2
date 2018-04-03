#include "stdafx.h"
#include "AbsOriDlgHandler.h"
#include <json.h>  
#include "OsgbView.h"
#include <fstream>
#include "qcomm.h"
#include "simple_handler.h"
#include "myAbsOriDlg.h"

using namespace std;

extern COsgbView* getOsgView();

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
template <class Type>
Type stringToNum(const string& str);

CAbsOriDlgHandler::CAbsOriDlgHandler()
{
}


CAbsOriDlgHandler::~CAbsOriDlgHandler()
{
}


CAbsOriDlgHandler* CAbsOriDlgHandler::getInstance()
{
	static CAbsOriDlgHandler h;
	return &h;
}


void CAbsOriDlgHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("absDlg"))
	{
		int order = 3;
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			pOsgbView->GetDocument()->UpdateAllViews(NULL, 70, (CObject*)&order);
			pOsgbView->showAbsOriDlg();
		}
	}
	else if (funcName == _T("loadCtrl"))
	{
		Json::Reader reader;
		Json::Value root;

		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("ctrl Point Files (*.txt)|*.txt;|All Files (*.*)|*.*||"), NULL);

		if (dlg.DoModal() == IDOK)
		{
			CString fileName = dlg.GetPathName();
			string ctrlPtFile = fileName;

			Json::Value children;
			Json::Value child;

			ifstream in(ctrlPtFile.c_str());
			string s;
			int cnt = 0;
			int blockNum = 7;

			while (in >> s)
			{
				cnt++;

				if (cnt % blockNum == 1)
				{
					child["ctrlPtNo"] = s;
				}
				else if (cnt % blockNum == 2)
				{
					child["absX"] = s;
				}
				else if (cnt % blockNum == 3)
				{
					child["absY"] = s;
				}
				else if (cnt % blockNum == 4)
				{
					child["absZ"] = s;
				}
				else if (cnt % blockNum == 5)
				{
					child["relaX"] = s;
				}
				else if (cnt % blockNum == 6)
				{
					child["relaY"] = s;
				}
				else if (cnt % blockNum == 0)
				{
					child["relaZ"] = s;
					child["bCheck"] = "false";
					child["bSel"] = "false";
					child["finish"] = "";

					children.append(child);
				}
			}

			root["children"] = children;

			string str = root.toStyledString();

			SimpleHandler* handler = SimpleHandler::GetInstance();
			list<CefRefPtr<CefBrowser>> lstBr = handler->getBrowserList();
			string scriptWeb = "setCtrlVm(" + str + ")";

			for (auto br : lstBr)
			{
				CefRefPtr<CefFrame> frame = br->GetMainFrame();
				frame->ExecuteJavaScript(scriptWeb, frame->GetURL(), 0);
			}
		}
	}
	else if (funcName == _T("closeAbsDlg"))
	{
		int order = 0;
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			pOsgbView->hideAbsOriDlg();
			pOsgbView->GetDocument()->UpdateAllViews(NULL, 70, (CObject*)&order);
		}
	}
	else if (funcName == _T("sel"))
	{
		if (arguments.size() == 1)
		{
			string ctrlPtNo = arguments[0]->GetStringValue();

			COsgbView* pOsgbView = getOsgView();

			if (pOsgbView != nullptr)
			{
				pOsgbView->GetDocument()->UpdateAllViews(NULL, 80, (CObject*)&ctrlPtNo);
			}

		}
	}
	else if (funcName == _T("absOri"))
	{
		if (arguments.size() == 1)
		{
			string data = arguments[0]->GetStringValue();
			COsgbView* pOsgbView = getOsgView();

			if (pOsgbView != nullptr)
			{
				pOsgbView->GetDocument()->UpdateAllViews(NULL, 81, (CObject*)&data);
			}
		}
	}
	else if (funcName == _T("predictOriPt"))
	{
		string data = arguments[0]->GetStringValue();
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			pOsgbView->GetDocument()->UpdateAllViews(NULL, 85, (CObject*)&data);
		}

	}
	else if (funcName == _T("goto"))
	{
		if (arguments.size() == 3)
		{
			double relaX = arguments[0]->GetDoubleValue();
			double relaY = arguments[1]->GetDoubleValue();
			double relaZ = arguments[2]->GetDoubleValue();
			string data = to_string(relaX) + " " + to_string(relaY) + " " + to_string(relaZ);

			COsgbView* pOsgbView = getOsgView();

			if (pOsgbView != nullptr)
			{
				pOsgbView->GetDocument()->UpdateAllViews(NULL, 82, (CObject*)&data);
			}
		}
	}
	else if (funcName == _T("dragAbs"))
	{
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			if (arguments.size() == 2)
			{
				double x = arguments[0]->GetDoubleValue();
				double y = arguments[1]->GetDoubleValue();
				shared_ptr<CMyAbsOriDlg> absDlg = pOsgbView->getAbsOriDlg();
				::SendMessage(absDlg->GetSafeHwnd(), WM_LBUTTONDOWN, 0, MAKELPARAM(x, y));
			}
		}
	}
	else if (funcName == _T("showPrecision"))
	{
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			pOsgbView->getAbsOriDlg()->showPrecision();
		}
	}
	else if (funcName == _T("backToAbsDlg"))
	{
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			pOsgbView->getAbsOriDlg()->backToAbsDlg();
		}
	}
	else if (funcName == _T("showOriData"))
	{
		if (arguments.size() == 1)
		{
			string oriRes = arguments[0]->GetStringValue();

			COsgbView* pOsgbView = getOsgView();

			if (pOsgbView != nullptr)
			{
				pOsgbView->GetDocument()->UpdateAllViews(NULL, 84, (CObject*)&oriRes);
			}
		}
	}
}