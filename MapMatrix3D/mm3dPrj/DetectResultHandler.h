#pragma once
#include <include/cef_v8.h>

/*
** brief 观察结果回调类
*/
class CDetectResultHandler
{
public:
	CDetectResultHandler();
	virtual ~CDetectResultHandler();

	/*
	** brief getInstance 返回回调函数的单例
	*/
	static CDetectResultHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

