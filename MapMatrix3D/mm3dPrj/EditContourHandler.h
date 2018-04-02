#pragma once
#include <include/cef_v8.h>

/*
** brief 编辑等高线回调类
*/
class CEditContourHandler
{
public:
	CEditContourHandler();
	virtual ~CEditContourHandler();

	/*
	** brief getInstance 返回回调函数类的单例
	*/
	static CEditContourHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

