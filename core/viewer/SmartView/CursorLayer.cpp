// CursorLayer.cpp: implementation of the CCursorLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CursorLayer.h"
#include "resource.h"
#include "GrElementList.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern BOOL LoadMyString(CString &str, UINT uID);

MyNameSpaceBegin



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CColorCursor::CColorCursor()
{

}

CColorCursor::~CColorCursor()
{

}

//////////////////////////////////////////////////////////////////////
//  CreateCursorFromBitmap 
//  Function to create a cursor from HBITMAP. 
//  Pass bitmaps having standard cursor sizes like 16*16, 32*32...
//////////////////////////////////////////////////////////////////////
HCURSOR CColorCursor::CreateCursorFromBitmap(HBITMAP hSourceBitmap,
											 COLORREF clrTransparent,
											 DWORD   xHotspot,DWORD   yHotspot)
{
	HCURSOR hRetCursor = NULL;

	do
	{
		if(NULL == hSourceBitmap)
		{
			break;
		}

		//Create the AND and XOR masks for the bitmap
		HBITMAP hAndMask = NULL;
		HBITMAP hXorMask = NULL;
		GetMaskBitmaps(hSourceBitmap,clrTransparent,hAndMask,hXorMask);
		if(NULL == hAndMask || NULL == hXorMask)
		{
			::DeleteObject(hAndMask);
			::DeleteObject(hXorMask);
			break;
		}

		//Create the cursor using the masks and the hotspot values provided
		ICONINFO iconinfo = {0};
		iconinfo.fIcon		= FALSE;
		iconinfo.xHotspot	= xHotspot;
		iconinfo.yHotspot	= yHotspot;
		iconinfo.hbmMask	= hAndMask;
		iconinfo.hbmColor	= hXorMask;

		hRetCursor = ::CreateIconIndirect(&iconinfo);

		::DeleteObject(hAndMask);
		::DeleteObject(hXorMask);

	}
	while(0);

	return hRetCursor;
}


//////////////////////////////////////////////////////////////////////
//  GetMaskBitmaps
//  Function to AND and XOR masks for a cursor from a HBITMAP. 
//////////////////////////////////////////////////////////////////////
void CColorCursor::GetMaskBitmaps(HBITMAP hSourceBitmap, COLORREF clrTransparent, 
								  HBITMAP &hAndMaskBitmap, HBITMAP &hXorMaskBitmap)
{
	HDC hDC					= ::GetDC(NULL);
	HDC hMainDC				= ::CreateCompatibleDC(hDC); 
	HDC hAndMaskDC			= ::CreateCompatibleDC(hDC); 
	HDC hXorMaskDC			= ::CreateCompatibleDC(hDC); 

	//Get the dimensions of the source bitmap
	BITMAP bm;
	::GetObject(hSourceBitmap,sizeof(BITMAP),&bm);

	
	hAndMaskBitmap	= ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);
	hXorMaskBitmap	= ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);

	//Select the bitmaps to DC
	HBITMAP hOldMainBitmap = (HBITMAP)::SelectObject(hMainDC,hSourceBitmap);
	HBITMAP hOldAndMaskBitmap	= (HBITMAP)::SelectObject(hAndMaskDC,hAndMaskBitmap);
	HBITMAP hOldXorMaskBitmap	= (HBITMAP)::SelectObject(hXorMaskDC,hXorMaskBitmap);

	//Scan each pixel of the souce bitmap and create the masks
	COLORREF MainBitPixel;
	for(int x=0;x<bm.bmWidth;++x)
	{
		for(int y=0;y<bm.bmHeight;++y)
		{
			MainBitPixel = ::GetPixel(hMainDC,x,y);
			if(MainBitPixel == clrTransparent)
			{
				::SetPixel(hAndMaskDC,x,y,RGB(255,255,255));
				::SetPixel(hXorMaskDC,x,y,RGB(0,0,0));
			}
			else
			{
				::SetPixel(hAndMaskDC,x,y,RGB(0,0,0));
				::SetPixel(hXorMaskDC,x,y,MainBitPixel);
			}
		}
	}
	
	::SelectObject(hMainDC,hOldMainBitmap);
	::SelectObject(hAndMaskDC,hOldAndMaskBitmap);
	::SelectObject(hXorMaskDC,hOldXorMaskBitmap);

	::DeleteDC(hXorMaskDC);
	::DeleteDC(hAndMaskDC);
	::DeleteDC(hMainDC);

	::ReleaseDC(NULL,hDC);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCursorLayer::CCursorLayer()
{
	m_bSelMarkVisible = TRUE;
	m_bCrossVisible = TRUE;
	m_clrCursor = RGB(255,255,255);
	m_nSelMarkWid = 10;
	m_fCursorX = m_fCursorY = 0;

	m_hCurCursor = m_hUseCursor = NULL;

	m_bUseSaveBmp = TRUE;
}

CCursorLayer::~CCursorLayer()
{
	if( m_hCurCursor )::DestroyCursor(m_hCurCursor);
	if( m_hUseCursor && m_hUseCursor!=m_hCurCursor )
		::DestroyCursor(m_hUseCursor);
}



BOOL CCursorLayer::Load(const char *filename)
{
	m_nSelMarkWid  = 10;

	if( !m_fileCursor.LoadCursor(filename) )
	{
		CString str1,str2;
		LoadMyString(str1,IDS_LOADCURSOR_ERR);
		str2.Format(str1,(LPCTSTR)ConvertCharToTChar(filename));
		AfxMessageBox(str2);
		return FALSE;
	}

	m_fileCursor.SetType(0);
	CSize szCursor = GetCursorSize();

	m_clrCursor	= RGB(255,255,255);
	m_bSelMarkVisible = FALSE;

	CreateCurosrBuffer();
	
	return TRUE;
}


int CCursorLayer::GetCursorCount()
{
	return m_fileCursor.GetTypeSum();
}

COLORREF CCursorLayer::GetCursorColor()
{
	return m_clrCursor;
}

int CCursorLayer::SetSelMarkWid(int wid)
{
	int old = m_nSelMarkWid;
	CSize szCursor = GetCursorSize();

	m_nSelMarkWid = wid;
	if( m_nSelMarkWid>szCursor.cy/2 ) m_nSelMarkWid = szCursor.cy/2;

	if( m_bSelMarkVisible )CreateCurosrBuffer();

	return old;
}

int CCursorLayer::GetSelMarkWid()
{
	return m_nSelMarkWid;
}

int CCursorLayer::SetCursorType(int type)
{
	int old = m_fileCursor.GetType();
	if( old==type )
		return old;
	
	m_fileCursor.SetType(type);
	
	CreateCurosrBuffer();

	return old;
}

int CCursorLayer::GetCursorType()
{
	return m_fileCursor.GetType();
}


CSize CCursorLayer::GetCursorSize()
{
	return m_fileCursor.GetSize();
}


void CCursorLayer::EnableSelMarkVisible(BOOL bEnable)
{
	if( m_bSelMarkVisible==bEnable )
		return;

	m_bSelMarkVisible = bEnable;
	CreateCurosrBuffer();
}

void CCursorLayer::EnableCrossVisible(BOOL bEnable)
{
	if( m_bCrossVisible==bEnable )
		return;
	
	m_bCrossVisible = bEnable;
	CreateCurosrBuffer();
}


COLORREF CCursorLayer::SetCursorColor(COLORREF color)
{
	COLORREF old = m_clrCursor;
	m_clrCursor = color;

	m_cursorBuf.SetAllColor(m_clrCursor);

	return old;
}


void CCursorLayer::SetPos(float x, float y)
{
	m_fCursorX = x; m_fCursorY = y;
}


float CCursorLayer::GetPos(int i)
{
	return (i==0?m_fCursorX:m_fCursorY);
}


void CCursorLayer::CreateCurosrBuffer()
{
	m_cursorBuf.DeleteAll();

	int i, j, nCount = 0;
	char *pBits = m_fileCursor.GetTypeData();
	if( !pBits )return;

	CSize szCursor = GetCursorSize();
	int ox = szCursor.cx/2, oy = szCursor.cy/2;	
	int wid = GetSelMarkWid();

	if( m_bCrossVisible )
	{
		m_cursorBuf.BeginPointString(m_clrCursor,0,0,FALSE);
		for( i=0;i<szCursor.cx;i++)
		{
			for( j=0;j<szCursor.cy;j++)
			{
				if( nCount>7 )
				{
					nCount=0;		
					pBits++;
				}
				if( (*pBits)&(1<<nCount) )
					m_cursorBuf.PointString(&PT_2D(i-ox,j-oy),0);
				nCount++;
			}
			pBits++;
			nCount = 0;
		}
		m_cursorBuf.End();
	}

	if( m_bSelMarkVisible )
	{
		ox = 0, oy = 0;
		m_cursorBuf.BeginLineString(m_clrCursor,1,FALSE);
		m_cursorBuf.MoveTo(&PT_2D(ox-wid,oy-wid));
		m_cursorBuf.LineTo(&PT_2D(ox+wid,oy-wid));
		m_cursorBuf.LineTo(&PT_2D(ox+wid,oy+wid));
		m_cursorBuf.LineTo(&PT_2D(ox-wid,oy+wid));
		m_cursorBuf.LineTo(&PT_2D(ox-wid,oy-wid));
		m_cursorBuf.End(FALSE);
	}

	CreateSysCursor();
}

void CCursorLayer::SaveBmp()
{
	if( !m_bUseSaveBmp )
		return;

	CSize szCursor = GetCursorSize();
	CRect rc = CRect(-szCursor.cx/2-1,-szCursor.cy/2-1,szCursor.cx/2+1,szCursor.cy/2+1);
	rc.OffsetRect(PIXEL(m_fCursorX),PIXEL(m_fCursorY));

	CRect rcView;
	m_pContext->GetViewRect(rcView);
	CRect retRect;
	if (!retRect.IntersectRect(&rc,&rcView))
	{
		rc.SetRectEmpty();
	}
	/*rc &= rcView;*/
	m_pContext->SaveBmp(m_nSaveDataIdx,rc);
}


void CCursorLayer::RestoreBmp()
{
	if( !m_bUseSaveBmp )
		return;
	
	m_pContext->RestoreBmp(m_nSaveDataIdx);
}

void CCursorLayer::Erase()
{
	if( !(GetAttribute()&CDrawingLayer::attrVisible) )
		return;

	RestoreBmp();
}

void CCursorLayer::Draw()
{
	if( !(GetAttribute()&CDrawingLayer::attrVisible) )
		return;

	SaveBmp();

	CCoordSys *pOld = m_pContext->GetCoordSys();
	m_pContext->SetCoordSys(NULL);
	CRect rcOld;
	m_pContext->GetViewRect(rcOld);
	CSize szDC = m_pContext->GetDCSize();
	m_pContext->SetViewRect(CRect(0,0,szDC.cx,szDC.cy));

	m_cursorBuf.ZoomPointSize(0,0);

	m_cursorBuf.Move(m_fCursorX,m_fCursorY);
	m_pContext->DrawGrBuffer2d(&GrElementList(&m_cursorBuf),CDrawingContext::modeNormal);
	m_cursorBuf.Move(-m_fCursorX,-m_fCursorY);

	m_pContext->SetViewRect(rcOld);
	m_pContext->SetCoordSys(pOld);
}


void CCursorLayer::Reset()
{
	m_pContext->ResetBmp(m_nSaveDataIdx);
}

void CCursorLayer::CreateSysCursor()
{
	if( !m_fileCursor.IsValid() ) return;
	
	char *pBits = m_fileCursor.GetTypeData();
	if( !pBits )return;
	
	CSize szCursor = GetCursorSize();
	
	//draw cursor bitmap
	HDC hMainDC = ::GetDC(NULL);
	HDC hTempDC = ::CreateCompatibleDC(hMainDC); 
	HBITMAP hCursorBitmap = ::CreateCompatibleBitmap(hMainDC,szCursor.cx,szCursor.cy);
	HBITMAP hOldBitmap	= (HBITMAP)::SelectObject(hTempDC,hCursorBitmap);
	::FillRect(hTempDC,&CRect(0,0,szCursor.cx,szCursor.cy),(HBRUSH)CBrush((COLORREF)0));
	
	char ctemp;
	int bitcount,nBitWid = ((szCursor.cx+7)>>3);
	COLORREF color = GetCursorColor();
	if( color==0 )color = 1;
	
	//draw curosr
	if( m_bCrossVisible )
	{
		for(int i=0;i<szCursor.cy;i++)
		{
			for(int j=0;j<nBitWid;j++)
			{
				bitcount=0;
				while(bitcount<8)
				{
					ctemp=pBits[i*nBitWid+j]<<7-bitcount;
					if((int)ctemp<0)
						::SetPixel(hTempDC,i,8*j+bitcount,color);
					bitcount++;
				}
			}
		}
	}
	
	// Draw select rect
	if( m_nSelMarkWid>0&&m_bSelMarkVisible )
	{
		HPEN hPen = ::CreatePen(0,0,color);
		HPEN hOldPen = (HPEN)::SelectObject(hTempDC,hPen);
		int sx = szCursor.cx/2-m_nSelMarkWid, sy = szCursor.cy/2-m_nSelMarkWid;
		int ex = sx + m_nSelMarkWid*2, ey = sy + m_nSelMarkWid*2;
		if( sx<0 )sx = 0;
		if( sy<0 )sy = 0;
		if( ex>=szCursor.cx )ex = szCursor.cx-1;
		if( ey>=szCursor.cy )ey = szCursor.cy-1;
		
		::MoveToEx(hTempDC,sx,sy,NULL);
		::LineTo(hTempDC,ex,sy);
		::LineTo(hTempDC,ex,ey);
		::LineTo(hTempDC,sx,ey);
		::LineTo(hTempDC,sx,sy);
		
		::SelectObject(hTempDC,hOldPen);
		::DeleteObject(hPen);
	}
	
	::SelectObject(hTempDC,hOldBitmap);
	::DeleteDC(hTempDC);
	::ReleaseDC(NULL,hMainDC);
	
	//create color cursor from bitmap
	if( m_hCurCursor && m_hCurCursor!=m_hUseCursor )::DestroyCursor(m_hCurCursor);
	m_hCurCursor = CColorCursor::CreateCursorFromBitmap(hCursorBitmap,RGB(0,0,0),szCursor.cx/2,szCursor.cy/2);	
	::DeleteObject(hCursorBitmap);
}


void CCursorLayer::DrawSysCursor()
{
	if( m_hUseCursor && m_hUseCursor!=m_hCurCursor )
		::DestroyCursor(m_hUseCursor);
	
	m_hUseCursor = m_hCurCursor;
	::SetCursor(m_hUseCursor);
}

HCURSOR CCursorLayer::GetSysCursor()
{
	return m_hCurCursor;
}

MyNameSpaceEnd