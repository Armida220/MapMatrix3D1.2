// D3DClass.h: interface for the D3DClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_D3DCLASS_H__D1C83447_2FC1_45B4_BC36_6EEEB999492E__INCLUDED_)
#define AFX_D3DCLASS_H__D1C83447_2FC1_45B4_BC36_6EEEB999492E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "d3d9.h"
#include "d3dx9core.h"

class GLUtesselator;

MyNameSpaceBegin

class CD3DTexture;


class EXPORT_SMARTVIEW CD3DWrapper
{
public:
	struct D3DVertex
	{
		float x, y, z; // Position of vertex in 3D space
		DWORD color;   // Color of vertex
	};
	struct D3DTexVertex
	{
		float x, y, z; // Position of vertex in 3D space
		DWORD color;
		float u, v;   // texture coordinates
	};
	
	CD3DWrapper();
	~CD3DWrapper();
	
	BOOL Create(HWND hWnd);
	void OnSize(CSize sz);
	void Destory();
	
	void BeginDrawing();
	void EndDrawing(BOOL bPresent);
	void ClearBuffer(COLORREF color=0, BYTE alpha=0);
	
	BOOL CreateRenderTexture(int cx, int cy);
	void BeginRenderTexture();
	void EndRenderTexture();
	void DisplayTexture();

	int AllocTexture(int cx, int cy, int nType);
	void ReleaseAllocTexture(int index);
	CD3DTexture *GetAllocTexture(int index);

	BOOL CreateTempTextureForCopy(int cx, int cy);
	BOOL CreateDisplayTexture(int cx, int cy);

	BOOL CreateTextureVB();
	
	void BeginLock(CRect rect, BOOL bRead);
	void Scroll(int dx, int dy);
	void BitBltFrom(HBITMAP hBmp, CRect rcBmp);
	void BitBltTo(HBITMAP hBmp, CRect rcBmp);
	void EndLock();
	void SetRenderTexture(LPDIRECT3DTEXTURE9 pText, int cx, int cy);
	HRESULT SetRenderState(D3DRENDERSTATETYPE state, int value);

	void ResetDevice(BOOL bForce=FALSE);

	void SetGDIPresent(BOOL bGDIPresent);
	BOOL IsGDIPresent()const;
	void SetSize(CSize *sz=NULL);

protected:

	HBITMAP CreatePresentBmp(int w, int h);
	HDC CreatePresentDC();

protected:
	//用GDI代替d3d作最后的显示
	BOOL m_bGDIPresent;
	HBITMAP m_hBmpPresent;
	HDC m_hDCPresent;
	
public:
	
	HWND m_hWnd;
	CSize m_szWnd;
	CSize m_szViewPort;
	LPDIRECT3D9 m_pD3D9;
	LPDIRECT3DDEVICE9 m_pD3DDevice9;
	LPDIRECT3DSURFACE9 m_pBackSurface;
	LPDIRECT3DSURFACE9 m_pOffSurface;
	D3DLOCKED_RECT m_rcLocked;
	
	LPDIRECT3DVERTEXBUFFER9 m_pPointList_VB;
	LPDIRECT3DVERTEXBUFFER9 m_pLineStrip_VB;
	LPDIRECT3DVERTEXBUFFER9 m_pTriangleList_VB;
	LPDIRECT3DVERTEXBUFFER9 m_pTexture_VB;

	D3DVertex *m_pVertexForTriangleList;
	int m_nVertexForTriangleList;
	
	LPD3DXLINE m_pLine;
	
	D3DPRESENT_PARAMETERS m_D3DPresentParams;
	
	CSize  m_szRenderTexture, m_szDisplayTexture;
	LPDIRECT3DTEXTURE9 m_pRenderTexture, m_pDisplayTexture;
	LPDIRECT3DSURFACE9 m_pRenderTextureSurface;

	CSize  m_szTempTexture;
	LPDIRECT3DTEXTURE9 m_pTempTexture;

	int m_nBeginScene;

	CArray<CD3DTexture*,CD3DTexture*> m_arrPAllocTextures;
};


class EXPORT_SMARTVIEW CD3DTexture
{
public:
	enum TYPE
	{
		typeRender = 0,      // Render Target
		typeDynamicVideo = 1,  // Dynamic and placed in Video Memory
		typeDynamicSysMem = 2  // Dynamic and placed in System Memory
	};
	CD3DTexture();
	~CD3DTexture();
	
	BOOL Create(CD3DWrapper *pD3D, int cx, int cy, int nType);
	void Delete();
	BOOL SetBitmap(HBITMAP hBmp,int x,int y,
				  int xTar,int yTar,int nTarWidth,int nTarHeight,BOOL bSetAlpha);
	BOOL SetBitmap(HBITMAP hBmp,BOOL bSetAlpha);
	HBITMAP GetBitmap();
	HBITMAP GetBitmapForRenderTexture();
	void Display(double x0[4], double y0[4], double x[4], double y[4]);
	void Display(double x[4], double y[4]);

	void Cutto(CD3DTexture *p);
	void CopyFromRenderTexture(LPDIRECT3DTEXTURE9 pTexture);
	BOOL CopyToBitmap(HBITMAP hBmp);
	
	CD3DWrapper *m_pD3D;
	LPDIRECT3DTEXTURE9 m_pTexture;
	CSize m_size;
	BOOL m_bfilled;
	int m_nType;
};



class CD3DPolygon
{
public:
	CD3DPolygon();
	~CD3DPolygon();
	void Init(CD3DWrapper *p);
	void BeginPolygon(COLORREF color);
	void BeginContour();
	void Vertex(double x, double y, double z);
	void EndPolygon();
	void EndContour();

	static BOOL m_bSmoothLine;
	
protected:
	static void DrawCells();
	
	static void CALLBACK beginCallback(int which) ;
	static void CALLBACK endCallback(void) ;
	static void CALLBACK errorCallback(int errorCode) ;
	static void CALLBACK vertexCallback(void *vertex) ;
	static void CALLBACK combineCallback(double coords[3], 
	double *vertex_data[4],
	double weight[4], double **dataOut ) ;
	
	GLUtesselator *m_pObj;
	static CD3DWrapper *m_pD3D;
	
	static CPtrList gm_VertexPtrList;
	
	static DWORD m_color;
	
	static int m_nTriType;
	
	static CArray<CD3DWrapper::D3DVertex,CD3DWrapper::D3DVertex> m_arrPts;
};

MyNameSpaceEnd

#endif // !defined(AFX_D3DCLASS_H__D1C83447_2FC1_45B4_BC36_6EEEB999492E__INCLUDED_)
