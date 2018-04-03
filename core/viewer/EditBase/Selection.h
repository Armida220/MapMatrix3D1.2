// EBSelection.h: interface for the CSelection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBSELECTION_H__628B5207_52C4_472D_82E9_9DB7847A5E39__INCLUDED_)
#define AFX_EBSELECTION_H__628B5207_52C4_472D_82E9_9DB7847A5E39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"
#include "FArray.hpp"
#include "CoordSys.h"

MyNameSpaceBegin

struct SELCHG_STATE
{
	SELCHG_STATE(FTR_HANDLE  hd=0, int s=-1)
	{
		handle = hd;
		tobesel= s;
	}	
	FTR_HANDLE  handle;
	// -1, invalid; 0, to be unselected; 1, to be selected
	int  tobesel;
};

#define SELCHG_INVALID		-1
#define SELCHG_TOBEUNSEL		0
#define SELCHG_TOBESEL		1


struct SELECTSETTINGS
{
	SELECTSETTINGS::SELECTSETTINGS()
	{
		lfRadius = 10;
	}
	double lfRadius;
};

struct SelNode
{
	SelNode(){
		handle = 0;
		prev = NULL; next = NULL;
	}
	FTR_HANDLE handle;
	SelNode *prev;
	SelNode *next;
};

class SelNodeList
{
public:
	SelNodeList();
	~SelNodeList();
	void delAll();
	
	void del(SelNode *nod);
	void add(SelNode *nod);
	
	SelNode *head, *tail;
};

class EXPORT_EDITBASE CSelection  
{
public:
	void SelectObj(const FTR_HANDLE *handles, int num);
	void EraseChangeFlag();
	void DeselectAllBut(FTR_HANDLE handle);
	int GetSelChgObjs(SELCHG_STATE *states)const;
	CSelection();
	virtual ~CSelection();

	double GetSelectRadius()const;
	void SetSelectRadius(double r);
	
	void SelectObj(FTR_HANDLE handle);
	void SelectAll(const FTR_HANDLE *handles, int num);

	void DeselectObj(FTR_HANDLE handle);
	void DeselectAll();
	
	const FTR_HANDLE *GetSelectedObjs(int &num);
	FTR_HANDLE GetLastSelectedObj()const;
	
	BOOL IsObjInSelection(FTR_HANDLE handle);

	void UpdateSettings(BOOL bSave);

	//在待选集中找到距离指定点最近的地物
	FTR_HANDLE GetNearestInCanSelObjs(PT_3D pt, CCoordSys *pCS);

	void RemoveOneFromCanSelObjs(FTR_HANDLE hFtr);

protected:
	//if objnum found, return TRUE, and insert_idx is its index;
	//if not found, return FALSE, and insert_idx is its suit pos inserted at, 
	//that's next pos to the pos this new obj wound be inserted at.
	BOOL FindObj(FTR_HANDLE handle, int& insert_idx);

	CArray<FTR_HANDLE,FTR_HANDLE> m_arrSelObjs;

	double m_lfSelRadius;
	CArray<SELCHG_STATE,SELCHG_STATE> m_arrSelChgObjs;

	// 从小到大排序
	CFArray<SelNode*> m_arrSortedSelObjs;
	
	// 地物的原始顺序
	SelNodeList m_arrSelObjList;

	// 获取选择的地物集的状态
	BOOL    m_bGetSelChanged;

public:
	
	//单击选择时，除最近地物之外，其他仍然落在选择范围中的地物的集合；
	//可以称之为待选集
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrCanSelObjs;
};

MyNameSpaceEnd

#endif // !defined(AFX_EBSELECTION_H__628B5207_52C4_472D_82E9_9DB7847A5E39__INCLUDED_)
