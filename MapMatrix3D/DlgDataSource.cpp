// DlgDataSource.cpp: implementation of the CDlgDataSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "Access.h"
#include "DlgDataSource.h"
#include "Symbol.h"
#include "Scheme.h "
#include "SymbolLib.h "
#include "Feature.h"
#include "SQLiteAccess.h "
#include "SmartViewFunctions.h"
#include "regdef.h"
#include "regdef2.h"
#include "GeoText.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CDlgDataQuery::CDlgDataQuery()
{
}

CDlgDataQuery:: ~CDlgDataQuery()
{
	
}


CDlgDataSource::CDlgDataSource(CDataQueryEx *pDQ)
: CDataSourceEx(pDQ)
{
	m_bReadTMParams = FALSE;
	m_bSurfaceInsideSelect = AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_SURFACEINSIDESELECT,TRUE);
}

CDlgDataSource::~CDlgDataSource()
{
	
}


void CDlgDataSource::CreateSearchableUnit(CFeature *pObj, CPtrArray& arrPUnits)
{
	CDataSourceEx::CreateSearchableUnit(pObj, arrPUnits);
}



TMProjectionZone CDlgDataSource::GetProjection()
{
	if( !m_bReadTMParams )
		ReadTMParams();
	
	return m_projection;
}

TMDatum CDlgDataSource::GetDatum()
{
	if( !m_bReadTMParams )
		ReadTMParams();
	
	return m_datum;
}

void CDlgDataSource::SetProjection(TMProjectionZone& prj)
{
	m_projection = prj;
	
	SaveTMParams();
}


void CDlgDataSource::SetDatum(TMDatum& datum)
{
	m_datum = datum;
	
	SaveTMParams();
}

BOOL CDlgDataSource::ReadTMParams()
{
	CString name,value,type;
	
	BOOL bRet = GetDataSettings("TM_zoneName",name,value,type);
	strncpy(m_projection.zoneName,value,sizeof(m_projection.zoneName)-1);
	
	GetDataSettings("TM_tmName",name,value,type);
	strncpy(m_projection.tmName,value,sizeof(m_projection.tmName)-1);
	
	m_projection.central = GetDataSettingsDouble("TM_central",m_projection.central);
	m_projection.origin_Lat = GetDataSettingsDouble("TM_origin_Lat",m_projection.origin_Lat);
	m_projection.scale = GetDataSettingsDouble("TM_scale",m_projection.scale);
	m_projection.false_Easting = GetDataSettingsDouble("TM_false_Easting",m_projection.false_Easting);
	m_projection.false_Northing = GetDataSettingsDouble("TM_false_Northing",m_projection.false_Northing);
	
	GetDataSettings("TM_tmName2",name,value,type);
	strncpy(m_datum.tmName,value,sizeof(m_projection.tmName)-1);
	
	m_datum.a = GetDataSettingsDouble("TM_a",m_datum.a);
	m_datum.b = GetDataSettingsDouble("TM_b",m_datum.b);
	m_datum.dx = GetDataSettingsDouble("TM_dx",m_datum.dx);
	m_datum.dy = GetDataSettingsDouble("TM_dy",m_datum.dy);
	m_datum.dz = GetDataSettingsDouble("TM_dz",m_datum.dz);
	
	m_bReadTMParams = bRet;
	
	return bRet;
}


BOOL CDlgDataSource::SaveTMParams()
{
	SetDataSettingsString("TM_zoneName",m_projection.zoneName);
	SetDataSettingsString("TM_tmName",m_projection.tmName);
	
	SetDataSettingsDouble("TM_central",m_projection.central);
	SetDataSettingsDouble("TM_origin_Lat",m_projection.origin_Lat);
	SetDataSettingsDouble("TM_scale",m_projection.scale);
	SetDataSettingsDouble("TM_false_Easting",m_projection.false_Easting);
	SetDataSettingsDouble("TM_false_Northing",m_projection.false_Northing);
	
	SetDataSettingsString("TM_tmName2",m_datum.tmName);
	SetDataSettingsDouble("TM_a",m_datum.a);
	SetDataSettingsDouble("TM_b",m_datum.b);
	SetDataSettingsDouble("TM_dx",m_datum.dx);
	SetDataSettingsDouble("TM_dy",m_datum.dy);
	SetDataSettingsDouble("TM_dz",m_datum.dz);
	
	return TRUE;
}
