// PlotChar.cpp: implementation of the CPlotChar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlotChar.h"
#include "GrBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlotChar::CPlotChar()
{
	m_nChar = 0;
	m_bPlotSpline = TRUE;
}

CPlotChar::~CPlotChar()
{
}

void CPlotChar::SetEnChar(char c)
{
	m_nChar = c;		
}

void CPlotChar::SetCnChar(char c1, char c2)
{
	m_nChar = ((c1&0xff)<<8) + (c2&0xff);
}


double CPlotChar::FixedToDouble(FIXED fxy)
{
	long lxy;
	lxy = *(LONG *)&fxy;
	return ((double)(lxy)/65536.0);
}


FIXED CPlotChar::FloatToFixed( double d )
{
	long l;
	l = (long)(d * 65536L);
	return *(FIXED *)&l;
}



void CPlotChar::DrawOutLine(CArray<GrVertex,GrVertex> *pts,int wType,int cpfx, POINTFX apfx[],POINTFX &pfxStart)
{
	GrVertex pt;
    int i;
	
	pt.z = 0;
	
	if( wType==TT_PRIM_LINE || !m_bPlotSpline )
	{
		for (i=0; i<cpfx;i++) 
		{
			pt.x = FixedToDouble( apfx[i].x );
			pt.y = FixedToDouble( apfx[i].y );
			pt.code = GRBUFFER_PTCODE_LINETO;
			pts->Add(pt);
		}
		pfxStart = apfx[i-1];
	}
	else if( wType==TT_PRIM_QSPLINE )
	{
		PT_3D pfxA, pfxB, pfxC;
		pfxA.x = FixedToDouble(pfxStart.x);                // Starting point for this polygon
		pfxA.y = FixedToDouble(pfxStart.y);  
		int u,count = 0;
		for (u = 0; u < cpfx - 1; u++)  // Walk through points in spline
		{
			pfxB.x = FixedToDouble(apfx[u].x);              // B is always the current point
			pfxB.y = FixedToDouble(apfx[u].y); 
			if (u < cpfx - 2)            // If not on last spline, compute C
			{
				pfxC.x = (pfxB.x + FixedToDouble(apfx[u+1].x))/2;  // x midpoint
				pfxC.y = (pfxB.y + FixedToDouble(apfx[u+1].y))/2;  // y midpoint
			}
			else                         // Else, next point is C
			{
				pfxC.x = FixedToDouble(apfx[u+1].x);
				pfxC.y = FixedToDouble(apfx[u+1].y);
			}
			for(float t = 0.0f;t <= 1.0f;t += 0.2f) 
			{
				double x = (pfxA.x-2*pfxB.x+pfxC.x)*t*t + (2*pfxB.x-2*pfxA.x)*t + pfxA.x;
				double y = (pfxA.y-2*pfxB.y+pfxC.y)*t*t + (2*pfxB.y-2*pfxA.y)*t + pfxA.y;
				pt.x = x;
				pt.y = y;
				if( count==0 )
				{
					pt.code = GRBUFFER_PTCODE_LINETO;
					pts->Add(pt);
				}
				else 
				{
					pt.code = GRBUFFER_PTCODE_LINETO;
					pts->Add(pt);
				}
				count++;
			}			
			pfxA = pfxC;                 // Update current point
		} 
		pfxStart.x = FloatToFixed(pfxC.x);
		pfxStart.y = FloatToFixed(pfxC.y);
	}
	else
	{
		// Bezier曲线
		PT_3D p0,p1,p2,p3,p11,p22,pp0,pp1,pp2,pt11,pt22;
		int i, count = 0;
		for(i = 0;i < cpfx-1;i++) 
		{
			pt11.x = FixedToDouble(apfx[i].x);
			pt11.y = FixedToDouble(apfx[i].y);
			pt22.x = FixedToDouble(apfx[i+1].x);
			pt22.y = FixedToDouble(apfx[i+1].y);
	
			pp0 = pts->GetAt(pts->GetSize()-1);
			pp1 = pt11;
			pp2.x = (pt11.x + pt22.x)/2;
			pp2.y = (pt11.y + pt22.y)/2;
			
			p0 = pp0;
			p1.x = pp0.x/3 + 2 * pp1.x/3;
			p1.y = pp0.y/3 + 2 * pp1.y/3;
			p2.x = 2 * pp1.x/3 + pp2.x/3;
			p2.y = 2 * pp1.y/3 + pp2.y/3;
			p3 = pp2;
			
			for(float t = 0.0f;t <= 1.0f;t += 0.25f) 
			{
				double x = (1-t)*(1-t)*(1-t)*p0.x+ 
					3*t*(1-t)*(1-t)*p1.x+ 3*t*t
					*(1-t)*p2.x + t*t*t*p3.x;
				double y = (1-t)*(1-t)*(1-t)*p0.y
					+ 3*t*(1-t)*(1-t)*p1.y+3
					*t*t*(1-t)*p2.y + t*t*t*p3.y;
				pt.x = x;
				pt.y = y;
				
				if( count==0 )
				{
					pt.code = GRBUFFER_PTCODE_LINETO;
					pts->Add(pt);
				}
				else 
				{
					pt.code = GRBUFFER_PTCODE_LINETO;
					pts->Add(pt);
				}
				count++;
			}
		}
		pfxStart = apfx[i];
		pt.x = FixedToDouble(apfx[i].x);
		pt.y = FixedToDouble(apfx[i].y);
		pt.code = GRBUFFER_PTCODE_LINETO;
		pts->Add(pt);
	}
}


BOOL CPlotChar::Plot(CArray<GrVertex,GrVertex> *pts, CDC *pDC)
{
	if( !pts||!pDC || m_nChar==0 )return FALSE;
//    pts->RemoveAll();
	MAT2 m2;

	m2.eM11 = FloatToFixed(1.0);
	m2.eM12 = FloatToFixed(0.0);
	m2.eM21 = FloatToFixed(0.0);
	m2.eM22 = FloatToFixed(1.0);

	GLYPHMETRICS gm; 

	//计算需要的内存尺寸
	DWORD dwSize = pDC->GetGlyphOutline(m_nChar,GGO_NATIVE,&gm,0L,NULL,&m2); 
	BYTE* pData= new BYTE[dwSize];
	if( !pData )return FALSE;

	LPTTPOLYGONHEADER lpph=(LPTTPOLYGONHEADER)pData;
	pDC->GetGlyphOutline(m_nChar,GGO_NATIVE,&gm,dwSize,lpph,&m2);

	long cbTotal = dwSize, cbOutline;

	LPBYTE lpb;
	GrVertex pt;
	pt.z = 0;
	while( cbTotal > 0 )
	{
		pt.x = FixedToDouble( lpph->pfxStart.x );
		pt.y = FixedToDouble( lpph->pfxStart.y );
		pt.code = GRBUFFER_PTCODE_MOVETO;
		pts->Add(pt);
        //pBuf->MoveTo(&pt);

		lpb  = (LPBYTE)lpph + sizeof(TTPOLYGONHEADER);
		cbOutline = (long)lpph->cb - sizeof(TTPOLYGONHEADER);
    
		while( cbOutline > 0 )
		{
			int  n;
			LPTTPOLYCURVE lpc;
			lpc = (LPTTPOLYCURVE)lpb;                                 
            DrawOutLine(pts,lpc->wType,lpc->cpfx,lpc->apfx,lpph->pfxStart);		
			n = sizeof(TTPOLYCURVE) + sizeof(POINTFX) * (lpc->cpfx - 1);
			lpb += n;
			cbOutline -= n;
		}                   
		pt.code = GRBUFFER_PTCODE_LINETO;
		pts->Add(pt);
		cbTotal -= lpph->cb;
		lpph     = (LPTTPOLYGONHEADER)lpb;
	}	

	delete[] pData;
	return TRUE;
}

MyNameSpaceEnd