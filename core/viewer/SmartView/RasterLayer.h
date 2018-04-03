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
	// ��ÿ��ŵĺ����ȡֵ��Χ
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax);
	
	// ��ý��㴦���ڵĿ���
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum);
	
	// ��ÿ���ĸ��ǵ������ڿͻ�����ϵ�µ�XY����
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]);

	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);

	//pts�����ĸ�����
	void SetCornerPts(BOOL bUseCorner, PT_3D* pts);
	//��ת������
	void SetCornerPts(float ang, float fKX, float fKY);
	//�任����
	void SetTransform(double m[9]);

	virtual void Destroy();

	virtual void ClearAll();

	virtual void PlanBGLoad();

	virtual BOOL FillBlock(int blkIdx, CacheID id);

	//����ʵ�֣�����ÿ����� zoom ����
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

	//rcImage��Ӱ��Χ��x[4],y[4]���ĸ��ǵ��ڿͻ�����ϵ�µ�����

	//Ӱ�����������͵�������֮���ת���ӿ�
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

	//�� CThreadImageRead ������Ӱ�����أ��ǻ������½�����
	//Ҳ���Ƕ�����Ӱ������Ϊ(0,0)�����ؾ������½ǵ�
	//��������½���ָ���Էǵ�����Ϣ��Ӱ����ԣ�����ACDSee������鿴�����½ǣ����е��������Ӱ����ԣ����ǵ���������㣻
	CThreadImageRead	m_image;
	BOOL		m_bSkipRead;
	int			m_nSkipReadRate;
	CDialog     *m_pDummyDialog;
	ReadQueue	m_qThreadReadItem;

	HBITMAP		m_hBlockBmp;

	//�ĸ��������꣬�Լ��任ϵ��
	PT_3D		m_ptsCorner[4];
	BOOL		m_bUseCornerPts;
	BOOL		m_bCalcParam;
	double		m_lfA[3], m_lfB[3];
	CSize		m_szImage;
	float		m_fKX, m_fKY, m_fRotAng;

	CRITICAL_SECTION m_critical;

	BOOL		m_bEnableLoadAll;
	BOOL		m_bLoadedAll;

	//�������껻��Ľ�������ʵʱ���껻����ٶ�
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
