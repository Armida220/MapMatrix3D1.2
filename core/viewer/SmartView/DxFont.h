// DxFont.h: interface for the CDxFont class.
// 实现OpenGL中文字的绘制
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXFONT_H__6A2FC517_8929_45EB_BEEF_30CFEA0AA71F__INCLUDED_)
#define AFX_DXFONT_H__6A2FC517_8929_45EB_BEEF_30CFEA0AA71F__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DX_FONT_SIZE		1024
#define DX_FONT_ASCII		256
#define DX_FONT_FLAG_MAX	0x80000000
#define DX_FONT_FLAG_INDEX_MAX	30
#define DX_FONT_FLAG_INDEX_MIN	16

class GLUtesselator;
MyNameSpaceBegin
class EXPORT_SMARTVIEW CDxFont  
{
public:
	//用来缓存字符的绘制
	struct CharBuf
	{
		int ch;
		int nGLListIdx;
		TCHAR font[64];
		DWORD charset;
		DWORD bItalic;
		DWORD bUnderline;
		DWORD bStrikeOut;
		DWORD align;
	};
	CDxFont();
	virtual ~CDxFont();

public:
	void SetBufSize(int size);
	BOOL SetFont(TCHAR *chName,DWORD charset,DWORD bItalic,DWORD bUnderline,DWORD bStrikeOut,DWORD align);
	void Render(LPCTSTR text,double lfX,double lfY,double lfSize,double lfAngle,int aligntype);
	void SetColor(COLORREF color) { m_color=color; };
protected:
	int FindChar(DWORD dwText);
	void DrawChar(HDC hdc, BOOL bPlotSpline, char a1, char a2, double kx, double ky);

private:
	int		m_nListBase;

	HFONT	m_hFont;
	HFONT	m_hFontOld;

	CharBuf m_CurChar;
	CArray<CharBuf,CharBuf> m_arrBufs;
	CArray<int,int> m_arrUsedFlag;

	COLORREF m_color;
};



class EXPORT_SMARTVIEW CDxPolygon
{
public:
	CDxPolygon();
	~CDxPolygon();
	void BeginPolygon(COLORREF color);
	void BeginContour();
	void Vertex(double x, double y, double z);
	void EndPolygon();
	void EndContour();
	
protected:
	static void CALLBACK beginCallback(int which) ;
	static void CALLBACK endCallback(void) ;
	static void CALLBACK errorCallback(int errorCode) ;
	static void CALLBACK vertexCallback(void *vertex) ;
	static void CALLBACK combineCallback(double coords[3], 
		double *vertex_data[4],
		double weight[4], double **dataOut ) ;

	static CPtrList gm_VertexPtrList;

	GLUtesselator *m_pObj;
};
MyNameSpaceEnd

#endif // !defined(AFX_DXFONT_H__6A2FC517_8929_45EB_BEEF_30CFEA0AA71F__INCLUDED_)
