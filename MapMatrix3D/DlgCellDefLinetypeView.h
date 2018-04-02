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

	//��ʼ��ʱ��ʾģʽ(nMode:1:ʹĳЩ�ؼ���Ч,2:����,nLibTypeΪ������,0:ͼԪ��,1:���Ϳ�,nameҪѡ��������)
	void SetShowMode(BOOL bLibTypeValid, BOOL bShowOK, int nLibType, CString name="", BOOL bScaleValid=FALSE);
	void EnableNewCell() { m_bNewCellValid = TRUE; }

	//ѡ��ģʽ�������޸�ͼԪ�����Ϳ⣬ֻ��ѡ��һ������
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
	// typeΪ0ʱ�Ȳ�ȥ��ǰ���ģ�Ϊ1ʱֱ�ӻ� �� ��ȥԭ��
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
	// ���Ԥ������Ĵ���:0,1,2
	int m_nClickNum;
	CPoint m_ptStart;
	CPoint m_ptCur;
	// �任����������
	double m_transformMatrix[9];

	BOOL m_bModified;
	//ͼԪ��������
	int m_nLibType;  
	DWORD m_dScale;

	ConfigLibItem m_config;

	CImageList m_listImages;
	CArray<int,int> m_arrIdxCreateFlag;
	int m_nIdxToCreateImage, m_nImageWid, m_nImageHei;

// 	CCellDefLib          *m_pCellLib;
// 	CBaseLineTypeLib     *m_pLinetypeLib;
	CString				 m_strCellDefLib;       //����ͼԪ��
	CString				 m_strLinetypeLib;       //�������Ϳ�

	CString m_strName;		//ͼԪ�����͵�����
	BOOL m_bShowOK;            //��ʼ��ʱ��ʾģʽ(1:ʹĳЩ�ؼ���Ч,2:����)
	BOOL m_bLibTypeValid;    //m_comboLibType
	BOOL m_bScaleValid;     //m_scalecombo
	BOOL m_bAddSymbol;
	BOOL m_bNewCellValid;
	BOOL m_bHideModifyCtrl;

	int m_nNewCell;         //�����½�ͼԪ�ĸ���

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
