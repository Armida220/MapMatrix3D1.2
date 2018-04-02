// DrawingContext.h: interface for the CDrawingContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWINGCONTEXT_H__0090C03A_CEC9_4CA7_8D9F_FFC6AAC13700__INCLUDED_)
#define AFX_DRAWINGCONTEXT_H__0090C03A_CEC9_4CA7_8D9F_FFC6AAC13700__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CoordSys.h"
#include "GrBuffer.h"
#include "GrBuffer2d.h"
#include "DxFont.h"
#include "GLDynamicTexture.h"
#include "D3dClass.h"
#include "DrawLineDC.h"

// RGB Channel Mask
#define	RGBMASK_RED					0x00000001
#define	RGBMASK_GREEN				0x00000002
#define	RGBMASK_BLUE				0x00000004
#define RGBMASK_ALL					(RGBMASK_RED|RGBMASK_GREEN|RGBMASK_BLUE)

class tex_image;

MyNameSpaceBegin

typedef struct EXPORT_SMARTVIEW MarkPtInfor
{
	MarkPtInfor(){}
	MarkPtInfor(int t, COLORREF c, float w){
		type = t; clr = c; wid = w;
	}
	int type;
	COLORREF clr;
	float wid;
}MarkPtInfor;

typedef CArray<MarkPtInfor,MarkPtInfor> CMarkPtInforArray;

class EXPORT_SMARTVIEW CMarkPtInfor
{
public:
	CMarkPtInfor();
	~CMarkPtInfor();
	void Add(const MarkPtInfor &infor);
	void Add(int t, COLORREF c, float w);
	COLORREF GetCol(int type);
	float GetWid(int type);
protected:
	CMarkPtInforArray arr;
};


struct ImgSaveData
{
public:
	enum
	{
		typeNone = 0,
		typeGDI = 1,
		typeGL = 2
	};
	ImgSaveData():rcSaveRect(0,0,0,0),
		nSaveRectType(0),pRectData(0)
	{
	}
	CRect rcSaveRect;
	int	  nSaveRectType;
	BYTE *pRectData;
	
	CDC dcMemRect;
	CBitmap bmpRect;
};

class GrElementList;

//绘图环境
//提供一个抽象的绘图环境的概念，提供统一的绘图接口，使得使用者不需要关心具体的绘图接口
//（例如GDI、OpenGL）、坐标系（正射、中心投影等等）、显示模式（平面、立体等等），
// 从而减少绘图数据和绘图环境的耦合
class EXPORT_SMARTVIEW CDrawingContext : public CObject
{
	DECLARE_DYNCREATE(CDrawingContext)
public:
	enum DrawMode
	{
		modeDynamic = 1,
		modeNormal = 2
	};
	CDrawingContext();
	virtual ~CDrawingContext();

	virtual BOOL CreateContext(HDC hDC);
	virtual void SetDCSize(CSize szDC);

	BOOL CreateD3DContext(HWND hWnd);
	void SetD3DGDIPresent(BOOL bGDIPresent);

	virtual HDC GetContext();
	CSize GetDCSize();

	//设置绘图区域；rcView 应是 szDC 的一部分
	virtual void SetViewRect(CRect rcView);
	virtual void GetViewRect(CRect& rcView);

	void EnableMonocolor(BOOL bEnable, COLORREF clr){
		m_bMonoMode = bEnable;
		m_monoColor = clr;
	}

	void GetMonoColor(BOOL& bEnable,COLORREF& clr){
		bEnable = m_bMonoMode;
		clr = m_monoColor;
	}

	void SetRGBMask(DWORD mask){
		m_nRGBMask = mask;
	}
	DWORD GetRGBMask(){
		return m_nRGBMask;
	}

	static COLORREF GetColorMask(DWORD mask){
		COLORREF maskrgb = 0;	
		if( mask&RGBMASK_RED )maskrgb |= 0xff;
		if( mask&RGBMASK_GREEN )maskrgb |= 0xff00;
		if( mask&RGBMASK_BLUE )maskrgb |= 0xff0000;
		return maskrgb;
	}

	COLORREF GetBackColor(){
		return m_clrBack;
	}
	
	void EnableD3D(BOOL bEnable){
		m_bUseD3D = bEnable;
	}
	
	void SetPrint(BOOL bPrint, float fDefautPrintWidth=1.0)
	{
		m_bPrint = bPrint;
		m_fDefautPrintWidth = fDefautPrintWidth;
	}

	virtual void SetBackColor(COLORREF clr);

	virtual void DrawGrBuffer(GrElementList *pList, int mode);
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode);
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);

	virtual void DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor);

	virtual CCoordSys *GetCoordSys();
	virtual void SetCoordSys(CCoordSys *pCS);
	virtual void BeginDrawTempBmp(HBITMAP hBmp,Envelope e){m_hTempBmp = hBmp;}
	virtual void DrawTempBmp(HBITMAP hBmp, CRect rcBmp, double x[4], double y[4], BOOL bUsetransColer = FALSE, COLORREF transColor = 0){}
	virtual void EndDrawTempBmp(int xl, int xh, int yl, int yh){}
	
	virtual void BeginDrawing();
	virtual void EndDrawing();

	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4]);

	//pRects 返回需要重绘的矩形区域
	virtual void Scroll(int dx, int dy, CRect *pRcView, CArray<CRect,CRect>* pRects);
	virtual void EraseBackground();

public:
	//保存像素和恢复像素的几个工具函数
	virtual void SaveBmp(int idx, CRect rect){}
	virtual void RestoreBmp(int idx){}
	virtual void ResetBmp(int idx);

	void SetGlViewRect(CRect *pRect);
	void SetGDIViewRect(CRect *pRect);

	int	RequestSaveData(LONG_PTR id);
	void ReleaseSaveData(int idx);

	void CopyDisplaySetting(CDrawingContext *pDC);

	static BOOL m_bTextClear;
protected:
	BOOL CreateGlContext(BOOL bStereo, BOOL bOverlay );
	BOOL CreateGlContextForBmp();
	void DestroyGlContext();
	int	 SetGlPixelFormatOverlay(HDC hDC, BYTE type, DWORD flags);
	BOOL SetGlPixelFormat_new(HDC hDC, BOOL bOverlay, BOOL bAlpha, int flags);
	void BltBmpToGlContext(HBITMAP hBmp,int x,int y,int xTar,int yTar,int nTarWidth,int nTarHeight);

	void DrawGrBuffer_GDI(HDC hdc, GrElementList *pList, int mode);	
	void DrawGrBuffer2d_GDI(HDC hdc, GrElementList *pList, int mode);
	void DrawSelectionMark_GDI(HDC hdc, GrElementList *pList, int mode, COLORREF clr, float wid);
	void DrawSelectionMark2d_GDI(HDC hdc, GrElementList *pList, int mode, COLORREF clr, float wid);
	void DrawGeometryMark_GDI(HDC hdc, GrElementList *pList, int mode, CMarkPtInfor &ptInfor);
	void DrawGeometryMark_GL(HDC hdc, GrElementList *pList, int mode, CMarkPtInfor &ptInfor);

	void DrawGrBuffer_D3D(GrElementList *pList, int mode);
	void DrawGrBuffer2d_D3D(GrElementList *pList, int mode);
	void DrawSelectionMark_D3D(GrElementList *pList, int mode, COLORREF clr, float wid);
	void DrawSelectionMark2d_D3D(GrElementList *pList, int mode, COLORREF clr, float wid);
	void DrawGeometryMark_D3D(GrElementList *pList, int mode, CMarkPtInfor &ptInfor);

	void DrawGrBuffer_GL(GrElementList *pList, int mode);
	void DrawGrBuffer2d_GL(GrElementList *pList, int mode);
	void DrawSelectionMark_GL(GrElementList *pList, int mode, COLORREF clr, float wid);
	void DrawSelectionMark2d_GL(GrElementList *pList, int mode, COLORREF clr, float wid);

	void DrawGrBuffer_Base(CSuperDrawDC* pDC, GrElementList *pList, int mode, double lfWidthRatio=1.0);
	void DrawGrBuffer2d_Base(CSuperDrawDC* pDC, GrElementList *pList, int mode, double lfWidthRatio=1.0);
	void DrawSelectionMark_Base(CSuperDrawDC* pDC, GrElementList *pList, int mode, COLORREF clr, float wid);
	void DrawSelectionMark2d_Base(CSuperDrawDC* pDC, GrElementList *pList, int mode, COLORREF clr, float wid);
	void DrawGeometryMark_Base(CSuperDrawDC* pDC, GrElementList *pList, int mode, CMarkPtInfor &ptInfor);

	void SaveBmp_GDI(HDC hdc, int idx, CRect rect);
	void RestoreBmp_GDI(HDC hdc, int idx);

	void SaveBmp_GL(int idx, CRect rect);
	void RestoreBmp_GL(int idx);

	inline COLORREF GetDrawColor(COLORREF color, BOOL bXorMode)
	{
		if( m_bMonoMode )
		{
			//灰度转换 R*0.3+G*0.59+B*0.11 
			DWORD grey = GetRValue(color)*307L + GetGValue(color)*604L + GetBValue(color)*113L;
			BYTE R2 = ((grey*GetRValue(m_monoColor))>>18);
			BYTE G2 = ((grey*GetGValue(m_monoColor))>>18);
			BYTE B2 = ((grey*GetBValue(m_monoColor))>>18);

			COLORREF clr2 = RGB(R2,G2,B2);

			return clr2;
		}
		else if( bXorMode )
		{
			return color;
		}
		else
		{
			if (m_bStereo)
			{
				return color;
			}

			BYTE R2 = GetRValue(color);
			BYTE G2 = GetGValue(color);
			BYTE B2 = GetBValue(color);
			
			BYTE R0 = GetRValue(m_clrBack);
			BYTE G0 = GetGValue(m_clrBack);
			BYTE B0 = GetBValue(m_clrBack);
			
			//不与背景色完全一致
			if( (R2>R0?(R2-R0):(R0-R2))+(G2>G0?(G2-G0):(G0-G2))+(B2>B0?(B2-B0):(B0-B2))<30 )
			{
				R2 = 255-R2;
				G2 = 255-G2;
				B2 = 255-B2;

				color = RGB(R2,G2,B2);
			}
			
			return color;
		}
	}
protected:
	CCoordSys *m_pCoordSys;
	HDC m_hOriginalDC;
	CSize m_szDC;

	CRect m_rcView;
	BOOL m_bMonoMode;
	COLORREF m_monoColor;

	COLORREF m_clrBack;

	BOOL m_bCreateOK;

	DWORD m_nRGBMask;

	HBITMAP m_hTempBmp;

public:
	HWND m_hWnd;
	CD3DWrapper *m_pD3D;

	BOOL m_bCreateD3D, m_bUseD3D;

public:
	BOOL m_bNoLineWeight;
	BOOL m_bNoHatch;
	BOOL m_bStereo;
	
	float m_fDrawCellAngle, m_fDrawCellKX, m_fDrawCellKY;

	BOOL m_bSupportLogicOp;

protected:
	
	HGLRC			m_hglRC; //影像显示的opengl 图层
	HGLRC           m_hglRC_overlay; //用于硬测标显示的叠加图层
	CPalette		m_Palette_Overlay; // Overlay 的调色板

protected:
	ImgSaveData		m_saveData[20];
	LONG_PTR			m_saveDataUser[20];

	CDxFont			m_TextDrawing;

	// 打印
	BOOL            m_bPrint;
	float           m_fDefautPrintWidth;
	
	CDrawOpSet		m_PointDrawOpSet;  //缓存上个图元点的绘图操作，以减少重复运算

	CDrawOpSet		m_PointStringDrawOpSet;  //绘制点串时，缓存第一个图元点的绘图操作，以减少重复运算

};

class C2DGLDrawingContext;

//平面的绘图环境，使用GDI绘图库
//采用一个3×3的矩阵表示其坐标系
//使用2d的矢量缓存，采用2d的 VectorLayer
class EXPORT_SMARTVIEW C2DGDIDrawingContext : public CDrawingContext
{
	DECLARE_DYNCREATE(C2DGDIDrawingContext)
public:
	virtual void SetDCSize(CSize szDC);
	C2DGDIDrawingContext();
	virtual ~C2DGDIDrawingContext();
	
	virtual BOOL CreateContext(HDC hDC);
	virtual HDC GetContext();
	HDC SetDrawingDC(HDC hDC);
	HDC GetOriginalDC(HDC hDC);

	void CreateMemBoard();

	//创建内存位图画板，用于优化贴图操作
	HBITMAP CreateMemBmpBoard(int w, int h);

	//获得内存位图画板
	HBITMAP GetMemBoardBmp();
	HDC GetMemBoardDC();

	virtual void BeginDrawTempBmp(HBITMAP hBmp,Envelope e);
	virtual void DrawTempBmp(HBITMAP hBmp, CRect rcBmp, double x[4], double y[4], BOOL bUsetransColer = FALSE, COLORREF transColor = 0);
	virtual void EndDrawTempBmp(int xl, int xh, int yl, int yh);
	virtual void BeginDrawing();
	virtual void EndDrawing();

	virtual void EraseBackground();

	virtual void DrawGrBuffer(GrElementList *pList, int mode);
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode);
	virtual void PrintGrBuffer2d(GrElementList *pList, int mode, double lfWidthRatio = 1);

	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4]);
	virtual void SetViewRect(CRect rcView);

	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);

	virtual void DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor);

	void TurnonMemBmpBoard();
	void TurnoffMemBmpBoard();
	void SetAsMemOpOnly(BOOL bYes);
	BOOL IsMemOpOnly(){
		return m_bMemOpOnly;
	}

	void SetBackColor(COLORREF clr);

	void DrawBitmapByAnyXY(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4]);

	int m_nCopyMode;

protected:
	virtual void SaveBmp(int idx, CRect rect);
	virtual void RestoreBmp(int idx);

	BOOL CreateMemGLDC(int w, int h);

public:
	//按任意的XY坐标 绘制位图（纹理方式，可任意旋转拉伸）
	BOOL m_bDrawBmpByAnyXY;

protected:
	Envelope m_eClient;
	HDC m_hTemDC;
	HBITMAP m_hOldTemBitmap;

	HDC m_hCurDC;
	HDC m_hMemDC;
	HBITMAP m_hOldBmp, m_hMemBmp;
	HDC m_hTurnonOldDC;

	CRgn m_rgnCur;

	int m_nBeginDrawOp;
	int m_nTurnonOp;
	BOOL m_bMemOpOnly;

	HDC m_hMemGLDC;
	HBITMAP m_hMemGLBmp, m_hMemOldGLBmp;
	C2DGLDrawingContext *m_pMemGLDC;

};


class EXPORT_SMARTVIEW CActiveGLRC
{
public:
	CActiveGLRC();
	CActiveGLRC(HDC hDC, HGLRC hRC);
	~CActiveGLRC();

	BOOL Active(HDC hDC, HGLRC hRC);
	BOOL Restore();
	int  CalledDepth(){
		return m_nOp;
	}

	static HGLRC GetCurRC(){
		return m_hCurRC;
	}

	static HDC GetCurDC(){
		return m_hCurDC;
	}

	static int GetOwnerCount(){
		return m_nOwner;
	}

	static void ActiveAhead(HDC hDC, HGLRC hRC);

public:
	static HDC m_hCurDC;
	static HGLRC m_hCurRC;
	static int m_nOwner;
	
protected:
	HDC m_hOldDC;
	HGLRC m_hOldRC;
	BOOL m_bChanged;
	int m_nOp;
};


//平面的绘图环境，使用OpenGL绘图库
//采用一个3×3的矩阵表示其坐标系
//使用2d的矢量缓存，采用2d的 VectorLayer
class EXPORT_SMARTVIEW CD3DDrawingContext : public CDrawingContext
{
	DECLARE_DYNCREATE(CD3DDrawingContext)
public:
	CD3DDrawingContext();
	virtual ~CD3DDrawingContext();
	
	virtual void BeginDrawing();
	virtual void EndDrawing();
	
	virtual void DrawGrBuffer(GrElementList *pList, int mode);
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode);
	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4]);
	virtual void SetViewRect(CRect rcView);
	virtual void EraseBackground();
	
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawGeometryMark(GrElementList *pList, int mode, CMarkPtInfor &ptInfor);
	
	virtual void Scroll(int dx, int dy, CRect *pRcView, CArray<CRect,CRect>* pRects);
	
	//保存像素和恢复像素的几个工具函数
	virtual void SaveBmp(int idx, CRect rect);
	virtual void RestoreBmp(int idx);
	HBITMAP Create32MemBmp(int w, int h);
	void SetTransColorValues(HBITMAP hBmp, COLORREF color);

protected:
	//用于临时绘制位图
	
};



//平面的绘图环境，使用OpenGL绘图库
//采用一个3×3的矩阵表示其坐标系
//使用2d的矢量缓存，采用2d的 VectorLayer
class EXPORT_SMARTVIEW C2DGLDrawingContext : public CDrawingContext
{
	DECLARE_DYNCREATE(C2DGLDrawingContext)
public:
	C2DGLDrawingContext();
	virtual ~C2DGLDrawingContext();
	
	BOOL CreateContext(HDC hDC);
	BOOL CreateContextForBmp(HDC hDC);
	HGLRC GetGLRC();
	HGLRC SetDrawingRC(HDC hDC, HGLRC hRC);
	virtual void BeginDrawTempBmp(HBITMAP hBmp, Envelope e);
	virtual void DrawTempBmp(HBITMAP hBmp, CRect rcBmp, double x[4], double y[4], BOOL bUsetransColer = FALSE, COLORREF transColor = 0);
	virtual void EndDrawTempBmp(int xl, int xh, int yl, int yh);

	virtual void BeginDrawing();
	virtual void EndDrawing();

	virtual void DrawGrBuffer(GrElementList *pList, int mode);
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode);
	virtual void DrawBitmap(HBITMAP hBmp, CRect rcBmp, double x0[4], double y0[4], double x[4], double y[4]);
	virtual void SetViewRect(CRect rcView);
	virtual void EraseBackground();
	
	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);
	
	virtual void Scroll(int dx, int dy, CRect *pRcView, CArray<CRect,CRect>* pRects);

	//保存像素和恢复像素的几个工具函数
	virtual void SaveBmp(int idx, CRect rect);
	virtual void RestoreBmp(int idx);
	HBITMAP Create32MemBmp(int w, int h);
	void SetTransColorValues(HBITMAP hBmp, COLORREF color);
protected:
	//用于临时绘制位图
	HDC m_hMemDC;
	HBITMAP m_hOldBitmap;
	HGLRC m_hTmpRC;
	HGLRC m_hOldRC;
	HDC m_hOldDC;
	tex_image *m_pTmpTex;
	int m_nLastTextSize;
	HBITMAP m_hBitmap32;

	HGLRC m_hCurRC;
	CActiveGLRC		m_switchRC;
};


class EXPORT_SMARTVIEW C2DPrintGLDrawingContext:public C2DGLDrawingContext
{
	DECLARE_DYNCREATE(C2DPrintGLDrawingContext)
public:
	C2DPrintGLDrawingContext();
	virtual ~C2DPrintGLDrawingContext();
	BOOL CreateContext(HDC hDC);//必须是内存DC
	virtual void BeginDrawing();
	virtual void EndDrawing();
protected:
	HGLRC			m_hglOldRC0;
	HDC      m_hOldDC0;
};
class EXPORT_SMARTVIEW C2DPrintGDIDrawingContext:public C2DGDIDrawingContext
{
	DECLARE_DYNCREATE(C2DPrintGDIDrawingContext)
public:
	C2DPrintGDIDrawingContext();
	virtual ~C2DPrintGDIDrawingContext();
	void CreateMemBoard();
	void TurnonMemBmpBoard();
	void TurnoffMemBmpBoard();
	void SetBackColor(COLORREF clr);

	virtual void BeginDrawing();
	virtual void EndDrawing();
};
//三维的绘图环境，使用OpenGL图形库
//支持透视和正射显示模式，支持任意角度的显示
//采用一个4×4的矩阵表示其坐标系
//使用3d的矢量缓存，采用3d的 DrawingVector3d
class EXPORT_SMARTVIEW C3DDrawingContext : public CDrawingContext
{
	DECLARE_DYNCREATE(C3DDrawingContext)
public:
	C3DDrawingContext();
	virtual ~C3DDrawingContext();

	HGLRC GetGLRC();
};


//立体的绘图环境（同时也兼容了单屏方式）
//无坐标系转换
//使用2个2d的矢量缓存，采用2d的 DrawingVector2d
class EXPORT_SMARTVIEW CStereoDrawingContext : public CDrawingContext
{
	DECLARE_DYNCREATE(CStereoDrawingContext)

protected:
	enum DrawState
	{
		drawNoneState = 0,
		drawNormal = 1,
		drawErasable = 2,
		drawOverlay = 3,
		drawBitmap = 4
	};
public:
	enum DrawSide
	{
		drawLeft = 1,
		drawRight = 2
	};
	enum DisplayMode
	{
		modeSingle = 0,
		modeShutterStereo = 1,
		modeSplit = 2,
		modeRGStereo = 3,
		modeInterleavedStereo = 4
	};
	enum TextureMode
	{
		textureModeNone = 0,
		textureModeGL = 1,
		textureModeD3D = 2
	};
	
	CStereoDrawingContext();
	virtual ~CStereoDrawingContext();

	HGLRC GetRCContext();
	HGLRC GetOverlayRCContext();

	BOOL SafeActiveGLRC();

	BOOL SetDisplayMode(int nMode);
	inline int  GetDisplayMode(){
		return m_nDisplayMode;
	}

	void SetDrawSide(int nSide);
	int GetDrawSide();

	void SetGLDrawBuffer(int nSide);
	void SetFillBlockGLDrawBuffer(int nSide);

	CDrawingContext *GetLeftContext();
	CDrawingContext *GetRightContext();
	CDrawingContext *GetCurContext();
	void OnFinishDrawSide();

	virtual BOOL CreateContext(HDC hDC);
	virtual void BeginDrawing();
	virtual void EndDrawing();
	virtual void DrawGrBuffer(GrElementList *pList, int mode);
	virtual void DrawGrBuffer2d(GrElementList *pList, int mode);
	virtual void SetViewRect(CRect rcView);
	virtual void SetDCSize(CSize szDC);
	virtual void EraseBackground();
	void Scroll(int dx, int dy, int dp, int dq, CRect *pRcView, 
		CArray<CRect,CRect>* pLRects, CArray<CRect,CRect> *pRRects,
		CArray<CRect,CRect>* pPublicRects);

	virtual void DrawSelectionMark(GrElementList *pList, int mode, COLORREF clr, float wid);
	virtual void DrawSelectionMark2d(GrElementList *pList, int mode, COLORREF clr, float wid);

	virtual CCoordSys *GetCoordSys();
	virtual void SetCoordSys(CCoordSys *pCS);

	virtual void SaveBmp(int idx, CRect rect);
	virtual void RestoreBmp(int idx);

	//overlay上的绘图
	void BeginDrawingOverlay();
	void EndDrawingOverlay();

	//即时绘图
	BOOL CanPaintErasable();  //是否可以在整体绘制(OnPaint)时绘制Erasable图层
	BOOL CanUpdateErasable(); //是否可以在部分绘制(Update)时绘制Erasable图层
	void BeginDrawingErasable();
	void EndDrawingErasable();	

	//bitmap上的绘图
	void BeginDrawingBitmap();
	void EndDrawingBitmap();
	void OnFinishDrawSideBitmap();
	BOOL GetDrawBitmap(HBITMAP hbitmap,CRect bmpRect,CRect *pRect);

	CRect GetSplitViewRect(int nDrawSide);
	CRect GetSplitDrawRect(int nDrawSide);

	void UpdateRGBMask(BOOL bLeft, BOOL bSave, DWORD& mask);

	void EnableLogicOp(BOOL bEnable);

	void SetRegPath(LPCTSTR path);

	void SetBackColor(COLORREF clr);

	void OnCreateD3D();

	BOOL IsD3DLost();

protected:
	void ScrollGlPixel(int dx,int dy,int dp,int dq,CRect *pRcView, 
		CArray<CRect,CRect>* pLRects, CArray<CRect,CRect> *pRRects);

	void DrawGrBuffer_interleaved(GrElementList *pList, int mode);
	void DrawGrBuffer2d_interleaved(GrElementList *pList, int mode);
	int  GetSplitX(int nDrawSide);
	BOOL IsPolarizedVisibleLeft(int y);
private:
	BOOL PrepareMemBoard(int w, int h);
	BOOL PrepareRGMemBmp(int w, int h);
	BOOL ReadGLPixels(CRect rect);
	BOOL ReadD3DPixels(CRect rect);
	BOOL MakeInterleavedBmp(HBITMAP hBmp1, int x1, int y1, HBITMAP hBmp2, CRect rc2, BOOL bOdd);

public:
	//是否以立体模式创建
	BOOL m_bCreateStereo;

	//是否创建叠加层
	BOOL m_bCreateOverlay;

	//是否使用OpengGL的纹理模式显示影像
	BOOL m_nTextMode;

	//是否反立体显示
	BOOL m_bInverseStereo;

	//隔行立体显示的窗口句柄，用于测试首行显示的奇偶性
	HWND m_hInterleavedWnd;

	//是否采用以前的滚屏方式（平滑滚动）
	BOOL m_bOldStereoScroll;

	//是否只更新左右片中的一片
	BOOL m_bPaintOneSide;

	//纠正立体闪烁
	BOOL m_bCorrectFlicker;

protected:
	CDrawingContext *m_pLeft;
	CDrawingContext *m_pRight;

	int m_nDisplayMode;
	int m_nDrawSide;
	int m_nDrawState;

	int m_nBeginDrawOp;

	CActiveGLRC		m_switchRC;

	//for RGStereo or InterleavedStereo
	HDC m_hMemBoardDC;
	HBITMAP m_hMemBoardBmp;
	HBITMAP m_hMemRGBmp;

	//是否使用后备缓冲
	BOOL m_bDrawBack;

	TCHAR m_strSegPath[256];

	CGLDynamicTexture m_dt;
};


class EXPORT_SMARTVIEW CPrintingDrawingContext : public CDrawingContext
{
	DECLARE_DYNCREATE(CPrintingDrawingContext)
public:
	CPrintingDrawingContext();
	virtual ~CPrintingDrawingContext();		
	
	CDrawingContext *GetCurContext();
	BOOL BeginDrawVector();
	BOOL EndDrawVector();

	BOOL BeginDrawImg();
	BOOL BltImgToPrint(CRect curRect);//往打印机DC上帖图的区域
	BOOL EndDrawImg();
	
protected:
	CDrawingContext  *m_pCurContext;
	C2DPrintGLDrawingContext *m_pGlContext;
	C2DPrintGDIDrawingContext *m_pGdiContext;

	HDC m_hTempMemDC;//opengl在帖位图时没法直接作用于打印机，故用此做中介
	HBITMAP m_hTempMemBmp;
	HBITMAP m_hOldBmp;
};
MyNameSpaceEnd


#endif // !defined(AFX_DRAWINGCONTEXT_H__0090C03A_CEC9_4CA7_8D9F_FFC6AAC13700__INCLUDED_)
