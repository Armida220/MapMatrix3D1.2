// CursorLayer.h: interface for the CCursorLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURSORLAYER_H__9DD9C14C_F334_47C2_A2A6_64E841B7E403__INCLUDED_)
#define AFX_CURSORLAYER_H__9DD9C14C_F334_47C2_A2A6_64E841B7E403__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RealtimeDrawingLayer.h"
#include "CursorFile.h"

MyNameSpaceBegin



class CColorCursor  
{
public:
	static void GetMaskBitmaps(HBITMAP hSourceBitmap,COLORREF clrTransparent,
		HBITMAP &hAndMaskBitmap,HBITMAP &hXorMaskBitmap);
	static HCURSOR CreateCursorFromBitmap(HBITMAP hSourceBitmap,COLORREF clrTransparent,
		DWORD   xHotspot,DWORD   yHotspot);
	CColorCursor();
	virtual ~CColorCursor();
	
};

class EXPORT_SMARTVIEW CCursorLayer : public CRealtimeDrawingLayer  
{
public:
	CCursorLayer();
	virtual ~CCursorLayer();
	
	virtual BOOL Load(const char *filename);

	void SetPos(float x, float y);
	float GetPos(int i);
	int SetCursorType( int type );
	int	GetCursorType();
	int	SetSelMarkWid(int ap);
	int	GetSelMarkWid();
	void EnableSelMarkVisible(BOOL bEnable);
	void EnableCrossVisible(BOOL bEnable);
	inline BOOL IsSelMarkVisible(){ return m_bSelMarkVisible; }
	inline BOOL IsCrossVisible(){ return m_bCrossVisible; }
	COLORREF GetCursorColor();
	COLORREF SetCursorColor( COLORREF color );
	int	GetCursorCount();
	CSize GetCursorSize();

	virtual void Erase();
	
	//绘制
	virtual void Draw();
	void Reset();

	//系统光标的处理函数
	void CreateSysCursor();
	void DrawSysCursor();
	HCURSOR GetSysCursor();

protected:
	void CreateCurosrBuffer();
	void SaveBmp();
	void RestoreBmp();
	
protected:
	BOOL m_bSelMarkVisible;
	BOOL m_bCrossVisible;

	int	m_nSelMarkWid;
	COLORREF m_clrCursor;
	float m_fCursorX, m_fCursorY;
	
	CCursorFile m_fileCursor;

	GrBuffer2d m_cursorBuf;

protected:
	//系统光标
	HCURSOR m_hCurCursor, m_hUseCursor;

public:
	BOOL m_bUseSaveBmp;

};


MyNameSpaceEnd

#endif // !defined(AFX_CURSORLAYER_H__9DD9C14C_F334_47C2_A2A6_64E841B7E403__INCLUDED_)
