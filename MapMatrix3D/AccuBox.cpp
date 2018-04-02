// AccuBox.cpp: implementation of the CAccuBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "AccuBox.h"
#include "SmartViewFunctions.h"
#include "exMessage.h"
#include "RegDef.h"
#include "RegDef2.h"

#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern CDocument *GetCurDocument();
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAccuBox::CAccuBox()
{
	m_bNeedRedraw = FALSE;
	m_nBoxWid = 40;
	m_nRectWid = 2;
	m_nStickLen = 14;

	m_bOpen = FALSE;
	m_bActivate = FALSE;
	m_bBoxValid = FALSE;
	m_bDrgValid = FALSE;
	m_nClick = 0;
	m_lfBaseAngle = 0;
	m_lfBaseAngle_reserve = 0;
	m_ptOrigin.x = m_ptOrigin.y = m_ptOrigin.z = 0;
	m_ptDataM.x = m_ptDataM.y = m_ptDataM.z = 0;
	m_ptData.x = m_ptData.y = m_ptData.z = 0;

	m_ptDrawM.x = m_ptDrawM.y = m_ptDrawM.z = 0;
	m_ptDraw.x = m_ptDraw.y = m_ptDraw.z = 0;

	m_nDrgMWid = 0;
	m_nStatus  = 0;

	m_bLockX = m_bLockY = FALSE;
	m_bLockAngle = m_bLockDistance = FALSE;
	m_bLockIndex = TRUE;
	m_nCoordBase = sysRelative;

	m_lfAngle = m_lfDistance = 0.0;
	m_nTolerance = 10;

	m_bViewDraw = TRUE;

	m_curWaitDir.type = ACCU_DIR::invalid;
	m_bCanSurePoint = FALSE;

	m_nRightAnglePlace = rightangleNone;
//	AfxLinkCallback(FCCM_UPDATE_ACCUDATA,this,(PFUNCALLBACK)OnUpdateData);

}
 
CAccuBox::~CAccuBox()
{
//	AfxUnlinkCallback(this,(PFUNCALLBACK)OnUpdateData);
}


BOOL CAccuBox::IsActive()
{
	return m_bActivate;
}

BOOL CAccuBox::IsOpen()
{
	return m_bOpen;
}

void CAccuBox::Activate(BOOL bAct)
{
	m_bActivate = bAct;
}

void CAccuBox::Open()
{
	m_lfBaseAngle = 0;
	m_nDrgMWid = 1;
	m_bOpen = TRUE;
	m_nStatus = 0;

	m_bBoxValid = FALSE;
	m_bDrgValid = FALSE;
	m_nClick = 0;
	m_bViewDraw = TRUE;

	UpdateDialog();
}

void CAccuBox::Close()
{
	m_bOpen = FALSE;

	m_bLockX = m_bLockY = FALSE;
	m_bLockAngle = m_bLockDistance = FALSE;

	m_ptDataM.x = m_ptDataM.y = m_ptDataM.z = 0;
	m_lfAngle = m_lfDistance = 0.0;
}


void CAccuBox::GlobalToLocal(PT_3D *pt)
{
	double ang = m_lfBaseAngle;
	//对0.5PI的整数倍角，在此程序中求正/余弦时会产生误差，故需要纠正一下（后面所有地方都做同样处理）
	double cosA = cos(ang);
	double sinA = sin(ang);
	if( fabs(cosA)<1e-10 ) cosA = 0;
	if( fabs(sinA)<1e-10 ) sinA = 0;

	if( m_nClick>=1 )
	{
		pt->x -= m_ptOrigin.x;  pt->y -= m_ptOrigin.y;	pt->z -= m_ptOrigin.z;

		double x = pt->x*cosA + pt->y*sinA;
		double y = -pt->x*sinA + pt->y*cosA;
		pt->x = x; pt->y = y;
	}
}


void CAccuBox::LocalToGlobal(PT_3D *pt)
{
	double ang = m_lfBaseAngle;
	double cosA = cos(ang);
	double sinA = sin(ang);
	if( fabs(cosA)<1e-10 ) cosA = 0;
	if( fabs(sinA)<1e-10 ) sinA = 0;

	if( m_nClick>=1 )
	{			
		double x = pt->x*cosA - pt->y*sinA;
		double y = pt->x*sinA + pt->y*cosA;
		pt->x = x; pt->y = y;

		pt->x += m_ptOrigin.x;  pt->y += m_ptOrigin.y;	pt->z += m_ptOrigin.z;
	}
		
}

void CAccuBox::LockX(BOOL bLock)
{
	m_bLockX = bLock;
	UpdateDialog();
}


void CAccuBox::LockY(BOOL bLock)
{
	m_bLockY = bLock;
	UpdateDialog();
}


void CAccuBox::LockAngle(BOOL bLock)
{
	m_bLockAngle = bLock;
	UpdateDialog();
}


void CAccuBox::LockDistance(BOOL bLock)
{
	m_bLockDistance = bLock;
	UpdateDialog();
}


void CAccuBox::LockIndex(BOOL bLock)
{
	m_bLockIndex = bLock;
	if( !m_bLockIndex )m_nDrgMWid = 1;
}

void CAccuBox::SetRightAngleMode(int mode)
{
	static int oldCoord = -1;
	BOOL bDlgVisiable = ( (CMainFrame*)AfxGetMainWnd() )->m_wndAccu.IsWindowVisible();

	m_nRightAnglePlace = mode;
	
	if( !bDlgVisiable )
	{
		m_bViewDraw = FALSE;
		if( mode==rightangleIM )
		{
			m_nCoordBase = sysRelative;
			m_bOpen = TRUE;
			m_bActivate = TRUE;
			m_lfAngle = PI*0.5;
			LockAngle(TRUE);
		}
		else
		{
			m_bOpen = FALSE;
			m_bActivate = FALSE;
			
			m_bLockX = m_bLockY = FALSE;
			m_bLockAngle = m_bLockDistance = FALSE;
			
			m_ptDataM.x = m_ptDataM.y = m_ptDataM.z = 0;
			m_lfAngle = m_lfDistance = 0.0;
		}
	}
	else
	{
		if( m_bOpen )
		{
			if( mode==rightangleIM )
			{
				oldCoord = m_nCoordBase;
				m_nCoordBase = sysRelative;
				m_bActivate = TRUE;
				m_lfAngle = PI*0.5;
				LockAngle(TRUE);
			}
			else
			{
				if(oldCoord != -1)
				{
					m_nCoordBase = oldCoord;
					oldCoord = -1;
				}
				m_lfAngle = 0;
				LockAngle(FALSE);
			}
		}
		else
		{
			( (CMainFrame*)AfxGetMainWnd() )->m_wndAccu.ShowWindow(SW_HIDE);
		}
	}
}

void CAccuBox:: SetReDrawFlag(BOOL flag)
{
	m_bNeedRedraw = flag;
}

void CAccuBox::SetCoordSysBase(int base)
{
	m_bNeedRedraw = TRUE;
	if( m_nCoordBase!=base )
	{
		//1.先把方向锁定的坐标全转成全局坐标
		LocalToGlobal(&m_curWaitDir.ptLine0);
		LocalToGlobal(&m_curWaitDir.ptLine1);

		struct ACCU_DIR* pdirs = NULL;
		pdirs = m_dirs.GetData();
		if(pdirs && m_dirs.GetSize()>0)
		{
			for(int i=0; i<m_dirs.GetSize(); i++)
			{
				LocalToGlobal( &(pdirs[i].ptLine0) );
				LocalToGlobal( &(pdirs[i].ptLine1) );
			}
		}

		LocalToGlobal(&m_ptData);
		LocalToGlobal(&m_ptDataM);

		if( base==sysRelative )
		{
			m_nCoordBase = sysRelative;
			m_lfBaseAngle = m_lfBaseAngle_reserve;
		}
		else
		{
			m_nCoordBase = sysBase;
			m_lfBaseAngle = 0;
		}

		GlobalToLocal(&m_ptData);
		GlobalToLocal(&m_ptDataM);

		//2.再把方向锁定的坐标全转成局部坐标
		GlobalToLocal(&m_curWaitDir.ptLine0);
		GlobalToLocal(&m_curWaitDir.ptLine1);

		if(pdirs && m_dirs.GetSize()>0)
		{
			for(int i=0; i<m_dirs.GetSize(); i++)
			{
				GlobalToLocal( &(pdirs[i].ptLine0) );
				GlobalToLocal( &(pdirs[i].ptLine1) );
			}
		}

		//这里应该先绘制框框，再更新数据
		CDlgDoc *pDoc = (CDlgDoc*)GetCurDocument();
		pDoc->UpdateAllViews(NULL,hc_Refresh,0);
		UpdateDisAng();
		UpdateDialog();
	}
}


void CAccuBox::UpdateDisAng()
{
	double dis = sqrt(
		(m_ptData.x-m_ptDataM.x)*(m_ptData.x-m_ptDataM.x) + 
		(m_ptData.y-m_ptDataM.y)*(m_ptData.y-m_ptDataM.y) 
		);
	
	if( !m_bLockDistance )
	{
		m_lfDistance = dis;
	}
	
	if( !m_bLockAngle )
	{
		if(dis>=1e-10 )
			m_lfAngle = GraphAPI::GGetAngle(m_ptData.x,m_ptData.y,m_ptDataM.x,m_ptDataM.y);
		else
			m_lfAngle = 0;
	}
}


void CAccuBox::UpdateDialog()
{
	ACCU_DATA data;

	data.bRelativeCoord = m_nCoordBase==sysBase?FALSE:TRUE;
	data.status = m_nStatus;
	
	data.pt = m_ptDataM;
	data.bLockX = m_bLockX;
	data.bLockY = m_bLockY;
	
	if( m_bLockX || m_bLockY )
		data.status = 1;
	
	data.dis = m_lfDistance;
	data.ang = m_lfAngle;
	data.bLockDis = m_bLockDistance;
	data.bLockAng = m_bLockAngle;
	
	if( m_bLockDistance || m_bLockAngle )
		data.status = 1;
	
	AfxGetMainWnd()->SendMessage(FCCM_UPDATE_ACCUDLG,0,LPARAM(&data));
}


LONG CAccuBox::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	if( wParam==0 )
	{
		if( lParam )
		{
			ACCU_DATA *data = (ACCU_DATA*)lParam;

			m_ptDataM = data->pt    ;
			m_bLockX  = data->bLockX;
			m_bLockY  = data->bLockY;			
			
			m_lfDistance	 = data->dis     ;
			m_lfAngle		 = data->ang     ;
			m_bLockDistance	 = data->bLockDis;
			m_bLockAngle	 = data->bLockAng;

			if( FALSE == data->bRelativeCoord )
				m_nCoordBase = sysBase;
			else
				m_nCoordBase = sysRelative;
		}
	}
	else if( wParam==1 )
	{
		LockDir(lParam);
	}
	else if( wParam==2 )
	{
		ClearDir();
	}
	else if( wParam==3 )
	{
		if( lParam )
		{
			ACCU_DATA *data = (ACCU_DATA*)lParam;			
			
			data->pt     = m_ptDataM ;
			data->bLockX = m_bLockX  ;
			data->bLockY = m_bLockY  ;	
			
			data->dis      = m_lfDistance	 ;
			data->ang      = m_lfAngle		 ;
			data->bLockDis = m_bLockDistance ;
			data->bLockAng = m_bLockAngle	 ;

			if( m_nCoordBase == sysBase)
				data->bRelativeCoord = FALSE;
			else
				data->bRelativeCoord = TRUE;
		}
	}

	return 0;
}


BOOL CAccuBox::IsLockX()
{
	return m_bLockX;
}


BOOL CAccuBox::IsLockY()
{
	return m_bLockY;
}


BOOL CAccuBox::IsLockAngle()
{
	return m_bLockAngle;
}


BOOL CAccuBox::IsLockDistance()
{
	return m_bLockDistance;
}


BOOL CAccuBox::IsLockIndex()
{
	return m_bLockIndex;
}

BOOL CAccuBox::IsViewDraw()
{
	return m_bViewDraw;
}


int	 CAccuBox::GetCoordSysBase()
{
	return m_nCoordBase;
}


void CAccuBox::GetDataPt(PT_3D &ptdata,PT_3D& ptmove)
{
	ptdata = m_ptData; ptmove = m_ptDataM;
	LocalToGlobal(&ptdata);
	LocalToGlobal(&ptmove);
}


void CAccuBox::GetDisAng(double& dis,double& ang)
{
	dis = m_lfDistance; ang = m_lfAngle;
}


void CAccuBox::LockDir(int dir, BOOL bUpdateDialog)
{
	memset(&m_curWaitDir,0,sizeof(m_curWaitDir));
	m_curWaitDir.type = dir;
	m_bCanSurePoint = FALSE;
	if( bUpdateDialog )
	{
		AfxGetMainWnd()->SendMessage(FCCM_UPDATE_ACCUDLG,1,dir);
	}
}

void CAccuBox::SetDir(int type, PT_3D pt0, PT_3D pt1)
{
	m_curWaitDir.type = type;
	m_curWaitDir.ptLine0 = pt0;
	m_curWaitDir.ptLine1 = pt1;
	
	m_dirs.RemoveAll();
	m_dirs.Add(m_curWaitDir);
	
	memset(&m_curWaitDir,0,sizeof(m_curWaitDir));
	m_curWaitDir.type = ACCU_DIR::invalid;
	m_bCanSurePoint = FALSE;
}

void CAccuBox::UnlockDir(int idx)
{
	if( idx<0 )
	{
		memset(&m_curWaitDir,0,sizeof(m_curWaitDir));
	}
	else if( idx<m_dirs.GetSize() )
	{
		m_dirs.RemoveAt(idx);
	}
}


BOOL CAccuBox::IsWaitDir()
{
	return ( m_curWaitDir.type==ACCU_DIR::extension || 
		m_curWaitDir.type==ACCU_DIR::parallel || m_curWaitDir.type==ACCU_DIR::perpendicular );
}


const ACCU_DIR* CAccuBox::GetLockedDirs(int& num)
{
	num = m_dirs.GetSize();
	return m_dirs.GetData();
}


void CAccuBox::MoveDir(PT_3D pt0, PT_3D pt1)
{
	m_curWaitDir.ptLine0 = pt0;
	m_curWaitDir.ptLine1 = pt1;

	if( (pt0.x!=0 || pt0.y!=0 || 
		 pt1.x!=0 || pt1.y!=0 ) &&
		(pt1.x-pt0.x!=0 || pt1.y-pt0.y!=0) )
	{
		GlobalToLocal(&m_curWaitDir.ptLine0);
		GlobalToLocal(&m_curWaitDir.ptLine1);
	}
}


BOOL CAccuBox::ClickDir(PT_3D pt)
{
	if( m_curWaitDir.type!=ACCU_DIR::invalid )
	{
		if( !m_bCanSurePoint )
		{
			if (GraphAPI::GIsEqual2DPoint(&m_curWaitDir.ptLine0, &m_curWaitDir.ptLine1))
				return FALSE;
			m_dirs.Add(m_curWaitDir);
			LockDir(ACCU_DIR::invalid,TRUE);
			m_bNeedRedraw = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}


void CAccuBox::ClearDir()
{
	LockDir(ACCU_DIR::invalid,TRUE);
	m_dirs.RemoveAll();
}

BOOL CAccuBox::ThinkDir(PT_3D& pt)
{
	m_arrDrawDirPts.RemoveAll();
	m_bCanSurePoint = FALSE;

	BOOL bTempAdd = FALSE, bRet = FALSE;
	int num = m_dirs.GetSize();
	BOOL bBaseDir = (num>0);

	if( m_curWaitDir.type!=ACCU_DIR::invalid &&
		( m_curWaitDir.ptLine1.x!=m_curWaitDir.ptLine0.x||
		  m_curWaitDir.ptLine1.y!=m_curWaitDir.ptLine0.y) )
	{
		m_dirs.Add(m_curWaitDir);
		bTempAdd = TRUE;
		num++;
	}
	else if( num<=0 )return FALSE;
	
	if( m_bLockX || m_bLockY ||
		(((m_nStatus&2)!=0||(m_nStatus&4)!=0)&&(m_nStatus&1)==0/*&&!bBaseDir*/) )
	{
		PT_3D ptline00(0,0,0), ptline01(0,0,0);
		//计算吸附/锁定线
		if( m_bLockX )
		{
			ptline00.x = ptline01.x = m_ptDataM.x;
			ptline01.y = 100.0;
		}
		else if( m_bLockY )
		{
			ptline00.y = ptline01.y = m_ptDataM.y;
			ptline01.x = 100.0;
		}
		else if( (m_nStatus&2)!=0 )
		{
			ptline01.x = 100.0;
		}
		else if( (m_nStatus&4)!=0 )
		{
			ptline01.y = 100.0;
		}
		
		//与方向线求交点
		for( int i=0; i<num; i++)
		{
			if( m_dirs.GetAt(i).type!=ACCU_DIR::invalid )break;
		}
		
		if( i<num )
		{
			PT_3D ptline10(0,0,0), ptline11(0,0,0);
		
			ACCU_DIR dir = m_dirs.GetAt(i);

			switch(dir.type) 
			{
			case ACCU_DIR::extension:
				ptline10 = dir.ptLine0; 
				ptline11 = dir.ptLine1;
				break;
			case ACCU_DIR::parallel:
				ptline11.x = dir.ptLine1.x-dir.ptLine0.x; 
				ptline11.y = dir.ptLine1.y-dir.ptLine0.y;
				if( fabs(ptline11.x)<1e-4 ) ptline11.x = 0;
				if( fabs(ptline11.y)<1e-4 ) ptline11.y = 0;
				break;
			case ACCU_DIR::perpendicular:
				ptline11.x = dir.ptLine1.y-dir.ptLine0.y; 
				ptline11.y = dir.ptLine0.x-dir.ptLine1.x;
				if( fabs(ptline11.x)<1e-4 ) ptline11.x = 0;
				if( fabs(ptline11.y)<1e-4 ) ptline11.y = 0;
				break;
			}

			double x, y;
			if( GraphAPI::GGetLineIntersectLine(ptline00.x,ptline00.y,
				ptline01.x-ptline00.x,ptline01.y-ptline00.y,
				ptline10.x,ptline10.y,
				ptline11.x-ptline10.x,ptline11.y-ptline10.y,&x,&y,NULL) )
			{
				pt.x = x; pt.y = y;

				if(dir.type == ACCU_DIR::extension)
				{
					if( fabs(x-dir.ptLine0.x)<fabs(x-dir.ptLine1.x) )
					{
						m_arrDrawDirPts.Add(dir.ptLine0);
					}
					else
						m_arrDrawDirPts.Add(dir.ptLine1);
				}

			//	m_arrDrawDirPts.Add(pt);
				bRet = TRUE;
				m_bCanSurePoint = TRUE;
			}
		}


	}

	if( !bRet && m_bLockAngle )
	{
		PT_3D ptline00(0,0,0), ptline01(0,0,0);
		
		double cosA = cos(m_lfAngle);
		double sinA = sin(m_lfAngle);
		if( fabs(cosA)<1e-10 ) cosA = 0;
		if( fabs(sinA)<1e-10 ) sinA = 0;
		
		ptline01.x = 100.0*cosA;
		ptline01.y = 100.0*sinA;

		//锁定角度时，只能与延长线求交点
		for( int i=0; i<num; i++)
		{
			if( m_dirs.GetAt(i).type==ACCU_DIR::extension )break;
		}

		if( i<num )
		{
			//计算延长线与吸附/锁定线的交点
			double x,y;
			ACCU_DIR dir = m_dirs.GetAt(i);
			if( GraphAPI::GGetLineIntersectLine(ptline00.x,ptline00.y,
				ptline01.x-ptline00.x,ptline01.y-ptline00.y,
				dir.ptLine0.x,dir.ptLine0.y,
				dir.ptLine1.x-dir.ptLine0.x,dir.ptLine1.y-dir.ptLine0.y,&x,&y,NULL) )
			{
				pt.x = x; pt.y = y;
				if( fabs(x-dir.ptLine0.x)<fabs(x-dir.ptLine1.x) )
				{
					m_arrDrawDirPts.Add(dir.ptLine0);
				}
				else
					m_arrDrawDirPts.Add(dir.ptLine1);
				m_arrDrawDirPts.Add(pt);
				bRet = TRUE;
				m_bCanSurePoint = TRUE;
			}
		}
		
	}
	
	if( !bRet && m_bLockDistance && m_lfDistance>0 )
	{
		//方向线
		for( int i=0; i<num; i++)
		{
			if( m_dirs.GetAt(i).type!=ACCU_DIR::invalid )break;
		}
		
		if( i<num )
		{
			PT_3D ptline10(0,0,0), ptline11(0,0,0);
		
			ACCU_DIR dir = m_dirs.GetAt(i);

			switch(dir.type) 
			{
			case ACCU_DIR::extension:
				ptline10 = dir.ptLine0; 
				ptline11 = dir.ptLine1;
				break;
			case ACCU_DIR::parallel:
				ptline11.x = dir.ptLine1.x-dir.ptLine0.x; 
				ptline11.y = dir.ptLine1.y-dir.ptLine0.y;
				break;
			case ACCU_DIR::perpendicular:
				ptline11.x = dir.ptLine1.y-dir.ptLine0.y; 
				ptline11.y = dir.ptLine0.x-dir.ptLine1.x;
				break;
			}

			//计算延长线与定长距离的交点
			if( dir.type==ACCU_DIR::extension )
			{
				double xr,yr,x1,y1,x2,y2;
				double dis = GraphAPI::GGetNearestDisOfPtToLine(ptline10.x,ptline10.y,ptline11.x,ptline11.y,
					0,0,&xr,&yr);
				if( dis<m_lfDistance )
				{
					double l = sqrt(m_lfDistance*m_lfDistance-dis*dis);
					double r = sqrt((ptline11.x-ptline10.x)*(ptline11.x-ptline10.x)+
						(ptline11.y-ptline10.y)*(ptline11.y-ptline10.y));

					x1 = xr+l*(ptline11.x-ptline10.x)/r;
					y1 = yr+l*(ptline11.y-ptline10.y)/r;
					x2 = xr-l*(ptline11.x-ptline10.x)/r;
					y2 = yr-l*(ptline11.y-ptline10.y)/r;
					if( (pt.x-x1)*(pt.x-x1)+(pt.y-y1)*(pt.y-y1)<(pt.x-x2)*(pt.x-x2)+(pt.y-y2)*(pt.y-y2) )
					{	pt.x = x1; pt.y = y1; }
					else
					{	pt.x = x2; pt.y = y2; }

					if( fabs(pt.x-ptline10.x)<fabs(pt.x-ptline11.x) )
					{
						m_arrDrawDirPts.Add(ptline10);
					}
					else
						m_arrDrawDirPts.Add(ptline11);
					m_arrDrawDirPts.Add(pt);

					m_arrDrawDirPts.Add(pt);
					PT_3D t(0,0,m_ptData.z);
					m_arrDrawDirPts.Add(t);

					bRet = TRUE;
				}
			}
			//计算平行线或垂线与定长距离的交点
			else
			{
				double r = sqrt(ptline11.x*ptline11.x+ptline11.y*ptline11.y);
				double x1 = m_lfDistance*ptline11.x/r, y1 = m_lfDistance*ptline11.y/r;
				double x2 = -m_lfDistance*ptline11.x/r, y2 = -m_lfDistance*ptline11.y/r;

				if( (pt.x-x1)*(pt.x-x1)+(pt.y-y1)*(pt.y-y1)<(pt.x-x2)*(pt.x-x2)+(pt.y-y2)*(pt.y-y2) )
				{	pt.x = x1; pt.y = y1; }
				else
				{	pt.x = x2; pt.y = y2; }

				bRet = TRUE;
			}
		}
	}
	
	if( !bRet )
	{
		PT_3D ptline00(0,0,0), ptline01(0,0,0);
		PT_3D ptline10(0,0,0), ptline11(0,0,0);
		//找到第一个方向（必须是延长线）
		for( int i=0; i<num; i++)
		{
			if( m_dirs.GetAt(i).type==ACCU_DIR::extension )break;
		}

		if( i<num )
		{
			ACCU_DIR dir = m_dirs.GetAt(i);
			ptline00 = dir.ptLine0; 
			ptline01 = dir.ptLine1;
		}

		//找到第一个方向（平行线或者垂线）
		for( int j=0; j<num; j++)
		{
			if( j!=i && m_dirs.GetAt(j).type!=ACCU_DIR::invalid )break;
		}

		if( j<num )
		{
			ACCU_DIR dir = m_dirs.GetAt(j);
			switch(dir.type) 
			{
			case ACCU_DIR::extension:
				ptline10 = dir.ptLine0; 
				ptline11 = dir.ptLine1;
				break;
			case ACCU_DIR::parallel:
				ptline11.x = dir.ptLine1.x-dir.ptLine0.x; 
				ptline11.y = dir.ptLine1.y-dir.ptLine0.y;
				break;
			case ACCU_DIR::perpendicular:
				ptline11.x = dir.ptLine1.y-dir.ptLine0.y; 
				ptline11.y = dir.ptLine0.x-dir.ptLine1.x;
				break;
			}
		}

		//计算两个方向之间的交点，这里要求第一个方向必然是延长线方向，
		//两个平行线或者垂线的相交是没有意义的，必然相交于当前数据点
		if( i<num && j<num )
		{
			double x,y;
			ACCU_DIR dir = m_dirs.GetAt(i);
			if( GraphAPI::GGetLineIntersectLine(ptline00.x,ptline00.y,
				ptline01.x-ptline00.x,ptline01.y-ptline00.y,
				ptline10.x,ptline10.y,
				ptline11.x-ptline10.x,ptline11.y-ptline10.y,&x,&y,NULL) )
			{
				pt.x = x; pt.y = y;

				if( fabs(x-ptline00.x)<fabs(x-ptline01.x) )
				{
					m_arrDrawDirPts.Add(ptline00);
				}
				else
					m_arrDrawDirPts.Add(ptline01);

				m_arrDrawDirPts.Add(pt);
				m_arrDrawDirPts.Add(pt);
				if( fabs(x-ptline10.x)<fabs(x-ptline11.x) )
				{
					m_arrDrawDirPts.Add(ptline10);
				}
				else
					m_arrDrawDirPts.Add(ptline11);

				bRet = TRUE;
				m_bCanSurePoint = TRUE;
			}
		}
		//只有一个方向有效
		else if( i<num || j<num )
		{
			if( j<num )
			{
				ptline00 = ptline10; 
				ptline01 = ptline11;
			}

			double x,y;
			double dis = GraphAPI::GGetNearestDisOfPtToLine(ptline00.x,ptline00.y,ptline01.x,ptline01.y,
				pt.x,pt.y,&x,&y);
			pt.x = x; pt.y = y;

			if( fabs(x-ptline00.x)<fabs(x-ptline01.x) )
			{
				m_arrDrawDirPts.Add(ptline00);
			}
			else
				m_arrDrawDirPts.Add(ptline01);
			m_arrDrawDirPts.Add(pt);

			bRet = TRUE;
		}
	}

	if( bTempAdd )
		m_dirs.RemoveAt(m_dirs.GetSize()-1);
	if (bRet)
	{
		m_bNeedRedraw = TRUE;
	}
	return bRet;
}



void CAccuBox::DrawDir(CCoordWnd wnd,GrBuffer2d *pBufL,GrBuffer2d *pBufR)
{
//	float gscale = GetGrdToCltScale(wnd);
	int num = m_arrDrawDirPts.GetSize();
	PT_3D pt0, pt1;
	PT_4D pt4d0,pt4d1;
	for( int i=1; i<num; i+=2 )
	{
		pt0 = m_arrDrawDirPts.GetAt(i-1);
		pt1 = m_arrDrawDirPts.GetAt(i);

		LocalToGlobal(&pt0);
		LocalToGlobal(&pt1);
		pt4d0 = PT_4D(pt0);
		pt4d1 = PT_4D(pt1);
		wnd.m_pViewCS->GroundToClient(&pt4d0,1);
		wnd.m_pViewCS->GroundToClient(&pt4d1,1);
		pBufL->Dash(pt4d0.ToL2D(),pt4d1.ToL2D(),4,4);
		pBufR->Dash(pt4d0.ToR2D(),pt4d1.ToR2D(),4,4);
	}
}


void CAccuBox::Click(PT_3D pt)
{
	if( !m_bOpen ) return;

	m_bNeedRedraw = TRUE;
	if( m_nClick==0 )
	{
		m_lfBaseAngle = 0;
		m_ptOrigin = pt;
		m_nClick++;
	}
	else
	{
		LocalToGlobal(&m_ptData);
		m_ptOrigin = pt;
		
		m_lfBaseAngle_reserve = GraphAPI::GGetAngle(m_ptData.x,m_ptData.y,pt.x,pt.y);

		if( sysRelative==m_nCoordBase )
		{
			m_lfBaseAngle = GraphAPI::GGetAngle(m_ptData.x,m_ptData.y,pt.x,pt.y);	
		}
		else
		{
			m_lfBaseAngle = 0;
		}

		m_nClick++;

		GlobalToLocal(&pt);
		GlobalToLocal(&m_ptData);

		m_ptData = pt;
		m_ptDataM = pt;
	}
	
	m_bBoxValid = TRUE;

	ClearDir();

	if(m_bLockX) LockX(FALSE);
	if(m_bLockY) LockY(FALSE);

	UpdateDialog();
}

void CAccuBox::MoveDataPt(CCoordWnd wnd, PT_3D& pt)
{
	if( !m_bOpen || !m_bBoxValid )return;
	if( m_nClick<=0 )return;

	if(m_nRightAnglePlace==rightangleIM)
	{
		if(m_nClick<=1 && m_dirs.GetSize()<=0 )
			return;
	}

	PT_3D pts[2], savePt;
	pts[0] = pt;

	GlobalToLocal(&pt);
	savePt = pt;

	double len = sqrt((pt.x-m_ptData.x)*(pt.x-m_ptData.x)+(pt.y-m_ptData.y)*(pt.y-m_ptData.y));
	double ang = GraphAPI::GGetAngle(m_ptData.x,m_ptData.y,pt.x,pt.y);
	m_nStatus  = 0;

	//吸附方向调整
	if( m_bLockIndex )
	{
		
		pts[1] = pt;

		ang = ang/PI*4;
		//靠近y轴
		if( (ang>1 && ang<3)||(ang>5 && ang<7) )
			pts[1].x = m_ptData.x;
		//靠近x轴
		else
			pts[1].y = m_ptData.y;

		//计算邻近距离
		LocalToGlobal(pts+1);

		PT_4D pt4ds[2];
		pt4ds[0] = PT_4D(pts[0]); pt4ds[1] = PT_4D(pts[1]);
		wnd.m_pViewCS->GroundToClient(pt4ds,2);

		double r = sqrt( (pt4ds[0].x-pt4ds[1].x)*(pt4ds[0].x-pt4ds[1].x) + 
			(pt4ds[0].y-pt4ds[1].y)*(pt4ds[0].y-pt4ds[1].y) );

		//吸附中
		if( r<=m_nTolerance )
		{
			if(m_nDrgMWid!=3)
			    m_bNeedRedraw = TRUE;
			if( (ang>1 && ang<3)||(ang>5 && ang<7) )
			{
				pt.x = m_ptData.x;
				m_nStatus |= 4;
			}
			else
			{
				pt.y = m_ptData.y;
				m_nStatus |= 2;
			}
			m_nDrgMWid = 3;
		}
		else 
		{	
			if(m_nDrgMWid!=1)
				m_bNeedRedraw = TRUE;
			m_nDrgMWid = 1;
		}
	}

	//X,Y,Z方向锁定的调整
	if( m_bLockX )pt.x = m_ptDataM.x;
	if( m_bLockY )pt.y = m_ptDataM.y;

	//更新长度和角度
	len = sqrt((pt.x-m_ptData.x)*(pt.x-m_ptData.x)+(pt.y-m_ptData.y)*(pt.y-m_ptData.y));
	ang = GraphAPI::GGetAngle(m_ptData.x,m_ptData.y,pt.x,pt.y);
	
	//角度锁定的调整
	if( m_bLockAngle )
	{
		m_bNeedRedraw = TRUE;
		double delta = ang - m_lfAngle;
		//更新长度和角度
		ang = m_lfAngle;

		if( m_bLockX && m_bLockY )
		{
		}
		else if (m_bLockX )
		{
			if( pt.x > 0 ) //角度的取值范围只能为[0, 0.5*PI)、(1.5*PI, 2*PI]、(-0.5*PI, 0]
			{
				if( (ang>=0 && ang <0.5*PI) || (ang>1.5*PI && ang<=2*PI) || (ang>-0.5*PI && ang<=0) )
					pt.y = pt.x * tan(ang);
			}
			else if( pt.x < 0 )	//角度的取值范围只能为 (0.5*PI, 1.5*PI)、(-0.5*PI, -PI]
			{
				if( (ang>0.5*PI && ang<1.5*PI) || (ang>-0.5*PI && ang<=PI) )
					pt.y = pt.x * tan(ang);
			}
		}
		else if (m_bLockY )
		{
			if( pt.y > 0 )	//角度的取值范围只能为 (0,PI)
			{
				if( ang>0 && ang<PI )
					pt.x = pt.y / tan(ang);
			}
			else if( pt.y < 0 )	//角度的取值范围只能为 (PI, 2*PI)、(-PI,0);
			{
				if( (ang>PI && ang<2*PI) || (ang>-PI && ang<0) )
					pt.x = pt.y / tan(ang);
			}
		}
		else
		{
			double cosa = cos(delta);
			if( fabs(cosa)<1e-10 ) cosa = 0;
			len = len*cosa;
			
			double cosA = cos(m_lfAngle);
			double sinA = sin(m_lfAngle);
			if( fabs(cosA)<1e-10 ) cosA = 0;
			if( fabs(sinA)<1e-10 ) sinA = 0;

			pt.x = m_ptData.x + len*cosA;
			pt.y = m_ptData.y + len*sinA;
		}
	}

	//距离锁定的调整
	if( m_bLockDistance )
	{
		m_bNeedRedraw = TRUE;
		if( m_bLockX && m_bLockY )
		{
		}
		else if (m_bLockX )
		{
			double yLen = sqrt( m_lfDistance*m_lfDistance - pt.x*pt.x );
			if( pt.y < 0 )
				pt.y = -yLen;
			else
				pt.y = yLen;
		}
		else if( m_bLockY )
		{
			double xLen = sqrt( m_lfDistance*m_lfDistance - pt.y*pt.y );
			if( pt.x < 0 )
				pt.x = -xLen;
			else
				pt.x = xLen;
		}
		else
		{
			double cosA = cos(ang);
			double sinA = sin(ang);
			if( fabs(cosA)<1e-10 ) cosA = 0;
			if( fabs(sinA)<1e-10 ) sinA = 0;

			pt.x = m_ptData.x + m_lfDistance*cosA;
			pt.y = m_ptData.y + m_lfDistance*sinA;
		}
	}

	if( m_bLockX || m_bLockY/* || m_bLockZ*/ ||
		m_bLockDistance || m_bLockAngle )
		m_nStatus |= 1;

	m_ptDataM = pt;
	m_bDrgValid = TRUE;

	if( ThinkDir(savePt) )
	{
		m_ptDataM = savePt;
		pt = savePt;
	}

	UpdateDisAng();
	UpdateDialog();
	LocalToGlobal(&pt);
}

static void GRotate(double *px, double *py, int sum, double x0, double y0, double angle)
{
	double dx,dy,sina,cosa;
	
	sina = sin(angle);
	cosa = cos(angle);
	
	for (int i=0; i<sum; i++)
	{
		dx = px[i] - x0;
		dy = py[i] - y0;
		
		px[i] = dx * cosa - dy * sina  + x0;
		py[i] = dy * cosa + dx * sina  + y0;
	}
}


void CAccuBox::DrawBox(CCoordWnd wnd, GrBuffer2d *pBufL,GrBuffer2d *pBufR)
{
	if( !m_bOpen|| !pBufL || !pBufR || !m_bBoxValid || !m_bViewDraw )return;

	double ang = m_lfBaseAngle;

	PT_4D  pt4d = PT_4D(m_ptOrigin), pt4d1 = PT_4D(m_ptOrigin);
	pt4d1.x += 100 * cos(ang);  pt4d1.y += 100 * sin(ang);
	wnd.m_pViewCS->GroundToClient(&pt4d,1);
	wnd.m_pViewCS->GroundToClient(&pt4d1,1);
	ang = -GraphAPI::GGetAngle(pt4d.x,pt4d.y,pt4d1.x,pt4d1.y);

	PT_3D pt = m_ptData;
	LocalToGlobal(&pt);
	pt4d = PT_4D(pt);
	wnd.m_pViewCS->GroundToClient(&pt4d,1);
	m_ptDraw = pt4d;

	
	double x[4], y[4], z[4], yr[4];
	double xb[4],yb[4],zb[4], yrb[4], xe[4],ye[4],ze[4],yre[4];
	
	GrBuffer bufL, bufR;
	{
		//外框
		x[0] = m_ptDraw.x - m_nBoxWid;	y[0] = m_ptDraw.y - m_nBoxWid;   
		x[1] = m_ptDraw.x + m_nBoxWid;	y[1] = m_ptDraw.y - m_nBoxWid;   
		x[2] = m_ptDraw.x + m_nBoxWid;	y[2] = m_ptDraw.y + m_nBoxWid;   
		x[3] = m_ptDraw.x - m_nBoxWid;	y[3] = m_ptDraw.y + m_nBoxWid;   
	//	x[4] = m_ptDraw.x - m_nBoxWid;	y[4] = m_ptDraw.y - m_nBoxWid;  
		
		z[0] = m_ptDraw.z - m_nBoxWid;  yr[0] = m_ptDraw.yr - m_nBoxWid; 
		z[1] = m_ptDraw.z + m_nBoxWid;  yr[1] = m_ptDraw.yr - m_nBoxWid; 
		z[2] = m_ptDraw.z + m_nBoxWid;  yr[2] = m_ptDraw.yr + m_nBoxWid; 
		z[3] = m_ptDraw.z - m_nBoxWid;  yr[3] = m_ptDraw.yr + m_nBoxWid; 
	//	z[4] = m_ptDraw.z - m_nBoxWid;  yr[4] = m_ptDraw.yr - m_nBoxWid; 		
		
		//x轴负向标线
		xb[0] = x[0] - m_nStickLen/2.0;	yb[0] = m_ptDraw.y;  
		xe[0] = x[0] + m_nStickLen/2.0;	ye[0] = m_ptDraw.y;  

		zb[0] = z[0] - m_nStickLen/2.0; yrb[0] = m_ptDraw.yr;  
		ze[0] = z[0] + m_nStickLen/2.0; yre[0] = m_ptDraw.yr;  
		
		//y轴负向标线
		xb[1] = m_ptDraw.x;	yb[1] = y[2] - m_nStickLen/2.0;   
		xe[1] = m_ptDraw.x;	ye[1] = y[2] + m_nStickLen/2.0;   

		zb[1] = m_ptDraw.z; yrb[1] = yr[2] - m_nStickLen/2.0;   
		ze[1] = m_ptDraw.z; yre[1] = yr[2] + m_nStickLen/2.0;   
		
		//x轴正向标线
		xb[2] = x[1] - m_nStickLen/2.0;	yb[2] = m_ptDraw.y;
		xe[2] = x[1] + m_nStickLen/2.0;	ye[2] = m_ptDraw.y;  

		zb[2] = z[1] - m_nStickLen/2.0; yrb[2] = m_ptDraw.yr;
		ze[2] = z[1] + m_nStickLen/2.0; yre[2] = m_ptDraw.yr;  
		
		//y轴正向标线
		xb[3] = m_ptDraw.x;	yb[3] = y[1] - m_nStickLen/2.0;    
		xe[3] = m_ptDraw.x;	ye[3] = y[1] + m_nStickLen/2.0;    

		zb[3] = m_ptDraw.z; yrb[3] = yr[1] - m_nStickLen/2.0;    
		ze[3] = m_ptDraw.z; yre[3] = yr[1] + m_nStickLen/2.0;    
		
		GRotate (x , y , 4, m_ptDraw.x, m_ptDraw.y, -ang);
		GRotate (z , yr , 4, m_ptDraw.z, m_ptDraw.yr, -ang); 
		
		GRotate (xb, yb, 4, m_ptDraw.x, m_ptDraw.y, -ang);
		GRotate (zb, yrb , 4, m_ptDraw.z, m_ptDraw.yr, -ang); 
	
		GRotate (xe, ye, 4, m_ptDraw.x, m_ptDraw.y, -ang);
		GRotate (ze, yre , 4, m_ptDraw.z, m_ptDraw.yr, -ang); 

		PT_4D Frame[4]/*, t[5]*/;
		PT_4D b[4], e[4];
		Frame[0].x = x[0];Frame[0].y = y[0];
		Frame[0].z = z[0];Frame[0].yr = yr[0];

		Frame[1].x = x[1];Frame[1].y = y[1];
		Frame[1].z = z[1];Frame[1].yr = yr[1];

		Frame[2].x = x[2];Frame[2].y = y[2];
		Frame[2].z = z[2];Frame[2].yr = yr[2];

		Frame[3].x = x[3];Frame[3].y = y[3];
		Frame[3].z = z[3];Frame[3].yr = yr[3];

		b[0].x = xb[0];b[0].y = yb[0];
		b[0].z = zb[0];b[0].yr = yrb[0];

		b[1].x = xb[1];b[1].y = yb[1];
		b[1].z = zb[1];b[1].yr = yrb[1];

		b[2].x = xb[2];b[2].y = yb[2];
		b[2].z = zb[2];b[2].yr = yrb[2];

		b[3].x = xb[3];b[3].y = yb[3];
		b[3].z = zb[3];b[3].yr = yrb[3];

		e[0].x = xe[0];e[0].y = ye[0];
		e[0].z = ze[0];e[0].yr = yre[0];
		
		e[1].x = xe[1];e[1].y = ye[1];
		e[1].z = ze[1];e[1].yr = yre[1];
		
		e[2].x = xe[2];e[2].y = ye[2];
		e[2].z = ze[2];e[2].yr = yre[2];
		
		e[3].x = xe[3];e[3].y = ye[3];
		e[3].z = ze[3];e[3].yr = yre[3];

// 		wnd.m_pViewCS->ClientToGround(Frame,4);
// 		wnd.m_pViewCS->ClientToGround(b,4);
// 		wnd.m_pViewCS->ClientToGround(e,4);
		//绘制边框
		pBufL->BeginLineString(RGB(192,192,192),1,FALSE);	
		pBufR->BeginLineString(RGB(192,192,192),1,FALSE);
		for (int i=0; i<4; i++)
		{			
			if( i==0 )
			{
				pBufL->MoveTo(&(Frame[i].ToL2D()));
				pBufR->MoveTo(&(Frame[i].ToR2D()));
			}
			else 
			{
				pBufL->LineTo(&(Frame[i].ToL2D()));
				pBufR->LineTo(&(Frame[i].ToR2D()));
			}
		}
		pBufL->LineTo(&(Frame[0].ToL2D()));
		pBufR->LineTo(&(Frame[0].ToR2D()));
		pBufL->End();
		pBufR->End();
		
		//绘制x，y轴负向
		pBufL->BeginLineString(RGB(255,255,255),1,FALSE);
		pBufL->MoveTo(&b[0].ToL2D());
		pBufL->LineTo(&e[0].ToL2D());
		pBufL->MoveTo(&b[1].ToL2D());
		pBufL->LineTo(&e[1].ToL2D());
		pBufL->End();
		pBufR->BeginLineString(RGB(255,255,255),1,FALSE);
		pBufR->MoveTo(&b[0].ToR2D());
		pBufR->LineTo(&e[0].ToR2D());
		pBufR->MoveTo(&b[1].ToR2D());
		pBufR->LineTo(&e[1].ToR2D());
		pBufR->End();

		//绘制x,y轴正向
		pBufL->BeginLineString(RGB(255,0,0),3,FALSE);
		pBufL->MoveTo(&b[2].ToL2D());
		pBufL->LineTo(&e[2].ToL2D());
		pBufL->End();
		pBufL->BeginLineString(RGB(0,0,255),3,FALSE);
		pBufL->MoveTo(&b[3].ToL2D());
		pBufL->LineTo(&e[3].ToL2D());
		pBufL->End();
		pBufR->BeginLineString(RGB(255,0,0),3,FALSE);
		pBufR->MoveTo(&b[2].ToR2D());
		pBufR->LineTo(&e[2].ToR2D());
		pBufR->End();
		pBufR->BeginLineString(RGB(0,0,255),3,FALSE);
		pBufR->MoveTo(&b[3].ToR2D());
		pBufR->LineTo(&e[3].ToR2D());
		pBufR->End();

		//绘制中心点
		CRect	rc;	
		PT_4D pt0,pt1;
		pBufL->BeginLineString(RGB(255,255,255),1,FALSE);
		pBufR->BeginLineString(RGB(255,255,255),1,FALSE);

		for( i=1; i<=m_nRectWid; i++)
		{
			pt0.x = int(m_ptDraw.x-i); pt0.y = int(m_ptDraw.y+i);
			pt0.z = int(m_ptDraw.z-i); pt0.yr = int(m_ptDraw.yr+i);			
			pBufL->MoveTo(&pt0.ToL2D());
			pBufR->MoveTo(&pt0.ToR2D());
			pt0.x = int(m_ptDraw.x+i); pt0.y = int(m_ptDraw.y+i);
			pt0.z = int(m_ptDraw.z+i); pt0.yr = int(m_ptDraw.yr+i);				
			pBufL->LineTo(&pt0.ToL2D());
			pBufR->LineTo(&pt0.ToR2D());
			pt0.x = int(m_ptDraw.x+i); pt0.y = int(m_ptDraw.y-i);
			pt0.z = int(m_ptDraw.z+i); pt0.yr = int(m_ptDraw.yr-i);		
			pBufL->LineTo(&pt0.ToL2D());
			pBufR->LineTo(&pt0.ToR2D());
			pt0.x = int(m_ptDraw.x-i); pt0.y = int(m_ptDraw.y-i);
			pt0.z = int(m_ptDraw.z-i); pt0.yr = int(m_ptDraw.yr-i);		
			pBufL->LineTo(&pt0.ToL2D());
			pBufR->LineTo(&pt0.ToR2D());
			pt0.x = int(m_ptDraw.x-i); pt0.y = int(m_ptDraw.y+i);
			pt0.z = int(m_ptDraw.z-i); pt0.yr = int(m_ptDraw.yr+i);		
			pBufL->LineTo(&pt0.ToL2D());
			pBufR->LineTo(&pt0.ToR2D());
		}
		pBufL->End();
		pBufR->End();
	}
}


float CAccuBox::GetGrdToCltScale(CCoordWnd& wnd)
{
	PT_3D  pt = m_ptData;
	LocalToGlobal(&pt);
	
	PT_4D pt4d = PT_4D(pt);
	wnd.m_pViewCS->GroundToClient(&pt4d,1);

	pt4d.x += 10;  pt4d.z += 10;
	wnd.m_pViewCS->ClientToGround(&pt4d,1);

	return (pt4d.x-pt.x)/10;
}


void CAccuBox::DrawDrg(CCoordWnd wnd,GrBuffer2d *pBufL,GrBuffer2d *pBufR)
{
	if( !m_bOpen|| !pBufL || !pBufR || !m_bDrgValid || !m_bViewDraw )return;

//	float gscale = GetGrdToCltScale(wnd);
	
	PT_3D pt,pt0,pt2;
	int wid = m_bLockAngle?3:m_nDrgMWid;
	PT_4D pt4d0,pt4d1;
	if( wid>1 )
	{		
		pBufL->BeginLineString(RGB(255,255,255),wid,FALSE);
		pBufR->BeginLineString(RGB(255,255,255),wid,FALSE);
		pt = m_ptData;
		LocalToGlobal(&pt);
		pt4d0 = PT_4D(pt);
		wnd.m_pViewCS->GroundToClient(&pt4d0,1);
		pBufL->MoveTo(&(pt4d0.ToL2D()));
		pBufR->MoveTo(&(pt4d0.ToR2D()));

		pt = m_ptDataM;
		LocalToGlobal(&pt);
		pt4d0 = PT_4D(pt);
		wnd.m_pViewCS->GroundToClient(&pt4d0,1);
		pBufL->LineTo(&(pt4d0.ToL2D()));
		pBufR->LineTo(&(pt4d0.ToR2D()));
		pBufL->End();
		pBufR->End();
	}

	pt = m_ptDataM;
	LocalToGlobal(&pt);
	
	pBufL->BeginLineString(RGB(255,255,255),1,FALSE);
	pBufR->BeginLineString(RGB(255,255,255),1,FALSE);
	if( m_bLockX )
	{
		PT_3D pt1 = m_ptDataM;
		pt1.y = m_ptData.y;
		LocalToGlobal(&pt1);
		pt4d0 = PT_4D(pt);
		wnd.m_pViewCS->GroundToClient(&pt4d0,1);
		pt4d1 = PT_4D(pt1);
		wnd.m_pViewCS->GroundToClient(&pt4d1,1);
		
		pBufL->Dash(pt4d0.ToL2D(),pt4d1.ToL2D(),4,4);
		pBufR->Dash(pt4d0.ToR2D(),pt4d1.ToR2D(),4,4);
	}

	if( m_bLockY )
	{
		PT_3D pt1 = m_ptDataM;
		pt1.x = m_ptData.x;
		LocalToGlobal(&pt1);

		pt4d0 = PT_4D(pt);
		wnd.m_pViewCS->GroundToClient(&pt4d0,1);
		pt4d1 = PT_4D(pt1);
		wnd.m_pViewCS->GroundToClient(&pt4d1,1);		

		pBufL->Dash(pt4d0.ToL2D(),pt4d1.ToL2D(),4,4);
		pBufR->Dash(pt4d0.ToR2D(),pt4d1.ToR2D(),4,4);
	}
	
	DrawDir(wnd,pBufL,pBufR);
	pBufL->End();
	pBufR->End();	
}

void CAccuBox::Reset()
{
	m_bNeedRedraw = FALSE;
	m_lfBaseAngle = 0;
	m_nDrgMWid = 1;
	m_nStatus = 0;
	
	m_bBoxValid = FALSE;
	m_bDrgValid = FALSE;
	m_nClick = 0;
}

