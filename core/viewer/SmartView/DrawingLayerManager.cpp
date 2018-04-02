// DrawingLayerManager.cpp: implementation of the CDrawingLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawingLayerManager.h"
#include "RasterLayer.h"
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

CDrawingLayerManager::CDrawingLayerManager()
{

}

CDrawingLayerManager::~CDrawingLayerManager()
{

}


void CDrawingLayerManager::InsertDrawingLayer(CDrawingLayer *pLayer, int idx)
{
	if( pLayer==NULL )return;
	int number = m_arrLayers.GetSize();	
	for( int i=0; i<number; i++)
	{
		if( m_arrLayers.GetAt(i)==pLayer )
		{
			if( idx>=0 && idx!=i )
			{
				m_arrLayers.InsertAt(idx,pLayer);
				if( i>idx )m_arrLayers.RemoveAt(i+1);
				else m_arrLayers.RemoveAt(i);
			}
			return;
		}
	}

	if( idx==-1 || idx>=number )
		m_arrLayers.Add(pLayer);
	else
		m_arrLayers.InsertAt(idx,pLayer);
}


void CDrawingLayerManager::InsertBeforeDrawingLayer(CDrawingLayer *pLayer, CDrawingLayer *pLayer0)
{
	InsertDrawingLayer(pLayer,FindDrawingLayer(pLayer0));
}


void CDrawingLayerManager::RemoveDrawingLayer(CDrawingLayer *pLayer)
{
	if( pLayer==NULL )return;
	int number = m_arrLayers.GetSize();
	
	for( int i=number-1; i>=0; i--)
	{
		if( m_arrLayers.GetAt(i)==pLayer )
		{			
			m_arrLayers.RemoveAt(i);			
		}
	}	
}


int CDrawingLayerManager::GetDrawingLayerCount()
{
	return m_arrLayers.GetSize();
}


int CDrawingLayerManager::FindDrawingLayer(CDrawingLayer *pLayer)
{
	if( pLayer==NULL )return -1;
	int number = m_arrLayers.GetSize();
	
	for( int i=number-1; i>=0; i--)
	{
		if( m_arrLayers.GetAt(i)==pLayer )
		{			
			return i;	
		}
	}
	return -1;
}


CDrawingLayer *CDrawingLayerManager::GetDrawingLayer(int idx)
{
	int number = m_arrLayers.GetSize();
	
	if( idx<0 || idx>=number )return NULL;	
	
	return m_arrLayers.GetAt(idx);
}

BOOL CDrawingLayerManager::IsIncludeRasterLayer()
{
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number; i++)
	{
		if( m_arrLayers.GetAt(i)->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
			return TRUE;
	}

	return FALSE;
}


BOOL CDrawingLayerManager::IsIncludeVectorLayer()
{
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number; i++)
	{
		if( m_arrLayers.GetAt(i)->IsKindOf(RUNTIME_CLASS(CVectorLayer)) )
			return TRUE;
	}
	
	return FALSE;
}

Envelope CDrawingLayerManager::GetDataBound()
{
	int number = m_arrLayers.GetSize();

	Envelope e;
	for( int i=0; i<number; i++)
	{
		e.Union(&m_arrLayers.GetAt(i)->GetDataBound(),3);
	}

	return e;
}



void CDrawingLayerManager::RemoveAll()
{
	m_arrLayers.RemoveAll();
}

void CDrawingLayerManager::BGLoad()
{
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number; i++)
	{
		m_arrLayers.GetAt(i)->BGLoad();
	}
}


void CDrawingLayerManager::PlanBGLoad()
{
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number; i++)
	{
		m_arrLayers.GetAt(i)->PlanBGLoad();
	}
}

void CDrawingLayerManager::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number; i++)
	{
		m_arrLayers.GetAt(i)->OnChangeCoordSys(bJustScrollScreen);
	}
}

void CDrawingLayerManager::SaveForceDrawFlags(CArray<BOOL,BOOL>& arrFlags)
{
	arrFlags.RemoveAll();
	
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number; i++)
	{
		arrFlags.Add(m_arrLayers.GetAt(i)->m_bForceDraw);
	}
}


void CDrawingLayerManager::SetForceDrawFlags(CArray<BOOL,BOOL>& arrFlags)
{
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number && i<arrFlags.GetSize(); i++)
	{
		m_arrLayers.GetAt(i)->m_bForceDraw = arrFlags[i];
	}
}


void CDrawingLayerManager::SetForceDrawFlags(BOOL bForceDraw)
{
	int number = m_arrLayers.GetSize();
	
	for( int i=0; i<number; i++)
	{
		m_arrLayers.GetAt(i)->m_bForceDraw = bForceDraw;
	}
}

MyNameSpaceEnd