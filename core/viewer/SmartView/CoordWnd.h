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


//视图窗口的坐标信息类
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
	   为了获得更快的图形搜索速度，视图的显示坐标系和搜索坐标系不一定一致；	   
	*/

	//视图窗口的搜索坐标系
	CCoordSys * m_pSearchCS;

	//视图窗口的显示坐标系
	CCoordSys * m_pViewCS;

	//当前测标的大地坐标（捕捉后的）
	PT_3D m_ptGrd;

	//当前测标的大地坐标（捕捉前的）
	PT_3D m_ptCursor;

	//是否是三维窗口
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
