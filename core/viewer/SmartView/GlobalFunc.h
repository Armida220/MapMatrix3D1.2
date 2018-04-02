#ifndef INCLUDE_GLOBAL_FILE
#define INCLUDE_GLOBAL_FILE

#include "iseeirw.h"
#include "SmartViewDef.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	RGBMASK_RED					0x00000001
#define	RGBMASK_GREEN				0x00000002
#define	RGBMASK_BLUE				0x00000004

float SmartDivide(float x1, float y1);

//for test
void EXPORT_SMARTVIEW WriteToFile(HBITMAP hBmp);

//create a DIB bitmap with 24 bits
//[in] w, h, the bitmap size;
//[in] bSetZero, flag if zero this bitmap.
HBITMAP EXPORT_SMARTVIEW Create24BitDIB(int w, int h, BOOL bSetZero=TRUE,COLORREF clr=0);


// overlap a bitmap onto the other bitmap;
// [in] hSrc, the source bitmap;
// [in] srcClTbl, source color table;
// [in] srcRect, the rectangle of data to be overlapped;
// [in] hDest, the overlapped bitmap;
// [in] desClTbl, target color table;
// [in] desRect, the rectangle of target bitmap;
// [in] bTransparent, TRUE if pixels with color of 'transColor' will not be copied
// [in] transColor, color of pixels which will not be copied;
// [in] rgbMask, flag which color bits will be copied;
BOOL EXPORT_SMARTVIEW OverlapBitmap(HBITMAP hSrc, RGBQUAD* srcClTbl, CRect& srcRect,
				   HBITMAP hDest, RGBQUAD* desClTbl, CRect& desRect,
				   BOOL bTransparent=FALSE, COLORREF transColor=0,
				   DWORD rgbMask=RGBMASK_RED|RGBMASK_GREEN|RGBMASK_BLUE);


// [in] brightness, the brightness value when hSrc will be adjusted;
// [in] contrast, the contrast value when hSrc will be adjusted;
// [in] rgbMask, flag which color bits will be copied;
class CImageAdjust;
BOOL EXPORT_SMARTVIEW AdjustBitmap(HBITMAP hBmp, CRect rcAdjust, CImageAdjust *pAdjust, BOOL bUseBKClr,COLORREF bkClr,
				  DWORD rgbMask=RGBMASK_RED|RGBMASK_GREEN|RGBMASK_BLUE);

BOOL EXPORT_SMARTVIEW OverlapBits(BYTE* pSrc, int sMemWid, int sBitCnt, RGBQUAD* srcClTbl, CRect& srcRect,
				   BYTE* pDest, int dMemWid, int dBitCnt, RGBQUAD* desClTbl, CRect& desRect,
				   BOOL bTransparent=FALSE, COLORREF transColor=0,
				   DWORD rgbMask=RGBMASK_RED|RGBMASK_GREEN|RGBMASK_BLUE);

BOOL EXPORT_SMARTVIEW SetBitmapBKColor(HBITMAP hBmp, CRect rcValid,COLORREF bkClr);


#endif
