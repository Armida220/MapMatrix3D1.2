// MapDecorateDlg.h: interface for the CMapDecorateDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPDECORATEDLG_H__9FFA626D_CDB4_42AB_9A0D_2E898B52545D__INCLUDED_)
#define AFX_MAPDECORATEDLG_H__9FFA626D_CDB4_42AB_9A0D_2E898B52545D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MapDecorator.h"
#include "UIFPropListExPlus.h"
#include "Command.h"

class CStaticEx : public CStatic
{
	// Construction
	DECLARE_DYNAMIC(CStaticEx)
public:
	CStaticEx();
	void SetTextColor(COLORREF clrText);    
	void SetHotTextColor(COLORREF clrHotText);    
	void SetBgColor(COLORREF clrBack);    
	//	void EnableHotTrack(BOOL bHotTrack);
	void SetBorderColor(COLORREF clrBorder);
	inline void Clicked(){ clicked=!clicked; };
	inline BOOL GetClick() { return clicked;}
	virtual void PreSubclassWindow();
	// Member variblesprotected:    
	COLORREF m_clrText;    
	COLORREF m_clrHotText;    
	COLORREF m_clrBack;
	COLORREF m_clrBorder;
	CBrush m_brBkgnd; 
	//	BOOL m_bHotTrack;    
	BOOL m_bHover;
	BOOL clicked;
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticEx)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CStaticEx();
	
	// Generated message map functions
	/*protected:*/
	//{{AFX_MSG(CStaticEx)
	afx_msg void OnPaint();
	//}}AFX_MSG
	//	afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);    
	afx_msg void OnMouseMove(UINT nFlags, CPoint point); 
	afx_msg void OnMouseLeave();
	DECLARE_MESSAGE_MAP()
};



class CMapDecorateDlg : public CDialog
{
// Construction
public:
	CMapDecorateDlg(UINT nDlgID, CWnd* pParent = NULL);	// standard constructor	
	virtual ~CMapDecorateDlg(){}

	void SetBound(PT_3D pts[4]);
	void SetBoundCorner(PT_3D pt);
	void ClickStatic(int i);

// Dialog Data
	//{{AFX_DATA(CMapDecorateDlg)
	CStaticEx	m_staticSecret;		//密级
	CStaticEx	m_staticNorth;		//指北针
	CStaticEx	m_staticScale;		//比例尺
	CStaticEx	m_staticNearMap;	//邻接表（政区略图）
	CStaticEx	m_staticName;		//图名
	CStaticEx	m_staticArea;		//区域注记
	CStaticEx	m_staticNum;		//图号
	CStaticEx	m_staticLBName;		
	CStaticEx	m_staticLTName;
	CStaticEx	m_staticRTName;
	CStaticEx	m_staticRBName;
	CStaticEx	m_staticFigure;		//图例
	CStaticEx	m_staticRuler;		//坡度尺
	CStaticEx	m_staticDummy;		//方里网、内图廓、外图廓
	//}}AFX_DATA
    CCheckListBox	m_ctlCheckList;

	int			m_nMapType;
	int			m_nMapBorderType;
	float		m_fMapWid, m_fMapHei;

	BOOL		m_bCutStripNO;

	CString		m_strMapNum, m_strMapName;
  
	CRect rect_outbound;	//out_figure
	CRect rect_innerbound;	//in_fiture
	CRect rect_grid;		//in_net
	CRect rect_magnor;     

	BOOL bGrid;
	BOOL bOutBound;
	BOOL bInnerBound;
	BOOL bMagNor;

	int m_nCurItemSel;
	BOOL m_bClickList;

	CArray<CStaticEx*,CStaticEx*> m_arrPCtrls;
	CStringArray m_arrObjIDStrings;
	CUIntArray m_arrCtrlIDs;

	MapDecorate::CMapDecorator *m_pMapDecorator;
	CUIFPropListExPlus m_wndPropList;

	CCommand *m_pCmd;

	BOOL m_bMapDecorateSettings;
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDecorateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	HICON m_hIcon;

	CUIParam *CreateNewUIParam(int index);
	void SaveCurrent();
	void UpdatePropList();

	int GetObjIndex(LPCTSTR id);

	// Generated message map functions
	//{{AFX_MSG(CMapDecorateDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeMapList();
	afx_msg void OnChangeNumEdit();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetParam();
	afx_msg void OnSelectMapBorder();
	afx_msg void OnSetCoordSys();
	afx_msg void OnSelchangeMaptype();
	afx_msg void OnSelchangeMapBorder();
	afx_msg void OnSelectAll();
	afx_msg void OnSelectNone();
	afx_msg void OnDblClickMapList();
	afx_msg void OnStaticButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};




class CMapDecorateParamDlg : public CDialog
{
	// Construction
public:
	CMapDecorateParamDlg(CWnd* pParent = NULL);	// standard constructor	
	
	// Dialog Data
	//{{AFX_DATA(CMapDecorateDlg)
	enum { IDD = IDD_MAPSHOW_PARAM };

	//}}AFX_DATA
    CListBox	m_ctlCheckList;
		
	int m_nCurItemSel;
	BOOL m_bClickList;
	
	MapDecorate::CMapDecorator *m_pMapDecorator;
	CUIFPropListExPlus m_wndPropList;
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDecorateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	CUIParam *CreateNewUIParam(int index);
	void SaveCurrent();
	void UpdatePropList();

	void UpdateItem(int i);
	
	// Generated message map functions
	//{{AFX_MSG(CMapDecorateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeMapList();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSaveDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//5000-10000 新版
class CMapDecorateDlg_5KN : public CMapDecorateDlg
{
// Construction
public:
	CMapDecorateDlg_5KN(CWnd* pParent = NULL);	// standard constructor	
	virtual ~CMapDecorateDlg_5KN(){}

// Dialog Data
	//{{AFX_DATA(CMapDecorate5KNDlg)
	enum { IDD = IDD_MAPSHOW_5KN_DIALOG };
	CStaticEx	m_staticNote;
	CStaticEx	m_staticDataDesc;
	CStaticEx	m_staticMakerBottom;
	CStaticEx	m_staticSecret;
	CStaticEx	m_staticScale;
	CStaticEx	m_staticNearMap;
	CStaticEx	m_staticName;
	CStaticEx	m_staticArea;
	CStaticEx	m_staticNum;
	CStaticEx	m_staticLBName;
	CStaticEx	m_staticLTName;
	CStaticEx	m_staticRTName;
	CStaticEx	m_staticRBName;
	CStaticEx	m_staticFigure;
	CStaticEx	m_staticRuler;
	CStaticEx	m_staticDummy;
	CStaticEx	m_staticDigitalScale;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDecorateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMapDecorateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//500-1000-2000比例尺 旧版
class CMapDecorateDlg_2KO : public CMapDecorateDlg
{
// Construction
public:
	CMapDecorateDlg_2KO( CWnd* pParent = NULL);	// standard constructor	
	virtual ~CMapDecorateDlg_2KO(){}

// Dialog Data
	//{{AFX_DATA(CMapDecorateDlg)
	enum { IDD = IDD_MAPSHOW_DIALOG };
	CStaticEx	m_staticNote;
	CStaticEx	m_staticDataDesc;
	CStaticEx	m_staticMakerRight;
	CStaticEx	m_staticMakerLeft;
	CStaticEx	m_staticSecret;
//	CStaticEx	m_staticNorth;
	CStaticEx	m_staticScale;
	CStaticEx	m_staticNearMap;
	CStaticEx	m_staticName;
	CStaticEx	m_staticArea;
	CStaticEx	m_staticNum;
	CStaticEx	m_staticLBName;
	CStaticEx	m_staticLTName;
	CStaticEx	m_staticRTName;
	CStaticEx	m_staticRBName;
	CStaticEx	m_staticFigure;
//	CStaticEx	m_staticRuler;
	CStaticEx   m_staticCheckMan;
	CStaticEx	m_staticDummy;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDecorateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CMapDecorateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//25000-50000比例尺	新版
class CMapDecorateDlg_25KN : public CMapDecorateDlg
{
// Construction
public:
	CMapDecorateDlg_25KN(CWnd* pParent = NULL);	// standard constructor	
	virtual ~CMapDecorateDlg_25KN(){}

// Dialog Data
	//{{AFX_DATA(CMapDecorateDlg_25KN)
	enum { IDD = IDD_MAPSHOW_25KN_DIALOG };
	CStaticEx	m_staticNote;
	CStaticEx	m_staticDataDesc;
	CStaticEx	m_staticMakerBottom;
	CStaticEx	m_staticSecret;
	CStaticEx	m_staticNorth;
	CStaticEx	m_staticScale;
	CStaticEx	m_staticNearMap;
	CStaticEx	m_staticName;
	CStaticEx	m_staticArea;
	CStaticEx	m_staticNum;
	CStaticEx	m_staticLBName;
	CStaticEx	m_staticLTName;
	CStaticEx	m_staticRTName;
	CStaticEx	m_staticRBName;
	CStaticEx	m_staticFigure;
	CStaticEx	m_staticRuler;
	CStaticEx	m_staticDummy;
	CStaticEx	m_staticDigitalScale;
	CStaticEx	m_staticMagline;
	//}}AFX_DATA

	CRect rect_outbound_decorator;	//外图廓装饰线
	BOOL  bOBD;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDecorateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMapDecorateDlg)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeMapList();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//25000-50000比例尺	旧版
class CMapDecorateDlg_25KO : public CMapDecorateDlg
{
// Construction
public:
	CMapDecorateDlg_25KO(CWnd* pParent = NULL);	// standard constructor	
	virtual ~CMapDecorateDlg_25KO(){}

// Dialog Data
	//{{AFX_DATA(CMapDecorateDlg_25KO)
	enum { IDD = IDD_MAPSHOW_25KO_DIALOG };
	CStaticEx	m_staticName;
	CStaticEx	m_staticNum;
	CStaticEx	m_staticArea;
	CStaticEx	m_staticDummy;
	CStaticEx	m_staticLBName;
	CStaticEx	m_staticLTName;
	CStaticEx	m_staticRTName;
	CStaticEx	m_staticRBName;	
	CStaticEx	m_staticNearMap;
	CStaticEx	m_staticSecret;
	CStaticEx	m_staticFigure;
	CStaticEx	m_staticMakerLeft;
	CStaticEx	m_staticAdminRegion;	
	CStaticEx	m_staticDataDesc;
	CStaticEx	m_staticRuler;
	CStaticEx	m_staticScale;	
	CStaticEx	m_staticNorth;	
	CStaticEx	m_staticNote;
	CStaticEx	m_staticMagline;
	//}}AFX_DATA

	CRect rect_outbound_decorator;	//外图廓装饰线
	BOOL  bOBD;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDecorateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMapDecorateDlg)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeMapList();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MAPDECORATEDLG_H__9FFA626D_CDB4_42AB_9A0D_2E898B52545D__INCLUDED_)
