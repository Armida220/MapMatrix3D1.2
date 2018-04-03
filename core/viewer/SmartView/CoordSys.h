// CoordSys.h: interface for the CCoordSys class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COORDSYS_H__9DE2AC7C_4E94_42D2_8713_05114D72A564__INCLUDED_)
#define AFX_COORDSYS_H__9DE2AC7C_4E94_42D2_8713_05114D72A564__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Envelope.h"
#include <math.h>

//����ϵ���Ͷ���

#define COORDSYS_NONE			0  //��Ч����
#define COORDSYS_33MATRIX		1  //�ʹ������ϵ֮���Ǿ���任��ϵ
#define COORDSYS_44MATRIX		2  //�ʹ������ϵ֮���Ǿ���任��ϵ
#define COORDSYS_WND			3  //�ʹ��ڰ󶨵�����ϵ
#define COORDSYS_CALLBACK		4  //��CObject�ص�������ɵ�����ϵ
#define COORDSYS_COMPOUND		5  //�ɶ��ת�������ϵ�����ϵ
#define COORDSYS_CUSTOM			6  //�Զ���任

#define MSGID_GNDTOCLT			(WM_USER+400)
#define MSGID_CLTTOGND			(WM_USER+401)

MyNameSpaceBegin

typedef BOOL (CObject::*COORDSYS_CALLBACK_PFUN)(PT_4D *pts, int num);

//�ͻ�����ϵ����һ�ֹ���ĸ������ȫָ���ڵ�����ϵ
class EXPORT_SMARTVIEW CCoordSys  
{
public:
	CCoordSys();
	virtual ~CCoordSys();

	static CCoordSys *CreateObject();
	static void DeleteObject(CCoordSys* pObj);

	virtual void CopyFrom(CCoordSys *pCS);

	BOOL IsNoRotated(){
		return fabs(m_matrix[8])<1e-10&&fabs(m_matrix[4])<1e-10&&fabs(m_matrix[9])<1e-10;
	}
	BOOL Create33Matrix(const double *m); //����3*3����任����ϵ
	BOOL Create44Matrix(const double *m); //����4*4����任����ϵ
	BOOL CreateWnd(HWND hWnd);	//���촰�ڰ󶨵Ŀͻ�����ϵ
	BOOL CreateCallback(COORDSYS_CALLBACK_PFUN pFunGC, COORDSYS_CALLBACK_PFUN pFunCG);
	BOOL CreateCompound(CArray<CCoordSys*,CCoordSys*>& arrItems);

	int  GetCoordSysType(){
		return m_nType;
	}

	//��ǰ����ϵ<->�������ϵ��ת��
	BOOL ClientToGround(PT_4D *pts, int num);
	BOOL GroundToClient(PT_4D *pts, int num);
	BOOL GroundToClient(const PT_3D *pt1, PT_3D *pt2);
	BOOL ClientToGround(const PT_3D *pt1, PT_3D *pt2);
	
	//�ɴ�������ʹ������뾶 ����һ���������ľ��η�Χ
	Envelope GrdEvlpFromGrdPtAndCltRadius(PT_4D grdpt, double r);

	//�ɿͻ������Ϳͻ�����뾶 ����һ���������ľ��η�Χ
	Envelope GrdEvlpFromCltPtAndCltRadius(PT_4D cltpt, double r);

	//�ɿͻ�������� ����һ���������ľ��η�Χ
	Envelope GrdEvlpFromCltRect(CRect& rect);
	CRect GrdEvlpToCltRect(Envelope e);

	double CalcScale();

	void SetClientAsLeft(BOOL bClientLeft){
		m_bClientLeft = bClientLeft;
	}

	void GetMatrix(double m[16]);
	void GetRMatrix(double m[16]);
	void MultiplyMatrix(double m[16]);
	void MultiplyRightMatrix(double m[16]);

public:
	//��Ч���ű��ʣ���������ʵ����ͨ�� ClientToGround �������
	double m_lfGScale;

protected:
	//��ǰ����ϵ<->�������ϵ��ת������ӿڣ�������չ
	virtual BOOL ClientToGround0(PT_4D *pts, int num);
	virtual BOOL GroundToClient0(PT_4D *pts, int num);
	
protected:
	int m_nType;
	HWND m_hCoordWnd;
	double m_matrix[16], m_rmatrix[16];
	CArray<CCoordSys*,CCoordSys*> m_arrPCS;

	BOOL m_bClientLeft;

	//��ص��ͻ���ת���������Լ��ͻ�����ص�ת������
	COORDSYS_CALLBACK_PFUN m_pFunGC, m_pFunCG;
};

MyNameSpaceEnd

#endif // !defined(AFX_COORDSYS_H__9DE2AC7C_4E94_42D2_8713_05114D72A564__INCLUDED_)
