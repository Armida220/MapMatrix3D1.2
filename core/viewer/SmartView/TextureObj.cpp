// TextureObj.cpp: implementation of the CTextureObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glew.h"
#include "gl/glu.h"
#include "TextureObj.h"
#include "GPUTexture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void tex_object::bind()
{ 
	if(!valid) gen(); 
	glBindTexture(target, texture);
 }	

void tex_object::parameter(UINT pname, UINT i)
{ glTexParameteri(target, pname, i); }

void tex_object::parameter(UINT pname, float f)
{ glTexParameterf(target, pname, f); }

void tex_object::parameter(UINT pname, const int * ip)
{ GLint tip = *ip; glTexParameteriv(target, pname, &tip); }

void tex_object::parameter(UINT pname, float * fp)
{ GLfloat tfp = *fp; glTexParameterfv(target, pname, &tfp); }

void tex_object::enable() { glEnable(target); }
void tex_object::disable() { glDisable(target); }

void tex_object::del()
{
	if (valid)
	{
		glDeleteTextures(1, &texture); 
		valid = false;
	}
}
bool tex_object::is_valid() const { return valid; }

void tex_object::gen() 
{ 
	glGenTextures(1, &texture);
	valid=true;
 }

tex_object_2d::tex_object_2d() : tex_object(GL_TEXTURE_2D,true), width(0), height(0){}
tex_object_2d::~tex_object_2d(){}

void tex_object_2d::define(int w, int h, int format, void* pBits)
{
	width = w; height = h;
	glPixelStorei(GL_UNPACK_ROW_LENGTH,w);
	if( format==GL_BGRA_EXT )
		glTexImage2D(target, 0, 4, width, height, 0, format, GL_UNSIGNED_BYTE, pBits);
	else
		glTexImage2D(target, 0, 3, width, height, 0, format, GL_UNSIGNED_BYTE, pBits);
}

void tex_object_2d::setbits(int x, int y, int w, int h, void *pBits, int format)
{
	bind();
	glPixelStorei(GL_UNPACK_ROW_LENGTH,w);
	if( format==GL_BGRA_EXT )
		glTexSubImage2D(target,0,x,y,w,h,GL_BGRA_EXT,GL_UNSIGNED_BYTE,pBits);
	else
	glTexSubImage2D(target,0,x,y,w,h,GL_BGR_EXT,GL_UNSIGNED_BYTE,pBits);
}

void tex_object_2d::subcopy( int xoff, int yoff, int x, int y, int wid, int hei)
{
	bind();
	glCopyTexSubImage2D(target,0,xoff,yoff,x,y,wid,hei);
}


class CTempMemAllocate
{
public:
	CTempMemAllocate(){
		m_pBuf = NULL;
		m_nSize = 0;
	}
	~CTempMemAllocate(){
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

CTempMemAllocate gMem;


/*
  x,y 是hBmp中有效数据的偏移坐标
  xTar,yTar 是纹理的坐标
*/
BOOL tex_image::bmptotexture(HBITMAP hBmp,int x,int y,
				  int xTar,int yTar,int nTarWidth,int nTarHeight,int format)
{
	if( hBmp==NULL ) return FALSE;
	if( nTarWidth<=0 || nTarHeight<=0 )return FALSE;

	tex_image *obj = this;
	
	CRect rectTar(xTar,yTar,xTar+nTarWidth,yTar+nTarHeight);
	CRect rectTex(0,0,obj->width,obj->height);
	CRect rectin;
	
	rectin = rectTar&rectTex;
	if( rectin!=rectTar )
	{
		x += (rectin.left-rectTar.left);
		y += (rectin.top-rectTar.top);
		xTar = rectin.left;
		yTar = rectin.top;
		nTarWidth = rectin.Width();
		nTarHeight = rectin.Height();
	}
	
	if( x<0 || y<0 || nTarWidth<=0 || nTarHeight<=0 )
	{
		return FALSE;
	}
	
	{
		BITMAP src;	
		::GetObject( hBmp , sizeof(BITMAP), &src );	
		BYTE* pSrc =(BYTE*)src.bmBits;
		if( pSrc==NULL )return FALSE;
		
		int bitcount = src.bmBitsPixel/8;
		int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
		linesize =  ((linesize+3)&(~3));
		pSrc = pSrc+ (src.bmHeight-1-y)*linesize + x*bitcount;

		BYTE *pNewSrc = gMem.Allocate(linesize*src.bmHeight);
		BYTE *p1 = pNewSrc, *p2 = pSrc;
		for( int i=0; i<src.bmHeight; i++)
		{
			memcpy(p1,p2,linesize);
			p1 += linesize;
			p2 -= linesize;
		}
		pSrc = pNewSrc;

		glPixelStorei(GL_UNPACK_ROW_LENGTH,src.bmWidth);	
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
		glPixelZoom(1.0,1.0);
	
		obj->setbits(xTar,yTar,
			nTarWidth,nTarHeight,pSrc,format);
	}

	return TRUE;
}



void tex_image::setviewbmp(HBITMAP hBmp)
{	
	filled = true;

	bmptotexture(hBmp,0,0,0,0,width,height,GL_BGR_EXT);
}


void tex_image::setviewbmp_BGRA(HBITMAP hBmp)
{
	filled = true;

	if( hBmp==NULL ) return;
	
	{
		BITMAP src;	
		::GetObject( hBmp , sizeof(BITMAP), &src );	
		BYTE* pSrc =(BYTE*)src.bmBits;
		if( pSrc==NULL )return;
		
		int bitcount = src.bmBitsPixel/8;
		int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
		linesize =  ((linesize+3)&(~3));
		pSrc = pSrc+ (src.bmHeight-height)*linesize;
		
		BYTE *pNewSrc = gMem.Allocate(linesize*src.bmHeight);
		BYTE *p1 = pNewSrc, *p2 = pSrc+(src.bmHeight-1)*linesize;
		for( int i=0; i<src.bmHeight; i++)
		{
			memcpy(p1,p2,linesize);
			p1 += linesize;
			p2 -= linesize;
		}
		pSrc = pNewSrc+ (src.bmHeight-height)*linesize;
		
		glPixelStorei(GL_UNPACK_ROW_LENGTH,src.bmWidth);	
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
		glPixelZoom(1.0,1.0);

		bind();
		glTexSubImage2D(target,0,0,0,width,height,GL_BGRA_EXT,GL_UNSIGNED_BYTE,pSrc);
	}
	
	return;
}

#include "GlobalFunc.h"
BOOL tex_image::copyToBmp(HBITMAP hBmp)
{
	if( hBmp==NULL ) return FALSE;
	
	BITMAP src;	
	if( ::GetObject( hBmp , sizeof(BITMAP), &src )==0 )
		return FALSE;
	
	if( src.bmBitsPixel<24 )
		return FALSE;
	
	BYTE* pSrc0 =(BYTE*)src.bmBits, *pSrc;
	if( pSrc0==NULL )return FALSE;
	
	int pixelSize = (src.bmBitsPixel>>3);
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));
	pSrc = pSrc0;

	BYTE *pNewSrc = gMem.Allocate(linesize*src.bmHeight);
	if( !pNewSrc )
		return FALSE;

	bind();

	glPixelStorei(GL_PACK_ROW_LENGTH,src.bmWidth);	
	glPixelStorei(GL_PACK_SKIP_PIXELS,0);
	glPixelStorei(GL_PACK_SKIP_ROWS,0);
	glGetTexImage(target,0,pixelSize==4?GL_BGRA_EXT:GL_BGR_EXT,GL_UNSIGNED_BYTE,pNewSrc);

	BYTE *p1 = pNewSrc, *p2 = pSrc+(src.bmHeight-1)*linesize;
	for( int i=0; i<src.bmHeight; i++)
	{
		memcpy(p2,p1,linesize);
		p1 += linesize;
		p2 -= linesize;
	}

//	WriteToFile(hBmp);

	return TRUE;	
}


void tex_image::display(double x[4], double y[4], BOOL bLinear, BOOL bGPU)
{
	bind();

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (float)GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (float)GL_CLAMP_TO_EDGE );

	if( bLinear || bGPU )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}
	
	if( bGPU )
	{		
		GPU_EnableTexture(true);
		GPU_SetImageAttrib(width, 0 );
	}

	::glLogicOp(GL_COPY);

	glBegin(GL_QUADS);
	glColor3f(1.0,1.0,1.0);
	glTexCoord2d(0.0,0.0);glVertex3d(x[0],y[0],TEXTURE_DEPTH);
	glTexCoord2d(1.0,0.0);glVertex3d(x[1],y[1],TEXTURE_DEPTH); 
	glTexCoord2d(1.0,1.0);glVertex3d(x[2],y[2],TEXTURE_DEPTH); 
	glTexCoord2d(0.0,1.0);glVertex3d(x[3],y[3],TEXTURE_DEPTH);

	glEnd();

	if( bGPU )GPU_EnableTexture(false);
}


void tex_image::display_alpha(double x[4], double y[4])
{
	bind();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
	
	glColor4f(1.0,1.0,1.0,1.0);	
	
	glBegin(GL_QUADS);	
	glTexCoord2d(0.0,0.0);glVertex3d(x[0],y[0],TEXTURE_DEPTH);
	glTexCoord2d(1.0,0.0);glVertex3d(x[1],y[1],TEXTURE_DEPTH); 
	glTexCoord2d(1.0,1.0);glVertex3d(x[2],y[2],TEXTURE_DEPTH); 
	glTexCoord2d(0.0,1.0);glVertex3d(x[3],y[3],TEXTURE_DEPTH);
	
	glEnd();
}

void tex_image::display(double x0[4], double y0[4],double x[4], double y[4], BOOL bLinear, BOOL bGPU)
{
	bind();
	
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (float)GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (float)GL_CLAMP_TO_EDGE );
	
	if( bLinear || bGPU )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}
	
	if( bGPU )
	{		
		GPU_EnableTexture(true);
		GPU_SetImageAttrib(width, 0 );
	}
	
	::glLogicOp(GL_COPY);
	
	glBegin(GL_QUADS);
	glColor3f(1.0,1.0,1.0);
	glTexCoord2d(x0[0],y0[0]);glVertex3d(x[0],y[0],TEXTURE_DEPTH);
	glTexCoord2d(x0[1],y0[1]);glVertex3d(x[1],y[1],TEXTURE_DEPTH); 
	glTexCoord2d(x0[2],y0[2]);glVertex3d(x[2],y[2],TEXTURE_DEPTH); 
	glTexCoord2d(x0[3],y0[3]);glVertex3d(x[3],y[3],TEXTURE_DEPTH);
	
	glEnd();

	if( bGPU )GPU_EnableTexture(false);
}


void tex_image::cutto(tex_image& a)
{
	if( a.manageObjects && valid>0 )
		return;

	a.manageObjects = manageObjects;
	a.target = target;
	a.texture = texture;
	a.filled = filled;
	a.width = width;
	a.height = height;
	a.valid = valid;

	manageObjects = false;
	texture = 0;
	valid = false;
	filled = false;
}