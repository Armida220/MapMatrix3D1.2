#include "stdafx.h"
#include "EditBase.h"
#include "Command.h"
#include "Editor.h"
#include "GeoCurve.h"
#include "RegDef.h"
#include "RegDef2.h"
#include "UIParam2.h"
#include "Linearizer.h"
#include "DataSource.h"
#include "Functions_temp.h "
#include "SmartViewFunctions.h"
#include "Resource.h"
#include "DlgCommand.h"
#include "MainFrm.h"
#include "ExMessage.h"
#include "PropertiesViewBar.h"
#include "EditBasedoc.h"
#include "DlgDataSource.h "
#include "ConditionSelect.h "
#include "StreamCompress.h "
#include "Container.h" 
#include "GeoText.h "
#include "GrTrim.h "
#include "Scheme.h "
#include "SymbolLib.h "
#include "PlotText.h "
#include "DlgInputData.h"
//#include "GeoSurface.h "
#include <math.h>
#include "DlgSetXYZ.h"
#include "..\CORE\viewer\EditBase\res\resource.h"

#include "CommandLidar.h"

#include "DlgChangeFCode.h"
#include <map>
#include <list>
#include <set>
#include "baseview.h"
#include "DlgOpSettings.h"
#include "SQLiteAccess.h"
#include "Mapper.h"
#include "DlgEditText.h"

#include "EditBase.h"
#include "DlgBatExportMaps.h"

#include "UIFBoolProp.h"
#include "PlotWChar.h"
#include "DlgInputCode.h"
#include "DlgSymbolTable.h"
#include "SymbolBase.h"

#include "Polygonwork.h"
#include "PolygonBooleanHander.h"
#include "GeoSurfaceBooleanHandle.h"
#include "vector2d.h"
#include "TINHandler.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  MAXDISTANCE 99999999
#define  INTPAPI_SUCESS  0
#define  INTPAPI_INVALIDE_PARAM  1
#define  INTPAPI_ERROR  2

int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf, LPNEWTEXTMETRIC lpntm, DWORD nFontType, LONG_PTR lparam);
extern BOOL convertStringToStrArray(LPCTSTR str,CStringArray &arr);

static bool GLineIntersectLineSeg1(double x0,double y0, double x1, double y1,double x2,double y2,double z2,double x3,double y3, double z3,double *x, double *y,double *z, double *t,double *t3)
{
	double vector1x = x1-x0, vector1y = y1-y0;
	double vector2x = x3-x2, vector2y = y3-y2,vector2z=z3-z2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if( delta<1e-6 && delta>-1e-6 )return false;         //平行无交点
	double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
	double xr = x0 + t1*vector1x, yr = y0 + t1*vector1y;
	if( t1<0 || t1>1 )
	{
		if( fabs(xr-x0)<=1e-6 && fabs(yr-y0)<=1e-6 );
		else if( fabs(xr-x1)<=1e-6 && fabs(yr-y1)<=1e-6 );
		else return false;
	}
	
	double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
	
	if( t2<0 || t2>1 )
	{
		if( fabs(xr-x2)<=1e-6 && fabs(yr-y2)<=1e-6 );
		else if( fabs(xr-x3)<=1e-6 && fabs(yr-y3)<=1e-6 );
		else return false;
	}
	
	if(x)*x = x0 + t1*vector1x;
	if(y)*y = y0 + t1*vector1y;
	if(z)*z = z2 + t2*vector2z;
	if(t)*t = t1;
	if (t3)
	{
		*t3=t2;
	}
	return true;
}


CString ChangeXYPrecesionFormat(int string_ID)
{
	CString strFormat = StrFromResID(string_ID);
	int nxy = floor(-log10(GraphAPI::g_lfDisTolerance)+0.5);
	int nz = floor(-log10(GraphAPI::g_lfZTolerance)+0.5);

	CString strFormat2;
	strFormat2.Format("%%.%df",nxy);

	strFormat.Replace("%.4f",strFormat2);
	return strFormat;
}



extern BOOL TrimRZero(CString &str);
//////////////////////////////////////////////////////////////////////
// CSmoothCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSmoothCommand,CEditCommand)

CSmoothCommand::CSmoothCommand()
{
	m_nMode=modeWhole;
	m_nPartWay=modeDrag;
	m_lfLimit = 0.1;
	strcat(m_strRegPath,"\\Smooth");
}

CSmoothCommand::~CSmoothCommand()
{

}



CString CSmoothCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SMOOTHCURVES);
}

void CSmoothCommand::Start()
{
	if( !m_pEditor )return;	

	
	m_nStep = 0;
	m_arrPts.RemoveAll();
	m_nMode=modeWhole;

	CCommand::Start();
	if (m_nMode!=modeWhole)
	{
		m_pEditor->DeselectAll();
		m_pEditor->CloseSelector();
	}
	else
	{
		m_pEditor->OpenSelector();
		
	}
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{		
		CCommand::Start();		
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	if (m_nMode==modePart&&m_nPartWay==modeTwoPT)
	{		
	   ;
	}	
		
}

void CSmoothCommand::Abort()
{	
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CSmoothCommand::Finish()
{

	CEditCommand::Finish();
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


void CSmoothCommand::GetPartObjs(CArray<OBJ_PART,OBJ_PART> *arr)
{
	//粗略查找第一条线段的相交地物
	Envelope e1, e2,e3,e4;
	e1.CreateFromPts(m_ptLine,2);
	e3 = e1;
	e3.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e3,m_pEditor->GetCoordWnd().m_pSearchCS);
	
	int num1;
	const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
	if( !ppftr1  )
	{	
		return;
	}
	CArray<CPFeature,CPFeature> ftr1;
	ftr1.SetSize(num1);
	memcpy(ftr1.GetData(),ppftr1,num1*sizeof(*ppftr1));
	
	
	//粗略查找第二条线段的相交地物
	e2.CreateFromPts(m_ptLine+2,2);
	e4 = e2;
	e4.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e4,m_pEditor->GetCoordWnd().m_pSearchCS);
	
	int num2;
	const CPFeature * ppftr2 = m_pEditor->GetDataQuery()->GetFoundHandles(num2);
	if( !ppftr2  )
	{	
		return;
	}
	CArray<CPFeature,CPFeature> ftr2;
	ftr2.SetSize(num2);
	memcpy(ftr2.GetData(),ppftr2,num2*sizeof(*ppftr2));
	

	
	//获得有效的对象与两线段的相交信息，并排序
	CArray<OBJ_ITEM,OBJ_ITEM> arr1, arr2;
	OBJ_ITEM item;
	CGeometry *pObj;
	const CShapeLine *pSL;
	PT_3DEX *pts;
	int size1, size2;
	for( int i=0; i<num1; i++)
	{
		pObj = ftr1[i]->GetGeometry();
		if( !pObj )continue;
		
		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;

		if( pObj->GetDataPointSum()<2 )continue;
		
		//精细检验是否与第一线段相交，记录交点
		pSL = pObj->GetShape();
		if( !pSL )continue;
		
		PT_3D ret;
		double t=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k =0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e1.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if(/* pts[1].code==GRBUFFER_PTCODE_LINETO &&*/ e1.bIntersect(&pts[0],&pts[1]) )
					{
						//求相交点
						if( !CGrTrim::Intersect(m_ptLine,m_ptLine+1,&pts[0],&pts[1],&ret,&t) )
							continue;
						

						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						
						item.id = FtrToHandle(ftr1[i]);item.pos= pos+j; item.ret= ret; item.t = t;
						
						//按照 t 大小排序插入
						size1 = arr1.GetSize();
						for( int m=0; m<size1 && item.t>=arr1[m].t; m++);
						if( m<size1 )arr1.InsertAt(m,item);
						else arr1.Add(item);
						
						goto FINDOVER_1;
					}
				}
			}
			pos += pUnit->nuse;		
		}
FINDOVER_1:;
	}
	
	for( i=0; i<num2; i++)
	{
		pObj = ftr2[i]->GetGeometry();
		if( !pObj )continue;
		
		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;

		if( pObj->GetDataPointSum()<2 )continue;
		
		//精细检验是否与第二线段相交，记录交点
		pSL = pObj->GetShape();
		if( !pSL )continue;
		
		PT_3D ret;
		double t=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k=0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e2.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if( /*pts[1].code==GRBUFFER_PTCODE_LINETO && */e2.bIntersect(&pts[0],&pts[1]) )
					{
						//求相交点
						if( !CGrTrim::Intersect(m_ptLine+2,m_ptLine+3,&pts[0],&pts[1],&ret,&t) )
							continue;						

						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						
						item.id = FtrToHandle(ftr2[i]); item.pos= pos+j; item.ret= ret; item.t = t;
						
						//按照 t 大小排序插入
						size1 = arr2.GetSize();
						for( int m=0; m<size1 && item.t>=arr2[m].t; m++);
						if( m<size1 )arr2.InsertAt(m,item);
						else arr2.Add(item);
						
						goto FINDOVER_2;
					}
				}
			}
			pos += pUnit->nuse;
		}
FINDOVER_2:;
	}
	

	
	
	OBJ_PART part;
	//对排序好的两个地物组作匹配合并，结果保存在 arr 中
	size1 = arr1.GetSize(); size2 = arr2.GetSize();
	for( i=0; i<size1; i++)
	{
		FTR_HANDLE id = arr1[i].id;
		for( int j=0; j<size2 && id!=arr2[j].id; j++);
		if( j<size2 )
		{
			part.id = id;
			part.pos1 = arr1[i].pos; part.pos2 = arr2[j].pos;
			part.pt1 = arr1[i].ret; part.pt2 = arr2[j].ret; 
			if(arr) arr->Add(part);
		}
	}
	
}


void CSmoothCommand::PtClick(PT_3D &pt, int flag)
{
	//整体光滑
	if(m_nMode==modeWhole)
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag) )return;
			EditStepOne();
		}
		
		if( m_nStep==1 )
		{
			CGeometry *pObj;
			int num;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
// 			m_pEditor->DeselectAll();
// 			m_pEditor->OnSelectChanged();
// 			CArray<OBJ_GUID,OBJ_GUID> idsold;
// 			idsold.Copy(m_idsOld);
/*			m_idsOld.RemoveAll();*/
			CUndoFtrs undo(m_pEditor,Name());
			
			for( int i=num-1; i>=0; i--)
			{
				pObj = HandleToFtr(handles[i])->GetGeometry();
				
				if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
				{
					CArray<PT_3DEX,PT_3DEX> pts;
					pObj->GetShape(pts);

					int num = pts.GetSize();
					if( num<=2 )continue;
					
					//判断是否全为线串码
					PT_3DEX expt;
					for( int j=1; j<num; j++)
					{
						expt = pts[j];
						if( expt.pencode!=penLine && expt.pencode!=penStream )
							break;
					}
					
					if( j<num )
					{
						continue;
					}
					CGeometry *pNewObj = pObj->Clone();
					CArray<PT_3DEX,PT_3DEX> arr;
			
					
					//弦高压缩
					PT_3D pt3d;
					CStreamCompress2_PT_3D comp;
					comp.BeginCompress(m_lfLimit);
					
					for( j=0; j<num; j++)
					{
						expt = pts[j];
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
							arr.Add(expt);
						}
					}
					
					//取点
					num = arr.GetSize();
					CArray<PT_3DEX,PT_3DEX>  arrRets;
				
					
					//光滑
					SmoothLines(arr.GetData(),num,m_lfLimit,arrRets);
					
					num = arrRets.GetSize();
					
					//添加结果
					if( num>=2)
					{
						pNewObj->CreateShape(arrRets.GetData(),num);
						CFeature *pFtr = HandleToFtr(handles[i])->Clone();
						if (!pFtr) return;
						pFtr->SetID(OUID());
						pFtr->SetGeometry(pNewObj);						
						
						m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(handles[i]));

						GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pFtr);
						undo.arrNewHandles.Add(FtrToHandle(pFtr));

						m_pEditor->DeleteObject(handles[i]);
						undo.arrOldHandles.Add(handles[i]);
					}				
				}
			}
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			undo.Commit();
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 2;
		}
	}
	//当地物完整的落在方框内时，整体光滑
	//部分落在方框内时，部分光滑
	//部分光滑再加上另一种方式
	else
	{
		if(m_nPartWay==modeDrag)
		{
			if (m_nStep==0)
			{
				m_ptStart=pt;
				GotoState(PROCSTATE_PROCESSING);
				m_nStep = 1;

			}
			else if (m_nStep==1)
			{
				m_pEditor->UpdateDrag(ud_ClearDrag);
				CGeometry *pObj;
				m_ptEnd=pt;
				//查询在拉框内的地物
				Envelope e;
				e.m_xl=(m_ptStart.x+m_ptEnd.x)/2-fabs(m_ptStart.x-m_ptEnd.x)/2;
				e.m_xh=(m_ptStart.x+m_ptEnd.x)/2+fabs(m_ptStart.x-m_ptEnd.x)/2;
				e.m_yl=(m_ptStart.y+m_ptEnd.y)/2-fabs(m_ptStart.y-m_ptEnd.y)/2;
				e.m_yh=(m_ptStart.y+m_ptEnd.y)/2+fabs(m_ptStart.y-m_ptEnd.y)/2;
				e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
				m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);
				int num;
				const CPFeature * ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);
				CUndoFtrs undo(m_pEditor,Name());
			    for( int i=num-1; i>=0; i--)
				{
				    pObj = ftr[i]->GetGeometry();
					
					if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
					{
						CArray<PT_3DEX,PT_3DEX> pts;
						pObj->GetShape(pts);
						
						int num0 = pts.GetSize();

						if( num0<=2 )continue;
						
						//判断是否全为线串码
						PT_3DEX expt,expt1;
						expt1.pencode = penLine;
						for( int j=1; j<num0; j++)
						{
							expt = pts[j];
							if( expt.pencode!=penLine && expt.pencode!=penStream )
								break;
						}
						
						if( j<num0)
						{
							continue;
						}
						CGeometry *pNewObj = pObj->Clone();
						CArray<PT_3DEX,PT_3DEX> arr,arrPts,arrRets;
						
						
						//弦高压缩
						PT_3D pt3d;
						CStreamCompress2_PT_3D comp;
						
						bool flag=false;
						int count=0;
						for( j=0; j<num0; j++)
						{
							
							expt = pts[j];
							COPY_3DPT(pt3d,expt);
							if(GraphAPI::GIsPtInRange(m_ptStart,m_ptEnd,pt3d))
							{
 								if (flag!=true)
 								{   
// 									comp.BeginCompress(m_lfLimit);
									arr.RemoveAll();
									arrRets.RemoveAll();
 									flag=true;
 								}
								arr.Add(expt);
								//comp.AddStreamPt(pt3d);
							}
							else
							{
								if (flag!=false)
								{
									//comp.EndCompress();
									flag=false;
									if(arr.GetSize()>=3)
									{
										comp.BeginCompress(m_lfLimit);
										for (int k=0;k<arr.GetSize();k++ )
										{
											comp.AddStreamPt(arr[k]);
										}
										comp.EndCompress();
										//取出压缩点
										arr.RemoveAll();
										{
											PT_3D *tpts;
											int ti, tnpt;
											comp.GetPts(tpts,tnpt);
											for( ti=0; ti<tnpt; ti++)
											{
												COPY_3DPT(expt1,tpts[ti]);
												arr.Add(expt1);
											}
											
											count = tnpt;
										}
										SmoothLines(arr.GetData(),count,m_lfLimit,arrRets);
										count=0;
										arrPts.Append(arrRets);

									}
									else
									{
										arrPts.Append(arr);
									}
								}

								arrPts.Add(expt);
							}
						}
						if (flag!=false)
						{
							//comp.EndCompress();
							flag=false;
							if(arr.GetSize()>=3)
							{
								comp.BeginCompress(m_lfLimit);
								for (int k=0;k<arr.GetSize();k++ )
								{
									comp.AddStreamPt(arr[k]);
								}
								comp.EndCompress();
								//取出压缩点
								arr.RemoveAll();
								{
									PT_3D *tpts;
									int ti, tnpt;
									comp.GetPts(tpts,tnpt);
									for( ti=0; ti<tnpt; ti++)
									{
										COPY_3DPT(expt,tpts[ti]);
										arr.Add(expt);
									}
									
									count = tnpt;
								}
								SmoothLines(arr.GetData(),count,m_lfLimit,arrRets);
								count=0;
								arrPts.Append(arrRets);
								
							}
							else
							{
								arrPts.Append(arr);
							}
						}				
						//添加结果
						if( arrPts.GetSize()>=2)
						{
							CFeature *pFtr = ftr[i]->Clone();
							if(!pFtr) return;
							pNewObj->CreateShape(arrPts.GetData(),arrPts.GetSize());
							pFtr->SetGeometry(pNewObj);
							m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(ftr[i])));
							GETXDS(m_pEditor)->CopyXAttributes(ftr[i],pFtr);
							undo.arrNewHandles.Add(FtrToHandle(pFtr));

							m_pEditor->DeleteObject(FtrToHandle(ftr[i]));
							undo.arrOldHandles.Add(FtrToHandle(ftr[i]));
						}						
					}
				}
				undo.Commit();
				m_pEditor->RefreshView();
				
				m_nStep = 2;
				Finish();

			}
		}
		else
		{
			//分别在m_nStep=0和m_nStep=1时绘制两条线段
			if( m_nStep<=1 )
			{	

				m_arrPts.Add(pt);
				
				GotoState(PROCSTATE_PROCESSING);
				
				if( m_arrPts.GetSize()>=2 )
				{
					GrBuffer vbuf;
					vbuf.BeginLineString(RGB(255,0,0),0);
					vbuf.MoveTo(&m_arrPts[0]);
					vbuf.LineTo(&m_arrPts[1]);
					vbuf.End();
					
					m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
					m_pEditor->UpdateDrag(ud_AddConstDrag,&vbuf);
					
					m_ptLine[m_nStep*2] = m_arrPts[0];
					m_ptLine[m_nStep*2+1] = m_arrPts[1];
									
					m_nStep++;
					m_arrPts.RemoveAll();
					

				}
				

			}
			else if (m_nStep==2)
			{
				//清除显示
			
				//m_pDoc->SetCurDrawingObj(DrawingInfo());
				m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			    m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
                //得到与两条线都相交的匹配的地物

				CGeometry *pObj=NULL;
				CArray<OBJ_PART,OBJ_PART> arr;
				GetPartObjs(&arr);
				CUndoFtrs undo(m_pEditor,Name());
				int num=arr.GetSize();
				for (int i=0;i<num;i++)
				{
					pObj = HandleToFtr(arr[i].id)->GetGeometry();
					if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
					{
						CArray<PT_3DEX,PT_3DEX> pts;
						pObj->GetShape(pts);
						
						int num = pts.GetSize();

						if( num<=2 )continue;
						
						//判断是否全为线串码
						PT_3DEX expt,expt1;
						for( int j=1; j<num; j++)
						{
							expt = pts[j];
							if( expt.pencode!=penLine && expt.pencode!=penStream )
								break;
						}
						
						if( j<num )
						{
							continue;
						}
						CGeometry *pNewObj = pObj->Clone();
					//for( j=num-1; j>=0; j--)pNewObj->DeletePt(j);
						CArray<PT_3DEX,PT_3DEX> arrPts;

						//弦高压缩    以下需要更改
						PT_3D pt3d;
						int min_idx=min((arr.GetAt(i)).pos1,(arr.GetAt(i)).pos2);
						int max_idx=max((arr.GetAt(i)).pos1,(arr.GetAt(i)).pos2);
						CArray<int ,int > ip;
						pObj->GetShape()->GetKeyPosOfBaseLines(ip);
						int Keypos_sum = num;
						for (int l=0;min_idx>=ip[l];l++);
						min_idx=l-1;
						for (l=0;max_idx>=ip[l];l++);
						max_idx=l-1;	
						
						CStreamCompress2_PT_3D comp;
						comp.BeginCompress(m_lfLimit);
					
						for( j=min_idx; j<=max_idx+1; j++)
						{
							expt = pts[j];
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
								arrPts.Add(expt);
							}
						}
						
						//取点
						num = arrPts.GetSize();
						CArray<PT_3DEX,PT_3DEX>  arrRets;
// 						arrPts.SetSize(num);
// 						for( j=num-1; j>=0; j--)
// 						{
// 							pNewObj->GetPt(j,&expt);
// 							pNewObj->DeletePt(j);
// 							arrPts.SetAt(j,expt);
// 						}
// 						
						//光滑
						SmoothLines(arrPts.GetData(),num,m_lfLimit,arrRets);
						arrPts.RemoveAll();
						for (j=0;j<min_idx;j++)
						{
							expt = pts[j];
							arrPts.Add(expt);
						}
						num = arrRets.GetSize();
						for( j=0; j<num; j++)
						{
							expt = arrRets.GetAt(j);
						    arrPts.Add(expt);
						}
						for (j=max_idx+2;j<pObj->GetDataPointSum();j++)
						{
							expt = pts[j];
							arrPts.Add(expt);
						}
						
						//添加结果
						if( arrPts.GetSize()>=2 )
						{
							pNewObj->CreateShape(arrPts.GetData(),arrPts.GetSize());
							CFeature *pFtr = HandleToFtr(arr[i].id)->Clone();
							pFtr->SetID(OUID());
							pFtr->SetGeometry(pNewObj);
							m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(arr[i].id));
							GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(arr[i].id),pFtr);
							undo.arrNewHandles.Add(FtrToHandle(pFtr));
							m_pEditor->DeleteObject(arr[i].id);
							undo.arrOldHandles.Add(arr[i].id);
						}
						else
						{							
							delete pNewObj;
						}
					}
				}
				m_nStep++;
				Finish();
				undo.Commit();
				m_pEditor->RefreshView();
				
			}
			CCommand::PtClick(pt,flag);
			return;
		   
		}
	
	}	

	CEditCommand::PtClick(pt,flag);
}

void CSmoothCommand::PtMove(PT_3D &pt)
{
	if(m_nMode ==modePart)
	{
		if(m_nPartWay==modeDrag)
		{
			if(m_nStep==1)
			{
				GrBuffer buf;
				buf.BeginLineString(0,0);
				buf.MoveTo(&m_ptStart);
				PT_3D tem;
				tem.x=pt.x;
				tem.y=m_ptStart.y;
				tem.z=m_ptStart.z;
				buf.LineTo(&tem);
				buf.LineTo(&pt);
				tem.x=m_ptStart.x;
				tem.y=pt.y;
				tem.z=m_ptStart.z;
				buf.LineTo(&tem);
				buf.LineTo(&m_ptStart);
				buf.End();
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf); 		
			}		
		}
		else
		{
			
			if(  GetState()==PROCSTATE_FINISHED || m_arrPts.GetSize()<=0 )return;
			
			if(m_arrPts.GetSize()==1)
			{
				m_arrPts.Add(pt);
				GrBuffer vbuf;
				vbuf.BeginLineString(0,0);
				vbuf.MoveTo(&m_arrPts[0]);
				vbuf.LineTo(&m_arrPts[1]);
				vbuf.End();
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
				m_arrPts.RemoveAt(1);

			}
			CCommand::PtMove(pt);
		}
	}

}


void CSmoothCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue(PF_SMOOTHWAY,&CVariantEx(var));
	var = (long)(m_nPartWay);
	tab.AddValue(PF_SMOOTHSELMODE,&CVariantEx(var));
	var = (double)(m_lfLimit);
	tab.AddValue(PF_SMOOTHTOLER,&CVariantEx(var));

}


void CSmoothCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("SmoothCommand",StrFromLocalResID(IDS_CMDNAME_SMOOTHCURVES));
	param->BeginOptionParam(PF_SMOOTHWAY,StrFromResID(IDS_CMDPLANE_SMOOTHWAY));
	param->AddOption(StrFromResID(IDS_WHOLEMODE),modeWhole,' ',m_nMode==modeWhole);
	param->AddOption(StrFromResID(IDS_PARTMODE),modePart,' ',m_nMode==modePart);	
	param->EndOptionParam();
	
	if(m_nMode==modePart||bForLoad)
	{
			
		param->BeginOptionParam(PF_SMOOTHSELMODE,StrFromResID(IDS_CMDPLANE_SELMODE));
		param->AddOption(StrFromResID(IDS_DRAGMODE),modeDrag,' ',m_nPartWay==modeDrag);
		param->AddOption(StrFromResID(IDS_TWOPT),modeTwoPT,' ',m_nPartWay==modeTwoPT);
		param->EndOptionParam();
	}
	param->AddParam(PF_SMOOTHTOLER,double(m_lfLimit),StrFromResID(IDS_CMDPLANE_TOLER));

}


void CSmoothCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_SMOOTHWAY,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_SMOOTHSELMODE,var) )
	{
		m_nPartWay = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_SMOOTHTOLER,var) )
	{
		m_lfLimit = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (m_nMode==modePart)
	{
		m_pEditor->CloseSelector();
		SetSettingsModifyFlag();
	}
	else
	{
		m_pEditor->OpenSelector();
	}
	CEditCommand::SetParams(tab,bInit);
}



//////////////////////////////////////////////////////////////////////
// CFCodeChgAllCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////









	
// 	CFtrLayer *pLayer1 = pDS->GetFtrLayerByNameOrLayerIdx(code1);	
// 	CFtrLayer *pLayer2 = pDS->GetFtrLayerByNameOrLayerIdx(code2);
	
	//获取实体对象总数



// 		else if (nCls==CLS_GEOSURFACE||nCls==CLS_GEOMULTISURFACE)
// 		{
// 		}







	
	
	





 //	param->SetOwnerID("DrawTextCommand",StrFromLocalResID(IDS_CMDNAME_DRAWTEXT));





//////////////////////////////////////////////////////////////////////
// CFormatFtrCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CFormatFtrCommand,CEditCommand)

CFormatFtrCommand::CFormatFtrCommand()
{
	m_nStep = -1;

}

CFormatFtrCommand::~CFormatFtrCommand()
{
	
}



CString CFormatFtrCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_FORMATFTR);
}

void CFormatFtrCommand::Start()
{
	if( !m_pEditor )return;
//	m_pEditor->DeselectAll();
	CEditCommand::Start();
	
	m_pEditor->OpenSelector(SELMODE_MULTI);
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num==1 )
	{
		
		m_handleRefer = m_pEditor->GetSelection()->GetLastSelectedObj();
		CFeature *pp = HandleToFtr(m_handleRefer);
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 1;
		
		//开始选择目标地物
	//	OutputTipString(StrFromResID(IDS_CMDTIP_SELECTTARGET));
	}
	else
	{
	//	OutputTipString(StrFromResID(IDS_CMDTIP_SELECTREFER));
	}
}

void CFormatFtrCommand::Abort()
{
	m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();
	
	CEditCommand::Abort();
}



void CFormatFtrCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( (flag&SELSTAT_POINTSEL)==0 && (flag&SELSTAT_DRAGSEL)==0 )
			return;
		int num;
		m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num!=1 )
			return;		
		
		m_handleRefer = m_pEditor->GetSelection()->GetLastSelectedObj();
		
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 1;
		
		//开始选择目标地物
		PromptString(StrFromResID(IDS_CMDTIP_SELECTTARGET));
	}
	else if( m_nStep==1 )
	{
		if( (flag&SELSTAT_MULTISEL)!=0 && (flag&SELSTAT_POINTSEL)==0 && (flag&SELSTAT_DRAGSEL_RESTART)==0 )
			return;
		
		CUndoFtrs undo(m_pEditor,Name());

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS) return;
	
		CFeature *pFtr = HandleToFtr(m_handleRefer);
		CGeometry *pGeo = pFtr->GetGeometry();
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pFtrLayer) return;

		int layerID = pFtrLayer->GetID();
		CValueTable table;
		table.BeginAddValueItem();
		pFtr->WriteTo(table);
		table.EndAddValueItem();

		if (pGeo->GetColor() == COLOUR_BYLAYER)
		{	
			table.SetValue(0,FIELDNAME_GEOCOLOR,&CVariantEx((_variant_t)(long)(pFtrLayer->GetColor())));
		}
		//by mzy，解决格式刷刷平行线时同时改变“平行线宽度”问题
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
			table.DelField("Width");
		//
	    table.DelField(FIELDNAME_SHAPE);
		table.DelField(FIELDNAME_GEOCURVE_CLOSED);
		table.DelField(FIELDNAME_FTRID);
		table.DelField(FIELDNAME_GEOCLASS);
		table.DelField(FIELDNAME_GEOTEXT_CONTENT);
		table.DelField(FIELDNAME_FTRDISPLAYORDER);
// 		table.DelField(FIELDNAME_GEOPOINT_WIDTH);
// 		table.DelField(FIELDNAME_GEOCURVE_LINEWIDTH);
// 		table.DelField(FIELDNAME_GEOSURFACE_LINEWIDTH);
		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		for( int i=num-1; i>=0; i--)
		{
			if( handles[i] && m_handleRefer!=handles[i] )
			{
				CFeature *pNewFtr = HandleToFtr(handles[i])->Clone();
				
				CGeometry *pNewGeo = pNewFtr->GetGeometry();
				CArray<PT_3DEX,PT_3DEX> pts;
				pNewGeo->GetShape(pts);
				int num = pts.GetSize();
				int nGeoType = pGeo->GetClassType(), nNewGeoType = pNewGeo->GetClassType();
				if (nGeoType != nNewGeoType)
				{
					if ((pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && nNewGeoType != CLS_GEODIRPOINT && num < 2) || (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && num < 3))
					{
						delete pNewFtr;
						continue;
					}
					else
					{
						CGeometry *pTmpGeo = (CGeometry*)CPermanent::CreatePermanentObject(nGeoType);//GCreateGeometry(nGeoType);
						if (!pTmpGeo) 
						{
							delete pNewFtr;
							continue;
						}

						// 有向点->线
						if (nGeoType == CLS_GEOCURVE && nNewGeoType == CLS_GEODIRPOINT)
						{
							CGeoDirPoint *pDir = (CGeoDirPoint*)pNewGeo;
							if (!pDir) continue;
							pts.Add(PT_3DEX(pDir->GetCtrlPoint(0),penLine));
						}

						if (!pTmpGeo->CreateShape(pts.GetData(),pts.GetSize()))
						{
							delete pNewFtr;
							delete pTmpGeo;
							continue;
						}
						pNewFtr->SetGeometry(pTmpGeo);

						BOOL bValid = TRUE;
						if( pTmpGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
						{
							if( ((CGeoText*)pTmpGeo)->GetText().IsEmpty() )
								bValid = FALSE;
						}
						else if( pTmpGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
						{
							if( pTmpGeo->GetDataPointSum()<2 )
								bValid = FALSE;							
						}

						if( !bValid )
						{
							delete pNewFtr;
							continue;							
						}
					}
				}
				
				CValueTable tab;
				tab.BeginAddValueItem();
				GETXDS(m_pEditor)->GetXAttributes(pFtr,tab );
				tab.EndAddValueItem();
				
				m_pEditor->DeleteObject(handles[i]);
				undo.AddOldFeature(handles[i]);
				
				pNewFtr->ReadFrom(table);
				pNewFtr->SetID(OUID());
				m_pEditor->AddObject(pNewFtr,layerID);
				undo.AddNewFeature(FtrToHandle(pNewFtr));
				GETXDS(m_pEditor)->SetXAttributes(pNewFtr,tab );
			}
		}
		
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->SelectObj(m_handleRefer);
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		m_nStep = 1;

		PromptString(StrFromResID(IDS_CMDTIP_SELECTTARGET));
	
	}	
	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CTrimCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTrimCommand,CEditCommand)

CTrimCommand::CTrimCommand()
{
//	m_bAutoClosed = TRUE;
	m_bTrimOut = TRUE;
	m_nStep = -1;
	m_nBoundType = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	m_bTrimClosedCurve = FALSE;
	m_bTrimSurface = FALSE;
	strcat(m_strRegPath,"\\Trim");
}

CTrimCommand::~CTrimCommand()
{
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
	}
}



CString CTrimCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TRIM);
}

void CTrimCommand::Back()
{
	if (m_pDrawProc)
	{
		m_pDrawProc->Back();
		if (!m_pDrawProc || IsProcOver(m_pDrawProc))
		{
			Abort();
		}
	}
}

void CTrimCommand::Start()
{
	if( !m_pEditor )return;	
	
	m_nStep = 0;
	m_bTrimOut = TRUE;
	m_nBoundType = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;

	CEditCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	if (m_nBoundType==0)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		UpdateParams(FALSE);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = TRUE;
		CCommand::Start();
		m_pEditor->CloseSelector();
		return;
	}	
	if( m_nBoundType==1 )
		m_pEditor->OpenSelector(SELMODE_MULTI);
	if (m_nBoundType==2)
	{
		m_pEditor->CloseSelector();
	}	
}

void CTrimCommand::Abort()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CTrimCommand::Finish()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CTrimCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nBoundType==0)
	{
		if (m_pDrawProc)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
		}
	}
	else if( m_nBoundType==1 )
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag) )return;
			
			int num ;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num!=1 )return;
			
			FTR_HANDLE  handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
			if( !pObj || pObj->GetDataPointSum()<2 || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				return;

			if(pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
			{
				Abort();
				return;
			}
			
			m_pEditor->CloseSelector();			
			m_pGeoCurve = pObj->Clone();
			m_handleBound = handle;
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
//			return;//解决选择裁剪必须多点一下的问题
		}
	}
	else if( m_nBoundType==2 )
	{
		if( m_nStep==0 )
		{
			m_pGeoCurve = new CGeoCurve;
			if(m_pGeoCurve )
			{
				PT_3D pts[4];
				PT_3DEX expts[4];
				PDOC(m_pEditor)->GetDlgDataSource()->GetBound(pts,NULL,NULL);
				
				for( int i=0; i<4; i++)
				{
					expts[i].z = 0;
					expts[i].pencode = penLine;
					COPY_2DPT(expts[i],pts[i]);
					
				}
				((CGeoCurve*)m_pGeoCurve)->EnableClose(TRUE);
				m_pGeoCurve->CreateShape(expts,4);
				GotoState(PROCSTATE_PROCESSING);
				m_nStep = 2;
			}
		}
	}
		//确认并执行
	if( m_nStep==2 )
	{
		if( IsProcFinished(this) )return;
		
		const CShapeLine *pBase  = m_pGeoCurve->GetShape();
		CArray<PT_3DEX,PT_3DEX> pts;
		GetPolyPts(m_pGeoCurve, pts);
		
		//初始化裁剪器
		int polysnum = pts.GetSize();
		PT_3D *polys = new PT_3D[polysnum];
		if( !polys ) return;

		for( int i=0; i<polysnum; i++)
		{
			COPY_3DPT(polys[i],pts[i]);
		}
		
		Envelope evlp = pBase->GetEnvelope();
		double wid = evlp.Width()/10;
		if( wid<0.1 )wid = 1;
		
		CGrTrim trim;
		trim.InitTrimPolygon(polys,polysnum,wid);
		PromptString(StrFromResID(IDS_CMDTIP_TRIM));
	//	OutputTipString(StrFromResID(IDS_CMDTIP_TRIM));
		//计算进度条
		int nsum = 0;
		for ( i=0; i<PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerCount(); i++)
		{
			CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
			if(!pLayer||pLayer->IsLocked()||!pLayer->IsVisible()) continue;
			int nobj = pLayer->GetObjectCount();
			nsum += nobj;
		}
		
		if( nsum>10 )GProgressStart(nsum);
		
		CFeature *pConvertTempl = NULL;
		
		if( m_bTrimClosedCurve )
		{
//			pConvertTempl = PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMES));
		}
		
		CDlgDoc *pDoc = PDOC(m_pEditor);
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(HandleToFtr(m_handleBound));
		CFeature *pFtr = NULL;
		CPtrArray arr;
		CUndoFtrs undo(m_pEditor,Name());
		for ( i=0; i<pDS->GetFtrLayerCount(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||pLayer->IsLocked()||!pLayer->IsVisible()) continue;
		
			int nobj = pLayer->GetObjectCount();
		
			for (int j=0;j<nobj;j++)
			{
				//增长进度条
				if( nsum>10 )GProgressStep();

				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;			
				if( m_nBoundType==1 && 
					pFtrLayer==pLayer && m_handleBound==FtrToHandle(pFtr) )
					continue;
				
				arr.RemoveAll();		
				//处理点
				if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)) )
				{				
					PT_3D pt3d;
					PT_3DEX expt;
					
					expt = pFtr->GetGeometry()->GetDataPoint(0);
					COPY_3DPT(pt3d,expt);
					int ret = GraphAPI::GIsPtInRegion(pt3d,polys,polysnum);
					if( m_bTrimOut!=(ret==2) )
					{
					//	m_idsOld.Add(id);
						m_pEditor->DeleteObject(FtrToHandle(pFtr));

						undo.AddOldFeature(FtrToHandle(pFtr));				
					}
				}
				//处理线
				else if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					//按照面的方式裁剪闭合线
					if( m_bTrimClosedCurve && ((CGeoCurve*)(pFtr->GetGeometry()))->IsClosed() )
					{
						CFeature *pFtr1 = pLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOSURFACE);
						if( pFtr1==NULL )continue;
						// 复制固定属性
						CValueTable tab;
						tab.BeginAddValueItem();
						pFtr->WriteTo(tab);
						tab.EndAddValueItem();
						tab.DelField(FIELDNAME_SHAPE);
						tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
						tab.DelField(FIELDNAME_FTRID);
						tab.DelField(FIELDNAME_GEOCLASS);
						pFtr1->ReadFrom(tab);

						CArray<PT_3DEX,PT_3DEX> arrPts;
						pFtr->GetGeometry()->GetShape(arrPts);
						pFtr1->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());						
						
						if( TrimSurface(pFtr1->GetGeometry(),&arr,evlp,polys,polysnum,&trim,m_bTrimOut) )
						{
							CValueTable tab;
							tab.BeginAddValueItem();
							GETXDS(m_pEditor)->GetXAttributes(pFtr,tab);
							tab.EndAddValueItem();						
							m_pEditor->DeleteObject(FtrToHandle(pFtr));
							undo.AddOldFeature(FtrToHandle(pFtr));
							int num = arr.GetSize();
							for( int k=0; k<num; k++)
							{
								CGeometry *pObj2 = (CGeometry*)arr[k];
								if( !pObj2 )continue;
								if( pObj2->GetDataPointSum()<2 )
								{
									delete pObj2;
									continue;
								}

								CGeometry *pCurve = pFtr->GetGeometry()->Clone();
								CArray<PT_3DEX, PT_3DEX> arrPts1;
								pObj2->GetShape(arrPts1);
								delete pObj2;
								if (!pCurve->CreateShape(arrPts1.GetData(), arrPts1.GetSize()))
								{
									delete pCurve;
									continue;
								}

								((CGeoCurveBase*)pCurve)->EnableClose(TRUE);
								
								CFeature *pFtr3 = pFtr->Clone();
								pFtr3->SetID(OUID());
								pFtr3->SetToDeleted(FALSE);
								pFtr3->SetGeometry(pCurve);
								m_pEditor->AddObject(pFtr3,pLayer->GetID());
								undo.AddNewFeature(FtrToHandle(pFtr3));
								GETXDS(m_pEditor)->SetXAttributes(pFtr3,tab);
							}
						}
						delete pFtr1;
					}
					else if( TrimCurve(pFtr->GetGeometry(),&arr,evlp,polys,polysnum,&trim,m_bTrimOut) )
					{	
						int num = arr.GetSize();
						CFeature *pFtr0;
						for( int k=0; k<num; k++)
						{
							CGeometry* pObj = (CGeometry*)arr[k];
							if( !pObj )continue;
							if( pObj->GetDataPointSum()<2 )
							{
								delete pObj;
								continue;
							}
							pFtr0 = pFtr->Clone();
						
							pFtr0->SetID(OUID());
							pFtr0->SetGeometry(pObj);
							if(m_pEditor->AddObject(pFtr0,pLayer->GetID()))
							{
								GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtr0);

								undo.AddNewFeature(FtrToHandle(pFtr0));	
							}							
						}
						if(m_pEditor->DeleteObject(FtrToHandle(pFtr)))
						{
							undo.AddOldFeature(FtrToHandle(pFtr));
						}
					}
				}
				else if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					if (m_bTrimSurface)
					{
						CFeature *pFtr1 = pLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
						if( pFtr1==NULL )continue;
						// 复制固定属性
						CValueTable tab;
						tab.BeginAddValueItem();
						pFtr->WriteTo(tab);
						tab.EndAddValueItem();
						tab.DelField(FIELDNAME_SHAPE);
						tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
						tab.DelField(FIELDNAME_FTRID);
						tab.DelField(FIELDNAME_GEOCLASS);
						pFtr1->ReadFrom(tab);
						
						CArray<PT_3DEX,PT_3DEX> arrPts;
						pFtr->GetGeometry()->GetShape(arrPts);
						pFtr1->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());						
						
						if( TrimCurve(pFtr1->GetGeometry(),&arr,evlp,polys,polysnum,&trim,m_bTrimOut) )
						{
							CValueTable tab;
							tab.BeginAddValueItem();
							GETXDS(m_pEditor)->GetXAttributes(pFtr,tab);
							tab.EndAddValueItem();						
							m_pEditor->DeleteObject(FtrToHandle(pFtr));
							undo.AddOldFeature(FtrToHandle(pFtr));
							int num = arr.GetSize();
							for( int k=0; k<num; k++)
							{
								CGeometry *pObj2 = (CGeometry*)arr[k];
								if( !pObj2 )continue;
								if( pObj2->GetDataPointSum()<2 )
								{
									delete pObj2;
									continue;
								}
								
								CFeature *pFtr3 = pFtr->Clone();
								pFtr3->SetID(OUID());
								pFtr3->SetToDeleted(FALSE);
								pFtr3->SetGeometry(pObj2);							
								m_pEditor->AddObject(pFtr3,pLayer->GetID());
								undo.AddNewFeature(FtrToHandle(pFtr3));
								GETXDS(m_pEditor)->SetXAttributes(pFtr3,tab);
							}
						}
						delete pFtr1;
					}
					else if( TrimSurface(pFtr->GetGeometry(),&arr,evlp,polys,polysnum,&trim,m_bTrimOut) )
					{				
											
						int num = arr.GetSize();
						CFeature *pFtr0;
						for( int k=0; k<num; k++)
						{
							CGeometry* pObj = (CGeometry*)arr[k];
							if( !pObj )continue;
							if( pObj->GetDataPointSum()<2 )
							{
								delete pObj;
								continue;
							}

							if (!((CGeoCurveBase*)pFtr->GetGeometry())->IsClosed())
							{
								((CGeoCurveBase*)pObj)->EnableClose(TRUE);
							}							

							pFtr0 = pFtr->Clone();
							pFtr0->SetGeometry(pObj);
							pFtr0->SetToDeleted(FALSE);
							pFtr0->SetID(OUID());
							if(m_pEditor->AddObject(pFtr0,pLayer->GetID()))	
							{
								GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtr0);
								undo.AddNewFeature(FtrToHandle(pFtr0));	
							}
						}
						if(m_pEditor->DeleteObject(FtrToHandle(pFtr)))
							undo.AddOldFeature(FtrToHandle(pFtr));
					}
				}
			}		
		}
		undo.Commit();
		
		if( m_nBoundType==1 )
		{
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
		}
		if( polys )delete[] polys;
		
		if( nsum>10 )GProgressEnd();
		PromptString(StrFromResID(IDS_CMDTIP_TRIMEND));		
		Finish();
	}

	//画线
	if( m_nBoundType==0 )
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}

	return;		    
}

CProcedure *CTrimCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nBoundType==0&&(nMsgType==msgEnd||nMsgType==msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}

int CTrimCommand::GetState()
{
	return m_nState;
}

void CTrimCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}


void CTrimCommand::OnSonEnd(CProcedure *son)
{
	if( m_nBoundType==0 )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;

		CEditCommand::OnSonEnd(son);
	}
}

void CTrimCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_bTrimOut);
	tab.AddValue(PF_TRIMPART,&CVariantEx(var));
	var = (long)(m_nBoundType);
	tab.AddValue(PF_TRIMBOUND,&CVariantEx(var));
	if(m_pDrawProc)
	{
		var = double(m_pDrawProc->m_compress.GetLimit());
		tab.AddValue(PF_TRIMTOLER,&CVariantEx(var));
	}	
	var = (bool)(m_bTrimClosedCurve);
	tab.AddValue(PF_TRIMCLOSELINE,&CVariantEx(var));

	var = (bool)(m_bTrimSurface);
	tab.AddValue(PF_TRIMSURFACE,&CVariantEx(var));
}


void CTrimCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("TrimCommand",StrFromLocalResID(IDS_CMDNAME_TRIM));
	param->BeginOptionParam(PF_TRIMPART,StrFromResID(IDS_CMDPLANE_TRIMPART));
	param->AddOption(StrFromResID(IDS_CMDPLANE_OUTSIDE),1,' ',m_bTrimOut);
	param->AddOption(StrFromResID(IDS_CMDPLANE_INSIDE),0,' ',!m_bTrimOut);	
	param->EndOptionParam();
	
	param->BeginOptionParam(PF_TRIMBOUND,StrFromResID(IDS_CMDPLANE_BOUND));
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),0,' ',m_nBoundType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),1,' ',m_nBoundType==1);
	param->AddOption(StrFromResID(IDS_CMDPLANE_WSBOUND),2,' ',m_nBoundType==2);
	param->EndOptionParam();
	if(m_nBoundType==0&&m_pDrawProc) 
	{
		BOOL bHide = (m_pDrawProc->m_nCurPenCode!=penStream);
 		if(!bHide||bForLoad)param->AddParam(PF_TRIMTOLER,double(m_pDrawProc->m_compress.GetLimit()),StrFromResID(IDS_CMDPLANE_TOLER));
	}
	param->BeginOptionParam(PF_TRIMCLOSELINE,StrFromResID(IDS_CMDPLANE_TRIM_CLOSEDCURVE));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bTrimClosedCurve);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bTrimClosedCurve);
	param->EndOptionParam();

	param->BeginOptionParam(PF_TRIMSURFACE,StrFromResID(IDS_CMDPLANE_TRIM_SURFACE));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bTrimSurface);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bTrimSurface);
	param->EndOptionParam();
}


void CTrimCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_TRIMPART,var) )
	{
		m_bTrimOut = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TRIMBOUND,var) )
	{					
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}
	if( tab.GetValue(0,PF_TRIMTOLER,var) )
	{
		double toler = (double)(_variant_t)*var;
		if(m_pDrawProc)m_pDrawProc->m_compress.SetLimit(toler);
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TRIMCLOSELINE,var) )
	{
		m_bTrimClosedCurve = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TRIMSURFACE,var) )
	{
		m_bTrimSurface = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}


static BOOL IsObjInRegion(CGeometry* pObj, PT_3D *polys, int num)
{
	CArray<PT_3DEX, PT_3DEX> pts;
	pObj->GetShape(pts);
	int nPt = pts.GetSize();
	nPt = GraphAPI::GKickoffSamePoints(pts.GetData(), nPt);

	PT_3D expt;

	if (nPt == 1)
	{
		expt.x = pts[0].x;
		expt.y = pts[0].y;
		expt.z = 0.0;
		int r = GraphAPI::GIsPtInRegion(expt, polys, num);
		if (r == 2)return TRUE;
		else if (r == -1)return FALSE;
		else return -1;
	}
	else if (nPt == 2)//只判断中点
	{
		expt.x = (pts[0].x + pts[1].x)*0.5;
		expt.y = (pts[0].y + pts[1].y)*0.5;
		expt.z = 0.0;
		int r = GraphAPI::GIsPtInRegion(expt, polys, num);
		if (r == 2)return TRUE;
		else if (r == -1)return FALSE;
		else return -1;
	}
	else
	{
		expt.x = pts[nPt / 3].x;
		expt.y = pts[nPt / 3].y;
		expt.z = 0.0;
		int r1 = GraphAPI::GIsPtInRegion(expt, polys, num);

		expt.x = pts[nPt / 3 * 2].x;
		expt.y = pts[nPt / 3 * 2].y;
		expt.z = 0.0;
		int r2 = GraphAPI::GIsPtInRegion(expt, polys, num);

		if (r1 == 2 && r2 >= 0)
		{
			return TRUE;
		}
		else if (r2 == 2 && r1 >= 0)
		{
			return TRUE;
		}
		else if (r1 == -1 && r2 <= 1)
		{
			return FALSE;
		}
		else if (r2 == -1 && r1 <= 1)
		{
			return FALSE;
		}
		else
		{
			return -1;
		}
	}

	return -1;
}

void CTrimCommand::GetPolyPts(CGeometry *pObj, CArray<PT_3DEX,PT_3DEX>& pts)
{
	if(!pObj) return;
	int nCls = pObj->GetClassType();
	CArray<PT_3DEX,PT_3DEX> pts1, pts2;
	if(nCls==CLS_GEOSURFACE || nCls==CLS_GEOCURVE)
	{
		const CShapeLine *pBase = pObj->GetShape();
		if(!pBase) return;
		pBase->GetPts(pts1);
	}
	else if(nCls==CLS_GEOPARALLEL || nCls==CLS_GEODCURVE)
	{
		CGeometry* pObj1=NULL;
		CGeometry* pObj2=NULL;
		if(nCls==CLS_GEODCURVE)
			((CGeoDCurve*)pObj)->Separate(pObj1, pObj2);
		else if(nCls==CLS_GEOPARALLEL)
			((CGeoParallel*)pObj)->Separate(pObj1, pObj2);
		if(!pObj1 || !pObj2) return;

		pObj1->GetShape(pts1);
		pObj2->GetShape(pts2);
		pts1[0].pencode=penLine;
		pts2[0].pencode=penLine;
		PT_3DEX p1 = pts1[0];
		PT_3DEX p2 = pts2[0];
		PT_3DEX p3 = pts1[pts1.GetSize()-1];
		PT_3DEX p4 = pts2[pts2.GetSize()-1];
		double x,y,t0,t1;
		bool bIntersect = GraphAPI::GGetLineIntersectLineSeg(p1.x,p1.y,
			p2.x,p2.y,p3.x,p3.y,p4.x,p4.y,&x,&y,&t0,&t1);
		for(int j=0; j<pts2.GetSize(); j++)
		{
			if(bIntersect)
				pts1.Add(pts2[j]);
			else
				pts1.Add(pts2[pts2.GetSize()-1-j]);
		}
		pts1.Add(pts1[0]);
		delete pObj1;
		delete pObj2;
	}


	int ret = GraphAPI::GIsClockwise(pts1.GetData(),pts1.GetSize());
	if( ret==0)
	{
		for(int i=0; i<pts1.GetSize(); i++)
		{
			pts.Add(pts1[pts1.GetSize()-1-i]);
		}
	}
	else if(ret==1)
	{
		pts.Copy(pts1);
	}
}

BOOL CTrimCommand::TrimCurve(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim, BOOL bTrimOutside)
{
	if (!pObj) return FALSE;

	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return FALSE;

	if (pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
	{		
		CGeometry *pCur1 = NULL, *pCur2 = NULL;
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
		{
			((CGeoParallel*)pObj)->Separate(pCur1,pCur2);
		}
		else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
		{
			((CGeoDCurve*)pObj)->Separate(pCur1,pCur2);
		}
		
		BOOL bRet1=FALSE, bRet2=FALSE;
		if (pCur1)
		{
			bRet1 = TrimCurve(pCur1,pArray,evlp,polys,polysnum,ptrim,bTrimOutside);
			delete pCur1;
		}
		
		if (pCur2)
		{
			bRet2 = TrimCurve(pCur2,pArray,evlp,polys,polysnum,ptrim,bTrimOutside);
			delete pCur2;
		}
		
		return (bRet1||bRet2);
	}

	CGeoCurveBase *pCurve = (CGeoCurveBase*)pObj;
	
	CArray<PT_3DEX,PT_3DEX> arrKeyPts;
	pCurve->GetShape(arrKeyPts);
	if (arrKeyPts.GetSize()<=0)
	{
		return FALSE;
	}
	if( arrKeyPts.GetSize()==1 )
	{
		PT_3DEX expt;
		expt = arrKeyPts.GetAt(0);
		
		PT_3D pt3d;
		COPY_3DPT(pt3d,expt);
		return ( (GraphAPI::GIsPtInRegion(pt3d,polys,polysnum)==2)!=bTrimOutside);
	}
	
	const CShapeLine *pSL = pObj->GetShape();
	/*const GrUnit *pUnit = NULL;*/
	PT_3DEX  *pts = NULL;
	
	if( (evlp.m_xh-evlp.m_xl)<1e-10 || (evlp.m_yh-evlp.m_yl)<1e-10 )
		return FALSE;
	
	//完全不相交
	if( !evlp.bIntersect(&pSL->GetEnvelope()) )
	{
		return bTrimOutside;
	}
	
	PT_3D trimLine[2];
	CArray<PT_3D,PT_3D> arrTrimPts;
	//记录所有裁剪点
	CPtrArray arr;
	if(!pSL->GetShapeLineUnit(arr)) return FALSE;
	for( int i=0;i<arr.GetSize();i++ )
	{
		CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[i];
		if( evlp.bIntersect(&(pUnit->evlp))) 
		{
			pts = pUnit->pts;
			for( int k=0; k<pUnit->nuse-1; k++,pts++)
			{
				if( !evlp.bIntersect(&pts[0],&pts[1]) )continue;
				
				ptrim->TrimLine(&pts[0],&pts[1]);
				int trimnum = ptrim->GetTrimedLinesNum();
				if( trimnum<=0 )continue;
				
				//记录裁剪点
				for( int i=0; i<trimnum; i++)
				{
					int type = ptrim->GetTrimedLine(i,trimLine,trimLine+1);
					
					if( (type&TL_MID1)!=0 || (type&TL_ENDON1)!=0 )arrTrimPts.Add(trimLine[0]);
					if( (type&TL_MID2)!=0 || (type&TL_ENDON2)!=0 )arrTrimPts.Add(trimLine[1]);
				}
			}
		}		
	}
	

	//圆弧的裁剪不稳定，只作全在内或者全在外的判断
	BOOL bHasArc = FALSE, bLinearized = FALSE;
	for( i=0; i<arrKeyPts.GetSize(); i++)
	{		 
		if( arrKeyPts[i].pencode==penArc )
		{
			bHasArc = TRUE;
			break;
		}
	}

	if( bHasArc )
	{
		if( arrTrimPts.GetSize()==0 && evlp.bEnvelopeIn(&pSL->GetEnvelope()) )
			return !bTrimOutside;
		else
		{
			pObj = pCurve->Linearize();
			if( !pObj )return FALSE;
			bLinearized = TRUE;
			pCurve = (CGeoCurveBase*)pObj;
		}
	}
	
	//根据裁剪点生成裁剪对象
	if( arrTrimPts.GetSize()>0 )
	{
		//准备
		CGeometry *trimObj[3];
		BOOL bFirstObj = TRUE;
		
		int trimnum = arrTrimPts.GetSize();
		for( int i=0; i<trimnum; i+=2 )
		{
			if( i==trimnum-1 )
				pCurve->GetBreak(arrTrimPts[i],arrTrimPts[i],trimObj[0],
				trimObj[1],trimObj[2],FALSE);
			else
				pCurve->GetBreak(arrTrimPts[i],arrTrimPts[i+1],trimObj[0],
				trimObj[1],trimObj[2],FALSE);
			
			//依次判断各个子对象是否保留
			
			if( trimObj[0] )
			{
				BOOL bObjIn = IsObjInRegion(trimObj[0],polys,polysnum);
				if (bObjIn == bTrimOutside || bObjIn == -1)
					pArray->Add(trimObj[0]);
				else
					delete trimObj[0];
			}
			
			if( trimObj[1] )
			{
				BOOL bObjIn = IsObjInRegion(trimObj[1],polys,polysnum);
				if (bObjIn == bTrimOutside || bObjIn == -1)
					pArray->Add(trimObj[1]);
				else
					delete trimObj[1];
			}
			
			if( !bFirstObj && pCurve )delete pCurve;
			bFirstObj = 0;
			pCurve = NULL;
			
			if( trimObj[2] )
			{
				//以尾对象为新目标，继续裁剪
				pCurve = (CGeoCurveBase*)trimObj[2];
			}
			else break;
		}
		
		if( !bFirstObj && pCurve )
		{
			BOOL bObjIn = IsObjInRegion(pCurve,polys,polysnum);
			if (bObjIn == bTrimOutside || bObjIn == -1)
				pArray->Add(pCurve);
			else
				delete pCurve;
		}
		if( bLinearized )
			delete pObj;
		return TRUE;
	}
	else//全部在边界外或者全部在边界内
	{
		BOOL bHeadIn = IsObjInRegion(pObj,polys,polysnum);
		
		if( bLinearized )
			delete pObj;

		return (bHeadIn != bTrimOutside && bHeadIn != -1);
	}
}


// z = Ax + By + C
BOOL SolvePlaneEquation2(PT_3D *pts, int num, double* a,double* b,double* c)
{
	double M[9];
	double RM[9];
	double A[3];
	ZeroMemory(M,sizeof(double)*9);
	ZeroMemory(RM,sizeof(double)*9);
	ZeroMemory(A,sizeof(double)*3);
	
	for(int i = 0;i<num;i++)
	{
		PT_3D point = pts[i];	
		M[0] += point.x * point.x;
		M[1] += point.x * point.y;
		M[2] += point.x;
		
		M[4] += point.y * point.y;
		M[5] += point.y;
		
		
		A[0] += point.x*point.z;
		A[1] += point.y*point.z;
		A[2] += point.z;
	}	

	M[3] = M[1];
	M[6] = M[2];
	M[7] = M[5];
	M[8] = num;	
	
	//如果矩阵不可逆，
	if (!matrix_reverse(M,3,RM))
	{
		return FALSE;
	}

	*a = RM[0]*A[0] + RM[1]*A[1] + RM[2]*A[2];
	*b = RM[3]*A[0] + RM[4]*A[1] + RM[5]*A[2];
	*c = RM[6]*A[0] + RM[7]*A[1] + RM[8]*A[2];
	
	return TRUE;
}


BOOL CTrimCommand::TrimSurface(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim, BOOL bTrimOutside)
{
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )return FALSE;
	if (!((CGeoCurveBase*)pObj)->IsClosed())
	{
		return TrimCurve(pObj,pArray,evlp,polys,polysnum,ptrim,bTrimOutside);
	}

	CGeoSurface *pCurve = (CGeoSurface*)pObj;
	
	if( pCurve->GetDataPointSum()==1 )
	{
		PT_3DEX expt;
		expt = pCurve->GetDataPoint(0);
		
		PT_3D pt3d;
		COPY_3DPT(pt3d,expt);
		return ( (GraphAPI::GIsPtInRegion(pt3d,polys,polysnum)==2)!=bTrimOutside);
	}
	
	if( (evlp.m_xh-evlp.m_xl)<1e-10 || (evlp.m_yh-evlp.m_yl)<1e-10 )
		return FALSE;
	
	const CShapeLine *pSL = pObj->GetShape();
	
	//完全不相交
	if( !evlp.bIntersect(&pSL->GetEnvelope()) )
	{
		return bTrimOutside;
	}

	//获取基线点
	CArray<PT_3DEX,PT_3DEX> pts;
	if(!pSL->GetPts(pts)) return FALSE;
	int npt = pts.GetSize(),i,j;
	if(npt<=0) return FALSE;

	//if(pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
	{		
		CGeoSurface obj2;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.SetSize(polysnum);
		int i = 0;
		for(i=0; i<polysnum; i++)
		{
			arrPts[i].x = polys[i].x;
			arrPts[i].y = polys[i].y;
			arrPts[i].z = polys[i].z;
			arrPts[i].pencode = penLine;
		}
		obj2.CreateShape(arrPts.GetData(),arrPts.GetSize());
		obj2.EnableClose(TRUE);
		
		gsbh::GeoSurfaceBooleanHandle gsbhandle;
		if (!gsbhandle.setSurface((CGeoSurface*)pObj, &obj2))
			return FALSE;
		
		//
		if(!gsbhandle.init())
			return FALSE;
		
		//
		if(bTrimOutside)
			gsbhandle.intersection();
		else
			gsbhandle.difference();
		//
		std::vector<CGeoSurface*> res;
		gsbhandle.getResult(res);
		//
		bool crossed = gsbhandle.isSurfaceCrossed();
		if(!crossed && res.size()>0)
			return FALSE;
		//
		for(i=0; i<res.size(); ++i)
		{
			CGeoSurface* pNewObj = res[i];
			if(pNewObj == NULL)
				continue;
			//
			CArray<PT_3DEX, PT_3DEX> temp_shape;
			pNewObj->GetShape(temp_shape);
			pNewObj->CopyFrom(pObj);
			pNewObj->CreateShape(temp_shape.GetData(),temp_shape.GetSize());
		    pNewObj->EnableClose(TRUE);
			
			pArray->Add(pNewObj);	
		}
		return TRUE;
	}

	//基线点确保为顺时针
	CArray<PT_3DEX,PT_3DEX> MutiPartPts;

	//去除复杂面的内部点
	for(i=1; i<npt&&pts[i].pencode!=penMove; i++);
	for(; i<npt; i++)
	{
		MutiPartPts.Add(pts[i]);
	}
	for(i=1; i<npt; i++)
	{
		if(pts[i].pencode==penMove)
		{
			pts.RemoveAt(i, npt-i);
			break;
		}
	}
	npt = pts.GetSize();

	PT_3D *pt3ds = new PT_3D[npt];
	if(!pt3ds) return FALSE;	
	//裁里就用逆时针（保留相减区域），裁外就用顺时针（保留相交区域）
	if( bTrimOutside!=(GraphAPI::GIsClockwise(pts.GetData(),npt)?TRUE:FALSE) )
	{
		for( i=0; i<npt; i++)
		{
			COPY_3DPT(pt3ds[i] ,pts[npt-1-i]);
		}
	}
	else
	{
		for( i=0; i<npt; i++)
		{
			COPY_3DPT(pt3ds[i] ,pts[i]);
		}
	}
		
	//起点是否在多边形内或外
	BOOL bHeadOut = !(GraphAPI::GIsPtInRegion(pt3ds[0],polys,polysnum)==2);
	BOOL bObjOut = !IsObjInRegion(pCurve,polys,polysnum);

	//new algorithm
	{
		PolygonIntersect::CPolygonWork pw;		

		for( i=0; i<npt; i++)
		{
			pw.add_point(0,pt3ds[i].x,pt3ds[i].y,pt3ds[i].z);
		}

		for( i=0; i<polysnum; i++)
		{
			pw.add_point(1,polys[i].x,polys[i].y,polys[i].z);
		}

		pw.set_overlap(GraphAPI::GetDisTolerance());
		pw.initialize();

		vector< vector<PolygonIntersect::tpoint> > *pResult = NULL;

		if( bTrimOutside )
		{
			//取相交区域
			pw.find_intersection();
			pResult = &pw.intersection_list;
		}
		else
		{
			//取补集
			pw.find_XOR(0,1,1);
			pResult = &pw.xor_list;
		}

		if( pw.GetError()==PolygonIntersect::CPolygonWork::NONE )
		{
			if( pResult->size()<=0 )
			{
				if( pt3ds )delete[] pt3ds;
				return (bObjOut==bTrimOutside);
			}

			//创建模板对象
			CGeoSurface *pTempl = (CGeoSurface*)pObj->Clone();
			if( !pTempl )
			{		
				if( pt3ds )delete[] pt3ds;
				return FALSE;
			}

			double A=0,B=0,C=0;
			BOOL bInterZ = SolvePlaneEquation2(pt3ds,npt,&A,&B,&C);

			CArray<PT_3DEX,PT_3DEX> arrNewPts;
			PT_3DEX expt;
			expt.pencode = penLine;
			for( i=0; i<pResult->size(); i++)
			{
				vector<PolygonIntersect::tpoint> list1 = pResult->at(i);
				if( list1.size()>=3 )
				{
					arrNewPts.RemoveAll();
					for( j=0; j<list1.size(); j++)
					{
						PolygonIntersect::tpoint tpt = list1[j];
						COPY_3DPT(expt,tpt);
						if( bInterZ && GraphAPI::GIsPtInRegion((PT_3D)expt,pt3ds,npt)==2 )
						{
							expt.z = A*expt.x + B*expt.y + C;
						}
						arrNewPts.Add(expt);
					}
					arrNewPts.Append(MutiPartPts);
					CGeoSurface *pNew = (CGeoSurface*)pTempl->Clone();
					pNew->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize());

					if( arrNewPts.GetSize()>=3 )
					{
						pNew->EnableClose(TRUE);
					}

					pArray->Add(pNew);
				}
			}

			if( pt3ds )delete[] pt3ds;
			delete pTempl;
			
			return TRUE;
		}
	}

	// old algorithm 
	if( 1 )
	{
		Envelope evlpobj = pSL->GetEnvelope();	
		
		BOOL bIntersectNode = TRUE;
		//将母线节点和相交点组成一个顺序的点列
		CArray<SurfaceTrimPt,SurfaceTrimPt> arrObjPt, arrPolyPt;
		for( i=0; i<npt; i++)
		{
			SurfaceTrimPt tpt;
			tpt.pt = pt3ds[i];
			tpt.no1 = 0; tpt.no2 = 0;
			//type为0，表示是基线点，不为0，表示是交点；
			//其中，为-1，no1是作交点对应之前的对象的基线点号，no2是作交点对应之前的范围线的基线点号
			//为1，no1是作交点对应之后的对象总点列的索引号，no2是作交点对应之后的范围线总点列的索引号
			tpt.type = 0;	
			tpt.t = 0;	//t是记录的此点在线段上的位置参数
			tpt.trace = 0;
			int i0 = arrObjPt.Add(tpt);
			if( i==(npt-1) )continue;
			
			if( !evlp.bIntersect(pt3ds+i,pt3ds+i+1) )continue;

			for( j=0; j<polysnum; j++)
			{
				int j1 = (j+1)%polysnum;		
				
				SurfaceTrimPt tpt0;
				tpt0.no1 = 0; tpt0.no2 = 0;
				tpt0.type = 0;
				tpt0.t = 0; 
				tpt0.trace = 0;

				if( GraphAPI::GGetLineIntersectLineSeg(pt3ds[i].x,pt3ds[i].y,
					pt3ds[i+1].x,pt3ds[i+1].y,polys[j].x,polys[j].y,
					polys[j1].x,polys[j1].y,&tpt.pt.x,&tpt.pt.y,&tpt.t,&tpt0.t) )
				{
					if (tpt.t < -1e-10 || (i<npt-2?tpt.t>=1-1e-10:tpt.t>1+1e-10) || tpt0.t < -1e-10 || (j<polysnum-2?tpt0.t>=1-1e-10:tpt0.t>1+1e-10))
					{
						continue;
					}

					if (tpt.t > 1e-10 && tpt.t < 1-1e-10)
					{
						bIntersectNode = FALSE;
					}
					
					GraphAPI::GGetPtZOfLine(pt3ds[i].x,pt3ds[i].y,pt3ds[i].z,pt3ds[i+1].x,pt3ds[i+1].y,pt3ds[i+1].z,&tpt.pt.x,&tpt.pt.y,&tpt.pt.z);
					tpt.type = -1;
					tpt.no1 = i; tpt.no2 = j;
					
					//保持一致的顺序
					for( int k=i0+1; k<arrObjPt.GetSize(); k++)
					{
						if( arrObjPt.GetAt(k).t>tpt.t )break;
					}
					if( k>=arrObjPt.GetSize() )arrObjPt.Add(tpt);
					else arrObjPt.InsertAt(k,tpt);

					//////////////////////////////////////////////////////////////////////////
					tpt0.t += j;
					tpt0.pt = tpt.pt;
					tpt0.type = -1;
					tpt0.no1 = i; tpt0.no2 = j;
					
					//保持一致的顺序
					for( k=0; k<arrPolyPt.GetSize(); k++)
					{
						if( arrPolyPt.GetAt(k).t>tpt0.t )break;
					}
					if( k>=arrPolyPt.GetSize() )arrPolyPt.Add(tpt0);
					else arrPolyPt.InsertAt(k,tpt0);
				}
			}
		}

		int objPtSize = arrObjPt.GetSize();
		for (i=objPtSize-1; i>=0; i--)
		{
			for (j=i-1; j>=0 && (arrObjPt[j].type == -1 && arrObjPt[i].type == -1) && GraphAPI::GGet2DDisOf2P(arrObjPt[j].pt,arrObjPt[i].pt) < 1e-10; j--)
			{
				arrObjPt.RemoveAt(j);
				i--;
			}
		}

		if( arrObjPt.GetSize()<=npt )
		{	
			if( pt3ds )delete[] pt3ds;
			return bObjOut==bTrimOutside;
		}

		// 如果相交点全部在关键点处就检测地物是否全部在范围线内或者全部在范围线外
		if (bIntersectNode)
		{
			int lastRet = -3;
			int objPtSize = arrObjPt.GetSize();
			for (i=0; i<objPtSize-1; i++)
			{
				int r = GraphAPI::GIsPtInRegion(arrObjPt[i].pt,polys,polysnum);	
				PT_3D expt;
				expt.x = (arrObjPt[i].pt.x + arrObjPt[i+1].pt.x) * 0.5;
				expt.y = (arrObjPt[i].pt.y + arrObjPt[i+1].pt.y) * 0.5;
				expt.z = (arrObjPt[i].pt.z + arrObjPt[i+1].pt.z) * 0.5;
				int r1 = GraphAPI::GIsPtInRegion(expt,polys,polysnum);	
				
				if (lastRet != -3 &&  (r == -1 && lastRet == 2) || (r == 2 && lastRet == -1) || 
							(r1 == -1 && lastRet == 2) || (r1 == 2 && lastRet == -1))
				{
					break;
				}

				if (lastRet == -3 && (r == -1 || r == 2))
				{
					lastRet = r;
				}
							
			}

			if (i >= objPtSize-1)
			{
				if( pt3ds )delete[] pt3ds;
				return bObjOut==bTrimOutside;
			}
		}
		
		for( j=0; j<polysnum; j++)
		{
			SurfaceTrimPt tpt;
			tpt.pt = polys[j];
			tpt.no1 = 0; tpt.no2 = 0;
			tpt.type = 0;
			tpt.t = j; 
			tpt.trace = 0;

			for(int k=0; k<arrPolyPt.GetSize(); k++)
			{
				if( arrPolyPt.GetAt(k).t>=tpt.t )break;
			}
			if( k>=arrPolyPt.GetSize() )arrPolyPt.Add(tpt);
			else arrPolyPt.InsertAt(k,tpt);
		}

		for (j=0; j<arrPolyPt.GetSize(); j++)
		{
			arrPolyPt[j].t -= int(arrPolyPt[j].t);
		}

		int polyPtSize = arrPolyPt.GetSize();
		for (i=polyPtSize-1; i>=0; i--)
		{
			for (j=i-1; j>=0 && (arrPolyPt[j].type == -1 && arrPolyPt[i].type == -1) && GraphAPI::GGet2DDisOf2P(arrPolyPt[j].pt,arrPolyPt[i].pt) < 1e-10; j--)
			{
				arrPolyPt.RemoveAt(j);
				i--;
			}
		}

		//SurfaceTrimPt::type为0，表示是基线点，不为0，表示是交点；
		//其中，为-1，no1是作交点对应之前的对象的基线点号，no2是作交点对应之前的范围线的基线点号
		//为1，no1是作交点对应之后的对象总点列的索引号，no2是作交点对应之后的范围线总点列的索引号

		if( arrPolyPt.GetSize()<=polysnum )
		{	
			if( pt3ds )delete[] pt3ds;
			return bObjOut==bTrimOutside;
		}

		//将两个点列中的相交点一一对应起来
		SurfaceTrimPt *tpts1 = arrObjPt.GetData();
		SurfaceTrimPt *tpts2 = arrPolyPt.GetData();
		int ntpt1 = arrObjPt.GetSize(), ntpt2 = arrPolyPt.GetSize();
		for( i=0; i<ntpt1; i++)
		{
			if( tpts1[i].type!=-1 )continue;
			for( j=0; j<ntpt2; j++)
			{
				if( tpts2[j].type!=-1 )continue;
				if( tpts1[i].no1==tpts2[j].no1 && tpts1[i].no2==tpts2[j].no2 )
				{
					if( fabs(tpts1[i].pt.x-tpts2[j].pt.x)<1e-10 && 
						fabs(tpts1[i].pt.y-tpts2[j].pt.y)<1e-10 &&
						fabs(tpts1[i].pt.z-tpts2[j].pt.z)<1e-10 )
					{
						tpts1[i].no1 = i; tpts1[i].no2 = j;
						tpts2[j].no1 = i; tpts2[j].no2 = j;
						tpts1[i].type = 1;
						tpts2[j].type = 1;
						break;
					}
				}
			}
			//匹配失败
			if( j>=ntpt2 )
			{		
				if( pt3ds )delete[] pt3ds;
				return FALSE;
			}
		}
		
		//创建模板对象
		CGeoSurface *pTempl = (CGeoSurface*)pObj->Clone();
		if( !pTempl )
		{		
			if( pt3ds )delete[] pt3ds;
			return FALSE;
		}
		
		//跳过起点开始的一段，达到相交点
		for( i=0; i<ntpt1; i++)
		{
			if( tpts1[i].type==1 )break;
		}
		
		//如果从起点开始的一段不会被裁剪，那么下一段就会被裁剪，
		//为了保证从i0开始的地方总会被保留，我们就再走一段
		if( bHeadOut!=bTrimOutside )
		{
			for( i++; i<ntpt1; i++)
			{
				if( tpts1[i].type==1 )break;
			}
		}

		if( i>=ntpt1 )
		{
			delete pTempl;
			if( pt3ds )delete[] pt3ds;
			return FALSE;
		}
		
		int i0 = i%ntpt1;
		PT_3DEX expt;
		expt.pencode = penLine;
		
		do
		{
			CGeoSurface *pNew = (CGeoSurface*)pTempl->Clone();
			if( !pNew )break;
			CArray<PT_3DEX,PT_3DEX> arr;
			//添加相交点，这是新对象的起点
			COPY_3DPT(expt,tpts1[i].pt);
			arr.Add(expt);
			tpts1[i].trace = 1;
			
			int start = i, stop, bloop = 0, j0;
	LOOP:
			i = (i+1)%ntpt1;
			
			//跟踪对象上的节点，直到某个交点
			for( ; i!=i0; i = (i+1)%ntpt1 )
			{
				COPY_3DPT(expt,tpts1[i].pt);
				arr.Add(expt);
				tpts1[i].trace = 1;
				if( tpts1[i].type==1 )break;
			}
			if( !bloop )stop = i;
			//跳跃到范围线上的对应的那个交点
			j = tpts1[i].no2;
			
			//跟踪范围线上的节点，直到某个交点，并包括该相交点
			j0 = j = (j+1)%ntpt2;
			for(;;)
			{
				COPY_3DPT(expt,tpts2[j].pt);
				arr.Add(expt);
				if( tpts2[j].type==1 )
				{
					tpts1[tpts2[j].no1].trace = 1;
					break;
				}
				j = (j+1)%ntpt2;
				if( j==j0 )break;
			}
			
			//如果不是跟踪的起点，说明当前新对象的边界线还没有跟踪完毕，需要继续
			if( tpts2[j].no1!=tpts1[start].no1 || tpts2[j].no2!=tpts1[start].no2 )
			{
				//跳跃到新的起点
				i = tpts2[j].no1;
				//循环
				bloop = 1;
				goto LOOP;
			}
			
			//恢复到没有跳跃前的位置
			if( bloop )i = stop;
			
			//对象的下一个相交段是被裁剪部分，跳过
			i = (i+1)%ntpt1;
			for( ; i!=i0; i = (i+1)%ntpt1 )
			{
				//跳过跟踪过的点
				if( tpts1[i].trace==1 )continue;
				if( tpts1[i].type==1 )break;
			}
			
			//去除相同的点
			{
				int ptsum = arr.GetSize();			
				
				PT_3DEX *expts = arr.GetData();
				int pos = 0;
				for(int k=1; k<ptsum; k++)
				{
					if( fabs(expts[pos].x-expts[k].x)<1e-6 && 
						fabs(expts[pos].y-expts[k].y)<1e-6 )
						continue;
					
					if( k!=(pos+1) )expts[pos+1] = expts[k];
					pos++;
				}
				if( ptsum!=(pos+1) )
				{
					for( k=ptsum-1; k>=0; k--)
					{
						if( k>=(pos+1) )arr.RemoveAt(k);
						else arr.SetAt(k,expts[k]);
					}
				}
			}
			
			pNew->CreateShape(arr.GetData(),arr.GetSize());
			if( arr.GetSize()>=4 )
			{
				pNew->EnableClose(TRUE);
				pArray->Add(pNew);
			}
			else
			{
				delete pNew;
			}
			
		}while(i!=i0);	

		if( pt3ds )delete[] pt3ds;
		delete pTempl;
		
		return TRUE;
	}

}

//////////////////////////////////////////////////////////////////////
// CLinkContourCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CLinkContourCommand,CEditCommand)

CLinkContourCommand::CLinkContourCommand()
{
	m_nStep = -1;
	m_bMatchHeight = TRUE;
	m_bCrossLayer = FALSE;
	strcat(m_strRegPath,"\\LinkContour");
}

CLinkContourCommand::~CLinkContourCommand()
{
	
}



CString CLinkContourCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_LINKCONTOUR);
}

void CLinkContourCommand::Start()
{
	if( !m_pEditor )return;

	m_bMatchHeight = TRUE;
	

	m_nStep = 0;
	CEditCommand::Start();
	m_pEditor->CloseSelector();
	m_arrPts.RemoveAll();
}

void CLinkContourCommand::Abort()
{
	
	CEditCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}



void CLinkContourCommand::PtClick(PT_3D &pt, int flag)
{
	//分别在m_nStep=0和m_nStep=1时绘制两条线段
	if( m_nStep<=1 )
	{	
		
		m_arrPts.Add(pt);
		
		GotoState(PROCSTATE_PROCESSING);
		
		if( m_arrPts.GetSize()>=2 )
		{
			GrBuffer vbuf;
			vbuf.BeginLineString(RGB(255,0,0),0);
			vbuf.MoveTo(&m_arrPts[0]);
			vbuf.LineTo(&m_arrPts[1]);
			vbuf.End();
			
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&vbuf);
			
			m_ptLine[m_nStep*2] = m_arrPts[0];
			m_ptLine[m_nStep*2+1] = m_arrPts[1];
			
			m_nStep++;
			m_arrPts.RemoveAll();
			
			
		}
		
		
	}
	else if (m_nStep==2)
	{
		//清除显示
	
		//m_pDoc->SetCurDrawingObj(DrawingInfo());
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	    m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
		
		//执行合并
		LinkObjs();
		
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
		
	}

	CEditCommand::PtClick(pt,flag);
}

void CLinkContourCommand::PtMove(PT_3D &pt)
{
	
	if(  GetState()==PROCSTATE_FINISHED || m_arrPts.GetSize()<=0 )return;
	
	if(m_arrPts.GetSize()==1)
	{
		m_arrPts.Add(pt);
		GrBuffer vbuf;
		vbuf.BeginLineString(0,0);
		vbuf.MoveTo(&m_arrPts[0]);
		vbuf.LineTo(&m_arrPts[1]);
		vbuf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		m_arrPts.RemoveAt(1);
		
	}
	CCommand::PtMove(pt);
}


void CLinkContourCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
 	_variant_t var;
	var = (bool)(m_bMatchHeight);
	tab.AddValue(PF_LINKCONTORMATCHHEI,&CVariantEx(var));

	var = (bool)(m_bCrossLayer);
	tab.AddValue(PF_LINKCONTORCROSSLAYER,&CVariantEx(var));
}


void CLinkContourCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("LinkContourCommand",StrFromLocalResID(IDS_CMDNAME_LINKCONTOUR));
	param->AddParam(PF_LINKCONTORMATCHHEI,bool(m_bMatchHeight),StrFromResID(IDS_CMDPLANE_MATCHHEIGHT));
	param->AddParam(PF_LINKCONTORCROSSLAYER,bool(m_bCrossLayer),StrFromResID(IDS_LINK_CROSSLAYER));
}


void CLinkContourCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
 	if( tab.GetValue(0,PF_LINKCONTORMATCHHEI,var) )
	{
		m_bMatchHeight = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_LINKCONTORCROSSLAYER,var) )
	{
		m_bCrossLayer = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}


void CLinkContourCommand::LinkObjs()
{
	//粗略查找第一条线段的相交地物
	Envelope e1, e2,e3,e4;
	e1.CreateFromPts(m_ptLine,2);
	e3 = e1;
	e3.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e3,m_pEditor->GetCoordWnd().m_pSearchCS);
	
	int num1;
	const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
	if( !ppftr1  )
	{	
		return;
	}
	CArray<CPFeature,CPFeature> ftr1;
	ftr1.SetSize(num1);
	memcpy(ftr1.GetData(),ppftr1,num1*sizeof(*ppftr1));
	
	
	//粗略查找第二条线段的相交地物
	e2.CreateFromPts(m_ptLine+2,2);
	e4 = e2;
	e4.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e4,m_pEditor->GetCoordWnd().m_pSearchCS);
	
	int num2;
	const CPFeature * ppftr2 = m_pEditor->GetDataQuery()->GetFoundHandles(num2);
	if( !ppftr2  )
	{	
		return;
	}
	CArray<CPFeature,CPFeature> ftr2;
	ftr2.SetSize(num2);
	memcpy(ftr2.GetData(),ppftr2,num2*sizeof(*ppftr2));
	
	
	
//获得有效的对象与两线段的相交信息，并排序
	CArray<OBJ_ITEM,OBJ_ITEM> arr1, arr2;
	OBJ_ITEM item;
	CGeometry *pObj;
	const CShapeLine *pSL;
	PT_3DEX *pts;
	int size1, size2;
	for( int i=0; i<num1; i++)
	{
		pObj = ftr1[i]->GetGeometry();
		if( !pObj )continue;
		
		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )continue;

		if( pObj->GetDataPointSum()<2 )continue;
		
		//精细检验是否与第一线段相交，记录交点
		pSL = pObj->GetShape();
		if( !pSL )continue;
		
		PT_3D ret;
		double t=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k =0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e1.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if(/* pts[1].code==GRBUFFER_PTCODE_LINETO &&*/ e1.bIntersect(&pts[0],&pts[1]) )
					{
						//求相交点
						if( !CGrTrim::Intersect(m_ptLine,m_ptLine+1,&pts[0],&pts[1],&ret,&t) )
							continue;
						

						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						
						item.id = FtrToHandle(ftr1[i]);item.pos= pos+j; item.ret= ret; item.t = t;
						
						//按照 t 大小排序插入
						size1 = arr1.GetSize();
						for( int m=0; m<size1 && item.t>=arr1[m].t; m++);
						if( m<size1 )arr1.InsertAt(m,item);
						else arr1.Add(item);
						
						goto FINDOVER_1;
					}
				}
			}
			pos += pUnit->nuse;		
		}
FINDOVER_1:;
	}
	
	for( i=0; i<num2; i++)
	{
		pObj = ftr2[i]->GetGeometry();
		if( !pObj )continue;
		
		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;

		if( pObj->GetDataPointSum()<2 )continue;
		
		//精细检验是否与第二线段相交，记录交点
		pSL = pObj->GetShape();
		if( !pSL )continue;
		
		PT_3D ret;
		double t=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k=0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e2.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if( /*pts[1].code==GRBUFFER_PTCODE_LINETO && */e2.bIntersect(&pts[0],&pts[1]) )
					{
						//求相交点
						if( !CGrTrim::Intersect(m_ptLine+2,m_ptLine+3,&pts[0],&pts[1],&ret,&t) )
							continue;						

						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						
						item.id = FtrToHandle(ftr2[i]); item.pos= pos+j; item.ret= ret; item.t = t;
						
						//按照 t 大小排序插入
						size1 = arr2.GetSize();
						for( int m=0; m<size1 && item.t>=arr2[m].t; m++);
						if( m<size1 )arr2.InsertAt(m,item);
						else arr2.Add(item);
						
						goto FINDOVER_2;
					}
				}
			}
			pos += pUnit->nuse;
		}
FINDOVER_2:;
	}	
	//对排序好的两个地物组作匹配和合并
	size1 = arr1.GetSize(); size2 = arr2.GetSize();
	CGeometry *pObj2;
	PT_3DEX expt1, expt2;
	if( m_bMatchHeight )
	{
		CUndoFtrs undo(m_pEditor,Name());
		
		int m = -1, order = 0;
		//判断顺序
		for( i=0; i<size1; i++)
		{
			FTR_HANDLE id = arr1[i].id;
			pObj = HandleToFtr(id)->GetGeometry();
			if( !pObj )continue;
			
			double len1 = pObj->GetShape()->GetLength();
			double lenpos1 = pObj->GetShape()->GetLength(&(arr1[i].ret));
			
			if( lenpos1<len1/2 )
				expt1 = pObj->GetDataPoint(pObj->GetDataPointSum()-1);
			else
				expt1 = pObj->GetDataPoint(0);
			
			for( int j=0; j<size2; j++)
			{
				if( id==arr2[j].id )continue;
				pObj2= HandleToFtr(arr2[j].id)->GetGeometry();
				if( !pObj2 )continue;
				
				double len2 = pObj2->GetShape()->GetLength();
				double lenpos2 = pObj2->GetShape()->GetLength(&(arr2[j].ret));
				
				if( lenpos2>=len2/2 )
					expt2 = pObj2->GetDataPoint(pObj2->GetDataPointSum()-1);
				else
					expt2 = pObj2->GetDataPoint(0);
				
				if( fabs(expt1.z-expt2.z)<1e-4 )
				{
					if( m>=0 )order += (j-m);
					m = j;
				}
			}
		}
		
		//依次连接
		int s = 0, e = size2, d = 1;
		if( order<0 )
		{
			s = size2-1; e = -1; d = -1;
		}
		m = s;
		for( i=0; i<size1; i++)
		{
			for( int j=m; j!=e; j+=d)
			{
				FTR_HANDLE id = arr1[i].id;
				if( id==arr2[j].id )continue;
				
				if (id==0||arr2[j].id==0)
				{
					continue;	
				}
				pObj = HandleToFtr(id)->GetGeometry();
				pObj2= HandleToFtr(arr2[j].id)->GetGeometry();
				
				if( !pObj || !pObj2 )continue;

				if (!m_bCrossLayer && PDOC(m_pEditor)->GetFtrLayerIDOfFtr(id) != PDOC(m_pEditor)->GetFtrLayerIDOfFtr(arr2[j].id))
					continue;
				
				double len1 = pObj->GetShape()->GetLength();
				double lenpos1 = pObj->GetShape()->GetLength(&(arr1[i].ret));
				
				double len2 = pObj2->GetShape()->GetLength();
				double lenpos2 = pObj2->GetShape()->GetLength(&(arr2[j].ret));
				
				if( lenpos1<len1/2 )
					expt1 = pObj->GetDataPoint(pObj->GetDataPointSum()-1);
				else
					expt1 = pObj->GetDataPoint(0);
				
				if( lenpos2>=len2/2 )
					expt2 = pObj2->GetDataPoint(pObj2->GetDataPointSum()-1);
				else
					expt2 = pObj2->GetDataPoint(0);
				
				if( fabs(expt1.z-expt2.z)>=1e-4 )continue;
				
				pObj = pObj->Clone();
				if( !pObj )
				{
					m = j;
					break;
				}
				
				CArray<PT_3DEX,PT_3DEX> arrPts;
				CArray<PT_3DEX,PT_3DEX> arrPts2;
				pObj->GetShape(arrPts2);
				int ptnum = arrPts2.GetSize();

				if( lenpos1<len1/2 )
				{					
					for( int k=ptnum-1; k>=0; k--)
					{
						expt1 = arrPts2[k];
						
						arrPts.Add(expt1);
					}
				}
				else
				{				
					for( int k=0; k<ptnum; k++)
					{
						expt1 = arrPts2[k];
						arrPts.Add(expt1);
					}
				}
				
				arrPts2.RemoveAll();
				pObj2->GetShape(arrPts2);
				ptnum = arrPts2.GetSize();
				if( lenpos2>=len2/2 )
				{			
					for( int k=ptnum-1; k>=0; k--)
					{
						expt1 = arrPts2[k];
						arrPts.Add(expt1);
					}
				}
				else
				{			
					for( int k=0; k<ptnum; k++)
					{
						expt1 = arrPts2[k];
						arrPts.Add(expt1);
					}
				}
				
				arrPts.SetSize(GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize()));

				pObj->CreateShape(arrPts.GetData(),arrPts.GetSize());
				
				if( pObj->GetDataPointSum()<2  )
				{
					m = j;
					delete pObj;
					break;
				}				
				CFeature *pFtr = HandleToFtr(id)->Clone();
			    int FtrLayID = 	m_pEditor->GetFtrLayerIDOfFtr(id);//必须在地物删除之前获得，着实不方便
				if (!pFtr) return;

				pFtr->SetID(OUID());			
				
				pFtr->SetGeometry(pObj);
				m_pEditor->AddObject(pFtr,FtrLayID);
				
				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(id),pFtr);
				undo.AddNewFeature(FtrToHandle(pFtr));
				
				m_pEditor->DeleteObject(id);				
				m_pEditor->DeleteObject(arr2[j].id);
				
				undo.AddOldFeature(id);
				undo.AddOldFeature(arr2[j].id);

				arr1[i].id = 0;
				arr2[j].id = 0;

				m = j;
				break;
			}
		}
		undo.Commit();
	}
	else
	{
		CUndoFtrs undo(m_pEditor,Name());
		
		for( i=0; i<size1 && i<size2; i++)
		{
			FTR_HANDLE id = arr1[i].id;
			if( id!=arr2[i].id )
			{
				pObj = HandleToFtr(id)->GetGeometry();
				pObj2= HandleToFtr(arr2[i].id)->GetGeometry();
				
				if( !pObj || !pObj2 )continue;

				if (!m_bCrossLayer && PDOC(m_pEditor)->GetFtrLayerIDOfFtr(id) != PDOC(m_pEditor)->GetFtrLayerIDOfFtr(arr2[i].id))
					continue;
				
				double len1 = pObj->GetShape()->GetLength();
				double lenpos1 = pObj->GetShape()->GetLength(&(arr1[i].ret));
				
				double len2 = pObj2->GetShape()->GetLength();
				double lenpos2 = pObj2->GetShape()->GetLength(&(arr2[i].ret));
				
				pObj = pObj->Clone();
				if( !pObj )continue;
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pObj->GetShape(arrPts);
				if( lenpos1<len1/2 )
				{
					int ptnum = arrPts.GetSize();					
					for( int k=0; k<(ptnum>>1); k++)
					{
						expt1 = arrPts.GetAt(k); 
						expt2 = arrPts.GetAt(ptnum-k-1);
						arrPts.SetAt(k,expt2); 
						arrPts.SetAt(ptnum-k-1,expt1);
					}
				}

				CArray<PT_3DEX,PT_3DEX> arrPts2;
				pObj2->GetShape(arrPts2);
				
				if( lenpos2>=len2/2 )
				{
					int ptnum = arrPts2.GetSize();					
					for( int k=ptnum-1; k>=0; k--)
					{
						expt1 = arrPts2[k];
						arrPts.Add(expt1);
					}
				}
				else
				{
					int ptnum = arrPts2.GetSize();				
					for( int k=0; k<ptnum; k++)
					{
						expt1 = arrPts2[k];
						arrPts.Add(expt1);
					}
				}

				arrPts.SetSize(GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize()));

				pObj->CreateShape(arrPts.GetData(),arrPts.GetSize());
				CFeature *pFtr = HandleToFtr(id)->Clone();
				int FtrlayID = m_pEditor->GetFtrLayerIDOfFtr(id);
				if(!pFtr) return;				
					
				pFtr->SetID(OUID());
				pFtr->SetGeometry(pObj);
				m_pEditor->AddObject(pFtr,FtrlayID);

				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(id),pFtr);
				undo.AddNewFeature(FtrToHandle(pFtr));			

				m_pEditor->DeleteObject(id);
				m_pEditor->DeleteObject(arr2[i].id);
				
				undo.AddOldFeature(id);
				undo.AddOldFeature(arr2[i].id);	
			}
		}
		undo.Commit();
	}
}


//////////////////////////////////////////////////////////////////////
// CAutoLinkContourCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAutoLinkContourCommand,CEditCommand)

CAutoLinkContourCommand::CAutoLinkContourCommand()
{
	m_nState = -1;
	m_nPart = modePart;
	m_lftolerace=0;
	m_bMatchHeight = TRUE;
	m_bRiverHeightError = FALSE;
	if(m_arrObjHandles.GetSize()>0) m_arrObjHandles.RemoveAll();
	strcat(m_strRegPath,"\\AutoLinkContour");
}

CAutoLinkContourCommand::~CAutoLinkContourCommand()
{

}



CString CAutoLinkContourCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_AUTOLINKCONTOUR);
}

void CAutoLinkContourCommand::Start()
{
 	if( !m_pEditor )return;	
	
	if(m_arrObjHandles.GetSize()>0) m_arrObjHandles.RemoveAll();
	
	//m_pDoc->UpdateAllViews(NULL,hc_Attach_Accubox);
	m_nPart = modePart;
	m_lftolerace=0;
	m_nStep = 0;
	m_bMatchHeight = TRUE;
	
	CEditCommand::Start();
	
	m_pEditor->CloseSelector();
	if( m_nPart==modeWhole )
	{
		
	;//	OutputTipString(StrFromResID(IDS_CMDTIP_LINKLINE1));
	}
	else
	{		
	;//	OutputTipString(StrFromResID(IDS_CMDTIP_LINKLINE2));
	}
}

void CAutoLinkContourCommand::Abort()
{	
	
	CEditCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}


int KickoffSame2DPoints_notPenMove(CArray<PT_3DEX,PT_3DEX>& arrPts);
 
 
void CAutoLinkContourCommand::PtClick(PT_3D &pt, int flag)
{
	CUndoFtrs undo(m_pEditor,Name());
	CArray<FTR_HANDLE,FTR_HANDLE> arr_addobj;
	CArray<FTR_HANDLE,FTR_HANDLE> arr_delobj;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	if(m_nPart==modePart)
	{
		if (m_nStep==0)
		{
			//PT_4D pt4d(pt);		
			//m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt4d,1);			
			m_pPts[0] = pt;
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;
		}
		else if (m_nStep==1)
		{
			
			//PT_4D pt4d(pt);		
			//m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt4d,1);
			//m_pt4dEnd=pt4d;
			m_pPts[1] = pt;
			GotoState(PROCSTATE_PROCESSING);
			m_nStep=2;
			
			//	m_pDoc->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)&buf);
		}
		else
		{
			m_pEditor->UpdateDrag(ud_ClearDrag);
			
			
			PT_2D t1[4];
			t1[0].x = m_pPts[0].x; t1[0].y = m_pPts[0].y;
			
			t1[1].x = m_pPts[0].x; t1[1].y = m_pPts[1].y;
			
			t1[2].x = m_pPts[1].x; t1[2].y = m_pPts[0].y;
			
			t1[3].x = m_pPts[1].x; t1[3].y = m_pPts[1].y;
			
			Envelope e;
			e.CreateFromPts(t1,4); 
			e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);
			//在指定矩形区域内遍历所有元素，并将满足要求都元素加进m_arrObjHandles中
			int num;
			const CPFeature * ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);
			
			for (int i=0;i<num;i++)
			{
				FTR_HANDLE objhand = FtrToHandle(ftr[i]);
				if(!objhand) continue;
				CGeometry *pObj=HandleToFtr(objhand)->GetGeometry();
				if( !pObj )continue;
				if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )continue;
				if( ((CGeoCurve*)pObj)->IsClosed() )continue;
				if (pObj->GetDataPointSum()<2)
				{
					continue;
				}
				if(m_strLayerCode.IsEmpty()|| CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr[i],m_strLayerCode) )
				{
					m_arrObjHandles.Add(objhand);
				}
			}
			////////////////////////////////////////////////////////////////////////
			int ObjNum=m_arrObjHandles.GetSize();
			CGeometry *pAddObj=NULL;
		
			bool isz=false;
			for (i=0;i<ObjNum;i++)
			{
				FTR_HANDLE objhandle0=m_arrObjHandles.GetAt(i);

				int layerid = PDOC(m_pEditor)->GetFtrLayerIDOfFtr(objhandle0);

				if(HandleToFtr(objhandle0)->IsDeleted()) continue;
				CGeometry *pObj=HandleToFtr(objhandle0)->GetGeometry();
				if (!pObj)
				{
					continue;
				}
				if (CheckObjForZ(pObj))
				{
					isz=true;
				}
				else
					isz=false;

				PT_3DEX pt1,pt2;//用于存储地物的首尾点
				pt1 = pObj->GetDataPoint(0);
			    pt2 = pObj->GetDataPoint(pObj->GetDataPointSum()-1);

				FTR_HANDLE *ids0=NULL,*ids1=NULL;
				/////////先处理首点
				if (IsPtInRect(&pt1))
				{
					Envelope e(pt1.x-m_lftolerace,pt1.x+m_lftolerace,pt1.y-m_lftolerace,pt1.y+m_lftolerace);
					e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
				//	m_pEditor->DeselectAll();
					m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);
					int num0;
					const CPFeature *ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num0);
// 					ids0 = new OBJ_GUID[num0];
// 					if( ids0 )m_pDoc->m_selection.GetFoundObjectHandles(ids0);
					bool isfirst;
					
					double mindis= 1.7976931348623158e+308;
					FTR_HANDLE objhandle1 = 0;//保存要合并的地物的句柄
					for (int j=0;j<num0;j++)
					{
						FTR_HANDLE objhand=FtrToHandle(ftr[j]);
						if (FindEqualObj(m_arrObjHandles,objhand)==-1)
						{
							continue;
						}
						CGeometry *pObj1=HandleToFtr(objhand)->GetGeometry();
						if( !pObj1 )continue;
						if ( !m_strLayerCode.IsEmpty()&&!CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr[j],m_strLayerCode) )
						{
							continue;
						}

						if(!IsSameLayer(objhand,objhandle0))
							continue;

						if (m_bMatchHeight)
						{
							bool isz1 = CheckObjForZ(pObj1);
							if (isz && isz1)
							{
								PT_3DEX ptz;
								ptz = pObj->GetDataPoint(0);
								double lfz=ptz.z;
								ptz = pObj1->GetDataPoint(0);
								if (fabs(lfz-ptz.z)>/*1e-6*/GraphAPI::g_lfZTolerance)
								{
									continue;
								}
							}
						}

						PT_3DEX pt3,pt4;
						
						pt3 = pObj1->GetDataPoint(0);
						pt4 = pObj1->GetDataPoint(pObj1->GetDataPointSum()-1);
						if (objhand==objhandle0)
						{
							if (GraphAPI::GGet2DDisOf2P(pt4,pt3)<m_lftolerace&&IsPtInRect(&pt4))
							{
								if( mindis > GraphAPI::GGet2DDisOf2P(pt4,pt3) )
								{
									objhandle1=objhand;
									mindis = GraphAPI::GGet2DDisOf2P(pt4,pt3);
									//break;
								}
							}
						}
						else
						{
							if (GraphAPI::GGet2DDisOf2P(pt1,pt3)<m_lftolerace&&IsPtInRect(&pt3))
							{
								if (mindis>GraphAPI::GGet2DDisOf2P(pt1,pt3))
								{
									objhandle1=objhand;
									mindis=GraphAPI::GGet2DDisOf2P(pt1,pt3);					
									isfirst=true;	
								}
							}
							if (GraphAPI::GGet2DDisOf2P(pt1,pt4)<m_lftolerace&&IsPtInRect(&pt4))
							{
								if (mindis>GraphAPI::GGet2DDisOf2P(pt1,pt4))
								{
									objhandle1=objhand;
									mindis=GraphAPI::GGet2DDisOf2P(pt1,pt4);
									isfirst=false;	
								}
							}
						}
						//判断河流高程是否有误
						if(m_bRiverHeightError)
						{
							if(isfirst)
							{
								if(pt1.z>pt2.z && pt3.z>pt4.z)
								{
									objhandle1 = 0;//连接无效
								}
								else if(pt1.z<pt2.z && pt3.z<pt4.z)
								{
									objhandle1 = 0;
								}
							}
							else
							{
								if(pt1.z>pt2.z && pt4.z>pt3.z)
								{
									objhandle1 = 0;//连接无效
								}
								else if(pt1.z<pt2.z && pt4.z<pt3.z)
								{
									objhandle1 = 0;
								}
							}
						}
					}

					if (objhandle1!=0)
					{
						if(objhandle1==objhandle0)
						{
							//合并,删除原地物，不增加到尾部
							pAddObj = pObj->Clone();
							if( !pAddObj )continue;
							CFeature *pFtr = HandleToFtr(objhandle0)->Clone();
//							int layerid = m_pEditor->GetFtrLayerIDOfFtr(objhandle0);
							if(!pFtr) return;
							m_pEditor->DeleteObject(objhandle0);
							int tem=FindEqualObj(arr_addobj,objhandle0);
							if(tem == -1)
								arr_delobj.Add(objhandle0);
							else
								arr_addobj.RemoveAt(tem);
												
							pFtr->SetID(OUID());
							((CGeoCurve*)pAddObj)->EnableClose(TRUE);
							pFtr->SetGeometry(pAddObj);
							m_pEditor->AddObject(pFtr,layerid);
							arr_addobj.Add(FtrToHandle(pFtr));
						//	arr_addobj.(m_pDoc->AddObject(pAddObj,objhandle0.layhdl));//jjjj
							pAddObj=NULL;
							pObj = NULL;
							continue;
						}
						else
						{
							pAddObj = pObj->Clone();
							if( !pAddObj )continue;
							CArray<PT_3DEX,PT_3DEX> arr;
							pAddObj->GetShape(arr);
							m_pEditor->DeleteObject(objhandle0);
							int tem_idx0=FindEqualObj(arr_addobj,objhandle0);
							if(tem_idx0 == -1)
								arr_delobj.Add(objhandle0);
							else
								arr_addobj.RemoveAt(tem_idx0);
							CGeometry *pObj2=HandleToFtr(objhandle1)->GetGeometry();
							if (!pObj2) continue;
							PT_3DEX tem;
							if (isfirst)
							{						
								for(int k=0;k<pObj2->GetDataPointSum();k++)
								{
									tem = pObj2->GetDataPoint(k);
									arr.InsertAt(0,tem);
								}
								
							}
							else
							{
								for(int k=pObj2->GetDataPointSum()-1;k>=0;k--)
								{
									tem = pObj2->GetDataPoint(k);
									arr.InsertAt(0,tem);
								}
								
							}
							KickoffSame2DPoints_notPenMove(arr);
							pAddObj->CreateShape(arr.GetData(),arr.GetSize());
							m_pEditor->DeleteObject(objhandle1);
							int tem_idx1=FindEqualObj(arr_addobj,objhandle1);
							if(tem_idx1 == -1)
								arr_delobj.Add(objhandle1);
							else
								arr_addobj.RemoveAt(tem_idx1);
							
							
							//往objhandle0所代表的地物中加点，并删除，有待尾点处理
						}
						
					}
				}
			
				/////////处理尾点

				if (IsPtInRect(&pt2))
				{
					Envelope e1(pt2.x-m_lftolerace,pt2.x+m_lftolerace,pt2.y-m_lftolerace,pt2.y+m_lftolerace);
					//m_pDoc->m_selection.DeselectAll();
					e1.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
					m_pEditor->GetDataQuery()->FindObjectInRect(e1,m_pEditor->GetCoordWnd().m_pSearchCS);
					
					int num1;
					const CPFeature *ftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
				
					bool isfirst1;
					
					double mindis1= 1.7976931348623158e+308 ;
					FTR_HANDLE objhandle2 = 0;//保存要合并的地物的句柄
					for ( int j=0;j<num1;j++)
					{
						FTR_HANDLE objhand=FtrToHandle(ftr1[j]);
						if (FindEqualObj(m_arrObjHandles,objhand)==-1)
						{
							continue;
						}
						CGeometry *pObj1=HandleToFtr(objhand)->GetGeometry();
						if( !pObj1 )continue;
						if (!m_strLayerCode.IsEmpty()&& !CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr1[j],m_strLayerCode) )
						{
							continue;
						}
						
						if(!IsSameLayer(objhand,objhandle0))
							continue;
						
						if (m_bMatchHeight)
						{
							bool isz1 = CheckObjForZ(pObj1);
							if (isz && isz1)
							{
								PT_3DEX ptz;
								ptz = pObj->GetDataPoint(0);
								double lfz=ptz.z;
								ptz = pObj1->GetDataPoint(0);
								if (fabs(lfz-ptz.z)>/*1e-6*/GraphAPI::g_lfZTolerance)
								{
									continue;
								}
							}
						}

						PT_3DEX pt3,pt4;
						
						pt3 = pObj1->GetDataPoint(0);
						pt4 = pObj1->GetDataPoint(pObj1->GetDataPointSum()-1);
						
						if (objhand==objhandle0)
						{
							if (GraphAPI::GGet2DDisOf2P(pt4,pt3)<m_lftolerace&&IsPtInRect(&pt3))
							{
								if( mindis1 > GraphAPI::GGet2DDisOf2P(pt4,pt3) )
								{
									objhandle2=objhand;
									mindis1 = GraphAPI::GGet2DDisOf2P(pt4,pt3);
									//break;
								}
							}
						}
						else
						{
							if (GraphAPI::GGet2DDisOf2P(pt2,pt3)<m_lftolerace&&IsPtInRect(&pt3))
							{
								if (mindis1>GraphAPI::GGet2DDisOf2P(pt2,pt3))
								{
									objhandle2=objhand;
									mindis1=GraphAPI::GGet2DDisOf2P(pt2,pt3);					
									isfirst1=true;	
								}
							}
							if (GraphAPI::GGet2DDisOf2P(pt2,pt4)<m_lftolerace&&IsPtInRect(&pt4))//和自身的情况存在
							{
								if (mindis1>GraphAPI::GGet2DDisOf2P(pt2,pt4))
								{
									objhandle2=objhand;
									mindis1=GraphAPI::GGet2DDisOf2P(pt2,pt4);
									isfirst1=false;	
								}
							}
						}
						//判断河流高程是否有误
						if(m_bRiverHeightError)
						{
							if(isfirst1)
							{
								if(pt1.z>pt2.z && pt4.z>pt3.z)
								{
									objhandle2 = 0;//连接无效
								}
								else if(pt1.z<pt2.z && pt4.z<pt3.z)
								{
									objhandle2 = 0;
								}
							}
							else
							{
								if(pt1.z>pt2.z && pt3.z>pt4.z)
								{
									objhandle2 = 0;//连接无效
								}
								else if(pt1.z<pt2.z && pt3.z<pt4.z)
								{
									objhandle2 = 0;
								}
							}
						}
					}
					
					if (objhandle2!=0)
					{
						if(!pAddObj)
						{
							pAddObj=pObj->Clone();
							m_pEditor->DeleteObject(objhandle0);
							int tem=FindEqualObj(arr_addobj,objhandle0);
							if(tem == -1)
								arr_delobj.Add(objhandle0);
							else
								arr_addobj.RemoveAt(tem);
							
						}
						if( !pAddObj )continue;
						CArray<PT_3DEX,PT_3DEX> arr;
						pAddObj->GetShape(arr);
						CGeometry *pObj2=HandleToFtr(objhandle2)->GetGeometry();
						if (!pObj2) continue;
						PT_3DEX tem;
						if (isfirst1)
						{						
							for(int k=0;k<pObj2->GetDataPointSum();k++)
							{
								tem = pObj2->GetDataPoint(k);
								arr.Add(tem);
							}
							//m_pDoc->AddObject(pAddObj,objhandle0.layhdl);
						}
						else
						{
							for(int k=pObj2->GetDataPointSum()-1;k>=0;k--)
							{
								tem = pObj2->GetDataPoint(k);
								arr.Add(tem);
							}
							//	m_pDoc->AddObject(pAddObj,objhandle0.layhdl);
						}
						KickoffSame2DPoints_notPenMove(arr);
						pAddObj->CreateShape(arr.GetData(),arr.GetSize());
						m_pEditor->DeleteObject(objhandle2);
						int tem_idx2=FindEqualObj(arr_addobj,objhandle2);
						if(tem_idx2 == -1)
							arr_delobj.Add(objhandle2);
						else
							arr_addobj.RemoveAt(tem_idx2);
						//合并，删除原地物，并增加到尾部
						
					}
					
				}
			

				if(pAddObj)
				{
					CFeature *pFtr = HandleToFtr(objhandle0)->Clone();					
					if(!pFtr) return;
					pFtr->SetGeometry(pAddObj);
					pFtr->SetID(OUID());
					pFtr->SetToDeleted(FALSE);
					m_pEditor->AddObject(pFtr,layerid);
					
					arr_addobj.Add(FtrToHandle(pFtr));
					ObjNum++;
					m_arrObjHandles.Add(FtrToHandle(pFtr));
				}
				//	delete pAddObj;
				pAddObj=NULL;
				pObj = NULL;
				
				
			}
			m_pEditor->RefreshView();
		
			m_nStep=3;
			Finish();
		}
		
	}
	else
	{
		//遍历所有元素，将指定层码的元素的句柄加到m_arrObjHandles中
		CFeature *pFtr=NULL;	
				
		CFtrLayer *pLayer = NULL;

		int nLayNum = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerCount(),i,j;
		for( i=0; i<nLayNum; i++)
		{
			pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
			if( !pLayer || !pLayer->IsVisible())continue;
			
			int nObjNum = pLayer->GetObjectCount();
			for( j=0; j<nObjNum; j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )continue;
				if( ((CGeoCurve*)(pFtr->GetGeometry()))->IsClosed() )continue;				
				if(m_strLayerCode.IsEmpty()|| CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pFtr,m_strLayerCode) )
				{
					m_arrObjHandles.Add(FtrToHandle(pFtr));
					
				}
				pFtr = NULL;
			}
		}

		////////循环对每一个符合条件的地物进行处理///////////////////////////////////////////////
		int ObjNum=m_arrObjHandles.GetSize();
		CGeometry *pAddObj=NULL;
		int isz=false;
		for (i=0;i<ObjNum;i++)
		{
			FTR_HANDLE objhandle0 = m_arrObjHandles.GetAt(i);
			if(HandleToFtr(objhandle0)->IsDeleted()) continue;
			CGeometry *pObj=HandleToFtr(objhandle0)->GetGeometry();
			if (!pObj)
			{
				continue;
			}
		
			if( pObj->GetDataPointSum()<2 )continue;

			if (CheckObjForZ(pObj))
			{
				isz=true;
			}
			else
				isz=false;

            PT_3DEX pt1,pt2;//用于存储地物的首尾点
			pt1 = pObj->GetDataPoint(0);
			pt2 = pObj->GetDataPoint(pObj->GetDataPointSum()-1);
			/////////先处理首点
			Envelope e(pt1.x-m_lftolerace,pt1.x+m_lftolerace,pt1.y-m_lftolerace,pt1.y+m_lftolerace);
			e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);
			int num0;
			const CPFeature * ftr0 = m_pEditor->GetDataQuery()->GetFoundHandles(num0);
		
			bool isfirst;

			//记录附近地物的数目，超过1个，就不连接
			int nearObjCount = 0;
			
			double mindis= 1.7976931348623158e+308;
			FTR_HANDLE objhandle1 = 0;//保存要合并的地物的句柄
			for ( j=0;j<num0;j++)
			{
				FTR_HANDLE objhand=FtrToHandle(ftr0[j]);
				CGeometry *pObj1=HandleToFtr(objhand)->GetGeometry();
				if( !pObj1 )continue;
				if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )continue;
				if( ((CGeoCurve*)pObj1)->IsClosed() )continue;			
				if( pObj1->GetDataPointSum()<2 )continue;
				if ( !m_strLayerCode.IsEmpty()&&!CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr0[j],m_strLayerCode) )
				{
					continue;
				}
				
				if(!IsSameLayer(objhand,objhandle0))
					continue;

				if (isz)
				{
					if( m_bMatchHeight && CheckObjForZ(pObj1) )
					{
						PT_3DEX ptz;
						ptz = pObj->GetDataPoint(0);
						double lfz=ptz.z;
						ptz = pObj1->GetDataPoint(0);
						if (fabs(lfz-ptz.z)>GraphAPI::g_lfZTolerance)
						{
							continue;
						}
					}
				}

				PT_3DEX pt3,pt4;
				
				pt3 = pObj1->GetDataPoint(0);
				pt4 = pObj1->GetDataPoint(pObj1->GetDataPointSum()-1);
				if (objhand==objhandle0)
				{
					if (GraphAPI::GGet2DDisOf2P(pt4,pt3)<m_lftolerace)
					{
						nearObjCount++;
						if( mindis > GraphAPI::GGet2DDisOf2P(pt4,pt3) )
						{
							objhandle1=objhand;
							mindis = GraphAPI::GGet2DDisOf2P(pt4,pt3);
							//	break;
						}
					}
				}
				else
				{
					if (GraphAPI::GGet2DDisOf2P(pt1,pt3)<m_lftolerace)
					{
						nearObjCount++;
						if (mindis>GraphAPI::GGet2DDisOf2P(pt1,pt3))
						{
							objhandle1=objhand;
							mindis=GraphAPI::GGet2DDisOf2P(pt1,pt3);					
							isfirst=true;	
						}
					}
					if (GraphAPI::GGet2DDisOf2P(pt1,pt4)<m_lftolerace)
					{
						nearObjCount++;
						if (mindis>GraphAPI::GGet2DDisOf2P(pt1,pt4))
						{
							objhandle1=objhand;
							mindis=GraphAPI::GGet2DDisOf2P(pt1,pt4);
							isfirst=false;	
						}
					}
				}
				//判断河流高程是否有误
				if(m_bRiverHeightError)
				{
					if(isfirst)
					{
						if(pt1.z>pt2.z && pt3.z>pt4.z)
						{
							objhandle1 = 0;//连接无效
						}
						else if(pt1.z<pt2.z && pt3.z<pt4.z)
						{
							objhandle1 = 0;
						}
					}
					else
					{
						if(pt1.z>pt2.z && pt4.z>pt3.z)
						{
							objhandle1 = 0;//连接无效
						}
						else if(pt1.z<pt2.z && pt4.z<pt3.z)
						{
							objhandle1 = 0;
						}
					}
				}
			}

			if (objhandle1!=0 && nearObjCount==1)
			{
				if(objhandle1==objhandle0)
				{
					//合并,删除原地物，不增加到尾部
					pAddObj = pObj->Clone();
					if( !pAddObj )continue;
					CFeature *pFtr = HandleToFtr(objhandle0)->Clone();
					if(!pFtr) return;
					
					PT_3DEX Pt;
					((CGeoCurve*)pAddObj)->EnableClose(TRUE);

					pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(objhandle0));

					pFtr->SetGeometry(pAddObj);
					pFtr->SetID(OUID());
					m_pEditor->AddObject(pFtr,pLayer->GetID());

					GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(objhandle0),pFtr);

					m_pEditor->DeleteObject(objhandle0);
					int tem = FindEqualObj(arr_addobj,objhandle0);
					if(tem == -1)
						arr_delobj.Add(objhandle0);
					else
						arr_addobj.RemoveAt(tem);

					arr_addobj.Add(FtrToHandle(pFtr));
					
					pAddObj=NULL;
					pObj = NULL;
					continue;
				}
				else
				{
					pAddObj = pObj->Clone();
					if( !pAddObj )continue;
					CArray<PT_3DEX,PT_3DEX> arr;
					pAddObj->GetShape(arr);
					m_pEditor->DeleteObject(objhandle0);
					int tem_idx3=FindEqualObj(arr_addobj,objhandle0);
					if(tem_idx3 == -1)
						arr_delobj.Add(objhandle0);
					else
						arr_addobj.RemoveAt(tem_idx3);
					CGeometry *pObj2=HandleToFtr(objhandle1)->GetGeometry();
					if (!pObj2) continue;
					PT_3DEX tem;
					if (isfirst)
					{		
						CArray<PT_3DEX,PT_3DEX> temp;
						pObj2->GetShape(temp);
						
						for(int k=0;k<temp.GetSize();k++)
						{
							tem = temp.GetAt(k);
							arr.InsertAt(0,tem);
						}
						
					}
					else
					{
						CArray<PT_3DEX,PT_3DEX> temp;
						pObj2->GetShape(temp);
						for(int k=temp.GetSize()-1;k>=0;k--)
						{
							tem = temp.GetAt(k);
							arr.InsertAt(0,tem);
						}
						
					}

					KickoffSame2DPoints_notPenMove(arr);
					pAddObj->CreateShape(arr.GetData(),arr.GetSize());
					m_pEditor->DeleteObject(objhandle1);
					int tem_idx4=FindEqualObj(arr_addobj,objhandle1);
					if(tem_idx4 == -1)
						arr_delobj.Add(objhandle1);
					else
						arr_addobj.RemoveAt(tem_idx4);
					//往objhandle0所代表的地物中加点，并删除，有待尾点处理
				}
				
			}
		
			/////////处理尾点
			Envelope e1(pt2.x-m_lftolerace,pt2.x+m_lftolerace,pt2.y-m_lftolerace,pt2.y+m_lftolerace);			
			e1.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e1,m_pEditor->GetCoordWnd().m_pSearchCS);
			int num1;
			const CPFeature * ftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
		
			bool isfirst1;

			nearObjCount = 0;
			
			double mindis1= 1.7976931348623158e+308 ;
			FTR_HANDLE objhandle2 = 0;//保存要合并的地物的句柄
			for (j=0;j<num1;j++)
			{
				FTR_HANDLE objhand=FtrToHandle(ftr1[j]);
				CGeometry *pObj1=HandleToFtr(objhand)->GetGeometry();
				if( !pObj1 )continue;
				if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )continue;
				if( ((CGeoCurve*)pObj1)->IsClosed() )continue;			
				if( pObj1->GetDataPointSum()<2 )continue;
				if (!m_strLayerCode.IsEmpty()&& !CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr1[j],m_strLayerCode))
				{
					continue;
				}
				
				if(!IsSameLayer(objhand,objhandle0))
					continue;

				if (isz)
				{
					if (m_bMatchHeight && CheckObjForZ(pObj1))
					{
						PT_3DEX ptz;
						ptz = pObj->GetDataPoint(0);
						double lfz=ptz.z;
						ptz = pObj1->GetDataPoint(0);
						if (fabs(lfz-ptz.z)>/*1e-6*/GraphAPI::g_lfZTolerance)
						{
							continue;
						}
					}
				}

				PT_3DEX pt3,pt4;
				
				pt3 = pObj1->GetDataPoint(0);
				pt4 = pObj1->GetDataPoint(pObj1->GetDataPointSum()-1);
				
				if (objhand==objhandle0)
				{
					if (GraphAPI::GGet2DDisOf2P(pt4,pt3)<m_lftolerace)
					{
						nearObjCount++;
						if( mindis1 > GraphAPI::GGet2DDisOf2P(pt4,pt3) )
						{
							objhandle2=objhand; 
							mindis1 = GraphAPI::GGet2DDisOf2P(pt4,pt3);
							//break;
						}
					}
				}
				else
				{
					if (GraphAPI::GGet2DDisOf2P(pt2,pt3)<m_lftolerace)
					{
						nearObjCount++;
						if (mindis1>GraphAPI::GGet2DDisOf2P(pt2,pt3))
						{
							objhandle2=objhand;
							mindis1=GraphAPI::GGet2DDisOf2P(pt2,pt3);
							isfirst1=true;	
						}
					}
					if (GraphAPI::GGet2DDisOf2P(pt2,pt4)<m_lftolerace)//和自身的情况存在
					{
						nearObjCount++;
						if (mindis1>GraphAPI::GGet2DDisOf2P(pt2,pt4))
						{
							objhandle2=objhand;
							mindis1=GraphAPI::GGet2DDisOf2P(pt2,pt4);
							isfirst1=false;	
						}
					}
				}
				//判断河流高程是否有误
				if(m_bRiverHeightError)
				{
					if(isfirst1)
					{
						if(pt1.z>pt2.z && pt4.z>pt3.z)
						{
							objhandle2 = 0;//连接无效
						}
						else if(pt1.z<pt2.z && pt4.z<pt3.z)
						{
							objhandle2 = 0;
						}
					}
					else
					{
						if(pt1.z>pt2.z && pt3.z>pt4.z)
						{
							objhandle2 = 0;//连接无效
						}
						else if(pt1.z<pt2.z && pt3.z<pt4.z)
						{
							objhandle2 = 0;
						}
					}
				}
			}

			if (objhandle2!=0 && nearObjCount==1 )
			{
				if(!pAddObj)
				{
					pAddObj=pObj->Clone();
					
					m_pEditor->DeleteObject(objhandle0);
					int tem=FindEqualObj(arr_addobj,objhandle0);
					if(tem == -1)
						arr_delobj.Add(objhandle0);
					else
						arr_addobj.RemoveAt(tem);
				}
				if( !pAddObj )continue;
				CArray<PT_3DEX,PT_3DEX> arr;
				pAddObj->GetShape(arr);
				CGeometry *pObj2=HandleToFtr(objhandle2)->GetGeometry();
				if (!pObj2) continue;
				PT_3DEX tem;
				if (isfirst1)
				{		
					CArray<PT_3DEX,PT_3DEX> temp;
					pObj2->GetShape(temp);
					arr.Append(temp);
// 					for(int k=0;k<pObj2->GetDataPointSum();k++)
// 					{
// 						tem = pObj2->GetDataPoint(k);
// 						arr.Add(tem);
// 					}
					KickoffSame2DPoints_notPenMove(arr);
					pAddObj->CreateShape(arr.GetData(),arr.GetSize());
				}
				else
				{
					CArray<PT_3DEX,PT_3DEX> temp;
					pObj2->GetShape(temp);
					for(int k=temp.GetSize()-1;k>=0;k--)
					{
						tem = temp.GetAt(k);
						arr.Add(tem);
					}
					KickoffSame2DPoints_notPenMove(arr);
					pAddObj->CreateShape(arr.GetData(),arr.GetSize());
				}
				m_pEditor->DeleteObject(objhandle2);
				int tem_idx5=FindEqualObj(arr_addobj,objhandle2);
				if(tem_idx5 == -1)
					arr_delobj.Add(objhandle2);
				else
					arr_addobj.RemoveAt(tem_idx5);
				//合并，删除原地物，并增加到尾部 
			}
			if(pAddObj)
			{
				CFeature *pFtr = HandleToFtr(objhandle0)->Clone();
				pFtr->SetID(OUID());
				pFtr->SetToDeleted(FALSE);
				if(!pFtr) return;
				pFtr->SetGeometry(pAddObj);

				pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(objhandle0));
				m_pEditor->AddObject(pFtr,pLayer->GetID());
				arr_addobj.Add(FtrToHandle(pFtr));
				ObjNum++;
				m_arrObjHandles.Add(FtrToHandle(pFtr));
			}
			pAddObj=NULL;
			pObj = NULL;
		
		}
		m_pEditor->RefreshView();
		Finish();	
	}
	if(arr_delobj.GetSize()<=0&&arr_addobj.GetSize()<=0)
	{
		CEditCommand::PtClick(pt,flag);
		return;
	}

	undo.arrOldHandles.Copy(arr_delobj);
	undo.arrNewHandles.Copy(arr_addobj);
	undo.Commit();

	GOutPut(StrFromResID(IDS_TIP_AUTOLINKFINISH));

	CEditCommand::PtClick(pt,flag);
}

void CAutoLinkContourCommand::PtMove(PT_3D &pt)
{
	if (m_nPart==modePart)
	{
		if(m_nStep==1)
		{
			GrBuffer buf;
			PT_3D pt3d;
	
			buf.BeginLineString(0,0);
			buf.MoveTo(&m_pPts[0]);
			
			pt3d.x = m_pPts[0].x; pt3d.y = pt.y; pt3d.z = m_pPts[0].z;
			buf.LineTo(&pt3d);
			
			pt3d.x = pt.x; pt3d.y = pt.y; pt3d.z = m_pPts[0].z;
			buf.LineTo(&pt);
			
			pt3d.x = pt.x; pt3d.y = m_pPts[0].y; pt3d.z = m_pPts[0].z;
			buf.LineTo(&pt3d);
			
			buf.LineTo(&m_pPts[0]);			
			buf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);			
		}		
	}
}

BOOL CheckNameForLayerCode(CDlgDataSource *pDS, LPCTSTR name, LPCTSTR pstrCode)
{
	if (!pDS || !name||!pstrCode||strlen(pstrCode)<=0)
	{
		return FALSE;
	}
	
	CConfigLibManager *pConfi = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	CScheme *pScheme = pConfi->GetScheme(pDS->GetScale());
	if(!pScheme)return FALSE;
	
	char text[1024];
	strcpy(text,pstrCode);
	
	char *start = text;
	int len = 0;
	while( (len=strlen(start))>0 )
	{
		char *pos0 = start;
		char *pos1 = strchr(pos0,',');
		if( pos1 )
		{
			if( pos1==pos0 )break;
			*pos1 = 0;
			start = pos1+1;
		}
		else
		{
			start += len;
		}
		
		CString ret = pos0;
		__int64 code = _atoi64(pos0);
		if( code>0 )
		{
			pScheme->FindLayerIdx(TRUE,code,ret);
		}
		
		if( stricmp(pos0,name)==0 || stricmp(ret,name)==0 )
			return TRUE;
	}
	
	return FALSE;
}

BOOL CAutoLinkContourCommand::CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode)
{
	if (!pDS||!pFtr)
	{
		return FALSE;
	}

	if(strlen(Layercode)<=0)
		return TRUE;
	
	CFtrLayer *player = pDS->GetFtrLayerOfObject(pFtr);

	return CheckNameForLayerCode(pDS,player->GetName(),Layercode);	
}


void CAutoLinkContourCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCSTR)(m_strLayerCode);
	tab.AddValue(PF_AUTOLINKCONTORLAYERCODE,&CVariantEx(var));
	var = (long)(m_nPart);
	tab.AddValue(PF_AUTOLINKCONTORSCOPE,&CVariantEx(var));
	var = double(m_lftolerace);
	tab.AddValue(PF_AUTOLINKCONTORTOLER,&CVariantEx(var));
	var = (bool)(m_bMatchHeight);
	tab.AddValue(PF_LINKCONTORMATCHHEI,&CVariantEx(var));
	var = (bool)(m_bRiverHeightError);
	tab.AddValue("RiverHeightError",&CVariantEx(var));
}


void CAutoLinkContourCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("AutoLinkContourCommand",StrFromLocalResID(IDS_CMDNAME_AUTOLINKCONTOUR));
	param->AddLayerNameParam(PF_AUTOLINKCONTORLAYERCODE,LPCTSTR(m_strLayerCode),StrFromResID(IDS_CMDPLANE_LAYERCODE));
	param->BeginOptionParam(PF_AUTOLINKCONTORSCOPE,StrFromResID(IDS_CMDPLANE_SCOPE));
	param->AddOption(StrFromResID(IDS_WHOLEMAP),modeWhole,' ',m_nPart==modeWhole);
	param->AddOption(StrFromResID(IDS_PARTMAP),modePart,' ',m_nPart==modePart);	
	param->EndOptionParam();
 	
	param->AddParam(PF_AUTOLINKCONTORTOLER,double(m_lftolerace),StrFromResID(IDS_CMDPLANE_TOLERANCE));

	param->AddParam(PF_LINKCONTORMATCHHEI,bool(m_bMatchHeight),StrFromResID(IDS_CMDPLANE_MATCHHEIGHT));
	param->AddParam("RiverHeightError",bool(m_bRiverHeightError),StrFromResID(IDS_CMDPLANE_CONSIDERZ));
}


void CAutoLinkContourCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_AUTOLINKCONTORLAYERCODE,var) )
	{
		m_strLayerCode = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_AUTOLINKCONTORSCOPE,var) )
	{		
		m_nPart = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_AUTOLINKCONTORTOLER,var) )
	{
		m_lftolerace = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_LINKCONTORMATCHHEI,var) )
	{
		m_bMatchHeight = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"RiverHeightError",var) )
	{
		m_bRiverHeightError = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}



bool CAutoLinkContourCommand::CheckObjForZ( CGeometry *pObj)
{
	if (!pObj) return false;

	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);

	int size = pts.GetSize();
	if (size < 2) return false;

	double z = pts[0].z;
	for(int i=1;i<size;i++)
	{
		PT_3DEX ptex = pts[i];
		if (fabs(z-ptex.z)>=GraphAPI::g_lfZTolerance)
		{
			return false;
		}
	}
	return true;
}

bool CAutoLinkContourCommand::IsPtInRect(PT_3DEX *pt)
{
	Envelope e;
	e = CreateEnvelopeFromPts(m_pPts,2);
	return e.bPtIn(pt);
}

bool CAutoLinkContourCommand::IsSameLayer(FTR_HANDLE h1, FTR_HANDLE h2)
{
	if(h1!=0 && h2!=0)
	{
		if(h1==h2)
			return true;

		CDataSourceEx *pDS = m_pEditor->GetDataSource();
		CFtrLayer *pLayer1 = pDS->GetFtrLayerOfObject(HandleToFtr(h1));
		CFtrLayer *pLayer2 = pDS->GetFtrLayerOfObject(HandleToFtr(h2));

		if(pLayer1==NULL || pLayer2==NULL )
			return false;

		if(pLayer1==pLayer2)
			return true;

		if(stricmp(pLayer1->GetName(),pLayer2->GetName())==0)
		{
			return true;
		}

		return false;
	}

	return false;
}

int CAutoLinkContourCommand::FindEqualObj(const CArray<FTR_HANDLE,FTR_HANDLE> &ref,FTR_HANDLE objid)
{
	if (objid!=0)
	{
		for (int i=0;i<ref.GetSize();i++)
		{
			
			if (objid==ref.GetAt(i))
			{
				return i;
			}
		}
	}
	return -1;	
}


//////////////////////////////////////////////////////////////////////
// CInterpolateCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CInterpolateCommand,CEditCommand)

CInterpolateCommand::CInterpolateCommand()
{
	m_nMode = wayEdis;
	m_nPart = modePart;
	//m_pObj = NULL;
	m_nInNum = 4;
	m_nFindRadius = 200;
	m_strBaseFCode = _T("");
	m_strSonFCode = _T("");
	m_bOnlyContour = TRUE;
	m_lft1.RemoveAll();
	m_lft2.RemoveAll();
	m_fLimit = 1;
	m_lfDis = 1.0;
	strcat(m_strRegPath,"\\Interpolate");
}

CInterpolateCommand::~CInterpolateCommand()
{

}



CString CInterpolateCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_INTERP);
}

void CInterpolateCommand::Start()
{
 	if( !m_pEditor )return;		
	
	m_nMode = wayEdis;
	m_nPart = modePart;
	m_arrPts.RemoveAll();
//	m_pObj = NULL;
	m_nInNum = 4;
	m_nFindRadius = 200;
	m_strBaseFCode = _T("");
	m_strSonFCode = _T("");
	m_bOnlyContour = TRUE;
	
//	m_pDoc->UpdateAllViews(NULL,hc_Attach_Accubox);
	
	m_nStep = 0;
	m_lft1.RemoveAll();
	m_lft2.RemoveAll();

	m_arrLine.RemoveAll();
	m_arrObjRatio1.RemoveAll();
	m_arrObjRatio2.RemoveAll();

	CCommand::Start();
	
	
	if( m_nPart==modeWhole )
		m_pEditor->OpenSelector(SELMODE_MULTI);
	else
		m_pEditor->CloseSelector();
	
	if( m_nPart==modeWhole )
		PromptString(StrFromResID(IDS_CMDTIP_INTOBJ1));
	else
		PromptString(StrFromResID(IDS_CMDTIP_INTLINE1));
}

void CInterpolateCommand::InitForFunction(int interp_num)
{
	m_nMode = wayEdis;
	m_nPart = modePart;
	m_arrPts.RemoveAll();

	m_nInNum = interp_num;
	m_nFindRadius = 200;
	m_strBaseFCode = _T("");
	m_strSonFCode = _T("");
	m_bOnlyContour = TRUE;
		
	m_nStep = 0;
	m_lft1.RemoveAll();
	m_lft2.RemoveAll();
	
	m_arrLine.RemoveAll();
	m_arrObjRatio1.RemoveAll();
	m_arrObjRatio2.RemoveAll();
}

void CInterpolateCommand::Abort()
{	
	m_arrPts.RemoveAll();
	m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

// line为线段 
template<class T>
double GetIntersectPos(T *pData1, int npt, T *line)
{
	int i, ok = 0;
	double pos, t, x, y;
	
	pos = -1;
	for( i=0; i<npt-1; i++)
	{
		if( GraphAPI::GGetLineIntersectLineSeg(pData1[i].x,pData1[i].y,
			pData1[i+1].x,pData1[i+1].y,line[0].x,line[0].y,
			line[1].x,line[1].y,&x,&y,&t) )
		{
			pos = i+t;
			ok = 1;
		}
	}
	
	if( npt>=2 && ok==0 )
	{
		double dis1 = -2, dis2 = -2;
		pos = 0;
		i = 0;
		if( GraphAPI::GGetLineIntersectLine(pData1[i].x,pData1[i].y,
			pData1[i+1].x,pData1[i+1].y,line[0].x,line[0].y,line[1].x,line[1].y,&x,&y,&t) )
		{
			dis1 = (pData1[i].x-x)*(pData1[i].x-x) + (pData1[i].y-y)*(pData1[i].y-y);
			ok++;
		}
		
		i = npt-2;
		if( GraphAPI::GGetLineIntersectLine(pData1[i].x,pData1[i].y,
			pData1[i+1].x,pData1[i+1].y,line[0].x,line[0].y,line[1].x,line[1].y,&x,&y,&t) )
		{
			i = npt-1;
			dis2 = (pData1[i].x-x)*(pData1[i].x-x) + (pData1[i].y-y)*(pData1[i].y-y);
			ok++;
		}
		
		if( ok==2 )
		{
			if( dis1>dis2 )
			{
				pos = npt-1;
			}
		}
	}
	
	return pos;
}

// line为线段
template<class T>
int IsSameSide(T *line, T *pt1, PT_3D *pt2)
{
	double k1, k2;
	k1 = (line[1].x-line[0].x)*(pt1->y-line[0].y) - (line[1].y-line[0].y)*(pt1->x-line[0].x);
	k2 = (line[1].x-line[0].x)*(pt2->y-line[0].y) - (line[1].y-line[0].y)*(pt2->x-line[0].x);
		
	if( (k1>0.0 && k2>0.0) || (k1<0.0 && k2<0.0) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

template<class T>
double GetPosLen(T *pts, int npt, double pos)
{
	if (!pts || npt < 2 || pos < 1e-6) return 0;

	double dis = 0;
	int key = (int)pos;
	for (int i=0; i<key && i<npt-1; i++)
	{
		dis = dis + sqrt( (pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x) +
			(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y) + 
				(pts[i].z-pts[i+1].z)*(pts[i].z-pts[i+1].z) );
	}

	if (key < npt-1)
	{
		dis += (pos-(int)pos)*sqrt( (pts[key].x-pts[key+1].x)*(pts[key].x-pts[key+1].x) +
			(pts[key].y-pts[key+1].y)*(pts[key].y-pts[key+1].y) + 
				(pts[key].z-pts[key+1].z)*(pts[key].z-pts[key+1].z) );
	}

	return dis;
}

template<class T>
void ReversePoints(T *pts, int npt)
{
	for (int i=0; i<npt/2; i++)
	{
		T pt = pts[i];
		pts[i] = pts[npt-1-i];
		pts[npt-1-i] = pt;
	}
}

template<class T>
int MergeElementOne(T *pData1, int npt1, T *pData2, int npt2, T* line, CArray<T,T> &pts)
{
	int i;
	double pos1, pos2, dis1, dis2, dis3, dis4;

	T *pt1=NULL, *pt2=NULL, *pt3=NULL, *pt4=NULL;

	if( npt1<2 || npt2<2 )return 0;

	pos1 = GetIntersectPos(pData1,npt1,line);
	pos2 = GetIntersectPos(pData2,npt2,line);

	dis1 = GetPosLen(pData1,npt1,pos1);
	dis2 = GetPosLen(pData1,npt1,npt1);

	dis3 = GetPosLen(pData2,npt2,pos2);
	dis4 = GetPosLen(pData2,npt2,npt2);

	// previous point
	if( pos1>=1.01 )
	{
		pt1 = pData1 + (int)pos1 - 1;
	}
	else if( pos1>0.01 )
	{
		pt1 = pData1;
	}
	// next point
	if( pos1<npt1-1.01 )
	{
		pt2 = pData1 + (int)pos1 + 1;
	}

	// previous point
	if( pos2>1.01 )
	{
		pt3 = pData2 + (int)pos2 - 1;
	}
	else if( pos2>0.01 )
	{
		pt3 = pData2;
	}
	// next point
	if( pos2<npt2-1.01 )
	{
		pt4 = pData2 + (int)pos2 + 1;
	}

	//调整点列的顺序，使元素pData1,pData2 在line上按照正向的顺序衔接
	if( pt1!=NULL )
	{
		if( pt3!=NULL )
		{
			if( IsSameSide(line,pt1,pt3) )
			{
			}
			else
			{
				ReversePoints(pData2,npt2);
				dis3 = dis4-dis3;
				pos2 = npt2-1-pos2;
			}
		}
		else if( pt4!=NULL )
		{
			if( IsSameSide(line,pt1,pt4) )
			{
				ReversePoints(pData2,npt2);
				dis3 = dis4-dis3;
				pos2 = npt2-1-pos2;

			}
		}
		else
		{
			//_printf("MergeElementOne error1!\n");
		}
	}
	else if( pt2!=NULL )
	{
		if( pt3!=NULL )
		{
			if( IsSameSide(line,pt2,pt3) )
			{
				ReversePoints(pData1,npt1);
				dis1 = dis2-dis1;
				pos1 = npt1-1-pos1;
			}
			else
			{
				ReversePoints(pData1,npt1);
				ReversePoints(pData2,npt2);

				dis1 = dis2-dis1;
				dis3 = dis4-dis3;
				pos1 = npt1-1-pos1;
				pos2 = npt2-1-pos2;
			}
		}
		else if( pt4!=NULL )
		{
			if( IsSameSide(line,pt2,pt4) )
			{
				ReversePoints(pData1,npt1);
				ReversePoints(pData2,npt2);

				dis1 = dis2-dis1;
				dis3 = dis4-dis3;
				pos1 = npt1-1-pos1;
				pos2 = npt2-1-pos2;
			}
			else
			{
				ReversePoints(pData1,npt1);

				dis1 = dis2-dis1;
				pos1 = npt1-1-pos1;
			}
		}
		else
		{
			//_printf("MergeElementOne error2!\n");
		}
	}
	else
	{
		//_printf("MergeElementOne error3!\n");
	}

	//如果这个衔接方式得到的线串长度不及丢弃的部分，我们就反向过来，得到更长的衔接的线串
	if( (dis1+dis4-dis3)<(dis3+dis2-dis1) )
	{
		ReversePoints(pData1,npt1);
		ReversePoints(pData2,npt2);

		pos1 = npt1-1-pos1;
		pos2 = npt2-1-pos2;

	}
	
	pts.RemoveAll();
	for( i=0; i<pos1; i++)
	{
		pts.Add(*(pData1+i));
	}

	for( i=(int)ceil(pos2); i<npt2; i++)
	{
		pts.Add(*(pData2+i));
	}

	//int npt = GraphAPI::GKickoffSamePoints(pts.GetData(),pts.GetSize());


	return 1;
}

void CInterpolateCommand::Back()
{
	int size=m_arrLine.GetSize();
	if(size>1)
	{	
		m_pEditor->Undo();
		m_arrLine.RemoveAt(size-1);
		
		m_ptLine[0]=(m_arrLine.ElementAt(size-2)).sp;
		
		m_ptLine[1]=(m_arrLine.ElementAt(size-2)).ep;
		
		//显示上次的红线
		if( 1 )
		{ 
			GrBuffer abuf;
			abuf.BeginLineString(RGB(255,0,0),0);
			abuf.MoveTo(&m_ptLine[0]);
			abuf.LineTo(&m_ptLine[1]);
			abuf.End();
			m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);				
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&abuf);			
		}
		m_pEditor->RefreshView();	
	}
	else
	{		
		m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
		m_pEditor->RefreshView();
		Start();
		return;
	}
	m_arrPts.RemoveAll();
}

void CInterpolateCommand::PtClick(PT_3D &pt, int flag)
{	
	//整体内插
	if( m_nPart==modeWhole )
	{
		const FTR_HANDLE *handles = NULL;
		int nsel = 0;
		//得到目标地物
		if( m_nStep==0 )
		{
			if(  !CanGetSelObjs(flag,TRUE) ||flag!=SELSTAT_DRAGSEL_RESTART )//拾取地物线
			{
				PDOC(m_pEditor)->GetSelection()->GetSelectedObjs(nsel);
				if( nsel>0 )
					GotoState(PROCSTATE_PROCESSING);
				
				if( Name()==StrFromResID(IDS_CMDNAME_INTERP)|| Name()==StrFromResID(IDS_CMDPLANE_INTERPOLATECENTERLINE))//cjc 2012年11月12日
				{
					if( nsel==1 )
						PromptString(StrFromResID(IDS_CMDTIP_INTOBJ2));
					else if( nsel>=2 )
						PromptString(StrFromResID(IDS_CMDTIP_SURE));
				}
				
				return;
			}

			handles = PDOC(m_pEditor)->GetSelection()->GetSelectedObjs(nsel);
			if( nsel<2 )return;			
			
			for(int i=nsel-1; i>=0; i--)
			{
				CGeometry *pObj = HandleToFtr(handles[i])->GetGeometry();
				if( !pObj )break;
				if( m_bOnlyContour && !CModifyZCommand::CheckObjForContour(pObj) )
					break;
			}			
			if( i>=0 )
			{
				PromptString(StrFromResID(IDS_CMDTIP_ONLYCONTOUR_ERR));//含有不等高地物，重新选择
				return;
			}
			
			EditStepOne();//步数+1
			
		}
		
		//开始内插
		if( m_nStep==1 )
		{	
			if (m_strSonFCode.IsEmpty())
			{
				Abort();
				return;
			}

			CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strSonFCode);
			if( !pLayer )
			{
				PromptString(StrFromResID(IDS_CMDTIP_LAYER_NOT_INVALID));
				Abort();
				return;
			}

			CFeature* pFtr= PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(m_strSonFCode);
			if( !pFtr )return;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (pGeo && (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve))) )
			{
				PromptString(StrFromResID(IDS_TIP_NOTSUPPORT_PARA));//内插线不支持平行线或双线
				Abort();
				return;
			}

			CUndoFtrs undo(m_pEditor,Name());	
			
			for (int i=0; i<nsel-1; i++)
			{
				CFeature *pFtr1 = HandleToFtr(handles[i]);
				CFeature *pFtr2 = HandleToFtr(handles[i+1]);

				if (!pFtr1 || !pFtr2)
				{
					continue;
				}
				
				CFeature *pTmp1 =pFtr1->Clone();
				CFeature *pTmp2 =pFtr2->Clone();
				if (pTmp1)
				{
					const CShapeLine *pLine1 = pTmp1->GetGeometry()->GetShape();//线型
					if (pLine1)
					{
						CArray<PT_3DEX,PT_3DEX> tmpArr;
						pLine1->GetPts(tmpArr);
						int code = penNone;
						for (int j=0; j<tmpArr.GetSize(); j++)
						{
							if (j == 0) code = tmpArr[j].pencode;
							else if (tmpArr[j].pencode == penNone)
							{
								tmpArr[j].pencode = code;
							}
						}
						pTmp1->GetGeometry()->CreateShape(tmpArr.GetData(),tmpArr.GetSize());
					}
				}
				if (pTmp2)
				{
					const CShapeLine *pLine2 = pTmp2->GetGeometry()->GetShape();
					if (pLine2)
					{
						CArray<PT_3DEX,PT_3DEX> tmpArr;
						pLine2->GetPts(tmpArr);
						int code = penNone;
						for (int j=0; j<tmpArr.GetSize(); j++)
						{
							if (j == 0) code = tmpArr[j].pencode;
							else if (tmpArr[j].pencode == penNone)
							{
								tmpArr[j].pencode = code;
							}
						}
						pTmp2->GetGeometry()->CreateShape(tmpArr.GetData(),tmpArr.GetSize());
					}
				}

				CPtrArray arr;
				if (m_nMode == wayNdis)
					InterpWholeObjByEquidis(pTmp1,pTmp2,pFtr,&arr);
				else if (m_nMode == wayEdis)
					InterpWholeObjByEquidis1(pTmp1, pTmp2, pFtr, &arr);
				else if (m_nMode == wayConstDis)
					InterpWholeObjByEquidis2(pTmp1, pTmp2, pFtr, &arr);
				if (pTmp1)
				{
					delete pTmp1;
				}
				if (pTmp2)
				{
					delete pTmp2;
				}
				
				if( arr.GetSize()>0 )
				{
					for( int i=0; i<arr.GetSize(); i++)
					{
						CFeature *pFtr = (CFeature*)arr[i];
						if (pFtr)
						{
							CArray<PT_3DEX,PT_3DEX> pts;
							pFtr->GetGeometry()->GetShape(pts);
							Compress(pts);
							pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
						}
						
						m_pEditor->AddObject((CFeature*)arr[i],pLayer->GetID());
						undo.AddNewFeature(FtrToHandle((CFeature*)arr[i]));	
					}
					
				}
			}

			delete pFtr;

			undo.Commit();	
			
			m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			Finish();
			m_nStep = 2;

			
			
		}
	}
	//部分内插
	if( m_nPart==modePart )
	{
		//分别在m_nStep=0和m_nStep=1时绘制两条线段
		if( m_nStep<=1 )
		{	
			
			m_arrPts.Add(pt);
			
			GotoState(PROCSTATE_PROCESSING);
			
			if( m_arrPts.GetSize()>=2 )//内插点数
			{
				GrBuffer vbuf;
				vbuf.BeginLineString(RGB(255,0,0),0);
				vbuf.MoveTo(&m_arrPts[0]);
				vbuf.LineTo(&m_arrPts[1]);
				vbuf.End();
				m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
				m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
				m_pEditor->UpdateDrag(ud_AddConstDrag,&vbuf);
				
				m_ptLine[m_nStep*2] = m_arrPts[0];
				m_ptLine[m_nStep*2+1] = m_arrPts[1];
				
				LINE_3D tem;
				tem.sp=m_ptLine[m_nStep*2];
				tem.ep=m_ptLine[m_nStep*2+1];
				m_arrLine.Add(tem);
				if( m_nStep==1 )
				{
					//清除显示			
					CFeature *pFtr = NULL;
					CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strSonFCode);
					if( !pLayer )
					{
						PromptString(StrFromResID(IDS_CMDTIP_LAYER_NOT_INVALID));
						Abort();
						return;
					}
					
					CFeature *pTestFtr = PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(m_strSonFCode);
					if (!pTestFtr)
					{
						PromptString(StrFromResID(IDS_CMDTIP_LAYER_NOT_INVALID));
						Abort();
						return;
					}

// 					CGeometry *pGeo = pTestFtr->GetGeometry();
// 					if (pGeo && (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve))) )
// 					{
// 						PromptString(StrFromResID(IDS_TIP_NOTSUPPORT_PARA));
// 						Abort();
// 						delete pTestFtr;
// 						return;
// 					}
					delete pTestFtr;
					
					//截取地物片断
					CPtrArray arrPart,arrRet;
					
					m_lft1.RemoveAll();
					m_lft2.RemoveAll();
					
					GetPartObjs(&arrPart);
					int num1=0;
					int num2=0;
					
					PT_3DEX ptLine[4];
					for (int i=0; i<4; i++)
					{
						COPY_3DPT(ptLine[i],m_ptLine[i]);
					}
					
					CUndoFtrs undo(m_pEditor,Name());
					
					//在匹配的相邻两个地物间内插
					for( i=0; i<arrPart.GetSize()-1; i++)
					{
						CFeature* pFtr= PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(m_strSonFCode);
						if( !pFtr )continue;				
						if (m_nMode==wayNdis)
						{
							InterpWholeObjByEquidis((CFeature*)arrPart[i],(CFeature*)arrPart[i+1],pFtr,&arrRet);		
						}
						else if (m_nMode == wayEdis)
						{
							InterpWholeObjByEquidis1((CFeature*)arrPart[i],(CFeature*)arrPart[i+1],pFtr,&arrRet);
						}
						else if (m_nMode == wayConstDis)
						{
							InterpWholeObjByEquidis2((CFeature*)arrPart[i], (CFeature*)arrPart[i + 1], pFtr, &arrRet);
						}
						
						// 支持非等高情况(包括两母线不等高和母线等高但高程相等的情况)
						BOOL bNoContourMode = FALSE;
						CGeometry *pGeo1 = ((CFeature*)arrPart[i])->GetGeometry();
						CGeometry *pGeo2 = ((CFeature*)arrPart[i+1])->GetGeometry();
						if (!CModifyZCommand::CheckObjForContour(pGeo1) || !CModifyZCommand::CheckObjForContour(pGeo2))
						{
							bNoContourMode = TRUE;
						}
						else if (pGeo1 && pGeo2)
						{
							PT_3DEX pt1, pt2;
							pt1 = pGeo1->GetDataPoint(0);
							pt2 = pGeo2->GetDataPoint(0);
							if (fabs(pt1.z-pt2.z) < 1e-4)
							{
								bNoContourMode = TRUE;
							}
						}
						
						delete pFtr;
						pFtr = NULL;
						
						if (arrRet.GetSize() <= 0)  continue;
						
						//粗略查找第一条线段的相交地物
						/*Envelope e1, e2,e3,e4;
						e1.CreateFromPts(m_ptLine,2);
						e3 = e1;
						e3.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
						m_pEditor->GetDataQuery()->FindObjectInRect(e3,m_pEditor->GetCoordWnd().m_pSearchCS);
						
						  int num1;
						  const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
						  if( !ppftr1  )
						  {	
						  return;
						  }
						  CArray<CPFeature,CPFeature> ftr1;
						  ftr1.SetSize(num1);
						  memcpy(ftr1.GetData(),ppftr1,num1*sizeof(*ppftr1));
						  
							
							  //粗略查找第二条线段的相交地物
							  e2.CreateFromPts(m_ptLine+2,2);
							  e4 = e2;
							  e4.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
							  m_pEditor->GetDataQuery()->FindObjectInRect(e4,m_pEditor->GetCoordWnd().m_pSearchCS);
							  
								int num2;
								const CPFeature * ppftr2 = m_pEditor->GetDataQuery()->GetFoundHandles(num2);
								if( !ppftr2  )
								{	
								return;
								}
								CArray<CPFeature,CPFeature> ftr2;
								ftr2.SetSize(num2);
						memcpy(ftr2.GetData(),ppftr2,num2*sizeof(*ppftr2));*/
						
						// 寻找两母线之间的地物
						double fTMin1, fTMax1;
						if (m_lft1[i] < m_lft1[i+1])
						{
							fTMin1 = m_lft1[i];
							fTMax1 = m_lft1[i+1];
						}
						else
						{
							fTMax1 = m_lft1[i];
							fTMin1 = m_lft1[i+1];
						}
						
						CArray<ObjRatio,ObjRatio> ftr1;
						for (int j=0; j<m_arrObjRatio1.GetSize(); j++)
						{
							ObjRatio ratio = m_arrObjRatio1[j];
							if (ratio.lfRatio > fTMin1 && ratio.lfRatio < fTMax1)
							{
								ftr1.Add(ratio);
							}
						}
						num1 = ftr1.GetSize();
						
						double fTMin2, fTMax2;
						if (m_lft2[i] < m_lft2[i+1])
						{
							fTMin2 = m_lft2[i];
							fTMax2 = m_lft2[i+1];
						}
						else
						{
							fTMax2 = m_lft2[i];
							fTMin2 = m_lft2[i+1];
						}
						
						CArray<ObjRatio,ObjRatio> ftr2;
						for ( j=0; j<m_arrObjRatio2.GetSize(); j++)
						{
							ObjRatio ratio = m_arrObjRatio2[j];
							if (ratio.lfRatio > fTMin2 && ratio.lfRatio < fTMax2)
							{
								ftr2.Add(ratio);
							}
						}
						num2 = ftr2.GetSize();
						
						for( j=0; j<arrRet.GetSize(); j++)
						{
							CGeometry *pObj=NULL;
							double z;//高程
							CArray<PT_3DEX,PT_3DEX> pts;
							((CFeature*)arrRet[j])->GetGeometry()->GetShape(pts);
							
							// 样条抽稀
							Compress(pts);
							
							PT_3DEX expt,expt1;
							expt = pts.GetAt(0);
							expt1 = pts[pts.GetSize()-1];
							z=expt.z;
							
							
							// 							double fMin = -1;
							// 							int iMin = -1;
							// 							int arrNum1;
							// 							PT_3D ptIntersect;
							
							// 内插线与第一条线段相交的地物连接
							for (int l=0;l<num1;l++)
							{
								pObj=(HandleToFtr(ftr1[l].ftr))->GetGeometry();
								if (!pObj) continue;
								
								CArray<PT_3DEX,PT_3DEX> pt1;
								
								const CShapeLine *pSL = pObj->GetShape();
								if( !pSL )continue;
								
								pSL->GetPts(pt1);
								
								expt = pt1[0];
								
								CFtrLayer *pLayer0= PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject((CFeature*)ftr1[l].ftr);
								if (!pLayer0)  continue;
								
								if (pLayer0==pLayer || strcmp(pLayer0->GetName(),pLayer->GetName())==0)
								{
									if ((!bNoContourMode&&fabs(z-expt.z)<GraphAPI::g_lfZTolerance))
									{
										// 										int arrNum1 = pt1.GetSize();
										// 										
										// 										if(!GetFirstPointOfIntersect(pt1.GetData(),arrNum1,m_ptLine[0],m_ptLine[1],&index,&t,&expt)) continue;
										// 										if(t>fTMin1&&t<fTMax1)
										{
											m_pEditor->DeleteObject(ftr1[l].ftr);
											undo.AddOldFeature(ftr1[l].ftr);
											
											pObj->GetShape(pt1);
											
											CArray<PT_3DEX,PT_3DEX> pts1;								
											MergeElementOne(pt1.GetData(),pt1.GetSize(),pts.GetData(),pts.GetSize(),ptLine,pts1);
											pts.Copy(pts1);
										}
									}
									else if (bNoContourMode)
									{
										if (j == l)
										{
											m_pEditor->DeleteObject(ftr1[l].ftr);
											undo.AddOldFeature(ftr1[l].ftr);
											
											pObj->GetShape(pt1);
											
											CArray<PT_3DEX,PT_3DEX> pts1;								
											MergeElementOne(pt1.GetData(),pt1.GetSize(),pts.GetData(),pts.GetSize(),ptLine,pts1);
											pts.Copy(pts1);
											break;
										}
									}
									
								}
							}
							
							//第二条线段
							for (l=0;l<num2;l++)
							{
								pObj = HandleToFtr(ftr2[l].ftr)->GetGeometry();
								if (!pObj) continue;
								
								CArray<PT_3DEX,PT_3DEX> pt2;
								
								const CShapeLine *pSL = pObj->GetShape();
								if( !pSL )continue;
								
								pSL->GetPts(pt2);
								
								expt = pt2[0];
								
								CFtrLayer *pLayer0= PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject((CFeature*)ftr2[l].ftr);
								if (!pLayer0)  continue;
								
								if (pLayer0==pLayer || strcmp(pLayer0->GetName(),pLayer->GetName())==0)
								{
									if (!bNoContourMode&&fabs(z-expt.z)<GraphAPI::g_lfZTolerance)
									{
										// 										int arrNum2 = pt2.GetSize();
										// 										
										// 										if(!GetFirstPointOfIntersect(pt2.GetData(),arrNum2,m_ptLine[2],m_ptLine[3],&index,&t,&expt)) continue;
										// 										if(t>fTMin2&&t<fTMax2)
										{
											m_pEditor->DeleteObject(ftr2[l].ftr);
											undo.AddOldFeature(ftr2[l].ftr);
											
											pObj->GetShape(pt2);
											
											CArray<PT_3DEX,PT_3DEX> pts1;
											MergeElementOne(pt2.GetData(),pt2.GetSize(),pts.GetData(),pts.GetSize(),ptLine+2,pts1);
											pts.Copy(pts1);
										}
									}
									else if (bNoContourMode)
									{
										if (j == l)
										{
											m_pEditor->DeleteObject(ftr2[l].ftr);
											undo.AddOldFeature(ftr2[l].ftr);
											
											pObj->GetShape(pt2);
											
											CArray<PT_3DEX,PT_3DEX> pts1;								
											MergeElementOne(pt2.GetData(),pt2.GetSize(),pts.GetData(),pts.GetSize(),ptLine+2,pts1);
											pts.Copy(pts1);
											break;
										}
									}
									
								}
							}
							
							((CFeature*)arrRet[j])->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
							
							
						}
						
						for( j=0; j<arrRet.GetSize(); j++)
						{
							m_pEditor->AddObject((CFeature*)arrRet[j],pLayer->GetID());
							undo.AddNewFeature(FtrToHandle((CFeature*)arrRet[j]));					
						}
						
						arrRet.RemoveAll();
					}
					
					for( i=0; i<arrPart.GetSize(); i++)
					{
						delete (CFeature*)arrPart[i];
					}
					
					undo.Commit();
					
					m_pEditor->RefreshView();
					m_ptLine[0]=m_ptLine[2];
					m_ptLine[1]=m_ptLine[3];
					
				}

				m_nStep= 1;//步数置1
				m_arrPts.RemoveAll();	
			}
		
			CCommand::PtClick(pt,flag);
			return;
		}
	}
	
	CEditCommand::PtClick(pt,flag);
}

void CInterpolateCommand::PtMove(PT_3D &pt)
{
	if(m_nPart == modePart)
	{			
		if(  IsProcFinished(this) || m_arrPts.GetSize()<=0 )return;
		
		if(m_arrPts.GetSize()==1)
		{
			m_arrPts.Add(pt);
			GrBuffer vbuf;
			vbuf.BeginLineString(0,0);
			vbuf.MoveTo(&m_arrPts[0]);
			vbuf.LineTo(&m_arrPts[1]);
			vbuf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			m_arrPts.RemoveAt(1);
			
		}
		CCommand::PtMove(pt);		
	}
}


void CInterpolateCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue(PF_INTERWAY,&CVariantEx(var));
	var = (bool)(m_bOnlyContour);
	tab.AddValue(PF_INTERONLYCONTOUR,&CVariantEx(var));
	var = (_bstr_t)(LPCSTR)(m_strSonFCode);
	tab.AddValue(PF_INTERSONCODE,&CVariantEx(var));
	var = (long)(m_nInNum);
	tab.AddValue(PF_INTERSONNUM,&CVariantEx(var));
	var = (long)(m_nPart);
	tab.AddValue(PF_INTERRANGE,&CVariantEx(var));
	var = (long)(m_nFindRadius);
	tab.AddValue(PF_INTERRADIUS,&CVariantEx(var));
	var = (_bstr_t)(LPCSTR)(m_strBaseFCode);
	tab.AddValue(PF_INTERBASECODE,&CVariantEx(var));
	var = (float)(m_fLimit);
	tab.AddValue(PF_INTERLIMIT,&CVariantEx(var));
	var = (double)(m_lfDis);
	tab.AddValue("lfDis", &CVariantEx(var));
}


void CInterpolateCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("InterpolateCommand",StrFromResID(IDS_CMDNAME_INTERP));
	
	param->BeginOptionParam(PF_INTERWAY,StrFromResID(IDS_CMDPLANE_WAY));
//	param->AddOption(StrFromResID(IDS_CMDPLANE_NEARESTDIS),wayNdis,' ',m_nMode==wayNdis);
	param->AddOption(StrFromResID(IDS_CMDPLANE_EDIS),wayEdis,' ',m_nMode==wayEdis);
	param->AddOption("固定距离内插", wayConstDis, ' ', m_nMode == wayConstDis);
	param->EndOptionParam();

	param->AddParam(PF_INTERONLYCONTOUR,bool(m_bOnlyContour),StrFromResID(IDS_CMDPLANE_ONLYCONTOUR));
 	
	param->AddLayerNameParam(PF_INTERSONCODE,LPCTSTR(m_strSonFCode),StrFromResID(IDS_CMDPLANE_SONCODE));
	if (m_nMode == wayEdis || bForLoad) param->AddParam(PF_INTERSONNUM, int(m_nInNum), StrFromResID(IDS_CMDPLANE_SONNUM));
	if (m_nMode == wayConstDis || bForLoad) param->AddParam("lfDis", m_lfDis, "内插间距");
	param->BeginOptionParam(PF_INTERRANGE,StrFromResID(IDS_CMDPLANE_RANGE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_WHOLE),modeWhole,' ',m_nPart==modeWhole);
	param->AddOption(StrFromResID(IDS_CMDPLANE_PART),modePart,' ',m_nPart==modePart);	
	param->EndOptionParam();
	if(m_nMode==wayNdis||bForLoad)param->AddParam(PF_INTERRADIUS,int(m_nFindRadius),StrFromResID(IDS_CMDPLANE_RADIUS));
	if(m_nPart==modePart||bForLoad)param->AddLayerNameParam(PF_INTERBASECODE,LPCTSTR(m_strBaseFCode),StrFromResID(IDS_CMDPLANE_BASECODE));

	param->AddParam(PF_INTERLIMIT,float(m_fLimit),StrFromResID(IDS_CMDPLANE_LIMIT));
}


void CInterpolateCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_INTERWAY,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERONLYCONTOUR,var) )
	{		
		m_bOnlyContour = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERSONCODE,var) )
	{
		m_strSonFCode = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERSONNUM,var) )
	{
		m_nInNum = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERRANGE,var) )
	{		
		m_nPart = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(m_nPart==modePart)
			m_pEditor->CloseSelector();
		else
			m_pEditor->OpenSelector(SELMODE_MULTI);

		if( m_nPart==modeWhole )
			PromptString(StrFromResID(IDS_CMDTIP_INTOBJ1));
		else
			PromptString(StrFromResID(IDS_CMDTIP_INTLINE1));

	}
	if( tab.GetValue(0,PF_INTERRADIUS,var) )
	{
		m_nFindRadius = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERBASECODE,var) )
	{
		m_strBaseFCode = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERLIMIT,var) )
	{
		m_fLimit = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "lfDis", var))
	{
		m_lfDis = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}



BOOL CInterpolateCommand::Compress(CArray<PT_3DEX,PT_3DEX> &arr)
{	
	int num = arr.GetSize();
	if( num<=2 )return FALSE;
	
	//判断是否为样条
	PT_3DEX expt;
	for( int j=1; j<num; j++)
	{
		expt = arr.GetAt(j);
		if( expt.pencode!=penSpline )
			break;
	}
	
	if( j<num )
	{
		return FALSE;
	}
	
	CArray<PT_3DEX,PT_3DEX> pts;
	
	//弦高压缩
	PT_3D pt3d;
	
	double fToler = fabs(m_fLimit);
	if (fToler < 1e-10)
	{
		fToler = 0.001;
	}
	
	double fLimit = (double)PDOC(m_pEditor)->GetDlgDataSource()->GetScale()/500/fToler;
	CStreamCompress2_PT_3D comp;
	comp.BeginCompress(fLimit);
	
	for( j=0; j<num; j++)
	{
		COPY_3DPT(pt3d,arr[j]);
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
			pts.Add(expt);
		}
	}
	
	//添加结果
	if( pts.GetSize()>=2 )
	{
		arr.Copy(pts);
		return TRUE;
	}
	
	return FALSE;

}

void CInterpolateCommand::GetPartObjs(CPtrArray *arr)
{
	//粗略查找第一条线段的相交地物
	Envelope e1, e2,e3,e4;
	e1.CreateFromPts(m_ptLine,2);
	e3 = e1;
	e3.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e3,m_pEditor->GetCoordWnd().m_pSearchCS);
	
	int num1;
	const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
	if( !ppftr1  )
	{	
		return;
	}
	CArray<CPFeature,CPFeature> ftr1;
	ftr1.SetSize(num1);
	memcpy(ftr1.GetData(),ppftr1,num1*sizeof(*ppftr1));
	
	
	//粗略查找第二条线段的相交地物
	e2.CreateFromPts(m_ptLine+2,2);
	e4 = e2;
	e4.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e4,m_pEditor->GetCoordWnd().m_pSearchCS);
	
	int num2;
	const CPFeature * ppftr2 = m_pEditor->GetDataQuery()->GetFoundHandles(num2);
	if( !ppftr2  )
	{	
		return;
	}
	CArray<CPFeature,CPFeature> ftr2;
	ftr2.SetSize(num2);
	memcpy(ftr2.GetData(),ppftr2,num2*sizeof(*ppftr2));
	

	BOOL bCheckBase = (m_strBaseFCode.GetLength()>0);
	BOOL bCheckSon = (m_strSonFCode.GetLength()>0);
	//获得有效的对象与两线段的相交信息，并排序
	CArray<CSmoothCommand::OBJ_ITEM,CSmoothCommand::OBJ_ITEM> arr1, arr2, arrSon1, arrSon2;
	CSmoothCommand::OBJ_ITEM item;
	CGeometry *pObj;
	const CShapeLine *pSL;
	PT_3DEX *pts;
	int size1, size2;
	for( int i=0; i<num1; i++)
	{
		pObj = ftr1[i]->GetGeometry();
		if( !pObj )continue;
		
		BOOL bBaseLine = FALSE, bSonLine = FALSE;
		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
		if( m_bOnlyContour && !CModifyZCommand::CheckObjForContour(pObj) )continue;
		if( !bCheckBase || CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr1[i],(LPCTSTR)m_strBaseFCode) )
			bBaseLine = TRUE;
		if( !bCheckSon || CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr1[i],(LPCTSTR)m_strSonFCode) )
			bSonLine = TRUE;

		if (!bBaseLine && !bSonLine) continue;

		if( pObj->GetDataPointSum()<2 )continue;
		
		//精细检验是否与第一线段相交，记录交点
		pSL = pObj->GetShape();
		if( !pSL )continue;
		
		PT_3D ret;
		double t=-1,st0=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k =0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e1.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if(/* pts[1].code==GRBUFFER_PTCODE_LINETO &&*/ e1.bIntersect(&pts[0],&pts[1]) )
					{
						//求相交点
						if (!GLineIntersectLineSeg1(m_ptLine[0].x,m_ptLine[0].y,m_ptLine[1].x,m_ptLine[1].y,pts[0].x,pts[0].y,pts[0].z,
							pts[1].x,pts[1].y,pts[1].z,&(ret.x),&(ret.y),&(ret.z),&t,&st0))
						{
							continue;
						}
// 						if( !CGrTrim::Intersect(m_ptLine,m_ptLine+1,&pts[0],&pts[1],&ret,&t) )
// 							continue;
						

						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						
						item.id = FtrToHandle(ftr1[i]);item.pos= pos+j; item.ret= ret; item.t = t;
						
						//按照 t 大小排序插入
						if (bBaseLine)
						{
							size1 = arr1.GetSize();
							for( int m=0; m<size1 && item.t>=arr1[m].t; m++);
							if( m<size1 )arr1.InsertAt(m,item);
							else arr1.Add(item);											
						
						}
						
						if (bSonLine)
						{
							size1 = arrSon1.GetSize();
							for( int m=0; m<size1 && item.t>=arrSon1[m].t; m++);
							if( m<size1 )arrSon1.InsertAt(m,item);
							else arrSon1.Add(item);
						}

						goto FINDOVER_1;
						
					}
				}
			}
			pos += pUnit->nuse;		
		}
FINDOVER_1:;
	}
	
	for( i=0; i<num2; i++)
	{
		pObj = ftr2[i]->GetGeometry();
		if( !pObj )continue;
		
		BOOL bBaseLine = FALSE, bSonLine = FALSE;

		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;

		if( m_bOnlyContour && !CModifyZCommand::CheckObjForContour(pObj) )continue;
		if( !bCheckBase || CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr2[i],(LPCTSTR)m_strBaseFCode) )
			bBaseLine = TRUE;

		if( !bCheckSon || CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr2[i],(LPCTSTR)m_strSonFCode) )
			bSonLine = TRUE;

		if (!bBaseLine && !bSonLine) continue;

		if( pObj->GetDataPointSum()<2 )continue;
		
		//精细检验是否与第二线段相交，记录交点
		pSL = pObj->GetShape();
		if( !pSL )continue;
		
		PT_3D ret;
		double t=-1,st0=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k=0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e2.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if( /*pts[1].code==GRBUFFER_PTCODE_LINETO && */e2.bIntersect(&pts[0],&pts[1]) )
					{
						//求相交点
						if (!GLineIntersectLineSeg1(m_ptLine[2].x,m_ptLine[2].y,m_ptLine[3].x,m_ptLine[3].y,pts[0].x,pts[0].y,pts[0].z,
							pts[1].x,pts[1].y,pts[1].z,&(ret.x),&(ret.y),&(ret.z),&t,&st0))
						{
							continue;
						}
// 						if( !CGrTrim::Intersect(m_ptLine+2,m_ptLine+3,&pts[0],&pts[1],&ret,&t) )
// 							continue;						

						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						
						item.id = FtrToHandle(ftr2[i]); item.pos= pos+j; item.ret= ret; item.t = t;
						
						//按照 t 大小排序插入
						if (bBaseLine)
						{
							size1 = arr2.GetSize();
							for( int m=0; m<size1 && item.t>=arr2[m].t; m++);
							if( m<size1 )arr2.InsertAt(m,item);
							else arr2.Add(item);
							
						}
						
						if (bSonLine)
						{
							size1 = arrSon2.GetSize();
							for( int m=0; m<size1 && item.t>=arrSon2[m].t; m++);
							if( m<size1 )arrSon2.InsertAt(m,item);
							else arrSon2.Add(item);
						}
						
						goto FINDOVER_2;
					}
				}
			}
			pos += pUnit->nuse;
		}
FINDOVER_2:;
	}
	CArray<CSmoothCommand::OBJ_PART,CSmoothCommand::OBJ_PART> arr3;
	CSmoothCommand::OBJ_PART part;
	//对排序好的两个地物组作匹配合并，结果保存在 arr 中
	
	// 提取相交的母线
	size1 = arr1.GetSize(); size2 = arr2.GetSize();
	for( i=0; i<size1; i++)
	{
		FTR_HANDLE id = arr1[i].id;
		for( int j=0; j<size2 && id!=arr2[j].id; j++);

		if( j<size2 )
		{
			part.id = id;
			part.pos1 = arr1[i].pos; part.pos2 = arr2[j].pos;
			part.pt1 = arr1[i].ret; part.pt2 = arr2[j].ret; 
			//t1,t2保存
			m_lft1.Add( arr1[i].t);
			m_lft2.Add( arr2[j].t);
			
			arr3.Add(part);			
		}
	}

	// 获取相交的内插线地物
	m_arrObjRatio1.RemoveAll();
	m_arrObjRatio2.RemoveAll();

	for (i=0; i<arrSon1.GetSize(); i++)
	{
		ObjRatio ratio;
		ratio.ftr = arrSon1[i].id;
		ratio.lfRatio = arrSon1[i].t;
		m_arrObjRatio1.Add(ratio);
	}

	for (i=0; i<arrSon2.GetSize(); i++)
	{
		ObjRatio ratio;
		ratio.ftr = arrSon2[i].id;
		ratio.lfRatio = arrSon2[i].t;
		m_arrObjRatio2.Add(ratio);
	}
	
	//保存两个点，用于后面的判断方向 by shy
	if(arr3.GetSize()>0)
	{
		m_pt[0]=(arr3.GetAt(0)).pt1;
		m_pt[1]=(arr3.GetAt(0)).pt2;
	}
	
	//生成新的母线地物
	size1 = arr3.GetSize();
	PT_3DEX expt;
	for( i=0; i<size1; i++)
	{
		part = arr3[i];
		pObj = HandleToFtr(part.id)->GetGeometry();
		const CShapeLine *pSL = pObj->GetShape();
		CArray<PT_3DEX,PT_3DEX> arr0;
		pObj->GetShape(arr0);
		
		
		int code;
		for( int j=arr0.GetSize()-1; j>=0; j--)
		{
			expt = arr0[j];
			if( j==0 )code = expt.pencode;
			else if( code!=expt.pencode )code = penLine;
			arr0.RemoveAt(j);
		}
		
		expt.pencode = code;		
		COPY_3DPT(expt,part.pt1);
		arr0.Add(expt);
		CPtrArray arrUnit;
		if(!pSL->GetShapeLineUnit(arrUnit))return;
			
		CShapeLine::ShapeLineUnit *pUnit = NULL;
		int pos = 0, end;
		BOOL bcopyover = (part.pos1==part.pos2);
		for(int k = 0;k<arrUnit.GetSize()&&!bcopyover;k++ )
		{
			pUnit = (CShapeLine::ShapeLineUnit *)arrUnit[k];
			pts = pUnit->pts;
			if( part.pos1<part.pos2 )
			{
				if( part.pos1<pos+pUnit->nuse )
				{
					end = ((part.pos2<pos+pUnit->nuse)?(part.pos2-pos+1):pUnit->nuse);
					for( int j=part.pos1-pos+1; j<end; j++)
					{
						COPY_3DPT(expt,(pts[j]));
						arr0.Add(expt);
					}
					part.pos1 = pos+j;
				}
				
				if( part.pos1>=part.pos2 )bcopyover = TRUE;
			}
			else
			{
				if( part.pos2<pos+pUnit->nuse )
				{
					end = ((part.pos1<pos+pUnit->nuse)?(part.pos1-pos+1):pUnit->nuse);
					for( int j=part.pos2-pos+1; j<end; j++)
					{
						COPY_3DPT(expt,(pts[j]));
						arr0.InsertAt(1,expt);
					}
					part.pos2 = pos+j;
				}
				
				if( part.pos2>=part.pos1 )bcopyover = TRUE;
			}
			
			pos += pUnit->nuse;
			pUnit = pUnit->next;
		}
		
		part = arr3[i];
		COPY_3DPT(expt,part.pt2);
		arr0.Add(expt);

		CFeature *pFtr = HandleToFtr(part.id)->Clone();
		if(!pFtr) return;
		pFtr->GetGeometry()->CreateShape(arr0.GetData(),arr0.GetSize());
		arr->Add(pFtr);
	}
	
}


extern int InterpolateContour(PT_3D * ptArray1,int sum1, PT_3D * ptArray2,int sum2,
					   PT_3D * ptArray , int* pSum, int nIntNum,int nPart);
extern int InterpolateContour1(PT_3D * ptArray1, int sum1, PT_3D * ptArray2, int sum2,
	PT_3D * ptArray, int* pSum, double lfDis, int nPart);
extern  int Interpolate1(PT_3D * ptArray1,int sum1, PT_3D * ptArray2,int sum2,
						 PT_3D * ptArray ,int* pSum, int nScale, int scanRange, int optimize,float tolerance);

void CInterpolateCommand::InterpWholeObjByEquidis2(CFeature *pFtr1, CFeature *pFtr2, CFeature *pTempl, CPtrArray *pArr)
{
	if (pFtr1 == NULL || pFtr2 == NULL || pTempl == NULL || pArr == NULL)
	{
		return;
	}

	CGeometry *pObj1 = pFtr1->GetGeometry();
	CGeometry *pObj2 = pFtr2->GetGeometry();
	int num1 = pObj1->GetDataPointSum(), num2 = pObj2->GetDataPointSum(), i, *pnum = NULL;
	if (num1 < 2 || num2 < 2)return;

	//计算插入条数
	{
		m_nInNum = 0;
		double z1 = pObj1->GetDataPoint(0).z;
		double z2 = pObj2->GetDataPoint(0).z;
		double z3 = min(z1, z2);
		double z4 = max(z1, z2);

		double curZ = z3 + m_lfDis;
		int curZ1 = (int)(curZ * 2 + 0.25);//四舍五入
		curZ = curZ1 / 2.0;
		while (curZ < z4)
		{
			m_nInNum++;
			curZ += m_lfDis;
		}
	}

	int Rt;
	PT_3D *pts1 = new PT_3D[num1];
	PT_3D *pts2 = new PT_3D[num2];
	PT_3D *pts3 = NULL, *t;
	PT_3DEX expt;
	CTempTolerance tempTol(1e-6);
	int num = m_nInNum*((num1 + num2) / 2);//m_nInNum 内插条数
	CArray<PT_3DEX, PT_3DEX> arrPts;

	pnum = (int*)malloc(sizeof(int)*m_nInNum);
	if (!pts1 || !pts2 || !pnum)goto EXIT_ERR;

	pObj1->GetShape(arrPts);
	for (i = 0; i < num1; i++)
	{
		expt = arrPts.GetAt(i);
		COPY_3DPT((pts1[i]), expt);
	}
	num1 = GraphAPI::GKickoffSamePoints(pts1, num1);
	arrPts.RemoveAll();
	pObj2->GetShape(arrPts);
	for (i = 0; i < num2; i++)
	{
		expt = arrPts.GetAt(i);
		COPY_3DPT((pts2[i]), expt);
	}
	num2 = GraphAPI::GKickoffSamePoints(pts2, num2);

	if (num <= 1)goto EXIT_ERR;
	pts3 = (PT_3D*)malloc(sizeof(PT_3D)*num);
	if (!pts3)goto EXIT_ERR;

	Rt = InterpolateContour1(pts1, num1, pts2, num2, pts3, pnum, m_lfDis, m_nPart);

	if (Rt != 0)
	{
		goto EXIT_ERR;
	}

	t = pts3;

	for (i = 0; i < m_nInNum; i++)
	{
		//保持流线线型
		expt = pObj1->GetDataPoint(1);
		if (expt.pencode != penStream && expt.pencode != penSpline)expt.pencode = penLine;

		CFeature  *pFtr = pTempl->Clone();
		if (pFtr)
		{
			pFtr->SetID(OUID());
			CArray<PT_3DEX, PT_3DEX> arr;
			for (int j = 0; j < pnum[i]; j++)
			{
				COPY_3DPT(expt, (t[j]));
				arr.Add(expt);
			}
			if (arr.GetSize()>1)
			{
				if (pFtr->GetGeometry()->CreateShape(arr.GetData(), arr.GetSize()))
					pArr->Add(pFtr);
			}
			else delete pFtr;
		}
		t += pnum[i];
	}

	if (pts3)free(pts3);

EXIT_ERR:
	if (pts1)delete[] pts1;
	if (pts2)delete[] pts2;
	if (pnum)free(pnum);

}

void CInterpolateCommand::InterpWholeObjByEquidis1(CFeature *pFtr1, CFeature *pFtr2, CFeature *pTempl, CPtrArray *pArr)
{	
	if (pFtr1==NULL||pFtr2==NULL||pTempl==NULL||pArr==NULL)
	{
		return;
	}

	CGeometry *pObj1 = pFtr1->GetGeometry();
	CGeometry *pObj2 = pFtr2->GetGeometry();
	int num1 = pObj1->GetDataPointSum(), num2 = pObj2->GetDataPointSum(), i, *pnum = NULL;
	if( num1<2 || num2<2 )return;
	
	int Rt;
	PT_3D *pts1 = new PT_3D[num1];
	PT_3D *pts2 = new PT_3D[num2];
	PT_3D *pts3 = NULL, *t;
	PT_3DEX expt;
	CTempTolerance tempTol(1e-6);
	int num=m_nInNum*((num1+num2)/2);//m_nInNum 内插条数
	CArray<PT_3DEX,PT_3DEX> arrPts;

	pnum = (int*)malloc(sizeof(int)*m_nInNum);
	if( !pts1||!pts2||!pnum )goto EXIT_ERR;
	
	pObj1->GetShape(arrPts);
	for( i=0; i<num1; i++)
	{
		expt = arrPts.GetAt(i);
		COPY_3DPT((pts1[i]),expt);
	}	
	num1=GraphAPI::GKickoffSamePoints(pts1,num1);
	arrPts.RemoveAll();
	pObj2->GetShape(arrPts);
	for( i=0; i<num2; i++)
	{
		expt = arrPts.GetAt(i);
		COPY_3DPT((pts2[i]),expt);
	}
	num2 = GraphAPI::GKickoffSamePoints(pts2,num2);

	if( num<=1 )goto EXIT_ERR;
	pts3 = (PT_3D*)malloc(sizeof(PT_3D)*num);
	if( !pts3 )goto EXIT_ERR;

	Rt = InterpolateContour(pts1,num1,pts2,num2,pts3,pnum,m_nInNum,m_nPart);

	if (Rt!=0)
	{
		goto EXIT_ERR;
	}
	
	t = pts3;

	for( i=0; i<m_nInNum; i++)
	{
		//保持流线线型
		expt = pObj1->GetDataPoint(1);
		if( expt.pencode!=penStream && expt.pencode!=penSpline )expt.pencode = penLine;
		
		CFeature  *pFtr = pTempl->Clone();
		if( pFtr )
		{
			pFtr->SetID(OUID());
			CArray<PT_3DEX,PT_3DEX> arr;
			for( int j=0; j<pnum[i]; j++)
			{
				COPY_3DPT(expt,(t[j]));
				arr.Add(expt);
			}
			if( arr.GetSize()>1 )
			{
				if(pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize()))				
				    pArr->Add(pFtr);
			}
			else delete pFtr;
		}
		t += pnum[i];
	}

	if( pts3 )free(pts3);
	
EXIT_ERR:
	if( pts1 )delete[] pts1;
	if( pts2 )delete[] pts2;
	if( pnum )free(pnum);
	
}

void CInterpolateCommand::InterpWholeObjByEquidis(CFeature *pFtr1, CFeature *pFtr2, CFeature *pTempl, CPtrArray *pArr)
{
	if (pFtr1==NULL||pFtr2==NULL||pTempl==NULL||pArr==NULL)
	{
		return;
	}
	CGeometry *pObj1 = pFtr1->GetGeometry();
	CGeometry *pObj2 = pFtr2->GetGeometry();
	int num1 = pObj1->GetDataPointSum(), num2 = pObj2->GetDataPointSum(), i, *pnum = NULL;
	if( num1<2 || num2<2 )return;
	int Rt;
	PT_3D *pts1 = new PT_3D[num1];
	PT_3D *pts2 = new PT_3D[num2];
	PT_3D *pts3 = NULL, *t;
	PT_3DEX expt;
	CTempTolerance tempTol(1e-6);
	int num=m_nInNum*((num1+num2)/2);//m_nInNum 内插条数
	CArray<PT_3DEX,PT_3DEX> arrPts;	
	pnum = (int*)malloc(sizeof(int)*m_nInNum);
	if( !pts1||!pts2||!pnum )goto EXIT_ERR;
	pObj1->GetShape(arrPts);
	for( i=0; i<num1; i++)
	{
		expt = arrPts.GetAt(i);
		COPY_3DPT((pts1[i]),expt);
	}	
	num1=GraphAPI::GKickoffSamePoints(pts1,num1);
	arrPts.RemoveAll();
	pObj2->GetShape(arrPts);
	for( i=0; i<num2; i++)
	{
		expt = arrPts.GetAt(i);
		COPY_3DPT((pts2[i]),expt);
	}
	num2 = GraphAPI::GKickoffSamePoints(pts2,num2);
	if( num<=1 )goto EXIT_ERR;
	pts3 = (PT_3D*)malloc(sizeof(PT_3D)*num);
	if( !pts3 )goto EXIT_ERR;	
	if( Interpolate1(pts1,num1,pts2,num2,NULL,pnum,m_nInNum,m_nFindRadius,0,0)==INTPAPI_SUCESS )
	{
		if( pnum[0]<=1 )goto EXIT_ERR;
		pts3 = (PT_3D*)malloc(sizeof(PT_3D)*pnum[0]);
		if( !pts3 )goto EXIT_ERR;
		Interpolate1(pts1,num1,pts2,num2,pts3,pnum,m_nInNum,m_nFindRadius,1,0);
		t = pts3;
		for( i=0; i<m_nInNum; i++)
		{
			expt = pObj1->GetDataPoint(1);
			if( expt.pencode!=penStream && expt.pencode!=penSpline )expt.pencode = penLine;
			CFeature  *pFtr = pTempl->Clone();
			if( pFtr )
			{
				pFtr->SetID(OUID());
				CArray<PT_3DEX,PT_3DEX> arr;
				for( int j=0; j<pnum[i]; j++)
				{
					COPY_3DPT(expt,(t[j]));
					arr.Add(expt);
				}
				if( arr.GetSize()>1 )
				{
					if(pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize()))				
					{
						pArr->Add(pFtr);
					}
				}
				else delete pFtr;
			}
			t += pnum[i];
		}
		if( pts3 )free(pts3);
	}
EXIT_ERR:
	if( pts1 )delete[] pts1;
	if( pts2 )delete[] pts2;
	if( pnum )free(pnum);			
}

bool CInterpolateCommand::GetFirstPointOfIntersect(PT_3DEX *pts, int num, PT_3D pt1, PT_3D pt2, int *index,double *rett,PT_3D *ret)
{
	int i,index1;
	double mint=2,t,st0;
	PT_3D tem,ret1;
	for (i=0;i<num-1;i++)
	{
		if(GLineIntersectLineSeg1(pt1.x,pt1.y,pt2.x,pt2.y,pts[i].x,pts[i].y,pts[i].z,pts[i+1].x,pts[i+1].y,pts[i+1].z,&(tem.x),&(tem.y),&(tem.z),&t,&st0))
		{
			if (t<mint)
			{
				if (st0<0.5)
				{
					if(sqrt((pts[i].x-tem.x)*(pts[i].x-tem.x)+(pts[i].y-tem.y)*(pts[i].y-tem.y))<1e-10)
					{
						if (i!=0 && (GraphAPI::GGetMultiply(PT_3DEX(pt1,penLine),PT_3DEX(pt2,penLine),pts[i-1])*GraphAPI::GGetMultiply(PT_3DEX(pt1,penLine),PT_3DEX(pt2,penLine),pts[i+1])<0))
						{
							index1=i;
							ret1=pts[i];
							mint=t;
							continue;
						}
// 						else
// 							continue;
					}
				}
				else
				{
					if(sqrt((pts[i+1].x-tem.x)*(pts[i+1].x-tem.x)+(pts[i+1].y-tem.y)*(pts[i+1].y-tem.y))<1e-10)
					{
						if (i+2!=num && (GraphAPI::GGetMultiply(PT_3DEX(pt1,penLine),PT_3DEX(pt2,penLine),pts[i])*GraphAPI::GGetMultiply(PT_3DEX(pt1,penLine),PT_3DEX(pt2,penLine),pts[i+2])<0))
						{	
							index1=i;
							ret1=pts[i+1];
							mint=t;
							continue;
						}
// 						else
// 							continue;
					}					
				}
				index1=i;
				mint=t;
				ret1=tem;
			}			
		}
	}
	if (mint==2)
	{
		return false;
	}
	else
	{
		*index=index1;
		*rett=mint;
		*ret=ret1;
		return true;		
	}	
}


//////////////////////////////////////////////////////////////////////
// CContinueInterpolateCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CContinueInterpolateCommand,CInterpolateCommand)

CContinueInterpolateCommand::CContinueInterpolateCommand()
{

	m_arrInt.RemoveAll();
	m_arrLine.RemoveAll();
	m_arrAddObjs.RemoveAll();
	m_arrDelObjs.RemoveAll();

	strcat(m_strRegPath,"\\ContinueInterpolate");
}
CContinueInterpolateCommand::~CContinueInterpolateCommand()
{

}

void CContinueInterpolateCommand::Back()
{
	int size=m_arrLine.GetSize();
	if(size>1)
	{	
		m_pEditor->Undo();
		m_arrLine.RemoveAt(size-1);
		
		m_ptLine[0]=(m_arrLine.ElementAt(size-2)).sp;
		
		m_ptLine[1]=(m_arrLine.ElementAt(size-2)).ep;
	
		//显示上次的红线
		if( 1 )
		{ 
			GrBuffer abuf;
			abuf.BeginLineString(RGB(255,0,0),0);
			abuf.MoveTo(&m_ptLine[0]);
			abuf.LineTo(&m_ptLine[1]);
			abuf.End();
			m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);				
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&abuf);			
		}
	
		int num=m_arrInt.GetAt(m_arrInt.GetUpperBound());
		m_arrInt.RemoveAt(m_arrInt.GetUpperBound());
		for (int l=0;l<num;l++)
		{
			//m_pEditor->DeleteObject(m_arrAddObjs.GetAt(m_arrAddObjs.GetUpperBound()));
			m_arrAddObjs.RemoveAt(m_arrAddObjs.GetUpperBound());
			m_arrUndoobjs.RemoveAt(m_arrUndoobjs.GetUpperBound());
		}
		num=m_arrInt.GetAt(m_arrInt.GetUpperBound());
		m_arrInt.RemoveAt(m_arrInt.GetUpperBound());
		for (int m=0;m<num;m++)
		{
			m_arrUndoobjs.Add(m_arrDelObjs.GetAt(m_arrDelObjs.GetUpperBound()));
			//m_pEditor->RestoreObject(m_arrDelObjs.GetAt(m_arrDelObjs.GetUpperBound()));
			m_arrDelObjs.RemoveAt(m_arrDelObjs.GetUpperBound());
		}
		m_pEditor->RefreshView();	
	}
	else
	{		
		m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
		m_pEditor->RefreshView();
		Start();
		return;
	}
	m_arrPts.RemoveAll();
}

CString CContinueInterpolateCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CONTINUEINTERP);
}

void CContinueInterpolateCommand::Start()
{
	if( !m_pEditor )return;
	
	m_nMode = 0;

//	m_pObj = NULL;
	m_nInNum = 4;
	m_nFindRadius = 20;
	m_strBaseFCode = _T("");
	m_strSonFCode = _T("");
	m_bOnlyContour = TRUE;
	m_arrLine.RemoveAll();
	m_arrAddObjs.RemoveAll();
//	m_pDoc->UpdateAllViews(NULL,hc_Attach_Accubox);
	
	m_nStep = 0;
	
	CCommand::Start();

	m_pEditor->CloseSelector();
}


void CContinueInterpolateCommand::PtMove(PT_3D &pt)
{
	if( IsProcFinished(this) || m_arrPts.GetSize()<=0 )return;
	
	m_arrPts.Add(pt);
	GrBuffer vbuf;
	vbuf.BeginLineString(0,0);
	vbuf.MoveTo(&m_arrPts[0]);
	vbuf.LineTo(&m_arrPts[1]);
	vbuf.End();
	m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
	m_arrPts.RemoveAt(1);
	CCommand::PtMove(pt);

}

int CContinueInterpolateCommand::FindIndexInArray(FTR_HANDLE handle,const CArray<FTR_HANDLE,FTR_HANDLE> &arrUndoobjs)
{
	for (int i=0;i<arrUndoobjs.GetSize();i++)
	{
		if (handle==arrUndoobjs[i])
		{
			return i;
		}
	}
	return -1;
}

void CContinueInterpolateCommand::PtClick(PT_3D &pt, int flag)
{
	//分别在m_nStep=0和m_nStep=1时绘制两条线段
	if( m_nStep<=1 )
	{
		m_arrPts.Add(pt);
		
		GotoState(PROCSTATE_PROCESSING);
		
		if( m_arrPts.GetSize()>=2 )
		{
			GrBuffer vbuf;
			vbuf.BeginLineString(RGB(255,0,0),0);
			vbuf.MoveTo(&m_arrPts[0]);
			vbuf.LineTo(&m_arrPts[1]);
			vbuf.End();
			m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&vbuf);
			
			m_ptLine[m_nStep*2] = m_arrPts[0];
			m_ptLine[m_nStep*2+1] = m_arrPts[1];

			LINE_3D tem;
			tem.sp=m_ptLine[m_nStep*2];
			tem.ep=m_ptLine[m_nStep*2+1];
			m_arrLine.Add(tem);
			if( m_nStep==1 )
			{
				//清除显示			
				CFeature *pFtr = NULL;
				CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strSonFCode);
				if( !pLayer )
				{
					PromptString(StrFromResID(IDS_CMDTIP_LAYER_NOT_INVALID));
					Abort();
					return;
				}
				
				CFeature *pTestFtr = PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(m_strSonFCode);
				CGeometry *pGeo = pTestFtr->GetGeometry();
				if (pGeo && (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve))) )
				{
					PromptString(StrFromResID(IDS_TIP_NOTSUPPORT_PARA));
					Abort();
					delete pTestFtr;
					return;
				}
				delete pTestFtr;

				//截取地物片断
				CPtrArray arrPart,arrRet;
				GetPartObjs(&arrPart);
				int num1=0;
				int num2=0;
				int tem;
				CArray<FTR_HANDLE,FTR_HANDLE> arrTmp;
				arrTmp.Copy(m_arrAddObjs);

				CUndoFtrs undo(m_pEditor,Name());

				//在匹配的相邻两个地物间内插
				for( int i=0; i<arrPart.GetSize()-1; i++)
				{
					CFeature* pFtr= PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(m_strSonFCode);
					if( !pFtr )continue;				
					if (m_nMode==wayNdis)
					{
					}
					else
					{
						InterpWholeObjByEquidis1((CFeature*)arrPart[i],(CFeature*)arrPart[i+1],pFtr,&arrRet);
					}
					
					delete pFtr;
					pFtr = NULL;

					for( int j=0; j<arrRet.GetSize(); j++)
					{
						bool flag=false;
						double z=0;
						PT_3DEX ptex;
						
						if (m_arrInt.GetSize()>0)
						{
							for (int a=0;a<m_arrInt.GetAt(m_arrInt.GetUpperBound());a++)
							{
								FTR_HANDLE objhandle = arrTmp.GetAt(arrTmp.GetUpperBound()-a);
								
								if( !objhandle )continue;

								CArray<PT_3DEX,PT_3DEX> arr,arr0;
								((CFeature*)arrRet[j])->GetGeometry()->GetShape(arr);
								HandleToFtr(objhandle)->GetGeometry()->GetShape(arr0);
								
								int objnum , retnum;
								objnum = arr0.GetSize();
								retnum = arr.GetSize();
								
								if (fabs(arr0[objnum-1].x-arr[0].x) + fabs(arr0[objnum-1].y-arr[0].y) <= 1e-4)
								{
									pFtr = HandleToFtr(objhandle)->Clone();
									flag = true;
									
									// 连接点的高程为两点高程平均值
									arr0[objnum-1].z = (arr0[objnum-1].z+arr[0].z)/2;

									if(retnum>0)arr.RemoveAt(0);
									arr0.Append(arr);
									pFtr->GetGeometry()->CreateShape(arr0.GetData(),arr0.GetSize());

									undo.AddOldFeature(objhandle);

									m_pEditor->DeleteObject(objhandle);
									m_arrDelObjs.Add(objhandle);
									if ((tem=FindIndexInArray(objhandle,m_arrUndoobjs))!=-1)
									{
										m_arrUndoobjs.RemoveAt(tem);
									}
									num1++;
									pFtr->SetID(OUID());
									m_pEditor->AddObject(pFtr,pLayer->GetID());
									m_arrUndoobjs.Add(FtrToHandle(pFtr));								
									m_arrAddObjs.Add(FtrToHandle(pFtr));

									undo.AddNewFeature(FtrToHandle(pFtr));

									break;
								}
								
							}
							
						}

						if (flag == false)
						{
							m_pEditor->AddObject((CFeature*)arrRet[j],pLayer->GetID());
							m_arrAddObjs.Add(FtrToHandle((CFeature*)arrRet[j]));
							m_arrUndoobjs.Add(FtrToHandle((CFeature*)arrRet[j]));

							undo.AddNewFeature(FTR_HANDLE(arrRet[j]));
						}
						 
						
					}
					num2+=arrRet.GetSize();
					arrRet.RemoveAll();
				}
				m_arrInt.Add(num1);
				m_arrInt.Add(num2);
				for( i=0; i<arrPart.GetSize(); i++)
				{
					delete (CFeature*)arrPart[i];
				}
				
				undo.Commit();

				m_pEditor->RefreshView();
				m_ptLine[0]=m_ptLine[2];
				m_ptLine[1]=m_ptLine[3];

			}
			m_nStep= 1;
			m_arrPts.RemoveAll();	
		}	
	}	
	CCommand::PtClick(pt,flag);
	
}
void CContinueInterpolateCommand::Abort()
{
	if (m_arrAddObjs.GetSize()>0)
	{
// 		for(int k=0;k<m_arrAddObjs.GetSize();k++)
// 			m_pEditor->DeleteObject(m_arrAddObjs.GetAt(k));
		m_arrAddObjs.RemoveAll();

	}
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
//	m_pEditor->UpdateDrag(ud_Detach_Accubox);
	m_pEditor->RefreshView();	
	CCommand::Abort();

}

void CContinueInterpolateCommand::Finish()
{
	m_nStep = -1;
	CEditCommand::Finish();
}


void CContinueInterpolateCommand::GetPartObjs(CPtrArray *arr)
{
	Envelope e1, e2,e3,e4;
	e1.CreateFromPts(m_ptLine,2);
	e3 = e1;
	e3.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e3,m_pEditor->GetCoordWnd().m_pSearchCS);
	int num1;
	const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
	if( !ppftr1  )
	{	
		return;
	}	
	CArray<CPFeature,CPFeature> ftr1;
	for (int i=0;i<num1;i++)
	{
		for(int j=0;j<m_arrUndoobjs.GetSize();j++)
		{
			if (m_arrUndoobjs[j]==FtrToHandle(ppftr1[i]))
			{
				break;
			}				
		}
		if (j>=m_arrUndoobjs.GetSize())
		{
			ftr1.Add(ppftr1[i]);
		}
	}
	num1 = ftr1.GetSize();
	e2.CreateFromPts(m_ptLine+2,2);
	e4 = e2;
	e4.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e4,m_pEditor->GetCoordWnd().m_pSearchCS);
	int num2;
	const CPFeature * ppftr2 = m_pEditor->GetDataQuery()->GetFoundHandles(num2);
	if( !ppftr2  )
	{	
		return;
	}
	CArray<CPFeature,CPFeature> ftr2;
	for ( i=0;i<num2;i++)
	{
		for(int j=0;j<m_arrUndoobjs.GetSize();j++)
		{
			if (m_arrUndoobjs[j]==FtrToHandle(ppftr2[i]))
			{
				break;
			}				
		}
		if (j>=m_arrUndoobjs.GetSize())
		{
			ftr2.Add(ppftr2[i]);
		}
	}
	num2 = ftr2.GetSize();
	BOOL bCheckBase = (m_strBaseFCode.GetLength()>0);
	CArray<CSmoothCommand::OBJ_ITEM,CSmoothCommand::OBJ_ITEM> arr1, arr2;
	CSmoothCommand::OBJ_ITEM item;
	CGeometry *pObj;
	const CShapeLine *pSL;
	PT_3DEX *pts;
	int size1, size2;
	for(  i=0; i<num1; i++)
	{
		pObj = ftr1[i]->GetGeometry();
		if( !pObj )continue;
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
		if( m_bOnlyContour && !CModifyZCommand::CheckObjForContour(pObj) )continue;
		if( bCheckBase && !CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr1[i],(LPCTSTR)m_strBaseFCode) )continue;
		if( pObj->GetDataPointSum()<2 )continue;
		pSL = pObj->GetShape();
		if( !pSL )continue;
		PT_3D ret;
		double t=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k =0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e1.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if(/* pts[1].code==GRBUFFER_PTCODE_LINETO &&*/ e1.bIntersect(&pts[0],&pts[1]) )
					{
						if( !CGrTrim::Intersect(m_ptLine,m_ptLine+1,&pts[0],&pts[1],&ret,&t) )
							continue;
						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						item.id = FtrToHandle(ftr1[i]);item.pos= pos+j; item.ret= ret; item.t = t;
						size1 = arr1.GetSize();
						for( int m=0; m<size1 && item.t>=arr1[m].t; m++);
						if( m<size1 )arr1.InsertAt(m,item);
						else arr1.Add(item);
						goto FINDOVER_1;
					}
				}
			}
			pos += pUnit->nuse;		
		}
FINDOVER_1:;
	}
	for( i=0; i<num2; i++)
	{
		pObj = ftr2[i]->GetGeometry();
		if( !pObj )continue;
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
		if( m_bOnlyContour && !CModifyZCommand::CheckObjForContour(pObj) )continue;
		if( bCheckBase && !CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr2[i],(LPCTSTR)m_strBaseFCode) )continue;
		if( pObj->GetDataPointSum()<2 )continue;
		pSL = pObj->GetShape();
		if( !pSL )continue;
		PT_3D ret;
		double t=-1;
		CPtrArray arr;
		pSL->GetShapeLineUnit(arr);
		int pos = 0;
		for( int k=0;k<arr.GetSize();k++ )
		{
			CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
			if( e2.bIntersect(&pUnit->evlp) )
			{
				pts = pUnit->pts;
				for( int j=0; j<pUnit->nuse-1; j++,pts++)
				{
					if( /*pts[1].code==GRBUFFER_PTCODE_LINETO && */e2.bIntersect(&pts[0],&pts[1]) )
					{
						if( !CGrTrim::Intersect(m_ptLine+2,m_ptLine+3,&pts[0],&pts[1],&ret,&t) )
							continue;						
						else
						{
							if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
								ret.z = pts[0].z;
							else
							{
								if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
									ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
								else
									ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
							}
						}
						item.id = FtrToHandle(ftr2[i]); item.pos= pos+j; item.ret= ret; item.t = t;
						size1 = arr2.GetSize();
						for( int m=0; m<size1 && item.t>=arr2[m].t; m++);
						if( m<size1 )arr2.InsertAt(m,item);
						else arr2.Add(item);
						goto FINDOVER_2;
					}
				}
			}
			pos += pUnit->nuse;
		}
FINDOVER_2:;
	}
	CArray<CSmoothCommand::OBJ_PART,CSmoothCommand::OBJ_PART> arr3;
	CSmoothCommand::OBJ_PART part;
	size1 = arr1.GetSize(); size2 = arr2.GetSize();
	for( i=0; i<size1; i++)
	{
		FTR_HANDLE id = arr1[i].id;
		for( int j=0; j<size2 && id!=arr2[j].id; j++);
		if( j<size2 )
		{
			part.id = id;
			part.pos1 = arr1[i].pos; part.pos2 = arr2[j].pos;
			part.pt1 = arr1[i].ret; part.pt2 = arr2[j].ret; 
			m_lft1.Add( arr1[i].t);
			m_lft2.Add( arr2[j].t);
			arr3.Add(part);			
		}
	}
	if(arr3.GetSize()>0)
	{
		m_pt[0]=(arr3.GetAt(0)).pt1;
		m_pt[1]=(arr3.GetAt(0)).pt2;
	}
	size1 = arr3.GetSize();
	PT_3DEX expt;
	for( i=0; i<size1; i++)
	{
		part = arr3[i];
		pObj = HandleToFtr(part.id)->GetGeometry();
		const CShapeLine *pSL = pObj->GetShape();
		CArray<PT_3DEX,PT_3DEX> arr0;
		pObj->GetShape(arr0);
		int code;
		for( int j=arr0.GetSize()-1; j>=0; j--)
		{
			expt = arr0[j];
			if( j==0 )code = expt.pencode;
			else if( code!=expt.pencode )code = penLine;
			arr0.RemoveAt(j);
		}
		expt.pencode = code;		
		COPY_3DPT(expt,part.pt1);
		arr0.Add(expt);
		CPtrArray arrUnit;
		if(!pSL->GetShapeLineUnit(arrUnit))return;
		CShapeLine::ShapeLineUnit *pUnit = NULL;
		int pos = 0, end;
		BOOL bcopyover = (part.pos1==part.pos2);
		for(int k = 0;k<arrUnit.GetSize()&&!bcopyover;k++ )
		{
			pUnit = (CShapeLine::ShapeLineUnit *)arrUnit[k];
			pts = pUnit->pts;
			if( part.pos1<part.pos2 )
			{
				if( part.pos1<pos+pUnit->nuse )
				{
					end = ((part.pos2<pos+pUnit->nuse)?(part.pos2-pos+1):pUnit->nuse);
					for( int j=part.pos1-pos+1; j<end; j++)
					{
						COPY_3DPT(expt,(pts[j]));
						arr0.Add(expt);
					}
					part.pos1 = pos+j;
				}
				if( part.pos1>=part.pos2 )bcopyover = TRUE;
			}
			else
			{
				if( part.pos2<pos+pUnit->nuse )
				{
					end = ((part.pos1<pos+pUnit->nuse)?(part.pos1-pos+1):pUnit->nuse);
					for( int j=part.pos2-pos+1; j<end; j++)
					{
						COPY_3DPT(expt,(pts[j]));
						arr0.InsertAt(1,expt);
					}
					part.pos2 = pos+j;
				}
				if( part.pos2>=part.pos1 )bcopyover = TRUE;
			}
			pos += pUnit->nuse;
			pUnit = pUnit->next;
		}
		part = arr3[i];
		COPY_3DPT(expt,part.pt2);
		arr0.Add(expt);
		CFeature *pFtr = HandleToFtr(part.id)->Clone();
		if(!pFtr) return;
		pFtr->GetGeometry()->CreateShape(arr0.GetData(),arr0.GetSize());
		arr->Add(pFtr);
	}
}
void CContinueInterpolateCommand::PtReset(PT_3D &pt)
{
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->RefreshView();
	m_arrLine.RemoveAll();
	if(m_arrAddObjs.GetSize()<=0) 
	{
		Abort();
		return;
	}
// 	CUndoFtrs undo(m_pEditor,Name());
// 	for (int i=0;i<m_arrUndoobjs.GetSize();i++)
// 	{
// 		undo.AddNewFeature(m_arrUndoobjs.GetAt(i));
// 	}
// 	undo.Commit();
	m_arrUndoobjs.RemoveAll();
	m_arrAddObjs.RemoveAll();	
	Finish();
}

void CContinueInterpolateCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ContinueInterpolateCommand",StrFromResID(IDS_CMDNAME_CONTINUEINTERP));
	
	m_nMode = wayEdis;

	param->BeginOptionParam(PF_INTERWAY,StrFromResID(IDS_CMDPLANE_WAY));
//	param->AddOption(StrFromResID(IDS_CMDPLANE_NEARESTDIS),wayNdis,' ',m_nMode==wayNdis);
	param->AddOption(StrFromResID(IDS_CMDPLANE_EDIS),wayEdis,' ',m_nMode==wayEdis);	
	param->EndOptionParam();
	
	param->AddParam(PF_INTERONLYCONTOUR,bool(m_bOnlyContour),StrFromResID(IDS_CMDPLANE_ONLYCONTOUR));
	
	param->AddLayerNameParam(PF_INTERSONCODE,LPCTSTR(m_strSonFCode),StrFromResID(IDS_CMDPLANE_SONCODE));
	param->AddParam(PF_INTERSONNUM,int(m_nInNum),StrFromResID(IDS_CMDPLANE_SONNUM));
	if(m_nMode==wayNdis||bForLoad)param->AddParam(PF_INTERRADIUS,int(m_nFindRadius),StrFromResID(IDS_CMDPLANE_RADIUS));
	param->AddLayerNameParam(PF_INTERBASECODE,LPCTSTR(m_strBaseFCode),StrFromResID(IDS_CMDPLANE_BASECODE));

}
void CContinueInterpolateCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_INTERWAY,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERONLYCONTOUR,var) )
	{		
		m_bOnlyContour = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERSONCODE,var) )
	{
		m_strSonFCode = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERSONNUM,var) )
	{
		m_nInNum = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERRADIUS,var) )
	{
		m_nFindRadius = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERBASECODE,var) )
	{
		m_strBaseFCode = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}



//////////////////////////////////////////////////////////////////////
// CInterpolateCenterLineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CInterpolateCenterLineCommand,CContinueInterpolateCommand)

CInterpolateCenterLineCommand::CInterpolateCenterLineCommand()
{
	strcat(m_strRegPath,"\\InterpolateCenterLine");
}

CInterpolateCenterLineCommand::~CInterpolateCenterLineCommand()
{
	
}

void CInterpolateCenterLineCommand::Start()
{
	if( !m_pEditor )return;	
	
	m_strBaseFCode = _T("");
	m_strSonFCode = _T("");
	m_bOnlyContour = FALSE;	

	CCommand::Start();

	m_nMode = wayEdis;
	m_nPart = modePart;
	m_nFindRadius = 20;
	m_nStep = 0;
	m_nInNum = 1;

	m_lft1.RemoveAll();
	m_lft2.RemoveAll();
	
	m_arrLine.RemoveAll();
	m_arrObjRatio1.RemoveAll();
	m_arrObjRatio2.RemoveAll();

	CCommand::Start();
//cjc 2012年11月12日
	if( m_nPart==modeWhole )
		m_pEditor->OpenSelector(SELMODE_MULTI);
	else
		m_pEditor->CloseSelector();	

	if( m_nPart==modeWhole )
		PromptString(StrFromResID(IDS_CMDTIP_INTOBJ1));//OutputTipString(StrFromResID(IDS_CMDTIP_INTOBJ1));
	else
		PromptString(StrFromResID(IDS_CMDTIP_INTLINE1));//OutputTipString(StrFromResID(IDS_CMDTIP_INTLINE1));
//	m_pEditor->CloseSelector();
}

CString CInterpolateCenterLineCommand::Name()
{
	return StrFromResID(IDS_CMDPLANE_INTERPOLATECENTERLINE);
}


void CInterpolateCenterLineCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("InterpolateCenterLineCommand",StrFromResID(IDS_CMDPLANE_INTERPOLATECENTERLINE));
	
	m_nMode = wayEdis;
	
	param->AddLayerNameParam(PF_INTERSONCODE,LPCTSTR(m_strSonFCode),StrFromResID(IDS_CMDPLANE_SONCODE));
	param->AddLayerNameParam(PF_INTERBASECODE,LPCTSTR(m_strBaseFCode),StrFromResID(IDS_CMDPLANE_BASECODE));
	
	param->BeginOptionParam(PF_INTERRANGE,StrFromResID(IDS_CMDPLANE_RANGE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_WHOLE),modeWhole,' ',m_nPart==modeWhole);
	param->AddOption(StrFromResID(IDS_CMDPLANE_PART),modePart,' ',m_nPart==modePart);	
	param->EndOptionParam();
}


//////////////////////////////////////////////////////////////////////
///////CInterpolateCenterLoadCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CInterpolateCenterLoadCommand,CInterpolateCommand)

CString CInterpolateCenterLoadCommand::Name()
{
   return StrFromResID(IDS_CMDPLANE_INTERPOLATECENTERLOAD);
}

void CInterpolateCenterLoadCommand::PtClick1(PT_3D &pt, int flag)
{
    //get layers;
	std::map<CString,CString> lsource_ldes;
	lsource_ldes[m_strBaseFCode] = m_strSonFCode;
	//
	for(std::map<CString,CString>::iterator itr_layer = lsource_ldes.begin(); itr_layer!=lsource_ldes.end(); ++itr_layer)
	{
		CFtrLayer* psource_layer = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayer(itr_layer->first);
		CFtrLayer* pdes_layer = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayer(itr_layer->second);
		if(psource_layer == NULL || pdes_layer == NULL)
			continue;
		// 
		std::map<CString,CFeature*> target_ftrs;
		std::map<CString,CGeoParallel*> target_parallels;
		std::map<CString,CGeoDCurve*> target_dcurves;
		std::map<CString,CGeoCurve*> target_curves;
		for(int i=0; i<psource_layer->GetValidObjsCount(); ++i)
		{
			CFeature* pftr = psource_layer->GetObject(i);
			if(pftr == NULL || pftr->GetGeometry() == NULL)
				continue;
			//
			if(pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			{
               target_parallels[pftr->GetID().ToString()] = (CGeoParallel*)pftr->GetGeometry();
			}
			else if(pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
               target_dcurves[pftr->GetID().ToString()] = (CGeoDCurve*)pftr->GetGeometry();
			}
			else if(pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
			   target_curves[pftr->GetID().ToString()] = (CGeoCurve*)pftr->GetGeometry();
			}
			//
			target_ftrs[pftr->GetID().ToString()] = pftr;
		}
		//simplefy;
		std::vector<bool> vailds(target_curves.size(),false);
		std::vector<bool> compaired(target_curves.size(),false);
		std::vector<bool> closed(target_curves.size(),false);
		std::map<CString,CArray<PT_3DEX, PT_3DEX>*> target_shapes;
		i=0;
        for(std::map<CString,CGeoCurve*>::iterator itr_curve = target_curves.begin(); itr_curve != target_curves.end(); ++itr_curve,++i)
		{
            CArray<PT_3DEX,PT_3DEX> source_points;
			itr_curve->second->GetShape(source_points);
			if(source_points.GetSize()<2)
			{
				target_shapes[itr_curve->first] = NULL;
				continue;
			}
			//
            CArray<PT_3DEX,PT_3DEX>* res_points = new CArray<PT_3DEX,PT_3DEX>();
			res_points->Add(source_points[0]);
			double check_angle = cos(20*3.14159/180);
            for(int t=1; t<source_points.GetSize(); ++t)
			{
                if(fabs(source_points[t].x-res_points->GetAt(res_points->GetSize()-1).x)<=0.001 && 
				   fabs(source_points[t].y-res_points->GetAt(res_points->GetSize()-1).y)<=0.001)
                   continue;
				//
				if(res_points->GetSize()==1)
				{
					res_points->Add(source_points[t]);
					continue;
				}
				//
				if(t == source_points.GetSize() - 1)
				{
					res_points->Add(source_points[t]);
				    continue;
				}
				//
				int res_size = res_points->GetSize();
				geom::vector2d va(res_points->GetAt(res_size-1).x-res_points->GetAt(res_size-2).x, 
					              res_points->GetAt(res_size-1).y-res_points->GetAt(res_size-2).y);
				geom::vector2d vb(source_points[t].x-res_points->GetAt(res_size-1).x, source_points[t].y-res_points->GetAt(res_size-1).y);
				geom::vector2d vc(source_points[t].x-source_points[t-1].x, source_points[t].y-source_points[t-1].y);
                geom::vector2d vd(source_points[t+1].x-source_points[t].x, source_points[t+1].y-source_points[t-1].y);
				va = va.normalize();
				vb = vb.normalize();
				vc = vc.normalize();
                vd = vd.normalize();
				if(va*vb<=check_angle || vc*vd<=check_angle)
				{
                   res_points->Add(source_points[t]);
				   continue;
				}
			}
			//
			if(res_points->GetSize()>=2)
			{
				vailds[i] = true;
				if(fabs(res_points->GetAt(0).x - res_points->GetAt(res_points->GetSize()-1).x)<0.001 &&
				   fabs(res_points->GetAt(0).y - res_points->GetAt(res_points->GetSize()-1).y)<0.001)
				   closed[i] = true;
				//
                target_shapes[itr_curve->first] = res_points;

				//
				GrBuffer buf_con;
				buf_con.SetAllColor(RGB(255,255,255));
				buf_con.BeginLineString(RGB(255,255,255),100);
				//
				PT_3DEX pt3d;
				for(int t = 0; t<res_points->GetSize()-1; ++t)
				{
						pt3d.x = res_points->GetAt(t).x;
						pt3d.y = res_points->GetAt(t).y;
						pt3d.z = 0;
						buf_con.MoveTo(&pt3d);
						//
						pt3d.x = res_points->GetAt(t+1).x;
						pt3d.y = res_points->GetAt(t+1).y;
						pt3d.z = 0;
						buf_con.LineTo(&pt3d);
				}
				//
				buf_con.End(FALSE);
				buf_con.RefreshEnvelope();
	            ((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_AddGraph,(CObject*)&buf_con);
				//
				GrBuffer buf_sp;
				buf_sp.SetAllColor(RGB(0,0,255));
				buf_sp.BeginPointString(RGB(0,0,255),30,30);
				// 
                for(t = 0; t<res_points->GetSize(); ++t)
				{
					pt3d.x = res_points->GetAt(t).x;
					pt3d.y = res_points->GetAt(t).y;
					pt3d.z = 0;
					buf_con.PointString(&pt3d);
				}
				buf_sp.End(FALSE);
				buf_sp.RefreshEnvelope();
	            ((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_AddGraph,(CObject*)&buf_sp);
			}
			else
			{
				target_shapes[itr_curve->first] = NULL;
			}
		}
		//compaire;
		const double max_flag = 3e100;
		std::map<CString,CString> pairs;
		std::map<CString,double> compaire_flags;
		std::map<CString,CArray<PT_3DEX, PT_3DEX>*>::iterator itr_curve_a = target_shapes.begin();
		for(int n=0; itr_curve_a != target_shapes.end(); ++itr_curve_a, ++n)
		{
            if(!vailds[n])
				continue;
			//
			CArray<PT_3DEX, PT_3DEX>& master_curve = *itr_curve_a->second;
			geom::vector2d master_direct(master_curve[master_curve.GetSize()-1].x - master_curve[0].x,
				                         master_curve[master_curve.GetSize()-1].y - master_curve[0].y);
			//
			std::vector<double> flags(target_shapes.size(),max_flag);
			//
			std::map<CString,CArray<PT_3DEX, PT_3DEX>*>::iterator itr_curve_b = target_shapes.begin();
			for(int m=0; itr_curve_b != target_shapes.end(); ++itr_curve_b, ++m)
			{ 
                if(!vailds[m] || n==m || closed[n]!=closed[m])
					continue;
				//
                CArray<PT_3DEX, PT_3DEX>& test_curve = *itr_curve_b->second;
				geom::vector2d test_direct (test_curve[test_curve.GetSize()-1].x - test_curve[0].x,
				                            test_curve[test_curve.GetSize()-1].y - test_curve[0].y);
				//
				if(closed[n])
				{
					double flag = 0;
                    for(int t=0; t<master_curve.GetSize(); ++t)
					{
						double temp_flag = max_flag;
						for(int k=0; k<test_curve.GetSize()-1; ++k)
						{
							double lpx,lpy;
							double distance = GraphAPI::GGetNearestDisOfPtToLine(test_curve[k].x,test_curve[k].y,test_curve[k+1].x,test_curve[k+1].y,
								                                                  master_curve[t].x,master_curve[t].y,&lpx,&lpy,FALSE);
							if(distance<temp_flag)
								temp_flag = distance;
						}
						flag += temp_flag;
					}
					//
					flags[m] = flag;
				}
				else if(master_direct.normalize()*test_direct.normalize()>0.6)
				{
					double flag = 0;
					int k=0;
                    for(int t=0; t<master_curve.GetSize(); ++t)
					{
						double temp_flag = max_flag;
						for(; k<test_curve.GetSize()-1;)
						{
							double lpx,lpy;
							double distance = GraphAPI::GGetNearestDisOfPtToLine(test_curve[k].x,test_curve[k].y,test_curve[k+1].x,test_curve[k+1].y,
								                                                  master_curve[t].x,master_curve[t].y,&lpx,&lpy,FALSE);
							if(distance<temp_flag)
							{
								temp_flag = distance;
								++k;
								continue;
							}
							//
							break;
						}
						if(k==test_curve.GetSize()-1)
							temp_flag = geom::vector2d(master_curve[t].x-test_curve[k].x,master_curve[t].y-test_curve[k].y).length();
						//
						flag += temp_flag;
					}
					//
					flags[m] = flag;
				}
				else if(master_direct.normalize()*test_direct.normalize()<-0.6)
				{
                    double flag = 0;
					int k=test_curve.GetSize()-1;
                    for(int t=0; t<master_curve.GetSize(); ++t)
					{
						double temp_flag = max_flag;
						for(; k>0;)
						{
							double lpx,lpy;
							double distance = GraphAPI::GGetNearestDisOfPtToLine(test_curve[k].x,test_curve[k].y,test_curve[k+1].x,test_curve[k+1].y,
								                                                  master_curve[t].x,master_curve[t].y,&lpx,&lpy,FALSE);
							if(distance<temp_flag)
							{
								temp_flag = distance;
								--k;
								continue;
							}
							//
							break;
						}
						if(k==0)
							temp_flag = geom::vector2d(master_curve[t].x-test_curve[k].x,master_curve[t].y-test_curve[k].y).length();
						//
						flag += temp_flag;
					}
					//
					flags[m] = flag;
				}
				else
				{
                     
				}
			}
			//
            double min_flag = max_flag;
			int index = -1;
			for(m=0; m<flags.size(); ++m)
			{
				if(flags[m]<min_flag)
				{
					min_flag = flags[m];
					index = m;
				}
			}
			if(index != -1)
			{
                std::map<CString,CArray<PT_3DEX, PT_3DEX>*>::iterator itr_target = target_shapes.begin();
				for(m=0; m<index; ++m)
                   ++itr_target;
				//
				//compaired[index] = true;
				pairs[itr_curve_a->first] = itr_target->first;
				compaire_flags[itr_curve_a->first] = min_flag;

				//AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)(itr_curve_a->first+"--"+itr_target->first));
			}
			//
            //compaired[n] = true;
			
		}

		std::map<CString,CString> final_pairs;
        for(std::map<CString,CString>::iterator itr_pair = pairs.begin(); itr_pair != pairs.end(); ++itr_pair)
		{
			if(final_pairs.find(itr_pair->first) != final_pairs.end() ||
			   final_pairs.find(itr_pair->second) != final_pairs.end())
			   continue;
			//
			CString first = itr_pair->first;
			CString second = itr_pair->second;
			CString third = pairs[itr_pair->second];
			while(first != third)
			{
               first = third;
			   second = pairs[third];
			   third = pairs[second];
			}
			//
			final_pairs[first] = second;
            
		}
		//interpolate;
		int k = 0;
        for(itr_pair = final_pairs.begin(); itr_pair != final_pairs.end(); ++itr_pair)
		{		
			FTR_HANDLE handle_a = FtrToHandle(target_ftrs[itr_pair->first]);
            FTR_HANDLE handle_b = FtrToHandle(target_ftrs[itr_pair->second]);
			m_pEditor->DeleteObject(handle_a,FALSE);
			m_pEditor->DeleteObject(handle_b,FALSE);
			double psin = sin(double(k)*3.14159/2/final_pairs.size());
			double pcos = cos(double(k)*3.14159/2/final_pairs.size());
            target_ftrs[itr_pair->first]->GetGeometry()->SetColor(RGB(255*psin,255*pcos,255*k/final_pairs.size()));
		    target_ftrs[itr_pair->second]->GetGeometry()->SetColor(RGB(255*psin,255*pcos,255*k/final_pairs.size()));
            m_pEditor->RestoreObject(handle_a);
			m_pEditor->RestoreObject(handle_b);

			++k;
		}
		//std::map<CString,CFeature*> target_ftrs;
		//std::map<CString,CGeoParallel*> target_parallels;
		//std::map<CString,CGeoDCurve*> target_dcurves;
		//std::map<CString,CGeoCurve*> target_curves;
		//interpolate pallar;
		m_nStep = 1;
		CFeature* pFtr= PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(itr_layer->second);
		if( !pFtr )return;
		//
        for(std::map<CString,CGeoParallel*>::iterator itr_parallel = target_parallels.begin(); itr_parallel!=target_parallels.end(); ++itr_parallel)
		{

		}
		//interpolate dcurve;
		for(std::map<CString,CGeoDCurve*>::iterator itr_dcurve = target_dcurves.begin(); itr_dcurve!=target_dcurves.end(); ++itr_dcurve)
		{
            
		}
		//interpolate compaired curve;
		for(itr_pair = final_pairs.begin(); itr_pair != final_pairs.end(); ++itr_pair)
		{
			CPtrArray arr;
			CFeature *pTmp1 =target_ftrs[itr_pair->first]->Clone();
			CFeature *pTmp2 =target_ftrs[itr_pair->second]->Clone();
			if (pTmp1)
			{
				const CShapeLine *pLine1 = pTmp1->GetGeometry()->GetShape();//线型
				if (pLine1)
				{
					CArray<PT_3DEX,PT_3DEX> tmpArr;
					pLine1->GetPts(tmpArr);
					int code = penNone;
					for (int j=0; j<tmpArr.GetSize(); j++)
					{
						if (j == 0) code = tmpArr[j].pencode;
						else if (tmpArr[j].pencode == penNone)
						{
							tmpArr[j].pencode = code;
						}
					}
					pTmp1->GetGeometry()->CreateShape(tmpArr.GetData(),tmpArr.GetSize());
				}
			}
			if (pTmp2)
			{
				const CShapeLine *pLine2 = pTmp2->GetGeometry()->GetShape();
				if (pLine2)
				{
					CArray<PT_3DEX,PT_3DEX> tmpArr;
					pLine2->GetPts(tmpArr);
					int code = penNone;
					for (int j=0; j<tmpArr.GetSize(); j++)
					{
						if (j == 0) code = tmpArr[j].pencode;
						else if (tmpArr[j].pencode == penNone)
						{
							tmpArr[j].pencode = code;
						}
					}
					pTmp2->GetGeometry()->CreateShape(tmpArr.GetData(),tmpArr.GetSize());
				}
			}				
			InterpWholeObjByEquidis(pTmp1,pTmp2,pFtr,&arr);
			if (pTmp1)
			{
				delete pTmp1;
			}
			if (pTmp2)
			{
				delete pTmp2;
			}
			//
			if( arr.GetSize()>0 )
			{
				for( int i=0; i<arr.GetSize(); i++)
				{
					CFeature *pFtr = (CFeature*)arr[i];
					if (pFtr)
					{
						CArray<PT_3DEX,PT_3DEX> pts;
						pFtr->GetGeometry()->GetShape(pts);
						Compress(pts);
						pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
					}
					
					m_pEditor->AddObject((CFeature*)arr[i],pdes_layer->GetID());
				}	
			}
			
			
			//m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
			//m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			Finish();
			m_nStep = 2;
		}
	}
}

void CInterpolateCenterLoadCommand::KickoffSameGeoPts(CArray<GeoPoint,GeoPoint>& arr)
{
	GeoPoint *pts = arr.GetData();
	if( !pts )return;
	
	GeoPoint *pt = pts+1;
	int pos=0, num = arr.GetSize();
	double tolerance = 1e-4;
	
	for( int i=1; i<num; i++,pt++)
	{
		if( fabs(pts[pos].x-pt->x)<tolerance && 
			fabs(pts[pos].y-pt->y)<tolerance && 
			fabs(pts[pos].z-pt->z)<tolerance )
			continue;
		
		if( i!=(pos+1) )pts[pos+1] = *pt;
		pos++;
	}
	
	pos++;
	
	if( num>pos )arr.RemoveAt(pos,num-pos);
}


void StringSplit(CString source, CStringArray& dest, CString division)
{
	dest.RemoveAll();
	//
    while( source.GetLength()>0 )
	{
		source.TrimLeft();
		source.TrimRight();
		source.TrimLeft(division);
	    source.TrimRight(division);
		if(source.GetLength()==0)
			break;
		//
		source+=division;
        //
		int pos = source.Find(division);
        dest.Add(source.Left(pos));
		if(pos == source.GetLength()-1)
			break;
		//
		source = source.Right(source.GetLength()-pos);
	}
}
void CInterpolateCenterLoadCommand::PtClick(PT_3D &pt, int flag)
{
	//get layers;
	CStringArray bases;
	StringSplit(m_strBaseFCode, bases, ",");
	CStringArray sons;
	StringSplit(m_strSonFCode, sons, ",");
	//
	std::map<CString,CString> lsource_ldes;
	for(int m=0; m<bases.GetSize(); ++m)
	{
		if(m<sons.GetSize())
		{
            lsource_ldes[bases[m]] = sons[m];
		}
		else
		{
			CString temp;
			temp.LoadString(IDS_DEFLAYER_NAMEL);
            lsource_ldes[bases[m]] = temp;
		}
	}
	
	//
	for(std::map<CString,CString>::iterator itr_layer = lsource_ldes.begin(); itr_layer!=lsource_ldes.end(); ++itr_layer)
	{
		CFtrLayer* psource_layer = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayer(itr_layer->first);
		CFtrLayer* pdes_layer = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayer(itr_layer->second);
		if(psource_layer == NULL || pdes_layer == NULL)
			continue;
		//
		std::vector<tin2d::edge2d> segments;
		int constraint_id = 0;  //给每条线段一个 id 号；
		CArray<GeoPoint, GeoPoint> arrAll, arrCur;
		PT_3DEX expt;
		GeoPoint geopt;
		//
		CFeature* ptmp = pdes_layer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
		//
		for(int i=0; i<psource_layer->GetObjectCount(); ++i)
		{
			CFeature* pftr = psource_layer->GetObject(i);
			if(pftr == NULL || pftr->GetGeometry() == NULL)
				continue;
			//
		    CGeometry *pGeo = pftr->GetGeometry()->Linearize();
			//
			if( pGeo != NULL && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			{
				const CShapeLine *pShape = pGeo->GetShape();
				if (!pShape) continue;
				
				CArray<PT_3DEX,PT_3DEX> pts;
				pShape->GetKeyPts(pts);
				
				int npt = pts.GetSize();
				arrCur.RemoveAll();
				for( int n=0; n<npt; n++)
				{
					expt = pts[n];
					geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; 
					arrCur.Add(geopt);
				}
				
				KickoffSameGeoPts(arrCur);

				if( arrCur.GetSize()>=2 )
				{
					int k=0, t=1;
					for(;t<arrCur.GetSize();++k,++t)
					{
						tin2d::edge2d edge;
					
						edge.sp.x = arrCur[k].x; edge.sp.y = arrCur[k].y; edge.sp.z = arrCur[k].z; edge.sp.constraint_id = constraint_id;
						edge.ep.x = arrCur[t].x; edge.ep.y = arrCur[t].y; edge.ep.z = arrCur[t].z; edge.ep.constraint_id = constraint_id;
						edge.constraint_id = constraint_id;
						//
						segments.push_back(edge);
					}
				}
			}
			if( pGeo != NULL && pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
			{
				CArray<PT_3DEX,PT_3DEX> pts;
				((CGeoParallel*)pGeo)->GetParallelShape(pts);
				//
				int npt = pts.GetSize();
				arrCur.RemoveAll();
				for( int n=0; n<npt; n++)
				{
					expt = pts[n];
					geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; 
					arrCur.Add(geopt);
				}
				
				KickoffSameGeoPts(arrCur);

				if( arrCur.GetSize()>=2 )
				{
					int k=0, t=1;
					for(;t<arrCur.GetSize();++k,++t)
					{
						tin2d::edge2d edge;
					
						edge.sp.x = arrCur[k].x; edge.sp.y = arrCur[k].y; edge.sp.z = arrCur[k].z; edge.sp.constraint_id = constraint_id + 1;
						edge.ep.x = arrCur[t].x; edge.ep.y = arrCur[t].y; edge.ep.z = arrCur[t].z; edge.ep.constraint_id = constraint_id + 1;
						edge.constraint_id = constraint_id + 1;
						//
						segments.push_back(edge);
					}	
				}	
			}
			if( pGeo != NULL && pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
			{
				CArray<PT_3DEX,PT_3DEX> pts;
				((CGeoDCurve*)pGeo)->GetOrderShape(pts);
				//
				int npt = pts.GetSize();
				arrCur.RemoveAll();
				for( int n=0; n<npt; n++)
				{
					expt = pts[n];
					geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; 
					arrCur.Add(geopt);
				}
				
				KickoffSameGeoPts(arrCur);

				if( arrCur.GetSize()>=2 )
				{
					int k=0, t=1;
					for(;t<arrCur.GetSize();++k,++t)
					{
						tin2d::edge2d edge;
					
						edge.sp.x = arrCur[k].x; edge.sp.y = arrCur[k].y; edge.sp.z = arrCur[k].z; edge.sp.constraint_id = constraint_id + 1;
						edge.ep.x = arrCur[t].x; edge.ep.y = arrCur[t].y; edge.ep.z = arrCur[t].z; edge.ep.constraint_id = constraint_id + 1;
						edge.constraint_id = constraint_id + 1;
						//
						segments.push_back(edge);
					}
					//
				}	
			}
			//
			constraint_id += 2;
		}
		//build tin;
		tin2d::TINHandler* tin_cgaldelaunay = new tin2d::TINHandler();
		tin_cgaldelaunay->BuildTIN(std::vector<tin2d::point2d>(), &segments);
		//get compaire information;
		std::map<int,std::vector<CompaireItem> > compaires;     //可能的配对关系，key 是线的 id 号；  
		std::map<int,double> constraint_height;                 //每条线配对的宽度的估算值；
		std::map<int,int> constraint_size;                      //每条线的子线段数， key 是线的 id 号； 
		std::map<int,int> edge_segment;                         //当线段两端的 id 不一致时，到 segment 中去匹配，key 是三角网中edge 的序号，value 是对应的 segment 中的序号；
        int edge_count = tin_cgaldelaunay->getEdgeCount();
		for(i=0; i<edge_count; ++i)
		{
            if(tin_cgaldelaunay->isConstraint(i))
			{
				tin2d::triangle2d trians[2];
				bool useable_trians[2] = {true,true};
				tin_cgaldelaunay->getTriangleContainEdge(trians,i);
				//
				int compaired_constraint = -1;
				double height = 0;
				//
				if(trians[0].pm.constraint_id != -1 && 
				   (trians[0].pm.constraint_id == trians[0].pl.constraint_id || 
				   trians[0].pm.constraint_id == trians[0].pr.constraint_id) )
				{
					useable_trians[0] = false;
				}
				//
				if(trians[1].pm.constraint_id != -1 &&
				   (trians[1].pm.constraint_id == trians[1].pl.constraint_id || 
				   trians[1].pm.constraint_id == trians[1].pr.constraint_id) )
				{
					useable_trians[1] = false;
				}
				//
				if(useable_trians[0] == useable_trians[1])
				{
					geom::vector2d ve(trians[0].pl.x - trians[0].pr.x,trians[0].pl.y - trians[0].pr.y);
					geom::vector2d va(trians[0].pm.x - trians[0].pr.x,trians[0].pm.y - trians[0].pr.y);
					geom::vector2d vb(trians[1].pm.x - trians[1].pr.x,trians[1].pm.y - trians[1].pr.y);
					//
					double squard_a = fabs(ve.cross(va));
					double squard_b = fabs(ve.cross(vb));
					//
					
					if(squard_a<=squard_b)
					{
						compaired_constraint = trians[0].pm.constraint_id;
						height = squard_a / ve.length();
					}
					else
					{
						compaired_constraint = trians[1].pm.constraint_id;
						height = squard_b / ve.length();
					}
				}
				else if(useable_trians[0] && !useable_trians[1])
				{
					compaired_constraint = trians[0].pm.constraint_id;
					//
					geom::vector2d ve(trians[0].pl.x - trians[0].pr.x,trians[0].pl.y - trians[0].pr.y);
					geom::vector2d va(trians[0].pm.x - trians[0].pr.x,trians[0].pm.y - trians[0].pr.y);
					//
					double squard_a = fabs(ve.cross(va));
					//
					height = squard_a / ve.length();
				}
				else if(useable_trians[1] && !useable_trians[0])
				{
					compaired_constraint = trians[1].pm.constraint_id;
					//
					geom::vector2d ve(trians[0].pl.x - trians[0].pr.x,trians[0].pl.y - trians[0].pr.y);
					geom::vector2d vb(trians[1].pm.x - trians[1].pr.x,trians[1].pm.y - trians[1].pr.y);
					//
				    double squard_b = fabs(ve.cross(vb));
					//
					height = squard_b / ve.length();
				}
				//
				std::vector<CompaireItem>* pcomp = NULL;
				int constraint_id;
				//
				if(trians[0].pl.constraint_id != -1 && trians[0].pl.constraint_id == trians[0].pr.constraint_id)
				{
					pcomp = &compaires[trians[0].pl.constraint_id];	
					constraint_id = trians[0].pl.constraint_id;
				}
				else if(trians[0].pl.constraint_id != -1 && trians[0].pr.constraint_id == - 1)
				{
					pcomp = &compaires[trians[0].pl.constraint_id];
					constraint_id = trians[0].pl.constraint_id;
				}
				else if(trians[0].pr.constraint_id != -1 && trians[0].pl.constraint_id == - 1)
				{
					pcomp = &compaires[trians[0].pr.constraint_id];
					constraint_id = trians[0].pr.constraint_id;
				}
				else if(trians[0].pr.constraint_id == -1 && trians[0].pl.constraint_id == - 1)
				{
					pcomp = NULL;
				}
				else 
				{
					for(int k=0; k<segments.size(); ++k)
					{
						if((trians[0].pl == segments[k].sp || trians[0].pl == segments[k].ep) && 
							(trians[0].pr == segments[k].sp || trians[0].pr == segments[k].ep))
						{
							pcomp = &compaires[segments[k].constraint_id];
							constraint_id = segments[k].constraint_id;
							//
							edge_segment[i] = k;
							break;
						}
					}
				}
				//
				if(pcomp != NULL)
				{
                    std::map<int,double>::iterator itr_ch = constraint_height.find(constraint_id);
					if(itr_ch != constraint_height.end())
					{
						itr_ch->second += height;
						++constraint_size[itr_ch->first];
					}
					else
					{
						constraint_height[constraint_id] = height;
						constraint_size[constraint_id] = 1;
					}
				}
				//
				if(compaired_constraint != -1)
				{
					if(pcomp != NULL)
					{
						int t = 0;
						for(; t<pcomp->size(); ++t)
						{
							if(pcomp->at(t).constraint_id == compaired_constraint)
							{
								++pcomp->at(t).compaire_count;
								break;
							}
						}
						if(t==pcomp->size())
						{
							CompaireItem temp;
							temp.constraint_id = compaired_constraint;
							temp.compaire_count = 1;
							pcomp->push_back(temp);
						}
					}
				}
			}
		}
		//get ave height;
		double average_height = 0;    // 整体平均宽度估算值；
		for(std::map<int,double>::iterator itr_con = constraint_height.begin(); itr_con != constraint_height.end(); ++itr_con)
		{
			itr_con->second = itr_con->second/constraint_size[itr_con->first];
			average_height += itr_con->second;
		}
		average_height /= 2*constraint_height.size(); 
		//get center lines;
		//每个线段都会有两个三角形，选取合适的三角形连接另外两边的中点；
		std::vector<tin2d::edge2d> target_lines;          //离散的中心线；
		std::map<int,std::vector<int> >final_compaires;   //最终的配对关系；
		for(i=0; i<edge_count; ++i)
		{
            if(tin_cgaldelaunay->isConstraint(i))
			{
				tin2d::triangle2d trians[2];
				bool useable_trians[2] = {true,true};
				tin_cgaldelaunay->getTriangleContainEdge(trians,i);
				//
// 				if((fabs(trians[0].pl.x+1362.2563)<0.001 && fabs(trians[0].pl.y+393.8360)<0.001 && fabs(trians[0].pr.x+1368.9379)<0.001 && fabs(trians[0].pr.y+398.0695)<0.001) ||
// 				   (fabs(trians[0].pr.x+1362.2563)<0.001 && fabs(trians[0].pr.y+393.8360)<0.001 && fabs(trians[0].pl.x+1368.9379)<0.001 && fabs(trians[0].pl.y+398.0695)<0.001))
// 				{
// 					int a = 0;
// 				}
				//
				tin2d::triangle2d* right_trian = NULL;
				//
				if(trians[0].pm.constraint_id != -1 && 
					(trians[0].pm.constraint_id == trians[0].pl.constraint_id || 
					trians[0].pm.constraint_id == trians[0].pr.constraint_id) )
				{
					useable_trians[0] = false;
				}
				//
				if(trians[1].pm.constraint_id != -1 &&
					(trians[1].pm.constraint_id == trians[1].pl.constraint_id || 
					trians[1].pm.constraint_id == trians[1].pr.constraint_id) )
				{
					useable_trians[1] = false;
				}
				//
				double compaire_heights[2];
				if(useable_trians[0] == useable_trians[1])
				{
					geom::vector2d ve(trians[0].pl.x - trians[0].pr.x,trians[0].pl.y - trians[0].pr.y);
					geom::vector2d va(trians[0].pm.x - trians[0].pr.x,trians[0].pm.y - trians[0].pr.y);
					geom::vector2d vb(trians[1].pm.x - trians[1].pr.x,trians[1].pm.y - trians[1].pr.y);
					//
					double squard_a = fabs(ve.cross(va));
					double squard_b = fabs(ve.cross(vb));
					//
					compaire_heights[0] = squard_a / ve.length();
				    //
					compaire_heights[1] = squard_b / ve.length();
					//
					double compaire_rates[2] = {0,0};
					std::vector<CompaireItem>* pcomp  = NULL;
					int constraint_id;
					if(trians[0].pl.constraint_id == trians[0].pr.constraint_id && trians[0].pl.constraint_id!=-1)
					{
						pcomp = &compaires[trians[0].pl.constraint_id];
						constraint_id = trians[0].pl.constraint_id;
					}
					else if(trians[0].pl.constraint_id != -1 && trians[0].pr.constraint_id == -1)
					{
						pcomp = &compaires[trians[0].pl.constraint_id];
						constraint_id = trians[0].pl.constraint_id;
					}
					else if(trians[0].pr.constraint_id != -1 && trians[0].pl.constraint_id == -1)
					{
						pcomp = &compaires[trians[0].pr.constraint_id];
						constraint_id = trians[0].pr.constraint_id;
					}
					else if(trians[0].pl.constraint_id != -1 && trians[0].pr.constraint_id != -1)
					{
						pcomp = &compaires[segments[edge_segment[i]].constraint_id];
						constraint_id = segments[edge_segment[i]].constraint_id;
					}
					if(pcomp != NULL)
					{
						bool can_compaire = true;
						//
						for(int k=0; k<2 && can_compaire; ++k)
						{
							if(useable_trians[k])
							{
								if(trians[k].pm.constraint_id != -1)
								{
									int compaire_index = -1;
									int total_count = 0;
									for(int t=0; t<pcomp->size(); ++t)
									{
										total_count += pcomp->at(t).compaire_count;
										//
										if(pcomp->at(t).constraint_id == trians[k].pm.constraint_id)
											compaire_index = t;
									}
									//
									double compaire_rate = 0;
									if(compaire_index != -1)
									{
										compaire_rate += (double)pcomp->at(compaire_index).compaire_count/(double)total_count;
									}
									else
									{
										can_compaire = false;
										continue;
									}
									//
									compaire_index = -1;
									total_count = 0;
									std::vector<CompaireItem>& comp = compaires[trians[k].pm.constraint_id];
									for(t=0; t<comp.size(); ++t)
									{
										total_count += comp[t].compaire_count;
										//
										if(comp[t].constraint_id == constraint_id)
											compaire_index = t;
									}
									if(compaire_index != -1)
									{
										compaire_rate += (double)comp[compaire_index].compaire_count/(double)total_count;
									}
									else
									{
										compaire_rate += 0.15;
									}
									//计算两条线的匹配率，匹配率太低则放弃此三角形；
									if(compaire_rate < 0.5)
										useable_trians[k] = false;
									//
									compaire_rates[k] = compaire_rate;
								}
								else
                                    can_compaire = false;
							}
						}
						//如果两个三角形都可用，则首先检查有没有畸形的三角形
						if(useable_trians[0] && useable_trians[1] && compaire_rates[0]<1.0 && compaire_rates[1]<1.0)
						{
							geom::vector2d ve(trians[0].pl.x - trians[0].pr.x,trians[0].pl.y - trians[0].pr.y);
							for(int k=0; k<2; ++k)
							{
                                geom::vector2d va(trians[0].pl.x - trians[k].pm.x,trians[0].pl.y - trians[k].pm.y);
					            geom::vector2d vb(trians[0].pr.x - trians[k].pm.x,trians[0].pr.y - trians[k].pm.y);
								//
                                double cosa = va.normalize()*ve.normalize();
								double cosb = vb.normalize()*ve.normalize();
								//高太大或太小；
								if(compaire_heights[k]<average_height/10 || compaire_heights[k]>average_height*10)
								{
									useable_trians[k] = false;
									continue;
								}
								//三角形包和度太差；
								double check_flag = cos(20*3.1415926/180);
								if(cosa*cosb>0 && (fabs(cosa)>check_flag || fabs(cosb)<check_flag))
								{
									if(compaire_heights[(k+1)%2]<=average_height*10)
										useable_trians[k] = false;
								}
							}	
						}
						//如果如果两个三角形都可用，则放弃高度不合适的；
						//如果高度都合适，则优先选取匹配率更好的；
						if(useable_trians[0] && useable_trians[1])
						{
							if(compaire_heights[1] > constraint_height[constraint_id]*3 || compaire_heights[1] < constraint_height[constraint_id]/3 ||
							   (compaire_rates[0] > compaire_rates[1] && compaire_rates[1]<=1.0))
								useable_trians[1] = false;
							//
							if(compaire_heights[0] > constraint_height[constraint_id]*3 || compaire_heights[0] < constraint_height[constraint_id]/3 ||
							   (compaire_rates[0] < compaire_rates[1] && compaire_rates[0]<=1.0))
							    useable_trians[0] = false;
						}
                        //如果如果两个三角形都可用，则选取高度更小的；
						if(!can_compaire || useable_trians[0] == useable_trians[1])
						{
							if(compaire_heights[0] >= compaire_heights[1])
								useable_trians[0] = false;
							else 
								useable_trians[1] = false;
						}   
					}	
				}	
 				//
				if(useable_trians[0] == useable_trians[1])
				{
					geom::vector2d ve(trians[0].pl.x - trians[0].pr.x,trians[0].pl.y - trians[0].pr.y);
					geom::vector2d va(trians[0].pm.x - trians[0].pr.x,trians[0].pm.y - trians[0].pr.y);
					geom::vector2d vb(trians[1].pm.x - trians[1].pr.x,trians[1].pm.y - trians[1].pr.y);
					//
					double squard_a = fabs(ve.cross(va));
					double squard_b = fabs(ve.cross(vb));
					//
					if(squard_a<=squard_b)
						right_trian = &trians[0];
					else
						right_trian = &trians[1];
				}
				else if(useable_trians[0] && !useable_trians[1])
				{
					right_trian = &trians[0];
				}
				else if(useable_trians[1] && !useable_trians[0])
				{
					right_trian = &trians[1];
				}
				else
				{
					right_trian = NULL;
				}
				//
				if(right_trian != NULL)
				{
					tin2d::edge2d temp;
					//
					temp.sp.x = (right_trian->pl.x+right_trian->pm.x)/2;
					temp.sp.y = (right_trian->pl.y+right_trian->pm.y)/2;
					temp.sp.z = (right_trian->pl.z+right_trian->pm.z)/2;
					//
					temp.ep.x = (right_trian->pr.x+right_trian->pm.x)/2;
					temp.ep.y = (right_trian->pr.y+right_trian->pm.y)/2;
					temp.ep.z = (right_trian->pr.z+right_trian->pm.z)/2;
					//
					temp.flag = 1;
					target_lines.push_back(temp);
					//////
					int constraint_id = -1;
					if(right_trian->pl.constraint_id == right_trian->pr.constraint_id && right_trian->pl.constraint_id!=-1)
					{
						constraint_id = right_trian->pl.constraint_id;
					}
					else if(right_trian->pl.constraint_id != -1 && right_trian->pr.constraint_id == -1)
					{
						constraint_id = right_trian->pl.constraint_id;
					}
					else if(right_trian->pr.constraint_id != -1 && right_trian->pl.constraint_id == -1)
					{
						constraint_id = right_trian->pr.constraint_id;
					}
					else if(right_trian->pl.constraint_id != -1 && right_trian->pr.constraint_id != -1)
					{
						constraint_id = segments[edge_segment[i]].constraint_id;
					}
                    //
					target_lines[target_lines.size()-1].constraint_id = constraint_id;
					std::vector<int>& comp = final_compaires[constraint_id];
					int k=0;
                    for(k=0; k<comp.size(); ++k)
					{
						if(comp[k] == right_trian->pm.constraint_id)
							break;
					}
					if(k==comp.size())
					{
						comp.push_back(right_trian->pm.constraint_id);
					}
				}
			}
		}
		//拼接中心线 ;
		std::vector<CGeoCurve*> center_shapes;
		for(std::map<int,std::vector<int> >::iterator itr_itm = final_compaires.begin(); itr_itm != final_compaires.end(); ++itr_itm)
		{
			if(itr_itm->first == -1)
				continue;
			//
			std::vector<tin2d::edge2d*> useable_edges;
			//
			int m = 0;
			int n = 0;
			for(int t=0; t<target_lines.size(); ++t)
			{
               if(target_lines[t].flag != 1)
				   continue;
			   //
			   if(target_lines[t].constraint_id == itr_itm->first)
			   {
                   useable_edges.push_back(&target_lines[t]);
				   ++m;
				   continue;
			   }
			   //
			   if(target_lines[t].constraint_id == -1)
			   {
				   useable_edges.push_back(&target_lines[t]);
				   ++n;
				   continue;
			   }  
			   //
			   for(int k=0; k<itr_itm->second.size(); ++k)
			   {
				   if(target_lines[t].constraint_id == itr_itm->second[k])
				   {
					   useable_edges.push_back(&target_lines[t]);
					   ++n;
					   break;
				   }
			   }
			}
			if(n<double(m)/3.0 && constraint_height[itr_itm->first]>average_height)
			{
				for(int t=0; t<target_lines.size(); ++t)
				{
					if(target_lines[t].constraint_id == itr_itm->first)
					{
						target_lines[t].flag = 0;
						continue;
					}
				}
				//
				continue;
			}
			//
L:
			std::vector<tin2d::edge2d> edges;
            for(t=0; t<useable_edges.size(); ++t)
			{
               if(useable_edges[t]->constraint_id == itr_itm->first && useable_edges[t]->flag == 1)
			   {
				   edges.push_back(*useable_edges[t]);
				   useable_edges[t]->flag = 0;
				   break;
			   }
			}
			//
			if(edges.size() == 0)
			{
               continue;
			}
			//
			t=0;
			for(; t<useable_edges.size();)
			{
				if(useable_edges[t]->flag != 1)
				{
					++t;
					continue;
				}		
				//
				bool updated = false;
				//
				if(edges[edges.size()-1].ep == useable_edges[t]->sp)
				{
					edges.push_back(*useable_edges[t]);
				    useable_edges[t]->flag = 0;
					updated = true;
				}
				else if(edges[edges.size()-1].ep == useable_edges[t]->ep)
				{
					tin2d::edge2d temp;
					temp.sp = useable_edges[t]->ep;
					temp.ep = useable_edges[t]->sp;
					edges.push_back(temp);
					useable_edges[t]->flag = 0;
					updated = true;
				}
				else if(edges[0].sp == useable_edges[t]->ep)
				{
                    edges.insert(edges.begin(), *useable_edges[t]);
					useable_edges[t]->flag = 0;
					updated = true;
				}
				else if(edges[0].sp == useable_edges[t]->sp)
				{
					tin2d::edge2d temp;
                    temp.sp = useable_edges[t]->ep;
					temp.ep = useable_edges[t]->sp;
					edges.insert(edges.begin(),temp);
					useable_edges[t]->flag = 0;
					updated = true;
				}
				//
				if(updated)
				  t = 0;
				else
				  ++t;
			}
			//
			CArray<PT_3DEX, PT_3DEX> shape;
			PT_3DEX pa;
			pa.x = edges[0].sp.x;
			pa.y = edges[0].sp.y;
			pa.z = edges[0].sp.z;
			pa.pencode = penLine;
			shape.Add(pa);

			pa.x = edges[0].ep.x;
			pa.y = edges[0].ep.y;
			pa.z = edges[0].ep.z;
			pa.pencode = penLine;
			shape.Add(pa);

			for(t=1; t<edges.size(); ++t)
			{
				pa.x = edges[t].ep.x;
				pa.y = edges[t].ep.y;
				pa.z = edges[t].ep.z;
				pa.pencode = penLine;

				if(fabs(shape[shape.GetSize()-1].x - pa.x)<0.001 && fabs(shape[shape.GetSize()-1].y - pa.y)<0.001)
					continue;
				//
			    shape.Add(pa);
			}

            //
			if(shape.GetSize()<3)
			{
				edges.clear();
			    goto L;
			}
			//
		    CGeoCurve* pcurve = new CGeoCurve();
			BOOL s = pcurve->CreateShape(shape.GetData(),shape.GetSize());
			//pcurve->SetColor(RGB(0,255,0));
			center_shapes.push_back(pcurve);
			//
			edges.clear();
			goto L;
		}
		//
		CUndoFtrs undo(m_pEditor,Name());
		for(int k=0; k<center_shapes.size(); ++k)
		{
			CFeature* pftr = ptmp->Clone();
			pftr->SetGeometry(center_shapes[k]);
			m_pEditor->AddObject(pftr,pdes_layer->GetID());
			undo.AddNewFeature(FtrToHandle(pftr));
		}
		undo.Commit();
		//
		delete ptmp;
		ptmp = NULL;

		//show the tin; 
// 		GrBuffer buf1;
// 		COLORREF color = RGB(100,0,0);
// 		PT_3D pt3d;
// 		buf1.BeginLineString(color,0);
// 		buf1.SetAllColor(RGB(100,0,0));
// 		for(int t=0; t<tin_cgaldelaunay->getTriangleCount(); ++t)
// 		{
// 			tin2d::triangle2d temp_trian = tin_cgaldelaunay->getTriangle(t);
// 			//
// 			
// 			pt3d.x = temp_trian.pl.x;
// 			pt3d.y = temp_trian.pl.y;
// 			pt3d.z = 0;			
// 			buf1.MoveTo(&pt3d);
// 			//
// 			pt3d.x = temp_trian.pm.x;
// 			pt3d.y = temp_trian.pm.y;
// 			pt3d.z = 0;
// 			buf1.LineTo(&pt3d);
// 			//
// 			pt3d.x = temp_trian.pr.x;
// 			pt3d.y = temp_trian.pr.y;
// 			pt3d.z = 0;
// 			buf1.LineTo(&pt3d);
// 			//
// 			pt3d.x = temp_trian.pl.x;
// 			pt3d.y = temp_trian.pl.y;
// 			pt3d.z = 0;			
// 			buf1.LineTo(&pt3d);
// 		}
// 		
// 		buf1.End(FALSE);
// 		
// 		buf1.RefreshEnvelope();
// 		((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_AddGraph,(CObject*)&buf1);	
        
		//show the target lines;
// 		GrBuffer buf2;
//         color = RGB(0,255,0);
// 		buf2.BeginLineString(color,0);
// 		buf2.SetAllColor(color);
// 		for(t=0; t<target_lines.size(); ++t)
// 		{
// 			pt3d.x = target_lines[t].sp.x;
// 			pt3d.y = target_lines[t].sp.y;
// 			pt3d.z = 0;			
// 			buf2.MoveTo(&pt3d);
// 			//
// 			pt3d.x = target_lines[t].ep.x;
// 			pt3d.y = target_lines[t].ep.y;
// 			pt3d.z = 0;
// 			buf2.LineTo(&pt3d);
// 		}
// 		
// 		buf2.End(FALSE);
// 		
// 		buf2.RefreshEnvelope();
// 		((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_AddGraph,(CObject*)&buf2);
		
		//	//
		delete tin_cgaldelaunay;
		tin_cgaldelaunay = NULL;
	}
	//
	Finish();
}

void CInterpolateCenterLoadCommand::Start()
{
	if( !m_pEditor )return;	
	
	m_strBaseFCode = _T("");
	m_strSonFCode = _T("");
	m_bOnlyContour = FALSE;	
	
	m_nMode = wayEdis;
	m_nPart = modeWhole;
	m_nFindRadius = 20;
	m_nStep = 0;
	m_nInNum = 1;
	
	m_lft1.RemoveAll();
	m_lft2.RemoveAll();
	
	m_arrLine.RemoveAll();
	m_arrObjRatio1.RemoveAll();
	m_arrObjRatio2.RemoveAll();
	
	CCommand::Start();
	m_pEditor->CloseSelector();
}

void CInterpolateCenterLoadCommand::SetParams(CValueTable& tab,BOOL bInit)
{
    const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_INTERSONCODE,var) )
	{
		m_strSonFCode = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTERBASECODE,var) )
	{
		m_strBaseFCode = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	//
	CEditCommand::SetParams(tab,bInit);
}

void CInterpolateCenterLoadCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("InterpolateCenterLoadCommand",StrFromResID(IDS_CMDPLANE_INTERPOLATECENTERLINE));
	
	m_nMode = wayEdis;
	
	param->AddLayerNameParamEx(PF_INTERSONCODE,LPCTSTR(m_strSonFCode),StrFromResID(IDS_CMDPLANE_SONCODE));
	param->AddLayerNameParamEx(PF_INTERBASECODE,LPCTSTR(m_strBaseFCode),StrFromResID(IDS_CMDPLANE_BASECODE));
}


//
//////////////////////////////////////////////////////////////////////
// CShowDirCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CShowDirCommand,CEditCommand)

CShowDirCommand::CShowDirCommand()
{

}

CShowDirCommand::~CShowDirCommand()
{

}



CString CShowDirCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SHOWDIR);
}

void CShowDirCommand::Start()
{
 	if( !m_pEditor )return;	
	int nsel;
	m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		
	if( nsel>0 )
	{
		CCommand::Start();
		
		PT_3D pt = m_pEditor->GetCoordWnd().m_ptGrd;
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	m_pEditor->OpenSelector();	
	CCommand::Start();
}

void CShowDirCommand::Abort()
{	
	CCommand::Abort();	
	m_nExitCode = CMPEC_STARTOLD;
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->CloseSelector();
}


void CShowDirCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	m_pEditor->UpdateDrag(ud_ClearDrag, NULL);
	int num ;
	const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num<=0 )return;
	
	double dirlen = 0.0;
	
	PT_4D pts[2];
	pts[0] = pt;
	m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(pts,1);
	pts[1] = pts[0];
	pts[1].x += 10;
	pts[1].y += 10;
	m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pts,2);
	dirlen = sqrt((pts[1].x-pts[0].x)*(pts[1].x-pts[0].x)+(pts[1].y-pts[0].y)*(pts[1].y-pts[0].y));//  关键在这儿实现坐标的转换
	if( dirlen<=0.0 )dirlen = 1.0;
	
	GrBuffer buf;

	CGeometry *pObj;
	
	for(int i=0; i<num; i++)
	{
		pObj = HandleToFtr(handles[i])->GetGeometry();
		if( pObj && pObj->GetDataPointSum()>1 )
		{	
			buf.BeginLineString(pObj->GetColor(),0);
			DrawDir(pObj, dirlen, &buf);
			buf.End();
		}
	}	
	m_pEditor->UpdateDrag(ud_AddConstDrag, &buf);

	GotoState(PROCSTATE_PROCESSING);
	
	CCommand::PtClick(pt,flag);
}

void CShowDirCommand::DrawDir(CGeometry *pObj, double dirlen, GrBuffer *pBuf)
{
	int ptnum = pObj->GetDataPointSum();
	if( ptnum<2 )return;
	
	PT_3D pt;
	PT_3DEX expt1, expt2;
	double dx, dy, dz, len, slen=0;	
	for( int i=0; i<ptnum-1; i++)
	{
		expt1 = pObj->GetDataPoint(i);
		expt2 = pObj->GetDataPoint(i+1);
		
		len = sqrt( (expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y)+
			(expt1.z-expt2.z)*(expt1.z-expt2.z) );
		
		slen += len;
		if( i != 0  && slen<dirlen*20 )continue;
		
		dx = (expt2.x-expt1.x)/len; dy = (expt2.y-expt1.y)/len; dz = (expt2.z-expt1.z)/len;
		
		//中心线起点
		pt.x = expt1.x+dx*(len*0.5-dirlen);
		pt.y = expt1.y+dy*(len*0.5-dirlen);
		pt.z = expt1.z+dz*(len*0.5-dirlen);
		pBuf->MoveTo(&pt);
		
		//中心线终点
		pt.x = expt1.x+dx*(len*0.5);
		pt.y = expt1.y+dy*(len*0.5);
		pt.z = expt1.z+dz*(len*0.5);
		pBuf->LineTo(&pt);
		
		//左箭头起点
		pt.x -= dy*dirlen*0.25;
		pt.y += dx*dirlen*0.25;
		pBuf->MoveTo(&pt);
		
		//箭头顶端
		pt.x = expt1.x+dx*(len*0.5+dirlen);
		pt.y = expt1.y+dy*(len*0.5+dirlen);
		pt.z = expt1.z+dz*(len*0.5+dirlen);
		pBuf->LineTo(&pt);
		
		//右箭头起点
		pt.x = expt1.x+dx*(len*0.5)+dy*dirlen*0.25;
		pt.y = expt1.y+dy*(len*0.5)-dx*dirlen*0.25;
		pt.z = expt1.z+dz*(len*0.5);
		pBuf->LineTo(&pt);
		
		slen = 0;
	}
}



//////////////////////////////////////////////////////////////////////
// CMeasureDistanceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMeasureDistanceCommand,CEditCommand)

CMeasureDistanceCommand::CMeasureDistanceCommand()
{
	m_nStep = -1;
}

CMeasureDistanceCommand::~CMeasureDistanceCommand()
{

}



CString CMeasureDistanceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MEASDIS);
}

void CMeasureDistanceCommand::Start()
{
 	if( !m_pEditor )return;	
	
	m_nStep = 0;

	CCommand::Start();	
	m_pEditor->CloseSelector();

	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)"\n");
}

void CMeasureDistanceCommand::Abort()
{	
	CCommand::Abort();
	m_nStep = -1;

	m_pEditor->UpdateDrag(ud_ClearDrag);
}
 
 
void CMeasureDistanceCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{
		m_ptStart = pt;
		m_nStep = 1;		
		GotoState(PROCSTATE_PROCESSING);
	}
	else if( m_nStep==1 )
	{
		double dis2d = (pt.x-m_ptStart.x)*(pt.x-m_ptStart.x)+(pt.y-m_ptStart.y)*(pt.y-m_ptStart.y);
		double dz = pt.z-m_ptStart.z;
		double dis3d = sqrt(dis2d+dz*dz);
		
		CString str;
		str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), sqrt(dis2d),dz,dis3d);
		
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
		
	//	m_pDoc->SetCurDrawingObj(DrawingInfo());
		m_pEditor->UpdateDrag(ud_ClearDrag);	
		Finish();
		return;
	}
	CCommand::PtClick(pt,flag);
}

DrawingInfo CMeasureDistanceCommand::GetCurDrawingInfo()
{
	if( m_nStep==1 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.Add(PT_3DEX(m_ptStart.x,m_ptStart.y,m_ptStart.z,penLine));
		CFeature *pFtr = new CFeature;
		if(pFtr)
		{
			CGeometry *pObj = (CGeometry*)CPermanent::CreatePermanentObject(CLS_GEOCURVE);//GCreateGeometry(CLS_GEOCURVE);
			if(pObj&&pObj->CreateShape(arrPts.GetData(),arrPts.GetSize()))
			{
				pFtr->SetGeometry(pObj);
				DrawingInfo info(pFtr,arrPts);
				delete pFtr;
				return info;
			}
			delete pFtr;
		}
	}
	return	DrawingInfo();	
}


void CMeasureDistanceCommand::PtMove(PT_3D &pt)
{	
	if( !m_pEditor )return;
	if( m_nStep==1 )
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptStart);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		double dis2d = (pt.x-m_ptStart.x)*(pt.x-m_ptStart.x)+(pt.y-m_ptStart.y)*(pt.y-m_ptStart.y);
		double dz = pt.z-m_ptStart.z;
		double dis3d = sqrt(dis2d+dz*dz);
		
		CString str;
		str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), sqrt(dis2d),dz,dis3d);
		
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
	}
	CCommand::PtMove(pt);	
}

//////////////////////////////////////////////////////////////////////
// CMeasureDisOnMapCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMeasureDisOnMapCommand,CEditCommand)

CMeasureDisOnMapCommand::CMeasureDisOnMapCommand()
{
	m_nStep = -1;
}

CMeasureDisOnMapCommand::~CMeasureDisOnMapCommand()
{

}

CString CMeasureDisOnMapCommand::Name()
{ 
	return StrFromResID(ID_MEASUREDIS_ONMAP);
}

void CMeasureDisOnMapCommand::Start()
{
 	if( !m_pEditor )return;	
	
	m_nStep = 0;

	CCommand::Start();	
	m_pEditor->CloseSelector();

	GOutPut("\n");
	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)"\n");
}

void CMeasureDisOnMapCommand::Abort()
{	
	CCommand::Abort();
	m_nStep = -1;

	m_pEditor->UpdateDrag(ud_ClearDrag);
}
 
 
void CMeasureDisOnMapCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{
		m_ptStart = pt;
		m_nStep = 1;		
		GotoState(PROCSTATE_PROCESSING);
	}
	else if( m_nStep==1 )
	{
		double dis2d = (pt.x-m_ptStart.x)*(pt.x-m_ptStart.x)+(pt.y-m_ptStart.y)*(pt.y-m_ptStart.y);
		double dz = pt.z-m_ptStart.z;
		double dis3d = sqrt(dis2d+dz*dz);
		
		CString str;
		str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), sqrt(dis2d),dz,dis3d);
		
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
		
	//	m_pDoc->SetCurDrawingObj(DrawingInfo());
		m_pEditor->UpdateDrag(ud_ClearDrag);	
		Finish();
		return;
	}
	CCommand::PtClick(pt,flag);
}

DrawingInfo CMeasureDisOnMapCommand::GetCurDrawingInfo()
{
	if( m_nStep==1 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.Add(PT_3DEX(m_ptStart.x,m_ptStart.y,m_ptStart.z,penLine));
		CFeature *pFtr = new CFeature;
		if(pFtr)
		{
			CGeometry *pObj = (CGeometry*)CPermanent::CreatePermanentObject(CLS_GEOCURVE);//GCreateGeometry(CLS_GEOCURVE);
			if(pObj&&pObj->CreateShape(arrPts.GetData(),arrPts.GetSize()))
			{
				pFtr->SetGeometry(pObj);
				DrawingInfo info(pFtr,arrPts);
				delete pFtr;
				return info;
			}
			delete pFtr;
		}
	}
	return	DrawingInfo();	
}
void CMeasureDisOnMapCommand::PtMove(PT_3D &pt)
{	
	if( !m_pEditor )return;
	if( m_nStep==1 )
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptStart);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		double dis = sqrt( (pt.x-m_ptStart.x)*(pt.x-m_ptStart.x)+(pt.y-m_ptStart.y)*(pt.y-m_ptStart.y) );

		int scale = m_pEditor->GetDataSource()->GetScale();
		double disOnMap = dis*1000/scale;

		double zoom = m_pEditor->GetCoordWnd().m_pViewCS->CalcScale();
		CView *pView = GetActiveView();
		if(pView && pView->IsKindOf(RUNTIME_CLASS(CBaseView)))
		{
			zoom = ((CBaseView*)pView)->GetZoomRate();
		}
		double disOnView = disOnMap*zoom;
		
		CString str;
		str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO2), disOnMap, disOnView);
		
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
	}
	CCommand::PtMove(pt);	
}

//////////////////////////////////////////////////////////////////////
// CMeasurePointToLineDistanceCommand Class 点到线串最近距离
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMeasurePointToLineDistanceCommand,CEditCommand)

CMeasurePointToLineDistanceCommand::CMeasurePointToLineDistanceCommand()
{
	m_nStep = -1;
	m_fMindistance2d = MAXDISTANCE;
	m_pObj = NULL;
}

CMeasurePointToLineDistanceCommand::~CMeasurePointToLineDistanceCommand()
{
	


}



CString CMeasurePointToLineDistanceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MEASPTTOLINEDIS);
}

void CMeasurePointToLineDistanceCommand::Start()
{
	if( !m_pEditor )return;	
	
	m_nStep = 0;
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num >0 )
	{
		m_pEditor->DeselectAll();			
		
	}		
//	m_pEditor->OpenSelector(SELMODE_SINGLE);//支持多选
	m_pEditor->CloseSelector();
	CCommand::Start();	
	if (0 == m_nStep )
	{
		PromptString(StrFromResID(IDS_SELECT_POINT));
	}
	else if (1 == m_nStep)
	{
		PromptString(StrFromResID(IDS_SELECT_LINE));
	}
	
	
}

void CMeasurePointToLineDistanceCommand::Abort()
{	
	CCommand::Abort();
	m_nStep = -1;
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}


BOOL CMeasurePointToLineDistanceCommand::GetDistanceFromPointToLine(PT_3D ptA, PT_3D ptB, PT_3D ptC, double *fDistance,PT_3D *rtPT)
{
	//A B 为线串上两点 C 为点地物 计算时A B 高程相同,暂时计算二维平面距离
	double minDistance = 0; 
	PT_3D  AB, AC, BC, BA, BD,D;//向量 ab ac bc
	double AB_E, AC_E, BC_E;
	double r;
	double cosabc,cosbac,sinabc;//<ABC <BAC 夹角
	AB = PT_3D(ptB.x-ptA.x,ptB.y-ptA.y,ptB.z-ptA.z);
	BA = PT_3D(ptA.x-ptB.x,ptA.y-ptB.y,ptA.z-ptB.z);
	AC = PT_3D(ptC.x-ptA.x,ptC.y-ptA.y,ptC.z-ptA.z);
	BC = PT_3D(ptC.x-ptB.x,ptC.y-ptB.y,ptC.z-ptB.z);	
	AB_E = sqrt(AB.x*AB.x+AB.y*AB.y);
	AC_E = sqrt(AC.x*AC.x+AC.y*AC.y);
	BC_E = sqrt(BC.x*BC.x+BC.y*BC.y);	
	cosabc = (BA.x*BC.x + BA.y*BC.y )/(AB_E*BC_E);
	cosbac = (AB.x*AC.x + AB.y*AC.y )/(AB_E*AC_E);
	if (cosabc<0)//余弦值小于0 垂直线在延长线上
	{
		minDistance = BC_E;//返回点到端点线段的值
		if(fDistance) *fDistance = minDistance;
		if(rtPT) *rtPT = ptB;
		return TRUE;
	}

	if (cosbac<0) //同上
	{
		minDistance	= AC_E;
		if(fDistance) *fDistance = minDistance;
		if(rtPT) *rtPT = ptA;
		return TRUE;
	}
	minDistance = BC_E*sqrt(1-cosabc*cosabc);
	r = (BC_E*cosabc/AB_E);
	D  = PT_3D(r*BA.x+ptB.x,r*BA.y+ptB.y,ptB.z);
	if (fDistance) *fDistance = minDistance;
	if (rtPT) *rtPT = D;
	return TRUE;

}



void CMeasurePointToLineDistanceCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor) return;
	const FTR_HANDLE *handles = NULL;
	CArray<PT_3DEX,PT_3DEX> tarrPts;
	double dis3d,dz;//三维距离 高差
	int nsel = 0;
	GrBuffer buf;
	buf.BeginLineString(0,0);
	if (m_nStep == 0)
	{	
	
		m_ptStart = pt;
		m_nStep = 1;	
		m_pEditor->OpenSelector(SELMODE_SINGLE);
		GotoState(PROCSTATE_PROCESSING);
	}
	else if (m_nStep == 1)//选择线串
	{
		PDOC(m_pEditor)->GetSelection()->GetSelectedObjs(nsel);
		if( nsel>0 )
			GotoState(PROCSTATE_PROCESSING);
		/*		m_ptStart = pt;*/
		handles = PDOC(m_pEditor)->GetSelection()->GetSelectedObjs(nsel);
		if (!handles)
		{
			return;
		}
		CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
		if( !pObj )
		{
			return;
		}
		m_arrPts.RemoveAll();
		pObj->GetShape(m_arrPts);//先选取关键点 
		if (m_arrPts.GetSize()>1)//获取选择的线串
		{
			m_arrPts.RemoveAll();
			pObj->GetShape()->GetPts(m_arrPts);
			int nsize = m_arrPts.GetSize();
			double tfmindis = 0;
			PT_3D tptEnd;
			PT_3DEX tPT[2];
			for (int i=0;i<nsize-1;i++)//计算线串上两点到点的最近距离
			{
				tPT[0] = m_arrPts.GetAt(i);
				tPT[1] = m_arrPts.GetAt(i+1);
				
				//获取点对象到线串关键点连线的最短距离
				if (GetDistanceFromPointToLine(tPT[0],tPT[1],m_ptStart,&tfmindis,&tptEnd))
				{
					if ( m_fMindistance2d>tfmindis )
					{
						m_fMindistance2d = tfmindis;
						m_ptEnd = tptEnd;
					}
				}		 
			}
			buf.MoveTo(&m_ptStart);
 			buf.LineTo(&m_ptEnd);
 			buf.End(TRUE);
 			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);

			dz = fabs(m_ptStart.z - tPT[0].z);//高差
			dis3d = sqrt(m_fMindistance2d*m_fMindistance2d+dz*dz);//三维距离
			CString str;
			str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), m_fMindistance2d,dz,dis3d);
			str+=_T("\n");
			m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
				
			Finish();
			return;
		}	
	
	}
			handles = NULL;
			m_arrPts.RemoveAll();
	CCommand::PtClick(pt,flag);
}

void CMeasurePointToLineDistanceCommand::PtMove(PT_3D &pt)
{
	GrBuffer buf;
	if (1 == m_nStep)
	{
		//判断目标地物是否在可以查找的范围内
		CFeature *pOldObj = NULL;
		double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
		PT_3D p1;
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
		Envelope e;
		e.CreateFromPtAndRadius(p1,r);
		pOldObj = m_pEditor->GetDataQuery()->FindNearestObject(p1,r,m_pEditor->GetCoordWnd().m_pSearchCS);	
		if (!pOldObj)
		{
			buf.BeginLineString(0,0);
			buf.MoveTo(&m_ptStart);
			buf.LineTo(&pt);
			buf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
			double dis2d = (pt.x-m_ptStart.x)*(pt.x-m_ptStart.x)+(pt.y-m_ptStart.y)*(pt.y-m_ptStart.y);
			double dz = pt.z;
			double dis3d = sqrt(dis2d+dz*dz);
			
			CString str;
			str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), sqrt(dis2d),dz,dis3d);
			m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
		}	
		if (pOldObj)
		{
			if(  (pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
			{
				m_pObj = pOldObj->GetGeometry();
				if (!m_pObj) return;
				m_arrPts.RemoveAll();
				//	pObj->GetShape()->GetPts(m_arrPts);//获取点对象的坐标
				m_pObj->GetShape(m_arrPts);//先选取关键点 
				if (m_arrPts.GetSize()>1)//获取选择的线串
				{
					m_arrPts.RemoveAll();
					m_pObj->GetShape()->GetPts(m_arrPts);
					int nsize = m_arrPts.GetSize();
					double tfmindis = 0;
					PT_3D tptEnd;
					PT_3DEX tPT[2];
					for (int i=0;i<nsize-1;i++)//计算线串上两点到点的最近距离
					{
						tPT[0] = m_arrPts.GetAt(i);
						tPT[1] = m_arrPts.GetAt(i+1);
						
						//获取点对象到线串关键点连线的最短距离
						if (GetDistanceFromPointToLine(tPT[0],tPT[1],m_ptStart,&tfmindis,&tptEnd))
						{
							if ( m_fMindistance2d>tfmindis )
							{
								m_fMindistance2d = tfmindis;
								m_ptEnd = tptEnd;
							}
						}		 
					}
					buf.BeginLineString(0,0);
					buf.MoveTo(&m_ptStart);
					buf.LineTo(&m_ptEnd);
					buf.End(TRUE);
					double dis3d,dz;//三维距离 高差
					m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
					dz = fabs(m_ptStart.z - tPT[0].z);//高差
					dis3d = sqrt(m_fMindistance2d*m_fMindistance2d+dz*dz);//三维距离
					CString str;
					str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), m_fMindistance2d,dz,dis3d);
					m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
				}	
			}		
		}
		else
		{
			pOldObj = NULL;	
		}
		m_fMindistance2d =MAXDISTANCE;	//完成后给他个最大值
	}

	CCommand::PtMove(pt);
}
//////////////////////////////////////////////////////////////////////
// CMeasureThreePointAngleCommand Class //求三点夹角
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMeasureThreePointAngleCommand,CEditCommand)

CMeasureThreePointAngleCommand::CMeasureThreePointAngleCommand()
{
	m_nStep = -1;

}

CMeasureThreePointAngleCommand::~CMeasureThreePointAngleCommand()
{
	
	
	
}



CString CMeasureThreePointAngleCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MEASTHREEPOINTANGLE);
}

void CMeasureThreePointAngleCommand::Start()
{
	if( !m_pEditor )return;	
	
	m_nStep = 0;
	int num;
	CString str_info;
	str_info.Format(IDS_POINT_SELECT,m_nStep+1);
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num >0 )
	{
		m_pEditor->DeselectAll();			
	}		
	m_pEditor->CloseSelector();
	PromptString(str_info);
	CCommand::Start();	
	
}

void CMeasureThreePointAngleCommand::Abort()
{	
	CCommand::Abort();
	m_nStep = -1;
	m_pEditor->UpdateDrag(ud_ClearDrag);

}

DrawingInfo CMeasureThreePointAngleCommand::GetCurDrawingInfo()
{
	if( m_nStep==1 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.Add(PT_3DEX(m_pt[0].x,m_pt[0].y,m_pt[0].z,penLine));
		CFeature *pFtr = new CFeature;
		if(pFtr)
		{
			CGeometry *pObj = (CGeometry*)CPermanent::CreatePermanentObject(CLS_GEOCURVE);//GCreateGeometry(CLS_GEOCURVE);
			if(pObj&&pObj->CreateShape(arrPts.GetData(),arrPts.GetSize()))
			{
				pFtr->SetGeometry(pObj);
				DrawingInfo info(pFtr,arrPts);
				delete pFtr;
				return info;
			}
			delete pFtr;
		}
	}
	return	DrawingInfo();	
}

void CMeasureThreePointAngleCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor) return;
	const FTR_HANDLE *handles = NULL;
	CArray<PT_3DEX,PT_3DEX> tarrPts;
	int nsel = 0;
	GrBuffer buf;
	CString str_info;
	buf.BeginLineString(0,0);
	if (0 == m_nStep )
	{	

		m_pt[0]= pt;
		m_nStep = 1;
		str_info.Format(IDS_POINT_SELECT,m_nStep+1);
		PromptString(str_info);
	}
	else if (1 == m_nStep )
	{

		m_pt[1]= pt;
		m_nStep = 2;
		str_info.Format(IDS_POINT_SELECT,m_nStep+1);
		PromptString(str_info);
				
	}	
	else if (2 == m_nStep)
	{

		m_pt[2]= pt;
		m_nStep = -1;
		buf.Lines(m_pt,3);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		double ang =GraphAPI::GGetIncludedAngle(m_pt[1],m_pt[0],m_pt[1],m_pt[2]);//获取三点组成滴向量的夹角
		CString str;
		str.Format(IDS_MEAS_ANGLE_INFO, ang*180/PI);
		
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
		m_nStep = -1;
 		GotoState(PROCSTATE_FINISHED);
		Finish();
		handles = NULL;
		return;
	}
	CCommand::PtClick(pt,flag);
}


void CMeasureThreePointAngleCommand::PtMove(PT_3D &pt)
{
	GrBuffer buf;
	PT_3D tpt[3];
	if (1 == m_nStep)
	{
		buf.BeginLineString(0,0);

		tpt[0] = m_pt[0];
		tpt[1] = pt;
		buf.Lines(tpt,2);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
	else if (2 == m_nStep)
	{
		buf.BeginLineString(0,0);
		tpt[0] = m_pt[0];
		tpt[1] = m_pt[1];
		tpt[2] = pt;
		buf.Lines(tpt,3);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);

		double ang =GraphAPI::GGetIncludedAngle(m_pt[1],m_pt[0],m_pt[1],pt);//获取三点组成滴向量的夹角
		CString str;
		str.Format(IDS_MEAS_ANGLE_INFO, ang*180/PI);
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
		
	}
	CCommand::PtMove(pt);

}




//////////////////////////////////////////////////////////////////////
// CMeasureLengthCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMeasureLengthCommand,CEditCommand)

CMeasureLengthCommand::CMeasureLengthCommand()
{
	m_nMode = 0;
	m_nStep = -1;
	m_fLength2d = m_fLength3d = 0;
	m_pDrawProc = NULL;
	m_pGeoCurve = NULL;
}

CMeasureLengthCommand::~CMeasureLengthCommand()
{
	
}



CString CMeasureLengthCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MEASLEN);
}

void CMeasureLengthCommand::Start()
{
	if( !m_pEditor )return;	
	
	m_nStep = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;

	CCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)"\n");
	
	if (m_nMode == 0)
	{
		m_pEditor->OpenSelector(SELMODE_SINGLE);
	}
	else if (m_nMode == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = FALSE;
		m_pEditor->CloseSelector();
		
		return;
	}
		
}

void CMeasureLengthCommand::Finish()
{
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;

	CCommand::Finish();
}

void CMeasureLengthCommand::Abort()
{	
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;
	CCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CMeasureLengthCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)m_nMode;
	tab.AddValue(PF_MEASUREMODE,&CVariantEx(var));
}


void CMeasureLengthCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MeasureLength",StrFromLocalResID(IDS_CMDNAME_MEASLEN));
	
	param->BeginOptionParam(PF_MEASUREMODE,StrFromResID(IDS_CMDDESC_MEASUREMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SELECT),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_COLLECT),1,' ',m_nMode==1);
	param->EndOptionParam();
}



void CMeasureLengthCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_MEASUREMODE,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();
	}
	
	CCommand::SetParams(tab);
}

void CMeasureLengthCommand::PtReset(PT_3D &pt)
{
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
		Abort();
	}
	else
		Abort();
}

void CMeasureLengthCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	if(m_nMode == 0)
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag,FALSE) )return;
			
			int num ;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num!=1 )return;
			
			FTR_HANDLE  handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
			if( !pObj || pObj->GetDataPointSum()<2 )return;

			double dis2d = 0, dz = 0, dis3d = 0;
			const CShapeLine *pShape = pObj->GetShape();
			CArray<PT_3DEX,PT_3DEX> arr;
			pShape->GetPts(arr);
			int size = arr.GetSize();
			for (int i=1; i<arr.GetSize(); i++)
			{
				dis2d += sqrt( (arr[i].x-arr[i-1].x)*(arr[i].x-arr[i-1].x) + (arr[i].y-arr[i-1].y)*(arr[i].y-arr[i-1].y) );
				dis3d += sqrt( (arr[i].x-arr[i-1].x)*(arr[i].x-arr[i-1].x) + (arr[i].y-arr[i-1].y)*(arr[i].y-arr[i-1].y) +
													(arr[i].z-arr[i-1].z)*(arr[i].z-arr[i-1].z) );
			}

			dz = arr[size-1].z - arr[0].z;

			CString str;
			str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), dis2d,dz,dis3d);
			
			m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
			
			m_pEditor->UpdateDrag(ud_ClearDrag);	
			Finish();			

		}

	}
	else if (m_nMode == 1)
	{
		if (m_pDrawProc)
		{
			if( m_nStep==0 )
			{
				m_ptStart = pt;
				m_ptLast = pt;
				m_nStep = 1;		
				GotoState(PROCSTATE_PROCESSING);
			}
			else if( m_nStep==1 )
			{
				double dis2d = (pt.x-m_ptLast.x)*(pt.x-m_ptLast.x)+(pt.y-m_ptLast.y)*(pt.y-m_ptLast.y);
				m_fLength2d += sqrt(dis2d);

				double dz = pt.z-m_ptLast.z;

				double dis3d = sqrt(dis2d+dz*dz);
				m_fLength3d += dis3d;
				
				CString str;
				str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), m_fLength2d,dz,m_fLength3d);
				
				m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));

				m_ptLast = pt;
			}

			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
		}
	}

	//画线
	if (m_nMode == 1)
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}

	CCommand::PtClick(pt,flag);
}

DrawingInfo CMeasureLengthCommand::GetCurDrawingInfo()
{
	if (m_nMode==1)
	{
		CFeature *pFtr = new CFeature;
		if(pFtr)
		{
			CGeometry* pObj = m_pGeoCurve->Clone();
			if(pObj)
			{
				pFtr->SetGeometry(pObj);
				DrawingInfo info(pFtr,m_pDrawProc->m_arrPts);
				delete pFtr;
				return info;
			}
			delete pFtr;
		}
		return DrawingInfo();
	}
	else
		return DrawingInfo();
}
void CMeasureLengthCommand::PtMove(PT_3D &pt)
{	
	if( !m_pEditor )return;
	if( m_nStep==1 && m_nMode == 1)
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptLast);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		double dis2d = (pt.x-m_ptLast.x)*(pt.x-m_ptLast.x)+(pt.y-m_ptLast.y)*(pt.y-m_ptLast.y);
		
		double dz = pt.z-m_ptLast.z;
		
		double dis3d = sqrt(dis2d+dz*dz);
		
		CString str;
		str.Format(ChangeXYPrecesionFormat(IDS_CMDTIP_DISINFO), m_fLength2d+sqrt(dis2d),dz,m_fLength3d+dis3d);
		
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
	}
	CCommand::PtMove(pt);	
}

//////////////////////////////////////////////////////////////////////
// CMeasurePhotoLengthCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMeasurePhotoLengthCommand,CEditCommand)

CMeasurePhotoLengthCommand::CMeasurePhotoLengthCommand()
{
	m_nMode = 0;
	m_nStep = -1;
	m_fLength2d = m_fLength3d = 0;
	m_pDrawProc = NULL;
	m_pGeoCurve = NULL;

	m_fPGScale = m_fPixelScale = 1.0f;
}

CMeasurePhotoLengthCommand::~CMeasurePhotoLengthCommand()
{
	
}



CString CMeasurePhotoLengthCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MEASUREPHOTO);
}

void CMeasurePhotoLengthCommand::Start()
{
	if( !m_pEditor )return;	
	
	m_nStep = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;

	CCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)"\n");

	if (m_nMode == 0)
	{
		m_pEditor->OpenSelector(SELMODE_SINGLE);
	}
	else if (m_nMode == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = FALSE;
		m_pEditor->CloseSelector();

		return;
	}
		
}

void CMeasurePhotoLengthCommand::Finish()
{
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;

	CCommand::Finish();
}

void CMeasurePhotoLengthCommand::Abort()
{	
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;
	CCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

#define PF_PGSCALE		"PGScale"
#define PF_PIXELSCALE	"PixelScale"

void CMeasurePhotoLengthCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)m_nMode;
	tab.AddValue(PF_MEASUREMODE,&CVariantEx(var));

	var = (double)m_fPGScale;
	tab.AddValue(PF_PGSCALE,&CVariantEx(var));

	var = (double)m_fPixelScale;
	tab.AddValue(PF_PIXELSCALE,&CVariantEx(var));
}


void CMeasurePhotoLengthCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MeasurePhotoLength",StrFromLocalResID(IDS_CMDNAME_MEASUREPHOTO));
	
	param->BeginOptionParam(PF_MEASUREMODE,StrFromResID(IDS_CMDDESC_MEASUREMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SELECT),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_COLLECT),1,' ',m_nMode==1);
	param->EndOptionParam();

	param->AddParam(PF_PGSCALE,m_fPGScale,StrFromResID(IDS_CMDPLANE_PGSCALE));
	param->AddParam(PF_PIXELSCALE,m_fPixelScale,StrFromResID(IDS_CMDPLANE_PIXELSCALE));
}



void CMeasurePhotoLengthCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_MEASUREMODE,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();
	}
	if( tab.GetValue(0,PF_PGSCALE,var) )
	{
		m_fPGScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_PIXELSCALE,var) )
	{
		m_fPixelScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CCommand::SetParams(tab);
}

void CMeasurePhotoLengthCommand::PtReset(PT_3D &pt)
{
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
		Abort();
	}
	else
		Abort();
}

void CMeasurePhotoLengthCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	float pixelscale = m_fPGScale*m_fPixelScale*0.001f;

	if(m_nMode == 0)
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag,FALSE) )return;
			
			int num ;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num!=1 )return;
			
			FTR_HANDLE  handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
			if( !pObj || pObj->GetDataPointSum()<2 )return;

			double dis2d = 0, dz = 0, dis3d = 0;
			const CShapeLine *pShape = pObj->GetShape();
			CArray<PT_3DEX,PT_3DEX> arr;
			pShape->GetPts(arr);
			int size = arr.GetSize();
			for (int i=1; i<arr.GetSize(); i++)
			{
				dis2d += sqrt( (arr[i].x-arr[i-1].x)*(arr[i].x-arr[i-1].x) + (arr[i].y-arr[i-1].y)*(arr[i].y-arr[i-1].y) );
				dis3d += sqrt( (arr[i].x-arr[i-1].x)*(arr[i].x-arr[i-1].x) + (arr[i].y-arr[i-1].y)*(arr[i].y-arr[i-1].y) +
													(arr[i].z-arr[i-1].z)*(arr[i].z-arr[i-1].z) );
			}

			dz = arr[size-1].z - arr[0].z;

			CString str;
			str.Format(IDS_CMDTIP_PHOTO_DISINFO, pixelscale,pixelscale*dis2d);
			
			m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
			
			m_pEditor->UpdateDrag(ud_ClearDrag);	
			Finish();			

		}

	}
	else if (m_nMode == 1)
	{
		if (m_pDrawProc)
		{
			if( m_nStep==0 )
			{
				m_ptStart = pt;
				m_ptLast = pt;
				m_nStep = 1;		
				GotoState(PROCSTATE_PROCESSING);
			}
			else if( m_nStep==1 )
			{
				double dis2d = (pt.x-m_ptLast.x)*(pt.x-m_ptLast.x)+(pt.y-m_ptLast.y)*(pt.y-m_ptLast.y);
				m_fLength2d += sqrt(dis2d);

				double dz = pt.z-m_ptLast.z;

				double dis3d = sqrt(dis2d+dz*dz);
				m_fLength3d += dis3d;
				
				CString str;
				str.Format(IDS_CMDTIP_PHOTO_DISINFO, pixelscale,pixelscale*m_fLength2d);
				
				m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));

				m_ptLast = pt;
			}

			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
		}
	}

	//画线
	if (m_nMode == 1)
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}

	CCommand::PtClick(pt,flag);
}

void CMeasurePhotoLengthCommand::PtMove(PT_3D &pt)
{	
	if( !m_pEditor )return;
	float pixelscale = m_fPGScale*m_fPixelScale*0.001f;

	if( m_nStep==1 && m_nMode == 1)
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptLast);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		double dis2d = (pt.x-m_ptLast.x)*(pt.x-m_ptLast.x)+(pt.y-m_ptLast.y)*(pt.y-m_ptLast.y);
		
		double dz = pt.z-m_ptLast.z;
		
		double dis3d = sqrt(dis2d+dz*dz);
		
		CString str;
		str.Format(IDS_CMDTIP_PHOTO_DISINFO, pixelscale,(m_fLength2d+sqrt(dis2d))*pixelscale);
		
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str));
	}
	CCommand::PtMove(pt);	
}



//////////////////////////////////////////////////////////////////////
// CMeasureAngleCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMeasureAngleCommand,CCommand)

CMeasureAngleCommand::CMeasureAngleCommand()
{
	m_nStep = -1;
	memset(m_linesection,0,sizeof(m_linesection));
}

CMeasureAngleCommand::~CMeasureAngleCommand()
{

}



CString CMeasureAngleCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MEASANG);
}

void CMeasureAngleCommand::Start()
{
	if( m_pEditor )
	{
		m_nStep = 0;
		int num;
		m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num >0 )
		{
			m_pEditor->DeselectAll();			
			
		}		
		m_pEditor->OpenSelector();
	}	
	CCommand::Start();
}

void CMeasureAngleCommand::Abort()
{	
	CCommand::Abort();	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_nStep = -1;
}


void CMeasureAngleCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	int num ;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num<=0 )return;
	
	GrBuffer buf;
	
	CGeometry *pObj;
	PT_3D ret1,ret2;
	
	double len = 0.0,temp;
	
	PT_4D pts[2];
	pts[0] = pt;
	m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(pts,1);
	pts[1] = pts[0];
	pts[1].x += 20;
	pts[1].z += 20;
	m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pts,2);
	len =2* sqrt((pts[1].x-pts[0].x)*(pts[1].x-pts[0].x)+(pts[1].y-pts[0].y)*(pts[1].y-pts[0].y));//  关键在这儿实现坐标的转换
	if( len<=0.0 )len = 1.0;
	
	if( m_nStep==0 )
	{		 
		FTR_HANDLE handle = m_pEditor->GetSelection()->GetLastSelectedObj();
		PT_3D pt1;
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&pt1);
		Envelope e;
		e.CreateFromPtAndRadius(pt1,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS());
	
		
		pObj = HandleToFtr(handle)->GetGeometry();
		if( pObj && pObj->GetDataPointSum()>1 )
		{
			if(pObj->FindNearestBaseLine(pt1, e, m_pEditor->GetCoordWnd().m_pSearchCS, &ret1, &ret2,NULL))
			{
				if ((temp=sqrt((ret2.x-ret1.x)*(ret2.x-ret1.x)+(ret2.y-ret1.y)*(ret2.y-ret1.y)+(ret2.z-ret1.z)*(ret2.z-ret1.z)))<len)
				{
					double dx, dy, dz;
					dx=(ret2.x-ret1.x)/temp;dy=(ret2.y-ret1.y)/temp;dz=(ret2.z-ret1.z)/temp;
					
					m_linesection[0].sp.x=(ret1.x+ret2.x)/2+dx*len/2;
					m_linesection[0].sp.y=(ret1.y+ret2.y)/2+dy*len/2;
					m_linesection[0].sp.z=(ret1.z+ret2.z)/2+dz*len/2;
					m_linesection[0].ep.x=(ret1.x+ret2.x)/2-dx*len/2;
					m_linesection[0].ep.y=(ret1.y+ret2.y)/2-dy*len/2;
					m_linesection[0].ep.z=(ret1.z+ret2.z)/2-dz*len/2;
				}
				else
				{
					m_linesection[0].sp=ret1;
					m_linesection[0].ep=ret2;
					
				}
				m_ptclick1 = pt;
				buf.BeginLineString(0,4,FALSE);
				//设备坐标的转换暂且搁下
				buf.MoveTo(&(m_linesection[0].sp));
				buf.LineTo(&(m_linesection[0].ep));
				buf.End();
				GotoState(PROCSTATE_PROCESSING);				
			}			
		}		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		m_nStep++;
		
	}
	else if( m_nStep==1 )
	{
		FTR_HANDLE handle = m_pEditor->GetSelection()->GetLastSelectedObj();
		PT_3D pt1;
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&pt1);
		Envelope e;
		e.CreateFromPtAndRadius(pt1,m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS());

		pObj = HandleToFtr(handle)->GetGeometry();

		if( pObj && pObj->GetDataPointSum()>1 )
		{
			if(pObj->FindNearestBaseLine(pt1, e, m_pEditor->GetCoordWnd().m_pSearchCS, &ret1, &ret2,NULL))
			{
				if ((temp=sqrt((ret2.x-ret1.x)*(ret2.x-ret1.x)+(ret2.y-ret1.y)*(ret2.y-ret1.y)+(ret2.z-ret1.z)*(ret2.z-ret1.z)))<len)
				{
					double dx, dy, dz;
					dx=(ret2.x-ret1.x)/temp;dy=(ret2.y-ret1.y)/temp;dz=(ret2.z-ret1.z)/temp;
					
					m_linesection[1].sp.x=(ret1.x+ret2.x)/2+dx*len/2;
					m_linesection[1].sp.y=(ret1.y+ret2.y)/2+dy*len/2;
					m_linesection[1].sp.z=(ret1.z+ret2.z)/2+dz*len/2;
					m_linesection[1].ep.x=(ret1.x+ret2.x)/2-dx*len/2;
					m_linesection[1].ep.y=(ret1.y+ret2.y)/2-dy*len/2;
					m_linesection[1].ep.z=(ret1.z+ret2.z)/2-dz*len/2;
				}
				else
				{
					m_linesection[1].sp=ret1;
					m_linesection[1].ep=ret2;
					
				}

				PT_3D ret3;
				if( GraphAPI::GGetLineIntersectLineSeg(m_linesection[0].sp.x, m_linesection[0].sp.y, m_linesection[0].ep.x,
								m_linesection[0].ep.y, m_linesection[1].sp.x, m_linesection[1].sp.y, m_linesection[1].ep.x,
								m_linesection[1].ep.y, &ret3.x, &ret3.y, NULL, NULL))
				{
					if(GraphAPI::GIsPtInRange(m_linesection[0].sp, ret3, m_ptclick1))
					{
						m_linesection[0].ep = m_linesection[0].sp;
						m_linesection[0].sp = ret3;
					}
					else if(GraphAPI::GIsPtInRange(m_linesection[0].ep, ret3, m_ptclick1))
					{
						m_linesection[0].sp = ret3;
					}

					if(GraphAPI::GIsPtInRange(m_linesection[1].sp, ret3, pt))
					{
						m_linesection[1].ep = m_linesection[1].sp;
						m_linesection[1].sp = ret3;
					}
					else if(GraphAPI::GIsPtInRange(m_linesection[1].ep, ret3, pt))
					{
						m_linesection[1].sp = ret3;
					}
				}
		
				buf.BeginLineString(0,4,FALSE);
				//设备坐标的转换暂且搁下
				buf.MoveTo(&(m_linesection[1].sp));
				buf.LineTo(&(m_linesection[1].ep));
				buf.MoveTo(&(m_linesection[0].sp));
				buf.LineTo(&(m_linesection[0].ep));
				buf.End();
				GotoState(PROCSTATE_PROCESSING);
				
			}			
		}					
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		m_nStep++;	
		m_pEditor->CloseSelector();
        
	}
	else
	{
		double ang=GraphAPI::GGetIncludedAngle(m_linesection[0].sp,m_linesection[0].ep,m_linesection[1].sp,m_linesection[1].ep);
		m_nStep=-1;
		CString str;
		str.Format(IDS_CMDTIP_DISINFO1, ang*180/PI,ang);
		m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)(str));
		
	//	m_pDoc->SetCurDrawingObj(DrawingInfo());
		m_pEditor->UpdateDrag(ud_ClearDrag);		
		Finish();		
	}	
	CCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CMeasureAreaCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMeasureAreaCommand,CEditCommand)

CMeasureAreaCommand::CMeasureAreaCommand()
{
	m_nMode = 0;
	m_nStep = -1;
	m_pDrawProc = NULL;
	m_pGeoCurve = NULL;
}

CMeasureAreaCommand::~CMeasureAreaCommand()
{

}

CString CMeasureAreaCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MEASAREA);
}

void CMeasureAreaCommand::Start()
{
	if( !m_pEditor )return;	

	m_nStep = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;

	CCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)"\n");


	if (m_nMode == 0)
	{
		int num;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if(num >0 )
		{	
			TipArea(handles[0]);
			return;
		}

		m_pEditor->OpenSelector(SELMODE_SINGLE);
	}
	else if (m_nMode == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = FALSE;
		m_pEditor->CloseSelector();
		return;
	}

}

void CMeasureAreaCommand::Finish()
{
	CCommand::Finish();

	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;
	
		
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CMeasureAreaCommand::Abort()
{	
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;

	CCommand::Abort();	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
}

void CMeasureAreaCommand::PtMove(PT_3D &pt)
{	
	if( !m_pEditor )return;
	if (m_nMode == 1 && m_nStep == 1)
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptLast);
		buf.LineTo(&pt);
		
		CFeature *pFtr = new CFeature;
		if (!pFtr) 
		{
			Abort();
			return;
		}

		CGeoCurve *pCurve = (CGeoCurve*)m_pGeoCurve->Clone();
		if (pCurve)
		{
			const CShapeLine  *pSL = pCurve->GetShape();
			CArray<PT_3DEX,PT_3DEX> pts;
			pSL->GetPts(pts);
			if (pts.GetSize() > 1)
			{
				buf.MoveTo(&m_ptStart);
				buf.LineTo(&pt);
			}
			pts.Add(PT_3DEX(pt,penLine));

			pCurve->CreateShape(pts.GetData(),pts.GetSize());
		}

		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
		pFtr->SetGeometry(pCurve);

		TipArea(FTR_HANDLE(pFtr));
		
		delete pFtr;	
	}
	CCommand::PtMove(pt);	
}

void CMeasureAreaCommand::TipArea(FTR_HANDLE handle)
{
	CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
	if( !pObj )return;
	
	double area = 0;
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||
		pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		const CShapeLine  *pSL = pObj->GetShape();
		if( pSL )
		{
			CArray<PT_3DEX,PT_3DEX> pts;
			pSL->GetPts(pts);
			int npt = pts.GetSize();
			if( npt>2 )
			{	
				PT_3D *arr = new PT_3D[npt];
				
				if( arr )
				{
					for (int i=0; i<npt; i++)
					{
						COPY_3DPT(arr[i],pts[i]);
					}
					area = GraphAPI::GGetPolygonArea(arr,npt);
					delete[] arr;
				}
				
// 				for( int i=0; i<npt-1; i++)
// 				{
// 					area += ((pts[i].y+pts[i+1].y)*(pts[i+1].x-pts[i].x)*0.5);					
// 				}
// 				area += ((pts[npt-1].y+pts[0].y)*(pts[npt-1].x-pts[0].x)*0.5);
// 				area = fabs(area);				
			}
		}
	}	
	CString str0,str1;
	str0.Format("%.4f",area);
	str1.Format(IDS_CMDTIP_AREA,str0);
	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG_OLDLINE, (LONG_PTR)(LPCTSTR)(str1));
}

void CMeasureAreaCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)m_nMode;
	tab.AddValue(PF_MEASUREMODE,&CVariantEx(var));
}


void CMeasureAreaCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MeasureArea",StrFromLocalResID(IDS_CMDNAME_MEASAREA));
	
	param->BeginOptionParam(PF_MEASUREMODE,StrFromResID(IDS_CMDDESC_MEASUREMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SELECT),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_COLLECT),1,' ',m_nMode==1);
	param->EndOptionParam();
}



void CMeasureAreaCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_MEASUREMODE,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();
	}
	
	CCommand::SetParams(tab);
}

VOID CMeasureAreaCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	if(m_nMode == 0)
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag,FALSE) )return;
			
			int num ;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);

			if( num<=0 )return;	
			TipArea(handles[0]);
				
			Finish();
			
		}
		
	}
	else if (m_nMode == 1)
	{
		if (m_pDrawProc)
		{
			m_pDrawProc->PtClick(pt,flag);

			if (m_nStep == 0)
			{
				m_ptStart = pt;
			}
			m_ptLast = pt;
			m_nStep = 1;

// 			CFeature *pFtr = new CFeature;
// 			if (!pFtr) 
// 			{
// 				Abort();
// 				return;
// 			}			
// 
// 			CGeoCurve *pCurve = (CGeoCurve*)m_pGeoCurve->Clone();
// 
// 			pFtr->SetGeometry(pCurve);
// 			TipArea(FTR_HANDLE(pFtr));
// 
// 			delete pFtr;

			GrBuffer buf;
			buf.BeginLineString(0,0);
			buf.MoveTo(&m_ptStart);
			buf.LineTo(&pt);
			buf.End();

			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
			
			GotoState(PROCSTATE_PROCESSING);
			
		}
	}
	
	//画线
	if (m_nMode == 1)
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}

	CCommand::PtClick(pt,flag);
}



//////////////////////////////////////////////////////////////////////
// CMeasureImageCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMeasureImageCommand,CCommand)

CMeasureImageCommand::CMeasureImageCommand()
{
	m_nStep = -1;
}

CMeasureImageCommand::~CMeasureImageCommand()
{
}



CCommand* CMeasureImageCommand::Create()
{
	return new CMeasureImageCommand;
}

CString CMeasureImageCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SETIMAGEPOS);
}

void CMeasureImageCommand::Start()
{
	m_nStep = 0;

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_Attach_Accubox);	
	m_pEditor->UpdateDrag(ud_ClearDrag);

	CCommand::Start();

	GOutPut(StrFromResID(IDS_TIP_ADJUSTIMAGE));

	CString str;
	str.Format(StrFromResID(IDS_CMDTIP_SELIMGPT),1);

	GOutPut(str);
}


void CMeasureImageCommand::Abort()
{
	if( PDOC(m_pEditor) )
	{
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_ClearDragLine);
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Detach_Accubox);
	}
	
	m_nStep = -1;
	CCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;

}



void CMeasureImageCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
}


void CMeasureImageCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("AdjustImagePos",StrFromResID(IDS_CMDNAME_SETIMAGEPOS));

	m_arrFileNames.RemoveAll();
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_GetOverlayImgNames,&m_arrFileNames);

	if( bForLoad && m_arrFileNames.GetSize()>0 )
	{
		m_strFileName = m_arrFileNames[0];
	}
	
	param->BeginOptionParam("FileName",StrFromResID(IDS_TARGETIMG));

	for( int i=0; i<m_arrFileNames.GetSize(); i++)
	{
		param->AddOption(m_arrFileNames[i],i,' ',(i==0));
	}

	param->EndOptionParam();

	if( !m_strFileName.IsEmpty() )
	{
		for( i=0; i<m_arrFileNames.GetSize(); i++)
		{
			if( m_strFileName.CompareNoCase(m_arrFileNames[i])==0 )
			{
				param->SetOptionDefault("FileName",i);
				break;
			}
		}		
	}
}



void CMeasureImageCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"FileName",var) )
	{
		int index = (long)(_variant_t)*var;
		if( index>=0 && index<m_arrFileNames.GetSize() )
			m_strFileName = m_arrFileNames[index];

		SetSettingsModifyFlag();
	}

	CCommand::SetParams(tab);
}

void CMeasureImageCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		m_arrPts.Add(pt);

		int nsz = m_arrPts.GetSize();

		if( (nsz%2)==0 && nsz>0 )
		{
			GrBuffer buf;
			buf.BeginLineString(0,0);
			buf.MoveTo(&m_arrPts[nsz-2]);
			buf.LineTo(&m_arrPts[nsz-1]);
			buf.End();
			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_AddConstDragLine,(CObject*)&buf);
		}

		CString str;
		if( (nsz%2)==0 )
			str.Format(StrFromResID(IDS_CMDTIP_SELIMGPT),nsz/2+1);
		else
			str.Format(StrFromResID(IDS_CMDTIP_SELGRDPT),nsz/2+1);

		GOutPut(str);

		GotoState(PROCSTATE_PROCESSING);
	}
		
	CCommand::PtClick(pt,flag);
}


void CMeasureImageCommand::PtReset(PT_3D &pt)
{
	if( m_arrPts.GetSize()>=2 )
	{		
		double m[9];
		matrix_toIdentity(m,3);
		int nsz1 = m_arrPts.GetSize();
		PT_3D *pts = m_arrPts.GetData();
		if( nsz1<6 )
		{
			m[2] = pts[1].x - pts[0].x;
			m[5] = pts[1].y - pts[0].y;
		}
		else
		{				
			double *buf = new double[nsz1*4];
			double *x1 = buf, *y1 = buf+nsz1;
			double *x2 = y1+nsz1, *y2 = y1+nsz1+nsz1;
			
			for( int i=0, j=0; i<nsz1; i+=2, j++)
			{
				x1[j] = pts[i].x; y1[j] = pts[i].y;
				x2[j] = pts[i+1].x; y2[j] = pts[i+1].y;
			}
			
			CalcAffineParams(x1,y1,x2,y2,j,m,m+3);				
			
			delete[] buf;
		}

		strcpy(m_SaveImgPos.fileName,(LPCTSTR)m_strFileName);

		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_GetImagePosition,(CObject*)&m_SaveImgPos);

		ViewImgPosition item = m_SaveImgPos;
		
		CUndoAdjustImagePosition undo(m_pEditor,Name());
		undo.oldImgPos = item;
		undo.newImgPos = item;

		item.ConvertToMatrixType();
		double m2[9];
		matrix_multiply(m,item.lfDataMatrix,3,m2);

		undo.newImgPos = item;		
		memcpy(undo.newImgPos.lfDataMatrix,m2,sizeof(undo.newImgPos.lfDataMatrix));
		undo.newImgPos.lfDataMatrix[4] = fabs(undo.newImgPos.lfDataMatrix[4]);//可能不合理
		undo.Commit();
		
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetImagePosition,(CObject*)&undo.newImgPos);

		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_ClearDragLine);	
		Finish();
		m_nExitCode = CMPEC_STARTOLD;
	}

	CCommand::PtReset(pt);
}

void CMeasureImageCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==0 && (m_arrPts.GetSize()%2)==1  )
	{
		int nsz = m_arrPts.GetSize();

		PT_3D ptt;
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_arrPts[nsz-1]);
		buf.LineTo(&pt);
		buf.End();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)&buf);
	}
	
	CCommand::PtMove(pt);
}




//////////////////////////////////////////////////////////////////////
// CCellDefineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCellDefineCommand,CEditCommand)

CCellDefineCommand::CCellDefineCommand()
{
	m_nStep = -1;
	m_strCellDefName = "cell";
	m_CellDef.m_pgr = NULL;	
}

CCellDefineCommand::~CCellDefineCommand()
{
	
}

CString CCellDefineCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DEFCELL);
}

void CCellDefineCommand::Start()
{
	if( !m_pEditor )return;	
	
	// 
	CEditCommand::Start();
}

void CCellDefineCommand::Abort()
{	
	CCommand::Abort();	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
}

void CCellDefineCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) ) return;
		EditStepOne();
		GOutPut(StrFromResID(IDS_CELL_SETORIGIN));
	}
	else if( m_nStep==1 )
	{
		if (IsCellExist(m_strCellDefName))
		{
			if (AfxMessageBox(IDS_CELLDEF_OVERLAYOK,MB_YESNO) == IDNO)
			{
				Abort();
				return;
			}
			
		}

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		ConfigLibItem config = gpCfgLibMan->GetConfigLibItemByScale(pDS->GetScale());
		CCellDefLib *pCellLib = config.pCellDefLib;
		
		// 创建单元
		m_CellDef = pCellLib->GetCellDef(m_strCellDefName);
		if (m_CellDef.m_pgr==NULL)
		{
			m_CellDef.m_pgr = new GrBuffer2d();
			strcpy(m_CellDef.m_name,LPCTSTR(m_strCellDefName));
		}
		m_CellDef.m_pgr->DeleteAll();
		int nsel;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		// 生成图形基本单位
		for (int i=0;i<nsel;i++)
		{
			CFeature *pFtr = HandleToFtr(handles[i]);
			if (!pFtr) continue;
			GrBuffer buf;
			pFtr->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_CellDef.m_pgr->AddBuffer(&buf);
		}
		m_CellDef.m_pgr->Move(-pt.x,-pt.y);
		m_CellDef.m_pgr->RefreshEnvelope();
		
		// 将生成的单元加入符号库
		int idx = pCellLib->AddCellDef(m_CellDef);
		config.SaveCellLine();
		// 更新主界面符号索引界面
		AfxGetMainWnd()->SendMessage(FCCM_SYMBOLLIBVIEW,WPARAM(0),
			LPARAM(pCellLib));
		
		m_nStep = 2;
		Finish();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		
	}
	CCommand::PtClick(pt,flag);
}

void CCellDefineCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	var = (_bstr_t)(m_strCellDefName);
	tab.AddValue(CELLDEFNAME,&CVariantEx(var));
}


void CCellDefineCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CellDefCommand",StrFromResID(IDS_CMDPLANE_CELLNAME));
	param->AddParam(CELLDEFNAME,(LPCTSTR)m_strCellDefName,StrFromResID(IDS_CMDPLANE_CELLNAME));
}



void CCellDefineCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,CELLDEFNAME,var) )
	{
		m_strCellDefName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		if (!bInit && IsCellExist(m_strCellDefName))
		{
			AfxMessageBox(IDS_CELLDEF_OVERLAY);
		}
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CCheckPLErrorCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCheckPLErrorCommand,CEditCommand)

CCheckPLErrorCommand::CCheckPLErrorCommand()
{
	m_strCheckFCode = _T("");
	m_fContourInter = 5.0;
	m_nStep = -1;
	m_lfMarkWid = 10;
	m_fMinDZ = 0;
	m_nIntensity = 1;
	m_nBoundType = 1;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	strcat(m_strRegPath,"\\CheckPLError");
}

CCheckPLErrorCommand::~CCheckPLErrorCommand()
{
}

CCommand* CCheckPLErrorCommand::Create()
{
	return new CCheckPLErrorCommand;
}

CString CCheckPLErrorCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CHECKPL);
}


void CCheckPLErrorCommand::Start()
{
	if( !m_pEditor )return;
	
	m_strCheckFCode = _T("");
	m_lfMarkWid = 10;
	m_fContourInter = 5.0;
	m_fMinDZ = 0;
	m_nIntensity = 10;
	m_nBoundType = 1;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	
	m_nStep = 0;
	CEditCommand::Start();

	// 全图
	if (m_nBoundType == 0)
	{
		m_pEditor->CloseSelector();
	}
	// 选择
	else if (m_nBoundType == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		UpdateParams(FALSE);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = TRUE;
		m_pEditor->CloseSelector();
		return;
	}

	
}

void CCheckPLErrorCommand::Abort()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CCheckPLErrorCommand::Finish()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CCheckPLErrorCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(StrFromResID(IDS_CMDNAME_CHECKPL),StrFromResID(IDS_CMDNAME_CHECKPL));

	param->BeginOptionParam(PF_CHECKBOUND,StrFromResID(IDS_CMDPLANE_CHECKBOUND));
	param->AddOption(StrFromResID(IDS_WHOLEMAP),0,' ',m_nBoundType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),1,' ',m_nBoundType==1);
	param->EndOptionParam();

	param->AddLayerNameParamEx("ContourFCode",(LPCTSTR)m_strContourFCode, StrFromResID(IDS_CMDPLANE_CONTOURCODE),NULL,LAYERPARAMITEM_LINE);
	
	param->AddParam(CMDPLANE_CONTOURINTER,m_fContourInter, StrFromResID(IDS_CMDPLANE_CONTOURINTER));
	param->AddLayerNameParam(CMDPLANE_CHECKPCODE,(LPCTSTR)m_strCheckFCode, StrFromResID(IDS_CMDPLANE_CHECKPCODE),NULL,LAYERPARAMITEM_POINT);
	param->AddParam(CMDPLANE_PLMINDZ,m_fMinDZ, StrFromResID(IDS_CMDPLANE_PLMINDZ));
	
	
}

void CCheckPLErrorCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_CHECKBOUND,var) )
	{					
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}
	if( tab.GetValue(0,CMDPLANE_CONTOURINTER,var) )
	{
		m_fContourInter = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	if( tab.GetValue(0,"ContourFCode",var) )
	{
		m_strContourFCode = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,CMDPLANE_CHECKPCODE,var) )
	{
		m_strCheckFCode = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,CMDPLANE_PLMINDZ,var) )
	{
		m_fMinDZ = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	if( tab.GetValue(0,CMDPLANE_INDENSITY,var) )
	{
		m_nIntensity = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CCheckPLErrorCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = (long)(m_nBoundType);
	tab.AddValue(PF_CHECKBOUND,&CVariantEx(var));
	var = m_fContourInter;
	tab.AddValue(CMDPLANE_CONTOURINTER,&CVariantEx(var));
	var = (_bstr_t)m_strContourFCode;
	tab.AddValue("ContourFCode",&CVariantEx(var));
	var = (_bstr_t)m_strCheckFCode;
	tab.AddValue(CMDPLANE_CHECKPCODE,&CVariantEx(var));
	var = m_fMinDZ;
	tab.AddValue(CMDPLANE_PLMINDZ,&CVariantEx(var));
	var = (_variant_t)(long)m_nIntensity;
	tab.AddValue(CMDPLANE_INDENSITY,&CVariantEx(var));
}

BOOL CCheckPLErrorCommand::FilterFeature(LONG_PTR id)
{
	CFeature *pFtr = (CFeature*)id;
	CDlgDataSource  *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if(pDS) 
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return FALSE;	
		if(pLayer->IsLocked()) return FALSE;
		if ((!pLayer->IsVisible()||!pFtr->IsVisible())) return FALSE;
		CGeometry *po = pFtr->GetGeometry();
		if( !po )return FALSE;
		if( !po->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return FALSE;
		if( !CheckNameForLayerCode(pDS,pLayer->GetName(),m_strContourFCode) )
			return FALSE;

		if( !CContourValidChkCmd::CheckObjForZ(po) )return FALSE;

	}
	return TRUE;
}

CProcedure *CCheckPLErrorCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nBoundType==1&&(nMsgType==msgEnd||nMsgType==msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}

void CCheckPLErrorCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}

void CCheckPLErrorCommand::OnSonEnd(CProcedure *son)
{
	if( m_nBoundType==1 )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
		
		CEditCommand::OnSonEnd(son);
	}
}

void CCheckPLErrorCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nBoundType == 0)
	{
		m_nStep = 2;
	}
	else if (m_nBoundType == 1)
	{
		if (m_pDrawProc)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
		}
	}
   
	if( m_nStep==2 )
	{
		BOOL bCheckCode = (m_strCheckFCode.GetLength()>0);
		if( !bCheckCode )
		{
			GOutPut(StrFromResID(IDS_CMDERR_CHECKCODE));
			return;
		}

		if (m_nBoundType == 0)
		{
			CFeature *pFtr;	
			CFtrLayer *pLayer;
			
			int nLayNum = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerCount(), i , j, lSum = 0;
			for( i=0; i<nLayNum; i++)
			{
				pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
				if( !pLayer )continue;
				
				if( bCheckCode && !CheckNameForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pLayer->GetName(),m_strCheckFCode) )
					continue;
				
				int nObjNum = pLayer->GetObjectCount();
				for( j=0; j<nObjNum; j++)
				{
					pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;
					
					if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
						continue;
					
					lSum++;
				}
			}
			
			if( lSum<=0 )
			{
				GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
				return;
			}
			CDlgDataQuery* pQuery = (CDlgDataQuery*)PDOC(m_pEditor)->GetDataQuery();
			CDlgDataQuery::FilterFeatureItem oldItem;
			CDlgDataQuery::FilterFeatureItem newItem((CSearcherFilter*)this, (PFilterFunc)&CCheckPLErrorCommand::FilterFeature);
			oldItem = pQuery->SetFilterFeature(newItem);
			GProgressStart(lSum);
			
			CCPResultList result;
			result.attrNameList.Add(StrFromResID(IDS_CMDNAME_CHECKPL));
			for( i=0; i<nLayNum; i++)
			{
				pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
				if( !pLayer )continue;
				
				if( bCheckCode && !CheckNameForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pLayer->GetName(),m_strCheckFCode) )
					continue;
				
				int nObjNum = pLayer->GetObjectCount();
				for( j=0; j<nObjNum; j++)
				{
					pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;
					
					if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
						continue;
					
					result.StartRecord();
					result.SetCurFtr(pFtr);
					result.SetCurAttr(0);
					CheckObj(pFtr,&result);
					result.FinishRecord();
					
					GProgressStep();
				}
			}
			pQuery->SetFilterFeature(oldItem);
			
			GProgressEnd();
			
			AfxGetMainWnd()->SendMessage(FCCM_SEARCHRESULT,2,(LPARAM)&result);
		}
		else if (m_nBoundType == 1)
		{
			int i, lSum = 0;

			const CShapeLine *pBase  = m_pGeoCurve->GetShape();
			Envelope e = pBase->GetEnvelope();

			e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);
			int num;
			const CPFeature *ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);	
			
			CArray<CPFeature,CPFeature> arrFtrs;

			for ( i=0; i<num; i++)
			{
				CFeature *pFtr = ftr[i];
				CFtrLayer *pLayer = GETDS(m_pEditor)->GetFtrLayerOfObject(pFtr);
				if (!pLayer) continue;

				if( bCheckCode && !CheckNameForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pLayer->GetName(),m_strCheckFCode) )
					continue;

				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
					continue;
				
				lSum++;
				arrFtrs.Add(pFtr);
			}
			
			if( lSum<=0 )
			{
				GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
				return;
			}
			CDlgDataQuery* pQuery = (CDlgDataQuery*)PDOC(m_pEditor)->GetDataQuery();
			CDlgDataQuery::FilterFeatureItem oldItem;
			CDlgDataQuery::FilterFeatureItem newItem((CSearcherFilter*)this, (PFilterFunc)&CCheckPLErrorCommand::FilterFeature);
			oldItem = pQuery->SetFilterFeature(newItem);
			GProgressStart(lSum);
			
			CCPResultList result;
			result.attrNameList.Add(StrFromResID(IDS_CMDNAME_CHECKPL));

			for ( i=0; i<lSum; i++)
			{
				CFeature *pFtr = arrFtrs[i];
				
				result.StartRecord();
				result.SetCurFtr(pFtr);
				result.SetCurAttr(0);
				CheckObj(pFtr,&result);
				result.FinishRecord();
				
				GProgressStep();
			}

			pQuery->SetFilterFeature(oldItem);
			
			GProgressEnd();
			
			AfxGetMainWnd()->SendMessage(FCCM_SEARCHRESULT,2,(LPARAM)&result);
		}
		
		Finish();
		m_nStep = 2;
	}

	//画线
	if( m_nBoundType==1 )
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}
	
	CEditCommand::PtClick(pt, flag);
}


void CCheckPLErrorCommand::CheckObj(CFeature *pFtr, CCPResultList *result)
{
	PT_3DEX expt;
	PT_3D pt3d[8];
	Envelope e;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds1, arrIds2;
	
	CGeometry *pObj = pFtr->GetGeometry();
	expt = pObj->GetDataPoint(0);
	
	CTree8Search * pSearch = (CTree8Search *)PDOC(m_pEditor)->GetDataQuery()->GetActiveSearcher();
	double r;
	int  nObj = ((CDlgDataQuery*)(PDOC(m_pEditor)->GetDataQuery()))->FindNearestObjectByObjNum(expt,5,NULL,TRUE,FALSE,&r);
	if( nObj<2 )
		return;

	Envelope e0 = pSearch->GetRegion();
	double dx,dy;
	const CPFeature *ftr = PDOC(m_pEditor)->GetDataQuery()->GetFoundHandles(nObj);
	nObj = nObj<=5?nObj:5;
	
	dx = r; dy = r;

	pt3d[0].x = expt.x-dx; pt3d[0].y = expt.y-dy; pt3d[0].z = expt.z;
	pt3d[1].x = expt.x+dx; pt3d[1].y = expt.y+dy; pt3d[1].z = expt.z;
	pt3d[2].x = expt.x-dx; pt3d[2].y = expt.y+dy; pt3d[2].z = expt.z;
	pt3d[3].x = expt.x+dx; pt3d[3].y = expt.y-dy; pt3d[3].z = expt.z;
	
	pt3d[4].x = expt.x-dx; pt3d[4].y = expt.y; pt3d[4].z = expt.z;
	pt3d[5].x = expt.x+dx; pt3d[5].y = expt.y; pt3d[5].z = expt.z;
	pt3d[6].x = expt.x; pt3d[6].y = expt.y+dy; pt3d[6].z = expt.z;
	pt3d[7].x = expt.x; pt3d[7].y = expt.y-dy; pt3d[7].z = expt.z;
	
	//找到八个方向(四条线段)下获得地物的数目最多的一个方向
	int nfind1 = 0;
	for( int n=0; n<4; n++)
	{
		arrIds2.RemoveAll();
		int nfind2;
		{
			CFeature *pFtr0 = NULL;
//			GrBuffer buf;
//			Graph *pGraph = NULL;
			CShapeLine::ShapeLineUnit *shapeUnit = NULL;
			Envelope evlp;
			PT_3D pt0,  pt1;
			PT_3D pts3d[2];
			pt0 = pts3d[0] = pt3d[n*2], pt1 = pts3d[1] = pt3d[n*2+1];
			evlp.CreateFromPts(pts3d,2,sizeof(PT_3D));
			for(int i=0;i<nObj;i++)
			{
				pFtr0 = ftr[i];
				if( !pFtr0 )continue;
				const CShapeLine *shape = pFtr0->GetGeometry()->GetShape();
			    if( !evlp.bIntersect(&shape->GetEnvelope()) )continue;
// 				GrBuffer buf;
// 				pFtr0->Draw(&buf);
// 				if( !evlp.bIntersect(&buf.GetEnvelope()) )continue;
				shapeUnit = shape->HeadUnit();
				while( shapeUnit )
				{
					if (evlp.bIntersect(&shapeUnit->evlp))
					{
						PT_3DEX *pts = shapeUnit->pts;
						for( int k=0; k<shapeUnit->nuse-1; k++,pts++)
						{
							if( !evlp.bIntersect(pts,pts+1) )
								continue;
							
							if( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt1.x,pt1.y,
								pts[0].x,pts[0].y,pts[1].x,pts[1].y,NULL,NULL,NULL) )break;
						}
						
						if (k < shapeUnit->nuse-1)
						{
							arrIds2.Add(FtrToHandle(pFtr0));
							break;
						}
					}
					
					shapeUnit = shapeUnit->next;
				}
			}
		}
		nfind2 = arrIds2.GetSize();		
		
		if( nfind2<2 )continue;
		
		if( n==0 )
		{
			nfind1 = nfind2;
			arrIds1.Copy(arrIds2);
		}
		else if( nfind1<nfind2 )
		{
			nfind1 = nfind2;
			pt3d[0] = pt3d[n*2]; pt3d[1] = pt3d[n*2+1];
			arrIds1.Copy(arrIds2);
		}
	}
	
	Envelope evlp;
	evlp.CreateFromPts(pt3d,2);
	
	CArray<double,double> arrPos, arrZ;
	PT_3D ret;
	double t;
	for( int i=0; i<nfind1; i++)
	{
		CGeometry *po = HandleToFtr(arrIds1[i])->GetGeometry();
		if( !po )continue;
	
		const CShapeLine *pShape = ((CGeoCurve*)po)->GetShape();

		if (!pShape)   continue;

		if (evlp.bIntersect(&pShape->GetEnvelope()))
		{
			CArray<PT_3DEX,PT_3DEX> arrpts;
			pShape->GetPts(arrpts);
			PT_3DEX *pts = arrpts.GetData();
			for( int j=0; j<arrpts.GetSize()-1; j++,pts++)
			{
				if( CGrTrim::Intersect(pt3d,pt3d+1,&pts[0],&pts[1],&ret,&t) )
				{
					int nsz = arrPos.GetSize();
					for( int k=0; k<nsz; k++)
					{
						if( t<arrPos[k] )break;
					}
					if( k>=nsz )
					{
						arrPos.Add(t);
						arrZ.Add(pts[1].z);
					}
					else
					{
						arrPos.InsertAt(k,t);
						arrZ.InsertAt(k,pts[1].z);
					}
				}
			}
		}
		
	}
	
	int nsz = arrPos.GetSize();
	for( i=0; i<nsz; i++)
	{
		if( arrPos[i]>0.5 )break;
	}
	
	//等高线之间
	if( i>0 && i<nsz && fabs(arrZ[i-1]-arrZ[i])>1e-4 )
	{
		if( expt.z>=max(arrZ[i-1],arrZ[i]) || expt.z<=min(arrZ[i-1],arrZ[i]) )
			result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_ZOUTSIDE));
		else if( fabs(expt.z-arrZ[i-1])<=m_fMinDZ || fabs(expt.z-arrZ[i])<=m_fMinDZ )
			result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_LESSDZ));
		else if( m_fContourInter>0 && (fabs(expt.z-arrZ[i-1])>=m_fContourInter||fabs(expt.z-arrZ[i])>=m_fContourInter) )
			result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_GREATERDZ));
	}
	//山谷或者山顶
	else
	{
		if( i>=2 )
		{
			if( (expt.z>=arrZ[i-2]&&expt.z<=arrZ[i-1])||(expt.z>=arrZ[i-1]&&expt.z<=arrZ[i-2]) )
			{
				result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_ZOUTSIDE));
			}
			else if( m_fContourInter>0 && fabs(expt.z-arrZ[i-1])>=m_fContourInter )
			{
				result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_GREATERDZ));
			}
			else if( fabs(expt.z-arrZ[i-1])<=m_fMinDZ )
				result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_LESSDZ));	
		}
		else if( i<nsz-1 )
		{
			if( (expt.z>=arrZ[i]&&expt.z<=arrZ[i+1])||(expt.z>=arrZ[i+1]&&expt.z<=arrZ[i]) )
			{
				result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_ZOUTSIDE));
			}
			else if( m_fContourInter>0 && fabs(expt.z-arrZ[i])>=m_fContourInter )
			{
				result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_GREATERDZ));
			}
			else if( fabs(expt.z-arrZ[i])<=m_fMinDZ )
				result->AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_LESSDZ));	
		}
	}
}

void CCheckPLErrorCommand::MarkPt(PT_3D pt)
{
	CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
	if( !pLayer )return;
	
	CFeature *pFtr = pLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
	if( !pFtr ) return;

	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj ) return;
	pObj->SetColor(RGB(255,0,0));
		
	CArray<PT_3DEX,PT_3DEX> arr;
	PT_3DEX expt;
	COPY_3DPT(expt,pt);
	
	expt.x = pt.x-m_lfMarkWid;  expt.y = pt.y-m_lfMarkWid; expt.pencode = penLine;
	arr.Add(expt);
	expt.x = pt.x+m_lfMarkWid;  expt.y = pt.y+m_lfMarkWid; expt.pencode = penLine;
	arr.Add(expt);
	
	expt.x = pt.x-m_lfMarkWid;  expt.y = pt.y+m_lfMarkWid; expt.pencode = penMove;
	arr.Add(expt);
	expt.x = pt.x+m_lfMarkWid;  expt.y = pt.y-m_lfMarkWid; expt.pencode = penLine;
	arr.Add(expt);
	
}





//////////////////////////////////////////////////////////////////////
// CCheckLLErrorCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCheckLLErrorCommand,CEditCommand)

CCheckLLErrorCommand::CCheckLLErrorCommand()
{
	m_strBaseFCode = _T("");
	m_strCheckFCode = _T("");
	m_nStep = -1;
	m_lfMarkWid = 10;
	strcat(m_strRegPath,"\\CheckLLError");
}

CCheckLLErrorCommand::~CCheckLLErrorCommand()
{
}

CCommand* CCheckLLErrorCommand::Create()
{
	return new CCheckLLErrorCommand;
}

CString CCheckLLErrorCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CHECKLL);
}


void CCheckLLErrorCommand::Start()
{
	if( !m_pEditor )return;
	
	m_strBaseFCode = _T("");
	m_strCheckFCode = _T("");
	m_lfMarkWid = 10;
	
	m_nStep = 0;
	
	CEditCommand::Start();

	m_pEditor->CloseSelector();
}

void CCheckLLErrorCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(StrFromResID(IDS_CMDNAME_CHECKLL),StrFromResID(IDS_CMDNAME_CHECKLL));
	
	param->AddLayerNameParam(CMDPLANE_CONTOURCODE,(LPCTSTR)m_strBaseFCode,StrFromResID(IDS_CMDPLANE_CONTOURCODE));
	param->AddLayerNameParam(CMDPLANE_CHECKCODE,(LPCTSTR)m_strCheckFCode, StrFromResID(IDS_CMDPLANE_CHECKCODE));
	
}

void CCheckLLErrorCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CMDPLANE_CONTOURCODE,var) )
	{
		m_strBaseFCode = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,CMDPLANE_CHECKCODE,var) )
	{
		m_strCheckFCode = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CCheckLLErrorCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = (_bstr_t)m_strBaseFCode;
	tab.AddValue(CMDPLANE_CONTOURCODE,&CVariantEx(var));
	var = (_bstr_t)m_strCheckFCode;
	tab.AddValue(CMDPLANE_CHECKCODE,&CVariantEx(var));
}




void CCheckLLErrorCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 && m_pEditor )
	{
		BOOL bCheckCode = (m_strCheckFCode.GetLength()>0);
		if( !bCheckCode )
		{
			GOutPut(StrFromResID(IDS_CMDERR_CHECKCODE));
			return;
		}
		
//		CGeometry *pObj;
		CFeature  *pFtr = NULL;
		CFtrLayer *pLayer = NULL;
		
		int nLayNum = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerCount(), i, j , lSum = 0;
		for( i=0; i<nLayNum; i++)
		{
			pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
			if( !pLayer )continue;
			
			int nObjNum = pLayer->GetObjectCount();
			for( j=0; j<nObjNum; j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				
				if( bCheckCode && !CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pFtr,m_strCheckFCode))
					continue;
				lSum++;
			}
		}
		
		if( lSum<=0 )
		{
			GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
			return;
		}
		
		GProgressStart(lSum);
		
		CCPResultList result;
		result.attrNameList.Add(StrFromResID(IDS_CMDNAME_CHECKLL));
		for( i=0; i<nLayNum; i++)
		{
			pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
			if( !pLayer )continue;
			
			int nObjNum = pLayer->GetObjectCount();
			for( j=0; j<nObjNum; j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				
				if( bCheckCode && !CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pFtr,m_strCheckFCode))
					continue;
				
				result.StartRecord();
				result.SetCurFtr(pFtr);
				result.SetCurAttr(0);
				CheckObj(pFtr,&result);
				result.FinishRecord();
				/*
				int ptnum = pts.GetSize();
				for( k=0; k<ptnum; k++)
				{
				MarkPt(pts[k]);
			}*/
				
				GProgressStep();
			}
		}
		
		GProgressEnd();

		AfxGetMainWnd()->SendMessage(FCCM_SEARCHRESULT,2,(LPARAM)&result);
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt, flag);
}


void CCheckLLErrorCommand::GetIntersectObjs(PT_3D pt0, PT_3D pt1, CArray<FTR_HANDLE, FTR_HANDLE>& arr)
{
	PT_3D ptLine[2]; 
	ptLine[0] = pt0; ptLine[1] = pt1;
	//粗略查找第一条线段的相交地物
	Envelope e1;
	e1.CreateFromPts(ptLine,2);
	PDOC(m_pEditor)->GetDataQuery()->FindObjectInRect(e1,NULL);
	
	int num1;
	const CPFeature *ids1 = PDOC(m_pEditor)->GetDataQuery()->GetFoundHandles(num1);
	
	BOOL bCheckBase = (m_strBaseFCode.GetLength()>0);
	
	//获得有效的对象与两线段的相交信息，并排序
	CArray<CSmoothCommand::OBJ_ITEM,CSmoothCommand::OBJ_ITEM> arr1;
	CSmoothCommand::OBJ_ITEM item;
	CGeometry *pObj;
	for( int i=0; i<num1; i++)
	{
		CFeature *pFtr = (ids1[i]);
		if (!pFtr)  continue;
		pObj = pFtr->GetGeometry();
		if( !pObj )continue;
		
		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
		
		if( pObj->GetDataPointSum()<2 )continue;

		if( CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pFtr,m_strCheckFCode) )continue;
		if( bCheckBase && !CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),pFtr,m_strBaseFCode) )continue;

		if( !CModifyZCommand::CheckObjForContour(pObj) )continue;
		
		
		const CShapeLine *pShape = NULL;
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			pShape = ((CGeoCurve*)pObj)->GetShape();
		}
		else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			pShape = ((CGeoSurface*)pObj)->GetShape();
		}
		
		if (!pShape)  continue;

		int pos = 0;
		if (e1.bIntersect(&pShape->GetEnvelope()))
		{
			CArray<PT_3DEX,PT_3DEX> arrpts;
			pShape->GetPts(arrpts);
			PT_3DEX *pts = arrpts.GetData();
			for( int j=0; j<arrpts.GetSize(); j++,pts++)
			{
				if( e1.bIntersect(pts,&pts[1]) )
				{
					//求相交点
					PT_3D ret;
					double t=-1;
					if( !CGrTrim::Intersect(ptLine,ptLine+1,pts,&pts[1],&ret,&t) )
						continue;
					
					ret.z = pts->z;
					
					item.id = FTR_HANDLE(ids1[i]); item.pos= pos+j; item.ret= ret; item.t = t;
					
					//按照 t 大小排序插入
					int size1 = arr1.GetSize();
					for( int k=0; k<size1 && item.t>=arr1[k].t; k++);
					if( k<size1 )arr1.InsertAt(k,item);
					else arr1.Add(item);
					
					goto FINDOVER_1;
				}
			}
		}
FINDOVER_1:;
	}
	
	int size1 = arr1.GetSize();
	for( i=0; i<size1; i++)
	{
		FTR_HANDLE id = arr1[i].id;
		arr.Add(id);
	}
}



void CCheckLLErrorCommand::CheckObj(CFeature *pFtr, CCPResultList *result)
{
	if (!pFtr) return;
	CGeometry *pObj = pFtr->GetGeometry();
	if (!pObj) return;
	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);
	int i,j,ptnum = pts.GetSize();
	if( ptnum<2 )return;
	
	PT_3D ptLine[2],pt;
	PT_3DEX expt1, expt2, expt3, expt4, expt5;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds;
	FTR_HANDLE id1=0, id2=0;
	CGeometry *pObj1, *pObj2;
	int checkpos = -1; 
	for( i=0; i<=ptnum; i++) 
	{
		if( i==0 )
		{
			expt2 = pts[i];
			continue;
		}
		else if( i==ptnum )
		{
			continue;
		}
		else
		{
			expt1 = expt2;
			expt2 = pts[i];
		}
		
		COPY_3DPT(ptLine[0],expt1);
		COPY_3DPT(ptLine[1],expt2);
		
		arrIds.RemoveAll();
		GetIntersectObjs(ptLine[0],ptLine[1],arrIds);
		if( arrIds.GetSize()>0 )
		{
			id1 = id2;
			id2 = arrIds[0];
			
			if( id1 && id2 )
			{
				pObj1 = HandleToFtr(id1)->GetGeometry();
				pObj2 = HandleToFtr(id2)->GetGeometry();
				if( pObj1 && pObj2 && pObj1!=pObj2 )
				{
					expt3 = pObj1->GetDataPoint(0);
					expt4 = pObj2->GetDataPoint(0);
					
					if( checkpos>=0 )
					{
						//从上次检查结束的位置开始检查
						for( j=checkpos; j<i; j++)
						{
							expt5 = pts[j];
							if( expt5.z<(expt3.z<expt4.z?expt3.z:expt4.z) || 
								expt5.z>(expt3.z>expt4.z?expt3.z:expt4.z) )
							{
								result->AddPointResult(j,(short)1);
							}
						}
					}
				}
			}
			
			checkpos = i;
			id2 = arrIds[arrIds.GetSize()-1];
		}
	}
}

void CCheckLLErrorCommand::MarkPt(PT_3D pt)
{
	CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
	if( !pLayer )return;
	
	CFeature *pFtr = pLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
	if( !pFtr ) return;

	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj ) return;
	pObj->SetColor(RGB(255,0,0));
	
	CArray<PT_3DEX,PT_3DEX> arr;
	PT_3DEX expt;
	COPY_3DPT(expt,pt);
	
	expt.x = pt.x-m_lfMarkWid;  expt.y = pt.y-m_lfMarkWid; expt.pencode = penLine;
	arr.Add(expt);
	expt.x = pt.x+m_lfMarkWid;  expt.y = pt.y+m_lfMarkWid; expt.pencode = penLine;
	arr.Add(expt);
	
	expt.x = pt.x-m_lfMarkWid;  expt.y = pt.y+m_lfMarkWid; expt.pencode = penMove;
	arr.Add(expt);
	expt.x = pt.x+m_lfMarkWid;  expt.y = pt.y-m_lfMarkWid; expt.pencode = penLine;
	arr.Add(expt);
	
}




//////////////////////////////////////////////////////////////////////
// CCheckPPErrorCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCheckPPErrorCommand,CEditCommand)

CCheckPPErrorCommand::CCheckPPErrorCommand()
{
	m_nStep = -1;
	m_lfFindRadius = 500;
	strcat(m_strRegPath,"\\CheckPPError");
}

CCheckPPErrorCommand::~CCheckPPErrorCommand()
{
}

CCommand* CCheckPPErrorCommand::Create()
{
	return new CCheckPPErrorCommand;
}

CString CCheckPPErrorCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CHECKPP);
}


void CCheckPPErrorCommand::Start()
{
	if( !m_pEditor )return;
	
	m_lfFindRadius = 500;
	
	m_nStep = 0;
	
	CCommand::Start();
}

void CCheckPPErrorCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(StrFromResID(IDS_CMDNAME_CHECKPP),StrFromResID(IDS_CMDNAME_CHECKPP));
	
	param->AddParam(CMDPLANE_AREARADIUS,m_lfFindRadius, StrFromResID(IDS_CMDPLANE_AREARADIUS));
		
		
	
}

void CCheckPPErrorCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CMDPLANE_AREARADIUS,var) )
	{
		m_lfFindRadius = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CCheckPPErrorCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = m_lfFindRadius;
	tab.AddValue(CMDPLANE_AREARADIUS,&CVariantEx(var));
}



void CCheckPPErrorCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 && m_pEditor )
	{
		CFeature *pFtr;	
		CFtrLayer *pLayer;
		
		int nLayNum = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerCount(), i , j, lSum = 0;
		for( i=0; i<nLayNum; i++)
		{
			pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
			if( !pLayer )continue;
			
			int nObjNum = pLayer->GetObjectCount();
			for( j=0; j<nObjNum; j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				
				if( !FilterObj(pFtr,pLayer) )
					continue;
				lSum++;
			}
		}
		
		if( lSum<=0 )
		{
			GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
			return;
		}
		
		gSSP_forPoint.Clear();
		gSSP_forPoint.SetCheckFunc(CCheckPPErrorCommand::FilterObj);
		gSSP_forPoint.Init(PDOC(m_pEditor)->GetDlgDataSource(),m_pEditor->GetCoordWnd().m_pSearchCS);
//		gSSP_forPoint.m_dwFindMode = m_pDoc->m_selection.m_dwFindMode;
//		gSSP_forPoint.m_bWithSymbol= m_pEditor->m_selection.m_bWithSymbol;
		gSSP_forPoint.PrepareWithRadius(m_lfFindRadius);		
		
		GProgressStart(lSum);
		
		CCPResultList result;
		result.attrNameList.Add(StrFromResID(IDS_CMDNAME_CHECKPP));
		for( i=0; i<nLayNum; i++)
		{
			pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
			if( !pLayer )continue;
			
			int nObjNum = pLayer->GetObjectCount();
			for( j=0; j<nObjNum; j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				
				result.StartRecord();
				result.SetCurFtr(pFtr);
				result.SetCurAttr(0);
				CheckObj(pFtr,pLayer,&result);
				result.FinishRecord();
				
				GProgressStep();
			}
		}
		
		gSSP_forPoint.Clear();
		gSSP_forPoint.SetCheckFunc(NULL);
		
		GProgressEnd();

		AfxGetMainWnd()->SendMessage(FCCM_SEARCHRESULT,2,(LPARAM)&result);
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt, flag);
}

void CCheckPPErrorCommand::CheckObj(CFeature *pFtr, CFtrLayer *pLayer, CCPResultList *result)
{
	CScheme *pScheme = gpCfgLibMan->GetScheme(PDOC(m_pEditor)->GetDlgDataSource()->GetScale());
	if (!pScheme)  return;

	CSchemeLayerDefine *pSLayer = pScheme->GetLayerDefine(pLayer->GetName());

	if (!pSLayer) return;

	CAnnotation *pAnno = pSLayer->GetHeightAnnotation();
	if (!pAnno) return;

	PT_3DEX expt,expt1;
	PT_3D pt3d[4];
	Envelope e;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds1, arrIds2;
	FTR_HANDLE *ids;

	CGeometry *pObj = pFtr->GetGeometry();
	
	expt = pObj->GetDataPoint(0);
	
	double dx,dy, limit = m_lfFindRadius*m_lfFindRadius;
	gSSP_forPoint.GetGridSize(dx,dy);
	
	pt3d[0].x = expt.x-dx; pt3d[0].y = expt.y-dy; pt3d[0].z = expt.z;
	pt3d[1].x = expt.x+dx; pt3d[1].y = expt.y+dy; pt3d[1].z = expt.z;
	pt3d[2].x = expt.x-dx; pt3d[2].y = expt.y+dy; pt3d[2].z = expt.z;
	pt3d[3].x = expt.x+dx; pt3d[3].y = expt.y-dy; pt3d[3].z = expt.z;
	e.CreateFromPts(pt3d,4);
	
	FTR_HANDLE theid = FtrToHandle(pFtr);
	
	{
		int nfind2 = gSSP_forPoint.FindObjectInRect(pFtr,&expt,e,TRUE);
		if( nfind2>=1 )
		{
			arrIds2.SetSize(nfind2);
			gSSP_forPoint.GetFoundObjectHandles(arrIds2.GetData());
			
			//去除可能存在的重复地物
			ids = arrIds2.GetData();
			for( int i=nfind2-1; i>=0; i--)
			{
				if( ids[i]==theid )continue;
				
				CGeometry *pObj2 = HandleToFtr(ids[i])->GetGeometry();
				if( !pObj2 )continue;
				
				CFtrLayer *pLayer2 = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(ids[i]));

				if (!pLayer2) continue;

				CSchemeLayerDefine *pSLayer2 = pScheme->GetLayerDefine(pLayer2->GetName());
				
				if (!pSLayer2) continue;
				
				CAnnotation *pAnno2 = pSLayer2->GetHeightAnnotation();
				if (!pAnno2) continue;

				expt1 = pObj2->GetDataPoint(0);

				int nDigit = pAnno->m_nDigit>pAnno2->m_nDigit?pAnno->m_nDigit:pAnno2->m_nDigit;

				if (expt.z-expt1.z < pow(10,nDigit))
				{
					CString str;
					str.Format("%f",expt.z);
					result->AddAttrResult((LPCTSTR)str);
					str.Format("%f",expt1.z);
					result->AddAttrResult((LPCTSTR)str);
					return;
				}

/*				
				
				CPtrArray AnnoPtr2;
				pLayer->GetAnnotation(pObj2,AnnoPtr2);
				for(int i=0;i<AnnoPtr2.GetSize();i++)
				{
					
					CAnnotation *pAnnot2 = (CAnnotation*)AnnoPtr2[i];
					if( pAnnot2->m_nType!=1 )
						break;
					CString strAnnot2 = pAnnot2->m_strAnnot;
					
					CPtrArray AnnoPtr1;
					pLayer->GetAnnotation(pObj,AnnoPtr1);
					for(int j=0;i<AnnoPtr1.GetSize();i++)
					{
						CAnnotation *pAnnot1 = (CAnnotation*)AnnoPtr1[j];
						if( pAnnot1->m_nType!=1 )continue;
						CString strAnnot1 = pAnnot1->m_strAnnot;
						
						pObj2->GetPt(0,&expt1);
						
						if( strAnnot1==strAnnot2 )
						{
							double dis = (expt.x-expt1.x)*(expt.x-expt1.x)+(expt.y-expt1.y)*(expt.y-expt1.y);
							if( dis<=limit )
							{
								result->AddAttrResult((LPCTSTR)strAnnot1);
	//							break;
								return;
							}
						}
					}

				}*/
			}
		}
	}
}


BOOL CCheckPPErrorCommand::FilterObj(CFeature *pObj, CFtrLayer *pLayer)
{
	if( !pObj )return FALSE;
	if( !pObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))return FALSE;
//////////////////////////////////////////////////////////////////////////
// 	CPtrArray m_AnnoPtr;
// 	pLayer->GetAnnotation(pObj,m_AnnoPtr);
// 	if(m_AnnoPtr.GetSize()<1)
// 		return FALSE;
// 	for(int i=0;i<m_AnnoPtr.GetSize()-1;i++)
// 	{
// 		CAnnotation *pAnnot = (CAnnotation*)m_AnnoPtr[i];
// 		if(pAnnot->m_nType == 1)
// 			return TRUE;
// 	}
//////////////////////////////////////////////////////////////////////////
/*	CAnnotation *pAnnot = (CAnnotation*)pLayer->GetAnnotation(pObj);
	
	if( !pAnnot )return FALSE;
	if( pAnnot->m_nType!=1 )return FALSE;
*/	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPointTextConvertBaseCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPointTextConvertBaseCommand,CEditCommand)

CPointTextConvertBaseCommand::CPointTextConvertBaseCommand()
{
	m_nStep = -1;
}

CPointTextConvertBaseCommand::~CPointTextConvertBaseCommand()
{
}

void CPointTextConvertBaseCommand::Start()
{
	if( !m_pEditor )return;
	
	int num = 0;
	const FTR_HANDLE * pHandle = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		m_nStep = 0;		
		CCommand::Start();
		
		PT_3D pt;
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}

void CPointTextConvertBaseCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		int num = 0;
		const FTR_HANDLE * pHandle = m_pEditor->GetSelection()->GetSelectedObjs(num);
		for (int i=0; i<num; i++)
		{
			m_arrFtrs.Add(pHandle[i]);
		}

		Convert();
		
		m_pEditor->DeselectAll();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt,flag);
}

//////////////////////////////////////////////////////////////////////
// CAnnotToTextCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CAnnotToTextCommand,CEditCommand)

CAnnotToTextCommand::CAnnotToTextCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\AnnotToText");
}

CAnnotToTextCommand::~CAnnotToTextCommand()
{
}

CString CAnnotToTextCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_ANNOTTOTEXT);
}

BOOL CAnnotToTextCommand::Convert()
{	
	int num = m_arrFtrs.GetSize();
	if (num <= 0) return FALSE;

	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return FALSE;

	CUndoFtrs undo(m_pEditor,Name());
	for( int i=num-1; i>=0; i--)
	{
		CFeature *pFtr = HandleToFtr(m_arrFtrs[i]);
		if (!pFtr) continue;
		
		int nLayerId = PDOC(m_pEditor)->GetFtrLayerIDOfFtr(m_arrFtrs[i]);
		CFtrLayer *pLayer = pDS->GetFtrLayer(nLayerId);
		if (!pLayer) continue;

		CGeometry *pObj = pFtr->GetGeometry();
		if( !pObj || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) ) continue;

		CFtrLayer *pTextLayer = pDS->GetLocalFtrLayer((StrFromResID(IDS_DEFLAYER_NAMET)));
		if (!pTextLayer)  return FALSE;
		CFeature *pTempFtr = pTextLayer->CreateDefaultFeature(pDS->GetScale());
		if (!pTempFtr) return FALSE;
		
		{
			CPtrArray arr;
			CConfigLibManager *pCLM = gpCfgLibMan;
			pCLM->GetSymbol(pDS,pFtr,arr);
			
			for(int i=0; i<arr.GetSize(); i++)
			{
				CSymbol *pSymbol = (CSymbol*)arr.GetAt(i);
				if (pSymbol && pSymbol->GetType()==SYMTYPE_ANNOTATION )
				{
					CAnnotation *pAnnot = (CAnnotation*)arr.GetAt(i);
					
					if (pAnnot->m_nAnnoType != CAnnotation::Attribute) continue;
					
					CPtrArray parr;
					CValueTable tab;
					pDS->GetAllAttribute(pFtr,tab);
					double fDrawScale = ((double)pDS->GetScale()/1000)*pDS->GetDrawScaleByUser();
					if( pAnnot->ExtractGeoText(pFtr,parr,tab,fDrawScale) )
					{
						for (int j=0; j<parr.GetSize(); j++)
						{
							CGeoText *pText = (CGeoText*)parr[j];
							pText->SetColor(pObj->GetColor());
							
							pTempFtr->SetGeometry(pText);
							
							CFeature *pNewFtr = pTempFtr->Clone();
							
							PDOC(m_pEditor)->AddObject(pNewFtr,pTextLayer->GetID());
							
							
							undo.arrNewHandles.Add(FtrToHandle(pNewFtr));								
							
						}				
						
					}
				}
			}
		}	

		if( pTempFtr!=NULL )delete pTempFtr;

	}
	
	undo.Commit();
	

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPointZToTextCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CPointZToTextCommand,CEditCommand)

CPointZToTextCommand::CPointZToTextCommand()
{
	m_nStep = -1;
	m_nDigitNum = 2;
	strcat(m_strRegPath,"\\PointZToText");

	m_sTxtSettings.fHeight = 2.0;
}

CPointZToTextCommand::~CPointZToTextCommand()
{
}

CString CPointZToTextCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_POINTZTOTEXT);
}

BOOL CPointZToTextCommand::Convert()
{	
	int num = m_arrFtrs.GetSize();
	if (num <= 0) return FALSE;
	
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return FALSE;
	
	CUndoFtrs undo(m_pEditor,Name());
	for( int i=num-1; i>=0; i--)
	{
		CFeature *pFtr = HandleToFtr(m_arrFtrs[i]);
		if (!pFtr) continue;
		
		CFtrLayer *pLayer = pDS->GetFtrLayer(m_strTextLayerCode);
		if( !pLayer )pLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
		if (!pLayer) continue;
		
		CGeometry *pObj = pFtr->GetGeometry();
		if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) ) continue;

		PT_3D pt = pObj->GetDataPoint(0);

		pt.x += pDS->GetSymbolDrawScale()*1.0;
		
		CFeature *pTextFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOTEXT);
		if (!pTextFtr) continue;

		CString strFormat;
		strFormat.Format(_T("%%.%df"), m_nDigitNum);

		CString strText;
		strText.Format(strFormat,pt.z);
		//TrimRZero(strText);

		m_sTxtSettings.nAlignment = TAH_LEFT|TAV_MID;
	
		CGeoText *pText = (CGeoText*)pTextFtr->GetGeometry();
		if (pText)
		{
			pText->CreateShape(&PT_3DEX(pt,penLine),1);
			pText->SetText(strText);
			pText->SetSettings(&m_sTxtSettings);

			PDOC(m_pEditor)->AddObject(pTextFtr,pLayer->GetID());			
			
			undo.arrNewHandles.Add(FtrToHandle(pTextFtr));			
		}	
	}	

	undo.Commit();	
	
	return TRUE;
}


void CPointZToTextCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(m_strTextLayerCode);
	tab.AddValue(PF_CONTOURTEXTCODE,&CVariantEx(var));
	var = (long)(m_nDigitNum);
	tab.AddValue(PF_CONTOURDIGITNUM,&CVariantEx(var));
	var = (_bstr_t)(m_sTxtSettings.strFontName);
	tab.AddValue(PF_FONT,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fHeight);
	tab.AddValue(PF_CHARH,&CVariantEx(var));
}


void CPointZToTextCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("PointZToTextCommand",Name());
	param->AddLayerNameParam(PF_CONTOURTEXTCODE,LPCTSTR(m_strTextLayerCode),StrFromResID(IDS_CMDPLANE_HPC));
	param->AddParam(PF_CONTOURDIGITNUM,int(m_nDigitNum),StrFromResID(IDS_CMDPLANE_DIGITNUM));
  	param->AddFontNameParam(PF_FONT,m_sTxtSettings.strFontName,StrFromResID(IDS_CMDPLANE_FONT));

	param->AddParam(PF_CHARH,(double)m_sTxtSettings.fHeight,StrFromResID(IDS_CMDPLANE_CHARH));
}



void CPointZToTextCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	if( tab.GetValue(0,PF_CONTOURTEXTCODE,var) )
	{
		m_strTextLayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CONTOURDIGITNUM,var) )
	{
		m_nDigitNum = (long)(_variant_t)*var;
		if (m_nDigitNum < 1)
		{
			m_nDigitNum = 1;
			AfxMessageBox(IDS_PARAM_ERROR);
			return;
		}
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_FONT,var) )
	{
		CString temp = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		strncpy(m_sTxtSettings.strFontName,temp,sizeof(m_sTxtSettings.strFontName)-1);
		TextStyle style = GetUsedTextStyles()->GetTextStyleByName(temp);
		if( style.IsValid() )
		{
			m_sTxtSettings.fWidScale = style.fWidScale;
			m_sTxtSettings.nInclineType = style.nInclineType;
			m_sTxtSettings.fInclineAngle = style.fInclinedAngle;
			m_sTxtSettings.SetBold(style.bBold);
		}
		
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CHARH,var) )
	{
		m_sTxtSettings.fHeight = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CTextToPointCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CTextToPointCommand,CEditCommand)

CTextToPointCommand::CTextToPointCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\TextToPoint");
}

CTextToPointCommand::~CTextToPointCommand()
{
}

CString CTextToPointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TEXTTOPOINT);
}

BOOL CTextToPointCommand::Convert()
{	
	int num = m_arrFtrs.GetSize();
	if (num <= 0) return FALSE;
	
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return FALSE;
	
	CUndoFtrs undo(m_pEditor,Name());
	for( int i=num-1; i>=0; i--)
	{
		CFeature *pFtr = HandleToFtr(m_arrFtrs[i]);
		if (!pFtr) continue;

		CFtrLayer *pLayer = pDS->GetFtrLayer(m_strLayerCode);
		if( !pLayer )pLayer = pDS->GetFtrLayerOfObject(pFtr);
		
		if (!pLayer) continue;
		
		CGeometry *pObj = pFtr->GetGeometry();
		if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) ) continue;
		
		PT_3D pt = pObj->GetDataPoint(0);
		
		CFeature *pPointFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOPOINT);
		if (!pPointFtr) continue;
				
		PT_3D ptNew = pt;

		CString strText = ((CGeoText*)pObj)->GetText();
		if (IsDigital(strText))
		{
			ptNew.z = atof(strText);
		}			
		
		CGeoPoint *pPoint = (CGeoPoint*)pPointFtr->GetGeometry();
		if (pPoint)
		{
			pPoint->CreateShape(&PT_3DEX(ptNew,penLine),1);
			
			PDOC(m_pEditor)->AddObject(pPointFtr,pLayer->GetID());			
			
			undo.arrNewHandles.Add(FtrToHandle(pPointFtr));			
		}	
	}	
	
	undo.Commit();	
	
	return TRUE;
}


void CTextToPointCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)((LPCTSTR)m_strLayerCode);
	tab.AddValue(PF_CONTOURTEXTCODE,&CVariantEx(var));
}


void CTextToPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("TextToPointCommand",Name());
	param->AddLayerNameParam(PF_CONTOURTEXTCODE,LPCTSTR(m_strLayerCode),StrFromResID(IDS_LAYER_NAME));
}



void CTextToPointCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	if( tab.GetValue(0,PF_CONTOURTEXTCODE,var) )
	{
		m_strLayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CModifyPointzAndTextCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifyPointzAndTextCommand,CEditCommand)

CModifyPointzAndTextCommand::CModifyPointzAndTextCommand()
{
	m_nStep = 0;
	m_nDigitNum = 2;
}

CModifyPointzAndTextCommand::~CModifyPointzAndTextCommand()
{
}

void CModifyPointzAndTextCommand::Start()
{
	if( !m_pEditor )return;
	
	m_nDigitNum = 2;
	CCommand::Start();

	int num = 0;
	const FTR_HANDLE * pHandle = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num == 2)
	{
		m_nStep = 1;		
		
		PT_3D pt;
		PtClick(pt,0);

		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}

CString CModifyPointzAndTextCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_POINTZANDTEXT);
}



void CModifyPointzAndTextCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (long)(m_nDigitNum);
	tab.AddValue(PF_CONTOURDIGITNUM,&CVariantEx(var));
}

void CModifyPointzAndTextCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ModifyPointzAndText",Name());
	param->AddParam(PF_CONTOURDIGITNUM,int(m_nDigitNum),StrFromResID(IDS_CMDPLANE_DIGITNUM));
	
}

void CModifyPointzAndTextCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_CONTOURDIGITNUM,var) )
	{
		m_nDigitNum = (long)(_variant_t)*var;
		if (m_nDigitNum < 1)
		{
			AfxMessageBox(IDS_PARAM_ERROR);
			m_nDigitNum = 1;
			return;
		}
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CModifyPointzAndTextCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag,TRUE) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		int num = 0;
		const FTR_HANDLE * pHandle = m_pEditor->GetSelection()->GetSelectedObjs(num);

		if (num != 2)
		{
			GOutPut(StrFromResID(IDS_CMDTIP_SELPTTEXT));
			Abort();
			return;
		}
		
		CGeometry *pGeo1 = HandleToFtr(pHandle[0])->GetGeometry(), *pGeo2 = HandleToFtr(pHandle[1])->GetGeometry();
		if (!pGeo1 || !pGeo2) 
		{
			Abort();
			return;
		}
		
		CFeature *pPointFtr=NULL, *pTextFtr=NULL;
		if (pGeo1->IsKindOf(RUNTIME_CLASS(CGeoPoint)) && pGeo2->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			pPointFtr = HandleToFtr(pHandle[0]);
			pTextFtr = HandleToFtr(pHandle[1]);			
		}
		else if (pGeo2->IsKindOf(RUNTIME_CLASS(CGeoPoint)) && pGeo1->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			pPointFtr = HandleToFtr(pHandle[1]);
			pTextFtr = HandleToFtr(pHandle[0]);	
		}
		
		if (!pPointFtr || !pTextFtr)
		{
			GOutPut(StrFromResID(IDS_CMDTIP_SELPTTEXT));
			Abort();
			return;
		}

		PT_3D pt = pPointFtr->GetGeometry()->GetDataPoint(0);
		
		GOutPut(StrFromResID(IDS_CMDTIP_INPUTHEIGHT));

		CDlgInputData dlg;
		dlg.SetInputData(pt.z);
		if (dlg.DoModal() == IDOK)
		{
			double z = dlg.GetInputData();

			CUndoBatchAction batchUndo(m_pEditor,Name());

			CUndoVertex undo(m_pEditor,Name());
			m_pEditor->DeleteObject(FtrToHandle(pPointFtr),FALSE);

			COPY_3DPT(undo.ptOld,pt);
			undo.ptOld.pencode = penMove;
			PT_3DEX t(pt.x,pt.y,z,penMove);
			pPointFtr->GetGeometry()->SetDataPoint(0,t);
			COPY_3DPT(undo.ptNew,t);
			undo.ptNew.pencode = penMove;

			undo.handle = FtrToHandle(pPointFtr);
			undo.nPtType = PT_KEYCTRL::typeKey;
			undo.nPtIdx = 0;

			m_pEditor->RestoreObject(FtrToHandle(pPointFtr));

			batchUndo.AddAction(&undo);


			CUndoModifyProperties undo2(m_pEditor,Name());

			CValueTable tab;
			tab.BeginAddValueItem();
			pTextFtr->WriteTo(tab);
			tab.EndAddValueItem();

			undo2.arrHandles.Add(FtrToHandle(pTextFtr));
			
			m_pEditor->DeleteObject(FtrToHandle(pTextFtr),FALSE);

			undo2.oldVT.CopyFrom(tab);
	
			CGeoText *pText = (CGeoText*)pTextFtr->GetGeometry();
			CString strFormat,str;
			int nDigitNum = m_nDigitNum;
			if( nDigitNum<=0 )nDigitNum = 0;
			strFormat.Format("%%.%df",nDigitNum);
			str.Format((LPCTSTR)strFormat,z);
		//	TrimRZero(str);
			pText->SetText(str);

			PT_3DEX t1 = pText->GetDataPoint(0);
			t1.z = z;
			pText->SetDataPoint(0,t1);

			CValueTable tab1;
			tab1.BeginAddValueItem();
			pTextFtr->WriteTo(tab1);
			tab1.EndAddValueItem();

			undo2.newVT.CopyFrom(tab1);
					
			m_pEditor->RestoreObject(FtrToHandle(pTextFtr));

			batchUndo.AddAction(&undo2);
			

			batchUndo.Commit();
		}
		else
		{
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			Abort();
			return;
		}
		
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CModifyTextDigitCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifyTextDigitCommand,CEditCommand)

CModifyTextDigitCommand::CModifyTextDigitCommand()
{
	m_nStep = 0;
	m_nDigitNum = 2;
}

CModifyTextDigitCommand::~CModifyTextDigitCommand()
{
}

void CModifyTextDigitCommand::Start()
{
	if( !m_pEditor )return;
	
	CCommand::Start();

	int num = 0;
	const FTR_HANDLE * pHandle = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num == 2)
	{
		m_nStep = 1;		
		
		PT_3D pt;
		PtClick(pt,0);

		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}

CString CModifyTextDigitCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_TEXTDIGIT);
}

void CModifyTextDigitCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (long)(m_nDigitNum);
	tab.AddValue(PF_CONTOURDIGITNUM,&CVariantEx(var));
}

void CModifyTextDigitCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ModifyTextDigit",StrFromResID(IDS_CMDNAME_TEXTDIGIT));
	param->AddParam(PF_CONTOURDIGITNUM,int(m_nDigitNum),StrFromResID(IDS_CMDPLANE_DIGITNUM));

}



void CModifyTextDigitCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;

	if( tab.GetValue(0,PF_CONTOURDIGITNUM,var) )
	{
		m_nDigitNum = (long)(_variant_t)*var;
		if (m_nDigitNum < 1)
		{
			m_nDigitNum = 1;
			AfxMessageBox(IDS_PARAM_ERROR);
			return;
		}
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

void CModifyTextDigitCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag,TRUE) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		int num = 0;
		const FTR_HANDLE * pHandle = m_pEditor->GetSelection()->GetSelectedObjs(num);

		CUndoModifyProperties undo(m_pEditor,Name());
		
		for (int i=num-1; i>=0; i--)
		{
			CFeature *pFtr = (CFeature*)pHandle[i];
			if (!pFtr) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText))) continue;

			CGeoText *pText = (CGeoText*)pGeo;
			if (pText)
			{
				CString text = pText->GetText();
				if (IsDigital(text))
				{		
					PT_3D pt0 = pText->GetDataPoint(0);

					CString strFormat;
					strFormat.Format(_T("%%.%df"), m_nDigitNum);

					CString newText;
					//double fText = atof(text);
					newText.Format(strFormat,pt0.z);

					m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
					pText->SetText(newText);		
					m_pEditor->RestoreObject(FtrToHandle(pFtr));

					undo.SetModifyProp(pFtr,FIELDNAME_GEOTEXT_CONTENT,&CVariantEx((_variant_t)(LPCTSTR)text),
						&CVariantEx((_variant_t)(LPCTSTR)newText));
				}
			}
		}	
		
		undo.Commit();
		
		
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt,flag);
}

//////////////////////////////////////////////////////////////////////
// CDrawMultiSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawMultiSurfaceCommand,CDrawSurfaceCommand)

CDrawMultiSurfaceCommand::CDrawMultiSurfaceCommand()
{
	m_bMulitSurface = FALSE;
	m_pSel = NULL;
}

CDrawMultiSurfaceCommand::~CDrawMultiSurfaceCommand()
{
	if (m_pSel)
	{
		delete m_pSel;
	}
}



CString CDrawMultiSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWSURFACE);
}

void CDrawMultiSurfaceCommand::Start()
{	
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOSURFACE);
	if(!m_pFtr) return;	

	CDlgDataSource *pDS = GETDS(m_pEditor);			

	m_pDrawProc = new CDrawSurfaceProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);

	m_pDrawProc->m_pGeoSurface = (CGeoSurface*)m_pFtr->GetGeometry();
	UpdateParams(FALSE);
	m_pDrawProc->Start();	
	
	CDrawCommand::Start();

	//CDrawSurfaceCommand::Start();
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
void CDrawMultiSurfaceCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}

void CDrawMultiSurfaceCommand::Finish()
{
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CDrawSurfaceCommand::Finish();
}

DrawingInfo CDrawMultiSurfaceCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	return DrawingInfo(m_pFtr,m_pDrawProc->m_arrPts);
}

void CDrawMultiSurfaceCommand::Abort()
{	
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CDrawSurfaceCommand::Abort();	
}


CFeature *CreateMultiSurface(CFtrLayer *pLayer,CFeature* pMother, CGeoSurface *pSon)
{
	CFeature *pNewObj = pLayer->CreateDefaultFeature(::GetActiveDlgDoc()->GetDlgDataSource()->GetScale(),CLS_GEOMULTISURFACE);
	
	if( !pNewObj )
	{
		return NULL;
	}
	
	
	((CGeoMultiSurface*)(pNewObj->GetGeometry()))->CopyFromSurface(pMother->GetGeometry());
	CGeoSurface* pObj = pSon;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pObj->GetShape(arrPts);
	
	((CGeoMultiSurface*)(pNewObj->GetGeometry()))->AddSurface(arrPts);
	
	return pNewObj;
}


void CDrawMultiSurfaceCommand::OnSonEnd(CProcedure *son)
{
	ASSERT( m_pDrawProc==son );
	if( IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoSurface )
	{
		Abort();
		return;
	}

	//定义变量
	//pObj: 记录当前完成的对象; pObj2: 创建复杂面的母对象
	CGeometry *pObj = NULL;
	Envelope e, e0,e1; 
//	int  nPtsum;

	CPFeature  minpFtr=NULL, pFtr2 = NULL;
	CGeometry *pObj1 = NULL;
//	double area;
	BOOL bMotherValid = FALSE;
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	PT_3DEX expt;
	////////////////////////////////////////

	CUndoFtrs undo(m_pEditor,Name());

	pObj = m_pDrawProc->m_pGeoSurface;
	
	if( !pObj )goto _DrawMultiSurface_OldProcess;

	//找到包含当前对象的母亲对象
	e = pObj->GetShape()->GetEnvelope();


// 	//要求包含当前对象，且面积最小
// 	ftr = m_pEditor->GetDataQuery()->GetFoundHandles(nObjs);
	minpFtr = FindSmallClosureSurfaceOfEnvelope(GETDS(m_pEditor),e);
	if( !minpFtr )goto _DrawMultiSurface_OldProcess;



	pFtr2 = CreateMultiSurface(PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(minpFtr),minpFtr,(CGeoSurface*)pObj);
	if( !pFtr2 )
		goto _DrawMultiSurface_OldProcess;
	
	//特征码相同，就创建单一的复杂面
	if( 1 )
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
		pFtr2->SetID(OUID());
		m_pEditor->AddObject(pFtr2,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(minpFtr)));
		undo.AddNewFeature(FtrToHandle(pFtr2));
		PDOC(m_pEditor)->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(minpFtr,pFtr2);
		m_pEditor->DeleteObject(FtrToHandle(minpFtr));
		undo.AddOldFeature(FtrToHandle(minpFtr));
	}
	//不相同，除了创建复杂面以外，还要将当前面仍然需要保留
	else
	{
// 		m_pEditor->DeleteObject(m_idMother);
// 		CPFeature pFtr = new CFeature;
// 		pFtr->SetGeometry(pObj);
// 		m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(m_idMother));
// 		m_idComplex = m_pEditor->AddObject(pObj2,m_pEditor->GetFtrLayerIDOfFtr(m_idMother));
	}

	undo.Commit();

	m_bMulitSurface = TRUE;
	m_pEditor->RefreshView();
	Finish();
	return;

	// 创建复杂面不成功，就按照基类中绘制面的行为继续下去
_DrawMultiSurface_OldProcess:
	CDlgDrawSurfaceCommand::OnSonEnd(son);

}

void CDrawMultiSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	GotoState(PROCSTATE_PROCESSING);

	CDlgDrawSurfaceCommand::PtClick(pt,flag);
}

void CDrawMultiSurfaceCommand::PtMove(PT_3D &pt)
{
	CDlgDrawSurfaceCommand::PtMove(pt);
}




void CDrawMultiSurfaceCommand::GetParams(CValueTable &tab)
{	
//	CDlgDrawSurfaceCommand::GetParams(tab);
//  	_variant_t var;
// 	var = (bool)(m_bMatchHeight);
// 	tab.AddValue(PF_LINKCONTORMATCHHEI,&CVariantEx(var));
// 	var = (long)(m_nBoundType);
// 	tab.AddValue(PF_TRIMBOUND,&CVariantEx(var));
// 	var = double(m_pDrawProc->m_compress.GetLimit());
// 	tab.AddValue(PF_TRIMTOLER,&CVariantEx(var));

}


void CDrawMultiSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
  	param->SetOwnerID("DrawMultiSurface",Name());
// 	param->AddParam(PF_LINKCONTORMATCHHEI,bool(m_bMatchHeight),StrFromResID(IDS_CMDPLANE_MATCHHEIGHT));
	
// 	param->BeginOptionParam(PF_TRIMBOUND,StrFromResID(IDS_CMDPLANE_BOUND));
// 	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),0,' ',m_nBoundType==0);
// 	param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),1,' ',m_nBoundType==1);
// 	param->AddOption(StrFromResID(IDS_CMDPLANE_WSBOUND),2,' ',m_nBoundType==2);
// 	param->EndOptionParam();
// 	if(m_nBoundType!=0) return;
// 	BOOL bHide = (m_pDrawProc->m_nCurPenCode!=penStream);
//  	if(!bHide)param->AddParam(PF_TRIMTOLER,double(m_pDrawProc->m_compress.GetLimit()),StrFromResID(IDS_CMDPLANE_TOLER));

}



CFeature * CDrawMultiSurfaceCommand::FindSmallClosureSurfaceOfEnvelope(CDlgDataSource *pDS,Envelope e)
{
	if (!pDS)
	{
		return NULL;
	}
	CFtrLayer *pFtrLayer = NULL;
	CFeature *pFtr = NULL,*pFtr0 = NULL;
	Envelope e0;
	CGeometry *pObj = NULL;
	double area = 0;
	int j = 0;
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if (!pFtrLayer||pFtrLayer->IsEmpty()||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible()) continue;
		for (j=0;j<pFtrLayer->GetObjectCount();j++)
		{
			pFtr = pFtrLayer->GetObject(j);
			if (pFtr)
			{
				pObj = pFtr->GetGeometry();
				if (!pObj||!pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					continue;
				}
				e0 = pObj->GetEnvelope();
				if (e0.bEnvelopeIn(&e))
				{
					if (fabs(area)<1e-4||e0.Height()*e0.Width()<area)
					{
						area = e0.Height()*e0.Width();
						pFtr0 = pFtr;
					}					
				}
			}
		}		
	}
	return pFtr0;
}

void CDrawMultiSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
//	CDrawSurfaceCommand::SetParams(tab);
}


//////////////////////////////////////////////////////////////////////
// CDrawLinesbyAngCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawLinesbyAngCommand,CDrawSurfaceCommand)

CDrawLinesbyAngCommand::CDrawLinesbyAngCommand()
{
	m_nMode=modeLine;
	m_nStep=-1;
	m_bClosed = FALSE;
	m_pFtr = NULL;
	m_pSel = NULL;
	m_bOpenIfSnapped = FALSE;
	m_nEndSnapped = 0;
	m_bSnapPt1 = FALSE;
	strcat(m_strRegPath,"\\LinesbyAng");
}

CDrawLinesbyAngCommand::~CDrawLinesbyAngCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if (m_pSel)
	{
		delete m_pSel;
	}
}

void CDrawLinesbyAngCommand::Finish()
{	
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	if( m_pEditor )
	{
		if( m_pFtr )
		{			
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_ANNOTTOTEXT,FALSE);
			
			if( bAnnotToText )
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if(pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOTEXT);
				if( pLayer!=NULL && pTextLayer!=NULL && pTempl!=NULL )
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor),m_pFtr,arrAnnots);
					CUndoFtrs undo(m_pEditor,Name());
					for(int i=0; i<arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType()==SYMTYPE_ANNOTATION )
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;							
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr,tab);
							if( pAnnot->ExtractGeoText(m_pFtr,parr,tab,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()) )
							{
								for (int j=0; j<parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);									
									PDOC(m_pEditor)->AddObject(pNewFtr,pTextLayer->GetID());									
									
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
									
								}
							}
						}					
						
					}
					undo.Commit();					
				}
				if( pTempl!=NULL )delete pTempl;
			}
		}		
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	CDrawCommand::Finish();
}

void CDrawLinesbyAngCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
CString CDrawLinesbyAngCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWLINESBYANG);
}

void CDrawLinesbyAngCommand::Start()
{
	m_nMode=modeLine;
	m_nStep=0;
	m_bClosed = FALSE;
	m_bOpenIfSnapped = FALSE;
	m_nEndSnapped = 0;

	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	

	CDlgDataSource *pDS = GETDS(m_pEditor);			

	CDrawCommand::Start();
	((CGeoCurve*)m_pFtr->GetGeometry())->EnableClose(m_bClosed);
	m_pEditor->CloseSelector();	
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
	m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)StrFromResID(IDS_CMDTIP_DRAWBYANG));
	
}

DrawingInfo CDrawLinesbyAngCommand::GetCurDrawingInfo()
{
	return DrawingInfo(m_pFtr,m_arrPts);
}

void CDrawLinesbyAngCommand::Abort()
{	
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CDrawCommand::Abort();	
}


void CDrawLinesbyAngCommand::PtClick(PT_3D &pt, int flag)
{
	if( IsProcFinished(this) )return;

	GotoState(PROCSTATE_PROCESSING);

	if (m_nStep==0)
	{
		PT_3DEX expt(pt,penLine);	
		m_arrPts.Add(expt);
		m_nStep=1;	
		
		m_nEndSnapped = IsEndSnapped()?1:0;
	}
	else if (m_nStep==1)
	{
		if (m_bSnapPt1)
		{
			PT_3DEX expt(m_SnapPt1, penLine);
			m_arrPts.Add(expt);
		}
		else
		{
			PT_3DEX expt(pt, penLine);
			m_arrPts.Add(expt);
		}
		m_nStep++;
		GrBuffer abuf,vbuf;	
		abuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(),0);
		abuf.MoveTo(&m_arrPts[0]);
		abuf.LineTo(&m_arrPts[1]);
		abuf.End();
	//	abuf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
		m_pEditor->UpdateDrag(ud_AddConstDrag,&abuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	//	m_pEditor->RefreshView();	
	}
	else
	{
		int size = m_arrPts.GetSize();
		PT_3DEX expt0;
		expt0.z = pt.z;
		expt0.pencode = penLine;
		GraphAPI::GGetRightAnglePoint(m_arrPts[size-2].x,m_arrPts[size-2].y, m_arrPts[size-1].x,m_arrPts[size-1].y,pt.x,pt.y,&(expt0.x),&(expt0.y));
		m_arrPts.Add(expt0);
		PT_3DEX expt(pt,penLine);	
		m_arrPts.Add(expt);	
		size = m_arrPts.GetSize();
		GrBuffer abuf;
		abuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(),0);
		abuf.MoveTo(&m_arrPts[size-3]);
		abuf.LineTo(&m_arrPts[size-2]);
		abuf.LineTo(&m_arrPts[size-1]);
		abuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&abuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);

		if( IsEndSnapped() )
			m_nEndSnapped |= 2;
		else
			m_nEndSnapped &= (~2);
	}

	GotoState(PROCSTATE_PROCESSING);	
	CDrawCommand::PtClick(pt,flag);
}

void CDrawLinesbyAngCommand::PtMove(PT_3D &pt)
{
	if( !m_pFtr || IsProcFinished(this) )return;
	if(m_nStep<1) return;
	if (m_nStep==1)
	{
		m_bSnapPt1 = FALSE;
		PT_3DEX expt(pt, penLine);

		Envelope e;
		e.CreateFromPtAndRadius(pt, GraphAPI::g_lfDisTolerance);
		CDataQueryEx *pDQ = PDOC(m_pEditor)->GetDataQuery();
		if (pDQ)
		{
			int num = 0;
			pDQ->FindObjectInRect(e, NULL, NULL, FALSE);
			const CPFeature *ftr = pDQ->GetFoundHandles(num);
			CGeometry *pGeo = NULL;
			for (int i = 0; i<num; i++)
			{
				CGeometry *pGeo1 = ftr[i]->GetGeometry();
				if (pGeo1 && pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					pGeo = pGeo1;
					break;
				}
			}
			if (pGeo)
			{
				CArray<PT_3DEX, PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);
				for (int j = 0; j<arrPts.GetSize() - 1; j++)
				{
					PT_3D pt1 = arrPts[j];
					PT_3D pt2 = arrPts[j + 1];
					if (GraphAPI::GIsPtInLine(pt1, pt2, pt))
					{
						PT_3D retPt;
						if (CMeasurePointToLineDistanceCommand::GetDistanceFromPointToLine(pt1, pt2, m_arrPts[0], NULL, &retPt))
						{
							retPt.z = pt.z;
							COPY_3DPT(expt, retPt);
							m_SnapPt1 = retPt;
							m_bSnapPt1 = TRUE;
							break;
						}
					}
				}
			}

			/*PT_3D pt1,pt2;
			if(pGeo && pGeo->FindNearestBaseLine(pt, e, m_pEditor->GetCoordWnd().m_pSearchCS, &pt1, &pt2,NULL))
			{
			PT_3D retPt;
			if(CMeasurePointToLineDistanceCommand::GetDistanceFromPointToLine(pt1,pt2,m_arrPts[0], NULL, &retPt))
			{
			COPY_3DPT(expt, retPt);
			}
			}*/
		}

		m_arrPts.Add(expt);		
	}
	if (m_nStep>1)
	{
		int size;
		if ((size=m_arrPts.GetSize())<2)
		{
			return;
		}
		PT_3DEX expt3;
		expt3.z=pt.z;
		expt3.pencode=penLine;
		
		
		GraphAPI::GGetRightAnglePoint(m_arrPts[size-2].x,m_arrPts[size-2].y, m_arrPts[size-1].x,m_arrPts[size-1].y,pt.x,pt.y,&(expt3.x),&(expt3.y));
		m_arrPts.Add(expt3);
		PT_3DEX expt(pt,penLine);		
		m_arrPts.Add(expt);		
	}
	GrBuffer vbuf;
	if (m_arrPts.GetSize()>=3)
	{
		int num = m_arrPts.GetSize();
		vbuf.BeginLineString(0,0);
		vbuf.MoveTo(&m_arrPts[num-3]);
		vbuf.LineTo(&m_arrPts[num-2]);
		vbuf.LineTo(&m_arrPts[num-1]);
		vbuf.End();		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
	}
	else
	{
		int num = m_arrPts.GetSize();
		vbuf.BeginLineString(0,0);
		vbuf.MoveTo(&m_arrPts[num-2]);
		vbuf.LineTo(&m_arrPts[num-1]);	
		vbuf.End();		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
	}	
	m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
	if(m_nStep>1)
    	m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
 	CDrawCommand::PtMove(pt);
}

void CDrawLinesbyAngCommand::PtReset(PT_3D &pt)
{
	if( !m_pFtr || IsProcFinished(this) )return;
	if (m_arrPts.GetSize()<2)
	{
		Abort();
		return;
	}

	if (m_bClosed && m_arrPts.GetSize() > 2 && !(m_bOpenIfSnapped && m_nEndSnapped==3))
	{
		PT_3DEX ptex;
		ptex.pencode = penLine;

		int size = m_arrPts.GetSize();
		ptex.z = m_arrPts[size-1].z;
		m_arrPts.RemoveAt(size-1);
		GraphAPI::GGetRightAnglePoint(m_arrPts[1].x,m_arrPts[1].y,m_arrPts[0].x,m_arrPts[0].y,m_arrPts[size-2].x,m_arrPts[size-2].y,&ptex.x,&ptex.y);
		m_arrPts.Add(ptex);

		m_arrPts.Add(m_arrPts[0]);
		
	}
	m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
	//((CGeoCurve*)(m_pFtr->GetGeometry()))->EnableClose(m_bClosed);
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	if( !AddObject(m_pFtr) )
	{
		Abort();
		return;
	}

	CUndoFtrs undo(m_pEditor,Name());
	undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
	undo.Commit();
	m_pEditor->RefreshView();

	Finish();
	
	CDrawCommand::PtReset(pt);	

}

void CDrawLinesbyAngCommand::Back()
{
	if (!m_pFtr)
	{
		return;
	}
	int num = m_arrPts.GetSize();
	if(num<=1)
	{
		Abort();
		m_arrPts.RemoveAll();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		m_pEditor->RefreshView();
		return ;
	}
	if( num>3 )
	{
		PT_3D pt;
		pt = m_arrPts.GetAt(num-1);			
		GrBuffer vbuf;
		vbuf.BeginLineString(0,0);
		vbuf.MoveTo(&m_arrPts.GetAt(num-1));
		vbuf.LineTo(&m_arrPts.GetAt(num-2));
		vbuf.LineTo(&m_arrPts.GetAt(num-3));
		vbuf.End();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt);		
		m_arrPts.RemoveAt(num-2,2);
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(),0);
		cbuf.Lines(m_arrPts.GetData(),m_arrPts.GetSize(),sizeof(PT_3DEX));
		cbuf.End();
		//	pObj->Draw(&vbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
	
		//	delete pObj;}
		m_pEditor->RefreshView();
	}
	else
	{
		m_nStep = 1;
		PT_3D pt;
		pt = m_arrPts.GetAt(num-1);	
		GrBuffer vbuf;
		vbuf.BeginLineString(0,0);
		vbuf.MoveTo(&m_arrPts.GetAt(num-1));
		vbuf.LineTo(&m_arrPts.GetAt(num-2));	
		vbuf.End();
		m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt);	
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
		m_pEditor->RefreshView();
	}	
	CDrawCommand::Back();	
}

void CDrawLinesbyAngCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);
  	_variant_t var;
	var = (bool)(m_bClosed);
	tab.AddValue(PF_BYANGCOLSE,&CVariantEx(var));

	var = (bool)(m_bOpenIfSnapped);
	tab.AddValue(PF_OPENIFSNAP,&CVariantEx(var));
}

BOOL CDrawLinesbyAngCommand::IsEndSnapped()
{
	if( !m_pEditor )
		return FALSE;
	
	if( !PDOC(m_pEditor)->m_snap.bOpen() )
		return FALSE;
	
	SNAPITEM item = PDOC(m_pEditor)->m_snap.GetFirstSnapResult();
	if( !item.IsValid() )
		return FALSE;
	
	if( item.nSnapMode==CSnap::modeNearPoint ||
		item.nSnapMode==CSnap::modeKeyPoint ||
		item.nSnapMode==CSnap::modeMidPoint || 
		item.nSnapMode==CSnap::modeIntersect ||
		item.nSnapMode==CSnap::modePerpPoint ||
		item.nSnapMode==CSnap::modeTangPoint ||
		item.nSnapMode==CSnap::modeEndPoint )
		return TRUE;
	
	return FALSE;
}

void CDrawLinesbyAngCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{

	param->SetOwnerID("DrawLinesbyAng",StrFromLocalResID(IDS_CMDNAME_DRAWLINESBYANG));
 	param->AddParam(PF_BYANGCOLSE,bool(m_bClosed),StrFromResID(IDS_CMDNAME_CLOSE));
	param->AddParam(PF_CLOSEDYACCKEY,'c',StrFromResID(IDS_CMDPLANE_CLOSE));
	param->AddParam(PF_REVERSEACCKEY, 't', StrFromResID(IDS_CMDPLANE_REVERSE));

	if (m_bClosed || bForLoad)
	{
		param->BeginOptionParam(PF_OPENIFSNAP,StrFromResID(IDS_CMDPLANE_OPENIFSNAP));
		param->AddOption(StrFromResID(IDS_YES),1,' ',m_bOpenIfSnapped);
		param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bOpenIfSnapped);
		param->EndOptionParam();
	}
}


void CDrawLinesbyAngCommand::SetParams(CValueTable& tab,BOOL bInit)
{		
 	const CVariantEx *var; 	
 	if( tab.GetValue(0,PF_BYANGCOLSE,var) )
	{
		m_bClosed = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,PF_CLOSEDYACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'c'||ch == 'C')
		{
			m_bClosed = !m_bClosed;
		}
		
	}

	if (tab.GetValue(0, PF_REVERSEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 't' || ch == 'T')
		{
			int nPt = m_arrPts.GetSize();
			if (nPt >= 2)
			{
				for (int i = 0; i < nPt / 2; i++)
				{
					PT_3DEX temp = m_arrPts[i];
					m_arrPts[i] = m_arrPts[nPt - 1 - i];
					m_arrPts[nPt - 1 - i] = temp;
				}
			}
		}
	}

	if( tab.GetValue(0,PF_OPENIFSNAP,var) )
	{
		m_bOpenIfSnapped = (bool)(_variant_t)*var;
	}
	CDrawCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CCreateMultiSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCreateMultiSurfaceCommand,CEditCommand)

CCreateMultiSurfaceCommand::CCreateMultiSurfaceCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\CreateMultiSurface");

	m_bKeepInnerBoundObj = FALSE;
}

CCreateMultiSurfaceCommand::~CCreateMultiSurfaceCommand()
{
}

CString CCreateMultiSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CREATEMULTISURFACE);
}

void CCreateMultiSurfaceCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();
	
	m_pEditor->OpenSelector(SELMODE_MULTI);
}

void CCreateMultiSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	int num ;
	const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( m_nStep==0 )
	{
		if( num>=2 )
		{	
			if( !CanGetSelObjs(flag,TRUE) )return;
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;
			m_pEditor->CloseSelector();
		}		
	}
	
	if( m_nStep==1 && num>=2 )
	{
		//找出选择集中的面对象，顺便计算其中面积最大的作为多面中的父面		
		CArray<FTR_HANDLE, FTR_HANDLE> arr;
		double area, max;
		int nMax = -1, nSize = num;
		for( int i=0; i<nSize; i++)
		{
			CGeometry *pObj = (CGeometry*)(HandleToFtr(handles[i])->GetGeometry());
			if( !pObj )continue;
			
			if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				continue;			
			
			arr.Add(handles[i]);
			
			Envelope e = pObj->GetShape()->GetEnvelope();
			area = (e.m_xh-e.m_xl)*(e.m_yh-e.m_yl);
			
			if( nMax<0 || area>max )
			{
				max = area;
				nMax = arr.GetSize()-1;
			}
		}
		
		if( arr.GetSize()<2 )
		{
			Abort();
			return;
		}
		
		CFtrLayer* pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(arr[nMax]));
		
		CFeature* pNewFtr = pLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOMULTISURFACE);
		if( !pNewFtr )
		{
			Abort();
			return;
		}		
		
		CValueTable tab;
		tab.BeginAddValueItem();
		GETXDS(m_pEditor)->GetXAttributes(HandleToFtr(arr[nMax]),tab);
		tab.EndAddValueItem();

		CUndoFtrs undo(m_pEditor,Name());
		((CGeoMultiSurface*)pNewFtr->GetGeometry())->CopyFromSurface(HandleToFtr(arr[nMax])->GetGeometry());
		nSize = arr.GetSize();
		CArray<PT_3DEX,PT_3DEX> arrPts;

		int id = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(arr[nMax]))->GetID();
		for( i=0; i<nSize; i++)
		{
			if( i!=nMax )
			{
				arrPts.RemoveAll();
				
				CGeoSurface* pObj = (CGeoSurface*)(HandleToFtr(arr[i])->GetGeometry());
				int nPtSum = pObj->GetDataPointSum();//需要改
				//PT_3DEX expt;
				pObj->GetShape(arrPts);
				
				((CGeoMultiSurface*)(pNewFtr->GetGeometry()))->AddSurface(arrPts);
			}
			
			if(!m_bKeepInnerBoundObj)
			{
				m_pEditor->DeleteObject(arr[i]);
				undo.arrOldHandles.Add(arr[i]);
			}
		}
		
		m_pEditor->AddObject(pNewFtr,id);
		GETXDS(m_pEditor)->SetXAttributes(pNewFtr,tab);
		undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		undo.Commit();
		Finish();
		m_nStep = 3;
	}	
	CEditCommand::PtClick(pt, flag);
}


void CCreateMultiSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CreateMultiSurface",Name());
	
	param->AddParam(PF_KEEPOLD,(bool)m_bKeepInnerBoundObj,StrFromResID(IDS_CMDPLANE_KEEPOLD));
}

void CCreateMultiSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_KEEPOLD,var) )
	{
		m_bKeepInnerBoundObj = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
}

void CCreateMultiSurfaceCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	var = (bool)(m_bKeepInnerBoundObj);
	tab.AddValue(PF_KEEPOLD,&CVariantEx(var));	
}



void CCreateMultiSurfaceCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

//////////////////////////////////////////////////////////////////////
// CSeparateMultiSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSeparateMultiSurfaceCommand,CEditCommand)

CSeparateMultiSurfaceCommand::CSeparateMultiSurfaceCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\SeparateMultiSurface");
}

CSeparateMultiSurfaceCommand::~CSeparateMultiSurfaceCommand()
{
}

CString CSeparateMultiSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SEPERATEMULTISURFACE);
}

void CSeparateMultiSurfaceCommand::Start()
{
	if( !m_pEditor )return;
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 0;
	
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}	
	
	CEditCommand::Start();	
}


BOOL CSeparateMultiSurfaceCommand::SeparateSurface(CFeature *pFtr, CFtrLayer *pLayer, CPtrArray& arrObjs)
{	
	if( !pFtr || !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )return FALSE;
	CGeoMultiSurface *pPara = (CGeoMultiSurface*)(pFtr->GetGeometry());
	
	CFeature *pTempl = pLayer->CreateDefaultFeature(2000,CLS_GEOSURFACE);
	if( !pTempl )return FALSE;
	
	arrObjs.RemoveAll();
	
	pPara->CopyToSurface(pTempl->GetGeometry());
	
	int nSurface = pPara->GetSurfaceNum();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	for(int i=0; i<nSurface; i++)
	{
		arrPts.RemoveAll();
		pPara->GetSurface(i,arrPts);
		
		if( arrPts.GetSize()>=3 )
		{
			arrPts[0].pencode = arrPts[1].pencode;

			CFeature  *pNewFtr = pTempl->Clone();
			pNewFtr->SetID(OUID());
			if( !pNewFtr )continue;			
			pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize()); 			
			arrObjs.Add(pNewFtr);
		}
		
	}
	
	delete pTempl;
	
	return arrObjs.GetSize()>0;
}


void CSeparateMultiSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CUndoFtrs undo(m_pEditor,Name());		
		CPtrArray arrObjs;
		int nsel;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		for( int i=nsel-1; i>=0; i--)
		{
		
			//生成新的对象，这里需要指定新对象层码
		/*	long lh = m_idsOld[i].layhdl;*/
			CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(handles[i]));
			
			if( SeparateSurface(HandleToFtr(handles[i]),pLayer,arrObjs) )
			{								
				for( int j=0; j<arrObjs.GetSize(); j++)
				{
					m_pEditor->AddObject((CFeature*)arrObjs.GetAt(j),pLayer->GetID());
					PDOC(m_pEditor)->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(HandleToFtr(handles[i]),(CFeature*)arrObjs.GetAt(j));
					undo.AddNewFeature(FtrToHandle((CFeature*)arrObjs.GetAt(j)));				
				}
				
				//删除原来的对象
				m_pEditor->DeleteObject(handles[i]);
				undo.AddOldFeature(handles[i]);
			}
		
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	CEditCommand::PtClick(pt, flag);
}


void CSeparateMultiSurfaceCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

//////////////////////////////////////////////////////////////////////
// CSeparateSymbolsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSeparateSymbolsCommand,CEditCommand)

CSeparateSymbolsCommand::CSeparateSymbolsCommand()
{
	m_nStep = -1;
	m_bKeepOld = FALSE;
	m_nSepMode = 0;
	strcat(m_strRegPath,"\\SeparateSymbols");
}

CSeparateSymbolsCommand::~CSeparateSymbolsCommand()
{
}

CString CSeparateSymbolsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SEPERATESYMBOLS);
}

void CSeparateSymbolsCommand::Start()
{
	if( !m_pEditor )return;

	m_bKeepOld = FALSE;
	m_nSepMode = 0;

	CEditCommand::Start();

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 0;
	
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}		
}

static void ConvertGrBufferToFtrs(GrBuffer *pBuf, CFtrLayer *pFtrLay, int nScale, CPtrArray &arr)
{
	if (!pBuf) return;

	const Graph *pGr = pBuf->HeadGraph(); 

	if (!pGr) return;
	
	while(pGr)
	{
		if (IsGrPoint(pGr))
		{
			const GrPoint *cgr = (const GrPoint*)pGr;			
			PT_3D pt = cgr->pt;

			CFeature *pFtr = pFtrLay->CreateDefaultFeature(nScale,CLS_GEOPOINT);
			if (!pFtr)
			{
				pGr = pGr->next;
				continue;
			}
			CGeoPoint *pGeo = (CGeoPoint*)pFtr->GetGeometry();
			if (!pGeo) 
			{
				delete pFtr;
				pGr = pGr->next;
				continue;
			}
			pGeo->CreateShape(&PT_3DEX(pt,penLine),1);
			if(pGeo->GetColor()==COLOUR_BYLAYER)
			{
				pGeo->SetColor(pFtrLay->GetColor());
			}
			else
			{
				pGeo->SetColor(cgr->color);
			}

			pGeo->m_fKx = cgr->kx;
			pGeo->m_fKy = cgr->ky;
			pGeo->m_fWidth = cgr->width;
			pGeo->m_lfAngle = cgr->angle*180/PI;

			arr.Add(pFtr);
		}
		else if( IsGrLineString(pGr) || IsGrPolygon(pGr) )
		{
			GrVertexList *ptlist;
			BOOL bCurve = TRUE;
			if( IsGrLineString(pGr) )
				ptlist = &((GrLineString*)pGr)->ptlist;
			else
			{
				ptlist = &((GrPolygon*)pGr)->ptlist;
				bCurve = FALSE;
			}
			
			ptlist->nuse = GraphAPI::GKickoffSamePoints(ptlist->pts,ptlist->nuse);

			GrVertex *ptNodes = ptlist->pts;
			int ptnum = ptlist->nuse;
			
			CArray<PT_3DEX,PT_3DEX> arrpts;
			arrpts.SetSize(ptnum);

			for (int i=0; i<ptnum; i++)
			{
				COPY_2DPT(arrpts[i],ptNodes[i]);
				arrpts[i].pencode = penLine;
			}

			CFeature *pFtr = pFtrLay->CreateDefaultFeature(nScale,bCurve?CLS_GEOCURVE:CLS_GEOSURFACE);
			if (!pFtr)
			{
				pGr = pGr->next;
				continue;
			}
			CGeometry *pGeo = pFtr->GetGeometry();
			if (pGeo && pGeo->CreateShape(arrpts.GetData(),arrpts.GetSize()))
			{
// 				if(pGeo->GetColor()==COLOUR_BYLAYER)
// 				{
// 					pGeo->SetColor(pFtrLay->GetColor());
// 				}
// 				else
				{
					pGeo->SetColor(pGr->color);
				}
				
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					if (((GrLineString*)pGr)->bGrdWid)
					{
						((CGeoCurve*)pGeo)->m_fLineWidth = ((GrLineString*)pGr)->width;
					}
					else
					{
						((CGeoCurve*)pGeo)->m_fLinewidthScale = ((GrLineString*)pGr)->width;
					}
					
				}
				else 
				{
					((CGeoSurface*)pGeo)->m_fLineWidth = ((GrLineString*)pGr)->width;
				}
				arr.Add(pFtr);			
			}			
			
		}
		else if (IsGrText(pGr))
		{
			GrText *pText = (GrText*)pGr;
			
			CFeature *pFtr = pFtrLay->CreateDefaultFeature(nScale,CLS_GEOTEXT);
			if (!pFtr)
			{
				pGr = pGr->next;
				continue;
			}
			CGeoText *pGeo = (CGeoText*)pFtr->GetGeometry();
			if (!pGeo)
			{
				pGr = pGr->next;
				continue;
			}

			int nPlaceType = singPt;
			{
				pGeo->CreateShape(&PT_3DEX(pText->pt.x,pText->pt.y,pText->pt.z,penLine),1);
			}

			
			if(pGeo->GetColor()==COLOUR_BYLAYER)
			{
				pGeo->SetColor(pFtrLay->GetColor());
			}
			else
			{
				pGeo->SetColor(pText->color);
			}
			pGeo->SetText(pText->text);
			
			TEXT_SETTINGS0 settings;
			settings.fHeight = pText->settings.fHeight;
			settings.fWidScale = pText->settings.fWidScale;
			settings.fCharIntervalScale = pText->settings.fCharIntervalScale;
			settings.fLineSpacingScale = pText->settings.fLineSpacingScale;
			settings.fTextAngle = pText->settings.fTextAngle;
			settings.fCharAngle = pText->settings.fCharAngle;			
			settings.nAlignment = pText->settings.nAlignment;
			settings.nInclineType = pText->settings.nInclineType;
			settings.fInclineAngle = pText->settings.fInclineAngle;	
			settings.nPlaceType = nPlaceType;
			_tcscpy(settings.strFontName,pText->settings.tcFaceName);		
			pGeo->SetSettings(&settings);

			arr.Add(pFtr);

		}

		pGr = pGr->next;
	}
}


BOOL CSeparateSymbolsCommand::SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrObjs)
{
	//一般化打散
	if( m_nSepMode==0 )
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		pDS->ExplodeSymbols(pFtr,pLayer,arrObjs);

		return (arrObjs.GetSize()>0);

	}
	//彻底打散
	else
	{	
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		CFtrArray arrInputs, arrOutputs0, arrOutputs1;

		arrInputs.Add(pFtr);

		while( arrInputs.GetSize()>0 )
		{
			for( int i=0; i<arrInputs.GetSize(); i++)
			{
				arrOutputs1.RemoveAll();
				if( pDS->ExplodeSymbols(arrInputs[i],pLayer,arrOutputs1) )
				{
					arrOutputs0.Append(arrOutputs1);
				}
				else
				{
					arrObjs.Add(arrInputs[i]);
				}				
			}
			
			arrInputs.Copy(arrOutputs0);

			arrOutputs0.RemoveAll();
		}

		for( int i=arrObjs.GetSize()-1; i>=0; i--)
		{
			if( arrObjs[i]==pFtr )
			{
				arrObjs.RemoveAt(i);
			}
		}

		return (arrObjs.GetSize()>0);
	}
}



void CSeparateSymbolsCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CUndoFtrs undo(m_pEditor,Name());
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		
		int nsel;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		for( int i=nsel-1; i>=0; i--)
		{
		
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(handles[i]));
			
			CFtrArray arrObjs;
			if( SeparateSymbols_new(HandleToFtr(handles[i]),pLayer,arrObjs) )
			{
				ObjectGroup *pNewGrp = pDS->CreateNewObjectGroup();
				for( int j=0; j<arrObjs.GetSize(); j++)
				{
					((CFeature*)arrObjs.GetAt(j))->SetPurpose(FTR_MARK);
					((CFeature*)arrObjs.GetAt(j))->AddObjectGroup(pNewGrp->id);

					m_pEditor->AddObject((CFeature*)arrObjs.GetAt(j),pLayer->GetID());
					pDS->GetXAttributesSource()->CopyXAttributes(HandleToFtr(handles[i]),(CFeature*)arrObjs.GetAt(j));

					undo.AddNewFeature(FtrToHandle((CFeature*)arrObjs.GetAt(j)));
					pNewGrp->AddObject(arrObjs.GetAt(j));
				}

				pDS->UpdateObjectGroup(pNewGrp);
				
				//删除原来的对象
				if (!m_bKeepOld)
				{
					m_pEditor->DeleteObject(handles[i]);
					undo.AddOldFeature(handles[i]);
				}
				
			}
		
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	CEditCommand::PtClick(pt, flag);
}


void CSeparateSymbolsCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CSeparateSymbolsCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("SeparateSymbolsCommand",StrFromLocalResID(IDS_CMDNAME_SEPERATESYMBOLS));
	
	param->AddParam(PF_KEEPOLD,(bool)m_bKeepOld,StrFromResID(IDS_CMDPLANE_KEEPOLD));

	param->BeginOptionParam(PF_SEPSYMMOD,StrFromResID(IDS_CMDPLANE_SEPSYMMOD));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEPSYMMOD_STEP),0,' ',m_nSepMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEPSYMMOD_FINAL),1,' ',m_nSepMode==1);
	param->EndOptionParam();
}

void CSeparateSymbolsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_KEEPOLD,var) )
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_SEPSYMMOD,var) )
	{
		m_nSepMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
}

void CSeparateSymbolsCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_KEEPOLD,&CVariantEx(var));

	var = (_variant_t)(long)(m_nSepMode);
	tab.AddValue(PF_SEPSYMMOD,&CVariantEx(var));
}


//////////////////////////////////////////////////////////////////////
// CCreateSymbolsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCreateSymbolsCommand,CEditCommand)

CCreateSymbolsCommand::CCreateSymbolsCommand()
{
	m_nStep = -1;
	m_bKeepOld = FALSE;
	strcat(m_strRegPath,"\\CreateSymbols");
}

CCreateSymbolsCommand::~CCreateSymbolsCommand()
{
}

CString CCreateSymbolsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CREATESYMBOLS);
}

void CCreateSymbolsCommand::Start()
{
	if( !m_pEditor )return;

	m_bKeepOld = FALSE;

	CEditCommand::Start();

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 0;
	
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}		
}


BOOL CCreateSymbolsCommand::SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrObjs)
{
	//一般化打散
	if( 1 )
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		return pDS->ExplodeSymbols(pFtr,pLayer,arrObjs,m_strRetLay);

	}
	//彻底打散
	else
	{	
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		CFtrArray arrInputs, arrOutputs0, arrOutputs1;

		arrInputs.Add(pFtr);

		while( arrInputs.GetSize()>0 )
		{
			for( int i=0; i<arrInputs.GetSize(); i++)
			{
				arrOutputs1.RemoveAll();
				if( pDS->ExplodeSymbols(arrInputs[i],pLayer,arrOutputs1) )
				{
					arrOutputs0.Append(arrOutputs1);
				}
				else
				{
					arrObjs.Add(arrInputs[i]);
				}				
			}
			
			arrInputs.Copy(arrOutputs0);

			arrOutputs0.RemoveAll();
		}

		for( int i=arrObjs.GetSize()-1; i>=0; i--)
		{
			if( arrObjs[i]==pFtr )
			{
				arrObjs.RemoveAt(i);
			}
		}
		return (arrObjs.GetSize()>0);
	}
}


void CCreateSymbolsCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		int num ;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num == 1)
		{
			CGeometry *pObj1 = HandleToFtr(handles[0])->GetGeometry();
			
			int cls1 = pObj1->GetClassType();
			
			if( cls1 != CLS_GEOCURVE )
			{
				GOutPut(StrFromResID(IDS_CMDTIP_MERGECURVE_TIP));
				return;
			}
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;
			
			m_arrFtrs[0] = handles[0];
		}
		else if( num==2 )
		{	
			if( !CanGetSelObjs(flag,TRUE) )return;
			CGeometry *pObj1 = HandleToFtr(handles[0])->GetGeometry();
			CGeometry *pObj2 = HandleToFtr(handles[1])->GetGeometry();
			
			if( pObj1==NULL || pObj2==NULL )
				return;

			int cls1 = pObj1->GetClassType(), cls2 = pObj2->GetClassType();
			
			if( cls1 != CLS_GEOCURVE || cls2 != CLS_GEOCURVE )
			{
				GOutPut(StrFromResID(IDS_CMDTIP_MERGECURVE_TIP));
				return;
			}
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
			m_pEditor->CloseSelector();

			m_arrFtrs[0] = handles[0];
			m_arrFtrs[1] = handles[1];
		}
	}
	else if (m_nStep == 1)
	{
		FTR_HANDLE handle = m_pEditor->GetSelection()->GetLastSelectedObj();
		if (handle == 0 || handle == m_arrFtrs[0]) return;

		CGeometry *pObj1 = HandleToFtr(m_arrFtrs[0])->GetGeometry();
		CGeometry *pObj2 = HandleToFtr(handle)->GetGeometry();
		
		if( pObj1==NULL || pObj2==NULL )
			return;
		
		int cls1 = pObj1->GetClassType(), cls2 = pObj2->GetClassType();
		
		if( cls1 != CLS_GEOCURVE || cls2 != CLS_GEOCURVE )
		{
			GOutPut(StrFromResID(IDS_CMDTIP_MERGECURVE_TIP));
			return;
		}
		
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 2;
		m_pEditor->CloseSelector();
		
		m_arrFtrs[1] = handle;

	}
	
	if( m_nStep==2 )
	{
		// 获取双线类型
		CDlgDataSource *pDS = GETDS(m_pEditor);

		CFtrLayer *pLayer = GETDS(m_pEditor)->GetFtrLayer(m_strRetLay);
		if (!pLayer)
		{
			Abort();
			return;
		}

		CFeature *pFtr = pLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale());
		if (!pFtr || !pFtr->GetGeometry())
		{
			Abort();
			return;
		}

		int clstype = pFtr->GetGeometry()->GetClassType();
		if (clstype != CLS_GEOPARALLEL && clstype != CLS_GEODCURVE)
		{
			Abort();
			return;
		}

		CGeoCurve *pObj1 = (CGeoCurve*)HandleToFtr(m_arrFtrs[0])->GetGeometry();
		CGeoCurve *pObj2 = (CGeoCurve*)HandleToFtr(m_arrFtrs[1])->GetGeometry();
		
		CArray<PT_3DEX,PT_3DEX> arr1,arr2;
		pObj1->GetShape(arr1);
		pObj2->GetShape(arr2);

		// 判断是否可生成平行线
		if (clstype == CLS_GEOPARALLEL)
		{
			BOOL bParallel = TRUE;
			int num1 = arr1.GetSize(), num2 = arr2.GetSize();
			if ( GraphAPI::GGetLineIntersectLine(arr1[0].x,arr1[0].y,arr1[num1-1].x-arr1[0].x,arr1[num1-1].y-arr1[0].y,
				arr2[0].x,arr2[0].y,arr2[num2-1].x-arr2[0].x,arr2[num2-1].y-arr2[0].y,NULL,NULL,NULL) )
			{
				bParallel = FALSE;
			}
			else
			{
				PT_3DEX ret;
				double dis1 = GraphAPI::GGetNearstDisPToPointSet2D(arr1.GetData(),arr1.GetSize(),arr2[0],ret,NULL);
				double dis2 = GraphAPI::GGetNearstDisPToPointSet2D(arr1.GetData(),arr1.GetSize(),arr2[num2-1],ret,NULL);
				
				if (fabs(dis1-dis2) > 1e-4)
				{
					bParallel = FALSE;
				}
			}

			if (!bParallel)
			{
				clstype = CLS_GEODCURVE;
				delete pFtr;
				pFtr = pLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),CLS_GEODCURVE);
				if (!pFtr || !pFtr->GetGeometry())
				{
					Abort();
					return;
				}
			}
		}

		CGeometry *pGeo = pFtr->GetGeometry();

		if (clstype == CLS_GEOPARALLEL)
		{
			if (!pGeo->CreateShape(arr1.GetData(),arr1.GetSize()) || !((CGeoParallel*)pGeo)->SetCtrlPoint(0,arr2[0]))
			{
				delete pFtr;
				Abort();
				return;
			}
		}
		else if (clstype == CLS_GEODCURVE)
		{
			CArray<PT_3DEX,PT_3DEX> pts;
			pts.Append(arr1);
			arr2[0].pencode = penMove;
			pts.Append(arr2);
			if (!pGeo->CreateShape(pts.GetData(),pts.GetSize()))
			{
				delete pFtr;
				Abort();
				return;
			}
		}
		else
		{
			delete pFtr;
			Abort();
			return;
		}

		CValueTable tab;
		tab.BeginAddValueItem();
		HandleToFtr(m_arrFtrs[0])->WriteTo(tab);
		tab.EndAddValueItem();

		tab.DelField(FIELDNAME_SHAPE);
		tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
		tab.DelField(FIELDNAME_FTRDELETED);
		tab.DelField(FIELDNAME_FTRID);
		tab.DelField(FIELDNAME_GEOCLASS);
		tab.DelField(FIELDNAME_FTRDISPLAYORDER);
		tab.DelField(FIELDNAME_FTRGROUPID);

		//pFtr->ReadFrom(tab);

//		pDS->GetXAttributesSource()->CopyXAttributes(HandleToFtr(m_arrFtrs[0]),pFtr);

		CFtrArray arrObjs;
		if( SeparateSymbols_new(pFtr,pLayer,arrObjs) )
		{	
			CUndoFtrs undo(m_pEditor,Name());

			for( int j=0; j<arrObjs.GetSize(); j++)
			{
				((CFeature*)arrObjs.GetAt(j))->SetPurpose(FTR_MARK);
				m_pEditor->AddObject((CFeature*)arrObjs.GetAt(j),pLayer->GetID());
//				PDOC(m_pEditor)->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(HandleToFtr(m_arrFtrs[0]),(CFeature*)arrObjs.GetAt(j));
				undo.AddNewFeature(FtrToHandle((CFeature*)arrObjs.GetAt(j)));				
			}
			
			//删除原来的对象
			if (!m_bKeepOld)
			{
				m_pEditor->DeleteObject(m_arrFtrs[0]);
				m_pEditor->DeleteObject(m_arrFtrs[1]);
				undo.AddOldFeature(m_arrFtrs[0]);
				undo.AddOldFeature(m_arrFtrs[1]);
			}

			undo.Commit();
			
		}

		delete pFtr;
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 3;
	}

	CEditCommand::PtClick(pt, flag);
}


void CCreateSymbolsCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CCreateSymbolsCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CreateSymbolsCommand",StrFromLocalResID(IDS_CMDNAME_CREATESYMBOLS));
	
	param->AddLayerNameParam(PF_CREATESYMBOLS_LAYERNAME,(LPCTSTR)m_strRetLay,StrFromResID(IDS_CMDPLANE_CREATESYMSLAYERNAME));

	param->BeginOptionParam(PF_KEEPOLD,StrFromResID(IDS_CMDPLANE_KEEPOLD));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bKeepOld);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bKeepOld);
	param->EndOptionParam();
	
}

void CCreateSymbolsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_CREATESYMBOLS_LAYERNAME,var) )
	{
		m_strRetLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_KEEPOLD,var) )
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

}

void CCreateSymbolsCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strRetLay;
	tab.AddValue(PF_CREATESYMBOLS_LAYERNAME,&CVariantEx(var));

	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_KEEPOLD,&CVariantEx(var));

}



//////////////////////////////////////////////////////////////////////
// CCreateHatchSymbolsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCreateHatchSymbolsCommand,CEditCommand)

CCreateHatchSymbolsCommand::CCreateHatchSymbolsCommand()
{
	m_nStep = -1;
	m_bKeepOld = TRUE;
	strcat(m_strRegPath,"\\CreateHatchSymbols");
}

CCreateHatchSymbolsCommand::~CCreateHatchSymbolsCommand()
{
}

CString CCreateHatchSymbolsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CREATEHATCHSYMBOLS);
}

void CCreateHatchSymbolsCommand::Start()
{
	if( !m_pEditor )return;

	m_bKeepOld = FALSE;

	CEditCommand::Start();

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num==1 )
	{
		m_nStep = 0;
	
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}		
}


void CCreateHatchSymbolsCommand::DeleteSomeSymbols(CPtrArray& arrPSyms)
{
	//只保留填充符号
	for( int i=arrPSyms.GetSize()-1; i>=0; i--)
	{
		CSymbol *pSym = (CSymbol*)arrPSyms[i];
		int type = pSym->GetType();
		
		BOOL bDel = TRUE;
		
		if( type==SYMTYPE_CELLHATCH )
		{
			bDel = FALSE;
		}
		else if( type==SYMTYPE_CELL )
		{
			CCell *pCell = (CCell*)pSym;
			if( pCell->m_nPlaceType==CCell::Center )
			{
				bDel = FALSE;
			}
		}
		else if( type==SYMTYPE_LINEHATCH )
		{
			bDel = FALSE;
		}
		
		if( bDel )
		{
			arrPSyms.RemoveAt(i);
		}
	}
}


BOOL CCreateHatchSymbolsCommand::SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrObjs)
{
	//一般化打散
	if( 1 )
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();	
		
		CPtrArray arrPSyms;
		gpCfgLibMan->GetSymbol(pDS,pFtr,arrPSyms,pLayer->GetName());

		DeleteSomeSymbols(arrPSyms);
		if( arrPSyms.GetSize()<=0 )
			return FALSE;

		return pDS->ExplodeSymbols(pFtr,arrPSyms,pLayer,arrObjs);

	}
	//彻底打散
	else
	{	
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		CFtrArray arrInputs, arrOutputs0, arrOutputs1;

		arrInputs.Add(pFtr);

		while( arrInputs.GetSize()>0 )
		{
			for( int i=0; i<arrInputs.GetSize(); i++)
			{
				arrOutputs1.RemoveAll();
				if( pDS->ExplodeSymbols(arrInputs[i],pLayer,arrOutputs1) )
				{
					arrOutputs0.Append(arrOutputs1);
				}
				else
				{
					arrObjs.Add(arrInputs[i]);
				}				
			}
			
			arrInputs.Copy(arrOutputs0);

			arrOutputs0.RemoveAll();
		}

		for( int i=arrObjs.GetSize()-1; i>=0; i--)
		{
			if( arrObjs[i]==pFtr )
			{
				arrObjs.RemoveAt(i);
			}
		}
		return (arrObjs.GetSize()>0);
	}
}


void CCreateHatchSymbolsCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;

		int num ;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num == 1)
		{
			CGeometry *pObj1 = HandleToFtr(handles[0])->GetGeometry();
						
			if (!pObj1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				GOutPut(StrFromResID(IDS_CMDTIP_CREATESYMS_TIP));
				return;
			}
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
			
			m_hFtr = handles[0];
		}		
	}
	
	if( m_nStep==2 )
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);

		CFtrLayer *pLayer = GETDS(m_pEditor)->GetFtrLayer(m_strRetLay);
		if (!pLayer)
		{
			Abort();
			return;
		}

		CFeature *pFtr = pLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale());
		if (!pFtr || !pFtr->GetGeometry())
		{
			Abort();
			return;
		}

		CGeometry *pGeo = pFtr->GetGeometry();

		if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			delete pFtr;
			Abort();
			return;
		}

		CFeature *pFtr0 = HandleToFtr(m_hFtr);

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pFtr0->GetGeometry()->GetShape(arrPts);

		if( !pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize()) )
		{
			delete pFtr;
			Abort();
			return;
		}

		CValueTable tab;
		tab.BeginAddValueItem();
		pFtr0->WriteTo(tab);
		tab.EndAddValueItem();

		tab.DelField(FIELDNAME_SHAPE);
		tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
		tab.DelField(FIELDNAME_FTRDELETED);
		tab.DelField(FIELDNAME_FTRID);
		tab.DelField(FIELDNAME_GEOCLASS);
		tab.DelField(FIELDNAME_FTRDISPLAYORDER);
		tab.DelField(FIELDNAME_FTRGROUPID);

		pFtr->ReadFrom(tab);
		pFtr->GetGeometry()->SetSymbolName("");

		CFtrArray arrObjs;
		if( SeparateSymbols_new(pFtr,pLayer,arrObjs) )
		{	
			CUndoFtrs undo(m_pEditor,Name());

			for( int j=0; j<arrObjs.GetSize(); j++)
			{
				((CFeature*)arrObjs.GetAt(j))->SetPurpose(FTR_MARK);
				m_pEditor->AddObject((CFeature*)arrObjs.GetAt(j),pLayer->GetID());
				undo.AddNewFeature(FtrToHandle((CFeature*)arrObjs.GetAt(j)));				
			}
			
			//删除原来的对象
			if (!m_bKeepOld)
			{
				m_pEditor->DeleteObject(m_hFtr);
				undo.AddOldFeature(m_hFtr);
			}

			undo.Commit();
			
		}

		delete pFtr;
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 3;
	}

	CEditCommand::PtClick(pt, flag);
}


void CCreateHatchSymbolsCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CCreateHatchSymbolsCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CreateSymbolsCommand",StrFromLocalResID(IDS_CMDNAME_CREATEHATCHSYMBOLS));
	
	param->AddLayerNameParam(PF_CREATESYMBOLS_LAYERNAME,(LPCTSTR)m_strRetLay,StrFromResID(IDS_CMDPLANE_CREATESYMSLAYERNAME));

	param->BeginOptionParam(PF_KEEPOLD,StrFromResID(IDS_CMDPLANE_KEEPOLD));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bKeepOld);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bKeepOld);
	param->EndOptionParam();
	
}

void CCreateHatchSymbolsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_CREATESYMBOLS_LAYERNAME,var) )
	{
		m_strRetLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_KEEPOLD,var) )
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

}

void CCreateHatchSymbolsCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strRetLay;
	tab.AddValue(PF_CREATESYMBOLS_LAYERNAME,&CVariantEx(var));

	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_KEEPOLD,&CVariantEx(var));

}

IMPLEMENT_DYNAMIC(CMakeCheckPtSampleCommand,CEditCommand)


CString CMakeCheckPtSampleCommand::Name()
{
	return StrFromResID(IDS_MAKE_CHECKPTSAMPLE);
}

void CMakeCheckPtSampleCommand::PtClick(PT_3D &pt, int flag)
{
	PDOC(m_pEditor)->MakeCheckPtSample();
}


void CMakeCheckPtSampleCommand::Start()
{
	if( m_pEditor )
	{
		m_pEditor->CloseSelector();
	}
	
	m_nStep = 0;	
	CCommand::Start();
}


void CMakeCheckPtSampleCommand::PtReset(PT_3D &pt)
{
	CEditCommand::PtReset(pt);
}


//////////////////////////////////////////////////////////////////////
// CCopyEXTAttributionCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCopyEXTAttributionCommand,CEditCommand)

CCopyEXTAttributionCommand::CCopyEXTAttributionCommand()
{
	m_nStep = -1;
}

CCopyEXTAttributionCommand::~CCopyEXTAttributionCommand()
{
	
}

CString CCopyEXTAttributionCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPYEXT);
}

void CCopyEXTAttributionCommand::Start()
{
	if( !m_pEditor )return;
//	m_pEditor->DeselectAll();
	CEditCommand::Start();
	
	m_pEditor->OpenSelector(SELMODE_MULTI);
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num==1 )
	{
		
		m_handleRefer = m_pEditor->GetSelection()->GetLastSelectedObj();
		CFeature *pp = HandleToFtr(m_handleRefer);
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 1;
		
		//开始选择目标地物
	//	OutputTipString(StrFromResID(IDS_CMDTIP_SELECTTARGET));
	}
	else
	{
	//	OutputTipString(StrFromResID(IDS_CMDTIP_SELECTREFER));
	}
}

void CCopyEXTAttributionCommand::Abort()
{
	m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();
	
	CEditCommand::Abort();
}

void CCopyEXTAttributionCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)(m_strFieldName);
	tab.AddValue("CopyFields",&CVariantEx(var));
}

void CCopyEXTAttributionCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CopyEXTAttributionCommand",Name());	
	param->AddParam("CopyFields",(LPCTSTR)m_strFieldName,StrFromResID(IDS_CMDPLANE_FIELDNAME));
}


void CCopyEXTAttributionCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;

	if( tab.GetValue(0,"CopyFields",var) )
	{
		m_strFieldName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CCopyEXTAttributionCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( (flag&SELSTAT_POINTSEL)==0 && (flag&SELSTAT_DRAGSEL)==0 )
			return;
		int num;
		m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num!=1 )
			return;		
		
		m_handleRefer = m_pEditor->GetSelection()->GetLastSelectedObj();
		
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 1;
		
		//开始选择目标地物
		PromptString(StrFromResID(IDS_CMDTIP_SELECTTARGET));
	}
	else if( m_nStep==1 )
	{
		if( (flag&SELSTAT_MULTISEL)!=0 && (flag&SELSTAT_POINTSEL)==0 && (flag&SELSTAT_DRAGSEL_RESTART)==0 )
			return;

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS)
		{
			Abort();
			return;
		}
		CAttributesSource *pXDS = pDS->GetXAttributesSource();
		if(!pXDS)
		{
			Abort();
			return;
		}
	
		CFeature *pFtr = HandleToFtr(m_handleRefer);
		CValueTable tab;
		tab.BeginAddValueItem();
		GETXDS(m_pEditor)->GetXAttributes(pFtr, tab);
		tab.EndAddValueItem();

		if(!m_strFieldName.IsEmpty())
		{
			CStringArray arr;
			convertStringToStrArray(m_strFieldName, arr);
			ChkFieldName(arr, tab);
		}

		CUndoModifyProperties undo(m_pEditor,Name());
		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		for( int i=0; i<num; i++)
		{
			if( handles[i] && m_handleRefer!=handles[i] )
			{
				CFeature *pFtr1 = HandleToFtr(handles[i]);
				
				CValueTable oldTab, newTab;
				oldTab.BeginAddValueItem();
				pXDS->GetXAttributes(pFtr1, oldTab);
				oldTab.EndAddValueItem();
				
				pXDS->SetXAttributes(pFtr1, tab);

				newTab.BeginAddValueItem();
				pXDS->GetXAttributes(pFtr1, newTab);
				newTab.EndAddValueItem();
				
				undo.SetModifyProperties(handles[i], oldTab, newTab, FALSE);
			}
		}
		
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->SelectObj(m_handleRefer);
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		m_nStep = 1;

		PromptString(StrFromResID(IDS_CMDTIP_SELECTTARGET));
	}	
	CEditCommand::PtClick(pt,flag);
}

void CCopyEXTAttributionCommand::ChkFieldName(CStringArray& arr, CValueTable& tab)
{
	CValueTable temp;
	temp.BeginBatchAddValueItem();
	for(int i=0; i<arr.GetSize(); i++)
	{
		const CVariantEx *var;
		if(tab.GetValue(0, arr[i], var))
		{
			_variant_t var1 = *var;
			temp.AddValue(arr[i], &CVariantEx(var1));
		}
	}
	temp.EndAddValueItem();

	tab.CopyFrom(temp);
}



//////////////////////////////////////////////////////////////////////
// CCreateSurfacePtFromSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCreateSurfacePtFromSurfaceCommand,CEditCommand)

CCreateSurfacePtFromSurfaceCommand::CCreateSurfacePtFromSurfaceCommand()
{
}

CCreateSurfacePtFromSurfaceCommand::~CCreateSurfacePtFromSurfaceCommand()
{
	
}

CString CCreateSurfacePtFromSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SURFACEPT_FROM_SURFACE);
}

void CCreateSurfacePtFromSurfaceCommand::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();
	
	m_pEditor->CloseSelector();
}

void CCreateSurfacePtFromSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS)
	{
		return;
	}

	CFtrArray ftrs;
	CFtrLayerArray ftrlays;

	int nSum = 0;

	int i, j, k;
	int nLayer = pDS->GetFtrLayerCount();
	for(i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(i);
		if(!pLayer)
			continue;
		
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr)
				continue;
			
			if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
			{
				ftrs.Add(pFtr);
				ftrlays.Add(pLayer);
			}

			if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				nSum++;
			}
		}
	}

	GProgressStart(nSum);

	CUndoFtrs undo(m_pEditor,Name());
	for(i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(i);
		if(!pLayer || pLayer->IsLocked() )
			continue;
		
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr)
				continue;
			
			if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				GProgressStep();
				BOOL bFindSurfacePt = FALSE;
				for(k=0; k<ftrs.GetSize(); k++)
				{
					PT_3D pt = ftrs[k]->GetGeometry()->GetDataPoint(0);
					if(stricmp(ftrlays[k]->GetName(),pLayer->GetName())==0 && pFtr->GetGeometry()->GetEnvelope().bPtIn(&pt))
					{
						if(((CGeoSurface*)pFtr->GetGeometry())->bPtIn(&pt))
						{
							bFindSurfacePt = TRUE;
							break;
						}
					}
				}
				
				if(!bFindSurfacePt)
				{
					CArray<PT_3D,PT_3D> arrPts;
					GetSurfaceInnerPoint(pFtr->GetGeometry(),arrPts);
					for(k=0; k<arrPts.GetSize(); k++)
					{
						PT_3D pt = arrPts[k];
						PT_3DEX expt(pt.x,pt.y,pt.z,penLine);
						CFeature *pNewFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOSURFACEPOINT);

						pNewFtr->GetGeometry()->CreateShape(&expt,1);

						if( m_pEditor->AddObject(pNewFtr,pLayer->GetID()) )
						{
							undo.AddNewFeature(FtrToHandle(pNewFtr));
						}
					}
				}					
			}
		}
	}

	GProgressEnd();

	undo.Commit();
	m_pEditor->RefreshView();
	Finish();
	return;
}

BOOL CCreateSurfacePtFromSurfaceCommand::GetSurfaceInnerPoint_single(CGeometry *pObj, CGeoArray& sons, PT_3D& pt_ret)
{
	CGeoArray sons2;
	sons2.Add(pObj);
	sons2.Append(sons);

	Envelope e = pObj->GetEnvelope();
	PT_3D cpt;
	pObj->GetCenter(NULL,&cpt);
		
	PT_3DEX pttest1, pttest2;
	pttest1.x = e.m_xl - e.Width(); pttest1.y = cpt.y;
	pttest2.x = e.m_xl + e.Width(); pttest2.y = cpt.y;

	CArray<PT_3DEX,PT_3DEX> arrPts_intersect1, arrPts_intersect2;
	double z = 0;
	int npt = 0;

	for(int i=0; i<sons2.GetSize();i++)
	{
		CGeometry *pObj2 = sons2[i];

		CArray<PT_3DEX,PT_3DEX> arrPts0;
		pObj2->GetShape(arrPts0);

		for(int j=0; j<arrPts0.GetSize(); j++)
		{
			z += arrPts0[j].z;
		}
		npt += arrPts0.GetSize();
		
		arrPts_intersect1.SetSize(arrPts0.GetSize());
		int nRet = 0;
		GraphAPI::GGetPointsOfIntersect(arrPts0.GetData(),arrPts0.GetSize(),pttest1,pttest2,arrPts_intersect1.GetData(),&nRet);
		arrPts_intersect1.SetSize(nRet);
		arrPts_intersect2.Append(arrPts_intersect1);
	}

	GraphAPI::GSortXY(arrPts_intersect2.GetData(),arrPts_intersect2.GetSize());

	int nintersect = GraphAPI::GKickoffSame2DPoints(arrPts_intersect2.GetData(),arrPts_intersect2.GetSize());

	if(nintersect>1)
	{
		pt_ret.x = (arrPts_intersect2[0].x+arrPts_intersect2[1].x)*0.5;
		pt_ret.y = (arrPts_intersect2[0].y+arrPts_intersect2[1].y)*0.5;
		pt_ret.z = z/npt;

		return TRUE;
	}

	return FALSE;
}

void CCreateSurfacePtFromSurfaceCommand::GetSurfaceInnerPoint(CGeometry *pObj, CArray<PT_3D,PT_3D>& arrPts)
{
	if(pObj->GetClassType()==CLS_GEOSURFACE)
	{
		PT_3D pt_ret;
		if(GetSurfaceInnerPoint_single(pObj,CGeoArray(),pt_ret))
		{
			arrPts.Add(pt_ret);
		}
	}
	else if(pObj->GetClassType()==CLS_GEOMULTISURFACE)
	{
		CGeoMultiSurface *pMObj = (CGeoMultiSurface*)pObj;

		int nSon = pMObj->GetSurfaceNum();

		//1、生成各个子面对象
		int i, j, k;
		CArray<CGeoSurface*,CGeoSurface*> sonSurfaces;
		CArray<double,double> areas;
		CArray<PT_3DEX,PT_3DEX> arrPts1;
		for(i=0; i<nSon; i++)
		{
			arrPts1.RemoveAll();
			pMObj->GetSurface(i,arrPts1);
			if(arrPts1.GetSize()<3)
				continue;
			
			CGeoSurface *pObj1 = new CGeoSurface();
			pObj1->CreateShape(arrPts1.GetData(),arrPts1.GetSize());
			sonSurfaces.Add(pObj1);
			areas.Add(pObj1->GetArea());
		}

		//2、依次取面积最大的，合成无孤岛的复杂面
		nSon = sonSurfaces.GetSize();
		while(1)
		{
			double max_area=-1;
			int index = -1;
			for(i=0; i<nSon; i++)
			{
				if(areas[i]<0)
					continue;

				if(max_area<0 || max_area<areas[i])
				{
					max_area = areas[i];
					index = i;
				}
			}
			if(index<0)break;

			areas[index] = -1;

			CGeoSurface *cur_obj = sonSurfaces[index];

			//3、寻找该面的包含面，并去掉多重包含的面
			CArray<CGeoSurface*,CGeoSurface*> newSons;
			for(i=0; i<nSon; i++)
			{
				if(areas[i]<0)
					continue;

				CGeoSurface *p1 = sonSurfaces[i];

				PT_3DEX pt1 = sonSurfaces[i]->GetDataPoint(0);
				if(cur_obj->bPtIn(&pt1))
				{
					for(j=0; j<newSons.GetSize(); j++)
					{
						CGeoSurface *p2 = newSons[j];
						PT_3DEX pt2 = p2->GetDataPoint(0);
						if(p2->bPtIn(&pt1))
						{
							newSons[j] = p1;
							break;
						}
						else if(p1->bPtIn(&pt2))
						{							
							break;
						}
					}

					if(j>=newSons.GetSize())
					{
						newSons.Add(p1);
					}
				}
			}

			//将上述面从sonSurfaces面列表中去掉
			for(i=0; i<nSon; i++)
			{			
				for(j=0; j<newSons.GetSize(); j++)
				{
					if(sonSurfaces[i]==newSons[j])
					{
						areas[i] = -1;
						break;
					}
				}				
			}

			//由 cur_obj 和 newSons 生成内部点
			CGeoArray newSons2;
			for(j=0; j<newSons.GetSize(); j++)
			{
				newSons2.Add(newSons[j]);
			}

			PT_3D pt_ret;
			if(GetSurfaceInnerPoint_single(cur_obj,newSons2,pt_ret))
			{
				arrPts.Add(pt_ret);
			}
		}

		for(i=0; i<nSon; i++)
		{
			delete sonSurfaces[i];
		}
	}
}
