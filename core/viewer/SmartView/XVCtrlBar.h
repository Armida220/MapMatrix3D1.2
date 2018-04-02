// XVCtrlBar.h: interface for the CXVCtrlBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XVCTRLBAR_H__22C680AA_C8FA_4BC0_AF52_E56BC19AD926__INCLUDED_)
#define AFX_XVCTRLBAR_H__22C680AA_C8FA_4BC0_AF52_E56BC19AD926__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SmartViewDef.h"

#define IDC_SLIDER_EDIT            5000


#define REGPATH_USER			_T("Config\\Options\\User")
#define REGITEM_SLIDERZOOM		_T("SliderZoom")

#define BTN_STATE_ENABLE			1
#define BTN_STATE_CHECKED			2


class EXPORT_SMARTVIEW CXVCtrlBar : public CDialogBar  
{
	DECLARE_DYNCREATE(CXVCtrlBar)
public:
	CXVCtrlBar();
	virtual ~CXVCtrlBar();
	BOOL Create( CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID );
	void AddButtons(UINT *ids, UINT *bmpIds, int num);
	void AddButtons(UINT *ids, HBITMAP *bmps, int num);
	BOOL AddComboBox(UINT id);
	BOOL CreateStatic();
	CButton *GetButton(UINT id);
	BOOL AddScrollBar(UINT id);
	CScrollBar *GetScrollBar(UINT id);
	CComboBox *GetComboBox(UINT id);
	int GetButtonsWidth();
	int GetButtonsHeight();

	void AdjustLayout();
	void SetMsgWnd(HWND hWnd);
	void SetButtonState( int id, UINT add, UINT del );
	void SetButtonState2( int id, BOOL bEnable, BOOL bCheck );
	UINT GetButtonState( int id );

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler );

	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnComboZoomChange();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
private:
	void AdjustComboBox();
	void AdjustStatic();

private:
	UINT m_style;
	CPtrArray  m_listBtn;
	CScrollBar m_scrollBar;
	CUIntArray m_listBtnStates;

	CPtrArray  m_listCombo;
	HWND		m_hMsgWnd;
public:
	CStatic     m_Static;
	BOOL m_bSliderZoom;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
};

#endif // !defined(AFX_XVCTRLBAR_H__22C680AA_C8FA_4BC0_AF52_E56BC19AD926__INCLUDED_)
