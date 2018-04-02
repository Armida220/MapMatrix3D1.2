// ImageAdjust.cpp: implementation of the CImageAdjust class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageAdjust.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CImageAdjust::CImageAdjust()
{
	nBrightness = 0;
	nContrast = 0;
	bHistogram = FALSE;
	nRRange[0] = nRRange[1] = 0;
	nGRange[0] = nGRange[1] = 0;
	nBRange[0] = nBRange[1] = 0;

	bAdjustMap = FALSE;
	bAdjustHistogram = FALSE;
	
	memset(pRAdjustMap,0,sizeof(pRAdjustMap));
	memset(pGAdjustMap,0,sizeof(pGAdjustMap));
	memset(pBAdjustMap,0,sizeof(pBAdjustMap));
}


CImageAdjust::~CImageAdjust()
{
}

void CImageAdjust::StatHistogramInfo(HBITMAP hBmp, double k)
{
	DIBSECTION info;
	if( GetObject(hBmp,sizeof(info),&info)==0 )
		return;

	if( info.dsBm.bmBitsPixel!=24 )
		return;

	int bitCnt = info.dsBm.bmBitsPixel;
	
	int lineBytes= (info.dsBm.bmWidth*bitCnt+7)/8;
	lineBytes = ((lineBytes+3)&(~3));

	BYTE* pBits=(BYTE*)info.dsBm.bmBits;

	DWORD num1[256], num2[256], num3[256];

	memset(num1,0,sizeof(num1));
	memset(num2,0,sizeof(num2));
	memset(num3,0,sizeof(num3));

	int nWid = info.dsBm.bmWidth, nHei = info.dsBm.bmHeight;
	int nSkipBytes = lineBytes-(nWid*bitCnt+7)/8, i, j;
	for( i=0; i<nHei; i++)
	{
		for( j=0; j<nWid; j++,pBits+=3)
		{
			num1[pBits[2]]++;
			num2[pBits[1]]++;
			num3[pBits[0]]++;
		}
		
		pBits += nSkipBytes;
	}

	num1[0] = num1[255] = num2[0] = num2[255] = num3[0] = num3[255] = 0;

	if( k<0 )
		k = 0.005;

	int limit = nHei*nWid * k, t = 0;

	// red range
	for( i=0, t=0; i<256; i++)
	{
		t += num1[i];
		if( t>=limit )break;
	}
	nRRange[0] = i;
	
	for( i=255, t=0; i>=0; i--)
	{
		t += num1[i];
		if( t>=limit )break;
	}
	nRRange[1] = i;

	// green range
	for( i=0, t=0; i<256; i++)
	{
		t += num2[i];
		if( t>=limit )break;
	}
	nGRange[0] = i;
	
	for( i=255, t=0; i>=0; i--)
	{
		t += num2[i];
		if( t>=limit )break;
	}
	nGRange[1] = i;

	// blue range
	for( i=0, t=0; i<256; i++)
	{
		t += num3[i];
		if( t>=limit )break;
	}
	nBRange[0] = i;
	
	for( i=255, t=0; i>=0; i--)
	{
		t += num3[i];
		if( t>=limit )break;
	}
	nBRange[1] = i;

	bAdjustHistogram = TRUE;
}


void CImageAdjust::CalcAdjustMap()
{
	/* 自动直方图调节的计算公式 
	*  y = 0; (x<=x0)
	*  y = 255; (x>=x1)
	*  y = (x-x0)*255/(x1-x0); (x>x0 && x<x1)
	*  x0,x1 分别是象素的有效范围（比实际的范围要稍小）；
	*/

	/* 对比度亮度调节的计算公式 y = (x-127)*c+b+127; 
	*  c是对比度，取值范围可以定义为[0,N](N>1)，b是亮度，取值范围可以定义为[-255,255]；
	*  但使用的时候，b,c的取值范围为[-100,100]；通过平移后，可以得到公式
	*  1) y = (x-127)*(c/100+1)+b*2.55+127; (c<0)
	*  2) y = (x-127)*(c*(N-1)/100+1)+b*2.55+127; (c>=0)
	*  下面的代码里，取 N=5;
	*/
	
	float c = (nContrast>=0?(nContrast*0.04+1):(nContrast*0.01+1));
	float b = nBrightness*2.55+127-127*c;
	float k[3] = {
		255.0/(nRRange[1]-nRRange[0]),
		255.0/(nGRange[1]-nGRange[0]),
		255.0/(nBRange[1]-nBRange[0])
	};
	float d[3] = {
		-nRRange[0]*k[0],
		-nGRange[0]*k[1],
		-nBRange[0]*k[2]
	};

	float t;
	for( int i=0; i<256; i++)
	{
		// red map
		t = i;
		if( bHistogram )
		{
			if( t<nRRange[0] )t = 0;
			else if( t>nRRange[1] )t = 255;
			else
			{
				t = k[0]*t + d[0];
			}
		}
		
		t = t*c + b;
		if( t<0 )t = 0;
		else if( t>255 )t = 255;
		pRAdjustMap[i] = t;

		// green map
		t = i;
		if( bHistogram )
		{
			if( t<nGRange[0] )t = 0;
			else if( t>nGRange[1] )t = 255;
			else
			{
				t = k[1]*t + d[1];
			}
		}
		
		t = t*c + b;
		if( t<0 )t = 0;
		else if( t>255 )t = 255;
		pGAdjustMap[i] = t;

		// blue map
		t = i;
		if( bHistogram )
		{
			if( t<nBRange[0] )t = 0;
			else if( t>nBRange[1] )t = 255;
			else
			{
				t = k[2]*t + d[2];
			}
		}
		
		t = t*c + b;
		if( t<0 )t = 0;
		else if( t>255 )t = 255;
		pBAdjustMap[i] = t;
	}

	bAdjustMap = TRUE;
}

