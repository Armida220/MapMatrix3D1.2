// Copyright (C) 2004 - 2005 Gibuilder Group

#ifndef _INC_GRBUFFER2D_41084E5603AD_INCLUDED
#define _INC_GRBUFFER2D_41084E5603AD_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewBaseType.h"
#include "Envelope.h"
#include "Float.h"
#include "MemoryPool.h"

//文本放置方式
#define TAH_LEFT                      0x01
#define TAH_MID						  0x02
#define TAH_RIGHT                     0x04

#define TAV_TOP                       0x08
#define TAV_MID						  0x10
#define TAV_BOTTOM                    0x20
#define TAH_DOT						  0x40		//小数点对齐方式

#define GRAPH_TYPE_INVALID				0
#define GRAPH_TYPE_POINT2D				1
#define GRAPH_TYPE_LINESTRING2D			2
#define GRAPH_TYPE_POLYGON2D			3
#define GRAPH_TYPE_TEXT2D				4
#define GRAPH_TYPE_POINTSTRING2D		5
#define GRAPH_TYPE_POINTSTRING2DEX		6

#define IsGrPoint2d(gr)					((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POINT2D)
#define IsGrLineString2d(gr)			((gr)!=NULL&&(gr)->type==GRAPH_TYPE_LINESTRING2D)
#define IsGrPolygon2d(gr)				((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POLYGON2D)
#define IsGrText2d(gr)					((gr)!=NULL&&(gr)->type==GRAPH_TYPE_TEXT2D)
#define IsGrPointString2d(gr)			((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POINTSTRING2D)
#define IsGrPointString2dEx(gr)			((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POINTSTRING2DEX)

#define GRBUFFER_PTCODE_MOVETO			0
#define GRBUFFER_PTCODE_LINETO			1

#define IsGrPtCodeMoveTo(pt)			(((pt)->code&1)==0)
#define IsGrPtCodeLineTo(pt)			(((pt)->code&1)==1)


#define GRPT_MARK_DIAMOND				0x100


MyNameSpaceBegin

struct EXPORT_SMARTVIEW GR_PT_2D
{
	GR_PT_2D(float x0 = 0, float y0 = 0){
		x = x0, y = y0;
	}
	float x,y;
};

//图形顶点
struct EXPORT_SMARTVIEW GrVertex2d : public GR_PT_2D
{
	// flag this vertex is MoveTo or LineTo
	long code;

	inline float CodeToFloat()const{
		return code*0.00001f;
	}
	inline void CodeFromFloat(float a){
		code = a*100000;
	}
};

struct GrVertexList;

//图形顶点列
struct EXPORT_SMARTVIEW GrVertexList2d
{
	GrVertexList2d(){
		pts=0; nlen=nuse=0;
		isCompress = 0;
	}
	DECLARE_MEMPOOL(GrVertexList2d);
	//去除多余空间
	void CutSpace();
	void CopyFrom(const GrVertexList2d *list);
	void CopyFrom(const GrVertexList *list,float xoff,float yoff);

	//顶点列表
	GrVertex2d *pts;

	//顶点内存长度和当前使用数
	DWORD nlen:16;
	DWORD nuse:15;
	//是否做过压缩
	DWORD isCompress:1;
};

class GrBuffer2d;

//图形单位
struct EXPORT_SMARTVIEW Graph2d
{
	Graph2d(){
		next=NULL;color=0;
		type=GRAPH_TYPE_INVALID;
		use_needdraw = needdraw = 0;
		owner = NULL;
		bUseSelfcolor = 0;
		bUseBackColor = 0;
	}
	//图元类型
	DWORD type:4;

	//RGB 颜色
	DWORD color:24;

	//是否需要绘制，属于临时成员变量，绘图时由VectorLayer临时设置
	//为 0，表示不需要绘制
	DWORD needdraw:1;

	//绘制时是否使用 needdraw
	//为 0，表示不使用
	DWORD use_needdraw:1;

	// 是否使用独立颜色
	DWORD bUseSelfcolor:1;

	// 使用背景色绘制
	DWORD bUseBackColor:1;

	//下一个图元的链接指针
	Graph2d *next;

	GrBuffer2d *owner;
};

//点状图形
struct EXPORT_SMARTVIEW GrPoint2d : public Graph2d
{
	GrPoint2d(){
		kx = ky = 1; bGrdSize=TRUE;
		type=GRAPH_TYPE_POINT2D;
		angle = 0;
		width = 0;
		bFlat = 0;
	}

	DECLARE_MEMPOOL(GrPoint2d);

	GR_PT_2D pt;
	float kx;    //x缩放比例
	float ky;    //y缩放比例
	float angle; //旋转角
	int bGrdSize:1; //尺寸是否随显示比率变化
	int bFlat:1; //方点还是圆点
	int cell:24; //图元符号的索引(例如方块、圆点、矩形框、圆圈、十字丝、叉叉等等)
	float width; // 图元线宽，约定：width>=0表示指向的图元使用统一线宽width，width<0表示指向的图元的线宽比例为width
};


//点串状图形
struct EXPORT_SMARTVIEW GrPointString2d : public Graph2d
{	
	GrPointString2d(){
		kx = ky = 1; bGrdSize=TRUE;
		type=GRAPH_TYPE_POINTSTRING2D;
		cell = 0;
		width = 0;
		bFlat = 0;
	}
	GrPoint2d GetPoint(int i)const;

	Envelope GetEnvelope()const{
		return Envelope(xl,xh,yl,yh,-1e+12,1e+12);
	}
	
	GrVertexList2d ptlist; //旋转角存到点的code字段中
	float xl,xh,yl,yh;
	float kx;	 //x缩放比例
	float ky;	 //y缩放比例
	int bGrdSize:1; //尺寸是否随显示比率变化
	int bFlat:1; //方点还是圆点
	int cell:24; //图元符号的索引
	float width; // 线宽
};


//带属性的点串状图形
struct EXPORT_SMARTVIEW GrPointString2dEx : public GrPointString2d
{	
	GrPointString2dEx(){
		type=GRAPH_TYPE_POINTSTRING2DEX;
	}
	
	GrVertexAttrList attrList; //属性信息
};

//线状图形
struct EXPORT_SMARTVIEW GrLineString2d : public Graph2d
{
	GrLineString2d(){
		width=0; bGrdWid=TRUE; style=0;
		type=GRAPH_TYPE_LINESTRING2D;

		xl = -1e+12, xh = 1e+12, yl = -1e+12, yh = 1e+12;

		fDrawScale = 0.0;
	}
	Envelope GetEnvelope()const{
		return Envelope(xl,xh,yl,yh,-1e+12,1e+12);
	}

	DECLARE_MEMPOOL(GrLineString2d);

	//顶点列表
	GrVertexList2d ptlist;
	float xl,xh,yl,yh;
	float width;
	int bGrdWid:1;
	//比例系数
	float fDrawScale;
	int style:24; //线型符号的索引，如果是基本线型，就按照基本线型绘制，否则，就按照实线绘制
};

#define POLYGON_FILLTYPE_COLOR				0
#define POLYGON_FILLTYPE_PATTERN			1
#define POLYGON_FILLTYPE_TEXTURE			2

#define COMP_SCALE0							8.0

//面状图形
struct EXPORT_SMARTVIEW GrPolygon2d : public Graph2d
{
	GrPolygon2d(){
		pts=NULL;
		type=GRAPH_TYPE_POLYGON2D;
		filltype = POLYGON_FILLTYPE_COLOR;
		index = 0;

		xl = -1e+12, xh = 1e+12, yl = -1e+12, yh = 1e+12;
	}
	DECLARE_MEMPOOL(GrPolygon2d);

	Envelope GetEnvelope(){
		return Envelope(xl,xh,yl,yh,-DBL_MAX,DBL_MAX);
	}
	GrVertexList2d ptlist;
	float xl,xh,yl,yh;
	int filltype:8;    //填充类型，分为：颜色填充、纹理填充、
	int index:24;  //填充符号的索引，对应类型分别为：不使用、纹理ID；
	PT_2D *pts;	   //当为纹理填充时，此参数为纹理坐标，点数目与顶点数目一致
};

//文本图形
struct EXPORT_SMARTVIEW GrText2d : public Graph2d
{
	GrText2d(){
		type=GRAPH_TYPE_TEXT2D;
		bGrdSize=TRUE;
		text=NULL;	
		bRotateWithView = TRUE;
	}
	DECLARE_MEMPOOL(GrText2d);
	
	void Release();
	void SetText(LPCTSTR str);
	void SetSettings(const TextSettings *textSettings);
	BOOL CompareFont(const GrText2d *t)const;
	Envelope GetEnvelope(){
		double xl,xh,yl,yh;
			for (int i=0;i<4;i++ )
			{
				if (i==0)
				{
					xl = x[i];
					xh = x[i];
					
					yl = y[i];
					yh = y[i];
				}
				else
				{
					xl = (xl<x[i]?xl:x[i]);
					xh = (xh>x[i]?xh:x[i]);
					
					yl = (yl<y[i]?yl:y[i]);
					yh = (yh>y[i]?yh:y[i]);
				}
			
			}		
		return Envelope(xl,xh,yl,yh,-DBL_MAX,DBL_MAX);
	}
	GR_PT_2D pt;
	float x[4],y[4];
	DWORD bGrdSize:1;
	DWORD bRotateWithView:1;
	TCHAR *text;	
	TextSettings settings;	
};

Envelope EXPORT_SMARTVIEW GetEnvelopeOfGraph2d(const Graph2d *pGr, float fDrawCellAngle=0, float fDrawCellKX=1, float fDrawCellKY=1, float fDrawScale=1);
Envelope EXPORT_SMARTVIEW GetEnvelopeOfPointInString2d(const Graph2d *gr, float fDrawCellAngle=0, float fDrawCellKX=1, float fDrawCellKY=1, float fDrawScale=1);

void EXPORT_SMARTVIEW DeleteGraph2d(Graph2d *p);

class GrBuffer;
struct Graph;

class EXPORT_SMARTVIEW GrBuffer2d
{
public:	
	void GetOrigin(float& xoff, float& yoff) const;
	void SetOrigin(float xoff,float yoff);
	GrBuffer2d();
	~GrBuffer2d();

	DECLARE_MEMPOOL(GrBuffer2d);
	
	BOOL ReadFrom(CString& strXML);
	BOOL WriteTo(CString& strXML);

	void Text(COLORREF clr, PT_2D *pt, LPCTSTR text, const TextSettings *settings, BOOL bGrdSize=TRUE,BOOL bRotateWithView=TRUE,BOOL bCalcEnvelope=TRUE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void Point(COLORREF clr,PT_2D *pt,float kx,float ky, float angle=0, BOOL bGrdSize=TRUE, int cell=0, float width = 0, BOOL bFlat=FALSE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);	
	void BeginPolygon(COLORREF clr,int type=POLYGON_FILLTYPE_COLOR, int index=0, const PT_2D *pts=NULL, int nPt=0, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void BeginLineString(COLORREF clr, float width, BOOL bGrdWid=TRUE, int style=0, float fDrawScale=1.0, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void End(BOOL bCompress=TRUE);
	void BeginPointString(COLORREF clr,float kx,float ky, BOOL bGrdSize=TRUE, int cell=0, float width=0, BOOL bFlat=FALSE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void BeginPointStringEx(COLORREF clr,float kx,float ky, BOOL bGrdSize=TRUE, int cell=0, float width=0, BOOL bFlat=FALSE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void PointString(PT_2D *pt, float fAngle=0);
	void PointStringEx(PT_2D *pt, COLORREF color, BOOL isMark, float fAngle=0);
	void LineTo(PT_2D *pt, int info=-1);
	void MoveTo(PT_2D *pt, int info=-1);
	void Lines(PT_2D *pts, int npt, int sizeofpt=sizeof(PT_2D), int info=-1);
	void Dash( PT_2D pt0, PT_2D pt1, double len0, double len1);
	const Graph2d* HeadGraph() const;
	Graph2d* HeadGraph(); //用于修改

	BOOL IsOnlyType(int type);

	void SetAllColor(COLORREF color);
	void SetAllLineWidth(BOOL bGrdWid, float width);
	void SetAllCell(int cell);
	void SetAllLineStyle(int style);
	void SetAllPolygon(int type, int index, PT_2D *pts);
	void SetAllDrawFlag(int needdraw);
	void SetAllLineWidthOrWidthScale(BOOL bGrdWid, float width);
	
	void RefreshEnvelope(BOOL bCalcCompressInfo=FALSE);
	Envelope GetEnvelope()const;

	void AddBuffer(const GrBuffer2d *pBuf);
	void AddBuffer(const GrBuffer *pBuf);
	void CopyFrom(const GrBuffer2d *pBuf);
	void CutTo(GrBuffer2d *pBuf, BOOL bClearOld=TRUE);
	void InsertGraph(const Graph2d *pGr, float xoff,float yoff, BOOL bRecalcEvlp = FALSE);
	//直接Text后得到的文本不需要RecalcEvlp；但若文本闭包经过变换则必须重新计算
	void InsertGraph(const Graph *pGr, BOOL bRecalcEvlp = FALSE);
	int  GetLinePts(PT_2D *pts);
	int	 GetVertexPts(GrVertex2d *pts);
	void SetAllPts(const PT_2D *pts);

	int	 GetPtAttrs(GrVertex2d *pts, GrVertexAttr *attrs);
	void SetPtAttrs(GrVertexAttr *attrs);

	void DeleteAll();
	void DeleteGraph(const Graph2d *pGr0);
	void Transform(const double *m);
	void KickoffSamePoints();

	void Move(double dx, double dy=0);
	void Zoom(double kx, double ky);
	void Rotate(double angle, const PT_2D *pt=NULL);
	void ZoomCompressInfo(double scale);

	void ZoomPointSize(double kx, double ky);
	void ZoomWidth(double k);

	//读写二进制数据
	BOOL ReadFromBlob(const BYTE *pData, int nLen);
	BOOL WriteToBlob(BYTE *pData, int &nLen);

private:
	void AddGraph(Graph2d *pGr);
	void AddVertex(GrVertex2d *pt);
	COLORREF MatchColor(COLORREF clr);
	void CalcEnvelope(Graph2d *pGr);
	void SetCompressInfo(GrVertexList2d *pList);

private:
	float m_ptXoff, m_ptYoff;
	Graph2d	*m_pCurGr, *m_pHeadGr;
};


class GrStreamCompress2d
{
public:
	GrStreamCompress2d();
	virtual ~GrStreamCompress2d();
	
	void BeginCompress(double limit=-1);
	void EndCompress();
	int  AddStreamPt(GR_PT_2D pt);
	int  GetLastCompPt(GR_PT_2D &pt);
	int  GetCurCompPt(GR_PT_2D &pt);
	
	//跳过一个点
	inline void SkipPt(){ m_nSkipPt++; }
	inline void SetLimit(double limit){ m_lfLimit = limit;	}
	inline double GetLimit(){ return m_lfLimit;	}
	
private:
	double m_lfLimit;
	GR_PT_2D m_ptCurComp;
	GR_PT_2D m_ptLastComp;
	int m_nCurPt,m_nLastPt,m_nSkipPt;
	
	double m_lfMinX, m_lfMinY, m_lfMaxX, m_lfMaxY;
};


MyNameSpaceEnd


#endif /* _INC_GRBUFFER2D_41084E5603AD_INCLUDED */
