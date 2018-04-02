// VVTAccess.cpp: implementation of the CVVTAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "VVTAccess.h"
#include "editbasedoc.h "
#include "DlgDataSource.h "
#include "GeoText.h "
#include "Feature.h "

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVVTAccess::CVVTAccess()
{
	m_fp = NULL;
	m_pDS = NULL;
}

CVVTAccess::~CVVTAccess()
{
	if (!m_fp)
	{
		fclose(m_fp);
	}
	m_pDS = NULL;
}

BOOL CVVTAccess::OpenRead(LPCTSTR fileName)
{
	m_strReadName = fileName;
//	m_nOpenMode = modeRead;
	m_fp = fopen(fileName,"rt");
	if (!m_fp)
	{
		return FALSE;
	}
	char line[1024] = {0};
	fgets(line,sizeof(line),m_fp);
	if( stricmp(line,"FCode ObjNum PtSum Color {x,y,z,pencode}\n")!=0 )
	{	
		return FALSE;
	}	
	return TRUE;
}

BOOL CVVTAccess::CloseRead()
{
	int res = fclose(m_fp);
	m_nCurFtrLayID = -1;
	m_nCurFtrGrpID = -1;
	if(res==EOF) return FALSE;
	return TRUE;
}


BOOL CVVTAccess::ReadDataSourceInfo(CDlgDataSource *pDS)
{
	if(!pDS) return FALSE;
	m_pDS = pDS;
// 	if( !m_xmlfile.IsWellFormed() )
// 		return FALSE;
// 	
// 	m_xmlfile.ResetPos();
// 	if( !m_xmlfile.FindElem(XMLTAG_FILE,TRUE) )
// 		return FALSE;	
// 	m_xmlfile.IntoElem();
// 	
// 	if( !m_xmlfile.FindElem(XMLTAG_DATASOURCE,TRUE) )
// 		return FALSE;
// 	m_xmlfile.IntoElem();
// 	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	
// 	if( !Xml_ReadValueTable(m_xmlfile,tab) )
// 	{
// 		return FALSE;
// 	}
// 	
// 	tab.EndAddValueItem();
// 	pDS->ReadFrom(tab);
	
	return TRUE;

}


CFtrLayer *CVVTAccess::GetFirstFtrLayer(long &idx)
{
// 	if( !m_xmlfile.IsWellFormed() )
// 		return NULL;
// 	
// 	m_xmlfile.ResetPos();
// 	if( !m_xmlfile.FindElem(XMLTAG_FILE,TRUE) )
// 		return NULL;
// 	
// 	m_xmlfile.IntoElem();
// 	if( !m_xmlfile.FindElem(XMLTAG_LAYER,TRUE) )
// 		return NULL;
// 	m_xmlfile.IntoElem();
// 	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	
// 	if( !Xml_ReadValueTable(m_xmlfile,tab) )
// 	{
// 		m_xmlfile.OutOfElem();
// 		return NULL;
// 	}
// 	
// 	tab.EndAddValueItem();
// 	m_xmlfile.OutOfElem();
// 	
// 	CFtrLayer *pLayer = new CFtrLayer;
// 	if( !pLayer )return NULL;
// 	
// 	pLayer->ReadFrom(tab);
// 	
	return NULL;

}

CFtrLayer *CVVTAccess::GetNextFtrLayer(long &idx)
{
// 	if( !m_xmlfile.IsWellFormed() )
// 		return NULL;
// 	
// 	if( !m_xmlfile.FindElem(XMLTAG_LAYER) )
// 		return NULL;
// 	m_xmlfile.IntoElem();
// 	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	
// 	if( !Xml_ReadValueTable(m_xmlfile,tab) )
// 	{
// 		m_xmlfile.OutOfElem();
// 		return NULL;
// 	}
// 	
// 	tab.EndAddValueItem();
// 	m_xmlfile.OutOfElem();
// 	
// 	CFtrLayer *pLayer = new CFtrLayer;
// 	if( !pLayer )return NULL;
// 	
// 	pLayer->ReadFrom(tab);
	
	return NULL;

}

// BOOL VVT_ReadValueTable()
// {
// 
// }



CFeature *CVVTAccess::GetFirstFeature(long &idx, CValueTable *exAttriTab)
{
	if(feof(m_fp)) return NULL;
	int pSum;
	int fcode;
	int objnum;
	int cbr;
	CString strLayName, strFtrID;
	if( fscanf(m_fp,"%d%d%d%d",&fcode,&objnum,&pSum,&cbr)!=4 )
			return NULL;
	long code = fcode;
	const char* name = NULL;
	CFtrLayer *pLayer = NULL;

	char line[1024] = {0};

	if( fcode==8999 )
	{
		strLayName.LoadString(IDS_DEFLAYER_NAMET);
		pLayer = m_pDS->GetDefaultTxtFtrLayer();
		if (pLayer)
		{
			//CFeature *pFtr = pLayer->CreateDefaultFeature(m_pDS->GetScale());
			CGeoText *pGeo = new CGeoText;
			CArray<PT_3DEX,PT_3DEX> pts;
			PT_3DEX expt;
			for( int j=0;j<pSum;j++ )
			{
				fscanf( m_fp,"%lf %lf %lf %d",&expt.x,&expt.y,&expt.z,&expt.pencode );
				if( expt.pencode!=penLine || expt.pencode!=penArc || expt.pencode!=penSpline || expt.pencode!=penStream )
					expt.pencode = penLine;
				
					pts.Add(expt);
			}
			pGeo->CreateShape(pts.GetData(),pts.GetSize());
			char feature = getc(m_fp);
			while(feature=='\n') feature = getc(m_fp);
			if (feature=='~') 
			{
				int tmp,tmp1,tmp2,tmp3;
				fscanf(m_fp,"%d %d %d %d\n",&tmp,&tmp1,&tmp2,&tmp3);
				
				TEXT_SETTINGS0 tset;
				tset.nPlaceType = tmp;
				tset.nAlignment = tmp1;
				tset.nInclineType = tmp3;
				
				double f1,f2;
				fscanf(m_fp,"%lf %lf\n",&f1,&f2);
				tset.fInclineAngle = f1;
				
				fscanf(m_fp,"%lf %lf %lf %lf\n",&f1,&f1,&f1,&f2);
				tset.fHeight = f2;
				tset.fWidScale = 1.0;
				
				fscanf(m_fp,"%d \n",&tmp);
				pGeo->SetColor(FindColorOfVVT(tmp,FALSE));
				
				fscanf(m_fp,"%d %d\n",&tmp,&tmp1);
				fgets(line,sizeof(line),m_fp);
				if( tmp<sizeof(line) )
					line[tmp] = '\0';
				
				pGeo->SetText(line);
				pGeo->SetSettings(&tset);
				CFeature *pFtr = new CFeature;
				pFtr->SetGeometry(pGeo);
				return pFtr;
			}
		}
	}

	return NULL;
}

CFeature *CVVTAccess::GetNextFeature(long &idx, CValueTable *exAttriTab)
{
	return NULL;
// 	if( !m_xmlfile.IsWellFormed() )
// 		return NULL;
// 	
// 	if( !m_xmlfile.FindElem(XMLTAG_FEATURE) )
// 		return NULL;
// 	m_xmlfile.IntoElem();
// 	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	
// 	if( !Xml_ReadValueTable(m_xmlfile,tab) )
// 	{
// 		m_xmlfile.OutOfElem();
// 		return NULL;
// 	}
// 	
// 	tab.EndAddValueItem();
// 	m_xmlfile.OutOfElem();
// 	
// 	const CVariantEx *p;
// 	tab.GetValue(0,XMLTAG_LAYERID,p);	
// 	m_nCurFtrLayID = (long)(_variant_t)*p;
// 	tab.GetValue(0,XMLTAG_GROUPID,p);
// 	m_nCurFtrGrpID = (long)(_variant_t)*p;
// 	
//	return (CFeature*)CreateObject(tab);

}

int CVVTAccess::GetCurFtrLayID()
{
 return 1;
}

int CVVTAccess::GetCurFtrGrpID()
{
 return 1;
}