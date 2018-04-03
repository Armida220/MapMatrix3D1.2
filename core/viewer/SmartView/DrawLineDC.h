// DrawLineDC.h: interface for the CMyDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDC_H__595E6CD2_F6AA_4552_B81D_DFF98B844E69__INCLUDED_)
#define AFX_MYDC_H__595E6CD2_F6AA_4552_B81D_DFF98B844E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Envelope.h"
#include "Drawing.h"
#include "D3DClass.h"


MyNameSpaceBegin

struct GrPt3D
{
	double x,y,z;
	int pencode;
};


class EXPORT_SMARTVIEW CDrawLineDC
{
public:
	enum
	{
		penStart = 0,
		penMove = 1,
		penLine = 2
	};
	CDrawLineDC();
	~CDrawLineDC();

	void SetEnvelope(BOOL bUseEnvelope, Envelope e);
	void MoveTo(double x, double y, double z=0);
	void LineTo(double x, double y, double z=0);
	void SetColor(COLORREF clr);
	void SetWidth(float wid);

	void Rectangle(double x1, double y1, double x2, double y2, double z=0);

	void Begin(int npt);
	void End();
	COLORREF GetColor();
	int GetPts(GrPt3D *&arr);

protected:
	COLORREF cur_color;
	float	 cur_width;
	double	 cur_x,cur_y,cur_z;

	BOOL	 bPenChanged;
	
//	float   *pts;
	GrPt3D   *pts;
	int      npt_use, npt_len;
	int      cur_state;//-1, initial, 0, moveto, 1, lineto

	BOOL	 m_bUseEnvelope;
	double    m_xl,m_xh,m_yl,m_yh;

	long	 count;
};

#define REGPATH_SYMBOL			_T("Config\\Options\\Symbol")
#define REGITEM_ROUNDLINE		_T("RoundLine")

class EXPORT_SMARTVIEW CGdiDrawLineDC : public CDrawLineDC
{
public:
	CGdiDrawLineDC(HDC hDC);
	~CGdiDrawLineDC();

	//xor,copy���ȵ�
	void SetDrawMode(int mode);

	void End();
	void RestoreDefPen();
	void SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle);
	void Point(double x, double y, COLORREF color, float size, BOOL bFlat=FALSE);
	void EnableRoundLine(BOOL bRound) { m_bRoundLine=bRound; }

protected:
	HDC hDC;
	HPEN hDefPen, hCurPen;
	DWORD nExtStyle;
	DWORD pStyles[8];
	BOOL m_bRoundLine;

	void *pGraphics;
};


class EXPORT_SMARTVIEW CGdiDrawingDC : public CGdiDrawLineDC
{
public:
	CGdiDrawingDC(HDC hDC);
	~CGdiDrawingDC();
	
	void MoveTo(double x, double y, double z=0);
	void LineTo(double x, double y, double z=0);
	void SetColor(COLORREF clr);
	void SetWidth(float wid);
	
	void Rectangle(double x1, double y1, double x2, double y2, double z=0);
	
	void Begin(int npt);
	void End();
	void RestoreDefPen();
	void SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle);
	void Point(double x, double y, COLORREF color, float size, BOOL bFlat=FALSE);

	void PolyPolygon(GrPt3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color);

public:
	static BOOL m_bUseGDIPolygon;
	
protected:
	BOOL m_bUseGDIDC;

	CDrawing m_dr;
};

class EXPORT_SMARTVIEW CGlDrawLineDC : public CDrawLineDC
{
public:
	CGlDrawLineDC();
	~CGlDrawLineDC();

	//xor,copy���ȵ�
	void SetDrawMode(int mode);
	
	void End();
	void SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle);

	void BeginPoint(COLORREF color, float size, BOOL bFlat=FALSE);
	void Point(double x, double y, double z=0);
	void BatchPointColor(COLORREF color);
    void EndPoint();

	void PolyPolygon(GrPt3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color);
	
protected:
	DWORD nExtStyle;
	DWORD pStyles[8];
	BOOL m_bRoundLine;

protected:
	float curPointSize;
	BOOL curPointFlat;
	COLORREF curPointColor;
};


class EXPORT_SMARTVIEW CD3DDrawLineDC : public CDrawLineDC
{
public:
	CD3DDrawLineDC();
	~CD3DDrawLineDC();

	//xor,copy���ȵ�
	void SetDrawMode(int mode);
	
	void Init(CD3DWrapper *p);
	void Point(double x, double y, COLORREF color, float size, BOOL bFlat=FALSE);
	void Begin(int npt);
	void End();
	
	void BeginTriangles(int num);
	void Triangle(double x1, double y1, double x2, double y2, double x3, double y3, double z=0);
	void EndTriangles();
	
	void SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle);

	void PolyPolygon(GrPt3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color);
	
	BOOL m_bUseStyle;
	
	CD3DWrapper *m_pD3D;
};


#define DRAWOP_POINT			1
#define DRAWOP_LINE				2
#define DRAWOP_TRIANGLE			3
#define DRAWOP_POLYGON			4
#define DRAWOP_TEXT				5

#define DRAWMODE_XOR			1
#define DRAWMODE_COPY			2


//��ͼ���������ڻ��ƴ�����ͬ��ͼԪʱ�����Խ���һ��ͼԪ�Ļ�ͼ���������������Ա������ͼԪʹ�ã�
//ʵ���Ͼ��ǽ��������õĲ�������������
class EXPORT_SMARTVIEW CDrawOpSet
{
public:
	struct OpItem
	{
		int type;
		COLORREF color;
	};
	struct OpPoint : public OpItem
	{
		OpPoint(){
			type = DRAWOP_POINT;
			x = y = z = 0;
			width = 1.0f;
			bFlat = FALSE;
			color = 0xffffff;
		}
		double x,y,z;
		float width;
		BOOL bFlat;
	};
	struct OpLines : public OpItem
	{
		OpLines(){
			type = DRAWOP_LINE;
			bRoundLine = FALSE;
			width = 1.0f;
			nstyle = 0;
			color = 0xffffff;
		}
		float width;
		BOOL bRoundLine;
		DWORD nstyle;
		DWORD styles[8];
		CArray<GrPt3D,GrPt3D> arrPts;
	};
	struct OpTriangles : public OpItem
	{
		OpTriangles(){
			type = DRAWOP_TRIANGLE;
			nTriType = 0;
			color = 0xffffff;
		}
		int nTriType;
		CArray<GrPt3D,GrPt3D> arrPts;
	};
	struct OpPolygons : public OpItem
	{
		OpPolygons(){
			type = DRAWOP_POLYGON;
			color = 0xffffff;
		}
		CArray<int,int> arrCounts;
		CArray<PT_3D,PT_3D> arrPts;
	};
	struct OpText : public OpItem
	{
		OpText(){
			type = DRAWOP_TEXT;
			color = 0xffffff;
		}
		TextSettings settings;
		CString strText;
		PT_3D pt;
		COLORREF bkColor;
		BOOL bSmooth;
		BOOL bOutLine;
	};
	struct CellInfo
	{
		CellInfo(){
			valid = 0;
		}
		int valid;
		PT_3D pt;
		COLORREF color;
		float kx;	 //x���ű���
		float ky;	 //y���ű���
		float angle; //��ת��
		int bGrdSize:1; //�ߴ��Ƿ�����ʾ���ʱ仯
		int bFlat:1; //���㻹��Բ��
		int cell:24; //ͼԪ���ŵ�����
		float width; // �߿�
	};
	
	CDrawOpSet();
	~CDrawOpSet();
	
	//Ŀ��ͼԪ���󣬿��Բ�����
	void SetCellInfo(int cell, COLORREF color,float kx, float ky, float angle, float lineWidth, BOOL bGrdSize);
	void SetCellPt(double x, double y, double z);
	BOOL CompareCell(int cell, COLORREF color,float kx, float ky, float angle, float lineWidth, BOOL bGrdSize);

	void SetDrawMode(int mode);
	
	void Point(double x, double y, double z, COLORREF color, float size, BOOL bFlat=FALSE);
	
	void Begin(int npt);
	void SetColor(COLORREF clr);
	void SetWidth(float wid);
	void MoveTo(double x, double y, double z=0);
	void LineTo(double x, double y, double z=0);
	void SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle);
	void End();
	
	void Polygon(PT_3D *pts, int npt, COLORREF color);
	void PolyPolygon(PT_3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color);

	void SetTextSettings(TextSettings *pSet);
	void DrawText(LPCTSTR strText, double x, double y, COLORREF color, COLORREF bkColor, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);
	
	void MoveOpSetXY(double dx, double dy, double dz);
	void MoveToOpSetXY(double x, double y, double z);
	void SetAllColor(COLORREF clr);
	
	void ClearAll();
	
	CellInfo m_cell;
	int m_nDrawMode;
	HDC m_hDC;
	OpItem *m_pCurItem;
	CArray<OpItem*,OpItem*> m_arrPItems;
};


#define DRAWDC_GDI					1
#define DRAWDC_GL					2
#define DRAWDC_D3D					3


//����������drawdc��װ�����ĳ��࣬Ϊ�˱���ʹ���麯����ͨ��int����������
class EXPORT_SMARTVIEW CSuperDrawDC
{
public:
	CSuperDrawDC();
	~CSuperDrawDC();
	
	int GetType()const;
	void CreateType(int type, HDC hdc, void *param=NULL);

	//xor,copy���ȵ�
	void SetDrawMode(int mode);

	void SetCoordMode(BOOL bInt, BOOL bInverseY, CSize szDC);
	COLORREF GetColor();
	
	//����
	void Point(double x, double y, double z, COLORREF color, float size, BOOL bFlat=FALSE);

	BOOL CanBatchPoint();
	void StartBatchPoint(COLORREF color, float size, BOOL bFlat=FALSE);
	void BatchPoint(double x, double y, double z);
	void BatchPointColor(COLORREF color);
	void EndBatchPoint();
	
	//����
	void Begin(int npt);
	void SetColor(COLORREF clr);
	void SetWidth(float wid);
	void MoveTo(double x, double y, double z=0);
	void LineTo(double x, double y, double z=0);
	void SetExtPen(DWORD dwStyleCount,const DWORD *lpStyle);
	void End();
	void Rectangle(double x1, double y1, double x2, double y2, double z=0);
	void MarkBox(double x1, double y1, double x2, double y2, double z=0, BOOL bFill=TRUE);

	//��������
	void DrawDiamond(double x, double y, double w, double z=0);
	
	//����
	void Polygon(PT_3D *pts, int npt, COLORREF color);
	void PolyPolygon(PT_3D *pts, int *pCounts, int nPolygon, int npt, COLORREF color);

	//�ı�
	void SetTextSettings(TextSettings *pSet);
	void DrawText(LPCTSTR strText, double x, double y, COLORREF color, COLORREF bkColor, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);

	//������
	void EnableUseOpSet(BOOL bEnable, CDrawOpSet *pOp);
	BOOL IsEnableUseOpSet();
	void DrawSet(CDrawOpSet *pSet);

protected:
	CGdiDrawingDC *GetGDI();
	CGlDrawLineDC *GetGL();
	CD3DDrawLineDC *GetD3D();
	float CX(double x);
	float CY(double y);

	void DeleteDC();

	HDC m_hDC;
	CDrawLineDC *m_pDC;
	int m_nType;

	BOOL m_bInt, m_bInverseY;
	CSize m_szDC;

	TextSettings m_textSettings;

	BOOL m_bStartBatchPoint;

	BOOL m_bEnableUseOpSet;
	CDrawOpSet *m_pDrawOpSet;
};


//��ӡԤ��ʱHDC����������ϵ�ı仯����ʱgdiplus����ʾ����ȷ����Ҫ����Ϊ��ʼ״̬���������ڱ���ͻָ�HDC��״̬
class CTempResetDCInfos
{
public:
	CTempResetDCInfos(HDC hDC);
	~CTempResetDCInfos();
private:	
/*
	�������ԣ����������⼸���������У�����ֻ����SaveDC/RestoreDC
	int v1,v2;
	SIZE s1,s2;
	POINT p1,p2;
	XFORM f1;
*/	
	HDC hDC0;
	int nSaveID;
	BOOL bUseSave;
};


MyNameSpaceEnd

#endif // !defined(AFX_MYDC_H__595E6CD2_F6AA_4552_B81D_DFF98B844E69__INCLUDED_)
