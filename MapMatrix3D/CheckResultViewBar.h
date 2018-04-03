// CheckResultViewBar.h: interface for the CCheckResultViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKRESULTVIEWBAR_H__E8053987_15C4_4E39_AF91_C0706C06FA2B__INCLUDED_)
#define AFX_CHECKRESULTVIEWBAR_H__E8053987_15C4_4E39_AF91_C0706C06FA2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXToolTipCtrl.h"
#include "editbaseDoc.h"



class CChkCmd;
struct ChkResultItem
{
	ChkResultItem()
	{
		pos = PT_3D(0,0,0);
		chkCmd = _T("");
		reason = _T("");
		state = 0;
		pFtrMark = NULL;
		ntime = 0;
	}
	BOOL operator==(const ChkResultItem& item)
	{
		if(chkCmd!=item.chkCmd)
		{
			return FALSE;
		}
		if(reason!=item.reason)
		{
			return FALSE;
		}
		if(pFtrMark!=pFtrMark)
		{
			return FALSE;
		}
		if(!GraphAPI::GIsEqual3DPoint(&pos, &item.pos))
		{
			return FALSE;
		}
		return TRUE;
	}
	CArray<CFeature*,CFeature*> arrFtrs;

	//标记对象，程序需要保持标记对象与状态的一致性：标记为空，state应该为1或2，标记不为空，state应该为0；
	CFeature* pFtrMark;

	PT_3D pos;
	int state;		//状态；0表示错误，1表示OK，2表示已经修改
	CString chkCmd;
	CString reason;
	long ntime;
};

class CChkResManger
{
	friend CChkResManger& GetChkResMgr();
public:
//	CChkResManger();
	~CChkResManger();
	void SetCurChkCmd(const CChkCmd* pCmd);

	BOOL BeginResItem(LPCTSTR name=NULL);
	void AddAssociatedFtr(CFeature* pFtr);
	void SetAssociatedPos(PT_3D pos);
	void SetReason(LPCTSTR reson);
	BOOL EndResItem();
	int GetChkResCount()const;
	ChkResultItem* GetChkResByIdx(int idx);
	int GetCurItemAssociatedFtrCount(){
		return m_pCurResultItem->arrFtrs.GetSize();
	}
	void Clear();
	void Load();
	void Load(CString path, CArray<ChkResultItem*, ChkResultItem*>& arrChkRes_Compare);
	void Save(CDocument *pDoc, LPCTSTR savepath=NULL);
	BOOL MarkItem(CDlgDoc *pDoc, ChkResultItem *pItem, int icon_index, COLORREF color, float icon_size);
	BOOL DeleteMark(CDlgDoc *pDoc, ChkResultItem *pItem);
	BOOL DeleteAllMarks(CDlgDoc *pDoc);
	void CompareLastResult();

protected:
	CString GetSavePath();

	CChkResManger();
	CArray<ChkResultItem*,ChkResultItem*> m_arrChkRes;
	ChkResultItem * m_pCurResultItem;
	const CChkCmd *m_pCurCmd;
};



class CChkResSettings
{
public:
	struct ChkIcon
	{
		ChkIcon(){
			color = RGB(255,0,0);
		}
		ChkIcon(const ChkIcon& a){
			cmd = a.cmd;
			reason = a.reason;
			icon = a.icon;
			color = a.color;
		}
		const ChkIcon& operator =(const ChkIcon& a){
			cmd = a.cmd;
			reason = a.reason;
			icon = a.icon;
			color = a.color;
			return *this;
		}
		CString cmd;
		CString reason;
		CString icon;
		COLORREF color;
	};
	CChkResSettings();
	~CChkResSettings();

	void Copy(CChkResSettings *p);

	BOOL Load();
	BOOL Save();

	BOOL IsLoaded(){
		return m_bLoad;
	}

	static CChkResSettings* Obj();
	CStringArray *GetResultIconNames(){
		return &m_arrResultIconNames;
	}
	CString GetIcon(LPCTSTR cmd, LPCTSTR reason);
	COLORREF GetColor(LPCTSTR cmd, LPCTSTR reason);
	void SetIcon(LPCTSTR cmd, LPCTSTR reason, LPCTSTR icon);
	void SetColor(LPCTSTR cmd, LPCTSTR reason, COLORREF color);

	float GetResultIconSize(){
		return m_fResultIconSize;
	}
	void SetResultIconSize(float size){
		m_fResultIconSize = size;
	}

protected:
	BOOL CompareReason(LPCTSTR reason0, LPCTSTR reason1);
	CString GetPath();
	
	CStringArray m_arrResultIconNames;
	
	float m_fResultIconSize;
	CArray<ChkIcon,ChkIcon> m_arrChkIcons;

	BOOL m_bLoad;
	CString m_path;
};

CChkResManger& GetChkResMgr();

class CListCtrl0: public CListCtrl
{
public:
	CListCtrl0()
	{
		m_nSortCol = -1;
		m_bAscending = FALSE;
	}
	int m_nSortCol;
	BOOL m_bAscending;
protected:
	//{{AFX_MSG(CListEx)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	void CustDraw(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

class CCheckResultViewBar : public CDockablePane  
{
public:
	class CMyToolBar : public CMFCToolBar
	{
		//##ModelId=41466B7F009D
		virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)	
		{		
			CMFCToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
		}
		virtual BOOL AllowShowOnList () const		{	return FALSE;	}
		
	};

	CCheckResultViewBar();
	virtual ~CCheckResultViewBar();
	
	LONG OnSetResult(WPARAM wParam,LPARAM lParam);
	void SelectLastResult();
	void SelectNextResult();
	void DeleteOrRestoreCurResult();

	void InitLoad();

	BOOL HaveInitLoad()
	{
		return m_bInitLoad;
	}

	BOOL IsResultIterate()
	{
		return m_bResultIterate;
	}

	ChkResultItem *GetCurItem();
	void SetAllStates(int state);
	void SetItemState(ChkResultItem *pItem, int state);
	
	void OnCloseDoc(CDocument *pDoc);
	void OnMarkSettings();
	afx_msg void OnResultClear();

private:
	void FillChkResItem();
	void ClearAll();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)	
	{		
		CDockablePane::OnUpdateCmdUI ( (CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}
	CDocument *m_pDoc;
	
	int m_nTimerState;
	GrBuffer m_gr;

//	CFeature *GetSelFtr(int *idx = NULL);
protected:	
	void GetSelFtrs(CArray<CFeature*,CFeature*> &arr);
	ChkResultItem* GetSelPos(PT_3D *pt, int *pItem=NULL);

	void CreateFilter();
	CString MakeObjInfo(const ChkResultItem *pItem, BOOL bForTips);
	void UpdateToolTips2();

	void LocateCurFtrs();

	//用户修改了一些问题地物之后，RefreshResultStates 函数能把相应的问题地物状态(state)标记为2
	void RefreshResultStates();

	int GetIconIndex(ChkResultItem *pItem);
	COLORREF GetIconColor(ChkResultItem *pItem);

	void SelectItem(int index, BOOL bScrollCtrl);
	void SelectItem(const ChkResultItem *pItem, BOOL bScrollCtrl);

	void AdjustLayout();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg	void OnPaint();
	afx_msg void OnResultMarkall();
	afx_msg void OnResultMarkpt();
	afx_msg void OnResultSelall();
	afx_msg void OnResultSelobj();
//	afx_msg void OnResultUnselall();
//	afx_msg void OnResultUnselobj();
	afx_msg void OnResultFilter();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnResultLoad();
	afx_msg void OnSaveRet();
	afx_msg void OnLoadRets();
	afx_msg void OnResultIterate();
	afx_msg void OnUpdateResultIterate(CCmdUI *pCmdUI);
	afx_msg void OnRefreshResultStates();
	afx_msg void OnLocateItem();
	afx_msg void OnExport();
	afx_msg LRESULT OnClickIcon(WPARAM wParam, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
private:
	void UpdateDrawing(BOOL bDraw);
	CListCtrl0 m_listCtrl;
	CImageList m_imageList;
	CFont m_font;
	CMyToolBar m_wndToolBar;
	CChkResManger *m_pChkResManger;

	CStringArray m_arrFilterNames;
	CArray<int,int> m_arrFilterFlags;

	COXToolTipCtrl m_ToolTip;

	BOOL m_bInitLoad;
	BOOL m_bResultIterate;

	BOOL m_bStopLocateResultPt;
};

#endif // !defined(AFX_CHECKRESULTVIEWBAR_H__E8053987_15C4_4E39_AF91_C0706C06FA2B__INCLUDED_)
