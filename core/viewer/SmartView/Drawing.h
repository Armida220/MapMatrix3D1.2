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
	
	//���� m_op Ϊ R2_COPYPEN ʱ��m_alpha �������壻
	//m_alphaΪ͸���ȣ�0����ȫ͸����256����ȫ��͸����
	int m_op, m_alpha;
	CDrawing *m_p;
};


//����������
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


//�ڴ��ͼ��
class CDrawing  
{
public:
	CDrawing();
	~CDrawing();
	void Rectangle(int x1, int y1, int x2, int y2, int z=0);
	BYTE GetColorFrom24Bit(COLORREF color);
	//ָ����ͼ��Ŀ���ڴ�
	void Init8BitMem(BYTE *pBuf, int wid, int hei, int bitCount);

	//��
	void Point(int x, int y, BYTE clr, int size);

	//�����ߵķ��
	//bRound ��־�ߵĶ˵㴦�Ƿ�Բ��
	void SetLineStyle(BYTE clr,int wid, BOOL bRound=FALSE);
	void SetLineStyleEx0( int nseg, unsigned long *seg, BOOL bRound=FALSE);
	//�����ߵ���չ��񣺼��ж����ɫ�������
	void SetLineStyleEx(int wid, int nseg, long *seg, BOOL bRound=FALSE);


	//����
	inline void MoveTo(int x, int y){
		m_ptCurrent.x = x;	m_ptCurrent.y = y;
	}
	void LineTo(int x, int y);	
	void Polyline(POINT *pts, int npt);

	//��������
	void Polygon(POINT *pts, int npt, BYTE clr);

	// ��������
	void PolyPolygon(POINT *pts, int *lpPolyCounts, int nPolyCount, BYTE clr);

	//Բ��
	void Circle(int x, int y, int r, BYTE clr, int wid);

	void SetROP2(int op){
		m_pixel.m_op = op;
	}

	void SetAlpha(int alpha){
		m_pixel.m_alpha = alpha;
	}

	void SetColorTable(RGBQUAD *tbl, int num);

protected:

	//����߿�ˢ��
	void GetLineWidthBuf(int x1, int y1, int x2, int y2, int wid,
		int &nbuf, int *&buf, int &nbuf2, int *&buf2);

	//���ߺ���
	void Line(int x1, int y1, int x2, int y2, int wid);

	//������չ�ߵĺ���
	void LineEx(int x1, int y1, int x2, int y2, int wid);

	//������չ���͵ĸ�������
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
	CByteArray      m_recentUsedColorIDX;  //Լ�����10��	
	CDWordArray     m_recentUsedColor;		//Լ�����10��	
	CPolyFill m_poly;

	int m_buf[TMPBUF_LEN];

	CDrawingPixel m_pixel;
};

#endif // !defined(AFX_DRAWING_H__F2C13983_AB7E_48F6_94FB_1D91F6E90110__INCLUDED_)





















