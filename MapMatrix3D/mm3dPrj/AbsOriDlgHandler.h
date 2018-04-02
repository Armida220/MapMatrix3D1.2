#pragma once
#include <include/cef_v8.h>

/*
** brief 绝对定向对话框回调类
*/
class CAbsOriDlgHandler
{
public:
	CAbsOriDlgHandler();
	virtual ~CAbsOriDlgHandler();

	/*
	** brief getInstance 返回绝对定向对话框回调类的单例
	*/
	static CAbsOriDlgHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);

};

