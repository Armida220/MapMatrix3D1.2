#include "stdafx.h"
#include "mm3d_clientHandler.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "ScriptExecutor.h"

CMm3dClientHandler* g_instance = NULL;

CMm3dClientHandler::CMm3dClientHandler()
{
	g_instance = this;
}


CMm3dClientHandler::~CMm3dClientHandler()
{
}

CMm3dClientHandler* CMm3dClientHandler::GetInstance()
{
	return g_instance;
}

void CMm3dClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0)
	{
	}

	// call parent
	CefContextMenuHandler::OnBeforeContextMenu(browser, frame, params, model);
}


bool CMm3dClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle());

	// call parent
	return CefContextMenuHandler::OnContextMenuCommand(browser, frame, params, command_id, event_flags);
}



CefRefPtr<CefResourceHandler> CMm3dClientHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
	return NULL;
}





bool CMm3dClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle());

	// get URL requested
	CefString newURL = request->GetURL();



	// call parent
	return CefRequestHandler::OnBeforeBrowse(browser, frame, request, is_redirect);
}


bool CMm3dClientHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	if (rect.x > 0)
	{
		return false;
	}
}

void CMm3dClientHandler::OnPaint(CefRefPtr<CefBrowser> browser,
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


void CMm3dClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Add to the list of existing browsers.
	browser_list_.push_back(browser);
}

bool CMm3dClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (browser_list_.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void CMm3dClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			browser_list_.erase(bit);
			break;
		}
	}

	if (browser_list_.empty()) {
		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
}

void CMm3dClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void CMm3dClientHandler::CloseAllBrowsers(bool force_close) {
	//if (!CefCurrentlyOn(TID_UI)) {
	//	// Execute on the UI thread.
	//	CefPostTask(TID_UI, base::Bind(&CMm3dClientHandler::CloseAllBrowsers, this, force_close));
	//	return;
	//}

	//if (browser_list_.empty())
	//	return;

	//BrowserList::const_iterator it = browser_list_.begin();
	//for (; it != browser_list_.end(); ++it)
	//	(*it)->GetHost()->CloseBrowser(force_close);
}



