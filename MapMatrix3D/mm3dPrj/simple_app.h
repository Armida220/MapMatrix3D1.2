// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include "include/cef_app.h"
#include <include/cef_v8.h>
#include "V8JsHandler.h"

/**
* @brief 作为cef浏览器的应用端，可以
* @brief 处理浏览和渲染时候的回调事件
*/
class SimpleApp : public CefApp,
		public CefBrowserProcessHandler,
		public CefRenderProcessHandler 
{
public:
	SimpleApp();

	virtual ~SimpleApp();

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;

	/**
	* @brief 作为cef浏览器的应用端，可以
	* @return 获取渲染处理回调类
	*/
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;
	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);

	virtual void OnWebKitInitialized();


public:
	void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line);

	void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info);

	void OnScheduleMessagePumpWork(int64 delay);

protected:
	/**
	* @brief 注册回调函数
	* @param window窗口
	* @param cbName 回调函数
	* @param pJsHandler v8回调类
	*/
	void registerCallbackFunc(CefRefPtr<CefV8Value> window, const CefString& cbName, CefRefPtr<CV8JsHandler> pJsHandler);

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(SimpleApp);

};


