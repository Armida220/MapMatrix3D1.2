// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include "include/cef_client.h"
#include <list>

class SimpleHandler : public CefClient,
                      public CefDisplayHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler,
					  public CefContextMenuHandler,
					  public CefDragHandler,
					  public CefRenderHandler,
					  public CefKeyboardHandler{
public:
  SimpleHandler();
  ~SimpleHandler();

  // Provide access to the single global instance of this object.
  static SimpleHandler* GetInstance();

  // CefClient methods:
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
    return this;
  }

  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
	  return this;
  }

  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE{
	  return this;
  }

  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE{
	  return this;
  }

  // CefDisplayHandler methods:
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) OVERRIDE;

  virtual bool OnTooltip(CefRefPtr<CefBrowser> browser,
	  CefString& text) OVERRIDE;

  // CefLifeSpanHandler methods:
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

  // CefLoadHandler methods:
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) OVERRIDE;

  // CefContextMenuHandler methods:
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
	  CefRefPtr<CefFrame> frame,
	  CefRefPtr<CefContextMenuParams> params,
	  CefRefPtr<CefMenuModel> model) OVERRIDE;

  // CefDragHandler methods:

  // CefRenderHandler methods:
  typedef cef_drag_operations_mask_t DragOperationsMask;

  virtual bool StartDragging(CefRefPtr<CefBrowser> browser,
	  CefRefPtr<CefDragData> drag_data,
	  DragOperationsMask allowed_ops,
	  int x, int y);

  virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);


  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
	  PaintElementType type,
	  const RectList& dirtyRects,
	  const void* buffer,
	  int width, int height);

  // CefKeyBoardEventHandler methods:
  virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
	  const CefKeyEvent& event,
	  CefEventHandle os_event);


  // Request that all existing browser windows close.
  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }

 
private:
  // List of existing browser windows. Only accessed on the CEF UI thread.
  typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
  BrowserList browser_list_;

  bool is_closing_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleHandler);

public:
	inline BrowserList getBrowserList() { return browser_list_; }
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
