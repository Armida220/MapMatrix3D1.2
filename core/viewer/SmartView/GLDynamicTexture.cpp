// GLDynamicTexture.cpp: implementation of the GLDynamicTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glew.h"
#include "GLDynamicTexture.h"
#include <glew.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

using std::string;
using std::stringstream;
using std::vector;
using std::cout;
using std::endl;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

// struct variable to store OpenGL info
struct glInfo
{
    std::string vendor;
    std::string renderer;
    std::string version;
    std::vector <std::string> extensions;
    int redBits;
    int greenBits;
    int blueBits;
    int alphaBits;
    int depthBits;
    int stencilBits;
    int maxTextureSize;
    int maxLights;
    int maxAttribStacks;
    int maxModelViewStacks;
    int maxProjectionStacks;
    int maxClipPlanes;
    int maxTextureStacks;
	
    // ctor, init all members
    glInfo() : redBits(0), greenBits(0), blueBits(0), alphaBits(0), depthBits(0),
		stencilBits(0), maxTextureSize(0), maxLights(0), maxAttribStacks(0),
		maxModelViewStacks(0), maxClipPlanes(0), maxTextureStacks(0) {}
	
    bool getInfo();                             // extract info
    void printSelf();                           // print itself
    bool isExtensionSupported(const char* ext); // check if a extension is supported
};


///////////////////////////////////////////////////////////////////////////////
// extract openGL info
// This function must be called after GL rendering context opened.
///////////////////////////////////////////////////////////////////////////////
bool glInfo::getInfo()
{
    char* str = 0;
    char* tok = 0;

    // get vendor string
    str = (char*)glGetString(GL_VENDOR);
    if(str) this->vendor = str;                  // check NULL return value
    else return false;

    // get renderer string
    str = (char*)glGetString(GL_RENDERER);
    if(str) this->renderer = str;                // check NULL return value
    else return false;

    // get version string
    str = (char*)glGetString(GL_VERSION);
    if(str) this->version = str;                 // check NULL return value
    else return false;

    // get all extensions as a string
    str = (char*)glGetString(GL_EXTENSIONS);

    // split extensions
    if(str)
    {
        tok = strtok((char*)str, " ");
        while(tok)
        {
            this->extensions.push_back(tok);    // put a extension into struct
            tok = strtok(0, " ");               // next token
        }
    }
    else
    {
        return false;
    }

    // sort extension by alphabetical order
    std::sort(this->extensions.begin(), this->extensions.end());

    // get number of color bits
    glGetIntegerv(GL_RED_BITS, &this->redBits);
    glGetIntegerv(GL_GREEN_BITS, &this->greenBits);
    glGetIntegerv(GL_BLUE_BITS, &this->blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &this->alphaBits);

    // get depth bits
    glGetIntegerv(GL_DEPTH_BITS, &this->depthBits);

    // get stecil bits
    glGetIntegerv(GL_STENCIL_BITS, &this->stencilBits);

    // get max number of lights allowed
    glGetIntegerv(GL_MAX_LIGHTS, &this->maxLights);

    // get max texture resolution
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->maxTextureSize);

    // get max number of clipping planes
    glGetIntegerv(GL_MAX_CLIP_PLANES, &this->maxClipPlanes);

    // get max modelview and projection matrix stacks
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &this->maxModelViewStacks);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &this->maxProjectionStacks);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &this->maxAttribStacks);

    // get max texture stacks
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &this->maxTextureStacks);

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// check if the video card support a certain extension
///////////////////////////////////////////////////////////////////////////////
bool glInfo::isExtensionSupported(const char* ext)
{
    // search corresponding extension
    std::vector< string >::const_iterator iter = this->extensions.begin();
    std::vector< string >::const_iterator endIter = this->extensions.end();

    while(iter != endIter)
    {
        if(ext == *iter)
            return true;
        else
            ++iter;
    }
    return false;
}



///////////////////////////////////////////////////////////////////////////////
// print OpenGL info to screen and save to a file
///////////////////////////////////////////////////////////////////////////////
void glInfo::printSelf()
{
    stringstream ss;

    ss << endl; // blank line
    ss << "OpenGL Driver Info" << endl;
    ss << "==================" << endl;
    ss << "Vendor: " << this->vendor << endl;
    ss << "Version: " << this->version << endl;
    ss << "Renderer: " << this->renderer << endl;

    ss << endl;
    ss << "Color Bits(R,G,B,A): (" << this->redBits << ", " << this->greenBits
       << ", " << this->blueBits << ", " << this->alphaBits << ")\n";
    ss << "Depth Bits: " << this->depthBits << endl;
    ss << "Stencil Bits: " << this->stencilBits << endl;

    ss << endl;
    ss << "Max Texture Size: " << this->maxTextureSize << "x" << this->maxTextureSize << endl;
    ss << "Max Lights: " << this->maxLights << endl;
    ss << "Max Clip Planes: " << this->maxClipPlanes << endl;
    ss << "Max Modelview Matrix Stacks: " << this->maxModelViewStacks << endl;
    ss << "Max Projection Matrix Stacks: " << this->maxProjectionStacks << endl;
    ss << "Max Attribute Stacks: " << this->maxAttribStacks << endl;
    ss << "Max Texture Stacks: " << this->maxTextureStacks << endl;

    ss << endl;
    ss << "Total Number of Extensions: " << this->extensions.size() << endl;
    ss << "==============================" << endl;
    for(unsigned int i = 0; i < this->extensions.size(); ++i)
        ss << this->extensions.at(i) << endl;

    ss << "======================================================================" << endl;

    cout << ss.str() << endl;
}


bool CheckFBOSupported()
{
    // get OpenGL info
    glInfo glInfo;
    glInfo.getInfo();
    glInfo.printSelf();
	
    // check if FBO is supported by your video card
    if(glInfo.isExtensionSupported("GL_EXT_framebuffer_object"))
    {
        // get pointers to GL functions
        glGenFramebuffersEXT                     = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
        glDeleteFramebuffersEXT                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
        glBindFramebufferEXT                     = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
        glCheckFramebufferStatusEXT              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
        glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
        glGenerateMipmapEXT                      = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");
        glFramebufferTexture2DEXT                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
        glFramebufferRenderbufferEXT             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
        glGenRenderbuffersEXT                    = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
        glDeleteRenderbuffersEXT                 = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
        glBindRenderbufferEXT                    = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
        glRenderbufferStorageEXT                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
        glGetRenderbufferParameterivEXT          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
        glIsRenderbufferEXT                      = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
		
        // check once again FBO extension
        if(glGenFramebuffersEXT && glDeleteFramebuffersEXT && glBindFramebufferEXT && glCheckFramebufferStatusEXT &&
			glGetFramebufferAttachmentParameterivEXT && glGenerateMipmapEXT && glFramebufferTexture2DEXT && glFramebufferRenderbufferEXT &&
			glGenRenderbuffersEXT && glDeleteRenderbuffersEXT && glBindRenderbufferEXT && glRenderbufferStorageEXT &&
			glGetRenderbufferParameterivEXT && glIsRenderbufferEXT)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

	return false;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLDynamicTexture::CGLDynamicTexture()
{
	m_szTexture = CSize(0,0);
	m_fboID = 0;                 
	m_rboID = 0;                 
	m_texID1 = 0;
	m_texID2 = 0; 
	m_bCreateFBO = FALSE;

	m_nInRendingTexture = 0;

	m_szStencil = CSize(0,0);
}

CGLDynamicTexture::~CGLDynamicTexture()
{

}

void CGLDynamicTexture::Clear()
{
	if( m_texID1!=0 )
		glDeleteTextures(1, &m_texID1);
	if( m_texID2!=0 )
		glDeleteTextures(1, &m_texID2);
	
    // clean up FBO, RBO
	if( m_rboID!=0 )
		glDeleteRenderbuffersEXT(1, &m_rboID);
    if( m_fboID!=0 )
		glDeleteFramebuffersEXT(1, &m_fboID);
	
	m_szTexture = CSize(0,0);
	m_fboID = 0;                 
	m_rboID = 0;       
	m_rboID2 = 0;
	m_texID1 = 0;
	m_texID2 = 0; 

	m_nInRendingTexture = 0;
}

static int To2Exp(int a)
{
	for( int i=0; i<32; i++)
	{
		if( a<=(1<<i) )return (1<<i);
	}
	return 256;
}



///////////////////////////////////////////////////////////////////////////////
// check FBO completeness
///////////////////////////////////////////////////////////////////////////////
bool checkFramebufferStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        std::cout << "Framebuffer complete." << std::endl;
        return true;
		
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
        return false;
		
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
        return false;
		
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
        return false;
		
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
        return false;
		
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
        return false;
		
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
        return false;
		
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        std::cout << "[ERROR] Unsupported by FBO implementation." << std::endl;
        return false;
		
    default:
        std::cout << "[ERROR] Unknow error." << std::endl;
        return false;
    }
}

BOOL CALLBACK MonitorsCallBack( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{
	CSize *psz = (CSize*)dwData;

	MONITORINFO info;
	info.cbSize = sizeof( info );
	
	GetMonitorInfo(hMonitor,&info);
	
	int cx = info.rcWork.right-info.rcWork.left;
	int cy = info.rcWork.bottom-info.rcWork.top;
	if( cx>psz->cx )psz->cx = cx;
	if( cy>psz->cy )psz->cy = cy;
	
	return TRUE;
}


BOOL GetMaxScreenSize(int& cx, int& cy)
{
	CSize sz = CSize(0,0);
	::EnumDisplayMonitors( NULL, NULL, MonitorsCallBack, (LPARAM)&sz );

	if( cx<sz.cx )cx = sz.cx;
	if( cy<sz.cy )cy = sz.cy;

	return TRUE;
}


void GetMaxScreenSize2(int& cx, int& cy)
{
	GetMaxScreenSize(cx, cy);

	cx = To2Exp(cx);
	cy = To2Exp(cy);
}


BOOL CGLDynamicTexture::CreateFBO(int cx, int cy)
{
	if( m_bCreateFBO )
		return TRUE;

	GetMaxScreenSize(cx,cy);
	cy /= 2;
	cx = To2Exp(cx);
	cy = To2Exp(cy);

	if( cx>2048 )cx = 2048;
	if( cy>512 )cy = 512;

	if( cx==m_szTexture.cx && cy==m_szTexture.cy )
		return TRUE;
	
	glewInit();
	
	Clear();
	m_bCreateFBO = FALSE;

	CreateTwoDynamicTextures(cx,cy);
	
	// create a framebuffer object, you need to delete them when program exits.
	glGenFramebuffersEXT(1, &m_fboID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
	
	// create a renderbuffer object to store depth info
	// NOTE: A depth renderable image should be attached the FBO for depth test.
	// If we don't attach a depth renderable image to the FBO, then
	// the rendering output will be corrupted because of missing depth test.
	// If you also need stencil test for your rendering, then you must
	// attach additional image to the stencil attachement point, too.
	glGenRenderbuffersEXT(1, &m_rboID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_rboID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, cx, cy);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	
	// attach a texture to FBO color attachement point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texID1, 0);
	
	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_rboID);
	
	//@ disable color buffer if you don't attach any color buffer image,
	//@ for example, rendering depth buffer only to a texture.
	//@ Otherwise, glCheckFramebufferStatusEXT will not be complete.
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	m_bCreateFBO = TRUE;
	
	// check FBO status
	bool status = checkFramebufferStatus();
	if(!status)
		m_bCreateFBO = FALSE;

	if( m_bCreateFBO )
		m_szTexture = CSize(cx,cy);

	return m_bCreateFBO;
}


BOOL CGLDynamicTexture::CreateTwoDynamicTextures(int cx, int cy)
{
	glGenTextures(1, &m_texID1);
	glGenTextures(1, &m_texID2);

    glBindTexture(GL_TEXTURE_2D, m_texID1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cx, cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, m_texID2);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cx, cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

	return TRUE;
}


BOOL CGLDynamicTexture::BeginRenderToTexture(int idx)
{
	if( !m_bCreateFBO )return FALSE;
	if( m_nInRendingTexture==(idx+1) )return TRUE;

	if( m_nInRendingTexture )
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 
			(idx==0?m_texID1:m_texID2), 0);
	}
	else
	{
		// set the rendering destination to FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
		
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 
			(idx==0?m_texID1:m_texID2), 0);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
	}

	m_nInRendingTexture = (idx+1);

    // adjust viewport and projection matrix to texture dimension
    glViewport(0, 0, m_szTexture.cx, m_szTexture.cy);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, m_szTexture.cx, 0, m_szTexture.cy);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.375,0.375,0.0);

	glClear(GL_COLOR_BUFFER_BIT);

	return TRUE;
}


void CGLDynamicTexture::EndRenderToTexture()
{
	if( !m_bCreateFBO )return;

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	m_nInRendingTexture = 0;
}


/*
void CGLDynamicTexture::DisplayInterleavedTextures(CSize szDC, CRect rcView, BOOL bStartLeft)
{
	SetStencil(szDC);

	glDisable(GL_COLOR_LOGIC_OP);

	glEnable(GL_TEXTURE_2D);
	glDrawBuffer(GL_BACK);

	glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);

	rcView.top /= 2;
	rcView.bottom /= 2;
	
	// draw left texture
	int startRow = rcView.top, i, j;
	if( !bStartLeft )startRow++;
	
	glBindTexture(GL_TEXTURE_2D, m_texID1);
	
	glColor3f(1.0,1.0,1.0);
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	glBegin(GL_QUADS);
	glTexCoord2f(rcView.left/(float)m_szTexture.cx, (szDC.cy-1-rcView.top)/(float)m_szTexture.cy);		
	glVertex3f(rcView.left, szDC.cy-1-rcView.top*2, 0);
	
	glTexCoord2f(rcView.right/(float)m_szTexture.cx, (szDC.cy-1-rcView.top)/(float)m_szTexture.cy);
	glVertex3f(rcView.right, szDC.cy-1-rcView.top*2, 0);
	
	glTexCoord2f(rcView.right/(float)m_szTexture.cx, (szDC.cy-1-rcView.bottom)/(float)m_szTexture.cy);
	glVertex3f(rcView.right, szDC.cy-1-rcView.bottom*2, 0);

	glTexCoord2f(rcView.left/(float)m_szTexture.cx, (szDC.cy-1-rcView.bottom)/(float)m_szTexture.cy);		
	glVertex3f(rcView.left, szDC.cy-1-rcView.bottom*2, 0);

	glEnd();

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	
	// draw right texture
	startRow = rcView.top+1;
	if( !bStartLeft )startRow--;
	glBindTexture(GL_TEXTURE_2D, m_texID2);
	
//	glLogicOp(GL_COPY);
	glColor3f(1.0,1.0,1.0);
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	glBegin(GL_QUADS);
	glTexCoord2f(rcView.left/(float)m_szTexture.cx, (szDC.cy-1-rcView.top)/(float)m_szTexture.cy);		
	glVertex3f(rcView.left, szDC.cy-1-rcView.top*2, 0);
	
	glTexCoord2f(rcView.right/(float)m_szTexture.cx, (szDC.cy-1-rcView.top)/(float)m_szTexture.cy);
	glVertex3f(rcView.right, szDC.cy-1-rcView.top*2, 0);

	
	glTexCoord2f(rcView.right/(float)m_szTexture.cx, (szDC.cy-1-rcView.bottom)/(float)m_szTexture.cy);
	glVertex3f(rcView.right, szDC.cy-1-rcView.bottom*2, 0);
	
	glTexCoord2f(rcView.left/(float)m_szTexture.cx, (szDC.cy-1-rcView.bottom)/(float)m_szTexture.cy);		
	glVertex3f(rcView.left, szDC.cy-1-rcView.bottom*2, 0);
	
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_COLOR_LOGIC_OP);
	
	glFlush();
}
*/


void CGLDynamicTexture::DisplayInterleavedTextures(CSize szDC, CRect rcView, BOOL bStartLeft)
{
	if( !m_bCreateFBO )return;

	glEnable(GL_TEXTURE_2D);
	glDrawBuffer(GL_BACK);

	// draw left texture
	int startRow = rcView.top, i, j;
	if( !bStartLeft )startRow++;

	glBindTexture(GL_TEXTURE_2D, m_texID1);

	glLogicOp(GL_COPY);
	glColor3f(1.0,1.0,1.0);
	glLineWidth(1.0f);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glBegin(GL_LINES);
	for( i=startRow, j=0; i<rcView.bottom; i+=2, j++)
	{
		glTexCoord2f(rcView.left/(float)m_szTexture.cx, (szDC.cy/2-1-j)/(float)m_szTexture.cy);		
		glVertex3f(rcView.left, szDC.cy-1-i, 0);

		glTexCoord2f(rcView.right/(float)m_szTexture.cx, (szDC.cy/2-1-j)/(float)m_szTexture.cy);
		glVertex3f(rcView.right, szDC.cy-1-i, 0);
	}
	glEnd();

	// draw right texture
	startRow = rcView.top+1;
	if( !bStartLeft )startRow--;
	glBindTexture(GL_TEXTURE_2D, m_texID2);

	glLogicOp(GL_COPY);
	glColor3f(1.0,1.0,1.0);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glBegin(GL_LINES);
	for( i=startRow, j=0; i<rcView.bottom; i+=2, j++)
	{
		glTexCoord2f(rcView.left/(float)m_szTexture.cx, (szDC.cy/2-1-j)/(float)m_szTexture.cy);
		glVertex3f(rcView.left, szDC.cy-1-i, 0);

		glTexCoord2f(rcView.right/(float)m_szTexture.cx, (szDC.cy/2-1-j)/(float)m_szTexture.cy);		
		glVertex3f(rcView.right, szDC.cy-1-i, 0);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glFlush();
}


BOOL CGLDynamicTexture::SetStencil(CSize szDC)
{
	if( !m_bCreateFBO )return FALSE;
	
	if( m_szStencil==szDC )
		return TRUE;
	
	glEnable(GL_STENCIL_TEST);
	glDrawBuffer(GL_NONE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	
	glBegin(GL_LINES);

	int cx = m_szStencil.cx;
	int cy = m_szStencil.cy;

	for (int i = 0; i < cy; i += 2)
	{
		glVertex2i(0, i);
		glVertex2i(cx, i);
	}
	glEnd();
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	
	m_szStencil = szDC;

	return TRUE;
}

MyNameSpaceEnd