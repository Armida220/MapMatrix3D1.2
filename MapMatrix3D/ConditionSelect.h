// ConditionSelect.h : APIs use for condition select
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONDITIONSELECT_H__35D1FE7C_0B36_4003_B651_C81E994E65E8__INCLUDED_)
#define AFX_CONDITIONSELECT_H__35D1FE7C_0B36_4003_B651_C81E994E65E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "DlgConditionSelect.h"
//#include "GeoBaseType.h"
#include "DlgDataSource.H"
//#include "Search.h"

class CDlgDoc;

#define OP_EQUAL				0
#define OP_UNEQUAL				1
#define OP_GREATER				2
#define OP_LESS					3

// 0, ��Ч���; 1, С��; 2, ����; 4, ����;
#define COMP_ERROR					0
#define COMP_LESS					1
#define COMP_EQUAL					2
#define COMP_GREATER				4

#define COMP_CURFALSE				0	
#define COMP_CURTRUE				1
			

//�������������������������������ȽϽ������ϸ����
//������Ϣ�ṹ
struct CP_ATTR
{
	int index; //�������ڵ����
	_variant_t value; //����ֵ
};

//һ���ڵ����Ϣ
struct CP_POINT
{
	int ptidx; //�ڵ��
	CArray<CP_ATTR,CP_ATTR> arrAttrs; //�ڵ��������Ϣ
	CP_POINT& operator=(CP_POINT& info)
	{
		ptidx = info.ptidx;
		arrAttrs.Copy(info.arrAttrs);
		return *this;
	}
};


//һ������Ĳ�ѯ�������
struct CP_RESULT
{
	CFeature *m_pFtr;
	CArray<CP_ATTR,CP_ATTR> arrAttrs; //�������������Ա�
	CPtrArray arrPoints; //���������Ľڵ�,���CP_POINT��ָ��

	CP_RESULT& operator=(CP_RESULT& r)
	{
		m_pFtr = r.m_pFtr;
		arrAttrs.Copy(r.arrAttrs);
		int np = r.arrPoints.GetSize(), i;
		CP_POINT *p, *t;
		for( i=0; i<np; i++)
		{
			p = new CP_POINT;
			t = (CP_POINT*)r.arrPoints.GetAt(i);
			if( p && t )
			{
				*p = *t;
				arrPoints.Add(p);
			}
		}
		return *this;
	}

	void Clear()
	{
		int np = arrPoints.GetSize();
		for( int j=0; j<np; j++)
		{
			CP_POINT *p = (CP_POINT*)arrPoints.GetAt(j);
			if( p )delete p;
		}
		arrAttrs.RemoveAll();
		arrPoints.RemoveAll();
	}
};

class CCPResultList
{
public:
	CCPResultList();
	CCPResultList(CStringArray& nameList);
	~CCPResultList();
	void Copy(CCPResultList& list);
	void CombineList(CCPResultList& list);
	void SubtractList(CCPResultList& list);
	void Clear();

	void StartRecord();
	void AbortRecord();
	void FinishRecord();

	void SetCurFtr(CFeature *pFt);
	void SetCurAttr(int idx);
	void AddAttrResult(_variant_t value);
	void AddPointResult(int ptidx, _variant_t value);
	CFeature* GetCurFtr(){ return curResult.m_pFtr; }

	CStringArray attrNameList;
	CPtrArray resultList;
private:
	void Combine();
	void AddAttrResult(CP_ATTR *attr);
	void AddPointResult(CP_POINT* pt);

	CP_RESULT curResult;
	int nCurAttr;//, nCombineType;
};

typedef BOOL (*PFUNCOMP)(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�Ƚϵ�����������
BOOL CompField(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r); 

//�ȽϽڵ� x ����
BOOL CompX(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�ȽϽڵ� y ����
BOOL CompY(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�ȽϽڵ� z ����
BOOL CompZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�ȽϽڵ� z �Ƿ�Ϊ�̲߳����������
BOOL CompIZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�Ƚ����ڽڵ��߲�
BOOL CompDZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�Ƚ����ڽڵ���
BOOL CompPerDis(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�Ƚ����߼��
BOOL CompPerAngle(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�Ƚ���β�ڵ����
BOOL CompEndDis(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�ȽϽڵ�����
BOOL CompLineType(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//˳/��ʱ�뷽��
BOOL CompClockwise(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//����
BOOL CompLen(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//���
BOOL CompArea(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//���ཻ
BOOL CompIntersectSelf(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//���ҵ�
BOOL CompSuspend(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//ˮ������ by shy
BOOL StreamReverse(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//������
BOOL CompSamePoint(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//������
BOOL CompSameLine(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//���ཻ
BOOL CompIntersectLine(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//�����ȽϽṹ
struct CONDCOMP 
{
	UINT strid;
	int	 id;
	PFUNCOMP pfun;
};

typedef BOOL (*PFUN_CHECKOBJ)(CFeature *pFtr, CFtrLayer *pLayer);
class CSearchMultiObj
{
public:
	CSearchMultiObj();
	virtual ~CSearchMultiObj();

	void Init(CDlgDataSource *pDataSouce,CCoordSys *pCoordSys);

	//����ָ�ĸ�����ԭ�㡢��ȡ���Ŀ��
	void Prepare(int intensity=1);

	//����ָ�ĸ�����ԭ�㡢��ȡ���Ŀ��
	void PrepareWithRadius(double r);

	//�����ʱ����
	void Clear();

	//�������� pt ����������ϵĵ���������� pFtr0
	int FindObjectInRect(CFeature *pFtr0, PT_3D *pt, Envelope evlp, BOOL bForPoint=TRUE);

	//�����������߶�pt0,pt1�ཻ�ĵ���������� pFtr0
	int FindIntersectInRect(CFeature *pFtr0, PT_3D pt0, PT_3D pt1);

	int GetFoundObjectHandles(FTR_HANDLE *pFtrHandle);

	inline void GetGridSize(double& dx, double& dy){
		dx = m_lfDX; dy = m_lfDY;
	}
	inline void SetCheckFunc(PFUN_CHECKOBJ pfun){
		m_pFuncCheckObj = pfun;
	}
	inline void SetDataSource(CDlgDataSource *pDataSource){
		m_pDataSource = pDataSource;
	}
	inline CDlgDataSource* GetDataSource(){
		return m_pDataSource;
	}
	inline PFUN_CHECKOBJ GetCheckFunc()const{
		return m_pFuncCheckObj;
	}
	inline CCoordSys* GetCoorSys(){
		return m_pCoorSys;
	}

private:
	//���ո��������еĵ�����ൽ������
	void CalcGridForPoint();
	void CalcGridForLine();
	double m_lfOX, m_lfOY;
	double m_lfDX, m_lfDY;
	int m_nx, m_ny;
	CPtrArray m_arrObjs;
	BOOL m_bCalcGrid;

	CDlgDataSource *m_pDataSource;

	CArray<FTR_HANDLE,FTR_HANDLE> m_foundHandles;

	CCoordSys *m_pCoorSys;

	PFUN_CHECKOBJ m_pFuncCheckObj;
};


class CLinesSearch
{
public:
	CLinesSearch();
	~CLinesSearch();
	void Init(const PT_3D *pts, int num);
	int *FindNearLines(PT_3D pt0, PT_3D pt1, int &num);
private:
	double m_lfOX, m_lfOY;
	double m_lfDX, m_lfDY;
	int m_nx, m_ny;
	CArray<int,int> *m_pArrIndex;
	CArray<int,int> m_arrFound;
};


extern CSearchMultiObj gSSP_forPoint, gSSP_forLine;

#endif // !defined(AFX_CONDITIONSELECT_H__35D1FE7C_0B36_4003_B651_C81E994E65E8__INCLUDED_)