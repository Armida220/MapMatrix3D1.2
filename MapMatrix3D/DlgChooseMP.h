#if !defined(AFX_DIALOGCHOOSEMP_H__ACBCB4C4_A191_4E99_968B_9F96FA8AF7EB__INCLUDED_)
#define AFX_DIALOGCHOOSEMP_H__ACBCB4C4_A191_4E99_968B_9F96FA8AF7EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChooseMP.h : header file
//

struct ModelScope
{
	ModelScope()
	{
		flag = 0;
	}
	ModelScope(PT_2D pp1, PT_2D pp2, PT_2D pp3, PT_2D pp4, CString str) :p1(pp1), p2(pp2), p3(pp3), p4(pp4), name(str){}
	void GetRect(double *minX, double *minY, double *maxX, double *maxY)
	{
		double x1, y1, x2, y2;

		//用第一个顶点的坐标初试化变量
		x1 = p1.x;
		x2 = p1.x;
		y1 = p1.y;
		y2 = p1.y;
		x1 = min(x1, p2.x);
		x2 = max(x2, p2.x);
		y1 = min(y1, p2.y);
		y2 = max(y2, p2.y);
		x1 = min(x1, p3.x);
		x2 = max(x2, p3.x);
		y1 = min(y1, p3.y);
		y2 = max(y2, p3.y);
		x1 = min(x1, p4.x);
		x2 = max(x2, p4.x);
		y1 = min(y1, p4.y);
		y2 = max(y2, p4.y);

		*minX = x1; *maxX = x2; *minY = y1; *maxY = y2; //将值赋给地址返回
		flag = 0;
	}
	PT_2D p1, p2, p3, p4;
	CString name;
	int flag;
};

class CDrawGraphStatic1 : public CStatic
{
	// Construction
public:
	CDrawGraphStatic1();
	~CDrawGraphStatic1();
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
	void AddModelScope(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4, CString str);
	void SetCurrentVecWin(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4);
	void SetDrawArea(const CRect &rect);
	void DrawScope(CDC *pDC, const ModelScope &ms);
	void GeoToDev(PT_2D *pt);
	void GeoToDev(ModelScope *ms);
	void ShowWholeMap();
	void SetEditInfoWnd(HWND hWnd);
	ModelScope* GetNearestMsToPt(CPoint point);
	bool IsEqualForMS(const ModelScope &ms, const ModelScope &ms1);
	void ClearMSArray();
public:
	CArray<ModelScope, ModelScope> m_arrModelScopes;		 //模型范围线
protected:
	CRect m_rectDrawArea;                                //画图区域
	double m_lfMsOrgWorldY;
	double m_lfMsOrgWorldX;
	double m_lfMsOrgLogicY;
	double m_lfMsOrgLogicX;
	double m_lfRatioForCoorTrans;
	bool m_bIsExistForVecWin;
	ModelScope m_msMain;
	ModelScope* m_msMove;
protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseMP dialog

class CDlgChooseMP : public CDialog
{
// Construction
public:
	CDlgChooseMP(CWnd* pParent = NULL);   // standard constructor
    CStringArray m_arrSelectedMaps;
// Dialog Data
	//{{AFX_DATA(CDlgChooseMP)
	enum { IDD = IDD_MAPSHEETLIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChooseMP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDrawGraphStatic1 m_staticDrawGraph;
	BOOL m_bNearest;
	void AdjustLayout();

	// Generated message map functions
	//{{AFX_MSG(CDlgChooseMP)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCheckNearest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCHOOSEMP_H__ACBCB4C4_A191_4E99_968B_9F96FA8AF7EB__INCLUDED_)
