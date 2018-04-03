// Linearizer.h: interface for the CLinearizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_LINEARIZER__INCLUDED_)
#define _LINEARIZER__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrBuffer.h"
#include "SmartViewBaseType.h"

MyNameSpaceBegin

class CCoordSys;


//线对象的基线
class EXPORT_EDITBASE CShapeLine
{
public:
	struct ShapeLineUnit
	{
		PT_3DEX *pts;
		int nuse;
		Envelope evlp;
		ShapeLineUnit *next;

		BOOL FindNearestPt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *mindis)const;
	};
public:
	CShapeLine();
	~CShapeLine();

	inline ShapeLineUnit *HeadUnit()const {return m_pHead;}
	inline ShapeLineUnit *TailUnit()const {return m_pTail;}
	BOOL CopyFrom(const CShapeLine *pShape);
	BOOL AddShapeLine(const CShapeLine *pShape);
	void LinePt(PT_3D pt, int pencode, float wid=0, int type=ptNone);
	void LinePts(const PT_3DEX *pts, int num);
	void LineEnd();	
	void SetLastPtcd(int pencode);
	void SetFirstPtcd(int pencode);
	Envelope GetEnvelope()const;
	BOOL GetPts(CArray<PT_3DEX,PT_3DEX> &pts)const;
	BOOL GetKeyPts(CArray<PT_3DEX,PT_3DEX> &pts)const;
	PT_3DEX GetPt(int idx)const;
	int  GetPtsCount()const;
	int GetKeyPtsCount()const;

	BOOL GetShapeLineUnit(CPtrArray& arr)const;
    
	void Clear();
	BOOL ToGrBuffer(GrBuffer *buf)const;

	int FindNearestKeyPt(PT_3D pt)const;

	int FindPosofBaseLines(PT_3D pt)const;

	double GetLength(BOOL b2D=TRUE)const;
	double GetLength(PT_3D *testPt, BOOL bFirst=TRUE, BOOL b2D=TRUE)const;

	// search a line segment in an object
	BOOL FindNearestLine(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, 
		PT_3D *ret1, PT_3D *ret2, double *mindis, int *pKeyPos)const;
	
	// search a point in an object
	BOOL FindNearestPt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *mindis)const;
	virtual void GetConstGrBuffer(GrBuffer *pBuf, bool bClosed = false) const;
	virtual void GetVariantGrBuffer(GrBuffer *pBuf,bool bClosed = false, int varPt=-1, bool bEditOrInsert = false)const;
	virtual void GetAddConstGrBuffer(GrBuffer *pBuf, bool bClosed = false)const;
	BOOL GetKeyPosOfBaseLines(CArray<int,int> &arr)const;
	BOOL bIntersect(PT_3D pt1, PT_3D pt2) const;
	int  GetKeyPos(int basept_index)const;
private:
	BOOL ReallocLineUnit();
	BOOL GetPartShape(CArray<PT_3D,PT_3D> &pts,int idx)const;

private:
	ShapeLineUnit *m_pHead;
	ShapeLineUnit *m_pTail;
};

//线性化解释器
class EXPORT_EDITBASE CLinearizer
{
public:
	CLinearizer();
	virtual ~CLinearizer();
	void SetShapeBuf(CShapeLine *pBuf);
	void LineString(const PT_3DEX* pts, int nPts);
	BOOL Arc(const PT_3D *ptc, double radius, double sAngle, double mAngle, double eAngle, double dz, bool bclockwise);
	void Arc(const PT_3D *pts0, const PT_3D *pts1, const PT_3D *pts2);
	BOOL Arc(const PT_3D *ptc, double radius, double sAngle, double eAngle, double dz, bool bclockwise);
	void Spline(const PT_3DEX* pts, int nPts);

	void Linearize(const PT_3DEX* pts, int nPts, BOOL bClosed);
	static int GetTangency(PT_3D *ipt,PT_3DEX* pts, int nPts, BOOL bClosed, int nPos,PT_3D opt[2]);
	static BOOL GetCenter(PT_3DEX* pts,int nPts,BOOL bClosed, int nPos,PT_3D *opt);
	static int FindKeyPosOfBaseLine(PT_3D sch_pt, const CShapeLine *pBaseLine, CCoordSys *pCS, int *state);

	static float m_fPrecision;
	static double m_st;
private:
	CShapeLine *m_pBuf;
};

class EXPORT_EDITBASE CLinearizerPrecisionChange
{
public:
	CLinearizerPrecisionChange(float newValue);
	~CLinearizerPrecisionChange();

	float m_fPrecisionSaved;
};

class EXPORT_EDITBASE CLinearizerStChange
{
public:
	CLinearizerStChange(double newValue);
	~CLinearizerStChange();

	double m_lfStSaved;
};

MyNameSpaceEnd


//根据3点来计算圆心、半径、方向角、顺逆时针方向
template<class T>
bool CalcArcParamFrom3P(const T pts[3], T* c,double *r,double *ang, int *bclockwise)
{
	//中垂线交点即为圆心
	double x1 = (pts[0].x + pts[1].x)*0.5, y1 = (pts[0].y + pts[1].y)*0.5;
	double x2 = (pts[2].x + pts[1].x)*0.5, y2 = (pts[2].y + pts[1].y)*0.5;

	double vx1 = pts[0].y - pts[1].y, vy1 = pts[1].x - pts[0].x;
	double vx2 = pts[1].y - pts[2].y, vy2 = pts[2].x - pts[1].x;

	if( !GraphAPI::GGetLineIntersectLine(x1,y1,vx1,vy1,x2,y2,vx2,vy2,&c->x,&c->y,NULL) )
		return false;
	
	//半径
	*r = GraphAPI::GGet2DDisOf2P(*c,pts[0]); 
	
	//各个点的方向角
	if (ang)
	{
		for( int i=0; i<3; i++) 
		{
			ang[i] = GraphAPI::GGetAngle(c->x,c->y,pts[i].x,pts[i].y);
		}
	}
	
	//顺逆时针方向
	if( bclockwise )
	{
		*bclockwise = GraphAPI::GIsClockwise(ang[0],ang[1],ang[2]);
	}	
	
	return true;
}


//根据2点和其中一点上的径向方向来计算圆心、半径、方向角、顺逆时针方向
template<class T>
bool CalcArcParamFrom1Tan2P(double vx0, double vy0, const T pts[2], T *pc, double *r,double ang[2])
{
	double vx1=pts[1].x-pts[0].x, vy1=pts[1].y-pts[0].y;
	double x1=(pts[0].x+pts[1].x)/2, y1=(pts[0].y+pts[1].y)/2;
	if( !GraphAPI::GGetLineIntersectLine( pts[0].x, pts[0].y, vx0, vy0,
		x1,y1,-vy1,vx1,&pc->x,&pc->y,NULL) )
		return false;
	
	*r = sqrt((pc->x-pts[0].x)*(pc->x-pts[0].x)+(pc->y-pts[0].y)*(pc->y-pts[0].y));
	ang[0] = GraphAPI::GGetAngle(pc->x,pc->y,pts[0].x,pts[0].y);
	ang[1] = GraphAPI::GGetAngle(pc->x,pc->y,pts[1].x,pts[1].y);
	
	return true; 
}

//生成有理B样条曲线
void  EXPORT_EDITBASE rbspline(const EditBase::PT_3DEX *pts, int npts, int k, double *h, int npts2, EditBase::PT_3D *pts_out);

#endif // !defined(_LINEARIZER__INCLUDED_)
