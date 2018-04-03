#pragma once
#include "include/cef_browser.h"
#include <string>

/*
** ½Å±¾Ö´ÐÐÆ÷
*/
class CScriptExecutor
{
private:
	CScriptExecutor();
	virtual ~CScriptExecutor();

public:
	static CScriptExecutor* getInstance();

	/*
	** brief registerBrowser ×¢²áä¯ÀÀÆ÷
	** param browser ×¢²áä¯ÀÀÆ÷
	*/
	virtual bool registerBrowser(CefRefPtr<CefBrowser> browser);

	/*
	** brief excuteScript Ö´ÐÐ½Å±¾
	** param script ×¢²áä¯ÀÀÆ÷
	*/
	virtual void excuteScript(std::string script);

protected:
	/*
	** brief mBrowser ä¯ÀÀÆ÷
	*/
	CefRefPtr<CefBrowser> mBrowser;
};

