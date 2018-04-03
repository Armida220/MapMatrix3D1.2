// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "stdafx.h"
#include "mm3d_app.h"
#include <string>
#include "mm3d_v8handler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "ScriptExecutor.h"

CMM3dApp::CMM3dApp()
{

}


CMM3dApp::~CMM3dApp(){

}


CefRefPtr<CefRenderProcessHandler> CMM3dApp::GetRenderProcessHandler()
{
	return this;
}

CefRefPtr<CefBrowserProcessHandler> CMM3dApp::GetBrowserProcessHandler()
{
	return this;
}



//////////////////////////////////////
//在此将函数名绑定到js
/////////////////////////////////////
void CMM3dApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// The var type can accept all object or variable
	CefRefPtr<CefV8Value> window = context->GetGlobal();

	// bind value into window[or you can bind value into window sub node]
	/*CScriptExecutor::getInstance()->registerBrowser(browser);*/

	// bind function 
	CefRefPtr<CMM3DV8Handler> pJsHandler(new CMM3DV8Handler(browser));
	registerCallbackFunc(window, _T("openSceneDir"), pJsHandler);

	registerCallbackFunc(window, _T("openOutDir"), pJsHandler);

	registerCallbackFunc(window, _T("produceDom"), pJsHandler);

	registerCallbackFunc(window, _T("produceDsm"), pJsHandler);

	registerCallbackFunc(window, _T("closeWindow"), pJsHandler);

	registerCallbackFunc(window, _T("openClipDir"), pJsHandler);

	registerCallbackFunc(window, _T("clipScene"), pJsHandler);
}



void CMM3dApp::registerCallbackFunc(CefRefPtr<CefV8Value> window, const CefString& cbName, CefRefPtr<CMM3DV8Handler> pJsHandler)
{
	CefRefPtr<CefV8Value> myFunc = CefV8Value::CreateFunction(cbName, pJsHandler);
	window->SetValue(cbName, myFunc, V8_PROPERTY_ATTRIBUTE_NONE);

}

