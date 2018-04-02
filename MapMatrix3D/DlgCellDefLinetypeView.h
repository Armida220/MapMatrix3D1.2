#if !defined(AFX_DLGCELLDEFLINETYPEVIEW_H__B0131EC8_55F6_4B40_9C52_8334E7554221__INCLUDED_)
#define AFX_DLGCELLDEFLINETYPEVIEW_H__B0131EC8_55F6_4B40_9C52_8334E7554221__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCellDefLinetypeView.h : header file
//
#include "Symbol.h"
#include "SymbolLib.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgCellDefLinetypeView dialog
class CDlgCellDoc;
class CDlgCellDefLinetypeView : public CDialog
{
// Construction
public:
	CDlgCellDefLinetypeView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCellDefLinetypeView();
	
	void InitInterFace();

	BOOL IsClosing() { return m_bClosing; }

	CString GetName() { return m_strName; }

	void SaveCellLinetype();

	BOOL SaveEditCell(CString name="");

	//初始化时显示模式(nMode:1:使某些控件无效,2:隐藏,nLibType为库类型,0:图元库,1:线型库,name要选中项名字)
	void SetShowMode(BOOL bLibTypeValid, BOOL bShowOK, int nLibType, CString name="", BOOL bScaleValid=FALSE);
	void EnableNewCell() { m_bNewCellValid = TRUE; }

	//选择模式，不会修改图元库线型库，只是选择一个符号
	void SetAsSelectMode(int nLibType); 

	void SetConfig(ConfigLibItem config);
	ConfigLibItem *GetConfig() { return &m_config;}
// 	void SetCellDefLib(CCellDefLib *pLib);
// 	void SetLineTypeLib(CBaseLineTypeLib *pLib);

	void SetScale(DWORD dScale) { m_dScale = dScale; };

	void FillSymbolList();	
	void CreateImageList(int cx, int cy, int nsize);
	BOOL CreateImageItem(int idx);
	void DrawSymbol(CellDef *def, GrBuffer2d *buf);
	void DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf);
	
	void DrawPreview(LPCTSTR strSymName);
	// type为0时先擦去先前画的，为1时直接画 或 擦去原来
	void DrawTemLine(CPoint pt=CPoint(0,0), int type=1);
	
	void OnStaticPreview();

// Dialog Data
	//{{AFX_DATA(CDlgCellDefLinetypeView)
	enum { IDD = IDD_CELLDEF_LINETYPE };
	CListCtrl	m_wndListSymbol;
	CComboBox	m_comboLibType;
	CComboBox	m_scaleCombo;
	CComboBox   m_comboLayers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCellDefLinetypeView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCellDefLinetypeView)	
	afx_msg void OnChangeEditSearch();
	afx_msg void OnItemchangedListSymbol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndEditListSymbol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListSymbol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeTypeCombo();
	afx_msg void OnCancle();
	afx_msg void OnSetfocusEditSearch();
	afx_msg void OnKillfocusEditSearch();
	afx_msg void OnAddlineButton();
	afx_msg void OnDellineButton();
	afx_msg void OnDelallButton();
	afx_msg void OnClose();
	afx_msg void OnBeginlabeleditListSymbol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickStatic();
	afx_msg void OnImportcellfromdxfButton();
	afx_msg void OnExportcelltodxfButton();
	afx_msg void OnSelchangeScaleCombo();
	afx_msg void OnItemchangingListSymbol(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// 点击预览区域的次数:0,1,2
	int m_nClickNum;
	CPoint m_ptStart;
	CPoint m_ptCur;
	// 变换矩阵的逆矩阵
	double m_transformMatrix[9];

	BOOL m_bModified;
	//图元还是线型
	int m_nLibType;  
	DWORD m_dScale;

	ConfigLibItem m_config;

	CImageList m_listImages;
	CArray<int,int> m_arrIdxCreateFlag;
	int m_nIdxToCreateImage, m_nImageWid, m_nImageHei;

// 	CCellDefLib          *m_pCellLib;
// 	CBaseLineTypeLib     *m_pLinetypeLib;
	CString				 m_strCellDefLib;       //备份图元库
	CString				 m_strLinetypeLib;       //备份线型库

	CString m_strName;		//图元或线型的名字
	BOOL m_bShowOK;            //初始化时显示模式(1:使某些控件无效,2:隐藏)
	BOOL m_bLibTypeValid;    //m_comboLibType
	BOOL m_bScaleValid;     //m_scalecombo
	BOOL m_bAddSymbol;
	BOOL m_bNewCellValid;
	BOOL m_bHideModifyCtrl;

	int m_nNewCell;         //批量新建图元的个数

	BOOL m_bClosing;

	int m_nLastSel;
	CSwitchScale m_scaleSwitch;

	HBITMAP m_hBmp;
public:

	CDlgCellDoc *m_pCellDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCELLDEFLINETYPEVIEW_H__B0131EC8_55F6_4B40_9C52_8334E7554221__INCLUDED_)
