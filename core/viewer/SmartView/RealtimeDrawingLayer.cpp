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



//当绘图环境（pContext）的坐标系变化的时候应调用此接口
void CRealtimeDrawingLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	return;
}



//获得图层的数据边界（基于大地坐标系）
Envelope CRealtimeDrawingLayer::GetDataBound()
{
	return Envelope();
}



//是否可以擦除
BOOL CRealtimeDrawingLayer::CanErase()
{
	return TRUE;
}


void CRealtimeDrawingLayer::Erase()
{
	
}



//绘制
void CRealtimeDrawingLayer::Draw()
{
	
}



//填充cache块
BOOL CRealtimeDrawingLayer::FillBlock(int blkIdx, CacheID id)
{
	return TRUE;
}


MyNameSpaceEnd
