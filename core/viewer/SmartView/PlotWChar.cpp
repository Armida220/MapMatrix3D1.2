// PlotWChar.cpp: implementation of the CPlotWChar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlotWChar.h"
#include <string>
#include "SmartViewFunctions.h"
#include "glew.h"
#include "DxFont.h"
#include "GlobalFunc.h"
#include "DrawLineDC.h"

#define PC_SIZE				100
extern BYTE clrTable_CAD[];

MyNameSpaceBegin


extern void  PolyPath(GraphicsPath* pPath, 
					  CONST LPPOINT lppt, 
					  CONST LPBYTE lpbTypes,
		int cCount );

extern void  PolyPath_compress(GraphicsPath* pPath, 
					  CONST LPPOINT lppt, 
					  CONST LPBYTE lpbTypes,
		int cCount );

CMemDc g_Memdc;
CFontLib g_Fontlib;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetCharInfo::CGetCharInfo()
{
	m_hFont = NULL;
}

CGetCharInfo:: ~CGetCharInfo()
{
	if(m_hFont)
		DeleteObject(m_hFont);
}
	
void CGetCharInfo::SetSettings(const TextSettings *settings)
{
	ASSERT(settings!=NULL);
	memcpy(&m_settings,settings,sizeof(TextSettings));
	if(m_hFont) ::DeleteObject(m_hFont);
	m_hFont = ::CreateFont(
		m_settings.fHeight,				   // nHeight
		0,                   // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		(settings->IsBold()?FW_BOLD:0),                 // nWeight
		FALSE,           // bItalic
		settings->IsUnderline(),        // bUnderline
		FALSE,        // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH,  // nPitchAndFamily
		m_settings.tcFaceName);	
}

void CGetCharInfo::SetWChar(WCHAR wchar)
{
	m_wChar = wchar;
}

BOOL CGetCharInfo::Plot(CArray<GrVertex,GrVertex> *pts, double *nextCharPosDis, double *Width, double *Height)
{
	if(m_hFont==NULL)
		return FALSE;
	
	GraphicsPath charpath(FillModeWinding);	
	HDC hdc = g_Memdc.GetHdc();
	HFONT oldFont = (HFONT)::SelectObject(hdc,m_hFont);
	TEXTMETRICW textMetric;
	::GetTextMetricsW(hdc,&textMetric);
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	if (nextCharPosDis)
	{
		ABC abc;
		::GetCharABCWidthsW(hdc,m_wChar,m_wChar,&abc);
		*nextCharPosDis = ceil((abc.abcA+abc.abcB+abc.abcC)*m_settings.fWidScale+tmAveCharWidth*m_settings.fCharIntervalScale);
	}
	if (pts==NULL&&Width==NULL&&Height==NULL)
	{
		::SelectObject(hdc,oldFont);
		return TRUE;
	}
	Matrix matrix, matrix0;
	//先计算矩阵变换	
	//旋转变换
	matrix.Scale(m_settings.fWidScale,-1.0);
	//耸肩变换 
	double angle = m_settings.fInclineAngle*PI/180;
	if (m_settings.nInclineType==SHRUGL)
	{
		matrix.Shear(-tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
	}
	else if (m_settings.nInclineType==SHRUGR)
	{
		matrix.Shear(tan(angle),0.0,MatrixOrderAppend);
	}
	else if (m_settings.nInclineType==SHRUGU)
	{
		matrix.Shear(0.0,tan(angle),MatrixOrderAppend);
	}
	else if (m_settings.nInclineType==SHRUGD)
	{
		matrix.Shear(0.0,-tan(angle),MatrixOrderAppend);
	}
	//旋转变换
	matrix.Rotate(m_settings.fCharAngle,MatrixOrderAppend);

	wchar_t wChar[2] = {0};
	int nPrevMode = ::SetBkMode(hdc, TRANSPARENT);
	wChar[0] = m_wChar;
	::BeginPath(hdc);
	::TextOutW(hdc, 0, 0, wChar, 1);
	::EndPath(hdc);
	
	int nNumPts = ::GetPath(hdc, NULL, NULL, 0);
	if (nNumPts == 0)
	{	
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);
		return FALSE;
	}
	
	// Allocate memory to hold points and stroke types from
	// the path.
	LPPOINT lpPoints = new POINT[nNumPts];
	if (lpPoints == NULL)
	{	
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);		
		return FALSE;
	}
	LPBYTE lpTypes = new BYTE[nNumPts];
	if (lpTypes == NULL)
	{
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);
		delete []lpPoints;
		return FALSE;
	}
	
	// Now that we have the memory, really get the path data.
	nNumPts = GetPath(hdc, lpPoints, lpTypes, nNumPts);
	
	// If it worked, draw the lines. Win95 and Win98 don't support
	// the PolyDraw API, so we use our own member function to do
	// similar work. If you're targeting only Windows NT, you can
	// use the PolyDraw() API and avoid the COutlineView::PolyDraw()
	// member function.
	
	if (nNumPts != -1)
	{	
		PolyPath(&charpath, lpPoints, lpTypes, nNumPts);		
		charpath.Transform(&matrix);				
	}
	else
	{
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);
		delete []lpPoints;
		delete []lpTypes;
		return FALSE;
	}
	delete [] lpPoints;
	delete [] lpTypes;
	::SelectObject(hdc,oldFont);
	::SetBkMode(hdc, nPrevMode);
	
	
	Rect rect;
	charpath.GetBounds(&rect, NULL, NULL);
	if (Width)
	{
		*Width = rect.Width;
	}
	if (Height)
	{
		*Height = rect.Height;
	}
	if (pts==NULL)
	{
		return TRUE;
	}
	Matrix mat;
	float orgx, orgy;
	long yStart = 0;
	long yEnd = -textMetric.tmHeight;

	float x=0,y=0;
	switch(m_settings.nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);			
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);	
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);	
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);	
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);	
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);			
			charpath.Transform(&mat);
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yEnd/*-lineSpacing*//*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);				
			charpath.Transform(&mat);
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);			
			charpath.Transform(&mat);
			break;
	}
	
	if (1)
	{
		GrVertex pt,start;
		pt.z = 0;
		charpath.Flatten(NULL);
		PathData pathData;
		charpath.GetPathData(&pathData);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			pt.x = pathData.Points[j].X;
			pt.y = pathData.Points[j].Y;
			if (pathData.Types[j]==PathPointTypeStart )
			{
				pt.code = GRBUFFER_PTCODE_MOVETO;
				start = pt;				
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
			}
			else if(pathData.Types[j]==(PathPointTypeCloseSubpath|PathPointTypeLine))
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
				pts->Add(pt);
				pt = start;
			}
			pts->Add(pt);			
		}	
	}
	return TRUE;
}

//------------------------
CGetTextInfo::CGetTextInfo()
{
	m_hFont = NULL;
}

CGetTextInfo:: ~CGetTextInfo()
{
	if(m_hFont)
		DeleteObject(m_hFont);
}

void CGetTextInfo::SetSettings(const TextSettings *settings, BOOL GetEnv)
{
	ASSERT(settings!=NULL);
	memcpy(&m_settings,settings,sizeof(TextSettings));
	if(m_hFont) ::DeleteObject(m_hFont);
	int FontHei = m_settings.fHeight;
	if (GetEnv)
	{
		FontHei = PC_SIZE;
	}
	m_hFont = ::CreateFont(
		FontHei,				   // nHeight
		0,                   // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		(settings->IsBold()?FW_BOLD:0),                 // nWeight
		FALSE,           // bItalic
		settings->IsUnderline(),        // bUnderline
		FALSE,        // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH,  // nPitchAndFamily
		m_settings.tcFaceName);	

}

void CGetTextInfo::SetString(LPCTSTR pStr)
{
	m_strText = CString(pStr);
}


//(0,0)点为字符的定位点
BOOL CGetTextInfo::Plot(CArray<GrVertex,GrVertex> *pts/*,double ex[4],double ey[4]*/)
{
	if (m_strText.IsEmpty())
	{
		return FALSE;
	}

	HDC hdc = g_Memdc.GetHdc();
	HFONT oldFont = (HFONT)::SelectObject(hdc,m_hFont);
	TEXTMETRICW textMetric;
	::GetTextMetricsW(hdc,&textMetric);
	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*m_settings.fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*m_settings.fLineSpacingScale);//行步距
	
	GraphicsPath textpath(FillModeWinding),rowtextpath(FillModeWinding), charpath(FillModeWinding);
	//分行做处理（\n）做隔断符
	
#ifdef _UNICODE
	size_t len  = wcslen(LPCTSTR(m_strText));	
#else
	std::wstring wstr0;
	ConvertCharToWstring(LPCTSTR(m_strText),wstr0,CP_ACP);
	size_t len  = wcslen(wstr0.c_str());	
#endif
	int xStart = 0, yStart = 0;	
	wchar_t *str = new wchar_t[len+1];	
	if (!str) 
	{
		::SelectObject(hdc,oldFont);
		return FALSE;
	}
	Matrix matrix, matrix0;
	//先计算矩阵变换	
	//旋转变换
	matrix.Scale(m_settings.fWidScale,-1.0);
	//耸肩变换 
	double angle = m_settings.fInclineAngle*PI/180;
	if (m_settings.nInclineType==SHRUGL)
	{
		matrix.Shear(-tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
	}
	else if (m_settings.nInclineType==SHRUGR)
	{
		matrix.Shear(tan(angle),0.0,MatrixOrderAppend);
	}
	else if (m_settings.nInclineType==SHRUGU)
	{
		matrix.Shear(0.0,tan(angle),MatrixOrderAppend);
	}
	else if (m_settings.nInclineType==SHRUGD)
	{
		matrix.Shear(0.0,-tan(angle),MatrixOrderAppend);
	}
	//缩放变换，实现字宽的变化
	matrix.Rotate(m_settings.fCharAngle,MatrixOrderAppend);

#ifdef _UNICODE
	wcscpy(str,LPCTSTR(m_strText));	
#else
	wcscpy(str,wstr0.c_str());		
#endif
	
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
//	memset(wChar,0,sizeof(wChar));
	int nPrevMode = ::SetBkMode(hdc, TRANSPARENT);
	Rect firstRect;
	BOOL bFirst = TRUE;
	while( token != NULL )
	{
		/* While there are tokens in "str" */
		int strlength = wcslen(token);
		rowtextpath.Reset();
		for (int i=0;i<strlength;i++)
		{
			wChar[0] = token[i];
			::BeginPath(hdc);
			::TextOutW(hdc, 0, 0, wChar, 1);
			::EndPath(hdc);

			int nNumPts = ::GetPath(hdc, NULL, NULL, 0);
			if (nNumPts == 0)
			{
				::SelectObject(hdc,oldFont);
				delete []str;
				return FALSE;
			}
			
			// Allocate memory to hold points and stroke types from
			// the path.
			LPPOINT lpPoints = new POINT[nNumPts];
			if (lpPoints == NULL)
			{
				::SelectObject(hdc,oldFont);
				delete []str;
				return FALSE;
			}
			LPBYTE lpTypes = new BYTE[nNumPts];
			if (lpTypes == NULL)
			{
				::SelectObject(hdc,oldFont);
				delete []str;
				delete [] lpPoints;
				return FALSE; 
			}
			
			// Now that we have the memory, really get the path data.
			nNumPts = GetPath(hdc, lpPoints, lpTypes, nNumPts);
			
			// If it worked, draw the lines. Win95 and Win98 don't support
			// the PolyDraw API, so we use our own member function to do
			// similar work. If you're targeting only Windows NT, you can
			// use the PolyDraw() API and avoid the COutlineView::PolyDraw()
			// member function.
			
			if (nNumPts != -1)
			{
				charpath.Reset();
				PolyPath(&charpath, lpPoints, lpTypes, nNumPts);
				matrix0.Reset();
				matrix0.Translate(xStart,yStart);
				matrix0.Multiply(&matrix/*,MatrixOrderAppend*/);
				charpath.Transform(&matrix0);			
				rowtextpath.AddPath(&charpath,FALSE);				
			}
			else
			{
				::SelectObject(hdc,oldFont);
				delete []str;
				delete []lpPoints;
				delete []lpTypes;
				return FALSE;

			}
			ABC abc;
			::GetCharABCWidthsW(hdc,token[i],token[i],&abc);
			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*m_settings.fWidScale+CharInterval);
			// Release the memory we used
			delete [] lpPoints;
			delete [] lpTypes;
		}
		if (bFirst)
		{
			rowtextpath.GetBounds(&firstRect,NULL,NULL);
			bFirst = FALSE;
		}		
		else
		{
			Rect rowRect;		
		//更新yStart
			if (m_settings.nAlignment&TAH_LEFT)
			{
				;
			}
			else if (m_settings.nAlignment&TAH_MID)
			{
				rowtextpath.GetBounds(&rowRect,NULL,NULL);
				Matrix mat0;
				mat0.Translate(ceil((firstRect.GetLeft()+firstRect.GetRight()-rowRect.GetLeft()-rowRect.GetRight())/2.0),0.0);
				rowtextpath.Transform(&mat0);
			}
			else
			{
				rowtextpath.GetBounds(&rowRect,NULL,NULL);
				Matrix mat0;
				mat0.Translate(ceil((double)(firstRect.GetRight()-rowRect.GetRight())),0.0);
				rowtextpath.Transform(&mat0);
			}
		}
		textpath.AddPath(&rowtextpath,FALSE);
		xStart = 0;
		yStart += LineStep;
		/* Get next token: */
		token = wcstok( NULL, L"\n" );
	}
	delete []str;
	::SelectObject(hdc,oldFont);
	::SetBkMode(hdc, nPrevMode);

	yStart -= textMetric.tmExternalLeading*m_settings.fLineSpacingScale;

//对齐方式处理
	// Get the path's bounding rectangle.
	long yEnd = - (yStart);
	yStart = 0;
	
	Matrix mat;
	RectF rect;
	textpath.GetBounds(&rect, NULL, NULL);

	float orgx, orgy;	
	float x=0,y=0;
	switch(m_settings.nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));						
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
	}
// 	if (ex!=NULL&&ey!=NULL)
// 	{
// 		PointF ptf[4];
// 		ptf[3].X = ptf[0].X = rect.GetLeft();
// 		ptf[1].Y = ptf[0].Y = rect.GetBottom();
// 		ptf[2].X = ptf[1].X = rect.GetRight();
// 		ptf[2].Y = ptf[3].Y = rect.GetTop();
// 		mat.TransformPoints(ptf,4);
// 		ex[0] = ptf[0].X;ey[0] = ptf[0].Y;
// 		ex[1] = ptf[1].X;ey[1] = ptf[1].Y;
// 		ex[2] = ptf[2].X;ey[2] = ptf[2].Y;
// 		ex[3] = ptf[3].X;ey[3] = ptf[3].Y;
// 	}	
	if (pts)
	{
		textpath.Transform(&mat);
		GrVertex pt,start;
		pt.z = 0;
		textpath.Flatten(NULL);
		PathData pathData;
		textpath.GetPathData(&pathData);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			pt.x = pathData.Points[j].X;
			pt.y = pathData.Points[j].Y;
			if (pathData.Types[j]==PathPointTypeStart )
			{
				pt.code = GRBUFFER_PTCODE_MOVETO;
				start = pt;				
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
			}
			else if(pathData.Types[j]==(PathPointTypeCloseSubpath|PathPointTypeLine))
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
				pts->Add(pt);
				pt = start;
			}
			pts->Add(pt);			
		}	
	}
	return TRUE;
}

BOOL CGetTextInfo::GetEnvlope(double ex[4],double ey[4])
{
	if (m_strText.IsEmpty())
	{
		return FALSE;
	}
	HDC hdc = g_Memdc.GetHdc();
	HFONT oldFont = (HFONT)::SelectObject(hdc,m_hFont);
	TEXTMETRICW textMetric;
	::GetTextMetricsW(hdc,&textMetric);
	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*m_settings.fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*m_settings.fLineSpacingScale);//行步距
	GraphicsPath textpath(FillModeWinding),rowtextpath(FillModeWinding), charpath(FillModeWinding);
#ifdef _UNICODE
	size_t len  = wcslen(LPCTSTR(m_strText));	
#else
	std::wstring wstr0;
	ConvertCharToWstring(LPCTSTR(m_strText),wstr0,CP_ACP);
	size_t len  = wcslen(wstr0.c_str());	
#endif
	int xStart = 0, yStart = 0;	
	wchar_t *str = new wchar_t[len+1];	
	if (!str) 
	{
		::SelectObject(hdc,oldFont);
		return FALSE;
	}
	Matrix matrix, matrix0;
	matrix.Scale(m_settings.fWidScale,-1.0);
	double angle = m_settings.fInclineAngle*PI/180;
	if (m_settings.nInclineType==SHRUGL)
	{
		matrix.Shear(-tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
	}
	else if (m_settings.nInclineType==SHRUGR)
	{
		matrix.Shear(tan(angle),0.0,MatrixOrderAppend);
	}
	else if (m_settings.nInclineType==SHRUGU)
	{
		matrix.Shear(0.0,tan(angle),MatrixOrderAppend);
	}
	else if (m_settings.nInclineType==SHRUGD)
	{
		matrix.Shear(0.0,-tan(angle),MatrixOrderAppend);
	}
	matrix.Rotate(m_settings.fCharAngle,MatrixOrderAppend);

#ifdef _UNICODE
	wcscpy(str,LPCTSTR(m_strText));	
#else
	wcscpy(str,wstr0.c_str());		
#endif
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
	int nPrevMode = ::SetBkMode(hdc, TRANSPARENT);
	Rect firstRect;
	BOOL bFirst = TRUE;
	while( token != NULL )
	{
		int strlength = wcslen(token);
		rowtextpath.Reset();
		for (int i=0;i<strlength;i++)
		{
			wChar[0] = token[i];
			::BeginPath(hdc);
			::TextOutW(hdc, 0, 0, wChar, 1);
			::EndPath(hdc);
			int nNumPts = ::GetPath(hdc, NULL, NULL, 0);
			if (nNumPts == 0)
			{
// 				::SelectObject(hdc,oldFont);
// 				delete []str;
				continue;
			}
			LPPOINT lpPoints = new POINT[nNumPts];
			if (lpPoints == NULL)
			{
				::SelectObject(hdc,oldFont);
				delete []str;
				return FALSE;
			}
			LPBYTE lpTypes = new BYTE[nNumPts];
			if (lpTypes == NULL)
			{
				::SelectObject(hdc,oldFont);
				delete []str;
				delete [] lpPoints;
				return FALSE; 
			}
			nNumPts = GetPath(hdc, lpPoints, lpTypes, nNumPts);
			if (nNumPts != -1)
			{
				charpath.Reset();
				PolyPath(&charpath, lpPoints, lpTypes, nNumPts);
				matrix0.Reset();
				matrix0.Translate(xStart,yStart);
				matrix0.Multiply(&matrix/*,MatrixOrderAppend*/);
				charpath.Transform(&matrix0);			
				rowtextpath.AddPath(&charpath,FALSE);				
			}
			else
			{
				::SelectObject(hdc,oldFont);
				delete []str;
				delete []lpPoints;
				delete []lpTypes;
				return FALSE;
			}
			ABC abc;
			::GetCharABCWidthsW(hdc,token[i],token[i],&abc);
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*m_settings.fWidScale+CharInterval);
			delete [] lpPoints;
			delete [] lpTypes;
		}
		if (bFirst)
		{
			rowtextpath.GetBounds(&firstRect,NULL,NULL);
			bFirst = FALSE;
		}		
		else
		{
			Rect rowRect;		
			if (m_settings.nAlignment&TAH_LEFT)
			{
				;
			}
			else if (m_settings.nAlignment&TAH_MID)
			{
				rowtextpath.GetBounds(&rowRect,NULL,NULL);
				Matrix mat0;
				mat0.Translate(ceil((firstRect.GetLeft()+firstRect.GetRight()-rowRect.GetLeft()-rowRect.GetRight())/2.0),0.0);
				rowtextpath.Transform(&mat0);
			}
			else
			{
				rowtextpath.GetBounds(&rowRect,NULL,NULL);
				Matrix mat0;
				mat0.Translate(ceil((double)(firstRect.GetRight()-rowRect.GetRight())),0.0);
				rowtextpath.Transform(&mat0);
			}
		}
		textpath.AddPath(&rowtextpath,FALSE);
		xStart = 0;
		yStart += LineStep;
		token = wcstok( NULL, L"\n" );
	}
	delete []str;
	::SelectObject(hdc,oldFont);
	::SetBkMode(hdc, nPrevMode);
	long yEnd = - (yStart);
	yStart = 0;
	Matrix mat;
	RectF rect;
	textpath.GetBounds(&rect, NULL, NULL);

	float orgx, orgy;	
	float x=0,y=0;
	switch(m_settings.nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));						
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(m_settings.fTextAngle,PointF(orgx,orgy));
			break;
	}
	if (ex!=NULL&&ey!=NULL)
	{
		PointF ptf[4];
		ptf[3].X = ptf[0].X = rect.GetLeft();
		ptf[1].Y = ptf[0].Y = rect.GetBottom();
		ptf[2].X = ptf[1].X = rect.GetRight();
		ptf[2].Y = ptf[3].Y = rect.GetTop();
		mat.TransformPoints(ptf,4);
		ex[0] = ptf[0].X*m_settings.fHeight/PC_SIZE;ey[0] = ptf[0].Y*m_settings.fHeight/PC_SIZE;
		ex[1] = ptf[1].X*m_settings.fHeight/PC_SIZE;ey[1] = ptf[1].Y*m_settings.fHeight/PC_SIZE;
		ex[2] = ptf[2].X*m_settings.fHeight/PC_SIZE;ey[2] = ptf[2].Y*m_settings.fHeight/PC_SIZE;
		ex[3] = ptf[3].X*m_settings.fHeight/PC_SIZE;ey[3] = ptf[3].Y*m_settings.fHeight/PC_SIZE;
	}		
	return TRUE;
}

// CFontLib

CFontLib::CFontLib()
{
	m_pCurFont = NULL;
	m_hdc = NULL;

	m_pBmpInfo = NULL;

	m_pDrawGraphics = NULL;

	m_hDrawDC = NULL;

	m_bUseTextBmpCache = TRUE;
}


CFontLib::~CFontLib()
{
	Clear();
}


void CFontLib::Clear()
{
	for( int i=m_arrItems.GetSize()-1; i>=0; i--)
	{
		delete m_arrItems[i];
	}
	m_arrItems.RemoveAll();

	if( m_pBmpInfo )
	{
		delete[] (BYTE*)m_pBmpInfo;
	}

	if( m_pDrawGraphics )
		delete m_pDrawGraphics;

	m_pDrawGraphics = NULL;
	m_hDrawDC = NULL;

	if( m_hdc )
	{
		::DeleteDC(m_hdc);
	}
	m_pCurFont = NULL;
	m_hdc = NULL;
	
	m_pBmpInfo = NULL;
}

void CFontLib::ClearDrawGraphics()
{
	if( m_pDrawGraphics )
		delete m_pDrawGraphics;
	
	m_pDrawGraphics = NULL;
	m_hDrawDC = NULL;
}

FontLibItem* CFontLib::SetSettings(const TextSettings *settings)
{
	FontLibItem *pFont = FindFont(settings);
	if( !pFont )
	{
		pFont = CreateFont(settings);
	}

	if( !pFont )
		return NULL;

	m_pCurFont = pFont;

	return m_pCurFont;
}


CharItem *CFontLib::GetCharItem(int ch)
{
	if( !m_pCurFont )
		return NULL;

	CharItem *pChar = m_pCurFont->FindItem(ch);
	if( !pChar )
	{
		pChar = CreateChar(m_pCurFont,ch);
	}

	return pChar;
}


BOOL CFontLib::IsSame(const TextSettings *s1, const TextSettings *s2)
{
	if( (int)s1->fHeight==(int)s2->fHeight && _tcsicmp(s1->tcFaceName,s2->tcFaceName)==0 &&
		s1->fWidScale==s2->fWidScale && s1->fCharIntervalScale==s2->fCharIntervalScale &&
		s1->fLineSpacingScale==s2->fLineSpacingScale && s1->fTextAngle==s2->fTextAngle &&
		s1->fCharAngle==s2->fCharAngle && s1->nAlignment==s2->nAlignment &&
		s1->nInclineType==s2->nInclineType && s1->fInclineAngle==s2->fInclineAngle &&
		s1->nOtherFlag==s2->nOtherFlag )
		return TRUE;

	return FALSE;
}

FontLibItem *CFontLib::FindFont(const TextSettings *settings)
{
	for( int i=m_arrItems.GetSize()-1; i>=0; i--)
	{
		FontLibItem *pItem = m_arrItems[i];
		if( IsSame(settings,&pItem->fontSetting) )
		{
			m_arrItems.RemoveAt(i);
			m_arrItems.Add(pItem);
			
			return pItem;
		}
	}
	return NULL;	
}


FontLibItem *CFontLib::CreateFont(const TextSettings *settings)
{
	float fHei = settings->fHeight;
	if( fHei<2 )
		fHei = 2;

	HFONT hFont = ::CreateFont(
		fHei,				   // nHeight
		0,                   // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		(settings->IsBold()?FW_BOLD:0),                 // nWeight
		FALSE,           // bItalic
		settings->IsUnderline(),        // bUnderline
		FALSE,        // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH,  // nPitchAndFamily
		settings->tcFaceName);	

	if( hFont==NULL )
		return NULL;
	
	FontLibItem *pItem = new FontLibItem();
	pItem->hFont = hFont;
	pItem->fontSetting = *settings;

	HDC hdc = GetMemDC();
	HFONT oldFont = (HFONT)::SelectObject(hdc,hFont);
	::GetTextMetricsW(hdc,&pItem->textMetric);
	::SelectObject(hdc,oldFont);

	pItem->textMetric.tmHeight = pItem->textMetric.tmHeight*settings->fHeight/fHei;
	pItem->textMetric.tmAscent = pItem->textMetric.tmAscent*settings->fHeight/fHei;
	pItem->textMetric.tmDescent = pItem->textMetric.tmDescent*settings->fHeight/fHei;
	pItem->textMetric.tmInternalLeading = pItem->textMetric.tmInternalLeading*settings->fHeight/fHei;
	pItem->textMetric.tmExternalLeading = pItem->textMetric.tmExternalLeading*settings->fHeight/fHei;
	pItem->textMetric.tmAveCharWidth = pItem->textMetric.tmAveCharWidth*settings->fHeight/fHei;
	pItem->textMetric.tmMaxCharWidth = pItem->textMetric.tmMaxCharWidth*settings->fHeight/fHei;
	pItem->textMetric.tmWeight = pItem->textMetric.tmWeight*settings->fHeight/fHei;
	pItem->textMetric.tmOverhang = pItem->textMetric.tmOverhang*settings->fHeight/fHei;

	m_arrItems.Add(pItem);
	if( m_arrItems.GetSize()>100 )
	{
		FontLibItem *pItem2 = m_arrItems.GetAt(0);
		m_arrItems.RemoveAt(0);
		delete pItem2;
	}

	return pItem;
}


//CharItem 中存储的 charPath 与大地坐标系一致；坐标朝上，原点(0,0)在字符的左下角
CharItem* CFontLib::CreateChar(FontLibItem *pFont, int ch)
{
	CharItem *pItem = new CharItem();

	pItem->ch = ch;
	
	GraphicsPath& charpath = pItem->path;	
	HDC hdc = GetMemDC();
	HFONT oldFont = (HFONT)::SelectObject(hdc,pFont->hFont);
	TEXTMETRICW textMetric = pFont->textMetric;

	LONG lineSpacing = textMetric.tmHeight;	 //行之间的默认间隔
	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度

	::GetCharABCWidthsW(hdc,ch,ch,&pItem->abc);
	if( pFont->fontSetting.fHeight<2 )
	{
		pItem->abc.abcA = pItem->abc.abcA*pFont->fontSetting.fHeight/2;
		pItem->abc.abcB = pItem->abc.abcB*pFont->fontSetting.fHeight/2;
		pItem->abc.abcC = pItem->abc.abcC*pFont->fontSetting.fHeight/2;
	}
	
	if( pFont->fontSetting.fHeight<6 )
	{
		pItem->rcBound.X = pItem->rcBound.Y = 0;
		pItem->rcBound.Width = (pItem->abc.abcA+pItem->abc.abcB+pItem->abc.abcC)*pFont->fontSetting.fWidScale;
		pItem->rcBound.Height = textMetric.tmHeight;
		
		if( pFont->fontSetting.fHeight>3 )
			charpath.AddRectangle(RectF(pItem->rcBound.Width*0.2,pItem->rcBound.Height*0.2,pItem->rcBound.Width*0.6,pItem->rcBound.Height*0.6));
		else
			charpath.AddLine(pItem->rcBound.Width*0.5,0.0f,pItem->rcBound.Width*0.5,pItem->rcBound.Height);
		
		pFont->AddItem(pItem);

		::SelectObject(hdc,oldFont);

		return pItem;
	}

	Matrix matrix, matrix0;
	//先计算矩阵变换，变换的结果是使文字最后落在第一象限中
	//旋转变换
	matrix.Translate(0,textMetric.tmHeight);

	matrix.Scale(pFont->fontSetting.fWidScale,-1.0);		
	//耸肩变换 
	double angle = pFont->fontSetting.fInclineAngle*PI/180;
	if (pFont->fontSetting.nInclineType==SHRUGL)
	{
		matrix.Shear(-tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
	}
	else if (pFont->fontSetting.nInclineType==SHRUGR)
	{
		matrix.Shear(tan(angle),0.0,MatrixOrderAppend);
	}
	else if (pFont->fontSetting.nInclineType==SHRUGU)
	{
		matrix.Shear(0.0,tan(angle),MatrixOrderAppend);
	}
	else if (pFont->fontSetting.nInclineType==SHRUGD)
	{
		matrix.Shear(0.0,-tan(angle),MatrixOrderAppend);
	}

	int nPrevMode = ::SetBkMode(hdc, TRANSPARENT);

	wchar_t wch = ch;
	::BeginPath(hdc);
	::TextOutW(hdc, 0, 0, &wch, 1);
	::EndPath(hdc);
	
	int nNumPts = ::GetPath(hdc, NULL, NULL, 0);
	if (nNumPts == 0)
	{	
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);

		pItem->rcBound.X = pItem->rcBound.Y = 0;
		pItem->rcBound.Width = (pItem->abc.abcA+pItem->abc.abcB+pItem->abc.abcC)*pFont->fontSetting.fWidScale;
		pItem->rcBound.Height = textMetric.tmHeight;

		pFont->AddItem(pItem);
		return pItem;
	}
	
	// Allocate memory to hold points and stroke types from
	// the path.
	LPPOINT lpPoints = new POINT[nNumPts];
	if (lpPoints == NULL)
	{	
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);
		delete pItem;
		return NULL;
	}
	LPBYTE lpTypes = new BYTE[nNumPts];
	if (lpTypes == NULL)
	{
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);
		delete []lpPoints;
		delete pItem;
		return NULL;
	}
	
	// Now that we have the memory, really get the path data.
	nNumPts = GetPath(hdc, lpPoints, lpTypes, nNumPts);
	
	// If it worked, draw the lines. Win95 and Win98 don't support
	// the PolyDraw API, so we use our own member function to do
	// similar work. If you're targeting only Windows NT, you can
	// use the PolyDraw() API and avoid the COutlineView::PolyDraw()
	// member function.
	
	if (nNumPts == -1)
	{
		::SetBkMode(hdc, nPrevMode);
		::SelectObject(hdc,oldFont);
		delete []lpPoints;
		delete []lpTypes;
		delete pItem;
		return NULL;
	}
	
	if( pFont->fontSetting.fHeight<12 )
		PolyPath_compress(&charpath, lpPoints, lpTypes, nNumPts);
	else
		PolyPath(&charpath, lpPoints, lpTypes, nNumPts);

	RectF rcBound;
	charpath.GetBounds(&rcBound, NULL, NULL);
	PT_3D cpt(rcBound.X+rcBound.Width/2,rcBound.Y+rcBound.Height/2,0);

	matrix.Translate(cpt.x,cpt.y);

	//旋转变换
	matrix.Rotate(-pFont->fontSetting.fCharAngle);

	matrix.Translate(-cpt.x,-cpt.y);

	charpath.Transform(&matrix);
	charpath.GetBounds(&pItem->rcBound, NULL, NULL);

	delete [] lpPoints;
	delete [] lpTypes;
	::SetBkMode(hdc, nPrevMode);
	::SelectObject(hdc,oldFont);

	pFont->AddItem(pItem);

	return pItem;
}

extern void SetbmiHeader(BITMAPINFO *p, int w, int h, int bitcount);


static BITMAPINFO *Create8BmpInfo(int w, int h)
{
	BITMAPINFO *pBmpInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)];
	memset(pBmpInfo,0,sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));
	
	SetbmiHeader(pBmpInfo,w,h,8);
	
	RGBQUAD *table = (RGBQUAD*)pBmpInfo->bmiColors;
	for( int i=0; i<256; i++)
	{						
		table[i].rgbRed=clrTable_CAD[i*3];
		table[i].rgbGreen=clrTable_CAD[i*3+1];
		table[i].rgbBlue=clrTable_CAD[i*3+2];
		table[i].rgbReserved=0;
	}
	return pBmpInfo;
}

static HBITMAP Create8BitDIB(BITMAPINFO *pBmpInfo, BOOL bSetZero)
{
	void *pBits;
	HBITMAP hBmp = ::CreateDIBSection( NULL, pBmpInfo, DIB_RGB_COLORS,(void**)(&pBits), NULL,0L );

	if( bSetZero )memset(pBits,0,pBmpInfo->bmiHeader.biSizeImage);

	return hBmp;
}


void CFontLib::CreateCharBmp(FontLibItem *pFont, CharItem* pChar, COLORREF color, COLORREF bkColor)
{
	if( pChar->hBmp )
	{
		::DeleteObject(pChar->hBmp);
		pChar->hBmp = NULL;
	}
	pChar->nBmpWid = ceil(pChar->rcBound.Width);
	pChar->nBmpHei = ceil(pChar->rcBound.Height);

	int inflateX = pChar->nBmpWid/4, inflateY = pChar->nBmpHei/4;
	if( inflateX<2 )inflateX = 2;
	if( inflateY<2 )inflateY = 2;

	pChar->x0 = inflateX-pChar->rcBound.X; 
	pChar->y0 = inflateY+pChar->rcBound.Y+pChar->nBmpHei; // Y 是反的

	pChar->nBmpWid += (inflateX+inflateX); pChar->nBmpHei += (inflateY+inflateY); 

	if( !m_pBmpInfo )
	{
		m_pBmpInfo = Create8BmpInfo(pChar->nBmpWid,pChar->nBmpHei);
	}

	SetbmiHeader(m_pBmpInfo,pChar->nBmpWid,pChar->nBmpHei,8);

	pChar->hBmp = Create8BitDIB(m_pBmpInfo,TRUE);
	if( !pChar->hBmp )
		return;

	pChar->color = color;
	pChar->bkcolor = bkColor;

	HDC hdc = GetMemDC();
	HBITMAP hOld = (HBITMAP)::SelectObject(hdc,pChar->hBmp);

	if( bkColor!=0 )
	{
		::FillRect(hdc,&CRect(0,0,pChar->nBmpWid,pChar->nBmpHei),CBrush(bkColor));
	}

	//文本工具配置	
	Graphics graphics(hdc);

	CTempResetDCInfos resetDC(hdc);	

	graphics.SetPageUnit(UnitPixel);
	Color fontcolor;
	fontcolor.SetFromCOLORREF(color);
	
	SolidBrush brush(fontcolor);
	Pen pen(fontcolor);
	::SetTextAlign(hdc,TA_LEFT|TA_TOP);	
	
	BOOL bSmooth = FALSE;
	BOOL bOutLine = FALSE;
	//	textPath->Flatten(NULL,1.0f);

	graphics.TranslateTransform(pChar->x0,pChar->y0);
	graphics.ScaleTransform(1,-1);	
	
	if (bSmooth)
	{
		graphics.SetSmoothingMode(SmoothingModeHighQuality);
	}
	else
	{
		graphics.SetSmoothingMode(SmoothingModeHighSpeed);	
	}
	if (bOutLine)
	{
		graphics.DrawPath(&pen,&pChar->path);
	}
	else
	{
		graphics.FillPath(&brush,&pChar->path);
		graphics.DrawPath(&pen,&pChar->path);
	}

	(HBITMAP)::SelectObject(hdc,hOld);
}


wchar_t *ToWChar(LPCTSTR strText)
{
	
#ifdef _UNICODE
	size_t len  = wcslen(strText);	
#else
	std::wstring wstr0;
	ConvertCharToWstring(strText,wstr0,CP_ACP);
	size_t len  = wcslen(wstr0.c_str());	
#endif
	
	wchar_t *str = new wchar_t[len+1];	
	if (!str) 
	{
		return NULL;
	}
	
#ifdef _UNICODE
	wcscpy(str,LPCTSTR(strText));	
#else
	wcscpy(str,wstr0.c_str());		
#endif

	return str;
}

//x,y基于窗口坐标系，如果是GDI绘图，那么y轴正向朝下，否则y轴朝上
GraphicsPath *CFontLib::Plot(LPCTSTR text, int x, int y, BOOL bInverseY)
{
	if( m_pCurFont==NULL )
		return NULL;

	TEXTMETRICW textMetric = m_pCurFont->textMetric;

	TextSettings *pfs = &m_pCurFont->fontSetting;

	float fLineSpacingScale = pfs->fLineSpacingScale;
	if( fLineSpacingScale==0 )
		fLineSpacingScale = 1.0f;

	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*pfs->fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*fLineSpacingScale);//行步距	

	GraphicsPath textpath(FillModeWinding),rowtextpath(FillModeWinding), charpath(FillModeWinding);
	//分行做处理（\n）做隔断符

	wchar_t *str = ToWChar(text);
	if( !str )
		return NULL;

	int xStart = 0, yStart = 0, xMax = 0, yMax = 0;	
	
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
	Matrix matrix0;

	BOOL bFindDot = FALSE, bMultiLine = FALSE;
	float dotx=0, doty=0;
	
	while( token != NULL )
	{
		/* While there are tokens in "str" */
		int strlength = wcslen(token);
		rowtextpath.Reset();
		for (int i=0;i<strlength;i++)
		{
			CharItem *pCharItem = GetCharItem(token[i]);

			GraphicsPath *pCharPath =  pCharItem->path.Clone(); 
			
			matrix0.Reset();
			matrix0.Translate(xStart,yStart);

			if( bInverseY )
			{
				matrix0.Scale(1.0,-1.0);				
				
				matrix0.Translate(0,-textMetric.tmHeight);
			}

			pCharPath->Transform(&matrix0);			
			rowtextpath.AddPath(pCharPath,FALSE);

			if( pCharItem->ch==L'.' )
			{
				RectF rcDot;
				pCharPath->GetBounds(&rcDot);
				dotx = rcDot.X + rcDot.Width/2;
				doty = rcDot.Y + rcDot.Height/2;

				bFindDot = TRUE;
			}

			delete pCharPath;

			ABC abc = pCharItem->abc;

			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval);

		}

		if( xStart>xMax )xMax = xStart;
		xStart = 0;

		yMax += LineStep;
		
		if( bInverseY )
		{
			yStart = yMax;
		}
		else
		{
			matrix0.Reset();
			matrix0.Translate(0,LineStep);
			textpath.Transform(&matrix0);
		}

		textpath.AddPath(&rowtextpath,FALSE);

		/* Get next token: */
		token = wcstok( NULL, L"\n" );

		if( token )
		{
			bMultiLine = TRUE;
		}
	}
	delete []str;

	yMax -= textMetric.tmExternalLeading*fLineSpacingScale;
		
	Matrix mat;
	RectF rect;
	
	rect.X = 0;	rect.Width = xMax;
	rect.Y = 0; rect.Height = yMax;

	//对齐方式处理
	int yEnd = yMax;
	yStart = 0;

	if( bInverseY )
	{
		yStart = yEnd;
		yEnd = 0;
	}

	float orgx, orgy;	

	float angle = (bInverseY?-pfs->fTextAngle:pfs->fTextAngle);

	if( pfs->nAlignment==TAH_DOT  )
	{
		if( !bFindDot || bMultiLine )
		{
			dotx = rect.GetRight();
			doty = yStart;
		}
	}

	switch(pfs->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));						
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_DOT:
			orgx = dotx, orgy = doty;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));				
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
	}

	textpath.Transform(&mat);

	return textpath.Clone();
}

static void TransformPT_3Ds(Matrix *mt, PT_3D *pts, int npt)
{
	double x,y;
	float m[16];
	mt->GetElements(m);

	for( int i=0; i<npt; i++)
	{
		x = pts[i].x * m[0] + pts[i].y * m[2] + m[4];
		y = pts[i].x * m[1] + pts[i].y * m[3] + m[5];

		pts[i].x = x;
		pts[i].y = y;
	}
}


BOOL CFontLib::PlotOrigins(LPCTSTR text, CArray<PT_3D,PT_3D> *pts)
{
	if( m_pCurFont==NULL )
		return FALSE;

	TextSettings ts0 = m_pCurFont->fontSetting;
	TextSettings ts1 = ts0;
	ts1.fHeight = PC_SIZE;
	
	SetSettings(&ts1);
	if( m_pCurFont==NULL )
		return FALSE;

	pts->RemoveAll();

	TEXTMETRICW textMetric = m_pCurFont->textMetric;

	TextSettings *pfs = &m_pCurFont->fontSetting;

	float fLineSpacingScale = pfs->fLineSpacingScale;
	if( fLineSpacingScale==0 )
		fLineSpacingScale = 1.0f;

	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*pfs->fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*fLineSpacingScale);//行步距	

	CArray<PT_3D,PT_3D> textpath,rowtextpath,charpath;
	PT_3D pt0;

	//分行做处理（\n）做隔断符

	wchar_t *str = ToWChar(text);
	if( !str )
		return FALSE;

	int xStart = 0, yStart = 0, xMax = 0, yMax = 0;	
	
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
	Matrix matrix0;

	BOOL bFindDot = FALSE, bMultiLine = FALSE;
	float dotx = 0, doty = 0;
	
	while( token != NULL )
	{
		/* While there are tokens in "str" */
		int strlength = wcslen(token);
		rowtextpath.RemoveAll();
		for (int i=0;i<strlength;i++)
		{
			CharItem *pCharItem = GetCharItem(token[i]);

			ABC abc = pCharItem->abc;

			pt0 = PT_3D(0,0,0);

			if( pfs->nAlignment&TAV_TOP )
				pt0.y = LineStep;
			else if( pfs->nAlignment&TAV_MID )
				pt0.y = LineStep/2;

			if( pfs->nAlignment&TAH_MID )
				pt0.x = ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval)/2;
			else if( pfs->nAlignment&TAH_RIGHT )
				pt0.x = ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval);

			if( pfs->nAlignment==TAH_DOT )
			{
				pt0.x = ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval);
				pt0.y = 0;
			}
			
			matrix0.Reset();
			matrix0.Translate(xStart,yStart);

			TransformPT_3Ds(&matrix0,&pt0,1);

			if( pCharItem->ch==L'.' )
			{
				RectF rcDot;
				pCharItem->path.GetBounds(&rcDot);
				dotx = xStart + rcDot.X + rcDot.Width/2;
				doty = yStart + rcDot.Y + rcDot.Height/2;
				
				bFindDot = TRUE;
			}
		
			rowtextpath.Add(pt0);			

			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval);

		}

		if( xStart>xMax )xMax = xStart;
		xStart = 0;

		yMax += LineStep;
		
		matrix0.Reset();
		matrix0.Translate(0,LineStep);
		TransformPT_3Ds(&matrix0,textpath.GetData(),textpath.GetSize());

		textpath.Append(rowtextpath);

		/* Get next token: */
		token = wcstok( NULL, L"\n" );

		if( token!=NULL )
		{
			bMultiLine = TRUE;
		}
	}
	delete []str;
		
	Matrix mat;
	RectF rect;
	
	rect.X = 0;	rect.Width = xMax;
	rect.Y = 0; rect.Height = yMax;

	//对齐方式处理
	// Get the path's bounding rectangle.
	long yEnd = yMax;
	yStart = 0;

	float x=0, y=0;
	float orgx, orgy;	

	float angle = pfs->fTextAngle;

	mat.Scale(ts0.fHeight/PC_SIZE,ts0.fHeight/PC_SIZE);

	if( pfs->nAlignment==TAH_DOT  )
	{
		if( !bFindDot || bMultiLine )
		{
			dotx = rect.GetRight();
			doty = yStart;
		}
	}

	switch(pfs->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));						
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_DOT:
			orgx = dotx;
			orgy = doty;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
	}

	TransformPT_3Ds(&mat,textpath.GetData(),textpath.GetSize());

	pts->Copy(textpath);

	SetSettings(&ts0);

	return TRUE;
}


CPoint CFontLib::GetGDIPlotOrigin(LPCTSTR text, int x, int y)
{
	CPoint pt0(x,y);
	if( m_pCurFont==NULL )
		return pt0;

	TEXTMETRICW textMetric = m_pCurFont->textMetric;

	TextSettings *pfs = &m_pCurFont->fontSetting;

	float fLineSpacingScale = pfs->fLineSpacingScale;
	if( fLineSpacingScale==0 )
		fLineSpacingScale = 1.0f;

	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*pfs->fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*pfs->fLineSpacingScale);//行步距	

	//分行做处理（\n）做隔断符

	wchar_t *str = ToWChar(text);
	if( !str )
		return pt0;

	int xStart = 0, yStart = 0, xMax = 0;	

	BOOL bFindDot = FALSE, bMultiLine = FALSE;
	float dotx=0, doty=0;
	
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
	Matrix matrix0;
	
	while( token != NULL )
	{
		/* While there are tokens in "str" */
		int strlength = wcslen(token);
		for (int i=0;i<strlength;i++)
		{
			CharItem *pCharItem = GetCharItem(token[i]);
			
			if( pCharItem->ch==L'.' )
			{
				RectF rcDot;
				pCharItem->path.GetBounds(&rcDot);
				dotx = xStart + rcDot.X + rcDot.Width/2;
				doty = yStart + textMetric.tmHeight - (rcDot.Y + rcDot.Height/2);
				
				bFindDot = TRUE;
			}

			ABC abc = pCharItem->abc;

			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval);
		}

		if( xStart>xMax )xMax = xStart;

		xStart = 0;
		yStart += LineStep;

		/* Get next token: */
		token = wcstok( NULL, L"\n" );

		if( token )
		{
			bMultiLine = TRUE;
		}
	}

	yStart -= textMetric.tmExternalLeading*fLineSpacingScale;
		
	delete []str;
	
	Matrix mat;
	RectF rect;
	
	rect.X = 0;	rect.Width = xMax;
	rect.Y = 0; rect.Height = yStart;

	//对齐方式处理
	int yEnd = 0;

	float orgx, orgy;	
	
	if( pfs->nAlignment==TAH_DOT  )
	{
		if( !bFindDot || bMultiLine )
		{
			dotx = rect.GetRight();
			doty = yStart;
		}
	}

	switch(pfs->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;					
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetTop()*/;
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetTop()*/;
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetTop()*/;
			break;
		case TAH_DOT:
			orgx = dotx, orgy = doty;			
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			break;
	}

	return CPoint(x-orgx,y-orgy);
}


BOOL CFontLib::Plot(WCHAR wchar, CArray<GrVertex,GrVertex> *pts, double *nextCharPosDis, double *Width, double *Height)
{
	if( m_pCurFont==NULL )
		return FALSE;

	CharItem *pCharItem = GetCharItem(wchar);
	if( !pCharItem )
		return FALSE;

	TextSettings *pfs = &m_pCurFont->fontSetting;
	TEXTMETRICW textMetric = m_pCurFont->textMetric;

	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度

	long CharInterval = tmAveCharWidth*pfs->fCharIntervalScale;//字符间距
	long LineStep = ceil(lineSpacing*pfs->fLineSpacingScale);//行步距

	if (nextCharPosDis)
	{
		ABC abc = pCharItem->abc;
		*nextCharPosDis = ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+tmAveCharWidth*pfs->fCharIntervalScale);
	}

	RectF rect = pCharItem->rcBound;
	if (Width)
	{
		*Width = rect.Width;
	}
	if (Height)
	{
		*Height = rect.Height;
	}

	if( pts==NULL )
		return TRUE;

	int xStart = 0, yStart = 0, yEnd = rect.GetBottom();

	//对齐方式处理
	
	Matrix mat;

	float x = 0, y = 0, orgx, orgy;	

	float angle = pfs->fTextAngle;

	switch(pfs->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));						
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(angle,PointF(orgx,orgy));
			break;
	}

	{
		GrVertex pt,start;
		pt.z = 0;

		GraphicsPath *path = pCharItem->path.Clone();
		path->Transform(&mat);
		path->Flatten(NULL);
		PathData pathData;
		path->GetPathData(&pathData);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			pt.x = pathData.Points[j].X;
			pt.y = pathData.Points[j].Y;
			if (pathData.Types[j]==PathPointTypeStart )
			{
				pt.code = GRBUFFER_PTCODE_MOVETO;
				start = pt;				
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
			}
			else if(pathData.Types[j]==PathPointTypeCloseSubpath|PathPointTypeLine)
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
				pts->Add(pt);
				pt = start;
			}
			pts->Add(pt);			
		}	

		delete path;
	}
	return TRUE;
}


BOOL CFontLib::Plot(LPCTSTR text, CArray<GrVertex,GrVertex> *pts)
{
	GraphicsPath *path = Plot(text,0,0,FALSE);
	if (pts!=NULL)
	{
		GrVertex pt,start;
		pt.z = 0;
		
		path->Flatten(NULL);
		PathData pathData;
		path->GetPathData(&pathData);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			pt.x = pathData.Points[j].X;
			pt.y = pathData.Points[j].Y;

			int type = (pathData.Types[j]&PathPointTypePathTypeMask);

			if (type==PathPointTypeStart )
			{
				pt.code = GRBUFFER_PTCODE_MOVETO;
				start = pt;				
			}
			else if (type==PathPointTypeLine || type==PathPointTypeBezier )
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
			}
			else
			{
				int aaa=1;
			}
			
			if( type!=PathPointTypeStart && (pathData.Types[j]&PathPointTypeCloseSubpath) )
			{
				pt.code = GRBUFFER_PTCODE_LINETO;
				pts->Add(pt);
				pt = start;
			}

			pts->Add(pt);			
		}
	}

	delete path;

	return TRUE;
}


BOOL CFontLib::GetEnvlope(LPCTSTR text, double ex[4],double ey[4])
{
	if( m_pCurFont==NULL )
		return FALSE;

	TextSettings ts0 = m_pCurFont->fontSetting;
	TextSettings ts1 = ts0;
	ts1.fHeight = PC_SIZE;

	SetSettings(&ts1);
	if( m_pCurFont==NULL )
		return FALSE;

	TEXTMETRICW textMetric = m_pCurFont->textMetric;

	TextSettings *pfs = &ts0;

	float fLineSpacingScale = pfs->fLineSpacingScale;
	if( fLineSpacingScale==0 )
		fLineSpacingScale = 1.0f;

	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*pfs->fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*fLineSpacingScale);//行步距
	
	//分行做处理（\n）做隔断符

	wchar_t *str = ToWChar(text);
	if( !str )
		return FALSE;

	int xStart = 0, yStart = 0, xMax = 0;
	
	BOOL bFindDot = FALSE, bMultiLine = FALSE;
	float dotx=0, doty=0;
	
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
	Matrix matrix0;
	
	while( token != NULL )
	{
		/* While there are tokens in "str" */
		int strlength = wcslen(token);

		for (int i=0;i<strlength;i++)
		{
			CharItem *pCharItem = GetCharItem(token[i]);

			if( pCharItem->ch==L'.' )
			{
				RectF rcDot;
				pCharItem->path.GetBounds(&rcDot);
				dotx = xStart + rcDot.X + rcDot.Width/2;
				doty = yStart + rcDot.Y + rcDot.Height/2;
				
				bFindDot = TRUE;
			}

			ABC abc = pCharItem->abc;

			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval);
		}

		if( xStart>xMax )xMax = xStart;

		xStart = 0;
		yStart += LineStep;

		/* Get next token: */
		token = wcstok( NULL, L"\n" );
	}
	delete []str;
		
	//对齐方式处理
	// Get the path's bounding rectangle.
		
	Matrix mat;
	RectF rect;

	rect.X = 0;	rect.Width = xMax;
	rect.Y = 0; rect.Height = yStart;

	long yEnd = yStart;
	yStart = 0;

	if( pfs->nAlignment==TAH_DOT  )
	{
		if( !bFindDot || bMultiLine )
		{
			dotx = rect.GetRight();
			doty = yStart;
		}
	}

	float orgx, orgy;	
	float x=0,y=0;
	switch(pfs->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));						
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetTop()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_DOT:
			orgx = dotx, orgy = doty;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));	
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
	}

	if (ex!=NULL&&ey!=NULL)
	{
		Matrix matShear;
		//耸肩变换 
		double angle = pfs->fInclineAngle*PI/180;
		if (pfs->nInclineType==SHRUGL)
		{
			matShear.Shear(-tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
		}
		else if (pfs->nInclineType==SHRUGR)
		{
			matShear.Shear(tan(angle),0.0,MatrixOrderAppend);
		}
		else if (pfs->nInclineType==SHRUGU)
		{
			matShear.Shear(0.0,tan(angle),MatrixOrderAppend);
		}
		else if (pfs->nInclineType==SHRUGD)
		{
			matShear.Shear(0.0,-tan(angle),MatrixOrderAppend);
		}

		PointF ptf[4];
		ptf[3].X = ptf[0].X = rect.GetLeft();
		ptf[1].Y = ptf[0].Y = rect.GetBottom();
		ptf[2].X = ptf[1].X = rect.GetRight();
		ptf[2].Y = ptf[3].Y = rect.GetTop();
		matShear.TransformPoints(ptf,4);
		mat.TransformPoints(ptf,4);

		ex[0] = ptf[0].X*pfs->fHeight/PC_SIZE;ey[0] = ptf[0].Y*pfs->fHeight/PC_SIZE;
		ex[1] = ptf[1].X*pfs->fHeight/PC_SIZE;ey[1] = ptf[1].Y*pfs->fHeight/PC_SIZE;
		ex[2] = ptf[2].X*pfs->fHeight/PC_SIZE;ey[2] = ptf[2].Y*pfs->fHeight/PC_SIZE;
		ex[3] = ptf[3].X*pfs->fHeight/PC_SIZE;ey[3] = ptf[3].Y*pfs->fHeight/PC_SIZE;
	}

	SetSettings(&ts0);

	return TRUE;
}

static BOOL IncludeCnChar(LPCTSTR text)
{

#ifdef _UNICODE

	const TCHAR *p = text;
	while( (*p)!=0 )
	{
		if( (*p)>=255 )
			return TRUE;
		p++;
	}

#else

	const TCHAR *p = text;
	while( (*p)!=0 )
	{
		if( (*p)<0 )
			return TRUE;
		p++;
	}

#endif

	return FALSE;
}


void CFontLib::DrawText_GDIPlus(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, COLORREF bkColor, BOOL bSmooth, BOOL bOutLine)
{
	if( 0 && !IncludeCnChar(strText) && m_bUseTextBmpCache && settings->fHeight<30 && settings->fTextAngle==0 && color!=0 )
	{
		DrawTextWithBmp(strText,x,y,hdc,settings,color,bkColor);
		return;
	}
	//文本工具配置
	Graphics& graphics = GetDrawGraphics(hdc);

	graphics.ResetClip();
	graphics.ResetTransform();
	graphics.SetPageUnit(UnitPixel);
	Color fontcolor;
	fontcolor.SetFromCOLORREF(color);

	SolidBrush brush(fontcolor);
	Pen pen(fontcolor);
	::SetTextAlign(hdc,TA_LEFT|TA_TOP);

	FontLibItem *pItem = SetSettings(settings);
	if( pItem!=NULL && settings->IsStandardFont() )
	{
		CPoint pt0 = GetGDIPlotOrigin(strText,x,y);

		HFONT hOldFont = (HFONT)::SelectObject(hdc,pItem->hFont);
		::SetBkMode(hdc,TRANSPARENT);
		::SetTextColor(hdc,color);
		
		TEXTMETRICW textMetric = m_pCurFont->textMetric;
		
		TextSettings *pfs = &m_pCurFont->fontSetting;
		
		LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
		long LineStep = ceil(lineSpacing*pfs->fLineSpacingScale);//行步距
		
		if( pfs->fLineSpacingScale==0 )
			LineStep = lineSpacing;

		int nlen = _tcslen(strText);
		TCHAR *str2 = new TCHAR[nlen+1];
		_tcscpy(str2,strText);

		int yStart = pt0.y;	

		TCHAR *pos0 = str2, *pos;

		graphics.SetSmoothingMode(SmoothingModeHighQuality);


		do 
		{
			pos = _tcschr(pos0,'\n');
			if( pos )
			{
				*pos = 0;
				::TextOut(hdc,pt0.x,yStart,pos0,_tcslen(pos0));

				pos0 = pos+1;
				
			}
			else
			{
				::TextOut(hdc,pt0.x,yStart,pos0,_tcslen(pos0));
			}			

			yStart += LineStep;

		} while (pos!=NULL);

		delete[] str2;

		::SelectObject(hdc,hOldFont);
		return;
	}
	GraphicsPath *textPath = Plot(strText,x,y,TRUE);
	
//	bSmooth = FALSE;
//	textPath->Flatten(NULL,1.0f);

	if( settings->fHeight<30 )
		bSmooth = FALSE;

	if (bSmooth)
	{
		graphics.SetSmoothingMode(SmoothingModeHighQuality);
	}
	else
	{
		graphics.SetSmoothingMode(SmoothingModeHighSpeed);	
	}
	if (bOutLine)
	{
		graphics.DrawPath(&pen,textPath);
	}
	else
	{
		if( settings->fHeight>8 )
			graphics.FillPath(&brush,textPath);
		graphics.DrawPath(&pen,textPath);
	}

	delete textPath;
}

void CFontLib::DrawTextWithBmp(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, COLORREF bkColor)
{
	if( m_pCurFont==NULL )
		return;

	TEXTMETRICW textMetric = m_pCurFont->textMetric;

	TextSettings *pfs = &m_pCurFont->fontSetting;

	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*pfs->fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*pfs->fLineSpacingScale);//行步距

	if( pfs->fLineSpacingScale==0 )
		LineStep = lineSpacing;
	
	//分行做处理（\n）做隔断符

	wchar_t *str = ToWChar(strText);
	if( !str )
		return;

	int xStart = 0, yStart = 0, xMax = 0;	
	
	CArray<CPoint,CPoint> arrStartPts;
	CArray<CharItem*,CharItem*> arrPChars;
	
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
	Matrix matrix0;
	
	while( token != NULL )
	{
		int strlength = wcslen(token);
		//rowtextpath.Reset();
		for (int i=0;i<strlength;i++)
		{
			CharItem *pCharItem = GetCharItem(token[i]);

			if( pCharItem->hBmp==NULL || pCharItem->color!=color || pCharItem->bkcolor!=bkColor )
			{
				CreateCharBmp(m_pCurFont,pCharItem,color,bkColor);
			}
			
			//GraphicsPath *pCharPath =  pCharItem->path.Clone(); 

			//matrix0.Reset();
			//matrix0.Translate(xStart,yStart);

			//matrix0.Scale(1,-1);			

			//pCharPath->Transform(&matrix0);			
			//rowtextpath.AddPath(pCharPath,FALSE);
			
			//delete pCharPath;

			arrPChars.Add(pCharItem);
			arrStartPts.Add(CPoint(xStart,yStart));
			
			ABC abc = pCharItem->abc;
			
			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*pfs->fWidScale+CharInterval);
		}
		
		if( xStart>xMax )xMax = xStart;
		//textpath.AddPath(&rowtextpath,FALSE);
		xStart = 0;

		yStart += LineStep;

		/* Get next token: */
		token = wcstok( NULL, L"\n" );
	}
	delete []str;

	//对齐方式处理
	// Get the path's bounding rectangle.
	Matrix mat;
	RectF rect;
	
	int yEnd = yStart;
	yStart = 0;

	rect.X = 0;	rect.Width = xMax;
	rect.Y = 0; rect.Height = yEnd;	

	//textpath.GetBounds(&rect, NULL, NULL);

	float orgx, orgy;	

	switch(pfs->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yStart/*rect.GetBottom()*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));						
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart/*rect.GetBottom()*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yStart/*rect.GetBottom()*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetTop()*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetTop()*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetTop()*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			//mat.Translate(x-orgx,y-orgy);
			//mat.RotateAt(pfs->fTextAngle,PointF(orgx,orgy));
			break;
	}

	orgy -= lineSpacing;

	//textpath.Transform(&mat);
	HDC hdc0 = GetMemDC();
	HBITMAP hOld = NULL;

	for( int i=0; i<arrPChars.GetSize(); i++)
	{
		CharItem *pCharItem = arrPChars[i];
		CPoint pt = arrStartPts[i];

		if( i==0 )
			hOld = (HBITMAP)SelectObject(hdc0, pCharItem->hBmp);
		else
			SelectObject(hdc0, pCharItem->hBmp);

		TransparentBlt(hdc,x-orgx+pt.x-pCharItem->x0-2,y-orgy+pt.y-pCharItem->y0,
			pCharItem->nBmpWid,pCharItem->nBmpHei,
			hdc0,0,0,pCharItem->nBmpWid,pCharItem->nBmpHei,bkColor);

	}

	if( hOld )
	{
		SelectObject(hdc0, hOld);
	}

	return;
}


void CFontLib::DrawText_GL(LPCTSTR strText, int x, int y, const TextSettings *settings, COLORREF color, BOOL bSmooth, BOOL bOutLine)
{
	//文本工具配置	
	SetSettings(settings);
	GraphicsPath *ptextPath = Plot(strText,x,y,FALSE);
	GraphicsPath& textpath = *ptextPath;

	if (!bOutLine)
	{
		glMatrixMode(GL_MODELVIEW_MATRIX);	
		glPushMatrix();
		
		//glTranslated(x,y,0);
		//		glEnable(GL_BLEND);
		//		glColor3f(GetRValue(color)/255.0,GetGValue(color)/255.0,GetBValue(color)/255.0);
		int nPos = 0;
		textpath.Flatten(NULL,0.1);
		PathData pathData;
		textpath.GetPathData(&pathData);

		CArray<PT_3D,PT_3D> arrPts;

		CDxPolygon ploy;
		//		glEnable(GL_POLYGON_SMOOTH);
		ploy.BeginPolygon(color);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			
			if (pathData.Types[j]==PathPointTypeStart )
			{
				ploy.BeginContour();		
				//				::glBegin(GL_POLYGON);			
				//				::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);

				arrPts.RemoveAll();
				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
				
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				//ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//				::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);

				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
				
			}
			else if(pathData.Types[j]==(PathPointTypeCloseSubpath|PathPointTypeLine))
			{
				//ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//			::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//			::glEnd();

				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
				
				SimpleCompressPoints(arrPts,0.1f);
				for( int k=0; k<arrPts.GetSize(); k++)
				{
					ploy.Vertex(arrPts[k].x,arrPts[k].y,0);
				}

				ploy.EndContour();
			}
			
		}
		ploy.EndPolygon();
		// 		glDisable(GL_POLYGON_SMOOTH);
		// 		glDisable(GL_BLEND);
		glPopMatrix();
	}
	else
	{
		glMatrixMode(GL_MODELVIEW_MATRIX);	
		glPushMatrix();
		//glTranslated(x,y,0);
		int nPos = 0;
		textpath.Flatten(NULL,0.1);
		PathData pathData;
		textpath.GetPathData(&pathData);
		glColor3f(GetRValue(color)/255.0,GetGValue(color)/255.0,GetBValue(color)/255.0);
		glLineWidth(1.0f);

		CArray<PT_3D,PT_3D> arrPts;

		for(INT j = 0; j < pathData.Count; ++j)
		{	
			if (pathData.Types[j]==PathPointTypeStart )
			{
				//::glBegin(GL_LINE_STRIP);							
				//::glVertex3f(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);

				arrPts.RemoveAll();
				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				//::glVertex3f(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);

				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
			}
			else if(pathData.Types[j]==(PathPointTypeCloseSubpath|PathPointTypeLine))
			{
				//::glVertex3f(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);		
				//::glEnd();

				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
				
				SimpleCompressPoints(arrPts,0.1f);

				::glBegin(GL_LINE_STRIP);	
				for( int k=0; k<arrPts.GetSize(); k++)
				{
					::glVertex3f(arrPts[k].x,arrPts[k].y,VERTEX_DEPTH);
				}
				::glEnd();
			}
		}
		glPopMatrix();
	}
	
	delete ptextPath;
}



void CFontLib::DrawText_D3D(LPCTSTR strText, int x, int y, HDC hdc, CD3DDrawLineDC *pDC, const TextSettings *settings, COLORREF color, BOOL bSmooth, BOOL bOutLine)
{
	SetSettings(settings);
	GraphicsPath *ptextPath = Plot(strText,x,y,FALSE);
	GraphicsPath& textpath = *ptextPath;
	
	if (!bOutLine)
	{
		D3DXMATRIX matWorld, m1, m2;
		pDC->m_pD3D->m_pD3DDevice9->GetTransform(D3DTS_WORLD,&matWorld);
		
		D3DXMatrixTranslation(&m1,x,y,0);
		D3DXMatrixMultiply(&m2,&matWorld,&m1);
		
		//pDC->m_pD3D->m_pD3DDevice9->SetTransform(D3DTS_WORLD,&m2);
		
		//		glEnable(GL_BLEND);
		//		glColor3f(GetRValue(color)/255.0,GetGValue(color)/255.0,GetBValue(color)/255.0);
		
		pDC->SetColor(color);
		
		int nPos = 0;
		textpath.Flatten(NULL,0.1);
		PathData pathData;
		textpath.GetPathData(&pathData);
		
		CD3DPolygon ploy;
		ploy.Init(pDC->m_pD3D);		

		CArray<PT_3D,PT_3D> arrPts;
		
		//		glEnable(GL_POLYGON_SMOOTH);
		ploy.BeginPolygon(color);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			
			if (pathData.Types[j]==PathPointTypeStart )
			{
				ploy.BeginContour();		
				//				::glBegin(GL_POLYGON);			
				//				::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);

				arrPts.RemoveAll();
				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
				
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				//ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//				::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);

				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));
				
			}
			else if(pathData.Types[j]==(PathPointTypeCloseSubpath|PathPointTypeLine))
			{
				//ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//			::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//			::glEnd();

				arrPts.Add(PT_3D(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH));

				SimpleCompressPoints(arrPts,1.0f);
				for( int k=0; k<arrPts.GetSize(); k++)
				{
					ploy.Vertex(arrPts[k].x,arrPts[k].y,0);
				}
				ploy.EndContour();
				
			}
			
		}
		ploy.EndPolygon();
		
		pDC->m_pD3D->m_pD3DDevice9->SetTransform(D3DTS_WORLD,&matWorld);
		// 		glDisable(GL_POLYGON_SMOOTH);
		// 		glDisable(GL_BLEND);
	}
	else
	{
		int nPos = 0;
		textpath.Flatten(NULL,0.1);
		PathData pathData;
		textpath.GetPathData(&pathData);
		
		D3DXMATRIX matWorld, m1, m2;
		pDC->m_pD3D->m_pD3DDevice9->GetTransform(D3DTS_WORLD,&matWorld);
		
		D3DXMatrixTranslation(&m1,x,y,0);
		D3DXMatrixMultiply(&m2,&m1,&matWorld);
		
		//pDC->m_pD3D->m_pD3DDevice9->SetTransform(D3DTS_WORLD,&m2);
		
		pDC->SetColor(color);
		pDC->SetWidth(1.0f);
		pDC->Begin(pathData.Count);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			if (pathData.Types[j]==PathPointTypeStart )
			{
				pDC->MoveTo(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				pDC->LineTo(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
			}
			else if(pathData.Types[j]==(PathPointTypeCloseSubpath|PathPointTypeLine))
			{
				pDC->LineTo(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
			}
		}
		pDC->End();
		
		pDC->m_pD3D->m_pD3DDevice9->SetTransform(D3DTS_WORLD,&matWorld);
	}
	
	delete ptextPath;
}



HDC CFontLib::GetMemDC()
{
	if(m_hdc==NULL)
		m_hdc = ::CreateCompatibleDC(NULL);
	return m_hdc;
}


Graphics& CFontLib::GetDrawGraphics(HDC hdc)
{
	if( m_pDrawGraphics==NULL  )
	{
		if( hdc )
		{
			m_pDrawGraphics = new Graphics(hdc);
		}

		m_hDrawDC = hdc;
	}
	else if( m_hDrawDC!=hdc && hdc!=NULL )
	{
		delete m_pDrawGraphics;

		m_pDrawGraphics = new Graphics(hdc);

		m_hDrawDC = hdc;
	}
	
	return *m_pDrawGraphics;
}


BOOL CFontLib::EnableTextBmpCache(BOOL bEnable)
{
	BOOL bSaveFlag = m_bUseTextBmpCache;
	m_bUseTextBmpCache = bEnable;
	
	return bSaveFlag;
}

EXPORT_SMARTVIEW void ClearFontLib()
{
	g_Fontlib.Clear();
}

MyNameSpaceEnd