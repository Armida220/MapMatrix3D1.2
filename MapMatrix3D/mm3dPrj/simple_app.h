// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include "include/cef_app.h"
#include <include/cef_v8.h>
#include "V8JsHandler.h"

/**
* @brief ��Ϊcef�������Ӧ�öˣ�����
* @brief �����������Ⱦʱ��Ļص��¼�
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
	* @brief ��Ϊcef�������Ӧ�öˣ�����
	* @return ��ȡ��Ⱦ����ص���
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
	* @brief ע��ص�����
	* @param window����
	* @param cbName �ص�����
	* @param pJsHandler v8�ص���
	*/
	void registerCallbackFunc(CefRefPtr<CefV8Value> window, const CefString& cbName, CefRefPtr<CV8JsHandler> pJsHandler);

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(SimpleApp);

};


