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

//��ͼ����
//�ṩһ������Ļ�ͼ�����ĸ���ṩͳһ�Ļ�ͼ�ӿڣ�ʹ��ʹ���߲���Ҫ���ľ���Ļ�ͼ�ӿ�
//������GDI��OpenGL��������ϵ�����䡢����ͶӰ�ȵȣ�����ʾģʽ��ƽ�桢����ȵȣ���
// �Ӷ����ٻ�ͼ���ݺͻ�ͼ���������
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

	//���û�ͼ����rcView Ӧ�� szDC ��һ����
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

	//pRects ������Ҫ�ػ�ľ�������
	virtual void Scroll(int dx, int dy, CRect *pRcView, CArray<CRect,CRect>* pRects);
	virtual void EraseBackground();

public:
	//�������غͻָ����صļ������ߺ���
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
			//�Ҷ�ת�� R*0.3+G*0.59+B*0.11 
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
			
			//���뱳��ɫ��ȫһ��
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
	
	HGLRC			m_hglRC; //Ӱ����ʾ��opengl ͼ��
	HGLRC           m_hglRC_overlay; //����Ӳ�����ʾ�ĵ���ͼ��
	CPalette		m_Palette_Overlay; // Overlay �ĵ�ɫ��

protected:
	ImgSaveData		m_saveData[20];
	LONG_PTR			m_saveDataUser[20];

	CDxFont			m_TextDrawing;

	// ��ӡ
	BOOL            m_bPrint;
	float           m_fDefautPrintWidth;
	
	CDrawOpSet		m_PointDrawOpSet;  //�����ϸ�ͼԪ��Ļ�ͼ�������Լ����ظ�����

	CDrawOpSet		m_PointStringDrawOpSet;  //���Ƶ㴮ʱ�������һ��ͼԪ��Ļ�ͼ�������Լ����ظ�����

};

class C2DGLDrawingContext;

//ƽ��Ļ�ͼ������ʹ��GDI��ͼ��
//����һ��3��3�ľ����ʾ������ϵ
//ʹ��2d��ʸ�����棬����2d�� VectorLayer
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

	//�����ڴ�λͼ���壬�����Ż���ͼ����
	HBITMAP CreateMemBmpBoard(int w, int h);

	//����ڴ�λͼ����
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
	//�������XY���� ����λͼ������ʽ����������ת���죩
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


//ƽ��Ļ�ͼ������ʹ��OpenGL��ͼ��
//����һ��3��3�ľ����ʾ������ϵ
//ʹ��2d��ʸ�����棬����2d�� VectorLayer
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
	
	//�������غͻָ����صļ������ߺ���
	virtual void SaveBmp(int idx, CRect rect);
	virtual void RestoreBmp(int idx);
	HBITMAP Create32MemBmp(int w, int h);
	void SetTransColorValues(HBITMAP hBmp, COLORREF color);

protected:
	//������ʱ����λͼ
	
};



//ƽ��Ļ�ͼ������ʹ��OpenGL��ͼ��
//����һ��3��3�ľ����ʾ������ϵ
//ʹ��2d��ʸ�����棬����2d�� VectorLayer
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

	//�������غͻָ����صļ������ߺ���
	virtual void SaveBmp(int idx, CRect rect);
	virtual void RestoreBmp(int idx);
	HBITMAP Create32MemBmp(int w, int h);
	void SetTransColorValues(HBITMAP hBmp, COLORREF color);
protected:
	//������ʱ����λͼ
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
	BOOL CreateContext(HDC hDC);//�������ڴ�DC
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
//��ά�Ļ�ͼ������ʹ��OpenGLͼ�ο�
//֧��͸�Ӻ�������ʾģʽ��֧������Ƕȵ���ʾ
//����һ��4��4�ľ����ʾ������ϵ
//ʹ��3d��ʸ�����棬����3d�� DrawingVector3d
class EXPORT_SMARTVIEW C3DDrawingContext : public CDrawingContext
{
	DECLARE_DYNCREATE(C3DDrawingContext)
public:
	C3DDrawingContext();
	virtual ~C3DDrawingContext();

	HGLRC GetGLRC();
};


//����Ļ�ͼ������ͬʱҲ�����˵�����ʽ��
//������ϵת��
//ʹ��2��2d��ʸ�����棬����2d�� DrawingVector2d
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

	//overlay�ϵĻ�ͼ
	void BeginDrawingOverlay();
	void EndDrawingOverlay();

	//��ʱ��ͼ
	BOOL CanPaintErasable();  //�Ƿ�������������(OnPaint)ʱ����Erasableͼ��
	BOOL CanUpdateErasable(); //�Ƿ�����ڲ��ֻ���(Update)ʱ����Erasableͼ��
	void BeginDrawingErasable();
	void EndDrawingErasable();	

	//bitmap�ϵĻ�ͼ
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
	//�Ƿ�������ģʽ����
	BOOL m_bCreateStereo;

	//�Ƿ񴴽����Ӳ�
	BOOL m_bCreateOverlay;

	//�Ƿ�ʹ��OpengGL������ģʽ��ʾӰ��
	BOOL m_nTextMode;

	//�Ƿ�������ʾ
	BOOL m_bInverseStereo;

	//����������ʾ�Ĵ��ھ�������ڲ���������ʾ����ż��
	HWND m_hInterleavedWnd;

	//�Ƿ������ǰ�Ĺ�����ʽ��ƽ��������
	BOOL m_bOldStereoScroll;

	//�Ƿ�ֻ��������Ƭ�е�һƬ
	BOOL m_bPaintOneSide;

	//����������˸
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

	//�Ƿ�ʹ�ú󱸻���
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
	BOOL BltImgToPrint(CRect curRect);//����ӡ��DC����ͼ������
	BOOL EndDrawImg();
	
protected:
	CDrawingContext  *m_pCurContext;
	C2DPrintGLDrawingContext *m_pGlContext;
	C2DPrintGDIDrawingContext *m_pGdiContext;

	HDC m_hTempMemDC;//opengl����λͼʱû��ֱ�������ڴ�ӡ�������ô����н�
	HBITMAP m_hTempMemBmp;
	HBITMAP m_hOldBmp;
};
MyNameSpaceEnd


#endif // !defined(AFX_DRAWINGCONTEXT_H__0090C03A_CEC9_4CA7_8D9F_FFC6AAC13700__INCLUDED_)
