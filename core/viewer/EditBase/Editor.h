// EBEditor.h: interface for the CEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBEDITOR_H__EB28124E_DA6F_49FA_B169_742110C87070__INCLUDED_)
#define AFX_EBEDITOR_H__EB28124E_DA6F_49FA_B169_742110C87070__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EditDef.h"
#include "Feature.h"
#include "FtrLayer.h"
#include "Selection.h"
#include "CoordWnd.h"
#include "UndoAction.h"
#include "DataSourceEx.h"

MyNameSpaceBegin

class CDataQuery;
class CDataSourceEx;
class CUndoAction;
class CFtrLayer;

//CEditor 定义为编辑者，使用它可以修改对象、控制选择集、请求视图更新

class EXPORT_EDITBASE CEditor
{
public:
	
	CEditor();
	virtual ~CEditor();

	// manage data
	virtual CDataQueryEx* GetDataQuery();
	
	virtual BOOL AddObject(CPFeature pFtr, int layid = -1);	
	virtual BOOL RestoreObject(FTR_HANDLE handle);
	virtual BOOL DeleteObject(FTR_HANDLE handle, BOOL bUpdateSel = TRUE);
	virtual BOOL ClearObject(FTR_HANDLE handle, BOOL bUpdateSel = FALSE);
	virtual BOOL UpdateObject(FTR_HANDLE handle, BOOL bOnlyUpdateView = FALSE);
	virtual BOOL UpdateObjectKeepOrder(FTR_HANDLE handle, BOOL bOnlyUpdateView = FALSE){return TRUE;};

	virtual BOOL AddFtrLayer(CFtrLayer *pLayer);
	virtual BOOL DelFtrLayer(CFtrLayer *pLayer);
	virtual BOOL RestoreFtrLayer(CFtrLayer *pLayer);
	virtual BOOL UpdateFtrLayer(CFtrLayer *pLayer);
	virtual BOOL ModifyLayer(CFtrLayer *pLayer, LPCTSTR field, _variant_t& value, BOOL bUndo=TRUE, BOOL bAllView=TRUE);
	virtual BOOL ModifyLayerGroup(FtrLayerGroup *pGroup, LPCTSTR field, _variant_t& value, BOOL bUndo=TRUE, BOOL bAllView=TRUE);
	
	// manage undo/redo
	virtual void SetUndo(CUndoAction *pAction);
	void Undo(int nStep=1);
	void Redo(int nStep=1);

	void GetUndoRedoActions(CStringList& lstActions, BOOL bUndo) const;
	
	// manage select & selection
	virtual void OpenSelector(int mode=SELMODE_NORMAL, int filter=SELFILTER_ALL);
	virtual void CloseSelector();
	
	CSelection *GetSelection()const{
		return (CSelection*)(&m_selection);
	}
	virtual void SelectObj(FTR_HANDLE handle,BOOL bUpdateSel = FALSE);
	virtual void DeselectObj(FTR_HANDLE handle,BOOL bUpdateSel = FALSE);
	virtual void SelectObj(const FTR_HANDLE *handles, int num,BOOL bUpdateSel = FALSE);
	virtual void SelectAll();
	virtual void DeselectAll();
	
	// manage updating
	virtual BOOL UIRequest(long reqtype, LONG_PTR param);
	virtual void RefreshView();
	virtual void UpdateDrag(long updatecode, const GrBuffer *pBuf=NULL, UpdateViewType updateview=uv_AllView);
	virtual void SetCrossPos(PT_3D pt);
	
	void SetCoordWnd(CCoordWnd coordwnd){
		m_coordwnd = coordwnd;
	}

	virtual int GetFtrLayerIDOfFtr(FTR_HANDLE handle);
	CDataSourceEx* GetDataSourceOfFtr(CPFeature pFtr);

	BOOL IsSelectorOpen();
//	virtual void  SetCurDrawingObj(struct DrawingInfo info);

	CCoordWnd GetCoordWnd(){
		return m_coordwnd;
	}
	virtual void OnSelectChanged(BOOL bAlways=FALSE);

	int GetSelectorMode();
public:
	// manage data
	virtual CDataSourceEx* GetDataSource(int idx=-1);

	int GetDataSourceCount();

	int GetActiveDataSourceIdx()const{
		return m_nActiveData;
	}

	virtual void SetCursorType(int type);
	virtual void UpdateView(long viewid, long updateid, LONG_PTR param);
	void DeleteUndoItems(int nStartPos=0);

//	virtual void ClearObjectsOfUndoItem(CUndoAction *pAction);	
	
	virtual BOOL FilterSelect(FTR_HANDLE handle);

	virtual void UndoOneStep(CUndoAction *pAction);
	virtual void RedoOneStep(CUndoAction *pAction);

	virtual void CloseAll();
	
public:

	CDataQueryEx *m_pDataQuery;

	CArray<CDataSourceEx*, CDataSourceEx*> m_arrData;
	int   m_nActiveData; //活动数据源索引
	
	CSelection	m_selection;
	
	//选择器是否开启，如果开启，视图先完成选择，再做其他操作
	BOOL m_bSelectorOpen;
	
	//选择器的模式；0，正常模式，即，点击时，单选；按住Ctrl/Shift时多选
	//1，单选模式，即，每次选择的结果最多只能有一个对象选中；
	//2，多选模式，即，每次点击中的地物，不在选择集中的，加到选择集，已经选中的，踢出选择集
	int  m_nSelectMode;

	//选择器是否正在拉框
//	BOOL m_bSelDrag;
	
	int  m_nSelectFilter;
	
	//选择器是否正在拉框
	BOOL m_bSelectDrag;
	
	//选择器拉框的起点
	PT_3D m_ptSelDragStart;

	CArray<PT_3D,PT_3D> m_arrSelDragPts;
	
	CCoordWnd		m_coordwnd;
	
	//Undo 队列
	CArray<CUndoAction*,CUndoAction*> m_arrUndos;
	//指向当前的一个Undo项，
	//所有下标大于m_nCurUndoPos的Undo项都是已经被Undo过的；
	//所有下标小或者等于m_nCurUndoPos的Undo项都是还没有被Undo过的
	int m_nCurUndoPos;	
};

MyNameSpaceEnd

#endif // !defined(AFX_EBEDITOR_H__EB28124E_DA6F_49FA_B169_742110C87070__INCLUDED_)
