// AccuBox.h: interface for the CAccuBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCUBOX_H__6EE8E05E_C369_4704_973D_73D75475B733__INCLUDED_)
#define AFX_ACCUBOX_H__6EE8E05E_C369_4704_973D_73D75475B733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SmartViewBaseType.h"
#include "CoordWnd.h "
#include "GrBuffer2d.h"


struct ACCU_DATA
{
	ACCU_DATA()
	{
		dis = ang = 0;
		bLockDis = bLockAng = 0;
		pt.x = pt.y = pt.z = 0;
		bLockX = bLockY = bLockZ = 0;
		
		bRelativeCoord = 0;
		dir = 0;
		status = 0;
	}
	
	PT_3D pt;
	BOOL bLockX,bLockY,bLockZ;	
	double dis,ang;
	BOOL bLockDis,bLockAng;	

	BOOL bRelativeCoord;	
	int  dir;
	int  status;
};


struct ACCU_DIR
{
	enum
	{
		invalid=0,
		extension=1,
		parallel=2,
		perpendicular=3
	};
	int type;
	PT_3D ptLine0, ptLine1;
};


class CAccuBox: public CObject  
{
public:
	enum
	{
		sysBase,
		sysRelative
	};
	enum
	{
		rightangleNone = 0,
			rightangleIM = 1 //立即模式
	};
	CAccuBox();
	virtual ~CAccuBox();
	
	inline	int	 GetClicks(){ return m_nClick; }
	
	void    Open();
	void    Close();
	void	Reset();
	void	Activate(BOOL bAct);
	BOOL	IsActive();
	BOOL	IsOpen();
	BOOL    IsNeedRedraw(){ return m_bNeedRedraw;}
	void	LockX(BOOL bLock);
	void	LockY(BOOL bLock);
	void	LockAngle(BOOL bLock);
	void	LockDistance(BOOL bLock);
	void	LockIndex(BOOL bLock);
	void	SetCoordSysBase(int type);
	void    SetReDrawFlag(BOOL flag);
	void    SetRightAngleMode(int mode);

	BOOL	IsLockX();
	BOOL	IsLockY();
	BOOL	IsLockAngle();
	BOOL	IsLockDistance();
	BOOL	IsLockIndex();
	BOOL	IsViewDraw();
	int		GetCoordSysBase();
	
	void	LockDir(int dir, BOOL bUpdateDialog=FALSE);
	void	EnableMultiDir(BOOL bEnable);
	BOOL	IsWaitDir();
	void	MoveDir(PT_3D pt0, PT_3D pt1);
	BOOL	ClickDir(PT_3D pt);
	void	ClearDir();
	const ACCU_DIR* GetLockedDirs(int& num);
	void	UnlockDir(int idx);
	void	SetDir(int type, PT_3D pt0, PT_3D pt1);
	
	void	GetDataPt(PT_3D &ptdata,PT_3D& ptmove);
	void	GetDisAng(double& dis,double& ang);
	void	Click(PT_3D pt);
	void	MoveDataPt(CCoordWnd wnd, PT_3D &pt);
	void	DrawBox(CCoordWnd wnd,GrBuffer2d *pBufL,GrBuffer2d *pBufR);
	void	DrawDrg(CCoordWnd wnd,GrBuffer2d *pBufL,GrBuffer2d *pBufR);
	
	LONG	OnUpdateData(WPARAM wParam, LPARAM lParam);
	inline int GetStatus(){ return m_nStatus; }
protected:
	void	GlobalToLocal(PT_3D *pt);
	void	LocalToGlobal(PT_3D *pt);
	void	UpdateDisAng();
	void	UpdateDialog();
	
private:
	BOOL	ThinkDir(PT_3D &pt);
	void	DrawDir(CCoordWnd wnd,GrBuffer2d *pBufL,GrBuffer2d *pBufR);
	float	GetGrdToCltScale(CCoordWnd& wnd);

public:
	int  	m_nRightAnglePlace; //直角化采集模式
private:
	int		m_nCoordBase;
	double	m_lfBaseAngle;	
	double  m_lfBaseAngle_reserve;		//备用的（只有在切换坐标系时才用得到）
	PT_3D	m_ptOrigin;

	PT_3D	m_ptData,m_ptDataM;
	PT_4D   m_ptDraw,m_ptDrawM;
	
	int		m_nClick, m_nStatus, m_nTolerance;
	int		m_nBoxWid,m_nStickLen,m_nRectWid,m_nDrgMWid;
	BOOL	m_bDrgValid,m_bBoxValid,m_bOpen,m_bActivate;
	
	BOOL    m_bNeedRedraw;
	BOOL	m_bLockX, m_bLockY;
	BOOL	m_bLockAngle, m_bLockDistance;
	double  m_lfAngle,m_lfDistance;

	BOOL    m_bViewDraw;

	BOOL	m_bLockIndex;	
	BOOL	m_bCanSurePoint;
	ACCU_DIR m_curWaitDir;
	CArray<ACCU_DIR,ACCU_DIR> m_dirs;

	CArray<PT_3D,PT_3D> m_arrDrawDirPts;
};

// #include "SmartViewBaseType.h"
// #include "CoordWnd.h"
// 
// class GrBuffer;
// 
// struct ACCU_DATA
// {
// 	ACCU_DATA()
// 	{
// 		type = 0;
// 		u.Polar.dis = u.Polar.ang = 0;
// 		u.Polar.bLockDis = u.Polar.bLockAng = 0;
// 		u.Cartesian.pt.x = u.Cartesian.pt.y = u.Cartesian.pt.z = 0;
// 		u.Cartesian.bLockX = u.Cartesian.bLockY = u.Cartesian.bLockZ = 0;
// 
// 		bUnlockFirstLine = FALSE;
// 		bViewDraw = TRUE;
// 		bAutoReset = TRUE;
// 		dir = 0;
// 		status = 0;
// 		tolerance = 10;
// 	}
// 
// 	int type;
// 	union
// 	{
// 		struct
// 		{
// 			PT_3D pt;
// 			BOOL bLockX,bLockY,bLockZ;
// 		}Cartesian;
// 
// 		struct
// 		{
// 			double dis,ang;
// 			BOOL bLockDis,bLockAng;
// 		}Polar;
// 	}u;
// 
// 	BOOL bUnlockFirstLine;
// 	BOOL bViewDraw;
// 	BOOL bAutoReset;
// 	int  dir;
// 	int  status;
// 	int	 tolerance;
// };
// 
// 
// struct ACCU_DIR
// {
// 	enum
// 	{
// 		invalid=0,
// 		extension=1,
// 		parallel=2,
// 		perpendicular=3
// 	};
// 	int type;
// 	PT_3D ptLine0, ptLine1;
// };
// 
// class CAccuBox : public CObject
// {
// public:
// 	enum
// 	{
// 		sysBase,
// 		sysCustom
// 	};
// 	enum
// 	{
// 		sysCartesian,
// 		sysPolar
// 	};
// 	CAccuBox();
// 	virtual ~CAccuBox();
// 
// 	inline  void Enable3DMode(bool bEnable){ m_b3DMode=bEnable; }
// 	inline  BOOL Is3DMode(){ return m_b3DMode; }
// 	inline	int	 GetClicks(){ return m_nClick; }
// 
// 	void    Open();
// 	void    Close();
// 	void	Reset();
// 	void	Activate(BOOL bAct);
// 	BOOL	IsActive();
// 	BOOL	IsOpen();
// 
// 	void	LockFirstPt(BOOL bLock);
// 	void	LockX(BOOL bLock);
// 	void	LockY(BOOL bLock);
// 	void	LockZ(BOOL bLock);
// 	void	LockAngle(BOOL bLock);
// 	void	LockDistance(BOOL bLock);
// 	void	LockIndex(BOOL bLock);
// 	void	SetCoordSysBase(int type);
// 
// 	BOOL	IsUnlockFirstLine();
// 	BOOL	IsLockX();
// 	BOOL	IsLockY();
// 	BOOL	IsLockZ();
// 	BOOL	IsLockAngle();
// 	BOOL	IsLockDistance();
// 	BOOL	IsLockIndex();
// 	BOOL	IsViewDraw();
// 	int		GetCoordSysBase();
// 
// 	void	LockDir(int dir, BOOL bUpdateDialog=FALSE);
// 	void	EnableMultiDir(BOOL bEnable);
// 	BOOL	IsWaitDir();
// 	void	MoveDir(PT_3D pt0, PT_3D pt1);
// 	BOOL	ClickDir(PT_3D pt);
// 	void	ClearDir();
// 	const ACCU_DIR* GetLockedDirs(int& num);
// 	void	UnlockDir(int idx);
// 
// 	void	GetDataPt(PT_3D &ptdata,PT_3D& ptmove);
// 	void	GetDisAng(double& dis,double& ang);
// 	void	Click(PT_3D pt);
// 	void	MoveDataPt(CCoordWnd man, PT_3D &pt);
// 	void	DrawBox(CCoordWnd man,GrBuffer *pBuf);
// 	void	DrawDrg(CCoordWnd man,GrBuffer *pBuf);
// 
// 	LONG	OnUpdateData(WPARAM wParam, LPARAM lParam);
// 	inline int GetStatus(){ return m_nStatus; }
// protected:
// 	void	GlobalToLocal(PT_3D *pt);
// 	void	LocalToGlobal(PT_3D *pt);
// 	void	UpdateDisAng();
// 	void	UpdateDialog();
// 	void    UpdateSettings(BOOL bSave);
// 
// private:
// 	BOOL	ThinkDir(PT_3D &pt);
// 	void	DrawDir(CCoordWnd man,GrBuffer *pBuf);
// 	float	GetGrdToCltScale(CCoordWnd& man);
// 	
// private:
// 	double	m_lfBaseAngle, m_lfCustomAng; 
// 
// 	PT_3D	m_ptOrigin;
// 	PT_3D	m_ptData,m_ptDataM;
// 	PT_4D   m_ptDraw,m_ptDrawM;
// 
// 	int		m_nClick, m_nStatus, m_nTolerance;
// 	int		m_nBoxWid,m_nStickLen,m_nRectWid,m_nDrgMWid;
// 	BOOL	m_bDrgValid,m_bBoxValid,m_bOpen,m_bActivate,m_b3DMode;
// 	
// 	BOOL	m_bLockX, m_bLockY, m_bLockZ;
// 	BOOL	m_bLockAngle, m_bLockDistance;
// 	BOOL	m_bLockIndex;
// 	int		m_nCoordBase, m_nCoordType;
// 
// 	BOOL    m_bUnlockFirstLine, m_bViewDraw, m_bAutoReset;
// 
// 	double  m_lfAngle,m_lfDistance;
// 
// 	BOOL	m_bCanSurePoint;
// 	ACCU_DIR m_curWaitDir;
// 	CArray<ACCU_DIR,ACCU_DIR> m_dirs;
// 	CArray<PT_3D,PT_3D> m_arrDrawDirPts;
// };
#endif // !defined(AFX_ACCUBOX_H__6EE8E05E_C369_4704_973D_73D75475B733__INCLUDED_)
