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
	//��ʱ����
	struct TempObj
	{
		TempObj(){
			no = 0; pBuf = 0;
			color = 0; usecolor = 0; visible = 1;
		}
		LONG_PTR no;		 //����ָ��
		GrBuffer *pBuf;  //ͼ�Σ����pBufΪ�գ���ö������Ҫʹ�õ���ָ��ʵʱ����
		DWORD color:24;  //��ɫ
		DWORD usecolor:1; //�Ƿ�ʹ����ɫ
		DWORD visible:1; //�ɼ���
	};
	struct BlockFtrs
	{
		int xnum, ynum;
		CArray<CFeature*,CFeature*> m_arrFtrs;

		//m_arrFlags���ȵ���m_arrFtrs��ÿ������һ����־�ֽڣ�
		//0��ʾû�м��������ཻ�ԣ�1��ʾ�����������ཻ�Ե����ཻ��2��ʾ�����������ཻ�����ཻ
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
	
	// ��ÿ��ŵĺ����ȡֵ��Χ
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax);
	
	// ��ý��㴦���ڵĿ���
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum);
	
	// ��ÿ���ĸ��ǵ������ڿͻ�����ϵ�µ�XY����
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

	// ���������
	virtual BOOL FillBlock(int blkIdx, CacheID id);
	
	// ��������ת�����ͻ������������(xl,xh,yl,yh)�ཻ��ʸ��
	virtual void DrawClient(double xl, double xh, double yl, double yh);
	void DrawClientWithCache(double xl, double xh, double yl, double yh, BOOL bUseCache, BOOL bStart, BOOL bEnd);

	//ǿ�����cache��
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
	
	//ͼ��ӳ��ľ��������û���ͼ���е�ʸ���ٴα任��ȱʡΪ��λ����
	double   m_DataMatrix[16], m_DataRMatrix[16];

	CObject *m_pDrawMan;
	PFDrawFtr m_pDrawFun;
	PFCheckLayerVisible m_pCheckLayerFun;
	
	//��¼��Խcache��ĵ��
	CArray<BlockFtrs*,BlockFtrs*> m_arrPBlockFtrs;
	//��m_arrPBlockFtrsʹ�ã���� m_arrPBlockFtrs ���ٶ�;
	CArray<CPFeature,CPFeature> m_arrPBlockAllFtrs; 

	GrElementList m_eleListBlockFtrs;
	int m_nLoadMaxNum;
	int m_nDrawNum;

	//���ǵ����ڿ��д�Խ�ĵ�������Ժܴ�Ϊ������ٶȣ����ǻ���2000������ķ�������(GrBuffer)
	CArray<GrElement*,GrElement*> m_arrCacheBufs;  //���ձ���������򣬱��ڿ��ٲ���
	GrElementList m_listCacheBufs;  //�����Ⱥ�˳�򹹳����������޸ģ����ʹ�õĻᱻ����β����

	//��������б����������ʱ������߶����õ���ĸ�����������ɫ���ɼ��ԣ���
	//����Ҫ��һ�����õ�������������ʱ��������ͻ����һ��TempObj����������
	//��һ�����TempObj::pBuf�ǿգ��ڶ������TempObj::pBufΪ�գ�
	CFArray<TempObj> m_arrTempObjs;	
	GrElementList m_eleListTempObjsForemost;   //�ⲿ����һ��������֮ǰ����
	GrElementList m_eleListTempObjsUppermost;  //�ⲿ����һ��������֮�����
	
	//�ǻ���ʸ�����Լ��Ƿ���Ʊ�ǿ�
	BOOL	 m_bDrawMark1,m_bDrawMark2;
	
	//��ǿ�Ⱥ���ɫ
	float m_fMarkWidth1, m_fMarkWidth2;
	COLORREF m_clrMark1, m_clrMark2; 
	
	//�����ݣ����ڿ��ٵĻ��ƻ��߲����������������ɺ������ݾ��Զ��ͷţ�
	CArray<GrBuffer*,GrBuffer*> m_arrDirtyPBufs;
	BOOL m_bTooManyDirtyBufs;

	BOOL m_bEnableDelayDraw;

	//�Ƿ�Ϊ2d����ϵ��ȱʡ��ʸ����ͼΪ2D����ϵ������ͼΪ3D����ϵ��
	BOOL m_b2DCoordSys;

	//�Ƿ���Ҫ���Ż�
	BOOL m_bSymbolized;

};

#endif // !defined(AFX_REALTIMEVECTLAYER_H__8480839F_4E9C_4C2F_84E3_6611EFA84E24__INCLUDED_)
