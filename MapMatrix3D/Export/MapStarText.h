// MapStarText.h: interface for the MapStarText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPSTARTEXT_H__64641745_42C2_4E6C_8E92_56EC272D679F__INCLUDED_)
#define AFX_MAPSTARTEXT_H__64641745_42C2_4E6C_8E92_56EC272D679F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewBaseType.h"
#include "ListFile.h"
#include "Geometry.h"
#include "GeoPoint.h"
#include "GeoDirPoint.h"
#include "GeoCurve.h"
#include "GeoText.h"
#include "GeoSurface.h"
#include "FtrLayer.h"
#include "DlgDataSource.h"
//class CGeometry;
// class CGeoPoint;
// class CGeoCurve;
// class CAnnotation;
// class CGeoText;
// class CFtrLayer;
class CMapStarWrite
{
public:
	CMapStarWrite();
	~CMapStarWrite();
	BOOL Open(LPCTSTR strInitDir, LPCTSTR strGDBfile);
	BOOL SetPointListFile(LPCTSTR pointFile);
	BOOL SetCurveListFile(LPCTSTR curveFile);

	BOOL Bound(PT_3D *pts);
	BOOL Object(CFeature *pFtr, CFtrLayer *pLayer, CDlgDataSource *pDS);
	BOOL Point(CFeature *pFtr, CAnnotation *pAnnot);
	BOOL Point_ZDif(CFeature *pFtr, CAnnotation *pAnnot);
	BOOL Curve(CGeoCurve *pObj);
	BOOL Text(CGeoText *pObj);
	void Close();

	BOOL IsContour(CGeoCurve *pObj);
private:
	CString GetMainName();
	BOOL Contour(CGeoCurve *pObj);
	BOOL Annotation(CFeature *pObj, CAnnotation *pAnnot);

	CString GetCodeString(CGeometry *pObj);
	int  GetFontIndex(LPCTSTR fontname);
	BOOL CheckContourList(LPCTSTR strFID);

public:
	double m_lfContourInterval;
	
private:
	CString m_strDir;
	CString m_strMainName;

	FILE *m_fpPoint, *m_fpCurve, *m_fpText, *m_fpContour;
	CListFile m_lstPoint, m_lstCurve;

	CString m_strFID;
};


#endif // !defined(AFX_MAPSTARTEXT_H__64641745_42C2_4E6C_8E92_56EC272D679F__INCLUDED_)
