// ObjectXmlIO.cpp: implementation
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ObjectXmlIO.h"
#include "TextFile.h"
#include "Geometry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
MyNameSpaceBegin

void SplitToTypeAndData(LPCTSTR str, char *type, char *data)
{
	CString text = str;
	int pos = text.Find(']');
	if( pos<=1 )return;
	strcpy(type,text.Left(pos+1));

	char *buf = text.GetBuffer(0)+pos+1;
	while( *buf==' ' )buf++;
	strcpy(data,buf);
}

BOOL Xml_ReadShape(CString& data, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	{			
		int len = data.GetLength();
		char *pBuf = data.GetBuffer(len), *pStart, *pStop, *pMax;
		
		double value;
		PT_3DEX pt;
		
		pStart = pBuf; pStop = pStart; pMax = pBuf+len;
		while( pStop<pMax )
		{
			value = strtod(pStart,&pStop);
			if( pStop>=pMax || pStop==pStart )break;
			pt.x = value;
			pStart = pStop;
			
			value = strtod(pStart,&pStop);
			if( pStop>=pMax || pStop==pStart )break;
			pt.y = value;
			pStart = pStop;
			
			value = strtod(pStart,&pStop);
			if( pStop>=pMax || pStop==pStart )break;
			pt.z = value;
			pStart = pStop;
			
			value = strtod(pStart,&pStop);
			if( pStop>=pMax || pStop==pStart )break;
			pt.pencode = (int)value;
			pStart = pStop;
			
			arrPts.Add(pt);
		}
		
		data.ReleaseBuffer();
	}
	return TRUE;
}


BOOL Xml_WriteShape(CString& data, CArray<PT_3DEX,PT_3DEX>& arrPts)
{	
	data.Empty();

	PT_3DEX pt;
	CString strPt;
	int nsz = arrPts.GetSize();
	for( int k=0; k<nsz; k++)
	{
		pt = arrPts.GetAt(k);
		strPt.Format(_T("%.6f %.6f %.6f %d "),pt.x,pt.y,pt.z,pt.pencode);
		data += strPt;
	}

	return TRUE;
}

BOOL Xml_ReadValueTable(CMarkup& xml, CValueTable& tab)
{
	CString tag, data, data2;
	char	t1[256], *t2;
	int		v1;
	double	v2;
	CVariantEx var;
	
	while( xml.FindElem(NULL) )
	{
		tag = xml.GetTagName();
		data = xml.GetData();

		t2 = new char[data.GetLength()+1];
		if( !t2 )continue;
		
		memset(t1,0,sizeof(t1));
		memset(t2,0,data.GetLength()+1);
		v1 = 0; v2 = 0;
		
		SplitToTypeAndData(data,t1,t2);
		
		if( stricmp(t1,"[XYZ]")==0 )
		{
			CArray<PT_3DEX,PT_3DEX> arrPts;
			Xml_ReadShape(CString(t2),arrPts);
			
			if( arrPts.GetSize()>0 )
			{
				var.SetAsShape(arrPts);
			}
		}
		else if( stricmp(t1,"[COLOR]")==0 )
		{
			sscanf(t2,"%d",&v1);
			var	 = (_variant_t)(long)v1;
		}
		else if( stricmp(t1,"[BOOL]")==0 )
		{
			sscanf(t2,"%d",&v1);
			var	 = (_variant_t)(bool)v1;
		}
		else if( stricmp(t1,"[BYTE]")==0 )
		{
			sscanf(t2,"%d",&v1);
			var	 = (_variant_t)(BYTE)v1;
		}
		else if( stricmp(t1,"[INT]")==0 )
		{
			sscanf(t2,"%d",&v1);
			var	 = (_variant_t)(long)v1;
		}
		else if( stricmp(t1,"[FLOAT]")==0 )
		{
			sscanf(t2,"%lf",&v2);
			var	 = (_variant_t)(float)v2;
		}
		else if( stricmp(t1,"[DOUBLE]")==0 )
		{
			sscanf(t2,"%lf",&v2);
			var	 = (_variant_t)(double)v2;
		}
		else if( stricmp(t1,"[STRING]")==0 )
		{
			var	 = (_variant_t)(LPCTSTR)(t2);
		}
		else 
		{
			delete[] t2;
			continue;
		}

		delete[] t2;
		
		tab.AddValue(tag,&var);
	}
	return TRUE;
}


BOOL Xml_WriteValueTable(CMarkup& xml, CValueTable& tab)
{
	int m = tab.GetFieldCount();
	int n = tab.GetItemCount();
	CString field, name, data;
	const CVariantEx *var;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int type;
	
	for( int i=0; i<n; i++)
	{
		for( int j=0; j<m; j++)
		{
			tab.GetField(j,field,type,name);
			if( !tab.GetValue(i,j,var) )
				continue;
			
			data.Empty();
			switch( var->GetType() ) 
			{			
			case VT_I1:
			case VT_UI1:
				data.Format("[INT] %d",(BYTE)(_variant_t)*var);
				break;
			case VT_I2:
			case VT_UI2:
				data.Format("[INT] %d",(short)(_variant_t)*var);
				break;
			case VT_I4:
			case VT_UI4:
			case VT_INT:
			case VT_UINT:
				data.Format("[INT] %d",(long)(_variant_t)*var);
				break;
			case VT_BSTR:
				data.Format("[STRING] %s",(LPCTSTR)(_bstr_t)(_variant_t)*var);
				break;
			case VT_BOOL:
				data.Format("[BOOL] %d",((bool)(_variant_t)*var)?1:0);
				break;
			case VT_R4:
				data.Format("[FLOAT] %f",(float)(_variant_t)*var);
				break;
			case VT_R8:
				data.Format("[DOUBLE] %.6f",(double)(_variant_t)*var);
				break;
			case VT_EX_SHAPE:
				{
					arrPts.RemoveAll();
					var->GetShape(arrPts);
					
					Xml_WriteShape(data,arrPts);

					data = "[XYZ] " + data;
				}
				break;
			default:;
			}
			
			if( !data.IsEmpty() )
			{
				xml.AddElem(field,data);
			}
		}
	}	
	
	return TRUE;
}


BOOL Xml_ReadTextFile(CMarkup& xml, LPCTSTR path, BOOL bUnicode)
{
	if( bUnicode )
	{
		CString strDoc;
		CTextFileRead rfile(path);
		rfile.Read(strDoc);
		return xml.SetDoc(strDoc);
	}
	else
	{
		return xml.Load(path);
	}
}


BOOL Xml_WriteTextFile(CMarkup& xml, LPCTSTR path, BOOL bUnicode)
{
	if( bUnicode )
	{
		CString strDoc = xml.GetDoc();
		CTextFileWrite wfile(path, CTextFileBase::UNI16_LE);
		if (wfile.IsOpen())
		{
			wfile.SetCodePage(0);
			wfile<<strDoc;
			return TRUE;
		}
	}

	CString strDoc = xml.GetDoc();

	return xml.WriteTextFile(path,strDoc);
}

MyNameSpaceEnd