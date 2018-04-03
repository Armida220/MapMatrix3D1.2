#include "StdAfx.h"
#include "res/resource.h"
#include "SmartViewBaseType.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL LoadMyString(CString &str, UINT uID);


#define FLAG_BOLD					0x01
#define FLAG_UNDERLINE				0x02


MyNameSpaceBegin



TEXT_SETTINGS0::TEXT_SETTINGS0()
{
	fHeight = 2.0f;
	fWidScale = 1.0f;
	fCharIntervalScale = 0.0f;
	fLineSpacingScale = 1.0f;
	fTextAngle = 0.0f;
	fCharAngle = 0.0f;
	nPlaceType = singPt;
	nAlignment = TAH_LEFT|TAV_BOTTOM;
	nInclineType = SHRUGN;
	fInclineAngle = 0.0f;
	nOtherFlag = 0;
	CString font;
	LoadMyString(font,IDS_TEXT_DEFAULTFONT);
	_tcscpy(strFontName,font);
}


BOOL TEXT_SETTINGS0::IsBold()const
{
	return ((nOtherFlag&FLAG_BOLD)!=0);
}
BOOL TEXT_SETTINGS0::IsUnderline()const
{
	return ((nOtherFlag&FLAG_UNDERLINE)!=0);
}
void TEXT_SETTINGS0::SetBold(BOOL bValue)
{
	if( bValue )
	{
		nOtherFlag |= FLAG_BOLD;
	}
	else
	{
		nOtherFlag = (nOtherFlag&(~FLAG_BOLD));
	}
}

void TEXT_SETTINGS0::SetUnderline(BOOL bValue)
{
	if( bValue )
	{
		nOtherFlag |= FLAG_UNDERLINE;
	}
	else
	{
		nOtherFlag = (nOtherFlag&(~FLAG_UNDERLINE));
	}
}

tagTextSettings::tagTextSettings()
{
	fHeight = 4.0f;
	fWidScale = 1.0f;
	fCharIntervalScale = 0.0f;
	fLineSpacingScale = 1.0f;
	fTextAngle = 0.0f;
	fCharAngle = 0.0f;
	nAlignment = TAH_LEFT|TAV_BOTTOM;
	nInclineType = SHRUGN;
	fInclineAngle = 0.0f;
	nOtherFlag = 0;
	CString font;
	LoadMyString(font,IDS_TEXT_DEFAULTFONT);
	_tcscpy(tcFaceName,font);
}



BOOL tagTextSettings::IsBold()const
{
	return ((nOtherFlag&FLAG_BOLD)!=0);
}
BOOL tagTextSettings::IsUnderline()const
{
	return ((nOtherFlag&FLAG_UNDERLINE)!=0);
}
void tagTextSettings::SetBold(BOOL bValue)
{
	if( bValue )
	{
		nOtherFlag |= FLAG_BOLD;
	}
	else
	{
		nOtherFlag = (nOtherFlag&(~FLAG_BOLD));
	}
}

void tagTextSettings::SetUnderline(BOOL bValue)
{
	if( bValue )
	{
		nOtherFlag |= FLAG_UNDERLINE;
	}
	else
	{
		nOtherFlag = (nOtherFlag&(~FLAG_UNDERLINE));
	}
}


void GrVertexAttrList::CutSpace()
{
	if( nuse<=0 )
	{
		if( pts )delete[] pts;
		pts = NULL;
		nuse = 0;
		nlen = 0;
	}
	else if( nlen>nuse )
	{
		GrVertexAttr *pts2 = new GrVertexAttr[nuse];
		if( pts2 )
		{
			memcpy(pts2,pts,sizeof(GrVertexAttr)*nuse);
			delete[] pts;
			pts = pts2;
			nlen = nuse;
		}
	}
}


void GrVertexAttrList::CopyFrom(const GrVertexAttrList *list)
{
	if( pts && nlen>=list->nlen )
	{
		memcpy(pts,list->pts,sizeof(GrVertexAttr)*list->nuse);
		nuse = list->nuse;
	}
	else
	{
		GrVertexAttr *pts2 = new GrVertexAttr[list->nlen];
		if( pts2 )
		{
			memcpy(pts2,list->pts,sizeof(GrVertexAttr)*list->nuse);
			if( pts )delete[] pts;
			pts = pts2;
			nlen = list->nlen;
			nuse = list->nuse;
		}		
	}
}

MyNameSpaceEnd

