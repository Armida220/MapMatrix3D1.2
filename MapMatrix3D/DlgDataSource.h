// DlgDataSource.h: interface for the CDlgDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGDATASOURCE_H__7612B7A5_D365_448E_BAA1_75B2E967C0E6__INCLUDED_)
#define AFX_DLGDATASOURCE_H__7612B7A5_D365_448E_BAA1_75B2E967C0E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataSourceEx.h"
#include "FtrLayer.h"
#include "DllDef.h"


class CDlgDataQuery : public CDataQueryEx
{
public:
	CDlgDataQuery();
	virtual ~CDlgDataQuery();

};


class CDlgDataSource : public CDataSourceEx
{
public:	
	CDlgDataSource(CDataQueryEx *pDQ);
	virtual ~CDlgDataSource();

	virtual void CreateSearchableUnit(CFeature *pObj, CPtrArray& arrPUnits);

	TMDatum GetDatum();
	void SetDatum(TMDatum& datum);
	
	BOOL ReadTMParams();
	BOOL SaveTMParams();

	TMProjectionZone GetProjection();
	void SetProjection(TMProjectionZone& prj);
	
	BOOL m_bReadTMParams;

	//椭球参数
	TMDatum m_datum;
	
	//投影参数
	TMProjectionZone m_projection;
};

#endif // !defined(AFX_DLGDATASOURCE_H__7612B7A5_D365_448E_BAA1_75B2E967C0E6__INCLUDED_)
