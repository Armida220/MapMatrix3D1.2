// OutputViewBar.h: interface for the COutputViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTVIEWBAR_H__3095A6D2_3E8F_48AB_8312_9209282D9395__INCLUDED_)
#define AFX_OUTPUTVIEWBAR_H__3095A6D2_3E8F_48AB_8312_9209282D9395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueTable.h"
#include "UIParam.h"
#include "CustomizeKey.h"

#define WM_CHARONE					(WM_USER+1)
#define WM_KEYONE					(WM_USER+2)
#define WM_KEYINSTRING				(WM_USER+3)
#define OUTSTR_RETURN				_T("\r\n")
#define OUTSTR_NULL					_T("")

class CDlgInputCommand;
class COutputEdit : public CEdit
{
	// Construction
public:
	COutputEdit();
	virtual ~COutputEdit();
	
	// Operations
public:
	void Output(LPCTSTR strText);
	void Output_ReplaceLastLine(LPCTSTR strText);
	BOOL IsEmpty();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputEdit)
	//}}AFX_VIRTUAL
	
	// Implementation
public:	
	
	// Generated message map functions
protected:
	//{{AFX_MSG(COutputEdit)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:

	CBrush m_brush;
};


class CInputEdit : public CEdit
{
	
	// Construction
public:

	typedef enum {
		InputNone  = -1,
			InputChar  = 0,
			InputBack  = 1,
			inputReturn = 2
			
	}InputType;

	CInputEdit();
	virtual ~CInputEdit();

	CString GetText();
	CString GetPrompt();
	void SetText(LPCTSTR strText);
	void SetPrompt(LPCTSTR strText);
	void SetLastPrompt();
	void Return();
	InputType GetInputType() {
		return m_nInputType;
	}
	
	// Generated message map functions
protected:

	//{{AFX_MSG(CInputEdit)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditClearall();
	afx_msg void OnUpdateEditClearall(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);


	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

private:
	CString m_strPrompt;
	CString m_strLastPrompt;
	BOOL m_bLButtonDown;
	CPoint m_ptLButtonDown;
	CStringArray m_strHistory;
	int m_nHistPos;

	InputType m_nInputType;

	CBrush m_brush;
};


class COutputViewBar : public CDockablePane
{
public:
	typedef enum {
		inputNone = -1,
		inputKeyEnough = 0,
		inputKeyRequestValue = 1,
		inputKeyGetValue = 2,
		inputNeedMoreData = 3

	}InputState;

	struct SortItem{
		SortItem()
		{
			findIndex = -1;
			cmdid = 0;
		}
		SortItem& operator=(const SortItem& item)
		{
            text =  item.text;
			cmdid = item.cmdid;
			sortStr = item.sortStr;
			findIndex = item.findIndex;
			return *this;
		}
		CString text;//输出的文字
		int cmdid;//命令id
		CString sortStr;//排序用字符串
		int findIndex;//查找到的位置
	};
	typedef CArray<SortItem, SortItem> SortItemArray;
	COutputViewBar();
	virtual ~COutputViewBar();
	
	void LoadParams(const CUIParam *param);
	void ShowParamsWithoutKey(const CUIParam *param);
	void ShowKeyParams(const CUIParam *param);
	void ClearParams();
	void OutputMsg(LPCTSTR msg, BOOL bReplaceLastLine=FALSE);

	void InputValueForCommand();

	BOOL Keyin(LPCTSTR str, int flag=0);
	BOOL SendCmdKeyin(LPCTSTR str);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//快捷命令
	void LoadCustomizeKey();
	int GetCustomizeKeyCommand(CString str);
	
	// Generated message map functions
protected:
	void AdjustLayout();

	CString ParamAccelKeyToText(const CUIParam::ParamItem& item);
	CString ToText(const CUIParam::ParamItem& item);
	void UpdateParam(CValueTable& tab);

	BOOL MatchAccelKeyParam(LPCTSTR input, CUIParam::ParamItem& item, CValueTable& tab, InputState *state=NULL);

	BOOL MatchParam(LPCTSTR input, CUIParam::ParamItem& item, CValueTable& tab);
	BOOL MatchOneParam(const CUIParam::ParamItem& item, LPCTSTR input, CValueTable& tab);
	
	afx_msg LRESULT OnCharOne(WPARAM nChar, LPARAM nFlags);
	afx_msg LRESULT OnKeyOne(WPARAM nChar, LPARAM nFlags);
	afx_msg LRESULT OnKeyinString(WPARAM flag, LPARAM text);

	afx_msg void OnChangeEditFcode();

	void SortSearchRsltList(SortItemArray& arr);

	//{{AFX_MSG(COutputViewBar)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
	afx_msg LRESULT OnSearchListSelChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	COutputEdit m_wndOutEdit;
	CInputEdit m_wndInEdit;

protected:
	CUIParam *m_pParam;
	CFont m_Font;
	// 当前输入状态
	InputState m_nInputState;
	// 当前修改的参数
	CUIParam::ParamItem m_CurParamItem;

	CDlgInputCommand	*m_pWndSearchRsltList;
	BOOL		m_bChgFromSet;
	CustomizeKeyArray m_arrkeys;
	CString m_keyfilepath;

	BOOL m_bChangePrompt;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CBrush m_brush;
};

#endif // !defined(AFX_OUTPUTVIEWBAR_H__3095A6D2_3E8F_48AB_8312_9209282D9395__INCLUDED_)
