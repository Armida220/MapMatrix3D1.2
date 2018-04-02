// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "StdAfx.h"
#include "simple_handler.h"

#include <string>
#include <windows.h>

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"

void SimpleHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	CefRefPtr<CefMenuModel> model) {

	if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) {
		// Add a separator if the menu already has items.
		if (model->GetCount() > 0)
		{
			model->Clear();
			//model->AddSeparator();
		}
	}
}