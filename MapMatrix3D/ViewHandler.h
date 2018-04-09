#pragma once
#include <include/cef_v8.h>

/*
** brief 视图回调类
*/
class CViewHandler
{
public:
	CViewHandler();
	virtual ~CViewHandler();

	/*
	** brief getInstance 返回回调函数的单例
	*/
	static CViewHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

