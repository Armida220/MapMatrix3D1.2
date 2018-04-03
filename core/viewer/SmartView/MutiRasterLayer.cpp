// MutiRasterLayer.cpp: implementation of the CMutiRasterLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlobalFunc.h "
#include "MutiRasterLayer.h"
#include "Matrix.h "
#include "DummyDialog.h "
#include "resource.h"
#include "SmartViewFunctions.h"

//#include "Functions_temp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern AFX_EXTENSION_MODULE SmartViewDLL;


MyNameSpaceBegin

IMPLEMENT_DYNAMIC(CMutiRasterLayer, CDrawingLayer)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMutiRasterLayer::CMutiRasterLayer():m_imageReadThread()
{
	m_pDummyDialog = NULL;
	m_qThreadReadItem.blkidx = -1;
	m_bUseImageThread = TRUE;
	m_hBlockBmp = NULL;
	m_bCalcClientBound = FALSE;
//	m_pFuncOwner = NULL;
//	m_pFuncConvertCoord = NULL;
//	m_bForceDraw = FALSE;

	m_bImgInitOK = FALSE;
	InitializeCriticalSection(&m_critical);
}

CMutiRasterLayer::~CMutiRasterLayer()
{
	UnlinkImageThread();
	m_imageReadThread.Close();
	for (int i=0;i<m_arrImgPars.GetSize();i++)
	{
		delete m_arrSkipInfos[i];
		delete m_arrImgPars[i];
	}
	m_arrSkipInfos.RemoveAll();
	m_arrImgPars.RemoveAll();
	Destroy();

	DeleteCriticalSection(&m_critical);
}

//AddImage和setcorner之后调用
BOOL CMutiRasterLayer::InitParams()
{
	//计算外包
	m_dataBound = Envelope();
	Envelope e;
	for (int i=m_arrImgPars.GetSize()-1;i>=0;i--)
	{
		if( m_arrImgPars[i]->m_bUseCornerPts )
		{			
			e.CreateFromPts(m_arrImgPars[i]->m_ptsCorner,4);
			m_dataBound.Union(&e);			
		}
// 		CRgn rgn,rgn0,rgn1;
// 		rgn.CreateRectRgn(0,0,0,0);
// 		rgn0.CreateRectRgn(0,0,0,0);
// 		rgn1.CreateRectRgn(0,0,0,0);
// 		for (int j=m_arrImgPars.GetSize()-1;j>i;j--)
// 		{
// 			PT_4D pts[4];
// 			COPY_3DPT((pts[0]),(m_arrImgPars[j]->m_ptsCorner[0]));
// 			COPY_3DPT((pts[1]),(m_arrImgPars[j]->m_ptsCorner[1]));
// 			COPY_3DPT((pts[2]),(m_arrImgPars[j]->m_ptsCorner[2]));
// 			COPY_3DPT((pts[3]),(m_arrImgPars[j]->m_ptsCorner[3]));
// 			GroundToImage(i,pts,4);
// 			POINT point[4];
// 			point[0].x = pts[0].x;point[0].y = pts[0].y;
// 			point[1].x = pts[1].x;point[1].y = pts[1].y;
// 			point[2].x = pts[2].x;point[2].y = pts[2].y;
// 			point[3].x = pts[3].x;point[3].y = pts[3].y;
// 			rgn1.CopyRgn(&rgn0);
// 			rgn0.CreatePolygonRgn(point,4,ALTERNATE);
// 			rgn.CombineRgn(&rgn1,&rgn0,RGN_OR);
// 			rgn0.CopyRgn(&rgn);
// 		}
// 		PT_4D pts[4];
// 		COPY_3DPT((pts[0]),(m_arrImgPars[i]->m_ptsCorner[0]));
// 		COPY_3DPT((pts[1]),(m_arrImgPars[i]->m_ptsCorner[1]));
// 		COPY_3DPT((pts[2]),(m_arrImgPars[i]->m_ptsCorner[2]));
// 		COPY_3DPT((pts[3]),(m_arrImgPars[i]->m_ptsCorner[3]));
// 		GroundToImage(i,pts,4);
// 		POINT point[4];
// 		point[0].x = pts[0].x;point[0].y = pts[0].y;
// 		point[1].x = pts[1].x;point[1].y = pts[1].y;
// 		point[2].x = pts[2].x;point[2].y = pts[2].y;
// 		point[3].x = pts[3].x;point[3].y = pts[3].y;		
// 		rgn0.DeleteObject();
// 		rgn0.CreatePolygonRgn(point,4,ALTERNATE);
// 		m_arrImgPars[i]->m_rgn.CreateRectRgn(0,0,0,0);
// 		m_arrImgPars[i]->m_rgn.CombineRgn(&rgn0,&rgn,RGN_DIFF);
// 		rgn.DeleteObject();
// 		rgn0.DeleteObject();
// 		rgn1.DeleteObject();
	}
	m_imageReadThread.SetImgParmasArr(&m_arrImgPars);
	m_imageReadThread.SetArrSkipInfos(&m_arrSkipInfos);
	m_hBlockBmp = Create24BitDIB(m_szBlock.cx,m_szBlock.cy);
	if( m_bUseImageThread )
	{
		m_imageReadThread.InitReadThread();		
		LinkImageThread();	
		m_imageReadThread.BeginReadThread();
		m_bEnableBGLoad = FALSE;
	}
	m_bImgInitOK = TRUE;
	OnChangeCoordSys(FALSE);
	return TRUE;	
}

void CMutiRasterLayer::SetTransColor(LPCTSTR fileName,COLORREF color)
{
	int i = -1;
	for (i=m_arrImgPars.GetSize()-1;i>=0;i--)
	{
		if (_tcscmp(fileName,m_arrImgPars[i]->m_strImgName)==0)
		{
			break;
		}
	}
	if (i>=0)
	{
		m_arrImgPars[i]->m_transColor = color;	
	}
}

BOOL CMutiRasterLayer::AddImage(LPCTSTR fileName)
{
	if( !m_imageReadThread.AttachImage(fileName, TRUE) )
		return FALSE;

	ImgParaItem *pitem = new ImgParaItem;
	_tcscpy(pitem->m_strImgName ,fileName);	
	pitem->m_szImage = m_imageReadThread.GetImageOriginalSize(fileName);

	m_arrImgPars.Add(pitem);
	ImgSkipInfo *pInfo = new ImgSkipInfo;
	m_arrSkipInfos.Add(pInfo);
	if (m_bImgInitOK==TRUE)
	{
		m_bImgInitOK = FALSE;
	}
	return TRUE;
}

BOOL CMutiRasterLayer::RemoveImgAt(int idx)
{
	m_imageReadThread.Detach(m_arrImgPars[idx]->m_strImgName);
	ImgParaItem *pitem = m_arrImgPars.GetAt(idx);
	ImgSkipInfo *pInfo = m_arrSkipInfos.GetAt(idx);
	m_arrImgPars.RemoveAt(idx);
	m_arrSkipInfos.RemoveAt(idx);
	if (pitem) delete pitem;
	if (pInfo) delete pInfo;
	
	if (m_bImgInitOK==TRUE)
	{
		m_bImgInitOK = FALSE;
	}
	return TRUE;
}

BOOL CMutiRasterLayer::InsertImgAt(int idx, LPCTSTR fileName)
{
	if( !m_imageReadThread.InsertImage(idx, fileName) )
		return FALSE;
	
	ImgParaItem *pitem = new ImgParaItem;
	_tcscpy(pitem->m_strImgName, fileName);	
	pitem->m_szImage = m_imageReadThread.GetImageOriginalSize(fileName);
	
	m_arrImgPars.InsertAt(idx, pitem);
	ImgSkipInfo *pInfo = new ImgSkipInfo;
	m_arrSkipInfos.InsertAt(idx, pInfo);
	if (m_bImgInitOK==TRUE)
	{
		m_bImgInitOK = FALSE;
	}	
	return TRUE;
}

BOOL CMutiRasterLayer::RemoveAllImgs()
{	
	UnlinkImageThread();
	ClearAll();
	m_imageReadThread.Close();
	m_qThreadReadItem.blkidx = -1;
	m_qThreadReadItem.id = CacheID();
	for (int i=0;i<m_arrImgPars.GetSize();i++)
	{
		delete m_arrSkipInfos[i];
		delete m_arrImgPars[i];
	}
	m_arrSkipInfos.RemoveAll();
	m_arrImgPars.RemoveAll();
	if (m_bImgInitOK==TRUE)
	{
		m_bImgInitOK = FALSE;
	}
	return TRUE;
}

int CMutiRasterLayer::GetImgNum()
{
	return m_arrImgPars.GetSize();
}

BOOL CMutiRasterLayer::InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK)
{
	if (IsValid())  Destroy();
	if( nCacheType==CHEDATA_TYPE_BMP )
	{
		CCacheBmpData *pCache = new CCacheBmpData;
		if( pCache!=NULL )
		{
			pCache->SetColorTable(24,NULL);
			pCache->EnableTransparentOverlap(TRUE);
			
			m_pCacheData = pCache;
		}
	}
	return CDrawingLayer::InitCache(nCacheType,szBlock,nBlock,clrBK);

}



// BOOL CMutiRasterLayer::IsValidImage()
// {
// 	return TRUE;
// }

LPCTSTR CMutiRasterLayer::GetImgFileName(int idx)
{
	return m_arrImgPars[idx]->m_strImgName;
}

CSize CMutiRasterLayer::GetImageOriginalSize(int idx)
{
	return m_arrImgPars[idx]->m_szImage;
//	return m_imageReadThread.GetImageOriginalSize(m_arrImgPars[idx]->m_strImgName);
}

CSize CMutiRasterLayer::GetImageOriginalSize(LPCTSTR fileName)
{
	for (int i=m_arrImgPars.GetSize()-1;i>=0;i--)
	{
		if (_tcscmp(fileName,m_arrImgPars[i]->m_strImgName)==0)
		{
			return m_arrImgPars[i]->m_szImage;
		}
	}
	return CSize(-1,-1);
//	return m_imageReadThread.GetImageOriginalSize(fileName);
}

CSize CMutiRasterLayer::GetImageSize(int idx)
{
	return m_imageReadThread.GetImageSize(m_arrImgPars[idx]->m_strImgName);
}

CSize CMutiRasterLayer::GetImageSize(LPCTSTR fileName)
{
	return m_imageReadThread.GetImageSize(fileName);
}


Envelope CMutiRasterLayer::GetDataBound()
{
	return m_dataBound;
}

// int CMutiRasterLayer::FindIdxInArray(const CArray<CacheID,CacheID>& arr,const CacheID &id)
// {
// 	for (int i=arr.GetSize()-1;i>=0;i--)
// 	{
// 		if (arr[i]==id)
// 		{
// 			return i;
// 		}
// 	}
// 	return -1;
// }


void CMutiRasterLayer::GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum)
{
	CalcClientBound();
	
	xnum = floor((ptFocus.x-m_DataClientBound.m_xl)/m_szBlock.cx);
	ynum = floor((ptFocus.y-m_DataClientBound.m_yl)/m_szBlock.cy);
}

void CMutiRasterLayer::CalcClientBound()
{
	if( !m_bCalcClientBound )
	{
		//获得数据范围在客户坐标系下的范围
		CCoordSys *pCS = m_pContext->GetCoordSys();
		
		m_DataClientBound = GetDataBound();
		m_DataClientBound.TransformGrdToClt(pCS,1);
		
		//取整
		m_DataClientBound.m_xl = floor(m_DataClientBound.m_xl);
		m_DataClientBound.m_yl = floor(m_DataClientBound.m_yl);
		m_DataClientBound.m_xh = ceil(m_DataClientBound.m_xh);
		m_DataClientBound.m_yh = ceil(m_DataClientBound.m_yh);
		
		m_bCalcClientBound = TRUE;
	}

}

// 获得块编号的合理的取值范围
void CMutiRasterLayer::GetBlockNumRange( int &xmin, int &xmax, int &ymin, int &ymax)
{
	CalcClientBound();
	xmin = 0;ymin =0;	
	xmax = ceil(m_DataClientBound.Width()/m_szBlock.cx)-1;
	ymax = ceil(m_DataClientBound.Height()/m_szBlock.cy)-1;
}


// 获得块的四个角点坐标在客户坐标系下的XY坐标
void CMutiRasterLayer::GetBlockClientXY(CacheID id, double x[4], double y[4])
{
	CalcClientBound();
	
	x[0] = m_DataClientBound.m_xl + id.xnum*m_szBlock.cx; 
	y[0] = m_DataClientBound.m_yl + id.ynum*m_szBlock.cy;
	x[1] = m_DataClientBound.m_xl + (id.xnum+1)*m_szBlock.cx; 
	y[1] = m_DataClientBound.m_yl + id.ynum*m_szBlock.cy;
	x[2] = m_DataClientBound.m_xl + (id.xnum+1)*m_szBlock.cx; 
	y[2] = m_DataClientBound.m_yl + (id.ynum+1)*m_szBlock.cy;
	x[3] = m_DataClientBound.m_xl + id.xnum*m_szBlock.cx; 
	y[3] = m_DataClientBound.m_yl + (id.ynum+1)*m_szBlock.cy;
}

// int CMutiRasterLayer::FileNameToIndex(LPCTSTR fileName)
// {
// 	for (int i=m_arrImgPars.GetSize()-1;i>=0;i--)
// 	{
// 		if (_tcscmp(fileName,m_arrImgPars[i]->m_strImgName)==0)
// 		{
// 			return i;
// 		}
// 	}
// 	return -1;
// }

// BOOL FindStrInArray(LPCTSTR str,const CStringArray& arrStr)
// {
// 	for (int i=arrStr.GetSize()-1;i>=0;i--)
// 	{
// 		if (_tcscmp(str,(LPCTSTR)arrStr[i])==0)
// 		{
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }
// 
// BOOL FindIntInArray(int value,const CUIntArray& arrInt)
// {
// 	for (int i=arrInt.GetSize()-1;i>=0;i--)
// 	{
// 		if (value==arrInt[i])
// 		{
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }


// void CMutiRasterLayer::UpdateCache(const CUIntArray& arr)
// {
// 	for( int i=0;i<m_cacheTable.nsum;i++ )
// 	{
// 		if(FindIntInArray(m_cacheTable.ids[i].idx,arr))
// 		{
// 			m_cacheTable.ids[i].bvalid  = CacheID::stateInvalid;
// 			if( m_pCacheData )
// 			{
// 				m_pCacheData->ClearBlock(i);
// 			}
// 		}
// 	}
// 	m_aReadQueue.RemoveAll();	
// 	
// }

void CMutiRasterLayer::ClearCHEIDImgs()
{
    for (int i=m_arrCHEIDImgs.GetSize()-1;i>=0;i--)
    {
		if(m_arrCHEIDImgs[i].nsum>0)delete []m_arrCHEIDImgs[i].p;
    }
	m_arrCHEIDImgs.RemoveAll();
}

void CMutiRasterLayer::SetZoomRate(float zoomRate)
{
	m_imageReadThread.ClearReadImgInfo();
	m_qThreadReadItem.blkidx = -1;
	m_qThreadReadItem.id = CacheID();

//	CUIntArray arrInt; 
	float zoomRate0 = zoomRate;
	for (int i = 0;i<m_arrImgPars.GetSize();i++)
	{
		zoomRate0 = zoomRate*min(m_arrImgPars[i]->m_fKX,m_arrImgPars[i]->m_fKY);
		m_imageReadThread.SetImgZoomRate(i,zoomRate0);
		m_arrSkipInfos[i]->bSkipRead = FALSE;
		m_arrSkipInfos[i]->nSkipReadRate = 0;

		BOOL bSkipRead = m_arrSkipInfos[i]->bSkipRead;
		int nNewRate = m_arrSkipInfos[i]->nSkipReadRate;

		int zmin;
		m_imageReadThread.GetParamidRange(i,&zmin,NULL);		
		float fzmin = 2.0f/(-zmin), fzmax = 1.0f;
		if( zoomRate0>=fzmax )
		{
			bSkipRead = FALSE;
			nNewRate = 0;
		}
		else
		{
			float fk = zoomRate0/fzmin;
			
			//[1,0.25) ->采用1比率读取; 
			if( fk>0.5 )
			{
				bSkipRead = FALSE;
				nNewRate = 0;
			}
			
			//[1/4,1/16) ->采用1/4比率读取; 
			//[1/16,1/64) ->采用1/16比率读取;依次类推
			else
			{
				int nk = 0.01 + 1.0/fk;
				
				bSkipRead = TRUE;
				for( int n=1; n<32 && (1<<n)<=nk; n++);
				nNewRate = (int)(1<<(n-1));
			}			
		}
		if( bSkipRead!=m_arrSkipInfos[i]->bSkipRead || nNewRate!=m_arrSkipInfos[i]->nSkipReadRate )
		{		
			m_arrSkipInfos[i]->bSkipRead = bSkipRead;
			m_arrSkipInfos[i]->nSkipReadRate = nNewRate;
		}

	}	
	
	ClearAll();
}



void CMutiRasterLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	m_bCalcClientBound = FALSE;
	CalcClientBound();
	if( !bJustScrollScreen )
	{
		m_imageReadThread.SetRateChangeFlag(TRUE);
		if(m_bUseImageThread)
			EnterCriticalSection(&g_section);
		float zoomRate = m_pContext->GetCoordSys()->CalcScale();		
		SetZoomRate(zoomRate);
		ClearCHEIDImgs();
		m_imageReadThread.ClearAllReadBmpInfo();		
		ClearAll();
		if(m_bUseImageThread)
			LeaveCriticalSection(&g_section);
		m_imageReadThread.SetRateChangeFlag(FALSE);
	}

}


void CMutiRasterLayer::SetCornerPts(LPCTSTR fileName,BOOL bUseCorner, PT_3D* pts)
{
	int i = -1;
	for (i=m_arrImgPars.GetSize()-1;i>=0;i--)
	{
		if (_tcscmp(fileName,m_arrImgPars[i]->m_strImgName)==0)
		{
			break;
		}
	}
	if (i>=0)
	{
		m_arrImgPars[i]->m_bUseCornerPts = bUseCorner;
		m_arrImgPars[i]->m_bCalcParam = FALSE;
		if( bUseCorner )
		{
			memcpy(m_arrImgPars[i]->m_ptsCorner,pts,sizeof(PT_3D)*4);
			
			double dis1 = DIST_3DPT(pts[0],pts[1]);
			double dis2 = DIST_3DPT(pts[1],pts[2]);
			
			CSize szImage = m_imageReadThread.GetImageOriginalSize(fileName);
			
			m_arrImgPars[i]->m_fKX = dis1/szImage.cx;
			m_arrImgPars[i]->m_fKY = dis2/szImage.cy;
			
//			OnChangeCoordSys(FALSE);
		}		
	}
}

void CMutiRasterLayer::SetCornerPts(LPCTSTR fileName,float ang, float fKX, float fKY)
{
	int i = -1;
	for (i=m_arrImgPars.GetSize()-1;i>=0;i--)
	{
		if (_tcscmp(fileName,m_arrImgPars[i]->m_strImgName)==0)
		{
			break;
		}
	}
	if (i>=0)
	{
		m_arrImgPars[i]->m_bUseCornerPts = TRUE;
		m_arrImgPars[i]->m_bCalcParam = FALSE;		
		m_arrImgPars[i]->m_fRotAng = ang;

		CSize szImage = m_imageReadThread.GetImageOriginalSize(fileName);
// 		if( m_pCacheData->GetType()==CHEDATA_TYPE_BMP )
// 		{
// 			((CCacheBmpData*)m_pCacheData)->SetRotateAndZoom(ang,1,1);
// 		}
		
		{
			double x = szImage.cx, y = szImage.cy;
			
			//顺时针旋转
			if( ang==90 )
			{
				m_arrImgPars[i]->m_ptsCorner[0].x = 0; m_arrImgPars[i]->m_ptsCorner[0].y = x; m_arrImgPars[i]->m_ptsCorner[0].z = 0;
				m_arrImgPars[i]->m_ptsCorner[1].x = 0; m_arrImgPars[i]->m_ptsCorner[1].y = 0; m_arrImgPars[i]->m_ptsCorner[1].z = 0;
				m_arrImgPars[i]->m_ptsCorner[2].x = y; m_arrImgPars[i]->m_ptsCorner[2].y = 0; m_arrImgPars[i]->m_ptsCorner[2].z = 0;
				m_arrImgPars[i]->m_ptsCorner[3].x = y; m_arrImgPars[i]->m_ptsCorner[3].y = x; m_arrImgPars[i]->m_ptsCorner[3].z = 0;
			}
			else if( ang==180 )
			{
				m_arrImgPars[i]->m_ptsCorner[0].x = x; m_arrImgPars[i]->m_ptsCorner[0].y = y; m_arrImgPars[i]->m_ptsCorner[0].z = 0;
				m_arrImgPars[i]->m_ptsCorner[1].x = 0; m_arrImgPars[i]->m_ptsCorner[1].y = y; m_arrImgPars[i]->m_ptsCorner[1].z = 0;
				m_arrImgPars[i]->m_ptsCorner[2].x = 0; m_arrImgPars[i]->m_ptsCorner[2].y = 0; m_arrImgPars[i]->m_ptsCorner[2].z = 0;
				m_arrImgPars[i]->m_ptsCorner[3].x = x; m_arrImgPars[i]->m_ptsCorner[3].y = 0; m_arrImgPars[i]->m_ptsCorner[3].z = 0;
			}
			//垂直翻转
			else if( ang==-180 )
			{
				m_arrImgPars[i]->m_ptsCorner[0].x = 0; m_arrImgPars[i]->m_ptsCorner[0].y = y; m_arrImgPars[i]->m_ptsCorner[0].z = 0;
				m_arrImgPars[i]->m_ptsCorner[1].x = x; m_arrImgPars[i]->m_ptsCorner[1].y = y; m_arrImgPars[i]->m_ptsCorner[1].z = 0;
				m_arrImgPars[i]->m_ptsCorner[2].x = x; m_arrImgPars[i]->m_ptsCorner[2].y = 0; m_arrImgPars[i]->m_ptsCorner[2].z = 0;
				m_arrImgPars[i]->m_ptsCorner[3].x = 0; m_arrImgPars[i]->m_ptsCorner[3].y = 0; m_arrImgPars[i]->m_ptsCorner[3].z = 0;
			}
			else if( ang==270 )
			{
				m_arrImgPars[i]->m_ptsCorner[0].x = y; m_arrImgPars[i]->m_ptsCorner[0].y = 0; m_arrImgPars[i]->m_ptsCorner[0].z = 0;
				m_arrImgPars[i]->m_ptsCorner[1].x = y; m_arrImgPars[i]->m_ptsCorner[1].y = x; m_arrImgPars[i]->m_ptsCorner[1].z = 0;
				m_arrImgPars[i]->m_ptsCorner[2].x = 0; m_arrImgPars[i]->m_ptsCorner[2].y = x; m_arrImgPars[i]->m_ptsCorner[2].z = 0;
				m_arrImgPars[i]->m_ptsCorner[3].x = 0; m_arrImgPars[i]->m_ptsCorner[3].y = 0; m_arrImgPars[i]->m_ptsCorner[3].z = 0;
			}
			else
			{
				m_arrImgPars[i]->m_ptsCorner[0].x = 0; m_arrImgPars[i]->m_ptsCorner[0].y = 0; m_arrImgPars[i]->m_ptsCorner[0].z = 0;
				m_arrImgPars[i]->m_ptsCorner[1].x = x; m_arrImgPars[i]->m_ptsCorner[1].y = 0; m_arrImgPars[i]->m_ptsCorner[1].z = 0;
				m_arrImgPars[i]->m_ptsCorner[2].x = x; m_arrImgPars[i]->m_ptsCorner[2].y = y; m_arrImgPars[i]->m_ptsCorner[2].z = 0;
				m_arrImgPars[i]->m_ptsCorner[3].x = 0; m_arrImgPars[i]->m_ptsCorner[3].y = y; m_arrImgPars[i]->m_ptsCorner[3].z = 0;
			}
			
			for( int j=0; j<4; j++)
			{
				m_arrImgPars[i]->m_ptsCorner[j].x *= fKX;
				m_arrImgPars[i]->m_ptsCorner[j].y *= fKY;
			}
		}
		
		m_arrImgPars[i]->m_fKX = fKX;
		m_arrImgPars[i]->m_fKY = fKY;
	//	OnChangeCoordSys(FALSE);
	}

}


void CMutiRasterLayer::CalcAffineParm(int idx)
{
	//计算仿射关系
	double x[4], y[4], x2[4], y2[4];

	x[0] = m_arrImgPars[idx]->m_ptsCorner[0].x; y[0] = m_arrImgPars[idx]->m_ptsCorner[0].y; 
	x[1] = m_arrImgPars[idx]->m_ptsCorner[1].x; y[1] = m_arrImgPars[idx]->m_ptsCorner[1].y; 
	x[2] = m_arrImgPars[idx]->m_ptsCorner[2].x; y[2] = m_arrImgPars[idx]->m_ptsCorner[2].y; 
	x[3] = m_arrImgPars[idx]->m_ptsCorner[3].x; y[3] = m_arrImgPars[idx]->m_ptsCorner[3].y; 

	x2[0] = 0; y2[0] = 0; 
	x2[1] = m_arrImgPars[idx]->m_szImage.cx; y2[1] = 0; 
	x2[2] = m_arrImgPars[idx]->m_szImage.cx; y2[2] = m_arrImgPars[idx]->m_szImage.cy; 
	x2[3] = 0; y2[3] = m_arrImgPars[idx]->m_szImage.cy; 

	CalcAffineParams(x,y,x2,y2,4,m_arrImgPars[idx]->m_lfA,m_arrImgPars[idx]->m_lfB);

	m_arrImgPars[idx]->m_bCalcParam = TRUE;
}


void CMutiRasterLayer::GroundToImage(int idx, PT_4D *pts, int num)
{	
// 	if( m_pFuncConvertCoord )
// 	{
// 		PT_4D pt;
// 		for( int i=0; i<num; i++)
// 		{
// 			pt = pts[i];
// 			(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x,pt.y,pts[i].x,pts[i].y,FALSE);
// 		}
// 		return;
// 	}
	if (idx<0||idx>=m_arrImgPars.GetSize())
	{
		return;
	}
	if( m_arrImgPars[idx]->m_bUseCornerPts )
	{
		if( !m_arrImgPars[idx]->m_bCalcParam )
			CalcAffineParm(idx);

		PT_4D pt;
		for( int i=0; i<num; i++)
		{
			pt = pts[i];
			pts[i].x = pt.x*m_arrImgPars[idx]->m_lfA[0] + pt.y*m_arrImgPars[idx]->m_lfA[1] + m_arrImgPars[idx]->m_lfA[2];
			pts[i].y = pt.x*m_arrImgPars[idx]->m_lfB[0] + pt.y*m_arrImgPars[idx]->m_lfB[1] + m_arrImgPars[idx]->m_lfB[2];
		}
	}
	else
	{
	}

	float zoom = GetActualZoomRate(idx);
	
	for( int i=0; i<num; i++)
	{
		pts[i].x = pts[i].x*zoom;
		pts[i].y = pts[i].y*zoom;
	}
}


void CMutiRasterLayer::ImageToGround(int idx, PT_4D *pts, int num)
{
	float zoom = GetActualZoomRate(idx);
	for( int i=0; i<num; i++)
	{
		pts[i].x = pts[i].x/zoom;
		pts[i].y = pts[i].y/zoom;
	}

// 	if( m_pFuncConvertCoord )
// 	{
// 		PT_4D pt;
// 		for( int i=0; i<num; i++)
// 		{
// 			pt = pts[i];
// 			(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x,pt.y,pts[i].x,pts[i].y,TRUE);
// 		}
// 		return;
// 	}

	if( m_arrImgPars[idx]->m_bUseCornerPts )
	{
		if( !m_arrImgPars[idx]->m_bCalcParam )
			CalcAffineParm(idx);

		double m[9] = {
			m_arrImgPars[idx]->m_lfA[0],m_arrImgPars[idx]->m_lfA[1],m_arrImgPars[idx]->m_lfA[2],
			m_arrImgPars[idx]->m_lfB[0],m_arrImgPars[idx]->m_lfB[1],m_arrImgPars[idx]->m_lfB[2],
			0,0,1
		};

		double m2[9];
		matrix_reverse(m,3,m2);
		
		PT_4D pt;
		for( i=0; i<num; i++)
		{
			pt = pts[i];
			pts[i].x = pt.x*m2[0] + pt.y*m2[1] + m2[2];
			pts[i].y = pt.x*m2[3] + pt.y*m2[4] + m2[5];
		}
	}
	else
	{
	}
}


void CMutiRasterLayer::DrawClient(double xl, double xh, double yl, double yh)
{
// 	PT_4D pts[4];
// 
// 	//计算与客户坐标相对应的影像区域
// 	memset(pts,0,sizeof(pts));
// 	pts[0].x = xl, pts[0].y = yl, pts[0].z = 0;
// 	pts[1].x = xh, pts[1].y = yl, pts[1].z = 0;
// 	pts[2].x = xh, pts[2].y = yh, pts[2].z = 0;
// 	pts[3].x = xl, pts[3].y = yh, pts[3].z = 0;
// 
// 	m_pContext->GetCoordSys()->ClientToGround(pts,4);
// 	GroundToImage(pts,4);
// 
// 	Envelope e;
// 	e.CreateFromPts(pts,4,sizeof(PT_4D));
// 
// 	CRect rcImage(floor(e.m_xl),floor(e.m_yl),ceil(e.m_xh),ceil(e.m_yh));
// 	CRect rcClient(floor(xl),floor(yl),ceil(xh),ceil(yh));
// 	
// 	//设置可视区域
// 	CRect rcViewSave;
// 	m_pContext->GetViewRect(rcViewSave);
// 	m_pContext->SetViewRect(rcClient);
// 
// 	//绘图
// 	double x[4] = {xl,xh,xh,xl};
// 	double y[4] = {yl,yl,yh,yh};
// 	HBITMAP hBmp = Create24BitDIB(rcImage.Width(),rcImage.Height());
// 	if( !hBmp )return;
// 	if( m_imageReadThread.ReadRectBmp(rcImage,hBmp) )
// 	{
// 		m_pContext->DrawBitmap(hBmp,CRect(0,0,rcImage.Width(),rcImage.Height()),x,y);
// 	}
// 	
// 	::DeleteObject(hBmp);
// 
// 	//恢复可视区域
// 	m_pContext->SetViewRect(rcViewSave);
}


void CMutiRasterLayer::LinkImageThread()
{
	UnlinkImageThread();
	m_pDummyDialog = new CDummyDialog(this, (PFUNCALLBACK)&CMutiRasterLayer::OnThreadMsg);
	((CDummyDialog*)m_pDummyDialog)->Create2((HINSTANCE)SmartViewDLL.hModule,IDD_DUMMY);
	m_pDummyDialog->ShowWindow(SW_HIDE);
	m_imageReadThread.SetThreadCallback(m_pDummyDialog->GetSafeHwnd(),WM_THREADMSG);
}


void CMutiRasterLayer::UnlinkImageThread()
{
	m_imageReadThread.SetThreadCallback(NULL,NULL);
	if( m_pDummyDialog )
	{
		m_pDummyDialog->DestroyWindow();
		delete m_pDummyDialog;
	}
	m_pDummyDialog = NULL;
}


float CMutiRasterLayer::GetActualZoomRate(int idx)
{
	if( m_arrSkipInfos[idx]->bSkipRead )
		return (m_imageReadThread.GetCurReadZoom(idx)/m_arrSkipInfos[idx]->nSkipReadRate);
 	return m_imageReadThread.GetCurReadZoom(idx);
}

//以后可以再优化速度此处
void CMutiRasterLayer::GetBlockImgInfo(CacheID id,CArray<ReqImgInfo,ReqImgInfo> &arrImgInfo)
{
	for (int i=m_arrCHEIDImgs.GetSize()-1;i>=0;i-- )
	{
		if (m_arrCHEIDImgs[i].id.CompareIncLoading(id))
		{
			arrImgInfo.SetSize(m_arrCHEIDImgs[i].nsum);
			memcpy(arrImgInfo.GetData(),m_arrCHEIDImgs[i].p,sizeof(ReqImgInfo)*m_arrCHEIDImgs[i].nsum);
			return;
		}
	}
	Envelope e1, e2, e3;
	double x[4], y[4];
	PT_4D pts[4],pts0[4];	
	GetBlockClientXY(id,x,y);
	pts[0].x = x[0]; pts[0].y = y[0]; pts[0].z = 0;
	pts[1].x = x[1]; pts[1].y = y[1]; pts[1].z = 0;
	pts[2].x = x[2]; pts[2].y = y[2]; pts[2].z = 0;
	pts[3].x = x[3]; pts[3].y = y[3]; pts[3].z = 0;
	CHEIDImg item0;
	m_pContext->GetCoordSys()->ClientToGround(pts,4);
	e1.CreateFromPts(pts,4,sizeof(PT_4D));
	for (i=m_arrImgPars.GetSize()-1;i>=0;i--)
	{		
		e2.CreateFromPts(m_arrImgPars[i]->m_ptsCorner,4);
		if(!e1.bIntersect(&e2))
			continue;
		memcpy(pts0,pts,sizeof(pts));
		GroundToImage(i,pts0,4);
		e2.CreateFromPts(pts0,4,sizeof(PT_4D));
		COPY_3DPT(pts0[0],m_arrImgPars[i]->m_ptsCorner[0]);
		COPY_3DPT(pts0[1],m_arrImgPars[i]->m_ptsCorner[1]);
		COPY_3DPT(pts0[2],m_arrImgPars[i]->m_ptsCorner[2]);
		COPY_3DPT(pts0[3],m_arrImgPars[i]->m_ptsCorner[3]);
		GroundToImage(i,pts0,4);
		e3.CreateFromPts(pts0,4,sizeof(PT_4D));
		e2.Intersect(&e3);
	
		if (!e2.IsEmpty())
		{
			ReqImgInfo item;
			item.idx = i;
			item.e = e2;//top和bottom哪个小
			arrImgInfo.Add(item);
		}		
	}
	if (arrImgInfo.GetSize()>0)
	{
		item0.id = id;
		item0.nsum = arrImgInfo.GetSize();
		item0.p = new ReqImgInfo[item0.nsum];
		memcpy(item0.p,arrImgInfo.GetData(),sizeof(ReqImgInfo)*item0.nsum);
		m_arrCHEIDImgs.Add(item0);
	}
}


LRESULT CMutiRasterLayer::OnThreadMsg(WPARAM wParam, LPARAM lParam)
{
	
//	CRoatableImageReadEx *p = (CRoatableImageReadEx *)lParam;
	if(!m_imageReadThread.IsReadThreadStarted() )return 1;
	if( !m_bUseImageThread )return 1;
	
	// the thread finished reading a block
	if( wParam==1 )
	{
		if( m_qThreadReadItem.blkidx>=0 && m_qThreadReadItem.blkidx<m_cacheTable.nsum )
		{
			if( m_qThreadReadItem.id.bvalid==2 )m_qThreadReadItem.id.bvalid = 1;
			if( GetBlockIdx(m_qThreadReadItem.id)<0 )
			{
				TRACE("OnReadFinish %d\n",m_qThreadReadItem.blkidx);

				FillBlockWithImage(m_qThreadReadItem.blkidx,m_qThreadReadItem.id);

			}
			else
			{
				m_imageReadThread.ClearAllReadBmpInfo();
			}
		}
	}
	// the thread has no block to read
	else
	{
	}
// 	EndTick("FillBlockWithImage")
// 	BeginTick
	ClearQueue();

	m_bEnableBGLoad = TRUE;
	PlanBGLoad();
	m_bEnableBGLoad = FALSE;

	if( m_aReadQueue.GetSize()>0 )
	{
		ReadQueue q = m_aReadQueue.GetAt(m_aReadQueue.GetSize()-1);
		if( q.id.bvalid!=0 && q.blkidx>=0 && q.blkidx<m_cacheTable.nsum )
		{
			CArray<ReqImgInfo,ReqImgInfo> arr;
			CArray<int,int> arrIdx;
			CArray<CRect,CRect> arrOldRect;
			CArray<CRect,CRect> arrNewRect;

			GetBlockImgInfo(q.id,arr);
			for (int i=0;i<arr.GetSize();i++)
			{
				CRect rc(FixPreciError(arr[i].e.m_xl),FixPreciError(arr[i].e.m_yl),FixPreciError(arr[i].e.m_xh),FixPreciError(arr[i].e.m_yh));
				arrOldRect.Add(rc);
				if (rc.Width()>1024||rc.Height()>1024)
				{
					int i=0;
				}
				if( m_arrSkipInfos[arr[i].idx]->bSkipRead )
				{
					rc.left *= m_arrSkipInfos[arr[i].idx]->nSkipReadRate;  rc.right *= m_arrSkipInfos[arr[i].idx]->nSkipReadRate;
					rc.top *= m_arrSkipInfos[arr[i].idx]->nSkipReadRate;  rc.bottom *= m_arrSkipInfos[arr[i].idx]->nSkipReadRate;
				}
				if (rc.Width()>1024||rc.Height()>1024)
				{
					int i=0;
				}
				arrIdx.Add(arr[i].idx);
				arrNewRect.Add(rc);
			}


			m_imageReadThread.SetReadImgInfo(arrIdx,arrOldRect,arrNewRect);
			m_qThreadReadItem = q;
		}
	}
	else
	{
		m_imageReadThread.ClearReadImgInfo();
		m_qThreadReadItem.blkidx = -1;
		m_qThreadReadItem.id = CacheID();
	}

	return 0;
}



void CMutiRasterLayer::ClearImageAdjust()
{
	m_Adjust = CImageAdjust();
}


void CMutiRasterLayer::CalcImageAdjust(HBITMAP hBmp)
{
	if( !m_Adjust.bAdjustMap )
	{
		m_Adjust.StatHistogramInfo(hBmp);
		m_Adjust.CalcAdjustMap();
	}
}

void CMutiRasterLayer::Destroy()
{
	UnlinkImageThread();
	m_imageReadThread.DestroyReadThread();
	if( m_pDummyDialog )
	{
		delete m_pDummyDialog;
		m_pDummyDialog = NULL;
	}
	
	if( m_hBlockBmp!=NULL )
	{
		::DeleteObject(m_hBlockBmp);
		m_hBlockBmp = NULL;
	}
// 	for (int i=0;i<m_arrImgPars.GetSize();i++)
// 	{
// 		delete m_arrSkipInfos[i];
// 		delete m_arrImgPars[i];
// 	}
// 	m_arrSkipInfos.RemoveAll();
// 	m_arrImgPars.RemoveAll();
	CDrawingLayer::Destroy();
}

BOOL CMutiRasterLayer::FillBlock(int blkIdx, CacheID id)
{
	if( m_pContext==NULL || m_pCacheData==NULL )
		return FALSE;
	
	CArray<ReqImgInfo,ReqImgInfo> arrImgInfo;
	GetBlockImgInfo(id,arrImgInfo);
	
	//未使用xy坐标值
	double x[4] = {0,0,0,0};
	double y[4] = {0,0,0,0};
	PT_4D pts[4];
	Envelope e;
	if( arrImgInfo.GetSize()>0 )
	{	
		
		GetBlockClientXY(id,x,y);
		pts[0].x = x[0];pts[0].y = y[0];pts[0].z = 0;
		pts[1].x = x[1];pts[1].y = y[1];pts[1].z = 0;
		pts[2].x = x[2];pts[2].y = y[2];pts[2].z = 0;
		pts[3].x = x[3];pts[3].y = y[3];pts[3].z = 0;
		e.CreateFromPts(pts,4,sizeof(PT_4D));
		
		m_pContext->BeginDrawTempBmp(m_hBlockBmp,e);
		
		for (int i = arrImgInfo.GetSize()-1;i>=0;i--)
		{	
		
			CRect rc(FixPreciError(arrImgInfo[i].e.m_xl),FixPreciError(arrImgInfo[i].e.m_yl),FixPreciError(arrImgInfo[i].e.m_xh),FixPreciError(arrImgInfo[i].e.m_yh));
			HBITMAP hbitmap = Create24BitDIB(rc.Width(),rc.Height());
			if(!m_imageReadThread.ReadRectBmp(arrImgInfo[i].idx,rc,hbitmap,m_arrSkipInfos[arrImgInfo[i].idx]->bSkipRead,m_arrSkipInfos[arrImgInfo[i].idx]->nSkipReadRate))
			{
				::DeleteObject(hbitmap);
				continue;
			}		
			
			pts[0].x =arrImgInfo[i].e.m_xl;pts[0].y = arrImgInfo[i].e.m_yl;pts[0].z = 0;
			pts[1].x =arrImgInfo[i].e.m_xh;pts[1].y = arrImgInfo[i].e.m_yl;pts[1].z = 0;
			pts[2].x =arrImgInfo[i].e.m_xh;pts[2].y = arrImgInfo[i].e.m_yh;pts[2].z = 0;
			pts[3].x =arrImgInfo[i].e.m_xl;pts[3].y = arrImgInfo[i].e.m_yh;pts[3].z = 0;
			ImageToGround(arrImgInfo[i].idx,pts,4);
			m_pContext->GetCoordSys()->GroundToClient(pts,4);
			x[0] = pts[0].x;y[0] = e.m_yl+e.m_yh - pts[0].y;
			x[1] = pts[1].x;y[1] = e.m_yl+e.m_yh - pts[1].y;
			x[2] = pts[2].x;y[2] = e.m_yl+e.m_yh - pts[2].y;
			x[3] = pts[3].x;y[3] = e.m_yl+e.m_yh - pts[3].y;
			COLORREF transcolor = m_arrImgPars[arrImgInfo[i].idx]->m_transColor;
			m_pContext->DrawTempBmp(hbitmap,CRect(0,0,0,0),x,y,TRUE,transcolor);
			::DeleteObject(hbitmap);
		}
		
		m_pContext->EndDrawTempBmp(e.m_xl,e.m_xh,e.m_yl,e.m_yh);
		
	
//		m_imageReadThread.ClearAllReadBmpInfo();
		
		CalcImageAdjust(m_hBlockBmp);
		AdjustBitmap(m_hBlockBmp,CRect(0,0,m_szBlock.cx,m_szBlock.cy),&m_Adjust,
			m_pContext->GetBackColor(),m_pContext->GetRGBMask());
		
		EnterCriticalSection(&m_critical);
		m_pCacheData->BeginFillBlock(m_pContext,blkIdx);
		
		m_pCacheData->DrawBitmap(m_hBlockBmp,CRect(0,0,m_szBlock.cx,m_szBlock.cy),CRect(0,0,m_szBlock.cx,m_szBlock.cy));
		m_pCacheData->EndFillBlock();
		LeaveCriticalSection(&m_critical);
	}
	else return FALSE;
	
	return CDrawingLayer::FillBlock(blkIdx,id);
}

// 填充块的内容
BOOL CMutiRasterLayer::FillBlockWithImage(int blkIdx, CacheID id)
{
	if( m_pContext==NULL || m_pCacheData==NULL )
		return FALSE;

	//获取影像的角点坐标
	
	//未使用xy坐标值
	double x[4] = {0,0,0,0};
	double y[4] = {0,0,0,0};
	PT_4D pts[4];
	Envelope e;
	CArray<HBITMAP,HBITMAP> arr;
	if( m_imageReadThread.GetReadBmp(arr) )
	{

		CArray<ReqImgInfo,ReqImgInfo> arrImgInfo;
		GetBlockImgInfo(id,arrImgInfo);
		if(arrImgInfo.GetSize()!=arr.GetSize())
			int i=0;
		GetBlockClientXY(id,x,y);
		pts[0].x = x[0];pts[0].y = y[0];pts[0].z = 0;
		pts[1].x = x[1];pts[1].y = y[1];pts[1].z = 0;
		pts[2].x = x[2];pts[2].y = y[2];pts[2].z = 0;
		pts[3].x = x[3];pts[3].y = y[3];pts[3].z = 0;
		e.CreateFromPts(pts,4,sizeof(PT_4D));
	
		m_pContext->BeginDrawTempBmp(m_hBlockBmp,e);
		for (int i = arr.GetSize()-1;i>=0;i--)
		{	
			
			pts[0].x =arrImgInfo[i].e.m_xl;pts[0].y = arrImgInfo[i].e.m_yl;pts[0].z = 0;
			pts[1].x =arrImgInfo[i].e.m_xh;pts[1].y = arrImgInfo[i].e.m_yl;pts[1].z = 0;
			pts[2].x =arrImgInfo[i].e.m_xh;pts[2].y = arrImgInfo[i].e.m_yh;pts[2].z = 0;
			pts[3].x =arrImgInfo[i].e.m_xl;pts[3].y = arrImgInfo[i].e.m_yh;pts[3].z = 0;
			ImageToGround(arrImgInfo[i].idx,pts,4);
			m_pContext->GetCoordSys()->GroundToClient(pts,4);
			x[0] = pts[0].x;y[0] = e.m_yl+e.m_yh - pts[0].y;
			x[1] = pts[1].x;y[1] = e.m_yl+e.m_yh - pts[1].y;
			x[2] = pts[2].x;y[2] = e.m_yl+e.m_yh - pts[2].y;
			x[3] = pts[3].x;y[3] = e.m_yl+e.m_yh - pts[3].y;
			COLORREF transcolor = m_arrImgPars[arrImgInfo[i].idx]->m_transColor;
			m_pContext->DrawTempBmp(arr[i],CRect(0,0,0,0),x,y,TRUE,transcolor);	
		}
	
		m_pContext->EndDrawTempBmp(e.m_xl,e.m_xh,e.m_yl,e.m_yh);
	
	
		m_imageReadThread.ClearAllReadBmpInfo();
		
		CalcImageAdjust(m_hBlockBmp);
		AdjustBitmap(m_hBlockBmp,CRect(0,0,m_szBlock.cx,m_szBlock.cy),&m_Adjust,
			m_pContext->GetBackColor(),m_pContext->GetRGBMask());

		EnterCriticalSection(&m_critical);
		m_pCacheData->BeginFillBlock(m_pContext,blkIdx);

		m_pCacheData->DrawBitmap(m_hBlockBmp,CRect(0,0,m_szBlock.cx,m_szBlock.cy),CRect(0,0,m_szBlock.cx,m_szBlock.cy));
		m_pCacheData->EndFillBlock();
		LeaveCriticalSection(&m_critical);
	}
	else return FALSE;
	
	return CDrawingLayer::FillBlock(blkIdx,id);
}


MyNameSpaceEnd