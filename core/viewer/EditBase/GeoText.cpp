// GeoText.cpp: implementation of the CGeoText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoText.h"
#include "resource.h"
#include "PlotText.h"
#include "SmartViewFunctions.h"
#include "PlotWChar.h"

MyNameSpaceBegin


float GetSymbolDrawScale();

IMPLEMENT_DYNCREATE(CGeoText, CPermanent)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CGeoText::CGeoText()
{
	m_nCoverType = COVERTYPE_NONE;
	m_fExtendDis = 0;
}

CGeoText::~CGeoText()
{

}



int  CGeoText::GetClassType()const
{
	return CLS_GEOTEXT;
}

int CGeoText::GetDataPointSum()const
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	m_shape.GetKeyPts(arrPts);
	return arrPts.GetSize();
}


BOOL CGeoText::SetDataPoint(int i,PT_3DEX pt)
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	m_shape.GetKeyPts(arrPts);

	int nSum = arrPts.GetSize();
	
	if( i>=0 && i<nSum )
	{
		arrPts.SetAt(i,pt);
		CreateShape(arrPts.GetData(),nSum);
		return TRUE;
	}
	return FALSE;
}


PT_3DEX CGeoText::GetDataPoint(int i)const
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	m_shape.GetKeyPts(arrPts);

	int nSum = arrPts.GetSize();
	
	if( i>=0 && i<nSum )
	{
		return arrPts.GetAt(i);
	}
	return PT_3DEX();
}

Envelope CGeoText::GetEnvelope()
{
	return m_shape.GetEnvelope();
}


BOOL CGeoText::CreateShape(const PT_3DEX *pts, int npt)
{
	m_shape.Clear();

	if( npt<1 )
		return FALSE;
	
	CLinearizer line;
	line.SetShapeBuf(&m_shape);
	line.Linearize(pts,npt,FALSE);
	
	m_shape.LineEnd();
	
	return TRUE;
}

void CGeoText::GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const
{
	m_shape.GetKeyPts(pts);
}

const CShapeLine *CGeoText::GetShape()
{
	return &m_shape;
}


CGeometry* CGeoText::Clone()const
{
	CGeoText *pNew = new CGeoText;
	pNew->CopyFrom(this);
	return pNew;
}


BOOL CGeoText::CopyFrom(const CGeometry *pObj)
{
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{		
		CGeoText *pText = (CGeoText*)pObj;
		const CShapeLine *pShape = pText->GetShape();
		m_shape.CopyFrom(pShape);

		TEXT_SETTINGS0 settings;
		pText->GetSettings(&settings);
		SetSettings(&settings);
		
		SetText(pText->GetText());

		m_nCoverType = pText->m_nCoverType;
		m_fExtendDis = pText->m_fExtendDis;
	}
	
	return CGeometry::CopyFrom(pObj);
}

BOOL CGeoText::WriteTo(CValueTable& tab)const
{
	CGeometry::WriteTo(tab);
	
	CVariantEx var;
	var = (_variant_t)(LPCTSTR)m_charList;
	tab.AddValue(FIELDNAME_GEOTEXT_CONTENT,&var,IDS_FIELDNAME_TEXT_CONTENT);

	var = (_variant_t)(LPCTSTR)m_settings.strFontName;
	tab.AddValue(FIELDNAME_GEOTEXT_FONT,&var,IDS_FIELDNAME_TEXT_FONT);

	var = (_variant_t)m_settings.fWidScale;
	tab.AddValue(FIELDNAME_GEOTEXT_CHARWIDTHS,&var,IDS_FIELDNAME_TEXT_CHARWIDTHS);

	var = (_variant_t)m_settings.fHeight;
	tab.AddValue(FIELDNAME_GEOTEXT_CHARHEIGHT,&var,IDS_FIELDNAME_TEXT_CHARHEIGHT);

	var = (_variant_t)m_settings.fCharIntervalScale;
	tab.AddValue(FIELDNAME_GEOTEXT_CHARINTVS,&var,IDS_FIELDNAME_TEXT_CHARINTVS);
	
	var = (_variant_t)m_settings.fLineSpacingScale;
	tab.AddValue(FIELDNAME_GEOTEXT_LINEINTVS,&var,IDS_FIELDNAME_TEXT_LINEINTVS);
	
	var = (_variant_t)(long)m_settings.nAlignment;
	tab.AddValue(FIELDNAME_GEOTEXT_ALIGNTYPE,&var,IDS_FIELDNAME_TEXT_ALIGNTYPE);
	
	var = (_variant_t)m_settings.fTextAngle;
	tab.AddValue(FIELDNAME_GEOTEXT_TEXTANGLE,&var,IDS_FIELDNAME_TEXTANGLE);	
	
	var = (_variant_t)m_settings.fCharAngle;
	tab.AddValue(FIELDNAME_GEOTEXT_CHARANGLE,&var,IDS_FIELDNAME_CHARANGLE);	

	var = (_variant_t)(long)m_settings.nInclineType;
	tab.AddValue(FIELDNAME_GEOTEXT_INCLINE,&var,IDS_FIELDNAME_TEXT_INCLINE);

	var = (_variant_t)m_settings.fInclineAngle;
	tab.AddValue(FIELDNAME_GEOTEXT_INCLINEANGLE,&var,IDS_FIELDNAME_TEXT_INCLINEANGLE);

	var = (_variant_t)(long)m_settings.nPlaceType;
	tab.AddValue(FIELDNAME_GEOTEXT_PLACETYPE,&var,IDS_FIELDNAME_TEXT_PLACETYPE);

	var = (_variant_t)(long)m_settings.nOtherFlag;
	tab.AddValue(FIELDNAME_GEOTEXT_OTHERFLAG,&var,IDS_FIELDNAME_TEXT_OTHERFLAG);

	var = (_variant_t)(long)m_nCoverType;
	tab.AddValue(FIELDNAME_GEOTEXT_COVERTYPE,&var,IDS_FIELDNAME_GEOCOVERTYPE);

	var = (_variant_t)m_fExtendDis;
	tab.AddValue(FIELDNAME_GEOTEXT_EXTENDDIS,&var,IDS_FIELDNAME_GEOEXTENDDIS);

	return TRUE;
}


BOOL CGeoText::ReadFrom(CValueTable& tab,int idx)
{
	CGeometry::ReadFrom(tab,idx);

	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_CONTENT,var) )
	{
		m_charList = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_FONT,var) )
	{
		memset(m_settings.strFontName,0,sizeof(m_settings.strFontName));
		_tcsncpy(m_settings.strFontName,(LPCTSTR)(_bstr_t)(_variant_t)*var,sizeof(m_settings.strFontName)-1);
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_CHARWIDTHS,var) )
	{
		m_settings.fWidScale = (double)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_CHARHEIGHT,var) )
	{
		m_settings.fHeight = (double)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_CHARINTVS,var) )
	{
		m_settings.fCharIntervalScale = (double)(_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_LINEINTVS,var) )
	{
		m_settings.fLineSpacingScale = (double)(_variant_t)*var;
	}		

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_ALIGNTYPE,var) )
	{
		m_settings.nAlignment = (long)(_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_TEXTANGLE,var) )
	{
		m_settings.fTextAngle = (_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_CHARANGLE,var) )
	{
		m_settings.fCharAngle = (_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_INCLINE,var) )
	{
		m_settings.nInclineType = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_INCLINEANGLE,var) )
	{
		m_settings.fInclineAngle = (double)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_PLACETYPE,var) )
	{
		m_settings.nPlaceType = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_OTHERFLAG,var) )
	{
		m_settings.nOtherFlag = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_COVERTYPE,var) )
	{
		m_nCoverType = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOTEXT_EXTENDDIS,var) )
	{
		m_fExtendDis = (_variant_t)*var;
	}
	
	return TRUE;
}

void CGeoText::CreateSearchReg(GrBuffer *pBuf)
{
	if (!pBuf)
	{
		return;
	}
	GrBuffer buf;
	Draw(&buf,GetSymbolDrawScale());
	
	PT_3D pt;
	Graph* pGr = buf.HeadGraph();
	while(pGr)
	{
		if (IsGrText(pGr))
		{
			pBuf->BeginPolygon(0,0);
			pt.x = ((GrText*)pGr)->x[0]+((GrText*)pGr)->pt.x;
			pt.y = ((GrText*)pGr)->y[0]+((GrText*)pGr)->pt.y;
			pt.z = ((GrText*)pGr)->pt.z;
			pBuf->MoveTo(&pt);
			pt.x = ((GrText*)pGr)->x[1]+((GrText*)pGr)->pt.x;
			pt.y = ((GrText*)pGr)->y[1]+((GrText*)pGr)->pt.y;
			pt.z = ((GrText*)pGr)->pt.z;
			pBuf->LineTo(&pt);
			pt.x = ((GrText*)pGr)->x[2]+((GrText*)pGr)->pt.x;
			pt.y = ((GrText*)pGr)->y[2]+((GrText*)pGr)->pt.y;
			pt.z = ((GrText*)pGr)->pt.z;
			pBuf->LineTo(&pt);
			pt.x = ((GrText*)pGr)->x[3]+((GrText*)pGr)->pt.x;
			pt.y = ((GrText*)pGr)->y[3]+((GrText*)pGr)->pt.y;
			pt.z = ((GrText*)pGr)->pt.z;
			pBuf->LineTo(&pt);
			pBuf->End();
		}
		pGr=pGr->next;
	}
	//母线也加进去
	pBuf->BeginLineString(0,0);
	m_shape.ToGrBuffer(pBuf);
	pBuf->End();
	
}


void CGeoText::SeparateText(GrBuffer *pBuf, float fDrawScale)
{
	CPlotTextEx drawTxt;
	CArray<PT_3DEX,PT_3DEX> arr;
	m_shape.GetPts(arr);
	int nsum = GraphAPI::GKickoffSame2DPoints(arr.GetData(),arr.GetSize());

	PT_3D  *pts = new PT_3D[nsum];
	if(!pts) return;
	for (int i=0;i<nsum;i++)
	{
		COPY_3DPT(pts[i],arr[i]);
	}	

	TEXT_SETTINGS0 settingTmp = m_settings;
	settingTmp.fHeight *= fDrawScale;
	
	drawTxt.Plot(LPCTSTR(m_charList),pts,nsum,&settingTmp,TRUE,pBuf,TRUE);
	pBuf->SetAllColor(m_nColor);
	
	delete []pts;
}


void CGeoText::ExplodeAsGrBuffer(GrBuffer *pBuf, float fDrawScale)
{
	CPlotTextEx plot;
	TEXT_SETTINGS0 settingTmp;
	settingTmp = m_settings;
	settingTmp.fHeight *= fDrawScale;
	plot.SetSettings(&settingTmp);	
	
	CShapeLine shape;

	shape.CopyFrom(&m_shape);
	
	plot.SetShape(&shape);
	plot.SetText(m_charList);
	plot.GetOutLineBuf(pBuf);
}

void CGeoText::Draw(GrBuffer *pBuf, float fDrawScale)
{
	if (!pBuf)
	{
		return;
	}
	CPlotTextEx drawTxt;
	CArray<PT_3DEX,PT_3DEX> arr;
	m_shape.GetPts(arr);

	int nsum = GraphAPI::GKickoffSame2DPoints(arr.GetData(),arr.GetSize());

	PT_3D  *pts = new PT_3D[nsum];
	if(!pts) return;
	for (int i=0;i<nsum;i++)
	{
		COPY_3DPT(pts[i],arr[i]);
	}
// 	pttt.x = xx[0]+pts[0].x;
// 	pttt.y = yy[0]+pts[0].y;
// 	pBuf->LineTo(&pttt);
// 	pBuf->End();

	TEXT_SETTINGS0 settingTmp;
	memcpy(&settingTmp,&m_settings,sizeof(TEXT_SETTINGS0));
	CUsedTextStyles *pStyles = GetUsedTextStyles();
	TextStyle style = pStyles->GetTextStyleByName(m_settings.strFontName);
	if( style.IsValid() )
	{
		_tcsncpy(settingTmp.strFontName,style.font,sizeof(settingTmp.strFontName)-1);
	}

	settingTmp.fHeight *= fDrawScale;

	drawTxt.Plot(LPCTSTR(m_charList),pts,nsum,&settingTmp,TRUE,pBuf);

	pBuf->SetAllColor(m_nColor);
	delete []pts;

//	pBuf->TestGrText();
}



void CGeoText::SetText(LPCTSTR text)
{
	m_charList.Format(_T("%s"),text);
}

CString CGeoText::GetText()const
{
	return m_charList;
	
}
void CGeoText::SetSettings(TEXT_SETTINGS0 *settings)
{
	memcpy(&m_settings,settings,sizeof(TEXT_SETTINGS0));
}

void CGeoText::GetSettings(TEXT_SETTINGS0 *settings)const
{
	memcpy(settings,&m_settings,sizeof(TEXT_SETTINGS0));
}

double CGeoText::GetTextAngle()const
{
	CArray<PT_3DEX,PT_3DEX> arr;
	m_shape.GetKeyPts(arr);

	int npt = arr.GetSize();

	if( npt<=0 )
		return 0;

	if( npt==1 )
	{
		return m_settings.fTextAngle;
	}

	if( m_settings.nPlaceType==singPt || m_settings.nPlaceType==mutiPt )
	{
		return m_settings.fTextAngle;
	}

	return GraphAPI::GGetAngle(arr[0].x,arr[0].y,arr[1].x,arr[1].y)*180/PI;
}



#define TEXTSTYLE_HEAD		"TextStyle 1.0"



BOOL TextStyle::IsValid()
{
	if (fWidScale>0 && _tcslen(font)>0 && _tcslen(name)>0)
		return  TRUE;

	return FALSE;
}


CUsedTextStyles::CUsedTextStyles()
{
	m_bLoad = FALSE;
}



CUsedTextStyles::~CUsedTextStyles()
{

}


void CUsedTextStyles::CreatePath()
{
	if( m_strPath.IsEmpty() )
	{
		TCHAR path[256]={0};
		GetModuleFileName(NULL,path,sizeof(path));
		TCHAR *pos = _tcsrchr(path, _T('\\'));
		if( pos )*pos = '\0';
		pos = _tcsrchr(path, _T('\\'));
		if( pos )*pos = '\0';

		_tcscat(path, _T("\\Config\\UsedFonts.dat"));

		m_strPath = path;
	}
}


void CUsedTextStyles::GetTextStyles(TextStyles& styles)
{
	Load();

	styles.Copy(m_arrTextStyles);
}

void CUsedTextStyles::GetTextStyleNames(CStringArray& names)
{
	Load();

	names.RemoveAll();
	for( int i=0; i<m_arrTextStyles.GetSize(); i++)
	{
		names.Add(m_arrTextStyles[i].name);
	}
}



void CUsedTextStyles::Load()
{
	if( m_bLoad )
		return;

	CreatePath();

	FILE *fp = _tfopen(m_strPath,_T("r"));
	if( !fp )return;

	char head[16] = {0};
	fread(head,sizeof(head),1,fp);
	if( head[15]!=0 || strcmp(head,TEXTSTYLE_HEAD)!=0 )
	{
		fclose(fp);
		return;
	}

	TextStyle item;

	m_arrTextStyles.RemoveAll();
	while(!feof(fp))
	{
		int nRead = fread(&item,sizeof(item),1,fp);
		if( nRead==1 )
		{
			m_arrTextStyles.Add(item);
		}
	}
	fclose(fp);

	m_bLoad = TRUE;
}



void CUsedTextStyles::Save(const TextStyles& arr)
{
	CreatePath();

	m_arrTextStyles.Copy(arr);

	FILE *fp = _tfopen(m_strPath,_T("wt"));
	if( !fp )return;

	char head[16] = {0};
	strcpy(head,TEXTSTYLE_HEAD);
	fwrite(head,sizeof(head),1,fp);

	for( int i=0; i<m_arrTextStyles.GetSize(); i++)
	{
		TextStyle item = m_arrTextStyles[i];
		fwrite(&item,sizeof(item),1,fp);
	}
	fclose(fp);
}


TextStyle CUsedTextStyles::GetTextStyleByName(LPCTSTR name)
{
	for( int i=0; i<m_arrTextStyles.GetSize(); i++)
	{
		if( _tcsicmp(name,m_arrTextStyles[i].name)==0 )
		{
			return m_arrTextStyles[i];
		}
	}
	return TextStyle();
}



CUsedTextStyles *GetUsedTextStyles()
{
	static CUsedTextStyles s_UsedStyles;
	return &s_UsedStyles;
}


MyNameSpaceEnd