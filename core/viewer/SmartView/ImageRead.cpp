// ImageRead.cpp: implementation of the CImageRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
#include "ImageRead.h"
#include "GlobalFunc.h"
#include "MutiRasterLayer.h "
#include "iseeirw.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern AFX_EXTENSION_MODULE SmartViewDLL;

static char *static_extList[] =
{
	"tif","tiff","xif",
//	"jpg", "jpeg", "jpe", "jif", "jfif",
	"jpg", "jpeg", "jpe", "jif", "jfif",
		"lei","rei","eil","eir","ei",
		"ads", "sup", "vz"
};

static char *static_dllList[] = 
{
	"tiff.dll","tiff.dll","tiff.dll",
//	"com.dll", "com.dll", "com.dll", "com.dll", "com.dll",
	"com_jpg.dll", "com_jpg.dll", "com_jpg.dll", "com_jpg.dll", "com_jpg.dll",
		"tileimage.dll","tileimage.dll","tileimage.dll","tileimage.dll","tileimage.dll",
		"com_gdal.dll", "com_gdal.dll", "com_vz.dll"
};

//分配和释放都是使用 iseeirw.dll 中的函数来做，这是最安全的
static LPINFOSTR CALLAGREEMENT AllocINFOSTR()
{
	return isirw_alloc_INFOSTR();
}


static void FreeINFOSTR(LPINFOSTR pinfo)
{
	isirw_free_INFOSTR(pinfo);
}

static void FreePartINFOSTR(LPINFOSTR pinfo)
{
	if( pinfo==NULL )
		return;
	
	EnterCriticalSection(&(pinfo->access));
	if (pinfo->pp_line_addr )
	{
		isirw_free_mem(pinfo->pp_line_addr);
		pinfo->pp_line_addr = (void **)0;
	}
	if (pinfo->p_bit_data )
	{	
		isirw_free_mem(pinfo->p_bit_data);		
		pinfo->p_bit_data = (unsigned char *)0;
	}
	if (pinfo->play_order )
	{
		isirw_free_mem(pinfo->play_order);
		pinfo->play_order = (unsigned long *)0;
	}
	
	LeaveCriticalSection(&(pinfo->access));
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRITICAL_SECTION g_section;
CImageRead::CImageRead()
{

	m_bLoad = FALSE;
	m_pInfoStr = NULL;
	memset(m_chFileName,0,sizeof(m_chFileName));
	m_nPyramid = 0;

	m_hImgDll =NULL;
	m_pInfoStr = NULL;	
	m_pInfoFunc	= NULL;
	m_pReadFunc	= NULL;
	m_pCloseFunc = NULL;
	m_pInitCacheFunc = NULL;
	m_pClearCacheFunc = NULL;
}

CImageRead::~CImageRead()
{
	Close();
}
CMutiImageRead::CMutiImageRead()
{
	m_bLoad = FALSE;
	m_pInfoStr = NULL;
	memset(m_chFileName,0,sizeof(m_chFileName));
	m_nPyramid = 0;
	m_hImgDll =NULL;
	m_pInfoStr = NULL;	
	m_pInfoFunc	= NULL;
	m_pReadFunc	= NULL;
}
CMutiImageRead::~CMutiImageRead()
{
	Close();
}

BOOL CImageRead::LoadImgDll(LPCTSTR fileName)
{
	if( m_bLoad && strcmp(ConvertTCharToChar(fileName),m_chFileName)==0 )
		return TRUE;
	
	if( m_bLoad )Close();

	DWORD err = GetLastError();

	HMODULE hDll = ::LoadLibraryEx(fileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	err = GetLastError();
	if( hDll==NULL )
		return FALSE;

	LONG_PTR test = isirw_test_plugin(hDll);
	if( test!=0 )
	{
		m_pDllInfo = isirw_get_info(test);
		m_hImgDll = hDll;
	}
	else
	{
		FreeLibrary(hDll);
		return FALSE;
	}	
	
	m_pInfoFunc	= (IRWP_API_GET_IMG_INFO)isirw_get_api_addr(m_hImgDll, m_pDllInfo, get_image_info);
	m_pReadFunc	= (IRWP_API_READ_IMG)isirw_get_api_addr(m_hImgDll, m_pDllInfo, read_image);
	
	m_pCloseFunc	= (IRWP_API_CLOSE_IMG)isirw_get_api_addr(m_hImgDll, m_pDllInfo, close_image);

	m_pInitCacheFunc = (LPFUNC_InitCache)GetProcAddress(m_hImgDll,"init_cache");
	m_pClearCacheFunc = (LPFUNC_ClearCache)GetProcAddress(m_hImgDll,"clear_cache");

	return TRUE;
}


BOOL CImageRead::IsValid()
{
	return m_bLoad;
}


LPCTSTR CImageRead::GetImageName()
{
	static CString s;
	s = ConvertCharToTChar(m_chFileName);
	return (LPCTSTR)s;
}


LPSUBIMGBLOCK CImageRead::GetCurSubImgInfo()
{
	if( m_pInfoStr==NULL )return NULL;
	if( m_nPyramid<=0 )return NULL;

	LPSUBIMGBLOCK p=m_pInfoStr->psubimg;
	for( int i=1; i<m_nPyramid; i++ )
	{
		if( p!=NULL )p = p->next;
	}

	return p;
}
void CImageRead::Process()
{
}

int CImageRead::SetCurReadZoom(float fZoom)
{
	if( !IsValid() )
		return SETZOOM_INVALID;
	if( fZoom<=0 )return SETZOOM_INVALID;
	if( fZoom>1 ){
		if( m_nPyramid==0 )return (SETZOOM_NOCHANGE|SETZOOM_OUTTOP);
		m_nPyramid=0;
		return (SETZOOM_VALID|SETZOOM_OUTTOP);
	}

	fZoom = 1/fZoom;
	int	  i0,i;	
	for( i0=1, i=0; ; i++ ){
		if( fZoom<=i0 )break;
		i0 = i0<<1;				
	}
	
	if( i==m_nPyramid ) 
		return SETZOOM_NOCHANGE;

	if( i>(int)m_pInfoStr->imgnumbers-1 ){
		if( m_nPyramid==m_pInfoStr->imgnumbers-1 )
			return (SETZOOM_NOCHANGE|SETZOOM_OUTBOTTOM);

		m_nPyramid = m_pInfoStr->imgnumbers-1;
		return (SETZOOM_VALID|SETZOOM_OUTBOTTOM);
	}
		
	m_nPyramid = i;
	return SETZOOM_VALID;
}
BOOL CMutiImageRead::LoadImgDll(LPCTSTR fileName)
{
	if( strcmp(ConvertTCharToChar(fileName),m_chFileName)==0 && m_bLoad )
		return TRUE;
	
	if( m_bLoad )Close();
	
	HMODULE hDll = ::LoadLibrary(fileName);
	if( hDll==NULL )
		return FALSE;
	
	LONG_PTR test = isirw_test_plugin(hDll);
	if( test!=0 )
	{
		m_pDllInfo = isirw_get_info(test);
		m_hImgDll = hDll;
	}
	else
	{
		FreeLibrary(hDll);
		return FALSE;
	}	
	
	m_pInfoFunc	= (IRWP_API_GET_IMG_INFO)isirw_get_api_addr(m_hImgDll, m_pDllInfo, get_image_info);
	m_pReadFunc	= (IRWP_API_READ_IMG)isirw_get_api_addr(m_hImgDll, m_pDllInfo, read_image);
	m_pCloseFunc= (IRWP_API_CLOSE_IMG)isirw_get_api_addr(m_hImgDll, m_pDllInfo, close_image);
	return TRUE;
}
BOOL CMutiImageRead::IsValid()
{
	return m_bLoad;
}
LPCTSTR CMutiImageRead::GetImageName()
{
	static CString s;
	s = ConvertCharToTChar(m_chFileName);
	return (LPCTSTR)s;
}
LPSUBIMGBLOCK CMutiImageRead::GetCurSubImgInfo()
{
	if( m_pInfoStr==NULL )return NULL;
	if( m_nPyramid<=0 )return NULL;
	LPSUBIMGBLOCK p=m_pInfoStr->psubimg;
	for( int i=1; i<m_nPyramid; i++ )
	{
		if( p!=NULL )p = p->next;
	}
	return p;
}
void CMutiImageRead::Process()
{
}
int CMutiImageRead::SetCurReadZoom(float fZoom)
{
	if( fZoom<=0 )return SETZOOM_INVALID;
	if( fZoom>1 ){
		if( m_nPyramid==0 )return (SETZOOM_NOCHANGE|SETZOOM_OUTTOP);
		m_nPyramid=0;
		return (SETZOOM_VALID|SETZOOM_OUTTOP);
	}
	fZoom = 1/fZoom;
	int	  i0,i;	
	for( i0=1, i=0; ; i++ ){
		if( fZoom<=i0 )break;
		i0 = i0<<1;				
	}
	if( i==m_nPyramid ) 
		return SETZOOM_NOCHANGE;
	if( i>(int)m_pInfoStr->imgnumbers-1 ){
		if( m_nPyramid==m_pInfoStr->imgnumbers-1 )
			return (SETZOOM_NOCHANGE|SETZOOM_OUTBOTTOM);
		m_nPyramid = m_pInfoStr->imgnumbers-1;
		return (SETZOOM_VALID|SETZOOM_OUTBOTTOM);
	}
	m_nPyramid = i;
	return SETZOOM_VALID;
}

void CImageRead::Close()
{
	if(m_pCloseFunc) m_pCloseFunc(m_pInfoStr,m_chFileName);
	memset(m_chFileName,0,sizeof(m_chFileName));

	if( m_pClearCacheFunc && !m_strUISCacheDir.IsEmpty() )
	{
		(*m_pClearCacheFunc)();
	}

	if( m_pInfoStr!=NULL )
	{
		FreeINFOSTR(m_pInfoStr);
		m_pInfoStr = NULL;
	}

	m_nPyramid = 0;

	if (m_hImgDll != NULL && 0 != strcmp("com_uis", m_pDllInfo->irwp_name))
		::FreeLibrary(m_hImgDll);
	m_hImgDll=NULL;
	
	m_pInfoStr = NULL;
	m_pInfoFunc	= NULL;
	m_pReadFunc	= NULL;
	m_pCloseFunc = NULL;
	m_bLoad = FALSE;
	m_pInitCacheFunc = NULL;
	m_pClearCacheFunc = NULL;
}

CSize CImageRead::GetImageOriginalSize()
{
	if( !IsValid() )
		return CSize(0,0);
	
	return CSize(m_pInfoStr->width,m_pInfoStr->height);
}


int CImageRead::GetColorBitCount()
{
	if( m_pInfoStr==NULL )return 0;
	return m_pInfoStr->bitcount;
}


void CMutiImageRead::Close()
{
	if(m_pCloseFunc)m_pCloseFunc(m_pInfoStr,m_chFileName);
	memset(m_chFileName,0,sizeof(m_chFileName));
	if( m_pInfoStr!=NULL )
	{
		FreeINFOSTR(m_pInfoStr);
		m_pInfoStr = NULL;
	}
	m_nPyramid = 0;
	if( m_hImgDll!=NULL)::FreeLibrary(m_hImgDll);
	m_hImgDll=NULL;
	m_pInfoStr = NULL;
	m_pInfoFunc	= NULL;
	m_pReadFunc	= NULL;
	m_pCloseFunc = NULL;
	m_bLoad = FALSE;
}
CSize CMutiImageRead::GetImageOriginalSize()
{
	if( !IsValid() )
		return CSize(0,0);
	return CSize(m_pInfoStr->width,m_pInfoStr->height);
}
int CMutiImageRead::GetColorBitCount()
{
	if( m_pInfoStr==NULL )return 0;
	return m_pInfoStr->bitcount;
}


//////////////////////////////////////////////////////////////////////
// CRoatableImageRead class
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRoatableImageRead::CRoatableImageRead()
{
	m_fAngle = 0;

	m_pRotateBuf = NULL;
	m_nBufSize = 0;
}


CRoatableImageRead::~CRoatableImageRead()
{
	if( m_pRotateBuf )
		delete m_pRotateBuf;
}


BYTE *CRoatableImageRead::GetRotateBuf(int size)
{
	if( size<m_nBufSize )
		return m_pRotateBuf;

	if( m_pRotateBuf )delete[] m_pRotateBuf;
	m_pRotateBuf = new BYTE[size];
	if( m_pRotateBuf )
	{
		m_nBufSize = size;
	}

	return m_pRotateBuf;
}



CSize CRoatableImageRead::GetImageSize()
{
	if( !IsRotated() || m_fAngle==180.0 )
		return CImageRead::GetImageSize();

	if( !IsValid() || m_nPyramid<0 )return CSize(0,0);
	
	if( m_nPyramid==0 )
		return CSize( m_pInfoStr->height, m_pInfoStr->width);

	LPSUBIMGBLOCK p=GetCurSubImgInfo();
		
	if( p==NULL )return CSize(0,0);		
	return CSize(p->height,p->width);
}
CRoatableMutiImageRead::CRoatableMutiImageRead()
{
	m_fAngle = 0;
	m_pRotateBuf = NULL;
	m_nBufSize = 0;
}
CRoatableMutiImageRead::~CRoatableMutiImageRead()
{
	if( m_pRotateBuf )
		delete m_pRotateBuf;
}
BYTE *CRoatableMutiImageRead::GetRotateBuf(int size)
{
	if( size<m_nBufSize )
		return m_pRotateBuf;
	if( m_pRotateBuf )delete[] m_pRotateBuf;
	m_pRotateBuf = new BYTE[size];
	if( m_pRotateBuf )
	{
		m_nBufSize = size;
	}
	return m_pRotateBuf;
}

CSize CRoatableMutiImageRead::GetImageSize()
{
	if( !IsRotated() || m_fAngle==180.0 )
		return CMutiImageRead::GetImageSize();
	if( !IsValid() || m_nPyramid<0 )return CSize(0,0);
	if( m_nPyramid==0 )
		return CSize( m_pInfoStr->height, m_pInfoStr->width);
	LPSUBIMGBLOCK p=GetCurSubImgInfo();
	if( p==NULL )return CSize(0,0);		
	return CSize(p->height,p->width);
}

// 顺时针旋转
typedef enum {
	ROTATE_90 = 0,
	ROTATE_180 = 1,
	ROTATE_270 = 2,
	ROTATE_VERTICAL = 3,
	ROTATE_HORZONTAL = 4
}RotateFlag;

// 旋转一个影像块
// lpSrc的第一个象素对应rcSrc的左下角
bool RotateBlock(LPBYTE lpSrc, CRect rcSrc, int nColor, LPBYTE lpDest, int flag)
{
	int nSrcRow,nSrcCol,nDestRow,nDestCol;
	int nSrcWidth,nSrcHeight,nDestWidth,nDestHeight;

	nSrcWidth = rcSrc.Width();
	nSrcHeight = rcSrc.Height();
	switch( flag )
	{
	case ROTATE_90:
	case ROTATE_270:
		nDestWidth	= nSrcHeight;
		nDestHeight	= nSrcWidth;
		break;
	case ROTATE_180:
	case ROTATE_VERTICAL:
	case ROTATE_HORZONTAL:
		nDestWidth	= nSrcWidth;
		nDestHeight	= nSrcHeight;
		break;
	}

	int nSrcLineSize = (nSrcWidth*nColor+3)&(~3);
	int nDestLineSize = (nDestWidth*nColor+3)&(~3);

	CRect rcValid;
	rcValid = rcSrc;

	// 变成缓冲区坐标
	rcValid.right	-= rcSrc.left;
	rcValid.left	-= rcSrc.left;
	rcValid.bottom	-= rcSrc.top;
	rcValid.top		-= rcSrc.top;

	BYTE *posSrc, *posDest;
	int i;
	for( nSrcRow = rcValid.top; nSrcRow < rcValid.bottom; nSrcRow ++ )
	{
		for( nSrcCol = rcValid.left; nSrcCol < rcValid.right; nSrcCol ++ )
		{
			switch( flag )
			{
			case ROTATE_90:
				nDestRow = rcSrc.Width() - 1 - nSrcCol;
				nDestCol = nSrcRow;
				break;

			case ROTATE_180:
				nDestRow = rcSrc.Height() - 1 - nSrcRow;
				nDestCol = rcSrc.Width() - 1 - nSrcCol;
				break;

			case ROTATE_270:
				nDestRow = nSrcCol;
				nDestCol = rcSrc.Height() - 1 - nSrcRow;
				break;

			case ROTATE_VERTICAL:
				nDestRow = rcSrc.Height() - 1 - nSrcRow;
				nDestCol = nSrcCol;
				break;

			case ROTATE_HORZONTAL:
				nDestRow = nSrcRow;
				nDestCol = rcSrc.Width() - 1 - nSrcCol;
				break;

			default:
				return false;
			}

			posSrc	= lpSrc + nSrcRow*nSrcLineSize + nSrcCol*nColor;
			posDest	= lpDest + nDestRow*nDestLineSize + nDestCol*nColor;
			for( i = 0; i < nColor; i ++ )
			{
				posDest[i] = posSrc[i];
			}
		}
	}

	return true;
}

CRect RotateRect(CRect rect, CSize szImg, float ang)
{
	CRect rcRotate;

	if( ang==270.0 )
	{
		rcRotate.SetRect(szImg.cx-rect.bottom,rect.left,szImg.cx-rect.top,rect.right);
	}
	else if( ang==180.0 )
	{
		rcRotate.SetRect(szImg.cx-rect.right,szImg.cy-rect.bottom,szImg.cx-rect.left,szImg.cy-rect.top);
	}
	else if( ang==90.0 )
	{
		rcRotate.SetRect(rect.top,szImg.cy-rect.right,rect.bottom,szImg.cy-rect.left);
	}
	else
		rcRotate = rect;

	return rcRotate;
}

int FlagFromAngle(float ang)
{
	RotateFlag flag = (RotateFlag)-1;

	if( ang==90.0 )
		flag = ROTATE_270;
	else if( ang==180.0 )
		flag = ROTATE_180;
	else if( ang==-180.0 )
		flag = ROTATE_VERTICAL;
	else if( ang==270.0 )
		flag = ROTATE_90;

	return flag;
}


CSize CImageRead::GetImageSize()
{
	if( !IsValid() || m_nPyramid<0 )
		return CSize(0,0);
	
	if( m_nPyramid==0 )
		return CSize(m_pInfoStr->width,m_pInfoStr->height);

	LPSUBIMGBLOCK p = GetCurSubImgInfo();
	
	if( p==NULL )return CSize(0,0);		
	return CSize(p->width,p->height);
}

CSize CMutiImageRead::GetImageSize()
{
	if( !IsValid() || m_nPyramid<0 )
		return CSize(0,0);
	if( m_nPyramid==0 )
		return CSize(m_pInfoStr->width,m_pInfoStr->height);
	
	LPSUBIMGBLOCK p=GetCurSubImgInfo();
	if( p==NULL )return CSize(0,0);		
	return CSize(p->width,p->height);
}

BOOL CRoatableImageRead::ReadRectBmp(CRect rect, HBITMAP hBmp)
{
	if( !IsRotated() )
		return CImageRead::ReadRectBmp(rect,hBmp);

	if( !IsValid()||hBmp==NULL )return FALSE;	

	CSize szImg = CImageRead::GetImageSize();
	CRect rcRotate = RotateRect(rect,szImg,m_fAngle);
		
	if( m_pReadFunc==NULL )return FALSE;
	if((*m_pReadFunc)(&m_IsAddr, m_pInfoStr,m_nPyramid,rcRotate)!= ER_SUCCESS)
	{
		return FALSE;
	}

	int nbuf = ((((rect.Width()*m_pInfoStr->bitcount+7)>>3)+3)&(~3))*rect.Height();
	BYTE *pRotateBuf = GetRotateBuf(nbuf);

	RotateBlock(m_pInfoStr->p_bit_data,CRect(0,0,rcRotate.Width(),rcRotate.Height()),m_pInfoStr->bitcount>>3,
		pRotateBuf,FlagFromAngle(m_fAngle));

	return PasteBits(hBmp,rect,pRotateBuf,nbuf);
}

void CRoatableImageRead::SetRotate(float ang)
{
	m_fAngle = ang;
}


BOOL CRoatableImageRead::IsRotated()
{
	return (m_fAngle!=0.0);
}

BOOL CRoatableMutiImageRead::ReadRectBmp(CRect rect, HBITMAP hBmp)
{
	if( !IsRotated() )
		return CMutiImageRead::ReadRectBmp(rect,hBmp);
	if( !IsValid()||hBmp==NULL )return FALSE;	
	CSize szImg = CMutiImageRead::GetImageSize();
	CRect rcRotate = RotateRect(rect,szImg,m_fAngle);
	if( m_pReadFunc==NULL )return FALSE;
	if((*m_pReadFunc)(&m_IsAddr, m_pInfoStr,m_nPyramid,rcRotate)!= ER_SUCCESS)
	{
		return FALSE;
	}
	int nbuf = ((((rect.Width()*m_pInfoStr->bitcount+7)>>3)+3)&(~3))*rect.Height();
	BYTE *pRotateBuf = GetRotateBuf(nbuf);
	RotateBlock(m_pInfoStr->p_bit_data,CRect(0,0,rcRotate.Width(),rcRotate.Height()),m_pInfoStr->bitcount>>3,
		pRotateBuf,FlagFromAngle(m_fAngle));
	return PasteBits(hBmp,rect,pRotateBuf,nbuf);
}
void CRoatableMutiImageRead::SetRotate(float ang)
{
	m_fAngle = ang;
}
BOOL CRoatableMutiImageRead::IsRotated()
{
	return (m_fAngle!=0.0);
}


float CImageRead::GetCurReadZoom()
{
	if( m_nPyramid<=0 )
		return 1.0;
	
	LPSUBIMGBLOCK p = GetCurSubImgInfo();
	if( p==NULL )return 1.0;
	
	return p->zoomRate;	
}

float CMutiImageRead::GetCurReadZoom()
{
	if( m_nPyramid<=0 )
		return 1.0;

	LPSUBIMGBLOCK p = GetCurSubImgInfo();
	if( p==NULL )return 1.0;		
	return p->zoomRate;	
}


//////////////////////////////////////////////////////////////////////
// CThreadImageRead class
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadImageRead::CThreadImageRead():m_rcBlock(0,0,0,0)
{
	m_hReadThread = NULL;
	m_hEvent = NULL;
	m_nThreadCtrl = threadDead;
	m_hBlockBmp = NULL;
	m_hLineBmp = NULL;
	m_hWndCallback= NULL;
	m_nMsgCallback = 0;
	m_pBlockBits = NULL;
	m_pLineBits = NULL;
	m_nCurRead = 0;
	m_bSkipRead = FALSE;
	m_nSkipReadRate = 1;
	m_pBmpInfo = NULL;
}


CThreadImageRead::~CThreadImageRead()
{
	DestroyReadThread();
	if( m_hBlockBmp )DeleteObject(m_hBlockBmp);
	if( m_hLineBmp )DeleteObject(m_hLineBmp);
	if( m_pBmpInfo )delete[] (BYTE*)m_pBmpInfo;
}


void CThreadImageRead::Lock()
{
	if( IsReadThreadStarted() )EnterCriticalSection(&m_section);
}

void CThreadImageRead::Unlock()
{
	if( IsReadThreadStarted() )LeaveCriticalSection(&m_section);
}


int CThreadImageRead::SetCurReadZoom(float zoomRate)
{
	Lock();
	int ret = CImageRead::SetCurReadZoom(zoomRate);
	Unlock();
	return ret;
}


void CThreadImageRead::Close()
{
	DestroyReadThread();
	CImageRead::Close();
}



BOOL CThreadImageRead::ReadRectBmp(CRect rect, HBITMAP hBmp)
{
	Lock();
	BOOL ret = CRoatableImageRead::ReadRectBmp(rect,hBmp);
	Unlock();
	return ret;
}

#define SAVE_VAR(type,x)			type save##x = x; x = 0;
#define SAVE_VAR2(type,x)			type save##x = x;
#define RESTORE_VAR(type,x)			x = save##x;


BOOL CThreadImageRead::ReadRectBmp(CRect rect, HBITMAP hBmp, BOOL bSkipRead, int nSkipRate)
{
	if( !IsValid() )
		return FALSE;
	if( !bSkipRead )
		return ReadRectBmp(rect,hBmp);

	Lock();

	// save member variants
	SAVE_VAR2(CRect,m_rcBlock);
	SAVE_VAR(BOOL,m_bSkipRead);
	SAVE_VAR(int,m_nCurRead);
	SAVE_VAR(int,m_nSkipReadRate);
	SAVE_VAR(HBITMAP,m_hBlockBmp);
	SAVE_VAR(HBITMAP,m_hLineBmp);
	SAVE_VAR(BYTE *,m_pBlockBits);
	SAVE_VAR(BYTE *,m_pLineBits);
	SAVE_VAR(BITMAPINFO *,m_pBmpInfo);

	SetReadBmpInfo(rect.Width(),rect.Height(),m_pInfoStr->bitcount);

	CRect rect2 = rect;
	rect2.left *= nSkipRate; rect2.right *= nSkipRate;
	rect2.top *= nSkipRate; rect2.bottom *= nSkipRate;
	SetReadRect(rect2,bSkipRead,nSkipRate);
	for( int i=0; i<rect.Height(); i++)
	{
		ReadBlockLine(m_nCurRead);
		m_nCurRead += nSkipRate;
	}

	int linesize = (m_pBmpInfo->bmiHeader.biWidth*(m_pBmpInfo->bmiHeader.biBitCount>>3)+3)&(~3);

	//由于上面的for循环是按照行依次读取并存储到 m_pBlockBits 中，所以 m_pBlockBits 中
	//的像素的行顺序是按照Bitmap的顺序，
	DWORD saveorder = m_pInfoStr->order;
	m_pInfoStr->order = 0;
	PasteBits(hBmp,rect,m_pBlockBits,linesize*m_pBmpInfo->bmiHeader.biHeight);
	m_pInfoStr->order = saveorder;

	if( m_hBlockBmp )DeleteObject(m_hBlockBmp);
	if( m_hLineBmp )DeleteObject(m_hLineBmp);
	if( m_pBmpInfo )delete[] (BYTE*)m_pBmpInfo;

	BOOL ret = TRUE;

	// restore member variants
	RESTORE_VAR(CRect,m_rcBlock);
	RESTORE_VAR(BOOL,m_bSkipRead);
	RESTORE_VAR(int,m_nCurRead);
	RESTORE_VAR(int,m_nSkipReadRate);
	RESTORE_VAR(HBITMAP,m_hBlockBmp);
	RESTORE_VAR(HBITMAP,m_hLineBmp);
	RESTORE_VAR(BYTE *,m_pBlockBits);
	RESTORE_VAR(BYTE *,m_pLineBits);
	RESTORE_VAR(BITMAPINFO *,m_pBmpInfo);

	Unlock();
	return ret;
}


BOOL CThreadImageRead::InitReadThread()
{
	m_hReadThread = CreateThread(
						  NULL,
						  0,
						  ReadThread,
						  (LPVOID)this,	
						  (DWORD)CREATE_SUSPENDED,
						  NULL);
	
	if(m_hReadThread==NULL)
	{		
		return FALSE;
	}

	SetThreadPriority(m_hReadThread,THREAD_PRIORITY_IDLE);

	m_nThreadCtrl = threadPause; // suspend
	InitializeCriticalSection(&m_section);

	m_hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	return TRUE;
}

#define CREATEDC 	HDC hdc = ::CreateCompatibleDC(NULL);\
HBITMAP hOld = (HBITMAP)::SelectObject(hdc,hBmp);\
if( hOld==NULL )\
{\
	::DeleteObject(hdc);\
	return FALSE;\
}\


void CThreadImageRead::DestroyReadThread()
{
	if( m_hReadThread )
	{
		m_nThreadCtrl = threadDead;
		if( WaitForSingleObject( m_hReadThread,INFINITE )!=WAIT_OBJECT_0 )
		{
			TerminateThread( m_hReadThread, 1 );
		}
		CloseHandle(m_hReadThread);
		m_hReadThread = NULL;
		DeleteCriticalSection(&m_section);
	}

	if( m_hEvent )
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}


void CThreadImageRead::BeginReadThread()
{
	m_nThreadCtrl = threadRun; // go
	if( m_hReadThread )ResumeThread(m_hReadThread);
}


void CThreadImageRead::EndReadThread()
{
	m_nThreadCtrl = threadPause; // suspend
	WaitForSingleObject(m_hEvent,INFINITE);
}

#define SETBMPINFO(pInfo) 	pInfo->bmiHeader.biSize = sizeof(pInfo->bmiHeader);\
pInfo->bmiHeader.biWidth = nWidth;\
pInfo->bmiHeader.biHeight = nHeight;\
pInfo->bmiHeader.biSizeImage = m_pInfoStr->bit_data_len;\
pInfo->bmiHeader.biPlanes = 1;\
pInfo->bmiHeader.biBitCount = (unsigned short)m_pInfoStr->bitcount;\
pInfo->bmiHeader.biCompression = BI_RGB;\
pInfo->bmiHeader.biXPelsPerMeter = 0;\
pInfo->bmiHeader.biYPelsPerMeter = 0;\
pInfo->bmiHeader.biClrUsed = 0;\
pInfo->bmiHeader.biClrImportant = 0;\

BOOL CImageRead::PasteBits(HBITMAP hBmp, CRect rect, BYTE *pBuf, int nBufSize)
{
	DIBSECTION sect;
	if( ::GetObject(hBmp,sizeof(sect),&sect)==0 )return FALSE;
	
	if( m_pInfoStr->bitcount==24 && ((m_pInfoStr->order&3)==0 || (m_pInfoStr->order&3)==3) )
	{
		memcpy(sect.dsBm.bmBits, pBuf, min(nBufSize,sect.dsBmih.biSizeImage));
		return TRUE;
	}
	
	CREATEDC;
	
	UINT iUsage = DIB_RGB_COLORS;
	
	BITMAPINFO* pInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)+255*sizeof(RGBQUAD)];
	if( pInfo ==NULL )
	{
		::SelectObject(hdc,hOld);
		::DeleteDC(hdc);
		return FALSE;
	}
	
	int nWidth = rect.Width(), nHeight = rect.Height();

	SETBMPINFO(pInfo);
	memcpy(pInfo->bmiColors,m_pInfoStr->palette, 256*sizeof(RGBQUAD));
	
	if( (m_pInfoStr->order&3)==0 || (m_pInfoStr->order&3)==3 )
	{
		::StretchDIBits( hdc, 0,0,nWidth,nHeight,0,0,nWidth,nHeight,
			pBuf,pInfo,iUsage,SRCCOPY);
	}
	else
	{
		::StretchDIBits( hdc, 0,nHeight-1,nWidth,-nHeight,0,0,nWidth,nHeight,
			pBuf,pInfo,iUsage,SRCCOPY);
	}
	
	::SelectObject(hdc,hOld);
	::DeleteDC(hdc);
	
	delete[] (BYTE*)pInfo;
	
	return TRUE;
}

BOOL CImageRead::ReadRectBmp(CRect rect, HBITMAP hBmp)
{
	if( !IsValid()||hBmp==NULL )return FALSE;	
	if( m_pReadFunc==NULL )return FALSE;
	
	CSize szImage = GetImageSize();
	rect = CRect(rect.left,szImage.cy-rect.bottom,rect.right,szImage.cy-rect.top);
	CRect rect0 = rect;
	rect0.right = rect.left+(m_pInfoStr->bitcount*rect.Width()+31)/32*32/m_pInfoStr->bitcount;
	if((*m_pReadFunc)(&m_IsAddr, m_pInfoStr,m_nPyramid,rect0)!= ER_SUCCESS)
	{
		return FALSE;
	}
	BOOL bRet = PasteBits(hBmp, rect, m_pInfoStr->p_bit_data, m_pInfoStr->bit_data_len);

	return bRet;
}
BOOL CMutiImageRead::PasteBits(HBITMAP hBmp, CRect rect, BYTE *pBuf, int nBufSize)
{
	DIBSECTION sect;
	if( ::GetObject(hBmp,sizeof(sect),&sect)==0 )return FALSE;
	
	if( m_pInfoStr->bitcount==24 && ((m_pInfoStr->order&3)==0 || (m_pInfoStr->order&3)==3) )
	{
		memcpy(sect.dsBm.bmBits, pBuf, min(nBufSize,sect.dsBmih.biSizeImage));
		return TRUE;
	}
	CREATEDC;
	BITMAPINFO* pInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)+255*sizeof(RGBQUAD)];
	if( pInfo ==NULL )
	{
		::SelectObject(hdc,hOld);
		::DeleteDC(hdc);
		return FALSE;
	}
	int nWidth = rect.Width(), nHeight = rect.Height();
	SETBMPINFO(pInfo);
	memcpy(pInfo->bmiColors,m_pInfoStr->palette, 256*sizeof(RGBQUAD));
	if( (m_pInfoStr->order&3)==0 || (m_pInfoStr->order&3)==3 )
	{
		::StretchDIBits( hdc, 0,0,nWidth,nHeight,0,0,nWidth,nHeight,
			pBuf,pInfo,DIB_RGB_COLORS,SRCCOPY);
	}
	else
	{
		::StretchDIBits( hdc, 0,nHeight-1,nWidth,-nHeight,0,0,nWidth,nHeight,
			pBuf,pInfo,DIB_RGB_COLORS,SRCCOPY);
	}
	::SelectObject(hdc,hOld);
	::DeleteDC(hdc);
	delete[] (BYTE*)pInfo;
	return TRUE;
}
BOOL CMutiImageRead::ReadRectBmp(CRect rect, HBITMAP hBmp)
{
	if( !IsValid()||hBmp==NULL )return FALSE;
	if( m_pReadFunc==NULL )return FALSE;
	CSize szImage = GetImageSize();
	rect = CRect(rect.left,szImage.cy-rect.bottom,rect.right,szImage.cy-rect.top);
	CRect rect0 = rect;
	rect0.right = rect.left+(m_pInfoStr->bitcount*rect.Width()+31)/32*32/m_pInfoStr->bitcount;
	if((*m_pReadFunc)(&m_IsAddr, m_pInfoStr,m_nPyramid,rect0)!= ER_SUCCESS)
	{
		return FALSE;
	}

	return PasteBits(hBmp,rect,m_pInfoStr->p_bit_data,m_pInfoStr->bit_data_len);
}


BOOL CThreadImageRead::ReadBlockLine(int nCurLine)
{
	if( !m_hLineBmp || !m_pBlockBits || !m_pLineBits ||!m_pBmpInfo )return FALSE;
	BYTE *bit0 = m_pBlockBits, *bit1 = m_pLineBits;
	
	CRect rcLine = m_rcBlock;
	rcLine.top = nCurLine;  rcLine.bottom = rcLine.top+1;
	if( !ReadRectBmp(rcLine,m_hLineBmp) )
		return TRUE;
	
	int linesize = ((m_pBmpInfo->bmiHeader.biWidth*m_pBmpInfo->bmiHeader.biBitCount+7)/8+3)&(~3);
	bit0 += (linesize*(m_pBmpInfo->bmiHeader.biHeight-1-(nCurLine-m_rcBlock.top)/m_nSkipReadRate));

	BYTE *t0, *t1;
	t0 = bit0;  t1 = bit1;
	if( m_pBmpInfo->bmiHeader.biBitCount==24 )
	{
		for( int j=m_rcBlock.Width()/m_nSkipReadRate-1; j>=0; j--)
		{
			*t0++ = t1[0]; *t0++ = t1[1]; *t0++ = t1[2];
			t1 += 3*m_nSkipReadRate;
		}
	}
	else if( m_pBmpInfo->bmiHeader.biBitCount==8 )
	{
		for( int j=m_rcBlock.Width()/m_nSkipReadRate-1; j>=0; j--)
		{
			*t0++ = t1[0];
			t1 += m_nSkipReadRate;
		}
	}

	return TRUE;
}




DWORD CThreadImageRead::ReadThread(LPVOID pParam)
{
    CThreadImageRead* pthis = (CThreadImageRead*)pParam;
	while( pthis->m_nThreadCtrl!=threadDead )
	{
		if( pthis->m_nThreadCtrl==threadRun && !pthis->m_rcBlock.IsRectEmpty() )
		{
			ResetEvent(pthis->m_hEvent);

			EnterCriticalSection(&pthis->m_section);
			if( pthis->m_bSkipRead )
			{
				if( pthis->m_nCurRead>=pthis->m_rcBlock.top &&
					pthis->m_nCurRead<pthis->m_rcBlock.bottom &&
					pthis->ReadBlockLine(pthis->m_nCurRead) )
				{
					pthis->m_nCurRead += pthis->m_nSkipReadRate;
				}
			}
			else
			{
				if( pthis->ReadRectBmp(pthis->m_rcBlock,pthis->m_hBlockBmp) )
				{
					pthis->m_nCurRead = pthis->m_rcBlock.bottom;
				}
			}
			LeaveCriticalSection(&pthis->m_section);

			if( pthis->m_nCurRead>=pthis->m_rcBlock.bottom && ::IsWindow(pthis->m_hWndCallback) )
			{
				SendMessage(pthis->m_hWndCallback,pthis->m_nMsgCallback,1,0);
				Sleep(5);
			}
			else
				Sleep(0);
		}
		else 
		{
			SetEvent(pthis->m_hEvent);
			DWORD time0 = GetTickCount();
			if (::IsWindow(pthis->m_hWndCallback))
			{
				SendMessage(pthis->m_hWndCallback, pthis->m_nMsgCallback, 0, 0);
			}
			DWORD time1 = GetTickCount()-time0;
			if (time1 < 20)time1 = 100;
			else time1 = time1 * 5;

			time0 = GetTickCount();
			time1 = 100;
			for (int i = 0; i < time1; i+=10)
			{
				if (GetTickCount() - time0 >= time1)
					break;

				if (pthis->m_nThreadCtrl != threadRun || !pthis->m_rcBlock.IsRectEmpty())
					break;

				Sleep(10);
			}			
		}
	}
	return 0;
}




static CString MatchDllFile(LPCTSTR fileName)
{
	TCHAR path[_MAX_PATH]={0};
	::GetModuleFileName(SmartViewDLL.hModule,path,sizeof(path)-1);
	TCHAR *pos = _tcsrchr(path,'\\');
	if( pos )pos[1] = 0;

	CString strPath = CString(path);

	CString comDLL = _T("com_gdal.dll");

	if( fileName==NULL )
		return strPath + comDLL;

	if (CString(fileName).Left(6).CompareNoCase(_T("uis://")) == 0)
		return strPath + "com_uis.dll";
	
	const TCHAR *ext = _tcsrchr(fileName,_T('.'));
	if (ext == NULL || ext[0] == '\0')return strPath + comDLL;
	
	CStringA str = ConvertTCharToChar(ext+1);
	
	for( int i=sizeof(static_extList)/sizeof(static_extList[0])-1; i>=0; i--)
	{
		if( str.CompareNoCase(static_extList[i])==0 )
		{
			if( access(ConvertTCharToChar(strPath+static_dllList[i]),0)==-1 )continue;
			return strPath+static_dllList[i];
		}
	}
	
	return strPath + comDLL;
}

void CImageRead::SetUISCacheDir(LPCTSTR dir)
{
	m_strUISCacheDir = dir;
}

BOOL CImageRead::Load(LPCTSTR lpszFileName)
{
	if( m_bLoad )
	{
		if( strcmp(ConvertTCharToChar(lpszFileName), m_chFileName)==0 )return TRUE;
		Close();
	}
	
	CString dllFile = MatchDllFile(lpszFileName);
	if( !LoadImgDll(dllFile) )return FALSE;	

	if( m_pInfoFunc==NULL )return FALSE;

	char filename[1024] = {0};
	strcpy(filename,ConvertTCharToChar(lpszFileName));
	
	isio_open_param(&m_IsAddr, ISEEIO_CONFER_LOC_FILE, filename, 0, 0);

	if( (m_pInfoStr=AllocINFOSTR())==NULL )
		return FALSE;

	if( m_pInitCacheFunc && !m_strUISCacheDir.IsEmpty() )
	{
		(*m_pInitCacheFunc)(ConvertTCharToChar(m_strUISCacheDir));
	}
	
	if( (*m_pInfoFunc)(&m_IsAddr, m_pInfoStr)!=ER_SUCCESS )
	{
		Close();
		return FALSE;
	}
	
	m_IsAddr.name = m_chFileName;

	strcpy( m_chFileName, ConvertTCharToChar(lpszFileName));
	m_bLoad = TRUE;
	Process();

	return TRUE;
}


BOOL CMutiImageRead::Load(LPCTSTR lpszFileName)
{	
	if( lpszFileName==NULL )return FALSE;

	CStringA name_a = ConvertTCharToChar(lpszFileName);

	if( m_bLoad )
	{
		if( strcmp(name_a, m_chFileName)==0 )return TRUE;
		Close();
	}
	CString dllFile = MatchDllFile(lpszFileName);
	if( !LoadImgDll(dllFile) )return FALSE;	
//	strcpy( m_chFileName, lpszFileName );
//	if( !LoadImgDll(_T("com.dll")) )return FALSE;

	if( m_pInfoFunc==NULL )return FALSE;
	isio_open_param(&m_IsAddr, ISEEIO_CONFER_LOC_FILE, name_a, 0, 0);
	
	m_pInfoStr = AllocINFOSTR();
	if( m_pInfoStr==NULL )return FALSE;
	
	if((*m_pInfoFunc)(&m_IsAddr, m_pInfoStr) != ER_SUCCESS)
	{
		Close();
		return FALSE;
	}
	strcpy( m_chFileName,  name_a);
	m_bLoad = TRUE;
	Process();
	return TRUE;
}




void CThreadImageRead::SetReadBmpInfo(int w, int h, int bitcount)
{
	if( m_hBlockBmp )DeleteObject(m_hBlockBmp);
	m_pBlockBits = NULL;

	if( m_pBmpInfo )delete[] (BYTE*)m_pBmpInfo;
	m_pBmpInfo = NULL;

	m_pBmpInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)+255*sizeof(RGBQUAD)];
	if( !m_pBmpInfo )return;
	
	UINT iUsage = DIB_RGB_COLORS;
	
	int nWidth = w, nHeight = h;

	SETBMPINFO(m_pBmpInfo);
	m_pBmpInfo->bmiHeader.biBitCount = (bitcount<=0?(unsigned short)m_pInfoStr->bitcount:bitcount);
	
	memcpy(m_pBmpInfo->bmiColors,m_pInfoStr->palette, 256*sizeof(RGBQUAD));

	if( m_pInfoStr->bitcount==8 )
	{
		for( int i=0; i<256; i++ )
		{
			if( m_pInfoStr->palette[i]!=0 )break;
		}
		if( i>=256 )
		{
			for( i=0; i<256; i++ )
			{
				m_pBmpInfo->bmiColors[i].rgbBlue = i;
				m_pBmpInfo->bmiColors[i].rgbGreen = i;
				m_pBmpInfo->bmiColors[i].rgbRed = i;
			}
		}
	}

	m_hBlockBmp = ::CreateDIBSection( NULL, m_pBmpInfo, DIB_RGB_COLORS, 
		(void**)&m_pBlockBits, NULL,0L );
}

void CThreadImageRead::SetReadRect(CRect rect, BOOL bSkipRead, int nSkipRate)
{
	if( !m_pBmpInfo )return;
	
	m_rcBlock = rect;
	m_nCurRead= rect.top;

	if( (!m_bSkipRead&&bSkipRead) || ((m_bSkipRead&&bSkipRead)&&m_nSkipReadRate!=nSkipRate) )
	{
		if( m_hLineBmp )::DeleteObject(m_hLineBmp);
		int savew = m_pBmpInfo->bmiHeader.biWidth, saveh = m_pBmpInfo->bmiHeader.biHeight;
		m_pBmpInfo->bmiHeader.biWidth *= nSkipRate; m_pBmpInfo->bmiHeader.biHeight = 1;
		m_hLineBmp = ::CreateDIBSection( NULL, m_pBmpInfo, DIB_RGB_COLORS, 
			(void**)&m_pLineBits, NULL,0L );

		memset(m_pLineBits,0, ((((m_pBmpInfo->bmiHeader.biWidth*m_pBmpInfo->bmiHeader.biBitCount+7)>>3)+3)&(~3))*m_pBmpInfo->bmiHeader.biHeight);

		m_pBmpInfo->bmiHeader.biWidth = savew;
		m_pBmpInfo->bmiHeader.biHeight= saveh;
	}

	if( m_pBlockBits!=NULL )
		memset(m_pBlockBits,0, ((((m_pBmpInfo->bmiHeader.biWidth*m_pBmpInfo->bmiHeader.biBitCount+7)>>3)+3)&(~3))*m_pBmpInfo->bmiHeader.biHeight);

	m_bSkipRead = bSkipRead;
	m_nSkipReadRate = nSkipRate;
}


BOOL CThreadImageRead::GetReadBmp(HBITMAP hBmp)
{
	if( !m_hBlockBmp || !m_pBlockBits )return FALSE;

	DIBSECTION sect;
	if( ::GetObject(hBmp,sizeof(sect),&sect)==0 )return FALSE;
	memcpy(sect.dsBm.bmBits, m_pBlockBits, sect.dsBmih.biSizeImage);
	return TRUE;
}



void CThreadImageRead::SetThreadCallback(HWND hWnd, UINT nMsg)
{
	m_hWndCallback = hWnd;
	m_nMsgCallback = nMsg;
}



void CImageRead::GetParamidRange(int * pnMin, int * pnMax)
{
	if( !IsValid() )return;
	if( pnMin!=NULL )*pnMin = 1;
	
	BOOL bOriginalImage = TRUE;
	LPSUBIMGBLOCK p = m_pInfoStr->psubimg;

	while( p!=NULL )
	{
		int nZoom = floor(1.0f/p->zoomRate);
		if( bOriginalImage ){
			if( pnMin!=NULL )*pnMin = nZoom;
			bOriginalImage = FALSE;
		}
		else{
			if( *pnMin<nZoom )
				*pnMin = nZoom;			
		}
		
		p = p->next;
	}
	
	if( pnMin!=NULL )
		*pnMin = -*pnMin*2;
	if( pnMax!=NULL )
		*pnMax = 20;
}


void CMutiImageRead::GetParamidRange(int * pnMin, int * pnMax)
{
	if( !IsValid() )return;
	if( pnMin!=NULL )*pnMin = 1;
	BOOL bOriginalImage = TRUE;
	LPSUBIMGBLOCK p = m_pInfoStr->psubimg;
	while( p!=NULL )
	{
		int nZoom = floor(1.0f/p->zoomRate);
		if( bOriginalImage ){
			if( pnMin!=NULL )*pnMin = nZoom;
			bOriginalImage = FALSE;
		}
		else{
			if( *pnMin<nZoom )
				*pnMin = nZoom;			
		}
		p = p->next;
	}
	if( pnMin!=NULL )
		*pnMin = -*pnMin*2;
	if( pnMax!=NULL )
		*pnMax = 20;
}
CRoatableMutiImageReadEx::CRoatableMutiImageReadEx()
{
	m_bIsAttach = FALSE;
}
CRoatableMutiImageReadEx::~CRoatableMutiImageReadEx()
{
}
BOOL CRoatableMutiImageReadEx::Attach()
{
	if (!m_bLoad)
	{
		return FALSE;
	}
	if( m_bIsAttach ) return TRUE;	
	if (strlen(m_chFileName)==0)
	{
		Close();
		return FALSE;
	}	
//	IRWP_API_GET_IMG_INFO	infoFunc = m_pInfoFunc;
	if( m_pInfoFunc==NULL )return FALSE;
	isio_open_param(&m_IsAddr, ISEEIO_CONFER_LOC_FILE, m_chFileName, 0, 0);
	if( m_pInfoStr!=NULL )
	{
		FreeINFOSTR(m_pInfoStr);
		m_pInfoStr = NULL;
	}
	m_pInfoStr = AllocINFOSTR();
	if( m_pInfoStr==NULL )
	{		
		Close();		
		return FALSE;
	}
// 	int iii = (*m_pInfoFunc)(&m_IsAddr, m_pInfoStr);
// 	if (iii==5)
// 	{
// 		CString str = m_chFileName;
// 		Close();
// 		Load(str);
// 	}
	if((*m_pInfoFunc)(&m_IsAddr, m_pInfoStr) != ER_SUCCESS)
	{
		Close();
		return FALSE;
	}
	m_bIsAttach = TRUE;
	return TRUE;	
}
BOOL CRoatableMutiImageReadEx::DeAttach()
{
	if(!m_pInfoStr||!m_hImgDll||!m_pInfoFunc||!m_pReadFunc)
	{
		Close();
		return FALSE;
	}
	if( m_pInfoStr!=NULL )
	{
// 		FreeINFOSTR(m_pInfoStr);
// 		m_pInfoStr = NULL;
		FreePartINFOSTR(m_pInfoStr);
		
	}
	if(m_pCloseFunc)m_pCloseFunc(m_pInfoStr,m_chFileName);
	m_bIsAttach = FALSE;
	return TRUE;
}
int  CRoatableMutiImageReadEx::SetCurReadZoom(float zoomRate)
{
	m_lfZoomRate = zoomRate;
	return	CMutiImageRead::SetCurReadZoom(zoomRate);
}
float CRoatableMutiImageReadEx::GetZoomRateEx()
{
	return m_lfZoomRate;
}
void CRoatableMutiImageReadEx::Process()
{
	m_bIsAttach = TRUE;
}
#define MAX_IMGREAD   10
CThreadMutiImageRead::CThreadMutiImageRead():m_rcBlock(0,0,0,0)
{
	m_hReadThread = NULL;
	m_hEvent = NULL;
	m_nThreadCtrl = threadDead;
	m_hBlockBmp = NULL;
	m_hLineBmp = NULL;
	m_hWndCallback= NULL;
	m_nMsgCallback = 0;
	m_pBlockBits = NULL;
	m_pLineBits = NULL;
	m_nCurRead = 0;
	m_bSkipRead = FALSE;
	m_nSkipReadRate = 1;
	m_pBmpInfo = NULL;
	m_pImgParArr = NULL;
	m_pArrSkipInfos = NULL;
	m_bChangeImageRate = TRUE;
}
CThreadMutiImageRead::~CThreadMutiImageRead()
{
	DestroyReadThread();
	for (int i=0;i<m_arrImageReads.GetSize();i++)
	{
		delete m_arrImageReads[i];
	}
	ClearAllReadBmpInfo();
	m_arrImageReads.RemoveAll();
	if( m_hBlockBmp )DeleteObject(m_hBlockBmp);
	if( m_hLineBmp )DeleteObject(m_hLineBmp);
	if( m_pBmpInfo )delete[] (BYTE*)m_pBmpInfo;
}
BOOL CThreadMutiImageRead::AttachImage(LPCTSTR fileName, BOOL bInit)
{
	if (bInit)
	{
		CRoatableMutiImageReadEx* p = new CRoatableMutiImageReadEx;
		if (!p) return FALSE;
		if( !p->Load(fileName) )
			return FALSE;
		p->SetCurReadZoom(1.0);
		p->DeAttach();
		m_arrImageReads.Add(p);
		return TRUE;
	}
	BOOL bFind = FALSE;
	int i;
	for (i=0;i<m_arrRecentImageReads.GetSize();i++)
	{
		if(_tcscmp(fileName,m_arrRecentImageReads[i]->GetImageName())==0)
		{
			bFind = TRUE;
			break;
		}
	}
	if (bFind)
	{
		if (i == m_arrRecentImageReads.GetSize()-1)
		{
			return TRUE;
		}
		CRoatableMutiImageReadEx* p = m_arrRecentImageReads.GetAt(i);
		m_arrRecentImageReads.RemoveAt(i);
		m_arrRecentImageReads.Add(p);
		return TRUE;
	}
	else
	{
		CRoatableMutiImageReadEx *p = NULL;
		for (i = m_arrImageReads.GetSize()-1;i>=0;i--)
		{
			p = m_arrImageReads.GetAt(i);
			if (!p)continue;
			if (_tcscmp(p->GetImageName(),fileName)==0)
			{
				if(!p->Attach()) 
				{
					delete p;				
					m_arrImageReads.RemoveAt(i);
					m_arrImageReads.FreeExtra();
					return FALSE;
				}
				break;
			}
		}		
		if (i>=0)
		{
			if (m_arrRecentImageReads.GetSize()<MAX_IMGREAD)
			{
				m_arrRecentImageReads.Add(p);
				return TRUE;
			}
			else
			{	
				m_arrRecentImageReads.GetAt(0)->DeAttach();
				m_arrRecentImageReads.RemoveAt(0);
				m_arrRecentImageReads.Add(p);
				return TRUE;
			}		
		}
	}
	return FALSE;
}

BOOL CThreadMutiImageRead::InsertImage(int idx, LPCTSTR fileName)
{
	if (idx<0||idx>=m_arrImageReads.GetSize())
	{
		return FALSE;
	}
	CRoatableMutiImageReadEx* p = new CRoatableMutiImageReadEx;
	if (!p) return FALSE;
	if( !p->Load(fileName) )
		return FALSE;
	p->SetCurReadZoom(1.0);
	p->DeAttach();
	m_arrImageReads.InsertAt(idx,p);
	return TRUE;
}
BOOL CThreadMutiImageRead::Detach(LPCTSTR fileName)
{
	BOOL bFind = FALSE;
	int i;
	for (i=0;i<m_arrRecentImageReads.GetSize();i++)
	{
		if(_tcscmp(fileName,m_arrRecentImageReads[i]->GetImageName())==0)
		{
			bFind = TRUE;
			break;
		}
	}
	if (bFind)
	{	
		CRoatableMutiImageReadEx* p = m_arrRecentImageReads.GetAt(i);
		m_arrRecentImageReads.RemoveAt(i);
		for (i=m_arrImageReads.GetSize()-1;i>=0;i--)
		{
			if(m_arrImageReads.GetAt(i)==p)
			{
				m_arrImageReads.RemoveAt(i);
				delete p;
				return TRUE;
			}
		}
		return FALSE;
	}
	else
	{
		CRoatableMutiImageReadEx *p = NULL;
		for (int i = m_arrImageReads.GetSize()-1;i>=0;i--)
		{
			p = m_arrImageReads.GetAt(i);
			if (!p)continue;
			if (_tcscmp(p->GetImageName(),fileName)==0)
			{
				delete p;				
				m_arrImageReads.RemoveAt(i);
				return TRUE;			
			}
		}		
		return FALSE;		
	}
}
void CThreadMutiImageRead::Lock()
{
	if( IsReadThreadStarted() )EnterCriticalSection(&m_section);
}
void CThreadMutiImageRead::Unlock()
{
	if( IsReadThreadStarted() )LeaveCriticalSection(&m_section);
}
CSize CThreadMutiImageRead::GetImageSize(LPCTSTR fileName)
{	
	int i;
	for (i=m_arrRecentImageReads.GetSize()-1;i>=0;i--)
	{
		if(_tcscmp(fileName,m_arrRecentImageReads[i]->GetImageName())==0)
		{
			return m_arrRecentImageReads[i]->GetImageSize();
		}
	}	
	if ((i = m_arrImageReads.GetSize())>0)
	{
		i--;
		for (;i>=0;i--)
		{
			CRoatableMutiImageReadEx* p = m_arrImageReads.GetAt(i);
			if(_tcscmp(fileName,p->GetImageName())==0)
			{
				return p->GetImageSize();
			}
		}		
	}
	return CSize(-1,-1);
}
CSize CThreadMutiImageRead::GetImageOriginalSize(LPCTSTR fileName)
{
	int i;
	for (i=m_arrRecentImageReads.GetSize()-1;i>=0;i--)
	{
		if(_tcscmp(fileName,m_arrRecentImageReads[i]->GetImageName())==0)
		{
			return m_arrRecentImageReads[i]->GetImageOriginalSize();
		}
	}	
	if ((i = m_arrImageReads.GetSize())>0)
	{
		i--;
		for (;i>=0;i--)
		{
			CRoatableMutiImageReadEx* p = m_arrImageReads.GetAt(i);
			if(_tcscmp(fileName,p->GetImageName())==0)
			{
				return p->GetImageOriginalSize();
			}
		}		
	}
	return CSize(-1,-1);
}
int  CThreadMutiImageRead::SetImgZoomRate(int idx, float zoomRate)
{
	if(m_arrImageReads.GetSize()>0)
	{
		CRoatableMutiImageReadEx* p = m_arrImageReads.GetAt(idx);
		return p->SetCurReadZoom(zoomRate);
	}
	return -1;
}
float  CThreadMutiImageRead::GetCurReadZoom(LPCTSTR fileName)
{
	for (int i=m_arrImageReads.GetSize()-1;i>=0;i--)
	{
		CRoatableMutiImageReadEx* p = m_arrImageReads.GetAt(i);		
		if (_tcscmp(fileName,p->GetImageName())==0)
		{
			p->GetCurReadZoom();
		}
	}
	return 0;	
}
float  CThreadMutiImageRead::GetCurReadZoom(int idx)
{
	if(m_arrImageReads.GetSize()<idx+1||idx<0) return 0; 
	CRoatableMutiImageReadEx*  p = m_arrImageReads.GetAt(idx);
	return p->GetCurReadZoom();	
}

void CThreadMutiImageRead::Close()
{
	DestroyReadThread();
	for (int i=0;i<m_arrImageReads.GetSize();i++)
	{
		m_arrImageReads[i]->Close();
	}
	m_arrImageReads.RemoveAll();
	m_arrRecentImageReads.RemoveAll();
	
}

BOOL CThreadMutiImageRead::ReadRectBmp(int idx ,CRect rect, HBITMAP hBmp, BOOL bSkipRead, int nSkipRate)
{
	if (m_arrRecentImageReads.GetSize()<=0)
	{
		return FALSE;
	}
	const CArray<ImgParaItem*,ImgParaItem*> *p0 = (const CArray<ImgParaItem*,ImgParaItem*>*)m_pImgParArr;
	
	if( !bSkipRead )
	{		
		Lock();
		if(!AttachImage((*p0)[idx]->m_strImgName))return FALSE;
		BOOL ret = m_arrRecentImageReads.GetAt(m_arrRecentImageReads.GetSize()-1)->ReadRectBmp(rect,hBmp);
		Unlock();
		return ret;	
	}
	Lock();
	if(!AttachImage((*p0)[idx]->m_strImgName))return FALSE;
	// save member variants
	SAVE_VAR2(CRect,m_rcBlock);
	SAVE_VAR(BOOL,m_bSkipRead);
	SAVE_VAR(int,m_nCurRead);
	SAVE_VAR(int,m_nSkipReadRate);
	SAVE_VAR(HBITMAP,m_hBlockBmp);
	SAVE_VAR(HBITMAP,m_hLineBmp);
	SAVE_VAR(BYTE *,m_pBlockBits);
	SAVE_VAR(BYTE *,m_pLineBits);
	SAVE_VAR(BITMAPINFO *,m_pBmpInfo);
	LPINFOSTR pInfoStr = m_arrRecentImageReads.GetAt(m_arrRecentImageReads.GetSize()-1)->m_pInfoStr;
	SetReadBmpInfo(rect.Width(),rect.Height(),pInfoStr->bitcount);
	
	CRect rect2 = rect;
	rect2.left *= nSkipRate; rect2.right *= nSkipRate;
	rect2.top *= nSkipRate; rect2.bottom *= nSkipRate;
	SetCurReadRect(rect2,bSkipRead,nSkipRate);
	for( int i=0; i<rect.Height(); i++)
	{
		ReadCurBlockLine(m_nCurRead);
		m_nCurRead += nSkipRate;
	}
	
	int linesize = (m_pBmpInfo->bmiHeader.biWidth*(m_pBmpInfo->bmiHeader.biBitCount>>3)+3)&(~3);
	DWORD saveorder = pInfoStr->order;
	pInfoStr->order = 0;
	m_arrRecentImageReads.GetAt(m_arrRecentImageReads.GetSize()-1)->PasteBits(hBmp,rect,m_pBlockBits,linesize*m_pBmpInfo->bmiHeader.biHeight);
	pInfoStr->order = saveorder;
	
	if( m_hBlockBmp )DeleteObject(m_hBlockBmp);
	if( m_hLineBmp )DeleteObject(m_hLineBmp);
	if( m_pBmpInfo )delete[] (BYTE*)m_pBmpInfo;
	
	BOOL ret = TRUE;
	
	// restore member variants
	RESTORE_VAR(CRect,m_rcBlock);
	RESTORE_VAR(BOOL,m_bSkipRead);
	RESTORE_VAR(int,m_nCurRead);
	RESTORE_VAR(int,m_nSkipReadRate);
	RESTORE_VAR(HBITMAP,m_hBlockBmp);
	RESTORE_VAR(HBITMAP,m_hLineBmp);
	RESTORE_VAR(BYTE *,m_pBlockBits);
	RESTORE_VAR(BYTE *,m_pLineBits);
	RESTORE_VAR(BITMAPINFO *,m_pBmpInfo);
	
	Unlock();
	return ret;
}

BOOL CThreadMutiImageRead::ReadCurRectBmp(CRect rect, HBITMAP hBmp)
{
	if (m_arrRecentImageReads.GetSize()<=0)
	{
		return FALSE;
	}
	Lock();
	BOOL ret = m_arrRecentImageReads.GetAt(m_arrRecentImageReads.GetSize()-1)->ReadRectBmp(rect,hBmp);
	Unlock();
	return ret;
}
BOOL CThreadMutiImageRead::InitReadThread()
{
	m_hReadThread = CreateThread(
						  NULL,
						  0,
						  ReadThread,
						  (LPVOID)this,	
						  (DWORD)CREATE_SUSPENDED,
						  NULL);
	if(m_hReadThread==NULL)
	{		
		return FALSE;
	}
	SetThreadPriority(m_hReadThread,THREAD_PRIORITY_IDLE);
	m_nThreadCtrl = threadPause; // suspend
	InitializeCriticalSection(&m_section);
	InitializeCriticalSection(&g_section);
	m_hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	return TRUE;
}
void CThreadMutiImageRead::DestroyReadThread()
{
	if( m_hReadThread )
	{
		m_nThreadCtrl = threadDead;
		if( WaitForSingleObject( m_hReadThread,INFINITE )!=WAIT_OBJECT_0 )
		{
			TerminateThread( m_hReadThread, 1 );
		}
		CloseHandle(m_hReadThread);
		m_hReadThread = NULL;
		DeleteCriticalSection(&m_section);
		DeleteCriticalSection(&g_section);
	}
	if( m_hEvent )
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}
void CThreadMutiImageRead::BeginReadThread()
{
	m_nThreadCtrl = threadRun; // go
	if( m_hReadThread )ResumeThread(m_hReadThread);
}
void CThreadMutiImageRead::EndReadThread()
{
	m_nThreadCtrl = threadPause; // suspend
	WaitForSingleObject(m_hEvent,INFINITE);
	if( m_hReadThread )SuspendThread(m_hReadThread);
}
BOOL CThreadMutiImageRead::ReadCurBlockLine(int nCurLine)
{
	if( !m_hLineBmp || !m_pBlockBits || !m_pLineBits ||!m_pBmpInfo )return FALSE;
	BYTE *bit0 = m_pBlockBits, *bit1 = m_pLineBits;
	CRect rcLine = m_rcBlock;
	rcLine.top = nCurLine;  rcLine.bottom = rcLine.top+1;
	if( !ReadCurRectBmp(rcLine,m_hLineBmp) )
		return TRUE;
	int linesize = ((m_pBmpInfo->bmiHeader.biWidth*m_pBmpInfo->bmiHeader.biBitCount+7)/8+3)&(~3);
	bit0 += (linesize*(m_pBmpInfo->bmiHeader.biHeight-1-(nCurLine-m_rcBlock.top)/m_nSkipReadRate));
	BYTE *t0, *t1;
	t0 = bit0;  t1 = bit1;
	if( m_pBmpInfo->bmiHeader.biBitCount==24 )
	{
		for( int j=m_rcBlock.Width()/m_nSkipReadRate-1; j>=0; j--)
		{
			*t0++ = t1[0]; *t0++ = t1[1]; *t0++ = t1[2];
			t1 += 3*m_nSkipReadRate;
		}
	}
	else if( m_pBmpInfo->bmiHeader.biBitCount==8 )
	{
		for( int j=m_rcBlock.Width()/m_nSkipReadRate-1; j>=0; j--)
		{
			*t0++ = t1[0];
			t1 += m_nSkipReadRate;
		}
	}
	return TRUE;
}
DWORD CThreadMutiImageRead::ReadThread(LPVOID pParam)
{
	CoInitialize(NULL);
    CThreadMutiImageRead* pthis = (CThreadMutiImageRead*)pParam;
	const CArray<ImgSkipInfo*,ImgSkipInfo*>* p	= (const CArray<ImgSkipInfo*,ImgSkipInfo*>*)pthis->m_pArrSkipInfos;
	const CArray<ImgParaItem*,ImgParaItem*> *p0 = (const CArray<ImgParaItem*,ImgParaItem*>*)pthis->m_pImgParArr;
	while( pthis->m_nThreadCtrl!=threadDead )
	{   
		if( pthis->m_nThreadCtrl==threadRun && pthis->m_arrImgIdx.GetSize()>0)
		{
			ResetEvent(pthis->m_hEvent);
			EnterCriticalSection(&g_section);
			EnterCriticalSection(&pthis->m_section);
			for (int i=0;i<pthis->m_arrImgIdx.GetSize();i++)
			{
				if (pthis->m_bChangeImageRate)
				{
					goto end;
				}
				if(!pthis->AttachImage((*p0)[pthis->m_arrImgIdx[i]]->m_strImgName))continue;
				pthis->SetCurReadBmpInfo(pthis->m_arrOldImgRect[i].Width(),pthis->m_arrOldImgRect[i].Height(),24);
				pthis->SetCurReadRect(pthis->m_arrNewImgRect[i],(*p)[pthis->m_arrImgIdx[i]]->bSkipRead,(*p)[pthis->m_arrImgIdx[i]]->nSkipReadRate);
				if( pthis->m_bSkipRead )
				{
					while (pthis->m_nCurRead<pthis->m_rcBlock.bottom)
					{
						if (pthis->m_bChangeImageRate)
						{
							goto end;
						}
						if( pthis->m_nCurRead>=pthis->m_rcBlock.top &&							
							pthis->ReadCurBlockLine(pthis->m_nCurRead) )
						{
							pthis->m_nCurRead += pthis->m_nSkipReadRate;
						}
					}				
				}
				else
				{
					if( pthis->ReadCurRectBmp(pthis->m_rcBlock,pthis->m_hBlockBmp) )
					{					
						pthis->m_nCurRead = pthis->m_rcBlock.bottom;
					}
				}
				if( pthis->m_nCurRead>=pthis->m_rcBlock.bottom)
				{
				}
			}	
end:
			LeaveCriticalSection(&pthis->m_section);
			LeaveCriticalSection(&g_section);
			if( i>=pthis->m_arrImgIdx.GetSize() && ::IsWindow(pthis->m_hWndCallback) )
			{
				SendMessage(pthis->m_hWndCallback,pthis->m_nMsgCallback,1,0);
			}
			else
				Sleep(0);
		}
		else 
		{
			SetEvent(pthis->m_hEvent);
			if( ::IsWindow(pthis->m_hWndCallback) )
				SendMessage(pthis->m_hWndCallback,pthis->m_nMsgCallback,0,0);
			Sleep(100);
		}
	}
	CoUninitialize();
	return 0;
}
#define SETBMPINFO0(pInfo,m_pInfoStr) 	pInfo->bmiHeader.biSize = sizeof(pInfo->bmiHeader);\
	pInfo->bmiHeader.biWidth = nWidth;\
	pInfo->bmiHeader.biHeight = nHeight;\
	pInfo->bmiHeader.biSizeImage = m_pInfoStr->bit_data_len;\
	pInfo->bmiHeader.biPlanes = 1;\
	pInfo->bmiHeader.biBitCount = (unsigned short)m_pInfoStr->bitcount;\
	pInfo->bmiHeader.biCompression = BI_RGB;\
	pInfo->bmiHeader.biXPelsPerMeter = 0;\
	pInfo->bmiHeader.biYPelsPerMeter = 0;\
	pInfo->bmiHeader.biClrUsed = 0;\
	pInfo->bmiHeader.biClrImportant = 0;

void CThreadMutiImageRead::SetCurReadBmpInfo(int w, int h, int bitcount)
{
	if (m_arrImageReads.GetSize()<=0) return;
	m_pBlockBits = NULL;
	if( m_pBmpInfo )delete[] (BYTE*)m_pBmpInfo;
	m_pBmpInfo = NULL;
	m_pBmpInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)+255*sizeof(RGBQUAD)];
	if( !m_pBmpInfo )return;
	UINT iUsage = DIB_RGB_COLORS;
	int nWidth = w, nHeight = h;
	CRoatableMutiImageReadEx* p = m_arrRecentImageReads.GetAt(m_arrRecentImageReads.GetSize()-1);
	SETBMPINFO0(m_pBmpInfo,(p->m_pInfoStr));
	m_pBmpInfo->bmiHeader.biBitCount = (bitcount<=0?(unsigned short)(p->m_pInfoStr->bitcount):bitcount);
	memcpy(m_pBmpInfo->bmiColors,p->m_pInfoStr->palette, 256*sizeof(RGBQUAD));
	if( p->m_pInfoStr->bitcount==8 )
	{
		for( int i=0; i<256; i++ )
		{
			if( p->m_pInfoStr->palette[i]!=0 )break;
		}
		if( i>=256 )
		{
			for( i=0; i<256; i++ )
			{
				m_pBmpInfo->bmiColors[i].rgbBlue = i;
				m_pBmpInfo->bmiColors[i].rgbGreen = i;
				m_pBmpInfo->bmiColors[i].rgbRed = i;
			}
		}
	}
	m_hBlockBmp = ::CreateDIBSection( NULL, m_pBmpInfo, DIB_RGB_COLORS, 
		(void**)&m_pBlockBits, NULL,0L );
	m_arrHbitmap.Add(m_hBlockBmp);
}

void CThreadMutiImageRead::SetReadBmpInfo(int w, int h, int bitcount)
{
	if (m_arrImageReads.GetSize()<=0) return;
	m_pBlockBits = NULL;
	if( m_pBmpInfo )delete[] (BYTE*)m_pBmpInfo;
	m_pBmpInfo = NULL;
	m_pBmpInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)+255*sizeof(RGBQUAD)];
	if( !m_pBmpInfo )return;
	UINT iUsage = DIB_RGB_COLORS;
	int nWidth = w, nHeight = h;
	CRoatableMutiImageReadEx* p = m_arrRecentImageReads.GetAt(m_arrRecentImageReads.GetSize()-1);
	SETBMPINFO0(m_pBmpInfo,(p->m_pInfoStr));
	m_pBmpInfo->bmiHeader.biBitCount = (bitcount<=0?(unsigned short)(p->m_pInfoStr->bitcount):bitcount);
	memcpy(m_pBmpInfo->bmiColors,p->m_pInfoStr->palette, 256*sizeof(RGBQUAD));
	if( p->m_pInfoStr->bitcount==8 )
	{
		for( int i=0; i<256; i++ )
		{
			if( p->m_pInfoStr->palette[i]!=0 )break;
		}
		if( i>=256 )
		{
			for( i=0; i<256; i++ )
			{
				m_pBmpInfo->bmiColors[i].rgbBlue = i;
				m_pBmpInfo->bmiColors[i].rgbGreen = i;
				m_pBmpInfo->bmiColors[i].rgbRed = i;
			}
		}
	}
	m_hBlockBmp = ::CreateDIBSection( NULL, m_pBmpInfo, DIB_RGB_COLORS, 
		(void**)&m_pBlockBits, NULL,0L );
}

void CThreadMutiImageRead::ClearAllReadBmpInfo()
{
	for (int i=m_arrHbitmap.GetSize()-1;i>=0;i--)
	{
		if (m_arrHbitmap[i])
		{
			DeleteObject(m_arrHbitmap[i]);
		}
	}
	m_arrHbitmap.RemoveAll();
	//m_hBlockBmp = NULL;
}
void CThreadMutiImageRead::SetReadImgInfo(const CArray<int,int> &imgIdx ,const CArray<CRect,CRect> &oldimgRect,const CArray<CRect,CRect> &newimgRect)
{	
	m_arrImgIdx.Copy(imgIdx);
	m_arrImgIdx.FreeExtra();
	m_arrOldImgRect.Copy(oldimgRect);
	m_arrOldImgRect.FreeExtra();
	m_arrNewImgRect.Copy(newimgRect);
	m_arrNewImgRect.FreeExtra();
}
void CThreadMutiImageRead::ClearReadImgInfo()
{
	m_arrImgIdx.RemoveAll();
	m_arrOldImgRect.RemoveAll();
	m_arrNewImgRect.RemoveAll();
}
void CThreadMutiImageRead::SetCurReadRect(CRect rect, BOOL bSkipRead, int nSkipRate)
{
	if( !m_pBmpInfo )return;
	m_rcBlock = rect;
	m_nCurRead= rect.top;
	if( m_hLineBmp )::DeleteObject(m_hLineBmp);
	m_hLineBmp = NULL;
	if( bSkipRead )
	{		
		int savew = m_pBmpInfo->bmiHeader.biWidth, saveh = m_pBmpInfo->bmiHeader.biHeight;
		m_pBmpInfo->bmiHeader.biWidth *= nSkipRate; m_pBmpInfo->bmiHeader.biHeight = 1;
		m_hLineBmp = ::CreateDIBSection( NULL, m_pBmpInfo, DIB_RGB_COLORS, 
			(void**)&m_pLineBits, NULL,0L );
		m_pBmpInfo->bmiHeader.biWidth = savew;
		m_pBmpInfo->bmiHeader.biHeight= saveh;
	}
	if( m_pBlockBits!=NULL )
		memset(m_pBlockBits,0, ((((m_pBmpInfo->bmiHeader.biWidth*m_pBmpInfo->bmiHeader.biBitCount+7)>>3)+3)&(~3))*m_pBmpInfo->bmiHeader.biHeight);
	m_bSkipRead = bSkipRead;
	m_nSkipReadRate = nSkipRate;
}
BOOL CThreadMutiImageRead::GetReadBmp(CArray<HBITMAP,HBITMAP> &arrhBmp)
{
	if( !m_hBlockBmp || !m_pBlockBits )return FALSE;
	if (m_arrHbitmap.GetSize()<=0) return FALSE;
	arrhBmp.Copy(m_arrHbitmap);
	return TRUE;
}
void CThreadMutiImageRead::SetThreadCallback(HWND hWnd, UINT nMsg)
{
	m_hWndCallback = hWnd;
	m_nMsgCallback = nMsg;
}
void CThreadMutiImageRead::SetImgParmasArr(/*const CArray<ImgParaItem,ImgParaItem&>*/const void *pImgParArr)
{
	m_pImgParArr = pImgParArr;
}
void CThreadMutiImageRead::SetArrSkipInfos(const void *pArrSkipInfos)
{
	m_pArrSkipInfos = pArrSkipInfos;
}
int  CThreadMutiImageRead:: GetParamidRange(int idx,int * pnMin, int * pnMax)
{
	if (m_arrImageReads.GetSize()>idx&&idx>=0)
	{
		CRoatableMutiImageReadEx* p = m_arrImageReads.GetAt(idx);
		p->GetParamidRange(pnMin, pnMax );
	}
	return 0;
}
void  CThreadMutiImageRead::SetRateChangeFlag(BOOL flag )
{
	m_bChangeImageRate = flag;
}