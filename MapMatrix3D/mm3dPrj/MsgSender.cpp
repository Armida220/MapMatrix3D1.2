#include "stdafx.h"
#include "MsgSender.h"
#include <Resource.h>
#include "osgdef.h"
#include "simple_handler.h"
#include <list>

CMsgSender::CMsgSender(CView* view)
{
	pView = view;
}


CMsgSender::~CMsgSender()
{
}



void CMsgSender::sendMsgLfClk(double x, double y, double z)
{
	stPoint pt(x, y, z);
	/*向本视图发送添加点消息，主要是由于线程冲突的问题，所以这里采用发送消息方式*/
	pView->SendMessage(WM_LF_CLK, (WPARAM)&pt);
}

void CMsgSender::sendMsgMouseMove(double x, double y, double z)
{
	stPoint pt(x, y, z);
	/*向本视图发送添加面消息，主要是由于线程冲突的问题，所以这里采用发送消息方式*/
	pView->SendMessage(WM_OSG_MOUSE_MV, (WPARAM)&pt);
}

void CMsgSender::sendMsgRightClk(double x, double y, double z)
{
	stPoint pt(x, y, z);
	/*向本视图发送添加面消息，主要是由于线程冲突的问题，所以这里采用发送消息方式*/
	pView->SendMessage(WM_RIGHT_CLK, (WPARAM)&pt);
}

void CMsgSender::sendMsgPromptLock(bool bLock)
{
	/*向本视图发送添加面消息，主要是由于线程冲突的问题，所以这里采用发送消息方式*/
	pView->SendMessage(WM_PROMPT_LOCK, (WPARAM)bLock);
}

void  CMsgSender::sendMsgPromptCatch(bool bCatch)
{
	/*向本视图发送添加面消息，主要是由于线程冲突的问题，所以这里采用发送消息方式*/
	pView->SendMessage(WM_PROMPT_CATCH, (WPARAM)bCatch);
}

void CMsgSender::sendMsgBkspace()
{
	pView->SendMessage(WM_BACK, 0, 0);
}

void CMsgSender::sendMsgPromptOpera(bool bShiftOpera)
{
	pView->SendMessage(WM_PROMPT_OPERA, (WPARAM)bShiftOpera);
}

void CMsgSender::sendMsgPan(double centerX, double centerY, double centerZ)
{
	stPoint pt(centerX, centerY, centerZ);
	pView->SendMessage(WM_PAN_XY, (WPARAM)&pt);
}

void CMsgSender::sendMsgZoom(double centerX, double centerY, double scale)
{
	stPoint pt(centerX, centerY, scale);
	pView->SendMessage(WM_ZOOM_TO, (WPARAM)&pt);
}

void CMsgSender::sendMsgChangeCurPt()
{
	pView->SendMessage(WM_CHANGE_CURPT, 1, 0);
}

void CMsgSender::sendMsgAdjustFakePt()
{
	pView->SendMessage(WM_CHANGE_CURPT, 0, 0);
}

void CMsgSender::sendMsgShowImg(double x, double y, double z)
{
	stPoint pt(x, y, z);
	pView->SendMessage(WM_SHOW_IMG, (WPARAM)&pt, 0);
}

void CMsgSender::sendMsgDoubleClick(double x, double y, double z, int bt)
{
	if (bt == 1)
	{
		stPoint pt(x, y, z);
		pView->SendMessage(WM_LEFTBTDOUBLECLICK, (WPARAM)&pt, 0);
	}
}

void CMsgSender::sendMsgChangeAbsOriDlg(double relaX, double relaY, double relaZ)
{
	SimpleHandler* handler = SimpleHandler::GetInstance();
	std::list<CefRefPtr<CefBrowser>> lstBr = handler->getBrowserList();
	string script = "setRecordClickFinish(" + to_string(relaX) + "," + to_string(relaY) + "," + to_string(relaZ) + " )";

	for (auto br : lstBr)
	{
		CefRefPtr<CefFrame> frame = br->GetMainFrame();
		frame->ExecuteJavaScript(script, frame->GetURL(), 0);
	}
}

void CMsgSender::sendMsgFetchOriData()
{
	SimpleHandler* handler = SimpleHandler::GetInstance();
	std::list<CefRefPtr<CefBrowser>> lstBr = handler->getBrowserList();
	string script = "fetchOriData()";

	for (auto br : lstBr)
	{
		CefRefPtr<CefFrame> frame = br->GetMainFrame();
		frame->ExecuteJavaScript(script, frame->GetURL(), 0);
	}
}