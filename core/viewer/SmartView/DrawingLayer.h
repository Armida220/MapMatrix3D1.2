// DrawingLayer.h: interface for the CDrawingLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWINGLAYER_H__747B66FF_959A_4687_AC63_BF2D03B6747B__INCLUDED_)
#define AFX_DRAWINGLAYER_H__747B66FF_959A_4687_AC63_BF2D03B6747B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingContext.h"
#include "Envelope.h"
#include "CoordSys.h"
#include "CacheController.h"


MyNameSpaceBegin

//绘图层
//负责数据的管理，坐标关系的换算，Cache缓存的协调，
class EXPORT_SMARTVIEW CDrawingLayer : public CCacheLoader
{
	DECLARE_DYNAMIC(CDrawingLayer)
public:
	enum
	{
		attrVisible = 0x01,
		attrVisibleAdjust = 0x02,
		attrSmoothZoom = 0x04,
		attrExtraPrepareCache = 0x08
	};
	CDrawingLayer();
	virtual ~CDrawingLayer();

	//设置绘图环境
	virtual void SetContext(CDrawingContext *pContext);
	CDrawingContext *GetContext() const{
		return m_pContext;
	}

	virtual void Destroy();

	virtual BOOL DestroyCache();

	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);

	//当绘图环境（pContext）的坐标系变化的时候应调用此接口
	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);

	//设置可视区域
	virtual void SetViewBound(Envelope e);

	//获得图层的数据边界（基于大地坐标系）
	virtual Envelope GetDataBound();

	//设置属性
	void ModifyAttribute(DWORD add, DWORD cut);
	DWORD GetAttribute();
	void SetAttribute(DWORD attr);

	//擦除使其需要重绘
	void Invalidate();
	BOOL IsInvalidated();

	//是否可以擦除
	virtual BOOL CanErase();
	virtual void Erase();

	//绘制
	virtual void Draw();

	//填充cache块
	virtual BOOL FillBlock(int blkIdx, CacheID id);

	virtual BOOL CheckCacheReady();

	//单独准备 Cache 块（D3D无法在绘图过程中加载纹理，它只能先准备好纹理再绘制；这个函数就是针对类似这种情况）
	virtual BOOL ExtraPrepareCaches();

public:
	
	//强制填充cache块
	virtual int ForceFillBlock(CacheID id);
	
	//xl,xh,yl,yh是客户坐标系范围
	virtual void DrawClient(double xl, double xh, double yl, double yh);

protected:

	CDrawingContext *m_pContext;
	DWORD m_dwAttribute;

public:
	BOOL m_bForceDraw;
};


MyNameSpaceEnd

#endif // !defined(AFX_DRAWINGLAYER_H__747B66FF_959A_4687_AC63_BF2D03B6747B__INCLUDED_)
