#include "stdafx.h"
#include "math.h"
#include "glew.h"
#include "GlobalFunc.h"
#include "ImageAdjust.h"

extern AFX_EXTENSION_MODULE XtremeViewDLL;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//for test
void WriteToFile(HBITMAP hBmp)
{
	DIBSECTION dibInfo;
	::GetObject(hBmp,sizeof(dibInfo),&dibInfo);
	BITMAPFILEHEADER fileInfo;
	BITMAPINFO		 bmpInfo;

	fileInfo.bfType = 0x4d42;
	fileInfo.bfSize = sizeof(fileInfo)+sizeof(bmpInfo)+dibInfo.dsBmih.biSizeImage;
	fileInfo.bfReserved1 = 0;
	fileInfo.bfReserved2 = 0;
	fileInfo.bfOffBits = sizeof(fileInfo)+sizeof(bmpInfo);

	memset(&bmpInfo,0,sizeof(bmpInfo));
	memcpy(&bmpInfo.bmiHeader,&dibInfo.dsBmih,sizeof(dibInfo.dsBmih));
	
	char fileName[256];
	static int num=0;
	sprintf(fileName,"F:\\test%d.bmp",num++);
	FILE *fp = fopen(fileName,"wb");
	if( !fp )return;
	fwrite(&fileInfo,sizeof(fileInfo),1,fp);
	fwrite(&bmpInfo ,sizeof(bmpInfo ),1,fp);
	fwrite(dibInfo.dsBm.bmBits,1,dibInfo.dsBmih.biSizeImage,fp);
	fclose(fp);
}


HBITMAP Create24BitDIB(int w, int h, BOOL bSetZero,COLORREF clr)
{
	// a new bitmap need to be created
	BITMAPINFO bitmapInfo;

	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = w;
	bitmapInfo.bmiHeader.biHeight = h;
	bitmapInfo.bmiHeader.biSizeImage = ((w*3+3)&(~3))*h;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 24;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biClrImportant = 0;

	BYTE *pBits;
	HBITMAP hBmp = CreateDIBSection( NULL,&bitmapInfo,DIB_RGB_COLORS,
		(void**)&pBits,0,0L );

	if( bSetZero && hBmp && pBits )
	{
		if (clr == 0 || clr == 0xffffff)
			memset(pBits,clr,bitmapInfo.bmiHeader.biSizeImage);	
		else
		{
			HDC hTemDC = ::CreateCompatibleDC(NULL);
			HBITMAP hOldTemBitmap = (HBITMAP)::SelectObject(hTemDC, 			
				hBmp);
			HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(clr);
			CRect rect(0,0,w,h);
			::FillRect(hTemDC,&rect,hbrush);
			DeleteObject(hbrush);
			(HBITMAP)::SelectObject(hTemDC, hOldTemBitmap);

			if( hTemDC )::DeleteDC(hTemDC);
		}
// 		if( clr==0 )memset(pBits, 0, bitmapInfo.bmiHeader.biSizeImage);
// 		else memset(pBits, clr&0xff, bitmapInfo.bmiHeader.biSizeImage);
	}
	return hBmp;

}

int FindNearestColorItem(RGBQUAD *clrTbl, int nItem, COLORREF clr)
{
	int dis = -1, min = -1, idx = -1;
	BYTE r = GetRValue(clr), g = GetGValue(clr), b = GetBValue(clr); 
	for( int i=0; i<nItem; i++)
	{
		dis = (clrTbl[i].rgbRed-r)*(clrTbl[i].rgbRed-r)
			+(clrTbl[i].rgbGreen-g)*(clrTbl[i].rgbGreen-g)
			+(clrTbl[i].rgbBlue-b)*(clrTbl[i].rgbBlue-b);
		if( min<0 || min>dis )
		{
			idx = i; min = dis;
		}
	}
	return idx;
}


#define CPYCOLOR_BYMASK(_bMask,_p1,_p2)\
{\
	if( _bMask[0] )_p1[2]=_p2->rgbRed;\
	if( _bMask[1] )_p1[1]=_p2->rgbGreen;\
	if( _bMask[2] )_p1[0]=_p2->rgbBlue;\
}


BOOL OverlapBitmap(HBITMAP hSrc, RGBQUAD* srcClTbl, CRect& srcRect,
				   HBITMAP hDest, RGBQUAD* desClTbl, CRect& desRect,
				   BOOL bTransparent, COLORREF transColor,DWORD rgbMask)
{
	if( hDest==NULL || hSrc==NULL )return FALSE;

	DIBSECTION dinfo,sinfo;
	if( ::GetObject( hDest, sizeof(DIBSECTION), &dinfo)==0 )return FALSE;
	if( ::GetObject( hSrc , sizeof(DIBSECTION), &sinfo )==0 )return FALSE;

	BYTE* pDest=(BYTE*)dinfo.dsBm.bmBits;
	BYTE* pSrc =(BYTE*)sinfo.dsBm.bmBits;

	if( pDest==NULL || pSrc==NULL )return FALSE;

	int sBitCnt = sinfo.dsBm.bmBitsPixel, dBitCnt = dinfo.dsBm.bmBitsPixel;

	if( (sBitCnt==1||sBitCnt==4||sBitCnt==8) && srcClTbl==NULL )return FALSE;
	if( (dBitCnt==1||dBitCnt==4||dBitCnt==8) && desClTbl==NULL )return FALSE;	

	int	readWidth= srcRect.Width();
	int readBytes= (readWidth*sBitCnt+7)/8;
	int destWidth= (dinfo.dsBm.bmWidth*dBitCnt+7)/8;
	int srcWidth = (sinfo.dsBm.bmWidth*sBitCnt+7)/8;

	destWidth = ((destWidth+3)&(~3));
	srcWidth  = ((srcWidth+3)&(~3));

	BYTE* pTmpD = pDest+(dinfo.dsBm.bmHeight-1-desRect.top)*destWidth
		+ desRect.left*dBitCnt/8;

	BYTE* pTmpS = pSrc+(sinfo.dsBm.bmHeight-1-srcRect.top)*srcWidth
		+ srcRect.left*sBitCnt/8;	

	BYTE pixel[3]={0};

	BOOL bMask[3]={	((rgbMask&RGBMASK_RED)!=0), //red
					((rgbMask&RGBMASK_GREEN)!=0), //green
					((rgbMask&RGBMASK_BLUE)!=0) //blue 
	};

	//for speed
	if( sBitCnt==dBitCnt && !bTransparent && (bMask[0]&&bMask[1]&&bMask[2]) )
	{
		for( int i=srcRect.top; i<srcRect.bottom; i++)
		{
			memcpy(pTmpD,pTmpS,readBytes);
			pTmpD -= destWidth;
			pTmpS -= srcWidth;
		}
		return TRUE;
	}

	//calculate a serial of matched color indexes
	int *clridx = NULL;
	int scn = (1<<(sBitCnt)), dcn = (1<<(dBitCnt));
	if( (sBitCnt==1||sBitCnt==4||sBitCnt==8 ) && (dBitCnt==1||dBitCnt==4||dBitCnt==8) )
	{
		clridx = new int[scn];
		if( !clridx )return FALSE;

		int dis = -1, min = -1;
		for( int i=0; i<scn; i++)
		{
			if( srcClTbl[i].rgbRed==desClTbl[i].rgbRed && srcClTbl[i].rgbGreen==desClTbl[i].rgbGreen &&
				srcClTbl[i].rgbBlue==desClTbl[i].rgbBlue )
			{
				clridx[i] = i;
			}
			else
			{
				clridx[i] = FindNearestColorItem(desClTbl,dcn,
					RGB(srcClTbl[i].rgbRed,srcClTbl[i].rgbGreen,srcClTbl[i].rgbBlue));
			}
		}
	}

	if( (sBitCnt==1||sBitCnt==4||sBitCnt==8) && bTransparent )
	{
		transColor = FindNearestColorItem(srcClTbl,scn,transColor);
	}

	int nSInByte = 8/sBitCnt, nDInByte = 8/dBitCnt;	
	int	sidx = 0, didx = 0;
	int srest = 0, drest = 0;
	BYTE sBitMask, dBitMask, sbyte, dbyte;
	BYTE* pTmp1=NULL,*pTmp2=NULL;
	RGBQUAD* pCl = NULL;

	for( int i=srcRect.top; i<srcRect.bottom; i++)
	{
		pTmp1 =pTmpD;
		pTmp2 =pTmpS;

		int sp=sBitCnt, dp=dBitCnt;
		if( sBitCnt==1 || sBitCnt==4 || sBitCnt==8 )
		{
			for( int j=0; j<readBytes; j++,pTmp2++ )
			{
				sbyte = *pTmp2;
				dbyte = *pTmp1;
				sBitMask = 0xff;
				dBitMask = 0xff;
				for( sp=sBitCnt; sp<=8; sp+=sBitCnt )
				{
					sidx = ((sbyte>>(8-sp))&(0xff>>(8-sBitCnt)));
					if( !bTransparent || (bTransparent && sidx!=(int)transColor) )
					{			
						if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
						{
							sidx = clridx[sidx];
							dbyte = ((sidx<<(8-dp))|((~(((1<<dBitCnt)-1)<<(8-dp)))&dbyte) );
							dp += dBitCnt;
							if( dp>=8 )
							{
								*pTmp1++ = dbyte; dbyte = *pTmp1; dp = dBitCnt;
							}
						}
						else if( dBitCnt==24 )
						{
							pCl = srcClTbl+sidx;
							CPYCOLOR_BYMASK(bMask,pTmp1,pCl);
							pTmp1 += 3;
						}
						else if( dBitCnt==32 )
						{
							pCl = srcClTbl+sidx;
							CPYCOLOR_BYMASK(bMask,pTmp1,pCl);
							pTmp1 += 4;
						}
					}
					else
					{
						if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
						{
							dp += dBitCnt;
							if( dp>=8 )
							{
								*pTmp1++ = dbyte; dbyte = *pTmp1; dp = dBitCnt;
							}
						}
						else if( dBitCnt==24 )
						{
							pTmp1 += 3;
						}
						else if( dBitCnt==32 )
						{
							pTmp1 += 4;
						}
					}

					sBitMask = (sBitMask>>sBitCnt);
					dBitMask = (dBitMask>>dBitCnt);	
				}
			}
		}
		else
		{
			for( int j=0; j<readWidth; j++ )
			{
				COLORREF clr;
				if( sBitCnt==24 )
				{
					clr = RGB(pTmp2[2],pTmp2[1],pTmp2[0]);
					pTmp2 += 3;
				}
				else if( sBitCnt==32 )
				{
 					clr = RGB(pTmp2[2],pTmp2[1],pTmp2[0]);
					pTmp2 += 4;
				}

				if( !bTransparent || (bTransparent && clr!=transColor) )
				{				
					if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
					{
						sidx = FindNearestColorItem(desClTbl,dcn,clr);
						dbyte = ((sidx<<(8-dp))|((~(((1<<dBitCnt)-1)<<(8-dp)))&dbyte) );
						dp += dBitCnt;
						if( dp>=8 )
						{
							*pTmp1++ = dbyte; dbyte = *pTmp1; dp = dBitCnt;
						}
					}
					else if( dBitCnt==24 )
					{
						if( bMask[2] )pTmp1[0] = GetBValue(clr);
						if( bMask[1] )pTmp1[1] = GetGValue(clr);
						if( bMask[0] )pTmp1[2] = GetRValue(clr);
						pTmp1 += 3;
					}
					else if( dBitCnt==32 )
					{
						if( bMask[2] )pTmp1[0] = GetBValue(clr);
						if( bMask[1] )pTmp1[1] = GetGValue(clr);
						if( bMask[0] )pTmp1[2] = GetRValue(clr);
						pTmp1 += 4;
					}
				}
				else
				{
					if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
					{
						dp += dBitCnt;
						if( dp>=8 )
						{
							*pTmp1++ = dbyte; dbyte = *pTmp1; dp = dBitCnt;
						}
					}
					else if( dBitCnt==24 )
					{
						pTmp1 += 3;
					}
					else if( dBitCnt==32 )
					{
						pTmp1 += 4;
					}
				}
			}
		}

		pTmpD -= destWidth;
		pTmpS -= srcWidth;		
	}

	if( clridx )delete[] clridx;
	return TRUE;
}


BOOL OverlapBits(BYTE* pSrc, int sMemWid, int sBitCnt, RGBQUAD* srcClTbl, CRect& srcRect,
				   BYTE* pDest, int dMemWid, int dBitCnt, RGBQUAD* desClTbl, CRect& desRect,
				   BOOL bTransparent, COLORREF transColor,DWORD rgbMask)
{
	if( pSrc==NULL || pDest==NULL )return FALSE;

	if( (sBitCnt==1||sBitCnt==4||sBitCnt==8) && srcClTbl==NULL )return FALSE;
	if( (dBitCnt==1||dBitCnt==4||dBitCnt==8) && desClTbl==NULL )return FALSE;	

	int	readWidth= srcRect.Width();
	int readBytes= (readWidth*sBitCnt+7)/8;

	BYTE* pTmpD = pDest+(desRect.top)*dMemWid
		+ desRect.left*dBitCnt/8;

	BYTE* pTmpS = pSrc+(srcRect.top)*sMemWid
		+ srcRect.left*sBitCnt/8;	

	BYTE pixel[3]={0};

	BOOL bMask[3]={	((rgbMask&RGBMASK_RED)!=0), //red
					((rgbMask&RGBMASK_GREEN)!=0), //green
					((rgbMask&RGBMASK_BLUE)!=0) //blue 
	};

	//for speed
	if( sBitCnt==dBitCnt && !bTransparent && (bMask[0]&&bMask[1]&&bMask[2]) )
	{
		for( int i=srcRect.top; i<srcRect.bottom; i++)
		{
			memcpy(pTmpD,pTmpS,readBytes);
			pTmpD -= dMemWid;
			pTmpS -= sMemWid;
		}
		return TRUE;
	}

	//calculate a serial of matched color indexes
	int *clridx = NULL;
	int scn = (1<<(sBitCnt)), dcn = (1<<(dBitCnt));
	if( (sBitCnt==1||sBitCnt==4||sBitCnt==8 ) && (dBitCnt==1||dBitCnt==4||dBitCnt==8) )
	{
		clridx = new int[scn];
		if( !clridx )return FALSE;

		int dis = -1, min = -1;
		for( int i=0; i<scn; i++)
		{
			clridx[i] = FindNearestColorItem(desClTbl,dcn,
				RGB(srcClTbl[i].rgbRed,srcClTbl[i].rgbGreen,srcClTbl[i].rgbBlue));
		}
	}

	if( sBitCnt==1||sBitCnt==4||sBitCnt==8 )
	{
		transColor = FindNearestColorItem(desClTbl,dcn,transColor);
	}

	int nSInByte = 8/sBitCnt, nDInByte = 8/dBitCnt;	
	int	sidx = 0, didx = 0;
	int srest = 0, drest = 0;
	BYTE sBitMask, dBitMask, sbyte, dbyte;
	BYTE* pTmp1=NULL,*pTmp2=NULL;
	RGBQUAD* pCl = NULL;

	for( int i=srcRect.top; i<srcRect.bottom; i++)
	{
		pTmp1 =pTmpD;
		pTmp2 =pTmpS;

		int sp=0, dp=0;
		if( sBitCnt==1 || sBitCnt==4 || sBitCnt==8 )
		{
			dbyte = *pTmp1;
			for( int j=0; j<readBytes; j++ )
			{
				sbyte = *pTmp2++;
				sBitMask = 0xff;
				dBitMask = 0xff;
				sp = 0;
				for( ;sp<8; sp+=sBitCnt )
				{
					//取出sp到sp+sBitCnt中的位值
					sidx = ((sbyte>>sp)&(0xff>>(8-sBitCnt)));
					if( sidx!=(int)transColor )
					{			
						if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
						{
							sidx = clridx[sidx];

							//清除dp到dp+dBitCnt中的位值
							dbyte &= ((0xff<<(dp+dBitCnt))|(0xff>>(8-dp)));

							//将sidx填入到dp到dp+dBitCnt的位上
							dbyte |= (sidx<<dp);

							dp += dBitCnt;
							if( dp>=8 )
							{
								*pTmp1++ = dbyte; dbyte = *pTmp1; dp = 0;
							}
						}
						else if( dBitCnt==24 )
						{
							pCl = srcClTbl+sidx;
							CPYCOLOR_BYMASK(bMask,pTmp1,pCl);
							pTmp1 += 3;
						}
						else if( dBitCnt==32 )
						{
							pCl = srcClTbl+sidx;
							CPYCOLOR_BYMASK(bMask,pTmp1,pCl);
							pTmp1 += 4;
						}
					}
					else
					{
						if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
						{
							dp += dBitCnt;
							if( dp>=8 )
							{
								*pTmp1++ = dbyte; dbyte = *pTmp1; dp = 0;
							}
						}
						else if( dBitCnt==24 )pTmp1 += 3;
						else if( dBitCnt==32 )pTmp1 += 4;
					}

					sBitMask = (sBitMask>>sBitCnt);
					dBitMask = (dBitMask>>dBitCnt);	
				}
			}
		}
		else
		{
			for( int j=0; j<readWidth; j++ )
			{
				COLORREF clr;
				if( sBitCnt==24 )
				{
					clr = RGB(pTmp2[2],pTmp2[1],pTmp2[0]);
					pTmp2 += 3;
				}
				else if( sBitCnt==32 )
				{
 					clr = RGB(pTmp2[2],pTmp2[1],pTmp2[0]);
					pTmp2 += 4;
				}

				if( clr!=transColor )
				{				
					if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
					{
						sidx = FindNearestColorItem(desClTbl,scn,clr);
						//清除dp到dp+dBitCnt中的位值
						dbyte &= ((0xff<<(dp+dBitCnt))|(0xff>>(8-dp)));

						//将sidx填入到dp到dp+dBitCnt的位上
						dbyte |= (sidx<<dp);

						dp += dBitCnt;
						if( dp>=8 )
						{
							*pTmp1++ = dbyte; dbyte = *pTmp1; dp = 0;
						}
					}
					else if( dBitCnt==24 )
					{
						if( bMask[2] )pTmp1[0] = GetBValue(clr);
						if( bMask[1] )pTmp1[1] = GetGValue(clr);
						if( bMask[0] )pTmp1[2] = GetRValue(clr);
						pTmp1 += 3;
					}
					else if( dBitCnt==32 )
					{
						if( bMask[2] )pTmp1[0] = GetBValue(clr);
						if( bMask[1] )pTmp1[1] = GetGValue(clr);
						if( bMask[0] )pTmp1[2] = GetRValue(clr);
						pTmp1 += 4;
					}
				}
				else
				{
					if( dBitCnt==1 || dBitCnt==4 || dBitCnt==8 )
					{
						dp += dBitCnt;
						if( dp>=8 )
						{
							*pTmp1++ = dbyte; dbyte = *pTmp1; dp = 0;
						}
					}
					else if( dBitCnt==24 )pTmp1 += 3;
					else if( dBitCnt==32 )pTmp1 += 4;
				}
			}
		}

		pTmpD += dMemWid;
		pTmpS += sMemWid;		
	}

	if( clridx )delete[] clridx;
	return TRUE;
}


BOOL AdjustBitmap(HBITMAP hBmp, CRect rcAdjust, CImageAdjust *pAdjust, BOOL bUseBKClr,COLORREF bkClr, DWORD rgbMask)
{
	if( (pAdjust==NULL||pAdjust->bAdjustMap==FALSE) && 
		(rgbMask&RGBMASK_RED)!=0 && (rgbMask&RGBMASK_GREEN)!=0 && (rgbMask&RGBMASK_BLUE)!=0 )
		return TRUE;

	if( hBmp==NULL )return FALSE;

	DIBSECTION info;
	if( ::GetObject( hBmp, sizeof(DIBSECTION), &info)==0 )return FALSE;

	if( info.dsBm.bmBitsPixel!=24 )return FALSE;
	int bitCnt = info.dsBm.bmBitsPixel;

	int lineBytes= (info.dsBm.bmWidth*bitCnt+7)/8;
	lineBytes = ((lineBytes+3)&(~3));

	BYTE* pBits=(BYTE*)info.dsBm.bmBits+(info.dsBm.bmHeight-rcAdjust.bottom)*lineBytes
		+ rcAdjust.left*3;	
	
	if( pBits==NULL )return FALSE;

	BOOL bMask1 = ((rgbMask&RGBMASK_RED)!=0); //red
	BOOL bMask2 = ((rgbMask&RGBMASK_GREEN)!=0); //green
	BOOL bMask3 = ((rgbMask&RGBMASK_BLUE)!=0); //blue 

	bkClr = (bkClr&0xffffff);

	BYTE *b = pAdjust->pBAdjustMap, *g = pAdjust->pGAdjustMap, *r = pAdjust->pRAdjustMap;

	int nWid = rcAdjust.Width(), nHei = rcAdjust.Height(), nSkipBytes = lineBytes-(nWid*bitCnt+7)/8;

	if( pAdjust && pAdjust->bAdjustMap )
	{
		for( int i=0; i<nHei; i++)
		{
			for( int j=0; j<nWid; j++,pBits+=3)
			{
				if( bUseBKClr && (pBits[2]|(pBits[1]<<8)|(pBits[0]<<16))==bkClr )
					continue;
				if( bMask3 )pBits[0] = b[pBits[0]];
				else pBits[0] = 0;

				if( bMask2 )pBits[1] = g[pBits[1]];
				else pBits[1] = 0;

				if( bMask1 )pBits[2] = r[pBits[2]];
				else pBits[2] = 0;
			}

			pBits += nSkipBytes;
		}
	}
	else
	{
		for( int i=0; i<nHei; i++)
		{
			for( int j=0; j<nWid; j++,pBits+=3)
			{
				if( bUseBKClr && (pBits[2]|(pBits[1]<<8)|(pBits[0]<<16))==bkClr )
					continue;
				if( bMask3 );
				else pBits[0] = 0;
				
				if( bMask2 );
				else pBits[1] = 0;
				
				if( bMask1 );
				else pBits[2] = 0;
			}
			
			pBits += nSkipBytes;
		}
	}

	return TRUE;
}


BOOL SetBitmapBKColor(HBITMAP hBmp, CRect rcValid,COLORREF bkClr)
{
	if( hBmp==NULL )return FALSE;
	
	DIBSECTION info;
	if( ::GetObject( hBmp, sizeof(DIBSECTION), &info)==0 )return FALSE;

	rcValid = rcValid&CRect(0,0,info.dsBm.bmWidth,info.dsBm.bmHeight);
	
	if( info.dsBm.bmBitsPixel!=24 )return FALSE;
	int bitCnt = info.dsBm.bmBitsPixel;
	
	int lineBytes= (info.dsBm.bmWidth*bitCnt+7)/8;
	lineBytes = ((lineBytes+3)&(~3));
	
	BYTE* pBits = (BYTE*)info.dsBm.bmBits;
	BYTE* pLine = (BYTE*)info.dsBm.bmBits + lineBytes*(info.dsBm.bmHeight-1);
	
	if( pBits==NULL )return FALSE;

	bitCnt = (bitCnt>>3);

	BYTE r = GetRValue(bkClr), g = GetGValue(bkClr), b = GetBValue(bkClr);

	int x0, x1;
	for( int i=0; i<info.dsBm.bmHeight; i++, pLine-=lineBytes )
	{
		if( i>=rcValid.top && i<rcValid.bottom )
		{
			x0 = rcValid.left; x1 = rcValid.right;
		}
		else
		{
			x0 = 0; x1 = 0;
		}

		pBits = pLine;
		for( int j=0; j<info.dsBm.bmWidth && j<x0; j++, pBits+=bitCnt )
		{
			pBits[0] = b; pBits[1] = g; pBits[2] = r;
		}

		pBits = pLine + x1*bitCnt;
		for( j=x1; j<info.dsBm.bmWidth; j++, pBits+=bitCnt )
		{
			pBits[0] = b; pBits[1] = g; pBits[2] = r;
		}
	}
	return TRUE;
}

float SmartDivide(float x1, float x2)
{
	if( x1<0 || x2<0 )
	{
		return (x1/x2);
	}
	else if( x1==0 )return 0;
	else if( x2==0 )return (float)INT_MAX;

	static double test[] = { 0.75,0.5,0.375,0.333333,0.25,0.2,0.166667,0.142857,0.125 };
	static int ntest = sizeof(test)/sizeof(test[0]);

	if( x1>=x2 )
	{
		float tolerance = (x1/(x2-1)-x1/(x2+1))*0.5;
		if( x2==1 || x2==-1 || tolerance>0.5 || tolerance<-0.5 )tolerance = 0.5;
		float x = x1/x2;
		if( x-int(x)<tolerance || x-int(x)>1-tolerance  )
			return (float)(int(x+tolerance));
		else 
		{
			float dx = x-int(x);
			tolerance *= 1.01;
			for( int i=0; i<ntest; i++)
			{
				if( tolerance<test[i] && fabs(dx-test[i])<tolerance )
					return (int(x)+test[i]);
			}
			return (x1/x2);
		}
	}
	else
	{
		float tolerance = (x2/(x1-1)-x2/(x1+1))*0.5;
		if( x1==1 || x1==-1 || tolerance>0.5 || tolerance<-0.5 )tolerance = 0.5;
		float x = x2/x1;
		if( x-int(x)<tolerance || x-int(x)>1-tolerance  )
			return (float)1/(int(x+tolerance));
		else 
		{
			float dx = x-int(x);
			tolerance *= 1.01;
			for( int i=0; i<ntest; i++)
			{
				if( tolerance<test[i] && fabs(dx-test[i])<tolerance )
					return 1.0/(int(x)+test[i]);
			}
			return (x1/x2);
		}
	}
}
