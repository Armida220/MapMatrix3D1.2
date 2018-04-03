#pragma once
#include <include/cef_v8.h>
#include "include/internal/cef_ptr.h"

class CDrawPointHandler;

class COsgbView;
/**
* @brief Execute v8引擎回调类，所有的网页端回调都经过这里
*/
class CV8JsHandler : public CefV8Handler
{
public:
	CV8JsHandler();
	virtual ~CV8JsHandler(void);

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

	IMPLEMENT_REFCOUNTING(CV8JsHandler);


protected:

};
