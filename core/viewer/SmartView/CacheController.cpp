// CacheController.cpp: implementation of the CCacheLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glew.h"
#include "CacheController.h"
#include "math.h"
#include "GlobalFunc.h"
#include "DrawLineDC.h"
#include "GPUTexture.h"
#include "SmartViewFunctions.h"
#include "GlobalFunc.h"
#include "PlotWChar.h"
#include "RasterLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern BYTE clrTable_CAD[];
extern int FlagFromAngle(float ang);
extern bool RotateBlock(LPBYTE lpSrc, CRect rcSrc, int nColor, LPBYTE lpDest, int flag);

MyNameSpaceBegin


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgDataEncoder::CProgDataEncoder()
{
	m_pTmpMem = NULL;
	m_nTmpMemLen = 0;
}

CProgDataEncoder::~CProgDataEncoder()
{
	if( m_pTmpMem )delete[] m_pTmpMem;
}


BOOL CProgDataEncoder::Init(int type, long param1, long param2)
{
	return TRUE;
}


BOOL CProgDataEncoder::Encode(void* p, long n, void** pp, long* pn)
{
	if( !pp && !pn )return TRUE;
	
	BYTE *pa = (BYTE*)p;
	BYTE *pb = 0;
	BYTE *pc = pa+n;
	
	if( m_nTmpMemLen<(n<<1) )
	{
		BYTE* pTmp=NULL;
		pTmp = (BYTE*)new BYTE[n<<1];
		if( !pTmp )return FALSE;
		
		if( m_pTmpMem )delete[] m_pTmpMem;
		m_pTmpMem = pTmp;
		m_nTmpMemLen = (n<<1);
	}
	
	long n1 = 0;
	BYTE n2 = 0, n3 = 0;
	if( !pp && pn )
	{
		while( pa<pc )
		{
			n3 = 0;
			n2 = *pa;
			while( pa<pc && *pa==n2 && n3<0xff  )
			{ 
				pa++; 
				n3++; 
			}		
			n1++;	
		}
		
		n1 = (n1<<1);
		*pn=n1;
		return TRUE;
	}
	
	pb = m_pTmpMem;
	pa = (BYTE*)p;
	
	while( pa<pc )
	{
		n3 = 0;
		n2 = *pa;	
		while( pa<pc && *pa==n2 && n3<0xff )
		{ 
			pa++; 
			n3++; 
		}	
		*pb++ = n3;
		*pb++ = n2;		
	}
	
	n1 = (pb-m_pTmpMem);
	*pp = (BYTE*)new BYTE[n1];
	if( !(*pp) )return FALSE;
	
	memcpy(*pp,m_pTmpMem,n1);
	if( pn )*pn = n1;
	
	return TRUE;
}

BOOL CProgDataEncoder::Decode(void* p, long n, void* p2, long* pn)
{
	if( !p2 && !pn )return FALSE;
	
	BYTE *pa = (BYTE*)p;
	BYTE *pb = (BYTE*)p2;
	BYTE *pc = pa+n;
	
	//只计算解压长度
	if( !p2 && pn )
	{
		*pn = 0;
		while( pa<pc )
		{			
			*pn += pa[0];	
			pa += 2;
		}
		return TRUE;
	}
	
	//解压
	long n1=0;
	while( pa<pc )
	{		
		memset(pb,pa[1],pa[0]);
		pb += pa[0];
		n1 += pa[0];	
		pa += 2;	
	}
	
	if( pn )*pn = n1;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCacheData::CCacheData():
m_szBlock(0,0)
{
	m_nBlockSum = 0;
	m_pFillContext = NULL;
	m_nFillBlkIdx = 0;

	m_bTransparentOverlap = TRUE;
	m_bSmoothZoom = TRUE;
	m_clrBack = 0;
	m_rcFillBlk = CRect(0, 0, 0, 0);
}

CCacheData::~CCacheData()
{
	Destroy();
}


BOOL CCacheData::Init(CSize szBlock, int nBlock, DWORD clrBK)
{
	m_szBlock = szBlock;
	m_nBlockSum = nBlock;
	m_clrBack = clrBK;
	return TRUE;
}


BOOL CCacheData::AskforMoreBlocks(int num)
{
	return FALSE;
}


void CCacheData::Destroy()
{
	m_szBlock = CSize(0,0);
	m_nBlockSum = 0;
	
	m_pFillContext = NULL;
	m_nFillBlkIdx = 0;
}

CSize CCacheData::GetBlockSize()
{
	return m_szBlock;
}

int CCacheData::GetBlockCount()
{
	return m_nBlockSum;
}


int CCacheData::GetBlockDecodedState(int blkIdx)
{
	return DecodedItem::state_decoded;
}


void CCacheData::SetBlockDecodedState(int blkIdx, int state)
{
}


void CCacheData::ClearAll()
{
	for( int i=0; i<m_nBlockSum; i++)
	{
		ClearBlock(i);
	}
}


void CCacheData::EnableTransparentOverlap(BOOL bEnable)
{
	m_bTransparentOverlap = bEnable;
}


BOOL CCacheData::IsTransparentOverlap()
{
	return m_bTransparentOverlap;
}

void CCacheData::EnableSmoothZoom(BOOL bEnable)
{
	m_bSmoothZoom = bEnable;
}


BOOL CCacheData::IsSmoothZoom()
{
	return m_bSmoothZoom;
}


void CCacheData::BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock)
{
	m_pFillContext = p;
	m_nFillBlkIdx = blkIdx;
	m_rcFillBlk = CRect(0, 0, 0, 0);
}

void CCacheData::SetFillBlockRect(CRect rect)
{
	m_rcFillBlk = rect;
}

void CCacheData::EndFillBlock()
{
	m_pFillContext = NULL;
	m_nFillBlkIdx = -1;
}

void CCacheData::BeginDisplayBlock()
{
}
void CCacheData::EndDisplayBlock()
{
}


CChgBmpOfDC::CChgBmpOfDC(HDC hdc, HBITMAP hBmp)
{
	m_hDC = hdc;
	m_hBmpOld = (HBITMAP)::SelectObject(hdc,hBmp);
}


CChgBmpOfDC::~CChgBmpOfDC()
{
	m_hBmpOld = (HBITMAP)::SelectObject(m_hDC,m_hBmpOld);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCacheBmpData::CCacheBmpData()
{
	m_clrBack = 0;
	m_nBitCount = 0;
	m_pRGBQuad = NULL;
	m_pCacheBmpInfo = NULL;

	m_pHBmps = NULL;
	m_ppBits = NULL;

	m_pCurDataEncoder = NULL;
	m_pDefDataEncoder = NULL;
	
	m_pEncodedItems    = NULL;
	m_pDecodedItems = NULL;
	m_nDecodedItemCount = 0;

	m_hMemDC = NULL;
	m_hTemDC = NULL;

	SetColorTable(24,NULL);

	m_fRotAng = 0.0f;
	m_fKX = m_fKY = 1.0f;

	m_pRotateBuf = NULL;
	m_nBufSize = 0;

	m_hSaveBmp = NULL;
}

CCacheBmpData::~CCacheBmpData()
{
	Destroy();
}

void CCacheBmpData::SetColorTable(int bitCount, RGBQUAD* pClrTbl)
{
  	if( bitCount!=24 && bitCount!=8 )
		return;
	
	if( bitCount==8 )
	{
		if( m_pRGBQuad!=NULL )
			delete m_pRGBQuad;
		
		m_pRGBQuad = new RGBQUAD[256];
		if( !m_pRGBQuad )
			return;

		if( pClrTbl!=NULL )
			memcpy(m_pRGBQuad,pClrTbl,sizeof(RGBQUAD)*256);
		else
		{
			for( int i=0; i<256; i++)
			{						
				m_pRGBQuad[i].rgbRed=clrTable_CAD[i*3];
				m_pRGBQuad[i].rgbGreen=clrTable_CAD[i*3+1];
				m_pRGBQuad[i].rgbBlue=clrTable_CAD[i*3+2];
				m_pRGBQuad[i].rgbReserved=0;
			}
		}
	}
	m_nBitCount = bitCount;
}


void CCacheBmpData::CreateProgDataEncoder()
{
	m_pDefDataEncoder = new CProgDataEncoder;
	m_pCurDataEncoder = m_pDefDataEncoder;
}

void SetbmiHeader(BITMAPINFO *p, int w, int h, int bitcount)
{
	memset(&p->bmiHeader,0,sizeof(p->bmiHeader));

	int linesize = w*(bitcount>>3);
	linesize = (linesize+3)&(~3);

	p->bmiHeader.biSize	= sizeof(BITMAPINFOHEADER);
	p->bmiHeader.biWidth		= w;
	p->bmiHeader.biHeight		= h;
	p->bmiHeader.biSizeImage	= h*linesize;
	p->bmiHeader.biPlanes		= 1;
	p->bmiHeader.biCompression	= BI_RGB;
	p->bmiHeader.biClrUsed		= 0;
	p->bmiHeader.biBitCount		= bitcount;
	p->bmiHeader.biClrImportant = 0;
}


BOOL CCacheBmpData::Init(CSize szBlock, int nBlock, DWORD clrBK)
{
	if( !CCacheData::Init(szBlock,nBlock,clrBK) )
		return FALSE;

	if( nBlock<=0 || szBlock.cx<=0 || szBlock.cy<=0 )
		return FALSE;

	m_ppBits	= new BYTE*[nBlock];
	m_pHBmps	= new HBITMAP[nBlock];
	m_nBlockSum	= nBlock;
	m_szBlock	= szBlock;

	memset(m_ppBits,0,sizeof(BYTE*)*nBlock);
	memset(m_pHBmps,0,sizeof(HBITMAP)*nBlock);

	m_pCacheBmpInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)];
	memset(m_pCacheBmpInfo,0,sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));

	SetbmiHeader(m_pCacheBmpInfo,szBlock.cx,szBlock.cy,m_nBitCount);

	if( m_nBitCount==8 && m_pRGBQuad!=NULL )
	{
		memcpy(m_pCacheBmpInfo->bmiColors,m_pRGBQuad,sizeof(RGBQUAD)*256);
	}

	if( !m_pCurDataEncoder )
	{
		for( int i=0;i<nBlock;i++ )
		{
			m_pHBmps[i] = ::CreateDIBSection( NULL, m_pCacheBmpInfo, DIB_RGB_COLORS,(void**)(m_ppBits+i), NULL,0L );
		}
	}
	else
	{
		m_nDecodedItemCount = 10;
		if( m_nDecodedItemCount>nBlock )m_nDecodedItemCount = nBlock;
		
		for( int i=0;i<m_nDecodedItemCount;i++ )
		{
			m_pHBmps[i] = ::CreateDIBSection( NULL, m_pCacheBmpInfo, DIB_RGB_COLORS,(void**)(m_ppBits+i), NULL,0L );
		}	
		
		m_pDecodedItems = new DecodedItem[m_nDecodedItemCount];
		m_pEncodedItems = new EncodedItem[nBlock];
		
		memset(m_pEncodedItems,0,sizeof(EncodedItem)*nBlock);
		
		m_arrDecodedUsedCount.SetSize(m_nDecodedItemCount);
		for( i=0;i<m_nDecodedItemCount;i++ )
		{
			m_arrDecodedUsedCount.SetAt(i,-1);
		}		
	}
	
	DIBSECTION section;
	::GetObject( m_pHBmps[0], sizeof(DIBSECTION), &section);
	memcpy(&m_pCacheBmpInfo->bmiHeader,&section.dsBmih,sizeof(BITMAPINFOHEADER));

	m_hMemDC = ::CreateCompatibleDC(NULL);
	m_hTemDC = ::CreateCompatibleDC(NULL);

	return TRUE;
}


BOOL CCacheBmpData::AskforMoreBlocks(int num)
{
	if( m_pHBmps==NULL || num<=0 )
		return FALSE;

	int nBlock = m_nBlockSum + num;

	BYTE **ppBits	= new BYTE*[nBlock];
	HBITMAP *pHBmps	= new HBITMAP[nBlock];
	
	memset(ppBits,0,sizeof(BYTE*)*nBlock);
	memset(pHBmps,0,sizeof(HBITMAP)*nBlock);

	memcpy(ppBits,m_ppBits,sizeof(BYTE*)*m_nBlockSum);
	memcpy(pHBmps,m_pHBmps,sizeof(HBITMAP)*m_nBlockSum);

	delete[] m_ppBits;
	delete[] m_pHBmps;

	m_ppBits = ppBits;
	m_pHBmps = pHBmps;
	
	if( !m_pCurDataEncoder )
	{
		for( int i=m_nBlockSum;i<nBlock;i++ )
		{
			m_pHBmps[i] = ::CreateDIBSection( NULL, m_pCacheBmpInfo, DIB_RGB_COLORS,(void**)(m_ppBits+i), NULL,0L );
		}
	}
	else
	{		
		EncodedItem* pEncodedItems = new EncodedItem[nBlock];		
		memset(pEncodedItems,0,sizeof(EncodedItem)*nBlock);

		memcpy(pEncodedItems,m_pEncodedItems,sizeof(EncodedItem)*m_nBlockSum);
		delete[] m_pEncodedItems;

		m_pEncodedItems = pEncodedItems;
	}

	m_nBlockSum = nBlock;

	return TRUE;
}


void CCacheBmpData::Destroy()
{
	if( m_pHBmps!=NULL )
	{
		for(int i=0;i<m_nBlockSum;i++ )
		{
			if( m_pHBmps[i] )
			{
				::DeleteObject( m_pHBmps[i] );
			}
		}
		delete[] m_pHBmps;
		m_pHBmps = NULL;
	}
	
	if( m_ppBits!=NULL )
	{
		delete[] m_ppBits;
		m_ppBits = NULL;
	}

	if( m_pCacheBmpInfo!=NULL ) 
	{
		delete[] (BYTE*)m_pCacheBmpInfo;
		m_pCacheBmpInfo = NULL;
	}

	if( m_pRGBQuad!=NULL )
	{
		delete[] m_pRGBQuad;
		m_pRGBQuad = NULL;
	}
		
	if( m_pEncodedItems )
	{
		for(int i=0; i<m_nBlockSum; i++)
		{
			if( m_pEncodedItems[i].pData )delete[] m_pEncodedItems[i].pData;			
		}
		
		delete[] m_pEncodedItems;
		m_pEncodedItems = NULL;
	}
	
	if( m_pDecodedItems )delete[] m_pDecodedItems;
	m_pDecodedItems = NULL;
	m_arrDecodedUsedCount.RemoveAll();
	m_nDecodedItemCount = 0;
	
	if( m_pDefDataEncoder )delete m_pDefDataEncoder;
	m_pDefDataEncoder = NULL;
	m_pCurDataEncoder = NULL;	

	if( m_hMemDC )::DeleteDC(m_hMemDC);
	if( m_hTemDC )::DeleteDC(m_hTemDC);

	m_hMemDC = NULL;
	m_hTemDC = NULL;
	m_hSaveBmp = NULL;
	
	CCacheData::Destroy();
}


int CCacheBmpData::GetType()
{
	return CHEDATA_TYPE_BMP;
}


void CCacheBmpData::ClearBlock(int blkIdx)
{
	if( !m_pCurDataEncoder )
		return;

	for(int i=0; i<m_nDecodedItemCount; i++)
	{
		if( m_pDecodedItems[i].blkidx==blkIdx )
		{
			for(int j=0; j<m_nDecodedItemCount; j++)
			{
				if( m_arrDecodedUsedCount[j]==i )
				{
					m_arrDecodedUsedCount.RemoveAt(j);
					m_arrDecodedUsedCount.InsertAt(m_nDecodedItemCount-1,-1);
					break;
				}
			}
			m_pDecodedItems[i].blkidx = -1;
			m_pDecodedItems[i].state = DecodedItem::state_none;

			break;
		}
	}
	
	if( m_pEncodedItems[blkIdx].pData )
		delete[] m_pEncodedItems[blkIdx].pData;
	
	m_pEncodedItems[blkIdx].pData = NULL;
	m_pEncodedItems[blkIdx].nDataLen = 0;
	m_pEncodedItems[blkIdx].bIsEncoded = FALSE;
}


int CCacheBmpData::GetBlockDecodedState(int idx)
{
	if( !m_pCurDataEncoder )
		return DecodedItem::state_decoded;

	for(int i=0; i<m_nDecodedItemCount; i++)
	{
		if( m_pDecodedItems[i].blkidx==idx )
		{
			return m_pDecodedItems[i].state;
		}
	}
	
	return DecodedItem::state_decoded;
}


void CCacheBmpData::SetBlockDecodedState(int idx, int state)
{
	if( !m_pCurDataEncoder )
		return;

	for(int i=0; i<m_nDecodedItemCount; i++)
	{
		if( m_pDecodedItems[i].blkidx==idx )
		{
			m_pDecodedItems[i].state = state;
		}
	}
}



BOOL CCacheBmpData::DecompressBitmap(int blkIdx)
{
	if( blkIdx<0 || blkIdx>=m_nBlockSum )return FALSE;	
	if( m_pCurDataEncoder )
	{
		int i, same=-1, space=-1, target=-1;
		for( i=0; i<m_nDecodedItemCount; i++)
		{
			if( m_pDecodedItems[i].blkidx==blkIdx )
			{
				same = i;
			}
			else if( m_pDecodedItems[i].blkidx==-1 )
			{
				space = i;
			}	
		}
		
		if( same!=-1 )
		{
			for( i=0; i<m_nDecodedItemCount; i++)
			{
				if( m_arrDecodedUsedCount[i]==same )break;
			}
			if( i>=m_nDecodedItemCount )
			{
				return FALSE;
			}
			
			if( i!=0 )
			{
				m_arrDecodedUsedCount.RemoveAt(i);
				m_arrDecodedUsedCount.InsertAt(0,same);
			}
			return TRUE;
		}
		
		if( space!=-1 )target = space;
		else
		{
			target = m_arrDecodedUsedCount[m_nDecodedItemCount-1];
			
			if( m_pDecodedItems[target].state==DecodedItem::state_modified )
			{
				CompressBitmap(m_pDecodedItems[target].blkidx);
			}
		}
		
		if( !m_pEncodedItems[blkIdx].bIsEncoded )
		{
			if( m_pEncodedItems[blkIdx].pData )		
				memcpy(m_ppBits[target],m_pEncodedItems[blkIdx].pData,m_pEncodedItems[blkIdx].nDataLen);
			else
			{
				DIBSECTION info;
				::GetObject(m_pHBmps[target],sizeof(info),&info);
				memset(m_ppBits[target],m_clrBack,info.dsBmih.biSizeImage);
			}
		}
		
		else if( m_pEncodedItems[blkIdx].bIsEncoded )
		{
			m_pDecodedItems[target].blkidx = -1;
			if( !m_pCurDataEncoder->Decode(m_pEncodedItems[blkIdx].pData,m_pEncodedItems[blkIdx].nDataLen,
				m_ppBits[target],NULL) )
			{
				return FALSE;
			}
		}
		
		m_pDecodedItems[target].blkidx = blkIdx;
		m_pDecodedItems[target].state = DecodedItem::state_decoded;
		m_arrDecodedUsedCount.RemoveAt(m_nDecodedItemCount-1);
		m_arrDecodedUsedCount.InsertAt(0,target);
	}
	
	return TRUE;
}


BOOL CCacheBmpData::CompressBitmap(int blkIdx)
{
	if( blkIdx<0 || blkIdx>=m_nBlockSum )return FALSE;
	if( m_pCurDataEncoder )
	{
		for( int i=0; i<m_nDecodedItemCount; i++)
		{
			if( m_pDecodedItems[i].blkidx==blkIdx )break;		
		}
		
		if( i>=m_nDecodedItemCount )return FALSE;
		
		if( m_pDecodedItems[i].state!=DecodedItem::state_modified )return TRUE;
		
		long retlen=0;
		BYTE *pBits=NULL;
		if( !m_pCurDataEncoder->Encode(m_ppBits[i],m_pCacheBmpInfo->bmiHeader.biSizeImage,
			(void**)&pBits,&retlen) )			
			return FALSE;	
		
		if( m_pEncodedItems[blkIdx].pData )delete[] m_pEncodedItems[blkIdx].pData;
		
		m_pEncodedItems[blkIdx].pData = 0;
		m_pEncodedItems[blkIdx].nDataLen  = 0;
		m_pEncodedItems[blkIdx].bIsEncoded = FALSE;
		
		if( retlen<m_pCacheBmpInfo->bmiHeader.biSizeImage )
		{
			m_pEncodedItems[blkIdx].pData = pBits;
			m_pEncodedItems[blkIdx].nDataLen  = retlen;
			m_pEncodedItems[blkIdx].bIsEncoded = TRUE;
		}
		else
		{
			delete[] pBits;
			pBits = new BYTE[m_pCacheBmpInfo->bmiHeader.biSizeImage];
			if( !pBits )return FALSE;
			memcpy( pBits,m_ppBits[i],m_pCacheBmpInfo->bmiHeader.biSizeImage);
			m_pEncodedItems[blkIdx].pData = pBits;
			m_pEncodedItems[blkIdx].nDataLen = m_pCacheBmpInfo->bmiHeader.biSizeImage;
			m_pEncodedItems[blkIdx].bIsEncoded = FALSE;
		}
		
		m_pDecodedItems[i].state = DecodedItem::state_decoded;
	}
	
	return TRUE;
}


void FixPreciError2(float& x)
{
	long n = (long)x;
	float e = n-x;
	
	if( e==0.0 )return;
	else if( e>=0.5 )
	{
		e -= 1.0;
		n -= 1;
	}
	else if( e<-0.5 )
	{
		e += 1.0;
		n += 1;
	}
	
	if( n!=0 && e<1e-2 && e>-1e-2 )
		x = n;
}


void InterpolateBmp(HBITMAP hSrc, HBITMAP hDest, CRect rcSrc, float kx, float ky)
{
	BITMAP dinfo,sinfo;
	if( ::GetObject( hDest, sizeof(BITMAP), &dinfo)==0 )return;
	if( ::GetObject( hSrc , sizeof(BITMAP), &sinfo)==0 )return;

	BYTE* pDest=(BYTE*)dinfo.bmBits;
	BYTE* pSrc =(BYTE*)sinfo.bmBits;
	
	int srcWidth = ((sinfo.bmWidth*(sinfo.bmBitsPixel>>3)+3)&~3);
	int destWidth= ((dinfo.bmWidth*3+3)&~3);
	int sw = sinfo.bmWidth, sh = sinfo.bmHeight;
	int dw = dinfo.bmWidth, dh = dinfo.bmHeight, ncol, nrow, i, j;

#if 0
	//非优化的方式（作为代码的逻辑参考）
	double s,t;
	BYTE *pd, *ps1, *ps2;
	unsigned long sw2 = ((rcSrc.Width()-1)), sh2 = ((rcSrc.Height()-1));

	for( i=0; i<dh; i++)
	{
		for( j=0; j<dw; j++)
		{
			pd = pDest+(dinfo.bmHeight-1-i)*destWidth+(j)*3;
			
			s = (j+0.5)/kx-0.5; t = (i+0.5)/ky-0.5; 
			if( s<0 )s = 0;
			if( t<0 )t = 0;
			if( s>(sw2) )s = sw2;
			if( t>(sh2) )s = sh2;

			ncol = (int)s; nrow = (int)t; 
			s = s-ncol, t = t-nrow;

			ncol += rcSrc.left; nrow += rcSrc.top; 
			
			ps1= pSrc+(sinfo.bmHeight-1-nrow)*srcWidth+(ncol)*3;
			ps2= ps1-srcWidth;
			
			for( int k=0; k<3; k++)
			{
				pd[k] = ps1[k]*(1-s)*(1-t) + ps1[k+3]*s*(1-t) +
					ps2[k]*(1-s)*t + ps2[k+3]*s*t;			
			}
		}
	}

#else

	BYTE *pd, *ps1, *ps2;

	//优化: 用整数代替浮点；用移位代替乘除	
	unsigned long s,t,st, sa=0x1000/kx, ta=0x1000/ky, s1, t1;
	unsigned long sw2 = ((rcSrc.Width()-1)<<12), sh2 = ((rcSrc.Height()-1)<<12);
	int nkx = (int)kx, nky = (int)ky;
	unsigned long rsa=0x1000%nkx, rta=0x1000%nky, rs = 0, rt = 0; //记录余数
	BOOL bUseRest = ( nkx>1 && nky>1 && (kx-nkx)==0 && (ky-nky)==0 && rsa!=0 && rta!=0 );

	if( sinfo.bmBitsPixel==24 )
	{
		t = (ta>>1)-0x800; 
		for( i=0; i<dh; i++)
		{
			pd = pDest+(dinfo.bmHeight-1-i)*destWidth;
			
			s = (sa>>1)-0x800; 
			for( j=0; j<dw; j++)
			{
				s1 = s; t1 = t;

				//左越界的处理
				if( ((long)s1)<0 )
				{
					//如果左边有可用像素，不妨继续越界
					if( rcSrc.left>0 )
					{
						ncol = -1;
						s1 += 0x1000;
					}
					//否则，归附到边界
					else
					{
						ncol = 0;
						s1 = 0;
					}
				}
				//右越界的处理
				else if( ((long)s1)>sw2 )
				{
					//如果右边有可用像素，不妨继续越界
					if( rcSrc.right<sinfo.bmWidth )
					{
						ncol = (s1>>12);
						s1 -= (ncol<<12);
					}
					//否则，归附到边界
					else
					{
						ncol = (sw2>>12);
						s1 = 0;
					}
				}
				//正常处理
				else
				{
					ncol = (s1>>12);
					s1 -= (ncol<<12);
				}

				//上越界的处理
				if( ((long)t1)<0 )
				{
					//如果上边有可用像素，不妨继续越界
					if( rcSrc.top>0 )
					{
						nrow = -1;
						t1 += 0x1000;
					}
					//否则，归附到边界
					else
					{
						nrow = 0;
						t1 = 0;
					}
				}
				//下越界的处理
				else if( ((long)t1)>sh2 )
				{
					//如果下边有可用像素，不妨继续越界
					if( rcSrc.bottom<sinfo.bmHeight )
					{
						nrow = (t1>>12);
						t1 -= (nrow<<12);
					}
					//否则，归附到边界
					else
					{
						nrow = (sh2>>12);
						t1 = 0;
					}
				}
				//正常处理
				else
				{
					nrow = (t1>>12);
					t1 -= (nrow<<12);
				}

				st= s1*t1;

				ncol += rcSrc.left; nrow += rcSrc.top; 
				
				ps1= pSrc+(sinfo.bmHeight-1-nrow)*srcWidth+(ncol)*3;
				ps2= ps1-srcWidth;

				//此时与ps2相关的系数值t1、st一定为0，所以ps2<pSrc是不影响结果的，
				//但是ps2<pSrc和ps1+k+3>pSrc会导致内存访问冲突，所以需要规避；
				if( ps1<pSrc )ps1 = pSrc;
				if( ps2<pSrc )ps2 = pSrc; 

				if( s1==0 && t1==0 )
				{
					pd[0] = ps1[0]; 
					pd[1] = ps1[1]; 
					pd[2] = ps1[2];
				}
				else if( s1==0 && t1!=0 )
				{
					for( int k=0; k<3; k++)
					{
						pd[k] = ((ps1[k]*(0x1000000-(t1<<12)) + ps2[k]*((t1<<12)))>>24);			
					}
				}
				else if( t1==0 && s1!=0 )
				{
					for( int k=0; k<3; k++)
					{
						pd[k] = ((ps1[k]*(0x1000000-(s1<<12)) + ps1[k+3]*((s1<<12)))>>24);			
					}
				}
				else
				{
					for( int k=0; k<3; k++)
					{
						pd[k] = ((ps1[k]*(0x1000000-(s1<<12)-(t1<<12)+st) + ps1[k+3]*((s1<<12)-st) +
							ps2[k]*((t1<<12)-st) + ps2[k+3]*st)>>24);			
					}
				}
				
				pd += 3;
				s += sa; 
				if( bUseRest )
				{
					rs += rsa;
					if( rs>=nkx )
					{
						s++;
						rs -= nkx;
					}
				}
			}
			t += ta; 
			if( bUseRest )
			{
				rt += rta;
				if( rt>=nky )
				{
					t++;
					rt -= nky;
				}
			}
		}
	}
	else if( sinfo.bmBitsPixel==8 )
	{	
		t = (ta>>1)-0x800; 
		for( i=0; i<dh; i++)
		{
			pd = pDest+(dinfo.bmHeight-1-i)*destWidth;
			
			s = (sa>>1)-0x800; 
			for( j=0; j<dw; j++)
			{
				s1 = s; t1 = t;
				if( ((long)s1)<0 )
				{
					if( rcSrc.left>0 )
					{
						ncol = -1;
						s1 += 0x1000;
					}
					else
					{
						ncol = 0;
						s1 = 0;
					}
				}
				else if( ((long)s1)>sw2 )
				{
					if( rcSrc.right<sinfo.bmWidth )
					{
						ncol = (s1>>12);
						s1 -= (ncol<<12);
					}
					else
					{
						ncol = (sw2>>12);
						s1 = 0;
					}
				}
				else
				{
					ncol = (s1>>12);
					s1 -= (ncol<<12);
				}
				
				if( ((long)t1)<0 )
				{
					if( rcSrc.top>0 )
					{
						nrow = -1;
						t1 += 0x1000;
					}
					else
					{
						nrow = 0;
						t1 = 0;
					}
				}
				else if( ((long)t1)>sh2 )
				{
					if( rcSrc.bottom<sinfo.bmHeight )
					{
						nrow = (t1>>12);
						t1 -= (nrow<<12);
					}
					else
					{
						nrow = (sh2>>12);
						t1 = 0;
					}
				}
				else
				{
					nrow = (t1>>12);
					t1 -= (nrow<<12);
				}
				
				st= s1*t1;
				
				ncol += rcSrc.left; nrow += rcSrc.top; 
				
				ps1= pSrc+(sinfo.bmHeight-1-nrow)*srcWidth+(ncol);
				ps2= ps1-srcWidth;

				//此时与ps2相关的系数值t1、st一定为0，所以ps2<pSrc是不影响结果的，
				//但是ps2<pSrc会导致内存访问冲突，所以需要规避；
				if( ps2<pSrc )ps2 = pSrc; 

				if( s1==0 && t1==0 )
				{
					pd[0] = ps1[0];
				}
				else if( s1==0 && t1!=0 )
				{
					pd[0] = ((ps1[0]*(0x1000000-(t1<<12)) + ps2[0]*((t1<<12)))>>24);
				}
				else if( t1==0 && s1!=0 )
				{
					pd[0] = ((ps1[0]*(0x1000000-(s1<<12)) + ps1[1]*((s1<<12)))>>24);
				}
				else
				{
					pd[0] = ((ps1[0]*(0x1000000-(s1<<12)-(t1<<12)+st) + ps1[1]*((s1<<12)-st) +
						ps2[0]*((t1<<12)-st) + ps2[1]*st)>>24);
				}				

				pd[1] = pd[0];
				pd[2] = pd[0];
				
				pd += 3;
				s += sa; 
				if( bUseRest )
				{
					rs += rsa;
					if( rs>=nkx )
					{
						s++;
						rs -= nkx;
					}
				}
			}
			t += ta; 
			if( bUseRest )
			{
				rt += rta;
				if( rt>=nky )
				{
					t++;
					rt -= nky;
				}
			}
		}
	}
#endif
}


void FilterBmp(HBITMAP hBmp, DWORD mask, BYTE zero)
{
	BITMAP sinfo;
	if( ::GetObject( hBmp, sizeof(BITMAP), &sinfo)==0 )return;
	
	if( sinfo.bmBitsPixel!=24 )return;

	BYTE *pSrc, *p;
	
	int srcWidth = ((sinfo.bmWidth*(sinfo.bmBitsPixel>>3)+3)&~3);
	int sw = sinfo.bmWidth, sh = sinfo.bmHeight;
	int i, j;

	int skip = srcWidth-sw*3;

	if( (mask&RGBMASK_RED)==0 )
	{
		pSrc =(BYTE*)sinfo.bmBits;
		p = pSrc + 2;

		for( i=0; i<sh; i++)
		{
			for( j=0; j<sw; j++, p+=3 )
			{
				*p = zero;
			}
			p += skip;
		}
	}

	if( (mask&RGBMASK_GREEN)==0 )
	{
		pSrc =(BYTE*)sinfo.bmBits;
		p = pSrc + 1;

		for( i=0; i<sh; i++)
		{
			for( j=0; j<sw; j++, p+=3 )
			{
				*p = zero;
			}
			p += skip;
		}
	}

	if( (mask&RGBMASK_BLUE)==0 )
	{
		pSrc =(BYTE*)sinfo.bmBits;
		p = pSrc;

		for( i=0; i<sh; i++)
		{
			p = pSrc;
			for( j=0; j<sw; j++, p+=3 )
			{
				*p = zero;
			}
			p += skip;
		}
	}
}


HBITMAP	CCacheBmpData::GetBlockBitmap(int blkIdx, BYTE **ppBits)
{
	if( blkIdx<0 || blkIdx>=m_nBlockSum )return NULL;
	
	if( m_pCurDataEncoder )
	{
		if( !DecompressBitmap(blkIdx) )
			return NULL;

		if( ppBits!=NULL )*ppBits = m_ppBits[m_arrDecodedUsedCount[0]];
		return m_pHBmps[m_arrDecodedUsedCount[0]];
	}
	
	if( ppBits!=NULL )*ppBits= m_ppBits[blkIdx];
	return m_pHBmps[blkIdx];
}


//填充cache块的准备函数
void CCacheBmpData::BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock)
{
	CCacheData::BeginFillBlock(p,blkIdx,bZeroBlock);

	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}

	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;

	HDC hdc = m_hMemDC;

	BYTE *pBits;
	HBITMAP hBmp = GetBlockBitmap(blkIdx,&pBits);
	if( hBmp!=NULL )
	{
		if( bZeroBlock )
		{
			if (m_clrBack == 0 || m_clrBack == 0xffffff)
				memset(pBits,m_clrBack,m_pCacheBmpInfo->bmiHeader.biSizeImage);	
			else
			{
				HBITMAP hOldTemBitmap = (HBITMAP)::SelectObject(m_hTemDC, hBmp);
				HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(m_clrBack);
				CRect rect(0,0,m_szBlock.cx,m_szBlock.cy);
				::FillRect(m_hTemDC,&rect,hbrush);
				DeleteObject(hbrush);
				(HBITMAP)::SelectObject(m_hTemDC, hOldTemBitmap);
			}
		}
		m_hSaveBmp = (HBITMAP)::SelectObject(hdc,hBmp);
	}
}



//填充cache块的结束函数
void CCacheBmpData::EndFillBlock()
{
	CDrawingContext *p = m_pFillContext;
	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}

	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;

	HDC hdc = m_hMemDC;
	HBITMAP hBmp = m_hSaveBmp;
	if( hBmp!=NULL )::SelectObject(hdc,hBmp);

	m_hSaveBmp = NULL;
	
	CCacheData::EndFillBlock();
}


//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
void CCacheBmpData::DrawGrBuffer(GrElementList *pList, int mode)
{
	CDrawingContext *p = m_pFillContext;
	if( !p )return;

	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}
	
	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;

	HDC hOldDC = ((C2DGDIDrawingContext*)p)->SetDrawingDC(m_hMemDC);

	p->DrawGrBuffer(pList,mode);

	((C2DGDIDrawingContext*)p)->SetDrawingDC(hOldDC);
}



//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
void CCacheBmpData::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	CDrawingContext *p = m_pFillContext;
	if( !p )return;
	
	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}
	
	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;

	HDC hOldDC = ((C2DGDIDrawingContext*)p)->SetDrawingDC(m_hMemDC);
	
	p->DrawGrBuffer2d(pList,mode);

	((C2DGDIDrawingContext*)p)->SetDrawingDC(hOldDC);
}


void CCacheBmpData::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CDrawingContext *p = m_pFillContext;
	if( !p )return;
	
	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}
	
	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;
	
	HDC hOldDC = ((C2DGDIDrawingContext*)p)->SetDrawingDC(m_hMemDC);
	
	p->DrawSelectionMark2d(pList,mode,clr,wid);
	
	((C2DGDIDrawingContext*)p)->SetDrawingDC(hOldDC);
}

void CCacheBmpData::DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	CDrawingContext *p = m_pFillContext;
	if( !p )return;
	
	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}
	
	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;
	
	HDC hOldDC = ((C2DGDIDrawingContext*)p)->SetDrawingDC(m_hMemDC);
	
	p->DrawGeometryMark(pList,mode,ptInfor);
	
	((C2DGDIDrawingContext*)p)->SetDrawingDC(hOldDC);
}

void CCacheBmpData::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	CDrawingContext *p = m_pFillContext;
	if( !p )return;
	
	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}
	
	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;
	
	HDC hOldDC = ((C2DGDIDrawingContext*)p)->SetDrawingDC(m_hMemDC);
	
	p->DrawSelectionMark(pList,mode,clr,wid);
	
	((C2DGDIDrawingContext*)p)->SetDrawingDC(hOldDC);
}


BYTE *CCacheBmpData::GetRotateBuf(int size)
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


void CCacheBmpData::SetRotateAndZoom(float ang, float kx, float ky)
{
	m_fRotAng = ang;
	m_fKX = kx;
	m_fKY = ky;
}

void ReverseBmp(HBITMAP hBmp)
{
	BITMAP info;
	if( !::GetObject(hBmp,sizeof(BITMAP),&info) )
		return;

	int linesize = info.bmWidthBytes;
	BYTE *p1 = (BYTE*)info.bmBits;
	BYTE *p2 = p1 + (info.bmHeight-1)*linesize;
	BYTE *pLine = new BYTE[linesize];
	memset(pLine,0,linesize);

	int num = (info.bmHeight>>1);
	for( int i=0; i<num; i++)
	{
		memcpy(pLine,p2,linesize);
		memcpy(p2,p1,linesize);
		memcpy(p1,pLine,linesize);

		p1 += linesize;
		p2 -= linesize;
	}

	delete[] pLine;
}


//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
void CCacheBmpData::DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover)
{
	CDrawingContext *p = m_pFillContext;
	if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p = ((CStereoDrawingContext*)p)->GetCurContext();
	}

	if( !p->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;

	DIBSECTION sect;
	if( ::GetObject(hBmp,sizeof(sect),&sect)==0 )return;
	
	if( m_fRotAng!=0 )
	{
		int nColor = (m_pCacheBmpInfo->bmiHeader.biBitCount>>3);
		int nBufSize = ((m_szBlock.cx*nColor + 3)&(~3))*m_szBlock.cy;
		BYTE *pRotateBuf = GetRotateBuf(nBufSize);

		::RotateBlock((BYTE*)sect.dsBm.bmBits,CRect(0,0,m_szBlock.cx,m_szBlock.cy),nColor,
			pRotateBuf,::FlagFromAngle(m_fRotAng));

		memcpy(sect.dsBm.bmBits, pRotateBuf, min(sect.dsBmih.biSizeImage,nBufSize));
	}

	ReverseBmp(hBmp);

	CRect rcDraw2 = (rcDraw&CRect(0,0,m_szBlock.cx,m_szBlock.cy));
	rcBmp.OffsetRect(rcDraw2.left-rcDraw.left,rcDraw2.top-rcDraw.top);
	rcBmp.right = rcBmp.left + rcDraw2.Width();
	rcBmp.bottom = rcBmp.top + rcDraw2.Height();

	//get color table
	RGBQUAD clrtbl[256], *pclrtbl = NULL;
	memset(clrtbl,0,sizeof(clrtbl));
	if( sect.dsBmih.biBitCount==8 )
	{
		CChgBmpOfDC chg1(m_hMemDC,hBmp);
		::GetDIBColorTable(m_hMemDC,0,256,clrtbl);

		pclrtbl = clrtbl;
	}
	
	// overlap
	if( bCover )
	{
		HBITMAP hDest = (HBITMAP)GetCurrentObject(m_hMemDC,OBJ_BITMAP);
		OverlapBitmap(hBmp,pclrtbl,rcBmp,hDest,NULL,rcDraw2,FALSE,0,p->GetRGBMask());
	}
	else
	{
		HBITMAP hDest = (HBITMAP)GetCurrentObject(m_hMemDC,OBJ_BITMAP);
		OverlapBitmap(hBmp,pclrtbl,rcBmp,hDest,NULL,rcDraw2,TRUE,p->GetBackColor(),p->GetRGBMask());
	}
}


HGLRC CreateMemRC(HDC hMemDC, int w, int h)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 
			1,                             
			PFD_SUPPORT_OPENGL|PFD_DRAW_TO_BITMAP,
			PFD_TYPE_RGBA,
			24, 0, 0, 0, 0, 0, 0,           
			0, 0, 0, 0, 0, 0, 0,
			16,                         
			0, 0,                      
			PFD_MAIN_PLANE,
			0,                         
			0, 0, 0                    
	};
	
	int pixelFormat = ChoosePixelFormat(hMemDC, &pfd);
	
	if( !SetPixelFormat(hMemDC, pixelFormat, &pfd) )
	{			
		return NULL;
	}
	
	HGLRC hglRC = wglCreateContext(hMemDC);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClearDepth(1.0f);
	
	glDrawBuffer(GL_FRONT);
	glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );

	return hglRC;
}

void WriteToFile2(HBITMAP hBmp, CRect rc)
{
	HBITMAP hBmp2 = Create24BitDIB(rc.Width(),rc.Height(),TRUE,0xffffff);
	OverlapBitmap(hBmp,NULL,rc,hBmp2,NULL,CRect(0,0,rc.Width(),rc.Height()),TRUE,0x00ffffff);

	WriteToFile(hBmp2);
	::DeleteObject(hBmp2);
	return;
}

#define PIXELX_GDI(x)	(int)((x)-0.5)

CMemDc gdc1, gdc2;

// x,y是客户窗口坐标
void CCacheBmpData::DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4])
{
	CDrawingContext *p1 = p;
	if( p1->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
	{
		p1 = ((CStereoDrawingContext*)p)->GetCurContext();
	}

	if( !p1->IsKindOf(RUNTIME_CLASS(C2DGDIDrawingContext)) )
		return;

	C2DGDIDrawingContext *p2 = (C2DGDIDrawingContext*)p1;

	PT_2D pts[4];
	pts[0].x = x[0]; pts[0].y = y[0];
	pts[1].x = x[1]; pts[1].y = y[1];
	pts[2].x = x[2]; pts[2].y = y[2];
	pts[3].x = x[3]; pts[3].y = y[3];

	Envelope e;
	e.CreateFromPts(pts,4);

	CRect rect;
	rect.left = PIXEL(e.m_xl); rect.right = PIXEL(e.m_xh);
	rect.top = PIXEL(e.m_yl); rect.bottom = PIXEL(e.m_yh);

	float kx = rect.Width()/(float)m_szBlock.cx, ky = rect.Height()/(float)m_szBlock.cy;
	
	FixPreciError2(kx); FixPreciError2(ky);
	rect.right = rect.left + (int)(m_szBlock.cx*kx+0.1);
	rect.bottom = rect.top + (int)(m_szBlock.cy*ky+0.1);

	CRect rcView;
	p2->GetViewRect(rcView);

	CRect rc1 = (rect&rcView);
	if( rc1.IsRectEmpty() )
		return;

	int nDestWid = rect.Width()+2, nDestHei = rect.Height()+2;

	BYTE *pBits = NULL;
	HBITMAP hSrc = GetBlockBitmap(blkIdx, &pBits);

	if( kx>1.0f && ky>1.0f )
	{
		if( m_bSmoothZoom  )
			goto smooth_stretch_and_transparent_overlay;
		else
			goto stretch_and_transparent_overlay;
	}
	else if( kx==1.0f && ky==1.0f )
	{
		goto transparent_overlay;
	}
	else
	{
		goto stretch_and_transparent_overlay;
	}

	if(0)
	{
stretch_and_transparent_overlay:
	{
		rc1.OffsetRect(CPoint(-rect.left,-rect.top));
		CRect rc2;
		rc2.left = floor(rc1.left/kx); rc2.right = ceil(rc1.right/kx);
		rc2.top = floor(rc1.top/ky); rc2.bottom = ceil(rc1.bottom/ky);
		
		CRect rc3;
		rc3.left = 0; rc3.right = ceil(rc2.Width()*kx);
		rc3.top = 0; rc3.bottom = ceil(rc2.Height()*ky);
		
		HBITMAP hDest = Create24BitDIB(rc3.Width(),rc3.Height(),TRUE,m_clrBack);
		
		//Stretch
		{
			CChgBmpOfDC chg1(m_hMemDC,hSrc);
			::SetStretchBltMode(p2->GetMemBoardDC(),STRETCH_DELETESCANS);
			
			CChgBmpOfDC chg2(p2->GetMemBoardDC(),hDest);
			
			::StretchBlt(p2->GetMemBoardDC(), rc3.left,rc3.top,rc3.Width(),rc3.Height(),
				m_hMemDC, rc2.left,rc2.top,rc2.Width(),rc2.Height(),SRCCOPY);
		}
		
		// overlap
		rc3.left = rc2.left*kx; rc3.right = rc2.right*kx; 
		rc3.top = rc2.top*ky; rc3.bottom = rc2.bottom*ky; 
		rc3.OffsetRect(CPoint(rect.left,rect.top));
		rc1.OffsetRect(CPoint(rect.left,rect.top));
		rc3.SetRect(rc1.left-rc3.left,rc1.top-rc3.top,rc1.right-rc3.left,rc1.bottom-rc3.top);
		
		HBITMAP hDest2 = p2->GetMemBoardBmp();
		OverlapBitmap(hDest,NULL,rc3,hDest2,NULL,rc1,m_bTransparentOverlap,m_clrBack);
		
		::DeleteObject(hDest);
	}
	}

	if(0)
	{
smooth_stretch_and_transparent_overlay:
	{
		rc1.OffsetRect(CPoint(-rect.left,-rect.top));
		CRect rc2;
		rc2.left = floor(rc1.left/kx); rc2.right = ceil(rc1.right/kx);
		rc2.top = floor(rc1.top/ky); rc2.bottom = ceil(rc1.bottom/ky);
		
		CRect rc3;
		rc3.left = 0; rc3.right = ceil(rc2.Width()*kx);
		rc3.top = 0; rc3.bottom = ceil(rc2.Height()*ky);
		
		HBITMAP hDest = Create24BitDIB(rc3.Width(),rc3.Height(),TRUE,m_clrBack);
		
		InterpolateBmp(hSrc,hDest,rc2,kx,ky);
		
		rc3.left = rc2.left*kx; rc3.right = rc2.right*kx; 
		rc3.top = rc2.top*ky; rc3.bottom = rc2.bottom*ky; 
		rc3.OffsetRect(CPoint(rect.left,rect.top));
		rc1.OffsetRect(CPoint(rect.left,rect.top));
		rc3.SetRect(rc1.left-rc3.left,rc1.top-rc3.top,rc1.right-rc3.left,rc1.bottom-rc3.top);
		
		HBITMAP hDest2 = p2->GetMemBoardBmp();
		//OverlapBitmap(hDest,NULL,rc3,hDest2,NULL,rc1,m_bTransparentOverlap,m_clrBack);
		
		HDC hdc1 = gdc1.GetHdc();
		//HDC hdc2 = gdc2.GetHdc();
		HBITMAP hOld1 = (HBITMAP)SelectObject(hdc1,hDest);
		//HBITMAP hOld2 = (HBITMAP)SelectObject(hdc2,hDest2);
		
		TransparentBlt(p2->GetMemBoardDC(),rc1.left,rc1.top,rc1.Width(),rc1.Height(),
			hdc1,rc3.left,rc3.top,rc1.Width(),rc1.Height(),m_clrBack);
		
		SelectObject(hdc1,hOld1);
		//SelectObject(hdc2,hOld2);
		
		::DeleteObject(hDest);
	}
	}

	if(0)
	{
transparent_overlay:
	{
		CRect rc2 = rc1;
		rc2.OffsetRect(CPoint(-rect.left,-rect.top));
		
		HBITMAP hDest2 = p2->GetMemBoardBmp();

		//OverlapBitmap(hSrc,m_pRGBQuad,rc2,hDest2,NULL,rc1,m_bTransparentOverlap,m_clrBack);

		HDC hdc1 = gdc1.GetHdc();
		//HDC hdc2 = gdc2.GetHdc();
		HBITMAP hOld1 = (HBITMAP)SelectObject(hdc1,hSrc);
		//HBITMAP hOld2 = (HBITMAP)SelectObject(hdc2,hDest2);

		TransparentBlt(p2->GetMemBoardDC(),rc1.left,rc1.top,rc1.Width(),rc1.Height(),
			hdc1,rc2.left,rc2.top,rc1.Width(),rc1.Height(),m_clrBack);

		SelectObject(hdc1,hOld1);
		//SelectObject(hdc2,hOld2);
	}
	}


	//既没有启动内存位图板，也没有设置为“仅内存操作”的模式，就直接贴图
	if( p2->GetMemBoardDC()!=p2->GetContext() && !p2->IsMemOpOnly() )
	{
		HBITMAP hOld = (HBITMAP)::SelectObject(p2->GetMemBoardDC(),p2->GetMemBoardBmp());
		::BitBlt(p2->GetContext(), rc1.left,rc1.top,rc1.Width(),rc1.Height(),
			p2->GetMemBoardDC(), rc1.left,rc1.top,p2->m_nCopyMode);
		::SelectObject(p2->GetMemBoardDC(),hOld);
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCacheGLListData::CCacheGLListData()
{
}


CCacheGLListData::~CCacheGLListData()
{
	Destroy();
}


int CCacheGLListData::GetType()
{
	return CHEDATA_TYPE_GLLIST;
}

void CCacheGLListData::Destroy()
{
	for( int i=0; i<m_arrListIDs.GetSize(); i++)
	{
		glDeleteLists(m_arrListIDs[i],1);
	}
}

void CCacheGLListData::ClearBlock(int idx)
{
}


BOOL CCacheGLListData::Init(CSize szBlock, int nBlock, DWORD clrBK)
{
	int nStart = glGenLists(nBlock);
	for( int i=0; i<nBlock; i++)
	{
		m_arrListIDs.Add(nStart+i);
	}

	return CCacheData::Init(szBlock,nBlock,clrBK);
}


BOOL CCacheGLListData::AskforMoreBlocks(int num)
{
	int nStart = glGenLists(num);
	for( int i=0; i<num; i++)
	{
		m_arrListIDs.Add(nStart+i);
	}

	return TRUE;
}

//填充cache块的准备函数
void CCacheGLListData::BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock)
{
	CCacheData::BeginFillBlock(p,blkIdx);

	m_pFillContext->BeginDrawing();
	
	glNewList(m_arrListIDs[blkIdx],GL_COMPILE);
}



//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
void CCacheGLListData::DrawGrBuffer(GrElementList *pList, int mode)
{
	if( m_pFillContext )
	{
		m_pFillContext->DrawGrBuffer(pList,mode);
	}
}



//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
void CCacheGLListData::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	if( m_pFillContext )
	{
		m_pFillContext->DrawGrBuffer2d(pList,mode);
	}
}


void CCacheGLListData::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	if( m_pFillContext )
	{
		m_pFillContext->DrawSelectionMark2d(pList,mode,clr,wid);
	}
}

void CCacheGLListData::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	if( m_pFillContext )
	{
		m_pFillContext->DrawSelectionMark(pList,mode,clr,wid);
	}
}

//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
void CCacheGLListData::DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover)
{
}



//填充cache块的结束函数
void CCacheGLListData::EndFillBlock()
{
	glEndList();
	m_pFillContext->EndDrawing();

	CCacheData::EndFillBlock();
}


// x,y是客户窗口坐标
void CCacheGLListData::DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4])
{
//	p->SetViewRect(CRect(x[0],y[1],x[1],y[2]));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(PIXEL(x[0]),p->GetDCSize().cy-PIXEL(y[2]),0);
	
	glCallList(m_arrListIDs[blkIdx]);
	
	glPopMatrix();
/*
	CGlDrawLineDC dc;
	dc.Begin(5);
	dc.SetColor(RGB(255,0,0));

	dc.MoveTo(0,0);
	dc.LineTo(1000,1000);

	dc.End();
	*/
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CCacheGLTextData::m_bUseGPU = FALSE;

CCacheGLTextData::CCacheGLTextData()
{
	m_pTextures = NULL;
	m_nTextures = 0;
	m_hMemDC = NULL;
	m_hBmp = NULL;
	m_hBmpOld = NULL;
	m_bUseAlpha = FALSE;
	m_bUseLinear = TRUE;
	m_bDirectRenderVectors = FALSE;
}

CCacheGLTextData::~CCacheGLTextData()
{
	GPU_Destroy();
	Destroy();
}

BOOL CCacheGLTextData::Init(CSize szBlock, int nBlock, DWORD clrBK)
{
	Destroy();

	if( !CCacheData::Init(szBlock,nBlock,clrBK) )
		return FALSE;

	int num = nBlock;
	int texsize = szBlock.cx;
	
	m_pTextures = new tex_image[num];
	if( !m_pTextures )
		return FALSE;

	BYTE *pBits = new BYTE[texsize*texsize*4+1024];
	memset(pBits,0,texsize*texsize*4+1024);

	m_nTextures = nBlock;
	
	for( int i=0; i<m_nTextures; i++)
	{
		m_pTextures[i].gen();
		m_pTextures[i].bind();
		if( m_bUseAlpha )
			m_pTextures[i].define(texsize,texsize,GL_BGRA_EXT,pBits);
		else
			m_pTextures[i].define(texsize,texsize,GL_BGR_EXT,pBits);
	}

	m_pTextures[0].enable();
	
	delete[] pBits;
	
	GLuint *texts = new GLuint[m_nTextures];
	GLclampf *priorities = new GLclampf[m_nTextures];
	if( texts && priorities )
	{
		for( i=0; i<m_nTextures; i++)
		{
			texts[i] = m_pTextures[i].texture;
			priorities[i] = 100;
		}
		
		glPrioritizeTextures(m_nTextures,texts,priorities);
	}
	
	if( texts )delete[] texts;
	if( priorities )delete[] priorities;
	if( m_bUseGPU && GPU_CheckSupported() )
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		GPU_InitGL();
		glDisable(GL_DEPTH_TEST);
	}
	
	return TRUE;
}


BOOL CCacheGLTextData::AskforMoreBlocks(int num)
{
	int nBlock = m_nBlockSum + num;
	int texsize = m_szBlock.cx;
	
	tex_image *pTextures = new tex_image[nBlock];
	if( !pTextures )
		return FALSE;

	for( int i=0; i<m_nBlockSum; i++)
	{
		m_pTextures[i].cutto(pTextures[i]);
	}
	delete[] m_pTextures;

	m_pTextures = pTextures;
	
	BYTE *pBits = new BYTE[texsize*texsize*4+1024];
	memset(pBits,0,texsize*texsize*4+1024);
	
	m_nTextures = nBlock;
	
	for( i=0; i<num; i++)
	{
		m_pTextures[m_nBlockSum+i].gen();
		m_pTextures[m_nBlockSum+i].bind();
		if( m_bUseAlpha )
			m_pTextures[m_nBlockSum+i].define(texsize,texsize,GL_BGRA_EXT,pBits);
		else
			m_pTextures[m_nBlockSum+i].define(texsize,texsize,GL_BGR_EXT,pBits);
	}

	delete[] pBits;
	
	m_pTextures[0].enable();
	
	GLuint *texts = new GLuint[m_nTextures];
	GLclampf *priorities = new GLclampf[m_nTextures];
	if( texts && priorities )
	{
		for( i=0; i<m_nTextures; i++)
		{
			texts[i] = m_pTextures[i].texture;
			priorities[i] = 100;
		}
		
		glPrioritizeTextures(m_nTextures,texts,priorities);
	}
	
	if( texts )delete[] texts;
	if( priorities )delete[] priorities;
	
	return TRUE;
}

void CCacheGLTextData::Destroy()
{
	if( m_hMemDC!=NULL && m_hBmpOld!=NULL )
	{
		::SelectObject(m_hMemDC,m_hBmpOld);
		::DeleteDC(m_hMemDC);
	}
	if( m_hBmp!=NULL )
		::DeleteObject(m_hBmp);
	if( m_pTextures!=NULL )
		delete[] m_pTextures;
	
	m_pTextures = NULL;
	m_nTextures = 0;
}
HBITMAP CCacheGLTextData::Create32MemBmp(int w, int h)
{
	COLORREF bakCol = m_pFillContext->GetBackColor();

	if( m_hBmp!=NULL )
	{
		BITMAP sinfo;
		if( ::GetObject( m_hBmp, sizeof(BITMAP), &sinfo)==0 )return NULL;

		if (bakCol == 0 || bakCol == 0xffffff)
			memset(sinfo.bmBits, bakCol, sinfo.bmWidthBytes*sinfo.bmHeight);
		else
		{
			HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(bakCol);
			CRect rect(0, 0, w, h);
			::FillRect(m_hMemDC, &rect, hbrush);
			DeleteObject(hbrush);
		}

		return m_hBmp;
	}
	m_hMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFO bitmapInfo;
	memset(&bitmapInfo,0,sizeof(bitmapInfo));
	SetbmiHeader(&bitmapInfo,w,h,32);
	
	BYTE *pBits;
	m_hBmp = CreateDIBSection( NULL,&bitmapInfo,DIB_RGB_COLORS,
		(void**)&pBits,0,0L );
	memset(pBits,0,w*h*4);
	if( m_hMemDC!=NULL && m_hBmp!=NULL )
		m_hBmpOld = (HBITMAP)::SelectObject(m_hMemDC,m_hBmp);

	if (bakCol == 0 || bakCol == 0xffffff)
		memset(pBits, bakCol, bitmapInfo.bmiHeader.biSizeImage);
	else
	{
		HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(bakCol);
		CRect rect(0, 0, w, h);
		::FillRect(m_hMemDC, &rect, hbrush);
		DeleteObject(hbrush);
	}

	return m_hBmp;
}


int CCacheGLTextData::GetType()
{
	return CHEDATA_TYPE_GLTEXT;
}


void CCacheGLTextData::ClearBlock(int idx)
{
}


//填充cache块的准备函数
void CCacheGLTextData::BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock)
{
	CCacheData::BeginFillBlock(p,blkIdx);
	m_pFillContext->BeginDrawing();

	Create32MemBmp(m_szBlock.cx,m_szBlock.cy);

	SetAlphaValues(m_hBmp);

	if( !bZeroBlock )
	{
		m_pTextures[m_nFillBlkIdx].copyToBmp(m_hBmp);
	}
}





void CCacheGLTextData::SetAlphaValues(HBITMAP hBmp)
{
	BITMAP sinfo;
	if( ::GetObject( hBmp, sizeof(BITMAP), &sinfo)==0 )return;
	if( sinfo.bmBitsPixel!=32 )return;

	DWORD clrBack = m_pFillContext->GetBackColor();

	DWORD *p, *pMax;
	int srcWidth = ((sinfo.bmWidth*(sinfo.bmBitsPixel>>3)+3)&~3);

	p = (DWORD*)sinfo.bmBits;
	pMax = (DWORD*)(((BYTE*)sinfo.bmBits)+srcWidth*sinfo.bmHeight);

	while (p<pMax)
	{
		if (((*p) & 0xffffff) != clrBack)
		{
			if (((*p) & 0xff000000) == 0)
				(*p) = ((*p) | 0xff000000);
		}
		else
		{
			(*p) = ((*p) & 0xffffff);
		}

		p++;
	}

	/*
	_asm
	{
			push eax
			push ebx
			push ecx
			push edx
			mov  eax, dword ptr [pMax]
			mov	 ecx, dword ptr [p]
			mov	 edx, dword ptr[clrBack]
		SetAlphaValues_ASM_LOOP_START:
			cmp  ecx, eax					; // while( p<pMax )
			jge  SetAlphaValues_ASM_LOOP_OVER
			mov  ebx, dword ptr [ecx]
			and  ebx, 0xffffff
			cmp	 ebx, edx					; // if( ((*p)&0xffffff)!=clrBack )
			je   SetAlphaValues_ASM_CLEAR
			or   ebx, 0xff000000			; // (*p) = ((*p)|0xff000000);
			mov  dword ptr [ecx], ebx
			jmp  SetAlphaValues_ASM_LOOP_CONTINUE
SetAlphaValues_ASM_CLEAR:
			and   ebx, 0xffffff			; // (*p) = ((*p)&0xffffff);
			mov  dword ptr [ecx], ebx
SetAlphaValues_ASM_LOOP_CONTINUE:
			add  ecx, 4						; // p++;
			jmp  SetAlphaValues_ASM_LOOP_START
SetAlphaValues_ASM_LOOP_OVER:
			pop  edx
			pop  ecx
			pop  ebx
			pop  eax
	}*/
}




void SetAlphaValues_Rect(HBITMAP hBmp, CRect rect)
{
	BITMAP sinfo;
	if (::GetObject(hBmp, sizeof(BITMAP), &sinfo) == 0)return;
	if (sinfo.bmBitsPixel != 32)return;

	rect = (rect&CRect(0,0,sinfo.bmWidth, sinfo.bmHeight));

	BYTE *p0, *p;
	int srcWidth = ((sinfo.bmWidth*(sinfo.bmBitsPixel >> 3) + 3)&~3);

	p0 = (BYTE*)sinfo.bmBits + (sinfo.bmHeight - 1)*srcWidth;
	
	for (int i = 0; i < sinfo.bmHeight; i++)
	{
		p = p0;
		if (i < rect.top || i >= rect.bottom)
		{
			for (int j = 0; j < sinfo.bmWidth; j++)
			{
				p[3] = 0;
				p += 4;
			}
		}
		else
		{
			for (int j = 0; j < rect.left; j++)
			{
				p[3] = 0;
				p += 4;
			}
			for (; j < rect.right; j++)
			{
				p[3] = 255;
				p += 4;
			}
			for (; j < sinfo.bmWidth; j++)
			{
				p[3] = 0;
				p += 4;
			}
		}
		p0 -= srcWidth;
	}
}



//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
void CCacheGLTextData::DrawGrBuffer(GrElementList *pList, int mode)
{
	HBITMAP hBmp = m_hBmp;
	C2DGDIDrawingContext dc;
	dc.CreateContext(m_hMemDC);
	dc.SetCoordSys(m_pFillContext->GetCoordSys());
	dc.SetDCSize(m_pFillContext->GetDCSize());
	dc.CopyDisplaySetting(m_pFillContext);
	CRect rcView;
	m_pFillContext->GetViewRect(rcView);
	dc.SetViewRect(rcView);
	dc.DrawGrBuffer(pList,mode);
}



//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
void CCacheGLTextData::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	HBITMAP hBmp = m_hBmp;
	C2DGDIDrawingContext dc;
	dc.CreateContext(m_hMemDC);
	dc.SetCoordSys(m_pFillContext->GetCoordSys());
	dc.SetDCSize(m_pFillContext->GetDCSize());
	dc.CopyDisplaySetting(m_pFillContext);
	CRect rcView;
	m_pFillContext->GetViewRect(rcView);
	dc.SetViewRect(rcView);	
	dc.DrawGrBuffer2d(pList,mode);
}

void CCacheGLTextData::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	HBITMAP hBmp = m_hBmp;
	C2DGDIDrawingContext dc;
	dc.CreateContext(m_hMemDC);
	dc.SetCoordSys(m_pFillContext->GetCoordSys());
	dc.SetDCSize(m_pFillContext->GetDCSize());
	dc.CopyDisplaySetting(m_pFillContext);
	CRect rcView;
	m_pFillContext->GetViewRect(rcView);
	dc.SetViewRect(rcView);
	dc.DrawSelectionMark2d(pList,mode,clr,wid);
}

void CCacheGLTextData::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	HBITMAP hBmp = m_hBmp;
	C2DGDIDrawingContext dc;
	dc.CreateContext(m_hMemDC);
	dc.SetCoordSys(m_pFillContext->GetCoordSys());
	dc.SetDCSize(m_pFillContext->GetDCSize());
	dc.CopyDisplaySetting(m_pFillContext);
	CRect rcView;
	m_pFillContext->GetViewRect(rcView);
	dc.SetViewRect(rcView);
	dc.DrawSelectionMark(pList,mode,clr,wid);
}



void CCacheGLTextData::DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	HBITMAP hBmp = m_hBmp;
	C2DGDIDrawingContext dc;
	dc.CreateContext(m_hMemDC);
	dc.SetCoordSys(m_pFillContext->GetCoordSys());
	dc.SetDCSize(m_pFillContext->GetDCSize());
	dc.CopyDisplaySetting(m_pFillContext);
	CRect rcView;
	m_pFillContext->GetViewRect(rcView);
	dc.SetViewRect(rcView);
	dc.DrawGeometryMark(pList,mode,ptInfor);
}

//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
void CCacheGLTextData::DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover)
{
	if( bCover )
	{
		if( m_nFillBlkIdx>=0 && m_nFillBlkIdx<m_nBlockSum )
		{
			CDrawingContext *p = m_pFillContext;
			if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
			{
				p = ((CStereoDrawingContext*)p)->GetCurContext();
			}
			
			FilterBmp(hBmp,p->GetRGBMask(),(p->GetBackColor()&0xff));

			OverlapBitmap(hBmp, NULL, rcBmp, m_hBmp, NULL, rcDraw, FALSE,m_clrBack);

// 			m_pTextures[m_nFillBlkIdx].bmptotexture(hBmp,rcBmp.left,rcBmp.top,
// 				rcDraw.left,rcDraw.top,rcDraw.Width(),rcDraw.Height(),GL_BGR_EXT);
// 			m_pTextures[m_nFillBlkIdx].filled = true;
		}
	}
	else
	{
		CDrawingContext *p = m_pFillContext;
		if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
		{
			p = ((CStereoDrawingContext*)p)->GetCurContext();
		}

		OverlapBitmap(hBmp,NULL,rcBmp,m_hBmp,NULL,CRect(0,0,m_szBlock.cx,m_szBlock.cy),
			TRUE,p->GetBackColor(),p->GetRGBMask());

// 		m_pTextures[m_nFillBlkIdx].bmptotexture(hBmp2,0,0,
// 			0,0,m_szBlock.cx,m_szBlock.cy,GL_BGR_EXT);
// 		m_pTextures[m_nFillBlkIdx].filled = true;
	}
}



//填充cache块的结束函数
void CCacheGLTextData::EndFillBlock()
{
	if( m_nFillBlkIdx>=0 && m_nFillBlkIdx<m_nBlockSum )
	{
		if (m_rcFillBlk.IsRectEmpty())
			SetAlphaValues(m_hBmp);
		else
			SetAlphaValues_Rect(m_hBmp, m_rcFillBlk);
		m_pTextures[m_nFillBlkIdx].setviewbmp_BGRA(m_hBmp);
	}

	m_pFillContext->EndDrawing();
	CCacheData::EndFillBlock();
}


void CCacheGLTextData::BeginDisplayBlock()
{
	m_pTextures[0].enable();
	if( m_bUseAlpha )
	{
		//glEnable(GL_ALPHA_TEST);     
		//glAlphaFunc(GL_GREATER,0.1);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_COLOR_LOGIC_OP);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}
void CCacheGLTextData::EndDisplayBlock()
{
	m_pTextures[0].disable();
	if( m_bUseAlpha )
	{
		glDisable(GL_BLEND);
		//glDisable(GL_ALPHA_TEST);
	}
}
// x,y是客户窗口坐标
void CCacheGLTextData::DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4])
{
	double y2[4];
	CSize szDC = p->GetDCSize();

	//转换成 Opengl 坐标
	y2[0] = szDC.cy-y[0];
	y2[1] = szDC.cy-y[1];
	y2[2] = szDC.cy-y[2];
	y2[3] = szDC.cy-y[3];

	if( m_bUseAlpha )
	{
		m_pTextures[blkIdx].display_alpha(x,y2);
	}
	else
	{
		m_pTextures[blkIdx].display(x,y2,m_bUseLinear,(m_bUseGPU && GPU_CheckSupported()));
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCacheD3DTextData::CCacheD3DTextData()
{
	m_pD3D = NULL;
	m_hMemDC = NULL;
	m_hTmpDC = NULL;
	m_hBmp = NULL;
	m_hBmpOld = NULL;
	m_bUseAlpha = FALSE;
	m_nRenderTexture = -1;
	m_bUseLinearZoom = TRUE;
	m_nSymTextureForSetBitmap = -1;
}

CCacheD3DTextData::~CCacheD3DTextData()
{
	Destroy();
}


void CCacheD3DTextData::SetD3D(CD3DWrapper *p, BOOL bRenderVectors)
{
	m_pD3D = p;
	m_bDirectRenderVectors = bRenderVectors;
}

BOOL CCacheD3DTextData::Init(CSize szBlock, int nBlock, DWORD clrBK)
{
	Destroy();

	if( !m_pD3D )return FALSE;

	int num = nBlock;
		
	for( int i=0; i<nBlock; i++)
	{
		int index = m_pD3D->AllocTexture(szBlock.cx,szBlock.cy,CD3DTexture::typeRender);
		if( index<0 )
			break;
		m_arrCacheTextureIndex.Add(index);
	}
	if(i<=0)
		return FALSE;

	if( !CCacheData::Init(szBlock,i,clrBK) )
		return FALSE;
	
	return TRUE;
}


BOOL CCacheD3DTextData::AskforMoreBlocks(int num)
{
	if( !m_pD3D )return FALSE;

	int nBlock = m_nBlockSum + num;

	for( int i=m_nBlockSum; i<nBlock; i++)
	{
		int index = m_pD3D->AllocTexture(m_szBlock.cx,m_szBlock.cy,CD3DTexture::typeRender);
		if( index<0 )
			break;
		m_arrCacheTextureIndex.Add(index);
	}

	m_nBlockSum = i;
	
	return TRUE;
}

void CCacheD3DTextData::Destroy()
{
	if( m_hMemDC!=NULL && m_hBmpOld!=NULL )
	{
		::SelectObject(m_hMemDC,m_hBmpOld);
		::DeleteDC(m_hMemDC);
	}
	if( m_hBmp!=NULL )
		::DeleteObject(m_hBmp);

	if (m_hTmpDC)
	{
		::DeleteDC(m_hTmpDC);
	}
	m_pD3D->ReleaseAllocTexture(m_nRenderTexture);
	m_pD3D->ReleaseAllocTexture(m_nSymTextureForSetBitmap);

	m_nRenderTexture = -1;
	m_nSymTextureForSetBitmap = -1;

	for(int i=0; i<m_arrCacheTextureIndex.GetSize(); i++)
	{
		m_pD3D->ReleaseAllocTexture(m_arrCacheTextureIndex[i]);
	}

	m_arrCacheTextureIndex.RemoveAll();
}


int CCacheD3DTextData::GetType()
{
	return CHEDATA_TYPE_D3DTEXT;
}


void CCacheD3DTextData::ClearBlock(int idx)
{
}

CD3DTexture *CCacheD3DTextData::GetRenderTexture()
{
	if( m_nRenderTexture<0 )
	{
		m_nRenderTexture = m_pD3D->AllocTexture(m_szBlock.cx,m_szBlock.cy,CD3DTexture::typeRender);
		if( m_nRenderTexture<0 )
			return NULL;
	}

	return m_pD3D->GetAllocTexture(m_nRenderTexture);
}

CD3DTexture *CCacheD3DTextData::GetSystemTextureForSetBitmap()
{
	if( m_nSymTextureForSetBitmap<0 )
	{
		m_nSymTextureForSetBitmap = m_pD3D->AllocTexture(m_szBlock.cx,m_szBlock.cy,CD3DTexture::typeDynamicSysMem);
		if( m_nSymTextureForSetBitmap<0 )
			return NULL;
	}
	
	return m_pD3D->GetAllocTexture(m_nSymTextureForSetBitmap);
}

CD3DTexture& CCacheD3DTextData::GetCacheTexture(int index)
{
	static CD3DTexture dummy;

	if(index<0 || index>=m_arrCacheTextureIndex.GetSize())
		return dummy;

	CD3DTexture *p = m_pD3D->GetAllocTexture(m_arrCacheTextureIndex[index]);
	if(p==NULL)
		return dummy;
	
	return *p;
}


//填充cache块的准备函数
void CCacheD3DTextData::BeginFillBlock(CDrawingContext *p, int blkIdx, BOOL bZeroBlock)
{
	CCacheData::BeginFillBlock(p,blkIdx);

	if( m_bDirectRenderVectors )
	{		
		if( m_pD3D->m_pD3DDevice9->TestCooperativeLevel()!=D3D_OK )
			return;

		if(GetCacheTexture(m_nFillBlkIdx).m_nType==CD3DTexture::typeRender)
		{
			m_pD3D->SetRenderTexture(GetCacheTexture(m_nFillBlkIdx).m_pTexture,m_szBlock.cx,m_szBlock.cy);
			m_pD3D->BeginRenderTexture();

			m_pD3D->BeginDrawing();

			if( bZeroBlock )
			{
				m_pD3D->ClearBuffer(m_pFillContext->GetBackColor(),0);		
			}
		}
		else
		{
			CD3DTexture *pRT = GetRenderTexture();
			
			if( !pRT )
				return;

			m_pD3D->SetRenderTexture(pRT->m_pTexture,m_szBlock.cx,m_szBlock.cy);
			m_pD3D->BeginRenderTexture();

			m_pD3D->BeginDrawing();

			m_pD3D->ClearBuffer(m_pFillContext->GetBackColor(),0);

			if( !bZeroBlock )
			{
				double x[] = {0,m_szBlock.cx,m_szBlock.cx,0};
				double y[] = {m_szBlock.cy,m_szBlock.cy,0,0};

				BeginDisplayBlock();
				GetCacheTexture(m_nFillBlkIdx).Display(x,y);	
				EndDisplayBlock();
			}			
		}
	}
	else
	{		
		Create32MemBmp(m_szBlock.cx,m_szBlock.cy);

		SetAlphaValues(m_hBmp);

		if( !bZeroBlock )
		{
			if( m_pD3D->m_pD3DDevice9->TestCooperativeLevel()!=D3D_OK )
				return;

			if(GetCacheTexture(m_nFillBlkIdx).m_nType==CD3DTexture::typeRender)
			{
				CD3DTexture *pSrc = &GetCacheTexture(m_nFillBlkIdx);
				CD3DTexture *pDes = GetSystemTextureForSetBitmap();
				if(pSrc)
				{
					LPDIRECT3DSURFACE9 pSurface1=NULL;
					LPDIRECT3DSURFACE9 pSurface2=NULL; 
					
					pSrc->m_pTexture->GetSurfaceLevel(0,&pSurface1);
					
					pDes->m_pTexture->GetSurfaceLevel(0,&pSurface2);

					m_pD3D->m_pD3DDevice9->GetRenderTargetData(pSurface1,pSurface2);

					pSurface2->Release();
					pSurface1->Release();
					pDes->CopyToBitmap(m_hBmp);
				}			
			}
			else
			{
				GetCacheTexture(m_nFillBlkIdx).CopyToBitmap(m_hBmp);			
			}
		}
	}
}


HBITMAP CCacheD3DTextData::Create32MemBmp(int w, int h)
{
	COLORREF bakCol = m_pFillContext->GetBackColor();

	if( m_hBmp!=NULL )
	{
		BITMAP sinfo;
		if( ::GetObject( m_hBmp, sizeof(BITMAP), &sinfo)==0 )return NULL;

		if (bakCol == 0 || bakCol == 0xffffff)
			memset(sinfo.bmBits,bakCol,sinfo.bmWidthBytes*sinfo.bmHeight);	
		else
		{
			::SelectObject(m_hMemDC,m_hBmpOld);

			HBITMAP hOldTemBitmap = (HBITMAP)::SelectObject(m_hTmpDC, m_hBmp);
			HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(bakCol);
			CRect rect(0,0,w,h);
			::FillRect(m_hTmpDC,&rect,hbrush);
			DeleteObject(hbrush);
			(HBITMAP)::SelectObject(m_hTmpDC, hOldTemBitmap);

			m_hBmpOld = (HBITMAP)::SelectObject(m_hMemDC,m_hBmp);
		}

		//memset(sinfo.bmBits,0,sinfo.bmWidthBytes*sinfo.bmHeight);
		return m_hBmp;
	}
	m_hMemDC = ::CreateCompatibleDC(NULL);
	m_hTmpDC = ::CreateCompatibleDC(NULL);

	BITMAPINFO bitmapInfo;
	memset(&bitmapInfo,0,sizeof(bitmapInfo));
	SetbmiHeader(&bitmapInfo,w,h,32);
	
	BYTE *pBits;
	m_hBmp = CreateDIBSection( NULL,&bitmapInfo,DIB_RGB_COLORS,
		(void**)&pBits,0,0L );

	//memset(pBits,0,w*h*4);
	if (bakCol == 0 || bakCol == 0xffffff)
		memset(pBits,bakCol,w*h*4);	
	else
	{
		HBITMAP hOldTemBitmap = (HBITMAP)::SelectObject(m_hTmpDC, m_hBmp);
		HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(bakCol);
		CRect rect(0,0,w,h);
		::FillRect(m_hTmpDC,&rect,hbrush);
		DeleteObject(hbrush);
		(HBITMAP)::SelectObject(m_hTmpDC, hOldTemBitmap);
	}
	
	if( m_hMemDC!=NULL && m_hBmp!=NULL )
		m_hBmpOld = (HBITMAP)::SelectObject(m_hMemDC,m_hBmp);
	return m_hBmp;
}

void CCacheD3DTextData::SetAlphaValues(HBITMAP hBmp)
{
	BITMAP sinfo;
	if( ::GetObject( hBmp, sizeof(BITMAP), &sinfo)==0 )return;
	if( sinfo.bmBitsPixel!=32 )return;

	DWORD *p, *pMax;
	int srcWidth = ((sinfo.bmWidth*(sinfo.bmBitsPixel>>3)+3)&~3);

	p = (DWORD*)sinfo.bmBits;

	COLORREF bakCol = m_pFillContext->GetBackColor();

	DWORD rgbaCol = D3DCOLOR_RGBA(GetRValue(bakCol),GetGValue(bakCol),GetBValue(bakCol),0);

	pMax = (DWORD*)(((BYTE*)sinfo.bmBits)+srcWidth*sinfo.bmHeight);

	while( p<pMax )
	{
		if( ((*p)&0xffffff)!=rgbaCol )
		{
			if(((*p)&0xff000000)==0)
				(*p) = ((*p)|0xff000000);
		}
		else
		{
			(*p) = ((*p)&0xffffff);
		}

		p++;
	}

	/*
	_asm
	{
			push eax
			push ebx
			push ecx
			push edx
			mov  eax, dword ptr[pMax]
			mov	 ecx, dword ptr [p]
			mov	 edx, dword ptr[rgbaCol]
		SetAlphaValues_ASM_LOOP_START:
			cmp  ecx, eax					; // while( p<pMax )
			jge  SetAlphaValues_ASM_LOOP_OVER
			mov  ebx, dword ptr [ecx]
			and  ebx, 0xffffff
			cmp	 ebx, edx					; // if( ((*p)&0xffffff)!=rgbaCol )
			je   SetAlphaValues_ASM_CLEAR
			or   ebx, 0xff000000			; // (*p) = ((*p)|0xff000000);
			mov  dword ptr [ecx], ebx
			jmp  SetAlphaValues_ASM_LOOP_CONTINUE
SetAlphaValues_ASM_CLEAR:
			and   ebx, 0xffffff			; // (*p) = ((*p)&0xffffff);
			mov  dword ptr [ecx], ebx
SetAlphaValues_ASM_LOOP_CONTINUE:
			add  ecx, 4						; // p++;
			jmp  SetAlphaValues_ASM_LOOP_START
SetAlphaValues_ASM_LOOP_OVER:
			pop  edx
			pop  ecx
			pop  ebx
			pop  eax
	}*/
}

//此函数应实现：缓存CDrawingContext::DrawGrBuffer的操作，以便将来能够快速显示它
void CCacheD3DTextData::DrawGrBuffer(GrElementList *pList, int mode)
{
	if( m_bDirectRenderVectors )
	{
		if( m_pFillContext )
		{
			m_pFillContext->DrawGrBuffer(pList,mode);
		}
	}
	else
	{
		HBITMAP hBmp = m_hBmp;
		C2DGDIDrawingContext dc;
		dc.SetBackColor(m_pFillContext->GetBackColor());
		dc.CreateContext(m_hMemDC);
		dc.SetCoordSys(m_pFillContext->GetCoordSys());
		dc.SetDCSize(m_pFillContext->GetDCSize());
		dc.CopyDisplaySetting(m_pFillContext);
		CRect rcView;
		m_pFillContext->GetViewRect(rcView);
		dc.SetViewRect(rcView);
		dc.DrawGrBuffer(pList,mode);
	}
}



//此函数应实现：缓存CDrawingContext::DrawGrBuffer2d的操作，以便将来能够快速显示它
void CCacheD3DTextData::DrawGrBuffer2d(GrElementList *pList, int mode)
{
	if( m_bDirectRenderVectors )
	{
		if( m_pFillContext )
		{
			m_pFillContext->DrawGrBuffer2d(pList,mode);
		}
	}
	else
	{
		HBITMAP hBmp = m_hBmp;
		C2DGDIDrawingContext dc;
		dc.CreateContext(m_hMemDC);
		dc.SetCoordSys(m_pFillContext->GetCoordSys());
		dc.SetDCSize(m_pFillContext->GetDCSize());
		dc.CopyDisplaySetting(m_pFillContext);
		CRect rcView;
		m_pFillContext->GetViewRect(rcView);
		dc.SetViewRect(rcView);
		dc.DrawGrBuffer2d(pList,mode);
	}

}


void CCacheD3DTextData::DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor)
{
	if( m_bDirectRenderVectors )
	{
		CDrawingContext *p = m_pFillContext;
		if( !p )return;
		
		if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
		{
			p = ((CStereoDrawingContext*)p)->GetCurContext();
		}
		
		if( !p->IsKindOf(RUNTIME_CLASS(CD3DDrawingContext)) )
			return;	
		
		p->DrawGeometryMark(pList,mode,ptInfor);
	}
	else
	{
		HBITMAP hBmp = m_hBmp;
		C2DGDIDrawingContext dc;
		dc.CreateContext(m_hMemDC);
		dc.SetCoordSys(m_pFillContext->GetCoordSys());
		dc.SetDCSize(m_pFillContext->GetDCSize());
		dc.CopyDisplaySetting(m_pFillContext);
		CRect rcView;
		m_pFillContext->GetViewRect(rcView);
		dc.SetViewRect(rcView);
		dc.DrawGeometryMark(pList,mode,ptInfor);
	}
}

void CCacheD3DTextData::DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	if( m_bDirectRenderVectors )
	{
		if( m_pFillContext )
		{
			m_pFillContext->DrawSelectionMark2d(pList,mode,clr,wid);
		}
	}
	else
	{
		HBITMAP hBmp = m_hBmp;
		C2DGDIDrawingContext dc;
		dc.CreateContext(m_hMemDC);
		dc.SetCoordSys(m_pFillContext->GetCoordSys());
		dc.SetDCSize(m_pFillContext->GetDCSize());
		dc.CopyDisplaySetting(m_pFillContext);
		CRect rcView;
		m_pFillContext->GetViewRect(rcView);
		dc.SetViewRect(rcView);
		dc.DrawSelectionMark2d(pList,mode,clr,wid);
	}
}

void CCacheD3DTextData::DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid)
{
	if( m_bDirectRenderVectors )
	{
		if( m_pFillContext )
		{
			m_pFillContext->DrawSelectionMark(pList,mode,clr,wid);
		}
	}
	else
	{
		HBITMAP hBmp = m_hBmp;
		C2DGDIDrawingContext dc;
		dc.CreateContext(m_hMemDC);
		dc.SetCoordSys(m_pFillContext->GetCoordSys());
		dc.SetDCSize(m_pFillContext->GetDCSize());
		dc.CopyDisplaySetting(m_pFillContext);
		CRect rcView;
		m_pFillContext->GetViewRect(rcView);
		dc.SetViewRect(rcView);
		dc.DrawSelectionMark(pList,mode,clr,wid);
	}
}


//此函数应实现：缓存CDrawingContext::DrawBmp的操作，以便将来能够快速显示它
void CCacheD3DTextData::DrawBitmap(HBITMAP hBmp, CRect rcBmp, CRect rcDraw,  BOOL bCover)
{
	if( bCover )
	{
		if( m_nFillBlkIdx>=0 && m_nFillBlkIdx<m_nBlockSum )
		{
			CDrawingContext *p = m_pFillContext;
			if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
			{
				p = ((CStereoDrawingContext*)p)->GetCurContext();
			}
			
			FilterBmp(hBmp,p->GetRGBMask(),(p->GetBackColor()&0xff));
//			GetCacheTexture(m_nFillBlkIdx).SetBitmap(hBmp,rcBmp.left,rcBmp.top,
//				rcDraw.left,rcDraw.top,rcDraw.Width(),rcDraw.Height(),FALSE);
//			GetCacheTexture(m_nFillBlkIdx).m_bfilled = TRUE;

			OverlapBitmap(hBmp,NULL,rcBmp,m_hBmp,NULL,rcDraw,FALSE);
		}
	}
	else
	{
		CDrawingContext *p = m_pFillContext;
		if( p->IsKindOf(RUNTIME_CLASS(CStereoDrawingContext)) )
		{
			p = ((CStereoDrawingContext*)p)->GetCurContext();
		}

		HBITMAP hBmp2 = GetCacheTexture(m_nFillBlkIdx).GetBitmap();

		OverlapBitmap(hBmp,NULL,rcBmp,hBmp2,NULL,CRect(0,0,m_szBlock.cx,m_szBlock.cy),
			TRUE,p->GetBackColor(),p->GetRGBMask());

//		GetCacheTexture(m_nFillBlkIdx).SetBitmap(hBmp2,rcBmp.left,rcBmp.top,
//				rcDraw.left,rcDraw.top,rcDraw.Width(),rcDraw.Height(),FALSE);
//		GetCacheTexture(m_nFillBlkIdx).m_bfilled = TRUE;

		::DeleteObject(hBmp2);
	}
}



//填充cache块的结束函数
void CCacheD3DTextData::EndFillBlock()
{
	if( m_bDirectRenderVectors )
	{
		if( m_pD3D->m_pD3DDevice9->TestCooperativeLevel()!=D3D_OK )
			return;

		m_pD3D->EndDrawing(FALSE);

		m_pD3D->EndRenderTexture();
		m_pD3D->SetRenderTexture(0,0,0);

		if(GetCacheTexture(m_nFillBlkIdx).m_nType==CD3DTexture::typeRender)
		{
			
		}
		else
		{
			CD3DTexture *pRT = GetRenderTexture();
			if( pRT )
				GetCacheTexture(m_nFillBlkIdx).CopyFromRenderTexture(pRT->m_pTexture);			
		}
	}
	else
	{
		if (m_rcFillBlk.IsRectEmpty())
			SetAlphaValues(m_hBmp);
		else
			SetAlphaValues_Rect(m_hBmp, m_rcFillBlk);
			
		if(GetCacheTexture(m_nFillBlkIdx).m_nType==CD3DTexture::typeRender)
		{
			if( m_pD3D->m_pD3DDevice9->TestCooperativeLevel()!=D3D_OK )
				return;

			CD3DTexture *pSrc = GetSystemTextureForSetBitmap();
			CD3DTexture *pDes = &GetCacheTexture(m_nFillBlkIdx);
			if(pSrc)
			{
				pSrc->SetBitmap(m_hBmp,0,0,0,0,m_szBlock.cx,m_szBlock.cy,TRUE);

				LPDIRECT3DSURFACE9 pSurface1=NULL;
				LPDIRECT3DSURFACE9 pSurface2=NULL; 
				
				pSrc->m_pTexture->GetSurfaceLevel(0,&pSurface1);
				
				pDes->m_pTexture->GetSurfaceLevel(0,&pSurface2);

				m_pD3D->m_pD3DDevice9->UpdateSurface(pSurface1,NULL,pSurface2,NULL);

				pSurface2->Release();
				pSurface1->Release();
			}			
		}
		else
		{
			GetCacheTexture(m_nFillBlkIdx).SetBitmap(m_hBmp,0,0,0,0,m_szBlock.cx,m_szBlock.cy,TRUE);
		}
	}

	CCacheData::EndFillBlock();
}


void CCacheD3DTextData::BeginDisplayBlock()
{
// 	m_pD3D->SetRenderState(D3DRS_ALPHATESTENABLE,m_bUseAlpha);
// 	m_pD3D->SetRenderState(D3DRS_ALPHAREF,0);
// 	m_pD3D->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, m_bUseAlpha );//开启ALPHA混合功能  
	m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);//设置源混合因子为（As,As,As,As)  
	m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);//设置目标混合因子为

//	m_pD3D->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pD3D->SetRenderState(D3DRS_ZENABLE,FALSE);

	if( m_bUseLinearZoom )
	{
		m_pD3D->m_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_pD3D->m_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_pD3D->m_pD3DDevice9->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		m_pD3D->m_pD3DDevice9->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	}
	else
	{
		m_pD3D->m_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_pD3D->m_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_pD3D->m_pD3DDevice9->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);
		m_pD3D->m_pD3DDevice9->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
	}
}

void CCacheD3DTextData::EndDisplayBlock()
{
//	m_pD3D->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	m_pD3D->SetRenderState(D3DRS_ZENABLE,FALSE);
}

// x,y是客户窗口坐标
void CCacheD3DTextData::DisplayBlock(CDrawingContext *p, int blkIdx, double x[4], double y[4])
{
	double y2[4];
	CSize szDC = p->GetDCSize();

	//转换成 Opengl 坐标
	y2[0] = szDC.cy-y[0];
	y2[1] = szDC.cy-y[1];
	y2[2] = szDC.cy-y[2];
	y2[3] = szDC.cy-y[3];
	
	GetCacheTexture(blkIdx).Display(x,y2);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCacheLoader::CCacheLoader():
m_szBlock(512,512),m_ptViewFocus(0,0)
{
	m_bInitCacheOK= FALSE;
	
	m_pCacheData = NULL;

	m_bLastFindFreeBlockFull = FALSE;

	m_bEnableBGLoad = TRUE;

	m_cacheTable.ids = NULL;
	m_cacheTable.nsum = 0;
}

CCacheLoader::~CCacheLoader()
{
	Destroy();
}


BOOL CCacheLoader::InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK)
{
	if (IsValid())  Destroy();

	if( m_pCacheData==NULL )
	{
		if( nCacheType==CHEDATA_TYPE_BMP )
			m_pCacheData = new CCacheBmpData;
		else if( nCacheType==CHEDATA_TYPE_GLLIST )
			m_pCacheData = new CCacheGLListData;
		else if( nCacheType==CHEDATA_TYPE_GLTEXT )
			m_pCacheData = new CCacheGLTextData;
		else if( nCacheType==CHEDATA_TYPE_D3DTEXT )
			m_pCacheData = new CCacheD3DTextData;
	}
	
	if( m_pCacheData!=NULL )
	{
		if( !m_pCacheData->Init(szBlock,nBlock,clrBK) )
			return FALSE;
	}

	m_szBlock = szBlock;

	m_cacheTable.ids	= new CacheID[nBlock];
	m_cacheTable.nsum	= nBlock;
	
	if( m_cacheTable.ids==NULL )
	{
		Destroy();
		return FALSE;
	}

	m_bInitCacheOK = TRUE;

	return TRUE;
}

BOOL CCacheLoader::DestroyCache()
{
	if( m_cacheTable.ids!=0 )
	{
		delete[] m_cacheTable.ids;
		m_cacheTable.ids = NULL;
		m_cacheTable.nsum = 0;
	}
	
	if( m_pCacheData!=NULL )
	{
		delete m_pCacheData;
		m_pCacheData = NULL;
	}
	
	m_bInitCacheOK= FALSE;
	
	m_bLastFindFreeBlockFull = FALSE;

	return TRUE;
}

BOOL CCacheLoader::IsValid()
{
	return m_bInitCacheOK;
}


void CCacheLoader::Destroy()
{
	if( m_cacheTable.ids!=0 )
	{
		delete[] m_cacheTable.ids;
		m_cacheTable.ids = NULL;
		m_cacheTable.nsum = 0;
	}

	if( m_pCacheData!=NULL )
	{
		delete m_pCacheData;
		m_pCacheData = NULL;
	}

	m_bInitCacheOK= FALSE;
	
	m_bLastFindFreeBlockFull = FALSE;
}


int CCacheLoader::GetCacheType()
{
	if( m_pCacheData )
		return m_pCacheData->GetType();

	return CHEDATA_TYPE_NONE;
}


void CCacheLoader::PlanBGLoad()
{
	int blkidx;
	CacheID cid;
	
	if( !m_bEnableBGLoad )return;
	
	if( FindFreeBlock(&blkidx,&cid) )
	{
		if( blkidx>=0 && blkidx<m_cacheTable.nsum )
		{
			ClearBlock(blkidx);					
			QueueBlock( blkidx, cid );
		}
	}
}



BOOL CCacheLoader::FindFreeBlock( int* pBlkIdx, CacheID* pid )
{
	// validity
	if( !IsValid() )return FALSE;

	CPoint ptFocus = m_ptViewFocus;

	int maxnum = -1;
	double dx,dy,dis,maxdis=-1,mindis=-1;
	double x[4],y[4];
	int blkIdx = -1, i, j;	
	CacheID tc;

	//最远的调入到cache中的影像块ID
	CacheID farest_id;

	int xfocus, yfocus;
	int nxmin = 0, nxmax = 0, nymin = 0, nymax = 0;
	
	GetFocusBlockNum(ptFocus, xfocus, yfocus);
	GetBlockNumRange(nxmin, nxmax, nymin, nymax);
	
	int diam = (int)ceil(sqrt((double)m_cacheTable.nsum))+2;
	
	CRect rctest(xfocus-diam,yfocus-diam,xfocus+diam,yfocus+diam);

	int testwid = rctest.Width(), testhei = rctest.Height();

	//影像块是否已经在cache中的标记数组
	int *hitflags = new int[testwid*testhei];
	if( hitflags==NULL )return FALSE;

	memset(hitflags,0,sizeof(int)*testwid*testhei);

	//查找空块或者最远的块
	for( i=0;i<m_cacheTable.nsum;i++ )
	{
		tc = m_cacheTable.ids[i];
		if( tc.IsNotInValid() )
		{
			//计算块中心到当前焦点的距离
			GetBlockClientXY(tc,x,y);
			
			x[0] = (x[0]+x[1]+x[2]+x[3])*0.25;
			y[0] = (y[0]+y[1]+y[2]+y[3])*0.25;
			
			dx = (x[0] - ptFocus.x);
			dy = (y[0] - ptFocus.y);
			dis = dx*dx + dy*dy;
			
			if( dis>maxdis )
			{ 
				maxdis = dis; 
				maxnum = i; 
				farest_id = tc; 
			}

			//此块在判断区域内部
			if( tc.xnum>=rctest.left && tc.xnum<rctest.right &&
				tc.ynum>=rctest.top && tc.ynum<rctest.bottom )
			{
				hitflags[(tc.ynum-rctest.top)*testwid+tc.xnum-rctest.left] = 1;
			}
		}
		else
		{
			blkIdx = i;
		}
	}

	//没有空块，就找最远的块
	if( blkIdx==-1 )blkIdx = maxnum;
	else farest_id = CacheID();

	if( pBlkIdx!=NULL )*pBlkIdx = blkIdx;
	if( pid==NULL )
	{
		delete[] hitflags;
		return TRUE;
	}

	//查找最近的没有调入的块
	pid->bvalid = CacheID::stateInvalid;
	for( i=rctest.top;i<rctest.bottom;i++ )
	{
		if( i<nymin || i>nymax )continue;
		for( j=rctest.left;j<rctest.right;j++ )
		{
			if( j<nxmin || j>nxmax )continue;

			//找到最近的不在cache中的影像块号
			if( hitflags[(i-rctest.top)*testwid+j-rctest.left]!=1 )
			{
				//计算块中心到当前焦点的距离
				tc.xnum = j; tc.ynum = i;
				GetBlockClientXY(tc,x,y);
				
				x[0] = (x[0]+x[1]+x[2]+x[3])*0.25;
				y[0] = (y[0]+y[1]+y[2]+y[3])*0.25;
				
				dx = (x[0] - ptFocus.x);
				dy = (y[0] - ptFocus.y);
				dis = dx*dx + dy*dy;
				if( mindis<0 || dis<mindis )
				{ 
					mindis = dis; 
					pid->Set(j,i,CacheID::stateLoading);
				}
			}
		}
	}

	delete[] hitflags;	
	if( pid->bvalid==CacheID::stateInvalid )return FALSE;

	//计算最远的块的中心到当前焦点的距离
	if( farest_id.bvalid!=CacheID::stateInvalid )
	{
		GetBlockClientXY(farest_id,x,y);
		
		x[0] = (x[0]+x[1]+x[2]+x[3])*0.25;
		y[0] = (y[0]+y[1]+y[2]+y[3])*0.25;
		
		dx = (x[0] - ptFocus.x);
		dy = (y[0] - ptFocus.y);

		double d1 = dx*dx + dy*dy;

		//计算命中的块的中心到当前焦点的距离
		GetBlockClientXY(*pid,x,y);
		
		x[0] = (x[0]+x[1]+x[2]+x[3])*0.25;
		y[0] = (y[0]+y[1]+y[2]+y[3])*0.25;
		
		dx = (x[0] - ptFocus.x);
		dy = (y[0] - ptFocus.y);

		if( m_bLastFindFreeBlockFull )
		{
			int limit = 1024/m_szBlock.cx;
			//如果命中块在当前块的 1/4 的核心区域外，那我们可以暂时不去调度它
			if( fabs((double)(pid->xnum-xfocus))>=limit || fabs((double)(pid->ynum-yfocus))>=limit )
				return FALSE;
		}
		
		double d2 = dx*dx + dy*dy;
		
		//如果最远的调入到cache的影像块比最近的没有调入到cache的影像块还要远，
		//那么可以认为现在的cache是理想的，不需要再调度了；
		if( farest_id.bvalid!=CacheID::stateInvalid && (d1*0.8)<d2 )
		{
			m_bLastFindFreeBlockFull = TRUE;
			return FALSE;
		}
		m_bLastFindFreeBlockFull = FALSE;
	}

	return TRUE;
}


void CCacheLoader::ClearBlock(int blkIdx)
{
	if( blkIdx<0 || blkIdx>=m_cacheTable.nsum )return;
	if( m_cacheTable.ids[blkIdx].bvalid==1 )
	{
		if( m_pCacheData )
		{
			m_pCacheData->ClearBlock(blkIdx);
		}
	}
	
	m_cacheTable.ids[blkIdx]  = CacheID();
	
	ReadQueue q;
	for( int i=m_aReadQueue.GetSize()-1; i>=0; i--)
	{
		q = m_aReadQueue.GetAt(i);
		if( q.id.CompareIncLoading(m_cacheTable.ids[blkIdx]) || q.blkidx==blkIdx )
		{
			m_aReadQueue.RemoveAt(i);
		}
	}
}


void CCacheLoader::QueueBlock(int blkIdx, CacheID id)
{
	ReadQueue q;
	q.id = id;
	q.blkidx = blkIdx;
	m_aReadQueue.Add(q);
	m_cacheTable.ids[blkIdx].xnum = id.xnum;
	m_cacheTable.ids[blkIdx].ynum = id.ynum;
	m_cacheTable.ids[blkIdx].bvalid = CacheID::stateLoading;
}


void CCacheLoader::ClearQueue()
{
	m_aReadQueue.RemoveAll();

	for( int i=0; i<m_cacheTable.nsum; i++)
	{
		if( m_cacheTable.ids[i].bvalid==CacheID::stateLoading )
		{
			m_cacheTable.ids[i] = CacheID();
		}
	}
}


BOOL CCacheLoader::ReplyQueue(int queueIdx)
{
	if( !IsValid() )return FALSE;
	if( queueIdx<0 || queueIdx>=m_aReadQueue.GetSize() )
		return FALSE;
	
	ReadQueue q = m_aReadQueue.GetAt(queueIdx);
	if( q.id.bvalid==CacheID::stateInvalid ||
		q.blkidx<0 || q.blkidx>=m_cacheTable.nsum )
	{
		return FALSE;
	}
	
	if( !FillBlock(q.blkidx,q.id) )return FALSE;
	
	return TRUE;
}


BOOL CCacheLoader::FillBlock(int blkIdx, CacheID id)
{
	ReadQueue q;
	for( int i=m_aReadQueue.GetSize()-1; i>=0; i--)
	{
		q = m_aReadQueue.GetAt(i);
		if( q.id.CompareIncLoading(id) ||	q.blkidx==blkIdx )
		{
			m_aReadQueue.RemoveAt(i);
		}
	}
	
	if( blkIdx>=0 && blkIdx<m_cacheTable.nsum )
	{
		m_cacheTable.ids[blkIdx].xnum = id.xnum;
		m_cacheTable.ids[blkIdx].ynum = id.ynum;
		m_cacheTable.ids[blkIdx].bvalid = CacheID::stateValid;
		m_cacheTable.ids[blkIdx].zoom = id.zoom;
		if (m_pCacheData != NULL)
			m_pCacheData->SetBlockDecodedState(blkIdx,CCacheData::DecodedItem::state_modified);
	}
	
	return TRUE;
}


BOOL CCacheLoader::BGLoad()
{
	if( !m_bEnableBGLoad )return FALSE;
	
	for( int i=m_aReadQueue.GetSize()-1; i>=0; i--)
	{
		if( ReplyQueue(i) )
			return TRUE;
		m_aReadQueue.RemoveAt(i);
	}
	
	return FALSE;
}

void CCacheLoader::SetViewFocus(int x, int y)
{
	m_ptViewFocus.x = x;
	m_ptViewFocus.y = y;
}


void CCacheLoader::EnableBGLoad(BOOL bEnable)
{
	m_bEnableBGLoad = bEnable;
}




int CCacheLoader::GetBlockIdx( CacheID id )
{
	for( int i=0;i<m_cacheTable.nsum;i++ )
	{
		if( m_cacheTable.ids[i]==id ) return i;
	}
	
	return -1;
}


void CCacheLoader::ClearAll()
{
	for( int i=0;i<m_cacheTable.nsum;i++ )
	{
		m_cacheTable.ids[i]  = CacheID();
	}
	m_aReadQueue.RemoveAll();

	if( m_pCacheData )
	{
		m_pCacheData->ClearAll();
	}
}


MyNameSpaceEnd



BYTE clrTable_CAD[] = {
0,0,0,
255,0,0,
255,255,0,
0,255,0,
0,255,255,
0,0,255,
255,0,255,
254,254,254,
65,65,65,
128,128,128,
255,0,0,
255,170,170,
189,0,0,
189,126,126,
129,0,0,
129,86,86,
104,0,0,
104,69,69,
79,0,0,
79,53,53,
255,63,0,
255,191,170,
189,46,0,
189,141,126,
129,31,0,
129,96,86,
104,25,0,
104,78,69,
79,19,0,
79,59,53,
255,127,0,
255,212,170,
189,94,0,
189,157,126,
129,64,0,
129,107,86,
104,52,0,
104,86,69,
79,39,0,
79,66,53,
255,191,0,
255,234,170,
189,141,0,
189,173,126,
129,96,0,
129,118,86,
104,78,0,
104,95,69,
79,59,0,
79,73,53,
255,255,0,
255,255,170,
189,189,0,
189,189,126,
129,129,0,
129,129,86,
104,104,0,
104,104,69,
79,79,0,
79,79,53,
191,255,0,
234,255,170,
141,189,0,
173,189,126,
96,129,0,
118,129,86,
78,104,0,
95,104,69,
59,79,0,
73,79,53,
127,255,0,
212,255,170,
94,189,0,
157,189,126,
64,129,0,
107,129,86,
52,104,0,
86,104,69,
39,79,0,
66,79,53,
63,255,0,
191,255,170,
46,189,0,
141,189,126,
31,129,0,
96,129,86,
25,104,0,
78,104,69,
19,79,0,
59,79,53,
0,255,0,
170,255,170,
0,189,0,
126,189,126,
0,129,0,
86,129,86,
0,104,0,
69,104,69,
0,79,0,
53,79,53,
0,255,63,
170,255,191,
0,189,46,
126,189,141,
0,129,31,
86,129,96,
0,104,25,
69,104,78,
0,79,19,
53,79,59,
0,255,127,
170,255,212,
0,189,94,
126,189,157,
0,129,64,
86,129,107,
0,104,52,
69,104,86,
0,79,39,
53,79,66,
0,255,191,
170,255,234,
0,189,141,
126,189,173,
0,129,96,
86,129,118,
0,104,78,
69,104,95,
0,79,59,
53,79,73,
0,255,255,
170,255,255,
0,189,189,
126,189,189,
0,129,129,
86,129,129,
0,104,104,
69,104,104,
0,79,79,
53,79,79,
0,191,255,
170,234,255,
0,141,189,
126,173,189,
0,96,129,
86,118,129,
0,78,104,
69,95,104,
0,59,79,
53,73,79,
0,127,255,
170,212,255,
0,94,189,
126,157,189,
0,64,129,
86,107,129,
0,52,104,
69,86,104,
0,39,79,
53,66,79,
0,63,255,
170,191,255,
0,46,189,
126,141,189,
0,31,129,
86,96,129,
0,25,104,
69,78,104,
0,19,79,
53,59,79,
0,0,255,
170,170,255,
0,0,189,
126,126,189,
0,0,129,
86,86,129,
0,0,104,
69,69,104,
0,0,79,
53,53,79,
63,0,255,
191,170,255,
46,0,189,
141,126,189,
31,0,129,
96,86,129,
25,0,104,
78,69,104,
19,0,79,
59,53,79,
127,0,255,
212,170,255,
94,0,189,
157,126,189,
64,0,129,
107,86,129,
52,0,104,
86,69,104,
39,0,79,
66,53,79,
191,0,255,
234,170,255,
141,0,189,
173,126,189,
96,0,129,
118,86,129,
78,0,104,
95,69,104,
59,0,79,
73,53,79,
255,0,255,
255,170,255,
189,0,189,
189,126,189,
129,0,129,
129,86,129,
104,0,104,
104,69,104,
79,0,79,
79,53,79,
255,0,191,
255,170,234,
189,0,141,
189,126,173,
129,0,96,
129,86,118,
104,0,78,
104,69,95,
79,0,59,
79,53,73,
255,0,127,
255,170,212,
189,0,94,
189,126,157,
129,0,64,
129,86,107,
104,0,52,
104,69,86,
79,0,39,
79,53,66,
255,0,63,
255,170,191,
189,0,46,
189,126,141,
129,0,31,
129,86,96,
104,0,25,
104,69,78,
79,0,19,
79,53,59,
51,51,51,
80,80,80,
105,105,105,
130,130,130,
190,190,190,
255,255,255 
};