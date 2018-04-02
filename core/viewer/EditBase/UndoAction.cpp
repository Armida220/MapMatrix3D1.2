// UndoAction.cpp: implementation of the CUndoAction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoAction.h"
#include "GeoSurface.h"


MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoAction::CUndoAction()
{
	m_pEditor = NULL;
}


CUndoAction::CUndoAction(CEditor *p, LPCTSTR name)
{
	m_strName = name;
	m_pEditor = p;
}

CUndoAction::~CUndoAction()
{

}

void CUndoAction::CopyFrom(const CUndoAction *pObj)
{
	m_pEditor = pObj->m_pEditor;
	m_strName = pObj->m_strName;
}


void CUndoAction::Commit()
{
	if( m_pEditor!=NULL )
	{
		m_pEditor->SetUndo(this);
	}
}

CUndoBatchAction::CUndoBatchAction():CUndoAction(NULL,NULL)
{
}


CUndoBatchAction::CUndoBatchAction(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
}

CUndoBatchAction::~CUndoBatchAction()
{
	for (int i=0; i<arrActions.GetSize(); i++)
	{
		delete arrActions.GetAt(i);
	}
	arrActions.RemoveAll();
}

void CUndoBatchAction::Undo()
{
	for (int i=arrActions.GetSize()-1; i>=0; i--)
	{
		arrActions.GetAt(i)->Undo();
	}
}


void CUndoBatchAction::Redo()
{
	for (int i=0; i<arrActions.GetSize(); i++)
	{
		arrActions.GetAt(i)->Redo();
	}
}


void CUndoBatchAction::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);
	
	for (int i=0; i<((CUndoBatchAction*)pObj)->arrActions.GetSize(); i++)
	{
		CUndoAction *pNew = ((CUndoBatchAction*)pObj)->arrActions.GetAt(i)->Clone();
		arrActions.Add(pNew);
	}
}


void CUndoBatchAction::Commit()
{
	if( m_pEditor!=NULL && arrActions.GetSize()>0 )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoBatchAction::DestoryNewObjs()
{
	for (int i=0; i<arrActions.GetSize(); i++)
	{
		arrActions.GetAt(i)->DestoryNewObjs();
	}
}

void CUndoBatchAction::AddAction(CUndoAction* pAction)
{
	if( !pAction )return;
	CUndoAction *pNew = pAction->Clone();
	if( !pNew )return;
	arrActions.Add(pNew);
}


CUndoFtrs::CUndoFtrs():CUndoAction(NULL,NULL)
{
}

CUndoFtrs::CUndoFtrs(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
}

CUndoFtrs::~CUndoFtrs()
{
}

void CUndoFtrs::Undo()
{
	const FTR_HANDLE *buf;
	int  j, nsz;
	
	nsz = arrNewHandles.GetSize();
	buf = arrNewHandles.GetData();
	for( j=0; j<nsz; j++)
	{
		m_pEditor->DeleteObject(buf[j]);
	}
	
	nsz = arrOldHandles.GetSize();
	buf = arrOldHandles.GetData();
	for( j=0; j<nsz; j++)
	{
		m_pEditor->RestoreObject(buf[j]);
	}
}


void CUndoFtrs::Redo()
{
	const FTR_HANDLE *buf;
	int  j, nsz;
	
	CUndoFtrs *po = this;

	nsz = po->arrOldHandles.GetSize();
	buf = po->arrOldHandles.GetData();
	for( j=0; j<nsz; j++)
	{
		m_pEditor->DeleteObject(buf[j]);
	}
	
	nsz = po->arrNewHandles.GetSize();
	buf = po->arrNewHandles.GetData();
	for( j=0; j<nsz; j++)
	{
		m_pEditor->RestoreObject(buf[j]);
	}
}


void CUndoFtrs::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	arrNewHandles.Copy(((CUndoFtrs*)pObj)->arrNewHandles);
	arrOldHandles.Copy(((CUndoFtrs*)pObj)->arrOldHandles);
}


void CUndoFtrs::Commit()
{
	if( m_pEditor!=NULL && 
		(arrNewHandles.GetSize()>0 || arrOldHandles.GetSize()>0) )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoFtrs::DestoryNewObjs()
{
	const FTR_HANDLE *buf;
	int  j, nsz;
	
	CUndoFtrs *po = this;
	
	nsz = po->arrNewHandles.GetSize();
	buf = po->arrNewHandles.GetData();
	for( j=0; j<nsz; j++)
	{
		m_pEditor->ClearObject(buf[j]);
	}
}

void CUndoFtrs::DestoryOldObjs()
{
	const FTR_HANDLE *buf;
	int  j, nsz;
	
	CUndoFtrs *po = this;
	
	nsz = po->arrOldHandles.GetSize();
	buf = po->arrOldHandles.GetData();
	for( j=0; j<nsz; j++)
	{
		m_pEditor->ClearObject(buf[j]);
	}
}

void CUndoFtrs::DelInvalidUndoInfo()
{
	for (int i=arrNewHandles.GetSize()-1;i>=0;i--)
	{
		for (int j=arrOldHandles.GetSize()-1;j>=0;j--)
		{
			if (arrNewHandles[i]==arrOldHandles[j])
			{
				m_pEditor->ClearObject(arrNewHandles[i]);
				arrNewHandles.RemoveAt(i);
				arrOldHandles.RemoveAt(j);
				break;
			}
		}
	}
}
void CUndoFtrs::AddNewFeature(FTR_HANDLE handle)
{
	arrNewHandles.Add(handle);
}


void CUndoFtrs::AddOldFeature(FTR_HANDLE handle)
{
	arrOldHandles.Add(handle);
}



CUndoVertex::CUndoVertex():CUndoAction(NULL,NULL)
{
}

CUndoVertex::CUndoVertex(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
	handle = NULL;
	nPtType = nPtIdx = -1;
	bRepeat = FALSE;
}

CUndoVertex::~CUndoVertex()
{
}

void CUndoVertex::Undo()
{
	CUndoVertex *po = this;
	CFeature * pFt = HandleToFtr(po->handle);
	CGeometry *pGeo = pFt->GetGeometry();

	m_pEditor->DeleteObject(po->handle);

	if( po->nPtType==PT_KEYCTRL::typeKey )
	{
		// modify vertex
		if( po->ptOld.pencode!=penNone && po->ptNew.pencode!=penNone )
		{
			pGeo->SetDataPoint(po->nPtIdx,po->ptOld);
			if (bRepeat)
			{
				if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
				{
					((CGeoMultiSurface*)pGeo)->SetClosedPt(po->nPtIdx,po->ptOld,TRUE);
				}
				else if (po->nPtIdx)
				{
					pGeo->SetDataPoint(0,po->ptOld);
				}
				else
				{
					pGeo->SetDataPoint(pGeo->GetDataPointSum()-1,po->ptOld);
				}
			}
		}
		// insert vertex
		else if( po->ptOld.pencode==penNone && po->ptNew.pencode!=penNone )
		{
			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			arrPts.RemoveAt(po->nPtIdx);
			pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
		}
		// delete vertex
		else if( po->ptOld.pencode!=penNone && po->ptNew.pencode==penNone )
		{
			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			arrPts.InsertAt(po->nPtIdx,po->ptOld);
			if (bRepeat)
			{
				if (po->nPtIdx)
				{
					arrPts.InsertAt(0,po->ptOld);
				}
				else
				{
					arrPts.SetAt(arrPts.GetSize()-1,po->ptOld);
				}
			}
			pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
		}
	}
	else
	{
		PT_3D pt3d;
		COPY_3DPT(pt3d,po->ptOld);
		pGeo->SetCtrlPoint(po->nPtIdx,pt3d);
	}
	
	m_pEditor->RestoreObject(po->handle);
}


void CUndoVertex::Redo()
{
	CFeature *pFt = HandleToFtr(handle);
	CGeometry *pGeo = pFt->GetGeometry();

	m_pEditor->DeleteObject(handle);

	if( nPtType==PT_KEYCTRL::typeKey )
	{
		// modify vertex
		if( ptOld.pencode!=penNone && ptNew.pencode!=penNone )
		{
			pGeo->SetDataPoint(nPtIdx,ptNew);
			if (bRepeat)
			{
				if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
				{
					((CGeoMultiSurface*)pGeo)->SetClosedPt(nPtIdx,ptNew,TRUE);
				}
				else if (nPtIdx)
				{
					pGeo->SetDataPoint(0,ptNew);
				}
				else
				{
					pGeo->SetDataPoint(pGeo->GetDataPointSum()-1,ptNew);
				}
			}
		}
		// insert vertex
		else if( ptOld.pencode==penNone && ptNew.pencode!=penNone )
		{
			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			arrPts.InsertAt(nPtIdx,ptNew);
			pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
		}
		// delete vertex
		else if( ptOld.pencode!=penNone && ptNew.pencode==penNone )
		{
			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			arrPts.RemoveAt(nPtIdx);
			if (bRepeat)
			{
				if (nPtIdx)
				{
					arrPts.RemoveAt(0);
				}
				else
				{
					arrPts.RemoveAt(arrPts.GetSize()-1);
				}
			}
			pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
		}
	}
	else
	{
		PT_3D pt3d;
		COPY_3DPT(pt3d,ptNew);
		pGeo->SetCtrlPoint(nPtIdx,pt3d);
	}
	
	m_pEditor->RestoreObject(handle);
}


void CUndoVertex::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoVertex *po = (CUndoVertex*)pObj;
	handle = po->handle;
	nPtType = po->nPtType;
	nPtIdx = po->nPtIdx;
	ptOld = po->ptOld;
	ptNew = po->ptNew;
	bRepeat = po->bRepeat;
}



void CUndoVertex::Commit()
{
	if( handle!=NULL && m_pEditor!=NULL && nPtIdx>=0 )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoVertex::DestoryNewObjs()
{
}




CUndoShape::CUndoShape():CUndoAction(NULL,NULL)
{
}

CUndoShape::CUndoShape(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
}

CUndoShape::~CUndoShape()
{
}

void CUndoShape::Undo()
{
	CFeature * pFt = HandleToFtr(handle);
	CGeometry *pGeo = pFt->GetGeometry();

	m_pEditor->DeleteObject(handle);

	pGeo->CreateShape(arrPtsOld.GetData(),arrPtsOld.GetSize());
	
	m_pEditor->RestoreObject(handle);
}


void CUndoShape::Redo()
{
	CFeature * pFt = HandleToFtr(handle);
	CGeometry *pGeo = pFt->GetGeometry();
	
	m_pEditor->DeleteObject(handle);
	
	pGeo->CreateShape(arrPtsNew.GetData(),arrPtsNew.GetSize());
	
	m_pEditor->RestoreObject(handle);
}


void CUndoShape::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoShape *po = (CUndoShape*)pObj;
	handle = po->handle;
	arrPtsOld.Copy(po->arrPtsOld);
	arrPtsNew.Copy(po->arrPtsNew);
}



void CUndoShape::Commit()
{
	if( handle!=NULL && m_pEditor!=NULL )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoShape::DestoryNewObjs()
{
}



CUndoTransform::CUndoTransform():CUndoAction(NULL,NULL)
{
}


CUndoTransform::CUndoTransform(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
	matrix_toIdentity(matrix,4);
}

CUndoTransform::~CUndoTransform()
{
}

void CUndoTransform::Undo()
{
	const FTR_HANDLE *buf;
	int  j, nsz;

	CUndoTransform *po = this;
	nsz = po->arrHandles.GetSize();
	buf = po->arrHandles.GetData();
	double matrix2[16];
	matrix_reverse(po->matrix,4,matrix2);
	for( j=0; j<nsz; j++)
	{
		CFeature *pFt = HandleToFtr(buf[j]);
		m_pEditor->DeleteObject(buf[j]);
		pFt->GetGeometry()->Transform(matrix2);
		m_pEditor->RestoreObject(buf[j]);
		//m_pEditor->UpdateObject(buf[j]);
	}
}


void CUndoTransform::Redo()
{
	const FTR_HANDLE *handles;
	int  j, nsz;
	
	CUndoTransform *po = this;
	nsz = po->arrHandles.GetSize();
	handles = po->arrHandles.GetData();
	for( j=0; j<nsz; j++)
	{
		CFeature *pFt = HandleToFtr(handles[j]);
		m_pEditor->DeleteObject(handles[j],FALSE);
		pFt->GetGeometry()->Transform(po->matrix);
		m_pEditor->RestoreObject(handles[j]);
	}
}


void CUndoTransform::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoTransform *po = (CUndoTransform*)pObj;
	arrHandles.Copy(po->arrHandles);
	memcpy(matrix,po->matrix,sizeof(matrix));
}



void CUndoTransform::Commit()
{
	if( m_pEditor!=NULL && arrHandles.GetSize()>0 )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoTransform::DestoryNewObjs()
{
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUndoNewDelLayer::CUndoNewDelLayer():CUndoAction(NULL,NULL)
{
}

CUndoNewDelLayer::CUndoNewDelLayer(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
}

CUndoNewDelLayer::~CUndoNewDelLayer()
{
}

void CUndoNewDelLayer::Undo()
{
	int  j, nsz;
	
	CUndoNewDelLayer *po = this;
	if (po->isDel)
	{
		nsz = po->arrObjs.GetSize();
		for (j=0;j<nsz;j++)
		{
			m_pEditor->RestoreFtrLayer(po->arrObjs[j]);
		}			
	}
	else
	{
		nsz = po->arrObjs.GetSize();
		for (j=0;j<nsz;j++)
		{
			m_pEditor->DelFtrLayer(po->arrObjs[j]);
		}
	}
}


void CUndoNewDelLayer::Redo()
{
	int  j, nsz;
		
	CUndoNewDelLayer *po = this;
	if (po->isDel)
	{
		nsz = po->arrObjs.GetSize();
		for (j=0;j<nsz;j++)
		{
			m_pEditor->DelFtrLayer(po->arrObjs[j]);
		}			
	}
	else
	{
		nsz = po->arrObjs.GetSize();
		for (j=0;j<nsz;j++)
		{
			m_pEditor->RestoreFtrLayer(po->arrObjs[j]);
		}
	}
}


void CUndoNewDelLayer::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoNewDelLayer *po = (CUndoNewDelLayer*)pObj;

	isDel = po->isDel;
	arrObjs.Copy(po->arrObjs);
}


void CUndoNewDelLayer::Commit()
{
	if( m_pEditor!=NULL && 
		arrObjs.GetSize()>0 )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoNewDelLayer::DestoryNewObjs()
{
}




CUndoModifyLayer::CUndoModifyLayer():CUndoAction(NULL,NULL)
{
}

CUndoModifyLayer::CUndoModifyLayer(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
	
}

CUndoModifyLayer::~CUndoModifyLayer()
{
}

void CUndoModifyLayer::Undo()
{	
	int i;
	if (_tcscmp(field,FIELDNAME_LAYERNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetName((LPCTSTR)(_bstr_t)arrOldVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYMAPNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetMapName((LPCTSTR)(_bstr_t)arrOldVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYCOLOR)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetColor((long)arrOldVars[i]);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeCOLOR;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (long)arrOldVars[i];
			m_pEditor->UpdateView(0, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)arrOldVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYVISIBLE)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableVisible((bool)arrOldVars[i]);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeVISIBLE;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (bool)arrOldVars[i];
			m_pEditor->UpdateView(NULL, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)arrOldVars[i]);		
		}
	}
	else
		return;
	for (i=0;i<arrLayers.GetSize();i++)
	{
		m_pEditor->UpdateFtrLayer(arrLayers[i]);
	}	
}


void CUndoModifyLayer::Redo()
{
	int i;
	if (_tcscmp(field,FIELDNAME_LAYERNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetName((LPCTSTR)(_bstr_t)(newVar));		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYMAPNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetMapName((LPCTSTR)(_bstr_t)newVar);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYCOLOR)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetColor((long)newVar);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeCOLOR;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (long)newVar;
			m_pEditor->UpdateView(NULL, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)newVar);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYVISIBLE)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableVisible((bool)newVar);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeVISIBLE;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (bool)newVar;
			m_pEditor->UpdateView(0, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)newVar);		
		}
	}
	else if(_tcscmp(field,FIELDNAME_LAYERDISPLAYORDER)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetDisplayOrder((long)newVar);		
		}
		m_pEditor->UpdateView(0,hc_UpdateLayerDisplayOrder,0);
	}
	else
		return;	

	for (i=0;i<arrLayers.GetSize();i++)
	{
		m_pEditor->UpdateFtrLayer(arrLayers[i]);
	}	
}


void CUndoModifyLayer::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoModifyLayer *po = (CUndoModifyLayer*)pObj;
	arrLayers.Copy(po->arrLayers);
	field.Format(_T("%s"),(LPCTSTR)(po->field));
	arrOldVars.Copy(po->arrOldVars);
	newVar = po->newVar;
}


void CUndoModifyLayer::Commit()
{
	if( m_pEditor!=NULL && arrLayers.GetSize()>0 && !field.IsEmpty() )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoModifyLayer::DestoryNewObjs()
{
}


CUndoModifyLayer0::CUndoModifyLayer0():CUndoAction(NULL,NULL)
{
}


CUndoModifyLayer0::CUndoModifyLayer0(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
	
}

CUndoModifyLayer0::~CUndoModifyLayer0()
{
}

void CUndoModifyLayer0::Undo()
{	
	int i;
	if (_tcscmp(field,FIELDNAME_LAYERNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetName((LPCTSTR)(_bstr_t)arrOldVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYMAPNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetMapName((LPCTSTR)(_bstr_t)arrOldVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYCOLOR)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetColor((long)arrOldVars[i]);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeCOLOR;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (long)arrOldVars[i];
			m_pEditor->UpdateView(NULL, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)arrOldVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYVISIBLE)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableVisible((bool)arrOldVars[i]);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeVISIBLE;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (bool)arrOldVars[i];
			m_pEditor->UpdateView(0, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)arrOldVars[i]);		
		}
	}
	else
		return;
	for (i=0;i<arrLayers.GetSize();i++)
	{
		m_pEditor->UpdateFtrLayer(arrLayers[i]);
	}	
}


void CUndoModifyLayer0::Redo()
{
	int i;
	if (_tcscmp(field,FIELDNAME_LAYERNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetName((LPCTSTR)(_bstr_t)(arrNewVars[i]));		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYMAPNAME)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetMapName((LPCTSTR)(_bstr_t)arrNewVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYCOLOR)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetColor((long)arrNewVars[i]);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeCOLOR;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (long)arrNewVars[i];
			m_pEditor->UpdateView(NULL, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)arrNewVars[i]);		
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYVISIBLE)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableVisible((bool)arrNewVars[i]);
			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeVISIBLE;
			param.handle = (LONG_PTR)arrLayers[i];
			param.data.lColor = (bool)arrNewVars[i];
			m_pEditor->UpdateView(NULL, hc_UpdateLayerDisplay, (LONG_PTR)(CObject*)&param);
		}
	}
	else if (_tcscmp(field,FIELDNAME_LAYLOCKED)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->EnableLocked((bool)arrNewVars[i]);		
		}
	}
	else if(_tcscmp(field,FIELDNAME_LAYERDISPLAYORDER)==0)
	{
		for (i=0;i<arrLayers.GetSize();i++)
		{
			arrLayers[i]->SetDisplayOrder((long)arrNewVars[i]);		
		}
		m_pEditor->UpdateView(NULL,hc_UpdateLayerDisplayOrder,0);
	}
	else
		return;	

	for (i=0;i<arrLayers.GetSize();i++)
	{
		m_pEditor->UpdateFtrLayer(arrLayers[i]);
	}	
}


void CUndoModifyLayer0::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoModifyLayer0 *po = (CUndoModifyLayer0*)pObj;
	arrLayers.Copy(po->arrLayers);
	field.Format(_T("%s"),(LPCTSTR)(po->field));
	arrOldVars.Copy(po->arrOldVars);
	arrNewVars.Copy(po->arrNewVars);
}


void CUndoModifyLayer0::Commit()
{
	if( m_pEditor!=NULL && arrLayers.GetSize()>0 && !field.IsEmpty() )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoModifyLayer0::DestoryNewObjs()
{
}



CUndoModifyProperties::CUndoModifyProperties():CUndoAction(NULL,NULL)
{
}


CUndoModifyProperties::CUndoModifyProperties(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
}

CUndoModifyProperties::~CUndoModifyProperties()
{
}

void CUndoModifyProperties::Undo()
{
	CUndoModifyProperties *po = this;
		
	for (int i =0;i<po->arrHandles.GetSize();i++)
	{
		m_pEditor->DeleteObject(po->arrHandles[i],FALSE);

		if( po->oldVT.GetItemCount()>0 )
			HandleToFtr(po->arrHandles[i])->ReadFrom(po->oldVT,i);			
		
		m_pEditor->RestoreObject(po->arrHandles[i]);

		if( po->XoldVT.GetItemCount()>0 )
			m_pEditor->GetDataSource()->GetXAttributesSource()->SetXAttributes(HandleToFtr(po->arrHandles[i]),po->XoldVT,i);
		//m_pEditor->UpdateObject(po->arrHandles[i]);
	}
}


void CUndoModifyProperties::Redo()
{
	CUndoModifyProperties *po = this;
	
	for (int i =0;i<po->arrHandles.GetSize();i++)
	{
		m_pEditor->DeleteObject(po->arrHandles[i],FALSE);

		if( po->newVT.GetItemCount()>0 )
			HandleToFtr(po->arrHandles[i])->ReadFrom(po->newVT,i);

		m_pEditor->RestoreObject(po->arrHandles[i]);

		if( po->XnewVT.GetItemCount()>0 )
			m_pEditor->GetDataSource()->GetXAttributesSource()->SetXAttributes(HandleToFtr(po->arrHandles[i]),po->XnewVT,i);
		//m_pEditor->UpdateObject(po->arrHandles[i]);
	}		
}


void CUndoModifyProperties::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoModifyProperties *po = (CUndoModifyProperties*)pObj;
	
	oldVT.CopyFrom(po->oldVT);
	newVT.CopyFrom(po->newVT);
	XoldVT.CopyFrom(po->XoldVT);
	XnewVT.CopyFrom(po->XnewVT);
	arrHandles.Copy(po->arrHandles);
}


void CUndoModifyProperties::Commit()
{
	if( m_pEditor!=NULL && 
		arrHandles.GetSize()>0 )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoModifyProperties::DestoryNewObjs()
{
}


void CUndoModifyProperties::SetModifyProperties(FTR_HANDLE handle, CValueTable &oldtab,CValueTable &newtab,BOOL isBaseAttr )
{
	arrHandles.Add(handle);
	if (isBaseAttr)
	{
		oldVT.AddItemFromTab(oldtab);
		newVT.AddItemFromTab(newtab);
	}
	else
	{
		XoldVT.AddItemFromTab(oldtab);
		XnewVT.AddItemFromTab(newtab);
	}
	return;
}

void CUndoModifyProperties::SetModifyProp(CFeature *pFtr, LPCTSTR fieldName, CVariantEx *pOldValue, CVariantEx *pNewValue)
{
	FTR_HANDLE h = FtrToHandle(pFtr);

	CValueTable tabOld, tabNew;

	tabOld.BeginAddValueItem();
	tabOld.AddValue(fieldName,pOldValue);
	tabOld.EndAddValueItem();
	
	tabNew.BeginAddValueItem();
	tabNew.AddValue(fieldName,pNewValue);
	tabNew.EndAddValueItem();

	SetModifyProperties(h,tabOld,tabNew,TRUE);
}


void CUndoModifyProperties::JustKeepBasicFields(CString& fieldList)
{
	CStringArray fields;
	int pos = 0;
	fieldList.TrimLeft();
	while( (pos=fieldList.Find(' '))>=0 )
	{
		fields.Add(fieldList.Left(pos));
		fieldList = fieldList.Mid(pos+1);
		fieldList.TrimLeft();
	}

	if(fieldList.GetLength()>0)
		fields.Add(fieldList);

	for(int j=0; j<oldVT.GetFieldCount(); j++)
	{
		int type;
		CString name, field;
		oldVT.GetField(j,field,type,name);
		for(int i=0; i<fields.GetSize(); i++)
		{
			if(field.CompareNoCase(fields[i])==0)
			{
				break;
			}
		}

		if(i>=fields.GetSize())
		{
			oldVT.DelField(field);
			newVT.DelField(field);			
		}
	}

}



CUndoModifyLayerOfObj::CUndoModifyLayerOfObj():CUndoAction(NULL,NULL)
{
}

CUndoModifyLayerOfObj::CUndoModifyLayerOfObj(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
}

CUndoModifyLayerOfObj::~CUndoModifyLayerOfObj()
{
}

void CUndoModifyLayerOfObj::Undo()
{
	CUndoModifyLayerOfObj *po = this;
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	for (int i =0;i<po->arrHandles.GetSize();i++)
	{
		m_pEditor->DeleteObject(po->arrHandles[i],FALSE);

		if( po->oldVT.GetItemCount()>0 )
			HandleToFtr(po->arrHandles[i])->ReadFrom(po->oldVT,i);

		//用注释掉的接口也可以达到目的
		pDS->RestoreFtrLayerOfObject(HandleToFtr(po->arrHandles[i]),po->oldLayerArr[i]);
		//	pDS->SetFtrLayerOfObject(HandleToFtr(po->arrHandles[i]),po->oldLayerArr[i]);

		m_pEditor->RestoreObject(po->arrHandles[i]);
		//m_pEditor->UpdateObject(po->arrHandles[i]);
	}
}


void CUndoModifyLayerOfObj::Redo()
{	
	CUndoModifyLayerOfObj *po = this;
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	for (int i =0;i<po->arrHandles.GetSize();i++)
	{
		m_pEditor->DeleteObject(po->arrHandles[i],FALSE);

		if( po->oldVT.GetItemCount()>0 )
			HandleToFtr(po->arrHandles[i])->ReadFrom(po->newVT,i);

		//两个接口应该都可以达到目的
		pDS->RestoreFtrLayerOfObject(HandleToFtr(po->arrHandles[i]),po->newLayerArr[i]);

		m_pEditor->RestoreObject(po->arrHandles[i]);
		//m_pEditor->UpdateObject(po->arrHandles[i]);
	}
}


void CUndoModifyLayerOfObj::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);

	CUndoModifyLayerOfObj *po = (CUndoModifyLayerOfObj*)pObj;
	
	oldVT.CopyFrom(po->oldVT);
	newVT.CopyFrom(po->newVT);
	arrHandles.Copy(po->arrHandles);
	oldLayerArr.Copy(po->oldLayerArr);
	newLayerArr.Copy(po->newLayerArr);
}

void CUndoModifyLayerOfObj::ModifyLayerOfObj(FTR_HANDLE handle, int oldlayer, int newlayer)
{
	arrHandles.Add(handle);
	oldLayerArr.Add(oldlayer);
	newLayerArr.Add(newlayer);
}

void CUndoModifyLayerOfObj::Commit()
{
	if( m_pEditor!=NULL && arrHandles.GetSize()>0 &&
		oldLayerArr.GetSize()>0 && newLayerArr.GetSize()>0 )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoModifyLayerOfObj::DestoryNewObjs()
{
}




MyNameSpaceEnd