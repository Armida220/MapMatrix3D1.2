// UIFSimpleEx.h: interface for the CUIFPropEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UIFSIMPLEEX_H_INCLUDED_)
#define _UIFSIMPLEEX_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSonDialog : public CDialog
{
public:
	CSonDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL):CDialog(lpszTemplateName,pParentWnd){}
	CSonDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL ):CDialog(nIDTemplate,pParentWnd){}
	CSonDialog():CDialog(){}
	virtual BOOL PreTranslateMessage(MSG *pMsg)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL ||
				pMsg->wParam == VK_RETURN)
			{
				CWnd *pwnd = GetParent();
				if (pwnd && pwnd->IsKindOf(RUNTIME_CLASS(CDialog)))
				{
					pMsg->hwnd = pwnd->GetSafeHwnd();
					return pwnd->PreTranslateMessage(pMsg);
				}
			}
		}
		return CDialog::PreTranslateMessage(pMsg);
	}
};

#endif // !defined(_UIFSIMPLEEX_H_INCLUDED_)
