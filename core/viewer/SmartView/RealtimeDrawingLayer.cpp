// RealtimeDrawingLayer.cpp: implementation of the CRealtimeDrawingLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RealtimeDrawingLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

IMPLEMENT_DYNAMIC(CRealtimeDrawingLayer,CDrawingLayer)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRealtimeDrawingLayer::CRealtimeDrawingLayer()
{
	m_bEnableBGLoad = FALSE;
	m_nSaveDataIdx = -1;
}

CRealtimeDrawingLayer::~CRealtimeDrawingLayer()
{
	Destroy();
}


void CRealtimeDrawingLayer::Destroy()
{
	m_nSaveDataIdx = -1;

	if( m_pContext )
		m_pContext->ReleaseSaveData(m_nSaveDataIdx);

	CDrawingLayer::Destroy();
}


void CRealtimeDrawingLayer::SetContext(CDrawingContext *pContext)
{
	if( m_pContext )m_pContext->ReleaseSaveData(m_nSaveDataIdx);

	CDrawingLayer::SetContext(pContext);
	if( pContext )m_nSaveDataIdx = pContext->RequestSaveData((LONG_PTR)this);
}

BOOL CRealtimeDrawingLayer::InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK)
{
	return TRUE;
}



//����ͼ������pContext��������ϵ�仯��ʱ��Ӧ���ô˽ӿ�
void CRealtimeDrawingLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	return;
}



//���ͼ������ݱ߽磨���ڴ������ϵ��
Envelope CRealtimeDrawingLayer::GetDataBound()
{
	return Envelope();
}



//�Ƿ���Բ���
BOOL CRealtimeDrawingLayer::CanErase()
{
	return TRUE;
}


void CRealtimeDrawingLayer::Erase()
{
	
}



//����
void CRealtimeDrawingLayer::Draw()
{
	
}



//���cache��
BOOL CRealtimeDrawingLayer::FillBlock(int blkIdx, CacheID id)
{
	return TRUE;
}


MyNameSpaceEnd
