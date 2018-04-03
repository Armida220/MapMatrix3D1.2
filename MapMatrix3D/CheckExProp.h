#if !defined(AFX_CHECKEXPROP_H__880E2467_7F5E_4743_80AF_F61F40B4C38F__INCLUDED_)
#define AFX_CHECKEXPROP_H__880E2467_7F5E_4743_80AF_F61F40B4C38F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckExProp.h : header file
//

#include "EditBase.h"
#include "GridCtrl_src/GridCtrl.h"


class CDlgDoc;
class CDlgDataSource;

namespace EditBase{

class CAttributesSource;

}

/////////////////////////////////////////////////////////////////////////////
// filed规则

typedef struct tagRule {
	int RuleType;//rule的类型枚举或范围：1枚举  2范围  0无值  -1无规则
	CString field_name;
	int MustSel;//必选
	CStringArray EVal;//枚举值的一组字符串
	double maxVal;//范围值得最大值
	double minVal;//范围值的最小值
} FieldRule;

class Field  
{
public:
	Field();
	virtual ~Field();
	bool Init(int nScale);
	void CleanUp();

	//获取某层的信息：扩展属性的名称和规则
	int getFieldInfo(LPCTSTR layername,BOOL byDbName);
	int GetRuleID(CString fieldname);
	bool checkRule(CString field, CString valstr);
public:
	FieldRule *m_rules;
	int m_fields_num;//属性数量

private:
	CScheme *m_pScheme;
};

struct LAYERINFO
{
	CString layName;
	CString EDBName;
	int num;
};

enum{BASEFTR,LINEFTR,SURFACEFTR,TEXTFTR};

struct BaseFtr
{
	BaseFtr(){
		index = -1;
		pFtr = NULL;
		pTab = NULL;
		type=BASEFTR;
	}
	int index;//地物序号
	int type;
	CFeature *pFtr;
	CValueTable *pTab;
};

struct LineFtr : public BaseFtr
{
	LineFtr(){
		index = -1;
		pFtr = NULL;
		pTab = NULL;
		type=LINEFTR;
		lenth = -1;
	}
	double lenth;
};

struct SurfaceFtr : public LineFtr
{
	SurfaceFtr(){
		index = -1;
		pFtr = NULL;
		pTab = NULL;
		type=SURFACEFTR;
		area = -1;
	}
	double area;
};

struct TextFtr : public BaseFtr
{
	TextFtr(){
		index = -1;
		pFtr = NULL;
		pTab = NULL;
		type=TEXTFTR;
	}
	CString text;
};

typedef CArray<BaseFtr*,BaseFtr*> CMyFtrArray;


/////////////////////////////////////////////////////////////////////////////
// CEditExProp dialog  :显示所有层

class CEditExProp : public CDialog
{
// Construction
public:
	CEditExProp(CWnd* pParent = NULL);   // standard constructor
	~CEditExProp();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Dialog Data
	//{{AFX_DATA(CCheckExProp)
	enum { IDD = IDD_CHECKEXPROP };
	CGridCtrl m_gridCtrl;
	BOOL m_bShowByEDB;//按入库分组
	CMFCToolBar m_wndToolBar;
	CStringArray m_arrLayers;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckExProp)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCheckExProp)
	afx_msg void OnOK(){}//屏蔽回车关对话框的功能
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeCombo();
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridStartSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGridComboChange(WPARAM wParam,LPARAM lParam);
	afx_msg void OnPrevPage();
	afx_msg void OnNextPage();
	afx_msg void OnCheckLayerDB();
	afx_msg void OnUpdateCheckLayerDB(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrevPage1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextPage1(CCmdUI* pCmdUI);
	afx_msg void OnExport();
	afx_msg void OnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL LoadRuleFile();
	virtual void Check();
	void UpdateSelection();

	void setInitLayerName(CString layer_name)
	{
		m_curLayerName = layer_name;
	}

	void checkByDBLayerName(BOOL by_dblayer)
	{
		m_bShowByEDB = by_dblayer;
		m_wndToolBar.UpdateButton(2);
	}

	CDlgDoc *m_pDoc;

protected:
	CDlgDataSource *m_pDS;
	CAttributesSource *m_pXDS;
	CScheme *m_pScheme;
	int m_curPage;//当前页
	Field m_lf;//当前层的信息和规则
	CMyFtrArray m_ftrs;
	int m_SelColumn;//被选中的列，未选中为-1
	CArray<int,int> m_SelectedRows;//被选中的行
	int m_comboRow;
	int m_comboCol;

	CArray<LAYERINFO,LAYERINFO> m_layerinfos;

	CString m_curLayerName;

	virtual void loadLayer(LPCTSTR layname);
	BaseFtr* AddFtr(CFeature *pFtr);
	void clear();
	virtual void showLayer();
	virtual int CheckVal(CValueTable &tab, BaseFtr *pItem, BOOL OnlyCheck);
	void AddComboToCell(int row, int col);
	void SetSelected(int col, BOOL SEL)
	{
		for(int i=0; i<m_SelectedRows.GetSize(); i++)
		{
			int row = m_SelectedRows.GetAt(i);
			int nState = m_gridCtrl.GetItemState(row, col);
			if(SEL)
				m_gridCtrl.SetItemState(row, col, nState|GVIS_SELECTED);
			else
				m_gridCtrl.SetItemState(row, col, nState & ~GVIS_SELECTED);
		}
		m_gridCtrl.Refresh();
	}
	void SortItems(CString field);
	void UpdateLayerCombo(int sel=0);
	LPCTSTR GetCurLayer();
};


/////////////////////////////////////////////////////////////////////////////
// CCheckExProp dialog :显示有错误的层
class CCheckExProp : public CEditExProp
{
	// Construction
public:
	CCheckExProp(CWnd* pParent = NULL);   // standard constructor
	~CCheckExProp();
	virtual void Check();

protected:
	virtual BOOL OnInitDialog();
	virtual int CheckVal(CValueTable &tab, BaseFtr *pItem, BOOL OnlyCheck);
	virtual void loadLayer(LPCTSTR layname);
};


/////////////////////////////////////////////////////////////////////////////
// CDlgOutputLayerCheck dialog  :显示所有层

class CDlgOutputLayerCheck : public CDialog
{
// Construction
public:
	CDlgOutputLayerCheck(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCheckExProp)
	enum { IDD = IDD_FILE_OUTPUT_LAYERCHECK };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckExProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOutputLayerCheck)
	afx_msg void OnOK(){}//屏蔽回车关对话框的功能
	afx_msg BOOL PreTranslateMessage(MSG* pMsg) 
	{
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
			return TRUE;
		return CDialog::PreTranslateMessage(pMsg);
	}
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOutput();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CDlgDoc *m_pDoc;

private:
	void InitLayer(CString name);
	CComboBox	m_wndComboFields;
	CDlgDataSource *m_pDS;
	CAttributesSource *m_pXDS;
	CString m_LayerName;
	CString m_Field;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKEXPROP_H__880E2467_7F5E_4743_80AF_F61F40B4C38F__INCLUDED_)
