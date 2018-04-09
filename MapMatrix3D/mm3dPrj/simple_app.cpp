// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "stdafx.h"
#include "simple_app.h"

#include <string>

#include "simple_handler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "V8JsHandler.h"

SimpleApp::SimpleApp()
{
}


SimpleApp::~SimpleApp(){

}


CefRefPtr<CefRenderProcessHandler> SimpleApp::GetRenderProcessHandler()
{
	return this;
}

CefRefPtr<CefBrowserProcessHandler> SimpleApp::GetBrowserProcessHandler()
{
	return this;
}



void SimpleApp::OnContextInitialized() {

}

//////////////////////////////////////
//在此将函数名绑定到js
/////////////////////////////////////
void SimpleApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// The var type can accept all object or variable
	CefRefPtr<CefV8Value> window = context->GetGlobal();

	// bind value into window[or you can bind value into window sub node]

	// bind function 
	CefRefPtr<CV8JsHandler> pJsHandler(new CV8JsHandler());

	//标题回调
	{
		registerCallbackFunc(window, _T("selectWin"), pJsHandler);
		registerCallbackFunc(window, _T("minusMainWin"), pJsHandler);
		registerCallbackFunc(window, _T("closeMainWin"), pJsHandler);
	}

	//工程操作操作回调
	{
		registerCallbackFunc(window, _T("loadPrj"), pJsHandler);
		registerCallbackFunc(window, _T("unloadPrj"), pJsHandler);
		registerCallbackFunc(window, _T("closePrj"), pJsHandler);
		registerCallbackFunc(window, _T("newDocument"), pJsHandler);
		registerCallbackFunc(window, _T("openFile"), pJsHandler);
		registerCallbackFunc(window, _T("saveFile"), pJsHandler);
	}

	//画点
	{
		registerCallbackFunc(window, _T("drawDot"), pJsHandler);
		registerCallbackFunc(window, _T("drawVectorDot"), pJsHandler);
		registerCallbackFunc(window, _T("drawHeightDot"), pJsHandler);
		registerCallbackFunc(window, _T("drawAutoHeightDot"), pJsHandler);
		registerCallbackFunc(window, _T("drawSurfacePoint"), pJsHandler);
	}

	//画线
	{
		registerCallbackFunc(window, _T("drawLine"), pJsHandler);
		registerCallbackFunc(window, _T("drawRect"), pJsHandler);
		registerCallbackFunc(window, _T("drawUnpararell"), pJsHandler);
		registerCallbackFunc(window, _T("drawRegularPoly"), pJsHandler);
		registerCallbackFunc(window, _T("drawPararell"), pJsHandler);
		registerCallbackFunc(window, _T("drawPararellSingle"), pJsHandler);
		registerCallbackFunc(window, _T("drawDoubleLine"), pJsHandler);
	}

	//画面
	{
		registerCallbackFunc(window, _T("drawPoly"), pJsHandler);
		registerCallbackFunc(window, _T("drawComplxPoly"), pJsHandler);
		registerCallbackFunc(window, _T("drawAutoCompletePoly"), pJsHandler);
	}

	//画面
	{
		registerCallbackFunc(window, _T("drawCircle2Pt"), pJsHandler);
		registerCallbackFunc(window, _T("drawCircle3Pt"), pJsHandler);
		registerCallbackFunc(window, _T("drawCircleRadius"), pJsHandler);
	}

	//画线串
	{
		registerCallbackFunc(window, _T("drawLineStr"), pJsHandler);
		registerCallbackFunc(window, _T("drawCurve"), pJsHandler);
		registerCallbackFunc(window, _T("drawArc"), pJsHandler);
		registerCallbackFunc(window, _T("draw3Arc"), pJsHandler);
	}

	//倾斜视图
	{
		registerCallbackFunc(window, _T("openOsgbView"), pJsHandler);
	}

	//图形编辑
	{
		registerCallbackFunc(window, _T("move"), pJsHandler);
		registerCallbackFunc(window, _T("delete"), pJsHandler);
		registerCallbackFunc(window, _T("copy"), pJsHandler);
		registerCallbackFunc(window, _T("rotate"), pJsHandler);
		registerCallbackFunc(window, _T("copyRotate"), pJsHandler);
		registerCallbackFunc(window, _T("parallelMove"), pJsHandler);
		registerCallbackFunc(window, _T("editVert"), pJsHandler);
		registerCallbackFunc(window, _T("insertVert"), pJsHandler);
		registerCallbackFunc(window, _T("delVert"), pJsHandler);
		registerCallbackFunc(window, _T("closed"), pJsHandler);
		registerCallbackFunc(window, _T("reverse"), pJsHandler);
		registerCallbackFunc(window, _T("curveSmooth"), pJsHandler);
		registerCallbackFunc(window, _T("rectify"), pJsHandler);
		registerCallbackFunc(window, _T("seperate"), pJsHandler);
		registerCallbackFunc(window, _T("break"), pJsHandler);
		registerCallbackFunc(window, _T("replaceCurve"), pJsHandler);
		registerCallbackFunc(window, _T("replaceCurveOverlap"), pJsHandler);

	}


}


void SimpleApp::OnWebKitInitialized()
{

}


void SimpleApp::OnBeforeChildProcessLaunch(
	CefRefPtr<CefCommandLine> command_line)
{

}

void SimpleApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
{

}

void SimpleApp::OnScheduleMessagePumpWork(int64 delay)
{
}

void SimpleApp::registerCallbackFunc(CefRefPtr<CefV8Value> window, const CefString& cbName, CefRefPtr<CV8JsHandler> pJsHandler)
{
	CefRefPtr<CefV8Value> myFunc = CefV8Value::CreateFunction(cbName, pJsHandler);
	window->SetValue(cbName, myFunc, V8_PROPERTY_ATTRIBUTE_NONE);

}

