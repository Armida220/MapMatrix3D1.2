// DrawingContext.cpp: implementation of the CDrawingContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glew.h"
#include "DrawingContext.h"
#include "SymbolBase.h"
#include <math.h>
#include "DrawLineDC.h"
#include "DxFont.h"
#include "GlobalFunc.h"
#include "textureobj.h"
#include "SmartViewFunctions.h"

#include "vectorlayer.h"
#include "GPUtexture.h"

#include "Matrix.h"
#include "TextRender.h "
#include "GeometryBase.h"
#include "PlotWChar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable:4244)


#define PIXELX_GDI(x)	PIXEL(x)
#define PIXELY_GDI(x)	PIXEL(x)

#define CLEARTYPE_QUALITY 5

//#define PIXEL_GL(x)		(x>=0?((int)(x+1e-6)):((int)(x-1e-6)))
#define PIXELX_GL(x)	(x)
#define PIXELY_GL(x)	(x)



class CTempMemAllocate2
{
public:
	CTempMemAllocate2(){
		m_pBuf = NULL;
		m_nSize = 0;
	}
	~CTempMemAllocate2(){
		if( m_pBuf )delete[] m_pBuf;
	}
	BYTE* Allocate(int size){
		if( size<=m_nSize )
			return m_pBuf;
		
		if( m_pBuf )delete[] m_pBuf;
		m_pBuf = new BYTE[size];
		if( m_pBuf )
		{
			m_nSize = size;
			return m_pBuf;
		}
		return NULL;
	}
	
private:
	BYTE *m_pBuf;
	int m_nSize;
};


CTempMemAllocate2 gMem2;

MyNameSpaceBegin


extern BOOL gbUseTrimDrawLine;


CMarkPtInfor::CMarkPtInfor()
{

}

CMarkPtInfor::~CMarkPtInfor()
{
	arr.RemoveAll();
}

void CMarkPtInfor::Add(const MarkPtInfor &infor)
{
	arr.Add(infor);
}

void CMarkPtInfor::Add(int t, COLORREF c, float w)
{
	arr.Add(MarkPtInfor(t,c,w));
}

COLORREF CMarkPtInfor::GetCol(int type)
{
	for (int i=0; i<arr.GetSize(); i++)
	{
		if (arr[i].type == type)
		{
			return arr[i].clr;
		}
	}

	return RGB(255,255,255);
}
	
float CMarkPtInfor::GetWid(int type)
{
	for (int i=0; i<arr.GetSize(); i++)
	{
		if (arr[i].type == type)
		{
			return arr[i].wid;
		}
	}
	
	return 0;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawingContext, CObject)

BOOL CDrawingContext::m_bTextClear = FALSE;

CDrawingContext::CDrawingContext():
m_szDC(0,0),m_rcView(0,0,0,0)
{
	m_pCoordSys = NULL;
	m_hOriginalDC = NULL;

	m_bMonoMode = FALSE;
	m_clrBack = 0;
	m_monoColor = RGB(255,255,255);

	m_hglRC = NULL;
	m_hglRC_overlay = NULL;
	m_bCreateOK = FALSE;

	m_nRGBMask = RGBMASK_ALL;

	m_bNoLineWeight = FALSE;
	m_bNoHatch = FALSE;
	
	m_fDrawCellAngle = 0;
	m_fDrawCellKX = m_fDrawCellKY = 1.0;
	memset(m_saveDataUser,0,sizeof(m_saveDataUser));

	m_bSupportLogicOp = TRUE;

	m_bStereo = FALSE;
	m_bUseD3D = FALSE;
	m_pD3D = NULL;
	m_bCreateD3D = FALSE;

	m_bPrint = FALSE;
	m_fDefautPrintWidth = 1.0;
}

CDrawingContext::~CDrawingContext()
{
	for( int i=0; i<sizeof(m_saveData)/sizeof(m_saveData[0]); i++ )
	{
		if(	 m_saveData[i].pRectData )
			delete[] m_saveData[i].pRectData;
		
		if( NULL!=(HDC)m_saveData[i].dcMemRect )
			m_saveData[i].dcMemRect.DeleteDC();
		
		if( NULL!=(HBITMAP)m_saveData[i].bmpRect )
			m_saveData[i].bmpRect.DeleteObject();
	}

	if( m_bCreateD3D && m_pD3D )
		delete m_pD3D;
}


BOOL CDrawingContext::CreateContext(HDC hDC)
{
	m_hOriginalDC = hDC;
	m_bCreateOK = TRUE;
	return TRUE;
}

BOOL CDrawingContext::CreateD3DContext(HWND hWnd)
{
	m_hWnd = hWnd;

	if( m_bCreateD3D && m_pD3D!=NULL )
		delete m_pD3D;

	m_pD3D = new CD3DWrapper();
	if (!m_pD3D) return FALSE;

	m_bCreateD3D = TRUE;

	return m_pD3D->Create(hWnd);
}

void CDrawingContext::SetD3DGDIPresent(BOOL bGDIPresent)
{
	if( m_bCreateD3D && m_pD3D!=NULL )
	{
		m_pD3D->SetGDIPresent(bGDIPresent);
	}
}

void CDrawingContext::SetDCSize(CSize szDC)
{
	m_szDC = szDC;

	if( m_bUseD3D && m_pD3D!=NULL )
		m_pD3D->OnSize(m_szDC);
}

HDC CDrawingContext::GetContext()
{
	return m_hOriginalDC;
}

CSize CDrawingContext::GetDCSize()
{
	return m_szDC;
}


void CDrawingContext::SetViewRect(CRect rcView)
{
	m_rcView = rcView;
}


void CDrawingContext::GetViewRect(CRect& rcView)
{
	rcView = m_rcView;
}


void CDrawingContext::SetBackColor(COLORREF clr)
{
	m_clrBack = clr;
}
void CDrawingContext::DrawGrBuffer(GrElementList *pList, int mode)
{
	
}


void CDrawingContext::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	
}

void CDrawingContext::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	
}


void CDrawingContext::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	
}

void CDrawingContext::DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	
}


void CDrawingContext::DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4])
{
}

void CDrawingContext::CopyDisplaySetting(CDrawingContext *pDC)
{
	m_bNoHatch = pDC->m_bNoHatch;
	m_bNoLineWeight = pDC->m_bNoLineWeight;
	m_bStereo = pDC->m_bStereo;

	m_fDrawCellAngle = pDC->m_fDrawCellAngle;
	m_fDrawCellKX = pDC->m_fDrawCellKX;
	m_fDrawCellKY = pDC->m_fDrawCellKY;
}

void CDrawingContext::DrawGrBuffer_GDI(HDC hdc, GrElementList *pList, int mode)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GDI,GetContext(),m_pD3D);

	dc.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);

	m_PointDrawOpSet.ClearAll();
	m_PointStringDrawOpSet.ClearAll();
	
	DrawGrBuffer_Base(&dc,pList,mode);
}



void CDrawingContext::DrawGrBuffer2d_GDI(HDC hdc, GrElementList *pList, int mode)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GDI,GetContext(),m_pD3D);

	dc.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);

	m_PointDrawOpSet.ClearAll();
	m_PointStringDrawOpSet.ClearAll();
	
	DrawGrBuffer2d_Base(&dc,pList,mode);
}


void CDrawingContext::DrawSelectionMark_GDI(HDC hdc, GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GDI,GetContext(),m_pD3D);

	dc.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);

	DrawSelectionMark_Base(&dc,pList,mode,clr,wid);
}


void CDrawingContext::DrawSelectionMark2d_GDI(HDC hdc, GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GDI,GetContext(),m_pD3D);

	dc.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	DrawSelectionMark2d_Base(&dc,pList,mode,clr,wid);
}

void CDrawingContext::DrawGeometryMark_GDI(HDC hdc, GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GDI,GetContext(),m_pD3D);

	dc.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	DrawGeometryMark_Base(&dc,pList,mode,ptInfor);
}



void CDrawingContext::DrawGeometryMark_GL(HDC hdc, GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GL,GetContext(),m_pD3D);

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	DrawGeometryMark_Base(&dc,pList,mode,ptInfor);
}


void CDrawingContext::DrawGrBuffer_D3D(GrElementList *pList, int mode)
{
	if( m_pD3D==NULL )
		return;

	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_D3D,GetContext(),m_pD3D);

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	m_PointDrawOpSet.ClearAll();
	m_PointStringDrawOpSet.ClearAll();
	DrawGrBuffer_Base(&dc,pList,mode);
}


void CDrawingContext::DrawGrBuffer2d_D3D(GrElementList *pList, int mode)
{
	if( m_pD3D==NULL )
		return;

	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_D3D,GetContext(),m_pD3D);

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	m_PointDrawOpSet.ClearAll();
	m_PointStringDrawOpSet.ClearAll();
	DrawGrBuffer2d_Base(&dc,pList,mode);
}


void CDrawingContext::DrawSelectionMark_D3D(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	if( m_pD3D==NULL )
		return;
	
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_D3D,GetContext(),m_pD3D);

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	DrawSelectionMark_Base(&dc,pList,mode,clr,wid);
}


void CDrawingContext::DrawSelectionMark2d_D3D(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	if( m_pD3D==NULL )
		return;
	
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_D3D,GetContext(),m_pD3D);

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	DrawSelectionMark2d_Base(&dc,pList,mode,clr,wid);
}


void CDrawingContext::DrawGeometryMark_D3D(GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	if( m_pD3D==NULL )
		return;
	
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_D3D,GetContext(),m_pD3D);

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	DrawGeometryMark_Base(&dc,pList,mode,ptInfor);
}



void CDrawingContext::DrawGrBuffer_GL(GrElementList *pList, int mode)
{	
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GL,GetContext());

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	m_PointDrawOpSet.ClearAll();
	m_PointStringDrawOpSet.ClearAll();
	DrawGrBuffer_Base(&dc,pList,mode);
}



void CDrawingContext::DrawGrBuffer2d_Base(CSuperDrawDC *pDC, GrElementList *pList, int mode, double lfWidthRatio)
{
	CRect rcView = m_rcView;
	
	Envelope e0(rcView.left,rcView.right,rcView.top,rcView.bottom),e;

	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	CellDef cell;
	BaseLineType linetype; 

	CCoordSys *pCS = GetCoordSys();

	float gscale = 1.0;
	if( pCS )gscale = pCS->CalcScale();	

	BYTE code = 2;
	{
		for( int i=0; i<7; i++)
		{
			if( ((1<<i)/COMP_SCALE0)>=gscale )break;
			code = (code<<1);
		}
		
		if( i>=7 )code = 0;
	}
	
	Graph2d *gr;
	PT_4D pt2d, oldpt2d;

	long grptcount = 0, grlinecount = 0, ptcount = 0;

	//直接使用 matrix，速度会更快
	double m[16], tx, ty;
	BOOL bUseMatrix = FALSE;
	if( pCS!=NULL && pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		bUseMatrix = TRUE;
		pCS->GetMatrix(m);
	}

	if( pCS )e0.TransformCltToGrd(pCS);

	GrElement *ele = pList->head;
	for( ; ele!=NULL; ele=ele->next )
	{
		if( !ele->bvisible )
			continue;
		
		GrBuffer2d *pBuf= (GrBuffer2d*)ele->pBuf;
		
		float xoff,yoff;
		pBuf->GetOrigin(xoff,yoff);
		
		gr = pBuf->HeadGraph();
		if( !gr )continue;		

		for( ; gr!=NULL; gr=gr->next )
		{
			if( gr->use_needdraw )
			{
				if( !gr->needdraw )
					continue;
			}
			else if( !pDC->IsEnableUseOpSet() )
			{
				e = GetEnvelopeOfGraph2d(gr,m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY,gscale);
				e.m_xh += xoff;
				e.m_xl += xoff;
				e.m_yl += yoff;
				e.m_yh += yoff;

				if( max(e.m_xl,e0.m_xl)>min(e.m_xh,e0.m_xh) ||
					max(e.m_yl,e0.m_yl)>min(e.m_yh,e0.m_yh) )
				continue;
			}

			DWORD dcolor = gr->bUseBackColor?GetBackColor():GetDrawColor(gr->color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);

			if( IsGrPoint2d(gr) )
			{
				grptcount++;
				const GrPoint2d *cgr = (const GrPoint2d*)gr;

				pt2d.x = cgr->pt.x + (double)xoff;
				pt2d.y = cgr->pt.y + (double)yoff;
				
				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
					ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
					
					pt2d.x = tx;
					pt2d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt2d,1);	
								
				//图元块
				if( cgr->cell>0 && pCellLib!=NULL && cgr->cell<=pCellLib->GetCellDefCount() )
				{
					pDC->SetColor(dcolor);

					cell = pCellLib->GetCellDef(cgr->cell-1);
					if( cell.m_pgr!=NULL )
					{
						Envelope et = GetEnvelopeOfGraph2d(gr,m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY,gscale);

						double wt = et.Width(), ht = et.Height();
						if( cgr->bGrdSize )
						{
							wt *= gscale; ht *= gscale;
						}
						if( wt<3.0 && ht<3.0 )
						{
							pDC->Point(pt2d.x,pt2d.y,0,dcolor,(wt+ht)*0.25*lfWidthRatio);
						}
						else if( !pDC->IsEnableUseOpSet() )
						{							
							//使用已经生成的 DrawOpSet，直接绘制
							if( m_PointDrawOpSet.CompareCell(cgr->cell,dcolor,cgr->kx,cgr->ky,cgr->angle,cgr->width,cgr->bGrdSize) )
							{
								m_PointDrawOpSet.MoveToOpSetXY(pt2d.x,pt2d.y,0);
								pDC->DrawSet(&m_PointDrawOpSet);
							}
							else
							{			
								//生成 DrawOpSet
								double m1[9],m2[9],m3[9];
								
								Matrix33FromZoom(cgr->kx,cgr->ky,m1);
								Matrix33FromRotate(&PT_2D(),cgr->angle,m2);
								matrix_multiply(m2,m1,3,m3);
								
								// 考虑视图的旋转和拉伸
								if( m_fDrawCellAngle!=0.0f )
								{
									Matrix33FromRotate(&PT_2D(),m_fDrawCellAngle,m2);
									matrix_multiply(m2,m3,3,m1);
									memcpy(m3,m1,sizeof(m3));
								}
								if( m_fDrawCellKX!=1.0f || m_fDrawCellKY!=1.0f )
								{
									Matrix33FromZoom(m_fDrawCellKX,m_fDrawCellKY,m1);
									matrix_multiply(m1,m3,3,m2);
									memcpy(m3,m2,sizeof(m3));
								}

								Matrix33FromMove(cgr->pt.x+xoff,cgr->pt.y+yoff,m2);
								matrix_multiply(m2,m3,3,m1);
								
								GrBuffer2d buf;
								buf.AddBuffer(cell.m_pgr);
								
								buf.Transform(m1);
								buf.ZoomPointSize(cgr->kx,cgr->ky);
								buf.ZoomCompressInfo(max(cgr->kx*m_fDrawCellKX,cgr->ky*m_fDrawCellKY));	
								
								buf.SetAllColor(dcolor);
								buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);
								
								m_PointDrawOpSet.ClearAll();
								pDC->EnableUseOpSet(TRUE,&m_PointDrawOpSet);
								
								DrawGrBuffer2d_Base(pDC,&GrElementList(&buf),mode,lfWidthRatio);
								
								pDC->EnableUseOpSet(FALSE,NULL);
								
								m_PointDrawOpSet.SetCellInfo(cgr->cell,dcolor,cgr->kx,cgr->ky,cgr->angle,cgr->width,cgr->bGrdSize);
								m_PointDrawOpSet.SetCellPt(pt2d.x,pt2d.y,0);
								
								//绘制 DrawOpSet
								pDC->DrawSet(&m_PointDrawOpSet);
							}
							
						}
						else
						{
							GrBuffer2d buf;
							
							buf.AddBuffer(cell.m_pgr);

							//Transform 应该会更慢，应减少其调用，以下代码改进速度
							double m1[9],m2[9],m3[9];
							Matrix33FromZoom(cgr->kx,cgr->ky,m1);
							Matrix33FromRotate(&PT_2D(),cgr->angle,m2);
							matrix_multiply(m2,m1,3,m3);

							// 考虑视图的旋转和拉伸
							if( m_fDrawCellAngle!=0.0f )
							{
								Matrix33FromRotate(&PT_2D(),m_fDrawCellAngle,m2);
								matrix_multiply(m2,m3,3,m1);
								memcpy(m3,m1,sizeof(m3));
							}
							if( m_fDrawCellKX!=1.0f || m_fDrawCellKY!=1.0f )
							{
								Matrix33FromZoom(m_fDrawCellKX,m_fDrawCellKY,m1);
								matrix_multiply(m1,m3,3,m2);
								memcpy(m3,m2,sizeof(m3));
							}

							Matrix33FromMove(cgr->pt.x+xoff,cgr->pt.y+yoff,m2);
							matrix_multiply(m2,m3,3,m1);

							buf.Transform(m1);
							buf.ZoomPointSize(cgr->kx,cgr->ky);
							buf.ZoomCompressInfo(max(cgr->kx*m_fDrawCellKX,cgr->ky*m_fDrawCellKY));
							
							buf.SetAllColor(dcolor);
							buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);

							GrElementList list(&buf);
							DrawGrBuffer2d_Base(pDC,&list,mode,lfWidthRatio);
						}
					}
				}
				//缺省处理行为
				else
				{				
					if( !cgr->bGrdSize )
						pDC->Point(pt2d.x,pt2d.y,0,dcolor,cgr->kx*lfWidthRatio,cgr->bFlat);
					else 
						pDC->Point(pt2d.x,pt2d.y,0,dcolor,cgr->kx*gscale*m_fDrawCellKX*lfWidthRatio,cgr->bFlat);
				}
			}
			else if( IsGrPointString2d(gr)||IsGrPointString2dEx(gr) )
			{
				const GrPointString2d *cgr = (const GrPointString2d*)gr;

				const GrPointString2dEx *cgrEx = NULL;
				if( !gr->bUseBackColor && IsGrPointString2dEx(gr) )
				{
					cgrEx = (const GrPointString2dEx*)gr;
				}
				
				//图元块
				if( cgr->cell>0 && pCellLib!=NULL && cgr->cell<=pCellLib->GetCellDefCount() && cgr->ptlist.nuse>0 )
				{
					pDC->SetColor(dcolor);
					
					cell = pCellLib->GetCellDef(cgr->cell-1);
					if( cell.m_pgr!=NULL )
					{
						Envelope et = GetEnvelopeOfPointInString2d(gr,m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY,gscale);

						double wt = et.Width(), ht = et.Height();
						if( cgr->bGrdSize )
						{
							wt *= gscale; ht *= gscale;
						}
						if( wt<3.0 && ht<3.0 )
						{
							double wid = (wt+ht)*0.5;
							wid *= lfWidthRatio;

							Envelope e2 = e0;
							if( cgr->bGrdSize )
								e2.Inflate(wid/gscale,wid/gscale,0);
							else
								e2.Inflate(wid,wid,0);
							
							BOOL bBatchPoint = pDC->CanBatchPoint();
							if( bBatchPoint )
								pDC->StartBatchPoint(dcolor,wid);

							GrVertex2d *pts = cgr->ptlist.pts;

							for(int i=0; i<cgr->ptlist.nuse; i++,pts++)
							{
								pt2d.x = pts->x + (double)xoff;
								pt2d.y = pts->y + (double)yoff;

								//至少打一个点
								if( i!=0 && !e2.bPtIn(&pt2d) )
									continue;
								
								//直接使用矩阵，速度更快
								if( bUseMatrix )
								{
									tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
									ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
									
									pt2d.x = tx;
									pt2d.y = ty;
								}
								else if( pCS )
									pCS->GroundToClient(&pt2d,1);

								if( cgrEx )
								{
									dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
								}

								pDC->Point(pt2d.x,pt2d.y,0,dcolor,wid);
							}

							if( bBatchPoint )
							{
								pDC->EndBatchPoint();
							}

						}
						//使用 DrawOpCache，减少重复的坐标换算
						else if( !pDC->IsEnableUseOpSet() && cgr->ptlist.nuse>2 )
						{
							m_PointStringDrawOpSet.ClearAll();

							//先用第一个点生成 chache
							double m1[9],m2[9],m3[9];
							{
								Matrix33FromZoom(cgr->kx,cgr->ky,m1);
								Matrix33FromRotate(&PT_2D(),cgr->ptlist.pts[0].CodeToFloat(),m2);
								matrix_multiply(m2,m1,3,m3);
								
								// 考虑视图的旋转和拉伸
								if( m_fDrawCellAngle!=0.0f )
								{
									Matrix33FromRotate(&PT_2D(),m_fDrawCellAngle,m2);
									matrix_multiply(m2,m3,3,m1);
									memcpy(m3,m1,sizeof(m3));
								}
								if( m_fDrawCellKX!=1.0f || m_fDrawCellKY!=1.0f )
								{
									Matrix33FromZoom(m_fDrawCellKX,m_fDrawCellKY,m1);
									matrix_multiply(m1,m3,3,m2);
									memcpy(m3,m2,sizeof(m3));
								}

								pt2d.x = cgr->ptlist.pts[0].x + (double)xoff;
								pt2d.y = cgr->ptlist.pts[0].y + (double)yoff;
								
								Matrix33FromMove(pt2d.x,pt2d.y,m2);
								matrix_multiply(m2,m3,3,m1);
								
								GrBuffer2d buf;
								buf.AddBuffer(cell.m_pgr);
								
								buf.Transform(m1);
								buf.ZoomPointSize(cgr->kx,cgr->ky);
								buf.ZoomCompressInfo(max(cgr->kx*m_fDrawCellKX,cgr->ky*m_fDrawCellKY));	
								
								buf.SetAllColor(dcolor);
								buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);
								
								pDC->EnableUseOpSet(TRUE,&m_PointStringDrawOpSet);

								DrawGrBuffer2d_Base(pDC,&GrElementList(&buf),mode,lfWidthRatio);

								pDC->EnableUseOpSet(FALSE,NULL);

							}

							//绘制第一个点
							pDC->DrawSet(&m_PointStringDrawOpSet);

							PT_3D tpt0, tpti;
							pt2d.x = cgr->ptlist.pts[0].x + (double)xoff;
							pt2d.y = cgr->ptlist.pts[0].y + (double)yoff;

							tpt0 = pt2d;

							if( bUseMatrix )
							{
								tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
								ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
								
								pt2d.x = tx;
								pt2d.y = ty;
							}
							else if( pCS )
								pCS->GroundToClient(&pt2d,1);

							m_PointStringDrawOpSet.SetCellPt(pt2d.x,pt2d.y,pt2d.z);

							Envelope e2 = GetEnvelopeOfGraph2d(&cgr->GetPoint(0));
							Envelope e3;

							e2.m_xh += xoff;
							e2.m_xl += xoff;
							e2.m_yl += yoff;
							e2.m_yh += yoff;

							//绘制其他点
							for(int i=1; i<cgr->ptlist.nuse; i++)
							{	
								//相同的cell直接用已有的范围平移，得到新点的范围，更快

								pt2d.x = cgr->ptlist.pts[i].x + (double)xoff;
								pt2d.y = cgr->ptlist.pts[i].y + (double)yoff;

								tpti = pt2d;
								
								if( bUseMatrix )
								{
									tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
									ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
									
									pt2d.x = tx;
									pt2d.y = ty;
								}
								else if( pCS )
									pCS->GroundToClient(&pt2d,1);

								e3 = e2;
								e3.m_xl += (tpti.x-tpt0.x);
								e3.m_xh += (tpti.x-tpt0.x);
								e3.m_yl += (tpti.y-tpt0.y);
								e3.m_yh += (tpti.y-tpt0.y);

								if( !e3.bIntersect(&e0) )continue;

								//code中存储的是旋转角
								if( cgr->ptlist.pts[i].code==cgr->ptlist.pts[0].code )
								{
									m_PointStringDrawOpSet.MoveToOpSetXY(pt2d.x,pt2d.y,pt2d.z);	

									if( cgrEx )
									{
										dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
										m_PointStringDrawOpSet.SetAllColor(dcolor);
									}

									tpt0 = tpti;
									e2 = e3;
									pDC->DrawSet(&m_PointStringDrawOpSet);
								}
								//旋转角不一样，就单独绘制
								else
								{									
									Matrix33FromZoom(cgr->kx,cgr->ky,m1);
									Matrix33FromRotate(&PT_2D(),cgr->ptlist.pts[i].CodeToFloat(),m2);
									matrix_multiply(m2,m1,3,m3);
										
									// 考虑视图的旋转和拉伸
									if( m_fDrawCellAngle!=0.0f )
									{
										Matrix33FromRotate(&PT_2D(),m_fDrawCellAngle,m2);
										matrix_multiply(m2,m3,3,m1);
										memcpy(m3,m1,sizeof(m3));
									}
									if( m_fDrawCellKX!=1.0f || m_fDrawCellKY!=1.0f )
									{
										Matrix33FromZoom(m_fDrawCellKX,m_fDrawCellKY,m1);
										matrix_multiply(m1,m3,3,m2);
										memcpy(m3,m2,sizeof(m3));
									}

									pt2d.x = cgr->ptlist.pts[i].x + (double)xoff;
									pt2d.y = cgr->ptlist.pts[i].y + (double)yoff;	
									
									Matrix33FromMove(pt2d.x,pt2d.y,m2);
									matrix_multiply(m2,m3,3,m1);
									
									GrBuffer2d buf;
									buf.AddBuffer(cell.m_pgr);
									
									buf.Transform(m1);
									buf.ZoomPointSize(cgr->kx,cgr->ky);
									buf.ZoomCompressInfo(max(cgr->kx*m_fDrawCellKX,cgr->ky*m_fDrawCellKY));	

									if( cgrEx )
									{
										dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
									}

									buf.SetAllColor(dcolor);
									buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);
									
									DrawGrBuffer2d_Base(pDC,&GrElementList(&buf),mode,lfWidthRatio);
								}
							}
						}
						else
						{
							double m1[9],m2[9],m3[9];
							GrBuffer2d allBuf, buf0;

							GrVertex2d pt4d0, pt4d;

							//考虑视图的旋转和拉伸
							for(int i=0; i<cgr->ptlist.nuse; i++)
							{
								if( i>0 && cgr->ptlist.pts[i].code==cgr->ptlist.pts[i-1].code )
								{
									pt4d0 = cgr->ptlist.pts[i-1];
									pt4d = cgr->ptlist.pts[i];
									
									buf0.Move(pt4d.x-pt4d0.x,pt4d.y-pt4d0.y);

									allBuf.AddBuffer(&buf0);
								}
								else
								{
									Matrix33FromZoom(cgr->kx,cgr->ky,m1);
									Matrix33FromRotate(&PT_2D(),cgr->ptlist.pts[i].CodeToFloat(),m2);
									matrix_multiply(m2,m1,3,m3);
									
									// 考虑视图的旋转和拉伸
									if( m_fDrawCellAngle!=0.0f )
									{
										Matrix33FromRotate(&PT_2D(),m_fDrawCellAngle,m2);
										matrix_multiply(m2,m3,3,m1);
										memcpy(m3,m1,sizeof(m3));
									}
									if( m_fDrawCellKX!=1.0f || m_fDrawCellKY!=1.0f )
									{
										Matrix33FromZoom(m_fDrawCellKX,m_fDrawCellKY,m1);
										matrix_multiply(m1,m3,3,m2);
										memcpy(m3,m2,sizeof(m3));
									}

									pt2d.x = cgr->ptlist.pts[i].x + (double)xoff;
									pt2d.y = cgr->ptlist.pts[i].y + (double)yoff;								
																		
									Matrix33FromMove(pt2d.x,pt2d.y,m2);
									matrix_multiply(m2,m3,3,m1);

									GrBuffer2d buf;
									buf.AddBuffer(cell.m_pgr);
									
									buf.Transform(m1);
									buf.ZoomPointSize(cgr->kx,cgr->ky);
									buf.ZoomCompressInfo(max(cgr->kx*m_fDrawCellKX,cgr->ky*m_fDrawCellKY));

									if( cgrEx )
									{
										dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
									}

									buf.SetAllColor(dcolor);
									buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);

									buf0.CopyFrom(&buf);
									
									allBuf.AddBuffer(&buf);
								}
							}
							
							DrawGrBuffer2d_Base(pDC,&GrElementList(&allBuf),mode,lfWidthRatio);
						}
					}
				}
				//缺省处理行为
				else if( cgr->ptlist.nuse>0 )
				{
					pDC->SetColor(dcolor);

					double wid = cgr->kx*lfWidthRatio;
					
					Envelope e2 = e0;
					e2.Inflate(wid,wid,0);

					if( cgr->bGrdSize )
						wid = cgr->kx*gscale;

					BOOL bBatchPoint = pDC->CanBatchPoint();
					if( bBatchPoint )
						pDC->StartBatchPoint(dcolor,wid,cgr->bFlat);

					for(int i=0; i<cgr->ptlist.nuse; i++)
					{
						pt2d.x = cgr->ptlist.pts[i].x + (double)xoff;
						pt2d.y = cgr->ptlist.pts[i].y + (double)yoff;
					
						//至少打一个点
						if( i!=0 && !e2.bPtIn(&pt2d) )
							continue;
						
						if( bUseMatrix )
						{
							tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
							ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
							
							pt2d.x = tx;
							pt2d.y = ty;
						}
						else if( pCS )
							pCS->GroundToClient(&pt2d,1);

						if( cgrEx )
						{
							dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
						}

						pDC->Point(pt2d.x,pt2d.y,0,dcolor,wid,cgr->bFlat);
					}

					if( bBatchPoint )
					{
						pDC->EndBatchPoint();
					}
				}
			}
			else if( IsGrLineString2d(gr) )
			{
				grlinecount++;
				const GrLineString2d *cgr = (const GrLineString2d*)gr;

				pDC->Begin(cgr->ptlist.nuse);

				pDC->SetColor(dcolor);
				double lfWidth = cgr->width;

				if( !cgr->bGrdWid )
				{
					lfWidth = cgr->width*lfWidthRatio;
					if (lfWidth<lfWidthRatio)
					{
						lfWidth = lfWidthRatio;
					}
					pDC->SetWidth(lfWidth);
				}
				else
				{
					lfWidth = cgr->width*gscale*max(m_fDrawCellKX,m_fDrawCellKY)*lfWidthRatio;
					if (lfWidth<lfWidthRatio)
					{
						lfWidth = lfWidthRatio;
					}
					pDC->SetWidth(lfWidth);
				}
				if (m_bNoLineWeight) pDC->SetWidth(lfWidthRatio);

				if( cgr->style>0 && pLineLib!=NULL && cgr->style<=pLineLib->GetBaseLineTypeCount() )
				{
					linetype = pLineLib->GetBaseLineType(cgr->style-1);

					DWORD nStyle = 0;
					float style_len = 0;
					DWORD styles[8] = {0};
					{
						nStyle = linetype.m_nNum;
						for( int i=0; i<linetype.m_nNum; i++)
						{
							float len = fabs(linetype.m_fLens[i])*gscale*cgr->fDrawScale*max(m_fDrawCellKX,m_fDrawCellKY);
							if (fabs(len-int(len)) >= 0.5)
							{
								styles[i] = ceilf(len);
							}
							else
								styles[i] = floorf(len);
							
							style_len += len;
						}
					}

					//空线
					if(style_len==0.0f)
						continue;
					pDC->SetExtPen(nStyle,styles);

				}
				else
					pDC->SetExtPen(0,0);

				int nused = 0;
				PT_3D pts2[2];

				Envelope e2;
				BOOL bSkip = FALSE, bUseClip = (!pDC->IsEnableUseOpSet());
				int i0=-1, nSkipPt = -1;

				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					if( cgr->ptlist.isCompress!=0 && code!=0 && (cgr->ptlist.pts[i].code&code)==0 )
					{
						continue;
					}

					if( bUseMatrix && bUseClip )
					{
						if( i0>=0 && IsGrPtCodeLineTo(cgr->ptlist.pts+i) )
						{
							e2.m_xl = min((cgr->ptlist.pts+i0)->x,(cgr->ptlist.pts+i)->x)+(double)xoff; e2.m_xh = max((cgr->ptlist.pts+i0)->x,(cgr->ptlist.pts+i)->x)+(double)xoff;
							e2.m_yl = min((cgr->ptlist.pts+i0)->y,(cgr->ptlist.pts+i)->y)+(double)yoff; e2.m_yh = max((cgr->ptlist.pts+i0)->y,(cgr->ptlist.pts+i)->y)+(double)yoff;
							
							if( !e2.bIntersect(&e0) )
							{
								i0 = i;
								nSkipPt = i;
								bSkip = TRUE;
								continue;
							}
						}
					}

					if( bSkip )
					{
						bSkip = FALSE;
						if( nSkipPt>=0 )
						{
							pt2d.x = cgr->ptlist.pts[nSkipPt].x+(double)xoff;
							pt2d.y = cgr->ptlist.pts[nSkipPt].y+(double)yoff;
							
							//直接使用矩阵，速度更快
							if( bUseMatrix )
							{
								tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
								ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
								
								pt2d.x = tx;
								pt2d.y = ty;
							}
							else if( pCS )
								pCS->GroundToClient(&pt2d,1);
							
							if( nused==0 )
								pts2[0] = pt2d;
							else if( nused==1 )
								pts2[1] = pt2d;

							pDC->MoveTo(pt2d.x,pt2d.y,0);
						}
					}

					i0 = i;
					
					pt2d.x = cgr->ptlist.pts[i].x+(double)xoff;
					pt2d.y = cgr->ptlist.pts[i].y+(double)yoff;

					ptcount++;

					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];

						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);

					if( nused==0 )
						pts2[0] = pt2d;
					else if( nused==1 )
						pts2[1] = pt2d;

					if( IsGrPtCodeMoveTo(cgr->ptlist.pts+i) )
						pDC->MoveTo(pt2d.x,pt2d.y,0);
					else
					{
						pDC->LineTo(pt2d.x,pt2d.y,0);
						nused++;
					}						
				}
				pDC->End();		
				
				//至少打个点
				if( nused==1 && GraphAPI::GGet3DDisOf2P(pts2[0],pts2[1]) < GraphAPI::GetDisTolerance() )
				{
					pDC->Point(pts2[0].x,pts2[0].y,0,dcolor,1);
				}
				else if( nused==0 )
				{
					pt2d.x = cgr->ptlist.pts[0].x+(double)xoff;
					pt2d.y = cgr->ptlist.pts[0].y+(double)yoff;					
					
					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
						
						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);

					pDC->Point(pt2d.x,pt2d.y,0,dcolor,1);
				}
				
			}
			else if( IsGrPolygon2d(gr) )
			{
				const GrPolygon2d *cgr = (const GrPolygon2d*)gr;

				if (cgr->bUseBackColor&&m_bStereo)
				{
					continue;
				}

				if( cgr->filltype==POLYGON_FILLTYPE_COLOR )
				{
					dcolor = ((dcolor&0xffffff)|((cgr->index&0xff)<<24));
				}

				COLORREF color = dcolor;
				
				PT_3D *pts = new PT_3D[cgr->ptlist.nuse];
				int *polycount = new int[cgr->ptlist.nuse];
				if(polycount) 
					memset(polycount,0,sizeof(int)*cgr->ptlist.nuse);
				int npoly = -1, nnewpt = 0;
				if( pts && polycount )
				{
					for( int i=0; i<cgr->ptlist.nuse; i++)
					{
						if( IsGrPtCodeMoveTo(cgr->ptlist.pts+i) )
						{
							if( npoly>=0 && polycount[npoly]>2 )
								npoly++;
							else if( npoly>=0 && polycount[npoly]<=2 )
								polycount[npoly] = 0;
							else if( npoly<0 ) 
								npoly++;
						}

						if( npoly<0 )continue;

						if( cgr->ptlist.isCompress!=0 && code!=0 && (cgr->ptlist.pts[i].code&code)==0 )
						{
							continue;
						}
						
						pt2d.x = cgr->ptlist.pts[i].x+(double)xoff;
						pt2d.y = cgr->ptlist.pts[i].y+(double)yoff;

						//直接使用矩阵，速度更快
						if( bUseMatrix )
						{
							tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
							ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
							
							pt2d.x = tx;
							pt2d.y = ty;
						}
						else if( pCS )
							pCS->GroundToClient(&pt2d,1);	

						pts[nnewpt].x = pt2d.x;
						pts[nnewpt].y = pt2d.y;
						polycount[npoly]++;
						nnewpt++;
					}
					
					if( npoly>=0 && polycount[npoly]>2 )npoly++;

					//太小，成为一个点
					if( nnewpt<=2 && cgr->ptlist.nuse>0 )
					{
						pt2d.x = cgr->ptlist.pts[0].x+(double)xoff;
						pt2d.y = cgr->ptlist.pts[0].y+(double)yoff;
						
						//直接使用矩阵，速度更快
						if( bUseMatrix )
						{
							tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
							ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
							
							pt2d.x = tx;
							pt2d.y = ty;
						}
						else if( pCS )
							pCS->GroundToClient(&pt2d,1);								
						
						pDC->Point(pt2d.x,pt2d.y,0,dcolor,1,TRUE);
					}
					else
					{
						if( !m_bNoHatch )
						{
							if( npoly>1 )
								pDC->PolyPolygon(pts,polycount,npoly,nnewpt,dcolor);
							else if( npoly==1 )
								pDC->Polygon(pts,nnewpt,dcolor);
						}

						PT_3D *ptt = pts, *ptt2 = NULL;

						pDC->Begin(nnewpt+npoly);
						
						pDC->SetColor(dcolor);
						pDC->SetWidth(1);
						
						for( i=0; i<npoly; i++)
						{
							ptt2 = NULL;
							for( int j=0; j<polycount[i]; j++, ptt++)
							{
								if( j==0 )
								{
									pDC->MoveTo(ptt->x,ptt->y,ptt->z);
									ptt2 = ptt;
								}
								else
								{
									pDC->LineTo(ptt->x,ptt->y,ptt->z);
								}
							}
							
							if( ptt2!=NULL )
							{
								pDC->LineTo(ptt2->x,ptt2->y,ptt2->z);
							}
						}
						
						pDC->End();
					}


					delete[] pts;
					delete[] polycount;
				}
			}
			else if( IsGrText2d(gr) )
			{
				const GrText2d *cgr = (const GrText2d*)gr;
				COLORREF fontColor;

				fontColor = dcolor;
				
				float fh = cgr->settings.fHeight;
				
				if( cgr->bGrdSize )
				{
					fh = fh*gscale*(m_fDrawCellKX+m_fDrawCellKY)/2/*max(m_fDrawCellKX,m_fDrawCellKY)*/;
				}
				
				pt2d.x = cgr->pt.x+(double)xoff;
				pt2d.y = cgr->pt.y+(double)yoff;			

				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
					ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
					
					pt2d.x = tx;
					pt2d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt2d,1);

//				Envelope et = GetEnvelopeOfGraph2d(gr,m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY,gscale);
				double wt = e.Width(), ht = e.Height();
				if( cgr->bGrdSize )
				{
					//外包的计算已经考虑的m_fDrawCellKX和m_fDrawCellKY，所以不需要再乘他们
					wt *= gscale;
					ht *= gscale;
				}
				if (wt<2&&ht<2)
				{
					pDC->Point(pt2d.x,pt2d.y,0,fontColor,wt*0.5);
					continue;
				}
				if ((wt<3||ht<3))
				{
					PT_4D rectpts[4];
					if( bUseMatrix )
					{
						for( int i=0; i<4; i++)
						{
							pt2d.x = cgr->pt.x + cgr->x[i]+(double)xoff; 
							pt2d.y = cgr->pt.y + cgr->y[i]+(double)yoff;

							rectpts[i].x = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
							rectpts[i].y = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
						}
					}
					else
					{
						for( int i=0; i<4; i++)
						{
							rectpts[i].x = cgr->pt.x + cgr->x[i]+(double)xoff; 
							rectpts[i].y = cgr->pt.y + cgr->y[i]+(double)yoff;
						}

						if( pCS )pCS->GroundToClient(rectpts,4);
					}
					pDC->Begin(2);
					pDC->SetWidth(ht*0.5);
					pDC->SetColor(fontColor);
					PT_4D rectpts2[4];
					rectpts2[0].x = (rectpts[0].x + rectpts[1].x)*0.5;	rectpts2[0].y = (rectpts[0].y + rectpts[1].y)*0.5;
					rectpts2[1].x = (rectpts[1].x + rectpts[2].x)*0.5;	rectpts2[1].y = (rectpts[1].y + rectpts[2].y)*0.5;
					rectpts2[2].x = (rectpts[2].x + rectpts[3].x)*0.5;	rectpts2[2].y = (rectpts[2].y + rectpts[3].y)*0.5;
					rectpts2[3].x = (rectpts[3].x + rectpts[0].x)*0.5;	rectpts2[3].y = (rectpts[3].y + rectpts[0].y)*0.5;
					pDC->MoveTo(rectpts2[1].x,rectpts2[1].y,0);
					pDC->LineTo(rectpts2[3].x,rectpts2[3].y,0);
					pDC->End();
					continue;
				}

				TextSettings settings;
				memcpy(&settings,&(cgr->settings),sizeof(TextSettings));
				settings.fHeight = fh;	
				if (cgr->bRotateWithView) settings.fTextAngle += m_fDrawCellAngle*180/PI;
				
				pDC->SetTextSettings(&settings);
				pDC->DrawText(cgr->text,pt2d.x,pt2d.y,fontColor,m_clrBack,!m_bStereo,m_bStereo);		
			}
		}
	}

	g_Fontlib.ClearDrawGraphics();

	TRACE("draw grptcount %d, grlinecount %d, ptcount %d\n",grptcount,grlinecount,ptcount);
}

void CDrawingContext::DrawGrBuffer_Base(CSuperDrawDC *pDC, GrElementList *pList, int mode, double lfWidthRatio)
{
	CRect rcView = m_rcView;

	Envelope e0(rcView.left,rcView.right,rcView.top,rcView.bottom),e;

	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	CellDef cell;
	BaseLineType linetype; 

	CCoordSys *pCS = GetCoordSys();	

	//大地坐标转换到客户坐标的缩放系数
	float gscale = 1.0;
	if( pCS )gscale = pCS->CalcScale();

	gbUseTrimDrawLine = gscale>1e+4?TRUE:FALSE;

	//直接使用 matrix，速度会更快
	double m[16], tx, ty, tz;
	BOOL bUseMatrix = FALSE;
	if( pCS!=NULL && pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		bUseMatrix = TRUE;
		pCS->GetMatrix(m);
	}
	
	Envelope e1 = e0;
	if( pCS )e1.TransformCltToGrd(pCS);

	const Graph *gr;
	PT_4D pt4d;

	GrElement *ele = pList->head;
	for( ; ele!=NULL; ele=ele->next )
	{
		if( !ele->bvisible )
			continue;
		
		GrBuffer *pBuf= (GrBuffer*)ele->pBuf;
		
		gr = pBuf->HeadGraph();
		
		for( ; gr!=NULL; gr=gr->next )
		{
			BOOL bConvert_e = FALSE;
			e = GetEnvelopeOfGraph_fast(gr,m_fDrawCellAngle,1,1,gscale);

			//直接比较，速度略快
			if( bUseMatrix && fabs(m[2])<1e-10 && fabs(m[6])<1e-10 )
			{
				if( !e.bIntersect(&e1) && !pDC->IsEnableUseOpSet() )continue;
			}
			else
			{
				if( pCS )e.TransformGrdToClt(pCS,1);

				if( !e.bIntersect(&e0) && !pDC->IsEnableUseOpSet() )continue;

				bConvert_e = TRUE;
			}

			DWORD dcolor = gr->bUseBackColor?GetBackColor():GetDrawColor(gr->color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);

			if( IsGrPoint(gr) )
			{
				const GrPoint *cgr = (const GrPoint*)gr;

				pt4d = cgr->pt;
				
				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
					ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
					pt4d.x = tx;
					pt4d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt4d,1);
				
				//图元块
				if( cgr->cell>0 && pCellLib!=NULL && cgr->cell<=pCellLib->GetCellDefCount() )
				{
					pDC->SetColor(dcolor);
					
					cell = pCellLib->GetCellDef(cgr->cell-1);
					if( cell.m_pgr!=NULL )
					{
						Envelope et = e;
						double wt = et.Width(), ht = et.Height();
						if( !bConvert_e && cgr->bGrdSize )
						{
							wt *= gscale; ht *= gscale;
						}
						if( wt<3.0 && ht<3.0 )
						{
							pDC->Point(pt4d.x,pt4d.y,0,dcolor,(wt+ht)*0.5);
						}
						else if( !pDC->IsEnableUseOpSet() )
						{							
							//使用已经生成的 DrawOpSet，直接绘制
							if( m_PointDrawOpSet.CompareCell(cgr->cell,dcolor,cgr->kx,cgr->ky,cgr->angle,cgr->width,cgr->bGrdSize) )
							{
								m_PointDrawOpSet.MoveToOpSetXY(pt4d.x,pt4d.y,pt4d.z);
								pDC->DrawSet(&m_PointDrawOpSet);
							}
							else
							{			
								//生成 DrawOpSet
								double m1[16],m2[16],m3[16];
								
								Matrix44FromZoom(cgr->kx,cgr->ky,1,m1);
								Matrix44FromRotate(&PT_3D(),cgr->angle,m2);
								matrix_multiply(m2,m1,4,m3);
								
								Matrix44FromMove(cgr->pt.x,cgr->pt.y,cgr->pt.z,m2);
								matrix_multiply(m2,m3,4,m1);
								
								GrBuffer buf;
								buf.AddBuffer(cell.m_pgr,0);
								
								buf.Transform(m1);
								buf.ZoomCompressInfo(max(cgr->kx,cgr->ky));	
								buf.ZoomPointSize(cgr->kx,cgr->ky);
								
								buf.SetAllBackColor(gr->bUseBackColor);
								buf.SetAllColor(dcolor);
								buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);
								
								m_PointDrawOpSet.ClearAll();
								pDC->EnableUseOpSet(TRUE,&m_PointDrawOpSet);
								
								DrawGrBuffer_Base(pDC,&GrElementList(&buf),mode,lfWidthRatio);
								
								pDC->EnableUseOpSet(FALSE,NULL);

								m_PointDrawOpSet.SetCellInfo(cgr->cell,dcolor,cgr->kx,cgr->ky,cgr->angle,cgr->width,cgr->bGrdSize);
								m_PointDrawOpSet.SetCellPt(pt4d.x,pt4d.y,pt4d.z);

								//绘制 DrawOpSet
								pDC->DrawSet(&m_PointDrawOpSet);
							}

						}
						else
						{
							GrBuffer buf;
							buf.AddBuffer(cell.m_pgr,0);

							//Transform 应该会更慢，应减少其调用，以下代码改进速度
							double m1[16],m2[16],m3[16];
							Matrix44FromZoom(cgr->kx,cgr->ky,1,m1);
							Matrix44FromRotate(&PT_3D(),(double)cgr->angle,m2);
							matrix_multiply(m2,m1,4,m3);

							// 不需要考虑视图的旋转和拉伸
							pt4d = cgr->pt;
							Matrix44FromMove(pt4d.x,pt4d.y,pt4d.z,m2);
							matrix_multiply(m2,m3,4,m1);
							
							buf.Transform(m1);
							buf.ZoomPointSize(cgr->kx,cgr->ky);
							buf.ZoomCompressInfo(max(cgr->kx,cgr->ky));
							
							buf.SetAllBackColor(gr->bUseBackColor);
							buf.SetAllColor(dcolor);
							buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);

							DrawGrBuffer_Base(pDC,&GrElementList(&buf),mode);
						}
					}
				}
				//缺省处理行为
				else
				{
					double wid = cgr->kx;
					if( cgr->bGrdSize )
						wid = cgr->kx*gscale;

					pDC->Point(pt4d.x,pt4d.y,0,dcolor,wid,cgr->bFlat);
				}
			}
			else if( IsGrPointString(gr) || IsGrPointStringEx(gr) )
			{
				const GrPointString *cgr = (const GrPointString*)gr;

				const GrPointStringEx *cgrEx = NULL;
				if( !gr->bUseBackColor && IsGrPointStringEx(gr) )
				{
					cgrEx = (const GrPointStringEx*)gr;
				}
				
				//图元块
				if( cgr->cell>0 && pCellLib!=NULL && cgr->cell<=pCellLib->GetCellDefCount() && cgr->ptlist.nuse>0 )
				{
					pDC->SetColor(dcolor);
					
					cell = pCellLib->GetCellDef(cgr->cell-1);
					if( cell.m_pgr!=NULL )
					{
						Envelope et = GetEnvelopeOfPointInString_fast(gr,m_fDrawCellAngle,1,1);

						double wt = et.Width(), ht = et.Height();
						if( cgr->bGrdSize )
						{
							wt *= gscale; ht *= gscale;
						}
						if( wt<3.0 && ht<3.0 )
						{
							double wid = (wt+ht)*0.5*lfWidthRatio;

							BOOL bBatchPoint = pDC->CanBatchPoint();
							if( bBatchPoint )
								pDC->StartBatchPoint(dcolor,wid,cgr->bFlat);

							Envelope e2 = e0;
							e2.Inflate(wid,wid,0);

							for(int i=0; i<cgr->ptlist.nuse; i++)
							{
								pt4d = (PT_3D)cgr->ptlist.pts[i];
								
								//直接使用矩阵，速度更快
								if( bUseMatrix )
								{
									tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
									ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
									pt4d.x = tx;
									pt4d.y = ty;
								}
								else if( pCS )
									pCS->GroundToClient(&pt4d,1);

								//至少打一个点
								if( i==0 || e2.bPtIn(&pt4d) )
								{
									if( cgrEx )
									{
										dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
									}

									pDC->Point(pt4d.x,pt4d.y,0,dcolor,wid);
								}
							}

							if( bBatchPoint )
							{
								pDC->EndBatchPoint();
							}
						}
						//使用 DrawOpCache，减少重复的坐标换算
						else if( !pDC->IsEnableUseOpSet() && cgr->ptlist.nuse>2 )
						{
							m_PointStringDrawOpSet.ClearAll();

							//先用第一个点生成 chache
							{

								double m1[16],m2[16],m3[16];

								Matrix44FromZoom(cgr->kx,cgr->ky,1,m1);
								Matrix44FromRotate(&PT_3D(),cgr->ptlist.pts[0].CodeToFloat(),m2);
								matrix_multiply(m2,m1,4,m3);
								
								pt4d = (PT_3D)cgr->ptlist.pts[0];								
								
								Matrix44FromMove(pt4d.x,pt4d.y,pt4d.z,m2);
								matrix_multiply(m2,m3,4,m1);
								
								GrBuffer buf;
								buf.AddBuffer(cell.m_pgr,0);
								
								buf.Transform(m1);
								buf.ZoomPointSize(cgr->kx,cgr->ky);
								buf.ZoomCompressInfo(max(cgr->kx,cgr->ky));	

								if( cgrEx )
								{
									dcolor = GetDrawColor(cgrEx->attrList.pts[0].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
								}
								
								buf.SetAllBackColor(gr->bUseBackColor);
								buf.SetAllColor(dcolor);
								buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);
								
								pDC->EnableUseOpSet(TRUE,&m_PointStringDrawOpSet);

								DrawGrBuffer_Base(pDC,&GrElementList(&buf),mode,lfWidthRatio);

								pDC->EnableUseOpSet(FALSE,NULL);
							}

							//绘制第一个点
							pDC->DrawSet(&m_PointStringDrawOpSet);

							PT_3D tpt0, tpti;
							if( pCS )
								pCS->GroundToClient(&cgr->ptlist.pts[0],&tpt0);
							else
								tpt0 = cgr->ptlist.pts[0];

							m_PointStringDrawOpSet.SetCellPt(tpt0.x,tpt0.y,tpt0.z);

							Envelope e2 = GetEnvelopeOfGraph_fast(&cgr->GetPoint(0));
							if( pCS )e2.TransformGrdToClt(pCS,1);

							Envelope e3;

							//绘制其他点
							for(int i=1; i<cgr->ptlist.nuse; i++)
							{	
								pt4d = (PT_3D)cgr->ptlist.pts[i];		
								//相同的cell直接用已有的范围平移，得到新点的范围，更快
								if( bUseMatrix )
								{
									tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
									ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
									pt4d.x = tx;
									pt4d.y = ty;
								}
								else if( pCS )
									pCS->GroundToClient(&pt4d,1);

								tpti = pt4d;

								e3 = e2;
								e3.m_xl += (tpti.x-tpt0.x);
								e3.m_xh += (tpti.x-tpt0.x);
								e3.m_yl += (tpti.y-tpt0.y);
								e3.m_yh += (tpti.y-tpt0.y);

								if( !e3.bIntersect(&e0) )continue;

								//code中存储的是旋转角
								if( cgr->ptlist.pts[i].code==cgr->ptlist.pts[0].code )
								{
									m_PointStringDrawOpSet.MoveToOpSetXY(tpti.x,tpti.y,tpti.z);	

									if( cgrEx )
									{
										dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
										m_PointStringDrawOpSet.SetAllColor(dcolor);
									}

									tpt0 = tpti;
									e2 = e3;
									pDC->DrawSet(&m_PointStringDrawOpSet);
								}
								//旋转角不一样，就单独绘制
								else
								{
									double m1[16],m2[16],m3[16];
									
									Matrix44FromZoom(cgr->kx,cgr->ky,1,m1);
									Matrix44FromRotate(&PT_3D(),cgr->ptlist.pts[i].CodeToFloat(),m2);
									matrix_multiply(m2,m1,4,m3);
									
									pt4d = (PT_3D)cgr->ptlist.pts[i];								
									
									Matrix44FromMove(pt4d.x,pt4d.y,pt4d.z,m2);
									matrix_multiply(m2,m3,4,m1);
									
									GrBuffer buf;
									buf.AddBuffer(cell.m_pgr,0);
									
									buf.Transform(m1);
									buf.ZoomPointSize(cgr->kx,cgr->ky);
									buf.ZoomCompressInfo(max(cgr->kx,cgr->ky));	

									if( cgrEx )
									{
										dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
									}
									
									buf.SetAllBackColor(gr->bUseBackColor);
									buf.SetAllColor(dcolor);
									buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);

									DrawGrBuffer_Base(pDC,&GrElementList(&buf),mode,lfWidthRatio);
								}
							}
						}
						else
						{
							double m1[16],m2[16],m3[16];
							GrBuffer allBuf, buf0;

							//不需要考虑视图的旋转和拉伸
							for(int i=0; i<cgr->ptlist.nuse; i++)
							{
								if( i>0 && cgr->ptlist.pts[i].code==cgr->ptlist.pts[i-1].code )
								{
									PT_3D pt4d0 = (PT_3D)cgr->ptlist.pts[i-1];
									pt4d = (PT_3D)cgr->ptlist.pts[i];
									
									buf0.Move(pt4d.x-pt4d0.x,pt4d.y-pt4d0.y,pt4d.z-pt4d0.z);

									allBuf.AddBuffer(&buf0);
								}
								else
								{
									Matrix44FromZoom(cgr->kx,cgr->ky,1,m1);
									Matrix44FromRotate(&PT_3D(),cgr->ptlist.pts[i].CodeToFloat(),m2);
									matrix_multiply(m2,m1,4,m3);
									
									pt4d = (PT_3D)cgr->ptlist.pts[i];								
																		
									Matrix44FromMove(pt4d.x,pt4d.y,pt4d.z,m2);
									matrix_multiply(m2,m3,4,m1);

									GrBuffer buf;
									buf.AddBuffer(cell.m_pgr,0);
									
									buf.Transform(m1);
									buf.ZoomPointSize(cgr->kx,cgr->ky);
									buf.ZoomCompressInfo(max(cgr->kx,cgr->ky));

									if( cgrEx )
									{
										dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
									}

									buf.SetAllBackColor(gr->bUseBackColor);
									buf.SetAllColor(dcolor);
									buf.SetAllLineWidthOrWidthScale(TRUE,cgr->width);

									buf0.CopyFrom(&buf);
									
									allBuf.AddBuffer(&buf);
								}
							}
							
							DrawGrBuffer_Base(pDC,&GrElementList(&allBuf),mode,lfWidthRatio);
						}
					}
				}
				//缺省处理行为
				else if( cgr->ptlist.nuse>0 )
				{
					pDC->SetColor(dcolor);

					double wid = cgr->kx;
					if( cgr->bGrdSize )
						wid = cgr->kx*gscale;

					wid *= lfWidthRatio;

					Envelope e2 = e0;
					e2.Inflate(wid,wid,0);

					BOOL bBatchPoint = pDC->CanBatchPoint();
					if( bBatchPoint )
						pDC->StartBatchPoint(dcolor,wid,cgr->bFlat);

					for(int i=0; i<cgr->ptlist.nuse; i++)
					{
						pt4d = (PT_3D)cgr->ptlist.pts[i];
						
						//直接使用矩阵，速度更快
						if( bUseMatrix )
						{
							tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
							ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
							pt4d.x = tx;
							pt4d.y = ty;
						}
						else if( pCS )
							pCS->GroundToClient(&pt4d,1);

						//至少打一个点
						if( i==0 || e2.bPtIn(&pt4d) )
						{
							if( cgrEx )
							{
								dcolor = GetDrawColor(cgrEx->attrList.pts[i].color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);
							}
							pDC->Point(pt4d.x,pt4d.y,0,dcolor,wid,cgr->bFlat);
						}
					}

					if( bBatchPoint )
					{
						pDC->EndBatchPoint();
					}
				}
			}
			else if( IsGrLineString(gr) )
			{
				const GrLineString *cgr = (const GrLineString*)gr;

				pDC->Begin(cgr->ptlist.nuse);

				pDC->SetColor(dcolor);

				float lfWidth = 0;
				if (m_bPrint)
				{
					// 线宽为 0 的按默认线宽打印
					if( !cgr->bGrdWid )
					{
						lfWidth = cgr->width*lfWidthRatio;
						if (fabs(lfWidth) < 1e-4)
						{
							lfWidth = m_fDefautPrintWidth*gscale*lfWidthRatio;
						}						
						pDC->SetWidth(lfWidth);
					}
					else 
					{
						lfWidth = cgr->width*gscale*lfWidthRatio;
						if (fabs(lfWidth) < 1e-4)
						{
							lfWidth = m_fDefautPrintWidth*gscale*lfWidthRatio;
						}
						pDC->SetWidth(lfWidth);
					}
					
					if (m_bNoLineWeight) pDC->SetWidth(m_fDefautPrintWidth*gscale*lfWidthRatio);
				}
				else
				{
					lfWidth = cgr->width;
					if( !cgr->bGrdWid )
					{
						lfWidth = cgr->width*lfWidthRatio;
						if (lfWidth<lfWidthRatio)
						{
							lfWidth = lfWidthRatio;
						}
						pDC->SetWidth(lfWidth);
					}
					else
					{
						lfWidth = cgr->width*gscale*lfWidthRatio;
						if (lfWidth<lfWidthRatio)
						{
							lfWidth = lfWidthRatio;
						}
						pDC->SetWidth(lfWidth);
					}

					if (m_bNoLineWeight) pDC->SetWidth(0);
				}

				//设置线型
				if( cgr->style>0 && pLineLib!=NULL && cgr->style<=pLineLib->GetBaseLineTypeCount() )
				{
					linetype = pLineLib->GetBaseLineType(cgr->style-1);

					DWORD nStyle = 0;
					float style_len = 0;
					DWORD styles[8] = {0};

					nStyle = linetype.m_nNum;
					for( int i=0; i<linetype.m_nNum; i++)
					{
						float len = fabs(linetype.m_fLens[i])*gscale*cgr->fDrawScale;
						if (fabs(len-int(len)) >= 0.5)
						{
							styles[i] = ceilf(len);
						}
						else
							styles[i] = floorf(len);
						
						style_len += len;
					}

					//空线
					if(style_len==0.0f)
						continue;

					pDC->SetExtPen(nStyle,styles);
				}
				else
					pDC->SetExtPen(0,0);

				double len_limit = 1.0/gscale;
				
				//先裁剪
				int *moveto_flags = NULL;
				moveto_flags = new int[cgr->ptlist.nuse];
				memset(moveto_flags,0,sizeof(int)*cgr->ptlist.nuse);

				if( bUseMatrix && fabs(m[2])<1e-10 && fabs(m[6])<1e-10 && !pDC->IsEnableUseOpSet() )
				{
					int i, n = cgr->ptlist.nuse;					

					moveto_flags[0] = 1;
					Envelope e2;

					GrVertex *p = cgr->ptlist.pts+1;
					for( i=1; i<n; i++,p++ )
					{
						if( (p->code&1)==GRBUFFER_PTCODE_LINETO )
						{
							int i0 = i-1;
							e2.m_xl = min((cgr->ptlist.pts+i0)->x,(cgr->ptlist.pts+i)->x); 
							e2.m_xh = max((cgr->ptlist.pts+i0)->x,(cgr->ptlist.pts+i)->x);
							e2.m_yl = min((cgr->ptlist.pts+i0)->y,(cgr->ptlist.pts+i)->y); 
							e2.m_yh = max((cgr->ptlist.pts+i0)->y,(cgr->ptlist.pts+i)->y);

							e2.Inflate(lfWidth*2,lfWidth*2,0);
							
							if( !e2.bIntersect(&e1) )
							{
								moveto_flags[i] = 1;
							}
						}
					}
				}
				
				//画线
				{
					int i, n = cgr->ptlist.nuse;

					GrVertex *p = cgr->ptlist.pts+1;
					PT_3DEX expt;
					PT_4D pt2d, pt2d0;						

					SimpleStreamCompress<PT_3DEX> comp;
					comp.BeginCompress(len_limit);

					SSC_AddStreamPt(comp,*(p-1));

					int start = 1, ndraw = 0, candraw = 0;
					BOOL bSkip = FALSE, bUseClip = (!pDC->IsEnableUseOpSet());
					double dx_all = 0, dy_all = 0;

					for( i=1; i<n; i++,p++ )
					{
						bSkip = FALSE;

						if( bSkip || (p->code&1)==GRBUFFER_PTCODE_MOVETO || moveto_flags[i] )
						{
			draw_over:
							comp.GetCurCompPt(expt);

							if( start==0 )
							{
								pt2d.x = expt.x;
								pt2d.y = expt.y;
								pt2d.z = expt.z;
								
								//直接使用矩阵，速度更快
								if( bUseMatrix )
								{
									tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
									ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
									
									pt2d.x = tx;
									pt2d.y = ty;
								}
								else
									pCS->GroundToClient(&pt2d,1);
								
								pDC->LineTo(pt2d.x,pt2d.y);
								ndraw++;

								if( !candraw )
								{
									dx_all += fabs(pt2d.x-pt2d0.x);
									dy_all += fabs(pt2d.y-pt2d0.y);
									if( dx_all>1.0 || dy_all>1.0 )candraw = 1;
								}

								pt2d0 = pt2d;
							}
							
							comp.BeginCompress(len_limit);

							SSC_AddStreamPt(comp,*p);

							start = 1;
						}
						else
						{
							if( SSC_AddStreamPt(comp,*p)==2 )
							{
								comp.GetLastCompPt(expt);

								pt2d.x = expt.x;
								pt2d.y = expt.y;
								pt2d.z = expt.z;
								
								//直接使用矩阵，速度更快
								if( bUseMatrix )
								{
									tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
									ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
									
									pt2d.x = tx;
									pt2d.y = ty;
								}
								else
									pCS->GroundToClient(&pt2d,1);

								if( start )
									pDC->MoveTo(pt2d.x,pt2d.y);
								else
								{
									pDC->LineTo(pt2d.x,pt2d.y);
									ndraw++;
								}	
								
								if( !candraw && !start )
								{
									dx_all += fabs(pt2d.x-pt2d0.x);
									dy_all += fabs(pt2d.y-pt2d0.y);
									if( dx_all>1.0 || dy_all>1.0 )candraw = 1;
								}
								
								pt2d0 = pt2d;
							}
							start = 0;
						}

						if( i==(n-1) )
						{
							i = n;
							goto draw_over;
						}
					}

					pDC->End();

					//太小，成为一个点
					if( !candraw && ndraw==1 && n>0 )
					{
						pt4d.x = cgr->ptlist.pts[0].x;
						pt4d.y = cgr->ptlist.pts[0].y;
						pt4d.z = cgr->ptlist.pts[0].z;
						
						//直接使用矩阵，速度更快
						if( bUseMatrix )
						{
							tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
							ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
							pt4d.x = tx;
							pt4d.y = ty;
						}
						else if( pCS )
							pCS->GroundToClient(&pt4d,1);								

						pDC->Point(pt4d.x,pt4d.y,0,dcolor,1,TRUE);
					}
				}

				delete[] moveto_flags;
			}
			else if( IsGrPolygon(gr) )
			{
				const GrPolygon *cgr = (const GrPolygon*)gr;

				if (cgr->bUseBackColor&&m_bStereo)
				{
					continue;
				}

				if( cgr->filltype==POLYGON_FILLTYPE_COLOR )
				{
					dcolor = ((dcolor&0xffffff)|((cgr->index&0xff)<<24));
				}

				double wt = e.Width(), ht = e.Height();
				if( !bConvert_e )
				{
					wt *= gscale; ht *= gscale;
				}

				//太小，成为一个点
				if( wt<3 && ht<3 && cgr->ptlist.nuse>0 )
				{
					pt4d.x = cgr->ptlist.pts[0].x;
					pt4d.y = cgr->ptlist.pts[0].y;
					pt4d.z = cgr->ptlist.pts[0].z;
					
					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
						ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
						pt4d.x = tx;
						pt4d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt4d,1);								
					
					pDC->Point(pt4d.x,pt4d.y,0,dcolor,(wt+ht)*0.25,TRUE);
					continue;
				}

				double len_limit = 1.0/gscale;	
				
				GrVertex *p = cgr->ptlist.pts+1, expt;
				
				SimpleStreamCompress<GrVertex> comp;
				comp.BeginCompress(len_limit);

				SSC_AddStreamPt(comp,*(p-1));
				
				PT_3D *pts = new PT_3D[cgr->ptlist.nuse];
				int *polycount = new int[cgr->ptlist.nuse];
				int npoly = 0, nPtSum = 0;

				if( pts && polycount )
				{
					memset(polycount,0,sizeof(int)*cgr->ptlist.nuse);
					
					if (cgr->ptlist.nuse > 0)
					{
						npoly++;
					}

					int n = cgr->ptlist.nuse;
					for( int i=1; i<n; i++,p++)
					{
						if( IsGrPtCodeMoveTo(cgr->ptlist.pts+i) )
						{
				draw_surface_over:
							comp.GetCurCompPt(expt);
							pt4d = (PT_3D)expt;

							//直接使用矩阵，速度更快
							if( bUseMatrix )
							{
								tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
								ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
								tz = pt4d.x*m[8] + pt4d.y*m[9] + pt4d.z*m[10] + m[11];
								pt4d.x = tx;
								pt4d.y = ty;
								pt4d.z = tz;
							}
							else if( pCS )
								pCS->GroundToClient(&pt4d,1);		
							
							pts[nPtSum].x = pt4d.x;
							pts[nPtSum].y = pt4d.y;
							polycount[npoly-1]++;
							nPtSum++;

							if (i < n)
							{
								npoly++;
								
								comp.BeginCompress(len_limit);								
								SSC_AddStreamPt(comp,*p);
							}
							
						}
						else
						{
							if( SSC_AddStreamPt(comp,*p)==2 )
							{
								comp.GetLastCompPt(expt);
								
								pt4d = (PT_3D)expt;
						
							
								//直接使用矩阵，速度更快
								if( bUseMatrix )
								{
									tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
									ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
									tz = pt4d.x*m[8] + pt4d.y*m[9] + pt4d.z*m[10] + m[11];
									pt4d.x = tx;
									pt4d.y = ty;
									pt4d.z = tz;
								}
								else if( pCS )
									pCS->GroundToClient(&pt4d,1);		
								
								pts[nPtSum].x = pt4d.x;
								pts[nPtSum].y = pt4d.y;
								polycount[npoly-1]++;
								nPtSum++;
							}
						}

						if (i == n-1)
						{
							i = n;
							goto draw_surface_over;
						}
						
					}
					
					if( polycount[npoly]>0 )npoly++;
					
					//太小，成为一个点
					if( nPtSum<=2 && cgr->ptlist.nuse>0 )
					{
						pt4d.x = cgr->ptlist.pts[0].x;
						pt4d.y = cgr->ptlist.pts[0].y;
						pt4d.z = cgr->ptlist.pts[0].z;
						
						//直接使用矩阵，速度更快
						if( bUseMatrix )
						{
							tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
							ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
							pt4d.x = tx;
							pt4d.y = ty;
						}
						else if( pCS )
							pCS->GroundToClient(&pt4d,1);								
						
						pDC->Point(pt4d.x,pt4d.y,0,dcolor,1,TRUE);
					}
					else
					{
						if( !m_bNoHatch )
						{
							if( npoly>1 )
								pDC->PolyPolygon(pts,polycount,npoly,nPtSum,dcolor);
							else if( npoly==1 && polycount[0]>=3 )
							pDC->Polygon(pts,polycount[0],dcolor);
						}

						PT_3D *ptt = pts, *ptt2 = NULL;

						//把边线绘制一遍
						pDC->Begin(nPtSum+npoly);

						pDC->SetColor(dcolor);
						pDC->SetWidth(1);

						for( int i=0; i<npoly; i++)
						{
							ptt2 = NULL;
							for( int j=0; j<polycount[i]; j++, ptt++)
							{
								if( j==0 )
								{
									pDC->MoveTo(ptt->x,ptt->y,ptt->z);
									ptt2 = ptt;
								}
								else
								{
									pDC->LineTo(ptt->x,ptt->y,ptt->z);
								}
							}

							if( ptt2!=NULL )
							{
								pDC->LineTo(ptt2->x,ptt2->y,ptt2->z);
							}
						}

						pDC->End();
					}
					
					
					delete[] pts;
					delete[] polycount;
				}

			}
			else if( IsGrText(gr) )
			{
				const GrText *cgr = (const GrText*)gr;

				COLORREF fontColor = dcolor;	
				
				float fh = cgr->settings.fHeight;				
				if( cgr->bGrdSize )
				{
					fh = fh*gscale;
				}
				
				pt4d = cgr->pt;				
				
				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
					ty = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
					pt4d.x = tx;
					pt4d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt4d,1);
				
				double wt = e.Width(), ht = e.Height();

				if( !bConvert_e )
				{
					wt = wt*gscale;
					ht = ht*gscale;
				}

				if (wt<2&&ht<2)
				{
					pDC->Point(pt4d.x,pt4d.y,0,fontColor,wt*0.5,TRUE);
					continue;
				}
				if (wt<4||ht<4)
				{
					PT_4D rectpts[4];
					if( bUseMatrix )
					{
						for( int i=0; i<4; i++)
						{
							pt4d.x = cgr->x[i]+cgr->pt.x; 
							pt4d.y = cgr->y[i]+cgr->pt.y;
							pt4d.z = cgr->pt.z;
							rectpts[i].x = pt4d.x*m[0] + pt4d.y*m[1] + pt4d.z*m[2] + m[3];
							rectpts[i].y = pt4d.x*m[4] + pt4d.y*m[5] + pt4d.z*m[6] + m[7];
						}
					}
					else
					{
						for( int i=0; i<4; i++)
						{
							rectpts[i].x = cgr->x[i]+cgr->pt.x; 
							rectpts[i].y = cgr->y[i]+cgr->pt.y;
							rectpts[i].z = cgr->pt.z;
						}
						if( pCS )pCS->GroundToClient(rectpts,4);
					}
					pDC->Begin(20);
					pDC->SetWidth(fh*0.6);
					pDC->SetColor(fontColor);
					
					if( wt>4 )
					{
						DWORD styles[2] = {3,1};
						pDC->SetExtPen(2,styles);
					}
					
					PT_4D rectpts2[2];
					rectpts2[0].x = (rectpts[0].x + rectpts[3].x)*0.5;	rectpts2[0].y = (rectpts[0].y + rectpts[3].y)*0.5;
					rectpts2[1].x = (rectpts[1].x + rectpts[2].x)*0.5;	rectpts2[1].y = (rectpts[1].y + rectpts[2].y)*0.5;
					rectpts2[1].x = rectpts2[0].x + (rectpts2[1].x-rectpts2[0].x)*0.8;
					pDC->MoveTo(rectpts2[0].x,rectpts2[0].y,0);
					pDC->LineTo(rectpts2[1].x,rectpts2[1].y,0);
					pDC->End();
					
					pDC->SetExtPen(0,0);
					
					continue;
				}
				
				TextSettings settings;
				memcpy(&settings,&(cgr->settings),sizeof(TextSettings));
				settings.fHeight = fh;
				if (cgr->bRotateWithView)   settings.fTextAngle += m_fDrawCellAngle*180/PI;				

				pDC->SetTextSettings(&settings);
				pDC->DrawText(cgr->text,pt4d.x,pt4d.y,fontColor,GetBackColor(),!m_bStereo,m_bStereo);
			}
		}
	}

	g_Fontlib.ClearDrawGraphics();
}


void CDrawingContext::DrawSelectionMark_Base(CSuperDrawDC* pDC, GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CRect rcView = m_rcView;

	float widt = wid*2;
	Envelope e0(rcView.left-widt,rcView.right+widt,rcView.top-widt,rcView.bottom+widt),e;

	CCoordSys *pCS = GetCoordSys();

	float gscale = 1.0;
	if( pCS )gscale = pCS->CalcScale();

	//直接使用 matrix，速度会更快
	double m[16], tx, ty;
	BOOL bUseMatrix = FALSE;
	if( pCS!=NULL && pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		bUseMatrix = TRUE;
		pCS->GetMatrix(m);
	}
	
	const Graph *gr;
	PT_4D pt2d;
	
	GrElement *ele = pList->head;
	
	//统计个数，判断是否需要填充
	int nCount = 0;
	BOOL bFillMark = FALSE;
	if( !m_bStereo )
	{
		bFillMark = TRUE;
		for( ; ele!=NULL; ele=ele->next )
		{
			if( pList->which_mark==1 && !ele->bmark1 )
				continue;
			
			if( pList->which_mark==2 && !ele->bmark2 )
				continue;
			
			nCount++;
			if( nCount>=100 )
			{
				bFillMark = FALSE;
				break;
			}
		}
	}
		
	ele = pList->head;
	for( ; ele!=NULL; ele=ele->next )
	{
		if( pList->which_mark==1 && !ele->bmark1 )
			continue;
		
		if( pList->which_mark==2 && !ele->bmark2 )
			continue;
		
		GrBuffer *pBuf= (GrBuffer*)ele->pBuf;
				
		gr = pBuf->HeadGraph();
		
		for( ; gr!=NULL; gr=gr->next )
		{
			e = GetEnvelopeOfGraph_fast(gr);
			if( pCS )e.TransformGrdToClt(pCS,1);

			if( !e.bIntersect(&e0) )continue;

			if( IsGrPoint(gr) )
			{
				const GrPoint *cgr = (const GrPoint*)gr;
				
				pt2d.x = cgr->pt.x;
				pt2d.y = cgr->pt.y;
				pt2d.z = cgr->pt.z;
				
				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
					ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
					
					pt2d.x = tx;
					pt2d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt2d,1);				
				
				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);
				
				
				pDC->Begin(5);
				pDC->SetWidth(1);
				pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				pDC->End();

			}
			else if( IsGrPointString(gr) || IsGrPointStringEx(gr) )
			{
				const GrPointString *cgr = (const GrPointString*)gr;

				const GrPointStringEx *cgrEx = NULL;
				if( !gr->bUseBackColor && IsGrPointStringEx(gr) )
				{
					cgrEx = (const GrPointStringEx*)gr;
				}
				
				pDC->Begin(cgr->ptlist.nuse*5);
				
				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);
				
				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					if( cgrEx )
					{
						if( !cgrEx->attrList.pts[i].isMarked )
							continue;
					}
					
					pt2d.x = cgr->ptlist.pts[i].x;
					pt2d.y = cgr->ptlist.pts[i].y;
					pt2d.z = cgr->ptlist.pts[i].z;

					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
						
						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);
					
					if( cgr->ptlist.pts[i].code&GRPT_MARK_DIAMOND )
						pDC->DrawDiamond(pt2d.x,pt2d.y,wid*2,VERTEX_DEPTH);
					else
						pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				}
				pDC->End();
			}
			else if( IsGrLineString(gr) )
			{
				const GrLineString *cgr = (const GrLineString*)gr;
				if( pCS )pCS->GroundToClient(&pt2d,1);

				pDC->Begin(cgr->ptlist.nuse*5);
				
				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);				
				
				pDC->SetWidth(1);

				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					pt2d.x = cgr->ptlist.pts[i].x;
					pt2d.y = cgr->ptlist.pts[i].y;
					pt2d.z = cgr->ptlist.pts[i].z;

					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
						
						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);			

					if( cgr->ptlist.pts[i].code&GRPT_MARK_DIAMOND )
						pDC->DrawDiamond(pt2d.x,pt2d.y,wid*2,VERTEX_DEPTH);
					else
						pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				}
				pDC->End();
			}
			else if( IsGrPolygon(gr) )
			{
				const GrPolygon *cgr = (const GrPolygon*)gr;
				if( pCS )pCS->GroundToClient(&pt2d,1);

				pDC->Begin(cgr->ptlist.nuse*5);
				
				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);
				pDC->SetWidth(1);				
				
				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					pt2d.x = cgr->ptlist.pts[i].x;
					pt2d.y = cgr->ptlist.pts[i].y;
					pt2d.z = cgr->ptlist.pts[i].z;
					
					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
						
						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);			
					
					if( cgr->ptlist.pts[i].code&GRPT_MARK_DIAMOND )
						pDC->DrawDiamond(pt2d.x,pt2d.y,wid*2,VERTEX_DEPTH);
					else
						pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				}
				pDC->End();
			}
			else if( IsGrText(gr) )
			{
				const GrText *cgr = (const GrText*)gr;
				
				pt2d.x = cgr->pt.x;
				pt2d.y = cgr->pt.y;
				pt2d.z = cgr->pt.z;
				
				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
					ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
					
					pt2d.x = tx;
					pt2d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt2d,1);
				
				pDC->Begin(5);

				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);
				pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				pDC->End();
			}
		}
	}
}


void CDrawingContext::DrawSelectionMark2d_Base(CSuperDrawDC* pDC, GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CRect rcView = m_rcView;

	float widt = wid*2;
	Envelope e0(rcView.left-widt,rcView.right+widt,rcView.top-widt,rcView.bottom+widt),e;

	CCoordSys *pCS = GetCoordSys();

	float gscale = 1.0;
	if( pCS )gscale = pCS->CalcScale();

	//直接使用 matrix，速度会更快
	double m[16], tx, ty;
	BOOL bUseMatrix = FALSE;
	if( pCS!=NULL && pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		bUseMatrix = TRUE;
		pCS->GetMatrix(m);
	}
		
	if( pCS )e0.TransformCltToGrd(pCS);
	
	const Graph2d *gr;
	PT_4D pt2d;
	GrElement *ele = pList->head;

	//统计个数，判断是否需要填充
	int nCount = 0;
	BOOL bFillMark = FALSE;
	if( !m_bStereo )
	{
		bFillMark = TRUE;
		for( ; ele!=NULL; ele=ele->next )
		{
			if( pList->which_mark==1 && !ele->bmark1 )
				continue;
			
			if( pList->which_mark==2 && !ele->bmark2 )
				continue;
			
			nCount++;
			if( nCount>=100 )
			{
				bFillMark = FALSE;
				break;
			}
		}
	}
	
	ele = pList->head;
	for( ; ele!=NULL; ele=ele->next )
	{
		if( pList->which_mark==1 && !ele->bmark1 )
			continue;
		
		if( pList->which_mark==2 && !ele->bmark2 )
			continue;
		
		GrBuffer2d *pBuf= (GrBuffer2d*)ele->pBuf;
		
		float xoff,yoff;
		pBuf->GetOrigin(xoff,yoff);
		
		gr = pBuf->HeadGraph();
		
		for( ; gr!=NULL; gr=gr->next )
		{
			e = GetEnvelopeOfGraph2d(gr);
			e.m_xh += xoff;
			e.m_xl += xoff;
			e.m_yl += yoff;
			e.m_yh += yoff;
			
			if( max(e.m_xl,e0.m_xl)>min(e.m_xh,e0.m_xh) ||
				max(e.m_yl,e0.m_yl)>min(e.m_yh,e0.m_yh) )
				continue;

			if( IsGrPoint2d(gr) )
			{
				const GrPoint2d *cgr = (const GrPoint2d*)gr;
				
				pt2d.x = cgr->pt.x + (double)xoff;
				pt2d.y = cgr->pt.y + (double)yoff;
				
				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
					ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
					
					pt2d.x = tx;
					pt2d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt2d,1);	
				
				pDC->Begin(5);

				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);
				pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				pDC->End();

			}
			else if( IsGrPointString2d(gr) || IsGrPointString2dEx(gr) )
			{
				const GrPointString2d *cgr = (const GrPointString2d*)gr;

				const GrPointString2dEx *cgrEx = NULL;
				if( !gr->bUseBackColor && IsGrPointString2dEx(gr) )
				{
					cgrEx = (const GrPointString2dEx*)gr;
				}
				
				pDC->Begin(cgr->ptlist.nuse*5);
				
				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);
				
				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					if( cgrEx )
					{
						if( !cgrEx->attrList.pts[i].isMarked )
							continue;
					}

					pt2d.x = cgr->ptlist.pts[i].x+(double)xoff;
					pt2d.y = cgr->ptlist.pts[i].y+(double)yoff;
					
					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
						
						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);
					
					if( cgr->ptlist.pts[i].code&GRPT_MARK_DIAMOND )
						pDC->DrawDiamond(pt2d.x,pt2d.y,wid*2,VERTEX_DEPTH);
					else
						pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				}
				pDC->End();
			}
			else if( IsGrLineString2d(gr) )
			{
				const GrLineString2d *cgr = (const GrLineString2d*)gr;
				
				pDC->Begin(cgr->ptlist.nuse*5);

				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);

				pDC->SetWidth(1);
		
				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					pt2d.x = cgr->ptlist.pts[i].x+(double)xoff;
					pt2d.y = cgr->ptlist.pts[i].y+(double)yoff;

					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
						
						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);

					if( cgr->ptlist.pts[i].code&GRPT_MARK_DIAMOND )
						pDC->DrawDiamond(pt2d.x,pt2d.y,wid*2,VERTEX_DEPTH);
					else
						pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				}
				pDC->End();
			}
			else if( IsGrPolygon2d(gr) )
			{
				const GrPolygon2d *cgr = (const GrPolygon2d*)gr;
				
				pDC->Begin(cgr->ptlist.nuse*5);

				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);

				pDC->SetWidth(1);
				
				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					pt2d.x = cgr->ptlist.pts[i].x+(double)xoff;
					pt2d.y = cgr->ptlist.pts[i].y+(double)yoff;

					//直接使用矩阵，速度更快
					if( bUseMatrix )
					{
						tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
						ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
						
						pt2d.x = tx;
						pt2d.y = ty;
					}
					else if( pCS )
						pCS->GroundToClient(&pt2d,1);
					
					if( cgr->ptlist.pts[i].code&GRPT_MARK_DIAMOND )
						pDC->DrawDiamond(pt2d.x,pt2d.y,wid*2,VERTEX_DEPTH);
					else
						pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);
				}
				pDC->End();
			}
			else if( IsGrText2d(gr) )
			{
				const GrText2d *cgr = (const GrText2d*)gr;
				
				pt2d.x = cgr->pt.x + (double)xoff;
				pt2d.y = cgr->pt.y + (double)yoff;
				
				//直接使用矩阵，速度更快
				if( bUseMatrix )
				{
					tx = pt2d.x*m[0] + pt2d.y*m[1] + m[3];
					ty = pt2d.x*m[4] + pt2d.y*m[5] + m[7];
					
					pt2d.x = tx;
					pt2d.y = ty;
				}
				else if( pCS )
					pCS->GroundToClient(&pt2d,1);

				pDC->Begin(5);
				
				if( clr==(DWORD)-1 )pDC->SetColor(cgr->color);
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);
				
				pDC->MarkBox(pt2d.x-wid,pt2d.y-wid,pt2d.x+wid,pt2d.y+wid,VERTEX_DEPTH,bFillMark);

				pDC->End();
			}
		}
	}
}


void CDrawingContext::DrawGeometryMark_Base(CSuperDrawDC *pDC, GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	CRect rcView = m_rcView;

	COLORREF clr = ptInfor.GetCol(ptNone);
	float wid = ptInfor.GetWid(ptNone);

	COLORREF clrFtrPt = ptInfor.GetCol(ptSpecial);
	float widFtrPt = ptInfor.GetWid(ptSpecial);

	float widt = wid*2;
	if( widt<widFtrPt )
		widt = widFtrPt;
	
	Envelope e0(rcView.left-widt,rcView.right+widt,rcView.top-widt,rcView.bottom+widt),e;

	PT_4D pt2d;

	CCoordSys *pCS = GetCoordSys();
	float gscale = 1.0;
	if( pCS )gscale = pCS->CalcScale();

	double len_limit = 0.5/gscale;	
	
	//直接使用 matrix，速度会更快
	double m[16], tx, ty;
	BOOL bUseMatrix = FALSE;
	if( pCS!=NULL && pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		bUseMatrix = TRUE;
		pCS->GetMatrix(m);
	}

	CArray<PT_3DEX,PT_3DEX> arrPts, arrPts2;

	PT_3DEX last_pt;
	
	GrElement *ele = pList->head;
	
	//统计个数，判断是否需要填充
	int nCount = 0;
	BOOL bFillMark = FALSE;
	if( !m_bStereo )
	{
		bFillMark = TRUE;
		for( ; ele!=NULL; ele=ele->next )
		{
			if( pList->which_mark==1 && !ele->bmark1 )
				continue;
			
			if( pList->which_mark==2 && !ele->bmark2 )
				continue;
			
			nCount++;
			if( nCount>=100 )
			{
				bFillMark = FALSE;
				break;
			}
		}
	}
	
	ele = pList->head;
	for( ; ele!=NULL; ele=ele->next )
	{
		if( pList->which_mark==1 && !ele->bmark1 )
			continue;
		
		if( pList->which_mark==2 && !ele->bmark2 )
			continue;
		
		CGeometryBase *pBuf= (CGeometryBase*)ele->pBuf;

		Envelope e = pBuf->GetEnvelope();
		e.TransformGrdToClt(pCS,1);
		if( !e0.bIntersect(&e) )
			continue;

		arrPts.RemoveAll();
		pBuf->GetShape(arrPts);

		int n0 = arrPts.GetSize();

		for( int i=pBuf->GetCtrlPointSum()-1; i>=0; i--)
		{
			arrPts.Add(pBuf->GetCtrlPoint(i));
		}

		int n = arrPts.GetSize();
		PT_3DEX *pts = arrPts.GetData(), *p;
		p = pts;
		
		int last_valid = 0;
		for( i=0; i<n; i++,p++ )
		{
			if( last_valid )
			{
				if( fabs(last_pt.x-p->x)<len_limit && fabs(last_pt.y-p->y)<len_limit )
					continue;
			}

			pt2d.x = p->x;
			pt2d.y = p->y;
			pt2d.z = p->z;

			//直接使用矩阵，速度更快
			if( bUseMatrix )
			{
				tx = pt2d.x*m[0] + pt2d.y*m[1] + pt2d.z*m[2] + m[3];
				ty = pt2d.x*m[4] + pt2d.y*m[5] + pt2d.z*m[6] + m[7];
				
				pt2d.x = tx;
				pt2d.y = ty;
			}
			else
				pCS->GroundToClient(&pt2d,1);

			if( pt2d.x<e0.m_xl || pt2d.x>e0.m_xh || pt2d.y<e0.m_yl || pt2d.y>e0.m_yh )
				continue;

			if( i<n0 )
			{
				pDC->Begin(5);	

				if (p->type == ptSpecial)
				{
					pDC->SetColor(GetDrawColor(clrFtrPt,FALSE));
				}
				else if( clr==(DWORD)-1 )pDC->SetColor(GetDrawColor(pBuf->GetColor(),FALSE));
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);

				float rectwid = wid;
				
				if (p->type == ptSpecial)
				{
					rectwid = widFtrPt;
				}

				pDC->MarkBox(pt2d.x-rectwid,pt2d.y-rectwid,pt2d.x+rectwid,pt2d.y+rectwid,VERTEX_DEPTH,bFillMark);
				pDC->End();
			}
			else if( p->type!=ctrlType_Scale )
			{
				pDC->Begin(5);
				
				if( clr==(DWORD)-1 )pDC->SetColor(GetDrawColor(pBuf->GetColor(),FALSE));
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);
				
				float rectwid = wid;
				
				pDC->MarkBox(pt2d.x-rectwid,pt2d.y-rectwid,pt2d.x+rectwid,pt2d.y+rectwid,VERTEX_DEPTH,bFillMark);
				pDC->End();
			}
			//对 ctrlType_Scale 点，绘制菱形
			else
			{
				pDC->Begin(5);	
				
				if( clr==(DWORD)-1 )pDC->SetColor(GetDrawColor(pBuf->GetColor(),FALSE));
				else pDC->SetColor(clr);
				
				pDC->SetWidth(1);
				
				pDC->DrawDiamond(pt2d.x,pt2d.y,wid*2,VERTEX_DEPTH);
				pDC->End();
			}

			last_pt = pts[i];
			last_valid = 1;
		}
	}
}

void CDrawingContext::DrawGrBuffer2d_GL(GrElementList *pList, int mode)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GL,GetContext());

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);

	m_PointDrawOpSet.ClearAll();
	m_PointStringDrawOpSet.ClearAll();
	DrawGrBuffer2d_Base(&dc,pList,mode);
}



void CDrawingContext::DrawSelectionMark_GL(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GL,GetContext());
	
	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	DrawSelectionMark_Base(&dc,pList,mode,clr,wid);
}


void CDrawingContext::DrawSelectionMark2d_GL(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GL,GetContext());

	dc.SetCoordMode(FALSE,TRUE,m_szDC);

	//不需要异或模式
	if( mode&CDrawingContext::modeDynamic )
		mode = (mode^CDrawingContext::modeDynamic);

	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_COPY);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);	

	DrawSelectionMark2d_Base(&dc,pList,mode,clr,wid);
}


CCoordSys *CDrawingContext::GetCoordSys()
{
	return m_pCoordSys;
}


void CDrawingContext::SetCoordSys(CCoordSys *pCS)
{
	m_pCoordSys = pCS;
}



void CDrawingContext::BeginDrawing()
{
	
}


void CDrawingContext::EndDrawing()
{
	
}



void CDrawingContext::Scroll(int dx, int dy, CRect *pRcView, CArray<CRect,CRect>* pRects)
{
	ScrollDC(m_hOriginalDC,dx,dy,pRcView,&m_rcView,NULL,NULL);

	CRect rcDC = CRect(0,0,m_szDC.cx,m_szDC.cy);
	if( pRcView==NULL )pRcView = &rcDC;
	else *pRcView = ((*pRcView)&rcDC);
	
	CRect rcRect = *pRcView;
	CRect rcRect2 = rcRect;
	rcRect2.OffsetRect(dx,dy);
	
	CRgn rgn1, rgn2, rgn;
	rgn1.CreateRectRgnIndirect(&rcRect);
	rgn2.CreateRectRgnIndirect(&rcRect2);
	rgn.CreateRectRgn(0,0,0,0);
	
	rgn.CombineRgn(&rgn1,&rgn2,RGN_DIFF);
	
	int	num = rgn.GetRegionData(NULL,0);
	RGNDATA *pRgnData = (RGNDATA *)new BYTE[num];
	
	if( pRgnData )
	{
		rgn.GetRegionData( pRgnData,num );
		
		RECT *pRect = (RECT *)pRgnData->Buffer;
		for( int j=0;j<int(pRgnData->rdh.nCount);j++ )
		{
			pRects->Add(*pRect);
			pRect += 1;
		}
		
		delete[] pRgnData;
	}
}


void CDrawingContext::EraseBackground()
{
}


typedef struct {
	PIXELFORMATDESCRIPTOR	pfd;
	LAYERPLANEDESCRIPTOR	lpd;
} PIXELINFO;

int PixelInfoCompare(const void *elem1, const void *elem2)
{
	PIXELINFO* pi1 = (PIXELINFO*)elem1;
	PIXELINFO* pi2 = (PIXELINFO*)elem2;
	
	if (pi1->pfd.bReserved > pi2->pfd.bReserved)
		return -1;
	else if (pi1->pfd.bReserved < pi2->pfd.bReserved)
		return 1;
	else
	{
		if (pi1->pfd.cStencilBits < pi2->pfd.cStencilBits)
			return -1;
		else if (pi1->pfd.cStencilBits > pi2->pfd.cStencilBits)
			return 1;
		else
		{
			if (pi1->pfd.bReserved)	// have overlays
			{
				if ((pi1->lpd.dwFlags & LPD_DOUBLEBUFFER) && !(pi2->lpd.dwFlags & LPD_DOUBLEBUFFER))
					return -1;
				else if (!(pi1->lpd.dwFlags & LPD_DOUBLEBUFFER) && (pi2->lpd.dwFlags & LPD_DOUBLEBUFFER))
					return 1;
				else
				{
					if (pi1->pfd.cAuxBuffers < pi2->pfd.cAuxBuffers)
						return -1;
					else if (pi1->pfd.cAuxBuffers > pi2->pfd.cAuxBuffers)
						return 1;
					else
						return 0;
				}
			}
			else
			{
				if (pi1->pfd.cAuxBuffers > pi2->pfd.cAuxBuffers)
					return -1;
				else if (pi1->pfd.cAuxBuffers < pi2->pfd.cAuxBuffers)
					return 1;
				else
					return 0;
			}
		}
	}
}

//能选择更好的硬件表现
BOOL CDrawingContext::SetGlPixelFormat_new(HDC hDC, BOOL bOverlay, BOOL bAlpha, int flags)
{
	BOOL bUseOverlayOnlyWhenDoubleBufferIsSupported = TRUE;
	PIXELFORMATDESCRIPTOR	m_pfd;
	LAYERPLANEDESCRIPTOR	m_lpd;

	{
		int						pf, maxpf;
		int						nEntries = 125;	/* number of entries in palette */
		int						ret;
		PIXELFORMATDESCRIPTOR	pfd;
		LAYERPLANEDESCRIPTOR	lpd;

		/* get the maximum number of pixel formats */
		maxpf = DescribePixelFormat(hDC, 0, 0, NULL);
		PIXELINFO* pPixelInfo = new PIXELINFO[maxpf];
		int nPixelInfoCount = 0;

		/* find an overlay layer descriptor */
		for(pf = 0; pf < maxpf; pf++)
		{
			LPPIXELFORMATDESCRIPTOR pfd = &pPixelInfo[nPixelInfoCount].pfd;
			
			ret = DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), pfd);
			if (!ret)
				continue;

			if (pfd->iPixelType != PFD_TYPE_RGBA)
				continue;
			if ( flags!=(pfd->dwFlags&flags) )
				continue;

			/*
			if( 0==(pfd->dwFlags&PFD_DOUBLEBUFFER) )
				continue;

			if( 0==(pfd->dwFlags&PFD_DRAW_TO_WINDOW) )
				continue;

			if( 0==(pfd->dwFlags&PFD_SUPPORT_OPENGL) )
				continue;

			if( 0==(pfd->dwFlags&PFD_STEREO) )
				continue;*/
			
//			if (pfd->dwFlags & (PFD_NEED_PALETTE|PFD_NEED_SYSTEM_PALETTE))
//				continue;

			if( bAlpha && pfd->cAlphaBits!=8 )
				continue;

			if (pfd->cRedBits != 8 || pfd->cGreenBits != 8 || pfd->cBlueBits != 8)
				continue;

			if (bOverlay && (pfd->bReserved & 0x0f) > 0) // have overlays
			{
				if (!wglDescribeLayerPlane(hDC, pf, 1, 
					sizeof(LAYERPLANEDESCRIPTOR), &pPixelInfo[nPixelInfoCount].lpd))
					continue;
				if (bUseOverlayOnlyWhenDoubleBufferIsSupported)
				{
					if (!(pPixelInfo[nPixelInfoCount].lpd.dwFlags & LPD_DOUBLEBUFFER))
						continue;
				}
				pfd->bReserved = 1;
			}
			else
				pfd->bReserved = pfd->bReserved;

			pfd->nSize = pf;	// use this field to store pf
			nPixelInfoCount++;
		}

		//调试信息
		{
			TRACE("Selected PFs: ");
			for (int i = 0; i < nPixelInfoCount; i++)
				TRACE(" 0x%02x", pPixelInfo[i].pfd.nSize);
			TRACE("\n");
		}

		if( nPixelInfoCount==0 )
			return FALSE;

		qsort(pPixelInfo, nPixelInfoCount, sizeof(PIXELINFO), PixelInfoCompare);

		//调试信息
		{
			TRACE("Sorted PFs: ");
			for (int i = 0; i < nPixelInfoCount; i++)
				TRACE(" 0x%02x", pPixelInfo[i].pfd.nSize);
			TRACE("\n");
		}

		int i = 0;
		if (!bOverlay)
		{
			// skip pixel format which support overlays
			for ( ; i < nPixelInfoCount; i++)
			{
				if (pPixelInfo[i].pfd.bReserved)
					continue;
				break;
			}
		}

		if (i >= nPixelInfoCount)
			i = 0;

		int pixelFormat = pPixelInfo[i].pfd.nSize;
		delete[] pPixelInfo;

		TRACE("pixelFormat = %d(0x%02x)\n", pixelFormat, pixelFormat);

		ret = DescribePixelFormat(hDC, pixelFormat, sizeof(m_pfd), &m_pfd);
		if ((m_pfd.bReserved & 0x0f) > 0)
			wglDescribeLayerPlane(hDC, pixelFormat, 1, sizeof(LAYERPLANEDESCRIPTOR), &m_lpd);
		
		ret = SetPixelFormat(hDC, pixelFormat, &m_pfd);
				
		COLORREF color[2];
		color[0] = RGB(0xff, 0xff, 0xff);	// Cursor Color
		color[1] = RGB(0xff, 0xff, 0xff);	// Dragging Box Color

		/* set up the layer palette */
		ret = wglSetLayerPaletteEntries(hDC, 1, 1, 2, color);
		
		/* realize the palette */
		ret = wglRealizeLayerPalette(hDC, 1, TRUE);
	}

	return TRUE;
}

int CDrawingContext::SetGlPixelFormatOverlay(HDC hDC, BYTE type, DWORD flags)
{
	int						pf, maxpf;
	int						nEntries = 125;	/* number of entries in palette */
    PIXELFORMATDESCRIPTOR	pfd;
	PIXELFORMATDESCRIPTOR	normal_pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 
		1,                             
		PFD_SUPPORT_OPENGL|PFD_SUPPORT_GDI|PFD_DOUBLEBUFFER|PFD_STEREO,
		PFD_TYPE_RGBA,
		24, 0, 0, 0, 0, 0, 0,           
		0, 0, 0, 0, 0, 0, 0,
		0,                         
		0, 0,                      
		PFD_MAIN_PLANE,
		0,                         
		0, 0, 0                    
	};

    LAYERPLANEDESCRIPTOR	lpd;			/* layer plane descriptor */
	int						ret;
	int						ci[5] = { 0, 64, 128, 192, 255 };
	COLORREF				crEntries[256];
	LPLOGPALETTE			lpLogPalette = NULL;
	int						count = 0;
	
	lpLogPalette = (LPLOGPALETTE)new BYTE[sizeof(LOGPALETTE)+(nEntries-1)*sizeof(PALETTEENTRY)];
	lpLogPalette->palVersion    = 0x300;
	lpLogPalette->palNumEntries = nEntries;
	for( int i=0;i<5;i++ )
	{
		for( int j=0;j<5;j++ )
		{
			for( int k=0;k<5;k++ )
			{
				lpLogPalette->palPalEntry[count].peRed   = ci[i]; 
				lpLogPalette->palPalEntry[count].peGreen = ci[j]; 
				lpLogPalette->palPalEntry[count].peBlue  = ci[k]; 
				lpLogPalette->palPalEntry[count].peFlags = NULL;
				
				crEntries[count++] = RGB( ci[i], ci[j], ci[k] );
			}
		}
	}
	
	m_Palette_Overlay.CreatePalette( lpLogPalette );
	if( lpLogPalette!=NULL )
	{
		delete[] lpLogPalette;
		lpLogPalette = NULL;
	}

    /* get the maximum number of pixel formats */
    maxpf = DescribePixelFormat(hDC, 0, 0, NULL);

    /* find an overlay layer descriptor */
    for(pf = 1; pf <= maxpf; pf++)
	{
		ret = DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

		/* the bReserved field of the PIXELFORMATDESCRIPTOR contains the
		number of overlay/underlay planes */
		if (pfd.bReserved > 0) 
		{
			/* aha! This format has overlays/underlays */
			if( wglDescribeLayerPlane(hDC, pf, 1,sizeof(LAYERPLANEDESCRIPTOR), &lpd)==FALSE )
			{
				break;
			}

			if( lpd.dwFlags    & LPD_SUPPORT_OPENGL &&
//				lpd.dwFlags    & LPD_SUPPORT_GDI &&
				lpd.dwFlags    & flags &&
				lpd.dwFlags    & LPD_STEREO )
				
			{
				goto found;
			}
		}
	}

    /* couldn't find any overlay/underlay planes */
    if( pf>maxpf )
	{
		return FALSE;
	}

	pf = ChoosePixelFormat(hDC, &normal_pfd);

	if( !SetPixelFormat(hDC, pf, &normal_pfd) )
	{		
		return FALSE;
	}

    return TRUE;

found:
    /* now get the "normal" pixel format descriptor for the layer */
    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    /* set the pixel format */
    if(SetPixelFormat(hDC, pf, &pfd) == FALSE)
	{		
		return FALSE;
	}
    
	/* set up the layer palette */
    ret = wglSetLayerPaletteEntries(hDC, 1, 0, nEntries, crEntries);

    /* realize the palette */
    ret = wglRealizeLayerPalette(hDC, 1, TRUE);

    return pf;
}

BOOL CDrawingContext::CreateGlContext(BOOL bStereo, BOOL bOverlay )
{
	if( bStereo )
	{
		if( m_hglRC && (!bOverlay||m_hglRC_overlay) )return TRUE;
		
		DestroyGlContext();

		if (!SetGlPixelFormat_new(m_hOriginalDC, bOverlay, TRUE,
			PFD_DOUBLEBUFFER|PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_STEREO ))
		{
			if( !bOverlay )
			{
				if( !SetGlPixelFormat_new(m_hOriginalDC, bOverlay, TRUE,
					PFD_DOUBLEBUFFER|PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL) )
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		
		if( bOverlay )
		{
			m_hglRC_overlay = wglCreateLayerContext( m_hOriginalDC,1 );
			
			if( m_hglRC_overlay!=NULL )
			{
				CRect rect = m_rcView;
				CRect rcClient = CRect(0,0,m_szDC.cx,m_szDC.cy);
				
				CActiveGLRC changeRC(m_hOriginalDC,m_hglRC_overlay);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glViewport(rect.left,rcClient.bottom-rect.bottom,rect.Width(),rect.Height());
				gluOrtho2D(rect.left,rect.right,rcClient.bottom-rect.bottom,rcClient.bottom-rect.top);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				//glTranslatef(0.375,0.375,0.0);
			}
		}
		
		m_hglRC = wglCreateContext(m_hOriginalDC);
		
		if( m_hglRC!=NULL )
		{
			CRect rect = m_rcView;
			CRect rcClient = CRect(0,0,m_szDC.cx,m_szDC.cy);
			
			CActiveGLRC changeRC(m_hOriginalDC,m_hglRC);

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glEnableClientState(GL_VERTEX_ARRAY);

			if( CCacheGLTextData::m_bUseGPU && GPU_CheckSupported() )
			{
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_TEXTURE_2D);
				GPU_InitGL();
				glDisable(GL_DEPTH_TEST);
			}

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glViewport(rect.left,rcClient.bottom-rect.bottom,rect.Width(),rect.Height());
			gluOrtho2D(rect.left,rect.right,rcClient.bottom-rect.bottom,rcClient.bottom-rect.top);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			//glTranslatef(0.375,0.375,0.0);
			glClearColor(GetRValue(m_clrBack)/255.0,GetGValue(m_clrBack)/255.0,GetBValue(m_clrBack)/255.0,1);
//			glClearColor(0.0f,0.0f,0.0f,1.0f);
			glClearDepth(1.0f);
			
			glDrawBuffer(GL_FRONT_LEFT);
			glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
			
			glDrawBuffer(GL_FRONT_RIGHT);
			glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
		}
	}
	else
	{
		if( m_hglRC )return TRUE;
		
		DestroyGlContext();
		
		if (!SetGlPixelFormat_new(m_hOriginalDC, bOverlay, TRUE,
			PFD_DOUBLEBUFFER|PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL ))
		{		
			return FALSE;
		}
		
		m_hglRC = wglCreateContext(m_hOriginalDC);
		
		if( m_hglRC!=NULL )
		{
			wglMakeCurrent(m_hOriginalDC,m_hglRC);
		}

		CRect rect = m_rcView;
		CRect rcClient = CRect(0,0,m_szDC.cx,m_szDC.cy);		
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glEnableClientState(GL_VERTEX_ARRAY);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(rect.left,rcClient.bottom-rect.bottom,rect.Width(),rect.Height());
		gluOrtho2D(rect.left,rect.right,rcClient.bottom-rect.bottom,rcClient.bottom-rect.top);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glTranslatef(0.375,0.375,0.0);
		glClearColor(GetRValue(m_clrBack)/255.0,GetGValue(m_clrBack)/255.0,GetBValue(m_clrBack)/255.0,1);
//		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClearDepth(1.0f);
		
		glDrawBuffer(GL_FRONT_LEFT);
		glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
		
		glDrawBuffer(GL_FRONT_RIGHT);
		glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
	}

	return TRUE;
}

BOOL CDrawingContext::CreateGlContextForBmp()
{
	if( m_hglRC )return TRUE;
	
	DestroyGlContext();

	HDC hDC = m_hOriginalDC;

	CBitmap* pBitmap = CDC::FromHandle(hDC)->GetCurrentBitmap();
	if( !pBitmap )
		return FALSE;
	
	BITMAP bmInfo ;
	pBitmap->GetObject(sizeof(BITMAP), &bmInfo) ;
	
    PIXELFORMATDESCRIPTOR pfd=
    {	
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_BITMAP|
			PFD_SUPPORT_OPENGL| PFD_SUPPORT_GDI,
			PFD_TYPE_RGBA,
			(BYTE)bmInfo.bmBitsPixel,
			0,0,0,0,0,0,
			0,0,0,0,0,0,0,
			32,
			0,0,
			PFD_MAIN_PLANE,
			0,
			0,0,0
    };
    int pixelFormat=::ChoosePixelFormat(hDC,&pfd);
    ::SetPixelFormat(hDC,pixelFormat,&pfd);
		
	m_hglRC = wglCreateContext(m_hOriginalDC);
	
	if( m_hglRC!=NULL )
	{
		wglMakeCurrent(m_hOriginalDC,m_hglRC);
	}
	
	CRect rect = m_rcView;
	CRect rcClient = CRect(0,0,m_szDC.cx,m_szDC.cy);		
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(rect.left,rcClient.bottom-rect.bottom,rect.Width(),rect.Height());
	gluOrtho2D(rect.left,rect.right,rcClient.bottom-rect.bottom,rcClient.bottom-rect.top);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.375,0.375,0.0);
	glClearColor(GetRValue(m_clrBack)/255.0,GetGValue(m_clrBack)/255.0,GetBValue(m_clrBack)/255.0,1);
	//glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClearDepth(1.0f);
	
	glDrawBuffer(GL_LEFT);
	glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );

	return TRUE;
}



void CDrawingContext::DestroyGlContext()
{
	if( m_hglRC )
	{
		if( m_hglRC==wglGetCurrentContext() )
			wglMakeCurrent (NULL,NULL);
		wglDeleteContext(m_hglRC);
		m_hglRC = NULL;
	}
	
	if( m_hglRC_overlay )
	{
		wglMakeCurrent (NULL,NULL);
		wglDeleteContext(m_hglRC_overlay);
		m_hglRC_overlay = NULL;
	}	
}



void CDrawingContext::SetGlViewRect(CRect *pRect)
{
	CRect rcDC = CRect(0,0,m_szDC.cx,m_szDC.cy), rect;
	
	if( pRect==NULL )
		rect = rcDC;
	else
	{
		rect = *pRect;
	}
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(rect.left,rcDC.bottom-rect.bottom,rect.Width(),rect.Height());
	gluOrtho2D(rect.left,rect.right,rcDC.bottom-rect.bottom,rcDC.bottom-rect.top);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.375,0.375,0.0);
}


void CDrawingContext::SetGDIViewRect(CRect *pRect)
{
	if( pRect==NULL )
	{
		::SelectClipRgn(m_hOriginalDC,NULL);
		return;
	}
	
	CRect rcDC = CRect(0,0,m_szDC.cx,m_szDC.cy), rect;
	
	if( pRect==NULL )
		rect = rcDC;
	else
	{
		rect = *pRect;
	}

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);

	::SelectClipRgn(m_hOriginalDC,(HRGN)rgn);
}



void CDrawingContext::BltBmpToGlContext(HBITMAP hBmp,int x,int y,int xTar,int yTar,int nTarWidth,int nTarHeight)
{
	if( hBmp==NULL ) return;
	if( nTarWidth<=0 || nTarHeight<=0 )return;
	
	CRect rectTar(xTar,yTar,xTar+nTarWidth,yTar+nTarHeight);
	CRect rectSrc(0,0,m_szDC.cx,m_szDC.cy), rectin;
	
	rectin = rectTar&rectSrc;
	if( rectin!=rectTar )
	{
		x = rectin.left-rectTar.left;
		y = rectin.top-rectTar.top;
		xTar = rectin.left;
		yTar = rectin.top;
		nTarWidth = rectin.Width();
		nTarHeight = rectin.Height();
	}

	BITMAP src;	
	::GetObject( hBmp , sizeof(BITMAP), &src );	
	BYTE* pSrc =(BYTE*)src.bmBits;
	if( pSrc==NULL )return;
	
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));
	pSrc = pSrc+ (src.bmHeight-(y+nTarHeight))*linesize + x*src.bmBitsPixel/8;

	if( nTarWidth>src.bmWidth || nTarHeight>src.bmHeight )
	{
		nTarWidth = min(nTarWidth,src.bmWidth);
		nTarHeight = min(nTarHeight,src.bmHeight);
	}

	::glLogicOp(GL_COPY);
	
	glPixelStorei(GL_UNPACK_ROW_LENGTH,src.bmWidth);	
	glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
	glPixelZoom(1.0,1.0);
	glRasterPos2i(xTar,m_szDC.cy-yTar-nTarHeight);
	glDrawPixels(nTarWidth,nTarHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,pSrc);
}



//保存像素和恢复像素的几个工具函数
void CDrawingContext::SaveBmp_GDI(HDC hdc, int idx, CRect rect)
{
	if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
		return;

	if (rect.IsRectEmpty())
	{
		return;
	}
	
	CDC *pDC = CDC::FromHandle(hdc);
	
	if( NULL!=(HDC)m_saveData[idx].dcMemRect )
		m_saveData[idx].dcMemRect.DeleteDC();
	if( NULL!=(HBITMAP)m_saveData[idx].bmpRect )
		m_saveData[idx].bmpRect.DeleteObject();
	
	m_saveData[idx].dcMemRect.CreateCompatibleDC(pDC);
	
	int w = rect.Width();
	int h = rect.Height();
	
	m_saveData[idx].bmpRect.CreateCompatibleBitmap(pDC,w,h);
	CBitmap *pOldBmp = m_saveData[idx].dcMemRect.SelectObject(&m_saveData[idx].bmpRect);
	
	m_saveData[idx].dcMemRect.BitBlt(0,0,rect.Width(),rect.Height(),pDC,rect.left,rect.top,SRCCOPY);
	
	m_saveData[idx].dcMemRect.SelectObject(pOldBmp);
	
	m_saveData[idx].rcSaveRect = rect;
	m_saveData[idx].nSaveRectType = ImgSaveData::typeGDI;
}


void CDrawingContext::RestoreBmp_GDI(HDC hdc, int idx)
{
	if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
		return;
	
	if( m_saveData[idx].nSaveRectType==ImgSaveData::typeNone )
		return;
	
	if( NULL==(HDC)m_saveData[idx].dcMemRect || NULL==(HBITMAP)m_saveData[idx].bmpRect )
		return;
	
	CBitmap *pOldBmp = m_saveData[idx].dcMemRect.SelectObject(&m_saveData[idx].bmpRect);
	
	CDC *pDC = CDC::FromHandle(hdc);
	
	pDC->BitBlt( m_saveData[idx].rcSaveRect.left,m_saveData[idx].rcSaveRect.top,
		m_saveData[idx].rcSaveRect.Width(),m_saveData[idx].rcSaveRect.Height(),
		&m_saveData[idx].dcMemRect,0,0,SRCCOPY);
	
	m_saveData[idx].dcMemRect.SelectObject(pOldBmp);
}



//保存像素和恢复像素的几个工具函数
void CDrawingContext::SaveBmp_GL(int idx, CRect rect)
{
	if( rect.Width()<=0 || rect.Height()<=0 )
		return;
	
	if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
		return;
	
	CRect rcView, rcDC;
	CSize szDC = GetDCSize();
	rcDC = CRect(0,0,szDC.cx,szDC.cy);
	GetViewRect(rcView);
	
	rect = ((rect&rcDC)&rcView);
	
	if( !m_saveData[idx].pRectData || 
		m_saveData[idx].rcSaveRect.Width()*m_saveData[idx].rcSaveRect.Height()<rect.Width()*rect.Height() )
	{
		if( m_saveData[idx].pRectData )delete[] m_saveData[idx].pRectData;
		m_saveData[idx].pRectData = new BYTE[(rect.Width()+1)*(rect.Height()+1)*3];
		if( !m_saveData[idx].pRectData )return;
	}
	
	// Store the covered bits on gl left buffer
	glPixelStorei(GL_PACK_ROW_LENGTH,rect.Width());	
	glPixelStorei(GL_PACK_SKIP_PIXELS,0);
	glPixelStorei(GL_PACK_SKIP_ROWS,0);
	
	glReadPixels( rect.left,rcDC.bottom-rect.bottom,rect.Width(),rect.Height(),
		GL_RGB,GL_UNSIGNED_BYTE,m_saveData[idx].pRectData );
	
	m_saveData[idx].rcSaveRect = rect;
	m_saveData[idx].nSaveRectType = ImgSaveData::typeGL;
}


void CDrawingContext::RestoreBmp_GL(int idx)
{
	if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
		return;
	
	if( m_saveData[idx].nSaveRectType==ImgSaveData::typeNone )
		return;
	
	if( !m_saveData[idx].pRectData )
		return;
	
	CRect rcDC;
	CSize szDC = GetDCSize();
	rcDC = CRect(0,0,szDC.cx,szDC.cy);
	
	try
	{
		glLogicOp(GL_COPY);
		glPixelStorei(GL_UNPACK_ROW_LENGTH,m_saveData[idx].rcSaveRect.Width());
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
		glPixelZoom(1,1);
		glRasterPos2i(m_saveData[idx].rcSaveRect.left,rcDC.bottom-m_saveData[idx].rcSaveRect.bottom);
		glDrawPixels(m_saveData[idx].rcSaveRect.Width(),m_saveData[idx].rcSaveRect.Height(),
			GL_RGB,GL_UNSIGNED_BYTE,m_saveData[idx].pRectData);
	//	glFlush();
	}
	catch(...)
	{
	}
}


void CDrawingContext::ResetBmp(int idx)
{
	if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
		return;
	
	m_saveData[idx].nSaveRectType = ImgSaveData::typeNone;
}

int CDrawingContext::RequestSaveData(LONG_PTR id)
{
	int nMax = 0, nBlank = -1;
	for( int i=0; i<sizeof(m_saveDataUser)/sizeof(m_saveDataUser[0]); i++)
	{
		if( m_saveDataUser[i]==id )
			return i;

		if( m_saveDataUser[i]==0 && nBlank<0 )
		{
			nBlank = i;
		}
	}

	if( nBlank<0 )
		return -1;

	m_saveDataUser[nBlank] = id;

	return nBlank;
}



void CDrawingContext::ReleaseSaveData(int idx)
{
	if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
		return;

	m_saveDataUser[idx] = 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(C2DGDIDrawingContext, CDrawingContext)

C2DGDIDrawingContext::C2DGDIDrawingContext()
{
	m_hMemDC = NULL;
	m_hMemBmp = NULL;
	m_nBeginDrawOp = 0;
	m_hCurDC = NULL;
	m_hOldBmp = NULL;
	m_nBeginDrawOp = 0;
	m_nTurnonOp = 0;

	m_bMemOpOnly = FALSE;
	m_hTurnonOldDC = NULL;

	m_nCopyMode = SRCCOPY;
	m_hTemDC = NULL;
	m_bDrawBmpByAnyXY = TRUE;

	m_pMemGLDC = NULL;
	m_hMemGLDC = NULL;
	m_hMemGLBmp = NULL;
	m_hMemOldGLBmp = NULL;
}

C2DGDIDrawingContext::~C2DGDIDrawingContext()
{
	if( m_pMemGLDC )
	{
		delete m_pMemGLDC;
	}

	if( m_hMemGLBmp )
	{
		::SelectObject(m_hMemGLDC,m_hMemOldGLBmp);
		::DeleteObject(m_hMemGLBmp);
	}

	if( m_hOldBmp )
	{
		::SelectObject(m_hMemDC,m_hOldBmp);
	}
	
	if( m_hMemGLDC )
		::DeleteDC(m_hMemGLDC);

	TurnoffMemBmpBoard();

	if( m_hMemBmp )
		::DeleteObject(m_hMemBmp);
	if( m_hMemDC )
		::DeleteDC(m_hMemDC);

	if(m_hTemDC)
	{
		::DeleteDC(m_hTemDC);
		m_hTemDC = NULL;
	}
}


void C2DGDIDrawingContext::SetDCSize(CSize szDC)
{
	CDrawingContext::SetDCSize(szDC);
}



void C2DGDIDrawingContext::CreateMemBoard()
{
	if( m_hMemBmp!=NULL )
	{
		BITMAP bitmap;
		if(::GetObject(m_hMemBmp,sizeof(BITMAP),&bitmap)==0)
			return;
		int cx = bitmap.bmWidth;	
		int cy = bitmap.bmHeight;
		if (m_szDC.cx>cx||m_szDC.cy>cy)
		{	
			int scx = ::GetSystemMetrics(SM_CXSCREEN);
			
			int scy = ::GetSystemMetrics(SM_CYSCREEN);
			if (m_szDC.cx<scx&&m_szDC.cy<scy)
			{
				if(m_hMemBmp)::DeleteObject(m_hMemBmp);
				m_hMemBmp = CreateMemBmpBoard(scx,scy);
			}
			else
			{
				if(m_hMemBmp)::DeleteObject(m_hMemBmp);
				m_hMemBmp = CreateMemBmpBoard(m_szDC.cx,m_szDC.cy);
			}
		}
	}
	else
	{	
		int scx = ::GetSystemMetrics(SM_CXSCREEN);
		
		int scy = ::GetSystemMetrics(SM_CYSCREEN);
		if (m_szDC.cx<scx&&m_szDC.cy<scy)
		{
			m_hMemBmp = CreateMemBmpBoard(scx,scy);
		}
		else
		{
			m_hMemBmp = CreateMemBmpBoard(m_szDC.cx,m_szDC.cy);
		}
	}

	if( m_hMemDC==NULL )
		m_hMemDC = ::CreateCompatibleDC(NULL);
}

BOOL C2DGDIDrawingContext::CreateContext(HDC hDC)
{
	CDrawingContext::CreateContext(hDC);

	m_hCurDC = m_hOriginalDC;

	return TRUE;
}


HDC C2DGDIDrawingContext::GetContext()
{
	return m_hCurDC;
}


HDC C2DGDIDrawingContext::GetOriginalDC(HDC hDC)
{
	return m_hOriginalDC;
}


HDC C2DGDIDrawingContext::SetDrawingDC(HDC hDC)
{
	HDC hOldDC = m_hCurDC;
	m_hCurDC = hDC;
	return hOldDC;
}


void C2DGDIDrawingContext::SetAsMemOpOnly(BOOL bYes)
{
	m_bMemOpOnly = bYes;
}

void C2DGDIDrawingContext::TurnonMemBmpBoard()
{
	if( m_nTurnonOp==0 )
	{
		CreateMemBoard();
		if( m_hMemDC!=NULL )
		{
			m_hOldBmp = (HBITMAP)::SelectObject(m_hMemDC,m_hMemBmp);
			m_hTurnonOldDC = m_hCurDC;
			m_hCurDC = m_hMemDC;
		}
	}
	m_nTurnonOp++;
}

void C2DGDIDrawingContext::TurnoffMemBmpBoard()
{
	if( m_nTurnonOp==1 )
	{
		if( m_hMemDC!=NULL )
		{
			::SelectObject(m_hMemDC,m_hOldBmp);	
			m_hCurDC = m_hTurnonOldDC;
		}
	}
	m_nTurnonOp--;
}

void C2DGDIDrawingContext::BeginDrawing()
{
	if( m_nBeginDrawOp==0 )
	{
	}
	m_nBeginDrawOp++;
}

void C2DGDIDrawingContext::EndDrawing()
{
	if( m_nBeginDrawOp==1 )
	{
		if( !m_bMemOpOnly )
		{
			::BitBlt(m_hCurDC,m_rcView.left,m_rcView.top,m_rcView.Width(),m_rcView.Height(),
				m_hMemDC,m_rcView.left,m_rcView.top,m_nCopyMode);
		}
	}
	m_nBeginDrawOp--;
}

//创建内存位图画板，用于优化贴图操作
HBITMAP C2DGDIDrawingContext::CreateMemBmpBoard(int w, int h)
{
	return Create24BitDIB(w,h,TRUE,m_clrBack);
}

//获得内存位图画板
HBITMAP C2DGDIDrawingContext::GetMemBoardBmp()
{
	CreateMemBoard();
	return m_hMemBmp;
}


HDC C2DGDIDrawingContext::GetMemBoardDC()
{
	return m_hMemDC;
}


void C2DGDIDrawingContext::BeginDrawTempBmp(HBITMAP hBmp, Envelope e)
{
	CDrawingContext::BeginDrawTempBmp(hBmp,e);
	if (m_hTemDC)
	{
		::DeleteDC(m_hTemDC);
	}
	m_eClient = e;
	m_hTemDC = ::CreateCompatibleDC(NULL);
	m_hOldTemBitmap = (HBITMAP)::SelectObject(m_hTemDC, hBmp);
	CRect rect(0,0,FixPreciError(e.Width()),FixPreciError(e.Height()));
	HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(m_clrBack);
	::FillRect(m_hTemDC,&rect,hbrush);
	DeleteObject(hbrush);
/*	m_hOldTemBitmap = */(HBITMAP)::SelectObject(m_hTemDC, m_hOldTemBitmap);
}
void C2DGDIDrawingContext::DrawTempBmp(HBITMAP hBmp, CRect rcBmp, double x[4], double y[4], BOOL bUsetransColer , COLORREF transColor)
{
	if (!m_hTemDC||!m_hTempBmp)
	{
		return;
	}
	BITMAP bm;
	if(::GetObject(hBmp,sizeof(BITMAP),&bm)==0)
		return;
	Envelope temp;
	PT_3D pts[4];
	pts[0].x = x[0];pts[0].y = y[0];pts[0].z = 0;
	pts[1].x = x[1];pts[1].y = y[1];pts[1].z = 0;
	pts[2].x = x[2];pts[2].y = y[2];pts[2].z = 0;
	pts[3].x = x[3];pts[3].y = y[3];pts[3].z = 0;
	temp.CreateFromPts(pts,4);
	temp.Intersect(&m_eClient);
	HDC MemDC = ::CreateCompatibleDC(NULL);
	if(!MemDC)return;
	HBITMAP oldbitmap = (HBITMAP)::SelectObject(MemDC,hBmp);
	int w = FixPreciError(temp.m_xh)-FixPreciError(temp.m_xl);
	int h = FixPreciError(temp.m_yh)-FixPreciError(temp.m_yl);
	HBITMAP bitmap0 = (HBITMAP)::Create24BitDIB(w,h);
	HBITMAP oldbitmap0 = (HBITMAP)::SelectObject(m_hTemDC,bitmap0);
	::SetStretchBltMode(m_hTemDC,STRETCH_DELETESCANS);
	::StretchBlt(m_hTemDC,0,0, w,h,MemDC,0,0,bm.bmWidth, bm.bmHeight, SRCCOPY);
	::SelectObject(MemDC, oldbitmap);
	::DeleteDC(MemDC);
	::SelectObject(m_hTemDC,oldbitmap0);
	OverlapBitmap(bitmap0,NULL,CRect(0,0,w, h),m_hTempBmp,NULL,CRect(FixPreciError(temp.m_xl-m_eClient.m_xl),FixPreciError(temp.m_yl-m_eClient.m_yl),FixPreciError(temp.m_xh-m_eClient.m_xl),FixPreciError(temp.m_yh-m_eClient.m_yl)),bUsetransColer,transColor);
	::DeleteObject(bitmap0);
}
void C2DGDIDrawingContext::EndDrawTempBmp(int xl, int xh, int yl, int yh)
{
	if(m_hTemDC)
	{
		::DeleteDC(m_hTemDC);
		m_hTemDC = NULL;
	}	
}

void C2DGDIDrawingContext::EraseBackground()
{
	HDC hdc = m_hMemDC;
	CBrush brush(m_clrBack);
	
	::FillRect(hdc,m_rcView,brush);

	if( m_hMemDC!=m_hCurDC && !m_bMemOpOnly )
	{
		::FillRect(m_hOriginalDC,m_rcView,brush);
	}
}


void C2DGDIDrawingContext::DrawGrBuffer(GrElementList *pList, int mode)
{
	DrawGrBuffer_GDI(GetContext(), pList, mode);
}

void C2DGDIDrawingContext::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	DrawGrBuffer2d_GDI(GetContext(), pList, mode);
}

void C2DGDIDrawingContext::PrintGrBuffer2d(GrElementList *pList, int mode, double lfWidthRatio)
{
	CSuperDrawDC dc;
	dc.CreateType(DRAWDC_GDI,GetContext(),m_pD3D);

	dc.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc.SetDrawMode(DRAWMODE_COPY);
	
	m_PointDrawOpSet.ClearAll();
	m_PointStringDrawOpSet.ClearAll();
	DrawGrBuffer2d_Base(&dc,pList,mode,lfWidthRatio);
}

void C2DGDIDrawingContext::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	DrawSelectionMark2d_GDI(GetContext(), pList, mode, clr, wid);
}


void C2DGDIDrawingContext::DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	DrawGeometryMark_GDI(GetContext(), pList, mode, ptInfor);
}


void C2DGDIDrawingContext::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	DrawSelectionMark_GDI(GetContext(), pList, mode, clr, wid);
}

extern void FixPreciError2(float& x);
extern void InterpolateBmp(HBITMAP hSrc, HBITMAP hDest, CRect rcSrc, float kx, float ky);


BOOL C2DGDIDrawingContext::CreateMemGLDC(int w, int h)
{
	if( w<512 )w = 512;
	if( h<512 )h = 512;
	
	if( m_hMemGLDC==NULL )
	{
		m_hMemGLDC = ::CreateCompatibleDC(NULL);
	}
	
	if( m_hMemGLDC==NULL )
		return FALSE;

	BOOL bRecreateBmp = TRUE;
	if( m_pMemGLDC )
	{
		BITMAP bmp;
		GetObject(m_hMemGLBmp,sizeof(bmp),&bmp);

		if( w<=bmp.bmWidth && h<=bmp.bmHeight )
		{
			bRecreateBmp = FALSE;
		}
	}

	if( bRecreateBmp )
	{
		if( m_pMemGLDC )
		{
			delete m_pMemGLDC;
			m_pMemGLDC = NULL;
		}

		if( m_hMemGLBmp )
		{
			::SelectObject(m_hMemGLDC,m_hMemOldGLBmp);
			::DeleteObject(m_hMemGLBmp);
		}
		
		m_hMemGLBmp = Create24BitDIB(w,h);
		if( !m_hMemGLBmp )
			return FALSE;

		m_hMemOldGLBmp = (HBITMAP)::SelectObject(m_hMemGLDC,m_hMemGLBmp);

		m_pMemGLDC = new C2DGLDrawingContext();
		m_pMemGLDC->CreateContextForBmp(m_hMemGLDC);
	}

	return TRUE;
}

void C2DGDIDrawingContext::DrawBitmapByAnyXY(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4])
{
	if( !CreateMemGLDC(m_szDC.cx,m_szDC.cy) )
		return;
	
	PT_2D pts[4];
	pts[0].x = x[0]; pts[0].y = y[0];
	pts[1].x = x[1]; pts[1].y = y[1];
	pts[2].x = x[2]; pts[2].y = y[2];
	pts[3].x = x[3]; pts[3].y = y[3];
	
	Envelope e;
	e.CreateFromPts(pts,4);
	
	CRect rect;
	rect.left = PIXEL(e.m_xl); rect.right = PIXEL(e.m_xh);
	rect.top = PIXEL(e.m_yl); rect.bottom = PIXEL(e.m_yh);
	
	CRect rcView;
	GetViewRect(rcView);
	
	CRect rc1 = (rect&rcView);
	if( rc1.IsRectEmpty() )
		return;

	double y2[4];
	CSize szDC = m_szDC;
	
	//转换成 Opengl 坐标
	y2[0] = szDC.cy-y[0];
	y2[1] = szDC.cy-y[1];
	y2[2] = szDC.cy-y[2];
	y2[3] = szDC.cy-y[3];

//	WriteToFile(hBmp);

	m_pMemGLDC->BeginDrawing();
	m_pMemGLDC->SetDCSize(m_szDC);
	m_pMemGLDC->SetViewRect(&m_rcView);
	m_pMemGLDC->SetGlViewRect(&m_rcView);
	m_pMemGLDC->SetBackColor(m_clrBack);
	m_pMemGLDC->EraseBackground();
	m_pMemGLDC->DrawBitmap(hBmp,rcBmp,x0,y0,x,y2);
	m_pMemGLDC->EndDrawing();

//	WriteToFile(m_hMemGLBmp);

	::BitBlt(GetContext(), rc1.left,rc1.top,rc1.Width(),rc1.Height(),
		m_hMemGLDC, rc1.left,rc1.top,SRCCOPY);
}

void C2DGDIDrawingContext::DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4])
{
	if( m_bDrawBmpByAnyXY )
	{
		DrawBitmapByAnyXY(hBmp,rcBmp,x0,y0,x,y);
		return;
	}

	PT_2D pts[4];
	pts[0].x = x[0]; pts[0].y = y[0];
	pts[1].x = x[1]; pts[1].y = y[1];
	pts[2].x = x[2]; pts[2].y = y[2];
	pts[3].x = x[3]; pts[3].y = y[3];

	PT_2D pts0[4];
	pts0[0].x = x0[0]; pts0[0].y = y0[0];
	pts0[1].x = x0[1]; pts0[1].y = y0[1];
	pts0[2].x = x0[2]; pts0[2].y = y0[2];
	pts0[3].x = x0[3]; pts0[3].y = y0[3];
	
	Envelope e;
	e.CreateFromPts(pts,4);

	Envelope e0;
	e0.CreateFromPts(pts0,4);
	
	CRect rect;
	rect.left = PIXEL(e.m_xl); rect.right = PIXEL(e.m_xh);
	rect.top = PIXEL(e.m_yl); rect.bottom = PIXEL(e.m_yh);

	CRect rect0;
	rect0.left = PIXEL(e0.m_xl); rect0.right = PIXEL(e0.m_xh);
	rect0.top = PIXEL(e0.m_yl); rect0.bottom = PIXEL(e0.m_yh);
	
	CRect rcView;
	GetViewRect(rcView);
	
	CRect rc1 = (rect&rcView);
	if( rc1.IsRectEmpty() )
		return;
	
	float kx = rect.Width()/(float)rect0.Width(), ky = rect.Height()/(float)rect0.Height();
	
	FixPreciError2(kx); FixPreciError2(ky);
	
	int nDestWid = rect.Width()+2, nDestHei = rect.Height()+2;
	
	HBITMAP hSrc = hBmp;
	
	if( kx>1.0f && ky>1.0f )
	{
		rc1.OffsetRect(CPoint(-rect.left,-rect.top));
		CRect rc2;
		rc2.left = floor(rc1.left/kx); rc2.right = ceil(rc1.right/kx);
		rc2.top = floor(rc1.top/ky); rc2.bottom = ceil(rc1.bottom/ky);
		
		CRect rc3;
		rc3.left = 0; rc3.right = ceil(rc2.Width()*kx);
		rc3.top = 0; rc3.bottom = ceil(rc2.Height()*ky);
		
		HBITMAP hDest = Create24BitDIB(rc3.Width(),rc3.Height(),TRUE,m_clrBack);
		
		CRect rc4 = rc2;
		rc4.OffsetRect(CPoint(rect0.left,rect0.top));
		InterpolateBmp(hSrc,hDest,rc4,kx,ky);
		
		rc3.left = rc2.left*kx; rc3.right = rc2.right*kx; 
		rc3.top = rc2.top*ky; rc3.bottom = rc2.bottom*ky; 
		rc3.OffsetRect(CPoint(rect.left,rect.top));
		rc1.OffsetRect(CPoint(rect.left,rect.top));
		rc3.SetRect(rc1.left-rc3.left,rc1.top-rc3.top,rc1.right-rc3.left,rc1.bottom-rc3.top);
		
		HBITMAP hOld = (HBITMAP)::SelectObject(m_hMemDC,hDest);
		::BitBlt(GetContext(), rc1.left,rc1.top,rc1.Width(),rc1.Height(),
			m_hMemDC, rc3.left,rc3.top,SRCCOPY);
		::SelectObject(m_hMemDC,hOld);
		
		::DeleteObject(hDest);
	}
	else
	{
		CRect rc2 = rc1;
		rc2.OffsetRect(CPoint(-rect.left,-rect.top));
		rc2.left = floor(rc2.left/kx); rc2.right = ceil(rc2.right/kx);
		rc2.top = floor(rc2.top/ky); rc2.bottom = ceil(rc2.bottom/ky);

		CRect rc4 = rc2;
		rc4.OffsetRect(CPoint(rect0.left,rect0.top));
				
		::SetStretchBltMode(GetContext(),STRETCH_DELETESCANS);
		
		HBITMAP hOld = (HBITMAP)::SelectObject(m_hMemDC,hSrc);
		::StretchBlt(GetContext(), rc1.left,rc1.top,rc1.Width(),rc1.Height(),
			m_hMemDC, rc4.left,rc4.top,rc4.Width(),rc4.Height(),SRCCOPY);
		::SelectObject(m_hMemDC,hOld);
	}
}

void C2DGDIDrawingContext::SetViewRect(CRect rect)
{
/*	CRect rcView = (m_rcView&CRect(0,0,m_szDC.cx,m_szDC.cy));

	m_rgnCur.DeleteObject();
	m_rgnCur.CreateRectRgn(rcView.left,rcView.top,rcView.right,rcView.bottom);

	::SelectClipRgn(m_hCurDC,m_rgnCur);
*/
	CDrawingContext::SetViewRect(rect);
}

void C2DGDIDrawingContext::SetBackColor(COLORREF clr)
{
	CDrawingContext::SetBackColor(clr);

	if( m_hMemBmp )
	{
		SetBitmapBKColor(m_hMemBmp,CRect(0,0,1000000,1000000),m_clrBack);
	}
}

void C2DGDIDrawingContext::SaveBmp(int idx, CRect rect)
{
	SaveBmp_GDI(GetContext(),idx,rect);
}


void C2DGDIDrawingContext::RestoreBmp(int idx)
{
	RestoreBmp_GDI(GetContext(),idx);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDC CActiveGLRC::m_hCurDC = NULL;
HGLRC CActiveGLRC::m_hCurRC = NULL;
int CActiveGLRC::m_nOwner = 0;

CActiveGLRC::CActiveGLRC(HDC hDC, HGLRC hRC)
{
	m_hOldDC = NULL;
	m_hOldRC = NULL;
	m_bChanged = FALSE;

	if( (m_hCurDC!=hDC || m_hCurRC!=hRC) )
	{
		m_hOldDC = m_hCurDC;
		m_hOldRC = m_hCurRC;

		m_hCurDC = hDC;
		m_hCurRC = hRC;
		
		wglMakeCurrent(m_hCurDC,m_hCurRC);

		m_bChanged = TRUE;

		m_nOwner++;
	}
}


CActiveGLRC::~CActiveGLRC()
{
	if( m_bChanged )
	{
		m_hCurDC = m_hOldDC;
		m_hCurRC = m_hOldRC;

		wglMakeCurrent(m_hOldDC,m_hOldRC);

		m_nOwner--;
	}
}


CActiveGLRC::CActiveGLRC()
{
	m_hOldDC = NULL;
	m_hOldRC = NULL;
	m_bChanged = FALSE;
	m_nOp = 0;
}


void CActiveGLRC::ActiveAhead(HDC hDC, HGLRC hRC)
{
	if( hDC!=NULL && hRC!=NULL && (m_hCurDC!=hDC || m_hCurRC!=hRC) )
	{		
		m_hCurDC = hDC;
		m_hCurRC = hRC;
		
		wglMakeCurrent(m_hCurDC,m_hCurRC);
	}
}

BOOL CActiveGLRC::Active(HDC hDC, HGLRC hRC)
{
	BOOL bRet = FALSE;
	if( m_nOp==0 && hDC!=NULL && hRC!=NULL && (m_hCurDC!=hDC || m_hCurRC!=hRC) )
	{
		m_hOldDC = m_hCurDC;
		m_hOldRC = m_hCurRC;
		
		m_hCurDC = hDC;
		m_hCurRC = hRC;
		
		wglMakeCurrent(m_hCurDC,m_hCurRC);
		
		m_bChanged = TRUE;
		bRet = TRUE;

		m_nOwner++;
	}

	m_nOp++;

	return bRet;
}


BOOL CActiveGLRC::Restore()
{
	BOOL bRet = FALSE;

	if( m_nOp==1 && m_bChanged )
	{
		m_hCurDC = m_hOldDC;
		m_hCurRC = m_hOldRC;
		
		wglMakeCurrent(m_hOldDC,m_hOldRC);

		m_bChanged = FALSE;
		bRet = TRUE;

		m_nOwner--;
	}

	m_nOp--;
	return bRet;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(C2DGLDrawingContext, CDrawingContext)

C2DGLDrawingContext::C2DGLDrawingContext()
{
	m_hMemDC = NULL;
	m_hTmpRC = NULL;
	m_hCurRC = NULL;
	m_pTmpTex = NULL;
}

C2DGLDrawingContext::~C2DGLDrawingContext()
{
	
	if (m_hBitmap32)
	{
		::DeleteObject(m_hBitmap32);
		m_hBitmap32 = NULL;
	}

	if (m_hMemDC)
	{
		::DeleteDC(m_hMemDC);
	}
	DestroyGlContext();

	if(m_pTmpTex)
	{
		delete m_pTmpTex;
		m_pTmpTex = NULL;
	}
	if (m_hTmpRC)
	{
		wglDeleteContext(m_hTmpRC);
		m_hTmpRC = NULL;
	}

}


BOOL C2DGLDrawingContext::CreateContext(HDC hDC)
{
	m_hOriginalDC = hDC;
	CreateGlContext(FALSE,FALSE);
	m_hCurRC = m_hglRC;
	return TRUE;
}


BOOL C2DGLDrawingContext::CreateContextForBmp(HDC hDC)
{
	m_hOriginalDC = hDC;

	CreateGlContextForBmp();
	
	m_hCurRC = m_hglRC;
	return TRUE;
}

HGLRC C2DGLDrawingContext::GetGLRC()
{
	return m_hCurRC;
}


HGLRC C2DGLDrawingContext::SetDrawingRC(HDC hDC, HGLRC hRC)
{
	HGLRC hOldRC = m_hCurRC;
	m_hCurRC = hRC;
	m_hOriginalDC = hDC;
	return hOldRC;
}
static void InitOpenGLWithMemoryDC(HDC hdc)
{
	CBitmap* pBitmap = CDC::FromHandle(hdc)->GetCurrentBitmap() ;
	BITMAP bmInfo ;
	pBitmap->GetObject(sizeof(BITMAP), &bmInfo) ;
    PIXELFORMATDESCRIPTOR pfd=
    {	
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_BITMAP|
			PFD_SUPPORT_OPENGL| PFD_SUPPORT_GDI,
			PFD_TYPE_RGBA,
			(BYTE)bmInfo.bmBitsPixel,
			0,0,0,0,0,0,
			0,0,0,0,0,0,0,
			32,
			0,0,
			PFD_MAIN_PLANE,
			0,
			0,0,0
    };
    int pixelFormat=::ChoosePixelFormat(hdc,&pfd);
    ::SetPixelFormat(hdc,pixelFormat,&pfd);
}
HBITMAP C2DGLDrawingContext::Create32MemBmp(int w, int h)
{
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = w;
	bitmapInfo.bmiHeader.biHeight = h;
	bitmapInfo.bmiHeader.biSizeImage = w*h*4;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biClrImportant = 0;
	BYTE *pBits;
	HBITMAP hBmp = CreateDIBSection( NULL,&bitmapInfo,DIB_RGB_COLORS,
		(void**)&pBits,0,0L );
	if (hBmp==NULL)
	{
		return NULL;
	}
	memset(pBits,0,w*h*4);
	if( hBmp!=NULL )	
	     return hBmp;
	return NULL;
}
void C2DGLDrawingContext::SetTransColorValues(HBITMAP hBmp, COLORREF color)
{
	BITMAP sinfo;
	if( ::GetObject( hBmp, sizeof(BITMAP), &sinfo)==0 )return;
	if( sinfo.bmBitsPixel!=32 )return;
	DWORD *p, *pMax;
	int srcWidth = sinfo.bmWidth;
	p = (DWORD*)sinfo.bmBits;
	pMax = (DWORD*)(((BYTE*)sinfo.bmBits)+srcWidth*sinfo.bmHeight*4);

	while (p < pMax)
	{
		if ((*p) != color)
		{
			(*p) = ((*p) | 0xff000000);
		}
		p++;
	}
}
void C2DGLDrawingContext::BeginDrawTempBmp(HBITMAP hBmp, Envelope e)
{
	CDrawingContext::BeginDrawTempBmp(hBmp,e);
	if (m_hMemDC)
	{
		::DeleteDC(m_hMemDC);
	}
	BITMAP bm;
	if(::GetObject(hBmp,sizeof(BITMAP),&bm)==0)
		return;
	m_hMemDC = ::CreateCompatibleDC(NULL);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC,hBmp);
	InitOpenGLWithMemoryDC(m_hMemDC);
	m_hTmpRC = wglCreateContext(m_hMemDC);
	if (!m_hTmpRC)
	{
		TRACE("wglCreateContext Failed %x\r\n", GetLastError()) ;
		return ;
	}
	m_hOldRC = wglGetCurrentContext();
	m_hOldDC = wglGetCurrentDC();
	wglMakeCurrent(m_hMemDC,m_hTmpRC);
	glViewport(0,0,bm.bmWidth,bm.bmHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(e.m_xl,e.m_xh,e.m_yl,e.m_yh,-1,1);
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	glClearColor(GetRValue(m_clrBack)/255.0,GetGValue(m_clrBack)/255.0,GetBValue(m_clrBack)/255.0,1);
	glClear(GL_COLOR_BUFFER_BIT );
	m_nLastTextSize = -1;
	m_hBitmap32 = NULL;		
}
void C2DGLDrawingContext::DrawTempBmp(HBITMAP hBmp, CRect rcBmp, double x[4], double y[4], BOOL bUsetransColer, COLORREF transColor)
{
	BITMAP info;
	if( ::GetObject(hBmp,sizeof(BITMAP),&info)==0 )
		return;
	if (rcBmp.IsRectEmpty())
	{
		rcBmp = CRect(0,0,info.bmWidth,info.bmHeight);
	}
	if (m_hBitmap32)
	{
		::DeleteObject(m_hBitmap32);
		m_hBitmap32 = NULL;
	}
	m_hBitmap32 = Create32MemBmp(info.bmWidth,info.bmHeight);
	if (!m_hBitmap32)
	{
		return;
	}
	{
		BITMAP info0;
		if(::GetObject(m_hBitmap32,sizeof(info0),&info0)==0)
			return;
		int nLineSize = (info.bmWidth*(info.bmBitsPixel>>3)+3)&~3;
		BYTE *pSrc = (BYTE*)info.bmBits;
		BYTE *pDes = (BYTE*)info0.bmBits;
		BYTE *p1 = pSrc, *p2 = pDes;
		for (int i=0;i<info.bmHeight;i++)
		{
			p1 = pSrc + nLineSize*i;
			for (int j=0;j<info.bmWidth;j++)
			{
				memcpy(p2,p1,sizeof(BYTE)*3);
				p1+=3;
				p2+=4;
			}			
		}
		SetTransColorValues(m_hBitmap32,transColor);
	}
	int texsize = max(info.bmWidth,info.bmHeight);
	for( int i=0; i<32 && texsize>((1<<i)); i++);
	texsize = ((1<<i)); 
	double xr = double(info.bmWidth)/double(texsize);
	double yr = double(info.bmHeight)/double(texsize);
	double x0[4], y0[4];
	x0[0] = 0.0;y0[0] = 1.0-yr;
	x0[1] = xr ;y0[1] = 1.0-yr;
	x0[2] = xr ;y0[2] = 1;
	x0[3] = 0.0;y0[3] = 1;
	BYTE *pBits = new BYTE[texsize*texsize*4];
	if( !pBits )return;
	if (m_nLastTextSize==-1||m_nLastTextSize!=texsize)
	{
		m_nLastTextSize = texsize;
		if (m_pTmpTex)
		{
			delete m_pTmpTex;
		}
		m_pTmpTex = new tex_image;
		m_pTmpTex->gen();
		m_pTmpTex->bind();
		glAlphaFunc(GL_GREATER,0.5);
		glEnable(GL_ALPHA_TEST);
		{
			m_pTmpTex->define(texsize,texsize,GL_BGRA_EXT,pBits);
			m_pTmpTex->bmptotexture(m_hBitmap32,rcBmp.left,rcBmp.top,rcBmp.left,rcBmp.top,rcBmp.Width(),rcBmp.Height(), GL_BGRA_EXT);
		}
		m_pTmpTex->enable();
		glTexEnvf(GL_TEXTURE_ENV,   GL_TEXTURE_ENV_MODE,   GL_MODULATE);
		m_pTmpTex->display(x0,y0,x,y,TRUE,FALSE);
		m_pTmpTex->disable();
		glDisable(GL_ALPHA_TEST); 
	}
	else
	{
		m_pTmpTex->bind();
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.5);
		{		
			m_pTmpTex->bmptotexture(m_hBitmap32,rcBmp.left,rcBmp.top,rcBmp.left,rcBmp.top,rcBmp.Width(),rcBmp.Height(), GL_BGRA_EXT);
		}
		m_pTmpTex->enable();
		glTexEnvf(GL_TEXTURE_ENV,   GL_TEXTURE_ENV_MODE,   GL_MODULATE);
		m_pTmpTex->display(x0,y0,x,y,TRUE,FALSE);
		m_pTmpTex->disable();
		glDisable(GL_ALPHA_TEST);
	}
	delete []pBits;
}
void C2DGLDrawingContext::EndDrawTempBmp(int xl, int xh, int yl, int yh)
{
	if (m_hBitmap32)
	{
		::DeleteObject(m_hBitmap32);
		m_hBitmap32 = NULL;
	}
	m_nLastTextSize =-1;
	if(m_pTmpTex)
	{
		delete m_pTmpTex;
		m_pTmpTex = NULL;
	}
	glDisable(GL_ALPHA_TEST);
	wglMakeCurrent(m_hOldDC,m_hOldRC);
	if (m_hTmpRC)
	{
		wglDeleteContext(m_hTmpRC);
		m_hTmpRC = NULL;
	}
	if (m_hMemDC)
	{
		::SelectObject(m_hMemDC,m_hOldBitmap);
		::DeleteDC(m_hMemDC);
	}
	CDrawingContext::EndDrawTempBmp(xl,xh,yl,yh);
}


void C2DGLDrawingContext::BeginDrawing()
{
	m_switchRC.Active(m_hOriginalDC,m_hCurRC);
}


void C2DGLDrawingContext::EndDrawing()
{
	m_switchRC.Restore();
}


void C2DGLDrawingContext::Scroll(int dx, int dy, CRect *pRcView, CArray<CRect,CRect>* pRects)
{
	if( pRects!=NULL )
	{
		CRect rect(0,0,m_szDC.cx,m_szDC.cy);
		pRects->Add(rect);
	}
}

void C2DGLDrawingContext::SetViewRect(CRect rect)
{
	CDrawingContext::SetViewRect(rect);
}

void C2DGLDrawingContext::DrawGrBuffer(GrElementList *pList, int mode)
{
	DrawGrBuffer_GL(pList,mode);
}

void C2DGLDrawingContext::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	DrawGrBuffer2d_GL(pList,mode);
}


void C2DGLDrawingContext::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	DrawSelectionMark2d_GL(pList,mode,clr,wid);
}

void C2DGLDrawingContext::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	DrawSelectionMark_GL(pList,mode,clr,wid);
}

void C2DGLDrawingContext::DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4])
{
	BITMAP info;
	if( ::GetObject(hBmp,sizeof(BITMAP),&info)==0 )
		return;

	int texsize = max(info.bmWidth,info.bmHeight);
	for( int i=0; i<32 && texsize>(1<<i); i++);

	if( (1<<(i-1))>=texsize )
		texsize = (1<<(i-1));
	else
		texsize = (1<<i);

	BYTE *pBits = gMem2.Allocate(texsize*texsize*3);
	if( !pBits )return;

	for( i=0; i<4; i++)
	{
		x0[i] = x0[i]/texsize;
		y0[i] = y0[i]/texsize;//(texsize-y0[i])/texsize;
	}

	tex_image item;
	item.gen();
	item.bind();
	item.define(texsize,texsize,GL_BGR_EXT,pBits);
	
	item.bmptotexture(hBmp,rcBmp.left,rcBmp.top,rcBmp.left,rcBmp.top,rcBmp.Width(),rcBmp.Height(),GL_BGR_EXT);

	item.enable();
	item.display(x0,y0,x,y,TRUE,FALSE);
	item.disable();
}


void C2DGLDrawingContext::EraseBackground()
{
	glClearColor(GetRValue(m_clrBack)/255.0,GetGValue(m_clrBack)/255.0,GetBValue(m_clrBack)/255.0,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
}



//保存像素和恢复像素的几个工具函数
void C2DGLDrawingContext::SaveBmp(int idx, CRect rect)
{
	SaveBmp_GL(idx,rect);
}


void C2DGLDrawingContext::RestoreBmp(int idx)
{
	RestoreBmp_GL(idx);
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CD3DDrawingContext, CDrawingContext)

CD3DDrawingContext::CD3DDrawingContext()
{
}

CD3DDrawingContext::~CD3DDrawingContext()
{
}


void CD3DDrawingContext::BeginDrawing()
{
	m_pD3D->BeginDrawing();
}


void CD3DDrawingContext::EndDrawing()
{
	m_pD3D->EndDrawing(FALSE);
}


void CD3DDrawingContext::Scroll(int dx, int dy, CRect *pRcView, CArray<CRect,CRect>* pRects)
{
	if( pRects!=NULL )
	{
		CRect rect(0,0,m_szDC.cx,m_szDC.cy);
		pRects->Add(rect);
	}
}

void CD3DDrawingContext::SetViewRect(CRect rect)
{
	CDrawingContext::SetViewRect(rect);
}

void CD3DDrawingContext::DrawGrBuffer(GrElementList *pList, int mode)
{
	DrawGrBuffer_D3D(pList,mode);
}

void CD3DDrawingContext::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	DrawGrBuffer2d_D3D(pList,mode);
}


void CD3DDrawingContext::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	DrawSelectionMark2d_D3D(pList,mode,clr,wid);
}

void CD3DDrawingContext::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	DrawSelectionMark_D3D(pList,mode,clr,wid);
}

void CD3DDrawingContext::DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	DrawGeometryMark_D3D(pList, mode, ptInfor);
}


void CD3DDrawingContext::DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4])
{
	if( !m_pD3D )
		return;

	BITMAP info;
	if( ::GetObject(hBmp,sizeof(BITMAP),&info)==0 )
		return;

	int texsize = max(info.bmWidth,info.bmHeight);
	for( int i=0; i<32 && texsize>(1<<i); i++);

	if( (1<<(i-1))>=texsize )
		texsize = (1<<(i-1));
	else
		texsize = (1<<i);

	for( i=0; i<4; i++)
	{
		x0[i] = x0[i]/rcBmp.Width();
		y0[i] = y0[i]/rcBmp.Height();
	}

	CD3DTexture texture;
	texture.Create(m_pD3D,texsize,texsize,FALSE);
	texture.SetBitmap(hBmp,rcBmp.left,rcBmp.top,rcBmp.left,rcBmp.top,rcBmp.Width(),rcBmp.Height(),FALSE);

	texture.Display(x0,y0,x,y);
}


void CD3DDrawingContext::EraseBackground()
{
	if( !m_pD3D )
		return;

	m_pD3D->ClearBuffer(m_clrBack);
}



//保存像素和恢复像素的几个工具函数
void CD3DDrawingContext::SaveBmp(int idx, CRect rect)
{
//	SaveBmp_GL(idx,rect);
}


void CD3DDrawingContext::RestoreBmp(int idx)
{
//	RestoreBmp_GL(idx);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(C2DPrintGLDrawingContext, C2DGLDrawingContext)

C2DPrintGLDrawingContext::C2DPrintGLDrawingContext()
{
	
}

C2DPrintGLDrawingContext:: ~C2DPrintGLDrawingContext()
{
//	m_switchRC.Restore();
//	if(m_hglRC)wglDeleteContext(m_hglRC);
}

BOOL C2DPrintGLDrawingContext::CreateContext(HDC hDC)
{
	m_hOriginalDC = hDC;
	InitOpenGLWithMemoryDC(hDC);
	m_hglRC = wglCreateContext(m_hOriginalDC);
	CActiveGLRC activeGl(m_hOriginalDC,m_hglRC);

	CRect rect = m_rcView;
	CRect rcClient = CRect(0,0,m_szDC.cx,m_szDC.cy);		
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(rect.left,rcClient.bottom-rect.bottom,rect.Width(),rect.Height());
	glOrtho(rect.left,rect.right,rcClient.bottom-rect.bottom,rcClient.bottom-rect.top,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	glClearColor(GetRValue(m_clrBack)/255.0,GetGValue(m_clrBack)/255.0,GetBValue(m_clrBack)/255.0,1.0f);
//	glClearDepth(1.0f);
//	glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );

	m_hCurRC = m_hglRC;

	return TRUE;
}

void C2DPrintGLDrawingContext::BeginDrawing()
{
	C2DGLDrawingContext::BeginDrawing();
}

void C2DPrintGLDrawingContext::EndDrawing()
{
	C2DGLDrawingContext::EndDrawing();
}


IMPLEMENT_DYNCREATE(C2DPrintGDIDrawingContext, C2DGDIDrawingContext)

C2DPrintGDIDrawingContext::C2DPrintGDIDrawingContext()
{
	m_bDrawBmpByAnyXY = TRUE;
}

C2DPrintGDIDrawingContext:: ~C2DPrintGDIDrawingContext()
{

}

void C2DPrintGDIDrawingContext::TurnonMemBmpBoard()
{
	if( m_nTurnonOp==0 )
	{
		CreateMemBoard();
		if( m_hMemDC!=NULL )
		{
			m_hOldBmp = (HBITMAP)::SelectObject(m_hMemDC,m_hMemBmp);
			m_hTurnonOldDC = m_hCurDC;
			m_hCurDC = m_hMemDC;
// 			::FillRect(GetMemBoardDC(),
// 					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
		}
	}
	m_nTurnonOp++;
}

void C2DPrintGDIDrawingContext::TurnoffMemBmpBoard()
{
	if( m_nTurnonOp==1 )
	{
		if( m_hMemDC!=NULL )
		{
			::SelectObject(m_hMemDC,m_hOldBmp);	
			m_hCurDC = m_hTurnonOldDC;
		}
	}
	m_nTurnonOp--;
}

void C2DPrintGDIDrawingContext::CreateMemBoard()
{
	if( m_hMemBmp!=NULL )
	{			
		if(m_hMemBmp)::DeleteObject(m_hMemBmp);				
	}
	m_hMemBmp = CreateMemBmpBoard(m_szDC.cx,m_szDC.cy);
	if( m_hMemDC==NULL )
		m_hMemDC = ::CreateCompatibleDC(m_hOriginalDC);
}

void C2DPrintGDIDrawingContext::SetBackColor(COLORREF clr)
{	
	CDrawingContext::SetBackColor(clr);
}



void C2DPrintGDIDrawingContext::BeginDrawing()
{
	C2DGDIDrawingContext::BeginDrawing();

	CGdiDrawingDC::m_bUseGDIPolygon = TRUE;
}

void C2DPrintGDIDrawingContext::EndDrawing()
{
	CGdiDrawingDC::m_bUseGDIPolygon = FALSE;

	C2DGDIDrawingContext::EndDrawing();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(C3DDrawingContext, CDrawingContext)

C3DDrawingContext::C3DDrawingContext()
{
	
}

C3DDrawingContext::~C3DDrawingContext()
{
	
}

HGLRC C3DDrawingContext::GetGLRC()
{
	return NULL;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CStereoDrawingContext, CDrawingContext)

CStereoDrawingContext::CStereoDrawingContext()
{
	m_pLeft = NULL;
	m_pRight = NULL;

	m_nTextMode = textureModeNone;

	m_nDisplayMode = modeSingle;
	m_nDrawSide = drawLeft;

	m_bCreateStereo = TRUE;
	m_bCreateOverlay = FALSE;
	m_bInverseStereo = FALSE;
	m_bDrawBack = FALSE;

	m_hMemBoardBmp = NULL;
	m_hMemBoardDC = NULL;
	m_hMemRGBmp = NULL;

	m_nBeginDrawOp = 0;

	m_nDrawState = drawNoneState;

	m_bOldStereoScroll = TRUE;

	m_bSupportLogicOp = TRUE;

	m_bPaintOneSide = FALSE;

	m_bCorrectFlicker = FALSE;

	memset(m_strSegPath,0,sizeof(m_strSegPath));
}

CStereoDrawingContext::~CStereoDrawingContext()
{
	m_switchRC.Active(m_hOriginalDC,m_hglRC);
	m_dt.Clear();

	if( m_pLeft )
		delete m_pLeft;

	if( m_pRight )
		delete m_pRight;

	DestroyGlContext();

	if( m_hMemBoardDC )
		::DeleteDC(m_hMemBoardDC);

	if( m_hMemBoardBmp )
		::DeleteObject(m_hMemBoardBmp);

	if( m_hMemRGBmp )
		::DeleteObject(m_hMemRGBmp);
}

HGLRC CStereoDrawingContext::GetRCContext()
{
	return m_hglRC;
}


HGLRC CStereoDrawingContext::GetOverlayRCContext()
{
	return m_hglRC_overlay;
}

BOOL CStereoDrawingContext::SafeActiveGLRC()
{
	if( m_switchRC.GetCurRC()!=GetRCContext() && m_switchRC.GetOwnerCount()==0 )
	{
		//m_switchRC.ActiveAhead(m_hOriginalDC,m_hglRC);
		return TRUE;
	}

	return FALSE;
}

BOOL CStereoDrawingContext::SetDisplayMode(int nMode)
{
	if( m_bCreateOK )
	{
		//不允许单片模式和其他模式之间切换
//		if( m_nDisplayMode==modeSingle || nMode==modeSingle )
//			return FALSE;
	}

	m_nDisplayMode = nMode;

	if (nMode == modeShutterStereo)
	{
		SetDCSize(m_szDC);
	}	

	if( m_bCreateOK )
	{
		DWORD mask = 0;
		UpdateRGBMask(TRUE,FALSE,mask);
		UpdateRGBMask(FALSE,FALSE,mask);
		
		if( m_nDisplayMode==modeRGStereo )
		{
			GetLeftContext()->EnableMonocolor(TRUE,GetColorMask(GetLeftContext()->GetRGBMask())&RGB(255,255,255));
			GetRightContext()->EnableMonocolor(TRUE,GetColorMask(GetRightContext()->GetRGBMask())&RGB(255,255,255));
		}
		else
		{
			GetLeftContext()->EnableMonocolor(FALSE,0);
			GetRightContext()->EnableMonocolor(FALSE,0);
		}
	}

	return TRUE;
}


void CStereoDrawingContext::SetDrawSide(int nSide)
{
	m_nDrawSide = nSide;
	
	if (nSide==drawLeft)
	{
		m_nRGBMask=m_pLeft->GetRGBMask();
	}
	else
	{
		m_nRGBMask=m_pRight->GetRGBMask();
	}
	SetGLDrawBuffer(m_nDrawSide);

	SetViewRect(m_rcView);

	return;
}


int CStereoDrawingContext::GetDrawSide()
{
	return m_nDrawSide;
}


CDrawingContext *CStereoDrawingContext::GetLeftContext()
{
	return m_pLeft;
}


CDrawingContext *CStereoDrawingContext::GetRightContext()
{
	return m_pRight;
}


CDrawingContext *CStereoDrawingContext::GetCurContext()
{
	if( m_nDrawSide==drawLeft )
	{
		if( m_pLeft )
		{
			return m_pLeft;
		}
		return NULL;
	}
	else
	{
		if( m_pRight )
		{
			return m_pRight;
		}
		return NULL;
	}
}


BOOL CStereoDrawingContext::CreateContext(HDC hDC)
{
	CDrawingContext::CreateContext(hDC);

	if( m_nTextMode==textureModeGL )
	{
		m_pLeft = new C2DGLDrawingContext;
		m_pRight = new C2DGLDrawingContext;
	}
	else if( m_nTextMode==textureModeD3D )
	{
		m_pLeft = new CD3DDrawingContext;
		m_pRight = new CD3DDrawingContext;
	}
	else
	{
		m_pLeft = new C2DGDIDrawingContext;
		m_pRight = new C2DGDIDrawingContext;
	}
	
	if( m_pLeft==NULL || m_pRight==NULL )
		return FALSE;

	m_pLeft->m_bStereo = m_bStereo;
	m_pRight->m_bStereo = m_bStereo;

	if( m_bCreateStereo )
	{
		if( m_bCreateOverlay )
		{
			if( !CreateGlContext(TRUE,TRUE) )
			{
				m_bCreateOverlay = FALSE;
			}
		}

		if( !m_bCreateOverlay )
		{
			if( !CreateGlContext(TRUE,FALSE) )
			{
				m_bCreateStereo = FALSE;
			}
		}
	}

	if( !m_bCreateStereo && m_nTextMode==textureModeGL )
	{
		if( !CreateGlContext(FALSE,FALSE) )
		{
			m_nTextMode = textureModeNone;
		}

		m_bCreateOverlay = FALSE;
	}

	if( m_nTextMode==textureModeGL )
	{
		((C2DGLDrawingContext*)m_pLeft)->SetDrawingRC(hDC,m_hglRC);
		((C2DGLDrawingContext*)m_pRight)->SetDrawingRC(hDC,m_hglRC);
	}
	else if( m_nTextMode==textureModeD3D )
	{
		m_pLeft->CreateContext(hDC);
		m_pRight->CreateContext(hDC);

		m_pLeft->m_pD3D = m_pD3D;
		m_pRight->m_pD3D = m_pD3D;

		m_pLeft->m_bUseD3D = TRUE;
		m_pRight->m_bUseD3D = FALSE;
	}
	else
	{
		m_pLeft->CreateContext(hDC);
		m_pRight->CreateContext(hDC);

		 if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		 {
			 ((C2DGDIDrawingContext*)m_pLeft)->CreateMemBoard();
			 ((C2DGDIDrawingContext*)m_pRight)->CreateMemBoard();
		 }
	}

	return TRUE;
}


void CStereoDrawingContext::SetDCSize(CSize szDC)
{
	CDrawingContext::SetDCSize(szDC);

	if( !m_bCreateOK )
		return;

	{
		CActiveGLRC changeRC(m_hOriginalDC,m_hglRC);		

		if( m_nDisplayMode==CStereoDrawingContext::modeSplit )
		{
			if (m_nDrawState==drawBitmap)
			{
				;
			}
			else
			{
				szDC.cx = (szDC.cx+1)/2;
			
				m_pLeft->SetDCSize(szDC);
				m_pRight->SetDCSize(szDC);
				SetGlViewRect(NULL);
				return;
			}
		
		}

		m_pLeft->SetDCSize(szDC);
		m_pRight->SetDCSize(szDC);
		SetGlViewRect(NULL);
	}

	if( m_hglRC_overlay!=NULL )
	{
		CActiveGLRC changeRC(m_hOriginalDC,m_hglRC_overlay);
		SetGlViewRect(NULL);
	}
}


void CStereoDrawingContext::SetViewRect(CRect rcView)
{
	if( m_nTextMode!=textureModeNone )
	{
		rcView = CRect(0,0,m_szDC.cx,m_szDC.cy);
	}
	
	CDrawingContext::SetViewRect(rcView);

	if( !m_bCreateOK )
		return;

	if( m_nDisplayMode==CStereoDrawingContext::modeSplit )
	{
		if (m_nDrawState==drawBitmap)
		{
			GetCurContext()->SetViewRect(m_rcView);
		}
		else
		    GetCurContext()->SetViewRect(GetSplitDrawRect(m_nDrawSide));
	}
	else if( m_nDisplayMode==CStereoDrawingContext::modeInterleavedStereo )
	{
		if (m_nDrawState==drawBitmap)
		{
			GetCurContext()->SetViewRect(m_rcView);
		}
		else
		{
			CRect rcView = m_rcView;
			rcView.top = rcView.top/2;
			rcView.bottom = (rcView.bottom+1)/2;
			if( rcView.bottom==rcView.top )
				rcView.bottom = rcView.top+1;
			
			GetCurContext()->SetViewRect(rcView);
		}
		
	}
	else
		GetCurContext()->SetViewRect(m_rcView);
}


void CStereoDrawingContext::OnCreateD3D()
{
	if( m_bCreateD3D && m_pD3D )
	{
		m_pLeft->m_pD3D = m_pD3D;
		m_pRight->m_pD3D = m_pD3D;

		m_pLeft->m_bUseD3D = m_bUseD3D;
		m_pRight->m_bUseD3D = m_bUseD3D;
	}
}


void CStereoDrawingContext::SetGLDrawBuffer(int nSide)
{
	if( m_hglRC==NULL )
		return;

// 	if( !m_bCreateStereo )
// 		return;

	if( m_nDisplayMode==modeSingle || m_nDisplayMode==modeRGStereo || m_nDisplayMode==modeSplit )
	{
		if( m_bDrawBack )
		{
			glDrawBuffer(GL_BACK);
			glReadBuffer(GL_BACK);
		}
		else
		{
			glDrawBuffer(GL_FRONT);
			glReadBuffer(GL_FRONT);
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		if( m_nDrawState==drawNormal )
		{
			m_dt.BeginRenderToTexture(nSide==drawLeft?0:1);
			
			glMatrixMode(GL_MODELVIEW);
			glTranslatef(0,-m_szDC.cy/2,0);
		}
	}
	else
	{
		GLenum target = GL_FRONT_LEFT;
		
		if( m_bInverseStereo )
			nSide = ((int)(drawLeft+drawRight))-nSide/*m_nDrawSide*/;
		
		if( !m_bCreateStereo )
		{
			if( m_bDrawBack )
				target = GL_BACK;
			else
				target = GL_FRONT;
		}
		else if( nSide==drawLeft )
		{
			if( m_bDrawBack )
				target = GL_BACK_LEFT;
			else
				target = GL_FRONT_LEFT;
		}
		else if( nSide==drawRight )
		{
			if( m_bDrawBack )
				target = GL_BACK_RIGHT;
			else
				target = GL_FRONT_RIGHT;
		}
		
		glDrawBuffer(target);
		glReadBuffer(target);
	}

}

void CStereoDrawingContext::SetFillBlockGLDrawBuffer(int nSide)
{
	if( m_hglRC==NULL )
		return;
		
	if( m_nDisplayMode==modeSingle || m_nDisplayMode==modeRGStereo || m_nDisplayMode==modeSplit )
	{
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		if( m_nDrawState==drawNormal )
		{
			m_dt.BeginRenderToTexture(nSide==drawLeft?0:1);
			
			glMatrixMode(GL_MODELVIEW);
			glTranslatef(0,-m_szDC.cy/2,0);
		}
	}
	else
	{
		GLenum target = GL_FRONT_LEFT;
		
		if( m_bInverseStereo )
			nSide = ((int)(drawLeft+drawRight))-nSide/*m_nDrawSide*/;
		
		if( !m_bCreateStereo )
		{
			target = GL_BACK;
		}
		else if( nSide==drawLeft )
		{
			target = GL_BACK_LEFT;
		}
		else if( nSide==drawRight )
		{
			target = GL_BACK_RIGHT;
		}
		glDrawBuffer(target);
		glReadBuffer(target);
	}
	
}

CRect CStereoDrawingContext::GetSplitDrawRect(int nDrawSide)
{
	CRect rcDC = CRect(0,0,m_szDC.cx,m_szDC.cy);

	if( nDrawSide==drawLeft )
	{
		if( m_bInverseStereo )
		{
			rcDC.left = m_szDC.cx/2+1;
			rcDC = (rcDC&m_rcView);
			rcDC.OffsetRect(-(m_szDC.cx/2+1),0);
		}
		else
		{
			rcDC.right = m_szDC.cx/2-1;
			rcDC = (rcDC&m_rcView);
		}
	}
	else if( nDrawSide==drawRight )
	{
		if( m_bInverseStereo )
		{
			rcDC.right = m_szDC.cx/2-1;
			rcDC = (rcDC&m_rcView);
		}
		else
		{
			rcDC.left = m_szDC.cx/2+1;
			rcDC = (rcDC&m_rcView);
			rcDC.OffsetRect(-(m_szDC.cx/2+1),0);
		}
	}
	
	return rcDC;
}

int CStereoDrawingContext::GetSplitX(int nDrawSide)
{
	if( nDrawSide==drawLeft )
	{
		if( m_bInverseStereo )
		{
			return m_szDC.cx/2+1;
		}
		else
		{
			return 0;
		}
	}
	else if( nDrawSide==drawRight )
	{
		if( m_bInverseStereo )
		{
			return 0;
		}
		else
		{
			return m_szDC.cx/2+1;
		}
	}
	return 0;
}

CRect CStereoDrawingContext::GetSplitViewRect(int nDrawSide)
{
	CRect rcDC = CRect(0,0,m_szDC.cx,m_szDC.cy);
	if( nDrawSide==drawLeft )
	{
		if( m_bInverseStereo )
			rcDC.left = m_szDC.cx/2+1;
		else
			rcDC.right = m_szDC.cx/2-1;
	}
	else if( nDrawSide==drawRight )
	{
		if( m_bInverseStereo )
			rcDC.right = m_szDC.cx/2-1;
		else
			rcDC.left = m_szDC.cx/2+1;
	}
	
	rcDC = (rcDC&m_rcView);
	return rcDC;
}

//偏振立体时，不适合在整体绘制中绘制Erasable图层
BOOL CStereoDrawingContext::CanPaintErasable()
{
	if( m_nTextMode!=textureModeNone && m_nDisplayMode==modeInterleavedStereo )
		return TRUE;

	if( m_nTextMode==textureModeNone && m_nDisplayMode==modeInterleavedStereo )
		return FALSE;

	if( m_nDisplayMode==modeRGStereo )
		return FALSE;

	if( m_nDisplayMode==modeSplit )
		return FALSE;

	if( m_nTextMode!=textureModeNone && m_nDisplayMode==modeShutterStereo )
		return TRUE;

	if( m_nTextMode!=textureModeNone && m_nDisplayMode==modeSingle )
		return TRUE;

	return FALSE;
}


//纹理模式下，不适合在局部更新中绘制Erasable图层，应总是整体绘制
BOOL CStereoDrawingContext::CanUpdateErasable()
{
	if( m_nTextMode!=textureModeNone && m_nDisplayMode==modeShutterStereo )
		return FALSE;

	if( m_nTextMode!=textureModeNone && m_nDisplayMode==modeSingle )
		return FALSE;

	if( m_nTextMode!=textureModeNone && m_nDisplayMode==modeInterleavedStereo )
		return FALSE;
	
	return TRUE;
}

void CStereoDrawingContext::BeginDrawingErasable()
{
	if( m_nDisplayMode==modeSingle )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = FALSE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}		
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = FALSE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			m_bDrawBack = FALSE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = FALSE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = FALSE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = FALSE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}

	m_nDrawState = drawErasable;
	m_nBeginDrawOp++;
}


void CStereoDrawingContext::EndDrawingErasable()
{
	if( m_nDisplayMode==modeSingle )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_switchRC.Restore();
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}

	m_nDrawState = drawNoneState;
	m_nBeginDrawOp--;
}

void CStereoDrawingContext::BeginDrawing()
{
	if( m_nDisplayMode==modeSingle )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
			glClearColor(GetRValue(m_clrBack)/255.0,GetGValue(m_clrBack)/255.0,GetBValue(m_clrBack)/255.0,1);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			m_pD3D->BeginDrawing();
			m_pD3D->ClearBuffer(m_clrBack);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
// 			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
// 			::FillRect(,
// 					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();

			{
 				::FillRect(((C2DGDIDrawingContext*)m_pLeft)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));

 				::FillRect(((C2DGDIDrawingContext*)m_pRight)->GetMemBoardDC(),
 					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
			}
	
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
		}		
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);

			EnableLogicOp(FALSE);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_LIGHTING);
			glDisable(GL_STENCIL_TEST);
			glDisable(GL_DEPTH_TEST);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		if( m_nBeginDrawOp==0 )
		{
			PrepareRGMemBmp(m_rcView.right,m_rcView.bottom);
			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
		}

		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);

		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
			m_dt.CreateFBO(m_szDC.cx,m_szDC.cy/2);

			EnableLogicOp(FALSE);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			if( m_nBeginDrawOp==0 )
			{
				PrepareRGMemBmp(m_rcView.right,m_rcView.bottom);
				PrepareMemBoard(m_rcView.right,m_rcView.bottom);
			}
			
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
		}
	}

	m_nDrawState = drawNormal;
	m_nBeginDrawOp++;
}


BOOL CStereoDrawingContext::PrepareMemBoard(int w, int h)
{
	if( m_hMemBoardDC==NULL )
		m_hMemBoardDC = ::CreateCompatibleDC(NULL);

	if( m_hMemBoardBmp==NULL )
		m_hMemBoardBmp = ::Create24BitDIB(w,h,TRUE,m_clrBack);
	else
	{
		BITMAP info;	
		if( ::GetObject( m_hMemBoardBmp , sizeof(BITMAP), &info )==0 )
			return FALSE;

		if( info.bmWidth<w || info.bmHeight<h )
		{
			::DeleteObject(m_hMemBoardBmp);

			m_hMemBoardBmp = ::Create24BitDIB(w,h,TRUE,m_clrBack);
		}
		else
		{
			memset(info.bmBits,0,info.bmWidthBytes*info.bmHeight);
		}
	}

	if( m_hMemBoardDC!=NULL && m_hMemBoardBmp!=NULL )
		return TRUE;

	return FALSE;
}


BOOL CStereoDrawingContext::PrepareRGMemBmp(int w, int h)
{	
	if( m_hMemRGBmp==NULL )
		m_hMemRGBmp = ::Create24BitDIB(w,h,TRUE,m_clrBack);
	else
	{
		BITMAP info;	
		if( ::GetObject( m_hMemRGBmp , sizeof(BITMAP), &info )==0 )
			return FALSE;
		
		if( info.bmWidth<w || info.bmHeight<h )
		{
			::DeleteObject(m_hMemRGBmp);
			
			m_hMemRGBmp = ::Create24BitDIB(w,h,TRUE,m_clrBack);
		}
		else
		{
			memset(info.bmBits,0,info.bmWidthBytes*info.bmHeight);
		}
	}
	
	if( m_hMemRGBmp!=NULL )
		return TRUE;
	
	return FALSE;
}


BOOL CStereoDrawingContext::ReadGLPixels(CRect rect)
{
	if( m_hMemBoardBmp==NULL ) return FALSE;
	
	HBITMAP hBmp = m_hMemBoardBmp;
	
	int xTar = rect.left, yTar = rect.top;
	int nTarWidth = rect.Width(), nTarHeight = rect.Height();
	
	CRect rcClt = CRect(0,0,m_szDC.cx,m_szDC.cy);
	CSize szDevice = rcClt.Size();
	
	if( nTarWidth<=0 || nTarHeight<=0 )return FALSE;
	
	CRect rectTar(xTar,yTar,xTar+nTarWidth,yTar+nTarHeight);
	CRect rectSrc(0,0,szDevice.cx,szDevice.cy), rectin;
	
	rectin = rectTar&rectSrc;
	if( rectin!=rectTar )
	{
		xTar = rectin.left;
		yTar = rectin.top;
		nTarWidth = rectin.Width();
		nTarHeight = rectin.Height();
	}
	
	{
		BOOL bNewBuf = FALSE, bSafe = TRUE;
		
		BITMAP src;	
		if( ::GetObject( hBmp , sizeof(BITMAP), &src )==0 )
			return FALSE;

		BYTE* pSrc0 =(BYTE*)src.bmBits, *pSrc;
		if( pSrc0==NULL )return FALSE;
		
		int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
		linesize =  ((linesize+3)&(~3));
		pSrc = pSrc0+ (src.bmHeight-(nTarHeight))*linesize;

		glPixelStorei(GL_PACK_ROW_LENGTH,src.bmWidth);	
		glPixelStorei(GL_PACK_SKIP_PIXELS,0);
		glPixelStorei(GL_PACK_SKIP_ROWS,0);
		
		glReadPixels(xTar,szDevice.cy-yTar-nTarHeight,
			nTarWidth,nTarHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,pSrc);
		
	}
//	WriteToFile(m_hMemBoardBmp);
	return TRUE;
}


BOOL CStereoDrawingContext::ReadD3DPixels(CRect rect)
{
	if( m_hMemBoardBmp==NULL ) return FALSE;

	m_pD3D->BeginLock(rect,TRUE);
	m_pD3D->BitBltTo(m_hMemBoardBmp,rect);
	m_pD3D->EndLock();

	return TRUE;
}


void CopyBackFrameToFront(CSize szDC)
{
	glReadBuffer(GL_BACK_LEFT);
	glDrawBuffer(GL_FRONT_LEFT);
	glRasterPos2i(0,0);
	try
	{
		glCopyPixels(0,0,szDC.cx,szDC.cy,GL_COLOR ); 
	}
	catch(...)
	{
		GLvoid *pBuf = gMem2.Allocate(szDC.cx*(szDC.cy+2)*3);
		if( pBuf )
		{
			glPixelStorei(GL_PACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_PACK_SKIP_PIXELS,0);
			glPixelStorei(GL_PACK_SKIP_ROWS,0);
			glPixelStorei(GL_UNPACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
			
			glReadPixels(0,0,szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
			glDrawPixels(szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
		}
	}
	
	glReadBuffer(GL_BACK_RIGHT);
	glDrawBuffer(GL_FRONT_RIGHT);
	glRasterPos2i(0,0);
	try
	{
		glCopyPixels(0,0,szDC.cx,szDC.cy,GL_COLOR ); 
	}
	catch(...)
	{
		GLvoid *pBuf = gMem2.Allocate(szDC.cx*(szDC.cy+2)*3);
		if( pBuf )
		{
			glPixelStorei(GL_PACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_PACK_SKIP_PIXELS,0);
			glPixelStorei(GL_PACK_SKIP_ROWS,0);
			glPixelStorei(GL_UNPACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
			
			glReadPixels(0,0,szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
			glDrawPixels(szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
		}
	}
}


void CopyFrontFrameToToBack(CSize szDC)
{
	glReadBuffer(GL_FRONT_LEFT);
	glDrawBuffer(GL_BACK_LEFT);
	glRasterPos2i(0,0);
	try
	{
		glCopyPixels(0,0,szDC.cx,szDC.cy,GL_COLOR ); 
	}
	catch(...)
	{
		GLvoid *pBuf = gMem2.Allocate(szDC.cx*(szDC.cy+2)*3);
		if( pBuf )
		{
			glPixelStorei(GL_PACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_PACK_SKIP_PIXELS,0);
			glPixelStorei(GL_PACK_SKIP_ROWS,0);
			glPixelStorei(GL_UNPACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
			
			glReadPixels(0,0,szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
			glDrawPixels(szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
		}
	}
	
	glReadBuffer(GL_FRONT_RIGHT);
	glDrawBuffer(GL_BACK_RIGHT);
	glRasterPos2i(0,0);
	try
	{
		glCopyPixels(0,0,szDC.cx,szDC.cy,GL_COLOR ); 
	}
	catch(...)
	{
		GLvoid *pBuf = gMem2.Allocate(szDC.cx*(szDC.cy+2)*3);
		if( pBuf )
		{
			glPixelStorei(GL_PACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_PACK_SKIP_PIXELS,0);
			glPixelStorei(GL_PACK_SKIP_ROWS,0);
			glPixelStorei(GL_UNPACK_ROW_LENGTH,szDC.cx);	
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
			
			glReadPixels(0,0,szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
			glDrawPixels(szDC.cx,szDC.cy,GL_RGB,GL_UNSIGNED_BYTE,pBuf);
		}
	}
}

BOOL CStereoDrawingContext::IsD3DLost()
{
	if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) && m_pD3D!=NULL && m_pD3D->m_pD3DDevice9!=NULL )
	{
		HRESULT hr = m_pD3D->m_pD3DDevice9->TestCooperativeLevel();
		
		if (hr==D3DERR_DEVICENOTRESET)
		{
			return TRUE;
		}
		else if(hr==D3DERR_DEVICELOST )
		{
			while(hr==D3DERR_DEVICELOST)
			{
				Sleep(0);				
				hr = m_pD3D->m_pD3DDevice9->TestCooperativeLevel();
			}

			if (hr==D3DERR_DEVICENOTRESET)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


void CStereoDrawingContext::EndDrawing()
{
	if( m_nDisplayMode==modeSingle )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			if( m_nBeginDrawOp==1 && m_bDrawBack )
			{
				glFlush();
				TRACE("swap time %d\n",GetTickCount());

				SwapBuffers(m_hOriginalDC);
			}
			
			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			m_pD3D->EndDrawing(TRUE);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			::BitBlt(m_hOriginalDC,
				m_rcView.left,m_rcView.top,m_rcView.Width(),m_rcView.Height(),
				((C2DGDIDrawingContext*)m_pLeft)->GetMemBoardDC(),m_rcView.left,m_rcView.top,
				SRCCOPY);
			
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			if( m_nBeginDrawOp==1 && m_bDrawBack )
			{
				//glFinish();
				TRACE("swap time %d\n",GetTickCount());
				if( m_bCorrectFlicker )
					CopyBackFrameToFront(m_szDC);
				else
					wglSwapLayerBuffers(m_hOriginalDC, WGL_SWAP_MAIN_PLANE);				
			}

			EnableLogicOp(TRUE);

			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			m_switchRC.Active(m_hOriginalDC,m_hglRC);

			if( !m_bPaintOneSide )
			{
				SetGLDrawBuffer(drawLeft);
				BltBmpToGlContext(((C2DGDIDrawingContext*)GetLeftContext())->GetMemBoardBmp(),
					m_rcView.left,m_rcView.top,
					m_rcView.left,m_rcView.top,
					m_rcView.Width(),m_rcView.Height());
				
				SetGLDrawBuffer(drawRight);
				BltBmpToGlContext(((C2DGDIDrawingContext*)GetRightContext())->GetMemBoardBmp(),
					m_rcView.left,m_rcView.top,
					m_rcView.left,m_rcView.top,
					m_rcView.Width(),m_rcView.Height());
				glFinish();
			}
			else
			{
				SetGLDrawBuffer(m_nDrawSide);
				BltBmpToGlContext(((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp(),
					m_rcView.left,m_rcView.top,
					m_rcView.left,m_rcView.top,
					m_rcView.Width(),m_rcView.Height());
				glFinish();
			}
			
			m_switchRC.Restore();
			
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
		if( m_nBeginDrawOp==1 )
		{
			::FillRect( m_hOriginalDC,
				&CRect(m_szDC.cx/2-1,0,m_szDC.cx/2+1,m_szDC.cy),(HBRUSH)CBrush(RGB(255,255,255)) );
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		if( m_nBeginDrawOp==1 )
		{
			HBITMAP hOldBmp = (HBITMAP)::SelectObject(m_hMemBoardDC,m_hMemRGBmp);
			::BitBlt(m_hOriginalDC,
				m_rcView.left,m_rcView.top,m_rcView.Width(),m_rcView.Height(),
				m_hMemBoardDC,0,0,
				SRCCOPY);
			::SelectObject(m_hMemBoardDC,hOldBmp);
		}
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			//显示数据应该在 m_hMemRGBmp 中准备好了
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glViewport(0, 0, m_szDC.cx, m_szDC.cy);
			m_dt.DisplayInterleavedTextures(m_szDC,m_rcView,IsPolarizedVisibleLeft(0));

			glFinish();
			SwapBuffers(m_hOriginalDC);

			EnableLogicOp(TRUE);

			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			//显示数据应该在 m_hMemRGBmp 中准备好了
			if( m_nBeginDrawOp==1 )
			{
				HBITMAP hOldBmp = (HBITMAP)::SelectObject(m_hMemBoardDC,m_hMemRGBmp);
				
				::BitBlt(m_hOriginalDC,
					m_rcView.left,m_rcView.top,m_rcView.Width(),m_rcView.Height(),
					m_hMemBoardDC,0,0,
					SRCCOPY);
				
				::SelectObject(m_hMemBoardDC,hOldBmp);
			}

			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}

	m_nDrawState = drawNoneState;
	m_nBeginDrawOp--;
}

void CStereoDrawingContext::BeginDrawingBitmap()
{
	if( m_nDisplayMode==modeSingle )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			m_pD3D->BeginDrawing();	
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			{
				::FillRect(((C2DGDIDrawingContext*)m_pLeft)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
				::FillRect(((C2DGDIDrawingContext*)m_pRight)->GetMemBoardDC(),
 					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
			}		
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
		}		
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			{
				::FillRect(((C2DGDIDrawingContext*)m_pLeft)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
				::FillRect(((C2DGDIDrawingContext*)m_pRight)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
			}
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			{
				::FillRect(((C2DGDIDrawingContext*)m_pLeft)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
				::FillRect(((C2DGDIDrawingContext*)m_pRight)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
			}

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		if( m_nBeginDrawOp==0 )
		{
			PrepareRGMemBmp(m_rcView.right,m_rcView.bottom);
			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
		}

		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			{
				::FillRect(((C2DGDIDrawingContext*)m_pLeft)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
				::FillRect(((C2DGDIDrawingContext*)m_pRight)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
			}
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);

		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_bDrawBack = TRUE;
			m_switchRC.Active(m_hOriginalDC,m_hglRC);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnonMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnonMemBmpBoard();
			{
				::FillRect(((C2DGDIDrawingContext*)m_pLeft)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
				::FillRect(((C2DGDIDrawingContext*)m_pRight)->GetMemBoardDC(),
					&CRect(0,0,m_szDC.cx,m_szDC.cy),CBrush(m_clrBack));
			}
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(TRUE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(TRUE);
			PrepareRGMemBmp(m_rcView.right,m_rcView.bottom);
		}
	}

	m_nDrawState = drawBitmap;
	m_nBeginDrawOp++;

}


void CStereoDrawingContext::OnFinishDrawSideBitmap()
{
	if( m_nDisplayMode==modeSingle )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			OverlapBitmap(((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp(),NULL,m_rcView,m_hMemBoardBmp,NULL,m_rcView,FALSE,0);		
			
		}
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
		
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			OverlapBitmap(((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp(),NULL,m_rcView,m_hMemBoardBmp,NULL,m_rcView,FALSE,0);		

		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
		
			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
			ReadGLPixels(m_rcView);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
 	    	OverlapBitmap(((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp(),NULL,m_rcView,m_hMemBoardBmp,NULL,m_rcView,FALSE,0);		
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			ReadGLPixels(m_rcView);
			
			if( m_nDrawSide==drawLeft )
			{
				OverlapBitmap(m_hMemBoardBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
			else
			{
				OverlapBitmap(m_hMemBoardBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			HBITMAP hBmp = ((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp();

			if( m_nDrawSide==drawLeft )
			{
				OverlapBitmap(hBmp,NULL,m_rcView,
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
			else
			{
				OverlapBitmap(hBmp,NULL,m_rcView,
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_dt.EndRenderToTexture();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			HBITMAP hBmp = ((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp();

			CRect rcView;
			GetCurContext()->GetViewRect(rcView);
			
			if( m_nDrawSide==drawLeft )
			{
				MakeInterleavedBmp(m_hMemRGBmp,0,0,
					hBmp,rcView,m_bInverseStereo?TRUE:FALSE);
			}
			else
			{
				MakeInterleavedBmp(m_hMemRGBmp,0,0,
					hBmp,rcView,m_bInverseStereo?FALSE:TRUE);
			}
		}
	}
	
}

void CStereoDrawingContext::EndDrawingBitmap()
{
	if( m_nDisplayMode==modeSingle )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			if( m_nBeginDrawOp==1 && m_bDrawBack )
			{
				glFinish();
				PrepareMemBoard(m_rcView.right,m_rcView.bottom);
				ReadGLPixels(m_rcView);
			}
			
			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			m_pD3D->EndDrawing(FALSE);

			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
			ReadD3DPixels(m_rcView);
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			if( m_nBeginDrawOp==1 && m_bDrawBack )
			{
				PrepareMemBoard(m_rcView.right,m_rcView.bottom);
				glFinish();
				ReadGLPixels(m_rcView);

//				::SwapBuffers(m_hOriginalDC);
			}

			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		//显示数据应该在 m_hMemRGBmp 中准备好了
		if( m_nBeginDrawOp==1 )
		{

		}
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFinish();
			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();

			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glViewport(0, 0, m_szDC.cx, m_szDC.cy);
			m_dt.DisplayInterleavedTextures(m_szDC,m_rcView,IsPolarizedVisibleLeft(0));
			
			glFinish();
			PrepareMemBoard(m_rcView.right,m_rcView.bottom);
				ReadGLPixels(m_rcView);

			EnableLogicOp(TRUE);
			
			m_switchRC.Restore();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			//显示数据应该在 m_hMemRGBmp 中准备好了
			
			((C2DGDIDrawingContext*)m_pLeft)->TurnoffMemBmpBoard();
			((C2DGDIDrawingContext*)m_pRight)->TurnoffMemBmpBoard();
			
			((C2DGDIDrawingContext*)m_pLeft)->SetAsMemOpOnly(FALSE);
			((C2DGDIDrawingContext*)m_pRight)->SetAsMemOpOnly(FALSE);
		}
	}

	m_nDrawState = drawNoneState;
	m_nBeginDrawOp--;

}


BOOL CStereoDrawingContext::GetDrawBitmap(HBITMAP hbitmap,CRect bmpRect,CRect *pRect )
{
	if(!hbitmap) return FALSE;
	CRect rect = *pRect;
	if (pRect==NULL)
	{
		pRect = &m_rcView;
	}
	else
	{
		*pRect = m_rcView&rect;
	}
	if( m_nDisplayMode==modeSingle||m_nDisplayMode==modeShutterStereo||m_nDisplayMode==modeSplit )
	{		
		OverlapBitmap(m_hMemBoardBmp,NULL,*pRect,hbitmap,NULL,bmpRect);
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		OverlapBitmap(m_hMemRGBmp,NULL,*pRect,hbitmap,NULL,bmpRect);
	}
	else if(m_nDisplayMode==modeInterleavedStereo)
	{
		OverlapBitmap(m_hMemRGBmp,NULL,*pRect,hbitmap,NULL,bmpRect);
	}
	return TRUE;
}

void CStereoDrawingContext::BeginDrawingOverlay()
{
	if( m_nDisplayMode==modeSingle )
	{
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		m_bDrawBack = FALSE;
		m_switchRC.Active(m_hOriginalDC,m_hglRC_overlay);
	}
	else if( m_nDisplayMode==modeSplit )
	{
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
	}

	m_nDrawState = drawOverlay;
}


void CStereoDrawingContext::EndDrawingOverlay()
{
	if( m_nDisplayMode==modeSingle )
	{
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		glFinish();
		m_switchRC.Restore();
	}
	else if( m_nDisplayMode==modeSplit )
	{
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
	}

	m_nDrawState = drawNoneState;
}


BOOL CStereoDrawingContext::MakeInterleavedBmp(HBITMAP hBmp1, int x1, int y1, HBITMAP hBmp2, CRect rc2, BOOL bOdd)
{
	if( hBmp1==NULL || hBmp2==NULL )return FALSE;
	
	DIBSECTION info1,info2;
	if( ::GetObject( hBmp1, sizeof(DIBSECTION), &info1)==0 )return FALSE;
	if( ::GetObject( hBmp2 , sizeof(DIBSECTION), &info2 )==0 )return FALSE;
	
	BYTE* pBits1 =(BYTE*)info1.dsBm.bmBits;
	BYTE* pBits2 =(BYTE*)info2.dsBm.bmBits;
	
	if( pBits1==NULL || pBits2==NULL )return FALSE;
	if( info1.dsBm.bmBitsPixel!=info2.dsBm.bmBitsPixel )
		return FALSE;
	
	int BitCnt = info1.dsBm.bmBitsPixel;

	rc2 &= CRect(0,0,info2.dsBm.bmWidth,info2.dsBm.bmHeight);
	
	int	readWidth= rc2.Width();
	if( readWidth>(info1.dsBm.bmWidth-x1) )
		readWidth = (info1.dsBm.bmWidth-x1);

	int readBytes= (readWidth*BitCnt+7)/8;
	int LineWidth1 = (info1.dsBm.bmWidth*BitCnt+7)/8;
	int LineWidth2 = (info2.dsBm.bmWidth*BitCnt+7)/8;
	
	LineWidth1 = ((LineWidth1+3)&(~3));
	LineWidth2  = ((LineWidth2+3)&(~3));
	
	BYTE* pTmp1 = pBits1+(info1.dsBm.bmHeight-1-y1)*LineWidth1
		+ x1*BitCnt/8;
	
	BYTE* pTmp2 = pBits2+(info2.dsBm.bmHeight-1-rc2.top)*LineWidth2
		+ rc2.left*BitCnt/8;
	
	int height = rc2.Height();
	if( (height+height)>(info1.dsBm.bmHeight-y1) )
		height = (info1.dsBm.bmHeight-y1)/2;
	
	if( bOdd )
	{
		for( int i=0; i<height; i++)
		{
			memcpy(pTmp1,pTmp2,readBytes);
			
			pTmp1 -= LineWidth1;
			pTmp1 -= LineWidth1;
			pTmp2 -= LineWidth2;
		}
	}
	else
	{
		pTmp1 -= LineWidth1;
		for( int i=0; i<height; i++)
		{
			memcpy(pTmp1,pTmp2,readBytes);
			
			pTmp1 -= LineWidth1;
			pTmp1 -= LineWidth1;
			pTmp2 -= LineWidth2;
		}
	}
	return TRUE;
}


void CStereoDrawingContext::OnFinishDrawSide()
{
	if( m_nDisplayMode==modeSingle )
	{
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			glFlush();
			//glFinish();

			//SwapBuffers(m_hOriginalDC);
			//wglSwapLayerBuffers(m_hOriginalDC,WGL_SWAP_MAIN_PLANE);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else
		{
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			CRect rcDraw = GetSplitDrawRect(m_nDrawSide);
			
			CRect rcView = GetSplitViewRect(m_nDrawSide);
			PrepareMemBoard(rcDraw.right,rcDraw.bottom);
			ReadGLPixels(rcDraw);

			HBITMAP hOldBmp = (HBITMAP)::SelectObject(m_hMemBoardDC,m_hMemBoardBmp);
			
			::BitBlt(m_hOriginalDC,
				rcView.left,rcView.top,rcView.Width(),rcView.Height(),
				m_hMemBoardDC,0,0,SRCCOPY);
			
			::SelectObject(m_hMemBoardDC,hOldBmp);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			CRect rcDraw = GetSplitDrawRect(m_nDrawSide);
			CRect rcView = GetSplitViewRect(m_nDrawSide);
			
			::BitBlt(m_hOriginalDC,
				rcView.left,rcView.top,rcView.Width(),rcView.Height(),
				((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardDC(),
				rcDraw.left,rcDraw.top,SRCCOPY);
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			ReadGLPixels(m_rcView);
			
			if( m_nDrawSide==drawLeft )
			{
				OverlapBitmap(m_hMemBoardBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
			else
			{
				OverlapBitmap(m_hMemBoardBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			HBITMAP hBmp = ((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp();

			if( m_nDrawSide==drawLeft )
			{
				OverlapBitmap(hBmp,NULL,m_rcView,
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
			else
			{
				OverlapBitmap(hBmp,NULL,m_rcView,
					m_hMemRGBmp,NULL,CRect(0,0,m_rcView.Width(),m_rcView.Height()),
					FALSE,0,GetCurContext()->GetRGBMask());
			}
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			m_dt.EndRenderToTexture();
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			HBITMAP hBmp = ((C2DGDIDrawingContext*)GetCurContext())->GetMemBoardBmp();

			CRect rcView;
			GetCurContext()->GetViewRect(rcView);
			
			if( m_nDrawSide==drawLeft )
			{
				MakeInterleavedBmp(m_hMemRGBmp,0,0,
					hBmp,rcView,m_bInverseStereo?TRUE:FALSE);
			}
			else
			{
				MakeInterleavedBmp(m_hMemRGBmp,0,0,
					hBmp,rcView,m_bInverseStereo?FALSE:TRUE);
			}
		}
	}
}


void CStereoDrawingContext::DrawGrBuffer(GrElementList *pList, int mode)
{
	if( m_nDisplayMode==modeInterleavedStereo )
	{
		CRect rcView;
			
		GetCurContext()->GetViewRect(rcView);
		
		CRect rcNewView = rcView;
		rcNewView.top *= 2;
		rcNewView.bottom *= 2;
		
		GetCurContext()->SetViewRect(rcNewView);

		DrawGrBuffer_interleaved(pList, mode);

		GetCurContext()->SetViewRect(rcView);
	}
	else
	{
		//OpenGL 方式
		if( m_nDisplayMode==modeShutterStereo )
		{
			DrawGrBuffer_GL(pList, mode);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			DrawGrBuffer_D3D(pList, mode);
		}
		else
		{
			CCoordSys *pOldCS = GetCoordSys();
			CCoordSys moveCS;
			CCoordSys compound;
			if( m_nDisplayMode==modeSplit )
			{
				CCoordSys *pCS = pOldCS;
				int xoff = GetSplitX(m_nDrawSide);

				//定义偏移换算关系
				double m[16];
				Matrix44FromMove(xoff,0,0,m);
				moveCS.Create44Matrix(m);
				
				//为cache中的数据创造复合坐标系（也就是cache的坐标系）
				CArray<CCoordSys*,CCoordSys*> arr;
				if( pOldCS )arr.Add(pOldCS);
				arr.Add(&moveCS);
				compound.CreateCompound(arr);
				SetCoordSys(&compound);
			}

			DrawGrBuffer_GDI(m_hOriginalDC,pList, mode);

			SetCoordSys(pOldCS);
		}
	}
}

void CStereoDrawingContext::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	GetCurContext()->DrawSelectionMark2d(pList,mode,clr,wid);
}

void CStereoDrawingContext::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	GetCurContext()->DrawSelectionMark(pList,mode,clr,wid);
}


void CStereoDrawingContext::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	if( m_nDisplayMode==modeInterleavedStereo )
	{
		CRect rcView;
		
		GetCurContext()->GetViewRect(rcView);
		
		CRect rcNewView = rcView;
		rcNewView.top *= 2;
		rcNewView.bottom *= 2;
		
		GetCurContext()->SetViewRect(rcNewView);

		DrawGrBuffer2d_interleaved(pList, mode);

		GetCurContext()->SetViewRect(rcView);
	}
	else
	{
		//OpenGL 方式
		if( m_nDisplayMode==modeShutterStereo )
		{
			DrawGrBuffer2d_GL(pList, mode);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			DrawGrBuffer2d_D3D(pList, mode);
		}
		else if (m_nDisplayMode==modeRGStereo)
		{
			DWORD mask = GetRGBMask();
			BOOL bMonoMode = m_bMonoMode;
			COLORREF monoColor = m_monoColor;

			if( m_nDrawSide==drawLeft )
			{
				SetRGBMask(GetLeftContext()->GetRGBMask());
				BOOL b;
				COLORREF c;
				GetLeftContext()->GetMonoColor(b,c);

				if( mode&modeNormal )
				{
					BOOL b2;
					COLORREF c2;
					GetRightContext()->GetMonoColor(b2,c2);
					c = c|c2;
				}

				EnableMonocolor(b,c);
			}
			else
			{
				SetRGBMask(GetRightContext()->GetRGBMask());
				BOOL b;
				COLORREF c;
				GetRightContext()->GetMonoColor(b,c);

				if( mode&modeNormal )
				{
					BOOL b2;
					COLORREF c2;
					GetLeftContext()->GetMonoColor(b2,c2);
					c = c|c2;
				}
				EnableMonocolor(b,c);
			}

			DrawGrBuffer2d_GDI(m_hOriginalDC,pList, mode);

			m_bMonoMode = bMonoMode;
			m_monoColor = monoColor;
			SetRGBMask(mask);			
 		}
		else if( m_nDisplayMode==modeSingle )
		{
			if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
			{
				DrawGrBuffer2d_GL(pList, mode);
			}
			// D3D 方式
			else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
			{
				
			}
			else
				DrawGrBuffer2d_GDI(m_hOriginalDC,pList, mode);
		}
		else if (m_nDisplayMode==modeSplit)
		{
			CCoordSys *pOldCS = GetCoordSys();
			CCoordSys moveCS;
			CCoordSys compound;

			CRect rcView;
			GetViewRect(rcView);

			CCoordSys *pCS = pOldCS;
			int xoff = GetSplitX(m_nDrawSide);
			
			//定义偏移换算关系
			double m[16];
			Matrix44FromMove(xoff,0,0,m);
			moveCS.Create44Matrix(m);
			
			//为cache中的数据创造复合坐标系（也就是cache的坐标系）
			CArray<CCoordSys*,CCoordSys*> arr;
			if( pOldCS )arr.Add(pOldCS);
			arr.Add(&moveCS);
			compound.CreateCompound(arr);
			SetCoordSys(&compound);

			m_rcView = GetSplitViewRect(m_nDrawSide);
			
			DrawGrBuffer2d_GDI(m_hOriginalDC,pList, mode);
			
			SetCoordSys(pOldCS);
			m_rcView = rcView;
		}

	}
}


void CStereoDrawingContext::SaveBmp(int idx, CRect rect)
{
	if( m_nDisplayMode==modeInterleavedStereo )
	{
		rect.top *= 2;
		rect.bottom *= 2;

		if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
			return;
		
		CDC *pDC = CDC::FromHandle(m_hOriginalDC);
		
		if( NULL!=(HDC)m_saveData[idx].dcMemRect )
			m_saveData[idx].dcMemRect.DeleteDC();
		if( NULL!=(HBITMAP)m_saveData[idx].bmpRect )
			m_saveData[idx].bmpRect.DeleteObject();
		
		m_saveData[idx].dcMemRect.CreateCompatibleDC(pDC);
		
		int w = rect.Width();
		int h = rect.Height();
		
		m_saveData[idx].bmpRect.CreateCompatibleBitmap(pDC,w,h);
		CBitmap *pOldBmp = m_saveData[idx].dcMemRect.SelectObject(&m_saveData[idx].bmpRect);
		
		m_saveData[idx].dcMemRect.BitBlt(0,0,rect.Width(),rect.Height(),pDC,rect.left,rect.top,SRCCOPY);
		
		m_saveData[idx].dcMemRect.SelectObject(pOldBmp);
		
		m_saveData[idx].rcSaveRect = rect;
		m_saveData[idx].nSaveRectType = ImgSaveData::typeGDI;
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		SaveBmp_GL(idx,rect);
	}
	else if( m_nDisplayMode==modeSplit )
	{
		int xoff = GetSplitX(m_nDrawSide);
		rect.OffsetRect(xoff,0);

		SaveBmp_GDI(m_hOriginalDC,idx,rect);
	}
	else if( m_nDisplayMode==modeSingle )
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			SaveBmp_GL(idx,rect);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		else
			SaveBmp_GDI(m_hOriginalDC,idx,rect);
	}
	else if (m_nDisplayMode==modeRGStereo)
	{
		SaveBmp_GDI(m_hOriginalDC,idx,rect);
	}
	else
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			SaveBmp_GL(idx,rect);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		else
			SaveBmp_GDI(m_hOriginalDC,idx,rect);
	}
}


void CStereoDrawingContext::RestoreBmp(int idx)
{
	if( m_nDisplayMode==modeInterleavedStereo )
	{
		if( idx<0 || idx>=sizeof(m_saveData)/sizeof(m_saveData[0]) )
			return;
		
		if( m_saveData[idx].nSaveRectType==ImgSaveData::typeNone )
			return;
		
		if( NULL==(HDC)m_saveData[idx].dcMemRect || NULL==(HBITMAP)m_saveData[idx].bmpRect )
			return;
		
		CBitmap *pOldBmp = m_saveData[idx].dcMemRect.SelectObject(&m_saveData[idx].bmpRect);
		
		CDC *pDC = CDC::FromHandle(m_hOriginalDC);
		
		pDC->BitBlt( m_saveData[idx].rcSaveRect.left,m_saveData[idx].rcSaveRect.top,
			m_saveData[idx].rcSaveRect.Width(),m_saveData[idx].rcSaveRect.Height(),
			&m_saveData[idx].dcMemRect,0,0,SRCCOPY);
		
		m_saveData[idx].dcMemRect.SelectObject(pOldBmp);
		return;
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
		RestoreBmp_GL(idx);
	}
	else if( m_nDisplayMode==modeSingle )
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			RestoreBmp_GL(idx);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		else
			RestoreBmp_GDI(m_hOriginalDC,idx);
	}
	else if(m_nDisplayMode==modeSplit)
	{
		RestoreBmp_GDI(m_hOriginalDC,idx);
	}
	else if (m_nDisplayMode==modeRGStereo)
	{
		RestoreBmp_GDI(m_hOriginalDC,idx);
	}
	else
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			RestoreBmp_GL(idx);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		else
			RestoreBmp_GDI(m_hOriginalDC,idx);
	}
}



class CDrawPolarizedLine
{
public:
	CDrawPolarizedLine(){
		m_pDC = NULL;
	}
	~CDrawPolarizedLine(){}
	
	void SetDC( CSuperDrawDC *pDC ){
		m_pDC = pDC;
	}
	void SetStartVisible( BOOL bVisible ){
		m_bStartVisible = bVisible;
	}
	void MoveTo(int x, int y){
		m_nx = x; m_ny = y;		
	}
	void LineTo(int x, int y){
		LineDDA(m_nx,m_ny,x,y,(LINEDDAPROC)CDrawPolarizedLine::Callback,(LPARAM)this);
		m_nx = x; m_ny = y;	
	}
	void Point(int x, int y, COLORREF clr, int size){
		if( m_bStartVisible )
		{
			{
				m_pDC->Begin(2);
				m_pDC->MoveTo(x,y+y);
				m_pDC->LineTo(x+1,y+y);
				m_pDC->End();
			}
		}
		else
		{
			{
				m_pDC->Begin(2);
				m_pDC->MoveTo(x,y+y+1);
				m_pDC->LineTo(x+1,y+y+1);
				m_pDC->End();
			}
		}
	}
	
	static void Callback(int x, int y, LPARAM pData){
		CDrawPolarizedLine *pthis = (CDrawPolarizedLine*)pData;
		if( pthis->m_bStartVisible )
		{
			{
				pthis->m_pDC->Begin(2);
				pthis->m_pDC->MoveTo(x,y+y);
				pthis->m_pDC->LineTo(x+1,y+y);
				pthis->m_pDC->End();
			}
		}
		else
		{
			{
				pthis->m_pDC->Begin(2);
				pthis->m_pDC->MoveTo(x,y+y+1);
				pthis->m_pDC->LineTo(x+1,y+y+1);
				pthis->m_pDC->End();
			}
		}
	}
	
private:
	BOOL m_bStartVisible;
	CSuperDrawDC *m_pDC;
	int m_nx, m_ny;
};


BOOL CStereoDrawingContext::IsPolarizedVisibleLeft(int y)
{
	//偏振立体的模式是：（以0为索引起点）偶数行通左片，奇数行通右片
	CPoint test(0,y);
	::ClientToScreen(m_hInterleavedWnd,&test);
	BOOL bLeft = ((test.y%2)==0);
	
	if( m_bInverseStereo )bLeft = !bLeft;
	return bLeft;
}

void CStereoDrawingContext::DrawGrBuffer_interleaved(GrElementList *pList, int mode)
{
	CRect rcView = m_rcView;
	Envelope e0(rcView.left,rcView.right,rcView.top,rcView.bottom),e;

	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	CellDef cell;
	BaseLineType linetype; 
	CCoordSys *pCS = GetCurContext()->GetCoordSys();
	
	float gscale = 1.0;
	if( pCS )gscale = pCS->CalcScale();
	
	HDC hdc = GetContext();
// 	CGdiDrawingDC dc0(hdc);
// 
// 	if( (mode&modeDynamic)!=0 && m_bSupportLogicOp )
// 		::SetROP2(hdc,R2_XORPEN);
// 	else
// 		::SetROP2(hdc,R2_COPYPEN);
	CSuperDrawDC dc0;
	dc0.CreateType(DRAWDC_GDI,hdc);
	
	dc0.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc0.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc0.SetDrawMode(DRAWMODE_COPY);

	CDrawPolarizedLine dc;
	dc.SetDC(&dc0);
	dc.SetStartVisible(m_nDrawSide==drawLeft?IsPolarizedVisibleLeft(0):(!IsPolarizedVisibleLeft(0)));

	CDrawPolarizedLine *pDC = &dc;
	
	Graph *gr;
	PT_4D pt4d;

	GrElement *ele = pList->head;
	for( ; ele!=NULL; ele=ele->next )
	{
		if( !ele->bvisible )
			continue;
		
		GrBuffer *pBuf= (GrBuffer*)ele->pBuf;
		
		gr = pBuf->HeadGraph();
		
		for( ; gr!=NULL; gr=gr->next )
		{
			e = GetEnvelopeOfGraph_fast(gr,m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY,gscale);
			if( pCS )e.TransformGrdToClt(pCS,1);

			{
				if( !e.bIntersect(&e0) )continue;

				COLORREF dcolor = gr->bUseBackColor?GetBackColor():GetDrawColor(gr->color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);

				if( IsGrPoint(gr) )
				{
					const GrPoint *cgr = (const GrPoint*)gr;

					pt4d = cgr->pt;
					if( pCS )pCS->GroundToClient(&pt4d,1);
					
					//图元块
					if( cgr->cell>0 && pCellLib!=NULL && cgr->cell<=pCellLib->GetCellDefCount() )
					{
						dc0.SetColor(dcolor);

						cell = pCellLib->GetCellDef(cgr->cell-1);
						if( cell.m_pgr!=NULL )
						{
							GrBuffer buf;
							buf.AddBuffer(cell.m_pgr,0);
							buf.Zoom(cgr->kx,cgr->ky);
							buf.Rotate(cgr->angle);
							
							buf.Move(pt4d.x,pt4d.y);
							buf.ZoomCompressInfo(max(cgr->kx*m_fDrawCellKX,cgr->ky*m_fDrawCellKY));

							DrawGrBuffer(&GrElementList(&buf), mode);
						}
					}
					//缺省处理行为
					else
					{
						dc0.Begin(1);
						if( !cgr->bGrdSize )
							pDC->Point(PIXELX_GDI(pt4d.x),PIXELY_GDI(pt4d.y),dcolor,cgr->kx);
						else 
							pDC->Point(PIXELX_GDI(pt4d.x),PIXELY_GDI(pt4d.y),dcolor,cgr->kx*gscale);
					}
				}
				else if( IsGrLineString(gr) )
				{
					const GrLineString *cgr = (const GrLineString*)gr;
					dc0.SetColor(dcolor);
					
					for( int i=0; i<cgr->ptlist.nuse; i++)
					{
						pt4d = (PT_3D)cgr->ptlist.pts[i];
						if( pCS )pCS->GroundToClient(&pt4d,1);

						if( IsGrPtCodeMoveTo(cgr->ptlist.pts+i) )
							pDC->MoveTo(PIXELX_GDI(pt4d.x),PIXELY_GDI(pt4d.y));
						else
							pDC->LineTo(PIXELX_GDI(pt4d.x),PIXELY_GDI(pt4d.y));
					}
				}
				else if( IsGrPolygon(gr) )
				{
					const GrPolygon *cgr = (const GrPolygon*)gr;
					dc0.SetColor(dcolor);
					
					for( int i=0; i<cgr->ptlist.nuse; i++)
					{
						pt4d = (PT_3D)cgr->ptlist.pts[i];
						if( pCS )pCS->GroundToClient(&pt4d,1);
						
						if( IsGrPtCodeMoveTo(cgr->ptlist.pts+i) )
							pDC->MoveTo(PIXELX_GDI(pt4d.x),PIXELY_GDI(pt4d.y));
						else
							pDC->LineTo(PIXELX_GDI(pt4d.x),PIXELY_GDI(pt4d.y));
					}
				}
				else if( IsGrText(gr) )
				{
					HDC hdc = GetContext();
					const GrText *cgr = (const GrText*)gr;
					COLORREF fontColor = dcolor;

					float fh = cgr->settings.fHeight;

					if( cgr->bGrdSize )
					{
						fh = fh*gscale*(m_fDrawCellKX+m_fDrawCellKY)/2/*max(m_fDrawCellKX,m_fDrawCellKY)*/;
					}
					
					pt4d = cgr->pt;
					if( pCS!=NULL )pCS->GroundToClient(&pt4d,1);

					
					TextSettings settings;
					memcpy(&settings,&(cgr->settings),sizeof(TextSettings));
					settings.fHeight = fh;
					if (cgr->bRotateWithView) settings.fTextAngle += m_fDrawCellAngle*180/PI;
					
					CTextRender render;
					if (cgr->text!=NULL)
					{
						render.DrawText(cgr->text,pt4d.x,pt4d.y,hdc,&settings,fontColor);
					}
				}
			}
		}
	}
}

void CStereoDrawingContext::DrawGrBuffer2d_interleaved(GrElementList *pList, int mode)
{
	CRect rcView = m_rcView;
	
	Envelope e0(rcView.left,rcView.right,rcView.top,rcView.bottom),e;

	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	CellDef cell;
	BaseLineType linetype; 

	CCoordSys *pCS = GetCurContext()->GetCoordSys();
	
	float gscale = 1.0;
	if( pCS )gscale = pCS->CalcScale();
	
	HDC hdc = GetContext();
// 	CGdiDrawingDC dc0(hdc);
// 
// 	if( (mode&modeDynamic)!=0 && m_bSupportLogicOp )
// 		::SetROP2(hdc,R2_XORPEN);
// 	else
// 		::SetROP2(hdc,R2_COPYPEN);
	CSuperDrawDC dc0;
	dc0.CreateType(DRAWDC_GDI,hdc);
	
	dc0.SetCoordMode(TRUE,FALSE,m_szDC);
	
	//设置异或模式
	if( mode&CDrawingContext::modeDynamic && m_bSupportLogicOp )
	{
		dc0.SetDrawMode(DRAWMODE_XOR);
	}
	else
		dc0.SetDrawMode(DRAWMODE_COPY);
	
	CDrawPolarizedLine dc;
	dc.SetDC(&dc0);
	dc.SetStartVisible(m_nDrawSide==drawLeft?IsPolarizedVisibleLeft(0):(!IsPolarizedVisibleLeft(0)));
	
	CDrawPolarizedLine *pDC = &dc;

	if( pCS )e0.TransformCltToGrd(pCS);
	
	Graph2d *gr;
	PT_4D pt2d;
	
	GrElement *ele = pList->head;
	for( ; ele!=NULL; ele=ele->next )
	{
		if( !ele->bvisible )
			continue;
		
		GrBuffer2d *pBuf= (GrBuffer2d*)ele->pBuf;
		
		float xoff,yoff;
		pBuf->GetOrigin(xoff,yoff);
		
		gr = pBuf->HeadGraph();
		
		for( ; gr!=NULL; gr=gr->next )
		{
			e = GetEnvelopeOfGraph2d(gr,m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY,gscale);
			e.m_xh += xoff;
			e.m_xl += xoff;
			e.m_yl += yoff;
			e.m_yh += yoff;

			if( max(e.m_xl,e0.m_xl)>min(e.m_xh,e0.m_xh) ||
				max(e.m_yl,e0.m_yl)>min(e.m_yh,e0.m_yh) )
				continue;

			COLORREF dcolor = gr->bUseBackColor?GetBackColor():GetDrawColor(gr->color,(mode&modeDynamic)!=0 && m_bSupportLogicOp);

			if( IsGrPoint2d(gr) )
			{
				const GrPoint2d *cgr = (const GrPoint2d*)gr;

				pt2d.x = cgr->pt.x + (double)xoff;
				pt2d.y = cgr->pt.y + (double)yoff;
				
				if( pCS )pCS->GroundToClient(&pt2d,1);
				
				//图元块
				if( cgr->cell>0 && pCellLib!=NULL && cgr->cell<=pCellLib->GetCellDefCount() )
				{
					dc0.SetColor(dcolor);

					cell = pCellLib->GetCellDef(cgr->cell-1);
					if( cell.m_pgr!=NULL )
					{
						GrBuffer2d buf;
						buf.AddBuffer(cell.m_pgr);
						buf.Zoom(cgr->kx,cgr->ky);
						buf.Rotate(cgr->angle);
						
						buf.Move(cgr->pt.x+xoff,cgr->pt.y+yoff);
						buf.ZoomCompressInfo(max(cgr->kx*m_fDrawCellKX,cgr->ky*m_fDrawCellKY));

						DrawGrBuffer2d(&GrElementList(&buf), mode);
					}
				}
				//缺省处理行为
				else
				{
					dc0.SetColor(dcolor);
					
					if( !cgr->bGrdSize )
						pDC->Point(PIXELX_GDI(pt2d.x),PIXELY_GDI(pt2d.y),dcolor,cgr->kx);
					else 
						pDC->Point(PIXELX_GDI(pt2d.x),PIXELY_GDI(pt2d.y),dcolor,cgr->kx*gscale*m_fDrawCellKX);
				}
			}
			else if( IsGrLineString2d(gr) )
			{
				const GrLineString2d *cgr = (const GrLineString2d*)gr;
				dc0.SetColor(dcolor);
				
				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					pt2d.x = cgr->ptlist.pts[i].x+(double)xoff;
					pt2d.y = cgr->ptlist.pts[i].y+(double)yoff;
					if( pCS )pCS->GroundToClient(&pt2d,1);

					if( IsGrPtCodeMoveTo(cgr->ptlist.pts+i) )
						pDC->MoveTo(PIXELX_GDI(pt2d.x),PIXELY_GDI(pt2d.y));
					else
						pDC->LineTo(PIXELX_GDI(pt2d.x),PIXELY_GDI(pt2d.y));
				}
			}
			else if( IsGrPolygon2d(gr) )
			{
				const GrPolygon2d *cgr = (const GrPolygon2d*)gr;
				dc0.SetColor(dcolor);
				
				for( int i=0; i<cgr->ptlist.nuse; i++)
				{
					pt2d.x = cgr->ptlist.pts[i].x+(double)xoff;
					pt2d.y = cgr->ptlist.pts[i].y+(double)yoff;
					if( pCS )pCS->GroundToClient(&pt2d,1);
					
					if( IsGrPtCodeMoveTo(cgr->ptlist.pts+i) )
						pDC->MoveTo(PIXELX_GDI(pt2d.x),PIXELY_GDI(pt2d.y));
					else
						pDC->LineTo(PIXELX_GDI(pt2d.x),PIXELY_GDI(pt2d.y));
				}
			}
			else if( IsGrText2d(gr) )
			{
				HDC hdc = GetContext();
				const GrText2d *cgr = (const GrText2d*)gr;
				COLORREF fontColor = dcolor;

				float fh = cgr->settings.fHeight;

				if( cgr->bGrdSize )
				{
					fh = fh*gscale*(m_fDrawCellKX+m_fDrawCellKY)/2/*max(m_fDrawCellKX,m_fDrawCellKY)*/;
				}
				
				pt2d.x = cgr->pt.x+(double)xoff;
				pt2d.y = cgr->pt.y+(double)yoff;


				if( pCS )pCS->GroundToClient(&pt2d,1);



				TextSettings settings;
				memcpy(&settings,&(cgr->settings),sizeof(TextSettings));
				settings.fHeight = fh;
				if (cgr->bRotateWithView)  settings.fTextAngle += m_fDrawCellAngle*180/PI;
				
				CTextRender render;
				if (cgr->text!=NULL)
				{
					render.DrawText(cgr->text,PIXELX_GDI(pt2d.x),PIXELY_GDI(pt2d.y),hdc,&settings,fontColor);
			}
			}
		}
	}
}


void CStereoDrawingContext::EraseBackground()
{
	if( m_nDrawState==drawOverlay )
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else if( m_nDrawState==drawErasable )
	{
		if( m_nDisplayMode==modeShutterStereo )
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			HDC hdc = m_hOriginalDC;
			CBrush brush(m_clrBack);
			
			::FillRect(hdc,m_rcView,brush);
		}
	}
	else
		GetCurContext()->EraseBackground();
}


void CStereoDrawingContext::ScrollGlPixel(int dx, int dy, int dp, int dq, CRect *pRcView, 
										CArray<CRect,CRect>* pLRects, 
										CArray<CRect,CRect>* pRRects)
{
	CRect rcClient, rcSrc, rcDest;
	rcClient = CRect(0,0,m_szDC.cx,m_szDC.cy);
	
	CRect rect;
	if( pRcView==NULL )rect = rcClient;
	else rect = rcClient&*pRcView;

	CRgn rgnScroll,rgnCopy[2],rgnUpdate[2];
	rgnScroll.CreateRectRgn( rect.left,rect.top,rect.right,rect.bottom );
	rgnUpdate[0].CreateRectRgn(0,0,0,0);
	rgnUpdate[1].CreateRectRgn(0,0,0,0);

	CActiveGLRC change(m_hOriginalDC,m_hglRC);

	glViewport( 0,0,rcClient.right,rcClient.bottom );
	glPixelZoom(1.0,1.0);
	glLogicOp(GL_COPY);

	CopyFrontFrameToToBack(m_szDC);
	m_bDrawBack = TRUE;

	rcDest = rect;
	rcDest.OffsetRect(dx,dy);
	rcDest.IntersectRect(&rcDest,&rect);

	rcSrc = rcDest;
	rcSrc.OffsetRect(-dx,-dy);

	rgnCopy[0].CreateRectRgn( rcDest.left,rcDest.top,rcDest.right,rcDest.bottom);

	if( dx!=0 || dy!=0 )
	{
		SetGLDrawBuffer(drawLeft);

		glRasterPos2i(rcDest.left,rcClient.bottom-rcDest.bottom);
		try
		{
			glCopyPixels(rcSrc.left,rcClient.bottom-rcSrc.bottom,rcSrc.Width(),rcSrc.Height(),GL_COLOR ); 
		}
		catch(...)
		{
			GLvoid *pBuf = new BYTE[rcSrc.Width()*(rcSrc.Height()+2)*3];
			if( pBuf )
			{
				glPixelStorei(GL_PACK_ROW_LENGTH,0);	
				glPixelStorei(GL_PACK_SKIP_PIXELS,0);
				glPixelStorei(GL_PACK_SKIP_ROWS,0);
				
				glReadPixels(rcSrc.left,rcClient.bottom-rcSrc.bottom,rcSrc.Width(),rcSrc.Height(),GL_RGB,GL_UNSIGNED_BYTE,pBuf);
				glDrawPixels(rcSrc.Width(),rcSrc.Height(),GL_RGB,GL_UNSIGNED_BYTE,pBuf);
				delete[] pBuf;
			}
		}

		rgnUpdate[0].CombineRgn( &rgnScroll,&rgnCopy[0],RGN_DIFF );
		int	nBytes = rgnUpdate[0].GetRegionData(NULL,0);
		RGNDATA *pRgnData = (RGNDATA *)new BYTE[nBytes];
		
		if( pRgnData )
		{			
			rgnUpdate[0].GetRegionData( pRgnData,nBytes );
			
			RECT *pRect = (RECT *)pRgnData->Buffer;
			for( int i=0;i<pRgnData->rdh.nCount;i++ )
			{
				pLRects->Add(CRect(pRect));
				pRect += 1;
			}
			
			delete[] pRgnData;
		}
	}

	SwapBuffers(m_hOriginalDC);
	
	dx += dp;
	dy += dq;
	rcDest = rect;
	rcDest.OffsetRect(dx,dy);
	rcDest.IntersectRect(&rcDest,&rect);

	rcSrc = rcDest;
	rcSrc.OffsetRect(-dx,-dy);		

	rgnCopy[1].CreateRectRgn(rcDest.left,rcDest.top,rcDest.right,rcDest.bottom);

	if( dx!=0 || dy!=0 )
	{
		SetGLDrawBuffer(drawRight);
		
		glRasterPos2i(rcDest.left,rcClient.bottom-rcDest.bottom);

		try
		{
			glCopyPixels(rcSrc.left,rcClient.bottom-rcSrc.bottom,rcSrc.Width(),rcSrc.Height(),GL_COLOR ); 
		}
		catch(...)
		{
			GLvoid *pBuf = new BYTE[rcSrc.Width()*(rcSrc.Height()+2)*3];
			if( pBuf )
			{	
				glPixelStorei(GL_PACK_ROW_LENGTH,0);	
				glPixelStorei(GL_PACK_SKIP_PIXELS,0);
				glPixelStorei(GL_PACK_SKIP_ROWS,0);

				glReadPixels(rcSrc.left,rcClient.bottom-rcSrc.bottom,rcSrc.Width(),rcSrc.Height(),GL_RGB,GL_UNSIGNED_BYTE,pBuf);
				glDrawPixels(rcSrc.Width(),rcSrc.Height(),
					GL_RGB,GL_UNSIGNED_BYTE,pBuf);
				delete[] pBuf;
			}
		}

		rgnUpdate[1].CombineRgn( &rgnScroll,&rgnCopy[1],RGN_DIFF );
		int	nBytes = rgnUpdate[1].GetRegionData(NULL,0);
		RGNDATA *pRgnData = (RGNDATA *)new BYTE[nBytes];
		
		if( pRgnData )
		{			
			rgnUpdate[1].GetRegionData( pRgnData,nBytes );
			
			RECT *pRect = (RECT *)pRgnData->Buffer;
			for( int i=0;i<pRgnData->rdh.nCount;i++ )
			{
				pRRects->Add(CRect(pRect));
				pRect += 1;
			}
			
			delete[] pRgnData;
		}
	}

	m_bDrawBack = FALSE;
	SwapBuffers(m_hOriginalDC);
}


void CStereoDrawingContext::Scroll(int dx, int dy, int dp, int dq, CRect *pRcView, 
								   CArray<CRect,CRect>* pLRects, 
								   CArray<CRect,CRect>* pRRects,
								   CArray<CRect,CRect>* pPublicRects)
{
	if( m_nDisplayMode==modeSingle )
	{
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		{
			if( pPublicRects!=NULL )
			{
				CRect rect(0,0,m_szDC.cx,m_szDC.cy);
				pPublicRects->Add(rect);
			}
		}
		else
		{
			m_pLeft->Scroll(dx,dy,pRcView,pLRects);
		}
		
	//		m_pRight->Scroll(dx+dp,dy+dq,pRcView,pRRects);
	}
	else if( m_nDisplayMode==modeShutterStereo )
	{
#ifndef _WILDCAT_
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
#else
		if( 1 )
#endif
		{
			if( m_bOldStereoScroll )
			{
				CRect rect(0,0,m_szDC.cx,m_szDC.cy);
				pPublicRects->Add(rect);
				return;
			}
			
			m_pLeft->Scroll(dx,dy,pRcView,pLRects);
			m_pRight->Scroll(dx+dp,dy+dq,pRcView,pRRects);
		}
		// GDI 方式
		else
		{
			ScrollGlPixel(dx,dy,dp,dq,pRcView,pLRects,pRRects);
		}
	}
	else if( m_nDisplayMode==modeSplit )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			CRect rcSaveView = m_rcView;
			CRect rcView = GetSplitViewRect(drawLeft);
			SetViewRect(rcView);
			SetGDIViewRect(&rcView);
			CDrawingContext::Scroll(dx,dy,&rcView,pLRects);
			SetViewRect(rcSaveView);

			rcView = GetSplitViewRect(drawRight);
			SetViewRect(rcView);
			SetGDIViewRect(&rcView);
			CDrawingContext::Scroll(dx+dp,dy+dq,&rcView,pRRects);
			SetViewRect(rcSaveView);
			SetGDIViewRect(NULL);
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else
		{
			CRect rcSaveView = m_rcView;
			
			CRect rcView = GetSplitViewRect(drawLeft);
			SetViewRect(rcView);
			SetGDIViewRect(&rcView);
			CDrawingContext::Scroll(dx,dy,&rcView,pLRects);
			SetViewRect(rcSaveView);			
			
			rcView = GetSplitViewRect(drawRight);
			SetViewRect(rcView);
			SetGDIViewRect(&rcView);
			CDrawingContext::Scroll(dx+dp,dy+dq,&rcView,pRRects);

			SetViewRect(rcSaveView);
			SetGDIViewRect(NULL);
		}
	}
	else if( m_nDisplayMode==modeRGStereo )
	{
		if( dp==0 && dq==0 )
		{
			CDrawingContext::Scroll(dx,dy,pRcView,pPublicRects);
			return;
		}

		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			if( pPublicRects!=NULL )
			{
				CRect rect(0,0,m_szDC.cx,m_szDC.cy);
				pPublicRects->Add(rect);
			}
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else
		{
			if( pPublicRects!=NULL )
			{
				CRect rect(0,0,m_szDC.cx,m_szDC.cy);
				pPublicRects->Add(rect);
			}
		}
	}
	else if( m_nDisplayMode==modeInterleavedStereo )
	{
		//OpenGL 方式
		if( m_pLeft->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
		{
			if( pPublicRects!=NULL )
			{
				CRect rect(0,0,m_szDC.cx,m_szDC.cy);
				pPublicRects->Add(rect);
			}
		}
		// D3D 方式
		else if( m_pLeft->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
		{
			
		}
		// GDI 方式
		else
		{
			dy *= 2;
			if( dp==0 && dq==0 )
			{
				CDrawingContext::Scroll(dx,dy,pRcView,pPublicRects);
				return;
			}
			else if( pPublicRects!=NULL )
			{
				CRect rect(0,0,m_szDC.cx,m_szDC.cy);
				pPublicRects->Add(rect);
			}
		}
	}
}


CCoordSys *CStereoDrawingContext::GetCoordSys()
{
	return GetCurContext()->GetCoordSys();
}


void CStereoDrawingContext::SetCoordSys(CCoordSys *pCS)
{
	GetCurContext()->SetCoordSys(pCS);
}


void CStereoDrawingContext::SetRegPath(LPCTSTR path)
{
	_tcscpy(m_strSegPath,path);
}

void CStereoDrawingContext::UpdateRGBMask(BOOL bLeft, BOOL bSave, DWORD& mask)
{
	CWinApp *pApp = AfxGetApp();
	if( !pApp )return;

	CString strKey;

	strKey.Format(_T("RGBMask_%d_%d"),bLeft,m_nDisplayMode);
	if( bSave )
	{
		pApp->WriteProfileInt(m_strSegPath,strKey,mask);
		if (bLeft)
		{
			GetLeftContext()->SetRGBMask(mask);
		}
		else
		{
			GetRightContext()->SetRGBMask(mask);
		}
	}
	else
	{
		mask = pApp->GetProfileInt(m_strSegPath,strKey,-1);
		if( mask==(DWORD)-1 )
		{
			if( m_nDisplayMode==modeRGStereo )
				mask = (bLeft?RGBMASK_RED:(RGBMASK_GREEN|RGBMASK_BLUE));
			else
				mask = RGBMASK_ALL;
		}
		if (bLeft)
		{
			GetLeftContext()->SetRGBMask(mask);
		}
		else
		{
			GetRightContext()->SetRGBMask(mask);
		}
	}
}


void CStereoDrawingContext::EnableLogicOp(BOOL bEnable)
{
	m_bSupportLogicOp = bEnable;
	GetLeftContext()->m_bSupportLogicOp = bEnable;
	GetRightContext()->m_bSupportLogicOp = bEnable;
}

void CStereoDrawingContext::SetBackColor(COLORREF clr)
{
	CDrawingContext::SetBackColor(clr);
	if (m_pLeft)m_pLeft->SetBackColor(clr);
	if (m_pRight)m_pRight->SetBackColor(clr);
		
	if( m_hMemBoardDC )
		::DeleteDC(m_hMemBoardDC);
	m_hMemBoardDC = NULL;
	if( m_hMemBoardBmp )
		::DeleteObject(m_hMemBoardBmp);
	m_hMemBoardBmp = NULL;
	if( m_hMemRGBmp )
		::DeleteObject(m_hMemRGBmp);
	m_hMemRGBmp = NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPrintingDrawingContext, CDrawingContext)

CPrintingDrawingContext::CPrintingDrawingContext()
{
	m_pCurContext = NULL;
	m_pGlContext = NULL;
	m_pGdiContext = NULL;

	m_hTempMemDC = NULL;
	m_hTempMemBmp = NULL;
}

CPrintingDrawingContext::~CPrintingDrawingContext()
{
	if (m_pGlContext) delete m_pGlContext;
    if (m_pGdiContext) delete m_pGdiContext;

	if (m_hTempMemDC) ::DeleteDC(m_hTempMemDC);
	if(m_hTempMemBmp) ::DeleteObject(m_hTempMemBmp);
}
	
CDrawingContext *CPrintingDrawingContext::GetCurContext()
{
// 	int i=0;
// 	if(i==0&&m_pCurContext==m_pGlContext)
// 		WriteToFile((HBITMAP)::SelectObject(m_pCurContext->GetContext(),m_pGlContext->Create32MemBmp(100,100)));
	return  m_pCurContext;
}

BOOL CPrintingDrawingContext::BeginDrawVector()
{
	if (m_pGlContext)
	{
		if(m_hTempMemBmp) ::DeleteObject(m_hTempMemBmp);
		if (m_hTempMemDC) ::DeleteDC(m_hTempMemDC);
		if (m_pGlContext)delete m_pGlContext;
		m_pCurContext = m_pGlContext = NULL;
		return FALSE;
	}

	if(m_pGdiContext)delete m_pGdiContext;
	m_pGdiContext = new C2DPrintGDIDrawingContext;
	if (!m_pGdiContext)return FALSE;
	m_pGdiContext->CreateContext(GetContext());
	
	m_pGdiContext->SetDCSize(GetDCSize());
	m_pGdiContext->SetViewRect(m_rcView);
	m_pGdiContext->SetCoordSys(GetCoordSys());
	m_pGdiContext->SetBackColor(GetBackColor());
	m_pCurContext = m_pGdiContext;
	return TRUE;
}

BOOL CPrintingDrawingContext::EndDrawVector()
{
	if(m_pGdiContext)delete m_pGdiContext;
	m_pCurContext = m_pGdiContext = NULL;
	return TRUE;
}

BOOL CPrintingDrawingContext::BeginDrawImg()
{
	if (m_pGdiContext)
	{
		if(EndDrawVector());
		else
			return FALSE;
	}
	if ((GetDeviceCaps(GetContext(), RASTERCAPS) 
		& RC_BITBLT) == 0) 
	{ 		
		if(m_hTempMemBmp) ::DeleteObject(m_hTempMemBmp);
		if (m_hTempMemDC) ::DeleteDC(m_hTempMemDC);
		return FALSE;
	} 	

	if(m_hTempMemBmp) ::DeleteObject(m_hTempMemBmp);
	if (m_hTempMemDC) ::DeleteDC(m_hTempMemDC);	
	
	if(m_pGlContext)delete m_pGlContext;
	m_pGlContext = new C2DPrintGLDrawingContext;
	if (!m_pGlContext)return FALSE;

	m_hTempMemDC = ::CreateCompatibleDC(GetContext());
	m_hTempMemBmp = m_pGlContext->Create32MemBmp(m_szDC.cx,m_szDC.cy);
	m_hOldBmp = (HBITMAP)::SelectObject(m_hTempMemDC,m_hTempMemBmp);
	
	m_pGlContext->SetDCSize(m_szDC);
	m_pGlContext->SetViewRect(m_rcView);
	m_pGlContext->SetBackColor(GetBackColor());
	m_pGlContext->CreateContext(m_hTempMemDC);
	m_pGlContext->SetCoordSys(GetCoordSys());
	
	m_pCurContext = m_pGlContext;
	return TRUE;
}

BOOL CPrintingDrawingContext::BltImgToPrint(CRect curRect)
{
	if(!GetContext())return FALSE;
	::BitBlt(GetContext(),curRect.left,curRect.top,curRect.Width(),curRect.Height(),m_hTempMemDC,m_rcView.left,m_rcView.top,SRCCOPY);
	return TRUE;
}

BOOL CPrintingDrawingContext::EndDrawImg()
{
	if (m_pGlContext)delete m_pGlContext;
	::SelectObject(m_hTempMemDC,m_hOldBmp);
	if(m_hTempMemBmp) ::DeleteObject(m_hTempMemBmp);
	if (m_hTempMemDC) ::DeleteDC(m_hTempMemDC);
	m_pCurContext = m_pGlContext = NULL;
	return TRUE;
}

MyNameSpaceEnd

