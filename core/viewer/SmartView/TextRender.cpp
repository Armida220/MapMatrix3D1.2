// GDIPlusFont.cpp: implementation of the CGDIPlusFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextRender.h"
#include "math.h"
#include <string>
#include "SmartViewFunctions.h"
#include "glew.h"
#include "DxFont.h "
#include "GrBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MyNameSpaceBegin


CBufAccel::CBufAccel()
{
	m_nMaxBufSize = 10;
}

void CBufAccel::Clear()
{
	while(m_arrBufs.GetSize()>0)
	{
		
		delete (GraphicsPath*)m_arrBufs.GetAt(0);
		m_arrBufs.RemoveAt(0);
	}
	m_arrBufs.RemoveAll();
	m_arrChars.RemoveAll();
	m_arrCmpPars.RemoveAll();
}

CBufAccel::~CBufAccel()
{
	while(m_arrBufs.GetSize()>0)
	{
		
		delete (GraphicsPath*)m_arrBufs.GetAt(0);
		m_arrBufs.RemoveAt(0);
	}
	m_arrBufs.RemoveAll();
	m_arrChars.RemoveAll();
	m_arrCmpPars.RemoveAll();
}

GraphicsPath* CBufAccel::FindBuf(const TextSettings *settings,wchar_t ch,long *abc)
{
	
	BOOL bFind = FALSE;
	for (int i=m_arrCmpPars.GetSize()-1;i>=0;i--)
	{
		if (memcmp(&(m_arrCmpPars.GetAt(i)),settings,sizeof(TextSettings))==0&&memcmp(&ch,m_arrChars.GetData()+i,sizeof(wchar_t))==0)
		{
			bFind = TRUE;
			break;
		}
	}
	if (bFind)
	{
		if (i!=m_arrCmpPars.GetSize()-1)
		{	int abc0 = m_arrABCs.GetAt(i); 
			TextSettings set = m_arrCmpPars.GetAt(i);
			GraphicsPath *path = m_arrBufs.GetAt(i);
			wchar_t ch = m_arrChars.GetAt(i);
			m_arrABCs.RemoveAt(i);
			m_arrChars.RemoveAt(i);
			m_arrCmpPars.RemoveAt(i);
			m_arrBufs.RemoveAt(i);
			m_arrABCs.Add(abc0);
			m_arrCmpPars.Add(set);
			m_arrChars.Add(ch);
			m_arrBufs.Add(path);
			if (abc)
			{
				*abc = abc0;
			}
			return path;
		}
		else
		{
			if(abc) *abc = m_arrABCs.GetAt(i);
			return m_arrBufs.GetAt(i);
		}
	}
	else
	{
		return NULL;
	}
}

void CBufAccel::AddBuf(const GraphicsPath *path,const TextSettings *settings,wchar_t ch,long abc)
{
	if (m_arrCmpPars.GetSize()==m_nMaxBufSize)
	{
		GraphicsPath *path0 = path->Clone();		
		delete m_arrBufs.GetAt(0);
		m_arrABCs.RemoveAt(0);
		m_arrBufs.RemoveAt(0);
		m_arrCmpPars.RemoveAt(0);
		m_arrChars.RemoveAt(0);
		m_arrABCs.Add(abc);
		m_arrBufs.Add(path0);
		m_arrCmpPars.Add(*settings);
		m_arrChars.Add(ch);
	}
	else
	{
		GraphicsPath *path0 = path->Clone();
		m_arrABCs.Add(abc);
		m_arrBufs.Add(path0);
		m_arrCmpPars.Add(*settings);
		m_arrChars.Add(ch);
	}
}


CFontParAccel::CFontParAccel()
{
	m_nMaxBufSize = 10;
}

CFontParAccel::~CFontParAccel()
{
	for (int i=0;i<m_arrFont.GetSize()-1;i++)
	{
		DeleteObject(m_arrFont.GetAt(i));
	}
}

HFONT CFontParAccel::FindFontPar(const TextSettings *settings,FontItem *item)
{
	BOOL bFind = FALSE;
	for (int i=m_arrCmpPars.GetSize()-1;i>=0;i--)
	{
		if (memcmp(&(m_arrCmpPars.GetAt(i)),settings,sizeof(TextSettings))==0)
		{
			bFind = TRUE;
			break;
		}
	}
	if (bFind)
	{
		if (i!=m_arrCmpPars.GetSize()-1)
		{
			TextSettings set = m_arrCmpPars.GetAt(i);
			FontItem  item0 = m_arrBufs.GetAt(i);
			HFONT hfont = m_arrFont.GetAt(i);
		
			m_arrCmpPars.RemoveAt(i);
			m_arrBufs.RemoveAt(i);
			
			m_arrFont.RemoveAt(i);
		
			m_arrCmpPars.Add(set);		
			m_arrFont.Add(hfont);
			m_arrBufs.Add(item0);
			if (item)
			{
				*item = item0;
			}
			return hfont;
		}
		else
		{
			if (item)
			{
				*item = m_arrBufs.GetAt(i);
			}
			return m_arrFont.GetAt(i);
		}
	}
	else
	{
		return 0;
	}
}

void CFontParAccel::AddFontPar(FontItem item,HFONT font,const TextSettings *settings)
{
	if (m_arrCmpPars.GetSize()==m_nMaxBufSize)
	{	
		m_arrCmpPars.RemoveAt(0);
		DeleteObject(m_arrFont.GetAt(0));
		m_arrFont.RemoveAt(0);
		m_arrBufs.RemoveAt(0);
		m_arrCmpPars.Add(*settings);
		m_arrFont.Add(font);
		m_arrBufs.Add(item);
	}
	else
	{
		m_arrCmpPars.Add(*settings);
		m_arrFont.Add(font);
		m_arrBufs.Add(item);
	}
}

CBufAccel g_BufAccel;
CFontParAccel g_fontParAccel;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextRender::CTextRender()
{

}

CTextRender::~CTextRender()
{

}

void  PolyPath(GraphicsPath* pPath, 
					 CONST LPPOINT lppt, 
					 CONST LPBYTE lpbTypes,
		int cCount )
{
	int nIndex;
	LPPOINT pptLastMoveTo = NULL;
	LPPOINT pptPrev = NULL;
	
	// for each of the points we have...
	for (nIndex = 0; nIndex < cCount; nIndex++)
	{
		switch(lpbTypes[nIndex])
		{
		case PT_MOVETO:
			if (pptLastMoveTo != NULL && nIndex > 0)
				pPath->CloseFigure();
			pptLastMoveTo = &lppt[nIndex];
			pptPrev = &lppt[nIndex];
			break;
			
		case PT_LINETO | PT_CLOSEFIGURE:
			pPath->AddLine( pptPrev->x, pptPrev->y, lppt[nIndex].x, lppt[nIndex].y);
			pptPrev = &lppt[nIndex];
			if (pptLastMoveTo != NULL)
			{
				pPath->CloseFigure();
				pptPrev = pptLastMoveTo;
			}
			pptLastMoveTo = NULL;
			break;
			
		case PT_LINETO:
			pPath->AddLine( pptPrev->x, pptPrev->y, lppt[nIndex].x, lppt[nIndex].y);
			pptPrev = &lppt[nIndex];
			break;
			
		case PT_BEZIERTO | PT_CLOSEFIGURE:
			//ASSERT(nIndex + 2 <= cCount);
			pPath->AddBezier( 
				pptPrev->x, pptPrev->y,
				lppt[nIndex].x, lppt[nIndex].y,
				lppt[nIndex+1].x, lppt[nIndex+1].y,
				lppt[nIndex+2].x, lppt[nIndex+2].y );
			nIndex += 2;
			pptPrev = &lppt[nIndex];
			if (pptLastMoveTo != NULL)
			{
				pPath->CloseFigure();
				pptPrev = pptLastMoveTo;
			}
			pptLastMoveTo = NULL;
			break;
			
		case PT_BEZIERTO:
			//ASSERT(nIndex + 2 <= cCount);
			pPath->AddBezier( 
				pptPrev->x, pptPrev->y,
				lppt[nIndex].x, lppt[nIndex].y,
				lppt[nIndex+1].x, lppt[nIndex+1].y,
				lppt[nIndex+2].x, lppt[nIndex+2].y );
			nIndex += 2;
			pptPrev = &lppt[nIndex];
			break;
		}
	}
	
	// If the figure was never closed and should be,
	// close it now.
	if (pptLastMoveTo != NULL && nIndex > 1)
	{
		pPath->AddLine( pptPrev->x, pptPrev->y, pptLastMoveTo->x, pptLastMoveTo->y);
		//pPath->CloseFigure();
	}
}



void  PolyPath_compress(GraphicsPath* pPath, 
			   CONST LPPOINT lppt, 
			   CONST LPBYTE lpbTypes,
			   int cCount )
{
	int nIndex;
	LPPOINT pptLastMoveTo = NULL;
	LPPOINT pptPrev = NULL;

	CArray<PT_3D,PT_3D> arrPts;
	
	// for each of the points we have...
	for (nIndex = 0; nIndex < cCount; nIndex++)
	{
		switch(lpbTypes[nIndex])
		{
		case PT_MOVETO:
			if (pptLastMoveTo != NULL && nIndex > 0)
			{
				SimpleCompressPoints(arrPts,0.1f);
				for( int i=0; i<arrPts.GetSize()-1; i++)
				{
					pPath->AddLine( (float)arrPts[i].x, (float)arrPts[i].y, (float)arrPts[i+1].x, (float)arrPts[i+1].y);
				}
				arrPts.RemoveAll();

				pPath->CloseFigure();
			}
			pptLastMoveTo = &lppt[nIndex];
			pptPrev = &lppt[nIndex];
			break;
			
		case PT_LINETO | PT_CLOSEFIGURE:
			//pPath->AddLine( pptPrev->x, pptPrev->y, lppt[nIndex].x, lppt[nIndex].y);
			arrPts.Add(PT_3D(pptPrev->x, pptPrev->y,0));
			arrPts.Add(PT_3D(lppt[nIndex].x, lppt[nIndex].y,0));

			pptPrev = &lppt[nIndex];
			if (pptLastMoveTo != NULL)
			{
				SimpleCompressPoints(arrPts,0.1f);
				for( int i=0; i<arrPts.GetSize()-1; i++)
				{
					pPath->AddLine( (float)arrPts[i].x, (float)arrPts[i].y, (float)arrPts[i+1].x, (float)arrPts[i+1].y);
				}
				arrPts.RemoveAll();

				pPath->CloseFigure();
				pptPrev = pptLastMoveTo;
			}
			pptLastMoveTo = NULL;
			break;
			
		case PT_LINETO:
			//pPath->AddLine( pptPrev->x, pptPrev->y, lppt[nIndex].x, lppt[nIndex].y);
			arrPts.Add(PT_3D(pptPrev->x, pptPrev->y,0));
			arrPts.Add(PT_3D(lppt[nIndex].x, lppt[nIndex].y,0));

			pptPrev = &lppt[nIndex];
			break;
			
		case PT_BEZIERTO | PT_CLOSEFIGURE:
			//ASSERT(nIndex + 2 <= cCount);
// 			pPath->AddBezier( 
// 				pptPrev->x, pptPrev->y,
// 				lppt[nIndex].x, lppt[nIndex].y,
// 				lppt[nIndex+1].x, lppt[nIndex+1].y,
// 				lppt[nIndex+2].x, lppt[nIndex+2].y );
			arrPts.Add(PT_3D(pptPrev->x, pptPrev->y,0));
			arrPts.Add(PT_3D(lppt[nIndex].x, lppt[nIndex].y,0));
			arrPts.Add(PT_3D(lppt[nIndex+1].x, lppt[nIndex+1].y,0));
			arrPts.Add(PT_3D(lppt[nIndex+2].x, lppt[nIndex+2].y,0));

			nIndex += 2;
			pptPrev = &lppt[nIndex];
			if (pptLastMoveTo != NULL)
			{
				SimpleCompressPoints(arrPts,0.1f);
				for( int i=0; i<arrPts.GetSize()-1; i++)
				{
					pPath->AddLine( (float)arrPts[i].x, (float)arrPts[i].y, (float)arrPts[i+1].x, (float)arrPts[i+1].y);
				}
				arrPts.RemoveAll();

				pPath->CloseFigure();
				pptPrev = pptLastMoveTo;
			}
			pptLastMoveTo = NULL;
			break;
			
		case PT_BEZIERTO:
			//ASSERT(nIndex + 2 <= cCount);
// 			pPath->AddBezier( 
// 				pptPrev->x, pptPrev->y,
// 				lppt[nIndex].x, lppt[nIndex].y,
// 				lppt[nIndex+1].x, lppt[nIndex+1].y,
// 				lppt[nIndex+2].x, lppt[nIndex+2].y );

			arrPts.Add(PT_3D(pptPrev->x, pptPrev->y,0));
			arrPts.Add(PT_3D(lppt[nIndex].x, lppt[nIndex].y,0));
			arrPts.Add(PT_3D(lppt[nIndex+1].x, lppt[nIndex+1].y,0));
			arrPts.Add(PT_3D(lppt[nIndex+2].x, lppt[nIndex+2].y,0));

			nIndex += 2;
			pptPrev = &lppt[nIndex];
			break;
		}
	}
	
	// If the figure was never closed and should be,
	// close it now.
	if (pptLastMoveTo != NULL && nIndex > 1)
	{
		//pPath->AddLine( pptPrev->x, pptPrev->y, pptLastMoveTo->x, pptLastMoveTo->y);
		//pPath->CloseFigure();

		arrPts.Add(PT_3D(pptPrev->x, pptPrev->y,0));
		arrPts.Add(PT_3D(pptLastMoveTo->x, pptLastMoveTo->y,0));

		SimpleCompressPoints(arrPts,0.1f);
		for( int i=0; i<arrPts.GetSize()-1; i++)
		{
			pPath->AddLine( (float)arrPts[i].x, (float)arrPts[i].y, (float)arrPts[i+1].x, (float)arrPts[i+1].y);
		}
		arrPts.RemoveAll();
	}
}

void CTextRender::DrawText(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, BOOL bSmooth, BOOL bOutLine)
{
	const  wchar_t *text = NULL;
#ifdef _UNICODE
	text = strText;
#else
	std::wstring wstr;
	ConvertCharToWstring(strText,wstr,CP_ACP);
	text = wstr.c_str();
#endif
	if (text==NULL||wcslen(text)<=0 || settings->fHeight<1 )
	{
		return;
	}
	//文本工具配置
	Graphics graphics(hdc);
	graphics.SetPageUnit(UnitPixel);
	Color fontcolor;
	fontcolor.SetFromCOLORREF(color);
// #ifdef _UNICODE
// 	FontFamily fontFamily(settings->tcFaceName);
// #else
// 	std::wstring wstr0;
// 	ConvertCharToWstring(settings->tcFaceName,wstr0,CP_ACP);
// 	FontFamily fontFamily(wstr0.c_str());
// #endif
	
// 	Font font(&fontFamily,settings->fHeight,FontStyleRegular,UnitWorld);
	SolidBrush brush(fontcolor);
	Pen pen(fontcolor);
	::SetTextAlign(hdc,TA_LEFT|TA_TOP);
	GraphicsPath textpath(FillModeWinding),rowtextpath(FillModeWinding), charpath(FillModeWinding);

	HFONT hFont;
	HFONT oldFont;
	TEXTMETRIC textMetric;
	
	LONG tmAveCharWidth ; //字体的平均宽度
	long CharInterval ;//字符间距
	LONG lineSpacing ;	 //行之间的默认间隔
	long LineStep ;//行步距
	CFontParAccel::FontItem item;
	if(hFont = g_fontParAccel.FindFontPar(settings,&item))
	{
		oldFont = (HFONT)::SelectObject(hdc,hFont);		
		tmAveCharWidth = item.tmAveCharWidth;
		CharInterval = item.CharInterval;
		lineSpacing = item.lineSpacing;
		LineStep = item.LineStep;
	}
	else
	{
		hFont = ::CreateFont(
			settings->fHeight,				   // nHeight
			0,                   // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			0,                 // nWeight
			FALSE,           // bItalic
			FALSE,        // bUnderline
			FALSE,        // cStrikeOut
			DEFAULT_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH,  // nPitchAndFamily
		settings->tcFaceName);
		oldFont = (HFONT)::SelectObject(hdc,hFont);
		::GetTextMetrics(hdc,&textMetric);
		item.tmAveCharWidth = tmAveCharWidth = textMetric.tmAveCharWidth;
		item.CharInterval = CharInterval = tmAveCharWidth*settings->fCharIntervalScale;
		item.lineSpacing = lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;
		item.LineStep = LineStep = ceil(lineSpacing*settings->fLineSpacingScale);
		g_fontParAccel.AddFontPar(item,hFont,settings);
	}

	//分行做处理（\n）做隔断符
	int xStart = 0, yStart = 0;
	size_t len = wcslen(text);
	
	wchar_t *str = new wchar_t[len+1];
	if (!str) 
	{
		::SelectObject(hdc,oldFont);	
//		::DeleteObject(hFont);
		return;
	}
	Matrix matrix, matrix0;

	

	wcscpy(str,text);
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
	int nPrevMode = ::SetBkMode(hdc, TRANSPARENT);
	Rect firstRect;
	BOOL bInitMatrix = FALSE;
	GraphicsPath* tempath;
	BOOL bFirst = TRUE;
	while( token != NULL )
	{
		/* While there are tokens in "str" */
		int strlength = wcslen(token);
		rowtextpath.Reset();
		for (int i=0;i<strlength;i++)
		{
			long abc0;
			if (tempath = g_BufAccel.FindBuf(settings,token[i],&abc0))
			{
				tempath = tempath->Clone();
				matrix0.Reset();
				matrix0.Translate(xStart,yStart);
				tempath->Transform(&matrix0);
				rowtextpath.AddPath(tempath,FALSE);
				delete tempath;
				xStart += ceil(abc0*settings->fWidScale+CharInterval);
				continue;
			}
			else
			{
				wChar[0] = token[i];
				::BeginPath(hdc);
				::TextOutW(hdc, 0, 0, wChar, 1);
				::EndPath(hdc);
				
				int nNumPts = ::GetPath(hdc, NULL, NULL, 0);
				if (nNumPts == 0)
				{
// 					::SelectObject(hdc,oldFont);
// 					::SetBkMode(hdc, nPrevMode);
//					::DeleteObject(hFont);
// 					delete []str;
					continue;
				}
				
				// Allocate memory to hold points and stroke types from
				// the path.
				LPPOINT lpPoints = new POINT[nNumPts];
				if (lpPoints == NULL)
				{
					::SelectObject(hdc,oldFont);
					::SetBkMode(hdc, nPrevMode);
//					::DeleteObject(hFont);
					delete []str;
					return;
				}
				LPBYTE lpTypes = new BYTE[nNumPts];
				if (lpTypes == NULL)
				{
					::SelectObject(hdc,oldFont);
					::SetBkMode(hdc, nPrevMode);
//					::DeleteObject(hFont);
					delete []str;
					delete [] lpPoints;
					return;
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
					if (!bInitMatrix)
					{
						bInitMatrix = TRUE;
						//先计算矩阵变换	
						//缩放变换 实现字宽的变化
						matrix.Scale(settings->fWidScale,1.0);
						//耸肩变换 
						double angle = settings->fInclineAngle*PI/180;
						if (settings->nInclineType==SHRUGL)
						{
							matrix.Shear(tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
						}
						else if (settings->nInclineType==SHRUGR)
						{
							matrix.Shear(-tan(angle),0.0,MatrixOrderAppend);
						}
						else if (settings->nInclineType==SHRUGU)
						{
							matrix.Shear(0.0,-tan(angle),MatrixOrderAppend);
						}
						else if (settings->nInclineType==SHRUGD)
						{
							matrix.Shear(0.0,tan(angle),MatrixOrderAppend);
						}
						//旋转变换
						matrix.Rotate(-settings->fCharAngle,MatrixOrderAppend);
					}
					charpath.Reset();
					PolyPath(&charpath, lpPoints, lpTypes, nNumPts);
					charpath.Transform(&matrix);
					ABC abc;
					::GetCharABCWidthsW(hdc,token[i],token[i],&abc);
					abc0 = abc.abcA+abc.abcB+abc.abcC;
					g_BufAccel.AddBuf(&charpath,settings,token[i],abc0);
					matrix0.Reset();
					matrix0.Translate(xStart,yStart);
//					matrix0.Multiply(&matrix/*,MatrixOrderAppend*/);
					charpath.Transform(&matrix0);			
					rowtextpath.AddPath(&charpath,FALSE);				
				}
				else
				{
					::SelectObject(hdc,oldFont);
					::SetBkMode(hdc, nPrevMode);
//					::DeleteObject(hFont);
					delete []str;
					delete [] lpPoints;
					delete []lpTypes;
					return;
				}
// 				ABC abc;
// 				::GetCharABCWidthsW(hdc,token[i],token[i],&abc);
// 				//更新xStart
// 				xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*settings->fWidScale+CharInterval);

				xStart += ceil((abc0)*settings->fWidScale+CharInterval);
				// Release the memory we used
				delete [] lpPoints;
				delete [] lpTypes;

			}
			
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
			if (settings->nAlignment&TAH_LEFT)
			{
				;
			}
			else if (settings->nAlignment&TAH_MID)
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
//	::DeleteObject(hFont);

//对齐方式处理
	
	Matrix mat;
	Rect rect;
	textpath.GetBounds(&rect, NULL, NULL);
	
	long yEnd = yStart-LineStep+lineSpacing;
	yStart = 0.0;

	INT orgx, orgy;	
	switch(settings->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yStart;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yStart;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));	
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart;
			mat.Translate(x-orgx,y-orgy);
			mat.RotateAt(-settings->fTextAngle,PointF(orgx,orgy));			
			break;
	}
	textpath.Transform(&mat);
//	bSmooth = FALSE;
//	graphics.SetPageUnit(UnitPixel);
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
		graphics.DrawPath(&pen,&textpath);
	}
	else
	{
		graphics.FillPath(&brush,&textpath);
		graphics.DrawPath(&pen,&textpath);
	}

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextGlRender::CTextGlRender()
{

}

CTextGlRender::~CTextGlRender()
{

}

void CTextGlRender::DrawText(LPCTSTR strText, int x, int y, const TextSettings *settings, COLORREF color, BOOL bSmooth, BOOL bOutLine)
{
	const  wchar_t *text = NULL;
#ifdef _UNICODE
	text = strText;
#else
	std::wstring wstr;
	ConvertCharToWstring(strText,wstr,CP_ACP);
	text = wstr.c_str();
#endif
	if (text==NULL||wcslen(text)<=0)
	{
		return;
	}
	HDC hdc = ::wglGetCurrentDC();
	

	GraphicsPath textpath(FillModeWinding),rowtextpath(FillModeWinding), charpath(FillModeWinding);

	
	//获得精细控制文本所需的相关参数
	HFONT hFont = ::CreateFont(
		settings->fHeight,				   // nHeight
		0,                   // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		0,                 // nWeight
		FALSE,           // bItalic
		FALSE,        // bUnderline
		FALSE,        // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH,  // nPitchAndFamily
		settings->tcFaceName);
	HFONT oldFont = (HFONT)::SelectObject(hdc,hFont);
	TEXTMETRIC textMetric;
	::GetTextMetrics(hdc,&textMetric);
	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*settings->fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*settings->fLineSpacingScale);//行步距
	
	//分行做处理（\n）做隔断符
	int xStart = 0, yStart = 0;
	size_t len = wcslen(text);
	
	wchar_t *str = new wchar_t[len+1];
	if (!str) 
	{
		::SelectObject(hdc,oldFont);	
		::DeleteObject(hFont);
		return;
	}
	Matrix matrix, matrix0;
	//先计算矩阵变换	
	//旋转变换
	matrix.Scale(settings->fWidScale,1.0);
	//耸肩变换 
	double angle = settings->fInclineAngle*PI/180;
	if (settings->nInclineType==SHRUGL)
	{
		matrix.Shear(tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
	}
	else if (settings->nInclineType==SHRUGR)
	{
		matrix.Shear(-tan(angle),0.0,MatrixOrderAppend);
	}
	else if (settings->nInclineType==SHRUGU)
	{
		matrix.Shear(0.0,-tan(angle),MatrixOrderAppend);
	}
	else if (settings->nInclineType==SHRUGD)
	{
		matrix.Shear(0.0,tan(angle),MatrixOrderAppend);
	}
	//缩放变换，实现字宽的变化
	matrix.Rotate(-settings->fCharAngle,MatrixOrderAppend);

	wcscpy(str,text);
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
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
// 				::SelectObject(hdc,oldFont);
// 				::SetBkMode(hdc, nPrevMode);
// 				::DeleteObject(hFont);
// 				delete []str;
// 				return;
				continue;
			}
			
			// Allocate memory to hold points and stroke types from
			// the path.
			LPPOINT lpPoints = new POINT[nNumPts];
			if (lpPoints == NULL)
			{
				::SelectObject(hdc,oldFont);
				::SetBkMode(hdc, nPrevMode);
				::DeleteObject(hFont);
				delete []str;
				return;
			}
			LPBYTE lpTypes = new BYTE[nNumPts];
			if (lpTypes == NULL)
			{
				::SelectObject(hdc,oldFont);
				::SetBkMode(hdc, nPrevMode);
				::DeleteObject(hFont);
				delete []str;
				delete [] lpPoints;
				return;
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
				::SetBkMode(hdc, nPrevMode);
				::DeleteObject(hFont);
				delete []str;
				delete [] lpPoints;
				delete []lpTypes;
				return;
			}
			ABC abc;
			::GetCharABCWidthsW(hdc,token[i],token[i],&abc);
			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*settings->fWidScale+CharInterval);
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
			if (settings->nAlignment&TAH_LEFT)
			{
				;
			}
			else if (settings->nAlignment&TAH_MID)
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
	::DeleteObject(hFont);
	

//对齐方式处理
	
	Matrix mat;
	mat.Scale(1,-1);
	textpath.Transform(&mat);
	Rect rect;
	textpath.GetBounds(&rect, NULL, NULL);	
	long yEnd = - (yStart-LineStep+lineSpacing);
	yStart = 0.0;
	mat.Reset();

	INT orgx, orgy;	
	switch(settings->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));	
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
	}	
	textpath.Transform(&mat);
// 	if (bSmooth)
// 	{
// 		graphics.SetSmoothingMode(SmoothingModeHighQuality);
// 	}
// 	else
// 	{
// 		graphics.SetSmoothingMode(SmoothingModeHighSpeed);	
// 	}
	if (!bOutLine)
	{
		glMatrixMode(GL_MODELVIEW_MATRIX);	
		glPushMatrix();
		
		glTranslated(x,y,0);
//		glEnable(GL_BLEND);
//		glColor3f(GetRValue(color)/255.0,GetGValue(color)/255.0,GetBValue(color)/255.0);
		int nPos = 0;
		textpath.Flatten(NULL,0.1);
		PathData pathData;
		textpath.GetPathData(&pathData);
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
				ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
//				::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
			
			}
			else if(pathData.Types[j]==PathPointTypeCloseSubpath|PathPointTypeLine)
			{
				ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
	//			::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
	//			::glEnd();
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
		glTranslated(x,y,0);
		int nPos = 0;
		textpath.Flatten(NULL,0.1);
		PathData pathData;
		textpath.GetPathData(&pathData);
		glColor3f(GetRValue(color)/255.0,GetGValue(color)/255.0,GetBValue(color)/255.0);
		glLineWidth(1.0f);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			if (pathData.Types[j]==PathPointTypeStart )
			{
				::glBegin(GL_LINE_STRIP);							
				::glVertex3f(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				::glVertex3f(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
			}
			else if(pathData.Types[j]==PathPointTypeCloseSubpath|PathPointTypeLine)
			{
				::glVertex3f(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);		
				::glEnd();
			}
		}
		glPopMatrix();
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextD3DRender::CTextD3DRender()
{
	
}

CTextD3DRender::~CTextD3DRender()
{
	
}

#define VERTEX_DEPTH 0

void CTextD3DRender::DrawText(LPCTSTR strText, int x, int y, HDC hdc, CD3DDrawLineDC *pDC, const TextSettings *settings, COLORREF color, BOOL bSmooth, BOOL bOutLine)
{
	const  wchar_t *text = NULL;
#ifdef _UNICODE
	text = strText;
#else
	std::wstring wstr;
	ConvertCharToWstring(strText,wstr,CP_ACP);
	text = wstr.c_str();
#endif
	if (text==NULL||wcslen(text)<=0)
	{
		return;
	}

	GraphicsPath textpath(FillModeWinding),rowtextpath(FillModeWinding), charpath(FillModeWinding);

	
	//获得精细控制文本所需的相关参数
	HFONT hFont = ::CreateFont(
		settings->fHeight,				   // nHeight
		0,                   // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		0,                 // nWeight
		FALSE,           // bItalic
		FALSE,        // bUnderline
		FALSE,        // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH,  // nPitchAndFamily
		settings->tcFaceName);
	HFONT oldFont = (HFONT)::SelectObject(hdc,hFont);
	TEXTMETRIC textMetric;
	::GetTextMetrics(hdc,&textMetric);
	LONG tmAveCharWidth = textMetric.tmAveCharWidth; //字体的平均宽度
	long CharInterval = tmAveCharWidth*settings->fCharIntervalScale;//字符间距
	LONG lineSpacing = textMetric.tmHeight+textMetric.tmExternalLeading;	 //行之间的默认间隔
	long LineStep = ceil(lineSpacing*settings->fLineSpacingScale);//行步距
	
	//分行做处理（\n）做隔断符
	int xStart = 0, yStart = 0;
	size_t len = wcslen(text);
	
	wchar_t *str = new wchar_t[len+1];
	if (!str) 
	{
		::SelectObject(hdc,oldFont);	
		::DeleteObject(hFont);
		return;
	}
	Matrix matrix, matrix0;
	//先计算矩阵变换	
	//旋转变换
	matrix.Scale(settings->fWidScale,1.0);
	//耸肩变换 
	double angle = settings->fInclineAngle*PI/180;
	if (settings->nInclineType==SHRUGL)
	{
		matrix.Shear(tan(angle),0.0,MatrixOrderAppend); //settings.lfInclineAngle还要做变换，变成错切因子
	}
	else if (settings->nInclineType==SHRUGR)
	{
		matrix.Shear(-tan(angle),0.0,MatrixOrderAppend);
	}
	else if (settings->nInclineType==SHRUGU)
	{
		matrix.Shear(0.0,-tan(angle),MatrixOrderAppend);
	}
	else if (settings->nInclineType==SHRUGD)
	{
		matrix.Shear(0.0,tan(angle),MatrixOrderAppend);
	}
	//缩放变换，实现字宽的变化
	matrix.Rotate(-settings->fCharAngle,MatrixOrderAppend);

	wcscpy(str,text);
	wchar_t *token = wcstok( str, L"\n" );
	wchar_t wChar[2] = {0};
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
// 				::SelectObject(hdc,oldFont);
// 				::SetBkMode(hdc, nPrevMode);
// 				::DeleteObject(hFont);
// 				delete []str;
// 				return;
				continue;
			}
			
			// Allocate memory to hold points and stroke types from
			// the path.
			LPPOINT lpPoints = new POINT[nNumPts];
			if (lpPoints == NULL)
			{
				::SelectObject(hdc,oldFont);
				::SetBkMode(hdc, nPrevMode);
				::DeleteObject(hFont);
				delete []str;
				return;
			}
			LPBYTE lpTypes = new BYTE[nNumPts];
			if (lpTypes == NULL)
			{
				::SelectObject(hdc,oldFont);
				::SetBkMode(hdc, nPrevMode);
				::DeleteObject(hFont);
				delete []str;
				delete [] lpPoints;
				return;
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
				::SetBkMode(hdc, nPrevMode);
				::DeleteObject(hFont);
				delete []str;
				delete [] lpPoints;
				delete []lpTypes;
				return;
			}
			ABC abc;
			::GetCharABCWidthsW(hdc,token[i],token[i],&abc);
			//更新xStart
			xStart += ceil((abc.abcA+abc.abcB+abc.abcC)*settings->fWidScale+CharInterval);
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
			if (settings->nAlignment&TAH_LEFT)
			{
				;
			}
			else if (settings->nAlignment&TAH_MID)
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
	::DeleteObject(hFont);
	

//对齐方式处理
	
	Matrix mat;
	mat.Scale(1,-1);
	textpath.Transform(&mat);
	Rect rect;
	textpath.GetBounds(&rect, NULL, NULL);	
	long yEnd = - (yStart-LineStep+lineSpacing);
	yStart = 0.0;
	mat.Reset();

	INT orgx, orgy;	
	switch(settings->nAlignment)
	{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			orgx = rect.GetLeft();
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			orgx = rect.GetRight();
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_LEFT|TAV_MID:
		case TAV_MID:
			orgx = rect.GetLeft();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID|TAV_MID:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT|TAV_MID:	
			orgx = rect.GetRight();
			orgy = (yEnd+yStart)/2/*(rect.GetTop()+rect.GetBottom())/2*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			orgx = rect.GetLeft();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_MID|TAV_BOTTOM:
			orgx = (rect.GetLeft()+rect.GetRight())/2;
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			orgx = rect.GetRight();
			orgy = yEnd/*rect.GetBottom()*/;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));	
			break;
		default:
			orgx = rect.GetLeft();
			orgy = yStart;
			mat.Translate(-orgx,-orgy);
			mat.RotateAt(settings->fTextAngle,PointF(orgx,orgy));			
			break;
	}	
	textpath.Transform(&mat);
// 	if (bSmooth)
// 	{
// 		graphics.SetSmoothingMode(SmoothingModeHighQuality);
// 	}
// 	else
// 	{
// 		graphics.SetSmoothingMode(SmoothingModeHighSpeed);	
// 	}
	if (!bOutLine)
	{
		D3DXMATRIX matWorld, m1, m2;
		pDC->m_pD3D->m_pD3DDevice9->GetTransform(D3DTS_WORLD,&matWorld);
		
		D3DXMatrixTranslation(&m1,x,y,0);
		D3DXMatrixMultiply(&m2,&m1,&matWorld);
		
		pDC->m_pD3D->m_pD3DDevice9->SetTransform(D3DTS_WORLD,&m2);

//		glEnable(GL_BLEND);
//		glColor3f(GetRValue(color)/255.0,GetGValue(color)/255.0,GetBValue(color)/255.0);

		pDC->SetColor(color);

		int nPos = 0;
		textpath.Flatten(NULL,0.1);
		PathData pathData;
		textpath.GetPathData(&pathData);

		CD3DPolygon ploy;
		ploy.Init(pDC->m_pD3D);

		
		//		glEnable(GL_POLYGON_SMOOTH);
		ploy.BeginPolygon(color);
		for(INT j = 0; j < pathData.Count; ++j)
		{	
			
			if (pathData.Types[j]==PathPointTypeStart )
			{
				ploy.BeginContour();		
				//				::glBegin(GL_POLYGON);			
				//				::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				
			}
			else if (pathData.Types[j]==PathPointTypeLine )
			{
				ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//				::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				
			}
			else if(pathData.Types[j]==PathPointTypeCloseSubpath|PathPointTypeLine)
			{
				ploy.Vertex(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//			::glVertex3i(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
				//			::glEnd();
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

		pDC->m_pD3D->m_pD3DDevice9->SetTransform(D3DTS_WORLD,&m2);

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
			else if(pathData.Types[j]==PathPointTypeCloseSubpath|PathPointTypeLine)
			{
				pDC->LineTo(pathData.Points[j].X,pathData.Points[j].Y,VERTEX_DEPTH);
			}
		}
		pDC->End();

		pDC->m_pD3D->m_pD3DDevice9->SetTransform(D3DTS_WORLD,&matWorld);
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextFastRender::CTextFastRender()
{
	
}

CTextFastRender::~CTextFastRender()
{
	
}



void CTextFastRender::DrawText(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, BOOL bSmooth, BOOL bOutLine)
{
	HFONT font = 
		::CreateFont(
		settings->fHeight,             // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		settings->tcFaceName);
	
	HFONT OldFont = (HFONT)::SelectObject(hdc,font);
	TEXTMETRIC textMetric;
	::GetTextMetrics(hdc,&textMetric);
	::SelectObject(hdc,OldFont);
	::DeleteObject(font);
	int angle = settings->fTextAngle;

	switch(settings->nAlignment)
	{
	case TAH_LEFT:
	case TAH_LEFT|TAV_TOP:
		::SetTextAlign(hdc,TA_LEFT|TA_TOP);
		
		break;
	case TAH_MID:
	case TAH_MID|TAV_TOP:
		::SetTextAlign(hdc,TA_CENTER|TA_TOP);		
		break;
	case TAH_RIGHT:
	case TAH_RIGHT|TAV_TOP:
		::SetTextAlign(hdc,TA_RIGHT|TA_TOP);	
		break;
	case TAH_LEFT|TAV_MID:
	case TAV_MID:
		::SetTextAlign(hdc,TA_LEFT|TA_BASELINE);			
		break;
	case TAH_MID|TAV_MID:
		::SetTextAlign(hdc,TA_CENTER|TA_BASELINE);			
		break;
	case TAH_RIGHT|TAV_MID:	
		::SetTextAlign(hdc,TA_RIGHT|TA_BASELINE);		
		break;
	case TAH_LEFT|TAV_BOTTOM:
	case TAV_BOTTOM:
		::SetTextAlign(hdc,TA_LEFT|TA_BOTTOM);			
		break;
	case TAH_MID|TAV_BOTTOM:
		::SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);		
		break;
	case TAH_RIGHT|TAV_BOTTOM:
		::SetTextAlign(hdc,TA_RIGHT|TA_BOTTOM);	
		break;
	default:
		::SetTextAlign(hdc,TA_LEFT|TA_TOP);		
		break;
	}

font = 
		::CreateFont(
		settings->fHeight,             // nHeight
		textMetric.tmAveCharWidth*settings->fWidScale,                         // nWidth
		angle*10,                        // nEscapement
		angle*10,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		settings->tcFaceName);
	OldFont = (HFONT)::SelectObject(hdc,font);
	int oldMode = ::SetBkMode(hdc, TRANSPARENT);
	::SetTextColor(hdc,color);
	

	::TextOut(hdc,x,y,strText,_tcslen(strText));
	::SelectObject(hdc,OldFont);
	::DeleteObject(font);
	::SetBkMode(hdc,oldMode);
//	::SetGraphicsMode(hdc,oldGraphicsMode);
}
MyNameSpaceEnd