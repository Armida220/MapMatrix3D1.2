// TmpGraphLayer.cpp: implementation of the CTmpGraphLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TmpGraphLayer.h"
#include "matrix.h"
#include <math.h>
#include "SmartViewFunctions.h"
#include "VectorLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTmpGraphLayer::CTmpGraphLayer(int nDataType)
{
	m_nType = nDataType;
	m_pCacheData = NULL;

	m_nEraseType = eraseNone;
	m_bValidXOR = TRUE;

	m_bUseCoordSys = TRUE;

	m_clrMark = 0xffffff;
	m_bEnableMark = FALSE;
	m_fMarkWid = 3.0f;
}

CTmpGraphLayer::~CTmpGraphLayer()
{
	DelAllObjs();
}

void CTmpGraphLayer::SetDataType(int nDataType)
{
	if( m_nType!=nDataType )
	{
		DelAllObjs();
	}
	m_nType = nDataType;
}

void *CTmpGraphLayer::CreateObject()
{
	if( m_nType==typeGrBuffer2d )
		return new GrBuffer2d;
	else if( m_nType==typeGrBuffer )
		return new GrBuffer;

	return NULL;
}


void CTmpGraphLayer::ReleaseObject(void *pBuf)
{
	if( !pBuf )return;

	if( m_nType==typeGrBuffer2d )
		delete (GrBuffer2d*)pBuf;
	else if( m_nType==typeGrBuffer )
		delete (GrBuffer*)pBuf;
}

void *CTmpGraphLayer::OpenObj(LONG_PTR objnum)
{
	int i;
	// it is found!
	if( FindObj(objnum,i) )
	{
		GrElement *ele = m_arrPEles.GetAt(i);
		
		return ele->pBuf;
	}
	
	GrElement *ele = new GrElement;
	if( !ele )return NULL;
	
	ele->pBuf = CreateObject();
	ele->no = objnum;
	m_arrPEles.InsertAt(i,ele);

	return ele->pBuf;
}


void CTmpGraphLayer::DelObj(LONG_PTR objnum)
{
	int i;

	if( !FindObj(objnum,i) )return;
	GrElement *pEl = m_arrPEles.GetAt(i);
	ReleaseObject(pEl->pBuf);
	delete pEl;

	m_arrPEles.RemoveAt(i);
}

void CTmpGraphLayer::DelAllObjs()
{
	int size = m_arrPEles.GetSize();
	for( int i=0; i<size; i++)
	{
		GrElement *pEl = m_arrPEles.GetAt(i);
		ReleaseObject(pEl->pBuf);
		delete pEl;
	}

	m_arrPEles.RemoveAll();
}


BOOL CTmpGraphLayer::FindObj(LONG_PTR objnum, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrPEles.GetSize()<=0 ){ findidx = 0; bnew = 1; }
	else
	{
		int i0 = 0, i1 = m_arrPEles.GetSize()-1, i2;
		while(findidx==-1) 
		{
			if( objnum<=m_arrPEles.GetAt(i0)->no )
			{
				bnew = (objnum==m_arrPEles.GetAt(i0)->no?0:1);
				findidx = i0;
				break;
			}
			else if( objnum>=m_arrPEles.GetAt(i1)->no )
			{
				bnew = (objnum==m_arrPEles.GetAt(i1)->no?0:1);
				findidx = (objnum==m_arrPEles.GetAt(i1)->no?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (objnum==m_arrPEles.GetAt(i0)->no?0:1);
				findidx = (objnum==m_arrPEles.GetAt(i0)->no?i0:(i0+1));
				break;
			}
			
			if( objnum<m_arrPEles.GetAt(i2)->no )
				i1 = i2;
			else if( objnum>m_arrPEles.GetAt(i2)->no )
				i0 = i2;
			else
			{
				findidx = i2; bnew = 0;
				break;
			}
		}
	}
	
	insert_idx = findidx;
	return (bnew==0);
}

void CTmpGraphLayer::SetMarkParams(BOOL bEnable, COLORREF color, float wid)
{
	m_bEnableMark = bEnable;
	m_clrMark = color;
	m_fMarkWid = wid;
}


BOOL CTmpGraphLayer::IsEnableMark()
{
	return m_bEnableMark;
}


void CTmpGraphLayer::SetObjMark(LONG_PTR objnum, BOOL bMark)
{
	int i;
	
	if( !FindObj(objnum,i) )return;
	GrElement *pEl = m_arrPEles.GetAt(i);
	
	pEl->bmark1 = (bMark?1:0);
}


BOOL CTmpGraphLayer::IsObjMark(LONG_PTR objnum)
{
	int i;
	
	if( !FindObj(objnum,i) )return FALSE;
	GrElement *pEl = m_arrPEles.GetAt(i);
	
	return (pEl->bmark1==1);
}


void CTmpGraphLayer::DrawBuffers()
{
	int i, nCount = m_arrPEles.GetSize();

	CCoordSys *pOld = m_pContext->GetCoordSys();
	if( !m_bUseCoordSys )
		m_pContext->SetCoordSys(NULL);

	int drawmode = CDrawingContext::modeNormal;
	if( m_nEraseType==eraseXOR )
		drawmode = CDrawingContext::modeDynamic;

	GrElementList list;
	
	for( i=0; i<nCount; i++)
	{
		GrElement *pe = m_arrPEles.GetAt(i);
		if( pe==NULL )continue;
		if( !pe->bvisible )continue;

		list.add(pe);
	}
	
	if( m_nType==typeGrBuffer2d )
	{
		m_pContext->DrawGrBuffer2d(&list,drawmode);
		if( m_bEnableMark )
		{
			m_pContext->DrawSelectionMark2d(&list,drawmode,m_clrMark,m_fMarkWid);
		}
	}
	else
	{
		m_pContext->DrawGrBuffer(&list,drawmode);
		if( m_bEnableMark )
		{
			m_pContext->DrawSelectionMark(&list,drawmode,m_clrMark,m_fMarkWid);
		}
	}

	if( !m_bUseCoordSys )
		m_pContext->SetCoordSys(pOld);
}

BOOL CTmpGraphLayer::CanErase()
{
	if( m_nEraseType==eraseBMP )
	{
		return TRUE;
	}
	else if( m_nEraseType==eraseXOR )
	{
		return TRUE;
	}

	return FALSE;
}


void CTmpGraphLayer::Erase()
{
	if( m_nEraseType==eraseBMP )
	{
		m_pContext->RestoreBmp(m_nSaveDataIdx);
	}
	else if( m_nEraseType==eraseXOR )
	{
		if( m_bValidXOR )DrawBuffers();
	}
}


void CTmpGraphLayer::Save()
{
	if( m_nEraseType==eraseBMP )
	{
		Envelope e;
		int i, nCount = m_arrPEles.GetSize();
		
		for( i=0; i<nCount; i++)
		{
			GrElement *pe = m_arrPEles.GetAt(i);
			if( pe==NULL )continue;
			if( !pe->bvisible )continue;
			
			if( m_nType==typeGrBuffer2d )
			{
				GrBuffer2d *pBuf = (GrBuffer2d*)pe->pBuf;
				e.Union(&pBuf->GetEnvelope());
			}
			else
			{
				GrBuffer *pBuf = (GrBuffer*)pe->pBuf;
				e.Union(&pBuf->GetEnvelope());
			}
		}
		
		if( m_bUseCoordSys )
			e.TransformGrdToClt(m_pContext->GetCoordSys(),1);
		
		if( m_bEnableMark )
		{
			e.m_xl -= m_fMarkWid;
			e.m_xh += m_fMarkWid;
			e.m_yl -= m_fMarkWid;
			e.m_yh += m_fMarkWid;
		}

		CRect rc(floor(e.m_xl)-1,floor(e.m_yl)-1,ceil(e.m_xh)+1,ceil(e.m_yh)+1);
		m_pContext->SaveBmp(m_nSaveDataIdx,rc);
	}
	else if( m_nEraseType==eraseXOR )
	{
		m_bValidXOR = TRUE;
	}
}

void CTmpGraphLayer::Draw()
{
	Save();
	DrawBuffers();
}


void CTmpGraphLayer::Reset()
{
	if( m_nEraseType==eraseBMP )
	{
		m_pContext->ResetBmp(m_nSaveDataIdx);
	}
	else if( m_nEraseType==eraseXOR )
	{
		m_bValidXOR = FALSE;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVariantDragLayer::CVariantDragLayer()
{
	m_pDrawBuf = NULL;
	m_pEraseBuf = NULL;
	m_pDrawBuf2d = NULL;
	m_pEraseBuf2d = NULL;

	m_bUseCoordSys = TRUE;
	m_bBufferChanged = FALSE;

	m_nEraseType = eraseXOR;

	m_bUseGrBuffer = FALSE;
}


CVariantDragLayer::~CVariantDragLayer()
{
	if( m_pDrawBuf!=m_pEraseBuf && m_pEraseBuf!=NULL )
		delete m_pEraseBuf;

	if( m_pDrawBuf )
		delete m_pDrawBuf;

	if( m_pDrawBuf2d!=m_pEraseBuf2d && m_pEraseBuf2d!=NULL )
		delete m_pEraseBuf2d;
	
	if( m_pDrawBuf2d )
		delete m_pDrawBuf2d;
}



void CVariantDragLayer::SetBuffer(GrBuffer2d *pBuf)
{
	if( m_pDrawBuf2d!=m_pEraseBuf2d && m_pDrawBuf2d!=NULL )
		delete m_pDrawBuf2d;

	m_pDrawBuf2d = NULL;

	if( pBuf )
	{
		m_pDrawBuf2d = new GrBuffer2d;

		if( m_pDrawBuf2d )
			m_pDrawBuf2d->CopyFrom(pBuf);
	}

	m_bBufferChanged = TRUE;
}




void CVariantDragLayer::SetBuffer(GrBuffer *pBuf)
{
	if( m_pDrawBuf!=m_pEraseBuf && m_pDrawBuf!=NULL )
		delete m_pDrawBuf;
	
	m_pDrawBuf = NULL;
	
	if( pBuf )
	{
		m_pDrawBuf = new GrBuffer;
		
		if( m_pDrawBuf )
			m_pDrawBuf->CopyFrom(pBuf);
	}
	
	m_bBufferChanged = TRUE;
}

void CVariantDragLayer::AddBuffer(GrBuffer2d *pBuf)
{
	if( pBuf )
	{
		if(!m_pDrawBuf2d)
		{
			m_pDrawBuf2d = new GrBuffer2d;			
		}
		
		if( m_pDrawBuf2d )
			m_pDrawBuf2d->AddBuffer(pBuf);
	}
	
	m_bBufferChanged = TRUE;
}

void CVariantDragLayer::AddBuffer(GrBuffer *pBuf)
{
	if( pBuf )
	{
		if(!m_pDrawBuf)
		{
			m_pDrawBuf = new GrBuffer;			
		}
		
		if( m_pDrawBuf )
			m_pDrawBuf->AddBuffer(pBuf);
	}
	
	m_bBufferChanged = TRUE;
}

BOOL CVariantDragLayer::CanErase()
{
	return TRUE;
}


void CVariantDragLayer::Erase()
{
	if( m_bUseGrBuffer )
	{
		if( m_nEraseType==eraseXOR )
		{
			CCoordSys *pOld = m_pContext->GetCoordSys();
			if( !m_bUseCoordSys )
				m_pContext->SetCoordSys(NULL);
			
			if( m_pEraseBuf )
				m_pContext->DrawGrBuffer(&GrElementList(m_pEraseBuf),CDrawingContext::modeDynamic);
			
			if( !m_bUseCoordSys )
				m_pContext->SetCoordSys(pOld);
		}
		else if( m_nEraseType==eraseBMP )
		{
			m_pContext->RestoreBmp(m_nSaveDataIdx);
		}

		if( m_pEraseBuf && m_pEraseBuf!=m_pDrawBuf )
		{
			delete m_pEraseBuf;
		}
		m_pEraseBuf = NULL;
	}
	else
	{
		if( m_nEraseType==eraseXOR )
		{
			CCoordSys *pOld = m_pContext->GetCoordSys();
			if( !m_bUseCoordSys )
				m_pContext->SetCoordSys(NULL);
			
			if( m_pEraseBuf2d )
				m_pContext->DrawGrBuffer2d(&GrElementList(m_pEraseBuf2d),CDrawingContext::modeDynamic);
			
			if( !m_bUseCoordSys )
				m_pContext->SetCoordSys(pOld);
		}
		else if( m_nEraseType==eraseBMP )
		{
			m_pContext->RestoreBmp(m_nSaveDataIdx);
		}
		
		if( m_pEraseBuf2d && m_pEraseBuf2d!=m_pDrawBuf2d )
		{
			delete m_pEraseBuf2d;
		}
		m_pEraseBuf2d = NULL;
	}
}



//»æÖÆ
void CVariantDragLayer::Draw()
{
	if( m_bUseGrBuffer )
	{
		if( m_pEraseBuf && m_pEraseBuf!=m_pDrawBuf )
			delete m_pEraseBuf;
		m_pEraseBuf = NULL;

		if( m_nEraseType==eraseBMP )
		{
			if( m_pDrawBuf==NULL )
			{
				Reset();
			}
			else
			{
				Envelope e = m_pDrawBuf->GetEnvelope();

				if( m_bUseCoordSys )
					e.TransformGrdToClt(m_pContext->GetCoordSys(),1);

				CRect rc(floor(e.m_xl)-1,floor(e.m_yl)-1,ceil(e.m_xh)+1,ceil(e.m_yh)+1);
				m_pContext->SaveBmp(m_nSaveDataIdx,rc);
			}
		}

		CCoordSys *pOld = m_pContext->GetCoordSys();
		if( !m_bUseCoordSys )
			m_pContext->SetCoordSys(NULL);

		if( m_pDrawBuf )
			m_pContext->DrawGrBuffer(&GrElementList(m_pDrawBuf),
			m_nEraseType==eraseXOR?CDrawingContext::modeDynamic:CDrawingContext::modeNormal);
		
		if( !m_bUseCoordSys )
			m_pContext->SetCoordSys(pOld);

		m_pEraseBuf = m_pDrawBuf;
		m_bBufferChanged = FALSE;
	}
	else
	{
		if( m_pEraseBuf2d && m_pEraseBuf2d!=m_pDrawBuf2d )
			delete m_pEraseBuf2d;
		m_pEraseBuf2d = NULL;
		
		if( m_nEraseType==eraseBMP )
		{
			if( m_pDrawBuf2d==NULL )
			{
				Reset();
			}
			else
			{
				Envelope e = m_pDrawBuf2d->GetEnvelope();
				
				if( m_bUseCoordSys )
					e.TransformGrdToClt(m_pContext->GetCoordSys(),1);
				
				CRect rc(floor(e.m_xl)-1,floor(e.m_yl)-1,ceil(e.m_xh)+1,ceil(e.m_yh)+1);
				m_pContext->SaveBmp(m_nSaveDataIdx,rc);
			}
		}
		
		CCoordSys *pOld = m_pContext->GetCoordSys();
		if( !m_bUseCoordSys )
			m_pContext->SetCoordSys(NULL);
		
		if( m_pDrawBuf2d )
			m_pContext->DrawGrBuffer2d(&GrElementList(m_pDrawBuf2d),
			m_nEraseType==eraseXOR?CDrawingContext::modeDynamic:CDrawingContext::modeNormal);
		
		if( !m_bUseCoordSys )
			m_pContext->SetCoordSys(pOld);
		
		m_pEraseBuf2d = m_pDrawBuf2d;
		m_bBufferChanged = FALSE;		
	}
}


void CVariantDragLayer::Reset()
{
	if( m_bUseGrBuffer )
	{
		if( m_pEraseBuf && m_pEraseBuf!=m_pDrawBuf )
			delete m_pEraseBuf;
		m_pEraseBuf = NULL;
		
		m_bBufferChanged = FALSE;
		
		if( m_nEraseType==eraseBMP )
		{
			m_pContext->ResetBmp(m_nSaveDataIdx);
		}
	}
	else
	{
		if( m_pEraseBuf2d && m_pEraseBuf2d!=m_pDrawBuf2d )
			delete m_pEraseBuf2d;
		m_pEraseBuf2d = NULL;
		
		m_bBufferChanged = FALSE;
		
		if( m_nEraseType==eraseBMP )
		{
			m_pContext->ResetBmp(m_nSaveDataIdx);
		}
	}
}



MyNameSpaceEnd
