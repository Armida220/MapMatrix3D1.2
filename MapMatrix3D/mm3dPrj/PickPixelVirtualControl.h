#pragma once
#include "VirtualWindow.h"
#include <functional>
#include <string>
typedef std::function<void(bool, std::string, double, double)> notifyFun;
typedef std::function<void()> callbackFun;
class CImageVirtualControl;
class CCheckBoxControl;
class CPickPixelVirtualControl :public CVirtualWindow
{
	notifyFun m_notifyFun;
	callbackFun m_cbfun;
	CImageVirtualControl * m_ic;
	CCheckBoxControl * m_cc;
	friend class CImageVirtualControl;
	friend class CCheckBoxControl;
public:
	CPickPixelVirtualControl(int px, int py, int w, int h, int type);
	~CPickPixelVirtualControl();
	void Paint(CHwndRenderTarget* pRenderTarget);
	void SetNotifyFun(notifyFun fun);
	void SetIdx(int idx);
	int GetIdx() const;
	void Zoom(bool in, const CPoint & pt);
	void Move(double dx, double dy);
	void OnCheckToUncheckByClick();

	void SetState(int checkState);
	bool IsSetNotifyFun() const;
	void Notify(bool);
	void setCheckToUncheckByClickFun(callbackFun fun);
};

