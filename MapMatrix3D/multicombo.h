#if !defined(AFX_LAYERCOMBO_H__F1530E8E_D359_11D5_A0E0_0050BA555258__INCLUDED_)
#define AFX_LAYERCOMBO_H__F1530E8E_D359_11D5_A0E0_0050BA555258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayerCombo.h : header file

#include "editbasedoc.h"
#define BH_COMBOX_HEIGHT			300
#define BW_LAYERGROUPCOMBOX_WIDTH   130
#define BW_LAYERCOMBOX_WIDTH		190
#define BW_FTRCOLORCOMBOX_WIDTH		100
#define BW_FTRSYMCOMBOX_WIDTH		140
#define BW_FTRWIDTHCOMBOX_WIDTH		130
/////////////////////////////////////////////////////////////////////////////
// CMultiCombo window
#define LS_LAMP_ON			0x0001
#define PS_PADLOCK_OPEN		0x0004
#define ID_COMBO_LAYER      100

#include "UIFToolbarComboBoxButtonEx.h"

extern CRect g_Btn0Rect;

class CUIFToolbarBaseCustomComboBoxButton : public CUIFToolbarComboBoxButtonEx
{
	DECLARE_SERIAL(CUIFToolbarBaseCustomComboBoxButton)
public:
	CUIFToolbarBaseCustomComboBoxButton();
	virtual ~CUIFToolbarBaseCustomComboBoxButton();
	
	virtual void RefreshComboBox(CDlgDoc* pDoc=NULL);
	
	CUIFToolbarBaseCustomComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0) :
	CUIFToolbarComboBoxButtonEx(uiID, iImage, dwStyle, iWidth){}
	virtual CComboBox* CreateCombo (CWnd* pWndParent, const CRect& rect);
	virtual void OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE,
		BOOL bDrawBorder = TRUE,
		BOOL bGrayDisabledButtons = TRUE);
	
	virtual void CopyFrom(const CMFCToolBarButton& src);
	
	virtual void Serialize (CArchive& ar);
};

class CBaseBitmapComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CBaseBitmapComboBox)
		// Construction
public:
	CBaseBitmapComboBox();
	virtual ~CBaseBitmapComboBox();
	
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	int  AddString(LPCTSTR lpszString);
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiCombo)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	HWND m_hListBox;
	
	//{{AFX_MSG(CMultiCombo)
	afx_msg void OnDropdown();
	afx_msg void OnCloseUp();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
		
protected:
	LONG    m_lfHeight;
    LONG    m_lfWeight;
    CString m_strFaceName;
	
	CFont m_font;
public:
	CUIFToolbarBaseCustomComboBoxButton *m_pToolbarComboxButton;
};

class CLayerGroupBitmapComboBox : public CBaseBitmapComboBox
{
	DECLARE_DYNAMIC(CLayerGroupBitmapComboBox)
		// Construction
public:
	CLayerGroupBitmapComboBox();
	virtual ~CLayerGroupBitmapComboBox();
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiCombo)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	HWND m_hListBox;
	
	//{{AFX_MSG(CMultiCombo)
	afx_msg void OnDropdown();
	
	//}}AFX_MSG
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
		
};

class CLayerBitmapComboBox : public CBaseBitmapComboBox
{
	DECLARE_DYNAMIC(CLayerBitmapComboBox)
// Construction
public:
	CLayerBitmapComboBox();
	virtual ~CLayerBitmapComboBox();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiCombo)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	HWND m_hListBox;

	//{{AFX_MSG(CMultiCombo)
	afx_msg void OnDropdown();

	//}}AFX_MSG
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};

class CFtrWidthBitmapComboBox : public CBaseBitmapComboBox
{
	DECLARE_DYNAMIC(CFtrWidthBitmapComboBox)
		// Construction
public:
	CFtrWidthBitmapComboBox();
	virtual ~CFtrWidthBitmapComboBox();
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiCombo)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	//}}AFX_VIRTUAL
		
	// Generated message map functions
protected:
	
	//{{AFX_MSG(CLayerComboBox)
	afx_msg void OnDropdown();
	//}}AFX_MSG
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

protected:
	CArray<float, float> m_arrFtrWids;

};

class CFtrSymBitmapComboBox : public CBaseBitmapComboBox
{
	DECLARE_DYNAMIC(CFtrSymBitmapComboBox)
		// Construction
public:
	CFtrSymBitmapComboBox();
	virtual ~CFtrSymBitmapComboBox();
	
	// Operations
public:
	
	void DrawImageItem(GrBuffer2d *pBuf, HDC destHdc, CRect destRect);
	BOOL GetSymGrBuffer(CString name, GrBuffer2d *pBuf);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiCombo)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	
	//{{AFX_MSG(CLayerComboBox)
	afx_msg void OnDropdown();
	//}}AFX_MSG
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
		
	
};

class CUIFToolbarLayerGroupComboBoxButton : public CUIFToolbarBaseCustomComboBoxButton
{
	DECLARE_SERIAL(CUIFToolbarLayerGroupComboBoxButton)
public:
	CUIFToolbarLayerGroupComboBoxButton();
	virtual ~CUIFToolbarLayerGroupComboBoxButton();
	
	virtual void RefreshComboBox(CDlgDoc* pDoc = NULL);
	
	CUIFToolbarLayerGroupComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0) :
	CUIFToolbarBaseCustomComboBoxButton(uiID, iImage, dwStyle, iWidth){};
	virtual CComboBox* CreateCombo (CWnd* pWndParent, const CRect& rect);
	
protected:
};

class CUIFToolbarLayerComboBoxButton : public CUIFToolbarBaseCustomComboBoxButton
{
	DECLARE_SERIAL(CUIFToolbarLayerComboBoxButton)
public:
	CUIFToolbarLayerComboBoxButton();
	virtual ~CUIFToolbarLayerComboBoxButton();

	virtual void RefreshComboBox(CDlgDoc* pDoc = NULL);

	CUIFToolbarLayerComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0) :
	CUIFToolbarBaseCustomComboBoxButton(uiID, iImage, dwStyle, iWidth){};
	virtual CComboBox* CreateCombo (CWnd* pWndParent, const CRect& rect);

protected:
};

class CUIFToolbarFtrWidthComboBoxButton : public CUIFToolbarBaseCustomComboBoxButton
{
	DECLARE_SERIAL(CUIFToolbarFtrWidthComboBoxButton)
public:
	CUIFToolbarFtrWidthComboBoxButton();
	virtual ~CUIFToolbarFtrWidthComboBoxButton();
	
	virtual void RefreshComboBox(CDlgDoc* pDoc = NULL);

	CUIFToolbarFtrWidthComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0) :
	CUIFToolbarBaseCustomComboBoxButton(uiID, iImage, dwStyle, iWidth){};
	virtual CComboBox* CreateCombo (CWnd* pWndParent, const CRect& rect);
	
protected:
};

class CUIFToolbarFtrSymComboBoxButton : public CUIFToolbarBaseCustomComboBoxButton
{
	DECLARE_SERIAL(CUIFToolbarFtrSymComboBoxButton)
public:
	CUIFToolbarFtrSymComboBoxButton();
	virtual ~CUIFToolbarFtrSymComboBoxButton();
	
	virtual void RefreshComboBox(CDlgDoc* pDoc = NULL);
	
	CUIFToolbarFtrSymComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0) :
	CUIFToolbarBaseCustomComboBoxButton(uiID, iImage, dwStyle, iWidth){};
	virtual CComboBox* CreateCombo (CWnd* pWndParent, const CRect& rect);
	
protected:
};

#define STATICID 100
class CUIFColorToolBarButton : public CMFCColorMenuButton
{
	DECLARE_SERIAL(CUIFColorToolBarButton)
public:
	CUIFColorToolBarButton();
	CUIFColorToolBarButton(UINT uiCmdID, LPCTSTR lpszText,
		CPalette* pPalette = NULL, int iWidth = 0);
	virtual ~CUIFColorToolBarButton();

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE,
		BOOL bDrawBorder = TRUE,
						BOOL bGrayDisabledButtons = TRUE);

	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void OnChangeParentWnd (CWnd* pWndParent);
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL NotifyCommand (int iNotifyCode);
	BOOL OnClick (CWnd* pWnd, BOOL bDelay);

	COLORREF GetColor ();

	virtual void SetStyle (UINT nStyle);
	virtual void SetColor (COLORREF clr, BOOL bNotify = TRUE);
	
	CString FormatProperty ();

	BOOL IsEmpty(){
		return m_bEmpty;
	}
	void SetEmpty(BOOL bEmpty);

	virtual void Serialize (CArchive& ar);
	//void SetByLayer(BOOL bByLayer);
protected:
	//CRect			m_Rect;
	CStatic	*m_pWndEdit;
	//BOOL m_bByLayerFlag;
public:
	int			m_iWidth;
	BOOL        m_bEmpty;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERCOMBO_H__F1530E8E_D359_11D5_A0E0_0050BA555258__INCLUDED_)
