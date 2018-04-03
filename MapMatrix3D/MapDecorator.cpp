// MapDecorator.cpp: implementation of the CMapDecorator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapDecorator.h"
#include "EditbaseDoc.h"
#include "Mapper.h"
#include "Linearizer.h"
#include "GeoCurve.h"
#include "Envelope.h"
#include "SmartViewFunctions.h"
#include "resource.h"
#include "FtrLayer.h"
#include "DlgDataSource.h"
#include "SymbolLib.h"
#include "GrTrim.h"
#include "PlotWChar.h"
#include "Functions_temp.h"
#include "Markup.h"
#include "ObjectXmlIO.h"
#include "XmlAccess.h"
#include "..\CORE\viewer\EditBase\res\resource.h"


static CString GetMyConfigPath(long scale)
{
	CString path = GetConfigPath();
	scale = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager()->GetSuitedScale(scale);

	char line[12];
	path +=_T("\\");
	path += _ltoa(scale,line,10);
	path += _T("\\");
	
	return path;
}


static CString FormatFloat(double value, int nint, int nfloat)
{
	if( nfloat<0 )
		nfloat = 1;
	
	CString text;
	CString format;
	format.Format("%%.%df",nfloat);
	text.Format(format,value);
	if( nint<0 )
		return text;
	
	int pos = text.Find('.');
	if( pos>=0 )
	{
		if( pos<=nint )
			return text;
		
		text.Delete(0,pos-nint);
		return text;
	}
	
	return text;
}

extern BOOL convertStringToStrArray(LPCTSTR str,CStringArray &arr);
/*
{
	if(!str)
		return FALSE;
	CString temp(str);
	int startPos = 0,endPos = 0;
	while(endPos>=0)
	{
		endPos = temp.Find(_T(","),startPos);
		if(endPos>startPos)
			arr.Add(temp.Mid(startPos,endPos-startPos));
		else
			arr.Add(temp.Mid(startPos));
		startPos = endPos+1;
	}
	return TRUE;
}
*/
CGeometry* Compress(CGeometry *pObj, double limit)
{
	if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
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
		comp.BeginCompress(limit);
		
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
			if(pNewObj->CreateShape(pts.GetData(),pts.GetSize()))
				return pNewObj;
		}
		else
			delete pNewObj;
		
	}
	return NULL;
}

template<class T>
void RotatePts(T *pts, int npt, double cosa, double sina)
{
	double x, y;
	for( int i=0; i<npt; i++)
	{
		x = pts[i].x * cosa - pts[i].y * sina;
		y = pts[i].x * sina + pts[i].y * cosa;
		pts[i].x = x;
		pts[i].y = y;
	}
}


//外扩 wid 的距离
static void Extend(PT_3D pts[4], double wid)
{
	PT_3D pts2[5], pts3[5];
	
	memcpy(pts2,pts,sizeof(PT_3D)*4);
	pts2[4] = pts2[0];
	
	GraphAPI::GGetParallelLine(pts2,5,wid,pts3);
	memcpy(pts,pts3,sizeof(PT_3D)*4);
}

//对经纬度作格式化，flag: 1,包含度,2,包含分,4,包含秒
static CString FormatRadian(double x, int flag)
{
	x = x*180/PI + 0.5/3600;
	int n1 = (int)floor(x);
	x = (x-n1)*60;
	int n2 = (int)floor(x);
	x = (x-n2)*60;
	int n3 = (int)floor(x);
	
	CString t1, t2;
	if( flag&1 )
	{
		t2.Format("%d",n1);
		t2 += StrFromResID(IDS_MD_DEGREE);
		t1 += t2;
	}
	if( flag&2 )
	{
		t2.Format("%d'",n2);
		t1 += t2;
	}
	if( flag&4 )
	{
		t2.Format("%d\"",n3);
		t1 += t2;
	}
	
	return t1;
}


static void InsertReturns(char *text)
{
	char backup[1024];
	strcpy(backup,text);
	
	int j = 0, len = strlen(text);
	for( int i=0; i<len; i++)
	{
		if( backup[i]>0 )
		{
			text[j++] = backup[i];
			text[j++] = '\n';
		}
		else
		{
			text[j++] = backup[i];
			text[j++] = backup[i+1];
			i++;
		}
	}
	text[j] = 0;
}


namespace MapDecorate
{

CBaseObj *CreateFromID(LPCTSTR id)
{
	if( stricmp(id,"Name")==0 )
	{
		return new CName();
	}
	else if( stricmp(id,"Number")==0 )
	{
		return new CNumber();
	}
	else if( stricmp(id,"AreaDesc")==0 )
	{
		return new CAreaDesc();
	}
	else if( stricmp(id,"NearMap")==0 )
	{
		return new CNearMap();
	}
	else if( stricmp(id,"Secret")==0 )
	{
		return new CSecret();
	}
	else if( stricmp(id,"OuterBound")==0 )
	{
		return new COuterBound();
	}
	else if( stricmp(id,"InnerBound")==0 )
	{
		return new CInnerBound();
	}
	else if( stricmp(id,"Grid")==0 )
	{
		return new CGrid();
	}
	else if( stricmp(id,"LTname")==0 )
	{
		return new CLTname();
	}
	else if( stricmp(id,"LBname")==0 )
	{
		return new CLBname();
	}
	else if( stricmp(id,"RTname")==0 )
	{
		return new CRTname();
	}
	else if( stricmp(id,"RBname")==0 )
	{
		return new CRBname();
	}
	else if( stricmp(id,"Figure")==0 )
	{
		return new CFigure();
	}
	else if( stricmp(id,"Scale")==0 )
	{
		return new CScale();
	}
	else if( stricmp(id,"Ruler")==0 )
	{
		return new CRuler();
	}
	else if( stricmp(id,"NorthPointer")==0 )
	{
		return new CNorthPointer();
	}
	else if( stricmp(id,"Magline")==0 )
	{
		return new CMagline();
	}
	else if( stricmp(id,"MakerLeft")==0 )
	{
		return new CMakerLeft();
	}
	else if( stricmp(id,"MakerRight")==0 )
	{
		return new CMakerRight();
	}
	else if( stricmp(id,"DataDesc")==0 )
	{
		return new CDataDesc();
	}
	else if( stricmp(id,"Note")==0 )
	{
		return new CNote();
	}
	else if( stricmp(id,"Checkman")==0 )
	{
		return new CCheckman();
	}
	else if( stricmp(id,"DataDesc_5KN")==0 )
	{
		return new CDataDesc_5KN();
	}
	else if( stricmp(id,"AdminRegion_NearMap")==0 )
	{
		return new CAdminRegion_NearMap();
	}
	else if( stricmp(id,"MakerBottom")==0 )
	{
		return new CMakerBottom();
	}
	else if( stricmp(id,"DigitalScale")==0 )
	{
		return new CDigitalScale();
	}
	else if( stricmp(id,"NorthPointer_25KN")==0 )
	{
		return new CNorthPointer_25KN();
	}
	else if( stricmp(id,"Magline_25KN")==0 )
	{
		return new CMagline_25KN();
	}
	else if( stricmp(id,"Scale_5KN")==0 )
	{
		return new CScale_5KN();
	}
	else if( stricmp(id,"Scale_25KN")==0 )
	{
		return new CScale_25KN();
	}
	else if( stricmp(id,"Ruler_25KN")==0 )
	{
		return new CRuler_25KN();
	}
	else if( stricmp(id,"OuterBoundDecorator")==0 )
	{
		return new COuterBoundDecorator();
	}
	else if( stricmp(id,"AdminRegion")==0 )
	{
		return new CAdminRegion();
	}
	else if( stricmp(id,"Grid_New")==0 )
	{
		return new CGrid_New();
	}
	return NULL;
}

CName::CName()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_MAPNAME2);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_name");
	strcpy(name,StrFromResID(IDS_MD_MAPNAME));
	size = 6;
	wscale = 1;
	char_interv = 0;
	dy = 13;

	if( 5000 == scale || 10000 == scale )
	{
		strcpy(font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
		size = 7.5;
		dy = 14.5;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if(MapDecorate::bMapDecorateNew)
		{
			strcpy(font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
			size = 7.5;
			dy = 14.5;
		}
		else
		{
			strcpy(font,StrFromResID(IDS_MD_CUDENGXIANTI));
			size = 6;
			dy = 17;
		}
	}
	
}


CName::~CName()
{
}

void CName::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("char_interv",char_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARINTVS));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BTM_SPACE));

}

void CName::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv",pvar) )
	{
		char_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CName::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = char_interv;
	tab.AddValue("char_interv",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CName::GetUIParams_data(CUIParam *pParam)
{
}

void CName::SetData(CValueTable& tab)
{
}

void CName::GetData(CValueTable& tab)
{
}

void CName::MakeFtrs()
{	
	if( !m_pMain || !m_pAssist )
		return;

	//为了方便计算，旋转坐标系，使得 X 轴平行于外图廓的顶边
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	double xs, xe, ys, ye;
	xs = e.m_xl; xe = e.m_xh;
	ys = e.m_yl; ye = e.m_yh;
	
	CFtrLayer *pLayer = NULL;
	
	//创建图号文字
	
	PT_3D textpt;
	textpt.x = (xs+xe)*0.5; textpt.y = ye+dy*m_pAssist->Unit();
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText,&textpt,1,name,font,size,wscale,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CNumber::CNumber()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_MAPNUM2);	
	strcpy(layer,"TK_number");
	strcpy(number,StrFromResID(IDS_MD_MAPNUM));	
	char_interv = 0;
	dy = 5;
	int_num = 2;
	float_num = 1;

	if( 5000 == scale || 10000 == scale )
	{
		strcpy(font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
		size = 4.5;
		wscale = 1;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if(MapDecorate::bMapDecorateNew)
		{
			strcpy(font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
			size = 4.5;
			wscale = 1;
		}
		else
		{
			strcpy(font,StrFromResID(IDS_MD_CHANGDENGXIANTI));
			size = 5;
			wscale = 0.5;
			dy = 9.0;
		}
	}
	else
	{
		strcpy(font,StrFromResID(IDS_MD_HEITI));
		size = 5;
		wscale = 0.5;
		dy =9.0;
	}

}


CNumber::~CNumber()
{
}

void CNumber::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("char_interv",char_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARINTVS));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BTM_SPACE));

	pParam->AddParam("int_num",int_num,StrFromResID(IDS_MAPNUM_INTNUM));
	pParam->AddParam("float_num",float_num,StrFromResID(IDS_MAPNUM_FLOATNUM));
	
}

void CNumber::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv",pvar) )
	{
		char_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"int_num",pvar) )
	{
		int_num = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"float_num",pvar) )
	{
		float_num = (float)(_variant_t)*pvar;
	}
}

void CNumber::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = char_interv;
	tab.AddValue("char_interv",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
	
	var = int_num;
	tab.AddValue("int_num",&CVariantEx(var));
	
	var = float_num;
	tab.AddValue("float_num",&CVariantEx(var));
}

void CNumber::GetUIParams_data(CUIParam *params)
{
}

void CNumber::SetData(CValueTable& tab)
{
}

void CNumber::GetData(CValueTable& tab)
{
}

void CNumber::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	double xs, xe, ys, ye;
	xs = e.m_xl; xe = e.m_xh;
	ys = e.m_yl; ye = e.m_yh;
	
	CFtrLayer *pLayer = NULL;
	
	//创建图号文字
	
	PT_3D textpt;
	textpt.x = (xs+xe)*0.5; textpt.y = ye+dy*m_pAssist->Unit();
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText,&textpt,1,number,font,size,wscale,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CAreaDesc::CAreaDesc()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();
	m_bUsed  = FALSE;

	m_strName = StrFromResID(IDS_MD_AREADESC);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_area_desc");
	strcpy(area_desc,StrFromResID(IDS_MD_AREADESC));
	size = 3;
	wscale = 1;
	dy = 3;

	if( (25000 == scale || 50000 == scale || 100000 == scale) &&  !MapDecorate::bMapDecorateNew)
		strcpy(font,StrFromResID(IDS_MD_BIANSONGTI));
}


CAreaDesc::~CAreaDesc()
{
}

void CAreaDesc::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BTM_SPACE));
	
}

void CAreaDesc::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CAreaDesc::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));	
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CAreaDesc::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddParam("area_desc",area_desc,StrFromResID(IDS_MD_AREADESC));
}

void CAreaDesc::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"area_desc",pvar) )
	{
		strncpy(area_desc,(LPCTSTR)(_bstr_t)*pvar,sizeof(area_desc)-1);
	}
}

void CAreaDesc::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)area_desc;
	tab.AddValue("area_desc",&CVariantEx(var));
}

void CAreaDesc::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);
	
	//创建接图表表格线
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	double xs, xe, ys, ye;
	xs = e.m_xl; xe = e.m_xh;
	ys = e.m_yl; ye = e.m_yh;
	
	CFtrLayer *pLayer = NULL;
	
	//创建接图表中的图号文字
	
	PT_3D textpt;
	textpt.x = (xs+xe)*0.5; textpt.y = ye+dy*m_pAssist->Unit();
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText,&textpt,1,area_desc,font,size,wscale,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CNearMap::CNearMap()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_NEARMAP);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_nearmap");
	size = 2.5;
	wscale = 1;
	width = 45;
	height = 24;
	dy = 3;
	memset(&nearmap,0,sizeof(nearmap));

	if( (25000 == scale || 50000 == scale || 100000 == scale) && !MapDecorate::bMapDecorateNew )
	{
		strcpy(font,StrFromResID(IDS_MD_XIDENGXIANTI));
		width = 39;
		height = 21;
	}
}


CNearMap::~CNearMap()
{
}

void CNearMap::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("width",width,StrFromResID(IDS_MD_ALLWID));
	pParam->AddParam("height",height,StrFromResID(IDS_MD_ALLHEI));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BTM_SPACE));
	
}

void CNearMap::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"width",pvar) )
	{
		width = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"height",pvar) )
	{
		height = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CNearMap::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = width;
	tab.AddValue("width",&CVariantEx(var));

	var = height;
	tab.AddValue("height",&CVariantEx(var));

	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));	
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CNearMap::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddParam("name1",nearmap.name1,StrFromResID(IDS_MD_NEARMAP_11));
	pParam->AddParam("name2",nearmap.name2,StrFromResID(IDS_MD_NEARMAP_12));
	pParam->AddParam("name3",nearmap.name3,StrFromResID(IDS_MD_NEARMAP_13));
	pParam->AddParam("name4",nearmap.name4,StrFromResID(IDS_MD_NEARMAP_21));
	pParam->AddParam("name5",nearmap.name5,StrFromResID(IDS_MD_NEARMAP_23));
	pParam->AddParam("name6",nearmap.name6,StrFromResID(IDS_MD_NEARMAP_31));
	pParam->AddParam("name7",nearmap.name7,StrFromResID(IDS_MD_NEARMAP_32));
	pParam->AddParam("name8",nearmap.name8,StrFromResID(IDS_MD_NEARMAP_33));
}

void CNearMap::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"name1",pvar) )
	{
		strncpy(nearmap.name1,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name1)-1);
	}
	if( tab.GetValue(0,"name2",pvar) )
	{
		strncpy(nearmap.name2,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name2)-1);
	}
	if( tab.GetValue(0,"name3",pvar) )
	{
		strncpy(nearmap.name3,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name3)-1);
	}
	if( tab.GetValue(0,"name4",pvar) )
	{
		strncpy(nearmap.name4,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name4)-1);
	}
	if( tab.GetValue(0,"name5",pvar) )
	{
		strncpy(nearmap.name5,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name5)-1);
	}
	if( tab.GetValue(0,"name6",pvar) )
	{
		strncpy(nearmap.name6,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name6)-1);
	}
	if( tab.GetValue(0,"name7",pvar) )
	{
		strncpy(nearmap.name7,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name7)-1);
	}
	if( tab.GetValue(0,"name8",pvar) )
	{
		strncpy(nearmap.name8,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name8)-1);
	}
}

void CNearMap::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)nearmap.name1;
	tab.AddValue("name1",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name2;
	tab.AddValue("name2",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name3;
	tab.AddValue("name3",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name4;
	tab.AddValue("name4",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name5;
	tab.AddValue("name5",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name6;
	tab.AddValue("name6",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name7;
	tab.AddValue("name7",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name8;
	tab.AddValue("name8",&CVariantEx(var));	
}

void CNearMap::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	//为了方便计算，旋转坐标系，使得 X 轴平行于外图廓的顶边
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);

	//创建接图表表格线
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);

	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	double xs, xe, ys, ye;
	xs = e2.m_xl; 
	xe = xs + width*m_pAssist->Unit();
	ys = e.m_yh + dy*m_pAssist->Unit(); 
	ye = ys + height*m_pAssist->Unit();

	PT_3DEX expt;
	expt.pencode = penLine;
	expt.z = 0;

	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( pGeo )
	{
		//外边框
		PT_3DEX pts[5];
		expt.x = xs; expt.y = ys;
		pts[0] = expt;
		expt.x = xe; expt.y = ys;
		pts[1] = expt;
		expt.x = xe; expt.y = ye;
		pts[2] = expt;
		expt.x = xs; expt.y = ye;
		pts[3] = expt;
		expt.x = xs; expt.y = ys;
		pts[4] = expt;

		RotatePts(pts,5,xb.cosa,xb.sina);

		pGeo->CreateShape(pts,5);

		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	PT_3D linePt[2];

	linePt[0] = PT_3D(xs,ys + height*m_pAssist->Unit()/3,0);
	linePt[1] = PT_3D(xe,ys + height*m_pAssist->Unit()/3,0);

	RotatePts(linePt,2,xb.cosa,xb.sina);

	//内部表格的横线
	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

	linePt[0] = PT_3D(xs,ys + height*m_pAssist->Unit()*2/3,0);
	linePt[1] = PT_3D(xe,ys + height*m_pAssist->Unit()*2/3,0);
	
	RotatePts(linePt,2,xb.cosa,xb.sina);

	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);
	
	//内部表格的竖线
	linePt[0] = PT_3D(xs + width*m_pAssist->Unit()/3,ys,0);
	linePt[1] = PT_3D(xs + width*m_pAssist->Unit()/3,ye,0);
	
	RotatePts(linePt,2,xb.cosa,xb.sina);

	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

	linePt[0] = PT_3D(xs + width*m_pAssist->Unit()*2/3,ys,0);
	linePt[1] = PT_3D(xs + width*m_pAssist->Unit()*2/3,ye,0);
	
	RotatePts(linePt,2,xb.cosa,xb.sina);

	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);
	
	//创建接图表中的图号文字
	CGeoText *pText;
	
	PT_3D textpts[9];
	memset(textpts,0,sizeof(textpts));
	double xoff0 = (xe-xs)/6, yoff0 = (ye-ys)/6;
	double dx = (xe-xs)/3, dy = (ye-ys)/3;

	textpts[0].x = xs + xoff0; textpts[0].y = ys + yoff0 + dy*2;
	textpts[1].x = textpts[0].x + dx; textpts[1].y = textpts[0].y;
	textpts[2].x = textpts[1].x + dx; textpts[2].y = textpts[0].y;
	
	textpts[3].x = xs + xoff0; textpts[3].y = ys + yoff0 + dy;
	textpts[4].x = textpts[0].x + dx; textpts[4].y = textpts[3].y;
	textpts[5].x = textpts[1].x + dx; textpts[5].y = textpts[3].y;

	textpts[6].x = xs + xoff0; textpts[6].y = ys + yoff0;
	textpts[7].x = textpts[0].x + dx; textpts[7].y = textpts[6].y;
	textpts[8].x = textpts[1].x + dx; textpts[8].y = textpts[6].y;

	RotatePts(textpts,9,xb.cosa,xb.sina);

	char *textlist[9] = {
		nearmap.name1,nearmap.name2,nearmap.name3,
		nearmap.name4,nearmap.name4,nearmap.name5,
		nearmap.name6,nearmap.name7,nearmap.name8
	};

	for( int i=0; i<9; i++)
	{
		if( i==4 )continue;
		if( strlen(textlist[i])<=0 )
			continue;

		pFtr = m_pAssist->CreateText(layer,pLayer);
		if( !pFtr )continue;

		pText = (CGeoText*)pFtr->GetGeometry();		

		m_pAssist->SetText(pText,textpts+i,1,textlist[i],font,size,wscale,TAH_MID|TAV_MID);
		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	//当前图幅的晕线填充
	textpts[0].x = xs + dx; textpts[0].y = ys + dy;
	textpts[1].x = textpts[0].x + dx; textpts[1].y = textpts[0].y;
	textpts[2].x = textpts[1].x; textpts[2].y = textpts[1].y+dy;	
	textpts[3].x = textpts[1].x - dx; textpts[3].y = textpts[2].y;

	RotatePts(textpts,4,xb.cosa,xb.sina);

	CFtrArray arrFtrs;
	m_pAssist->CreateHachure(layer,textpts,4,PI/4+xb.ang,1*m_pAssist->Unit(),pLayer,arrFtrs);
	for( i=0; i<arrFtrs.GetSize(); i++)
	{
		m_pAssist->AddObject(arrFtrs[i],pLayer->GetID());
	}
}


CSecret::CSecret()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_SECRET);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_secret");
	strcpy(secret,StrFromResID(IDS_MD_SECRET_LEVEL1));
	nSecret = 1;
	//size = 6;
	wscale = 1.5;
	//dy = 5.5;

	if( 5000 == scale || 10000 == scale )
	{
		size = 4.0;
		dy = 5.0;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if(MapDecorate::bMapDecorateNew)
		{
			size = 4.0;
			wscale = 1.0;
			dy = 5;
		}
		else
		{
			strcpy(font,StrFromResID(IDS_MD_BIANCUDENGXIANTI));
			size = 4.5;
			dy = 3;
		}
	}
	else
	{
		size = 6;
		dy = 5.5;
	}

}


CSecret::~CSecret()
{
}

void CSecret::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BSPACE));
	
}

void CSecret::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CSecret::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));	
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CSecret::GetUIParams_data(CUIParam *pParam)
{
	pParam->BeginOptionParam("secret",StrFromResID(IDS_MD_SECRET_LEVEL));
	pParam->AddOption(StrFromResID(IDS_MD_SECRET_LEVEL1),1,0,nSecret==1);
	pParam->AddOption(StrFromResID(IDS_MD_SECRET_LEVEL2),2,0,nSecret==2);
	pParam->AddOption(StrFromResID(IDS_MD_SECRET_LEVEL3),3,0,nSecret==3);
	pParam->EndOptionParam();
}

void CSecret::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"secret",pvar) )
	{
		nSecret = (long)(_variant_t)*pvar;
		switch(nSecret)
		{
		case 1:
			strncpy(secret,StrFromResID(IDS_MD_SECRET_LEVEL1),sizeof(secret)-1);
			break;
		case 2:
			strncpy(secret,StrFromResID(IDS_MD_SECRET_LEVEL2),sizeof(secret)-1);
			break;
		case 3:
			strncpy(secret,StrFromResID(IDS_MD_SECRET_LEVEL3),sizeof(secret)-1);
			break;
		}
	}
}

void CSecret::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (long)nSecret;
	tab.AddValue("secret",&CVariantEx(var));
}

void CSecret::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	double xs, xe, ys, ye;
	xs = e.m_xl; xe = e.m_xh;
	ys = e.m_yl; ye = e.m_yh;
	
	CFtrLayer *pLayer = NULL;
	
	//创建接图表中的图号文字
	
	PT_3D textpt;
	textpt.x = e2.m_xh; 
	textpt.y = ye+dy*m_pAssist->Unit();
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText,&textpt,1,secret,font,size,wscale,TAH_RIGHT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



COuterBound::COuterBound()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_OUTTER_MAPBORDER);
	strcpy(layer,"TK_outer_bound");
	width = 0.5;
	extension = 12;

	if(5000 == scale)
	{
		extension = 9.5;
	}
	else if(10000 == scale)
	{
		extension = 8;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		extension = 7.75;
	}

}


COuterBound::~COuterBound()
{
}

void COuterBound::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("width",width,StrFromResID(IDS_MD_OBOUND_LINEWID));
	pParam->AddParam("extension",extension,StrFromResID(IDS_MD_OBOUND_OFFSET));
	
}

void COuterBound::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"width",pvar) )
	{
		width = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"extension",pvar) )
	{
		extension = (float)(_variant_t)*pvar;
	}
}

void COuterBound::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = width;
	tab.AddValue("width",&CVariantEx(var));
	
	var = extension;
	tab.AddValue("extension",&CVariantEx(var));		
}


void COuterBound::MakeFtrs()
{	
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( pGeo )
	{
		PT_3DEX expt, pts[5];
		expt.z = 0;
		expt.pencode = penLine;
		for( int i=0; i<5; i++)
		{
			COPY_3DPT(expt,m_pMain->m_outerBound[i%4]);
			pts[i] = expt;
		}
		
		pGeo->CreateShape(pts,5);
		
		pGeo->m_fLineWidth = width;
		
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	PT_3D pt1,pt2,pt3,pt4,pt5,pt6, textpt;
	PT_3DEX expt, pts[2];

	pt5.z = pt6.z = 0;

//	CGrid *pGrid = (CGrid*)m_pMain->GetObj("Grid");
	CGrid *pGrid = NULL;
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();
	if( 25000 == scale || 50000 == scale || 100000 == scale )
	{
		pGrid = (CGrid*)m_pMain->GetObj("Grid_New");
	}
	else
	{
		pGrid = (CGrid*)m_pMain->GetObj("Grid");
	}
	if( !pGrid )return;

	CNumber *pNumber = (CNumber*)m_pMain->GetObj("Number");
	if( !pNumber )return;

	//底边
	{
		expt.pencode = penLine;
		
		//计算内图廓底边的两头短线
		
		pt1 = m_pMain->m_innerBound[0];
		pt2 = m_pMain->m_innerBound[1];
		
		pt3 = m_pMain->m_outerBound[0];
		pt4 = m_pMain->m_outerBound[3];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
		
		pt3 = m_pMain->m_outerBound[1];
		pt4 = m_pMain->m_outerBound[2];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);
		
		COPY_3DPT(expt,pt5);
		pts[0] = expt;
		
		COPY_3DPT(expt,pt6);
		pts[1] = expt;
		
		m_pAssist->AddLine(layer,pt1,pt5,0);
		m_pAssist->AddLine(layer,pt2,pt6,0);
		
		double ang = GraphAPI::GGetAngle(pt2.x,pt2.y,pt6.x,pt6.y)*180/PI;
		
		//-----计算两头的图廓坐标注记
		//底边的公里数
		if( m_pMain->m_scale<=2000 )
		{
			CString text;
			text = FormatFloat(m_pMain->m_innerBound[0].y*0.001,-1,pNumber->float_num);
			
			//底边的左边点
			PT_3D dpt(pGrid->kmLit_dx * m_pAssist->Unit(), pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			textpt = pt5;
			textpt.x += dpt.x;
			textpt.y += dpt.y;				
			
			m_pAssist->AddText(layer,textpt,text,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_LEFT|TAV_BOTTOM,ang);				
			
			//底边的右边点	
			dpt = PT_3D(-pGrid->kmLit_dx * m_pAssist->Unit(), pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			
			textpt = pt6;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,text,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_RIGHT|TAV_BOTTOM,ang);
			
		}
		//底边的经纬度
		else
		{
			//纬度
			pt1 = m_pMain->GaussToGeo(m_pMain->m_innerBound[0]);
			pt2 = m_pMain->GaussToGeo(m_pMain->m_innerBound[1]);

			//度
			CString text1 = FormatRadian(pt1.y,1);
			//分秒
			CString text2 = FormatRadian(pt1.y,6);
			
			double ang = GraphAPI::GGetAngle(m_pMain->m_innerBound[0].x,m_pMain->m_innerBound[0].y,m_pMain->m_innerBound[1].x,m_pMain->m_innerBound[1].y)*180/PI;
			
			//底边的左边的纬度的度数
			textpt = m_pMain->m_innerBound[0];
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_BOTTOM,ang);
	
			//底边的左边的纬度的分秒
			textpt = m_pMain->m_innerBound[0];
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_TOP,ang);
			
			//底边的右边的纬度的度数
			textpt = m_pMain->m_innerBound[1];
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_BOTTOM,ang);
			
			//底边的右边的纬度的分秒
			textpt = m_pMain->m_innerBound[1];
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_TOP,ang);
		}
	}

	//右侧
	{
		//内图廓的右侧边的两头短线
		pt1 = m_pMain->m_innerBound[1];
		pt2 = m_pMain->m_innerBound[2];
		
		pt3 = m_pMain->m_outerBound[0];
		pt4 = m_pMain->m_outerBound[1];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
		
		pt3 = m_pMain->m_outerBound[2];
		pt4 = m_pMain->m_outerBound[3];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);
		
		COPY_3DPT(expt,pt5);
		pts[0] = expt;
		
		COPY_3DPT(expt,pt6);
		pts[1] = expt;
		
		m_pAssist->AddLine(layer,pt1,pt5,0);
		m_pAssist->AddLine(layer,pt2,pt6,0);
		
		double ang = GraphAPI::GGetAngle(pt2.x,pt2.y,pt6.x,pt6.y)*180/PI-90;
		
		//-----计算两端的图廓坐标注记
		//内图廓的右侧边的公里数注记
		if( m_pMain->m_scale<=2000 )
		{
			CString text;
			text = FormatFloat(m_pMain->m_innerBound[2].x*0.001,-1,pNumber->float_num);
			
			CString left, right;
			int pos = text.Find('.');
			if( pos>0 )
			{
				left = text.Left(pos+1);
				right = text.Mid(pos+1);
			}
			else
			{
				left = text;
			}
			
			PT_3D textpt;
			
			//右侧边的下边点的整数部分
			PT_3D dpt(0, pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			textpt = pt5;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			
			m_pAssist->AddText(layer,textpt,left,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_RIGHT|TAV_BOTTOM,ang);
			
			//右侧边的下边点的小数部分
			textpt = pt5;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,right,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_LEFT|TAV_BOTTOM,ang);
			
			//右侧边的上边点的整数部分
			dpt = PT_3D(0, -pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			
			textpt = pt6;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,left,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_RIGHT|TAV_TOP,ang);
			
			//右侧边的上边点的小数部分
			textpt = pt6;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,right,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_LEFT|TAV_TOP,ang);
			
		}
		//内图廓的右侧边的经纬度
		else
		{
			//经度
			pt1 = m_pMain->GaussToGeo(m_pMain->m_innerBound[1]);
			pt2 = m_pMain->GaussToGeo(m_pMain->m_innerBound[2]);
			
			//度
			CString text1 = FormatRadian(pt1.x,1);
			//分秒
			CString text2 = FormatRadian(pt1.x,6);
			
			//右侧边的上边点的度
			textpt = m_pMain->m_innerBound[2];
			textpt.y += pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_BOTTOM,ang);
			
			//右侧边的上边点的分秒
			textpt = m_pMain->m_innerBound[2];
			textpt.y += pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_BOTTOM,ang);
			
			//右侧边的下边点的度
			textpt = m_pMain->m_innerBound[1];
			textpt.y -= pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_TOP,ang);
			
			//右侧边的下边点的分秒
			textpt = m_pMain->m_innerBound[1];
			textpt.y -= pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_TOP,ang);
			
		}
	}
	
	//顶边
	{
		//内图廓的顶边的两头短线
		pt1 = m_pMain->m_innerBound[2];
		pt2 = m_pMain->m_innerBound[3];
		
		pt3 = m_pMain->m_outerBound[0];
		pt4 = m_pMain->m_outerBound[3];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
		
		pt3 = m_pMain->m_outerBound[1];
		pt4 = m_pMain->m_outerBound[2];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);
		
		COPY_3DPT(expt,pt5);
		pts[0] = expt;
		
		COPY_3DPT(expt,pt6);
		pts[1] = expt;
		
		m_pAssist->AddLine(layer,pt2,pt5,0);
		m_pAssist->AddLine(layer,pt1,pt6,0);
		
		double ang = GraphAPI::GGetAngle(pt1.x,pt1.y,pt6.x,pt6.y)*180/PI;
		
		//-----计算两头的图廓坐标注记
		//内图廓的顶边的公里数
		if( m_pMain->m_scale<=2000 )
		{
			CString text;
			text = FormatFloat(m_pMain->m_innerBound[2].y*0.001,-1,pNumber->float_num);
			
			//顶边的左边点
			PT_3D dpt(pGrid->kmLit_dx * m_pAssist->Unit(), pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			textpt = pt5;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,text,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_LEFT|TAV_BOTTOM,ang);
			
			//顶边的右边点
			dpt = PT_3D(-pGrid->kmLit_dx * m_pAssist->Unit(), pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			textpt = pt6;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,text,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_RIGHT|TAV_BOTTOM,ang);
			
		}
		//顶边的经纬度
		else
		{
			//纬度
			pt1 = m_pMain->GaussToGeo(m_pMain->m_innerBound[3]);
			pt2 = m_pMain->GaussToGeo(m_pMain->m_innerBound[2]);
			
			//度
			CString text1 = FormatRadian(pt1.y,1);
			//分秒
			CString text2 = FormatRadian(pt1.y,6);
			
			//顶边的左边点
			textpt = m_pMain->m_innerBound[3];
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_BOTTOM,ang);
			
			textpt = m_pMain->m_innerBound[3];
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_TOP,ang);
			
			//顶边的右边点
			textpt = m_pMain->m_innerBound[2];
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_BOTTOM,ang);
			
			textpt = m_pMain->m_innerBound[2];
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_TOP,ang);
			
		}
	}
	
	//左侧
	{
		//内图廓的左侧边的两头短线
		pt1 = m_pMain->m_innerBound[0];
		pt2 = m_pMain->m_innerBound[3];
		
		pt3 = m_pMain->m_outerBound[2];
		pt4 = m_pMain->m_outerBound[3];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
		
		pt3 = m_pMain->m_outerBound[0];
		pt4 = m_pMain->m_outerBound[1];
		GraphAPI::GGetLineIntersectLine(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y,
			pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);
		
		COPY_3DPT(expt,pt5);
		pts[0] = expt;
		
		COPY_3DPT(expt,pt6);
		pts[1] = expt;
		
		m_pAssist->AddLine(layer,pt2,pt5,0);
		m_pAssist->AddLine(layer,pt1,pt6,0);
		
		double ang = GraphAPI::GGetAngle(pt2.x,pt2.y,pt5.x,pt5.y)*180/PI-90;
		
		//-----计算两端的图廓坐标注记
		//左侧边的公里数注记
		if( m_pMain->m_scale<=2000 )
		{
			CString text;
			text = FormatFloat(m_pMain->m_innerBound[0].x*0.001,-1,pNumber->float_num);
			
			CString left, right;
			int pos = text.Find('.');
			if( pos>0 )
			{
				left = text.Left(pos+1);
				right = text.Mid(pos+1);
			}
			else
			{
				left = text;
			}
			
			PT_3D textpt;
			
			//左侧边的上边点
			PT_3D dpt(0, -pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			textpt = pt5;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,left,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_RIGHT|TAV_TOP,ang);
			
			textpt = pt5;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,right,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_LEFT|TAV_TOP,ang);
			
			//左侧边的下边点
			dpt = PT_3D(0, pGrid->kmLit_dy * m_pAssist->Unit(), 0);
			RotatePts(&dpt,1,cos(ang*PI/180),sin(ang*PI/180));
			textpt = pt6;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,left,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_RIGHT|TAV_BOTTOM,ang);
			
			textpt = pt6;
			textpt.x += dpt.x;
			textpt.y += dpt.y;
			m_pAssist->AddText(layer,textpt,right,
				pGrid->kmLit_font,pGrid->kmLit_size,0,TAH_LEFT|TAV_BOTTOM,ang);
			
		}
		//左侧边的经纬度
		else
		{
			//经度
			pt1 = m_pMain->GaussToGeo(m_pMain->m_innerBound[0]);
			pt2 = m_pMain->GaussToGeo(m_pMain->m_innerBound[3]);
			
			//度
			CString text1 = FormatRadian(pt1.x,1);
			//分秒
			CString text2 = FormatRadian(pt1.x,6);
			
			//左侧边的上边点				
			textpt = m_pMain->m_innerBound[0];
			textpt.y -= pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_TOP,ang);
			
			textpt = m_pMain->m_innerBound[0];
			textpt.y -= pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_TOP,ang);
			
			//左侧边的下边点
			textpt = m_pMain->m_innerBound[3];
			textpt.y += pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text1,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_RIGHT|TAV_BOTTOM,ang);
			
			textpt = m_pMain->m_innerBound[3];
			textpt.y += pGrid->kmBig_size*m_pAssist->Unit();
			m_pAssist->AddText(layer,textpt,text2,
				pGrid->geo_font,pGrid->geo_size,pGrid->geo_wscale,TAH_LEFT|TAV_BOTTOM,ang);
		}
	}
}




CInnerBound::CInnerBound()
{
	m_strName = StrFromResID(IDS_MD_INNER_MAPBORDER);
	strcpy(layer,"TK_inner_bound");
	width = 0;
}


CInnerBound::~CInnerBound()
{
}

void CInnerBound::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("width",width,StrFromResID(IDS_MD_IBOUND_LINEWID));
	
}

void CInnerBound::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"width",pvar) )
	{
		width = (float)(_variant_t)*pvar;
	}
}

void CInnerBound::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = width;
	tab.AddValue("width",&CVariantEx(var));	
}


void CInnerBound::MakeFtrs()
{
	//内图廓
	PT_3D pt1,pt2,pt3,pt4,pt5,pt6, textpt;
	PT_3DEX expt, pts[2];

	//生成图廓线
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);		
	if( pFtr )
	{
		PT_3DEX bounds[5];
		for( int i=0; i<4; i++)
		{
			bounds[i] = PT_3DEX(m_pMain->m_innerBound[i],penLine);
		}
		bounds[4] = bounds[0];
		
		pFtr->GetGeometry()->CreateShape(bounds,5);

		CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
		pGeo->m_fLineWidth = width;

		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}
}




CGrid::CGrid()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_GRID);

	strcpy(layer,"TK_grid");
	type = 0;
	cross_len = 10;
	bFrameLine = 1;
	grid_wid = 100;
	strcpy(kmBig_font,StrFromResID(IDS_MD_HEITI));
	kmBig_size = 2;
	kmBig_wscale = 1;
	kmBig_dx = 1;
	kmBig_dy = 0.5;
		
	strcpy(kmLit_font,StrFromResID(IDS_MD_HEITI));
	kmLit_size = 2.5;
	kmLit_wscale = 1;
	kmLit_dx = 1;
	kmLit_dy = 0.5;
	
	bGeoNumber = 0;
	strcpy(geo_font,StrFromResID(IDS_MD_HEITI));
	geo_size = 2;
	geo_wscale = 1;
	geo_dx = 1;
	geo_dy = 0.5;

	if( 5000 ==  scale || 10000 == scale )
	{
		strcpy(kmBig_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
		strcpy(kmLit_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
		strcpy(geo_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
		kmLit_size = 3;
		type = 1;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if(MapDecorate::bMapDecorateNew)
		{
			strcpy(kmBig_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
			strcpy(kmLit_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
			strcpy(geo_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
			kmLit_size = 3;
		}
		else
		{
			strcpy(kmBig_font,StrFromResID(IDS_MD_ZHENGDENGXIANTI));
			strcpy(kmLit_font,StrFromResID(IDS_MD_ZHENGDENGXIANTI));
			strcpy(geo_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
			kmLit_size = 4;
			geo_size = 2.5;
			kmBig_size = 2.5;
		}
		type = 1;
	}

}


CGrid::~CGrid()
{
}

void CGrid::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->BeginOptionParam("type",StrFromResID(IDS_MD_GRID_TYPE));
	pParam->AddOption(StrFromResID(IDS_MD_GRID_TYPECROSS),0,0,TRUE);
	pParam->AddOption(StrFromResID(IDS_MD_GRID_TYPELINE),1,0,FALSE);
	pParam->EndOptionParam();
	pParam->SetOptionDefault("type",type);
	pParam->AddParam("cross_len",cross_len,StrFromResID(IDS_MD_CROSSLEN));
	pParam->AddParam("bFrameLine",(bool)bFrameLine,StrFromResID(IDS_MD_GRID_COORDMARKLINE));
	pParam->AddParam("grid_wid",grid_wid,StrFromResID(IDS_MD_GRID_WID));
	
	pParam->AddFontNameParam("kmBig_font",kmBig_font,StrFromResID(IDS_MD_HKM_FONTNAME));
	pParam->AddParam("kmBig_size",kmBig_size,StrFromResID(IDS_MD_HKM_FONTSIZE));
	pParam->AddParam("kmBig_wscale",kmBig_wscale,StrFromResID(IDS_MD_HKM_FONTWSCALE));
	pParam->AddParam("kmBig_dx",kmBig_dx,StrFromResID(IDS_MD_HKM_LRSPACE));
	pParam->AddParam("kmBig_dy",kmBig_dy,StrFromResID(IDS_MD_HKM_TBSPACE));
	
	pParam->AddFontNameParam("kmLit_font",kmLit_font,StrFromResID(IDS_MD_KM_FONTNAME));
	pParam->AddParam("kmLit_size",kmLit_size,StrFromResID(IDS_MD_KM_FONTSIZE));
	pParam->AddParam("kmLit_wscale",kmLit_wscale,StrFromResID(IDS_MD_KM_FONTWSCALE));
	pParam->AddParam("kmLit_dx",kmLit_dx,StrFromResID(IDS_MD_KM_LRSPACE));
	pParam->AddParam("kmLit_dy",kmLit_dy,StrFromResID(IDS_MD_KM_TBSPACE));
	
	pParam->AddParam("bGeoNumber",(bool)bGeoNumber,StrFromResID(IDS_MD_LL_USE));
	pParam->AddFontNameParam("geo_font",geo_font,StrFromResID(IDS_MD_LL_FONTNAME));
	pParam->AddParam("geo_size",geo_size,StrFromResID(IDS_MD_LL_FONTSIZE));
	pParam->AddParam("geo_wscale",geo_wscale,StrFromResID(IDS_MD_LL_FONTWSCALE));
	pParam->AddParam("geo_dx",geo_dx,StrFromResID(IDS_MD_LL_LRSPACE));
	pParam->AddParam("geo_dy",geo_dy,StrFromResID(IDS_MD_LL_TBSPACE));
	
}

void CGrid::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"type",pvar) )
	{
		type = (long)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"cross_len",pvar) )
	{
		cross_len = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"bFrameLine",pvar) )
	{
		bFrameLine = (bool)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"grid_wid",pvar) )
	{
		grid_wid = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"kmBig_font",pvar) )
	{
		strncpy(kmBig_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(kmBig_font)-1);
	}
	if( tab.GetValue(0,"kmBig_size",pvar) )
	{
		kmBig_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"kmBig_wscale",pvar) )
	{
		kmBig_wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"kmBig_dx",pvar) )
	{
		kmBig_dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"kmBig_dy",pvar) )
	{
		kmBig_dy = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"kmLit_font",pvar) )
	{
		strncpy(kmLit_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(kmLit_font)-1);
	}
	if( tab.GetValue(0,"kmLit_size",pvar) )
	{
		kmLit_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"kmLit_wscale",pvar) )
	{
		kmLit_wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"kmLit_dx",pvar) )
	{
		kmLit_dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"kmLit_dy",pvar) )
	{
		kmLit_dy = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"bGeoNumber",pvar) )
	{
		bGeoNumber = (bool)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"geo_font",pvar) )
	{
		strncpy(geo_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(geo_font)-1);
	}
	if( tab.GetValue(0,"geo_size",pvar) )
	{
		geo_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"geo_wscale",pvar) )
	{
		geo_wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"geo_dx",pvar) )
	{
		geo_dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"geo_dy",pvar) )
	{
		geo_dy = (float)(_variant_t)*pvar;
	}
}

void CGrid::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = (long)type;
	tab.AddValue("type",&CVariantEx(var));	

	var = cross_len;
	tab.AddValue("cross_len",&CVariantEx(var));	

	var = (bool)bFrameLine;
	tab.AddValue("bFrameLine",&CVariantEx(var));	

	var = grid_wid;
	tab.AddValue("grid_wid",&CVariantEx(var));	

	var = kmBig_font;
	tab.AddValue("kmBig_font",&CVariantEx(var));
	
	var = kmBig_size;
	tab.AddValue("kmBig_size",&CVariantEx(var));
	
	var = kmBig_wscale;
	tab.AddValue("kmBig_wscale",&CVariantEx(var));
	
	var = kmBig_dx;
	tab.AddValue("kmBig_dx",&CVariantEx(var));
	
	var = kmBig_dy;
	tab.AddValue("kmBig_dy",&CVariantEx(var));
	
	var = kmBig_size;
	tab.AddValue("kmBig_size",&CVariantEx(var));
	
	var = kmLit_font;
	tab.AddValue("kmLit_font",&CVariantEx(var));
	
	var = kmLit_size;
	tab.AddValue("kmLit_size",&CVariantEx(var));
	
	var = kmLit_wscale;
	tab.AddValue("kmLit_wscale",&CVariantEx(var));
	
	var = kmLit_dx;
	tab.AddValue("kmLit_dx",&CVariantEx(var));
	
	var = kmLit_dy;
	tab.AddValue("kmLit_dy",&CVariantEx(var));
	
	var = (bool)bGeoNumber;
	tab.AddValue("bGeoNumber",&CVariantEx(var));

	var = geo_font;
	tab.AddValue("geo_font",&CVariantEx(var));
	
	var = geo_size;
	tab.AddValue("geo_size",&CVariantEx(var));
	
	var = geo_wscale;
	tab.AddValue("geo_wscale",&CVariantEx(var));
	
	var = geo_dx;
	tab.AddValue("geo_dx",&CVariantEx(var));
	
	var = geo_dy;
	tab.AddValue("geo_dy",&CVariantEx(var));
}


void CGrid::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	//格网长度 10 cm
	float gridwid = grid_wid*m_pAssist->Unit();

	PT_3D pt1,pt3,pt4,pt5,pt6,pt7,pt8;
	PT_3DEX expt;

	Envelope e;
	e.CreateFromPts(m_pMain->m_outerBound,4);

	Envelope e2;
	e2.CreateFromPts(m_pMain->m_innerBound,4);

	double toler = cross_len*0.5*m_pAssist->Unit();

	double sx = ceil((e2.m_xl+toler)/gridwid)*gridwid;
	double sy = ceil((e2.m_yl+toler)/gridwid)*gridwid;
	double x,y;	

	if( e2.Width()/sx>100 || e2.Height()/sy>100 )
		return;

	COuterBound *pOutB = (COuterBound*)m_pMain->GetObj("OuterBound");

	CFtrLayer *pLayer = NULL;

	//格网线
	if( type==1 )
	{
		pt5.z = pt6.z = 0;

		//横格网线
		for( y=sy; y<e2.m_yh-toler; y+=gridwid)
		{
			pt3 = m_pMain->m_outerBound[0];
			pt4 = m_pMain->m_outerBound[3];
			GraphAPI::GGetLineIntersectLine(0,y,1000,0,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);

			pt3 = m_pMain->m_outerBound[1];
			pt4 = m_pMain->m_outerBound[2];
			GraphAPI::GGetLineIntersectLine(0,y,1000,0,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);

			BOOL bCutHead = FALSE, bCutTail = FALSE;

			if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[0].x,m_pMain->m_innerBound[0].y,m_pMain->m_innerBound[1].x,m_pMain->m_innerBound[1].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}
			else if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[2].x,m_pMain->m_innerBound[2].y,m_pMain->m_innerBound[3].x,m_pMain->m_innerBound[3].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}

			CGeoCurve* pGeo1 = m_pAssist->AddLine(layer,pt5,pt6,0);
			if( (5000==m_pMain->m_scale || 10000==m_pMain->m_scale) && pGeo1)
				pGeo1->SetSymbolName("@0.20X0.20");

			//公里数注记（百公里数，公里数）
			CString big,lit;
			lit.Format("%02d",((int)(y/1000))%100);
			big.Format("%d",((int)(y/1000))/100);

			pt3 = pt5;
			pt3.x += pOutB->width*0.5*m_pAssist->Unit();
			pt3.y += kmLit_size*m_pAssist->Unit();
			CGeoText *pText = NULL;

			//完整百公里或者靠近图廓角，就需要添加百公里注记
			if( !bCutHead )
			{
				if( lit.CompareNoCase("00")==0 || fabs(y-sy)<1e-4 || (y+gridwid)>=(e2.m_yh-toler) )
				{
					pText = m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_LEFT|TAV_TOP);
				}

				if( pText )
				{
					Envelope e = m_pAssist->GetTextEnvelope(pText);
					pt3.x += e.Width();
				}
				
				m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_LEFT|TAV_TOP);
			}

			pt3 = pt6;
			pt3.x -= pOutB->width*0.5*m_pAssist->Unit();
			pt3.y += kmLit_size*m_pAssist->Unit();

			if( !bCutTail )
			{
				pText = m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_RIGHT|TAV_TOP);

				if( pText )
				{
					//完整百公里或者靠近图廓角，就需要添加百公里注记
					if( lit.CompareNoCase("00")==0 || fabs(y-sy)<1e-4 || (y+gridwid)>=(e2.m_yh-toler) )
					{
						Envelope e = m_pAssist->GetTextEnvelope(pText);
						pt3.x -= e.Width();
						m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_RIGHT|TAV_TOP);
					}
				}
			}
		}

		//竖格网线
		for( x=sx; x<e2.m_xh-toler; x+=gridwid)
		{
			pt3 = m_pMain->m_outerBound[0];
			pt4 = m_pMain->m_outerBound[1];
			GraphAPI::GGetLineIntersectLine(x,0,0,1000,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
			
			pt3 = m_pMain->m_outerBound[2];
			pt4 = m_pMain->m_outerBound[3];
			GraphAPI::GGetLineIntersectLine(x,0,0,1000,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);

			BOOL bCutHead = FALSE, bCutTail = FALSE;

			if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[0].x,m_pMain->m_innerBound[0].y,m_pMain->m_innerBound[3].x,m_pMain->m_innerBound[3].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}
			else if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[1].x,m_pMain->m_innerBound[1].y,m_pMain->m_innerBound[2].x,m_pMain->m_innerBound[2].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}

			CGeoCurve* pGeo2 = m_pAssist->AddLine(layer,pt5,pt6,0);
			if( (5000==m_pMain->m_scale || 10000==m_pMain->m_scale) && pGeo2)
				pGeo2->SetSymbolName("@0.20X0.20");
			
			//公里数注记（百公里数，公里数）
			CString big,lit;
			lit.Format("%02d",((int)(x/1000))%100);
			big.Format("%d",((int)(x/1000))/100);
			
			pt3 = pt6;		
			pt3.y -= pOutB->width*0.5*m_pAssist->Unit();
			if( !bCutTail )
			{
				m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_LEFT|TAV_TOP);

				//完整百公里或者靠近图廓角，就需要添加百公里注记
				if( lit.CompareNoCase("00")==0 || fabs(x-sx)<1e-4 || (x+gridwid)>=(e2.m_xh-toler) )
				{
					m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_RIGHT|TAV_TOP);
				}
			}
			
			pt3 = pt5;
			pt3.y += pOutB->width*0.5*m_pAssist->Unit();

			if( !bCutHead )
			{			
				m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_LEFT|TAV_BOTTOM);

				//完整百公里或者靠近图廓角，就需要添加百公里注记
				if( lit.CompareNoCase("00")==0 || fabs(x-sx)<1e-4 || (x+gridwid)>=(e2.m_xh-toler) )
				{
					m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_RIGHT|TAV_BOTTOM);
				}
			}
		}
	}
	//十字丝
	else
	{
		PT_3D boundPts2[5], boundPts3[5];
		memcpy(boundPts2,m_pMain->m_innerBound,4*sizeof(PT_3D));
		boundPts2[4] = boundPts2[0];

		GraphAPI::GGetParallelLine(boundPts2,5,cross_len*3*m_pAssist->Unit(),boundPts3);
		for( x=sx; x<e2.m_xh-toler; x+=gridwid)
		{
			for( y=sy; y<e2.m_yh-toler; y+=gridwid)
			{
				pt1.x = x; pt1.y = y; pt1.z = 0;

				if( GraphAPI::GIsPtInRegion(pt1,boundPts3,4)!=2 )
					continue;

				CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
				if( !pFtr )
					continue;

				CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
				
				m_pAssist->CreateCross(pGeo,pt1,0,cross_len*0.5*m_pAssist->Unit());
				m_pAssist->AddObject(pFtr,pLayer->GetID());

				pFtr = m_pAssist->CreateCurve(layer,pLayer);
				if( !pFtr )
					continue;

				pGeo = (CGeoCurve*)pFtr->GetGeometry();
				
				m_pAssist->CreateCross(pGeo,pt1,1,cross_len*0.5*m_pAssist->Unit());				
				m_pAssist->AddObject(pFtr,pLayer->GetID());
			}
		}

		//------边上的刻度线
		pt5.z = pt6.z = pt7.z = pt8.z = 0;

		double len = cross_len*m_pAssist->Unit()*0.5;

		PT_3DEX pts[2];

		//横刻度线
		if( bFrameLine )
		{
			for( y=sy; y<e2.m_yh-toler; y+=gridwid)
			{
				pt3 = m_pMain->m_innerBound[0];
				pt4 = m_pMain->m_innerBound[3];
				GraphAPI::GGetLineIntersectLine(0,y,1000,0,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);

				pt3 = m_pMain->m_innerBound[1];
				pt4 = m_pMain->m_innerBound[2];
				GraphAPI::GGetLineIntersectLine(0,y,1000,0,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);

				double r = GraphAPI::GGet2DDisOf2P(pt5,pt6);
				if( r<cross_len*m_pAssist->Unit() )continue;

				double dx = (pt6.x-pt5.x)/r, dy = (pt6.y-pt5.y)/r;

				pt7.x = pt5.x + 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt5.y + 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt5.x + len*dx;
				pt8.y = pt5.y + len*dy;

				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt5,pt8,0);
				}

				pt7.x = pt6.x - 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt6.y - 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt6.x - len*dx;
				pt8.y = pt6.y - len*dy;

				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt6,pt8,0);
				}			
			}

			//竖刻度线
			for( x=sx; x<e2.m_xh-toler; x+=gridwid)
			{			
				pt3 = m_pMain->m_innerBound[0];
				pt4 = m_pMain->m_innerBound[1];
				GraphAPI::GGetLineIntersectLine(x,0,0,1000,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
				
				pt3 = m_pMain->m_innerBound[2];
				pt4 = m_pMain->m_innerBound[3];
				GraphAPI::GGetLineIntersectLine(x,0,0,1000,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);
				
				double r = GraphAPI::GGet2DDisOf2P(pt5,pt6);
				if( r<m_pAssist->Unit() )continue;
				
				double dx = (pt6.x-pt5.x)/r, dy = (pt6.y-pt5.y)/r;
				
				pt7.x = pt5.x + 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt5.y + 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt5.x + len*dx;
				pt8.y = pt5.y + len*dy;

				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt5,pt8,0);
				}
				
				pt7.x = pt6.x - 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt6.y - 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt6.x - len*dx;
				pt8.y = pt6.y - len*dy;
				
				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt6,pt8,0);
				}
			}
		}
	}
}


CGrid_New::CGrid_New():CGrid()
{
}

CGrid_New::~CGrid_New()
{
}

void CGrid_New::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	COuterBoundDecorator* pOBD = (COuterBoundDecorator*)m_pMain->GetObj("OuterBoundDecorator");
	if( !pOBD ) return ;

	PT_3D ptsBound[4];
	memcpy(ptsBound, m_pMain->m_outerBound, sizeof(PT_3D)*4);
	double ext = pOBD->extension*m_pAssist->Unit();
	Extend(ptsBound, ext);


	//格网长度 10 cm
	float gridwid = grid_wid*m_pAssist->Unit();

	PT_3D pt1,pt3,pt4,pt5,pt6,pt7,pt8;
	PT_3DEX expt;

	Envelope e;
	e.CreateFromPts(m_pMain->m_outerBound,4);

	Envelope e2;
	e2.CreateFromPts(m_pMain->m_innerBound,4);

	double toler = cross_len*0.5*m_pAssist->Unit();

	double sx = ceil((e2.m_xl+toler)/gridwid)*gridwid;
	double sy = ceil((e2.m_yl+toler)/gridwid)*gridwid;
	double x,y;	

	if( e2.Width()/sx>100 || e2.Height()/sy>100 )
		return;

	COuterBound *pOutB = (COuterBound*)m_pMain->GetObj("OuterBound");

	CFtrLayer *pLayer = NULL;

	//格网线
	if( type==1 )
	{
		pt5.z = pt6.z = 0;

		//横格网线
		for( y=sy; y<e2.m_yh-toler; y+=gridwid)
		{
			pt3 = ptsBound[0];
			pt4 = ptsBound[3];
			GraphAPI::GGetLineIntersectLine(0,y,1000,0,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);

			pt3 = ptsBound[1];
			pt4 = ptsBound[2];
			GraphAPI::GGetLineIntersectLine(0,y,1000,0,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);

			BOOL bCutHead = FALSE, bCutTail = FALSE;

			if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[0].x,m_pMain->m_innerBound[0].y,m_pMain->m_innerBound[1].x,m_pMain->m_innerBound[1].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}
			else if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[2].x,m_pMain->m_innerBound[2].y,m_pMain->m_innerBound[3].x,m_pMain->m_innerBound[3].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}

			CGeoCurve* pGeo1 = m_pAssist->AddLine(layer,pt5,pt6,0);
			if( (5000==m_pMain->m_scale || 10000==m_pMain->m_scale) && pGeo1)
				pGeo1->SetSymbolName("@0.20X0.20");

			//公里数注记（百公里数，公里数）
			CString big,lit;
			lit.Format("%02d",((int)(y/1000))%100);
			big.Format("%d",((int)(y/1000))/100);

			pt3 = pt5;
			pt3.x += pOutB->width*0.5*m_pAssist->Unit();
			pt3.y += kmLit_size*m_pAssist->Unit();
			CGeoText *pText = NULL;

			//完整百公里或者靠近图廓角，就需要添加百公里注记
			if( !bCutHead )
			{
				if( lit.CompareNoCase("00")==0 || fabs(y-sy)<1e-4 || (y+gridwid)>=(e2.m_yh-toler) )
				{
					pText = m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_LEFT|TAV_TOP);
				}

				if( pText )
				{
					Envelope e = m_pAssist->GetTextEnvelope(pText);
					pt3.x += e.Width();
				}
				
				m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_LEFT|TAV_TOP);
			}

			pt3 = pt6;
			pt3.x -= pOutB->width*0.5*m_pAssist->Unit();
			pt3.y += kmLit_size*m_pAssist->Unit();

			if( !bCutTail )
			{
				pText = m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_RIGHT|TAV_TOP);

				if( pText )
				{
					//完整百公里或者靠近图廓角，就需要添加百公里注记
					if( lit.CompareNoCase("00")==0 || fabs(y-sy)<1e-4 || (y+gridwid)>=(e2.m_yh-toler) )
					{
						Envelope e = m_pAssist->GetTextEnvelope(pText);
						pt3.x -= e.Width();
						m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_RIGHT|TAV_TOP);
					}
				}
			}
		}

		//竖格网线
		for( x=sx; x<e2.m_xh-toler; x+=gridwid)
		{
			pt3 = ptsBound[0];
			pt4 = ptsBound[1];
			GraphAPI::GGetLineIntersectLine(x,0,0,1000,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
			
			pt3 = ptsBound[2];
			pt4 = ptsBound[3];
			GraphAPI::GGetLineIntersectLine(x,0,0,1000,
				pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);

			BOOL bCutHead = FALSE, bCutTail = FALSE;

			if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[0].x,m_pMain->m_innerBound[0].y,m_pMain->m_innerBound[3].x,m_pMain->m_innerBound[3].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}
			else if( GraphAPI::GGetLineIntersectLineSeg(pt5.x,pt5.y,pt6.x,pt6.y,
				m_pMain->m_innerBound[1].x,m_pMain->m_innerBound[1].y,m_pMain->m_innerBound[2].x,m_pMain->m_innerBound[2].y,&pt7.x,&pt7.y,NULL) )
			{
				pt7.z = 0;
				pt8.x = (pt7.x+pt5.x)*0.5; pt8.y = (pt7.y+pt5.y)*0.5; pt8.z = 0;
				if( GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					pt6 = pt7;
					bCutTail = TRUE;
				}
				else
				{
					pt5 = pt7;
					bCutHead = TRUE;
				}
			}

			CGeoCurve* pGeo2 = m_pAssist->AddLine(layer,pt5,pt6,0);
			if( (5000==m_pMain->m_scale || 10000==m_pMain->m_scale) && pGeo2)
				pGeo2->SetSymbolName("@0.20X0.20");
			
			//公里数注记（百公里数，公里数）
			CString big,lit;
			lit.Format("%02d",((int)(x/1000))%100);
			big.Format("%d",((int)(x/1000))/100);
			
			pt3 = pt6;		
			pt3.y -= pOutB->width*0.5*m_pAssist->Unit();
			if( !bCutTail )
			{
				m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_LEFT|TAV_TOP);

				//完整百公里或者靠近图廓角，就需要添加百公里注记
				if( lit.CompareNoCase("00")==0 || fabs(x-sx)<1e-4 || (x+gridwid)>=(e2.m_xh-toler) )
				{
					m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_RIGHT|TAV_TOP);
				}
			}
			
			pt3 = pt5;
			pt3.y += pOutB->width*0.5*m_pAssist->Unit();

			if( !bCutHead )
			{			
				m_pAssist->AddText(layer,pt3,lit,kmLit_font,kmLit_size,kmLit_wscale,TAH_LEFT|TAV_BOTTOM);

				//完整百公里或者靠近图廓角，就需要添加百公里注记
				if( lit.CompareNoCase("00")==0 || fabs(x-sx)<1e-4 || (x+gridwid)>=(e2.m_xh-toler) )
				{
					m_pAssist->AddText(layer,pt3,big,kmBig_font,kmBig_size,kmBig_wscale,TAH_RIGHT|TAV_BOTTOM);
				}
			}
		}
	}
	//十字丝
	else
	{
		PT_3D boundPts2[5], boundPts3[5];
		memcpy(boundPts2,m_pMain->m_innerBound,4*sizeof(PT_3D));
		boundPts2[4] = boundPts2[0];

		GraphAPI::GGetParallelLine(boundPts2,5,cross_len*3*m_pAssist->Unit(),boundPts3);
		for( x=sx; x<e2.m_xh-toler; x+=gridwid)
		{
			for( y=sy; y<e2.m_yh-toler; y+=gridwid)
			{
				pt1.x = x; pt1.y = y; pt1.z = 0;

				if( GraphAPI::GIsPtInRegion(pt1,boundPts3,4)!=2 )
					continue;

				CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
				if( !pFtr )
					continue;

				CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
				
				m_pAssist->CreateCross(pGeo,pt1,0,cross_len*0.5*m_pAssist->Unit());
				m_pAssist->AddObject(pFtr,pLayer->GetID());

				pFtr = m_pAssist->CreateCurve(layer,pLayer);
				if( !pFtr )
					continue;

				pGeo = (CGeoCurve*)pFtr->GetGeometry();
				
				m_pAssist->CreateCross(pGeo,pt1,1,cross_len*0.5*m_pAssist->Unit());				
				m_pAssist->AddObject(pFtr,pLayer->GetID());
			}
		}

		//------边上的刻度线
		pt5.z = pt6.z = pt7.z = pt8.z = 0;

		double len = cross_len*m_pAssist->Unit()*0.5;

		PT_3DEX pts[2];

		//横刻度线
		if( bFrameLine )
		{
			for( y=sy; y<e2.m_yh-toler; y+=gridwid)
			{
				pt3 = m_pMain->m_innerBound[0];
				pt4 = m_pMain->m_innerBound[3];
				GraphAPI::GGetLineIntersectLine(0,y,1000,0,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);

				pt3 = m_pMain->m_innerBound[1];
				pt4 = m_pMain->m_innerBound[2];
				GraphAPI::GGetLineIntersectLine(0,y,1000,0,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);

				double r = GraphAPI::GGet2DDisOf2P(pt5,pt6);
				if( r<cross_len*m_pAssist->Unit() )continue;

				double dx = (pt6.x-pt5.x)/r, dy = (pt6.y-pt5.y)/r;

				pt7.x = pt5.x + 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt5.y + 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt5.x + len*dx;
				pt8.y = pt5.y + len*dy;

				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt5,pt8,0);
				}

				pt7.x = pt6.x - 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt6.y - 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt6.x - len*dx;
				pt8.y = pt6.y - len*dy;

				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt6,pt8,0);
				}			
			}

			//竖刻度线
			for( x=sx; x<e2.m_xh-toler; x+=gridwid)
			{			
				pt3 = m_pMain->m_innerBound[0];
				pt4 = m_pMain->m_innerBound[1];
				GraphAPI::GGetLineIntersectLine(x,0,0,1000,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt5.x,&pt5.y,NULL);
				
				pt3 = m_pMain->m_innerBound[2];
				pt4 = m_pMain->m_innerBound[3];
				GraphAPI::GGetLineIntersectLine(x,0,0,1000,
					pt3.x,pt3.y,pt4.x-pt3.x,pt4.y-pt3.y,&pt6.x,&pt6.y,NULL);
				
				double r = GraphAPI::GGet2DDisOf2P(pt5,pt6);
				if( r<m_pAssist->Unit() )continue;
				
				double dx = (pt6.x-pt5.x)/r, dy = (pt6.y-pt5.y)/r;
				
				pt7.x = pt5.x + 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt5.y + 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt5.x + len*dx;
				pt8.y = pt5.y + len*dy;

				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt5,pt8,0);
				}
				
				pt7.x = pt6.x - 0.01*m_pAssist->Unit()*dx;
				pt7.y = pt6.y - 0.01*m_pAssist->Unit()*dy;
				pt8.x = pt6.x - len*dx;
				pt8.y = pt6.y - len*dy;
				
				if( GraphAPI::GIsPtInRegion(pt7,m_pMain->m_innerBound,4)==2 && GraphAPI::GIsPtInRegion(pt8,m_pMain->m_innerBound,4)==2 )
				{
					m_pAssist->AddLine(layer,pt6,pt8,0);
				}
			}
		}
	}
}



CLTname::CLTname()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_NAMENUM_LT);
	strcpy(name_layer,"TK_name1");
	strcpy(number_layer,"TK_number1");

	strcpy(name_font,StrFromResID(IDS_MD_HEITI));
	name_size = 4.5;
	name_wscale = 1;
//	name_dx = 0;
	name_dy = 30;
	
	strcpy(number_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	number_size = 3.5;
	number_wscale = 1;
	number_dx = 0;
	number_dy = 37.5;

	if(5000 == scale || 10000 == scale)
	{
		m_bUsed = TRUE;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if(MapDecorate::bMapDecorateNew)
		{
			m_bUsed = TRUE;
		}
		else
		{
			m_bUsed = FALSE;
		}
	}

}


CLTname::~CLTname()
{
}

void CLTname::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("name_layer",name_layer,StrFromResID(IDS_MAPNAME_LAYER));
	pParam->AddParam("number_layer",number_layer,StrFromResID(IDS_MAPNUM_LAYER));
	pParam->AddFontNameParam("name_font",name_font,StrFromResID(IDS_MD_NAME_FONTNAME));			
	pParam->AddParam("name_size",name_size,StrFromResID(IDS_MD_NAME_FONTSIZE));
	pParam->AddParam("name_wscale",name_wscale,StrFromResID(IDS_MD_NAME_FONTWSCALE));
//	pParam->AddParam("name_dx",name_dx,StrFromResID(IDS_MD_NAME_RSPACE));
	pParam->AddParam("name_dy",name_dy,StrFromResID(IDS_MD_NAME_BSPACE));
	
	pParam->AddFontNameParam("number_font",number_font,StrFromResID(IDS_MD_NUM_FONTNAME));			
	pParam->AddParam("number_size",number_size,StrFromResID(IDS_MD_NUM_FONTSIZE));
	pParam->AddParam("name_wscale",number_wscale,StrFromResID(IDS_MD_NUM_FONTWSCALE));
	pParam->AddParam("number_dx",number_dx,StrFromResID(IDS_MD_NUM_RSPACE));
	pParam->AddParam("number_dy",number_dy,StrFromResID(IDS_MD_NUM_BSPACE));
}

void CLTname::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"name_layer",pvar) )
	{
		strncpy(name_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_layer)-1);
	}
	if( tab.GetValue(0,"number_layer",pvar) )
	{
		strncpy(number_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_layer)-1);
	}
	if( tab.GetValue(0,"name_font",pvar) )
	{
		strncpy(name_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_font)-1);
	}
	if( tab.GetValue(0,"name_size",pvar) )
	{
		name_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"name_wscale",pvar) )
	{
		name_wscale = (float)(_variant_t)*pvar;
	}
//	if( tab.GetValue(0,"name_dx",pvar) )
//	{
//		name_dx = (float)(_variant_t)*pvar;
//	}
	if( tab.GetValue(0,"name_dy",pvar) )
	{
		name_dy = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"number_font",pvar) )
	{
		strncpy(number_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_font)-1);
	}
	if( tab.GetValue(0,"number_size",pvar) )
	{
		number_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_wscale",pvar) )
	{
		number_wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dx",pvar) )
	{
		number_dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dy",pvar) )
	{
		number_dy = (float)(_variant_t)*pvar;
	}
}

void CLTname::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)name_layer;
	tab.AddValue("name_layer",&CVariantEx(var));

	var = (LPCTSTR)number_layer;
	tab.AddValue("number_layer",&CVariantEx(var));
	
	var = name_font;
	tab.AddValue("name_font",&CVariantEx(var));
	
	var = number_font;
	tab.AddValue("number_font",&CVariantEx(var));
	
	var = name_size;
	tab.AddValue("name_size",&CVariantEx(var));
	
	var = name_wscale;
	tab.AddValue("name_wscale",&CVariantEx(var));
	
//	var = name_dx;
//	tab.AddValue("name_dx",&CVariantEx(var));
	
	var = name_dy;
	tab.AddValue("name_dy",&CVariantEx(var));
	
	var = number_size;
	tab.AddValue("number_size",&CVariantEx(var));
	
	var = number_wscale;
	tab.AddValue("number_wscale",&CVariantEx(var));
	
	var = number_dx;
	tab.AddValue("number_dx",&CVariantEx(var));
	
	var = number_dy;
	tab.AddValue("number_dy",&CVariantEx(var));
}


void CLTname::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	CNumber *pNumber = (CNumber*)m_pMain->GetObj("Number");
	CName *pName = (CName*)m_pMain->GetObj("Name");
	
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;

	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();
	
	//创建左上角图号
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xl-number_dx*m_pAssist->Unit();
	else
		textpt.x = e2.m_xl-number_dx*m_pAssist->Unit();
	textpt.y = e.m_yh+number_dy*m_pAssist->Unit();
	textpt.z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(number_layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pNumber->number,number_font,number_size,0,TAH_RIGHT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
	
	//创建左上角图名
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xl-number_dx*m_pAssist->Unit()-m_pAssist->GetTextEnvelope(pText).Width()/2;
	else
		textpt.x = e2.m_xl-number_dx*m_pAssist->Unit()-m_pAssist->GetTextEnvelope(pText).Width()/2;
	textpt.y = e.m_yh+name_dy*m_pAssist->Unit();
	textpt.z = 0;
	
	pFtr = m_pAssist->CreateText(name_layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pName->name,name_font,name_size,0,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CLBname::CLBname()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_NAMENUM_LB);
	strcpy(name_layer,"TK_name1");
	strcpy(number_layer,"TK_number1");

	strcpy(name_font,StrFromResID(IDS_MD_HEITI));
	name_size = 4.5;
	name_wscale = 1;
//	name_dx = 0;
	name_dy = 30;
	
	strcpy(number_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	number_size = 3.5;
	number_wscale = 1;
	number_dx = 0;
	number_dy = 37.5;

	if( 5000 == scale || 10000 == scale )
	{
		m_bUsed = TRUE;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale )
	{
		if(!MapDecorate::bMapDecorateNew)
		{
			name_dy = 5;
			number_dy = 13;
		}
		m_bUsed = TRUE;
	}
}


CLBname::~CLBname()
{
}

void CLBname::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("name_layer",name_layer,StrFromResID(IDS_MAPNAME_LAYER));
	pParam->AddParam("number_layer",number_layer,StrFromResID(IDS_MAPNUM_LAYER));
	pParam->AddFontNameParam("name_font",name_font,StrFromResID(IDS_MD_NAME_FONTNAME));			
	pParam->AddParam("name_size",name_size,StrFromResID(IDS_MD_NAME_FONTSIZE));
	pParam->AddParam("name_wscale",name_wscale,StrFromResID(IDS_MD_NAME_FONTWSCALE));
//	pParam->AddParam("name_dx",name_dx,StrFromResID(IDS_MD_NAME_RSPACE));
	pParam->AddParam("name_dy",name_dy,StrFromResID(IDS_MD_NAME_TSPACE));
	
	pParam->AddFontNameParam("number_font",number_font,StrFromResID(IDS_MD_NUM_FONTNAME));			
	pParam->AddParam("number_size",number_size,StrFromResID(IDS_MD_NUM_FONTSIZE));
	pParam->AddParam("name_wscale",number_wscale,StrFromResID(IDS_MD_NUM_FONTWSCALE));
	pParam->AddParam("number_dx",number_dx,StrFromResID(IDS_MD_NUM_RSPACE));
	pParam->AddParam("number_dy",number_dy,StrFromResID(IDS_MD_NUM_TSPACE));
}

void CLBname::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"name_layer",pvar) )
	{
		strncpy(name_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_layer)-1);
	}
	if( tab.GetValue(0,"number_layer",pvar) )
	{
		strncpy(number_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_layer)-1);
	}	
	if( tab.GetValue(0,"name_font",pvar) )
	{
		strncpy(name_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_font)-1);
	}
	if( tab.GetValue(0,"name_size",pvar) )
	{
		name_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"name_wscale",pvar) )
	{
		name_wscale = (float)(_variant_t)*pvar;
	}
//	if( tab.GetValue(0,"name_dx",pvar) )
//	{
//		name_dx = (float)(_variant_t)*pvar;
//	}
	if( tab.GetValue(0,"name_dy",pvar) )
	{
		name_dy = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"number_font",pvar) )
	{
		strncpy(number_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_font)-1);
	}
	if( tab.GetValue(0,"number_size",pvar) )
	{
		number_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_wscale",pvar) )
	{
		number_wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dx",pvar) )
	{
		number_dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dy",pvar) )
	{
		number_dy = (float)(_variant_t)*pvar;
	}
}

void CLBname::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)name_layer;
	tab.AddValue("name_layer",&CVariantEx(var));
	
	var = (LPCTSTR)number_layer;
	tab.AddValue("number_layer",&CVariantEx(var));	
	
	var = name_font;
	tab.AddValue("name_font",&CVariantEx(var));
	
	var = number_font;
	tab.AddValue("number_font",&CVariantEx(var));
	
	var = name_size;
	tab.AddValue("name_size",&CVariantEx(var));
	
	var = name_wscale;
	tab.AddValue("name_wscale",&CVariantEx(var));
	
//	var = name_dx;
//	tab.AddValue("name_dx",&CVariantEx(var));
	
	var = name_dy;
	tab.AddValue("name_dy",&CVariantEx(var));
	
	var = number_size;
	tab.AddValue("number_size",&CVariantEx(var));
	
	var = number_wscale;
	tab.AddValue("number_wscale",&CVariantEx(var));
	
	var = number_dx;
	tab.AddValue("number_dx",&CVariantEx(var));
	
	var = number_dy;
	tab.AddValue("number_dy",&CVariantEx(var));
}


void CLBname::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);	

	CNumber *pNumber = (CNumber*)m_pMain->GetObj("Number");
	CName *pName = (CName*)m_pMain->GetObj("Name");
	
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;
	
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	//创建左下角图号
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xl-number_dx*m_pAssist->Unit();
	else
		textpt.x = e2.m_xl-number_dx*m_pAssist->Unit();
	textpt.y = e.m_yl-number_dy*m_pAssist->Unit();
	textpt.z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(number_layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pNumber->number,number_font,number_size,0,TAH_RIGHT|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
	
	//创建左下角图名
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xl-number_dx*m_pAssist->Unit()-m_pAssist->GetTextEnvelope(pText).Width()/2;
	else
		textpt.x = e2.m_xl-number_dx*m_pAssist->Unit()-m_pAssist->GetTextEnvelope(pText).Width()/2;
	textpt.y = e.m_yl-name_dy*m_pAssist->Unit();
	textpt.z = 0;
	
	pFtr = m_pAssist->CreateText(name_layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pName->name,name_font,name_size,0,TAH_MID|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CRTname::CRTname()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();
	m_strName = StrFromResID(IDS_MD_NAMENUM_RT);
	strcpy(name_layer,"TK_name1");
	strcpy(number_layer,"TK_number1");

	strcpy(name_font,StrFromResID(IDS_MD_HEITI));
	name_size = 4.5;
	name_wscale = 1;
//	name_dx = 0;
	name_dy = 30;
	
	strcpy(number_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	number_size = 3.5;
	number_wscale = 1;
	number_dx = 70;
	number_dy = 37.5;

	if( 5000 == scale || 10000 == scale )
	{
		number_dx = 90;
		m_bUsed = TRUE;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale )
	{
		if(!MapDecorate::bMapDecorateNew)
		{
			name_dy = 9.5;
			number_dy = 3;
			number_dx = 15.75;
		}
		m_bUsed = TRUE;
	}
}


CRTname::~CRTname()
{
}

void CRTname::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("name_layer",name_layer,StrFromResID(IDS_MAPNAME_LAYER));
	pParam->AddParam("number_layer",number_layer,StrFromResID(IDS_MAPNUM_LAYER));
	pParam->AddFontNameParam("name_font",name_font,StrFromResID(IDS_MD_NAME_FONTNAME));			
	pParam->AddParam("name_size",name_size,StrFromResID(IDS_MD_NAME_FONTSIZE));
	pParam->AddParam("name_wscale",name_wscale,StrFromResID(IDS_MD_NAME_FONTWSCALE));
//	pParam->AddParam("name_dx",name_dx,StrFromResID(IDS_MD_NAME_LSPACE));
	pParam->AddParam("name_dy",name_dy,StrFromResID(IDS_MD_NAME_BSPACE));
	
	pParam->AddFontNameParam("number_font",number_font,StrFromResID(IDS_MD_NUM_FONTNAME));			
	pParam->AddParam("number_size",number_size,StrFromResID(IDS_MD_NUM_FONTSIZE));
	pParam->AddParam("name_wscale",number_wscale,StrFromResID(IDS_MD_NUM_FONTWSCALE));
	pParam->AddParam("number_dx",number_dx,StrFromResID(IDS_MD_NUM_LSPACE));
	pParam->AddParam("number_dy",number_dy,StrFromResID(IDS_MD_NUM_BSPACE));
}

void CRTname::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"name_layer",pvar) )
	{
		strncpy(name_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_layer)-1);
	}
	if( tab.GetValue(0,"number_layer",pvar) )
	{
		strncpy(number_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_layer)-1);
	}
	if( tab.GetValue(0,"name_font",pvar) )
	{
		strncpy(name_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_font)-1);
	}
	if( tab.GetValue(0,"name_size",pvar) )
	{
		name_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"name_wscale",pvar) )
	{
		name_wscale = (float)(_variant_t)*pvar;
	}
//	if( tab.GetValue(0,"name_dx",pvar) )
//	{
//		name_dx = (float)(_variant_t)*pvar;
//	}
	if( tab.GetValue(0,"name_dy",pvar) )
	{
		name_dy = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"number_font",pvar) )
	{
		strncpy(number_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_font)-1);
	}
	if( tab.GetValue(0,"number_size",pvar) )
	{
		number_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_wscale",pvar) )
	{
		number_wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dx",pvar) )
	{
		number_dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dy",pvar) )
	{
		number_dy = (float)(_variant_t)*pvar;
	}
}

void CRTname::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)name_layer;
	tab.AddValue("name_layer",&CVariantEx(var));
	
	var = (LPCTSTR)number_layer;
	tab.AddValue("number_layer",&CVariantEx(var));	
	
	var = name_font;
	tab.AddValue("name_font",&CVariantEx(var));
	
	var = number_font;
	tab.AddValue("number_font",&CVariantEx(var));
	
	var = name_size;
	tab.AddValue("name_size",&CVariantEx(var));
	
	var = name_wscale;
	tab.AddValue("name_wscale",&CVariantEx(var));
	
//	var = name_dx;
//	tab.AddValue("name_dx",&CVariantEx(var));
	
	var = name_dy;
	tab.AddValue("name_dy",&CVariantEx(var));
	
	var = number_size;
	tab.AddValue("number_size",&CVariantEx(var));
	
	var = number_wscale;
	tab.AddValue("number_wscale",&CVariantEx(var));
	
	var = number_dx;
	tab.AddValue("number_dx",&CVariantEx(var));
	
	var = number_dy;
	tab.AddValue("number_dy",&CVariantEx(var));
}


void CRTname::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);	

	CNumber *pNumber = (CNumber*)m_pMain->GetObj("Number");
	CName *pName = (CName*)m_pMain->GetObj("Name");	
	
	CFtrLayer *pLayer = NULL;
	
	PT_3D textpt;

	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();
	
	//创建右上角图号
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xh+number_dx*m_pAssist->Unit(); 
	else
		textpt.x = e2.m_xh+number_dx*m_pAssist->Unit(); 
	textpt.y = e.m_yh+number_dy*m_pAssist->Unit(); 
	textpt.z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(number_layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pNumber->number,number_font,number_size,0,TAH_LEFT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
	
	//创建右上角图名
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xh+number_dx*m_pAssist->Unit()+m_pAssist->GetTextEnvelope(pText).Width()/2;
	else
		textpt.x = e2.m_xh+number_dx*m_pAssist->Unit()+m_pAssist->GetTextEnvelope(pText).Width()/2;
	textpt.y = e.m_yh+name_dy*m_pAssist->Unit();
	textpt.z = 0;
	
	pFtr = m_pAssist->CreateText(name_layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pName->name,name_font,name_size,0,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CRBname::CRBname()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_NAMENUM_RB);
	strcpy(name_layer,"TK_name1");
	strcpy(number_layer,"TK_number1");

	strcpy(name_font,StrFromResID(IDS_MD_HEITI));
	name_size = 4.5;
	name_wscale = 1;
//	name_dx = 0;
	name_dy = 30;
	
	strcpy(number_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	number_size = 3.5;
	number_wscale = 1;
	number_dx = 70;
	number_dy = 37.5;

	if( 5000 == scale || 10000 == scale )
	{
		number_dx = 90;
		m_bUsed = TRUE;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale )
	{
		if(MapDecorate::bMapDecorateNew)
		{
			m_bUsed = TRUE;
		}
		else
		{
			m_bUsed = FALSE;
		}
	}

}


CRBname::~CRBname()
{
}

void CRBname::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("name_layer",name_layer,StrFromResID(IDS_MAPNAME_LAYER));
	pParam->AddParam("number_layer",number_layer,StrFromResID(IDS_MAPNUM_LAYER));
	pParam->AddFontNameParam("name_font",name_font,StrFromResID(IDS_MD_NAME_FONTNAME));			
	pParam->AddParam("name_size",name_size,StrFromResID(IDS_MD_NAME_FONTSIZE));
	pParam->AddParam("name_wscale",name_wscale,StrFromResID(IDS_MD_NAME_FONTWSCALE));
//	pParam->AddParam("name_dx",name_dx,StrFromResID(IDS_MD_NAME_LSPACE));
	pParam->AddParam("name_dy",name_dy,StrFromResID(IDS_MD_NAME_TSPACE));
	
	pParam->AddFontNameParam("number_font",number_font,StrFromResID(IDS_MD_NUM_FONTNAME));			
	pParam->AddParam("number_size",number_size,StrFromResID(IDS_MD_NUM_FONTSIZE));
	pParam->AddParam("name_wscale",number_wscale,StrFromResID(IDS_MD_NUM_FONTWSCALE));
	pParam->AddParam("number_dx",number_dx,StrFromResID(IDS_MD_NUM_LSPACE));
	pParam->AddParam("number_dy",number_dy,StrFromResID(IDS_MD_NUM_TSPACE));
}

void CRBname::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"name_layer",pvar) )
	{
		strncpy(name_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_layer)-1);
	}
	if( tab.GetValue(0,"number_layer",pvar) )
	{
		strncpy(number_layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_layer)-1);
	}	
	if( tab.GetValue(0,"name_font",pvar) )
	{
		strncpy(name_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(name_font)-1);
	}
	if( tab.GetValue(0,"name_size",pvar) )
	{
		name_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"name_wscale",pvar) )
	{
		name_wscale = (float)(_variant_t)*pvar;
	}
//	if( tab.GetValue(0,"name_dx",pvar) )
//	{
//		name_dx = (float)(_variant_t)*pvar;
//	}
	if( tab.GetValue(0,"name_dy",pvar) )
	{
		name_dy = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"number_font",pvar) )
	{
		strncpy(number_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(number_font)-1);
	}
	if( tab.GetValue(0,"number_size",pvar) )
	{
		number_size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_wscale",pvar) )
	{
		number_wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dx",pvar) )
	{
		number_dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"number_dy",pvar) )
	{
		number_dy = (float)(_variant_t)*pvar;
	}
}

void CRBname::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)name_layer;
	tab.AddValue("name_layer",&CVariantEx(var));
	
	var = (LPCTSTR)number_layer;
	tab.AddValue("number_layer",&CVariantEx(var));	
	
	var = name_font;
	tab.AddValue("name_font",&CVariantEx(var));
	
	var = number_font;
	tab.AddValue("number_font",&CVariantEx(var));
	
	var = name_size;
	tab.AddValue("name_size",&CVariantEx(var));
	
	var = name_wscale;
	tab.AddValue("name_wscale",&CVariantEx(var));
	
//	var = name_dx;
//	tab.AddValue("name_dx",&CVariantEx(var));
	
	var = name_dy;
	tab.AddValue("name_dy",&CVariantEx(var));
	
	var = number_size;
	tab.AddValue("number_size",&CVariantEx(var));
	
	var = number_wscale;
	tab.AddValue("number_wscale",&CVariantEx(var));
	
	var = number_dx;
	tab.AddValue("number_dx",&CVariantEx(var));
	
	var = number_dy;
	tab.AddValue("number_dy",&CVariantEx(var));
}


void CRBname::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);	
	
	CFtrLayer *pLayer = NULL;

	CNumber *pNumber = (CNumber*)m_pMain->GetObj("Number");
	CName *pName = (CName*)m_pMain->GetObj("Name");		
	
	PT_3D textpt;

	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();
	
	//创建右下角图号
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xh+number_dx*m_pAssist->Unit(); 
	else
		textpt.x = e2.m_xh+number_dx*m_pAssist->Unit(); 
	textpt.y = e.m_yl-number_dy*m_pAssist->Unit(); 
	textpt.z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(number_layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pNumber->number,number_font,number_size,0,TAH_LEFT|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
	
	//创建右下角图名
	if( 25000 == scale || 50000 == scale || 100000 == scale )
		textpt.x = e.m_xh+number_dx*m_pAssist->Unit()+m_pAssist->GetTextEnvelope(pText).Width()/2;
	else
		textpt.x = e2.m_xh+number_dx*m_pAssist->Unit()+m_pAssist->GetTextEnvelope(pText).Width()/2;
	textpt.y = e.m_yl-name_dy*m_pAssist->Unit();
	textpt.z = 0;
	
	pFtr = m_pAssist->CreateText(name_layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	m_pAssist->SetText(pText,&textpt,1,pName->name,name_font,name_size,0,TAH_MID|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CFigure::CFigure()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_FIGURE);
	strcpy(layer,"TK_figure");
	dx = 5;
	dy = 35;

	if( (25000 == scale || 50000 == scale || 100000 == scale) && !MapDecorate::bMapDecorateNew)
		dx = 3;
}


CFigure::~CFigure()
{
}

void CFigure::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_TSPACE));			
	
}

void CFigure::SetParams(CValueTable& tab)
{
	const  CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CFigure::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}


void CFigure::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[3],m_pMain->m_outerBound[2],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	CString filePath = CMapDecorator::GetFigurePath(m_pMain->m_scale);
	
	PT_3D pt = PT_3D(e.m_xh+dx*m_pAssist->Unit(),e.m_yh,0);
	
	RotatePts(&pt,1,xb.cosa,xb.sina);
	
	double ang = GraphAPI::GGetAngle(m_pMain->m_outerBound[2].x,m_pMain->m_outerBound[2].y,m_pMain->m_outerBound[1].x,m_pMain->m_outerBound[1].y)-1.5*PI;
	
	m_pMain->ImportXML(filePath,m_pAssist->GetLayer(layer),pt,pt,ang);
}



CScale::CScale()
{
	strcpy(scaleText,"1:2000");

	m_strName = StrFromResID(IDS_MD_SCALE);
	strcpy(font,StrFromResID(IDS_MD_SONGTI));
	strcpy(layer,"TK_scale");
	size = 4;
	wscale = 1;
	dy_scaleRuler = 20;
	dy = 5;
	strcpy(scaleLine_font,StrFromResID(IDS_MD_HEITI));
	scaleLine_size = 1.6;
}


CScale::~CScale()
{
}

void CScale::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromResID(IDS_MD_ST_FONTNAME));
	pParam->AddParam("size",size,StrFromResID(IDS_MD_ST_FONTSIZE));
	pParam->AddParam("wscale",wscale,StrFromResID(IDS_MD_ST_FONTWSCALE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_ST_TSPACE));
	pParam->AddParam("dy_scaleRuler",dy_scaleRuler,StrFromResID(IDS_MD_SB_TSPACE));
	pParam->AddFontNameParam("scaleLine_font",scaleLine_font,StrFromResID(IDS_SCALELINE_FONT));
	pParam->AddParam("scaleLine_size",scaleLine_size,StrFromResID(IDS_SCALELINE_FONTSIZE));
}

void CScale::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy_scaleRuler",pvar) )
	{
		dy_scaleRuler = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"scaleLine_font",pvar) )
	{
		strncpy(scaleLine_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(scaleLine_font)-1);
	}
	if( tab.GetValue(0,"scaleLine_size",pvar) )
	{
		scaleLine_size = (float)(_variant_t)*pvar;
	}
}

void CScale::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = dy_scaleRuler;
	tab.AddValue("dy_scaleRuler",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));

	var = (LPCTSTR)scaleLine_font;
	tab.AddValue("scaleLine_font",&CVariantEx(var));

	var = scaleLine_size;
	tab.AddValue("scaleLine_size",&CVariantEx(var));
}

void CScale::GetUIParams_data(CUIParam *pParam)
{
}

void CScale::SetData(CValueTable& tab)
{
}

void CScale::GetData(CValueTable& tab)
{
}

void CScale::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	//创建比例尺中的图号文字
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;
	textpt.x = (e.m_xl+e.m_xh)*0.5; 
	textpt.y = e.m_yl-dy*m_pAssist->Unit();
	textpt.z = 0;

	if( 1 )
	{		
		CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
		if( !pFtr )return;

		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();

		CString text;
		text.Format("1:%d",m_pMain->m_scale);

		RotatePts(&textpt,1,xb.cosa,xb.sina);
		
		m_pAssist->SetText(pText,&textpt,1,text,font,size,wscale,TAH_MID|TAV_TOP);
		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	if( 1 )
	{
		PT_3D ptstart;

		ptstart.x = (e.m_xl+e.m_xh)*0.5 - 50 * m_pAssist->Unit(); 
		ptstart.y = e.m_yl-dy_scaleRuler*m_pAssist->Unit();
		ptstart.z = 0;

		PT_3DEX expts[2];
		expts[0].pencode = expts[1].pencode = penLine;

		//底线
		CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
		if( !pFtr )return;
		
		CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
		COPY_3DPT(expts[0],ptstart);
		COPY_3DPT(expts[1],ptstart);

		expts[1].x += 100 * m_pAssist->Unit(); 

		RotatePts(expts,2,xb.cosa,xb.sina);

		pGeo->m_fLineWidth = 0.6;
		pGeo->CreateShape(expts,2);

		m_pAssist->AddObject(pFtr,pLayer->GetID());

		//底线2
		pFtr = m_pAssist->CreateCurve(layer,pLayer);
		if( !pFtr )return;
		
		pGeo = (CGeoCurve*)pFtr->GetGeometry();
		COPY_3DPT(expts[0],ptstart);
		COPY_3DPT(expts[1],ptstart);

		expts[1].x += 100 * m_pAssist->Unit(); 
		
		expts[0].y += 1.7 * m_pAssist->Unit(); 
		expts[1].y += 1.7 * m_pAssist->Unit(); 

		RotatePts(expts,2,xb.cosa,xb.sina);
		
		pGeo->CreateShape(expts,2);
		
		m_pAssist->AddObject(pFtr,pLayer->GetID());

		//刻度线和长度注记
		
		for( int i=0; i<11; i++)
		{
			//刻度线
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();

			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);	

			expts[0].x += i*10 * m_pAssist->Unit(); 
			expts[1].x += i*10 * m_pAssist->Unit(); 
			
			if( (i%2)==0 )
			{
				expts[1].y = ptstart.y + 3.7 * m_pAssist->Unit();
			}
			else
			{
				expts[1].y = ptstart.y + 2.7 * m_pAssist->Unit();
			}

			RotatePts(expts,2,xb.cosa,xb.sina);
			
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());

			//长度注记
			if( (i%2)==0 || i==1 )
			{
				pFtr = m_pAssist->CreateText(layer,pLayer);
				if( !pFtr )continue;
				
				CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
				
				CString text;
				text.Format("%d",10*(int)abs(i-2)*(int)m_pAssist->Unit());
				
				textpt = ptstart;
				textpt.x += i*10 * m_pAssist->Unit(); 

				textpt.y = ptstart.y + 4.2 * m_pAssist->Unit();

				RotatePts(&textpt,1,xb.cosa,xb.sina);
				
				m_pAssist->SetText(pText,&textpt,1,text,scaleLine_font,scaleLine_size,0,TAH_MID|TAV_BOTTOM);
				m_pAssist->SetText(pText,xb.ang*180/PI);
				m_pAssist->AddObject(pFtr,pLayer->GetID());

				textpt = ptstart;
				textpt.x += i*10 * m_pAssist->Unit(); 
				
				textpt.y = ptstart.y + 4.2 * m_pAssist->Unit();

				if( i==0 )
				{
					pFtr = m_pAssist->CreateText(layer,pLayer);
					if( !pFtr )continue;
					
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();					
					
					textpt.x -= 2*m_pAssist->Unit();

					RotatePts(&textpt,1,xb.cosa,xb.sina);
					
					m_pAssist->SetText(pText,&textpt,1,"m",scaleLine_font,scaleLine_size,0,TAH_RIGHT|TAV_BOTTOM);
					m_pAssist->SetText(pText,xb.ang*180/PI);
					m_pAssist->AddObject(pFtr,pLayer->GetID());
				}
				else if( i==10 )
				{
					pFtr = m_pAssist->CreateText(layer,pLayer);
					if( !pFtr )continue;
					
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
					
					textpt.x += 2*m_pAssist->Unit();

					RotatePts(&textpt,1,xb.cosa,xb.sina);
					
					m_pAssist->SetText(pText,&textpt,1,"m",scaleLine_font,scaleLine_size,0,TAH_LEFT|TAV_BOTTOM);
					m_pAssist->SetText(pText,xb.ang*180/PI);
					m_pAssist->AddObject(pFtr,pLayer->GetID());
				}

			}
		}

		//小刻度尺

		for( i=0; i<10; i+=2)
		{
			//粗横线
			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);

			expts[0].y += 0.6 *m_pAssist->Unit();
			expts[1].y += 0.6 *m_pAssist->Unit();

			expts[0].x += i*2*m_pAssist->Unit();
			expts[1].x += (i+1)*2*m_pAssist->Unit();
			
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();
			pGeo->m_fLineWidth = 0.6;

			RotatePts(expts,2,xb.cosa,xb.sina);
						
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());

			//细竖线
			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);
			
			expts[1].y += 1.7*m_pAssist->Unit();
			
			expts[0].x += (i+1)*2*m_pAssist->Unit();
			expts[1].x += (i+1)*2*m_pAssist->Unit();
			
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();
			
			RotatePts(expts,2,xb.cosa,xb.sina);
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());

			if( i!=0 )
			{
				COPY_3DPT(expts[0],ptstart);
				COPY_3DPT(expts[1],ptstart);
				
				expts[1].y += 1.7*m_pAssist->Unit();
				
				expts[0].x += i*2*m_pAssist->Unit();
				expts[1].x += i*2*m_pAssist->Unit();
				
				pFtr = m_pAssist->CreateCurve(layer,pLayer);
				if( !pFtr )continue;
				
				pGeo = (CGeoCurve*)pFtr->GetGeometry();
				
				RotatePts(expts,2,xb.cosa,xb.sina);
				pGeo->CreateShape(expts,2);
				
				m_pAssist->AddObject(pFtr,pLayer->GetID());
			}

		}
	}
}




CRuler::CRuler()
{
	m_strName = StrFromResID(IDS_MD_GRADE);
	strcpy(layer,"TK_ruler");
	dy = 25;
	dx = 140;
	dy2 = 3;

	strcpy(title_font,StrFromResID(IDS_MD_BIANSONGTI));
	title_size = 2.5;
	title_wscale = 1.4;

	strcpy(note_font,StrFromResID(IDS_MD_BIANSONGTI));
	note_size = 2;
	note_wscale = 1.375;

	strcpy(cal_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	cal_size = 1.6;
	cal_wscale = 1;

	x_interv = 2.5;
	con_interv = 2;
}


CRuler::~CRuler()
{
}

void CRuler::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_INNER_LSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BSPACE));
	pParam->AddParam("con_interv",con_interv,StrFromResID(IDS_CONTOUR_INTERVAL));	
}

void CRuler::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"con_interv",pvar) )
	{
		con_interv = (float)(_variant_t)*pvar;
	}
}

void CRuler::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));

	var = con_interv;
	tab.AddValue("con_interv",&CVariantEx(var));
}


void CRuler::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	CFtrLayer *pLayer = NULL;
	
	//横线
	PT_3D linePt[2];

	linePt[0].x = e2.m_xl + dx*m_pAssist->Unit();
	linePt[0].y = e.m_yl - dy*m_pAssist->Unit();
	linePt[0].z = 0;
	linePt[1].x = linePt[0].x + 29*x_interv*m_pAssist->Unit();
	linePt[1].y = linePt[0].y;
	linePt[1].z = 0;

	RotatePts(linePt,2,xb.cosa,xb.sina);
	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

	//刻度
	CArray<PT_3DEX,PT_3DEX> pts_bottom;
	CArray<PT_3DEX,PT_3DEX> pts_top;

	CArray<PT_3DEX,PT_3DEX> pts_top1;
	CArray<PT_3DEX,PT_3DEX> pts_top2;
	CArray<PT_3DEX,PT_3DEX> pts_top3;
	CArray<PT_3DEX,PT_3DEX> pts_top4;

	int i = 0;
	for(i = 1; i <= 30; i++)
	{
		PT_3D pts[2];
		pts[0].x = e2.m_xl + (dx+(i-1)*x_interv)*m_pAssist->Unit();
		pts[0].y = e.m_yl - dy*m_pAssist->Unit();		
		pts[0].z = 0;
		
		PT_3DEX tpt0;
		COPY_3DPT(tpt0,pts[0]);
		pts_bottom.Add( tpt0 );


		float ctgx = 1 / ( tan( i*PI/180 ) ); 
		pts[1].x = pts[0].x;
		pts[1].y = pts[0].y + ctgx*con_interv;
		pts[1].z = 0;

		PT_3DEX tpt;
		tpt.pencode = penSpline;
		COPY_3DPT(tpt,pts[1]);
		pts_top.Add( tpt );

		if( i >= 10 )
		{
			PT_3DEX tpt1 = tpt;
			tpt1.y = tpt.y + ctgx*con_interv;
			pts_top1.Add( tpt1 );

			PT_3DEX tpt2 = tpt;
			tpt2.y = tpt.y + 2*ctgx*con_interv;
			pts_top2.Add( tpt2 );

			PT_3DEX tpt3 = tpt;
			tpt3.y = tpt.y + 3*ctgx*con_interv;
			pts_top3.Add( tpt3 );

			PT_3DEX tpt4 = tpt;
			tpt4.y = tpt.y + 4*ctgx*con_interv;
			pts_top4.Add( tpt4 );
		}

		if( i == 1 || (i % 2 == 0) )
		{
			PT_3D textpt;
			textpt.x = pts[0].x + 0.5*m_pAssist->Unit();
			textpt.y = pts[0].y - 0.3*m_pAssist->Unit();
			textpt.z = 0;
			
			RotatePts(&textpt,1,xb.cosa,xb.sina);
			
			CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
			CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
			if( !pText )return;

			CString cal_text;
			cal_text.Format("%d",i);
			cal_text += StrFromResID(IDS_MD_DEGREE);
			m_pAssist->SetText(pText,&textpt,1,cal_text,cal_font,cal_size,cal_wscale,TAH_MID|TAV_TOP);
			m_pAssist->SetText(pText,xb.ang*180/PI);
			m_pAssist->AddObject(pFtr,pLayer->GetID());
		}
	}

	//竖线
	for(i = 0; i < 30; i++)
	{
		PT_3DEX pts[2];
		pts[0] = pts_bottom.GetAt(i);
		if( i < 9 )
			pts[1] = pts_top.GetAt(i);
		else
			pts[1] = pts_top4.GetAt(i-9);

		RotatePts(pts,2,xb.cosa,xb.sina);
		m_pAssist->AddLine(layer,pts[0],pts[1],0);
	}

	//曲线（相邻两条等高线）
	CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;

	RotatePts(pts_top.GetData() ,pts_top.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top.GetData(), pts_top.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	//曲线（相邻六条等高线）
	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top1.GetData() ,pts_top1.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top1.GetData(), pts_top1.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top2.GetData() ,pts_top2.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top2.GetData(), pts_top2.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top3.GetData() ,pts_top3.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top3.GetData(), pts_top3.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top4.GetData() ,pts_top4.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top4.GetData(), pts_top4.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	//标题
	PT_3DEX textpt;
	textpt.x = e2.m_xl + (dx+0.5*29*x_interv)*m_pAssist->Unit();
	textpt.y = e.m_yl - dy2*m_pAssist->Unit();
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	char title_text[64];
	strcpy(title_text,StrFromResID(IDS_MD_GRADE));
	m_pAssist->SetText(pText,&textpt,1,title_text,title_font,title_size,title_wscale,TAH_MID|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	//注释
	textpt = pts_top.GetAt(1);
	textpt.y += 0.3*m_pAssist->Unit();	
//	RotatePts(&textpt,1,xb.cosa,xb.sina);	//pts_top中的所有坐标之前已经旋转过了	
	pFtr = m_pAssist->CreateText(layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	char note_text[64];
	strcpy(note_text,StrFromResID(IDS_MD_TWO_CONTOUR_NEAR));
	m_pAssist->SetText(pText,&textpt,1,note_text,note_font,note_size,note_wscale,TAH_LEFT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	textpt = pts_top4.GetAt(4);
	textpt.y += 0.3*m_pAssist->Unit();
	pFtr = m_pAssist->CreateText(layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;	

	strcpy(note_text,StrFromResID(IDS_MD_SIX_CONTOUR_NEAR));
	m_pAssist->SetText(pText,&textpt,1,note_text,note_font,note_size,note_wscale,TAH_LEFT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}




CNorthPointer::CNorthPointer()
{
	m_bUsed  = FALSE;

	m_strName = StrFromResID(IDS_MD_NORTHPOINTER);
	strcpy(layer,"TK_north_pointer");
	dx = 600;
	dy = 20;
}


CNorthPointer::~CNorthPointer()
{
}

void CNorthPointer::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_TSPACE));			
	
}

void CNorthPointer::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CNorthPointer::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}


void CNorthPointer::MakeFtrs()
{
	
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	CString path = GetMyConfigPath(m_pMain->m_scale);
	
	CString filePath = path;
	filePath.Format("NorthPointer%d.xml",m_pMain->m_scale);
	filePath = path+filePath;
	
	PT_3D pt = PT_3D(e.m_xl+dx*m_pAssist->Unit(),e.m_yl-dy*m_pAssist->Unit(),0);
	
	RotatePts(&pt,1,xb.cosa,xb.sina);
	
	m_pMain->ImportXML(filePath,m_pAssist->GetLayer(layer),
		pt,
		pt,xb.ang);
	
}




CMagline::CMagline()
{
	m_bUsed  = FALSE;

	m_strName = StrFromResID(IDS_MD_MAGNETIC_LINE);
	strcpy(layer,"TK_magline");

	magline = 0;
}


CMagline::~CMagline()
{
}

void CMagline::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	
}

void CMagline::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
}

void CMagline::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
}

void CMagline::GetUIParams_data(CUIParam *params)
{
	params->AddParam("angle",angle,StrFromResID(IDS_MD_LSPACE));
}

void CMagline::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"angle",pvar) )
	{
		angle = (float)(_variant_t)*pvar;
	}
}

void CMagline::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = angle;
	tab.AddValue("angle",&CVariantEx(var));
}

void CMagline::MakeFtrs()
{
}



CMakerLeft::CMakerLeft()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_MAKER_LEFT);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_publisher");
	strcpy(makerLeft,StrFromResID(IDS_MD_MAKER_LEFT));
	size = 4;
	wscale = 1;
	char_interv = 0;
	line_interv = 1;
	dx = 3;

	if( (25000 == scale || 50000 == scale || 100000 == scale) && !MapDecorate::bMapDecorateNew )
	{
		strcpy(font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
		dx = 5;
	}
}


CMakerLeft::~CMakerLeft()
{
}

void CMakerLeft::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("char_interv",char_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARINTVS));
	pParam->AddParam("line_interv",line_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_LINEINTVS));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_RSPACE));

}

void CMakerLeft::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv",pvar) )
	{
		char_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"line_interv",pvar) )
	{
		line_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
}

void CMakerLeft::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = char_interv;
	tab.AddValue("char_interv",&CVariantEx(var));

	var = line_interv;
	tab.AddValue("line_interv",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
}

void CMakerLeft::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddParam("makerLeft",makerLeft,StrFromResID(IDS_MD_MAKER_LEFT));
}

void CMakerLeft::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"makerLeft",pvar) )
	{
		strncpy(makerLeft,(LPCTSTR)(_bstr_t)*pvar,sizeof(makerLeft)-1);
	}
}

void CMakerLeft::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)makerLeft;
	tab.AddValue("makerLeft",&CVariantEx(var));
}

void CMakerLeft::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	PT_3D tpt;
	tpt.x = m_pMain->m_outerBound[0].x + m_pMain->m_outerBound[3].y-m_pMain->m_outerBound[0].y;
	tpt.y = m_pMain->m_outerBound[0].y - (m_pMain->m_outerBound[3].x-m_pMain->m_outerBound[0].x);
	
	//旋转使得 m_pMain->m_outerBound[0], m_pMain->m_outerBound[3] 平行于y轴
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],tpt,xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt[2];
	textpt[0].x = e.m_xl-(dx+size)*m_pAssist->Unit(); 
	textpt[0].y = e2.m_yl+size*m_pAssist->Unit()*strlen(makerLeft)*10;
	textpt[0].z = 0;
	
	textpt[1].x = e.m_xl-dx-size*m_pAssist->Unit(); 
	textpt[1].y = e2.m_yl;
	textpt[1].z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	char makertext[256];
	strcpy(makertext,makerLeft);
	InsertReturns(makertext);
	
	m_pAssist->SetText(pText,textpt,2,makertext,font,size,wscale,TAH_RIGHT|TAV_BOTTOM);
	
	TEXT_SETTINGS0 ts;
	pText->GetSettings(&ts);
	
	ts.nPlaceType = byLineV;
	ts.fCharIntervalScale = char_interv;
	ts.fLineSpacingScale = line_interv;
	pText->SetSettings(&ts);
	
	Envelope e3 = m_pAssist->GetTextEnvelope(pText);
	PT_3DEX pts[2];
	pts[0].x = e.m_xl-dx*m_pAssist->Unit(); 
	pts[0].y = e2.m_yl+e3.Height();
	pts[0].z = 0;
	pts[0].pencode = penLine;
	
	pts[1].x = e.m_xl-dx*m_pAssist->Unit(); 
	pts[1].y = e2.m_yl;
	pts[1].z = 0;
	pts[1].pencode = penLine;
	
	RotatePts(pts,2,xb.cosa,xb.sina);
	
	pText->CreateShape(pts,2);
	
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CMakerRight::CMakerRight()
{
	m_strName = StrFromResID(IDS_MD_MAKER_RIGHT);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_maker");
	strcpy(makerRight,StrFromResID(IDS_MD_MAKER_RIGHT));
	size = 3;
	wscale = 1;
	char_interv = 0;
	line_interv = 1;
	dx = 3.5;
}


CMakerRight::~CMakerRight()
{
}

void CMakerRight::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("char_interv",char_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARINTVS));
	pParam->AddParam("line_interv",line_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_LINEINTVS));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));

}

void CMakerRight::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv",pvar) )
	{
		char_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"line_interv",pvar) )
	{
		line_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
}

void CMakerRight::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = char_interv;
	tab.AddValue("char_interv",&CVariantEx(var));

	var = line_interv;
	tab.AddValue("line_interv",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
}

void CMakerRight::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddParam("makerRight",makerRight,StrFromResID(IDS_MD_MAKER_RIGHT));
}

void CMakerRight::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"makerRight",pvar) )
	{
		strncpy(makerRight,(LPCTSTR)(_bstr_t)*pvar,sizeof(makerRight)-1);
	}
}

void CMakerRight::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)makerRight;
	tab.AddValue("makerRight",&CVariantEx(var));
}

void CMakerRight::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	PT_3D tpt;
	tpt.x = m_pMain->m_outerBound[1].x + m_pMain->m_outerBound[2].y-m_pMain->m_outerBound[1].y;
	tpt.y = m_pMain->m_outerBound[1].y - (m_pMain->m_outerBound[2].x-m_pMain->m_outerBound[1].x);
	
	//旋转使得 m_pMain->m_outerBound[1], m_pMain->m_outerBound[2] 平行于y轴
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[1],tpt,xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt[2];
	textpt[0].x = e.m_xh+dx*m_pAssist->Unit(); 
	textpt[0].y = e2.m_yl+size*m_pAssist->Unit()*strlen(makerRight)*10;
	textpt[0].z = 0;
	
	textpt[1].x = e.m_xh+dx*m_pAssist->Unit(); 
	textpt[1].y = e2.m_yl;
	textpt[1].z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	char makertext[256];
	strcpy(makertext,makerRight);
	InsertReturns(makertext);
	
	m_pAssist->SetText(pText,textpt,2,makertext,font,size,wscale,TAH_LEFT|TAV_BOTTOM);
	
	TEXT_SETTINGS0 ts;
	pText->GetSettings(&ts);
	
	ts.nPlaceType = byLineV;
	ts.fCharIntervalScale = char_interv;
	ts.fLineSpacingScale = line_interv;
	pText->SetSettings(&ts);
	
	Envelope e3 = m_pAssist->GetTextEnvelope(pText);
	PT_3DEX pts[2];
	pts[0].x = e.m_xh+dx*m_pAssist->Unit(); 
	pts[0].y = e2.m_yl+e3.Height();
	pts[0].z = 0;
	pts[0].pencode = penLine;
	
	pts[1].x = e.m_xh+dx*m_pAssist->Unit(); 
	pts[1].y = e2.m_yl;
	pts[1].z = 0;
	pts[1].pencode = penLine;
	
	RotatePts(pts,2,xb.cosa,xb.sina);
	
	pText->CreateShape(pts,2);
	
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}




CDataDesc::CDataDesc()
{
	m_strName = StrFromResID(IDS_MD_DATADESC);
	strcpy(font,StrFromResID(IDS_MD_BIANSONGTI));
	strcpy(layer,"TK_data_desc");
	strcpy(data_desc,StrFromResID(IDS_MD_DATADESC_CONTENT));
	size = 3;
	wscale = 1;
	char_interv = 0;
	line_interv = 1;
	dy = 3;
}


CDataDesc::~CDataDesc()
{
}

void CDataDesc::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("char_interv",char_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARINTVS));
	pParam->AddParam("line_interv",line_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_LINEINTVS));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_TSPACE));

}

void CDataDesc::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv",pvar) )
	{
		char_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"line_interv",pvar) )
	{
		line_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CDataDesc::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = char_interv;
	tab.AddValue("char_interv",&CVariantEx(var));
	
	var = line_interv;
	tab.AddValue("line_interv",&CVariantEx(var));

	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CDataDesc::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddMultiEditParam("data_desc",data_desc,StrFromResID(IDS_MD_DATADESC));
}

void CDataDesc::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"data_desc",pvar) )
	{
		strncpy(data_desc,(LPCTSTR)(_bstr_t)*pvar,sizeof(data_desc)-1);
	}
}

void CDataDesc::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)data_desc;
	tab.AddValue("data_desc",&CVariantEx(var));
}

void CDataDesc::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;
	textpt.x = xb.innerBound[0].x; 
	textpt.y = e.m_yl-dy*m_pAssist->Unit();
	textpt.z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	if( !pFtr )
		return;
	
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	
	m_pAssist->SetText(pText,&textpt,1,data_desc,font,size,wscale,TAH_LEFT|TAV_TOP);
	
	TEXT_SETTINGS0 ts;
	pText->GetSettings(&ts);
	
	ts.fCharIntervalScale = char_interv;
	ts.fLineSpacingScale = 0;
	ts.fTextAngle = 0;
	pText->SetSettings(&ts);
	
	CFtrArray arrFtrs;
	
	if( m_pAssist->SplitMultipleLineText(pFtr,line_interv*m_pAssist->Unit(),arrFtrs) )
	{
		for( int i=0; i<arrFtrs.GetSize(); i++)
		{
			CGeoText *pText = (CGeoText*)arrFtrs[i]->GetGeometry();
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pText->GetShape(arrPts);
			
			RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
			pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
			
			m_pAssist->SetText(pText,xb.ang*180/PI);
			
			m_pAssist->AddObject(arrFtrs[i],pLayer->GetID());
		}
		delete pFtr;
	}
	else
	{
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pText->GetShape(arrPts);
		
		RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
		pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		m_pAssist->SetText(pText,xb.ang*180/PI);
		
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}
}




CNote::CNote()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_NOTE);
	strcpy(font,StrFromResID(IDS_MD_BIANSONGTI));
	strcpy(layer,"TK_note");
	strcpy(note,StrFromResID(IDS_MD_NOTE123));
	size = 3;
	wscale = 1;
	char_interv = 0;
	line_interv = 1;
	dx = 90;
	dy = 4;

	if( 5000 == scale || 10000 == scale )
	{
		dx = 0;
		size = 2;
		wscale = 1.375;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if(MapDecorate::bMapDecorateNew)
		{
			dx = 0;
			size = 2;
			wscale = 1.375;
		}
		else
		{
			dx = 0;
			dy = 3;
		}
	}

}


CNote::~CNote()
{
}

void CNote::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("char_interv",char_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARINTVS));
	pParam->AddParam("line_interv",line_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_LINEINTVS));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE2));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_TSPACE));
}

void CNote::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv",pvar) )
	{
		char_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"line_interv",pvar) )
	{
		line_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CNote::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = char_interv;
	tab.AddValue("char_interv",&CVariantEx(var));

	var = line_interv;
	tab.AddValue("line_interv",&CVariantEx(var));

	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CNote::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddMultiEditParam("note",note,StrFromResID(IDS_MD_NOTE));
}

void CNote::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"note",pvar) )
	{
		strncpy(note,(LPCTSTR)(_bstr_t)*pvar,sizeof(note)-1);
	}
}

void CNote::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)note;
	tab.AddValue("note",&CVariantEx(var));
}

void CNote::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;
	textpt.x = e2.m_xh-dx*m_pAssist->Unit();
	textpt.y = e.m_yl-dy*m_pAssist->Unit();
	textpt.z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	if( !pFtr )
		return;
	
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	
	m_pAssist->SetText(pText,&textpt,1,note,font,size,wscale,TAH_LEFT|TAV_TOP);
	
	TEXT_SETTINGS0 ts;
	pText->GetSettings(&ts);
	
	ts.fCharIntervalScale = char_interv;
	ts.fLineSpacingScale = line_interv;
	pText->SetSettings(&ts);
	
	CFtrArray arrFtrs;
	
	if( m_pAssist->SplitMultipleLineText(pFtr,line_interv*m_pAssist->Unit(),arrFtrs) )
	{
		for( int i=0; i<arrFtrs.GetSize(); i++)
		{
			CGeoText *pText = (CGeoText*)arrFtrs[i]->GetGeometry();
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pText->GetShape(arrPts);
			
			RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
			pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
			
			m_pAssist->SetText(pText,xb.ang*180/PI);
			
			m_pAssist->AddObject(arrFtrs[i],pLayer->GetID());
		}
		delete pFtr;
	}
	else
	{
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pText->GetShape(arrPts);
		
		RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
		pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		m_pAssist->SetText(pText,xb.ang*180/PI);
		
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}
}



CCheckman::CCheckman()
{
	m_strName = StrFromResID(IDS_MD_CHECKER);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_checkman");
	strcpy(checkman,StrFromResID(IDS_MD_CHECKER));
	size = 2;
	wscale = 1;
	char_interv = 0;
	line_interv = 1;
	dx = 30;
	dy = 3;

	strcpy(surveyman,"A");
	strcpy(drawman,"B");
	strcpy(checkman,"C");
}


CCheckman::~CCheckman()
{
}

void CCheckman::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("char_interv",char_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARINTVS));
	pParam->AddParam("line_interv",line_interv,StrFromLocalResID(IDS_FIELDNAME_TEXT_LINEINTVS));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_TSPACE));
}

void CCheckman::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv",pvar) )
	{
		char_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"line_interv",pvar) )
	{
		line_interv = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CCheckman::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = char_interv;
	tab.AddValue("char_interv",&CVariantEx(var));

	var = line_interv;
	tab.AddValue("line_interv",&CVariantEx(var));

	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CCheckman::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddParam("surveyman",surveyman,StrFromResID(IDS_MD_MEASURER));
	pParam->AddParam("drawman",drawman,StrFromResID(IDS_MD_PLOTER));
	pParam->AddParam("checkman",checkman,StrFromResID(IDS_MD_CHECKER));
}

void CCheckman::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"surveyman",pvar) )
	{
		strncpy(surveyman,(LPCTSTR)(_bstr_t)*pvar,sizeof(surveyman)-1);
	}
	if( tab.GetValue(0,"drawman",pvar) )
	{
		strncpy(drawman,(LPCTSTR)(_bstr_t)*pvar,sizeof(drawman)-1);
	}
	if( tab.GetValue(0,"checkman",pvar) )
	{
		strncpy(checkman,(LPCTSTR)(_bstr_t)*pvar,sizeof(checkman)-1);
	}
}

void CCheckman::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)surveyman;
	tab.AddValue("surveyman",&CVariantEx(var));

	var = (LPCTSTR)drawman;
	tab.AddValue("drawman",&CVariantEx(var));

	var = (LPCTSTR)checkman;
	tab.AddValue("checkman",&CVariantEx(var));
}

void CCheckman::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;
	textpt.x = e2.m_xh-dx*m_pAssist->Unit();
	textpt.y = e.m_yl-dy*m_pAssist->Unit();
	textpt.z = 0;
	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	if( !pFtr )return;
	
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	
	CString text = StrFromResID(IDS_MD_MEASURER);
	text += ": "; text += surveyman; text += "\n";
	text += StrFromResID(IDS_MD_PLOTER);
	text += ": "; text += drawman; text += "\n";
	text += StrFromResID(IDS_MD_CHECKER);
	text += ": "; text += checkman;
	
	m_pAssist->SetText(pText,&textpt,1,text,font,size,wscale,TAH_LEFT|TAV_TOP);
	
	TEXT_SETTINGS0 ts;
	pText->GetSettings(&ts);
	
	ts.fCharIntervalScale = char_interv;
	ts.fLineSpacingScale = line_interv;
	pText->SetSettings(&ts);
	
	CFtrArray arrFtrs;
	
	if( m_pAssist->SplitMultipleLineText(pFtr,line_interv*m_pAssist->Unit(),arrFtrs) )
	{
		for( int i=0; i<arrFtrs.GetSize(); i++)
		{
			CGeoText *pText = (CGeoText*)arrFtrs[i]->GetGeometry();
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			arrFtrs[i]->GetGeometry()->GetShape(arrPts);
			
			RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
			arrFtrs[i]->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
			
			m_pAssist->SetText(pText,xb.ang*180/PI);
			
			m_pAssist->AddObject(arrFtrs[i],pLayer->GetID());
		}
		delete pFtr;
	}
	else
	{
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pFtr->GetGeometry()->GetShape(arrPts);
		
		RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
		pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		m_pAssist->SetText(pText,xb.ang*180/PI);
		
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMapDecoratorAssist::CMapDecoratorAssist()
{
	m_pDoc = NULL;
	m_pDS = NULL;
	m_scale = 2000;

	m_pArrNewObjHandles = NULL;
}


CMapDecoratorAssist::~CMapDecoratorAssist()
{

}


void CMapDecoratorAssist::SetDoc(CDlgDoc *pDoc, int scale)
{
	m_pDoc = pDoc;
	m_scale = scale;
}

void CMapDecoratorAssist::SetDlgDataSource(CDlgDataSource *pDS, int scale)
{
	m_pDS = pDS;
	m_scale = scale;
}

CDlgDataSource *CMapDecoratorAssist::GetDlgDataSource()
{
	if( m_pDoc )
		return m_pDoc->GetDlgDataSource();
	return m_pDS;
}

BOOL CMapDecoratorAssist::AddObject(CFeature *pFt, int layid)
{
	BOOL bRet = FALSE;
	if( m_pDoc )
		bRet = m_pDoc->AddObject(pFt,layid);
	else
		bRet = m_pDS->AddObject(pFt,layid);

	if( bRet && m_pArrNewObjHandles )
	{
		m_pArrNewObjHandles->Add(FtrToHandle(pFt));
	}
	return bRet;
}

CFtrLayer *CMapDecoratorAssist::GetLayer(LPCTSTR fid)
{
	CFtrLayer *pLayer = NULL;
	if( fid!=NULL )
		pLayer = GetDlgDataSource()->GetFtrLayer(fid);
	
	if( pLayer==NULL )
	{
		pLayer = new CFtrLayer();
		pLayer->SetName(fid);
		
		if( m_pDoc )
			m_pDoc->AddFtrLayer(pLayer);
		else
			m_pDS->AddFtrLayer(pLayer);
	}
	
	return pLayer;
}


CFeature *CMapDecoratorAssist::CreateCurve(LPCTSTR fid, CFtrLayer *&pLayer)
{
	pLayer = GetLayer(fid);
	
	if( pLayer==NULL )
		return NULL;
	
	CFeature *pObj = pLayer->CreateDefaultFeature(m_scale,CLS_GEOCURVE);
	
	pObj->GetGeometry()->SetColor(RGB(255,255,255));
	return pObj;
}

CFeature *CMapDecoratorAssist::CreatePoint(LPCTSTR fid, CFtrLayer *&pLayer)
{
	pLayer = GetLayer(fid);	
	if( pLayer==NULL )
		return NULL;
	
	CFeature *pObj = pLayer->CreateDefaultFeature(m_scale,CLS_GEOPOINT);
	
	pObj->GetGeometry()->SetColor(RGB(255,255,255));
	return pObj;
}


CFeature *CMapDecoratorAssist::CreateText(LPCTSTR fid, CFtrLayer *&pLayer)
{
	pLayer = GetLayer(fid);
	if( pLayer==NULL )
		return NULL;
	
	CFeature *pObj = pLayer->CreateDefaultFeature(m_scale,CLS_GEOTEXT);
	
	pObj->GetGeometry()->SetColor(RGB(255,255,255));
	return pObj;
}


void CMapDecoratorAssist::CreateCross(CGeoCurve *pGeo, PT_3D pt, int dir, float len)
{	
	PT_3DEX expt;
	expt.pencode = penLine;
	
	PT_3DEX pts[2];
	
	//横线
	if( dir==0 )
	{
		expt.x = pt.x - len; expt.y = pt.y; expt.z = pt.z;
		pts[0] = expt;
		expt.x = pt.x + len;
		pts[1] = expt;
		pGeo->CreateShape(pts,2);
	}
	//竖线
	else
	{		
		expt.x = pt.x; expt.y = pt.y-len; expt.z = pt.z;
		pts[0] = expt;
		expt.y = pt.y + len;
		pts[1] = expt;
		pGeo->CreateShape(pts,2);
	}
	
}


void CMapDecoratorAssist::SetText(CGeoText *pObj, PT_3D* pt3ds, int npt, LPCTSTR text, LPCTSTR font, float size, float wscale, int nAlign)
{
	PT_3DEX *pts = new PT_3DEX[npt];
	
	for( int i=0; i<npt; i++)
	{
		COPY_3DPT(pts[i],pt3ds[i]);
		
		pts[i].pencode = penLine;
	}	
	
	pObj->CreateShape(pts,npt);
	
	delete[] pts;
	
	TEXT_SETTINGS0 ts;
	pObj->GetSettings(&ts);
	strcpy(ts.strFontName,font);
	ts.fHeight = size;
	ts.nAlignment = nAlign;
	
	TextStyle style = GetUsedTextStyles()->GetTextStyleByName(font);
	if( style.IsValid() )
	{
		ts.fWidScale = style.fWidScale;
		ts.nInclineType = style.nInclineType;
		ts.fInclineAngle = style.fInclinedAngle;

		ts.SetBold(style.bBold);
	}
	
	if( wscale>1e-4 )
		ts.fWidScale = wscale;

	pObj->SetSettings(&ts);
	pObj->SetText(text);
}


void CMapDecoratorAssist::SetText(CGeoText *pObj, double lfTextAngle)
{
	TEXT_SETTINGS0 ts;
	pObj->GetSettings(&ts);

	ts.fTextAngle = lfTextAngle;
	
	pObj->SetSettings(&ts);
}


CGeoText* CMapDecoratorAssist::AddText(LPCTSTR fid, PT_3D pt, LPCTSTR text, LPCTSTR font, float size, float wscale, int nAlign, double ang)
{
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = CreateText(fid,pLayer);
	if( !pFtr )
		return NULL;
	
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	
	SetText(pText,&pt,1,text,font,size,wscale,nAlign);
	SetText(pText,ang);
	AddObject(pFtr,pLayer->GetID());
	
	return pText;
}


CGeoCurve* CMapDecoratorAssist::AddLine(LPCTSTR fid, PT_3D pt0, PT_3D pt1, float wid)
{
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = CreateCurve(fid,pLayer);
	if( !pFtr )
		return NULL;
	
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	
	PT_3DEX pts[2];
	pts[0].pencode = penLine;
	COPY_3DPT(pts[0],pt0);
	
	pts[1].pencode = penLine;
	COPY_3DPT(pts[1],pt1);
	
	pGeo->CreateShape(pts,2);
	
	AddObject(pFtr,pLayer->GetID());
	
	return pGeo;
}


Envelope CMapDecoratorAssist::GetTextEnvelope(CGeoText *pText)
{
	GrBuffer buf;

	if( m_pDoc )
		pText->Draw(&buf,m_pDoc->GetDlgDataSource()->GetSymbolDrawScale());
	else
		pText->Draw(&buf,m_pDS->GetSymbolDrawScale());	
	
	return buf.GetEnvelope();
}


BOOL CMapDecoratorAssist::SplitMultipleLineText(CFeature *pObj, double interval, CFtrArray& arrFtrs)
{
	CGeometry *pGeo = pObj->GetGeometry();
	if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		return FALSE;
	}
	
	CGeoText *pText = (CGeoText*)pGeo;
	CString text = pText->GetText();
	CString left, right;
	
	TEXT_SETTINGS0 set;
	pText->GetSettings(&set);
	
	int pos = text.Find('\n');
	if( pos<=0 )
		return FALSE;
	
	double dy = 0;
	
	text += '\n';
	
	while( pos>=0 )
	{
		left = text.Left(pos);
		right = text.Mid(pos+1);
		
		if( left.GetLength()>0 )
		{
			CFeature *pFtr = pObj->Clone();
			if( pFtr )
			{
				CGeoText *pText2 = (CGeoText*)pFtr->GetGeometry();
				pText2->SetText(left);
				
				double m[16];
				Matrix44FromMove(0,-dy,0,m);
				pText2->Transform(m);
				
				arrFtrs.Add(pFtr);
				
				Envelope e = GetTextEnvelope(pText2);
				dy += e.Height();
			}
		}
		
		dy += interval;
		text = right;
		pos = text.Find('\n');
	}
	
	return TRUE;
}


double CMapDecoratorAssist::Unit()
{
	return m_scale*0.001;
}



void CMapDecoratorAssist::CreateHachure(LPCTSTR layname, PT_3D *pts, int npt, double ang, double interval, CFtrLayer*& pLayer, CFtrArray& arrFtrs)
{
	//以ang方向为X轴、垂直于ang方向为Y轴，建立新坐标系；该坐标系中Y向坐标最大的点定义为填充起点
	// 1) 建立新坐标系(旋转-ang角)
	PT_3D *pts2 = new PT_3D[npt];
	
	double cosa = cos(ang), sina = sin(ang);
	for( int i=0; i<npt; i++)
	{
		pts2[i].x = pts[i].x*cosa + pts[i].y*sina;
		pts2[i].y = -pts[i].x*sina + pts[i].y*cosa;
		pts2[i].z = pts[i].z;
	}
	
	// 2)计算Y向坐标最大值
	Envelope e;
	e.CreateFromPts(pts2,npt);
	
	// 3)生成扫描线
	CGrTrim trim;
	trim.InitTrimPolygon(pts2,npt,interval);
	
	double y0 = e.m_yh;
	PT_3D line[2], line2[2];
	PT_3DEX expts[2];
	
	line[0].x = e.m_xl-1; line[1].x = e.m_xh+1;
	expts[0].pencode = expts[1].pencode = penLine;
	
	for( ; y0>e.m_yl; y0-=interval )
	{
		line[0].y = line[1].y = y0;
		
		trim.TrimLine(line,line+1);
		
		if( trim.GetTrimedLinesNum()<=0 )
			continue;
		
		trim.GetTrimedLine(0,line2,line2+1);
		
		CFeature *pFtr = CreateCurve(layname,pLayer);
		if( !pFtr )continue;
		
		CGeometry *pGeo = pFtr->GetGeometry();
		
		//将坐标转换回来
		expts[0].x = line2[0].x*cosa - line2[0].y*sina;
		expts[0].y = line2[0].x*sina + line2[0].y*cosa;
		expts[0].z = line2[0].z;
		
		expts[1].x = line2[1].x*cosa - line2[1].y*sina;
		expts[1].y = line2[1].x*sina + line2[1].y*cosa;
		expts[1].z = line2[1].z;
		
		pGeo->CreateShape(expts,2);
		
		arrFtrs.Add(pFtr);
	}		
	delete[] pts2;
}


static BOOL ImportXMLtoDS(LPCTSTR path, CDlgDataSource *pDS, CFtrLayer *pLayer, PT_3D move_pt, PT_3D rotate_pt, double angle, CFtrArray *pArr)
{
	CString strPathName = path;
	
	if( !pLayer )
		return FALSE;
	
	{
		CXmlAccess xml;
		BOOL bOpen = xml.OpenRead(strPathName);
		if( !bOpen )
		{
			CString temp;
			temp.Format(IDS_REFDATA_NOT_EXIST,path);
			//AfxMessageBox(temp);
			return FALSE;
		}
		
		// 记录当前层，导完后得置
		CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
		if( pLayer )pDS->SetCurFtrLayer(pLayer->GetID());
		
		// 加载
		xml.BatchUpdateBegin();
		
		long idx;
		int lSum = xml.GetFeatureCount();
		
		GOutPut(StrFromResID(IDS_LOAD_FEATURES));
		GProgressStart(lSum);			
		
		CFeature *pFt = xml.GetFirstFeature(idx);

		long ilay = 0;
		CFtrLayer *pl = xml.GetFirstFtrLayer(ilay);
		CArray<CFtrLayer*,CFtrLayer*> arrPLayers;
		while( pl )
		{
			arrPLayers.Add(pl);
			pl = xml.GetNextFtrLayer(ilay);
		}
		
		double m1[16], m2[16], m3[16];
		Matrix44FromMove(move_pt.x,move_pt.y,move_pt.z,m1);
		if( angle!=0.0 )
			Matrix44FromRotate(&rotate_pt,angle,m2);
		else
			matrix_toIdentity(m2,4);
		
		matrix_multiply(m2,m1,4,m3);
		
		while( pFt )
		{
			GProgressStep();
			
			pFt->GetGeometry()->Transform(m3);

			if( pLayer )
			{
				int layID = xml.GetCurFtrLayID();
				for( int i=0; i<arrPLayers.GetSize(); i++)
				{
					if( layID==arrPLayers[i]->GetID() )
						break;
				}
				if( i<arrPLayers.GetSize() )
				{
					pFt->GetGeometry()->SetSymbolName(arrPLayers[i]->GetName());
					if( pFt->GetGeometry()->GetColor()==COLOUR_BYLAYER )
					{
						pFt->GetGeometry()->SetColor(arrPLayers[i]->GetColor());
					}
				}
			}
			
			pDS->AddObject(pFt);

			if( pArr )
			{
				pArr->Add(pFt);
			}
			
			pFt = xml.GetNextFeature(idx);
			
		}		
		
		GProgressEnd();
		GOutPut(StrFromResID(IDS_LOAD_FINISH));

		for( int i=0; i<arrPLayers.GetSize(); i++)
		{
			delete arrPLayers[i];
		}
		
		arrPLayers.RemoveAll();
		
		xml.BatchUpdateEnd();
		xml.Close();
		
		// 重置当前层
		if (pCurLayer)
		{
			pDS->SetCurFtrLayer(pCurLayer->GetID());
		}
	}
	
	return TRUE;	
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapDecorator::CMapDecorator()
{
	m_pDoc = NULL;
	m_pDS = NULL;
	m_scale = 2000;
	memset(m_innerBound,0,sizeof(m_innerBound));
	memset(m_outerBound,0,sizeof(m_outerBound));

	m_bSilence = FALSE;
	m_bSortBound = TRUE;

	m_nMapType = TK_MAPTYPE_RECT5050;
	m_nMapBorderType = TK_MAPBORDER_4PT;
	
	m_fMapWid = 50; 
	m_fMapWid = 50;

	m_bCutStripNum = FALSE;

	m_bNew = FALSE;
}

CMapDecorator::~CMapDecorator()
{
	Clear();
}

void CMapDecorator::CreateObjs(long scale)
{
	m_arrPObjs.RemoveAll();

	CreateObj("Name");
	CreateObj("Number");
	CreateObj("AreaDesc");
	CreateObj("Secret");
	CreateObj("OuterBound");
	CreateObj("InnerBound");
//	CreateObj("Grid");
	CreateObj("LTname");
	CreateObj("LBname");
	CreateObj("RTname");
	CreateObj("RBname");
	CreateObj("Figure");	
	CreateObj("Note");

	if( scale < 5000 )
	{
		CreateObj("Grid");
		CreateObj("Scale");
		CreateObj("Magline");
		CreateObj("NearMap");
		CreateObj("Checkman");
		CreateObj("DataDesc");
		CreateObj("MakerLeft");
		CreateObj("MakerRight");		
	}
	else if( 5000 == scale || 10000 == scale )
	{
		CreateObj("Grid");
		CreateObj("DigitalScale");
		CreateObj("AdminRegion_NearMap");
		CreateObj("MakerBottom");
		CreateObj("Ruler");	
		CreateObj("Scale_5KN");	
		CreateObj("DataDesc_5KN");						
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if( TRUE == m_bNew)
		{
			CreateObj("Grid_New");
			CreateObj("OuterBoundDecorator");
			CreateObj("DigitalScale");
			CreateObj("AdminRegion_NearMap");
			CreateObj("MakerBottom");
			CreateObj("Ruler_25KN");	
			CreateObj("Scale_25KN");			
			CreateObj("NorthPointer_25KN");
			CreateObj("DataDesc_5KN");			
			CreateObj("Magline_25KN");
		}
		else
		{
			//旧版
			CreateObj("Grid_New");
			CreateObj("NearMap");
			CreateObj("OuterBoundDecorator");
			CreateObj("MakerLeft");	
			CreateObj("DataDesc");
			CreateObj("Ruler_25KN");	
			CreateObj("Scale_25KN");
			CreateObj("NorthPointer_25KN");
			CreateObj("AdminRegion");
			CreateObj("Magline_25KN");
		}
	}
}


void CMapDecorator::InitData(CDlgDataSource *pDS)
{
	if( !pDS )
		return;

	int scale = pDS->GetScale();
	m_scale = scale;
	m_pDS = pDS;

	if( m_bNew )
		MapDecorate::bMapDecorateNew = TRUE;
	else
		MapDecorate::bMapDecorateNew = FALSE;
	CreateObjs(scale);
	
	memset(m_SelectedBoundPts,0,sizeof(m_SelectedBoundPts));

	pDS->GetBound(m_SelectedBoundPts,NULL,NULL);

	char fileName[256], fileExt[256];
	_splitpath(pDS->GetName(),NULL,NULL,fileName,fileExt);
	CString mapNum = fileName;

	CNumber *pNumber = (CNumber*)GetObj("Number");
	
	strncpy(pNumber->number,mapNum,sizeof(pNumber->number)-1);

	m_tm.SetDatum(&pDS->GetDatum());
	m_tm.SetZone(&pDS->GetProjection());
	
	if( scale<5000 )
	{
		m_nMapType = TK_MAPTYPE_RECT5050;
		m_nMapBorderType = TK_MAPBORDER_4PT;
		
		m_fMapWid = 50; 
		m_fMapHei = 50;
	}
	else
	{
		m_nMapType = TK_MAPTYPE_STD;
		m_nMapBorderType = TK_MAPBORDER_MAPNUM;
		
		m_fMapWid = 50; 
		m_fMapHei = 50;
	}

	if( scale==25000 || scale==50000 )
	{
//		CGrid *pGrid = (CGrid*)GetObj("Grid");
		CGrid *pGrid = (CGrid*)GetObj("Grid_New");
		if( !pGrid )return;
		
		pGrid->grid_wid = 1000*1000/scale;
	}
	if( scale==100000 )
	{
//		CGrid *pGrid = (CGrid*)GetObj("Grid");
		CGrid *pGrid = (CGrid*)GetObj("Grid_New");
		if( !pGrid )return;
		
		pGrid->grid_wid = 2000*1000/scale;
	}

	Envelope e;
	e.CreateFromPts(m_SelectedBoundPts,4);
	CalcMapNumberAndNearmap(pDS,PT_3D(e.CenterX(),e.CenterY(),0));

//这里被改写过
	if( 5000 == scale || 10000 ==  scale )
	{
		CScale_5KN *pScale = (CScale_5KN*)GetObj("Scale_5KN");
		sprintf(pScale->scaleText,"1:%d",scale);
	}
	else if( 25000 == scale || 50000 ==  scale || 100000 == scale)
	{
		CScale_25KN *pScale = (CScale_25KN*)GetObj("Scale_25KN");
		sprintf(pScale->scaleText,"1:%d",scale);
	}
	else
	{
		CScale *pScale = (CScale*)GetObj("Scale");
		sprintf(pScale->scaleText,"1:%d",scale);
	}

}

void CMapDecorator::Clear()
{
	for( int i=0; i<m_arrPObjs.GetSize(); i++)
	{
		delete m_arrPObjs[i];
	}
	m_arrPObjs.RemoveAll();

	for( i=0; i<m_arrParamTabs.GetSize(); i++)
	{
		delete m_arrParamTabs[i];
	}
	m_arrParamTabs.RemoveAll();
}


void CMapDecorator::CreateObj(LPCTSTR id)
{
	if( GetObj(id)==NULL )
	{
		CBaseObj *pObj = CreateFromID(id);
		if( pObj )
		{
			pObj->m_pMain = this;
			pObj->m_pAssist = &m_assist;

			m_arrPObjs.Add(pObj);
		}
	}
}


void CMapDecorator::CalcMapSize()
{
	if( m_nMapType==TK_MAPTYPE_RECT5050 )
	{
		m_fMapWid = 50;
		m_fMapHei = 50;
	}
	else if( m_nMapType==TK_MAPTYPE_RECT5040 )
	{
		m_fMapWid = 50;
		m_fMapHei = 40;
	}
	else if( m_nMapType==TK_MAPTYPE_RECT4040 )
	{
		m_fMapWid = 40;
		m_fMapHei = 40;
	}
}


static void CreateRectMapNumber(PT_3D pt, long scale, int nint, int nfloat, CString *pText, char *pNumber)
{
	CString text;
	text = FormatFloat(pt.y*0.001,nint,nfloat) + "-" + FormatFloat(pt.x*0.001,nint,nfloat);

	if( pText )*pText = text;
	if( pNumber )strcpy(pNumber,text);
}


CString CMapDecorator::CalcRectMapNumber(PT_3D pt, long scale)
{
	CString text;

	int type = m_nMapType;
	if( scale<5000 && type==TK_MAPTYPE_STD )
	{
		type = TK_MAPTYPE_RECT5050;
	}
	if( type==TK_MAPTYPE_RECT5050 || type==TK_MAPTYPE_RECT5040 || type==TK_MAPTYPE_RECT4040 )
	{
		pt.x = floor(pt.x/(m_fMapWid*scale*0.01) + 1e-6)*(m_fMapWid*scale*0.01);
		pt.y = floor(pt.y/(m_fMapHei*scale*0.01) + 1e-6)*(m_fMapHei*scale*0.01);

		CNumber *pNumber = (CNumber*)GetObj("Number");
		if( pNumber )
			CreateRectMapNumber(pt,scale,pNumber->int_num,pNumber->float_num,&text,NULL);
		else
			CreateRectMapNumber(pt,scale,2,1,&text,NULL);
	}
	return text;
}

CBaseObj *CMapDecorator::GetObj(LPCTSTR id)
{
	for( int i=0; i<m_arrPObjs.GetSize(); i++)
	{
		if( stricmp(m_arrPObjs[i]->StringID(),id)==0 )
		{
			return m_arrPObjs[i];
		}
	}
	return NULL;
}

void CMapDecorator::CalcRectNearmap(PT_3D pt, long scale)
{	
	if( scale<5000 && m_nMapType==TK_MAPTYPE_STD )
	{
		m_nMapType = TK_MAPTYPE_RECT5050;
	}
	if( m_nMapType==TK_MAPTYPE_RECT5050 || m_nMapType==TK_MAPTYPE_RECT5040 || m_nMapType==TK_MAPTYPE_RECT4040 )
	{
		CalcMapSize();

		CNearMap *pObj = (CNearMap*)GetObj("NearMap");
		if( !pObj )return;

		CNumber *pNumber = (CNumber*)GetObj("Number");
		if( !pNumber )return;

		pt.x = floor(pt.x/(m_fMapWid*scale*0.01) + 1e-6)*(m_fMapWid*scale*0.01);
		pt.y = floor(pt.y/(m_fMapHei*scale*0.01) + 1e-6)*(m_fMapHei*scale*0.01);

		PT_3D pt1 = pt;
		pt1.x -= m_fMapWid*scale*0.01;
		pt1.y += m_fMapHei*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name1);

		pt1 = pt;
		pt1.y += m_fMapHei*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name2);

		pt1 = pt;
		pt1.x += m_fMapWid*scale*0.01;
		pt1.y += m_fMapHei*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name3);

		pt1 = pt;
		pt1.x -= m_fMapWid*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name4);

		pt1 = pt;
		pt1.x += m_fMapWid*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name5);

		pt1 = pt;
		pt1.x -= m_fMapWid*scale*0.01;
		pt1.y -= m_fMapHei*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name6);

		pt1 = pt;
		pt1.y -= m_fMapHei*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name7);

		pt1 = pt;
		pt1.x += m_fMapWid*scale*0.01;
		pt1.y -= m_fMapHei*scale*0.01;
		CreateRectMapNumber(pt1,scale,pNumber->int_num,pNumber->float_num,NULL,pObj->nearmap.name8);
	}
}

void CMapDecorator::CalcSmallScaleNearmap()
{
	if( m_nMapType==TK_MAPTYPE_STD )
	{
//		CNearMap *pObj = (CNearMap*)GetObj("NearMap");
//		if( !pObj )return;
//此处被修改
		CNearMap *pObj = NULL;		
		if( m_scale < 5000 && FALSE == m_bNew )
		{//500-1000-2000旧版
			pObj = (CNearMap*)GetObj("NearMap");
		}
		else if( (5000==m_scale || 10000==m_scale) && TRUE==m_bNew )	
		{//5000-10000新版
			pObj = (CAdminRegion_NearMap*)GetObj("AdminRegion_NearMap");
		}
		else if( (25000==m_scale || 50000==m_scale || 100000 == m_scale) && FALSE==m_bNew )	
		{//25000-50000旧版
			pObj = (CNearMap*)GetObj("NearMap");
		}
		else if( (25000==m_scale || 50000==m_scale || 100000 == m_scale) && TRUE==m_bNew )	
		{//25000-50000新版
			pObj = (CAdminRegion_NearMap*)GetObj("AdminRegion_NearMap");
		}
		else
		{//若增加了其他图廓整饰，需改写此处代码
			pObj = (CNearMap*)GetObj("NearMap");
		}
		if( !pObj )return;

		CNumber *pNumber = (CNumber*)GetObj("Number");
		if( !pNumber )return;

		CMapper mc;

		TMDatum datum;
		TMProjectionZone zone;
				
		m_tm.GetDatum( (BYTE*)datum.tmName,&datum.a,&datum.b,&datum.dx,&datum.dy,&datum.dz );
		m_tm.GetProjection( (BYTE*)zone.zoneName,&zone.central,&zone.origin_Lat,&zone.false_Easting,&zone.false_Northing,&zone.scale );
		strcpy( zone.tmName,datum.tmName );
		mc.SetDatum( &datum );
		mc.SetZone( &zone );

		CNMapIndex mapIndex;
		mc.CNMapNameToIndex(pNumber->number,mapIndex);
		
		if( mc.IsValidCNMapIndex(mapIndex) )
		{
			CNMapIndex index1;

			if( mc.GetMapIndexNearby(mapIndex,&index1,-1,1) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name6);
			}
			if( mc.GetMapIndexNearby(mapIndex,&index1,0,1) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name7);
			}
			if( mc.GetMapIndexNearby(mapIndex,&index1,1,1) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name8);
			}
			if( mc.GetMapIndexNearby(mapIndex,&index1,-1,0) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name4);
			}
			if( mc.GetMapIndexNearby(mapIndex,&index1,1,0) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name5);
			}
			if( mc.GetMapIndexNearby(mapIndex,&index1,-1,-1) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name1);
			}
			if( mc.GetMapIndexNearby(mapIndex,&index1,0,-1) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name2);
			}
			if( mc.GetMapIndexNearby(mapIndex,&index1,1,-1) )
			{
				mc.CNMapIndexToName(index1,pObj->nearmap.name3);
			}
		}
	}
}


BOOL CMapDecorator::CalcSmallScaleMapNumber(CDlgDataSource *pDS, PT_3D pt)
{
	TMProjectionZone prj = pDS->GetProjection();
	TMDatum datum = pDS->GetDatum();
	
	if( strlen(datum.tmName)<=0 )
	{
		return FALSE;
	}

	CNumber *pNumber = (CNumber*)GetObj("Number");
	if( !pNumber )return FALSE;

	CTM tm;
	tm.SetDatum(&datum);
	tm.SetZone(&prj);

	PT_3D pt2;
	tm.ConvertToGeodetic(pt.x,pt.y,&pt2.y,&pt2.x);

	pt2.x *= 180/PI;
	pt2.y *= 180/PI;

	CMapper::CNMapCoordToName(pt2.x,pt2.y,pDS->GetScale(),pNumber->number);

	return TRUE;
}


void CMapDecorator::CalcMapNumberAndNearmap(CDlgDataSource *pDS, PT_3D pt)
{
	long scale = pDS->GetScale();

	if( scale<5000 && m_nMapType==TK_MAPTYPE_STD )
	{
		m_nMapType = TK_MAPTYPE_RECT5050;
	}

	if( m_nMapType==TK_MAPTYPE_RECT5050 || m_nMapType==TK_MAPTYPE_RECT5040 || m_nMapType==TK_MAPTYPE_RECT4040 )
	{
		CNumber *pNumber = (CNumber*)GetObj("Number");
		if( !pNumber )return;

		strcpy(pNumber->number,CalcRectMapNumber(pt,scale));
		CalcRectNearmap(pt,scale);
	}
	else
	{
		CalcSmallScaleMapNumber(pDS,pt);
		CalcSmallScaleNearmap();
	}
}


void CMapDecorator::LoadParams(long scale)
{
	CString path = GetMyConfigPath(scale);
	
	CString filePath;
	filePath.Format("MapDecorator%d.dat",scale);
	filePath = path+filePath;
	
	LoadParams(filePath);
}


void CMapDecorator::LoadParams(LPCTSTR path)
{
	m_strSavePath = path;

	CMarkup xml;
	if( !xml.Load(path) )
		return;

	if( !xml.FindElem("MapDecorateParam") )
	{
		return;
	}

	xml.IntoElem();

	CValueTable tab;	

	while( xml.FindElem(NULL,FALSE) )
	{
		CString id = xml.GetTagName();
		CBaseObj *pObj = GetObj(id);
		if( !pObj )
			continue;

		xml.IntoElem();

		CValueTable tab;
		tab.BeginAddValueItem();
		Xml_ReadValueTable(xml,tab);
		tab.EndAddValueItem();

		pObj->SetParams(tab);

		xml.OutOfElem();
	}

	xml.OutOfElem();
}


void CMapDecorator::SaveParams(LPCTSTR path)
{
	CString path2 = path;
	if( path2.IsEmpty() )
	{
		path2 = m_strSavePath;
	}
	
	CMarkup xml;
	
	xml.AddElem("MapDecorateParam");
	xml.IntoElem();

	for( int i=0; i<m_arrPObjs.GetSize(); i++)
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		m_arrPObjs[i]->GetParams(tab);
		tab.EndAddValueItem();

		xml.AddElem(m_arrPObjs[i]->StringID());
		xml.IntoElem();

		Xml_WriteValueTable(xml,tab);

		xml.OutOfElem();
	}
	
	xml.OutOfElem();

	xml.Save(path2);
}

void CMapDecorator::LoadDatas(LPCTSTR path)
{
	CString path2 = path;
	if( path2.IsEmpty() )
	{
		path2 = GetConfigPath(TRUE);
		path2 += _T("\\MapDecoratorData.xml");
	}
	CMarkup xml;
	if( !xml.Load(path2) )
		return;
	
	if( !xml.FindElem("MapDecorateData") )
	{
		return;
	}
	
	xml.IntoElem();
	
	CValueTable tab;	
	
	while( xml.FindElem(NULL,FALSE) )
	{
		CString id = xml.GetTagName();
		CBaseObj *pObj = GetObj(id);
		if( !pObj )
			continue;
		
		xml.IntoElem();
		
		CValueTable tab;
		tab.BeginAddValueItem();
		Xml_ReadValueTable(xml,tab);
		tab.EndAddValueItem();
		
		pObj->SetData(tab);
		pObj->SetBaseData(tab);
		
		xml.OutOfElem();
	}
	
	xml.OutOfElem();
}


void CMapDecorator::SaveDatas(LPCTSTR path)
{
	CString path2 = path;
	if( path2.IsEmpty() )
	{
		path2 = GetConfigPath(TRUE);
		path2 += _T("\\MapDecoratorData.xml");
	}
	
	CMarkup xml;
	
	xml.AddElem("MapDecorateData");
	xml.IntoElem();
	
	for( int i=0; i<m_arrPObjs.GetSize(); i++)
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		m_arrPObjs[i]->GetData(tab);
		m_arrPObjs[i]->GetBaseData(tab);
		tab.EndAddValueItem();
		
		xml.AddElem(m_arrPObjs[i]->StringID());
		xml.IntoElem();
		
		Xml_WriteValueTable(xml,tab);
		
		xml.OutOfElem();
	}
	
	xml.OutOfElem();
	
	xml.Save(path2);
}

void CMapDecorator::SaveParamsMem()
{
	for( int i=0; i<m_arrParamTabs.GetSize(); i++)
	{
		delete m_arrParamTabs[i];
	}
	m_arrParamTabs.RemoveAll();

	for( i=0; i<m_arrPObjs.GetSize(); i++)
	{
		CValueTable *pTab = new CValueTable();
		pTab->BeginAddValueItem();
		m_arrPObjs[i]->GetParams(*pTab);
		pTab->EndAddValueItem();

		m_arrParamTabs.Add(pTab);
	}
}


void CMapDecorator::RestoreParamsMem()
{
	for( int i=0; i<m_arrPObjs.GetSize(); i++)
	{
		CValueTable *pTab = m_arrParamTabs[i];
		m_arrPObjs[i]->SetParams(*pTab);
	}	
}


BOOL CMapDecorator::Build(CDlgDoc *pDoc, CDlgDataSource *pDS, int scale, BOOL bSilence, BOOL bSortBound)
{
	m_pDoc = pDoc;
	m_scale = scale;
	m_bSilence = bSilence;
	m_bSortBound = bSortBound;

	m_arrNewObjHandles.RemoveAll();

	m_assist.m_pArrNewObjHandles = &m_arrNewObjHandles;

	if( !m_pDoc )
	{
		if( !pDS )
			return FALSE;

		m_pDS = pDS;
		m_assist.SetDlgDataSource(pDS,scale);
		//分幅输出时图廓范围
		PT_3D pts[4];
		double zmin,zmax;
		m_pDS->GetBound(pts, &zmin, &zmax);
		memcpy(m_SelectedBoundPts,pts,sizeof(pts));
	}
	else
	{
		m_assist.SetDoc(pDoc,scale);
	}

	if( !CalcMapBound() )
	{
		AfxMessageBox(StrFromResID(IDS_MD_CREATE_FAIL));
		return FALSE;
	}

	COuterBound *pOutB = (COuterBound*)GetObj("OuterBound");

	//计算外图廓
	double ext = pOutB->extension*m_assist.Unit();	
	memcpy(m_outerBound,m_innerBound,sizeof(m_outerBound));
	Extend(m_outerBound,-ext);
	
	if( m_pDoc )
		m_pDoc->BeginBatchUpdate();
	else if( m_pDS->GetAccessObject() )
		m_pDS->GetAccessObject()->BatchUpdateBegin();

	for( int i=0; i<m_arrPObjs.GetSize(); i++)
	{
		if( m_arrPObjs[i]->m_bUsed )
		{
			m_arrPObjs[i]->MakeFtrs();
		}
	}

	if( m_pDoc )
		m_pDoc->EndBatchUpdate();
	else if( m_pDS->GetAccessObject() )
		m_pDS->GetAccessObject()->BatchUpdateEnd();

	return TRUE;
}



BOOL CMapDecorator::CalcMapBound()
{
	//计算内图廓坐标
	if( m_nMapBorderType==0 )
	{
		memcpy(m_innerBound,m_SelectedBoundPts,sizeof(m_innerBound));
	}
	else if( m_nMapBorderType==1 )
	{
		m_innerBound[0] = m_SelectBoundCorner;
		m_innerBound[1] = m_innerBound[0];
		m_innerBound[1].x += m_fMapWid * 10 *m_assist.Unit();
		m_innerBound[2] = m_innerBound[1];
		m_innerBound[2].y += m_fMapHei * 10 *m_assist.Unit();
		m_innerBound[3] = m_innerBound[0];
		m_innerBound[3].y += m_fMapHei * 10 *m_assist.Unit();
	}
	else if( m_nMapBorderType==2 )
	{
		TMDatum datum;
		TMProjectionZone zone;

		CMapper mc;
		double lfCoord[8];

		m_tm.GetDatum( (BYTE*)datum.tmName,&datum.a,&datum.b,&datum.dx,&datum.dy,&datum.dz );
		m_tm.GetProjection( (BYTE*)zone.zoneName,&zone.central,&zone.origin_Lat,&zone.false_Easting,&zone.false_Northing,&zone.scale );
		strcpy( zone.tmName,datum.tmName );
		mc.SetDatum( &datum );
		mc.SetZone( &zone );

		CNumber *pNumber = (CNumber*)GetObj("Number");

		if( mc.CNMapNameToCoord2(pNumber->number,lfCoord) )
		{
			m_innerBound[0].x = lfCoord[0];
			m_innerBound[0].y = lfCoord[1];
			m_innerBound[1].x = lfCoord[2];
			m_innerBound[1].y = lfCoord[3];
			m_innerBound[2].x = lfCoord[4];
			m_innerBound[2].y = lfCoord[5];
			m_innerBound[3].x = lfCoord[6];
			m_innerBound[3].y = lfCoord[7];
		}
		else
		{
			m_assist.GetDlgDataSource()->GetBound(m_innerBound,NULL,NULL);
		}
	}
	else
	{
		m_assist.GetDlgDataSource()->GetBound(m_innerBound,NULL,NULL);
	}

	if( m_bSortBound )
		SortBoundPoints(m_innerBound);
	
	if( GraphAPI::GIsClockwise(m_innerBound,4) )
	{
		PT_3D t = m_innerBound[1];
		
		m_innerBound[1] = m_innerBound[3];
		m_innerBound[3] = t;
	}

	if( GraphAPI::GIsClockwise(m_innerBound,4)==-1 )
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CMapDecorator::ImportXML(LPCTSTR path, CFtrLayer *pLayer, PT_3D move_pt, PT_3D rotate_pt, double angle)
{
	if( m_bSilence )
	{
		if( _access(path,04)!=0 )
			return FALSE;
	}

	CFtrArray arr;
	BOOL bRet = FALSE;

	if( m_pDoc )
	{
		bRet = m_pDoc->ImportXML(path,pLayer,move_pt,rotate_pt,angle,&arr);
	}
	else
		bRet = ImportXMLtoDS(path,m_pDS,pLayer,move_pt,rotate_pt,angle,&arr);

	for( int i=0; i<arr.GetSize(); i++)
	{
		m_arrNewObjHandles.Add(FtrToHandle(arr[i]));
	}
	return bRet;
}


//调整节点顺序，使得左下角点为第一个点
void CMapDecorator::SortBoundPoints(PT_3D pts[4])
{
	PT_3D pts2[4];
	
	int k = -1;
	double dis, min;
	for(int i=0; i<4; i++)
	{
		dis = pts[i].x + pts[i].y;
		if( k<0 || min>dis )
		{
			k = i;
			min = dis;
		}
	}
	
	for( i=0; i<4; i++)
	{
		pts2[i] = pts[(k+i)%4];
	}
	if( GraphAPI::GIsClockwise(pts2,4) )
	{
		PT_3D t = pts2[1];
		pts2[1] = pts2[3];
		pts2[3] = t;
	}
	memcpy(pts,pts2,sizeof(pts2));
}


void CMapDecorator::RotateBound(PT_3D pt0, PT_3D pt1, CMapDecorator::XBound& xb)
{	
	double r = GraphAPI::GGet2DDisOf2P(pt0,pt1);
	xb.cosa = (pt1.x-pt0.x)/r, xb.sina = (pt1.y-pt0.y)/r;
	xb.ang = GraphAPI::GGetAngle(pt0.x,pt0.y,pt1.x,pt1.y);
	
	memcpy(xb.innerBound,m_innerBound,sizeof(xb.innerBound));
	memcpy(xb.outerBound,m_outerBound,sizeof(xb.outerBound));
	
	RotatePts(xb.innerBound,4,xb.cosa,-xb.sina);
	RotatePts(xb.outerBound,4,xb.cosa,-xb.sina);
}



CString CMapDecorator::GetFigurePath(long scale)
{
	CString path = GetMyConfigPath(scale);
	
	CString filePath;
	filePath.Format("Figure%d.xml",(int)scale);
	filePath = path+filePath;

	return filePath;
}


PT_3D CMapDecorator::GaussToGeo(PT_3D pt)
{
	double lat = 0, lon = 0;
	m_tm.ConvertToGeodetic(pt.x,pt.y,&lat,&lon);

	return PT_3D(lon,lat,0);
}


PT_3D CMapDecorator::GeoToGauss(PT_3D pt)
{
	double x = 0, y = 0;
	m_tm.ConvertFromGeodetic(pt.y,pt.x,&x,&y);
	return PT_3D(x,y,0);
}

//------------------------------------------------------------
CDataDesc_5KN::CDataDesc_5KN()
{
	m_strName = StrFromResID(IDS_MD_DATADESC);
	dx = 8.0;
	strcpy(layer,"TK_data_desc");

	strcpy(data_desc1,StrFromResID(IDS_MD_DATADESC_5KN1));
	strcpy(font1,StrFromResID(IDS_MD_BIANSONGTI));	
	size1 = 2;
	wscale1 = 1.375;
	char_interv1 = 0;
	line_interv1 = 1;

	strcpy(data_desc2,StrFromResID(IDS_MD_DATADESC_5KN2));
	strcpy(font2,StrFromResID(IDS_MD_BIANSONGTI));	
	size2 = 2;
	wscale2 = 1;
	char_interv2 = 0;
	line_interv2 = 1;
}

CDataDesc_5KN::~CDataDesc_5KN()
{
}

void CDataDesc_5KN::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));

	pParam->AddFontNameParam("font1",font1,StrFromResID(IDS_FIELDNAME_TEXT_FONT1));
	pParam->AddParam("size1",size1,StrFromResID(IDS_FIELDNAME_TEXT_CHARHEIGHT1));
	pParam->AddParam("wscale1",wscale1,StrFromResID(IDS_FIELDNAME_TEXT_CHARWIDTHS1));
	pParam->AddParam("char_interv1",char_interv1,StrFromResID(IDS_FIELDNAME_TEXT_CHARINTVS1));
	pParam->AddParam("line_interv1",line_interv1,StrFromResID(IDS_FIELDNAME_TEXT_LINEINTVS1));

	pParam->AddFontNameParam("font2",font2,StrFromResID(IDS_FIELDNAME_TEXT_FONT2));
	pParam->AddParam("size2",size2,StrFromResID(IDS_FIELDNAME_TEXT_CHARHEIGHT2));
	pParam->AddParam("wscale2",wscale2,StrFromResID(IDS_FIELDNAME_TEXT_CHARWIDTHS2));
	pParam->AddParam("char_interv2",char_interv2,StrFromResID(IDS_FIELDNAME_TEXT_CHARINTVS2));
	pParam->AddParam("line_interv2",line_interv2,StrFromResID(IDS_FIELDNAME_TEXT_LINEINTVS2));
}

void CDataDesc_5KN::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
//
	if( tab.GetValue(0,"font1",pvar) )
	{
		strncpy(font1,(LPCTSTR)(_bstr_t)*pvar,sizeof(font1)-1);
	}
	if( tab.GetValue(0,"size1",pvar) )
	{
		size1 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale1",pvar) )
	{
		wscale1 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv1",pvar) )
	{
		char_interv1 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"line_interv1",pvar) )
	{
		line_interv1 = (float)(_variant_t)*pvar;
	}
//
	if( tab.GetValue(0,"font2",pvar) )
	{
		strncpy(font2,(LPCTSTR)(_bstr_t)*pvar,sizeof(font2)-1);
	}
	if( tab.GetValue(0,"size2",pvar) )
	{
		size2 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale2",pvar) )
	{
		wscale2 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"char_interv2",pvar) )
	{
		char_interv2 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"line_interv2",pvar) )
	{
		line_interv2 = (float)(_variant_t)*pvar;
	}
}

void CDataDesc_5KN::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
//
	var = (LPCTSTR)font1;
	tab.AddValue("font1",&CVariantEx(var));	

	var = size1;
	tab.AddValue("size1",&CVariantEx(var));	

	var = wscale1;
	tab.AddValue("wscale1",&CVariantEx(var));
	
	var = char_interv1;
	tab.AddValue("char_interv1",&CVariantEx(var));
	
	var = line_interv1;
	tab.AddValue("line_interv1",&CVariantEx(var));
//
	var = (LPCTSTR)font2;
	tab.AddValue("font2",&CVariantEx(var));
	
	var = size2;
	tab.AddValue("size2",&CVariantEx(var));
	
	var = wscale2;
	tab.AddValue("wscale2",&CVariantEx(var));
	
	var = char_interv2;
	tab.AddValue("char_interv2",&CVariantEx(var));
	
	var = line_interv2;
	tab.AddValue("line_interv2",&CVariantEx(var));
}

void CDataDesc_5KN::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddMultiEditParam("data_desc1",data_desc1,StrFromResID(IDS_MD_DATADESC));
	pParam->AddMultiEditParam("data_desc2",data_desc2,StrFromResID(IDS_MD_DATADESC));
}

void CDataDesc_5KN::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"data_desc1",pvar) )
	{
		strncpy(data_desc1,(LPCTSTR)(_bstr_t)*pvar,sizeof(data_desc1)-1);
	}

	if( tab.GetValue(0,"data_desc2",pvar) )
	{
		strncpy(data_desc2,(LPCTSTR)(_bstr_t)*pvar,sizeof(data_desc2)-1);
	}
}

void CDataDesc_5KN::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)data_desc1;
	tab.AddValue("data_desc1",&CVariantEx(var));

	var = (LPCTSTR)data_desc2;
	tab.AddValue("data_desc2",&CVariantEx(var));
}

void CDataDesc_5KN::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	//获取坡度尺上的等高距
	int con_interv = 5;
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();
	if( 5000 == scale || 10000 == scale )
	{
		CRuler* pRuler = (CRuler*)m_pMain->GetObj("Ruler");
		if(pRuler) con_interv = pRuler->con_interv;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale )
	{
		CRuler_25KN* pRuler = (CRuler_25KN*)m_pMain->GetObj("Ruler_25KN");
		if(pRuler) con_interv = pRuler->con_interv;
	}
	CString str;
	str.Format(IDS_MD_DATADESC_5KN2, con_interv);
	strcpy(data_desc2, str.GetBuffer(str.GetLength()) );
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	CFtrLayer *pLayer = NULL;

	//资料说明第一部分
	PT_3D textpt;
	textpt.x = xb.outerBound[1].x + dx*m_pAssist->Unit(); 
	textpt.y = xb.outerBound[1].y + ( (3+3*line_interv2)*size2+(2+line_interv1)*size1 )*m_pAssist->Unit();
	textpt.z = 0;

	CFeature *pFtr1 = m_pAssist->CreateText(layer,pLayer);
	if( !pFtr1 )
		return;

	CGeoText *pText1 = (CGeoText*)pFtr1->GetGeometry();

	m_pAssist->SetText(pText1,&textpt,1,data_desc1,font1,size1,wscale1,TAH_LEFT|TAV_TOP);

	TEXT_SETTINGS0 ts1;
	pText1->GetSettings(&ts1);

	ts1.fCharIntervalScale = char_interv1;
	ts1.fLineSpacingScale = 0;
	ts1.fTextAngle = 0;
	pText1->SetSettings(&ts1);

	CFtrArray arrFtrs;
	
	if( m_pAssist->SplitMultipleLineText(pFtr1,line_interv1*m_pAssist->Unit(),arrFtrs) )
	{
		for( int i=0; i<arrFtrs.GetSize(); i++)
		{
			CGeoText *pText = (CGeoText*)arrFtrs[i]->GetGeometry();
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pText->GetShape(arrPts);
			
			RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
			pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
			
			m_pAssist->SetText(pText,xb.ang*180/PI);
			
			m_pAssist->AddObject(arrFtrs[i],pLayer->GetID());
		}
		delete pFtr1;
	}
	else
	{
		CGeoText *pText = (CGeoText*)pFtr1->GetGeometry();
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pText->GetShape(arrPts);
		
		RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
		pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		m_pAssist->SetText(pText,xb.ang*180/PI);
		
		m_pAssist->AddObject(pFtr1,pLayer->GetID());
	}
	//

	//资料说明第二部分
	textpt.x = xb.outerBound[1].x + dx*m_pAssist->Unit(); 
	textpt.y = xb.outerBound[1].y + ( (3+2*line_interv2)*size2 )*m_pAssist->Unit();
	textpt.z = 0;

	CFeature *pFtr2 = m_pAssist->CreateText(layer,pLayer);
	if( !pFtr2 )
		return;

	CGeoText *pText2 = (CGeoText*)pFtr2->GetGeometry();

	m_pAssist->SetText(pText2,&textpt,1,data_desc2,font2,size2,wscale2,TAH_LEFT|TAV_TOP);

	TEXT_SETTINGS0 ts2;
	pText2->GetSettings(&ts2);

	ts2.fCharIntervalScale = char_interv2;
	ts2.fLineSpacingScale = 0;
	ts2.fTextAngle = 0;
	pText2->SetSettings(&ts2);

	arrFtrs.RemoveAll();
	
	if( m_pAssist->SplitMultipleLineText(pFtr2,line_interv2*m_pAssist->Unit(),arrFtrs) )
	{
		for( int i=0; i<arrFtrs.GetSize(); i++)
		{
			CGeoText *pText = (CGeoText*)arrFtrs[i]->GetGeometry();
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pText->GetShape(arrPts);
			
			RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
			pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
			
			m_pAssist->SetText(pText,xb.ang*180/PI);
			
			m_pAssist->AddObject(arrFtrs[i],pLayer->GetID());
		}
		delete pFtr2;
	}
	else
	{
		CGeoText *pText = (CGeoText*)pFtr2->GetGeometry();
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pText->GetShape(arrPts);
		
		RotatePts(arrPts.GetData(),arrPts.GetSize(),xb.cosa,xb.sina);
		pText->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		m_pAssist->SetText(pText,xb.ang*180/PI);
		
		m_pAssist->AddObject(pFtr2,pLayer->GetID());
	}
	
}

//
CAdminRegion_NearMap::CAdminRegion_NearMap()
{
	m_strName = StrFromResID(IDS_MD_ADMINREGION_NEARMAP);
	memset(&nearmap,0,sizeof(nearmap));
	dx = 8;

	strcpy(font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	strcpy(layer,"TK_admin_region_nearmap");
	size = 2.5;
	wscale = 1;
	width = 80;
	height = 60;

	src_layers = "";

	strcpy(title_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	title_size = 4;
	title_wscale = 1;
}

CAdminRegion_NearMap::~CAdminRegion_NearMap()
{
}

void CAdminRegion_NearMap::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("width",width,StrFromResID(IDS_MD_ALLWID));
	pParam->AddParam("height",height,StrFromResID(IDS_MD_ALLHEI));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));	
}

void CAdminRegion_NearMap::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"width",pvar) )
	{
		width = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"height",pvar) )
	{
		height = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
}

void CAdminRegion_NearMap::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = width;
	tab.AddValue("width",&CVariantEx(var));

	var = height;
	tab.AddValue("height",&CVariantEx(var));

	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));	
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));	
}

void CAdminRegion_NearMap::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddLayerNameParamEx("src_layers",src_layers,StrFromResID(IDS_ADMINREGION_SRC_LAYER_NAME),NULL,LAYERPARAMITEM_LINEAREA);

	pParam->AddParam("name1",nearmap.name1,StrFromResID(IDS_MD_NEARMAP_11));
	pParam->AddParam("name2",nearmap.name2,StrFromResID(IDS_MD_NEARMAP_12));
	pParam->AddParam("name3",nearmap.name3,StrFromResID(IDS_MD_NEARMAP_13));
	pParam->AddParam("name4",nearmap.name4,StrFromResID(IDS_MD_NEARMAP_21));
	pParam->AddParam("name5",nearmap.name5,StrFromResID(IDS_MD_NEARMAP_23));
	pParam->AddParam("name6",nearmap.name6,StrFromResID(IDS_MD_NEARMAP_31));
	pParam->AddParam("name7",nearmap.name7,StrFromResID(IDS_MD_NEARMAP_32));
	pParam->AddParam("name8",nearmap.name8,StrFromResID(IDS_MD_NEARMAP_33));
}

void CAdminRegion_NearMap::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"src_layers",pvar) )
	{
		src_layers = (const char*)(_bstr_t)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"name1",pvar) )
	{
		strncpy(nearmap.name1,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name1)-1);
	}
	if( tab.GetValue(0,"name2",pvar) )
	{
		strncpy(nearmap.name2,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name2)-1);
	}
	if( tab.GetValue(0,"name3",pvar) )
	{
		strncpy(nearmap.name3,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name3)-1);
	}
	if( tab.GetValue(0,"name4",pvar) )
	{
		strncpy(nearmap.name4,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name4)-1);
	}
	if( tab.GetValue(0,"name5",pvar) )
	{
		strncpy(nearmap.name5,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name5)-1);
	}
	if( tab.GetValue(0,"name6",pvar) )
	{
		strncpy(nearmap.name6,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name6)-1);
	}
	if( tab.GetValue(0,"name7",pvar) )
	{
		strncpy(nearmap.name7,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name7)-1);
	}
	if( tab.GetValue(0,"name8",pvar) )
	{
		strncpy(nearmap.name8,(LPCTSTR)(_bstr_t)*pvar,sizeof(nearmap.name8)-1);
	}
}

void CAdminRegion_NearMap::GetData(CValueTable& tab)
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)src_layers;
	tab.AddValue("src_layers", &CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name1;
	tab.AddValue("name1",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name2;
	tab.AddValue("name2",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name3;
	tab.AddValue("name3",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name4;
	tab.AddValue("name4",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name5;
	tab.AddValue("name5",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name6;
	tab.AddValue("name6",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name7;
	tab.AddValue("name7",&CVariantEx(var));
	
	var = (LPCTSTR)nearmap.name8;
	tab.AddValue("name8",&CVariantEx(var));	
}

void CAdminRegion_NearMap::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);	

	CFtrLayer *pLayer = NULL;

	PT_3D textpt;
	textpt.x = e.m_xh + (dx+0.5*width)*m_pAssist->Unit();
	textpt.y = e.m_yh + (15-title_size)*m_pAssist->Unit();
	textpt.z = 0;

	PT_3D ptsAdmin[4];		//政区略图的边界
	memset(ptsAdmin,0,sizeof(PT_3D)*4);

	//创建接图表表格线
	double xs, xe, ys, ye;
	xs = e.m_xh + dx*m_pAssist->Unit();
	xe = xs + width*m_pAssist->Unit();
	ye = textpt.y - 3*m_pAssist->Unit();
	ys = ye - height*m_pAssist->Unit();

	PT_3DEX expt;
	expt.pencode = penLine;
	expt.z = 0;

	CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( pGeo )
	{
		//外边框
		PT_3DEX pts[5];
		expt.x = xs; expt.y = ys;
		pts[0] = expt;
		expt.x = xe; expt.y = ys;
		pts[1] = expt;
		expt.x = xe; expt.y = ye;
		pts[2] = expt;
		expt.x = xs; expt.y = ye;
		pts[3] = expt;
		expt.x = xs; expt.y = ys;
		pts[4] = expt;

		RotatePts(pts,5,xb.cosa,xb.sina);

		pGeo->CreateShape(pts,5);

		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	PT_3D linePt[2];
	//内部表格的横线
	linePt[0] = PT_3D(xs,ys + height*m_pAssist->Unit()/4,0);
	linePt[1] = PT_3D(xe,ys + height*m_pAssist->Unit()/4,0);

	ptsAdmin[0].y = ptsAdmin[1].y = linePt[0].y;	

	RotatePts(linePt,2,xb.cosa,xb.sina);	
	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

	linePt[0] = PT_3D(xs,ys + height*m_pAssist->Unit()*3/4,0);
	linePt[1] = PT_3D(xe,ys + height*m_pAssist->Unit()*3/4,0);
	
	ptsAdmin[2].y = ptsAdmin[3].y = linePt[0].y;	

	RotatePts(linePt,2,xb.cosa,xb.sina);
	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);
	
	//内部表格的竖线
	linePt[0] = PT_3D(xs + width*m_pAssist->Unit()/4,ys,0);
	linePt[1] = PT_3D(xs + width*m_pAssist->Unit()/4,ye,0);

	ptsAdmin[0].x = ptsAdmin[3].x = linePt[0].x;
	
	RotatePts(linePt,2,xb.cosa,xb.sina);
	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

	linePt[0] = PT_3D(xs + width*m_pAssist->Unit()*3/4,ys,0);
	linePt[1] = PT_3D(xs + width*m_pAssist->Unit()*3/4,ye,0);

	ptsAdmin[1].x = ptsAdmin[2].x = linePt[0].x;	
	
	RotatePts(linePt,2,xb.cosa,xb.sina);
	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);


	//标题
	pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText_title = (CGeoText*)pFtr->GetGeometry();
	if( !pText_title )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText_title,&textpt,1,StrFromResID(IDS_MD_ADMINREGION_NEARMAP),title_font,title_size,title_wscale,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText_title,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	//创建接图表中的图号文字
	CGeoText *pText;
	
	PT_3D textpts[9];
	memset(textpts,0,sizeof(textpts));
	double xoff0 = (xe-xs)/8, yoff0 = (ye-ys)/8;

	textpts[0].x = xs + xoff0; textpts[0].y = ys + yoff0*7;
	textpts[1].x = textpts[0].x + xoff0*3; textpts[1].y = textpts[0].y;
	textpts[2].x = textpts[1].x + xoff0*3; textpts[2].y = textpts[0].y;
	
	textpts[3].x = xs + xoff0; textpts[3].y = ys + yoff0*4;
	textpts[4].x = textpts[0].x + xoff0*3; textpts[4].y = textpts[3].y;
	textpts[5].x = textpts[1].x + xoff0*3; textpts[5].y = textpts[3].y;

	textpts[6].x = xs + xoff0; textpts[6].y = ys + yoff0;
	textpts[7].x = textpts[0].x + xoff0*3; textpts[7].y = textpts[6].y;
	textpts[8].x = textpts[1].x + xoff0*3; textpts[8].y = textpts[6].y;

	RotatePts(textpts,9,xb.cosa,xb.sina);

	char *textlist[9] = {
		nearmap.name1,nearmap.name2,nearmap.name3,
		nearmap.name4,nearmap.name4,nearmap.name5,
		nearmap.name6,nearmap.name7,nearmap.name8
	};

	for( int i=0; i<9; i++)
	{
		if( i==4 )continue;
		if( strlen(textlist[i])<=0 )
			continue;

		pFtr = m_pAssist->CreateText(layer,pLayer);
		if( !pFtr )continue;

		pText = (CGeoText*)pFtr->GetGeometry();		

		m_pAssist->SetText(pText,textpts+i,1,textlist[i],font,size,wscale,TAH_MID|TAV_MID);
		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	//填充政区略图
	double matrix1[16];			//缩放矩阵
	Envelope e1;
	e1.CreateFromPts(xb.innerBound,4);
	Envelope e2;
	e2.CreateFromPts(ptsAdmin,4);
	double kx = double(e2.Width()/e1.Width());
	double ky = double(e2.Height()/e1.Height());
	double kz = 1.0;
	Matrix44FromZoom(kx,ky,kz,matrix1);
	
	double matrixA[16];			//平移矩阵（将内图廓的中心移至原点）
	Envelope eA;
	eA.CreateFromPts(m_pMain->m_innerBound,4);
	double ax = 0.5*(eA.m_xl+eA.m_xh);
	double ay = 0.5*(eA.m_yl+eA.m_yh);
	double az = 0;
	Matrix44FromMove(-ax,-ay,-az,matrixA);

	double matrixB[16];			//平移矩阵（将抽稀缩放后的地图移至政区略图）
	Envelope eB;
	RotatePts(ptsAdmin,4,xb.cosa,xb.sina);
	eB.CreateFromPts(ptsAdmin,4);
	double bx = 0.5*(eB.m_xl+eB.m_xh);
	double by = 0.5*(eB.m_yl+eB.m_yh);
	double bz = 0;
	Matrix44FromMove(bx,by,bz,matrixB);

	DrawAdminRegion(matrix1, matrixA, matrixB);
}

void CAdminRegion_NearMap::DrawAdminRegion(double matrix1[16], double matrixA[16], double matrixB[16])
{
	if( !m_pMain || !m_pAssist || !m_pMain->m_pDS )
		return;

	CStringArray arr;
	if(!convertStringToStrArray(src_layers,arr))
		return;
	CFtrArray ftrs;
	int i = 0, j = 0;
	for(int a=0;a<arr.GetSize();a++)
	{
		CPtrArray arrLayers;
		if(!(m_pMain->m_pDS)->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
			return;
		int nTar = arrLayers.GetSize();	
		for (i=0;i<nTar;i++)
		{
			CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				if( !IsFeatureInBound(pFtr) )
					continue;
				
				ftrs.Add( pFtr );
			}
		}
	}

	CFtrLayer *pLayer = NULL;
	m_pAssist->CreateCurve(layer,pLayer);

	for(i = 0; i < ftrs.GetSize(); i++)
	{
		CFeature* pFtr = ftrs.GetAt(i);
		CFeature* pNewFtr = pFtr->Clone();

		CGeometry* pObj = pFtr->GetGeometry();
		if(!pObj) continue;

		CGeometry* pNewObj = Compress( pObj , 10);
		if(!pNewObj) continue;
		pNewObj->Transform(matrixA);
		pNewObj->Transform(matrix1);
		pNewObj->Transform(matrixB);

		pNewFtr->SetID(OUID());
		pNewFtr->SetGeometry( pNewObj );
		(m_pMain->m_pDS)->GetXAttributesSource()->CopyXAttributes(pFtr,pNewFtr);

		//由于缩放后的地物放在新的图层，图层间颜色可能不一样，故颜色做单独处理
		long col = pObj->GetColor();
		if( -1 == col )
		{
			CFtrLayer* pOriLayer = (m_pMain->m_pDS)->GetFtrLayerOfObject(pFtr);
			col = pOriLayer->GetColor();
		}
		pNewObj->SetColor( col );

		m_pAssist->AddObject(pNewFtr,pLayer->GetID());
	}

}

bool CAdminRegion_NearMap::IsFeatureInBound(CFeature* pFtr)
{
	if( !m_pMain || !pFtr ) return false;

	//旋转内图廓与坐标系平行
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_innerBound[0],m_pMain->m_innerBound[1],xb);
	Envelope e;
	e.CreateFromPts(xb.innerBound,4);

	CArray<PT_3DEX,PT_3DEX> pts;
	CGeometry* pGeo = pFtr->GetGeometry();
	if( !pGeo ) return false;
	pGeo->GetShape( pts );

	Envelope e2;
	int num = pts.GetSize();
	if( num > 0 )
	{
		PT_3D* allpts = new PT_3D[num];
		for(int i = 0; i < num; i++)
		{
			COPY_3DPT(allpts[i], pts.GetAt(i) );
		}
		RotatePts(allpts, num, xb.cosa, -xb.sina);
		e2.CreateFromPts(allpts, num);
		delete [] allpts;
	}	

	if( e.bEnvelopeIn(&e2) ) return true;
	return false;
}

//
CMakerBottom::CMakerBottom()
{
	m_strName = StrFromResID(IDS_MD_MAKER_BOTTOM);
	strcpy(font,StrFromResID(IDS_MD_HEITI));
	strcpy(layer,"TK_publisher");
	strcpy(makerBottom,StrFromResID(IDS_MD_STATE_BUREAU));
	size = 4.5;
	wscale = 1;
	dy = 5;
}

CMakerBottom::~CMakerBottom()
{
}

void CMakerBottom::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_TSPACE));
}

void CMakerBottom::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CMakerBottom::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CMakerBottom::GetUIParams_data(CUIParam *pParam)
{
}

void CMakerBottom::SetData(CValueTable& tab)
{
}

void CMakerBottom::GetData(CValueTable& tab)
{
}

void CMakerBottom::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	CFtrLayer *pLayer = NULL;

	PT_3D textpt;
	textpt.x = e2.m_xl;
	if( 25000 == m_pMain->m_scale || 50000 == m_pMain->m_scale  || 100000 == m_pMain->m_scale)
		textpt.x = e.m_xl;
	textpt.y = e.m_yl - dy*m_pAssist->Unit();
	textpt.z = 0;

	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText,&textpt,1,makerBottom,font,size,wscale,TAH_LEFT|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



//
CDigitalScale::CDigitalScale()
{
	m_strName = StrFromResID(IDS_MD_DIGITAL_SCALE);
	strcpy(font,StrFromResID(IDS_MD_SONGTI));
	strcpy(layer,"TK_digital_scale");
	size = 4;
	wscale = 1;
	dy = 5;

	strcpy(title_font,StrFromResID(IDS_MD_HEITI));
	title_size = 4.5;
	title_wscale = 1;
}

CDigitalScale::~CDigitalScale()
{
}

void CDigitalScale::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BSPACE));
}

void CDigitalScale::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
}

void CDigitalScale::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));	
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));
}

void CDigitalScale::GetUIParams_data(CUIParam *pParam)
{
}

void CDigitalScale::SetData(CValueTable& tab)
{
}

void CDigitalScale::GetData(CValueTable& tab)
{
}

void CDigitalScale::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	char title[256];
	strcpy(title,StrFromResID(IDS_MD_DIGITAL_SCALE_TITLE));
	char scaleText[64];
	sprintf(scaleText,"1:%d",m_pMain->m_scale);

	CFtrLayer *pLayer = NULL;

//标题（中华人民共和国基本比例尺地形图）
	PT_3D textpt;
	textpt.x = e2.m_xl;
	if( 25000 == m_pMain->m_scale || 50000 == m_pMain->m_scale  || 100000 == m_pMain->m_scale)
		textpt.x = e.m_xl;
	textpt.y = e.m_yh + (dy+size+3)*m_pAssist->Unit();
	textpt.z = 0;

	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText,&textpt,1,title,title_font,title_size,title_wscale,TAH_LEFT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());

//数字比例尺
	PT_3D textpt2;
	textpt2.x = e2.m_xl + title_size*m_pAssist->Unit()*strlen(title)*0.25;
	if( 25000 == m_pMain->m_scale || 50000 == m_pMain->m_scale  || 100000 == m_pMain->m_scale )
		textpt2.x = e.m_xl + title_size*m_pAssist->Unit()*strlen(title)*0.25;
	textpt2.y = e.m_yh + dy*m_pAssist->Unit();
	textpt2.z = 0;

	CFeature* pFtr2 = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText_scale = (CGeoText*)pFtr2->GetGeometry();
	if( !pText_scale )return;

	RotatePts(&textpt2,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText_scale,&textpt2,1,scaleText,font,size,wscale,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText_scale,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr2,pLayer->GetID());
}
//



CNorthPointer_25KN::CNorthPointer_25KN()
{
	m_strName = StrFromResID(IDS_MD_NORTHPOINTER);

	strcpy(layer,"TK_north_pointer");
	dx = 97;
	dy = 3;

	strcpy(font1,StrFromResID(IDS_MD_XIDENGXIANTI));
	size1 = 1.6;
	wscale1 = 1;

	strcpy(font2,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	size2 = 1.6;
	wscale2 = 1;

	angle1 = 0;
	angle2 = 0;

	if(!MapDecorate::bMapDecorateNew)
	{
		size1 = 2;
		size2 = 2;
		strcpy(font2,StrFromResID(IDS_MD_ZHENGDENGXIANTI));
	}
}


CNorthPointer_25KN::~CNorthPointer_25KN()
{
}

void CNorthPointer_25KN::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_NP_TSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_NP_MIDSPACE));	
	
	pParam->AddFontNameParam("font1",font1,StrFromResID(IDS_MD_NP_LINENAME_FONT));
	pParam->AddParam("size1",size1,StrFromResID(IDS_MD_NP_LINENAME_CHARHEIGHT));
	pParam->AddParam("wscale1",wscale1,StrFromResID(IDS_MD_NP_LINENAME_CHARWIDTHS));

	pParam->AddFontNameParam("font2",font2,StrFromResID(IDS_MD_NP_NOTE_FONT));
	pParam->AddParam("size2",size2,StrFromResID(IDS_MD_NP_NOTE_CHARHEIGHT));
	pParam->AddParam("wscale2",wscale2,StrFromResID(IDS_MD_NP_NOTE_CHARWIDTHS));
}

void CNorthPointer_25KN::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}

	if( tab.GetValue(0,"font1",pvar) )
	{
		strncpy(font1,(LPCTSTR)(_bstr_t)*pvar,sizeof(font1)-1);
	}
	if( tab.GetValue(0,"size1",pvar) )
	{
		size1 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale1",pvar) )
	{
		wscale1 = (float)(_variant_t)*pvar;
	}

	if( tab.GetValue(0,"font2",pvar) )
	{
		strncpy(font2,(LPCTSTR)(_bstr_t)*pvar,sizeof(font2)-1);
	}
	if( tab.GetValue(0,"size2",pvar) )
	{
		size2 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale2",pvar) )
	{
		wscale2 = (float)(_variant_t)*pvar;
	}
}

void CNorthPointer_25KN::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));

	var = (LPCTSTR)font1;
	tab.AddValue("font1",&CVariantEx(var));

	var = size1;
	tab.AddValue("size1",&CVariantEx(var));

	var = wscale1;
	tab.AddValue("wscale1",&CVariantEx(var));

	var = (LPCTSTR)font2;
	tab.AddValue("font2",&CVariantEx(var));

	var = size2;
	tab.AddValue("size2",&CVariantEx(var));

	var = wscale2;
	tab.AddValue("wscale2",&CVariantEx(var));
}

void CNorthPointer_25KN::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddParam("angle1",angle1,StrFromResID(IDS_MD_NP_MAGNETON_ANGLE));
	pParam->AddParam("angle2",angle2,StrFromResID(IDS_MD_NP_YCOORD_ANGLE));
}

void CNorthPointer_25KN::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"angle1",pvar) )
	{
		angle1 = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"angle2",pvar) )
	{
		angle2 = (float)(_variant_t)*pvar;
	}
}

void CNorthPointer_25KN::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = angle1;
	tab.AddValue("angle1",&CVariantEx(var));

	var = angle2;
	tab.AddValue("angle2",&CVariantEx(var));
}



void CNorthPointer_25KN::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	CFtrLayer *pLayer = NULL;

	PT_3D linePt[2];
	PT_3D textpt[2];
	memset( linePt, 0, 2*sizeof(PT_3D) );
	memset( textpt, 0, 2*sizeof(PT_3D) );

	PT_3DEX arcPt1[2];	//在每条线上截取两个点，留到后面画圆弧时用
	PT_3DEX arcPt2[4];
	PT_3DEX arcPt3[4];	

	double len = 24;		//假设三北线长度为24mm
	linePt[0].x = e2.m_xh - dx*m_pAssist->Unit();
	linePt[0].y = e.m_yl - dy*m_pAssist->Unit();
	linePt[1].x = linePt[0].x;
	linePt[1].y = linePt[0].y - len*m_pAssist->Unit();

	COPY_3DPT(arcPt1[0], linePt[0]);
	COPY_3DPT(arcPt1[1], linePt[0]);
	arcPt1[0].y = linePt[0].y - 0.20*len*m_pAssist->Unit();
	arcPt1[1].y = linePt[0].y - 0.35*len*m_pAssist->Unit();

	textpt[0].x = linePt[0].x;
	textpt[0].y = linePt[0].y - 0.37*len*m_pAssist->Unit();
	textpt[1].x = textpt[0].x;
	textpt[1].y = textpt[0].y - size1*m_pAssist->Unit()*4;

	//真子午线
	RotatePts(linePt,2,xb.cosa,xb.sina);							//线
	CGeoCurve* pGeo = m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

	RotatePts(arcPt1,2,xb.cosa,xb.sina);
	arcPt1[0].pencode = arcPt1[1].pencode = penLine;
	CGeoCurve* pTmp = (CGeoCurve*)pGeo->Clone();							//这里只为了旋转坐标方便，无其他用处
	pTmp->CreateShape(arcPt1, 2);

	CFeature *pFtrName = m_pAssist->CreateText(layer,pLayer);		//线上的名字
	CGeoText *pText = (CGeoText*)pFtrName->GetGeometry();
	if( !pText )return;

	RotatePts(textpt,2,xb.cosa,xb.sina);	
	m_pAssist->SetText(pText,textpt,2,StrFromResID(IDS_MD_NP_TRUE_MERIDIAN),font1,size1,wscale1,TAH_LEFT|TAV_TOP);

	TEXT_SETTINGS0 ts;
	pText->GetSettings(&ts);	
	ts.nPlaceType = byLineV;
	pText->SetSettings(&ts);
	m_pAssist->AddObject(pFtrName,pLayer->GetID());

	PT_3DEX ptA;													//线末端的五角星			
	COPY_3DPT( ptA, linePt[0] );
	CFeature* pFtr = m_pAssist->CreatePoint(layer, pLayer);
	if(!pFtr) return;
	CGeoPoint* ppoint = (CGeoPoint*)pFtr->GetGeometry();
	ppoint->CreateShape( &ptA, 1 );
	ppoint->SetSymbolName( "@指北针五角星" );
	ppoint->SetDirection( xb.ang*180/PI );
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	double matrix1[16];			//给磁子午线用
	double matrix2[16];			//给纵坐标线用
	double rotate_ang1 = 0;
	double rotate_ang2 = 0;
	
	if( angle1 >=0 && angle2 >= 0 )		//同在东（右）侧
	{
		if( angle1 < angle2 )
		{
			rotate_ang1 = -PI/6;
			rotate_ang2 = -PI/3;
		}
		else
		{
			rotate_ang1 = -PI/3;
			rotate_ang2 = -PI/6;
		}
	}									//同在西（左）侧
	else if( angle1 < 0 && angle2 < 0 )
	{
		if( fabs(angle1) < fabs(angle2) )
		{
			rotate_ang1 = PI/6;
			rotate_ang2 = PI/3;
		}
		else
		{
			rotate_ang1 = PI/3;
			rotate_ang2 = PI/6;
		}
	}
	else if( angle1 >= 0 && angle2 < 0 )//磁子午线在东，纵坐标线在西
	{
		rotate_ang1 = -PI/6;
		rotate_ang2 = PI/6;
	}
	else if( angle1 < 0 && angle2 >= 0 )//磁子午线在西，纵坐标线在东
	{
		rotate_ang1 = PI/6;
		rotate_ang2 = -PI/6;		
	}
	
	//磁子午线
	Matrix44FromRotate(&linePt[1],&PT_3D(0,0,1),rotate_ang1,matrix1);

	pFtr = m_pAssist->CreateCurve(layer,pLayer);		//线
	if( !pFtr ) return;
	CGeoCurve *pObj = (CGeoCurve*)pGeo->Clone();
	if( !pObj ) return;
	pObj->Transform( matrix1 );
	pFtr->SetGeometry( pObj );
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	CFeature *pFtrName1 = pFtrName->Clone();			//线上的名字
	CGeoText *pText1 = (CGeoText*)pText->Clone();
	m_pAssist->SetText(pText1,textpt,2,StrFromResID(IDS_MD_NP_MAGNETON_MERIDIAN),font1,size1,wscale1,TAH_RIGHT|TAV_TOP);
	pText1->Transform( matrix1 );
	pFtrName1->SetGeometry( pText1 );
	m_pAssist->AddObject(pFtrName1,pLayer->GetID());

	pFtr = m_pAssist->CreatePoint(layer, pLayer);		//线末端的箭头
	if(!pFtr) return;
	ppoint = (CGeoPoint*)pFtr->GetGeometry();
	ppoint->CreateShape( &ptA, 1 );
	ppoint->SetSymbolName( "@指北针箭头" );
	ppoint->Transform( matrix1 );
	ppoint->SetDirection( (xb.ang+rotate_ang1)*180/PI );
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	CGeoCurve* pTmp1 = (CGeoCurve*)pTmp->Clone();					//旋转截取的两个备用坐标
	pTmp1->Transform( matrix1 );
	COPY_3DPT( arcPt2[0], pTmp1->GetDataPoint(0) );
	COPY_3DPT( arcPt2[1], pTmp1->GetDataPoint(1) );

	Matrix44FromRotate(&linePt[1],&PT_3D(0,0,1),rotate_ang1/2,matrix1);
	pTmp1 = (CGeoCurve*)pTmp->Clone();
	pTmp1->Transform( matrix1 );
	COPY_3DPT( arcPt2[2], pTmp1->GetDataPoint(0) );
	COPY_3DPT( arcPt2[3], pTmp1->GetDataPoint(1) );

	//纵坐标线
	Matrix44FromRotate(&linePt[1],&PT_3D(0,0,1),rotate_ang2,matrix2);

	pFtr = m_pAssist->CreateCurve(layer,pLayer);		//线
	if( !pFtr ) return;
	pObj = (CGeoCurve*)pGeo->Clone();
	if( !pObj ) return;
	pObj->Transform( matrix2 );
	pFtr->SetGeometry( pObj );
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	CFeature *pFtrName2 = pFtrName->Clone();			//线上的名字
	CGeoText *pText2 = (CGeoText*)pText->Clone();
	m_pAssist->SetText(pText2,textpt,2,StrFromResID(IDS_MD_NP_YCOORD_LINE),font1,size1,wscale1,TAH_LEFT|TAV_TOP);
	pText2->Transform( matrix2 );
	pFtrName2->SetGeometry( pText2 );
	m_pAssist->AddObject(pFtrName2,pLayer->GetID());

	pFtr = m_pAssist->CreatePoint(layer, pLayer);		//线末端的箭头
	if(!pFtr) return;
	ppoint = (CGeoPoint*)pFtr->GetGeometry();
	ppoint->CreateShape( &ptA, 1 );
	ppoint->SetSymbolName( "@指北针箭头" );
	ppoint->Transform( matrix2 );
	ppoint->SetDirection( (xb.ang+rotate_ang2+PI)*180/PI );
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	CGeoCurve* pTmp2 = (CGeoCurve*)pTmp->Clone();					//旋转截取的两个备用坐标
	pTmp2->Transform( matrix2 );
	COPY_3DPT( arcPt3[0], pTmp2->GetDataPoint(0) );
	COPY_3DPT( arcPt3[1], pTmp2->GetDataPoint(1) );

	Matrix44FromRotate(&linePt[1],&PT_3D(0,0,1),rotate_ang2/2,matrix2);
	pTmp2 = (CGeoCurve*)pTmp->Clone();
	pTmp2->Transform( matrix2 );
	COPY_3DPT( arcPt3[2], pTmp2->GetDataPoint(0) );
	COPY_3DPT( arcPt3[3], pTmp2->GetDataPoint(1) );


	double matrix3[16];
	PT_3DEX arcPt4[2];

	Matrix44FromRotate(&linePt[1],&PT_3D(0,0,1),(rotate_ang1+rotate_ang2)/2,matrix3);
	pTmp2 = (CGeoCurve*)pTmp->Clone();
	pTmp2->Transform( matrix3 );
	COPY_3DPT( arcPt4[0], pTmp2->GetDataPoint(0) );
	COPY_3DPT( arcPt4[1], pTmp2->GetDataPoint(1) );

	//圆弧
	if( (angle1>=0 && angle2>=0) || (angle1<0 && angle2<0) )
	{//在同侧时
		if( fabs(angle1) < fabs(angle2) )
		{
			//1.磁子午线与真子午线间的夹角与圆弧
			PT_3DEX arcpts[3];
			arcpts[0].pencode = arcpts[1].pencode = arcpts[2].pencode = pen3PArc;
			
			COPY_3DPT( arcpts[0], arcPt1[1] );
			COPY_3DPT( arcpts[1], arcPt2[3] );
			COPY_3DPT( arcpts[2], arcPt2[1] );
			
			float included_ang = angle1;
			float text_ang = (xb.ang+rotate_ang1/2)*180/PI;
			AddArcAndNote(arcpts, included_ang, text_ang);

			//2.纵坐标线与真子午线间的夹角与圆弧
			COPY_3DPT( arcpts[0], arcPt1[0] );
			COPY_3DPT( arcpts[1], arcPt3[2] );
			COPY_3DPT( arcpts[2], arcPt3[0] );
			
			included_ang = angle2;
			text_ang = (xb.ang+rotate_ang2/2)*180/PI;
			AddArcAndNote(arcpts, included_ang, text_ang);
			
			//3.磁子午线与纵坐标线间的夹角与圆弧
			COPY_3DPT( arcpts[0], arcPt2[1] );
			COPY_3DPT( arcpts[1], arcPt4[1] );
			COPY_3DPT( arcpts[2], arcPt3[1] );
			
			included_ang = fabs( AddAngle(angle1, -angle2) );
			text_ang = (xb.ang+(rotate_ang1+rotate_ang2)/2)*180/PI;
			AddArcAndNote(arcpts, included_ang, text_ang);
		}
		else
		{
			//1.磁子午线与真子午线间的夹角与圆弧
			PT_3DEX arcpts[3];
			arcpts[0].pencode = arcpts[1].pencode = arcpts[2].pencode = pen3PArc;
			
			COPY_3DPT( arcpts[0], arcPt1[0] );
			COPY_3DPT( arcpts[1], arcPt2[2] );
			COPY_3DPT( arcpts[2], arcPt2[0] );
			
			float included_ang = angle1;
			float text_ang = (xb.ang+rotate_ang1/2)*180/PI;
			AddArcAndNote(arcpts, included_ang, text_ang);

			//2.纵坐标线与真子午线间的夹角与圆弧
			COPY_3DPT( arcpts[0], arcPt1[1] );
			COPY_3DPT( arcpts[1], arcPt3[3] );
			COPY_3DPT( arcpts[2], arcPt3[1] );
			
			included_ang = angle2;
			text_ang = (xb.ang+rotate_ang2/2)*180/PI;
			AddArcAndNote(arcpts, included_ang, text_ang);

			//3.磁子午线与纵坐标线间的夹角与圆弧
			COPY_3DPT( arcpts[0], arcPt2[1] );
			COPY_3DPT( arcpts[1], arcPt4[1] );
			COPY_3DPT( arcpts[2], arcPt3[1] );
			
			included_ang = fabs( AddAngle(angle1, -angle2) );
			text_ang = (xb.ang+(rotate_ang1+rotate_ang2)/2)*180/PI;
			AddArcAndNote(arcpts, included_ang, text_ang);
		}
	}
	else
	{//在异侧时
		//1.磁子午线与真子午线间的夹角与圆弧
		PT_3DEX arcpts[3];
		arcpts[0].pencode = arcpts[1].pencode = arcpts[2].pencode = pen3PArc;

		COPY_3DPT( arcpts[0], arcPt1[0] );
		COPY_3DPT( arcpts[1], arcPt2[2] );
		COPY_3DPT( arcpts[2], arcPt2[0] );
		
		float included_ang = angle1;
		float text_ang = (xb.ang+rotate_ang1/2)*180/PI;
		AddArcAndNote(arcpts, included_ang, text_ang);

		//2.纵坐标线与真子午线间的夹角与圆弧
		COPY_3DPT( arcpts[0], arcPt1[0] );
		COPY_3DPT( arcpts[1], arcPt3[2] );
		COPY_3DPT( arcpts[2], arcPt3[0] );
		
		included_ang = angle2;
		text_ang = (xb.ang+rotate_ang2/2)*180/PI;
		AddArcAndNote(arcpts, included_ang, text_ang);

		//3.磁子午线与纵坐标线间的夹角与圆弧
		COPY_3DPT( arcpts[0], arcPt2[1] );
		COPY_3DPT( arcpts[1], arcPt1[1] );
		COPY_3DPT( arcpts[2], arcPt3[1] );
		
		included_ang = AddAngle( fabs(angle1), fabs(angle2) );
		text_ang = xb.ang*180/PI;
		AddArcAndNote(arcpts, included_ang, text_ang);
	}

}

void CNorthPointer_25KN::AddArcAndNote( PT_3DEX pts[3], float included_angle, float text_angle)
{//这里角的单位全部为度
	CFtrLayer *pLayer = NULL;	
		
	CFeature* pArc = m_pAssist->CreateCurve( layer, pLayer );
	if( !pArc ) return;
	CGeoCurve *pArcObj = (CGeoCurve*)pArc->GetGeometry();
	if( !pArcObj ) return;
	pArcObj->CreateShape( pts, 3 );
	pArcObj->SetSymbolName( "@2x2" );
	m_pAssist->AddObject( pArc, pLayer->GetID() );
	
	CFeature *pNote = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pNote->GetGeometry();
	if( !pText )return;	
	
	PT_3D textpt;
	COPY_3DPT(textpt, pts[1]);

	int x = int(included_angle);
	float temp = ( fabs(included_angle)-abs(x) )*100 + 1e-4;
	int y = int(temp);

	CString text;
	text.Format("%d",x);
	text += StrFromResID(IDS_MD_DEGREE);
	CString t1;
	t1.Format("%d'",y);
	text += t1;

	m_pAssist->SetText(pText,&textpt,1,text,font2,size2,wscale2,TAH_MID|TAV_BOTTOM);		
	
	TEXT_SETTINGS0 ts;
	pText->GetSettings(&ts);
	ts.fTextAngle = text_angle;
	pText->SetSettings(&ts);
	m_pAssist->AddObject(pNote,pLayer->GetID());
}

float CNorthPointer_25KN::AddAngle( float angle1, float angle2 )
{
	//1.全部换算成分
	int x1 = int(angle1);
	float tol1 = 1e-4;
	if( angle1 < 0 ) tol1 *= -1;
	float temp1 = (angle1-x1)*100 + tol1;
	int y1 = int(temp1);
	int t1 = x1*60 + y1;

	int x2 = int(angle2);
	float tol2 = 1e-4;
	if( angle2 < 0 ) tol2 *= -1;
	float temp2 = (angle2-x2)*100 + tol2;
	int y2 = int(temp2);
	int t2 = x2*60 + y2;

	//2.相加
	int t = t1 + t2;

	//3.换算成度
	int x = t / 60;
	int y = t % 60;

	float ret = x + y/100.0;
	return ret;
}
//




CMagline_25KN::CMagline_25KN()
{
	m_strName = StrFromResID(IDS_MD_MAGNETIC_LINE);
	strcpy(layer,"TK_magline");

	angle = -3;

	strcpy(font,StrFromResID(IDS_MD_XIDENGXIANTI));
	size = 2.5;
	wscale = 1;
}


CMagline_25KN::~CMagline_25KN()
{
}

void CMagline_25KN::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));
	
}

void CMagline_25KN::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
}

void CMagline_25KN::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
}

void CMagline_25KN::GetUIParams_data(CUIParam *params)
{
	params->AddParam("angle",angle,StrFromResID(IDS_MD_MAGLINE_ANGLE));
}

void CMagline_25KN::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"angle",pvar) )
	{
		angle = (float)(_variant_t)*pvar;
	}
}

void CMagline_25KN::GetData(CValueTable& tab)
{
	_variant_t var;
	
	var = angle;
	tab.AddValue("angle",&CVariantEx(var));
}

void CMagline_25KN::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	PT_3D ptSouth;	//磁南点
	PT_3D ptNorth;	//磁北点

//1.算出三北线的位置
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	CNorthPointer_25KN *pNorth = (CNorthPointer_25KN*)m_pMain->GetObj( "NorthPointer_25KN" );
	if( !pNorth ) return;
	PT_3D expt = PT_3D(0,0,0);
	expt.x = e2.m_xh - (pNorth->dx)*m_pAssist->Unit();
	expt.y = e.m_yl - (pNorth->dy)*m_pAssist->Unit();
	RotatePts(&expt,1,xb.cosa,xb.sina);		//三北线的位置

//2.算出磁南点的位置
	CGrid *pGrid = (CGrid*)m_pMain->GetObj( "Grid_New" );
	if( !pGrid ) return;
	float gridwid = (pGrid->grid_wid)*m_pAssist->Unit();

	double sx = floor(expt.x/gridwid)*gridwid;
	
	PT_3D pt0 = m_pMain->m_innerBound[0];
	PT_3D pt1 = m_pMain->m_innerBound[1];
	GraphAPI::GGetLineIntersectLine(sx,0,0,1000,
		pt0.x,pt0.y,pt1.x-pt0.x,pt1.y-pt0.y,&ptSouth.x,&ptSouth.y,NULL);

//3.计算磁北点的位置
	double vx = sin( angle*PI/180 );	//方向向量
	double vy = cos( angle*PI/180 );
	pt0 = m_pMain->m_innerBound[3];
	pt1 = m_pMain->m_innerBound[2];
	GraphAPI::GGetLineIntersectLine(ptSouth.x,ptSouth.y,vx,vy,
		pt0.x,pt0.y,pt1.x-pt0.x,pt1.y-pt0.y,&ptNorth.x,&ptNorth.y,NULL);

//4.文字
	CFtrLayer *pLayer = NULL;	
	CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	m_pAssist->SetText(pText,&ptSouth,1,StrFromResID(IDS_MD_MAGLINE_SOUTH_POINT),font,size,wscale,TAH_LEFT|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
	
	pFtr = m_pAssist->CreateText(layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	m_pAssist->SetText(pText,&ptNorth,1,StrFromResID(IDS_MD_MAGLINE_NORTH_POINT),font,size,wscale,TAH_RIGHT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());

//5.磁北线
//	m_pAssist->AddLine(layer,ptNorth,ptSouth,0);
	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	
	PT_3DEX pts[2];
	pts[0].pencode = pts[1].pencode = penLine;
	COPY_3DPT(pts[0],ptNorth);
	COPY_3DPT(pts[1],ptSouth);	
	pGeo->CreateShape(pts,2);
	pGeo->SetSymbolName( "@5.0X5.0" );
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



CScale_5KN::CScale_5KN()
{
	strcpy(scaleText,"1:5000");

	m_strName = StrFromResID(IDS_MD_SCALE);
	strcpy(font,StrFromResID(IDS_MD_SONGTI));
	strcpy(layer,"TK_scale");
	size = 4;
	wscale = 1;
	dy_scaleRuler = 15;
	dy = 5;
	dx = 90;
	strcpy(scaleLine_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	scaleLine_size = 1.6;

	dy_text = 2;
	strcpy(text_font, StrFromResID(IDS_MD_BIANSONGTI));
	text_size = 2;
	text_wscale = 1.375;
}


CScale_5KN::~CScale_5KN()
{
}

void CScale_5KN::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromResID(IDS_MD_ST_FONTNAME));
	pParam->AddParam("size",size,StrFromResID(IDS_MD_ST_FONTSIZE));
	pParam->AddParam("wscale",wscale,StrFromResID(IDS_MD_ST_FONTWSCALE));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_INNER_RSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_ST_TSPACE));
	pParam->AddParam("dy_scaleRuler",dy_scaleRuler,StrFromResID(IDS_MD_SB_TSPACE));
	pParam->AddFontNameParam("scaleLine_font",scaleLine_font,StrFromResID(IDS_SCALELINE_FONT));
	pParam->AddParam("scaleLine_size",scaleLine_size,StrFromResID(IDS_SCALELINE_FONTSIZE));
}

void CScale_5KN::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy_scaleRuler",pvar) )
	{
		dy_scaleRuler = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"scaleLine_font",pvar) )
	{
		strncpy(scaleLine_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(scaleLine_font)-1);
	}
	if( tab.GetValue(0,"scaleLine_size",pvar) )
	{
		scaleLine_size = (float)(_variant_t)*pvar;
	}
}

void CScale_5KN::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = dy_scaleRuler;
	tab.AddValue("dy_scaleRuler",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));

	var = dy;
	tab.AddValue("dy",&CVariantEx(var));

	var = (LPCTSTR)scaleLine_font;
	tab.AddValue("scaleLine_font",&CVariantEx(var));

	var = scaleLine_size;
	tab.AddValue("scaleLine_size",&CVariantEx(var));
}

void CScale_5KN::GetUIParams_data(CUIParam *pParam)
{
}

void CScale_5KN::SetData(CValueTable& tab)
{
}

void CScale_5KN::GetData(CValueTable& tab)
{
}

void CScale_5KN::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);
	
	//创建比例尺中的图号文字
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;
	textpt.x = e2.m_xh - (dx+50)*m_pAssist->Unit(); 
	textpt.y = e.m_yl-dy*m_pAssist->Unit();
	textpt.z = 0;

	if( 1 )
	{		
		CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
		if( !pFtr )return;

		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();

		CString text;
		text.Format("1:%d",m_pMain->m_scale);

		RotatePts(&textpt,1,xb.cosa,xb.sina);
		
		m_pAssist->SetText(pText,&textpt,1,text,font,size,wscale,TAH_MID|TAV_TOP);
		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	if( 1 )
	{
		PT_3D ptstart;

		ptstart.x = e2.m_xh - (dx+100)*m_pAssist->Unit(); 
		ptstart.y = e.m_yl-dy_scaleRuler*m_pAssist->Unit();
		ptstart.z = 0;

		PT_3DEX expts[2];
		expts[0].pencode = expts[1].pencode = penLine;

		//底线
		CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
		if( !pFtr )return;
		
		CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
		COPY_3DPT(expts[0],ptstart);
		COPY_3DPT(expts[1],ptstart);

		expts[1].x += 100 * m_pAssist->Unit(); 

		RotatePts(expts,2,xb.cosa,xb.sina);

		//pGeo->m_fLineWidth = 0.6;
		pGeo->CreateShape(expts,2);

		m_pAssist->AddObject(pFtr,pLayer->GetID());

		//底线2
		pFtr = m_pAssist->CreateCurve(layer,pLayer);
		if( !pFtr )return;
		
		pGeo = (CGeoCurve*)pFtr->GetGeometry();
		COPY_3DPT(expts[0],ptstart);
		COPY_3DPT(expts[1],ptstart);

		expts[1].x += 100 * m_pAssist->Unit(); 
		
		expts[0].y += 1.0 * m_pAssist->Unit(); 
		expts[1].y += 1.0 * m_pAssist->Unit(); 

		RotatePts(expts,2,xb.cosa,xb.sina);
		
		pGeo->CreateShape(expts,2);
		
		m_pAssist->AddObject(pFtr,pLayer->GetID());

		//刻度线和长度注记
	
		for( int i=0; i<11; i++)
		{
			//刻度线
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();

			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);	

			expts[0].x += i*10 * m_pAssist->Unit(); 
			expts[1].x += i*10 * m_pAssist->Unit(); 

			expts[1].y = ptstart.y + 2.0 * m_pAssist->Unit();


			RotatePts(expts,2,xb.cosa,xb.sina);
			
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());

			//长度注记
			if( (i%2)==0 || i==1 )
			{
				pFtr = m_pAssist->CreateText(layer,pLayer);
				if( !pFtr )continue;
				
				CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
				
				CString text;
				text.Format("%d",10*(int)abs(i-2)*(int)m_pAssist->Unit());
				
				textpt = ptstart;
				textpt.x += i*10 * m_pAssist->Unit(); 

				textpt.y = ptstart.y + 2.5 * m_pAssist->Unit();

				RotatePts(&textpt,1,xb.cosa,xb.sina);
				
				m_pAssist->SetText(pText,&textpt,1,text,scaleLine_font,scaleLine_size,0,TAH_MID|TAV_BOTTOM);
				m_pAssist->SetText(pText,xb.ang*180/PI);
				m_pAssist->AddObject(pFtr,pLayer->GetID());

				textpt = ptstart;
				textpt.x += i*10 * m_pAssist->Unit(); 
				
				textpt.y = ptstart.y + 2.5 * m_pAssist->Unit();

				if( i==0 )
				{
					pFtr = m_pAssist->CreateText(layer,pLayer);
					if( !pFtr )continue;
					
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();					
					
					textpt.x -= 2*m_pAssist->Unit();

					RotatePts(&textpt,1,xb.cosa,xb.sina);
					
					m_pAssist->SetText(pText,&textpt,1,"m",scaleLine_font,scaleLine_size,0,TAH_RIGHT|TAV_BOTTOM);
					m_pAssist->SetText(pText,xb.ang*180/PI);
					m_pAssist->AddObject(pFtr,pLayer->GetID());
				}
				else if( i==10 )
				{
					pFtr = m_pAssist->CreateText(layer,pLayer);
					if( !pFtr )continue;
					
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
					
					textpt.x += 2*m_pAssist->Unit();

					RotatePts(&textpt,1,xb.cosa,xb.sina);
					
					m_pAssist->SetText(pText,&textpt,1,"m",scaleLine_font,scaleLine_size,0,TAH_LEFT|TAV_BOTTOM);
					m_pAssist->SetText(pText,xb.ang*180/PI);
					m_pAssist->AddObject(pFtr,pLayer->GetID());
				}

			}
		}

		//小刻度尺

		for( i=2; i<10; i+=2)
		{
			//粗横线
			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);
			
			expts[0].y += 0.25 *m_pAssist->Unit();
			expts[1].y += 0.25 *m_pAssist->Unit();
			
			expts[0].x += i*10*m_pAssist->Unit();
			expts[1].x += (i+1)*10*m_pAssist->Unit();
			
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();
			pGeo->m_fLineWidth = 0.5;
			
			RotatePts(expts,2,xb.cosa,xb.sina);
			
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());
		}

		for( i=0; i<10; i+=2)
		{
			//粗横线
			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);

			expts[0].y += 0.25 *m_pAssist->Unit();
			expts[1].y += 0.25 *m_pAssist->Unit();

			expts[0].x += i*2*m_pAssist->Unit();
			expts[1].x += (i+1)*2*m_pAssist->Unit();
			
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();
			pGeo->m_fLineWidth = 0.5;

			RotatePts(expts,2,xb.cosa,xb.sina);
						
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());

			//细竖线
			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);
			
			expts[1].y += 1.7*m_pAssist->Unit();
			
			expts[0].x += (i+1)*2*m_pAssist->Unit();
			expts[1].x += (i+1)*2*m_pAssist->Unit();
			
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();
			
			RotatePts(expts,2,xb.cosa,xb.sina);
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());

			if( i!=0 )
			{
				COPY_3DPT(expts[0],ptstart);
				COPY_3DPT(expts[1],ptstart);
				
				expts[1].y += 1.5*m_pAssist->Unit();
				
				expts[0].x += i*2*m_pAssist->Unit();
				expts[1].x += i*2*m_pAssist->Unit();
				
				pFtr = m_pAssist->CreateCurve(layer,pLayer);
				if( !pFtr )continue;
				
				pGeo = (CGeoCurve*)pFtr->GetGeometry();
				
				RotatePts(expts,2,xb.cosa,xb.sina);
				pGeo->CreateShape(expts,2);
				
				m_pAssist->AddObject(pFtr,pLayer->GetID());
			}

		}
	}

	if(1)
	{
		textpt.x = e2.m_xh - (dx+50)*m_pAssist->Unit(); 
		textpt.y = e.m_yl-(dy_scaleRuler+dy_text)*m_pAssist->Unit();
		
		CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
		if( !pFtr )return;	
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		
		CString text;
		text.Format("图上1cm相当于实地%dm",int(m_pMain->m_scale*0.01));
		
		RotatePts(&textpt,1,xb.cosa,xb.sina);
		
		m_pAssist->SetText(pText,&textpt,1,text,text_font,text_size,text_wscale,TAH_RIGHT|TAV_TOP);
		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}
}






CScale_25KN::CScale_25KN()
{
	strcpy(scaleText,"1:5000");

	m_strName = StrFromResID(IDS_MD_SCALE);
	strcpy(font,StrFromResID(IDS_MD_SONGTI));
	strcpy(layer,"TK_scale");
	size = 4;
	wscale = 1;
	dy_scaleRuler = 21;
	dy = 5;
	dx = 90;
	strcpy(scaleLine_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	scaleLine_size = 1.6;

	dy_text = 2;
	strcpy(text_font, StrFromResID(IDS_MD_BIANSONGTI));
	text_size = 2;
	text_wscale = 1.375;

	if(!MapDecorate::bMapDecorateNew)
	{
		size = 5;
		scaleLine_size = 2;
		dy = 4;
		dy_scaleRuler = 17;
	}
}


CScale_25KN::~CScale_25KN()
{
}

void CScale_25KN::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddFontNameParam("font",font,StrFromResID(IDS_MD_ST_FONTNAME));
	pParam->AddParam("size",size,StrFromResID(IDS_MD_ST_FONTSIZE));
	pParam->AddParam("wscale",wscale,StrFromResID(IDS_MD_ST_FONTWSCALE));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_RULER_LSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_ST_TSPACE));
	pParam->AddParam("dy_scaleRuler",dy_scaleRuler,StrFromResID(IDS_MD_SB_TSPACE));
	pParam->AddFontNameParam("scaleLine_font",scaleLine_font,StrFromResID(IDS_SCALELINE_FONT));
	pParam->AddParam("scaleLine_size",scaleLine_size,StrFromResID(IDS_SCALELINE_FONTSIZE));
}

void CScale_25KN::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy_scaleRuler",pvar) )
	{
		dy_scaleRuler = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"scaleLine_font",pvar) )
	{
		strncpy(scaleLine_font,(LPCTSTR)(_bstr_t)*pvar,sizeof(scaleLine_font)-1);
	}
	if( tab.GetValue(0,"scaleLine_size",pvar) )
	{
		scaleLine_size = (float)(_variant_t)*pvar;
	}
}

void CScale_25KN::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));
	
	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));
	
	var = dy_scaleRuler;
	tab.AddValue("dy_scaleRuler",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));

	var = dy;
	tab.AddValue("dy",&CVariantEx(var));

	var = (LPCTSTR)scaleLine_font;
	tab.AddValue("scaleLine_font",&CVariantEx(var));

	var = scaleLine_size;
	tab.AddValue("scaleLine_size",&CVariantEx(var));
}

void CScale_25KN::GetUIParams_data(CUIParam *pParam)
{
}

void CScale_25KN::SetData(CValueTable& tab)
{
}

void CScale_25KN::GetData(CValueTable& tab)
{
}

void CScale_25KN::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	//获取坡度尺的位置，计算比例尺的位置
	CRuler_25KN* pRuler = (CRuler_25KN*)m_pMain->GetObj("Ruler_25KN");
	if( !pRuler ) return;
	
	//创建比例尺中的图号文字
	CFtrLayer *pLayer = NULL;	
	
	PT_3D textpt;
	textpt.x = e.m_xl+( pRuler->dx + 29*(pRuler->x_interv) + this->dx + 25)*m_pAssist->Unit();	//比例尺总长度50mm
	textpt.y = e.m_yl-dy*m_pAssist->Unit();
	textpt.z = 0;

	if( 1 )
	{		
		CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
		if( !pFtr )return;

		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();

		CString text;
		text.Format("1:%d",m_pMain->m_scale);

		RotatePts(&textpt,1,xb.cosa,xb.sina);
		
		m_pAssist->SetText(pText,&textpt,1,text,font,size,wscale,TAH_MID|TAV_TOP);
		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	if( 1 )
	{
		PT_3D ptstart;

		ptstart.x = e.m_xl+( pRuler->dx + 29*(pRuler->x_interv) + this->dx )*m_pAssist->Unit();
		ptstart.y = e.m_yl-dy_scaleRuler*m_pAssist->Unit();
		ptstart.z = 0;

		PT_3DEX expts[2];
		expts[0].pencode = expts[1].pencode = penLine;

		//底线
		CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
		if( !pFtr )return;
		
		CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
		COPY_3DPT(expts[0],ptstart);
		COPY_3DPT(expts[1],ptstart);

		expts[0].y += 0.2 * m_pAssist->Unit();
		expts[1].y += 0.2 * m_pAssist->Unit();
		expts[1].x += 50 * m_pAssist->Unit(); 

		RotatePts(expts,2,xb.cosa,xb.sina);

		pGeo->m_fLineWidth = 0.5;
		pGeo->CreateShape(expts,2);

		m_pAssist->AddObject(pFtr,pLayer->GetID());

		//底线2
		pFtr = m_pAssist->CreateCurve(layer,pLayer);
		if( !pFtr )return;
		
		pGeo = (CGeoCurve*)pFtr->GetGeometry();
		COPY_3DPT(expts[0],ptstart);
		COPY_3DPT(expts[1],ptstart);

		expts[1].x += 50 * m_pAssist->Unit(); 
		
		expts[0].y += 2 * m_pAssist->Unit(); 
		expts[1].y += 2 * m_pAssist->Unit(); 

		RotatePts(expts,2,xb.cosa,xb.sina);
		
		pGeo->CreateShape(expts,2);
		
		m_pAssist->AddObject(pFtr,pLayer->GetID());

		//刻度线和长度注记
		
		for( int i=0; i<6; i++)
		{
			//刻度线
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();

			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);	

			expts[0].x += i*10 * m_pAssist->Unit();	
			expts[1].x += i*10 * m_pAssist->Unit();	
			
			if( (i%2)==1 || 0==i )
			{
				expts[1].y = ptstart.y + 2.5 * m_pAssist->Unit();
			}
			else
			{
				expts[1].y = ptstart.y + 2.0 * m_pAssist->Unit();
			}

			RotatePts(expts,2,xb.cosa,xb.sina);
			
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());

			//长度注记
			if( i==0 || i==1 ||i==5 )
			{
				pFtr = m_pAssist->CreateText(layer,pLayer);
				if( !pFtr )continue;
				
				CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
				
				CString text;
				if( i==0 || i==1 )
					text.Format("%d",10*(int)abs(i-1)*(int)m_pAssist->Unit());			//250 500
				else if( i==5 )
					text.Format("%dkm",int(0.01*(int)abs(i-1)*(int)m_pAssist->Unit()) );		//1km
				
				textpt = ptstart;
				textpt.x += i*10 * m_pAssist->Unit();

				textpt.y = ptstart.y + 3.0 * m_pAssist->Unit();

				RotatePts(&textpt,1,xb.cosa,xb.sina);
				
				m_pAssist->SetText(pText,&textpt,1,text,scaleLine_font,scaleLine_size,0,TAH_MID|TAV_BOTTOM);
				m_pAssist->SetText(pText,xb.ang*180/PI);
				m_pAssist->AddObject(pFtr,pLayer->GetID());

				textpt = ptstart;
				textpt.x += i*10 * m_pAssist->Unit();	
				
				textpt.y = ptstart.y + 3.0 * m_pAssist->Unit();

				if( i==0 )
				{
					pFtr = m_pAssist->CreateText(layer,pLayer);
					if( !pFtr )continue;
					
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();					
					
					textpt.x -= 2*m_pAssist->Unit();

					RotatePts(&textpt,1,xb.cosa,xb.sina);
					
					m_pAssist->SetText(pText,&textpt,1,"m",scaleLine_font,scaleLine_size,0,TAH_RIGHT|TAV_BOTTOM);
					m_pAssist->SetText(pText,xb.ang*180/PI);
					m_pAssist->AddObject(pFtr,pLayer->GetID());
				}
				else if( i==10 )
				{
					pFtr = m_pAssist->CreateText(layer,pLayer);
					if( !pFtr )continue;
					
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
					
					textpt.x += 2*m_pAssist->Unit();

					RotatePts(&textpt,1,xb.cosa,xb.sina);
					
					m_pAssist->SetText(pText,&textpt,1,"m",scaleLine_font,scaleLine_size,0,TAH_LEFT|TAV_BOTTOM);
					m_pAssist->SetText(pText,xb.ang*180/PI);
					m_pAssist->AddObject(pFtr,pLayer->GetID());
				}

			}
		}

		//小刻度尺

		for( i=0; i<10; i++)
		{
			if( i <= 4 && 0==(i%2) )
			{
				//粗横线
				COPY_3DPT(expts[0],ptstart);
				COPY_3DPT(expts[1],ptstart);
				
				expts[0].y += 0.5 *m_pAssist->Unit();
				expts[1].y += 0.5 *m_pAssist->Unit();
				
				expts[0].x += i*2*m_pAssist->Unit();
				expts[1].x += (i+1)*2*m_pAssist->Unit();
				
				pFtr = m_pAssist->CreateCurve(layer,pLayer);
				if( !pFtr )continue;
				
				pGeo = (CGeoCurve*)pFtr->GetGeometry();
				pGeo->m_fLineWidth = 1.0;
				
				RotatePts(expts,2,xb.cosa,xb.sina);
				
				pGeo->CreateShape(expts,2);
				
				m_pAssist->AddObject(pFtr,pLayer->GetID());
			}
			//细竖线
			COPY_3DPT(expts[0],ptstart);
			COPY_3DPT(expts[1],ptstart);
			
			if( 0 == (i%2) )
				expts[1].y += 2.5*m_pAssist->Unit();
			else
				expts[1].y += 2.0*m_pAssist->Unit();
			
			expts[0].x += i*m_pAssist->Unit();
			expts[1].x += i*m_pAssist->Unit();
			
			pFtr = m_pAssist->CreateCurve(layer,pLayer);
			if( !pFtr )continue;
			
			pGeo = (CGeoCurve*)pFtr->GetGeometry();
			
			RotatePts(expts,2,xb.cosa,xb.sina);
			pGeo->CreateShape(expts,2);
			
			m_pAssist->AddObject(pFtr,pLayer->GetID());
		}
	}

	if(1)
	{
		textpt.x = e.m_xl+( pRuler->dx + 29*(pRuler->x_interv) + this->dx + 25)*m_pAssist->Unit();
		textpt.y = e.m_yl-(dy_scaleRuler+dy_text)*m_pAssist->Unit();
		
		CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
		if( !pFtr )return;	
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		
		CString text;
		text.Format("图上1cm相当于实地%dm",int(m_pMain->m_scale*0.01));
		
		RotatePts(&textpt,1,xb.cosa,xb.sina);
		
		m_pAssist->SetText(pText,&textpt,1,text,text_font,text_size,text_wscale,TAH_MID|TAV_TOP);
		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}
}


//
CRuler_25KN::CRuler_25KN()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_GRADE);
	strcpy(layer,"TK_ruler");
	dy = 25;
	dx = 95;
	dy2 = 3;

	strcpy(title_font,StrFromResID(IDS_MD_BIANSONGTI));
	title_size = 2.5;
	title_wscale = 1.4;

	strcpy(note_font,StrFromResID(IDS_MD_BIANSONGTI));
	note_size = 2;
	note_wscale = 1.375;

	strcpy(cal_font,StrFromResID(IDS_MD_ZHONGDENGXIANTI));
	cal_size = 1.8;
	cal_wscale = 1;

	x_interv = 2;
	con_interv = 5;

	if(!MapDecorate::bMapDecorateNew)
	{
		title_size = 3.5;
		note_size = 3;
		cal_size = 2.25;
	}

	if( 100000 == scale )
	{
		con_interv = 10;
	}
}


CRuler_25KN::~CRuler_25KN()
{
}

void CRuler_25KN::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_BSPACE));
	pParam->AddParam("con_interv",con_interv,StrFromResID(IDS_CONTOUR_INTERVAL));
	
}

void CRuler_25KN::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"con_interv",pvar) )
	{
		con_interv = (float)(_variant_t)*pvar;
	}
}

void CRuler_25KN::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));

	var = con_interv;
	tab.AddValue("con_interv",&CVariantEx(var));

}


void CRuler_25KN::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;
	
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	
	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	CFtrLayer *pLayer = NULL;

	//横线
	PT_3D linePt[2];

	linePt[0].x = e.m_xl + dx*m_pAssist->Unit();
	linePt[0].y = e.m_yl - dy*m_pAssist->Unit();
	linePt[0].z = 0;
	linePt[1].x = linePt[0].x + 29*x_interv*m_pAssist->Unit();
	linePt[1].y = linePt[0].y;
	linePt[1].z = 0;

	RotatePts(linePt,2,xb.cosa,xb.sina);
	m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

	//刻度
	CArray<PT_3DEX,PT_3DEX> pts_bottom;
	CArray<PT_3DEX,PT_3DEX> pts_top;		//相邻两条等高线

	CArray<PT_3DEX,PT_3DEX> pts_top1;		//这4根为相邻六条等高线
	CArray<PT_3DEX,PT_3DEX> pts_top2;
	CArray<PT_3DEX,PT_3DEX> pts_top3;
	CArray<PT_3DEX,PT_3DEX> pts_top4;

	int i = 0;
	for(i = 1; i <= 30; i++)
	{
		PT_3D pts[2];
		pts[0].x = e.m_xl + (dx+(i-1)*x_interv)*m_pAssist->Unit();
		pts[0].y = e.m_yl - dy*m_pAssist->Unit();		
		pts[0].z = 0;
		
		PT_3DEX tpt0;
		COPY_3DPT(tpt0,pts[0]);
		pts_bottom.Add( tpt0 );


		float ctgx = 1 / ( tan( i*PI/180 ) ); 
		pts[1].x = pts[0].x;
		pts[1].y = pts[0].y + ctgx*con_interv;
		pts[1].z = 0;

		PT_3DEX tpt;
		tpt.pencode = penSpline;
		COPY_3DPT(tpt,pts[1]);
		pts_top.Add( tpt );

		if( i >= 5 )
		{
			PT_3DEX tpt1 = tpt;
			tpt1.y = tpt.y + ctgx*con_interv;
			pts_top1.Add( tpt1 );

			PT_3DEX tpt2 = tpt;
			tpt2.y = tpt.y + 2*ctgx*con_interv;
			pts_top2.Add( tpt2 );

			PT_3DEX tpt3 = tpt;
			tpt3.y = tpt.y + 3*ctgx*con_interv;
			pts_top3.Add( tpt3 );

			PT_3DEX tpt4 = tpt;
			tpt4.y = tpt.y + 4*ctgx*con_interv;
			pts_top4.Add( tpt4 );
		}

		if( i == 1 || (i % 2 == 0) )
		{
			PT_3D textpt;
			textpt.x = pts[0].x + 0.5*m_pAssist->Unit();
			textpt.y = pts[0].y - 0.3*m_pAssist->Unit();
			textpt.z = 0;
			
			RotatePts(&textpt,1,xb.cosa,xb.sina);
			
			CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
			CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
			if( !pText )return;

			CString cal_text;
			cal_text.Format("%d",i);
			cal_text += StrFromResID(IDS_MD_DEGREE);
			if(MapDecorate::bMapDecorateNew)
				m_pAssist->SetText(pText,&textpt,1,cal_text,cal_font,1.6,cal_wscale,TAH_MID|TAV_TOP);
			else
				m_pAssist->SetText(pText,&textpt,1,cal_text,cal_font,cal_size,cal_wscale,TAH_MID|TAV_TOP);
			m_pAssist->SetText(pText,xb.ang*180/PI);
			m_pAssist->AddObject(pFtr,pLayer->GetID());
		}
	}

	//百分比
	for(i = 2; i <= 30; i+=2)
	{
		CString cal_text;
		if( i>=14 && i<=22 )
		{
			i += 2;			
		}
		else if( i >= 26 )
		{
			i += 4;
		}
		float percent = 100*tan(i*PI/180);
		int per = int(percent+0.5);
		cal_text.Format("%d",per);

		if( i == 2 )
		{
			float percent = 100*tan(i*PI/180);
			float per = float( (int(percent*10+0.5))/10.0 );
			cal_text.Format("%.1f",per);
		}

		PT_3D textpt;
		textpt.x = e.m_xl + (dx+(i-1)*x_interv)*m_pAssist->Unit();
		textpt.y = e.m_yl - (dy+cal_size+0.6)*m_pAssist->Unit();
		textpt.z = 0;

		RotatePts(&textpt,1,xb.cosa,xb.sina);

		if( cal_text.IsEmpty() ) continue;

		CFeature *pFtr = m_pAssist->CreateText(layer,pLayer);
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		if( !pText )return;		
		
		if(MapDecorate::bMapDecorateNew)
			m_pAssist->SetText(pText,&textpt,1,cal_text,cal_font,cal_size,cal_wscale,TAH_MID|TAV_TOP);
		else
			m_pAssist->SetText(pText,&textpt,1,cal_text,StrFromResID(IDS_MD_ZHENGDENGXIANTI),cal_size,cal_wscale,TAH_MID|TAV_TOP);

		m_pAssist->SetText(pText,xb.ang*180/PI);
		m_pAssist->AddObject(pFtr,pLayer->GetID());
		if( i == 30 )
		{
			RotatePts(&textpt,1,xb.cosa,-xb.sina);
			textpt.x += 2*m_pAssist->Unit();
			RotatePts(&textpt,1,xb.cosa,xb.sina);

			pFtr = m_pAssist->CreateText(layer,pLayer);
			pText = (CGeoText*)pFtr->GetGeometry();
			if( !pText )return;		
			
			if(MapDecorate::bMapDecorateNew)
				m_pAssist->SetText(pText,&textpt,1,"%",cal_font,cal_size,cal_wscale,TAH_MID|TAV_TOP);
			else
				m_pAssist->SetText(pText,&textpt,1,"%",StrFromResID(IDS_MD_ZHENGDENGXIANTI),cal_size,cal_wscale,TAH_MID|TAV_TOP);

			m_pAssist->SetText(pText,xb.ang*180/PI);
			m_pAssist->AddObject(pFtr,pLayer->GetID());
		}
	}

	//竖线
	for(i = 0; i < 30; i++)
	{
		PT_3DEX pts[2];
		pts[0] = pts_bottom.GetAt(i);
		if( i < 4 )
			pts[1] = pts_top.GetAt(i);
		else
			pts[1] = pts_top4.GetAt(i-4);

		RotatePts(pts,2,xb.cosa,xb.sina);
		m_pAssist->AddLine(layer,pts[0],pts[1],0);
	}

	//曲线（相邻两条等高线）
	CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;

	RotatePts(pts_top.GetData() ,pts_top.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top.GetData(), pts_top.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	//曲线（相邻六条等高线）
	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top1.GetData() ,pts_top1.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top1.GetData(), pts_top1.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top2.GetData() ,pts_top2.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top2.GetData(), pts_top2.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top3.GetData() ,pts_top3.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top3.GetData(), pts_top3.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());


	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	if( !pFtr ) return;	
	pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( !pGeo ) return;
	RotatePts(pts_top4.GetData() ,pts_top4.GetSize() ,xb.cosa,xb.sina);
	pGeo->CreateShape(pts_top4.GetData(), pts_top4.GetSize());	
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	//标题
	PT_3DEX textpt;
	textpt.x = e.m_xl + (dx+0.5*29*x_interv)*m_pAssist->Unit();
	textpt.y = e.m_yl - dy2*m_pAssist->Unit();
	textpt.z = 0;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	char title_text[64];
	strcpy(title_text,StrFromResID(IDS_MD_GRADE));
	m_pAssist->SetText(pText,&textpt,1,title_text,title_font,title_size,title_wscale,TAH_MID|TAV_TOP);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	//注释
	textpt = pts_top.GetAt(0);
	textpt.y += 0.3*m_pAssist->Unit();	
//	RotatePts(&textpt,1,xb.cosa,xb.sina);	//pts_top中的所有坐标之前已经旋转过了	
	pFtr = m_pAssist->CreateText(layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;
	
	char note_text[64];
	strcpy(note_text,StrFromResID(IDS_MD_TWO_CONTOUR_NEAR));
	m_pAssist->SetText(pText,&textpt,1,note_text,note_font,note_size,note_wscale,TAH_LEFT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	textpt = pts_top4.GetAt(6);
	textpt.y += 0.3*m_pAssist->Unit();
	pFtr = m_pAssist->CreateText(layer,pLayer);
	pText = (CGeoText*)pFtr->GetGeometry();
	if( !pText )return;	

	strcpy(note_text,StrFromResID(IDS_MD_SIX_CONTOUR_NEAR));
	m_pAssist->SetText(pText,&textpt,1,note_text,note_font,note_size,note_wscale,TAH_LEFT|TAV_BOTTOM);
	m_pAssist->SetText(pText,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());
}



//
COuterBoundDecorator::COuterBoundDecorator()
{
	int scale =  GetActiveDlgDoc()->GetDlgDataSource()->GetScale();

	m_strName = StrFromResID(IDS_MD_OUTTER_BORDER_DECORATOR);
	strcpy(layer,"TK_outer_bound_decorator");
	width = 0;
	extension = 4;
	interv = 50;

	if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		if(MapDecorate::bMapDecorateNew)
		{
			extension = 1.75;
		}
		else
		{
			extension = 1.8;
		}
	}

}


COuterBoundDecorator::~COuterBoundDecorator()
{
}

void COuterBoundDecorator::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("width",width,StrFromResID(IDS_MD_OBD_LINEWID));
	pParam->AddParam("extension",extension,StrFromResID(IDS_MD_OBD_OFFSET));	
}

void COuterBoundDecorator::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"width",pvar) )
	{
		width = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"extension",pvar) )
	{
		extension = (float)(_variant_t)*pvar;
	}
}

void COuterBoundDecorator::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = width;
	tab.AddValue("width",&CVariantEx(var));
	
	var = extension;
	tab.AddValue("extension",&CVariantEx(var));		
}


void COuterBoundDecorator::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	PT_3D ptsBound[4];
	memcpy(ptsBound, m_pMain->m_outerBound, sizeof(PT_3D)*4);
	double ext = extension*m_pAssist->Unit();
	Extend(ptsBound, ext);

	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = m_pAssist->CreateCurve(layer,pLayer);
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( pGeo )
	{
		PT_3DEX expt, pts[5];
		expt.z = 0;
		expt.pencode = penLine;
		for( int i=0; i<5; i++)
		{
			COPY_3DPT(expt,ptsBound[i%4]);
			pts[i] = expt;
		}
		
		pGeo->CreateShape(pts,5);		
		pGeo->m_fLineWidth = width;		
		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);
	RotatePts(ptsBound,4,xb.cosa,-xb.sina);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);

	float xv = interv*m_pAssist->Unit();
	int row = floor(e.Height()/xv);
	int col = floor(e.Width()/xv);

	PT_3D linePt[2];
	PT_3D ptA, ptB;
	int i = 0;
	for(i = 1; i <= row; i++)
	{
		double x1 = e.m_xl;
		double y1 = e.m_yl + i*xv;
		double x2 = e.m_xh;
		double y2 = y1;

		ptA = xb.outerBound[0];
		ptB = xb.outerBound[3];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[0].x),&(linePt[0].y),NULL);

		ptA = ptsBound[0];
		ptB = ptsBound[3];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[1].x),&(linePt[1].y),NULL);

		RotatePts(linePt,2,xb.cosa,xb.sina);
		m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

		ptA = xb.outerBound[1];
		ptB = xb.outerBound[2];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[0].x),&(linePt[0].y),NULL);

		ptA = ptsBound[1];
		ptB = ptsBound[2];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[1].x),&(linePt[1].y),NULL);

		RotatePts(linePt,2,xb.cosa,xb.sina);
		m_pAssist->AddLine(layer,linePt[0],linePt[1],0);
	}

	for(i = 1; i <= col; i++)
	{
		double x1 = e.m_xl + i*xv;
		double y1 = e.m_yl;
		double x2 = x1;
		double y2 = e.m_yh;

		ptA = xb.outerBound[0];
		ptB = xb.outerBound[1];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[0].x),&(linePt[0].y),NULL);

		ptA = ptsBound[0];
		ptB = ptsBound[1];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[1].x),&(linePt[1].y),NULL);

		RotatePts(linePt,2,xb.cosa,xb.sina);
		m_pAssist->AddLine(layer,linePt[0],linePt[1],0);

		ptA = xb.outerBound[3];
		ptB = xb.outerBound[2];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[0].x),&(linePt[0].y),NULL);

		ptA = ptsBound[3];
		ptB = ptsBound[2];
		GraphAPI::GGetLineIntersectLine(x1,y1,x2-x1,y2-y1,ptA.x,ptA.y,ptB.x-ptA.x,ptB.y-ptA.y,&(linePt[1].x),&(linePt[1].y),NULL);

		RotatePts(linePt,2,xb.cosa,xb.sina);
		m_pAssist->AddLine(layer,linePt[0],linePt[1],0);
	}

}

//
CAdminRegion::CAdminRegion()
{
	m_strName = StrFromResID(IDS_MD_ADMINREGION);
	strcpy(layer,"TK_admin_region");
	src_layers = "";

	dx = 3;
	dy = 10;
	width = 35;
	height = 26;

	strcpy(font,StrFromResID(IDS_MD_CUDENGXIANTI));
	size = 3.25;
	wscale = 1;	
}

CAdminRegion::~CAdminRegion()
{
}

void CAdminRegion::GetUIParams(CUIParam *pParam)
{
	pParam->AddParam("layer",layer,StrFromResID(IDS_LAYER_NAME));
	pParam->AddParam("dx",dx,StrFromResID(IDS_MD_LSPACE));
	pParam->AddParam("dy",dy,StrFromResID(IDS_MD_INNER_BSPACE));

	pParam->AddFontNameParam("font",font,StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT));
	pParam->AddParam("size",size,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT));
	pParam->AddParam("wscale",wscale,StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS));

	pParam->AddParam("width",width,StrFromResID(IDS_MD_ALLWID));
	pParam->AddParam("height",height,StrFromResID(IDS_MD_ALLHEI));
}

void CAdminRegion::SetParams(CValueTable& tab)
{
	const CVariantEx *pvar;
	
	if( tab.GetValue(0,"layer",pvar) )
	{
		strncpy(layer,(LPCTSTR)(_bstr_t)*pvar,sizeof(layer)-1);
	}
	if( tab.GetValue(0,"dx",pvar) )
	{
		dx = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"dy",pvar) )
	{
		dy = (float)(_variant_t)*pvar;
	}

	if( tab.GetValue(0,"font",pvar) )
	{
		strncpy(font,(LPCTSTR)(_bstr_t)*pvar,sizeof(font)-1);
	}
	if( tab.GetValue(0,"size",pvar) )
	{
		size = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"wscale",pvar) )
	{
		wscale = (float)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,"width",pvar) )
	{
		width = (float)(_variant_t)*pvar;
	}
	if( tab.GetValue(0,"height",pvar) )
	{
		height = (float)(_variant_t)*pvar;
	}
}

void CAdminRegion::GetParams(CValueTable& tab)
{
	_variant_t var;
	
	var = (LPCTSTR)layer;
	tab.AddValue("layer",&CVariantEx(var));
	
	var = dx;
	tab.AddValue("dx",&CVariantEx(var));
	
	var = dy;
	tab.AddValue("dy",&CVariantEx(var));

	var = (LPCTSTR)font;
	tab.AddValue("font",&CVariantEx(var));

	var = size;
	tab.AddValue("size",&CVariantEx(var));
	
	var = wscale;
	tab.AddValue("wscale",&CVariantEx(var));

	var = width;
	tab.AddValue("width",&CVariantEx(var));
	
	var = height;
	tab.AddValue("height",&CVariantEx(var));
}

void CAdminRegion::GetUIParams_data(CUIParam *pParam)
{
	pParam->AddLayerNameParamEx("src_layers",src_layers,StrFromResID(IDS_ADMINREGION_SRC_LAYER_NAME),NULL,LAYERPARAMITEM_LINEAREA);
}

void CAdminRegion::SetData(CValueTable& tab)
{
	const CVariantEx *pvar;

	if( tab.GetValue(0,"src_layers",pvar) )
	{
		src_layers = (const char*)(_bstr_t)(_variant_t)*pvar;
	}
}

void CAdminRegion::GetData(CValueTable& tab)
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)src_layers;
	tab.AddValue("src_layers", &CVariantEx(var));
}

void CAdminRegion::MakeFtrs()
{
	if( !m_pMain || !m_pAssist )
		return;

	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_outerBound[0],m_pMain->m_outerBound[1],xb);

	Envelope e;
	e.CreateFromPts(xb.outerBound,4);
	Envelope e2;
	e2.CreateFromPts(xb.innerBound,4);

	PT_3D ptsAdmin[4];

	CFtrLayer *pLayer = NULL;

	//标题
	PT_3D textpt;
	textpt.x = e.m_xh + (dx+0.5*width)*m_pAssist->Unit();
	textpt.y = e2.m_yl + (dy+height+3)*m_pAssist->Unit();
	textpt.z = 0;

	CFeature* pFtr = m_pAssist->CreateText(layer,pLayer);
	CGeoText *pText_title = (CGeoText*)pFtr->GetGeometry();
	if( !pText_title )return;
	
	RotatePts(&textpt,1,xb.cosa,xb.sina);
	
	m_pAssist->SetText(pText_title,&textpt,1,StrFromResID(IDS_MD_ADMINREGION),font,size,wscale,TAH_MID|TAV_BOTTOM);
	m_pAssist->SetText(pText_title,xb.ang*180/PI);
	m_pAssist->AddObject(pFtr,pLayer->GetID());

	//边框
	double xs, xe, ys, ye;
	xs = e.m_xh + dx*m_pAssist->Unit();
	xe = xs + width*m_pAssist->Unit();
	ys = e2.m_yl + dy*m_pAssist->Unit();
	ye = ys + height*m_pAssist->Unit();

	PT_3DEX expt;
	expt.pencode = penLine;
	expt.z = 0;

	pFtr = m_pAssist->CreateCurve(layer,pLayer);
	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( pGeo )
	{
		//外边框
		PT_3DEX pts[5];
		expt.x = xs; expt.y = ys;
		pts[0] = expt;
		COPY_3DPT(ptsAdmin[0], pts[0]);

		expt.x = xe; expt.y = ys;
		pts[1] = expt;
		COPY_3DPT(ptsAdmin[1], pts[1]);

		expt.x = xe; expt.y = ye;
		pts[2] = expt;
		COPY_3DPT(ptsAdmin[2], pts[2]);

		expt.x = xs; expt.y = ye;
		pts[3] = expt;
		COPY_3DPT(ptsAdmin[3], pts[3]);

		expt.x = xs; expt.y = ys;
		pts[4] = expt;

		RotatePts(pts,5,xb.cosa,xb.sina);

		pGeo->CreateShape(pts,5);

		m_pAssist->AddObject(pFtr,pLayer->GetID());
	}

	//填充政区略图
	double matrix1[16];			//缩放矩阵
	Envelope e3;
	e3.CreateFromPts(ptsAdmin,4);
	double kx = double(e3.Width()/e2.Width());
	double ky = double(e3.Height()/e2.Height());
	double kz = 1.0;
	Matrix44FromZoom(kx,ky,kz,matrix1);
	
	double matrixA[16];			//平移矩阵（将内图廓的中心移至原点）
	Envelope eA;
	eA.CreateFromPts(m_pMain->m_innerBound,4);
	double ax = 0.5*(eA.m_xl+eA.m_xh);
	double ay = 0.5*(eA.m_yl+eA.m_yh);
	double az = 0;
	Matrix44FromMove(-ax,-ay,-az,matrixA);

	double matrixB[16];			//平移矩阵（将抽稀缩放后的地图移至政区略图）
	Envelope eB;
	RotatePts(ptsAdmin,4,xb.cosa,xb.sina);
	eB.CreateFromPts(ptsAdmin,4);
	double bx = 0.5*(eB.m_xl+eB.m_xh);
	double by = 0.5*(eB.m_yl+eB.m_yh);
	double bz = 0;
	Matrix44FromMove(bx,by,bz,matrixB);

	DrawAdminRegion(matrix1, matrixA, matrixB);
}

void CAdminRegion::DrawAdminRegion(double matrix1[16], double matrixA[16], double matrixB[16])
{
	if( !m_pMain || !m_pAssist || !m_pMain->m_pDS )
		return;

	CStringArray arr;
	if(!convertStringToStrArray(src_layers,arr))
		return;
	CFtrArray ftrs;
	int i = 0, j = 0;
	for(int a=0;a<arr.GetSize();a++)
	{
		CPtrArray arrLayers;
		if(!(m_pMain->m_pDS)->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
			return;
		int nTar = arrLayers.GetSize();	
		for (i=0;i<nTar;i++)
		{
			CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				if( !IsFeatureInBound(pFtr) )
					continue;
				
				ftrs.Add( pFtr );
			}
		}
	}

	CFtrLayer *pLayer = NULL;
	m_pAssist->CreateCurve(layer,pLayer);

	for(i = 0; i < ftrs.GetSize(); i++)
	{
		CFeature* pFtr = ftrs.GetAt(i);
		CGeometry* pObj = pFtr->GetGeometry();
		if(!pObj) continue;

		CGeometry* pNewObj = Compress( pObj , 10);
		if(!pNewObj) continue;
		pNewObj->Transform(matrixA);
		pNewObj->Transform(matrix1);
		pNewObj->Transform(matrixB);

		CFeature* pNewFtr = pFtr->Clone();
		pNewFtr->SetID(OUID());
		pNewFtr->SetGeometry( pNewObj );
		(m_pMain->m_pDS)->GetXAttributesSource()->CopyXAttributes(pFtr,pNewFtr);

		long col = pObj->GetColor();
		if( -1 == col )
		{
			CFtrLayer* pOriLayer = (m_pMain->m_pDS)->GetFtrLayerOfObject(pFtr);
			col = pOriLayer->GetColor();
		}
		pNewObj->SetColor( col );

		m_pAssist->AddObject(pNewFtr,pLayer->GetID());
	}
}

bool CAdminRegion::IsFeatureInBound(CFeature* pFtr)
{
	if( !m_pMain || !pFtr ) return false;

	//旋转内图廓与坐标系平行
	CMapDecorator::XBound xb;
	m_pMain->RotateBound(m_pMain->m_innerBound[0],m_pMain->m_innerBound[1],xb);
	Envelope e;
	e.CreateFromPts(xb.innerBound,4);

	CArray<PT_3DEX,PT_3DEX> pts;
	CGeometry* pGeo = pFtr->GetGeometry();
	if( !pGeo ) return false;
	pGeo->GetShape( pts );

	Envelope e2;
	int num = pts.GetSize();
	if( num > 0 )
	{
		PT_3D* allpts = new PT_3D[num];
		for(int i = 0; i < num; i++)
		{
			COPY_3DPT(allpts[i], pts.GetAt(i) );
		}
		RotatePts(allpts, num, xb.cosa, -xb.sina);
		e2.CreateFromPts(allpts, num);
		delete [] allpts;
	}	

	if( e.bEnvelopeIn(&e2) ) return true;
	return false;
}

//
}