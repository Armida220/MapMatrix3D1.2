// Snap.h: interface for the CSnap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNAP_H__6B9DA9F8_43EB_449C_A451_0798DA5674BB__INCLUDED_)
#define AFX_SNAP_H__6B9DA9F8_43EB_449C_A451_0798DA5674BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewBaseType.h"
#include "Feature.h "

MyNameSpaceBegin


class CDataQueryEx;

struct EXPORT_EDITBASE SNAPITEM
{
	SNAPITEM()
	{
		pFtr =NULL;
		bInPreObj = 0;
		nSnapMode = 0;
	}
	BOOL IsValid();
	CFeature * pFtr;
	int bInPreObj;
	int nSnapMode;
	PT_3D pt;
};

struct EXPORT_EDITBASE SNAPSETTINGS
{
	SNAPSETTINGS::SNAPSETTINGS()
	{
		IS2D = true;
		bOpen = TRUE;
		lfRadius = 10;
		nMode = 1;
		bSnapSelf = TRUE;
		bOnlyBase = FALSE;
	}
	long nMode;
	double lfRadius;
	BOOL bOpen;
	BOOL IS2D;
	BOOL bSnapSelf;
	BOOL bOnlyBase;
};

struct EXPORT_EDITBASE SNAP_GRID
{
	SNAP_GRID()
	{
		ox = oy = dx = dy = 0;
		nx = ny = 0;
	}
	double ox,oy;
	double dx,dy;
	int	   nx,ny;
};


class EXPORT_EDITBASE CSnap  
{
public:
	enum
	{
		modeNearPoint=0x0001,
		modeKeyPoint=0x0002,
		modeMidPoint=0x0004,
		modeIntersect=0x0008,
		modePerpPoint=0x0010,
		modeCenterPoint=0x0020,
		modeTangPoint=0x0040,
		modeEndPoint=0x0080,
		modePolar=0x0100,
		modeGrid=0x10000000
	};
	
	CSnap();
	virtual ~CSnap();
	void Init(CDataQueryEx *pDQ);
	long GetSnapMode();
	void SetSnapMode(long nMode);
	double GetSnapRadius();
	void SetSnapRadius(double r);
	inline void Enable (BOOL bEnable){	m_bOpen = bEnable; }
	inline BOOL bOpen(){ return m_bOpen; }
	inline BOOL Is2D(){	return m_b2D; }
	inline void Enable2D( BOOL bEnable ){	m_b2D = bEnable; }
	inline void EnableSnapSelf(BOOL bEnable){ m_bSnapSelf = bEnable; }
	inline void EnableOnlyBase(BOOL bEnable){ m_bOnlyBase = bEnable; }
	inline BOOL bOnlyBase(){ return m_bOnlyBase; }
	inline BOOL bSnapSelf(){ return m_bSnapSelf; }
	BOOL PreSnap(Envelope e, PT_3D *lastPt, int lastPtNum, PT_3D *curPt,CCoordSys *pCS, CGeometry *pObj, BOOL bReset=TRUE);
	BOOL DoSnap(Envelope e, PT_3D *lastPt, int lastPtNum, PT_3D *curPt,CCoordSys *pCS);
	int GetSnapResult(SNAPITEM *pItems);
	SNAPITEM GetFirstSnapResult();
	SNAP_GRID GetGridParams();
	void SetGridParams(SNAP_GRID *param);
	void UpdateSettings(BOOL bSave);
	
private:
	BOOL DoSnapByObj(Envelope e, PT_3D *lastPt, int lastPtNum, PT_3D *curPt, CCoordSys *pCS,CGeometry *pObj,double *ret_mindis=NULL);
	BOOL DoSnapByNearPoint(Envelope e, PT_3D *pt,CCoordSys *pCS,CGeometry* pObj=NULL,double *ret_dis=NULL);
	BOOL DoSnapByEndPoint(Envelope e, PT_3D *pt,CCoordSys *pCS,CGeometry* pObj=NULL,double *ret_dis=NULL);
	BOOL DoSnapByKeyPoint(Envelope e, PT_3D *pt,CCoordSys *pCS,CGeometry* pObj=NULL,double *ret_dis=NULL);
	BOOL DoSnapByMidPoint(Envelope e, PT_3D *pt,CCoordSys *pCS,CGeometry* pObj=NULL,double *ret_dis=NULL);
	BOOL DoSnapByCenterPoint(Envelope e, PT_3D *pt,CCoordSys *pCS,CGeometry* pObj=NULL,double *ret_dis=NULL);
	BOOL DoSnapByIntersect(Envelope e, PT_3D *pt,CCoordSys *pCS,CGeometry* pObj0=NULL,CGeometry* pObj1=NULL,double *ret_dis=NULL);
	BOOL DoSnapByPerpPoint(Envelope e, PT_3D *lastPt,PT_3D *curPt,CCoordSys *pCS,CGeometry* pObj=NULL,double *ret_dis=NULL);
	BOOL DoSnapByTangPoint(Envelope e, PT_3D *lastPt,PT_3D *curPt,CCoordSys *pCS,CGeometry* pObj=NULL,double *ret_dis=NULL);
	BOOL DoSnapByGrid(Envelope e, PT_3D *pt,CCoordSys *pCS, double *ret_dis=NULL);
	// ¼«Öá
	BOOL DoSnapByPolar(Envelope e, PT_3D *lastPt,PT_3D *curPt,CCoordSys *pCS,double *ret_dis=NULL);
	
	
private:
	
	CArray<SNAPITEM,SNAPITEM> m_arrSnapItems;
	
	long m_nMode;
	BOOL m_bOpen;
	BOOL m_b2D;
	BOOL m_bSnapSelf;
	BOOL m_bOnlyBase;
	double m_lfRadius;
	
	double m_lfNearestPreSnap;
	SNAPITEM m_preItem;
	SNAP_GRID m_gridParm;

	CDataQueryEx *m_pDataQuery;
public:	
	BOOL m_bWithSymbol;

};

MyNameSpaceEnd

#endif // !defined(AFX_SNAP_H__6B9DA9F8_43EB_449C_A451_0798DA5674BB__INCLUDED_)
