// MemRasterLayer.h: interface for the CMemRasterLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMRASTERLAYER_H__3D7F5FEF_FF58_4151_B829_5D5CB66B7CCE__INCLUDED_)
#define AFX_MEMRASTERLAYER_H__3D7F5FEF_FF58_4151_B829_5D5CB66B7CCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RasterLayer.h"

MyNameSpaceBegin


//内存栅格图层，在正射影像修复中使用到
class EXPORT_SMARTVIEW CMemRasterLayer : public CDrawingLayer
{
	DECLARE_DYNAMIC(CMemRasterLayer)
public:
	CMemRasterLayer();
	virtual ~CMemRasterLayer();

	void SetImageSize(CSize szImg);
	CSize GetImageSize();
	CSize GetVirtualImageSize();
	
	virtual Envelope GetDataBound();
	
	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);	
	// 获得块编号的合理的取值范围
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax);
	
	// 获得焦点处所在的块编号
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum);
	
	// 获得块的四个角点坐标在客户坐标系下的XY坐标
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]);
	
	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);
	
	//pts包含四个顶点
	void SetCornerPts(BOOL bUseCorner, PT_3D* pts);
	//旋转和缩放
	void SetCornerPts(float ang, float fKX, float fKY);
	
	virtual void Destroy();
	
	void Draw();
	void ClearImageAdjust();
	
	void SetConvertCoordFunc(CObject *pObj, LPFUN_CONVERTCOORD pFunc){
		m_pFuncOwner = pObj;
		m_pFuncConvertCoord = pFunc;
	}

	BOOL WriteRectData(CRect inRect, HBITMAP hBmp, COLORREF bkColor);
	BOOL WriteRectData(CRect inRect, RGBQUAD* pClrTbl,
		int nBitCount, COLORREF bkColor, 
		BYTE* pSrc, int nMemXoffBytes, int nMemYoffBytes, 
		int nMemWidBytes, int nMemHeiBytes, 
		BOOL rgbOrder, BOOL bottomUp);

	virtual BOOL BGLoad();

	virtual void ClearAll(){}
	
protected:
	//强制填充cache块
	virtual int ForceFillBlock(CacheID id);
	
	virtual void DrawClient(double xl, double xh, double yl, double yh);
	
	//rcImage是影像范围，x[4],y[4]是四个角点在客户坐标系下的坐标
	
	//影像的像素坐标和地理坐标之间的转换接口
	virtual void GroundToImage(PT_4D *pts, int num);
	virtual void ImageToGround(PT_4D *pts, int num);
	
private:
	
	void CalcAffineParm();
	
	void CalcImageAdjust(HBITMAP hBmp);
	
protected:
	
	//四个顶点坐标，以及变换系数
	PT_3D		m_ptsCorner[4];
	BOOL		m_bUseCornerPts;
	BOOL		m_bCalcParam;
	double		m_lfA[3], m_lfB[3];
	CSize		m_szImage;
	float		m_fKX, m_fKY, m_fRotAng;
	
public:
	
	CImageAdjust m_Adjust;
	
	CObject	 *m_pFuncOwner;
	LPFUN_CONVERTCOORD m_pFuncConvertCoord;
};

MyNameSpaceEnd

#endif // !defined(AFX_MEMRASTERLAYER_H__3D7F5FEF_FF58_4151_B829_5D5CB66B7CCE__INCLUDED_)
