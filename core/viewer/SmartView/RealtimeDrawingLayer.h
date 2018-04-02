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

//实时绘制的图层，该图层的一般特点是，数据量小，变化性高，常常需要实时绘制；
//所以其坐标转换方式简单；做成可擦除的方式更合适；
//例如临时线图层，光标图层（光标图层变化性其实很小），捕捉框图层；
class EXPORT_SMARTVIEW CRealtimeDrawingLayer : public CDrawingLayer  
{
	DECLARE_DYNAMIC(CRealtimeDrawingLayer)
public:
	CRealtimeDrawingLayer();
	virtual ~CRealtimeDrawingLayer();

	virtual void Destroy();

	virtual void SetContext(CDrawingContext *pContext);
	
	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);
	
	//当绘图环境（pContext）的坐标系变化的时候应调用此接口
	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);
		
	//获得图层的数据边界（基于大地坐标系）
	virtual Envelope GetDataBound();
	
	//是否可以擦除
	virtual BOOL CanErase();
	virtual void Erase();
	
	//绘制
	virtual void Draw();
	
	//填充cache块
	virtual BOOL FillBlock(int blkIdx, CacheID id);

	// 获得块编号的合理的取值范围
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax){};
	
	// 获得焦点处所在的块编号
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum){};
	
	// 获得块的四个角点坐标在客户坐标系下的XY坐标
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]){};
	
protected:
	int m_nSaveDataIdx;
};

MyNameSpaceEnd

#endif // !defined(AFX_REALTIMEDRAWINGLAYER_H__A4271513_1CC3_4269_B2DE_F91E57BF8F31__INCLUDED_)
