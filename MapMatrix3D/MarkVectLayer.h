// MarkVectLayer.h: interface for the CMarkVectLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MARKVECTLAYER_H__47F25F50_7161_44C5_84F0_992FED7E3087__INCLUDED_)
#define AFX_MARKVECTLAYER_H__47F25F50_7161_44C5_84F0_992FED7E3087__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingLayer.h"
#include "GrElementList.h"
#include "Selection.h"


class CDlgDataSource;

class CMarkVectLayer : public CDrawingLayer
{
public:

	CMarkVectLayer();
	virtual ~CMarkVectLayer();

	DECLARE_DYNAMIC(CMarkVectLayer)

	void Init(CSelection *pSel, CDlgDataSource *pDS, CCoordSys *pDrawContentCS, CCoordSys *pCS);
	virtual void Destroy();

	// update selection drawing
	void UpdateMark1();

	// update mark drawing
	void SetAllMark2(BOOL bMark2);

	void SetMark1Width(float wid){
		m_fMarkWidth1 = wid;
	}
	void SetMark2Width(float wid){
		m_fMarkWidth2 = wid;
	}
	void SetMark1Color(COLORREF clr){
		m_clrMark1 = clr;
	}
	void SetMark2Color(COLORREF clr){
		m_clrMark2 = clr;
	}
	float GetMark1Width(){
		return m_fMarkWidth1;
	}
	float GetMark2Width(){
		return m_fMarkWidth2;
	}
	COLORREF GetMark1Color(){
		return m_clrMark1;
	}
	COLORREF GetMark2Color(){
		return m_clrMark2;
	}

	COLORREF GetFtrPtColor(){
		return m_clrFtrPtColor;
	}
	void SetFtrPtColor(COLORREF clr){
		m_clrFtrPtColor = clr;
	}

	virtual Envelope GetDataBound();
	void SetDataBound(Envelope e);
	void SetDataMatrix(double m[16]);

	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock);

	// 获得块编号的合理的取值范围
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax);
	
	// 获得焦点处所在的块编号
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum);
	
	// 获得块的四个角点坐标在客户坐标系下的XY坐标
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]);

	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);

protected:
	// 填充块的内容
	virtual BOOL FillBlock(int blkIdx, CacheID id);

	// 绘制所有转换到客户坐标后与区域(xl,xh,yl,yh)相交的矢量
	virtual void DrawClient(double xl, double xh, double yl, double yh);
	void DrawClientWithCache(double xl, double xh, double yl, double yh, BOOL bUseCache);

	void CalcClientBound();
	
protected:
	CSelection *m_pSel;
	CDlgDataSource *m_pDS;
	CCoordSys *m_pDrawContentCS;
	CCoordSys *m_pCS;

	//选中框
	GrElementList m_eleList1;

	//内节点
	GrElementList m_eleList2;

	//母线
	GrElementList m_eleList_baseline;
	
	Envelope m_DataBound;
	Envelope m_DataClientBound;
	BOOL	 m_bCalcClientBound;

	//标记宽度和颜色
	float m_fMarkWidth1, m_fMarkWidth2;
	COLORREF m_clrMark1, m_clrMark2; 

	COLORREF m_clrFtrPtColor;

	//图形映射的矩阵，允许用户将图层中的矢量再次变换，缺省为单位矩阵
	double   m_DataMatrix[16], m_DataRMatrix[16];

};

#endif // !defined(AFX_MARKVECTLAYER_H__47F25F50_7161_44C5_84F0_992FED7E3087__INCLUDED_)
