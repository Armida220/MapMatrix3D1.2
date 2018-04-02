// PlotWChar.h: interface for the CPlotWChar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOTWCHAR_H__B8812FB6_8FCC_4D5F_8F04_21EE9BE23B29__INCLUDED_)
#define AFX_PLOTWCHAR_H__B8812FB6_8FCC_4D5F_8F04_21EE9BE23B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrBuffer.h"
#include "DrawLineDC.h"
#include <gdiplus.h>
using namespace Gdiplus;

MyNameSpaceBegin

class EXPORT_SMARTVIEW CMemDc
{
public:
	CMemDc()
	{
		hdc = NULL;
	}
	~CMemDc()
	{
		if(hdc)
			::DeleteDC(hdc);
	}
	HDC GetHdc()
	{
		if(hdc==NULL)
			hdc = ::CreateCompatibleDC(NULL);
		return hdc;
	}
private:
	HDC hdc;
};

extern EXPORT_SMARTVIEW CMemDc g_Memdc;
class GrBuffer;
// long	lHeight;				//�ַ��߶�
// double	lfWidScale;				//�ַ������ƽ����ȵı�ֵ
// double	lfCharIntervalScale;	//�ַ������ƽ����ȵı�ֵ
// double	lfLineSpacingScale;			//�м�౶�� 
// double	lfTextAngle;			//�ı�ˮƽ������x��ļнǣ���ʱ�룩
// double	lfCharAngle;			//�����ַ�����ת�Ƕȣ���ʱ�룩һ��Ϊ90�ȣ�ʵ�������ߴ�ֱ�ֲ��ı�
// int		nAlignment;				//���뷽ʽ9��
// int     nInclineType;			//�ʼ緽ʽ5��
// double	lfInclineAngle;			//Ĭ��15�ȣ���ʱ�����û�����
// 	wchar_t	tcFaceName[LF_FACESIZE];//��������

class EXPORT_SMARTVIEW CGetCharInfo
{
public:	
	CGetCharInfo();
	virtual ~CGetCharInfo();	
	void SetSettings(const TextSettings *settings);
	void SetWChar(WCHAR wchar);
	BOOL Plot(CArray<GrVertex,GrVertex> *pts, double *nextCharPosDis, double *Width, double *Height);
private:
	WCHAR	m_wChar; 
	HFONT m_hFont;
	TextSettings m_settings;
};

class EXPORT_SMARTVIEW CGetTextInfo
{
public:	
	CGetTextInfo();
	virtual ~CGetTextInfo();

	void SetSettings(const TextSettings *settings,BOOL GetEnv = TRUE);
	void SetString(LPCTSTR pStr);
	BOOL Plot(CArray<GrVertex,GrVertex> *pts/*, double ex[4],double ey[4]*/);
	BOOL GetEnvlope(double ex[4],double ey[4]);
private:
	CString m_strText;
	HFONT m_hFont;
	TextSettings m_settings;
};


struct CharItem
{
	CharItem():path(FillModeWinding){
		hBmp = NULL;
	}
	~CharItem(){
		if( hBmp )::DeleteObject(hBmp);
	}
	WCHAR ch;
	GraphicsPath path;
	RectF rcBound;
	ABC abc;

	//���ƺõ�λͼ����
	COLORREF color, bkcolor;
	HBITMAP hBmp;
	int nBmpWid, nBmpHei;
	int x0, y0;
};

struct FontLibItem
{
	HFONT hFont;
	TextSettings fontSetting;
	TEXTMETRICW textMetric;

	CArray<CharItem*,CharItem*> arrCharItems;

	FontLibItem()
	{
		hFont = 0;
		memset(&fontSetting,0,sizeof(fontSetting));
		memset(&textMetric,0,sizeof(textMetric));
	}
	~FontLibItem()
	{
		Destroy();
	}
	void Destroy()
	{
		RemoveAll();
		DeleteObject(hFont);
	}
	void RemoveAll()
	{
		for( int i=arrCharItems.GetSize()-1; i>=0; i--)
		{
			delete arrCharItems[i];
		}
		arrCharItems.RemoveAll();
	}

	CharItem* FindItem(int c)
	{
		for( int i=arrCharItems.GetSize()-1; i>=0; i--)
		{
			CharItem *pItem = arrCharItems[i];
			if( pItem->ch==c )
			{
				arrCharItems.RemoveAt(i);
				arrCharItems.Add(pItem);

				return pItem;
			}
		}
		return NULL;
	}

	void AddItem(CharItem *pItem)
	{
		arrCharItems.Add(pItem);

		if( arrCharItems.GetSize()>50 )
		{
			pItem = arrCharItems.GetAt(0);
			arrCharItems.RemoveAt(0);

			delete pItem;
		}
	}
};


class EXPORT_SMARTVIEW CFontLib
{
public:
	CFontLib();
	~CFontLib();

	void Clear();
	void ClearDrawGraphics();

	FontLibItem* SetSettings(const TextSettings *settings);
	
	//x,y���ڴ�������ϵ�������GDI��ͼ����ôy�������£�����y�ᳯ��
	GraphicsPath* Plot(LPCTSTR text, int x, int y, BOOL bInverseY);

	//��õ�������ڴ������ϵ
	BOOL GetEnvlope(LPCTSTR text, double ex[4],double ey[4]);

	//��õ�CArray<GrVertex,GrVertex> *pts���ڴ������ϵ
	BOOL Plot(WCHAR wchar, CArray<GrVertex,GrVertex> *pts, double *nextCharPosDis, double *Width, double *Height);
	BOOL Plot(LPCTSTR text, CArray<GrVertex,GrVertex> *pts);

	//�㷨������GraphicsPath* Plot(...)һ�£���ͬ���������ַ���ԭ�㣨����(0,0)�������ַ�������path���Ӷ��õ������ַ��Ļ���ԭ�㣻���ڴ�ɢ���֣�
	BOOL PlotOrigins(LPCTSTR text, CArray<PT_3D,PT_3D> *pts);

	//x,y���ڴ�������ϵ�������GDI��ͼ����ôy�������£�����y�ᳯ��
	void DrawText_GDIPlus(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, COLORREF bkColor, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);
	void DrawText_GL(LPCTSTR strText, int x, int y, const TextSettings *settings, COLORREF color, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);
	void DrawText_D3D(LPCTSTR strText, int x, int y, HDC hdc, CD3DDrawLineDC *pDC, const TextSettings *settings, COLORREF color, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);

	BOOL EnableTextBmpCache(BOOL bEnable);

protected:
	//CharItem �д洢�� charPath ��������ϵһ�£����곯�ϣ�ԭ��(0,0)���ַ������½�
	CharItem *GetCharItem(int ch);

	FontLibItem *FindFont(const TextSettings *settings);
	FontLibItem *CreateFont(const TextSettings *settings);
	CharItem* CreateChar(FontLibItem *pFont, int ch);
	void CreateCharBmp(FontLibItem *pFont, CharItem* pChar, COLORREF color, COLORREF bkColor);

	void DrawTextWithBmp(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, COLORREF bkColor);
	CPoint GetGDIPlotOrigin(LPCTSTR text, int x, int y);

	HDC GetMemDC();
	Graphics& GetGraphics();
	Graphics& GetDrawGraphics(HDC hdc);

	BOOL IsSame(const TextSettings *s1, const TextSettings *s2);

	CArray<FontLibItem*,FontLibItem*> m_arrItems;
	FontLibItem* m_pCurFont;

	HDC m_hdc;

	HDC m_hDrawDC;
	Graphics *m_pDrawGraphics;

	BITMAPINFO *m_pBmpInfo;

public:
	BOOL m_bUseTextBmpCache;
};

extern EXPORT_SMARTVIEW CFontLib g_Fontlib;

EXPORT_SMARTVIEW void ClearFontLib();

MyNameSpaceEnd

#endif // !defined(AFX_PLOTWCHAR_H__B8812FB6_8FCC_4D5F_8F04_21EE9BE23B29__INCLUDED_)
