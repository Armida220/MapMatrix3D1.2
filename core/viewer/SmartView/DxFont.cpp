// DxFont.cpp: implementation of the CDxFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glew.h"
#include "DxFont.h"
#include "PlotChar.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxFont::CDxFont()
{
	m_nListBase = -1;
	
	m_hFont = NULL;
	m_hFontOld = NULL;
	memset(&m_CurChar,0,sizeof(CharBuf));
}

CDxFont::~CDxFont()
{
	if(m_hFont) ::DeleteObject(m_hFont);
	if( m_nListBase>0 )
	{
		glDeleteLists(m_nListBase,m_arrBufs.GetSize());
	}
}


void CDxFont::SetBufSize(int size)
{
	if( size<=0 || size<=m_arrBufs.GetSize() )
		return;
/*
	if( m_nListBase>0 )glDeleteLists(m_nListBase,m_arrBufs.GetSize());
	m_arrBufs.SetSize(size);
	memset(m_arrBufs.GetData(),0,sizeof(CharBuf)*size);
	
	m_nListBase = glGenLists(size);

	m_arrUsedFlag.SetSize(size);
	memset(m_arrUsedFlag.GetData(),0,sizeof(int)*size);
*/
}

BOOL CDxFont::SetFont(TCHAR *chName,DWORD charset,DWORD bItalic,DWORD bUnderline,DWORD bStrikeOut,DWORD align)
{
	_tcscpy(m_CurChar.font,chName);
	m_CurChar.charset = charset;
	m_CurChar.bItalic = bItalic;
	m_CurChar.bUnderline = bUnderline;
	m_CurChar.bStrikeOut = bStrikeOut;
	m_CurChar.align = align;

	if( m_hFont!=NULL )DeleteObject(m_hFont);

	m_hFont = CreateFont(100,0,0,0,400,
		m_CurChar.bItalic,m_CurChar.bUnderline,m_CurChar.bStrikeOut,m_CurChar.charset,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_MODERN,m_CurChar.font);

	return TRUE;
}


void CDxFont::Render(LPCTSTR text,double lfX,double lfY,double lfSize,double lfAngle,int aligntype)
{
	DWORD dwText = 0;
	LPCTSTR p = text;

	if( p==NULL )return;
	int index = 0;

	HDC hdc = wglGetCurrentDC();
	m_hFontOld = (HFONT)SelectObject(hdc,m_hFont);

	double xoff = 0, yoff = 0;
	switch(aligntype)
	{
	case TAH_LEFT:
	case TAH_LEFT|TAV_TOP:
		yoff -= lfSize;
		break;
	case TAH_MID:
	case TAH_MID|TAV_TOP:
		xoff -= (lfSize*0.5*_tcslen(text))/2;
		yoff -= (lfSize);
		break;
	case TAH_RIGHT:
	case TAH_RIGHT|TAV_TOP:
		xoff -= (lfSize*0.5*_tcslen(text));
		yoff -= (lfSize);
		break;
	case TAH_LEFT|TAV_MID:
	case TAV_MID:				
		yoff -= (lfSize)/2;
		break;
	case TAH_MID|TAV_MID:
		xoff -= (lfSize*0.5*_tcslen(text))/2;
		yoff -= (lfSize)/2;
		break;
	case TAH_RIGHT|TAV_MID:	
		xoff -= (lfSize*0.5*_tcslen(text));
		yoff -= (lfSize)/2;
		break;
	case TAH_LEFT|TAV_BOTTOM:
	case TAV_BOTTOM:				
		break;
	case TAH_MID|TAV_BOTTOM:
		xoff -= (lfSize*0.5*_tcslen(text))/2;
		break;
	case TAH_RIGHT|TAV_BOTTOM:
		xoff -= (lfSize*0.5*_tcslen(text));							
		break;
	default:
		yoff -= lfSize;
		break;
	}

	glMatrixMode(GL_MODELVIEW_MATRIX);

	while( *p!=0 )
	{
		glPushMatrix();
		
		glTranslated(lfX,lfY,0);
		glRotated(lfAngle*180/PI,0,0,1);
		glTranslated(lfSize*index*(IsDBCSLeadByte(p[0])?1:0.5)+xoff,yoff,0);
		glScaled(lfSize*0.01,lfSize*0.01,1);
		
		if( IsDBCSLeadByte(p[0]) )
		{
			DrawChar(hdc,lfSize>40,p[0],p[1],1,1);
			p += 2;
		}
		else
		{
			DrawChar(hdc,lfSize>40,p[0],0,1,1);
			p++;
		}

		glPopMatrix();

		index++;
	}

	SelectObject(hdc,m_hFontOld);
}

void CDxFont::DrawChar(HDC hdc, BOOL bPlotSpline, char a1, char a2, double kx, double ky)
{
	CPlotChar plot;
	plot.m_bPlotSpline = bPlotSpline;

	if( a1>0 )
		plot.SetEnChar(a1);
	else
		plot.SetCnChar(a1,a2);

	CArray<GrVertex,GrVertex> pts;
	plot.Plot(&pts,CDC::FromHandle(hdc));

	if( pts.GetSize()<=0 )
		return;

	//面方式
	if( bPlotSpline )
	{
		GrVertex *data = pts.GetData();
		GrVertex *p = data;

		CDxPolygon polygon;
		polygon.BeginPolygon(m_color/*RGB(255,255,255)*/);

		int npts = pts.GetSize(), nPos = 0;

		for( int i=0; i<npts; i++, p++)
		{
			if( (i==(npts-1)||p->code==GRBUFFER_PTCODE_MOVETO) )
			{
				GrVertex *p2 = p-1;
//				if( fabs(p2->x-p->x)<1e-4 && fabs(p2->y-p->y)<1e-4 )
//					continue;

				if( (i-nPos)>=3 )
				{
					polygon.BeginContour();

					p2 = data+nPos;
					for( ; p2<p; p2++)
					{
						polygon.Vertex(p2->x*kx,p2->y*ky,0);
					}

					polygon.EndContour();
				}
				nPos = i;
			}
		}
		
		polygon.EndPolygon();
	}
	else
	{
		GrVertex *data = pts.GetData();
		GrVertex *p = data;
		
		int npts = pts.GetSize();
		
		glBegin(GL_LINES);
		for( int i=0; i<npts; i++, p++)
		{
			if( p->code==GRBUFFER_PTCODE_LINETO )
			{
				glVertex3d((p-1)->x*kx,(p-1)->y*ky,VERTEX_DEPTH);
				glVertex3d(p->x*kx,p->y*ky,VERTEX_DEPTH);
			}
		}
		glEnd();
	}
}

int CDxFont::FindChar(DWORD dwText)
{
	CharBuf *buf = m_arrBufs.GetData();
	int nSize = m_arrBufs.GetSize();
	CharBuf *p = buf;

	//是否存在字符缓存
	for( int i=0; i<nSize; i++,p++)
	{
		if( p->ch==dwText )
		{
			if( p->charset==m_CurChar.charset && 
				p->bItalic==m_CurChar.bItalic &&
				p->bUnderline==m_CurChar.bUnderline &&
				p->bStrikeOut==m_CurChar.bStrikeOut &&
				p->align==m_CurChar.align &&
				_tcsicmp(p->font,m_CurChar.font)==0 )
			{
				return p->nGLListIdx;
			}

			break;
		}
	}

	//新字符的位置
	int nReplacePos = 0;

	//新列表的编号
	int nNewListIdx = 0;

	//存在，但是字体不一样，需要重新生成
	if( i<nSize )
	{
		nReplacePos = i;
		nNewListIdx = p->nGLListIdx;
	}
	else
	{
		//首先查找是否存在未使用的显示列表
		int *flags = m_arrUsedFlag.GetData();
		int nFlags = m_arrUsedFlag.GetSize();
		for( i=0; i<nFlags; i++)
		{
			if( flags[i]==0 )
				break;
		}

		//存在未使用的显示列表
		if( i<nFlags )
		{
			nReplacePos = nSize-1;
			nNewListIdx = m_nListBase+i;

			flags[i] = 1;
		}
		//不存在，就将最后一个缓存项替换掉
		else
		{
			nReplacePos = nSize-1;
			nNewListIdx = buf[nSize-1].nGLListIdx;
		}
	}

	HDC hdc = wglGetCurrentDC();
	::SetTextAlign(hdc,m_CurChar.align);
	
	m_hFontOld = (HFONT)SelectObject(hdc,m_hFont);
	
	glFlush();
	wglUseFontOutlines(hdc,dwText,1,nNewListIdx,0.002f,0,WGL_FONT_POLYGONS,NULL);
	glFlush();
	
	SelectObject(hdc,m_hFontOld);
	
	CharBuf item = m_CurChar;
	item.ch = dwText;
	item.nGLListIdx = nNewListIdx;
	
	m_arrBufs.RemoveAt(nReplacePos);
	m_arrBufs.InsertAt(0,item);

	
	return nNewListIdx;
}



//////////////////////////////////////////////////////////////////////////
//static member used to keep track of newly allocated vertices
CPtrList CDxPolygon::gm_VertexPtrList ; 

CDxPolygon::CDxPolygon()
{
	m_pObj = NULL;
}


CDxPolygon::~CDxPolygon()
{
	if( m_pObj!=NULL )gluDeleteTess( m_pObj );
	POSITION pos = gm_VertexPtrList.GetHeadPosition();
	while( pos!=NULL )
	{
		GLdouble *p = (GLdouble*)gm_VertexPtrList.GetNext(pos);
		delete[] p;
	}
	gm_VertexPtrList.RemoveAll();
}


void CDxPolygon::BeginPolygon(COLORREF color)
{
	if( m_pObj!=NULL )gluDeleteTess( m_pObj );
	m_pObj = gluNewTess();

	::glColor4f(GetRValue(color)/255.0,GetGValue(color)/255.0,GetBValue(color)/255.0,1.0 - (color>>24)/255.0);

	gluTessProperty(m_pObj, GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_ODD );

	gluTessCallback(m_pObj, GLU_TESS_BEGIN, (void (CALLBACK *) ())beginCallback);
	gluTessCallback(m_pObj, GLU_TESS_VERTEX, (void (CALLBACK *) ()) vertexCallback);
	gluTessCallback(m_pObj, GLU_TESS_END, (void (CALLBACK *) ())endCallback);
	gluTessCallback(m_pObj, GLU_TESS_ERROR,(void (CALLBACK *) ())errorCallback);
	gluTessCallback(m_pObj, GLU_TESS_COMBINE, (void (CALLBACK *) ())combineCallback);

	gluTessBeginPolygon(m_pObj,NULL);
}


void CDxPolygon::BeginContour()
{
	if( m_pObj!=NULL )
		gluTessBeginContour(m_pObj);
}


void CDxPolygon::Vertex(double x, double y, double z)
{
	GLdouble *vertex = new GLdouble[3];
	vertex[0] = x; vertex[1] = y; vertex[2] = z;
	gm_VertexPtrList.AddTail(vertex);

	gluTessVertex( m_pObj, vertex, vertex ) ;
}


void CDxPolygon::EndPolygon()
{
	gluTessEndPolygon(m_pObj);

	gluTessCallback(m_pObj, GLU_TESS_BEGIN, NULL);
	gluTessCallback(m_pObj, GLU_TESS_VERTEX, NULL);
	gluTessCallback(m_pObj, GLU_TESS_END, NULL);
	gluTessCallback(m_pObj, GLU_TESS_ERROR,NULL);
	gluTessCallback(m_pObj, GLU_TESS_COMBINE, NULL);
}

void CDxPolygon::EndContour()
{
	if( m_pObj!=NULL )
		gluTessEndContour(m_pObj);
}


void CALLBACK CDxPolygon::beginCallback(int which)
{
	glBegin(which);
}

void CALLBACK CDxPolygon::errorCallback(int errorCode)
{
	const GLubyte *estring;
	
	estring = gluErrorString(errorCode);
}

void CALLBACK CDxPolygon::endCallback(void)
{
	glEnd();
}

void CALLBACK CDxPolygon::vertexCallback(void *vertex)
{
	GLdouble *pointer;
	
	pointer = (GLdouble *) vertex;
	glVertex3dv(pointer);
}

/*  combineCallback is used to create a new vertex when edges
*  intersect.  coordinate location is trivial to calculate,
*  but weight[4] may be used to average color, normal, or texture
*  coordinate data.  In this program, color is weighted.
*/
void CALLBACK CDxPolygon::combineCallback(double coords[3], 
											double *vertex_data[4],
											double weight[4], double **dataOut )
{
	GLdouble *vertex = new GLdouble[6] ;
	gm_VertexPtrList.AddTail( vertex ) ; //keep track for later delete[] at bottom of CDxPolygon::OnDraw() 
	
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	vertex[3] = vertex[4] = vertex[5] = 0. ; //01/13/05 bugfix
	
	*dataOut = vertex;
	//   TRACE( "combine: returning (%f,%f,%f,%f,%f,%f)\n",vertex[0],vertex[1],vertex[2],vertex[3],vertex[4],vertex[5] ) ;
}

MyNameSpaceEnd
