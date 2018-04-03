#pragma once
#include "include/cef_browser.h"
#include <string>

/*
** �ű�ִ����
*/
class CScriptExecutor
{
private:
	CScriptExecutor();
	virtual ~CScriptExecutor();

public:
	static CScriptExecutor* getInstance();

	/*
	** brief registerBrowser ע�������
	** param browser ע�������
	*/
	virtual bool registerBrowser(CefRefPtr<CefBrowser> browser);

	/*
	** brief excuteScript ִ�нű�
	** param script ע�������
	*/
	virtual void excuteScript(std::string script);

protected:
	/*
	** brief mBrowser �����
	*/
	CefRefPtr<CefBrowser> mBrowser;
};

