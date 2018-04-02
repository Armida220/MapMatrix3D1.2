// EBWorker.cpp: implementation of the CEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "Worker.h"
#include "matrix.h"
#include "GeoPoint.h"
#include "GeoSurface.h"
#include "GeoCurve.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEditor::CEditor()
{
	m_nCurUndoPos = -1;
	m_bSelectDrag = FALSE;
	m_bSelectorOpen = FALSE;
	m_nSelectMode = SELMODE_NORMAL;
	m_nSelectFilter = SELFILTER_ALL;

	m_pDataQuery = NULL;
	m_nActiveData = 0;
}

CEditor::~CEditor()
{
	CloseAll();
	DeleteUndoItems();
}


void CEditor::CloseAll()
{	
	for( int i=m_arrData.GetSize()-1; i>=0; i--)
	{
		CDataSourceEx *pDS = m_arrData.GetAt(i);

		if(pDS!= NULL)
		{
			delete pDS;
		}
	}
	
	m_arrData.RemoveAll();	
	
	if( m_pDataQuery )
	{
		delete m_pDataQuery;
		m_pDataQuery = NULL;
	}
}


BOOL CEditor::AddObject(CPFeature pFtr, int layid)
{	
 	CDataSourceEx *pDS = GetDataSource();
	if( !pDS )return FALSE;

 	ASSERT(pDS);
 	BOOL bRet = pDS->AddObject(pFtr,layid);
	if( bRet )UpdateView(0,hc_AddObject,(LPARAM)(pFtr));
	return TRUE;
}


BOOL CEditor::DeleteObject(FTR_HANDLE handle, BOOL bUpdateSel)
{
	CDataSource *pDS = GetDataSource();
	CDataQuery  *pDQ = GetDataQuery();
	ASSERT(pDS);
	if( bUpdateSel && m_selection.IsObjInSelection(handle) )
	{
		m_selection.DeselectObj(handle);
		//if( bUpdateSel )OnSelectChanged();
	}

	if( handle )UpdateView(0,hc_DelObject,(LPARAM)(HandleToFtr(handle)));

	if( !pDS->DeleteObject(HandleToFtr(handle)) )
		return FALSE;

	return TRUE;
}

BOOL CEditor::RestoreObject(FTR_HANDLE handle)
{
	CDataSource *pDS = GetDataSource();	
	CDataQuery  *pDQ = GetDataQuery();
	ASSERT(pDS);
	if( !pDS->RestoreObject(HandleToFtr(handle)) )
		return FALSE;
	UpdateView(0,hc_AddObject,(LPARAM)(HandleToFtr(handle)));
	return TRUE;
}

BOOL CEditor::ClearObject(FTR_HANDLE handle, BOOL bUpdateSel)
{
	CDataSource *pDS = GetDataSource();
	CDataQuery  *pDQ = GetDataQuery();

	if (!pDS)  return FALSE;
/*
	if( m_selection.IsObjInSelection(handle) )
	{
		m_selection.DeselectObj(handle);
		if( bUpdateSel )OnSelectChanged();
	}

	if( handle )UpdateView(0,hc_DelObject,(LPARAM)(HandleToFtr(handle)));
*/

	if( !pDS->ClearObject(HandleToFtr(handle)) )
		return FALSE;
	
	return TRUE;
}


BOOL CEditor::UpdateObject(FTR_HANDLE handle, BOOL bOnlyUpdateView)
{
	if( DeleteObject(handle) && RestoreObject(handle) )
		return TRUE;
	return FALSE;
}

void CEditor::OpenSelector(int mode, int filter)
{
	m_bSelectorOpen = TRUE;
	m_nSelectMode = mode;
	m_nSelectFilter = filter;
	
	SetCursorType(CURSOR_SELECT);
}


void CEditor::CloseSelector()
{
	m_bSelectDrag	= FALSE;
	m_bSelectorOpen = FALSE;
	SetCursorType(CURSOR_DRAW);
}


BOOL CEditor::FilterSelect(FTR_HANDLE handle)
{
	if( handle == 0 )
		return FALSE;

	if( m_nSelectFilter==SELFILTER_ALL )
		return TRUE;

	CDataQuery *pDQ = GetDataQuery();
	if( !pDQ->IsManagedObj(HandleToFtr(handle)) )
		return FALSE;

	CGeometry *pGeo = (HandleToFtr(handle))->GetGeometry();
	if( !pGeo )return FALSE;

	if( m_nSelectFilter&SELFILTER_POINT )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			return TRUE;
	}
	if( m_nSelectFilter&SELFILTER_CURVE )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			return TRUE;
	}
	if( m_nSelectFilter&SELFILTER_SURFACE )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			return TRUE;
	}
	return FALSE;
}


void CEditor::SelectObj(FTR_HANDLE handle,BOOL bUpdateSel)
{
	m_selection.SelectObj(handle);
	if(bUpdateSel)
		OnSelectChanged();
}

void CEditor::DeselectObj(FTR_HANDLE handle,BOOL bUpdateSel)
{
	m_selection.DeselectObj(handle);
	if(bUpdateSel)
		OnSelectChanged();
}

void CEditor::SelectObj(const FTR_HANDLE *handles, int num,BOOL bUpdateSel )
{	
	m_selection.SelectObj(handles,num);
	if(bUpdateSel)
		OnSelectChanged();
}

void CEditor::SelectAll()
{
}


void CEditor::DeselectAll()
{
	m_selection.DeselectAll();
	OnSelectChanged();
}

void CEditor::OnSelectChanged(BOOL bAlways)
{
	UpdateView(0,hc_SelChanged,bAlways);
	m_selection.EraseChangeFlag();
}

void CEditor::SetCursorType(int type)
{
	UpdateView(NULL,hc_SetCursorType,type);
}

void CEditor::UpdateView(long viewid, long updateid, LONG_PTR param)
{
}

void CEditor::RefreshView()
{
	UpdateView(NULL,hc_Refresh,0);
}


void CEditor::UpdateDrag(long updatecode, const GrBuffer *pBuf, UpdateViewType updateview)
{
}


void CEditor::SetCrossPos(PT_3D pt)
{
	UpdateView(NULL, hc_SetCrossPos, (LONG_PTR)&pt);
}



BOOL CEditor::UIRequest(long reqtype, LONG_PTR param)
{
	return TRUE;
}


void CEditor::DeleteUndoItems(int nStartPos)
{
	int nsz = m_arrUndos.GetSize();
	if( nStartPos<0 )nStartPos = 0;
	for( int i=nsz-1; i>=nStartPos; i--)
	{
//		ClearObjectsOfUndoItem(m_arrUndos.GetAt(i));
		if (i <= m_nCurUndoPos)
		{
			m_arrUndos.GetAt(i)->DestoryOldObjs();
		}
		else
		{
			m_arrUndos.GetAt(i)->DestoryNewObjs();
		}
		
		delete m_arrUndos.GetAt(i);

		m_arrUndos.RemoveAt(i);
	}

	m_nCurUndoPos = nStartPos - 1;
}


// void CEditor::ClearObjectsOfUndoItem(CUndoAction *pAction)
// {
// 	pAction->DestoryNewObjs();
// }


// void CEditor::SetCurDrawingObj(DrawingInfo info)
// {
// 	
// }

void CEditor::SetUndo(CUndoAction *pAction)
{
	if( !pAction )return;
	CUndoAction *pNew = pAction->Clone();
	if( !pNew )return;

	DeleteUndoItems(m_nCurUndoPos+1);

	m_arrUndos.Add(pNew);
	m_nCurUndoPos++;
}


void CEditor::Undo(int nStep)
{
	if( m_nCurUndoPos<0 )
		return;
	
	if( m_nCurUndoPos+1<nStep )
		nStep = m_nCurUndoPos+1;

	int endpos = m_nCurUndoPos+1-nStep, i;

// 	CDataSource *pDS = GetDataSource();
// 	CDataQuery  *pDQ = GetDataQuery();
/*	ASSERT(pDS);*/

	for( i=m_nCurUndoPos; i>=endpos; i--)
	{
		CUndoAction *pAction = m_arrUndos.GetAt(i);
		UndoOneStep(pAction);
	}

	OnSelectChanged(TRUE);

	m_nCurUndoPos = i;
	RefreshView();
}


void CEditor::Redo(int nStep)
{
	int nsum = m_arrUndos.GetSize();
	if( m_nCurUndoPos>=nsum-1 )
		return;
	
	if( m_nCurUndoPos+nStep>=nsum )
		nStep = nsum-1-m_nCurUndoPos;
	
	int endpos = m_nCurUndoPos+nStep, i;
	
// 	CDataSource *pDS = GetDataSource();
// 	CDataQuery  *pDQ = GetDataQuery();
// 	ASSERT(pDS);
	
	for( i=m_nCurUndoPos+1; i<=endpos; i++)
	{
		CUndoAction *pAction = m_arrUndos.GetAt(i);
		RedoOneStep(pAction);
	}

	OnSelectChanged(TRUE);
	
	m_nCurUndoPos = i-1;
	RefreshView();
}


void CEditor::UndoOneStep(CUndoAction *pAction)
{
	pAction->Undo();
}


void CEditor::RedoOneStep(CUndoAction *pAction)
{
	pAction->Redo();
}

void CEditor::GetUndoRedoActions(CStringList& lstActions, BOOL bUndo) const
{
	int nidx;
	
	//	Get undo/redo actions:
	lstActions.RemoveAll();
	
	if (bUndo)
	{
		nidx = 0;
		while (nidx<=m_nCurUndoPos)
		{
			lstActions.AddHead(m_arrUndos.GetAt(nidx)->m_strName);
			nidx++;
		}
	}
	else
	{
		nidx = m_arrUndos.GetSize()-1;
		while (nidx>m_nCurUndoPos)
		{
			lstActions.AddHead(m_arrUndos.GetAt(nidx)->m_strName);
			nidx--;
		}
	}
}

BOOL CEditor::IsSelectorOpen()
{
	return m_bSelectorOpen;
}

int CEditor::GetSelectorMode()
{
	return m_nSelectMode;
}


BOOL CEditor::AddFtrLayer(CFtrLayer *pLayer)
{
	if( !pLayer )return FALSE;

	CDataSourceEx *pDS = GetDataSource();
	if( !pDS )return FALSE;

	if( !pDS->AddFtrLayer(pLayer) )
		return FALSE;

	return TRUE;
}


BOOL CEditor::DelFtrLayer(CFtrLayer *pLayer)
{
	if( !pLayer )return FALSE;
	
	CDataSourceEx *pDS = GetDataSource();
	if( !pDS )return FALSE;

	pDS->DelFtrLayer(pLayer);	
	
	return TRUE;
}


BOOL CEditor::UpdateFtrLayer(CFtrLayer *pLayer)
{
	if( DelFtrLayer(pLayer) && RestoreFtrLayer(pLayer) )
		return TRUE;

	return FALSE;
}



BOOL CEditor::RestoreFtrLayer(CFtrLayer *pLayer)
{
	if( !pLayer )return FALSE;
	
	CDataSourceEx *pDS = GetDataSource();
	if( !pDS )return FALSE;
	
	pDS->RestoreLayer(pLayer);	
	
	return TRUE;
}


BOOL CEditor::ModifyLayer(CFtrLayer *pLayer, LPCTSTR field, _variant_t& value, BOOL bUndo, BOOL bAllView)
{
	CDataSourceEx *pDS = GetDataSource();
	if(!pDS) return FALSE;
    if (_tcsicmp(field,FIELDNAME_LAYERNAME)==0)
    {	
		pLayer->SetName((LPCTSTR)(_bstr_t)value);
    }
	else if (_tcsicmp(field,FIELDNAME_LAYGROUPNAME)==0)
    {	
		pLayer->SetGroupName((LPCTSTR)(_bstr_t)value);
    }
    else if (_tcsicmp(field,FIELDNAME_LAYMAPNAME)==0)
    {	
		pLayer->SetMapName((LPCTSTR)(_bstr_t)value);
    }
	else if (_tcsicmp(field,FIELDNAME_LAYLOCKED)==0)
    {		
		pLayer->EnableLocked((bool)value);
    }
	else if (_tcsicmp(field,FIELDNAME_LAYVISIBLE)==0)
    {	
		if (bAllView)
		{
			pLayer->EnableVisible((bool)value);
		}
		
		UpdateDispyParam param;
		param.type = UpdateDispyParam::typeVISIBLE;
		param.handle = (LONG_PTR)pLayer;
		param.data.bVisible = (bool)value;
		
		UpdateView(0, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
	}
	else if (_tcsicmp(field,FIELDNAME_LAYCOLOR)==0)
	{	
		pLayer->SetColor((long)(value));
		UpdateDispyParam param;
		param.type = UpdateDispyParam::typeCOLOR;
		param.handle = (LONG_PTR)pLayer;
		param.data.lColor = (long)(value);

		UpdateView(0, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
	}
	else if (_tcsicmp(field,FIELDNAME_LAYERDISPLAYORDER)==0)
    {		
		pLayer->SetDisplayOrder((long)value);

		UpdateView(0,hc_UpdateLayerDisplayOrder,0);
    }
	else if (_tcsicmp(field,FIELDNAME_LAYERSYMBOLIZED)==0)
    {	
		pLayer->EnableSymbolized((bool)value);		
    }
	
	if( bUndo )
	{
		BOOL bCurLayer = pLayer==pDS->GetCurFtrLayer()?TRUE:FALSE;

		UpdateFtrLayer(pLayer);	
	
		if (bCurLayer)
		{
			pDS->SetCurFtrLayer(pLayer->GetID());
		}
	}

	return TRUE;
}


BOOL CEditor::ModifyLayerGroup(FtrLayerGroup *pGroup, LPCTSTR field, _variant_t& value, BOOL bUndo, BOOL bAllView)
{
	CDataSourceEx *pDS = GetDataSource();
	if(!pDS) return FALSE;

    if (_tcsicmp(field,FIELDNAME_LAYGROUPNAME)==0)
	{
		pGroup->Name = (LPCTSTR)(_bstr_t)value;	
	}
	else if (_tcsicmp(field,FIELDNAME_LAYVISIBLE)==0)
	{
		pGroup->Visible = (long)value;							
	}
	else if (_tcsicmp(field,FIELDNAME_LAYCOLOR)==0)
	{
		pGroup->Color = (long)value;			
	}
	else if (_tcsicmp(field,FIELDNAME_LAYERSYMBOLIZED)==0)
	{
		pGroup->Symbolized = (long)value;			
	}

	pDS->UpdateFtrLayerGroup(pGroup);
	
	CUndoModifyLayer undo(this,_T("ModifyLayerGroup"));
	undo.field.Format(_T("%s"),field);
	undo.newVar = value;

	for (int j=0; j<pGroup->ftrLayers.GetSize(); j++)
	{
		_variant_t data;
		CFtrLayer *pLayer = pGroup->ftrLayers[j];
		if (_tcsicmp(field,FIELDNAME_LAYGROUPNAME)==0)
		{
			data = pGroup->Name;	
		}
		else if (_tcsicmp(field,FIELDNAME_LAYVISIBLE)==0)
		{
			data = (bool)pLayer->IsVisible();							
		}
		else if (_tcsicmp(field,FIELDNAME_LAYCOLOR)==0)
		{
			data = (long)pLayer->GetColor();			
		}
		else if (_tcsicmp(field,FIELDNAME_LAYERSYMBOLIZED)==0)
		{
			data = (bool)pLayer->IsSymbolized();			
		}
		
		undo.arrLayers.Add(pLayer);
		undo.arrOldVars.Add(data);
		ModifyLayer(pLayer,field,value,bUndo);
		
	}
	
	if (bUndo)
	{
		undo.Commit();
	}

	return TRUE;
}



CDataSourceEx* CEditor::GetDataSource(int idx)
{
	if( m_arrData.GetSize()<=0 )return NULL;
	if (idx==-1)
	{
		if( m_nActiveData>=0 && m_nActiveData<m_arrData.GetSize() )
			return m_arrData.GetAt(m_nActiveData);
		else
			return NULL;
	}	
	if ( idx<0||idx>=m_arrData.GetSize() ) return NULL;
	return m_arrData.GetAt(idx);
}


int CEditor::GetDataSourceCount()
{
	return m_arrData.GetSize();
}



CDataSourceEx* CEditor::GetDataSourceOfFtr(CPFeature pFtr)
{
	LONG_PTR info = 0;
	if( m_pDataQuery->GetObjectInfo(pFtr,0,info) )
	{
		return (CDataSourceEx*)info;
	}

	return NULL;
}


int CEditor::GetFtrLayerIDOfFtr(FTR_HANDLE handle)
{
	CFtrLayer *pLayer = GetDataSource()->GetFtrLayerOfObject(HandleToFtr(handle));
	if (pLayer)
	{
		return pLayer->GetID();
	}
	return -1;

}



CDataQueryEx* CEditor::GetDataQuery()
{
	return m_pDataQuery;
}



MyNameSpaceEnd






