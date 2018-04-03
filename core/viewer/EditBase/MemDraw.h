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
	// �߱�ڵ����һ�ڵ����ָ��
    MEdge *next;

	// ����CPolyFill�Ĵ��붼�������ˣ����³�Ա������˵�������Ʋ�
	// ��ǰ�� y ���������յ�
    long yTop, yBot;
	// ����ɨ��ʱ�߱�ڵ��һЩ����
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

//����������
class CPolyFill  
{
public:
	CPolyFill();
	virtual ~CPolyFill();

	/* ���ܣ�������Σ�
	 * ����ֵ���ޣ�
	 * ������
	 * pts, [In], ����α߽������ꣻ
	 * npt, [In], ����α߽�����Ŀ��
	 */	
	void FillPolygon(POINT *pts, int npt);

	/* ���ܣ������ж�߽������
	 * ����ֵ���ޣ�
	 * ������
	 * pts, [In], ��������б߽������꣨���δ�ţ���
	 * npt, [In], ����ĸ����߽�ĵ����Ŀ��
	 * num, [In], ����ı߽���Ŀ��
	 */
	void FillMultiPolygon(POINT *pts, int *npt, int num);

	//����������Ч���������ⲻ�����
	RECT m_rcClip;
	
private:
	/* ���ܣ����һ��ˮƽɨ���ߣ������±߱��и����ڵ�Ĳ���������
	 * ����ֵ���ޣ�
	 * ������
	 * r, [In], ��RAS_BUF����BYTE��ɨ�������ڵ��ڴ��ַ��
	 * active, [In], ��ǰ��߱��ͷָ�룻
	 * curY, [In], ��ǰɨ���ߵ�y���ꣻ
	 * pat, [In], ���ģ�壬δʹ�ã�
	 */
	void DrawRuns(RAS_BUF *r, MEdge *active, long curY, RAS_BUF *pat);

	/* ���ܣ�������߱�ȥ��ɨ�����Ѿ�Խ���ı߽ڵ㣬������ɨ�����������ı߽ڵ㣻
	 * ����ֵ���µĻ�߱��ͷָ�룻
	 * ������
	 * active, [In], ��ǰ��߱��ͷָ�룻
	 * curY, [In], ��ǰɨ���ߵ�y���ꣻ
	 */
	MEdge *UpdateActive(MEdge *active, long curY);

	/* ���ܣ������߱�
	 * ����ֵ���ޣ�
	 * ������
	 * p, [In], ����α߽磻
	 */
	void FillEdges(POLYGON *p);
		
	/* ���ܣ���Ծ��ĳ��y�����ϵ�ɨ���ߣ�
	         �ú���ֻ������ʹ����Ч����ʱ�������ܿ��������Ч�������㣻
	 * ����ֵ���µĻ�߱��ͷָ�룻
	 * ������
	 * y, [In], Ŀ��� y ���ꣻ
	 */
	MEdge *JumpToY(int y);

	/* ���ܣ���ĳ���߱�ڵ���뵽�߱�
	 * ����ֵ���ޣ�
	 * ������
	 * e, [In], ���ӵı߱�ڵ㣻
	 * yPos, [In], �߱�ڵ�� y ���ꣻ
	 */
	void AddEdgeRecord(MEdge *e, int yPos);

	/* ���ܣ��ӱ߱���ĳ�� y ����ı߱�ڵ㣻
	 * ����ֵ��Ŀ��߱�ڵ㣻
	 * ������
	 * yPos, [In], Ŀ��߱�ڵ�� y ���ꣻ
	 */
	MEdge *GetEdge(int yPos);

	/* ���ܣ��½�һ���߱�ڵ㣬������ new MEdge ��䣻
	 * ����ֵ���µı߱�ڵ㣻
	 * �������ޣ�
	 */
	MEdge *NewEdge();

	/* ���ܣ��½�һ���߱�ڵ㣬������ delete p ��䣻
	 * ����ֵ���ޣ�
	 * �������ޣ�
	 */	
	void FreeEdge(MEdge *p);

	BOOL ReAllocListMem();
	
	// �߱�ڵ�İ����� y ����˳����б�������߲����ٶȣ�
	MEdgeRecord *m_list;
	int m_nListUse, m_nListLen;
};

//�ڴ��ͼ��
class CMemDraw  
{
public:
	CMemDraw();
	~CMemDraw();

	/* ���ܣ�ָ�����ڻ�ͼ��λͼ�ڴ�飻
	         ����ʹ�õ���256ɫλͼ������ÿ�����ؾ���һ���ֽڣ�
			 �ڴ��Ĵ�СҲ�Ϳ��Լ���Ϊ size = wid * hei;
	 * ����ֵ���ޣ�
	 * ������
	 * pBuf, [In], λͼ�ڴ��ĵ�ַ��
	 * wid, [In], λͼ�Ŀ�ȣ�
	 * hei, [In], λͼ�ĸ߶ȣ�
	 */
	void Init8BitMem(BYTE *pBuf, int wid, int hei);

	/* ���ܣ����Ƶ㣻
	 * ����ֵ���ޣ�
	 * ������
	 * x, [In], ��� x ���ꣻ
	 * y, [In], ��� y ���ꣻ
	 * clr, [In], ����ɫ������ֵ��
	 * size, [In], ��ĳߴ磻
	 */
	void Point(int x, int y, BYTE clr, int size);

	/* ���ܣ������ߵķ��
	 * ����ֵ���ޣ�
	 * ������
	 * clr, [In], ����ɫ������ֵ��
	 * wid, [In], �߿�
	 * bRound, [In], ����ȵ������߽ڵ㴦�Ƿ�Բ�Σ�
	 */
	void SetLineStyle(BYTE clr,int wid, BOOL bRound=FALSE);

	/* ���ܣ������߷�����չ�����������ö����ɫ������ߣ�
	 * ����ֵ���ޣ�
	 * ������
	 * wid, [In], �߿�
	 * nseg, [In], ������ɫ����Ŀ��
	 * seg, [In], ������ɫ�εĶγ�����ɫ��
	 * bRound, [In], ����ȵ������߽ڵ㴦�Ƿ�Բ�Σ�
	 */
	void SetLineStyleEx(int wid, int nseg, long *seg, BOOL bRound=FALSE);

	/* ���ܣ������ߣ����� CDC::MoveTo �� CDC::LineTo ��
	 * ����ֵ���ޣ�
	 * �������ԣ�
	 */
	inline void MoveTo(int x, int y){
		m_ptCurrent.x = x;	m_ptCurrent.y = y;
	}
	void LineTo(int x, int y);	

	/* ���ܣ������ߣ��������� MoveTo/LineTo�����߿��Ի���ʹ�ã�
	 * ����ֵ���ޣ�
	 * ������
	 * pts, [In], ����б�
	 * npt, [In], �����Ŀ��
	 */
	void Polyline(POINT *pts, int npt);

	/* ���ܣ�������Σ�
	 * ����ֵ���ޣ�
	 * ������
	 * pts, [In], ����εı߽�㣻
	 * npt, [In], ����εı߽������
	 * clr, [In], ���ɫ��
	 */
	void Polygon(POINT *pts, int npt, BYTE clr);

	/* ���ܣ�����Բ�ܣ�
	 * ����ֵ���ޣ�
	 * ������
	 * x, [In], Բ��Բ�ĵ� x ���ꣻ
	 * y, [In], Բ��Բ�ĵ� y ���ꣻ
	 * r, [In], Բ�ܵİ뾶��
	 * clr, [In], ���Ƶ���ɫ��
	 * wid, [In], ���Ƶ��߿�
	 */
	void Circle(int x, int y, int r, BYTE clr, int wid);

protected:

	/* ���ܣ�����߿�ˢ�ӵĵ������꣬�ڻ��ƴ������ʱ��Line��LineExʹ�ã�
	 * ����ֵ���ޣ�
	 * ������
	 * x1,y1, [In], ���ƵĴ�����ߵ�������ꣻ
	 * x2,y2, [In], ���ƵĴ�����ߵ��յ����ꣻ
	 * wid, [In], ���Ƶ��߿�
	 * nbuf, [Out], ���������x��y��Ŀ��
	 * buf, [Out], ���������x��y����ֵ��
	 * nbuf2, [In], ˢ�ӵ��������������x��y��Ŀ��
	 * buf2, [In], ˢ�ӵ��������������x��y����ֵ��
	 */
	void GetLineWidthBuf(int x1, int y1, int x2, int y2, int wid,
		int &nbuf, int *&buf, int &nbuf2, int *&buf2);

	/* ���ܣ����ߣ�
	 * ����ֵ���ޣ�
	 * ������
	 * x1,y1, [In], ���ƵĴ�����ߵ�������ꣻ
	 * x2,y2, [In], ���ƵĴ�����ߵ��յ����ꣻ
	 * wid, [In], ���Ƶ��߿�
	 */
	void Line(int x1, int y1, int x2, int y2, int wid);

	/* ���ܣ�������չ�ߣ�
	 * ����ֵ���ޣ�
	 * ������
	 * x1,y1, [In], �ߵ�������ꣻ
	 * x2,y2, [In], �ߵ��յ����ꣻ
	 * wid, [In], �߿�
	 */
	void LineEx(int x1, int y1, int x2, int y2, int wid);

	/* ���ܣ��ڻ�����չ��ʱ��ÿ����һ���㣬�˺����ᱻ���ã����ڼ��������չ�����е����ֽ�����ɫ���ƣ�
	 * ����ֵ���ޣ�
	 * ������
	 * len, ���Ƶ�ǰ������Ĳ������ȣ�
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
	//���ڻ��Ƶ��ڴ���ַ������
	BYTE *m_pMemBits;
	int m_nMemWid, m_nMemHei;

	//��ǰ���ߵ���ɫ�����
	BYTE m_color;
	int m_width;

	//��ǰ�㣬����MoveTo��LineTo����
	POINT m_ptCurrent;

	//��ǰ����չ���ͷ��ͷ��Ķ�����Ϊ0��ʾ����չ���
	long m_linestyle[8];
	int m_nStyleSeg;

	//���۽Ǵ��Ƿ���Բ�ε�
	BOOL m_bRoundLine;

	//�����������
	CPolyFill m_poly;
	
	//��ͼ�Ĺ�����ʹ�õ���ʱ����
	//////temporary data for drawing line with an extension style
	int m_nCurSeg, m_nNextLen;
	int m_nCurLen;
	//////

	//��ͼ�Ĺ�����ʹ�õ���ʱ����
	int m_buf[TMPBUF_LEN];
};

MyNameSpaceEnd

#endif // !defined(_MEMDRAW_H)

