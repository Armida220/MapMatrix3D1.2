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

#include "PolygonWork.h"
#include "PolygonBooleanHander.h"
#include "GeoSurfaceBooleanHandle.h"

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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CLineToDirPointCommand,CEditCommand)

CLineToDirPointCommand::CLineToDirPointCommand()
{
}

CLineToDirPointCommand::~CLineToDirPointCommand()
{
}

CString CLineToDirPointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_LINETOPOINT);
}

void CLineToDirPointCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		
		return;
	}
	
	CEditCommand::Start();
}

void CLineToDirPointCommand::LineToDirPoint(const FTR_HANDLE* handles,int num)
{
	CUndoFtrs undo(m_pEditor,Name());
	
	for (int i=0;i<num;i++)
	{
		CGeometry *pGeo = HandleToFtr(handles[i])->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			continue;
		}
				
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);
		
		if (pts.GetSize() != 2) continue;
		
		CFeature *pNew = HandleToFtr(handles[i])->Clone();
		if (!pNew) continue;
		
		if (!pNew->CreateGeometry(CLS_GEODIRPOINT)) continue;
		
		pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
		
		if (!m_pEditor->AddObject(pNew,m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
		{
			delete pNew;
			continue;
		}
		
		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handles[i]);		
		m_pEditor->DeleteObject(handles[i]);
	}
	
	undo.Commit();
	
}

void CLineToDirPointCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		LineToDirPoint(handles,num);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CLineToDirPoint1Command,CEditCommand)

CLineToDirPoint1Command::CLineToDirPoint1Command()
{
}

CLineToDirPoint1Command::~CLineToDirPoint1Command()
{
}

CString CLineToDirPoint1Command::Name()
{ 
	return StrFromResID(IDS_CMDNAME_LINETOPOINT);
}

void CLineToDirPoint1Command::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CLineToDirPoint1Command::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue(PF_HANDLELAYERS,&CVariantEx(var));
	
	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue(PF_LAYERNAME,&CVariantEx(var));
}

void CLineToDirPoint1Command::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("LineToDirPoint1Command",Name());
	
	param->AddLayerNameParamEx(PF_HANDLELAYERS,(LPCTSTR)m_handleLayers,StrFromResID(IDS_CMDPLANE_LINELAYER),NULL,LAYERPARAMITEM_LINE);
	
	param->AddLayerNameParam(PF_LAYERNAME,(LPCTSTR)m_targetLayer,StrFromResID(IDS_CMDPLANE_DIRPOINTLAYER),NULL,LAYERPARAMITEM_POINT);
}

void CLineToDirPoint1Command::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_HANDLELAYERS,var) )
	{					
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,PF_LAYERNAME,var) )
	{					
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimRight();
		m_targetLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CLineToDirPoint1Command::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor || m_targetLayer.IsEmpty())
		return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	CFtrLayer *out_layer = pDS->GetFtrLayer((LPCTSTR)m_targetLayer);
	if(out_layer == NULL)
	{
		out_layer=new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_targetLayer);
		pDS->AddFtrLayer(out_layer);
	}
	m_layerID = out_layer->GetID();
	CFeature *pTemp = out_layer->CreateDefaultFeature(pDS->GetScale(), CLS_GEODIRPOINT);

	CFtrLayerArray arrLayers;
	int i=0, j=0;
	pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers,arrLayers);

	CUndoFtrs undo(m_pEditor,Name());
	
	for (i=0;i<arrLayers.GetSize();i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if(!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				continue;
			}
						
			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);
			
			if (pts.GetSize() != 2) continue;
			
			CFeature *pNew = pTemp->Clone();
			if (!pNew) continue;
			
			pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
			
			if (!m_pEditor->AddObject(pNew,m_layerID))
			{
				delete pNew;
				continue;
			}
			
			GETXDS(m_pEditor)->CopyXAttributes(pFeature,pNew);
			undo.AddNewFeature(FTR_HANDLE(pNew));
			undo.AddOldFeature(FtrToHandle(pFeature));		
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
		}
	}
	
	if(pTemp) delete pTemp;
	undo.Commit();
	
	CCommand::PtClick(pt, flag);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPointToSurfacePointCommand,CEditCommand)

CPointToSurfacePointCommand::CPointToSurfacePointCommand()
{
}

CPointToSurfacePointCommand::~CPointToSurfacePointCommand()
{
}

CString CPointToSurfacePointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_POINTTOSURFACEPOINT);
}

void CPointToSurfacePointCommand::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CPointToSurfacePointCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue(PF_HANDLELAYERS,&CVariantEx(var));
	
	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue(PF_LAYERNAME,&CVariantEx(var));
}

void CPointToSurfacePointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("PointToSurfacePoint",Name());
	
	param->AddLayerNameParamEx(PF_HANDLELAYERS,(LPCTSTR)m_handleLayers,StrFromResID(IDS_SRC_LAYER),NULL);
	
	param->AddLayerNameParam(PF_LAYERNAME,(LPCTSTR)m_targetLayer,StrFromResID(IDS_DES_LAYER),NULL,LAYERPARAMITEM_POINT);
}

void CPointToSurfacePointCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_HANDLELAYERS,var) )
	{					
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,PF_LAYERNAME,var) )
	{					
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimRight();
		m_targetLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CPointToSurfacePointCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor)
		return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	CFtrLayer *out_layer = NULL;
	if (!m_targetLayer.IsEmpty())
		out_layer = pDS->GetOrCreateFtrLayer((LPCTSTR)m_targetLayer);

	CFtrLayerArray arrLayers;
	int i=0, j=0;
	pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers,arrLayers);

	CUndoFtrs undo(m_pEditor,Name());

	CFeature *pTempFtr = NULL;
	if(out_layer)
	{
		pTempFtr = out_layer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOSURFACEPOINT);
		if(!pTempFtr) return;
	}
	
	for (i=0;i<arrLayers.GetSize();i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if(!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || (CLS_GEOPOINT!=pGeo->GetClassType()))
			{
				continue;
			}
						
			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);			

			CFeature *pNew = NULL;
			if (pTempFtr)
			{
				pNew = pTempFtr->Clone();
			}
			else
			{
				pNew = pFeature->Clone();
				if (!pNew->CreateGeometry(CLS_GEOSURFACEPOINT)) continue;
			}
			
			if (!pNew) continue;
			
			pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
			
			if (!m_pEditor->AddObject(pNew, out_layer ? out_layer->GetID() : pLayer->GetID()) )
			{
				delete pNew;
				continue;
			}

			if (!out_layer)
			{
				GETXDS(m_pEditor)->CopyXAttributes(pFeature,pNew);
			}
			undo.AddNewFeature(FTR_HANDLE(pNew));
			undo.AddOldFeature(FtrToHandle(pFeature));		
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
		}
	}

	if(pTempFtr) delete pTempFtr;
	
	undo.Commit();
	
	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSurfacePointToPointCommand,CEditCommand)

CSurfacePointToPointCommand::CSurfacePointToPointCommand()
{
}

CSurfacePointToPointCommand::~CSurfacePointToPointCommand()
{
}

CString CSurfacePointToPointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SURFACEPOINTTOPOINT);
}

void CSurfacePointToPointCommand::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CSurfacePointToPointCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue(PF_HANDLELAYERS,&CVariantEx(var));
	
	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue(PF_LAYERNAME,&CVariantEx(var));
}

void CSurfacePointToPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SurfacePointToPoint",Name());
	
	param->AddLayerNameParamEx(PF_HANDLELAYERS,(LPCTSTR)m_handleLayers,StrFromResID(IDS_SRC_LAYER),NULL,LAYERPARAMITEM_POINT);
	
	param->AddLayerNameParam(PF_LAYERNAME,(LPCTSTR)m_targetLayer,StrFromResID(IDS_DES_LAYER),NULL,LAYERPARAMITEM_POINT);
}

void CSurfacePointToPointCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_HANDLELAYERS,var) )
	{					
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,PF_LAYERNAME,var) )
	{					
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimRight();
		m_targetLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CSurfacePointToPointCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor)
		return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	CFtrLayer *out_layer = pDS->GetFtrLayer((LPCTSTR)m_targetLayer);
	if(out_layer==NULL && m_targetLayer.GetLength()>0 )
	{
		out_layer=new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_targetLayer);
		pDS->AddFtrLayer(out_layer);
	}

	if(out_layer)
		m_layerID = out_layer->GetID();
	else
		m_layerID = -1;
	
	CFtrLayerArray arrLayers;
	int i=0, j=0;
	pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers,arrLayers);

	CUndoFtrs undo(m_pEditor,Name());

	CFeature *pTempFtr = NULL;
	if(out_layer)
	{
		pTempFtr = out_layer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOPOINT);
		if(!pTempFtr) return;
	}
	
	for (i=0;i<arrLayers.GetSize();i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if(!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
			{
				continue;
			}
						
			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);
			
			CFeature *pNew =  pTempFtr ? pTempFtr->Clone() : pFeature->Clone();
			if (!pNew) continue;
			
			if (!pNew->CreateGeometry(CLS_GEOPOINT)) continue;
			
			pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
			
			if (!m_pEditor->AddObject(pNew,m_layerID==-1?pLayer->GetID():m_layerID))
			{
				delete pNew;
				continue;
			}
			
			if(m_layerID==-1)
			{
				GETXDS(m_pEditor)->CopyXAttributes(pFeature,pNew);
			}
			undo.AddNewFeature(FTR_HANDLE(pNew));
			undo.AddOldFeature(FtrToHandle(pFeature));		
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
		}
	}

	if(pTempFtr) delete pTempFtr;
	
	undo.Commit();
	
	CCommand::PtClick(pt, flag);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifySurfaceToCurveCommand,CEditCommand)

CModifySurfaceToCurveCommand::CModifySurfaceToCurveCommand()
{
}

CModifySurfaceToCurveCommand::~CModifySurfaceToCurveCommand()
{
}

CString CModifySurfaceToCurveCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SURFACETOCURVE);
}

void CModifySurfaceToCurveCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		
		return;
	}
	
	CEditCommand::Start();
}

void CModifySurfaceToCurveCommand::SurfaceToCurve(const FTR_HANDLE* handles,int num)
{
	CDlgDataSource *pDS = GETDS(m_pEditor);

	CUndoFtrs undo(m_pEditor,Name());
	
	for (int i=0;i<num;i++)
	{
		CGeometry *pGeo = HandleToFtr(handles[i])->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			continue;
		}

		BOOL bOK = FALSE;

		CArray<PT_3DEX,PT_3DEX> pts, pts1;
		pGeo->GetShape(pts);
		int nsize = pts.GetSize();
		for(int k=0; k<=nsize; k++)
		{
			if(k==nsize || (k!=0&&pts[k].pencode==penMove))
			{	
				CValueTable tab;
				tab.BeginAddValueItem();
				HandleToFtr(handles[i])->WriteTo(tab);
				tab.EndAddValueItem();
				
				tab.DelField(FIELDNAME_SHAPE);
				tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
				tab.DelField(FIELDNAME_FTRDELETED);
				tab.DelField(FIELDNAME_FTRID);
				tab.DelField(FIELDNAME_GEOCLASS);
				tab.DelField(FIELDNAME_FTRDISPLAYORDER);
				tab.DelField(FIELDNAME_FTRGROUPID);
				tab.DelField(FIELDNAME_SYMBOLNAME);
				
				CFeature *pNew = HandleToFtr(handles[i])->Clone();
				if (!pNew) continue;
				
				if (!pNew->CreateGeometry(CLS_GEOCURVE)) continue;
				
				pNew->ReadFrom(tab);
				
				pts1[0].pencode = penLine;
				if( !pNew->GetGeometry()->CreateShape(pts1.GetData(),pts1.GetSize()) )
				{
					delete pNew;
					continue;
				}
				
				if (!m_pEditor->AddObject(pNew,m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
				{
					delete pNew;
					continue;
				}
				
				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pNew);
				undo.AddNewFeature(FTR_HANDLE(pNew));
				pts1.RemoveAll();
				bOK = TRUE;
			}
			
			if(k==nsize) break;
			pts1.Add(pts[k]);
		}

		if(bOK)
		{
			undo.AddOldFeature(handles[i]);			
			m_pEditor->DeleteObject(handles[i]);
		}
	}
	
	undo.Commit();
}

void CModifySurfaceToCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		SurfaceToCurve(handles,num);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifyCurveToSurfaceCommand,CEditCommand)

CModifyCurveToSurfaceCommand::CModifyCurveToSurfaceCommand()
{
	m_bClosedCurveOnly = FALSE;
}

CModifyCurveToSurfaceCommand::~CModifyCurveToSurfaceCommand()
{
}

CString CModifyCurveToSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CURVETOSURFACE);
}

void CModifyCurveToSurfaceCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		
		return;
	}
	
	CEditCommand::Start();
}

void CModifyCurveToSurfaceCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (bool)(m_bClosedCurveOnly);
	tab.AddValue(PF_CURVETOSURFACE_CLOSEDONLY,&CVariantEx(var));
	
}

void CModifyCurveToSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CurveToSurface",StrFromLocalResID(IDS_CMDNAME_CURVETOSURFACE));
	
	param->AddParam(PF_CURVETOSURFACE_CLOSEDONLY,bool(m_bClosedCurveOnly),StrFromResID(IDS_CMDPLANE_CLOSEDCURVEONLY));
	
}


void CModifyCurveToSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;	
	if( tab.GetValue(0,PF_CURVETOSURFACE_CLOSEDONLY,var) )
	{
		m_bClosedCurveOnly = (bool)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CModifyCurveToSurfaceCommand::CurveToSurface(const FTR_HANDLE* handles,int num)
{
	CUndoFtrs undo(m_pEditor,Name());
	
	for (int i=0;i<num;i++)
	{
		CGeometry *pGeo = HandleToFtr(handles[i])->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			continue;
		}

		CGeoCurve *pDir = (CGeoCurve*)pGeo;

		CArray<PT_3DEX,PT_3DEX> pts;
		pDir->GetShape(pts);

		if (pts.GetSize() < 3) continue;

		BOOL bClosed = pDir->IsClosed();
		if (m_bClosedCurveOnly && !bClosed)
		{
			continue;
		}

		CValueTable tab;
		tab.BeginAddValueItem();
		HandleToFtr(handles[i])->WriteTo(tab);
		tab.EndAddValueItem();
		
		tab.DelField(FIELDNAME_SHAPE);
		tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
		tab.DelField(FIELDNAME_FTRDELETED);
		tab.DelField(FIELDNAME_FTRID);
		tab.DelField(FIELDNAME_GEOCLASS);
		tab.DelField(FIELDNAME_FTRDISPLAYORDER);
		tab.DelField(FIELDNAME_FTRGROUPID);

		CFeature *pNew = HandleToFtr(handles[i])->Clone();
		if (!pNew) continue;

		if (!pNew->CreateGeometry(CLS_GEOSURFACE)) continue;

		pNew->ReadFrom(tab);

// 		if (bClosed)
// 		{
// 			pts.RemoveAt(pts.GetSize()-1);
// 		}

		pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());

		if (!m_pEditor->AddObject(pNew,m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
		{
			delete pNew;
			continue;
		}

		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handles[i]);		
		m_pEditor->DeleteObject(handles[i]);
	}
	
	undo.Commit();
}

void CModifyCurveToSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		CurveToSurface(handles,num);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CResetBaseAttributionCommand,CEditCommand)

CResetBaseAttributionCommand::CResetBaseAttributionCommand()
{
	m_nResetFlg = 0;
}

CResetBaseAttributionCommand::~CResetBaseAttributionCommand()
{
}

CString CResetBaseAttributionCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_RESETATTRIBUTION);
}


void CResetBaseAttributionCommand::Abort()
{
	UpdateParams(TRUE);
	m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	m_pEditor->RefreshView();
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Abort();
}



void CResetBaseAttributionCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		
		return;
	}
	
	CEditCommand::Start();
}

void CResetBaseAttributionCommand::ResetBaseAttributions(const FTR_HANDLE* handles,int num)
{	
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return;

	CConfigLibManager *pConfi = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	CScheme *pScheme = pConfi->GetScheme(pDS->GetScale());
	if(!pScheme)return;

	CUndoBatchAction batchUndo(m_pEditor,Name());

	for (int i=0;i<num;i++)
	{
		CFeature *pFtr = HandleToFtr(handles[i]);
		if (!pFtr) continue;

		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo) continue;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) continue;

		CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(pLayer->GetName());
		if (!pSchemeLayer) continue;

		CUndoModifyProperties undo(m_pEditor,Name());
		
		undo.arrHandles.Add(handles[i]);		
		
		m_pEditor->DeleteObject(handles[i],FALSE);
		undo.oldVT.BeginAddValueItem();
		CPFeature(handles[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();

		CValueTable tab;
		if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			tab.BeginAddValueItem();
			tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(long)COLOUR_BYLAYER,IDS_FIELDNAME_COLOR);
			tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(const char*)"",IDS_FIELDNAME_GEOSYMBOLNAME);
			tab.EndAddValueItem();

			PDOC(m_pEditor)->GetTextParamsFromScheme(pLayer->GetName(),tab);
		}

		if (0 == m_nResetFlg) //0 缺省 1 方案配置
		{
			pSchemeLayer->GetDefaultValues(tab);
			tab.DelField(FIELDNAME_FTRDISPLAYORDER);
			tab.DelField(FIELDNAME_GEOTEXT_CONTENT);
			tab.DelField(FIELDNAME_FTRMODIFYTIME);
			tab.DelField(FIELDNAME_GEOCLASS);
			tab.DelField(FIELDNAME_CLSTYPE);
			tab.DelField(FIELDNAME_GEOPOINT_ANGLE);
			tab.DelField(FIELDNAME_GEOCURVE_WIDTH);
			tab.DelField(FIELDNAME_GEOCURVE_DHEIGHT);
			tab.DelField(FIELDNAME_SYMBOLNAME);
			pFtr->ReadFrom(tab);
		}
		else
		{
			pSchemeLayer->GetBasicAttributeDefaultValues(tab);
			tab.DelField(FIELDNAME_FTRDISPLAYORDER);
			tab.DelField(FIELDNAME_GEOTEXT_CONTENT);
			tab.DelField(FIELDNAME_FTRMODIFYTIME);
			tab.DelField(FIELDNAME_GEOCLASS);
			tab.DelField(FIELDNAME_CLSTYPE);
			tab.DelField(FIELDNAME_GEOPOINT_ANGLE);
			tab.DelField(FIELDNAME_GEOCURVE_WIDTH);
			tab.DelField(FIELDNAME_GEOCURVE_DHEIGHT);
			tab.DelField(FIELDNAME_SYMBOLNAME);
			pFtr->ReadFrom(tab);
			pFtr->GetGeometry()->SetColor(pSchemeLayer->GetColor());
		}

		m_pEditor->RestoreObject(handles[i]);		
		
		undo.newVT.BeginAddValueItem();
		CPFeature(handles[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
		batchUndo.AddAction(&undo);
		
	}
	
	batchUndo.Commit();
}


void CResetBaseAttributionCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_RESETFLG,var) )
	{
		m_nResetFlg = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}



void CResetBaseAttributionCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)m_nResetFlg;
	tab.AddValue(PF_RESETFLG,&(CVariantEx)(long)(var));
	
}


void CResetBaseAttributionCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CResetBaseAttributionCommand",StrFromResID(IDS_CMDNAME_RESETATTRIBUTION));
	param->BeginOptionParam(PF_RESETFLG,StrFromResID(IDS_RESETFLAGE));
	param->AddOption(StrFromResID(IDS_DEFAULTFEATURE),DefaultFeature,' ',m_nResetFlg==DefaultFeature);
	param->AddOption(StrFromResID(IDS_LAYERCFGSCHEME),LayerCfgScheme,' ',m_nResetFlg==LayerCfgScheme);		
	param->EndOptionParam();
	
}



void CResetBaseAttributionCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		ResetBaseAttributions(handles,num);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}

#include "DeleteCommonSideDlg.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDeleteCommonSideCommand,CEditCommand)

CDeleteCommonSideCommand::CDeleteCommonSideCommand()
{
	m_nStep = 0;
	m_lfLimit = 1;
	m_bKeepCommonSide = FALSE;
}

CDeleteCommonSideCommand::~CDeleteCommonSideCommand()
{
}

CString CDeleteCommonSideCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DELETECOMMONSIDE);
}

void CDeleteCommonSideCommand::Start()
{
	if( !m_pEditor )return;
	m_nStep = 0;
	m_lfLimit = 1;
	m_bKeepCommonSide = FALSE;
	
	CEditCommand::Start();

	m_pEditor->CloseSelector();
}




//line为线段 
template<class T>
bool GetIntersectPos(T *pData1, int npt, T *line, double *t1, double *t2)
{
	int i, ok = 0;
	double pos, tol1, tol2, x, y;
	
	pos = -1;
	for( i=0; i<npt-1; i++)
	{
		if( GraphAPI::GGetLineIntersectLineSeg(pData1[i].x,pData1[i].y,
			pData1[i+1].x,pData1[i+1].y,line[0].x,line[0].y,
			line[1].x,line[1].y,&x,&y,&tol1,&tol2) )
		{
			if (t1)
			{
				*t1 = i+tol1;
			}
			
			if (t2)
			{
				*t2 = tol2;
			}
			ok = 1;
		}
	}
	
	return ok;
}

void CDeleteCommonSideCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		m_ptLine[0] = pt;	
		m_nStep = 1;
	}
	else if (m_nStep == 1)
	{
		m_ptLine[1] = pt;
		m_nStep = 2;
	}
	
	if( m_nStep==2 )
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
        PT_3D Pt[2];	
		(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(m_ptLine,Pt);
		(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(m_ptLine+1,Pt+1);
		
		Envelope el;
		el.CreateFromPts(Pt,2);
		m_pEditor->GetDataQuery()->FindObjectInRect(el,(m_pEditor->GetCoordWnd()).m_pSearchCS);
		int num1 ;
		const CPFeature *ftr = (m_pEditor->GetDataQuery())->GetFoundHandles(num1);

		if(num1!=2) return;
		if(!ftr[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) || !ftr[1]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		{
			Abort();
			return;
		}

		//指定删除层，如果层相同略过， 层不同弹出对话框选择
		int delete_part = 0;
		CFtrLayer* pLayer1 = m_pEditor->GetDataSource()->GetFtrLayerOfObject(ftr[0]);
		CFtrLayer* pLayer2 = m_pEditor->GetDataSource()->GetFtrLayerOfObject(ftr[1]);
		if(!pLayer1 || !pLayer2)
		{
			Abort();
			return;
		}
		if(pLayer1!=pLayer2)
		{
			CDeleteCommonSideDlg dlg;
			dlg.m_layname1 = pLayer1->GetName();
			dlg.m_layname2 = pLayer2->GetName();
			if(dlg.DoModal())
			{
				delete_part = dlg.m_side;
			}
		}

		if(delete_part!=0 && delete_part!=1)
		{
			Abort();
			return;
		}

		CFeature* pFtr = ftr[delete_part];
		int layid = m_pEditor->GetDataSource()->GetFtrLayerOfObject(pFtr)->GetID();
		IntersectItemArray arr1, arr2;
		GetIntersects(ftr[0], ftr[1], arr1);
		int i=0;
		for(i=0; i<arr1.GetSize(); i++)
		{
			//剔除不同地物
			if(pFtr != arr1[i].pFtr)
				continue;

			//剔除重复交点,并从小到大排序
			BOOL bSame = FALSE;//是否有重复
			int k=0;
			for(k=0; k<arr2.GetSize(); k++)
			{
				double postion1 = arr1[i].ptIdx + arr1[i].t;
				double postion2 = arr2[k].ptIdx + arr2[k].t;
				if(postion1<postion2)
				{
					break;
				}
				if( fabs(postion1-postion2)<1e-5 )
				{
					bSame = TRUE;
					break;
				}
			}
			if(bSame) continue;
			if(k==arr2.GetSize())
			{
				arr2.Add(arr1[i]);
			}
			else if(k<arr2.GetSize())
			{
				arr2.InsertAt(k, arr1[i]);
			}
		}

		if(arr2.GetSize()<=1)
		{
			Abort();
			return;
		}

		CArray<PT_3DEX, PT_3DEX> pts;
		pFtr->GetGeometry()->GetShape(pts);
		PT_3DEX line[2];
		COPY_3DPT(line[0],m_ptLine[0]);
		COPY_3DPT(line[1],m_ptLine[1]);
		double t;
		GetIntersectPos(pts.GetData(), pts.GetSize(), line, &t, NULL);

		int j=0, pos1=0, pos2=0;// pos1  pos2记录公共部分的起始/终点ptIdx
		double t1=0, t2=0;// t1 t2记录公共部分的起始/终点交点位置
		PT_3DEX pt1,pt2;//pt1  pt2记录公共部分的起始/终点交点
LP:		
		//公共部分的起点
		pos1 = arr2[j].ptIdx;
		t1 = arr2[j].t;
		COPY_3DPT(pt1, arr2[j].pt);
		//从下一个点开始找公共部分的末点
		j++;
		for(; j<arr2.GetSize(); j++)
		{
			if(arr2[j].ptIdx == arr2[j-1].ptIdx)//同一段
			{
				continue;
			}
			if( arr2[j].ptIdx==arr2[j-1].ptIdx+1 && fabs(arr2[j].t)<1e-5)//下一段的起点,也可以算作同一段
			{
				continue;
			}
			break;
		}

		t2 = arr2[j-1].t;
		pos2 = arr2[j-1].ptIdx;
		COPY_3DPT(pt2, arr2[j-1].pt);

		if(pos1+t1>t || pos2+t2<t)
		{
			if(j>=arr2.GetSize())//没有公共部分
			{
				Abort();
				return;
			}
			goto LP;
		}

		CArray<PT_3DEX, PT_3DEX> arrPts1, arrPts2;//可能被分成两部分

		for(i=0; i<=pos1; i++)
		{
			arrPts1.Add(pts[i]);	
		}
		if(t1>0)//添加一个点
		{
			pt1.pencode = penLine;
			arrPts1.Add(pt1);
		}

		for(i=pos2; i<pts.GetSize(); i++)
		{
			arrPts2.Add(pts[i]);
		}
		if(t2>0)
		{
			COPY_3DPT(arrPts2[0], pt2);
		}

		int ptnum1 = arrPts1.GetSize();
		int ptnum2 = arrPts2.GetSize();

		CUndoFtrs undo(m_pEditor,Name());
		if(ptnum1>1 && ptnum2>1 && GraphAPI::GIsEqual3DPoint(&arrPts1[0], &arrPts2[ptnum2-1]))
		{
			arrPts2.RemoveAt(ptnum2-1);
			arrPts2.Append(arrPts1);
			CFeature *pNew = pFtr->Clone();
			pNew->SetID(OUID());
			pNew->GetGeometry()->CreateShape(arrPts2.GetData(),arrPts2.GetSize());
			m_pEditor->AddObject(pNew,layid);
			
			GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNew);
			undo.arrNewHandles.Add(FtrToHandle(pNew));
		}
		else if(ptnum1<2 && ptnum2<2)
		{
			Abort();
			return;
		}
		else
		{
			if(ptnum1>1)
			{
				CFeature *pNew = pFtr->Clone();
				pNew->SetID(OUID());
				pNew->GetGeometry()->CreateShape(arrPts1.GetData(),arrPts1.GetSize());
				m_pEditor->AddObject(pNew,layid);
				
				GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNew);
				undo.arrNewHandles.Add(FtrToHandle(pNew));
			}
			if(ptnum2>1)
			{
				CFeature *pNew = pFtr->Clone();
				pNew->SetID(OUID());
				pNew->GetGeometry()->CreateShape(arrPts2.GetData(),arrPts2.GetSize());
				m_pEditor->AddObject(pNew,layid);
				
				GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNew);
				undo.arrNewHandles.Add(FtrToHandle(pNew));
			}
		}		
		
 		m_pEditor->DeleteObject(FTR_HANDLE(pFtr));
 		undo.AddOldFeature(FTR_HANDLE(pFtr));
		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 3;
	}
	
	CCommand::PtClick(pt, flag);
}

static void _FixT(double& t, double dis)
{
	double toler = 1e-4/dis;
	if(fabs(t)<toler)t = 0.0;
	if(fabs(1-t)<toler)t = 1.0;	
}

static void _Swap(double& a, double& b)
{
	double t = a;
	a = b;
	b = t;
}

static bool GGetLineIntersectLineSeg_withOverlap(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, 
												 double *plfX, double *plfY,double *plfT0, double *plfT1,
												 bool &twoIntersect,
												 double *plfX2, double *plfY2,double *plfT02, double *plfT12)
{
	twoIntersect = false;

	double vector1x = lfX1-lfX0, vector1y = lfY1-lfY0;
	double vector2x = lfX3-lfX2, vector2y = lfY3-lfY2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<GraphAPI::g_lfPraTolerance && unitdelta>-GraphAPI::g_lfPraTolerance )
	{	
		//虽然平行，但并不共线，而是有间距
		if(GraphAPI::GGetDisofPtToLine(lfX0,lfY0,lfX1,lfY1,lfX2,lfY2)>GraphAPI::g_lfDisTolerance )
			return false;
	
		double vector3x = lfX2-lfX0, vector3y = lfY2-lfY0;
		//共线的情况
		double av1x = fabs(vector1x), av1y = fabs(vector1y);
		double av2x = fabs(vector2x), av2y = fabs(vector2y);
		double av3x = fabs(vector3x), av3y = fabs(vector3y);

		//两对点都是重叠点
		if( av1x<GraphAPI::g_lfDisTolerance && av1y<GraphAPI::g_lfDisTolerance && av2x<GraphAPI::g_lfDisTolerance && av2y<GraphAPI::g_lfDisTolerance )
		{
			if( av3x<GraphAPI::g_lfDisTolerance && av3y<GraphAPI::g_lfDisTolerance )
			{
				if(plfX)*plfX = lfX0;
				if(plfY)*plfY = lfY0;
				if(plfT0)*plfT0 = 0;
				if(plfT1)*plfT1 = 0;

				return true;
			}
			
			return false;

		}
		//第一对点都是重叠点
		else if( av1x<GraphAPI::g_lfDisTolerance && av1y<GraphAPI::g_lfDisTolerance )
		{
			double t1, t2;
			if( av2x>av2y )
				t2 = -vector3x/vector2x;
			else
				t2 = -vector3y/vector2y;

			if( t2<0 || t2>1 )
				return false;

			t1 = 0;

			if(plfX)*plfX = lfX0;
			if(plfY)*plfY = lfY0;
			if(plfT0)*plfT0 = t1;
			if(plfT1)*plfT1 = t2;

			return true;
		}
		//第二对点都是重叠点
		else if( av2x<GraphAPI::g_lfDisTolerance && av2y<GraphAPI::g_lfDisTolerance )
		{
			double t1, t2;
			if( av1x>av1y )
				t1 = vector3x/vector1x;
			else
				t1 = vector3y/vector1y;
			
			if( t1<0 || t1>1 )
				return false;
			
			t2 = 0;
			
			if(plfX)*plfX = lfX2;
			if(plfY)*plfY = lfY2;

			if(plfT0)*plfT0 = t1;
			if(plfT1)*plfT1 = t2;

			return true;
		}
		//一般情况
		else
		{
			twoIntersect = true;

			//将两线段的方向调整，保持一致
			bool swapflag = false;
			if( av2x>av2y )
			{
				if( vector1x*vector2x<0 )
					swapflag = true;
			}
			else
			{
				if( vector1y*vector2y<0 )
					swapflag = true;
			}

			if(swapflag)
			{
				_Swap(lfX2,lfX3);
				_Swap(lfY2,lfY3);
				vector2x = -vector2x;
				vector2y = -vector2y;				
			}
			
			//先计算线段的端点在另一线段上的比例点ta,tb
			double ta1, tb1, ta2, tb2;
			if( av2x>av2y )
			{
				ta1 = (lfX0-lfX2)/vector2x;
				tb1 = (lfX1-lfX2)/vector2x;
			}
			else
			{
				ta1 = (lfY0-lfY2)/vector2y;
				tb1 = (lfY1-lfY2)/vector2y;
			}

			if( av1x>av1y )
			{
				ta2 = (lfX2-lfX0)/vector1x;
				tb2 = (lfX3-lfX0)/vector1x;
			}
			else
			{
				ta2 = (lfY2-lfY0)/vector1y;
				tb2 = (lfY3-lfY0)/vector1y;
			}

			_FixT(ta1,max(av2x,av2y));
			_FixT(tb1,max(av2x,av2y));
			_FixT(ta2,max(av1x,av1y));
			_FixT(tb2,max(av1x,av1y));

			//由于两个线段的方向一致，必定有ta1<tb1,ta2<tb2
			//根据ta,tb计算两个交点的坐标和参数，由于两线段共线，所以两个交点实际上是靠内的两个端点
			//共6种可能性
			if( (ta1<0 && tb1<0) || (ta1>1 && tb1>1) )
			{
				//线段共线不相交
				return false;
			}
			else if(ta1<=0 && tb1>=0 && tb1<=1 )
			{
				if(plfX)*plfX = lfX2;
				if(plfY)*plfY = lfY2;
				if(plfT0)*plfT0 = ta2;
				if(plfT1)*plfT1 = 0;

				if(plfX2)*plfX2 = lfX1;
				if(plfY2)*plfY2 = lfY1;
				if(plfT02)*plfT02 = 1;
				if(plfT12)*plfT12 = tb1;
			}
			else if(ta1<=0 && tb1>=1 )
			{
				if(plfX)*plfX = lfX2;
				if(plfY)*plfY = lfY2;
				if(plfT0)*plfT0 = ta2;
				if(plfT1)*plfT1 = 0;

				if(plfX2)*plfX2 = lfX3;
				if(plfY2)*plfY2 = lfY3;
				if(plfT02)*plfT02 = tb2;
				if(plfT12)*plfT12 = 1;
			}
			else if(ta1>=0 && ta1<=1 && tb1>=0 && tb1<=1 )
			{
				if(plfX)*plfX = lfX0;
				if(plfY)*plfY = lfY0;
				if(plfT0)*plfT0 = 0;
				if(plfT1)*plfT1 = ta1;

				if(plfX2)*plfX2 = lfX1;
				if(plfY2)*plfY2 = lfY1;
				if(plfT02)*plfT02 = 1;
				if(plfT12)*plfT12 = tb1;
			}
			else if(ta1>=0 && ta1<=1 && tb1>=1 )
			{
				if(plfX)*plfX = lfX0;
				if(plfY)*plfY = lfY0;
				if(plfT0)*plfT0 = 0;
				if(plfT1)*plfT1 = ta1;

				if(plfX2)*plfX2 = lfX3;
				if(plfY2)*plfY2 = lfY3;
				if(plfT02)*plfT02 = tb2;
				if(plfT12)*plfT12 = 1;
			}

			//前后端点衔接的情况，就认为只有一个交点
			if( (ta1<0 && tb1==0) || (ta1==1 && tb1>1) )
			{
				twoIntersect = false;
			}

			if(swapflag)
			{
				if(plfT1)*plfT1 = 1.0-*plfT1;
				if(plfT12)*plfT12 = 1.0-*plfT12;			
			}

			return true;
		}
	}

	double t1 = ( (lfX2-lfX0)*vector2y-(lfY2-lfY0)*vector2x )/delta;
	double t2 = ( (lfX2-lfX0)*vector1y-(lfY2-lfY0)*vector1x )/delta;
	double xr = lfX0 + t1*vector1x, yr = lfY0 + t1*vector1y;

	bool bSnapEndPoint = true;
	if( fabs(xr-lfX0)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY0)<=GraphAPI::g_lfDisTolerance )
	{
		t1 = 0;
		xr = lfX0; yr = lfY0;
	}
	else if( fabs(xr-lfX1)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY1)<=GraphAPI::g_lfDisTolerance )
	{
		t1 = 1;
		xr = lfX1; yr = lfY1;
	}
	else bSnapEndPoint = false;

	if( fabs(xr-lfX2)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY2)<=GraphAPI::g_lfDisTolerance )
	{
		t2 = 0;
		xr = lfX2; yr = lfY2;
	}
	else if( fabs(xr-lfX3)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY3)<=GraphAPI::g_lfDisTolerance )
	{
		t2 = 1;
		xr = lfX3; yr = lfY3;
	}
	else bSnapEndPoint = false;

	if(!bSnapEndPoint)
	{
		if( t1<0 || t1>1 || t2<0 || t2>1 )
		{
			return false;
		}
	}

	if(plfX)*plfX = xr;
	if(plfY)*plfY = yr;

	if(plfT0)*plfT0 = t1;
	if(plfT1)*plfT1 = t2;

	return true;
}

void CDeleteCommonSideCommand::GetIntersects(CFeature* f1, CFeature* f2, IntersectItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	const CShapeLine *pShp1 = pGeo1->GetShape();
	const CShapeLine *pShp2 = pGeo2->GetShape();

	if( !pShp1 || !pShp2 )
		return;

	PT_3D pt1,pt2,line[2],ret;
	double mindis = -1;
	
	int nIdx1=-1,nIdx2=-1;
	int nBaseIdx1 = 0, nBaseIdx2 = 0;
	const CShapeLine::ShapeLineUnit *pList1 = pShp1->HeadUnit();
	while (pList1!=NULL)
	{
		nIdx1++;

		Envelope e = pList1->evlp;
		e.m_xl -= 1e-6; e.m_xh += 1e-6;
		e.m_yl -= 1e-6; e.m_yh += 1e-6;

		const CShapeLine::ShapeLineUnit *pList2 = pShp2->HeadUnit();
		nIdx2 = -1;
		while(pList2!=NULL)
		{
			nIdx2++;
			if(e.bIntersect(&(pList2->evlp)))
			{
				for (int i=0;i<pList1->nuse-1;i++)
				{
					if( pList1->pts[i+1].pencode==penMove )
						continue;

					COPY_3DPT(line[0],pList1->pts[i]);
					COPY_3DPT(line[1],pList1->pts[i+1]);
					
					Envelope e0 = pList2->evlp;
					e0.m_xl -= 1e-4; e0.m_xh += 1e-4;
					e0.m_yl -= 1e-4; e0.m_yh += 1e-4;
					if(!e0.bIntersect(line,line+1))continue;

					for (int j=0;j<pList2->nuse-1;j++)
					{
						if( pList2->pts[j+1].pencode==penMove )
							continue;

						//为了优化，先判断一下
						double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
						
						if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
						else { xmin1 = line[1].x; xmax1 = line[0].x; }
						if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
						else { ymin1 = line[1].y; ymax1 = line[0].y; }
						
						if( pList2->pts[j].x<pList2->pts[j+1].x ){ xmin2 = pList2->pts[j].x; xmax2 = pList2->pts[j+1].x; }
						else { xmin2 = pList2->pts[j+1].x; xmax2 = pList2->pts[j].x; }
						if( pList2->pts[j].y<pList2->pts[j+1].y ){ ymin2 = pList2->pts[j].y; ymax2 = pList2->pts[j+1].y; }
						else { ymin2 = pList2->pts[j+1].y; ymax2 = pList2->pts[j].y; }
						
						if( xmax1<xmin2-1e-4 || xmax2<xmin1-1e-4 || 
							ymax1<ymin2-1e-4 || ymax2<ymin1-1e-4 )
							continue;

						double xt,yt,xt2,yt2;
						double t0=0,t1=0,t2=0,t3=0;
						bool twoIntersect = false;
						if(GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
							&xt,&yt,&t0,&t1,
							twoIntersect,
							&xt2,&yt2,&t2,&t3))
						{	
							if(t0<-0.01 || t1<-0.01 || t2<-0.01||t3<-0.01)
							{
								//不应该得到这样的结果
								GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
									&xt,&yt,&t0,&t1,
									twoIntersect,
									&xt2,&yt2,&t2,&t3);
								int a=1;	
							}
							IntersectItem item0,item1;
							item0.pFtr = f1;
							item0.pt.x = xt;
							item0.pt.y = yt;
							item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
							item0.t = t0;
							item0.ptIdx = nBaseIdx1 + i;

							if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
							{
								item0.t = 0;
							}

							if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
							{
								item0.ptIdx++;
								item0.t = 0;
							}

							item1.pFtr = f2;
							item1.pt.x = xt;
							item1.pt.y = yt;
							item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
							item1.t = t1;
							item1.ptIdx = nBaseIdx2 + j;

							if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
							{
								item1.t = 0;
							}

							if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
							{
								item1.ptIdx++;
								item1.t = 0;
							}

							arr.Add(item0);
							arr.Add(item1);

							if(twoIntersect)
							{
								item0 = IntersectItem();
								item1 = IntersectItem();
								xt = xt2; yt = yt2;
								t0 = t2; t1 = t3;
								
								item0.pFtr = f1;
								item0.pt.x = xt;
								item0.pt.y = yt;
								item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
								item0.t = t0;
								item0.ptIdx = nBaseIdx1 + i;

								if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
								{
									item0.t = 0;
								}

								if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
								{
									item0.ptIdx++;
									item0.t = 0;
								}

								item1.pFtr = f2;
								item1.pt.x = xt;
								item1.pt.y = yt;
								item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
								item1.t = t1;
								item1.ptIdx = nBaseIdx2 + j;

								if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
								{
									item1.t = 0;
								}

								if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
								{
									item1.ptIdx++;
									item1.t = 0;
								}

								arr.Add(item0);
								arr.Add(item1);
							}
						}
					}
				}				
			}
			nBaseIdx2 += pList2->nuse;
			pList2 = pList2->next;
		}		

		nBaseIdx1 += pList1->nuse;
		pList1 = pList1->next;
	}
}

void CDeleteCommonSideCommand::PtMove(PT_3D &pt)
{
	if (m_nStep == 1)
	{
		GrBuffer buf;
		buf.BeginLineString(255,0,0);
		buf.MoveTo(&m_ptLine[0]);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
	
	CEditCommand::PtMove(pt);
}

/////////////////////////////////////////////////////////////////////
// CModifyZCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CModifyZCommand,CEditCommand)

CModifyZCommand::CModifyZCommand()
{
	m_nStep = -1;
	m_bOnlyContour = TRUE;
	m_lfResultZ = 0;
	m_bStartSnapHeight = FALSE;
	strcat(m_strRegPath,"\\ModifyZ");
}

CModifyZCommand::~CModifyZCommand()
{
}

void CModifyZCommand::Abort()
{
	CEditCommand::Abort();
}


CString CModifyZCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MODIFYZ);
}

void CModifyZCommand::Start()
{
	if( !m_pEditor )return;
	m_bOnlyContour = TRUE;
	m_lfResultZ = 0;	
	CEditCommand::Start();
}

BOOL CModifyZCommand::CheckObjForContour(CGeometry *pObj)
{
	if( !pObj )return FALSE;		
	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);
	int num = pts.GetSize();
	if (num<=0)
	{
		return FALSE;
	}

	PT_3DEX pt0,pt1;
	pt0 = pts.GetAt(0);
	for( int j=1; j<num; j++)
	{
		pt1 = pts.GetAt(j);
		if( fabs(pt1.z-pt0.z)>=GraphAPI::g_lfZTolerance )break;
	}
	
	if( j<num )return FALSE;
	return TRUE;
}

void CModifyZCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_bStartSnapHeight )
	{
		CSelection * pSel = m_pEditor->GetSelection();
		int num = 0;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		if( num<=0 )
			return;
		
		double z0 = 0;
		CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
		if( CModifyZCommand::CheckObjForContour(pObj) )
		{
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pObj->GetShape(arrPts);
			
			z0 = arrPts[0].z;
		}
		else
		{
			Envelope evlp;
			evlp.CreateFromPtAndRadius(pt,1e+100);
			PT_3D retPt;
			pObj->FindNearestBasePt(pt,evlp,NULL,&retPt,NULL);
			z0 = retPt.z;
		}
		
		m_lfResultZ = z0;
		SetSettingsModifyFlag();
		
		CUIParam param;
		FillShowParams(&param);
		m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, (LONG_PTR)&param);
		
		m_bStartSnapHeight = FALSE;
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		
		return;
	}

	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CGeometry *pObj;		
	//	CGeometry *pObj1, *pObj2;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		//CUndoFtrs undo(m_pEditor,Name());
		CUndoModifyProperties undo(m_pEditor,Name());
		GrBuffer buf;
		PT_3DEX pt0,pt1;
			
		for( int i = num-1; i>=0; i--)
		{
			pObj = HandleToFtr(handles[i])->GetGeometry();
			
			//生成新的对象，这里需要指定新对象层码
			if( !pObj || pObj->GetDataPointSum()<=0 )continue;

			if( m_bOnlyContour && !CheckObjForContour(pObj) )
				continue;

			undo.arrHandles.Add(handles[i]);
			undo.oldVT.BeginAddValueItem();
			HandleToFtr(handles[i])->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
			
// 			pObj = pObj->Clone();
// 			if( !pObj )continue;

			CArray<PT_3DEX,PT_3DEX> arr;
			pObj->GetShape(arr);
			int num0 = arr.GetSize();
			
			for( int j=0; j<num0; j++)
			{
				PT_3DEX &pt1 = arr.ElementAt(j);
				pt1.z = m_lfResultZ;
			}

			m_pEditor->DeleteObject(handles[i],FALSE);

			pObj->CreateShape(arr.GetData(),arr.GetSize());						
			
			if( !m_pEditor->RestoreObject(handles[i]) )
			{				
				Abort();
				return;
			}

			undo.newVT.BeginAddValueItem();
			HandleToFtr(handles[i])->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();

		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt,flag);
}


void CModifyZCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bOnlyContour);
	tab.AddValue(PF_MODIFYZONLYCONTOR,&CVariantEx(var));
	var = (double)(m_lfResultZ);
	tab.AddValue(PF_MODIFYZHEIGHT,&CVariantEx(var));	
}

void CModifyZCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ModifyZCommand",StrFromLocalResID(IDS_CMDNAME_MODIFYZ));	
	param->AddParam(PF_MODIFYZONLYCONTOR,bool(m_bOnlyContour),StrFromResID(IDS_CMDPLANE_ONLYCONTOUR));
	param->AddParam(PF_MODIFYZHEIGHT,double(m_lfResultZ),StrFromResID(IDS_CMDPLANE_Z));
	param->AddButtonParam("SnapHeight",StrFromResID(IDS_SNAPHEIGHT));
}

void CModifyZCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_MODIFYZONLYCONTOR,var) )
	{
		m_bOnlyContour = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_MODIFYZHEIGHT,var) )
	{
		m_lfResultZ = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	if( tab.GetValue(0,"SnapHeight",var) )
	{
		m_bStartSnapHeight = TRUE;
		m_pEditor->OpenSelector();
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CModifyColorCommand Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CModifyColorCommand,CEditCommand)

CModifyColorCommand::CModifyColorCommand()
{
   m_nStep = -1;
}

CModifyColorCommand::~CModifyColorCommand()
{

}

CString CModifyColorCommand::Name()
{
     return StrFromResID(IDS_CMDNAME_MODIFYCOLOR);
}

void CModifyColorCommand::Start()
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
		return;
	}
	CEditCommand::Start();
}

void CModifyColorCommand::Abort()
{
	CEditCommand::Abort();
}

void CModifyColorCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	CEditCommand::Finish();
}

void CModifyColorCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ModifyColorCommand",StrFromLocalResID(IDS_CMDNAME_MODIFYCOLOR));
	param->AddColorParam("ChooseColor",(long)m_nColor,StrFromResID(IDS_CHOOSECOLOR));
}

void CModifyColorCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
		
	if( tab.GetValue(0,"ChooseColor",var) )
	{
		m_nColor = (long)(_variant_t)*var;
		m_pEditor->OpenSelector();
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CModifyColorCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	
	var = (long)m_nColor;
	tab.AddValue("ChooseColor",&CVariantEx(var));
}


void CModifyColorCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	if( m_nStep==1 )
	{
		CSelection * pSel = m_pEditor->GetSelection();
		int num = 0;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		if( num<=0 )
			return;


		CUndoModifyProperties undo(m_pEditor,Name());
		GetActiveDlgDoc()->BeginBatchUpdate();
		for(int i=0; i<num; ++i)
		{
		  CPFeature pftr = HandleToFtr(handles[i]);
		  if(pftr==NULL || pftr->GetGeometry()==NULL)
			  continue;
		  //
		  undo.arrHandles.Add(handles[i]);
		  undo.oldVT.BeginAddValueItem();
		  HandleToFtr(handles[i])->WriteTo(undo.oldVT);
		  undo.oldVT.EndAddValueItem();

		  m_pEditor->DeleteObject(handles[i],FALSE);
		  
		  pftr->GetGeometry()->SetColor(m_nColor);
		  if( !m_pEditor->RestoreObject(handles[i]) )
		  {				
			  continue;
		  }
	  
		  undo.newVT.BeginAddValueItem();
		  HandleToFtr(handles[i])->WriteTo(undo.newVT);
		  undo.newVT.EndAddValueItem();
		}
		GetActiveDlgDoc()->EndBatchUpdate();
		undo.Commit();
	}

	Finish();
	CEditCommand::PtClick(pt,flag);

	
}

//////////////////////////////////////////////////////////////////////
// CModifyIntersectionCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CModifyIntersectionCommand,CEditCommand)

CModifyIntersectionCommand::CModifyIntersectionCommand()
{
	m_nStep = -1;

}

CModifyIntersectionCommand::~CModifyIntersectionCommand()
{

}



CString CModifyIntersectionCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MODIFYINTERSECTION);
}

void CModifyIntersectionCommand::Start()
{
 	if( !m_pEditor )return;
	m_nStep=0;
	CEditCommand::Start();
	m_pEditor->CloseSelector();
}

void CModifyIntersectionCommand::Abort()
{	
	CEditCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}
 
bool CModifyIntersectionCommand::IsInRange(PT_3DEX *pts, int nums, PT_3D pt1, PT_3D pt2)
{
	for (int i=0;i<nums;i++)
	{
		if ((pts+i)->x>=min(pt1.x,pt2.x)&&
			(pts+i)->x<=max(pt1.x,pt2.x)&&
			(pts+i)->y>=min(pt1.y,pt2.y)&&
			(pts+i)->y<=max(pt1.y,pt2.y))	
		{
			continue;
			
		}
		else
			return false;
	}
	return true;
	
}
 
void CModifyIntersectionCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	if( m_nStep==0 )
	{
		m_start = pt;		
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 1;
		return;		
	}
	
	if( m_nStep==1 )
	{
        CArray<CGeometry*,CGeometry*&> addObject;
		CArray<FTR_HANDLE,FTR_HANDLE> arrHandles;
//		CArray<int,int> objLayerHandle;
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		m_end = pt;
		
		PT_4D Pt[2];
		COPY_3DPT(Pt[0],m_start);
		COPY_3DPT(Pt[1], m_end);
		
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(Pt,2);
		Envelope el;
		el.CreateFromPts(Pt,2,sizeof(PT_4D));
		m_pEditor->DeselectAll();
		m_pEditor->GetDataQuery()->FindObjectInRect(el,m_pEditor->GetCoordWnd().m_pSearchCS);		
		int num1 ;
		const CPFeature *ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
		if (num1>10)
		{
			Abort();
			return;
		}

		CArray<CPFeature,CPFeature> arrFtrs;
		for (int i=0; i<num1; i++)
		{
			arrFtrs.Add(ftr[i]);
		}
		
		CGeometry *pObj=NULL;
		CGeometry *pObj1=NULL;

		// 将双线地物打散
		CArray<CPFeature,CPFeature> arrSepFtrs;
		for (i=0; i<arrFtrs.GetSize(); i++)
		{
			pObj = ftr[i]->GetGeometry();			
			if( !pObj )continue;
			
			CGeometry *pCur1 = NULL, *pCur2 = NULL;
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			{
				((CGeoParallel*)pObj)->Separate(pCur1,pCur2);
			}
			else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
				((CGeoDCurve*)pObj)->Separate(pCur1,pCur2);
			}

			if (pCur1)
			{
				CPFeature pNewFtr = ftr[i]->Clone();
				if (pNewFtr)
				{
					pNewFtr->SetGeometry(pCur1);
					arrSepFtrs.Add(pNewFtr);
 					if( !m_pEditor->AddObject(pNewFtr,m_pEditor->GetFtrLayerIDOfFtr(FTR_HANDLE(ftr[i]))))
 					{
 						delete pNewFtr;
 						Abort();
						continue;
					}
				}
			}

			if (pCur2)
			{
				CPFeature pNewFtr = ftr[i]->Clone();
				if (pNewFtr)
				{
					pNewFtr->SetGeometry(pCur2);
					arrSepFtrs.Add(pNewFtr);
 					if( !m_pEditor->AddObject(pNewFtr,m_pEditor->GetFtrLayerIDOfFtr(FTR_HANDLE(ftr[i]))))
 					{
 						delete pNewFtr;
 						Abort();
 						continue;
 					}
				}
			}
		}

		arrFtrs.Append(arrSepFtrs);
		
		int ftrNum = arrFtrs.GetSize();
		for( i=0; i<ftrNum; i++)//遍历每一个线状地物
		{			
			int PtNums1;
			CArray<PT_3DEX,PT_3DEX> pts1;
			pObj = arrFtrs[i]->GetGeometry();

			if( !pObj )continue;
			
			else if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) 
				|| pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))continue;
			else if((PtNums1=pObj->GetDataPointSum())<2 )continue;
			else
			{
				const CShapeLine *pSL = pObj->GetShape();
				if( !pSL )continue;				
				pSL->GetPts(pts1);
// 				pSL->GetPtsCount()
// 				for (int i=0;i<pts1.GetSize();i++)
// 				{
// 					COPY_3DPT()
// 				}
			}
			//PtNums = pts1.GetSize();
			PtNums1 = GraphAPI::GKickoffSamePoints(pts1.GetData(),pts1.GetSize());
			pts1.SetSize(PtNums1);
			pts1.FreeExtra();
			PT_3D startPt;
			COPY_3DPT(startPt,pts1[0]);
			PT_3D endPt;
			COPY_3DPT(endPt,pts1[0]);
			int start=1;
			int end=0;
			for (int k=0;k<PtNums1-1;k++)//对每条线段处理
			{
				
				PT_3DEX ret,minret;
				PT_3DEX start1;
				double t,mint=1;
				start1=pts1[k];				
				int count=0;
				do{
				
// 						if (!(max(pts1[k].x,pts1[k+1].x)<min(m_start.x,m_end.x)||
// 							min(pts1[k].x,pts1[k+1].x)>max(m_start.x,m_end.x)||
// 							max(pts1[k].y,pts1[k+1].y)<min(m_start.y,m_end.y)||
// 							min(pts1[k].y,pts1[k+1].y)>max(m_start.y,m_end.y)))
						{					
							mint=1;
							for (int j=0;j<ftrNum;j++)//遍历其余地物
							{
								
								if (j!=i)
								{									
									int PtNums2;
									pObj1 = arrFtrs[j]->GetGeometry();
									CArray<PT_3DEX,PT_3DEX> pts2;
									if( !pObj1 )continue;
									else if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj1->IsKindOf(RUNTIME_CLASS(CGeoParallel)) 
											|| pObj1->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))continue;
									else if((PtNums2=pObj1->GetDataPointSum())<2 )continue;
									else
									{
										const CShapeLine *pSL1 = pObj1->GetShape();
										if( !pSL1 )continue;
										pSL1->GetPts(pts2);										
									}
									PtNums2 = GraphAPI::GKickoffSamePoints(pts2.GetData(),pts2.GetSize());
									pts2.SetSize(PtNums2);
									pts2.FreeExtra();
									
									if ( 
										GraphAPI::GGetFirstPointOfIntersect(pts2.GetData(),PtNums2,start1,pts1[k+1],&ret,&t)&&
										GraphAPI::GIsPtInRange(m_start,m_end,PT_3D(ret.x,ret.y,ret.z)))//进不去
									{
										count++;
										if (t<=mint)
										{
											mint=t;
											minret=ret;
										}
									}							
									
								}
							}
						}
						if(count==0&&k!=PtNums1-2)
							break;
						if(count==0&&k==PtNums1-2)
						{
							endPt=pts1[PtNums1-1];
							end=PtNums1-2;
							if(!IsInRange(&(pts1[start]),end-start+1,m_start,m_end)||!GraphAPI::GIsPtInRange(m_start,m_end,startPt)||!GraphAPI::GIsPtInRange(m_start,m_end,endPt))
							{
								if (GraphAPI::GIsEqual3DPoint(&startPt,&endPt))
								{
									addObject.Add(pObj);
									arrHandles.Add(FtrToHandle(arrFtrs[i]));
								//	objLayerHandle.Add(ids1[i].layhdl);
								}
								else if (GraphAPI::GIsEqual3DPoint(&pts1[0],&PT_3DEX(endPt,0)))
								{
									CGeometry *objs[3]={0};							
									((CGeoCurve*)pObj)->GetBreak(startPt,endPt,objs[0],objs[1],objs[2]);
									if( objs[0] )
									{
										delete objs[0];
										objs[0] = NULL;
									}
									if( objs[1] )
									{
										delete objs[1];
										objs[1] = NULL;
									}
									
									if (objs[2])
									{
										addObject.Add(objs[2]);
										arrHandles.Add(FtrToHandle(arrFtrs[i]));

										CArray<PT_3DEX,PT_3DEX> arr0;
										objs[2]->GetShape(arr0);
										int size0 = arr0.GetSize();
										
										if (!GraphAPI::GIsEqual2DPoint(&arr0[0],&pts1[0]))
										{
											arr0[0].z = (startPt.z + arr0[0].z) / 2;
										}
										
										if (!GraphAPI::GIsEqual2DPoint(&arr0[size0-1],&pts1[PtNums1-1]))
										{
											arr0[size0-1].z = (endPt.z + arr0[size0-1].z) / 2;
										}
										
										objs[2]->CreateShape(arr0.GetData(),arr0.GetSize());
									//	objLayerHandle.Add(ids1[i].layhdl);								
									}		
									
								}
								else
								{							
									CGeometry *objs[3]={0};							
									((CGeoCurve*)pObj)->GetBreak(startPt,endPt,objs[0],objs[1],objs[2]);
									if( objs[0] )
									{
										delete objs[0];
										objs[0] = NULL;
									}
									if( objs[2] )
									{
										delete objs[2];
										objs[2] = NULL;
									}
									
									if (objs[1])
									{
										addObject.Add(objs[1]);
										arrHandles.Add(FtrToHandle(arrFtrs[i]));

										CArray<PT_3DEX,PT_3DEX> arr0;
										objs[1]->GetShape(arr0);
										int size0 = arr0.GetSize();
										
										if (!GraphAPI::GIsEqual2DPoint(&arr0[0],&pts1[0]))
										{
											arr0[0].z = (startPt.z + arr0[0].z) / 2;
										}
										
										if (!GraphAPI::GIsEqual2DPoint(&arr0[size0-1],&pts1[PtNums1-1]))
										{
											arr0[size0-1].z = (endPt.z + arr0[size0-1].z) / 2;
										}
										
										objs[1]->CreateShape(arr0.GetData(),arr0.GetSize());
								//		objLayerHandle.Add(ids1[i].layhdl);										
									
									}		
								 }
							}
							break;
						}
						if (count==1)
						{
							endPt=minret;
							end=k;
							if(!IsInRange(&(pts1[start]),end-start+1,m_start,m_end)||!GraphAPI::GIsPtInRange(m_start,m_end,startPt)||!GraphAPI::GIsPtInRange(m_start,m_end,endPt))
							{
								CGeometry *objs[3]={0};							
								((CGeoCurve*)pObj)->GetBreak(startPt,endPt,objs[0],objs[1],objs[2]);
								if( objs[0] )
								{
									delete objs[0];
									objs[0] = NULL;
								}
								if( objs[2] )
								{
									delete objs[2];
									objs[2] = NULL;
								}
								
								if (objs[1])
								{
									addObject.Add(objs[1]);
									arrHandles.Add(FtrToHandle(arrFtrs[i]));

									CArray<PT_3DEX,PT_3DEX> arr0;
									objs[1]->GetShape(arr0);
									int size0 = arr0.GetSize();
									
									if (!GraphAPI::GIsEqual2DPoint(&arr0[0],&pts1[0]))
									{
										arr0[0].z = (startPt.z + arr0[0].z) / 2;
									}
									
									if (!GraphAPI::GIsEqual2DPoint(&arr0[size0-1],&pts1[PtNums1-1]))
									{
										arr0[size0-1].z = (endPt.z + arr0[size0-1].z) / 2;
									}
									
									objs[1]->CreateShape(arr0.GetData(),arr0.GetSize());
							//		objLayerHandle.Add(ids1[i].layhdl);							
								}									
								start = end+1;
								startPt = endPt;
								COPY_3DPT(start1,endPt);
								count = 0;
								continue;
							}					
							else
							{
								start = end+1;
								count = 0;
								startPt = endPt;
								COPY_3DPT(start1,endPt);
								continue;
							}
						}
						if (count>=2)
						{
							endPt=minret;
							end=k;
							if(!IsInRange(&(pts1[start]),end-start+1,m_start,m_end)||!GraphAPI::GIsPtInRange(m_start,m_end,startPt)||!GraphAPI::GIsPtInRange(m_start,m_end,endPt))
							{
								CGeometry *objs[3]={0};
								
								((CGeoCurve*)pObj)->GetBreak(startPt,endPt,objs[0],objs[1],objs[2]);//首尾相同时做例外处理
								if( objs[0] )
								{
									delete objs[0];
									objs[0] = NULL;
								}
								if( objs[2] )
								{
									delete objs[2];
									objs[2] = NULL;
								}
								
								if (objs[1])
								{
									addObject.Add(objs[1]);
									arrHandles.Add(FtrToHandle(arrFtrs[i]));

									CArray<PT_3DEX,PT_3DEX> arr0;
									objs[1]->GetShape(arr0);
									int size0 = arr0.GetSize();
									
									if (!GraphAPI::GIsEqual2DPoint(&arr0[0],&pts1[0]))
									{
										arr0[0].z = (startPt.z + arr0[0].z) / 2;
									}
									
									if (!GraphAPI::GIsEqual2DPoint(&arr0[size0-1],&pts1[PtNums1-1]))
									{
										arr0[size0-1].z = (endPt.z + arr0[size0-1].z) / 2;
									}
									
									objs[1]->CreateShape(arr0.GetData(),arr0.GetSize());
							//		objLayerHandle.Add(ids1[i].layhdl);								
								}									
								
								
							}
							start = end+1;
							startPt = endPt;
							count = 0;
							COPY_3DPT(start1,endPt);
							continue;
						}					
					}while(1);
					
			}		
		}
		CUndoFtrs undo(m_pEditor,Name());
		
		for (int l=0;l<addObject.GetSize();l++)  //添加地物
		{
			CFeature *pFtr = HandleToFtr(arrHandles[l])->Clone() ;
			if (pFtr)
			{
				pFtr->SetID(OUID());
				pFtr->SetGeometry(addObject[l]);
				int layid = m_pEditor->GetFtrLayerIDOfFtr(arrHandles[l]);
				if(!m_pEditor->AddObject(pFtr,layid))
				{
					delete pFtr;
					continue;
				}
				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(arrHandles[l]),pFtr);
				undo.arrNewHandles.Add(FtrToHandle(pFtr));	
			}

		//	 m_pDoc->AddObject(addObject.GetAt(l),objLayerHandle.GetAt(l) ));
			
		}

		for( int b=0; b<num1; b++)//遍历每一个线状地物,将不是全部在拉框内的删除
		{
			
			pObj = ftr[b]->GetGeometry();
			CArray<PT_3DEX,PT_3DEX> pts;
			if( !pObj )continue;
			else if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )continue;
			else if((pObj->GetDataPointSum())<2 )continue;
			else
			{
				const CShapeLine *pSL = pObj->GetShape();
				if( !pSL )continue;
				pSL->GetPts(pts);			
			}
			if(!IsInRange(pts.GetData(),pts.GetSize(),m_start,m_end))
			{
				m_pEditor->DeleteObject(FtrToHandle(ftr[b]));
				undo.arrOldHandles.Add(FtrToHandle(ftr[b]));
				//	m_idsOld.Add(ids1[b]);
			}
		}

		addObject.RemoveAll();

		for (i=0; i<arrSepFtrs.GetSize(); i++)
		{
			if (arrSepFtrs[i])
			{
 				m_pEditor->DeleteObject(FtrToHandle(arrSepFtrs[i]));	//删除打散后的双线
			}
		}
		arrSepFtrs.RemoveAll();	
		
		undo.Commit();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	CEditCommand::PtClick(pt,flag);
}

void CModifyIntersectionCommand::PtMove(PT_3D &pt)
{
	if (m_nStep==1)
	{
		
		GrBuffer buf;
		PT_3D tem[2];
		tem[0].x=m_start.x;
		tem[0].y=pt.y;
		tem[0].z=m_start.z;
		tem[1].x=pt.x;
		tem[1].y=m_start.y;
		tem[1].z=m_start.z;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_start);
		buf.LineTo(&tem[0]);
		buf.LineTo(&pt);
		buf.LineTo(&tem[1]);
		buf.LineTo(&m_start);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		return;
	}
}


//////////////////////////////////////////////////////////////////////
// CChamferCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CChamferCommand,CEditCommand)

CChamferCommand::CChamferCommand()
{
	m_nStep = -1;
	m_nMode = modeArcChamfer;
	m_bLineserial = FALSE;
	strcat(m_strRegPath,"\\Chamfer");
}

CChamferCommand::~CChamferCommand()
{
	
}



CString CChamferCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CHAMFER);
}

void CChamferCommand::Start()
{
 	if( !m_pEditor )return;
	
	m_nMode = modeArcChamfer;
	m_bLineserial = FALSE;
	m_pNewFtrs[0]  = NULL;
	m_pNewFtrs[1]  = NULL;
	
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);
	if( m_nMode==modeArcChamfer )
		;//OutputTipString(StrFromResID(IDS_CMDTIP_CLICKCHAMFER));
	else
		;//OutputTipString(StrFromResID(IDS_CMDTIP_CLICKCHAMFER));
}

void CChamferCommand::Abort()
{	
	m_pNewFtrs[0] = NULL;
	if(m_pNewFtrs[1]) 
	{
		delete m_pNewFtrs[1];
		m_pNewFtrs[1] = NULL;
	}
	CEditCommand::Abort();
	m_pEditor->RefreshView();
}
 
int CChamferCommand::GPtinRegion(LINE_2D l1,LINE_2D l2,PT_2D &pt,PT_2D *ret1,PT_2D *ret2,double *t1,double *t2,int mode)
{
	PT_2D Pt0;//交点
	PT_2D Pt1;//角平分线上的某点
	PT_2D Pt2;//过交点且垂直于角平分线的直线上的一点
	PT_2D Pt3,Pt4;//垂足
	PT_3D temp[3];
	PT_3D ptt;
	temp[0].x=l1.sp.x;
	temp[0].y=l1.sp.y;
	temp[2].x=l2.sp.x;
	temp[2].y=l2.sp.y;
	ptt.x=pt.x;
	ptt.y=pt.y;
	ptt.z=0;
	if(GraphAPI::GGetRay1IntersectRay2(l1.sp.x,l1.sp.y,l1.ep.x,l1.ep.y,l2.sp.x,l2.sp.y,l2.ep.x,l2.ep.y,&(Pt0.x),&(Pt0.y)))
	{
		temp[1].x=Pt0.x;
		temp[1].y=Pt0.y;
		if(!GraphAPI::GGetPtInAngleBisector2D(Pt0.x,Pt0.y,l1.sp.x,l1.sp.y,l2.sp.x,l2.sp.y,&(Pt1.x),&(Pt1.y)))return -1;
		GraphAPI::GGetPerpendicular(Pt0.x,Pt0.y,Pt1.x,Pt1.y,l1.sp.x,l1.sp.y,&(Pt3.x),&(Pt3.y),NULL);
		GraphAPI::GGetPerpendicular(Pt0.x,Pt0.y,Pt1.x,Pt1.y,l2.sp.x,l2.sp.y,&(Pt4.x),&(Pt4.y),NULL);
		Pt2.x = -(Pt1.y-Pt0.y)+Pt0.x;
		Pt2.y = Pt1.x-Pt0.x+Pt0.y;		
	}	
	//pt点在垂直于角平分线，垂足为两线段交点的直线的异于l1起点的一侧
	if (GraphAPI::GGetMultiply(Pt0,Pt2,pt)*GraphAPI::GGetMultiply(Pt0,Pt2,l1.sp)<0)
	{
		return 1;
	}
	//pt在另一侧的情况
	else if (GraphAPI::GGetMultiply(l1.sp,Pt3,pt)*GraphAPI::GGetMultiply(l1.sp,Pt3,Pt0)<0||GraphAPI::GGetMultiply(l2.sp,Pt4,pt)*GraphAPI::GGetMultiply(l2.sp,Pt4,Pt0)<0)
	{
		return 2;
	}
	else if (GraphAPI::GIsPtInRegion(ptt,temp,3)==2)
	{
		PT_2D tem;
		if (mode==1)
		{
			GraphAPI::GGetPerpendicular(Pt0.x,Pt0.y,Pt1.x,Pt1.y,pt.x,pt.y,&(tem.x),&(tem.y),NULL);
			GraphAPI::GGetLineIntersectLine(l1.sp.x,l1.sp.y,l1.ep.x-l1.sp.x,l1.ep.y-l1.sp.y,pt.x,pt.y,tem.x-pt.x,tem.y-pt.y,&(ret1->x),&(ret1->y),t1);
			GraphAPI::GGetLineIntersectLine(l2.sp.x,l2.sp.y,l2.ep.x-l2.sp.x,l2.ep.y-l2.sp.y,pt.x,pt.y,tem.x-pt.x,tem.y-pt.y,&(ret2->x),&(ret2->y),t2);
		}
		if(mode==0)
		{
			double da=l1.ep.x-l1.sp.x;
			double db=l1.ep.y-l1.sp.y;
			double dx=Pt1.x-Pt0.x;
			double dy=Pt1.y-Pt0.y;
			double a=(da*da+db*db)*(dx*dx+dy*dy)-(da*dy-db*dx)*(da*dy-db*dx);
			double b=2*((da*da+db*db)*((Pt0.x-pt.x)*dx+(Pt0.y-pt.y)*dy)-(da*dy-db*dx)*(da*(Pt0.y-l1.sp.y)-db*(Pt0.x-l1.sp.x)));
			double c=(da*da+db*db)*((Pt0.x-pt.x)*(Pt0.x-pt.x)+(Pt0.y-pt.y)*(Pt0.y-pt.y))-(da*(Pt0.y-l1.sp.y)-db*(Pt0.x-l1.sp.x))*(da*(Pt0.y-l1.sp.y)-db*(Pt0.x-l1.sp.x));
			double x1,x2;
			x1=(-b+sqrt(b*b-4*a*c))/(2*a);
			x2=(-b-sqrt(b*b-4*a*c))/(2*a);
			x1=x1>x2?x1:x2;
			double  xcenter = x1*dx+Pt0.x;
			double  ycenter = x1*dy+Pt0.y;
			GraphAPI::GGetPerpendicular(l1.sp.x,l1.sp.y,l1.ep.x,l1.ep.y,xcenter,ycenter,&(ret1->x),&(ret1->y),t1);
			GraphAPI::GGetPerpendicular(l2.sp.x,l2.sp.y,l2.ep.x,l2.ep.y,xcenter,ycenter,&(ret2->x),&(ret2->y),t2);
		}
		return 3;
	}
	else 
	{
		PT_2D tem;
		//mode是标志是圆弧还是折线倒角
		if (mode==1)
		{
			GraphAPI::GGetPerpendicular(Pt0.x,Pt0.y,Pt1.x,Pt1.y,pt.x,pt.y,&(tem.x),&(tem.y),NULL);
			GraphAPI::GGetLineIntersectLine(l1.sp.x,l1.sp.y,l1.ep.x-l1.sp.x,l1.ep.y-l1.sp.y,pt.x,pt.y,tem.x-pt.x,tem.y-pt.y,&(ret1->x),&(ret1->y),t1);
			GraphAPI::GGetLineIntersectLine(l2.sp.x,l2.sp.y,l2.ep.x-l2.sp.x,l2.ep.y-l2.sp.y,pt.x,pt.y,tem.x-pt.x,tem.y-pt.y,&(ret2->x),&(ret2->y),t2);
		}
		if(mode==0)
		{
			GraphAPI::GGetPerpendicular(Pt0.x,Pt0.y,Pt1.x,Pt1.y,pt.x,pt.y,&(tem.x),&(tem.y),NULL);
			pt=tem;
			double da=l1.ep.x-l1.sp.x;
			double db=l1.ep.y-l1.sp.y;
			double dx=Pt1.x-Pt0.x;
			double dy=Pt1.y-Pt0.y;
			double a=(da*da+db*db)*(dx*dx+dy*dy)-(da*dy-db*dx)*(da*dy-db*dx);
			double b=2*((da*da+db*db)*((Pt0.x-tem.x)*dx+(Pt0.y-tem.y)*dy)-(da*dy-db*dx)*(da*(Pt0.y-l1.sp.y)-db*(Pt0.x-l1.sp.x)));
			double c=(da*da+db*db)*((Pt0.x-tem.x)*(Pt0.x-tem.x)+(Pt0.y-tem.y)*(Pt0.y-tem.y))-(da*(Pt0.y-l1.sp.y)-db*(Pt0.x-l1.sp.x))*(da*(Pt0.y-l1.sp.y)-db*(Pt0.x-l1.sp.x));
			double x1,x2;
			x1=(-b+sqrt(b*b-4*a*c))/(2*a);
			x2=(-b-sqrt(b*b-4*a*c))/(2*a);
			x1=x1>x2?x1:x2;
			double  xcenter = x1*dx+Pt0.x;
			double  ycenter = x1*dy+Pt0.y;
			GraphAPI::GGetPerpendicular(l1.sp.x,l1.sp.y,l1.ep.x,l1.ep.y,xcenter,ycenter,&(ret1->x),&(ret1->y),t1);
			GraphAPI::GGetPerpendicular(l2.sp.x,l2.sp.y,l2.ep.x,l2.ep.y,xcenter,ycenter,&(ret2->x),&(ret2->y),t2);
		}
		return 4;
	}
}

void CChamferCommand::PtClick(PT_3D &pt, int flag)
{
    if( !m_pEditor )return;		
	if(m_nStep==0 )//选择第一个地物，并获取待处理的线段
	{
		if( !CanGetSelObjs(flag,FALSE) )return;
		m_pOldHandles[0] = m_pEditor->GetSelection()->GetLastSelectedObj();
		if (!m_pOldHandles[0])
		{
			return;
		}
		m_pNewFtrs[0] =HandleToFtr(m_pOldHandles[0])->Clone();
		if( !(m_pNewFtrs[0]) )return;
		if( !(m_pNewFtrs[0]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return;
		int PtNums;
		if((PtNums=(m_pNewFtrs[0]->GetGeometry())->GetDataPointSum())<2 )return;
		const CShapeLine *pSL = (m_pNewFtrs[0]->GetGeometry())->GetShape();
		if( !pSL )return;
		CArray<PT_3DEX,PT_3DEX> pts;
		pSL->GetPts(pts);			
		
		PT_2D *Pnts=new PT_2D[PtNums];
		for (int j=0;j<PtNums;j++)//获取点数组
		{
			COPY_2DPT(Pnts[j],pts[j]);
		}
		//		delete []pts;
		int idx;
		PT_2D tem,tem1;
		COPY_2DPT(tem,pt);
		GraphAPI::GGetNearstDisPToPointSet2D(Pnts,PtNums,tem,tem1,&idx);
		//判断取出首尾哪条线段来进行倒角处理
		if (GraphAPI::GGetAllLen2D(Pnts,idx+1)+GraphAPI::GGet2DDisOf2P(Pnts[idx],tem1)<GraphAPI::GGetAllLen2D(&(Pnts[idx+1]),PtNums-1-idx)+GraphAPI::GGet2DDisOf2P(tem1,Pnts[idx+1]))
		{
			m_lineseg[0].sp=Pnts[1];
			m_lineseg[0].ep=Pnts[0];
		}
		else
		{
			m_lineseg[0].sp=Pnts[PtNums-2];
			m_lineseg[0].ep=Pnts[PtNums-1];
			
		}
		
		//		m_pEditor->SetCurDrawingObj(DrawingInfo(m_pNewFtrs[0]->GetGeometry(),(m_pNewFtrs[0]->GetGeometry())->GetDataPointSum()+1));
		
		if (Pnts)
		{
			delete []Pnts;
			Pnts=NULL;
		}
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
	}
	//选取第二条线段
	else if (m_nStep==1)
	{
		if( !CanGetSelObjs(flag,FALSE) )return;
		m_pOldHandles[1] = m_pEditor->GetSelection()->GetLastSelectedObj();
		if( m_pOldHandles[1]==0 )
			return;
		
		m_pNewFtrs[1]=HandleToFtr(m_pOldHandles[1])->Clone();
		if( !(m_pNewFtrs[1]) )return;
		if( !(m_pNewFtrs[1]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return;
		int PtNums;
		if((m_pNewFtrs[1]->GetGeometry())->GetDataPointSum()<2 )return;
		const CShapeLine *pSL = (m_pNewFtrs[1])->GetGeometry()->GetShape();
		if( !pSL )return;
		CArray<PT_3DEX,PT_3DEX> pts;
		pSL->GetPts(pts);
		PtNums = pts.GetSize();
		PT_2D *Pnts=new PT_2D[PtNums];
		for (int j=0;j<PtNums;j++)//获取点数组
		{
			COPY_2DPT(Pnts[j],pts[j]);
		}	
		
		PT_2D tem;
		COPY_2DPT(tem,pt);
		//此时直接根据所点击点距断点的远近来判断
		if (GraphAPI::GGet2DDisOf2P(Pnts[0],tem)<GraphAPI::GGet2DDisOf2P(tem,Pnts[PtNums-1]))
		{
			m_lineseg[1].sp=Pnts[1];
			m_lineseg[1].ep=Pnts[0];
		}
		else
		{
			m_lineseg[1].sp=Pnts[PtNums-2];
			m_lineseg[1].ep=Pnts[PtNums-1];
			
		}
		//		m_pEditor->SetCurDrawingObj(DrawingInfo(m_pNewFtrs[1]->GetGeometry(),(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum()+1));
		
		m_pEditor->CloseSelector();
		if (Pnts)
		{
			delete []Pnts;
			Pnts=NULL;
		}
		m_nStep = 2;
		GotoState(PROCSTATE_PROCESSING);
		
	}
	//进行倒角处理
	else
	{
		if(!m_pNewFtrs[0]||!m_pNewFtrs[1])
			return;
		//层必须相同
		CFtrLayer* pLay0 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(m_pOldHandles[0]));
		CFtrLayer* pLay1 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(m_pOldHandles[1]));
		
		if (pLay0!=pLay1&&strcmp(pLay0->GetName(),pLay1->GetName())!=0)
		{
			return;
		}
		CArray<PT_3DEX,PT_3DEX> pts0;
		CArray<PT_3DEX,PT_3DEX> pts1;
		
		(m_pNewFtrs[0]->GetGeometry())->GetShape(pts0);
		(m_pNewFtrs[1]->GetGeometry())->GetShape(pts1);
		//
		PT_2D p1,p2;
		double  t1,t2;
		//较复杂，根据第三下点击的方位决定如何进行倒角处理
		if (m_nMode==modePolylineChamfer)
		{
			
			PT_2D pt2d,intersec;
            COPY_2DPT(pt2d,pt);
			//射线延长线有交点再进行进一步处理
			if(GraphAPI::GGetRay1IntersectRay2(m_lineseg[0].sp.x,m_lineseg[0].sp.y,m_lineseg[0].ep.x,m_lineseg[0].ep.y,m_lineseg[1].sp.x,m_lineseg[1].sp.y,m_lineseg[1].ep.x,m_lineseg[1].ep.y,&(intersec.x),&(intersec.y)))
			{
				
				if( !CanGetSelObjs(flag) )return;				
				if( !(m_pNewFtrs[0]) )return;
				if( !(m_pNewFtrs[0]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return;
				
				if(((m_pNewFtrs[0]->GetGeometry())->GetDataPointSum())<2 )return;
				
				if( !(m_pNewFtrs[1]) )return;
				if( !(m_pNewFtrs[1]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return;
				
				if(((m_pNewFtrs[1]->GetGeometry())->GetDataPointSum())<2 )return;			
				
				CUndoFtrs undo(m_pEditor,Name());
				//K代表点所在区域
				int K = GPtinRegion(m_lineseg[0],m_lineseg[1],pt2d,&p1,&p2,&t1,&t2,1);
				if(K==4||K==3)
				{
					
					PT_3DEX temp;
					PT_3D temp1,temp2;
					temp = pts0.GetAt(0);					  
					COPY_3DPT(temp1,temp);
					temp2.x=m_lineseg[0].ep.x;
					temp2.y=m_lineseg[0].ep.y;
					temp2.z=0;
					if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
					{	
						temp = pts0.GetAt(0);
						//z=temp.z;					
						temp.pencode=penLine;
						temp.x=p1.x;
						temp.y=p1.y;
						//temp.z=z;	
						pts0.SetAt(0,temp);
						//	(m_pNewFtrs[0]->GetGeometry())->SetDataPoint(0,temp);
						
						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = pts1.GetAt(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{
							temp = pts1.GetAt(0);
							
							temp.pencode=penLine;
							temp.x=p2.x;
							temp.y=p2.y;
							
							pts0.InsertAt(0,temp);
							
							pts1.RemoveAt(0);
							int ptnum=pts1.GetSize();
							for (int j=0;j<ptnum;j++)
							{
								pts0.InsertAt(0,pts1.GetAt(j));
							}
						}
						else
						{
							int ptnum=pts1.GetSize();
							
							temp = pts1.GetAt(ptnum-1);
							
							temp.pencode=penLine;
							temp.x=p2.x;
							temp.y=p2.y;
							
							pts0.InsertAt(0,temp);
							
							pts1.RemoveAt(ptnum-1);
							for (int j=ptnum-2;j>=0;j--)
							{								
								pts0.InsertAt(0,pts1.GetAt(j));
							}							
						}						
					}
					else
					{
						int ptnum1=pts0.GetSize();
						
						temp = pts0.GetAt(ptnum1-1);						
						
						pts0.RemoveAt(ptnum1-1);
						temp.pencode=penLine;
						temp.x=p1.x;
						temp.y=p1.y;
						
						pts0.Add(temp);
						
						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = pts1.GetAt(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{
							temp = pts1.GetAt(0);
							
							temp.pencode=penLine;
							temp.x=p2.x;
							temp.y=p2.y;
							
							pts0.Add(temp);
							
							pts1.RemoveAt(0);
							int ptnum=pts1.GetSize();
							for (int j=0;j<ptnum;j++)
							{
								pts0.Add(pts1.GetAt(j));
							}
						}
						else
						{
							int ptnum=pts1.GetSize();
							
							temp = pts1.GetAt(ptnum-1);
							
							temp.pencode=penLine;
							temp.x=p2.x;
							temp.y=p2.y;
							
							pts0.Add(temp);
							
							pts1.RemoveAt(ptnum-1);
							for (int j=ptnum-2;j>=0;j--)
							{
								pts0.Add(pts1.GetAt(j));
							}						
						}						
					}
					(m_pNewFtrs[0]->GetGeometry())->CreateShape(pts0.GetData(),pts0.GetSize());
					m_pNewFtrs[0]->SetID(OUID());
					m_pEditor->AddObject(m_pNewFtrs[0],m_pEditor->GetFtrLayerIDOfFtr(m_pOldHandles[0]));

					GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_pOldHandles[0]),m_pNewFtrs[0]);
					undo.arrNewHandles.Add(FtrToHandle(m_pNewFtrs[0]));

					m_pEditor->DeleteObject(m_pOldHandles[0]);					
					m_pEditor->DeleteObject(m_pOldHandles[1]);
					
					undo.arrOldHandles.Add(m_pOldHandles[0]);
					undo.arrOldHandles.Add(m_pOldHandles[1]);

					
					undo.Commit();
					m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
					m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
					m_pEditor->RefreshView();	
					
				}
				if (K==1)
				{
					
					m_pEditor->RefreshView();
					
					PT_3DEX temp;
					PT_3D temp1,temp2;
					temp = pts0.GetAt(0);
					double z;
					COPY_3DPT(temp1,temp);
					temp2.x=m_lineseg[0].ep.x;
					temp2.y=m_lineseg[0].ep.y;
					temp2.z=0;
					if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
					{						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = pts1.GetAt(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{							
							int ptnum=pts1.GetSize();
							
							temp = pts0.GetAt(0);
							z=temp.z;
							temp = pts1.GetAt(0);
							z=(z+temp.z)/2;
							temp.pencode=penLine;
							temp.x=intersec.x;
							temp.y=intersec.y;
							temp.z=z;						
							pts0.InsertAt(0,temp);
							
							for (int j=0;j<ptnum;j++)
							{
								pts0.InsertAt(0,pts1.GetAt(j));
							}
						}
						else
						{
							int ptnum=pts1.GetSize();	
							
							temp = pts0.GetAt(0);
							z=temp.z;
							temp = pts1.GetAt(ptnum-1);
							z=(z+temp.z)/2;
							temp.pencode=penLine;
							temp.x=intersec.x;
							temp.y=intersec.y;
							temp.z=z;						
							pts0.InsertAt(0,temp);
							
							for (int j=ptnum-1;j>=0;j--)
							{								
								pts0.InsertAt(0,pts1.GetAt(j));
							}
							
						}
						
					}
					else
					{						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = pts1.GetAt(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{								
							int ptnum=pts1.GetSize();
							
							temp = pts0.GetAt(pts0.GetSize()-1);
							z=temp.z;
							temp = pts1.GetAt(0);
							z=(z+temp.z)/2;
							temp.pencode=penLine;
							temp.x=intersec.x;
							temp.y=intersec.y;
							temp.z=z;									
							pts0.Add(temp);
							
							for (int j=0;j<ptnum;j++)
							{								
								pts0.Add(pts1.GetAt(j));
							}
						}
						else
						{
							int ptnum=pts1.GetSize();	
							
							temp = pts0.GetAt(pts0.GetSize()-1);
							z=temp.z;
							temp = pts1.GetAt(ptnum-1);
							z=(z+temp.z)/2;
							temp.pencode=penLine;
							temp.x=intersec.x;
							temp.y=intersec.y;
							temp.z=z;									
							pts0.Add(temp);
							
							for (int j=ptnum-1;j>=0;j--)
							{
								pts0.Add(pts1.GetAt(j));
							}
							
						}
						
					}
					
					m_pNewFtrs[0]->SetID(OUID());
					(m_pNewFtrs[0]->GetGeometry())->CreateShape(pts0.GetData(),pts0.GetSize());
					m_pEditor->AddObject(m_pNewFtrs[0],m_pEditor->GetFtrLayerIDOfFtr(m_pOldHandles[0]));
					
					GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_pOldHandles[0]),m_pNewFtrs[0]);			
					undo.arrNewHandles.Add(FtrToHandle(m_pNewFtrs[0]));
					
					m_pEditor->DeleteObject(m_pOldHandles[0]);					
					m_pEditor->DeleteObject(m_pOldHandles[1]);
					
					undo.arrOldHandles.Add(m_pOldHandles[0]);
					undo.arrOldHandles.Add(m_pOldHandles[1]);

					undo.Commit();
					
					m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
					m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
					m_pEditor->RefreshView();					
					
					}
					if(K==2)
					{
						
						m_pEditor->RefreshView();
						
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = pts0.GetAt(0);
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{
							pts0.RemoveAt(0);
							
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								pts1.RemoveAt(0);
								int ptnum=pts1.GetSize();
								for (int j=0;j<ptnum;j++)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();
								pts1.RemoveAt(ptnum-1);
								for (int j=ptnum-2;j>=0;j--)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}
								
							}
							
						}
						else
						{
							int ptnum1=pts0.GetSize();
							pts0.RemoveAt(ptnum1-1);
							
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								pts1.RemoveAt(0);
								int ptnum=pts1.GetSize();
								for (int j=0;j<ptnum;j++)
								{
									pts0.Add(pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();
								pts1.RemoveAt(ptnum-1);
								for (int j=ptnum-2;j>=0;j--)
								{
									pts0.Add(pts1.GetAt(j));
								}
								
							}
							
						}
						(m_pNewFtrs[0]->GetGeometry())->CreateShape(pts0.GetData(),pts0.GetSize());
						m_pNewFtrs[0]->SetID(OUID());
						m_pEditor->AddObject(m_pNewFtrs[0],m_pEditor->GetFtrLayerIDOfFtr(m_pOldHandles[0]));
						
						GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_pOldHandles[0]),m_pNewFtrs[0]);			
						undo.arrNewHandles.Add(FtrToHandle(m_pNewFtrs[0]));
						
						m_pEditor->DeleteObject(m_pOldHandles[0]);					
						m_pEditor->DeleteObject(m_pOldHandles[1]);
						
						undo.arrOldHandles.Add(m_pOldHandles[0]);
						undo.arrOldHandles.Add(m_pOldHandles[1]);

						undo.Commit();
						
						m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
						m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
						m_pEditor->RefreshView();		
						
						
						
					}								
				}
				m_pNewFtrs[0]=NULL;
				m_pNewFtrs[1]=NULL;
				
			}
			if(m_nMode==modeArcChamfer)
			{
				PT_2D pt2d,intersec;
				COPY_2DPT(pt2d,pt);
				if(GraphAPI::GGetRay1IntersectRay2(m_lineseg[0].sp.x,m_lineseg[0].sp.y,m_lineseg[0].ep.x,m_lineseg[0].ep.y,m_lineseg[1].sp.x,m_lineseg[1].sp.y,m_lineseg[1].ep.x,m_lineseg[1].ep.y,&(intersec.x),&(intersec.y)))
				{
					////////////////////////////////
					if( !CanGetSelObjs(flag) )return;				
					if( !(m_pNewFtrs[0]) )return;
					if( !(m_pNewFtrs[0]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return;
					
					if(pts0.GetSize()<2 )return;
					
					if( !(m_pNewFtrs[1]) )return;
					if( !(m_pNewFtrs[1]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return;
					
					if(pts1.GetSize()<2 )return;			
					///////////////////////////////////////////
					
					int K=GPtinRegion(m_lineseg[0],m_lineseg[1],pt2d,&p1,&p2,&t1,&t2,0);
					CUndoFtrs undo(m_pEditor,Name());
					if(K==4||K==3)
					{						
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = pts0.GetAt(0);
						double z;						
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;

						int addNum = 0;

						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{	
							temp = pts0.GetAt(0);
							z=temp.z;
							
							pts0.RemoveAt(0);
							if(t1>0&&t2>0)
							{
								temp.pencode=penArc;
								temp.x=p1.x;
								temp.y=p1.y;
								temp.z=z;						
								pts0.InsertAt(0,temp);

								addNum++;
								
							}							
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								temp = pts1.GetAt(0);
								z=(z+temp.z)/2;
								if(t1>0&&t2>0)
								{
									if (K==3)
									{
										temp.pencode=penArc;
										temp.x=pt.x;
										temp.y=pt.y;
										temp.z=z;
										pts0.InsertAt(0,temp);

										addNum++;
									}
									else
									{
										temp.pencode=penArc;
										temp.x=pt2d.x;
										temp.y=pt2d.y;
										temp.z=z;
										pts0.InsertAt(0,temp);

										addNum++;
									}
									temp = pts1.GetAt(0);
									z=temp.z;
									temp.pencode=penLine;
									temp.x=p2.x;
									temp.y=p2.y;
									temp.z=z;	
									pts0.InsertAt(0,temp);

									addNum++;
									
									if (m_bLineserial)
									{
										CGeoCurve curve;
										curve.CreateShape(pts0.GetData(),addNum);
										CGeometry *pNewGeo = curve.Linearize();
										if (pNewGeo)
										{
											pts0.RemoveAt(0,addNum);
											
											CArray<PT_3DEX,PT_3DEX> newPts;
											pNewGeo->GetShape(newPts);
											
											for (int j=newPts.GetSize()-1;j>=0;j--)
											{
												pts0.InsertAt(0,newPts.GetAt(j));
											}
											
											delete pNewGeo;
											
										}
										
										
									}
								}
								
								pts1.RemoveAt(0);
								int ptnum=pts1.GetSize();
								for (int j=0;j<ptnum;j++)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();
								
								temp = pts1.GetAt(ptnum-1);
								z=(z+temp.z)/2;
								if(t1>0&&t2>0)
								{
									if (K==3)
									{
										temp.pencode=penArc;
										temp.x=pt.x;
										temp.y=pt.y;
										temp.z=z;
										pts0.InsertAt(0,temp);

										addNum++;
									}
									else
									{
										temp.pencode=penArc;
										temp.x=pt2d.x;
										temp.y=pt2d.y;
										temp.z=z;
										pts0.InsertAt(0,temp);

										addNum++;
									}
									temp = pts1.GetAt(ptnum-1);
									z=temp.z;
									temp.pencode=penLine;
									temp.x=p2.x;
									temp.y=p2.y;
									temp.z=z;	
									pts0.InsertAt(0,temp);

									addNum++;
									
									if (m_bLineserial)
									{
										CGeoCurve curve;
										curve.CreateShape(pts0.GetData(),addNum);
										CGeometry *pNewGeo = curve.Linearize();
										if (pNewGeo)
										{
											pts0.RemoveAt(0,addNum);
											
											CArray<PT_3DEX,PT_3DEX> newPts;
											pNewGeo->GetShape(newPts);
											
											for (int j=newPts.GetSize()-1;j>=0;j--)
											{
												pts0.InsertAt(0,newPts.GetAt(j));
											}
											
											delete pNewGeo;
											
										}
										
										
									}
								}
								
								pts1.RemoveAt(ptnum-1);
								for (int j=ptnum-2;j>=0;j--)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}								
							}							
						}
						else
						{
							int ptnum1=pts0.GetSize();
							
							temp = pts0.GetAt(ptnum1-1);
							z=temp.z;
							
							pts0.RemoveAt(ptnum1-1);
							if(t1>0&&t2>0)
							{
								temp.pencode=penLine;
								temp.x=p1.x;
								temp.y=p1.y;
								temp.z=z;						
								pts0.Add(temp);

								addNum++;
								
							}
							
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								temp = pts1.GetAt(0);
								z=(z+temp.z)/2;
								
								if(t1>0&&t2>0)
								{
									if (K==3)
									{
										temp.pencode=penArc;
										temp.x=pt.x;
										temp.y=pt.y;
										temp.z=z;
										pts0.Add(temp);

										addNum++;
									}
									else
									{
										temp.pencode=penArc;
										temp.x=pt2d.x;
										temp.y=pt2d.y;
										temp.z=z;
										pts0.Add(temp);

										addNum++;
									}
									temp = pts1.GetAt(0);
									z=temp.z;
									temp.pencode=penArc;
									temp.x=p2.x;
									temp.y=p2.y;
									temp.z=z;	
									pts0.Add(temp);

									addNum++;
									
									if (m_bLineserial)
									{
										CGeoCurve curve;
										curve.CreateShape(pts0.GetData()+pts0.GetSize()-addNum,addNum);
										CGeometry *pNewGeo = curve.Linearize();
										if (pNewGeo)
										{
											pts0.RemoveAt(pts0.GetSize()-addNum,addNum);
											
											CArray<PT_3DEX,PT_3DEX> newPts;
											pNewGeo->GetShape(newPts);
											
											for (int j=0;j<newPts.GetSize();j++)
											{
												pts0.Add(newPts.GetAt(j));
											}
											
											delete pNewGeo;
											
										}
										
										
									}
								}
								
								pts1.RemoveAt(0);
								int ptnum=pts1.GetSize();
								for (int j=0;j<ptnum;j++)
								{
									pts0.Add(pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();
								
								temp = pts1.GetAt(ptnum-1);
								z=(z+temp.z)/2;
								
								if(t1>0&&t2>0)
								{
									if (K==3)
									{
										temp.pencode=penArc;
										temp.x=pt.x;
										temp.y=pt.y;
										temp.z=z;
										pts0.Add(temp);

										addNum++;
									}
									else
									{
										temp.pencode=penArc;
										temp.x=pt2d.x;
										temp.y=pt2d.y;
										temp.z=z;
										pts0.Add(temp);

										addNum++;
									}
									temp = pts1.GetAt(ptnum-1);
									z=temp.z;
									temp.pencode=penArc;
									temp.x=p2.x;
									temp.y=p2.y;
									temp.z=z;	
									pts0.Add(temp);

									addNum++;
									
									if (m_bLineserial)
									{
										CGeoCurve curve;
										curve.CreateShape(pts0.GetData()+pts0.GetSize()-addNum,addNum);
										CGeometry *pNewGeo = curve.Linearize();
										if (pNewGeo)
										{
											pts0.RemoveAt(pts0.GetSize()-addNum,addNum);
											
											CArray<PT_3DEX,PT_3DEX> newPts;
											pNewGeo->GetShape(newPts);
											
											for (int j=0;j<newPts.GetSize();j++)
											{
												pts0.Add(newPts.GetAt(j));
											}
											
											delete pNewGeo;
											
										}
										
										
									}
								}
								
								pts1.RemoveAt(ptnum-1);
								for (int j=ptnum-2;j>=0;j--)
								{
									pts0.Add(pts1.GetAt(j));
								}
								
							}
							
						}
						(m_pNewFtrs[0]->GetGeometry())->CreateShape(pts0.GetData(),pts0.GetSize());
						m_pNewFtrs[0]->SetID(OUID());
						m_pEditor->AddObject(m_pNewFtrs[0],m_pEditor->GetFtrLayerIDOfFtr(m_pOldHandles[0]));
						
						GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_pOldHandles[0]),m_pNewFtrs[0]);			
						undo.arrNewHandles.Add(FtrToHandle(m_pNewFtrs[0]));
						
						m_pEditor->DeleteObject(m_pOldHandles[0]);					
						m_pEditor->DeleteObject(m_pOldHandles[1]);
						
						undo.arrOldHandles.Add(m_pOldHandles[0]);
						undo.arrOldHandles.Add(m_pOldHandles[1]);

						undo.Commit();
						m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
						m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
						m_pEditor->RefreshView();		
						
					}
					if (K==1)
					{						
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = pts0.GetAt(0);
						double z;
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{								
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{							
								int ptnum=pts1.GetSize();
								
								temp = pts0.GetAt(0);
								z=temp.z;
								temp = pts1.GetAt(0);
								z=(z+temp.z)/2;
								temp.pencode=penLine;
								temp.x=intersec.x;
								temp.y=intersec.y;
								temp.z=z;						
								pts0.InsertAt(0,temp);
								
								for (int j=0;j<ptnum;j++)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();	
								
								temp = pts0.GetAt(0);
								z=temp.z;
								temp = pts1.GetAt(ptnum-1);
								z=(z+temp.z)/2;
								temp.pencode=penLine;
								temp.x=intersec.x;
								temp.y=intersec.y;
								temp.z=z;						
								pts0.InsertAt(0,temp);
								
								for (int j=ptnum-1;j>=0;j--)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}
								
							}
							
						}
						else
						{
							
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{								
								int ptnum=pts1.GetSize();
								
								temp = pts0.GetAt(pts0.GetSize()-1);
								z=temp.z;
								temp = pts1.GetAt(0);
								z=(z+temp.z)/2;
								temp.pencode=penLine;
								temp.x=intersec.x;
								temp.y=intersec.y;
								temp.z=z;									
								pts0.Add(temp);
								
								for (int j=0;j<ptnum;j++)
								{
									pts0.Add(pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();	
								
								temp = pts0.GetAt(pts0.GetSize()-1);
								z=temp.z;
								temp = pts1.GetAt(ptnum-1);
								z=(z+temp.z)/2;
								temp.pencode=penLine;
								temp.x=intersec.x;
								temp.y=intersec.y;
								temp.z=z;									
								pts0.Add(temp);
								
								for (int j=ptnum-1;j>=0;j--)
								{
									pts0.Add(pts1.GetAt(j));
								}
								
							}
							
						}
						(m_pNewFtrs[0]->GetGeometry())->CreateShape(pts0.GetData(),pts0.GetSize());

						m_pNewFtrs[0]->SetID(OUID());
						m_pEditor->AddObject(m_pNewFtrs[0],m_pEditor->GetFtrLayerIDOfFtr(m_pOldHandles[0]));
						
						GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_pOldHandles[0]),m_pNewFtrs[0]);			
						undo.arrNewHandles.Add(FtrToHandle(m_pNewFtrs[0]));
						
						m_pEditor->DeleteObject(m_pOldHandles[0]);					
						m_pEditor->DeleteObject(m_pOldHandles[1]);
						
						undo.arrOldHandles.Add(m_pOldHandles[0]);
						undo.arrOldHandles.Add(m_pOldHandles[1]);
						
						undo.Commit();
						m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
						m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
						m_pEditor->RefreshView();	
						
						
						
					}
					if(K==2)
					{						
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = pts0.GetAt(0);
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{
							pts0.RemoveAt(0);
							
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								pts1.RemoveAt(0);
								int ptnum=pts1.GetSize();
								for (int j=0;j<ptnum;j++)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();
								pts1.RemoveAt(ptnum-1);
								for (int j=ptnum-2;j>=0;j--)
								{
									pts0.InsertAt(0,pts1.GetAt(j));
								}
								
							}
							
						}
						else
						{
							int ptnum1=pts0.GetSize();
							pts0.RemoveAt(ptnum1-1);
							
							
							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = pts1.GetAt(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								pts1.RemoveAt(0);
								int ptnum=pts1.GetSize();
								for (int j=0;j<ptnum;j++)
								{
									pts0.Add(pts1.GetAt(j));
								}
							}
							else
							{
								int ptnum=pts1.GetSize();
								pts1.RemoveAt(ptnum-1);
								for (int j=ptnum-2;j>=0;j--)
								{
									pts0.Add(pts1.GetAt(j));
								}
								
							}
							
						}
						(m_pNewFtrs[0]->GetGeometry())->CreateShape(pts0.GetData(),pts0.GetSize());
						
						m_pNewFtrs[0]->SetID(OUID());
						m_pEditor->AddObject(m_pNewFtrs[0],m_pEditor->GetFtrLayerIDOfFtr(m_pOldHandles[0]));
						
						GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_pOldHandles[0]),m_pNewFtrs[0]);			
						undo.arrNewHandles.Add(FtrToHandle(m_pNewFtrs[0]));
						
						m_pEditor->DeleteObject(m_pOldHandles[0]);					
						m_pEditor->DeleteObject(m_pOldHandles[1]);
						
						undo.arrOldHandles.Add(m_pOldHandles[0]);
						undo.arrOldHandles.Add(m_pOldHandles[1]);

						undo.Commit();
						
						m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
						m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
						m_pEditor->RefreshView();						
						
					}								
				}
				m_pNewFtrs[0]=NULL;
				m_pNewFtrs[1]=NULL;
				
				
		}
		m_nStep=3;
		Finish();
	}	
		
	CEditCommand::PtClick(pt,flag);
}

void CChamferCommand::PtMove(PT_3D &pt)
{
	if (!m_pEditor)
	{
		return;
	}
	if(m_nStep==2)
	{
		CFtrLayer* pLayer0 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(m_pOldHandles[0]));
		CFtrLayer* pLayer1 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(m_pOldHandles[1]));
		
		if (pLayer0!=pLayer1&&strcmp(pLayer1->GetName(),pLayer0->GetName())!=0)
		{
			return;
		}
		PT_3DEX temp;
		temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(0);
		double z;
		PT_2D p1,p2;
		double  t1,t2;
		if (m_nMode==modePolylineChamfer)
		{
			GrBuffer buf;
			buf.BeginLineString(0,0);
			PT_3D pt1,pt2,pt3;
			
			PT_2D pt2d,intersec;
            COPY_2DPT(pt2d,pt);
			if(GraphAPI::GGetRay1IntersectRay2(m_lineseg[0].sp.x,m_lineseg[0].sp.y,m_lineseg[0].ep.x,m_lineseg[0].ep.y,m_lineseg[1].sp.x,m_lineseg[1].sp.y,m_lineseg[1].ep.x,m_lineseg[1].ep.y,&intersec.x,&intersec.y))
			{
				
				int K=GPtinRegion(m_lineseg[0],m_lineseg[1],pt2d,&p1,&p2,&t1,&t2,1);
				if(K==4||K==3)
				{
					PT_3DEX temp;
					PT_3D temp1,temp2;
					temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(0);

					COPY_3DPT(temp1,temp);
					temp2.x=m_lineseg[0].ep.x;
					temp2.y=m_lineseg[0].ep.y;
					temp2.z=0;
					if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
					{	
						temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(0);
						z=temp.z;
						pt1.x=p1.x;
						pt1.y=p1.y;
						pt1.z=z;
						
						buf.MoveTo(&pt1);
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
							z=temp.z;
							pt2.x=p2.x;
							pt2.y=p2.y;
							pt2.z=z;							
							buf.LineTo(&pt2);

						}
						else
						{
							int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(ptnum-1);
							z=temp.z;
							pt2.x=p2.x;
							pt2.y=p2.y;
							pt2.z=z;							
							buf.LineTo(&pt2);
						}
					}
					else
					{
						int ptnum1=(m_pNewFtrs[0]->GetGeometry())->GetDataPointSum();
						
						temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(ptnum1-1);
						z=temp.z;
						pt1.x=p1.x;
						pt1.y=p1.y;
						pt1.z=z;
						buf.MoveTo(&pt1);
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
							z=temp.z;
							pt2.x=p2.x;
							pt2.y=p2.y;
							pt2.z=z;							
							buf.LineTo(&pt2);
						}
						else
						{
							int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();
							
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(ptnum-1);
							z=temp.z;
							pt2.x=p2.x;
							pt2.y=p2.y;
							pt2.z=z;							
							buf.LineTo(&pt2);

						}

					}		

					if (t1>1)
					{
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(0);
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;					
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{
							pt3=temp1;

						}
						else
						{
							temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-1);
							pt3.x=m_lineseg[0].ep.x;
							pt3.y=m_lineseg[0].ep.y;
							pt3.z=temp.z;

						}

						
						buf.MoveTo(&pt3);
						buf.LineTo(&pt1);
					}
					
					if(t2>1)
					{
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(1);
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[1].ep.x;
						temp2.y=m_lineseg[1].ep.y;
						temp2.z=0;					
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{
							pt3=temp1;
							
						}
						else
						{
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(m_pNewFtrs[1]->GetGeometry()->GetDataPointSum()-1);
							pt3.x=m_lineseg[1].ep.x;
							pt3.y=m_lineseg[1].ep.y;
							pt3.z=temp.z;
							
						}

						
						buf.MoveTo(&pt3);
						buf.LineTo(&pt2);
						
					}
					
					
				}
				if (K==1)
				{
					PT_3DEX temp;
					PT_3D temp1,temp2;
					temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(0);
					double z;
					COPY_3DPT(temp1,temp);
					temp2.x=m_lineseg[0].ep.x;
					temp2.y=m_lineseg[0].ep.y;
					temp2.z=0;
					if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
					{	
						buf.MoveTo(&temp1);
						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{						
							
							temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(0);
							z=temp.z;
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;
							buf.LineTo(&pt1);
							buf.LineTo(&tem1);


						}
						else
						{
							int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();	
							
							temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(0);
							z=temp.z;
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(ptnum-1);

							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;

							buf.LineTo(&pt1);
							buf.LineTo(&temp);

						}
// 						buf.MoveTo(&pt1);
// 						buf.LineTo(&temp1);
					}
					else
					{
						temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint((m_pNewFtrs[0]->GetGeometry())->GetDataPointSum()-1);
						buf.MoveTo(&temp);
						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{								
							int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();
							
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-1);
							z=temp.z;
							temp = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;

 							buf.LineTo(&pt1);
							buf.LineTo(&tem1);

						}
						else
						{
							int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();	
							
							temp = (m_pNewFtrs[0]->GetGeometry())->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-1);
							z=temp.z;
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(ptnum-1);
							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;

 							buf.LineTo(&pt1);
							buf.LineTo(&temp);
						}
						tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(m_pNewFtrs[1]->GetGeometry()->GetDataPointSum()-1);
						COPY_3DPT(tem1,tem);

					
					}					
					
				}
				if(K==2)
				{
					PT_3DEX temp;
					PT_3D temp1,temp2;
					temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
					COPY_3DPT(temp1,temp);
					temp2.x=m_lineseg[0].ep.x;
					temp2.y=m_lineseg[0].ep.y;
					temp2.z=0;

					pt1.x=m_lineseg[1].sp.x;
					pt1.y=m_lineseg[1].sp.y;
					if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
					{
						temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(1);						
						pt1.z=temp.z;
					}
					else
					{
						temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-2);						
						pt1.z=temp.z;
					}
					buf.MoveTo(&pt1);

					PT_3DEX tem;
					PT_3D tem1,tem2;
					tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
					COPY_3DPT(tem1,tem);
					tem2.x=m_lineseg[1].ep.x;
					tem2.y=m_lineseg[1].ep.y;
					tem2.z=0;

					pt2.x=m_lineseg[0].sp.x;
					pt2.y=m_lineseg[0].sp.y;
					if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
					{	
						temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(1);						
						pt2.z=temp.z;
					}
					else
					{
						temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(m_pNewFtrs[1]->GetGeometry()->GetDataPointSum()-2);						
						pt2.z=temp.z;
					}
					buf.LineTo(&pt2);					
				}
				buf.End();
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
				
				
			}
			
		}
		if(m_nMode==modeArcChamfer)
		{
			
			CShapeLine buf;
			GrBuffer buf0;
			GrBuffer buf1;
			buf0.BeginLineString(0,0);
			buf1.BeginLineString(0,0);
			CLinearizer bufb;
			bufb.SetShapeBuf(&buf);
			
			PT_3D pt1,pt2,pt3;
			
			PT_2D pt2d,intersec;
            COPY_2DPT(pt2d,pt);
			if(GraphAPI::GGetRay1IntersectRay2(m_lineseg[0].sp.x,m_lineseg[0].sp.y,m_lineseg[0].ep.x,m_lineseg[0].ep.y,m_lineseg[1].sp.x,m_lineseg[1].sp.y,m_lineseg[1].ep.x,m_lineseg[1].ep.y,&intersec.x,&intersec.y))
			{
				int K=GPtinRegion(m_lineseg[0],m_lineseg[1],pt2d,&p1,&p2,&t1,&t2,0);
				if(K==4||K==3)
				{
					if(t1>0&&t2>0)
					{
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp =m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
						double z;						
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;
					
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{	
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
							z=temp.z;
							pt1.x=p1.x;
							pt1.y=p1.y;
							pt1.z=z;

							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = (m_pNewFtrs[1]->GetGeometry())->GetDataPoint(0);
							
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if ( GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
								z=(z+temp.z)/2;
								if (K==3)
								{
									pt3.x=pt.x;
									pt3.y=pt.y;
									pt3.z=z;							
								}
								else
								{
									pt3.x=pt2d.x;
									pt3.y=pt2d.y;
									pt3.z=z;
								}
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
								z=temp.z;
								pt2.x=p2.x;
								pt2.y=p2.y;
								pt2.z=z;
								bufb.Arc(&pt1,&pt3,&pt2);
							}
							else
							{
								int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();
								
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(ptnum-1);
								z=(z+temp.z)/2;
								if (K==3)
								{
									pt3.x=pt.x;
									pt3.y=pt.y;
									pt3.z=z;							
								}
								else
								{
									pt3.x=pt2d.x;
									pt3.y=pt2d.y;
									pt3.z=z;
								}
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(ptnum-1);
								z=temp.z;
								pt2.x=p2.x;
								pt2.y=p2.y;
								pt2.z=z;
								bufb.Arc(&pt1,&pt3,&pt2);
							}
						}
						else
						{
							int ptnum1=m_pNewFtrs[0]->GetGeometry()->GetDataPointSum();
							
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(ptnum1-1);
							z=temp.z;
							pt1.x=p1.x;
							pt1.y=p1.y;
							pt1.z=z;

							PT_3DEX tem;
							PT_3D tem1,tem2;
							tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
							COPY_3DPT(tem1,tem);
							tem2.x=m_lineseg[1].ep.x;
							tem2.y=m_lineseg[1].ep.y;
							tem2.z=0;
							if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
							{
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
								z=(z+temp.z)/2;
								if (K==3)
								{
									pt3.x=pt.x;
									pt3.y=pt.y;
									pt3.z=z;							
								}
								else
								{
									pt3.x=pt2d.x;
									pt3.y=pt2d.y;
									pt3.z=z;
								}
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
								z=temp.z;
								pt2.x=p2.x;
								pt2.y=p2.y;
								pt2.z=z;
								bufb.Arc(&pt1,&pt3,&pt2);
							}
							else
							{
								int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();
								
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(ptnum-1);
								z=(z+temp.z)/2;
								if (K==3)
								{
									pt3.x=pt.x;
									pt3.y=pt.y;
									pt3.z=z;							
								}
								else
								{
									pt3.x=pt2d.x;
									pt3.y=pt2d.y;
									pt3.z=z;
								}
								temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(ptnum-1);
								z=temp.z;
								pt2.x=p2.x;
								pt2.y=p2.y;
								pt2.z=z;
								bufb.Arc(&pt1,&pt3,&pt2);
							}

						}


					}
					else
					{
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;
						
						pt1.x=m_lineseg[1].sp.x;
						pt1.y=m_lineseg[1].sp.y;
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(1);						
							pt1.z=temp.z;
						}
						else
						{
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-2);						
							pt1.z=temp.z;
						}						
						buf0.MoveTo(&pt1);
						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						
						pt2.x=m_lineseg[0].sp.x;
						pt2.y=m_lineseg[0].sp.y;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{	
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(1);						
							pt2.z=temp.z;
						}
						else
						{
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(m_pNewFtrs[1]->GetGeometry()->GetDataPointSum()-2);						
							pt2.z=temp.z;
						}
						buf0.LineTo(&pt2);

					}
					if (t1>1)
					{
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[0].ep.x;
						temp2.y=m_lineseg[0].ep.y;
						temp2.z=0;					
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{
							pt3=temp1;
							
						}
						else
						{
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-1);
							pt3.x=m_lineseg[0].ep.x;
							pt3.y=m_lineseg[0].ep.y;
							pt3.z=temp.z;
							
						}		
						buf0.MoveTo(&pt3);
						buf0.LineTo(&pt1);
					}
					
					if(t2>1)
					{
						PT_3DEX temp;
						PT_3D temp1,temp2;
						temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(1);
						COPY_3DPT(temp1,temp);
						temp2.x=m_lineseg[1].ep.x;
						temp2.y=m_lineseg[1].ep.y;
						temp2.z=0;					
						if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
						{
							pt3=temp1;
							
						}
						else
						{
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(m_pNewFtrs[1]->GetGeometry()->GetDataPointSum()-1);
							pt3.x=m_lineseg[1].ep.x;
							pt3.y=m_lineseg[1].ep.y;
							pt3.z=temp.z;
							
						}						
						buf0.MoveTo(&pt3);
						buf0.LineTo(&pt2);
						
					}
					

				}
				if (K==1)
				{
						PT_3DEX temp;
					PT_3D temp1,temp2;
					temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
					double z;
					COPY_3DPT(temp1,temp);
					temp2.x=m_lineseg[0].ep.x;
					temp2.y=m_lineseg[0].ep.y;
					temp2.z=0;
					if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
					{	
						buf0.MoveTo(&temp1);
						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{						
							
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
							z=temp.z;
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;
							buf0.LineTo(&pt1);
							buf0.LineTo(&tem1);


						}
						else
						{
							int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();	
							
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
							z=temp.z;
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(ptnum-1);

							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;

							buf0.LineTo(&pt1);
							buf0.LineTo(&temp);

						}
// 						buf.MoveTo(&pt1);
// 						buf.LineTo(&temp1);
					}
					else
					{
						temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-1);
						buf0.MoveTo(&temp);
						
						PT_3DEX tem;
						PT_3D tem1,tem2;
						tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
						COPY_3DPT(tem1,tem);
						tem2.x=m_lineseg[1].ep.x;
						tem2.y=m_lineseg[1].ep.y;
						tem2.z=0;
						if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
						{								
							int ptnum=(m_pNewFtrs[1])->GetGeometry()->GetDataPointSum();
							
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-1);
							z=temp.z;
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;

 							buf0.LineTo(&pt1);
							buf0.LineTo(&tem1);

						}
						else
						{
							int ptnum=(m_pNewFtrs[1]->GetGeometry())->GetDataPointSum();	
							
							temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-1);
							z=temp.z;
							temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(ptnum-1);
							z=(z+temp.z)/2;
							pt1.x=intersec.x;
							pt1.y=intersec.y;
							pt1.z=z;

 							buf0.LineTo(&pt1);
							buf0.LineTo(&temp);
						}
						tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(m_pNewFtrs[1]->GetGeometry()->GetDataPointSum()-1);
						COPY_3DPT(tem1,tem);

					
					}					
					
					
				}
				if(K==2)
				{
					PT_3DEX temp;
					PT_3D temp1,temp2;
					temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(0);
					COPY_3DPT(temp1,temp);
					temp2.x=m_lineseg[0].ep.x;
					temp2.y=m_lineseg[0].ep.y;
					temp2.z=0;
					
					pt1.x=m_lineseg[1].sp.x;
					pt1.y=m_lineseg[1].sp.y;
					if(GraphAPI::GIsEqual2DPoint(&temp1,&temp2))
					{
						temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(1);						
						pt1.z=temp.z;
					}
					else
					{
						temp = m_pNewFtrs[0]->GetGeometry()->GetDataPoint(m_pNewFtrs[0]->GetGeometry()->GetDataPointSum()-2);						
						pt1.z=temp.z;
					}
					buf0.MoveTo(&pt1);
					
					PT_3DEX tem;
					PT_3D tem1,tem2;
					tem = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(0);
					COPY_3DPT(tem1,tem);
					tem2.x=m_lineseg[1].ep.x;
					tem2.y=m_lineseg[1].ep.y;
					tem2.z=0;
					
					pt2.x=m_lineseg[0].sp.x;
					pt2.y=m_lineseg[0].sp.y;
					if (GraphAPI::GIsEqual2DPoint(&tem1,&tem2))
					{	
						temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(1);						
						pt2.z=temp.z;
					}
					else
					{
						temp = m_pNewFtrs[1]->GetGeometry()->GetDataPoint(m_pNewFtrs[1]->GetGeometry()->GetDataPointSum()-2);						
						pt2.z=temp.z;
					}
					buf0.LineTo(&pt2);
					
				}
				buf.ToGrBuffer(&buf1);
				buf0.End();
				buf1.End();
				buf0.AddBuffer(&buf1);
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf0);		
				
			}
		}		
		return;
		
	}
	
}

void CChamferCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue(PF_CHAMFERWAY,&CVariantEx(var));
	var = (bool)m_bLineserial;
	tab.AddValue(PF_LINESERIAL,&CVariantEx(var));
}

void CChamferCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ChamferCommand",StrFromLocalResID(IDS_CMDNAME_CHAMFER));
	param->BeginOptionParam(PF_CHAMFERWAY,StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_POLYLINE),1,' ',m_nMode==modePolylineChamfer);
	param->AddOption(StrFromResID(IDS_CMDPLANE_ARC),0,' ',m_nMode==modeArcChamfer);	
	param->EndOptionParam();

	if (m_nMode==modeArcChamfer || bForLoad)
	{
		param->AddParam(PF_LINESERIAL,(bool)m_bLineserial,StrFromResID(IDS_CMDPLANE_LINESERIAL));
	}

}


void CChamferCommand::Finish()
{
	m_pNewFtrs[0] = NULL;
	if(m_pNewFtrs[1]) 
	{
		delete m_pNewFtrs[1];
		m_pNewFtrs[1] = NULL;
 	}
	CEditCommand::Finish();
}

void CChamferCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_CHAMFERWAY,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
		{
			Abort();
			return;
		}
	}

	if( tab.GetValue(0,PF_LINESERIAL,var) )
	{
		m_bLineserial = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}


/////////////////////////////////////////////////////////////////////
// CIntZCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIntZCommand,CEditCommand)

CIntZCommand::CIntZCommand()
{
	m_nStep = -1;
	m_bOnlyContour = TRUE;
	m_lfLimitDZ = 0.01;
	m_fUnitZ = 5.0;
	strcat(m_strRegPath,"\\IntZ");
}

CIntZCommand::~CIntZCommand()
{
}

void CIntZCommand::Abort()
{
	CEditCommand::Abort();
}


CString CIntZCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_INTZ);
}

void CIntZCommand::Start()
{
	if( !m_pEditor )return;
	m_bOnlyContour = TRUE;
	m_lfLimitDZ = 0.01;
	m_fUnitZ = 5.0;
	
	CEditCommand::Start();
//	m_pEditor->OpenSelector(SELMODE_NORMAL);
}


void CIntZCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	if( m_fUnitZ==0 )
	{
	//	OutputTipString(StrFromResID(IDS_CMDTIP_INTZ_ERR));
		return;
	}
	
	if( m_nStep==1 )
	{
		CGeometry *pObj;		
	//	CGeometry *pObj1, *pObj2;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor,Name());
		GrBuffer buf;
			
		for( int i = num-1; i>=0; i--)
		{
			pObj = HandleToFtr(handles[i])->GetGeometry();
			
			//生成新的对象，这里需要指定新对象层码
			if( !pObj || pObj->GetDataPointSum()<=0 )continue;

			if( m_bOnlyContour && !CModifyZCommand::CheckObjForContour(pObj) )
				continue;
			
			pObj = pObj->Clone();
			if( !pObj )continue;

			CArray<PT_3DEX,PT_3DEX> arr;
			pObj->GetShape(arr);
			int num0 = arr.GetSize();
			
			double v1, v2;
			for( int j=0; j<num0; j++)
			{
				PT_3DEX &pt1 = arr.ElementAt(j);
				
				v1 = fabs( pt1.z - m_fUnitZ*floor(pt1.z/m_fUnitZ) );
				v2 = fabs( pt1.z - m_fUnitZ*ceil(pt1.z/m_fUnitZ) );
				
				if( v1>v2 )
				{
					if( v2<m_lfLimitDZ )
						pt1.z = m_fUnitZ*ceil(pt1.z/m_fUnitZ);
				}
				else
				{
					if( v1<m_lfLimitDZ )
						pt1.z = m_fUnitZ*floor(pt1.z/m_fUnitZ);
				}
				
			}

			pObj->CreateShape(arr.GetData(),arr.GetSize());

			int nLayerID = m_pEditor->GetFtrLayerIDOfFtr(handles[i]);
			CFeature * pFtr1 = HandleToFtr(handles[i])->Clone();
			if(pFtr1&& pObj )pFtr1->SetGeometry(pObj);

			pFtr1->SetID(OUID());			
			
			if( !m_pEditor->AddObject(pFtr1,nLayerID) )
			{
				delete pFtr1;
				Abort();
				return;
			}	
			
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pFtr1);
			undo.arrNewHandles.Add(FtrToHandle(pFtr1));	
			
			m_pEditor->DeleteObject(handles[i]);
			undo.arrOldHandles.Add(handles[i]);	
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt,flag);
}

void CIntZCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bOnlyContour);
	tab.AddValue(PF_MODIFYZONLYCONTOR,&CVariantEx(var));
	var = (float)(m_fUnitZ);
	tab.AddValue(PF_INTZSTEP,&CVariantEx(var));	
	var = (double)(m_lfLimitDZ);
	tab.AddValue(PF_INTZMAXOFFSET,&CVariantEx(var));	
}

void CIntZCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("IntZCommand",StrFromResID(IDS_CMDNAME_INTZ));	
	param->AddParam(PF_MODIFYZONLYCONTOR,bool(m_bOnlyContour),StrFromResID(IDS_CMDPLANE_ONLYCONTOUR));
	param->AddParam(PF_INTZSTEP,float(m_fUnitZ),StrFromResID(IDS_CMDPLANE_INTZSTEP));
	param->AddParam(PF_INTZMAXOFFSET,double(m_lfLimitDZ),StrFromResID(IDS_CMDPLANE_MAXOFFSET));
		
}


void CIntZCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_MODIFYZONLYCONTOR,var) )
	{
		m_bOnlyContour = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTZSTEP,var) )
	{
		m_fUnitZ = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INTZMAXOFFSET,var) )
	{
		m_lfLimitDZ = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}


/////////////////////////////////////////////////////////////////////
// CConnectCodeChgCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CConnectCodeChgCommand,CEditCommand)

CConnectCodeChgCommand::CConnectCodeChgCommand()
{
	m_nStep = -1;
	m_nConnectCode= 2;
	strcat(m_strRegPath,"\\ConnectCodeChange");
}

CConnectCodeChgCommand::~CConnectCodeChgCommand()
{
}

void CConnectCodeChgCommand::Abort()
{
	CEditCommand::Abort();
}

CString CConnectCodeChgCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CHGCODE);
}

void CConnectCodeChgCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}	
	
	CEditCommand::Start();
}


void CConnectCodeChgCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CGeometry *pObj;		
	//	CGeometry *pObj1, *pObj2;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		CUndoModifyProperties undo(m_pEditor,Name());
		GrBuffer buf;
		PT_3DEX pt0,pt1;
			
		for( int i = num-1; i>=0; i--)
		{
			pObj = HandleToFtr(handles[i])->GetGeometry();
			
			//生成新的对象，这里需要指定新对象层码
			if( !pObj || pObj->GetDataPointSum()<=0 )continue;

			m_pEditor->DeleteObject(handles[i],FALSE);
		
			undo.arrHandles.Add(handles[i]);
			undo.oldVT.BeginAddValueItem();
			HandleToFtr(handles[i])->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
			CArray<PT_3DEX,PT_3DEX> pts;
			pObj->GetShape(pts);
			int num0 = pts.GetSize();
			for (int j=0;j<num0;j++)
			{
				pt0 = pts.GetAt(j);
				pt0.pencode = m_nConnectCode;
				pts.SetAt(j,pt0);
			}
			pObj->CreateShape(pts.GetData(),pts.GetSize());
			
			//if(pFtr1&& pObj )pFtr1->SetGeometry(pObj);
// 			if( !m_pEditor->UpdateObject(handles[i]) )
// 			{				
// 				Abort();
// 				return;
// 			}

			m_pEditor->RestoreObject(handles[i]);

			undo.newVT.BeginAddValueItem();
			HandleToFtr(handles[i])->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();
	//		undo.arrNewHandles.Add(FtrToHandle(pFtr1));			
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt,flag);
}

void CConnectCodeChgCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;	
	var = (long)(m_nConnectCode);
	tab.AddValue(PF_CHANGECODE,&CVariantEx(var));	
}

void CConnectCodeChgCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ConnectCodeChg",StrFromLocalResID(IDS_CMDNAME_CHGCODE));
	param->AddLineTypeParam(PF_CHANGECODE,m_nConnectCode,StrFromResID(IDS_CMDPLANE_LINETYPE));
}


void CConnectCodeChgCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_CHANGECODE,var) )
	{
		m_nConnectCode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	CEditCommand::SetParams(tab,bInit);
}

/////////////////////////////////////////////////////////////////////
// CCompressCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCompressCommand,CEditCommand)

CCompressCommand::CCompressCommand()
{
	m_lfLimit = 0.1;
	m_lfAngLimit = 0;	
	m_nPart = modeWhole;
	strcat(m_strRegPath,"\\Compress");
}

CCompressCommand::~CCompressCommand()
{
}

void CCompressCommand::Abort()
{
	CEditCommand::Abort();
}


CString CCompressCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COMPRESS);
}

void CCompressCommand::Start()
{
	if( !m_pEditor )return;

	m_nStep = 0;
	
	m_nPart = modeWhole;
	
	CCommand::Start();
	
	if( m_nPart==modeWhole )
	{
		int numsel;	
		m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
		if(numsel>0 )
		{
			CCommand::Start();
			
			PT_3D pt;
			PtClick(pt,0);		
			m_nExitCode = CMPEC_STARTOLD;
			return;		
		}
		
		m_pEditor->OpenSelector();
	}
	else
	{
		m_pEditor->CloseSelector();
	}

	//CEditCommand::Start();
}

CGeometry* CCompressCommand::CompressSimpleCore(CGeometry *pObj)
{	
	if(!pObj)
		return NULL;

	if(pObj->GetDataPointSum()<=2)
		return NULL;
	
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
	{
		CArray<PT_3DEX,PT_3DEX> arr;
		pObj->GetShape(arr);

		int num = arr.GetSize();
		if( num<=2 )return NULL;		

		//判断是否全为线串码
		PT_3DEX expt;
		for( int j=1; j<num; j++)
		{
			expt = arr.GetAt(j);
			if( expt.pencode!=penLine && expt.pencode!=penStream && expt.pencode!=penSpline )
				break;
		}
		
		if( j<num )
		{
			return NULL;
		}
		CGeometry *pNewObj = pObj->Clone();
		CArray<PT_3DEX,PT_3DEX> pts;
		
		//弦高压缩
		PT_3D pt3d;
		
		CStreamCompress2_PT_3D comp;
		comp.BeginCompress(m_lfLimit);

		PT_3D pt0 = arr[0];
		
		for( j=0; j<num; j++)
		{
			COPY_3DPT(pt3d,arr[j]);
			pt3d.x -= pt0.x;
			pt3d.y -= pt0.y;
			pt3d.z -= pt0.z;
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
		
		//角度压缩
		double anglmt = m_lfAngLimit/180;
		anglmt = anglmt-(int)anglmt;
		if( anglmt>0 )
		{
			anglmt = anglmt*PI;
			num = pts.GetSize();
			PT_3DEX expt1,expt2;
			CArray<int, int> flags;
			double curAng;
			
			flags.SetSize(num);
			flags.SetAt(0,0);
			flags.SetAt(num-1,0);
			expt = pts.GetAt(0);
			expt1 = pts.GetAt(1);
			for( j=2; j<num; j++)
			{
				expt2 = pts.GetAt(j);
				curAng = GraphAPI::GGetAngle(expt1.x,expt1.y,expt2.x,expt2.y)-GraphAPI::GGetAngle(expt1.x,expt1.y,expt.x,expt.y);
				if( curAng<-PI )curAng += 2*PI;
				else if( curAng>PI )curAng -= 2*PI;
				if( curAng<0 )curAng = -curAng;
				if( curAng<anglmt )
				{
					flags.SetAt(j-1,1);
				}
				else flags.SetAt(j-1,0);
				expt = expt1;  expt1 = expt2;
			}
			
			for( j=num-1; j>=0; j--)
			{
				if( flags[j] )
				{
					pts.RemoveAt(j);
				}
			}
		}
		
		//添加结果
		if( pts.GetSize()>=2 )
		{
			num = pts.GetSize();
			for( j=0; j<num; j++)
			{
				COPY_3DPT(pt3d,pts[j]);
				pt3d.x += pt0.x;
				pt3d.y += pt0.y;
				pt3d.z += pt0.z;
				COPY_3DPT(pts[j],pt3d);
			}
			if(pNewObj->CreateShape(pts.GetData(),pts.GetSize()))
			   return pNewObj;
		}
		else
			delete pNewObj;

	}

	return NULL;

}


CGeometry* CCompressCommand::CompressSimple(CGeometry *pObj)
{	
	if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
	{
		CGeometry *pNew = CCompressCommand::CompressSimpleCore(pObj);
		if(pNew && ((CGeoCurveBase*)pNew)->IsClosed())
		{
			CArray<PT_3DEX,PT_3DEX> arr;
			pNew->GetShape(arr);	
				
			int num = arr.GetSize();
			if( num<5 )return pNew;
			
			PT_3DEX test[3];
			test[0] = arr[num-2];
			test[1] = arr[num-1];
			test[2] = arr[1];

			CGeoCurve test_obj;
			test_obj.CreateShape(test,3);
			CGeometry *pTest = CCompressCommand::CompressSimpleCore(&test_obj);
			if(pTest)
			{
				int npt_test = pTest->GetDataPointSum();
				delete pTest;
				if( npt_test==2 )
				{
					arr.RemoveAt(num-1);
					arr[0] = arr[num-2];
					arr[0].pencode = penLine;
					pNew->CreateShape(arr.GetData(),arr.GetSize());
				}
				return pNew;
			}
		}
		else
		{
			return pNew;
		}
	}

	return NULL;

}

CGeometry* CCompressCommand::Compress(CGeometry *pObj)
{
	if(!pObj)
		return NULL;

	if(pObj->GetDataPointSum()<=2)
		return NULL;

	int nGeoClass = pObj->GetClassType();
	if( nGeoClass==CLS_GEOCURVE || nGeoClass==CLS_GEOPARALLEL || nGeoClass==CLS_GEOSURFACE )
	{
		return CompressSimple(pObj);
	}
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
	{
		CArray<PT_3DEX,PT_3DEX> arr, arr1, arr2;
		pObj->GetShape(arr);

		int npt = arr.GetSize();
		arr1.Add(arr[0]);

		for(int i=1; i<npt; i++)
		{
			if(arr[i].pencode==penMove || i==(npt-1))
			{
				if(i==(npt-1))
					arr1.Add(arr[i]);

				if(arr1.GetSize()>2)
				{
					CGeoCurve new_obj;
					new_obj.CreateShape(arr1.GetData(),arr1.GetSize());

					CGeometry *pNew = CompressSimple(&new_obj);
					if(pNew)
					{
						arr1.RemoveAll();
						pNew->GetShape(arr1);
						if(arr1.GetSize()>0)arr1[0].pencode = penMove;
						arr2.Append(arr1);
						delete pNew;
					}
					else
					{
						arr2.Append(arr1);
					}
				}
				arr1.RemoveAll();
				arr1.Add(arr[i]);
			}
			else
			{
				arr1.Add(arr[i]);
			}
		}

		if(arr2.GetSize()>2)
		{
			pObj = pObj->Clone();
			pObj->CreateShape(arr2.GetData(),arr2.GetSize());
			return pObj;
		}
	}

	return NULL;
}


CGeometry *GetPartInPolygon(CGeometry *pObj, PT_3D *pts, int npt, int &start, int &end)
{
	if (!pObj) return NULL;
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pObj->GetShape(arrPts);
	int nsum = arrPts.GetSize();
	if( nsum<=2 )return NULL;
	
	start = -1; end = -1;
	
	PT_3DEX expt;
	
	for( int i=0; i<nsum; i++)
	{
		expt = arrPts[i];
		
		if( start==-1 )
		{
			if( GraphAPI::GIsPtInRegion(PT_3D(expt),pts,npt)==2 )
			{
				start = i;
			}
		}
		else
		{
			if (GraphAPI::GIsPtInRegion(PT_3D(expt),pts,npt) != 2)
			{
				end = i;
				break;
			}
		}
	}
	
	if( start<0 )return NULL;
	if( start>0 )start--;
	if( end<0 )end = nsum-1;
	else if( end==(nsum-1) );
	else end++;
	
	if( (end-start)<=1)return NULL;
	
	CGeometry *pPart = new CGeoCurve;
	if( !pPart )return NULL;
	CArray<PT_3DEX,PT_3DEX> arr;
	for( i=start; i<end; i++)
	{
		arr.Add(arrPts[i]);		
	}
	pPart->CreateShape(arr.GetData(),arr.GetSize());
	return pPart;
}

void CCompressCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==1 && m_nPart==modePart )
	{
		//计算范围四边形
		PT_4D pts0[2],pts[4];
		pts0[0] = PT_4D(m_ptDragStart);
		pts0[1] = PT_4D(pt);
		
		m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(pts0,2);
		
		pts[0].x = pts0[0].x; pts[0].y = pts0[0].y; pts[0].z = pts0[0].x; pts[0].yr = pts0[0].y;
		pts[1].x = pts0[0].x; pts[1].y = pts0[1].y; pts[1].z = pts0[0].x; pts[1].yr = pts0[1].y;
		pts[2].x = pts0[1].x; pts[2].y = pts0[1].y; pts[2].z = pts0[1].x; pts[2].yr = pts0[1].y;
		pts[3].x = pts0[1].x; pts[3].y = pts0[0].y; pts[3].z = pts0[1].x; pts[3].yr = pts0[0].y;
		
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pts,4);
		
		PT_3D pt3ds[4];
		for( int i=0; i<4; i++)pt3ds[i] = pts[i].To3D();
		
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(pt3ds);
		buf.LineTo(pt3ds+1);
		buf.LineTo(pt3ds+2);
		buf.LineTo(pt3ds+3);
		buf.LineTo(pt3ds);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
	
	CEditCommand::PtMove(pt);

}	

void CCompressCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nPart==modeWhole )
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag) )return;
			EditStepOne();
		}
		
		if( m_nStep==1 )
		{
			CGeometry *pObj;
			CUndoFtrs undo(m_pEditor,Name());
			
			int num;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);

			GProgressStart(num);

			for( int i=num-1; i>=0; i--)
			{
				GProgressStep();

				pObj = HandleToFtr(handles[i])->GetGeometry();
				
				if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
				{
					CGeometry *pNewObj = Compress(pObj);
					
					//添加结果
					if( pNewObj!=NULL )
					{
								
						//m_idsOld.Add(idsOld[i]);
						CFeature *pFtr = HandleToFtr(handles[i])->Clone();
						if (pFtr)
						{
							pFtr->SetID(OUID());
							pFtr->SetGeometry(pNewObj);
							m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(handles[i]));

							GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pFtr);
							undo.AddNewFeature(FtrToHandle(pFtr));	

							m_pEditor->DeleteObject(handles[i]);
							undo.AddOldFeature(handles[i]);	
						}	
						//m_idsNew.Add(m_pDoc->AddObject(pNewObj,idsOld[i].layhdl));
					}
				}
			}

			GProgressEnd();

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			undo.Commit();
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 2;
		}
	}
	else
	{
		if( m_nStep==0 )
		{
			m_ptDragStart = pt;
			m_nStep = 1;
		}
		else if( m_nStep==1 )
		{
			//计算范围四边形
			PT_4D pts0[2],pts[4];
			pts0[0] = PT_4D(m_ptDragStart);
			pts0[1] = PT_4D(pt);
			
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(pts0,2);

			pts[0].x = pts0[0].x; pts[0].y = pts0[0].y; pts[0].z = pts0[0].x; pts[0].yr = pts0[0].y;
			pts[1].x = pts0[0].x; pts[1].y = pts0[1].y; pts[1].z = pts0[0].x; pts[1].yr = pts0[1].y;
			pts[2].x = pts0[1].x; pts[2].y = pts0[1].y; pts[2].z = pts0[1].x; pts[2].yr = pts0[1].y;
			pts[3].x = pts0[1].x; pts[3].y = pts0[0].y; pts[3].z = pts0[1].x; pts[3].yr = pts0[0].y;
			
			//m_pEditor->GetCoordWnd().m_pSearchCS->ClientToGround(pts,4);
			
			PT_3D pt3ds[4],pt3ds1[4];
			for( int i=0; i<4; i++)
			{
				pt3ds[i] = pts[i].To3D();
				m_pEditor->GetCoordWnd().m_pSearchCS->ClientToGround(&pts[i],&pt3ds1[i]);
			}
			Envelope e;
			e.CreateFromPts(pt3ds,4);

			//获得范围内的地物
			m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);

			int num;
			const CPFeature * ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);
			if( num>0 )
			{
				CGeometry *pObj;
				CUndoFtrs undo(m_pEditor,Name());
				
				GProgressStart(num);

				for(int i=0; i<num; i++)
				{
					GProgressStep();

					pObj = ftr[i]->GetGeometry();
				
					//依次遍历处理
					if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
					{
						int j, start, end;
						PT_3DEX expt;

						//取出地物上落在范围内的顶点部分
						CGeometry *pPart1 = NULL, *pPart2 = NULL;
						pPart1 = GetPartInPolygon(pObj,pt3ds1,4,start,end);

						if( pPart1 )
						{
							//压缩这部分数据
							pPart2 = Compress(pPart1);
							if( pPart2 )
							{
								//生成新地物
								CGeometry *pNewObj = pObj->Clone();								
								CArray<PT_3DEX,PT_3DEX> pts, oldpts, part2pts;
								pObj->GetShape(oldpts);
								pPart2->GetShape(part2pts);

								//1、取压缩段之前的顶点
								for( j=0; j<start; j++)
								{
									pts.Add(oldpts[j]);									
								}								
								//2、取压缩段的顶点
								for( j=0; j<part2pts.GetSize(); j++)
								{
									pts.Add(part2pts[j]);									
								}
								//3、取压缩段之后的顶点
								for( j=end+1; j<pObj->GetDataPointSum(); j++)
								{
									pts.Add(oldpts[j]);									
								}
								pNewObj->CreateShape(pts.GetData(),pts.GetSize());
								
								//添加地物								
								CFeature *pFtr = ftr[i]->Clone();
								if(pFtr)
								{
									pFtr->SetID(OUID());
									pFtr->SetGeometry(pNewObj);
									m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(ftr[i])));

									GETXDS(m_pEditor)->CopyXAttributes(ftr[i],pFtr);
									undo.arrNewHandles.Add(FtrToHandle(pFtr));

									m_pEditor->DeleteObject(FtrToHandle(ftr[i]));
								    undo.arrOldHandles.Add(FtrToHandle(ftr[i]));
								}
								
							}
						}

						if( pPart1 )delete pPart1;
						if( pPart2 )delete pPart2;
					}
				}
				undo.Commit();	
				
				GProgressEnd();
			}
		
			m_pEditor->RefreshView();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,0);
			Finish();
			m_nStep = 2;
		}
	}
	CEditCommand::PtClick(pt,flag);
}

void CCompressCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;	
	var = (double)(m_lfLimit);
	tab.AddValue(PF_COMTOL,&CVariantEx(var));
	var = (double)(m_lfAngLimit);
	tab.AddValue(PF_COMMINANGLE,&CVariantEx(var));	
	var = (short)(m_nPart);
	tab.AddValue(PF_COMWAY,&CVariantEx(var));	
}

void CCompressCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CompressCommand",StrFromLocalResID(IDS_CMDNAME_COMPRESS));	
	param->AddParam(PF_COMTOL,double(m_lfLimit),StrFromResID(IDS_CMDPLANE_TOLER));
	param->AddParam(PF_COMMINANGLE,double(m_lfAngLimit),StrFromResID(IDS_CMDPLANE_MINANGLE),StrFromResID(IDS_CMDTIP_MINANGLE));
	param->BeginOptionParam(PF_COMWAY,StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_WHOLE),modeWhole,' ',m_nPart==modeWhole);
	param->AddOption(StrFromResID(IDS_CMDPLANE_PART),modePart,' ',m_nPart==modePart);
	param->EndOptionParam();
}


void CCompressCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_COMTOL,var) )
	{
		m_lfLimit = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_COMMINANGLE,var) )
	{
		m_lfAngLimit = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_COMWAY,var) )
	{
		m_nPart = (short)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (m_nPart==modePart)
	{
		m_pEditor->CloseSelector();
	}
	else
		m_pEditor->OpenSelector();
	CEditCommand::SetParams(tab,bInit);
}



/////////////////////////////////////////////////////////////////////
// CCompressRedundantCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCompressRedundantCommand,CCompressCommand)

CCompressRedundantCommand::CCompressRedundantCommand()
{
	m_lfLimit = 0.00001;
	m_lfAngLimit = 0;	
	m_lfAngLimit = 0.0;
	m_nPart = modeWhole;
}

CCompressRedundantCommand::~CCompressRedundantCommand()
{
}


CString CCompressRedundantCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COMPRESS_REDUNDANT);
}

void CCompressRedundantCommand::Start()
{
	if( !m_pEditor )return;

	m_nStep = 0;
	m_lfLimit = 0.00001;
	m_lfAngLimit = 0.0;
	m_nPart = modeWhole;
	
	CCommand::Start();

	m_nPart = modeWhole;
	m_lfLimit = 0.00001;
	m_lfAngLimit = 0.0;

	if( m_nPart==modeWhole )
	{
		int numsel;	
		m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
		if(numsel>0 )
		{
			CCommand::Start();
			
			PT_3D pt;
			PtClick(pt,0);		
			m_nExitCode = CMPEC_STARTOLD;
			return;		
		}
		
		m_pEditor->OpenSelector();
	}
	else
	{
		m_pEditor->CloseSelector();
	}
}

CGeometry* CCompressRedundantCommand::CompressSimple(CGeometry *pObj)
{	
	if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
	{
		CGeometry *pNew = CCompressCommand::CompressSimpleCore(pObj);
		if(pNew && ((CGeoCurveBase*)pNew)->IsClosed())
		{
			CArray<PT_3DEX,PT_3DEX> arr;
			pNew->GetShape(arr);	
				
			int num = arr.GetSize();
			if( num<5 )return pNew;
			
			PT_3DEX test[3];
			test[0] = arr[num-2];
			test[1] = arr[num-1];
			test[2] = arr[1];

			CGeoCurve test_obj;
			test_obj.CreateShape(test,3);
			CGeometry *pTest = CCompressCommand::CompressSimpleCore(&test_obj);
			if(pTest)
			{
				int npt_test = pTest->GetDataPointSum();
				delete pTest;
				if( npt_test==2 )
				{
					arr.RemoveAt(num-1);
					arr[0] = arr[num-2];
					arr[0].pencode = penLine;
					pNew->CreateShape(arr.GetData(),arr.GetSize());
					return pNew;
				}
				else
				{
					return pNew;
				}
			}
		}
		else
		{
			return pNew;
		}
	}

	return NULL;

}


void CCompressRedundantCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
}

void CCompressRedundantCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(AccelStr(),Name());	
}


void CCompressRedundantCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	CEditCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CCloseCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCloseCommand,CEditCommand)

CCloseCommand::CCloseCommand()
{
}

CCloseCommand::~CCloseCommand()
{
}

CString CCloseCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CLOSE);
}

void CCloseCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		//PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	m_bClose = FALSE;
	CEditCommand::Start();
}

void CCloseCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();		
	}
	
	if( m_nStep==1 )
	{

		CGeometry *pObj;
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor,Name());
		//UndoModifyProperties
		for( int i=num-1; i>=0; i--)
		{
			pObj = HandleToFtr(handles[i])->GetGeometry();
			if( !pObj )continue;
			
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			{
				if( pObj->GetDataPointSum()<=2 )continue;
				
				if(!((CGeoCurve*)pObj)->IsClosed())
				{
					m_bClose = TRUE;
					break;
				}
				m_bClose = FALSE;
			}
		}
	/*	if (!m_bClose)*/
		{
			for( i=num-1; i>=0; i--)
			{
				pObj = HandleToFtr(handles[i])->GetGeometry();
				if( !pObj )continue;
				
				if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				{
					if( pObj->GetDataPointSum()<=2 ||
						(((CGeoCurveBase*)pObj)->IsClosed()&&m_bClose)||
						(!((CGeoCurveBase*)pObj)->IsClosed()&&!m_bClose))
					{					
						continue;
					}			
									
					CFeature *pFtr = HandleToFtr(handles[i])->Clone();
					if (pFtr)
					{
						pFtr->SetID(OUID());
						
						((CGeoCurveBase*)pFtr->GetGeometry())->EnableClose(m_bClose);

						m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(handles[i]));

						GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pFtr);
						undo.arrNewHandles.Add(FtrToHandle(pFtr));	
						
						undo.arrOldHandles.Add(handles[i]);
						m_pEditor->DeleteObject(handles[i]);
					}
					
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
	
	CCommand::PtClick(pt, flag);
}


void CCloseCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}


//////////////////////////////////////////////////////////////////////
// CCloseCommandWithTole Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCloseCommandWithTole,CEditCommand)

CCloseCommandWithTole::CCloseCommandWithTole()
{
	m_lfTorlerance = 0;
	strcat(m_strRegPath,"\\CloseWithTole");
}

CCloseCommandWithTole::~CCloseCommandWithTole()
{
}

CString CCloseCommandWithTole::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CLOSEWITHTOLE);
}

void CCloseCommandWithTole::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);		
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
//	m_bClose = FALSE;
	CEditCommand::Start();
}

void CCloseCommandWithTole::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();		
	}
	
	if( m_nStep==1 )
	{
		CGeometry *pObj;
		double lim = 0;
		if( m_lfTorlerance>=1e-4 )lim = m_lfTorlerance*m_lfTorlerance;
		CSelection* pSel = m_pEditor->GetSelection();

	
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		CUndoModifyProperties undo(m_pEditor,Name());
		
		for( int i=num-1; i>=0; i--)
		{
			pObj = HandleToFtr(handles[i])->GetGeometry();
			
			if( pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && !((CGeoCurveBase*)pObj)->IsClosed())
			{
				const CShapeLine *pShape = pObj->GetShape();
				if (!pShape) continue;

				CArray<PT_3DEX,PT_3DEX> pts;
				pShape->GetPts(pts);

				int num = pts.GetSize();
				if( num<=2 )continue;
				
				//判断是否全为线串码
				PT_3DEX expt1, expt2;
				expt1 = pts[0];
				expt2 = pts[num-1];
				double dis = (expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y)+(expt1.z-expt2.z)*(expt1.z-expt2.z);
				if( dis>=lim )continue;

				m_pEditor->DeleteObject(handles[i],FALSE);
				
				undo.arrHandles.Add(handles[i]);
				undo.oldVT.BeginAddValueItem();
				HandleToFtr(handles[i])->WriteTo(undo.oldVT);
				undo.newVT.EndAddValueItem();
				
				//添加结果
				((CGeoCurveBase*)pObj)->EnableClose(TRUE);
				//m_pEditor->UpdateObject(handles[i]);
				m_pEditor->RestoreObject(handles[i]);
				
				undo.newVT.BeginAddValueItem();
				HandleToFtr(handles[i])->WriteTo(undo.newVT);
				undo.newVT.EndAddValueItem();
			}
		}

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged(TRUE);
		undo.Commit();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;	
	}	
	CCommand::PtClick(pt, flag);
}


void CCloseCommandWithTole::Finish()
{	
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
}

void CCloseCommandWithTole::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;	
	var = (long)(m_lfTorlerance);
	tab.AddValue(PF_CLOSETOL,&CVariantEx(var));	
}

void CCloseCommandWithTole::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CloseCommandWithTole",StrFromLocalResID(IDS_CMDNAME_CLOSEWITHTOLE));	
	param->AddParam(PF_CLOSETOL,double(m_lfTorlerance),StrFromResID(IDS_CMDPLANE_CLOSETOLER));
}


void CCloseCommandWithTole::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_CLOSETOL,var) )
	{
		m_lfTorlerance = (double )(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	CEditCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CInterpolatePointZCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CInterpolatePointZCommand,CEditCommand)

CInterpolatePointZCommand::CInterpolatePointZCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\InterpolatePointZ");
}

CInterpolatePointZCommand::~CInterpolatePointZCommand()
{
}

void CInterpolatePointZCommand::Abort()
{	
	CEditCommand::Abort();
}

CString CInterpolatePointZCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_INTERPOLATEPOINTZ);
}

void CInterpolatePointZCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	GOutPut(StrFromResID(IDS_CMDTIP_SELFIRSTCONTOUR));

	m_pEditor->OpenSelector(SELMODE_SINGLE);
}

void CInterpolatePointZCommand::PtMove(PT_3D &pt)
{
	CEditCommand::PtMove(pt);
}

void CInterpolatePointZCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{	
		int nsel;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		if( nsel!=1 )
			return;		
		
		CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
		
		//不是等高线
		if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || !CModifyZCommand::CheckObjForContour(pObj) )
		{
			GOutPut(StrFromResID(IDS_CMDTIP_NOTCONTOUR));
			return;
		}

		m_objHandles[0] = handles[0];
		
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);	
		
		GOutPut(StrFromResID(IDS_CMDTIP_SELSEDCONTOUR));
	}
	
	else if( m_nStep==1 )
	{
		int nsel;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		if( nsel!=1 )
			return;	

		if (m_objHandles[0]==handles[0])
		{
			return;
		}

		CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
		
		//不是等高线
		if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || !CModifyZCommand::CheckObjForContour(pObj) )
		{
			GOutPut(StrFromResID(IDS_CMDTIP_NOTCONTOUR));
			return;
		}

		m_objHandles[1] = handles[0];
				
		m_pEditor->CloseSelector();
		m_nStep = 2;
		GotoState(PROCSTATE_PROCESSING);

		GOutPut(StrFromResID(IDS_CMDTIP_PLACEPT));
	}
	else if(m_nStep == 2)
	{
		// 根据点到两条等高线的距离的比例生成高程点
		CArray<PT_3DEX,PT_3DEX> pts1, pts2;
		const CShapeLine *pShape1 = HandleToFtr(m_objHandles[0])->GetGeometry()->GetShape();
		pShape1->GetPts(pts1);

		const CShapeLine *pShape2 = HandleToFtr(m_objHandles[1])->GetGeometry()->GetShape();
		pShape2->GetPts(pts2);

		double z, dis1, dis2;
		PT_3DEX p(pt,penMove), ret;
		dis1 = GraphAPI::GGetNearstDisPToPointSet2D(pts1.GetData(),pts1.GetSize(),p,ret,NULL);
		dis2 = GraphAPI::GGetNearstDisPToPointSet2D(pts2.GetData(),pts2.GetSize(),p,ret,NULL);

		if (dis1 <= 0)
		{
			z = pts1[0].z;
		}
		else if (dis2 <= 0)
		{
			z = pts2[0].z;
		}
		else 
		{
			// 内插高程
			z = pts1[0].z + dis1/(dis1+dis2)*(pts2[0].z-pts1[0].z);
		}

		ret.x = pt.x;
		ret.y = pt.y;
		ret.z = z;

		CString strLayerName = m_strLayerCode;
		if (strLayerName.IsEmpty())
		{
			strLayerName = StrFromResID(IDS_DEFLAYER_NAMEP);
		}

		CFtrLayer *pFtrLayer = GETDS(m_pEditor)->GetFtrLayer(strLayerName);
		if (pFtrLayer == NULL)
		{
			Abort();
			return;
		}

		CFeature *pNewFtr = pFtrLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),CLS_GEOPOINT);
		if (!pNewFtr)
		{
			Abort();
			return;
		}

		if (!pNewFtr->GetGeometry()->CreateShape(&PT_3DEX(ret,penLine),1))
		{			
			Abort();
			delete pNewFtr;
			return;
		}

		m_pEditor->AddObject(pNewFtr,pFtrLayer->GetID());

		CUndoFtrs undo(m_pEditor,Name());
		undo.AddNewFeature(FtrToHandle(pNewFtr));
		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		m_nStep = 4;
		Finish();
	}
	CCommand::PtClick(pt, flag);
}


void CInterpolatePointZCommand::Finish()
{	
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();	
}

void CInterpolatePointZCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;	

	var = (_bstr_t)(m_strLayerCode);
	tab.AddValue(PF_CONTOURCODE,&CVariantEx(var));
}

void CInterpolatePointZCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("IntersectPointZ",StrFromLocalResID(IDS_CMDNAME_INTERPOLATEPOINTZ));
	param->AddLayerNameParam(PF_CONTOURCODE,LPCTSTR(m_strLayerCode),StrFromResID(IDS_CMDPLANE_POINTZLAYER),NULL,LAYERPARAMITEM_POINT);
}


void CInterpolatePointZCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_CONTOURCODE,var) )
	{
		m_strLayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	CEditCommand::SetParams(tab,bInit);
}




//////////////////////////////////////////////////////////////////////
// CInterpolatePointZCommand_NoSelect Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CInterpolatePointZCommand_NoSelect,CEditCommand)

CInterpolatePointZCommand_NoSelect::CInterpolatePointZCommand_NoSelect()
{
	m_nStep = -1;
}

CInterpolatePointZCommand_NoSelect::~CInterpolatePointZCommand_NoSelect()
{
}

void CInterpolatePointZCommand_NoSelect::Abort()
{	
	CEditCommand::Abort();
}

CString CInterpolatePointZCommand_NoSelect::Name()
{ 
	return StrFromResID(ID_CMDNAME_INTERPOLATEPOINTZ_NOSELECT);
}

void CInterpolatePointZCommand_NoSelect::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();
}

void CInterpolatePointZCommand_NoSelect::PtMove(PT_3D &pt)
{
	CEditCommand::PtMove(pt);
}


BOOL CInterpolatePointZCommand_NoSelect::Do(PT_3D pt)
{	
	CFtrLayer *pFtrLayer0 = GETDS(m_pEditor)->GetFtrLayer(m_strLayerPt);

	CFtrLayerArray arrContourLayers;
	GETDS(m_pEditor)->GetFtrLayersByNameOrCode(m_strLayerContour,arrContourLayers);
	if (pFtrLayer0==NULL || m_strLayerContour.IsEmpty() )
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		Abort();
		return FALSE;
	}

	PT_3DEX expt;
	PT_3D pt3d[8];
	Envelope e;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds1, arrIds2;
	
	expt = PT_3DEX(pt,penLine);

	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	pDS->SaveAllQueryFlags(TRUE,FALSE);
	for(int i=0; i<arrContourLayers.GetSize(); i++)
	{
		((CFtrLayer*)arrContourLayers[i])->SetAllowQuery(TRUE);
	}
	
	double r;
	int  nObj = m_pEditor->GetDataQuery()->FindNearestObjectByObjNum(expt,10,NULL,FALSE,&r,NULL);
	pDS->RestoreAllQueryFlags();

	if( nObj<2 )
	{
		GOutPut(StrFromResID(ID_ERR_NOTFIND_NEARCONTOURS));
		return FALSE;
	}

	double dx,dy;
	const CPFeature *ftr = m_pEditor->GetDataQuery()->GetFoundHandles(nObj);
	nObj = (nObj<=10?nObj:10);
	
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

			CShapeLine::ShapeLineUnit *shapeUnit = NULL;
			Envelope evlp,evlp2;
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

				shapeUnit = shape->HeadUnit();
				while( shapeUnit )
				{
					if (evlp.bIntersect(&shapeUnit->evlp))
					{
						PT_3DEX *pts = shapeUnit->pts;
						for( int k=0; k<shapeUnit->nuse-1; k++,pts++)
						{							
							COPY_3DPT(pts3d[0],pts[0]);
							COPY_3DPT(pts3d[1],pts[1]);
							evlp2.CreateFromPts(pts3d,2,sizeof(PT_3D));
							if( !evlp.bIntersect(&evlp2) )continue;
							
							if( (fabs(pt0.x-pts[0].x)<1e-4&&fabs(pt0.y-pts[0].y)<1e-4)||
								(fabs(pt1.x-pts[0].x)<1e-4&&fabs(pt1.y-pts[0].y)<1e-4)||
								(fabs(pt0.x-pts[1].x)<1e-4&&fabs(pt0.y-pts[1].y)<1e-4)||
								(fabs(pt1.x-pts[1].x)<1e-4&&fabs(pt1.y-pts[1].y)<1e-4) )
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
	for( i=0; i<nfind1; i++)
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
		expt.z = arrZ[i-1] + (arrZ[i]-arrZ[i-1]) * (0.5-arrPos[i-1])/(arrPos[i]-arrPos[i-1]);
	}
	//山谷或者山顶
	else
	{
		if( 0 && i>=2 && fabs(arrZ[i-1]-arrZ[i])>1e-4 )
		{
			expt.z = arrZ[i] + (arrZ[i-1]-arrZ[i]) * (0.5-arrPos[i])/(arrPos[i-1]-arrPos[i]);
		}
		else if( 0 && i>=0 && i<nsz-1 && fabs(arrZ[i+1]-arrZ[i])>1e-4 )
		{
			expt.z = arrZ[i] + (arrZ[i+1]-arrZ[i]) * (0.5-arrPos[i])/(arrPos[i+1]-arrPos[i]);
		}
		else 
		{
			GOutPut(StrFromResID(ID_ERR_FAILED_INVALID_RESULT));
			return FALSE;
		}
	}

	
	CFeature *pNewFtr = pFtrLayer0->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),CLS_GEOPOINT);
	if (!pNewFtr)
	{
		Abort();
		return FALSE;
	}
	
	pNewFtr->GetGeometry()->CreateShape(&expt,1);
	
	m_pEditor->AddObject(pNewFtr,pFtrLayer0->GetID());
	
	CUndoFtrs undo(m_pEditor,Name());
	undo.AddNewFeature(FtrToHandle(pNewFtr));
	undo.Commit();

	m_nStep = 4;
	Finish();

	return TRUE;

}

extern void KickoffSameGeoPts(CArray<MYPT_3D,MYPT_3D>& arr);

BOOL CInterpolatePointZCommand_NoSelect::Do_TIN(PT_3D pt)
{
	CFtrLayer *pFtrLayer0 = GETDS(m_pEditor)->GetFtrLayer(m_strLayerPt);

	if (pFtrLayer0==NULL )
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		Abort();
		return FALSE;
	}

	PT_3DEX expt;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds1, arrIds2;
	
	expt = PT_3DEX(pt,penLine);

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	
	double r;
	int  nObj = m_pEditor->GetDataQuery()->FindNearestObjectByObjNum(expt,20,NULL,FALSE,&r,NULL);

	if( nObj<2 )
	{
		GOutPut(StrFromResID(ID_ERR_NOTFIND_NEARCONTOURS));
		return FALSE;
	}

	const CPFeature *ppFtr = m_pEditor->GetDataQuery()->GetFoundHandles(nObj);

	CPtrArray arrObjs;
	CPtrArray arrAllObjs;

	PT_3D polyPts[5];

	polyPts[0].x = pt.x-r; polyPts[0].y = pt.y-r;
	polyPts[1].x = pt.x+r; polyPts[1].y = pt.y-r;
	polyPts[2].x = pt.x+r; polyPts[2].y = pt.y+r;
	polyPts[3].x = pt.x-r; polyPts[3].y = pt.y+r;
	polyPts[4] = polyPts[0];

	int nPt = 5;
	CGrTrim trim;
	trim.InitTrimPolygon(polyPts,nPt,r*0.5);

	Envelope e = CreateEnvelopeFromPts(polyPts,5);
	
	//过滤对象
	for(int i=0; i<nObj; i++)
	{
		CFeature *pFtr = ppFtr[i];
		if (!pFtr) continue;
		CGeometry *pObj = pFtr->GetGeometry();
		if( !pObj )continue;

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			continue;
		}
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
		{
			continue;
		}

		CPtrArray arrObjs;
		CBatExportMapsCommand::TrimObj(pObj, &arrObjs, e, polyPts, nPt, &trim, FALSE, TRUE);
		arrAllObjs.Append(arrObjs);
	}

	int k = 0, npt;
	nObj = arrAllObjs.GetSize();

	CArray<MYPT_3D,MYPT_3D> arrAll, arrCur;
	MYPT_3D geopt;
	MYPT_3D tri[3];

	for( int j=0; j<nObj; j++)
	{
		CGeometry *pGeo = (CGeometry*)arrAllObjs[j];
		if( !pGeo )continue;
		
		k++;
		
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			expt = pGeo->GetDataPoint(0);
			geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; geopt.type = 0;
			geopt.id = k;
			arrAll.Add(geopt);
		}
		else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		{
			const CShapeLine *pShape = pGeo->GetShape();
			if (!pShape) continue;
			
			CArray<PT_3DEX,PT_3DEX> pts;
			pShape->GetKeyPts(pts);
			
			npt = pts.GetSize();
			arrCur.RemoveAll();
			for( int n=0; n<npt; n++)
			{
				expt = pts[n];
				geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; 
				geopt.type = 1;
				geopt.id = k;
				arrCur.Add(geopt);
			}
			
			KickoffSameGeoPts(arrCur);
			if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
		}
		else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			const CShapeLine *pShape = pGeo->GetShape();
			if (!pShape) continue;
			
			CArray<PT_3DEX,PT_3DEX> pts;
			pShape->GetKeyPts(pts);
			
			npt = pts.GetSize();
			arrCur.RemoveAll();
			for( int n=0; n<npt; n++)
			{
				expt = pts[n];
				if (expt.pencode == penMove && n != 0)
				{
					k++;
					continue;
				}
				geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
				geopt.type = 1;
				geopt.id = k;
				arrCur.Add(geopt);
			}
			
			KickoffSameGeoPts(arrCur);
			if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
		}

		delete pGeo;
	}

	if( arrAll.GetSize()>0 )
	{
		CMYTinObj tin;
		tin.Clear();
		
		MyTriangle::createTIN(arrAll.GetData(),arrAll.GetSize(),&tin.tins,&tin.nTIN,&tin.pts,&tin.npt);

		tin.RefreshTriangleBounds();
		
		MYPT_3D tri[3];

		CUndoFtrs undo(m_pEditor,Name());

		int index = tin.FindTriangle(pt);
		if(index>=0)
		{
			tin.GetTriangle(index,tri);

			tin.GetZFromTriangle(tri,pt.x,pt.y,pt.z);
			
			CFeature *pNewFtr = pFtrLayer0->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),CLS_GEOPOINT);
			if (!pNewFtr)
			{
				Abort();
				return FALSE;
			}
			
			if (!pNewFtr->GetGeometry()->CreateShape(&PT_3DEX(pt,penLine),1))
			{			
				Abort();
				delete pNewFtr;
				return FALSE;
			}
			
			m_pEditor->AddObject(pNewFtr,pFtrLayer0->GetID());
			
			undo.AddNewFeature(FtrToHandle(pNewFtr));
		}
		
		undo.Commit();								
	}

	m_nStep = 4;
	Finish();

	return TRUE;	
}

void CInterpolatePointZCommand_NoSelect::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	if(m_strLayerContour.IsEmpty())
		Do_TIN(pt);
	else
		Do(pt);

	CCommand::PtClick(pt, flag);
}


void CInterpolatePointZCommand_NoSelect::Finish()
{
	CEditCommand::Finish();	
}

void CInterpolatePointZCommand_NoSelect::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;	

	var = (_bstr_t)(LPCTSTR)(m_strLayerPt);
	tab.AddValue("PointLayer",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)(m_strLayerContour);
	tab.AddValue("ContourLayer",&CVariantEx(var));
	
}

void CInterpolatePointZCommand_NoSelect::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(AccelStr(),Name());
	param->AddLayerNameParam("PointLayer",LPCTSTR(m_strLayerPt),StrFromResID(IDS_CMDPLANE_POINTZLAYER),NULL,LAYERPARAMITEM_POINT);
	param->AddLayerNameParamEx("ContourLayer",LPCTSTR(m_strLayerContour),StrFromResID(IDS_CMDPLANE_CONTOURLAYER),NULL,LAYERPARAMITEM_LINE);
}


void CInterpolatePointZCommand_NoSelect::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"PointLayer",var) )
	{
		m_strLayerPt = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"ContourLayer",var) )
	{
		m_strLayerContour = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	CEditCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CDivideCurveCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDivideCurveCommand,CEditCommand)

CDivideCurveCommand::CDivideCurveCommand()
{
	m_nMode = 0;
	m_nDivideNum = 10;
	m_fDivideDis = 10;
	m_nStep = -1;
	strcat(m_strRegPath,"\\DivideCurve");
}

CDivideCurveCommand::~CDivideCurveCommand()
{
}

void CDivideCurveCommand::Abort()
{	
	CEditCommand::Abort();
}

CString CDivideCurveCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DIVIDECURVE);
}

void CDivideCurveCommand::Start()
{
	if( !m_pEditor )return;

	m_nMode = 0;
	m_nDivideNum = 10;
	m_fDivideDis = 10;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->OpenSelector(SELMODE_SINGLE);
}

void CDivideCurveCommand::PtMove(PT_3D &pt)
{
	CEditCommand::PtMove(pt);
}

void CDivideCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{	
		int nsel;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		if( nsel!=1 )
			return;		
		
		CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
		
		if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			return;
		}

		m_objHandles = handles[0];
		
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);	

		CString tip;
		if (m_nMode == 0)
		{
			tip = StrFromResID(IDS_CMDTIP_DIVIDE_NUM);			
		}
		else
		{
			tip = StrFromResID(IDS_CMDTIP_DIVIDE_DIS);
		}

		GOutPut(tip);
		
	}	
	
	if(m_nStep == 1)
	{
		CGeometry *pGeo = HandleToFtr(m_objHandles)->GetGeometry();

		CArray<PT_3DEX,PT_3DEX> pts, ret;
		const CShapeLine *pShape = pGeo->GetShape();
		pShape->GetPts(pts);
		
		float len = pShape->GetLength();

		float dCycle = 0;
		if (m_nMode == 0)
		{
			dCycle = len/m_nDivideNum;
		}
		else if (m_nMode == 1)
		{
			dCycle = m_fDivideDis;
		}

		GraphAPI::GGetCycleLine(pts.GetData(),pts.GetSize(),dCycle,dCycle,ret,0);
		int retsize = ret.GetSize();
		if (m_nMode == 0 && retsize > m_nDivideNum-1)
		{
			ret.RemoveAt(retsize-1);
		}

		CString strLayerName = m_strLayerCode;
		if (strLayerName.IsEmpty())
		{
			strLayerName = StrFromResID(IDS_DEFLAYER_NAMEP);
		}
		
		CFtrLayer *pFtrLayer = GETDS(m_pEditor)->GetFtrLayer(strLayerName);
		if (pFtrLayer == NULL)
		{
			Abort();
			return;
		}
		
		CFeature *pFtr0 = pFtrLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),CLS_GEOPOINT);
		if (!pFtr0)
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor,Name());
		for (int i=0; i<ret.GetSize(); i++)
		{
			CFeature *pNewFtr = pFtr0->Clone();
			if (!pNewFtr) 
			{
				Abort();
				return;
			}

			PT_3DEX retPt = ret[i];
			retPt.pencode = penLine;
			if (!pNewFtr->GetGeometry()->CreateShape(&retPt,1))
			{			
				delete pNewFtr;
				continue;
			}
			
			m_pEditor->AddObject(pNewFtr,pFtrLayer->GetID());

			undo.AddNewFeature(FtrToHandle(pNewFtr));
		}		
		
		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		m_nStep = 4;
		Finish();
	}
	CCommand::PtClick(pt, flag);
}


void CDivideCurveCommand::Finish()
{	
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();	
}

void CDivideCurveCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;	

	var = (_bstr_t)(m_strLayerCode);
	tab.AddValue(PF_CONTOURCODE,&CVariantEx(var));

	var = (long)(m_nMode);
	tab.AddValue(PF_DIVIDETYPE,&CVariantEx(var));
	
	var = (long)(m_nDivideNum);
	tab.AddValue(PF_DIVIDENUM,&CVariantEx(var));

	var = (float)(m_fDivideDis);
	tab.AddValue(PF_DIVIDEDIS,&CVariantEx(var));
}

void CDivideCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("DivideCurve",StrFromLocalResID(IDS_CMDNAME_DIVIDECURVE));
	param->BeginOptionParam(PF_DIVIDETYPE,StrFromResID(IDS_CMDPLANE_DIVIDEMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_DIVIDE_NUM),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_DIVIDE_DIS),1,' ',m_nMode==1);
	param->EndOptionParam();
	if (m_nMode == 0 || bForLoad)
	{
		param->AddParam(PF_DIVIDENUM,(int)m_nDivideNum,StrFromResID(IDS_CMDTIP_DIVIDE_NUM));
	}
	
	if (m_nMode == 1 || bForLoad)
	{
		param->AddParam(PF_DIVIDEDIS,(float)m_fDivideDis,StrFromResID(IDS_CMDTIP_DIVIDE_DIS));
	}
	param->AddLayerNameParam(PF_CONTOURCODE,LPCTSTR(m_strLayerCode),StrFromResID(IDS_CMDPLANE_DIVIDEPOINTCODE));
}


void CDivideCurveCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_CONTOURCODE,var) )
	{
		m_strLayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	

	if( tab.GetValue(0,PF_DIVIDETYPE,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();
	}	

	if( tab.GetValue(0,PF_DIVIDENUM,var) )
	{
		m_nDivideNum = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	

	if( tab.GetValue(0,PF_DIVIDEDIS,var) )
	{
		m_fDivideDis = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	

	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CIntersectCurveCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CIntersectCurveCommand,CEditCommand)

CIntersectCurveCommand::CIntersectCurveCommand()
{
	m_bIntersectptBreak = TRUE;
	m_nStep = -1;
	strcat(m_strRegPath,"\\IntersectCurve");
}

CIntersectCurveCommand::~CIntersectCurveCommand()
{
}

void CIntersectCurveCommand::Abort()
{	
	CEditCommand::Abort();
}

CString CIntersectCurveCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CURVEINTERSECT);
}

void CIntersectCurveCommand::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	GOutPut(StrFromResID(IDS_CMDTIP_SELFIRSTCURVE));

	m_pEditor->OpenSelector(SELMODE_SINGLE);
}

void CIntersectCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{	
		int nsel;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		if( nsel!=1 )
			return;		
		
		CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();

		if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			return;
		}

		m_objHandles[0] = handles[0];
		m_arrPts[0]  = pt;
		
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);	
		
		GOutPut(StrFromResID(IDS_CMDTIP_SELSECONDCURVE));
	}
	
	else if( m_nStep==1 )
	{
		int nsel;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		if( nsel!=1 )
			return;	

		if (m_objHandles[0]==handles[0])
		{
			return;
		}

		CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
		
		if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			return;
		}

		m_objHandles[1] = handles[0];
		m_arrPts[1]  = pt;
				
		m_pEditor->CloseSelector();
		m_nStep = 2;
		GotoState(PROCSTATE_PROCESSING);

		GOutPut(StrFromResID(IDS_CMDTIP_CLICKOK));
	}
	else if(m_nStep == 2)
	{
		CFeature *pFtr1 = HandleToFtr(m_objHandles[0]);
		CFeature *pFtr2 = HandleToFtr(m_objHandles[1]);
		CArray<PT_3DEX,PT_3DEX> pts1, pts2;
		const CShapeLine *pShape1 = pFtr1->GetGeometry()->GetShape();
		pShape1->GetPts(pts1);

		int size1 = pts1.GetSize();

		const CShapeLine *pShape2 = pFtr2->GetGeometry()->GetShape();
		pShape2->GetPts(pts2);

		int size2 = pts2.GetSize();

		int iIndex1 = -1, iIndex2 = -1;
		PT_3DEX p1(m_arrPts[0],penMove),p2(m_arrPts[1],penMove), ret;
		GraphAPI::GGetNearstDisPToPointSet2D(pts1.GetData(),pts1.GetSize(),p1,ret,&iIndex1);
		GraphAPI::GGetNearstDisPToPointSet2D(pts2.GetData(),pts2.GetSize(),p2,ret,&iIndex2);
		
		if (iIndex1 < 0 || iIndex2 < 0)
		{
			Abort();
			return;
		}

		PT_3D line1[2], line2[2], ptIntersect;
		line1[0] = pts1[iIndex1];
		line1[1] = pts1[iIndex1+1];
		line2[0] = pts2[iIndex2];
		line2[1] = pts2[iIndex2+1];
		
		BOOL bOrder1 = TRUE, bOrder2 = TRUE;
		if (GraphAPI::GGetRay1IntersectRay2(line1[0].x,line1[0].y,line1[1].x,line1[1].y,line2[0].x,line2[0].y,line2[1].x,line2[1].y,&ptIntersect.x,&ptIntersect.y));
		else if (GraphAPI::GGetRay1IntersectRay2(line1[1].x,line1[1].y,line1[0].x,line1[0].y,line2[1].x,line2[1].y,line2[0].x,line2[0].y,&ptIntersect.x,&ptIntersect.y))
		{
			bOrder1 = bOrder2 = FALSE;
		}
		else if (GraphAPI::GGetRay1IntersectRay2(line1[0].x,line1[0].y,line1[1].x,line1[1].y,line2[1].x,line2[1].y,line2[0].x,line2[0].y,&ptIntersect.x,&ptIntersect.y))
		{
			bOrder2 = FALSE;
		}
		else if (GraphAPI::GGetRay1IntersectRay2(line1[1].x,line1[1].y,line1[0].x,line1[0].y,line2[0].x,line2[0].y,line2[1].x,line2[1].y,&ptIntersect.x,&ptIntersect.y))
		{
			bOrder1 = FALSE;
		}
		else
		{
			Abort();
			return;
		}

		int i = 0;

		if (m_bIntersectptBreak)
		{
			CUndoModifyProperties undo(m_pEditor,Name());
			
			CArray<PT_3DEX,PT_3DEX> pts1New, pts2New;

			if (bOrder1)
			{
				for (i=0; i<=iIndex1; i++)
				{
					pts1New.Add(pts1[i]);
				}
				
				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[iIndex1+1].z,pts1[size1-1].pencode);
				pts1New.Add(addPt1);
			}
			else
			{
				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[0].z,pts1[0].pencode);
				pts1New.Add(addPt1);
				
				for (i=iIndex1+1; i<size1; i++)
				{
					pts1New.Add(pts1[i]);
				}
			}

			if (bOrder2)
			{
				for (i=0; i<=iIndex2; i++)
				{
					pts2New.Add(pts2[i]);
				}
				
				PT_3DEX addPt2(ptIntersect.x,ptIntersect.y,pts2[iIndex2+1].z,pts2[size2-1].pencode);
				pts2New.Add(addPt2);
			}
			else
			{
				PT_3DEX addPt2(ptIntersect.x,ptIntersect.y,pts2[0].z,pts2[0].pencode);
				pts2New.Add(addPt2);
				
				for (i=iIndex2+1; i<size2; i++)
				{
					pts2New.Add(pts2[i]);
				}
			}
			/*if (bOrderIntersect)
			{
				for (int i=0; i<=iIndex1; i++)
				{
					pts1New.Add(pts1[i]);
				}

				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[iIndex1+1].z,pts1[size1-1].pencode);
				pts1New.Add(addPt1);

				for (i=0; i<=iIndex2; i++)
				{
					pts2New.Add(pts2[i]);
				}
				
				PT_3DEX addPt2(ptIntersect.x,ptIntersect.y,pts2[iIndex2+1].z,pts2[size2-1].pencode);
				pts2New.Add(addPt2);
				
			}
			else
			{
				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[0].z,pts1[0].pencode);
				pts1New.Add(addPt1);

				for (int i=iIndex1+1; i<size1; i++)
				{
					pts1New.Add(pts1[i]);
				}
				
				PT_3DEX addPt2(ptIntersect.x,ptIntersect.y,pts2[0].z,pts2[0].pencode);
				pts2New.Add(addPt2);

				for (i=iIndex2+1; i<size2; i++)
				{
					pts2New.Add(pts2[i]);
				}
			}*/
			
			undo.arrHandles.Add(m_objHandles[0]);
			
			CValueTable oldTab;
			oldTab.BeginAddValueItem();
			pFtr1->WriteTo(oldTab);
			oldTab.EndAddValueItem();
			
			undo.oldVT.AddItemFromTab(oldTab);
			
			m_pEditor->DeleteObject(m_objHandles[0],FALSE);
			
			pFtr1->GetGeometry()->CreateShape(pts1New.GetData(),pts1New.GetSize());
			
			m_pEditor->RestoreObject(m_objHandles[0]);
			
			CValueTable newTab;
			newTab.BeginAddValueItem();
			pFtr1->WriteTo(undo.newVT);
			newTab.EndAddValueItem();
			
			undo.newVT.AddItemFromTab(newTab);
			
			
			undo.arrHandles.Add(m_objHandles[1]);
			
			oldTab.DelAll();
			oldTab.BeginAddValueItem();
			pFtr2->WriteTo(oldTab);
			oldTab.EndAddValueItem();
			
			undo.oldVT.AddItemFromTab(oldTab);
			
			m_pEditor->DeleteObject(m_objHandles[1],FALSE);
			
			pFtr2->GetGeometry()->CreateShape(pts2New.GetData(),pts2New.GetSize());
			
			m_pEditor->RestoreObject(m_objHandles[1]);
			
			newTab.DelAll();
			newTab.BeginAddValueItem();
			pFtr2->WriteTo(undo.newVT);
			newTab.EndAddValueItem();
			
			undo.newVT.AddItemFromTab(newTab);
			
			undo.Commit();

		}
		else
		{
			if (m_pEditor->GetFtrLayerIDOfFtr(m_objHandles[0]) != m_pEditor->GetFtrLayerIDOfFtr(m_objHandles[1]) || 
				pFtr1->GetGeometry()->GetClassType() != pFtr2->GetGeometry()->GetClassType())
			{
				Abort();
				return;
			}

			CUndoFtrs undo(m_pEditor,Name());
			
			CArray<PT_3DEX,PT_3DEX> ptsNew;
			
			if (bOrder1)
			{
				for (i=0; i<=iIndex1; i++)
				{
					ptsNew.Add(pts1[i]);
				}
				
				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[iIndex1+1].z,pts1[size1-1].pencode);
				ptsNew.Add(addPt1);
			}
			else
			{
				for (i=size1-1; i>=iIndex1+1; i--)
				{
					ptsNew.Add(pts1[i]);
				}
				
				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[0].z,pts1[0].pencode);
				ptsNew.Add(addPt1);
			}
			
			if (bOrder2)
			{
				for (i=iIndex2; i>=0; i--)
				{
					ptsNew.Add(pts2[i]);
				}
			}
			else
			{
				for (i=iIndex2+1; i<size2; i++)
				{
					ptsNew.Add(pts2[i]);
				}
			}

			/*if (bOrderIntersect)
			{
				for (int i=0; i<=iIndex1; i++)
				{
					ptsNew.Add(pts1[i]);
				}
				
				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[iIndex1+1].z,pts1[size1-1].pencode);
				ptsNew.Add(addPt1);
				
				for (i=iIndex2; i>=0; i--)
				{
					ptsNew.Add(pts2[i]);
				}
				
			}
			else
			{				
				for (int i=size1-1; i>=iIndex1+1; i--)
				{
					ptsNew.Add(pts1[i]);
				}

				PT_3DEX addPt1(ptIntersect.x,ptIntersect.y,pts1[0].z,pts1[0].pencode);
				ptsNew.Add(addPt1);
				
				for (i=iIndex2+1; i<size2; i++)
				{
					ptsNew.Add(pts2[i]);
				}
			}*/
			
			undo.AddOldFeature(m_objHandles[0]);
			undo.AddOldFeature(m_objHandles[1]);

			CFeature *pNewFtr = pFtr1->Clone();
			if (!pNewFtr)
			{
				Abort();
				return;
			}

			pNewFtr->SetID(OUID());

			if (!pNewFtr->GetGeometry()->CreateShape(ptsNew.GetData(),ptsNew.GetSize()))
			{
				Abort();
				return;
			}
			
			pNewFtr->GetGeometry()->CreateShape(ptsNew.GetData(),ptsNew.GetSize());
			
			m_pEditor->AddObject(pNewFtr,m_pEditor->GetFtrLayerIDOfFtr(m_objHandles[0]));

			undo.AddNewFeature(FtrToHandle(pNewFtr));

			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objHandles[0]),pNewFtr);

			m_pEditor->DeleteObject(m_objHandles[0]);
			m_pEditor->DeleteObject(m_objHandles[1]);			
			
			undo.Commit();
		}

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		m_nStep = 3;
		Finish();
	}
	CCommand::PtClick(pt, flag);
}


void CIntersectCurveCommand::Finish()
{	
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();	
}

void CIntersectCurveCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;	

	var = (bool)(m_bIntersectptBreak);
	tab.AddValue(PF_JOIN_MODE,&CVariantEx(var));
}

void CIntersectCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("IntersectCurve",StrFromLocalResID(IDS_CMDNAME_CURVEINTERSECT));
	param->AddParam(PF_JOIN_MODE,(bool)(m_bIntersectptBreak),StrFromResID(IDS_CMDPLANE_INTERSECTPOINTSPLIT));
}


void CIntersectCurveCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_JOIN_MODE,var) )
	{
		m_bIntersectptBreak = (bool)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CCurveToBoundCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCurveToBoundCommand,CEditCommand)

CCurveToBoundCommand::CCurveToBoundCommand()
{
	m_lfToler=0.2;
	m_nStep = -1;
	m_boundScope = NULL;
	strcat(m_strRegPath,"\\CurveToBound");
}

CCurveToBoundCommand::~CCurveToBoundCommand()
{
}

void CCurveToBoundCommand::Abort()
{	
	CEditCommand::Abort();
}

CString CCurveToBoundCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CURVETOBOUND);
}

void CCurveToBoundCommand::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();
	m_nStep = 0;
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

//线段和射线求交点
static bool GGetLineIntersectLineSeg2(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, double *plfX, double *plfY)
{
	double vector1x = lfX1-lfX0, vector1y = lfY1-lfY0;
	double vector2x = lfX3-lfX2, vector2y = lfY3-lfY2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double toler = GraphAPI::g_lfDisTolerance;
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<toler && unitdelta>-toler )return false;
	double t1 = ( (lfX2-lfX0)*vector2y-(lfY2-lfY0)*vector2x )/delta;
	double xr = lfX0 + t1*vector1x, yr = lfY0 + t1*vector1y;
	if( t1<0 || t1>1 )
	{
		if( fabs(xr-lfX0)<=toler && fabs(yr-lfY0)<=toler );
		else if( fabs(xr-lfX1)<=toler && fabs(yr-lfY1)<=toler );
		else return false;
	}
	double t2 = ( (lfX2-lfX0)*vector1y-(lfY2-lfY0)*vector1x )/delta;
	if( t2<toler )
	{
		return false;
	}
	if(plfX)*plfX = xr;
	if(plfY)*plfY = yr;
	return true;
}

void CCurveToBoundCommand::CurveToBound(CFeature* pFeature, CUndoModifyProperties& undo)
{
	CGeometry* pGeo = pFeature->GetGeometry();
	if(!pGeo)  return;
	
	CArray<PT_3DEX, PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	int nPt =arrPts.GetSize();
	if(nPt<2) return;
	
	if(GraphAPI::GIsEqual3DPoint(&arrPts[0], &arrPts[nPt-1]))
	{
		return;//闭合的不处理
	}

	int index1=-1, index2=-1;

	//地物的首点
	BoundScope* temp = m_boundScope;
	PT_3DEX expt1 = arrPts[1];
	PT_3DEX expt2 = arrPts[0];
	while(temp)
	{
		if(temp->env.bPtIn(&expt2))
		{
			PT_3D pt0 = temp->pt0;
			PT_3D pt1 = temp->pt1;
			PT_3D retPt;
			BOOL bret = GGetLineIntersectLineSeg2(pt0.x, pt0.y, pt1.x, pt1.y, 
							expt1.x, expt1.y, expt2.x, expt2.y, &retPt.x, &retPt.y);
			if(bret)
			{
				expt2.x = retPt.x;
				expt2.y = retPt.y;
				index1 = 0;
				break;
			}	
		}
		temp = temp->next;
	}
	
	//地物的尾点
	temp = m_boundScope;
	PT_3DEX expt3 = arrPts[nPt-2];
	PT_3DEX expt4 = arrPts[nPt-1];
	while(temp)
	{
		if(temp->env.bPtIn(&expt4))
		{
			PT_3D pt0 = temp->pt0;
			PT_3D pt1 = temp->pt1;
			PT_3D retPt;
			BOOL bret = GGetLineIntersectLineSeg2(pt0.x, pt0.y, pt1.x, pt1.y, 
							expt3.x, expt3.y, expt4.x, expt4.y, &retPt.x, &retPt.y);
			if(bret)
			{
				expt4.x = retPt.x;
				expt4.y = retPt.y;
				index2 = nPt-1;
				break;
			}	
		}
		temp = temp->next;
	}
	
	if(index1==0 || index2>0)
	{
		undo.arrHandles.Add(FtrToHandle(pFeature));
		CValueTable tab2;
		tab2.BeginAddValueItem();
		pFeature->WriteTo(tab2);	
		tab2.EndAddValueItem();
		undo.oldVT.AddItemFromTab(tab2);
		
		m_pEditor->DeleteObject(FtrToHandle(pFeature));
		if(index1==0)
		{
			pGeo->SetDataPoint(index1, expt2);
		}
		if(index2>0)
		{
			pGeo->SetDataPoint(index2, expt4);
		}
		m_pEditor->RestoreObject(FtrToHandle(pFeature));
		
		CVariantEx var;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo->GetShape(arrPts);
		var.SetAsShape(arrPts);
		
		if (tab2.SetValue(0,FIELDNAME_SHAPE,&var))
		{
			undo.newVT.AddItemFromTab(tab2);
		}
	}
}

void CCurveToBoundCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	//获取层
	CFtrLayerArray arrLayers;
	int i=0;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayer,arrLayers);

	m_boundScope = new BoundScope;
	if(m_strLayNameOfTKName.IsEmpty())
	{
		Envelope e = pDS->GetBound();
		m_boundScope->CreateOrAddFromEnv(e, m_lfToler);
	}
	else
	{
		CFtrLayerArray ftrLayers;
		pDS->GetFtrLayersByNameOrCode(m_strLayNameOfTKName,ftrLayers);
		for (i=0; i<ftrLayers.GetSize(); i++)
		{
			CFtrLayer *pLayer=NULL;
			pLayer = pDS->GetFtrLayer(ftrLayers[i]->GetName());
			
			CFeature *pFtr=NULL;
			int nObj = ftrLayers[i]->GetObjectCount();
			for(int j=0; j<nObj; j++)
			{
				pFtr = ftrLayers[i]->GetObject(j);
				if (pFtr==NULL||!pFtr->IsVisible())
					continue;

				Envelope e = pFtr->GetGeometry()->GetEnvelope();
				m_boundScope->CreateOrAddFromEnv(e, m_lfToler);
			}
		}
	}

	CUndoModifyProperties undo(m_pEditor,Name());
	for (i=0; i<arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer=NULL;
		pLayer = pDS->GetFtrLayer(arrLayers[i]->GetName());

		int nObj = arrLayers[i]->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			CFeature* pFeature = arrLayers[i]->GetObject(j);
			if(!pFeature || !pFeature->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;
			PT_3DEX pt3dex;
			CurveToBound(pFeature, undo);
		}	
	}

	undo.Commit();

	Finish();
	
	//清理
	BoundScope *temp = m_boundScope;
	while(temp)
	{
		BoundScope *temp1 = temp->next;
		delete temp;
		temp = temp1;
	}
	
	CCommand::PtClick(pt, flag);
}


void CCurveToBoundCommand::Finish()
{
	UpdateParams(TRUE);
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CCurveToBoundCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;	

	var = (LPCTSTR)(m_strLayer);
	tab.AddValue("Layer0",&CVariantEx(var));
	
	var = m_lfToler;
	tab.AddValue(PF_TOLER,&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfTKName;
	tab.AddValue(PF_LAYNAME_BOUNDNAME,&CVariantEx(var));
}

void CCurveToBoundCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CurveToBound",Name());
	param->AddLayerNameParamEx("Layer0",(LPCTSTR)m_strLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER));
	param->AddUsedLayerNameParam(PF_LAYNAME_BOUNDNAME,(LPCTSTR)m_strLayNameOfTKName,StrFromResID(IDS_LAYNAME_BOUNDNAME));
	param->AddParam(PF_TOLER,(double)m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));
}

void CCurveToBoundCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strLayer.TrimRight();
		m_strLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,PF_TOLER,var) )
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
 	}

	if (tab.GetValue(0,PF_LAYNAME_BOUNDNAME,var))
	{
		m_strLayNameOfTKName  = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CIntersectCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CIntersectCommand,CEditCommand)

CIntersectCommand::CIntersectCommand()
{
	m_nStep = -1;
	m_nMode = 0;
	strcat(m_strRegPath,"\\Intersect");
}

CIntersectCommand::~CIntersectCommand()
{
}

void CIntersectCommand::Abort()
{	
	CEditCommand::Abort();
}

CString CIntersectCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_INTERSECT);
}

void CIntersectCommand::Start()
{
	if( !m_pEditor )return;
	m_pFtrs[0]  = NULL;
	m_pFtrs[1]  = NULL;
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);
}

int KickoffSame2DPoints_notPenMove(CArray<PT_3DEX,PT_3DEX>& arrPts);

void CIntersectCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag,FALSE) )return;
		m_objHandles[0] = m_pEditor->GetSelection()->GetLastSelectedObj();
		m_pFtrs[0] = HandleToFtr(m_objHandles[0]);
		if (m_pFtrs[0]==NULL)
		{
			return;
		}
		m_pFtrs[0] =m_pFtrs[0]->Clone();
		if( !(m_pFtrs[0]) )return;
		if( !(m_pFtrs[0]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return;
		
//		m_pEditor->SetCurDrawingObj(DrawingInfo((m_pFtrs[0]->GetGeometry()),(m_pFtrs[0]->GetGeometry())->GetDataPointSum()+1));
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);		
	}
	
	else if( m_nStep==1 )
	{
		if( !CanGetSelObjs(flag,FALSE) )return;
		m_objHandles[1] = m_pEditor->GetSelection()->GetLastSelectedObj();
		if (m_objHandles[1]==m_objHandles[0])
		{
			return;
		}
		m_pFtrs[1] = HandleToFtr(m_objHandles[1]);
		if (m_pFtrs[1]==NULL)
		{
			return;
		}
		m_pFtrs[1] = m_pFtrs[1]->Clone();
		if( !(m_pFtrs[1]) )return;
		if( !(m_pFtrs[1]->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return;
		
//		m_pEditor->SetCurDrawingObj(DrawingInfo());
		
		m_pEditor->CloseSelector();
		m_nStep = 2;
		GotoState(PROCSTATE_PROCESSING);
	}
	else if(m_nStep == 2)
	{
		int modified=0;
		
		if(((m_pFtrs[0]->GetGeometry())->GetDataPointSum())<2 )return;
		const CShapeLine *pSL = (m_pFtrs[0]->GetGeometry())->GetShape();
		if( !pSL )return;
		CArray<int, int> pos1;
		pSL->GetKeyPosOfBaseLines(pos1);
		CArray<PT_3DEX,PT_3DEX> pts1,arr1;		
		pSL->GetPts(pts1);
		pSL->GetKeyPts(arr1);
		int PtNums1 = pts1.GetSize();
		PT_3D *Pnts1=new PT_3D[PtNums1];
		if(!Pnts1)return;
		for (int j=0;j<PtNums1;j++)//获取点数组
		{
			COPY_3DPT(Pnts1[j],pts1[j]);
		}
		
		if(((m_pFtrs[1]->GetGeometry())->GetDataPointSum())<2 )return;
		pSL = (m_pFtrs[1]->GetGeometry())->GetShape();
		if( !pSL )return;
		CArray<int, int> pos2;
		pSL->GetKeyPosOfBaseLines(pos2);
		CArray<PT_3DEX,PT_3DEX> pts2,arr2;
		pSL->GetPts(pts2);
		pSL->GetKeyPts(arr2);
		int PtNums2 = pts2.GetSize();
		PT_3D *Pnts2=new PT_3D[PtNums2];
		if(!Pnts2)return;
		for ( j=0;j<PtNums2;j++)//获取点数组
		{
			COPY_3DPT(Pnts2[j],pts2[j]);
		}
		int PtNums3=PtNums1;
		PT_3D  *Pnts3=new PT_3D[PtNums3];
		if(!Pnts3)return;
		memcpy(Pnts3,Pnts1,PtNums3*sizeof(PT_3D));		
		int len1=PtNums1;
		int len2=PtNums2;
		LinkList L=NULL;
		LNode *p=NULL;
		PT_3DEX  ret;
		initLList(L);		

		bool mark;
		if (m_nMode==0)
		{
			mark = true;
		}
		else
			mark = false;

		int nAddPtSum = 0;
		//内插两条折线的交点。分别以折线1和2为基准进行两次嵌套循环，并将所求节点按照一定顺序（t从小到大）加入到链表中，再从链表中
		//从头至尾取出节点插入
		//遍历折线1中的每条线段
		for(j=0;j<PtNums1-1;j++)
		{
			int k=j;
			//遍历折线2中的每条线段
			for( int i=0; i<PtNums2-1; i++)
			{
				int flag=-1;
				if( (flag=GLineIntersectLineSegInLimit1(Pnts1[j].x,Pnts1[j].y,Pnts1[j].z,
					Pnts1[j+1].x,Pnts1[j+1].y,Pnts1[j+1].z,
					Pnts2[i].x,Pnts2[i].y,Pnts2[i].z,Pnts2[i+1].x,
					Pnts2[i+1].y,Pnts2[i+1].z,					
					&ret.x,&ret.y,&ret.z,10e-4,mark))==0 )
					continue;
				
				else
				{
					if (flag==1)
					{
						addLnode(&L,&(Pnts1[j]),&ret);   //插入链表，排序过程在	addLnode中已经进行
					}
					else if (flag==2)
					{
						Pnts1[j].z=ret.z;
					}
					else if (flag==3)
					{
						Pnts1[j+1].z=ret.z;
					}
					modified=1;
					/*addLnode(&L,&(Pnts1[j]),&ret);*/	//插入链表，排序过程在	addLnode中已经进行
					
				}
				
			}

			int nAddPtSum0 = 0;
			int k0 = k + nAddPtSum;
			for (p=L;p!=NULL;p=p->next)
			{
				for (int m=0; m<pos1.GetSize(); m++)
				{
					if (k0 < pos1[m])
					{
						PT_3DEX ptex(p->pt3d,arr1[m].pencode);
						arr1.InsertAt(m,ptex);
						pos1.InsertAt(m,k0);

						if (m >= 1 && k0 == pos1[m-1])
						{
							for (int n=m; n<pos1.GetSize(); n++)
							{
								pos1[n]++;
							}
						}

						nAddPtSum0++;
						
						break;
					}
				}
// 				insertPoint(&Pnts1,&len1,&PtNums1,&(p->pt3d),k+1);//插入Pnts1数组中
// 				j++;//相应的循环变量也要增加，以便定位于第二条线段的起点
			}

			nAddPtSum += nAddPtSum0;
			
			destroyList(&L);				    
		}

		nAddPtSum = 0;
		for(int i=0;i<PtNums2-1;i++)
		{
			int flag=-1;
			int k=i;
			for(int j=0; j<PtNums3-1; j++)
			{ 
				if(  (flag=GLineIntersectLineSegInLimit1(Pnts2[i].x,Pnts2[i].y,Pnts2[i].z,
					Pnts2[i+1].x,Pnts2[i+1].y,Pnts2[i+1].z,
					Pnts3[j].x,Pnts3[j].y,Pnts3[j].z,
					Pnts3[j+1].x,Pnts3[j+1].y,Pnts3[j+1].z,
					&ret.x,&ret.y,&ret.z,10e-4,true))==0 )
				{
					
					continue;
				}
				
				else
				{
					if (flag==1)
					{
						addLnode(&L,&(Pnts2[i]),&ret);
						
					}
					else if (flag==2)
					{
						Pnts2[i].z=ret.z;
					}
					else if (flag==3)
					{
						Pnts2[i+1].z=ret.z;
					}
					modified=1;
					
				}
				
				
				
			}

			int nAddPtSum0 = 0;
			int k0 = k + nAddPtSum;
			for (p=L;p!=NULL;p=p->next)
			{
				for (int m=0; m<pos2.GetSize(); m++)
				{
					if (k0 < pos2[m])
					{
						PT_3DEX ptex(p->pt3d,arr2[m].pencode);
						arr2.InsertAt(m,ptex);
						pos2.InsertAt(m,k0);

						if (m >= 1 && k0 == pos2[m-1])
						{
							for (int n=m; n<pos2.GetSize(); n++)
							{
								pos2[n]++;
							}
						}

						nAddPtSum0++;
						break;
					}
				}
// 				insertPoint(&Pnts2,&len2,&PtNums2,&(p->pt3d),k+1);
// 				i++;
			}

			nAddPtSum += nAddPtSum0;
			destroyList(&L);
			
		}
		
		
		if( modified )
		{
			CUndoFtrs undo(m_pEditor,Name());

			KickoffSame2DPoints_notPenMove(arr1);
			
			m_pFtrs[0]->SetID(OUID());
			m_pFtrs[0]->GetGeometry()->CreateShape(arr1.GetData(),arr1.GetSize());
			m_pEditor->AddObject(m_pFtrs[0],m_pEditor->GetFtrLayerIDOfFtr(m_objHandles[0]));

			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objHandles[0]),m_pFtrs[0]);
			undo.arrNewHandles.Add(FtrToHandle(m_pFtrs[0]));
			m_pEditor->DeleteObject(m_objHandles[0]);			
			undo.arrOldHandles.Add(m_objHandles[0]);	
			
			KickoffSame2DPoints_notPenMove(arr2);

			m_pFtrs[1]->SetID(OUID());
			m_pFtrs[1]->GetGeometry()->CreateShape(arr2.GetData(),arr2.GetSize());
			m_pEditor->AddObject(m_pFtrs[1],m_pEditor->GetFtrLayerIDOfFtr(m_objHandles[1]));

			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objHandles[1]),m_pFtrs[1]);
			undo.arrNewHandles.Add(FtrToHandle(m_pFtrs[1]));
			m_pEditor->DeleteObject(m_objHandles[1]);
			undo.arrOldHandles.Add(m_objHandles[1]);

			undo.Commit();
			m_pEditor->RefreshView();			
	
		}		
		if( Pnts1 )delete Pnts1;
		if( Pnts2 )delete Pnts2;
		if( Pnts3 )delete Pnts3;
		
        m_nStep=3;
		Finish();
		
		
	}
	CCommand::PtClick(pt, flag);
}


void CIntersectCommand::Finish()
{	
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();	
}

void CIntersectCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;	
	var = (long)(m_nMode);
	tab.AddValue(PF_INTERSECTWAY,&CVariantEx(var));	
}

void CIntersectCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("IntersectCommand",StrFromLocalResID(IDS_CMDNAME_INTERSECT));
	param->BeginOptionParam(PF_INTERSECTWAY,StrFromResID(IDS_CMDPLANE_INTERSECTWAY));
	param->AddOption(StrFromResID(IDS_KEEPZ),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_NOKEEPZ),1,' ',m_nMode==1);
	param->EndOptionParam();
	
	
}


void CIntersectCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_INTERSECTWAY,var) )
	{
		m_nMode = (long )(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CLayerIntersectCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CLayerIntersectCommand,CEditCommand)
CLayerIntersectCommand::CLayerIntersectCommand()
{	
	m_nMode = 0;
	m_strFIDs = _T("");
	strcat(m_strRegPath,"\\LayerIntersect");	
}

CLayerIntersectCommand::~CLayerIntersectCommand()
{
	
}
void CLayerIntersectCommand::Abort()
{	
	CEditCommand::Abort();
}

CCommand* CLayerIntersectCommand::Create()
{
	return new CLayerIntersectCommand;
}

void CLayerIntersectCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;	
	var = (long)(m_nMode);
	tab.AddValue(PF_INTERSECTWAY,&CVariantEx(var));
	
	var = (_bstr_t)(m_strFIDs);
	tab.AddValue(PF_INTERSECTFIRST,&CVariantEx(var));
}

void CLayerIntersectCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("LayerIntersectCommand",StrFromLocalResID(IDS_CMDNAME_LAYERINTERSECT));
	param->BeginOptionParam(PF_INTERSECTWAY,StrFromResID(IDS_CMDPLANE_INTERSECTWAY));
	param->AddOption(StrFromResID(IDS_KEEPZ),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_NOKEEPZ),1,' ',m_nMode==1);
	param->EndOptionParam();

	param->AddLayerNameParamEx(PF_INTERSECTFIRST,(LPCTSTR)m_strFIDs,StrFromResID(IDS_FIELDNAME_LAYERNAME),NULL,LAYERPARAMITEM_LINEAREA);	
}


void CLayerIntersectCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_INTERSECTWAY,var) )
	{
		m_nMode = (long )(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_INTERSECTFIRST,var) )
	{
		m_strFIDs = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

CString CLayerIntersectCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_LAYERINTERSECT);
}

void CLayerIntersectCommand::Start()
{
	if( !m_pEditor )return;
	m_nMode = 0;
	m_strFIDs = _T("");
	
	CEditCommand::Start();
	m_pEditor->CloseSelector();
	
}

BOOL CLayerIntersectCommand::PreProcess1(CFeature *pFtr,CArray<ObjItem,ObjItem&> &arrItem)
{
	if(!pFtr) return FALSE;
	CGeometry *pObj = pFtr->GetGeometry();
	if(!pObj) return FALSE;
	BOOL ret = FALSE;
	const Envelope e = pObj->GetShape()->GetEnvelope();
	for(int i=arrItem.GetSize()-1;i>=0;i--)
	{
		if(pFtr==arrItem[i].pFtr)
		{
			arrItem[i].flag1 = TRUE;
			continue;
		}
		const Envelope e0 = arrItem[i].pFtr->GetGeometry()->GetShape()->GetEnvelope();
		if (e.bIntersect(&e0))
		{
			arrItem[i].flag1 = FALSE;
			ret = TRUE;
		}
		else
			arrItem[i].flag1 = TRUE;
	}
	return ret;
}

BOOL CLayerIntersectCommand::PreProcess2(PT_3D pt1, PT_3D pt2, CArray<ObjItem,ObjItem&> &arrItem)
{
	BOOL ret = FALSE;

	for(int i=arrItem.GetSize()-1;i>=0;i--)
	{
		const CShapeLine *pShape = arrItem[i].pFtr->GetGeometry()->GetShape();
		if (pShape && pShape->bIntersect(pt1,pt2))
		{
			arrItem[i].flag2 = FALSE;
			ret = TRUE;
		}
		else
			arrItem[i].flag2 = TRUE;
	}
	return ret;
}

//类似于智能指针，小范围使用一下
template<class T>
class MemMgr
{
public:
	MemMgr()
	{		
	}
	~MemMgr()
	{
		for (int i=arrElement.GetSize()-1;i>=0;i--)
		{
			if((*(arrElement[i]))==NULL)
				continue;
			if (arrFlag[i])
			{
				delete [](*(arrElement[i]));
			}
			else 
				delete (*(arrElement[i]));
		}
	}

	void Add(T** elem, BOOL flag = FALSE)
	{
		arrElement.Add(elem);
		arrFlag.Add(flag);
	}
protected:
private:
	CArray<T**,T**> arrElement;
	CArray<BOOL,BOOL> arrFlag;
};


static int _comp_featureP(const void *p1, const void *p2)
{
	const CFeature **p3 = (const CFeature **)p1;
	const CFeature **p4 = (const CFeature **)p2;

	if(*p3==*p4)
		return 0;
	return *p3<*p4?-1:1;
}


void CLayerIntersectCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_strFIDs, arr);

	CUndoBatchAction undo(m_pEditor,Name());

	CFtrArray backupFtrs, modifiedFtrs, ftrs2;

	for(int i=0; i<arr.GetSize(); i++)
	{
		for(int j=i; j<arr.GetSize(); j++)
		{
			m_strFID1 = arr[i]->GetName();
			m_strFID2 = arr[j]->GetName();

			CString strMsg;
			strMsg.Format("%02d\\%02d: %s--%s",(int)(i+1),(int)(j+1),(LPCTSTR)m_strFID1, (LPCTSTR)m_strFID2);
			
			GOutPut(strMsg);

			Process(backupFtrs,modifiedFtrs);			
		}
	}

	ftrs2.Copy(modifiedFtrs);

	//去除重复
	qsort(modifiedFtrs.GetData(),modifiedFtrs.GetSize(),sizeof(CFeature*),_comp_featureP);
	modifiedFtrs.SetSize(GraphAPI::GKickoffSameData(modifiedFtrs.GetData(),modifiedFtrs.GetSize()));

	for(i=0; i<modifiedFtrs.GetSize(); i++)
	{
		CUndoShape undo1(m_pEditor,Name());

		CFeature *pFtr = modifiedFtrs[i];
		for(int j=0; j<ftrs2.GetSize(); j++)
		{
			if( pFtr==ftrs2[j] )
			{
				backupFtrs[j]->GetGeometry()->GetShape(undo1.arrPtsOld);
				break;
			}
		}

		if(j>=ftrs2.GetSize())
			continue;

		pFtr->GetGeometry()->GetShape(undo1.arrPtsNew);
		undo1.handle = FtrToHandle(pFtr);

		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		m_pEditor->RestoreObject(FtrToHandle(pFtr));

		undo.AddAction(&undo1);
	}

	for(i=0; i<backupFtrs.GetSize(); i++)
	{
		delete backupFtrs[i];
	}

	undo.Commit();
	
	Finish();
	
	CString strMsg;
	strMsg.Format(IDS_PROCESS_OBJ_NUM,modifiedFtrs.GetSize());
	
	GOutPut(strMsg);	

	CEditCommand::PtClick(pt,flag);
}


BOOL CLayerIntersectCommand::Process(CFtrArray& backupFtrs, CFtrArray& modifiedFtrs)
{
	{
		//得到找到层1和层2的地物
		CArray<int,int> delObjsIdx;
		CArray<ObjItem,ObjItem&> arr1,arr2;
		CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strFID1);
		if (!pLayer||pLayer->IsLocked()||!pLayer->IsVisible())
		{
			return FALSE;
		}
		for (int i=0; i<pLayer->GetObjectCount(); i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (pFtr)
			{
				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					continue;
				arr1.Add(ObjItem(pFtr));
			}
			
		}
		pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strFID2);
		if (!pLayer||pLayer->IsLocked()||!pLayer->IsVisible())
		{
			return FALSE;
		}
		for (i=0; i<pLayer->GetObjectCount(); i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (pFtr)
			{
				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					continue;
				arr2.Add(ObjItem(pFtr));
			}
			
		}
		
		int nSum = arr1.GetSize()+arr2.GetSize();
		if (m_strFID1==m_strFID2)
		{
			nSum = nSum>>1;
		}
		int k,l;
		bool mark;
		MemMgr<PT_3D> memSecur;

		GProgressStart(nSum);
		//以arr1为基准，循环取其中的每个地物与arr2中的做内插处理，结果加入数据源.得到内插的原地物需要做个标记，便于以后删除
		
		if (m_nMode==0)
		{
			mark = true;
		}
		else
			mark = false;
		
		for (i=arr1.GetSize()-1;i>=0;i--)
		{
			GProgressStep();
			//初步过滤掉arr2中明显不想交的地物
			if(!PreProcess1(arr1[i].pFtr,arr2)) continue;			
			
			CGeometry *pGeo = arr1[i].pFtr->GetGeometry();
			const CShapeLine *pSL = pGeo->GetShape();
			if( !pSL )return FALSE;
			CArray<int, int> pos1;
			pSL->GetKeyPosOfBaseLines(pos1);
			CArray<PT_3DEX,PT_3DEX> pts1,arrkey1;		
			pSL->GetPts(pts1);
			pSL->GetKeyPts(arrkey1);
			BOOL bSame = pts1.GetSize()==arrkey1.GetSize()?TRUE:FALSE;
			int nPts1 = pts1.GetSize();
			PT_3D *Pnts1=new PT_3D[nPts1];
			if(!Pnts1)return FALSE;
			memSecur.Add(&Pnts1,TRUE);
			for (int j=0;j<nPts1;j++)//获取点数组
			{
				COPY_3DPT(Pnts1[j],pts1[j]);
			}
			
			BOOL modified = FALSE;
			LinkList L=NULL;
			LNode *p=NULL;
			PT_3D  ret;
			initLList(L);

			int len1 = nPts1;			
			for (j=0;j<nPts1-1;j++)
			{
				if(!PreProcess2(Pnts1[j],Pnts1[j+1],arr2))continue;
				int idx = j;
				for (k=arr2.GetSize()-1;k>=0;k--)
				{
					if (arr2[k].flag1==TRUE||arr2[k].flag2==TRUE)continue;
					pSL = arr2[k].pFtr->GetGeometry()->GetShape();
					if( !pSL )return FALSE;
					CArray<int, int> pos2;
					pSL->GetKeyPosOfBaseLines(pos2);
					CArray<PT_3DEX,PT_3DEX> pts2,arrkey2;		
					pSL->GetPts(pts2);
					pSL->GetKeyPts(arrkey2);
					int nPts2 = pts2.GetSize();
					PT_3D *Pnts2=new PT_3D[nPts2];
					if(!Pnts2)return FALSE;
					memSecur.Add(&Pnts2,TRUE);
					for (l=0;l<nPts2;l++)//获取点数组
					{
						COPY_3DPT(Pnts2[l],pts2[l]);
					}

					int idx = l;
					for ( l=0;l<nPts2-1;l++)
					{
						int flag=-1;
						if( (flag=GLineIntersectLineSegInLimit1(Pnts1[j].x,Pnts1[j].y,Pnts1[j].z,
							Pnts1[j+1].x,Pnts1[j+1].y,Pnts1[j+1].z,
							Pnts2[l].x,Pnts2[l].y,Pnts2[l].z,Pnts2[l+1].x,
							Pnts2[l+1].y,Pnts2[l+1].z,					
							&ret.x,&ret.y,&ret.z,1e-4,mark))==0 )
							continue;
						else
						{
							//交点在线段内部
							if (flag==1)
							{
								//按照距离反序排列
								addLnode(&L,&(Pnts1[j]),&ret);   //插入链表，排序过程在	addLnode中已经进行
							}
							else if (flag==2)
							{
								Pnts1[j].z = ret.z;
							}
							else if (flag==3)
							{
								Pnts1[j+1].z = ret.z;
							}
							modified = TRUE;
							
						}
					}
					delete []Pnts2;
					Pnts2 = NULL;				
				}

				if (bSame)
				{
					//相交点插入到Pnts1中 
					for (p=L;p!=NULL;p=p->next)
					{
						insertPoint(&Pnts1,&len1,&nPts1,&(p->pt3d),idx+1);//插入Pnts1数组中
						j++;//相应的循环变量也要增加，以便定位于第二条线段的起点
					}
				}
				else
				{
					//相交点插入到arrkey1中 
					for (int m=0; m<pos1.GetSize(); m++)
					{
						if (idx < pos1[m])
						{
							for (p=L;p!=NULL;p=p->next)
							{							
								PT_3DEX ptex(p->pt3d,arrkey1[m].pencode);
								arrkey1.InsertAt(m,ptex);
								pos1.InsertAt(m,idx);								
							}
							break;
						}
					}
				}
								
				destroyList(&L);

			}
			//添加地物
			if (modified)
			{
				delObjsIdx.Add(i);

				CFeature *pFtrObj = arr1[i].pFtr;
				backupFtrs.Add(pFtrObj->Clone());
				if (bSame)
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					PT_3DEX PtEX;
					for (int i=0;i<nPts1;i++)
					{
						COPY_3DPT(PtEX,Pnts1[i]);
						PtEX.pencode = penLine;
						
						arr.Add(PtEX);
						
					}

					KickoffSame2DPoints_notPenMove(arr);

					pFtrObj->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());
				}
				else
				{
					KickoffSame2DPoints_notPenMove(arrkey1);

					pFtrObj->GetGeometry()->CreateShape(arrkey1.GetData(),arrkey1.GetSize());
				}

				modifiedFtrs.Add(pFtrObj);
			}
			delete []Pnts1;
			Pnts1 = NULL;
		}
		if (m_strFID1==m_strFID2)
		{
			goto ret;
		}
		//以arr2为基准，循环取其中的每个地物与arr1中的做内插处理，结果加入数据源

		if (m_nMode==0||m_nMode==1)
		{
			mark = true;
		}
		else
			mark = false;
		
		for (i=arr2.GetSize()-1;i>=0;i--)
		{
			GProgressStep();
			//初步过滤掉arr2中明显不想交的地物
			if(!PreProcess1(arr2[i].pFtr,arr1)) continue;			
			
			CGeometry *pGeo = arr2[i].pFtr->GetGeometry();
			const CShapeLine *pSL = pGeo->GetShape();
			if( !pSL )return FALSE;
			CArray<int, int> pos1;
			pSL->GetKeyPosOfBaseLines(pos1);
			CArray<PT_3DEX,PT_3DEX> pts1,arrkey1;		
			pSL->GetPts(pts1);
			pSL->GetKeyPts(arrkey1);
			BOOL bSame = pts1.GetSize()==arrkey1.GetSize()?TRUE:FALSE;
			int nPts1 = pts1.GetSize();
			PT_3D *Pnts1=new PT_3D[nPts1];
			if(!Pnts1)return FALSE;
			memSecur.Add(&Pnts1,TRUE);
			for (int j=0;j<nPts1;j++)//获取点数组
			{
				COPY_3DPT(Pnts1[j],pts1[j]);
			}
			
			BOOL modified = FALSE;
			LinkList L=NULL;
			LNode *p=NULL;
			PT_3D  ret;
			initLList(L);
			
			int len1 = nPts1;			
			for (j=0;j<nPts1-1;j++)
			{
				if(!PreProcess2(Pnts1[j],Pnts1[j+1],arr1))continue;
				int idx = j;
				for (k=arr1.GetSize()-1;k>=0;k--)
				{
					if (arr1[k].flag1==TRUE||arr1[k].flag2==TRUE)continue;
					pSL = arr1[k].pFtr->GetGeometry()->GetShape();
					if( !pSL )return FALSE;
					CArray<int, int> pos2;
					pSL->GetKeyPosOfBaseLines(pos2);
					CArray<PT_3DEX,PT_3DEX> pts2,arrkey2;		
					pSL->GetPts(pts2);
					pSL->GetKeyPts(arrkey2);
					int nPts2 = pts2.GetSize();
					PT_3D *Pnts2=new PT_3D[nPts2];
					if(!Pnts2)return FALSE;
					memSecur.Add(&Pnts2,TRUE);
					for (l=0;l<nPts2;l++)//获取点数组
					{
						COPY_3DPT(Pnts2[l],pts2[l]);
					}
					
					int idx = l;
					for ( l=0;l<nPts2-1;l++)
					{
						int flag=-1;
						if( (flag=GLineIntersectLineSegInLimit1(Pnts1[j].x,Pnts1[j].y,Pnts1[j].z,
							Pnts1[j+1].x,Pnts1[j+1].y,Pnts1[j+1].z,
							Pnts2[l].x,Pnts2[l].y,Pnts2[l].z,Pnts2[l+1].x,
							Pnts2[l+1].y,Pnts2[l+1].z,					
							&ret.x,&ret.y,&ret.z,10e-4,mark))==0 )
							continue;
						else
						{
							if (flag==1)
							{
								addLnode(&L,&(Pnts1[j]),&ret);   //插入链表，排序过程在	addLnode中已经进行
							}
							else if (flag==2)
							{
								Pnts1[j].z = ret.z;
							}
							else if (flag==3)
							{
								Pnts1[j+1].z = ret.z;
							}
							modified = TRUE;
							
						}
					}
					delete []Pnts2;
					Pnts2 = NULL;				
				}
				
				if (bSame)
				{
					for (p=L;p!=NULL;p=p->next)
					{
						insertPoint(&Pnts1,&len1,&nPts1,&(p->pt3d),idx+1);//插入Pnts1数组中
						j++;//相应的循环变量也要增加，以便定位于第二条线段的起点
					}
				}
				else
				{
					for (int m=0; m<pos1.GetSize(); m++)
					{
						if (idx < pos1[m])
						{
							for (p=L;p!=NULL;p=p->next)
							{
								PT_3DEX ptex(p->pt3d,arrkey1[m].pencode);
								arrkey1.InsertAt(m,ptex);
								pos1.InsertAt(m,idx);								
							}
							break;
						}
					}
				}
				
				destroyList(&L);
				
			}
			//添加地物
			if (modified)
			{
				//直接删除原地物

				CFeature *pFtrObj = arr2[i].pFtr;
				backupFtrs.Add(pFtrObj->Clone());
				if (bSame)
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					PT_3DEX PtEX;
					for (int i=0;i<nPts1;i++)
					{
						COPY_3DPT(PtEX,Pnts1[i]);
						PtEX.pencode = penLine;
						
						arr.Add(PtEX);
						
					}
					KickoffSame2DPoints_notPenMove(arr);
					pFtrObj->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());
				}
				else
				{
					KickoffSame2DPoints_notPenMove(arrkey1);
					pFtrObj->GetGeometry()->CreateShape(arrkey1.GetData(),arrkey1.GetSize());
				}

				modifiedFtrs.Add(pFtrObj);
				
			}
			delete []Pnts1;
			Pnts1 = NULL;
		}
ret:	
		GProgressEnd();
	}	

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CDelAllSameCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDelAllSameCommand,CEditCommand)

CDelAllSameCommand::CDelAllSameCommand()
{
	m_nStep = -1;
	m_bCheckLayName = FALSE;
	m_nMode = 0;
}

CDelAllSameCommand::~CDelAllSameCommand()
{
}

CString CDelAllSameCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DELSAME);
}


//比较两个地物是否完全相同
BOOL CDelAllSameCommand::IsWholeOverlapedObj(CDataSourceEx *pDS, CFeature *pObj1, CFeature *pObj2, BOOL bCheckLayName, int mode)
{
	if (!pObj1 || !pObj2)return FALSE;

	//特征码必须相同
	CFtrLayer* pLay1 = pDS->GetFtrLayerOfObject(pObj1);
	CFtrLayer* pLay2 = pDS->GetFtrLayerOfObject(pObj2);
	if (bCheckLayName && pLay1 && pLay2 && stricmp(pLay1->GetName(), pLay2->GetName()) != 0)
		return FALSE;

	CGeometry *pGeo1 = pObj1->GetGeometry();
	CGeometry *pGeo2 = pObj2->GetGeometry();
	if (!pGeo1 || !pGeo2)
		return FALSE;

	//点数必须相同
	if (mode == 0 && pGeo1->GetDataPointSum() != pGeo2->GetDataPointSum())
		return FALSE;

	//类型必须相同
	if (pGeo1->GetClassType() != pGeo2->GetClassType())
		return FALSE;

	CArray<PT_3DEX, PT_3DEX> arrPts1, arrPts2;
	pGeo1->GetShape(arrPts1);
	pGeo2->GetShape(arrPts2);

	int nsz1 = GraphAPI::GKickoffSamePoints(arrPts1.GetData(), arrPts1.GetSize());
	arrPts1.SetSize(nsz1);
	int nsz2 = GraphAPI::GKickoffSamePoints(arrPts2.GetData(), arrPts2.GetSize());
	arrPts2.SetSize(nsz2);

	if (nsz1 == 1 && nsz2 == 1)//点，文本
	{
		if (!GraphAPI::GIsEqual3DPoint(&arrPts1[0], &arrPts2[0]))
			return FALSE;
		if (pGeo1->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CString text1 = ((CGeoText*)pGeo1)->GetText();
			CString text2 = ((CGeoText*)pGeo2)->GetText();
			if (0 != text1.CompareNoCase(text2))
				return FALSE;
		}
		return TRUE;
	}

	if (mode == 1 && nsz1 > 1 && nsz2 > 1)//剔除直线中间的点
	{
		CArray<PT_3DEX,PT_3DEX> temp;
		int i=0, a=1;
		double ang;
		for(i=0; i<nsz1-2; )
		{
			if(i+a+1>=nsz1) break;
			ang=GraphAPI::GGetIncludedAngle(arrPts1[i], arrPts1[i+a], arrPts1[i+a], arrPts1[i+a+1]);
			if(fabs(ang)<0.0001)//共线
			{
				a++;
			}
			else
			{
				temp.Add(arrPts1[i]);
				i+=a;
				a=1;
			}
		}
		temp.Add(arrPts1[i]);
		temp.Add(arrPts1[i+a]);
		if(i+a+1<nsz1)  temp.Add(arrPts1[i+a+1]);
		arrPts1.Copy(temp);
		nsz1 = arrPts1.GetSize();

		a = 1;
		temp.RemoveAll();
		for(i=0; i<nsz2-2; )
		{
			if(i+a+1>=nsz2) break;
			ang=GraphAPI::GGetIncludedAngle(arrPts2[i], arrPts2[i+a], arrPts2[i+a], arrPts2[i+a+1]);
			if(fabs(ang)<0.0001)//共线
			{
				a++;
			}
			else
			{
				temp.Add(arrPts2[i]);
				i+=a;
				a=1;
			}
		}
		temp.Add(arrPts2[i]);
		temp.Add(arrPts2[i+a]);
		if(i+a+1<nsz2)  temp.Add(arrPts2[i+a+1]);
		arrPts2.Copy(temp);
		nsz2 = arrPts2.GetSize();
	}

	if( nsz1!=nsz2 )
		return FALSE;

	if( nsz1<=0 )
		return FALSE;

	double toler = GraphAPI::g_lfDisTolerance;
	
	//当两地物都是面或闭合曲线时
	if( pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && pGeo2->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) &&
		((CGeoCurveBase*)pGeo1)->IsClosed() && ((CGeoCurveBase*)pGeo2)->IsClosed() && nsz1>2 )
	{
		//去掉闭合点
		int npt = nsz1-1;

		PT_3DEX expt1, expt2, expt3;
		expt1 = arrPts1[0];
		int i;
		for ( i=0;i<npt;i++)
		{
			expt2 = arrPts2[i];
			if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
			{
				break;
			}
		}
		if (i>=npt)
		{
			return FALSE;
		}
		else
		{
			//顺序检验
			{		
				for( int j=1; j<npt; j++ )
				{
					expt1 = arrPts1[j];
					expt2 = arrPts2[(i+j)%npt];
										
					if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
					{
					}
					else
					{
						break;
					}
				}
				if( j<npt ) ;
				else 
					return TRUE;
			}
			
			//反序检验			
			{
				for( int j=1; j<npt; j++ )
				{
					expt1 = arrPts1[j];
				    expt2 = arrPts2[(i+npt-j)%npt];
					
					if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
					{
					}
					else
					{
						break;
					}
				}
				if( j<npt )return FALSE;
				else 
					return TRUE;

			}
		}

	}
	else
	{	
		//首尾端点必须相同
		int npt = nsz1;
		PT_3DEX expt1, expt2, expt3;
		BOOL bSame1 = FALSE, bSame2 = FALSE;
		
		expt1 = arrPts1[0];
		expt2 = arrPts2[0];
		expt3 = arrPts2[npt-1];
		if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
		{
			bSame1 = TRUE;
		}
		if( (fabs(expt1.x-expt3.x)<toler && fabs(expt1.y-expt3.y)<toler) )
		{
			bSame2 = TRUE;
		}

		if( !bSame1 && !bSame2 )
			return FALSE;

		//其他节点必须相同
		if( bSame1 )
		{
			//顺序比较
			for( int i=1; i<npt; i++ )
			{
				expt1 = arrPts1[i];
				expt2 = arrPts2[i];

				if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
				{
				}
				else
				{
					break;
				}
			}
			if( i<npt )return FALSE;
			else
				return TRUE;
		}
		else
		{
			//反序比较
			for( int i=1; i<npt; i++ )
			{
				expt1 = arrPts1[i];
				expt2 = arrPts2[npt-1-i];			

				if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
				{
				}
				else
				{
					break;
				}
			}
			if( i<npt )return FALSE;
			else
				return TRUE;
		}
	}

	return TRUE;
}

int  CDelAllSameCommand::SelectKeepedOne(CArray<CFeature*,CFeature*>& arrObjs)
{	
	return 0;
}

void CDelAllSameCommand::Process()
{
	CDlgDataSource *pDS = ((CDlgDoc*)m_pEditor)->GetDlgDataSource();
	if(!pDS) return;

	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayers, arr);
	
	//计数
	int num = pDS->GetFtrLayerCount();
	CArray<CFeature*, CFeature*> arrObjs, arrObjs2;

	int i,j, lSum = 0, nObj;
	for ( i=0; i<arr.GetSize(); i++)
	{
		CFtrLayer *pLayer = arr[i];
		nObj = pLayer->GetObjectCount();
		for( j=0; j<nObj; j++ )
		{
			CFeature *pObj = pLayer->GetObject(j);
			if( !pObj )continue;

			arrObjs.Add(pObj);

			lSum++;
		}
	}
	
	GProgressStart(lSum);

	CUndoFtrs undo(m_pEditor,Name());

	BOOL bMsgBox = FALSE;
	nObj = arrObjs.GetSize();
	//遍历处理
	for ( i=0; i<nObj; i++)
	{
		CFeature *pObj = arrObjs[i];
		
		GProgressStep();
			
		if( pObj->IsDeleted() )continue;
			
		//找到相同的地物
		arrObjs2.RemoveAll();
		arrObjs2.Add(pObj);
			
		for( j=i+1; j<nObj; j++)
		{
			CFeature *pObj1 = arrObjs[j];
			if( pObj1->IsDeleted() )continue;

			if( IsWholeOverlapedObj(pDS,pObj,pObj1,m_bCheckLayName, m_nMode) )
			{
				arrObjs2.Add(pObj1);
			}
		}
			
		if( arrObjs2.GetSize()>1 )
		{			
			if( !bMsgBox && AfxMessageBox(IDS_CMDTIP_DELSAME,MB_OKCANCEL)!=IDOK )
			{
				Abort();
				m_nExitCode = CMPEC_STARTOLD;
				GProgressEnd();
				return;
			}

			bMsgBox = TRUE;

			//选择要保留的地物
			int nSel = SelectKeepedOne(arrObjs2);
			if( nSel>=0 )
			{
				arrObjs2.RemoveAt(nSel);
				
				//其他的全部删除
				for( j=0; j<arrObjs2.GetSize(); j++)
				{
					m_pEditor->DeleteObject(FtrToHandle(arrObjs2[j]),FALSE);
					undo.arrOldHandles.Add(FtrToHandle(arrObjs2[j]));
				}
			}
		}
	}

	undo.Commit();
	
	GProgressEnd();
	
	CString strFormat, strMsg;
	strFormat.LoadString(IDS_CMDTIP_DELSAMEOK);
	strMsg.Format(strFormat,lSum,undo.arrOldHandles.GetSize());
	AfxMessageBox(strMsg);
	
}

void CDelAllSameCommand::Start()
{	
	if( !m_pEditor || !((CDlgDoc*)m_pEditor)->GetDlgDataSource() )
	{
		Abort();
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
	
	Process();
	Finish();
	m_nExitCode = CMPEC_STARTOLD;
}

template<class runtimeCLass>
runtimeCLass * getView(CDocument * pdoc);
#include "mm3dprj\OsgbView.h"
//////////////////////////////////////////////////////////////////////
// CSplitSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSplitSurfaceCommand,CEditCommand)

CSplitSurfaceCommand::CSplitSurfaceCommand()
{
	m_nStep = -1;
	m_pDrawProc = NULL;
	m_pGeoCurve = NULL;
	m_bKeepSplitLine = TRUE;
	m_bPlaneFirst = FALSE;
	m_bKeepSrcVertexElevtion = FALSE;
	m_nOperMode = 1;
}

CSplitSurfaceCommand::~CSplitSurfaceCommand()
{
	if( m_pDrawProc )
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
}

CString CSplitSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SPLITSURFACE);
}

void CSplitSurfaceCommand::Start()
{
 	if( !m_pEditor )return;

	m_nStep = 0;

	CEditCommand::Start();

	if(m_nOperMode == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure();
		m_pDrawProc->Init(m_pEditor);

		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = m_pGeoCurve;
		UpdateParams(FALSE);

		int num;
		m_pEditor->GetSelection()->GetSelectedObjs(num);
		if(num>0)
		{
			m_nStep = 1;
			m_pDrawProc->Start();
		}
	}
}

void CSplitSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SplitSurfaceCommand",Name());

	param->BeginOptionParam("OperMode",StrFromResID(IDS_CMDPLANE_RIVERFILLTYPE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SELECT),0,' ',m_nOperMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),1,' ',m_nOperMode==1);
	param->EndOptionParam();

	if( m_nOperMode==1)
	{
		param->AddParam(PF_TRACKLINEACCKEY,'w',StrFromResID(IDS_CMDPLANE_TRACKLINE));
		param->AddParam("KeepSplitLine", (bool)m_bKeepSplitLine, StrFromResID(IDS_KEEPSPLITLINE));
		if(m_bKeepSplitLine)
		{
			param->AddLayerNameParam("StoreLayer", m_strStoreLayer,StrFromResID(IDS_PLANENAME_PLACELAYER),NULL,LAYERPARAMITEM_LINE);
		}
	}
	if (getView<COsgbView>(PDOC(m_pEditor)))
	{
		param->AddParam("PlaneFirst", (bool)m_bPlaneFirst, StrFromResID(IDS_PLANEFIRST));
		param->AddParam("KeepVertexElevation", (bool)m_bKeepSrcVertexElevtion, StrFromResID(IDS_KEEPSRCVERTEXELEVATION));
		
	}
}

void CSplitSurfaceCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (long)(m_nOperMode);
	tab.AddValue("OperMode",&CVariantEx(var));

	var = (bool)(m_bKeepSplitLine);
	tab.AddValue("KeepSplitLine",&CVariantEx(var));

	var = (LPCTSTR)(m_strStoreLayer);
	tab.AddValue("StoreLayer",&CVariantEx(var));
	if (getView<COsgbView>(PDOC(m_pEditor)))
	{
		var = (bool)(m_bPlaneFirst);
		tab.AddValue("PlaneFirst", &CVariantEx(var));
		var = (bool)(m_bKeepSrcVertexElevtion);
		tab.AddValue("KeepVertexElevation", &CVariantEx(var));
		
	}
}

void CSplitSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"OperMode",var) )
	{					
		m_nOperMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}
	if( tab.GetValue(0,PF_TRACKLINEACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'w'||ch == 'W')
		{
			if(m_nOperMode==1)
			{
				int num;
				const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
				if(num<=0)
				{
					AfxMessageBox(StrFromResID(IDS_SELECT_SURFACE_FIRST));
					Abort();
					return;
				}
			}
			m_pDrawProc->TrackLine();
		}
	}
	if( tab.GetValue(0,"KeepSplitLine",var) )
	{
		m_bKeepSplitLine = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"StoreLayer",var) )
	{
		m_strStoreLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "PlaneFirst", var))
	{
		m_bPlaneFirst = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "KeepVertexElevation", var))
	{
		m_bKeepSrcVertexElevtion = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CSplitSurfaceCommand::Abort()
{
	m_nStep = 0;
	CEditCommand::Abort();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CSplitSurfaceCommand::Finish()
{
	m_pEditor->RefreshView();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	CEditCommand::Finish();
}

CProcedure* CSplitSurfaceCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nOperMode==1&&(nMsgType==msgEnd||nMsgType==msgPtMove ||nMsgType==msgBack))
	{
		return m_pDrawProc;
	}
	return NULL;
}

void CSplitSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if(m_nOperMode==0)
	{
		if( !CanGetSelObjs(flag) )return;

		int num=0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		CGeoArray arr, arr1;
		for(int i=0; i<num; i++)
		{
			CGeometry *pObj = HandleToFtr(handles[i])->GetGeometry();
			if(!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;

			arr.Add(pObj);
		}

		CUndoFtrs undo(m_pEditor, Name());
		for(int j=0; j<num; j++)
		{
			CGeometry *pGeo = HandleToFtr(handles[j])->GetGeometry();
			if(pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				arr1.RemoveAll();
				for(i=0; i<arr.GetSize(); i++)
				{
					arr1.Add(arr[i]->Clone());
				}
				Split(HandleToFtr(handles[j]), arr1, undo);
			}
		}
		undo.Commit();
		Finish();
	}
	else if(m_nOperMode==1)
	{
		if(m_nStep==0)
		{
			int num;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if (num <= 0) return;

			m_nStep = 1;
			m_pDrawProc->Start();//启动绘线
		}
		else if(m_nStep==1)
		{
			if (m_pDrawProc)
			{
				GotoState(PROCSTATE_PROCESSING);
				m_pDrawProc->PtClick(pt,flag);
			}
		}
	}

	CEditCommand::PtClick(pt,flag);
}

void CSplitSurfaceCommand::PtReset(PT_3D &pt)
{
	if(!m_pEditor || !m_pDrawProc || m_pDrawProc->m_arrPts.GetSize()<2)
	{
		Abort();
		return;
	}
	m_pEditor->UpdateDrag(ud_ClearDrag,NULL);
	m_pDrawProc->PtReset(pt);

	int num;
	const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if(num<=0)
	{
		Abort();
		return;
	}

	CUndoFtrs undo(m_pEditor, Name());
	for(int i=0; i<num; i++)
	{
		CGeometry *pGeo = HandleToFtr(handles[i])->GetGeometry();
		if(!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			continue;
		if (!((CGeoCurveBase*)pGeo)->IsClosed())
			continue;

		CGeoArray arr;
		arr.Add(m_pGeoCurve->Clone());
		Split(HandleToFtr(handles[i]), arr, undo);
	}
	undo.Commit();

	m_pEditor->RefreshView();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	Finish();
	m_nStep = 3;
}

#include "DlgCommand2.h"
extern bool IsSurfaceInclude(CFeature* pFtr0, CFeature* pFtr1);
extern int GetCurveIntersectCurve(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2, CArray<PtIntersect,PtIntersect> &arrIntersectPts);
//点是否在线串上
static bool IsPtInCurve(PT_3DEX pt, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	for(int i=0; i<arrPts.GetSize()-1; i++)
	{
		if(GraphAPI::GIsPtInLine(arrPts[i], arrPts[i+1], pt))
		{
			return true;
		}
	}
	return false;
}

double get_min_fabs_no_zero(double a, double b, double c, double _default)
{
	double f[3] = {
		fabs(a),
		fabs(b),
		fabs(c)
	};
	int _idx = 0;
	while (f[_idx] == 0) { _idx++; if (_idx > 2) return _default; }
	int i = _idx + 1;
	for (; i < 3; i ++)
	{
		if (f[i] < f[_idx]) _idx = i;
	}
	return f[_idx];
}

class PlaneOrModel
{
	double a, b, c, d;
	double sa2b2c2;
	COsgbView * m_pview;
	void setsa2b2c2() {
		sa2b2c2 = sqrt(a * a + b * b + c * c);
	}
	struct PT_3D_cmp:public PT_3D
	{
		PT_3D_cmp(const PT_3D & s)
		{
			x = s.x;
			y = s.y;
			z = s.z;
		}
		PT_3D_cmp(const PT_3DEX & s)
		{
			x = s.x;
			y = s.y;
			z = s.z;
		}
		bool operator== (const PT_3D_cmp & s) const 
		{
			return s.x == x && s.y == y && s.z == z;
		}
		bool operator<(const PT_3D_cmp & s) const
		{
			if (x == s.x)
			{
				if (y == s.y)
				{
					return z < s.z;
				}
				else
					return y < s.y;
			}
			else
				return x < s.x;
		}
	};
	bool m_resetSrcPts;
	std::set<PT_3D_cmp> srcPtSet;
public:
	PlaneOrModel() :a(0), b(0), c(0), d(0), m_pview(nullptr), m_resetSrcPts(false){ sa2b2c2 = 0; }
	PlaneOrModel(double a, double b, double c, double d):a(a),b(b),c(c),d(d) {
		setsa2b2c2();
	}
	void setSrcPts(CArray<PT_3DEX, PT_3DEX> & arrPts)
	{
		if (arrPts.GetCount() >= 0)
		{
			m_resetSrcPts = true;
			for (int i = 0; i < arrPts.GetSize(); i++)
			{
				srcPtSet.insert(arrPts[i]);
			}
		}
	}
	void setCOsgbView(COsgbView * p) { m_pview = p; }
	void clear() { a = 0; b = 0; c = 0; sa2b2c2 = 0; }
	bool isValid() { return (sa2b2c2 > 0) || m_pview; }
	bool isModel() { return m_pview; }
	void createFrom3Point(const PT_3D & p1, const PT_3D & p2, const PT_3D & p3)
	{
		a = ((p2.y - p1.y)*(p3.z - p1.z) - (p2.z - p1.z)*(p3.y - p1.y));
		b = ((p2.z - p1.z)*(p3.x - p1.x) - (p2.x - p1.x)*(p3.z - p1.z));
		c = ((p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x));

		double nd = get_min_fabs_no_zero(a, b, c, 1);
		a /= nd;
		b /= nd;
		c /= nd;
		d = (0 - (a*p1.x + b*p1.y + c*p1.z));
		setsa2b2c2();
	}
	double pt_to(const PT_3D & pt)
	{
		if (m_pview)
		{
			auto zs = m_pview->getLinePick(pt.x, pt.y);
			if (zs.size() > 0)
			{
				double z = zs.back();
				return fabs(z - pt.z);
			}
			return -1;
		}
		if (sa2b2c2 == 0) return -1;
		double a1 = pt.x * a + pt.y * b + pt.z * c + d;
		if (a1 == 0) return 0;
		return fabs(a1) / sa2b2c2;
	}
	bool pt_in(const PT_3D & pt, double accept_error)
	{
		if (m_pview)
		{
			auto zs = m_pview->getLinePick(pt.x, pt.y);
			if (zs.size() > 0)
			{
				double z = zs.back();
				return fabs(z - pt.z) <= accept_error;
			}
			return false;
		}
		double a1 = pt.x * a + pt.y * b + pt.z * c + d;
		if (a1 == 0) return true;
		if (accept_error == 0) return false;
		a1 = fabs(a1);
		return a1 <= accept_error;
	}
	void reset_pt_z(PT_3D & pt)
	{
		if (m_resetSrcPts && !srcPtSet.empty())
		{
			if (srcPtSet.find(pt) != srcPtSet.end()) 
				return;
		}
		if (m_pview)
		{
			auto zs = m_pview->getLinePick(pt.x, pt.y);
			if (zs.size() > 0)
			{
				double z = zs.back();
				if (z != 0)
				{
					pt.z = z;
					return;
				}
			}
		}
		if (c == 0) return;
		double nz = -(pt.x * a + pt.y * b + d) / c;
		pt.z = nz;
	}
	bool pt_projection(PT_3D & pt)
	{
		reset_pt_z(pt);
// 		double a1 = pt.x * a + pt.y * b + pt.z * c + d;
// 		if (a1 == 0) return false;
// 		double t = a1 / (sa2b2c2 * sa2b2c2);
// 		pt.x -= a * t;
// 		pt.y -= b * t;
// 		pt.z -= c * t;
		return true;
	}
};
template<class PTA, class PTB, class PTC>
bool ptsInLine2D(const PTA & a, const PTB & b, const PTC & c, double err = 0.1)
{
	double dx_ab = a.x - b.x;
	double dy_ab = a.y - b.y;

	double dx_ac = a.x - c.x;
	double dy_ac = a.y - c.y;
	if (dx_ac == 0)
	{

		if (dx_ab == 0) 
			return true;
		return false;
	}
	if (dy_ac == 0)
	{
		if (dy_ab == 0) 
			return true;
		return false;
	}
	double xc = dx_ab / dx_ac;
	double yc = dy_ab / dy_ac;
	if (xc == yc) return true;
	if (err == 0 || (xc * yc < 0)) return false;
	double mc = (std::max)(xc, yc);
	xc /= mc;
	yc /= mc;
	double ec = fabs(xc - yc);
	return ec <= err;

}

void CSplitSurfaceCommand::Split(CFeature *pFtr, CGeoArray& curves, CUndoFtrs& undo)
{
	if(!pFtr || curves.GetSize()<=0) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	
	CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
	if(!pLayer) return;
	CFeature *pOldFtr = pFtr->Clone();
	CFtrArray oldftrs;
	oldftrs.Add(pOldFtr);//被分割的面
	COsgbView * pView = getView<COsgbView>(PDOC(m_pEditor));
	PlaneOrModel plane;
	if (pView)
	{
		bool setModel = true;
		CArray<PT_3DEX, PT_3DEX> arrPts;
		if (m_bKeepSrcVertexElevtion)
		{
			pFtr->GetGeometry()->GetShape(arrPts);
			plane.setSrcPts(arrPts);
		}

		if (m_bPlaneFirst)
		{
			//被分割的面所在的平面(取前三个点构成)
			if (arrPts.IsEmpty())
				pFtr->GetGeometry()->GetShape(arrPts);
			if (arrPts.GetSize() >= 3)
			{
				PT_3D pts[3];
				COPY_3DPT(pts[0], arrPts[0]);
				int _Idx = 0;
				for (int i = 1; i < arrPts.GetSize(); i++)
				{

					if (
						//与前一点相近的点
						(DIST_3DPT(pts[_Idx], arrPts[i]) < 0.1)
						// 与前两点在同一直线上的点
						|| ((_Idx == 1) && ptsInLine2D(pts[0], pts[1], arrPts[i]))
						)
					{
						// 点之间尽量分散
					}
					else
						_Idx++;

					COPY_3DPT(pts[_Idx], arrPts[i]);
					if (_Idx == 2) break;
				}
				if (_Idx == 2)
				{
					plane.createFrom3Point(pts[0], pts[1], pts[2]);
					setModel = false;
				}
			}
		}
		if (setModel)
			plane.setCOsgbView(pView);

	}
	
	bool need_reset_pts = false;
	for(int n=0; n<curves.GetSize(); n++)
	{
		CFeature *pFtr1 = pFtr->Clone();
		pFtr1->SetGeometry(curves[n]);
		pFtr1->SetID(OUID());
		oldftrs.Add(pFtr1);//分割线

		// 判断是否所有分割线的点在平面上
		if (pView && (!need_reset_pts) && plane.isValid())
		{
			if (plane.isModel())
			{
				need_reset_pts = true;
				continue;
			}
			CArray<PT_3DEX, PT_3DEX> arrPts;
			curves[n]->GetShape(arrPts);
			for (int i = 0; i < arrPts.GetSize(); i++)
			{
				if (plane.pt_in(arrPts[i],0.1)) continue;
				need_reset_pts = true;
				break;
			}
		}
	}
	if (need_reset_pts)
	{
		CArray<PT_3DEX, PT_3DEX> arrPts;
		pOldFtr->GetGeometry()->GetShape(arrPts);
		bool need_reset_ftr = false;
		for (int i = 0; i < arrPts.GetSize(); i++)
			need_reset_ftr |= plane.pt_projection(arrPts[i]);
		// 把原面设为一个平面
		if (need_reset_ftr)
		{
			pOldFtr->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());
		}
	}
	//调用非打断构面
	CTopoSurfaceNoBreakCommand cmd;
	cmd.Init(m_pEditor);
	CFtrArray newftrs;//存放旧地物
	cmd.BuildSurface(oldftrs, newftrs);
	
	CGeometry *pOldObj = pFtr->GetGeometry();
	BOOL bFill = ((CGeoCurveBase*)pOldObj)->IsFillColor();
	COLORREF color = ((CGeoCurveBase*)pOldObj)->GetColor();
	COLORREF fillcolor = ((CGeoCurveBase*)pOldObj)->GetFillColor();
	DWORD transparency = ((CGeoCurveBase*)pOldObj)->GetTransparency();

	CFtrArray arrnews;

	int nObj = newftrs.GetSize(), i, j, k;
	for(i=0; i<nObj; i++)
	{
		CFeature* pNewFtr = newftrs[i];

		//筛选被原面包含的面
		PT_3D opt;
		pNewFtr->GetGeometry()->GetCenter(NULL, &opt);
		if (!pOldFtr->GetGeometry()->bPtIn(&opt))
		{
			continue;
		}
		
		CFeature *pFtr0 = pFtr->Clone();
		pFtr0->SetID(OUID());
		pFtr0->SetAppFlag(0);

		CArray<PT_3DEX, PT_3DEX> arrPts;
		pNewFtr->GetGeometry()->GetShape(arrPts);
	
		CGeometry *pObj = pFtr0->GetGeometry();

		if (need_reset_pts)
		{
			for (int idx = 0; idx < arrPts.GetSize(); idx++)
			{
				PT_3DEX & pt = arrPts[idx];
				// 不在平面上的点重置z值到平面上
				if (plane.pt_to(pt) > 0)
				{
					plane.reset_pt_z(pt);
				}
				
			}
		}
		if(!pObj) continue;
		if( !pObj->CreateShape(arrPts.GetData(), arrPts.GetSize()) )
		{
			delete pFtr0;
			continue;
		}
		pObj->SetColor(color);
		if(bFill)
		{
			((CGeoCurveBase*)pObj)->EnableFillColor(bFill, fillcolor);
			((CGeoCurveBase*)pObj)->SetTransparency(transparency);
		}
		
		if( !m_pEditor->AddObject(pFtr0,pLayer->GetID()) )
		{
			delete pFtr0;
			continue;
		}
		arrnews.Add(pFtr0);
		GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtr0);
		undo.AddNewFeature(FtrToHandle(pFtr0));
	}
	
	m_pEditor->DeleteObject(FtrToHandle(pFtr));
	undo.AddOldFeature(FtrToHandle(pFtr));

	for(i=0; i<newftrs.GetSize(); i++)
	{
		delete newftrs[i];
	}

	if(m_nOperMode==1 && m_bKeepSplitLine && !m_strStoreLayer.IsEmpty())
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(m_strStoreLayer);
		if(!pLayer)
		{
			pLayer = new CFtrLayer();
			pLayer->SetName(m_strStoreLayer);
			m_pEditor->AddFtrLayer(pLayer);
		}

		for(i=0; i<curves.GetSize(); i++)
		{
			CArray<PT_3DEX,PT_3DEX> pts, pts1;
			curves[i]->GetShape(pts);
			int nPt = pts.GetSize();
			if(nPt<2) continue;

			//插入交点
			CArray<PtIntersect,PtIntersect> arr;
			for(j=0; j<arrnews.GetSize(); j++)
			{
				CArray<PT_3DEX,PT_3DEX> pts1;
				arrnews[j]->GetGeometry()->GetShape(pts1);
				GetCurveIntersectCurve(pts.GetData(), pts.GetSize(), pts1.GetData(), pts1.GetSize(), arr);
			}

			for(j=0; j<nPt; j++)
			{
				for(k=0; k<arr.GetSize(); k++)
				{
					if(arr[k].lfRatio<j &&arr[k].IntersectFlag<0)
					{
						PT_3DEX pt0(arr[k].pt, penLine);
						pts1.Add(pt0);
						arr[k].IntersectFlag = 1;
					}
				}
				pts1.Add(pts[j]);
			}

			nPt = GraphAPI::GKickoffSame2DPoints(pts1.GetData(), pts1.GetSize());
			pts1.SetSize(nPt);

			CArray<int,int> item;
			item.SetSize(nPt-1);
			for(j=0; j<nPt-1; j++)
			{
				item[j] = 0;
			}
			for(j=0; j<nPt-1; j++)
			{
				PT_3DEX ptcen;
				ptcen.x = (pts1[j].x+pts1[j+1].x)/2;
				ptcen.y = (pts1[j].y+pts1[j+1].y)/2;
				ptcen.z = (pts1[j].z+pts1[j+1].z)/2;
				
				for(int k=0; k<arrnews.GetSize(); k++)
				{
					CArray<PT_3DEX,PT_3DEX> pts2;
					arrnews[k]->GetGeometry()->GetShape(pts2);
					if(IsPtInCurve(ptcen, pts2))
					{
						item[j] = 1;
						break;
					}
				}
			}
			j=0;
			while(j<nPt-1)
			{
				for(; j<nPt-1 && item[j]==0 ; j++);

				CArray<PT_3DEX,PT_3DEX> pts2;
				for(; j<nPt-1 && item[j]==1; j++)
				{
					pts2.Add(pts1[j]);
				}
				pts2.Add(pts1[j]);
				if(pts2.GetSize()>1)
				{
					CFeature *pNew = pLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOCURVE);
					pNew->GetGeometry()->CreateShape(pts2.GetData(), pts2.GetSize());
					m_pEditor->AddObject(pNew, pLayer->GetID());
					undo.AddNewFeature(FtrToHandle(pNew));
				}
			}
		}
	}
}

bool GLineIntersectLineSegInLimit(double x0,double y0, double x1, double y1,double x2,double y2,double x3,double y3, double *x, double *y, double *ret_t1, double *ret_t2, double toler)
{
	double vector1x = x1-x0, vector1y = y1-y0;
	double vector2x = x3-x2, vector2y = y3-y2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if( delta<1e-10 && delta>-1e-10 )return false;
	double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
	double xr = x0 + t1*vector1x, yr = y0 + t1*vector1y;
	double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
	if( t1<0 || t1>1 )
	{
		if( _FABS(xr-x0)<=toler && _FABS(yr-y0)<=toler ) t1=0.0;
		else if( _FABS(xr-x1)<=toler && _FABS(yr-y1)<=toler ) t1=1.0;
		else
			return false;
	}
	if( t2<0 || t2>1 )
	{
		if( _FABS(xr-x2)<=toler && _FABS(yr-y2)<=toler ) t2=0.0;
		else if( _FABS(xr-x3)<=toler && _FABS(yr-y3)<=toler ) t2=1.0;
		else
			return false;
	}
	if(x) *x = xr;
	if(y) *y = yr;
	if(ret_t1) *ret_t1 = t1;
	if(ret_t2) *ret_t2 = t2;
	return true;
}

//////////////////////////////////////////////////////////////////////
// CXORSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CXORSurfaceCommand,CEditCommand)

CXORSurfaceCommand::CXORSurfaceCommand()
{
    m_nStep = -1;
}

CXORSurfaceCommand::~CXORSurfaceCommand()
{

}

CString CXORSurfaceCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_XORSURFACE);
}

void CXORSurfaceCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	GOutPut(StrFromResID(IDS_INFO_XORSUFACE1));
	m_pEditor->OpenSelector(SELMODE_MULTI);
}

void CXORSurfaceCommand::Abort()
{	
	CEditCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CXORSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	int num ;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( m_nStep==0 )
	{
		if( num==1 )
		{
			GOutPut(StrFromResID(IDS_INFO_XORSUFACE2));
		}
		else if( num==2 )
		{	
			if( !CanGetSelObjs(flag,FALSE) )return;
			CGeometry *pObj1 = HandleToFtr(handles[0])->GetGeometry();
			CGeometry *pObj2 = HandleToFtr(handles[1])->GetGeometry();
			
			if( pObj1==NULL || pObj2==NULL )
				return;
			
			if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				return;
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;
			m_pEditor->CloseSelector();

			return;
		}
		
	}
	
	if( m_nStep==1 && num==2 )
	{
		CGeoSurface *pObj1 = (CGeoSurface*)HandleToFtr(handles[1])->GetGeometry();
		CGeoSurface *pObj2 = (CGeoSurface*)HandleToFtr(handles[0])->GetGeometry();

		CArray<CGeoSurface*,CGeoSurface*> arr;
		BOOL bRet = XORSurface(pObj1, pObj2, arr);
		if(!bRet)
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor,Name());
		for(int i=0; i<arr.GetSize(); i++)
		{
			CFeature* pFtr = HandleToFtr(handles[1])->Clone();
			pFtr->SetID(OUID());
			pFtr->SetGeometry(arr[i]);
			
			m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(handles[1]));
			
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[1]),pFtr);
			undo.arrNewHandles.Add(FtrToHandle(pFtr));
		}
		
		m_pEditor->DeleteObject(handles[1]);

		undo.arrOldHandles.Add(handles[1]);
		
		undo.Commit(); 
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		Finish();
		m_nStep = 3;
	}

	CEditCommand::PtClick(pt,flag);
}


CGeoSurface *NormalizeSurface(CEditor *pEditor, CGeoSurface *pGeo);

BOOL CXORSurfaceCommand::XORSurface(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arr)
{
	if(1)
	{
		if( !m_pEditor )return FALSE;
		
		if( pObj1==NULL || pObj2==NULL )
			return FALSE;
		
		if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			return FALSE;
		
		GotoState(PROCSTATE_PROCESSING);
		
		
		gsbh::GeoSurfaceBooleanHandle gsbhandle;
		if (!gsbhandle.setSurface(pObj1, pObj2))
			return FALSE;
		
		//
		if(!gsbhandle.init())
			return FALSE;
		
		//
		gsbhandle.difference();
		
		
		std::vector<CGeoSurface*> res;
		gsbhandle.getResult(res);
		for(int i=0; i<res.size(); ++i)
		{
			CGeoSurface* pNewObj = res[i];
			if(pNewObj == NULL)
				return FALSE;
			//
			CArray<PT_3DEX, PT_3DEX> temp_shape;
			pNewObj->GetShape(temp_shape);
			pNewObj->CopyFrom(pObj1);
			pNewObj->CreateShape(temp_shape.GetData(),temp_shape.GetSize());
			pNewObj->EnableClose(TRUE);
			
		    arr.Add(pNewObj);		
		}
// 		CGeoSurface* pNewObj = gsbhandle.getResult();
// 		if(pNewObj == NULL)
// 			return FALSE;
// 		//
// 		CArray<PT_3DEX, PT_3DEX> temp_shape;
// 		pNewObj->GetShape(temp_shape);
// 		pNewObj->CopyFrom(pObj1);
// 		pNewObj->CreateShape(temp_shape.GetData(),temp_shape.GetSize());
// 		pNewObj->EnableClose(TRUE);
// 
// 		arr.Add(pNewObj);		
		
		return TRUE;

	}

    if( !m_pEditor )return FALSE;
	
	if( pObj1==NULL || pObj2==NULL )
		return FALSE;
	
	if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		return FALSE;
	
	GotoState(PROCSTATE_PROCESSING);

	CGeoSurface *pObj1_new = NormalizeSurface(m_pEditor,pObj1);
	CGeoSurface *pObj2_new = NormalizeSurface(m_pEditor,pObj2);
	if(pObj1_new==NULL)
		pObj1_new = pObj1;

	if(pObj2_new==NULL)
		pObj2_new = pObj2;

	gsbh::GeoSurfaceBooleanHandle gsbhandle;
	if (!gsbhandle.setSurface(pObj1_new,pObj2_new))
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}

	//
	if(!gsbhandle.init())
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}

	//
	gsbhandle.difference();

	if(pObj1_new!=pObj1)delete pObj1_new;
	if(pObj2_new!=pObj2)delete pObj2_new;

	CGeoSurface* pNewObj = gsbhandle.getResult();
	if(pNewObj == NULL)
		return FALSE;
	//
	CArray<PT_3DEX, PT_3DEX> temp_shape;
	pNewObj->GetShape(temp_shape);
	pNewObj->CopyFrom(pObj1);
	pNewObj->CreateShape(temp_shape.GetData(),temp_shape.GetSize());
	pNewObj->EnableClose(TRUE);

	arr.Add(pNewObj);		

	return TRUE;
}

BOOL CXORSurfaceCommand::XORSurface_BothSimple(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arr)
{
	CArray<PT_3DEX,PT_3DEX> pts1,pts2;
	pObj1->GetShape()->GetPts(pts1);
	pObj2->GetShape()->GetPts(pts2);
	int i=0,j=0;
	
	PolygonIntersect::CPolygonWork pw;		
	
	for( i=0; i<pts1.GetSize(); i++)
	{
		pw.add_point(0,pts1[i].x,pts1[i].y,pts1[i].z);
	}
	
	for( i=0; i<pts2.GetSize(); i++)
	{
		if(i!=0 && pts2[i].pencode==penMove) break;//先简单处理
		pw.add_point(1,pts2[i].x,pts2[i].y,pts2[i].z);
	}
	
	pw.set_overlap(GraphAPI::GetDisTolerance());
	pw.initialize();
	
	vector< vector<PolygonIntersect::tpoint> > *pResult = NULL;
	
	//取补集
	pw.find_XOR(0,1,1);
	pResult = &pw.xor_list;
	
	int error = pw.GetError();
	if( error==PolygonIntersect::CPolygonWork::NONE )
	{
		if( pResult->size()<=0 )
		{
			return FALSE;
		}
		
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
					arrNewPts.Add(expt);
				}

				CGeoSurface *pGeo = new CGeoSurface();
				if(pGeo && pGeo->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize()) )
				{
					pGeo->EnableClose(TRUE);
					arr.Add(pGeo);
				}
				else
				{
					delete pGeo;
				}
			}
		}
		return TRUE;
	}
	else if(error==PolygonIntersect::CPolygonWork::OVERLAP_EXACT )
	{
		//相交了，结果为空
		return FALSE;
	}
	else if(error==PolygonIntersect::CPolygonWork::NO_INTERSECTION)
	{
		PT_3D pt0;
		COPY_3DPT(pt0, pts2[0]);
		if(pObj1->bPtIn(&pt0))
		{
			CGeoMultiSurface *pMS = new CGeoMultiSurface();
			pMS->AddSurface(pts1);
			pMS->AddSurface(pts2);
			arr.Add(pMS);
			return TRUE;
		}
	}

	return FALSE;
}

//暂时不考虑内边界
BOOL CXORSurfaceCommand::XORSurface_Muti_Simple(CGeoMultiSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arr)
{
	CArray<PT_3DEX,PT_3DEX> pts1,pts2;
	pObj1->GetShape()->GetPts(pts1);
	pObj2->GetShape()->GetPts(pts2);

	int i=0,j=0;
	int nSurface = pObj1->GetSurfaceNum();
	
	PolygonIntersect::CPolygonWork pw;		
	
	for( i=0; i<pts1.GetSize(); i++)
	{
		if(i!=0 && pts1[i].pencode==penMove) break;//先简单处理
		pw.add_point(0,pts1[i].x,pts1[i].y,pts1[i].z);
	}
	
	for( i=0; i<pts2.GetSize(); i++)
	{
		if(i!=0 && pts2[i].pencode==penMove) break;//先简单处理
		pw.add_point(1,pts2[i].x,pts2[i].y,pts2[i].z);
	}
	
	pw.set_overlap(GraphAPI::GetDisTolerance());
	pw.initialize();
	
	vector< vector<PolygonIntersect::tpoint> > *pResult = NULL;
	
	//取补集
	pw.find_XOR(0,1,1);
	pResult = &pw.xor_list;
	
	int error = pw.GetError();
	if( error==PolygonIntersect::CPolygonWork::NONE )
	{
		if( pResult->size()<=0 )
		{
			return FALSE;
		}
		
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
					arrNewPts.Add(expt);
				}
				
				CGeoMultiSurface *pGeo = new CGeoMultiSurface();
				if(pGeo && pGeo->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize()) )
				{
					pGeo->EnableClose(TRUE);
					CArray<PT_3DEX,PT_3DEX> arrPts;
					for(j=1; j<nSurface; j++)
					{
						arrPts.RemoveAll();
						pObj1->GetSurface(j, arrPts);
						if(2==GraphAPI::GIsPtInRegion(arrPts[0], arrNewPts.GetData(),arrNewPts.GetSize()))
						{
							pGeo->AddSurface(arrPts);
						}
					}

					arr.Add(pGeo);
				}
				else
				{
					delete pGeo;
				}
			}
		}
		return TRUE;
	}
	else if(error==PolygonIntersect::CPolygonWork::OVERLAP_EXACT )
	{
		//相交了，结果为空
		return FALSE;
	}
	else if(error==PolygonIntersect::CPolygonWork::NO_INTERSECTION)
	{
		PT_3D pt0;
		COPY_3DPT(pt0, pts2[0]);
		if(pObj1->bPtIn(&pt0))
		{
			CGeoMultiSurface *pMS = (CGeoMultiSurface *)pObj1->Clone();
			pMS->AddSurface(pts2);
			arr.Add(pMS);
			return TRUE;
		}
	}
	
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// CMergeSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMergeSurfaceCommand,CEditCommand)

CMergeSurfaceCommand::CMergeSurfaceCommand()
{
    m_nStep = -1;
}

CMergeSurfaceCommand::~CMergeSurfaceCommand()
{

}



CString CMergeSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MERGESURFACE);
}

void CMergeSurfaceCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_strLayerCommonSide.Empty();
	m_pEditor->OpenSelector(SELMODE_MULTI);

	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>1 )
	{
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
}

void CMergeSurfaceCommand::Abort()
{	
	CEditCommand::Abort();
}
 

static void FindSameEdgePart(int *buf, int num, int& start, int& end)
{
	int i, i1;
	
	//找到任一个非公共点
	for( i=0; i<num; i++)
	{
		if( buf[i]==0 )break;
	}
	//无公共点
	if( i>=num )
	{
		start = end = 0;
		return;
	}
	//找到下一个公共点，此点可以认为是非公共部分的末点
	for( i1=i+1; i1<num+i; i1++)
	{
		if( buf[(i1%num)]!=0 )break;
	}
	end = (i1%num);
	
	//找到下一个非公共点，此点的前一点可以认为是非公共部分的起点
	for( ; i1<=num+i; i1++)
	{
		if( buf[((i1+1)%num)]==0 )break;
	}
	start = (i1%num);
}

//获得对象中在start和end之间的点，如果start>end，那么，获得的点是包括对象的首尾点的
static void GetObjectPart(CGeometry* pObj, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);
	int nPtSum = arr.GetSize();
	PT_3DEX expt;
	
	if( end<start )end += nPtSum;
	
	for( int i=start; i<=end; i++)
	{
		expt = arr[i%nPtSum];
		arrPts.Add(expt);
	}
}


static BOOL IsSameShape(CArray<PT_3DEX,PT_3DEX>& pts1, CArray<PT_3DEX,PT_3DEX>& pts2)
{
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	arrPts1.Copy(pts1);
	arrPts2.Copy(pts2);
	
	int nsz1 = GraphAPI::GKickoffSamePoints(arrPts1.GetData(),arrPts1.GetSize());
	arrPts1.SetSize(nsz1);

	int nsz2 = GraphAPI::GKickoffSamePoints(arrPts2.GetData(),arrPts2.GetSize());
	arrPts2.SetSize(nsz2);

	if( nsz1!=nsz2 )
		return FALSE;

	if( nsz1<=0 )
		return FALSE;

	BOOL bClose1 = ((nsz1>1 && GraphAPI::GIsEqual2DPoint(&arrPts1[0],&arrPts1[nsz1-1]))?TRUE:FALSE);
	BOOL bClose2 = ((nsz1>1 && GraphAPI::GIsEqual2DPoint(&arrPts2[0],&arrPts2[nsz1-1]))?TRUE:FALSE);

	if( bClose1!=bClose2 )
		return FALSE;

	PT_3DEX expt1, expt2;
	double toler = GraphAPI::g_lfDisTolerance;

	if(bClose1)
	{
		nsz1--;
		for(int off=0; off<nsz1; off++)
		{
			for(int i=0; i<nsz1; i++)
			{
				expt1 = arrPts1[i];
				expt2 = arrPts2[((off+i)%nsz1)];
				if( (fabs(expt1.x-expt2.x)>toler || fabs(expt1.y-expt2.y)>toler) )
				{
					break;
				}
			}

			if(i>=nsz1)
				return TRUE;
		}

		for(off=0; off<nsz1; off++)
		{
			for(int i=0; i<nsz1; i++)
			{
				expt1 = arrPts1[i];
				expt2 = arrPts2[((off+nsz1-i)%nsz1)];
				if( (fabs(expt1.x-expt2.x)>toler || fabs(expt1.y-expt2.y)>toler) )
				{
					break;
				}	
			}

			if(i>=nsz1)
				return TRUE;
		}		
	}
	else
	{
		for(int i=0; i<nsz1; i++)
		{
			expt1 = arrPts1[i];
			expt2 = arrPts2[i];
			if( (fabs(expt1.x-expt2.x)>toler || fabs(expt1.y-expt2.y)>toler) )
			{
				break;
			}
		}

		if(i>=nsz1)
			return TRUE;
		
		for(i=0; i<nsz1; i++)
		{
			expt1 = arrPts1[i];
			expt2 = arrPts2[nsz1-1-i];
			if( (fabs(expt1.x-expt2.x)>toler || fabs(expt1.y-expt2.y)>toler) )
			{
				break;
			}
		}

		if(i>=nsz1)
			return TRUE;
	}

	return FALSE;
}


static BOOL IsSameShape(CGeometry *pObj1, CGeometry *pObj2)
{
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pObj1->GetShape(arrPts1);
	pObj2->GetShape(arrPts2);

	return IsSameShape(arrPts1,arrPts2);
}


static BOOL IsPartShape(CArray<PT_3DEX,PT_3DEX>& pts1, CArray<PT_3DEX,PT_3DEX>& pts2)
{
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	arrPts1.Copy(pts1);
	arrPts2.Copy(pts2);
	
	int nsz1 = GraphAPI::GKickoffSamePoints(arrPts1.GetData(),arrPts1.GetSize());
	arrPts1.SetSize(nsz1);

	int nsz2 = GraphAPI::GKickoffSamePoints(arrPts2.GetData(),arrPts2.GetSize());
	arrPts2.SetSize(nsz2);

	if( nsz1<=0 || nsz2<=0 )
		return FALSE;

	if(nsz1<nsz2)
		return FALSE;

	double toler = GraphAPI::g_lfDisTolerance;

	{	
		//首尾端点必须相同
		int npt = nsz2;
		PT_3DEX expt1, expt2, expt3;
		BOOL bSame1 = FALSE, bSame2 = FALSE;
		
		expt1 = arrPts1[0];
		expt2 = arrPts2[0];
		expt3 = arrPts2[npt-1];
		if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
		{
			bSame1 = TRUE;
		}
		if( (fabs(expt1.x-expt3.x)<toler && fabs(expt1.y-expt3.y)<toler) )
		{
			bSame2 = TRUE;
		}

		if( !bSame1 && !bSame2 )
			return FALSE;

		//其他节点必须相同
		if( bSame1 )
		{
			//顺序比较
			for( int i=1; i<npt; i++ )
			{
				expt1 = arrPts1[i];
				expt2 = arrPts2[i];

				if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
				{
				}
				else
				{
					break;
				}
			}
			if( i<npt )return FALSE;
			else
				return TRUE;
		}
		else
		{
			//反序比较
			for( int i=1; i<npt; i++ )
			{
				expt1 = arrPts1[i];
				expt2 = arrPts2[npt-1-i];			

				if( (fabs(expt1.x-expt2.x)<toler && fabs(expt1.y-expt2.y)<toler) )
				{
				}
				else
				{
					break;
				}
			}
			if( i<npt )return FALSE;
			else
				return TRUE;
		}
	}
}


BOOL CMergeSurfaceCommand::FindSameEdgeObj(CArray<PT_3DEX,PT_3DEX>& arrPts, CFtrArray& arrFtrs)
{
	Envelope e;
	e.CreateFromPtAndRadius(arrPts[0],GraphAPI::g_lfDisTolerance);	
	m_pEditor->GetDataQuery()->FindObjectInRect(e,NULL);

	int num = 0;
	const CPFeature * ftrs0 = m_pEditor->GetDataQuery()->GetFoundHandles(num);
	if( num<2  )
	{	
		return FALSE;
	}
	if(num==2)
	{
		return TRUE;//没有其他公共边，只有要合并的两个面时，直接返回
	}

	//后面有递归调用，会覆盖 GetFoundHandles 的结果，所以这里将结果复制出来
	CArray<CPFeature,CPFeature> ftrs;
	ftrs.SetSize(num);
	memcpy(ftrs.GetData(),ftrs0,sizeof(CPFeature)*num);

	for(int i=0; i<num; i++)
	{
		CGeometry *pGeo = ftrs[i]->GetGeometry();
		CArray<PT_3DEX,PT_3DEX> arrPts2;
		pGeo->GetShape(arrPts2);

		if(IsPartShape(arrPts,arrPts2) && CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftrs[i],m_strLayerCommonSide) )
		{
			if(arrPts.GetSize()==arrPts2.GetSize())
			{
				arrFtrs.Add(ftrs[i]);
				return TRUE;
			}
			else
			{
				//取出后面一部分，递归查找
				//公共边有可能是多个线串衔接起来的
				CArray<PT_3DEX,PT_3DEX> arrPts3;
				arrPts3.Copy(arrPts);
				arrPts3.RemoveAt(0,arrPts2.GetSize()-1);
				if(arrPts3.GetSize()>1)
				{
					CFtrArray ftrs2;

					if( FindSameEdgeObj(arrPts3,ftrs2) )
					{
						arrFtrs.Add(ftrs[i]);
						arrFtrs.Append(ftrs2);
						return TRUE;
					}
				}				
			}
		}		
	}

	return FALSE;
}


CGeoSurface *CMergeSurfaceCommand::MergeSurface(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& commonsideFtrs)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	double toler = GraphAPI::g_lfDisTolerance;
	if( (e3.m_xl-e3.m_xh)>toler || (e3.m_yl-e3.m_yh>toler) )
		return NULL;

	CGeoSurface *pObj1_old = pObj1;
	CGeoSurface *pObj2_old = pObj2;

	pObj1 = NormalizeSurface(m_pEditor,pObj1);
	pObj2 = NormalizeSurface(m_pEditor,pObj2);

//	pObj1 = NULL;
//	pObj2 = NULL;

	if(pObj1==NULL)
		pObj1 = pObj1_old;

	if(pObj2==NULL)
		pObj2 = pObj2_old;

	CGeoSurface *pRetObj = NULL;

	if( pObj1->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) && pObj2->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
	{
		pRetObj = MergeSurface_BothMulti((CGeoMultiSurface*)pObj1,(CGeoMultiSurface*)pObj2,commonsideFtrs);
	}
	else if( pObj1->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) && pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CGeoMultiSurface *pObj3 = new CGeoMultiSurface();
		pObj3->CopyFrom(pObj2);
		
		CArray<PT_3DEX,PT_3DEX> arr;
		pObj2->GetShape(arr);
		pObj3->CreateShape(arr.GetData(),arr.GetSize());
		
		CGeoSurface *pNewObj = MergeSurface_BothMulti((CGeoMultiSurface*)pObj1,(CGeoMultiSurface*)pObj3,commonsideFtrs);
		
		delete pObj3;
		
		pRetObj = pNewObj;
	}
	else if( pObj2->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) && pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CGeoMultiSurface *pObj3 = new CGeoMultiSurface();
		pObj3->CopyFrom(pObj1);
		
		CArray<PT_3DEX,PT_3DEX> arr;
		pObj1->GetShape(arr);
		pObj3->CreateShape(arr.GetData(),arr.GetSize());
		
		CGeoSurface *pNewObj = MergeSurface_BothMulti((CGeoMultiSurface*)pObj3,(CGeoMultiSurface*)pObj2,commonsideFtrs);
		
		delete pObj3;
		
		pRetObj = pNewObj;
	}
	else
	{
		pRetObj = MergeSurface_BothSimple(pObj1,pObj2,commonsideFtrs);
	}

	if(pObj1_old!=pObj1)delete pObj1;
	if(pObj2_old!=pObj2)delete pObj2;

	return pRetObj;
}



CGeoSurface *CMergeSurfaceCommand::MergeSurface_new(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& commonsideFtrs)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	double toler = GraphAPI::g_lfDisTolerance;
	if( (e3.m_xl-e3.m_xh)>toler || (e3.m_yl-e3.m_yh>toler) )
		return NULL;

	if( pObj1==NULL || pObj2==NULL )
		return NULL;
	
	if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		return NULL;		

	CGeoSurface *pObj1_new = NormalizeSurface(m_pEditor,pObj1);
	CGeoSurface *pObj2_new = NormalizeSurface(m_pEditor,pObj2);

	if(pObj1_new==NULL || pObj2_new==NULL)
	{
		if(pObj1_new!=NULL)delete pObj1_new;
		if(pObj2_new!=NULL)delete pObj2_new;
		return FALSE;		
	}

//	CGeoSurface *pObj1_new = NULL;
//	CGeoSurface *pObj2_new = NULL;

	if(pObj1_new==NULL)
		pObj1_new = pObj1;

	if(pObj2_new==NULL)
		pObj2_new = pObj2;

	gsbh::GeoSurfaceBooleanHandle gsbhandle;
	if(!gsbhandle.setSurface(pObj2_new,pObj1_new))
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}
	//
	if(!gsbhandle.init())
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}
	//
	gsbhandle.join();

	CGeoSurface* pNewObj = gsbhandle.getResult();

	if(pObj1_new!=pObj1)delete pObj1_new;
	if(pObj2_new!=pObj2)delete pObj2_new;

	if(pNewObj == NULL)
		return NULL;
	//
	CArray<PT_3DEX, PT_3DEX> temp_shape;
	pNewObj->GetShape(temp_shape);
	pNewObj->CopyFrom(pObj1);
	pNewObj->CreateShape(temp_shape.GetData(), temp_shape.GetSize());

	return pNewObj;
}


BOOL CMergeSurfaceCommand::FindSameEdgeObj(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& commonsideFtrs)
{
	if(m_strLayerCommonSide.GetLength()<=0)
		return FALSE;

	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return FALSE;

	pObj1 = (CGeoSurface*)pObj1->Clone();
	pObj2 = (CGeoSurface*)pObj2->Clone();
	
	CArray<PT_3DEX,PT_3DEX> arr1, arr2;
	pObj1->GetShape(arr1);
	pObj2->GetShape(arr2);

	int npt1 = GraphAPI::GKickoffSame2DPoints(arr1.GetData(),arr1.GetSize());
	arr1.SetSize(npt1);
	int npt2 = GraphAPI::GKickoffSame2DPoints(arr2.GetData(),arr2.GetSize());
	arr2.SetSize(npt2);
	pObj1->CreateShape(arr1.GetData(),arr1.GetSize());
	pObj2->CreateShape(arr2.GetData(),arr2.GetSize());

	int nPtSum1 = arr1.GetSize(), nPtSum2 = arr2.GetSize(), count = 1;
	int nSamePtPos;
	CArray<int,int> pos1, pos2;
	pos1.SetSize(nPtSum1);
	pos2.SetSize(nPtSum2);

	//找到点重叠的部分，并设置标志位
	int *buf1 = pos1.GetData(), *buf2 = pos2.GetData();

	memset(buf1,0,sizeof(int)*nPtSum1);
	memset(buf2,0,sizeof(int)*nPtSum2);

	if (pObj1->IsClosed())
	{
		nPtSum1--;
	}

	if (pObj2->IsClosed())
	{
		nPtSum2--;
	}
	
	PT_3DEX expt;
	double tolerance = GraphAPI::g_lfDisTolerance;
	
	for( int i=0; i<nPtSum1; i++)
	{
		expt = arr1[i];

		if (i != 0 && GraphAPI::GIsEqual2DPoint(&expt,&arr1[i-1]) )
		{
			if (buf1[i-1] != 0)
			{					
				buf1[i] = buf1[i-1];
				buf1[i-1] = 0;
			}
			
			continue;
		}

		if(!e2.bPtIn(&expt))
			continue;

		PT_3D p0,p1;
		COPY_3DPT(p0,expt);
//		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&p0,&p1);
		PT_KEYCTRL pos;
		pos = pObj2->FindNearestKeyCtrlPt(p0,tolerance,NULL,1);
		if (pos.IsValid())
		{				
			for (int j=pos.index; j<nPtSum2-1; j++)
			{
				if (!GraphAPI::GIsEqual2DPoint(&arr2[j],&arr2[j+1]))
				{
					break;
				}
			}

			nSamePtPos = j;
			buf1[i] = count;
			buf2[nSamePtPos] = count;
			count++;
		}
	}

	int start1, end1, start2, end2;

	//获得非公共边的部分，但是包括了公共边的起点和末点
	FindSameEdgePart(buf1,nPtSum1,start1,end1);
	FindSameEdgePart(buf2,nPtSum2,start2,end2);

	//有问题的公共边
	if( (start1==end1) || (start2==end2) ||
		(buf1[start1]!=buf2[start2] && buf1[start1]!=buf2[end2]) ||
		(buf1[end1]!=buf2[start2] && buf1[end1]!=buf2[end2]) )
	{
		delete pObj1;
		delete pObj2;
		return FALSE;
	}

	//找公共边，看它是否符合要求
	//获取公共部分
	CArray<PT_3DEX,PT_3DEX> arrPts1;
	
	GetObjectPart(pObj1,end1,start1,arrPts1);
	int npt3 = GraphAPI::GKickoffSame2DPoints(arrPts1.GetData(),arrPts1.GetSize());
	arrPts1.SetSize(npt3);

	BOOL bRet = FindSameEdgeObj(arrPts1,commonsideFtrs);

	delete pObj1;
	delete pObj2;
	return bRet;
}

CGeoSurface *CMergeSurfaceCommand::MergeSurface_BothSimple(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& commonsideFtrs)
{
	CArray<CGeoSurface*,CGeoSurface*> arrRets;
	BOOL bRet = MergeSurface_BothSimple_LogicUnion(pObj1,pObj2,arrRets);
	if( bRet && arrRets.GetSize()>0 )
	{
		CGeoMultiSurface *pObj3 = new CGeoMultiSurface();
		pObj3->CopyFrom(pObj1);	
		
		CArray<PT_3DEX,PT_3DEX> arr_all, arr;
		double area_max = -1;
		int k = -1;
		for(int i=0; i<arrRets.GetSize(); i++)
		{
			if(area_max<arrRets[i]->GetArea())
			{
				area_max = arrRets[i]->GetArea();
				k = i;
			}
		}
		if(k>=0)
		{
			arrRets[k]->GetShape(arr);
			arr_all.Append(arr);
		}
		for(i=0; i<arrRets.GetSize(); i++)
		{
			if(k==i)continue;				
			arr.RemoveAll();
			arrRets[i]->GetShape(arr);
			arr[0].pencode = penMove;

			arr_all.Append(arr);
		}
		pObj3->CreateShape(arr_all.GetData(),arr_all.GetSize());
		pObj3->NormalizeDirection();
		return pObj3;
	}
	else
	{
		return NULL;
	}

	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return NULL;

	pObj1 = (CGeoSurface*)pObj1->Clone();
	pObj2 = (CGeoSurface*)pObj2->Clone();
	
	CArray<PT_3DEX,PT_3DEX> arr1, arr2;
	pObj1->GetShape(arr1);
	pObj2->GetShape(arr2);

	int npt1 = GraphAPI::GKickoffSame2DPoints(arr1.GetData(),arr1.GetSize());
	arr1.SetSize(npt1);
	int npt2 = GraphAPI::GKickoffSame2DPoints(arr2.GetData(),arr2.GetSize());
	arr2.SetSize(npt2);
	pObj1->CreateShape(arr1.GetData(),arr1.GetSize());
	pObj2->CreateShape(arr2.GetData(),arr2.GetSize());

	int nPtSum1 = arr1.GetSize(), nPtSum2 = arr2.GetSize(), count = 1;
	int nSamePtPos;
	CArray<int,int> pos1, pos2;
	pos1.SetSize(nPtSum1);
	pos2.SetSize(nPtSum2);

	//找到点重叠的部分，并设置标志位
	int *buf1 = pos1.GetData(), *buf2 = pos2.GetData();

	memset(buf1,0,sizeof(int)*nPtSum1);
	memset(buf2,0,sizeof(int)*nPtSum2);

	if (pObj1->IsClosed())
	{
		nPtSum1--;
	}

	if (pObj2->IsClosed())
	{
		nPtSum2--;
	}
	
	PT_3DEX expt;
	double tolerance = GraphAPI::g_lfDisTolerance;
	
	for( int i=0; i<nPtSum1; i++)
	{
		expt = arr1[i];

		if (i != 0 && GraphAPI::GIsEqual2DPoint(&expt,&arr1[i-1]) )
		{
			if (buf1[i-1] != 0)
			{					
				buf1[i] = buf1[i-1];
				buf1[i-1] = 0;
			}
			
			continue;
		}

		if(!e2.bPtIn(&expt))
			continue;

		PT_3D p0,p1;
		COPY_3DPT(p0,expt);
//		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&p0,&p1);
		PT_KEYCTRL pos;
		pos = pObj2->FindNearestKeyCtrlPt(p0,tolerance,NULL,1);
		if (pos.IsValid())
		{				
			for (int j=pos.index; j<nPtSum2-1; j++)
			{
				if (!GraphAPI::GIsEqual2DPoint(&arr2[j],&arr2[j+1]))
				{
					break;
				}
			}

			nSamePtPos = j;
			buf1[i] = count;
			buf2[nSamePtPos] = count;
			count++;
		}
	}

	int start1, end1, start2, end2;

	//获得非公共边的部分，但是包括了公共边的起点和末点
	FindSameEdgePart(buf1,nPtSum1,start1,end1);
	FindSameEdgePart(buf2,nPtSum2,start2,end2);

	//有问题的公共边
	if( (start1==end1) || (start2==end2) ||
		(buf1[start1]!=buf2[start2] && buf1[start1]!=buf2[end2]) ||
		(buf1[end1]!=buf2[start2] && buf1[end1]!=buf2[end2]) )
	{		
		delete pObj1;
		delete pObj2;
		return NULL;
	}


	//找公共边，看它是否符合要求
	if(m_strLayerCommonSide.GetLength()>0)
	{
		//获取公共部分
		CArray<PT_3DEX,PT_3DEX> arrPts1;
		
		GetObjectPart(pObj1,end1,start1,arrPts1);
		int npt3 = GraphAPI::GKickoffSame2DPoints(arrPts1.GetData(),arrPts1.GetSize());
		arrPts1.SetSize(npt3);

		if(!FindSameEdgeObj(arrPts1,commonsideFtrs))
		{
			delete pObj1;
			delete pObj2;
			return NULL;
		}
	}

	//获取非公共部分
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	
	GetObjectPart(pObj1,start1,end1,arrPts1);
	GetObjectPart(pObj2,start2,end2,arrPts2);

	//调整顺序
	if( buf1[end1]!=buf2[start2] )
	{
		int size = arrPts2.GetSize();
		for( i=0; i<size/2; i++)
		{
			expt = arrPts2.GetAt(i);
			arrPts2.SetAt(i,arrPts2.GetAt(size-1-i));
			arrPts2.SetAt(size-1-i,expt);
		}
	}

	//添加节点

	CArray<PT_3DEX,PT_3DEX> arr;

	arr.Append(arrPts1);
	arrPts2.RemoveAt(0);

	arr.Append(arrPts2);
	int npt3 = GraphAPI::GKickoffSame2DPoints(arr.GetData(),arr.GetSize());
	arr.SetSize(npt3);
	
	//建立新对象
	CGeoSurface *pNewObj = (CGeoSurface*)pObj1->Clone();	
	pNewObj->CreateShape(arr.GetData(),arr.GetSize());
	
	delete pObj1;
	delete pObj2;

	return pNewObj;
}


BOOL CMergeSurfaceCommand::MergeSurface_BothSimple_LogicUnion(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arrRets)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return FALSE;

	PolygonIntersect::CPolygonWork pw;	
	
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pObj1->GetShape()->GetPts(arrPts1);
	pObj2->GetShape()->GetPts(arrPts2);

	int i, j;

	for( i=0; i<arrPts1.GetSize(); i++)
	{
		pw.add_point(0,arrPts1[i].x,arrPts1[i].y,arrPts1[i].z);
	}

	for( i=0; i<arrPts2.GetSize(); i++)
	{
		pw.add_point(1,arrPts2[i].x,arrPts2[i].y,arrPts2[i].z);
	}

	pw.set_overlap(GraphAPI::GetDisTolerance());
	pw.initialize();

	vector< vector<PolygonIntersect::tpoint> > *pResult = NULL;

	pw.find_union();
	pResult = &pw.union_list;

	int error = pw.GetError();
	if( error==PolygonIntersect::CPolygonWork::NONE )
	{
		if( pResult->size()<=0 )
		{
			return FALSE;
		}

		CArray<PT_3DEX,PT_3DEX> arrNewPts;
		PT_3DEX expt;
		expt.pencode = penLine;
		for( i=0; i<pResult->size(); i++)
		{
			vector<PolygonIntersect::tpoint> list1 = pResult->at(i);
			if( list1.size()>=3 )
			{
				//创建模板对象
				CGeoSurface *pNew = (CGeoSurface*)pObj1->Clone();
				if(!pNew)continue;

				arrNewPts.RemoveAll();
				for( j=0; j<list1.size(); j++)
				{
					PolygonIntersect::tpoint tpt = list1[j];
					COPY_3DPT(expt,tpt);
					arrNewPts.Add(expt);
				}
				
				pNew->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize());
				pNew->EnableClose(TRUE);
				arrRets.Add(pNew);
			}
		}
	}
	else if(error==PolygonIntersect::CPolygonWork::OVERLAP_EXACT )
	{
		arrRets.Add((CGeoSurface*)pObj1->Clone());
		return TRUE;
	}

	return TRUE;
}


BOOL CMergeSurfaceCommand::MergeSurface_BothSimple_LogicSubtract(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arrRets)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return FALSE;

	PolygonIntersect::CPolygonWork pw;	
	
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pObj1->GetShape()->GetPts(arrPts1);
	pObj2->GetShape()->GetPts(arrPts2);

	int i, j;

	for( i=0; i<arrPts1.GetSize(); i++)
	{
		pw.add_point(0,arrPts1[i].x,arrPts1[i].y,arrPts1[i].z);
	}

	for( i=0; i<arrPts2.GetSize(); i++)
	{
		pw.add_point(1,arrPts2[i].x,arrPts2[i].y,arrPts2[i].z);
	}

	pw.set_overlap(GraphAPI::GetDisTolerance());
	pw.initialize();

	vector< vector<PolygonIntersect::tpoint> > *pResult = NULL;

	pw.find_XOR(0,1,1);
	pResult = &pw.xor_list;

	int error = pw.GetError();

	if( error==PolygonIntersect::CPolygonWork::NONE )
	{
		//等同于不相交
		if( pResult->size()<=0 )
		{
			return FALSE;
		}

		CArray<PT_3DEX,PT_3DEX> arrNewPts;
		PT_3DEX expt;
		expt.pencode = penLine;
		for( i=0; i<pResult->size(); i++)
		{
			vector<PolygonIntersect::tpoint> list1 = pResult->at(i);
			if( list1.size()>=3 )
			{
				//创建模板对象
				CGeoSurface *pNew = (CGeoSurface*)pObj1->Clone();
				if(!pNew)continue;

				arrNewPts.RemoveAll();
				for( j=0; j<list1.size(); j++)
				{
					PolygonIntersect::tpoint tpt = list1[j];
					COPY_3DPT(expt,tpt);
					arrNewPts.Add(expt);
				}
				
				pNew->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize());
				pNew->EnableClose(TRUE);

				arrRets.Add(pNew);
			}
		}

		return TRUE;
	}
	else if(error==PolygonIntersect::CPolygonWork::OVERLAP_EXACT )
	{
		//相交了，结果为空
		return TRUE;
	}

	return FALSE;
}



BOOL CMergeSurfaceCommand::MergeSurface_BothSimple_LogicIntersect(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arrRets)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return FALSE;

	PolygonIntersect::CPolygonWork pw;	
	
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pObj1->GetShape()->GetPts(arrPts1);
	pObj2->GetShape()->GetPts(arrPts2);

	int i, j;

	for( i=0; i<arrPts1.GetSize(); i++)
	{
		pw.add_point(0,arrPts1[i].x,arrPts1[i].y,arrPts1[i].z);
	}

	for( i=0; i<arrPts2.GetSize(); i++)
	{
		pw.add_point(1,arrPts2[i].x,arrPts2[i].y,arrPts2[i].z);
	}

	pw.set_overlap(GraphAPI::GetDisTolerance());
	pw.initialize();

	vector< vector<PolygonIntersect::tpoint> > *pResult = NULL;

	pw.find_intersection();
	pResult = &pw.intersection_list;

	int error = pw.GetError();

	if( error==PolygonIntersect::CPolygonWork::NONE )
	{
		//等同于不相交
		if( pResult->size()<=0 )
		{
			return FALSE;
		}

		CArray<PT_3DEX,PT_3DEX> arrNewPts;
		PT_3DEX expt;
		expt.pencode = penLine;
		for( i=0; i<pResult->size(); i++)
		{
			vector<PolygonIntersect::tpoint> list1 = pResult->at(i);
			if( list1.size()>=3 )
			{
				//创建模板对象
				CGeoSurface *pNew = (CGeoSurface*)pObj1->Clone();
				if(!pNew)continue;

				arrNewPts.RemoveAll();
				for( j=0; j<list1.size(); j++)
				{
					PolygonIntersect::tpoint tpt = list1[j];
					COPY_3DPT(expt,tpt);
					arrNewPts.Add(expt);
				}
				
				pNew->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize());
				pNew->EnableClose(TRUE);

				arrRets.Add(pNew);
			}
		}

		return TRUE;
	}
	else if(error==PolygonIntersect::CPolygonWork::OVERLAP_EXACT )
	{
		arrRets.Add((CGeoSurface*)pObj1->Clone());
		return TRUE;
	}

	return FALSE;
}


//判断面包含
static bool IsSurfaceInclude(CGeoSurface* pObj1, CGeoSurface* pObj2, BOOL& bReverse)
{	
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	bReverse = FALSE;

	if(e1.bEnvelopeIn(&e2))
	{
	}
	else if(e2.bEnvelopeIn(&e1))
	{
		CGeoSurface* pObj = pObj1;
		pObj1 = pObj2;
		pObj2 = pObj;
		bReverse = TRUE;
	}
	else
	{
		return false;
	}

	CArray<PT_3DEX, PT_3DEX> arrPts1, arrPts2;
	pObj1->GetShape()->GetPts(arrPts1);
	pObj2->GetShape()->GetPts(arrPts2);
	
	for (int i=0; i<arrPts2.GetSize(); i++)
	{
		int temp = GraphAPI::GIsPtInRegion(arrPts2[i],arrPts1.GetData(),arrPts1.GetSize());
		if (2==temp||0==temp||1==temp)
		{
			;
		}
		else
			break;
	}
	if(i==arrPts2.GetSize())
	{
		return true;
	}
	
	return false;
}


void CMergeSurfaceCommand::CalcSurfaceFlags(CArray<CGeoSurface*,CGeoSurface*>& arrObjs, CArray<int,int>& flags)
{
	//判断边界的性质，1表示外边界，0表示内边界，方法是当该面的某个顶点落在除该面以外的所有其他子面内部的次数为偶数时，为外边界，奇数时为内边界；
	CArray<PT_3DEX,PT_3DEX> arrPts;
	for(int i=0; i<arrObjs.GetSize(); i++)
	{
		CGeoSurface *pObj = arrObjs[i];

		CArray<PT_3DEX,PT_3DEX> arrPts1;
		pObj->GetShape(arrPts1);
		int npt = arrPts1.GetSize();
		int count = 0;

		for(int k=0; k<npt; k++)
		{
			PT_3DEX pt0 = arrPts1[k];
			
			BOOL bOnEdge = FALSE;
			count = 0;
			for(int j=0; j<arrObjs.GetSize(); j++)
			{
				if(j==i)continue;

				arrPts.RemoveAll();
				arrObjs[j]->GetShape()->GetPts(arrPts);

				int ret = GraphAPI::GIsPtInRegion(pt0,arrPts.GetData(),arrPts.GetSize());

				if(ret==2)
				{
					count++;
				}
				else if(ret==1)
				{
					bOnEdge = TRUE;
					break;
				}
			}

			//如果该点落在其他边界上，判断会失效，就需要判断其他点
			if(!bOnEdge)
				break;
		}

		flags.Add((count%2)==0?1:0);
	}
}

//都是复杂面对象
CGeoSurface *CMergeSurfaceCommand::MergeSurface_BothMulti_Logic(CGeoMultiSurface *pObj1, CGeoMultiSurface *pObj2)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return NULL;

	BOOL bModified_whole = FALSE;

	int nSon1 = pObj1->GetSurfaceNum();
	int nSon2 = pObj2->GetSurfaceNum();

	//思路：1、首先判断边界的性质，是外边界还是内边界；
	//2、将外边界与外边界合并，内边界与内边界合并，外边界与内边界做逻辑加法运算；
	//3、再作包含关系的判定，外边界邻近包含外边界时，去掉较小的外边界，内边界邻近包含内边界时，去掉较大的内边界；
	//所谓邻近包含是指：不存在越级包含，比如A包含B，B包含C，那么A包含C就是越级包含；
	//4、当2和3都对数据没有任何改动时，表明合并失败；

	//下面是实现
	//1、生成各个子面对象
	int i, j, k;
	CArray<CGeoSurface*,CGeoSurface*> sonSurfaces1, sonSurfaces2;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	for(i=0; i<nSon1; i++)
	{
		arrPts.RemoveAll();
		CGeoSurface *pObj = new CGeoSurface();
		pObj1->GetSurface(i,arrPts);
		if(arrPts.GetSize()<3)
			continue;
		
		pObj->CreateShape(arrPts.GetData(),arrPts.GetSize());
		sonSurfaces1.Add(pObj);
	}

	for(i=0; i<nSon2; i++)
	{
		arrPts.RemoveAll();
		CGeoSurface *pObj = new CGeoSurface();
		pObj2->GetSurface(i,arrPts);
		if(arrPts.GetSize()<3)
			continue;
		
		pObj->CreateShape(arrPts.GetData(),arrPts.GetSize());
		sonSurfaces2.Add(pObj);
	}
	
	//2、判断边界的性质，1表示外边界，0表示内边界，方法是当该面的某个顶点落在除该面以外的所有其他子面内部的次数为偶数时，为外边界，奇数时为内边界；
	CArray<int,int> sonSurfaceFlags1,sonSurfaceFlags2;
	
	CalcSurfaceFlags(sonSurfaces1,sonSurfaceFlags1);
	CalcSurfaceFlags(sonSurfaces2,sonSurfaceFlags2);

	//3、将外边界与外边界合并，内边界与内边界合并，外边界与内边界做逻辑加法运算；
	//先做减法，再做加法

	//A的内边界+B的内边界
	CArray<CGeoSurface*,CGeoSurface*> sonSurfaces3;
	for(i=0; i<sonSurfaces1.GetSize(); i++)
	{
		CGeoSurface *p1 = sonSurfaces1[i];
		if(p1==NULL)
			continue;

		if(sonSurfaceFlags1[i]!=0)
			continue;
		
		for(j=0; j<sonSurfaces2.GetSize(); j++)
		{
			CGeoSurface *p2 = sonSurfaces2[j];
			if(p2==NULL)
				continue;

			if(sonSurfaceFlags1[i]==sonSurfaceFlags2[j])
			{
				CArray<CGeoSurface*,CGeoSurface*> arrObjs;
				if(MergeSurface_BothSimple_LogicIntersect(p1, p2, arrObjs))
				{
					bModified_whole = TRUE;
					for(k=0; k<arrObjs.GetSize(); k++)
					{
						sonSurfaces3.Add(arrObjs[k]);
					}
				}
			}
		}
	}
	//A的内边界-B的外边界
	for(i=0; i<sonSurfaces1.GetSize(); i++)
	{
		CGeoSurface *p1 = sonSurfaces1[i];
		if(sonSurfaceFlags1[i]!=0)
			continue;
		
		for(j=0; j<sonSurfaces2.GetSize(); j++)
		{
			CGeoSurface *p2 = sonSurfaces2[j];
			if(sonSurfaceFlags2[j]!=1)
				continue;
		
			CArray<CGeoSurface*,CGeoSurface*> arrObjs;
			if(MergeSurface_BothSimple_LogicSubtract(p1, p2, arrObjs))
			{
				bModified_whole = TRUE;
				if(arrObjs.GetSize()==1 && IsSameShape(p1,arrObjs[0]))
				{						
				}
				else
				{
					delete p1;
					sonSurfaces1[i] = NULL;
					for(k=0; k<arrObjs.GetSize(); k++)
					{
						sonSurfaces1.Add(arrObjs[k]);
						sonSurfaceFlags1.Add(sonSurfaceFlags1[i]);
					}
					break;						
				}
			}
		}
	}

	//B的内边界-A的外边界
	for(i=0; i<sonSurfaces2.GetSize(); i++)
	{
		CGeoSurface *p1 = sonSurfaces2[i];
		if(sonSurfaceFlags2[i]!=0)
			continue;
		
		for(j=0; j<sonSurfaces1.GetSize(); j++)
		{
			CGeoSurface *p2 = sonSurfaces1[j];
			if(sonSurfaceFlags1[j]!=1)
				continue;
		
			CArray<CGeoSurface*,CGeoSurface*> arrObjs;
			if(MergeSurface_BothSimple_LogicSubtract(p1, p2, arrObjs))
			{
				bModified_whole = TRUE;
				if(arrObjs.GetSize()==1 && IsSameShape(p1,arrObjs[0]))
				{						
				}
				else
				{
					delete p1;
					sonSurfaces2[i] = NULL;
					for(k=0; k<arrObjs.GetSize(); k++)
					{
						sonSurfaces2.Add(arrObjs[k]);
						sonSurfaceFlags2.Add(sonSurfaceFlags2[i]);
					}
					break;						
				}
			}
		}
	}

	//A的外边界+B的外边界
	for(i=0; i<sonSurfaces1.GetSize(); i++)
	{
		CGeoSurface *p1 = sonSurfaces1[i];
		if(p1==NULL)
			continue;

		if(sonSurfaceFlags1[i]!=1)
			continue;
		
		for(j=0; j<sonSurfaces2.GetSize(); j++)
		{
			CGeoSurface *p2 = sonSurfaces2[j];
			if(p2==NULL)
				continue;

			if(sonSurfaceFlags1[i]==sonSurfaceFlags2[j])
			{
				CArray<CGeoSurface*,CGeoSurface*> arrObjs;
				CArray<int,int> flags;
				if( MergeSurface_BothSimple_LogicUnion(p1, p2, arrObjs) )
				{
					bModified_whole = TRUE;
					if( arrObjs.GetSize()==1 )
					{						
						delete p1;
						delete p2;
						sonSurfaces1[i] = NULL;
						sonSurfaces2[j] = arrObjs[0];
						break;
					}
					else if( arrObjs.GetSize()>1 )
					{
						delete p1;
						delete p2;
						sonSurfaces1[i] = NULL;
						sonSurfaces2[j] = NULL;	
						sonSurfaces2.Append(arrObjs);
						CalcSurfaceFlags(arrObjs,flags);
						sonSurfaceFlags2.Append(flags);
						break;
					}
				}
			}
		}
	}

	//将内部边界的交集添加进来
	for(i=0; i<sonSurfaces3.GetSize(); i++)
	{
		sonSurfaces1.Add(sonSurfaces3[i]);
		sonSurfaceFlags1.Add(0);
	}

	sonSurfaces1.Append(sonSurfaces2);
	sonSurfaceFlags1.Append(sonSurfaceFlags2);

	//4、再作包含关系的判定，外边界邻近包含外边界时，去掉较小的外边界，内边界邻近包含内边界时，去掉较大的内边界；
	BOOL bModified = TRUE;
	while(bModified)
	{
		bModified = FALSE;

		for(i=0; i<sonSurfaces1.GetSize(); i++)
		{
			CGeoSurface *p1 = sonSurfaces1[i];
			if(p1==NULL)continue;
			
			for(j=i+1; j<sonSurfaces1.GetSize(); j++)
			{
				CGeoSurface *p2 = sonSurfaces1[j];
				if(p2==NULL)continue;

				if(sonSurfaceFlags1[i]==sonSurfaceFlags1[j])
				{
					//bReverse：TRUE被p2包含，FALSE包含p2
					BOOL bReverse = FALSE;
					if( IsSurfaceInclude(p1,p2,bReverse) )
					{
						CGeoSurface *p_inner = (bReverse?p1:p2);
						CGeoSurface *p_outer = (bReverse?p2:p1);
						int index_outer = (bReverse?j:i);
						int index_inner = (bReverse?i:j);

						int nearest_index = index_outer;
						CGeoSurface *p_nearest = p_outer;
						for(k=0; k<sonSurfaces1.GetSize(); k++)
						{
							if(k==i || k==j)continue;
							CGeoSurface *p3 = sonSurfaces1[k];
							if(p3==NULL)continue;

							//看看有没有他们之间有没有其他边界
							BOOL bReverse2 = FALSE;
							if( IsSurfaceInclude(p_inner,p3,bReverse2) )
							{
								//被包含，取面积最小的
								if(bReverse2)
								{
									if(p_nearest->GetArea()>p3->GetArea())
									{
										p_nearest = p3;
										nearest_index = k;
									}
								}
							}
						}

						//如果有其他边界，就不处理。我们只处理相邻且同类型边界
						if(nearest_index!=index_outer)
							continue;

						BOOL bKill_Outer = FALSE;

						//外边界，去掉较小的
						if(sonSurfaceFlags1[i])
						{
						}
						//内边界，去掉较大的
						else
						{
							bKill_Outer = TRUE;
						}

						if(bKill_Outer)
						{
							delete p_outer;
							sonSurfaces1[index_outer] = NULL;
						}
						else
						{
							delete p_inner;
							sonSurfaces1[index_inner] = NULL;					
						}

						bModified = TRUE;
						bModified_whole = TRUE;

						break;
					}
				}
			}
		}
	}

	if(bModified_whole)
	{
		CGeoMultiSurface *pNewObj = (CGeoMultiSurface*)pObj1->Clone();
		CArray<PT_3DEX,PT_3DEX> arrPts_all;
		for(i=0; i<sonSurfaces1.GetSize(); i++)
		{
			if(sonSurfaces1[i]!=NULL)
			{
				arrPts.RemoveAll();
				sonSurfaces1[i]->GetShape(arrPts);
				arrPts[0].pencode = penMove;
				arrPts_all.Append(arrPts);
				delete sonSurfaces1[i];
			}
		}

		if(arrPts_all.GetSize()>0)
			arrPts_all[0].pencode = penLine;

		pNewObj->CreateShape(arrPts_all.GetData(),arrPts_all.GetSize());

		return pNewObj;
	}

	for(i=0; i<sonSurfaces1.GetSize(); i++)
	{
		if(sonSurfaces1[i]!=NULL)
		{
			delete sonSurfaces1[i];
		}
	}

	return NULL;
}


//都是复杂面对象
CGeoSurface *CMergeSurfaceCommand::MergeSurface_BothMulti(CGeoMultiSurface *pObj1, CGeoMultiSurface *pObj2, CFtrArray& commonsideFtrs)
{
	return MergeSurface_BothMulti_Logic(pObj1,pObj2);
	
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return NULL;

	int nSon1 = pObj1->GetSurfaceNum();
	int nSon2 = pObj2->GetSurfaceNum();

	//合并可以分为两种：1）外围面共边，则外围面合并成新的外围面，子面加到一起，形成新的复杂面；
	//2）外围面与子面共边，则外围面与子面形成新的外围面，子面加到一起（去掉前面合并的那个），形成新的复杂面；
	CArray<CGeoSurface*,CGeoSurface*> arrGeos1, arrGeos2;

	CArray<PT_3DEX,PT_3DEX> arr;
	
	for(int i=0; i<nSon1; i++)
	{
		arr.RemoveAll();

		pObj1->GetSurface(i,arr);
		CGeoSurface *pGeo = new CGeoSurface();
		pGeo->CreateShape(arr.GetData(),arr.GetSize());
		arrGeos1.Add(pGeo);
	}

	for(i=0; i<nSon2; i++)
	{
		arr.RemoveAll();
		
		pObj2->GetSurface(i,arr);
		CGeoSurface *pGeo = new CGeoSurface();
		pGeo->CreateShape(arr.GetData(),arr.GetSize());
		arrGeos2.Add(pGeo);
	}

	CFtrArray commonsideFtrs1;

	//1）外围面与外围面共边
	CGeoSurface *pNewGeo = MergeSurface_BothSimple(arrGeos1[0],arrGeos2[0],commonsideFtrs1);
	if(pNewGeo)
	{
		CGeoMultiSurface *pNewMulti = (CGeoMultiSurface*)pObj1->Clone();

		arr.RemoveAll();
		pNewGeo->GetShape(arr);
		pNewMulti->CreateShape(arr.GetData(),arr.GetSize());

		for(i=1; i<nSon1; i++)
		{
			arr.RemoveAll();
			
			pObj1->GetSurface(i,arr);
			pNewMulti->AddSurface(arr);

			delete arrGeos1[i];
		}
		
		for(i=1; i<nSon2; i++)
		{
			arr.RemoveAll();
			
			pObj2->GetSurface(i,arr);
			pNewMulti->AddSurface(arr);

			delete arrGeos2[i];
		}

		delete arrGeos1[0];
		delete arrGeos2[0];
		delete pNewGeo;
		commonsideFtrs.Copy(commonsideFtrs1);
		return pNewMulti;
	}

	commonsideFtrs1.RemoveAll();
	
	//2）外围面与子面共边
	BOOL bFindMerge = FALSE, bSameEdge = FALSE;
	for(i=1; i<nSon2; i++)
	{
		CGeoSurface *pNewGeo = MergeSurface_BothSimple(arrGeos1[0],arrGeos2[i],commonsideFtrs1);
		if(!pNewGeo)
		{
			if(IsSameShape(arrGeos1[0],arrGeos2[i]))
			{
				bSameEdge = TRUE;
				bFindMerge = TRUE;
				delete arrGeos1[0];
				delete arrGeos2[i];
				arrGeos1[0] = NULL;
				arrGeos2[i] = NULL;
			}
		}
		if(pNewGeo)
		{
			delete arrGeos1[0];
			delete arrGeos2[i];
			arrGeos1[0] = pNewGeo;
			arrGeos2[i] = NULL;
			
			bFindMerge = TRUE;
		}
	}

	if(bFindMerge)
	{
		CGeoMultiSurface *pNewMulti = (CGeoMultiSurface*)pObj1->Clone();
		
		arr.RemoveAll();
		arrGeos2[0]->GetShape(arr);
		pNewMulti->CreateShape(arr.GetData(),arr.GetSize());

		if(arrGeos1[0])
		{
			arr.RemoveAll();
			arrGeos1[0]->GetShape(arr);
			pNewMulti->AddSurface(arr);
			delete arrGeos1[0];
		}
		
		for(i=1; i<nSon1; i++)
		{
			arr.RemoveAll();
			
			pObj1->GetSurface(i,arr);
			pNewMulti->AddSurface(arr);
			
			delete arrGeos1[i];
		}
		
		for(i=1; i<nSon2; i++)
		{
			if(arrGeos2[i]==NULL)
				continue;

			arr.RemoveAll();
			
			pObj2->GetSurface(i,arr);
			pNewMulti->AddSurface(arr);
			
			delete arrGeos2[i];
		}
		
		delete arrGeos2[0];
		commonsideFtrs.Copy(commonsideFtrs1);
		return pNewMulti;
	}

	for(i=1; i<nSon1; i++)
	{
		CGeoSurface *pNewGeo = MergeSurface_BothSimple(arrGeos2[0],arrGeos1[i],commonsideFtrs1);
		if(!pNewGeo)
		{
			if(IsSameShape(arrGeos2[0],arrGeos1[i]))
			{
				bSameEdge = TRUE;
				bFindMerge = TRUE;
				delete arrGeos2[0];
				delete arrGeos1[i];
				arrGeos2[0] = NULL;
				arrGeos1[i] = NULL;
			}
		}
		if(pNewGeo)
		{
			delete arrGeos2[0];
			delete arrGeos1[i];
			arrGeos2[0] = pNewGeo;
			arrGeos1[i] = NULL;
			
			bFindMerge = TRUE;
		}
	}
	
	if(bFindMerge)
	{
		CGeoMultiSurface *pNewMulti = (CGeoMultiSurface*)pObj1->Clone();
		
		arr.RemoveAll();
		arrGeos1[0]->GetShape(arr);
		pNewMulti->CreateShape(arr.GetData(),arr.GetSize());

		if(arrGeos2[0])
		{
			arr.RemoveAll();
			arrGeos2[0]->GetShape(arr);
			pNewMulti->AddSurface(arr);
			delete arrGeos2[0];
		}
		
		for(i=1; i<nSon2; i++)
		{
			arr.RemoveAll();
			
			pObj2->GetSurface(i,arr);
			pNewMulti->AddSurface(arr);
			
			delete arrGeos2[i];
		}
		
		for(i=1; i<nSon1; i++)
		{
			if(arrGeos1[i]==NULL)
				continue;

			arr.RemoveAll();
			
			pObj1->GetSurface(i,arr);
			pNewMulti->AddSurface(arr);
			
			delete arrGeos1[i];
		}

		delete arrGeos1[0];
		commonsideFtrs.Copy(commonsideFtrs1);
		return pNewMulti;
	}

	return NULL;
}


 
void CMergeSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	int num0 ;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num0);
	if( m_nStep==0 && num0>=2 )
	{
		if( !CanGetSelObjs(flag,TRUE) )return;

		CFtrArray arrFtrs;
		CGeoArray arrGeos;
		for(int i=0; i<num0; i++)
		{
			CGeometry *pObj = HandleToFtr(handles[i])->GetGeometry();
			if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				continue;
			arrFtrs.Add(HandleToFtr(handles[i]));
			arrGeos.Add(pObj);
		}

		CGeoArray arrGeos0;
		arrGeos0.Copy(arrGeos);

		int num = arrGeos.GetSize();
		if(num<2)
			return;

		CFtrArray commonsideFtrs;

		BOOL bModified_all = FALSE, bModified = TRUE;
		
		while(bModified)
		{
			bModified = FALSE;
			for(i=0; i<num; i++)
			{
				for(int j=i+1; j<num; j++)
				{
					CGeoSurface *pObj1 = (CGeoSurface*)arrGeos[i];
					if( pObj1==NULL )continue;

					CGeoSurface *pObj2 = (CGeoSurface*)arrGeos[j];
					if( pObj2==NULL )continue;					
					
					CGeoSurface *pNewObj = MergeSurface_new(pObj2,pObj1,commonsideFtrs);
					if(pNewObj)
					{
						if(arrGeos[i]!=arrGeos0[i])
							delete arrGeos[i];					
						arrGeos[i] = pNewObj;
						arrGeos[j] = NULL;

						bModified_all = TRUE;
						bModified = TRUE;
					}				
				}
			}			
		}

		if(bModified_all)
		{
			CUndoFtrs undo(m_pEditor,Name());

			for(i=0; i<num; i++)
			{
				if(arrGeos[i]!=NULL && arrGeos[i]!=arrGeos0[i])
				{
					CFeature* pFtr = arrFtrs[1]->Clone();

					pFtr->SetID(OUID());
					pFtr->SetToDeleted(FALSE);
					pFtr->SetGeometry(arrGeos[i]);
					m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(arrFtrs[1])));

					GETXDS(m_pEditor)->CopyXAttributes(arrFtrs[1],pFtr);

					undo.arrNewHandles.Add(FtrToHandle(pFtr));
				}
			}

			for(i=0; i<num; i++)
			{
				if(arrGeos[i]!=arrGeos0[i])
				{
					m_pEditor->DeleteObject(FtrToHandle(arrFtrs[i]));
					undo.arrOldHandles.Add(FtrToHandle(arrFtrs[i]));
				}
			}
			
			for(int i=0; i<commonsideFtrs.GetSize(); i++)
			{
				m_pEditor->DeleteObject(FtrToHandle(commonsideFtrs[i]));
				undo.arrOldHandles.Add(FtrToHandle(commonsideFtrs[i]));
			}
			
			undo.Commit(); 
		}
		
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		Finish();
		m_nStep = 3;
	}

	CEditCommand::PtClick(pt,flag);
}

void CMergeSurfaceCommand::PtMove(PT_3D &pt)
{

}


void CMergeSurfaceCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
//	var = (LPCTSTR)(m_strLayerCommonSide);
//	tab.AddValue("LayerCommonSide",&CVariantEx(var));
	
}


void CMergeSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(AccelStr(),Name());
//	param->AddLayerNameParamEx("LayerCommonSide",m_strLayerCommonSide,StrFromResID(IDS_LAY_COMMONSIDE),NULL,LAYERPARAMITEM_LINE);
	param->AddParam("Enter",'\r',StrFromResID(IDS_ENTER_AFFIRM));
}


void CMergeSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"LayerCommonSide",var) )
	{
//		m_strLayerCommonSide = (LPCTSTR)(_bstr_t)(_variant_t)*var;
//		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"Enter",var) )
	{
		PtClick(PT_3D(),0);
	}
	
	CEditCommand::SetParams(tab,bInit);
}



//////////////////////////////////////////////////////////////////////
// CMergeSurfaceCommand_AllMap Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMergeSurfaceCommand_AllMap,CMergeSurfaceCommand)

CMergeSurfaceCommand_AllMap::CMergeSurfaceCommand_AllMap()
{

}

CMergeSurfaceCommand_AllMap::~CMergeSurfaceCommand_AllMap()
{

}



CString CMergeSurfaceCommand_AllMap::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MERGESURFACE_ALLMAP);
}

void CMergeSurfaceCommand_AllMap::Start()
{
 	if( !m_pEditor )return;
	

	CEditCommand::Start();
	
	m_strLayerCommonSide.Empty();
	m_pEditor->CloseSelector();
}

void CMergeSurfaceCommand_AllMap::Abort()
{	
	CEditCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}
 
BOOL Split(CString source, LPCTSTR divKey, CStringArray& dest)
{
	if (strlen(divKey)<=0)
	{
		return FALSE;
	}
	if (source.IsEmpty())
	{
		return FALSE;
	}
	int size = strlen(divKey);
	dest.RemoveAll();
	//判断结尾是不是分隔符 不是加上一个
	if (strcmp((LPCTSTR)source.Right(size),divKey)!=0)
	{
		source +=divKey;
	}

	int pos = 0;

	int pre_pos = 0;

	while( -1 != pos ){
		
		pre_pos = pos;
		
		pos = source.Find(divKey,pos);
		
		if (-1!= pos)
		{
			dest.Add(source.Mid(pre_pos,(pos-pre_pos)));
			pos= pos+1;
		}		
	}
	return TRUE;
}
 
void CMergeSurfaceCommand_AllMap::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	//遍历所有图层，按照图层操作
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	int nLay = pDS->GetFtrLayerCount();

	if(1)
	{
		CStringArray fields;
		if(m_strEqualFields.GetLength()>0)
		{
			Split(m_strEqualFields,",",fields);
		}
		//
		CUndoFtrs undo_pre(m_pEditor,Name());
		GProgressStart(nLay);
		//
		std::list<std::vector<CFeature*> > join_ftr_group;
		//
		for(int i=0; i<nLay; ++i)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if( !pLayer && (!pLayer->IsVisible() || pLayer->IsLocked()) )
				continue;
			//
			std::vector<CFeature*> surface_ftrs;
			int nObj = pLayer->GetObjectCount();
			for(int j=0; j<nObj; ++j)
			{
				CFeature* pftr = pLayer->GetObject(j);
				if(pftr!=NULL && pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					if(pftr->GetGeometry()->GetDataPointSum()>=4)
						surface_ftrs.push_back(pftr);
				}
			}
			if(surface_ftrs.size()<=1)
				continue;
			//
			bool *grouped = new bool[surface_ftrs.size()];
			for(j=0; j<surface_ftrs.size(); ++j)
				grouped[j] = false;
			//
			int k = 0;
			//
L:			while(k<nObj && grouped[k])
			{
				++k;
			}
			if(k>=nObj)
			{
                continue;
			}
			CFeature* pftr_sample = surface_ftrs[k];
			//
			std::vector<CFeature*> group_ftrs;
			group_ftrs.push_back(pftr_sample);
			grouped[k] = true;
			CValueTable tab_sample;
			tab_sample.BeginAddValueItem();
			GETXDS(m_pEditor)->GetXAttributes(pftr_sample,tab_sample);
			tab_sample.EndAddValueItem();
			for(j=k+1; j<surface_ftrs.size(); ++j)
			{
               if(grouped[j])
				   continue;
			   //
			   CFeature* temp = surface_ftrs[j];
			   //
			   bool same_xattri = true;
			   //
			   CValueTable tab1;
			   tab1.BeginAddValueItem();
			   GETXDS(m_pEditor)->GetXAttributes(temp,tab1);
		   	   tab1.EndAddValueItem();
			   //           
			   if(fields.GetSize() == 0 || (tab1.GetFieldCount() == 0 && tab_sample.GetFieldCount() == 0))
                   same_xattri = true;
			   else
			   {
				   for(int n=0; n<fields.GetSize(); n++)
				   {
					   const CVariantEx *var1=NULL, *var2=NULL;
					   if(!tab_sample.GetValue(0,fields[n],var1) || !tab1.GetValue(0,fields[n],var2))
					   {
						   same_xattri = false;
						   break;
					   }
					   
					   if((_variant_t)*var1!=(_variant_t)*var2)
					   {
						   same_xattri = false;
						   break;
					   }
					}
			   }
			   //
			   if(same_xattri)
			   {
				   grouped[j] = true;
				   group_ftrs.push_back(temp);
				   continue;
			   }
			   else
			   {
                  join_ftr_group.push_back(group_ftrs);
				  goto L;
			   }   
			}
			//
			join_ftr_group.push_back(group_ftrs);
			delete[] grouped;
			grouped = NULL;
			//
            GProgressStep();
		}
        GProgressEnd();
		//
		CUndoFtrs undo(m_pEditor,Name());
		GProgressStart(join_ftr_group.size());
		//
        for(std::list<std::vector<CFeature*> >::iterator itr = join_ftr_group.begin(); itr!= join_ftr_group.end(); ++itr)
		{
			gsbh::GeoSurfaceSetBooleanHandle gssbh;
			std::vector<CFeature*>& waiting_join_ftrs = *itr;
			std::map<CString,CFeature*> join_ftrs;
			std::vector<CGeometry*> geos;
			//
			for(int i=0; i<waiting_join_ftrs.size(); ++i)
			{
				CFeature *pFtr = waiting_join_ftrs[i];
				CGeoSurface *pGeo2 = NormalizeSurface(m_pEditor,(CGeoSurface*)pFtr->GetGeometry());
				geos.push_back(pGeo2);
				if(pGeo2!=NULL && gssbh.addSurface(pGeo2,pFtr->GetID().ToString()))
				{
					join_ftrs[pFtr->GetID().ToString()] = pFtr;
				}
			}
			//
			gssbh.init();
			gssbh.join();
			std::vector<gsbh::GeoSurfaceSetBooleanHandle::ResultSurface> res;
			gssbh.getResult(res);
			//
			for(int j=0; j<res.size(); ++j)
			{
				CFeature* pft = join_ftrs[res[j].ftr_id];
				CGeoSurface* pgeo = (CGeoSurface*)pft->GetGeometry();
				
				CArray<PT_3DEX, PT_3DEX> temp_shape;
				res[j].pgs->GetShape(temp_shape);
				res[j].pgs->CopyFrom(pgeo);
				res[j].pgs->CreateShape(temp_shape.GetData(), temp_shape.GetSize());
				
				CFeature* pNewFtr = pft->Clone();
				if (!pNewFtr)
				{
					continue;
				}
				
				pNewFtr->SetID(OUID());
				pNewFtr->SetToDeleted(FALSE);
				pNewFtr->SetGeometry(res[j].pgs);
				
				if(!m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pft)->GetID()))
				{
					continue;
				}
				undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
			}
			//
			for(std::map<CString,CFeature*>::iterator itr = join_ftrs.begin(); itr!=join_ftrs.end(); ++itr)
			{
				m_pEditor->DeleteObject(FtrToHandle(itr->second));
				undo.arrOldHandles.Add(FtrToHandle(itr->second));
			}
			//

			for(i=0; i<geos.size(); ++i)
			{
				if(geos[i]!=NULL)
					delete geos[i];
			}
			
			GProgressStep();
		}
		//
		undo.Commit();
		GProgressEnd();
		//
		CEditCommand::PtClick(pt,flag);
        return ;
	}
	
	
	int nSum = 0;
	for(int i=0; i<nLay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if( !pLayer && (!pLayer->IsVisible() || pLayer->IsLocked()) )
			continue;
		
		int nObj = pLayer->GetObjectCount();
		CFtrArray ftrs;
		for(int j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr)
				continue;
			nSum++;
		}
	}
	
	CUndoFtrs undo(m_pEditor,Name());

	CStringArray fields;
	if(m_strEqualFields.GetLength()>0)
	{
		Split(m_strEqualFields,",",fields);
	}

	GProgressStart(nSum);
	for(i=0; i<nLay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if( !pLayer && (!pLayer->IsVisible() || pLayer->IsLocked()) )
			continue;

		int nObj = pLayer->GetObjectCount();
		CFtrArray ftrs;
		for(int j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr)
				continue;

			if(!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				continue;
			}

			ftrs.Add(pFtr);
		}

		CFtrArray ftrs0, ftrs1;

		//合并标志，标记ftrs1中的对应元素是否被合并了，与ftrs1数组的长度相同
		CArray<int,int> arrMergeFlags;

		//无法合并的对象放到ftrs1数组，合并成功的对象，持续合并下去，到最后也放入ftrs1数组
		//ftrs0中存放原始对象，与ftrs1数组的长度相同，便于添加数据时，复制其属性
		for(j=0; j<ftrs.GetSize(); j++)
		{
			GProgressStep();

			CFeature *pFtr1 = ftrs[j];
			for(int k=0; k<ftrs1.GetSize(); k++)
			{
				CFeature *pFtr2 = ftrs1[k];
				if(pFtr2==NULL)continue;
				
				CGeoSurface *pObj1 = (CGeoSurface*)pFtr1->GetGeometry();
				CGeoSurface *pObj2 = (CGeoSurface*)pFtr2->GetGeometry();

				CFtrArray commonsideFtrs;
				CGeoSurface *pNewObj = MergeSurface(pObj1,pObj2,commonsideFtrs);
				if(!pNewObj)
				{
					continue;
				}				

				BOOL bFieldOK = TRUE;
				if(fields.GetSize()>0)
				{
					CValueTable tab1, tab2;
					tab1.BeginAddValueItem();
					GETXDS(m_pEditor)->GetXAttributes(pFtr1,tab1);
					tab1.EndAddValueItem();

					tab2.BeginAddValueItem();
					GETXDS(m_pEditor)->GetXAttributes(pFtr2,tab2);
					tab2.EndAddValueItem();
					
					if( tab1.GetFieldCount()>0 && tab2.GetFieldCount()>0 )
					{
						for(int n=0; n<fields.GetSize(); n++)
						{
							const CVariantEx *var1=NULL, *var2=NULL;
							if(!tab1.GetValue(0,fields[n],var1) || !tab2.GetValue(0,fields[n],var2))
								break;

							if((_variant_t)*var1!=(_variant_t)*var2)
								break;
						}

						if(n<fields.GetSize())
							bFieldOK = FALSE;
					}
					else
					{
						bFieldOK = FALSE;
					}
				}

				if(!bFieldOK)
				{
					continue;
				}
			
				CFeature* pNewFtr = pFtr1->Clone();

				pNewFtr->SetID(OUID());
				pNewFtr->SetToDeleted(FALSE);
				pNewFtr->SetGeometry(pNewObj);

				for(int m=0; m<commonsideFtrs.GetSize(); m++)
				{
					m_pEditor->DeleteObject(FtrToHandle(commonsideFtrs[m]));
					undo.arrOldHandles.Add(FtrToHandle(commonsideFtrs[m]));
				}

				//用新对象替换k位置，便于后面递归处理
				if(arrMergeFlags[k]==1)
					delete ftrs1[k];
				
				ftrs1[k] = NULL;
				pFtr1 = pNewFtr;
			}

			if( ftrs[j]!=pFtr1 )
			{
				arrMergeFlags.Add(1);
			}
			else
			{
				arrMergeFlags.Add(0);
			}

			ftrs0.Add(ftrs[j]);
			ftrs1.Add(pFtr1);
		}

		for(j=0; j<ftrs1.GetSize(); j++)
		{
			if(ftrs1[j]==NULL)
			{
				m_pEditor->DeleteObject(FtrToHandle(ftrs0[j]));
				undo.arrOldHandles.Add(FtrToHandle(ftrs0[j]));
			}
			else if(ftrs1[j]!=ftrs0[j])
			{
				m_pEditor->AddObject(ftrs1[j],pLayer->GetID());
				
				GETXDS(m_pEditor)->CopyXAttributes(ftrs0[j],ftrs1[j]);
				
				m_pEditor->DeleteObject(FtrToHandle(ftrs0[j]));
				undo.arrOldHandles.Add(FtrToHandle(ftrs0[j]));
				undo.arrNewHandles.Add(FtrToHandle(ftrs1[j]));
			}
		}
	}	

	undo.Commit();
	GProgressEnd();

	CEditCommand::PtClick(pt,flag);
}


void CMergeSurfaceCommand_AllMap::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
//	var = (LPCTSTR)(m_strLayerCommonSide);
//	tab.AddValue("LayerCommonSide",&CVariantEx(var));

	var = (LPCTSTR)(m_strEqualFields);
	tab.AddValue("EqualFields",&CVariantEx(var));
	
}


void CMergeSurfaceCommand_AllMap::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(AccelStr(),Name());
//	param->AddLayerNameParamEx("LayerCommonSide",m_strLayerCommonSide,StrFromResID(IDS_LAY_COMMONSIDE),NULL,LAYERPARAMITEM_LINE);
	param->AddParam("EqualFields",(LPCTSTR)m_strEqualFields,StrFromResID(IDS_EQUALVALUE_FIELDS),NULL);
}


void CMergeSurfaceCommand_AllMap::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"LayerCommonSide",var) )
	{
//		m_strLayerCommonSide = (LPCTSTR)(_bstr_t)(_variant_t)*var;
//		SetSettingsModifyFlag();
	}
	
	if( tab.GetValue(0,"EqualFields",var) )
	{
		m_strEqualFields = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CMergeCurveCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMergeCurveCommand,CEditCommand)

CMergeCurveCommand::CMergeCurveCommand()
{
    m_nStep = -1;
	m_nType = 0;
}

CMergeCurveCommand::~CMergeCurveCommand()
{

}



CString CMergeCurveCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MERGECURVE);
}

void CMergeCurveCommand::Start()
{
 	if( !m_pEditor )return;
	

	CEditCommand::Start();

	int num ;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num==2 )
	{
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		return;
	}

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->OpenSelector();
}

void CMergeCurveCommand::Abort()
{	
	CEditCommand::Abort();
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CMergeCurveCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nType);
	tab.AddValue(PF_MERGECURVE_TYPE,&CVariantEx(var));
	
}


void CMergeCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MergeCurveCommand",StrFromLocalResID(IDS_CMDNAME_MERGECURVE));
	param->BeginOptionParam(PF_MERGECURVE_TYPE,StrFromResID(IDS_CMDPLANE_DCURVETYPE));
	param->AddOption(StrFromResID(IDS_PARLINE),0,' ',m_nType==0);
	param->AddOption(StrFromResID(IDS_GEO_DCURVE),1,' ',m_nType==1);	
	param->EndOptionParam();
	
}


void CMergeCurveCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_MERGECURVE_TYPE,var) )
	{
		m_nType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CMergeCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
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

			if (m_pEditor->GetFtrLayerIDOfFtr(handles[0]) != m_pEditor->GetFtrLayerIDOfFtr(handles[1]))
			{
				GOutPut(StrFromResID(IDS_CMDTIP_MERGECURVE_TIP));
				return;
			}

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
		
		if (m_pEditor->GetFtrLayerIDOfFtr(m_arrFtrs[0]) != m_pEditor->GetFtrLayerIDOfFtr(handle))
		{
			GOutPut(StrFromResID(IDS_CMDTIP_MERGECURVE_TIP));
			return;
		}
		
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
		int layid = m_pEditor->GetFtrLayerIDOfFtr(m_arrFtrs[0]);
		CFtrLayer *pLayer = GETDS(m_pEditor)->GetFtrLayer(layid);
		if (!pLayer)
		{
			Abort();
			return;
		}

		CGeoCurve *pObj1 = (CGeoCurve*)HandleToFtr(m_arrFtrs[0])->GetGeometry();
		CGeoCurve *pObj2 = (CGeoCurve*)HandleToFtr(m_arrFtrs[1])->GetGeometry();
		
		CArray<PT_3DEX,PT_3DEX> arr1,arr2;
		pObj1->GetShape(arr1);
		pObj2->GetShape(arr2);
		if (arr1.GetSize()<=0||arr2.GetSize()<=0)
		{
			Abort();
			return;
		}
	
		int clstype = m_nType==0?CLS_GEOPARALLEL:CLS_GEODCURVE;

		// 判断是否可生成平行线
		if (clstype == CLS_GEOPARALLEL)
		{
			PT_3DEX ret;
			double dis1 = GraphAPI::GGetNearstDisPToPointSet2D(arr1.GetData(),arr1.GetSize(),arr2[0],ret,NULL);
			double dis2 = GraphAPI::GGetNearstDisPToPointSet2D(arr1.GetData(),arr1.GetSize(),arr2[arr2.GetSize()-1],ret,NULL);
			
			if (fabs(dis1-dis2) > 1e-4)
			{
				GOutPut(StrFromResID(IDS_CMDTIP_MERGECURVE_ERR));	
				return;
			}
		}

		CFeature *pFtr = pLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),clstype);
		if (!pFtr || !pFtr->GetGeometry())
		{
			Abort();
			return;
		}

		CGeometry *pGeo = pFtr->GetGeometry();

		if (clstype == CLS_GEOPARALLEL)
		{
			int isSameDirection = -1;//判断平行折线方向
			int nret1 = GraphAPI::GIsClockwise(arr1.GetData(),arr1.GetSize());	
			int nret2 = GraphAPI::GIsClockwise(arr2.GetData(),arr2.GetSize());
			if ((nret1!=-1&&nret2!=-1)&&(nret1!=nret2))//不相等为反向
			{
				isSameDirection = 0;
			}
			else if ((nret1!=-1&&nret2!=-1)&&(nret1==nret2))//相等则为同向
			{
				isSameDirection = 1;
			}
			else
			{
				isSameDirection = -1; //方向未知按反向处理
			}

			//双线打散的双线点会反向 符号打散的双线为同向
			PT_3D pt_3d;
			if(0 == isSameDirection||-1 == isSameDirection)//反向或者方向未知（方向未知按反方向处理）
			{
				pt_3d = arr2[0];		
			}
			else if (1 ==isSameDirection )//同向
			{
				pt_3d = arr2[arr2.GetSize()-1];
			}		

			if (!pGeo->CreateShape(arr1.GetData(),arr1.GetSize()) || !((CGeoParallel*)pGeo)->SetCtrlPoint(0,pt_3d))
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
		tab.DelField(FIELDNAME_SYMBOLNAME);

		pFtr->ReadFrom(tab);

		CUndoFtrs undo(m_pEditor,Name());
		
		m_pEditor->AddObject(pFtr,layid);
		undo.AddNewFeature(FtrToHandle(pFtr));
				
		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_arrFtrs[0]),pFtr );
		m_pEditor->DeleteObject(m_arrFtrs[0]);
		m_pEditor->DeleteObject(m_arrFtrs[1]);
		undo.AddOldFeature(m_arrFtrs[0]);
		undo.AddOldFeature(m_arrFtrs[1]);

		undo.Commit();
		
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		Finish();
		m_nStep = 3;
	}

	CEditCommand::PtClick(pt,flag);
}

void CMergeCurveCommand::PtMove(PT_3D &pt)
{

}

//////////////////////////////////////////////////////////////////////
// CMirrorCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMirrorCommand,CEditCommand)

CMirrorCommand::CMirrorCommand()
{
    m_nStep = -1;
	m_nGetAxisMode = modeDraw;
	strcat(m_strRegPath,"\\Mirror");
}

CMirrorCommand::~CMirrorCommand()
{

	m_ptrObjs.RemoveAll();
}



CString CMirrorCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MIRROR);
}

void CMirrorCommand::Start()
{
 	if( !m_pEditor )return;	

	m_ptrObjs.RemoveAll();
	
	CEditCommand::Start();
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( m_nGetAxisMode==modeDraw && num>0 )
		m_pEditor->CloseSelector();
	else if( m_nGetAxisMode==modeSelect )
	{
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
	}

	GOutPut(StrFromResID(IDS_CMDTIP_SELECTTARGET));
}

void CMirrorCommand::Abort()
{	
	m_ptrObjs.RemoveAll();		
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->RefreshView();

	CEditCommand::Abort();	
}
  
void CMirrorCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
//选取
	if( m_nGetAxisMode==modeSelect )
	{
		if( m_nStep==0 )
		{ 
			if( m_ptrObjs.GetSize()==0 || (flag&SELSTAT_MULTISEL)!=0 )
			{
				m_ptrObjs.RemoveAll();
				int num ;
				const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
				if( num>0 )
				{
					for(int i=0;i<num;i++)
					{
						m_ptrObjs.Add(HandleToFtr(handles[i]));
					}
				}
				GOutPut(StrFromResID(IDS_CMDTIP_SELECT_SYMMAXIS));
				return;
			}

			if( m_ptrObjs.GetSize()>0 )
			{							
				GotoState(PROCSTATE_PROCESSING);
				m_nStep = 1;
			}
		}

		if( m_nStep==1 )
		{ 
			if( (flag&SELSTAT_MULTISEL)!=0 )
			{
				return;
			}			
			//找到对象
			FTR_HANDLE handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			if (handle == NULL) return;
			CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
			if( !pObj )return;
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
			//	OutputTipString(CMDERR_TYPEINVALID);
				return;
			}
			
			//有效范围
			double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
			PT_3D p;
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p);
			Envelope e ;
			e.CreateFromPtAndRadius(p,r);
			
			if( !pObj->FindNearestBaseLine(p,e,m_pEditor->GetCoordWnd().m_pSearchCS,&m_ptDragStart,&m_ptDragEnd,NULL) )
			{
				return;
			}
			CUndoFtrs undo(m_pEditor,Name());
			for( int i=m_ptrObjs.GetSize()-1; i>=0; i--)
			{
				pObj = ((CPFeature)m_ptrObjs[i])->GetGeometry();
				if( !pObj )continue;
				pObj = MirrorObj(pObj, m_ptDragStart, m_ptDragEnd);
				if( pObj )
				{
					CFeature *pFtr = ((CPFeature)m_ptrObjs[i])->Clone();
					if (pFtr)
					{
						pFtr->SetID(OUID());
						pFtr->SetGeometry(pObj);
						m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle((CPFeature)m_ptrObjs[i])));
						GETXDS(m_pEditor)->CopyXAttributes((CPFeature)m_ptrObjs[i],pFtr);
					}				
					undo.arrNewHandles.Add(FtrToHandle(pFtr));
				}				
			}
			undo.Commit();
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 4;
		}
	}
	//画线
	else if( m_nGetAxisMode==modeDraw )
	{
		if( m_nStep==0 )
		{ 
			int num ;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num>0 )
			{
				if( !CanGetSelObjs(flag) )
				{
					GOutPut(StrFromResID(IDS_CMDTIP_DRAW_SYMMAXIS));
					return;
				}

				EditStepOne();
				m_ptDragStart = pt;
				m_ptDragEnd   = pt;
				
				CGeometry *pObj;
				for( int i=num-1; i>=0; i--)
				{
					pObj = HandleToFtr(handles[i])->GetGeometry();
					if( !pObj )continue;
					m_ptrObjs.Add(pObj);
				}
				
				if( (flag&SELSTAT_DRAGSEL_RESTART)!=0 || flag==SELSTAT_NONESEL )m_nStep = 2;
			}
		}
		else if( m_nStep==1 )
		{ 
			m_ptDragStart = pt;
			m_ptDragEnd   = pt;
			m_nStep = 2;
		}
		else if( m_nStep==3 )
		{ 
			CGeometry *pObj;
			m_ptDragEnd = pt;
			m_ptrObjs.RemoveAll();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
			
			
			PT_3D ptdir;
			ptdir.x = ptdir.y = 0;
			ptdir.z = 1000.0;
			CUndoFtrs undo(m_pEditor,Name());
			int num ;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			GProgressStart(num);
			for( int i=num-1; i>=0; i--)
			{
				GProgressStep();

				pObj = HandleToFtr(handles[i])->GetGeometry();
				if( !pObj )continue;
				pObj = MirrorObj(pObj, m_ptDragStart, m_ptDragEnd);
				if( pObj )
				{
					CFeature *pFtr = HandleToFtr(handles[i])->Clone();
					if (pFtr)
					{
						pFtr->SetID(OUID());
						pFtr->SetGeometry(pObj);
						m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(handles[i]));
						GETXDS(m_pEditor)->CopyXAttributes((CPFeature)handles[i],pFtr);
					}
					
					undo.arrNewHandles.Add(FtrToHandle(pFtr));
				}
				
			}

			GProgressEnd();

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			undo.Commit();
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 4;
		}
	}

	CEditCommand::PtClick(pt,flag);

	return;
}

CGeometry *CMirrorCommand::MirrorObj(CGeometry *pObj, PT_3D pt0, PT_3D pt1)
{
	CGeometry *pNew = pObj->Clone();
	if( !pNew )return NULL;
	
	PT_3DEX expt;
	double x,y;
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);
	int ptsum = arr.GetSize();
	for( int i=0; i<ptsum; i++)
	{
		expt = arr[i];
		GraphAPI::GGetPerpendicular(pt0.x,pt0.y,pt1.x,pt1.y,expt.x,expt.y,&x,&y,NULL);
		expt.x = 2*x-expt.x; expt.y = 2*y-expt.y;
		arr[i] = expt;
	}
	pNew->CreateShape(arr.GetData(),arr.GetSize());
	return pNew;

}

void CMirrorCommand::PtMove(PT_3D &pt)
{
	if( m_nGetAxisMode==modeDraw )
	{
		if( m_nStep==2 || m_nStep==3 )
		{
			CGeometry *pObj, *pTmp;
			GrBuffer buf;
			
			int num = m_ptrObjs.GetSize();
			for( int i=0; i<num; i++)
			{
				pObj = (CGeometry*)m_ptrObjs[i];
				pTmp = MirrorObj(pObj, m_ptDragStart, pt);
				pTmp->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
				delete pTmp;
			}
			buf.BeginLineString(0,0);
			buf.MoveTo(&m_ptDragStart);
			buf.LineTo(&pt);
			
			buf.End();
			m_ptDragEnd   = pt;
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
			m_nStep = 3;
		}
	}
	CEditCommand::PtMove(pt);

}

void CMirrorCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nGetAxisMode);
	tab.AddValue(PF_MTRRERAXIS,&CVariantEx(var));
}

void CMirrorCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("MirrorCommand",StrFromLocalResID(IDS_CMDNAME_MIRROR));
	param->BeginOptionParam(PF_MTRRERAXIS,StrFromResID(IDS_CMDPLANE_AXIS));
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),modeDraw,' ',m_nGetAxisMode==modeDraw);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),modeSelect,' ',m_nGetAxisMode==modeSelect);	
	param->EndOptionParam();
	
}


void CMirrorCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_MTRRERAXIS,var) )
	{
		m_nGetAxisMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if( !bInit )
		{
			Abort();
		}
	}

	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CCopyWithRotationCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCopyWithRotationCommand,CEditCommand)

CCopyWithRotationCommand::CCopyWithRotationCommand()
{
	m_nStep = -1;
	m_bKeepOld = TRUE;
	strcat(m_strRegPath,"\\CopyWithRotation");
}

CCopyWithRotationCommand::~CCopyWithRotationCommand()
{

}


CString CCopyWithRotationCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPYWITHROTATE);
}

void CCopyWithRotationCommand::Start()
{
	if( !m_pEditor )return;
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
//	m_pEditor->RefreshView();
	m_pEditor->OpenSelector();
	CEditCommand::Start();
	
}

void CCopyWithRotationCommand::Abort()
{	
	CEditCommand::Abort();
}
  
void CCopyWithRotationCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{ 
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num ==1  )
		{	
			if( flag!=SELSTAT_POINTSEL && flag!=SELSTAT_NONESEL )return;
		
			m_objHandleOld = m_pEditor->GetSelection()->GetLastSelectedObj();
			CGeometry *pObj = HandleToFtr(m_objHandleOld)->GetGeometry();
			if( !pObj )return;
			
			GOutPut(StrFromResID(IDS_CMDTIP_MOVEMOUSE));
		//	OutputTipString(StrFromResID(IDS_CMDTIP_ROTATEORIGIN));
			double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
			
			PT_3D p1;
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
			Envelope e;
			e.CreateFromPtAndRadius(p1,r);
			
			BOOL bRes = pObj->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&m_ptKey,NULL);
			if(!bRes) return;
			
			m_pEditor->CloseSelector();
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;			
		}
	}
	else if( m_nStep==1 )
	{ 
		m_ptNew1 = pt;
		m_nStep = 2;
		GOutPut(StrFromResID(IDS_CMDTIP_SETANGLE));
	}
	else if( m_nStep==2 )
	{ 
		m_ptNew2 = pt;
		
		CGeometry *pObj = HandleToFtr(m_objHandleOld)->GetGeometry();
		if( !pObj )return;
		pObj = pObj->Clone();
		if( !pObj )return;
		double m[16];
		m_ptNew1.z = m_ptKey.z;
		Matrix44FromMove(m_ptNew1.x-m_ptKey.x, m_ptNew1.y-m_ptKey.y,m_ptNew1.z-m_ptKey.z,m);
		pObj->Transform(m);
		
		PT_3D ptdir;
		ptdir.x = ptdir.y = 0;
		ptdir.z = 1000.0;
		Matrix44FromRotate(&m_ptNew1,&ptdir,GraphAPI::GGetAngle(m_ptNew1.x,m_ptNew1.y,m_ptNew2.x,m_ptNew2.y),m);

		pObj->Transform(m);	

		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			((CGeoPoint*)pObj)->SetDirection((GraphAPI::GGetAngle(m_ptNew1.x,m_ptNew1.y,m_ptNew2.x,m_ptNew2.y))*180/PI+((CGeoPoint*)pObj)->GetDirection());
		}
		else if(pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{			
			TEXT_SETTINGS0 testSetting;
			((CGeoText*)pObj)->GetSettings(&testSetting);
			testSetting.fTextAngle = (GraphAPI::GGetAngle(m_ptNew1.x,m_ptNew1.y,m_ptNew2.x,m_ptNew2.y))*180/PI;
			((CGeoText*)pObj)->SetSettings(&testSetting);			
		}
	
		CUndoFtrs undo(m_pEditor,Name());
		
		CFeature *pFtr = HandleToFtr(m_objHandleOld)->Clone();
		if(!pFtr)return;

		pFtr->SetID(OUID());
		pFtr->SetGeometry(pObj);
		m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(m_objHandleOld));

		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objHandleOld),pFtr);
		undo.arrNewHandles.Add(FtrToHandle(pFtr));

		if( !m_bKeepOld )
		{
			m_pEditor->DeleteObject(m_objHandleOld);
			undo.arrOldHandles.Add(m_objHandleOld);		
		}
		
		undo.Commit();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 3;
	}
	CEditCommand::PtClick(pt,flag);
}

void CCopyWithRotationCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==1 )
	{
		CGeometry *pObj = HandleToFtr(m_objHandleOld)->GetGeometry();
		if( !pObj )return ;
		pObj = pObj->Clone();
		if( !pObj )return ;
		
		double m[16];
		Matrix44FromMove(pt.x-m_ptKey.x, pt.y-m_ptKey.y,pt.z-m_ptKey.z,m);
		pObj->Transform(m);
		
		GrBuffer buf;
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CPlotTextEx plot;
			TEXT_SETTINGS0  settings;
			((CGeoText*)pObj)->GetSettings(&settings);
			settings.fHeight *= PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale();
			plot.SetSettings(&settings);	
			plot.SetShape(((CGeoText*)pObj)->GetShape());
			plot.SetText(((CGeoText*)pObj)->GetText());
			plot.GetOutLineBuf(&buf);


			pObj->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			delete pObj;
		}
		else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			CFeature *pFtr = HandleToFtr(m_objHandleOld)->Clone();
			pFtr->SetGeometry( pObj );
			
			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(m_objHandleOld));
			
			if( pLayer )
			{
				pDS->DrawFeature(pFtr,&buf,TRUE,0,pLayer->GetName());
			}
			delete pFtr;
		}
		else
		{
			pObj->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			//buf.RefreshEnvelope();
			delete pObj;
		}
		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
	else if( m_nStep==2 )
	{
		m_ptNew2 = pt;
		
		CGeometry *pObj = HandleToFtr(m_objHandleOld)->GetGeometry();
		if( !pObj )return ;
		pObj = pObj->Clone();
		if( !pObj )return ;
		
		double m[16];
		Matrix44FromMove(m_ptNew1.x-m_ptKey.x, m_ptNew1.y-m_ptKey.y,m_ptNew1.z-m_ptKey.z,m);
		pObj->Transform(m);
		
		PT_3D ptdir;
		ptdir.x = ptdir.y = 0;
		ptdir.z = 1000.0;

		Matrix44FromRotate(&m_ptNew1,&ptdir,GraphAPI::GGetAngle(m_ptNew1.x,m_ptNew1.y,m_ptNew2.x,m_ptNew2.y),m);
		
		pObj->Transform(m);		
		
		GrBuffer buf;
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			((CGeoPoint*)pObj)->SetDirection((GraphAPI::GGetAngle(m_ptNew1.x,m_ptNew1.y,m_ptNew2.x,m_ptNew2.y))*180/PI+((CGeoPoint*)pObj)->GetDirection());
			CFeature *pFtr = HandleToFtr(m_objHandleOld)->Clone();
			pFtr->SetGeometry( pObj );
			
			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(m_objHandleOld));
			
			if( pLayer )
			{
				pDS->DrawFeature(pFtr,&buf,TRUE,0,pLayer->GetName());
			}
			delete pFtr;
		}
		else if(pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CFeature *pFtr =HandleToFtr(m_objHandleOld)->Clone();
			pFtr->SetGeometry(pObj);
			
			TEXT_SETTINGS0 testSetting;
			((CGeoText*)pObj)->GetSettings(&testSetting);
			testSetting.fTextAngle = (GraphAPI::GGetAngle(m_ptNew1.x,m_ptNew1.y,m_ptNew2.x,m_ptNew2.y))*180/PI;
			((CGeoText*)pObj)->SetSettings(&testSetting);			
			
			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(m_objHandleOld));
			
			if( pLayer )
			{
				pDS->DrawFeature(pFtr,&buf,TRUE,0,pLayer->GetName());
			}
			
			delete pFtr;
		}
		else
		{
			pObj->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			//buf.RefreshEnvelope();
			delete pObj;
		}
		
    	m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
	CEditCommand::PtMove(pt);
}


void CCopyWithRotationCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_COPYROTATEKPOLD,&CVariantEx(var));
}


void CCopyWithRotationCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CopyWithRotationCommand",StrFromLocalResID(IDS_CMDNAME_COPYWITHROTATE));

	param->AddParam(PF_COPYROTATEKPOLD,bool(m_bKeepOld),StrFromResID(IDS_CMDPLANE_KEEPOLD));
	
}


void CCopyWithRotationCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_COPYROTATEKPOLD,var) )
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}
void CCopyWithRotationCommand::Finish()
{

	CEditCommand::Finish();
}

//////////////////////////////////////////////////////////////////////
// CExtendCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CExtendCommand,CEditCommand)

CExtendCommand::CExtendCommand()
{
	m_nStep = -1;
	m_bMulSelMode = FALSE;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	m_bAutoExtend = TRUE;
	m_bSnap3D = FALSE;
	strcat(m_strRegPath,"\\Extend");
}

CExtendCommand::~CExtendCommand()
{

}



CString CExtendCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_EXTEND);
}

void CExtendCommand::Start()
{
 	if( !m_pEditor )return;
	
	m_idsRefer.RemoveAll();
	m_bMulSelMode = FALSE;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	m_bAutoExtend = TRUE;
	m_bSnap3D = FALSE;

	CEditCommand::Start();

	m_pEditor->OpenSelector(SELMODE_NORMAL);
	
	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	//m_pDrawProc->Start();
	m_pGeoCurve = new CGeoCurve;
	if(!m_pGeoCurve) 
	{
		Abort();
		return ;
	}
	m_pGeoCurve->SetColor(RGB(255,255,255));
	m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
	m_pDrawProc->m_bClosed = FALSE;

	int num;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 2;
		for(int i=0;i<num;i++)
		{
			m_idsRefer.Add(handles[i]);
		}			
	}

	if (m_bAutoExtend)
	{
		m_pEditor->SelectAll();
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num>0 )
		{
			for (int i=0;i<num;i++)
			{
				m_idsRefer.Add(handles[i]);
			}	
			
		}

		m_pEditor->DeselectAll();

		m_nStep = 2;
	}

	if (m_nStep == 0)
	{
		GOutPut(StrFromResID(IDS_TIP_SELREFFTRS));
	}
	else if (m_nStep == 2)
	{
		GOutPut(StrFromResID(IDS_TIP_SELEXTENDFTRS));

		if (m_bMulSelMode)
		{
			m_pEditor->CloseSelector();
			m_pDrawProc->Start();
		}
	}

}
void CExtendCommand::Finish()
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
	m_pEditor->DeselectAll();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CExtendCommand::Abort()
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
	m_idsRefer.RemoveAll();
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CExtendCommand::GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, CArray<double,double> &ret, CArray<double,double> &ret_z)
{
	int num = m_idsRefer.GetSize();
	PT_3D pt3d0, pt3d1, tpt;
	
	COPY_3DPT(pt3d0,pt0);
	COPY_3DPT(pt3d1,pt1);
	
	CDlgDataSource *pDS = GETDS(m_pEditor);

	for( int i=0; i<num; i++)
	{
		CFeature *pFtr = HandleToFtr(m_idsRefer[i]);
		if (!pFtr) continue;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) continue;

		CFtrArray arrObjs;	
		
		CString strLayerName = pLayer->GetName();
		if (strLayerName.Find(StrFromResID(IDS_TIP_LAYERNAMECULVERT)) >= 0)
		{
			pDS->ExplodeSymbolsFinally(pFtr,pLayer,arrObjs,pLayer->GetName());
		}

		CGeometry *pObj = pFtr->GetGeometry();
		if(pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
		{
			CGeometry *pObj1=NULL;
			CGeometry *pObj2=NULL;
			if( ((CGeoParallel*)pObj)->Separate(pObj1,pObj2) )
			{
				if(pObj1) delete pObj1;
				if(pObj2)
				{
					CFeature *pFtr1 = pFtr->Clone();
					if(pFtr1)
					{
						pFtr1->SetGeometry(pObj2);
						arrObjs.Add(pFtr1);
					}
				}
			}
		}
		
		arrObjs.Add(pFtr);
		
		for (int j=0; j<arrObjs.GetSize(); j++)
		{
			CFeature *pFtr0 = arrObjs[j];
			if (!pFtr0) continue;

			CGeometry *po = pFtr0->GetGeometry();
			if( !po )continue;
			
			const CShapeLine  *pb = po->GetShape();
			
			if (pb == NULL) continue;
			
			int npt2 = pb->GetPtsCount();
			if( npt2<=1 )continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pb->GetPts(pts);	
			
			for( int k=0; k<npt2-1; k++)
			{
				if(pts[k].pencode==penMove) continue;
				//求交
				double vector1x = pt3d1.x-pt3d0.x, vector1y = pt3d1.y-pt3d0.y;
				double vector2x = pts[k+1].x-pts[k].x, vector2y = pts[k+1].y-pts[k].y;
				
				double delta = vector1x*vector2y-vector1y*vector2x;
				if( delta<1e-10 && delta>-1e-10 )continue;
				
				double t1 = ( (pts[k].x-pt3d0.x)*vector2y-(pts[k].y-pt3d0.y)*vector2x )/delta;
				if( t1<=1.0 )continue;
				
				double t2 = ( (pts[k].x-pt3d0.x)*vector1y-(pts[k].y-pt3d0.y)*vector1x )/delta;
				if( t2<0.0 || t2>1.0 )continue;
				
				double z = pts[k].z + t2 * (pts[k+1].z-pts[k].z);

				tpt.x = pt3d0.x + t1*vector1x;  tpt.y = pt3d0.y + t1*vector1y;
				if( fabs(tpt.x-pt3d1.x)<1e-4 && fabs(tpt.y-pt3d1.y)<1e-4 )continue;
				
				//由小到大排序
				for( int m=0; m<ret.GetSize(); m++)
				{
					if( t1<ret[m] )break;
				}
				
				if( m<ret.GetSize() )
				{
					ret.InsertAt(m,t1);
					ret_z.InsertAt(m,z);
				}
				else
				{
					ret.Add(t1);
					ret_z.Add(z);
				}
			}

			if (pFtr0 != pFtr)
			{
				delete pFtr0;
			}
		}
		
	}
}

void CExtendCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bMulSelMode);
	tab.AddValue(PF_EXTEND_MULSEL,&CVariantEx(var));

	var = (bool)(m_bAutoExtend);
	tab.AddValue(PF_EXTEND_AUTO,&CVariantEx(var));

	var = (long)(m_bSnap3D);
	tab.AddValue("Snap3D",&CVariantEx(var));
}

void CExtendCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ExtendCommand",StrFromLocalResID(IDS_CMDNAME_EXTEND));
	param->BeginOptionParam(PF_EXTEND_MULSEL,StrFromResID(IDS_CMDPLANE_MULSEL));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMulSelMode);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMulSelMode);
	param->EndOptionParam();

	param->AddParam(PF_EXTEND_AUTO,(bool)m_bAutoExtend,StrFromResID(IDS_CMDNAME_AUTOEXTEND));

	param->BeginOptionParam("Snap3D",StrFromResID(IDS_CMDPLANE_3DSNAP));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bSnap3D);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bSnap3D);
	param->EndOptionParam();
}

void CExtendCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_EXTEND_MULSEL,var) )
	{					
		m_bMulSelMode = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
		{
			Abort();					
			return;
		}
	}

	if( tab.GetValue(0,PF_EXTEND_AUTO,var) )
	{
		m_bAutoExtend = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit && m_nStep != 1)
		{
			Abort();
			return;
		}
	}

	if( tab.GetValue(0,"Snap3D",var) )
	{					
		m_bSnap3D = (long)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}

CProcedure *CExtendCommand::GetActiveSonProc(int nMsgType)
{
	if (m_bMulSelMode&&(nMsgType==msgEnd||nMsgType==msgPtMove||nMsgType==msgBack))
	{
		return m_pDrawProc;
	}
	return NULL;
}

void CExtendCommand::OnSonEnd(CProcedure *son)
{
	if( m_bMulSelMode )
	{
		if( m_nStep==2 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || !IsProcFinished(m_pDrawProc) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 3;
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		//m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		m_pDrawProc->Start();
// 		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
// 		m_pDrawProc->m_pGeoCurve = NULL;
// 		delete m_pDrawProc;
// 		m_pDrawProc = NULL;
// 		
// 		CEditCommand::OnSonEnd(son);
	}
}

void CExtendCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && IsProcProcessing(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}

void CExtendCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	if( m_nStep==0 )
	{
		//by mzy
		if( SELSTAT_DRAGSEL_RESTART == flag)
		{
			int num = -1;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if(num <= 0)
				return;

			m_pEditor->CloseSelector();
			m_nStep = 1;

			PT_3D ptnull;
			memset(&ptnull, 0, sizeof(PT_3D) );
			PtClick(ptnull,0);
		}
		//

		if( (flag&SELSTAT_MULTISEL)!=0 )return;
		if( !(flag==SELSTAT_NONESEL||(flag&SELSTAT_POINTSEL)!=0||(flag&SELSTAT_DRAGSEL)!=0) )
			return; 

//		if( !CanGetSelObjs(flag) )return;
		
		//GOutPut(StrFromResID(IDS_CMDTIP_CLICKOK));

		m_pEditor->CloseSelector();

		m_nStep = 1;
	}
	if (m_nStep == 1)
	{
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num<=0 )
		{
			m_pEditor->SelectAll();
			handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		}
		
		if (m_bMulSelMode)
		{
			m_pEditor->CloseSelector();
			m_pDrawProc->Start();
		}
		
		// 保持选择框不变
		//EditStepOne();
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
		
		for (int i=0;i<num;i++)
		{
			m_idsRefer.Add(handles[i]);
		}
		
		m_pEditor->DeselectAll();

		GOutPut(StrFromResID(IDS_TIP_SELEXTENDFTRS));
		
		m_nStep = 2;

		if (!m_bMulSelMode)
		{
			m_pEditor->OpenSelector();
		}
	}
	else if( m_nStep==2 )
	{
		if (m_bMulSelMode)
		{
			if (m_pDrawProc)
			{
				GotoState(PROCSTATE_PROCESSING);
				m_pDrawProc->PtClick(pt,flag);
			}
		}
		else
		{
			m_nStep = 3;
		}
	}

	if (m_nStep == 3)
	{
		// 寻找要延长的地物和相交点
		if (m_bMulSelMode)
		{
			//粗略查找相交地物
			Envelope e1, e2;
			CArray<PT_3DEX,PT_3DEX> arrGeo;
			m_pGeoCurve->GetShape(arrGeo);
			int sum = arrGeo.GetSize();
			if (sum < 2)
			{
				Abort();
				return;
			}
			e1.CreateFromPts(arrGeo.GetData(),arrGeo.GetSize(),sizeof(PT_3DEX));
			e2 = e1;
			e2.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e2,m_pEditor->GetCoordWnd().m_pSearchCS);
			
			int num1;
			const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
			if( !ppftr1  )
			{	
				Abort();
				return;
			}
			CArray<CPFeature,CPFeature> ftr1;
			ftr1.SetSize(num1);
			memcpy(ftr1.GetData(),ppftr1,num1*sizeof(*ppftr1));

			//获得有效的对象与两线段的相交信息，并排序
			CArray<CSmoothCommand::OBJ_ITEM,CSmoothCommand::OBJ_ITEM> arr1;
			CSmoothCommand::OBJ_ITEM item;
			CGeometry *pObj;
			const CShapeLine *pSL;
			PT_3DEX *pts;
			int size1;
			for (int l=0; l<sum-1; l++)
			{
				for( int i=0; i<num1; i++)
				{
					size1 = arr1.GetSize();
					for( int k=0; k<size1&&arr1[k].id!=(FTR_HANDLE)ftr1[i]; k++);
					if (k < size1) continue;

					pObj = ftr1[i]->GetGeometry();
					if( !pObj )continue;
					
					if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )continue;
					if (((CGeoCurve*)pObj)->IsClosed())continue;
					
					//精细检验是否与相交，记录交点
					pSL = pObj->GetShape();
					if( !pSL )continue;
					
					PT_3D ret;
					double t=-1,st0=-1;
					CPtrArray arr;
					pSL->GetShapeLineUnit(arr);
					int pos = 0;
					for( k =0;k<arr.GetSize();k++ )
					{
						CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
						if( e1.bIntersect(&pUnit->evlp) )
						{
							pts = pUnit->pts;
							for( int j=0; j<pUnit->nuse-1; j++,pts++)
							{
								if(e1.bIntersect(&pts[0],&pts[1]) )
								{
									//求相交点
									if (!GLineIntersectLineSeg1(arrGeo[l].x,arrGeo[l].y,arrGeo[l+1].x,arrGeo[l+1].y,pts[0].x,pts[0].y,pts[0].z,
										pts[1].x,pts[1].y,pts[1].z,&(ret.x),&(ret.y),&(ret.z),&t,&st0))
									{
										continue;
									}							
									
									item.id = FtrToHandle(ftr1[i]);item.pos= pos+j; item.ret= ret; item.t = st0;
									
									arr1.Add(item);

									goto FINDOVER_1;
									
								}
							}
						}
						pos += pUnit->nuse;		
					}
FINDOVER_1:;
				}
			}

			CUndoModifyProperties undo(m_pEditor,Name());
			for (int i=0; i<arr1.GetSize(); i++)
			{
				CFeature *pFtr = (CFeature*)arr1[i].id;
				if (!pFtr) continue;
				
				CGeometry *pObj = pFtr->GetGeometry();
				if( !pObj )return;

				PT_3D ret = arr1[i].ret;
				//---计算当前点位于哪一个端
				double alllen = pObj->GetShape()->GetLength();
				double curlen = pObj->GetShape()->GetLength(&ret);

				int nIndex = -1;
				PT_3DEX expt0, expt1;
				if (curlen <= alllen/2)
				{
					nIndex = 0;
					expt0 = pObj->GetDataPoint(1);
					expt1 = pObj->GetDataPoint(0);
				}
				else
				{
					nIndex = pObj->GetDataPointSum()-1;
					expt0 = pObj->GetDataPoint(nIndex-1);
					expt1 = pObj->GetDataPoint(nIndex);
				}

				//计算端点延长后，相交点的位置
				CArray<double,double> arrRets, arrRets_z;
				GetIntersectPoints(expt0,expt1,arrRets,arrRets_z);
				
				if( arrRets.GetSize()<=0 ) continue;
				
				m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
				
				//延长端点到相交点
				double t = arrRets[0];
				PT_3DEX expt = expt1;
				expt.x = expt0.x + t*(expt1.x-expt0.x);
				expt.y = expt0.y + t*(expt1.y-expt0.y);

				if( m_bSnap3D )
				{
					expt.z = arrRets_z[0];
				}
				
				COPY_3DPT(m_ptOld,expt1);
				COPY_3DPT(m_ptNew,expt);
				
				undo.arrHandles.Add(FtrToHandle(pFtr));
				CValueTable tab2;
				tab2.BeginAddValueItem();
				pFtr->WriteTo(tab2);	
				tab2.EndAddValueItem();
				
				undo.oldVT.AddItemFromTab(tab2);
				
				pObj->SetDataPoint(nIndex,expt);
				
				m_pEditor->RestoreObject(FtrToHandle(pFtr));
				
				CVariantEx var;
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pObj->GetShape(arrPts);
				var.SetAsShape(arrPts);

				if (tab2.SetValue(0,FIELDNAME_SYMBOLNAME,&var))
				{
					undo.newVT.AddItemFromTab(tab2);
				}				
			}

			undo.Commit();

			m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
			m_pEditor->DeselectAll();
// 			for (i=0;i<m_idsRefer.GetSize();i++)
// 			{
// 				m_pEditor->SelectObj(m_idsRefer[i]);
// 			}
			m_pEditor->OnSelectChanged();
			m_pEditor->RefreshView();
			m_nStep = 2;

// 			m_nStep = 2;
// 			Finish();
// 			m_idsRefer.RemoveAll();
			
		}
		else
		{
			//找到点中的地物	
			double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
			PT_3D p1;
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
			
			CPFeature pFtr = m_pEditor->GetDataQuery()->FindNearestObject(p1,r,m_pEditor->GetCoordWnd().m_pSearchCS);
			if( !pFtr )return;
			//if( m_pEditor->GetSelection()->IsObjInSelection(FtrToHandle(pFtr)) )return;
			
			CGeometry *pObj = pFtr->GetGeometry();
			if( !pObj )return;
			if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return;
			if( ((CGeoCurve*)pObj)->IsClosed() )return;
			
			//---计算当前点位于哪一个端
			double alllen = pObj->GetShape()->GetLength();
			double curlen = pObj->GetShape()->GetLength(&pt);
			
			int nIndex = -1;
			PT_3DEX expt0, expt1;
			if( curlen<=alllen/2 )
			{
				nIndex = 0;
				expt0 = pObj->GetDataPoint(1);
				expt1 = pObj->GetDataPoint(0);
			}
			else
			{
				nIndex = pObj->GetDataPointSum()-1;
				expt0 = pObj->GetDataPoint(nIndex-1);
				expt1 = pObj->GetDataPoint(nIndex);
			}
			
			//计算端点延长后，相交点的位置
			CArray<double,double> arrRets, arrRets_z;
			GetIntersectPoints(expt0,expt1,arrRets,arrRets_z);
			
			if( arrRets.GetSize()<=0 )return;
			
			m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
			
			//延长端点到相交点
			double t = arrRets[0];
			PT_3DEX expt = expt1;
			expt.x = expt0.x + t*(expt1.x-expt0.x);
			expt.y = expt0.y + t*(expt1.y-expt0.y);

			if( m_bSnap3D )
			{
				expt.z = arrRets_z[0];
			}
			
			COPY_3DPT(m_ptOld,expt1);
			COPY_3DPT(m_ptNew,expt);
			
			CUndoModifyProperties undo(m_pEditor,Name());
			
			undo.arrHandles.Add(FtrToHandle(pFtr));
			undo.oldVT.BeginAddValueItem();
			pFtr->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
			
			pObj->SetDataPoint(nIndex,expt);
			
			m_pEditor->RestoreObject(FtrToHandle(pFtr));
			
			//m_pEditor->UpdateObject(FtrToHandle(pFtr));
			undo.newVT.BeginAddValueItem();
			pFtr->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();
			
			m_pEditor->DeselectAll();
// 			for (int i=0;i<m_idsRefer.GetSize();i++)
// 			{
// 				m_pEditor->SelectObj(m_idsRefer[i]);
// 			}
			m_pEditor->OnSelectChanged();
			m_pEditor->RefreshView();
			m_nStep = 2;
// 			m_nStep = 2;
// 			Finish();
// 			m_idsRefer.RemoveAll();
			undo.Commit();
		}
		
		m_pEditor->RefreshView();
	}
	

	CEditCommand::PtClick(pt,flag);
}

//////////////////////////////////////////////////////////////////////
// CCutPartCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCutPartCommand,CEditCommand)

CCutPartCommand::CCutPartCommand()
{
	m_nStep = -1;
	m_bMulSelMode = FALSE;
	m_bKeepCutPart = FALSE;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	m_bCutpartSurface = FALSE;
	strcat(m_strRegPath,"\\CutPart");
}

CCutPartCommand::~CCutPartCommand()
{
	m_idsRefer.RemoveAll();
}



CString CCutPartCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CUTPART);
}

void CCutPartCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bMulSelMode);
	tab.AddValue(PF_EXTEND_MULSEL,&CVariantEx(var));

	var = (bool)(m_bKeepCutPart);
	tab.AddValue(PF_KEEPCUTPART,&CVariantEx(var));

	//var = (bool)(m_bCutpartSurface);
	//tab.AddValue(PF_CUTPARTSURFACE,&CVariantEx(var));
}

void CCutPartCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CutPart",StrFromLocalResID(IDS_CMDNAME_CUTPART));
	param->BeginOptionParam(PF_EXTEND_MULSEL,StrFromResID(IDS_CMDPLANE_MULSEL));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMulSelMode);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMulSelMode);
	param->EndOptionParam();

	param->AddParam(PF_KEEPCUTPART,(bool)m_bKeepCutPart,StrFromResID(IDS_CMDPLANE_KEEPCUTPART));

/*	param->BeginOptionParam(PF_CUTPARTSURFACE,StrFromResID(IDS_CMDPLANE_TRIM_SURFACE));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bCutpartSurface);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bCutpartSurface);
	param->EndOptionParam();*/
}

void CCutPartCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_EXTEND_MULSEL,var) )
	{					
		m_bMulSelMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}

	if( tab.GetValue(0,PF_KEEPCUTPART,var) )
	{
		m_bKeepCutPart = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

/*	if( tab.GetValue(0,PF_CUTPARTSURFACE,var) )
	{
		m_bCutpartSurface = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}*/

	CEditCommand::SetParams(tab,bInit);
}

CProcedure *CCutPartCommand::GetActiveSonProc(int nMsgType)
{
	if (m_bMulSelMode&&(nMsgType==msgEnd||nMsgType==msgPtMove||nMsgType==msgBack))
	{
		return m_pDrawProc;
	}
	return NULL;
}

void CCutPartCommand::OnSonEnd(CProcedure *son)
{
	if( m_bMulSelMode )
	{
		if( m_nStep==2 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || !IsProcFinished(m_pDrawProc) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 3;
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		m_pDrawProc->Start();
// 		m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
// 		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
// 		m_pDrawProc->m_pGeoCurve = NULL;
// 		delete m_pDrawProc;
// 		m_pDrawProc = NULL;
// 		
// 		CEditCommand::OnSonEnd(son);
	}
}

void CCutPartCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && IsProcProcessing(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}

void CCutPartCommand::Start()
{
 	if( !m_pEditor )return;	
	
	m_idsRefer.RemoveAll();
	CEditCommand::Start();
	
	m_pEditor->OpenSelector();

	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	//m_pDrawProc->Start();
	m_pGeoCurve = new CGeoCurve;
	if(!m_pGeoCurve) 
	{
		Abort();
		return ;
	}
	m_pGeoCurve->SetColor(RGB(255,255,255));
	m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
	m_pDrawProc->m_bClosed = FALSE;

	int num;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 1;
		for(int i=0;i<num;i++)
		{
			m_idsRefer.Add(handles[i]);
		}		
	}

	if (m_bMulSelMode && m_nStep == 1)
	{
		
	}

	if (m_nStep == 0)
	{
		GOutPut(StrFromResID(IDS_TIP_SELREFFTRS));
	}
	else if (m_nStep == 1)
	{
		GOutPut(StrFromResID(IDS_TIP_SELCUTPARTFTRS));

		if (m_bMulSelMode)
		{			
			m_pEditor->CloseSelector();
			m_pDrawProc->Start();
		}

		m_nStep = 2;
	}
}

void CCutPartCommand::Abort()
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
	m_idsRefer.RemoveAll();
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CCutPartCommand::Finish()
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
	m_pEditor->DeselectAll();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CCutPartCommand::GetIntersectPoints(CGeometry *pObj, CArray<double,double> &ret)
{
	const CShapeLine  *pSL = pObj->GetShape();

	if (!pSL) return;
	
	int npt = pSL->GetPtsCount();
	if( npt<=1 )return;		
	CArray<PT_3DEX,PT_3DEX> pts ;
	pSL->GetPts(pts);
	int num = m_idsRefer.GetSize();
	double t;
	PT_3D ptret;
	
	CDlgDataSource *pDS = GETDS(m_pEditor);

	for( int i=0; i<num; i++)
	{
		CFeature *pFtr = HandleToFtr(m_idsRefer[i]);
		if (!pFtr) continue;
		
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) continue;
		
		CFtrArray arrObjs;	

		CString strLayerName = pLayer->GetName();
		if (strLayerName.Find(StrFromResID(IDS_TIP_LAYERNAMECULVERT)) >= 0)
		{
			pDS->ExplodeSymbolsFinally(pFtr,pLayer,arrObjs,pLayer->GetName());
		}

		CGeometry *pObj = pFtr->GetGeometry();
		if(pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
		{
			CGeometry *pObj1=NULL;
			CGeometry *pObj2=NULL;
			if( ((CGeoParallel*)pObj)->Separate(pObj1,pObj2) )
			{
				if(pObj1) delete pObj1;
				if(pObj2)
				{
					CFeature *pFtr1 = pFtr->Clone();
					if(pFtr1)
					{
						pFtr1->SetGeometry(pObj2);
						arrObjs.Add(pFtr1);
					}
				}
			}
		}
		
		arrObjs.Add(pFtr);

		for (int n=0; n<arrObjs.GetSize(); n++)
		{
			CFeature *pFtr0 = arrObjs[n];
			if (!pFtr0) continue;

			CGeometry *po = pFtr0->GetGeometry();
			if( !po )continue;
			
			const CShapeLine  *pb = po->GetShape();
			
			if (!pb) continue;
			
			int npt2 = pb->GetPtsCount();
			if( npt2<=1 )continue;	
			
			CArray<PT_3DEX,PT_3DEX> pts2 ;
			pb->GetPts(pts2);
			
			
			for( int j=0; j<npt-1; j++)
			{
				//	if( pts[j+1].code==GRBUFFER_PTCODE_MOVETO )continue;*/
				for( int k=0; k<npt2-1; k++)
				{
					if(pts2[k].pencode==penMove) continue;
					if(/*GraphAPI:: GGetLineIntersectLineSeg*/GLineIntersectLineSegInLimit(pts[j].x,pts[j].y,
						pts[j+1].x,pts[j+1].y,pts2[k].x,pts2[k].y,
						pts2[k+1].x,pts2[k+1].y,&ptret.x,&ptret.y,&t, NULL, 1e-4) )
						//if( CGrTrim::Intersect(&pts[j].pt,&pts[j+1].pt,
						//	&pts2[k].pt,&pts2[k+1].pt,&ptret,&t) )
					{
						t = t+j;
						//由小到大排序
						for( int m=0; m<ret.GetSize(); m++)
						{
							if( t<=ret[m] )break;
						}
						
						if( m<ret.GetSize() )
						{
							//去除重复的
							if( t==ret[m] )continue;
							ret.InsertAt(m,t);
						}
						else ret.Add(t);
					}
				}
			}

			if (pFtr0 != pFtr)
			{
				delete pFtr0;
			}
		}
	
	}

}

BOOL CCutPartCommand::IsSelfIntersect(CGeometry *pObj)
{
	//是否为CGeoCurve和是否闭合，在执行此函数之前已经判断过了，这里不再重复
	const CShapeLine* pSL = pObj->GetShape(); 				
	CArray<PT_3DEX,PT_3DEX> arrPts;	
	pSL->GetPts(arrPts);
	int num = arrPts.GetSize();
	if( num < 4 )
		return FALSE;

	for(int i = 0; i < num-3; i++)
	{
		double x0 = arrPts.GetAt(i).x;
		double y0 = arrPts.GetAt(i).y;
		double x1 = arrPts.GetAt(i+1).x;
		double y1 = arrPts.GetAt(i+1).y;
		for(int j = i+2; j < num-1; j++) //两条相邻的线段,不算作相交,所以j=i+2
		{
			double xRet = 0, yRet = 0, T = -1;
			double x2 = arrPts.GetAt(j).x;
			double y2 = arrPts.GetAt(j).y;
			double x3 = arrPts.GetAt(j+1).x;
			double y3 = arrPts.GetAt(j+1).y;
			if( GraphAPI::GGetLineIntersectLineSeg(x0, y0, x1, y1, x2, y2, x3, y3, &xRet, &yRet, &T) )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CCutPartCommand::BreakLine(PT_3DEX pt1, PT_3DEX pt2, CGeoCurve* pCurve, CGeometry*& pObj0, CGeometry*& pObj1, CGeometry*& pObj2)
{
	const CShapeLine* pSL = pCurve->GetShape();
	if( !pSL ) return;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pSL->GetPts( arrPts );
	int num = arrPts.GetSize();
	if( num <= 1 ) return;

	//参数传进来的pt1和pt2必然是曲线上的点，可能是节点或者交点
	//分别获取两点在线上的位置，由于某一点可能是多段的相交点，因此它的位置可能不止一个
	CArray<double,double> pos1, pos2;
	int i = 0, j = 0;
	for(i = 0; i < num-1; i++)
	{
		PT_3DEX tmpt1 = arrPts.GetAt(i); 
		PT_3DEX tmpt2 = arrPts.GetAt(i+1);
		if( GraphAPI::GIsPtInLine(tmpt1, tmpt2, pt1) )
		{
			double ratio = 0;
			if( fabs(pt1.x-tmpt1.x) < 1e-4 && fabs(pt1.y-tmpt1.y) < 1e-4 );
			else if( fabs(pt1.x-tmpt2.x) < 1e-4 && fabs(pt1.y-tmpt2.y) < 1e-4 ) 
				ratio = 1;
			else if( fabs(tmpt2.x-tmpt1.x) < 1e-4 )
				ratio = (pt1.y-tmpt1.y) / (tmpt2.y-tmpt1.y);
			else
				ratio = (pt1.x-tmpt1.x) / (tmpt2.x-tmpt1.x);
			
			pos1.Add(i+ratio);
			i++; //如果一个交点出现在某一段，就绝不会出现在下一个相邻段（节点例个，但需去重）
		}
	}
	int nr1 = pos1.GetSize();
	if( nr1 <= 0 ) return;

	for(i = 0; i < num-1; i++)
	{
		PT_3DEX tmpt1 = arrPts.GetAt(i); 
		PT_3DEX tmpt2 = arrPts.GetAt(i+1);
		if( GraphAPI::GIsPtInLine(tmpt1, tmpt2, pt2) )
		{
			double ratio = 0;
			if( fabs(pt2.x-tmpt1.x) < 1e-4 && fabs(pt2.y-tmpt1.y) < 1e-4 );
			else if( fabs(pt2.x-tmpt2.x) < 1e-4 && fabs(pt2.y-tmpt2.y) < 1e-4 ) 
				ratio = 1;
			else if( fabs(tmpt2.x-tmpt1.x) < 1e-4 )
				ratio = (pt2.y-tmpt1.y) / (tmpt2.y-tmpt1.y);
			else
				ratio = (pt2.x-tmpt1.x) / (tmpt2.x-tmpt1.x);
			
			pos2.Add(i+ratio);
			i++; //如果一个交点出现在某一段，就绝不会出现在下一个相邻段（节点例个，但需去重）
		}
	}
	int nr2 = pos2.GetSize();
	if( nr2 <= 0 ) return;

	//找到第一个pt2前的最后一个pt1的位置 rpos; [0-rpos]作为一段
	//找到第一个比rpos大的pt2的位置rpos2; [rpos-rpos2]作为一段
	//rpos2到结束作为一段

	double rpos = -1;
	for(i = 0; i < nr1; i++)
	{
		if( pos2.GetAt(0) <= pos1.GetAt(i) )
		{
			if( 0 == i )
				rpos = pos1.GetAt(i);
			else
				rpos = pos1.GetAt(i-1);
			break;
		}
	}
	if( rpos == -1 && i >= nr1)
		rpos= pos1.GetAt(nr1-1);

	double rpos2 = -1;
	for(i = 0; i < nr2; i++)
	{
		if(pos2.GetAt(i) >= rpos)
		{
			rpos2 = pos2.GetAt(i);
			break;
		}
	}
	if( rpos2 == -1 && i >= nr2)
		return;

	int imin = (int)rpos;
	int imax = (int)rpos2;
	pObj0 = NULL; pObj1 = NULL; pObj2 = NULL;
	CArray<PT_3DEX,PT_3DEX> pts;

	//第一段
	{
		CGeoCurveBase *pObj = (CGeoCurveBase*)pCurve->Clone();
		for(int i = 0; i <= imin; i++)
		{
			PT_3DEX pt = arrPts.GetAt(i);
			pts.Add( pt );
		}
		if( fabs(rpos-imin) > 1e-4 )
		{
			PT_3DEX ptoo = pt1;
			ptoo.pencode = 2;
			pts.Add( ptoo );
		}

		if( pts.GetSize() > 1 )
		{
			if(!pObj->CreateShape(pts.GetData(),pts.GetSize())) 
				return;
			else
				pObj0 = pObj;
		}

		if( pObj0 && pObj0->GetDataPointSum()==2 )
		{
			PT_3DEX expt1, expt2;
			expt1 = pObj0->GetDataPoint(0);
			expt2 = pObj0->GetDataPoint(1);
			
			if( fabs(expt1.x-expt2.x)<1e-6 && fabs(expt1.y-expt2.y)<1e-6 )
			{
				delete pObj0;
				pObj0 = NULL;
			}
		}
	}

	//第二段
	pts.RemoveAll();
	{
		CGeoCurveBase *pObj = (CGeoCurveBase*)pCurve->Clone();

		PT_3DEX ptoo = pt1;
		ptoo.pencode = 2;
		pts.Add( ptoo );

		for(i = imin+1; i <= imax; i++)
		{
			PT_3DEX pt = arrPts.GetAt(i);
			pts.Add( pt );
		}
		if( fabs(rpos2-imax) > 1e-4 )
		{
			PT_3DEX ptoo2 = pt2;
			ptoo2.pencode = 2;
			pts.Add( ptoo2 );
		}
		if(!pObj->CreateShape(pts.GetData(),pts.GetSize())) 
			return;
		else
			pObj1 = pObj;
	}

	//第三段
	pts.RemoveAll();
	{
		CGeoCurveBase *pObj = (CGeoCurveBase*)pCurve->Clone();

		PT_3DEX ptoo2 = pt2;
		ptoo2.pencode = 2;
		pts.Add( ptoo2 );
		for(i = imax+1; i < num; i++)
		{
			PT_3DEX pt = arrPts.GetAt(i);
			pts.Add( pt );
		}

		if( pts.GetSize() > 1 )
		{
			if(!pObj->CreateShape(pts.GetData(),pts.GetSize())) 
				return;
			else
				pObj2 = pObj;
		}
		if( pObj2 && pObj2->GetDataPointSum()==2 )
		{
			PT_3DEX expt1, expt2;
			expt1 = pObj2->GetDataPoint(0);
			expt2 = pObj2->GetDataPoint(1);
			
			if( fabs(expt1.x-expt2.x)<1e-6 && fabs(expt1.y-expt2.y)<1e-6 )
			{
				delete pObj2;
				pObj2 = NULL;
			}
		}
	}
	return;
}


void CCutPartCommand::UpdateFtrHiliteDisplay()
{	
	CFtrsHighliteDisplay hd;
	hd.SetDoc(PDOC(m_pEditor));
	
	for (int i=0;i<m_idsRefer.GetSize();i++)
	{
		hd.Add(HandleToFtr(m_idsRefer[i]));
	}

	hd.Highlite();
}


void CCutPartCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	if( m_nStep==0 )
	{
		//by mzy
		if( SELSTAT_DRAGSEL_RESTART == flag)
		{
			int num = -1;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if(num <= 0)
				return;

			m_pEditor->CloseSelector();
			m_nStep = 1;

			PT_3D ptnull;
			memset(&ptnull, 0, sizeof(PT_3D) );
			PtClick(ptnull,0);
		}
		//

		if( (flag&SELSTAT_MULTISEL)!=0 )return;
		if( !(flag==SELSTAT_NONESEL||(flag&SELSTAT_POINTSEL)!=0||(flag&SELSTAT_DRAGSEL)!=0) )
			return; 

	//	if( !CanGetSelObjs(flag) )return;

	//	GOutPut(StrFromResID(IDS_CMDTIP_CLICKOK));

		m_pEditor->CloseSelector();
		
		m_nStep = 1;
	}
	if (m_nStep == 1)
	{
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num<=0 )
		{
			m_pEditor->SelectAll();
			handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		}
		
		if (m_bMulSelMode)
		{
			m_pEditor->CloseSelector();
			m_pDrawProc->Start();
		}
		
		//EditStepOne();
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
		
		m_idsRefer.RemoveAll();
		for (int i=0;i<num;i++)
		{
			m_idsRefer.Add(handles[i]);
		}

		UpdateFtrHiliteDisplay();
		
		m_pEditor->DeselectAll();

		GOutPut(StrFromResID(IDS_TIP_SELCUTPARTFTRS));

		m_nStep = 2;

		if (!m_bMulSelMode)
		{
			m_pEditor->OpenSelector();
		}
	}
	else if( m_nStep==2 )
	{
		if (m_bMulSelMode)
		{
			if (m_pDrawProc)
			{
				GotoState(PROCSTATE_PROCESSING);
				m_pDrawProc->PtClick(pt,flag);
			}
		}
		else
		{
			m_nStep = 3;
		}
	} 
	
	if (m_nStep == 3)
	{
		if (m_bMulSelMode)
		{
			//粗略查找相交地物
			Envelope e, e1;
			CArray<PT_3DEX,PT_3DEX> arrGeo;
			m_pGeoCurve->GetShape(arrGeo);
			int sum = arrGeo.GetSize();
			if (sum < 2)
			{
				Abort();
				return;
			}
			e.CreateFromPts(arrGeo.GetData(),arrGeo.GetSize(),sizeof(PT_3DEX));
			e1 = e;
			e1.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e1,m_pEditor->GetCoordWnd().m_pSearchCS);
			
			int num1;
			const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
			if( !ppftr1  )
			{	
				Abort();
				return;
			}
			CArray<CPFeature,CPFeature> ftr1;
			ftr1.SetSize(num1);
			memcpy(ftr1.GetData(),ppftr1,num1*sizeof(*ppftr1));
			
			//获得有效的对象与两线段的相交信息，并排序
			CArray<CSmoothCommand::OBJ_ITEM,CSmoothCommand::OBJ_ITEM> arr1;
			CSmoothCommand::OBJ_ITEM item;
			CGeometry *pObj;
			const CShapeLine *pSL;
			PT_3DEX *pts;
			int size1;
			for (int l=0; l<sum-1; l++)
			{
				for( int i=0; i<num1; i++)
				{
					size1 = arr1.GetSize();
					for( int k=0; k<size1&&arr1[k].id!=(FTR_HANDLE)ftr1[i]; k++);
					if (k < size1) continue;
					
					pObj = ftr1[i]->GetGeometry();
					if( !pObj )continue;
					
					if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
					if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )continue;
					//if( pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )continue;
					
					//精细检验是否与相交，记录交点
					pSL = pObj->GetShape();
					if( !pSL )continue;
					
					PT_3D ret;
					double t=-1,st0=-1;
					CPtrArray arr;
					pSL->GetShapeLineUnit(arr);
					int pos = 0;
					for( k =0;k<arr.GetSize();k++ )
					{
						CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
						if( e.bIntersect(&pUnit->evlp) )
						{
							pts = pUnit->pts;
							for( int j=0; j<pUnit->nuse-1; j++,pts++)
							{
								if(e.bIntersect(&pts[0],&pts[1]) )
								{
									//求相交点
									if (!GLineIntersectLineSeg1(arrGeo[l].x,arrGeo[l].y,arrGeo[l+1].x,arrGeo[l+1].y,pts[0].x,pts[0].y,pts[0].z,
										pts[1].x,pts[1].y,pts[1].z,&(ret.x),&(ret.y),&(ret.z),&t,&st0))
									{
										continue;
									}							
									
									item.id = FtrToHandle(ftr1[i]);item.pos= pos+j; item.ret= ret; item.t = st0;
									
									arr1.Add(item);
									
									goto FINDOVER_1;
									
								}
							}
						}
						pos += pUnit->nuse;		
					}
FINDOVER_1:;
				}
			}

			CUndoFtrs undo(m_pEditor,Name());

			for (int f=0; f<arr1.GetSize(); f++)
			{
				CFeature *pFtr = (CFeature*)arr1[f].id;
				CGeometry *pObj = pFtr->GetGeometry();
				if( !pObj )continue;

				PT_3D ret = arr1[f].ret;

				double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
				PT_3D ptclt;
				m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&ret,&ptclt);
				Envelope cltEnlp;
				cltEnlp.CreateFromPtAndRadius(ptclt,r);

				BOOL bClosed = FALSE;
				if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pObj)->IsClosed() )
					bClosed = TRUE;

				BOOL bSurface = FALSE;
				CFeature *pOldFtr = NULL, *pNewFtr = NULL;
				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					CDlgDataSource *pDS = GETDS(m_pEditor);
					CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
					if (!pFtrLayer)
					{
						Abort();
						return;
					}
					
					pNewFtr = pFtrLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
					if( pNewFtr==NULL )
					{
						Abort();
						return;
					}
					// 复制固定属性
					CValueTable tab;
					tab.BeginAddValueItem();
					pFtr->WriteTo(tab);
					tab.EndAddValueItem();
					tab.DelField(FIELDNAME_SHAPE);
					tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
					tab.DelField(FIELDNAME_FTRID);
					tab.DelField(FIELDNAME_GEOCLASS);
					pNewFtr->ReadFrom(tab);
					
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pFtr->GetGeometry()->GetShape(arrPts);
					//arrPts.Add(arrPts[0]);
					pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
					
					pOldFtr = pFtr;
					
					pFtr = pNewFtr;
					
					pObj = pFtr->GetGeometry();
					
					bClosed = TRUE;
					bSurface = TRUE;
				}
				
				//---判断当前点位于哪一个相交段
				const CShapeLine  *pBuf = pObj->GetShape(); 
				
				CArray<PT_3DEX,PT_3DEX> arrPts;
				int num = pBuf->GetPtsCount(); 
				if( num<=0 )continue;	
				pBuf->GetPts(arrPts);
				
				
				
				// 1) 找最近的图形点
				PT_3D rpt;
				double x,y,z;
				double dis,min=-1;
				int k;
				
				for( int i=0; i<num-1; i++)
				{
					//求出当前线段的最近距离
					dis = GraphAPI::GGetNearestDisOfPtToLine3D(arrPts[i].x,arrPts[i].y,arrPts[i].z,
						arrPts[i+1].x,arrPts[i+1].y,arrPts[i+1].z,ret.x,ret.y,ret.z,&x,&y,&z,false);
					
					if( min<0 || dis<min )
					{ 
						min = dis; rpt.x=x; rpt.y=y; rpt.z=z; 
						k = i;
					}
				}
				
				// 2) 得到当前点落在基线上的位置变量 t（整数部分表示线段序号，小数部分表示线段上的位置）
				double t = k;
				if( fabs(rpt.x-arrPts[k].x)<1e-4 && fabs(rpt.y-arrPts[k].y)<1e-4 );
				else if( fabs(rpt.x-arrPts[k+1].x)<1e-4 && fabs(rpt.y-arrPts[k+1].y)<1e-4 )
					t += 1;
				else if( fabs(arrPts[k+1].x-arrPts[k].x)<1e-4 )
					t += (rpt.y-arrPts[k].y)/(arrPts[k+1].y-arrPts[k].y);
				else t += (rpt.x-arrPts[k].x)/(arrPts[k+1].x-arrPts[k].x);
				
				// 首先检测当前点是否为相交中间段，若为中间段一起处理，否则一段段处理
				BOOL bRefSelf = FALSE;
				for ( i=0;i<m_idsRefer.GetSize();i++)
				{
					if (m_idsRefer[i] == FTR_HANDLE(pFtr))
					{
						bRefSelf = TRUE;
						break;
					}
				}
				
				//计算所有相交点
				CArray<double,double> arrRet;
				
				if (bRefSelf)
				{
					m_idsRefer.RemoveAt(i);
					GetIntersectPoints(pObj,arrRet);
					
					int nsz = arrRet.GetSize();
					
					for( i=0; i<nsz; i++)
					{
						if( t<arrRet[i] )break;
					}
					
					if (nsz == 0 || i == 0 || i >= nsz)
					{
						m_idsRefer.Add(FTR_HANDLE(pFtr));
						arrRet.RemoveAll();
						GetIntersectPoints(pObj,arrRet);
					}
					else
					{
						m_idsRefer.Add(FTR_HANDLE(pFtr));
					}
				}
				else
				{
					GetIntersectPoints(pObj,arrRet);
				}
				
				if( arrRet.GetSize()<=0 ) continue;

				// 3) 根据位置变量的关系得到相交段
				int nsz = arrRet.GetSize();
				for( i=0; i<nsz; i++)
				{
					if( t<arrRet[i] )break;
				}
				
				// 4) 得到相交段的起始点（交点）
				PT_3D pt1, pt2;
				if( i==0 )
				{
					pt1 = arrPts[0];
					//如果是闭合，相交点应该是最后一个交点
					if( bClosed )
					{
						i = nsz;
						k = (int)(arrRet[i-1]);
						pt1.x = arrPts[k].x + (arrRet[i-1]-k)*(arrPts[k+1].x-arrPts[k].x);
						pt1.y = arrPts[k].y + (arrRet[i-1]-k)*(arrPts[k+1].y-arrPts[k].y);
						pt1.z = arrPts[k].z + (arrRet[i-1]-k)*(arrPts[k+1].z-arrPts[k].z);
					}
				}
				else
				{
					k = (int)(arrRet[i-1]);
					pt1.x = arrPts[k].x + (arrRet[i-1]-k)*(arrPts[k+1].x-arrPts[k].x);
					pt1.y = arrPts[k].y + (arrRet[i-1]-k)*(arrPts[k+1].y-arrPts[k].y);
					pt1.z = arrPts[k].z + (arrRet[i-1]-k)*(arrPts[k+1].z-arrPts[k].z);
				}
				
				if( i>=nsz )
				{
					pt2 =  arrPts[num-1];
					//如果是闭合，相交点应该是第一个交点
					if( bClosed )
					{
						i = 0;
						pt2.x = arrPts[k].x;
						pt2.y = arrPts[k].y;
						pt2.z = arrPts[k].z;
						if(fabs(arrRet[i]-k)>1e-6)
						{
							pt2.x += (arrRet[i]-k)*(arrPts[k+1].x-arrPts[k].x);
							pt2.y += (arrRet[i]-k)*(arrPts[k+1].y-arrPts[k].y);
							pt2.z += (arrRet[i]-k)*(arrPts[k+1].z-arrPts[k].z);
						}
					}
				}
				else
				{
					k = (int)(arrRet[i]);
					pt2.x = arrPts[k].x;
					pt2.y = arrPts[k].y;
					pt2.z = arrPts[k].z;
					if(fabs(arrRet[i]-k)>1e-6)
					{
						pt2.x += (arrRet[i]-k)*(arrPts[k+1].x-arrPts[k].x);
						pt2.y += (arrRet[i]-k)*(arrPts[k+1].y-arrPts[k].y);
						pt2.z += (arrRet[i]-k)*(arrPts[k+1].z-arrPts[k].z);
					}
				}
				
				//	delete[] pts;
				
				//根据起始去除该段，并生成新地物
				if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					CGeoCurve *pCurve = (CGeoCurve*)pObj;
					CGeometry *pObjs[3] = {0};
					pCurve->GetBreak(pt1,pt2,pObjs[0],pObjs[1],pObjs[2]);
					
					//-------将需要删除的部分删除，需要生成的部分保留或者合并
					//删除中间部分
					BOOL bDelMid = FALSE;
					if( !bClosed )
					{
						if (!m_bKeepCutPart)
						{
							if( pObjs[1] )delete pObjs[1]; pObjs[1] = NULL;
						}
					}
					//如果地物处于闭合时，删除的那部分可能并不是中间的一部分，这里做一次判断
					else
					{
						if( pObjs[1] )
						{
							PT_3D tpt;
							if( (pObjs[1])->FindNearestBasePt(ptclt,cltEnlp,m_pEditor->GetCoordWnd().m_pSearchCS,&tpt,NULL) )
							{
								//if( fabs(tpt.x-rpt.x)<1e-4 && fabs(tpt.y-rpt.y)<1e-4 )
								{
									bDelMid = TRUE;
								}
							}
						}
						
						//首尾合并
						if( bDelMid )
						{
							if (!m_bKeepCutPart)
							{
								if( pObjs[1] )delete pObjs[1]; pObjs[1] = NULL;
							}
							
							//不能全空
							if( pObjs[0]==NULL && pObjs[2]==NULL )return;					
							if( pObjs[0] && pObjs[2] )
							{
								int nkeypt = pObjs[0]->GetDataPointSum();
								//	PT_3DEX expt;
								CArray<PT_3DEX,PT_3DEX> pts;
								pObjs[2]->GetShape(pts);
								for( i=0; i<nkeypt; i++ )
								{
									pts.Add(pObjs[0]->GetDataPoint(i));
								}
								pObjs[2]->CreateShape(pts.GetData(),pts.GetSize());
								delete pObjs[0]; pObjs[0] = NULL;
							}
						}
						//首尾删除
						else
						{
							if (!m_bKeepCutPart)
							{
								if( pObjs[0] )delete pObjs[0]; pObjs[0] = NULL;
								if( pObjs[2] )delete pObjs[2]; pObjs[2] = NULL;	
							}
						}
					}
					
					//不能全空
					if( pObjs[0]==NULL && pObjs[1]==NULL && pObjs[2]==NULL )return;
					
					if (pNewFtr)
					{
						pFtr = pOldFtr;
						delete pNewFtr;
					}

					CDlgDataSource *pDS = GETDS(m_pEditor);
					CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
					if (!pFtrLayer)
					{
						Abort();
						return;
					}
					
					int nLayerIdOfKeep = pFtrLayer->GetID();
					if (m_bKeepCutPart)
					{						
						CString strKeepLayerName;
						strKeepLayerName.Format("%s_%s",pFtrLayer->GetName(),Name());
						CFtrLayer *pKeepFtrLayer = pDS->GetFtrLayer(strKeepLayerName);
						
						if (pKeepFtrLayer == NULL)
						{				
							pKeepFtrLayer = pDS->CreateFtrLayer(strKeepLayerName);
							if (!pKeepFtrLayer) 
							{
								Abort();
								return;
							}
							pKeepFtrLayer->SetColor(pFtrLayer->GetColor());
							pDS->AddFtrLayer(pKeepFtrLayer);			
						}
						
						nLayerIdOfKeep = pKeepFtrLayer->GetID();
					}

					if( pObjs[0] )
					{
						CFeature *pF = pFtr->Clone();
						pF->SetID(OUID());
						BOOL bVisible = (!bClosed||bDelMid);
						pF->EnableVisible(bVisible);
						pF->SetToDeleted(FALSE);
						if (m_bCutpartSurface)
						{
							pF->SetGeometry(pObjs[0]);
						}
						else
						{
							CArray<PT_3DEX,PT_3DEX> pts0;
							pObjs[0]->GetShape(pts0);
							CGeoCurve *pCurve = new CGeoCurve();
							pCurve->CreateShape(pts0.GetData(),pts0.GetSize());
							pF->SetGeometry(pCurve);
							delete pObjs[0];
						}
						int layid = bVisible?m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)):nLayerIdOfKeep;
						m_pEditor->AddObject(pF,layid);
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pF);
						undo.arrNewHandles.Add(FtrToHandle(pF));
					}
					if( pObjs[1] )
					{
						CFeature *pF = pFtr->Clone();
						pF->SetID(OUID());
						BOOL bVisible = (bClosed&&!bDelMid);
						pF->EnableVisible(bVisible);
						pF->SetToDeleted(FALSE);
						if (m_bCutpartSurface)
						{
							pF->SetGeometry(pObjs[1]);
						}
						else
						{
							CArray<PT_3DEX,PT_3DEX> pts0;
							pObjs[1]->GetShape(pts0);
							CGeoCurve *pCurve = new CGeoCurve();
							pCurve->CreateShape(pts0.GetData(),pts0.GetSize());
							pF->SetGeometry(pCurve);
							delete pObjs[1];
						}
						int layid = bVisible?m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)):nLayerIdOfKeep;
						m_pEditor->AddObject(pF,layid);
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pF);
						undo.arrNewHandles.Add(FtrToHandle(pF));
					}
					if( pObjs[2] )
					{
						CFeature *pF = pFtr->Clone();
						pF->SetID(OUID());
						BOOL bVisible = (!bClosed||bDelMid);
						pF->EnableVisible(bVisible);
						pF->SetToDeleted(FALSE);
						if (m_bCutpartSurface)
						{
							pF->SetGeometry(pObjs[2]);
						}
						else
						{
							CArray<PT_3DEX,PT_3DEX> pts0;
							pObjs[2]->GetShape(pts0);
							CGeoCurve *pCurve = new CGeoCurve();
							pCurve->CreateShape(pts0.GetData(),pts0.GetSize());
							pF->SetGeometry(pCurve);
							delete pObjs[2];
						}
						int layid = bVisible?m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)):nLayerIdOfKeep;
						m_pEditor->AddObject(pF,layid);
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pF);
						undo.arrNewHandles.Add(FtrToHandle(pF));
					}
					
					m_pEditor->DeleteObject(FtrToHandle(pFtr));
					undo.arrOldHandles.Add(FtrToHandle(pFtr));

					int numRef = m_idsRefer.GetSize();
					for (int k=numRef-1; k>=0; k--)
					{
						if (m_idsRefer[k] == FtrToHandle(pFtr))
						{
							m_idsRefer.RemoveAt(k);
							for (int m=0; m<undo.arrNewHandles.GetSize(); m++)
							{
								m_idsRefer.Add(undo.arrNewHandles[m]);
							}
							
							break;
						}
					}
								
				}
				
			}

			undo.Commit();

			UpdateFtrHiliteDisplay();		

			m_pEditor->DeselectAll();
// 			for (i=0;i<m_idsRefer.GetSize();i++)
// 			{
// 				m_pEditor->SelectObj(m_idsRefer[i]);
// 			}
			m_pEditor->OnSelectChanged();
			m_pEditor->RefreshView();
			m_nStep = 2;

// 			m_nStep = 2;
// 			Finish();
// 			m_idsRefer.RemoveAll();

		}
		else
		{
			//找到点中的地物	
			double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
			PT_3D p1;
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
			Envelope e;
			e.CreateFromPtAndRadius(p1,r);
			CPFeature pFtr = m_pEditor->GetDataQuery()->FindNearestObject(p1,r,m_pEditor->GetCoordWnd().m_pSearchCS);
			if( !pFtr )return;
			//if( m_pEditor->GetSelection()->IsObjInSelection(FtrToHandle(pFtr)) )return;
			
			CGeometry *pObj = pFtr->GetGeometry();
			if( !pObj )return;
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )return;
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )return;
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )return;
			
			BOOL bClosed = FALSE;
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pObj)->IsClosed() )
				bClosed = TRUE;


			BOOL bSurface = FALSE;
			CFeature *pOldFtr = NULL, *pNewFtr = NULL;
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				CDlgDataSource *pDS = GETDS(m_pEditor);
				CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
				if (!pFtrLayer)
				{
					Abort();
					return;
				}

				pNewFtr = pFtrLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
				if( pNewFtr==NULL )
				{
					Abort();
					return;
				}
				// 复制固定属性
				CValueTable tab;
				tab.BeginAddValueItem();
				pFtr->WriteTo(tab);
				tab.EndAddValueItem();
				tab.DelField(FIELDNAME_SHAPE);
				tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
				tab.DelField(FIELDNAME_FTRID);
				tab.DelField(FIELDNAME_GEOCLASS);
				pNewFtr->ReadFrom(tab);
				
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pFtr->GetGeometry()->GetShape(arrPts);
				//arrPts.Add(arrPts[0]);
				pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());

				pOldFtr = pFtr;

				pFtr = pNewFtr;
				
				pObj = pFtr->GetGeometry();

				bClosed = TRUE;
				bSurface = TRUE;
			}
			
			//---判断当前点位于哪一个相交段
			const CShapeLine  *pBuf = pObj->GetShape(); 
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			int num = pBuf->GetPtsCount(); 
			if( num<=0 )return;	
			pBuf->GetPts(arrPts);
			
			
			
			// 1) 找最近的图形点
			PT_3D rpt;
			double x,y,z;
			double dis,min=-1;
			int k;
			
			for( int i=0; i<num-1; i++)
			{
				//求出当前线段的最近距离
				dis = GraphAPI::GGetNearestDisOfPtToLine3D(arrPts[i].x,arrPts[i].y,arrPts[i].z,
					arrPts[i+1].x,arrPts[i+1].y,arrPts[i+1].z,pt.x,pt.y,pt.z,&x,&y,&z,false);
				
				if( min<0 || dis<min )
				{ 
					min = dis; rpt.x=x; rpt.y=y; rpt.z=z; 
					k = i;
				}
			}
			
			// 2) 得到当前点落在基线上的位置变量 t（整数部分表示线段序号，小数部分表示线段上的位置）
			double t = k;
			if( fabs(rpt.x-arrPts[k].x)<1e-4 && fabs(rpt.y-arrPts[k].y)<1e-4 );
			else if( fabs(rpt.x-arrPts[k+1].x)<1e-4 && fabs(rpt.y-arrPts[k+1].y)<1e-4 )
				t += 1;
			else if( fabs(arrPts[k+1].x-arrPts[k].x)<1e-4 )
				t += (rpt.y-arrPts[k].y)/(arrPts[k+1].y-arrPts[k].y);
			else t += (rpt.x-arrPts[k].x)/(arrPts[k+1].x-arrPts[k].x);
			
			// 首先检测当前点是否为相交中间段，若为中间段一起处理，否则一段段处理
			BOOL bRefSelf = FALSE;
			for ( i=0;i<m_idsRefer.GetSize();i++)
			{
				if (m_idsRefer[i] == FTR_HANDLE(pFtr))
				{
					bRefSelf = TRUE;
					break;
				}
			}
			
			//计算所有相交点
			CArray<double,double> arrRet;

			if (bRefSelf)
			{
				m_idsRefer.RemoveAt(i);
				GetIntersectPoints(pObj,arrRet);

				int nsz = arrRet.GetSize();

				for( i=0; i<nsz; i++)
				{
					if( t<arrRet[i] )break;
				}

				if (nsz == 0 || i == 0 || i >= nsz)
				{
					m_idsRefer.Add(FTR_HANDLE(pFtr));
					arrRet.RemoveAll();
					GetIntersectPoints(pObj,arrRet);
				}
				else
				{
					m_idsRefer.Add(FTR_HANDLE(pFtr));
				}
			}
			else
			{
				GetIntersectPoints(pObj,arrRet);
			}

			if( arrRet.GetSize()<=0 )return;


			// 3) 根据位置变量的关系得到相交段
			int nsz = arrRet.GetSize();
			for( i=0; i<nsz; i++)
			{
				if( t<arrRet[i] )break;
			}
			
			// 4) 得到相交段的起始点（交点）
			PT_3D pt1, pt2;
			if( i==0 )
			{
				pt1 = arrPts[0];
				//如果是闭合，相交点应该是最后一个交点
				if( bClosed )
				{
					i = nsz;
					k = (int)(arrRet[i-1]);
					pt1.x = arrPts[k].x + (arrRet[i-1]-k)*(arrPts[k+1].x-arrPts[k].x);
					pt1.y = arrPts[k].y + (arrRet[i-1]-k)*(arrPts[k+1].y-arrPts[k].y);
					pt1.z = arrPts[k].z + (arrRet[i-1]-k)*(arrPts[k+1].z-arrPts[k].z);
				}
			}
			else
			{
				k = (int)(arrRet[i-1]);
				pt1.x = arrPts[k].x + (arrRet[i-1]-k)*(arrPts[k+1].x-arrPts[k].x);
				pt1.y = arrPts[k].y + (arrRet[i-1]-k)*(arrPts[k+1].y-arrPts[k].y);
				pt1.z = arrPts[k].z + (arrRet[i-1]-k)*(arrPts[k+1].z-arrPts[k].z);
			}
			
			if( i>=nsz )
			{
				pt2 =  arrPts[num-1];
				//如果是闭合，相交点应该是第一个交点
				if( bClosed )
				{
					i = 0;
					k = (int)(arrRet[i]);
					pt2.x = arrPts[k].x + (arrRet[i]-k)*(arrPts[k+1].x-arrPts[k].x);
					pt2.y = arrPts[k].y + (arrRet[i]-k)*(arrPts[k+1].y-arrPts[k].y);
					pt2.z = arrPts[k].z + (arrRet[i]-k)*(arrPts[k+1].z-arrPts[k].z);
				}
			}
			else
			{
				k = (int)(arrRet[i]);
				if (k == num - 1)
				{
					pt2.x = arrPts[k].x;
					pt2.y = arrPts[k].y;
					pt2.z = arrPts[k].z;
				}
				else
				{
					pt2.x = arrPts[k].x + (arrRet[i] - k)*(arrPts[k + 1].x - arrPts[k].x);
					pt2.y = arrPts[k].y + (arrRet[i] - k)*(arrPts[k + 1].y - arrPts[k].y);
					pt2.z = arrPts[k].z + (arrRet[i] - k)*(arrPts[k + 1].z - arrPts[k].z);
				}
			}
			
			//	delete[] pts;
			
			//根据起始去除该段，并生成新地物
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			{
				CGeoCurve *pCurve = (CGeoCurve*)pObj;
				CGeometry *pObjs[3] = {0};
			//	pCurve->GetBreak(pt1,pt2,pObjs[0],pObjs[1],pObjs[2]);

				if( IsSelfIntersect(pObj) && !bClosed )
				{
					PT_3DEX ptex1, ptex2;
					ptex1.x = pt1.x; ptex1.y = pt1.y; ptex1.z = pt1.z;
					ptex2.x = pt2.x; ptex2.y = pt2.y; ptex2.z = pt2.z;
					BreakLine(ptex1, ptex2, pCurve, pObjs[0],pObjs[1],pObjs[2]);
				}
				else
					pCurve->GetBreak(pt1,pt2,pObjs[0],pObjs[1],pObjs[2]);

				//-------将需要删除的部分删除，需要生成的部分保留或者合并
				//删除中间部分
				BOOL bDelMid = FALSE;
				if( !bClosed )
				{
					if (!m_bKeepCutPart)
					{
						if( pObjs[1] )delete pObjs[1]; pObjs[1] = NULL;
					}
				}
				//如果地物处于闭合时，删除的那部分可能并不是中间的一部分，这里做一次判断
				else
				{
					if( pObjs[1] )
					{
						PT_3D p1;
						m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
						PT_3D tpt;
						if( (pObjs[1])->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&tpt,NULL) )
						{
							//if( fabs(tpt.x-rpt.x)<1e-4 && fabs(tpt.y-rpt.y)<1e-4 )
							{
								bDelMid = TRUE;
							}
						}
					}
					
					//首尾合并
					if( bDelMid )
					{
						if (!m_bKeepCutPart)
						{
							if( pObjs[1] )delete pObjs[1]; pObjs[1] = NULL;
						}
						
						//不能全空
						if( pObjs[0]==NULL && pObjs[2]==NULL )return;					
						if( pObjs[0] && pObjs[2] )
						{
							int nkeypt = pObjs[0]->GetDataPointSum();
							//	PT_3DEX expt;
							CArray<PT_3DEX,PT_3DEX> pts;
							pObjs[2]->GetShape(pts);

							//by mzy相接处的点可能为闭合地物最初的起始点，这里判断一下
							PT_3DEX ptof0 = pObjs[0]->GetDataPoint(0);
							PT_3DEX ptof2 = pts.GetAt( pts.GetSize()-1 );
							i = 0;
							if( fabs(ptof0.x-ptof2.x)<1e-4 && fabs(ptof0.y-ptof2.y)<1e-4 && fabs(ptof0.z-ptof2.z)<1e-4 )
								i = 1;
							//

							for( /*i=0*/; i<nkeypt; i++ )
							{
								pts.Add(pObjs[0]->GetDataPoint(i));
							}
							pObjs[2]->CreateShape(pts.GetData(),pts.GetSize());
							delete pObjs[0]; pObjs[0] = NULL;
						}
					}
					//首尾删除
					else
					{
						if (!m_bKeepCutPart)
						{
							if( pObjs[0] )delete pObjs[0]; pObjs[0] = NULL;
							if( pObjs[2] )delete pObjs[2]; pObjs[2] = NULL;	
						}		
					}
				}
				
				//不能全空
				if( pObjs[0]==NULL && pObjs[1]==NULL && pObjs[2]==NULL )return;

				if (pNewFtr)
				{
					pFtr = pOldFtr;
					delete pNewFtr;
				}

				CUndoFtrs undo(m_pEditor,Name());

				CDlgDataSource *pDS = GETDS(m_pEditor);
				CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
				if (!pFtrLayer)
				{
					Abort();
					return;
				}

				int nLayerIdOfKeep = pFtrLayer->GetID();
				if (m_bKeepCutPart)
				{		
					CString strKeepLayerName;
					strKeepLayerName.Format("%s_%s",pFtrLayer->GetName(),Name());
					CFtrLayer *pKeepFtrLayer = pDS->GetFtrLayer(strKeepLayerName);
					
					if (pKeepFtrLayer == NULL)
					{				
						pKeepFtrLayer = pDS->CreateFtrLayer(strKeepLayerName);
						if (!pKeepFtrLayer) 
						{
							Abort();
							return;
						}
						pKeepFtrLayer->SetColor(pFtrLayer->GetColor());
						pDS->AddFtrLayer(pKeepFtrLayer);			
					}

					nLayerIdOfKeep = pKeepFtrLayer->GetID();
				}

				if( pObjs[0] )
				{
					CFeature *pF = pFtr->Clone();
					pF->SetID(OUID());
					BOOL bVisible = (!bClosed||bDelMid);
					pF->EnableVisible(bVisible);
					pF->SetToDeleted(FALSE);
					if (m_bCutpartSurface)
					{
						pF->SetGeometry(pObjs[0]);
					}
					else
					{
						CArray<PT_3DEX,PT_3DEX> pts0;
						pObjs[0]->GetShape(pts0);
						CGeoCurve *pCurve = new CGeoCurve();
						pCurve->CreateShape(pts0.GetData(),pts0.GetSize());
						pF->SetGeometry(pCurve);
						delete pObjs[0];
					}
					
					int layid = bVisible?m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)):nLayerIdOfKeep;
					m_pEditor->AddObject(pF,layid);
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pF);
					undo.arrNewHandles.Add(FtrToHandle(pF));
				}
				if( pObjs[1] )
				{
					CFeature *pF = pFtr->Clone();
					pF->SetID(OUID());
					BOOL bVisible = (bClosed&&!bDelMid);
					pF->EnableVisible(bVisible);
					pF->SetToDeleted(FALSE);
					if (m_bCutpartSurface)
					{
						pF->SetGeometry(pObjs[1]);
					}
					else
					{
						CArray<PT_3DEX,PT_3DEX> pts0;
						pObjs[1]->GetShape(pts0);
						CGeoCurve *pCurve = new CGeoCurve();
						pCurve->CreateShape(pts0.GetData(),pts0.GetSize());
						pF->SetGeometry(pCurve);
						delete pObjs[1];
					}
					int layid = bVisible?m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)):nLayerIdOfKeep;
					m_pEditor->AddObject(pF,layid);
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pF);
					undo.arrNewHandles.Add(FtrToHandle(pF));
				}
				if( pObjs[2] )
				{
					CFeature *pF = pFtr->Clone();
					pF->SetID(OUID());
					BOOL bVisible = (!bClosed||bDelMid);
					pF->EnableVisible(bVisible);
					pF->SetToDeleted(FALSE);
					if (m_bCutpartSurface)
					{
						pF->SetGeometry(pObjs[2]);
					}
					else
					{
						CArray<PT_3DEX,PT_3DEX> pts0;
						pObjs[2]->GetShape(pts0);
						CGeoCurve *pCurve = new CGeoCurve();
						pCurve->CreateShape(pts0.GetData(),pts0.GetSize());
						pF->SetGeometry(pCurve);
						delete pObjs[2];
					}
					int layid = bVisible?m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)):nLayerIdOfKeep;
					m_pEditor->AddObject(pF,layid);
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pF);
					undo.arrNewHandles.Add(FtrToHandle(pF));
				}

				m_pEditor->DeleteObject(FtrToHandle(pFtr));
				undo.arrOldHandles.Add(FtrToHandle(pFtr));
				
				undo.Commit();

				int numRef = m_idsRefer.GetSize();
				for (int k=numRef-1; k>=0; k--)
				{
					if (m_idsRefer[k] == FtrToHandle(pFtr))
					{
						m_idsRefer.RemoveAt(k);
						for (int m=0; m<undo.arrNewHandles.GetSize(); m++)
						{
							m_idsRefer.Add(undo.arrNewHandles[m]);
						}
						
						break;
					}
				}
			}

			m_pEditor->DeselectAll();

			UpdateFtrHiliteDisplay();		
// 			for (i=0;i<m_idsRefer.GetSize();i++)
// 			{
// 				m_pEditor->SelectObj(m_idsRefer[i]);
// 			}
			m_pEditor->OnSelectChanged();
			m_pEditor->RefreshView();
			m_nStep = 2;

// 			m_nStep = 2;
// 			Finish();
// 			m_idsRefer.RemoveAll();
		}
	}	

	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CReplaceLinesCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CReplaceLinesCommand,CDrawCommand)

CReplaceLinesCommand::CReplaceLinesCommand()
{
//	m_bAutoClosed = FALSE;
	m_nMode = modeTwoPoint;
	m_bShowMark = TRUE;
	
	m_bExtendCollect = FALSE;
	
	m_nWaitSelect = 0;
	m_nStep = -1;
	m_objOldID1 = 0;
	m_objOldID2 = 0;
    m_pObj = NULL;
		
	m_pOldObj1 = NULL;
	m_pOldObj2 = NULL;
	m_nStart = -1;
	m_nEnd = -1;
	m_bCanTwoObjs = TRUE;
	m_bMatchHeight = TRUE;
	
	m_nSaveViewParam = 0;
	m_bUseViewParam = FALSE;
	
	m_nSnapRadius = 20;
	m_bVWsnaplinestyle = FALSE;
	m_b3Dsnaplinestyle = TRUE;
	
	m_nVWPen = penLine;
	m_n3DPen = penLine;

	m_pDrawProc = NULL;
	strcat(m_strRegPath,"\\ReplaceLines");

}

CReplaceLinesCommand::~CReplaceLinesCommand()
{

	if( m_pDrawProc )delete m_pDrawProc;
	if (m_pObj)
	{
		delete m_pObj;
		m_pObj = NULL;
	}
}


CProcedure *CReplaceLinesCommand::GetActiveSonProc(int nMsgType)
{
	if( nMsgType==msgPtClick || nMsgType==msgPtMove || nMsgType==msgPtReset )
		return NULL;
	
	return m_pDrawProc;
}

void CReplaceLinesCommand::OnSonEnd(CProcedure *son)
{
	if( !m_pObj||m_pObj->GetDataPointSum()<2 )
	{
		Abort();
		return;
	}
	if(m_nMode==modeTwoPoint)
	{
		//当地物有共线的时候，目标地物(m_pOldObj1)可能会选错，而此时如果修测线的首尾两点都落在同一个地物上时，则这个地物应该是目标地物
		if(1)
		{
			CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

			BOOL bRet1 = FALSE, bRet2 = FALSE;
			PT_3D pt1, pt2, pt0;
			Envelope e;
			double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&pt0);
			e.CreateFromPtAndRadius(pt0,r);
			bRet1 = m_pOldObj1->GetGeometry()->FindNearestBasePt(pt0,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt1,NULL);

			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&pt0);
			e.CreateFromPtAndRadius(pt0,r);
			CPFeature pObj2 = pDQ->FindNearestObject(pt0,r,m_pEditor->GetCoordWnd().m_pSearchCS);
			if(pObj2)
				bRet2 = pObj2->GetGeometry()->FindNearestBasePt(pt0,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt2,NULL);

			//寻找所有经过pt1和pt2的地物，进行修测		
			e.CreateFromPtAndRadius(pt1,GraphAPI::GetDisTolerance());
			pDQ->FindObjectInRect(e,NULL);
			CFtrArray arr1, arr2;
			int num = 0;
			const CPFeature *ftrs = pDQ->GetFoundHandles(num);
			if(num>0)
			{
				arr1.SetSize(num);
				memcpy(arr1.GetData(),ftrs,num*sizeof(CPFeature));			
			}

			e.CreateFromPtAndRadius(pt2,GraphAPI::GetDisTolerance());
			pDQ->FindObjectInRect(e,NULL);
			ftrs = pDQ->GetFoundHandles(num);

			if(num>0)
			{
				arr2.SetSize(num);
				memcpy(arr2.GetData(),ftrs,num*sizeof(CPFeature));			
			}

			if(!bRet1 || !bRet2)
			{
				arr1.RemoveAll();
			}

			//清除未同时经过pt1,pt2的地物
			for(int i=arr1.GetSize()-1; i>=0; i--)
			{
				for(int j=0; j<arr2.GetSize(); j++)
				{
					if(arr2[j]==arr1[i])break;
				}
				if(j>=arr2.GetSize())
				{
					arr1.RemoveAt(i);
				}
			}
			
			if(arr1.GetSize()>0)
			{
				m_objOldID1 = FtrToHandle(arr1[0]);
				m_pOldObj1 = arr1[0];				
				m_objOldID2 = NULL_HANDLE;
				m_pOldObj2 = NULL;
			}
		}

		CGeometry *pObj = GetReplaceObj();
		if (!pObj)
		{
			Finish();
			return;
		}
			
		m_pEditor->UpdateDrag(ud_ClearDrag);	
		
		CFeature *pFtr = HandleToFtr(m_objOldID1)->Clone();
		if (!pFtr) return;

		CUndoFtrs undo(m_pEditor,Name());
		
		int layid = m_pEditor->GetFtrLayerIDOfFtr(m_objOldID1);
		pFtr->SetID(OUID());
		pFtr->SetGeometry(pObj);
		if(m_pEditor->AddObject(pFtr,layid))
		{
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objOldID1),pFtr);
			undo.AddNewFeature(FtrToHandle(pFtr));
		}
		if(m_pEditor->DeleteObject(m_objOldID1))
		{
			undo.AddOldFeature(m_objOldID1);
		}
		if(m_pEditor->DeleteObject(m_objOldID2))
		{
			undo.AddOldFeature(m_objOldID2);
		}		
		
		undo.Commit();
		Finish();
	}
	else
	{
		if (m_objOldID1==0&&m_objOldID2==0)
		{
			Abort();
			return;
		}
		if( m_pDrawProc )
		{
			delete m_pDrawProc;
			m_pDrawProc = NULL;
		}
		GrBuffer buf;
		m_pObj->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
		m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
	}
}


void CReplaceLinesCommand::PtReset(PT_3D &pt)
{
	if( m_pDrawProc )
	{
		m_pDrawProc->PtReset(pt);
	}
	//画线操作结束
	if( m_nStep==1 )
	{
		if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) || m_pDrawProc->m_pGeoCurve->GetDataPointSum()<2)
		{
			Abort();
			return;
		}
		m_pObj = m_pDrawProc->m_pGeoCurve->Clone();
		//获得终点
		PT_3DEX expt;
		expt = m_pObj->GetDataPoint(m_pObj->GetDataPointSum()-1);
		COPY_3DPT(m_ptEnd,expt);
		
		//制作替换线
		m_nWaitSelect = 0;
		if( m_nMode==modeManyPoint )
		{
			if( m_pOldObj2 && m_pOldObj1!=m_pOldObj2 )
				m_nWaitSelect = 2;
			else m_nWaitSelect = 1;			
		}
	
		//需要进一步确认
		if( m_nWaitSelect>0 )
		{			
			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCursorType,(CObject*)CURSOR_SELECT);
			if( m_nWaitSelect==1 )
				PromptString(StrFromResID(IDS_CMDTIP_SELECTTOSAVE));
			else
				PromptString(StrFromResID(IDS_CMDTIP_SELECTTOSAVE1));
			m_nStep = 2;
			return;
		}	
	}
	if (m_nStep>=2)
	{
		CDrawCommand::PtReset(pt);
	}
}

CString CReplaceLinesCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_REPLACE);
}

void CReplaceLinesCommand::Start()
{
	m_nStep = 0;
	m_nMode = modeTwoPoint;
	m_bShowMark = TRUE;
	m_nWaitSelect = 0;
	m_objOldID1 = 0;
	m_objOldID2 = 0;
	//	m_objNewID = 0;
	m_pOldObj1 = NULL;
	m_pOldObj2 = NULL;
	m_nStart = -1;
	m_nEnd = -1;
	m_bCanTwoObjs = TRUE;
	m_bMatchHeight= TRUE;
	m_pObj = NULL;
	
	m_nSaveViewParam = 0;
	m_bUseViewParam = FALSE;
	m_nSnapRadius = 20;
	m_bVWsnaplinestyle = TRUE;
	m_b3Dsnaplinestyle = TRUE;	

	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	UpdateParams(FALSE);
	m_pObj = new CGeoCurve;
	if(!m_pObj)return ;
	m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pObj;
	m_pDrawProc->Start(); 	

	if (m_pDrawProc)
	{
		if( !m_pEditor->GetCoordWnd().m_bIsStereo && !m_bVWsnaplinestyle )
			m_pDrawProc->m_nCurPenCode = m_nVWPen;
		else if( m_pEditor->GetCoordWnd().m_bIsStereo && !m_b3Dsnaplinestyle )
			m_pDrawProc->m_nCurPenCode = m_n3DPen;
		else
			m_pDrawProc->m_nCurPenCode = penLine;
	}
	
	CCommand::Start();
}

void CReplaceLinesCommand::Abort()
{
	if( m_bUseViewParam )
	{
		char params[256]={0};
		sprintf(params,"%d",m_nSaveViewParam);

		PDOC(m_pEditor)->UpdateAllViewsParams(0,(LPARAM)params);

// 		CView *pView = PDOC(m_pEditor)->GetCurActiveView();
// 		if (pView)  pView->SendMessage(FCCM_UPDATESTEREOPARAM, 0, (LPARAM)params);

	}

	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}	
	if(m_pObj)
	{
		delete m_pObj;
		m_pObj = NULL;
	}
	if( m_bUseViewParam )
	{
		char params[256]={0};
		sprintf(params,"%d",m_nSaveViewParam);
	//	AfxCallMessage(FCCM_UPDATESTEREOPARAM,0,(LPARAM)params);
	}
	CDrawCommand::Abort();

}

void CReplaceLinesCommand::Finish()
{

	if( m_bUseViewParam )
	{
		char params[256]={0};
		sprintf(params,"%d",m_nSaveViewParam);
		
		PDOC(m_pEditor)->UpdateAllViewsParams(0,(LPARAM)params);

// 		CView *pView = PDOC(m_pEditor)->GetCurActiveView();
// 		if (pView)  pView->SendMessage(FCCM_UPDATESTEREOPARAM, 0, (LPARAM)params);
		
	}

	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pObj)
	{
		delete m_pObj;
		m_pObj = NULL;
	}
	
//	m_pDoc->UpdateAllViews(NULL,hc_Detach_Accubox);
	m_pEditor->RefreshView();
	
	m_nStep = -1;
	
	CCommand::Finish();
}

  
void CReplaceLinesCommand::PtClick(PT_3D &pt, int flag)
{
	//输入起点
	if( m_nStep==0 )
	{
		m_ptStart = pt;

		//当前选中的地物
		int num1;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num1);
		
		//吸取绘图钢笔码
		PT_3D pt1,pt2,ptnearest;
		double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
		Envelope e ;
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&pt1);
		e.CreateFromPtAndRadius(pt1,r);
		m_objOldID1 =FtrToHandle(m_pEditor->GetDataQuery()->FindNearestObject(pt1,r,m_pEditor->GetCoordWnd().m_pSearchCS));	
		int num2;
		const CPFeature* ftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num2);

		for(int i=0; i<num2; i++)
		{
			for(int j=0; j<num1; j++)
			{
				if(HandleToFtr(handles[j]) == ftrs[i])
				{
					m_objOldID1 = handles[j];
					break;
				}
			}
		}
		if( !m_objOldID1 )return;
		
		m_pOldObj1 = HandleToFtr(m_objOldID1);
		if( !m_pOldObj1 || !(m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
			return;
		
		if( m_pOldObj1->GetGeometry()->FindNearestBasePt(pt1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt2,NULL) )
		{
			Envelope ee;
			ee.CreateMaxEnvelope();
			int pos ;
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt2,&pt1);
			m_pOldObj1->GetGeometry()->GetShape()->FindNearestLine(pt1,ee,m_pEditor->GetCoordWnd().m_pSearchCS,NULL,NULL,NULL,&pos);
			
			//得到前面的关键点的绘图钢笔码以及高程
			CArray<int,int> pKeyPos;
			m_pOldObj1->GetGeometry()->GetShape()->GetKeyPosOfBaseLines(pKeyPos);
			if( pKeyPos.GetSize()>0 )
			{
				int num = m_pOldObj1->GetGeometry()->GetDataPointSum()-1;
				while( num>=0 && pKeyPos[num]>pos )num--;
				
				if( num>=0 )
				{
					PT_3DEX expt;
					expt = m_pOldObj1->GetGeometry()->GetDataPoint(num);
					if(!m_pEditor->GetCoordWnd().m_bIsStereo)
					{
						if(m_bVWsnaplinestyle)//如果m_bVWsnaplinestyle是true，则吸取，否则不吸取，而根据工具栏的选择确定
							m_pDrawProc->m_nCurPenCode = expt.pencode;
						else
							m_pDrawProc->m_nCurPenCode = m_nVWPen;
					}
					else
					{
						if(m_b3Dsnaplinestyle)//如果m_bVWsnaplinestyle是true，则吸取，否则不吸取，而根据工具栏的选择确定
							m_pDrawProc->m_nCurPenCode = expt.pencode;
						else
							m_pDrawProc->m_nCurPenCode = m_n3DPen;
					}
					
					
					// 保存立体视图参数
					char params[256]={0};
					PDOC(m_pEditor)->UpdateAllViewsParams(1,(LPARAM)params);

					if( sscanf(params,"%d",&m_nSaveViewParam)==1 )
						m_bUseViewParam = TRUE;

					/*CView *pView = PDOC(m_pEditor)->GetCurActiveView();
					if (pView)
					{
						pView->SendMessage(FCCM_UPDATESTEREOPARAM, 1, (LPARAM)params);
						if( sscanf(params,"%d",&m_nSaveViewParam)==1 )
							m_bUseViewParam = TRUE;
					}*/

					// 设置目标层码对应的视图参数
					CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
					if (!pDS){ Abort(); return;}
					int nLayerId = PDOC(m_pEditor)->GetFtrLayerIDOfFtr(m_objOldID1);
					CFtrLayer *pLayer = pDS->GetFtrLayer(nLayerId);
					if (pLayer)
					{
						CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(pDS->GetScale());
						if (pPlaceLib)
						{
							CPlaceConfig *pConfig = pPlaceLib->GetConfig(pLayer->GetName());
							if (pConfig/* && pView*/)
							{
								PDOC(m_pEditor)->UpdateAllViewsParams(0,(LPARAM)pConfig->m_strViewParams);
								//pView->SendMessage(FCCM_UPDATESTEREOPARAM, 0, (LPARAM)pConfig->m_strViewParams);
							}
						}		
					
					}
					
					//立体窗口
					if( m_pEditor->GetCoordWnd().m_bIsStereo )
					{
						((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_ModifyHeight,(CObject*)&pt2.z);
					}

					m_ptStart.z = pt2.z;
					pt.z = pt2.z;
				}
			}			
			
		} 	
		
		long color = m_pOldObj1->GetGeometry()->GetColor();
		if (color == FTRCOLOR_BYLAYER)
		{
			CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pOldObj1);
			if (!pLayer)
			{
				Abort();
				return;
			}
			color = pLayer->GetColor();
		}
		m_pObj->SetColor(color);
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
	}
	
	if( m_nStep>=2 )
	{
		if( m_nWaitSelect==1 )
			m_ptSel1 = pt;
		else if( m_nWaitSelect==2 )
		{
			if( m_nStep==2 )
			{
				PromptString(StrFromResID(IDS_CMDTIP_SELECTTOSAVE2));
				m_ptSel1 = pt;
				m_nStep = 3;
				return;
			}
			else m_ptSel2 = pt;
		}
		
		CGeometry *pObj = GetReplaceObj_needSelect();
		if( !pObj )
		{
			PromptString(StrFromResID(IDS_CMDTIP_SELECTFAIL));
			m_nStep = 2;
			return;
		}	

		m_pEditor->UpdateDrag(ud_ClearDrag);

		CFeature *pFtr = HandleToFtr(m_objOldID1)->Clone();
		if (!pFtr) return;
		
		CUndoFtrs undo(m_pEditor,Name());

		int layid = m_pEditor->GetFtrLayerIDOfFtr(m_objOldID1);
		pFtr->SetID(OUID());
		pFtr->SetGeometry(pObj);
		if(m_pEditor->AddObject(pFtr,layid))
		{
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objOldID1),pFtr);
			undo.AddNewFeature(FtrToHandle(pFtr));
		}
		if(m_pEditor->DeleteObject(m_objOldID1))
		{
			undo.AddOldFeature(m_objOldID1);
		}
		if(m_pEditor->DeleteObject(m_objOldID2))
		{
			undo.AddOldFeature(m_objOldID2);
		}	
		undo.Commit();
		
		Finish();
	}
	
	if( m_pDrawProc )
	{
		m_pDrawProc->PtClick(pt,flag);
	}
}

void CReplaceLinesCommand::PtMove(PT_3D &pt)
{
	if( m_pDrawProc )
	{
		m_pDrawProc->PtMove(pt);
	}
	if (m_nWaitSelect==1||m_nWaitSelect==2)
	{
		return;
	}
	if( IsProcFinished(this) )return;
	
	{
		//判断目标地物是否在可以连接的范围内
		BOOL bCanLink = TRUE;
		double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();		
		
		//m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		//查找
		
		CFeature *pOldObj = NULL;
		PT_3D p1;
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
		Envelope e;
		e.CreateFromPtAndRadius(p1,r);
		pOldObj = m_pEditor->GetDataQuery()->FindNearestObject(p1,r,m_pEditor->GetCoordWnd().m_pSearchCS);	
		if( pOldObj )
		{		
			if(  (pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
			{
				//层码必须一致
				CFtrLayer *pLayer1 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pOldObj1);
				CFtrLayer *pLayer2 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(pOldObj);
				
				if( pLayer1 && pLayer2&&pLayer1 != pLayer2&&
					strcmp(pLayer1->GetName(),pLayer2->GetName())!=0 )
				{
					pOldObj = NULL;
				}
				//如果是面，就不支持同时修测两个地物
				else if( (m_pOldObj1&&m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))||
					(pOldObj&&pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
				{
					if( FtrToHandle(pOldObj)!=m_objOldID1 )pOldObj = NULL;
				}
			}
			else pOldObj = NULL;
		}
		else pOldObj = NULL;
		
		/*if( !pOldObj )pFtr = NULL;*/
		
		//不是处于选点状态
		if( m_nWaitSelect==0 && m_bCanTwoObjs )
		{
			//高程必须对应
			if( m_bMatchHeight && pOldObj )
			{
				if( m_pOldObj1 && CModifyZCommand::CheckObjForContour(m_pOldObj1->GetGeometry()) )
				{
					PT_3DEX expt1, expt2;
					expt1 = m_pOldObj1->GetGeometry()->GetDataPoint(0);
					expt2 = pOldObj->GetGeometry()->GetDataPoint(0);
					if( !CModifyZCommand::CheckObjForContour(pOldObj->GetGeometry()) || 
						fabs(expt1.z-expt2.z)>=1e-4 )
					{					
						pOldObj = NULL;
					}
				}
			}

			if (!pOldObj)
			{
				int aaaa = 10;
			}
			m_objOldID2 = FtrToHandle(pOldObj);
			m_pOldObj2 = pOldObj;
		}
		//必须是已经被接受处理的两个地物
		else
		{
			if( (m_objOldID1&&FtrToHandle(pOldObj)!=m_objOldID1) && (m_objOldID2&&FtrToHandle(pOldObj)!=m_objOldID2) )
				pOldObj = NULL;
		}
		
		PT_3D pt0;
		if( !pOldObj ||!pOldObj->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt0,NULL) )
			bCanLink = FALSE;
		
		if( !m_bCanTwoObjs && m_objOldID1 && FtrToHandle(pOldObj)!=m_objOldID1 )
			bCanLink = FALSE;
		
		GrBuffer vbuf;
		if( bCanLink && m_bShowMark )
		{
			//更新显示
			GrBuffer buf;
			buf.BeginLineString(0,0);
			DrawPointTip(pOldObj->GetGeometry(),pt0,&buf);
			buf.End();
			vbuf.CopyFrom(&buf);
		}
		if (m_pDrawProc&&m_pDrawProc->m_arrPts.GetSize()<=0)
		{
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		}
		
		m_pEditor->UpdateDrag(ud_AddVariantDrag,&vbuf);
	}

}


void CReplaceLinesCommand::GetParams(CValueTable &tab)
{	
	if (m_pDrawProc)
	{
		if( !m_bVWsnaplinestyle )
			m_nVWPen = m_pDrawProc->m_nCurPenCode;
		if( !m_b3Dsnaplinestyle )
			m_n3DPen = m_pDrawProc->m_nCurPenCode;
	}	

	CDrawCommand::GetParams(tab);
	_variant_t var;
 	var = (_variant_t)(long)(m_nMode);
	tab.AddValue(PF_REPLACELINEWAY,&CVariantEx(var));
	var = (_variant_t)(long)(m_nVWPen);
	tab.AddValue(PF_VMPEN,&CVariantEx(var));
	var = (_variant_t)(long)(m_n3DPen);
	tab.AddValue(PF_3DPEN,&CVariantEx(var));
	var = (bool)(m_bCanTwoObjs);
	tab.AddValue(PF_REPLACELINETWOOBJ,&CVariantEx(var));
	var = (bool)(m_bMatchHeight);
	tab.AddValue(PF_REPLACELINEHEIMATCH,&CVariantEx(var));
	var = (bool)(m_bShowMark);
	tab.AddValue(PF_REPLACELINESHOWMARK,&CVariantEx(var));
	var = (_variant_t)(long)(m_nSnapRadius);
	tab.AddValue(PF_REPLACELINESNAPRADIUS,&CVariantEx(var));
	var = (bool)(m_bVWsnaplinestyle);
	tab.AddValue(PF_REPLACELINEVECTOR,&CVariantEx(var));
	var = (bool)(m_b3Dsnaplinestyle);
	tab.AddValue(PF_REPLACELINESTEREO,&CVariantEx(var));

	if(m_pDrawProc)
	{
		var = (double)(m_pDrawProc->m_compress.GetLimit());
		tab.AddValue(PF_TOLER,&CVariantEx(var));

		var = (_variant_t)(long)(m_pDrawProc->m_nCurPenCode);
		tab.AddValue(PF_PENCODE,&CVariantEx(var));
	}
}


void CReplaceLinesCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("ReplaceLines",StrFromResID(IDS_CMDNAME_REPLACE));
	param->BeginOptionParam(PF_REPLACELINEWAY,StrFromResID(IDS_CMDPLANE_WAY));
    param->AddOption(StrFromResID(IDS_CMDPLANE_TWOPT),modeTwoPoint,' ',m_nMode==modeTwoPoint);
	param->AddOption(StrFromResID(IDS_CMDPLANE_THREEPT),modeManyPoint,' ',m_nMode==modeManyPoint);
	param->EndOptionParam();
	param->AddParam(PF_REPLACELINETWOOBJ,(bool)(m_bCanTwoObjs),StrFromResID(IDS_CMDPLANE_CANTWOOBJ));
	if(m_bCanTwoObjs||bForLoad)param->AddParam(PF_REPLACELINEHEIMATCH,(bool)m_bMatchHeight,StrFromResID(IDS_CMDPLANE_MATCHHEIGHT));
	param->AddParam(PF_REPLACELINESHOWMARK,(bool)m_bShowMark,StrFromResID(IDS_CMDPLANE_SHOWMARK));
	param->AddParam(PF_REPLACELINESNAPRADIUS,(int)m_nSnapRadius,StrFromResID(IDS_CMDPLANE_SNAPRADIUS));
	param->AddParam(PF_REPLACELINEVECTOR,(bool)m_bVWsnaplinestyle,StrFromResID(IDS_CMDPLANE_VWSNAPLINESTYLE));
	param->AddParam(PF_REPLACELINESTEREO,(bool)m_b3Dsnaplinestyle,StrFromResID(IDS_CMDPLANE_TDSNAPLINESTYLE));
	if(m_pDrawProc)
		param->AddLineTypeParam(PF_PENCODE,m_pDrawProc->m_nCurPenCode,StrFromLocalResID(IDS_CMDPLANE_LINETYPE));	
// 	param->AddParam(PF_COPYROTATEKPOLD,bool(m_bKeepOld),StrFromResID(IDS_CMDPLANE_KEEPOLD));

	BOOL b3D = m_pEditor->GetCoordWnd().m_bIsStereo;

	if (((!b3D&&m_bVWsnaplinestyle) || (b3D&&m_b3Dsnaplinestyle))/*m_pDrawProc->m_nCurPenCode == penStream*/ || bForLoad)
	{
		if(m_pDrawProc)
			param->AddParam(PF_TOLER,m_pDrawProc->m_compress.GetLimit(),StrFromResID(IDS_CMDPLANE_TOLERANCE));
	}
	
	param->AddParam(PF_REPLACELINE_WAY,char('c'),StrFromResID(IDS_CMDPLANE_WAY));
	param->AddParam(PF_TRACKLINEACCKEY,'w',StrFromResID(IDS_CMDPLANE_TRACKLINE));
}

int  CReplaceLinesCommand::GetCurPenCode()
{
	if(m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}

DrawingInfo CReplaceLinesCommand::GetCurDrawingInfo()
{
	if (m_pDrawProc)
	{
		CFeature *pFtr = new CFeature;
		pFtr->SetGeometry(m_pDrawProc->m_pGeoCurve);
		return DrawingInfo(pFtr,m_pDrawProc->m_arrPts);
	}
	else
		return DrawingInfo();
			
}

void CReplaceLinesCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_REPLACELINEWAY,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_VMPEN,var) )
	{
		m_nVWPen = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_3DPEN,var) )
	{
		m_n3DPen = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINETWOOBJ,var) )
	{
		m_bCanTwoObjs = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINEHEIMATCH,var) )
	{
		m_bMatchHeight = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINESHOWMARK,var) )
	{
		m_bShowMark = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINESNAPRADIUS,var) )
	{
		m_nSnapRadius = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINEVECTOR,var) )
	{
		m_bVWsnaplinestyle = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINESTEREO,var) )
	{
		m_b3Dsnaplinestyle = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if(m_pDrawProc&&tab.GetValue(0,PF_PENCODE,var) )
	{
		m_pDrawProc->m_nCurPenCode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if(m_pDrawProc && tab.GetValue(0,PF_TOLER,var) )
	{
		m_pDrawProc->m_compress.SetLimit((double)(_variant_t)*var);
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_REPLACELINE_WAY,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key == 'c'|| key == 'C')
		{
			if (m_nMode == modeTwoPoint)
			{
				m_nMode = modeManyPoint;
			}
			else
			{
				m_nMode = modeTwoPoint;
			}
			SetSettingsModifyFlag();
		}
	}

	if(m_pDrawProc && tab.GetValue(0,PF_TRACKLINEACCKEY,var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'w'||ch == 'W')
		{
			m_pDrawProc->TrackLine();
		}
	}

	CDrawCommand::SetParams(tab,bInit);
}


void CReplaceLinesCommand::DrawPointTip(CGeometry *pObj, PT_3D pt, GrBuffer *buf)
{
	PT_4D pts[4];
	pts[0] = PT_4D(pt);
	m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(pts,1);
	
	pts[0].x -= 10; pts[0].y -= 10; pts[0].z -= 10; pts[0].yr -= 10;
	
	pts[1].x = pts[0].x+20; pts[1].y = pts[0].y+20; 
	pts[1].z = pts[0].z+20; pts[1].yr = pts[0].yr+20; 
	
	pts[2].x = pts[0].x; pts[2].y = pts[0].y+20; 
	pts[2].z = pts[0].z; pts[2].yr = pts[0].yr+20; 
	
	pts[3].x = pts[0].x+20; pts[3].y = pts[0].y; 
	pts[3].z = pts[0].z+20; pts[3].yr = pts[0].yr; 
	
	m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pts,4);
	
	PT_3D pt3ds[4];
	for( int i=0; i<4; i++)pt3ds[i] = pts[i].To3D();
	
	buf->MoveTo(pt3ds);
	buf->LineTo(pt3ds+1);
	buf->MoveTo(pt3ds+2);
	buf->LineTo(pt3ds+3);
}


CGeometry* CReplaceLinesCommand::GetReplaceObj()
{
	//多点方式总是需要选择点的
// 	if( m_nMode==modeManyPoint )
// 	{
// 		if( m_pOldObj2 && m_pOldObj1!=m_pOldObj2 )
// 			m_nWaitSelect = 2;
// 		else m_nWaitSelect = 1;
// 		return NULL;
// 	}
	
	if( !m_pOldObj1||!m_pOldObj2 || m_pOldObj1==m_pOldObj2 )
		return GetReplaceObj_same();
	
	//获得各个点在基线上的最近点
	PT_3D ptStart, ptEnd;
	
	double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
	int start1=-1, end1=-1, start2=-1, end2=-1, i;
	PT_3D p1;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&p1);
	Envelope e ;
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptStart,NULL) )
	{
		start1 = -2;
	}
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&p1);
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj2->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptEnd,NULL) )
	{
		start2 = -2;
	}
	
	//两点都没有附着到地物上
	if( start1==-2 || start2==-2 )return NULL;
	
	CGeometry *pCurve1 = (CGeometry*)m_pOldObj1->GetGeometry(), *pCurve2 = (CGeometry*)m_pOldObj2->GetGeometry();
	
	 
	const CShapeLine  *pSL1 = pCurve1->GetShape();
	const CShapeLine  *pSL2 = pCurve2->GetShape();
	//得到关键点在基线中的对应序号
	CArray<int,int> KeyPos1 , KeyPos2 ;
	if(!pSL1->GetKeyPosOfBaseLines(KeyPos1)) return NULL;
	if(!pSL2->GetKeyPosOfBaseLines(KeyPos2)) return NULL;	
	
	int keynum1 = KeyPos1.GetSize();
	int keynum2 = KeyPos2.GetSize();
		
	CArray<PT_3DEX,PT_3DEX> pts1,pts2;
	if(!pSL1->GetPts(pts1))return NULL;
	int num1 = pts1.GetSize();
	if(!pSL2->GetPts(pts2))return NULL;
	int num2 = pts2.GetSize();
	if( num1<=1 || num2<=1 )
	{
		
		return NULL;
	}
	
	//获得上述各个最近点在基线中的位置
	PT_3D rpt2;
	double x,y,z;
	double dis,min1=-1,min2=-1, min3=-1;
	int i1=-1, i2=-1, i3=-1;
	
	//计算第一个地物
	
	for( i=0; i<num1-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts1[i].x,pts1[i].y,pts1[i].z,
			pts1[i+1].x,pts1[i+1].y,pts1[i+1].z,m_ptStart.x,m_ptStart.y,m_ptStart.z,&x,&y,&z,false);
		
		if( min2<0 || dis<min2 )
		{ 
			min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
			if( _FABS(x-pts1[i+1].x)+_FABS(y-pts1[i+1].y)>1e-10 )
				i2=i;
			//就是第二个点时，序号增加1
			else i2=i+1;
		}
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	
		double d=0;
		if( i2<num1 && i2>=0 )d = _FABS(pts1[i2].x-rpt2.x)+_FABS(pts1[i2].y-rpt2.y)+_FABS(pts1[i2].z-rpt2.z);
		
		if( d>1e-10 && i2<num1 && i2>=0 )
		{
			pts1.InsertAt(i2+1,PT_3DEX(rpt2,20));
			
			for( i=0; i<keynum1; i++)if( KeyPos1[i]>i2 )KeyPos1[i] = KeyPos1[i]+1;
			i2++; num1++;
		}
	}
	
	//判断哪一头比较长，比较长的保留
	{
		double d1 = pSL1->GetLength(&ptStart);
		double d2 = pSL1->GetLength()-d1;
		if( d1>=d2 )
		{
			start1 = -1;
			end1 = i2;
		}
		else
		{
			start1 = num1;
			end1 = i2;
		}
	}
	
	//计算第二个地物
	
	for( i=0; i<num2-1; i++)
	{
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts2[i].x,pts2[i].y,pts2[i].z,
			pts2[i+1].x,pts2[i+1].y,pts2[i+1].z,m_ptEnd.x,m_ptEnd.y,m_ptEnd.z,&x,&y,&z,false);
		
		if( min3<0 || dis<min3 )
		{ 
			min3 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
			if( _FABS(x-pts2[i+1].x)+_FABS(y-pts2[i+1].y)>1e-10 )
				i3=i;
			//就是第二个点时，序号增加1
			else i3=i+1;
		}
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	
		double d=0;
		if( i3<num2 && i3>=0 )d = _FABS(pts2[i3].x-rpt2.x)+_FABS(pts2[i3].y-rpt2.y)+_FABS(pts2[i3].z-rpt2.z);
		
		if( d>1e-10 && i3<num2 && i3>=0 )
		{
			pts2.InsertAt(i3+1,PT_3DEX(rpt2,20));
			
			for( i=0; i<keynum2; i++)if( KeyPos2[i]>i3 )KeyPos2[i] = KeyPos2[i]+1;
			i3++; num2++;
		}
	}
	
	//判断哪一头比较长，比较长的保留
	{
		double d1 = pSL2->GetLength(&ptEnd);
		double d2 = pSL2->GetLength()-d1;
		if( d1>=d2 )
		{
			start2 = -1;
			end2 = i3;
		}
		else
		{
			start2 = num2;
			end2 = i3;
		}
	}
	
	//计算各个位置最邻近的关键点
	int startKey1, endKey1,startKey2, endKey2;
	for( i=0; i<keynum1; i++)
	{
		if( start1<KeyPos1[i] )break;
	}
	
	startKey1 = i;
	
	for( i=0; i<keynum1; i++)
	{
		if( end1<KeyPos1[i] )break;
	}
	
	endKey1 = i;
	
	for( i=0; i<keynum2; i++)
	{
		if( start2<KeyPos2[i] )break;
	}
	
	startKey2 = i;
	
	for( i=0; i<keynum2; i++)
	{
		if( end2<KeyPos2[i] )break;
	}
	
	endKey2 = i;
	
	//准备新地物
	CGeometry *pNewObj = (CGeometry*)m_pOldObj1->GetGeometry()->Clone();
	if( !pNewObj )
	{		
		return NULL;
	}
	
	PT_3DEX expt;
	CArray<PT_3DEX,PT_3DEX> arr;
	pts1.RemoveAll();
	pNewObj->GetShape(pts1);
	//首段
	if( start1>end1 )
	{
		for( i=keynum1-1; i>=endKey1; i--)
		{
			arr.Add(pts1.GetAt(i));		
		}
	}
	else
	{
		for( i=0; i<endKey1; i++)
		{
			arr.Add(pts1.GetAt(i));
		
		}
	}
	
	//替换段
	pts1.RemoveAll();
	m_pObj->GetShape(pts1);
	arr.Append(pts1);
//	int newnum = m_pObj->GetDataPointSum();
// 	for( i=0; i<newnum; i++)
// 	{	
// 		arr.Add(m_pObj->GetDataPoint(i));
// 	}
	
	//尾段
	pts1.RemoveAll();
	m_pOldObj2->GetGeometry()->GetShape(pts1);
	if( start2>end2 )
	{
		for( i=endKey2; i<keynum2; i++)
		{
			arr.Add(pts1.GetAt(i));
		}
	}
	else
	{
		for( i=endKey2-1; i>=0; i--)
		{
			arr.Add(pts1.GetAt(i));			
		}
	}
	
	//去除重复的点(主要因为起点、终点落在原对象节点上)
	PT_3DEX tt;
	keynum1 = arr.GetSize();
	int pos = 0;
	tt = arr.GetAt(pos);
	for( i=1; i<keynum1 && keynum1>2; )
	{
		expt = arr.GetAt(i);
		if( _FABS(expt.x-tt.x)<1e-4 && _FABS(expt.y-tt.y)<1e-4 && _FABS(expt.z-tt.z)<1e-4 )
		{
			arr.RemoveAt(i);
			keynum1--;
			continue;
		}
		else
		{
			pos++;
			tt = arr.GetAt(pos);
		}
		i++;
	}
	if(!pNewObj->CreateShape(arr.GetData(),arr.GetSize())) return NULL;
	return pNewObj;
}

CGeometry* CReplaceLinesCommand::GetReplaceObj_same()
{
	if (!m_pOldObj1)
	{
		return NULL;
	}
	//获得各个点在基线上的最近点
	PT_3D ptStart, ptEnd;
	
	double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
	int select=-1, start=-1, end=-1, i;
	
	PT_3D p1;
	Envelope e;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&p1);	
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptStart,NULL) )
	{
		start = -2;
	}
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&p1);
	e .CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptEnd,NULL) )
	{
		end = -2;
	}
	
	//两点都没有附着到地物上
	if( start==-2 && end==-2 )return NULL;
	
	//如果是面，头尾点必须都附着在地物上
	if( m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		if( start==-2 || end==-2 )return NULL;
	}
	
	CGeometry *pCurve = (CGeometry*)m_pOldObj1->GetGeometry();
	
	int keynum = pCurve->GetDataPointSum();
	if( keynum<=1 )return NULL;
	
	const CShapeLine  *pSL = pCurve->GetShape();
	//得到关键点在基线中的对应序号
	CArray<int,int> KeyPos;
	if(!pSL->GetKeyPosOfBaseLines(KeyPos)) return NULL;
	
	CArray<PT_3DEX,PT_3DEX> pts;
	if(!pSL->GetPts(pts)) return NULL;

	int num = pts.GetSize();
	if( num<=1 )
	{
		return NULL;
	}
		
	//获得上述各个最近点在基线中的位置
	PT_3D rpt2,rpt3;
	double x,y,z;
	double dis,min2=-1, min3=-1;
	int i1=-1, i2=-1, i3=-1;
	
	for( i=0; i<num-1; i++)
	{
		//求出当前线段的最近距离
		if( start!=-2 )
		{
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i+1].x,pts[i+1].y,pts[i+1].z,m_ptStart.x,m_ptStart.y,m_ptStart.z,&x,&y,&z,false);
			
			if( min2<0 || dis<min2 )
			{ 
				min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
				if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
					i2=i;
				//就是第二个点时，序号增加1
				else i2=i+1;
			}
		}
		
		if( end!=-2 )
		{
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i+1].x,pts[i+1].y,pts[i+1].z,m_ptEnd.x,m_ptEnd.y,m_ptEnd.z,&x,&y,&z,false);
			
			if( min3<0 || dis<min3 )
			{ 
				min3 = dis; rpt3.x=x; rpt3.y=y; rpt3.z=z; 
				if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
					i3=i;
				//就是第二个点时，序号增加1
				else i3=i+1;
			}
		}
	}
	
	if( min2<0 && min3<0 )
	{	
		return NULL;
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	
		double d2=0, d3=0;
		if( i2<num && i2>=0 )d2 = _FABS(pts[i2].x-rpt2.x)+_FABS(pts[i2].y-rpt2.y)+_FABS(pts[i2].z-rpt2.z);
		if( i3<num && i3>=0 )d3 = _FABS(pts[i3].x-rpt3.x)+_FABS(pts[i3].y-rpt3.y)+_FABS(pts[i3].z-rpt3.z);
		
		if( d2>1e-10 && i2<num && i2>=0 )
		{
			pts.InsertAt(i2+1,PT_3DEX(rpt2,0));
		
			for( i=0; i<keynum; i++)if( KeyPos[i]>i2 )KeyPos[i] = KeyPos[i]+1;
			if( i2<i3 ||(i2==i3&&d2<=d3) )i3++;
			i2++; num++;
		}
		if( d3>1e-10 && i3<num && i3>=0 )
		{
			pts.InsertAt(i3+1,PT_3DEX(rpt3,0));
			for( i=0; i<keynum; i++)if( KeyPos[i]>i3 )KeyPos[i] = KeyPos[i]+1;
			if( i3<i2 ||(i3==i2&&d3<d2) )i2++;
			i3++; num++;
		}
	}
	
	//有个点在咬合范围外，就判断哪一头比较长，比较长的保留
	if( start==-2 || end==-2 )
	{
		double d1 = pSL->GetLength(&ptStart);
		double d2 = pSL->GetLength()-d1;
		if( d1>=d2 )
		{
			select = -1;
			start = (start==-2?num:i2);
			end = (end==-2?num:i3);
		}
		else
		{
			select = num;
			start = (start==-2?-1:i2);
			end = (end==-2?-1:i3);
		}
	}
	//两个点都在咬合范围内，
	else
	{
		//如果是闭合线或者是面，就判断哪一段比较长，比较长的保留
		if( m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) ||
			(m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)(m_pOldObj1->GetGeometry()))->IsClosed()) )
		{
			double d1 = fabs(pSL->GetLength(&ptEnd)-pSL->GetLength(&ptStart));
			double d2 = pSL->GetLength()-d1;
			//保留中间
			if( d1>d2 )
			{
				select = (i2+i3)/2;
				start  = i2;
				end    = i3;
			}
			//保留头尾
			else
			{
				select = -1;
				start  = i2;
				end    = i3;
			}
		}
		//否则替换中间的
		else
		{
			select = -1;
			start  = i2;
			end    = i3;
		}
	}
	
	//计算各个位置最邻近的关键点
	int startKey, endKey;
	for( i=0; i<keynum; i++)
	{
		if( start<KeyPos[i] )break;
	}
	
	startKey = i;
	
	for( i=0; i<keynum; i++)
	{
		if( end<KeyPos[i] )break;
	}
	
	endKey = i;
	
	//准备新地物
	CGeometry *pNewObj = (CGeometry*)m_pOldObj1->GetGeometry()->Clone();
	if( !pNewObj )
	{		
		return NULL;
	}	
	
	CArray<PT_3DEX,PT_3DEX> arr,pts1;
	PT_3DEX expt;
	//---------分如下情况生成新的地物
	//1. 选择点在起始点外
	if( select<=(start<end?start:end) || select>=(start>=end?start:end) )
	{
		if( startKey>endKey ){ int t=startKey; startKey=endKey; endKey=t; }
		
		//首段
		pNewObj->GetShape(pts1);
		for( i=0; i<startKey; i++)
		{		
			arr.Add(pts1.GetAt(i));
		}
		
		//防止expt具有无效的cd码
		if( startKey<=0 )expt = m_pObj->GetDataPoint(0);
		
		//替换段
		pts.RemoveAll();
		m_pObj->GetShape(pts);
		int newnum = pts.GetSize();
		if( start<=end )
		{
			arr.Append(pts);
// 			for( i=0; i<newnum; i++)
// 			{				
// 				arr.Add(m_pObj->GetDataPoint(i));
// 			}
		}
		else
		{
			for( i=newnum-1; i>=0; i--)
			{				
				arr.Add(pts.GetAt(i));
			}
		}
		
		//尾段
		for( i=endKey; i<keynum; i++)
		{		
			arr.Add(pts1.GetAt(i));
		}

		if (m_bExtendCollect && ( (i2==0 && i3==num-1) || (i3==0 && i2==num-1) ) )
		{
			if (startKey < endKey)
			{
				for (i=endKey-1; i>=startKey; i--)
				{
					arr.Add(pts1.GetAt(i));
				}
			}
			else
			{
				for (i=startKey; i<endKey; i++)
				{
					arr.Add(pts1.GetAt(i));
				}
			}
		}
		
		
	}
	//2. 选择点在起始点内
	else
	{
		if( startKey>endKey ){ int t=startKey; startKey=endKey; endKey=t; }
		
		//中间段
		expt = pNewObj->GetDataPoint(startKey);
		if( start<=end )COPY_3DPT(expt,m_ptStart);
			else COPY_3DPT(expt,m_ptEnd);
			arr.Add(expt);
		pNewObj->GetShape(pts1);
		for( i=startKey; i<endKey; i++)
		{			
			arr.Add(pts1.GetAt(i));
		}
		
		if( start<=end )COPY_3DPT(expt,m_ptEnd);
			else COPY_3DPT(expt,m_ptStart);
			arr.Add(expt);
		
		//替换段
		pts.RemoveAll();
		m_pObj->GetShape(pts);
		int newnum = pts.GetSize();
		if( start>end )
		{
			arr.Append(pts);
// 			for( i=0; i<newnum; i++)
// 			{			
// 				arr.Add(m_pObj->GetDataPoint(i));
// 			}
		}
		else
		{
			for( i=newnum-1; i>=0; i--)
			{				
				arr.Add(pts.GetAt(i));
			}
		}
	}
		
	//去除重复的点(主要因为起点、终点落在原对象节点上)
	PT_3DEX tt;
	keynum = arr.GetSize();
	int pos = 0;
	tt = arr.GetAt(pos);
	for( i=1; i<keynum && keynum>2; )
	{
		expt = arr.GetAt(i);
		if( _FABS(expt.x-tt.x)<1e-4 && _FABS(expt.y-tt.y)<1e-4 && _FABS(expt.z-tt.z)<1e-4 )
		{
			arr.RemoveAt(i);
			keynum--;
			continue;
		}
		else
		{
			pos++;
			tt = arr.GetAt(pos);
		}
		i++;
	}
	
	if (m_bExtendCollect && ( (i2==0 && i3==num-1) || (i3==0 && i2==num-1) ) )
	{
		arr.Add(arr[0]);
	}
	
	if (!pNewObj->CreateShape(arr.GetData(),arr.GetSize()))
	{
		return NULL;
	}
	
	return pNewObj;
}


CGeometry* CReplaceLinesCommand::GetReplaceObj_needSelect_same()
{
	//获得各个点在基线上的最近点
	PT_3D ptStart, ptSel, ptEnd;
	
	double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
	Envelope e ;
	
	int select=-1, start=-1, end=-1, i;
	PT_3D p1;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptSel1,&p1);
	e.CreateFromPtAndRadius(p1,r);
	
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptSel,NULL) )
		return NULL;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&p1);
	e .CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptStart,NULL) )
	{
		start = -2;
	}
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&p1);
	e .CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptEnd,NULL) )
	{
		end = -2;
	}
	
	if( start==-2 && end==-2 )return NULL;
	
	CGeometry *pCurve = (CGeometry*)m_pOldObj1->GetGeometry();
	
	int keynum = pCurve->GetDataPointSum();
	if( keynum<=1 )return NULL;
	
	//得到关键点在基线中的对应序号
	CArray<int,int> pKeyPos;
	const CShapeLine  *pSL = pCurve->GetShape();
	pSL->GetKeyPosOfBaseLines(pKeyPos);
		
	CArray<PT_3DEX,PT_3DEX> pts;
	
	pSL->GetPts(pts);
	int num = pts.GetSize();
	if( num<=1 )
	{		
		return NULL;
	}
	
	//获得上述各个最近点在基线中的位置
	PT_3D rpt1,rpt2,rpt3;
	double x,y,z;
	double dis,min1=-1,min2=-1, min3=-1;
	int i1=-1, i2=-1, i3=-1;
	
	for( i=0; i<num-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
			pts[i+1].x,pts[i+1].y,pts[i+1].z,m_ptSel1.x,m_ptSel1.y,m_ptSel1.z,&x,&y,&z,false);
		
		if( min1<0 || dis<min1 )
		{ 
			min1 = dis; rpt1.x=x; rpt1.y=y; rpt1.z=z; 
			
			if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
				i1=i;
			//就是第二个点时，序号增加1
			else i1=i+1;
		}
		
		if( start!=-2 )
		{
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i+1].x,pts[i+1].y,pts[i+1].z,m_ptStart.x,m_ptStart.y,m_ptStart.z,&x,&y,&z,false);
			
			if( min2<0 || dis<min2 )
			{ 
				min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
				if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
					i2=i;
				//就是第二个点时，序号增加1
				else i2=i+1;
			}
		}
		
		if( end!=-2 )
		{
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i+1].x,pts[i+1].y,pts[i+1].z,m_ptEnd.x,m_ptEnd.y,m_ptEnd.z,&x,&y,&z,false);
			
			if( min3<0 || dis<min3 )
			{ 
				min3 = dis; rpt3.x=x; rpt3.y=y; rpt3.z=z; 
				if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
					i3=i;
				//就是第二个点时，序号增加1
				else i3=i+1;
			}
		}
	}
	
	if( min1<0 || (min2<0&&min3<0) )
	{	
		return NULL;
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	// 	GrNode tnode;
		double d1=0, d2=0, d3=0;
		d1 = _FABS(pts[i1].x-rpt1.x)+_FABS(pts[i1].y-rpt1.y)+_FABS(pts[i1].z-rpt1.z);
		if( i2<num && i2>=0 )d2 = _FABS(pts[i2].x-rpt2.x)+_FABS(pts[i2].y-rpt2.y)+_FABS(pts[i2].z-rpt2.z);
		if( i3<num && i3>=0 )d3 = _FABS(pts[i3].x-rpt3.x)+_FABS(pts[i3].y-rpt3.y)+_FABS(pts[i3].z-rpt3.z);
		
		if( d1>1e-10 )
		{
			PT_3DEX ptt;
			COPY_3DPT(ptt,rpt1);
			pts.InsertAt(i1+1,ptt);
// 			memmove(pts+i1+2, pts+i1+1, (num-i1-1)*sizeof(GrNode));
// 			COPY_3DPT(tnode.pt,rpt1);
// 			memcpy(pts+i1+1,&tnode,sizeof(GrNode));
			for( i=0; i<keynum; i++)if( pKeyPos[i]>i1 )pKeyPos[i] = pKeyPos[i]+1;
			if( i1<i2 ||(i1==i2&&d1<=d2) )i2++;
			if( i1<i3 ||(i1==i3&&d1<=d3) )i3++;
			i1++; num++;
		}
		if( d2>1e-10 && i2<num && i2>=0 )
		{
			PT_3DEX ptt;
			COPY_3DPT(ptt,rpt2);
			pts.InsertAt(i2+1,ptt);
// 			memmove(pts+i2+2, pts+i2+1, (num-i2-1)*sizeof(GrNode));
// 			COPY_3DPT(tnode.pt,rpt2);
// 			memcpy(pts+i2+1,&tnode,sizeof(GrNode));
			for( i=0; i<keynum; i++)if( pKeyPos[i]>i2 )pKeyPos[i] = pKeyPos[i]+1;
			if( i2<i1 ||(i2==i1&&d2<d1) )i1++;
			if( i2<i3 ||(i2==i3&&d2<=d3) )i3++;
			i2++; num++;
		}
		if( d3>1e-10 && i3<num && i3>=0 )
		{
			PT_3DEX ptt;
			COPY_3DPT(ptt,rpt3);
			pts.InsertAt(i3+1,ptt);
			for( i=0; i<keynum; i++)if( pKeyPos[i]>i3 )pKeyPos[i] = pKeyPos[i]+1;
			if( i3<i1 ||(i3==i1&&d3<d1) )i1++;
			if( i3<i2 ||(i3==i2&&d3<d2) )i2++;
			i3++; num++;
		}
		
		//如果点在咬合范围外面，那么他们的位置可以理解为第一点之前或者最后点之后
		if( i2<0 )i2 = (i1<=i3?num:-1);
		if( i3<0 )i3 = (i1<=i2?num:-1);
	}
	
	select = i1;
	start  = i2;
	end    = i3;
	
	//计算各个位置最邻近的关键点
	int startKey, endKey;
	for( i=0; i<keynum; i++)
	{
		if( start<pKeyPos[i] )break;
	}
	
	startKey = i;
	
	for( i=0; i<keynum; i++)
	{
		if( end<pKeyPos[i] )break;
	}
	
	endKey = i;
	
	//准备新地物
	CGeometry *pNewObj = (CGeometry*)m_pOldObj1->GetGeometry()->Clone();
	if( !pNewObj )
	{	
		return NULL;
	}
	
	CArray<PT_3DEX,PT_3DEX> arr,pts1;
	PT_3DEX expt;
	//---------分如下情况生成新的地物
	//1. 选择点在起始点外
	if( select<=(start<end?start:end) || select>=(start>=end?start:end) )
	{
		if( startKey>endKey ){ int t=startKey; startKey=endKey; endKey=t; }
		
		//首段
		m_pOldObj1->GetGeometry()->GetShape(pts1);
		for( i=0; i<startKey; i++)
		{		
			arr.Add(pts1.GetAt(i));
		}
		
		//防止expt具有无效的cd码
		if( startKey<=0 )expt = m_pObj->GetDataPoint(0);
		
		//替换段
		pts.RemoveAll();
		m_pObj->GetShape(pts);
		int newnum = pts.GetSize();
		if( start<=end )
		{
			arr.Append(pts);
		}
		else
		{
			for( i=newnum-1; i>=0; i--)
			{
				expt = pts.GetAt(i);
				arr.Add(expt);
			}
		}
		
		//尾段
		for( i=endKey; i<keynum; i++)
		{		
			arr.Add(pts1.GetAt(i));
		}

	}
	//2. 选择点在起始点内
	else
	{
		if( startKey>endKey ){ int t=startKey; startKey=endKey; endKey=t; }
		
		//中间段
		expt = m_pOldObj1->GetGeometry()->GetDataPoint(startKey);
		if( start<=end )COPY_3DPT(expt,m_ptStart);
		else COPY_3DPT(expt,m_ptEnd);
		arr.Add(expt);
		
		m_pOldObj1->GetGeometry()->GetShape(pts1);
		for( i=startKey; i<endKey; i++)
		{		
			arr.Add(pts1.GetAt(i));
		}
		
		if( start<=end )COPY_3DPT(expt,m_ptEnd);
			else COPY_3DPT(expt,m_ptStart);
			arr.Add(expt);
		
		//替换段
		pts.RemoveAll();
		m_pObj->GetShape(pts);
		int newnum = pts.GetSize();
		if( start>end )
		{
			arr.Append(pts);
// 			for( i=0; i<newnum; i++)
// 			{		
// 				arr.Add(m_pObj->GetDataPoint(i));
// 			}
		}
		else
		{
			for( i=newnum-1; i>=0; i--)
			{			
				arr.Add(pts.GetAt(i));
			}
		}
	}
	//pNewObj->CreateShape(pts.GetData(),pts.GetSize());
	
	//去除重复的点(主要因为起点、终点落在原对象节点上)
	PT_3DEX tt;
	keynum = arr.GetSize();
	int pos = 0;
	tt = pNewObj->GetDataPoint(pos);
	for( i=1; i<keynum && keynum>2; )
	{
		expt = arr.GetAt(i);
		if( _FABS(expt.x-tt.x)<1e-4 && _FABS(expt.y-tt.y)<1e-4 && _FABS(expt.z-tt.z)<1e-4 )
		{
			arr.RemoveAt(i);
			keynum--;
			continue;
		}
		else
		{
			pos++;
			tt = arr.GetAt(pos);
		}
		i++;
	}
	pNewObj->CreateShape(arr.GetData(),arr.GetSize());
	return pNewObj;
}


CGeometry* CReplaceLinesCommand::GetReplaceObj_needSelect()
{
	if( !m_pOldObj2 || m_pOldObj1==m_pOldObj2 )
		return GetReplaceObj_needSelect_same();
	
	//获得各个点在基线上的最近点
	PT_3D ptSel1, ptSel2, ptStart, ptEnd;
	
	double r = m_nSnapRadius;
	int start1=-1, end1=-1, start2=-1, end2=-1, i;
	
	Envelope e;
	
	//判断m_ptSel1,m_ptSel2两个点是否对应于m_pOldObj1,m_pOldObj2，如果不对应，则需要交换一下
	int bShift = 0;
	PT_3D p1;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptSel1,&p1);
	
	e .CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptSel1,NULL) )
		bShift++;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptSel2,&p1);
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj2->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptSel2,NULL) )
		bShift++;
	
	if( bShift!=0 )
	{
		ptSel1 = m_ptSel1; m_ptSel1 = m_ptSel2; m_ptSel2 = ptSel1;
	}

	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptSel1,&p1);
	
	e .CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptSel1,NULL) )
		return NULL;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptSel2,&p1);
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj2->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptSel2,NULL) )
		return NULL;
	

	
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&p1);
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptSel2,NULL) )
	{
	    start1 = -2;
	}

	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&p1);
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj2->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptSel2,NULL) )
	{
		start2 = -2;
	}

	
	if( start1==-2 || start2==-2 )return NULL;
	
	CGeometry *pCurve1 = (CGeometry*)m_pOldObj1->GetGeometry(), *pCurve2 = (CGeometry*)m_pOldObj2->GetGeometry();
	
	CArray<int,int> KeyPos1,KeyPos2;
	if(!pCurve1->GetShape()->GetKeyPosOfBaseLines(KeyPos1))return NULL;
	if(!pCurve2->GetShape()->GetKeyPosOfBaseLines(KeyPos2))return NULL;
	int keynum1 = KeyPos1.GetSize(), keynum2 = KeyPos2.GetSize();
	if( keynum1<=1 || keynum2<=1 )return NULL;

	CArray<PT_3DEX,PT_3DEX> pts1 ,pts2;
	if(!pCurve1->GetShape()->GetPts(pts1)) return NULL;
	if(!pCurve2->GetShape()->GetPts(pts2)) return NULL;
	

    int num1 = pts1.GetSize();
	int num2 = pts2.GetSize();
	//获得上述各个最近点在基线中的位置
	PT_3D rpt1,rpt2;
	double x,y,z;
	double dis,min1=-1,min2=-1;
	int i1=-1, i2=-1;
	
	//计算第一个地物

	for( i=0; i<num1-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts1[i].x,pts1[i].y,pts1[i].z,
			pts1[i+1].x,pts1[i+1].y,pts1[i+1].z,m_ptSel1.x,m_ptSel1.y,m_ptSel1.z,&x,&y,&z,false);
		
		if( min1<0 || dis<min1 )
		{ 
			min1 = dis; rpt1.x=x; rpt1.y=y; rpt1.z=z; 
			
			if( _FABS(x-pts1[i+1].x)+_FABS(y-pts1[i+1].y)>1e-10 )
				i1=i;
			//就是第二个点时，序号增加1
			else i1=i+1;
		}
		
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts1[i].x,pts1[i].y,pts1[i].z,
			pts1[i+1].x,pts1[i+1].y,pts1[i+1].z,m_ptStart.x,m_ptStart.y,m_ptStart.z,&x,&y,&z,false);
		
		if( min2<0 || dis<min2 )
		{ 
			min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
			if( _FABS(x-pts1[i+1].x)+_FABS(y-pts1[i+1].y)>1e-10 )
				i2=i;
			//就是第二个点时，序号增加1
			else i2=i+1;
		}
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	
		double d1=0, d2=0;
		d1 = _FABS(pts1[i1].x-rpt1.x)+_FABS(pts1[i1].y-rpt1.y)+_FABS(pts1[i1].z-rpt1.z);
		d2 = _FABS(pts1[i2].x-rpt2.x)+_FABS(pts1[i2].y-rpt2.y)+_FABS(pts1[i2].z-rpt2.z);
		
		if( d1>1e-10 )
		{
			
			pts1.InsertAt(i1+1,PT_3DEX(rpt1,0));
			
			
			for( i=0; i<keynum1; i++)if( KeyPos1[i]>i1 )KeyPos1[i] = KeyPos1[i]+1;
			if( i1<i2 ||(i1==i2&&d1<=d2) )i2++;
			i1++; num1++;
		}
		if( d2>1e-10 )
		{
			pts1.InsertAt(i2+1,PT_3DEX(rpt2,0));
		
			for( i=0; i<keynum1; i++)if( KeyPos1[i]>i2 )KeyPos1[i] = KeyPos1[i]+1;
			if( i2<i1 ||(i2==i1&&d2<d1) )i1++;
			i2++; num1++;
		}
	}
	
	if( i1<=i2 )
	{
		start1 = -1;
		end1 = i2;
	}
	else
	{
		start1 = num1;
		end1 = i2;
	}
	
	//计算第二个地物
	i1=-1, i2=-1;
	min1=-1,min2=-1;

	for( i=0; i<num2-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts2[i].x,pts2[i].y,pts2[i].z,
			pts2[i+1].x,pts2[i+1].y,pts2[i+1].z,m_ptSel2.x,m_ptSel2.y,m_ptSel2.z,&x,&y,&z,false);
		
		if( min1<0 || dis<min1 )
		{ 
			min1 = dis; rpt1.x=x; rpt1.y=y; rpt1.z=z; 
			
			if( _FABS(x-pts2[i+1].x)+_FABS(y-pts2[i+1].y)>1e-10 )
				i1=i;
			//就是第二个点时，序号增加1
			else i1=i+1;
		}
		
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts2[i].x,pts2[i].y,pts2[i].z,
			pts2[i+1].x,pts2[i+1].y,pts2[i+1].z,m_ptEnd.x,m_ptEnd.y,m_ptEnd.z,&x,&y,&z,false);
		
		if( min2<0 || dis<min2 )
		{ 
			min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
			if( _FABS(x-pts2[i+1].x)+_FABS(y-pts2[i+1].y)>1e-10 )
				i2=i;
			//就是第二个点时，序号增加1
			else i2=i+1;
		}
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
		
		double d1=0, d2=0;
		d1 = _FABS(pts2[i1].x-rpt1.x)+_FABS(pts2[i1].y-rpt1.y)+_FABS(pts2[i1].z-rpt1.z);
		d2 = _FABS(pts2[i2].x-rpt2.x)+_FABS(pts2[i2].y-rpt2.y)+_FABS(pts2[i2].z-rpt2.z);
		
		if( d1>1e-10 )
		{
			pts2.InsertAt(i1+1,PT_3DEX(rpt1,0));
			
			for( i=0; i<keynum2; i++)if( KeyPos2[i]>i1 )KeyPos2[i] = KeyPos2[i]+1;
			if( i1<i2 ||(i1==i2&&d1<=d2) )i2++;
			i1++; num2++;
		}
		if( d2>1e-10 )
		{
			pts2.InsertAt(i2+1,PT_3DEX(rpt2,0));
			
			for( i=0; i<keynum2; i++)if( KeyPos2[i]>i2 )KeyPos2[i] = KeyPos2[i]+1;
			if( i2<i1 ||(i1==i2&&d2<d1) )i1++;
			i2++; num2++;
		}
	}
	
	if( i1<=i2 )
	{
		start2 = -1;
		end2 = i2;
	}
	else
	{
		start2 = num2;
		end2 = i2;
	}
	
	//计算各个位置最邻近的关键点
	int startKey1, endKey1,startKey2, endKey2;
	for( i=0; i<keynum1; i++)
	{
		if( start1<KeyPos1[i] )break;
	}
	
	startKey1 = i;
	
	for( i=0; i<keynum1; i++)
	{
		if( end1<KeyPos1[i] )break;
	}
	
	endKey1 = i;
	
	for( i=0; i<keynum2; i++)
	{
		if( start2<KeyPos2[i] )break;
	}
	
	startKey2 = i;
	
	for( i=0; i<keynum2; i++)
	{
		if( end2<KeyPos2[i] )break;
	}
	
	endKey2 = i;
	
	//准备新地物
	CGeometry *pNewObj = (CGeometry*)m_pOldObj1->GetGeometry()->Clone();
	if( !pNewObj )
	{
		
		return NULL;
	}	
	PT_3DEX expt;
	CArray<PT_3DEX,PT_3DEX> arr;
	//首段
	pts1.RemoveAll();
	pNewObj->GetShape(pts1);
	if( start1>end1 )
	{
		for( i=keynum1-1; i>=endKey1; i--)
		{
			arr.Add(pts1.GetAt(i));
		}
	}
	else
	{
		for( i=0; i<endKey1; i++)
		{			
			arr.Add(pts1.GetAt(i));
		}
	}
	
	//替换段
//	int newnum = m_pDrawProc->m_pGeoCurve->GetDataPointSum();
	CArray<PT_3DEX,PT_3DEX> pts;
	m_pObj->GetShape(pts);
	arr.Append(pts);
// 	for( i=0; i<newnum; i++)
// 	{
// 		arr.Add(m_pDrawProc->m_pGeoCurve->GetDataPoint(i));
// 	}
	pts.RemoveAll();
	m_pOldObj2->GetGeometry()->GetShape(pts);
	//尾段
	if( start2>end2 )
	{
		for( i=endKey2; i<keynum2; i++)
		{
			
			arr.Add(pts.GetAt(i));
		}
	}
	else
	{
		for( i=endKey2-1; i>=0; i--)
		{
			arr.Add(pts.GetAt(i));
		}
	}
	
	pNewObj->CreateShape(arr.GetData(),arr.GetSize());
	
	//去除重复的点(主要因为起点、终点落在原对象节点上)
	PT_3DEX tt;
	keynum1 = arr.GetSize();
	int pos = 0;
	tt = arr.GetAt(pos);
	for( i=1; i<keynum1 && keynum1>2; )
	{
		expt = arr.GetAt(i);
		if( _FABS(expt.x-tt.x)<1e-4 && _FABS(expt.y-tt.y)<1e-4 && _FABS(expt.z-tt.z)<1e-4 )
		{
			arr.RemoveAt(i);
			keynum1--;
			continue;
		}
		else
		{
			pos++;
			tt = arr.GetAt(pos);
		}
		i++;
	}
	pNewObj->CreateShape(arr.GetData(),arr.GetSize());
	return pNewObj;
}




//////////////////////////////////////////////////////////////////////
// CReplaceLinesCommand_overlap Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CReplaceLinesCommand_overlap,CReplaceLinesCommand)

CReplaceLinesCommand_overlap::CReplaceLinesCommand_overlap()
{
	m_nMode = modeTwoPoint;
	m_bCanTwoObjs = FALSE;
	m_bMatchHeight = FALSE;
}


CReplaceLinesCommand_overlap::~CReplaceLinesCommand_overlap()
{
}


CString CReplaceLinesCommand_overlap::Name()
{
	return StrFromResID(IDS_CMDNAME_REPLACE_OVERLAP);
}

void CReplaceLinesCommand_overlap::Start()
{
	CReplaceLinesCommand::Start();

	m_nMode = modeTwoPoint;
	m_bCanTwoObjs = FALSE;
	m_bMatchHeight = FALSE;
}

void CReplaceLinesCommand_overlap::OnSonEnd(CProcedure *son)
{
	if( !m_pObj||m_pObj->GetDataPointSum()<2 )
	{
		Abort();
		return;
	}
	if(m_nMode==modeTwoPoint)
	{
		if(m_pOldObj1==NULL)
		{
			Abort();
			return;
		}

		CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

		BOOL bRet1 = FALSE, bRet2 = FALSE;
		PT_3D pt1, pt2, pt0;
		Envelope e;
		double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&pt0);
		e.CreateFromPtAndRadius(pt0,r);
		bRet1 = m_pOldObj1->GetGeometry()->FindNearestBasePt(pt0,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt1,NULL);

		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&pt0);
		e.CreateFromPtAndRadius(pt0,r);
		CPFeature pObj2 = pDQ->FindNearestObject(pt0,r,m_pEditor->GetCoordWnd().m_pSearchCS);
		if(pObj2)
			bRet2 = pObj2->GetGeometry()->FindNearestBasePt(pt0,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt2,NULL);

		//寻找所有经过pt1和pt2的地物，进行修测		
		e.CreateFromPtAndRadius(pt1,GraphAPI::GetDisTolerance());
		pDQ->FindObjectInRect(e,NULL);
		CFtrArray arr1, arr2;
		int num = 0;
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		if(num>0)
		{
			arr1.SetSize(num);
			memcpy(arr1.GetData(),ftrs,num*sizeof(CPFeature));			
		}

		e.CreateFromPtAndRadius(pt2,GraphAPI::GetDisTolerance());
		pDQ->FindObjectInRect(e,NULL);
		ftrs = pDQ->GetFoundHandles(num);

		if(num>0)
		{
			arr2.SetSize(num);
			memcpy(arr2.GetData(),ftrs,num*sizeof(CPFeature));			
		}

		if(!bRet1 || !bRet2)
		{
			arr1.RemoveAll();
		}

		//清除未同时经过pt1,pt2的地物
		for(int i=arr1.GetSize()-1; i>=0; i--)
		{
			for(int j=0; j<arr2.GetSize(); j++)
			{
				if(arr2[j]==arr1[i])break;
			}
			if(j>=arr2.GetSize())
			{
				arr1.RemoveAt(i);
			}
		}

		//没有多个符合条件的地物，就按照修测原来的逻辑进行处理
		if(arr1.GetSize()<1)
		{
			CGeometry *pObj = GetReplaceObj();
			if (!pObj)
			{
				Finish();
				return;
			}
				
			m_pEditor->UpdateDrag(ud_ClearDrag);	
			
			CFeature *pFtr = HandleToFtr(m_objOldID1)->Clone();
			if (!pFtr) return;

			CUndoFtrs undo(m_pEditor,Name());
			
			int layid = m_pEditor->GetFtrLayerIDOfFtr(m_objOldID1);
			pFtr->SetID(OUID());
			pFtr->SetGeometry(pObj);
			if(m_pEditor->AddObject(pFtr,layid))
			{
				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objOldID1),pFtr);
				undo.AddNewFeature(FtrToHandle(pFtr));
			}
			if(m_pEditor->DeleteObject(m_objOldID1))
			{
				undo.AddOldFeature(m_objOldID1);
			}
			if(m_pEditor->DeleteObject(m_objOldID2))
			{
				undo.AddOldFeature(m_objOldID2);
			}		
			
			undo.Commit();
			Finish();
			
			return;
		}

		CUndoFtrs undo(m_pEditor,Name());

		for(i=0; i<arr1.GetSize(); i++)
		{		
			m_objOldID1 = FtrToHandle(arr1[i]);
			m_pOldObj1 = arr1[i];

			CGeometry *pObj = GetReplaceObj();
			if (!pObj)
			{
				continue;
			}				
			
			CFeature *pFtr = HandleToFtr(m_objOldID1)->Clone();
			if (!pFtr) continue;
			
			int layid = m_pEditor->GetFtrLayerIDOfFtr(m_objOldID1);
			pFtr->SetID(OUID());
			pFtr->SetGeometry(pObj);
			if(m_pEditor->AddObject(pFtr,layid))
			{
				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objOldID1),pFtr);
				undo.AddNewFeature(FtrToHandle(pFtr));
			}
			if(m_pEditor->DeleteObject(m_objOldID1))
			{
				undo.AddOldFeature(m_objOldID1);
			}
			if(m_pEditor->DeleteObject(m_objOldID2))
			{
				undo.AddOldFeature(m_objOldID2);
			}			
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);	
		
		undo.Commit();
		Finish();
	}
}


void CReplaceLinesCommand_overlap::GetParams(CValueTable& tab)
{
	
}


void CReplaceLinesCommand_overlap::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(AccelStr(),Name());
}


void CReplaceLinesCommand_overlap::SetParams(CValueTable& tab,BOOL bInit)
{
	
}

	


//////////////////////////////////////////////////////////////////////
// CExtendCollectCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CExtendCollectCommand,CDrawCommand)

CExtendCollectCommand::CExtendCollectCommand()
{
	m_nStep = 0;
	m_bReverse = FALSE;
	m_pDrawProc = NULL;
	m_pOldFtr = NULL;
	pUndo = NULL;
	strcat(m_strRegPath,"\\ExtendCollect");
}

CExtendCollectCommand::~CExtendCollectCommand()
{
	if( m_pDrawProc )
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(pUndo)
	{
		delete pUndo;
		pUndo = NULL;
	}
}

void CExtendCollectCommand::Start()
{
	if(!m_pEditor) return;
	m_nStep = 0;
	m_pFtr = NULL;
	m_pOldFtr = NULL;
	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);

	CGeoCurve *pGeo = new CGeoCurve;
	if(!pGeo) return;
	m_pDrawProc->m_pGeoCurve = pGeo;
	UpdateParams(FALSE);
	m_pDrawProc->Start();

	pUndo = new CUndoFtrs(m_pEditor, Name());
	
	CDrawCommand::Start();
}

CString CExtendCollectCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_EXTENDCOLLECT);
}

void CExtendCollectCommand::GetParams(CValueTable& tab)
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

void CExtendCollectCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(PID_DRAWCURVE,StrFromLocalResID(IDS_CMDNAME_PLACECURVE));
	if( m_pDrawProc )
	{
		PT_3D pt;
		param->AddParam(PF_POINT,pt,StrFromLocalResID(IDS_CMDPLANE_POINT));
// 		if(m_nStep==0)
// 			param->AddParam(PF_CLOSED,false,StrFromLocalResID(IDS_CMDPLANE_CLOSE));
// 		else
// 			param->AddParam(PF_CLOSED,(bool)(m_pDrawProc->m_bClosed),StrFromLocalResID(IDS_CMDPLANE_CLOSE));
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
		param->AddParam("ConnectAccKey",'f',StrFromResID(IDS_CMDNAME_COMBINE));
		
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
		
		param->AddParam(PF_LENGTH,double(1.0),StrFromResID(IDS_FIELDNAME_LENGTH),NULL,'D',FALSE);
		
		param->AddParam(PF_ANGLE,double(0.0),StrFromResID(IDS_FIELDNAME_ANGLE),NULL,'A',FALSE);
		
		if (m_pDrawProc->m_bMultiPtBuildPt || bForLoad)
		{
			param->AddParam(PF_BUILDPTTOLER,m_pDrawProc->m_fTolerBuildPt,StrFromResID(IDS_CMDPLANE_BUILDPTTOLERANCE));
		}
	}
}

void CExtendCollectCommand::SetParams(CValueTable& tab,BOOL bInit)
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
			m_bReverse = !m_bReverse;
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
	if( tab.GetValue(0,"ConnectAccKey",var) )
	{
		char ch = (BYTE)(_variant_t)*var;
		if(ch == 'f'||ch == 'F')
		{
			Connect();
			return;
		}
	}

	// 通过长度和角度指定点
	if (tab.GetValue(0,PF_LENGTH,var))
	{
		const CVariantEx *var1;
		if (tab.GetValue(0,PF_ANGLE,var1))
		{
			double len = (double)(_variant_t)*var;
			double ang = (double)(_variant_t)*var1;

			int num = m_pDrawProc->m_arrPts.GetSize();
			if (num > 0)
			{
				PT_3DEX pt = m_pDrawProc->m_arrPts[num-1];
				pt.x += len*cos(ang/180*PI);
				pt.y += len*sin(ang/180*PI);

				PtClick(pt,0);
				PtMove(pt);
				m_pEditor->RefreshView();
			}
		}
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
	
	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab,bInit);
}

void CExtendCollectCommand::PtReset(PT_3D &pt)
{
	CArray<PT_3DEX,PT_3DEX> arr;
	arr.Copy(m_pDrawProc->m_arrPts);
	if(arr.GetSize()>0)
	{
		CGeometry *pGeo = m_pFtr->GetGeometry();
		if(m_bReverse)
		{
			int nSize = arr.GetSize();
			for(int i=0; i<nSize/2; i++)
			{
				PT_3DEX temp = arr[i];
				arr[i] = arr[nSize-1-i];
				arr[nSize-1-i] = temp;
			}
		}
		if(m_pDrawProc->m_bClosed)
			arr.Add(arr[0]);
		pGeo->CreateShape(arr.GetData(),arr.GetSize());
		CDataSourceEx *pDS = m_pEditor->GetDataSource();
		if(!pDS)
		{
			Abort();
			return;
		}
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(m_pOldFtr);
		if(!pLayer || !pLayer->IsVisible())
		{
			Abort();
			return;
		}
		m_pEditor->AddObject(m_pFtr, pLayer->GetID());
		pUndo->AddNewFeature(FtrToHandle(m_pFtr));
		pUndo->AddOldFeature(FtrToHandle(m_pOldFtr));
		pUndo->Commit();
		Finish();
	}
	else
	{
		Abort();
	}
}

void CExtendCollectCommand::Abort()
{
	if( m_pDrawProc )
	{
		if(m_pOldFtr)
			m_pEditor->RestoreObject(FtrToHandle(m_pOldFtr));
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	m_nStep = 0;
	CDrawCommand::Abort();
}

void CExtendCollectCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	m_nStep = 0;
	CDrawCommand::Finish();
}

void CExtendCollectCommand::Back()
{
	if(m_pDrawProc)
	{
		if(m_pDrawProc->m_arrPts.GetSize()<=1)
		{
			Abort();
			return;
		}
		else
		{
			m_pDrawProc->Back();
		}
	}
}

void CExtendCollectCommand::Connect()
{
	if(!m_pEditor) return;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ) return;

	CView *pView = GetActiveView();
	if(!pView || !pView->IsKindOf(RUNTIME_CLASS(CBaseView)))
		return;
	PT_3D pt = ((CBaseView*)pView)->GetCurPoint();
	
	int ptsum = m_pDrawProc->m_arrPts.GetSize();
	if(ptsum<2)
	{
		PtClick(pt, 0);
		return;
	}
	
	Envelope e;
	double r = 0.01;
	e.CreateFromPtAndRadius(pt, r);
	pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);
	
	int num, i, j;
	const CPFeature *ftrs = pDQ->GetFoundHandles(num);
	if(num<=0) return;
	
	CArray<PT_3DEX,PT_3DEX> pts, temp;
	PT_3DEX curPt(pt, penNone);
	for(i=0; i<num; i++)
	{
		CGeometry *pGeo = ftrs[i]->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			continue;
		
		pGeo->GetShape(temp);
		int nPt = temp.GetSize();
		if(nPt<2) continue;
		
		if( GraphAPI::GIsEqual2DPoint(&curPt, &temp[0]) )
		{
			pts.Append(temp);
			break;
		}
		else if( GraphAPI::GIsEqual2DPoint(&curPt, &temp[nPt-1]) )
		{
			for(j=nPt-1; j>=0; j--)
			{
				pts.Add(temp[j]);
			}
			break;
		}
	}
	
	if(i>=num) return;
	
	GrBuffer buf;
	buf.BeginLineString(m_pDrawProc->m_layCol,0);
	PT_3D lastpt = m_pDrawProc->m_arrPts[ptsum-1];
	buf.MoveTo(&lastpt);
	buf.LineTo(&pts[0]);
	buf.Lines(pts.GetData(), pts.GetSize(), sizeof(PT_3DEX));
	buf.End();
	m_pEditor->UpdateDrag(ud_AddConstDrag,&buf);
	
	m_pDrawProc->m_arrPts.Append(pts);
	
	pUndo->AddOldFeature(FtrToHandle(ftrs[i]));
	m_pEditor->DeleteObject(FtrToHandle(ftrs[i]));
	
	PT_3DEX pt0 = pts[pts.GetSize()-1];
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
}


void CExtendCollectCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pDrawProc)
		return;
	if(m_nStep==1)
	{
		m_pDrawProc->PtClick(pt,flag);
	}
	if( m_nStep==0 )
	{
		CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
		//查找延长的地物
		Envelope e;
		double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
		PT_4D ptsch = pt;
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&ptsch,1);
		e.CreateFromPtAndRadius(ptsch, r);
		int num = pDQ->FindObjectInRect(e, m_pEditor->GetCoordWnd().m_pSearchCS);
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		if(num<=0) return;  

		CFeature *pFtr0 = ftrs[0];
		CGeometry* pGeo0 = pFtr0->GetGeometry();
		if(!pGeo0) return;

		int ptsum = pGeo0->GetDataPointSum();
		if(ptsum<2)
		{
			Abort();
			return;
		}

		CGeometry* pObj1 = NULL;
		CGeometry* pObj2 = NULL;
		CGeometry* pObj3 = NULL;
		PT_3D pt0, pt1;
		COPY_3DPT(pt0, pGeo0->GetDataPoint(0));
		COPY_3DPT(pt1, pGeo0->GetDataPoint(ptsum-1));
		if(GraphAPI::GIsEqual3DPoint(&pt0, &pt1))
		{
			m_pDrawProc->m_bClosed = TRUE;//让闭合的地物继续闭合
		}
		else
		{
			m_pDrawProc->m_bClosed = FALSE;
		}
		((CGeoCurve*)pGeo0)->GetBreak(pt0, pt, pObj1, pObj2, pObj3);
		CGeometry* pObj = NULL;
		if(pObj2==NULL && pObj3)
		{
			pObj = pObj3;
		}
		else if(pObj2 && pObj3==NULL)
		{
			pObj = pObj2;
		}
		else if(pObj2 && pObj3)
		{
			double len1 = pObj2->GetShape()->GetLength();
			double len2 = pObj3->GetShape()->GetLength();
			if(len1>len2)
			{
				pObj = pObj2;
			}
			else
			{
				pObj = pObj3;
			}
		}
		
		if(pObj)
		{
			CArray<PT_3DEX, PT_3DEX> arr;
			pObj->GetShape(arr);

			if(GraphAPI::GIsEqual3DPoint(&arr[0], &arr[arr.GetSize()-1]))
			{
				arr.RemoveAt(arr.GetSize()-1);
			}

			if(pObj==pObj3)
			{
				for(int i=arr.GetSize()-1; i>=0; i--)
				{
					m_pDrawProc->m_arrPts.Add(arr[i]);
				}
				m_bReverse = TRUE;
			}
			else
			{
				m_pDrawProc->m_arrPts.Copy(arr);
			}
			
			long color = pGeo0->GetColor();
			if (color == FTRCOLOR_BYLAYER)
			{
				CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(pFtr0);
				if (!pLayer)
				{
					if(pObj1) {delete pObj1; pObj1=NULL;}
					if(pObj2) {delete pObj2; pObj2=NULL;}
					if(pObj3) {delete pObj2; pObj3=NULL;}
					Abort();
					return;
				}
				color = pLayer->GetColor();
			}
			m_pDrawProc->m_layCol = color;

			m_pOldFtr = pFtr0;

			PT_3D pt3d;
			int npt = m_pDrawProc->m_arrPts.GetSize();
			COPY_3DPT(pt3d, m_pDrawProc->m_arrPts[npt-1]);
			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt3d);
			
			m_pFtr = pFtr0->Clone();
			m_pFtr->GetGeometry()->CreateShape(m_pDrawProc->m_arrPts.GetData(), m_pDrawProc->m_arrPts.GetSize());

			GrBuffer vbuf;
			vbuf.BeginLineString(0,0,0);					
			vbuf.Lines(m_pDrawProc->m_arrPts.GetData(), m_pDrawProc->m_arrPts.GetSize(), sizeof(PT_3DEX));
			vbuf.End();	
			vbuf.SetAllColor(color);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&vbuf);
			
			m_pEditor->DeleteObject(FtrToHandle(pFtr0));
			GotoState(PROCSTATE_PROCESSING);
		}
		
		if(pObj1) {delete pObj1; pObj1=NULL;}
		if(pObj2) {delete pObj2; pObj2=NULL;}
		if(pObj3) {delete pObj2; pObj3=NULL;}
		
		m_nStep=1;
	}
}

void CExtendCollectCommand::PtMove(PT_3D &pt)
{
	if(m_nStep==1 && m_pDrawProc)
	{
		m_pDrawProc->PtMove(pt);
	}
}

int  CExtendCollectCommand::GetCurPenCode()
{
	if(m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}

DrawingInfo CExtendCollectCommand::GetCurDrawingInfo()
{
	if (m_pDrawProc)
	{
		CFeature *pFtr = new CFeature;
		pFtr->SetGeometry(m_pDrawProc->m_pGeoCurve);
		return DrawingInfo(pFtr,m_pDrawProc->m_arrPts);
	}
	else
		return DrawingInfo();
}



//////////////////////////////////////////////////////////////////////
// CReplaceLinesWithCatchIntersecCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CReplaceLinesWithCatchIntersecCommand,CDrawCommand)

CReplaceLinesWithCatchIntersecCommand::CReplaceLinesWithCatchIntersecCommand()
{
//	m_bAutoClosed = FALSE;

	m_bSnap=TRUE;
	m_nflag=0;

	m_bVWsnaplinestyle = TRUE;         //标志矢量窗口吸取线型
	m_b3Dsnaplinestyle = TRUE;

	m_bShowMark = TRUE;
	m_nStep = -1;
	m_objOldID1 = 0;
	m_objOldID2 = 0;
    m_pObj = NULL;
		
	m_pOldObj1 = NULL;
	m_pOldObj2 = NULL;
	m_nStart = -1;
	m_nEnd = -1;

	
	m_nSaveViewParam = 0;
	m_bUseViewParam = FALSE;
	
	m_nSnapRadius = 20;

	m_pDrawProc = NULL;
	strcat(m_strRegPath,"\\ReplaceLinesWithCatchIntersec");

}

CReplaceLinesWithCatchIntersecCommand::~CReplaceLinesWithCatchIntersecCommand()
{
	if( m_pDrawProc )delete m_pDrawProc;
	if (m_pObj)
	{
		delete m_pObj;
		m_pObj = NULL;
	}
}

DrawingInfo CReplaceLinesWithCatchIntersecCommand::GetCurDrawingInfo()
{
	if (m_pDrawProc)
	{
		CFeature *pFtr = new CFeature;
		pFtr->SetGeometry(m_pDrawProc->m_pGeoCurve);
		return DrawingInfo(pFtr,m_pDrawProc->m_arrPts);
	}
	else
		return DrawingInfo();
}

CProcedure *CReplaceLinesWithCatchIntersecCommand::GetActiveSonProc(int nMsgType)
{
	if( nMsgType==msgPtClick || nMsgType==msgPtMove || nMsgType==msgPtReset )
		return NULL;
	
	return m_pDrawProc;
}

void CReplaceLinesWithCatchIntersecCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CFeature *pOldObj = NULL;
		PT_3D p1;
		PT_3D pt0;
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
		double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
		Envelope e;
		e.CreateFromPtAndRadius(p1,r);
		pOldObj = m_pEditor->GetDataQuery()->FindNearestObject(p1,r,m_pEditor->GetCoordWnd().m_pSearchCS);	
		if( pOldObj )
		{	
			if(  !(pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
				return;
			if (!CModifyZCommand::CheckObjForContour(pOldObj->GetGeometry()))
			{
				return;
			}
			if(!pOldObj->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt0,NULL) )
			{
				return;
			}
			m_ptStart = pt;
			m_objOldID1 = FtrToHandle(pOldObj);

			m_pOldObj1 = pOldObj;

			Envelope ee;
			ee.CreateMaxEnvelope();
			int pos ;
			m_pOldObj1->GetGeometry()->GetShape()->FindNearestLine(p1,ee,m_pEditor->GetCoordWnd().m_pSearchCS,NULL,NULL,NULL,&pos);
			
			//得到前面的关键点的绘图钢笔码以及高程
			CArray<int,int> pKeyPos;
			m_pOldObj1->GetGeometry()->GetShape()->GetKeyPosOfBaseLines(pKeyPos);
			if( pKeyPos.GetSize()>0 )
			{
				int num = m_pOldObj1->GetGeometry()->GetDataPointSum()-1;
				while( num>=0 && pKeyPos[num]>pos )num--;
				
				if( num>=0 )
				{
					PT_3DEX expt;
					expt = m_pOldObj1->GetGeometry()->GetDataPoint(num);
					if(!m_pEditor->GetCoordWnd().m_bIsStereo)
					{
						if(m_bVWsnaplinestyle)//如果m_bVWsnaplinestyle是true，则吸取，否则不吸取，而根据工具栏的选择确定
							m_pDrawProc->m_nCurPenCode = expt.pencode;
						else
							;
					}
					else
					{
						if(m_b3Dsnaplinestyle)//如果m_bVWsnaplinestyle是true，则吸取，否则不吸取，而根据工具栏的选择确定
							m_pDrawProc->m_nCurPenCode = expt.pencode;
						else
							;
					}
											
					// 保存立体视图参数
					char params[256]={0};
					PDOC(m_pEditor)->UpdateAllViewsParams(1,(LPARAM)params);
					if( sscanf(params,"%d",&m_nSaveViewParam)==1 )
						m_bUseViewParam = TRUE;

					/*CView *pView = PDOC(m_pEditor)->GetCurActiveView();
					if (pView)
					{
						pView->SendMessage(FCCM_UPDATESTEREOPARAM, 1, (LPARAM)params);
						if( sscanf(params,"%d",&m_nSaveViewParam)==1 )
							m_bUseViewParam = TRUE;
					}*/
					
					// 设置目标层码对应的视图参数
					CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
					if (!pDS){ Abort(); return;}
					int nLayerId = PDOC(m_pEditor)->GetFtrLayerIDOfFtr(m_objOldID1);
					CFtrLayer *pLayer = pDS->GetFtrLayer(nLayerId);
					if (pLayer)
					{
						CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(pDS->GetScale());
						if (pPlaceLib)
						{
							CPlaceConfig *pConfig = pPlaceLib->GetConfig(pLayer->GetName());
							if (pConfig/* && pView*/)
							{
								PDOC(m_pEditor)->UpdateAllViewsParams(0,(LPARAM)pConfig->m_strViewParams);
								//pView->SendMessage(FCCM_UPDATESTEREOPARAM, 0, (LPARAM)pConfig->m_strViewParams);
							}
						}		
						
					}
				
					//立体窗口
					if( m_pEditor->GetCoordWnd().m_bIsStereo )
					{
						((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_ModifyHeight,(CObject*)&pt0.z);
					}
					
					m_ptStart.z = pt0.z;
					pt.z = pt0.z;

					//二维窗口
					/*if( !m_pEditor->GetCoordWnd().m_bIsStereo )
					{
						pt.z = pt0.z;
						m_ptStart.z = pt0.z;
					}
					else if( CModifyZCommand::CheckObjForContour(m_pOldObj1->GetGeometry()) )
					{
						((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_ModifyHeight,(CObject*)&pt0.z);
						m_ptStart.z = pt0.z;
						pt.z = pt0.z;
					}*/
				}
			}
			
			//更新显示
			GrBuffer buf;
			buf.BeginLineString(0,0);
			DrawPointTip(m_pOldObj1->GetGeometry(),pt0,&buf);
			buf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);

			long color = m_pOldObj1->GetGeometry()->GetColor();
			if (color == FTRCOLOR_BYLAYER)
			{
				CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pOldObj1);
				if (!pLayer)
				{
					Abort();
					return;
				}
				color = pLayer->GetColor();
			}
			m_pObj->SetColor(color);

		}

 		m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
		m_pEditor->RefreshView();
 		
 		m_nStep = 1;
 		GotoState(PROCSTATE_PROCESSING);
	}		
	else if (m_nStep==1)
	{
		if (m_nflag==1&&m_bSnap)
		{
			//更改所画折线的尾点的位置
			m_pDrawProc->PtClick(m_pt,flag);
			return;
		}
		
	}
	if( m_pDrawProc )
	{
		m_pDrawProc->PtClick(pt,flag);
	}
	CDrawCommand::PtClick(pt,flag);
}

void CReplaceLinesWithCatchIntersecCommand::PtMove(PT_3D &pt)
{
	if( m_pDrawProc )
	{
		m_pDrawProc->PtMove(pt);
	}
	CDrawCommand::PtMove(pt);

	if( IsProcFinished(this) )return;
		
	m_nflag=0;
	double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();		
	
	//m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	//查找
	
	CFeature *pOldObj = NULL;
	PT_3D p1;
	PT_3D pt0;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&p1);
	Envelope e;
	e.CreateFromPtAndRadius(p1,r);
	pOldObj = m_pEditor->GetDataQuery()->FindNearestObject(p1,r,m_pEditor->GetCoordWnd().m_pSearchCS);	
	if( pOldObj )
	{	
		if(  !(pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
			return;
		if (m_nStep==0)
		{
			if (!CModifyZCommand::CheckObjForContour(pOldObj->GetGeometry()))
			{
				return;
			}
			if(!pOldObj->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt0,NULL) )
			{
				return;
			}	
				
		}
		if (m_nStep==1)
		{
			if (!m_pOldObj1) 
			{
				Finish();
				return;
			}				
			if(m_pOldObj1)
			{
				if (FtrToHandle(pOldObj)==m_objOldID1)
				{
					if(!pOldObj->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&pt0,NULL) )
					{
						return;
					}	
				}
				else
				{
// 					//层码必须一致
// 					CFtrLayer *pLayer1 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pOldObj1);
// 					CFtrLayer *pLayer2 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(pOldObj);
// 					
// 					if( pLayer1 && pLayer2&&pLayer1 != pLayer2&&
// 						strcmp(pLayer1->GetName(),pLayer2->GetName())!=0 )
// 					{
// 						return;
// 					}
					//如果是面，就不支持同时修测两个地物
					if( (m_pOldObj1&&m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))||
						(pOldObj&&pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
					{
						if( FtrToHandle(pOldObj)!=m_objOldID1 )return;
					}
					else
					{
						
						if(!FindPointOfSameZInObject(pOldObj->GetGeometry(),r,m_ptStart.z,pt,&pt0 ))
							return;
						m_nflag = 1;
						m_pt = pt0;
					}
				}
			
			}			
		}
	}

		
	GrBuffer vbuf;	
	if( pOldObj && m_bShowMark)
	{		
		//更新显示
		GrBuffer buf;
		buf.BeginLineString(0,0);
		DrawPointTip(pOldObj->GetGeometry(),pt0,&buf);
		buf.End();
		vbuf.CopyFrom(&buf);
	}

	if (m_pDrawProc && m_pDrawProc->m_pGeoCurve->GetDataPointSum()<=0)
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	}
	
	m_pEditor->UpdateDrag(ud_AddVariantDrag,&vbuf);
	return;		
}


void CReplaceLinesWithCatchIntersecCommand::OnSonEnd(CProcedure *son)
{
	if( !m_pObj )
	{
		CDrawCommand::Abort();
		return;
	}
	{
		CGeometry *pObj = GetReplaceObj();
		if (!pObj)
		{
			Finish();
			return;
		}
		
		m_pEditor->UpdateDrag(ud_ClearDrag);	
		
		CFeature *pFtr = HandleToFtr(m_objOldID1)->Clone();
		if (!pFtr) return;	

		CUndoFtrs undo(m_pEditor,Name());
		
		int layid = m_pEditor->GetFtrLayerIDOfFtr(m_objOldID1);	
		pFtr->SetID(OUID());
		pFtr->SetGeometry(pObj);
		if(m_pEditor->AddObject(pFtr,layid))
		{
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objOldID1),pFtr);
			undo.AddNewFeature(FtrToHandle(pFtr));
		}
		if(m_pEditor->DeleteObject(m_objOldID1))
		{	
			undo.AddOldFeature(m_objOldID1);			
		}
		if(m_pEditor->DeleteObject(m_objOldID2))
		{
			undo.AddOldFeature(m_objOldID2);	
		}		
		undo.Commit();	
		Finish();
	}
}


void CReplaceLinesWithCatchIntersecCommand::PtReset(PT_3D &pt)
{
	if( m_pDrawProc )
	{
		m_pDrawProc->PtReset(pt);
	}
	//画线操作结束
	if( m_nStep==1 )
	{
		if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
			return;

		if( m_pDrawProc->m_pGeoCurve->GetDataPointSum()<=1 )
		{
			Abort();
			return;
		}

		m_pObj = m_pDrawProc->m_pGeoCurve->Clone();
		//获得终点
		PT_3DEX expt;
		expt = m_pObj->GetDataPoint(m_pObj->GetDataPointSum()-1);
		COPY_3DPT(m_ptEnd,expt);
	}
}

CString CReplaceLinesWithCatchIntersecCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_REPLACECATCH);
}

void CReplaceLinesWithCatchIntersecCommand::Start()
{	
	m_bSnap=TRUE;
	m_nflag=0;

	m_bVWsnaplinestyle = TRUE;         //标志矢量窗口吸取线型
	m_b3Dsnaplinestyle = TRUE;
	m_nStep = 0;

	m_bShowMark = TRUE;

	m_objOldID1 = 0;
	m_objOldID2 = 0;
//	m_objNewID = 0;
	m_pOldObj1 = NULL;
	m_pOldObj2 = NULL;
	m_nStart = -1;
	m_nEnd = -1;

	m_pObj = NULL;

	m_nSaveViewParam = 0;
	m_bUseViewParam = FALSE;
	m_nSnapRadius = 20;	

	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	UpdateParams(FALSE);
	m_pObj = new  CGeoCurve;
	if(!m_pObj) return;
	m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pObj;
	m_pDrawProc->Start();
	CDrawCommand::Start();
}

void CReplaceLinesWithCatchIntersecCommand::Abort()
{

	if( m_bUseViewParam )
	{
		char params[256]={0};
		sprintf(params,"%d",m_nSaveViewParam);
		
		PDOC(m_pEditor)->UpdateAllViewsParams(0,(LPARAM)params);

// 		CView *pView = PDOC(m_pEditor)->GetCurActiveView();
// 		if (pView)  pView->SendMessage(FCCM_UPDATESTEREOPARAM, 0, (LPARAM)params);
		
	}
	
	UpdateParams(TRUE);

	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pObj)
	{
		delete m_pObj;
		m_pObj = NULL;
	}
	
	CDrawCommand::Abort();

}

void CReplaceLinesWithCatchIntersecCommand::Finish()
{	
	CDrawCommand::Finish();
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pObj)
	{
		delete m_pObj;
		m_pObj = NULL;
	}
}


void CReplaceLinesWithCatchIntersecCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);
 	_variant_t var;
	var = (bool)(m_bShowMark);
	tab.AddValue(PF_REPLACELINESHOWMARK,&CVariantEx(var));
	var = (_variant_t)(long)(m_nSnapRadius);
	tab.AddValue(PF_REPLACELINESNAPRADIUS,&CVariantEx(var));
	var = (bool)(m_bSnap);
	tab.AddValue(PF_REPLACELINESNAP,&CVariantEx(var));
	var = (bool)(m_bVWsnaplinestyle);
	tab.AddValue(PF_REPLACELINEVECTOR,&CVariantEx(var));
	var = (bool)(m_b3Dsnaplinestyle);
	tab.AddValue(PF_REPLACELINESTEREO,&CVariantEx(var));
	if(m_pDrawProc)
	{
		var = (_variant_t)(long)(m_pDrawProc->m_nCurPenCode);
		tab.AddValue(PF_PENCODE,&CVariantEx(var));
	}
}


void CReplaceLinesWithCatchIntersecCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
  	param->SetOwnerID("ReplaceLinesWithCatchIntersecCommand",StrFromResID(IDS_CMDNAME_REPLACECATCH));

 	param->AddParam(PF_REPLACELINESHOWMARK,(bool)m_bShowMark,StrFromResID(IDS_CMDPLANE_SHOWMARK));
 	param->AddParam(PF_REPLACELINESNAPRADIUS,(int)m_nSnapRadius,StrFromResID(IDS_CMDPLANE_SNAPRADIUS));
	param->AddParam(PF_REPLACELINESNAP,(bool)m_bSnap,StrFromResID(IDS_CMDPLANE_SNAP));
 	param->AddParam(PF_REPLACELINEVECTOR,(bool)m_bVWsnaplinestyle,StrFromResID(IDS_CMDPLANE_VWSNAPLINESTYLE));
 	param->AddParam(PF_REPLACELINESTEREO,(bool)m_b3Dsnaplinestyle,StrFromResID(IDS_CMDPLANE_TDSNAPLINESTYLE));
		
	if(m_pDrawProc)
		param->AddLineTypeParam(PF_PENCODE,m_pDrawProc->m_nCurPenCode,StrFromLocalResID(IDS_CMDPLANE_LINETYPE));	
// 	param->AddParam(PF_COPYROTATEKPOLD,bool(m_bKeepOld),StrFromResID(IDS_CMDPLANE_KEEPOLD));
	
}

int  CReplaceLinesWithCatchIntersecCommand::GetCurPenCode()
{
	if (m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}

void CReplaceLinesWithCatchIntersecCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;

	if( tab.GetValue(0,PF_REPLACELINESHOWMARK,var) )
	{
		m_bShowMark = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINESNAP,var) )
	{
		m_bSnap = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINESNAPRADIUS,var) )
	{
		m_nSnapRadius = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINEVECTOR,var) )
	{
		m_bVWsnaplinestyle = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_REPLACELINESTEREO,var) )
	{
		m_b3Dsnaplinestyle = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if(m_pDrawProc&&tab.GetValue(0,PF_PENCODE,var) )
	{
		m_pDrawProc->m_nCurPenCode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CDrawCommand::SetParams(tab,bInit);
}



void CReplaceLinesWithCatchIntersecCommand::DrawPointTip(CGeometry *pObj, PT_3D pt, GrBuffer *buf)
{
	PT_4D pts[4];
	pts[0] = PT_4D(pt);
	m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(pts,1);
	
	pts[0].x -= 10; pts[0].y -= 10; pts[0].z -= 10; pts[0].yr -= 10;
	
	pts[1].x = pts[0].x+20; pts[1].y = pts[0].y+20; 
	pts[1].z = pts[0].z+20; pts[1].yr = pts[0].yr+20; 
	
	pts[2].x = pts[0].x; pts[2].y = pts[0].y+20; 
	pts[2].z = pts[0].z; pts[2].yr = pts[0].yr+20; 
	
	pts[3].x = pts[0].x+20; pts[3].y = pts[0].y; 
	pts[3].z = pts[0].z+20; pts[3].yr = pts[0].yr; 
	
	m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pts,4);
	
	PT_3D pt3ds[4];
	for( int i=0; i<4; i++)pt3ds[i] = pts[i].To3D();
	
	buf->MoveTo(pt3ds);
	buf->LineTo(pt3ds+1);
	buf->MoveTo(pt3ds+2);
	buf->LineTo(pt3ds+3);
}

bool CReplaceLinesWithCatchIntersecCommand::FindPointOfSameZInObject(CGeometry *pObj, double r,double z,PT_3D pt, PT_3D *ret)
{
	if (!pObj)
	{
		return false;
	}

	PT_3DEX p1,p2;
	PT_3D tem;
	tem.z=z;
	double minr=r+1,t;
	CArray<PT_3DEX,PT_3DEX> pts;

	CGeometry *pObjs[2] = {pObj,NULL};

	BOOL bNeedRelease = FALSE;
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
	{
		if( !((CGeoParallel*)pObj)->Separate(pObjs[0],pObjs[1]) )
		{
			pObjs[0] = pObj;
			pObjs[1] = NULL;
		}
		else
		{
			bNeedRelease = TRUE;
		}
	}

	bool bFindOK = false;
	for( int k=0; k<2; k++)
	{
		pObj = pObjs[k];
		if( !pObj )continue;

		pObj->GetShape(pts);
		int num=pts.GetSize()-1;
		for (int i=0;i<num;i++)
		{
			p1 = pts.GetAt(i);
			p2 = pts.GetAt(i+1);
			double x,y;
			t=GraphAPI::GGetNearestDisOfPtToLine(p1.x,p1.y,p2.x,p2.y,pt.x,pt.y,&x,&y,false);
			if(z<min(p1.z,p2.z)||z>max(p1.z,p2.z)||t>r) continue;
			double dis;
			if (fabs(p2.z-p1.z)<1e-6)
			{
				if (fabs(z-p2.z)<1e-6)
				{
					dis=sqrt((x-pt.x)*(x-pt.x)+(y-pt.y)*(y-pt.y));
				}
				else
					continue;
				
			}
			else
			{
				x=(p2.x-p1.x)*(z-p1.z)/(p2.z-p1.z)+p1.x;
				y=(p2.y-p1.y)*(z-p1.z)/(p2.z-p1.z)+p1.y;
				dis=sqrt((x-pt.x)*(x-pt.x)+(y-pt.y)*(y-pt.y));
			}
			if (r>dis)
			{
				if(minr>dis)
				{
					minr=dis;
					tem.x=x;
					tem.y=y;
				}
				
			}
		}
		if (minr<r&&ret)
		{
			*ret=tem;
			bFindOK = true;
			break;
		}
	}

	if( bNeedRelease )
	{
		if( pObjs[0] )delete pObjs[0];
		if( pObjs[1] )delete pObjs[1];
	}

	return bFindOK;	
}


CGeometry* CReplaceLinesWithCatchIntersecCommand::GetReplaceObj()
{
	if( !m_pOldObj2 || m_pOldObj1==m_pOldObj2 )
		return GetReplaceObj_same();
	
	//获得各个点在基线上的最近点
	PT_3D ptStart, ptEnd;
	
	double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
	int start1=-1, end1=-1, start2=-1, end2=-1, i;
	PT_3D p1;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&p1);
	Envelope e ;
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptStart,NULL) )
	{
		start1 = -2;
	}
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&p1);
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj2->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptEnd,NULL) )
	{
		start2 = -2;
	}
	
	//两点都没有附着到地物上
	if( start1==-2 || start2==-2 )return NULL;
	
	CGeometry *pCurve1 = (CGeometry*)m_pOldObj1->GetGeometry(), *pCurve2 = (CGeometry*)m_pOldObj2->GetGeometry();
	
	 
	const CShapeLine  *pSL1 = pCurve1->GetShape();
	const CShapeLine  *pSL2 = pCurve2->GetShape();
	//得到关键点在基线中的对应序号
	CArray<int,int> KeyPos1 , KeyPos2 ;
	if(!pSL1->GetKeyPosOfBaseLines(KeyPos1)) return NULL;
	if(!pSL2->GetKeyPosOfBaseLines(KeyPos2)) return NULL;	
	
	int keynum1 = KeyPos1.GetSize();
	int keynum2 = KeyPos2.GetSize();
		
	CArray<PT_3DEX,PT_3DEX> pts1,pts2;
	if(!pSL1->GetPts(pts1))return NULL;
	int num1 = pts1.GetSize();
	if(!pSL2->GetPts(pts2))return NULL;
	int num2 = pts2.GetSize();
	if( num1<=1 || num2<=1 )
	{
		
		return NULL;
	}
	
	//获得上述各个最近点在基线中的位置
	PT_3D rpt2;
	double x,y,z;
	double dis,min1=-1,min2=-1, min3=-1;
	int i1=-1, i2=-1, i3=-1;
	
	//计算第一个地物
	
	for( i=0; i<num1-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts1[i].x,pts1[i].y,pts1[i].z,
			pts1[i+1].x,pts1[i+1].y,pts1[i+1].z,m_ptStart.x,m_ptStart.y,m_ptStart.z,&x,&y,&z,false);
		
		if( min2<0 || dis<min2 )
		{ 
			min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
			if( _FABS(x-pts1[i+1].x)+_FABS(y-pts1[i+1].y)>1e-10 )
				i2=i;
			//就是第二个点时，序号增加1
			else i2=i+1;
		}
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	
		double d=0;
		if( i2<num1 && i2>=0 )d = _FABS(pts1[i2].x-rpt2.x)+_FABS(pts1[i2].y-rpt2.y)+_FABS(pts1[i2].z-rpt2.z);
		
		if( d>1e-10 && i2<num1 && i2>=0 )
		{
			pts1.InsertAt(i2+1,PT_3DEX(rpt2,0));
			
			for( i=0; i<keynum1; i++)if( KeyPos1[i]>i2 )KeyPos1[i] = KeyPos1[i]+1;
			i2++; num1++;
		}
	}
	
	//判断哪一头比较长，比较长的保留
	{
		double d1 = pSL1->GetLength(&ptStart);
		double d2 = pSL1->GetLength()-d1;
		if( d1>=d2 )
		{
			start1 = -1;
			end1 = i2;
		}
		else
		{
			start1 = num1;
			end1 = i2;
		}
	}
	
	//计算第二个地物
	
	for( i=0; i<num2-1; i++)
	{
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts2[i].x,pts2[i].y,pts2[i].z,
			pts2[i+1].x,pts2[i+1].y,pts2[i+1].z,m_ptEnd.x,m_ptEnd.y,m_ptEnd.z,&x,&y,&z,false);
		
		if( min3<0 || dis<min3 )
		{ 
			min3 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
			if( _FABS(x-pts2[i+1].x)+_FABS(y-pts2[i+1].y)>1e-10 )
				i3=i;
			//就是第二个点时，序号增加1
			else i3=i+1;
		}
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	
		double d=0;
		if( i3<num2 && i3>=0 )d = _FABS(pts2[i3].x-rpt2.x)+_FABS(pts2[i3].y-rpt2.y)+_FABS(pts2[i3].z-rpt2.z);
		
		if( d>1e-10 && i3<num2 && i3>=0 )
		{
			pts2.InsertAt(i3+1,PT_3DEX(rpt2,0));
			
			for( i=0; i<keynum2; i++)if( KeyPos2[i]>i3 )KeyPos2[i] = KeyPos2[i]+1;
			i3++; num2++;
		}
	}
	
	//判断哪一头比较长，比较长的保留
	{
		double d1 = pSL2->GetLength(&ptEnd);
		double d2 = pSL2->GetLength()-d1;
		if( d1>=d2 )
		{
			start2 = -1;
			end2 = i3;
		}
		else
		{
			start2 = num2;
			end2 = i3;
		}
	}
	
	//计算各个位置最邻近的关键点
	int startKey1, endKey1,startKey2, endKey2;
	for( i=0; i<keynum1; i++)
	{
		if( start1<KeyPos1[i] )break;
	}
	
	startKey1 = i;
	
	for( i=0; i<keynum1; i++)
	{
		if( end1<KeyPos1[i] )break;
	}
	
	endKey1 = i;
	
	for( i=0; i<keynum2; i++)
	{
		if( start2<KeyPos2[i] )break;
	}
	
	startKey2 = i;
	
	for( i=0; i<keynum2; i++)
	{
		if( end2<KeyPos2[i] )break;
	}
	
	endKey2 = i;
	
	//准备新地物
	CGeometry *pNewObj = (CGeometry*)m_pOldObj1->GetGeometry()->Clone();
	if( !pNewObj )
	{		
		return NULL;
	}
// 	CDpDBVariant var;
// 	var = (long)0;
// 	pNewObj->GetBind()->SetAttrValue("EXCODE",var,DP_CFT_INTEGER);
// 	pNewObj->UpdateData(FALSE);
	

	
	PT_3DEX expt;
	CArray<PT_3DEX,PT_3DEX> arr;
	//首段
	pts1.RemoveAll();
	pNewObj->GetShape(pts1);
	if( start1>end1 )
	{
		for( i=keynum1-1; i>=endKey1; i--)
		{
			arr.Add(pts1.GetAt(i));		
		}
	}
	else
	{
		for( i=0; i<endKey1; i++)
		{
			arr.Add(pts1.GetAt(i));
		
		}
	}
	
	//替换段
	pts2.RemoveAll();
	m_pObj->GetShape(pts2);
//	int newnum = m_pObj->GetDataPointSum();
	arr.Append(pts2);
// 	for( i=0; i<newnum; i++)
// 	{	
// 		arr.Add(m_pObj->GetDataPoint(i));
// 	}
	
	//尾段
	pts2.RemoveAll();
	m_pOldObj2->GetGeometry()->GetShape(pts2);
	if( start2>end2 )
	{
		for( i=endKey2; i<keynum2; i++)
		{
			arr.Add(pts2.GetAt(i));
		}
	}
	else
	{
		for( i=endKey2-1; i>=0; i--)
		{
			arr.Add(pts2.GetAt(i));			
		}
	}
	
	//去除重复的点(主要因为起点、终点落在原对象节点上)
	PT_3DEX tt;
	keynum1 = arr.GetSize();
	int pos = 0;
	tt = arr.GetAt(pos);
	for( i=1; i<keynum1 && keynum1>2; )
	{
		expt = arr.GetAt(i);
		if( _FABS(expt.x-tt.x)<1e-4 && _FABS(expt.y-tt.y)<1e-4 && _FABS(expt.z-tt.z)<1e-4 )
		{
			arr.RemoveAt(i);
			keynum1--;
			continue;
		}
		else
		{
			pos++;
			tt = arr.GetAt(pos);
		}
		i++;
	}
	if(!pNewObj->CreateShape(arr.GetData(),arr.GetSize())) return NULL;
	return pNewObj;
}

CGeometry* CReplaceLinesWithCatchIntersecCommand::GetReplaceObj_same()
{
	if(!m_pOldObj1) return NULL;
	//获得各个点在基线上的最近点
	PT_3D ptStart, ptEnd;
	
	double r = m_nSnapRadius*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
	int select=-1, start=-1, end=-1, i;
	
	PT_3D p1;
	Envelope e;
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptStart,&p1);	
	e.CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptStart,NULL) )
	{
		start = -2;
	}
	m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptEnd,&p1);
	e .CreateFromPtAndRadius(p1,r);
	if( !m_pOldObj1->GetGeometry()->FindNearestBasePt(p1,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptEnd,NULL) )
	{
		end = -2;
	}
	
	//两点都没有附着到地物上
	if( start==-2 && end==-2 )return NULL;
	
	//如果是面，头尾点必须都附着在地物上
	if( m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		if( start==-2 || end==-2 )return NULL;
	}
	
	CGeometry *pCurve = (CGeometry*)m_pOldObj1->GetGeometry();
	
	int keynum = pCurve->GetDataPointSum();
	if( keynum<=1 )return NULL;
	
	const CShapeLine  *pSL = pCurve->GetShape();
	//得到关键点在基线中的对应序号
	CArray<int,int> KeyPos;
	if(!pSL->GetKeyPosOfBaseLines(KeyPos)) return NULL;
	
	CArray<PT_3DEX,PT_3DEX> pts;
	if(!pSL->GetPts(pts)) return NULL;

	int num = pts.GetSize();
	if( num<=1 )
	{
		return NULL;
	}
		
	//获得上述各个最近点在基线中的位置
	PT_3D rpt2,rpt3;
	double x,y,z;
	double dis,min2=-1, min3=-1;
	int i1=-1, i2=-1, i3=-1;
	
	for( i=0; i<num-1; i++)
	{
		//求出当前线段的最近距离
		if( start!=-2 )
		{
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i+1].x,pts[i+1].y,pts[i+1].z,m_ptStart.x,m_ptStart.y,m_ptStart.z,&x,&y,&z,false);
			
			if( min2<0 || dis<min2 )
			{ 
				min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
				if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
					i2=i;
				//就是第二个点时，序号增加1
				else i2=i+1;
			}
		}
		
		if( end!=-2 )
		{
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i+1].x,pts[i+1].y,pts[i+1].z,m_ptEnd.x,m_ptEnd.y,m_ptEnd.z,&x,&y,&z,false);
			
			if( min3<0 || dis<min3 )
			{ 
				min3 = dis; rpt3.x=x; rpt3.y=y; rpt3.z=z; 
				if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
					i3=i;
				//就是第二个点时，序号增加1
				else i3=i+1;
			}
		}
	}
	
	if( min2<0 && min3<0 )
	{	
		return NULL;
	}
	
	//(虚拟)插入上述最近点到基线位置表中; 
	{
	
		double d2=0, d3=0;
		if( i2<num && i2>=0 )d2 = _FABS(pts[i2].x-rpt2.x)+_FABS(pts[i2].y-rpt2.y)+_FABS(pts[i2].z-rpt2.z);
		if( i3<num && i3>=0 )d3 = _FABS(pts[i3].x-rpt3.x)+_FABS(pts[i3].y-rpt3.y)+_FABS(pts[i3].z-rpt3.z);
		
		if( d2>1e-10 && i2<num && i2>=0 )
		{
			pts.InsertAt(i2+1,PT_3DEX(rpt2,20));
		
			for( i=0; i<keynum; i++)if( KeyPos[i]>i2 )KeyPos[i] = KeyPos[i]+1;
			if( i2<i3 ||(i2==i3&&d2<=d3) )i3++;
			i2++; num++;
		}
		if( d3>1e-10 && i3<num && i3>=0 )
		{
			pts.InsertAt(i3+1,PT_3DEX(rpt3,20));
			for( i=0; i<keynum; i++)if( KeyPos[i]>i3 )KeyPos[i] = KeyPos[i]+1;
			if( i3<i2 ||(i3==i2&&d3<d2) )i2++;
			i3++; num++;
		}
	}
	
	//有个点在咬合范围外，就判断哪一头比较长，比较长的保留
	if( start==-2 || end==-2 )
	{
		double d1 = pSL->GetLength(&ptStart);
		double d2 = pSL->GetLength()-d1;
		if( d1>=d2 )
		{
			select = -1;
			start = (start==-2?num:i2);
			end = (end==-2?num:i3);
		}
		else
		{
			select = num;
			start = (start==-2?-1:i2);
			end = (end==-2?-1:i3);
		}
	}
	//两个点都在咬合范围内，
	else
	{
		//如果是闭合线或者是面，就判断哪一段比较长，比较长的保留
		if( m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) ||
			(m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)(m_pOldObj1->GetGeometry()))->IsClosed()) )
		{
			double d1 = fabs(pSL->GetLength(&ptEnd)-pSL->GetLength(&ptStart));
			double d2 = pSL->GetLength()-d1;
			//保留中间
			if( d1>d2 )
			{
				select = (i2+i3)/2;
				start  = i2;
				end    = i3;
			}
			//保留头尾
			else
			{
				select = -1;
				start  = i2;
				end    = i3;
			}
		}
		//否则替换中间的
		else
		{
			select = -1;
			start  = i2;
			end    = i3;
		}
	}
	
	//计算各个位置最邻近的关键点
	int startKey, endKey;
	for( i=0; i<keynum; i++)
	{
		if( start<KeyPos[i] )break;
	}
	
	startKey = i;
	
	for( i=0; i<keynum; i++)
	{
		if( end<KeyPos[i] )break;
	}
	
	endKey = i;
	
	//准备新地物
	CGeometry *pNewObj = (CGeometry*)m_pOldObj1->GetGeometry()->Clone();
	if( !pNewObj )
	{		
		return NULL;
	}	
	
	CArray<PT_3DEX,PT_3DEX> arr,pts1;
	PT_3DEX expt;
	//---------分如下情况生成新的地物
	//1. 选择点在起始点外
	if( select<=(start<end?start:end) || select>=(start>=end?start:end) )
	{
		if( startKey>endKey ){ int t=startKey; startKey=endKey; endKey=t; }
		
		//首段
		pNewObj->GetShape(pts1);
		for( i=0; i<startKey; i++)
		{		
			arr.Add(pts1.GetAt(i));
		}
		
		//防止expt具有无效的cd码
		if( startKey<=0 )expt = m_pObj->GetDataPoint(0);
		
		//替换段
		pts.RemoveAll();
		m_pObj->GetShape(pts);
		int newnum = pts.GetSize();
		if( start<=end )
		{
			arr.Append(pts);
// 			for( i=0; i<newnum; i++)
// 			{				
// 				arr.Add(m_pObj->GetDataPoint(i));
// 			}
		}
		else
		{
			for( i=newnum-1; i>=0; i--)
			{				
				arr.Add(pts.GetAt(i));
			}
		}
		
		//尾段
		for( i=endKey; i<keynum; i++)
		{		
			arr.Add(pts1.GetAt(i));
		}
	}
	//2. 选择点在起始点内
	else
	{
		if( startKey>endKey ){ int t=startKey; startKey=endKey; endKey=t; }
		
		//中间段
		expt = pNewObj->GetDataPoint(startKey);
		if( start<=end )COPY_3DPT(expt,m_ptStart);
			else COPY_3DPT(expt,m_ptEnd);
			arr.Add(expt);
		
		pNewObj->GetShape(pts1);
		for( i=startKey; i<endKey; i++)
		{			
			arr.Add(pts1.GetAt(i));
		}
		
		if( start<=end )COPY_3DPT(expt,m_ptEnd);
			else COPY_3DPT(expt,m_ptStart);
			arr.Add(expt);
		
		//替换段
		pts.RemoveAll();
		m_pObj->GetShape(pts);
		int newnum = pts.GetSize();
		if( start>end )
		{
			arr.Append(pts);
// 			for( i=0; i<newnum; i++)
// 			{			
// 				arr.Add(m_pObj->GetDataPoint(i));
// 			}
		}
		else
		{
			for( i=newnum-1; i>=0; i--)
			{				
				arr.Add(pts.GetAt(i));
			}
		}
	}
		
	
	//去除重复的点(主要因为起点、终点落在原对象节点上)
	PT_3DEX tt;
	keynum = arr.GetSize();
	int pos = 0;
	tt = arr.GetAt(pos);
	for( i=1; i<keynum && keynum>2; )
	{
		expt = arr.GetAt(i);
		if( _FABS(expt.x-tt.x)<1e-4 && _FABS(expt.y-tt.y)<1e-4 && _FABS(expt.z-tt.z)<1e-4 )
		{
			arr.RemoveAt(i);
			keynum--;
			continue;
		}
		else
		{
			pos++;
			tt = arr.GetAt(pos);
		}
		i++;
	}
	if (!pNewObj->CreateShape(arr.GetData(),arr.GetSize()))
	{
		return NULL;
	}
	return pNewObj;
}



//////////////////////////////////////////////////////////////////////
// CDrawRectCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawRectCommand,CDrawCommand)

CDrawRectCommand::CDrawRectCommand()
{
	m_nMode = modeCustom;
	m_pFtr = NULL;
	m_pSel = NULL;
	strcat(m_strRegPath,"\\Rect");

}

CDrawRectCommand::~CDrawRectCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if(m_pSel) delete m_pSel;
}



CString CDrawRectCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWRECT);
}

void CDrawRectCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	

	CDlgDataSource *pDS = GETDS(m_pEditor);			

	m_nMode = modeCustom;
	((CGeoCurve*)m_pFtr->GetGeometry())->EnableClose(TRUE);
	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);

	m_pEditor->CloseSelector();
		
}

void CDrawRectCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
void CDrawRectCommand::Finish()
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
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
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

DrawingInfo CDrawRectCommand::GetCurDrawingInfo()
{
	return DrawingInfo(m_pFtr,m_arrPts);
}

int  CDrawRectCommand::GetCurPenCode()
{
	return penLine;
}

void CDrawRectCommand::Abort()
{	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);

	m_pEditor->RefreshView();

	CDrawCommand::Abort();
}
  
void CDrawRectCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;

	GotoState(PROCSTATE_PROCESSING);

	PT_3DEX expt(pt,penLine);
	m_arrPts.Add(expt);
	
	if( m_nMode==modeHoriz )
	{
		if( m_arrPts.GetSize()==2 )
		{
			PT_3DEX pt0,pt1;
			pt0 = m_arrPts[0];
			pt1 = m_arrPts[1];
			
			expt.x = pt1.x;  expt.y = pt0.y; expt.z = pt0.z;
			m_arrPts.InsertAt(1, expt);			
			
			expt.x = pt0.x;  expt.y = pt1.y; expt.z = pt1.z;
			m_arrPts.Add(expt);
			m_arrPts.Add(m_arrPts.GetAt(0));
			if(!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize())) return;

			if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
			{
				Abort();
				return;
			}

			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
			undo.Commit();

			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pFtr = NULL;
			Finish();
			return;
		}
	}
	else if( m_nMode==modeCustom )
	{
		if (m_arrPts.GetSize()==2)
		{
			GrBuffer buf;
			buf.BeginLineString(0,0);
			buf.MoveTo(&m_arrPts[0]);
			buf.LineTo(&m_arrPts[1]);
			buf.End();
			buf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
			m_pEditor->UpdateDrag(ud_AddConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		}
		if( m_arrPts.GetSize()==3 )
		{
			PT_3DEX pt0,pt1,pt2,pt3;
			pt0 = m_arrPts[0];
			pt1 = m_arrPts[1];
			pt2 = m_arrPts[2];
			
			//根据垂直关系计算第三点和第四点
			GraphAPI::GGetRightAnglePoint(pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,&pt2.x,&pt2.y);
			pt3.x = pt2.x-pt1.x+pt0.x;  pt3.y = pt2.y-pt1.y+pt0.y; pt3.z = pt2.z;
			pt3.pencode= pt2.pencode;
			
			m_arrPts.SetAt(2, pt2);
			m_arrPts.Add(pt3);
			m_arrPts.Add(m_arrPts.GetAt(0));
			if(!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize())) return;

			if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
			{
				Abort();
				return;
			}

			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
			undo.Commit();

			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pFtr = NULL;

			Finish();
			return;
		}
	}
	
	if (PDOC(m_pEditor)->IsAutoSetAnchor() && m_pFtr->GetGeometry() 
		&& (m_pFtr->GetGeometry()->GetDataPointSum() == 1))
	{
		PDOC(m_pEditor)->SetAnchorPoint(pt);
	}

	CDrawCommand::PtClick(pt,flag);
}

void CDrawRectCommand::PtMove(PT_3D &pt)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;

	PT_3DEX expt(pt,penLine);
	m_arrPts.Add(expt);	
	
	if( m_nMode==modeHoriz )
	{
		GrBuffer buf;
		
		if( m_arrPts.GetSize()==2 )
		{
			buf.BeginLineString(0,0);
			PT_3DEX pt0,pt1;
			pt0 = m_arrPts[0];
			pt1 = m_arrPts[1];
			
			PT_3D t;
			COPY_3DPT(t,pt0);
			buf.MoveTo(&t);
			
			t.x = pt1.x;  t.y = pt0.y; t.z = pt0.z;
			buf.LineTo(&t);
			
			COPY_3DPT(t,pt1);
			buf.LineTo(&t);
			
			t.x = pt0.x;  t.y = pt1.y; t.z = pt0.z;
			buf.LineTo(&t);
			
			COPY_3DPT(t,pt0);
			buf.LineTo(&t);			
			buf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		}
// 		else
// 		{
// 			buf.BeginLineString(0,0);
// 			buf.MoveTo()
// 		}
		
		
	
	}
	else if( m_nMode==modeCustom )
	{
		if (m_arrPts.GetSize()==2)
		{
			GrBuffer buf;
			PT_3DEX pt0,pt1;
			pt0 = m_arrPts[0];
			pt1 = m_arrPts[1];
			buf.BeginLineString(0,0);
			buf.MoveTo(&pt0);
			buf.LineTo(&pt1);
			buf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);


		}
		if( m_arrPts.GetSize()==3 )
		{
			GrBuffer buf;
			PT_3DEX pt0,pt1,pt2,pt3;
			pt0 = m_arrPts[0];
			pt1 = m_arrPts[1];
			pt2 = m_arrPts[2];
			
			//根据垂直关系计算第三点和第四点
			GraphAPI::GGetRightAnglePoint(pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,&pt2.x,&pt2.y);
			pt3.x = pt2.x-pt1.x+pt0.x;  pt3.y = pt2.y-pt1.y+pt0.y; pt3.z = pt2.z;
			buf.BeginLineString(0,0);
			buf.MoveTo(&pt1);
			buf.LineTo(&pt2);
			buf.LineTo(&pt3);
			buf.LineTo(&pt0);
			buf.End();
			
		//	buf.RefreshEnvelope();
			
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
			
		}
	}
	m_arrPts.RemoveAt(m_arrPts.GetSize()-1);
	CDrawCommand::PtMove(pt);
}


void CDrawRectCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);
	_variant_t var;
	var = (_variant_t)(long)(m_nMode);
	tab.AddValue(PF_DRAWRECTWAY,&CVariantEx(var));
}


void CDrawRectCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("DrawRectCommand",StrFromLocalResID(IDS_CMDNAME_DRAW));
	param->BeginOptionParam(PF_DRAWRECTWAY,StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_HRECT),modeHoriz,' ',m_nMode==modeHoriz);
	param->AddOption(StrFromResID(IDS_CMDPLANE_ARECT),modeCustom,' ',m_nMode==modeCustom);
	param->EndOptionParam();
}


void CDrawRectCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_DRAWRECTWAY,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CDrawCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CDrawRegularPolygonCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawRegularPolygonCommand,CDrawCommand)

CDrawRegularPolygonCommand::CDrawRegularPolygonCommand()
{
	m_nSides = 3;
	m_pFtr = NULL;
	m_pSel = NULL;
	strcat(m_strRegPath,"\\Polygon");

}

CDrawRegularPolygonCommand::~CDrawRegularPolygonCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if(m_pSel) delete m_pSel;
}



CString CDrawRegularPolygonCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWPOLYGON);
}

void CDrawRegularPolygonCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	
	m_nSides = 3;
	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);

	m_pEditor->CloseSelector();
		
}

void CDrawRegularPolygonCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
void CDrawRegularPolygonCommand::Finish()
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
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
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

DrawingInfo CDrawRegularPolygonCommand::GetCurDrawingInfo()
{
	return DrawingInfo(m_pFtr,m_arrPts);
}

int  CDrawRegularPolygonCommand::GetCurPenCode()
{
	return penLine;
}

void CDrawRegularPolygonCommand::Abort()
{	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);

	m_pEditor->RefreshView();

	CDrawCommand::Abort();
}



template<class T>
static void polygon(const T &pt0, const T &pt1, const T &ptDir, int sides, CArray<T,T> &pts)
{
	if( sides < 3 || fabs(pt0.x-pt1.x)+fabs(pt0.y-pt1.y)+fabs(pt0.z-pt1.z) < 1e-4)
	  return;

	T arr[3];
	arr[0] = pt0;
	arr[1] = pt1;
	arr[2] = ptDir;

	bool dir = (GraphAPI::GIsClockwise(arr,3)==1);
	 
	double ang = 2*PI/sides;
	
	// 中点
	T cpt;
	cpt.x = (pt0.x+pt1.x)/2;
	cpt.y = (pt0.y+pt1.y)/2;
//	cpt.z = (pt0.z+pt1.z)/2;
	
	double dx, dy, len;
	dx = pt1.x - pt0.x;
	dy = pt1.y - pt0.y;
//	dz = pt1.z - pt0.z;

	len = sqrt(dx*dx+dy*dy/*+dz*dz*/);
	
	// 半径
	double r = (len/2)/sin(ang/2);
	
	// 圆心
	T point;

	double h = sqrt( r*r-(len/2)*(len/2) );
	double tdx, tdy;
	tdx = -h*dy/len;
	tdy = h*dx/len;
	
	if (dir)
	{
		point.x = cpt.x-tdx;
		point.y = cpt.y-tdy;
//		point.z = cpt.z;
	}
	else
	{
		point.x = cpt.x+tdx;
		point.y = cpt.y+tdy;
//		point.z = cpt.z;
	}
	
	// 沿圆心旋转sides-1次
	T retpt;
	double dx1,dy1,sina,cosa,lfAngle;

	pts.Add(pt0);

	lfAngle = ang;
	
	dx1 = pt0.x - point.x;
	dy1 = pt0.y - point.y;
	
	for (int i=1; i<sides; i++)
	{
		sina = sin(lfAngle*i);
		cosa = cos(lfAngle*i);		
		
		retpt.x = dx1 * cosa - dy1 * sina  + point.x;
		retpt.y = dy1 * cosa + dx1 * sina  + point.y;
//		retpt.z = point.z;

		GraphAPI::GetHeightFromSurface(pt0,pt1,ptDir,&retpt,1);

		pts.Add(retpt);
	}
	
}

void CDrawRegularPolygonCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;

	if (m_nSides < 3)
	{
		AfxMessageBox(IDS_PARAM_ERROR);
		return;
	}

	GotoState(PROCSTATE_PROCESSING);

	PT_3DEX expt(pt,penLine);
	m_arrPts.Add(expt);
	
	int size = m_arrPts.GetSize();
	if (size == 2)
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_arrPts[0]);
		buf.LineTo(&m_arrPts[1]);
		buf.End();
		buf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
		m_pEditor->UpdateDrag(ud_AddConstDrag,&buf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	}
	else if (size == 3)
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		polygon(m_arrPts[0],m_arrPts[1],m_arrPts[2],m_nSides,pts);
		int size = pts.GetSize();
		if (size > 1)
		{
			pts.Add(pts[0]);
		}
		else
		{
			Abort();
			return;
		}
			
		for (int i=0; i<pts.GetSize(); i++)
		{
			pts[i].pencode = penLine;
		}
		if(!m_pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize())) return;
		
		if( !AddObject(m_pFtr) )
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor,Name());
		undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
		undo.Commit();

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pFtr = NULL;
		Finish();
		return;
	}
	
	if (PDOC(m_pEditor)->IsAutoSetAnchor() && m_pFtr && m_pFtr->GetGeometry() 
		&& size == 1)
	{
		PDOC(m_pEditor)->SetAnchorPoint(pt);
	}

	CDrawCommand::PtClick(pt,flag);
}

void CDrawRegularPolygonCommand::PtMove(PT_3D &pt)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;	
	
	if( m_arrPts.GetSize()==1 )
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_arrPts[0]);
		buf.LineTo(&pt);			
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}

	CDrawCommand::PtMove(pt);
}


void CDrawRegularPolygonCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);
	_variant_t var;
	var = (_variant_t)(long)(m_nSides);
	tab.AddValue(PF_POLYGONSIDESNUM,&CVariantEx(var));
}


void CDrawRegularPolygonCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("DrawPolygonCommand",StrFromLocalResID(IDS_CMDNAME_DRAWPOLYGON));
	param->AddParam(PF_POLYGONSIDESNUM,int(m_nSides),StrFromResID(IDS_CMDPLANE_SIDES));
}


void CDrawRegularPolygonCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_POLYGONSIDESNUM,var) )
	{
		m_nSides = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CDrawCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CDrawCircleCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawCircleCommand,CDrawCommand)

CDrawCircleCommand::CDrawCircleCommand()
{
	m_nMode = -1;
	m_pFtr = NULL;
	m_pSel = NULL;
	m_bMouseDefineRadius = TRUE;
	m_fRadius = 10;
}

CDrawCircleCommand::~CDrawCircleCommand()
{
	m_nMode = -1;
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if (m_pSel)
	{
		delete m_pSel;
	}
}



CString CDrawCircleCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWCIRCLE);
}

void CDrawCircleCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	
/*
	CDlgDataSource *pDS = GETDS(m_pEditor);			
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (pScheme && pLayer)
	{
		float wid = pScheme->GetLayerDefineLineWidth(pLayer->GetName());
		((CGeoCurve*)m_pFtr->GetGeometry())->m_fLineWidth = wid;
	}
*/
	//((CGeoCurve*)m_pFtr->GetGeometry())->EnableClose(TRUE);	
	CDrawCommand::Start();
	
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
	m_pEditor->CloseSelector();
//	CDrawCommand::Start();	
}

void CDrawCircleCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
DrawingInfo CDrawCircleCommand::GetCurDrawingInfo()
{
	return DrawingInfo(m_pFtr,m_arrPts);
}

int  CDrawCircleCommand::GetCurPenCode()
{
	return penArc;
}

void CDrawCircleCommand::Abort()
{	
	if(m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);

	CDrawCommand::Abort();
}

void CDrawCircleCommand::Finish()
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
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
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

void CDrawCircleCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;

	GotoState(PROCSTATE_PROCESSING);

	// 二点圆
	if (m_nMode == 0)
	{
		PT_3DEX expt(pt,penArc);
		m_arrPts.Add(expt);
		
		if (m_arrPts.GetSize() == 2)
		{
			PT_3DEX cen;
			cen.x = (m_arrPts[0].x + m_arrPts[1].x)/2;
			cen.y = (m_arrPts[0].y + m_arrPts[1].y)/2;
			cen.z = (m_arrPts[0].z + m_arrPts[1].z)/2;
			cen.pencode = penArc;

			CArray<PT_3DEX,PT_3DEX> arrRotate;
			arrRotate.Copy(m_arrPts);
			GraphAPI::GRotate2DPT(cen,PI/2,arrRotate.GetData(),arrRotate.GetSize());

			arrRotate[0].z = arrRotate[1].z = cen.z;
			m_arrPts.InsertAt(1,arrRotate[0]);
			m_arrPts.Add(arrRotate[1]);
			m_arrPts.Add(m_arrPts[0]);

			if(!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize())) return;
			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr));	
			if( !AddObject(m_pFtr) )
				m_pEditor->UpdateDrag(ud_ClearDrag);
			undo.Commit();
			Finish();
		}
	}
	// 三点圆
	else if (m_nMode == 1)
	{
		PT_3DEX expt(pt,penArc);
		m_arrPts.Add(expt);
		if(!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize())) return;
		if (m_arrPts.GetSize()>=3)
		{
			m_arrPts.Add(m_arrPts.GetAt(0));
			if(!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize())) return;
			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr));	
			if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
				m_pEditor->UpdateDrag(ud_ClearDrag);
			undo.Commit();
			Finish();
			return;
		}
		GrBuffer buf;
		m_pFtr->GetGeometry()->GetShape()->GetAddConstGrBuffer(&buf);
		m_pEditor->UpdateDrag(ud_AddConstDrag,&buf);
		m_pEditor->UpdateDrag(ud_ClearDrag);
	}
	// 圆心,半径
	else if (m_nMode == 2)
	{
		PT_3DEX expt(pt,penArc);
		m_arrPts.Add(expt);

		int num = m_arrPts.GetSize();
		if (num >= 1)
		{
			if (num == 2 && m_bMouseDefineRadius)
			{
				PT_3DEX pt1, cenPt = m_arrPts[0];
				pt1.x = 2*m_arrPts[0].x - m_arrPts[1].x;
				pt1.y = 2*m_arrPts[0].y - m_arrPts[1].y;
				pt1.z = 2*m_arrPts[0].z - m_arrPts[1].z;
				pt1.pencode = penArc;
				
				m_arrPts.RemoveAt(0);
				m_arrPts.InsertAt(0,pt1);
				
				CArray<PT_3DEX,PT_3DEX> arrRotate;
				arrRotate.Copy(m_arrPts);
				GraphAPI::GRotate2DPT(cenPt,PI/2,arrRotate.GetData(),arrRotate.GetSize());
				
				arrRotate[0].z = arrRotate[1].z = m_arrPts[0].z;
				
				
				m_arrPts.InsertAt(1,arrRotate[0]);
				m_arrPts.Add(arrRotate[1]);
				m_arrPts.Add(m_arrPts[0]);
			}
			else if (num == 1 && !m_bMouseDefineRadius)
			{
				PT_3DEX cen = m_arrPts[0];
				m_arrPts.RemoveAll();

				double off = m_fRadius/sqrt(2);
				m_arrPts.Add(PT_3DEX(cen.x-off,cen.y-off,cen.z,penArc));
				m_arrPts.Add(PT_3DEX(cen.x+off,cen.y-off,cen.z,penArc));
				m_arrPts.Add(PT_3DEX(cen.x+off,cen.y+off,cen.z,penArc));
				m_arrPts.Add(PT_3DEX(cen.x-off,cen.y+off,cen.z,penArc));
				m_arrPts.Add(m_arrPts[0]);

			}
			
			if(!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize())) return;
			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr));	
			if( !AddObject(m_pFtr) )
				m_pEditor->UpdateDrag(ud_ClearDrag);
			undo.Commit();
			Finish();
		}
	}

	CDrawCommand::PtClick(pt,flag);
}

void CDrawCircleCommand::PtMove(PT_3D &pt)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;

	int size = m_arrPts.GetSize();
	if (size<=0)
	{
		return;
	}

	CArray<PT_3DEX,PT_3DEX> arrMovePts;
	arrMovePts.Copy(m_arrPts);

	if (m_nMode == 0 && size == 1)
	{
		PT_3DEX expt(pt,penArc);
		arrMovePts.Add(expt);
		
		PT_3DEX cen;
		cen.x = (arrMovePts[0].x + arrMovePts[1].x)/2;
		cen.y = (arrMovePts[0].y + arrMovePts[1].y)/2;
		cen.z = (arrMovePts[0].z + arrMovePts[1].z)/2;
		cen.pencode = penArc;
		
		CArray<PT_3DEX,PT_3DEX> arrRotate;
		arrRotate.Copy(arrMovePts);
		GraphAPI::GRotate2DPT(cen,PI/2,arrRotate.GetData(),arrRotate.GetSize());
		
		arrRotate[0].z = arrRotate[1].z = cen.z;
		arrMovePts.InsertAt(1,arrRotate[0]);
		arrMovePts.Add(arrRotate[1]);
		arrMovePts.Add(m_arrPts[0]);
		
		if( m_pFtr->GetGeometry()->CreateShape(arrMovePts.GetData(),arrMovePts.GetSize()) )
		{
			GrBuffer vbuf;
			m_pFtr->Draw(&vbuf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			vbuf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		}

	}
	else if (m_nMode == 1 && size == 2)
	{
		PT_3DEX expt(pt,penArc);
		arrMovePts.Add(expt);	
		arrMovePts.Add(arrMovePts.GetAt(0));	
		
		if( m_pFtr->GetGeometry()->CreateShape(arrMovePts.GetData(),arrMovePts.GetSize()) )
		{
			GrBuffer vbuf;
			
			const CShapeLine *pSL = m_pFtr->GetGeometry()->GetShape();
			
			pSL->GetVariantGrBuffer(&vbuf,true,arrMovePts.GetSize()-2);
			
			vbuf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		}
	}
	else if (m_nMode == 2 && size == 1)
	{
		PT_3DEX expt(pt,penArc);
		arrMovePts.Add(expt);

		if (m_bMouseDefineRadius)
		{
			PT_3DEX pt1, cenPt = m_arrPts[0];
			pt1.x = 2*arrMovePts[0].x - arrMovePts[1].x;
			pt1.y = 2*arrMovePts[0].y - arrMovePts[1].y;
			pt1.z = 2*arrMovePts[0].z - arrMovePts[1].z;
			pt1.pencode = penArc;
			
			arrMovePts.RemoveAt(0);
			arrMovePts.InsertAt(0,pt1);
			
			CArray<PT_3DEX,PT_3DEX> arrRotate;
			arrRotate.Copy(arrMovePts);
			GraphAPI::GRotate2DPT(cenPt,PI/2,arrRotate.GetData(),arrRotate.GetSize());
			
			arrRotate[0].z = arrRotate[1].z = cenPt.z;
			arrMovePts.InsertAt(1,arrRotate[0]);
			arrMovePts.Add(arrRotate[1]);
			arrMovePts.Add(arrMovePts[0]);
		}
		
		if( m_pFtr->GetGeometry()->CreateShape(arrMovePts.GetData(),arrMovePts.GetSize()) )
		{
			GrBuffer vbuf;
			m_pFtr->Draw(&vbuf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			vbuf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		}
	}
	

	CDrawCommand::PtMove(pt);
}


void CDrawCircleCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);

	_variant_t var;
	var = (bool)(m_bMouseDefineRadius);
	tab.AddValue(PF_MOUSEDEFINE,&CVariantEx(var));

	var = (float)m_fRadius;
	tab.AddValue(PF_RADIUS,&CVariantEx(var));
	
}


void CDrawCircleCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("DrawCircle",StrFromLocalResID(IDS_CMDNAME_CICLECENTERRADIUS));
	
	
	if (m_nMode == 2 || bForLoad)
	{
		param->BeginOptionParam(PF_MOUSEDEFINE,StrFromResID(IDS_CMDTIP_MOUSEDEF_RADIUS));
		param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMouseDefineRadius);
		param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMouseDefineRadius);
		param->EndOptionParam();
		
		if (!m_bMouseDefineRadius || bForLoad)
		{
			param->AddParam(PF_RADIUS,m_fRadius,StrFromResID(IDS_RADIUS));
		}
		
	}
	
}


void CDrawCircleCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_MOUSEDEFINE,var) )
	{
		m_bMouseDefineRadius = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if ( tab.GetValue(0,PF_RADIUS,var))
	{
		m_fRadius = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CDrawCommand::SetParams(tab,bInit);
}

IMPLEMENT_DYNCREATE(CDrawCircleTwoPtCommand,CDrawCircleCommand)

CDrawCircleTwoPtCommand::CDrawCircleTwoPtCommand()
{
	m_nMode = 0;
}

CDrawCircleTwoPtCommand::~CDrawCircleTwoPtCommand()
{

}

CString CDrawCircleTwoPtCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CICLETW0PT);
}

IMPLEMENT_DYNCREATE(CDrawCircleThreePtCommand,CDrawCircleCommand)

CDrawCircleThreePtCommand::CDrawCircleThreePtCommand()
{
	m_nMode = 1;
}

CDrawCircleThreePtCommand::~CDrawCircleThreePtCommand()
{
	
}

CString CDrawCircleThreePtCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CICLETHREEPT);
}

IMPLEMENT_DYNCREATE(CDrawCircleCenterRadiusCommand,CDrawCircleCommand)

CDrawCircleCenterRadiusCommand::CDrawCircleCenterRadiusCommand()
{
	m_nMode = 2;
}

CDrawCircleCenterRadiusCommand::~CDrawCircleCenterRadiusCommand()
{
	
}

CString CDrawCircleCenterRadiusCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CICLECENTERRADIUS);
}
//////////////////////////////////////////////////////////////////////
// CDrawLinesBySide Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawLinesBySide,CDrawCommand)

CDrawLinesBySide::CDrawLinesBySide()
{
	m_pFtr = NULL;
	m_pSel = NULL;
	m_bOpenIfSnapped = TRUE;
}

CDrawLinesBySide::~CDrawLinesBySide()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if (m_pSel)
	{
		delete m_pSel;
	}
}



CString CDrawLinesBySide::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWLINESBYSIDE);
}

void CDrawLinesBySide::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	

	CDlgDataSource *pDS = GETDS(m_pEditor);			

	CDrawCommand::Start();
	((CGeoCurve*)m_pFtr->GetGeometry())->EnableClose(TRUE);
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();	
	m_arrPts.RemoveAll();
	m_pEditor->CloseSelector();
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}

void CDrawLinesBySide::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
DrawingInfo CDrawLinesBySide::GetCurDrawingInfo()
{
	return DrawingInfo(m_pFtr,m_arrPts);
}

void CDrawLinesBySide::Back()
{
	if (!m_pFtr)
	{
		return;
	}
	int num = m_arrPts.GetSize();
	if( num>=1 )
	{
		PT_3D pt;
		pt = m_arrPts.GetAt(num-1);			

		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt);
		
		m_arrPts.RemoveAt(num-1);
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.Copy(m_arrPts);
		arrPts.Add(PT_3DEX(pt,penLine));
		
		BOOL finished = FALSE;
		CFeature *pObj = CreateObjBySide(arrPts,finished);
		if( !pObj )
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		else
		{
			GrBuffer vbuf;
			pObj->Draw(&vbuf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			delete pObj;
		}
		m_pEditor->RefreshView();
	}
	else
	{
		m_arrPts.RemoveAll();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	}
	
	CDrawCommand::Back();	
}


void CDrawLinesBySide::Abort()
{	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->RefreshView();
	if(m_pFtr)
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
	CDrawCommand::Abort();
}
 
BOOL CDrawLinesBySide::IsEndSnapped()
{
	if (!m_pEditor)
		return FALSE;

	if (!PDOC(m_pEditor)->m_snap.bOpen())
		return FALSE;

	SNAPITEM item = PDOC(m_pEditor)->m_snap.GetFirstSnapResult();
	if (!item.IsValid())
		return FALSE;

	if (item.nSnapMode == CSnap::modeNearPoint ||
		item.nSnapMode == CSnap::modeKeyPoint ||
		item.nSnapMode == CSnap::modeMidPoint ||
		item.nSnapMode == CSnap::modeIntersect ||
		item.nSnapMode == CSnap::modePerpPoint ||
		item.nSnapMode == CSnap::modeTangPoint ||
		item.nSnapMode == CSnap::modeEndPoint)
		return TRUE;

	return FALSE;
}

CFeature *CDrawLinesBySide::CreateObjBySide(CArray<PT_3DEX,PT_3DEX>& pts, BOOL& finished)
{
//	CFtrLayer* pLayer = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayer();
	CFtrLayer* pLayer = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetCurFtrLayer();
	//待改
//	CFeature *pFtr = pLayer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale());
	CFeature *pFtr = m_pFtr->Clone();//new CFeature;

	if (!pFtr)
	{
		return NULL;
	}
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return NULL;

	int num = pts.GetSize();

	BOOL bClosed = TRUE;
	if (finished)//结束生成时
	{
		if (m_bOpenIfSnapped && IsEndSnapped())
		{
			if (num < 3)
			{
				finished = FALSE;
				return NULL;
			}
			bClosed = FALSE;
		}
		else//非端点捕捉，点数至少5且是奇数才能生成地物
		{
			if (num<5 || num%2==0)
			{
				finished = FALSE;
				return NULL;
			}
			bClosed = TRUE;
		}
	}
	
	CArray<PT_3DEX,PT_3DEX> arr;
	if( finished )
	{		
		PT_3DEX expt;
		expt.pencode = penLine;
		const PT_3DEX *buf = pts.GetData();
		PT_3DEX lastpt = buf[0];
		
		//加中间的边相交点
		for( int i=2; i<num; i++)
		{
			GraphAPI::GGetPerpendicular3D(lastpt.x,lastpt.y,lastpt.z,
				buf[i-1].x,buf[i-1].y,buf[i-1].z,
				buf[i].x,buf[i].y,buf[i].z,
				&expt.x,&expt.y,&expt.z);
			
			arr.Add(expt);
			lastpt = expt;
		}
		
		if (bClosed)
		{
			//加首点
			GraphAPI::GGetPerpendicular3D(buf[0].x, buf[0].y, buf[0].z,
				buf[1].x, buf[1].y, buf[1].z,
				buf[num - 1].x, buf[num - 1].y, buf[num - 1].z,
				&expt.x, &expt.y, &expt.z);

			arr.Add(expt);
			//闭合
			arr.Add(arr[0]);
		}
		else
		{
			//加首点
			arr.InsertAt(0, m_arrPts[0]);
			//加尾点
			arr.Add(m_arrPts[m_arrPts.GetSize() - 1]);
		}
	}
	else if( num>=2 )
	{
		PT_3DEX expt;
		expt.pencode = penLine;
		
		const PT_3DEX *buf = pts.GetData();
		PT_3DEX lastpt = buf[0];
		
		//加首点
		COPY_3DPT(expt,buf[0]);
		arr.Add(expt);
		
		//加中间的边相交点
		for( int i=2; i<num; i++)
		{
			GraphAPI::GGetPerpendicular3D(lastpt.x,lastpt.y,lastpt.z,
				buf[i-1].x,buf[i-1].y,buf[i-1].z,
				buf[i].x,buf[i].y,buf[i].z,
				&expt.x,&expt.y,&expt.z);
			
			arr.Add(expt);
			lastpt = expt;
		}
		
		//加尾点
		COPY_3DPT(expt,buf[num-1]);
		arr.Add(expt);
		
		// 延长距离
		double len;
		PT_4D pt4d(arr[0].x,arr[0].y,arr[0].z);	
		m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(&pt4d,1);
		pt4d.x += 400;
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(&pt4d,1);
		len = GraphAPI::GGet3DDisOf2P(PT_3D(arr[0].x,arr[0].y,arr[0].z),PT_3D(pt4d.x,pt4d.y,pt4d.z));
		
		//将首点和尾点都延长400像素，模拟射线		
		PT_3DEX expt1, expt2;
		expt1 = arr.GetAt(0);
		expt2 = arr.GetAt(1);
		double len1 = GraphAPI::GGet3DDisOf2P(expt1,expt2);
		if (len1 <= GraphAPI::GetDisTolerance())
		{
			len1 = 1;
		}
		expt.x = expt1.x + (expt1.x-expt2.x)/len1*len;
		expt.y = expt1.y + (expt1.y-expt2.y)/len1*len;
		expt.z = expt1.z + (expt1.z-expt2.z)/len1*len;
		arr.SetAt(0,expt);
		
		if( num>2 )
		{
			expt1 = arr.GetAt(num-2);
			expt2 = arr.GetAt(num-1);
			len1 = GraphAPI::GGet3DDisOf2P(expt1,expt2);
			if (len1 <= GraphAPI::GetDisTolerance())
			{
				len1 = 1;
			}
		}
		expt.x = expt2.x + (expt2.x-expt1.x)/len1*len;
		expt.y = expt2.y + (expt2.y-expt1.y)/len1*len;
		expt.z = expt2.z + (expt2.z-expt1.z)/len1*len;
		arr.SetAt(num-1,expt);
	}
	pObj->CreateShape(arr.GetData(),arr.GetSize());
	return pFtr;
}


void CDrawLinesBySide::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;	
	
	GotoState(PROCSTATE_PROCESSING);
	
	int ntmppt = m_arrPts.GetSize();
	if( ntmppt>0 )GotoState(PROCSTATE_PROCESSING);
	if( ntmppt==1 && ((CDlgDoc*)m_pEditor)->IsAutoSetAnchor() )
	{
		((CDlgDoc*)m_pEditor)->SetAnchorPoint(pt);
	}
	m_arrPts.Add(PT_3DEX(pt,penLine));

	CDrawCommand::PtClick(pt,flag);
}

void CDrawLinesBySide::PtReset(PT_3D &pt)
{
	BOOL finished = TRUE;
	CFeature  *pFtr = CreateObjBySide(m_arrPts,finished);
	if( finished && pFtr )
	{		
		if( !AddObject(pFtr) )
		{
			Abort();
			return;
		}
		CUndoFtrs undo(m_pEditor,Name());
		undo.arrNewHandles.Add(FtrToHandle(pFtr));
		undo.Commit();
	}
	else if( pFtr )delete pFtr;
	
//	m_pEditor->SetCurDrawingObj(DrawingInfo());
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	//m_pDoc->UpdateAllViews(NULL,hc_Detach_Accubox);
	m_pEditor->RefreshView();
	
	m_arrPts.RemoveAll();
	
	if( !finished )
		Abort();
	else
	{
		if( m_pFtr!=NULL )
			delete m_pFtr;
		m_pFtr = NULL;
		Finish();
	}

}


void CDrawLinesBySide::PtMove(PT_3D &pt)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;
	
	if( m_arrPts.GetSize()<=0 )return;
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	arrPts.Copy(m_arrPts);
	arrPts.Add(PT_3DEX(pt,penLine));
	
	BOOL finished = FALSE;
	CFeature *pFtr = CreateObjBySide(arrPts,finished);
	if( !pFtr )
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	else
	{
		GrBuffer vbuf;
		pFtr->GetGeometry()->Draw(&vbuf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		delete pFtr;
	}
	
	CDrawCommand::PtMove(pt);
}


void CDrawLinesBySide::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);

	_variant_t var;
	var = (bool)(m_bOpenIfSnapped);
	tab.AddValue(PF_OPENIFSNAP, &CVariantEx(var));
}


void CDrawLinesBySide::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("DrawLinesBySide", Name());
	if (m_pFtr)
	{
		param->BeginOptionParam(PF_OPENIFSNAP, StrFromResID(IDS_CMDPLANE_OPENIFSNAP));
		param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_bOpenIfSnapped);
		param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_bOpenIfSnapped);
		param->EndOptionParam();
	}
}

void CDrawLinesBySide::Finish()
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
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
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

void CDrawLinesBySide::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_OPENIFSNAP, var))
	{
		m_bOpenIfSnapped = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CDrawCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CDrawLinesByMultiPt Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawLinesByMultiPt,CDrawCommand)

CDrawLinesByMultiPt::CDrawLinesByMultiPt()
{
	m_pFtr = NULL;
	m_pSel = NULL;
	m_bLastRClick = FALSE;
}

CDrawLinesByMultiPt::~CDrawLinesByMultiPt()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if (m_pSel)
	{
		delete m_pSel;
	}
}



CString CDrawLinesByMultiPt::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWLINESBYMULTIPT);
}

void CDrawLinesByMultiPt::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	
	m_bLastRClick = FALSE;
	m_arrBuildLine.RemoveAll();
	CDrawCommand::Start();
	((CGeoCurve*)m_pFtr->GetGeometry())->EnableClose(TRUE);
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();	
	m_arrPts.RemoveAll();
	m_pEditor->CloseSelector();
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}

void CDrawLinesByMultiPt::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
DrawingInfo CDrawLinesByMultiPt::GetCurDrawingInfo()
{
	return DrawingInfo(m_pFtr,m_arrPts);
}

void CDrawLinesByMultiPt::Back()
{
	if (!m_pFtr)
	{
		return;
	}
	int num = m_arrPts.GetSize();
	if( num>=1 )
	{
		PT_3D pt;
		pt = m_arrPts.GetAt(num-1);			

		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt);
		
		m_arrPts.RemoveAt(num-1);
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.Copy(m_arrPts);
		arrPts.Add(PT_3DEX(pt,penLine));
		
		BOOL finished = FALSE;
		CFeature *pObj = CreateObjBySide(finished);
		if( !pObj )
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		else
		{
			GrBuffer vbuf;
			pObj->Draw(&vbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			delete pObj;
		}
		m_pEditor->RefreshView();
	}
	else
	{
		m_arrPts.RemoveAll();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	}
	
	CDrawCommand::Back();	
}


void CDrawLinesByMultiPt::Abort()
{	
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->RefreshView();
	if(m_pFtr)
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
	CDrawCommand::Abort();
}
  

CFeature *CDrawLinesByMultiPt::CreateObjBySide(BOOL& finished)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	pts.Copy(m_arrPts);
	
	int num = pts.GetSize();
	if (num < 2) return NULL;

	CFtrLayer* pLayer = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetCurFtrLayer();

	CFeature *pFtr = m_pFtr->Clone();
	if (!pFtr)   return NULL;

	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return NULL;

	CArray<PT_3DEX,PT_3DEX> arr;
	if( finished )
	{
		PT_3DEX expt;
		expt.pencode = penLine;
		
		//加中间的边相交点
		for( int i=2; i<num; i+=2)
		{
			double vx0 = pts[i-1].x - pts[i-2].x, vx1 = pts[i+1].x - pts[i].x;
			double vy0 = pts[i-1].y - pts[i-2].y, vy1 = pts[i+1].y - pts[i].y;
			double t;
			GraphAPI::GGetLineIntersectLine(pts[i-2].x,pts[i-2].y,vx0,vy0,pts[i].x,pts[i].y,vx1,vy1,&expt.x,&expt.y,&t);
			
			expt.z = pts[i-2].z + t * (pts[i-1].z - pts[i-2].z);
			arr.Add(expt);
		}
		
		//加首点
		double vx0 = pts[1].x - pts[0].x, vx1 = pts[num-1].x - pts[num-2].x;
		double vy0 = pts[1].y - pts[0].y, vy1 = pts[num-1].y - pts[num-2].y;
		double t;
		GraphAPI::GGetLineIntersectLine(pts[0].x,pts[0].y,vx0,vy0,pts[num-2].x,pts[num-2].y,vx1,vy1,&expt.x,&expt.y,&t);
			
		expt.z = pts[0].z + t * (pts[1].z - pts[0].z);
		arr.Add(expt);
	}
	else
	{
		PT_3DEX expt;
		expt.pencode = penLine;
		
		//加首点
		arr.Add(pts[0]);
		
		//加中间的边相交点
		for( int i=2; i<num; i+=2)
		{
			double vx0 = pts[i-1].x - pts[i-2].x, vx1 = pts[i+1].x - pts[i].x;
			double vy0 = pts[i-1].y - pts[i-2].y, vy1 = pts[i+1].y - pts[i].y;
			double t;
			GraphAPI::GGetLineIntersectLine(pts[i-2].x,pts[i-2].y,vx0,vy0,pts[i].x,pts[i].y,vx1,vy1,&expt.x,&expt.y,&t);
			
			expt.z = pts[i-2].z + t * (pts[i-1].z - pts[i-2].z);
			arr.Add(expt);
		}
		
		//加尾点
		arr.Add(pts[num-1]);
		

		int num0 = arr.GetSize();

		// 延长距离
		double len;
		PT_4D pt4d(arr[0].x,arr[0].y,arr[0].z);	
		m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(&pt4d,1);
		pt4d.x += 400;
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(&pt4d,1);
		len = GraphAPI::GGet3DDisOf2P(PT_3D(arr[0].x,arr[0].y,arr[0].z),PT_3D(pt4d.x,pt4d.y,pt4d.z));
		
		//将首点和尾点都延长400像素，模拟射线		
		PT_3DEX expt1, expt2;
		expt1 = arr.GetAt(0);
		expt2 = arr.GetAt(1);
		double len1 = GraphAPI::GGet3DDisOf2P(expt1,expt2);
		if (len1 <= GraphAPI::GetDisTolerance())
		{
			len1 = 1;
		}
		expt.x = expt1.x + (expt1.x-expt2.x)/len1*len;
		expt.y = expt1.y + (expt1.y-expt2.y)/len1*len;
		expt.z = expt1.z;// + (expt1.z-expt2.z)*10;
		arr.SetAt(0,expt);
		
		if( num0>=2 )
		{
			expt1 = arr.GetAt(num0-2);
			expt2 = arr.GetAt(num0-1);
			len1 = GraphAPI::GGet3DDisOf2P(expt1,expt2);
			if (len1 <= GraphAPI::GetDisTolerance())
			{
				len1 = 1;
			}
		}
		expt.x = expt2.x + (expt2.x-expt1.x)/len1*len;
		expt.y = expt2.y + (expt2.y-expt1.y)/len1*len;
		expt.z = expt2.z;// + (expt2.z-expt1.z)*10;
		arr.SetAt(num0-1,expt);
	}
	pObj->CreateShape(arr.GetData(),arr.GetSize());
	return pFtr;
}


void CDrawLinesByMultiPt::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;	
	
	m_bLastRClick = FALSE;

	GotoState(PROCSTATE_PROCESSING);
	
	int ntmppt = m_arrPts.GetSize();
	if( ntmppt>0 )GotoState(PROCSTATE_PROCESSING);
	if( ntmppt==1 && ((CDlgDoc*)m_pEditor)->IsAutoSetAnchor() )
	{
		((CDlgDoc*)m_pEditor)->SetAnchorPoint(pt);
	}
	//m_arrPts.Add(PT_3DEX(pt,penLine));

	m_arrBuildLine.Add(PT_3DEX(pt,penLine));
	CDrawCommand::PtClick(pt,flag);
}

void CDrawLinesByMultiPt::PtReset(PT_3D &pt)
{
	if (m_bLastRClick)
	{
		BOOL finished = TRUE;
		CFeature  *pFtr = CreateObjBySide(finished);
		if (!pFtr) 
		{
			Abort();
			return;
		}

		((CGeoCurve*)pFtr->GetGeometry())->EnableClose(TRUE);
		if( finished && pFtr )
		{		
			if( !AddObject(pFtr) )
			{
				Abort();
				return;
			}
			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Add(FtrToHandle(pFtr));
			undo.Commit();
		}
		else if( pFtr )delete pFtr;
		
		//	m_pEditor->SetCurDrawingObj(DrawingInfo());
		m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		//m_pDoc->UpdateAllViews(NULL,hc_Detach_Accubox);
		m_pEditor->RefreshView();
		
		m_arrPts.RemoveAll();
		
		if( !finished )
			Abort();
		else
		{
			if( m_pFtr!=NULL )
				delete m_pFtr;
			m_pFtr = NULL;
			Finish();
		}
	}
	else
	{
		m_bLastRClick = TRUE;
		
		int size = m_arrBuildLine.GetSize();
		if (size < 2)
		{
			m_arrBuildLine.RemoveAll();
			return;
		}
		
		// 最小二乘法直线似合，调整首尾点平面位置
		double A = 0, B = 0, C = 0, D = 0;
		for (int i=0; i<size; i++)
		{
			A += m_arrBuildLine[i].x*m_arrBuildLine[i].x;
			B += m_arrBuildLine[i].x;
			C += m_arrBuildLine[i].x*m_arrBuildLine[i].y;
			D += m_arrBuildLine[i].y;
		}
		
		double a = 1, b = 0, tmp = A*size-B*B;
		
		if(fabs(tmp) > 1e-6)
		{
			a = (C*size-B*D)/tmp;
			b = (A*D-C*B)/tmp;
			
			m_arrBuildLine[0].y = a * m_arrBuildLine[0].x + b;
			m_arrBuildLine[size-1].y = a * m_arrBuildLine[size-1].x + b;
		}
		
		m_arrPts.Add(m_arrBuildLine[0]);
		m_arrPts.Add(m_arrBuildLine[size-1]);
		
		m_arrBuildLine.RemoveAll();

		//m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		BOOL finished = FALSE;
		CFeature *pFtr = CreateObjBySide(finished);
		if( !pFtr )
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		else
		{
			GrBuffer vbuf;
			pFtr->GetGeometry()->Draw(&vbuf);
			
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
			delete pFtr;
		}
	}
}


void CDrawLinesByMultiPt::PtMove(PT_3D &pt)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;
	
	if( m_arrPts.GetSize()<=0 )
	{
		GrBuffer vbuf;
		for (int i=0; i<m_arrBuildLine.GetSize(); i++)
		{
			vbuf.Point(m_pFtr->GetGeometry()->GetColor(),&m_arrBuildLine[i],1,1);
		}
		
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		return;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	arrPts.Copy(m_arrBuildLine);
	//arrPts.Add(PT_3DEX(pt,penLine));
	//return;
	BOOL finished = FALSE;
	CFeature *pFtr = CreateObjBySide(finished);
	if( !pFtr )
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	else
	{
		GrBuffer vbuf;
		pFtr->GetGeometry()->Draw(&vbuf);

		for (int i=0; i<m_arrBuildLine.GetSize(); i++)
		{
			vbuf.Point(m_pFtr->GetGeometry()->GetColor(),&m_arrBuildLine[i],1,1);
		}

		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		delete pFtr;
	}
	
	CDrawCommand::PtMove(pt);
}


void CDrawLinesByMultiPt::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);

}


void CDrawLinesByMultiPt::FillShowParams(CUIParam* param, BOOL bForLoad )
{

}

void CDrawLinesByMultiPt::Finish()
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
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
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

void CDrawLinesByMultiPt::SetParams(CValueTable& tab,BOOL bInit)
{	
	CDrawCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CDrawTableCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDrawTableCommand,CDrawCommand)

CDrawTableCommand::CDrawTableCommand()
{
	m_nMode = -1;
	m_nTableColumn = 2;
	m_nTableWidthSpace = 10;
	m_nTableLine = 0;
	m_StrTableColumnSpace = _T("200,50");
	m_pFtr = NULL;

}

CDrawTableCommand::~CDrawTableCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
}

CString CDrawTableCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DRAWTABLE);
}

BOOL CDrawTableCommand::GetPartColumSpace(CStringArray &StrColumnSpace,int i,double &Total)
{
	if (StrColumnSpace.GetSize()<=0||i<0||i>StrColumnSpace.GetSize())
	{
		return FALSE;
	}
	double Temp=0;
	for (int j=0;j<i;j++)
	{
		Temp = atof((LPTSTR)(LPCTSTR)StrColumnSpace.GetAt(j));
		Total +=Temp;
	}
	
	return TRUE;
	
	
}

void CDrawTableCommand::Start()
{
	if (!m_pEditor)
	{
		return;
	}
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	

	CDrawCommand::Start();
	m_nMode = 0;
	m_pEditor->CloseSelector();
}

void CDrawTableCommand::Finish()
{
//	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CDrawCommand::Finish();

}

void CDrawTableCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_TABLELINES,var) )
	{
		m_nTableLine = (double)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TABLECOLUMNS,var) )
	{
		m_nTableColumn = (double)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TABLESPACE,var) )
	{
		m_nTableWidthSpace = (double)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TABLECOLUMNSPACE,var) )
	{
		m_StrTableColumnSpace = (LPCTSTR)(_bstr_t)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	CDrawCommand::SetParams(tab,bInit);
}

void CDrawTableCommand::GetParams(CValueTable& tab)
{
	CDrawCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)m_nTableLine;
	tab.AddValue(PF_TABLELINES,&(CVariantEx)(long)(var));
	var = (long)m_nTableColumn;
	tab.AddValue(PF_TABLECOLUMNS,&(CVariantEx)(long)(var));
	var = (long)m_nTableWidthSpace;
	tab.AddValue(PF_TABLESPACE,&(CVariantEx)(long)(var));
	var = (_bstr_t)(LPCTSTR)m_StrTableColumnSpace;
	tab.AddValue(PF_TABLECOLUMNSPACE,&(CVariantEx)(var));

}

void CDrawTableCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DrawTable",StrFromLocalResID(IDS_CMDNAME_DRAWTABLE));
	param->AddParam(PF_TABLELINES,(int)m_nTableLine,StrFromResID(IDS_TABLELINES));
	param->AddParam(PF_TABLECOLUMNS,(int)m_nTableColumn,StrFromResID(IDS_TABLECOLUMNS));
	param->AddParam(PF_TABLESPACE,(int)m_nTableWidthSpace,StrFromResID(IDS_TABLESPACEWIDTH));
	param->AddParam(PF_TABLECOLUMNSPACE,(LPCTSTR)m_StrTableColumnSpace,StrFromResID(IDS_TABLECOLUMNSPACE));
	
}


// void CDrawTableCommand::PtReset(PT_3D &pt)
// {
// 
// 
// }

extern CDocument *GetCurDocument();

void CDrawTableCommand::PtClick(PT_3D &pt, int flag)
{

	
	GotoState(PROCSTATE_PROCESSING);
	PT_3DEX expt(pt,penLine);
	PT_3DEX	pt_t(expt);
	m_ptStart = pt;
	CDlgDoc *pDoc = (CDlgDoc*)GetCurDocument();
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	CFtrLayer *pLayer = pDS->GetFtrLayer(StrFromResID(IDS_LEGEND_TABLELINES));
	if (!pLayer)
	{
		pLayer = pDS->CreateFtrLayer(StrFromResID(IDS_LEGEND_TABLELINES));
		if (!pLayer)
		{
			return;
		}
		pDS->AddFtrLayer(pLayer);	
	}
	if (!Split(m_StrTableColumnSpace,_T(","),m_StrColumnSpace))
	{
		return;
	}
	if (m_nTableColumn != m_StrColumnSpace.GetSize())
	{
		AfxMessageBox(StrFromResID(IDS_PARAMNOTMATCH));
		return;
	}

	if (m_nMode == 0)
	{
		GrBuffer buf;
		if (m_nTableColumn<2)
		{
			AfxMessageBox(StrFromResID(IDS_NOTE_DRAWTABLE));
			return;
		}
		CUndoFtrs undo(m_pEditor,Name());

		for (int i = 0;i<m_nTableLine; i++)
		{
			
			for (int j = 0;j<m_nTableColumn; j++)
			{
				m_arrPts.RemoveAll();
				double TableColumnSpace = 0,TableColumnSpace1 = 0;
				if(!GetPartColumSpace(m_StrColumnSpace,j,TableColumnSpace)) return;
				if(!GetPartColumSpace(m_StrColumnSpace,j+1,TableColumnSpace1)) return;
				pt_t.x = expt.x + TableColumnSpace;	
				pt_t.y = expt.y - i*m_nTableWidthSpace;
				pt_t.z = expt.z;
				m_arrPts.Add(pt_t);	
				
				pt_t.x = expt.x + TableColumnSpace1;	
				pt_t.y = expt.y - i*m_nTableWidthSpace;
				pt_t.z = expt.z;					
				m_arrPts.Add(pt_t);
				pt_t.x = expt.x + TableColumnSpace1;	
				pt_t.y = expt.y - (i+1)*m_nTableWidthSpace;
				pt_t.z = expt.z;
				m_arrPts.Add(pt_t);
				pt_t.x = expt.x + TableColumnSpace;	
				pt_t.y = expt.y - (i+1)*m_nTableWidthSpace;
				pt_t.z = expt.z;
				m_arrPts.Add(pt_t);
				pt_t.x = expt.x + TableColumnSpace;	
				pt_t.y = expt.y - i*m_nTableWidthSpace;
				pt_t.z = expt.z;
				m_arrPts.Add(pt_t);
				m_pFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
				
				if(!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize())) return;
				m_pFtr->GetGeometry()->SetColor(RGB(255,0,0));	
				undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
				if( !m_pEditor->AddObject(m_pFtr,pLayer->GetID()) )
				{
					Abort();
					return;
				}	
				
			}							
		}

		undo.Commit();
		
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pFtr = NULL;
	
		Finish();
	}
	

	CDrawCommand::PtClick(pt,flag);
}



void CDrawTableCommand::Abort()
{
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->RefreshView();
	
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CDrawCommand::Abort();
}

//////////////////////////////////////////////////////////////////////
// CCreateSymbolAndDiscriptCommand Class//生成符号和文字描述
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCreateSymbolAndDiscriptCommand,CDrawCommand)
CCreateSymbolAndDiscriptCommand::CCreateSymbolAndDiscriptCommand()
{
	{
		m_sTxtSettings.nInclineType = 0;
		m_sTxtSettings.fInclineAngle = 0;
		m_sTxtSettings.nPlaceType = singPt;
		m_sTxtSettings.nAlignment = TAH_MID|TAV_MID;
		m_sTxtSettings.SetUnderline(FALSE);
	}
	m_bType = TRUE;
	m_nAlignment = 0;
	m_strLayName = StrFromResID(IDS_DEFLAYER_NAMEL);
	m_pFtr = NULL;
	pLayersymbol = NULL;
	m_pFtrSymbol = NULL;
	m_nShape = 1;
	m_nHeight = 2;
	m_nWidth = 2;
	m_nDiameter = 2;
	strcat(m_strRegPath,"\\CreateSymbolAndDiscript");

	
}

CCreateSymbolAndDiscriptCommand::~CCreateSymbolAndDiscriptCommand()
{
	if(m_pFtr) { delete m_pFtr; m_pFtr=NULL;}
	if(m_pFtrSymbol) { delete m_pFtrSymbol; m_pFtr = NULL;}	
}

CString CCreateSymbolAndDiscriptCommand::Name()
{
	return StrFromResID(IDS_CREATSYMBOLANDDISCRIPTION);
}

void CCreateSymbolAndDiscriptCommand::Start()
{
	if (!m_pEditor)
	{
		return;
	}

	m_pEditor->CloseSelector();
	CDrawCommand::Start();
}
BOOL  CheckCodeForLayerName(CDlgDataSource *pDS, LPTSTR code, CString &name)
{
	if (!pDS || !name||!code||strlen(code)<=0)
	{
		return FALSE;
	}
	CConfigLibManager *pConfi = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	CScheme *pScheme = pConfi->GetScheme(pDS->GetScale());
	if(!pScheme)return FALSE;
	name.Empty();

	CString text(code);
	CStringArray arr;
	convertStringToStrArray(text, arr);
	for (int i = 0; i < arr.GetSize(); i++)
	{
		CString ret = arr[i];
		__int64 code = _atoi64(ret);
		if( code>0 )
		{
			if(pScheme->FindLayerIdx(TRUE,code,ret))//层码层名互转	
			{
				name+=ret;
				name+=_T(",");
			}
			else
			{
				CString Tip = ret;
				Tip+=StrFromResID(IDS_CANNOT_FIND_LOCALLAYERNAME);
				GOutPut(Tip);
			}
		}
		else
		{
			if (pScheme->FindLayerIdx(FALSE, code, ret))//层码层名互转	
			{
				name += ret;
				name+=_T(",");
			}
			else
			{
				CString Tip = ret;
				Tip+=StrFromResID(IDS_CANNOT_FIND_LOCALLAYERNAME);
				GOutPut(Tip);
			}
		}
	
	}


	return TRUE;
}


BOOL CCreateSymbolAndDiscriptCommand::CheckStrLayerName(CString SrcLayerName,CStringArray &StrArrLayerName,CString &DesLayerName)
{
	if (SrcLayerName.IsEmpty())
	{
		return FALSE;
	}
	StrArrLayerName.RemoveAll();
	DesLayerName.Empty();
	CStringArray StrArrTemp;
	if (!Split(SrcLayerName,_T(","),StrArrTemp))
	{
		return FALSE;
	}
	int nsize = StrArrTemp.GetSize();
	for (int i=0;i<nsize;i++)
	{
		BOOL IsAdd = TRUE;
		for (int j=0;j<StrArrLayerName.GetSize();j++)
		{
			if (StrArrLayerName.GetAt(j).CompareNoCase(StrArrTemp.GetAt(i)) == 0)
			{
				IsAdd = FALSE;
			}
			
		}
		
		if (IsAdd)
		{
			StrArrLayerName.Add(StrArrTemp.GetAt(i));
		}
	}
	for (int k=0;k<StrArrLayerName.GetSize();k++)
	{
		DesLayerName =DesLayerName+StrArrLayerName.GetAt(k)+_T(",");		
			
	}
	CString DestLayerName;
	if(!CheckCodeForLayerName(PDOC(m_pEditor)->GetDlgDataSource(),(LPTSTR)(LPCTSTR)DesLayerName,DestLayerName))
	{
		return FALSE;	
	}
	DesLayerName.Empty();
	DesLayerName = DestLayerName;
	CString StrTemp(DesLayerName);
	if (StrTemp.Right(1).CompareNoCase(_T(","))==0)
	{
		DesLayerName.Delete(DesLayerName.GetLength()-1,1);
	}
	return TRUE;
		
}



void CCreateSymbolAndDiscriptCommand::Finish()
{
	UpdateParams(TRUE);
			
			
		
			
			
		
	if( m_pEditor )
	{	
		m_pFtr=NULL;
		m_pFtrSymbol = NULL;
		m_pEditor->RefreshView();
		GotoState(PROCSTATE_FINISHED);
		m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	}
	CDrawCommand::Finish();
}

void CCreateSymbolAndDiscriptCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	
	const CVariantEx *var;
	if( tab.GetValue(0,PF_LAYERNAME,var) )
	{
		m_strLayName =(LPCTSTR)(_bstr_t)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CREATETYPE,var) )
	{
		m_bType =(bool)(_variant_t)*var;	
		SetSettingsModifyFlag();
		if (!bInit)

	{
			Abort();
			
		}
	}

	{
		if( tab.GetValue(0,PF_FONT,var) )
		{
			if( var->m_variant.vt==VT_BSTR )
			{
				CString temp = (LPCTSTR)(_bstr_t)(_variant_t)*var;
				strncpy(m_sTxtSettings.strFontName,temp,sizeof(m_sTxtSettings.strFontName)-1);
				SetSettingsModifyFlag();
			}
		}
		if( tab.GetValue(0,PF_CHARH,var) )
		{
			m_sTxtSettings.fHeight = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if( tab.GetValue(0,PF_CHARW,var) )
		{
			m_sTxtSettings.fWidScale = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if( tab.GetValue(0,PF_CHARI,var) )
		{
			m_sTxtSettings.fCharIntervalScale = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if( tab.GetValue(0,PF_LINEI,var) )
		{
			m_sTxtSettings.fLineSpacingScale = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if( tab.GetValue(0,"Bold",var) )
		{
			m_sTxtSettings.SetBold((bool)(_variant_t)*var);
			SetSettingsModifyFlag();
		}
	}

	{
		if( tab.GetValue(0,PF_CREATE_SHAPE,var) )
		{
			m_nShape = (long)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		{
			if( tab.GetValue(0,PF_SHAPEWIDTH,var) )
			{
				m_nWidth = (double)(_variant_t)*var;
				SetSettingsModifyFlag();
			}
			if( tab.GetValue(0,PF_SHAPEHEIGHT,var) )
			{
				m_nHeight = (double)(_variant_t)*var;
				SetSettingsModifyFlag();
			}
		}
		{
			if( tab.GetValue(0,PF_SHAPEDIAMETER,var) )
			{
				m_nDiameter = (double)(_variant_t)*var;
				SetSettingsModifyFlag();
			}	
		}
	}
	CDrawCommand::SetParams(tab,bInit);

}

void CCreateSymbolAndDiscriptCommand::GetParams(CValueTable& tab)
{
	CDrawCommand::GetParams(tab);

	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strLayName;
	tab.AddValue(PF_LAYERNAME,&CVariantEx(var));
	var = (bool)(m_bType);
	tab.AddValue(PF_CREATETYPE,&(CVariantEx)(var));
// 	var = (long)(m_nAlignment);
// 	tab.AddValue(PF_ALIGNMENT,&(CVariantEx)(var));
	var = (_variant_t)(long)(m_nShape);
	tab.AddValue(PF_CREATE_SHAPE,&CVariantEx(var));
	var = (double)(m_nWidth);
	tab.AddValue(PF_SHAPEWIDTH,&CVariantEx(var));
	var = (double)(m_nHeight);
	tab.AddValue(PF_SHAPEHEIGHT,&CVariantEx(var));
	var = (double)(m_nDiameter);
	tab.AddValue(PF_SHAPEDIAMETER,&CVariantEx(var));
	var = (LPCTSTR)(m_sTxtSettings.strFontName);
	tab.AddValue(PF_FONT,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fHeight);
	tab.AddValue(PF_CHARH,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fWidScale);
	tab.AddValue(PF_CHARW,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fCharIntervalScale);
	tab.AddValue(PF_CHARI,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fLineSpacingScale);
	tab.AddValue(PF_LINEI,&CVariantEx(var));
	var = (_variant_t)(bool)(m_sTxtSettings.IsBold());
	tab.AddValue("Bold",&CVariantEx(var));

}


void CCreateSymbolAndDiscriptCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CreateSymbolAndDiscriptCommand",StrFromLocalResID(IDS_CREATSYMBOLANDDISCRIPTION));
	param->AddLayerNameParamEx("LayerName",(LPCTSTR)m_strLayName,StrFromResID(IDS_FIELDNAME_LAYERNAME));
	param->BeginOptionParam(PF_CREATETYPE,StrFromResID(IDS_CREATETYPE));
	param->AddOption(StrFromResID(IDS_LAYERNAME_SYM),0,' ',m_bType == 0);
	param->AddOption(StrFromResID(IDS_WORDSDISCRIPTION),1,' ',m_bType == 1);
	param->EndOptionParam();
	if(!m_bType||bForLoad )
	{
		param->BeginOptionParam(PF_CREATE_SHAPE,StrFromResID(IDS_CREATE_SHAPE));
		param->AddOption(StrFromResID(IDS_DRAWTYPE_LINE),shap_Line,' ',m_nShape==shap_Line);
		param->AddOption(StrFromResID(IDS_DRAWTYPE_RECT),shap_Rect,' ',m_nShape==shap_Rect);
		param->AddOption(StrFromResID(IDS_DRAWTYPE_CIRCLE),shap_CirCle,' ',m_nShape==shap_CirCle);
	param->EndOptionParam();
		if (shap_Rect == m_nShape||bForLoad)
		{
			param->AddParam(PF_SHAPEWIDTH,(double)m_nWidth,StrFromResID(IDS_SHAPE_WIDTH));
			param->AddParam(PF_SHAPEHEIGHT,(double)m_nHeight,StrFromResID(IDS_SHAPE_HEIGHT));	
		}
		if ( shap_CirCle == m_nShape||bForLoad)
		{
			param->AddParam(PF_SHAPEDIAMETER,(double)m_nDiameter,StrFromResID(IDS_SHAPE_DIAMETER));	
		}
	}

	if(m_bType||bForLoad)
	{
// 		param->BeginOptionParam(PF_ALIGNMENT,StrFromResID(IDS_CMDPLANE_ALIGNTYPE));
// 		param->AddOption(StrFromResID(IDS_LEFTALIGN),0,' ',m_nAlignment==0);
// 		param->AddOption(StrFromResID(IDS_CENTERALIGN),1,' ',m_nAlignment==1);
// 		param->AddOption(StrFromResID(IDS_RIGHTALIGN),2,' ',m_nAlignment==2);
// 		param->EndOptionParam();
		param->AddFontNameParam(PF_FONT,m_sTxtSettings.strFontName,StrFromResID(IDS_CMDPLANE_FONT));
		param->AddParam(PF_CHARH,(double)m_sTxtSettings.fHeight,StrFromResID(IDS_CMDPLANE_CHARH));
		param->AddParam(PF_CHARW,(double)m_sTxtSettings.fWidScale,StrFromResID(IDS_CMDPLANE_CHARWS));
		param->AddParam(PF_CHARI,(double)m_sTxtSettings.fCharIntervalScale,StrFromResID(IDS_CMDPLANE_CHARIS));
		param->AddParam(PF_LINEI,(double)m_sTxtSettings.fLineSpacingScale,StrFromResID(IDS_CMDPLANE_LINEIS));
		param->AddParam("Bold",(bool)m_sTxtSettings.IsBold(),StrFromResID(IDS_BOLD));
	}
	
}

void CCreateSymbolAndDiscriptCommand::PtReset(PT_3D &pt)
{
//  	if( !m_pFtr||!m_pFtrSymbol|| IsProcFinished(this) )
// 	{
// 		Abort();
// 		return;
// 	}
//  	if (m_pFtr->GetGeometry()->GetDataPointSum()<=0&&m_pFtrSymbol->GetGeometry()->GetDataPointSum()<=0)
//  	{
//  		Abort();
//  	}
//  	else
//  		Finish();
	Abort();
	CDrawCommand::Abort();

}

void CCreateSymbolAndDiscriptCommand::PtClick(PT_3D &pt, int flag)
{
	
	CDlgDataSource *pDS = GETDS(m_pEditor);
	
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());

	CString desLayerName;
	CheckStrLayerName(m_strLayName,m_StrArrLayerName,desLayerName);//去重复
//	m_strLayName = desLayerName;
	CUndoFtrs undo(m_pEditor,Name());
	GotoState(PROCSTATE_PROCESSING);
 	if (m_bType ==1)
 	{
		pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
		if(!pLayer) return;
		pLayer->SetName(StrFromResID(IDS_DEFLAYER_NAMET));
		m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOTEXT);
		if(!m_pFtr) return;	
		if (0 == m_nAlignment )
		{
			m_sTxtSettings.nAlignment = TAH_LEFT|TAV_MID;	
		}
		else if (1 == m_nAlignment  )
		{
			m_sTxtSettings.nAlignment = TAH_MID|TAV_MID;
		}
		else if (2 == m_nAlignment)
		{
			m_sTxtSettings.nAlignment = TAH_RIGHT|TAV_MID;
		}
		((CGeoText*)m_pFtr->GetGeometry())->SetText(desLayerName);
		((CGeoText*)m_pFtr->GetGeometry())->SetSettings(&m_sTxtSettings);
		
		//如果文本内容为空，则return;
		CString str=((CGeoText*)m_pFtr->GetGeometry())->GetText();
		if(str.IsEmpty())
		{
			return;
		}

		TEXT_SETTINGS0 settings;
		((CGeoText*)m_pFtr->GetGeometry())->GetSettings(&settings);
		CArray<PT_3DEX,PT_3DEX> arr;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		m_pFtr->GetGeometry()->GetShape(arr);
		PT_3DEX t;
		PT_3D retpt;
		retpt = GetAlignmentPt(pt,arrPts);
		int nsize = arrPts.GetSize();
		if (nsize!=5)
		{
			return;
		}	
		t.x = retpt.x;
		t.y = retpt.y;
		t.z = retpt.z;
		t.pencode= penLine;
		arr.Add(t);	
		m_pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());
		if( settings.nPlaceType==0 )//单点排列
		if (m_bType)
		{
			if(! m_pEditor->AddObject(m_pFtr,pLayer->GetID()) )
			{
				Abort();
				return;
			}	
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr));	
		}
 	}
 	else if (m_bType == 0)//符号
 	{

		int nLayerName = m_StrArrLayerName.GetSize();
		PT_3D retpt;
		double cx,cy;//矩形的长宽
		m_nAlignment = 0;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		retpt = GetAlignmentPt(pt,arrPts);	
		for (int i = 0;i<nLayerName;i++)
		{
			pLayersymbol = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_StrArrLayerName.GetAt(i));
			if(!pLayersymbol) continue;	
 		//	CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(m_StrArrLayerName.GetAt(i));
 		//	if (!pSchemeLayer) continue;
 		//	m_pFtrSymbol = pLayersymbol->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),pSchemeLayer->GetGeoClass());
			m_pFtrSymbol = pLayersymbol->CreateDefaultFeature(GETDS(m_pEditor)->GetScale());
			if(!m_pFtrSymbol) continue;
			m_nScale = pDS->GetScale();		
			int nsize = arrPts.GetSize();
			if (nsize!=5)
			{
				continue;
			}	
			cx =fabs( arrPts.GetAt(1).x-arrPts.GetAt(0).x);
			cy =fabs(arrPts.GetAt(0).y-arrPts.GetAt(3).y);

			if (0 == m_nAlignment)
			{
				if (i!=0)
				{
					retpt.x+=cx/(nLayerName);
				}
			}
			if(!CreateSymbolItem(pLayersymbol,retpt,cx,cy,nLayerName)) continue;
			if (m_pEditor)
			{
				if( !m_pEditor->AddObject(m_pFtrSymbol,pLayersymbol->GetID()) )
				{
					Abort();
					return;
		}
					undo.arrNewHandles.Add(FtrToHandle(m_pFtrSymbol));
			}
		}
 	}
	
	undo.Commit();
	Finish();
	CDrawCommand::PtClick(pt, flag);
}

void CCreateSymbolAndDiscriptCommand::Abort()
{
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();

	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CDrawCommand::Abort();
}

PT_3D CCreateSymbolAndDiscriptCommand::GetAlignmentPt(PT_3D &pt,CArray<PT_3DEX,PT_3DEX> &arrPts)
{
	PT_3D retPt;
	CDlgDataSource *pDS = GETDS(m_pEditor);
	CFtrLayer *pFtr = pDS->GetFtrLayer(StrFromResID(IDS_LEGEND_TABLELINES));//找图例表格线层
	if (!pFtr)
	{
		retPt.x = pt.x;
		retPt.y = pt.y;
		retPt.z = pt.z;
		return retPt;
	}
	int nObj = pFtr->GetObjectCount();
	Envelope e;
	for (int j=0;j<nObj;j++)
	{
		CFeature *pFeatr = pFtr->GetObject(j);
		if (!pFeatr||!pFeatr->IsVisible())
			continue;
		CGeometry *pGeo = pFeatr->GetGeometry(); 
		
		pGeo->GetShape()->GetPts(arrPts);
		int nPts = arrPts.GetSize();
		if (nPts!=5)//排除非闭合矩形地物
			continue;
		e = pGeo->GetShape()->GetEnvelope();
		if (e.bPtIn(&pt))
		{
			if (0 == m_nAlignment )//左对齐
			{
				retPt.x = arrPts.GetAt(0).x;
				retPt.y = (arrPts.GetAt(3).y+arrPts.GetAt(0).y)/2;
				retPt.z = arrPts.GetAt(0).z;
			}
			else if (-1 == m_nAlignment||1 == m_nAlignment )//居中对齐
			{
				retPt.x = (arrPts.GetAt(0).x+arrPts.GetAt(1).x)/2;
				retPt.y = (arrPts.GetAt(3).y+arrPts.GetAt(0).y)/2;
				retPt.z = arrPts.GetAt(0).z;

			}
			else if (2 == m_nAlignment)//右对齐
			{

				retPt.x = arrPts.GetAt(1).x;
				retPt.y = (arrPts.GetAt(1).y+arrPts.GetAt(2).y)/2;
				retPt.z = arrPts.GetAt(0).z	;
				
			}

			return retPt;
		}

	}	
	//找不到返回原坐标
	retPt.x = pt.x;
	retPt.y = pt.y;
	retPt.z = pt.z;
	arrPts.RemoveAll();
	return retPt;
}

BOOL CCreateSymbolAndDiscriptCommand::CreateSymbolItem(CSchemeLayerDefine *pLayer,PT_3D pt,double cx,double cy,int nLayerNum)
{
	if (!pLayer)
	{
		return FALSE;
	}
	int nSymbolNum = pLayer->GetSymbolCount();

	GrBuffer bufs;
	if (!m_pFtrSymbol)
	{
// 	CFeature *pFeature = pLayer->CreateDefaultFeature();
		return FALSE;
	}
	CGeometry *pGeo =m_pFtrSymbol->GetGeometry();
	if (!pGeo)
	{
		return FALSE;
	}
	int nGeoClass = pGeo->GetClassType();

	if (nGeoClass==CLS_GEOPOINT || nGeoClass==CLS_GEODIRPOINT)
	{
		pGeo->CreateShape(&PT_3DEX(pt.x+cx/nLayerNum/2,pt.y,0,penMove,cy/2),1);
	}
	else if (nGeoClass == CLS_GEOPARALLEL)//双线
	{
		// 平行线有填充时填充在基线和辅助线之间
		PT_3DEX pts[2];
		if (0 == m_nAlignment)//左对齐
		{
			pts[0].x = pt.x;
			pts[0].y = pt.y-cy/2+2;
			pts[0].pencode = penLine;
			pts[1].x = pt.x+cx/(nLayerNum);
			pts[1].y = pt.y-cy/2+2;
			pts[1].pencode = penLine;
		}

		pGeo->CreateShape(pts,2);

		CGeoParallel *pParallele = (CGeoParallel*)pGeo;
		pParallele->SetWidth(cy-4);

	}
	else if(nGeoClass == CLS_GEOCURVE)//单线 含填充
	{
		// 检查是否有颜色或图元填充,有则添加四个点方便填充
		BOOL bHatch = FALSE;
		BOOL bArc = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = pLayer->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALETURNPLATETYPE || nType==SYMTYPE_SCALE_LiangChang || nType==SYMTYPE_SCALE_YouGuan )
			{
				bArc = TRUE;
			}
			else if (nType == SYMTYPE_SCALEFUNNELTYPE && (((CScaleFunneltype*)pSym)->m_nFunnelType == 1 || ((CScaleFunneltype*)pSym)->m_nFunnelType == 3))
			{
				bArc = TRUE;
			}
			
			if( nType==SYMTYPE_CELLHATCH || nType==SYMTYPE_COLORHATCH || nType==SYMTYPE_LINEHATCH || nType == SYMTYPE_DIAGONAL || nType == SYMTYPE_SCALE_DiShangYaoDong ||
				nType==SYMTYPE_SCALE_JianFangWu || nType==SYMTYPE_ANGBISECTORTYPE  )
			{
				bHatch = TRUE;
				break;
			}
		}
		if (bArc)
		{
			PT_3DEX pts[4];
			pts[0].x = pt.x;
			pts[0].y = pt.y;
			pts[0].pencode = penArc;
			pts[1].x = pt.x+cy/nLayerNum/2;
			pts[1].y = pt.y+(cy/2);
			pts[1].pencode = penArc;
			pts[2].x = pt.x+(cy/nLayerNum);
			pts[2].y = pt.y;
			pts[2].pencode = penArc;
			pts[3].x = pt.x;
			pts[3].y = pt.y;
			pts[3].pencode = penArc;
			pGeo->CreateShape(pts,4);
		}
		else if (bHatch)
		{
			PT_3DEX pts[5];
			pts[0].x = pt.x;
			pts[0].y = pt.y;
			pts[0].pencode = penLine;
			pts[1].x = pt.x+cx/nLayerNum;
			pts[1].y = pt.y;
			pts[1].pencode = penLine;
			pts[2].x = pt.x+cx/nLayerNum;
			pts[2].y = pt.y+cy/nLayerNum;
			pts[2].pencode = penLine;
			pts[3].x = pt.x;
			pts[3].y = pt.y+cy/nLayerNum;
			pts[3].pencode = penLine;
			pts[4].x = pt.x;
			pts[4].y = pt.y;
			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);

		}
		else
		{
			PT_3DEX pts[2];
			if (0 == m_nAlignment  )
			{
				pts[0].x = pt.x;
				pts[0].y = pt.y;
				pts[0].pencode = penLine;
				pts[1].x = pt.x+cx/(nLayerNum);
				pts[1].y = pt.y;
				pts[1].pencode = penLine;
			}

		pGeo->CreateShape(pts,2);

		}	
		
		
	}
	else if (nGeoClass == CLS_GEODCURVE)
	{
		PT_3DEX pts[4];
		if ( 0 == m_nAlignment)
		{
			pts[0].x = pt.x;
			pts[0].y = pt.y-cy/2+2;
			pts[0].pencode = penMove;
			pts[1].x = pt.x+cx/(nLayerNum);
			pts[1].y = pt.y-cy/2+2;
			pts[1].pencode = penLine;
			pts[2].x = pt.x+cx/(nLayerNum);
			pts[2].y = pt.y+cy/2-2;
			pts[2].pencode = penMove;
			pts[3].x = pt.x;
			pts[3].y = pt.y+cy/2-2;
			pts[3].pencode = penLine;
		}
		pGeo->CreateShape(pts,4);
	}
	else if(nGeoClass == CLS_GEOSURFACE)
	{
		BOOL bArc = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = pLayer->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALEFUNNELTYPE && ((CScaleFunneltype*)pSym)->m_nFunnelType == 1)
			{
				bArc = TRUE;
			}
		}
		
		if (bArc)
		{

			PT_3DEX pts[4];
			pts[0].x = pt.x;
			pts[0].y = pt.y;
			pts[0].pencode = penArc;
			pts[1].x = pt.x+cy/nLayerNum/2;
			pts[1].y = pt.y+(cy/2);
			pts[1].pencode = penArc;
			pts[2].x = pt.x+(cy/nLayerNum);
			pts[2].y = pt.y;
			pts[2].pencode = penArc;
			pts[3].x = pt.x;
			pts[3].y = pt.y;
			pts[3].pencode = penArc;
			pGeo->CreateShape(pts,4);
		}
		else
		{
			PT_3DEX pts[5];
			if (0 == m_nAlignment)
			{
				pts[0].x = pt.x;
				pts[0].y = pt.y-cy/2+2;
				pts[0].pencode = penLine;
				pts[1].x = pt.x+cx/(nLayerNum);
				pts[1].y = pt.y-cy/2+2;
				pts[1].pencode = penLine;
				pts[2].x = pt.x+cx/(nLayerNum);
				pts[2].y = pt.y+cy/2-2;
				pts[2].pencode = penLine;
				pts[3].x = pt.x;
				pts[3].y = pt.y+cy/2-2;
				pts[3].pencode = penLine;
				pts[4].x = pt.x;
				pts[4].y = pt.y-cy/2+2;
				pts[4].pencode = penLine;	

			}

			pGeo->CreateShape(pts,5);
		}

	}
	else if (nGeoClass == CLS_GEOTEXT)
	{
		TEXT_SETTINGS0 settings;
		settings.fHeight = cy/(2*nLayerNum);
		pGeo->CreateShape(&PT_3DEX(pt.x,pt.y,0,penMove),1);
		if (0 == m_nAlignment )
		{
			settings.nAlignment = TAH_LEFT|TAV_MID;
		}
		((CGeoText*)pGeo)->SetSettings(&settings);
		((CGeoText*)pGeo)->SetText("abc");
	}



	return TRUE;
	
	
}

BOOL CCreateSymbolAndDiscriptCommand::CreateSymbolItem(CFtrLayer *pLayer,PT_3D pt,double cx,double cy,int nLayerNum)
{

	GrBuffer bufs;
	if (!m_pFtrSymbol)	return FALSE;
	CGeometry *pGeo =m_pFtrSymbol->GetGeometry();
	if (!pGeo) return FALSE;
	int nGeoClass = pGeo->GetClassType();

	if (nGeoClass==CLS_GEOPOINT || nGeoClass==CLS_GEODIRPOINT)
	{
		pGeo->CreateShape(&PT_3DEX(pt.x+cx/2/nLayerNum,pt.y,0,penMove,cy/2),1);
	}
	else if (nGeoClass == CLS_GEOPARALLEL)//双线
	{
		// 平行线有填充时填充在基线和辅助线之间
		PT_3DEX pts[2];
		if (0 == m_nAlignment)//左对齐
		{
			pt.x+=cx/nLayerNum/2;
			pts[0].x = pt.x-cx/nLayerNum/4;
			pts[0].y = pt.y-cy/4;
			pts[0].pencode = penLine;
			pts[1].x = pt.x+cx/(nLayerNum)/4;
			pts[1].y = pt.y-cy/4;
			pts[1].pencode = penLine;
		}

		pGeo->CreateShape(pts,2);

		CGeoParallel *pParallele = (CGeoParallel*)pGeo;
		pParallele->SetWidth(cy/2);

	}
	else if(nGeoClass == CLS_GEOCURVE)//单线 含填充
	{
		// 检查是否有颜色或图元填充,有则添加四个点方便填充
		BOOL bHatch = FALSE;
		BOOL bArc = FALSE;
		if (m_nShape == shap_CirCle)
		{
			bArc =TRUE;
			if (m_nDiameter<=0)
			{
				return FALSE;
			}
		}
		if (m_nShape == shap_Rect)
		{
			bHatch =TRUE;
			if (m_nHeight<=0||m_nWidth<=0)
			{
				return FALSE;
			}
		}
		if (bArc)
		{
			pt.x +=cx/2/nLayerNum;
			PT_3DEX pts[5];
			pts[0].x = pt.x;
			pts[0].y = pt.y+cy/(2*m_nDiameter);
			pts[0].pencode = penArc;
			pts[1].x = pt.x-cy/(2*m_nDiameter);
			pts[1].y = pt.y;
			pts[1].pencode = penArc;
			pts[2].x = pt.x;
			pts[2].y = pt.y-cy/(2*m_nDiameter);
			pts[2].pencode = penArc;
			pts[3].x = pt.x+cy/(2*m_nDiameter);
			pts[3].y = pt.y;
			pts[3].pencode = penArc;
			pts[4].x = pt.x;
			pts[4].y = pt.y+cy/(2*m_nDiameter);
			pts[4].pencode = penArc;
			pGeo->CreateShape(pts,5);
		}
		else if (bHatch)
		{
			pt.x +=cx/2/nLayerNum;
			PT_3DEX pts[5];
			pts[0].x = pt.x-cx/(2*m_nWidth*nLayerNum);
			pts[0].y = pt.y-cy/(2*m_nHeight);
			pts[0].pencode = penLine;
			pts[1].x = pt.x+cx/(2*m_nWidth*nLayerNum);
			pts[1].y = pt.y-cy/(2*m_nHeight);
			pts[1].pencode = penLine;
			pts[2].x = pt.x+cx/(2*m_nWidth*nLayerNum);
			pts[2].y = pt.y+cy/(2*m_nHeight);
			pts[2].pencode = penLine;
			pts[3].x = pt.x-cx/(2*m_nWidth*nLayerNum);
			pts[3].y = pt.y+cy/(2*m_nHeight);
			pts[3].pencode = penLine;
			pts[4].x = pt.x-cx/(2*m_nWidth*nLayerNum);
			pts[4].y = pt.y-cy/(2*m_nHeight);
			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);

		}
		else
		{
			pt.x +=cx/2/nLayerNum;
			PT_3DEX pts[2];
			if (0 == m_nAlignment  )
			{
				pts[0].x = pt.x-cx/nLayerNum/4;
				pts[0].y = pt.y;
				pts[0].pencode = penLine;
				pts[1].x = pt.x+cx/(nLayerNum)/4;
				pts[1].y = pt.y;
				pts[1].pencode = penLine;
			}

		pGeo->CreateShape(pts,2);

		}	
		
		
	}
	else if (nGeoClass == CLS_GEODCURVE)
	{
		PT_3DEX pts[4];
		if ( 0 == m_nAlignment)
		{
			pt.x+=cx/nLayerNum/2;
			pts[0].x = pt.x-cx/nLayerNum/4;
			pts[0].y = pt.y-cy/4;
			pts[0].pencode = penMove;
			pts[1].x = pt.x+cx/nLayerNum/4;
			pts[1].y = pt.y-cy/4;
			pts[1].pencode = penLine;
			pts[3].x = pt.x+cx/nLayerNum/4;
			pts[3].y = pt.y+cy/4;
			pts[3].pencode = penLine;
			pts[2].x = pt.x-cx/nLayerNum/4;
			pts[2].y = pt.y+cy/4;
			pts[2].pencode = penMove;
		}
		pGeo->CreateShape(pts,4);
	}
	else if(nGeoClass == CLS_GEOSURFACE)
	{
		BOOL bArc = FALSE;	
		if (bArc)
		{

			pt.x +=cx/2/nLayerNum;
			PT_3DEX pts[5];
			pts[0].x = pt.x;
			pts[0].y = pt.y+cx/(2*nLayerNum);
			pts[0].pencode = penArc;
			pts[1].x = pt.x-cx/(2*nLayerNum);
			pts[1].y = pt.y;
			pts[1].pencode = penArc;
			pts[2].x = pt.x;
			pts[2].y = pt.y-cx/(2*nLayerNum);
			pts[2].pencode = penArc;
			pts[3].x = pt.x+cx/(2*nLayerNum);
			pts[3].y = pt.y;
			pts[3].pencode = penArc;
			pts[4].x = pt.x;
			pts[4].y = pt.y+cx/(2*nLayerNum);
			pts[4].pencode = penArc;
			pGeo->CreateShape(pts,5);
		}
		else
		{
			PT_3DEX pts[5];
			
			pt.x +=cx/2/nLayerNum;
			pts[0].x = pt.x-cx/(4*nLayerNum);
			pts[0].y = pt.y-cy/(4);
			pts[0].pencode = penLine;
			pts[1].x = pt.x+cx/(4*nLayerNum);
			pts[1].y = pt.y-cy/(4);
			pts[1].pencode = penLine;
			pts[2].x = pt.x+cx/(4*nLayerNum);
			pts[2].y = pt.y+cy/(4);
			pts[2].pencode = penLine;
			pts[3].x = pt.x-cx/(4*nLayerNum);
			pts[3].y = pt.y+cy/(4);
			pts[3].pencode = penLine;
			pts[4].x = pt.x-cx/(4*nLayerNum);
			pts[4].y = pt.y-cy/(4);
			pts[4].pencode = penLine;	


			pGeo->CreateShape(pts,5);
		}

	}
	else if (nGeoClass == CLS_GEOTEXT)
	{
		TEXT_SETTINGS0 settings;
		settings.fHeight = cy/(2);
		pGeo->CreateShape(&PT_3DEX(pt.x,pt.y,0,penMove),1);
		if (0 == m_nAlignment )
		{
			settings.nAlignment = TAH_LEFT|TAV_MID;
		}
		((CGeoText*)pGeo)->SetSettings(&settings);
		((CGeoText*)pGeo)->SetText("abc");
	}

	return TRUE;
	
	
}


void CCreateSymbolAndDiscriptCommand::DrawSymbolItem(int cx, int cy, const GrBuffer2d *pBuf, COLORREF col, COLORREF colBak)
{
	//创建内存设备
	CClientDC cdc(PDOC(m_pEditor)->GetCurActiveView());
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
	if( !hDC )return;
	
	HBITMAP hBmp = ::CreateCompatibleBitmap(cdc.m_hDC,cx,cy);
	if( !hBmp )
	{
		::DeleteDC(hDC);
		return;
	}
	
	CBitmap bm;
	bm.Attach(hBmp);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
	
	CBrush br(RGB(128,128,128));
	::FillRect(hDC,CRect(0,0,cx,cy),(HBRUSH)br);
	//	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC,(HBRUSH)br);
	
	CPen pen;
	pen.CreatePen(PS_SOLID,0,(COLORREF)0);
	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);
	
	//计算变换系数
	CRect rect(2,2,cx-4,cy-4);
	Envelope e = pBuf->GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);
	float xoff=0, yoff=0;
	if( scalex>scaley )
	{
		xoff = cx/2-((e.m_xh-e.m_xl)/2*scaley+rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy/2-((e.m_yh-e.m_yl)/2*scalex+rect.top);
	}
	
	double matrix[9] = {
		scalex,0,-e.m_xl*scalex+rect.left+xoff+0.5,
			0,-scalex,cy+e.m_yl*scalex-rect.top-yoff-0.5,
			0,0,1
	};
	
	GDI_DrawGrBuffer2d(hDC,pBuf,TRUE,col,matrix,CSize(cx,cy),CRect(0,0,cx,cy),colBak);
	
	::SelectObject(hDC,hOldPen);
	::SelectObject(hDC,hOldBmp);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
}


//////////////////////////////////////////////////////////////////////
// CDrawTextCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawTextCommand,CDrawCommand)

CDrawTextCommand::CDrawTextCommand()
{
	m_pFtr = NULL;
	m_bDrag = FALSE;
	m_pSel = NULL;

}

CDrawTextCommand::~CDrawTextCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if(m_pSel)
		delete m_pSel;
}



CString CDrawTextCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWTEXT);
}

static BOOL FindStrInStrArr(const CStringArray *pArr,LPCSTR str)
{
	if(!pArr) return FALSE;
	for (int i=pArr->GetSize()-1;i>=0;i--)
	{
		if(strcmp(LPCSTR((*pArr)[i]),LPCSTR(str))==0)
			return TRUE;
	}
	if(i<0)
		return FALSE;
	return TRUE;
}
int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf, LPNEWTEXTMETRIC lpntm, DWORD nFontType, LONG_PTR lparam)
{
	
	CStringArray* pArr = (CStringArray*) lparam;
	if(!pArr) return 0;
    if (nFontType==TRUETYPE_FONTTYPE )
	{	
		if(!FindStrInStrArr(pArr,lpelf->elfLogFont.lfFaceName))
		    pArr->Add(lpelf->elfLogFont.lfFaceName);
	}
	return 1;
}


static BOOL ReadOptionalText(CStringArray& arr)
{
	CString path;
	path = GetConfigPath();
	
	CString filePath = path + "OptionalText.txt";
	FILE *fp = fopen(filePath,"r");
	if( !fp )return FALSE;
	
	char line[1024];
	while( !feof(fp) )
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		
		if( strlen(line)>0 )
		{
			arr.Add(line);
		}
	}
	
	fclose(fp);
	return TRUE;
}

static void AddOptionalDrawText(CUIFProp *pProp)
{
	CStringArray arr;
	if( !ReadOptionalText(arr) )
		return;
	
	for( int i=0; i<arr.GetSize(); i++)
	{
		pProp->AddOption(arr[i]);
	}
}


void CDrawTextCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOTEXT);
	if(!m_pFtr) return;	
	m_pEditor->CloseSelector();

	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
		
}

void CDrawTextCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
DrawingInfo CDrawTextCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX,PT_3DEX> arr;
	m_pFtr->GetGeometry()->GetShape(arr);
	return DrawingInfo(m_pFtr,arr);
}

void CDrawTextCommand::Abort()
{	
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	if(m_pFtr) 
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
	CDrawCommand::Abort();
}


void CDrawTextCommand::Back()
{	
// 	if( m_pFtr->GetPtSum()<=2 )
// 	{
// 		for( int i=m_pObj->GetPtSum()-1; i>=0; i--)
// 			m_pObj->DeletePt(m_pObj->GetPtSum()-1);
// 	}
// 	else
// 	{
// 		if( m_bDrag )
// 			m_pObj->DeletePt(m_pObj->GetPtSum()-2);
// 		else
// 			m_pObj->DeletePt(m_pObj->GetPtSum()-1);
// 	}
	
	CDrawCommand::Back();
}


void CDrawTextCommand::Finish()
{
	if (m_pEditor)
	{
		if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor,Name());
		undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
		
		undo.Commit();

		m_pEditor->UpdateDrag(ud_ClearDrag);
	}
	
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
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
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


void CDrawTextCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;

	GotoState(PROCSTATE_PROCESSING);

	((CGeoText*)m_pFtr->GetGeometry())->SetText(m_strText);
	((CGeoText*)m_pFtr->GetGeometry())->SetSettings(&m_sTxtSettings);
	
	//如果文本内容为空，则return;
	CString str=((CGeoText*)m_pFtr->GetGeometry())->GetText();
	if(str.IsEmpty())
	{
		//	Abort();
		return;
	}
	//获得文本设置选项
	TEXT_SETTINGS0 settings;
	((CGeoText*)m_pFtr->GetGeometry())->GetSettings(&settings);

	CArray<PT_3DEX,PT_3DEX> arr;
    m_pFtr->GetGeometry()->GetShape(arr);

	if( m_bDrag )
	{
		arr.RemoveAt(arr.GetSize()-1);
		m_bDrag = FALSE;
	}

	PT_3DEX t;
	t.x = pt.x;
	t.y = pt.y;
	t.z = pt.z;
	t.pencode= penLine;
	arr.Add(t);
    m_pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());

 	if( settings.nPlaceType==0 )
 		PtReset(pt);
 	else
	{
		GrBuffer cbuf,vbuf;
		
		CPlotTextEx plotText;
		plotText.SetShape(m_pFtr->GetGeometry()->GetShape());
		plotText.SetText(LPCTSTR(str));
		settings.fHeight *= PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale();
		plotText.SetSettings(&settings);
		plotText.GetAddConstGrBuffer(&cbuf);
	//	plotText.GetVariantGrBuffer(&vbuf);
		long color = m_pFtr->GetGeometry()->GetColor();
		if (color == FTRCOLOR_BYLAYER)
		{
			CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
			if(!pLayer)
			{	
				Abort();
				return;
			}
			color = pLayer->GetColor();
		}
		cbuf.SetAllColor(color);
		//vbuf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);	
		CDrawCommand::PtClick(pt,flag);
	}
	return;
}

void CDrawTextCommand::PtMove(PT_3D &pt)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;
	if( m_pFtr->GetGeometry()->GetDataPointSum()<=0 )return;
	
	PT_3DEX t;
	t.x = pt.x;
	t.y = pt.y;
	t.z = pt.z;
	t.pencode= penLine;
 	if( m_bDrag )
 		m_pFtr->GetGeometry()->SetDataPoint(m_pFtr->GetGeometry()->GetDataPointSum()-1,t);
 	else
 	{
 		CArray<PT_3DEX,PT_3DEX> arr;
 		m_pFtr->GetGeometry()->GetShape(arr);
 		arr.Add(t);
 		m_pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());
		m_bDrag = TRUE;
	}

	GrBuffer vbuf;
	CPlotTextEx plotText;
	plotText.SetShape(m_pFtr->GetGeometry()->GetShape());
	plotText.SetText(LPCTSTR(m_strText));

	TEXT_SETTINGS0 settingTmp;
	memcpy(&settingTmp,&m_sTxtSettings,sizeof(TEXT_SETTINGS0));
	CDlgDataSource *pDS = GETDS(m_pEditor);

	settingTmp.fHeight *= pDS->GetSymbolDrawScale();

//	m_pFtr->Draw(&vbuf,fDrawScale);

	plotText.SetSettings(&settingTmp);
	plotText.GetVariantGrBuffer(&vbuf);

	m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
// 	arr.RemoveAt(arr.GetSize()-1);
// 	m_pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());
	CDrawCommand::PtMove(pt);
}


void CDrawTextCommand::PtReset(PT_3D &pt)
{
	if( !m_pFtr || IsProcFinished(this) )return;
	
	if( m_bDrag )
	{
		CArray<PT_3DEX,PT_3DEX> arr;
		m_pFtr->GetGeometry()->GetShape(arr);
		arr.RemoveAt(arr.GetSize()-1);
		m_pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());		
	}
	if (m_pFtr->GetGeometry()->GetDataPointSum()<=0)
	{
		Abort();
	}
	else
		Finish();
}


void CDrawTextCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(m_strText);
	tab.AddValue(PF_TEXT,&CVariantEx(var));
	var = (LPCTSTR)(m_sTxtSettings.strFontName);
	tab.AddValue(PF_FONT,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fHeight);
	tab.AddValue(PF_CHARH,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fWidScale);
	tab.AddValue(PF_CHARW,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fCharIntervalScale);
	tab.AddValue(PF_CHARI,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fLineSpacingScale);
	tab.AddValue(PF_LINEI,&CVariantEx(var));
	var = (_variant_t)(long)(m_sTxtSettings.nInclineType);
	tab.AddValue(PF_SHRUG,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fInclineAngle);
	tab.AddValue(PF_SHRUGA,&CVariantEx(var));
	var = (_variant_t)(long)(m_sTxtSettings.nPlaceType);
	tab.AddValue(PF_LAYOUT,&CVariantEx(var));
	var = (_variant_t)(long)(m_sTxtSettings.nAlignment);
	tab.AddValue(PF_ALIGNTYPE,&CVariantEx(var));
	var = (_variant_t)(bool)(m_sTxtSettings.IsBold());
	tab.AddValue("Bold",&CVariantEx(var));
	var = (_variant_t)(bool)(m_sTxtSettings.IsUnderline());
	tab.AddValue("Underline",&CVariantEx(var));
}


void CDrawTextCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("DrawTextCommand",Name());
	param->AddMultiEditParam(PF_TEXT,(LPCTSTR)m_strText,StrFromResID(IDS_CMDPLANE_TEXT));
 	param->AddFontNameParam(PF_FONT,m_sTxtSettings.strFontName,StrFromResID(IDS_CMDPLANE_FONT));

	param->AddParam(PF_CHARH,(double)m_sTxtSettings.fHeight,StrFromResID(IDS_CMDPLANE_CHARH));
	param->AddParam(PF_CHARW,(double)m_sTxtSettings.fWidScale,StrFromResID(IDS_CMDPLANE_CHARWS));
	param->AddParam(PF_CHARI,(double)m_sTxtSettings.fCharIntervalScale,StrFromResID(IDS_CMDPLANE_CHARIS));
	param->AddParam(PF_LINEI,(double)m_sTxtSettings.fLineSpacingScale,StrFromResID(IDS_CMDPLANE_LINEIS));
	param->BeginOptionParam(PF_SHRUG,StrFromResID(IDS_CMDPLANE_SHRUG));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGN),SHRUGN,' ',m_sTxtSettings.nInclineType==SHRUGN);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGL),SHRUGL,' ',m_sTxtSettings.nInclineType==SHRUGL);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGR),SHRUGR,' ',m_sTxtSettings.nInclineType==SHRUGR);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGU),SHRUGU,' ',m_sTxtSettings.nInclineType==SHRUGU);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGD),SHRUGD,' ',m_sTxtSettings.nInclineType==SHRUGD);		
		
	param->EndOptionParam();
	
	param->AddParam(PF_SHRUGA,(double)m_sTxtSettings.fInclineAngle,StrFromResID(IDS_CMDPLANE_SHRUGA));
	param->BeginOptionParam(PF_LAYOUT,StrFromResID(IDS_CMDPLANE_LAYOUT));
	
	param->AddOption(StrFromResID(IDS_CMDPLANE_SINGLEPT),singPt,' ',m_sTxtSettings.nPlaceType==singPt);
	param->AddOption(StrFromResID(IDS_CMDPLANE_MULTIPT),mutiPt,' ',m_sTxtSettings.nPlaceType==mutiPt);
	param->AddOption(StrFromResID(IDS_CMDPLANE_BYLINES),byLineH,' ',m_sTxtSettings.nPlaceType==byLineH);
	param->AddOption(StrFromResID(IDS_CMDPLANE_BYLINES2),byLineV,' ',m_sTxtSettings.nPlaceType==byLineV);
	param->AddOption(StrFromResID(IDS_CMDPLANE_BYLINESGRID),byLineGridH,' ',m_sTxtSettings.nPlaceType==byLineGridH);
	param->AddOption(StrFromResID(IDS_CMDPLANE_BYLINES2GRID),byLineGridV,' ',m_sTxtSettings.nPlaceType==byLineGridV);
	param->EndOptionParam();
	
	param->BeginOptionParam(PF_ALIGNTYPE,StrFromResID(IDS_CMDPLANE_ALIGNTYPE));
	param->AddOption(StrFromResID(IDS_TEXTALIGNLT),TAH_LEFT|TAV_TOP,' ',m_sTxtSettings.nAlignment==(TAH_LEFT|TAV_TOP));
	param->AddOption(StrFromResID(IDS_TEXTALIGNLM),TAH_LEFT|TAV_MID,' ',m_sTxtSettings.nAlignment==(TAH_LEFT|TAV_MID));
	param->AddOption(StrFromResID(IDS_TEXTALIGNLB),TAH_LEFT|TAV_BOTTOM,' ',m_sTxtSettings.nAlignment==(TAH_LEFT|TAV_BOTTOM));
	param->AddOption(StrFromResID(IDS_TEXTALIGNMT),TAH_MID|TAV_TOP,' ',m_sTxtSettings.nAlignment==(TAH_MID|TAV_TOP));
	param->AddOption(StrFromResID(IDS_TEXTALIGNMM),TAH_MID|TAV_MID,' ',m_sTxtSettings.nAlignment==(TAH_MID|TAV_MID));
	param->AddOption(StrFromResID(IDS_TEXTALIGNMB),TAH_MID|TAV_BOTTOM,' ',m_sTxtSettings.nAlignment==(TAH_MID|TAV_BOTTOM));
	param->AddOption(StrFromResID(IDS_TEXTALIGNRT),TAH_RIGHT|TAV_TOP,' ',m_sTxtSettings.nAlignment==(TAH_RIGHT|TAV_TOP));
	param->AddOption(StrFromResID(IDS_TEXTALIGNRM),TAH_RIGHT|TAV_MID,' ',m_sTxtSettings.nAlignment==(TAH_RIGHT|TAV_MID));
	param->AddOption(StrFromResID(IDS_TEXTALIGNRB),TAH_RIGHT|TAV_BOTTOM,' ',m_sTxtSettings.nAlignment==(TAH_RIGHT|TAV_BOTTOM));
	param->AddOption(StrFromResID(IDS_TEXTALIGN_DOT),TAH_DOT,' ',m_sTxtSettings.nAlignment==TAH_DOT);
	param->EndOptionParam();

	param->AddParam("Bold",(bool)m_sTxtSettings.IsBold(),StrFromResID(IDS_BOLD));
	param->AddParam("Underline",(bool)m_sTxtSettings.IsUnderline(),StrFromResID(IDS_UNDERLINE));

}


void CDrawTextCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_TEXT,var) )
	{
		m_strText = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_FONT,var) )
	{
		if( var->m_variant.vt==VT_BSTR )
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
	}
	if( tab.GetValue(0,PF_CHARH,var) )
	{
		m_sTxtSettings.fHeight = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CHARW,var) )
	{
		m_sTxtSettings.fWidScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CHARI,var) )
	{
		m_sTxtSettings.fCharIntervalScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_LINEI,var) )
	{
		m_sTxtSettings.fLineSpacingScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_SHRUG,var) )
	{
		m_sTxtSettings.nInclineType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_SHRUGA,var) )
	{
		m_sTxtSettings.fInclineAngle = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_LAYOUT,var) )
	{
		m_sTxtSettings.nPlaceType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_ALIGNTYPE,var) )
	{
		m_sTxtSettings.nAlignment = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"Bold",var) )
	{
		m_sTxtSettings.SetBold((bool)(_variant_t)*var);
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"Underline",var) )
	{
		m_sTxtSettings.SetUnderline((bool)(_variant_t)*var);
		SetSettingsModifyFlag();
	}
	CDrawCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////
// CContourTextCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CContourTextCommand,CEditCommand)

CContourTextCommand::CContourTextCommand()
{
	m_nStep = -1;
	m_nDigitNum = 2;
	m_strTextLayerCode.Format("%s","");
	m_nMode = 0;
	m_nTextCoverType = COVERTYPE_NONE;
	strcat(m_strRegPath,"\\ContourText");

	m_fExtendDis = 0;
}

CContourTextCommand::~CContourTextCommand()
{

}



CString CContourTextCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CONTOURTEXT);
}

void CContourTextCommand::Start()
{
 	if( !m_pEditor )return;	
	
	m_nMode = 0;
	m_nDigitNum = 2;

	m_strTextLayerCode = StrFromResID(IDS_DEFLAYER_NAMET);
	m_strContourLayerCode = StrFromResID(IDS_INDEX_CONTOUR);	
	
	CEditCommand::Start();	

	if (m_nMode==modeLinesection)
	{
		m_pEditor->CloseSelector();
	}
	else
	{
		m_pEditor->OpenSelector(SELMODE_SINGLE);
	}
}

void CContourTextCommand::Abort()
{	

	m_pEditor->DeselectAll();
	
	CEditCommand::Abort();
	
	
	m_pEditor->UpdateDrag(ud_ClearDrag);
}
 
void CContourTextCommand::PtReset(PT_3D &pt)
{
	if( IsProcOver(this) )return;
	Abort();
	CEditCommand::PtReset(pt);
}

void CContourTextCommand::PtMove(PT_3D &pt)
{
	if(m_nStep==1&&m_nMode==modeLinesection)
	{
		GrBuffer buf;
		buf.BeginLineString(0,0,0);
		buf.MoveTo(&m_ptDragStart);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);		
	}
	
	CEditCommand::PtMove(pt);
}
BOOL CContourTextCommand::FilterFeature(LONG_PTR id)
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
		if( !CheckNameForLayerCode(pDS,pLayer->GetName(),StrFromResID(IDS_STD_CONTOUR)) && !CheckNameForLayerCode(pDS,pLayer->GetName(),StrFromResID(IDS_INDEX_CONTOUR)))
			return FALSE;
		if (m_pCurFtr != NULL && fabs(m_pCurFtr->GetGeometry()->GetDataPoint(0).z-po->GetDataPoint(0).z) < 1e-6)
			return FALSE;
		
	}
	return TRUE;
}


CFeature* CContourTextCommand::CreateText()
{
	const CShapeLine *pShape = m_pCurFtr->GetGeometry()->GetShape();
	if (!pShape) return NULL;

	CArray<PT_3DEX,PT_3DEX> arrPts,arrPts0;
	pShape->GetPts(arrPts0);		
	int size = GraphAPI::GKickoffSamePoints(arrPts0.GetData(),arrPts0.GetSize());
	arrPts.SetSize(size);
	for (int i=0; i<size; i++)
	{
		COPY_3DPT(arrPts[i],arrPts0[i]);
		arrPts[i].pencode = arrPts0[i].pencode;
	}
	
	PT_3D ptIntersect, ptDir;
	double pos;

	if (m_nMode == modeLinesection)
	{
		PT_3DEX line[2];
		line[0] = PT_3DEX(m_ptDragStart,penLine);
		line[1] = PT_3DEX(m_ptDragEnd,penLine);

		if (GetIntersectPos(arrPts.GetData(),arrPts.GetSize(),line,&pos,NULL))
		{
			if (fabs(pos - int(pos)) < 1e-6)
				pos++;

			int index = int(pos);
			double len = pos - index;
			if (fabs(pos-(size-1)) <= 1e-4)
			{
				ptIntersect = arrPts[size-1];
				ptDir.x = 2*arrPts[size-1].x - arrPts[size-2].x;
				ptDir.y = 2*arrPts[size-1].y - arrPts[size-2].y;
				ptDir.z = 2*arrPts[size-1].z - arrPts[size-2].z;
			}
			else
			{
				ptIntersect.x = arrPts[index].x + (arrPts[index+1].x - arrPts[index].x)*len;
				ptIntersect.y = arrPts[index].y + (arrPts[index+1].y - arrPts[index].y)*len;
				ptIntersect.z = arrPts[index].z + (arrPts[index+1].z - arrPts[index].z)*len;
				
				ptDir = arrPts[index+1];
			}

		}
		else
		{
			return NULL;
		}
	}
	else if (m_nMode == modeSinglePoint)
	{
		int pos1;
		
		PT_3DEX ret;
		GraphAPI::GGetNearstDisPToPointSet2D(arrPts.GetData(),arrPts.GetSize(),PT_3DEX(m_ptDragStart,penLine),ret,&pos1);

		if (abs(pos1 - int(pos1)) < 1e-6)
			pos1++;

		pos = pos1;
		ptIntersect = ret;

		if (pos1 == size-1)
		{
			ptDir.x = 2*arrPts[size-1].x - arrPts[size-2].x;
			ptDir.y = 2*arrPts[size-1].y - arrPts[size-2].y;
			ptDir.z = 2*arrPts[size-1].z - arrPts[size-2].z;
		}
		else
		{
			ptDir = arrPts[pos1+1];
		}


	}

	PT_3D ptAnother;
	if ( FindAnotherNearestPt(ptIntersect,&ptAnother) )
	{
		// 计算相对位置
		PT_3D dirPts[3];
		dirPts[0] = ptAnother;
		dirPts[1] = ptIntersect;
		dirPts[2] = ptDir;
		int dir = GraphAPI::GIsClockwise(dirPts,3);
		// 调整
		if ( (dir == TRUE && ptIntersect.z < ptAnother.z) || (dir == FALSE && ptIntersect.z > ptAnother.z))	
		{
			if (fabs(pos) <= 1e-4)
			{
				ptDir.x = 2*arrPts[0].x - arrPts[1].x;
				ptDir.y = 2*arrPts[0].y - arrPts[1].y;
				ptDir.z = 2*arrPts[0].z - arrPts[1].z;
			}
			else
			{
				int prePos = ceil(pos-1);
				if (prePos >= 0 && prePos < size)
				{
					ptDir = arrPts[ceil(pos-1)];
				}
				
			}
			
		}
	}
	
	PT_3DEX arrText[2];
	arrText[0] = PT_3DEX(ptIntersect,penLine);
	arrText[1] = PT_3DEX(ptDir,penLine);
	
	// 创建文本
	//CFeature *pNewFtr = MarkText(arrText,2,arrPts[0].z);
	//return pNewFtr;
	CDlgDataSource *pDS = GETDS(m_pEditor);
	if (!pDS) return NULL;
	
	CString strLayerName = m_strTextLayerCode;
	CFtrLayer *pFtrLay = pDS->GetFtrLayer(strLayerName);
	
	if (pFtrLay == NULL)
	{				
		pFtrLay = pDS->CreateFtrLayer(strLayerName);
		if (!pFtrLay)  return NULL;
		pFtrLay->SetColor(RGB(255,0,0));
		pDS->AddFtrLayer(pFtrLay);			
	}
	
	CFeature *pTextFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOTEXT);
	if (pTextFtr)
	{
		CGeoText *pText = (CGeoText*)pTextFtr->GetGeometry();
		if (pText)
		{
			pText->SetColor(COLOUR_BYLAYER/*RGB(255,255,255)*/);
			
			pText->CreateShape(arrText,2);
			
			CString strFormat, str;
			strFormat.Format(_T("%%.%df"), m_nDigitNum);
			str.Format(strFormat,arrPts[0].z);
			
			pText->SetText(str);
			
			m_sTxtSettings.nAlignment = TAH_MID|TAV_MID;
			m_sTxtSettings.nPlaceType = byLineH;
			pText->SetSettings(&m_sTxtSettings);
			pText->m_nCoverType = m_nTextCoverType;
			pText->m_fExtendDis = m_fExtendDis;

			TextSettings set;				
			set.CopyFrom(&m_sTxtSettings);
			
			double ex[4], ey[4];
			g_Fontlib.SetSettings(&set);
			g_Fontlib.GetEnvlope(str,ex,ey);
			
			double len = sqrt((ex[1]-ex[0])*(ex[1]-ex[0])+(ey[1]-ey[0])*(ey[1]-ey[0]))*pDS->GetSymbolDrawScale();
			
			double len1 = GraphAPI::GGet2DDisOf2P(arrText[0],arrText[1]);
			
			double a = (len/2)/len1;
			
			// 修正，相交点为文字的中点
			PT_3DEX arrTextNew[2];
			arrTextNew[1].x = arrText[0].x + (arrText[1].x - arrText[0].x) * a;
			arrTextNew[1].y = arrText[0].y + (arrText[1].y - arrText[0].y) * a;
			arrTextNew[1].z = arrText[0].z + (arrText[1].z - arrText[0].z) * a;
			arrTextNew[1].pencode = penLine;
			
			arrTextNew[0].x = 2 * arrText[0].x - arrTextNew[1].x;
			arrTextNew[0].y = 2 * arrText[0].y - arrTextNew[1].y;
			arrTextNew[0].z = 2 * arrText[0].z - arrTextNew[1].z;
			arrTextNew[0].pencode = penLine;
			
			memcpy(arrText,arrTextNew,sizeof(PT_3DEX)*2);

			{
				PT_3DEX pt = arrText[1];
				PT_3DEX ret = pt;
				double dis = GraphAPI::GGetNearstDisPToPointSet2D(arrPts.GetData(),arrPts.GetSize(),pt,ret,NULL);
				if (dis < DBL_MAX)
				{
					COPY_2DPT(arrText[1],ret);
				}										
				
				pText->CreateShape(arrText,2);
			}			
			
			int layid = pFtrLay->GetID();
			m_pEditor->AddObject(pTextFtr,layid);
		}
	}
	
	return pTextFtr;
							
}

BOOL CContourTextCommand::FindAnotherNearestPt(PT_3D pt, PT_3D *ret)
{
	CDlgDataQuery* pQuery = (CDlgDataQuery*)PDOC(m_pEditor)->GetDataQuery();
	CDlgDataQuery::FilterFeatureItem oldItem;
	CDlgDataQuery::FilterFeatureItem newItem((CSearcherFilter*)this, (PFilterFunc)&CContourTextCommand::FilterFeature);
	oldItem = pQuery->SetFilterFeature(newItem);
			
	PT_3D cltPt;
	(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&pt,&cltPt);

	CFeature *pFindFtr = NULL;
	double r;
	int  nObj = ((CDlgDataQuery*)(PDOC(m_pEditor)->GetDataQuery()))->FindNearestObjectByObjNum(cltPt,2,m_pEditor->GetCoordWnd().m_pSearchCS,FALSE,&r);
	if( nObj>=2 )	
	{
		const CPFeature *ftr = PDOC(m_pEditor)->GetDataQuery()->GetFoundHandles(nObj);
		double fMinLen = -1;
		for (int i=0; i<nObj; i++)
		{
			if (ftr[i] != m_pCurFtr)
			{
				CArray<PT_3DEX,PT_3DEX> arr;
				ftr[i]->GetGeometry()->GetShape(arr);
				PT_3DEX ret0;
				double dis = GraphAPI::GGetNearstDisPToPointSet2D(arr.GetData(),arr.GetSize(),PT_3DEX(pt,penMove),ret0,NULL);
				if (fMinLen < 0 || dis < fMinLen)
				{
					if (ret)
					{
						*ret = ret0;
					}
					 
					pFindFtr = ftr[i];
					fMinLen = dis;
				}
			}
		}
	}	

	pQuery->SetFilterFeature(oldItem);

	return pFindFtr!=NULL?TRUE:FALSE;
}

CFeature* CContourTextCommand::MarkText(PT_3DEX *pts, int num, float height)
{
	CDlgDataSource *pDS = GETDS(m_pEditor);
	if (!pDS) return FALSE;
	
	CString strLayerName = m_strTextLayerCode;
	CFtrLayer *pFtrLay = pDS->GetFtrLayer(strLayerName);
	
	if (pFtrLay == NULL)
	{				
		pFtrLay = pDS->CreateFtrLayer(strLayerName);
		if (!pFtrLay)  return FALSE;
		pFtrLay->SetColor(RGB(255,0,0));
		pDS->AddFtrLayer(pFtrLay);			
	}
	
	CFeature *pTextFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOTEXT);
	if (pTextFtr)
	{
		CGeoText *pText = (CGeoText*)pTextFtr->GetGeometry();
		if (pText)
		{
			pText->SetColor(RGB(255,255,255));
			
			pText->CreateShape(pts,num);
			
			CString strFormat, str;
			strFormat.Format(_T("%%.%df"), m_nDigitNum);
			str.Format(strFormat,height);

			pText->SetText(str);
			
			m_sTxtSettings.nAlignment = TAH_MID|TAV_MID;
			m_sTxtSettings.nPlaceType = byLineH;
			pText->SetSettings(&m_sTxtSettings);
			
			int layid = pFtrLay->GetID();
			m_pEditor->AddObject(pTextFtr,layid);
		}
	}
	
	return pTextFtr;
}

void CContourTextCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{
		if(m_nMode==modeLinesection)
		{			
			m_ptDragStart = pt;
			m_nStep = 1;
			GotoState(PROCSTATE_PROCESSING);		
		}
		else if (m_nMode == modeSinglePoint)
		{
			if( (flag&SELSTAT_POINTSEL)==0 && (flag&SELSTAT_DRAGSEL)==0 )
				return;
			
			m_ptDragStart = pt;

			int nsel;
			const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
			if( nsel!=1 )
				return;		
			m_idContour = handles[0];
			
			CGeometry *pObj = HandleToFtr(m_idContour)->GetGeometry();
			
			//不是等高线
			if( !CModifyZCommand::CheckObjForContour(pObj) )
			{
				m_pEditor->UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)StrFromResID(IDS_CMDTIP_NOTCONTOUR));
				return;
			}
			if (pObj->GetDataPointSum()==1)
			{
				return;
			}
			
			m_pEditor->CloseSelector();
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
		}
	}
	else if (m_nStep==1)
	{
		m_nStep = 2;
	}
	
	if( m_nStep==2 )
	{		

		if (m_nMode == modeLinesection)
		{
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_ptDragEnd = pt;
			PT_3D Pt[2];	
			(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_ptDragStart,Pt);
			(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_ptDragEnd,Pt+1);
			
			Envelope el;
			el.CreateFromPts(Pt,2);
			m_pEditor->GetDataQuery()->FindObjectInRect(el,(m_pEditor->GetCoordWnd()).m_pSearchCS);
			int num1 ;
			const CPFeature *ftr = (m_pEditor->GetDataQuery())->GetFoundHandles(num1);

			CArray<CFeature*,CFeature*> arrFtrs;
			for (int i=0; i<num1; i++)
			{
				arrFtrs.Add(ftr[i]);
			}
			
			CDlgDataSource *pDS = GETDS(m_pEditor);
			if (!pDS) 
			{
				Abort();
				return;
			}
			
			CUndoFtrs undo(m_pEditor,Name());	
			for(i=0; i<num1; i++)
			{
				m_pCurFtr = arrFtrs[i];
				CGeometry *pObj = (CGeoCurve*)m_pCurFtr->GetGeometry();
				if( !pObj )continue;
				
				if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
				if( !CModifyZCommand::CheckObjForContour(pObj) )continue;
				if( !CAutoLinkContourCommand::CheckObjForLayerCode(GETDS(m_pEditor),m_pCurFtr,(LPCTSTR)m_strContourLayerCode) )
					continue;
				
				CFeature *pNewFtr = CreateText();
				if (pNewFtr)
				{
					undo.AddNewFeature(FtrToHandle(pNewFtr));
				}
								
			}

			undo.Commit();
						
			Finish();
			m_nStep = 3;	
			
		
		}
		else if (m_nMode == modeSinglePoint)
		{
			// 转化为两点			
// 			m_ptDragStart.y -= 5;
// 			m_ptDragEnd = m_ptDragStart;
// 			m_ptDragEnd.y += 10;

			CUndoFtrs undo(m_pEditor,Name());	
			m_pCurFtr = HandleToFtr(m_idContour);
			
			CFeature *pNewFtr = CreateText();
			if (pNewFtr)
			{
				undo.AddNewFeature(FtrToHandle(pNewFtr));
			}
			
			undo.Commit();
			
			Finish();
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			
			m_pEditor->RefreshView();	

			m_nStep = 3;
		}
	}
	CEditCommand::PtClick(pt,flag);
}

void CContourTextCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue(PF_CONTOURTEXT_WAY,&CVariantEx(var));
	var = (_bstr_t)(m_strContourLayerCode);
	tab.AddValue(PF_CONTOURCODE,&CVariantEx(var));
	var = (_bstr_t)(m_strTextLayerCode);
	tab.AddValue(PF_CONTOURTEXTCODE,&CVariantEx(var));
	var = (long)(m_nDigitNum);
	tab.AddValue(PF_CONTOURDIGITNUM,&CVariantEx(var));
	var = (_bstr_t)(m_sTxtSettings.strFontName);
	tab.AddValue(PF_FONT,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fHeight);
	tab.AddValue(PF_CHARH,&CVariantEx(var));

	var = (_variant_t)(long)(m_nTextCoverType);//是否压盖
	tab.AddValue(PF_GEOCOVERTYPE,&CVariantEx(var));
	var = (_variant_t)(long)(m_fExtendDis);//压盖扩展
	tab.AddValue(PF_EXTENDDIS,&CVariantEx(var));
	
}

int CContourTextCommand::FindIndex(const CStringArray &arrStr,CString str)
{
	for (int i= 0;i<arrStr.GetSize();i++)
	{
		if(strcmp((LPCTSTR)arrStr[i],(LPCTSTR)str)==0)
			return i;
	}
	return -1;
}

void CContourTextCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{	

 	param->SetOwnerID("ContourTextCommand",Name());
	param->BeginOptionParam(PF_CONTOURTEXT_WAY,StrFromResID(IDS_CMDPLANE_BY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SINGLEPT),0,' ',m_nMode==modeSinglePoint);
	param->AddOption(StrFromResID(IDS_CMDPLANE_LINESECTION),1,' ',m_nMode==modeLinesection);
	param->EndOptionParam();
	if (m_nMode == modeLinesection || bForLoad)
	{
		param->AddLayerNameParam(PF_CONTOURCODE,LPCTSTR(m_strContourLayerCode),StrFromResID(IDS_CMDPLANE_CONTOURLAYER));
	}
	param->AddLayerNameParam(PF_CONTOURTEXTCODE,LPCTSTR(m_strTextLayerCode),StrFromResID(IDS_CMDPLANE_HPC));
	param->AddParam(PF_CONTOURDIGITNUM,int(m_nDigitNum),StrFromResID(IDS_CMDPLANE_DIGITNUM));
  	param->AddFontNameParam(PF_FONT,m_sTxtSettings.strFontName,StrFromResID(IDS_CMDPLANE_FONT));

	param->AddParam(PF_CHARH,(double)m_sTxtSettings.fHeight,StrFromResID(IDS_CMDPLANE_CHARH));
	
	//文字压盖
	param->BeginOptionParam(PF_GEOCOVERTYPE,StrFromResID(IDS_CMDPLANE_GEOCOVERTYPE));
	param->AddOption(StrFromResID(IDS_COVERTYPE_NONE),COVERTYPE_NONE,' ',m_nTextCoverType==COVERTYPE_NONE);
	param->AddOption(StrFromResID(IDS_COVERTYPE_RECT),COVERTYPE_RECT,' ',m_nTextCoverType==COVERTYPE_RECT);
	param->AddOption(StrFromResID(IDS_COVERTYPE_CIRCLE),COVERTYPE_CIRCLE,' ',m_nTextCoverType==COVERTYPE_CIRCLE);
	param->EndOptionParam();
	param->AddParam(PF_EXTENDDIS,(double)m_fExtendDis,StrFromResID(IDS_CMDPLANE_GEOEXTENDDISTANCE));
}



void CContourTextCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	if( tab.GetValue(0,PF_CONTOURTEXT_WAY,var) )
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
		{
			Abort();
			return;
		}
	}

	if( tab.GetValue(0,PF_CONTOURCODE,var) )
	{
		m_strContourLayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
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

	if (tab.GetValue(0,PF_GEOCOVERTYPE,var))
	{
		m_nTextCoverType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_EXTENDDIS,var) )
	{
		m_fExtendDis = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}




//////////////////////////////////////////////////////////////////////
// CNumberTextCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CNumberTextCommand,CDrawCommand)

CNumberTextCommand::CNumberTextCommand()
{
	m_pFtr = NULL;
	m_bDrag = FALSE;
	m_pSel = NULL;

	m_nNumber = 1;

}

CNumberTextCommand::~CNumberTextCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if(m_pSel)
		delete m_pSel;
}



CString CNumberTextCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_NUMBERTEXT);
}

void CNumberTextCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;

	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOTEXT);
	if(!m_pFtr) return;	
	m_pEditor->CloseSelector();

	m_nNumber = 1;

	LOGFONT logfont;
    logfont.lfCharSet = DEFAULT_CHARSET;
    strcpy(logfont.lfFaceName,"");
	HDC hDC = ::CreateCompatibleDC(NULL);
	
	::EnumFontFamiliesEx(hDC,&logfont,
		(FONTENUMPROC)EnumFontFamProc,(LPARAM)&m_arrFont,0);
	::DeleteDC(hDC);

	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
		
}

void CNumberTextCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
DrawingInfo CNumberTextCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX,PT_3DEX> arr;
	m_pFtr->GetGeometry()->GetShape(arr);
	return DrawingInfo(m_pFtr,arr);
}

void CNumberTextCommand::Abort()
{	
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	if(m_pFtr) 
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
	CDrawCommand::Abort();
}


void CNumberTextCommand::Back()
{	
	CDrawCommand::Back();
}



CFtrLayer *CNumberTextCommand::GetLayer(LPCTSTR fid)
{
	CDlgDoc *pDoc = (CDlgDoc*)(m_pEditor);
	CFtrLayer *pLayer = NULL;
	if( fid!=NULL )
		pLayer = pDoc->GetDlgDataSource()->GetFtrLayer(fid);
	
	if( pLayer==NULL )
	{
		pLayer = new CFtrLayer();
		pLayer->SetName(fid);
		
		pDoc->AddFtrLayer(pLayer);
	}
	
	return pLayer;
}

void CNumberTextCommand::FinishObject()
{
	if (m_pEditor)
	{
		CFtrLayer *pLayer = GetLayer(m_strLayerCode);

		if( !AddObject(m_pFtr,pLayer->GetID()) )
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor,Name());
		undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
		
		undo.Commit();

		m_pEditor->UpdateDrag(ud_ClearDrag);
	}
	
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}

	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
}


void CNumberTextCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr || GetState()==PROCSTATE_FINISHED )return;

	if( m_strLayerCode.IsEmpty() )
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	m_strText.Format("%d",m_nNumber);

	((CGeoText*)m_pFtr->GetGeometry())->SetText(m_strText);
	((CGeoText*)m_pFtr->GetGeometry())->SetSettings(&m_sTxtSettings);
	
	//如果文本内容为空，则return;
	CString str=((CGeoText*)m_pFtr->GetGeometry())->GetText();
	if(str.IsEmpty())
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}
	//获得文本设置选项
	TEXT_SETTINGS0 settings;
	((CGeoText*)m_pFtr->GetGeometry())->GetSettings(&settings);

	CArray<PT_3DEX,PT_3DEX> arr;
    m_pFtr->GetGeometry()->GetShape(arr);

	PT_3DEX t;
	t.x = pt.x;
	t.y = pt.y;
	t.z = pt.z;
	t.pencode= penLine;
	arr.Add(t);
    m_pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());

 	FinishObject();

	m_pFtr = NULL;
	
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOTEXT);
	if(!m_pFtr) return;	

	m_nNumber++;	

	SetSettingsModifyFlag();
	m_pEditor->UIRequest(UIREQ_TYPE_SAVEFILEPARAM,0);	

	CUIParam param;
	FillShowParams(&param);
	m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, (LONG_PTR)&param);

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);	

 		
	return;
}

void CNumberTextCommand::PtMove(PT_3D &pt)
{
	CDrawCommand::PtMove(pt);
}


void CNumberTextCommand::PtReset(PT_3D &pt)
{
	Abort();
}

int CNumberTextCommand::FindIndex(const CStringArray &arrStr,CString str)
{
	for (int i= 0;i<arrStr.GetSize();i++)
	{
		if(strcmp((LPCTSTR)arrStr[i],(LPCTSTR)str)==0)
			return i;
	}
	return -1;
}


void CNumberTextCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nNumber);
	tab.AddValue("Number",&CVariantEx(var));

	var = (LPCTSTR)(m_strLayerCode);
	tab.AddValue("PlaceLayer",&CVariantEx(var));

	var = (_variant_t)(m_sTxtSettings.strFontName);

	tab.AddValue(PF_FONT,&CVariantEx(var));
	var = (double)(m_sTxtSettings.fHeight);
	tab.AddValue(PF_CHARH,&CVariantEx(var));

	var = (_variant_t)(long)(m_sTxtSettings.nAlignment);
	tab.AddValue(PF_ALIGNTYPE,&CVariantEx(var));
}


void CNumberTextCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("NumberTextCommand",Name());
	param->AddParam("Number",(long)m_nNumber,StrFromResID(IDS_PLANENAME_CURNUMBER));

	param->AddLayerNameParam("PlaceLayer",(LPCTSTR)m_strLayerCode,StrFromResID(IDS_PLANENAME_PLACELAYER));
	
	param->AddFontNameParam(PF_FONT,(LPCTSTR)m_sTxtSettings.strFontName,StrFromResID(IDS_CMDPLANE_FONT));


	param->AddParam(PF_CHARH,(double)m_sTxtSettings.fHeight,StrFromResID(IDS_CMDPLANE_CHARH));

	
	param->BeginOptionParam(PF_ALIGNTYPE,StrFromResID(IDS_CMDPLANE_ALIGNTYPE));
	param->AddOption(StrFromResID(IDS_TEXTALIGNLT),TAH_LEFT|TAV_TOP,' ',m_sTxtSettings.nAlignment==(TAH_LEFT|TAV_TOP));
	param->AddOption(StrFromResID(IDS_TEXTALIGNMT),TAH_MID|TAV_TOP,' ',m_sTxtSettings.nAlignment==(TAH_MID|TAV_TOP));
	param->AddOption(StrFromResID(IDS_TEXTALIGNRT),TAH_RIGHT|TAV_TOP,' ',m_sTxtSettings.nAlignment==(TAH_RIGHT|TAV_TOP));
	param->AddOption(StrFromResID(IDS_TEXTALIGNLM),TAH_LEFT|TAV_MID,' ',m_sTxtSettings.nAlignment==(TAH_LEFT|TAV_MID));
	param->AddOption(StrFromResID(IDS_TEXTALIGNMM),TAH_MID|TAV_MID,' ',m_sTxtSettings.nAlignment==(TAH_MID|TAV_MID));
	param->AddOption(StrFromResID(IDS_TEXTALIGNRM),TAH_RIGHT|TAV_MID,' ',m_sTxtSettings.nAlignment==(TAH_RIGHT|TAV_MID));
	param->AddOption(StrFromResID(IDS_TEXTALIGNLB),TAH_LEFT|TAV_BOTTOM,' ',m_sTxtSettings.nAlignment==(TAH_LEFT|TAV_BOTTOM));
	param->AddOption(StrFromResID(IDS_TEXTALIGNMB),TAH_MID|TAV_BOTTOM,' ',m_sTxtSettings.nAlignment==(TAH_MID|TAV_BOTTOM));
	param->AddOption(StrFromResID(IDS_TEXTALIGNRB),TAH_RIGHT|TAV_BOTTOM,' ',m_sTxtSettings.nAlignment==(TAH_RIGHT|TAV_BOTTOM));
	param->EndOptionParam();
}


void CNumberTextCommand::SetParams(CValueTable& tab,BOOL bInit)
{	

	const CVariantEx *var;
	
	if( tab.GetValue(0,"Number",var) )
	{
		m_nNumber = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	if( tab.GetValue(0,"PlaceLayer",var) )
	{
		m_strLayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_FONT,var) )
	{
		CString strFont = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		strcpy(m_sTxtSettings.strFontName,strFont);
		
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_CHARH,var) )
	{
		m_sTxtSettings.fHeight = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_ALIGNTYPE,var) )
	{
		m_sTxtSettings.nAlignment = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CDrawCommand::SetParams(tab,bInit);
}





//////////////////////////////////////////////////////////////////////
// CAutoAdjustParallelCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CAutoAdjustParallelCommand,CEditCommand)

CAutoAdjustParallelCommand::CAutoAdjustParallelCommand()
{
}

CAutoAdjustParallelCommand::~CAutoAdjustParallelCommand()
{
}

CString CAutoAdjustParallelCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_ADJUST_CL);
}

void CAutoAdjustParallelCommand::PtMove(PT_3D &pt)
{
}

void CAutoAdjustParallelCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CAutoAdjustParallelCommand::Abort()
{
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CAutoAdjustParallelCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CAutoAdjustParallelCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;		
	var = (_bstr_t)(LPCTSTR)m_strParallelCode;
	tab.AddValue(CADJUST_CL_PARA, &CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strCenterLineCode;
	tab.AddValue(CADJUST_CL_CL, &CVariantEx(var));
}

void CAutoAdjustParallelCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CADJUST_CL_PARA,var) )
	{
		m_strParallelCode = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,CADJUST_CL_CL,var) )
	{
		m_strCenterLineCode = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CAutoAdjustParallelCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CAutoAdjustParallelCommand"),Name());	
	param->AddLayerNameParam(CADJUST_CL_PARA, m_strParallelCode, StrFromResID(IDS_PARALLEL_LAYER));
	param->AddLayerNameParam(CADJUST_CL_CL, m_strCenterLineCode, StrFromResID(IDS_CENTER_LINE_LAYER));
}


void CAutoAdjustParallelCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_strParallelCode.IsEmpty() || m_strCenterLineCode.IsEmpty() ) return;
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	

	//1.挑出所有与参数中层码对应的平行线地物
	CFtrLayerArray para_layers;
	pDS->GetFtrLayersByNameOrCode(m_strParallelCode,para_layers);
	int num = para_layers.GetSize();
	if(num <= 0 ) return;

	CFtrArray paraObjs;
	for(int i = 0; i < num; i++)
	{
		CFtrLayer *pLayer = para_layers.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
				continue;
			paraObjs.Add( pFtr );
		}
	}

	//2.在平行线附近搜索符合条件的中心线
	int n = paraObjs.GetSize();

	CUndoFtrs undo(m_pEditor, "Adjust CenterLine");
	GProgressStart(n);
	for(i = 0; i < n; i++)
	{
		GProgressStep();
		CFeature* pFtr = paraObjs.GetAt(i);
		if(!pFtr) continue;

		CFeature* pCenterLine = NULL;
		if( !IsCenterLineValid(pFtr, pCenterLine) ) continue;

		//3.删除找到的中心线，生成新的中心线
		CGeoParallel *pGeo = (CGeoParallel*)pFtr->GetGeometry();
		if( !pGeo ) continue;
		CGeoCurve *pNewGeo = pGeo->GetCenterlineCurve();
		if( !pNewGeo ) continue;
	//	long col = pCenterLine->GetGeometry()->GetColor();
	//	pNewGeo->SetColor( col );

		CFeature* pNew_CL = pCenterLine->Clone();
		if( !pNew_CL ) continue;
		pNew_CL->SetGeometry(pNewGeo);

		CValueTable table;
		table.BeginAddValueItem();
		pCenterLine->WriteTo(table);
		table.EndAddValueItem();

		table.DelField(FIELDNAME_SHAPE);
		table.DelField(FIELDNAME_GEOCURVE_CLOSED);
		table.DelField(FIELDNAME_FTRID);
		table.DelField(FIELDNAME_GEOCLASS);
		table.DelField(FIELDNAME_GEOTEXT_CONTENT);
		table.DelField(FIELDNAME_FTRDISPLAYORDER);
		pNew_CL->ReadFrom(table);

		if( m_pEditor->AddObject( pNew_CL, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pCenterLine))) )
		{
			GETXDS(m_pEditor)->CopyXAttributes(pCenterLine,pNew_CL);
			undo.AddNewFeature(FtrToHandle( pNew_CL ));
		}
		if( m_pEditor->DeleteObject( FtrToHandle(pCenterLine)) )
			undo.AddOldFeature(FtrToHandle( pCenterLine ));
	}
	GProgressEnd();

	undo.DelInvalidUndoInfo();
	undo.Commit();
	Finish();
}

BOOL CAutoAdjustParallelCommand::IsCenterLineValid(CFeature* pFtr, CFeature*& pCenterLine)
{
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	//1.算出平行线上所有点的坐标
	CArray<PT_3DEX,PT_3DEX> arrPts;
	CGeometry* pGeo = pFtr->GetGeometry();
	if(!pGeo)
		return FALSE;

	pGeo->GetShape( arrPts );
	int npt = arrPts.GetSize();
	if( npt < 2 )
		return FALSE;

	PT_3DEX* pts = new PT_3DEX[ npt ];
	double width = ((CGeoParallel*)pGeo)->GetWidth();
	GraphAPI::GGetParallelLine(arrPts.GetData(), npt, width, pts);
	for(int k = npt-1; k >= 0; k--)
	{
		arrPts.Add( pts[k] );
	}		
	delete [] pts;
	
	//2.搜索中心线
	Envelope e;
	e.CreateFromPts( arrPts.GetData(), arrPts.GetSize(), sizeof(PT_3DEX) );
	int n = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);
	const CPFeature *ftrs = pDQ->GetFoundHandles(n);


	CFtrArray Objs;
	//3.排除不符合要求的
	for(int i = 0; i < n; i++)
	{
		if( pFtr==ftrs[i] )
			continue;


		//比较层码
		CFtrLayer* pLayer = pDS->GetFtrLayerOfObject( ftrs[i] );
		CString strLayerName = pLayer->GetName();
		__int64 id = -1;
		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
		if (pScheme)
		{
			CSchemeLayerDefine *p_app_layer = pScheme->GetLayerDefine( strLayerName );
			if( p_app_layer )
				id = p_app_layer->GetLayerCode();
		}
		CString strID;
		strID.Format("%I64d", id);
		if( 0 != m_strCenterLineCode.CompareNoCase( strID ) && 0 != m_strCenterLineCode.CompareNoCase(strLayerName) )
			continue;

		//判断包裹在平行线内的长度是否达80%
		if( !IsMostWraped( ftrs[i], arrPts) )
			continue;

		Objs.Add( ftrs[i] );
	}


	//如果符合要求的有且只有一个，则取出这个中心线的指针
	if( 1 == Objs.GetSize() )
	{
		pCenterLine = Objs.GetAt(0);
		return TRUE;
	}

	return FALSE;
}

BOOL CAutoAdjustParallelCommand::IsMostWraped(CFeature* pLine, CArray<PT_3DEX,PT_3DEX>& para_pts)
{
	//1.计算结的全长
	CGeometry* pGeo = pLine->GetGeometry();
	if(!pGeo) return FALSE;
	const CShapeLine* pSL = pGeo->GetShape();
	if(!pSL) return FALSE;

	CArray<PT_3DEX,PT_3DEX> line_pts;
	pSL->GetPts( line_pts );
	int npt1 = line_pts.GetSize();

	PT_3DEX* line_pts_copy = new PT_3DEX[npt1];
	if(!line_pts_copy) return FALSE;
	memcpy( line_pts_copy, line_pts.GetData(), npt1*sizeof(PT_3DEX) );
	int m = GraphAPI::GKickoffSame2DPoints( line_pts_copy, npt1);
	if(m < 2) return FALSE;
	double all_len = GraphAPI::GGetAllLen2D( line_pts_copy, m);
	if( all_len < 1e-4 ) return FALSE;

	//2.计算被包裹的总长度
	int npt2 = para_pts.GetSize();
	PT_3DEX* para_pts_copy = new PT_3DEX[npt2];
	if(!para_pts_copy) return FALSE;
	memcpy( para_pts_copy, para_pts.GetData(), npt2*sizeof(PT_3DEX) );
	int n = GraphAPI::GKickoffSame2DPoints( para_pts_copy, npt2);

	double len_wraped  = 0;
	for(int i = 0; i < m-1; i++)
	{
		PT_3DEX p1 = line_pts_copy[i];
		PT_3DEX p2 = line_pts_copy[i+1];
		double len = GetWrapedLength(p1, p2, para_pts_copy,  n);
		len_wraped += len;
	}

	delete [] line_pts_copy;
	delete [] para_pts_copy;

	if( len_wraped/all_len > 0.8 )
		return TRUE;
	return FALSE;
}

double CAutoAdjustParallelCommand::GetWrapedLength(PT_3DEX p1, PT_3DEX p2, PT_3DEX* pts, int n)
{
	if( n < 3 ) return 0;

	CArray<double,double> idxs;
	double len_sum = 0;

	//算出线段与多边形的所有交点
	for(int i = 0; i < n; i++)
	{
		PT_3DEX p3, p4;
		p3 = pts[i];
		if( i == n-1 )
			p4 = pts[0];
		else
			p4 = pts[i+1];

		double t = -1;
		double x = 0, y = 0;
	//	if( GraphAPI::GGetLineIntersectLineSeg(p3.x, p3.y, p4.x, p4.y, p1.x, p1.y, p2.x, p2.y, &x, &y, &t) )
		if( GraphAPI::GGetLineIntersectLineSeg(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, &x, &y, &t) )
		{
			idxs.Add( t );
		}
	}

	if( 2 == GraphAPI::GIsPtInRegion(p1, pts, n) )
		idxs.Add( 0 );
	if( 2 == GraphAPI::GIsPtInRegion(p2, pts, n) )
		idxs.Add( 1 );

	int num = idxs.GetSize();
	if( num <= 0 ) return 0;

	//将所有交点从左到右排序
	double* pIdx = new double[num];
	if(!pIdx) return 0;
	memcpy( pIdx, idxs.GetData(), num*sizeof(double) );

	for(i = 0; i < num-1; i++)
	{
		int min_idx = i;
		for(int j = i+1; j < num; j++)
		{
			if( pIdx[j] < pIdx[i] )
				min_idx = j;
		}
		double temp = pIdx[i];
		pIdx[i] = pIdx[min_idx];
		pIdx[min_idx] = temp;
	}


	//从左到右两两配对，若这一对的中点，在多边形内，则求出这一对之间的距离
	for(i = 0; i < num-1; i++)
	{
		PT_3DEX ptS;
		ptS.x = p1.x + pIdx[i]*(p2.x-p1.x);
		ptS.y = p1.y + pIdx[i]*(p2.y-p1.y);

		PT_3DEX ptE;
		ptE.x = p1.x + pIdx[i+1]*(p2.x-p1.x);
		ptE.y = p1.y + pIdx[i+1]*(p2.y-p1.y);

		PT_3DEX ptMid;
		ptMid.x = 0.5*(ptS.x + ptE.x);
		ptMid.y = 0.5*(ptS.y + ptE.y);

		if( 2 == GraphAPI::GIsPtInRegion(ptMid, pts, n) )
		{
			double len = GraphAPI::GGet2DDisOf2P(ptS, ptE);
			len_sum += len;
		}
		i++;
	}

	delete [] pIdx;
	return len_sum;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDoubleLinesToSurfaceCommand,CEditCommand)

CDoubleLinesToSurfaceCommand::CDoubleLinesToSurfaceCommand()
{
}

CDoubleLinesToSurfaceCommand::~CDoubleLinesToSurfaceCommand()
{
}

CString CDoubleLinesToSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DLINE2SURFACE);
}

void CDoubleLinesToSurfaceCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		
		return;
	}
	
	CEditCommand::Start();
}

void CDoubleLinesToSurfaceCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue(PF_LAYERNAME,&CVariantEx(var));
}

void CDoubleLinesToSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DoubleLinesToSurface",Name());
	
	param->AddLayerNameParam(PF_LAYERNAME,(LPCTSTR)m_targetLayer,StrFromResID(IDS_CMDPLANE_SURFACELAYER),NULL,LAYERPARAMITEM_AREA);
}

void CDoubleLinesToSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_LAYERNAME,var) )
	{					
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimRight();
		m_targetLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CDoubleLinesToSurfaceCommand::DoubleLineToSurface(const FTR_HANDLE* handles,int num)
{
	CUndoFtrs undo(m_pEditor,Name());

	CFtrLayer *pTargetLayer = m_pEditor->GetDataSource()->GetFtrLayer(m_targetLayer);
	int layid = -1;
	CFeature *pTemp = NULL;
	if(pTargetLayer)
	{
		layid = pTargetLayer->GetID();
		pTemp = pTargetLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(), CLS_GEOSURFACE);
	}
	
	for (int i=0;i<num;i++)
	{
		CGeometry *pGeo = HandleToFtr(handles[i])->GetGeometry();
		if (!pGeo)
		{
			continue;
		}

		CGeometry* pObj1=NULL;
		CGeometry* pObj2=NULL;
		CArray<PT_3DEX,PT_3DEX> arrPts, pts;
		if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
		{
			((CGeoDCurve*)pGeo)->Separate(pObj1, pObj2);
			if(!pObj1 || !pObj2) continue;
			pObj1->GetShape(arrPts);
			pObj2->GetShape(pts);
			PT_3DEX p1 = arrPts[0];
			PT_3DEX p2 = pts[0];
			PT_3DEX p3 = arrPts[arrPts.GetSize()-1];
			PT_3DEX p4 = pts[pts.GetSize()-1];
			double x,y,t0,t1;
			bool bIntersect = GraphAPI::GGetLineIntersectLineSeg(p1.x,p1.y,
				p2.x,p2.y,p3.x,p3.y,p4.x,p4.y,&x,&y,&t0,&t1);
			for(int j=0; j<pts.GetSize(); j++)
			{
				if(bIntersect)
					arrPts.Add(pts[j]);
				else
					arrPts.Add(pts[pts.GetSize()-1-j]);
			}
		}
		else if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
		{
			((CGeoParallel*)pGeo)->Separate(pObj1, pObj2);
			if(!pObj1 || !pObj2) continue;
			pObj1->GetShape(arrPts);
			pObj2->GetShape(pts);
			for(int j=0; j<pts.GetSize(); j++)
			{
				arrPts.Add(pts[j]);
			}
		}
		else
		{
			continue;
		}
		
		arrPts.Add(arrPts[0]);
		
		CFeature *pNew = NULL;
		if(pTemp)
		{
			pNew = pTemp->Clone();
		}
		else
		{
			pNew = HandleToFtr(handles[i])->Clone();
			if (!pNew) continue;
			pNew->CreateGeometry(CLS_GEOSURFACE);
		}
		
		pNew->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());

		if(layid==-1)
		{
			layid = m_pEditor->GetFtrLayerIDOfFtr(handles[i]);
		}
		
		if ( !m_pEditor->AddObject(pNew,layid) )
		{
			delete pNew;
			continue;
		}
		
		if(layid == m_pEditor->GetFtrLayerIDOfFtr(handles[i]))
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handles[i]);		
		m_pEditor->DeleteObject(handles[i]);
	}
	
	undo.Commit();
	if(pTemp) delete pTemp;
}

void CDoubleLinesToSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		DoubleLineToSurface(handles,num);

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDoubleLinesToSurfaceAllCommand,CEditCommand)

CDoubleLinesToSurfaceAllCommand::CDoubleLinesToSurfaceAllCommand()
{
}

CDoubleLinesToSurfaceAllCommand::~CDoubleLinesToSurfaceAllCommand()
{
}

CString CDoubleLinesToSurfaceAllCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DLINE2SURFACE);
}

void CDoubleLinesToSurfaceAllCommand::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CDoubleLinesToSurfaceAllCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue(PF_HANDLELAYERS,&CVariantEx(var));
	
	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue(PF_LAYERNAME,&CVariantEx(var));
}

void CDoubleLinesToSurfaceAllCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DoubleLinesToSurfaceAll",Name());
	
	param->AddLayerNameParamEx(PF_HANDLELAYERS,(LPCTSTR)m_handleLayers,StrFromResID(IDS_CMDPLANE_DOUBLELINELAYER),NULL,LAYERPARAMITEM_LINE);
	
	param->AddLayerNameParamEx(PF_LAYERNAME,(LPCTSTR)m_targetLayer,StrFromResID(IDS_CMDPLANE_SURFACELAYER),NULL,LAYERPARAMITEM_AREA);
}

void CDoubleLinesToSurfaceAllCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_HANDLELAYERS,var) )
	{					
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,PF_LAYERNAME,var) )
	{					
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimRight();
		m_targetLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CDoubleLinesToSurfaceAllCommand::ReadFileStrings()
{
	if (!PathFileExists(m_handleLayers))
		return;

	FILE *fp = fopen(m_handleLayers, "r");
	if (fp)
	{
		char line[4096];
		int pos = 0;
		while (!feof(fp))
		{
			memset(line, 0, sizeof(line));
			fgets(line, sizeof(line) - 1, fp);

			if (pos == 0)
			{
				m_handleLayers = CString(line);
				m_handleLayers.TrimRight();
			}
			else if (pos == 1)
			{
				m_targetLayer = CString(line);
				m_targetLayer.TrimRight();
			}
			pos++;
		}
		fclose(fp);
		fp = NULL;
	}
}

void CDoubleLinesToSurfaceAllCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	ReadFileStrings();

	CFtrLayerArray arr;
	CStringArray strs0, strs;

	convertStringToStrArray(m_handleLayers, strs0);
	convertStringToStrArray(m_targetLayer, strs);
	int nSum=0, i, j;
	for(i=0; i<strs0.GetSize(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(strs0[i]);
		arr.Add(pLayer);
		if(pLayer)
		{
			nSum += pLayer->GetObjectCount();
		}
	}

	if(arr.GetSize()==0)
	{
		pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers, arr);
		for(i=0; i<arr.GetSize(); i++)
		{
			nSum += arr[i]->GetObjectCount();
		}
	}

	CUndoFtrs undo(m_pEditor,Name());
	
	GProgressStart(nSum);
	for (i=0;i<arr.GetSize();i++)
	{
		CFtrLayer *pLayer = arr[i];
		if(!pLayer) continue;

		CFtrLayer *pStoreLayer = NULL;
		if(strs.GetSize()==1)
		{
			if(strs[0].IsEmpty())
			{
				pStoreLayer = arr[i];
			}
			else
			{
				pStoreLayer = pDS->GetFtrLayer(strs[0]);
				if(!pStoreLayer)
				{
					pStoreLayer = new CFtrLayer();
					pStoreLayer->SetName(strs[0]);
					pDS->AddFtrLayer(pStoreLayer);
				}
			}
		}
		else if(i<strs.GetSize() && !strs.GetAt(i).IsEmpty())
		{
			pStoreLayer = pDS->GetFtrLayer(strs[i]);
			if(!pStoreLayer)
			{
				pStoreLayer = new CFtrLayer();
				pStoreLayer->SetName(strs[i]);
				pDS->AddFtrLayer(pStoreLayer);
			}
		}
		else
		{
			pStoreLayer = arr[i];
		}

		if(!pStoreLayer) continue;

		CFeature *pTemp = pStoreLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOSURFACE);

		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature *pFeature = pLayer->GetObject(j);
			if(!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo)
			{
				continue;
			}

			CGeometry* pObj1=NULL;
			CGeometry* pObj2=NULL;
			CArray<PT_3DEX,PT_3DEX> arrPts, pts;
			if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
				((CGeoDCurve*)pGeo)->Separate(pObj1, pObj2);
				if(!pObj1 || !pObj2) continue;
				pObj1->GetShape(arrPts);
				pObj2->GetShape(pts);
				PT_3DEX p1 = arrPts[0];
				PT_3DEX p2 = pts[0];
				PT_3DEX p3 = arrPts[arrPts.GetSize()-1];
				PT_3DEX p4 = pts[pts.GetSize()-1];
				double x,y,t0,t1;
				bool bIntersect = GraphAPI::GGetLineIntersectLineSeg(p1.x,p1.y,
					p2.x,p2.y,p3.x,p3.y,p4.x,p4.y,&x,&y,&t0,&t1);
				for(int j=0; j<pts.GetSize(); j++)
				{
					if(bIntersect)
						arrPts.Add(pts[j]);
					else
						arrPts.Add(pts[pts.GetSize()-1-j]);
				}
			}
			else if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			{
				((CGeoParallel*)pGeo)->Separate(pObj1, pObj2);
				if(!pObj1 || !pObj2) continue;
				pObj1->GetShape(arrPts);
				pObj2->GetShape(pts);
				for(int j=0; j<pts.GetSize(); j++)
				{
					arrPts.Add(pts[j]);
				}
			}
			else
			{
				continue;
			}
			
			arrPts.Add(arrPts[0]);
			
			CFeature *pNew = pTemp->Clone();
			if (!pNew) continue;
			
			pNew->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
			
			if (!m_pEditor->AddObject(pNew, pStoreLayer->GetID()))
			{
				delete pNew;
				continue;
			}
			
			if(pStoreLayer==arr[i])
				GETXDS(m_pEditor)->CopyXAttributes(pFeature,pNew);
			undo.AddNewFeature(FTR_HANDLE(pNew));
			undo.AddOldFeature(FtrToHandle(pFeature));		
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
		}
		if(pTemp) delete pTemp;
	}
	
	GProgressEnd();
	undo.Commit();
	
	CCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CReverseAssistlineCommand,CEditCommand)

CReverseAssistlineCommand::CReverseAssistlineCommand()
{
}

CReverseAssistlineCommand::~CReverseAssistlineCommand()
{
}

CString CReverseAssistlineCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DCURVEREVERSE);
}

void CReverseAssistlineCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;

		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		
		return;
	}
	
	CEditCommand::Start();
}

void CReverseAssistlineCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor,Name());
		
		for( int i = num-1; i>=0; i--)
		{
			CGeometry* pObj = HandleToFtr(handles[i])->GetGeometry();
			
			if( pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
			{
				CFeature *pFtr = HandleToFtr(handles[i])->Clone();
				if(!pFtr) continue;	
				pFtr->SetID(OUID());

				pObj = pFtr->GetGeometry();
				if (!pObj) continue;

				CArray<PT_3DEX,PT_3DEX> arr,arr1, arrnew;
				((CGeoDCurve*)pObj)->GetBaseShape(arr);
				((CGeoDCurve*)pObj)->GetAssistShape(arr1);

				int ptsnum = arr1.GetSize();
				PT_3DEX pt3dex = arr1[ptsnum-1];
				pt3dex.pencode = penMove;
				arrnew.Add(pt3dex);
				pt3dex = arr1[ptsnum-1];
				pt3dex.pencode = penNone;
				arrnew.Add(pt3dex);
				for(int j=2; j<ptsnum; j++)
				{
					pt3dex = arr1[ptsnum-j];
					pt3dex.pencode = arr1[ptsnum-j+1].pencode;
					arrnew.Add(pt3dex);
				}
				arr.Append(arrnew);
				pObj->CreateShape(arr.GetData(),arr.GetSize());				
				
				if( !m_pEditor->AddObject(pFtr,m_pEditor->GetFtrLayerIDOfFtr(handles[i]) ))
				{
					delete pFtr;
					Abort();
					return;
				}
				
				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]),pFtr);
				undo.arrNewHandles.Add(FtrToHandle(pFtr));	
				
				m_pEditor->DeleteObject(handles[i]);
				undo.arrOldHandles.Add(handles[i]);
			}
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSurfaceToDoubleLinesCommand,CEditCommand)

CSurfaceToDoubleLinesCommand::CSurfaceToDoubleLinesCommand()
{
}

CSurfaceToDoubleLinesCommand::~CSurfaceToDoubleLinesCommand()
{
	m_pos = 0;
}

CString CSurfaceToDoubleLinesCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SURFACE2DCURVE);
}

void CSurfaceToDoubleLinesCommand::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CSurfaceToDoubleLinesCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue(PF_HANDLELAYERS,&CVariantEx(var));
	
	var = (long)(m_pos);
	tab.AddValue("StartPos",&CVariantEx(var));
}

void CSurfaceToDoubleLinesCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SurfaceToDoubleLines",Name());
	
	param->AddLayerNameParamEx(PF_HANDLELAYERS,(LPCTSTR)m_handleLayers,StrFromResID(IDS_CMDPLANE_SURFACELAYER),NULL);
	
	param->AddParam("StartPos",m_pos,StrFromResID(IDS_CMDPLANE_STARTPOS),NULL);
}

void CSurfaceToDoubleLinesCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_HANDLELAYERS,var) )
	{					
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,"StartPos",var) )
	{					
		m_pos = (long)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CSurfaceToDoubleLinesCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;


	CFtrLayerArray arrLayers;
	int i=0, j=0;
	pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers,arrLayers);

	CUndoFtrs undo(m_pEditor,Name());
	for (i=0;i<arrLayers.GetSize();i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if(!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				continue;
			}

			((CGeoSurface*)pGeo)->EnableFillColor(FALSE, 0);

			CArray<PT_3DEX,PT_3DEX> arrPts, pts;
			pGeo->GetShape(arrPts);
			arrPts.RemoveAt(arrPts.GetSize()-1);//剔除闭合点
			int npt = arrPts.GetSize();
			if(m_pos<0 || m_pos>npt-1) continue;

			int k=0;
			for(k=0; k<npt; k++)
			{
				if(k==npt/2)
				{
					PT_3DEX temp = arrPts[(k+m_pos)%npt];
					temp.pencode = penMove;
					pts.Add(temp);
				}
				else
				{
					pts.Add(arrPts[(k+m_pos)%npt]);
				}
			}

			CFeature *pNew = pFeature->Clone();
			if (!pNew) continue;
			
			if (!pNew->CreateGeometry(CLS_GEODCURVE)) continue;
			
			pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());

			if (!m_pEditor->AddObject(pNew, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFeature))))
			{
				delete pNew;
				continue;
			}
			
			GETXDS(m_pEditor)->CopyXAttributes(pFeature,pNew);
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
			undo.AddOldFeature(FtrToHandle(pFeature));	
			undo.AddNewFeature(FTR_HANDLE(pNew));
		}
	}
	
	undo.Commit();
	
	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CMergeCurveFACommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMergeCurveFACommand,CEditCommand)

CMergeCurveFACommand::CMergeCurveFACommand()
{
    m_nStep = -1;
}

CMergeCurveFACommand::~CMergeCurveFACommand()
{

}

CString CMergeCurveFACommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MERGECURVEFA);
}

void CMergeCurveFACommand::Start()
{
 	if( !m_pEditor )return;
	
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
}

void CMergeCurveFACommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_lineLayer1);
	tab.AddValue("LineLayer1",&CVariantEx(var));

	var = (LPCTSTR)(m_lineLayer2);
	tab.AddValue("LineLayer2",&CVariantEx(var));

	var = (LPCTSTR)(m_DCurveLayer);
	tab.AddValue("DCurveLayer",&CVariantEx(var));
}

void CMergeCurveFACommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MergeCurveFACommand",Name());
	
	param->AddLayerNameParam("LineLayer1",(LPCTSTR)m_lineLayer1,StrFromResID(IDS_CMDPLANE_LINELAYER1));
	param->AddLayerNameParam("LineLayer2",(LPCTSTR)m_lineLayer2,StrFromResID(IDS_CMDPLANE_LINELAYER2));
	param->AddLayerNameParam("DCurveLayer",(LPCTSTR)m_DCurveLayer,StrFromResID(IDS_CMDPLANE_DCURVELAYER));
}


void CMergeCurveFACommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"LineLayer1",var) )
	{					
		m_lineLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_lineLayer1.TrimRight();
		m_lineLayer1.TrimLeft();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"LineLayer2",var) )
	{					
		m_lineLayer2 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_lineLayer2.TrimRight();
		m_lineLayer2.TrimLeft();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"DCurveLayer",var) )
	{					
		m_DCurveLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_DCurveLayer.TrimRight();
		m_DCurveLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CMergeCurveFACommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ) return;

	if(m_lineLayer1.IsEmpty() || m_lineLayer2.IsEmpty() || m_DCurveLayer.IsEmpty())
		return;

	vector<FeaItem> container1, container2;

	CFtrLayer *pLayer = pDS->GetFtrLayer(m_lineLayer1);
	if(!pLayer) return;
	int nObj = pLayer->GetObjectCount(), i;
	for(i=0; i<nObj; i++)
	{
		CFeature *pFtr = pLayer->GetObject(i);
		if(!pFtr) continue;
		CGeometry *pObj = pFtr->GetGeometry();
		if(!pObj) continue;

		int nCls = pObj->GetClassType();

		if(nCls==CLS_GEOCURVE)
		{
			FeaItem item;
			item.pFtr = pFtr;
			item.pObj = pObj;
			item.sp = pObj->GetDataPoint(0);
			int nSum = pObj->GetDataPointSum();
			item.ep = pObj->GetDataPoint(nSum-1);
			if(!GraphAPI::GIsEqual3DPoint(&item.sp, &item.ep))
			{
				pFtr->SetAppFlag(1);
				container1.push_back(item);
			}
		}
	}

	pLayer = pDS->GetFtrLayer(m_lineLayer2);
	if(!pLayer) return;
	nObj = pLayer->GetObjectCount();
	for(i=0; i<nObj; i++)
	{
		CFeature *pFtr = pLayer->GetObject(i);
		if(!pFtr) continue;
		CGeometry *pObj = pFtr->GetGeometry();
		if(!pObj) continue;

		int nCls = pObj->GetClassType();
		
		if(nCls==CLS_GEOCURVE)
		{
			FeaItem item;
			item.pFtr = pFtr;
			item.pObj = pObj;
			item.sp = pObj->GetDataPoint(0);
			int nSum = pObj->GetDataPointSum();
			item.ep = pObj->GetDataPoint(nSum-1);
			if(!GraphAPI::GIsEqual3DPoint(&item.sp, &item.ep))
			{
				pFtr->SetAppFlag(1);
				container2.push_back(item);
			}
		}
	}

	pLayer = pDS->GetFtrLayer(m_DCurveLayer);
	if(!pLayer)
	{
		pLayer = pDS->CreateFtrLayer(m_DCurveLayer);
		if(!pLayer) return;
		pDS->AddFtrLayer(pLayer);
	}
	CFeature *pFtrTemp = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEODCURVE);
	if (!pFtrTemp || !pFtrTemp->GetGeometry())
	{
		Abort();
		return;
	}

	vector<FeaItem>::const_iterator it1,it2;

	CUndoFtrs undo(m_pEditor, Name());
	for(it1= container1.begin();it1<container1.end();it1++)
	{
		if((*it1).pFtr->GetAppFlag()==0) continue;
		(*it1).pFtr->SetAppFlag(0);
		for(it2= container2.begin();it2<container2.end();it2++)
		{
			if((*it2).pFtr->GetAppFlag()==0) continue;
			if( (*it1).pFtr == (*it2).pFtr) continue;

			BOOL bConnect = FALSE;
			const PT_3DEX *pt1 = &(*it1).sp;
			const PT_3DEX *pt2 = &(*it1).ep;
			const PT_3DEX *pt3 = &(*it2).sp;
			const PT_3DEX *pt4 = &(*it2).ep;
			if(GraphAPI::GIsEqual2DPoint(pt1, pt3) && GraphAPI::GIsEqual2DPoint(pt2, pt4))
			{
				bConnect = TRUE;
			}
			if(GraphAPI::GIsEqual2DPoint(pt1, pt4) && GraphAPI::GIsEqual2DPoint(pt2, pt3))
			{
				bConnect = TRUE;
			}

			if(bConnect)
			{
				(*it2).pFtr->SetAppFlag(0);
				CFeature *pFtr = pFtrTemp->Clone();
				CGeometry *pGeo = pFtr->GetGeometry();
				
				CArray<PT_3DEX,PT_3DEX> pts1, pts2;
				(*it1).pObj->GetShape(pts1);
				(*it2).pObj->GetShape(pts2);
				pts2[0].pencode = penMove;
				pts1.Append(pts2);
				if (!pGeo->CreateShape(pts1.GetData(),pts1.GetSize()))
				{
					delete pFtr;
					continue;
				}
				
				m_pEditor->AddObject(pFtr,pLayer->GetID());
				undo.AddNewFeature(FtrToHandle(pFtr));

				m_pEditor->DeleteObject(FtrToHandle((*it1).pFtr));
				m_pEditor->DeleteObject(FtrToHandle((*it2).pFtr));
				undo.AddOldFeature(FtrToHandle((*it1).pFtr));
				undo.AddOldFeature(FtrToHandle((*it2).pFtr));
			}
		}
	}

	for(it2= container2.begin();it2<container2.end();it2++)
	{
		if( (*it2).pFtr )
		{
			(*it2).pFtr->SetAppFlag(0);
		}
	}

	delete pFtrTemp;
	undo.Commit();
	Finish();
	CEditCommand::PtClick(pt,flag);
}


IMPLEMENT_DYNCREATE(CSurfaceToDoubleLinesCommand_One,CEditCommand)


CSurfaceToDoubleLinesCommand_One::CSurfaceToDoubleLinesCommand_One()
{
	m_pFtr = NULL;
}


CSurfaceToDoubleLinesCommand_One::~CSurfaceToDoubleLinesCommand_One()
{
	
}

CString CSurfaceToDoubleLinesCommand_One::Name()
{
	return StrFromResID(IDS_CMDNAME_SURFACE2DCURVE_ONE);
}


void CSurfaceToDoubleLinesCommand_One::Start()
{
	CEditCommand::Start();

	m_pFtr = NULL;
	m_nStep = 0;
}


void CSurfaceToDoubleLinesCommand_One::PtMove(PT_3D &pt)
{
	if( m_nStep==1 )
	{
		m_ptEnd = pt;
		
		CGeometry *pNewObj = Convert(m_pFtr->GetGeometry());
		if(pNewObj!=NULL)
		{
			CGeoDCurve *pNewObj2 = (CGeoDCurve*)pNewObj;
			CGeometry *pObj1 = NULL, *pObj2 = NULL;
			pNewObj2->Separate(pObj1,pObj2);

			if(pObj1)
			{
				GrBuffer buf;
				pObj1->Draw(&buf);

				buf.SetAllLineWidth(FALSE,3);

				m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
				delete pObj1;
			}
			if(pObj2)
			{
				delete pObj2;
			}
			delete pNewObj;
		}	
	}
}



void CSurfaceToDoubleLinesCommand_One::PtClick(PT_3D &pt, int flag)
{
	if(m_nStep==0)
	{
		//得到目标地物
		if( !CanGetSelObjs(flag,FALSE) )return;
		
		FTR_HANDLE handle = m_pEditor->GetSelection()->GetLastSelectedObj();
		m_pFtr = HandleToFtr(handle);
		if(!m_pFtr) return;
		CGeometry *pObj = m_pFtr->GetGeometry();
		if(!pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			return;

		m_ptStart = pt;
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);

		m_pEditor->CloseSelector();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
	}
	else if(m_nStep==1)
	{
		if(!m_pFtr) return;

		m_ptEnd = pt;

		CGeometry *pNewObj = Convert(m_pFtr->GetGeometry());

		if( pNewObj!=NULL)
		{
			CFeature *pNewFtr = m_pFtr->Clone();
			pNewFtr->SetGeometry(pNewObj);

			m_pEditor->AddObject(pNewFtr,m_pEditor->GetDataSource()->GetFtrLayerOfObject(m_pFtr)->GetID());
			GETXDS(m_pEditor)->CopyXAttributes(m_pFtr, pNewFtr);
			m_pEditor->DeleteObject(FtrToHandle(m_pFtr));

			CUndoFtrs undo(m_pEditor,Name());

			undo.arrOldHandles.Add(FtrToHandle(m_pFtr));
			undo.arrNewHandles.Add(FtrToHandle(pNewFtr));

			undo.Commit();

			Finish();
		}
		else
		{
			Abort();
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);

		return;
	}

	CEditCommand::PtClick(pt,flag);
}


CGeometry *CSurfaceToDoubleLinesCommand_One::Convert(CGeometry *pObj0)
{
	CGeoSurface *pObj = (CGeoSurface *)pObj0;

	int pt_index1 = pObj->FindNearestKeyCtrlPt(m_ptStart,1e+10,NULL,1).index;
	int pt_index2 = pObj->FindNearestKeyCtrlPt(m_ptEnd,1e+10,NULL,1).index;

	if(pt_index1<0 || pt_index2<0 )
		return NULL;

	CArray<int,int> arrPos;
	pObj->GetShape()->GetKeyPosOfBaseLines(arrPos);

	int pt_index11 = arrPos[pt_index1];
	int pt_index22 = arrPos[pt_index2];

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pObj->GetShape()->GetPts(arrPts);

	double all_len = GraphAPI::GGetAllLen2D(arrPts.GetData(),arrPts.GetSize());
	
	int index_min = min(pt_index11,pt_index22);
	int index_max = max(pt_index11,pt_index22);
	
	//取短的一部分
	double part_len = GraphAPI::GGetAllLen2D(arrPts.GetData()+index_min,index_max-index_min);
	if(part_len>all_len*0.5)
	{
		index_min = min(pt_index1,pt_index2);
		index_max = max(pt_index1,pt_index2);
		
		CArray<PT_3DEX,PT_3DEX> arrPts2;
		pObj->GetShape(arrPts2);

		if(pObj->IsClosed())
			arrPts2.RemoveAt(arrPts2.GetSize()-1);

		if(index_max>=arrPts2.GetSize())
			return NULL;

		int npt = arrPts2.GetSize();

		CArray<PT_3DEX,PT_3DEX> arrPts3;
		for(int i=index_max; i<=index_min+npt; i++)
		{
			arrPts3.Add(arrPts2[(i%npt)]);
		}
		for(i=index_min+1; i<index_max; i++)
		{
			arrPts3.Add(arrPts2[i]);

			if( i==(index_min+1) )
			{
				arrPts3[arrPts3.GetSize()-1].pencode = penMove;
			}
		}

		CGeometry *pNewObj = pObj->Clone();
		pNewObj->CreateShape(arrPts3.GetData(),arrPts3.GetSize());

		return pNewObj;
	}
	else
	{
		index_min = min(pt_index1,pt_index2);
		index_max = max(pt_index1,pt_index2);
		
		CArray<PT_3DEX,PT_3DEX> arrPts2;
		pObj->GetShape(arrPts2);

		if(pObj->IsClosed())
			arrPts2.RemoveAt(arrPts2.GetSize()-1);

		if(index_max>=arrPts2.GetSize())
			return NULL;

		int npt = arrPts2.GetSize();

		CArray<PT_3DEX,PT_3DEX> arrPts3;
		for(int i=index_min; i<=index_max; i++)
		{
			arrPts3.Add(arrPts2[i]);
		}
		for(i=index_max+1; i<index_min+npt; i++)
		{
			arrPts3.Add(arrPts2[(i%npt)]);

			if( i==(index_max+1) )
			{
				arrPts3[arrPts3.GetSize()-1].pencode = penMove;
			}
		}

		CGeometry *pNewObj = new CGeoDCurve();
		pNewObj->CopyFrom(pObj);
		pNewObj->CreateShape(arrPts3.GetData(),arrPts3.GetSize());

		return pNewObj;
	}

	return NULL;
}


void CSurfaceToDoubleLinesCommand_One::Abort()
{
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}



//////////////////////////////////////////////////////////////////////
// CDeleteBorderInVEGACommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDeleteBorderInVEGACommand,CEditCommand)

CDeleteBorderInVEGACommand::CDeleteBorderInVEGACommand()
{
    m_nStep = -1;
}

CDeleteBorderInVEGACommand::~CDeleteBorderInVEGACommand()
{

}

CString CDeleteBorderInVEGACommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DEL_BORDER);
}

void CDeleteBorderInVEGACommand::Start()
{
 	if( !m_pEditor )return;
	
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
}

void CDeleteBorderInVEGACommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_borderLayer);
	tab.AddValue("BorderLayer",&CVariantEx(var));

	var = (LPCTSTR)(m_VEGALayers);
	tab.AddValue("VEGALayers",&CVariantEx(var));
}

void CDeleteBorderInVEGACommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("DeleteBorderInVEGA",Name());
	
	param->AddLayerNameParamEx("BorderLayer",(LPCTSTR)m_borderLayer,StrFromResID(IDS_CMDPLANE_BOUNDALYER));
	param->AddLayerNameParamEx("VEGALayers",(LPCTSTR)m_VEGALayers, StrFromResID(IDS_CMPPLANE_VEGALAYER));
}


void CDeleteBorderInVEGACommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"BorderLayer",var) )
	{					
		m_borderLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_borderLayer.TrimRight();
		m_borderLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"VEGALayers",var) )
	{					
		m_VEGALayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_VEGALayers.TrimRight();
		m_VEGALayers.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CDeleteBorderInVEGACommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ) return;

	if(m_borderLayer.IsEmpty() || m_VEGALayers.IsEmpty())
		return;

	CFtrArray arr1, arr2;
	int i,j;

	//地类界
	CFtrLayerArray layers0;
	pDS->GetFtrLayersByNameOrCode_editable(m_borderLayer,layers0);
	for(i=0; i<layers0.GetSize(); i++)
	{
		int nObj1 = layers0[i]->GetObjectCount();
		for(j=0; j<nObj1; j++)
		{
			CFeature *pFtr = layers0[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible()) continue;
			arr1.Add(pFtr);
		}
	}

	//植被
	CFtrLayerArray layers1;
	pDS->GetFtrLayersByNameOrCode_editable(m_VEGALayers,layers1);
	for(i=0; i<layers1.GetSize(); i++)
	{
		int nObj1 = layers1[i]->GetObjectCount();
		for(j=0; j<nObj1; j++)
		{
			CFeature *pFtr = layers1[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible()) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				arr2.Add(pFtr);
		}
	}

	CUndoFtrs undo(m_pEditor, Name());
	GProgressStart(arr1.GetSize());
	for(i=0; i<arr1.GetSize(); i++)
	{
		GProgressStep();
		PT_3DEX pt0, pt1, pt2;
		CGeometry *pObj = arr1[i]->GetGeometry();
		if(!pObj) continue;
		int nPt = pObj->GetDataPointSum();
		if(nPt<2) continue;

		pt0 = pObj->GetDataPoint(0);
		pt1 = pObj->GetDataPoint(nPt-1);
		pt2 = pObj->GetDataPoint(1);
		pt2.x = (pt0.x+pt2.x)/2;
		pt2.y = (pt0.y+pt2.y)/2;
		pt2.z = (pt0.z+pt2.z)/2;//pt2是第一段的终点
		Envelope e = pObj->GetEnvelope();

		for(j=0; j<arr2.GetSize(); j++)
		{
			CGeometry *pObj1 = arr2[j]->GetGeometry();
			if(!pObj1) continue;

			Envelope e1 = pObj1->GetEnvelope();
			if(!e.bIntersect(&e1)) continue;

			CArray<PT_3DEX,PT_3DEX> pts;
			const CShapeLine *pSh = pObj1->GetShape();
			if(!pSh) continue;
			pSh->GetPts(pts);

			BOOL bInside = TRUE;
			if(0==pObj1->bPtIn(&pt0))
			{
				bInside = FALSE;
			}
			else if(0==pObj1->bPtIn(&pt1))
			{
				bInside = FALSE;
			}
			else if(pObj1->bPtIn(&pt2)<2)
			{
				bInside = FALSE;
			}

			if(bInside)
			{
				m_pEditor->DeleteObject(FtrToHandle(arr1[i]));
				undo.AddOldFeature(FtrToHandle(arr1[i]));
			}
		}
	}
	GProgressEnd();
	undo.Commit();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

//////////////////////////////////////////////////////////////////////
// CDelPtAroundCurveCommand Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDelPtAroundCurveCommand,CEditCommand)

CDelPtAroundCurveCommand::CDelPtAroundCurveCommand()
{
   m_nStep = -1;
   m_dis = 10.0;
}

CDelPtAroundCurveCommand::~CDelPtAroundCurveCommand()
{

}

CString CDelPtAroundCurveCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DElPT_AROUNDCURVE);
}

void CDelPtAroundCurveCommand::Start()
{	
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
}

void CDelPtAroundCurveCommand::Abort()
{
	CEditCommand::Abort();
}

void CDelPtAroundCurveCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	CEditCommand::Finish();
}

void CDelPtAroundCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DelPtAroundCurve",Name());
	param->AddLayerNameParamEx("PointLayer",(LPCTSTR)m_strPoint,StrFromResID(IDS_CMDPLANE_POINTLAYER));
	param->AddLayerNameParamEx("CurveLayer",(LPCTSTR)m_strCurve,StrFromResID(IDS_CMDPLANE_LINELAYER));
	param->AddParam("Distance",m_dis,StrFromResID(IDS_CMDPLANE_MM_SCOPE));
}

void CDelPtAroundCurveCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
		
	if( tab.GetValue(0,"PointLayer",var) )
	{					
		m_strPoint = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strPoint.TrimLeft();
		m_strPoint.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,"CurveLayer",var) )
	{					
		m_strCurve = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strCurve.TrimRight();
		m_strCurve.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,"Distance",var) )
	{
		m_dis = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
 	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CDelPtAroundCurveCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	
	var = (LPCTSTR)(m_strPoint);
	tab.AddValue("PointLayer",&CVariantEx(var));
	
	var = (LPCTSTR)(m_strCurve);
	tab.AddValue("CurveLayer",&CVariantEx(var));
	
	var = (double)(m_dis);
	tab.AddValue("Distance",&CVariantEx(var));
}

static BOOL GetDistanceFromPointToLine(PT_3D ptA, PT_3D ptB, PT_3D ptC, double *fDistance,PT_3D *rtPT)
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
	if(fDistance) *fDistance = minDistance;
	if(rtPT) *rtPT = D;
	return TRUE;
	
}

double GetDistanceFromPointToCurve(CArray<PT_3DEX,PT_3DEX>& pts, PT_3D& pt, PT_3D *retpt=NULL)
{
	int nPt = pts.GetSize();
	double mindis = 9e10;
	if(nPt<2) return mindis;
	PT_3DEX tPT[2];
	for (int i=0;i<nPt-1;i++)//计算线串上两点到点的最近距离
	{
		tPT[0] = pts.GetAt(i);
		tPT[1] = pts.GetAt(i+1);
		
		//获取点对象到线串关键点连线的最短距离
		double dis;
		PT_3D ret;
		if (GetDistanceFromPointToLine(tPT[0],tPT[1],pt,&dis,&ret))
		{
			if ( mindis>dis )
			{
				mindis = dis;
				if(retpt) *retpt = ret;
			}
		}		 
	}

	return mindis;
}

void CDelPtAroundCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDS || !pDQ) return;
	
	CFtrLayerArray arr,arr1;
	pDS->GetFtrLayersByNameOrCode_editable(m_strCurve, arr);
	pDS->GetFtrLayersByNameOrCode_editable(m_strPoint, arr1);

	pDS->SaveAllQueryFlags(TRUE,FALSE);
	int i,j,k,nSum=0;
	for( i=0; i<arr.GetSize(); i++)
	{
		nSum += arr[i]->GetValidObjsCount();
	}
	for( i=0; i<arr1.GetSize(); i++)
	{
		arr1[i]->SetAllowQuery(TRUE);
	}

	double dis1 = m_dis+GraphAPI::g_lfDisTolerance;
	GProgressStart(nSum);
	CUndoFtrs undo(m_pEditor,Name());
	for(i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;

			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);
			int nPt = pts.GetSize();
			if(nPt<2) continue;

			Envelope e = pGeo->GetEnvelope();
			e.m_xh += dis1;
			e.m_xl -= dis1;
			e.m_yh += dis1;
			e.m_yl -= dis1;
			pDQ->FindObjectInRect(e,NULL,TRUE,FALSE);
			int num;
			const CPFeature *ftrs = pDQ->GetFoundHandles(num);

			for(k=0; k<num; k++)
			{
				CGeometry *pGeo1 = ftrs[k]->GetGeometry();
				if(!pGeo1 || !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
					continue;

				PT_3D temp = pGeo1->GetDataPoint(0);
				double dis = GetDistanceFromPointToCurve(pts,temp);
				if(dis<dis1)
				{
					m_pEditor->DeleteObject(FtrToHandle(ftrs[k]));
					undo.AddOldFeature(FtrToHandle(ftrs[k]));
				}
			}
		}
	}
	GProgressEnd();
	undo.Commit();
	pDS->RestoreAllQueryFlags();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

//////////////////////////////////////////////////////////////////////
// CModifyRoadBySunRuleCommand Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CModifyRoadBySunRuleCommand,CEditCommand)

CModifyRoadBySunRuleCommand::CModifyRoadBySunRuleCommand()
{
   m_nStep = -1;
   m_splitdis = 15.0;
}

CModifyRoadBySunRuleCommand::~CModifyRoadBySunRuleCommand()
{

}

CString CModifyRoadBySunRuleCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_MODIFYROAD);
}

void CModifyRoadBySunRuleCommand::Start()
{	
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
}

void CModifyRoadBySunRuleCommand::Abort()
{
	CEditCommand::Abort();
}

void CModifyRoadBySunRuleCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	CEditCommand::Finish();
}

void CModifyRoadBySunRuleCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ModifyRoadBySunRule",Name());
	param->AddLayerNameParamEx("Layers",(LPCTSTR)m_strLayers,StrFromResID(IDS_ROAD_LAYER));
	param->AddLayerNameParamEx("VirLayers",(LPCTSTR)m_strVirLayers,StrFromResID(IDS_ROAD_VIRLAYER));
	param->AddParam("SplitDis",m_splitdis,StrFromResID(IDS_SPLIT_DIS));
}

void CModifyRoadBySunRuleCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
		
	if( tab.GetValue(0,"Layers",var) )
	{					
		m_strLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strLayers.TrimLeft();
		m_strLayers.TrimRight();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"VirLayers",var) )
	{					
		m_strVirLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strVirLayers.TrimRight();
		m_strVirLayers.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,"SplitDis",var) )
	{
		m_splitdis = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
 	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CModifyRoadBySunRuleCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	
	var = (LPCTSTR)(m_strLayers);
	tab.AddValue("Layers",&CVariantEx(var));

	var = (LPCTSTR)(m_strVirLayers);
	tab.AddValue("VirLayers",&CVariantEx(var));
	
	var = (double)(m_splitdis);
	tab.AddValue("SplitDis",&CVariantEx(var));
}

void CModifyRoadBySunRuleCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	CStringArray arr1, arr2;
	convertStringToStrArray(m_strLayers, arr1);
	convertStringToStrArray(m_strVirLayers, arr2);

	int minsize = min( arr1.GetSize(), arr2.GetSize() );

	int nSum=0, i, j;
	for(i=0; i<minsize; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(arr1[i]);
		if(!pLayer || !pLayer->IsVisible())
			continue;
		nSum += pLayer->GetObjectCount();
	}
	
	CUndoFtrs undo(m_pEditor, Name());
	GProgressStart(nSum*3);

	for(i=0; i<minsize; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(arr1[i]);
		if(!pLayer || !pLayer->IsVisible())
			continue;

		CFtrLayer *pVirLayer = pDS->GetFtrLayer(arr2[i]);
		if(!pVirLayer)
		{
			pVirLayer = new CFtrLayer();
			pVirLayer->SetName(arr2[i]);
			m_pEditor->AddFtrLayer(pVirLayer);
		}

		pDS->SaveAllQueryFlags(TRUE, FALSE);

		pLayer->SetAllowQuery(TRUE);
		pVirLayer->SetAllowQuery(TRUE);
		ModifyRoad(pLayer, pVirLayer, undo);

		//处理短线
		ReplaceShortCurve(pLayer, pVirLayer);
		ReplaceShortCurve(pVirLayer, pLayer);

		pDS->RestoreAllQueryFlags();

		for(j=0; j<pLayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(pFtr && pFtr->IsVisible())
			{
				undo.AddNewFeature(FtrToHandle(pFtr));
			}
		}
		for(j=0; j<pVirLayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pVirLayer->GetObject(j);
			if(pFtr && pFtr->IsVisible())
			{
				undo.AddNewFeature(FtrToHandle(pFtr));
			}
		}
	}

	GProgressEnd();
	undo.Commit();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

void CModifyRoadBySunRuleCommand::ModifyRoad(CFtrLayer* pLayer, CFtrLayer *pVirLayer, CUndoFtrs& undo)
{
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDS || !pDQ) return;
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	if(!pXDS) return;
	double lfRange = 100;//匹配范围

	//道路边线分段
	int nObj = pLayer->GetObjectCount(), i, j, k;
	for(i=0; i<nObj; i++)
	{
		GProgressStep();
		CFeature *pFtr = pLayer->GetObject(i);
		if(!pFtr || !pFtr->IsVisible())
			continue;
		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			continue;

		CArray<PT_3DEX,PT_3DEX> arrPts, newPts;
		pGeo->GetShape(arrPts);

		if(arrPts.GetSize()<2) continue;
		if(arrPts[arrPts.GetSize()-1].x < arrPts[0].x)
		{
			pGeo->ReversePoints();
			pGeo->GetShape(arrPts);
		}

		CGeoArray geos;

		//以y=-x为分界线,越过此分界线的不打断
		double deta = (arrPts[0].x-arrPts[1].x) + (arrPts[0].y-arrPts[1].y) ;
		newPts.Add(arrPts[0]);

		for(j=1; j<arrPts.GetSize()-1; j++)
		{
			newPts.Add(arrPts[j]);

			double deta1 = (arrPts[j].x-arrPts[j+1].x) + (arrPts[j].y-arrPts[j+1].y);
			if( (deta>0&&deta1<0) || (deta<0&&deta1>0))
			{
				CGeoCurve *pCurve = new CGeoCurve();
				if(pCurve->CreateShape(newPts.GetData(), newPts.GetSize()) )
				{
					geos.Add(pCurve);
				}
				else
				{
					delete pCurve;
				}

				deta = deta1;
				newPts.RemoveAll();
				newPts.Add(arrPts[j]);
			}
		}

		//最后一段
		newPts.Add(arrPts[j]);
		CGeoCurve *pCurve = new CGeoCurve();
		if(pCurve->CreateShape(newPts.GetData(), newPts.GetSize()) )
		{
			geos.Add(pCurve);
		}
		else
		{
			delete pCurve;
		}

		for(k=0; k<geos.GetSize(); k++)
		{
			CFeature *pNew = pFtr->Clone();
			pNew->SetGeometry(geos[k]);
			pNew->SetAppFlag(i+1);
			m_pEditor->AddObject(pNew, pLayer->GetID());//pNew是临时的feature,不需要添加到undo里面
			pXDS->CopyXAttributes(pFtr, pNew);
		}
		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		undo.AddOldFeature(FtrToHandle(pFtr));
	}

	//阳光法则分虚实
	CFeature *pTempVirFtr = pVirLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOCURVE);
	if(!pTempVirFtr) return;
	nObj = pLayer->GetObjectCount();
	for(i=0; i<nObj; i++)
	{
		GProgressStep();
		CFeature *pFtr = pLayer->GetObject(i);
		if(!pFtr || !pFtr->IsVisible())
			continue;
		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			continue;
		
		int ptsum = pGeo->GetDataPointSum();
		if(ptsum<2) continue;
		PT_3D pt0 = pGeo->GetDataPoint(ptsum/2-1);
		PT_3D pt1 = pGeo->GetDataPoint(ptsum/2);
		{
			pt1.x = (pt0.x+pt1.x)/2;
			pt1.y = (pt0.y+pt1.y)/2;
		}

		//配对
		Envelope e;
		e.CreateFromPtAndRadius(pt1, lfRange);
		if( pDQ->FindObjectInRect(e, NULL, FALSE, FALSE) <2 ) continue;
		int num;
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		CFeature *pFound = NULL;
		PT_3D retpt, retpt1;
		double mindis = 9e10;
		for(j=0; j<num; j++)
		{
			if( ftrs[j]==pFtr ) continue;
			if( ftrs[j]->GetAppFlag() == pFtr->GetAppFlag() ) continue;

			CGeometry *pObj1 = ftrs[j]->GetGeometry();
			if(!pObj1 || !pObj1->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pObj1->GetShape(pts);
			if(pts.GetSize()<2) continue;
			double dis = GetDistanceFromPointToCurve(pts, pt1, &retpt1);
			if(dis<mindis)
			{
				pFound = ftrs[j];
				retpt = retpt1;
				mindis = dis;
			}
		}

		if(mindis>lfRange) continue;
		if(!pFound) continue;

		BOOL bUpOrLeft = ( fabs(pt0.x-pt1.x) - fabs(pt0.y-pt1.y) ) > 0 ;
		//判断虚实
		if(bUpOrLeft)//上虚下实
		{
			if(pt1.y>retpt.y)
			{
				CFeature *pNew = pTempVirFtr->Clone();
				pNew->SetGeometry(pGeo->Clone());
				m_pEditor->AddObject(pNew, pVirLayer->GetID());
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
			}
		}
		else//左虚右实
		{
			if(pt1.x<retpt.x)
			{
				CFeature *pNew = pTempVirFtr->Clone();
				pNew->SetGeometry(pGeo->Clone());
				m_pEditor->AddObject(pNew, pVirLayer->GetID());
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
			}
		}
	}
	delete pTempVirFtr;
}

//如果长度小于指定长度，并且两端没有连接同层地物， 则换层到pNewLayer
void CModifyRoadBySunRuleCommand::ReplaceShortCurve(CFtrLayer* pLayer, CFtrLayer *pNewLayer)
{
	if(!pLayer || !pNewLayer) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDS || !pDQ) return;

	int nObj = pLayer->GetObjectCount(), i;
	for(i=0; i<nObj; i++)
	{
		GProgressStep();
		CFeature *pFtr = pLayer->GetObject(i);
		if(!pFtr || !pFtr->IsVisible())
			continue;

		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			continue;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo->GetShape(arrPts);
		
		if(arrPts.GetSize()<2) continue;

		if(GraphAPI::GGetAllLen2D(arrPts.GetData(), arrPts.GetSize())>m_splitdis)
		{
			continue;
		}

		Envelope e0, e1;
		double r=0.01;
		e0.CreateFromPtAndRadius(arrPts[0], r);
		e1.CreateFromPtAndRadius(arrPts[arrPts.GetSize()-1], r);

		int num1, num2;
		CFtrLayer *pLayer1=NULL, *pLayer2=NULL;

		pDQ->FindObjectInRect(e0, NULL, FALSE, FALSE);
		const CPFeature *ftr = pDQ->GetFoundHandles(num1);
		if(num1==2)
		{
			CFeature *pFtr1 = ftr[0];
			if(pFtr1==pFtr) pFtr1=ftr[1];
			pLayer1 = pDS->GetFtrLayerOfObject(pFtr1);
		}
		
		pDQ->FindObjectInRect(e1, NULL, FALSE, FALSE);
		ftr = pDQ->GetFoundHandles(num2);
		if(num2==2)
		{
			CFeature *pFtr2 = ftr[0];
			if(pFtr2==pFtr) pFtr2=ftr[1];
			pLayer2 = pDS->GetFtrLayerOfObject(pFtr2);
		}

		if(pLayer1==NULL && pLayer2==NULL)
		{
			continue;
		}

		if(pLayer1!=pLayer && pLayer2!=pLayer)
		{
			CFeature *pNew = pNewLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOCURVE);
			if(pNew)
			{
				pNew->SetGeometry(pGeo->Clone());
				m_pEditor->AddObject(pNew, pNewLayer->GetID());
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifyLayerCommand,CEditCommand)

CModifyLayerCommand::CModifyLayerCommand()
{
	m_bResetBaseAtt = FALSE;
}

CModifyLayerCommand::~CModifyLayerCommand()
{
}

CString CModifyLayerCommand::Name()
{ 
	return StrFromResID(IDS_MODIFY_LAYER);
}

void CModifyLayerCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		
		return;
	}
	
	CEditCommand::Start();
}

void CModifyLayerCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ModifyLayer",Name());
	param->AddLayerNameParam("Layer",(LPCTSTR)m_strLayer,StrFromResID(IDS_CONVERTTO_LAYER));
}

void CModifyLayerCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layer",var) )
	{					
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CModifyLayerCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	
	var = (LPCTSTR)(m_strLayer);
	tab.AddValue("Layer",&CVariantEx(var));
}

void CModifyLayerCommand::ModifyLayer(const FTR_HANDLE* handles,int num)
{
	if(m_strLayer.IsEmpty()) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if(!pScheme) return;

	CFtrLayer *pDestLayer = pDS->GetFtrLayer(m_strLayer);
	if(!pDestLayer)
	{
		pDestLayer = new CFtrLayer();
		pDestLayer->SetName(m_strLayer);
		pDS->AddFtrLayer(pDestLayer);
	}

	CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(pDestLayer->GetName());
	
	float wid = pScheme->GetLayerDefineLineWidth(pDestLayer->GetName());
	
	CUndoModifyLayerOfObj undo(m_pEditor, "ModifyLayerOfObj");
	for (int i=0;i<num;i++)
	{
		CPFeature pftr = HandleToFtr(handles[i]);
		
		undo.arrHandles.Add(handles[i]);
		
		undo.oldVT.BeginAddValueItem();
		pftr->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		m_pEditor->DeleteObject(handles[i],FALSE);
		
		CGeometry *pGeo = pftr->GetGeometry();
		if (!pGeo) continue;
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		{
			((CGeoCurveBase*)pGeo)->m_fLineWidth = wid;
		}
		
		CFtrLayer* player_old = pDS->GetFtrLayerOfObject(pftr);
		undo.oldLayerArr.Add(player_old->GetID());
		pDS->SetFtrLayerOfObject(pftr,pDestLayer->GetID());	
		undo.newLayerArr.Add(pDestLayer->GetID()); 
		
		//改变层之后恢复缺省参数
		if(pSchemeLayer && m_bResetBaseAtt)
		{
			CValueTable tab;
			pSchemeLayer->GetBasicAttributeDefaultValues(tab);
			tab.DelField(FIELDNAME_FTRDISPLAYORDER);
			tab.DelField(FIELDNAME_GEOTEXT_CONTENT);
			tab.DelField(FIELDNAME_FTRMODIFYTIME);
			tab.DelField(FIELDNAME_GEOCLASS);
			tab.DelField(FIELDNAME_CLSTYPE);
			tab.DelField(FIELDNAME_GEOPOINT_ANGLE);
			tab.DelField(FIELDNAME_GEOCURVE_WIDTH);
			tab.DelField(FIELDNAME_GEOCURVE_DHEIGHT);
			tab.DelField(FIELDNAME_SYMBOLNAME);
			pftr->ReadFrom(tab);
			pftr->GetGeometry()->SetColor(-1);
			pftr->GetGeometry()->SetSymbolName("");
		}
		
		undo.newVT.BeginAddValueItem();
		pftr->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
		m_pEditor->RestoreObject(FtrToHandle(pftr));
	}
	
	undo.Commit();
}

void CModifyLayerCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		ModifyLayer(handles,num);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CSaveAssistLineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSaveAssistLineCommand,CEditCommand)

CSaveAssistLineCommand::CSaveAssistLineCommand()
{
    m_nStep = -1;
}

CSaveAssistLineCommand::~CSaveAssistLineCommand()
{

}

CString CSaveAssistLineCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPY_ASSISTLINE);
}

void CSaveAssistLineCommand::Start()
{
 	if( !m_pEditor )return;
	
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CSaveAssistLineCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("HandleLayer",&CVariantEx(var));

	var = (LPCTSTR)(m_StoreLayer);
	tab.AddValue("StoreLayer",&CVariantEx(var));
}

void CSaveAssistLineCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("SaveAssistLineCommand",Name());
	
	param->AddLayerNameParamEx("HandleLayer",(LPCTSTR)m_HandleLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParam("StoreLayer",(LPCTSTR)m_StoreLayer,StrFromResID(IDS_CONVERTTO_LAYER));
}


void CSaveAssistLineCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"HandleLayer",var) )
	{					
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimRight();
		m_HandleLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"StoreLayer",var) )
	{					
		m_StoreLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_StoreLayer.TrimRight();
		m_StoreLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CSaveAssistLineCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	if(m_HandleLayer.IsEmpty() || m_StoreLayer.IsEmpty())
		return;

	CFtrLayer *pStoreLayer = pDS->GetFtrLayer(m_StoreLayer);
	if(!pStoreLayer)
	{
		pStoreLayer = new CFtrLayer();
		pStoreLayer->SetName(m_StoreLayer);
		pDS->AddFtrLayer(pStoreLayer);
	}

	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_HandleLayer, arr);

	int nSum=0, i, j;
	for(i=0; i<arr.GetSize(); i++)
	{
		nSum += arr[i]->GetObjectCount();
	}

	CFeature *pFtrTemp = pStoreLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
	if (!pFtrTemp || !pFtrTemp->GetGeometry())
	{
		Abort();
		return;
	}

	GProgressStart(nSum);
	CUndoFtrs undo(m_pEditor, Name());
	for(i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo) continue;

			int nCls = pGeo->GetClassType();
			if(nCls==CLS_GEODCURVE)
			{
				CGeometry *pGeo1=NULL;
				CGeometry *pGeo2=NULL;
				((CGeoDCurve*)pGeo)->Separate(pGeo1, pGeo2);
				if(pGeo1 && pGeo2)
				{
					delete pGeo1;
					CFeature *pNew = pFtrTemp->Clone();
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo2->GetShape(arrPts);
					delete pGeo2;
					pNew->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());

					if(m_pEditor->AddObject(pNew, pStoreLayer->GetID()))
					{
						undo.AddNewFeature(FtrToHandle(pNew));
					}
				}
			}
			else if(nCls==CLS_GEOPARALLEL)
			{
				CGeometry *pGeo1=NULL;
				CGeometry *pGeo2=NULL;
				((CGeoParallel*)pGeo)->SeparateNoReverse(pGeo1, pGeo2);
				if(pGeo1 && pGeo2)
				{
					delete pGeo1;
					CFeature *pNew = pFtrTemp->Clone();
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo2->GetShape(arrPts);
					delete pGeo2;
					pNew->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());
					
					if(m_pEditor->AddObject(pNew, pStoreLayer->GetID()))
					{
						undo.AddNewFeature(FtrToHandle(pNew));
					}
				}
			}
			else if(nCls==CLS_GEOSURFACE)
			{
				CArray<PT_3DEX,PT_3DEX> arrPts, newPts;
				pGeo->GetShape(arrPts);
				int nPt = arrPts.GetSize();
				if(nPt<4) continue;

				for(int k=nPt/2; k<nPt-1; k++)
				{
					newPts.Add(arrPts[k]);
				}

				CGeoCurve *pCurve = new CGeoCurve();
				if(!pCurve->CreateShape(newPts.GetData(), newPts.GetSize()))
				{
					delete pCurve;
				}

				CFeature *pNew = pFtrTemp->Clone();
				pNew->SetGeometry(pCurve);
				
				if(m_pEditor->AddObject(pNew, pStoreLayer->GetID()))
				{
					undo.AddNewFeature(FtrToHandle(pNew));
				}
				else
				{
					delete pCurve;
				}
			}
		}
	}

	GProgressEnd();
	delete pFtrTemp;
	undo.Commit();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

//////////////////////////////////////////////////////////////////////
// CDelRoadSurfaceCommand Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDelRoadSurfaceCommand,CEditCommand)

CDelRoadSurfaceCommand::CDelRoadSurfaceCommand()
{
   m_nStep = -1;
   m_strLayer = _T("30");
}

CDelRoadSurfaceCommand::~CDelRoadSurfaceCommand()
{

}

CString CDelRoadSurfaceCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DELROAD);
}

void CDelRoadSurfaceCommand::Start()
{	
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
}

void CDelRoadSurfaceCommand::Abort()
{
	CEditCommand::Abort();
}

void CDelRoadSurfaceCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	CEditCommand::Finish();
}

void CDelRoadSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ModifyRoadBySunRule",Name());
	param->AddLayerNameParam("HandleLayer",(LPCTSTR)m_strLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER),NULL,LAYERPARAMITEM_NOTEMPTY_AREA);
	param->AddLayerNameParamEx("RoadLayers",(LPCTSTR)m_strRoadLayers,StrFromResID(IDS_ROAD_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
//	param->AddLayerNameParamEx("ExceptLayers",(LPCTSTR)m_strExceptLayers,_T("排除道路附属图层")/*StrFromResID(IDS_ROAD_LAYERS)*/);
}

void CDelRoadSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
		
	if( tab.GetValue(0,"HandleLayer",var) )
	{					
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strLayer.TrimLeft();
		m_strLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"RoadLayers",var) )
	{					
		m_strRoadLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strRoadLayers.TrimRight();
		m_strRoadLayers.TrimLeft();
		SetSettingsModifyFlag();		
	}

// 	if( tab.GetValue(0,"ExceptLayers",var) )
// 	{					
// 		m_strExceptLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
// 		m_strExceptLayers.TrimRight();
// 		m_strExceptLayers.TrimLeft();
// 		SetSettingsModifyFlag();		
// 	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CDelRoadSurfaceCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	
	var = (LPCTSTR)(m_strLayer);
	tab.AddValue("HandleLayer",&CVariantEx(var));

	var = (LPCTSTR)(m_strRoadLayers);
	tab.AddValue("RoadLayers",&CVariantEx(var));

// 	var = (LPCTSTR)(m_strExceptLayers);
// 	tab.AddValue("ExceptLayers",&CVariantEx(var));
}

extern BOOL convertStringToStrArray(LPCTSTR str,CStringArray &arr);

static BOOL IsLayerInArr(CFtrLayer *pLayer, CFtrLayerArray& arr)
{
	for(int i=0; i<arr.GetSize(); i++)
	{
		if(pLayer==arr[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CDelRoadSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ) return;

	CFtrLayer *pLayer = pDS->GetFtrLayer(m_strLayer);
	if(!pLayer) return;
	int nSum = pLayer->GetObjectCount();
	if(nSum<=0) return;

	CFtrLayerArray arr1, arr2;
	pDS->GetFtrLayersByNameOrCode(m_strRoadLayers, arr1);
//	pDS->GetFtrLayersByNameOrCode(m_strExceptLayers, arr2);

	//只让道路图层可查询
	pDS->SaveAllQueryFlags(TRUE,FALSE);
	
	int i,j,k;
	for(i=0; i<arr1.GetSize(); i++)
	{
		arr1[i]->SetAllowQuery(TRUE);
	}
	for(i=0; i<arr2.GetSize(); i++)
	{
		arr2[i]->SetAllowQuery(TRUE);
	}

	double r = GraphAPI::g_lfDisTolerance;
	CUndoFtrs undo(m_pEditor, Name());
	GProgressStart(nSum);
	for(i=0; i<nSum; i++)
	{
		GProgressStep();
		CFeature *pFtr = pLayer->GetObject(i);
		if(!pFtr || !pFtr->IsVisible())
			continue;

		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			continue;

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);
		int nSumOnRoad = 0;
		int nPt = pts.GetSize();
		for(j=0; j<nPt; j++)
		{
			Envelope e;
			e.CreateFromPtAndRadius(pts[j], r);
			pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);

			int num;
			const CPFeature* ftr= pDQ->GetFoundHandles(num);
			if(num<=0)
			{
				continue;
			}

			for(k=0; k<num; k++)
			{
				CFtrLayer *pLayer1 = pDS->GetFtrLayerOfObject(ftr[k]);
				if(IsLayerInArr(pLayer1, arr1))
				{
					nSumOnRoad++;
					break;
				}
			}
		}

		if(nSumOnRoad==nPt || (nPt>10 && 5*nSumOnRoad>4*nPt) )//80%
		{
			m_pEditor->DeleteObject(FtrToHandle(pFtr));
			undo.AddOldFeature(FtrToHandle(pFtr));
		}
	}
	m_pEditor->RefreshView();
	GProgressEnd();
	undo.Commit();
	pDS->RestoreAllQueryFlags();
	Finish();
	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CExplodeDcurveToSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CExplodeDcurveToSurfaceCommand,CEditCommand)

CExplodeDcurveToSurfaceCommand::CExplodeDcurveToSurfaceCommand()
{
	m_lfDis = 15;
    m_nStep = -1;
}

CExplodeDcurveToSurfaceCommand::~CExplodeDcurveToSurfaceCommand()
{

}

CString CExplodeDcurveToSurfaceCommand::Name()
{
	return StrFromResID(IDS_EXPOLODEDCURVE_TOSURFACE);
}

void CExplodeDcurveToSurfaceCommand::Start()
{
 	if( !m_pEditor )return;
	
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CExplodeDcurveToSurfaceCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("HandleLayer",&CVariantEx(var));

	var = (LPCTSTR)(m_StoreLayer);
	tab.AddValue("StoreLayer",&CVariantEx(var));

	var = (double)(m_lfDis);
	tab.AddValue("schdis",&CVariantEx(var));
}

void CExplodeDcurveToSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ExplodeDcurveToSurfaceCommand",Name());
	
	param->AddLayerNameParamEx("HandleLayer",(LPCTSTR)m_HandleLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx("StoreLayer",(LPCTSTR)m_StoreLayer,StrFromResID(IDS_CONVERTTO_LAYER));
	param->AddParam("schdis",(double)(m_lfDis),StrFromResID(IDS_CHK_CMD_DES_SEARCH_RADIUS));
}


void CExplodeDcurveToSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,"HandleLayer",var) )
	{					
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimRight();
		m_HandleLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"StoreLayer",var) )
	{					
		m_StoreLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_StoreLayer.TrimRight();
		m_StoreLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"schdis",var) )
	{					
		m_lfDis = (double)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CExplodeDcurveToSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	if(m_HandleLayer.IsEmpty()) return;

	CFtrLayerArray arr;
	CStringArray strs0, strs;

	convertStringToStrArray(m_HandleLayer, strs0);
	convertStringToStrArray(m_StoreLayer, strs);
	int nSum=0, i;
	for(i=0; i<strs0.GetSize(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(strs0[i]);
		arr.Add(pLayer);
		if(pLayer)
		{
			nSum += pLayer->GetObjectCount();
		}
	}

	pDS->SaveAllQueryFlags(TRUE, FALSE);

	GProgressStart(nSum);
	CUndoFtrs undo(m_pEditor, Name());
	for(i=0; i<arr.GetSize(); i++)
	{
		if(arr[i]==NULL) continue;
		if(arr[i]->GetObjectCount()<2)
			continue;

		CFtrLayer *pStoreLayer = NULL;
		if(i<strs.GetSize() && !strs.GetAt(i).IsEmpty())
		{
			pStoreLayer = pDS->GetFtrLayer(strs[i]);
			if(!pStoreLayer)
			{
				pStoreLayer = new CFtrLayer();
				pStoreLayer->SetName(strs[i]);
				pDS->AddFtrLayer(pStoreLayer);
			}
		}
		else
		{
			pStoreLayer = arr[i];
		}

		arr[i]->SetAllowQuery(TRUE);
		ToSurface(arr[i], pStoreLayer, undo);
		arr[i]->SetAllowQuery(FALSE);
	}

	GProgressEnd();
	
	undo.Commit();
	pDS->RestoreAllQueryFlags();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

void CExplodeDcurveToSurfaceCommand::ToSurface(CFtrLayer* pLayer, CFtrLayer* pStoreLayer, CUndoFtrs& undo)
{
	if(!pLayer || !pStoreLayer) return;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ) return;
	int nObj = pLayer->GetObjectCount(), j, k;
	if(nObj<2) return;

	CFeature *pFtrTemp = pStoreLayer->CreateDefaultFeature(m_pEditor->GetDataSource()->GetScale(),CLS_GEOSURFACE);
	if (!pFtrTemp || !pFtrTemp->GetGeometry())
	{
		return;
	}

	for(j=0; j<nObj; j++)
	{
		GProgressStep();
		CFeature *pFtr = pLayer->GetObject(j);
		if(!pFtr || !pFtr->IsVisible())
			continue;
		
		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			continue;
		
		CArray<PT_3DEX,PT_3DEX> pts, pts1, newPts;
		pGeo->GetShape(pts);
		int nPt = pts.GetSize();
		if(nPt<2) continue;
		
		PT_3D sch_pt = pts[0];
		double r;
		Envelope e;
		e.CreateFromPtAndRadius(pts[0], m_lfDis);
		pDQ->FindObjectInRect(e, NULL, FALSE, FALSE);
		int nFound;
		const CPFeature *ftr = pDQ->GetFoundHandles(nFound);
		if(nFound!=2) continue;
		
		CFeature *pFtr1 = ftr[0];
		if(pFtr1==pFtr) pFtr1 = ftr[1];
		CGeometry *pGeo1 = pFtr1->GetGeometry();
		if(!pGeo1 || !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			continue;
		pGeo1->GetShape(pts1);
		int nPt1 = pts1.GetSize();
		if(nPt1<2) continue;
		
		double dis1 = GraphAPI::GGetDisOf2P2D(pts[0].x, pts[0].y, pts1[0].x, pts1[0].y);
		double dis2 = GraphAPI::GGetDisOf2P2D(pts[0].x, pts[0].y, pts1[nPt1-1].x, pts1[nPt1-1].y);
		if(dis1<dis2)
		{
			for(k=0; k<nPt1/2; k++)
			{
				PT_3DEX temp = pts1[k];
				pts1[k] = pts1[nPt1-1-k];
				pts1[nPt1-1-k] = temp;
			}
		}
		
		newPts.Copy(pts);
		newPts.Append(pts1);
		if(0==GraphAPI::GIsClockwise(newPts.GetData(), newPts.GetSize()))
		{
			int nPt2 = newPts.GetSize();
			for(k=0; k<nPt2/2; k++)
			{
				PT_3DEX temp = newPts[k];
				newPts[k] = newPts[nPt2-1-k];
				newPts[nPt2-1-k] = temp;
			}
		}
		
		newPts.Add(newPts[0]);
		
		CGeoSurface *pObj = new CGeoSurface();
		if(!pObj->CreateShape(newPts.GetData(), newPts.GetSize()))
		{
			delete pObj;
		}
		
		CFeature *pNew = pFtrTemp->Clone();
		pNew->SetGeometry(pObj);
		
		if(!m_pEditor->AddObject(pNew, pStoreLayer->GetID()))
		{
			delete pObj;
			continue;
		}
		
		undo.AddNewFeature(FtrToHandle(pNew));
		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		undo.AddOldFeature(FtrToHandle(pFtr));
		m_pEditor->DeleteObject(FtrToHandle(pFtr1));
		undo.AddOldFeature(FtrToHandle(pFtr1));
	}

	delete pFtrTemp;
}


//////////////////////////////////////////////////////////////////////
// CRepairZCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CRepairZCommand,CEditCommand)

CRepairZCommand::CRepairZCommand()
{

}

CRepairZCommand::~CRepairZCommand()
{

}

CString CRepairZCommand::Name()
{
	return StrFromResID(IDS_CMDPNAME_REPAIRZ);
}

void CRepairZCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CRepairZCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ || !pDS)
		return;

	int nSum=0, i, j, k;
	int nLay = pDS->GetFtrLayerCount();
	for(i=0; i<nLay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer || !pLayer->IsVisible())
			continue;

		nSum += pLayer->GetObjectCount();
	}

	CUndoModifyProperties undo(m_pEditor,Name());
	GProgressStart(nSum);
	for(i=0; i<nLay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer || !pLayer->IsVisible())
			continue;
		
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo) continue;

			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);

			CVariantEx varEx;
			varEx.SetAsShape(pts);
			CValueTable oldtab, tab;
			oldtab.BeginAddValueItem();
			oldtab.AddValue(FIELDNAME_SHAPE,&varEx);
			oldtab.EndAddValueItem();
			
			BOOL bChanged = FALSE;
			for(k=0; k<pts.GetSize(); k++)
			{
				if(pts[k].z<-1000)
				{
					Envelope e;
					e.CreateFromPtAndRadius(pts[k], 0.01);
					pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);

					int num;
					const CPFeature *ftr = pDQ->GetFoundHandles(num);
					for(int m=0;m<num;m++)
					{
						if(ftr[m]==pFtr) continue;

						CGeometry *pGeo1 = ftr[m]->GetGeometry();
						if(!pGeo1) continue;
						CArray<PT_3DEX,PT_3DEX> pts1;
						pGeo1->GetShape(pts1);

						PT_KEYCTRL keypt = pGeo1->FindNearestKeyCtrlPt(pts[k], 0.01, NULL, 1);
						if(keypt.IsValid())
						{
							COPY_3DPT(pts[k], pts1[keypt.index]);
							bChanged = TRUE;
							break;
						}
					}
					if(m>=num)
					{
						pts[k].z = ( k==0 ? 0 : pts[0].z );
						bChanged = TRUE;
					}
				}
			}
			if(bChanged)
			{
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
				pGeo->CreateShape(pts.GetData(), pts.GetSize());
				m_pEditor->RestoreObject(FtrToHandle(pFtr));

				varEx.SetAsShape(pts);	
				tab.BeginAddValueItem();
				tab.AddValue(FIELDNAME_SHAPE,&varEx);
				tab.EndAddValueItem();
				
				undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,TRUE);
			}
		}
	}

	undo.Commit();
	GProgressEnd();
	Finish();
}



//////////////////////////////////////////////////////////////////////
// CRepairLineTypeCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CRepairLineTypeCommand,CEditCommand)

CRepairLineTypeCommand::CRepairLineTypeCommand()
{

}

CRepairLineTypeCommand::~CRepairLineTypeCommand()
{

}

CString CRepairLineTypeCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_REPAIR_LINETYPE);
}

void CRepairLineTypeCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CRepairLineTypeCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ || !pDS)
		return;

	int nSum=0, i, j, k;
	int nLay = pDS->GetFtrLayerCount();
	for(i=0; i<nLay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer || !pLayer->IsVisible())
			continue;

		nSum += pLayer->GetObjectCount();
	}

	CUndoModifyProperties undo(m_pEditor,Name());
	GProgressStart(nSum);
	int count = 0;
	for(i=0; i<nLay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer || !pLayer->IsVisible())
			continue;
		
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo) continue;

			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);

			CVariantEx varEx;
			varEx.SetAsShape(pts);
			CValueTable oldtab, tab;
			oldtab.BeginAddValueItem();
			oldtab.AddValue(FIELDNAME_SHAPE,&varEx);
			oldtab.EndAddValueItem();
			
			BOOL bChanged = FALSE;

			int nCls = pGeo->GetClassType();
			if(nCls==CLS_GEOPARALLEL || nCls==CLS_GEODCURVE)
			{
				continue;
			}
			else if(nCls==CLS_GEOMULTISURFACE)
			{
				for(k=0; k<pts.GetSize(); k++)
				{
					if(k==0 && pts[0].pencode!=penLine)
					{
						pts[0].pencode = penLine;
						bChanged = TRUE;
						continue;
					}

					if(pts[k].pencode!=penLine)
					{
						for(int m=k+1;m<pts.GetSize();m++)
						{
							if(GraphAPI::GIsEqual2DPoint(&pts[k], &pts[m]))
							{
								if(pts[k].pencode!=penMove || pts[m].pencode!=penLine)
								{
									pts[k].pencode = penMove;
									pts[m].pencode = penLine;
									bChanged = TRUE;
								}
								break;
							}
						}
						if(m>=pts.GetSize())
						{
							pts[k].pencode = penLine;
							bChanged = TRUE;
						}
					}
				}
			}
			else
			{
				for(k=0; k<pts.GetSize(); k++)
				{
					if(pts[k].pencode!=penLine)
					{
						pts[k].pencode = penLine;
						bChanged = TRUE;
					}
				}
			}
			
			if(bChanged)
			{
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
				pGeo->CreateShape(pts.GetData(), pts.GetSize());
				m_pEditor->RestoreObject(FtrToHandle(pFtr));

				varEx.SetAsShape(pts);	
				tab.BeginAddValueItem();
				tab.AddValue(FIELDNAME_SHAPE,&varEx);
				tab.EndAddValueItem();
				
				undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,TRUE);
				count ++;
			}
		}
	}

	CString tip;
	tip.Format(IDS_MODIFID_OBJSUM, count);
	AfxMessageBox(tip);
	undo.Commit();
	GProgressEnd();
	Finish();
}


//////////////////////////////////////////////////////////////////////
// CDHeightToPointCommand Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDHeightToPointCommand, CEditCommand)

CDHeightToPointCommand::CDHeightToPointCommand()
{
	m_strField = _T("HEIGHT");
}

CDHeightToPointCommand::~CDHeightToPointCommand()
{

}

CString CDHeightToPointCommand::Name()
{
	return StrFromResID(IDS_CMDPNAME_DHEIGHT_TO_POINT);
}

void CDHeightToPointCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CDHeightToPointCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_strField);
	tab.AddValue("strField", &CVariantEx(var));

	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("HandleLayer", &CVariantEx(var));
}

void CDHeightToPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ExplodeDcurveToSurfaceCommand", Name());

	param->AddLayerNameParamEx("HandleLayer", (LPCTSTR)m_HandleLayer, StrFromResID(IDS_CMDPLANE_HANDLELAYER), NULL, LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam("strField", (LPCTSTR)(m_strField), StrFromResID(IDS_CHK_PARAM_ATTRI_NAME));
}


void CDHeightToPointCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "HandleLayer", var))
	{
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimRight();
		m_HandleLayer.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "strField", var))
	{
		m_strField = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strField.TrimRight();
		m_strField.TrimLeft();
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CDHeightToPointCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor || m_HandleLayer.IsEmpty())
		return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;


	CFtrLayerArray arrLayers;
	int i = 0, j = 0;
	pDS->GetFtrLayersByNameOrCode_editable(m_HandleLayer, arrLayers);

	CUndoFtrs undo(m_pEditor, Name());
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for (j = 0; j < nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if (!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				continue;
			}

			CArray<PT_3DEX, PT_3DEX> arrPts, pts;
			pGeo->GetShape(arrPts);
			if (arrPts.GetSize() != 2) continue;

			double height = arrPts[0].z - arrPts[1].z;

			pts.Add(arrPts[0]);

			CFeature *pNew = pFeature->Clone();
			if (!pNew) continue;

			if (!pNew->CreateGeometry(CLS_GEOPOINT)) continue;

			pNew->GetGeometry()->CreateShape(pts.GetData(), pts.GetSize());

			if (!m_pEditor->AddObject(pNew, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFeature))))
			{
				delete pNew;
				continue;
			}

			CValueTable tab;
			tab.BeginAddValueItem();
			_variant_t var(height);
			tab.AddValue(m_strField, &CVariantEx(var));
			tab.EndAddValueItem();

			GETXDS(m_pEditor)->CopyXAttributes(pFeature, pNew);
			GETXDS(m_pEditor)->SetXAttributes(pNew, tab);
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
			undo.AddOldFeature(FtrToHandle(pFeature));
			undo.AddNewFeature(FTR_HANDLE(pNew));
		}
	}

	undo.Commit();

	CCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// CDCurveSplitCommand Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDCurveSplitCommand, CEditCommand)

CDCurveSplitCommand::CDCurveSplitCommand()
{

}

CDCurveSplitCommand::~CDCurveSplitCommand()
{

}

CString CDCurveSplitCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_SPLTPARALLEL);
}

void CDCurveSplitCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CDCurveSplitCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("HandleLayer", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer1);
	tab.AddValue("strLayer1", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer2);
	tab.AddValue("strLayer2", &CVariantEx(var));
}

void CDCurveSplitCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ExplodeDcurveToSurfaceCommand", Name());

	param->AddLayerNameParamEx("HandleLayer", (LPCTSTR)m_HandleLayer, StrFromResID(IDS_CMDPLANE_HANDLELAYER), NULL, LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx("strLayer1", (LPCTSTR)m_strLayer1, StrFromResID(IDS_CMDNAME_COPY_BASELINE), NULL, LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx("strLayer2", (LPCTSTR)m_strLayer2, StrFromResID(IDS_CMDPLANE_ASSISTLAYER), NULL, LAYERPARAMITEM_NOTEMPTY_LINE);
}


void CDCurveSplitCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "HandleLayer", var))
	{
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimRight();
		m_HandleLayer.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "strLayer1", var))
	{
		m_strLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strLayer1.TrimRight();
		m_strLayer1.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "strLayer2", var))
	{
		m_strLayer2 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strLayer2.TrimRight();
		m_strLayer2.TrimLeft();
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CDCurveSplitCommand::ReadFileStrings()
{
	if (!PathFileExists(m_HandleLayer))
		return;

	FILE *fp = fopen(m_HandleLayer, "r");
	if (fp)
	{
		char line[4096];
		int pos = 0;
		while (!feof(fp))
		{
			memset(line, 0, sizeof(line));
			fgets(line, sizeof(line) - 1, fp);

			if (pos == 0)
			{
				m_HandleLayer = CString(line);
				m_HandleLayer.TrimRight();
			}
			else if (pos == 1)
			{
				m_strLayer1 = CString(line);
				m_strLayer1.TrimRight();
			}
			else if (pos == 2)
			{
				m_strLayer2 = CString(line);
				m_strLayer2.TrimRight();
			}
			pos++;
		}
		fclose(fp);
		fp = NULL;
	}
}

void CDCurveSplitCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor) return;
	if (m_HandleLayer.IsEmpty())
	{
		AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	ReadFileStrings();

	if (m_strLayer1.IsEmpty() || m_strLayer2.IsEmpty())
	{
		AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;

	CFtrLayerArray arrLayers;
	CStringArray arr1, arr2;
	int i = 0, j = 0;
	pDS->GetFtrLayersByNameOrCode_editable(m_HandleLayer, arrLayers);
	convertStringToStrArray(m_strLayer1, arr1);
	convertStringToStrArray(m_strLayer2, arr2);
	int nSize = arrLayers.GetSize();
	int nSize1 = arr1.GetSize();
	int nSize2 = arr2.GetSize();
	if (nSize != nSize1 || nSize != nSize2)
	{
		AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	CUndoFtrs undo(m_pEditor, Name());
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();

		CFtrLayer *pTargetLayer1 = pDS->GetFtrLayer(arr1.GetAt(i));
		if (!pTargetLayer1) continue;
		CFtrLayer *pTargetLayer2 = pDS->GetFtrLayer(arr2.GetAt(i));
		if (!pTargetLayer2) continue;

		CFeature *pTemp1 = pTargetLayer1->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(), CLS_GEOCURVE);
		CFeature *pTemp2 = pTargetLayer2->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(), CLS_GEOCURVE);


		for (j = 0; j < nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if (!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo) continue;

			CGeometry* pObj1 = NULL;
			CGeometry* pObj2 = NULL;
			CArray<PT_3DEX, PT_3DEX> pts1, pts2;

			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
				((CGeoDCurve*)pGeo)->Separate(pObj1, pObj2);
				if (!pObj1 || !pObj2) continue;
				pObj1->GetShape(pts1);
				pObj2->GetShape(pts2);
				delete pObj1;
				delete pObj2;
			}
			else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			{
				((CGeoParallel*)pGeo)->Separate(pObj1, pObj2);
				if (!pObj1 || !pObj2) continue;
				pObj1->GetShape(pts1);
				pObj2->GetShape(pts2);
				delete pObj1;
				delete pObj2;
			}
			else
			{
				continue;
			}

			CFeature *pNew1 = pTemp1->Clone();
			pNew1->GetGeometry()->CreateShape(pts1.GetData(), pts1.GetSize());
			if (!m_pEditor->AddObject(pNew1, pTargetLayer1->GetID()))
			{
				delete pNew1;
				continue;
			}
			undo.AddNewFeature(FTR_HANDLE(pNew1));

			CFeature *pNew2 = pTemp2->Clone();
			pNew2->GetGeometry()->CreateShape(pts2.GetData(), pts2.GetSize());
			if (!m_pEditor->AddObject(pNew2, pTargetLayer2->GetID()))
			{
				delete pNew2;
				continue;
			}
			undo.AddNewFeature(FTR_HANDLE(pNew2));

			m_pEditor->DeleteObject(FtrToHandle(pFeature));
			undo.AddOldFeature(FtrToHandle(pFeature));
		}

		delete pTemp1;
		delete pTemp2;
	}

	undo.Commit();

	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CTextToSurfacePointCommand Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTextToSurfacePointCommand, CEditCommand)

CTextToSurfacePointCommand::CTextToSurfacePointCommand()
{
}

CTextToSurfacePointCommand::~CTextToSurfacePointCommand()
{
}

CString CTextToSurfacePointCommand::Name()
{
	return StrFromResID(IDS_MODIFY_TEXTTOSURFACEPOINT);
}

void CTextToSurfacePointCommand::Start()
{
	if (!m_pEditor)return;
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CTextToSurfacePointCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue(PF_HANDLELAYERS, &CVariantEx(var));

	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue(PF_LAYERNAME, &CVariantEx(var));
}

void CTextToSurfacePointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("PointToSurfacePoint", Name());

	param->AddLayerNameParamEx(PF_HANDLELAYERS, (LPCTSTR)m_handleLayers, StrFromResID(IDS_SRC_LAYER), NULL, LAYERPARAMITEM_POINT);

	param->AddLayerNameParam(PF_LAYERNAME, (LPCTSTR)m_targetLayer, StrFromResID(IDS_DES_LAYER), NULL, LAYERPARAMITEM_POINT);
}

void CTextToSurfacePointCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_HANDLELAYERS, var))
	{
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, PF_LAYERNAME, var))
	{
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimRight();
		m_targetLayer.TrimLeft();
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CTextToSurfacePointCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)
		return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;

	CFtrLayer *out_layer = pDS->GetFtrLayer((LPCTSTR)m_targetLayer);
	if (out_layer == NULL && m_targetLayer.GetLength() > 0)
	{
		out_layer = new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_targetLayer);
		pDS->AddFtrLayer(out_layer);
	}

	int layid = -1;
	if (out_layer)
	{
		layid = out_layer->GetID();
	}

	CFtrLayerArray arrLayers;
	int i = 0, j = 0;
	pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers, arrLayers);

	CUndoFtrs undo(m_pEditor, Name());

	CFeature *pTempFtr = NULL;
	if (out_layer)
	{
		pTempFtr = out_layer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOSURFACEPOINT);
		if (!pTempFtr) return;
	}

	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for (j = 0; j < nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if (!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				continue;
			}

			CArray<PT_3DEX, PT_3DEX> pts;
			pGeo->GetShape(pts);

			CFeature *pNew = pTempFtr ? pTempFtr->Clone() : pFeature->Clone();
			if (!pNew) continue;

			if (!pNew->CreateGeometry(CLS_GEOSURFACEPOINT)) continue;

			pNew->GetGeometry()->CreateShape(pts.GetData(), pts.GetSize());

			if (!m_pEditor->AddObject(pNew, layid == -1 ? pLayer->GetID() : layid))
			{
				delete pNew;
				continue;
			}

			if (layid == -1)
			{
				GETXDS(m_pEditor)->CopyXAttributes(pFeature, pNew);
			}
			undo.AddNewFeature(FTR_HANDLE(pNew));
			undo.AddOldFeature(FtrToHandle(pFeature));
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
		}
	}

	if (pTempFtr) delete pTempFtr;

	undo.Commit();

	CCommand::PtClick(pt, flag);
}



//////////////////////////////////////////////////////////////////////
// CAnnoToSurfacePointCommand Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAnnoToSurfacePointCommand, CEditCommand)

CAnnoToSurfacePointCommand::CAnnoToSurfacePointCommand()
{
}

CAnnoToSurfacePointCommand::~CAnnoToSurfacePointCommand()
{
}

CString CAnnoToSurfacePointCommand::Name()
{
	return StrFromResID(IDS_MODIFY_ANNOTOSURFACEPOINT);
}

void CAnnoToSurfacePointCommand::Start()
{
	if (!m_pEditor)return;
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CAnnoToSurfacePointCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_strcode);
	tab.AddValue(PF_HANDLELAYERS, &CVariantEx(var));
}

void CAnnoToSurfacePointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("PointToSurfacePoint", Name());

	param->AddFileNameParam(PF_HANDLELAYERS, (LPCTSTR)m_strcode, "*.txt|*.txt||", StrFromResID(IDS_TABLE_FILE));
}

void CAnnoToSurfacePointCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_HANDLELAYERS, var))
	{
		m_strcode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strcode.TrimLeft();
		m_strcode.TrimRight();
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CAnnoToSurfacePointCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)  return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;

	if (!PathFileExists(m_strcode))
		return;

	FILE *fp = fopen(m_strcode, "r");
	if (!fp) return;

	char src[128];
	char dst[128];
	CStringArray arr1, arr2;
	while (!feof(fp))
	{
		if (2 == fscanf(fp, "%s%s", src, dst))
		{
			arr1.Add(src);
			arr2.Add(dst);
		}
	}
	fclose(fp);

	int nSize = arr1.GetSize();
	if (nSize <= 0 || arr2.GetSize() != nSize)
		return;

	CUndoFtrs undo(m_pEditor, Name());

	for (int i = 0; i < nSize; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer((LPCTSTR)arr1[i]);
		if (!pLayer) continue;
		int nObj = pLayer->GetObjectCount();
		if (nObj < 0) continue;

		CFtrLayer *out_layer = pDS->GetFtrLayer((LPCTSTR)arr2[i]);
		if (out_layer == NULL)
		{
			out_layer = new CFtrLayer();
			out_layer->SetName((LPCTSTR)arr2[i]);
			pDS->AddFtrLayer(out_layer);
			if (!out_layer) continue;
		}

		CFeature *pTempFtr = out_layer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOSURFACEPOINT);
		if (!pTempFtr) continue;

		for (int j = 0; j < nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if (!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				continue;
			}

			CArray<PT_3DEX, PT_3DEX> pts;
			pGeo->GetShape(pts);

			CFeature *pNew = pTempFtr->Clone();
			if (!pNew) continue;

			if (!pNew->CreateGeometry(CLS_GEOSURFACEPOINT)) continue;

			pNew->GetGeometry()->CreateShape(pts.GetData(), pts.GetSize());

			if (!m_pEditor->AddObject(pNew, out_layer->GetID()))
			{
				delete pNew;
				continue;
			}

			undo.AddNewFeature(FTR_HANDLE(pNew));
			undo.AddOldFeature(FtrToHandle(pFeature));
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
		}

		if (pTempFtr) delete pTempFtr;
	}

	undo.Commit();

	CCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// CChangeDcurveByWidthCommand Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CChangeDcurveByWidthCommand, CEditCommand)

CChangeDcurveByWidthCommand::CChangeDcurveByWidthCommand()
{
	m_bExplode = FALSE;
}

CChangeDcurveByWidthCommand::~CChangeDcurveByWidthCommand()
{

}

CString CChangeDcurveByWidthCommand::Name()
{
	return StrFromResID(IDS_CHANGEDCURVEBYWIDTH);
}

void CChangeDcurveByWidthCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CChangeDcurveByWidthCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("HandleLayer", &CVariantEx(var));

	var = (LPCTSTR)(m_strWidth);
	tab.AddValue("strWidth", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayers);
	tab.AddValue("strLayers", &CVariantEx(var));

	var = (bool)m_bExplode;
	tab.AddValue("bExplode", &CVariantEx(var));
}

void CChangeDcurveByWidthCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ExplodeDcurveToSurfaceCommand", Name());

	param->AddLayerNameParam("HandleLayer", (LPCTSTR)m_HandleLayer, StrFromResID(IDS_PARALLEL_LAYER), NULL, LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam("strWidth", (LPCTSTR)m_strWidth, StrFromResID(IDS_WIDTH_CONDITION));
	param->AddLayerNameParamEx("strLayers", (LPCTSTR)m_strLayers, StrFromResID(IDS_CONVERTTO_LAYER), NULL, LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam("bExplode", (bool)m_bExplode, StrFromResID(IDS_CMDNAME_SEPPARALLEL));
}


void CChangeDcurveByWidthCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "HandleLayer", var))
	{
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimRight();
		m_HandleLayer.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "strWidth", var))
	{
		m_strWidth = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strWidth.TrimRight();
		m_strWidth.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "strLayers", var))
	{
		m_strLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strLayers.TrimRight();
		m_strLayers.TrimLeft();
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "bExplode", var))
	{
		m_bExplode = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CChangeDcurveByWidthCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor) return;

	if (m_strWidth.IsEmpty() || m_strLayers.IsEmpty())
	{
		AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;

	CFtrLayer* pHandleLayer = pDS->GetFtrLayer(m_HandleLayer);
	if (!pHandleLayer) return;

	CStringArray arr1, arr2;
	convertStringToStrArray(m_strWidth, arr1);
	convertStringToStrArray(m_strLayers, arr2);
	int nSize1 = arr1.GetSize();
	int nSize2 = arr2.GetSize();
	if (nSize1 != nSize2)
	{
		AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	//处理宽度组合
	double *widths = new double[nSize1 * 2];
	int i = 0, j = 0;
	for (i = 0; i < nSize1; i++)
	{
		CString str = arr1[i];
		int pos = str.Find('~');
		if (pos > 0)
		{
			widths[2 * i] = strtod(str.Left(pos), NULL);
			widths[2 * i + 1] = strtod(str.Mid(pos + 1), NULL);
		}
		else
		{
			AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
			return;
		}
	}

	//创建Feature模板
	int *ids = new int[nSize2];
	FTR_HANDLE *pTempFtrs = new FTR_HANDLE[nSize2];
	int scale = GETDS(m_pEditor)->GetScale();
	int nCls = m_bExplode ? CLS_GEOCURVE : CLS_GEOPARALLEL;
	for (i = 0; i < nSize2; i++)
	{
		CFtrLayer *pLayer = pDS->GetOrCreateFtrLayer(arr2[i]);
		if (pLayer)
		{
			ids[i] = pLayer->GetID();
			pTempFtrs[i] = FtrToHandle(pLayer->CreateDefaultFeature(scale, nCls));
		}
		else
		{
			ids[i] = -1;
			pTempFtrs[i] = NULL;
		}

	}

	CUndoFtrs undo(m_pEditor, Name());
	int nObj = pHandleLayer->GetObjectCount();
	for (i = 0; i < nObj; i++)
	{
		CFeature *pFeature = pHandleLayer->GetObject(i);
		if (!pFeature) continue;
		CGeometry *pGeo = pFeature->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			continue;

		double width = ((CGeoParallel*)pGeo)->GetWidth();

		for (j = 0; j<nSize1; j++)
		{
			if (fabs(width)>widths[2 * j] && fabs(width) < widths[2 * j + 1])
			{
				break;
			}
		}

		if (j >= nSize1 || pTempFtrs[j] == NULL || ids[j] < 0)
			continue;

		if (m_bExplode)
		{
			CGeometry* pObj1 = NULL;
			CGeometry* pObj2 = NULL;
			CArray<PT_3DEX, PT_3DEX> pts1, pts2;
			((CGeoParallel*)pGeo)->Separate(pObj1, pObj2);
			if (!pObj1 || !pObj2) continue;
			pObj1->GetShape(pts1);
			pObj2->GetShape(pts2);
			delete pObj1;
			delete pObj2;

			CFeature *pNew1 = HandleToFtr(pTempFtrs[j])->Clone();
			pNew1->GetGeometry()->CreateShape(pts1.GetData(), pts1.GetSize());
			if (!m_pEditor->AddObject(pNew1, ids[j]))
			{
				delete pNew1;
				continue;
			}

			CFeature *pNew2 = HandleToFtr(pTempFtrs[j])->Clone();
			pNew2->GetGeometry()->CreateShape(pts2.GetData(), pts2.GetSize());
			if (!m_pEditor->AddObject(pNew2, ids[j]))
			{
				delete pNew2;
				continue;
			}
			undo.AddNewFeature(FTR_HANDLE(pNew1));
			undo.AddNewFeature(FTR_HANDLE(pNew2));
		}
		else
		{
			CArray<PT_3DEX, PT_3DEX> pts;
			pGeo->GetShape(pts);
			CFeature *pNew = HandleToFtr(pTempFtrs[j])->Clone();
			CGeometry *pNewGeo = pNew->GetGeometry();
			pNewGeo->CreateShape(pts.GetData(), pts.GetSize());
			PT_3DEX expt = ((CGeoParallel*)pGeo)->GetCtrlPoint(0);
			expt.pencode = 70;
			((CGeoParallel*)pNewGeo)->SetCtrlPoint(0, expt);
			if (!m_pEditor->AddObject(pNew, ids[j]))
			{
				delete pNew;
				continue;
			}
			undo.AddNewFeature(FTR_HANDLE(pNew));
		}

		m_pEditor->DeleteObject(FtrToHandle(pFeature));
		undo.AddOldFeature(FtrToHandle(pFeature));
	}

	undo.Commit();

	delete[] widths;
	delete[] ids;
	for (i = 0; i < nSize2; i++)
	{
		if (pTempFtrs[i])
		{
			delete HandleToFtr(pTempFtrs[i]);
		}
	}

	CCommand::PtClick(pt, flag);
}




//////////////////////////////////////////////////////////////////////
// CPointtoDirPointCommand Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPointtoDirPointCommand, CEditCommand)

CPointtoDirPointCommand::CPointtoDirPointCommand()
{
	m_HandleLayer = _T("");
	m_strLayer = _T("");
	m_bCopyAttr = TRUE;
}

CPointtoDirPointCommand::~CPointtoDirPointCommand()
{

}

CString CPointtoDirPointCommand::Name()
{
	return StrFromResID(IDS_CMDPNAME_POINT_TO_DIRPOINT);
}

void CPointtoDirPointCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CPointtoDirPointCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("HandleLayer", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer);
	tab.AddValue("strLayer", &CVariantEx(var));

	var = (bool)m_bCopyAttr;
	tab.AddValue("CopyAttr", &CVariantEx(var));
}

void CPointtoDirPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ExplodeDcurveToSurfaceCommand", Name());

	param->AddLayerNameParamEx("HandleLayer", (LPCTSTR)m_HandleLayer, StrFromResID(IDS_CMDPLANE_HANDLELAYER), NULL, LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddLayerNameParam("strLayer", (LPCTSTR)m_strLayer, StrFromResID(IDS_CONVERTTO_LAYER), NULL, LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddParam("CopyAttr", (bool)m_bCopyAttr, StrFromResID(IDS_CMDNAME_COPYEXT));
}


void CPointtoDirPointCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "HandleLayer", var))
	{
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimRight();
		m_HandleLayer.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "strLayer", var))
	{
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_strLayer.TrimRight();
		m_strLayer.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "CopyAttr", var))
	{
		m_bCopyAttr = (bool)(_variant_t)*var;
	}

	CEditCommand::SetParams(tab, bInit);
}

void CPointtoDirPointCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor || m_HandleLayer.IsEmpty())
		return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;

	int layid = -1;
	if (!m_strLayer.IsEmpty())
	{
		CFtrLayer *pTagLayer = pDS->GetOrCreateFtrLayer(m_strLayer);
		if (!pTagLayer) return;
		layid = pTagLayer->GetID();
	}

	CFtrLayerArray arrLayers;
	int i = 0, j = 0;
	pDS->GetFtrLayersByNameOrCode_editable(m_HandleLayer, arrLayers);

	CUndoFtrs undo(m_pEditor, Name());
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for (j = 0; j < nObj; j++)
		{
			CFeature *pFeature = pLayer->GetObject(j);
			if (!pFeature) continue;
			CGeometry *pGeo = pFeature->GetGeometry();
			if (!pGeo || pGeo->GetClassType() != CLS_GEOPOINT)
			{
				continue;
			}

			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			if (arrPts.GetSize() < 1) continue;

			double angle = ((CGeoPoint*)pGeo)->GetDirection();

			CFeature *pNew = pFeature->Clone();
			if (!pNew) continue;

			if (!pNew->CreateGeometry(CLS_GEODIRPOINT)) continue;

			pNew->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());

			((CGeoPoint*)pNew->GetGeometry())->SetDirection(angle);

			if (!m_pEditor->AddObject(pNew, layid==-1?pLayer->GetID():layid))
			{
				delete pNew;
				continue;
			}

			if (m_bCopyAttr)
			{
				GETXDS(m_pEditor)->CopyXAttributes(pFeature, pNew);
			}
			m_pEditor->DeleteObject(FtrToHandle(pFeature));
			undo.AddOldFeature(FtrToHandle(pFeature));
			undo.AddNewFeature(FTR_HANDLE(pNew));
		}
	}

	undo.Commit();

	CCommand::PtClick(pt, flag);
}