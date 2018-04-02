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

//�ı����÷�ʽ
#define TAH_LEFT                      0x01
#define TAH_MID						  0x02
#define TAH_RIGHT                     0x04

#define TAV_TOP                       0x08
#define TAV_MID						  0x10
#define TAV_BOTTOM                    0x20
#define TAH_DOT						  0x40		//С������뷽ʽ

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

//ͼ�ζ���
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

//ͼ�ζ�����
struct EXPORT_SMARTVIEW GrVertexList2d
{
	GrVertexList2d(){
		pts=0; nlen=nuse=0;
		isCompress = 0;
	}
	DECLARE_MEMPOOL(GrVertexList2d);
	//ȥ������ռ�
	void CutSpace();
	void CopyFrom(const GrVertexList2d *list);
	void CopyFrom(const GrVertexList *list,float xoff,float yoff);

	//�����б�
	GrVertex2d *pts;

	//�����ڴ泤�Ⱥ͵�ǰʹ����
	DWORD nlen:16;
	DWORD nuse:15;
	//�Ƿ�����ѹ��
	DWORD isCompress:1;
};

class GrBuffer2d;

//ͼ�ε�λ
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
	//ͼԪ����
	DWORD type:4;

	//RGB ��ɫ
	DWORD color:24;

	//�Ƿ���Ҫ���ƣ�������ʱ��Ա��������ͼʱ��VectorLayer��ʱ����
	//Ϊ 0����ʾ����Ҫ����
	DWORD needdraw:1;

	//����ʱ�Ƿ�ʹ�� needdraw
	//Ϊ 0����ʾ��ʹ��
	DWORD use_needdraw:1;

	// �Ƿ�ʹ�ö�����ɫ
	DWORD bUseSelfcolor:1;

	// ʹ�ñ���ɫ����
	DWORD bUseBackColor:1;

	//��һ��ͼԪ������ָ��
	Graph2d *next;

	GrBuffer2d *owner;
};

//��״ͼ��
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
	float kx;    //x���ű���
	float ky;    //y���ű���
	float angle; //��ת��
	int bGrdSize:1; //�ߴ��Ƿ�����ʾ���ʱ仯
	int bFlat:1; //���㻹��Բ��
	int cell:24; //ͼԪ���ŵ�����(���緽�顢Բ�㡢���ο�ԲȦ��ʮ��˿�����ȵ�)
	float width; // ͼԪ�߿�Լ����width>=0��ʾָ���ͼԪʹ��ͳһ�߿�width��width<0��ʾָ���ͼԪ���߿����Ϊwidth
};


//�㴮״ͼ��
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
	
	GrVertexList2d ptlist; //��ת�Ǵ浽���code�ֶ���
	float xl,xh,yl,yh;
	float kx;	 //x���ű���
	float ky;	 //y���ű���
	int bGrdSize:1; //�ߴ��Ƿ�����ʾ���ʱ仯
	int bFlat:1; //���㻹��Բ��
	int cell:24; //ͼԪ���ŵ�����
	float width; // �߿�
};


//�����Եĵ㴮״ͼ��
struct EXPORT_SMARTVIEW GrPointString2dEx : public GrPointString2d
{	
	GrPointString2dEx(){
		type=GRAPH_TYPE_POINTSTRING2DEX;
	}
	
	GrVertexAttrList attrList; //������Ϣ
};

//��״ͼ��
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

	//�����б�
	GrVertexList2d ptlist;
	float xl,xh,yl,yh;
	float width;
	int bGrdWid:1;
	//����ϵ��
	float fDrawScale;
	int style:24; //���ͷ��ŵ�����������ǻ������ͣ��Ͱ��ջ������ͻ��ƣ����򣬾Ͱ���ʵ�߻���
};

#define POLYGON_FILLTYPE_COLOR				0
#define POLYGON_FILLTYPE_PATTERN			1
#define POLYGON_FILLTYPE_TEXTURE			2

#define COMP_SCALE0							8.0

//��״ͼ��
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
	int filltype:8;    //������ͣ���Ϊ����ɫ��䡢������䡢
	int index:24;  //�����ŵ���������Ӧ���ͷֱ�Ϊ����ʹ�á�����ID��
	PT_2D *pts;	   //��Ϊ�������ʱ���˲���Ϊ�������꣬����Ŀ�붥����Ŀһ��
};

//�ı�ͼ��
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
	Graph2d* HeadGraph(); //�����޸�

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
	//ֱ��Text��õ����ı�����ҪRecalcEvlp�������ı��հ������任��������¼���
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

	//��д����������
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
	
	//����һ����
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
