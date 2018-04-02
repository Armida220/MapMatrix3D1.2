#include "stdafx.h"
#include "ScriptExecutor.h"
#include "include/cef_v8.h"
#include "MM3DDialog.h"
using namespace std;

CScriptExecutor::CScriptExecutor()
{
}


CScriptExecutor::~CScriptExecutor()
{
}


CScriptExecutor* CScriptExecutor::getInstance()
{
	static CScriptExecutor executor;
	return &executor;
}

bool CScriptExecutor::registerBrowser(CefRefPtr<CefBrowser> browser)
{
	mBrowser = browser;
	return true;
}

void CScriptExecutor::excuteScript(string script)
{
	CefRefPtr<CMm3dClientHandler> handler = CMM3DDialog::getClientHandler();
	auto lstBr = handler->getBrowserList();

	for (auto br : lstBr)
	{
		CefString javascriptTxt(script);
		CefRefPtr<CefFrame> frame = br->GetMainFrame();
		frame->ExecuteJavaScript(javascriptTxt, frame->GetURL(), 0);
	}
	
}