#pragma once
#include <include/cef_v8.h>
#include "include/internal/cef_ptr.h"

/**
* @brief Execute v8引擎回调类，所有的网页端回调都经过这里
*/
class CMM3DV8Handler : public CefV8Handler
{
public:
	CMM3DV8Handler(CefRefPtr<CefBrowser> browser);
	virtual ~CMM3DV8Handler(void);

public:
	/**
	* @brief Execute javaScript 回调函数，所有的网页端回调都经过这里
	* @param func_name 网页中回调的函数名称
	* @param arguments 网页中回调函数的参数
	*/
	virtual bool Execute(const CefString& func_name,
						  CefRefPtr<CefV8Value> object,
						  const CefV8ValueList& arguments,
						  CefRefPtr<CefV8Value>& retval,
						  CefString& exception) OVERRIDE;

	IMPLEMENT_REFCOUNTING(CMM3DV8Handler);

protected:
	CefRefPtr<CefBrowser> mBrowser;
};
