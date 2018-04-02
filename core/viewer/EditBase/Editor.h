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

//CEditor ����Ϊ�༭�ߣ�ʹ���������޸Ķ��󡢿���ѡ�񼯡�������ͼ����

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
	int   m_nActiveData; //�����Դ����
	
	CSelection	m_selection;
	
	//ѡ�����Ƿ����������������ͼ�����ѡ��������������
	BOOL m_bSelectorOpen;
	
	//ѡ������ģʽ��0������ģʽ���������ʱ����ѡ����סCtrl/Shiftʱ��ѡ
	//1����ѡģʽ������ÿ��ѡ��Ľ�����ֻ����һ������ѡ�У�
	//2����ѡģʽ������ÿ�ε���еĵ������ѡ���еģ��ӵ�ѡ�񼯣��Ѿ�ѡ�еģ��߳�ѡ��
	int  m_nSelectMode;

	//ѡ�����Ƿ���������
//	BOOL m_bSelDrag;
	
	int  m_nSelectFilter;
	
	//ѡ�����Ƿ���������
	BOOL m_bSelectDrag;
	
	//ѡ������������
	PT_3D m_ptSelDragStart;

	CArray<PT_3D,PT_3D> m_arrSelDragPts;
	
	CCoordWnd		m_coordwnd;
	
	//Undo ����
	CArray<CUndoAction*,CUndoAction*> m_arrUndos;
	//ָ��ǰ��һ��Undo�
	//�����±����m_nCurUndoPos��Undo����Ѿ���Undo���ģ�
	//�����±�С���ߵ���m_nCurUndoPos��Undo��ǻ�û�б�Undo����
	int m_nCurUndoPos;	
};

MyNameSpaceEnd

#endif // !defined(AFX_EBEDITOR_H__EB28124E_DA6F_49FA_B169_742110C87070__INCLUDED_)
