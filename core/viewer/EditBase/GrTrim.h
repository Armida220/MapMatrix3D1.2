// GrTrim.h: interface for the CGrTrim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRTRIM_H__887230F0_D042_48B2_8222_0DF2D1F66C24__INCLUDED_)
#define AFX_GRTRIM_H__887230F0_D042_48B2_8222_0DF2D1F66C24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewBaseType.h"
#include "GrBuffer.h"

//�ֱ��ǲü����߶����������桢���桢����Ĺ�ϵ
//�д���
#define TL_ERR				0
//��һ���ü������߶��м�
#define TL_MID1				0x01
//��һ���ü������߶ζ˵㣬�Ҵ���������
#define TL_ENDIN1			0x02
//��һ���ü������߶ζ˵㣬�Ҵ�������߽���
#define TL_ENDON1			0x04
//�ڶ����ü������߶��м�
#define TL_MID2				0x10
//�ڶ����ü������߶ζ˵㣬�Ҵ���������
#define TL_ENDIN2			0x20
//�ڶ����ü������߶ζ˵㣬�Ҵ�������߽���
#define TL_ENDON2			0x40

struct TRIM_LINE
{
	int type;
	PT_3D pt0, pt1;
};

struct TRIM_BLOCK
{
	double x0,x1,y0,y1;
};

class EXPORT_EDITBASE CGrTrim  
{
public:
	struct IS_RET
	{
		IS_RET();
		IS_RET(PT_3D a, double b);
		PT_3D pt;
		double t;
	};
	
	CGrTrim();
	virtual ~CGrTrim();

	void InitTrimPolygon(PT_3D *pts, int num, double wid);
	void InitTrimPolygons(PT_3DEX *pts, int num, double wid);

	BOOL Trim(const GrBuffer *buf, GrBuffer& ret);

	//��ͨ���ֿ����ж�
	BOOL Trim_notBlockOptimized(const GrBuffer *buf, GrBuffer& ret);

	BOOL TrimLine(const PT_3D *pt0, const PT_3D *pt1);
	int	 GetTrimedLinesNum();
	int  GetTrimedLine(int idx, PT_3D *pt0, PT_3D *pt1);
	static BOOL Intersect(const PT_3D *pt0, const PT_3D *pt1, const PT_3D *pt2, const PT_3D *pt3,
		PT_3D *ret, double *t, double *t23=NULL);

private:
	void AddTrimedLine(PT_3D *pt0, PT_3D *pt1, int type);
	void ClearTrimedLines();

	int GetSubPolygonStartPos(int idx);
	int GetSubPolygonCount();

	void AddBlock(double x0, double x1, double y0, double y1);
	BOOL bPtInBlocks(double x, double y);
	BOOL bPtInBlocks(double x, double y, double radius);
	void ClearBlocks();

	//������TRIMLINE��type����
	BOOL TrimLine_notTypeRet(const PT_3D *pt0, const PT_3D *pt1);
public:
	BOOL bPtInPolygon(const PT_3D *pt);

	//radius�������뾶��ֻ��block�Ż�������
	BOOL bPtInPolygon(const PT_3D *pt,double radius);
	int  bEnvelopeInPolygon(const Envelope& e);

public:
	BOOL m_bSimplyTrim;
	BOOL m_bIntersectHeight;

private:
	PT_3D *m_pPolyPts;
	int m_nPolyPtsNum;

	//��������ʱ����¼ÿ������εĽڵ���
	CArray<int,int> m_arrSubPolyPtsNum;

	TRIM_BLOCK *m_pBlocks, *m_pCurBlock;
	int m_nBlocksNum;
	int m_nBlocksMemLen;

	TRIM_LINE *m_pTrimedLines;
	int m_nTrimedLinesNum;
	int m_nTrimedLinesMemLen;
};

#endif // !defined(AFX_GRTRIM_H__887230F0_D042_48B2_8222_0DF2D1F66C24__INCLUDED_)
