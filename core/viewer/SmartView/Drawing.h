// Drawing.h: interface for the CDrawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWING_H__F2C13983_AB7E_48F6_94FB_1D91F6E90110__INCLUDED_)
#define AFX_DRAWING_H__F2C13983_AB7E_48F6_94FB_1D91F6E90110__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RAS						BYTE
#define TMPBUF_LEN				2048
#define TMPBUF_HALF				(TMPBUF_LEN>>1)

#define MAX_POLYRECORD			10240
#define MAX_POLYEDGE			10240
#define GRIDDIS_MUL				10	

struct MEdge 
{
    MEdge *next;
    long yTop, yBot;
    long xNowWhole, xNowNum, xNowDen, xNowDir, xEndWhole;
    long xNowNumStep;
};

struct MEdgeRecord
{
	MEdge *e;
	int pos;
};

struct POLYGON
{
	POINT *pts;
	int npt;
};


class CDrawing;
class CDrawingPixel
{
public:
	CDrawingPixel();
	~CDrawingPixel(){}
	void Set(CDrawing *p);	
	void ScanLine(int x1, int x2, int y);
	void SetPixel(int x, int y);
	void SetPixel(int x, int y, BYTE clr);
	
	//仅当 m_op 为 R2_COPYPEN 时，m_alpha 才有意义；
	//m_alpha为透明度，0，完全透明，256，完全不透明；
	int m_op, m_alpha;
	CDrawing *m_p;
};


//多边形填充类
class CPolyFill  
{
public:
	CPolyFill();
	virtual ~CPolyFill();
	
	void FillPolygon(POINT *pts, int npt);
	void FillMultiPolygon(POINT *pts, int *npt, int num);
	RECT m_rcClip;

	CDrawingPixel *m_pPixel;
	
private:
	void DrawRuns(RAS *r, MEdge *active, long curY, RAS *pat);
	MEdge *UpdateActive(MEdge *active, long curY);
	void FillEdges(POLYGON *p);
		
	MEdge *JumpToY(int y);
	void AddEdgeRecord(MEdge *e, int yPos);
	MEdge *GetEdge(int yPos);

	MEdge *NewEdge();
	void FreeEdge(MEdge *p);
	
	MEdgeRecord m_list[MAX_POLYRECORD];
	MEdge m_edgePool[MAX_POLYEDGE];
	int m_nListUse, m_nEdgeUse;
};


//内存绘图类
class CDrawing  
{
public:
	CDrawing();
	~CDrawing();
	void Rectangle(int x1, int y1, int x2, int y2, int z=0);
	BYTE GetColorFrom24Bit(COLORREF color);
	//指定绘图的目标内存
	void Init8BitMem(BYTE *pBuf, int wid, int hei, int bitCount);

	//点
	void Point(int x, int y, BYTE clr, int size);

	//设置线的风格
	//bRound 标志线的端点处是否圆形
	void SetLineStyle(BYTE clr,int wid, BOOL bRound=FALSE);
	void SetLineStyleEx0( int nseg, unsigned long *seg, BOOL bRound=FALSE);
	//设置线的扩展风格：即有多段颜色交替的线
	void SetLineStyleEx(int wid, int nseg, long *seg, BOOL bRound=FALSE);


	//绘线
	inline void MoveTo(int x, int y){
		m_ptCurrent.x = x;	m_ptCurrent.y = y;
	}
	void LineTo(int x, int y);	
	void Polyline(POINT *pts, int npt);

	//多边形填充
	void Polygon(POINT *pts, int npt, BYTE clr);

	// 多个面填充
	void PolyPolygon(POINT *pts, int *lpPolyCounts, int nPolyCount, BYTE clr);

	//圆周
	void Circle(int x, int y, int r, BYTE clr, int wid);

	void SetROP2(int op){
		m_pixel.m_op = op;
	}

	void SetAlpha(int alpha){
		m_pixel.m_alpha = alpha;
	}

	void SetColorTable(RGBQUAD *tbl, int num);

protected:

	//获得线宽刷子
	void GetLineWidthBuf(int x1, int y1, int x2, int y2, int wid,
		int &nbuf, int *&buf, int &nbuf2, int *&buf2);

	//绘线函数
	void Line(int x1, int y1, int x2, int y2, int wid);

	//绘制扩展线的函数
	void LineEx(int x1, int y1, int x2, int y2, int wid);

	//处理扩展线型的辅助函数
	void StepPixel(int len);

public:
	
	BYTE *m_pMemBits;

	int m_nMemWid, m_nMemHei, m_nBitCount;

	BYTE m_color;
	int m_width;
	POINT m_ptCurrent;
	long m_linestyle[8];
	int m_nStyleSeg;
	BOOL m_bRoundLine;
	
	//////temporary data for drawing line with an extension style
	int m_nCurSeg, m_nNextLen, m_nAllLen;
	int m_nCurLen;
	//////

	RGBQUAD			*m_pRGBQuad;
	CByteArray      m_recentUsedColorIDX;  //约定最多10个	
	CDWordArray     m_recentUsedColor;		//约定最多10个	
	CPolyFill m_poly;

	int m_buf[TMPBUF_LEN];

	CDrawingPixel m_pixel;
};

#endif // !defined(AFX_DRAWING_H__F2C13983_AB7E_48F6_94FB_1D91F6E90110__INCLUDED_)





















