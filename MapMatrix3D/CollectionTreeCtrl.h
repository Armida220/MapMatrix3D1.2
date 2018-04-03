// CollectionTreeCtrl.h: interface for the CCollectionTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLECTIONTREECTRL_H__53814169_0D75_414A_A0BD_FADB722C11F7__INCLUDED_)
#define AFX_COLLECTIONTREECTRL_H__53814169_0D75_414A_A0BD_FADB722C11F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIEXTENSION\TreeCtrlEx.h"


#include "Scheme.h"
#include <vector>
#include <algorithm>
#include "SymbolLib.h"

using namespace std;
/////////////////////////////////////////////////////////////////////////////
#define GROUPID_FROMIDX(x)		((x<<16)|0xffff)
#define GROUPID_TOIDX(x)		(x>>16)
#define CODEID_FROMIDX(x)		(x)
#define CODEID_TOIDX(x)			(x)




extern BOOL IsValidConfigPath(CString path);//hcw,2012.7.23
extern BOOL CheckLayerFormat(CString strLayerName);
extern BOOL FindNum(__int64 num, vector<__int64> nums);
extern CString CheckName(const CString &strCheckedName,vector<CString> &names);


class CCollectionTreeCtrl : public CTreeCtrlEx  
{
public:
	CCollectionTreeCtrl();
	virtual ~CCollectionTreeCtrl();
	
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	//添加树控件可接收开始编辑和结束编辑消息
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	

	
protected:
	//{{AFX_MSG(CCollectionTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	
};

#endif // !defined(AFX_COLLECTIONTREECTRL_H__53814169_0D75_414A_A0BD_FADB722C11F7__INCLUDED_)
