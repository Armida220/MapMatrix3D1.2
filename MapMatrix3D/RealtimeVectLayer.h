// RealtimeVectLayer.h: interface for the CRealtimeVectLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REALTIMEVECTLAYER_H__8480839F_4E9C_4C2F_84E3_6611EFA84E24__INCLUDED_)
#define AFX_REALTIMEVECTLAYER_H__8480839F_4E9C_4C2F_84E3_6611EFA84E24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingLayer.h"
#include "GrElementList.h"
#include "EditbaseDoc.h"

#define  SYMBOLIZE_NONE			0
#define  SYMBOLIZE_ALL			1
#define  SYMBOLIZE_PART			2



typedef void (CObject::*PFDrawFtr)(CDlgDataSource *pDS,CFtrLayer *pLayer,CFeature *pFtr, int nSymbolizedFlag, GrBuffer *pBuf);
typedef BOOL (CObject::*PFCheckLayerVisible)(CFtrLayer *pLayer);

class CRealtimeVectLayer : public CDrawingLayer  
{
	DECLARE_DYNAMIC(CRealtimeVectLayer)

public:
	//临时对象
	struct TempObj
	{
		TempObj(){
			no = 0; pBuf = 0;
			color = 0; usecolor = 0; visible = 1;
		}
		LONG_PTR no;		 //地物指针
		GrBuffer *pBuf;  //图形，如果pBuf为空，则该对象就需要使用地物指针实时绘制
		DWORD color:24;  //颜色
		DWORD usecolor:1; //是否使用颜色
		DWORD visible:1; //可见性
	};
	struct BlockFtrs
	{
		int xnum, ynum;
		CArray<CFeature*,CFeature*> m_arrFtrs;

		//m_arrFlags长度等于m_arrFtrs，每个地物一个标志字节：
		//0表示没有计算区域相交性，1表示计算了区域相交性但不相交，2表示计算了区域相交性且相交
		CArray<BYTE,BYTE> m_arrFlags; 
	};

	CRealtimeVectLayer();
	virtual ~CRealtimeVectLayer();

	void Init(CDlgDoc *pDoc, CDlgDataQuery *pDQ, CCoordSys *pDrawContentCS, CCoordSys *pCS, CObject *pObj, PFDrawFtr pfun1, PFCheckLayerVisible pfun2);
	virtual void Destroy();

	void ClearBlockOfObj(LONG_PTR objnum);
	
	virtual Envelope GetDataBound();
	void SetDataBound(Envelope e);
	void SetDataMatrix(double m[16]);

	void *OpenObj(LONG_PTR objnum, BOOL bUppermost = FALSE);
	void FinishObj(LONG_PTR objnum);
	void DelObj(LONG_PTR objnum);
	void DelAllObjs();

	void SetObjColor(LONG_PTR objnum, COLORREF color);
	void SetObjVisible(LONG_PTR objnum, BOOL bVisble);

	virtual BOOL ExtraPrepareCaches();
	virtual void Draw();
	
	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock);
	
	// 获得块编号的合理的取值范围
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax);
	
	// 获得焦点处所在的块编号
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum);
	
	// 获得块的四个角点坐标在客户坐标系下的XY坐标
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]);
	
	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);

	virtual BOOL CheckCacheReady();

	virtual void ClearAll();

	void EraseObjFromCache(CFeature *pFtr);
	void DrawObjtoCache(CFeature *pFtr);

	BOOL EnableDelayDraw(BOOL bEnable);

	void EnableSymbolized(BOOL bSymbolized);
	void Enable2DCoordSys(BOOL b2DCoordSys);
	
protected:

	BOOL FindObj(LONG_PTR objnum, int& insert_idx);

	// 填充块的内容
	virtual BOOL FillBlock(int blkIdx, CacheID id);
	
	// 绘制所有转换到客户坐标后与区域(xl,xh,yl,yh)相交的矢量
	virtual void DrawClient(double xl, double xh, double yl, double yh);
	void DrawClientWithCache(double xl, double xh, double yl, double yh, BOOL bUseCache, BOOL bStart, BOOL bEnd);

	//强制填充cache块
	virtual int ForceFillBlock(CacheID id);
	
	void CalcClientBound();

	BOOL PrepareData(int blkIdx, CacheID id, double xl, double xh, double yl, double yh, int& new_num);
	//  in 3d coordinate system
	BOOL PrepareData_3DCS(int blkIdx, CacheID id, double xl, double xh, double yl, double yh, int& new_num);

	// for DrawClient()
	BOOL PrepareData(double xl, double xh, double yl, double yh, int draw_limit, CArray<CPFeature,CPFeature>& arrAllFtrs, int& draw_num);
	
	// for DrawClient() in 3d coordinate system
	BOOL PrepareData_3DCS(double xl, double xh, double yl, double yh, int draw_limit, CArray<CPFeature,CPFeature>& arrAllFtrs, int& draw_num);

	void DrawDirtyObjs();

	BOOL FindBuf(LONG_PTR objnum, int& insert_idx);

	int	CalcSymbolizedFlag(double gscale);

	GrElement *FindEleListTempObjs(LONG_PTR objnum);
	
protected:
	CDlgDoc *m_pDoc;
	CDlgDataQuery *m_pDQ;
	CDlgDataSource *m_pMainDS;

	CCoordSys *m_pDrawContentCS;
	CCoordSys *m_pCS;
	
	Envelope m_DataBound;
	Envelope m_DataClientBound;
	BOOL	 m_bCalcClientBound;
	
	//图形映射的矩阵，允许用户将图层中的矢量再次变换，缺省为单位矩阵
	double   m_DataMatrix[16], m_DataRMatrix[16];

	CObject *m_pDrawMan;
	PFDrawFtr m_pDrawFun;
	PFCheckLayerVisible m_pCheckLayerFun;
	
	//记录穿越cache块的地物；
	CArray<BlockFtrs*,BlockFtrs*> m_arrPBlockFtrs;
	//供m_arrPBlockFtrs使用，提高 m_arrPBlockFtrs 的速度;
	CArray<CPFeature,CPFeature> m_arrPBlockAllFtrs; 

	GrElementList m_eleListBlockFtrs;
	int m_nLoadMaxNum;
	int m_nDrawNum;

	//考虑到相邻块中穿越的地物相关性很大，为了提高速度，我们缓存2000个地物的符号数据(GrBuffer)
	CArray<GrElement*,GrElement*> m_arrCacheBufs;  //按照编号作了排序，便于快速查找
	GrElementList m_listCacheBufs;  //按照先后顺序构成链表，便于修改；最近使用的会被放在尾部；

	//地物对象列表，用来存放临时地物；或者对永久地物的附加描述（颜色、可见性）；
	//当需要对一个永久地物作附加描述时，该数组就会存在一个TempObj来描述它；
	//第一种情况TempObj::pBuf非空；第二种情况TempObj::pBuf为空；
	CFArray<TempObj> m_arrTempObjs;	
	GrElementList m_eleListTempObjsForemost;   //这部分在一般地物绘制之前绘制
	GrElementList m_eleListTempObjsUppermost;  //这部分在一般地物绘制之后绘制
	
	//是绘制矢量，以及是否绘制标记框
	BOOL	 m_bDrawMark1,m_bDrawMark2;
	
	//标记宽度和颜色
	float m_fMarkWidth1, m_fMarkWidth2;
	COLORREF m_clrMark1, m_clrMark2; 
	
	//脏数据；用于快速的绘制或者擦除少数地物；绘制完成后，脏数据就自动释放；
	CArray<GrBuffer*,GrBuffer*> m_arrDirtyPBufs;
	BOOL m_bTooManyDirtyBufs;

	BOOL m_bEnableDelayDraw;

	//是否为2d坐标系；缺省的矢量视图为2D坐标系，侧视图为3D坐标系；
	BOOL m_b2DCoordSys;

	//是否需要符号化
	BOOL m_bSymbolized;

};

#endif // !defined(AFX_REALTIMEVECTLAYER_H__8480839F_4E9C_4C2F_84E3_6611EFA84E24__INCLUDED_)
