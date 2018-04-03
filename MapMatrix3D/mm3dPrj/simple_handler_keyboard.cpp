// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "StdAfx.h"
#include "simple_handler.h"

#include <string>
#include <windows.h>

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"

bool SimpleHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser,
	const CefKeyEvent& event,
	CefEventHandle os_event) {
	return false;
}