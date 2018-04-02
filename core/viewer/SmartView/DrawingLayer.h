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

//��ͼ��
//�������ݵĹ��������ϵ�Ļ��㣬Cache�����Э����
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

	//���û�ͼ����
	virtual void SetContext(CDrawingContext *pContext);
	CDrawingContext *GetContext() const{
		return m_pContext;
	}

	virtual void Destroy();

	virtual BOOL DestroyCache();

	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);

	//����ͼ������pContext��������ϵ�仯��ʱ��Ӧ���ô˽ӿ�
	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);

	//���ÿ�������
	virtual void SetViewBound(Envelope e);

	//���ͼ������ݱ߽磨���ڴ������ϵ��
	virtual Envelope GetDataBound();

	//��������
	void ModifyAttribute(DWORD add, DWORD cut);
	DWORD GetAttribute();
	void SetAttribute(DWORD attr);

	//����ʹ����Ҫ�ػ�
	void Invalidate();
	BOOL IsInvalidated();

	//�Ƿ���Բ���
	virtual BOOL CanErase();
	virtual void Erase();

	//����
	virtual void Draw();

	//���cache��
	virtual BOOL FillBlock(int blkIdx, CacheID id);

	virtual BOOL CheckCacheReady();

	//����׼�� Cache �飨D3D�޷��ڻ�ͼ�����м���������ֻ����׼���������ٻ��ƣ����������������������������
	virtual BOOL ExtraPrepareCaches();

public:
	
	//ǿ�����cache��
	virtual int ForceFillBlock(CacheID id);
	
	//xl,xh,yl,yh�ǿͻ�����ϵ��Χ
	virtual void DrawClient(double xl, double xh, double yl, double yh);

protected:

	CDrawingContext *m_pContext;
	DWORD m_dwAttribute;

public:
	BOOL m_bForceDraw;
};


MyNameSpaceEnd

#endif // !defined(AFX_DRAWINGLAYER_H__747B66FF_959A_4687_AC63_BF2D03B6747B__INCLUDED_)
