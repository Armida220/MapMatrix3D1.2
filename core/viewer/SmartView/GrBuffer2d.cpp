// Copyright (C) 2004 - 2005 Gibuilder Group

#include "stdafx.h"
#include <float.h>
#include <math.h>
#include "GrBuffer2d.h"
#include "SmartViewFunctions.h"
#include "CoordWnd.h"
#include "GrBuffer.h"
#include "symbolbase.h"
#include "markup.h"
#include "PlotWChar.h "


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAX_GRAPH_PTNUM				256
#define _ttof _tstof

#define BID_none					0
#define BID_type					1
#define BID_color					2
#define BID_bUseSelfcolor			3
#define BID_bUseBackColor			4
#define BID_isCompress				5
#define BID_ptlist					6
#define BID_kxy						8
#define BID_angle					9
#define BID_bGrdSize				10
#define BID_bFlat					11
#define BID_cell					12
#define BID_width					13
#define BID_pt						14
#define BID_range					15
#define BID_style					16
#define BID_fDrawScale				17
#define BID_index					18
#define BID_filltype				19
#define BID_textSettings			20
#define BID_text					21
#define BID_bGrdWid					22
#define BID_nGraph					23
#define BID_org						24
#define BID_end						25
#define BID_polygon_pts				26


MyNameSpaceBegin


IMPLEMENT_MEMPOOL(GrVertexList2d);
IMPLEMENT_MEMPOOL(GrPoint2d);
IMPLEMENT_MEMPOOL(GrLineString2d);
IMPLEMENT_MEMPOOL(GrPolygon2d);
IMPLEMENT_MEMPOOL(GrText2d);
IMPLEMENT_MEMPOOL(GrBuffer2d);


void DeleteGraph2d(Graph2d *p)
{
	if( IsGrPoint2d(p) )
		delete (GrPoint2d*)p;
	else if( IsGrPointString2d(p) )
		delete (GrPointString2d*)p;
	else if( IsGrPointString2dEx(p) )
		delete (GrPointString2dEx*)p;
	else if( IsGrLineString2d(p) )
		delete (GrLineString2d*)p;
	else if( IsGrPoint2d(p) )
		delete (GrPolygon2d*)p;
	else if( IsGrText2d(p) )
		delete (GrText2d*)p;
}


void GrVertexList2d::CutSpace()
{
	if( nuse<=0 )
	{
		if( pts )delete[] pts;
		pts = NULL;
		nuse = 0;
		nlen = 0;
	}
	else if( nlen>nuse )
	{
		GrVertex2d *pts2 = new GrVertex2d[nuse];
		if( pts2 )
		{
			memcpy(pts2,pts,sizeof(GrVertex2d)*nuse);
			delete[] pts;
			pts = pts2;
			nlen = nuse;
		}
	}
}

void GrVertexList2d::CopyFrom(const GrVertexList2d *list)
{
	if( pts && nlen>=list->nlen )
	{
		memcpy(pts,list->pts,sizeof(GrVertex2d)*list->nuse);
		nuse = list->nuse;
		isCompress = list->isCompress;
	}
	else
	{
		GrVertex2d *pts2 = new GrVertex2d[list->nlen];
		if( pts2 )
		{
			memcpy(pts2,list->pts,sizeof(GrVertex2d)*list->nuse);
			if( pts )delete[] pts;
			pts = pts2;
			nlen = list->nlen;
			nuse = list->nuse;
			isCompress = list->isCompress;
		}		
	}
}


void GrVertexList2d::CopyFrom(const GrVertexList *list,float xoff,float yoff)
{
	int i;
	if( pts && nlen>=list->nlen )
	{
		for( i=0; i<list->nuse; i++)
		{
			pts[i].x = list->pts[i].x-xoff;
			pts[i].y = list->pts[i].y-yoff;
			pts[i].code = list->pts[i].code;
		}
		nuse = list->nuse;
		isCompress = list->isCompress;
	}
	else
	{
		GrVertex2d *pts2 = new GrVertex2d[list->nlen];
		if( pts2 )
		{
			for( i=0; i<list->nuse; i++)
			{
				pts2[i].x = list->pts[i].x-xoff;
				pts2[i].y = list->pts[i].y-yoff;
				pts2[i].code = list->pts[i].code;
			}
			if( pts )delete[] pts;
			pts = pts2;
			nlen = list->nlen;
			nuse = list->nuse;
			isCompress = list->isCompress;
		}		
	}
}


GrPoint2d GrPointString2d::GetPoint(int i)const
{
	GrPoint2d pt;
	
	if( i>=0 && i<ptlist.nuse )
	{
		pt.pt = ptlist.pts[i];
		pt.angle = ptlist.pts[i].CodeToFloat();
	}
	else
	{
		pt.pt.x = pt.pt.y = 0;
		pt.angle = PI/4;
	}
	
	pt.kx = kx;
	pt.ky = ky;
	pt.cell = cell;
	pt.color = color;
	pt.bGrdSize = bGrdSize;
	pt.width = width;
	pt.bFlat = bFlat;
	pt.bUseSelfcolor = bUseSelfcolor;
	pt.bUseBackColor = bUseBackColor;
	
	return pt;
}

void GrText2d::SetText(LPCTSTR str)
{
	if( str==NULL )return;
	if( text )delete[] text;
	text = NULL;
	
	int nLen =  _tcslen(str);
	if( nLen<=0 )return;	
	text = new TCHAR[nLen+1];
	if( text )_tcscpy(text,str);
}

void GrText2d::SetSettings(const TextSettings *textSettings)
{
   if(textSettings)
	   memcpy(&settings,textSettings,sizeof(TextSettings));
	
}

BOOL GrText2d::CompareFont(const GrText2d *t)const
{
	if( _tcscmp(settings.tcFaceName,t->settings.tcFaceName)==0 )
		return TRUE;
	return FALSE;
}

void GrText2d::Release()
{
	if( text )delete[] text;
}


GrBuffer2d::GrBuffer2d()
{
	m_pCurGr  = NULL;
	m_pHeadGr = NULL;
	m_ptXoff = 0;
	m_ptYoff = 0;
}

GrBuffer2d::~GrBuffer2d()
{
	DeleteAll();
}


void GrBuffer2d::DeleteAll()
{
	Graph2d *pGr = m_pHeadGr, *t;
	while( pGr )
	{
		if( IsGrLineString2d(pGr) )
		{
			if( ((GrLineString2d*)pGr)->ptlist.pts )
				delete[] ((GrLineString2d*)pGr)->ptlist.pts;
		}
		else if( IsGrPointString2d(pGr) )
		{
			if( ((GrPointString2d*)pGr)->ptlist.pts )
				delete[] ((GrPointString2d*)pGr)->ptlist.pts;
		}
		else if( IsGrPointString2dEx(pGr) )
		{
			if( ((GrPointString2dEx*)pGr)->ptlist.pts )
				delete[] ((GrPointString2dEx*)pGr)->ptlist.pts;

			if( ((GrPointString2dEx*)pGr)->attrList.pts )
				delete[] ((GrPointString2dEx*)pGr)->attrList.pts;
		}
		else if( IsGrPolygon2d(pGr) )
		{
			if( ((GrPolygon2d*)pGr)->ptlist.pts )
				delete[] ((GrPolygon2d*)pGr)->ptlist.pts;
			if( ((GrPolygon2d*)pGr)->pts )
				delete[] ((GrPolygon2d*)pGr)->pts;
		}
		else if( IsGrText2d(pGr) )
		{
			((GrText2d*)pGr)->Release();
		}

		t = pGr;
		pGr = pGr->next;		
		DeleteGraph2d(t);
	}

	m_pHeadGr = NULL;
	m_pCurGr = NULL;
}


void GrBuffer2d::DeleteGraph(const Graph2d *pGr0)
{
	Graph2d *pGr = m_pHeadGr, *t;
	t = pGr;
	while( pGr )
	{
		if( pGr==pGr0 )
		{
			if( pGr==m_pHeadGr )
			{
				m_pHeadGr = m_pHeadGr->next;
			}
			else
			{
				t->next = pGr->next;
			}

			if( pGr==m_pCurGr )
			{
				m_pCurGr = NULL;
			}

			if( IsGrLineString2d(pGr) )
			{
				if( ((GrLineString2d*)pGr)->ptlist.pts )
					delete[] ((GrLineString2d*)pGr)->ptlist.pts;
			}
			else if( IsGrPointString2d(pGr) )
			{
				if( ((GrPointString2d*)pGr)->ptlist.pts )
					delete[] ((GrPointString2d*)pGr)->ptlist.pts;
			}
			else if( IsGrPointString2dEx(pGr) )
			{
				if( ((GrPointString2dEx*)pGr)->ptlist.pts )
					delete[] ((GrPointString2dEx*)pGr)->ptlist.pts;
				
				if( ((GrPointString2dEx*)pGr)->attrList.pts )
					delete[] ((GrPointString2dEx*)pGr)->attrList.pts;
			}
			else if( IsGrPolygon2d(pGr) )
			{
				if( ((GrPolygon2d*)pGr)->ptlist.pts )
					delete[] ((GrPolygon2d*)pGr)->ptlist.pts;
				if( ((GrPolygon2d*)pGr)->pts )
					delete[] ((GrPolygon2d*)pGr)->pts;
			}
			else if( IsGrText2d(pGr) )
			{
				((GrText2d*)pGr)->Release();
			}

			DeleteGraph2d(pGr);
			break;
		}

		t = pGr;
		pGr = pGr->next;		
	}
}

void GrBuffer2d::InsertGraph(const Graph2d *pGr,  float xoff ,float yoff,BOOL bRecalcEvlp)
{
	if( IsGrPoint2d(pGr) )
	{
		GrPoint2d *pNew = new GrPoint2d();
		if( pNew )
		{
			GrPoint2d *pgr = (GrPoint2d*)pGr;

			memcpy(pNew,pgr,sizeof(GrPoint2d));
			pNew->pt.x = pgr->pt.x+(xoff-m_ptXoff);
			pNew->pt.y = pgr->pt.y+(yoff-m_ptYoff);
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPointString2d(pGr) )
	{
		GrPointString2d *pNew = new GrPointString2d();
		if( pNew )
		{
			GrPointString2d *pgr = (GrPointString2d*)pGr;

			pNew->color = pgr->color;
			pNew->xl = pgr->xl+xoff-m_ptXoff;
			pNew->xh = pgr->xh+xoff-m_ptXoff;
			pNew->yl = pgr->yl+yoff-m_ptYoff;
			pNew->yh = pgr->yh+yoff-m_ptYoff;
			pNew->width = pgr->width;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->cell = pgr->cell;
			pNew->bFlat = pgr->bFlat;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->ptlist.CopyFrom(&(pgr->ptlist));
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPointString2dEx(pGr) )
	{
		GrPointString2dEx *pNew = new GrPointString2dEx();
		if( pNew )
		{
			GrPointString2dEx *pgr = (GrPointString2dEx*)pGr;

			pNew->color = pgr->color;
			pNew->xl = pgr->xl+xoff-m_ptXoff;
			pNew->xh = pgr->xh+xoff-m_ptXoff;
			pNew->yl = pgr->yl+yoff-m_ptYoff;
			pNew->yh = pgr->yh+yoff-m_ptYoff;
			pNew->width = pgr->width;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->cell = pgr->cell;
			pNew->bFlat = pgr->bFlat;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->ptlist.CopyFrom(&(pgr->ptlist));
			pNew->attrList.CopyFrom(&(pgr->attrList));
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrLineString2d(pGr) )
	{
		GrLineString2d *pNew = new GrLineString2d();
		if( pNew )
		{
			GrLineString2d *pgr = (GrLineString2d*)pGr;

			pNew->color = pgr->color;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->xl = pgr->xl+xoff-m_ptXoff;
			pNew->xh = pgr->xh+xoff-m_ptXoff;
			pNew->yl = pgr->yl+yoff-m_ptYoff;
			pNew->yh = pgr->yh+yoff-m_ptYoff;
			pNew->width = pgr->width;
			pNew->bGrdWid = pgr->bGrdWid;
			pNew->style = pgr->style;
			pNew->fDrawScale = pgr->fDrawScale;
			pNew->ptlist.CopyFrom(&(pgr->ptlist));
			for (int i =0;i<pNew->ptlist.nuse;i++)
			{
				pNew->ptlist.pts[i].x+=(xoff-m_ptXoff);
				pNew->ptlist.pts[i].y+=(yoff-m_ptYoff);
			}
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPolygon2d(pGr) )
	{
		GrPolygon2d *pNew = new GrPolygon2d();
		if( pNew )
		{
			GrPolygon2d *pgr = (GrPolygon2d*)pGr;

			pNew->color = pgr->color;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->index = pgr->index;
			pNew->xl = pgr->xl+xoff-m_ptXoff;
			pNew->xh = pgr->xh+xoff-m_ptXoff;
			pNew->yl = pgr->yl+yoff-m_ptYoff;
			pNew->yh = pgr->yh+yoff-m_ptYoff;
			pNew->filltype = pgr->filltype;
			pNew->ptlist.CopyFrom(&(pgr->ptlist));
			for (int i =0;i<pNew->ptlist.nuse;i++)
			{
				pNew->ptlist.pts[i].x+=(xoff-m_ptXoff);
				pNew->ptlist.pts[i].y+=(yoff-m_ptYoff);
			}
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrText2d(pGr) )
	{
		GrText2d *pNew = new GrText2d();
		if( pNew )
		{
			GrText2d *pgr = (GrText2d*)pGr;

			pNew->color = pgr->color;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->SetText(pgr->text);
			pNew->SetSettings(&pgr->settings);
			pNew->pt.x = pgr->pt.x+xoff-m_ptXoff;
			pNew->pt.y = pgr->pt.y+yoff-m_ptYoff;
			pNew->bGrdSize = pgr->bGrdSize;	
			pNew->bRotateWithView = pgr->bRotateWithView;
			if ( 1 || !bRecalcEvlp)
			{
				pNew->x[0] = pgr->x[0];
				pNew->x[1] = pgr->x[1];
				pNew->x[2] = pgr->x[2];
				pNew->x[3] = pgr->x[3];
				pNew->y[0] = pgr->y[0];
				pNew->y[1] = pgr->y[1];
				pNew->y[2] = pgr->y[2];
				pNew->y[3] = pgr->y[3];		
			}
			else
			{
				GrText2d *p = (GrText2d *)pNew;

				double ex[4], ey[4];
				g_Fontlib.SetSettings(&p->settings);
				g_Fontlib.GetEnvlope(p->text,ex,ey);

				p->x[0] = ex[0];
				p->y[0] = ey[0];
				p->x[1] = ex[1];
				p->y[1] = ey[1];
				p->x[2] = ex[2];
				p->y[2] = ey[2];
				p->x[3] = ex[3];
				p->y[3] = ey[3];	
			}			
			AddGraph(pNew);
		}
	}
}

void GrBuffer2d::AddBuffer(const GrBuffer2d *pBuf)
{
	if( !pBuf )return;

	float xoff, yoff;
	pBuf->GetOrigin(xoff,yoff);

	if (!m_pHeadGr)
	{
		m_ptXoff = xoff;
		m_ptYoff = yoff;
	}
	const Graph2d *pGr = pBuf->HeadGraph();
	
	while( pGr )
	{
		InsertGraph(pGr,xoff,yoff,TRUE);
		
		pGr = pGr->next;
	}
}

BOOL GrBuffer2d::IsOnlyType(int type)
{
	Graph2d *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( p->type!=type )
		{
			return FALSE;
		}
		p = p->next;
	}
	return TRUE;
}


void GrBuffer2d::InsertGraph(const Graph *pGr, BOOL bRecalcEvlp)
{
	if( IsGrPoint(pGr) )
	{
		GrPoint2d *pNew = new GrPoint2d();
		if( pNew )
		{
			GrPoint *pgr = (GrPoint*)pGr;
			pNew->pt.x = pgr->pt.x-m_ptXoff;
			pNew->pt.y = pgr->pt.y-m_ptYoff;
			pNew->color = pgr->color;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->angle = pgr->angle;
			pNew->cell = pgr->cell;
			pNew->width = pgr->width;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->bFlat = pgr->bFlat;
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPointString(pGr) )
	{
		GrPointString2d *pNew = new GrPointString2d();
		if( pNew )
		{
			GrPointString *pgr = (GrPointString*)pGr;

			pNew->color = pgr->color;
			pNew->xl = pgr->evlp.m_xl-m_ptXoff;
			pNew->xh = pgr->evlp.m_xh-m_ptXoff;
			pNew->yl = pgr->evlp.m_yl-m_ptYoff;
			pNew->yh = pgr->evlp.m_yh-m_ptYoff;
			pNew->width = pgr->width;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->cell = pgr->cell;
			pNew->bFlat = pgr->bFlat;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),m_ptXoff,m_ptYoff);
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPointStringEx(pGr) )
	{
		GrPointString2dEx *pNew = new GrPointString2dEx();
		if( pNew )
		{
			GrPointStringEx *pgr = (GrPointStringEx*)pGr;

			pNew->color = pgr->color;
			pNew->xl = pgr->evlp.m_xl-m_ptXoff;
			pNew->xh = pgr->evlp.m_xh-m_ptXoff;
			pNew->yl = pgr->evlp.m_yl-m_ptYoff;
			pNew->yh = pgr->evlp.m_yh-m_ptYoff;
			pNew->width = pgr->width;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->cell = pgr->cell;
			pNew->bFlat = pgr->bFlat;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),m_ptXoff,m_ptYoff);
			pNew->attrList.CopyFrom(&(pgr->attrList));
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrLineString(pGr) )
	{
		GrLineString2d *pNew = new GrLineString2d();
		if( pNew )
		{
			GrLineString *pgr = (GrLineString*)pGr;

			pNew->color = pgr->color;
			pNew->xl = pgr->evlp.m_xl-m_ptXoff;
			pNew->xh = pgr->evlp.m_xh-m_ptXoff;
			pNew->yl = pgr->evlp.m_yl-m_ptYoff;
			pNew->yh = pgr->evlp.m_yh-m_ptYoff;
			pNew->width = pgr->width;
			pNew->bGrdWid = pgr->bGrdWid;
			pNew->style = pgr->style;
			pNew->fDrawScale = pgr->fDrawScale;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),m_ptXoff,m_ptYoff);		
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPolygon(pGr) )
	{
		GrPolygon2d *pNew = new GrPolygon2d();
		if( pNew )
		{
			GrPolygon *pgr = (GrPolygon*)pGr;
			
			pNew->color = pgr->color;
			pNew->index = pgr->index;
			pNew->xl = pgr->evlp.m_xl-m_ptXoff;
			pNew->xh = pgr->evlp.m_xh-m_ptXoff;
			pNew->yl = pgr->evlp.m_yl-m_ptYoff;
			pNew->yh = pgr->evlp.m_yh-m_ptYoff;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->filltype = pgr->filltype;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),m_ptXoff,m_ptYoff);
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrText(pGr) )
	{
		GrText2d *pNew = new GrText2d();
		if( pNew )
		{
			GrText *pgr = (GrText*)pGr;
			
			pNew->color = pgr->color;
			pNew->SetText(pgr->text);
			pNew->SetSettings(&(pgr->settings));
			pNew->pt.x = pgr->pt.x-m_ptXoff;
			pNew->pt.y = pgr->pt.y-m_ptYoff;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->bRotateWithView = pgr->bRotateWithView;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			if ( 1 || !bRecalcEvlp)
			{
				pNew->x[0] = pgr->x[0];
				pNew->x[1] = pgr->x[1];
				pNew->x[2] = pgr->x[2];
				pNew->x[3] = pgr->x[3];
				pNew->y[0] = pgr->y[0];
				pNew->y[1] = pgr->y[1];
				pNew->y[2] = pgr->y[2];
				pNew->y[3] = pgr->y[3];
			}
			else
			{	
				GrText2d *p = (GrText2d *)pNew;

				double ex[4], ey[4];
				g_Fontlib.SetSettings(&p->settings);
				g_Fontlib.GetEnvlope(p->text,ex,ey);

				p->x[0] = ex[0];
				p->y[0] = ey[0];
				p->x[1] = ex[1];
				p->y[1] = ey[1];
				p->x[2] = ex[2];
				p->y[2] = ey[2];
				p->x[3] = ex[3];
				p->y[3] = ey[3];		
			}
			AddGraph(pNew);
		}
	}
}

BOOL GrBuffer2d::ReadFrom(CString& strXML)
{

	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_GRBUFFER2D) )
		xmlFile.IntoElem();

	while( xmlFile.FindElem(XMLTAG_GRAPH2D) )
	{
		xmlFile.IntoElem();

		if( xmlFile.FindElem(XMLTAG_TYPE) )
		{
			data = xmlFile.GetData();
			int type = _ttoi(data);
			switch(type)
			{
			case GRAPH_TYPE_POINT2D:
				{
					DWORD dColor;
					float kx, ky, fAngle;
					int nGrdWid,nCell,bUseSelfcolor;
					GR_PT_2D grpt;
					
					if( xmlFile.FindElem(XMLTAG_DATA) )
					{
						xmlFile.IntoElem();
						
						if( xmlFile.FindElem(XMLTAG_COLOR) )
						{
							data = xmlFile.GetData();
							dColor = _ttoi(data);
						}

						if( xmlFile.FindElem(XMLTAG_USESELFCOLOR) )
						{
							data = xmlFile.GetData();
							bUseSelfcolor = _ttoi(data);
						}

						if( xmlFile.FindElem(XMLTAG_POINT) )
						{
							data = xmlFile.GetData();
							char strPoint[MAX_FNAME_LEN];
							strcpy(strPoint,ConvertTCharToChar(data));
							sscanf(strPoint,"%f %f",&grpt.x,&grpt.y);							
							
						}	
						
						if( xmlFile.FindElem(XMLTAG_KX) )
						{
							data = xmlFile.GetData();
							kx = _ttof(data);
						}

						if( xmlFile.FindElem(XMLTAG_KY) )
						{
							data = xmlFile.GetData();
							ky = _ttof(data);
						}
// 
// 						if( xmlFile.FindElem(XMLTAG_SIZE) )
// 						{
// 							data = xmlFile.GetData();
// 							fSize = atof(data);
// 						}

						if( xmlFile.FindElem(XMLTAG_ANGLE) )
						{
							data = xmlFile.GetData();
							fAngle = _ttof(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_CELLDEFGRDWID) )
						{
							data = xmlFile.GetData();
							nGrdWid = _ttof(data);
						}

						if( xmlFile.FindElem(XMLTAG_POINTCELL) )
						{
							data = xmlFile.GetData();
							nCell = _ttoi(data);
						}

						BOOL bFlat = FALSE;
						if( xmlFile.FindElem(XMLTAG_ISFLAT) )
						{
							data = xmlFile.GetData();
							bFlat = _ttoi(data);
						}

						PT_2D pt(grpt.x,grpt.y);
						Point(dColor,&pt,kx,ky,fAngle,nGrdWid,nCell,0,bFlat,bUseSelfcolor);

						xmlFile.OutOfElem();									
						
					}
					break;
				}
			case GRAPH_TYPE_POINTSTRING2D:
				{
					DWORD dColor;
					float kx, ky, fAngle;
					int nGrdWid,nCell,bUseSelfcolor;
					GR_PT_2D grpt;
					
					if( xmlFile.FindElem(XMLTAG_DATA) )
					{
						xmlFile.IntoElem();
						
						if( xmlFile.FindElem(XMLTAG_COLOR) )
						{
							data = xmlFile.GetData();
							dColor = _ttol(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_USESELFCOLOR) )
						{
							data = xmlFile.GetData();
							bUseSelfcolor = _ttoi(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_KX) )
						{
							data = xmlFile.GetData();
							kx = _ttof(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_KY) )
						{
							data = xmlFile.GetData();
							ky = _ttof(data);
						}						
						
						if( xmlFile.FindElem(XMLTAG_CELLDEFGRDWID) )
						{
							data = xmlFile.GetData();
							nGrdWid = _ttoi(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_POINTCELL) )
						{
							data = xmlFile.GetData();
							nCell = _ttoi(data);
						}
						
						BOOL bFlat = FALSE;
						if( xmlFile.FindElem(XMLTAG_ISFLAT) )
						{
							data = xmlFile.GetData();
							bFlat = _ttoi(data);
						}	
						
						BeginPointString(dColor,kx,ky,nGrdWid,nCell,0,bFlat,bUseSelfcolor);

						if( xmlFile.FindElem(XMLTAG_CELLDEFGR) )
						{
							xmlFile.IntoElem();
							
							if( xmlFile.FindElem(XMLTAG_CELLDEFGRVS) )
							{
								data = xmlFile.GetData();

								CStringA data_a = ConvertTCharToChar(data);
								
								PT_2D pt;
								GrVertex2d grpt2d;
								double	value;
								int code;
								char	*pBuf = data_a.GetBuffer(data.GetLength()), *pStart, *pStop, *pMax;
								
								pStart = pBuf; pStop = pStart; pMax = pBuf+data.GetLength();
								while( pStop<pMax )
								{
									value = strtod(pStart,&pStop);
									if( pStop>=pMax || pStop==pStart )break;
									pt.x = value;
									pStart = pStop;
									
									value = strtod(pStart,&pStop);
									if( pStop>=pMax || pStop==pStart )break;
									pt.y = value;
									pStart = pStop;
									
									code = strtol(pStart,&pStop,10);
									if( pStop>=pMax || pStop==pStart )break;

									pStart = pStop;

									grpt2d.code = code;

									PointString(&pt,grpt2d.CodeToFloat());
								}
								
								data.ReleaseBuffer();
							}
							xmlFile.OutOfElem();
							
						}
						
						xmlFile.OutOfElem();
					}
				}
			case GRAPH_TYPE_LINESTRING2D:
				{
					DWORD dColor;
					float fWidth;
					int nGrdWid,nStyle,bUseSelfcolor;

					if( xmlFile.FindElem(XMLTAG_DATA) )
					{
						xmlFile.IntoElem();

						if( xmlFile.FindElem(XMLTAG_COLOR) )
						{
							data = xmlFile.GetData();
							dColor = _ttol(data);
						}

						if( xmlFile.FindElem(XMLTAG_USESELFCOLOR) )
						{
							data = xmlFile.GetData();
							bUseSelfcolor = _ttoi(data);
						}

						if( xmlFile.FindElem(XMLTAG_CELLDEFWIDTH) )
						{
							data = xmlFile.GetData();
							fWidth = _ttof(data);
						}

						if( xmlFile.FindElem(XMLTAG_CELLDEFGRDWID) )
						{
							data = xmlFile.GetData();
							nGrdWid = _ttoi(data);
						}

						if( xmlFile.FindElem(XMLTAG_CELLDEFSTYLE) )
						{
							data = xmlFile.GetData();
							nStyle = _ttoi(data);
						}
						
						BeginLineString(dColor,fWidth,nGrdWid,nStyle,1,bUseSelfcolor);						
						
						if( xmlFile.FindElem(XMLTAG_CELLDEFGR) )
						{
							xmlFile.IntoElem();
							
							if( xmlFile.FindElem(XMLTAG_CELLDEFGRVS) )
							{
								data = xmlFile.GetData();

								CStringA data_a = ConvertTCharToChar(data);
								
								PT_2D pt;
								double	value;
								int code;
								char	*pBuf = data_a.GetBuffer(data.GetLength()), *pStart, *pStop, *pMax;
								
								pStart = pBuf; pStop = pStart; pMax = pBuf+data.GetLength();
								while( pStop<pMax )
								{
									
									value = strtod(pStart,&pStop);
									if( pStop>=pMax || pStop==pStart )break;
									pt.x = value;
									pStart = pStop;
									
									value = strtod(pStart,&pStop);
									if( pStop>=pMax || pStop==pStart )break;
									pt.y = value;
									pStart = pStop;
									
									code = strtol(pStart,&pStop,10);
									if( pStop>=pMax || pStop==pStart )break;
									//pt.code = code;
									pStart = pStop;

									if(code == 0)
									{
										MoveTo(&pt);
									}
									else
										LineTo(&pt);
										
								}
								
								data.ReleaseBuffer();
							}
							xmlFile.OutOfElem();
							
						}
						xmlFile.OutOfElem();

						End();					
						
					}
					break;
				}
			case GRAPH_TYPE_POLYGON2D:
				{
					DWORD dColor;
					int nFillType,nIndex,bUseSelfcolor;
					
					if( xmlFile.FindElem(XMLTAG_DATA) )
					{
						xmlFile.IntoElem();
						
						if( xmlFile.FindElem(XMLTAG_COLOR) )
						{
							data = xmlFile.GetData();
							dColor = _ttol(data);
						}

						if( xmlFile.FindElem(XMLTAG_USESELFCOLOR) )
						{
							data = xmlFile.GetData();
							bUseSelfcolor = _ttoi(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_POLYGONFILLTYPE) )
						{
							data = xmlFile.GetData();
							nFillType = _ttoi(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_POLYGONINDEX) )
						{
							data = xmlFile.GetData();
							nIndex = _ttoi(data);
						}
						
						BeginPolygon(dColor,nFillType,nIndex,NULL,0,bUseSelfcolor);
						
						
						if( xmlFile.FindElem(XMLTAG_CELLDEFGR) )
						{
							xmlFile.IntoElem();
							
							if( xmlFile.FindElem(XMLTAG_CELLDEFGRVS) )
							{
								data = xmlFile.GetData();

								CStringA data_a = ConvertTCharToChar(data);
								
								PT_2D pt;
								double	value;
								char	*pBuf = data_a.GetBuffer(data.GetLength()), *pStart, *pStop, *pMax;
								
								pStart = pBuf; pStop = pStart; pMax = pBuf+data.GetLength();
								while( pStop<pMax )
								{
									
									value = strtod(pStart,&pStop);
									if( pStop>=pMax || pStop==pStart )break;
									pt.x = value;
									pStart = pStop;
									
									value = strtod(pStart,&pStop);
									if( pStop>=pMax || pStop==pStart )break;
									pt.y = value;
									pStart = pStop;
									
									value = strtod(pStart,&pStop);
									if( pStop>=pMax || pStop==pStart )break;
									int code = (int)value;
									pStart = pStop;
									
									if(code == 0)
									{
										MoveTo(&pt);
									}
									else
										LineTo(&pt);
									
								}
								
								data.ReleaseBuffer();
							}
							xmlFile.OutOfElem();
							
						}
						xmlFile.OutOfElem();
						
						End();					
						
					}
					break;
				}
			case GRAPH_TYPE_TEXT2D:
				{
					DWORD dColor;
					GR_PT_2D pt;
					TCHAR text[_MAX_FNAME];
					DWORD bGrdSize;
					int bUseSelfcolor;
                    TextSettings settings;					
					if( xmlFile.FindElem(XMLTAG_DATA) )
					{
						xmlFile.IntoElem();
						
						if( xmlFile.FindElem(XMLTAG_COLOR) )
						{
							data = xmlFile.GetData();
							dColor = _ttol(data);
						}
						if( xmlFile.FindElem(XMLTAG_USESELFCOLOR) )
						{
							data = xmlFile.GetData();
							bUseSelfcolor = _ttoi(data);
						}

						if( xmlFile.FindElem(XMLTAG_POINT) )
						{
							data = xmlFile.GetData();
							TCHAR strPoint[MAX_FNAME_LEN];
							_tcscpy(strPoint,data);
							_stscanf(strPoint,_T("%f %f"),pt.x,pt.y);
						}

						if( xmlFile.FindElem(XMLTAG_TEXT) )
						{
							data = xmlFile.GetData();
							_tcscpy(text,data);
						}
						
						if( xmlFile.FindElem(XMLTAG_TEXTFONTNAME) )
						{
							data = xmlFile.GetData();
							_tcscpy(settings.tcFaceName,data);
						}
						if( xmlFile.FindElem(XMLTAG_CELLDEFGRDWID) )
						{
							data = xmlFile.GetData();
							bGrdSize = _ttol(data);
						}
						
						if( xmlFile.FindElem(XMLTAG_TEXTHEIGHT) )
						{
							data = xmlFile.GetData();
							settings.fHeight = _tcstod(data,NULL);							
						}

						if( xmlFile.FindElem(XMLTAG_TEXTWIDTHSCALE) )
						{
							data = xmlFile.GetData();
							settings.fWidScale = _tcstod(data,NULL);
						}

						if( xmlFile.FindElem(XMLTAG_TEXTINTVSCALE) )
						{
							data = xmlFile.GetData();
							settings.fCharIntervalScale = _tcstod(data,NULL);
						}

						if( xmlFile.FindElem(XMLTAG_TEXTLNSPCESCALE) )
						{
							data = xmlFile.GetData();
							settings.fLineSpacingScale = _tcstod(data,NULL);
						}

						if( xmlFile.FindElem(XMLTAG_TEXTTEXTANGLE) )
						{
							data = xmlFile.GetData();
							settings.fTextAngle = _tcstod(data,NULL);
						}

						if( xmlFile.FindElem(XMLTAG_TEXTCHARANGLE) )
						{
							data = xmlFile.GetData();
							settings.fCharAngle = _tcstod(data,NULL);
						}

						if( xmlFile.FindElem(XMLTAG_TEXTALIGN) )
						{
							data = xmlFile.GetData();
							settings.nAlignment = _ttoi(data);
						}
						if( xmlFile.FindElem(XMLTAG_TEXTINCLINETYPE) )
						{
							data = xmlFile.GetData();
							settings.nInclineType = _ttoi(data);
						}
						if( xmlFile.FindElem(XMLTAG_TEXTINCLINEANGLE) )
						{
							data = xmlFile.GetData();
							settings.fInclineAngle = _tcstod(data,NULL);
						}

						PT_2D p;
						p.x = pt.x;
						p.y = pt.y;
						Text(dColor,&p,text,&settings,bGrdSize,bUseSelfcolor);

//						MoveTo(&p);

						xmlFile.OutOfElem();				
						
					}
					break;
				}
			default:
				{
					xmlFile.OutOfElem();
					continue;   //下一个Graph2
				}
				
			}
		}
		xmlFile.OutOfElem();
								
								
	}
	xmlFile.OutOfElem();

	return TRUE;
	

}

BOOL GrBuffer2d::WriteTo(CString& strXML)
{

	CMarkup xmlFile;
//	xmlFile.SetDoc(strXML);
	CString data;

	if( xmlFile.AddElem(XMLTAG_GRBUFFER2D) )	
		xmlFile.IntoElem();	

	Graph2d *pGr = HeadGraph();
	int num = 0;
	while( pGr )
	{
		switch(pGr->type)
		{
		case GRAPH_TYPE_POINT2D:
			{
				GrPoint2d *pPoint = (GrPoint2d*)pGr;
				if(pPoint == NULL)
					continue;
				if( xmlFile.AddElem(XMLTAG_GRAPH2D) )	
				{
					xmlFile.IntoElem();
					
					data.Format(_T("%d"),pPoint->type);
					xmlFile.AddElem(XMLTAG_TYPE,data);
					
					if( xmlFile.AddElem(XMLTAG_DATA) )	
					{
						xmlFile.IntoElem();
						
						data.Format(_T("%d"),pPoint->color);
						xmlFile.AddElem(XMLTAG_COLOR,data);

						data.Format(_T("%d"),pPoint->bUseSelfcolor);
						xmlFile.AddElem(XMLTAG_USESELFCOLOR,data);
						
						data.Format(_T("%f %f"),(double)pPoint->pt.x+m_ptXoff,(double)pPoint->pt.y+m_ptYoff);
						xmlFile.AddElem(XMLTAG_POINT,data);
						
						data.Format(_T("%f"),pPoint->kx);
						xmlFile.AddElem(XMLTAG_KX,data);

						data.Format(_T("%f"),pPoint->ky);
						xmlFile.AddElem(XMLTAG_KY,data);
						
						data.Format(_T("%f"),pPoint->angle);
						xmlFile.AddElem(XMLTAG_ANGLE,data);
						
						data.Format(_T("%d"),pPoint->bGrdSize);
						xmlFile.AddElem(XMLTAG_CELLDEFGRDWID,data);

						data.Format(_T("%d"),pPoint->cell);
						xmlFile.AddElem(XMLTAG_POINTCELL,data);

						data.Format(_T("%d"),pPoint->bFlat);
						xmlFile.AddElem(XMLTAG_ISFLAT,data);
						
						xmlFile.OutOfElem();
					}
					
					xmlFile.OutOfElem();
					
				}
				break;
			}
		case GRAPH_TYPE_POINTSTRING2D:
			{
				GrPointString2d *pPoint = (GrPointString2d*)pGr;
				if(pPoint == NULL)
					continue;
				if( xmlFile.AddElem(XMLTAG_GRAPH2D) )	
				{
					xmlFile.IntoElem();
					
					data.Format(_T("%d"),pPoint->type);
					xmlFile.AddElem(XMLTAG_TYPE,data);
					
					if( xmlFile.AddElem(XMLTAG_DATA) )	
					{
						xmlFile.IntoElem();
						
						data.Format(_T("%d"),pPoint->color);
						xmlFile.AddElem(XMLTAG_COLOR,data);
						
						data.Format(_T("%d"),pPoint->bUseSelfcolor);
						xmlFile.AddElem(XMLTAG_USESELFCOLOR,data);
						
						data.Format(_T("%f"),pPoint->kx);
						xmlFile.AddElem(XMLTAG_KX,data);
						
						data.Format(_T("%f"),pPoint->ky);
						xmlFile.AddElem(XMLTAG_KY,data);
						
						data.Format(_T("%d"),pPoint->bGrdSize);
						xmlFile.AddElem(XMLTAG_CELLDEFGRDWID,data);
						
						data.Format(_T("%d"),pPoint->cell);
						xmlFile.AddElem(XMLTAG_POINTCELL,data);
						
						data.Format(_T("%d"),pPoint->bFlat);
						xmlFile.AddElem(XMLTAG_ISFLAT,data);

						
						if( xmlFile.AddElem(XMLTAG_CELLDEFGR) )	
						{
							xmlFile.IntoElem();
							
							data = _T("");
							for( int i=0; i<pPoint->ptlist.nuse; i++)
							{
								CString strtmp;
								strtmp.Format(_T("%16.6f %16.6f %d\n"),
									(double)pPoint->ptlist.pts[i].x+m_ptXoff,
									(double)pPoint->ptlist.pts[i].y+m_ptYoff,
									pPoint->ptlist.pts[i].code);
								
								data += strtmp;
							}
							
							xmlFile.AddElem(XMLTAG_CELLDEFGRVS,data);
							
							xmlFile.OutOfElem();
						}
						
						xmlFile.OutOfElem();
					}
					
					xmlFile.OutOfElem();
					
				}
				break;
			}
		case GRAPH_TYPE_LINESTRING2D:
			{
				GrLineString2d *pLineString = (GrLineString2d*)pGr;
				if(pLineString == NULL)
					continue;
				if( xmlFile.AddElem(XMLTAG_GRAPH2D) )	
				{
					xmlFile.IntoElem();

					data.Format(_T("%d"),pLineString->type);
					xmlFile.AddElem(XMLTAG_TYPE,data);

					if( xmlFile.AddElem(XMLTAG_DATA) )	
					{
						xmlFile.IntoElem();

						data.Format(_T("%d"),pLineString->color);
						xmlFile.AddElem(XMLTAG_COLOR,data);

						data.Format(_T("%d"),pLineString->bUseSelfcolor);
						xmlFile.AddElem(XMLTAG_USESELFCOLOR,data);

						data.Format(_T("%d"),pLineString->width);
						xmlFile.AddElem(XMLTAG_CELLDEFWIDTH,data);

						data.Format(_T("%d"),pLineString->bGrdWid);
						xmlFile.AddElem(XMLTAG_CELLDEFGRDWID,data);

						data.Format(_T("%d"),pLineString->style);
						xmlFile.AddElem(XMLTAG_CELLDEFSTYLE,data);

						if( xmlFile.AddElem(XMLTAG_CELLDEFGR) )	
						{
							xmlFile.IntoElem();

							data = _T("");
							for( int i=0; i<pLineString->ptlist.nuse; i++)
							{
								CString strtmp;
								strtmp.Format(_T("%16.6f %16.6f %3d\n"),
									(double)pLineString->ptlist.pts[i].x+m_ptXoff,
									(double)pLineString->ptlist.pts[i].y+m_ptYoff,
									pLineString->ptlist.pts[i].code&1);
								
								data += strtmp;
							}

							xmlFile.AddElem(XMLTAG_CELLDEFGRVS,data);	
							

							xmlFile.OutOfElem();
						}

						xmlFile.OutOfElem();
					}
					
					xmlFile.OutOfElem();
					
				}				
				break;
			}
		case GRAPH_TYPE_POLYGON2D:
			{
				GrPolygon2d *pPolygon = (GrPolygon2d*)pGr;
				if(pPolygon == NULL)
					continue;
				if( xmlFile.AddElem(XMLTAG_GRAPH2D) )	
				{
					xmlFile.IntoElem();
					
					data.Format(_T("%d"),pPolygon->type);
					xmlFile.AddElem(XMLTAG_TYPE,data);
					
					if( xmlFile.AddElem(XMLTAG_DATA) )	
					{
						xmlFile.IntoElem();
						
						data.Format(_T("%d"),pPolygon->color);
						xmlFile.AddElem(XMLTAG_COLOR,data);

						data.Format(_T("%d"),pPolygon->bUseSelfcolor);
						xmlFile.AddElem(XMLTAG_USESELFCOLOR,data);
						
						data.Format(_T("%d"),pPolygon->filltype);
						xmlFile.AddElem(XMLTAG_POLYGONFILLTYPE,data);
						
						data.Format(_T("%d"),pPolygon->index);
						xmlFile.AddElem(XMLTAG_POLYGONINDEX,data);
						
						if( xmlFile.AddElem(XMLTAG_CELLDEFGR) )	
						{
							xmlFile.IntoElem();
							
							data = _T("");
							for( int i=0; i<pPolygon->ptlist.nuse; i++)
							{
								CString strtmp;
								strtmp.Format(_T("%16.6f %16.6f %3d\n"),
									(double)pPolygon->ptlist.pts[i].x+m_ptXoff,
									(double)pPolygon->ptlist.pts[i].y+m_ptYoff,
									pPolygon->ptlist.pts[i].code);
								
								data += strtmp;
							}
							
							xmlFile.AddElem(XMLTAG_CELLDEFGRVS,data);	
							
							
							xmlFile.OutOfElem();
						}
						
						xmlFile.OutOfElem();
					}
					
					xmlFile.OutOfElem();
					
				}
				break;
			}
		case GRAPH_TYPE_TEXT2D:
			{
				GrText2d *pText = (GrText2d*)pGr;
				if (pText == NULL)
					continue;
				if( xmlFile.AddElem(XMLTAG_GRAPH2D) )	
				{
					xmlFile.IntoElem();
					
					data.Format(_T("%d"),pText->type);
					xmlFile.AddElem(XMLTAG_TYPE,data);
					
					if( xmlFile.AddElem(XMLTAG_DATA) )	
					{
						xmlFile.IntoElem();
						
						data.Format(_T("%ld"),pText->color);
						xmlFile.AddElem(XMLTAG_COLOR,data);

						data.Format(_T("%d"),pText->bUseSelfcolor);
						xmlFile.AddElem(XMLTAG_USESELFCOLOR,data);
						
						data.Format(_T("%lf %lf"),(double)pText->pt.x+m_ptXoff,(double)pText->pt.y+m_ptYoff);
						xmlFile.AddElem(XMLTAG_POINT,data);
						
						xmlFile.AddElem(XMLTAG_TEXT,pText->text);

						xmlFile.AddElem(XMLTAG_TEXTFONTNAME,pText->settings.tcFaceName);				


						data.Format(_T("%d"),pText->bGrdSize);
						xmlFile.AddElem(XMLTAG_CELLDEFGRDWID,data);

						data.Format(_T("%f"),pText->settings.fHeight);
						xmlFile.AddElem(XMLTAG_TEXTHEIGHT,data);

						data.Format(_T("%lf"),pText->settings.fWidScale);
						xmlFile.AddElem(XMLTAG_TEXTWIDTHSCALE,data);

						data.Format(_T("%lf"),pText->settings.fCharIntervalScale);
						xmlFile.AddElem(XMLTAG_TEXTINTVSCALE,data);

						data.Format(_T("%lf"),pText->settings.fLineSpacingScale);
						xmlFile.AddElem(XMLTAG_TEXTLNSPCESCALE,data);
						data.Format(_T("%lf"),pText->settings.fTextAngle);
						xmlFile.AddElem(XMLTAG_TEXTTEXTANGLE,data);

						data.Format(_T("%lf"),pText->settings.fCharAngle);
						xmlFile.AddElem(XMLTAG_TEXTCHARANGLE,data);
						data.Format(_T("%d"),pText->settings.nAlignment);
						xmlFile.AddElem(XMLTAG_TEXTALIGN,data);
						data.Format(_T("%d"),pText->settings.nInclineType);
						xmlFile.AddElem(XMLTAG_TEXTINCLINETYPE,data);
											
						data.Format(_T("%lf"),pText->settings.fInclineAngle);
						xmlFile.AddElem(XMLTAG_TEXTINCLINEANGLE,data);
						xmlFile.OutOfElem();
					}
					
					xmlFile.OutOfElem();
					
				}
				break;
			}
		default:
			pGr = NULL;
			break;

		}
		pGr = pGr->next;
		
	}

	xmlFile.OutOfElem();

	strXML = xmlFile.GetDoc();
	return TRUE;
}

void GrBuffer2d::AddBuffer(const GrBuffer *pBuf)
{
	if( !pBuf )return;
	if (!m_pHeadGr)
	{
		Envelope e = pBuf->GetEnvelope();
		m_ptXoff = floor((e.m_xl+e.m_xh)*0.5/1000)*1000;
		m_ptYoff = floor((e.m_yl+e.m_yh)*0.5/1000)*1000;
	}
	const Graph *pGr = pBuf->HeadGraph();
	while( pGr )
	{
		InsertGraph(pGr);
		
		pGr = pGr->next;
	}
}

void GrBuffer2d::CopyFrom(const GrBuffer2d *pBuf)
{
	DeleteAll();
	AddBuffer(pBuf);
}


void GrBuffer2d::CutTo(GrBuffer2d *pBuf, BOOL bClearOld)
{
	if( bClearOld )
	{
		pBuf->DeleteAll();
		
		pBuf->m_pCurGr = m_pCurGr;
		pBuf->m_pHeadGr = m_pHeadGr;

	}
	else
	{
		if( pBuf->m_pHeadGr==NULL )
		{
			pBuf->m_pHeadGr = m_pHeadGr;
			pBuf->m_pCurGr = m_pCurGr;
		}
		else
		{
			pBuf->m_pCurGr->next = m_pHeadGr;
			pBuf->m_pCurGr = m_pCurGr;
		}
	}
	
	m_pCurGr = NULL;
	m_pHeadGr = NULL;
}


int GrBuffer2d::GetLinePts(PT_2D *pts)
{
	const Graph2d *pGr = HeadGraph();
	const GrVertexList2d *pList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		if( IsGrLineString2d(pGr) )
		{
			pList = &((GrLineString2d*)pGr)->ptlist;
		}
		else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
		{
			pList = &((GrPointString2d*)pGr)->ptlist;
		}
		else if( IsGrPolygon2d(pGr) )
		{
			pList = &((GrPolygon2d*)pGr)->ptlist;
		}

		if( pList )
		{
			if( pts )
			{
				for( int i=0; i<pList->nuse; i++)
				{
					pts[num+i].x = pList->pts[i].x+m_ptXoff;
					pts[num+i].y = pList->pts[i].y+m_ptYoff;
				}
			}

			num += pList->nuse;
		}
		
		pGr = pGr->next;
	}

	return num;
}



int GrBuffer2d::GetVertexPts(GrVertex2d *pts)
{
	const Graph2d *pGr = HeadGraph();
	const GrVertexList2d *pList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		if( IsGrPoint2d(pGr) )
		{
			if( pts )
			{
				GrPoint2d* p = (GrPoint2d*)pGr;
				pts[num].code = 0;				
				pts[num].x = p->pt.x + m_ptXoff;
				pts[num].y = p->pt.y + m_ptYoff;
			}
			num++;
		}
		else if( IsGrLineString2d(pGr) )
		{
			pList = &((GrLineString2d*)pGr)->ptlist;
		}		
		else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
		{
			pList = &((GrPointString2d*)pGr)->ptlist;
		}
		else if( IsGrPolygon2d(pGr) )
		{
			pList = &((GrPolygon2d*)pGr)->ptlist;
		}
		else if( IsGrText2d(pGr) )
		{
			if( pts )
			{
				GrText2d* p = (GrText2d*)pGr;
				pts[num].x = p->pt.x + m_ptXoff;
				pts[num].y = p->pt.y + m_ptYoff;
				pts[num].code = 0;		

				for (int i = 0; i < 4; i++)
				{
					pts[num].x = p->x[i] + p->pt.x + m_ptXoff;
					pts[num].y = p->y[i] + p->pt.y + m_ptYoff;
					pts[num].code = 0;
					num++;
				}
			}
			else
			{
				num += 5;
			}
		}
		
		if( pList )
		{
			if( pts )
			{
				GrVertex2d *pts2 = pts + num;
				for (int i = 0; i<pList->nuse; i++)
				{
					pts2[i].x = pList->pts[i].x + m_ptXoff;
					pts2[i].y = pList->pts[i].y + m_ptYoff;
				}

				num += pList->nuse;			
			}
			num += pList->nuse;
		}
		
		pGr = pGr->next;
	}
	
	return num;
}


void GrBuffer2d::SetAllPts(const PT_2D *pts)
{
	if( !pts )
		return;
	
	m_ptXoff = floor(pts[0].x/1000)*1000;
	m_ptYoff = floor(pts[0].y/1000)*1000;

	Graph2d *pGr = HeadGraph();
	GrVertexList2d *pList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		if( IsGrPoint2d(pGr) )
		{
			GrPoint2d* p = (GrPoint2d*)pGr;
			p->pt.x = pts[num].x-m_ptXoff;
			p->pt.y = pts[num].y-m_ptYoff;

			num++;
		}
		else if( IsGrLineString2d(pGr) )
		{
			pList = &((GrLineString2d*)pGr)->ptlist;
		}		
		else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
		{
			pList = &((GrPointString2d*)pGr)->ptlist;
		}
		else if( IsGrPolygon2d(pGr) )
		{
			pList = &((GrPolygon2d*)pGr)->ptlist;
		}
		else if( IsGrText2d(pGr) )
		{
			GrText2d* p = (GrText2d*)pGr;
			p->pt.x = pts[num].x-m_ptXoff;
			p->pt.y = pts[num].y-m_ptYoff;

			num++;

			for (int i = 0; i < 4; i++)
			{
				p->x[i] = pts[num].x - m_ptXoff - p->pt.x;
				p->y[i] = pts[num].y - m_ptYoff - p->pt.y;
				num++;
			}
		}
		
		if( pList )
		{
			const PT_2D *pts2 = pts + num;
			for( int i=0; i<pList->nuse; i++)
			{
				pList->pts[i].x = pts2[i].x-m_ptXoff;
				pList->pts[i].y = pts2[i].y-m_ptYoff;
			}				

			num += pList->nuse;
		}
		
		pGr = pGr->next;
	}	
}



int	GrBuffer2d::GetPtAttrs(GrVertex2d *pts, GrVertexAttr *attrs)
{
	const Graph2d *pGr = HeadGraph();
	const GrVertexList2d *pList;
	const GrVertexAttrList *pAttrList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		pAttrList = NULL;
		if( IsGrPointString2dEx(pGr) )
		{
			pList = &((GrPointString2dEx*)pGr)->ptlist;
			pAttrList = &((GrPointString2dEx*)pGr)->attrList;
		}
		
		if( pList )
		{
			if( pts )
			{
				memcpy(pts,pList->pts,sizeof(GrVertex2d)*pList->nuse);
				pts += pList->nuse;				
			}
			num += pList->nuse;
		}

		if( pAttrList )
		{
			if( attrs )
			{
				memcpy(attrs,pAttrList->pts,sizeof(GrVertexAttr)*pAttrList->nuse);
				attrs += pAttrList->nuse;				
			}
		}
		
		pGr = pGr->next;
	}
	
	return num;
}


void GrBuffer2d::SetPtAttrs(GrVertexAttr *attrs)
{
	if( !attrs )
		return;

	Graph2d *pGr = HeadGraph();
	GrVertexAttrList *pAttrList;
	int num = 0;
	while( pGr )
	{
		pAttrList = NULL;
		if( IsGrPointString2dEx(pGr) )
		{
			pAttrList = &((GrPointString2dEx*)pGr)->attrList;
		}
		
		if( pAttrList )
		{
			const GrVertexAttr *pts2 = attrs + num;
			for( int i=0; i<pAttrList->nuse; i++)
			{
				pAttrList->pts[i] = pts2[i];
			}				

			num += pAttrList->nuse;
		}
		
		pGr = pGr->next;
	}	
}


COLORREF GrBuffer2d::MatchColor(COLORREF clr)
{
	return clr;
}

void GrBuffer2d::AddGraph(Graph2d *pGr)
{
	if( m_pHeadGr==NULL )
	{
		m_pHeadGr = pGr;
		m_pCurGr = pGr;
	}
	else
	{
		m_pCurGr->next = pGr;
		m_pCurGr = pGr;
	}
}


void GrBuffer2d::BeginPointString(COLORREF clr, float kx,float ky, BOOL bGrdSize, int cell, float width, BOOL bFlat, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPointString2d *pGr = new GrPointString2d();
	if( !pGr )return;
	
	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->kx = kx;
	pGr->ky = ky;
	pGr->bGrdSize = bGrdSize;
	pGr->width = width;
	pGr->cell = cell; 
	pGr->bFlat = bFlat;
	
	AddGraph(pGr);
}

void GrBuffer2d::PointString(PT_2D *pt, float fAngle)
{
	GrVertex2d gpt;
	gpt.x = pt->x-m_ptXoff;
	gpt.y = pt->y-m_ptXoff;
	
	gpt.CodeFromFloat(fAngle);
	
	AddVertex(&gpt);
}



void GrBuffer2d::BeginPointStringEx(COLORREF clr, float kx,float ky, BOOL bGrdSize, int cell, float width, BOOL bFlat, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPointString2dEx *pGr = new GrPointString2dEx();
	if( !pGr )return;
	
	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->kx = kx;
	pGr->ky = ky;
	pGr->bGrdSize = bGrdSize;
	pGr->width = width;
	pGr->cell = cell; 
	pGr->bFlat = bFlat;
	
	AddGraph(pGr);
}

void GrBuffer2d::PointStringEx(PT_2D *pt, COLORREF color, BOOL isMark, float fAngle)
{
	GrVertex2d gpt;
	gpt.x = pt->x-m_ptXoff;
	gpt.y = pt->y-m_ptYoff;
	
	gpt.CodeFromFloat(fAngle);
	
	AddVertex(&gpt);

	if( IsGrPointString2dEx(m_pCurGr) )
	{
		GrPointString2dEx *p = (GrPointString2dEx*)m_pCurGr;
		if( p->ptlist.nuse>=0 && p->ptlist.nuse<p->ptlist.nlen )
		{
			p->attrList.pts[p->attrList.nuse].color = color;
			p->attrList.pts[p->attrList.nuse].isMarked = isMark;
			p->attrList.nuse++;
		}
	}
}

void GrBuffer2d::Point(COLORREF clr, PT_2D *pt, float kx, float ky, float angle, BOOL bGrdSize, int cell, float width, BOOL bFlat, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPoint2d *pGr = new GrPoint2d();
	if( !pGr )return;
	
	pGr->pt.x = pt->x - m_ptXoff;
	pGr->pt.y = pt->y - m_ptYoff;

	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->kx = kx;
	pGr->ky = ky;
	pGr->bGrdSize = bGrdSize;
	pGr->width = width;
	pGr->cell = cell;
	pGr->angle = angle;
	pGr->bFlat = bFlat;
	
	AddGraph(pGr);
}

void GrBuffer2d::Text(COLORREF clr, PT_2D *pt, LPCTSTR text, const TextSettings *settings,BOOL bGrdSize,BOOL bRotateWithView,BOOL bCalcEnvelope, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	if(_tcslen(text)<=0)return;
	GrText2d *pGr = new GrText2d();
	if( !pGr )return;
	
	pGr->pt.x = pt->x - m_ptXoff;
	pGr->pt.y = pt->y - m_ptYoff;
	pGr->SetText(text);
	pGr->SetSettings(settings);
	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->bGrdSize = bGrdSize;
	pGr->bRotateWithView = bRotateWithView;
	if (bCalcEnvelope)
	{
		GrText2d *p = (GrText2d *)pGr;

		double ex[4], ey[4];
		g_Fontlib.SetSettings(&p->settings);
		g_Fontlib.GetEnvlope(p->text,ex,ey);

		p->x[0] = ex[0];
		p->y[0] = ey[0];
		p->x[1] = ex[1];
		p->y[1] = ey[1];
		p->x[2] = ex[2];
		p->y[2] = ey[2];
		p->x[3] = ex[3];
		p->y[3] = ey[3];		
	}
	
	
	AddGraph(pGr);
}

void GrBuffer2d::BeginLineString(COLORREF clr, float width, BOOL bGrdWid, int style, float fDrawScale, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrLineString2d *pGr = new GrLineString2d();
	if( !pGr )return;
	
	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->width = width;
	pGr->bGrdWid = bGrdWid;
	pGr->style = style;
	pGr->fDrawScale = fDrawScale;

	AddGraph(pGr);
}


void GrBuffer2d::BeginPolygon(COLORREF clr, int type, int index, const PT_2D *pts, int nPt, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPolygon2d *pGr = new GrPolygon2d();
	if( !pGr )return;
	
	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->filltype = type;
	pGr->index = index;
	if( pts!=NULL && nPt>0 )
	{
		pGr->pts = new PT_2D[nPt];
		if( pGr->pts!=NULL )
		{
			memcpy(pGr->pts,pts,nPt*sizeof(PT_2D));
		}
	}

	AddGraph(pGr);
}

void GrBuffer2d::SetAllColor(COLORREF color)
{
	Graph2d *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if (!p->bUseSelfcolor)
		{
			p->color = (color&0xffffff);
		}		
		p = p->next;
	}
}

void GrBuffer2d::SetAllLineWidth(BOOL bGrdWid, float width)
{
	Graph2d *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrLineString2d(p) )
		{
			((GrLineString2d*)p)->bGrdWid = bGrdWid;
			((GrLineString2d*)p)->width = width;
		}
		p = p->next;
	}
}



void GrBuffer2d::SetAllLineWidthOrWidthScale(BOOL bGrdWid, float width)
{
	if( width>=0.0f )
		SetAllLineWidth(bGrdWid,width);
	else
		ZoomWidth(-width);
}

void GrBuffer2d::SetAllCell(int cell)
{
	Graph2d *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrPoint2d(p) )
		{
			((GrPoint2d*)p)->cell = cell;
		}
		else if( IsGrPointString2d(p) || IsGrPointString2dEx(p) )
		{
			((GrPointString2d*)p)->cell = cell;
		}
		p = p->next;
	}
}

void GrBuffer2d::SetAllLineStyle(int style)
{
	Graph2d *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrLineString2d(p) )
		{
			((GrLineString2d*)p)->style = style;
		}
		p = p->next;
	}
}

void GrBuffer2d::SetAllPolygon(int type, int index, PT_2D *pts)
{
	Graph2d *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrPolygon2d(p) )
		{
			GrPolygon2d *p2 = (GrPolygon2d*)p;

			if( p2->ptlist.nuse<=0 )continue;

			p2->type = type;
			p2->index = index;
			if( p2->pts==NULL )
			{
				p2->pts = new PT_2D[p2->ptlist.nuse];
				if( !p2->pts )continue;
			}

			memcpy(p2->pts,pts,sizeof(PT_2D)*p2->ptlist.nuse);
		}
		p = p->next;
	}
}

void GrBuffer2d::SetAllDrawFlag(int needdraw)
{
	Graph2d *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		p->needdraw = needdraw;
		p = p->next;
	}
}


void GrBuffer2d::AddVertex(GrVertex2d *pt)
{	
	if( IsGrLineString2d(m_pCurGr) )
	{
		GrLineString2d *p = (GrLineString2d*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex2d[MAX_GRAPH_PTNUM];
			if( p->ptlist.pts==NULL )return;
			p->ptlist.nlen = MAX_GRAPH_PTNUM;
		}

		if( p->ptlist.nuse>=p->ptlist.nlen )
		{
			COLORREF color = p->color;
			float width = p->width;
			GrVertex2d pt0 = p->ptlist.pts[p->ptlist.nuse-1];
			BOOL bGrdWid = p->bGrdWid;
			int style = p->style;

			End();
			BeginLineString(color,width,bGrdWid,style);

			pt0.code = ((pt0.code&0xfffffffe)|GRBUFFER_PTCODE_MOVETO);
			AddVertex(&pt0);
			AddVertex(pt);
			return;
		}

		p->ptlist.pts[p->ptlist.nuse] = *pt;
		p->ptlist.nuse++;
		
	}
	else if( IsGrPointString2d(m_pCurGr) )
	{
		GrPointString2d *p = (GrPointString2d*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex2d[MAX_GRAPH_PTNUM];
			if( p->ptlist.pts==NULL )return;
			p->ptlist.nlen = MAX_GRAPH_PTNUM;
		}
		
		if( p->ptlist.nuse>=p->ptlist.nlen )
		{
			COLORREF color = p->color;
			float width = p->width;
			BOOL bGrdSize = p->bGrdSize;
			int cell = p->cell;
			BOOL bFlat = p->bFlat;
			BOOL bUseSelfCol = p->bUseSelfcolor;
			BOOL bUseBackColor = p->bUseBackColor;
			float kx = p->kx;
			float ky = p->ky;
			
			End();
			BeginPointString(color,kx,ky,bGrdSize,cell,width,bFlat,bUseSelfCol,bUseBackColor);
			
			AddVertex(pt);
			return;
		}
		
		p->ptlist.pts[p->ptlist.nuse] = *pt;
		p->ptlist.nuse++;
		
	}
	else if( IsGrPointString2dEx(m_pCurGr) )
	{
		GrPointString2dEx *p = (GrPointString2dEx*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex2d[MAX_GRAPH_PTNUM];
			if( p->ptlist.pts==NULL )return;
			p->ptlist.nlen = MAX_GRAPH_PTNUM;

			p->attrList.pts = new GrVertexAttr[MAX_GRAPH_PTNUM];
			if( p->attrList.pts==NULL )return;
			p->attrList.nlen = MAX_GRAPH_PTNUM;
		}
		
		if( p->ptlist.nuse>=p->ptlist.nlen )
		{
			COLORREF color = p->color;
			float width = p->width;
			BOOL bGrdSize = p->bGrdSize;
			int cell = p->cell;
			BOOL bFlat = p->bFlat;
			BOOL bUseSelfCol = p->bUseSelfcolor;
			BOOL bUseBackColor = p->bUseBackColor;
			float kx = p->kx;
			float ky = p->ky;
			
			End();
			BeginPointStringEx(color,kx,ky,bGrdSize,cell,width,bFlat,bUseSelfCol,bUseBackColor);
			
			AddVertex(pt);
			return;
		}
		
		p->ptlist.pts[p->ptlist.nuse] = *pt;
		p->ptlist.nuse++;
		
	}
	else if( IsGrPolygon2d(m_pCurGr) )
	{
		GrPolygon2d *p = (GrPolygon2d*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex2d[MAX_GRAPH_PTNUM];
			if( p->ptlist.pts==NULL )return;
			p->ptlist.nlen = MAX_GRAPH_PTNUM;
		}
		
		if( p->ptlist.nuse>=p->ptlist.nlen )
		{
			GrVertex2d *pNewPts = new GrVertex2d[p->ptlist.nlen+MAX_GRAPH_PTNUM];
			if( pNewPts==NULL )return;
			p->ptlist.nlen = p->ptlist.nlen+MAX_GRAPH_PTNUM;

			memcpy(pNewPts,p->ptlist.pts,sizeof(GrVertex2d)*p->ptlist.nuse);
			delete[] p->ptlist.pts;

			p->ptlist.pts = pNewPts;
		}
		
		p->ptlist.pts[p->ptlist.nuse] = *pt;
		p->ptlist.nuse++;
	}
}

void GrBuffer2d::MoveTo(PT_2D *pt, int info)
{
	if( !m_pCurGr )return;
	
	GrVertex2d vt;
	vt.x = pt->x-m_ptXoff;
	vt.y = pt->y-m_ptYoff; 
	vt.code = GRBUFFER_PTCODE_MOVETO;
	AddVertex(&vt);
}

void GrBuffer2d::LineTo(PT_2D *pt, int info)
{
	if( !m_pCurGr )return;
	
	GrVertex2d vt;
	vt.x = pt->x-m_ptXoff;
	vt.y = pt->y-m_ptYoff;
	vt.code = GRBUFFER_PTCODE_LINETO;
	AddVertex(&vt);
}


void GrBuffer2d::Lines(PT_2D *pts, int npt, int sizeofpt, int info)
{
	if( !m_pCurGr )return;
	
	for( int i=0; i<npt; i++)
	{
		if( i==0 )MoveTo(pts,info);
		else LineTo(pts+i,info);	
	}
}

void GrBuffer2d::Dash( PT_2D pt0, PT_2D pt1, double len0, double len1)
{
	if( len0<0 || len1<0 )return;
	
	double dis =  sqrt(
		(pt0.x-pt1.x)*(pt0.x-pt1.x) + 
		(pt0.y-pt1.y)*(pt0.y-pt1.y) 		
		);
	
	if( dis<1e-10 )return;
	
	double delta_x0 = len0/dis * (pt1.x-pt0.x), delta_x1 = len1/dis * (pt1.x-pt0.x);
	double delta_y0 = len0/dis * (pt1.y-pt0.y), delta_y1 = len1/dis * (pt1.y-pt0.y);
//	double delta_z0 = len0/dis * (pt1.z-pt0.z), delta_z1 = len1/dis * (pt1.z-pt0.z);
	
	double len = 0;
	int state = 0;	
	MoveTo(&pt0);
	while( 1 )
	{
		if( state==0 )
		{
			if( len+len0>dis )break;
			pt0.x += delta_x0;  pt0.y += delta_y0; 
			LineTo(&pt0);
			len += len0;
		}
		else
		{
			if( len+len1>dis )break;
			pt0.x += delta_x1;  pt0.y += delta_y1; 
			MoveTo(&pt0);
			len += len1;
		}
		
		state = 1-state;
	}
	
	if( state==0 )
		LineTo(&pt1);
	else
		MoveTo(&pt1);	
}

void GrBuffer2d::End(BOOL bCompress)
{
	if( !m_pCurGr )return;

	GrVertexList2d *pList;
	Graph2d *pGr = m_pCurGr;

	int zero_len = 0;
	
	if( IsGrLineString2d(pGr) )
		pList = &(((GrLineString2d*)pGr)->ptlist);
	else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
	{
		pList = &(((GrPointString2d*)pGr)->ptlist);
		zero_len = 0;
	}
	else if( IsGrPolygon2d(pGr) )
		pList = &(((GrPolygon2d*)pGr)->ptlist);

	if( bCompress && pList->nuse>0 )
	{
		SetCompressInfo(pList);
	}

	if( pList->nuse<=zero_len )
	{
		if( pList->pts )delete[] pList->pts;

		if( IsGrPointString2dEx(pGr) )
		{
			if( ((GrPointString2dEx*)pGr)->attrList.pts )
				delete[] ((GrPointString2dEx*)pGr)->attrList.pts;
		}

		pGr = m_pHeadGr;
		while( pGr!=NULL && pGr->next!=m_pCurGr )
		{
			pGr = pGr->next;
		}

		if( pGr!=NULL && pGr->next==m_pCurGr )
		{
			pGr->next = NULL;
		}

		if( m_pHeadGr==m_pCurGr )
			m_pHeadGr = NULL;

		DeleteGraph2d(m_pCurGr);
		m_pCurGr = pGr;

		return;
	}

	CalcEnvelope(m_pCurGr);
}

void GrBuffer2d::ZoomCompressInfo(double scale)
{
	int n = log(scale)/log(2.0);
	if (n == 0/* || n > 8 || n < -8*/) return;
	else if(n > 8 || n < -8)
	{
		n = n>8?8:-8;
	}
	
	Graph2d *pGr = HeadGraph();

	while( pGr )
	{
		GrVertexList2d *pList = NULL;

		if( IsGrLineString2d(pGr) )
			pList = &(((GrLineString2d*)pGr)->ptlist);
		else if( IsGrPolygon2d(pGr) )
			pList = &(((GrPolygon2d*)pGr)->ptlist);
		else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
		{
			pList = &(((GrPointString2d*)pGr)->ptlist);
		}

		if (pList == NULL) 
		{
			pGr = pGr->next;
			continue;
		}

		GrVertex2d *pts = pList->pts;
		if (n > 0)
		{
			for(int i=0; i<pList->nuse; i++,pts++)
			{		
				BYTE b = (pts->code&1);
				pts->code >>= n;
				pts->code |= (0xFF<<(8-n));

				pts->code &= 0xFE;
				pts->code |= b;
				
			}
		}
		else
		{
			for(int i=0; i<pList->nuse; i++,pts++)
			{	
				BYTE b = (pts->code&1);
				pts->code <<= (-n);
				pts->code |= (0xFF>>(8+n));

				pts->code &= 0xFE;
				pts->code |= b;
				
			}
			
		}
		
		pGr = pGr->next;
	}	

}

void GrBuffer2d::SetCompressInfo(GrVertexList2d *pList)
{
	GrStreamCompress2d comp;
	long code = 0x80;

	int i,j,idx, start, spos, skip;
	GrVertex2d *pts;
	GR_PT_2D pt3d;

	pts = pList->pts;
	for(i=0; i<pList->nuse; i++,pts++)
	{
		pts->code = (pts->code&1);
	}

	for( j=0; j<7; j++)
	{
		idx = -1, start = 1, spos = 0;
		comp.BeginCompress(COMP_SCALE0/(0x40>>j));
		pts = pList->pts;
		for(i=0; i<pList->nuse; i++,pts++)
		{
			if( j>0 && (pts->code&(code<<1))==0 )
			{
				comp.SkipPt();
				continue;
			}
			if( (start==1 && i<pList->nuse-1 && (pts[1].code&1)==GRBUFFER_PTCODE_LINETO )
				|| (pts->code&1)==GRBUFFER_PTCODE_LINETO )
			{
				start = 0;
				if( comp.AddStreamPt(*pts)==2 )
				{
					idx = comp.GetLastCompPt(pt3d)-1;
					if( idx>=0 )pList->pts[idx+spos].code |= code;
				}
			}
			else
			{
				idx = comp.GetCurCompPt(pt3d)-1;
				//if( idx>=0 && 1.0/(0x40>>j)<sqrt((pts->x-x)*(pts->x-x)+(pts->y-y)*(pts->y-y)) )
				if( idx>=0 && COMP_SCALE0/(0x40>>j)<_FABS(pts->x-pt3d.x)+_FABS(pts->y-pt3d.y) )
				{
					pList->pts[idx+spos].code |= code;
					
					start = 1, spos = i, skip = 0;
					comp.BeginCompress(COMP_SCALE0/(0x40>>j));
					pts--;
					i--;
				}
				else 
				{
					comp.SkipPt();
				}
			}
		}
		
		idx = comp.GetCurCompPt(pt3d)-1;
		if( idx>=0 )pList->pts[idx+spos].code |= code;

		code = (code>>1);
	}

	pList->isCompress = 1;
}


void GrBuffer2d::RefreshEnvelope(BOOL bCalcCompressInfo)
{
	Graph2d *pGr = m_pHeadGr;
	while( pGr )
	{
		CalcEnvelope(pGr);

		if( bCalcCompressInfo )
		{
			GrVertexList2d *pList = NULL;
			
			if( IsGrLineString2d(pGr) )
				pList = &(((GrLineString2d*)pGr)->ptlist);
			else if( IsGrPolygon2d(pGr) )
				pList = &(((GrPolygon2d*)pGr)->ptlist);
			
			if( pList!=NULL && pList->nuse>2 )
			{
				SetCompressInfo(pList);
			}
		}

		pGr = pGr->next;
	}
}


void GrBuffer2d::CalcEnvelope(Graph2d *pGr)
{
	double xlow,xhigh,ylow,yhigh;
	int i;

	GrVertex2d *pts;
	GrVertexList2d *pList = NULL;

	if( IsGrLineString2d(pGr) )
	{
		pList = &(((GrLineString2d*)pGr)->ptlist);
	}
	else if( IsGrPolygon2d(pGr) )
	{
		pList = &(((GrPolygon2d*)pGr)->ptlist);
	}
	else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
	{
		pList = &(((GrPointString2d*)pGr)->ptlist);
	}

	if( !pList )return;
	pList->CutSpace();

	// get the coordinate range of the current element unit
	pts = pList->pts;	
	for(i=0; i<pList->nuse; i++,pts++)
	{
		if( i==0 )
		{
			xlow = xhigh = pts->x;
			ylow = yhigh = pts->y;
		}
		else
		{
			if( xlow>pts->x )xlow = pts->x;
			if( xhigh<pts->x )xhigh = pts->x;
			if( ylow>pts->y )ylow = pts->y;
			if( yhigh<pts->y )yhigh = pts->y;
		}
	}

	if( IsGrLineString2d(pGr) )
	{
		((GrLineString2d*)pGr)->xl = xlow;
		((GrLineString2d*)pGr)->xh = xhigh;
		((GrLineString2d*)pGr)->yl = ylow;
		((GrLineString2d*)pGr)->yh = yhigh;
	}
	if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
	{
		((GrPointString2d*)pGr)->xl = xlow;
		((GrPointString2d*)pGr)->xh = xhigh;
		((GrPointString2d*)pGr)->yl = ylow;
		((GrPointString2d*)pGr)->yh = yhigh;
	}
	else if( IsGrPolygon2d(pGr) )
	{
		((GrPolygon2d*)pGr)->xl = xlow;
		((GrPolygon2d*)pGr)->xh = xhigh;
		((GrPolygon2d*)pGr)->yl = ylow;
		((GrPolygon2d*)pGr)->yh = yhigh;
	}
}

const Graph2d* GrBuffer2d::HeadGraph()const
{
	return m_pHeadGr;
}

Graph2d* GrBuffer2d::HeadGraph()
{
	return m_pHeadGr;
}

Envelope GrBuffer2d::GetEnvelope()const
{
	Envelope evlp,evlp0;
	if( !m_pHeadGr )
		return evlp;

	Graph2d *pGr = m_pHeadGr;
	while( pGr )
	{
		evlp0 = GetEnvelopeOfGraph2d(pGr);

		evlp.Union(&evlp0,2);
		pGr = pGr->next;
	}
	evlp.m_xl += m_ptXoff;
	evlp.m_xh += m_ptXoff;
	evlp.m_yl += m_ptYoff;
	evlp.m_yh += m_ptYoff;
	return evlp;
}


void GrBuffer2d::ZoomPointSize(double kx, double ky)
{
	Graph2d *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrPoint2d(pGr) )
		{
			GrPoint2d *pgr = (GrPoint2d*)(pGr);
			
			if( pgr->bGrdSize )
			{
				pgr->kx = pgr->kx*kx;
				pgr->ky = pgr->ky*ky;
			}
			
			CalcEnvelope(pGr);
		}
		else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
		{
			GrPointString2d *pgr = (GrPointString2d*)(pGr);
			
			if( pgr->bGrdSize )
			{
				pgr->kx = pgr->kx*kx;
				pgr->ky = pgr->ky*ky;
			}
			
			CalcEnvelope(pGr);
		}
		
		pGr = pGr->next;
	}
}


void GrBuffer2d::ZoomWidth(double k)
{
	Graph2d *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrPoint2d(pGr) )
		{
		}
		else if( IsGrPointString2d(pGr) )
		{
		}
		else if( IsGrLineString2d(pGr) )
		{
			GrLineString2d *pgr = (GrLineString2d*)(pGr);
			
			if( pgr->bGrdWid )
				pgr->width *= k;
			
			CalcEnvelope(pGr);
		}
		
		pGr = pGr->next;
	}
}

void GrBuffer2d::Transform(const double *m)
{
	Graph2d *pGr = m_pHeadGr;
	Envelope e = GetEnvelope();
	PT_2D pt[4];
	pt[0].x = e.m_xl;
	pt[0].y = e.m_yl;
	pt[1].x = e.m_xl;
	pt[1].y = e.m_yh;
	pt[2].x = e.m_xh;
	pt[2].y = e.m_yh;
	pt[3].x = e.m_xh;
	pt[3].y = e.m_yl;
	GraphAPI::TransformPointsWith33Matrix(m,pt,4);
	e.CreateFromPts(pt,4);
	float xoff = e.m_xl;
	float yoff = e.m_yl;
	PT_2D tem;
	while( pGr )
	{
		if( IsGrPoint2d(pGr) )
		{
			GrPoint2d *pgr = (GrPoint2d*)(pGr);
			tem.x = pgr->pt.x+m_ptXoff;
			tem.y = pgr->pt.y+m_ptYoff;
			GraphAPI::TransformPointsWith33Matrix(m,&tem,1);
			pgr->pt.x = tem.x - xoff;
			pgr->pt.y = tem.y - yoff;
		}
		else if( IsGrLineString2d(pGr) || IsGrPolygon2d(pGr) || IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
		{
			GrVertexList2d *pList;
			if( IsGrLineString2d(pGr) )
				pList = &((GrLineString2d*)pGr)->ptlist;
			else if( IsGrPolygon2d(pGr) )
				pList = &((GrPolygon2d*)pGr)->ptlist;
			else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
				pList = &((GrPointString2d*)pGr)->ptlist;

			for (int i=0;i<pList->nuse;i++)
			{
				tem.x = pList->pts[i].x+m_ptXoff;
				tem.y = pList->pts[i].y+m_ptYoff;
				GraphAPI::TransformPointsWith33Matrix(m,&tem,1);
				pList->pts[i].x = tem.x - xoff;
				pList->pts[i].y = tem.y - yoff;
			}
			
		}
		else if( IsGrText2d(pGr) )
		{
			GrText2d *pgr = (GrText2d*)(pGr);
			tem.x = pgr->pt.x+m_ptXoff;
			tem.y = pgr->pt.y+m_ptYoff;
			GraphAPI::TransformPointsWith33Matrix(m,&tem,1);
			pgr->pt.x = tem.x - xoff;
			pgr->pt.y = tem.y - yoff;
		}
		
		CalcEnvelope(pGr);
		pGr = pGr->next;
	}
	m_ptXoff = xoff;
	m_ptYoff = yoff;
}


void GrBuffer2d::KickoffSamePoints()
{
	CTempTolerance t(0,CTempTolerance::flagDistance);

	Graph2d *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrLineString2d(pGr) || IsGrPolygon2d(pGr) || IsGrPointString(pGr) )
		{
			GrVertexList2d *pList;
			if( IsGrLineString2d(pGr) )
				pList = &((GrLineString2d*)pGr)->ptlist;
			else if( IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
				pList = &((GrPointString2d*)pGr)->ptlist;
			else
				pList = &((GrPolygon2d*)pGr)->ptlist;
			
			pList->nuse = GraphAPI::GKickoffSame2DPoints(pList->pts,pList->nuse);
		}
		
		pGr = pGr->next;
	}
}


void GrBuffer2d::Move(double dx, double dy)
{
	/*
	double m[9];
	Matrix33FromMove(dx,dy,m);
	Transform(m);
	*/
	Graph2d *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrPoint(pGr) )
		{
			GrPoint2d *pgr = (GrPoint2d*)(pGr);
			pgr->pt.x += dx;
			pgr->pt.y += dy;
		}
		else if( IsGrLineString2d(pGr) || IsGrPolygon2d(pGr) || IsGrPointString2d(pGr) || IsGrPointString2dEx(pGr) )
		{
			GrVertexList2d *pList;
			if( IsGrLineString(pGr) )
				pList = &((GrLineString2d*)pGr)->ptlist;
			else if( IsGrPolygon(pGr) )
				pList = &((GrPolygon2d*)pGr)->ptlist;
			else if( IsGrPointString(pGr) || IsGrPointString2dEx(pGr) )
				pList = &((GrPointString2d*)pGr)->ptlist;
			
			GrVertex2d *pts = pList->pts;
			for( int i=0; i<pList->nuse; i++,pts++)
			{
				pts->x += dx;
				pts->y += dy;
			}
		}
		else if( IsGrText(pGr) )
		{
			GrText2d *pgr = (GrText2d*)(pGr);
			pgr->pt.x += dx;
			pgr->pt.y += dy;
		}
		
		CalcEnvelope(pGr);
		pGr = pGr->next;
	}
}


void GrBuffer2d::Zoom(double kx, double ky)
{
	double m[9];
	Matrix33FromZoom(kx,ky,m);
	Transform(m);
}


void GrBuffer2d::Rotate(double angle, const PT_2D *pt)
{
	double m[9];
	PT_2D opt(0,0);
	if( !pt )pt = &opt;
	
	Matrix33FromRotate(pt,angle,m);
	Transform(m);
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GrStreamCompress2d::GrStreamCompress2d()
{
	m_nLastPt= 0;
	m_nCurPt = -1;
	m_nSkipPt= 0;
	m_lfLimit= -1;
	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;
}

GrStreamCompress2d::~GrStreamCompress2d()
{
	
}

void GrStreamCompress2d::BeginCompress(double limit)
{
	if( limit>0 )m_lfLimit = limit;
	m_nCurPt = 0;
	m_nLastPt= 0;
	m_nSkipPt= 0;
	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;
}


void GrStreamCompress2d::EndCompress()
{
	m_nCurPt = -1;
}

int GrStreamCompress2d::GetLastCompPt(GR_PT_2D &pt)
{
	if( m_nLastPt>=1 )
	{
		pt = m_ptLastComp;
	}
	return m_nLastPt;
}

int GrStreamCompress2d::GetCurCompPt(GR_PT_2D &pt)
{
	if( m_nCurPt>=1 )
	{
		pt = m_ptCurComp;
	}
	return m_nCurPt;
}

int  GrStreamCompress2d::AddStreamPt(GR_PT_2D pt)
{
	if( m_nCurPt<0 )
		return 0;
	
	if( m_nCurPt==0 )
	{
		m_ptCurComp = pt;

		m_nCurPt += m_nSkipPt;
		m_nSkipPt = 0;
		m_nCurPt++;
		return 1;
	}
	
	if( m_nLastPt==0 )
	{
		m_ptLastComp = m_ptCurComp;
		m_ptCurComp = pt;

		m_nLastPt = m_nCurPt;
		m_nCurPt += m_nSkipPt;
		m_nSkipPt = 0;
		m_nCurPt++;
		return 2;
	}

	//求 m_ptCurComp 在 m_ptLastComp-pt 直线上的垂足
	double x,y,t;
	{
		double dx,dy;
		
		dx = pt.x-m_ptLastComp.x; dy = pt.y-m_ptLastComp.y;
		if( _FABS(dx)<1e-10 && _FABS(dy)<1e-10 )
		{
			x = m_ptLastComp.x; y = m_ptLastComp.y;
			t = 0;
		}
		else
		{

			t = ((m_ptCurComp.x-m_ptLastComp.x)*dx+(m_ptCurComp.y-m_ptLastComp.y)*dy)/
				(dx*dx+dy*dy);

			x = m_ptLastComp.x + t * dx; y = m_ptLastComp.y + t * dy;
		}
	}

	//判断 m_ptCurComp 在m_ptLastComp-pt 直线上的方位(是向左还是向右)
	bool bclockwise = ((m_ptCurComp.x-m_ptLastComp.x)*(pt.y-m_ptCurComp.y)-(pt.x-m_ptCurComp.x)*(m_ptCurComp.y-m_ptLastComp.y))<0;

	//计算各个方向的偏距
	double disy = _FABS(x-m_ptCurComp.x)+_FABS(y-m_ptCurComp.y);
	double disx = _FABS(x-pt.x)+_FABS(y-pt.y);

	if( t>=1.0 || t<0.0 )
	{
		m_lfMaxX += disx;
		if( m_lfMinX>=disx )m_lfMinX -= disx;
		else m_lfMinX = 0;
	}
	else
	{
		m_lfMinX += disx;
		if( m_lfMaxX>=disx )m_lfMaxX -= disx;
		else m_lfMaxX = 0;
	}

	if( bclockwise )
	{
		m_lfMaxY += disy;
		if( m_lfMinY>=disy )m_lfMinY -= disy;
		else m_lfMinY = 0;
	}
	else
	{
		m_lfMinY += disy;	
		if( m_lfMaxY>=disy )m_lfMaxY -= disy;
		else m_lfMaxY = 0;
	}

	double minX = m_lfMaxX;
	if( minX>=m_lfLimit || m_lfMinY>=m_lfLimit || m_lfMaxY>=m_lfLimit )
	{
		m_ptLastComp = m_ptCurComp;
		m_ptCurComp = pt;
		m_lfMaxX = 0;
		m_lfMinX = m_lfMinY = m_lfMaxY = 0;

		m_nLastPt = m_nCurPt;
		m_nCurPt += m_nSkipPt;
		m_nSkipPt = 0;
		m_nCurPt++;

		return 2;
	}

	m_nCurPt += m_nSkipPt;
	m_nSkipPt = 0;
	m_nCurPt++;

	m_ptCurComp = pt;
	return 1;
}

template<class T>
BOOL Blob_ReadValue(const BYTE *&pData, const BYTE *pMaxPos, T& v)
{
	if( pData+sizeof(T)<=pMaxPos )
	{
		v = *(T*)pData;
		pData += sizeof(T);
		return TRUE;
	}
	return FALSE;
}


BOOL Blob_ReadString(const BYTE *&pData, const BYTE *pMaxPos, char *buf, int nbufLen)
{
	int nStringLen = strlen((char*)pData);
	if( (pData+nStringLen+1)<=pMaxPos )
	{
		strncpy(buf,(char*)pData,nbufLen);
		buf[nbufLen-1] = 0;
		pData += (nStringLen+1);
		return TRUE;
	}
	return FALSE;
}

template<class T>
BOOL Blob_WriteValue(BYTE *&pData, BYTE *pMaxPos, T v)
{
	if( pData+sizeof(T)<=pMaxPos )
	{
		*(T*)pData = v;
		pData += sizeof(T);
		return TRUE;
	}
	return FALSE;
}


template<class T>
BOOL Blob_WriteValue2(BYTE *&pData, BYTE *pMaxPos, BYTE bid, T v)
{
	int nRet = 0;
	nRet += Blob_WriteValue(pData,pMaxPos,bid);
	nRet += Blob_WriteValue(pData,pMaxPos,v);

	return (nRet==2);
}

BOOL Blob_WriteGraph2dBase(BYTE *&pData, BYTE *pMaxPos, Graph2d *p)
{
	int nRet = 0;
	nRet += Blob_WriteValue2(pData,pMaxPos,BID_type,(BYTE)p->type);
	nRet += Blob_WriteValue2(pData,pMaxPos,BID_color,(long)p->color);
	nRet += Blob_WriteValue2(pData,pMaxPos,BID_bUseSelfcolor,(BYTE)p->bUseSelfcolor);
	nRet += Blob_WriteValue2(pData,pMaxPos,BID_bUseBackColor,(BYTE)p->bUseBackColor);

	return (nRet==4);
}



BOOL Blob_WriteGrVertexList2d(BYTE *&pData, BYTE *pMaxPos, GrVertexList2d& list)
{
	Blob_WriteValue(pData,pMaxPos,(BYTE)BID_ptlist);
	Blob_WriteValue(pData,pMaxPos,(BYTE)list.isCompress);
	Blob_WriteValue(pData,pMaxPos,(short)list.nuse);
	for( int i=0; i<list.nuse; i++)
	{
		Blob_WriteValue(pData,pMaxPos,list.pts[i]);
	}
	return TRUE;
}

BOOL Blob_WriteString(BYTE *&pData, BYTE *pMaxPos, const char *buf)
{
	int nStringLen = strlen(buf);
	if( (pData+nStringLen+1)<=pMaxPos )
	{
		strncpy((char*)pData,buf,nStringLen);
		pData[nStringLen] = 0;
		pData += (nStringLen+1);
		return TRUE;
	}
	return FALSE;	
}

//为了支持扩展性，采用DXF风格的组码来写二进制数据
BOOL GrBuffer2d::ReadFromBlob(const BYTE *pData, int nLen)
{
	const BYTE *pData1 = pData;
	const BYTE *pMaxPos = pData+nLen;

	BYTE c, cv;
	int iv;
	float fv;
	double dv;
	short sv;
	long lv;
	GR_PT_2D ptv;
	char ch[1024] = {0};
	int chlen = 1024;

	DeleteAll();

	if( nLen<=0 )
		return TRUE;

	if( !Blob_ReadValue(pData1,pMaxPos,c) )
		return FALSE;

	if( c!=BID_org )
		return FALSE;

	if( !Blob_ReadValue(pData1,pMaxPos,fv) )
		return FALSE;
	float fXoff = fv;

	if( !Blob_ReadValue(pData1,pMaxPos,fv) )
		return FALSE;
	float fYoff = fv;

	int grtype = 0;

	Graph2d *p = NULL;

	while( Blob_ReadValue(pData1,pMaxPos,c) )
	{
		if( c==BID_none )
		{
			//从出现错误
			if( p!=NULL )DeleteGraph2d(p);
			return FALSE;
		}

		int read = 1;
		//一般信息
		switch( c )
		{
		case BID_type:
			{
				cv = 0;
				Blob_ReadValue(pData1,pMaxPos,cv);
				grtype = cv;
				if( p!=NULL )
					DeleteGraph2d(p);
				p = NULL;
				if( cv==GRAPH_TYPE_POINT2D )
					p = new GrPoint2d();
				else if( cv==GRAPH_TYPE_POINTSTRING2D )
					p = new GrPointString2d();
				else if( cv==GRAPH_TYPE_LINESTRING2D )
					p = new GrLineString2d();
				else if( cv==GRAPH_TYPE_POLYGON2D )
					p = new GrPolygon2d();
				else if( cv==GRAPH_TYPE_TEXT2D )
					p = new GrText2d();
			}
			break;
		case BID_color:
			if( p )
			{
				lv = 0;
				Blob_ReadValue(pData1,pMaxPos,lv);
				p->color = lv;
			}
			break;
		case BID_bUseSelfcolor:
			if( p )
			{
				cv = 0;
				Blob_ReadValue(pData1,pMaxPos,cv);
				p->bUseSelfcolor = (cv==0?0:1);
			}
			break;
		case BID_bUseBackColor:
			if( p )
			{
				cv = 0;
				Blob_ReadValue(pData1,pMaxPos,cv);
				p->bUseBackColor = (cv==0?0:1);
			}
			break;
		case BID_end:
			if( p )
			{
				InsertGraph(p,fXoff,fYoff,TRUE);
				DeleteGraph2d(p);
				p = NULL;
			}
			break;
		default:
			read = 0;
			;
		}

		if( read || !p )
		{
			continue;
		}

		//读取GrPoint2d的信息
		if( IsGrPoint2d(p) )
		{
			GrPoint2d *pg = (GrPoint2d*)p;
			switch( c )
			{
			case BID_kxy:
				{
					fv = 0;
					Blob_ReadValue(pData1,pMaxPos,fv);
					pg->kx = fv;
					fv = 0;
					Blob_ReadValue(pData1,pMaxPos,fv);
					pg->ky = fv;
				}
				break;
			case BID_angle:
				{
					fv = 0;
					Blob_ReadValue(pData1,pMaxPos,fv);
					pg->angle = fv;
				}
				break;
			case BID_bGrdSize:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->bGrdSize = (cv==0?0:1);
				}
				break;
			case BID_bFlat:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->bFlat = (cv==0?0:1);
				}
				break;
			case BID_cell:
				{
					lv = 0;
					Blob_ReadValue(pData1,pMaxPos,lv);
					pg->cell = lv;
				}
				break;
			case BID_width:
				{
					fv = 0;
					Blob_ReadValue(pData1,pMaxPos,fv);
					pg->width = fv;
				}
				break;
			case BID_pt:
				{
					ptv = GR_PT_2D();
					Blob_ReadValue(pData1,pMaxPos,ptv);
					pg->pt = ptv;
				}
				break;
			}
		}
		//读取GrPointString2d的信息
		else if( IsGrPointString2d(p) )
		{
			GrPointString2d *pg = (GrPointString2d*)p;
			switch( c )
			{
			case BID_range:
				{
					pg->xl = pg->xh = pg->yl = pg->yh = 0;
					Blob_ReadValue(pData1,pMaxPos,pg->xl);
					Blob_ReadValue(pData1,pMaxPos,pg->xh);
					Blob_ReadValue(pData1,pMaxPos,pg->yl);
					Blob_ReadValue(pData1,pMaxPos,pg->yh);
				}
				break;
			case BID_cell:
				{
					lv = 0;
					Blob_ReadValue(pData1,pMaxPos,lv);
					pg->cell = lv;
				}
				break;
			case BID_bGrdSize:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->bGrdSize = (cv==0?0:1);
				}
				break;
			case BID_width:
				{
					pg->width = 0;
					Blob_ReadValue(pData1,pMaxPos,pg->width);
				}
				break;
			case BID_kxy:
				{
					pg->kx = pg->ky = 0;
					Blob_ReadValue(pData1,pMaxPos,pg->kx);
					Blob_ReadValue(pData1,pMaxPos,pg->ky);
				}
				break;
			case BID_ptlist:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->ptlist.isCompress = cv;

					sv = 0;
					Blob_ReadValue(pData1,pMaxPos,sv);
					if( sv>0 )
					{
						pg->ptlist.pts = new GrVertex2d[sv];
						memset(pg->ptlist.pts,0,sizeof(GrVertex2d)*sv);
						for( int i=0; i<sv; i++)
						{
							Blob_ReadValue(pData1,pMaxPos,pg->ptlist.pts[i]);
						}
						pg->ptlist.nuse = sv;
						pg->ptlist.nlen = sv;
					}					
				}
				break;
			}
		}
		//读取GrLineString2d的信息
		else if( IsGrLineString2d(p) )
		{
			GrLineString2d *pg = (GrLineString2d*)p;
			switch( c )
			{
			case BID_range:
				{
					pg->xl = pg->xh = pg->yl = pg->yh = 0;
					Blob_ReadValue(pData1,pMaxPos,pg->xl);
					Blob_ReadValue(pData1,pMaxPos,pg->xh);
					Blob_ReadValue(pData1,pMaxPos,pg->yl);
					Blob_ReadValue(pData1,pMaxPos,pg->yh);
				}
				break;
			case BID_style:
				{
					lv = 0;
					Blob_ReadValue(pData1,pMaxPos,lv);
					pg->style = lv;
				}
				break;
			case BID_bGrdWid:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->bGrdWid = (cv==0?0:1);
				}
				break;
			case BID_width:
				{
					pg->width = 0;
					Blob_ReadValue(pData1,pMaxPos,pg->width);
				}
				break;
			case BID_fDrawScale:
				{
					pg->fDrawScale = 0;
					Blob_ReadValue(pData1,pMaxPos,pg->fDrawScale);
				}
				break;
			case BID_ptlist:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->ptlist.isCompress = cv;

					sv = 0;
					Blob_ReadValue(pData1,pMaxPos,sv);
					if( sv>0 )
					{
						pg->ptlist.pts = new GrVertex2d[sv];
						for( int i=0; i<sv; i++)
						{
							Blob_ReadValue(pData1,pMaxPos,pg->ptlist.pts[i]);
						}
						pg->ptlist.nuse = sv;
						pg->ptlist.nlen = sv;
					}					
				}
				break;
			}
		}
		//读取GrPolygon2d的信息
		else if( IsGrPolygon2d(p) )
		{
			GrPolygon2d *pg = (GrPolygon2d*)p;
			switch( c )
			{
			case BID_range:
				{
					pg->xl = pg->xh = pg->yl = pg->yh = 0;
					Blob_ReadValue(pData1,pMaxPos,pg->xl);
					Blob_ReadValue(pData1,pMaxPos,pg->xh);
					Blob_ReadValue(pData1,pMaxPos,pg->yl);
					Blob_ReadValue(pData1,pMaxPos,pg->yh);
				}
				break;
			case BID_index:
				{
					lv = 0;
					Blob_ReadValue(pData1,pMaxPos,lv);
					pg->index = lv;
				}
				break;
			case BID_filltype:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->filltype = cv;
				}
				break;
			case BID_polygon_pts:
				{
					sv = 0;
					Blob_ReadValue(pData1,pMaxPos,sv);
					if( sv>0 )
					{
						pg->pts = new PT_2D[sv];
						for( int i=0; i<sv; i++)
						{
							Blob_ReadValue(pData1,pMaxPos,pg->pts[i]);
						}
					}
				}
				break;
			case BID_ptlist:
				{
					cv = 0;
					Blob_ReadValue(pData1,pMaxPos,cv);
					pg->ptlist.isCompress = cv;

					sv = 0;
					Blob_ReadValue(pData1,pMaxPos,sv);
					if( sv>0 )
					{
						pg->ptlist.pts = new GrVertex2d[sv];
						for( int i=0; i<sv; i++)
						{
							Blob_ReadValue(pData1,pMaxPos,pg->ptlist.pts[i]);
						}
						pg->ptlist.nuse = sv;
						pg->ptlist.nlen = sv;
					}					
				}
				break;
			}
		}
		//读取GrText2d的信息
		else if( IsGrText2d(p) )
		{
			GrText2d *pg = (GrText2d*)p;
			switch( c )
			{
			case BID_textSettings:
				{
					pg->settings = TextSettings();
					Blob_ReadValue(pData1,pMaxPos,pg->settings);
				}
				break;
			case BID_pt:
				{
					pg->pt = GR_PT_2D();
					Blob_ReadValue(pData1,pMaxPos,pg->pt);
				}
				break;
			case BID_text:
				{
					Blob_ReadString(pData1,pMaxPos,ch,chlen);
					int nch = strlen(ch);
					if( nch>0 )
					{
						pg->text = new TCHAR[nch+1];
						_tcscpy(pg->text,ConvertCharToTChar(ch));
					}
				}
				break;
			}
		}
	}

	return TRUE;
}


BOOL GrBuffer2d::WriteToBlob(BYTE *pData, int &nLen)
{
	if( pData==NULL )
	{
		nLen = 0;

		if( m_pHeadGr )
			nLen = 10;

		Graph2d *pGr = m_pHeadGr;
		while( pGr )
		{
			if( IsGrPoint2d(pGr) )
			{
				nLen += sizeof(GrPoint2d) + 30;
			}
			else if( IsGrPointString2d(pGr) )
			{
				GrPointString2d *p = (GrPointString2d*)pGr;
				nLen += sizeof(GrPointString2d) + 30 + p->ptlist.nuse*sizeof(GrVertex2d);
			}
			else if( IsGrLineString2d(pGr) )
			{
				GrLineString2d *p = (GrLineString2d*)pGr;
				nLen += sizeof(GrLineString2d) + 30 + p->ptlist.nuse*sizeof(GrVertex2d);
			}
			else if( IsGrPolygon2d(pGr) )
			{
				GrPolygon2d *p = (GrPolygon2d*)pGr;
				nLen += sizeof(GrPolygon2d) + 30 + p->ptlist.nuse*sizeof(GrVertex2d);

				if( p->pts )
					nLen += p->ptlist.nuse*sizeof(PT_2D);

			}
			else if( IsGrText2d(pGr) )
			{
				GrText2d *p = (GrText2d*)pGr;
				nLen += sizeof(GrText2d) + 20 + _tcslen(p->text);
			}
			pGr = pGr->next;
		}
	}
	else
	{
		BYTE *pData1 = pData;
		BYTE *pMaxPos = pData+nLen;

		if( m_pHeadGr )
		{
			Blob_WriteValue2(pData1,pMaxPos,BID_org,m_ptXoff);
			Blob_WriteValue(pData1,pMaxPos,m_ptYoff);
		}

		Graph2d *pGr = m_pHeadGr;
		while( pGr )
		{
			Blob_WriteGraph2dBase(pData1,pMaxPos,pGr);

			if( IsGrPoint2d(pGr) )
			{
				GrPoint2d *p = (GrPoint2d*)pGr;

				Blob_WriteValue2(pData1,pMaxPos,BID_kxy,p->kx);
				Blob_WriteValue(pData1,pMaxPos,p->ky);
				Blob_WriteValue2(pData1,pMaxPos,BID_angle,p->angle);
				Blob_WriteValue2(pData1,pMaxPos,BID_bGrdSize,(BYTE)p->bGrdSize);
				Blob_WriteValue2(pData1,pMaxPos,BID_bFlat,(BYTE)p->bFlat);
				Blob_WriteValue2(pData1,pMaxPos,BID_cell,(long)p->cell);
				Blob_WriteValue2(pData1,pMaxPos,BID_width,p->width);
				Blob_WriteValue2(pData1,pMaxPos,BID_pt,p->pt);
			}
			else if( IsGrPointString2d(pGr) )
			{
				GrPointString2d *p = (GrPointString2d*)pGr;
				Blob_WriteValue2(pData1,pMaxPos,BID_range,p->xl);
				Blob_WriteValue(pData1,pMaxPos,p->xh);
				Blob_WriteValue(pData1,pMaxPos,p->yl);
				Blob_WriteValue(pData1,pMaxPos,p->yh);
				Blob_WriteValue2(pData1,pMaxPos,BID_cell,(long)p->cell);
				Blob_WriteValue2(pData1,pMaxPos,BID_bGrdSize,(BYTE)p->bGrdSize);
				Blob_WriteValue2(pData1,pMaxPos,BID_width,p->width);
				Blob_WriteValue2(pData1,pMaxPos,BID_kxy,p->kx);
				Blob_WriteValue(pData1,pMaxPos,p->ky);
				
				Blob_WriteGrVertexList2d(pData1,pMaxPos,p->ptlist);
			}
			else if( IsGrLineString2d(pGr) )
			{
				GrLineString2d *p = (GrLineString2d*)pGr;
				Blob_WriteValue2(pData1,pMaxPos,BID_range,p->xl);
				Blob_WriteValue(pData1,pMaxPos,p->xh);
				Blob_WriteValue(pData1,pMaxPos,p->yl);
				Blob_WriteValue(pData1,pMaxPos,p->yh);
				Blob_WriteValue2(pData1,pMaxPos,BID_style,(long)p->style);
				Blob_WriteValue2(pData1,pMaxPos,BID_bGrdWid,(BYTE)p->bGrdWid);
				Blob_WriteValue2(pData1,pMaxPos,BID_width,p->width);
				Blob_WriteValue2(pData1,pMaxPos,BID_fDrawScale,p->fDrawScale);
				
				Blob_WriteGrVertexList2d(pData1,pMaxPos,p->ptlist);
			}
			else if( IsGrPolygon2d(pGr) )
			{
				GrPolygon2d *p = (GrPolygon2d*)pGr;
				Blob_WriteValue2(pData1,pMaxPos,BID_range,p->xl);
				Blob_WriteValue(pData1,pMaxPos,p->xh);
				Blob_WriteValue(pData1,pMaxPos,p->yl);
				Blob_WriteValue(pData1,pMaxPos,p->yh);
				Blob_WriteValue2(pData1,pMaxPos,BID_index,(long)p->index);
				Blob_WriteValue2(pData1,pMaxPos,BID_filltype,(BYTE)p->filltype);
				
				Blob_WriteGrVertexList2d(pData1,pMaxPos,p->ptlist);

				if( p->pts )
				{
					Blob_WriteValue(pData1,pMaxPos,(BYTE)BID_polygon_pts);
					Blob_WriteValue(pData1,pMaxPos,(short)p->ptlist.nuse);
					for( int i=0; i<p->ptlist.nuse; i++)
					{
						Blob_WriteValue(pData1,pMaxPos,p->pts[i]);
					}
				}
				
			}
			else if( IsGrText2d(pGr) )
			{
				GrText2d *p = (GrText2d*)pGr;
				Blob_WriteValue2(pData1,pMaxPos,BID_textSettings,p->settings);
				Blob_WriteValue2(pData1,pMaxPos,BID_pt,p->pt);
				Blob_WriteValue(pData1,pMaxPos,(BYTE)BID_text);
				Blob_WriteString(pData1,pMaxPos,ConvertTCharToChar(p->text));				
			}

			Blob_WriteValue(pData1,pMaxPos,(BYTE)BID_end);

			pGr = pGr->next;
		}

		nLen = pData1 - pData;
	}

	return TRUE;
}


Envelope GetEnvelopeOfGraph2d(const Graph2d *gr, float fDrawCellAngle, float fDrawCellKX, float fDrawCellKY, float fDrawScale)
{
	if( gr==NULL )return Envelope();
	if( IsGrPoint2d(gr) )
	{
		const GrPoint2d *grpt = (const GrPoint2d*)gr;

		static GrPoint2d last_pt;
		static Envelope last_evlp;
		static float last_drawAngle, last_drawCellKx, last_drawCellKy;
		if( grpt->cell!=0 && last_pt.cell==grpt->cell && last_pt.angle==grpt->angle && 
			last_pt.kx==grpt->kx && last_pt.ky==grpt->ky && last_drawAngle==fDrawCellAngle &&
			last_drawCellKx==fDrawCellKX && last_drawCellKy==fDrawCellKY)
		{
			double dx = grpt->pt.x-last_pt.pt.x; 
			double dy = grpt->pt.y-last_pt.pt.y;
			Envelope e2 = last_evlp;

			e2.m_xl += dx; e2.m_xh += dx;
			e2.m_yl += dy; e2.m_yh += dy;
			return e2;
		}

		GR_PT_2D pt = grpt->pt;
		CCellDefLib *pLib = GetCellDefLib();
		if( pLib!=NULL && grpt->cell>0 )
		{
			CellDef item = pLib->GetCellDef(grpt->cell-1);
			if( item.m_pgr!=NULL )
			{
				Envelope e0 = item.m_evlp;

				double fabxl = e0.m_xl<0?-e0.m_xl:e0.m_xl, fabxh = e0.m_xh<0?-e0.m_xh:e0.m_xh;
				double fabyl = e0.m_yl<0?-e0.m_yl:e0.m_yl, fabyh = e0.m_yh<0?-e0.m_yh:e0.m_yh;
				double maxx = fabxl>fabxh?fabxl:fabxh, maxy = fabyl>fabyh?fabyl:fabyh;

				double kx = grpt->kx*fDrawCellKX, ky = grpt->ky*fDrawCellKY;
				double r = maxx*kx + maxy*ky;

				float xoff, yoff;
				item.m_pgr->GetOrigin(xoff,yoff);

				e0.m_xl = xoff - r + pt.x; e0.m_xh = xoff + r + pt.x;
				e0.m_yl = yoff - r + pt.y; e0.m_yh = yoff + r + pt.y;

				last_pt = *grpt;
				last_evlp = e0;
				last_drawAngle = fDrawCellAngle;
				last_drawCellKx = fDrawCellKX;
				last_drawCellKy = fDrawCellKY;
				
				return e0;
			}

		}
		float sx = 0.5*grpt->kx*fDrawCellKX;
		float sy = 0.5*grpt->ky*fDrawCellKY;

		if( sx<0 )sx = -sx;
		if( sy<0 )sx = -sy;
		
		Envelope e;
		e.m_xl = pt.x-sx; e.m_xh = pt.x+sx;
		e.m_yl = pt.y-sy; e.m_yh = pt.y+sy;
		return e;
	}
	if( IsGrPointString2d(gr) || IsGrPointString2dEx(gr) )
	{
		const GrPointString2d *grpts = (const GrPointString2d*)gr;
		GrPoint2d pt = grpts->GetPoint(-1);
		
		Envelope e1 = GetEnvelopeOfGraph2d(&pt,fDrawCellAngle,fDrawCellKX,fDrawCellKY,fDrawScale);
		
		Envelope e = grpts->GetEnvelope();
		e.m_xl = e.m_xl+e1.m_xl; e.m_xh = e.m_xh+e1.m_xh;
		e.m_yl = e.m_yl+e1.m_yl; e.m_yh = e.m_yh+e1.m_yh;
		
		return e;
	}
	if( IsGrLineString2d(gr) )
	{
		float sx = 0;
		if (((GrLineString2d*)gr)->bGrdWid)
		{
			sx = 0.6*((GrLineString2d*)gr)->width;
		}
		if( sx<0 )sx = -sx;
		
		Envelope e = ((GrLineString2d*)gr)->GetEnvelope();
		e.m_xl = e.m_xl-sx; e.m_xh = e.m_xh+sx;
		e.m_yl = e.m_yl-sx; e.m_yh = e.m_yh+sx;

		return e;
	}
	if( IsGrPolygon2d(gr) )
	{
		return ((GrPolygon2d*)gr)->GetEnvelope();
	}
	if( IsGrText2d(gr) )
	{
		GrText2d *grtxt = (GrText2d*)gr;
		PT_2D pt[4];
		pt[0].x = grtxt->x[0]+grtxt->pt.x;
		pt[0].y = grtxt->y[0]+grtxt->pt.y;
		pt[1].x = grtxt->x[1]+grtxt->pt.x;
		pt[1].y = grtxt->y[1]+grtxt->pt.y;
		pt[2].x = grtxt->x[2]+grtxt->pt.x;
		pt[2].y = grtxt->y[2]+grtxt->pt.y;
		pt[3].x = grtxt->x[3]+grtxt->pt.x;
		pt[3].y = grtxt->y[3]+grtxt->pt.y;
		Envelope e;
		e.CreateFromPts(pt,4);
		return e;

//		return ((GrText2d*)gr)->GetEnvelope();
//		GR_PT_2D pt = ((GrText2d*)gr)->pt;
// 		pt.x += ((GrText2d*)gr)->dx * fDrawCellKX;
// 		pt.y -= ((GrText2d*)gr)->dy * fDrawCellKY;
// 		float szy = ((GrText2d*)gr)->set * 0.6 * max(fDrawCellKX,fDrawCellKY);
// 		float szx;
// 		if(((GrText2d*)gr)->text == NULL)
// 			szx = 0;
// 		else
// 			szx = ((GrText2d*)gr)->size * 0.6 * strlen(((GrText2d*)gr)->text) * max(fDrawCellKX,fDrawCellKY);
// 		Envelope e;

// 		szx = szy = max(szx,szy);

// 		e.m_xl = pt.x-szx; e.m_xh = pt.x+szx;
// 		e.m_yl = pt.y-szy; e.m_yh = pt.y+szy;
//		return e;
	}

	return Envelope();
}

void GrBuffer2d::SetOrigin(float xoff, float yoff)
{
	m_ptXoff = xoff;
	m_ptYoff = yoff;
}

void GrBuffer2d::GetOrigin(float &xoff, float &yoff) const
{
	xoff = m_ptXoff;
	yoff = m_ptYoff;
}



Envelope GetEnvelopeOfPointInString2d(const Graph2d *gr,float fDrawCellAngle, float fDrawCellKX, float fDrawCellKY, float fDrawScale)
{
	if( gr==NULL )return Envelope();
	if( IsGrPointString2d(gr) || IsGrPointString2dEx(gr) )
	{
		const GrPointString2d *grpts = (const GrPointString2d*)gr;
		GrPoint2d pt = grpts->GetPoint(-1);
		
		Envelope e1 = GetEnvelopeOfGraph2d(&pt,fDrawCellAngle,fDrawCellKX,fDrawCellKY,fDrawScale);
		
		return e1;
	}
	
	return Envelope();
}

MyNameSpaceEnd


