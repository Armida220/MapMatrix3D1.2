// RealtimeDrawingLayer.h: interface for the CRealtimeDrawingLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REALTIMEDRAWINGLAYER_H__A4271513_1CC3_4269_B2DE_F91E57BF8F31__INCLUDED_)
#define AFX_REALTIMEDRAWINGLAYER_H__A4271513_1CC3_4269_B2DE_F91E57BF8F31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingLayer.h"


MyNameSpaceBegin

//ʵʱ���Ƶ�ͼ�㣬��ͼ���һ���ص��ǣ�������С���仯�Ըߣ�������Ҫʵʱ���ƣ�
//����������ת����ʽ�򵥣����ɿɲ����ķ�ʽ�����ʣ�
//������ʱ��ͼ�㣬���ͼ�㣨���ͼ��仯����ʵ��С������׽��ͼ�㣻
class EXPORT_SMARTVIEW CRealtimeDrawingLayer : public CDrawingLayer  
{
	DECLARE_DYNAMIC(CRealtimeDrawingLayer)
public:
	CRealtimeDrawingLayer();
	virtual ~CRealtimeDrawingLayer();

	virtual void Destroy();

	virtual void SetContext(CDrawingContext *pContext);
	
	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);
	
	//����ͼ������pContext��������ϵ�仯��ʱ��Ӧ���ô˽ӿ�
	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);
		
	//���ͼ������ݱ߽磨���ڴ������ϵ��
	virtual Envelope GetDataBound();
	
	//�Ƿ���Բ���
	virtual BOOL CanErase();
	virtual void Erase();
	
	//����
	virtual void Draw();
	
	//���cache��
	virtual BOOL FillBlock(int blkIdx, CacheID id);

	// ��ÿ��ŵĺ����ȡֵ��Χ
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax){};
	
	// ��ý��㴦���ڵĿ���
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum){};
	
	// ��ÿ���ĸ��ǵ������ڿͻ�����ϵ�µ�XY����
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]){};
	
protected:
	int m_nSaveDataIdx;
};

MyNameSpaceEnd

#endif // !defined(AFX_REALTIMEDRAWINGLAYER_H__A4271513_1CC3_4269_B2DE_F91E57BF8F31__INCLUDED_)
