// MarkVectLayer.cpp: implementation of the CMarkVectLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkVectLayer.h"
#include "Matrix.h"
#include "SmartViewFunctions.h"
#include "GeoCurve.h"
#include "GeoText.h"

#include "Selection.h"
#include "DlgDataSource.h"
#include "Feature.h"
#include "GeoSurface.h"

IMPLEMENT_DYNAMIC(CMarkVectLayer,CDrawingLayer)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


static void ClearBuffers(GrElementList& list)
{
	GrElement *p = list.head;
	while(p!=NULL)
	{
		if(p->pBuf)
		{
			delete (GrBuffer*)(p->pBuf);
			p->pBuf = NULL;
		}
		p = p->next;
	}
}



CMarkVectLayer::CMarkVectLayer()
{
	m_bCalcClientBound = FALSE;
	m_pCacheData = NULL;

	m_bForceDraw = TRUE;

	m_fMarkWidth1 = 2; 
	m_fMarkWidth2 = 1;
	m_clrMark1 = RGB(255,255,255);
	m_clrMark2 = RGB(255,255,255);

	m_DataBound = Envelope(0,1000,0,1000);

	matrix_toIdentity(m_DataMatrix,4);
	matrix_toIdentity(m_DataRMatrix,4);

	m_pSel = NULL;
	m_pDS = NULL;
	m_pCS = NULL;
	m_dwAttribute |= CDrawingLayer::attrExtraPrepareCache;

}

CMarkVectLayer::~CMarkVectLayer()
{
	Destroy();
}

void CMarkVectLayer::Init(CSelection *pSel, CDlgDataSource *pDS, CCoordSys *pDrawContentCS, CCoordSys *pCS)
{
	m_pSel = pSel;
	m_pDS = pDS;
	m_pCS = pCS;
	m_pDrawContentCS = pDrawContentCS;
}

void CMarkVectLayer::Destroy()
{
	m_eleList1.delAll();
	m_eleList2.delAll();
	ClearBuffers(m_eleList_baseline);
	m_eleList_baseline.delAll();
	
	CDrawingLayer::Destroy();
	
	m_bCalcClientBound = FALSE;
	m_pCacheData = NULL;
}



BOOL CMarkVectLayer::InitCache(int nCacheType, CSize szBlock, int nBlock)
{
	if (IsValid())  Destroy();
	
	if( nCacheType==CHEDATA_TYPE_BMP )
	{
		CCacheBmpData *pCache = new CCacheBmpData;
		if( pCache!=NULL )
		{
			pCache->SetColorTable(8,NULL);
			pCache->EnableTransparentOverlap(TRUE);
			pCache->CreateProgDataEncoder();
			
			m_pCacheData = pCache;
		}
	}
	if( nCacheType==CHEDATA_TYPE_GLTEXT )
	{
		CCacheGLTextData *pCache = new CCacheGLTextData;
		if( pCache!=NULL )
		{
			pCache->m_bUseAlpha = TRUE;
			m_pCacheData = pCache;
		}
	}
	if( nCacheType==CHEDATA_TYPE_D3DTEXT )
	{
		CCacheD3DTextData *pCache = new CCacheD3DTextData;
		if( pCache!=NULL )
		{
			pCache->SetD3D(m_pContext->m_pD3D,TRUE);
			pCache->m_bUseAlpha = TRUE;
			pCache->m_bUseLinearZoom = FALSE;
			m_pCacheData = pCache;
		}
	}	
	if( !CDrawingLayer::InitCache(nCacheType, szBlock, nBlock) )
		return FALSE;
	return TRUE;
}


static BOOL IsNearColor(COLORREF c1, COLORREF c2)
{
	int r1 = GetRValue(c1), g1 = GetGValue(c1), b1 = GetBValue(c1);	
	int r2 = GetRValue(c2), g2 = GetGValue(c2), b2 = GetBValue(c2);	
	if( (r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2) < 2*70*70 )
	{
		return TRUE;
	}

	return FALSE;
}


static void DrawArrow(PT_3D pt0, PT_3D pt1, double scale, COLORREF color, GrBuffer *pBuf)
{
	//k是视图像素坐标到大地坐标的换算系数
	double k = 1.0/scale;
	double cosa = 1, sina = 0, dz = 0;
	double dis = GraphAPI::GGet2DDisOf2P(pt0,pt1);
	if( dis>1e-4 )
	{					
		cosa = (pt1.x-pt0.x)/dis;
		sina = (pt1.y-pt0.y)/dis;
		dz = (pt1.z-pt0.z)/dis;
	}

	PT_3D pt2, pt3, pt4, pt5;
	pt2.x = pt0.x + k*cosa*20; pt2.y = pt0.y + k*sina*20; pt2.z = pt0.z + k*dz*20;
	pt3.x = pt0.x + k*cosa*35; pt3.y = pt0.y + k*sina*35; pt3.z = pt0.z + k*dz*35;
	pt4.x = pt2.x - k*sina*5;  pt4.y = pt2.y + k*cosa*5;  pt4.z = pt2.z;
	pt5.x = pt2.x + k*sina*5;  pt5.y = pt2.y - k*cosa*5;  pt5.z = pt2.z;

	pBuf->BeginLineString(color,0.0);
	pBuf->MoveTo(&pt0);
	pBuf->LineTo(&pt2);
	pBuf->End();

	pBuf->BeginPolygon(color);
	pBuf->MoveTo(&pt3);
	pBuf->LineTo(&pt4);
	pBuf->LineTo(&pt5);
	pBuf->LineTo(&pt3);
	pBuf->End();
}

// update selection drawing
void CMarkVectLayer::UpdateMark1()
{
	if( m_pSel==NULL )
		return;

	int num;
	const FTR_HANDLE* handles = m_pSel->GetSelectedObjs(num);

	m_eleList1.delAll();

	ClearBuffers(m_eleList_baseline);
	m_eleList_baseline.delAll();

	int transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", 50);

	for( int i=0; i<num; i++)
	{
		CFeature *pFtr = HandleToFtr(handles[i]);
		if (!pFtr) continue;

		CGeometryBase *pObj = pFtr->GetGeometry();
		m_eleList1.addBuffer(pObj,TRUE,FALSE);

		if(1)
		{
			COLORREF hiColor = RGB(255,0,255);
			COLORREF geoColor = pObj->GetColor();
			if( IsNearColor(hiColor,geoColor) )
				hiColor = RGB(255,180,255);

			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			{
				if( ((CGeoCurveBase*)pObj)->IsFillColor() )
				{
					if( IsNearColor(hiColor,((CGeoCurveBase*)pObj)->GetFillColor() ) )
					{
						hiColor = RGB(255,180,255);
					}
				}
			}

			GrBuffer *pBuf = new GrBuffer();

			//文字的实时绘制太慢，于是取其轮廓
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				((CGeoText*)pObj)->ExplodeAsGrBuffer(pBuf,m_pDS->GetSymbolDrawScale());
			}
			else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				CGeometry *pNewObj = pFtr->GetGeometry()->Clone();
				((CGeoSurface*)pNewObj)->EnableFillColor(TRUE, hiColor);
				((CGeoSurface*)pNewObj)->SetTransparency(transparency);
				pNewObj->Draw(pBuf,m_pDS->GetSymbolDrawScale());
				delete pNewObj;
			}
			else
			{
				pFtr->GetGeometry()->Draw(pBuf,m_pDS->GetSymbolDrawScale());
			}
			
			pBuf->SetAllColor(hiColor);
			pBuf->SetAllLineWidth(FALSE,2);

			//绘制起点箭头
			if(num<100)
			{
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pFtr->GetGeometry()->GetShape(arrPts);
				if(arrPts.GetSize()>1)
				{
					CArray<PT_3DEX,PT_3DEX> arrPts2;
					PT_3D pt0 = arrPts[0];
					PT_3D pt1 = pt0;
					if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					{
						((CGeoCurveBase*)pObj)->GetShape()->GetPts(arrPts2);
						for(int k=1; k<arrPts2.GetSize(); k++)
						{
							if( !GraphAPI::GIsEqual2DPoint(&arrPts2[k],&arrPts2[k-1]) )
							{
								pt1 = arrPts2[k];
								break;
							}
						}
					}

					DrawArrow(pt0,pt1,m_pCS->CalcScale(),hiColor,pBuf);

					if( pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
					{
						arrPts2.RemoveAll();
						((CGeoDCurve*)pObj)->GetAssistShape(arrPts2);
						CGeoCurve obj;
						obj.CreateShape(arrPts2.GetData(),arrPts2.GetSize());
						arrPts2.RemoveAll();
						obj.GetShape()->GetPts(arrPts2);
						if(arrPts2.GetSize()>1)
						{
							pt0 = arrPts2[0];
							pt1 = pt0;

							for(int k=1; k<arrPts2.GetSize(); k++)
							{
								if( !GraphAPI::GIsEqual2DPoint(&arrPts2[k],&arrPts2[k-1]) )
								{
									pt1 = arrPts2[k];
									break;
								}
							}

							DrawArrow(pt0,pt1,m_pCS->CalcScale(),RGB(128,0,128),pBuf);
						}
					}
				}
			}

			m_eleList_baseline.addBuffer(pBuf);
		}
	}
}



// update mark drawing
void CMarkVectLayer::SetAllMark2(BOOL bMark2)
{
	if( !bMark2 )
	{
		m_eleList2.delAll();
		return;
	}

	int nlay = m_pDS->GetFtrLayerCount();
	int i,j;

	m_eleList2.delAll();

	for ( i=0;i<nlay;i++)
	{
		CFtrLayer *pFtrLayer = m_pDS->GetFtrLayerByIndex(i);
		if(!pFtrLayer||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible()) continue;
		
		for (j=0;j<pFtrLayer->GetObjectCount();j++)
		{
			CFeature *pFtr = pFtrLayer->GetObject(j);
			if (!pFtr)  continue;

			CGeometryBase *pObj = pFtr->GetGeometry();
			m_eleList2.addBuffer(pObj,FALSE,TRUE);	
		}		
	}
}

// 获得块编号的合理的取值范围
void CMarkVectLayer::GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax)
{
	xmin = ymin = -INT_MAX;
	xmax = ymax = INT_MAX;
}

// 获得焦点处所在的块编号
void CMarkVectLayer::GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum)
{
	CalcClientBound();

	xnum = floor((ptFocus.x-m_DataClientBound.m_xl)/m_szBlock.cx);
	ynum = floor((ptFocus.y-m_DataClientBound.m_yl)/m_szBlock.cy);
}


// 获得块的四个角点坐标在客户坐标系下的XY坐标
void CMarkVectLayer::GetBlockClientXY(CacheID id, double x[4], double y[4])
{
	CalcClientBound();

	x[0] = m_DataClientBound.m_xl + id.xnum*(double)m_szBlock.cx; 
	y[0] = m_DataClientBound.m_yl + id.ynum*(double)m_szBlock.cy;
	x[1] = m_DataClientBound.m_xl + (id.xnum+1)*(double)m_szBlock.cx; 
	y[1] = m_DataClientBound.m_yl + id.ynum*(double)m_szBlock.cy;
	x[2] = m_DataClientBound.m_xl + (id.xnum+1)*(double)m_szBlock.cx; 
	y[2] = m_DataClientBound.m_yl + (id.ynum+1)*(double)m_szBlock.cy;
	x[3] = m_DataClientBound.m_xl + id.xnum*(double)m_szBlock.cx; 
	y[3] = m_DataClientBound.m_yl + (id.ynum+1)*(double)m_szBlock.cy;
}


void CMarkVectLayer::CalcClientBound()
{
	if( m_pDS==NULL )
		return;

	if( !m_bCalcClientBound )
	{
		//获得数据范围在客户坐标系下的范围
		CCoordSys *pCS = m_pDrawContentCS;
		
		m_DataClientBound = GetDataBound();
		
		m_DataClientBound.TransformGrdToClt(pCS,1);

		//取整
		/*
		m_DataClientBound.m_xl = floor(m_DataClientBound.m_xl);
		m_DataClientBound.m_yl = floor(m_DataClientBound.m_yl);
		m_DataClientBound.m_xh = ceil(m_DataClientBound.m_xh);
		m_DataClientBound.m_yh = ceil(m_DataClientBound.m_yh);
		*/
		
		m_bCalcClientBound = TRUE;
	}
}


void CMarkVectLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	m_bCalcClientBound = FALSE;
	CalcClientBound();

	if( !bJustScrollScreen )
	{
		ClearAll();
	}
}


// 填充块的内容
BOOL CMarkVectLayer::FillBlock(int blkIdx, CacheID id)
{
	if( m_pDS==NULL )
		return FALSE;

	if( m_pContext==NULL || m_pCacheData==NULL )
		return FALSE;

	//获得数据范围在客户坐标系下的范围
	CCoordSys *pCS = m_pContext->GetCoordSys();

	CalcClientBound();
	Envelope e = m_DataClientBound;

	e.m_xl = PIXEL(e.m_xl) + (id.xnum*m_szBlock.cx);
	e.m_yl = PIXEL(e.m_yl) + (id.ynum*m_szBlock.cy);

	//定义偏移换算关系
	double m[16];
	Matrix44FromMove(-e.m_xl,-e.m_yl,0,m);
	
	CCoordSys moveCS;
	moveCS.Create44Matrix(m);

	CCoordSys dataCS;
	dataCS.Create44Matrix(m_DataMatrix);
	
	//为cache中的数据创造复合坐标系（也就是cache的坐标系）
	CCoordSys compound;
	CArray<CCoordSys*,CCoordSys*> arr;
	arr.Add(&dataCS);
	arr.Add(m_pCS);
	arr.Add(&moveCS);

	//使用单一的矩阵，速度更快
	if( m_pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		double m1[16], m2[16];
		m_pCS->GetMatrix(m1);

		matrix_multiply(m1,m_DataMatrix,4,m2);

		matrix_multiply(m,m2,4,m1);
		compound.Create44Matrix(m1);
	}
	else
		compound.CreateCompound(arr);

	//当前块在复合坐标系下的范围
	m_pContext->SetCoordSys(&compound);

	if( !m_pContext || !m_pCacheData )
		return FALSE;
	
	
	CRect rcViewSave;
	CSize szDC = m_pContext->GetDCSize();
	m_pContext->GetViewRect(rcViewSave);
	m_pContext->SetDCSize(m_szBlock);
	m_pContext->SetViewRect(CRect(0,0,m_szBlock.cx,m_szBlock.cy));

	m_pCacheData->BeginFillBlock(m_pContext,blkIdx);	

	//绘制数据
	DrawClientWithCache(0,m_szBlock.cx,0,m_szBlock.cy,TRUE);

	m_pCacheData->EndFillBlock();
	m_pContext->SetDCSize(szDC);
	m_pContext->SetViewRect(rcViewSave);

	//恢复坐标系
	m_pContext->SetCoordSys(pCS);
	

	return CDrawingLayer::FillBlock(blkIdx,id);
}

void CMarkVectLayer::DrawClientWithCache(double xl, double xh, double yl, double yh, BOOL bUseCache)
{
	DWORD time0 = GetTickCount();

	if( bUseCache && m_pCacheData!=NULL )
	{
		m_pCacheData->DrawGrBuffer(&m_eleList_baseline,CDrawingContext::modeNormal);
		{
			CMarkPtInfor markInfor;
			markInfor.Add(ptNone,m_clrMark1,m_fMarkWidth1);
			markInfor.Add(ptSpecial,m_clrFtrPtColor,m_fMarkWidth1*1.5);

			m_eleList1.which_mark = 1;
			m_pCacheData->DrawGeometryMark(&m_eleList1,CDrawingContext::modeNormal,markInfor);
		}			
		{
			CMarkPtInfor markInfor;
			markInfor.Add(ptNone,m_clrMark2,m_fMarkWidth2);
			markInfor.Add(ptSpecial,m_clrFtrPtColor,m_fMarkWidth2*1.5);

			m_eleList2.which_mark = 2;
			m_pCacheData->DrawGeometryMark(&m_eleList2,CDrawingContext::modeNormal,markInfor);
		}
		
	}
	else 
	{
		m_pContext->DrawGrBuffer(&m_eleList_baseline,CDrawingContext::modeNormal);
		{
			CMarkPtInfor markInfor;
			markInfor.Add(ptNone,m_clrMark1,m_fMarkWidth1);
			markInfor.Add(ptSpecial,m_clrFtrPtColor,m_fMarkWidth1*1.5);

			m_eleList1.which_mark = 1;
			m_pContext->DrawGeometryMark(&m_eleList1,CDrawingContext::modeNormal,markInfor);
		}
		{
			CMarkPtInfor markInfor;
			markInfor.Add(ptNone,m_clrMark2,m_fMarkWidth2);
			markInfor.Add(ptSpecial,m_clrFtrPtColor,m_fMarkWidth2*1.5);

			m_eleList2.which_mark = 2;
			m_pContext->DrawGeometryMark(&m_eleList2,CDrawingContext::modeNormal,markInfor);
		}				
	}

	DWORD time1 = GetTickCount();

}


void CMarkVectLayer::DrawClient(double xl, double xh, double yl, double yh)
{
	if( m_pDS==NULL )
		return;

	CCoordSys *pCS = m_pContext->GetCoordSys();

	CCoordSys dataCS;
	dataCS.Create44Matrix(m_DataMatrix);
	
	//创造复合坐标系（也就是cache的坐标系）
	CCoordSys compound;
	CArray<CCoordSys*,CCoordSys*> arr;
	arr.Add(&dataCS);
	arr.Add(m_pCS);
	
	//使用单一的矩阵，速度更快
	if( m_pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		double m1[16], m2[16];
		m_pCS->GetMatrix(m1);
		
		matrix_multiply(m1,m_DataMatrix,4,m2);
		
		compound.Create44Matrix(m2);
	}
	else
		compound.CreateCompound(arr);
	
	//当前块在复合坐标系下的范围
	m_pContext->SetCoordSys(&compound);	

	DrawClientWithCache(xl,xh,yl,yh,FALSE);	
	
	//恢复坐标系
	m_pContext->SetCoordSys(pCS);
}



void CMarkVectLayer::SetDataBound(Envelope e)
{
	m_DataBound = e;
	m_bCalcClientBound = FALSE;
}


void CMarkVectLayer::SetDataMatrix(double m[16])
{
	memcpy(m_DataMatrix,m,sizeof(m_DataMatrix));
	matrix_reverse(m_DataMatrix,4,m_DataRMatrix);
}

Envelope CMarkVectLayer::GetDataBound()
{
	Envelope e = m_DataBound;
	e.Transform(m_DataMatrix);
	return e;
}
