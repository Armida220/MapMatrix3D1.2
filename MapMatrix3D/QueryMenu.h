// QueryMenu.h: interface for the CQueryMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYMENU_H__6F8881AE_A962_4A49_8F3F_73DD976FCA85__INCLUDED_)
#define AFX_QUERYMENU_H__6F8881AE_A962_4A49_8F3F_73DD976FCA85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgConditionSelect.h"

extern void LoadCondCfg(LPCTSTR fileName, CArray<CONDSEL,CONDSEL>& arrConds);
extern void SaveCondCfg(LPCTSTR fileName, CArray<CONDSEL,CONDSEL>& arrConds);

struct CondMenu
{
	CString name;
	CArray<CONDSEL,CONDSEL> arrConds;
};
class CQueryMenu  
{
public:
	CQueryMenu();
	virtual ~CQueryMenu();
	void CopyCondData(CQueryMenu& menu);
	void SetDoc(CDocument *pDoc);
	void DeleteAll();
	void Load(LPCTSTR filename=NULL);
	void Save(LPCTSTR filename=NULL);
	BOOL IsLoaded();
	void GetQuery(UINT id, CArray<CONDSEL,CONDSEL>& arr);
	void UpdateWindow();
	void ClearCondData();

	HMENU m_hMenu;
	CPtrArray m_arrMenus;
};

#endif // !defined(AFX_QUERYMENU_H__6F8881AE_A962_4A49_8F3F_73DD976FCA85__INCLUDED_)
