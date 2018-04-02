// CacheController.h: interface for the CCacheController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACHECONTROLLER_H__E11CDB98_9171_4A19_8ED2_F131F51AEFB5__INCLUDED_)
#define AFX_CACHECONTROLLER_H__E11CDB98_9171_4A19_8ED2_F131F51AEFB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingContext.h"
#include "TextureObj.h"

#define CHEDATA_TYPE_NONE				0
#define CHEDATA_TYPE_BMP				1
#define CHEDATA_TYPE_GLLIST				2
#define CHEDATA_TYPE_GLTEXT				3
#define CHEDATA_TYPE_D3DTEXT			4


MyNameSpaceBegin


class EXPORT_SMARTVIEW CDataEncoder  
{
public:
	virtual ~CDataEncoder(){};
	virtual BOOL Init(int type, long param1, long param2) = 0;
	virtual BOOL Encode(void* p, long n, void** pp, long* pn) = 0;
	virtual BOOL Decode(void* p, long n, void* p2, long* pn) = 0;
	
};


class EXPORT_SMARTVIEW CProgDataEncoder : public CDataEncoder  
{
public:
	CProgDataEncoder();
	virtual ~CProgDataEncoder();
	
	virtual BOOL Init(int type, long param1, long param2);
	virtual BOOL Encode(void* p, long n, void** pp, long* pn);
	virtual BOOL Decode(void* p, long n, void* p2, long* pn);
private:
	BYTE *m_pTmpMem;
	unsigned long m_nTmpMemLen;
};


class EXPORT_SMARTVIEW CCacheData
{
public:
	struct EncodedItem
	{
		BOOL  bIsEncoded;
		BYTE* pData;
		DWORD nDataLen;
	};
	
	struct DecodedItem
	{
		enum
		{
			state_none = 0,
			state_decoded = 1,
			state_modified = 2
		};
		DecodedItem(){	blkidx = -1; state = state_none; }
		int blkidx;
		int state;
	};
	
	CCacheData();
	virtual ~CCacheData();
	
	virtual BOOL Init(CSize szBlock, int nBlock, DWORD clrBK);
	virtual void Destroy()=0;
	virtual int GetType()=0;
	virtual void ClearBlock(int blkIdx)=0;
	virtual CSize GetBlockSize();
	virtual int GetBlockCount();
	virtual BOOL AskforMoreBlocks(int num);

	virtual int GetBlockDecodedState(int idx);
	virtual void SetBlockDecodedState(int idx, int state);

	//填充cache块的准备函数
	virtual void BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock=TRUE);

	//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer(GrElementList *pList, int mode)=0; 

	//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode)=0; 

	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)=0; 
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)=0; 
	virtual void DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor){}; 

	
	//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw, BOOL bCover=TRUE)=0; 

	//填充cache块的结束函数
	virtual void EndFillBlock();
	virtual void SetFillBlockRect(CRect rect);

	virtual void BeginDisplayBlock();
	virtual void EndDisplayBlock();
	//显示cache块的接口；x,y是客户窗口坐标
	virtual void DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4])=0;

	void ClearAll();

	void EnableTransparentOverlap(BOOL bEnable);
	BOOL IsTransparentOverlap();
	void EnableSmoothZoom(BOOL bEnable);
	BOOL IsSmoothZoom();
	void SetBkColor(COLORREF clrBk){
		m_clrBack = clrBk;
	}
	inline COLORREF GetBkColor(){return m_clrBack;}

protected:
	COLORREF		m_clrBack;
	BOOL m_bTransparentOverlap;

	//缩放时，是否做线性内插
	BOOL m_bSmoothZoom;

	CSize m_szBlock;
	int m_nBlockSum;

	CDrawingContext *m_pFillContext;
	int m_nFillBlkIdx;
	CRect m_rcFillBlk;
};


class CChgBmpOfDC
{
public:
	CChgBmpOfDC(HDC hdc, HBITMAP hBmp);
	~CChgBmpOfDC();
private:
	HDC m_hDC;
	HBITMAP m_hBmpOld;
};


class EXPORT_SMARTVIEW CCacheBmpData : public CCacheData
{
public:
	CCacheBmpData();
	virtual ~CCacheBmpData();
	
	void SetColorTable(int bitCount, RGBQUAD* pClrTbl);
	virtual BOOL Init(CSize szBlock, int nBlock, DWORD clrBK);
	virtual void Destroy();
	virtual int GetType();
	virtual void ClearBlock(int idx);
	virtual BOOL AskforMoreBlocks(int num);
	
	virtual int GetBlockDecodedState(int idx);
	virtual void SetBlockDecodedState(int idx, int state);

	//填充cache块的准备函数
	virtual void BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock=TRUE);
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer(GrElementList *pList, int mode); 
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode); 

	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor);
	
	//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover=TRUE); 
	
	//填充cache块的结束函数
	virtual void EndFillBlock();
	
	// x,y是客户窗口坐标
	virtual void DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4]);

	void SetRotateAndZoom(float ang, float kx, float ky);

	void CreateProgDataEncoder();
	HBITMAP	GetBlockBitmap(int blkIdx, BYTE **ppBits);
	
protected:
	BOOL DecompressBitmap(int blkIdx);
	BOOL CompressBitmap(int blkIdx);

	BYTE *GetRotateBuf(int size);
protected:

	
	int				m_nBitCount;
	RGBQUAD			*m_pRGBQuad;
	
	EncodedItem		*m_pEncodedItems;
	BITMAPINFO		*m_pCacheBmpInfo;

	HBITMAP			*m_pHBmps;
	BYTE			**m_ppBits;
	
	CArray<int,int>	m_arrDecodedUsedCount;
	CDataEncoder	*m_pDefDataEncoder;
	CDataEncoder	*m_pCurDataEncoder;
	
	DecodedItem		*m_pDecodedItems;

	HDC				m_hMemDC;
	HDC				m_hTemDC;

	int				m_nDecodedItemCount;
	HBITMAP			m_hSaveBmp;

protected:
	float			m_fRotAng, m_fKX, m_fKY;
	BYTE *m_pRotateBuf;
	DWORD m_nBufSize;
};


class EXPORT_SMARTVIEW CCacheGLListData : public CCacheData
{
public:
	CCacheGLListData();
	virtual ~CCacheGLListData();
	
	virtual BOOL Init(CSize szBlock, int nBlock, DWORD clrBK);
	virtual void Destroy();
	virtual int GetType();
	virtual void ClearBlock(int idx);
	virtual BOOL AskforMoreBlocks(int num);

	//填充cache块的准备函数
	virtual void BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock=TRUE);
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer(GrElementList *pList, int mode); 
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode); 
	
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	
	//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover=TRUE); 
	
	//填充cache块的结束函数
	virtual void EndFillBlock();
	
	// x,y是客户窗口坐标
	virtual void DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4]);
	
private:
	CArray<int,int> m_arrListIDs;
};



class EXPORT_SMARTVIEW CCacheGLTextData : public CCacheData
{
public:
	CCacheGLTextData();
	virtual ~CCacheGLTextData();
	
	virtual BOOL Init(CSize szBlock, int nBlock, DWORD clrBK);
	virtual void Destroy();
	virtual int GetType();
	virtual void ClearBlock(int idx);
	virtual BOOL AskforMoreBlocks(int num);
	
	//填充cache块的准备函数
	virtual void BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock=TRUE);
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer(GrElementList *pList, int mode); 
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode); 
	
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor);
	
	//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover=TRUE); 
	
	//填充cache块的结束函数
	virtual void EndFillBlock();
	virtual void BeginDisplayBlock();
	virtual void EndDisplayBlock();
	
	// x,y是客户窗口坐标
	virtual void DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4]);
	HBITMAP Create32MemBmp(int w, int h);
	void SetAlphaValues(HBITMAP hBmp);
public:
	static BOOL m_bUseGPU;
	BOOL m_bUseAlpha;
	BOOL m_bUseLinear;
	BOOL m_bDirectRenderVectors;
private:	
	tex_image   *m_pTextures;
	int			m_nTextures;
	HDC			m_hMemDC;
	HBITMAP     m_hBmp, m_hBmpOld;
};


class EXPORT_SMARTVIEW CCacheD3DTextData : public CCacheData
{
public:
	CCacheD3DTextData();
	virtual ~CCacheD3DTextData();
	
	void SetD3D(CD3DWrapper *p, BOOL bRenderVectors);
	virtual BOOL Init(CSize szBlock, int nBlock, DWORD clrBK);
	virtual void Destroy();
	virtual int GetType();
	virtual void ClearBlock(int idx);
	virtual BOOL AskforMoreBlocks(int num);
	
	//填充cache块的准备函数
	virtual void BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock=TRUE);
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer(GrElementList *pList, int mode); 
	
	//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode); 
	
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor);
	
	//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover=TRUE); 
	
	//填充cache块的结束函数
	virtual void EndFillBlock();
	virtual void BeginDisplayBlock();
	virtual void EndDisplayBlock();
	
	// x,y是客户窗口坐标
	virtual void DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4]);
	void SetAlphaValues(HBITMAP hBmp);
	HBITMAP Create32MemBmp(int w, int h);

protected:
	CD3DTexture *GetRenderTexture();
	CD3DTexture *GetSystemTextureForSetBitmap();
	CD3DTexture& GetCacheTexture(int index);
	
public:
	BOOL m_bUseAlpha, m_bUseLinearZoom;
	
private:
	CD3DWrapper *m_pD3D;
	BOOL m_bDirectRenderVectors;
	int m_nRenderTexture;
	int m_nSymTextureForSetBitmap;
	HDC			m_hMemDC;
	HDC         m_hTmpDC;

	CArray<int,int> m_arrCacheTextureIndex;
public:
	HBITMAP     m_hBmp, m_hBmpOld;
};




//cache的目的是通过空间缓存提高显示速度；
//cache中分块的策略由CCacheLoader派生类根据自己的特点来选择，
//矢量图层通常根据客户坐标系划分块，目的是减少矢量数据的重绘过程；
//影像图层通常根据影像的象素坐标系划分块，目的是减少影像数据的读取次数；
//这就需要我们在定义CCacheLoader时，并不限定cache的坐标系类型、划分方式，
//而是在更抽象的层次上定义它，

class EXPORT_SMARTVIEW CCacheLoader : public CObject
{
public:
	struct CacheID
	{
		enum
		{
			stateInvalid = 0,
			stateValid = 1,
			stateLoading = 2
		};
		int bvalid; // stateInvalid, stateValid, stateLoading
		int xnum;
		int ynum;
		int load_num; // 加载数目
		float zoom;   // 当前块的缩放比例
		CacheID(){
			xnum = 0; ynum = 0; bvalid = 0; load_num = 0;
			zoom = 0;
		}
		CacheID(int xn, int yn){
			xnum = xn;	ynum = yn; bvalid = stateValid; load_num = 0;
			zoom = 0;
		}
		void Set(int xn, int yn, int bv=stateValid){
			xnum = xn;	ynum = yn; bvalid = bv; load_num = 0;
		}
		BOOL IsNotInValid(){
			return (bvalid!=stateInvalid);
		}
		BOOL operator==(const CacheID& id){
			return( bvalid==id.bvalid && xnum==id.xnum && ynum==id.ynum && zoom==id.zoom );
		}
		BOOL CompareIncLoading(CacheID& id){
			if (bvalid==stateInvalid||id.bvalid==stateInvalid)
			{
				return FALSE;
			}
			return(	xnum==id.xnum && ynum==id.ynum );
		}
		CacheID& operator=(const CacheID& id){
			bvalid=id.bvalid; xnum=id.xnum; ynum=id.ynum;
			load_num = id.load_num;
			zoom = id.zoom;
			return *this;
		}
	};
	
	struct ReadQueue 
	{
		CacheID id;
		int blkidx;
	};
	
	struct CacheTable
	{
		int	nsum;
		CacheID *ids;
	};
	
	CCacheLoader();
	virtual ~CCacheLoader();

	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);
	virtual BOOL IsValid();
	virtual void Destroy();
	virtual BOOL DestroyCache();
	
	// {{--------------- 以下几个函数，派生类应该予以实现
	// 获得块编号的合理的取值范围
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax) = 0;

	// 获得焦点处所在的块编号
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum) = 0;

	// 获得块的四个角点坐标在客户坐标系下的XY坐标
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]) = 0;

	// 填充块的内容
	virtual BOOL FillBlock(int blkIdx, CacheID id);

	// 清除块的内容
	virtual void ClearBlock(int blkIdx);
	// ---------------}}
	
	// {{--------------- 以下几个函数有缺省实现，派生类不需要重载
	virtual void PlanBGLoad();
	
	virtual BOOL FindFreeBlock( int* pBlkIdx, CacheID* pid );
	
	virtual void QueueBlock(int blkIdx, CacheID id);
	
	virtual BOOL ReplyQueue(int queueIdx);
	
	virtual BOOL BGLoad();
	
	// set the focus(client coordinates)
	virtual void SetViewFocus(int x, int y);

	void EnableBGLoad(BOOL bEnable);

	virtual void ClearAll();

	void ClearQueue();

	int GetCacheType();

public:
	int GetBlockIdx( CacheID id );
	
public:
	BOOL m_bInitCacheOK;

	BOOL m_bEnableBGLoad;
	
	CSize	m_szBlock;
	
	CPoint	m_ptViewFocus;
	
	// cache data
	CacheTable	m_cacheTable;
	CCacheData *m_pCacheData;
	
	CArray<ReadQueue,ReadQueue> m_aReadQueue;
	
	BOOL		m_bLastFindFreeBlockFull;
};


MyNameSpaceEnd

#endif // !defined(AFX_CACHECONTROLLER_H__E11CDB98_9171_4A19_8ED2_F131F51AEFB5__INCLUDED_)
