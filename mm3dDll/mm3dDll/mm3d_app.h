// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include "include/cef_app.h"
#include <include/cef_v8.h>
#include "mm3d_v8handler.h"
/**
* @brief ��Ϊcef�������Ӧ�öˣ�����
* @brief �����������Ⱦʱ��Ļص��¼�
*/
class CMM3dApp : public CefApp,
		public CefBrowserProcessHandler,
		public CefRenderProcessHandler 
{
public:
	CMM3dApp();

	virtual ~CMM3dApp();


	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;

	/**
	* @brief ��Ϊcef�������Ӧ�öˣ�����
	* @return ��ȡ��Ⱦ����ص���
	*/
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;


protected:
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);

	/**
	* @brief ע��ص�����
	* @param window����
	* @param cbName �ص�����
	* @param pJsHandler v8�ص���
	*/
	void registerCallbackFunc(CefRefPtr<CefV8Value> window, const CefString& cbName, CefRefPtr<CMM3DV8Handler> pJsHandler);

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CMM3dApp);

};


