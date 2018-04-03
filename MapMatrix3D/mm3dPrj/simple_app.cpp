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

	registerCallbackFunc(window, _T("reCalGrdPt"), pJsHandler);

	registerCallbackFunc(window, _T("reCalFakePt"), pJsHandler);

	registerCallbackFunc(window, _T("revertOps"), pJsHandler);

	registerCallbackFunc(window, _T("loadPrj"), pJsHandler);

	registerCallbackFunc(window, _T("unloadPrj"), pJsHandler);

	registerCallbackFunc(window, _T("closePrj"), pJsHandler);

	registerCallbackFunc(window, _T("newDocument"), pJsHandler);

	registerCallbackFunc(window, _T("openFile"), pJsHandler);

	registerCallbackFunc(window, _T("saveFile"), pJsHandler);

	registerCallbackFunc(window, _T("openOsgbView"), pJsHandler);

	registerCallbackFunc(window, _T("adjustOsgbArea"), pJsHandler);

	registerCallbackFunc(window, _T("adjustTileVert"), pJsHandler);

	registerCallbackFunc(window, _T("closeMainWin"), pJsHandler);

	registerCallbackFunc(window, _T("fullScreen"), pJsHandler);

	registerCallbackFunc(window, _T("minusMainWin"), pJsHandler);

	registerCallbackFunc(window, _T("selectWin"), pJsHandler);

	registerCallbackFunc(window, _T("showProperty"), pJsHandler);

	registerCallbackFunc(window, _T("showOutput"), pJsHandler);

	registerCallbackFunc(window, _T("drawDot"), pJsHandler);

	registerCallbackFunc(window, _T("drawDot"), pJsHandler);

	registerCallbackFunc(window, _T("drawVectorDot"), pJsHandler);

	registerCallbackFunc(window, _T("drawHeightDot"), pJsHandler);

	registerCallbackFunc(window, _T("drawAutoHeightDot"), pJsHandler);

	registerCallbackFunc(window, _T("drawLine"), pJsHandler);

	registerCallbackFunc(window, _T("drawLineRect"), pJsHandler);

	registerCallbackFunc(window, _T("drawUnpararell"), pJsHandler);

	registerCallbackFunc(window, _T("drawRegularPoly"), pJsHandler);

	registerCallbackFunc(window, _T("drawPararell"), pJsHandler);

	registerCallbackFunc(window, _T("drawPararellSingle"), pJsHandler);

	registerCallbackFunc(window, _T("drawDoubleLine"), pJsHandler);

	registerCallbackFunc(window, _T("drawPoly"), pJsHandler);

	registerCallbackFunc(window, _T("drawComplxPoly"), pJsHandler);

	registerCallbackFunc(window, _T("drawAutoCompletePoly"), pJsHandler);

	registerCallbackFunc(window, _T("drawCircle2Pt"), pJsHandler);

	registerCallbackFunc(window, _T("drawCircle3Pt"), pJsHandler);

	registerCallbackFunc(window, _T("drawCircleRadius"), pJsHandler);

	registerCallbackFunc(window, _T("drawLineStr"), pJsHandler);

	registerCallbackFunc(window, _T("drawCurve"), pJsHandler);

	registerCallbackFunc(window, _T("drawArc"), pJsHandler);

	registerCallbackFunc(window, _T("draw3Arc"), pJsHandler);

	registerCallbackFunc(window, _T("select"), pJsHandler);

	registerCallbackFunc(window, _T("selectMove"), pJsHandler);

	registerCallbackFunc(window, _T("copy"), pJsHandler);

	registerCallbackFunc(window, _T("rotateCopy"), pJsHandler);

	registerCallbackFunc(window, _T("delete"), pJsHandler);

	registerCallbackFunc(window, _T("rotate"), pJsHandler);

	registerCallbackFunc(window, _T("mirror"), pJsHandler);

	registerCallbackFunc(window, _T("modifyCurve"), pJsHandler);

	registerCallbackFunc(window, _T("modifyInterCurve"), pJsHandler);

	registerCallbackFunc(window, _T("modifyLink"), pJsHandler);

	registerCallbackFunc(window, _T("modifySmooth"), pJsHandler);

	registerCallbackFunc(window, _T("selObj"), pJsHandler);

	registerCallbackFunc(window, _T("unSelObj"), pJsHandler);

	registerCallbackFunc(window, _T("selAll"), pJsHandler);

	registerCallbackFunc(window, _T("unSelAll"), pJsHandler);

	registerCallbackFunc(window, _T("delObjPt"), pJsHandler);

	registerCallbackFunc(window, _T("delAll"), pJsHandler);

	registerCallbackFunc(window, _T("markObj"), pJsHandler);

	registerCallbackFunc(window, _T("markAll"), pJsHandler);

	registerCallbackFunc(window, _T("clearResult"), pJsHandler);

	registerCallbackFunc(window, _T("modifyReverse"), pJsHandler);

	registerCallbackFunc(window, _T("modifyClose"), pJsHandler);

	registerCallbackFunc(window, _T("modifyCloseTole"), pJsHandler);

	registerCallbackFunc(window, _T("modifyRectify"), pJsHandler);

	registerCallbackFunc(window, _T("modifyBreak"), pJsHandler);

	registerCallbackFunc(window, _T("modifyIntesection"), pJsHandler);

	registerCallbackFunc(window, _T("modifyCombine"), pJsHandler);

	registerCallbackFunc(window, _T("modifySeperate"), pJsHandler);

	registerCallbackFunc(window, _T("modifyLineSerial"), pJsHandler);

	registerCallbackFunc(window, _T("modifyCutPart"), pJsHandler);

	registerCallbackFunc(window, _T("modifyExtent"), pJsHandler);

	registerCallbackFunc(window, _T("modifyFormatObj"), pJsHandler);

	registerCallbackFunc(window, _T("modifyZ"), pJsHandler);

	registerCallbackFunc(window, _T("modifyCompress"), pJsHandler);

	registerCallbackFunc(window, _T("modifyParallel"), pJsHandler);

	registerCallbackFunc(window, _T("modifyTrim"), pJsHandler);

	registerCallbackFunc(window, _T("modifyMove"), pJsHandler);

	registerCallbackFunc(window, _T("produceDom"), pJsHandler);

	registerCallbackFunc(window, _T("produceDsm"), pJsHandler);

	registerCallbackFunc(window, _T("repair"), pJsHandler);

	registerCallbackFunc(window, _T("clip"), pJsHandler);

	registerCallbackFunc(window, _T("absDlg"), pJsHandler);

	registerCallbackFunc(window, _T("loadCtrl"), pJsHandler);

	registerCallbackFunc(window, _T("closeAbsDlg"), pJsHandler);

	registerCallbackFunc(window, _T("sel"), pJsHandler);

	registerCallbackFunc(window, _T("absOri"), pJsHandler);

	registerCallbackFunc(window, _T("goto"), pJsHandler);

	registerCallbackFunc(window, _T("dragAbs"), pJsHandler);

	registerCallbackFunc(window, _T("showPrecision"), pJsHandler);

	registerCallbackFunc(window, _T("backToAbsDlg"), pJsHandler);

	registerCallbackFunc(window, _T("showOriData"), pJsHandler);

	registerCallbackFunc(window, _T("predictOriPt"), pJsHandler);
	
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

