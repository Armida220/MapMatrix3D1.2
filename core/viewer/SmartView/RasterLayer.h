// RasterLayer.h: interface for the CRasterLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASTERLAYER_H__59675E27_F3A4_434E_9AB3_4B7C6C487519__INCLUDED_)
#define AFX_RASTERLAYER_H__59675E27_F3A4_434E_9AB3_4B7C6C487519__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingLayer.h"
#include "ImageRead.h"
#include "ImageAdjust.h"

MyNameSpaceBegin


typedef BOOL (CObject::*LPFUN_CONVERTCOORD)(double x0, double y0, double& x1, double& y1, BOOL bOrder);


class EXPORT_SMARTVIEW CRasterLayer : public CDrawingLayer  
{
	DECLARE_DYNAMIC(CRasterLayer)
public:
	CRasterLayer();
	virtual ~CRasterLayer();

	struct CoordConvertCache
	{
		int isvalid;
		PT_4D pts[4];
	};

public:
	void SetUISCacheDir(LPCTSTR dir);
	BOOL AttachImage(LPCTSTR fileName);
	void DetachImage();

	virtual void SetContext(CDrawingContext *pContext);
	BOOL IsValidImage();
	LPCTSTR GetImgFileName();
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
	//变换矩阵
	void SetTransform(double m[9]);

	virtual void Destroy();

	virtual void ClearAll();

	virtual void PlanBGLoad();

	virtual BOOL FillBlock(int blkIdx, CacheID id);

	//重新实现，考虑每个块的 zoom 参数
	virtual BOOL FindFreeBlock(int* pBlkIdx, CacheID* pid);

	virtual void QueueBlock(int blkIdx, CacheID id);
	virtual BOOL CheckCacheReady();
	virtual void Draw();

	void ClearImageAdjust();

	void SetConvertCoordFunc(CObject *pObj, LPFUN_CONVERTCOORD pFunc){
		m_pFuncOwner = pObj;
		m_pFuncConvertCoord = pFunc;
	}

	CImageRead *GetImageRead();
	
public:
	
	virtual void DrawClient(double xl, double xh, double yl, double yh);

	//rcImage是影像范围，x[4],y[4]是四个角点在客户坐标系下的坐标

	//影像的像素坐标和地理坐标之间的转换接口
	virtual void GroundToImage(PT_4D *pts, int num);
	virtual void ImageToGround(PT_4D *pts, int num);
	void ImageToGroundWithZoom(PT_4D *pts, int num, float zoom);

	BOOL FillBlockWithImage(int blkIdx, CacheID id);

	float GetActualZoomRate();

private:
	void LinkImageThread();
	void UnlinkImageThread();
	LRESULT OnThreadMsg(WPARAM wParam, LPARAM lParam);

	void SetZoomRate(float zoomRate);
	void CalcAffineParm();

	void CalcImageAdjust(HBITMAP hBmp);

	void LoadAllBlocks();
		
protected:
	CString		m_strUISCacheDir;

	//由 CThreadImageRead 读出的影像像素，是基于左下角起点的
	//也就是读出的影像坐标为(0,0)的像素就是左下角点
	//这里的左下角是指：对非地理信息的影像而言，是用ACDSee等软件查看的左下角；对有地理坐标的影像而言，就是地理坐标起点；
	CThreadImageRead	m_image;
	BOOL		m_bSkipRead;
	int			m_nSkipReadRate;
	CDialog     *m_pDummyDialog;
	ReadQueue	m_qThreadReadItem;

	HBITMAP		m_hBlockBmp;

	//四个顶点坐标，以及变换系数
	PT_3D		m_ptsCorner[4];
	BOOL		m_bUseCornerPts;
	BOOL		m_bCalcParam;
	double		m_lfA[3], m_lfB[3];
	CSize		m_szImage;
	float		m_fKX, m_fKY, m_fRotAng;

	CRITICAL_SECTION m_critical;

	BOOL		m_bEnableLoadAll;
	BOOL		m_bLoadedAll;

	//缓存坐标换算的结果，提高实时坐标换算的速度
	int m_ccXmin, m_ccXmax, m_ccYmin, m_ccYmax;
	float m_ccZoom;
	CArray<CoordConvertCache,CoordConvertCache> m_ccCache;
	CacheID		m_idConvert;
public:
	BOOL		m_bUseImageThread;

	BOOL		m_bPauseLoadAll;

	BOOL		m_bNotClearCacheOnZoom;

	CImageAdjust m_Adjust;

	CObject	 *m_pFuncOwner;
	LPFUN_CONVERTCOORD m_pFuncConvertCoord;
};


MyNameSpaceEnd

#endif // !defined(AFX_RASTERLAYER_H__59675E27_F3A4_434E_9AB3_4B7C6C487519__INCLUDED_)
