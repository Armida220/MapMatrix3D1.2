#pragma once
#include <include/cef_v8.h>

/*
** brief 数据库工程文件的工具栏 回调类
*/
class CDatabaseOpsHandler
{
public:
	CDatabaseOpsHandler();
	virtual ~CDatabaseOpsHandler();

	/*
	** brief getInstance 返回回调函数的单例
	*/
	static CDatabaseOpsHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

