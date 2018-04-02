// MutiRasterLayer.h: interface for the CMutiRasterLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTIRASTERLAYER_H__4281CBBF_1965_442D_BF9A_49980BAA1B27__INCLUDED_)
#define AFX_MUTIRASTERLAYER_H__4281CBBF_1965_442D_BF9A_49980BAA1B27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DrawingLayer.h"
#include "ImageRead.h"
#include "ImageAdjust.h"

MyNameSpaceBegin

//以下是对多影像漫游的支持代码
// struct IMGID
// {
// 	enum
// 	{
// 		stateInvalid = 0,
// 			stateValid = 1,
// 			stateLoading = 2
// 	};
// 	int idx;    //影像索引	
// 	int xnum:16;
// 	int ynum:16;
// 	int tempDis;
// 	unsigned int/*BOOL*/ bvalid:2; // stateInvalid, stateValid, stateLoading
// 	unsigned int/*BOOL*/ bPartCover:1;
// 	unsigned int bPart:1;
// 	unsigned int xsz:14;
// 	unsigned int ysz:14;
// 	IMGID(){
// 		idx = -1;
// 		tempDis = -1;
// 		xnum = 0; ynum = 0; bvalid = 0;
// 		bPartCover = FALSE;
// 		bPart = FALSE;
// 		xsz = ysz = 0;
// 	}
// 	IMGID(int i, int xn, int yn, int bv=stateValid){
// 		idx = i;
// 		xnum = xn;	ynum = yn; bvalid = stateValid;
// 		tempDis = -1;
// 		bPartCover = FALSE;
// 		bPart = FALSE;
// 		xsz = ysz = 0;
// 	}
// 	void SetPart(BOOL part=FALSE, int x=0, int y=0){		
// 		bPart = part;
// 		xsz = x;
// 		ysz = y;
// 	}
// 	void Set(int i, int xn, int yn, int bv=stateValid, BOOL part=FALSE, int x=0, int y=0){
// 		idx = i;
// 		xnum = xn;	ynum = yn; bvalid = bv;
// 		bPart = part;
// 		xsz = x;
// 		ysz = y;
// 	}
// 	BOOL IsNotInValid(){
// 		return (bvalid!=stateInvalid);
// 	}
// 	BOOL operator==(const IMGID& id){
// 		return( idx==id.idx && bvalid==id.bvalid 
// 			&&xnum==id.xnum && ynum==id.ynum
// 			&&bPart==id.bPart&&xsz==id.xsz&&ysz==id.ysz);
// 	}
// 	BOOL CompareIncLoading(IMGID& id){
// 		if (bvalid==stateInvalid||id.bvalid==stateInvalid)
// 		{
// 			return FALSE;
// 		}
// 		return(	xnum==id.xnum && ynum==id.ynum );
// 	}
// 	IMGID& operator=(const IMGID& id){
// 		if (this==&id)
// 		{
// 			return *this;
// 		}
// 		tempDis = id.tempDis;
// 		bPartCover = id.bPartCover;
// 		idx = id.idx;
// 		bvalid=id.bvalid; xnum=id.xnum; ynum=id.ynum;
// 		bPart = id.bPart;xsz = id.xsz;ysz = id.ysz;
// 		return *this;
// 	}
// };
// 
// 
// struct IMGREADQUEUE 
// {
// 	IMGID id;
// 	int blkidx;
// };
// 
// 
// struct IMGCACHETABLE
// {
// 	int		nsum;
// 	IMGID	*ids; //col + row * colwidth  -1 means empty	
// };

typedef struct tagReqImgInfo
{
	int idx;//影像的索引
	Envelope e;//基于影像坐标的矩形区
}ReqImgInfo;

typedef struct tagCHEIDImg
{
	CCacheLoader::CacheID id;
	ReqImgInfo *p;				//按优先级排列
	int nsum;
}CHEIDImg;

typedef struct ImgParaItem
{
	ImgParaItem()
	{
		memset(m_strImgName,0,sizeof(m_strImgName));
		m_bUseCornerPts = FALSE;
		m_bCalcParam = FALSE;		
		m_fKX = m_fKY = 1.0f;		
		m_szImage = CSize(-1,-1);
		m_fRotAng = 1.0;
		m_transColor = 0;
//		m_pImgInfo = NULL;
	}
	~ImgParaItem()
	{
// 		if (m_pImgInfo)
// 		{
// 			delete m_pImgInfo;
// 		}
//		m_rgn.DeleteObject();
	}
	TCHAR        m_strImgName[_MAX_PATH];
	PT_3D		m_ptsCorner[4];
	BOOL		m_bUseCornerPts;
	BOOL		m_bCalcParam;
	double		m_lfA[3], m_lfB[3];
	CSize		m_szImage;		//影像的原始尺寸
	float		m_fKX, m_fKY, m_fRotAng;
	COLORREF    m_transColor;
//	PImgInfo    m_pImgInfo;
/*	CRgn        m_rgn;	//未被上层影像遮盖的可见区域*/

}ImgParaItem;

typedef struct	tagImgSkipInfo
{
	tagImgSkipInfo()
	{
		bSkipRead = FALSE;
		nSkipReadRate = 0;
	}
	BOOL bSkipRead;
	int  nSkipReadRate;

}ImgSkipInfo;

//typedef BOOL (CObject::*LPFUN_CONVERTCOORD)(double x0, double y0, double& x1, double& y1, BOOL bOrder);
class EXPORT_SMARTVIEW CMutiRasterLayer : public CDrawingLayer  
{
	DECLARE_DYNAMIC(CMutiRasterLayer)
public:	
	CMutiRasterLayer();
	virtual ~CMutiRasterLayer();
public:
	BOOL InitParams();
	BOOL AddImage(LPCTSTR fileName);
	BOOL RemoveImgAt(int idx);
	BOOL InsertImgAt(int idx, LPCTSTR fileName);
	BOOL RemoveAllImgs();
	int GetImgNum();	
	void SetTransColor(LPCTSTR fileName,COLORREF color);
//	virtual void Draw();

	//pts包含四个顶点
	void SetCornerPts(LPCTSTR fileName,BOOL bUseCorner, PT_3D* pts);
	//旋转和缩放
	void SetCornerPts(LPCTSTR fileName,float ang, float fKX, float fKY);

	LPCTSTR GetImgFileName(int idx);
	CSize GetImageOriginalSize(int idx);
	CSize GetImageOriginalSize(LPCTSTR fileName);

	CSize GetImageSize(int idx);
	CSize GetImageSize(LPCTSTR fileName);

	virtual Envelope GetDataBound();

	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);

	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum);
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax);

	// 获得块的四个角点坐标在客户坐标系下的XY坐标
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]);


	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);

	virtual void Destroy();	
	
	
	void ClearImageAdjust();
	BOOL FillBlock(int blkIdx, CacheID id);
	
	
protected:
	
	void GetBlockImgInfo(CacheID id,CArray<ReqImgInfo,ReqImgInfo> &arrImgInfo);
//	int FileNameToIndex(const char fileName[100]);
	virtual void DrawClient(double xl, double xh, double yl, double yh);
	
	//rcImage是影像范围，x[4],y[4]是四个角点在客户坐标系下的坐标
	
	//影像的像素坐标和地理坐标之间的转换接口
	virtual void GroundToImage(int idx, PT_4D *pts, int num);
	virtual void ImageToGround(int idx, PT_4D *pts, int num);
	
	BOOL FillBlockWithImage(int blkIdx, CacheID id);
	
	float GetActualZoomRate(int idx);
	void ClearCHEIDImgs();

	void CalcClientBound();
	
private:
	void LinkImageThread();
	void UnlinkImageThread();
	LRESULT OnThreadMsg(WPARAM wParam, LPARAM lParam);
//	int FindIdxInArray(const CArray<CacheID,CacheID>& arr,const CacheID &id);
	void SetZoomRate( float zoomRate);
	void CalcAffineParm(int idx);
	
	void CalcImageAdjust(HBITMAP hBmp);
	
protected:
	//由 CThreadImageRead 读出的影像像素，是基于左下角起点的
	//也就是读出的影像坐标为(0,0)的像素就是左下角点
	BOOL m_bImgInitOK;
	CThreadMutiImageRead	m_imageReadThread;

	CDialog     *m_pDummyDialog;
	ReadQueue	m_qThreadReadItem;
	
	HBITMAP		m_hBlockBmp;	
	
	CArray<ImgParaItem*,ImgParaItem*> m_arrImgPars;
	CArray<ImgSkipInfo*,ImgSkipInfo*> m_arrSkipInfos;	
	CArray<CHEIDImg,CHEIDImg> m_arrCHEIDImgs;
	
	CRITICAL_SECTION m_critical;

	Envelope  m_dataBound;

	Envelope m_DataClientBound;
	BOOL	 m_bCalcClientBound;

	// cache data
	
public:
	BOOL		m_bUseImageThread;
	
	CImageAdjust m_Adjust;
	
//	CObject	 *m_pFuncOwner;
//	LPFUN_CONVERTCOORD m_pFuncConvertCoord;

};

MyNameSpaceEnd

#endif // !defined(AFX_MUTIRASTERLAYER_H__4281CBBF_1965_442D_BF9A_49980BAA1B27__INCLUDED_)
