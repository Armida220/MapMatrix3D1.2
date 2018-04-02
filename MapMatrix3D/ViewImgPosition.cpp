#include "stdafx.h"
#include "ViewImgPosition.h"
#include "EditBaseDoc.h"



CUndoAdjustImagePosition::CUndoAdjustImagePosition(CEditor *p, LPCTSTR name):
CUndoAction(p,name)
{
}

CUndoAdjustImagePosition::~CUndoAdjustImagePosition()
{
}

void CUndoAdjustImagePosition::Undo()
{
	CDlgDoc *pDoc = (CDlgDoc*)m_pEditor;
	pDoc->UpdateAllViews(NULL,hc_RestoreImagePosition,(CObject*)&oldImgPos);
}


void CUndoAdjustImagePosition::Redo()
{
	CDlgDoc *pDoc = (CDlgDoc*)m_pEditor;
	pDoc->UpdateAllViews(NULL,hc_SetImagePosition,(CObject*)&newImgPos);
}


void CUndoAdjustImagePosition::CopyFrom(const CUndoAction *pObj)
{
	CUndoAction::CopyFrom(pObj);
	
	CUndoAdjustImagePosition *po = (CUndoAdjustImagePosition*)pObj;
	
	oldImgPos = po->oldImgPos;
	newImgPos = po->newImgPos;
}


void CUndoAdjustImagePosition::Commit()
{
	if( m_pEditor!=NULL )
	{
		m_pEditor->SetUndo(this);
	}
}


void CUndoAdjustImagePosition::DestoryNewObjs()
{
}

