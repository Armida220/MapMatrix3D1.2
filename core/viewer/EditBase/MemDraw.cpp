// Drawing.cpp: implementation of the CMemDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemDraw.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define polygon_GetN(p)			p->npt
#define polygon_RefNth(p,i)		(p->pts+i)
#define point_GetX(pt)			(pt->x)
#define point_GetY(pt)			(pt->y)
#define ABS(x)					((x)>0?(x):-(x))
#define _FABS(x)				((x)>0?(x):(-x))
#define ras_TestPointModulo(pat, x, curY)	FALSE
#define ras_Point(r, x, curY)
#define SGN(x)					(x>0?1:-1)
#define SQRT(x,y)				(_FABS(x)>=_FABS(y)?(_FABS(x)+0.414*_FABS(y)):(_FABS(x)*0.414+_FABS(y)))

#define gnMemWid				(gthis->m_nMemWid)
#define gnMemHei				(gthis->m_nMemHei)
#define gpMemBits				(gthis->m_pMemBits)
#define gColor					(gthis->m_color)

#define GRIDDIS_PITCH			1448 //1.414*1024
#define GRIDDIS_DIRECT			1024 //1.0*1024	
				

MyNameSpaceBegin

CMemDraw *gthis = NULL;

inline void ScanLine(int x1, int x2, int y)
{
	if(x1==x2)
	{
		int a=1;
	}
	int x11=x1,x22=x2,y2=y;
	if( x11<0 )x11 = 0;
	if( x22>=gnMemWid )x22 = gnMemWid-1;
	if( x11<=x22 && y>=0 && y<gnMemHei )
	{
		BYTE *p = (BYTE*)gpMemBits+gnMemWid*(gnMemHei-1-y)+x11;
		memset(p,gColor,x22-x11+1);
	}
}

inline void SetPixel(int x, int y, COLORREF clr)
{
	if( x>=0 && x<gnMemWid && y>=0 && y<gnMemHei )
	{
		BYTE *p = (BYTE*)gpMemBits+gnMemWid*(gnMemHei-1-y)+x;
		*p = clr;
	}
}

inline void SetPixel0(int x, int y)
{
	if( x>=0 && x<gnMemWid && y>=0 && y<gnMemHei )
	{
		BYTE *p = (BYTE*)gpMemBits+gnMemWid*(gnMemHei-1-y)+x;
		*p = gColor;
	}
}


#define drawpixel_to_buf(x,y) {buf[i++]=x; buf[i++]=y;}

//中点画圆(1/8圆弧)的算法
static int Get8thCircleBuf(int r, int *buf)
{
	int i=0,x,y,d;
	x=0; y=r; d=1-r;	
	while(x<y)
	{
		drawpixel_to_buf(x,y);
		if(d<0)
		{ 
			d += 2*x+3; 
			x++; 
		}
		else
		{
			d += 2*(x-y) + 5; 
			x++;
			y--; 			
		}
	}
	if( x==y )drawpixel_to_buf(x,y);
	return i;
}

// Bresenham 画线算法
// add_pt: 0, not add any auxiliary point; 1, add an auxiliary point before turn; 
// 2, add an auxiliary point after turn;
static int GetLineBuf(int x1, int y1, int x2, int y2, int *buf, int add_pt)
{
	int dx,dy,x,y,p;
	int const1,const2;
	int inc,tmp,i=0;
	
	dx=x2-x1; dy=y2-y1;
	
	if( (dx>=0&&dy>=0) || (dx<=0&&dy<=0) ) /*准备x或y的单位递变值。*/
		inc=1;
	else
		inc=-1;
	
	if(_FABS(dx)>_FABS(dy))
	{
		if(dx<0)
		{			
			tmp=x1; /*将2a, 3a象限方向*/
			x1=x2; /*的直线变换到1a, 4a*/
			x2=tmp;
			tmp=y1; /*象限方向去*/
			y1=y2;
			y2=tmp;
			dx=-dx;
			dy=-dy;
		}
		
		p=-_FABS(dx);
		const1=(_FABS(dy)<<1); /*注意此时误差的*/
		const2=(_FABS(dx)<<1); /*变化参数取值. */
		
		x=x1; y=y1;
		buf[i++] = x; buf[i++] = y; 
		while(x<x2)
		{
			x++;
			p+=const1;
			if (p>=0)
			{
				if( add_pt==1 ){buf[i++] = x; buf[i++] = y; }
				y+=inc;
				p-=const2;
				if( add_pt==2 ){buf[i++] = x-1; buf[i++] = y; }
			}
			buf[i++] = x; buf[i++] = y; 
		}
	}
	else 
	{
		if (dy<0)
		{
			tmp=x1; /* 将3b, 4b象限方向的*/
			x1=x2; /*直线变换到2b, 1b */
			x2=tmp; /*象限方向去. */
			tmp=y1;
			y1=y2;
			y2=tmp;
			dx=-dx;
			dy=-dy;
		}
		
		p=-_FABS(dy); /*注意此时误差的*/
		const1=(_FABS(dx)<<1); /*变化参数取值. */
		const2=(_FABS(dy)<<1);
		
		x=x1;	y=y1;
		buf[i++] = x; buf[i++] = y; 
		
		while (y<y2)
		{
			y++;
			p+=const1;
			if (p>=0)
			{
				if( add_pt==1 ){buf[i++] = x; buf[i++] = y; }
				x+=inc;
				p-=const2;
				if( add_pt==2 ){buf[i++] = x; buf[i++] = y-1; }
			}
			buf[i++] = x; buf[i++] = y; 
		}
	}
	
	return i;
}

//////////////////////////////////////////////////////////////////////
// CPolyFill 
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolyFill::CPolyFill()
{
	m_rcClip.left = 0; m_rcClip.right = 1024;
	m_rcClip.top = 0; m_rcClip.bottom = 1024;

	m_nListUse = 0;
	m_nListLen = 0;
	m_list = NULL;
}

CPolyFill::~CPolyFill()
{
	if( m_list )delete[] m_list;
}

BOOL CPolyFill::ReAllocListMem()
{
	if( m_nListUse>=m_nListLen )
	{
		MEdgeRecord *pnew = new MEdgeRecord[m_nListLen+1024];
		if( !pnew )return FALSE;
		memset( pnew,0,sizeof(MEdgeRecord)*(m_nListLen+1024));
		
		if( m_nListUse )memcpy(pnew,m_list,m_nListUse*sizeof(MEdgeRecord));
		if( m_list )delete []m_list;
		m_list = pnew;
		m_nListLen = m_nListLen+1024;
	}
	
	return TRUE;
}


void CPolyFill::AddEdgeRecord(MEdge *e, int yPos)
{
	if( m_nListUse==0 )
	{	
		if( !ReAllocListMem() )
			return;
		
		m_list[0].e = e;
		m_list[0].pos = yPos;
		m_nListUse = 1;
		return;
	}

	int i=m_nListUse>>1, ns=0, ne=m_nListUse-1;
	while( i!=ns && i!=ne )
	{
		if( yPos>m_list[i].pos )
			ns = i;
		else if( yPos<m_list[i].pos )
			ne = i;
		else break;
		i = (ns+ne)>>1;
	}

	if( m_list[i].pos==yPos )
	{
		m_list[i].e = e;
		m_list[i].pos = yPos;
	}
	else if( yPos==m_list[ns].pos )
	{
		m_list[ns].e = e;
		m_list[ns].pos = yPos;
	}
	else if( yPos==m_list[ne].pos )
	{
		m_list[ne].e = e;
		m_list[ne].pos = yPos;
	}
	else 
	{
		if( yPos<m_list[ns].pos )
			i = ns;
		else if( yPos>m_list[ne].pos )
			i = ne+1;
		else
			i = ne;

		if( m_nListUse>=m_nListLen )
		{
			if( !ReAllocListMem() )return;
		}

		if( i>=m_nListUse )
		{
			m_list[m_nListUse].e = e;
			m_list[m_nListUse].pos = yPos;
			m_nListUse++;
		}
		else
		{
			memmove(m_list+i+1,m_list+i,sizeof(MEdgeRecord)*(m_nListUse-i));
			m_list[i].e = e;
			m_list[i].pos = yPos;
			m_nListUse++;
		}
	}
}

MEdge *CPolyFill::GetEdge(int yPos)
{
	if( m_nListUse<=0 )return NULL;

	int i=m_nListUse>>1, ns=0, ne=m_nListUse-1;
	while( i!=ns && i!=ne )
	{
		if( yPos>m_list[i].pos )
			ns = i;
		else if( yPos<m_list[i].pos )
			ne = i;
		else break;
		i = (ns+ne)>>1;
	}
	
	if( m_list[i].pos==yPos )
		return m_list[i].e;
	else if( yPos==m_list[ns].pos )
		return m_list[ns].e;
	else if( yPos==m_list[ne].pos )
		return m_list[ne].e;

	return NULL;
}

MEdge *CPolyFill::NewEdge()
{
	MEdge *pNew = new MEdge;
	if( !pNew )return NULL;
	
	memset(pNew, 0, sizeof(MEdge));

	return pNew;
}


void CPolyFill::FreeEdge(MEdge *p)
{
	delete p;
}


void CPolyFill::FillEdges(POLYGON *p)
{
    int i, j, n = polygon_GetN(p);

    for (i = 0; i < n; i++) 
	{
        POINT *p1, *p2, *p3;
        MEdge *e;
        p1 = polygon_RefNth(p, i);
        p2 = polygon_RefNth(p, (i + 1) % n);
        if (point_GetY(p1) == point_GetY(p2))
            continue;   /* Skip horiz. edges */

        /* Find next vertex not level with p2 */
        for (j = (i + 2) % n; ; j = (j + 1) % n) 
		{
            p3 = polygon_RefNth(p, j);
            if (point_GetY(p2) != point_GetY(p3))
                break;
        }
        e = NewEdge();
        e->xNowNumStep = ABS(point_GetX(p1) - point_GetX(p2));
        if (point_GetY(p2) > point_GetY(p1)) 
		{
            e->yTop = point_GetY(p1);
            e->yBot = point_GetY(p2);
            e->xNowWhole = point_GetX(p1);
			e->xEndWhole = point_GetX(p2);
            e->xNowDir = SGN(point_GetX(p2) - point_GetX(p1));
            e->xNowDen = e->yBot - e->yTop;
            e->xNowNum = (e->xNowDen >> 1);
			/* Record yBot only one time */
            if (point_GetY(p3) > point_GetY(p2))
                e->yBot--;
        } 
		else 
		{
            e->yTop = point_GetY(p2);
            e->yBot = point_GetY(p1);
            e->xNowWhole = point_GetX(p2);
			e->xEndWhole = point_GetX(p1);
            e->xNowDir = SGN(point_GetX(p1) - point_GetX(p2));
            e->xNowDen = e->yBot - e->yTop;
            e->xNowNum = (e->xNowDen >> 1);
            if (point_GetY(p3) < point_GetY(p2)) 
			{
				/* Record yTop only one time */
                e->yTop++;
				/* Trace this edge by one step */
                e->xNowNum += e->xNowNumStep;
                while (e->xNowNum >= e->xNowDen) 
				{
                    e->xNowWhole += e->xNowDir;
                    e->xNowNum -= e->xNowDen;
                }
            }
        }
        e->next = GetEdge(e->yTop);
        AddEdgeRecord(e,e->yTop);
    }
}

/*
 * UpdateActive first removes any edges which curY has entirely
 * passed by.  The removed edges are freed.
 * It then removes any edges from the MEdge table at curY and
 * places them on the active list.
 */

MEdge *CPolyFill::UpdateActive(MEdge *active, long curY)
{
    MEdge *e, **ep;
    for (ep = &active, e = *ep; e != NULL; e = *ep)
	{
        if (e->yBot < curY) 
		{
            *ep = e->next;
            FreeEdge(e);
        } 
		else
            ep = &e->next;
	}

	/* add the edge at the current pos to the active edge list*/
    *ep = GetEdge(curY);

    return active;
}

/*
 * DrawRuns first uses an insertion sort to order the X
 * coordinates of each active MEdge.  It updates the X coordinates
 * for each MEdge as it does this.
 * Then it draws a run between each pair of coordinates,
 * using the specified fill pattern.
 *
 * This routine is very slow and it would not be that
 * difficult to speed it way up.
 */

void CPolyFill::DrawRuns(RAS_BUF *r, MEdge *active, long curY, RAS_BUF *pat)
{
    MEdge *e;
    static long xCoords[1024];
    long numCoords = 0;
    int i;
    for (e = active; e != NULL; e = e->next) 
	{
		/* get the insert pos and move back data to empty the place */
        for (i = numCoords; i > 0 && xCoords[i-1]>e->xNowWhole; i--)
            xCoords[i] = xCoords[i - 1];
		/* insert the x coord of this edge */
        xCoords[i] = e->xNowWhole;
        numCoords++;
		/* trace this edge by one step */
        e->xNowNum += e->xNowNumStep;
        while (e->xNowNum >= e->xNowDen) 
		{
            e->xNowWhole += e->xNowDir;
            e->xNowNum -= e->xNowDen;
			if( (e->xNowDir>0 && e->xNowWhole<=e->xEndWhole) || 
				(e->xNowDir<0 && e->xNowWhole>=e->xEndWhole) )
				SetPixel0(e->xNowWhole,curY);
        }
    }
    if (numCoords % 2)  /* Protect from degenerate polygons */
        xCoords[numCoords] = xCoords[numCoords - 1],
        numCoords++;
    for (i = 0; i < numCoords; i += 2) 
	{
        /* Here's the graphics-dependent part. */
        /* All we need is to draw a horizontal line */
        /* from (xCoords[i], curY) to (xCoords[i + 1], curY). */
        /* Example: I want to fill the polygon with a pattern. */
        /* (This example is very slow because it's done point by */
        /* point, thus not taking advantage of the potential for */
        /* speed afforded by the scan conversion...) */
		ScanLine(xCoords[i],xCoords[i+1],curY);
		/*
        for (x = xCoords[i]; x <= xCoords[i + 1]; x++)
            if (ras_TestPointModulo(pat, x, curY))
                ras_Point(r, x, curY);
				*/
    }
}


MEdge *CPolyFill::JumpToY(int y)
{
	MEdgeRecord *p = m_list;
	MEdge *e = NULL, *cur, *active = NULL, **ep;
	int j;
	for( j=0; j<m_nListUse; j++,p++)
	{
		if( p->pos>=y )break;

		cur = p->e;
		
		/* remove edges which has passed by, similar to 'UpdateActive' */
		for (ep = &cur, e = *ep; e != NULL; e = *ep)
		{
			if (e->yBot < y) 
			{
				*ep = e->next;
				FreeEdge(e);
			} 
			else
				ep = &e->next;
		}

		if( cur==NULL )continue;

		/* trace edges until their y coord equal to y, similar to 'DrawRuns' */
		for( e=cur; e!=NULL; e=e->next )
		{
			/* skip to y directly, instead of using for/while loops */
			e->xNowNum = e->xNowNum + (y-e->yTop)*e->xNowNumStep;
			e->xNowWhole = e->xNowWhole + e->xNowDir*e->xNowNum/e->xNowDen;
			e->xNowNum = e->xNowNum%e->xNowDen;
			/*
			for( yt=e->yTop; yt<y; yt++ )
			{
				e->xNowNum += e->xNowNumStep;
				while (e->xNowNum >= e->xNowDen) 
				{
					e->xNowWhole += e->xNowDir;
					e->xNowNum -= e->xNowDen;
				}
			}*/
		}
		
		/* add to the active list */
		if( active )
		{
			/* find if it has already existed */
			for( e=active; e->next!=NULL; e=e->next)
			{
				if( e->next==cur )break;
			}			
			if( active==cur || e->next==cur )
				continue;

			/* adding */
			e->next = cur;
		}
		else active = cur;
	}
	
	return active;
}

void CPolyFill::FillPolygon(POINT *pts, int npt)
{
    MEdge *active;
    long curY;
	
	/* fill edges */
	POLYGON poly;
	poly.pts = pts; poly.npt = npt;
    FillEdges(&poly);

	if( m_nListUse<=0 )return;

	/* clip edges above m_rcClip.top */
	active = NULL;
	curY = m_list[0].pos;
	if( curY<m_rcClip.top )
	{
		active = JumpToY(m_rcClip.top);
		curY = m_rcClip.top;
	}
	
	/* drawing */
	for (; (active=UpdateActive(active,curY))!=NULL; curY++)
	{
		if( curY>=m_rcClip.bottom )break;
		DrawRuns(0, active, curY, 0);
	}

	/* free the rest */
	if( curY>=m_rcClip.bottom )
	{
		MEdge *e, *t;

		/* free the active edges */
		{
			e=active;
			while( e!=NULL )
			{
				t = e->next;
				FreeEdge(e);
				e = t;
			}
		}

		/* free non-active edges */
		for( int i=0; i<m_nListUse; i++)
		{
			if( m_list[i].e && m_list[i].pos>curY )
			{
				e=m_list[i].e;
				while( e!=NULL )
				{
					t = e->next;
					FreeEdge(e);
					e = t;
				}
			}
		}
	}

	m_nListUse = 0;
}

void CPolyFill::FillMultiPolygon(POINT *pts, int *npt, int num)
{
    MEdge *active;
    long curY;

	/* fill edges */
	int i;
	POLYGON poly;
	POINT *pts2 = pts;
    for( i=0; i<num; i++)
	{
		poly.pts = pts2; poly.npt = npt[i];
		FillEdges(&poly);
		pts2 += poly.npt;
	}
	if( m_nListUse<=0 )return;
	
	/* clip edges above m_rcClip.top */
	active = NULL;
	curY = m_list[0].pos;
	if( curY<m_rcClip.top )
	{
		active = JumpToY(m_rcClip.top);
		curY = m_rcClip.top;
	}
	
	/* A polygon may be away entirely from another one, which leads that */
	/* active is NULL but the isolated polygon may not be filled yet. */
	/* so, only it should be to check curY<m_rcClip.bottom in the loop. */
	for (; curY<m_rcClip.bottom; curY++)
	{
		active = UpdateActive(active,curY);
		if( active )DrawRuns(0, active, curY, 0);
	}
	
	/* free the rest */
	if( curY>=m_rcClip.bottom )
	{
		MEdge *e, *t;
		
		/* free the active edges */
		{
			e=active;
			while( e!=NULL )
			{
				t = e->next;
				FreeEdge(e);
				e = t;
			}
		}
		
		/* free non-active edges */
		for( int i=0; i<m_nListUse; i++)
		{
			if( m_list[i].e && m_list[i].pos>curY )
			{
				e=m_list[i].e;
				while( e!=NULL )
				{
					t = e->next;
					FreeEdge(e);
					e = t;
				}
			}
		}
	}
	
	m_nListUse = 0;
}

//////////////////////////////////////////////////////////////////////
// CMemDraw
//////////////////////////////////////////////////////////////////////
//
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemDraw::CMemDraw()
{
	m_pMemBits = 0;;
	m_nMemWid = 0;
	m_nMemHei = 0;
	
	m_color = 0;
	m_width = 0;
	m_ptCurrent.x = 0;
	m_ptCurrent.y = 0;
	m_bRoundLine = FALSE;
}

CMemDraw::~CMemDraw()
{

}

void CMemDraw::Init8BitMem(BYTE *pBuf, int wid, int hei)
{
	m_pMemBits = pBuf;
	m_nMemWid = wid; m_nMemHei = hei;
	m_poly.m_rcClip.right = wid;
	m_poly.m_rcClip.bottom = hei;
}

void CMemDraw::Point(int x, int y, BYTE clr, int size)
{
	int i,*buf = m_buf;
	int num = Get8thCircleBuf(size,buf);
	m_color = clr;

	gthis = this;
	
	for( i=0; i<num; i+=2)
	{
		if( i>num-4 || (i<=num-4 && buf[i+1]!=buf[i+3]) )
		{
			ScanLine(x-buf[i],x+buf[i],y+buf[i+1]);
			ScanLine(x-buf[i],x+buf[i],y-buf[i+1]);
		}		
		{
			ScanLine(x-buf[i+1],x+buf[i+1],y+buf[i]);
			ScanLine(x-buf[i+1],x+buf[i+1],y-buf[i]);
		}
	}
}

void CMemDraw::SetLineStyle(BYTE clr,int wid, BOOL bRound)
{
	m_color = clr;
	m_width = wid;
	m_nStyleSeg = 0;
	m_bRoundLine = bRound;
}

void CMemDraw::SetLineStyleEx(int wid, int nseg, long *seg, BOOL bRound)
{
	if( nseg>4 )nseg = 4;
	m_nStyleSeg = nseg;
	m_width = wid;
	m_bRoundLine = bRound;
	memcpy(m_linestyle,seg,sizeof(long)*(nseg<<1));

	m_nCurSeg = 0;
	m_nNextLen = (m_linestyle[1]<<GRIDDIS_MUL);
	m_nCurLen = 0;
	m_color = m_linestyle[0];
}


void CMemDraw::LineTo(int x, int y)
{
	if( x!=m_ptCurrent.x || y!=m_ptCurrent.y )
	{
		if( m_bRoundLine )
			Point(m_ptCurrent.x,m_ptCurrent.y,m_color,m_width/2);

		if( m_nStyleSeg>0 )
			LineEx(m_ptCurrent.x,m_ptCurrent.y,x,y,m_width);
		else Line(m_ptCurrent.x,m_ptCurrent.y,x,y,m_width);

		m_ptCurrent.x = x;
		m_ptCurrent.y = y;
	}
}


//沿绘线方向作垂线，取长度为wid的线段，该所覆盖的点集作为宽度的填充点集
void CMemDraw::GetLineWidthBuf(int x1, int y1, int x2, int y2, int wid,
					 int &nbuf, int *&buf, int &nbuf2, int *&buf2)
{
	int i,dx,dy,rx,ry;

	buf = m_buf;
	buf2 = m_buf+TMPBUF_HALF;
	nbuf = nbuf2 = 0;

	//垂线段的方向
	dx=y1-y2; dy=x2-x1;
	if( dx==0 && dy==0 )
	{
		for( i=0; i<wid; i++)
		{
			buf[(i<<1)] = 0;
			buf[(i<<1)+1] = i-(wid>>1);
		}
		nbuf = 2*wid;
		
		for( i=0; i<wid; i++)
		{
			buf2[(i<<1)] = 0;
			buf2[(i<<1)+1] = i-(wid>>1);
		}
		nbuf2 = 2*wid;
	}
	else
	{
		//获得垂线段的点集
		double r = SQRT(dx,dy);
		//double r = sqrt(dx*dx+dy*dy);
		rx = (int)(wid*0.5*dx/r+(dx>0?0.5:-0.5)), ry = (int)(wid*0.5*dy/r+(dy>0?0.5:-0.5));
		int add_pt = 0, d;
		
		if( _FABS(dy)>_FABS(dx) )// <==> _FABS(x2-x1)>_FABS(y2-y1)
		{
			if( x2>x1 )
			{
				if( y2>y1 )add_pt = 2;
				else add_pt = 1;
			}
			else
			{
				if( y2>y1 )add_pt = 2;
				else add_pt = 1;
			}
		}
		else
		{
			if( y2>y1 )
			{
				if( x2>x1 )add_pt = 2;
				else add_pt = 1;
			}
			else
			{
				if( x2>x1 )add_pt = 2;
				else add_pt = 1;
			}
		}
		
		if( _FABS(rx)>=_FABS(ry) )
		{
			//判断是否由于取整导致了刷子的直径变大并作修正
			d = 1+(_FABS(rx)<<1)-wid+_FABS(ry)*0.414+0.5;
			if( d>=2 ){ rx -= (dx>0?1:-1); d-=2; }
			if( d>=2 ){ ry -= (dy>0?1:-1); d-=2; }
			if( d<0 )d = 0;
			
			//修正相等导致的问题
			if( rx==ry )
			{
				if( _FABS(dy)>_FABS(dx) )add_pt = dx>0?1:2;
				else add_pt = dy>0?1:2;
			}
			
			if( _FABS(rx)==_FABS(ry) )
				nbuf = GetLineBuf(-rx,-ry,rx-(dx>0?d:-d),ry-(dy>0?d:-d),buf,add_pt);
			else
				nbuf = GetLineBuf(-rx,-ry,rx-(dx>0?d:-d),ry,buf,add_pt);
			
			if( _FABS(rx)==_FABS(ry) )
				nbuf2 = GetLineBuf(-rx,-ry,rx-(dx>0?d:-d),ry-(dy>0?d:-d),buf2,0);
			else
				nbuf2 = GetLineBuf(-rx,-ry,rx-(dx>0?d:-d),ry,buf2,0);
		}
		else
		{
			d = 1+(_FABS(ry)<<1)-wid+_FABS(rx)*0.414+0.5;
			if( d>=2 ){ ry -= (dy>0?1:-1); d-=2; }
			if( d>=2 ){ rx -= (dx>0?1:-1); d-=2; }
			if( d<0 )d = 0;
			
			nbuf = GetLineBuf(-rx,-ry,rx,ry-(dy>0?d:-d),buf,add_pt);
			nbuf2 = GetLineBuf(-rx,-ry,rx,ry-(dy>0?d:-d),buf2,0);
		}
	}
}

void CMemDraw::Line(int x1, int y1, int x2, int y2, int wid)
{
	int dx,dy,x,y,p;
	int const1,const2;
	int inc,i;
	int *buf, *buf2, nbuf, nbuf2;

	int xl=0, xr=m_nMemWid, yt=0, yb=m_nMemHei;

	gthis = this;

	xl -= (wid>>1);  xr += (wid>>1);
	yt -= (wid>>1);  yb += (wid>>1);
		
	if( wid>1 )
	{
		GetLineWidthBuf(x1,y1,x2,y2,wid,nbuf,buf,nbuf2,buf2);
	}
	
	dx=x2-x1; dy=y2-y1;
	
	if( (dx>=0&&dy>=0) || (dx<=0&&dy<=0) ) /*准备x或y的单位递变值。*/
		inc=1;
	else
		inc=-1;

	int add = 1;
	
	if(_FABS(dx)>_FABS(dy))
	{
		inc = dy>0?1:-1;
		if(dx>0)
		{
			p=-_FABS(dx);
			const1=(_FABS(dy)<<1); /*注意此时误差的*/
			const2=(_FABS(dx)<<1); /*变化参数取值. */
			
			x=x1; y=y1;
			
			//裁剪到可视区域
			if( x<xl && const2!=0 )
			{
				i = (p+(xl-x)*const1+const2-1)/const2;
				y = y1+inc*i;
				p = p+(xl-x)*const1-const2*i;
				x = xl;
			}
			
			if( x2>xr )x2 = xr;
			
			if( x<x2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while(x<x2)
			{
				x++;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					y+=inc;
					p-=const2;
					add = 1;
				}
				
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
		else if(dx<0)
		{		
			p=-_FABS(dx);
			const1=(_FABS(dy)<<1); /*注意此时误差的*/
			const2=(_FABS(dx)<<1); /*变化参数取值. */
			
			x=x1; y=y1;

			//裁剪到可视区域
			if( x>xr && const2!=0 )
			{
				i = (p+(x-xr)*const1+const2-1)/const2;
				y = y1+inc*i;
				p = p+(x-xr)*const1-const2*i;
				x = xr;
			}
			
			if( x2<xl )x2 = xl;
			
			if( x>x2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while(x>x2)
			{
				x--;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					y+=inc;
					p-=const2;
					add = 1;
				}

				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
	}
	else 
	{
		inc = dx>0?1:-1;
		if(dy>0)
		{
			p=-_FABS(dy); /*注意此时误差的*/
			const1=(_FABS(dx)<<1); /*变化参数取值. */
			const2=(_FABS(dy)<<1);
			
			x=x1;	y=y1;
			
			//裁剪到可视区域
			if( y<yt && const2!=0 )
			{
				i = (p+(yt-y)*const1+const2-1)/const2;
				x = x1+inc*i;
				p = p+(yt-y)*const1-const2*i;
				y = yt;
			}
			
			if( y2>yb )y2 = yb;
			
			if( y<y2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while (y<y2)
			{
				y++;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					x+=inc;
					p-=const2;
					add = 1;
				}
				
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
		else if (dy<0)
		{
			p=-_FABS(dy); /*注意此时误差的*/
			const1=(_FABS(dx)<<1); /*变化参数取值. */
			const2=(_FABS(dy)<<1);
			
			x=x1;	y=y1;

			//裁剪到可视区域
			if( y>yb && const2!=0 )
			{
				i = (p+(y-yb)*const1+const2-1)/const2;
				x = x1+inc*i;
				p = p+(y-yb)*const1-const2*i;
				y = yb;
			}

			if( y2<yt )y2 = yt;

			if( y>y2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while (y>y2)
			{
				y--;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					x+=inc;
					p-=const2;
					add = 1;
				}

				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
	}
}


void CMemDraw::LineEx(int x1, int y1, int x2, int y2, int wid)
{
	int dx,dy,x,y,p;
	int const1,const2;
	int inc, i;
	int *buf, *buf2, nbuf, nbuf2;

	int xl=0, xr=m_nMemWid, yt=0, yb=m_nMemHei;

	gthis = this;

	xl -= (wid>>1);  xr += (wid>>1);
	yt -= (wid>>1);  yb += (wid>>1);
	xl = INT_MIN;  xr = INT_MAX;
	yt = INT_MIN;  yb = INT_MAX;
		
	if( wid>1 )
	{
		GetLineWidthBuf(x1,y1,x2,y2,wid,nbuf,buf,nbuf2,buf2);
	}
	
	dx=x2-x1; dy=y2-y1;

	int add = 1;
	
	if(_FABS(dx)>_FABS(dy))
	{
		inc = dy>0?1:-1;
		if(dx>0)
		{
			p=-_FABS(dx);
			const1=(_FABS(dy)<<1); /*注意此时误差的*/
			const2=(_FABS(dx)<<1); /*变化参数取值. */			
			
			x=x1; y=y1;
			
			//裁剪到可视区域
			if( x<xl && const2!=0 )
			{
				i = (p+(xl-x)*const1+const2-1)/const2;
				y = y1+inc*i;
				p = p+(xl-x)*const1-const2*i;
				x = xl;
			}
			
			if( x2>xr )x2 = xr;
			
			if( x<x2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while(x<x2)
			{
				x++;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					y+=inc;
					p-=const2;
					add = 1;
				}
				
				if( add )StepPixel(GRIDDIS_PITCH);
				else StepPixel(GRIDDIS_DIRECT);
				
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
		else if(dx<0)
		{
			p=-_FABS(dx);
			const1=(_FABS(dy)<<1); /*注意此时误差的*/
			const2=(_FABS(dx)<<1); /*变化参数取值. */
			
			x=x1; y=y1;

			//裁剪到可视区域
			if( x>xr && const2!=0 )
			{
				i = (p+(x-xr)*const1+const2-1)/const2;
				y = y1+inc*i;
				p = p+(x-xr)*const1-const2*i;
				x = xr;
			}

			if( x2<xl )x2 = xl;

			if( x>x2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while(x>x2)
			{
				x--;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					y+=inc;
					p-=const2;
					add = 1;
				}

				if( add )StepPixel(GRIDDIS_PITCH);
				else StepPixel(GRIDDIS_DIRECT);

				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
	}
	else 
	{
		inc = dx>0?1:-1;
		if(dy>0)
		{
			p=-_FABS(dy); /*注意此时误差的*/
			const1=(_FABS(dx)<<1); /*变化参数取值. */
			const2=(_FABS(dy)<<1);
			
			x=x1;	y=y1;
			
			//裁剪到可视区域
			if( y<yt && const2!=0 )
			{
				i = (p+(yt-y)*const1+const2-1)/const2;
				x = x1+inc*i;
				p = p+(yt-y)*const1-const2*i;
				y = yt;
			}
			
			if( y2>yb )y2 = yb;
			
			if( y<y2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while (y<y2)
			{
				y++;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					x+=inc;
					p-=const2;
					add = 1;
				}
				
				if( add )StepPixel(GRIDDIS_PITCH);
				else StepPixel(GRIDDIS_DIRECT);
				
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
		else if(dy<0)
		{		
			p=-_FABS(dy); /*注意此时误差的*/
			const1=(_FABS(dx)<<1); /*变化参数取值. */
			const2=(_FABS(dy)<<1);
			
			x=x1;	y=y1;

			//裁剪到可视区域
			if( y>yb && const2!=0 )
			{
				i = (p+(y-yb)*const1+const2-1)/const2;
				x = x1+inc*i;
				p = p+(y-yb)*const1-const2*i;
				y = yb;
			}

			if( y2<yt )y2 = yt;

			if( y>y2 )
			{
				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					for( i=0; i<nbuf; i+=2)
					{
						SetPixel(x+buf[i],y+buf[i+1],m_color);
					}
				}
			}
			
			//绘线
			while (y>y2)
			{
				y--;
				p+=const1;
				add = 0;
				if (p>=0)
				{
					x+=inc;
					p-=const2;
					add = 1;
				}

				if( add )StepPixel(GRIDDIS_PITCH);
				else StepPixel(GRIDDIS_DIRECT);

				if( wid<=1 )SetPixel(x,y,m_color);
				else
				{
					if( add==1 )
					{
						for( i=0; i<nbuf; i+=2)
						{
							SetPixel(x+buf[i],y+buf[i+1],m_color);
						}
					}
					else
					{
						for( i=0; i<nbuf2; i+=2)
						{
							SetPixel(x+buf2[i],y+buf2[i+1],m_color);
						}
					}
				}
			}
		}
	}
}


void CMemDraw::Polyline(POINT *pts, int npt)
{
	if( npt<=1 )return;

	gthis = this;
	if( m_nStyleSeg>0 )
	{
		for( int i=0; i<npt-1; i++,pts++)
		{
			if( m_bRoundLine )
				Point(pts[0].x,pts[0].y,m_color,m_width/2);

			LineEx(pts->x,pts->y,pts[1].x,pts[1].y,m_width);

		}

		if( npt>1 && m_bRoundLine )
			Point(pts[npt-1].x,pts[npt-1].y,m_color,m_width/2);
	}
	else
	{
		for( int i=0; i<npt-1; i++,pts++)
		{
			if( m_bRoundLine )
				Point(pts[0].x,pts[0].y,m_color,m_width/2);

			Line(pts->x,pts->y,pts[1].x,pts[1].y,m_width);
		}

		if( npt>1 && m_bRoundLine )
			Point(pts[npt-1].x,pts[npt-1].y,m_color,m_width/2);
	}
}

void CMemDraw::Polygon(POINT *pts, int npt, BYTE clr)
{
	if( npt<=0 )return;

	gthis = this;
	m_color = clr;
	m_poly.FillPolygon(pts,npt);
}

void CMemDraw::Circle(int x, int y, int r, BYTE clr, int wid)
{
	gthis = this;
	
	if( wid<=1 )
	{
		int i,*buf = m_buf;
		int num = Get8thCircleBuf(r,buf);
		m_color = clr;
		
		for( i=0; i<num; i+=2)
		{
			{
				SetPixel(x-buf[i],y+buf[i+1],m_color);	SetPixel(x+buf[i],y+buf[i+1],m_color);
				SetPixel(x-buf[i],y-buf[i+1],m_color);	SetPixel(x+buf[i],y-buf[i+1],m_color);
			}		
			{
				SetPixel(x-buf[i+1],y+buf[i],m_color);	SetPixel(x+buf[i+1],y+buf[i],m_color);
				SetPixel(x-buf[i+1],y-buf[i],m_color);	SetPixel(x+buf[i+1],y-buf[i],m_color);
			}
		}
	}
	else
	{
		int i,j,*buf = m_buf, *buf2 = m_buf+TMPBUF_HALF;
		int num = Get8thCircleBuf(r+wid-1,buf);
		int num2= Get8thCircleBuf(r,buf2);
		int y0 = r, n, in = 0;

		//补充一个1/8圆弧
		if( buf[num-2]==buf[num-1] )i=num-4; //避免重复
		else i=num-2;
		for( n=num; i>=0; i-=2 )
		{
			buf[n++] = buf[i+1]; buf[n++] = buf[i];
		}
		num = n;

		if( buf2[num2-2]==buf2[num2-1] )i=num2-4; //避免重复
		else i=num2-2;
		for( n=num2; i>=0; i-=2 )
		{
			buf2[n++] = buf2[i+1]; buf2[n++] = buf2[i];
		}
		num2 = n;

		m_color = clr;
		j = 0;
		
		for( i=0; i<num; i+=2)
		{
			//外园还没有与内园在y方向相交
			if( buf[i+1]!=y0 && in==0 )
			{
				if( i>num-4 || (i<=num-4 && buf[i+1]!=buf[i+3]) )
				{
					ScanLine(x-buf[i],x+buf[i],y+buf[i+1]);
					ScanLine(x-buf[i],x+buf[i],y-buf[i+1]);
				}
			}
			else
			{
				in = 1; //进入内圆
				//直到外园在 y0 扫描线上的具有 x 最大值的点才开始绘制
				if( buf[i+1]==y0 && (i>num-4||(i<=num-4 && buf[i+1]!=buf[i+3])) )
				{
					//内园取在 y0 扫描线上的具有 x 最小值的点绘制，因而就是 y0 线上的第一个点
					ScanLine(x-buf[i],x-buf2[j],y+y0);
					ScanLine(x+buf2[j],x+buf[i],y+y0);

					ScanLine(x-buf[i],x-buf2[j],y-y0);
					ScanLine(x+buf2[j],x+buf[i],y-y0);
	
					//内圆跳到下一行
					for( ; j<num2; j+=2)
					{
						if( buf2[j+1]!=y0 )break;
					}
					if( j<num2 )
						y0 = buf2[j+1];
				}
			}

		}
	}
}



MyNameSpaceEnd