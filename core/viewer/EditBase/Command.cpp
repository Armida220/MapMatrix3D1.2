// EBCommand.cpp: implementation of the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "Command.h"
#include "Editor.h"
#include "GeoCurve.h"
//#include "RegDef.h"
#include "RegDef2.h"
#include "UIParam2.h"
#include "Linearizer.h"
#include "DataSource.h"
#include "SmartViewFunctions.h"
#include "Resource.h"
#include "DisplayObject.h"
#include "ExMessage.h"
#include "GeoText.h"
#include "Functions_temp.h"
#include "PlotText.h "

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin
IMPLEMENT_DYNAMIC(CProcedure,CObject)

CProcedure::CProcedure()
{
	m_nState = PROCSTATE_NOT_INIT;
	m_bSettingModified = FALSE;
	m_pEditor = NULL;
	m_bSettingModified = FALSE;
	m_nExitCode = CMPEC_NULL;

	_tcscpy(m_strRegPath, REGPATH_COMMAND);
}


CProcedure::~CProcedure()
{
}


void CProcedure::PtClick(PT_3D &pt, int flag)
{
	if( IsProcReady(this) )
	{
		GotoState(PROCSTATE_PROCESSING);
	}
	return;
}

void CProcedure::PtDblClick(PT_3D &pt, int flag)
{
	if( !(IsProcFinished(this)||IsProcAborted(this)) )PtClick(pt,flag);
	if( !(IsProcFinished(this)||IsProcAborted(this)) )PtClick(pt,flag);
	return;
}

void CProcedure::PtMove(PT_3D &pt)
{
}


void CProcedure::PtReset(PT_3D &pt)
{
}


void CProcedure::GetParams(CValueTable& tab)
{
}


void CProcedure::SetParams(CValueTable& tab,BOOL bInit)
{

}


BOOL CProcedure::Keyin(LPCTSTR text)
{
	return FALSE;
}


void CProcedure::UpdateParams(BOOL bSave)
{
	if( bSave )
	{
		if (m_bSettingModified)
		{
			m_pEditor->UIRequest(UIREQ_TYPE_SAVEFILEPARAM, 0);
		}
		
	}
	else
		m_pEditor->UIRequest(UIREQ_TYPE_LOADFILEPARAM, 0);
	
	m_bSettingModified = FALSE;
}

void CProcedure::PromptString(LPCTSTR str)
{
	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)str);
}


void CProcedure::FillShowParams(CUIParam* param, BOOL bForLoad)
{
}

void CProcedure::Init(CEditor* pEditor)
{
	m_nState = PROCSTATE_NOT_START;
	m_pEditor = pEditor;
	return;
}

void CProcedure::GotoState(int state)
{
	m_nState = state;
}

void CProcedure::Start()
{
	m_nState = PROCSTATE_READY;
	m_bSettingModified = FALSE;
	
	UpdateParams(FALSE);
}


void CProcedure::Finish()
{
	m_nExitCode = CMPEC_RESTART;

	if( m_bSettingModified )UpdateParams(TRUE);	
	GotoState(PROCSTATE_FINISHED);
}


void CProcedure::Abort()
{
	m_nExitCode = CMPEC_RESTART;

	if( m_bSettingModified )UpdateParams(TRUE);	
	GotoState(PROCSTATE_ABORTED);
}


void CProcedure::Back()
{

}

void CProcedure::BatchBack()
{

}

void CProcedure::SetSettingsModifyFlag()
{
	m_bSettingModified = TRUE;
}


int CProcedure::GetState()
{
	return m_nState;
}

int CProcedure::GetStateWithSonProcedure(CProcedure *pSon)
{
	if(pSon==NULL||m_nState!=PROCSTATE_READY)
	{
		return m_nState;	
	}	
	else
	{
		int nState = pSon->GetState();
		if (nState==PROCSTATE_PROCESSING||nState==PROCSTATE_FINISHED)
		{
			return PROCSTATE_PROCESSING;
		}
		else
			return PROCSTATE_READY;
	}
}

int  CProcedure::GetExitCode()
{
	return m_nExitCode;
}


CProcedure *CProcedure::GetActiveSonProc(int nMsgType)
{
	return NULL;
}



void CProcedure::OnSonEnd(CProcedure *pProc)
{
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawPointProcedure::CDrawPointProcedure()
{
	m_pGeoPoint = NULL;
}


CDrawPointProcedure::~CDrawPointProcedure()
{
}


void CDrawPointProcedure::PtClick(PT_3D &pt, int flag)
{
	GotoState(PROCSTATE_PROCESSING);

	if( !m_pGeoPoint )return;

	PT_3DEX expt(pt,penLine);	
	m_pGeoPoint->CreateShape(&expt,1);	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	Finish();
}

void CDrawPointProcedure::GetParams(CValueTable& tab)
{
	
}


void CDrawPointProcedure::SetParams(CValueTable& tab,BOOL bInit)
{
}

void CDrawPointProcedure::Start()
{
	GotoState(PROCSTATE_READY);
// 	m_pGeoPoint = new CGeoPoint;
	m_pEditor->CloseSelector();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawDirPointProcedure::CDrawDirPointProcedure()
{
	m_bDrawDir = FALSE;
	m_pGeoPoint = NULL;
}


CDrawDirPointProcedure::~CDrawDirPointProcedure()
{
//	if( m_pGeoPoint )delete m_pGeoPoint;
}


void CDrawDirPointProcedure::PtClick(PT_3D &pt, int flag)
{
	GotoState(PROCSTATE_PROCESSING);

	if( !m_pGeoPoint )return;
	if( !m_bDrawDir )
	{	
		PT_3DEX expt(pt,penLine);
		
		m_pGeoPoint->CreateShape(&expt,1);
		m_bDrawDir = TRUE;
	}
	else
	{
		m_pGeoPoint->SetCtrlPoint(0,pt);
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
}

void CDrawDirPointProcedure::PtReset(PT_3D &pt)
{
	if (!m_pGeoPoint || IsProcFinished(this)) return;
	Abort();
	
}

void CDrawDirPointProcedure::PtMove(PT_3D &pt)
{
	if( !m_pGeoPoint  )return;
	if( m_pGeoPoint->GetDataPointSum()<=0 )return;
	
	if( m_bDrawDir )
	{
		GrBuffer buf;
		m_pGeoPoint->SetCtrlPoint(0,pt);
		m_pGeoPoint->Draw(&buf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		CProcedure::PtMove(pt);
		return;
	}
	
	CProcedure::PtMove(pt);
}

void CDrawDirPointProcedure::GetParams(CValueTable& tab)
{
}


void CDrawDirPointProcedure::SetParams(CValueTable& tab,BOOL bInit)
{
}

void CDrawDirPointProcedure::Start()
{
	GotoState(PROCSTATE_READY);
	m_bDrawDir = FALSE;
//	m_pGeoPoint = new CGeoDirPoint;
	m_pEditor->CloseSelector();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawCurveProcedure::CDrawCurveProcedure()
{
	m_pGeoCurve = NULL;
	m_pGeoMoveCurve = NULL;
	m_bClosed = FALSE;
	m_nCurPenCode = penLine;
	m_bRectify = FALSE;

	m_fAutoCloseToler = 0;
	m_fMaxEndDis = 0;
	m_bDoSmooth = FALSE;
	m_bOpenIfSnapped = FALSE;
	m_nEndSnapped = 0;
	
	m_bFastDisplayCurve = FALSE;

	m_bChgPenCode = FALSE;

	m_bTrack = FALSE;
	m_trackHandle = 0;
	m_nLastTrackIndex = -1;
	m_TrackPart = 0;

	m_fCurNodeWid = 0;
	m_nCurNodeType = ptNone;

	m_bMultiPtBuildPt  = FALSE;
	m_fTolerBuildPt = 0.5;
	m_bSnap2D = TRUE;
}


CDrawCurveProcedure::~CDrawCurveProcedure()
{
	if (m_pGeoMoveCurve) delete m_pGeoMoveCurve;
//	if( m_pGeoCurve )delete m_pGeoCurve;
}

//曲线光滑的思路：用圆弧倒角代替折角
void SmoothLines(PT_3DEX *pts, int num, double toler, CArray<PT_3DEX, PT_3DEX>& arrPts)
{
	if( num<=2 || toler<=1e-10 )
	{
		for( int i=0; i<num; i++)
			arrPts.Add(pts[i]);
		return;
	}
	
	BOOL bClose = FALSE;
	if( fabs(pts[0].x-pts[num-1].x)<1e-10 && fabs(pts[0].y-pts[num-1].y)<1e-10 )
	{
		bClose = TRUE;
	}
	
	double ang,ang1,ang2,angt;
	double k,a,r,l;
	double rcosa,sina;
	double dis1,dis2,z0,z1;
	
	ang2 = GraphAPI::GGetAngle(pts[0].x,pts[0].y,pts[1].x,pts[1].y);
	dis2 = sqrt((pts[0].x-pts[1].x)*(pts[0].x-pts[1].x)+
		(pts[0].y-pts[1].y)*(pts[0].y-pts[1].y));
	
	if( !bClose )
	{
		arrPts.Add(pts[0]);
		
		for( int i=1; i<num-1; i++)
		{
			//计算夹角
			ang1 = ang2;
			ang2 = GraphAPI::GGetAngle(pts[i].x,pts[i].y,pts[i+1].x,pts[i+1].y);
			
			if( ang1>PI )ang1 -= PI;
			else ang1 += PI;
			
			ang  = ang2-ang1;
			if( ang<-PI )ang += 2*PI;
			if( ang>PI )ang -= 2*PI;
			ang *= 0.5;
			
			sina = sin(fabs(ang)); rcosa = 1/cos(fabs(ang));
			
			dis1 = dis2;
			dis2 = sqrt((pts[i+1].x-pts[i].x)*(pts[i+1].x-pts[i].x)+
				(pts[i+1].y-pts[i].y)*(pts[i+1].y-pts[i].y));
			
			k = (1-sina)*rcosa;
			a = (dis1<dis2?dis1:dis2)*0.5;
			
			//计算圆弧半径
			if( a*k>toler )a = toler/k;
			r = a*sina*rcosa;
			l = a*rcosa;

			if( a*k<toler*0.1 )
			{
				arrPts.Add(pts[i]);
				continue;
			}
			
			//计算圆心
			PT_3D cpt;
			cpt.x = pts[i].x + l*cos(ang+ang1);
			cpt.y = pts[i].y + l*sin(ang+ang1);
			cpt.z = pts[i].z;
			
			//计算圆弧的高差
			z0 = pts[i-1].z + (pts[i].z-pts[i-1].z)*(1.0-a/dis1);
			z1 = pts[i].z + (pts[i+1].z-pts[i].z)*(a/dis2);
			
			//计算圆弧的起始角度angt和绕过的弧度ang
			angt = 0;
			if( ang>0 )
			{
				angt = ang1-PI*0.5;
				ang = ang*2-PI;
			}
			else 
			{
				angt = ang1+PI*0.5;
				ang = PI+ang*2;
			}
			
			//绘制圆弧
			PT_3DEX cur;
			cur.pencode = pts[i].pencode;
			for( int j=0; j<=4; j++)
			{
				cur.x = cpt.x + r*cos(angt+ang*j/4);
				cur.y = cpt.y + r*sin(angt+ang*j/4);
				cur.z = z0 + (z1-z0)*j/4;
				
				arrPts.Add(cur);
			}
		}
		
		arrPts.Add(pts[num-1]);
	}
	else
	{	
		int i1;
		for( int i=1; i<num; i++)
		{
			i1 = (i+1)%num;
			if( i==num-1 )i1 = 1;
			//计算夹角
			ang1 = ang2;
			ang2 = GraphAPI::GGetAngle(pts[i].x,pts[i].y,pts[i1].x,pts[i1].y);
			
			if( ang1>PI )ang1 -= PI;
			else ang1 += PI;
			
			ang  = ang2-ang1;
			if( ang<-PI )ang += 2*PI;
			if( ang>PI )ang -= 2*PI;
			ang *= 0.5;
			
			sina = sin(fabs(ang)); rcosa = 1/cos(fabs(ang));
			
			dis1 = dis2;
			dis2 = sqrt((pts[i1].x-pts[i].x)*(pts[i1].x-pts[i].x)+
				(pts[i1].y-pts[i].y)*(pts[i1].y-pts[i].y));
			
			k = (1-sina)*rcosa;
			a = (dis1<dis2?dis1:dis2)*0.5;
			
			//计算圆弧半径
			if( a*k>toler )a = toler/k;
			r = a*sina*rcosa;
			l = a*rcosa;

			if( a*k<toler*0.1 )
			{
				arrPts.Add(pts[i]);
				continue;
			}
			
			//计算圆心
			PT_3D cpt;
			cpt.x = pts[i].x + l*cos(ang+ang1);
			cpt.y = pts[i].y + l*sin(ang+ang1);
			cpt.z = pts[i].z;
			
			//计算圆弧的高差
			z0 = pts[i-1].z + (pts[i].z-pts[i-1].z)*(1.0-a/dis1);
			z1 = pts[i].z + (pts[i1].z-pts[i].z)*(a/dis2);
			
			//计算圆弧的起始角度angt和绕过的弧度ang
			angt = 0;
			if( ang>0 )
			{
				angt = ang1-PI*0.5;
				ang = ang*2-PI;
			}
			else 
			{
				angt = ang1+PI*0.5;
				ang = PI+ang*2;
			}
			
			//绘制圆弧
			PT_3DEX cur;
			cur.pencode = pts[i].pencode;
			for( int j=0; j<=4; j++)
			{
				cur.x = cpt.x + r*cos(angt+ang*j/4);
				cur.y = cpt.y + r*sin(angt+ang*j/4);
				cur.z = z0 + (z1-z0)*j/4;
				
				arrPts.Add(cur);
			}
		}	
		arrPts.Add(arrPts[0]);
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts2;
	num = arrPts.GetSize();
	pts = arrPts.GetData();
	
	PT_3D pt3d;
	PT_3DEX expt;

	CStreamCompress2_PT_3D comp;
	comp.BeginCompress(toler*0.1);
	
	for( int i=0; i<num; i++)
	{
		expt = arrPts[i];
		COPY_3DPT(pt3d,expt);
		comp.AddStreamPt(pt3d);
	}
	
	comp.EndCompress();
	{
		PT_3D *tpts;
		int ti, tnpt;
		comp.GetPts(tpts,tnpt);
		for( ti=0; ti<tnpt; ti++)
		{
			COPY_3DPT(expt,tpts[ti]);
			arrPts2.Add(expt);
		}
	}
	
	arrPts.Copy(arrPts2);
}

void CDrawCurveProcedure::PutStreamPts()
{
	if( m_nCurPenCode==penStream && m_pGeoCurve!=NULL )
	{
		m_compress.EndCompress();
		
		PT_3DEX *pts = NULL;
		int i, npt = 0;
		m_compress.GetPts(pts,npt);
		
		if( npt>1 )
		{
// 			CArray<PT_3DEX,PT_3DEX> pts0;
// 			m_pGeoCurve->GetShape(pts0);

			int nsum = m_arrPts.GetSize();
			for( i=0; i<npt; i++)
			{
				if( nsum>0 && i==0 );
				else m_arrPts.Add(pts[i]);
			}
		//_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		}
		
		m_compress.BeginCompress();
	}
}

void CDrawCurveProcedure::ChangePencod(int pencode)
{
	if (pencode == m_nCurPenCode)  return;

	m_bChgPenCode = TRUE;

	PutStreamPts();	

	int num = m_arrPts.GetSize();

	// 连续换cd
	if (num == 1 || (num == 2 && m_nCurPenCode == penArc))
	{
		if (num == 1)
		{
			// 换成圆弧
			if (pencode == penArc)
			{
				if (m_arrAllPts.GetSize() > 1)
				{
					m_arrPts.InsertAt(0,m_arrAllPts.GetAt(m_arrAllPts.GetSize()-2));
				}
			}
			else
			{
				m_arrPts[0].pencode = pencode;
			}
		}
		// 圆弧换成其它
		else
		{
			m_arrPts.RemoveAt(0);
			m_arrPts[0].pencode = pencode;
		}
		
	}
	else if (num > 1)
	{
		long color = m_pGeoCurve->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}

		int size = m_arrAllPts.GetSize();
		int index = size>0?(--size):0;		
		m_arrIndex.Add(index);

		if (size > 0)
		{
			// 圆弧
			if (m_nCurPenCode == penArc)
			{
				m_arrPts.RemoveAt(0,2);
			}
			else
			{
				m_arrPts.RemoveAt(0);
			}
		}
		
		m_arrAllPts.Append(m_arrPts);

		// 圆弧
		if (pencode == penArc)
		{
			m_arrPts.RemoveAt(0,m_arrPts.GetSize()-2);
			//m_arrPts[1].pencode = pencode;
		}
		else
		{
			m_arrPts.RemoveAt(0,m_arrPts.GetSize()-1);
			m_arrPts[0].pencode = pencode;
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pGeoCurve->CreateShape(m_arrAllPts.GetData(),m_arrAllPts.GetSize());
		GrBuffer buf;
		m_pGeoCurve->Draw(&buf);
		buf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
		m_pEditor->RefreshView();
		
	}

	if(pencode == penStream && m_arrPts.GetSize() >= 1)
	{
		m_compress.BeginCompress();
		m_compress.AddStreamPt(m_arrPts[0]);
	}

	m_nCurPenCode = pencode;
}

void CDrawCurveProcedure::TempLine()
{
	if (m_nCurPenCode != penStream)
	{
		m_pEditor->UpdateDrag(ud_ClearDrag);
	}

	long color = m_pGeoCurve->GetColor();
	if (color == -1)
	{
		color = m_layCol;
	}
	
	GrBuffer cbuf,vbuf;
	const CShapeLine *pSL = m_pGeoCurve->GetShape();
	if (m_nCurPenCode == penLine || (m_bFastDisplayCurve && m_nCurPenCode==penSpline))
	{
		CArray<PT_3DEX,PT_3DEX> arrtmp;
		arrtmp.Append(m_arrPts);

		if( m_bFastDisplayCurve )
		{
			for (int i=0; i<arrtmp.GetSize(); i++)
			{
				PT_3DEX &t = arrtmp.ElementAt(i);
				t.pencode = penLine;
			}
			m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
		}

		if (m_bClosed && arrtmp.GetSize()>2 && !m_bChgPenCode)
		{
			arrtmp.Add(arrtmp.GetAt(0));
			m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());

			pSL->GetConstGrBuffer(&cbuf,true);

			arrtmp.RemoveAt(m_arrPts.GetSize()-1);
			m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
			
		}
		else
		{
			pSL->GetConstGrBuffer(&cbuf);
		}

		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);

		if( m_bFastDisplayCurve )
		{
			m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		}
	
	}
	else if (m_nCurPenCode == penSpline || m_nCurPenCode == penArc )
	{
		GrBuffer *pvbuf = NULL;
		if (m_bClosed && !m_bChgPenCode)
		{
			m_arrPts.Add(m_arrPts.GetAt(0));
			m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

			pSL->GetConstGrBuffer(&cbuf,true);

			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			
		}
		else
		{
			pSL->GetConstGrBuffer(&cbuf);
		}

		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);

	}
	else if(m_nCurPenCode == penStream)
	{
/*		m_compress.BeginCompress();
		m_compress.AddStreamPt(pt);
		
		if( m_arrPts.GetSize()>=2 )
		{
			PT_3DEX t0;
			t0 = m_arrPts.GetAt(m_arrPts.GetSize()-2);
			PT_3D tpt;
			COPY_3DPT(tpt,t0);
			GrBuffer addbuf;
			addbuf.BeginLineString(m_pGeoCurve->GetColor(),0);		
			addbuf.MoveTo(&tpt);
			addbuf.LineTo(&pt);
			addbuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
		}
			
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
*/
	}
}

void CDrawCurveProcedure::ReverseLine()
{
	PutStreamPts();

	if (m_arrPts.GetSize() >= 2)
	{		
		//数据点列方向
		int num = m_arrPts.GetSize();
		PT_3DEX expt1,expt2;
		for( int i=0; i<num/2; i++)
		{
			expt1 = m_arrPts[i];
			m_arrPts[i] = m_arrPts[num-1-i];
			m_arrPts[num-1-i] = expt1;
		}

		m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

		TempLine();
 		
 		PT_3D pt3d; 
		COPY_3DPT(pt3d,m_arrPts[num-1]);
		if( m_nCurPenCode==penStream )
		{
			m_compress.BeginCompress();
//			COPY_3DPT(pt3d,expt1);
			m_compress.AddStreamPt(m_arrPts[num-2]);
// 			COPY_3DPT(pt3d,expt2);
 			m_compress.AddStreamPt(m_arrPts[num-1]);
		}
		
		
		m_pEditor->SetCrossPos(pt3d);
		
		PtMove(pt3d);
		return;
	}
}

//获取的是二维点，计算长度是不应该考虑z值
template<class T>
static void GetPartLineOf2Pt(T pts[],int nCount,const T &pt0,int index0,const T &pt1,int index1,CArray<T,T> &arr)
{
	if (index0 < 0 || index0>nCount-1 || index1 < 0 || index1>nCount-1 ||index0 == index1)  return;

	double len=0;
	for (int i=0; i<nCount-1; i++)
	{
		len += sqrt( (pts[i+1].x-pts[i].x)*(pts[i+1].x-pts[i].x)+(pts[i+1].y-pts[i].y)*(pts[i+1].y-pts[i].y) );
	}

	BOOL bClose = ( _FABS(pts[0].x-pts[nCount-1].x)+_FABS(pts[0].y-pts[nCount-1].y) < 1e-6);
	
	// 非闭合直接提取中间段，闭合则提取中间段与首尾两段长度较小部分
	if (!bClose)
	{
		// 取中间段
		if (index0 < index1)
		{ 
			for (i=index0+1; i<=index1; i++)
			{
				arr.Add(pts[i]);
			}
		}
		else
		{
			for (i=index0; i>index1; i--)
			{
				arr.Add(pts[i]);
			}
		}
	}
	else
	{
		// 中间段的长度
		double fMidLen = 0;
		if (index0 < index1)
		{ 
			fMidLen += sqrt( (pts[index0+1].x-pt0.x)*(pts[index0+1].x-pt0.x)+(pts[index0+1].y-pt0.y)*(pts[index0+1].y-pt0.y) );
			
			for (i=index0+1; i<index1; i++)
			{
				fMidLen += sqrt( (pts[i+1].x-pts[i].x)*(pts[i+1].x-pts[i].x)+(pts[i+1].y-pts[i].y)*(pts[i+1].y-pts[i].y) );
			}
			
			fMidLen += sqrt( (pts[index1].x-pt1.x)*(pts[index1].x-pt1.x)+(pts[index1].y-pt1.y)*(pts[index1].y-pt1.y) );
			
		}
		else
		{
			fMidLen += sqrt( (pts[index0].x-pt0.x)*(pts[index0].x-pt0.x)+(pts[index0].y-pt0.y)*(pts[index0].y-pt0.y) );
			
			for (i=index1+1; i<index0; i++)
			{
				fMidLen += sqrt( (pts[i+1].x-pts[i].x)*(pts[i+1].x-pts[i].x)+(pts[i+1].y-pts[i].y)*(pts[i+1].y-pts[i].y) );
			}
			
			fMidLen += sqrt( (pts[index1+1].x-pt1.x)*(pts[index1+1].x-pt1.x)+(pts[index1+1].y-pt1.y)*(pts[index1+1].y-pt1.y) );
		}
		
		
		// 提取中间段
		if (fMidLen <= len/2)
		{
			if (index0 < index1)
			{ 
				for (i=index0+1; i<=index1; i++)
				{
					arr.Add(pts[i]);
				}
			}
			else
			{
				for (i=index0; i>index1; i--)
				{
					arr.Add(pts[i]);
				}
			}
			
		}
		else
		{
			if (index0 < index1)
			{
				// 首段
				for (i=index0; i>=0; i--)
				{
					arr.Add(pts[i]);
				}
				
				// 尾段
				for (i=nCount-2; i>index1; i--)
				{
					arr.Add(pts[i]);
				}
			}
			else
			{
				// 尾段
				for (i=index0+1; i<nCount; i++)
				{
					arr.Add(pts[i]);
				}
				
				// 首段
				for (i=1; i<=index1; i++)
				{
					arr.Add(pts[i]);
				}
			}	
		}
	}
}

//点是否在线串上
static int IsPtInCurve(PT_3DEX pt, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	for(int i=0; i<arrPts.GetSize()-1; i++)
	{
		if(GraphAPI::GIsPtInLine(arrPts[i], arrPts[i+1], pt))
		{
			return i;
		}
	}
	return -1;
}

//取出线串上最靠近m_curPt的独立部分 返回值为那一部分，对线，面，平行线，双线和复杂面都有效
static int GetCurvePartPts(PT_3D m_curPt, CGeometry* pGeo, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	const CShapeLine * pShpLine = pGeo->GetShape();
	if(!pShpLine) return -1;
	pShpLine->GetPts(pts);
	PT_3DEX pt3dex;
	COPY_3DPT(pt3dex, m_curPt);
	arrPts.RemoveAll();

	int geoType = pGeo->GetClassType();
	if(geoType == CLS_GEOPARALLEL)
	{	
		if(-1==IsPtInCurve(pt3dex, pts))
		{
			CGeometry *pObj1=NULL, *pObj2 = NULL;
			if( ((CGeoParallel*)pGeo)->Separate(pObj1, pObj2))
			{
				if(pObj1 && pObj2)
				{
					arrPts.RemoveAll();
					pObj2->GetShape()->GetPts(arrPts);
					delete pObj1;
					delete pObj2;
					return 1;//副线
				}
			}
		}
		else
		{
			arrPts.Copy(pts);
			return 0;
		}
	}
	else
	{
		int part=0;
		CArray<PT_3DEX,PT_3DEX> pts1;
		for(int i=0; i<pts.GetSize(); i++)
		{
			if(pts[i].pencode==penMove)
			{
				if(IsPtInCurve(pt3dex, pts1)>=0)
				{
					arrPts.Copy(pts1);
					return part;
				}
				pts1.RemoveAll();
				part++;
			}
			
			pts1.Add(pts[i]);
		}

		//最后一段
		if(IsPtInCurve(pt3dex, pts1)>=0)
		{
			arrPts.Copy(pts1);
			return part;
		}
	}

	return -1;//主线
}

//重载，取出第curPart部分的点
static int GetCurvePartPts(int curPart, CGeometry* pGeo, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	pGeo->GetShape()->GetPts(pts);
	int geoType = pGeo->GetClassType();
	arrPts.RemoveAll();

	if(geoType == CLS_GEOPARALLEL)
	{
		if(curPart==0)
		{
			arrPts.Copy(pts);
			return 0;
		}
		else if(curPart==1)
		{
			CGeometry *pObj1=NULL, *pObj2 = NULL;
			if( ((CGeoParallel*)pGeo)->Separate(pObj1, pObj2))
			{
				if(pObj1 && pObj2)
				{
					arrPts.RemoveAll();
					pObj2->GetShape()->GetPts(arrPts);
					delete pObj1;
					delete pObj2;
					return 1;//副线
				}
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		arrPts.RemoveAll();
		int part=0;
		CArray<PT_3DEX,PT_3DEX> pts1;
		for(int i=0; i<pts.GetSize(); i++)
		{
			if(pts[i].pencode==penMove)
			{
				part++;
			}
			
			if(part==curPart)
			{
				pts1.Add(pts[i]);
			}
		}
		arrPts.Copy(pts1);
		return part;
	}
	
	return -1;
}

void CDrawCurveProcedure::TrackLine()
{
	GotoState(PROCSTATE_PROCESSING);

	PT_3DEX curPt(m_curPt,penLine), retPt(m_curPt,penLine);
	if(m_bTrack)
	{
		m_trackPts.Add(retPt);
		int sum = GraphAPI::GKickoffSamePoints(m_trackPts.GetData(),m_trackPts.GetSize());
		m_trackPts.SetSize(sum);
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pGeoCurve->GetColor(),0);
		cbuf.Lines(m_trackPts.GetData(), sum, sizeof(PT_3DEX));
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);

		for(int i=0; i<m_trackPts.GetSize(); i++)
		{
			m_trackPts[i].pencode = penLine;
		}
		
		m_arrPts.Append(m_trackPts);
		m_bTrack = FALSE;
		m_trackPts.RemoveAll();
		return;
	}

	PT_3D cltpt;	
	(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_curPt,&cltpt);
	m_pEditor->GetDataQuery()->FindNearestObject(cltpt,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS(),
				m_pEditor->GetCoordWnd().m_pSearchCS);
	int num;
	const CPFeature *ftrs = (m_pEditor->GetDataQuery())->GetFoundHandles(num);
	if(num<=0) return;

	CFeature *pFtr = NULL;
	for(int k=0; k<num; k++)
	{
		CGeometry *pGeo = ftrs[k]->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			continue;
		}
		pFtr = ftrs[k];
		break;
	}
	if(pFtr==NULL)  return;
	CGeometry *pGeo = pFtr->GetGeometry();
	int geoType = pGeo->GetClassType();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int trackPart = GetCurvePartPts(m_curPt, pGeo, arrPts);

	// 线串化
	for (int i=0; i<arrPts.GetSize(); i++)
	{
		arrPts[i].pencode = penLine;
	}

	if (arrPts.GetSize() < 2) return;

	if (geoType == CLS_GEOSURFACE)
	{
		//arrPts.Add(arrPts[0]);
	}

	int index = -1;
	GraphAPI::GGetNearstDisPToPointSet2D(arrPts.GetData(),arrPts.GetSize(),curPt,retPt,&index);
	retPt.z = m_curPt.z;

	m_bTrack = TRUE;
	m_trackHandle = FtrToHandle(pFtr);
	m_nLastTrackIndex = index;
	m_nLastTrackPt = retPt;
	m_TrackPart = trackPart;

	retPt.pencode = arrPts[index].pencode;
	m_trackPts.Add(retPt);
	int nPt = m_arrPts.GetSize();
	if (m_arrPts.GetSize() > 0)
	{
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pGeoCurve->GetColor(),0);
		cbuf.MoveTo(&m_arrPts[nPt-1]);
		cbuf.LineTo(&m_trackPts[0]);
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
	}
}

void CDrawCurveProcedure::StartBuildPt()
{
	m_bMultiPtBuildPt = TRUE;
	m_arrBuildPt.RemoveAll();
}

void CDrawCurveProcedure::EndBuildPt()
{
	if (!m_bMultiPtBuildPt) return;
	
	m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);	
	
	m_bMultiPtBuildPt = FALSE;
	
	int size = m_arrBuildPt.GetSize();
	if (size <= 0) return;
	
	PT_3DEX pt;
	for (int i=0; i<size; i++)
	{
		pt.x += m_arrBuildPt[i].x;
		pt.y += m_arrBuildPt[i].y;
		pt.z += m_arrBuildPt[i].z;
	}
	
	pt.x /= size;
	pt.y /= size;
	pt.z /= size;
	pt.pencode = m_nCurPenCode;
	
	PtClick(pt,0);
	
	m_arrBuildPt.RemoveAll();
	
	m_pEditor->UpdateDrag(ud_UpdateVariantDrag);
	m_pEditor->UpdateDrag(ud_UpdateConstDrag);
}

void CDrawCurveProcedure::PtClick(PT_3D &pt, int flag)
{
	if(m_bTrack)
	{
		TrackLine();
		return;
	}

	if (m_bMultiPtBuildPt)
	{
		PT_3DEX expt(pt,m_nCurPenCode);
		int size = m_arrBuildPt.GetSize();
		if (size > 0 && GraphAPI::GGet3DDisOf2P(m_arrBuildPt[0],expt) > m_fTolerBuildPt)
		{
			CString str = StrFromLocalResID(IDS_CMDTIP_DISFAR);
			m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)str);
		}
		else
		{
			m_arrBuildPt.Add(expt);
		}
		
		return;
	}

	m_curPt = pt;

	if(!m_pGeoCurve)
		return;
	
	GotoState(PROCSTATE_PROCESSING);

 	PutStreamPts();

	PT_3DEX expt(pt,m_nCurPenCode,m_fCurNodeWid,m_nCurNodeType);
	m_arrPts.Add(expt);

	m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

	long color = m_pGeoCurve->GetColor();
	if (color == -1)
	{
		color = m_layCol;
	}
	
	GrBuffer cbuf,vbuf;
	const CShapeLine *pSL = m_pGeoCurve->GetShape();
	if (m_nCurPenCode == penLine || (m_bFastDisplayCurve && m_nCurPenCode==penSpline))
	{
		CArray<PT_3DEX,PT_3DEX> arrtmp;
		arrtmp.Append(m_arrPts);

		if( m_bFastDisplayCurve )
		{
			for (int i=0; i<arrtmp.GetSize(); i++)
			{
				PT_3DEX &t = arrtmp.ElementAt(i);
				t.pencode = penLine;
			}
			m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
		}

		if (m_bClosed && arrtmp.GetSize()>2 && !m_bChgPenCode)
		{
			arrtmp.Add(arrtmp.GetAt(0));
			m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
			pSL->GetAddConstGrBuffer(&cbuf,true);
			pSL->GetVariantGrBuffer(&vbuf,true,arrtmp.GetSize()-1);
			arrtmp.RemoveAt(arrtmp.GetSize()-1);
			m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
			
		}
		else
		{
			pSL->GetAddConstGrBuffer(&cbuf);
		}
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);

		if( m_bFastDisplayCurve )
		{
			m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		}
	
	}
	else if (m_nCurPenCode == penSpline || m_nCurPenCode == penArc || m_nCurPenCode == pen3PArc)
	{
		GrBuffer *pvbuf = NULL;
		if (m_bClosed && !m_bChgPenCode)
		{
			m_arrPts.Add(m_arrPts.GetAt(0));
			m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

			pSL->GetAddConstGrBuffer(&cbuf,true);

			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

			if (m_arrPts.GetSize() > 2)
			{
				m_arrPts.Add(m_arrPts.GetAt(0));
				m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

				pSL->GetVariantGrBuffer(&vbuf,TRUE,m_arrPts.GetSize()-1);

				m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
				m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
				
			}
			else
				pSL->GetVariantGrBuffer(&vbuf);

			pvbuf = &vbuf;

			
		}
		else
		{
			pSL->GetAddConstGrBuffer(&cbuf);

			// 只有样条有临时线
			if (m_nCurPenCode == penSpline)
			{
				m_arrPts.Add(expt);
				m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
				pSL->GetVariantGrBuffer(&vbuf);
				m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
				m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
				pvbuf = &vbuf;
			}	

		}

		m_pEditor->UpdateDrag(ud_SetVariantDrag,pvbuf);
		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
	}
	else if(m_nCurPenCode == penStream)
	{
		m_compress.BeginCompress();
		m_compress.AddStreamPt(pt);
		
		if( m_arrPts.GetSize()>=2 )
		{
			PT_3DEX t0;
			t0 = m_arrPts.GetAt(m_arrPts.GetSize()-2);
			PT_3D tpt;
			COPY_3DPT(tpt,t0);
			GrBuffer addbuf;
			addbuf.BeginLineString(m_pGeoCurve->GetColor(),0);		
			addbuf.MoveTo(&tpt);
			addbuf.LineTo(&pt);
			addbuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
		}
			
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);

	}
	
	return;
}

//返回距离retpt最近的交点
static bool GetCurveIntersectCurve(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2, PT_3DEX& pt, double *a, double *b)
{
	if (!pts1 || num1 < 2 || !pts2 || num2 < 2) return false;

	double t1, t2;
	PT_3DEX inpt, temp;
	COPY_3DPT(inpt, pt);
	double dis1 = 99999999.9;
	bool bIntersect = false;
	for( int i=0; i<num1-1; i++ )
	{
		if( (pts1+i+1)->pencode == penMove) continue;
		for( int j=0; j<num2-1; j++ )
		{
			if( (pts2+j+1)->pencode == penMove) continue;
			if (GraphAPI::GGetLineIntersectLineSeg(pts1[i].x,pts1[i].y,pts1[i+1].x,pts1[i+1].y,
				pts2[j].x,pts2[j].y,pts2[j+1].x,pts2[j+1].y,&(temp.x),&(temp.y),&t1,&t2))
			{
				bIntersect = true;
				double dis2 = GraphAPI::GGet2DDisOf2P(inpt, temp);
				if(dis2<dis1)
				{
					*a = i+t1;
					*b = j+t2;
					COPY_3DPT(pt, temp);
					dis1 = dis2;
				}
			}
		}
	}
	
	return bIntersect;
}

void CDrawCurveProcedure::PtMove(PT_3D &pt)
{
	m_curPt = pt;
	
	//追踪
	if (m_bTrack)
	{
		PT_3D cltpt;	
		(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_curPt,&cltpt);
		m_pEditor->GetDataQuery()->FindNearestObject(cltpt,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS(),
			m_pEditor->GetCoordWnd().m_pSearchCS);

		int num;
		const CPFeature *ftrs = (m_pEditor->GetDataQuery())->GetFoundHandles(num);

		BOOL bSameFtr = FALSE;
		int m;
		for(m=0; m<num; m++)
		{
			if(ftrs[m]==HandleToFtr(m_trackHandle))
			{
				bSameFtr = TRUE;
				break;
			}
		}
		
		PT_3DEX curPt(m_curPt,penLine), retPt(m_curPt,penLine);

		CGeometry *pGeo = HandleToFtr(m_trackHandle)->GetGeometry();
		CArray<PT_3DEX, PT_3DEX> pts1, pts2;
		if (bSameFtr)
		{
			int trackPart = GetCurvePartPts(m_curPt, pGeo, pts1);
			if(trackPart!=m_TrackPart)
			{
				bSameFtr = FALSE;
			}
			if(trackPart<0) goto trackend;
		}

		if(num<=0) goto trackend;
		
		if (bSameFtr)
		{
			int index = -1;
			GraphAPI::GGetNearstDisPToPointSet2D(pts1.GetData(),pts1.GetSize(),curPt,retPt,&index);
			curPt.z = retPt.z = m_curPt.z;

			PT_3DEX pt0 = m_trackPts[m_trackPts.GetSize()-1];
			if(GraphAPI::GIsEqual2DPoint(&curPt, &pt0))
			{
				;//如果咬合到最后一个点，则不处理
			}
			else if (index >= 0)
			{
				int pos = IsPtInCurve(curPt, m_trackPts);
				BOOL bSamePt = GraphAPI::GIsEqual2DPoint(&curPt, &m_trackPts[0]);
				if(pos>=0 && !bSamePt)
				{
					int cutnum = m_trackPts.GetSize()-pos-1;
					m_trackPts.RemoveAt(pos+1, cutnum);//回退
				}
				else
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					GetPartLineOf2Pt(pts1.GetData(),pts1.GetSize(),m_nLastTrackPt,m_nLastTrackIndex,retPt,index,arr);
					if(m_bSnap2D)//3D时保持高程为地物高程 2D则为测标高程
					{
						for (int i=0; i<arr.GetSize(); i++)
						{
							arr[i].z = m_curPt.z;
						}
					}
					if(arr.GetSize()>1 && IsPtInCurve(arr[1], m_trackPts)>=0)
					{
						goto trackend;//阻止调换方向，若需要换方向必须先回退到方向交叉点
					}
					m_trackPts.Append(arr);
				}
				m_nLastTrackIndex = index;
				m_nLastTrackPt = retPt;
			}
		}
		else//转移到另一个地物,仅补全m_nLastTrackPt到交点的部分，若有多个交点，取鼠标最近的
		{
			GetCurvePartPts(m_TrackPart, pGeo, pts1);
			for(int k=0; k<num; k++)
			{
				pGeo = ftrs[k]->GetGeometry();
				if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}

				int trackPart = GetCurvePartPts(m_curPt, pGeo, pts2);
				if(trackPart<0)
				{
					continue;
				}
				
				double t1, t2;
				PT_3DEX retpt1;//交点
				COPY_3DPT(retpt1, pt);
				retpt1.pencode = penLine;
				if(!GetCurveIntersectCurve(pts1.GetData(), pts1.GetSize(), pts2.GetData(), pts2.GetSize(), retpt1, &t1, &t2))
				{
					continue;
				}

				int pos = IsPtInCurve(retpt1, m_trackPts);
				BOOL bSamePt = GraphAPI::GIsEqual2DPoint(&retpt1, &m_trackPts[0]);
				
				if(pos>0 && !bSamePt)
				{
					int cutnum = m_trackPts.GetSize()-pos-1;
					m_trackPts.RemoveAt(pos+1, cutnum);
				}
				else
				{
					CArray<PT_3DEX, PT_3DEX> arr;
					GetPartLineOf2Pt(pts1.GetData(),pts1.GetSize(),m_nLastTrackPt,m_nLastTrackIndex,retpt1,(int)t1,arr);//首尾点重复
					if(m_bSnap2D)
					{
						for (int i=0; i<arr.GetSize(); i++)
						{
							arr[i].z = m_curPt.z;
						}
					}
					if(arr.GetSize()>1 && IsPtInCurve(arr[1], m_trackPts)>=0)
					{
						continue;//阻止调换方向，若需要换方向必须先回退到方向交叉点
					}
					m_trackPts.Append(arr);
				}
				m_trackPts.Add(retpt1);
				m_nLastTrackIndex = (int)(t2);
				m_nLastTrackPt = retpt1;
				m_trackHandle = FtrToHandle(ftrs[k]);
				m_TrackPart  = trackPart;
				break;
			}
		}
trackend:
		GrBuffer trackbuf;
		CArray<PT_3DEX,PT_3DEX> arr;	
		arr.Append(m_trackPts);
		arr.Add(retPt);		
		
		trackbuf.BeginLineString(RGB(255,0,0)/*m_pGeoCurve->GetColor()*/,0);
		for (int i=0; i<arr.GetSize(); i++)
		{
			if (i == 0)
			{
				trackbuf.MoveTo(&arr[i]);
			}
			else
			{
				trackbuf.LineTo(&arr[i]);
			}
		}
		if(m_bClosed)
		{
			CArray<PT_3DEX,PT_3DEX> allPts;
			allPts.Copy(m_arrPts);
			allPts.Append(m_arrAllPts);
			allPts.Append(m_trackPts);
			if(allPts.GetSize()>0)
			{
				trackbuf.MoveTo(&allPts[0]);
				trackbuf.LineTo(&curPt);
			}
		}
		trackbuf.End();

		m_pEditor->UpdateDrag(ud_SetVariantDrag,&trackbuf);
		return;
	}//追踪

	if (m_arrPts.GetSize()<=0||!m_pGeoCurve)
	{
		if (m_bMultiPtBuildPt)
		{
			GrBuffer vbuf;
			for (int i=0; i<m_arrBuildPt.GetSize(); i++)
			{
				vbuf.Point(m_pGeoCurve->GetColor(),&m_arrBuildPt[i],1,1);
			}
			
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			
		}
		return;
	}

	if( m_nCurPenCode==penStream )
	{
		long color = m_pGeoCurve->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}

		GrBuffer addbuf;
		addbuf.BeginLineString(color,0);
		
		if( 1 )
		{
			PT_3D tpt;
			m_compress.GetCurCompPt(tpt);
			m_compress.AddStreamPt(pt);
			
			addbuf.MoveTo(&tpt);
			addbuf.LineTo(&pt);
			addbuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
		}
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);

		//自动闭合处理
		if( m_arrPts.GetSize() > 0 )
		{
			PT_3DEX t0;
			t0 = m_arrPts.GetAt(0);//m_pGeoCurve->GetDataPoint(0);
			double dis = sqrt((pt.x-t0.x)*(pt.x-t0.x)+(pt.y-t0.y)*(pt.y-t0.y));
			if( dis>m_fMaxEndDis )
				m_fMaxEndDis = dis;
			
			if( m_fMaxEndDis>m_compress.GetLimit() && m_fMaxEndDis>m_fAutoCloseToler &&
				dis<m_fAutoCloseToler )
			{
				
				PutStreamPts();
				
				if( m_arrPts.GetSize() >= 3 )
				{
					PT_3DEX t;
					t = m_arrPts.GetAt(0);
					m_arrPts.Add(t);
// 					CArray<PT_3DEX,PT_3DEX> arr;
// 					m_pGeoCurve->GetShape(arr);
// 					arr.Add(t);
// 					m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
					
				}

				if (m_arrPts.GetSize() <= 1)
				{
					Abort();
				}
				else
					PtReset(pt);					
				
				return;
			}
		}

 		return ;
	}
	PT_3DEX expt(pt,m_nCurPenCode,m_fCurNodeWid,m_nCurNodeType);
	m_arrPts.Add(expt);

	if (m_bClosed && m_arrPts.GetSize()>2 && !m_bChgPenCode)
	{
		m_arrPts.Add(m_arrPts[0]);
	}

	BOOL bCreate = FALSE;
	BOOL bUseMoveCurve = FALSE;
	if( m_nCurPenCode==penSpline )
	{
		if (m_bFastDisplayCurve)
		{
			CArray<PT_3DEX,PT_3DEX> arrtmp;
			arrtmp.Append(m_arrPts);
			for (int i=0; i<arrtmp.GetSize(); i++)
			{
				PT_3DEX &t = arrtmp.ElementAt(i);
				t.pencode = penLine;
			}
			bCreate = m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
		}
		else
		{
			int size = m_arrPts.GetSize();
			if (0 && !m_bClosed && size > 6)
			{				
				if (m_pGeoMoveCurve == NULL)
				{
					m_pGeoMoveCurve = new CGeoCurve;
					if (m_pGeoMoveCurve == NULL)
						return;
				}
				
				bCreate = m_pGeoMoveCurve->CreateShape(m_arrPts.GetData()+size-6,6);
				
				bUseMoveCurve = TRUE;
				
			}
			else
			{
				bCreate = m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			}
			
		}
		
	}
	else
		bCreate = m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

	if (bCreate)
	{
		GrBuffer vbuf;
		vbuf.SetAllColor(m_pGeoCurve->GetColor());
		const CShapeLine *pSL = m_pGeoCurve->GetShape();
		if (bUseMoveCurve)
		{
			pSL = m_pGeoMoveCurve->GetShape();
		}
		if(m_bClosed && !m_bChgPenCode)
			pSL->GetVariantGrBuffer(&vbuf,true,m_arrPts.GetSize()-2);
		else
			pSL->GetVariantGrBuffer(&vbuf);
		//m_pGeoCurve->Draw(&vbuf);

		

		if (m_bMultiPtBuildPt)
		{
			for (int i=0; i<m_arrBuildPt.GetSize(); i++)
			{
				vbuf.Point(m_pGeoCurve->GetColor(),&m_arrBuildPt[i],1,1);
			}
			
		}

		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);

		if( m_bFastDisplayCurve && m_nCurPenCode==penSpline )
		{
			m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		}
	}
	if (m_bClosed && m_arrPts.GetSize()>2  && !m_bChgPenCode)m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
	m_arrPts.RemoveAt(m_arrPts.GetSize()-1);

	m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

	if (!bUseMoveCurve)
		m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
}


void CDrawCurveProcedure::Back()
{
	if (m_pGeoCurve)
	{
		PutStreamPts();

		if( m_arrPts.GetSize() <= 1 || (m_bChgPenCode && m_nCurPenCode == penArc && m_arrPts.GetSize() <= 2))
		{
			if (m_arrAllPts.GetSize() == 0)
			{
				Abort();
				return;
			}
			else
			{
				m_arrPts.RemoveAll();

				int pos = m_arrIndex.GetAt(m_arrIndex.GetSize()-1);
				for (int i=pos; i<m_arrAllPts.GetSize(); i++)
				{
					m_arrPts.InsertAt(m_arrPts.GetSize(),m_arrAllPts.GetAt(i));
				}

				m_arrIndex.RemoveAt(m_arrIndex.GetSize()-1);
				if (m_arrIndex.GetSize() == 0)
				{
					m_arrAllPts.RemoveAll();
				}
				else
					m_arrAllPts.RemoveAt(pos+1,m_arrAllPts.GetSize()-pos-1);

// 				if (m_arrPts.GetSize() >= 2)
// 				{
// 					m_arrPts[m_arrPts.GetSize()-1].pencode = m_arrPts[m_arrPts.GetSize()-2].pencode;
// 				}

				m_nCurPenCode = m_arrPts[m_arrPts.GetSize()-1].pencode;

				if (m_nCurPenCode == penArc && m_arrAllPts.GetSize() > 1)
				{
					m_arrPts.InsertAt(0,m_arrAllPts.GetAt(m_arrAllPts.GetSize()-2));
				}
				
			}
			
		}

		long color = m_pGeoCurve->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pGeoCurve->CreateShape(m_arrAllPts.GetData(),m_arrAllPts.GetSize());
		GrBuffer precbuf;
		m_pGeoCurve->Draw(&precbuf);
		precbuf.SetAllColor(color);


		if( m_nCurPenCode==penStream )
		{
			//PutStreamPts();
			if( m_arrPts.GetSize()<=1 )
			{
				Abort();
				return;
			}
			
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			
			PT_3DEX t0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);				
			PT_3D t1;
			COPY_3DPT(t1,t0);
			m_compress.BeginCompress();
			m_compress.AddStreamPt(t1);
			
			GrBuffer buf;
			buf.BeginLineString(color,0);
			buf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
			buf.End();
			
			buf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->SetCrossPos(t1);
			m_pEditor->RefreshView();
			
			
		}
		else 
		{
			if(m_arrPts.GetSize() < 2)
			{
				m_pGeoCurve->CreateShape(&PT_3DEX(),1);
				Abort();
				return;
			}
			PT_3DEX pt0;
			PT_3D pt1;
			CArray<PT_3DEX,PT_3DEX> pts;
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			pts.Append(m_arrPts);
			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			COPY_3DPT(pt1,pt0);	
			
			GrBuffer cbuf,vbuf;
			if (m_bClosed && m_arrPts.GetSize()>2 && !m_bChgPenCode)
			{
				pts.Add(pts[0]);
			}
			m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
			
			m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf,m_bClosed && !m_bChgPenCode);
			cbuf.SetAllColor(color);
			if(m_bClosed && !m_bChgPenCode)
			{
				if (m_arrPts.GetSize()>2)
				{
					m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf,true,pts.GetSize()-1);
				}
			}	
			// 只有样条有临时线
			else if (m_nCurPenCode == penSpline)
			{
				CArray<PT_3DEX,PT_3DEX> arrtmp;
				arrtmp.Append(pts);
				if( m_bFastDisplayCurve )
				{
					for (int i=0; i<arrtmp.GetSize(); i++)
					{
						PT_3DEX &t = arrtmp.ElementAt(i);
						t.pencode = penLine;
					}
				}
				arrtmp.Add(arrtmp.GetAt(arrtmp.GetSize()-1));
				m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf);
				
				m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
				
			}
			
			cbuf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_pEditor->SetCrossPos(pt1);
			m_pEditor->RefreshView();
		}
		
				

	}

}

void CDrawCurveProcedure::BatchBack()
{
	if (m_pGeoCurve)
	{
		PutStreamPts();

		if( m_arrPts.GetSize() <= 11 || (m_bChgPenCode && m_nCurPenCode == penArc && m_arrPts.GetSize() <= 12))
		{
			if (m_arrAllPts.GetSize() == 0)
			{
				Abort();
				return;
			}
			else
			{
				m_arrPts.RemoveAll();

				int pos = m_arrIndex.GetAt(m_arrIndex.GetSize()-1);
				for (int i=pos; i<m_arrAllPts.GetSize(); i++)
				{
					m_arrPts.InsertAt(m_arrPts.GetSize(),m_arrAllPts.GetAt(i));
				}

				m_arrIndex.RemoveAt(m_arrIndex.GetSize()-1);
				if (m_arrIndex.GetSize() == 0)
				{
					m_arrAllPts.RemoveAll();
				}
				else
					m_arrAllPts.RemoveAt(pos+1,m_arrAllPts.GetSize()-pos-1);

// 				if (m_arrPts.GetSize() >= 2)
// 				{
// 					m_arrPts[m_arrPts.GetSize()-1].pencode = m_arrPts[m_arrPts.GetSize()-2].pencode;
// 				}

				m_nCurPenCode = m_arrPts[m_arrPts.GetSize()-1].pencode;

				if (m_nCurPenCode == penArc && m_arrAllPts.GetSize() > 1)
				{
					m_arrPts.InsertAt(0,m_arrAllPts.GetAt(m_arrAllPts.GetSize()-2));
				}
				
			}
			
		}

		long color = m_pGeoCurve->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pGeoCurve->CreateShape(m_arrAllPts.GetData(),m_arrAllPts.GetSize());
		GrBuffer precbuf;
		m_pGeoCurve->Draw(&precbuf);
		precbuf.SetAllColor(color);


		if( m_nCurPenCode==penStream )
		{
			//PutStreamPts();	
			int nsz = m_arrPts.GetSize();
			if( nsz<=10 )
			{
				m_arrPts.RemoveAt(1,nsz-1);
			}
			else
			{
				m_arrPts.RemoveAt(nsz-10,10);
			}
			
			PT_3DEX t0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);				
			PT_3D t1;
			COPY_3DPT(t1,t0);
			m_compress.BeginCompress();
			m_compress.AddStreamPt(t1);
			
			GrBuffer buf;
			buf.BeginLineString(color,0);
			buf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
			buf.End();
			
			buf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->SetCrossPos(t1);
			m_pEditor->RefreshView();
			
			
		}
		else 
		{
			int nsz = m_arrPts.GetSize();
			if( nsz<=10 )
			{
				m_arrPts.RemoveAt(1,nsz-1);
			}
			else
			{
				m_arrPts.RemoveAt(nsz-10,10);
			}

			PT_3DEX pt0;
			PT_3D pt1;
			CArray<PT_3DEX,PT_3DEX> pts;
			pts.Append(m_arrPts);
			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			COPY_3DPT(pt1,pt0);	
			
			GrBuffer cbuf,vbuf;
			if (m_bClosed && m_arrPts.GetSize()>2 && !m_bChgPenCode)
			{
				pts.Add(pts[0]);
			}
			m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
			
			m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf,m_bClosed && !m_bChgPenCode);
			cbuf.SetAllColor(color);
			if(m_bClosed && !m_bChgPenCode)
			{
				if (m_arrPts.GetSize()>2)
				{
					m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf,true,pts.GetSize()-1);
				}
			}	
			// 只有样条有临时线
			else if (m_nCurPenCode == penSpline)
			{
				CArray<PT_3DEX,PT_3DEX> arrtmp;
				arrtmp.Append(pts);
				if( m_bFastDisplayCurve )
				{
					for (int i=0; i<arrtmp.GetSize(); i++)
					{
						PT_3DEX &t = arrtmp.ElementAt(i);
						t.pencode = penLine;
					}
				}
				arrtmp.Add(arrtmp.GetAt(arrtmp.GetSize()-1));
				m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf);
				
				m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
				
			}
			
			cbuf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_pEditor->SetCrossPos(pt1);
			m_pEditor->RefreshView();
		}
		
				

	}

}


void CDrawCurveProcedure::PtReset(PT_3D &pt)
{
	if( !m_pGeoCurve || IsProcFinished(this)) return;

	if (m_bChgPenCode && m_arrAllPts.GetSize() > 1)
	{
		// 圆弧
		if (m_nCurPenCode == penArc)
		{
			m_arrPts.RemoveAt(0,2);
		}
		else
		{
			m_arrPts.RemoveAt(0);
		}		
	}	
	m_arrAllPts.Append(m_arrPts);
	m_arrPts.Copy(m_arrAllPts);
	
	PutStreamPts();
	
	//采集后光滑一次
	if( m_bDoSmooth && m_pGeoCurve!=NULL )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts0, arrPts1;
		PT_3DEX expt;
		//m_pGeoCurve->GetShape(arrPts1);
		arrPts1.Copy(m_arrPts);
		int sum = arrPts1.GetSize();
		for( int i=0; i<sum; i++)
		{
			expt = arrPts1[i];
			if( expt.pencode!=penStream )break;
			arrPts0.Add(expt);
		}
		
		if( i>=sum && sum>3 )
		{
			//弦高压缩
			PT_3D pt3d;
			CStreamCompress2_PT_3D comp;
			comp.BeginCompress(m_compress.GetLimit()*m_compress.m_lfScale);
			
			for( i=0; i<sum; i++)
			{
				expt = arrPts1[i];
				COPY_3DPT(pt3d,expt);
				comp.AddStreamPt(pt3d);			
			}
			
			comp.EndCompress();
			
			//获得压缩点
			CArray<PT_3DEX,PT_3DEX> arrPts, arrRets;
			{

				PT_3D *tpts;
				int ti, tnpt;
				comp.GetPts(tpts,tnpt);
				arrPts.SetSize(tnpt);
				for( ti=0; ti<tnpt; ti++)
				{
					expt.pencode = penStream;
					COPY_3DPT(expt,tpts[ti]);
					arrPts.SetAt(ti,expt);
				}
				
				//光滑
				SmoothLines(arrPts.GetData(),tnpt,m_compress.GetLimit()*m_compress.m_lfScale,arrRets);
			}
						
			m_arrPts.Copy(arrRets);
		}
	}
	
	if (m_bClosed)
	{
		if (m_arrPts.GetSize()<2)
		{
			Abort();
			return;
		}
		else if (m_arrPts.GetSize()>2)
		{
			m_arrPts.Add(m_arrPts[0]);
		}
	}
	else
	{
		if (m_arrPts.GetSize()<2)
		{
			Abort();
			return;
		}
	}

	if( m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize()) )
	{
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
	else
		Abort();
}


void CDrawCurveProcedure::Abort()
{
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_arrPts.RemoveAll();
	CProcedure::Abort();
}

CGeometry *Rectify(CGeometry* pObj, double tolerance, bool bLockStart, bool bLockEnd)
{
	if( !pObj )return NULL;
	//准备
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);
	int num = arr.GetSize();
	if( num<=2 )return NULL;
	
	PT_3D *pts = new PT_3D[num];
	if( !pts )return NULL;
	
	//取点
	PT_3DEX expt;
	for(int j=0; j<num; j++)
	{
		COPY_3DPT(pts[j],arr[j]);
	}
	
	//直角化
	if( GraphAPI::GRectifyPoints(pts,num,tolerance,bLockStart,bLockEnd) )
	{
		//生成新的对象
		pObj = pObj->Clone();
		if( pObj )
		{
			CArray<PT_3DEX,PT_3DEX> arr1;
			arr1.Copy(arr);

			for(j=0; j<num; j++)
			{
				COPY_3DPT(arr1[j],pts[j]);
			}

			pObj->CreateShape(arr1.GetData(),arr1.GetSize());
			
			delete[] pts;
			return pObj;
		}
	}
	else
	{
		//	OutputTipString(StrFromResID(IDS_ERR_RECTIFY));
	}
	
	delete[] pts;
	return NULL;
}

void CDrawCurveProcedure::Finish()
{
	if( m_nCurPenCode==penLine && m_bRectify && m_pGeoCurve )
	{
		PT_3DEX cpt,pt1,pt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pGeoCurve->GetShape(arr);
		int nPtSum = arr.GetSize();
		if( nPtSum>2 )
		{
			BOOL bClosed = FALSE, bFixPt = FALSE;
			
			//是否闭合，如果闭合，就先去掉尾点
			pt1 = m_pGeoCurve->GetDataPoint(0);
			pt4 = m_pGeoCurve->GetDataPoint(nPtSum-1);
			if( fabs(pt1.x-pt4.x)<1e-4 && fabs(pt1.y-pt4.y)<1e-4 )
			{
				bClosed = TRUE;
				cpt = pt4;
				arr.RemoveAt(nPtSum-1);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
				nPtSum--;
			}
			
			//如果是捕捉时不闭合，就不要再闭合了
			if( (bClosed && m_bOpenIfSnapped && m_nEndSnapped==3) )
			{
				bClosed = FALSE;
			}
			
			//补上一个角点
			if( bClosed && nPtSum>2 && (nPtSum%2==1) )
			{
				PT_3DEX pt2,pt3;
				double dx1,dx2,dy1,dy2,c1,c2,det;
				
				pt1 = m_pGeoCurve->GetDataPoint(0);
				pt2 = m_pGeoCurve->GetDataPoint(1);
				pt3 = m_pGeoCurve->GetDataPoint(nPtSum-2);
				pt4 = m_pGeoCurve->GetDataPoint(nPtSum-1);
				
				dx1 = pt2.x-pt1.x;	dy1 = pt2.y-pt1.y;
				dx2 = pt4.x-pt3.x;	dy2 = pt4.y-pt3.y;
				
				c1 = dy2 * pt1.x - dx2 * pt1.y;
				c2 = dy1 * pt4.x - dx1 * pt4.y;
				
				det  = -dy2*dx1 + dy1*dx2;
				
				if( fabs(det)>1e-10 )
				{
					pt4.x = (-c1*dx1 + c2 *dx2)/det;
					pt4.y = (-c1*dy1 + c2*dy2 )/det;
					m_pGeoCurve->GetShape(arr);
					arr.Add(pt4);
					m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
					bFixPt = TRUE;
				}
			}
			
			if( bClosed )
			{
				m_pGeoCurve->GetShape(arr);
				arr.Add(cpt);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
			}
			
			double tole = GetProfileDouble(CString(REGPATH_COMMAND)+_T("\\Rectify"),_T("Sigma"),1.0);
			CGeoCurve *pObj = (CGeoCurve*)Rectify(m_pGeoCurve,tole,(m_nEndSnapped&1)!=0,(m_nEndSnapped&2)!=0);
			if( pObj )
			{
				pObj->GetShape(arr);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
				delete pObj;
			}
			//容差太小，直角化不成功
			else if( bFixPt )
			{
				m_pGeoCurve->GetShape(arr);
				arr.RemoveAt(nPtSum);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
			}
		}
	}
	else
	{
		PT_3DEX cpt,pt1,pt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pGeoCurve->GetShape(arr);
		int nPtSum = arr.GetSize();
		if( nPtSum>2 )
		{
			BOOL bClosed = FALSE, bFixPt = FALSE;
			
			//如果是捕捉时不闭合，就不要再闭合了
			pt1 = arr[0];
			pt4 = arr[nPtSum-1];
			if( fabs(pt1.x-pt4.x)<1e-4 && fabs(pt1.y-pt4.y)<1e-4
				&& m_bOpenIfSnapped && m_nEndSnapped==3 )
			{
				m_pGeoCurve->GetShape(arr);
				arr.RemoveAt(arr.GetSize()-1);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
			}
		}
	}
	CProcedure::Finish();
}


void CDrawCurveProcedure::GetParams(CValueTable& tab)
{
}


void CDrawCurveProcedure::SetParams(CValueTable& tab,BOOL bInit)
{
	CProcedure::SetParams(tab,bInit);
}

void CDrawCurveProcedure::Start()
{
//	m_pGeoCurve = new CGeoCurve;
// 	if( m_pGeoCurve )
// 		m_pGeoCurve->EnableClose(m_bClosed);
// 	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	m_pGeoCurve->WriteTo(tab);
// 	tab.EndAddValueItem();
	GotoState(PROCSTATE_READY);
		
	m_pEditor->CloseSelector();

	m_arrPts.RemoveAll();
	m_arrAllPts.RemoveAll();
	m_arrIndex.RemoveAll();
	m_bClosed = FALSE;
	m_bRectify = FALSE;
	m_bChgPenCode = FALSE;
	m_compress.SetLimit(0.1);
/*	m_compress.m_lfScale = m_pDoc->GetScale()*1e-3;*/
	m_bDoSmooth = FALSE;
	m_bFastDisplayCurve = FALSE;
	m_bTrack = FALSE;
	m_bMultiPtBuildPt = FALSE;
	m_fTolerBuildPt = 0.5;
	m_arrBuildPt.RemoveAll();

	if( m_nCurPenCode==penStream )
	{
		m_compress.BeginCompress();
		m_bRectify = FALSE;
	}
	
	m_fMaxEndDis = 0;
	m_nEndSnapped = 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDrawDCurveProcedure::CDrawDCurveProcedure()
{
	m_nMode = 0;
	m_bBaseLine = TRUE;

	m_bTrack = FALSE;
	m_trackHandle = 0;
	m_nLastTrackIndex = -1;
	m_TrackPart = 0;
}

CDrawDCurveProcedure::~CDrawDCurveProcedure()
{

}

void CDrawDCurveProcedure::PtReset(PT_3D &pt)
{
	if( !m_pGeoCurve || IsProcFinished(this)) return;
	
	if (m_bChgPenCode && m_arrAllPts.GetSize() > 1)
	{
		// 圆弧
		if (m_nCurPenCode == penArc)
		{
			m_arrPts.RemoveAt(0,2);
		}
		else
		{
			m_arrPts.RemoveAt(0);
		}		
	}	
	m_arrAllPts.Append(m_arrPts);
	m_arrPts.Copy(m_arrAllPts);
	
	PutStreamPts();
	
	//采集后光滑一次
	if( m_bDoSmooth && m_pGeoCurve!=NULL )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts1;
		PT_3DEX expt;
		//m_pGeoCurve->GetShape(arrPts1);
		arrPts1.Copy(m_arrPts);
		int sum = arrPts1.GetSize();
		for( int i=0; i<sum; i++)
		{
			expt = arrPts1[i];
			if( expt.pencode!=penStream )break;
		}
		
		if( i>=sum && sum>3 )
		{
			//弦高压缩
			PT_3D pt3d;
			CStreamCompress5_PT_3DEX comp;
			comp.BeginCompress(m_compress.GetLimit()*m_compress.m_lfScale);
			
			for( i=0; i<sum; i++)
			{
				expt = arrPts1[i];
				COPY_3DPT(pt3d,expt);
				comp.AddStreamPt(pt3d);
				
				arrPts1.RemoveAt(sum-1-i);				
			}
			
			comp.EndCompress();
			
			//获得压缩点
			CArray<PT_3DEX,PT_3DEX> arrPts, arrRets;
			{
				PT_3DEX *tpts;
				int ti, tnpt;
				comp.GetPts(tpts,tnpt);
				arrPts.SetSize(tnpt);
				for( ti=0; ti<tnpt; ti++)
				{
					COPY_3DPT(expt,tpts[ti]);
					arrPts.SetAt(ti,expt);
				}
				
				//光滑
				SmoothLines(arrPts.GetData(),tnpt,m_compress.GetLimit()*m_compress.m_lfScale,arrRets);
			}
			
			//取出结果点
			//m_pGeoCurve->CreateShape(arrRets.GetData(),arrRets.GetSize());	
			
			m_arrPts.Copy(arrRets);
		}
	}
	
	if (m_bClosed)
	{
		if (m_arrPts.GetSize()<2)
		{
			Abort();
			return;
		}
		else if (m_arrPts.GetSize()>2)
		{
			PT_3DEX pt0 = m_arrPts[0];
			pt0.pencode = m_arrPts[m_arrPts.GetUpperBound()].pencode;
			m_arrPts.Add(pt0);
		}
	}
	else
	{
		if (m_arrPts.GetSize()<2)
		{
			Abort();
			return;
		}
	}
	if( m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize()) )
	{
		if (m_bBaseLine)
		{
			FinishDCurve();
			m_pGeoCurve->GetShape(m_arrPts);

			m_bBaseLine = FALSE;
			m_arrBasePts.Copy(m_arrPts);
			m_pEditor->UpdateDrag(ud_ClearDrag);
			GrBuffer buf;

			long color = m_pGeoCurve->GetColor();
			if (color == -1)
			{
				color = m_layCol;
			}

			buf.BeginLineString(color,0);
			m_pGeoCurve->GetShape()->ToGrBuffer(&buf);
			buf.End();
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			Start();
			UpdateParams(FALSE);			
		}
		else
		{
			FinishDCurve();
			m_pGeoCurve->GetShape(m_arrPts);

			m_arrPts[0].pencode = penMove;
			m_arrBasePts.Append(m_arrPts);
			if (m_pGeoCurve->CreateShape(m_arrBasePts.GetData(),m_arrBasePts.GetSize()))
			{
				m_pEditor->UpdateDrag(ud_ClearDrag);
 				Finish();
			}
			else
			{
				Abort();
			}
			
		}
		
 	}
	else
		Abort();	
}

void CDrawDCurveProcedure::FinishDCurve()
{
	if( m_nCurPenCode==penLine && m_bRectify && m_pGeoCurve )
	{
		PT_3DEX cpt,pt1,pt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pGeoCurve->GetShape(arr);
		int nPtSum = arr.GetSize();
		if( nPtSum>2 )
		{
			BOOL bClosed = FALSE, bFixPt = FALSE;
			
			//是否闭合，如果闭合，就先去掉尾点
			pt1 = m_pGeoCurve->GetDataPoint(0);
			pt4 = m_pGeoCurve->GetDataPoint(nPtSum-1);
			if( fabs(pt1.x-pt4.x)<1e-4 && fabs(pt1.y-pt4.y)<1e-4 )
			{
				bClosed = TRUE;
				cpt = pt4;
				arr.RemoveAt(nPtSum-1);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
				nPtSum--;
			}
			
			//如果是捕捉时不闭合，就不要再闭合了
			if( (bClosed && m_bOpenIfSnapped && m_nEndSnapped==3) )
			{
				bClosed = FALSE;
			}
			
			//补上一个角点
			if( bClosed && nPtSum>2 && (nPtSum%2==1) )
			{
				PT_3DEX pt2,pt3;
				double dx1,dx2,dy1,dy2,c1,c2,det;
				
				pt1 = m_pGeoCurve->GetDataPoint(0);
				pt2 = m_pGeoCurve->GetDataPoint(1);
				pt3 = m_pGeoCurve->GetDataPoint(nPtSum-2);
				pt4 = m_pGeoCurve->GetDataPoint(nPtSum-1);
				
				dx1 = pt2.x-pt1.x;	dy1 = pt2.y-pt1.y;
				dx2 = pt4.x-pt3.x;	dy2 = pt4.y-pt3.y;
				
				c1 = dy2 * pt1.x - dx2 * pt1.y;
				c2 = dy1 * pt4.x - dx1 * pt4.y;
				
				det  = -dy2*dx1 + dy1*dx2;
				
				if( fabs(det)>1e-10 )
				{
					pt4.x = (-c1*dx1 + c2 *dx2)/det;
					pt4.y = (-c1*dy1 + c2*dy2 )/det;
					m_pGeoCurve->GetShape(arr);
					arr.Add(pt4);
					m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
					bFixPt = TRUE;
				}
			}
			
			if( bClosed )
			{
				m_pGeoCurve->GetShape(arr);
				arr.Add(cpt);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
			}
			
			double tole = GetProfileDouble(CString(REGPATH_COMMAND)+_T("\\Rectify"),_T("Sigma"),1.0);
			CGeoCurve *pObj = (CGeoCurve*)Rectify(m_pGeoCurve,tole,(m_nEndSnapped&1)!=0,(m_nEndSnapped&2)!=0);
			if( pObj )
			{
				pObj->GetShape(arr);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
				delete pObj;
			}
			//容差太小，直角化不成功
			else if( bFixPt )
			{
				m_pGeoCurve->GetShape(arr);
				arr.RemoveAt(nPtSum);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
			}
		}
	}
	else
	{
		PT_3DEX cpt,pt1,pt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pGeoCurve->GetShape(arr);
		int nPtSum = arr.GetSize();
		if( nPtSum>2 )
		{
			BOOL bClosed = FALSE, bFixPt = FALSE;
			
			//如果是捕捉时不闭合，就不要再闭合了
			pt1 = arr[0];
			pt4 = arr[nPtSum-1];
			if( fabs(pt1.x-pt4.x)<1e-4 && fabs(pt1.y-pt4.y)<1e-4
				&& m_bOpenIfSnapped && m_nEndSnapped==3 )
			{
				m_pGeoCurve->GetShape(arr);
				arr.RemoveAt(arr.GetSize()-1);
				m_pGeoCurve->CreateShape(arr.GetData(),arr.GetSize());
			}
		}
	}
}

void CDrawDCurveProcedure::Start()
{
	CDrawCurveProcedure::Start();
}

void CDrawDCurveProcedure::Abort()
{
	CDrawCurveProcedure::Abort();
}

void CDrawDCurveProcedure::Finish()
{
	CProcedure::Finish();
}

void CDrawDCurveProcedure::Back()
{
	if (m_pGeoCurve)
	{
		PutStreamPts();

		if( m_arrPts.GetSize() <= 1 || (m_bChgPenCode && m_nCurPenCode == penArc && m_arrPts.GetSize() <= 2))
		{
			if (m_arrAllPts.GetSize() == 0)
			{
				if (m_arrBasePts.GetSize() > 0)
				{
					m_arrPts.Copy(m_arrBasePts);
					m_arrBasePts.RemoveAll();
					m_bBaseLine = TRUE;
				}
				else
				{
					Abort();
					return;
				}
				
			}
			else
			{
				m_arrPts.RemoveAll();

				int pos = m_arrIndex.GetAt(m_arrIndex.GetSize()-1);
				for (int i=pos; i<m_arrAllPts.GetSize(); i++)
				{
					m_arrPts.InsertAt(m_arrPts.GetSize(),m_arrAllPts.GetAt(i));
				}

				m_arrIndex.RemoveAt(m_arrIndex.GetSize()-1);
				if (m_arrIndex.GetSize() == 0)
				{
					m_arrAllPts.RemoveAll();
				}
				else
					m_arrAllPts.RemoveAt(pos+1,m_arrAllPts.GetSize()-pos-1);

// 				if (m_arrPts.GetSize() >= 2)
// 				{
// 					m_arrPts[m_arrPts.GetSize()-1].pencode = m_arrPts[m_arrPts.GetSize()-2].pencode;
// 				}

				m_nCurPenCode = m_arrPts[m_arrPts.GetSize()-1].pencode;

				if (m_nCurPenCode == penArc && m_arrAllPts.GetSize() > 1)
				{
					m_arrPts.InsertAt(0,m_arrAllPts.GetAt(m_arrAllPts.GetSize()-2));
				}
				
			}
			
		}

		DWORD color = m_pGeoCurve->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);

		CArray<PT_3DEX,PT_3DEX> arrAll;
		arrAll.Append(m_arrBasePts);
		arrAll.Append(m_arrAllPts);

		m_pGeoCurve->CreateShape(arrAll.GetData(),arrAll.GetSize());
		GrBuffer precbuf;
		m_pGeoCurve->Draw(&precbuf);
		precbuf.SetAllColor(color);


		if( m_nCurPenCode==penStream )
		{
			//PutStreamPts();
			if( m_arrPts.GetSize()<=1 )
			{
				Abort();
				return;
			}
			
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			
			PT_3DEX t0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);				
			PT_3D t1;
			COPY_3DPT(t1,t0);
			m_compress.BeginCompress();
			m_compress.AddStreamPt(t1);
			
			GrBuffer buf;
			buf.BeginLineString(color,0);
			buf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
			buf.End();
			
			buf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->SetCrossPos(t1);
			m_pEditor->RefreshView();
			
			
		}
		else 
		{
			if(m_arrPts.GetSize() < 2)
			{
				m_pGeoCurve->CreateShape(&PT_3DEX(),1);
				Abort();
				return;
			}
			PT_3DEX pt0;
			PT_3D pt1;
			CArray<PT_3DEX,PT_3DEX> pts;
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			pts.Append(m_arrPts);
			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			COPY_3DPT(pt1,pt0);	
			
			GrBuffer cbuf,vbuf;
			if (m_bClosed && m_arrPts.GetSize()>2 && !m_bChgPenCode)
			{
				pts.Add(pts[0]);
			}
			m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
			
			m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf,m_bClosed && !m_bChgPenCode);
			cbuf.SetAllColor(color);
			if(m_bClosed && !m_bChgPenCode)
			{
				if (m_arrPts.GetSize()>2)
				{
					m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf,true,pts.GetSize()-1);
				}
			}	
			// 只有样条有临时线
			else if (m_nCurPenCode == penSpline)
			{
				CArray<PT_3DEX,PT_3DEX> arrtmp;
				arrtmp.Append(pts);
				if( m_bFastDisplayCurve )
				{
					for (int i=0; i<arrtmp.GetSize(); i++)
					{
						PT_3DEX &t = arrtmp.ElementAt(i);
						t.pencode = penLine;
					}
				}
				arrtmp.Add(arrtmp.GetAt(arrtmp.GetSize()-1));
				m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf);
				
				m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
				
			}
			
			cbuf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_pEditor->SetCrossPos(pt1);
			m_pEditor->RefreshView();
		}
		
				

	}
}

void CDrawDCurveProcedure::BatchBack()
{
	if (m_pGeoCurve)
	{
		PutStreamPts();

		if( m_arrPts.GetSize() <= 11 || (m_bChgPenCode && m_nCurPenCode == penArc && m_arrPts.GetSize() <= 12))
		{
			if (m_arrAllPts.GetSize() == 0)
			{
				Abort();
				return;
			}
			else
			{
				m_arrPts.RemoveAll();

				int pos = m_arrIndex.GetAt(m_arrIndex.GetSize()-1);
				for (int i=pos; i<m_arrAllPts.GetSize(); i++)
				{
					m_arrPts.InsertAt(m_arrPts.GetSize(),m_arrAllPts.GetAt(i));
				}

				m_arrIndex.RemoveAt(m_arrIndex.GetSize()-1);
				if (m_arrIndex.GetSize() == 0)
				{
					m_arrAllPts.RemoveAll();
				}
				else
					m_arrAllPts.RemoveAt(pos+1,m_arrAllPts.GetSize()-pos-1);

// 				if (m_arrPts.GetSize() >= 2)
// 				{
// 					m_arrPts[m_arrPts.GetSize()-1].pencode = m_arrPts[m_arrPts.GetSize()-2].pencode;
// 				}

				m_nCurPenCode = m_arrPts[m_arrPts.GetSize()-1].pencode;

				if (m_nCurPenCode == penArc && m_arrAllPts.GetSize() > 1)
				{
					m_arrPts.InsertAt(0,m_arrAllPts.GetAt(m_arrAllPts.GetSize()-2));
				}
				
			}
			
		}

		long color = m_pGeoCurve->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pGeoCurve->CreateShape(m_arrAllPts.GetData(),m_arrAllPts.GetSize());
		GrBuffer precbuf;
		m_pGeoCurve->Draw(&precbuf);
		precbuf.SetAllColor(color);


		if( m_nCurPenCode==penStream )
		{
			//PutStreamPts();	
			int nsz = m_arrPts.GetSize();
			if( nsz<=10 )
			{
				m_arrPts.RemoveAt(1,nsz-1);
			}
			else
			{
				m_arrPts.RemoveAt(nsz-10,10);
			}
			
			PT_3DEX t0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);				
			PT_3D t1;
			COPY_3DPT(t1,t0);
			m_compress.BeginCompress();
			m_compress.AddStreamPt(t1);
			
			GrBuffer buf;
			buf.BeginLineString(color,0);
			buf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
			buf.End();
			
			buf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->SetCrossPos(t1);
			m_pEditor->RefreshView();
			
			
		}
		else 
		{
			int nsz = m_arrPts.GetSize();
			if( nsz<=10 )
			{
				m_arrPts.RemoveAt(1,nsz-1);
			}
			else
			{
				m_arrPts.RemoveAt(nsz-10,10);
			}

			PT_3DEX pt0;
			PT_3D pt1;
			CArray<PT_3DEX,PT_3DEX> pts;
			pts.Append(m_arrPts);
			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			COPY_3DPT(pt1,pt0);	
			
			GrBuffer cbuf,vbuf;
			if (m_bClosed && m_arrPts.GetSize()>2 && !m_bChgPenCode)
			{
				pts.Add(pts[0]);
			}
			m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
			
			m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf,m_bClosed && !m_bChgPenCode);
			cbuf.SetAllColor(color);
			if(m_bClosed && !m_bChgPenCode)
			{
				if (m_arrPts.GetSize()>2)
				{
					m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf,true,pts.GetSize()-1);
				}
			}	
			// 只有样条有临时线
			else if (m_nCurPenCode == penSpline)
			{
				CArray<PT_3DEX,PT_3DEX> arrtmp;
				arrtmp.Append(pts);
				if( m_bFastDisplayCurve )
				{
					for (int i=0; i<arrtmp.GetSize(); i++)
					{
						PT_3DEX &t = arrtmp.ElementAt(i);
						t.pencode = penLine;
					}
				}
				arrtmp.Add(arrtmp.GetAt(arrtmp.GetSize()-1));
				m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf);
				
				m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
				
			}
			
			cbuf.AddBuffer(&precbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_pEditor->SetCrossPos(pt1);
			m_pEditor->RefreshView();
		}
		
	}
}


void CDrawDCurveProcedure::PtClick(PT_3D &pt, int flag)
{
	if (m_nMode == 1 && !m_bBaseLine)
	{
		int size = m_arrBasePts.GetSize();
		if(size <= 1)
		{
			Abort();
			return;
		}
		
		PT_3D ptStart = m_arrBasePts[size-1];
		double matrix[16];
		Matrix44FromMove(pt.x-ptStart.x,pt.y-ptStart.y,pt.z-ptStart.z,matrix);	
		
		CGeometry *pGeo = m_pGeoCurve->Clone();
		if (!pGeo)
		{
			Abort();
			return;
		}
		
		pGeo->Transform(matrix);

		pGeo->GetShape(m_arrPts);
		
		m_arrPts[0].pencode = penMove;
		m_arrBasePts.Append(m_arrPts);
		if (m_pGeoCurve->CreateShape(m_arrBasePts.GetData(),m_arrBasePts.GetSize()))
		{
			m_pEditor->UpdateDrag(ud_ClearDrag);
			Finish();
		}
		else
		{
			Abort();
		}
	}
	else
	{
		CDrawCurveProcedure::PtClick(pt,flag);
	}
}

void CDrawDCurveProcedure::PtMove(PT_3D &pt)
{
	m_curPt = pt;
	if (m_nMode == 1 && !m_bBaseLine)
	{
		int size = m_arrBasePts.GetSize();
		if(size <= 1)  return;

		PT_3D ptStart = m_arrBasePts[size-1];
		double matrix[16];
		Matrix44FromMove(pt.x-ptStart.x,pt.y-ptStart.y,pt.z-ptStart.z,matrix);	
		
		CGeometry *pGeo = m_pGeoCurve->Clone();
		if (!pGeo) return;

		pGeo->Transform(matrix);

		GrBuffer buf;
		pGeo->Draw(&buf);
// ***********************		GrBuffer vbuf;
// 		const CShapeLine *pSL = m_pGeoCurve->GetShape();
// 		//if (bUseMoveCurve)
// 		{
// 			//pSL = m_pGeoMoveCurve->GetShape();
// 		}
// 		if(m_bClosed && !m_bChgPenCode)
// 			pSL->GetVariantGrBuffer(&vbuf,true,m_arrPts.GetSize()-2);
// 		else
// 			pSL->GetVariantGrBuffer(&vbuf);
// 		vbuf.SetAllColor(m_pGeoCurve->GetColor());
// 		//m_pGeoCurve->Draw(&vbuf);
// 		
// 		if (m_bTrackFirstPt)
// 		{
// 			PT_3D cltpt;	
// 			(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_curPt,&cltpt);
// 			CPFeature pFtr = m_pEditor->GetDataQuery()->FindNearestObject(cltpt,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS(),
// 				m_pEditor->GetCoordWnd().m_pSearchCS);
// 			
// 			if (FtrToHandle(pFtr) != m_firstTrackHandle)
// 			{
// 				goto trackend;
// 			}
// 			
// 			GrBuffer trackbuf;
// 			CArray<PT_3DEX,PT_3DEX> arrPts,arr;
// 			CGeometry *pGeo = HandleToFtr(m_firstTrackHandle)->GetGeometry();
// 			
// 			pGeo->GetShape()->GetPts(arrPts);
// 			
// 			if (pGeo->GetClassType() == CLS_GEOSURFACE)
// 			{
// 				arrPts.Add(arrPts[0]);
// 			}
// 			int trackPart = GetDCurvePts(m_curPt, pGeo, arrPts);
// 			if (trackPart != m_TrackPart)
// 			{
// 				goto trackend;
// 			}
// 			
// 			PT_3DEX curPt(m_curPt,penLine), retPt(m_curPt,penLine);
// 			int index = -1;
// 			GraphAPI::GGetNearstDisPToPointSet2D(arrPts.GetData(),arrPts.GetSize(),curPt,retPt,&index);
// 			retPt.z = retPt.z = m_curPt.z;
// 			
// 			if (index >= 0)
// 			{				
// 				GetPartLineOf2Pt(arrPts.GetData(),arrPts.GetSize(),m_firstTrackPt,m_nFirtPtIndxtAtTrackLine,retPt,index,arr);
// 				
// 				for (int i=0; i<arr.GetSize(); i++)
// 				{
// 					arr[i].z = m_curPt.z;
// 				}
// 				
// 				arr.InsertAt(0,m_firstTrackPt);					
// 				arr.Add(retPt);				
// 				
// 				trackbuf.BeginLineString(RGB(255,0,0)m_pGeoCurve->GetColor(),0);
// 				for ( i=0; i<arr.GetSize(); i++)
// 				{
// 					if (i == 0)
// 					{
// 						trackbuf.MoveTo(&arr[i]);
// 					}
// 					else
// 					{
// 						trackbuf.LineTo(&arr[i]);
// 					}
// 				}
// 				trackbuf.End();
// 				
// 				vbuf.AddBuffer(&trackbuf);
// 			}
// 			
// 		}
// trackend:

		delete pGeo;
		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
	else
	{
		CDrawCurveProcedure::PtMove(pt);
	}
}

void CDrawDCurveProcedure::TrackLine()
{
	CDrawCurveProcedure::TrackLine();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawParallelProcedure::CDrawParallelProcedure()
{
	m_pGeoPara = NULL;
	m_bClosed = FALSE;
	m_nCurPenCode = penLine;
	m_lfSepcialWidth = 1.0;
	m_bDrawWidth = FALSE;
	m_bMouseWidth = TRUE;
	m_bByCenterLine = FALSE;


	m_bRectify = FALSE;
	m_bBreakup = FALSE;
	
	m_fAutoCloseToler = 0;
	m_fMaxEndDis = 0;
	m_bDoSmooth = FALSE;
	m_bOpenIfSnapped = FALSE;
	m_nEndSnapped = 0;

	m_bFastDisplayCurve = FALSE;

	m_bCenterlineMode = FALSE;
	m_strRetLayer = "";

	m_bDHeight = FALSE;

	m_fCurNodeWid = 0;
	m_nCurNodeType = ptNone;

	m_bTrack = FALSE;
	m_trackHandle = 0;
	m_nLastTrackIndex = -1;
	m_TrackPart = 0;
	m_bSnap2D = TRUE;
}


CDrawParallelProcedure::~CDrawParallelProcedure()
{
//	if( m_pGeoPara )delete m_pGeoPara;
}

void CDrawParallelProcedure::PutStreamPts()
{
	if( m_nCurPenCode==penStream && m_pGeoPara!=NULL )
	{
		m_compress.EndCompress();
		
		PT_3DEX *pts = NULL;
		int i, npt = 0;
		m_compress.GetPts(pts,npt);
		
		if( npt>1 )
		{
			// 			CArray<PT_3DEX,PT_3DEX> pts0;
			// 			m_pGeoCurve->GetShape(pts0);
			
			int nsum = m_arrPts.GetSize();
			for( i=0; i<npt; i++)
			{
				if( nsum>0 && i==0 );
				else m_arrPts.Add(pts[i]);
			}
			//_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		}
		
		m_compress.BeginCompress();
	}
}


void CDrawParallelProcedure::TempLine()
{
	BOOL m_bChgPenCode = FALSE;
	if (m_nCurPenCode != penStream)
	{
		m_pEditor->UpdateDrag(ud_ClearDrag);
	}

	long color = m_pGeoPara->GetColor();
	if (color == -1)
	{
		color = m_layCol;
	}
	
	GrBuffer cbuf,vbuf;
	const CShapeLine *pSL = m_pGeoPara->GetShape();
	if (m_nCurPenCode == penLine || (m_bFastDisplayCurve && m_nCurPenCode==penSpline))
	{
		CArray<PT_3DEX,PT_3DEX> arrtmp;
		arrtmp.Append(m_arrPts);

		if( m_bFastDisplayCurve )
		{
			for (int i=0; i<arrtmp.GetSize(); i++)
			{
				PT_3DEX &t = arrtmp.ElementAt(i);
				t.pencode = penLine;
			}
			m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
		}

		if (m_bClosed && arrtmp.GetSize()>2 && !m_bChgPenCode)
		{
			arrtmp.Add(arrtmp.GetAt(0));
			m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());

			pSL->GetConstGrBuffer(&cbuf,true);

			arrtmp.RemoveAt(m_arrPts.GetSize()-1);
			m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
			
		}
		else
		{
			pSL->GetConstGrBuffer(&cbuf);
		}

		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);

		if( m_bFastDisplayCurve )
		{
			m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		}
	
	}
	else if (m_nCurPenCode == penSpline || m_nCurPenCode == penArc )
	{
		GrBuffer *pvbuf = NULL;
		if (m_bClosed && !m_bChgPenCode)
		{
			m_arrPts.Add(m_arrPts.GetAt(0));
			m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

			pSL->GetConstGrBuffer(&cbuf,true);

			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			
		}
		else
		{
			pSL->GetConstGrBuffer(&cbuf);
		}

		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);

	}
	else if(m_nCurPenCode == penStream)
	{
/*		m_compress.BeginCompress();
		m_compress.AddStreamPt(pt);
		
		if( m_arrPts.GetSize()>=2 )
		{
			PT_3DEX t0;
			t0 = m_arrPts.GetAt(m_arrPts.GetSize()-2);
			PT_3D tpt;
			COPY_3DPT(tpt,t0);
			GrBuffer addbuf;
			addbuf.BeginLineString(m_pGeoPara->GetColor(),0);		
			addbuf.MoveTo(&tpt);
			addbuf.LineTo(&pt);
			addbuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
		}
			
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
*/
	}
}

void CDrawParallelProcedure::ReverseLine()
{
	PutStreamPts();
	
	if (m_arrPts.GetSize() >= 2)
	{		
		//数据点列方向
		int num = m_arrPts.GetSize();
		PT_3DEX expt1,expt2;
		for( int i=0; i<num/2; i++)
		{
			expt1 = m_arrPts[i];
			m_arrPts[i] = m_arrPts[num-1-i];
			m_arrPts[num-1-i] = expt1;
		}
		
		m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		
		TempLine();
		
		PT_3D pt3d; 
		COPY_3DPT(pt3d,m_arrPts[num-1]);
		if( m_nCurPenCode==penStream )
		{
			m_compress.BeginCompress();
			//			COPY_3DPT(pt3d,expt1);
			m_compress.AddStreamPt(m_arrPts[num-2]);
			// 			COPY_3DPT(pt3d,expt2);
			m_compress.AddStreamPt(m_arrPts[num-1]);
		}
		
		
		m_pEditor->SetCrossPos(pt3d);
		
		PtMove(pt3d);
		return;
	}
}

VOID CDrawParallelProcedure::PtClick(PT_3D &pt, int flag)
{	
	GotoState(PROCSTATE_PROCESSING);

	if(m_bTrack)
	{
		TrackLine();
		return;
	}

	PutStreamPts();
	if( !m_pGeoPara )return;

	PT_3DEX expt(pt,m_nCurPenCode,m_fCurNodeWid,m_nCurNodeType);
	m_arrPts.Add(expt);

	long color = m_pGeoPara->GetColor();
	if (color == -1)
	{
		color = m_layCol;
	}
	
	if( !m_bDrawWidth )
	{
		m_ptLast = pt;

		m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		
		GrBuffer cbuf,vbuf;
		const CShapeLine *pSL = m_pGeoPara->GetShape();
		if (m_nCurPenCode == penLine || (m_bFastDisplayCurve && m_nCurPenCode==penSpline))
		{
			CArray<PT_3DEX,PT_3DEX> arrtmp;
			arrtmp.Append(m_arrPts);
			
			if( m_bFastDisplayCurve )
			{
				for (int i=0; i<arrtmp.GetSize(); i++)
				{
					PT_3DEX &t = arrtmp.ElementAt(i);
					t.pencode = penLine;
				}
				m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
			}
			
			if (m_bClosed && arrtmp.GetSize()>2)
			{
				arrtmp.Add(arrtmp.GetAt(0));
				m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				pSL->GetAddConstGrBuffer(&cbuf,true);
				pSL->GetVariantGrBuffer(&vbuf,true,arrtmp.GetSize()-1);
				
//				m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
				arrtmp.RemoveAt(m_arrPts.GetSize()-1);
				m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				
			}
			else
			{
				pSL->GetAddConstGrBuffer(&cbuf);
			}
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			cbuf.SetAllColor(color);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
			
			if( m_bFastDisplayCurve )
			{
				m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			}
			
		}
		else if (m_nCurPenCode == penSpline || m_nCurPenCode == penArc )
		{
			GrBuffer *pvbuf = NULL;
			if (m_bClosed)
			{
				m_arrPts.Add(m_arrPts.GetAt(0));
				m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
				
				pSL->GetAddConstGrBuffer(&cbuf,true);
				
				m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
				m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
				
				if (m_arrPts.GetSize() > 2)
				{
					m_arrPts.Add(m_arrPts.GetAt(0));
					m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
					
					pSL->GetVariantGrBuffer(&vbuf,TRUE,m_arrPts.GetSize()-1);
					
					m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
					m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
					
				}
				else
					pSL->GetVariantGrBuffer(&vbuf);
				
				pvbuf = &vbuf;
				
				
			}
			else
			{
				pSL->GetAddConstGrBuffer(&cbuf);
				
				// 只有样条有临时线
				if (m_nCurPenCode == penSpline)
				{
					m_arrPts.Add(expt);
					m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
					pSL->GetVariantGrBuffer(&vbuf);
					m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
					m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
					pvbuf = &vbuf;
				}	
				
			}
			
			m_pEditor->UpdateDrag(ud_SetVariantDrag,pvbuf);
			cbuf.SetAllColor(color);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
		}
		else if(m_nCurPenCode == penStream)
		{
			m_compress.BeginCompress();
			m_compress.AddStreamPt(pt);
			
			if( m_arrPts.GetSize()>=2 )
			{
				PT_3DEX t0;
				t0 = m_arrPts.GetAt(m_arrPts.GetSize()-2);
				PT_3D tpt;
				COPY_3DPT(tpt,t0);
				GrBuffer addbuf;
				addbuf.BeginLineString(m_pGeoPara->GetColor(),0);		
				addbuf.MoveTo(&tpt);
				addbuf.LineTo(&pt);
				addbuf.End();
				m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
			}
			
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			
		}
		
		return;
	
	}
	else if( !m_bMouseWidth && !m_bByCenterLine )
	{
		PT_3D dhPt = pt;
		if (!m_bDHeight)
		{
			dhPt.z = m_ptLast.z;
		}
		m_pGeoPara->SetCtrlPoint(0,dhPt);
		
		double width = ((CGeoParallel*)m_pGeoPara)->GetWidth();
		
		if( width>0 )width = fabs(m_lfSepcialWidth);
		else width = -fabs(m_lfSepcialWidth);
		
		((CGeoParallel*)m_pGeoPara)->SetWidth(width);
		
		//((CGeoParallel*)m_pObj)->SetWidth(m_lfSepcialWidth);
		Finish();
		return;
	}
	else if( !m_bMouseWidth && m_bByCenterLine )
	{
		double width = m_lfSepcialWidth*0.5;
		((CGeoParallel*)m_pGeoPara)->SetWidth(-width);
		
		CGeometry *pObj1 = NULL, *pObj2 = NULL, *pObj3 = NULL;
		int i, num;
		PT_3DEX expt;
		PT_3DEX *pts;
		CArray<PT_3DEX,PT_3DEX> arr;
		if(!m_pGeoPara->Separate(pObj1,pObj2))
			goto drawparallel_click_error1;			
		
		pObj2->GetShape(arr);
		num = arr.GetSize();
		if(num<=0 ) return;
		pts = new PT_3DEX[num];
		if(!pts) return;
		for( i=num-1; i>=0; i--)
		{
			pts[num-1-i] = arr[i];
		}
		if(!m_pGeoPara->CreateShape(pts,num))return;
		((CGeoParallel*)m_pGeoPara)->SetWidth(width*2);	
				
		if( pObj1 )delete pObj1;
		if( pObj2 )delete pObj2;
		
		Finish();
		return;
		
drawparallel_click_error1:
		
		if( pObj1 )delete pObj1;
		if( pObj2 )delete pObj2;
		Abort();
		return;
	}
	else if( m_bMouseWidth && !m_bByCenterLine )
	{
		PT_3D dhPt = pt;
		if (!m_bDHeight)
		{
			dhPt.z = m_ptLast.z;
		}
		m_pGeoPara->SetCtrlPoint(0,dhPt);
		Finish();
		return;
	}
	else if( m_bMouseWidth && m_bByCenterLine )
	{
		PT_3D dhPt = pt;
		//if (!m_bDHeight)
		{
			dhPt.z = m_ptLast.z;
		}
		m_pGeoPara->SetCtrlPoint(0,dhPt);
		double width = ((CGeoParallel*)m_pGeoPara)->GetWidth();
		//width *= 0.5;
		((CGeoParallel*)m_pGeoPara)->SetWidth(-width);
		
		CGeometry *pObj1 = NULL, *pObj2 = NULL, *pObj3 = NULL;
		int i, num;
		
		CArray<PT_3DEX,PT_3DEX> arr;
		PT_3DEX *pts;
		
		if( !m_pGeoPara->Separate(pObj1,pObj2) )
			goto drawparallel_click_error2;
		
		pObj3 = m_pGeoPara->Clone();
		if( !pObj3 )
			goto drawparallel_click_error2;
		

		
		pObj2->GetShape(arr);
		num = arr.GetSize();
		if(num<=0 ) return;
		pts = new PT_3DEX[num];
		if(!pts) return;
		for( i=num-1; i>=0; i--)
		{
			pts[num-1-i] = arr[i];
		}
		if(!m_pGeoPara->CreateShape(pts,num))return;
		((CGeoParallel*)m_pGeoPara)->SetWidth(width*2);	
		
// 		delete m_pGeoPara;
// 		m_pGeoPara = (CGeoParallel*)pObj3;
	//	pObj3 = NULL;	
		
		if( pObj1 )delete pObj1;
		if( pObj2 )delete pObj2;
	//	if( pObj3 )delete pObj3;
		
		Finish();
		return;
		
drawparallel_click_error2:
		
		if( pObj1 )delete pObj1;
		if( pObj2 )delete pObj2;
	//	if( pObj3 )delete pObj3;
		Abort();
		return;
	}
	return;
}

void CDrawParallelProcedure::PtMove(PT_3D &pt)
{
	m_curPt = pt;
	//追踪
	if (m_bTrack)
	{
		PT_3D cltpt;	
		(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_curPt,&cltpt);
		m_pEditor->GetDataQuery()->FindNearestObject(cltpt,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS(),
			m_pEditor->GetCoordWnd().m_pSearchCS);

		int num;
		const CPFeature *ftrs = (m_pEditor->GetDataQuery())->GetFoundHandles(num);

		BOOL bSameFtr = FALSE;
		int m;
		for(m=0; m<num; m++)
		{
			if(ftrs[m]==HandleToFtr(m_trackHandle))
			{
				bSameFtr = TRUE;
				break;
			}
		}
		
		PT_3DEX curPt(m_curPt,penLine), retPt(m_curPt,penLine);

		CGeometry *pGeo = HandleToFtr(m_trackHandle)->GetGeometry();
		CArray<PT_3DEX, PT_3DEX> pts1, pts2;
		if (bSameFtr)
		{
			int trackPart = GetCurvePartPts(m_curPt, pGeo, pts1);
			if(trackPart!=m_TrackPart)
			{
				bSameFtr = FALSE;
			}
			if(trackPart<0) goto trackend;
		}

		if(num<=0) goto trackend;
		
		if (bSameFtr)
		{
			int index = -1;
			GraphAPI::GGetNearstDisPToPointSet2D(pts1.GetData(),pts1.GetSize(),curPt,retPt,&index);
			curPt.z = retPt.z = m_curPt.z;

			PT_3DEX pt0 = m_trackPts[m_trackPts.GetSize()-1];
			if(GraphAPI::GIsEqual2DPoint(&curPt, &pt0))
			{
				;//如果咬合到最后一个点，则不处理
			}
			else if (index >= 0)
			{
				int pos = IsPtInCurve(curPt, m_trackPts);
				BOOL bSamePt = GraphAPI::GIsEqual2DPoint(&curPt, &m_trackPts[0]);
				if(pos>=0 && !bSamePt)
				{
					int cutnum = m_trackPts.GetSize()-pos-1;
					m_trackPts.RemoveAt(pos+1, cutnum);//回退
				}
				else
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					GetPartLineOf2Pt(pts1.GetData(),pts1.GetSize(),m_nLastTrackPt,m_nLastTrackIndex,retPt,index,arr);
					if(m_bSnap2D)//3D时保持高程为地物高程 2D则为测标高程
					{
						for (int i=0; i<arr.GetSize(); i++)
						{
							arr[i].z = m_curPt.z;
						}
					}
					if(arr.GetSize()>1 && IsPtInCurve(arr[1], m_trackPts)>=0)
					{
						goto trackend;//阻止调换方向，若需要换方向必须先回退到方向交叉点
					}
					m_trackPts.Append(arr);
				}
				m_nLastTrackIndex = index;
				m_nLastTrackPt = retPt;
			}
		}
		else//转移到另一个地物,仅补全m_nLastTrackPt到交点的部分，若有多个交点，取鼠标最近的
		{
			GetCurvePartPts(m_TrackPart, pGeo, pts1);
			for(int k=0; k<num; k++)
			{
				pGeo = ftrs[k]->GetGeometry();
				if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}

				int trackPart = GetCurvePartPts(m_curPt, pGeo, pts2);
				if(trackPart<0)
				{
					continue;
				}
				
				double t1, t2;
				PT_3DEX retpt1;//交点
				COPY_3DPT(retpt1, pt);
				retpt1.pencode = penLine;
				if(!GetCurveIntersectCurve(pts1.GetData(), pts1.GetSize(), pts2.GetData(), pts2.GetSize(), retpt1, &t1, &t2))
				{
					continue;
				}

				int pos = IsPtInCurve(retpt1, m_trackPts);
				BOOL bSamePt = GraphAPI::GIsEqual2DPoint(&retpt1, &m_trackPts[0]);
				
				if(pos>0 && !bSamePt)
				{
					int cutnum = m_trackPts.GetSize()-pos-1;
					m_trackPts.RemoveAt(pos+1, cutnum);
				}
				else
				{
					CArray<PT_3DEX, PT_3DEX> arr;
					GetPartLineOf2Pt(pts1.GetData(),pts1.GetSize(),m_nLastTrackPt,m_nLastTrackIndex,retpt1,(int)t1,arr);//首尾点重复
					if(m_bSnap2D)
					{
						for (int i=0; i<arr.GetSize(); i++)
						{
							arr[i].z = m_curPt.z;
						}
					}
					if(arr.GetSize()>1 && IsPtInCurve(arr[1], m_trackPts)>=0)
					{
						continue;//阻止调换方向，若需要换方向必须先回退到方向交叉点
					}
					m_trackPts.Append(arr);
				}
				m_trackPts.Add(retpt1);
				m_nLastTrackIndex = (int)(t2);
				m_nLastTrackPt = retpt1;
				m_trackHandle = FtrToHandle(ftrs[k]);
				m_TrackPart  = trackPart;
				break;
			}
		}
trackend:
		GrBuffer trackbuf;
		CArray<PT_3DEX,PT_3DEX> arr;	
		arr.Append(m_trackPts);
		arr.Add(retPt);		
		
		trackbuf.BeginLineString(RGB(255,0,0)/*m_pGeoCurve->GetColor()*/,0);
		for (int i=0; i<arr.GetSize(); i++)
		{
			if (i == 0)
			{
				trackbuf.MoveTo(&arr[i]);
			}
			else
			{
				trackbuf.LineTo(&arr[i]);
			}
		}
		if(m_bClosed)
		{
			CArray<PT_3DEX,PT_3DEX> allPts;
			allPts.Copy(m_arrPts);
			allPts.Append(m_trackPts);
			if(allPts.GetSize()>0)
			{
				trackbuf.MoveTo(&allPts[0]);
				trackbuf.LineTo(&curPt);
			}
		}
		trackbuf.End();

		m_pEditor->UpdateDrag(ud_SetVariantDrag,&trackbuf);
		return;
	}//追踪

	if( !m_pGeoPara||IsProcFinished(this)||m_arrPts.GetSize()<=0  )return;	

	if( !m_bDrawWidth )
	{
		if( m_nCurPenCode==penStream )
		{
			long color = m_pGeoPara->GetColor();
			if (color == -1)
			{
				color = m_layCol;
			}

			GrBuffer addbuf;
			addbuf.BeginLineString(color,0);
			
			if( 1 )
			{
				PT_3D tpt;
				m_compress.GetCurCompPt(tpt);
				m_compress.AddStreamPt(pt);
				
				addbuf.MoveTo(&tpt);
				addbuf.LineTo(&pt);
				addbuf.End();
				m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
			}
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			
			//自动闭合处理
			if( m_arrPts.GetSize()>0 )
			{
				PT_3DEX t0;
				t0 = m_arrPts.GetAt(0);
				double dis = sqrt((pt.x-t0.x)*(pt.x-t0.x)+(pt.y-t0.y)*(pt.y-t0.y));
				if( dis>m_fMaxEndDis )
					m_fMaxEndDis = dis;
				
				if( m_fMaxEndDis>m_compress.GetLimit() && m_fMaxEndDis>m_fAutoCloseToler &&
					dis<m_fAutoCloseToler )
				{
					
					PutStreamPts();
					
					if( m_arrPts.GetSize()>=3 )
					{
						PT_3DEX t;
						t = m_arrPts.GetAt(0);
						m_arrPts.Add(t);
// 						CArray<PT_3DEX,PT_3DEX> arr;
// 						m_pGeoPara->GetShape(arr);
// 						arr.Add(t);
// 						m_pGeoPara->CreateShape(arr.GetData(),arr.GetSize());
					}

					if (m_arrPts.GetSize() <= 1)
					{
						Abort();
					}
					else
						PtReset(pt);
					
					return;
				}
			}
			
			return ;
		}
		PT_3DEX expt(pt,m_nCurPenCode,m_fCurNodeWid,m_nCurNodeType);
		m_arrPts.Add(expt);
		
		if (m_bClosed && m_arrPts.GetSize()>2)
		{
			m_arrPts.Add(m_arrPts[0]);
		}
		
		BOOL bCreate = FALSE;
		if( m_bFastDisplayCurve && m_nCurPenCode==penSpline )
		{
			CArray<PT_3DEX,PT_3DEX> arrtmp;
			arrtmp.Append(m_arrPts);
			for (int i=0; i<arrtmp.GetSize(); i++)
			{
				PT_3DEX &t = arrtmp.ElementAt(i);
				t.pencode = penLine;
			}
			bCreate = m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
		}
		else
			bCreate = m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		
		if (bCreate)
		{
			GrBuffer vbuf;
			const CShapeLine *pSL = m_pGeoPara->GetShape();
			if(m_bClosed)
				pSL->GetVariantGrBuffer(&vbuf,true,m_arrPts.GetSize()-2);
			else
				pSL->GetVariantGrBuffer(&vbuf);
			vbuf.SetAllColor(m_pGeoPara->GetColor());
			//m_pGeoPara->Draw(&vbuf);

			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			
			if( m_bFastDisplayCurve && m_nCurPenCode==penSpline )
			{
				m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			}
		}
		if (m_bClosed && m_arrPts.GetSize()>2)m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
		m_arrPts.RemoveAt(m_arrPts.GetSize()-1);

		m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
	}
	else if( !m_bMouseWidth && !m_bByCenterLine )
	{
		PT_3D dhPt = pt;
		if (!m_bDHeight)
		{
			dhPt.z = m_ptLast.z;
		}
		m_pGeoPara->SetCtrlPoint(0,dhPt);
		double width = ((CGeoParallel*)m_pGeoPara)->GetWidth();
		
		if( width>0 )width = fabs(m_lfSepcialWidth);
		else width = -fabs(m_lfSepcialWidth);
		
		((CGeoParallel*)m_pGeoPara)->SetWidth(width);
		GrBuffer buf;
		m_pGeoPara->Draw(&buf);
		if (m_bCenterlineMode)
		{
			CGeoCurve *pGeo = ((CGeoParallel*)m_pGeoPara)->GetCenterlineCurve();
			if (pGeo)
			{
				pGeo->Draw(&buf);
				delete pGeo;
			}
		}
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		CProcedure::PtMove(pt);
		return;
	}
	else if( m_bMouseWidth && !m_bByCenterLine )
	{
		PT_3D dhPt = pt;
		if (!m_bDHeight)
		{
			dhPt.z = m_ptLast.z;
		}
		m_pGeoPara->SetCtrlPoint(0,dhPt);
		GrBuffer buf;
		m_pGeoPara->Draw(&buf);
		if (m_bCenterlineMode)
		{
			CGeoCurve *pGeo = ((CGeoParallel*)m_pGeoPara)->GetCenterlineCurve();
			if (pGeo)
			{
				pGeo->Draw(&buf);
				delete pGeo;
			}
		}
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		CProcedure::PtMove(pt);
		return;
	}
	else if( m_bMouseWidth && m_bByCenterLine )
	{
		PT_3D dhPt = pt;
		//if (!m_bDHeight)
		{
			dhPt.z = m_ptLast.z;
		}
		GrBuffer buf0, buf;
		m_pGeoPara->SetCtrlPoint(0,dhPt);
		double width = ((CGeoParallel*)m_pGeoPara)->GetWidth();
		//width *= 0.5;
		((CGeoParallel*)m_pGeoPara)->SetWidth(-width);
		
		CGeometry *pObj1 = NULL, *pObj2 = NULL, *pObj3 = NULL;
		int i, num;
		CArray<PT_3DEX,PT_3DEX> arr;
		PT_3DEX *pts;
		if( !m_pGeoPara->Separate(pObj1,pObj2) )
			goto drawparallel_move_error;
		
		pObj3 = m_pGeoPara->Clone();
		if( !pObj3 )
			goto drawparallel_move_error;
		
		
		pObj2->GetShape(arr);
		num = arr.GetSize();
		if(num<=0 ) return ;
		pts = new PT_3DEX[num];
		if(!pts) return;
		for( i=num-1; i>=0; i--)
		{
			pts[num-1-i] = arr[i];
		}
		if(!pObj3->CreateShape(pts,num))return ;
		if (pts) delete []pts;
		
		((CGeoParallel*)pObj3)->SetWidth(width*2);
		
		if( pObj3 )
		{
			pObj3->Draw(&buf);
			buf0.AddBuffer(&buf);
		}

		if (m_bCenterlineMode)
		{
			CGeoCurve *pGeo = ((CGeoParallel*)pObj3)->GetCenterlineCurve();
			if (pGeo)
			{
				pGeo->Draw(&buf);
				buf0.AddBuffer(&buf);
				delete pGeo;
			}
		}
		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf0);
		
drawparallel_move_error:
		
		if( pObj1 )delete pObj1;
		if( pObj2 )delete pObj2;
		if( pObj3 )delete pObj3;
		
		//m_pGeoPara->SetCtrlPoint(0,pt);
		CProcedure::PtMove(pt);
		return;
	}
	else if( !m_bMouseWidth && m_bByCenterLine )
	{
		GrBuffer buf,buf0;

		PT_3D dhPt = pt;
		//if (!m_bDHeight)
		{
			dhPt.z = m_ptLast.z;
		}
		m_pGeoPara->SetCtrlPoint(0,dhPt);

		double width = m_lfSepcialWidth*0.5;
		((CGeoParallel*)m_pGeoPara)->SetWidth(-width);
		
		CGeometry *pObj1 = NULL, *pObj2 = NULL, *pObj3 = NULL;
		int i, num;
		PT_3DEX expt;
		PT_3DEX *pts;
		CArray<PT_3DEX,PT_3DEX> arr;
		if(!m_pGeoPara->Separate(pObj1,pObj2))
			goto drawparallel_move_error1;
		
		pObj3 = m_pGeoPara->Clone();
		if( !pObj3 )
			goto drawparallel_move_error1;
		
		pObj2->GetShape(arr);
		num = arr.GetSize();
		if(num<=0 ) return;
		pts = new PT_3DEX[num];
		if(!pts) return;
		for( i=num-1; i>=0; i--)
		{
			pts[num-1-i] = arr[i];
		}
		if(!pObj3->CreateShape(pts,num))return;
		((CGeoParallel*)pObj3)->SetWidth(width*2);	

		pObj3->Draw(&buf);
		buf0.AddBuffer(&buf);

		if (m_bCenterlineMode)
		{
			CGeoCurve *pGeo = ((CGeoParallel*)pObj3)->GetCenterlineCurve();
			if (pGeo)
			{
				pGeo->Draw(&buf);
				delete pGeo;
			}
		}
		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf0);
		
drawparallel_move_error1:
		
		if( pObj1 )delete pObj1;
		if( pObj2 )delete pObj2;
		if( pObj3 )delete pObj3;

	}
	CProcedure::PtMove(pt);
}

void CDrawParallelProcedure::Finish()
{
	if( m_nCurPenCode==penLine && m_bRectify && m_pGeoPara )
	{
		PT_3DEX cpt,pt1,pt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pGeoPara->GetShape(arr);
		int nPtSum = arr.GetSize();
		if( nPtSum>2 )
		{
			BOOL bClosed = FALSE, bFixPt = FALSE;
			
			//是否闭合，如果闭合，就先去掉尾点
			pt1 = m_pGeoPara->GetDataPoint(0);
			pt4 = m_pGeoPara->GetDataPoint(nPtSum-1);
			if( fabs(pt1.x-pt4.x)<1e-4 && fabs(pt1.y-pt4.y)<1e-4 )
			{
				bClosed = TRUE;
				cpt = pt4;
				arr.RemoveAt(nPtSum-1);
				m_pGeoPara->CreateShape(arr.GetData(),arr.GetSize());
				nPtSum--;
			}
			
			//如果是捕捉时不闭合，就不要再闭合了
			if( (bClosed && m_bOpenIfSnapped && m_nEndSnapped==3) )
			{
				bClosed = FALSE;
			}
			
			//补上一个角点
			if( bClosed && nPtSum>2 && (nPtSum%2==1) )
			{
				PT_3DEX pt2,pt3;
				double dx1,dx2,dy1,dy2,c1,c2,det;
				
				pt1 = m_pGeoPara->GetDataPoint(0);
				pt2 = m_pGeoPara->GetDataPoint(1);
				pt3 = m_pGeoPara->GetDataPoint(nPtSum-2);
				pt4 = m_pGeoPara->GetDataPoint(nPtSum-1);
				
				dx1 = pt2.x-pt1.x;	dy1 = pt2.y-pt1.y;
				dx2 = pt4.x-pt3.x;	dy2 = pt4.y-pt3.y;
				
				c1 = dy2 * pt1.x - dx2 * pt1.y;
				c2 = dy1 * pt4.x - dx1 * pt4.y;
				
				det  = -dy2*dx1 + dy1*dx2;
				
				if( fabs(det)>1e-10 )
				{
					pt4.x = (-c1*dx1 + c2 *dx2)/det;
					pt4.y = (-c1*dy1 + c2*dy2 )/det;
					m_pGeoPara->GetShape(arr);
					arr.Add(pt4);
					m_pGeoPara->CreateShape(arr.GetData(),arr.GetSize());
					bFixPt = TRUE;
				}
			}
			
			if( bClosed )
			{
				m_pGeoPara->GetShape(arr);
				arr.Add(cpt);
				m_pGeoPara->CreateShape(arr.GetData(),arr.GetSize());
			}
			
			double tole = GetProfileDouble(CString(REGPATH_COMMAND)+_T("\\Rectify"),_T("Sigma"),1.0);
			CGeoCurve *pObj = (CGeoCurve*)Rectify(m_pGeoPara,tole,(m_nEndSnapped&1)!=0,(m_nEndSnapped&2)!=0);
			if( pObj )
			{
				m_pGeoPara->CopyFrom(pObj);
// 				delete m_pGeoPara;
// 				m_pGeoPara = pObj;
			}
			//容差太小，直角化不成功
			else if( bFixPt )
			{
				m_pGeoPara->GetShape(arr);
				arr.RemoveAt(nPtSum);
				m_pGeoPara->CreateShape(arr.GetData(),arr.GetSize());
			}
		}
	}
	else
	{
		PT_3DEX cpt,pt1,pt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pGeoPara->GetShape(arr);
		int nPtSum = arr.GetSize();
		if( nPtSum>2 )
		{
			BOOL bClosed = FALSE, bFixPt = FALSE;
			
			//如果是捕捉时不闭合，就不要再闭合了
			pt1 = m_pGeoPara->GetDataPoint(0);
			pt4 = m_pGeoPara->GetDataPoint(nPtSum-1);
			if( fabs(pt1.x-pt4.x)<1e-4 && fabs(pt1.y-pt4.y)<1e-4
				&& m_bOpenIfSnapped && m_nEndSnapped==3 )
			{
				m_pGeoPara->GetShape(arr);
				arr.RemoveAt(arr.GetSize()-1);
				m_pGeoPara->CreateShape(arr.GetData(),arr.GetSize());
			}
		}
	}
	CProcedure::Finish();
}

 void CDrawParallelProcedure::PtReset(PT_3D &pt)
{
	if( !m_pGeoPara || IsProcFinished(this) )return;

	if (!m_bDrawWidth)
	{
		m_bDrawWidth = TRUE;

		PutStreamPts();		
		
		//采集后光滑一次
		if( m_bDoSmooth && m_pGeoPara!=NULL )
		{
			CArray<PT_3DEX,PT_3DEX> arrPts1;
			PT_3DEX expt;
			//m_pGeoPara->GetShape(arrPts1);
			arrPts1.Copy(m_arrPts);
			int sum = arrPts1.GetSize();
			for( int i=0; i<sum; i++)
			{
				expt = arrPts1[i];
				if( expt.pencode!=penStream )break;
			}
			
			if( i>=sum && sum>3 )
			{				
				//弦高压缩
				PT_3D pt3d;
				CStreamCompress5_PT_3DEX comp;
				comp.BeginCompress(m_compress.GetLimit()*m_compress.m_lfScale);
				
				for( i=0; i<sum; i++)
				{
					expt = arrPts1[i];
					COPY_3DPT(pt3d,expt);
					comp.AddStreamPt(pt3d);
					
					arrPts1.RemoveAt(sum-1-i);				
				}
								
				comp.EndCompress();
				
				//获得压缩点
				CArray<PT_3DEX,PT_3DEX> arrPts, arrRets;
				{
					PT_3DEX *tpts;
					int ti, tnpt;
					comp.GetPts(tpts,tnpt);
					arrPts.SetSize(tnpt);
					for( ti=0; ti<tnpt; ti++)
					{
						COPY_3DPT(expt,tpts[ti]);
						arrPts.SetAt(ti,expt);
					}
					
					//光滑
					SmoothLines(arrPts.GetData(),tnpt,m_compress.GetLimit()*m_compress.m_lfScale,arrRets);
				}
				
				m_arrPts.Copy(arrRets);
			}
		}
	}

	int size = m_arrPts.GetSize();
	if (size<2)
	{
		Abort();
		return;
	}
	if (m_bClosed && size > 2)
	{
		m_arrPts.Add(m_arrPts.GetAt(0));
	}
	if( m_pGeoPara->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize()) )
	{
		m_pEditor->UpdateDrag(ud_ClearDrag);
		
	}
	else
		Abort();
}


void CDrawParallelProcedure::Abort()
{
	m_arrPts.RemoveAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CProcedure::Abort();
}


void CDrawParallelProcedure::Back()
{
	if (m_pGeoPara)
	{
		long color = m_pGeoPara->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}
		
		if( m_nCurPenCode==penStream )
		{
			PutStreamPts();
			if( m_arrPts.GetSize()<=1 )
			{
				Abort();
				return;
			}
			
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			{
				PT_3DEX t0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);				
				PT_3D t1;
				COPY_3DPT(t1,t0);
				m_compress.BeginCompress();
				m_compress.AddStreamPt(t1);
				
				GrBuffer buf;
				buf.BeginLineString(color,0);
				buf.MoveTo(&t1);
				buf.LineTo(&t1);
				buf.End();
				
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
				buf.DeleteAll();
				buf.BeginLineString(color,0);
				buf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
				buf.End();
				m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
				m_pEditor->SetCrossPos(t1);
			//	m_pEditor->UpdateDrag(ud_ClearDrag);
				m_pEditor->RefreshView();
			}
			
			return;
		}
		else 
		{
			if(m_arrPts.GetSize()<2)
			{
				m_pGeoPara->CreateShape(&PT_3DEX(),1);
				Abort();
				return;
			}
			PT_3DEX pt0;
			PT_3D pt1;
			CArray<PT_3DEX,PT_3DEX> pts;
// 			pts.Copy(m_arrPts);
// 			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			pts.Copy(m_arrPts);
			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			COPY_3DPT(pt1,pt0);	

			GrBuffer cbuf,vbuf;
			if (m_bClosed && m_arrPts.GetSize()>2)
			{
				pts.Add(pts[0]);
			}
			m_pGeoPara->CreateShape(pts.GetData(),pts.GetSize());
			//	m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			
			m_pGeoPara->GetShape()->GetConstGrBuffer(&cbuf,m_bClosed);
			cbuf.SetAllColor(color);
			if(m_bClosed)
			{
				if (m_arrPts.GetSize()>2)
				{
					m_pGeoPara->GetShape()->GetVariantGrBuffer(&vbuf,true,pts.GetSize()-1);
				}
			}	
			// 只有样条有临时线
			else if (m_nCurPenCode == penSpline)
			{
				CArray<PT_3DEX,PT_3DEX> arrtmp;
				arrtmp.Append(pts);
				if( m_bFastDisplayCurve )
				{
					for (int i=0; i<arrtmp.GetSize(); i++)
					{
						PT_3DEX &t = arrtmp.ElementAt(i);
						t.pencode = penLine;
					}
//					m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				}
				arrtmp.Add(arrtmp.GetAt(arrtmp.GetSize()-1));
				m_pGeoPara->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
				m_pGeoPara->GetShape()->GetVariantGrBuffer(&vbuf);

				m_pGeoPara->CreateShape(pts.GetData(),pts.GetSize());
// 				arrtmp.RemoveAt(arrtmp.GetSize()-1);
// 				m_pGeoCurve->CreateShape(arrtmp.GetData(),arrtmp.GetSize());
// 				
// 				if( m_bFastDisplayCurve )
// 				{
// 					m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
// 				}
				
			}
				
			
			m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_pEditor->SetCrossPos(pt1);
			//	m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();
		}
		
	}

/*	if (m_pGeoPara)
	{	
		
		if(m_arrPts.GetSize()<2)
		{
			Abort();
			return;
		}
		
		CArray<PT_3DEX,PT_3DEX> pts;
		m_pGeoPara->GetShape(pts);
		pts.RemoveAt(pts.GetSize()-2);
		m_pGeoPara->CreateShape(pts.GetData(),pts.GetSize());
		//	m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		m_arrPts.RemoveAt(m_arrPts.GetSize()-1);		
		GrBuffer cbuf,vbuf;
		m_pGeoPara->GetShape()->GetConstGrBuffer(&cbuf);
		cbuf.SetAllColor(m_pGeoPara->GetColor());
		m_pGeoPara->GetShape()->GetVariantGrBuffer(&vbuf,m_pGeoPara->IsClosed());
		//	m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		m_pEditor->RefreshView();
	}
*/
}


void CDrawParallelProcedure::GetParams(CValueTable& tab)
{
	CProcedure::GetParams(tab);
}


void CDrawParallelProcedure::SetParams(CValueTable& tab,BOOL bInit)
{
	
	CProcedure::SetParams(tab,bInit);
}
void CDrawParallelProcedure::Start()
{
//	m_pGeoPara = new CGeoParallel;
// 	if( m_pGeoPara )
// 		m_pGeoPara->EnableClose(m_bClosed);

	GotoState(PROCSTATE_READY);
	
	m_bDrawWidth = FALSE;
	m_bMouseWidth = TRUE;
	m_bByCenterLine = FALSE;
	m_lfSepcialWidth = 1.0;

	m_bClosed = FALSE;
	m_bRectify = FALSE;
	m_compress.SetLimit(0.1);
	m_bDoSmooth = FALSE;
	m_bFastDisplayCurve = FALSE;
	m_arrPts.RemoveAll();

	m_bCenterlineMode = FALSE;
	m_strRetLayer = "";

	m_bDHeight = FALSE;
	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	m_pGeoCurve->WriteTo(tab);
// 	tab.EndAddValueItem();
	
	
	m_pEditor->CloseSelector();
}

void CDrawParallelProcedure::TrackLine()
{
	GotoState(PROCSTATE_PROCESSING);

	PT_3DEX curPt(m_curPt,penLine), retPt(m_curPt,penLine);
	if(m_bTrack)
	{
		m_trackPts.Add(retPt);
		int sum = GraphAPI::GKickoffSamePoints(m_trackPts.GetData(),m_trackPts.GetSize());
		m_trackPts.SetSize(sum);
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pGeoPara->GetColor(),0);
		cbuf.Lines(m_trackPts.GetData(), sum, sizeof(PT_3DEX));
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);

		for(int i=0; i<m_trackPts.GetSize(); i++)
		{
			m_trackPts[i].pencode = penLine;
		}
		
		m_arrPts.Append(m_trackPts);
		m_bTrack = FALSE;
		m_trackPts.RemoveAll();
		return;
	}

	PT_3D cltpt;	
	(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_curPt,&cltpt);
	m_pEditor->GetDataQuery()->FindNearestObject(cltpt,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS(),
				m_pEditor->GetCoordWnd().m_pSearchCS);
	int num;
	const CPFeature *ftrs = (m_pEditor->GetDataQuery())->GetFoundHandles(num);
	if(num<=0) return;

	CFeature *pFtr = NULL;
	for(int k=0; k<num; k++)
	{
		CGeometry *pGeo = ftrs[k]->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			continue;
		}
		pFtr = ftrs[k];
		break;
	}
	if(pFtr==NULL)  return;
	CGeometry *pGeo = pFtr->GetGeometry();
	int geoType = pGeo->GetClassType();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int trackPart = GetCurvePartPts(m_curPt, pGeo, arrPts);

	// 线串化
	for (int i=0; i<arrPts.GetSize(); i++)
	{
		arrPts[i].pencode = penLine;
	}

	if (arrPts.GetSize() < 2) return;

	if (geoType == CLS_GEOSURFACE)
	{
		//arrPts.Add(arrPts[0]);
	}

	int index = -1;
	GraphAPI::GGetNearstDisPToPointSet2D(arrPts.GetData(),arrPts.GetSize(),curPt,retPt,&index);
	retPt.z = m_curPt.z;

	m_bTrack = TRUE;
	m_trackHandle = FtrToHandle(pFtr);
	m_nLastTrackIndex = index;
	m_nLastTrackPt = retPt;
	m_TrackPart = trackPart;

	retPt.pencode = arrPts[index].pencode;
	m_trackPts.Add(retPt);
	int nPt = m_arrPts.GetSize();
	if (m_arrPts.GetSize() > 0)
	{
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pGeoPara->GetColor(),0);
		cbuf.MoveTo(&m_arrPts[nPt-1]);
		cbuf.LineTo(&m_trackPts[0]);
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawSurfaceProcedure::CDrawSurfaceProcedure()
{
	m_pGeoSurface = NULL;
	m_nCurPenCode = penLine;
	m_lfToler=0;
	m_bRectify = FALSE;
	m_bIsRef = FALSE;
	m_fCurNodeWid = 0;
	m_nCurNodeType = ptNone;

	m_bTrack = FALSE;
	m_trackHandle = 0;
	m_nLastTrackIndex = -1;
	m_TrackPart = 0;
	m_bSnap2D = TRUE;
}


CDrawSurfaceProcedure::~CDrawSurfaceProcedure()
{
/*	if( m_pGeoSurface )delete m_pGeoSurface;*/
}



VOID CDrawSurfaceProcedure::PtClick(PT_3D &pt, int flag)
{
	if(m_bTrack)
	{
		TrackLine();
		return;
	}

	if(!m_pGeoSurface)
		return;
	GotoState(PROCSTATE_PROCESSING);

 	PutStreamPts();

	PT_3DEX expt(pt,m_nCurPenCode,m_fCurNodeWid,m_nCurNodeType);
	m_arrPts.Add(expt);
	
	m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());

	long color = m_pGeoSurface->GetColor();
	if (color == -1)
	{
		color = m_layCol;
	}
	
	GrBuffer cbuf,vbuf;
	CShapeLine *pSL = const_cast<CShapeLine*>(m_pGeoSurface->GetShape());

	CShapeLine shape;
	if (m_arrPts.GetSize() <= 2)
	{
		CLinearizer line;
		line.SetShapeBuf(&shape);
		line.Linearize(m_arrPts.GetData(),m_arrPts.GetSize(),TRUE);
		
		shape.LineEnd();
		
		pSL = &shape;
	}
///*
	m_bIsRef = FALSE;
	if(  m_nCurPenCode!=penStream && m_pGeoSurface->GetDataPointSum()>=3 )
	{
		Envelope e;
		CFeature  *objhandle = NULL,*objhandle1 = NULL;
		e.m_xh=pt.x+m_lfToler;
		e.m_xl=pt.x-m_lfToler;
		e.m_yh=pt.y+m_lfToler;
		e.m_yl=pt.y-m_lfToler;
		PT_3DEX t0 = m_pGeoSurface->GetDataPoint(0);
	
		//如果首点末点都在某个线串或面上则共线引用
		PT_3D t1, t2;
		if (m_lfToler>0)
		{
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&t0,&t1);
			objhandle1 = m_pEditor->GetDataQuery()->FindNearestObject(t1,m_lfToler,m_pEditor->GetCoordWnd().m_pSearchCS);
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&t2);
			objhandle = m_pEditor->GetDataQuery()->FindNearestObject(t2,m_lfToler,m_pEditor->GetCoordWnd().m_pSearchCS);			
		}
		
		if (m_lfToler>0 && objhandle != NULL && objhandle1 != NULL && objhandle == objhandle1)
		{
			CGeometry *pGeo = objhandle->GetGeometry();
			if (!pGeo)
			{
				Abort();
				return;
			}
			if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))&&!pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				goto escaperef0;
			}
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			int ptsum = arrPts.GetSize();
			PT_3D *pt3d = new PT_3D[ptsum];
			if (!pt3d)  return;
			for(int i=0;i<ptsum;i++)
			{
				COPY_3DPT(pt3d[i],arrPts[i]);
			}
			ptsum = GraphAPI::GKickoffSamePoints(pt3d,ptsum);
			PT_2D *pt2d=new PT_2D[ptsum];
			for( i=0;i<ptsum;i++)
			{
				COPY_2DPT(pt2d[i],pt3d[i]);
			}
			PT_2D pt0,pt1,pt2;
			int index1,index2;
			COPY_2DPT(pt0,t0);
			double dis=GraphAPI::GGetNearstDisPToPointSet2D(pt2d,ptsum,pt0,pt1,&index1);//首点
			COPY_2DPT(pt0,pt);
			double dis1=GraphAPI::GGetNearstDisPToPointSet2D(pt2d,ptsum,pt0,pt2,&index2);//尾点
			
			if (dis<m_lfToler&&dis1<m_lfToler)
			{
				m_arrRefPts.Copy(m_arrPts);
				m_bIsRef = TRUE;
				if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))&&!((CGeoCurve*)pGeo)->IsClosed())
				{	
					
					double z=(pt3d[index1+1].z-pt3d[index1].z)*(pt1.x-pt2d[index1].x)/(pt2d[index1+1].x-pt2d[index1].x)+pt3d[index1].z;
					PT_3DEX ptex;
					ptex.x=pt1.x;
					ptex.y=pt1.y;
					ptex.z=z;
					ptex.pencode=m_nCurPenCode;
					m_arrRefPts.SetAt(0,ptex);
					z=(pt3d[index2+1].z-pt3d[index2].z)*(pt2.x-pt2d[index2].x)/(pt2d[index2+1].x-pt2d[index2].x)+pt3d[index2].z;
					ptex.x=pt2.x;
					ptex.y=pt2.y;
					ptex.z=z;
					ptex.pencode=m_nCurPenCode;
					m_arrRefPts.SetAt(m_arrRefPts.GetSize()-1,ptex);
					if(index1>=index2)
					{
//						m_nDelPtNum=0;
						for (int j=index2+1;j<=index1;j++)
						{
//							m_nDelPtNum++;
							COPY_3DPT(ptex,pt3d[j]);
							ptex.pencode=m_nCurPenCode;
							m_arrRefPts.Add(ptex);
						}
					}
					if (index1<index2)
					{
//						m_nDelPtNum=0;
						for (int k=index2;k>index1;k--)
						{
//							m_nDelPtNum++;	
							COPY_3DPT(ptex,pt3d[k]);
							ptex.pencode=m_nCurPenCode;
							m_arrRefPts.Add(ptex);
						}
					}
					
				}
				else
				{
				
					if(GraphAPI::GGetAllLen2D(pt2d,ptsum)>2*(GraphAPI::GGet2DDisOf2P(pt2,pt2d[(index1<index2)?index2:(index2+1)])+GraphAPI::GGet2DDisOf2P(pt1,pt2d[(index1<index2)?(index1+1):index1])+GraphAPI::GGetAllLen2D(&pt2d[(index1>index2?index2:index1)+1],abs(index2-index1))))//有待修改
					{
//						m_nIdxDif=abs(index2-index1);
						
						double z=(pt3d[index1+1].z-pt3d[index1].z)*(pt1.x-pt2d[index1].x)/(pt2d[index1+1].x-pt2d[index1].x)+pt3d[index1].z;
						PT_3DEX ptex;
						ptex.x=pt1.x;
						ptex.y=pt1.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						m_arrRefPts.SetAt(0,ptex);
						z=(pt3d[index2+1].z-pt3d[index2].z)*(pt2.x-pt2d[index2].x)/(pt2d[index2+1].x-pt2d[index2].x)+pt3d[index2].z;
						ptex.x=pt2.x;
						ptex.y=pt2.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						m_arrRefPts.SetAt(m_arrRefPts.GetSize()-1,ptex);
						if(index1>index2)
						{
//							m_nDelPtNum=0;
							for (int j=index2+1;j<=index1;j++)
							{
//								m_nDelPtNum++;
								COPY_3DPT(ptex,pt3d[j]);
								ptex.pencode=m_nCurPenCode;
								m_arrRefPts.Add(ptex);
							}
						}
						if (index1<index2)
						{
//							m_nDelPtNum=0;
							for (int k=index2;k>index1;k--)
							{
//								m_nDelPtNum++;
								COPY_3DPT(ptex,pt3d[k]);
								ptex.pencode=m_nCurPenCode;
								m_arrRefPts.Add(ptex);
							}
						}
						
					}
					else
					{
//						m_nIdxDif=abs(index2-index1);
						
						double z=(pt3d[index1+1].z-pt3d[index1].z)*(pt1.x-pt2d[index1].x)/(pt2d[index1+1].x-pt2d[index1].x)+pt3d[index1].z;
						PT_3DEX ptex;
						ptex.x=pt1.x;
						ptex.y=pt1.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						m_arrRefPts.SetAt(0,ptex);

						z=(pt3d[index2+1].z-pt3d[index2].z)*(pt2.x-pt2d[index2].x)/(pt2d[index2+1].x-pt2d[index2].x)+pt3d[index2].z;
						ptex.x=pt2.x;
						ptex.y=pt2.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						m_arrRefPts.SetAt(m_arrRefPts.GetSize()-1,ptex);
						if(index1>index2)
						{
//							m_nDelPtNum=0;
							for (int j=index2;j!=index1;)
							{
//								m_nDelPtNum++;
								COPY_3DPT(ptex,pt3d[j]);
								ptex.pencode=m_nCurPenCode;
								m_arrRefPts.Add(ptex);
								j--;
								if(j==-1) j=ptsum-2;
							}
						}
						if (index1<index2)
						{
//							m_nDelPtNum=0;
							for (int k=index2+1;k!=index1+1;)
							{
//								m_nDelPtNum++;	
								COPY_3DPT(ptex,pt3d[k]);
								ptex.pencode=m_nCurPenCode;
								m_arrRefPts.Add(ptex);
								k++;
								if (k==ptsum)
								{
									k=1;
								}
							}
						}
					}
				}
				
				
			}
			
			if (pt2d)
			{
				delete []pt2d;
				pt2d=NULL;
			}
			if (pt3d)
			{
				delete []pt3d;
				pt3d=NULL;
			}
				
		}
	}
escaperef0:	
		
//*/	
	
	CArray<PT_3DEX,PT_3DEX> savePts;
	// 要增加显示动态线的个数
	int vnum = 0;		
	if (m_bIsRef)
	{
		vnum = m_arrRefPts.GetSize() - m_arrPts.GetSize();
		savePts.Copy(m_arrPts);
		m_arrPts.Copy(m_arrRefPts);
	}
	if (m_nCurPenCode == penLine)
	{
		GrBuffer *pcbuf = NULL;

		if(m_arrPts.GetSize() >= 3 )
		{
			pSL->SetLastPtcd(m_nCurPenCode);
			pSL->GetAddConstGrBuffer(&cbuf,false);
			m_arrPts.Add(m_arrPts[0]);

			//if (m_bIsRef)
			{
				m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
				pSL->SetLastPtcd(m_nCurPenCode);
			}

			pSL->GetVariantGrBuffer(&vbuf,false,m_arrPts.GetSize()-2-vnum);

			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);

			if (m_bIsRef)
			{
				m_pGeoSurface->CreateShape(savePts.GetData(),savePts.GetSize());
			}
			pSL->SetLastPtcd(penNone);
		}
		else
		{
			pSL->GetAddConstGrBuffer(&cbuf);
		}

		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
		
	}
	else if (m_nCurPenCode == penSpline || m_nCurPenCode == penArc || m_nCurPenCode==pen3PArc )
	{
		GrBuffer *pvbuf = NULL;
		if (1)//m_bClosed)
		{

			if(m_arrPts.GetSize() >= 3 )
			{
				pSL->SetLastPtcd(m_nCurPenCode);
				pSL->GetAddConstGrBuffer(&cbuf,false);
				m_arrPts.Add(m_arrPts[0]);
				//if (m_bIsRef)
				{
					m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
					pSL->SetLastPtcd(m_nCurPenCode);
				}

				pSL->GetVariantGrBuffer(&vbuf,TRUE,m_arrPts.GetSize()-2-vnum);
				m_arrPts.RemoveAt(m_arrPts.GetSize()-1);

				if (m_bIsRef)
				{
					m_pGeoSurface->CreateShape(savePts.GetData(),savePts.GetSize());
				}
				pSL->SetLastPtcd(penNone);
			}
			else
				pSL->GetVariantGrBuffer(&vbuf);

			pvbuf = &vbuf;
			
		}
		else
		{
			pSL->GetAddConstGrBuffer(&cbuf);

			if (m_nCurPenCode == penSpline)
			{
				m_arrPts.Add(expt);
				m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			}

			pSL->GetVariantGrBuffer(&vbuf);
			pvbuf = &vbuf;

			if (m_nCurPenCode == penSpline)
			{
				m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
				m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			}

		}

		m_pEditor->UpdateDrag(ud_SetVariantDrag,pvbuf);
		cbuf.SetAllColor(color);
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
	}
	else if(m_nCurPenCode == penStream)
	{
		m_compress.BeginCompress();
		m_compress.AddStreamPt(pt);
		
		if( m_arrPts.GetSize()>=2 )
		{
			PT_3DEX t0;
			t0 = m_arrPts.GetAt(m_arrPts.GetSize()-2);
			PT_3D tpt;
			COPY_3DPT(tpt,t0);
			GrBuffer addbuf;
			addbuf.BeginLineString(m_pGeoSurface->GetColor(),0);		
			addbuf.MoveTo(&tpt);
			addbuf.LineTo(&pt);
			addbuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
		}
			
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);

	}

	if (m_bIsRef)
	{
		m_arrPts.Copy(savePts);
	}

}


void CDrawSurfaceProcedure::PtMove(PT_3D &pt)
{
	m_curPt = pt;
	//追踪
	if (m_bTrack)
	{
		PT_3D cltpt;	
		(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_curPt,&cltpt);
		m_pEditor->GetDataQuery()->FindNearestObject(cltpt,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS(),
			m_pEditor->GetCoordWnd().m_pSearchCS);

		int num;
		const CPFeature *ftrs = (m_pEditor->GetDataQuery())->GetFoundHandles(num);

		BOOL bSameFtr = FALSE;
		int m;
		for(m=0; m<num; m++)
		{
			if(ftrs[m]==HandleToFtr(m_trackHandle))
			{
				bSameFtr = TRUE;
				break;
			}
		}
		
		PT_3DEX curPt(m_curPt,penLine), retPt(m_curPt,penLine);

		CGeometry *pGeo = HandleToFtr(m_trackHandle)->GetGeometry();
		CArray<PT_3DEX, PT_3DEX> pts1, pts2;
		if (bSameFtr)
		{
			int trackPart = GetCurvePartPts(m_curPt, pGeo, pts1);
			if(trackPart!=m_TrackPart)
			{
				bSameFtr = FALSE;
			}
			if(trackPart<0) goto trackend;
		}

		if(num<=0) goto trackend;
		
		if (bSameFtr)
		{
			int index = -1;
			GraphAPI::GGetNearstDisPToPointSet2D(pts1.GetData(),pts1.GetSize(),curPt,retPt,&index);
			curPt.z = retPt.z = m_curPt.z;

			PT_3DEX pt0 = m_trackPts[m_trackPts.GetSize()-1];
			if(GraphAPI::GIsEqual2DPoint(&curPt, &pt0))
			{
				;//如果咬合到最后一个点，则不处理
			}
			else if (index >= 0)
			{
				int pos = IsPtInCurve(curPt, m_trackPts);
				BOOL bSamePt = GraphAPI::GIsEqual2DPoint(&curPt, &m_trackPts[0]);
				if(pos>=0 && !bSamePt)
				{
					int cutnum = m_trackPts.GetSize()-pos-1;
					m_trackPts.RemoveAt(pos+1, cutnum);//回退
				}
				else
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					GetPartLineOf2Pt(pts1.GetData(),pts1.GetSize(),m_nLastTrackPt,m_nLastTrackIndex,retPt,index,arr);
					if(m_bSnap2D)//3D时保持高程为地物高程 2D则为测标高程
					{
						for (int i=0; i<arr.GetSize(); i++)
						{
							arr[i].z = m_curPt.z;
						}
					}
					if(arr.GetSize()>1 && IsPtInCurve(arr[1], m_trackPts)>=0)
					{
						goto trackend;//阻止调换方向，若需要换方向必须先回退到方向交叉点
					}
					m_trackPts.Append(arr);
				}
				m_nLastTrackIndex = index;
				m_nLastTrackPt = retPt;
			}
		}
		else//转移到另一个地物,仅补全m_nLastTrackPt到交点的部分，若有多个交点，取鼠标最近的
		{
			GetCurvePartPts(m_TrackPart, pGeo, pts1);
			for(int k=0; k<num; k++)
			{
				pGeo = ftrs[k]->GetGeometry();
				if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}

				int trackPart = GetCurvePartPts(m_curPt, pGeo, pts2);
				if(trackPart<0)
				{
					continue;
				}
				
				double t1, t2;
				PT_3DEX retpt1;//交点
				COPY_3DPT(retpt1, pt);
				retpt1.pencode = penLine;
				if(!GetCurveIntersectCurve(pts1.GetData(), pts1.GetSize(), pts2.GetData(), pts2.GetSize(), retpt1, &t1, &t2))
				{
					continue;
				}

				int pos = IsPtInCurve(retpt1, m_trackPts);
				BOOL bSamePt = GraphAPI::GIsEqual2DPoint(&retpt1, &m_trackPts[0]);
				
				if(pos>0 && !bSamePt)
				{
					int cutnum = m_trackPts.GetSize()-pos-1;
					m_trackPts.RemoveAt(pos+1, cutnum);
				}
				else
				{
					CArray<PT_3DEX, PT_3DEX> arr;
					GetPartLineOf2Pt(pts1.GetData(),pts1.GetSize(),m_nLastTrackPt,m_nLastTrackIndex,retpt1,(int)t1,arr);//首尾点重复
					if(m_bSnap2D)
					{
						for (int i=0; i<arr.GetSize(); i++)
						{
							arr[i].z = m_curPt.z;
						}
					}
					if(arr.GetSize()>1 && IsPtInCurve(arr[1], m_trackPts)>=0)
					{
						continue;//阻止调换方向，若需要换方向必须先回退到方向交叉点
					}
					m_trackPts.Append(arr);
				}
				m_trackPts.Add(retpt1);
				m_nLastTrackIndex = (int)(t2);
				m_nLastTrackPt = retpt1;
				m_trackHandle = FtrToHandle(ftrs[k]);
				m_TrackPart  = trackPart;
				break;
			}
		}
trackend:
		GrBuffer trackbuf;
		CArray<PT_3DEX,PT_3DEX> arr;	
		arr.Append(m_trackPts);
		arr.Add(retPt);		
		
		trackbuf.BeginLineString(RGB(255,0,0)/*m_pGeoCurve->GetColor()*/,0);
		for (int i=0; i<arr.GetSize(); i++)
		{
			if (i == 0)
			{
				trackbuf.MoveTo(&arr[i]);
			}
			else
			{
				trackbuf.LineTo(&arr[i]);
			}
		}
		if(1)
		{
			CArray<PT_3DEX,PT_3DEX> allPts;
			allPts.Copy(m_arrPts);
			allPts.Append(m_trackPts);
			if(allPts.GetSize()>0)
			{
				trackbuf.MoveTo(&allPts[0]);
				trackbuf.LineTo(&curPt);
			}
		}
		trackbuf.End();

		m_pEditor->UpdateDrag(ud_SetVariantDrag,&trackbuf);
		return;
	}//追踪

	if (m_arrPts.GetSize()<=0||!m_pGeoSurface)
	{
		return;
	}
	if( m_nCurPenCode==penStream )
	{
		long color = m_pGeoSurface->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}

		GrBuffer addbuf;
		addbuf.BeginLineString(color,0);
		
		if( 1 )
		{
			PT_3D tpt;
			m_compress.GetCurCompPt(tpt);
			m_compress.AddStreamPt(pt);
			
			addbuf.MoveTo(&tpt);
			addbuf.LineTo(&pt);
			addbuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag,&addbuf);
		}
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
 		return ;
	}

	PT_3DEX expt(pt,m_nCurPenCode,m_fCurNodeWid,m_nCurNodeType);
	m_arrPts.Add(expt);

	CArray<PT_3DEX,PT_3DEX> arrtmpRefPts;
	if (m_arrPts.GetSize() >= 3)
	{
		Envelope e;
		CFeature  *objhandle = NULL,*objhandle1 = NULL;
		e.m_xh=pt.x+m_lfToler;
		e.m_xl=pt.x-m_lfToler;
		e.m_yh=pt.y+m_lfToler;
		e.m_yl=pt.y-m_lfToler;
		PT_3DEX t0 = m_pGeoSurface->GetDataPoint(0);
	
		//如果首点末点都在某个线串或面上则共线引用
		PT_3D t1, t2;
		if (m_lfToler>0)
		{
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&t0,&t1);
			objhandle1 = m_pEditor->GetDataQuery()->FindNearestObject(t1,m_lfToler,m_pEditor->GetCoordWnd().m_pSearchCS);
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&t2);
			objhandle = m_pEditor->GetDataQuery()->FindNearestObject(t2,m_lfToler,m_pEditor->GetCoordWnd().m_pSearchCS);			
		}
		
		if (m_lfToler>0 && objhandle != NULL && objhandle1 != NULL && objhandle == objhandle1)
		{
			CGeometry *pGeo = objhandle->GetGeometry();
			if (!pGeo)
			{
				Abort();
				return;
			}
			if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))&&!pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				goto escaperef0;
			}
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			int ptsum = arrPts.GetSize();
			PT_3D *pt3d = new PT_3D[ptsum];
			if (!pt3d)  return;
			for(int i=0;i<ptsum;i++)
			{
				COPY_3DPT(pt3d[i],arrPts[i]);
			}
			ptsum = GraphAPI::GKickoffSamePoints(pt3d,ptsum);
			PT_2D *pt2d=new PT_2D[ptsum];
			for( i=0;i<ptsum;i++)
			{
				COPY_2DPT(pt2d[i],pt3d[i]);
			}
			PT_2D pt0,pt1,pt2;
			int index1,index2;
			COPY_2DPT(pt0,t0);
			double dis=GraphAPI::GGetNearstDisPToPointSet2D(pt2d,ptsum,pt0,pt1,&index1);//首点
			COPY_2DPT(pt0,pt);
			double dis1=GraphAPI::GGetNearstDisPToPointSet2D(pt2d,ptsum,pt0,pt2,&index2);//尾点
//			m_bIsRef=FALSE;
			if (dis<m_lfToler&&dis1<m_lfToler)
			{
				arrtmpRefPts.Copy(m_arrPts);
//				m_bIsRef=TRUE;
				if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))&&!((CGeoCurve*)pGeo)->IsClosed())
				{	
					
					double z=(pt3d[index1+1].z-pt3d[index1].z)*(pt1.x-pt2d[index1].x)/(pt2d[index1+1].x-pt2d[index1].x)+pt3d[index1].z;
					PT_3DEX ptex;
					ptex.x=pt1.x;
					ptex.y=pt1.y;
					ptex.z=z;
					ptex.pencode=m_nCurPenCode;
					arrtmpRefPts.SetAt(0,ptex);
					z=(pt3d[index2+1].z-pt3d[index2].z)*(pt2.x-pt2d[index2].x)/(pt2d[index2+1].x-pt2d[index2].x)+pt3d[index2].z;
					ptex.x=pt2.x;
					ptex.y=pt2.y;
					ptex.z=z;
					ptex.pencode=m_nCurPenCode;
					arrtmpRefPts.SetAt(arrtmpRefPts.GetSize()-1,ptex);
					if(index1>=index2)
					{
//						m_nDelPtNum=0;
						for (int j=index2+1;j<=index1;j++)
						{
//							m_nDelPtNum++;
							COPY_3DPT(ptex,pt3d[j]);
							ptex.pencode=m_nCurPenCode;
							arrtmpRefPts.Add(ptex);
						}
					}
					if (index1<index2)
					{
//						m_nDelPtNum=0;
						for (int k=index2;k>index1;k--)
						{
//							m_nDelPtNum++;	
							COPY_3DPT(ptex,pt3d[k]);
							ptex.pencode=m_nCurPenCode;
							arrtmpRefPts.Add(ptex);
						}
					}
					
				}
				else
				{
				
					if(GraphAPI::GGetAllLen2D(pt2d,ptsum)>2*(GraphAPI::GGet2DDisOf2P(pt2,pt2d[(index1<index2)?index2:(index2+1)])+GraphAPI::GGet2DDisOf2P(pt1,pt2d[(index1<index2)?(index1+1):index1])+GraphAPI::GGetAllLen2D(&pt2d[(index1>index2?index2:index1)+1],abs(index2-index1))))//有待修改
					{
//						m_nIdxDif=abs(index2-index1);
						
						double z=(pt3d[index1+1].z-pt3d[index1].z)*(pt1.x-pt2d[index1].x)/(pt2d[index1+1].x-pt2d[index1].x)+pt3d[index1].z;
						PT_3DEX ptex;
						ptex.x=pt1.x;
						ptex.y=pt1.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						arrtmpRefPts.SetAt(0,ptex);
						z=(pt3d[index2+1].z-pt3d[index2].z)*(pt2.x-pt2d[index2].x)/(pt2d[index2+1].x-pt2d[index2].x)+pt3d[index2].z;
						ptex.x=pt2.x;
						ptex.y=pt2.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						arrtmpRefPts.SetAt(arrtmpRefPts.GetSize()-1,ptex);
						if(index1>index2)
						{
//							m_nDelPtNum=0;
							for (int j=index2+1;j<=index1;j++)
							{
//								m_nDelPtNum++;
								COPY_3DPT(ptex,pt3d[j]);
								ptex.pencode=m_nCurPenCode;
								arrtmpRefPts.Add(ptex);
							}
						}
						if (index1<index2)
						{
//							m_nDelPtNum=0;
							for (int k=index2;k>index1;k--)
							{
//								m_nDelPtNum++;
								COPY_3DPT(ptex,pt3d[k]);
								ptex.pencode=m_nCurPenCode;
								arrtmpRefPts.Add(ptex);
							}
						}
						
					}
					else
					{
//						m_nIdxDif=abs(index2-index1);
						
						double z=(pt3d[index1+1].z-pt3d[index1].z)*(pt1.x-pt2d[index1].x)/(pt2d[index1+1].x-pt2d[index1].x)+pt3d[index1].z;
						PT_3DEX ptex;
						ptex.x=pt1.x;
						ptex.y=pt1.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						arrtmpRefPts.SetAt(0,ptex);

						z=(pt3d[index2+1].z-pt3d[index2].z)*(pt2.x-pt2d[index2].x)/(pt2d[index2+1].x-pt2d[index2].x)+pt3d[index2].z;
						ptex.x=pt2.x;
						ptex.y=pt2.y;
						ptex.z=z;
						ptex.pencode=m_nCurPenCode;
						arrtmpRefPts.SetAt(arrtmpRefPts.GetSize()-1,ptex);
						if(index1>index2)
						{
//							m_nDelPtNum=0;
							for (int j=index2;j!=index1;)
							{
//								m_nDelPtNum++;
								COPY_3DPT(ptex,pt3d[j]);
								ptex.pencode=m_nCurPenCode;
								arrtmpRefPts.Add(ptex);
								j--;
								if(j==-1) j=ptsum-2;
							}
						}
						if (index1<index2)
						{
//							m_nDelPtNum=0;
							for (int k=index2+1;k!=index1+1;)
							{
//								m_nDelPtNum++;	
								COPY_3DPT(ptex,pt3d[k]);
								ptex.pencode=m_nCurPenCode;
								arrtmpRefPts.Add(ptex);
								k++;
								if (k==ptsum)
								{
									k=1;
								}
							}
						}
					}
				}
				
				
			}
			
			if (pt2d)
			{
				delete []pt2d;
				pt2d=NULL;
			}
			if (pt3d)
			{
				delete []pt3d;
				pt3d=NULL;
			}
				
		}
	}
escaperef0:	

	CArray<PT_3DEX,PT_3DEX> savePts;
	// 要增加显示动态线的个数
	int vnum = 0;
	if (arrtmpRefPts.GetSize() > 0)
	{
		vnum = arrtmpRefPts.GetSize() - m_arrPts.GetSize();
		savePts.Copy(m_arrPts);
		m_arrPts.Copy(arrtmpRefPts);
	}

	if (m_arrPts.GetSize() < 3)
	{
		CShapeLine shape;
		CLinearizer line;
		line.SetShapeBuf(&shape);
		line.Linearize(m_arrPts.GetData(),m_arrPts.GetSize(),TRUE);
		
		shape.LineEnd();
		
		GrBuffer vbuf;
		const CShapeLine *pSL = &shape;
		pSL->GetVariantGrBuffer(&vbuf,true,m_arrPts.GetSize()-2);
		// 		if(m_bClosed)
		// 			pSL->GetVariantGrBuffer(&vbuf,true,m_arrPts.GetSize()-2);
		// 		else
		// 			pSL->GetVariantGrBuffer(&vbuf);
		vbuf.SetAllColor(m_pGeoSurface->GetColor());
		//m_pGeoCurve->Draw(&vbuf);

		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
	}
	else 
	{
		m_arrPts.Add(m_arrPts[0]);
		if( m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize()) )
		{
			GrBuffer vbuf;
			CShapeLine *pSL = const_cast<CShapeLine*>(m_pGeoSurface->GetShape());
			
			//pSL->SetLastPtcd(m_nCurPenCode);		
			pSL->GetVariantGrBuffer(&vbuf,true,m_arrPts.GetSize()-2-vnum);
			//pSL->SetLastPtcd(penNone);
			
			vbuf.SetAllColor(m_pGeoSurface->GetColor());
			//m_pGeoCurve->Draw(&vbuf);

			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		}
		m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
	}

	if (arrtmpRefPts.GetSize() > 0)
	{
		m_arrPts.Copy(savePts);
	}

	m_arrPts.RemoveAt(m_arrPts.GetSize()-1);

	m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
}

void CDrawSurfaceProcedure::PutStreamPts()
{
	if( m_nCurPenCode==penStream && m_pGeoSurface!=NULL )
	{
		m_compress.EndCompress();
		
		PT_3DEX *pts = NULL;
		int i, npt = 0;
		m_compress.GetPts(pts,npt);
		
		if( npt>1 )
		{
			// 			CArray<PT_3DEX,PT_3DEX> pts0;
			// 			m_pGeoCurve->GetShape(pts0);
			
			int nsum = m_arrPts.GetSize();
			for( i=0; i<npt; i++)
			{
				if( nsum>0 && i==0 );
				else m_arrPts.Add(pts[i]);
			}
			//_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
		}
		
		m_compress.BeginCompress();
	}
}

void CDrawSurfaceProcedure::Finish()
{
	if( m_nCurPenCode==penLine && m_bRectify && m_pGeoSurface )
	{
		PT_3DEX cpt,pt1,pt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pGeoSurface->GetShape(arr);
		int nPtSum = arr.GetSize();
		if( nPtSum>2 )
		{
			BOOL bClosed = FALSE, bFixPt = FALSE;
			
			//是否闭合，如果闭合，就先去掉尾点
			pt1 = arr[0];
			pt4 = arr[nPtSum-1];
			if( fabs(pt1.x-pt4.x)<1e-4 && fabs(pt1.y-pt4.y)<1e-4 )
			{
				bClosed = TRUE;
				cpt = pt4;
				arr.RemoveAt(nPtSum-1);
				nPtSum--;
			}

			//补上一个角点
			if( bClosed && nPtSum>2 && (nPtSum%2==1) )
			{
				PT_3DEX pt2,pt3;
				double dx1,dx2,dy1,dy2,c1,c2,det;
				
				pt1 = arr[0];
				pt2 = arr[1];
				pt3 = arr[nPtSum-2];
				pt4 = arr[nPtSum-1];
				
				dx1 = pt2.x-pt1.x;	dy1 = pt2.y-pt1.y;
				dx2 = pt4.x-pt3.x;	dy2 = pt4.y-pt3.y;
				
				c1 = dy2 * pt1.x - dx2 * pt1.y;
				c2 = dy1 * pt4.x - dx1 * pt4.y;
				
				det  = -dy2*dx1 + dy1*dx2;
				
				if( fabs(det)>1e-10 )
				{
					pt4.x = (-c1*dx1 + c2 *dx2)/det;
					pt4.y = (-c1*dy1 + c2*dy2 )/det;
					arr.Add(pt4);
					bFixPt = TRUE;
				}
			}
			
			// 加一个尾点
			if( bClosed )
			{
				arr.Add(cpt);
			}

			m_pGeoSurface->CreateShape(arr.GetData(),arr.GetSize());
			
			double tole = GetProfileDouble(CString(REGPATH_COMMAND)+_T("\\Rectify"),_T("Sigma"),1.0);
			CGeoCurve *pObj = (CGeoCurve*)Rectify(m_pGeoSurface,tole,FALSE,FALSE);
			if( pObj )
			{
				pObj->GetShape(arr);
				m_pGeoSurface->CreateShape(arr.GetData(),arr.GetSize());
				delete pObj;
				
			}
			//容差太小，直角化不成功
			else if( bFixPt )
			{
				m_pGeoSurface->GetShape(arr);
				arr.RemoveAt(nPtSum);
				m_pGeoSurface->CreateShape(arr.GetData(),arr.GetSize());
			}
		}
	}

	CProcedure::Finish();
}

void CDrawSurfaceProcedure::PtReset(PT_3D &pt)
{
	PutStreamPts();
	if (m_arrPts.GetSize()<3)
	{
		Abort();
		return;
	}

	if (m_bIsRef)
	{
		m_arrPts.Copy(m_arrRefPts);	
	}

	m_arrPts.Add(m_arrPts[0]);

	if( m_pGeoSurface->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize()) )
	{
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
	else
		Abort();
}


void CDrawSurfaceProcedure::GetParams(CValueTable& tab)
{
	CProcedure::GetParams(tab);
}


void CDrawSurfaceProcedure::SetParams(CValueTable& tab,BOOL bInit)
{		
	CProcedure::SetParams(tab,bInit);
}

void CDrawSurfaceProcedure::Abort()
{
	m_arrPts.RemoveAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CProcedure::Abort();
}


void CDrawSurfaceProcedure::Back()
{
	if (m_pGeoSurface)
	{
		long color = m_pGeoSurface->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}
		
		if( m_nCurPenCode==penStream )
		{
			PutStreamPts();
			if( m_arrPts.GetSize()<=1 )
			{
				Abort();
				return;
			}
			
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			{
				PT_3DEX t0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);				
				PT_3D t1;
				COPY_3DPT(t1,t0);
				m_compress.BeginCompress();
				m_compress.AddStreamPt(t1);
				
				GrBuffer buf;
				buf.BeginLineString(color,0);
				buf.MoveTo(&t1);
				buf.LineTo(&t1);
				buf.End();
				
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
				buf.DeleteAll();
				buf.BeginLineString(color,0);
				buf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
				buf.End();
				m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
				m_pEditor->SetCrossPos(t1);
				//	m_pEditor->UpdateDrag(ud_ClearDrag);
				m_pEditor->RefreshView();
			}
			
			return;
		}
		else 
		{
			if(m_arrPts.GetSize()<2)
			{
				Abort();
				return;
			}
			PT_3DEX pt0;
			PT_3D pt1;
			CArray<PT_3DEX,PT_3DEX> pts;
			// 			pts.Copy(m_arrPts);
			// 			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
			pts.Copy(m_arrPts);
			pts.Add(pts[0]);
			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			COPY_3DPT(pt1,pt0);	

			CShapeLine *pSL = NULL;

			CShapeLine shape;
			if (m_arrPts.GetSize() <= 2)
			{
				CLinearizer line;
				line.SetShapeBuf(&shape);
				line.Linearize(m_arrPts.GetData(),m_arrPts.GetSize(),TRUE);
				
				shape.LineEnd();
				
				pSL = &shape;
			}
			else
			{
				m_pGeoSurface->CreateShape(pts.GetData(),pts.GetSize());			
				pSL = const_cast<CShapeLine*>(m_pGeoSurface->GetShape());
			}

			GrBuffer cbuf,vbuf;
			if (m_arrPts.GetSize() > 2)
			{
				pSL->SetLastPtcd(m_nCurPenCode);
			}
//			m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
			//	m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			
			pSL->GetConstGrBuffer(&cbuf,true);
			cbuf.SetAllColor(color);
			if(1)
			{
				if (m_arrPts.GetSize()>2)
				{
					pSL->GetVariantGrBuffer(&vbuf,true,-1);
				}
			}	
			
			if (m_arrPts.GetSize() > 2)
			{
				pSL->SetLastPtcd(penNone);
			}

			m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_pEditor->SetCrossPos(pt1);
			//	m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();

			m_pGeoSurface->CreateShape(m_arrPts.GetData(), m_arrPts.GetSize());
		}
		
	}

}

void CDrawSurfaceProcedure::BatchBack()
{
	if (m_pGeoSurface)
	{
		long color = m_pGeoSurface->GetColor();
		if (color == -1)
		{
			color = m_layCol;
		}
		
		if( m_nCurPenCode==penStream )
		{
			PutStreamPts();
			if( m_arrPts.GetSize()<=11 )
			{
				Abort();
				return;
			}
			
			m_arrPts.RemoveAt(m_arrPts.GetSize()-10,10);
			{
				PT_3DEX t0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);				
				PT_3D t1;
				COPY_3DPT(t1,t0);
				m_compress.BeginCompress();
				m_compress.AddStreamPt(t1);
				
				GrBuffer buf;
				buf.BeginLineString(color,0);
				buf.MoveTo(&t1);
				buf.LineTo(&t1);
				buf.End();
				
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
				buf.DeleteAll();
				buf.BeginLineString(color,0);
				buf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
				buf.End();
				m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
				m_pEditor->SetCrossPos(t1);
				//	m_pEditor->UpdateDrag(ud_ClearDrag);
				m_pEditor->RefreshView();
			}
			
			return;
		}
		else 
		{
			if(m_arrPts.GetSize()<11)
			{
				Abort();
				return;
			}
			PT_3DEX pt0;
			PT_3D pt1;
			CArray<PT_3DEX,PT_3DEX> pts;
			// 			pts.Copy(m_arrPts);
			// 			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			m_arrPts.RemoveAt(m_arrPts.GetSize()-10,10);
			pts.Copy(m_arrPts);
			pt0 = m_arrPts.GetAt(m_arrPts.GetSize()-1);
			COPY_3DPT(pt1,pt0);	

			CShapeLine *pSL = NULL;

			CShapeLine shape;
			if (m_arrPts.GetSize() <= 2)
			{
				CLinearizer line;
				line.SetShapeBuf(&shape);
				line.Linearize(m_arrPts.GetData(),m_arrPts.GetSize(),TRUE);
				
				shape.LineEnd();
				
				pSL = &shape;
			}
			else
			{
				m_pGeoSurface->CreateShape(pts.GetData(),pts.GetSize());			
				pSL = const_cast<CShapeLine*>(m_pGeoSurface->GetShape());
			}

			GrBuffer cbuf,vbuf;
			if (m_arrPts.GetSize() > 2)
			{
				pSL->SetLastPtcd(m_nCurPenCode);
			}
//			m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
			//	m_pGeoCurve->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			
			pSL->GetConstGrBuffer(&cbuf,true);
			cbuf.SetAllColor(color);
			if(1)
			{
				if (m_arrPts.GetSize()>2)
				{
					pSL->GetVariantGrBuffer(&vbuf,true,pts.GetSize());
				}
			}	
			
			if (m_arrPts.GetSize() > 2)
			{
				pSL->SetLastPtcd(penNone);
			}

			m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_pEditor->SetCrossPos(pt1);
			//	m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();
		}
		
	}

}


void CDrawSurfaceProcedure::Start()
{
/*	m_pGeoSurface = new CGeoSurface;*/
	GotoState(PROCSTATE_READY);
	m_arrPts.RemoveAll();
	m_pEditor->CloseSelector();
}

void CDrawSurfaceProcedure::TrackLine()
{
	GotoState(PROCSTATE_PROCESSING);
	
	PT_3DEX curPt(m_curPt,penLine), retPt(m_curPt,penLine);
	if(m_bTrack)
	{
		m_trackPts.Add(retPt);
		int sum = GraphAPI::GKickoffSamePoints(m_trackPts.GetData(),m_trackPts.GetSize());
		m_trackPts.SetSize(sum);
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pGeoSurface->GetColor(),0);
		cbuf.Lines(m_trackPts.GetData(), sum, sizeof(PT_3DEX));
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);

		for(int i=0; i<m_trackPts.GetSize(); i++)
		{
			m_trackPts[i].pencode = penLine;
		}
		
		m_arrPts.Append(m_trackPts);
		m_bTrack = FALSE;
		m_trackPts.RemoveAll();
		return;
	}
	
	PT_3D cltpt;	
	(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_curPt,&cltpt);
	m_pEditor->GetDataQuery()->FindNearestObject(cltpt,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS(),
		m_pEditor->GetCoordWnd().m_pSearchCS);
	int num;
	const CPFeature *ftrs = (m_pEditor->GetDataQuery())->GetFoundHandles(num);
	if(num<=0) return;
	
	CFeature *pFtr = NULL;
	for(int k=0; k<num; k++)
	{
		CGeometry *pGeo = ftrs[k]->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			continue;
		}
		pFtr = ftrs[k];
		break;
	}
	if(pFtr==NULL)  return;
	CGeometry *pGeo = pFtr->GetGeometry();
	int geoType = pGeo->GetClassType();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int trackPart = GetCurvePartPts(m_curPt, pGeo, arrPts);
	
	// 线串化
	for (int i=0; i<arrPts.GetSize(); i++)
	{
		arrPts[i].pencode = penLine;
	}
	
	if (arrPts.GetSize() < 2) return;
	
	if (geoType == CLS_GEOSURFACE)
	{
		//arrPts.Add(arrPts[0]);
	}
	
	int index = -1;
	GraphAPI::GGetNearstDisPToPointSet2D(arrPts.GetData(),arrPts.GetSize(),curPt,retPt,&index);
	retPt.z = m_curPt.z;
	
	m_bTrack = TRUE;
	m_trackHandle = FtrToHandle(pFtr);
	m_nLastTrackIndex = index;
	m_nLastTrackPt = retPt;
	m_TrackPart = trackPart;
	
	retPt.pencode = arrPts[index].pencode;
	m_trackPts.Add(retPt);
	int nPt = m_arrPts.GetSize();
	if (m_arrPts.GetSize() > 0)
	{
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pGeoSurface->GetColor(),0);
		cbuf.MoveTo(&m_arrPts[nPt-1]);
		cbuf.LineTo(&m_trackPts[0]);
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCommand,CProcedure)

CCommand::CCommand()
{

}

CCommand::~CCommand()
{

}

void CCommand::PtReset(PT_3D &pt)
{
	Abort();
}

void CCommand::Start()
{
	CProcedure::Start();

	CString strMsg = StrFromLocalResID(IDS_OUTPUT_CMDPROMPT)+_T(" : ");
	strMsg += Name()+_T('(')+AccelStr()+_T(')');
	PromptString(strMsg);

	CUIParam param0;
	FillShowParams(&param0,TRUE);
	m_pEditor->UIRequest(UIREQ_TYPE_LOADUIPARAM, (LONG_PTR)&param0);

	CUIParam param;
	FillShowParams(&param);
	m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, (LONG_PTR)&param);
}


void CCommand::Abort()
{
	PromptString(StrFromLocalResID(IDS_OUTPUT_CANCEL));
	m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, 0);

	CProcedure::Abort();
}

void CCommand::Finish()
{
	m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, 0);
	
	CProcedure::Finish();
}

//派生类读取tab，并更新自己的数据成员。完成后调用基类的SetParams,去更新参数界面
void CCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	CProcedure::SetParams(tab,bInit);

	CUIParam param;
	FillShowParams(&param);
	m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, (LONG_PTR)&param);
}

void CCommand::RefreshParams()
{
	CUIParam param0;
	FillShowParams(&param0,TRUE);
	m_pEditor->UIRequest(UIREQ_TYPE_LOADUIPARAM, (LONG_PTR)&param0);
	CUIParam param;
	FillShowParams(&param);
	m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, (LONG_PTR)&param);
}

//////////////////////////////////////////////////////////////////////
// CEditCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CEditCommand,CCommand)

CEditCommand::CEditCommand()
{
	m_nStep = -1;

}

CEditCommand::~CEditCommand()
{
}

void CEditCommand::Start()
{
	if( m_pEditor )
	{
		m_pEditor->OpenSelector();
	//	m_pDoc->UpdateAllViews(NULL,hc_Attach_Accubox);
	}
	
	m_nStep = 0;	
	CCommand::Start();
}


void CEditCommand::PtReset(PT_3D &pt)
{
//	m_pEditor->DeselectAll();
//	m_pEditor->OnSelectChanged();
	CCommand::PtReset(pt);
}


void CEditCommand::Abort()
{
	if( m_pEditor )
	{
//		m_pEditor->SetCurDrawingObj(DrawingInfo());
	
		m_pEditor->CloseSelector();
	//	m_pDoc->UpdateAllViews(NULL,hc_Detach_Accubox);
		m_pEditor->UpdateDrag(ud_ClearDrag,NULL);
	}
	
	m_nStep = -1;

	CCommand::Abort();
}

void CEditCommand::Finish()
{
//	m_pEditor->SetCurDrawingObj(DrawingInfo());
	m_pEditor->CloseSelector();
	CCommand::Finish();
}

void CEditCommand::EditStepOne()
{	
	GotoState(PROCSTATE_PROCESSING);
	m_nStep = 1;
	m_pEditor->CloseSelector();
}

BOOL CEditCommand::CanGetSelObjs(int flag, BOOL bMultiSel)
{
	//选择集没有发生变化，可以接收数据
	if( flag==SELSTAT_NONESEL )
		return TRUE;
	
	if( bMultiSel )
	{
		//正在多选，跳过
		if( (flag&SELSTAT_MULTISEL)!=0 )return FALSE;
		if ( (flag&SELSTAT_DRAGSEL_START) !=0 )return FALSE;
		
// 		if( (flag&SELSTAT_DRAGSEL_RESTART)==0 )
// 			PromptString(StrFromLocalResID(IDS_CMDTIP_CLICKOK));
// 		
		//正在重新拉框，可以接收数据
		return ( (flag&SELSTAT_DRAGSEL_RESTART)!=0 );
	}
	else
	{
		//点选完成，可以接收数据
		return ( (flag&SELSTAT_POINTSEL)!=0 );
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDrawPointCommand,CDrawCommand)

CDrawPointCommand::CDrawPointCommand()
{
	m_pDrawProc = NULL;
	m_pFtr = NULL;
}


CDrawPointCommand::~CDrawPointCommand()
{
	if( m_pDrawProc )delete m_pDrawProc;
	if(m_pFtr ) delete m_pFtr;
	
}

CGeometry *CDrawPointCommand::GetCurDrawingObj()
{
	if(!m_pDrawProc->m_pGeoPoint) return NULL;
	return  m_pDrawProc->m_pGeoPoint;
}

CString CDrawPointCommand::Name()
{
	return StrFromLocalResID(IDS_CMDNAME_PLACEPOINT);
}


void CDrawPointCommand::Start()
{
	m_pDrawProc = new CDrawPointProcedure;
	if( !m_pDrawProc )return;

	m_pDrawProc->Init(m_pEditor);
//	m_pFtr = new CFeature;
	if(!m_pFtr) return;
	CGeoPoint *pGeo = new CGeoPoint;
	if(!pGeo) return;
	m_pFtr->SetGeometry(pGeo);
	m_pDrawProc->m_pGeoPoint = pGeo;
	m_pDrawProc->Start();	
	
	CDrawCommand::Start();
}


void CDrawPointCommand::Finish()
{
	if( m_pDrawProc )
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	
	CDrawCommand::Finish();
}


void CDrawPointCommand::Abort()
{
	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	CDrawCommand::Abort();
}


int CDrawPointCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}

int  CDrawPointCommand::GetCurPenCode()
{
	return penLine;
}

void CDrawPointCommand::OnSonEnd(CProcedure *son)
{
	ASSERT( m_pDrawProc==son );
	if( IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoPoint )
		Abort();
	else
	{		
		if( m_pFtr )
		{
			if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
			{
// 				delete m_pFtr;
// 				m_pFtr = NULL;
				Abort();
				return;
			}

			CUndoFtrs undo(m_pEditor,Name());
			undo.AddNewFeature(FtrToHandle(m_pFtr));
			undo.Commit();

			Finish();
		}
		else
			Abort();
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDrawCurveCommand,CDrawCommand)

CDrawCurveCommand::CDrawCurveCommand()
{
	m_pDrawProc = NULL;
	m_pFtr = NULL;
	_tcscat(m_strRegPath, _T("\\DrawCurve"));
}


CDrawCurveCommand::~CDrawCurveCommand()
{
	if( m_pDrawProc )delete m_pDrawProc;
	if(m_pFtr) delete m_pFtr;
}

CGeometry *CDrawCurveCommand::GetCurDrawingObj()
{
	if(!m_pDrawProc->m_pGeoCurve) return NULL;
	return  m_pDrawProc->m_pGeoCurve;
}

CString CDrawCurveCommand::Name()
{
	return StrFromLocalResID(IDS_CMDNAME_PLACELINE);
}


void CDrawCurveCommand::Start()
{	
	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_bSnap2D = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_IS2D,TRUE);
//	m_pFtr = new CFeature;
	if(!m_pFtr) return;
	CGeoCurve *pGeo = new CGeoCurve;
	if(!pGeo) return;
	m_pFtr->SetGeometry(pGeo);
	m_pDrawProc->m_pGeoCurve = pGeo;
	UpdateParams(FALSE);
	m_pDrawProc->Start();	

	CDrawCommand::Start();
}


void CDrawCurveCommand::Finish()
{
	if(m_bSettingModified)
		UpdateParams(TRUE);
	if( m_pDrawProc )
	{		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	
	CDrawCommand::Finish();	
}


int  CDrawCurveCommand::GetCurPenCode()
{
	if (m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}

void CDrawCurveCommand::UpdateParams(BOOL bSave)
{
	CDrawCommand::UpdateParams(bSave);
}


void CDrawCurveCommand::Abort()
{
	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pFtr) 
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	CDrawCommand::Abort();
}


int CDrawCurveCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}


void CDrawCurveCommand::Back() //没有必要存在
{
	if (m_pDrawProc&&m_pDrawProc->m_pGeoCurve)
	{	
		if(m_pDrawProc->m_pGeoCurve->GetDataPointSum()<2)
		{
			Abort();
			return;
		}
		CArray<PT_3DEX,PT_3DEX> pts;
		m_pDrawProc->m_pGeoCurve->GetShape(pts);
		pts.RemoveAt(pts.GetSize()-1);
		m_pDrawProc->m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
		GrBuffer cbuf,vbuf;
		m_pDrawProc->m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf);
		m_pDrawProc->m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf,m_pDrawProc->m_pGeoCurve->IsClosed());
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
	}
}


void CDrawCurveCommand::OnSonEnd(CProcedure *son)
{
	ASSERT( m_pDrawProc==son );

	if( IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoCurve )
		Abort();
	else
	{		
		if( m_pFtr )
		{
			if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
			{			
				Abort();
				return;
			}
			
			CUndoFtrs undo(m_pEditor,Name());
			undo.AddNewFeature(FtrToHandle(m_pFtr));
			undo.Commit();
			
			Finish();
		}
		else
			Abort();
	}
}

void CDrawCurveCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);

	if(!m_pDrawProc) return;
	
	_variant_t var;
	var = (long)(m_pDrawProc->m_nCurPenCode);
	tab.AddValue(PF_PENCODE,&CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bClosed);
	tab.AddValue(PF_CLOSED,&CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bOpenIfSnapped);
	tab.AddValue(PF_OPENIFSNAP,&CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bRectify);
	tab.AddValue(PF_RECTIFY,&CVariantEx(var));

	var = (double)(m_pDrawProc->m_compress.GetLimit());
	tab.AddValue(PF_TOLER,&CVariantEx(var));

	var = (double)(m_pDrawProc->m_fAutoCloseToler);
	tab.AddValue(PF_AUTOCLOSETOLER,&CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bDoSmooth);
	tab.AddValue(PF_POSTSMOOTH,&CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bFastDisplayCurve);
	tab.AddValue(PF_FASTCURVE,&CVariantEx(var));

	var = (double)(m_pDrawProc->m_fTolerBuildPt);
	tab.AddValue(PF_BUILDPTTOLER,&CVariantEx(var));
	
}

void CDrawCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(AccelStr(),Name());
	if( m_pDrawProc )
	{
		PT_3D pt;
	    param->AddParam(PF_POINT,pt,StrFromLocalResID(IDS_CMDPLANE_POINT));
	    param->AddParam(PF_CLOSED,(bool)(m_pDrawProc->m_bClosed),StrFromLocalResID(IDS_CMDPLANE_CLOSE));
		param->AddParam(PF_CLOSEDYACCKEY,'c',StrFromResID(IDS_CMDPLANE_CLOSE));
		if (m_pDrawProc->m_bClosed || bForLoad)
		{
			param->BeginOptionParam(PF_OPENIFSNAP,StrFromResID(IDS_CMDPLANE_OPENIFSNAP));
			param->AddOption(StrFromResID(IDS_YES),1,' ',m_pDrawProc->m_bOpenIfSnapped);
			param->AddOption(StrFromResID(IDS_NO),0,' ',!m_pDrawProc->m_bOpenIfSnapped);
			param->EndOptionParam();
		}

		if (m_pDrawProc->m_nCurPenCode == penLine || bForLoad)
		{
			param->BeginOptionParam(PF_RECTIFY,StrFromResID(IDS_CMDPLANE_RECTIFY));
			param->AddOption(StrFromResID(IDS_YES),1,' ',m_pDrawProc->m_bRectify);
			param->AddOption(StrFromResID(IDS_NO),0,' ',!m_pDrawProc->m_bRectify);
			param->EndOptionParam();
		}

		param->AddParam(PF_RECTIFYACCKEY,'r',StrFromResID(IDS_CMDPLANE_RECTIFY));
		param->AddParam(PF_REVERSEACCKEY,'t',StrFromResID(IDS_CMDPLANE_REVERSE));
		
		param->AddParam(PF_TRACKLINEACCKEY,'w',StrFromResID(IDS_CMDPLANE_TRACKLINE));

		param->AddParam(PF_MULPTBUILDPTACCKEY,'m',StrFromResID(IDS_CMDPLANE_MULPTBUILDPT));

		if (m_pDrawProc->m_nCurPenCode == penStream || bForLoad)
		{
			// 容差，自动闭合容差，采集后光滑
			param->AddParam(PF_TOLER,m_pDrawProc->m_compress.GetLimit(),StrFromResID(IDS_CMDPLANE_TOLERANCE));
			param->AddParam(PF_AUTOCLOSETOLER,m_pDrawProc->m_fAutoCloseToler,StrFromResID(IDS_CMDPLANE_AUTOCLOSETOLER));

			param->BeginOptionParam(PF_POSTSMOOTH,StrFromResID(IDS_CMDPLANE_POSTSMOOTH));
			param->AddOption(StrFromResID(IDS_YES),1,' ',m_pDrawProc->m_bDoSmooth);
			param->AddOption(StrFromResID(IDS_NO),0,' ',!m_pDrawProc->m_bDoSmooth);
			param->EndOptionParam();
		}

		if (m_pDrawProc->m_nCurPenCode == penSpline || bForLoad)
		{
			param->BeginOptionParam(PF_FASTCURVE,StrFromResID(IDS_CMDPLANE_FASTCURVE));
			param->AddOption(StrFromResID(IDS_YES),1,' ',m_pDrawProc->m_bFastDisplayCurve);
			param->AddOption(StrFromResID(IDS_NO),0,' ',!m_pDrawProc->m_bFastDisplayCurve);
			param->EndOptionParam();
		}

	    param->AddLineTypeParam(PF_PENCODE,m_pDrawProc->m_nCurPenCode,StrFromLocalResID(IDS_CMDPLANE_LINETYPE));

		if (m_pDrawProc->m_bMultiPtBuildPt || bForLoad)
		{
			param->AddParam(PF_BUILDPTTOLER,m_pDrawProc->m_fTolerBuildPt,StrFromResID(IDS_CMDPLANE_BUILDPTTOLERANCE));
		}
	}
}


void CDrawCurveCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_PENCODE,var) )
	{
		if (bInit)
		{
			m_pDrawProc->m_nCurPenCode = (long)(_variant_t)*var;
		}
		else
			m_pDrawProc->ChangePencod((long)(_variant_t)*var);
	}
	if( tab.GetValue(0,PF_NODEWID,var) )
	{
		m_pDrawProc->m_fCurNodeWid = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_NODETYPE,var) )
	{
		m_pDrawProc->m_nCurNodeType = (short)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_CLOSED,var) )
	{
		m_pDrawProc->m_bClosed = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_CLOSEDYACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'c'||ch == 'C')
		{
			m_pDrawProc->m_bClosed = !m_pDrawProc->m_bClosed;
		}
		
	}
	if( tab.GetValue(0,PF_TRACKLINEACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'w'||ch == 'W')
		{
			m_pDrawProc->TrackLine();
		}
	}
	if( tab.GetValue(0,PF_OPENIFSNAP,var) )
	{
		m_pDrawProc->m_bOpenIfSnapped = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_RECTIFY,var) )
	{
		m_pDrawProc->m_bRectify = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_RECTIFYACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'r'||ch == 'R')
		{
			m_pDrawProc->m_bRectify = !m_pDrawProc->m_bRectify;
		}
		
	}
	if( tab.GetValue(0,PF_REVERSEACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 't'||ch == 'T')
		{
			m_pDrawProc->ReverseLine();
		}
		
	}
	if( tab.GetValue(0,PF_TOLER,var) )
	{
		m_pDrawProc->m_compress.SetLimit((double)(_variant_t)*var);
	}
	if( tab.GetValue(0,PF_AUTOCLOSETOLER,var) )
	{
		m_pDrawProc->m_fAutoCloseToler = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_POSTSMOOTH,var) )
	{
		m_pDrawProc->m_bDoSmooth = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_FASTCURVE,var) )
	{
		m_pDrawProc->m_bFastDisplayCurve = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_POINT,var) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		var->GetShape(arrPts);
		PT_3D point = arrPts.GetAt(0);
		PtClick(point,0);
		PtMove(point);
		m_pEditor->RefreshView();
	}

	if( tab.GetValue(0,PF_MULPTBUILDPTACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'm'||ch == 'M')
		{
			m_pDrawProc->m_bMultiPtBuildPt?m_pDrawProc->EndBuildPt():m_pDrawProc->StartBuildPt();
		}		
	}
	
	if( tab.GetValue(0,PF_BUILDPTTOLER,var) )
	{
		m_pDrawProc->m_fTolerBuildPt = (double)(_variant_t)*var;
	}

// 	if( m_pDrawProc->m_pGeoCurve )
// 		m_pDrawProc->m_pGeoCurve->EnableClose(m_pDrawProc->m_bClosed);	
	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab,bInit);
}


BOOL CDrawCurveCommand::Keyin(LPCTSTR text)
{	
	if(!m_pDrawProc)
		return FALSE;

	// 通过长度和角度指定点
	char buf[100] = {0};

	strncpy(buf,text,100-1);
	char *stop = NULL;
	double len = strtod(buf,&stop);
	if (stop!=NULL && strlen(stop)==0)
	{		
		int num = m_pDrawProc->m_arrPts.GetSize();
		if (num > 0)
		{
			PT_3D curPt = m_pEditor->GetCoordWnd().m_ptGrd;
			PT_3DEX pt0 = m_pDrawProc->m_arrPts[num-1], pt1;
			double dx = curPt.x-pt0.x, dy = curPt.y-pt0.y;
			double dis = sqrt(dx*dx+dy*dy);
			if(dis<GraphAPI::GetDisTolerance())
			{
				pt1.x = pt0.x + len;
				pt1.y = pt0.y;
				pt1.z = curPt.z;
			}
			else
			{
				pt1.x = pt0.x + dx/dis * len;
				pt1.y = pt0.y + dy/dis * len;
				pt1.z = curPt.z;
			}
			
			PtClick(pt1,0);
			PtMove(curPt);
			m_pEditor->RefreshView();

			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDrawSurfaceCommand,CDrawCommand)

CDrawSurfaceCommand::CDrawSurfaceCommand()
{
	m_pDrawProc = NULL;
	_tcscat(m_strRegPath,_T("\\DrawSurface"));
}


CDrawSurfaceCommand::~CDrawSurfaceCommand()
{
	if( m_pDrawProc )delete m_pDrawProc;
}

CGeometry *CDrawSurfaceCommand::GetCurDrawingObj()
{
	if(!m_pDrawProc->m_pGeoSurface) return NULL;
	return  m_pDrawProc->m_pGeoSurface;
}

CString CDrawSurfaceCommand::Name()
{
	return StrFromLocalResID(IDS_CMDNAME_PLACESURFACE);
}


void CDrawSurfaceCommand::Start()
{
	m_pDrawProc = new CDrawSurfaceProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_bSnap2D = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_IS2D,TRUE);
//	m_pFtr = new CFeature;
	if(!m_pFtr) return;
	CGeoSurface *pGeo = new CGeoSurface;
	if(!pGeo) return;
	m_pFtr->SetGeometry(pGeo);
	m_pDrawProc->m_pGeoSurface = pGeo;
	UpdateParams(FALSE);
	m_pDrawProc->Start();	

	CDrawCommand::Start();
}


int  CDrawSurfaceCommand::GetCurPenCode()
{
	if (m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}
void CDrawSurfaceCommand::Finish()
{
	if(m_bSettingModified)UpdateParams(TRUE);
	if( m_pDrawProc )
	{		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	
	CDrawCommand::Finish();
}


void CDrawSurfaceCommand::UpdateParams(BOOL bSave)
{
	CDrawCommand::UpdateParams(bSave);
}


void CDrawSurfaceCommand::Abort()
{
	if(m_bSettingModified)UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	CDrawCommand::Abort();
}


int CDrawSurfaceCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}


void CDrawSurfaceCommand::OnSonEnd(CProcedure *son)
{
	ASSERT( m_pDrawProc==son );
	if( IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoSurface )
		Abort();
	else
	{	
		if( m_pFtr )
		{
			if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
			{				
				Abort();
				return;
			}
			
			CUndoFtrs undo(m_pEditor,Name());
			undo.AddNewFeature(FtrToHandle(m_pFtr));
			undo.Commit();
			
			Finish();
		}
		else
			Abort();
	}
}


void CDrawSurfaceCommand::GetParams(CValueTable& tab)
{
	if (!m_pDrawProc)
		return;	
	_variant_t var;
	var = (long)m_pDrawProc->m_nCurPenCode;
	tab.AddValue(PF_PENCODE,&CVariantEx(var));
	var = (double)m_pDrawProc->m_lfToler;
	tab.AddValue(PF_REFTOLER,&CVariantEx(var));
	var = (bool)(m_pDrawProc->m_bRectify);
	tab.AddValue(PF_RECTIFY,&CVariantEx(var));
	var = (double)m_pDrawProc->m_compress.GetLimit();
	tab.AddValue(PF_TOLER,&CVariantEx(var));
}


void CDrawSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("DrawSurface"),StrFromLocalResID(IDS_CMDNAME_PLACESURFACE));
	if( m_pDrawProc )
	{
		PT_3D pt;
		param->AddParam(PF_POINT,pt,StrFromLocalResID(IDS_CMDPLANE_POINT));		
		param->AddLineTypeParam(PF_PENCODE,m_pDrawProc->m_nCurPenCode,StrFromLocalResID(IDS_CMDPLANE_LINETYPE));
	
		param->AddParam(PF_REFTOLER,m_pDrawProc->m_lfToler,StrFromLocalResID(IDS_CMDPLANE_REFTOLER));
		
		if (m_pDrawProc->m_nCurPenCode == penLine || bForLoad)
		{
			param->BeginOptionParam(PF_RECTIFY,StrFromResID(IDS_CMDPLANE_RECTIFY));
			param->AddOption(StrFromResID(IDS_YES),1,' ',m_pDrawProc->m_bRectify);
			param->AddOption(StrFromResID(IDS_NO),0,' ',!m_pDrawProc->m_bRectify);
			param->EndOptionParam();
		}

		param->AddParam(PF_RECTIFYACCKEY,'r',StrFromResID(IDS_CMDPLANE_RECTIFY));
		param->AddParam(PF_TRACKLINEACCKEY,'w',StrFromResID(IDS_CMDPLANE_TRACKLINE));

		if (m_pDrawProc->m_nCurPenCode == penStream || bForLoad)
		{
			// 容差，自动闭合容差，采集后光滑
			param->AddParam(PF_TOLER,m_pDrawProc->m_compress.GetLimit(),StrFromResID(IDS_CMDPLANE_TOLERANCE));
		}

		param->AddParam(PF_LENGTH,double(1.0),StrFromResID(IDS_FIELDNAME_LENGTH),NULL,'L',FALSE);
		
		param->AddParam(PF_ANGLE,double(0.0),StrFromResID(IDS_FIELDNAME_ANGLE),NULL,'A',FALSE);
		

	}
}


void CDrawSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_PENCODE,var) )
	{
		m_pDrawProc->m_nCurPenCode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_NODEWID,var) )
	{
		m_pDrawProc->m_fCurNodeWid = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_NODETYPE,var) )
	{
		m_pDrawProc->m_nCurNodeType = (short)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_REFTOLER,var) )
	{
		m_pDrawProc->m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_RECTIFY,var) )
	{
		m_pDrawProc->m_bRectify = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_RECTIFYACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'r'||ch == 'R')
		{
			m_pDrawProc->m_bRectify = !m_pDrawProc->m_bRectify;
		}
		
	}
	if( tab.GetValue(0,PF_TRACKLINEACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'w'||ch == 'W')
		{
			m_pDrawProc->TrackLine();
		}
	}
	if( tab.GetValue(0,PF_TOLER,var) )
	{
		m_pDrawProc->m_compress.SetLimit((double)(_variant_t)*var);
		SetSettingsModifyFlag();
	}

	// 通过长度和角度指定点
	if (tab.GetValue(0,PF_LENGTH,var))
	{
		const CVariantEx *var1;
		if (tab.GetValue(0,PF_ANGLE,var1))
		{
			double len = (double)(_variant_t)*var;
			double ang = (double)(_variant_t)*var1;
			
			if (m_pDrawProc->m_arrPts.GetSize() > 0)
			{
				PT_3DEX pt = m_pDrawProc->m_arrPts[0];
				pt.x += len*cos(ang/180*PI);
				pt.y += len*sin(ang/180*PI);
				
				PtClick(pt,0);
				PtMove(pt);
				m_pEditor->RefreshView();
			}
		}
	}
	CDrawCommand::SetParams(tab,bInit);
}


BOOL CDrawSurfaceCommand::Keyin(LPCTSTR text)
{	
	if(!m_pDrawProc)
		return FALSE;

	// 通过长度和角度指定点
	char buf[100] = {0};

	strncpy(buf,text,100-1);
	char *stop = NULL;
	double len = strtod(buf,&stop);
	if (stop!=NULL && strlen(stop)==0)
	{		
		int num = m_pDrawProc->m_arrPts.GetSize();
		if (num > 0)
		{
			PT_3D curPt = m_pEditor->GetCoordWnd().m_ptGrd;
			PT_3DEX pt0 = m_pDrawProc->m_arrPts[num-1], pt1;
			double dx = curPt.x-pt0.x, dy = curPt.y-pt0.y;
			double dis = sqrt(dx*dx+dy*dy);
			if(dis<GraphAPI::GetDisTolerance())
			{
				pt1.x = pt0.x + len;
				pt1.y = pt0.y;
				pt1.z = curPt.z;
			}
			else
			{
				pt1.x = pt0.x + dx/dis * len;
				pt1.y = pt0.y + dy/dis * len;
				pt1.z = curPt.z;
			}
			
			PtClick(pt1,0);
			PtMove(curPt);
			m_pEditor->RefreshView();

			return TRUE;
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CEditVertexCommand, CActiveStateCommand)

CEditVertexCommand::CEditVertexCommand()
{
	m_nStep = 0;	
	m_arrKeyPts.RemoveAll();
	m_arrFtrs.RemoveAll();
	m_arrPtsRepeat.RemoveAll();
}


CEditVertexCommand::~CEditVertexCommand()
{
}


CString CEditVertexCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_EDITVERTEX);
}


void CEditVertexCommand::Start()
{
	m_pEditor->OpenSelector(SELMODE_SINGLE);
	m_nStep = 0;
// 	m_pObj = NULL;
// 	m_pFtr = NULL;
// 	m_pt = PT_KEYCTRL();
	m_arrKeyPts.RemoveAll();
	m_arrFtrs.RemoveAll();
	m_arrPtsRepeat.RemoveAll();

	CCommand::Start();
}


void CEditVertexCommand::Abort()
{
	m_arrKeyPts.RemoveAll();
	m_arrFtrs.RemoveAll();
	m_arrPtsRepeat.RemoveAll();

	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_nStep = -1;
	CActiveStateCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}

void CEditVertexCommand::Finish()
{
	CActiveStateCommand::Finish();
	m_nExitCode = CMPEC_STARTOLD;
}


void CEditVertexCommand::OutputVertexInfo(PT_KEYCTRL pt, CGeometry *pGeo)
{
	CString str;
	if( pt.type==PT_KEYCTRL::typeKey )
	{		
		PT_3DEX t;
		t = pGeo->GetDataPoint(pt.index);
		
		CString strStyle;
		switch(t.pencode) {
		case penLine:
			strStyle = StrFromResID(IDS_CMDTIP_LINE);
			break;
		case penSpline:
			strStyle = StrFromResID(IDS_CMDTIP_CURVE);
			break;
		case penArc:
			strStyle = StrFromResID(IDS_CMDTIP_ARC);
			break;
		case penStream:
			strStyle = StrFromResID(IDS_CMDTIP_SYNCH);
			break;
		default:;
		}
		
		str.Format(IDS_CMDTIP_VERTEXINFO,
			pt.index,t.x,t.y,t.z);
		if( pGeo->GetDataPointSum()>1 )
		{
			CString str2;
			str2.Format(_T(", %s: %s"),(LPCTSTR)StrFromResID(IDS_CMDPLANE_LINETYPE),(LPCTSTR)strStyle);
			str += str2;

			str2.Format(_T(", %s: %.4f"),(LPCTSTR)StrFromResID(IDS_CMDTIP_NODEWID),t.wid);
			str += str2;
		}
		
		str += _T("\n");
	}
	else
	{
		PT_3D t;
		t = pGeo->GetCtrlPoint(pt.index);
		
		str.Format(IDS_CMDTIP_CTRLPTINFO,
			t.x,t.y,t.z);
	}
	
	PromptString(str);
}

void CEditVertexCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CSelection *pselection = m_pEditor->GetSelection();
		CCoordWnd pencode = m_pEditor->GetCoordWnd();
		double r = pselection->GetSelectRadius()*pencode.GetScaleOfSearchCSToViewCS();
		PT_3D spt = pt;
		
		pencode.m_pSearchCS->GroundToClient(&pt,&spt);
		
		CDataQuery *pDQ = m_pEditor->GetDataQuery();
		
		int nsel = 0;
		pselection->GetSelectedObjs(nsel);
		if( nsel==1 )
		{
			CFeature *pFtr =HandleToFtr(pselection->GetLastSelectedObj());
			m_arrFtrs.Add(pFtr);
		}
		else if( nsel>1 )
		{
			Envelope e;
			e.CreateFromPtAndRadius(spt,r);
			m_pEditor->GetDataQuery()->FindObjectInRect(e,pencode.m_pSearchCS);

			int num;
			const CPFeature * ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);

			for (int i=0; i<num; i++)
			{
				if( pselection->IsObjInSelection(FtrToHandle(ftr[i])) )
				{
					m_arrFtrs.Add(ftr[i]);
				}
			}

		}
		GotoState(PROCSTATE_PROCESSING);

		if (m_arrFtrs.GetSize() <= 0)
		{
			return;
		}

		Envelope e;
		e.CreateFromPtAndRadius(spt,r);

		for (int i=0; i<m_arrFtrs.GetSize(); i++)
		{
			CFeature *pFtr = m_arrFtrs[i];
			if (!pFtr) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;
			
			PT_KEYCTRL keyPt = pGeo->FindNearestKeyCtrlPt(spt,r,pencode.m_pSearchCS);
			if (!keyPt.IsValid())
			{
				m_arrFtrs.RemoveAt(i);
				i--;
			}
			else
			{
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
				{
					CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
					int nGeo = pMGeo->GetSurfaceNum();
					int nPtClosedPos = 0, j;
					for(j=0; j<nGeo; j++)
					{
						CArray<PT_3DEX,PT_3DEX> arrSonPts;
						pMGeo->GetSurface(j,arrSonPts);

						if(keyPt.index==nPtClosedPos || keyPt.index==(nPtClosedPos+arrSonPts.GetSize()-1) )
						{
							keyPt.index = nPtClosedPos;
							m_arrPtsRepeat.Add(TRUE);
							break;
						}

						nPtClosedPos += arrSonPts.GetSize();
					}
					if( j>=nGeo )
					{
						m_arrPtsRepeat.Add(FALSE);
					}
				}
				else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					if(((CGeoCurveBase*)pGeo)->IsClosed() && (keyPt.index==0 || keyPt.index==pGeo->GetDataPointSum()-1)) 
					{
						keyPt.index = 0;
						m_arrPtsRepeat.Add(TRUE);
					}
					else
						m_arrPtsRepeat.Add(FALSE);
				}
				else
				{
					m_arrPtsRepeat.Add(FALSE);
				}

				m_arrKeyPts.Add(keyPt);
			}
			
		}


		if (m_arrKeyPts.GetSize() <= 0)
		{
			return;
		}

		PT_KEYCTRL keyPt = m_arrKeyPts[0];

		CGeometry *pGeo = m_arrFtrs[0]->GetGeometry();
		if (!pGeo)
		{
			return;
		}
		if( keyPt.type==PT_KEYCTRL::typeKey )
		{
			PT_3DEX t;
			t = pGeo->GetDataPoint(keyPt.index);
			m_ptDragStart.x = t.x;
			m_ptDragStart.y = t.y;
			m_ptDragStart.z = t.z;
			m_ptDragEnd = m_ptDragStart;
		}
		else
		{
			PT_3D t;
			t = pGeo->GetCtrlPoint(keyPt.index);
			m_ptDragStart.x = t.x;
			m_ptDragStart.y = t.y;
			m_ptDragStart.z = t.z;
			m_ptDragEnd = m_ptDragStart;
		}

		OutputVertexInfo(keyPt,pGeo);
		
		m_nStep = 1;
		m_pEditor->CloseSelector();
		
	}
	else if( m_nStep==1 )
	{
		m_ptDragEnd = pt;

		CUndoBatchAction batchUndo(m_pEditor,_T("EditVertexs"));
		for (int i=0; i<m_arrFtrs.GetSize(); i++)
		{
			CFeature *pObj = m_arrFtrs[i];
			if( !pObj )return;
			
			CGeometry *pGeo = pObj->GetGeometry();
			if( !pGeo )return;

			m_pEditor->DeleteObject(FtrToHandle(pObj),FALSE);
			
			CUndoVertex undo(m_pEditor,Name());
			
			if( m_arrKeyPts[i].type==PT_KEYCTRL::typeKey )
			{	
				PT_3DEX t;
				t = pGeo->GetDataPoint(m_arrKeyPts[i].index);
				undo.ptOld = t;
				
				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
				pGeo->SetDataPoint(m_arrKeyPts[i].index,t);
				
				undo.ptNew = t;
				if(m_arrPtsRepeat[i])
				{
					if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
					{
						((CGeoMultiSurface*)pGeo)->SetClosedPt(m_arrKeyPts[i].index,pt,TRUE);
					}
					else if(m_arrKeyPts[i].index)  	
					{				
						t = pGeo->GetDataPoint(0);
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						pGeo->SetDataPoint(0,t);					
					}
					else
					{
						int nLastPt = pGeo->GetDataPointSum()-1;				
						t = pGeo->GetDataPoint(nLastPt);
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						pGeo->SetDataPoint(nLastPt,t);
					}
				}
			}
			else if( m_arrKeyPts[i].type==PT_KEYCTRL::typeCtrl )
			{
				PT_3D t;
				t = pGeo->GetCtrlPoint(m_arrKeyPts[i].index);
				
				COPY_3DPT(undo.ptOld,t);
				
				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
				pGeo->SetCtrlPoint(m_arrKeyPts[i].index,t);
				
				COPY_3DPT(undo.ptNew,t);
				
			}
			
			m_pEditor->RestoreObject(FtrToHandle(pObj));
			//m_pEditor->UpdateObject(FtrToHandle(m_arrFtrs[i]));	
			
			undo.handle = FtrToHandle(m_arrFtrs[i]);
			undo.bRepeat = m_arrPtsRepeat[i];
			undo.nPtType = m_arrKeyPts[i].type;
			undo.nPtIdx = m_arrKeyPts[i].index;
			//undo.Commit();

			batchUndo.AddAction(&undo);
		}	
		
		batchUndo.Commit();
		
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->OnSelectChanged(TRUE);			

		Finish();
		m_pEditor->CloseSelector();
		m_nStep = 3;
	
	}
	return;
}


void CEditVertexCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==1 )
	{
		GrBuffer buf;

		m_ptDragEnd = pt;

		for (int i=0; i<m_arrFtrs.GetSize(); i++)
		{
			CFeature *pFtr = m_arrFtrs[i];
			if( !pFtr )return;

			CFeature *pObj = pFtr->Clone();
			if( !pObj )return;
			
			CGeometry *pGeo = pObj->GetGeometry();
			if( !pGeo )return;

			if( m_arrKeyPts[i].type==PT_KEYCTRL::typeKey )
			{
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);

				PT_3DEX t;
				t = arrPts[m_arrKeyPts[i].index];
				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
				arrPts[m_arrKeyPts[i].index] = t;
				
				if(m_arrPtsRepeat[i])
				{
					if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
					{
						((CGeoMultiSurface*)pGeo)->SetClosedPt(m_arrKeyPts[i].index,pt,TRUE);
					}
					else if(m_arrKeyPts[i].index)  	
					{
						t = arrPts[0];
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						arrPts[0] = t;

						pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
					}
					else
					{
						int nLastPt = arrPts.GetSize()-1;
						t = arrPts[nLastPt];
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						arrPts[nLastPt] = t;

						pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
					}					
				}
				else
				{
					pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
				}

				BOOL close = FALSE;
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					close = ((CGeoCurveBase*)pGeo)->IsClosed();
				}
				const CShapeLine *pSL = pGeo->GetShape();
				if(pSL)pSL->GetVariantGrBuffer(&buf,close,m_arrKeyPts[i].index,true);
				
				
			}
			else if( m_arrKeyPts[i].type==PT_KEYCTRL::typeCtrl )
			{
				PT_3D t;
				t = pGeo->GetCtrlPoint(m_arrKeyPts[i].index);
				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
				pGeo->SetCtrlPoint(m_arrKeyPts[i].index,t);
				
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
				{
					CArray<PT_3DEX,PT_3DEX> pts;
					((CGeoParallel*)pGeo)->GetParallelShape(pts);
					int num = pts.GetSize();
					
					buf.BeginLineString(pGeo->GetColor(),0,0);				
					for(int i=num -1; i>=0; i--)
					{
						if( i==(num-1) )buf.MoveTo(&pts[i]);
						else buf.LineTo(&pts[i]);
					}				
					buf.End();
				}
// 				else
// 				{
// 					pGeo->Draw(&buf);
// 				}
				
			}

			delete pObj;
		}

		m_pEditor->UpdateDrag(ud_AddVariantDrag,&buf);
	}
}


BOOL CEditVertexCommand::HitTest(PT_3D pt, int state)const
{
	CSelection *pselection = m_pEditor->GetSelection();
	CCoordWnd cw = m_pEditor->GetCoordWnd();
	double r = pselection->GetSelectRadius()*cw.GetScaleOfSearchCSToViewCS();
	PT_3D spt = pt;	
	cw.m_pSearchCS->GroundToClient(&pt,&spt);
	
	CDataQuery *pDQ = m_pEditor->GetDataQuery();
	
	CPFeature pFtr = NULL;
	int nsel = 0;
	pselection->GetSelectedObjs(nsel);
	if( nsel==1 )pFtr = HandleToFtr(pselection->GetLastSelectedObj());
	else if( nsel>1 )
	{
		CPFeature pNearest = pDQ->FindNearestObject(spt,r,cw.m_pSearchCS);

		int nFoundNum = 0;
		const CPFeature *ftrs = pDQ->GetFoundHandles(nFoundNum);
		for( int i=0; i<nFoundNum; i++)
		{
			if( pselection->IsObjInSelection(FTR_HANDLE(ftrs[i])) )
			{
				pFtr = ftrs[i];
				break;
			}

		}

	}
	
	CFeature *pObj = pFtr;
	if( !pObj )return 0;
	
	CGeometry *pGeo = pObj->GetGeometry();
	if( !pGeo )return 0;
	
	Envelope e;
	e.CreateFromPtAndRadius(spt,r);
	
	PT_KEYCTRL retpt = pGeo->FindNearestKeyCtrlPt(spt,r,cw.m_pSearchCS);
	
	if( retpt.IsValid() )return TRUE;
	return FALSE;	
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CInsertVertexCommand, CActiveStateCommand)

CInsertVertexCommand::CInsertVertexCommand()
{
	m_nStep = 0;
	m_bGetPointZ = TRUE;
}


CInsertVertexCommand::~CInsertVertexCommand()
{
}


CString CInsertVertexCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_INSERTVERTEX);
}


void CInsertVertexCommand::Start()
{
	m_pEditor->OpenSelector(SELMODE_SINGLE, SELFILTER_CURVE|SELFILTER_SURFACE);
	m_nStep = 0;

	CCommand::Start();
}


void CInsertVertexCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_nStep = -1;
	CActiveStateCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}



void CInsertVertexCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CSelection *pselection = m_pEditor->GetSelection();
		CCoordWnd pencode = m_pEditor->GetCoordWnd();
		double r = pselection->GetSelectRadius()*pencode.GetScaleOfSearchCSToViewCS();
		PT_3D spt = pt;
		
		pencode.m_pSearchCS->GroundToClient(&pt,&spt);
		
		CDataQuery *pDQ = m_pEditor->GetDataQuery();
		
		int nsel = 0;
		pselection->GetSelectedObjs(nsel);
		if( nsel==1 )
		{
			CFeature *pFtr =HandleToFtr(pselection->GetLastSelectedObj());
			m_arrFtrs.Add(pFtr);
		}
		else if( nsel>1 )
		{
			Envelope e;
			e.CreateFromPtAndRadius(spt,r);
			m_pEditor->GetDataQuery()->FindObjectInRect(e,pencode.m_pSearchCS);

			int num;
			const CPFeature * ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);

			for (int i=0; i<num; i++)
			{
				if( pselection->IsObjInSelection(FtrToHandle(ftr[i])) )
				{
					m_arrFtrs.Add(ftr[i]);
				}
			}

		}
		GotoState(PROCSTATE_PROCESSING);

		if (m_arrFtrs.GetSize() <= 0)
		{
			return;
		}

		Envelope e;
		e.CreateFromPtAndRadius(spt,r);

		for (int i=0; i<m_arrFtrs.GetSize(); i++)
		{
			CFeature *pFtr = m_arrFtrs[i];
			CGeometry *pGeo = pFtr->GetGeometry();

			if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && 
				!pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) &&
				!pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				m_arrFtrs[i] = NULL;
				continue;
			}

			//是否在有效范围内
			Envelope e;
			e.CreateFromPtAndRadius(spt,r);

			PT_3D nearest;
			if( !pGeo->FindNearestBasePt(spt,e,pencode.m_pSearchCS,&nearest,NULL) )
			{
				m_arrFtrs[i] = NULL;
				continue;
			}

			const CShapeLine *pShape = NULL;
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				pShape = ((CGeoCurve*)pGeo)->GetShape();
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				pShape = ((CGeoSurface*)pGeo)->GetShape();
			else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
				pShape = ((CGeoText*)pGeo)->GetShape();

			//计算插入点的位置
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			int state = 0;
			int key = CLinearizer::FindKeyPosOfBaseLine(spt,pShape,pencode.m_pSearchCS,&state);
			if( key<0 )
			{
				m_arrFtrs[i] = NULL;
				continue;
			}

			int nPtNum = 0;
			if( key==0 && state==-1 )
			{
				nPtNum = 0;
			}
			else if( key+1==arrPts.GetSize()-1 && state==1 )
			{
				nPtNum = key+2;
			}
			else if( state==0 )
			{
				nPtNum = key+1;
			}
			else nPtNum = key+1;

			m_arrPtIndex.Add(nPtNum);
		}

		for (i=m_arrFtrs.GetSize()-1; i>=0; i--)
		{
			if(m_arrFtrs[i]==NULL)
			{
				m_arrFtrs.RemoveAt(i);
			}
		}

		m_nStep = 1;
		m_pEditor->CloseSelector();
	
	}
	else if( m_nStep==1 )
	{
		CUndoBatchAction batchUndo(m_pEditor,Name());

		for (int i=0; i<m_arrFtrs.GetSize(); i++)
		{
			CFeature *pObj = m_arrFtrs[i];
			CGeometry *pGeo = pObj->GetGeometry();
			if( !pGeo )continue;

			m_pEditor->DeleteObject(FtrToHandle(pObj),FALSE);

			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);

			int size = arrPts.GetSize();

			int nPtNum = m_arrPtIndex[i];

			PT_3DEX keyPt;

			if( nPtNum==size )keyPt = arrPts[nPtNum-1];
			else 
			{
				keyPt = arrPts[nPtNum];
				if (nPtNum >= 1)
				{
					keyPt.wid = (arrPts[nPtNum].wid + arrPts[nPtNum-1].wid)/2;
				}
				
			}
			keyPt.x = pt.x;  keyPt.y = pt.y;  keyPt.z = pt.z;
			
			arrPts.InsertAt(nPtNum,keyPt);
			if( !pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize()) )
			{
				continue;
			}

			m_pEditor->RestoreObject(FtrToHandle(pObj));
			
			CUndoVertex undo(m_pEditor,Name());
			undo.handle = FtrToHandle(pObj);
			undo.nPtType = PT_KEYCTRL::typeKey;
			undo.nPtIdx = nPtNum;
			undo.ptNew = keyPt;
			
			batchUndo.AddAction(&undo);
		}
		
		batchUndo.Commit();
		
		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);

		Finish();
		m_nExitCode = CMPEC_STARTOLD;
		m_nStep = 2;
	}
	return;
}


void CInsertVertexCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==1 )
	{
		if(m_arrFtrs.GetSize()>0)
		{
			CGeometry *pObj = m_arrFtrs[0]->GetGeometry()->Clone();
			if(!pObj)return;
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pObj->GetShape(arrPts);

			int nPtNum = m_arrPtIndex[0];
			
			PT_3DEX expt = arrPts.GetAt(nPtNum==0?0:(nPtNum-1));
			COPY_3DPT(expt,pt);
			arrPts.InsertAt(nPtNum,expt);

			pObj->CreateShape(arrPts.GetData(),arrPts.GetSize());

			GrBuffer buf;
			BOOL close = FALSE;
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				close = ((CGeoCurveBase*)pObj)->IsClosed();
			}
			pObj->GetShape()->GetVariantGrBuffer(&buf,close,nPtNum,true);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);	
			
			delete pObj;
		}
	}

	CActiveStateCommand::PtMove(pt);
}


BOOL CInsertVertexCommand::HitTest(PT_3D pt, int state)const
{
	CPFeature pFtr = NULL;

	CSelection *pselection = m_pEditor->GetSelection();
	CCoordWnd pencode = m_pEditor->GetCoordWnd();
	double r = pselection->GetSelectRadius()*pencode.GetScaleOfSearchCSToViewCS();
	PT_3D spt = pt;	
	pencode.m_pSearchCS->GroundToClient(&pt,&spt);
	
	CDataQuery *pDQ = m_pEditor->GetDataQuery();
	
	int nsel = 0;
	pselection->GetSelectedObjs(nsel);
	if( nsel==1 )pFtr = HandleToFtr(pselection->GetLastSelectedObj());
	else if( nsel>1 )
	{
		CPFeature pNearest = pDQ->FindNearestObject(spt,r,pencode.m_pSearchCS);
		
		int nFoundNum = 0;
		const CPFeature *ftrs = pDQ->GetFoundHandles(nFoundNum);
		for( int i=0; i<nFoundNum; i++)
		{
			if( pselection->IsObjInSelection(FTR_HANDLE(ftrs[i])) )
			{
				pFtr = ftrs[i];
				break;
			}
			
		}

	}
	
	CFeature *pObj = pFtr;
	if( !pObj )return 0;
	
	CGeometry *pGeo = pObj->GetGeometry();
	if( !pGeo )return 0;

	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		return 0;
	
	//是否在有效范围内
	Envelope e;
	e.CreateFromPtAndRadius(spt,r);
	
	PT_3D nearest;
	if( !pGeo->FindNearestBasePt(spt,e,pencode.m_pSearchCS,&nearest,NULL) )
	{
		return FALSE;
	}

	PT_KEYCTRL kc = pGeo->FindNearestKeyCtrlPt(spt,max(e.Width(),e.Height()),pencode.m_pSearchCS);
	if( kc.type==PT_KEYCTRL::typeKey && (kc.index==0||kc.index==(pGeo->GetDataPointSum()-1)) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pGeo)->IsClosed() )
			return FALSE;
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			return FALSE;
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDeleteVertexCommand, CEditCommand)

CDeleteVertexCommand::CDeleteVertexCommand()
{
	m_nStep = 0;	
}


CDeleteVertexCommand::~CDeleteVertexCommand()
{
}


CString CDeleteVertexCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DELVERTEX);
}


void CDeleteVertexCommand::Start()
{
	m_pEditor->OpenSelector(SELMODE_SINGLE, SELFILTER_CURVE|SELFILTER_SURFACE);
	m_nStep = 0;

	CEditCommand::Start();
}


void CDeleteVertexCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_nStep = -1;
	CEditCommand::Abort();
}


BOOL CDeleteVertexCommand::DeleteVertex(CEditor *pEditor, CFeature *pFtr, int ptIndex, CUndoBatchAction &batchUndo, LPCTSTR undoName)
{
	if( !pFtr )return FALSE;

	CGeometry *pGeo = pFtr->GetGeometry();
	if( !pGeo )return FALSE;

	int nPtNum = ptIndex;
	
	PT_3DEX keypt = pGeo->GetDataPoint(nPtNum);

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);

	CArray<PT_3DEX,PT_3DEX> arrNewPts;

	BOOL bPtRepeat = FALSE;

	BOOL bDelFtr = FALSE;
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
	{
		if (arrPts.GetSize() <= 1)
		{
			bDelFtr = TRUE;
		}
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
	{
		if (arrPts.GetSize() <= 3)
		{
			bDelFtr = TRUE;
		}
		else if( nPtNum==(arrPts.GetSize()-1) || (nPtNum<(arrPts.GetSize()-1) && arrPts[nPtNum+1].pencode==penMove) )
		{
			arrNewPts.Copy(arrPts);
			for(int j=nPtNum-1; j>=0; j--)
			{
				if(arrPts[j].pencode==penMove)
				{
					break;
				}
			}
			if(j<0)j = 0;
			arrNewPts[j+1].pencode = penMove;
			COPY_3DPT(arrNewPts[nPtNum],arrNewPts[j+1]);
			arrNewPts.RemoveAt(j);

			if(nPtNum-j<=3)
			{
				//需要删除整个子边界
				for(int k=0; k<(nPtNum-j); k++)
				{
					arrNewPts.RemoveAt(j);
				}
			}
		}
		else if(nPtNum==0 || arrPts[nPtNum].pencode==penMove)
		{
			arrNewPts.Copy(arrPts);

			for(int j=nPtNum+1; j<arrPts.GetSize(); j++)
			{
				if(arrPts[j].pencode==penMove)
				{
					break;
				}
			}
			j--;
			arrNewPts[nPtNum+1].pencode = penMove;
			COPY_3DPT(arrNewPts[j],arrNewPts[nPtNum+1]);
			arrNewPts.RemoveAt(nPtNum);	
			
			if(j-nPtNum<=3)
			{
				//需要删除整个子边界
				for(int k=0; k<(j-nPtNum); k++)
				{
					arrNewPts.RemoveAt(nPtNum);
				}
			}
		}
		else
		{
			for(int j=nPtNum+1; j<arrPts.GetSize(); j++)
			{
				if(arrPts[j].pencode==penMove)
				{
					break;
				}
			}
			int nPtLast = j-1;
			for(j=nPtLast; j>=0; j--)
			{
				if(arrPts[j].pencode==penMove)
				{
					break;
				}
			}
			if(j<0)j = 0;

			if(nPtLast-j<=3)
			{
				arrNewPts.Copy(arrPts);
				//需要删除整个子边界
				for(int k=0; k<(nPtLast-j+1); k++)
				{
					arrNewPts.RemoveAt(j);
				}
			}
		}
	}
	else if ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
	{
		if (arrPts.GetSize() <= 4)
		{
			bDelFtr = TRUE;
		}
		if(arrPts[nPtNum].pencode==penMove)
		{
			arrNewPts.Copy(arrPts);
			arrNewPts.RemoveAt(nPtNum);

			if(nPtNum<arrPts.GetSize())
				arrNewPts[nPtNum].pencode = penMove;
		}
	}
	else if ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		if (arrPts.GetSize() <= 2)
		{
			bDelFtr = TRUE;
		}
		if(((CGeoCurveBase*)pGeo)->IsClosed() && (nPtNum==0 || nPtNum==arrPts.GetSize()-1))  
			bPtRepeat = TRUE;
		else
			bPtRepeat = FALSE;
	}

	if (bDelFtr)
	{
		pEditor->DeleteObject(FtrToHandle(pFtr));
		CUndoFtrs undo(pEditor,undoName);
		undo.AddOldFeature(FtrToHandle(pFtr));

		batchUndo.AddAction(&undo);
	}
	else if(arrNewPts.GetSize()>0)
	{
		pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);

		if( !pGeo->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize()) )
		{
			return FALSE;
		}
		
		pEditor->RestoreObject(FtrToHandle(pFtr));
		
		CUndoShape undo(pEditor,undoName);
		undo.arrPtsOld.Copy(arrPts);
		undo.arrPtsNew.Copy(arrNewPts);
		undo.handle = FtrToHandle(pFtr);
		
		batchUndo.AddAction(&undo);				
	}
	else
	{
		pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);

		arrPts.RemoveAt(nPtNum);
		if(bPtRepeat)
		{
			if(nPtNum)  	
			{
				arrPts.RemoveAt(0);
				arrPts.Add(arrPts.GetAt(0));					
			}
			else
			{
				arrPts.SetAt(arrPts.GetSize()-1,arrPts.GetAt(0));
			}
		}

		if( !pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize()) )
		{
			return FALSE;
		}
		
		pEditor->RestoreObject(FtrToHandle(pFtr));
		
		CUndoVertex undo(pEditor,undoName);
		undo.bRepeat = bPtRepeat;
		undo.handle = FtrToHandle(pFtr);
		undo.nPtType = PT_KEYCTRL::typeKey;
		undo.nPtIdx = nPtNum;
		undo.ptOld = keypt;
		
		batchUndo.AddAction(&undo);
	}

	return TRUE;
}


VOID CDeleteVertexCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CSelection *pselection = m_pEditor->GetSelection();
		CCoordWnd pencode = m_pEditor->GetCoordWnd();
		double r = pselection->GetSelectRadius()*pencode.GetScaleOfSearchCSToViewCS();
		PT_3D spt = pt;
		
		pencode.m_pSearchCS->GroundToClient(&pt,&spt);
		
		CDataQuery *pDQ = m_pEditor->GetDataQuery();
		
		int nsel = 0;
		pselection->GetSelectedObjs(nsel);
		if( nsel==1 )
		{
			CFeature *pFtr =HandleToFtr(pselection->GetLastSelectedObj());
			m_arrFtrs.Add(pFtr);
		}
		else if( nsel>1 )
		{
			Envelope e;
			e.CreateFromPtAndRadius(spt,r);
			m_pEditor->GetDataQuery()->FindObjectInRect(e,pencode.m_pSearchCS);

			int num;
			const CPFeature * ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);

			for (int i=0; i<num; i++)
			{
				if( pselection->IsObjInSelection(FtrToHandle(ftr[i])) )
				{
					m_arrFtrs.Add(ftr[i]);
				}
			}

		}
		GotoState(PROCSTATE_PROCESSING);

		if (m_arrFtrs.GetSize() <= 0)
		{
			return;
		}

		Envelope e;
		e.CreateFromPtAndRadius(spt,r);

		for (int i=0; i<m_arrFtrs.GetSize(); i++)
		{
			CFeature *pFtr = m_arrFtrs[i];
			CGeometry *pGeo = pFtr->GetGeometry();
			
			Envelope e;
			e.CreateFromPtAndRadius(spt,r);
			
			PT_KEYCTRL keyctrl = pGeo->FindNearestKeyCtrlPt(spt,r,m_pEditor->GetCoordWnd().m_pSearchCS,1);
			if( !keyctrl.IsValid() )
			{
				m_arrFtrs[i] = NULL;
			}
			
			//计算删除点的位置
			m_arrPtIndex.Add(keyctrl.index);
		}
		
		m_nStep = 1;
		m_pEditor->CloseSelector();
	}
	
	if( m_nStep==1 )
	{
		CUndoBatchAction batchUndo(m_pEditor,Name());
		for (int i=0; i<m_arrFtrs.GetSize(); i++)
		{
			CFeature *pFtr = m_arrFtrs[i];
			if( !pFtr )return;

			CGeometry *pGeo = pFtr->GetGeometry();
			if( !pGeo )return;

			int nPtNum = m_arrPtIndex[i];

			DeleteVertex(m_pEditor,pFtr,nPtNum,batchUndo,Name());
		}

		batchUndo.Commit();

		m_pEditor->OnSelectChanged(TRUE);	
		
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
	
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMoveCommand,CCommand)

CMoveCommand::CMoveCommand()
{
	m_nStep = 0;
}


CMoveCommand::~CMoveCommand()
{
}


CString CMoveCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_MOVE);
}


void CMoveCommand::Start()
{
	m_pEditor->OpenSelector();
	m_nStep = 0;

	CEditCommand::Start();
}


void CMoveCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_nStep = -1;
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}



void CMoveCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		m_ptMoveStart = pt;
		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num>0 )
		{
			if( flag==SELSTAT_POINTSEL );
			else if( !CanGetSelObjs(flag) )return;
			m_arrObjs.SetSize(num);
			memcpy(m_arrObjs.GetData(),handles,sizeof(FTR_HANDLE)*num);
			m_nStep = 1;

			m_pEditor->CloseSelector();
		}
	}
	else if( m_nStep==1 )
	{	
		double matrix[16];
		Matrix44FromMove(pt.x-m_ptMoveStart.x,pt.y-m_ptMoveStart.y,pt.z-m_ptMoveStart.z,matrix);

		CArray<FTR_HANDLE,FTR_HANDLE> arrObjs;
		int num = m_arrObjs.GetSize();
		for( int i=0; i<num; i++)
		{
			FTR_HANDLE hdle = m_arrObjs.GetAt(i);

			m_pEditor->DeleteObject(hdle,FALSE);
			
			CGeometry *pGeo = HandleToFtr(hdle)->GetGeometry();
			pGeo->Transform(matrix);

			m_pEditor->RestoreObject(hdle);
			//m_pEditor->UpdateObject(m_arrObjs.GetAt(i));

			arrObjs.Add(m_arrObjs.GetAt(i));
		}
		
		CUndoTransform undo(m_pEditor,Name());
		memcpy(undo.matrix,matrix,sizeof(matrix));
		undo.arrHandles.Copy(arrObjs);
		undo.Commit();

		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
	return;
}

void CMoveCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==1 )
	{
		GrBuffer buf;
		
		double matrix[16];
		Matrix44FromMove(pt.x-m_ptMoveStart.x,pt.y-m_ptMoveStart.y,pt.z-m_ptMoveStart.z,matrix);
		
		int num = m_arrObjs.GetSize();
		for( int i=0; i<num && num<100; i++)
		{
			CFeature *pFtr = HandleToFtr(m_arrObjs.GetAt(i))->Clone();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			pGeo->Transform(matrix);
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				CPlotTextEx plot;
				TEXT_SETTINGS0  settings;
				((CGeoText*)pGeo)->GetSettings(&settings);
				plot.SetSettings(&settings);	
			
				plot.SetShape(((CGeoText*)pGeo)->GetShape());
				plot.SetText(((CGeoText*)pGeo)->GetText());
				plot.GetOutLineBuf(&buf);
			}
			else
			pFtr->Draw(&buf);
			
			delete pFtr;
			
		}
		
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptMoveStart);
		buf.LineTo(&pt);
		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
}

//////////////////////////////////////////////////////////////////////
// CActiveStateCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CActiveStateCommand,CCommand)


//////////////////////////////////////////////////////////////////////
// CDrawCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDrawCommand,CCommand)

CDrawCommand::CDrawCommand()
{
	m_pFtr = NULL;
	m_bReadCmdParas = FALSE;
}

void CDrawCommand::Start()
{
//	m_pEditor->SetCurDrawingObj(DrawingInfo(m_pFtr->GetGeometry(),-1));
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_Attach_Accubox);	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CCommand::Start();
}

void CDrawCommand::Abort()
{
//	m_pEditor->SetCurDrawingObj(DrawingInfo());
	m_pEditor->UpdateDrag(ud_Detach_Accubox);
	CCommand::Abort();	
}

void CDrawCommand::Finish()
{	
	m_pEditor->UpdateDrag(ud_Detach_Accubox);

	m_pFtr = NULL;
	CCommand::Finish();
}

BOOL CDrawCommand::AddObject(CPFeature pFtr, int layid)
{
	return m_pEditor->AddObject(pFtr,layid);
}



MyNameSpaceEnd


