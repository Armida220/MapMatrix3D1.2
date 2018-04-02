// DrawLineDC.cpp: implementation of the CMyDC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glew.h"
#include "DrawLineDC.h"
#include "math.h"
#include "SmartViewFunctions.h"
#include "DxFont.h"
#include "PlotWChar.h"


#define D3DFVF_MY_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )

#define COM_RELEASE(p)		if( (p)!=NULL ){ (p)->Release(); (p)=NULL; }


MyNameSpaceBegin

void trim_pts(GrPt3D *&pts, int& npt, int& npt_len, float wid, float hei);

BOOL gbUseTrimDrawLine = FALSE;

void SetDCMode(HDC hDC)
{
/*
	SIZE s1,s2;
	POINT p1,p2;
	XFORM f1;

	GetViewportExtEx(hDC,&s1);
	GetViewportOrgEx(hDC,&p1);
	GetWindowExtEx(hDC,&s2);
	GetWindowOrgEx(hDC,&p2);
	GetWorldTransform(hDC,&f1);	
*/	
	SetMapMode(hDC,MM_TEXT);
	SetViewportOrgEx(hDC,0,0,NULL);
	SetViewportExtEx(hDC,1,1,NULL);	
	SetWindowOrgEx(hDC,0,0,NULL);
	SetWindowExtEx(hDC,1,1,NULL);	
	
	XFORM xForm; 
    xForm.eM11 = (FLOAT) 1.0; 
    xForm.eM12 = (FLOAT) 0.0; 
    xForm.eM21 = (FLOAT) 0.0; 
    xForm.eM22 = (FLOAT) 1.0; 
    xForm.eDx  = (FLOAT) 0.0; 
    xForm.eDy  = (FLOAT) 0.0; 
    SetWorldTransform(hDC, &xForm); 	
}


CTempResetDCInfos::CTempResetDCInfos(HDC hDC)
{
	if( GetMapMode(hDC)!=MM_TEXT )
	{
		hDC0 = hDC;
		nSaveID = SaveDC(hDC);
		SetDCMode(hDC);	
		bUseSave = TRUE;
	}
	else
	{
		hDC0 = NULL;
		nSaveID = 0;
		bUseSave = FALSE;
	}
}


CTempResetDCInfos::~CTempResetDCInfos()
{
	if( bUseSave )
		RestoreDC(hDC0,nSaveID);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDrawOpSet::CDrawOpSet()
{
	m_pCurItem = NULL;
	m_nDrawMode = 0;
	m_hDC = NULL;
}


CDrawOpSet::~CDrawOpSet()
{
	
}


void CDrawOpSet::SetCellInfo(int cell, COLORREF color, float kx, float ky, float angle, float lineWidth, BOOL bGrdSize)
{
	m_cell.valid = 1;
	m_cell.cell = cell;
	m_cell.color = color;
	m_cell.kx = kx;
	m_cell.ky = ky;
	m_cell.angle = angle;
	m_cell.bGrdSize = bGrdSize;
	m_cell.width = lineWidth;
}


void CDrawOpSet::SetCellPt(double x, double y, double z)
{
	m_cell.pt.x = x;
	m_cell.pt.y = y;
	m_cell.pt.z = z;
}

BOOL CDrawOpSet::CompareCell(int cell, COLORREF color, float kx, float ky, float angle, float lineWidth, BOOL bGrdSize)
{
	if( m_cell.valid &&
		m_cell.cell==cell && 
		m_cell.color==color &&
		m_cell.angle==angle &&
		m_cell.kx==kx &&
		m_cell.ky==ky &&
		m_cell.width==lineWidth &&
		m_cell.bGrdSize==bGrdSize )
		return TRUE;

	return FALSE;
}


void CDrawOpSet::SetDrawMode(int mode)
{
	m_nDrawMode = mode;
}

void CDrawOpSet::Point(double x, double y, double z, COLORREF color, float size, BOOL bFlat)
{
	OpPoint *pItem = new OpPoint();
	pItem->x = x;
	pItem->y = y;
	pItem->z = z;
	pItem->color = color;
	pItem->width = size;
	pItem->bFlat = bFlat;

	m_arrPItems.Add(pItem);
}

void CDrawOpSet::Begin(int npt)
{
	OpLines *pItem = new OpLines();
	m_pCurItem = pItem;
	m_arrPItems.Add(pItem);	
}

void CDrawOpSet::SetColor(COLORREF clr)
{
	if( m_pCurItem )
		m_pCurItem->color = clr;
}


void CDrawOpSet::SetWidth(float wid)
{
	if( m_pCurItem && m_pCurItem->type==DRAWOP_LINE )
		((OpLines*)m_pCurItem)->width = wid;	
}

void CDrawOpSet::MoveTo(double x, double y, double z)
{
	if( m_pCurItem && m_pCurItem->type==DRAWOP_LINE )
	{
		GrPt3D pt = {x,y,z,0};
		((OpLines*)m_pCurItem)->arrPts.Add(pt);
	}
}


void CDrawOpSet::LineTo(double x, double y, double z)
{
	if( m_pCurItem && m_pCurItem->type==DRAWOP_LINE )
	{
		GrPt3D pt = {x,y,z,1};
		((OpLines*)m_pCurItem)->arrPts.Add(pt);
	}	
}


void CDrawOpSet::SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle)
{
	if( m_pCurItem && m_pCurItem->type==DRAWOP_LINE )
	{
		OpLines *p = ((OpLines*)m_pCurItem);
		p->nstyle = dwStyleCount;
		memcpy(p->styles,lpStyle,sizeof(DWORD)*dwStyleCount);
	}	
}


void CDrawOpSet::End()
{
	
}



void CDrawOpSet::Polygon(PT_3D *pts, int npt, COLORREF color)
{
	OpPolygons *pItem = new OpPolygons();

	pItem->color = color;
	pItem->arrCounts.Add(npt);
	pItem->arrPts.SetSize(npt);
	memcpy(pItem->arrPts.GetData(),pts,npt*sizeof(PT_3D));

	m_arrPItems.Add(pItem);		
}


void CDrawOpSet::PolyPolygon(PT_3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color)
{
	OpPolygons *pItem = new OpPolygons();
	
	pItem->color = color;
	pItem->arrCounts.SetSize(nPolygon);
	memcpy(pItem->arrCounts.GetData(),pCounts,nPolygon*sizeof(int));

	pItem->arrPts.SetSize(npt);
	memcpy(pItem->arrPts.GetData(),pts,npt*sizeof(PT_3D));
	
	m_arrPItems.Add(pItem);		
}


void CDrawOpSet::SetTextSettings(TextSettings *pSet)
{
	OpText *pItem = new OpText();
	
	memcpy(&pItem->settings,pSet,sizeof(TextSettings));
	
	m_arrPItems.Add(pItem);	

	m_pCurItem = pItem;
}


void CDrawOpSet::DrawText(LPCTSTR strText, double x, double y, COLORREF color, COLORREF bkColor, BOOL bSmooth, BOOL bOutLine)
{
	if( m_pCurItem && m_pCurItem->type==DRAWOP_TEXT )
	{
		OpText *p = ((OpText*)m_pCurItem);
		p->strText = strText;
		p->color = color;
		p->pt.x = x; p->pt.y = y;
		p->bkColor = bkColor;
		p->bSmooth = bSmooth;
		p->bOutLine = bOutLine;
	}
}



void CDrawOpSet::MoveOpSetXY(double dx, double dy, double dz)
{
	for( int i=0; i<m_arrPItems.GetSize(); i++)
	{
		OpItem *pItem = m_arrPItems[i];
		if( pItem->type==DRAWOP_POINT )
		{
			OpPoint *p = (OpPoint*)pItem;
			p->x += dx;
			p->y += dy;
			p->z += dz;
		}
		else if( pItem->type==DRAWOP_LINE )
		{
			OpLines *p = (OpLines*)pItem;
			GrPt3D *pts = p->arrPts.GetData();
			int npt = p->arrPts.GetSize();
			for( int i=0; i<npt; i++,pts++)
			{
				pts->x += dx;
				pts->y += dy;
				pts->z += dz;
			}
		}
		else if( pItem->type==DRAWOP_POLYGON )
		{
			OpPolygons *p = (OpPolygons*)pItem;
			PT_3D *pts = p->arrPts.GetData();
			int npt = p->arrPts.GetSize();
			for( int i=0; i<npt; i++,pts++)
			{
				pts->x += dx;
				pts->y += dy;
				pts->z += dz;
			}
		}
	}
}

void CDrawOpSet::MoveToOpSetXY(double x, double y, double z)
{
	double dx = x-m_cell.pt.x, dy = y-m_cell.pt.y, dz = z-m_cell.pt.z;
	MoveOpSetXY(dx,dy,dz);

	m_cell.pt.x = x;
	m_cell.pt.y = y;
	m_cell.pt.z = z;
}

void CDrawOpSet::SetAllColor(COLORREF clr)
{
	for( int i=0; i<m_arrPItems.GetSize(); i++)
	{
		OpItem *pItem = m_arrPItems[i];
		pItem->color = clr;
	}
}

void CDrawOpSet::ClearAll()
{
	for( int i=0; i<m_arrPItems.GetSize(); i++)
	{
		OpItem *pItem = m_arrPItems[i];
		if( pItem->type==DRAWOP_POINT )
		{
			delete (OpPoint*)pItem;
		}
		else if( pItem->type==DRAWOP_LINE )
		{
			delete (OpLines*)pItem;
		}
		else if( pItem->type==DRAWOP_POLYGON )
		{
			delete (OpPolygons*)pItem;
		}
	}
	m_arrPItems.RemoveAll();

	m_cell.valid = 0;
	m_pCurItem = NULL;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDrawLineDC::CDrawLineDC()
{
	cur_color = RGB(255,255,255);
	cur_width = 0;
	
	pts = NULL;
	npt_use = 0;
	npt_len = 0;
	cur_state = penStart;

	cur_x = 0;
	cur_y = 0;
	cur_z = 0;

	m_bUseEnvelope = FALSE;
	bPenChanged = TRUE;

	count = 0;
}


CDrawLineDC::~CDrawLineDC()
{
	if( pts )delete[] pts;
}

int CDrawLineDC::GetPts(GrPt3D *&arr)
{
	arr = pts;
	return npt_use;
}

void CDrawLineDC::SetEnvelope(BOOL bUseEnvelope, Envelope e)
{
	m_bUseEnvelope = bUseEnvelope;
	m_xl = e.m_xl; m_xh = e.m_xh;
	m_yl = e.m_yl; m_yh = e.m_yh;
}


void CDrawLineDC::MoveTo(double x, double y, double z)
{
	if( cur_state==penStart )
	{
// 		pts[npt_use++] = x;
// 		pts[npt_use++] = y;
// 		pts[npt_use++] = z;
		pts[npt_use].x = x;
		pts[npt_use].y = y;
		pts[npt_use].z = z;
		pts[npt_use].pencode = penMove;
		npt_use++;
		cur_state = penMove;
	}
	else
	{
		pts[npt_use].x = x;
		pts[npt_use].y = y;
		pts[npt_use].z = z;
		pts[npt_use].pencode = penMove;
		npt_use++;
// 		pts[npt_use-3] = x;
// 		pts[npt_use-2] = y;
// 		pts[npt_use-1] = z;
	}

	cur_x = x; cur_y = y; cur_z = z;
}


void CDrawLineDC::LineTo(double x, double y, double z)
{
	if( cur_state==penStart )
	{
		MoveTo(x,y,z);
		return;
	}

	if( m_bUseEnvelope && npt_use>=1/*npt_use>=3*/ )
	{
// 		if( min(x,pts[npt_use-3])>m_xh || max(x,pts[npt_use-3])<m_xl ||
// 			min(y,pts[npt_use-2])>m_yh || max(y,pts[npt_use-2])<m_yl )
		if( min(x,pts[npt_use-1].x)>m_xh+cur_width || max(x,pts[npt_use-1].x)<m_xl-cur_width ||
			min(y,pts[npt_use-1].y)>m_yh+cur_width || max(y,pts[npt_use-1].y)<m_yl-cur_width )
		{
// 			count++;
			MoveTo(x,y,z);
 			return;
		}
	}

	pts[npt_use].x = x;
	pts[npt_use].y = y;
	pts[npt_use].z = z;
	pts[npt_use].pencode = penLine;
	npt_use++;
// 	pts[npt_use++] = x;
// 	pts[npt_use++] = y;
// 	pts[npt_use++] = z;
// 
// 	pts[npt_use++] = x;
// 	pts[npt_use++] = y;
// 	pts[npt_use++] = z;

	cur_x = x; cur_y = y; cur_z = z;

	cur_state = penLine;
}


void CDrawLineDC::Rectangle(double x1, double y1, double x2, double y2, double z)
{
	MoveTo(x1,y1,z);
	LineTo(x2,y1,z);
	LineTo(x2,y2,z);
	LineTo(x1,y2,z);
	LineTo(x1,y1,z);
}

void CDrawLineDC::SetColor(COLORREF clr)
{
	if( cur_color!=clr )
	{
		End();
//		Begin(npt_len/6);
		Begin(npt_len);

		cur_color = clr;

		bPenChanged = TRUE;
	}
}

COLORREF CDrawLineDC::GetColor()
{
	return cur_color;
}


void CDrawLineDC::SetWidth(float wid)
{
//	wid = (int)wid;
	if( wid<1.0f )wid = 1.0f;
	if( cur_width!=wid )
	{
		End();
//		Begin(npt_len/6);
		Begin(npt_len);
		
		cur_width = wid;

		bPenChanged = TRUE;
	}
}



void CDrawLineDC::Begin(int npt)
{
	if( npt<=0 )
		return;
	
	int nuse = npt_use;
	npt_use = 0;
	cur_state = penStart;

//	npt = npt*6+3;
	if( npt<=npt_len )
	{
		if( nuse>0 )MoveTo(cur_x,cur_y,cur_z);
		return;
	}

	npt_len = 0;

	if( pts )delete[] pts;
	
//	pts = new float[npt];
	pts = new GrPt3D[npt];
	if( !pts )return;
	npt_len = npt;	

	if( nuse>0 )MoveTo(cur_x,cur_y,cur_z);
}



void CDrawLineDC::End()
{
	npt_use = 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define pGDIPlus		((Graphics*)pGraphics)	

CGdiDrawLineDC::CGdiDrawLineDC(HDC hdc)
{
	hDC = hdc;
	hDefPen = NULL;
	hCurPen = NULL;
	nExtStyle = 0;
	m_bRoundLine = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_ROUNDLINE,FALSE);

	pGraphics = new Graphics(hdc);
	pGDIPlus->SetPageUnit(UnitPixel); 
}

CGdiDrawLineDC::~CGdiDrawLineDC()
{
	if( pGDIPlus )
		delete pGDIPlus;

	RestoreDefPen();
}

void CGdiDrawLineDC::RestoreDefPen()
{
	if( hDC && hDefPen )
	{
		HPEN hPen = (HPEN)::SelectObject(hDC,hDefPen);
		if( hPen!=hDefPen )
			::DeleteObject(hPen);
	}

	hDefPen = NULL;
}


void CGdiDrawLineDC::SetDrawMode(int mode)
{
	if( mode==DRAWMODE_XOR )
		::SetROP2(hDC,R2_XORPEN);
	else if( mode==DRAWMODE_COPY )
		::SetROP2(hDC,R2_COPYPEN);
}

void CGdiDrawLineDC::End()
{
	if( !hDC )return;

	int use = npt_use;
	npt_use = 0;
	if( cur_state==penStart )return; 
//	if( cur_state==penMove )use -= 3;
//	if( use<6 )return;
	if( use<2 )return;

	if(gbUseTrimDrawLine)trim_pts(pts,use,npt_len,m_xh,m_yh);

	if( GetROP2(hDC)==R2_XORPEN )
	{
		HPEN hPen = hCurPen;
		
		if( bPenChanged )
		{
			if( nExtStyle>1 )
			{
				LOGBRUSH lbrush = {BS_SOLID,cur_color,0};
				DWORD dwPenStyle = PS_ENDCAP_FLAT|PS_GEOMETRIC|PS_USERSTYLE;
				if (m_bRoundLine)
				{
					dwPenStyle &= ~PS_ENDCAP_FLAT;
				}
				
				hPen = ::ExtCreatePen(dwPenStyle,(int)cur_width,&lbrush,nExtStyle,pStyles);
				nExtStyle = 0;
			}
			else
			{
				LOGBRUSH lbrush = {BS_SOLID,cur_color,0};
				
				DWORD dwPenStyle = PS_ENDCAP_FLAT|PS_SOLID|PS_GEOMETRIC;
				if (m_bRoundLine)
				{
					dwPenStyle &= ~PS_ENDCAP_FLAT;
				}
				
				hPen = ::ExtCreatePen(dwPenStyle,(int)cur_width,&lbrush,0,0);
			}
			
			hCurPen = hPen;
		}
		
		if( !hPen )
			return;
		
		if( !hDefPen )
			hDefPen = (HPEN)::SelectObject(hDC,hPen);
		else
		{
			HPEN hOld = (HPEN)::SelectObject(hDC,hPen);
			::DeleteObject(hOld);
		}
		
		POINT *lppt = new POINT[use];
		if (!lppt) return;
		int nBegin = 0;
		for( int i=0; i<use; i++ )
		{
			lppt[i].x = (int)pts[i].x;
			lppt[i].y = (int)pts[i].y;
			if (pts[i].pencode == penMove)
			{			
				if (i-nBegin>=2)
				{
					::Polyline(hDC,lppt+nBegin,i-nBegin);
				}
				nBegin = i;
				
			}
		}
		
		if (i-nBegin >= 2)
		{
			::Polyline(hDC,lppt+nBegin,i-nBegin);
		}
	}
	else if( pGraphics )
	{		
		Color color;
		color.SetFromCOLORREF(cur_color);
		
		Pen pen(color,cur_width);	
		if (m_bRoundLine)
		{
			pen.SetStartCap(LineCapRound);
			pen.SetEndCap(LineCapRound);
			pen.SetLineJoin(LineJoinRound);
		}
		
		if( nExtStyle>1 )
		{
			float fStyles[8];
			for( int i=0; i<8; i++)
			{
				fStyles[i] = pStyles[i];
			}
			pen.SetDashPattern(fStyles,nExtStyle);
		}

		CTempResetDCInfos resetDC(hDC);
		
		Gdiplus::Point *lppt = new Gdiplus::Point[use];
		if (!lppt) return;
		int nBegin = 0;
		for( int i=0; i<use; i++ )
		{
			lppt[i].X = pts[i].x;
			lppt[i].Y = pts[i].y;
			if (pts[i].pencode == penMove)
			{			
				if (i-nBegin>=2)
				{
					pGDIPlus->DrawLines(&pen,lppt+nBegin,i-nBegin);

					if( cur_width>6 && lppt[nBegin].Equals(lppt[i-1]) )
					{
						Point(lppt[i-1].X,lppt[i-1].Y,cur_color,cur_width/2,FALSE);
					}
				}
				nBegin = i;
				
			}
		}

		
		if (i-nBegin >= 2)
		{
			pGDIPlus->DrawLines(&pen,lppt+nBegin,i-nBegin);
			if( cur_width>6 && lppt[0].Equals(lppt[use-1]) && use>=4 && pts[1].pencode==penLine && pts[use-1].pencode==penLine )
			{
				Gdiplus::Point lppt2[3] = {lppt[1],lppt[0],lppt[use-2]};
				pGDIPlus->DrawLines(&pen,lppt2,3);
			}
		}
		
		delete []lppt;
	}

	bPenChanged = FALSE;
}

void CGdiDrawLineDC::SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle)
{
	nExtStyle = min(dwStyleCount,8);
	memcpy(pStyles,lpStyle,nExtStyle*sizeof(DWORD));

	bPenChanged = TRUE;
}

void CGdiDrawLineDC::Point(double x, double y, COLORREF color, float size, BOOL bFlat)
{
	if( !hDC )return;

	if( size<1.5f )
	{
		::SetPixel(hDC,x,y,color);
/*
		Color color1;
		color1.SetFromCOLORREF(color);
		
		Pen pen(color1,1);
		
		PointF lppt[2];
		lppt[0].X = x-0.3f;
		lppt[0].Y = y;
		lppt[1].X = x+0.3f;
		lppt[1].Y = y;	
		
		pGDIPlus->DrawLines(&pen,lppt,2);
*/		
		return;
	}

	if( size>400 )
		size = 400;
	if( bFlat )
	{
		::FillRect(hDC,&CRect(x-size*0.5,y-size*0.5,x+size*0.5,y+size*0.5),CBrush(color));

		return;
	}

	Color color1;
	color1.SetFromCOLORREF(color);

	CTempResetDCInfos resetDC(hDC);
	
	//这种方式绘制圆点效果更好，速度更快
	pGDIPlus->FillEllipse(&SolidBrush(color1),x-size*0.5,y-size*0.5,size,size);

/*
	::BeginPath(hDC);
	::MoveToEx(hDC,x,y,NULL);
	::AngleArc(hDC,x,y,size*0.5,0,360);
	::EndPath(hDC);

	HBRUSH hOldBrush = NULL;
	HPEN hOldPen = NULL;

	CBrush brsh(color);
	CPen pen(PS_SOLID|PS_GEOMETRIC,1,color);
	
	hOldPen = (HPEN)::SelectObject(hDC,pen);
	hOldBrush = (HBRUSH)::SelectObject(hDC,brsh);

	::StrokeAndFillPath(hDC);

	::SelectObject(hDC,hOldPen);
	::SelectObject(hDC,hOldBrush);
*/
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlDrawLineDC::CGlDrawLineDC()
{
	m_bRoundLine = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_ROUNDLINE,FALSE);
}

CGlDrawLineDC::~CGlDrawLineDC()
{
	
}



void CGlDrawLineDC::End()
{
	int use = npt_use;
	npt_use = 0;
	if( cur_state==penStart )return;
// 	if( cur_state==penMove )use -= 3;
// 	if( use<6 )return;
	if (use < 2) return; 

	::glColor3f(GetRValue(cur_color)/255.0,GetGValue(cur_color)/255.0,GetBValue(cur_color)/255.0);
	if( cur_width<1 )::glLineWidth(1.0f);
	else ::glLineWidth(cur_width);
	
	int cur_pos = 0;

	for( int i=0; i<use; i++ )
	{
		if( i==(use-1) || pts[i].pencode == penMove )
		{
			if( pts[i].pencode==penLine )
				i++;

			if( i-cur_pos>=2 )
			{
				::glBegin(GL_LINE_STRIP);
				for( int j=cur_pos; j<i; j++)
				{
					::glVertex3f(pts[j].x,pts[j].y,pts[j].z);
				}
				::glEnd();
			}
			cur_pos = i;
		}		
	}

	if (m_bRoundLine)
	{		
		double range[2];
		::glGetDoublev(GL_LINE_WIDTH_RANGE,range);

		if (cur_width > 2)
		{
			// 半径
			float r = cur_width;
			if (r > range[1])
			{
				r = range[1];
			}

			r = (r/9*8)/2;

			float delta_theta = 0.5;
			
			
			for(i=0; i<use; i++ )
			{
				glBegin(GL_POLYGON);

				for( float angle = 0; angle < 2*PI; angle += delta_theta )
					glVertex3f( pts[i].x+r*cos(angle), pts[i].y+r*sin(angle), pts[i].z );
		
				glEnd();
			}			
			
			
// 			::glEnable(GL_POINT_SMOOTH);
// 
// 			//draw the round points as joins
// 			float pointsize = cur_width;
// 			if (pointsize > range[1])
// 			{
// 				pointsize = range[1];
// 			}
// 			
// 			glPointSize(pointsize/4*3);
// 			::glBegin(GL_POINTS);
// 			for(i=0; i<use; i++ )
// 			{
// 				::glVertex3f(pts[i].x,pts[i].y,pts[i].z);
// 			}
// 			::glEnd();
		}
		else
		{
			//::glDisable(GL_POINT_SMOOTH);
		}

		
	}	

	::glDisable(GL_LINE_STIPPLE);
}


void CGlDrawLineDC::SetDrawMode(int mode)
{
	glEnable(GL_COLOR_LOGIC_OP);

	if( mode==DRAWMODE_XOR )
		::glLogicOp(GL_XOR);
	else if( mode==DRAWMODE_COPY )
		::glLogicOp(GL_COPY);
}


void CGlDrawLineDC::SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle)
{
	// 将GDI模式的虚线模式转到GL下
	GLushort pattern = 0xFFFF;
	GLint factor = 1;
	// 画虚线时
	if (dwStyleCount > 1)
	{
		
		GLushort nCount = 0;
		for (int i=0; i<dwStyleCount; i++)
		{
			nCount += lpStyle[i];
		}

		if (nCount <= 0)
		{
			goto END;
		}

		pattern = 0x00;

		factor = nCount/16>0?nCount/16:1;
		// 用16位表示,保持比例
		float fpart = (float)16/nCount;

		GLushort styles[8];
		// 16位用了多少位
		GLushort nCountUsed = 0;
		for (i=0; i<dwStyleCount; i++)
		{
			if (i == dwStyleCount-1)
			{
				styles[dwStyleCount-1] = 16 - nCountUsed;
				break;
			}
			styles[i] = lpStyle[i]*fpart;
			nCountUsed += styles[i];
		}

		// 给pattern赋值
		GLushort ustart = 0, uend = 0;
		for (i=0; i<dwStyleCount; i++)
		{
			ustart = uend;
			uend += styles[i];
			if (i%2 == 1) continue;

//			GLushort uvalue = pow(2,styles[i]) - 1;
//			GLint uvalue = 0x80000000;
//			uvalue = uvalue<<styles[i];
			GLushort uvalue = 0xFFFF;
			// 将styles[i]个低位置为1
			uvalue = ~(uvalue<<styles[i]);
			// 移到正确的位置
			uvalue = uvalue<<ustart;

			pattern |= uvalue;
			
		}		
		
	}
END:
	::glEnable(GL_LINE_STIPPLE); //启动虚线模式（使用glDisable(GL_LINE_STIPPLE)可以关闭之）
	::glLineStipple(factor,pattern);
}


void CGlDrawLineDC::BeginPoint(COLORREF color, float size, BOOL bFlat)
{
	size = (int)size;
	if( size<2.0f )bFlat = TRUE;

	curPointFlat = bFlat;
	curPointSize = size;
	curPointColor = color;
	if( curPointSize<1.0f )
		curPointSize = 1.0f;

	if( curPointFlat )
	{
		if( curPointSize<50 )
		{
			glPointSize(curPointSize);
			glColor3f(GetRValue(curPointColor)/255.0,GetGValue(curPointColor)/255.0,GetBValue(curPointColor)/255.0);
			glBegin(GL_POINTS);
		}
		else
		{

		}
	}
}

void CGlDrawLineDC::Point(double x, double y, double z)
{
	if( curPointFlat )
	{
		if( curPointSize<50 )
		{
			glVertex3f(x,y,z);
		}
		else
		{
			float r = curPointSize*0.5;
			
			glColor3f(GetRValue(curPointColor)/255.0,GetGValue(curPointColor)/255.0,GetBValue(curPointColor)/255.0);
			glBegin(GL_POLYGON);
			
			glVertex3f(x-r, y-r, z);
			glVertex3f(x+r, y-r, z);
			glVertex3f(x+r, y+r, z);
			glVertex3f(x-r, y+r, z);

			glEnd();
		}
	}
	else
	{
		float r = curPointSize*0.5;
		float step = 2/r, ang=0;

		if (step < 0.05)
			step = 0.05;

		glColor3f(GetRValue(curPointColor)/255.0,GetGValue(curPointColor)/255.0,GetBValue(curPointColor)/255.0);
		glBegin(GL_POLYGON);
		while( ang<6.283 )
		{
			glVertex3f(x + r*cos(ang), y + r*sin(ang), z);
			ang += step;
		}
		glEnd();
	}
}


void CGlDrawLineDC::BatchPointColor(COLORREF color)
{
	curPointColor = color;
	glColor3f(GetRValue(curPointColor)/255.0,GetGValue(curPointColor)/255.0,GetBValue(curPointColor)/255.0);
}


void CGlDrawLineDC::EndPoint()
{
	if( curPointFlat )
	{
		glEnd();
	}
}



void CGlDrawLineDC::PolyPolygon(GrPt3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color)
{
	::glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	CDxPolygon polygon;

	cur_color = color;

	polygon.BeginPolygon(cur_color);

	GrPt3D *p = pts;
	for( int j=0; j<nPolygon; j++)
	{
		polygon.BeginContour();			
		for( int i=0; i<pCounts[j]; i++, p++)
		{
			polygon.Vertex(p[0].x,p[0].y,p[0].z);
		}
		polygon.EndContour();
	}
	
	polygon.EndPolygon();
}


/// CGdiDrawingDC

BOOL CGdiDrawingDC::m_bUseGDIPolygon = FALSE;

CGdiDrawingDC::CGdiDrawingDC(HDC hDC):CGdiDrawLineDC(hDC)
{
	m_bUseGDIDC = TRUE;

	HBITMAP hbitmap = (HBITMAP)::GetCurrentObject(hDC,OBJ_BITMAP);
	DIBSECTION info;
	if( GetObject(hbitmap,sizeof(info),&info) )
	{
		if( info.dsBm.bmBits!=NULL )
		{
			m_dr.Init8BitMem((BYTE*)info.dsBm.bmBits,info.dsBm.bmWidth,info.dsBm.bmHeight,info.dsBmih.biBitCount);
			m_bUseGDIDC = FALSE;
		}

		if( info.dsBm.bmBitsPixel<=8 )
		{
			RGBQUAD clrtbl[256];
			memset(clrtbl,0,sizeof(clrtbl));
			int num = GetDIBColorTable(hDC,0,(1<<info.dsBm.bmBitsPixel),clrtbl);
			m_dr.SetColorTable(clrtbl,num);
		}

		if( info.dsBm.bmBitsPixel>=24 )
			m_bUseGDIDC = TRUE;
	}

	m_bUseGDIDC = TRUE;

	if( !m_bUseGDIDC )
	{
		m_dr.SetROP2(::GetROP2(hDC));
	}
}


CGdiDrawingDC::~CGdiDrawingDC()
{
}

void CGdiDrawingDC::MoveTo(double x, double y, double z)
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::MoveTo(x,y,z);
		return;
	}
	m_dr.MoveTo(x,y);
}

void CGdiDrawingDC::LineTo(double x, double y, double z)
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::LineTo(x,y,z);
		return;
	}
	m_dr.LineTo(x,y);
}

void CGdiDrawingDC::SetColor(COLORREF clr)
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::SetColor(clr);
		return;
	}
	m_dr.SetLineStyle(m_dr.GetColorFrom24Bit(clr),m_dr.m_width,m_bRoundLine);
}

void CGdiDrawingDC::SetWidth(float wid)
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::SetWidth(wid);
		return;
	}
	if( wid>256 )wid = 256;
	m_dr.SetLineStyle(m_dr.m_color,wid,m_bRoundLine);
}


void CGdiDrawingDC::Rectangle(double x1, double y1, double x2, double y2, double z)
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::Rectangle(x1,y1,x2,y2,z);
		return;
	}
	m_dr.Rectangle(x1,y1,x2,y2);
}


void CGdiDrawingDC::Begin(int npt)
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::Begin(npt);
		return;
	}
}

void CGdiDrawingDC::End()
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::End();
		return;
	}
}

void CGdiDrawingDC::RestoreDefPen()
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::RestoreDefPen();
		return;
	}
}

void CGdiDrawingDC::SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle)
{
	for( int i=0; i<dwStyleCount; i++)
	{
		if( lpStyle[i]<0.5 )break;
	}

	if( i<dwStyleCount )
	{
		dwStyleCount = 0;
		lpStyle = 0;
	}

	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::SetExtPen(dwStyleCount,lpStyle);
		return;
	}
	dwStyleCount = (dwStyleCount/2)*2;
	if( dwStyleCount>0 )
		m_dr.SetLineStyleEx0(dwStyleCount,(DWORD*)lpStyle,FALSE);
	else
		m_dr.m_nStyleSeg = 0;
}



void CGdiDrawingDC::Point(double x, double y, COLORREF color, float size, BOOL bFlat)
{
	if( m_bUseGDIDC )
	{
		CGdiDrawLineDC::Point(x,y,color,size,bFlat);
		return;
	}
	m_dr.Point(x,y,m_dr.GetColorFrom24Bit(color),size);
}


void CGdiDrawingDC::PolyPolygon(GrPt3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color)
{
	//用GDI+可以修正浮点误差
	if( !m_bUseGDIPolygon )
	{
		GraphicsPath   path1;		
		
		PointF *lppt = new PointF[npt];
		if (!lppt) return;

		for( int i=0; i<npt; i++)
		{
			lppt[i].X = pts[i].x;
			lppt[i].Y = pts[i].y;
		}

		int pos = 0;
		for( i=0; i<nPolygon; i++)
		{
			path1.AddPolygon(lppt+pos,pCounts[i]);
			pos += pCounts[i];
		}
		
		cur_color = color;

		Color color2(255-(cur_color>>24),GetRValue(cur_color),GetGValue(cur_color),GetBValue(cur_color));

		CTempResetDCInfos resetDC(hDC);

		SolidBrush brush(color2);
		pGDIPlus->FillPath(&brush,&path1);
		
		delete []lppt;
		return;

	}
	{
		POINT *pts2 = new POINT[npt];
		if( !pts2 )
			return;
		
		for( int i=0; i<npt; i++)
		{
			pts2[i].x = pts[i].x;
			pts2[i].y = pts[i].y;
		}

		if (m_bUseGDIDC)
		{
			cur_color = color;
			CBrush brush(color);
			CPen pen(PS_SOLID,1,color);
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC,brush);
			HPEN hOldPen = (HPEN)::SelectObject(hDC,pen);

			::PolyPolygon(hDC,pts2,pCounts,nPolygon);

			SelectObject(hDC,hOldPen);
			SelectObject(hDC,hOldBrush);
			delete[] pts2;
			return;
		}

		m_dr.PolyPolygon(pts2,pCounts,nPolygon,m_dr.GetColorFrom24Bit(color));
		delete[] pts2;
	}
}


CD3DDrawLineDC::CD3DDrawLineDC()
{
	m_pD3D = NULL;

	m_bUseStyle = FALSE;
}


CD3DDrawLineDC::~CD3DDrawLineDC()
{

}



void CD3DDrawLineDC::Init(CD3DWrapper *p)
{
	m_pD3D = p;
}



void CD3DDrawLineDC::SetDrawMode(int mode)
{
}

void CD3DDrawLineDC::Begin(int npt)
{
	CDrawLineDC::Begin(npt);
}



static void swap(GrPt3D *& pt1, GrPt3D *& pt2, double& dx, double& dy)
{
	GrPt3D *t = pt1;
	pt1 = pt2;
	pt2 = t;
	dx = -dx;
	dy = -dy;
}


static int trim_line(GrPt3D *pt1, GrPt3D *pt2, GrPt3D *pts_new, float wid, float hei)
{
	double ymin = min(pt1->y,pt2->y);
	double ymax = max(pt1->y,pt2->y);
	double xmin = min(pt1->x,pt2->x);
	double xmax = max(pt1->x,pt2->x);

	if(ymax<0 || ymin>hei)return 0;
	if(xmax<0 || xmin>wid)return 0;

	double dx0 = (pt2->x-pt1->x), dy0 = (pt2->y-pt1->y);
	double dx = fabs(dx0), dy = fabs(dy0);
	if(dx<1.0 && dy<1.0)
	{
		if(pt1->x<=wid+2 && pt1->y<=hei+2 && pt1->x>=-2 && pt1->y>=-2)
		{
			pts_new[0] = *pt1;
			pts_new[1] = *pt2;
			pts_new[0].pencode = penMove;
			pts_new[1].pencode = penLine;
			return 2;
		}
		else
		{
			return 0;
		}
	}
		
	if(dx>dy)
	{
		if(pt1->x>pt2->x)swap(pt1,pt2,dx0,dy0);

		if(pt1->x<=wid && pt1->x>=0)
		{
			pts_new[0] = *pt1;
			pts_new[0].pencode = penMove;
		}
		else
		{
			pts_new[0].x = 0;
			pts_new[0].y = pt1->y + dy0*(-pt1->x/dx0);
			pts_new[0].pencode = penMove;
		}

		if(pt2->x<=wid && pt2->x>=0)
		{
			pts_new[1] = *pt2;
			pts_new[1].pencode = penLine;
		}
		else
		{
			pts_new[1].x = wid;
			pts_new[1].y = pt1->y + dy0*(wid-pt1->x)/dx0;
			pts_new[1].pencode = penLine;
		}

		return 2;
	}
	else
	{
		if(pt1->y>pt2->y)swap(pt1,pt2,dx0,dy0);

		if(pt1->y<=hei && pt1->y>=0)
		{
			pts_new[0] = *pt1;
			pts_new[0].pencode = penMove;
		}
		else
		{
			pts_new[0].y = 0;
			pts_new[0].x = pt1->x + dx0*(-pt1->y/dy0);
			pts_new[0].pencode = penMove;
		}

		if(pt2->y<=hei && pt2->y>=0)
		{
			pts_new[1] = *pt2;
			pts_new[1].pencode = penLine;
		}
		else
		{
			pts_new[1].y = hei;
			pts_new[1].x = pt1->x + dx0*(wid-pt1->y)/dy0;
			pts_new[1].pencode = penLine;
		}
		
		return 2;
	}
}

static void trim_pts(GrPt3D *&pts, int& npt, int& npt_len, float wid, float hei)
{
	GrPt3D *pts_new = new GrPt3D[npt_len*2];
	int pos = 0;
	for(int i=1; i<npt; i++)
	{
		if(pts[i].pencode==penLine)
		{
			pos += trim_line(pts+i-1,pts+i,pts_new+pos,wid,hei);
		}
	}

	npt = pos;
	delete[] pts;
	pts = pts_new;
}


void CD3DDrawLineDC::End()
{
	if( npt_use<2 )
	{
		CDrawLineDC::End();
		return;
	}

	m_pD3D->m_pLine->SetWidth(cur_width);
	
	DWORD color = D3DCOLOR_RGBA(GetRValue(cur_color),GetGValue(cur_color),GetBValue(cur_color),255);
	
	D3DXVECTOR3 *pVertices = new D3DXVECTOR3[npt_use];
	
	int i = 0, j = 0;
	
	while( i<npt_use )
	{
		for( ; i<npt_use-1; i++)
		{
			if( pts[i+1].pencode==penLine )
				break;
		}
		
		j = 0;
		
		pVertices[j].x = pts[i].x;
		pVertices[j].y = pts[i].y;
		pVertices[j].z = 0;
		j++; i++;
		
		for( ; i<npt_use; i++, j++)
		{
			if( pts[i].pencode==penMove )
				break;
			
			pVertices[j].x = pts[i].x;
			pVertices[j].y = pts[i].y;
			pVertices[j].z = 0;
		}
		
		if( j>1 )
		{
			D3DXMATRIX matProject, matView, matWorld, m1, m2;
			m_pD3D->m_pD3DDevice9->GetTransform(D3DTS_PROJECTION,&matProject);
			m_pD3D->m_pD3DDevice9->GetTransform(D3DTS_VIEW,&matView);
			m_pD3D->m_pD3DDevice9->GetTransform(D3DTS_WORLD,&matWorld);
			
			D3DXMatrixMultiply(&m1,&matView,&matProject);
			D3DXMatrixMultiply(&m2,&matWorld,&m1);
			
			m_pD3D->m_pLine->DrawTransform(pVertices,j,&m2,color);
		}
	}
	
	delete[] pVertices;

	CDrawLineDC::End();
}

void CD3DDrawLineDC::Point(double x, double y, COLORREF color, float size, BOOL bFlat/* =TRUE */)
{
	if( m_pD3D->m_pPointList_VB==NULL )
		return;

	if( !bFlat && size>=2.0f )
	{
		float r = size*0.5;
		float step = 2/r, ang=0;
		
		if (step < 0.05)
			step = 0.05;

		CArray<CD3DWrapper::D3DVertex,CD3DWrapper::D3DVertex> arrPts;
		CD3DWrapper::D3DVertex v;
		DWORD dcolor = D3DCOLOR_RGBA(GetRValue(color),GetGValue(color),GetBValue(color),255);

		v.z = 0;
		v.color = dcolor;

		v.x = x;
		v.y = y;
		arrPts.Add(v);
		
		while( ang<6.283 )
		{
			v.x = x + r*cos(ang);
			v.y = y + r*sin(ang);
			arrPts.Add(v);
			ang += step;
		}
		v.x = x + r;
		v.y = y;
		arrPts.Add(v);

		int npt = arrPts.GetSize();
		
		D3DPRIMITIVETYPE type = D3DPT_TRIANGLEFAN;
		int obj_count = npt-2;
		
		LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
		m_pD3D->m_pD3DDevice9->CreateVertexBuffer( npt*sizeof(CD3DWrapper::D3DVertex), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE,
			D3DPOOL_DEFAULT, &pVB, NULL );	
		
		if( pVB==NULL )
			return;
		
		CD3DWrapper::D3DVertex *pts = NULL;
		pVB->Lock(0, sizeof(CD3DWrapper::D3DVertex)*npt, (void**)&pts, D3DLOCK_DISCARD );
		
		if( pts==NULL )
		{
			pVB->Release();
			return;
		}
		
		memcpy(pts,arrPts.GetData(),sizeof(CD3DWrapper::D3DVertex)*npt);
		
		pVB->Unlock();	
		
		m_pD3D->m_pD3DDevice9->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
		
		m_pD3D->m_pD3DDevice9->SetStreamSource(0,pVB,0,sizeof(CD3DWrapper::D3DVertex));

		m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_pD3D->m_pD3DDevice9->DrawPrimitive(type,0,obj_count);	

		m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		m_pD3D->m_pD3DDevice9->DrawPrimitive(type,0,obj_count);		
		
		m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

		m_pD3D->m_pD3DDevice9->SetStreamSource(0,0,0,0);
		
		pVB->Release();

		return;
	}

	m_pD3D->m_pD3DDevice9->SetRenderState(D3DRS_POINTSIZE,*(DWORD*)&size);

	CD3DWrapper::D3DVertex* pVertices = NULL;
	m_pD3D->m_pPointList_VB->Lock( 0, sizeof(CD3DWrapper::D3DVertex), (void**)&pVertices, D3DLOCK_DISCARD );
    pVertices[0].x = x;
	pVertices[0].y = y;
	pVertices[0].z = 0;
	pVertices[0].color = D3DCOLOR_RGBA(GetRValue(color),GetGValue(color),GetBValue(color),255);
    m_pD3D->m_pPointList_VB->Unlock();

	m_pD3D->m_pD3DDevice9->SetStreamSource( 0, m_pD3D->m_pPointList_VB, 0, sizeof(CD3DWrapper::D3DVertex) );
	m_pD3D->m_pD3DDevice9->SetFVF( D3DFVF_MY_VERTEX );
	m_pD3D->m_pD3DDevice9->DrawPrimitive( D3DPT_POINTLIST, 0, 1 );

	m_pD3D->m_pD3DDevice9->SetStreamSource(0,0,0,0);
}

void CD3DDrawLineDC::SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle)
{
	// 将GDI模式的虚线模式转到GL下
	DWORD pattern = 0xFFFFFFFF;
	float factor = 1, fpart = 1;

	m_bUseStyle = FALSE;

	// 画虚线时
	if (dwStyleCount > 1)
	{
		DWORD nCount = 0;
		for (int i=0; i<dwStyleCount; i++)
		{
			nCount += lpStyle[i];
		}
		
		if (nCount <= 0)
		{
			goto END;
		}
		
		pattern = 0x00;
		
		if( nCount>32 )
		{
			factor = (float)nCount/32>0?(float)nCount/32:1;
			// 用16位表示,保持比例
			fpart = (float)32/nCount;
		}
		
		DWORD styles[8];
		// 16位用了多少位
		DWORD nCountUsed = 0;
		for (i=0; i<dwStyleCount; i++)
		{
			if (i == dwStyleCount-1)
			{
				styles[dwStyleCount-1] = 16 - nCountUsed;
				break;
			}
			styles[i] = lpStyle[i]*fpart;
			nCountUsed += styles[i];
		}
		
		// 给pattern赋值
		DWORD ustart = 0, uend = 0;
		for (i=0; i<dwStyleCount; i++)
		{
			ustart = uend;
			uend += styles[i];
			if (i%2 == 1) continue;
			
			//			GLushort uvalue = pow(2,styles[i]) - 1;
			//			GLint uvalue = 0x80000000;
			//			uvalue = uvalue<<styles[i];
			DWORD uvalue = 0xFFFFFFFF;
			// 将styles[i]个低位置为1
			uvalue = ~(uvalue<<styles[i]);
			// 移到正确的位置
			uvalue = uvalue<<ustart;
			
			pattern |= uvalue;
			
		}		
		m_bUseStyle = TRUE;
	}
END:
	m_pD3D->m_pLine->SetPattern(pattern);
	m_pD3D->m_pLine->SetPatternScale(factor);
//	::glEnable(GL_LINE_STIPPLE); //启动虚线模式（使用glDisable(GL_LINE_STIPPLE)可以关闭之）
//	::glLineStipple(factor,pattern);
}



void CD3DDrawLineDC::BeginTriangles(int num)
{
	COM_RELEASE(m_pD3D->m_pTriangleList_VB);
	
	m_pD3D->m_pD3DDevice9->CreateVertexBuffer( num*3*sizeof(CD3DWrapper::D3DVertex), 0, D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_DEFAULT, &m_pD3D->m_pTriangleList_VB, NULL );
	
	m_pD3D->m_pVertexForTriangleList = NULL;
	m_pD3D->m_pTriangleList_VB->Lock(0, sizeof(CD3DWrapper::D3DVertex)*3*num, (void**)&m_pD3D->m_pVertexForTriangleList, 0 );
	
	m_pD3D->m_nVertexForTriangleList = 0;
}


void CD3DDrawLineDC::Triangle(double x1, double y1, double x2, double y2, double x3, double y3, double z)
{
	DWORD color = D3DCOLOR_RGBA(GetRValue(cur_color),GetGValue(cur_color),GetBValue(cur_color),255-(cur_color>>24));
	CD3DWrapper::D3DVertex *p = m_pD3D->m_pVertexForTriangleList + m_pD3D->m_nVertexForTriangleList*3;
	
	p[0].x = x1; p[0].y = y1; p[0].z = z; p[0].color = color;
	p[1].x = x2; p[1].y = y2; p[1].z = z; p[1].color = color;
	p[2].x = x3; p[2].y = y3; p[2].z = z; p[2].color = color;
	m_pD3D->m_nVertexForTriangleList++;
}

void CD3DDrawLineDC::EndTriangles()
{
	m_pD3D->m_pTriangleList_VB->Unlock();
	
	m_pD3D->m_pD3DDevice9->SetStreamSource( 0, m_pD3D->m_pTriangleList_VB, 0, sizeof(CD3DWrapper::D3DVertex) );
	m_pD3D->m_pD3DDevice9->SetFVF( D3DFVF_MY_VERTEX );
	
	m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	HRESULT hr = m_pD3D->m_pD3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST,0,m_pD3D->m_nVertexForTriangleList);

	//m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	//m_pD3D->m_pD3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST,0,m_pD3D->m_nVertexForTriangleList);

	m_pD3D->m_pD3DDevice9->SetStreamSource(0,0,0,0);

}


void CD3DDrawLineDC::PolyPolygon(GrPt3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color)
{
	CD3DPolygon polygon;

	polygon.Init(m_pD3D);
	
	cur_color = color;

	polygon.BeginPolygon(cur_color);
	
	GrPt3D *p = pts;
	for( int j=0; j<nPolygon; j++)
	{
		polygon.BeginContour();			
		for( int i=0; i<pCounts[j]; i++, p++)
		{
			polygon.Vertex(p[0].x,p[0].y,p[0].z);
		}
		polygon.EndContour();
	}
	
	polygon.EndPolygon();
}

///////////////////////////////////////////////////////////////////////////////////////////////		  
///////////////////////     CSuperDrawDC   ////////////////////////////////////////////////////		
///////////////////////////////////////////////////////////////////////////////////////////////		  

#define CHECKVALID		if(m_pDC==NULL)return;

#define X0				CX(x)
#define Y0				CY(y)

CSuperDrawDC::CSuperDrawDC():m_szDC(0,0)
{
	m_pDC = NULL;
	m_nType = 0;
	m_bEnableUseOpSet = FALSE;
	m_pDrawOpSet = NULL;

	m_bInt = TRUE;
	m_bInverseY = FALSE;
	m_hDC = NULL;

	m_bStartBatchPoint = FALSE;
}

CSuperDrawDC::~CSuperDrawDC()
{
	DeleteDC();
}

void CSuperDrawDC::DeleteDC()
{
	if( m_pDC )
	{
		switch( m_nType )
		{
		case DRAWDC_GDI:
			delete (CGdiDrawingDC*)m_pDC;
			break;
		case DRAWDC_GL:
			delete (CGlDrawLineDC*)m_pDC;
			break;
		case DRAWDC_D3D:
			delete (CD3DDrawLineDC*)m_pDC;
			break;
		}
	}
}

int CSuperDrawDC::GetType()const
{
	return m_nType;
}

void CSuperDrawDC::CreateType(int type, HDC hdc, void *param)
{
	DeleteDC();

	switch( type )
	{
	case DRAWDC_GDI:
		m_pDC = new CGdiDrawingDC(hdc);
		break;
	case DRAWDC_GL:
		m_pDC = new CGlDrawLineDC();
		break;
	case DRAWDC_D3D:
		{
			CD3DDrawLineDC *pDC = new CD3DDrawLineDC();
			pDC->Init((CD3DWrapper*)param);
			m_pDC = pDC;
		}
		break;
	}

	m_hDC = hdc;
	m_nType = type;
}

CGdiDrawingDC *CSuperDrawDC::GetGDI()
{
	return ((CGdiDrawingDC*)m_pDC);
}

CGlDrawLineDC *CSuperDrawDC::GetGL()
{
	return ((CGlDrawLineDC*)m_pDC);
}

CD3DDrawLineDC *CSuperDrawDC::GetD3D()
{
	return ((CD3DDrawLineDC*)m_pDC);
}


void CSuperDrawDC::EnableUseOpSet(BOOL bEnable, CDrawOpSet *pOp)
{
	m_bEnableUseOpSet = bEnable;
	m_pDrawOpSet = pOp;
}


BOOL CSuperDrawDC::IsEnableUseOpSet()
{
	return m_bEnableUseOpSet;
}

//xor,copy，等等
void CSuperDrawDC::SetDrawMode(int mode)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->SetDrawMode(mode);
		return;
	}

	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->SetDrawMode(mode);
		break;
	case DRAWDC_GL:
		GetGL()->SetDrawMode(mode);
		break;
	case DRAWDC_D3D:
		GetD3D()->SetDrawMode(mode);
		break;
	}
}

void CSuperDrawDC::SetCoordMode(BOOL bInt, BOOL bInverseY, CSize szDC)
{
	m_bInt = bInt;
	m_bInverseY = bInverseY;
	m_szDC = szDC;

	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->SetEnvelope(TRUE,Envelope(0,m_szDC.cx,0,m_szDC.cy));
		break;
	case DRAWDC_GL:
		GetGL()->SetEnvelope(TRUE,Envelope(0,m_szDC.cx,0,m_szDC.cy));
		break;
	case DRAWDC_D3D:
		GetD3D()->SetEnvelope(TRUE,Envelope(0,m_szDC.cx,0,m_szDC.cy));
		break;
	}
}

float CSuperDrawDC::CX(double x)
{
	if( m_bInt )
		return PIXEL(x);
	else
		return (float)x;
}


float CSuperDrawDC::CY(double x)
{
	if( !m_bInverseY )
	{
		if( m_bInt )
			return PIXEL(x);
		else
			return (float)x;
	}
	else
	{
		if( m_bInt )
			return m_szDC.cy-PIXEL(x);
		else
			return m_szDC.cy-(float)x;
	}
}

BOOL CSuperDrawDC::CanBatchPoint()
{
	if( !m_bEnableUseOpSet && m_nType==DRAWDC_GL )
		return TRUE;

	return FALSE;
}


void CSuperDrawDC::StartBatchPoint(COLORREF color, float size, BOOL bFlat)
{
	m_bStartBatchPoint = TRUE;

	if( m_nType==DRAWDC_GL )
		GetGL()->BeginPoint(color,size,bFlat);
}


void CSuperDrawDC::BatchPoint(double x, double y, double z)
{
	if( m_nType==DRAWDC_GL )
		GetGL()->Point(X0,Y0,z);
}


void CSuperDrawDC::BatchPointColor(COLORREF color)
{
	if( m_nType==DRAWDC_GL )
		GetGL()->BatchPointColor(color);
}

void CSuperDrawDC::EndBatchPoint()
{
	if( m_nType==DRAWDC_GL )
		GetGL()->EndPoint();

	m_bStartBatchPoint = FALSE;
}

//画点
void CSuperDrawDC::Point(double x, double y, double z, COLORREF color, float size, BOOL bFlat)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->Point(x,y,z,color,size,bFlat);
		return;
	}

	if( m_bStartBatchPoint )
	{
		BatchPointColor(color);
		BatchPoint(x,y,z);
		return;
	}

	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->Point(X0,Y0,color,size,bFlat);
		break;
	case DRAWDC_GL:
		GetGL()->BeginPoint(color,size,bFlat);
		GetGL()->Point(X0,Y0,z);
		GetGL()->EndPoint();
		break;
	case DRAWDC_D3D:
		GetD3D()->Point(X0,Y0,color,size,bFlat);
		break;
	}
}


//画线
void CSuperDrawDC::Begin(int npt)
{
	npt += npt + 20;
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->Begin(npt);
		return;
	}
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->Begin(npt);
		break;
	case DRAWDC_GL:
		GetGL()->Begin(npt);
		break;
	case DRAWDC_D3D:
		GetD3D()->Begin(npt);
		break;
	}
}

void CSuperDrawDC::SetColor(COLORREF clr)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->SetColor(clr);
		return;
	}
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->SetColor(clr);
		break;
	case DRAWDC_GL:
		GetGL()->SetColor(clr);
		break;
	case DRAWDC_D3D:
		GetD3D()->SetColor(clr);
		break;
	}	
}

void CSuperDrawDC::SetWidth(float wid)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->SetWidth(wid);
		return;
	}
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->SetWidth(wid);
		break;
	case DRAWDC_GL:
		GetGL()->SetWidth(wid);
		break;
	case DRAWDC_D3D:
		GetD3D()->SetWidth(wid);
		break;
	}	
}

void CSuperDrawDC::MoveTo(double x, double y, double z)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->MoveTo(x,y,z);
		return;
	}
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->MoveTo(X0,Y0,z);
		break;
	case DRAWDC_GL:
		GetGL()->MoveTo(X0,Y0,z);
		break;
	case DRAWDC_D3D:
		GetD3D()->MoveTo(X0,Y0,z);
		break;
	}	
}

void CSuperDrawDC::LineTo(double x, double y, double z)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->LineTo(x,y,z);
		return;
	}
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->LineTo(X0,Y0,z);
		break;
	case DRAWDC_GL:
		GetGL()->LineTo(X0,Y0,z);
		break;
	case DRAWDC_D3D:
		GetD3D()->LineTo(X0,Y0,z);
		break;
	}	
}

void CSuperDrawDC::SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->SetExtPen(dwStyleCount,lpStyle);
		return;
	}
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->SetExtPen(dwStyleCount,lpStyle);
		break;
	case DRAWDC_GL:
		GetGL()->SetExtPen(dwStyleCount,lpStyle);
		break;
	case DRAWDC_D3D:
		GetD3D()->SetExtPen(dwStyleCount,lpStyle);
		break;
	}	
}

void CSuperDrawDC::End()
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->End();
		return;
	}
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->End();
		break;
	case DRAWDC_GL:
		GetGL()->End();
		break;
	case DRAWDC_D3D:
		GetD3D()->End();
		break;
	}
}


void CSuperDrawDC::Rectangle(double x1, double y1, double x2, double y2, double z)
{
	MoveTo(x1,y1,z);
	LineTo(x2,y1,z);
	LineTo(x2,y2,z);
	LineTo(x1,y2,z);
	LineTo(x1,y1,z);
}


void CSuperDrawDC::MarkBox(double x1, double y1, double x2, double y2, double z, BOOL bFill)
{
	if( bFill )
	{
		Point((x1+x2)*0.5,(y1+y2)*0.5,z,RGB(0,127,255),(x2-x1-0.5),TRUE);
	}

	MoveTo(x1,y1,z);
	LineTo(x2,y1,z);
	LineTo(x2,y2,z);
	LineTo(x1,y2,z);
	LineTo(x1,y1,z);
}


void CSuperDrawDC::DrawDiamond(double x, double y, double w, double z)
{
	MoveTo(x-w,y,z);
	LineTo(x,y+w,z);
	LineTo(x+w,y,z);
	LineTo(x,y-w,z);
	LineTo(x-w,y,z);
}


COLORREF CSuperDrawDC::GetColor()
{
	switch( m_nType )
	{
	case DRAWDC_GDI:
		return GetGDI()->GetColor();
	case DRAWDC_GL:
		return GetGL()->GetColor();
	case DRAWDC_D3D:
		return GetD3D()->GetColor();
	}

	return RGB(255,255,255);
}


//画面
void CSuperDrawDC::Polygon(PT_3D *pts, int npt, COLORREF color)
{
	PolyPolygon(pts,&npt,1,npt,color);
}

void CSuperDrawDC::PolyPolygon(PT_3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->PolyPolygon(pts,pCounts,nPolygon,npt,color);
		return;
	}
	
	GrPt3D *pts2 = new GrPt3D[npt];
	if( !pts2 )return;
	
	for( int i=0; i<npt; i++)
	{
		pts2[i].x = CX(pts[i].x);
		pts2[i].y = CY(pts[i].y);
		pts2[i].z = pts[i].z;
	}
	
	switch( m_nType )
	{
	case DRAWDC_GDI:
		GetGDI()->PolyPolygon(pts2,pCounts,nPolygon,npt,color);
		break;
	case DRAWDC_GL:
		GetGL()->PolyPolygon(pts2,pCounts,nPolygon,npt,color);
		break;
	case DRAWDC_D3D:
		GetD3D()->PolyPolygon(pts2,pCounts,nPolygon,npt,color);
		break;
	}
	
	delete[] pts2;
}


//文本
void CSuperDrawDC::SetTextSettings(TextSettings *pSet)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->SetTextSettings(pSet);
		return;
	}

	m_textSettings = *pSet;
}


void CSuperDrawDC::DrawText(LPCTSTR strText, double x, double y, COLORREF color, COLORREF bkColor, BOOL bSmooth, BOOL bOutLine)
{
	if( m_bEnableUseOpSet )
	{
		m_pDrawOpSet->DrawText(strText,x,y,color,bkColor,bSmooth,bOutLine);
		return;
	}

	switch( m_nType )
	{
	case DRAWDC_GDI:
		g_Fontlib.DrawText_GDIPlus(strText,X0,Y0,m_hDC,&m_textSettings,color,bkColor,bSmooth,bOutLine);
		break;
	case DRAWDC_GL:
		g_Fontlib.DrawText_GL(strText,X0,Y0,&m_textSettings,color,bSmooth,bOutLine);
		break;
	case DRAWDC_D3D:
		g_Fontlib.DrawText_D3D(strText,X0,Y0,m_hDC,GetD3D(),&m_textSettings,color,bSmooth,bOutLine);
		break;
	}
}


void CSuperDrawDC::DrawSet(CDrawOpSet *pOp)
{
	if( pOp==NULL )
		return;

	BOOL bsave = m_bEnableUseOpSet;
	m_bEnableUseOpSet = FALSE;
	
	int nsize = pOp->m_arrPItems.GetSize();
	CDrawOpSet::OpItem **buf = pOp->m_arrPItems.GetData();
	
	for( int i=0; i<nsize; i++)
	{
		CDrawOpSet::OpItem *pItem = buf[i];
		if( pItem->type==DRAWOP_POINT )
		{
			CDrawOpSet::OpPoint *p = (CDrawOpSet::OpPoint*)pItem;
			
			Point(p->x,p->y,p->z,p->color,p->width,p->bFlat);
		}
		else if( pItem->type==DRAWOP_LINE )
		{
			CDrawOpSet::OpLines *p = (CDrawOpSet::OpLines*)pItem;
			
			Begin(p->arrPts.GetSize());
			SetColor(p->color);
			SetWidth(p->width);
			SetExtPen(p->nstyle,p->styles);
			for( int j=0; j<p->arrPts.GetSize(); j++)
			{
				GrPt3D pt = p->arrPts[j];
				if( pt.pencode==0 )
				{
					MoveTo(pt.x,pt.y,pt.z);
				}
				else
				{
					LineTo(pt.x,pt.y,pt.z);
				}
			}
			End();
		}
		else if( pItem->type==DRAWOP_POLYGON )
		{
			CDrawOpSet::OpPolygons *p = (CDrawOpSet::OpPolygons*)pItem;

			PolyPolygon(p->arrPts.GetData(),p->arrCounts.GetData(),p->arrCounts.GetSize(),p->arrPts.GetSize(),p->color);
		}
		else if( pItem->type==DRAWOP_TEXT )
		{
			CDrawOpSet::OpText *p = (CDrawOpSet::OpText*)pItem;
			SetTextSettings(&p->settings);
			DrawText(p->strText,p->pt.x,p->pt.y,p->color,p->bkColor,p->bSmooth,p->bOutLine);
		}
	}

	m_bEnableUseOpSet = bsave;
}

MyNameSpaceEnd
