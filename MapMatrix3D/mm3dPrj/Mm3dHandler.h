#pragma once
#include <include/cef_v8.h>

class CMyAbsOriDlg;
/*
** brief mm3d回调类
*/
class CMm3dHandler
{
public:
	CMm3dHandler();
	virtual ~CMm3dHandler();

	/*
	** brief getInstance 返回回调函数的单例
	*/
	static CMm3dHandler* getInstance();

	/*
	** brief handle 回调类处理函数
	** param funcName 回调函数名
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);

protected:
	
};

