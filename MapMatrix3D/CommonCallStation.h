// CommonCallStation.h : interface of the CCommonCallStation class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMONCALLSTATION_H__00D1FE7C_7136_4E53_B670_541E966E65E8__INCLUDED_)
#define AFX_COMMONCALLSTATION_H__00D1FE7C_7136_4E53_B670_541E966E65E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcoll.h>

class CCommonCallStation
{
private:
	struct CALLBACK_ITEM
	{
		UINT nMsg;
		CArray<HWND,HWND> arrCallLink;
	};

	CPtrArray m_arrCallStack;

public:
	CCommonCallStation();
	virtual ~CCommonCallStation();

	BOOL LinkCallback(UINT nMsg, HWND hwnd);
	BOOL UnlinkCallback(UINT nMsg, HWND hwnd);
	void UnlinkAll();
	BOOL CallMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	CCommonCallStation::CALLBACK_ITEM* FindCallItem(UINT nMsg);
	void BringCallbackToTop(UINT nMsg, HWND hwnd);
};

extern CCommonCallStation gCallStation;

inline BOOL AfxLinkCallback(UINT nMsg, HWND hwnd)
{
	return gCallStation.LinkCallback(nMsg,hwnd);
}

inline BOOL AfxUnlinkCallback(UINT nMsg, HWND hwnd)
{
	return gCallStation.UnlinkCallback(nMsg,hwnd);
}

inline void AfxUnlinkAll()
{
	gCallStation.UnlinkAll();
}

inline BOOL AfxCallMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return gCallStation.CallMessage(nMsg, wParam, lParam);
}

inline BOOL AfxFindMessage(UINT nMsg)
{
	return (gCallStation.FindCallItem(nMsg)!=NULL);
}

inline void AfxBringCallbackToTop(UINT nMsg, HWND hwnd)
{
	gCallStation.BringCallbackToTop(nMsg,hwnd);
}

#endif