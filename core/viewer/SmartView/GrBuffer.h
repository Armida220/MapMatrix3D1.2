// Copyright (C) 2004 - 2005 Gibuilder Group

#ifndef _INC_GRBUFFER_41084E5603AD_INCLUDED
#define _INC_GRBUFFER_41084E5603AD_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewBaseType.h"
#include "Envelope.h"
#include "MemoryPool.h"

#define TAH_LEFT                      0x01
#define TAH_MID						  0x02
#define TAH_RIGHT                     0x04

#define TAV_TOP                       0x08
#define TAV_MID						  0x10
#define TAV_BOTTOM                    0x20
#define TAH_DOT						  0x40		//小数点对齐方式

#define GRAPH_TYPE_INVALID				0
#define GRAPH_TYPE_POINT				1
#define GRAPH_TYPE_LINESTRING			2
#define GRAPH_TYPE_POLYGON				3
#define GRAPH_TYPE_TEXT					4
#define GRAPH_TYPE_POINTSTRING			5
#define GRAPH_TYPE_POINTSTRINGEX		6

#define IsGrPoint(gr)					((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POINT)
#define IsGrLineString(gr)				((gr)!=NULL&&(gr)->type==GRAPH_TYPE_LINESTRING)
#define IsGrPolygon(gr)					((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POLYGON)
#define IsGrText(gr)					((gr)!=NULL&&(gr)->type==GRAPH_TYPE_TEXT)
#define IsGrPointString(gr)				((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POINTSTRING)
#define IsGrPointStringEx(gr)			((gr)!=NULL&&(gr)->type==GRAPH_TYPE_POINTSTRINGEX)

#define GRBUFFER_PTCODE_MOVETO			0
#define GRBUFFER_PTCODE_LINETO			1

#define IsGrPtCodeMoveTo(pt)			(((pt)->code&1)==0)
#define IsGrPtCodeLineTo(pt)			(((pt)->code&1)==1)

#define GRPT_MARK_DIAMOND				0x100


MyNameSpaceBegin


//图形顶点
struct EXPORT_SMARTVIEW GrVertex : public PT_3D
{
	// flag this vertex is MoveTo or LineTo
	long code;

	//PointString 类型时，code 借用为点的角度；下面两个函数将 code 与 angle 互换
	inline float CodeToFloat()const{
		return code*0.00001f;
	}
	inline void CodeFromFloat(float a){
		code = a*100000;
	} 
};


struct GrVertexList2d;
struct GrColorVertexList2d;

//图形顶点列
struct EXPORT_SMARTVIEW GrVertexList
{
	GrVertexList(){
		pts=0; nlen=nuse=0;
		isCompress = 0;
	}

	//去除多余空间
	void CutSpace();
	void CopyFrom(const GrVertexList *list);
	void CopyFrom(const GrVertexList2d *list, double xoff, double yoff, double z);

	//顶点列表
	GrVertex *pts;

	//顶点内存长度和当前使用数
	DWORD nlen:16;
	DWORD nuse:15;
	DWORD isCompress:1;
};



//图形单位
struct EXPORT_SMARTVIEW Graph
{
	Graph(){
		next=NULL;color=0;
		type=GRAPH_TYPE_INVALID;
		bUseSelfcolor = 0;
		bUseBackColor = 0;
	}
	//图元类型
	DWORD type:4;

	//RGB 颜色
	DWORD color:24;

	// 是否使用独立颜色
	DWORD bUseSelfcolor:1;

	// 使用背景色绘制
	DWORD bUseBackColor:1;

	//下一个图元的链接指针
	Graph *next;
};



//点状图形
struct EXPORT_SMARTVIEW GrPoint : public Graph
{
	DECLARE_MEMPOOL(GrPoint);

	GrPoint(){
		kx = ky = 1; bGrdSize=TRUE;
		type=GRAPH_TYPE_POINT;
		cell = 0;
		width = 0;
		bFlat = 0;
	}
	PT_3D pt;
	float kx;	 //x缩放比例
	float ky;	 //y缩放比例
	float angle; //旋转角
	int bGrdSize:1; //尺寸是否随显示比率变化
	int bFlat:1; //方点还是圆点
	int cell:24; //图元符号的索引
	float width; // 线宽，约定：width>=0表示指向的图元使用统一线宽width，width<0表示指向的图元的线宽比例为width
};

//点串状图形
struct EXPORT_SMARTVIEW GrPointString : public Graph
{	
	GrPointString(){
		kx = ky = 1; bGrdSize=TRUE;
		type=GRAPH_TYPE_POINTSTRING;
		cell = 0;
		width = 0;
		bFlat = 0;
	}
	GrPoint GetPoint(int i)const;

	GrVertexList ptlist; //旋转角存到点的code字段中
	Envelope evlp;
	float kx;	 //x缩放比例
	float ky;	 //y缩放比例
	int bGrdSize:1; //尺寸是否随显示比率变化
	int bFlat:1; //方点还是圆点
	int cell:24; //图元符号的索引
	float width; // 线宽
};


//带属性的点串状图形
struct EXPORT_SMARTVIEW GrPointStringEx : public GrPointString
{	
	GrPointStringEx(){
		type=GRAPH_TYPE_POINTSTRINGEX;
	}
	
	GrVertexAttrList attrList; //属性信息
};

//线状图形
struct EXPORT_SMARTVIEW GrLineString : public Graph
{
	GrLineString(){
		width=0; bGrdWid=TRUE; style=0;
		type=GRAPH_TYPE_LINESTRING;

		fDrawScale = 0.0;

		evlp.CreateMaxEnvelope();
	}
	//顶点列表
	GrVertexList ptlist;
	Envelope evlp;
	float width;
	int bGrdWid:1;
	int style:24; //线型符号的索引，如果是基本线型，就按照基本线型绘制，否则，就按照实线绘制
	//比例系统
	float fDrawScale;
};

#define POLYGON_FILLTYPE_COLOR				0
#define POLYGON_FILLTYPE_PATTERN			1
#define POLYGON_FILLTYPE_TEXTURE			2

//面状图形
struct EXPORT_SMARTVIEW GrPolygon : public Graph
{
	GrPolygon(){
		pts=NULL;
		type=GRAPH_TYPE_POLYGON;
		filltype = POLYGON_FILLTYPE_COLOR;
		index = 0;

		evlp.CreateMaxEnvelope();
	}
	GrVertexList ptlist;
	Envelope evlp;
	int filltype:8;    //填充类型，分为：颜色填充、纹理填充、
	int index:24;  //填充符号的索引，对应类型分别为：透明度、纹理ID；
	PT_2D *pts;	   //当为纹理填充时，此参数为纹理坐标，点数目与顶点数目一致
};

//文本图形
struct EXPORT_SMARTVIEW GrText : public Graph
{
	GrText(){
		type=GRAPH_TYPE_TEXT;
		bGrdSize=TRUE;
		text=NULL;
		bRotateWithView = TRUE;
	}
	void Release();
	void SetText(LPCTSTR str);
	void SetSettings(const TextSettings *textSettings);
	BOOL CompareFont(const GrText *t)const;
	PT_3D pt;
	float  x[4],y[4],zmin,zmax;//以pt点为基点
	DWORD bGrdSize:1;
	DWORD bRotateWithView:1;
	TCHAR *text;
	TextSettings settings;	
};

Envelope EXPORT_SMARTVIEW GetEnvelopeOfGraph(const Graph *pGr);
Envelope EXPORT_SMARTVIEW GetEnvelopeOfGraph_fast(const Graph *pGr, float fDrawCellAngle=0, float fDrawCellKX=1, float fDrawCellKY=1, float gscale=1);
Envelope EXPORT_SMARTVIEW GetEnvelopeOfPointInString(const Graph *pGr);
Envelope EXPORT_SMARTVIEW GetEnvelopeOfPointInString_fast(const Graph *pGr, float fDrawCellAngle=0, float fDrawCellKX=1, float fDrawCellKY=1);

// search about Graph
double EXPORT_SMARTVIEW FindNearestVertex(PT_3D sch_pt, Envelope sch_evlp, const Graph *pGr, CCoordSys *pCS, PT_3D *pRet);
double EXPORT_SMARTVIEW FindMinimumDistance(PT_3D sch_pt, const Graph *pGr, CCoordSys *pCS, PT_3D *pRet);
BOOL EXPORT_SMARTVIEW FindNearestLine(PT_3D sch_pt, const Graph *pGr, CCoordSys *pCS, GrVertex *pRet1, GrVertex *pRet2, double *mindis);

void EXPORT_SMARTVIEW DeleteGraph(Graph *p);
class GrBuffer2d;
struct Graph2d;

class EXPORT_SMARTVIEW GrBuffer 
{
public:	

	GrBuffer();
	~GrBuffer();
	void Text(COLORREF clr, PT_3D *pt, LPCTSTR text,  const TextSettings *settings, BOOL bGrdSize=TRUE, BOOL bRotateWithView=TRUE, BOOL bCalcEnvelope=TRUE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void Point(COLORREF clr,PT_3D *pt,float kx,float ky, float fAngle=0, BOOL bGrdSize=TRUE, int cell=0, float width=0, BOOL bFlat=FALSE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);	
	void BeginPolygon(COLORREF clr,int type=POLYGON_FILLTYPE_COLOR, int index=0, const PT_2D *pts=NULL, int nPt=0, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void BeginLineString(COLORREF clr, float width, BOOL bGrdWid=TRUE, int style=0, float fDrawScale=1.0, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void End(BOOL bCompress=TRUE);
	void BeginPointString(COLORREF clr,float kx,float ky, BOOL bGrdSize=TRUE, int cell=0, float width=0, BOOL bFlat=FALSE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);
	void BeginPointStringEx(COLORREF clr,float kx,float ky, BOOL bGrdSize=TRUE, int cell=0, float width=0, BOOL bFlat=FALSE, BOOL bUseSelfcolor=FALSE, BOOL bUseBackCol=FALSE);	
	void PointString(PT_3D *pt, float fAngle=0);	
	void PointStringEx(PT_3D *pt, COLORREF color, BOOL isMark, float fAngle=0);
	void LineTo(PT_3D *pt, int info=-1);
	void MoveTo(PT_3D *pt, int info=-1);
	void Lines(PT_3D *pts, int npt, int sizeofpt=sizeof(PT_3D), int info=-1);
	void Lines(GrVertex *pts, int npt);
	void Dash( PT_3D pt0, PT_3D pt1, double len0, double len1);
	const Graph* HeadGraph() const; // 用于读取
	Graph* HeadGraph(); //用于修改

	BOOL IsOnlyType(int type);

	void SetAllColor(COLORREF color);
	void SetAllLineWidth(BOOL bGrdWid, float width);
	void SetAllCell(int cell);
	void SetAllLineStyle(int style);
	void SetAllPolygon(int type, int index, PT_2D *pts);
	void SetAllBackColor(BOOL bUseBackColor);

	void SetAllLineWidthOrWidthScale(BOOL bGrdWid, float width);

	void RefreshEnvelope();
	Envelope GetEnvelope()const;

	void AddBuffer(const GrBuffer *pBuf);
	void AddBuffer(const GrBuffer2d *pBuf, double z);
	void CopyFrom(const GrBuffer *pBuf);
	void CutTo(GrBuffer *pBuf, BOOL bClearOld=TRUE);
	void InsertGraph(const Graph *pGr, BOOL bRecalcEvlp = FALSE);
	void InsertGraph(const Graph2d *pGr, double xoff, double yoff, double z, BOOL bRecalcEvlp = FALSE);

	int  GetGraphPts(Graph *pGr, GrVertex *&pts);
	int  GetLinePts(PT_3D *pts,int sizeofpt=sizeof(PT_3D));
	int  GetVertexPts(GrVertex *pts);
	void SetAllPts(const PT_3D *pts);
	PT_3D GetFirstPt()const;

	int	 GetPtAttrs(GrVertex *pts, GrVertexAttr *attrs);
	void SetPtAttrs(GrVertexAttr *attrs);

	void DeleteAll();
	void DeleteGraph(const Graph *pGr0);
	void Transform(const double *m);
	void KickoffSamePoints();

	void Move(double dx, double dy=0, double dz=0);
	void Zoom(double kx, double ky, double kz=1.0);
	void Rotate(double angle, const PT_3D *pt=NULL, const PT_3D *dpt=NULL);
	
	//使用限制：angle值必须是立体的旋转角度，否则，该接口无意义。仅仅改变GrText的旋转角参数，对初始闭包不做变换。
	void RotateGrTextPar(double angle);
	//与上面函数正好相反
	void RotateGrTextEnvelope(double angle);
	//使用限制：此函数仅仅对文本的闭包初始值进行变换，不改变文本参数。
	void ScaleGrText(double lfXscale, double lfYscale);
	
	void TestGrText();//测试用

	void GetAllGraph(GrBuffer *buf);
	void ZoomCompressInfo(double scale);

	void ZoomPointSize(double kx, double ky);
	void ZoomWidth(double k);

	//合并线串；优化效率的函数；用来将比较碎小的GrLineString合并
	void MergeLineString();

private:
	void AddGraph(Graph *pGr);
	void AddVertex(GrVertex *pt);
	COLORREF MatchColor(COLORREF clr);
	void CalcEnvelope(Graph *pGr);
	void SetCompressInfo( GrVertexList*pList);

private:
	Graph	*m_pCurGr, *m_pHeadGr;
};

template<class T>
class SimpleStreamCompress
{
public:
	SimpleStreamCompress();
	~SimpleStreamCompress();
	
	void BeginCompress(double limit=-1);
	void EndCompress();

	int  AddStreamPt(T pt);
	int  GetLastCompPt(T &pt);
	int  GetCurCompPt(T &pt);
	
	//跳过一个点
	inline void SkipPt(){ m_nSkipPt++; }
	inline void SetLimit(double limit){ m_lfLimit = limit;	}
	inline double GetLimit(){ return m_lfLimit;	}
	
private:
	double m_lfLimit;
	T m_ptCurComp;
	T m_ptLastComp;
	int m_nCurPt,m_nLastPt,m_nSkipPt;
	
	double m_lfMinX, m_lfMinY, m_lfMaxX, m_lfMaxY;
};

template<class T>
SimpleStreamCompress<T>::SimpleStreamCompress()
{
	m_nLastPt= 0;
	m_nCurPt = -1;
	m_nSkipPt= 0;
	m_lfLimit= -1;
	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;
}
template<class T>
SimpleStreamCompress<T>::~SimpleStreamCompress()
{
	
}
template<class T>
void SimpleStreamCompress<T>::BeginCompress(double limit)
{
	if( limit>0 )m_lfLimit = limit;
	m_nCurPt = 0;
	m_nLastPt= 0;
	m_nSkipPt= 0;
	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;
}

template<class T>
void SimpleStreamCompress<T>::EndCompress()
{
	m_nCurPt = -1;
}
template<class T>
int SimpleStreamCompress<T>::GetLastCompPt(T &pt)
{
	if( m_nLastPt>=1 )
	{
		pt = m_ptLastComp;
	}
	return m_nLastPt;
}
template<class T>
int SimpleStreamCompress<T>::GetCurCompPt(T &pt)
{
	if( m_nCurPt>=1 )
	{
		pt = m_ptCurComp;
	}
	return m_nCurPt;
}

#define _FABS(x)	((x)>=0?(x):(-(x)))

template<class T>
int  SimpleStreamCompress<T>::AddStreamPt(T pt)
{
	if( m_nCurPt<0 )
		return 0;
	
	if( m_nCurPt==0 )
	{
		m_ptCurComp = pt;

		m_nCurPt += m_nSkipPt;
		m_nSkipPt = 0;
		m_nCurPt++;
		return 1;
	}
	
	if( m_nLastPt==0 )
	{
		m_ptLastComp = m_ptCurComp;
		m_ptCurComp = pt;

		m_nLastPt = m_nCurPt;
		m_nCurPt += m_nSkipPt;
		m_nSkipPt = 0;
		m_nCurPt++;
		return 2;
	}

	//求 m_ptCurComp 在 m_ptLastComp-pt 直线上的垂足
	double x,y,z,t;
	{
		double dx,dy,dz;
		
		dx = pt.x-m_ptLastComp.x; dy = pt.y-m_ptLastComp.y; dz = pt.z-m_ptLastComp.z;
		if( _FABS(dx)<1e-10 && _FABS(dy)<1e-10 && _FABS(dz)<1e-10 )
		{
			x = m_ptLastComp.x; y = m_ptLastComp.y; z = m_ptLastComp.z;
			t = 0;
		}
		else
		{

			t = ((m_ptCurComp.x-m_ptLastComp.x)*dx+(m_ptCurComp.y-m_ptLastComp.y)*dy+(m_ptCurComp.z-m_ptLastComp.z)*dz)/
				(dx*dx+dy*dy+dz*dz);

			x = m_ptLastComp.x + t * dx; y = m_ptLastComp.y + t * dy; z = m_ptLastComp.z + t * dz;
		}
	}

	//判断 m_ptCurComp 在m_ptLastComp-pt 直线上的方位(是向左还是向右)
	bool bclockwise = ((m_ptCurComp.x-m_ptLastComp.x)*(pt.y-m_ptCurComp.y)-(pt.x-m_ptCurComp.x)*(m_ptCurComp.y-m_ptLastComp.y))<0;

	//计算各个方向的偏距
	double disy = _FABS(x-m_ptCurComp.x)+_FABS(y-m_ptCurComp.y)+_FABS(z-m_ptCurComp.z);
	double disx = _FABS(x-pt.x)+_FABS(y-pt.y)+_FABS(z-pt.z);

	if( t>=1.0 || t<0.0 )
	{
		m_lfMaxX += disx;
		if( m_lfMinX>=disx )m_lfMinX -= disx;
		else m_lfMinX = 0;
	}
	else
	{
		m_lfMinX += disx;
		if( m_lfMaxX>=disx )m_lfMaxX -= disx;
		else m_lfMaxX = 0;
	}

	if( bclockwise )
	{
		m_lfMaxY += disy;
		if( m_lfMinY>=disy )m_lfMinY -= disy;
		else m_lfMinY = 0;
	}
	else
	{
		m_lfMinY += disy;	
		if( m_lfMaxY>=disy )m_lfMaxY -= disy;
		else m_lfMaxY = 0;
	}

	double minX = m_lfMaxX;
	if( minX>=m_lfLimit || m_lfMinY>=m_lfLimit || m_lfMaxY>=m_lfLimit )
	{
		m_ptLastComp = m_ptCurComp;
		m_ptCurComp = pt;
		m_lfMaxX = 0;
		m_lfMinX = m_lfMinY = m_lfMaxY = 0;

		m_nLastPt = m_nCurPt;
		m_nCurPt += m_nSkipPt;
		m_nSkipPt = 0;
		m_nCurPt++;

		return 2;
	}

	m_nCurPt += m_nSkipPt;
	m_nSkipPt = 0;
	m_nCurPt++;

	m_ptCurComp = pt;
	return 1;
}

template<class T,class T2>
int SSC_AddStreamPt(SimpleStreamCompress<T>& s, T2 pt2)
{
	T pt;
	pt.x = pt2.x; pt.y = pt2.y; pt.z = pt2.z;
	return s.AddStreamPt(pt);
}



template<class T>
void SimpleCompressPoints(CArray<T,T>& arrPts, float tolerance)
{
	if( tolerance<=0.0 )
		return;
	
	SimpleStreamCompress<T> comp;
	
	CArray<T,T> arrPts2;
	comp.BeginCompress(tolerance);
	
	T pt;
	T *pts = arrPts.GetData();
	int npt = arrPts.GetSize();
	for( int i=0; i<npt; i++)
	{
		if( comp.AddStreamPt(pts[i])==2 )
		{
			comp.GetLastCompPt(pt);
			arrPts2.Add(pt);
		}
	}
	
	if( npt>0 )
	{
		comp.GetCurCompPt(pt);
		arrPts2.Add(pt);
	}
	
	arrPts.Copy(arrPts2);
}



template<class T>
int SimpleCompressPoints(T* pts, int npt, float tolerance)
{
	if( tolerance<=0.0 )
		return npt;
	
	SimpleStreamCompress<T> comp;
	
	CArray<T,T> arrPts2;
	comp.BeginCompress(tolerance);
	
	T pt;
	for( int i=0; i<npt; i++)
	{
		if( comp.AddStreamPt(pts[i])==2 )
		{
			comp.GetLastCompPt(pt);
			arrPts2.Add(pt);
		}
	}
	
	if( npt>0 )
	{
		comp.GetCurCompPt(pt);
		arrPts2.Add(pt);
	}
	
	int npt2 = arrPts2.GetSize();
	memcpy(pts,arrPts2.GetData(),npt2*sizeof(T));

	return npt2;
}


// search about GrBuffer
double EXPORT_SMARTVIEW FindNearestVertex(PT_3D sch_pt, Envelope sch_evlp, const GrBuffer *pGr, CCoordSys* pCS, PT_3D *pRet);
double EXPORT_SMARTVIEW FindMinimumDistance(PT_3D sch_pt, const GrBuffer *pGr, CCoordSys* pCS, PT_3D *pRet, double *ret_real_mindis);
BOOL EXPORT_SMARTVIEW FindNearestLine(PT_3D sch_pt, const GrBuffer *pGr, CCoordSys* pCS, GrVertex *pRet1, GrVertex *pRet2, double *mindis);

int EXPORT_SMARTVIEW LayoutGrBufferByLines( PT_3D *linespt, int lineptnum, int sizeofpt, 
					 int& curpt, double& offlen,
					 GrBuffer *pBuf, double uselen);

MyNameSpaceEnd


#endif /* _INC_GRBUFFER_41084E5603AD_INCLUDED */
