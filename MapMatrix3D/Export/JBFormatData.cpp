// JBFormatData.cpp: implementation of the CJBFormatData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JBFormatData.h"
#include "SmartviewFunctions.h"
#include "EditbaseDoc.h"
#include "DlgDataSource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FMT_CHAR		"字符型"
#define FMT_SHORT		"短整型"
#define FMT_LONG		"长整型"
#define FMT_FLOAT		"浮点型"
#define FMT_DOUBLE		"双精度"

extern int ReadLineFromString(char *line, int *pos, char *ret);
extern int ReadStringFromString(char *line, int *pos, char *ret);

int ReadLineFromString(char *line, int *pos, char *ret)
{
	if( *pos<0 )
		return 1;
	
	if( line[0]==0 )
	{
		*pos = -1;
		return 1;
	}
	else if( *pos>=strlen(line) )
	{
		*pos = -1;
		return 1;
	}
	else
	{
		char *p = line + *pos;
		int readchar = 0;
		while( *p!=0 && *p!='\n' )
		{
			*ret = *p;
			ret++; p++;
			readchar++;
		}
		if( *p=='\n' )p++;
		
		*ret = 0;
		*pos = p - line;
		
		return 0;
	}
}


void JBFormatDefineItem::ConvertStringToValue(LPCTSTR text, _variant_t& var)const
{
	if( stricmp(type,FMT_LONG)==0 || stricmp(type,FMT_SHORT)==0 )
	{
		var = atol(text);
	}
	else if( stricmp(type,FMT_FLOAT)==0 || stricmp(type,FMT_DOUBLE)==0 )
	{
		var = atof(text);
	}
	else
	{
		var = text;
	}
}


void SortBoundPoints(PT_3D pts[4])
{
	PT_3D pts2[4];
	
	int k = -1;
	double dis, min;
	for(int i=0; i<4; i++)
	{
		dis = pts[i].x*pts[i].x + pts[i].y*pts[i].y;
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


void JBFormatDefineItem::ConvertValueToString(_variant_t& var, CString& text)const
{
	long lv = 0;
	double fv = 0;

	switch( var.vt ) 
	{
	case VT_UI1:
		lv = var.bVal;
		fv = lv;
		break;
	case VT_I2:
		lv = var.iVal;
		fv = lv;
		break;
	case VT_I4:
		lv = var.lVal;
		fv = lv;
		break;
	case VT_R4:
		fv = var.fltVal;
		lv = fv;
		break;
	case VT_R8:
		fv = var.dblVal;
		lv = fv;
		break;
	case VT_I1:
		lv = var.cVal;
		fv = lv;
		break;
	case VT_UI2:
		lv = var.uiVal;
		fv = lv;
		break;
	case VT_UI4:
		lv = var.ulVal;
		fv = lv;
		break;
	case VT_INT:
		lv = var.intVal;
		fv = lv;
		break;
	case VT_UINT:
		lv = var.uintVal;
		fv = lv;
		break;
	}
	
	CString strFormat = CString("%") + format;
	strFormat.MakeLower();

	if( stricmp(type,FMT_LONG)==0 || stricmp(type,FMT_SHORT)==0 )
	{
		text.Format((LPCTSTR)strFormat,(int)lv);
	}
	else if( stricmp(type,FMT_FLOAT)==0 || stricmp(type,FMT_DOUBLE)==0 )
	{
		text.Format((LPCTSTR)strFormat,(double)fv);
	}
	else if( stricmp(type,FMT_CHAR)==0 )
	{
		CString strValue = "NULL";
		if( var.vt==VT_BSTR )
		{
			strValue = (LPCTSTR)(_bstr_t)var;
		}
		text.Format((LPCTSTR)strFormat,(LPCTSTR)strValue);
	}
}


void JBFormatDefineItem::NormalizeFormat()
{
	CString str = format;
	str.MakeLower();
	str.Replace("ld","d");
	str.Replace("c","s");
	strcpy(format,(LPCTSTR)str);
}


BOOL JBFormatDefineItem::IsHaveEnumValues()const
{
	return (strchr(desc,';')!=0);
}

BOOL GetEnumNameAndValueFromDesc(CString& desc, CString& name, CString& value)
{
	int pos2 = desc.Find(':');
	if( pos2>0 )
	{
		name = desc.Mid(pos2+1);
		value = desc.Left(pos2);
		return TRUE;
	}
	else
	{
		name = desc;
		value = name;
		return FALSE;
	}
}

void JBFormatDefineItem::GetEnumValues(CStringArray* arrPNames, CStringArray *arrPValues)const
{
	if( !IsHaveEnumValues() )return;
	
	CStringArray arrNames, arrValues;
	
	CString str = desc, str2, name, value;
	while(1)
	{
		int pos = str.Find(';');
		if( pos>0 )
		{
			str2 = str.Left(pos);
			GetEnumNameAndValueFromDesc(str2,name,value);
			
			arrNames.Add(name);
			arrValues.Add(value);
			str = str.Mid(pos+1);
		}
		else
		{
			//连续两个分号
			if( str.GetLength()>0 )
			{
				if( str[0]==';' )break;
			}
			
			//没有连续两个分号
			if( str.GetLength()>0 )
			{
				GetEnumNameAndValueFromDesc(str,name,value);
				arrNames.Add(name);
				arrValues.Add(value);
			}
			break;
		}
	}

	if( arrPNames )arrPNames->Copy(arrNames);
	if( arrPValues )arrPValues->Copy(arrValues);
}


CString JBFormatDefineItem::ConvertEnumNameToValue(LPCTSTR text)const
{
	CStringArray arrNames, arrValues;
	GetEnumValues(&arrNames,&arrValues);

	for( int i=0; i<arrNames.GetSize(); i++)
	{
		if( arrNames[i].CompareNoCase(text)==0 )
			return arrValues[i];
	}

	return CString(text);
}


CString JBFormatDefineItem::ConvertEnumValueToName(LPCTSTR text)const
{
	CStringArray arrNames, arrValues;
	GetEnumValues(&arrNames,&arrValues);
	
	for( int i=0; i<arrNames.GetSize(); i++)
	{
		if( arrValues[i].CompareNoCase(text)==0 )
			return arrNames[i];
	}
	
	return CString(text);
}


void JBFormatDefineItem::CreateDefaultValue(_variant_t& var)const
{
	char defv[64] = {0};
	const char *pos = strstr(desc,"缺省:");
	if( pos )
	{
		pos += 5;
		const char *p = pos;
		while( *p!=0 )
		{
			if( *p==',' || *p==':' || *p==';' || *p=='/' )
				break;
			p++;
		}
		strncpy(defv,pos,p-pos);
	}
	if( stricmp(type,FMT_LONG)==0 || stricmp(type,FMT_SHORT)==0 )
	{
		var = (long)atol(defv);
	}
	else if( stricmp(type,FMT_FLOAT)==0 || stricmp(type,FMT_DOUBLE)==0 )
	{
		var = (long)atof(defv);
	}
	else
	{
		if( strlen(defv)==0 )
		{
			var = "NULL";
		}
		else
			var = defv;
	}

	if( IsHaveEnumValues() )
	{
		CStringArray arrNames, arrValues;
		GetEnumValues(&arrNames,&arrValues);
		if( arrValues.GetSize()>0 )
		{
			ConvertStringToValue(arrValues.GetAt(0),var);
			return;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJBFormatData::CJBFormatData()
{

}

CJBFormatData::~CJBFormatData()
{

}


BOOL CJBFormatData::LoadDefines(CString& text)
{
	char *buf = text.GetBuffer(1);	
	char line[1024], word[256];
	int bfinddefine = 0, pos = 0, pos2;
	JBFormatDefineItem item;
	m_FmtItems.RemoveAll();

	while(1)
	{
		if( ReadLineFromString(buf,&pos,line)!=0 )break;

		pos2 = 0;
		if( ReadStringFromString(line,&pos2,word)!=0 )continue;
		
		if( word[0]=='#' )continue;
		if( stricmp(word,"DefineEnd")==0 )break;
		if( stricmp(word,"Define")==0 )
		{
			bfinddefine = 1;
			ReadStringFromString(line,&pos2,word);
			m_strFmtName = word;
			continue;
		}
		
		if( bfinddefine )
		{
			pos2 = 0;
			memset(&item,0,sizeof(item));
			ReadStringFromString(line,&pos2,item.name);
			ReadStringFromString(line,&pos2,item.type);
			ReadStringFromString(line,&pos2,item.format);
			ReadStringFromString(line,&pos2,item.desc);

			item.NormalizeFormat();

			m_FmtItems.Add(item);
		}
	}

	text.ReleaseBuffer();

	text = text.Mid(pos);
	return (m_FmtItems.GetSize()>0);
}


BOOL CJBFormatData::LoadValues(int index, CString& text, CValueTable& tab)
{
	char *buf = text.GetBuffer(1);	
	char word[256];
	
	int pos = 0, nsz = m_FmtItems.GetSize();
	int nItems = tab.GetItemCount();

	_variant_t var;
	for( int i=0; i<nsz; i++)
	{
		ReadStringFromString(buf,&pos,word);
		JBFormatDefineItem item = m_FmtItems.GetAt(i);

		item.ConvertStringToValue(word,var);

		tab.SetValue(index,item.name,&CVariantEx(var));
	}

	text.ReleaseBuffer();
	
	text = text.Mid(pos);
	
	return TRUE;
}


BOOL CJBFormatData::SaveValues(int index, CValueTable& tab, CString& text)
{
	CString line, word;
	const CVariantEx *p = NULL;
	CString field, name;
	
	//for( int i=0; i<tab.GetItemCount(); i++)
	{
		line.Empty();

		for( int j=0; j<m_FmtItems.GetSize(); j++)
		{
			JBFormatDefineItem item = m_FmtItems.GetAt(j);
			if( tab.GetValue(index,item.name,p) )
			{
				word.Empty();
				item.ConvertValueToString((_variant_t)*p,word);
				line += word + " ";
			}
			else
			{
				line += " ";
			}
		}

		//line += "\r\n";

		text += line;
	}

	return TRUE;
}



JBFormatDefineItem CJBFormatData::FindDefine(LPCTSTR field)
{
	for(int i=0; i<m_FmtItems.GetSize(); i++)
	{
		if( stricmp(m_FmtItems[i].name,field)==0 )
		{
			return m_FmtItems[i];
		}
	}
	
	JBFormatDefineItem item;
	memset(&item,0,sizeof(item));
	
	return item;
}


CJBMetaData::CJBMetaData()
{
}


CJBMetaData::~CJBMetaData()
{
}



#define IsEndChar(x)		((x)==_T('\0'))
#define IsSpaceChar(x)		((x)==_T(' ')||(x)==_T('\t')||(x)==_T('\r')||(x)==_T('\n'))

static CString FormatKeyin(LPCTSTR keyin)
{
	CString str;
	LPCTSTR p = keyin;
	int space = 0;
	while( IsSpaceChar(*p) )p++;
	if( IsEndChar(*p) )return str;
	
	while( !IsEndChar(*p) )
	{
		if( space==0 )
		{
			if( !IsSpaceChar(*p) )
				str += *p;
			else
			{
				space = 1-space;
			}
		}
		else if( space==1 )
		{
			if( !IsSpaceChar(*p) )
			{
				str += ' ';
				str += *p;
				space = 1-space;
			}
		}
		p++;
	}
	
	return str;
}

BOOL CJBMetaData::LoadDefines()
{
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(256),256);
	strPath.ReleaseBuffer();
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	strPath += "\\Config\\MilitaryVectorFormat.txt";

	FILE *fp = fopen((LPCTSTR)strPath,"rt");
	if( !fp )return FALSE;
	fseek(fp,0,SEEK_END);
	long nSize = ftell(fp);

	CString text;
	char *buf = text.GetBuffer(nSize+1);
	fseek(fp,0,SEEK_SET);
	fread(buf,nSize,1,fp);
	text.ReleaseBuffer();
	fclose(fp);

	return LoadDefines(text);
}


BOOL CJBMetaData::LoadDefines(CString& text)
{
	m_mapItems.RemoveAll();
	m_layItems.RemoveAll();

	char *buf = text.GetBuffer(1);
	char line[1024];
	int pos = 0, bFindDefine = 0;
	JBMetaDefineItem item;
	BOOL bIsLay = FALSE;

	while( 1 ) 
	{
		if( ReadLineFromString(buf,&pos,line)!=0 )break;

		strcpy(line,FormatKeyin(line));

		if( strlen(line)<=0 )continue;
		if( line[0]=='#' )continue;
		if( stricmp(line,"Define MetaData")==0 )
		{
			bFindDefine = 1;
			continue;
		}
		else if( stricmp(line,"DefineEnd")==0 )
		{
			break;
		}

		memset(&item,0,sizeof(item));

		if( !bFindDefine )continue;

		if( sscanf(line,"%s%s%s%s",item.name,item.type,item.format,item.desc)==4 )
		{
			if( stricmp(item.name,"LAYERBEGIN")==0 )
			{
				bIsLay = TRUE;

				if( m_mapItems.GetSize()>0 )
				{
					JBMetaDefineItem t = m_mapItems.GetAt(m_mapItems.GetSize()-1);
					m_strLayNumField = t.name;
				}

				continue;
			}
			else if( stricmp(item.name,"LAYEREND")==0 )
			{
				bIsLay = FALSE;
				continue;
			}

			item.NormalizeFormat();
			if( !bIsLay )
			{
				m_mapItems.Add(item);
			}
			else
			{
				m_layItems.Add(item);
			}
		}
	}

	CreateDefaultValues();

	text = text.Mid(pos);

	return TRUE;
}


BOOL CJBMetaData::SetMapValue(LPCTSTR field, LPCTSTR value)
{
	JBMetaDefineItem item;
	_variant_t vt;
	int nItem;

	CArray<JBMetaDefineItem,JBMetaDefineItem> *pDefines;
	CValueTable *pValues;
	
	pDefines = &m_mapItems;
	pValues = &m_mapValues;

	nItem = pDefines->GetSize();

	for( int i=0; i<nItem; i++)
	{
		item = pDefines->GetAt(i);
		if( stricmp(item.name,field)==0 )
		{
			item.ConvertStringToValue(value,vt);
			pValues->SetValue(0,field,&CVariantEx(vt));
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CJBMetaData::SetLayValue(int idx, LPCTSTR field, LPCTSTR value)
{
	JBMetaDefineItem item;
	_variant_t vt;
	int nItem;
	
	CArray<JBMetaDefineItem,JBMetaDefineItem> *pDefines;
	CValueTable *pValues;
	
	pDefines = &m_layItems;
	pValues = &m_layValues;
	
	nItem = pDefines->GetSize();
	
	for( int i=0; i<nItem; i++)
	{
		item = pDefines->GetAt(i);
		if( stricmp(item.name,field)==0 )
		{
			item.ConvertStringToValue(value,vt);
			pValues->SetValue(idx,field,&CVariantEx(vt));
			return TRUE;
		}
	}
	
	return FALSE;
}

void CJBMetaData::CreateLayerValues(int num)
{
	JBMetaDefineItem item;
	int nItem;
	
	_variant_t vt;

	if( m_layValues.GetItemCount()>=num )
	{
		for( int j=m_layValues.GetItemCount()-1; j>=num; j++)
		{
			m_layValues.DelValueItem(j);
		}
		return;
	}
	
	for( int j=m_layValues.GetItemCount(); j<num; j++)
	{
		nItem = m_layItems.GetSize();

		m_layValues.BeginAddValueItem();
		for( int i=0; i<nItem; i++)
		{
			item = m_layItems.GetAt(i);
			vt.Clear();

			item.CreateDefaultValue(vt);
			
			m_layValues.AddValue(item.name,&CVariantEx(vt));
			
		}
		m_layValues.EndAddValueItem();
	}
}

void CJBMetaData::CreateDefaultValues()
{
	JBMetaDefineItem item;
	int nItem = m_mapItems.GetSize();

	_variant_t vt;
	
	m_mapValues.DelAll();
	m_mapValues.BeginAddValueItem();
	for( int i=0; i<nItem; i++)
	{
		item = m_mapItems.GetAt(i);
		vt.Clear();

		item.CreateDefaultValue(vt);
		
		m_mapValues.AddValue(item.name,&CVariantEx(vt));
		
	}
	m_mapValues.EndAddValueItem();
}


BOOL CJBMetaData::LoadValues(LPCTSTR fileName)
{
	FILE *fp = fopen(fileName,"r");
	if( !fp )return FALSE;

	m_strValuePath = fileName;

	char line[1024], field[256],value[256];
	int pos = 0;

	BOOL bStartReadLay = FALSE;
	int  idx = -1;

	CString strFirstLayField;
	if( m_layItems.GetSize()>0 )
		strFirstLayField = m_layItems.GetAt(0).name;

	while( !feof(fp) )
	{
		memset(field,0,sizeof(field));
		memset(value,0,sizeof(value));
		pos = 0;

		fgets(line,sizeof(line)-1,fp);

		ReadStringFromString(line,&pos,field);
		ReadStringFromString(line,&pos,field);
		ReadStringFromString(line,&pos,value);

		if( stricmp(field,m_strLayNumField)==0 )
		{
			int nLay = atoi(value);
			CreateLayerValues(nLay);

			SetMapValue(field,value);

			bStartReadLay = TRUE;

			continue;
		}

		if( !bStartReadLay )
			SetMapValue(field,value);
		else
		{
			if( strFirstLayField.CompareNoCase(field)==0 )
				idx++;

			SetLayValue(idx,field,value);
		}
	}

	fclose(fp);

	return TRUE;
}


BOOL CJBMetaData::SaveValues(LPCTSTR fileName)
{
	FILE *fp = fopen(fileName,"w");
	if( !fp )return FALSE;

	JBMetaDefineItem item;
	int nItem = m_mapItems.GetSize(), index = 1;
	
	_variant_t vt;
	const CVariantEx *pv;

	CString text,line;
	
	for( int i=0; i<nItem; i++)
	{
		item = m_mapItems.GetAt(i);

		vt.Clear();
		if( m_mapValues.GetValue(0,item.name,pv) )
		{
			vt = pv->m_variant;
		}

		if( stricmp(item.name,(LPCTSTR)m_strLayNumField)==0 )
		{
			vt = (long)m_layValues.GetItemCount();
		}

		text.Empty();
		item.ConvertValueToString(vt,text);

		fprintf(fp,"%10d %30s %20s\n",index++,item.name,(LPCTSTR)text);
	}

	nItem = m_layItems.GetSize();
	for( int j=0; j<m_layValues.GetItemCount(); j++)
	{
		for( i=0; i<nItem; i++)
		{
			item = m_layItems.GetAt(i);
			
			vt.Clear();
			if( m_layValues.GetValue(j,item.name,pv) )
			{
				vt = pv->m_variant;
			}
			
			text.Empty();
			item.ConvertValueToString(vt,text);
			
			fprintf(fp,"%10d %30s %20s\n",index++,item.name,(LPCTSTR)text);
		}
	}

	fclose(fp);

	return TRUE;
}


void CJBMetaData::ReloadValues()
{
	CString strPath = m_strValuePath;
	LoadValues(strPath);
}


void CJBMetaData::Clear()
{
	m_mapItems.RemoveAll();
	m_layItems.RemoveAll();

	m_mapValues.DelAll();
	m_layValues.DelAll();
}


extern void SortBoundPoints(PT_3D pts[4]);

extern char *groupIDList[18];
extern char *groupNameList[18];

void CJBMetaData::SetValuesFromDoc(CDlgDoc *pDoc)
{
	//设置图廓坐标
	PT_3D pts[4];
	pDoc->GetDlgDataSource()->GetBound(pts,NULL,NULL);
	SortBoundPoints(pts);

	m_mapValues.SetValue(0,JBFIELD_MAPLBX,&CVariantEx((_variant_t)pts[0].x));
	m_mapValues.SetValue(0,JBFIELD_MAPLBY,&CVariantEx((_variant_t)pts[0].y));
	m_mapValues.SetValue(0,JBFIELD_MAPRBX,&CVariantEx((_variant_t)pts[1].x));
	m_mapValues.SetValue(0,JBFIELD_MAPRBY,&CVariantEx((_variant_t)pts[1].y));
	m_mapValues.SetValue(0,JBFIELD_MAPRTX,&CVariantEx((_variant_t)pts[2].x));
	m_mapValues.SetValue(0,JBFIELD_MAPRTY,&CVariantEx((_variant_t)pts[2].y));
	m_mapValues.SetValue(0,JBFIELD_MAPLTX,&CVariantEx((_variant_t)pts[3].x));
	m_mapValues.SetValue(0,JBFIELD_MAPLTY,&CVariantEx((_variant_t)pts[3].y));

	m_mapValues.SetValue(0,JBFIELD_ORIGINX,&CVariantEx((_variant_t)(pts[0].x-1000)));
	m_mapValues.SetValue(0,JBFIELD_ORIGINY,&CVariantEx((_variant_t)(pts[0].y-1000)));

	long scale = pDoc->GetDlgDataSource()->GetScale();
	m_mapValues.SetValue(0,JBFIELD_MAPSCALE,&CVariantEx((_variant_t)scale));
	m_mapValues.SetValue(0,JBFIELD_MAPSCALE0,&CVariantEx((_variant_t)scale));
	m_mapValues.SetValue(0,JBFIELD_CONTOURINTERVAL,&CVariantEx((_variant_t)(long)10));
	m_mapValues.SetValue(0,JBFIELD_CONTOURINTERVAL0,&CVariantEx((_variant_t)(long)10));
	m_mapValues.SetValue(0,JBFIELD_COORDINATESCALE,&CVariantEx((_variant_t)(double)1.0));

	//设置层数据
	m_mapValues.SetValue(0,(LPCTSTR)m_strLayNumField,&CVariantEx((_variant_t)(long)18));
	CreateLayerValues(18);

	for( int i=0; i<18; i++)
	{
		m_layValues.SetValue(i,JBFIELD_LAYNAME,&CVariantEx((_variant_t)(LPCTSTR)groupNameList[i]));
	}
}


void CJBMetaData::RefreshBigNumber()
{
	const CVariantEx *p = NULL;
	if( m_mapValues.GetValue(0,JBFIELD_STRIPNUM,p) )
	{
		double xoff = (double)(_variant_t)*p;
		xoff *= 1e+6;

		if( m_mapValues.GetValue(0,JBFIELD_MAPLBX,p) )
		{
			double x = (double)(_variant_t)*p;
			x -= (floor(x/1e+6)*1e+6);
			x += xoff;
			m_mapValues.SetValue(0,JBFIELD_MAPLBX,&CVariantEx((_variant_t)x));
		}
		if( m_mapValues.GetValue(0,JBFIELD_MAPRBX,p) )
		{
			double x = (double)(_variant_t)*p;
			x -= (floor(x/1e+6)*1e+6);
			x += xoff;
			m_mapValues.SetValue(0,JBFIELD_MAPRBX,&CVariantEx((_variant_t)x));
		}
		if( m_mapValues.GetValue(0,JBFIELD_MAPRTX,p) )
		{
			double x = (double)(_variant_t)*p;
			x -= (floor(x/1e+6)*1e+6);
			x += xoff;
			m_mapValues.SetValue(0,JBFIELD_MAPRTX,&CVariantEx((_variant_t)x));
		}
		if( m_mapValues.GetValue(0,JBFIELD_MAPLTX,p) )
		{
			double x = (double)(_variant_t)*p;
			x -= (floor(x/1e+6)*1e+6);
			x += xoff;
			m_mapValues.SetValue(0,JBFIELD_MAPLTX,&CVariantEx((_variant_t)x));
		}
		if( m_mapValues.GetValue(0,JBFIELD_ORIGINX,p) )
		{
			double x = (double)(_variant_t)*p;
			x -= (floor(x/1e+6)*1e+6);
			x += xoff;
			m_mapValues.SetValue(0,JBFIELD_ORIGINX,&CVariantEx((_variant_t)x));
		}
	}

	if( m_mapValues.GetValue(0,JBFIELD_JIZHUN,p) )
	{
		CString jizhun = (LPCTSTR)(_bstr_t)*p;
		if( jizhun.CompareNoCase("1954年北京坐标系")==0 )
		{
			m_mapValues.SetValue(0,JBFIELD_LONGRAIUS,&CVariantEx((_variant_t)(double)6378245));
			m_mapValues.SetValue(0,JBFIELD_FLATTENING,&CVariantEx((_variant_t)(double)(1.0/298.3)));
		}
		else if( jizhun.CompareNoCase("1980年西安坐标系")==0 )
		{
			m_mapValues.SetValue(0,JBFIELD_LONGRAIUS,&CVariantEx((_variant_t)(double)6378140));
			m_mapValues.SetValue(0,JBFIELD_FLATTENING,&CVariantEx((_variant_t)(double)(1.0/298.25722101)));
		}
	}
}


JBFormatDefineItem CJBMetaData::FindMapDefine(LPCTSTR field)
{
	for(int i=0; i<m_mapItems.GetSize(); i++)
	{
		if( stricmp(m_mapItems[i].name,field)==0 )
		{
			return m_mapItems[i];
		}
	}

	JBFormatDefineItem item;
	memset(&item,0,sizeof(item));

	return item;
}
