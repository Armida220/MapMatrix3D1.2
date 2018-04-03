// MapStarText.cpp: implementation of the MapStarText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "MapStarText.h"
#include "DataSource.h"

#include "SmartViewFunctions.h"
#include "DlgCommand.h"
#include "PlotWChar.h"
#include "SymbolLib.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMapStarWrite::CMapStarWrite()
{
	m_fpPoint = NULL;
	m_fpCurve = NULL;
	m_fpText = NULL;
	m_fpContour = NULL;
	m_lfContourInterval = 5.0;
}

CMapStarWrite::~CMapStarWrite()
{
	Close();
}

BOOL CMapStarWrite::Open(LPCTSTR strInitDir, LPCTSTR strGDBfile)
{
	m_strDir = strInitDir;
	m_strMainName = strGDBfile;
	
	int pos = m_strMainName.ReverseFind(_T('\\'));
	if( pos>=0 )m_strMainName = m_strMainName.Mid(pos+1);
	pos = m_strMainName.ReverseFind(_T('.'));
	if( pos>=0 )m_strMainName = m_strMainName.Left(pos);

	Close();
	m_fpPoint = fopen(GetMainName()+_T(".txp"),"w");
	m_fpCurve = fopen(GetMainName()+_T(".txl"),"w");
	m_fpContour = fopen(GetMainName()+_T(".txc"),"w");
	m_fpText = fopen(GetMainName()+_T(".txn"),"w");

	return TRUE;
}


BOOL CMapStarWrite::SetPointListFile(LPCTSTR pointFile)
{
	return m_lstPoint.Open(pointFile);
}


BOOL CMapStarWrite::SetCurveListFile(LPCTSTR curveFile)
{
	return m_lstCurve.Open(curveFile);
}


void CMapStarWrite::Close()
{
	if( m_fpPoint )fclose(m_fpPoint);
	if( m_fpCurve )fclose(m_fpCurve);
	if( m_fpText )fclose(m_fpText);
	if( m_fpContour )fclose(m_fpContour);

	m_fpPoint = NULL;
	m_fpCurve = NULL;
	m_fpText = NULL;
	m_fpContour = NULL;
}

CString CMapStarWrite::GetMainName()
{
	return (m_strDir+_T("\\")+m_strMainName);
}


CString CMapStarWrite::GetCodeString(CGeometry *pObj)
{
	CListFile *pf = NULL;
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		pf = &m_lstPoint;
	else
		pf = &m_lstCurve;

	if( !pf )return _T("");

	CString ret = pf->FindMatchItem(m_strFID,TRUE,TRUE);
	if( ret.IsEmpty() )
	{
		ret = _T("0 0");//_T("Unknown_code");
	}
	else
	{
		int nCode = atoi(ret);
		ret.Format(_T("%d %d"), nCode/1000,nCode%1000 );
	}

	return ret;
}

BOOL CMapStarWrite::Bound(PT_3D *pts)
{
	CString filename = GetMainName() + _T(".txb");
	FILE *fp = fopen(filename, "w");
	if( !fp )return FALSE;

	//调整方向
	PT_3D pts2[4];
	if( GraphAPI::GIsClockwise(pts,4) )
		memcpy(pts2,pts,sizeof(pts2));
	else
	{
		pts2[0] = pts[0];
		pts2[1] = pts[3];
		pts2[2] = pts[2];
		pts2[3] = pts[1];
	}

	//计算最合理的左下角点
	//先按照 y 的大小排序
	int order[4] = {0}, i, j;
	for( i=0; i<4; i++)
	{
		for( j=i+1; j<4; j++)
		{
			if( pts2[i].y<pts2[j].y )
				order[j]++;
			else
				order[i]++;
		}
	}

	//取 y 较小的两个
	int i1 = -1, i2 = -1;
	for( i=0; i<4; i++)
	{
		if( order[i]==0 )i1 = i;
		else if( order[i]==1 )i2 = i;
	}

	//比较 x，取 x 较小的那个
	if( pts2[i1].x>=pts2[i2].x )
		i1 = i2;

	for( i=0; i<4; i++)
	{
		i2 = (i1+i)%4;
		fprintf(fp,"%20.6f %20.6f %20.6f\n", pts2[i2].y,pts2[i2].x,0.0 );
	}

	fclose(fp);
	return TRUE;
}



BOOL CMapStarWrite::Object(CFeature *pFtr, CFtrLayer *pLayer, CDlgDataSource *pDS)
{
//////////////////////////////////////////////////////////////////////////
	CGeometry *pObj = pFtr->GetGeometry();

	CPtrArray m_AnnoPtr;
	gpCfgLibMan->GetSymbol(pDS,pFtr,m_AnnoPtr);

	for( int i=m_AnnoPtr.GetSize()-1; i>=0;i--)
	{
		if( ((CSymbol*)m_AnnoPtr[i])->GetType()==SYMTYPE_ANNOTATION )
		{
			m_AnnoPtr.RemoveAt(i);
		}
	}

//	pLayer->GetAnnotation(pObj,m_AnnoPtr);
	m_strFID = pLayer->GetName();
	
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		if(m_AnnoPtr.GetSize() > 0)
			return Point(pFtr,(CAnnotation*)m_AnnoPtr[0]);
		else
			return Point(pFtr,NULL);
	}
	else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CAnnotation *pAnnot;
		if(m_AnnoPtr.GetSize()>0)
			pAnnot = (CAnnotation*)m_AnnoPtr[0];
		else
			pAnnot = NULL;
		if( pAnnot && pAnnot->m_nAnnoType==CAnnotation::DHeight )
		{
			return Point_ZDif(pFtr,pAnnot);
		}
		return Curve((CGeoCurve*)pObj);
	}
	
	
//////////////////////////////////////////////////////////////////////////
		
/*	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		return Point((CGeoPoint*)pObj,(CAnnotation*)pLayer->GetAnnotation(pObj));
	else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CAnnotation *pAnnot = (CAnnotation*)pLayer->GetAnnotation(pObj);
		if( pAnnot && pAnnot->m_nType==2 )
		{
			return Point_ZDif((CGeoCurve*)pObj,pAnnot);
		}
		return Curve((CGeoCurve*)pObj);
	}
*/	else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		return Text((CGeoText*)pObj);
	return FALSE;
}


BOOL CMapStarWrite::Point(CFeature *pFtr, CAnnotation *pAnnot)
{
	CGeometry *pObj = pFtr->GetGeometry();

	if( !m_fpPoint )return FALSE;

	char text[1024] = {0};
	char line[256] = {0};

	// code string
	sprintf(text,"%s ", GetCodeString(pObj));

	float ang = 0;
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		ang = ((CGeoPoint*)pObj)->m_lfAngle;
	}

	// type
	if( ang!=0.0f )
		strcat(text,"0\n");
	else
		strcat(text,"1\n");

	// point
	PT_3DEX expt;
	PT_3D dirpt;
	expt = pObj->GetDataPoint(0);

	// angle
	if( ang!=0.0f )
	{
		dirpt.x = expt.x + cos(ang)*100;
		dirpt.y = expt.y + sin(ang)*100;
		dirpt.z = 0;
		sprintf(line,"%20.6f %20.6f %20.6f %20.6f\n",expt.y,expt.x,dirpt.y,dirpt.x);
	}
	else
	{
		dirpt.x = 0;
		dirpt.y = 0;
		dirpt.z = 0;
		sprintf(line,"%20.6f %20.6f %20.6f 0\n",expt.y,expt.x,expt.z);
	}

//	sprintf(line,"%20.6f %20.6f %20.6f %20.6f\n",expt.y,expt.x,dirpt.y,dirpt.x);
	strcat(text,line);

	fprintf(m_fpPoint,"%s",text);
	return TRUE;
}


BOOL CMapStarWrite::Point_ZDif(CFeature *pFtr, CAnnotation *pAnnot)
{
	if( pFtr==NULL || pAnnot==NULL || pAnnot->m_nAnnoType!=CAnnotation::DHeight )
	{
		return FALSE;
	}

	CGeometry *pObj = pFtr->GetGeometry();

	Annotation(pFtr,pAnnot);
	if( !m_fpPoint )return FALSE;

	char text[1024] = {0};
	char line[256] = {0};

	// code string
	sprintf(text,"%s ", GetCodeString(pObj));

	// type
	strcat(text,"1\n");

	// point
	PT_3DEX expt0, expt1;
	expt0 = pObj->GetDataPoint(0);
	expt1 = pObj->GetDataPoint(1);

	{
		sprintf(line,"%20.6f %20.6f %20.6f 0\n",expt1.y,expt1.x,expt1.z-expt0.z);
	}

//	sprintf(line,"%20.6f %20.6f %20.6f %20.6f\n",expt.y,expt.x,dirpt.y,dirpt.x);
	strcat(text,line);

	fprintf(m_fpPoint,"%s",text);
	return TRUE;
}


BOOL CMapStarWrite::Curve(CGeoCurve *pObj)
{
	if( IsContour(pObj) )
		return Contour(pObj);
	
	if( !m_fpCurve )
		return FALSE;

	int nsum = pObj->GetDataPointSum();

	fprintf(m_fpCurve,"%s %d\n",GetCodeString(pObj),nsum );
	
	PT_3DEX expt;
	for( int i=0; i<nsum; i++)
	{
		expt = pObj->GetDataPoint(i);
	//	fprintf(m_fpCurve,"%20.6f %20.6f %20.6f\n", expt.x,expt.y,expt.z);
		fprintf(m_fpCurve,"%20.6f %20.6f\n", expt.y,expt.x );
	}
	return TRUE;
}


BOOL CMapStarWrite::IsContour(CGeoCurve *pObj)
{
//	CString strFID = pObj->GetFID();
	CString strFID = m_strFID;

	if( !CModifyZCommand::CheckObjForContour(pObj) )
		return FALSE;

	if( !CheckContourList(strFID) )
		return FALSE;
	return TRUE;
}


BOOL CMapStarWrite::CheckContourList(LPCTSTR strFID)
{
	static TCHAR *list[] = {
		_T("首曲线"),
		_T("计曲线"),
		_T("间曲线"),
		_T("助曲线"),
		_T("草绘等高线"),
		_T("示坡线")
	};

	int num = sizeof(list)/sizeof(list[0]);
	for( int i=0; i<num; i++)
	{
		if( stricmp(strFID,list[i])==0 )
			break;
	}
	return (i<num);
}


BOOL CMapStarWrite::Contour(CGeoCurve *pObj)
{	
	if( !m_fpContour )
		return FALSE;
	
	int nsum = pObj->GetDataPointSum();
	PT_3DEX expt;
	expt = pObj->GetDataPoint(0);
	
	fprintf(m_fpContour,"%10.2f %10.2f %d\n",expt.z,m_lfContourInterval,nsum );
	
	for( int i=0; i<nsum; i++)
	{
		expt = pObj->GetDataPoint(i);
		fprintf(m_fpContour,"%20.6f %20.6f\n", expt.y,expt.x);
	}

	return TRUE;
}


BOOL CMapStarWrite::Annotation(CFeature *pFtr, CAnnotation *pAnnot)
{
	if( pAnnot==NULL )return FALSE;
	CGeoText *pTempl = new CGeoText();
	CPtrArray textObjs;
	if( pAnnot->ExtractGeoText(pFtr,textObjs,CValueTable()) )
	{
		if( textObjs.GetSize()>0 )
		{
			Text((CGeoText*)textObjs[0]);
		}
	}

	for( int i=0; i<textObjs.GetSize(); i++)
	{
		if( textObjs[i] )
			delete (CGeoText*)textObjs[i];
	}
	return TRUE;
}


int CMapStarWrite::GetFontIndex(LPCTSTR fontname)
{
	return 4;
}

BOOL CMapStarWrite::Text(CGeoText *pObj)
{
	if( !m_fpText )return FALSE;

	TEXT_SETTINGS0 st;
	pObj->GetSettings(&st);

	char text[1024] = {0};
	char line[256] = {0};
	
	//定位方式
	strcat(text,"0\n");

	int ang = 0;
	if( st.nInclineType==SHRUGL )
		ang = -(int)(st.fInclineAngle);
	else if( st.nInclineType==SHRUGR )
		ang = (int)(st.fInclineAngle);
	HFONT hFont = ::CreateFont(
		st.fHeight,				   // nHeight
		0,                   // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		0,                 // nWeight
		FALSE,           // bItalic
		FALSE,        // bUnderline
		FALSE,        // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH,  // nPitchAndFamily
		st.strFontName);
	HDC hdc = g_Memdc.GetHdc();
	HFONT oldFont = (HFONT)::SelectObject(hdc,hFont);
	TEXTMETRIC txtMet;
	::GetTextMetrics(hdc,&txtMet);			
	::SelectObject(hdc,oldFont);
	::DeleteObject(hFont);	
	// font
	sprintf(line,"%d 0 %d 0 0 %d %d %d\n", 
		GetFontIndex(st.strFontName),
		(int)ceil(st.fHeight/12),
		ang,
		(int)st.fCharIntervalScale*txtMet.tmAveCharWidth,
		(int)st.fLineSpacingScale*(txtMet.tmExternalLeading+txtMet.tmHeight));
	strcat(text,line);

	// point
	PT_3DEX expt;
	expt = pObj->GetDataPoint(0);
	sprintf(line,"%20.6f %20.6f\n",expt.y,expt.x);
	strcat(text,line);
	
	// text
	{
		sprintf(line,"%s\n",(LPCTSTR)pObj->GetText());
		strcat(text,line);
	}
	
	fprintf(m_fpText,"%s",text);

	return TRUE;
}
