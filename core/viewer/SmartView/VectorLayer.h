// VectorLayer.h: interface for the CVectorLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWINGVECTOR2D_H__CCE8F7C6_218A_4AE4_B9A4_D57ABF84172A__INCLUDED_)
#define AFX_DRAWINGVECTOR2D_H__CCE8F7C6_218A_4AE4_B9A4_D57ABF84172A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingLayer.h"
#include "FArray.hpp"
#include "GrElementList.h"
#include "Graph2dSearch.h"

MyNameSpaceBegin

// ʸ��ͼ���cache���ֲ��ԣ�
// ��ʸ�����ݵ��������ת�����ͻ�����ϵ�µ�����������½ǵ�(x,y��Сֵ)��Ϊԭ�㣬
// ��cache��Ŀ�ߴ���Ϊ���ֵĳߴ磬���ÿ��cache��Ĵ洢��������ڿͻ���ʾ���ڣ�
// ֻ�Ǵ���һ��xyƫ�ơ�

class EXPORT_SMARTVIEW CVectorLayer : public CDrawingLayer  
{
	DECLARE_DYNAMIC(CVectorLayer)
public:
	enum DataType
	{
		typeNone = 0,
		typeGrBuffer2d = 1,
		typeGrBuffer = 2
	};

	CVectorLayer(int nDataType);
	virtual ~CVectorLayer();

	void SetDataType(int nDataType);
	int GetDataType();
	virtual void Destroy();

	void *OpenObj(LONG_PTR objnum);
	void FinishObj(LONG_PTR objnum);
	void DelObj(LONG_PTR objnum);
	void DelAllObjs();

	void SetObjColor(LONG_PTR objnum, COLORREF color);
	void SetObjVisible(LONG_PTR objnum, BOOL bVisble);
	void SetObjMark1(LONG_PTR objnum, BOOL bMark);
	void SetObjMark2(LONG_PTR objnum, BOOL bMark);

	void SetAllObjsMark1(BOOL bMark);
	void SetAllObjsMark2(BOOL bMark);

	void SetDrawType(BOOL bDrawVect, BOOL bDrawMark1,BOOL bDrawMark2){
		m_bDrawVect = bDrawVect, m_bDrawMark1 = bDrawMark1,m_bDrawMark2 = bDrawMark2;
	}

	void SetMark1Width(float wid){
		m_fMarkWidth1 = wid;
	}
	void SetMark2Width(float wid){
		m_fMarkWidth2 = wid;
	}
	void SetMark1Color(COLORREF clr){
		m_clrMark1 = clr;
	}
	void SetMark2Color(COLORREF clr){
		m_clrMark2 = clr;
	}
	float GetMark1Width(){
		return m_fMarkWidth1;
	}
	float GetMark2Width(){
		return m_fMarkWidth2;
	}
	COLORREF GetMark1Color(){
		return m_clrMark1;
	}
	COLORREF GetMark2Color(){
		return m_clrMark2;
	}
	COLORREF GetFtrPtColor(){
		return m_clrFtrPtColor;
	}
	void SetFtrPtColor(COLORREF clr){
		m_clrFtrPtColor = clr;
	}

	//��ʾ˳��Ĺ���
	void MoveToAfter(LONG_PTR objMove, LONG_PTR objAfter);
	void MoveToBefore(LONG_PTR objMove, LONG_PTR objBefore);
	void MoveToHead(LONG_PTR objMove);
	void MoveToTail(LONG_PTR objMove);
	void ReverseDisplayorder();

	virtual Envelope GetDataBound();
	void SetDataBound(Envelope e);
	void SetDataMatrix(double m[16]);

	virtual BOOL InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK=0xffffffff);

	// ��ÿ��ŵĺ����ȡֵ��Χ
	virtual void GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax);
	
	// ��ý��㴦���ڵĿ���
	virtual void GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum);
	
	// ��ÿ���ĸ��ǵ������ڿͻ�����ϵ�µ�XY����
	virtual void GetBlockClientXY(CacheID id, double x[4], double y[4]);

	virtual void OnChangeCoordSys(BOOL bJustScrollScreen);

	inline GrElementList * GetGrElementList()
	{
		return &m_eleList;
	}
public:
	//if objnum found, return TRUE, and insert_idx is its index;
	//if not found, return FALSE, and insert_idx is its suit pos inserted at, 
	//that's next pos to the pos this new obj wound be inserted at.
	BOOL FindObj(LONG_PTR objnum, int& insert_idx);

	virtual void *CreateObject();
	virtual void ReleaseObject(void *pBuf);
	
	// ���������
	virtual BOOL FillBlock(int blkIdx, CacheID id);

	// ��������ת�����ͻ������������(xl,xh,yl,yh)�ཻ��ʸ��
	virtual void DrawClient(double xl, double xh, double yl, double yh);
	void DrawClientWithCache(double xl, double xh, double yl, double yh, BOOL bUseCache);

	void CalcClientBound();
	
protected:
	int m_nType;
	CFArray<GrElement*> m_arrPEles;

	CGraph2dSearch m_search;

	GrElementList m_eleList;

	Envelope m_DataBound;
	Envelope m_DataClientBound;
	BOOL	 m_bCalcClientBound;

	//�ǻ���ʸ�����Լ��Ƿ���Ʊ�ǿ�
	BOOL	 m_bDrawVect, m_bDrawMark1,m_bDrawMark2;

	//��ǿ�Ⱥ���ɫ
	float m_fMarkWidth1, m_fMarkWidth2;
	COLORREF m_clrMark1, m_clrMark2; 

	COLORREF m_clrFtrPtColor;

	//ͼ��ӳ��ľ��������û���ͼ���е�ʸ���ٴα任��ȱʡΪ��λ����
	double   m_DataMatrix[16], m_DataRMatrix[16];
};


MyNameSpaceEnd

#endif // !defined(AFX_DRAWINGVECTOR2D_H__CCE8F7C6_218A_4AE4_B9A4_D57ABF84172A__INCLUDED_)
