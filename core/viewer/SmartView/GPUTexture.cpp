// Texture_Map.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"

#include <glew.h>
#include <glaux.h>		// Header File For The Glaux Library
#include <stdlib.h>
#include <stdio.h>			// Header File For Standard Input/Output
#include <math.h>

#include "resource.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLuint	g_texture[1];			// Storage For One Texture 
GLhandleARB g_progHandle = 0;
int g_ImageWidth = 0;

extern AFX_EXTENSION_MODULE SmartViewDLL;


static int LoadTexture( LPCTSTR pMapName );
static void processSpecialKeys(int key, int x, int y);
static void Draw();
static void InitShader();
static char *textFileRead(char* fn );
static void SavetoBMP( LPCTSTR FileName, int Width, int Height, BYTE* pPixelsBuffer );
bool GPU_CheckSupported();
void GPU_DrawTexture( float x0, float y0, float z0, float tx0, float ty0,
	 float x1, float y1, float z1, float tx1, float ty1,
	 float x2, float y2, float z2, float tx2, float ty2,
	 float x3, float y3, float z3, float tx3, float ty3 );
void GPU_SetImageAttrib( int ImageWidth, int TextureIndex );//当切换纹理时，设置源图片大小，和该图片对应的纹理索引
#define printOpenGLError() printOglError(__FILE__, __LINE__)

static int printOglError(LPCTSTR file, int line)
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}


static void printInfoLog(GLhandleARB obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
		&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		OutputDebugString(ConvertCharToTChar(infoLog));
		if( infologLength>1024 )
			OutputDebugString(ConvertCharToTChar(infoLog+1024));
		free(infoLog);
	}
}

static AUX_RGBImageRec *LoadBMP(LPCTSTR FileName)
{
	FILE *pFile=NULL;
	if (!FileName)
	{
		return NULL;
	}
	else
	{
		pFile=fopen(ConvertTCharToChar(FileName),"r");
	}
	if (!pFile)
	{
		return NULL;
	}
	else
	{
		fclose(pFile);
		return auxDIBImageLoad(FileName);
	}

}
static int LoadTexture( LPCTSTR pMapName )
{
	int Status=FALSE;									// Status Indicator
	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture
	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0]=LoadBMP( pMapName ) )
	{
		Status=TRUE;									// Set The Status To TRUE
		glGenTextures(1, &g_texture[0]);					// Create The Texture
		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, g_texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, 
			TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, 
			TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (TextureImage[0])									// If Texture Exists
	{
		if (TextureImage[0]->data)							// If Texture Image Exists
		{
			free(TextureImage[0]->data);					// Free The Texture Image Memory
		}
		free(TextureImage[0]);								// Free The Image Structure
	}
	return Status;
}


static bool InitGL()
{
//	GPU_CheckSupported();
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	return true;		
}


bool GPU_InitGL()
{
	if( g_progHandle==0 && GPU_CheckSupported() )
	{
		InitGL();
		InitShader();
	}
	return true;		
}


static char *textFileRead(char *fn) {
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

static void SavetoBMP(LPCTSTR FileName, int Width, int Height, BYTE* PixelBuf)
{
	FILE *pFile;
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	int width,height;
	pFile=fopen(ConvertTCharToChar(FileName), "wb");//("1.BMP","wb");
	fileheader.bfType = 19778;
	fileheader.bfSize = Width*Height+54;//921654;
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = 54;

	infoheader.biBitCount = 24;
	infoheader.biClrImportant = 0;
	infoheader.biClrUsed = 0;
	infoheader.biCompression = 0;
	infoheader.biHeight = Height;
	infoheader.biPlanes = 1;
	infoheader.biSize = 40;
	infoheader.biSizeImage = Width*Height;
	infoheader.biWidth = Width;
	infoheader.biXPelsPerMeter = 3780;
	infoheader.biYPelsPerMeter = 3780;
	fwrite(&fileheader,1,sizeof(tagBITMAPFILEHEADER),pFile);
	fwrite(&infoheader,1,sizeof(tagBITMAPINFOHEADER),pFile);

	width=infoheader.biWidth;
	height=infoheader.biHeight;
	for ( int i=0; i<infoheader.biHeight; i++ )
	{
		int sizeCount=0;
		char buffer[3];
		for ( int j=0; j<infoheader.biWidth; j++ )
		{
			fwrite(&PixelBuf[(i*infoheader.biWidth+j)*3+2],sizeof(BYTE),1,pFile);
			fwrite(&PixelBuf[(i*infoheader.biWidth+j)*3+1],sizeof(BYTE),1,pFile);
			fwrite(&PixelBuf[(i*infoheader.biWidth+j)*3],sizeof(BYTE),1,pFile);

		}
	}
	if (feof(pFile))
	{
	}
	fclose(pFile);
}

static void Draw()
{
	static int Times = 0;
	static DWORD PassTime = 0;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D,g_texture[0]);
	GPU_SetImageAttrib( 256, 0 );
	glBegin(GL_QUADS);
	//glColor3f( 1,0,0);
	//glTexCoord2f(0.0f, 0.0f); 
	//glVertex3f(-1.0f, -1.0f,  0.0f);
	//glTexCoord2f(1.0f, 0.0f); 
	//glVertex3f( 1.0f, -1.0f,  0.0f);
	//glTexCoord2f(1.0f, 1.0f); 
	//glVertex3f( 1.0f,  1.0f,  0.0f);
	//glTexCoord2f(0.0f, 1.0f); 
	//glVertex3f(-1.0f,  1.0f,  0.0f);

	GPU_DrawTexture( -1.0, -1.0, 0.0, 0.0, 0.0,  
		   1.0, -1.0, 0.0, 1.0, 0.0,  
		   1.0,  1.0, 0.0, 1.0, 1.0, 
		  -1.0,  1.0, 0.0, 0.0, 1.0  );
	glEnd();
	
}


bool GPU_CheckSupported()
{
	static bool s_bGPUSupported = false, s_bHaveChecked = false;
	
	if (s_bHaveChecked)
	{
		return s_bGPUSupported;
	}

	char *p = (char *)glGetString( GL_EXTENSIONS );

	OutputDebugString(ConvertCharToTChar(p));

	if ( p != NULL && strstr( p, "GL_ARB_shader_objects")!=NULL&&
		strstr( p, "GL_ARB_shading_language")!=NULL&&
		strstr( p, "GL_ARB_vertex_shader")!=NULL&&
		strstr( p, "GL_ARB_fragment_shader")!=NULL ) 
	{
		s_bGPUSupported = true;
	}

	s_bHaveChecked = true;
	
	return s_bGPUSupported;
}

static void InitShader0() {	
	GLhandleARB vertHandle, fragHandle;
	char *vs = NULL,*fs = NULL,*fs2 = NULL;	
	glewInit();
	vertHandle = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragHandle = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);	
	vs = textFileRead("bicubic.vert");
	fs = textFileRead("bicubic.frag");	
	const char * vv = vs;
	const char * ff = fs;	
	glShaderSourceARB(vertHandle, 1, &vv,NULL);
	glShaderSourceARB(fragHandle, 1, &ff,NULL);	
	free(vs);free(fs);	
	glCompileShaderARB(vertHandle);
	glCompileShaderARB(fragHandle);	

#ifdef _DEBUG
	printInfoLog(vertHandle);
	printInfoLog(fragHandle);
#endif
	
	g_progHandle = glCreateProgramObjectARB();
	glAttachObjectARB(g_progHandle, vertHandle);
	glAttachObjectARB(g_progHandle, fragHandle);
	glLinkProgramARB(g_progHandle);

#ifdef _DEBUG
	printInfoLog(g_progHandle);
#endif
	
	glDeleteShader( vertHandle );
	glDeleteShader( fragHandle );	

}

static char *textResource(LPCTSTR lpName, LPCTSTR lpType)
{
	//定位我们的自定义资源，这里因为我们是从本模块定位资源，所以将句柄简单地置为NULL即可
	HRSRC hRsrc = FindResource(SmartViewDLL.hModule, lpName, lpType);
	if (NULL == hRsrc)
		return NULL;
	//获取资源的大小
	DWORD dwSize = SizeofResource(SmartViewDLL.hModule, hRsrc); 
	if (0 == dwSize)
		return NULL;
	//加载资源
	HGLOBAL hGlobal = LoadResource(SmartViewDLL.hModule, hRsrc); 
	if (NULL == hGlobal)
		return NULL;
	//锁定资源
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
		return NULL;	
	return (char *)pBuffer;
}

static void Compare(char *a, char *b)
{
	char *p = a, *p2 = b;
	while( *a!=0 && *b!=0 )
	{
		int add = 0;
		if( *a==13 || *a==10 )
		{
			a++;
			add++;
		}
		if( *b==13 || *b==10 )
		{
			b++;
			add++;
		}
		if( add>0 )continue;

		if( *a!=*b )
		{
			int c=1;
		}

		a++;
		b++;
	}
}


static void InitShader() {	
	GLhandleARB vertHandle, fragHandle;
	char *vs = NULL,*fs = NULL,*fs2 = NULL;
	glewInit();
	vertHandle = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragHandle = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);	
	fs = textResource(MAKEINTRESOURCE(IDR_BICUBIC_FRAG),_T("GPUCODE"));
//	fs = textFileRead("D:\\bicubic.frag");	
	vs = textResource(MAKEINTRESOURCE(IDR_BICUBIC_VERT),_T("GPUCODE"));
//	vs = textFileRead("D:\\bicubic.vert");	

	const char * vv = vs;
	const char * ff = fs;	
	glShaderSourceARB(vertHandle, 1, &vv,NULL);
	glShaderSourceARB(fragHandle, 1, &ff,NULL);	
	glCompileShaderARB(vertHandle);
	glCompileShaderARB(fragHandle);	

#ifdef _DEBUG
	printInfoLog(vertHandle);
	printInfoLog(fragHandle);
#endif
	
	g_progHandle = glCreateProgramObjectARB();
	glAttachObjectARB(g_progHandle, vertHandle);
	glAttachObjectARB(g_progHandle, fragHandle);
	glLinkProgramARB(g_progHandle);

#ifdef _DEBUG
	printInfoLog(g_progHandle);
#endif
	
	glDeleteShader( vertHandle );
	glDeleteShader( fragHandle );	

}


void GPU_EnableTexture( bool flag)
{
	if ( flag)
	{
		glUseProgramObjectARB( g_progHandle );
	}
	else
	{
		glUseProgramObjectARB(0);
	}
}

//当切换纹理时，设置源图片尺寸，TextureIndex表示纹理的通道索引，默认均为0
void GPU_SetImageAttrib( int ImageWidth, int TextureIndex )
{
	GLint texLoc = glGetUniformLocationARB( g_progHandle, "Texture" );
	glUniform1iARB( texLoc, TextureIndex );
	glUniform1fARB(glGetUniformLocationARB(g_progHandle, "ImageWidth"), ImageWidth);
}

/*
0-------3
|       |
|       |
1-------2
绘制两个三角形，xn,yn,zn表示顶点几何坐标，txn,tyn表示顶点纹理坐标
n的顺序如上图。
*/
void GPU_DrawTexture( float x0, float y0, float z0, float tx0, float ty0,
	  float x1, float y1, float z1, float tx1, float ty1,
	  float x2, float y2, float z2, float tx2, float ty2,
	  float x3, float y3, float z3, float tx3, float ty3 )
{
	glTexCoord2f( tx0, ty0 ); 
	glVertex3f( x0, y0, z0 );
	glTexCoord2f( tx1, ty1 ); 
	glVertex3f( x1, y1, z1 );
	glTexCoord2f( tx2, ty2 ); 
	glVertex3f( x2, y2, z2 );
	glTexCoord2f( tx3, ty3); 
	glVertex3f( x3, y3, z3);
}

void GPU_Destroy()
{
	if( g_progHandle )glDeleteProgram(g_progHandle);
	g_progHandle = 0;
}
