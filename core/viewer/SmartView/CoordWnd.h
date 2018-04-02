// CoordWnd.h: interface for the CCoordWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COORDWND_H__AF958829_49BF_4920_A964_E9BBD2CD01AA__INCLUDED_)
#define AFX_COORDWND_H__AF958829_49BF_4920_A964_E9BBD2CD01AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "matrix.h"
#include "CoordSys.h"


MyNameSpaceBegin


//��ͼ���ڵ�������Ϣ��
class EXPORT_SMARTVIEW CCoordWnd
{
public:
	CCoordWnd(){
		m_pSearchCS = NULL;
		m_pViewCS = NULL;
		m_ptGrd = PT_3D(0,0,0);
		m_ptCursor = PT_3D(0,0,0);
		m_bIsStereo = FALSE;

		m_bScaleViewValid = FALSE;
		m_bScaleSearchValid = FALSE;

		m_lfScaleSearch = 1.0;
		m_lfScaleView = 1.0;
	}
	CCoordWnd(CCoordSys *p1, CCoordSys *p2, PT_3D pt, PT_3D pt1, BOOL b){
		m_pSearchCS = p1;
		m_pViewCS = p2;
		m_ptGrd = pt;
		m_bIsStereo = b;
		m_ptCursor = pt1;

		m_bScaleViewValid = FALSE;
		m_bScaleSearchValid = FALSE;
		
		m_lfScaleSearch = 1.0;
		m_lfScaleView = 1.0;
	}
	double GetScaleOfSearchCSToViewCS();
	/*
	   Ϊ�˻�ø����ͼ�������ٶȣ���ͼ����ʾ����ϵ����������ϵ��һ��һ�£�	   
	*/

	//��ͼ���ڵ���������ϵ
	CCoordSys * m_pSearchCS;

	//��ͼ���ڵ���ʾ����ϵ
	CCoordSys * m_pViewCS;

	//��ǰ���Ĵ�����꣨��׽��ģ�
	PT_3D m_ptGrd;

	//��ǰ���Ĵ�����꣨��׽ǰ�ģ�
	PT_3D m_ptCursor;

	//�Ƿ�����ά����
	BOOL m_bIsStereo;

public:
	void CalcScaleView();
	void CalcScaleSearch();
	BOOL m_bScaleSearchValid;
	BOOL m_bScaleViewValid;
	double m_lfScaleSearch;
	double m_lfScaleView;
};

MyNameSpaceEnd

#endif // !defined(AFX_COORDWND_H__AF958829_49BF_4920_A964_E9BBD2CD01AA__INCLUDED_)
