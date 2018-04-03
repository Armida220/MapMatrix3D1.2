//////////////////////////////////////////////////////////////////////

#if !defined(_MEMDRAW_H)
#define _MEMDRAW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RAS_BUF					BYTE
#define TMPBUF_LEN				2048
#define TMPBUF_HALF				(TMPBUF_LEN>>1)

#define GRIDDIS_MUL				10	


MyNameSpaceBegin

struct MEdge 
{
	// 边表节点的下一节点的链指针
    MEdge *next;

	// 由于CPolyFill的代码都来自他人，以下成员参数的说明都是推测
	// 当前边 y 坐标起点和终点
    long yTop, yBot;
	// 在作扫描时边表节点的一些参数
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

//多边形填充类
class CPolyFill  
{
public:
	CPolyFill();
	virtual ~CPolyFill();

	/* 功能：填充多边形；
	 * 返回值：无；
	 * 参数：
	 * pts, [In], 多边形边界点的坐标；
	 * npt, [In], 多边形边界点的数目；
	 */	
	void FillPolygon(POINT *pts, int npt);

	/* 功能：填充带有多边界的面域；
	 * 返回值：无；
	 * 参数：
	 * pts, [In], 面域的所有边界点的坐标（依次存放）；
	 * npt, [In], 面域的各个边界的点的数目；
	 * num, [In], 面域的边界数目；
	 */
	void FillMultiPolygon(POINT *pts, int *npt, int num);

	//定义填充的有效区域，区域外不作填充
	RECT m_rcClip;
	
private:
	/* 功能：填充一条水平扫描线，并更新边表中各个节点的步进参数；
	 * 返回值：无；
	 * 参数：
	 * r, [In], （RAS_BUF就是BYTE）扫描线所在的内存地址；
	 * active, [In], 当前活动边表的头指针；
	 * curY, [In], 当前扫描线的y坐标；
	 * pat, [In], 填充模板，未使用；
	 */
	void DrawRuns(RAS_BUF *r, MEdge *active, long curY, RAS_BUF *pat);

	/* 功能：遍历活动边表，去掉扫描线已经越过的边节点，并加入扫描线新遇到的边节点；
	 * 返回值：新的活动边表的头指针；
	 * 参数：
	 * active, [In], 当前活动边表的头指针；
	 * curY, [In], 当前扫描线的y坐标；
	 */
	MEdge *UpdateActive(MEdge *active, long curY);

	/* 功能：建立边表；
	 * 返回值：无；
	 * 参数：
	 * p, [In], 多边形边界；
	 */
	void FillEdges(POLYGON *p);
		
	/* 功能：跳跃到某个y坐标上的扫描线；
	         该函数只用于在使用有效区域时，尽可能快地跳到有效区域的起点；
	 * 返回值：新的活动边表的头指针；
	 * 参数：
	 * y, [In], 目标的 y 坐标；
	 */
	MEdge *JumpToY(int y);

	/* 功能：将某个边表节点加入到边表；
	 * 返回值：无；
	 * 参数：
	 * e, [In], 增加的边表节点；
	 * yPos, [In], 边表节点的 y 坐标；
	 */
	void AddEdgeRecord(MEdge *e, int yPos);

	/* 功能：从边表获得某个 y 坐标的边表节点；
	 * 返回值：目标边表节点；
	 * 参数：
	 * yPos, [In], 目标边表节点的 y 坐标；
	 */
	MEdge *GetEdge(int yPos);

	/* 功能：新建一个边表节点，类似于 new MEdge 语句；
	 * 返回值：新的边表节点；
	 * 参数：无；
	 */
	MEdge *NewEdge();

	/* 功能：新建一个边表节点，类似于 delete p 语句；
	 * 返回值：无；
	 * 参数：无；
	 */	
	void FreeEdge(MEdge *p);

	BOOL ReAllocListMem();
	
	// 边表节点的按照其 y 坐标顺序的列表；用于提高查找速度；
	MEdgeRecord *m_list;
	int m_nListUse, m_nListLen;
};

//内存绘图类
class CMemDraw  
{
public:
	CMemDraw();
	~CMemDraw();

	/* 功能：指定用于绘图的位图内存块；
	         由于使用的是256色位图，所以每个象素就是一个字节；
			 内存块的大小也就可以计算为 size = wid * hei;
	 * 返回值：无；
	 * 参数：
	 * pBuf, [In], 位图内存块的地址；
	 * wid, [In], 位图的宽度；
	 * hei, [In], 位图的高度；
	 */
	void Init8BitMem(BYTE *pBuf, int wid, int hei);

	/* 功能：绘制点；
	 * 返回值：无；
	 * 参数：
	 * x, [In], 点的 x 坐标；
	 * y, [In], 点的 y 坐标；
	 * clr, [In], 点颜色的索引值；
	 * size, [In], 点的尺寸；
	 */
	void Point(int x, int y, BYTE clr, int size);

	/* 功能：设置线的风格；
	 * 返回值：无；
	 * 参数：
	 * clr, [In], 线颜色的索引值；
	 * wid, [In], 线宽；
	 * bRound, [In], 带宽度的线在线节点处是否圆形；
	 */
	void SetLineStyle(BYTE clr,int wid, BOOL bRound=FALSE);

	/* 功能：设置线风格的扩展函数，可设置多段颜色交替的线；
	 * 返回值：无；
	 * 参数：
	 * wid, [In], 线宽；
	 * nseg, [In], 交替颜色的数目；
	 * seg, [In], 交替颜色段的段长和颜色；
	 * bRound, [In], 带宽度的线在线节点处是否圆形；
	 */
	void SetLineStyleEx(int wid, int nseg, long *seg, BOOL bRound=FALSE);

	/* 功能：绘制线，类似 CDC::MoveTo 和 CDC::LineTo ；
	 * 返回值：无；
	 * 参数：略；
	 */
	inline void MoveTo(int x, int y){
		m_ptCurrent.x = x;	m_ptCurrent.y = y;
	}
	void LineTo(int x, int y);	

	/* 功能：绘制线，功能类似 MoveTo/LineTo，两者可以互换使用；
	 * 返回值：无；
	 * 参数：
	 * pts, [In], 点的列表；
	 * npt, [In], 点的数目；
	 */
	void Polyline(POINT *pts, int npt);

	/* 功能：填充多边形；
	 * 返回值：无；
	 * 参数：
	 * pts, [In], 多边形的边界点；
	 * npt, [In], 多边形的边界点数；
	 * clr, [In], 填充色；
	 */
	void Polygon(POINT *pts, int npt, BYTE clr);

	/* 功能：绘制圆周；
	 * 返回值：无；
	 * 参数：
	 * x, [In], 圆周圆心的 x 坐标；
	 * y, [In], 圆周圆心的 y 坐标；
	 * r, [In], 圆周的半径；
	 * clr, [In], 绘制的颜色；
	 * wid, [In], 绘制的线宽；
	 */
	void Circle(int x, int y, int r, BYTE clr, int wid);

protected:

	/* 功能：获得线宽刷子的点阵坐标，在绘制带宽度线时供Line和LineEx使用；
	 * 返回值：无；
	 * 参数：
	 * x1,y1, [In], 绘制的带宽度线的起点坐标；
	 * x2,y2, [In], 绘制的带宽度线的终点坐标；
	 * wid, [In], 绘制的线宽；
	 * nbuf, [Out], 点阵坐标的x、y数目；
	 * buf, [Out], 点阵坐标的x、y坐标值；
	 * nbuf2, [In], 刷子的修正点阵坐标的x、y数目；
	 * buf2, [In], 刷子的修正点阵坐标的x、y坐标值；
	 */
	void GetLineWidthBuf(int x1, int y1, int x2, int y2, int wid,
		int &nbuf, int *&buf, int &nbuf2, int *&buf2);

	/* 功能：绘线；
	 * 返回值：无；
	 * 参数：
	 * x1,y1, [In], 绘制的带宽度线的起点坐标；
	 * x2,y2, [In], 绘制的带宽度线的终点坐标；
	 * wid, [In], 绘制的线宽；
	 */
	void Line(int x1, int y1, int x2, int y2, int wid);

	/* 功能：绘制扩展线；
	 * 返回值：无；
	 * 参数：
	 * x1,y1, [In], 线的起点坐标；
	 * x2,y2, [In], 线的终点坐标；
	 * wid, [In], 线宽；
	 */
	void LineEx(int x1, int y1, int x2, int y2, int wid);

	/* 功能：在绘制扩展线时，每绘制一个点，此函数会被调用，用于计算该用扩展线型中的哪种交替颜色绘制；
	 * 返回值：无；
	 * 参数：
	 * len, 绘制当前点产生的步进长度；
	 */
	inline void StepPixel(int len)
	{
		if( m_nCurLen+len>=m_nNextLen )
		{
			m_nCurSeg = (m_nCurSeg+1)%m_nStyleSeg;
			m_nCurLen = m_nCurLen+len-m_nNextLen;
			m_nNextLen = (m_linestyle[(m_nCurSeg<<1)+1]<<GRIDDIS_MUL);
			m_color = (BYTE)m_linestyle[(m_nCurSeg<<1)];
		}
		else
		{
			m_nCurLen += len;
		}
	}

public:
	//用于绘制的内存块地址，宽，高
	BYTE *m_pMemBits;
	int m_nMemWid, m_nMemHei;

	//当前的线的颜色，宽度
	BYTE m_color;
	int m_width;

	//当前点，用于MoveTo、LineTo函数
	POINT m_ptCurrent;

	//当前的扩展线型风格和风格的段数，为0表示非扩展风格
	long m_linestyle[8];
	int m_nStyleSeg;

	//线折角处是否是圆形的
	BOOL m_bRoundLine;

	//多边形填充对象
	CPolyFill m_poly;
	
	//绘图的过程中使用的临时变量
	//////temporary data for drawing line with an extension style
	int m_nCurSeg, m_nNextLen;
	int m_nCurLen;
	//////

	//绘图的过程中使用的临时变量
	int m_buf[TMPBUF_LEN];
};

MyNameSpaceEnd

#endif // !defined(_MEMDRAW_H)

