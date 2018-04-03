#pragma once
#include <string>
#include <include/cef_v8.h>

/*
** brief 画点命令的回调
*/
class CDrawPointHandler
{
public:
	CDrawPointHandler();
	virtual ~CDrawPointHandler();

	/*
	** brief getInstance 返回回调函数的单例
	*/
	static CDrawPointHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

