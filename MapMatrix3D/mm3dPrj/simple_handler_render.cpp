// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "StdAfx.h"
#include "simple_handler.h"

#include <string>
#include <windows.h>

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"


bool SimpleHandler::StartDragging(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefDragData> drag_data,
	DragOperationsMask allowed_ops,
	int x, int y) {

	if (x > 0)
	{
		return false;
	}


}

bool SimpleHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	if (rect.x > 0)
	{
		return false;
	}
}

void SimpleHandler::OnPaint(CefRefPtr<CefBrowser> browser,
	PaintElementType type,
	const RectList& dirtyRects,
	const void* buffer,
	int width, int height)
{
	if (width > 0)
	{
		return;
	}
}