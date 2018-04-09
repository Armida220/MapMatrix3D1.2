#pragma once
#include <include/cef_v8.h>



/*
** brief 图形编辑回调类
*/
class CGraphicEditHandler
{
public:
	CGraphicEditHandler();
	virtual ~CGraphicEditHandler();

	/*
	** brief getInstance 返回回调函数的单例
	*/
	static CGraphicEditHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

