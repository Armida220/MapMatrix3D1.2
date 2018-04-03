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

//坐标系类型定义

#define COORDSYS_NONE			0  //无效类型
#define COORDSYS_33MATRIX		1  //和大地坐标系之间是矩阵变换关系
#define COORDSYS_44MATRIX		2  //和大地坐标系之间是矩阵变换关系
#define COORDSYS_WND			3  //和窗口绑定的坐标系
#define COORDSYS_CALLBACK		4  //由CObject回调函数完成的坐标系
#define COORDSYS_COMPOUND		5  //由多个转换器复合的坐标系
#define COORDSYS_CUSTOM			6  //自定义变换

#define MSGID_GNDTOCLT			(WM_USER+400)
#define MSGID_CLTTOGND			(WM_USER+401)

MyNameSpaceBegin

typedef BOOL (CObject::*COORDSYS_CALLBACK_PFUN)(PT_4D *pts, int num);

//客户坐标系：是一种广义的概念，不完全指窗口的坐标系
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
	BOOL Create33Matrix(const double *m); //构造3*3矩阵变换坐标系
	BOOL Create44Matrix(const double *m); //构造4*4矩阵变换坐标系
	BOOL CreateWnd(HWND hWnd);	//构造窗口绑定的客户坐标系
	BOOL CreateCallback(COORDSYS_CALLBACK_PFUN pFunGC, COORDSYS_CALLBACK_PFUN pFunCG);
	BOOL CreateCompound(CArray<CCoordSys*,CCoordSys*>& arrItems);

	int  GetCoordSysType(){
		return m_nType;
	}

	//当前坐标系<->大地坐标系的转换
	BOOL ClientToGround(PT_4D *pts, int num);
	BOOL GroundToClient(PT_4D *pts, int num);
	BOOL GroundToClient(const PT_3D *pt1, PT_3D *pt2);
	BOOL ClientToGround(const PT_3D *pt1, PT_3D *pt2);
	
	//由大地坐标点和大地坐标半径 创建一个大地坐标的矩形范围
	Envelope GrdEvlpFromGrdPtAndCltRadius(PT_4D grdpt, double r);

	//由客户坐标点和客户坐标半径 创建一个大地坐标的矩形范围
	Envelope GrdEvlpFromCltPtAndCltRadius(PT_4D cltpt, double r);

	//由客户坐标矩形 创建一个大地坐标的矩形范围
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
	//等效缩放倍率，理论上其实可以通过 ClientToGround 来计算的
	double m_lfGScale;

protected:
	//当前坐标系<->大地坐标系的转换的虚接口，用作扩展
	virtual BOOL ClientToGround0(PT_4D *pts, int num);
	virtual BOOL GroundToClient0(PT_4D *pts, int num);
	
protected:
	int m_nType;
	HWND m_hCoordWnd;
	double m_matrix[16], m_rmatrix[16];
	CArray<CCoordSys*,CCoordSys*> m_arrPCS;

	BOOL m_bClientLeft;

	//大地到客户的转换函数，以及客户到大地的转换函数
	COORDSYS_CALLBACK_PFUN m_pFunGC, m_pFunCG;
};

MyNameSpaceEnd

#endif // !defined(AFX_COORDSYS_H__9DE2AC7C_4E94_42D2_8713_05114D72A564__INCLUDED_)
