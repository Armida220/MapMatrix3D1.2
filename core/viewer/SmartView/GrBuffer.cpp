// Copyright (C) 2004 - 2005 Gibuilder Group

#include "stdafx.h"
#include <float.h>
#include <math.h>
#include "GrBuffer.h"
#include "SmartViewFunctions.h"
#include "CoordWnd.h"
#include "GrBuffer2d.h"
#include "symbolbase.h"
#include "PlotWChar.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_GRAPH_PTNUM				256


MyNameSpaceBegin

IMPLEMENT_MEMPOOL(GrPoint);


void GrVertexList::CutSpace()
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
		GrVertex *pts2 = new GrVertex[nuse];
		if( pts2 )
		{
			memcpy(pts2,pts,sizeof(GrVertex)*nuse);
			delete[] pts;
			pts = pts2;
			nlen = nuse;
		}
	}
}

void GrVertexList::CopyFrom(const GrVertexList *list)
{
	if( pts && nlen>=list->nlen )
	{
		memcpy(pts,list->pts,sizeof(GrVertex)*list->nuse);
		nuse = list->nuse;
		isCompress = list->isCompress;
	}
	else
	{
		GrVertex *pts2 = new GrVertex[list->nlen];
		if( pts2 )
		{
			memcpy(pts2,list->pts,sizeof(GrVertex)*list->nuse);
			if( pts )delete[] pts;
			pts = pts2;
			nlen = list->nlen;
			nuse = list->nuse;
			isCompress = list->isCompress;
		}		
	}
}

void GrVertexList::CopyFrom(const GrVertexList2d *list, double xoff, double yoff, double z)
{
	int i;
	if( pts && nlen>=list->nlen )
	{
		for( i=0; i<list->nuse; i++)
		{
			pts[i].x = list->pts[i].x+xoff;
			pts[i].y = list->pts[i].y+yoff;
			pts[i].z = z;
			pts[i].code = list->pts[i].code;
		}
		nuse = list->nuse;
		isCompress = list->isCompress;
	}
	else
	{
		GrVertex *pts2 = new GrVertex[list->nlen];
		if( pts2 )
		{
			for( i=0; i<list->nuse; i++)
			{
				pts2[i].x = list->pts[i].x+xoff;
				pts2[i].y = list->pts[i].y+yoff;
				pts2[i].z = z;

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

GrPoint GrPointString::GetPoint(int i)const
{
	GrPoint pt;

	if( i>=0 && i<ptlist.nuse )
	{
		pt.pt = (PT_3D)ptlist.pts[i];
		pt.angle = ptlist.pts[i].CodeToFloat();
	}
	else
	{
		pt.pt.x = pt.pt.y = pt.pt.z = 0;
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

void GrText::SetText(LPCTSTR str)
{
	if( str==NULL )return;
	if( text )delete[] text;
	text = NULL;
	
	int nLen =  _tcslen(str);
	if( nLen<=0 )return;	
	text = new TCHAR[nLen+1];
	if( text )_tcscpy(text,str);
}

void GrText::SetSettings(const TextSettings *textSettings)
{
	if(textSettings)
		memcpy(&settings,textSettings,sizeof(TextSettings));
}

BOOL GrText::CompareFont(const GrText *t)const
{
	if(  _tcscmp(settings.tcFaceName,t->settings.tcFaceName)==0 )
		return TRUE;
	return FALSE;
}

void GrText::Release()
{
	if( text )delete[] text;
}


GrBuffer::GrBuffer()
{
	m_pCurGr  = NULL;
	m_pHeadGr = NULL;
}

GrBuffer::~GrBuffer()
{
	DeleteAll();
}


void ReleaseGraph(Graph *p)
{
	if( IsGrPoint(p) )
		delete (GrPoint*)p;
	else if( IsGrLineString(p) )
		delete (GrLineString*)p;
	else if( IsGrPointString(p) )
		delete (GrPointString*)p;
	else if( IsGrPointStringEx(p) )
		delete (GrPointStringEx*)p;
	else if( IsGrPolygon(p) )
		delete (GrPolygon*)p;
	else if( IsGrText(p) )
		delete (GrText*)p;
}


void GrBuffer::DeleteAll()
{
	Graph *pGr = m_pHeadGr, *t;
	while( pGr )
	{
		if( IsGrLineString(pGr) )
		{
			if( ((GrLineString*)pGr)->ptlist.pts )
				delete[] ((GrLineString*)pGr)->ptlist.pts;
		}
		else if( IsGrPointString(pGr) )
		{
			if( ((GrPointString*)pGr)->ptlist.pts )
				delete[] ((GrPointString*)pGr)->ptlist.pts;
		}
		else if( IsGrPointStringEx(pGr) )
		{
			if( ((GrPointStringEx*)pGr)->ptlist.pts )
				delete[] ((GrPointStringEx*)pGr)->ptlist.pts;

			if( ((GrPointStringEx*)pGr)->attrList.pts )
				delete[] ((GrPointStringEx*)pGr)->attrList.pts;
		}
		else if( IsGrPolygon(pGr) )
		{
			if( ((GrPolygon*)pGr)->ptlist.pts )
				delete[] ((GrPolygon*)pGr)->ptlist.pts;
			if( ((GrPolygon*)pGr)->pts )
				delete[] ((GrPolygon*)pGr)->pts;
		}
		else if( IsGrText(pGr) )
		{
			((GrText*)pGr)->Release();
		}

		t = pGr;
		pGr = pGr->next;		
		ReleaseGraph(t);	
	}

	m_pHeadGr = NULL;
	m_pCurGr = NULL;
}



void GrBuffer::DeleteGraph(const Graph *pGr0)
{
	Graph *pGr = m_pHeadGr, *t;
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
			
			if( IsGrLineString(pGr) )
			{
				if( ((GrLineString*)pGr)->ptlist.pts )
					delete[] ((GrLineString*)pGr)->ptlist.pts;
			}
			else if( IsGrPointString(pGr) )
			{
				if( ((GrPointString*)pGr)->ptlist.pts )
					delete[] ((GrPointString*)pGr)->ptlist.pts;
			}
			else if( IsGrPointStringEx(pGr) )
			{
				if( ((GrPointStringEx*)pGr)->ptlist.pts )
					delete[] ((GrPointStringEx*)pGr)->ptlist.pts;
				
				if( ((GrPointStringEx*)pGr)->attrList.pts )
					delete[] ((GrPointStringEx*)pGr)->attrList.pts;
			}
			else if( IsGrPolygon(pGr) )
			{
				if( ((GrPolygon*)pGr)->ptlist.pts )
					delete[] ((GrPolygon*)pGr)->ptlist.pts;
				if( ((GrPolygon*)pGr)->pts )
					delete[] ((GrPolygon*)pGr)->pts;
			}
			else if( IsGrText(pGr) )
			{
				((GrText*)pGr)->Release();
			}
			
			ReleaseGraph(pGr);
			break;
		}
		
		t = pGr;
		pGr = pGr->next;		
	}
}

void GrBuffer::InsertGraph(const Graph *pGr, BOOL bRecalcEvlp)
{
	if( IsGrPoint(pGr) )
	{
		GrPoint *pNew = new GrPoint();
		if( pNew )
		{
			memcpy(pNew,pGr,sizeof(GrPoint));
			pNew->next = NULL;
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPointString(pGr) )
	{
		GrPointString *pNew = new GrPointString();
		if( pNew )
		{
			GrPointString *p0 = (GrPointString*)pGr;
			pNew->color = p0->color;
			pNew->evlp = p0->evlp;
			pNew->width = p0->width;
			pNew->bGrdSize = p0->bGrdSize;
			pNew->cell = p0->cell;
			pNew->bFlat = p0->bFlat;
			pNew->bUseSelfcolor = p0->bUseSelfcolor;
			pNew->bUseBackColor = p0->bUseBackColor;
			pNew->kx = p0->kx;
			pNew->ky = p0->ky;
			pNew->ptlist.CopyFrom(&(p0->ptlist));

			if( bRecalcEvlp )
				CalcEnvelope(pNew);

			AddGraph(pNew);
		}
	}
	else if( IsGrPointStringEx(pGr) )
	{
		GrPointStringEx *pNew = new GrPointStringEx();
		if( pNew )
		{
			GrPointStringEx *p0 = (GrPointStringEx*)pGr;
			pNew->color = p0->color;
			pNew->evlp = p0->evlp;
			pNew->width = p0->width;
			pNew->bGrdSize = p0->bGrdSize;
			pNew->cell = p0->cell;
			pNew->bFlat = p0->bFlat;
			pNew->bUseSelfcolor = p0->bUseSelfcolor;
			pNew->bUseBackColor = p0->bUseBackColor;
			pNew->kx = p0->kx;
			pNew->ky = p0->ky;
			pNew->ptlist.CopyFrom(&(p0->ptlist));
			pNew->attrList.CopyFrom(&(p0->attrList));

			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrLineString(pGr) )
	{
		GrLineString *pNew = new GrLineString();
		if( pNew )
		{
			GrLineString *p0 = (GrLineString*)pGr;
			pNew->color = p0->color;
			pNew->evlp = p0->evlp;
			pNew->width = p0->width;
			pNew->bGrdWid = p0->bGrdWid;
			pNew->style = p0->style;
			pNew->fDrawScale = p0->fDrawScale;
			pNew->bUseSelfcolor = p0->bUseSelfcolor;
			pNew->bUseBackColor = p0->bUseBackColor;
			pNew->ptlist.CopyFrom(&(p0->ptlist));
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPolygon(pGr) )
	{
		GrPolygon *pNew = new GrPolygon();
		if( pNew )
		{
			GrPolygon *p0 = (GrPolygon*)pGr;
			pNew->color = p0->color;
			pNew->index = p0->index;
			pNew->evlp = p0->evlp;
			pNew->bUseSelfcolor = p0->bUseSelfcolor;
			pNew->bUseBackColor = p0->bUseBackColor;
			pNew->filltype = p0->filltype;
			pNew->ptlist.CopyFrom(&(p0->ptlist));
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrText(pGr) )
	{
		GrText *pNew = new GrText();
		if( pNew )
		{
			GrText *p0 = (GrText*)pGr;
			pNew->color = p0->color;
			pNew->SetText(p0->text);
			pNew->SetSettings(&p0->settings);
			pNew->pt = p0->pt;
			pNew->bUseSelfcolor = p0->bUseSelfcolor;
			pNew->bUseBackColor = p0->bUseBackColor;
			pNew->bGrdSize = p0->bGrdSize;
			pNew->bRotateWithView = p0->bRotateWithView;
			if( !bRecalcEvlp )
			{
				memcpy(pNew->x, p0->x, sizeof(pNew->x));
				memcpy(pNew->y, p0->y, sizeof(pNew->y));
				pNew->zmin = p0->zmin;
				pNew->zmax = p0->zmax;
//				memcpy(pNew->z,p0->z,sizeof(pNew->z));	
			}
			else
			{
				GrText *p = (GrText *)pGr;

				double ex[4], ey[4];
				g_Fontlib.SetSettings(&p->settings);
				g_Fontlib.GetEnvlope(p->text,ex,ey);
				
				p->x[0] = /*p->pt.x+*/ex[0];
				p->y[0] = /*p->pt.y+*/ey[0];
				p->x[1] = /*p->pt.x+*/ex[1];
				p->y[1] = /*p->pt.y+*/ey[1];
				p->x[2] = /*p->pt.x+*/ex[2];
				p->y[2] = /*p->pt.y+*/ey[2];
				p->x[3] = /*p->pt.x+*/ex[3];
				p->y[3] = /*p->pt.y+*/ey[3];
				p->zmin = FLT_MIN;
				p->zmax = FLT_MAX;
			}			
			AddGraph(pNew);
		}
	}
}

void GrBuffer::AddBuffer(const GrBuffer *pBuf)
{
	if( !pBuf )return;
		
	const Graph *pGr = pBuf->HeadGraph();
	while( pGr )
	{
		InsertGraph(pGr);
		
		pGr = pGr->next;
	}
}


void GrBuffer::InsertGraph(const Graph2d *pGr, double xoff, double yoff, double z, BOOL bRecalcEvlp)
{
	if( IsGrPoint2d(pGr) )
	{
		GrPoint *pNew = new GrPoint();
		if( pNew )
		{
			GrPoint2d *pgr = (GrPoint2d*)pGr;

			pNew->pt.x = pgr->pt.x+xoff;
			pNew->pt.y = pgr->pt.y+yoff;
			pNew->pt.z = z;
			pNew->color = pgr->color;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->angle = pgr->angle;
			pNew->width = pgr->width;
			pNew->cell = pgr->cell;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->bFlat = pgr->bFlat;

			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPointString2d(pGr) )
	{
		GrPointString *pNew = new GrPointString();
		if( pNew )
		{
			GrPointString2d *pgr = ((GrPointString2d*)pGr);

			pNew->color = pgr->color;
			pNew->evlp = pgr->GetEnvelope();
			pNew->evlp.m_xl += xoff; pNew->evlp.m_xh += xoff;
			pNew->evlp.m_yl += yoff; pNew->evlp.m_yh += yoff;
			pNew->evlp.m_zl = pNew->evlp.m_zh = z;

			pNew->width = pgr->width;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->cell = pgr->cell;
			pNew->bFlat = pgr->bFlat;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),xoff,yoff,z);
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPointString2dEx(pGr) )
	{
		GrPointStringEx *pNew = new GrPointStringEx();
		if( pNew )
		{
			GrPointString2dEx *pgr = ((GrPointString2dEx*)pGr);

			pNew->color = pgr->color;
			pNew->evlp = pgr->GetEnvelope();
			pNew->evlp.m_xl += xoff; pNew->evlp.m_xh += xoff;
			pNew->evlp.m_yl += yoff; pNew->evlp.m_yh += yoff;
			pNew->evlp.m_zl = pNew->evlp.m_zh = z;

			pNew->width = pgr->width;
			pNew->bGrdSize = pgr->bGrdSize;
			pNew->cell = pgr->cell;
			pNew->bFlat = pgr->bFlat;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->kx = pgr->kx;
			pNew->ky = pgr->ky;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),xoff,yoff,z);
			pNew->attrList.CopyFrom(&(pgr->attrList));
			
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrLineString2d(pGr) )
	{
		GrLineString *pNew = new GrLineString();
		if( pNew )
		{
			GrLineString2d *pgr = (GrLineString2d*)pGr;

			pNew->color = pgr->color;
			pNew->evlp = pgr->GetEnvelope();

			pNew->evlp.m_xl += xoff; pNew->evlp.m_xh += xoff;
			pNew->evlp.m_yl += yoff; pNew->evlp.m_yh += yoff;
			pNew->evlp.m_zl = pNew->evlp.m_zh = z;
			pNew->width = pgr->width;
			pNew->bGrdWid = pgr->bGrdWid;
			pNew->style = pgr->style;
			pNew->fDrawScale = pgr->fDrawScale;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),xoff,yoff,z);
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrPolygon2d(pGr) )
	{
		GrPolygon *pNew = new GrPolygon();
		if( pNew )
		{
			GrPolygon2d *pgr = (GrPolygon2d*)pGr;
			
			pNew->color = pgr->color;
			pNew->index = pgr->index;
			pNew->evlp = pgr->GetEnvelope();

			pNew->evlp.m_xl += xoff; pNew->evlp.m_xh += xoff;
			pNew->evlp.m_yl += yoff; pNew->evlp.m_yh += yoff;
			pNew->evlp.m_zl = pNew->evlp.m_zh = z;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;
			pNew->bUseBackColor = pgr->bUseBackColor;
			pNew->filltype = pgr->filltype;
			pNew->ptlist.CopyFrom(&(pgr->ptlist),xoff,yoff,z);
			if( bRecalcEvlp )
				CalcEnvelope(pNew);
			AddGraph(pNew);
		}
	}
	else if( IsGrText2d(pGr) )
	{
		GrText *pNew = new GrText();
		if( pNew )
		{
			GrText2d *pgr = (GrText2d*)pGr;

			pNew->color = pgr->color;
			pNew->SetText(pgr->text);
			pNew->SetSettings(&(pgr->settings));
			pNew->pt.x = pgr->pt.x+xoff;
			pNew->pt.y = pgr->pt.y+yoff;
			pNew->pt.z = z;	
			pNew->bGrdSize = pgr->bGrdSize;	
			pNew->bRotateWithView = pgr->bRotateWithView;
			pNew->bUseSelfcolor = pgr->bUseSelfcolor;	
			pNew->bUseBackColor = pgr->bUseBackColor;
			if(!bRecalcEvlp)
			{
				pNew->x[0] = pgr->x[0];
				pNew->x[1] = pgr->x[1];
				pNew->x[2] = pgr->x[2];
				pNew->x[3] = pgr->x[3];
				pNew->y[0] = pgr->y[0];
				pNew->y[1] = pgr->y[1];
				pNew->y[2] = pgr->y[2];
				pNew->y[3] = pgr->y[3];
				pNew->zmin = FLT_MIN;
				pNew->zmax = FLT_MAX;
//				memset(pNew->z , 0,sizeof(pNew->z));
			}
			else
			{
				GrText *p = (GrText *)pNew;
				double ex[4], ey[4];
				g_Fontlib.SetSettings(&p->settings);
				g_Fontlib.GetEnvlope(p->text,ex,ey);

				p->x[0] = /*p->pt.x+*/ex[0];
				p->y[0] = /*p->pt.y+*/ey[0];
				p->x[1] = /*p->pt.x+*/ex[1];
				p->y[1] = /*p->pt.y+*/ey[1];
				p->x[2] = /*p->pt.x+*/ex[2];
				p->y[2] = /*p->pt.y+*/ey[2];
				p->x[3] = /*p->pt.x+*/ex[3];
				p->y[3] = /*p->pt.y+*/ey[3];
				p->zmin = FLT_MIN;
				p->zmax = FLT_MAX;
			}				
			AddGraph(pNew);
		}
	}
}


void GrBuffer::AddBuffer(const GrBuffer2d *pBuf, double z)
{
	if( !pBuf )return;
	
	float xoff, yoff;
	pBuf->GetOrigin(xoff, yoff);
	
	const Graph2d *pGr = pBuf->HeadGraph();
	while( pGr )
	{
		InsertGraph(pGr, xoff, yoff, z);
		
		pGr = pGr->next;
	}
}

void GrBuffer::CopyFrom(const GrBuffer *pBuf)
{
	DeleteAll();
	AddBuffer(pBuf);
}


void GrBuffer::CutTo(GrBuffer *pBuf, BOOL bClearOld)
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

PT_3D GrBuffer::GetFirstPt()const
{
	const Graph *gr = HeadGraph();
	if( !gr )return PT_3D();

	if (IsGrPoint(gr))
	{
		const GrPoint *cgr = (const GrPoint*)gr;			
		return cgr->pt;
	}
	else if (IsGrPointString(gr)||IsGrPointStringEx(gr))
	{
		const GrPointString *cgr = (const GrPointString*)gr;		
		const GrVertexList *pList = &(cgr->ptlist);	
		if( pList->nuse<=0 )return PT_3D();
		return pList->pts[0];
	}	
	else if (IsGrLineString(gr))
	{
		const GrLineString *cgr = (const GrLineString*)gr;		
		const GrVertexList *pList = &(cgr->ptlist);	
		if( pList->nuse<=0 )return PT_3D();
		return pList->pts[0];
	}
	else if (IsGrPolygon(gr))
	{
		const GrPolygon *cgr = (const GrPolygon*)gr;	
		const GrVertexList *pList = &(cgr->ptlist);	
		if( pList->nuse<=0 )return PT_3D();
		return pList->pts[0];
	}
	else
	{
		const GrText *cgr = (const GrText *)gr;
		return cgr->pt;
	}
}

int GrBuffer::GetGraphPts(Graph *pGr, GrVertex *&pts)
{
	const GrVertexList *pList = NULL;
	pts = NULL;
	int num = 0;
	if( pGr )
	{
		pList = NULL;
		if( IsGrLineString(pGr) )
		{
			pList = &((GrLineString*)pGr)->ptlist;
		}
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			pList = &((GrPointString*)pGr)->ptlist;
		}		
		else if( IsGrPolygon(pGr) )
		{
			pList = &((GrPolygon*)pGr)->ptlist;
		}
		
		if( pList )
		{
			pts = pList->pts;
			num += pList->nuse;
		}
		
	}
	
	return num;
}

int GrBuffer::GetLinePts(PT_3D *pts,int sizeofpt)
{
	const Graph *pGr = HeadGraph();
	const GrVertexList *pList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		if( IsGrLineString(pGr) )
		{
			pList = &((GrLineString*)pGr)->ptlist;
		}		
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			pList = &((GrPointString*)pGr)->ptlist;
		}
		else if( IsGrPolygon(pGr) )
		{
			pList = &((GrPolygon*)pGr)->ptlist;
		}

		if( pList )
		{
			if( pts )
			{
				for( int i=0; i<pList->nuse; i++)
				{
					BYTE *pByte = (BYTE*)pts;
					PT_3D *pt = (PT_3D*)(pByte + (num+i)*sizeofpt);
					pt->x = pList->pts[i].x;
					pt->y = pList->pts[i].y;
					pt->z = pList->pts[i].z;
					if (sizeofpt == sizeof(PT_3DEX))
					{
						PT_3DEX *ptex = (PT_3DEX*)pt;
						ptex->pencode = pList->pts[i].code;
					}
				}
				
			}
			num += pList->nuse;
		}
		
		pGr = pGr->next;
	}

	return num;
}

int GrBuffer::GetVertexPts(GrVertex *pts)
{
	const Graph *pGr = HeadGraph();
	const GrVertexList *pList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		if( IsGrPoint(pGr) )
		{
			if( pts )
			{
				COPY_3DPT(pts[num],((GrPoint*)pGr)->pt);
				pts[num].code = 0;				
			}
			num++;
		}
		else if( IsGrLineString(pGr) )
		{
			pList = &((GrLineString*)pGr)->ptlist;
		}		
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			pList = &((GrPointString*)pGr)->ptlist;
		}
		else if( IsGrPolygon(pGr) )
		{
			pList = &((GrPolygon*)pGr)->ptlist;
		}
		else if( IsGrText(pGr) )
		{
			if( pts )
			{
				GrText *pGr2 = (GrText*)pGr;
				COPY_3DPT(pts[num], pGr2->pt);
				pts[num].code = 0;		
				num++;

				for (int i = 0; i < 4; i++)
				{
					pts[num].x = pGr2->x[i] + pGr2->pt.x;
					pts[num].y = pGr2->y[i] + pGr2->pt.y;
					pts[num].z = pGr2->pt.z;
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
				memcpy(pts+num,pList->pts,sizeof(GrVertex)*pList->nuse);	
			}
			num += pList->nuse;
		}
		
		pGr = pGr->next;
	}
	
	return num;
}


void GrBuffer::SetAllPts(const PT_3D *pts)
{
	if( !pts )
		return;

	Graph *pGr = HeadGraph();
	GrVertexList *pList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		if( IsGrPoint(pGr) )
		{
			((GrPoint*)pGr)->pt = pts[num];
			num++;
		}
		else if( IsGrLineString(pGr) )
		{
			pList = &((GrLineString*)pGr)->ptlist;
		}		
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			pList = &((GrPointString*)pGr)->ptlist;
		}
		else if( IsGrPolygon(pGr) )
		{
			pList = &((GrPolygon*)pGr)->ptlist;
		}
		else if( IsGrText(pGr) )
		{
			GrText *pGr2 = (GrText*)pGr;
			((GrText*)pGr)->pt = pts[num];
			num++;

			for (int i = 0; i < 4; i++)
			{
				pGr2->x[i] = pts[num].x - pGr2->pt.x;
				pGr2->y[i] = pts[num].y - pGr2->pt.y;
				num++;
			}
		}
		
		if( pList )
		{
			const PT_3D *pts2 = pts + num;
			for( int i=0; i<pList->nuse; i++)
			{
				pList->pts[i].x = pts2[i].x;
				pList->pts[i].y = pts2[i].y;
				pList->pts[i].z = pts2[i].z;
			}				

			num += pList->nuse;
		}
		
		pGr = pGr->next;
	}	
}


int	GrBuffer::GetPtAttrs(GrVertex *pts, GrVertexAttr *attrs)
{
	const Graph *pGr = HeadGraph();
	const GrVertexList *pList;
	const GrVertexAttrList *pAttrList;
	int num = 0;
	while( pGr )
	{
		pList = NULL;
		pAttrList = NULL;
		if( IsGrPointStringEx(pGr) )
		{
			pList = &((GrPointStringEx*)pGr)->ptlist;
			pAttrList = &((GrPointStringEx*)pGr)->attrList;
		}
		
		if( pList )
		{
			if( pts )
			{
				memcpy(pts+num,pList->pts,sizeof(GrVertex)*pList->nuse);	
			}
		}

		if( pAttrList )
		{
			if( attrs )
			{
				memcpy(attrs+num,pAttrList->pts,sizeof(GrVertexAttr)*pAttrList->nuse);		
			}
		}

		if (pList)
		{
			num += pList->nuse;
		}
		
		pGr = pGr->next;
	}
	
	return num;
}


void GrBuffer::SetPtAttrs(GrVertexAttr *attrs)
{
	if( !attrs )
		return;

	Graph *pGr = HeadGraph();
	GrVertexAttrList *pAttrList;
	int num = 0;
	while( pGr )
	{
		pAttrList = NULL;
		if( IsGrPointStringEx(pGr) )
		{
			pAttrList = &((GrPointStringEx*)pGr)->attrList;
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

COLORREF GrBuffer::MatchColor(COLORREF clr)
{
	return clr;
}

void GrBuffer::AddGraph(Graph *pGr)
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

void GrBuffer::BeginPointString(COLORREF clr, float kx,float ky, BOOL bGrdSize, int cell, float width, BOOL bFlat, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPointString *pGr = new GrPointString();
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

void GrBuffer::PointString(PT_3D *pt, float fAngle)
{
	GrVertex gpt;
	gpt.x = pt->x;
	gpt.y = pt->y;
	gpt.z = pt->z;

	gpt.CodeFromFloat(fAngle);

	AddVertex(&gpt);
}

void GrBuffer::BeginPointStringEx(COLORREF clr, float kx,float ky, BOOL bGrdSize, int cell, float width, BOOL bFlat, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPointStringEx *pGr = new GrPointStringEx();
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

void GrBuffer::PointStringEx(PT_3D *pt, COLORREF color, BOOL isMark, float fAngle)
{
	GrVertex gpt;
	gpt.x = pt->x;
	gpt.y = pt->y;
	gpt.z = pt->z;

	gpt.CodeFromFloat(fAngle);

	AddVertex(&gpt);

	if( IsGrPointStringEx(m_pCurGr) )
	{
		GrPointStringEx *p = (GrPointStringEx*)m_pCurGr;
		if( p->attrList.nuse>=0 && p->attrList.nuse<p->attrList.nlen )
		{
			p->attrList.pts[p->attrList.nuse].color = color;
			p->attrList.pts[p->attrList.nuse].isMarked = isMark;
			p->attrList.nuse++;
		}
	}
}

void GrBuffer::Point(COLORREF clr, PT_3D *pt, float kx,float ky, float fAngle, BOOL bGrdSize, int cell, float width, BOOL bFlat, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPoint *pGr = new GrPoint();
	if( !pGr )return;
	
	pGr->pt = *pt;
	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->kx = kx;
	pGr->ky = ky;
	pGr->angle = fAngle;
	pGr->bGrdSize = bGrdSize;
	pGr->width = width;
	pGr->cell = cell; 
	pGr->bFlat = bFlat;
	
	AddGraph(pGr);
}

void GrBuffer::Text(COLORREF clr, PT_3D *pt,  LPCTSTR text, const TextSettings *settings, BOOL bGrdSize, BOOL bRotateWithView, BOOL bCalcEnvelope, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	if(_tcslen(text)<=0)return;
	GrText *pGr = new GrText();
	if( !pGr )return;
	
	pGr->pt = *pt;
	pGr->SetText(text);
	pGr->SetSettings(settings);
	pGr->color = MatchColor(clr);
	pGr->bUseSelfcolor = bUseSelfcolor;
	pGr->bUseBackColor = bUseBackCol;
	pGr->bGrdSize = bGrdSize;
	pGr->bRotateWithView = bRotateWithView;
	if (bCalcEnvelope)
	{		
		
		GrText *p = (GrText *)pGr;

		double ex[4], ey[4];
		g_Fontlib.SetSettings(&p->settings);
		if( !g_Fontlib.GetEnvlope(p->text,ex,ey) )
			return;
		
		p->x[0] = /*p->pt.x+*/ex[0];
		p->y[0] = /*p->pt.y+*/ey[0];
		p->x[1] = /*p->pt.x+*/ex[1];
		p->y[1] = /*p->pt.y+*/ey[1];
		p->x[2] = /*p->pt.x+*/ex[2];
		p->y[2] = /*p->pt.y+*/ey[2];
		p->x[3] = /*p->pt.x+*/ex[3];
		p->y[3] = /*p->pt.y+*/ey[3];
		p->zmin = pt->z-1e-4;
		p->zmax = pt->z+1e-4;
	}	
 	AddGraph(pGr);
}

void GrBuffer::BeginLineString(COLORREF clr, float width, BOOL bGrdWid, int style, float fDrawScale, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrLineString *pGr = new GrLineString();
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


void GrBuffer::BeginPolygon(COLORREF clr, int type, int index, const PT_2D *pts, int nPt, BOOL bUseSelfcolor, BOOL bUseBackCol)
{
	GrPolygon *pGr = new GrPolygon();
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

void GrBuffer::SetAllColor(COLORREF color)
{
	Graph *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if (!p->bUseSelfcolor)
		{
		p->color = (color&0xffffff);
		}		
		p = p->next;
	}
}


void GrBuffer::SetAllBackColor(BOOL bUseBackColor)
{
	Graph *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if (!p->bUseSelfcolor)
		{
			p->bUseBackColor = bUseBackColor?1:0;
		}		
		p = p->next;
	}
}

void GrBuffer::SetAllLineWidth(BOOL bGrdWid, float width)
{
	Graph *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrLineString(p) )
		{
			((GrLineString*)p)->bGrdWid = bGrdWid;
			((GrLineString*)p)->width = width;
		}
		p = p->next;
	}
}

void GrBuffer::SetAllLineWidthOrWidthScale(BOOL bGrdWid, float width)
{
	if( width>=0.0f )
		SetAllLineWidth(bGrdWid,width);
	else
		ZoomWidth(-width);
}

BOOL GrBuffer::IsOnlyType(int type)
{
	Graph *p = m_pHeadGr;
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

void GrBuffer::SetAllCell(int cell)
{
	Graph *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrPoint(p) )
		{
			((GrPoint*)p)->cell = cell;
		}
		else if( IsGrPointString(p)||IsGrPointStringEx(p) )
		{
			((GrPointString*)p)->cell = cell;
		}
		p = p->next;
	}
}

void GrBuffer::SetAllLineStyle(int style)
{
	Graph *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrLineString(p) )
		{
			((GrLineString*)p)->style = style;
		}
		p = p->next;
	}
}

void GrBuffer::SetAllPolygon(int type, int index, PT_2D *pts)
{
	Graph *p = m_pHeadGr;
	while( p!=NULL ) 
	{
		if( IsGrPolygon(p) )
		{
			GrPolygon *p2 = (GrPolygon*)p;

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


void GrBuffer::AddVertex(GrVertex *pt)
{	
	if( IsGrLineString(m_pCurGr) )
	{
		GrLineString *p = (GrLineString*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex[MAX_GRAPH_PTNUM];
			if( p->ptlist.pts==NULL )return;
			p->ptlist.nlen = MAX_GRAPH_PTNUM;
		}
		
		if( p->ptlist.nuse>=p->ptlist.nlen )
		{
			COLORREF color = p->color;
			float width = p->width;
			GrVertex pt0 = p->ptlist.pts[p->ptlist.nuse-1];
			BOOL bGrdWid = p->bGrdWid;
			int style = p->style;
			BOOL bUseSelfCol = p->bUseSelfcolor;
			
			End();
			BeginLineString(color,width,bGrdWid,style,1,bUseSelfCol);
			
			pt0.code = ((pt0.code&0xfffffffe)|GRBUFFER_PTCODE_MOVETO);
			AddVertex(&pt0);
			AddVertex(pt);
			return;
		}
		
		p->ptlist.pts[p->ptlist.nuse] = *pt;
		p->ptlist.nuse++;
		
	}
	else if( IsGrPointString(m_pCurGr) )
	{
		GrPointString *p = (GrPointString*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex[MAX_GRAPH_PTNUM];
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
	else if( IsGrPointStringEx(m_pCurGr) )
	{
		GrPointStringEx *p = (GrPointStringEx*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex[MAX_GRAPH_PTNUM];
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
	else if( IsGrPolygon(m_pCurGr) )
	{
		GrPolygon *p = (GrPolygon*)m_pCurGr;
		if( p->ptlist.pts==NULL )
		{
			p->ptlist.pts = new GrVertex[MAX_GRAPH_PTNUM];
			if( p->ptlist.pts==NULL )return;
			p->ptlist.nlen = MAX_GRAPH_PTNUM;
		}

		if( p->ptlist.nuse>=p->ptlist.nlen )
		{
			GrVertex *pNewPts = new GrVertex[p->ptlist.nlen+MAX_GRAPH_PTNUM];
			if( pNewPts==NULL )return;
			p->ptlist.nlen = p->ptlist.nlen+MAX_GRAPH_PTNUM;

			memcpy(pNewPts,p->ptlist.pts,sizeof(GrVertex)*p->ptlist.nuse);
			delete[] p->ptlist.pts;

			p->ptlist.pts = pNewPts;
		}
		
		p->ptlist.pts[p->ptlist.nuse] = *pt;
		p->ptlist.nuse++;
	}
}

void GrBuffer::MoveTo(PT_3D *pt, int info)
{
	if( !m_pCurGr )return;
	
	GrVertex vt;
	COPY_3DPT(vt,pt[0]); 
	if( info==-1 )
		vt.code = GRBUFFER_PTCODE_MOVETO;
	else
		vt.code = ((info&0xffffff00)|GRBUFFER_PTCODE_MOVETO);
	AddVertex(&vt);
}

void GrBuffer::LineTo(PT_3D *pt, int info)
{
	if( !m_pCurGr )return;
	
	GrVertex vt;
	COPY_3DPT(vt,pt[0]);  
	if( info==-1 )
		vt.code = GRBUFFER_PTCODE_LINETO;
	else
		vt.code = ((info&0xffffff00)|GRBUFFER_PTCODE_LINETO);
	AddVertex(&vt);
}

void GrBuffer::Dash( PT_3D pt0, PT_3D pt1, double len0, double len1)
{
	if( len0<0 || len1<0 )return;
	
	double dis =  sqrt(
		(pt0.x-pt1.x)*(pt0.x-pt1.x) + 
		(pt0.y-pt1.y)*(pt0.y-pt1.y) + 
		(pt0.z-pt1.z)*(pt0.z-pt1.z)
		);
	
	if( dis<1e-10 )return;
	
	double delta_x0 = len0/dis * (pt1.x-pt0.x), delta_x1 = len1/dis * (pt1.x-pt0.x);
	double delta_y0 = len0/dis * (pt1.y-pt0.y), delta_y1 = len1/dis * (pt1.y-pt0.y);
	double delta_z0 = len0/dis * (pt1.z-pt0.z), delta_z1 = len1/dis * (pt1.z-pt0.z);
	
	double len = 0;
	int state = 0;	
	MoveTo(&pt0);
	while( 1 )
	{
		if( state==0 )
		{
			if( len+len0>dis )break;
			pt0.x += delta_x0;  pt0.y += delta_y0;  pt0.z += delta_z0;
			LineTo(&pt0);
			len += len0;
		}
		else
		{
			if( len+len1>dis )break;
			pt0.x += delta_x1;  pt0.y += delta_y1;  pt0.z += delta_z1;
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

void GrBuffer::Lines(PT_3D *pts, int npt, int sizeofpt, int info)
{
	if( !m_pCurGr )return;

	GrVertex *pts2 = new GrVertex[npt];
	GrVertex *p2 = pts2;
	PT_3D *p1 = pts;
	for( int i=0; i<npt; i++, p2++)
	{
		COPY_3DPT(*p2,*p1);
		if( i==0 )
			p2->code = GRBUFFER_PTCODE_MOVETO;
		else
			p2->code = GRBUFFER_PTCODE_LINETO;

		p1 = (PT_3D*)(((BYTE*)p1) + sizeofpt);
	}

	Lines(pts2,npt);
	delete[] pts2;
}


void GrBuffer::Lines(GrVertex *pts, int npt)
{
	if( !m_pCurGr )return;
//	for(int i=0; i<npt; i++)
//		AddVertex(pts+i);

	if( IsGrLineString(m_pCurGr) )
	{
		int i, cpy_len;
		for( i=0; i<npt; )
		{
			GrLineString *p = (GrLineString*)m_pCurGr;
			if( p->ptlist.pts==NULL )
			{
				p->ptlist.pts = new GrVertex[MAX_GRAPH_PTNUM];
				if( p->ptlist.pts==NULL )return;
				p->ptlist.nlen = MAX_GRAPH_PTNUM;
			}
			
			if( p->ptlist.nuse>=p->ptlist.nlen )
			{
				COLORREF color = p->color;
				float width = p->width;
				GrVertex pt0 = p->ptlist.pts[p->ptlist.nuse-1];
				BOOL bGrdWid = p->bGrdWid;
				int style = p->style;
				BOOL bUseSelfCol = p->bUseSelfcolor;
				
				End();
				BeginLineString(color,width,bGrdWid,style,1,bUseSelfCol);
				
				pt0.code = ((pt0.code&0xfffffffe)|GRBUFFER_PTCODE_MOVETO);
				AddVertex(&pt0);
			}

			p = (GrLineString*)m_pCurGr;

			cpy_len = npt-i;
			if( cpy_len>(MAX_GRAPH_PTNUM-p->ptlist.nuse) )
				cpy_len = (MAX_GRAPH_PTNUM-p->ptlist.nuse);
			
			memcpy(p->ptlist.pts+p->ptlist.nuse,pts+i,cpy_len*sizeof(GrVertex));
			p->ptlist.nuse += cpy_len;
			i += cpy_len;
		}
		
	}
	else if( IsGrPolygon(m_pCurGr) )
	{
		GrPolygon *p = (GrPolygon*)m_pCurGr;
		
		if( p->ptlist.nuse+npt>p->ptlist.nlen )
		{
			int new_len = MAX_GRAPH_PTNUM * ((p->ptlist.nuse+npt+MAX_GRAPH_PTNUM-1)/MAX_GRAPH_PTNUM);

			GrVertex *pNewPts = new GrVertex[new_len];
			if( pNewPts==NULL )return;
			p->ptlist.nlen = new_len;
			
			if( p->ptlist.pts!=NULL )
			{
				memcpy(pNewPts,p->ptlist.pts,sizeof(GrVertex)*p->ptlist.nuse);
				delete[] p->ptlist.pts;
			}
			
			p->ptlist.pts = pNewPts;
		}
		
		memcpy(p->ptlist.pts+p->ptlist.nuse,pts,sizeof(GrVertex)*npt);
		p->ptlist.nuse += npt;
	}
}

void GrBuffer::End(BOOL bCompress)
{
	if( !m_pCurGr )return;

	GrVertexList *pList = NULL;
	Graph *pGr = m_pCurGr;

	int zero_len = 0;
	
	if( IsGrLineString(pGr) )
		pList = &(((GrLineString*)pGr)->ptlist);
	else if( IsGrPolygon(pGr) )
		pList = &(((GrPolygon*)pGr)->ptlist);
	else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
	{
		pList = &(((GrPointString*)pGr)->ptlist);
		zero_len = 0;
	}

	if( bCompress && pList->nuse>1 )
	{
		//SetCompressInfo(pList);
	}

	if( pList->nuse<=zero_len )
	{
		if( pList->pts )delete[] pList->pts;

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

		ReleaseGraph(m_pCurGr);
		m_pCurGr = pGr;

		return;
	}

	CalcEnvelope(m_pCurGr);
}

void GrBuffer::ZoomCompressInfo(double scale)
{
	int n = log(scale)/log(2.0);
	if (n == 0/* || n > 8 || n < -8*/) return;
	else if(n > 8 || n < -8)
	{
		n = n>8?8:-8;
	}
	
	Graph *pGr = HeadGraph();
	
	while( pGr )
	{
		GrVertexList *pList = NULL;
		
		if( IsGrLineString(pGr) )
			pList = &(((GrLineString*)pGr)->ptlist);
		else if( IsGrPolygon(pGr) )
			pList = &(((GrPolygon*)pGr)->ptlist);
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			pList = &(((GrPointString*)pGr)->ptlist);
		}
		
		if (pList == NULL)
		{
			pGr = pGr->next;
			continue;
		}
		
		GrVertex *pts = pList->pts;
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

void GrBuffer::SetCompressInfo(GrVertexList *pList)
{
	SimpleStreamCompress<PT_3D> comp;
	long code = 0x80;

	int i,j,idx, start, spos, skip;
	GrVertex *pts;
	PT_3D pt3d;

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
				if( idx>=0 && COMP_SCALE0/(0x40>>j)<_FABS(pts->x-pt3d.x)+_FABS(pts->y-pt3d.y)+_FABS(pts->z-pt3d.z) )
				{
					pList->pts[idx+spos].code |= code;
					
					start = 1, spos = i, skip = 0;
					comp.BeginCompress(COMP_SCALE0/(0x40>>j));
					pts--;
					i--;
				}
				else comp.SkipPt();
			}
		}
		
		idx = comp.GetCurCompPt(pt3d)-1;
		if( idx>=0 )pList->pts[idx+spos].code |= code;

		code = (code>>1);
	}

	pList->isCompress = 1;
}

void GrBuffer::RefreshEnvelope()
{
	Graph *pGr = m_pHeadGr;
	while( pGr )
	{
		CalcEnvelope(pGr);
		pGr = pGr->next;
	}
}

void GrBuffer::CalcEnvelope(Graph *pGr)
{
	double xlow,xhigh,ylow,yhigh,zlow,zhigh;
	int i;

	GrVertex *pts;
	GrVertexList *pList = NULL;
	Envelope *e = NULL;

	if( IsGrLineString(pGr) )
	{
		pList = &(((GrLineString*)pGr)->ptlist);
		e = &(((GrLineString*)pGr)->evlp);
	}
	else if( IsGrPolygon(pGr) )
	{
		pList = &(((GrPolygon*)pGr)->ptlist);
		e = &(((GrPolygon*)pGr)->evlp);
	}
	else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
	{
		pList = &(((GrPointString*)pGr)->ptlist);
		e = &(((GrPointString*)pGr)->evlp);
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
			zlow = zhigh = pts->z;
		}
		else
		{
			if( xlow>pts->x )xlow = pts->x;
			if( xhigh<pts->x )xhigh = pts->x;
			if( ylow>pts->y )ylow = pts->y;
			if( yhigh<pts->y )yhigh = pts->y;
			if( zlow>pts->z )zlow = pts->z;
			if( zhigh<pts->z )zhigh = pts->z;
		}
	}

	(*e).m_xl = xlow; (*e).m_xh = xhigh; 
	(*e).m_yl = ylow; (*e).m_yh = yhigh; 
	(*e).m_zl = zlow; (*e).m_zh = zhigh; 
}

const Graph* GrBuffer::HeadGraph()const
{
	return m_pHeadGr;
}

Graph* GrBuffer::HeadGraph()
{
	return m_pHeadGr;
}

Envelope GrBuffer::GetEnvelope()const
{
	Envelope evlp;
	if( !m_pHeadGr )
		return evlp;

	Graph *pGr = m_pHeadGr;
	while( pGr )
	{
		evlp.Union(&GetEnvelopeOfGraph(pGr),3);
		pGr = pGr->next;
	}
	
	return evlp;
}


void GrBuffer::ZoomPointSize(double kx, double ky)
{
	Graph *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrPoint(pGr) )
		{
			GrPoint *pgr = (GrPoint*)(pGr);
			
			if( pgr->bGrdSize )
			{
				pgr->kx = pgr->kx*kx;
				pgr->ky = pgr->ky*ky;
			}

			CalcEnvelope(pGr);
		}
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			GrPointString *pgr = (GrPointString*)(pGr);
			
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


void GrBuffer::ZoomWidth(double k)
{
	Graph *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrPoint(pGr) )
		{
		}
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
		}
		else if( IsGrLineString(pGr) )
		{
			GrLineString *pgr = (GrLineString*)(pGr);
			
			if( pgr->bGrdWid )
				pgr->width *= k;
			
			CalcEnvelope(pGr);
		}
		
		pGr = pGr->next;
	}
}

void GrBuffer::Transform(const double *m)
{
	Graph *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrPoint(pGr) )
		{
			GrPoint *pgr = (GrPoint*)(pGr);
			GraphAPI::TransformPointsWith44Matrix(m,&pgr->pt,1);
		}
		else if( IsGrLineString(pGr) || IsGrPolygon(pGr) || IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			GrVertexList *pList;
			if( IsGrLineString(pGr) )
				pList = &((GrLineString*)pGr)->ptlist;
			else if( IsGrPolygon(pGr) )
				pList = &((GrPolygon*)pGr)->ptlist;
			else if( IsGrPointString(pGr) )
				pList = &((GrPointString*)pGr)->ptlist;
			else if( IsGrPointStringEx(pGr) )
				pList = &((GrPointStringEx*)pGr)->ptlist;	

			GraphAPI::TransformPointsWith44Matrix(m,pList->pts,pList->nuse);
		}
		else if( IsGrText(pGr) )
		{
			GrText *pgr = (GrText*)(pGr);
			GraphAPI::TransformPointsWith44Matrix(m,&pgr->pt,1);
		}
		
		CalcEnvelope(pGr);
		pGr = pGr->next;
	}	
}

void GrBuffer::GetAllGraph(GrBuffer *buf)
{
	if (!buf) return;
//	buf->CopyFrom(this);

	// 获取图元和线型
	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	if (!pCellLib || !pLineLib) return;

	const Graph *pGr = HeadGraph();
	while( pGr )
	{
		if( IsGrPoint(pGr) )
		{
			GrPoint *pPoint = (GrPoint*)pGr;
			if( pPoint && pPoint->cell>0 && pPoint->cell<=pCellLib->GetCellDefCount())
			{
				CellDef cell = pCellLib->GetCellDef(pPoint->cell-1);
				GrBuffer bufSymbol;
				bufSymbol.AddBuffer(cell.m_pgr,0);
				bufSymbol.Zoom(pPoint->kx,pPoint->ky);
				bufSymbol.ZoomPointSize(pPoint->kx,pPoint->ky);
				bufSymbol.Rotate(pPoint->angle);
				
				bufSymbol.Move(pPoint->pt.x,pPoint->pt.y,pPoint->pt.z);
				bufSymbol.SetAllColor(pPoint->color);

				bufSymbol.SetAllLineWidthOrWidthScale(pPoint->bGrdSize,pPoint->width);

				buf->AddBuffer(&bufSymbol);
			}
			else
				buf->InsertGraph(pGr);
		}
		else if( IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
		{
			GrPointString *pString = (GrPointString*)pGr;
			if( pString && pString->cell>0 && pString->cell<=pCellLib->GetCellDefCount())
			{
				GrVertex *pts = pString->ptlist.pts;
				for( int i=0; i<pString->ptlist.nuse; i++,pts++)
				{
					CellDef cell = pCellLib->GetCellDef(pString->cell-1);
					GrBuffer bufSymbol;
					bufSymbol.AddBuffer(cell.m_pgr,0);
					bufSymbol.Zoom(pString->kx,pString->ky);
					bufSymbol.ZoomPointSize(pString->kx,pString->ky);
					bufSymbol.Rotate(pts->CodeToFloat());
					
					bufSymbol.Move(pts->x,pts->y,pts->z);
					bufSymbol.SetAllColor(pString->color);
					
					buf->AddBuffer(&bufSymbol);
				}
			}
			else
				buf->InsertGraph(pGr);
		}
		else if( IsGrLineString(pGr) )
		{
			GrLineString *pLine = (GrLineString*)pGr;
			if (pLine && pLine->style>0 && pLine->style<=pLineLib->GetBaseLineTypeCount())
			{
				BaseLineType linetype = pLineLib->GetBaseLineType(pLine->style-1);

				GrBuffer bufSymbol;

				float hidelen = 0;
				for (int i=0; i<linetype.m_nNum; i++)
				{
					hidelen += (linetype.m_fLens[i]);
				}
				
				// 非隐藏线
				if (hidelen >= 0.2)
				{
					// 实线
					if (linetype.m_nNum <= 1 || fabs(hidelen-linetype.m_fLens[0]) < 0.2)
					{
						GrLineString *pNew = new GrLineString();
						if( pNew )
						{
							pNew->color = pLine->color;
							pNew->evlp = pLine->evlp;
							pNew->width = pLine->width;
							pNew->bGrdWid = pLine->bGrdWid;
							pNew->style = 0;
							pNew->fDrawScale = pLine->fDrawScale;
							pNew->bUseSelfcolor = pLine->bUseSelfcolor;
							pNew->ptlist.CopyFrom(&(pLine->ptlist));
							
							bufSymbol.AddGraph(pNew);
							bufSymbol.CalcEnvelope(pNew);
							
							buf->AddBuffer(&bufSymbol);
							
						}
					}
					else
					{
						double styles[2] = {0};
						for( int i=0; i<2; i++)
						{
							styles[i] = fabs(linetype.m_fLens[i]);
						}
						
						GrVertexList *pList = &pLine->ptlist;
						PT_3D pt0, pt1;
						
						bufSymbol.BeginLineString(pLine->color,pLine->width,pLine->bGrdWid,0,pLine->fDrawScale);
						for (i=1; i<pList->nuse; i+=2 )
						{
							COPY_3DPT(pt0,pList->pts[i-1]);
							COPY_3DPT(pt1,pList->pts[i]);
							
							bufSymbol.Dash(pt0,pt1,styles[0],styles[1]);
						}
						bufSymbol.End();
						
						buf->AddBuffer(&bufSymbol);						
					}
				}
			}
			else
				buf->InsertGraph(pGr);

		}	
		else
			buf->InsertGraph(pGr);
		
		pGr = pGr->next;
	}

	
}

void GrBuffer::KickoffSamePoints()
{
	CTempTolerance t(0,CTempTolerance::flagDistance);

	Graph *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrLineString(pGr) || IsGrPolygon(pGr) || IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
		{
			GrVertexList *pList;
			if( IsGrLineString(pGr) )
				pList = &((GrLineString*)pGr)->ptlist;
			else if( IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
				pList = &((GrPointString*)pGr)->ptlist;
			else
				pList = &((GrPolygon*)pGr)->ptlist;
			
			pList->nuse = GraphAPI::GKickoffSamePoints(pList->pts,pList->nuse);
		}
		
		pGr = pGr->next;
	}
}

void GrBuffer::MergeLineString()
{
	CArray<Graph*,Graph*> arrLines, arrOthers;

	int npt = 0;
	//先挑出线串
	Graph *p = m_pHeadGr;
	while( p )
	{
		if( IsGrLineString(p) )
		{
			arrLines.Add(p);

			npt += ((GrLineString*)p)->ptlist.nuse;
		}
		else
		{
			arrOthers.Add(p);
		}

		p = p->next;
	}

	if( npt<=0 || arrLines.GetSize()<=0 )
		return;

	//取出数据点
	GrVertex *pts = new GrVertex[npt];

	int nSize = arrLines.GetSize(), pos = 0;
	for( int i=0; i<nSize; i++)
	{
		GrLineString *p2 = (GrLineString*)arrLines[i];

		memcpy(pts+pos,p2->ptlist.pts,p2->ptlist.nuse*sizeof(GrVertex));

		pos += p2->ptlist.nuse;
	}

	CTempTolerance t(0,CTempTolerance::flagDistance);

	npt = GraphAPI::GKickoffSamePoints(pts,npt);

	//重新加入线串
	GrLineString *p2 = (GrLineString*)arrLines[0];

	m_pHeadGr = NULL;
	BeginLineString(p2->color,p2->width,p2->bGrdWid,p2->style,p2->fDrawScale,p2->bUseSelfcolor,p2->bUseBackColor);
	Lines(pts,npt);
	End();

	delete[] pts;

	for( i=0; i<nSize; i++)
	{
		ReleaseGraph(arrLines[i]);
	}

	//加入其他数据
	nSize = arrOthers.GetSize();
	for( i=0; i<nSize; i++)
	{
		AddGraph(arrOthers[i]);
	}
}


void GrBuffer::Move(double dx, double dy, double dz)
{
	/*
	double m[16];
	Matrix44FromMove(dx,dy,dz,m);
	Transform(m);
	*/
	Graph *pGr = m_pHeadGr;
	while( pGr )
	{
		if( IsGrPoint(pGr) )
		{
			GrPoint *pgr = (GrPoint*)(pGr);
			pgr->pt.x += dx;
			pgr->pt.y += dy;
			pgr->pt.z += dz;
		}
		else if( IsGrLineString(pGr) || IsGrPolygon(pGr) || IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
		{
			GrVertexList *pList;
			if( IsGrLineString(pGr) )
				pList = &((GrLineString*)pGr)->ptlist;
			else if( IsGrPolygon(pGr) )
				pList = &((GrPolygon*)pGr)->ptlist;
			else if( IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
				pList = &((GrPointString*)pGr)->ptlist;
			
			GrVertex *pts = pList->pts;
			for( int i=0; i<pList->nuse; i++,pts++)
			{
				pts->x += dx;
				pts->y += dy;
				pts->z += dz;
			}
		}
		else if( IsGrText(pGr) )
		{
			GrText *pgr = (GrText*)(pGr);
			pgr->pt.x += dx;
			pgr->pt.y += dy;
			pgr->pt.z += dz;
		}

		CalcEnvelope(pGr);
		pGr = pGr->next;
	}	
}


void GrBuffer::Zoom(double kx, double ky, double kz)
{
	double m[16];
	Matrix44FromZoom(kx,ky,kz,m);
	Transform(m);
}


void GrBuffer::Rotate(double angle, const PT_3D *pt, const PT_3D *dpt)
{
	double m[16];
	PT_3D opt(0,0,0);
	if( !pt )pt = &opt;
	PT_3D dpt2(pt->x,pt->y,pt->z+1.0);
	if( !dpt )dpt = &dpt2;
	
	Matrix44FromRotate(pt,dpt,angle,m);
	Transform(m);
}

void GrBuffer::RotateGrTextPar(double angle)
{
	Graph * pGr = m_pHeadGr;
	while(pGr)
	{
		if( IsGrText(pGr) )
		{
			((GrText*)pGr)->settings.fTextAngle-=angle;
//			CalcEnvelope(pGr);
		}		
		pGr = pGr->next;
	}
}


static void GRotate(float *px, float *py, int sum, float x0, float y0, double angle)
{
	float dx,dy,sina,cosa;
	
	sina = sin(angle);
	cosa = cos(angle);
	
	for (int i=0; i<sum; i++)
	{
		dx = px[i] - x0;
		dy = py[i] - y0;
		
		px[i] = dx * cosa - dy * sina  + x0;
		py[i] = dy * cosa + dx * sina  + y0;
	}
}


void GrBuffer::RotateGrTextEnvelope(double angle)
{
	Graph * pGr = m_pHeadGr;
	while(pGr)
	{
		if( IsGrText(pGr) )
		{
			GRotate(((GrText*)pGr)->x,((GrText*)pGr)->y,4,/*-((GrText*)pGr)->dx*/0,/*-((GrText*)pGr)->dy*/0,angle);
		}		
		pGr = pGr->next;
	}
}


static void GScale(float *px, float *py, int sum, float x0, float y0, double xScale, double yScale)
{	
	for (int i=0; i<sum; i++)
	{
		px[i] = (px[i] - x0)*xScale+x0;
		py[i] = (py[i] - y0)*yScale+y0;	
	}
}

void GrBuffer::ScaleGrText(double lfXscale, double lfYscale)
{
	Graph * pGr = m_pHeadGr;
	while(pGr)
	{
		if( IsGrText(pGr) )
		{	
//			double x = (((GrText*)pGr)->pt.x+((GrText*)pGr)->dx)*lfXscale;
//			double y = (((GrText*)pGr)->pt.y+((GrText*)pGr)->dy)*lfYscale;
//			GScale(((GrText*)pGr)->x,((GrText*)pGr)->y,4,-((GrText*)pGr)->dx,-((GrText*)pGr)->dy,lfXscale,lfYscale);
		}		
		pGr = pGr->next;
	}
}

void GrBuffer::TestGrText()
{
	PT_3D pts[4];
	Graph * pGr = m_pHeadGr;
	while(pGr)
	{
		if( IsGrText(pGr) )
		{		
			pts[0].x = ((GrText*)pGr)->x[0]+ ((GrText*)pGr)->pt.x;
			pts[0].y = ((GrText*)pGr)->y[0]+ ((GrText*)pGr)->pt.y;
			pts[0].z = ((GrText*)pGr)->pt.z;
			pts[1].x = ((GrText*)pGr)->x[1]+ ((GrText*)pGr)->pt.x;
			pts[1].y = ((GrText*)pGr)->y[1]+ ((GrText*)pGr)->pt.y;
			pts[1].z = ((GrText*)pGr)->pt.z;
			pts[2].x = ((GrText*)pGr)->x[2]+ ((GrText*)pGr)->pt.x;
			pts[2].y = ((GrText*)pGr)->y[2]+ ((GrText*)pGr)->pt.y;
			pts[2].z = ((GrText*)pGr)->pt.z;
			pts[3].x = ((GrText*)pGr)->x[3]+ ((GrText*)pGr)->pt.x;
			pts[3].y = ((GrText*)pGr)->y[3]+ ((GrText*)pGr)->pt.y;
			pts[3].z = ((GrText*)pGr)->pt.z;
			BeginLineString(0,0);
			Lines(pts,4);
			End();
// 			pts[0].x = ((GrText*)pGr)->pt.x;
// 			pts[0].y = ((GrText*)pGr)->pt.y;
// 			pts[0].z = ((GrText*)pGr)->pt.z;
// 			pts[1].x =  ((GrText*)pGr)->pt.x+((GrText*)pGr)->dx;
// 			pts[1].y =  ((GrText*)pGr)->pt.y+((GrText*)pGr)->dy;
// 			pts[1].z = ((GrText*)pGr)->pt.z;
// 			BeginLineString(255,0);
// 			Lines(pts,2);
// 			End();


		}		
		pGr = pGr->next;
	}
}


double FindNearestVertex(PT_3D sch_pt, Envelope sch_evlp, const Graph *pGr, CCoordSys *pCS, PT_3D *pRet)
{
	PT_3D pt2;
	double mindis = -1;
	const GrVertex *pts;
	if( IsGrPoint(pGr) )
	{
		GrPoint *pgr = (GrPoint*)(pGr);
		pCS->GroundToClient(&(pgr->pt),&pt2);
		//CONVERT_PT3D(pgr->pt,matrix,pt2);
		mindis = DIST_2DPT(sch_pt,pt2);
		*pRet = pgr->pt;
	}
	else if( IsGrLineString(pGr) || IsGrPolygon(pGr) || IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
	{
		GrVertexList *pList;
		if( IsGrLineString(pGr) )
			pList = &((GrLineString*)pGr)->ptlist;
		else if( IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
			pList = &((GrPointString*)pGr)->ptlist;
		else if( IsGrPolygon(pGr) )
			pList = &((GrPolygon*)pGr)->ptlist;

		pts = pList->pts;
		
		if( pList->nuse==1 )
		{
			pCS->GroundToClient(&(pts[0]),&pt2);
			//CONVERT_PT3D(pts[0],matrix,pt2);
			mindis = DIST_2DPT(sch_pt,pt2);
			*pRet = pts[0];
		}
		else
		{
			double dis = -1;
			for( int i=0; i<pList->nuse; i++,pts++)
			{
				pCS->GroundToClient(&(pts[0]),&pt2);
				//CONVERT_PT3D(pts[0],matrix,pt2);

				if( !sch_evlp.bPtIn(&pt2) )continue;
				
				dis = DIST_2DPT(sch_pt,pt2);
				
				if( mindis<0 || mindis>dis )
				{
					mindis = dis;
					*pRet = pts[0];
					
					if( mindis==0 )goto Find_Over;
				}
			}
		}
	}
	else if( IsGrText(pGr) )
	{
		GrText *pgr = (GrText*)(pGr);
		pCS->GroundToClient(&(pgr->pt),&pt2);
		//CONVERT_PT3D(pgr->pt,matrix,pt2);
		mindis = DIST_2DPT(sch_pt,pt2);
		*pRet = pgr->pt;
	}

Find_Over:
	return mindis;
}

double FindMinimumDistance(PT_3D sch_pt, const Graph *pGr, CCoordSys *pCS, PT_3D *pRet, double *ret_real_mindis)
{
	PT_3D pt1,pt2,line[2];
	double mindis = -1, mindis_real = -1;
	const GrVertex *pts;
	if( IsGrPoint(pGr) )
	{
		GrPoint *pgr = (GrPoint*)(pGr);
		pCS->GroundToClient(&(pgr->pt),&pt2);
//		CONVERT_PT3D(pgr->pt,matrix,pt2);
		mindis_real = mindis = DIST_2DPT(sch_pt,pt2);
		*pRet = pgr->pt;
	}
	else if( IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
	{
		GrPointString *pgr = (GrPointString*)(pGr);	
		pts = pgr->ptlist.pts;
	
		double dis = -1;
		int k = -1;
		for( int i=0; i<pgr->ptlist.nuse; i++,pts++)
		{
			pCS->GroundToClient(pts,&pt2);
			
			dis = DIST_2DPT(sch_pt,pt2);
			
			if( mindis<0 || mindis>dis )
			{
				mindis = dis;
				k = i;
				
				if( mindis==0 )
				{
					break;
				}
			}
		}
		
		if( k>=0 )
		{
			pts = pgr->ptlist.pts+k;
			pt1.x = pts->x;
			pt1.y = pts->y;
			pt1.z = pts->z;
			*pRet = pt1;
		}
		mindis_real = mindis;
	}
	else if( IsGrLineString(pGr) )
	{
		GrLineString *pgr = (GrLineString*)(pGr);	
		pts = pgr->ptlist.pts;
		
		if( pgr->ptlist.nuse==1 )
		{
			pCS->GroundToClient(&pts[0],&pt2);
			//CONVERT_PT3D(pts[0],matrix,pt2);
			mindis = DIST_2DPT(sch_pt,pt2);
			*pRet = pts[0];
		}
		else
		{	
			pCS->GroundToClient(&pts[0],&line[1]);
			//CONVERT_PT3D(pts[0],matrix,line[1]);
			pts++;
			double dis = -1, lfk;
			int k = -1;
			for( int i=1; i<pgr->ptlist.nuse; i++,pts++)
			{
				line[0] = line[1];
				pCS->GroundToClient(&pts[0],&line[1]);
				//CONVERT_PT3D(pts[0],matrix,line[1]);

				if(IsGrPtCodeMoveTo(&pts[0]))
					continue;
				
				dis = GraphAPI::GGetNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
					sch_pt.x,sch_pt.y,&pt2.x,&pt2.y,FALSE);
				
				if( mindis<0 || mindis>dis )
				{
					mindis = dis;
					double r1 = line[1].x-line[0].x;
					double r2 = pt2.x-line[0].x;
					if( r1==0 )lfk = 0;
					else lfk = r2/r1;
					k = i;
					
					if( mindis==0 )
					{
						pts = pgr->ptlist.pts;
						pt1.x = pts[k-1].x + lfk*(pts[k].x-pts[k-1].x);
						pt1.y = pts[k-1].y + lfk*(pts[k].y-pts[k-1].y);
						pt1.z = pts[k-1].z + lfk*(pts[k].z-pts[k-1].z);
						*pRet = pt1;
						
						goto Find_Over;
					}
				}
			}

			if( k>=0 )
			{
				pts = pgr->ptlist.pts;
				pt1.x = pts[k-1].x + lfk*(pts[k].x-pts[k-1].x);
				pt1.y = pts[k-1].y + lfk*(pts[k].y-pts[k-1].y);
				pt1.z = pts[k-1].z + lfk*(pts[k].z-pts[k-1].z);
				*pRet = pt1;
			}
		}
		mindis_real = mindis;
	}
	else if( IsGrPolygon(pGr) )
	{
		GrPolygon *pgr = (GrPolygon*)(pGr);	
		pts = pgr->ptlist.pts;
		PT_3D *pt3ds = new PT_3D[pgr->ptlist.nuse];
		if( pt3ds )
		{
			for( int i=0; i<pgr->ptlist.nuse; i++)
			{
				pCS->GroundToClient(&pts[i],&pt3ds[i]);
				//CONVERT_PT3D(pts[i],matrix,pt3ds[i]);
			}
			
// 			if( GraphAPI::GIsPtInRegion(sch_pt,pt3ds,pgr->ptlist.nuse) )
// 			{
// 				mindis = 0;
// 			}
// 			else
			{
				double dis = -1, lfk;
				int k = -1;
				
				line[1] = pt3ds[0];
				for( i=1; i<pgr->ptlist.nuse; i++)
				{
					line[0] = line[1];
					line[1] = pt3ds[i];
					
					if(IsGrPtCodeMoveTo(&pts[i]))
						continue;
					
					dis = GraphAPI::GGetNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
						sch_pt.x,sch_pt.y,&pt2.x,&pt2.y,FALSE);
					
					if( mindis<0 || mindis<dis )
					{
						mindis = dis;
					}

					if( mindis_real<0 || mindis_real>dis )
					{
						mindis_real = dis;
						double r1 = line[1].x-line[0].x;
						double r2 = pt2.x-line[0].x;
						if( r1==0 )lfk = 0;
						else lfk = r2/r1;
						k = i;
					}				
				}
				
				if( k>=0 )
				{
					pts = pgr->ptlist.pts;
					pt1.x = pts[k-1].x + lfk*(pts[k].x-pts[k-1].x);
					pt1.y = pts[k-1].y + lfk*(pts[k].y-pts[k-1].y);
					pt1.z = pts[k-1].z + lfk*(pts[k].z-pts[k-1].z);
					*pRet = pt1;
				}
			}
			
			delete[] pt3ds;					
		}
	}
	else if( IsGrText(pGr) )
	{
		GrText *pgr = (GrText*)(pGr);
		Envelope e = GetEnvelopeOfGraph(pgr);
		e.TransformGrdToClt(pCS,1);
		//e.Transform(matrix);
		if( e.bPtIn(&sch_pt) )
		{
			mindis = e.Width()<e.Height()?e.Width():e.Height();
		}
		else
		{
			PT_3D  pt((e.m_xh+e.m_xl)/2,(e.m_yh+e.m_yl)/2,(e.m_zh+e.m_zl)/2);
			mindis = DIST_2DPT(sch_pt,pt);
		}
		mindis_real = mindis;
	}

Find_Over:

	if(ret_real_mindis)
	{
		*ret_real_mindis = mindis_real;
	}

	return mindis;
}

BOOL FindNearestLine(PT_3D sch_pt, const Graph *pGr, CCoordSys *pCS, GrVertex *pRet1, GrVertex *pRet2, double *mindis)
{
	if( IsGrPoint(pGr) || IsGrText(pGr) )
	{
		return FALSE;
	}

	PT_3D pt2,line[2];
	*mindis = -1;
	const GrVertex *pts;

	GrVertexList *pList = NULL;
	if( IsGrLineString(pGr) )
		pList = &((GrLineString*)pGr)->ptlist;
	else if( IsGrPolygon(pGr) )
		pList = &((GrPolygon*)pGr)->ptlist;
	else if( IsGrPointString(pGr)|| IsGrPointStringEx(pGr) )
		pList = &((GrPointString*)pGr)->ptlist;
	
	pts = pList->pts;
		
	if( pList->nuse==1 )
	{
		return FALSE;
	}
	else
	{
		pCS->GroundToClient(&pts[0],&line[1]);
//		CONVERT_PT3D(pts[0],matrix,line[1]);
		pts++;
		double dis = -1;
		for( int i=1; i<pList->nuse; i++,pts++)
		{
			line[0] = line[1];
			pCS->GroundToClient(&pts[0],&line[1]);
		//	CONVERT_PT3D(pts[0],matrix,line[1]);
			
			dis = GraphAPI::GGetNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
				sch_pt.x,sch_pt.y,&pt2.x,&pt2.y,FALSE);
			
			if( *mindis<0 || *mindis>dis )
			{
				*mindis = dis;
				*pRet1 = *(pts-1);
				*pRet2 = *(pts);
				
				if( *mindis==0 )
				{
					goto Find_Over;
				}
			}
		}
	}

Find_Over:
	return TRUE;
}


double FindNearestVertex(PT_3D sch_pt, Envelope sch_evlp, const GrBuffer *pGr, CCoordSys* pCS, PT_3D *pRet)
{
	const Graph *pgr = pGr->HeadGraph();
	double mindis = -1, dis;
	PT_3D ptret;
	while( pgr )
	{
		dis = FindNearestVertex(sch_pt,sch_evlp,pgr,pCS,&ptret);
		if( mindis<0 || ( dis>=0 && mindis>dis) )
		{
			mindis = dis;
			*pRet = ptret;
		}
		pgr = pgr->next;
	}

	return mindis;
}


double FindMinimumDistance(PT_3D sch_pt, const GrBuffer *pGr, CCoordSys *pCS, PT_3D *pRet, double *real_mindis)
{
	const Graph *pgr = pGr->HeadGraph();
	double mindis = -1, dis;
	PT_3D ptret;
	while( pgr )
	{
		dis = FindMinimumDistance(sch_pt,pgr,pCS,&ptret,real_mindis);
		if( mindis<0 || ( dis>=0 && mindis>dis) )
		{
			mindis = dis;
			*pRet = ptret;
		}
		pgr = pgr->next;
	}
	
	return mindis;
}



BOOL FindNearestLine(PT_3D sch_pt, const GrBuffer *pGr, CCoordSys *pCS, GrVertex *pRet1, GrVertex *pRet2, double *mindis)
{
	const Graph *pgr = pGr->HeadGraph();
	double dis;
	*mindis = -1;
	GrVertex ptret1, ptret2;
	while( pgr )
	{
		if( FindNearestLine(sch_pt,pgr,pCS,&ptret1,&ptret2,&dis) )
		{
			if( *mindis<0 || ( dis>=0 && *mindis>dis) )
			{
				*mindis = dis;
				*pRet1 = ptret1;
				*pRet2 = ptret2;
			}
		}

		pgr = pgr->next;
	}
	
	return (*mindis>=0);
}



/*
函数功能：贴着已知基线排列图形数据
参数：
linepts, 已知基线的点
lineptnum, 已知基线的点数据
sizeofpt, 点数据的内存字节数
curpt, 基线中用于贴合排列的起始点号，返回值是排列完点串数据后，基线的新的贴合起点号
offlen, 从线串的curpt号的点再往后偏移的长度, 从这个长度之后才开始做贴合排列
返回值是排列完点串数据后，基于新的线串的贴合点号的新的偏移长度
pBuf, 需要排列的图形缓存
uselen, 排列的点集占用的长度
*/

#define IndexPt(i)		(*(PT_3D*)((BYTE*)linepts+sizeofpt*(i)))

int LayoutGrBufferByLines( PT_3D *linepts, int lineptnum, int sizeofpt, 
					 int& curpt, double& offlen,
					 GrBuffer *pBuf, double uselen)
{
	double len1 = offlen+uselen/2, len2 = offlen+uselen, lent=0, lenlast=0;
	
	double xoff=0;
	int nums=-1, nume=-1, ret=0;
	for( int i=curpt; i<lineptnum-1; i++ )
	{
		//依次累计基线的长度
		lent += sqrt((IndexPt(i+1).x-IndexPt(i).x)*(IndexPt(i+1).x-IndexPt(i).x)
			+ (IndexPt(i+1).y-IndexPt(i).y)*(IndexPt(i+1).y-IndexPt(i).y));
		
		//当累计长度刚刚超过贴合点串的中心位置时，当前的点号就作为点串排列的基点，
		//而基于该基点的水平偏移就是总偏移长度减去上次的累计长度
		if( nums<0 && lent>len1 )
		{
			nums = i;
			xoff = offlen-lenlast;
		}
		
		//当累计长度刚刚超过贴合点串的尾端位置时，当前的点号就作为点串排列的终点，
		//而相对该终点的水平偏移也就是深余偏移，
		//就是下一次排列的偏移长度
		if( nume<0 && lent>=len2 )
		{
			nume = i;
			if( nume==curpt )offlen += uselen;
			else offlen = offlen+uselen-lenlast;
		}
		
		if( nums>=0 && nume>=0 )break;
		lenlast = lent;
	}
	
	if( nume<0 )ret = 1; //中心在基线内，尾端在基线外
	if( nums<0 )ret = 2; //中心及尾端都在基线外
	
	//没有找到合适的贴合点，表明字符已经超过了基线所覆盖的长度，
	//那就从当前给定的贴合点开始计算；
	if( nums<0 ){ curpt = lineptnum-1; nums = curpt; xoff = offlen-lent; } 
	if( nume<0 ){ curpt = lineptnum-1; nume = curpt; offlen = offlen+uselen-lent; } 
	
	double angle = 0;
	
	if( lineptnum>1 )
	{
		if( nums>=lineptnum-1 )
			angle = GraphAPI::GGetAngle(IndexPt(lineptnum-2).x,IndexPt(lineptnum-2).y,
			IndexPt(lineptnum-1).x,IndexPt(lineptnum-1).y);
		else
			angle = GraphAPI::GGetAngle(IndexPt(nums).x,IndexPt(nums).y,IndexPt(nums+1).x,IndexPt(nums+1).y);
	}

	pBuf->Move(xoff);
	pBuf->Rotate(angle);

	double tz;
	//线性求出z
	if( nums>=lineptnum-1 )tz = IndexPt(nums).z;
	else
	{
		lent = sqrt((IndexPt(nums+1).x-IndexPt(nums).x)*(IndexPt(nums+1).x-IndexPt(nums).x)
			+ (IndexPt(nums+1).y-IndexPt(nums).y)*(IndexPt(nums+1).y-IndexPt(nums).y));
		if( fabs(lent)<1e-3 )tz = IndexPt(nums).z;
		else tz = IndexPt(nums).z + (IndexPt(nums+1).z-IndexPt(nums).z)*xoff/lent;
	}

	pBuf->Move(IndexPt(nums).x,IndexPt(nums).y,tz);
	
	curpt = nume;
	return ret;
}



Envelope GetEnvelopeOfGraph(const Graph *gr)
{
	if( gr==NULL )return Envelope();
	if( IsGrPoint(gr) )
	{
		const GrPoint *grpt = (const GrPoint*)gr;
		static GrPoint last_pt;
		static Envelope last_evlp;
		if( grpt->cell!=0 && last_pt.cell==grpt->cell && last_pt.angle==grpt->angle && 
			last_pt.kx==grpt->kx && last_pt.ky==grpt->ky )
		{
			double dx = grpt->pt.x-last_pt.pt.x; 
			double dy = grpt->pt.y-last_pt.pt.y;
			Envelope e2 = last_evlp;
			e2.m_xl += dx; e2.m_xh += dx;
			e2.m_yl += dy; e2.m_yh += dy;
			return e2;
		}
		CCellDefLib *pLib = GetCellDefLib();
		PT_3D pt = grpt->pt;

		if( pLib!=NULL && grpt->cell!=0 )
		{
			CellDef item = pLib->GetCellDef(grpt->cell-1);
			if( item.m_pgr!=NULL )
			{
				Envelope e0 = item.m_evlp;
				
				double m1[16],m2[16],m3[16];
				Matrix44FromZoom(grpt->kx,grpt->ky,1,m1);
				Matrix44FromRotate(NULL,grpt->angle,m2);
				matrix_multiply(m2,m1,4,m3);
				Matrix44FromMove(pt.x,pt.y,pt.z,m2);
				matrix_multiply(m2,m3,4,m1);
				
				e0.Transform(m1);

// 				e0.m_xl += pt.x; e0.m_xh += pt.x;
// 				e0.m_yl += pt.y; e0.m_yh += pt.y;
// 				e0.m_zl = pt.z - 0.5; e0.m_zh = pt.z + 0.5;
				
				last_pt = *grpt;
				last_evlp = e0;
				return e0;
			}
		}
		
		float sx = 0.5*grpt->kx;
		float sy = 0.5*grpt->ky;
		if( sx<0 )sx = -sx;
		if( sy<0 )sy = -sy;

		Envelope e;
		e.m_xl = pt.x-sx; e.m_xh = pt.x+sx;
		e.m_yl = pt.y-sy; e.m_yh = pt.y+sy;
		e.m_zl = pt.z-sx; e.m_zh = pt.z+sx;
		return e;
	}
	if( IsGrPointString(gr)|| IsGrPointStringEx(gr) )
	{
		const GrPointString *grpts = (const GrPointString*)gr;
		GrPoint pt = grpts->GetPoint(-1);

		Envelope e1 = GetEnvelopeOfGraph(&pt);
		
		Envelope e = ((GrPointString*)gr)->evlp;
		e.m_xl = e.m_xl+e1.m_xl; e.m_xh = e.m_xh+e1.m_xh;
		e.m_yl = e.m_yl+e1.m_yl; e.m_yh = e.m_yh+e1.m_yh;
		
		return e;
	}
	if( IsGrLineString(gr) )
	{
		float sx = 0;
		if (((GrLineString*)gr)->bGrdWid)
		{
			sx = 0.6*((GrLineString*)gr)->width;
		}		
		if( sx<0 )sx = -sx;
		
		Envelope e = ((GrLineString*)gr)->evlp;
		e.m_xl = e.m_xl-sx; e.m_xh = e.m_xh+sx;
		e.m_yl = e.m_yl-sx; e.m_yh = e.m_yh+sx;
		
		return e;

		//return ((GrLineString*)gr)->evlp;
	}
	if( IsGrPolygon(gr) )
	{
		return ((GrPolygon*)gr)->evlp;
	}
	if( IsGrText(gr) )
	{
		GrText *grtxt = (GrText*)gr;
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
		e.m_zl = grtxt->zmin;
		e.m_zh = grtxt->zmax;
		return e;
	}

	return Envelope();
}

Envelope GetEnvelopeOfGraph_fast(const Graph *gr, float fDrawCellAngle, float fDrawCellKX, float fDrawCellKY, float gscale)
{
	if( gr==NULL )return Envelope();
	if( IsGrPoint(gr) )
	{
		const GrPoint *grpt = (const GrPoint*)gr;
		static GrPoint last_pt;
		static Envelope last_evlp;
		if( grpt->cell!=0 && last_pt.cell==grpt->cell && last_pt.angle==grpt->angle && 
			last_pt.kx==grpt->kx && last_pt.ky==grpt->ky )
		{
			double dx = grpt->pt.x-last_pt.pt.x; 
			double dy = grpt->pt.y-last_pt.pt.y;
			Envelope e2 = last_evlp;
			e2.m_xl += dx; e2.m_xh += dx;
			e2.m_yl += dy; e2.m_yh += dy;
			return e2;
		}
		CCellDefLib *pLib = GetCellDefLib();
		PT_3D pt = grpt->pt;

		if( pLib!=NULL && grpt->cell!=0 )
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
				e0.m_zl = pt.z - 0.5; e0.m_zh = pt.z + 0.5;

				last_pt = *grpt;
				last_evlp = e0;
				return e0;
			}
		}
		
		float sx = 0.5*grpt->kx*fDrawCellKX;
		float sy = 0.5*grpt->ky*fDrawCellKY;

		if( sx<0 )sx = -sx;
		if( sy<0 )sy = -sy;

		Envelope e;
		e.m_xl = pt.x-sx; e.m_xh = pt.x+sx;
		e.m_yl = pt.y-sy; e.m_yh = pt.y+sy;
		e.m_zl = pt.z-sx; e.m_zh = pt.z+sx;
		return e;
	}
	if( IsGrPointString(gr)|| IsGrPointStringEx(gr) )
	{
		const GrPointString *grpts = (const GrPointString*)gr;
		GrPoint pt = grpts->GetPoint(-1);
		
		Envelope e1 = GetEnvelopeOfGraph_fast(&pt,fDrawCellAngle,fDrawCellKX,fDrawCellKY);
		
		Envelope e = ((GrLineString*)gr)->evlp;
		e.m_xl = e.m_xl+e1.m_xl; e.m_xh = e.m_xh+e1.m_xh;
		e.m_yl = e.m_yl+e1.m_yl; e.m_yh = e.m_yh+e1.m_yh;
		
		return e;
	}
	if( IsGrLineString(gr) )
	{
		float sx = 0;
		if (((GrLineString*)gr)->bGrdWid)
		{
			sx = 0.6*((GrLineString*)gr)->width*gscale;
		}
		else
		{
			sx = 0.6*((GrLineString*)gr)->width;
		}
		if( sx<0 )sx = -sx;
		
		Envelope e = ((GrLineString*)gr)->evlp;
		e.m_xl = e.m_xl-sx; e.m_xh = e.m_xh+sx;
		e.m_yl = e.m_yl-sx; e.m_yh = e.m_yh+sx;

		return e;
	}
	if( IsGrPolygon(gr) )
	{
		return ((GrPolygon*)gr)->evlp;
	}
	if( IsGrText(gr) )
	{
		GrText *grtxt = (GrText*)gr;
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
		e.m_zl = grtxt->zmin;
		e.m_zh = grtxt->zmax;
		return e;
// 		GrText *grtxt = (GrText*)gr;
// 		PT_3D pt = grtxt->pt;
// 		pt.x += grtxt->dx * fDrawCellKX;
// 		pt.y += grtxt->dy * fDrawCellKY;
// 		pt.z += grtxt->dz;
// 		float size = grtxt->size * 2 * max(fDrawCellKX,fDrawCellKY);		
// 		Envelope e;
// 
// 		double xh = grtxt->text!=NULL?size*0.5*strlen(grtxt->text):0;
// 
// 		switch(grtxt->align)
// 		{
// 		case TAH_LEFT:
// 		case TAH_LEFT|TAV_TOP:
// 			e.m_xl = pt.x; e.m_xh = pt.x+xh;
// 			e.m_yl = pt.y-size; e.m_yh = pt.y;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_MID:
// 		case TAH_MID|TAV_TOP:
// 			e.m_xl = pt.x-xh/2; e.m_xh = pt.x+xh/2;
// 			e.m_yl = pt.y-size; e.m_yh = pt.y;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_RIGHT:
// 		case TAH_RIGHT|TAV_TOP:
// 			e.m_xl = pt.x-xh; e.m_xh = pt.x;
// 			e.m_yl = pt.y-size; e.m_yh = pt.y;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_LEFT|TAV_MID:
// 		case TAV_MID:
// 			e.m_xl = pt.x; e.m_xh = pt.x+xh;
// 			e.m_yl = pt.y-size/2; e.m_yh = pt.y+size/2;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_MID|TAV_MID:
// 			e.m_xl = pt.x; e.m_xh = pt.x+xh;
// 			e.m_yl = pt.y-size/2; e.m_yh = pt.y+size/2;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_RIGHT|TAV_MID:	
// 			e.m_xl = pt.x-xh; e.m_xh = pt.x;
// 			e.m_yl = pt.y-size/2; e.m_yh = pt.y+size/2;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_LEFT|TAV_BOTTOM:
// 		case TAV_BOTTOM:
// 			e.m_xl = pt.x; e.m_xh = pt.x+xh;
// 			e.m_yl = pt.y; e.m_yh = pt.y+size;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_MID|TAV_BOTTOM:
// 			e.m_xl = pt.x-xh/2; e.m_xh = pt.x+xh/2;
// 			e.m_yl = pt.y; e.m_yh = pt.y+size;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		case TAH_RIGHT|TAV_BOTTOM:
// 			e.m_xl = pt.x-xh; e.m_xh = pt.x;
// 			e.m_yl = pt.y; e.m_yh = pt.y+size;
// 			e.m_zl = pt.z; e.m_zh = pt.z;					
// 		default:
// 			e.m_xl = pt.x; e.m_xh = pt.x+xh;
// 			e.m_yl = pt.y-size; e.m_yh = pt.y;
// 			e.m_zl = pt.z; e.m_zh = pt.z;
// 			break;
// 		}
// 		return e;
	}

	return Envelope();
}

Envelope GetEnvelopeOfPointInString(const Graph *gr)
{
	if( gr==NULL )return Envelope();
	if( IsGrPointString(gr)|| IsGrPointStringEx(gr) )
	{
		const GrPointString *grpts = (const GrPointString*)gr;
		GrPoint pt = grpts->GetPoint(-1);
		
		Envelope e1 = GetEnvelopeOfGraph(&pt);
		
		return e1;
	}
	
	return Envelope();
}

Envelope GetEnvelopeOfPointInString_fast(const Graph *gr, float fDrawCellAngle, float fDrawCellKX, float fDrawCellKY)
{
	if( gr==NULL )return Envelope();
	if( IsGrPointString(gr)|| IsGrPointStringEx(gr) )
	{
		const GrPointString *grpts = (const GrPointString*)gr;
		GrPoint pt = grpts->GetPoint(-1);
		
		Envelope e1 = GetEnvelopeOfGraph_fast(&pt,fDrawCellAngle,fDrawCellKX,fDrawCellKY);
		
		return e1;
	}

	return Envelope();
}

MyNameSpaceEnd