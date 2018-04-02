#pragma once
#include "include/cef_client.h"
#include <list>

class CMm3dClientHandler : public CefClient,
	public CefContextMenuHandler,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRequestHandler,
	public CefJSDialogHandler,
	public CefRenderHandler
{
public:
	CMm3dClientHandler();
	virtual ~CMm3dClientHandler();

public:
	CefRefPtr<CefBrowser> m_pBrowser;

	bool m_bIsClosing;
	bool IsClosing() { return m_bIsClosing; }

public:
	// Provide access to the single global instance of this object.
	static CMm3dClientHandler* GetInstance();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override{
		return this;
	}
	
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override{
		return this;
	}
	
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override{
		return this;
	}

	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override{
		return this;
	}

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override{
		return this;
	}

	// CefLifeSpanHandler methods:

	// CefContextMenuHandler methods:
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) override;


	bool IsClosing() const { return is_closing_; }

	// CefLifeSpanHandler methods
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;

	// CefContextMenuHandler methods

	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) override;

	virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) override;

	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect) override;

	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);


	virtual void OnPaint(CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width, int height);

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;

	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;


	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);



private:
	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;

	bool is_closing_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CMm3dClientHandler);

public:
	inline BrowserList getBrowserList() { return browser_list_; }
};

