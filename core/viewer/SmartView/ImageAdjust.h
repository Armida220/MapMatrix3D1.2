// ImageAdjust.h: interface for the CImageAdjust class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XVIMAGEADJUST_H__3BF2A7DF_1DF4_483D_BD76_3F9F842F67B8__INCLUDED_)
#define AFX_XVIMAGEADJUST_H__3BF2A7DF_1DF4_483D_BD76_3F9F842F67B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "SmartViewDef.h"

class EXPORT_SMARTVIEW CImageAdjust
{
public:
	CImageAdjust();
	~CImageAdjust();
	
	void StatHistogramInfo(HBITMAP hBmp, double k=0.005);
	void CalcAdjustMap();
	
	int		nBrightness;
	int		nContrast;
	BOOL	bHistogram;
	BYTE	nRRange[2];
	BYTE	nGRange[2];
	BYTE	nBRange[2];
	
	BOOL	bAdjustMap;
	BOOL	bAdjustHistogram;
	BYTE	pRAdjustMap[256];
	BYTE	pGAdjustMap[256];
	BYTE	pBAdjustMap[256];
};

#endif // !defined(AFX_XVIMAGEADJUST_H__3BF2A7DF_1DF4_483D_BD76_3F9F842F67B8__INCLUDED_)
