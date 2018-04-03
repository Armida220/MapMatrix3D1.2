#if !defined(AFX_OPTIONSPAGES_H__E56D719D_EDD5_4BD0_8DA7_E0A99ECF0425__INCLUDED_)
#define AFX_OPTIONSPAGES_H__E56D719D_EDD5_4BD0_8DA7_E0A99ECF0425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPages.h : header file
//

#include "resource.h"

class COp_Page_Base:public CMFCPropertyPage
{
	DECLARE_DYNCREATE(COp_Page_Base)
		
		// Construction
public:
	COp_Page_Base();
	COp_Page_Base(UINT nIDTemplate, UINT nIDCaption = 0);
	COp_Page_Base(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
};
//----------------------------------------------------------------------------
//                    属性设置对话框页面          
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// VIEW

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_Commonly dialog

//##ModelId=41466B7C02BF
class COp_Page_View_Commonly : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_View_Commonly)

// Construction
public:
	//##ModelId=41466B7C02CE
	COp_Page_View_Commonly();
	//##ModelId=41466B7C02CF
	virtual ~COp_Page_View_Commonly();

// Dialog Data
	//{{AFX_DATA(COp_Page_View_Commonly)
	enum { IDD = IDD_OP_PAGE_VIEW_COMMONLY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_View_Commonly)
	protected:
	//##ModelId=41466B7C02D0
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_View_Commonly)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_Measure dialog

//##ModelId=41466B770260
class COp_Page_View_Measure : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_View_Measure)

// Construction
public:
	//##ModelId=4146677C0262
	COp_Page_View_Measure();
	//##ModelId=4146677C026F
	virtual ~COp_Page_View_Measure();

// Dialog Data
	//{{AFX_DATA(COp_Page_View_Measure)
	enum { IDD = IDD_OP_PAGE_VIEW_MEASURE };
	BOOL	m_bCenter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_View_Measure)
	protected:
	//##ModelId=41466B7C0270
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_View_Measure)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



//##ModelId=41466B7C02D1
class COp_Page_View_VectView : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_View_VectView)
		
		// Construction
public:
	//##ModelId=41466B7C02E1
	COp_Page_View_VectView();
	//##ModelId=41466B7C02E1
	virtual ~COp_Page_View_VectView();
	
	// Dialog Data
	//{{AFX_DATA(COp_Page_View_VectView)
	enum { IDD = IDD_OP_PAGE_VIEW_VECTVIEW };
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_View_VectView)
public:
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	CMFCColorButton m_DragColor;
	CMFCColorButton m_BorderColor;
	CMFCColorButton m_HiliteColor;
	CMFCColorButton m_BackColor;
	CMFCColorButton m_FtrPtColor;
	BOOL	m_bSymbolize;
	BOOL    m_bDriveVector;
	BOOL    m_bCorrectVectViewDisorder;
	BOOL	m_bGoodVectView;
	BOOL	m_bImageZoomNotLinear;
	//BOOL	m_bSolidText;
//	BOOL	m_bWhileBK;
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_View_VectView)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};


/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_ImageView dialog

//##ModelId=4146677C0260
class COp_Page_View_ImageView : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_View_ImageView)

// Construction
public:
	//##ModelId=41467B7C0262
	COp_Page_View_ImageView();
	//##ModelId=41467B7C026F
	virtual ~COp_Page_View_ImageView();

// Dialog Data
	//{{AFX_DATA(COp_Page_View_ImageView)
	enum { IDD = IDD_OP_PAGE_VIEW_IMAGEVIEW };
	BOOL m_bDoubleScreen;
	BOOL m_bOutImgAlert;
	CMFCColorButton m_DragColor;
	CMFCColorButton m_HiliteColor;
	BOOL	m_bSymbolize;
	BOOL	m_bDisableMouse;
	BOOL	m_bFasterMappingForASD40;
	BOOL	m_bFasterMappingForASD401;
	double  m_lfExtendDis;
	BOOL	m_bManualLoadVect;
	BOOL	m_bSupportStretch;
	BOOL	m_bTextModeStereo;
	int		m_nMemNum;
	BOOL	m_bSharplyZoom;
	BOOL	m_bInterleavedStereo;
	BOOL	m_bReadViewPara;
	BOOL	m_bCorrectFlicker;
	BOOL	m_bModifyheightBy3DMouse;
	BOOL    m_bZoomImageWithCurMouse;
	CComboBox	m_wndBlockSize;
	//}}AFX_DATA

	int		m_nDragLineWid;
	int		m_nBlockSize;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_View_ImageView)
	protected:
	//##ModelId=4146677C0270
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_View_ImageView)
	virtual BOOL OnInitDialog();
	afx_msg void OnGoodStereo();
	afx_msg void OnCheckModified();
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//---------------------------------------------------------------------------
// DOCMENT

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Catch dialog

//##ModelId=41466B7C027F
class COp_Page_Doc_Snap : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Doc_Snap)

// Construction
public:
	//##ModelId=41466B7C0290
	COp_Page_Doc_Snap();
	//##ModelId=41466B7C0291
	virtual ~COp_Page_Doc_Snap();

// Dialog Data
	//{{AFX_DATA(COp_Page_Doc_Catch)
	enum { IDD = IDD_OP_PAGE_DOC_SNAP };
	BOOL	m_PerpPoint;
	BOOL	m_CenterPoint;
	BOOL	m_TangPoint;
	BOOL	m_bEnable;
	BOOL	m_Intersect;
	BOOL    m_EndPoint;
	BOOL	m_KeyPoint;
	BOOL	m_MidPoint;
	BOOL	m_NearPoint;
	BOOL	m_SnapSelf;
	BOOL	m_bOnlyBaseLine;
	int		m_nDistance;
	BOOL	m_b3D;
	BOOL	m_bShowSnap;
	CMFCColorButton m_SnapColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_Catch)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_Catch)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Save dialog

//##ModelId=41466B7C029E
class COp_Page_Doc_Save : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Doc_Save)

// Construction
public:
	//##ModelId=41466B7C02AF
	COp_Page_Doc_Save();
	//##ModelId=41466B7C02B0
	virtual ~COp_Page_Doc_Save();

// Dialog Data
	//{{AFX_DATA(COp_Page_Doc_Save)
	enum { IDD = IDD_OP_PAGE_DOC_SAVE };
	BOOL	m_bAutoSave;
	int		m_nSaveTime;
	BOOL	m_bUseCache;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_Save)
	protected:
	//##ModelId=41466B7C02B1
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_Save)
	afx_msg void OnAutosave();
	afx_msg void OnDelaysave();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_User dialog

//##ModelId=41466B7C027F
class COp_Page_Doc_User : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Doc_User)

// Construction
public:
	//##ModelId=41466B7C0290
	COp_Page_Doc_User();
	//##ModelId=41466B7C0291
	virtual ~COp_Page_Doc_User();

// Dialog Data
	//{{AFX_DATA(COp_Page_Doc_User)
	enum { IDD = IDD_OP_PAGE_DOC_USER };
	BOOL	m_bRCLKSwitch;
	BOOL	m_bPlaceProperties;
	BOOL	m_bStereoMidButton;
	BOOL	m_bSliderZoom;
	BOOL	m_bMouseWheelZoom;
	BOOL	m_bCADStyle;
	BOOL    m_bFocusColletion;
	int	m_nRecentlyCmd;
	int m_nRecentlyLayer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_User)
	protected:
	//##ModelId=41466B7C0292
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_User)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//---------------------------------------------------------------------------
// INTERFACE


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Ui_Commonly dialog

//##ModelId=41466B7C0241
class COp_Page_Ui_Commonly : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Ui_Commonly)

// Construction
public:
	//##ModelId=41466B7C0243
	COp_Page_Ui_Commonly();
	//##ModelId=41466B7C0250
	virtual ~COp_Page_Ui_Commonly();

// Dialog Data
	//{{AFX_DATA(COp_Page_Ui_Commonly)
	enum { IDD = IDD_OP_PAGE_UI_COMMONLY };
	//BOOL	m_bShowStartup;
	BOOL	m_bTopMost;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Ui_Commonly)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Ui_Commonly)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckShowStartup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Ui_SaveOptions dialog

//##ModelId=41466B7C02DE
class COp_Page_Ui_SaveOptions : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Ui_SaveOptions)

// Construction
public:
	//##ModelId=41466B7C02ED
	COp_Page_Ui_SaveOptions();
	//##ModelId=41466B7C02EE
	virtual ~COp_Page_Ui_SaveOptions();

// Dialog Data
	//{{AFX_DATA(COp_Page_Ui_SaveOptions)
	enum { IDD = IDD_OP_PAGE_DOC_SAVEOPTIONS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Ui_SaveOptions)
	protected:
	//##ModelId=41466B7C02EF
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Ui_SaveOptions)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// COp_Page_DOC_ALERT dialog

//##ModelId=41466B7C02D0
class COp_Page_Doc_Alert : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Doc_Alert)

// Construction
public:
	//##ModelId=41466B7C02E0
	COp_Page_Doc_Alert();
	//##ModelId=41466B7C02E0
	virtual ~COp_Page_Doc_Alert();

// Dialog Data
	//{{AFX_DATA(COp_Page_Doc_Alert)
	enum { IDD = IDD_OP_PAGE_DOC_ALERT };
	BOOL	m_bAlertOver;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_Alert)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_Alert)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// COp_Page_DOC_PAINT dialog
/*
//##ModelId=41466B7C02D1
class COp_Page_Doc_Paint : public CUIFPropertyPage
{
	DECLARE_DYNCREATE(COp_Page_Doc_Paint)

// Construction
public:
	//##ModelId=41466B7C02E1
	COp_Page_Doc_Paint();
	//##ModelId=41466B7C02E1
	virtual ~COp_Page_Doc_Paint();

// Dialog Data
	//{{AFX_DATA(COp_Page_Doc_Paint)
	enum { IDD = IDD_OP_PAGE_DOC_PAINT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_Paint)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CMFCColorButton m_VectorColor;
	CMFCColorButton m_StereoColor;
	CMFCColorButton m_BorderColor;
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_Paint)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
*/

/////////////////////////////////////////////////////////////////////////////
// COp_Page_DOC_PAINT dialog

//##ModelId=41466B7C02D2
class COp_Page_Doc_Select : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Doc_Select)

// Construction
public:
	//##ModelId=41466B7C02E2
	COp_Page_Doc_Select();
	//##ModelId=41466B7C02E2
	virtual ~COp_Page_Doc_Select();

// Dialog Data
	//{{AFX_DATA(COp_Page_Doc_Select)
	enum { IDD = IDD_OP_PAGE_DOC_SELECT };
	CStatic	m_staticSelectSize;
	CSliderCtrl	m_sliderSelectSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_Select)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//}}AFX_VIRTUAL
public:
	int	m_nSelectSize;
	//BOOL m_bSymbolizedSelect;
	BOOL m_bObjectGroup;
	BOOL m_bSurfaceInsideSelect;

private:
	CDC		m_dcMem;
	CBitmap m_bmpShow;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_Select)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DrawSelectSize();
};



class COp_Page_Precision : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_Precision)
		
	// Construction
public:
	COp_Page_Precision();
	virtual ~COp_Page_Precision();
	
	// Dialog Data
	//{{AFX_DATA(COp_Page_Precision)
	enum { IDD = IDD_OP_PAGE_PRECISION };
	//}}AFX_DATA	
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_Select)
public:
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	//}}AFX_VIRTUAL
public:
	int	m_nXYPrecision;
	int	m_nZPrecision;	
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_Select)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Ui_Commonly dialog

//##ModelId=41466B7C0241
class COp_Page_NetWork : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_NetWork)

	// Construction
public:
	//##ModelId=41466B7C0243
	COp_Page_NetWork();
	//##ModelId=41466B7C0250
	virtual ~COp_Page_NetWork();

	// Dialog Data
	//{{AFX_DATA(COp_Page_NetWork)
	enum { IDD = IDD_OP_NETWORK };
	CString m_inipath;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_NetWork)
public:
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_NetWork)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPAGES_H__E56D719D_EDD5_4BD0_8DA7_E0A99ECF0425__INCLUDED_)
