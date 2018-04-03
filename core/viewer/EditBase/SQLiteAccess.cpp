// SQLiteAccess.cpp: implementation of the CSQLiteAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SQLiteAccess.h"
#include "DataSourceEx.h"
#include "Scheme.h"
#include "GeoPoint.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "SmartViewFunctions.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin


BOOL  VariantToTextA(const CVariantEx& v, CStringA &text)
{
	switch(v.m_variant.vt) 
	{
	case VT_UI1:
		text.Format("%i",v.m_variant.bVal);
		break;
	case VT_UI2:
		text.Format("%i",v.m_variant.uiVal);
		break;
	case VT_UI4:
		text.Format("%i",v.m_variant.ulVal);
		break;
	case VT_UINT:
		text.Format("%i",v.m_variant.uintVal);
		break;
	case VT_I1:
		text.Format("%c",v.m_variant.cVal);
		break;
	case VT_I2:
		text.Format("%i",v.m_variant.iVal);
		break;	
	case VT_I4:
		text.Format("%i",(long)(_variant_t)v);
		break;
	case VT_INT:
		text.Format("%i",(long)(_variant_t)v);
		break;
	case VT_BOOL:
		text.Format("%i",(bool)(_variant_t)v);
		break;
	case VT_R4:
		text.Format("%f",(float)(_variant_t)v);
		break;
	case VT_R8:
		text.Format("%f",(double)(_variant_t)v);
		break;
	case VT_BSTR:
		text.Format("%s",(LPCSTR)(_bstr_t)(_variant_t)v);
		break;
	default:
		text.Empty();
		return FALSE;
	}
	
	return TRUE;
}

CString EncodeString(const char *p)
{
	if( p==NULL )return CString();

	char tmp[16] = {0};
	CString ret;
	while( *p!=0 )
	{
		sprintf(tmp,"%x",(BYTE)*p);
		ret = ret + tmp;
		p++;
	}
	return ret;
}

CString DecodeString(const char *p)
{
	if( p==NULL )return CString();

	char tmp[4] = {0};
	CString ret;
	int v;
	while( p[0]!=0 && p[1]!=0 )
	{
		tmp[0] = p[0];
		tmp[1] = p[1];

		v = 0;
		if( sscanf(tmp,"%x",&v)<=0 )
			break;

		ret = ret + (char)v;
		p+=2;
	}
	
	return ret;
}


LPCSTR ReplaceQuotationMark(LPCSTR text)
{
	static CString str;
	str = text;
	str.Replace("'","''");
	return str;
}


template<class T>
inline void ByteBuf_PutValue(BYTE *& pBuf, T x)
{
	*(T*)pBuf = x;
	pBuf += sizeof(T);
}

template<class T>
inline void ByteBuf_GetValue(const BYTE *& pBuf, T& x)
{
	x = *(T*)pBuf;
	pBuf += sizeof(T);
}


//返回值在4~-16之间，使得半个字节可以表示
static int DecimalNum(double x)
{
	static double v[] = {
		1e+3,1e+2,1e+1,1e+0,1e-1,1e-2,1e-3,1e-4,1e-5,1e-6,
		1e-7,1e-8,1e-9,1e-10,1e-11,1e-13,1e-14,1e-15,1e-16,1e-17
	};

	//相邻点的坐标值的位数是相近的
	static int last_index = 5;
	int i = last_index;

	if( x<0 )x = -x;

	if( i!=5 )
	{
		if( i<4 && i>-16 && x<v[3-i] && x>=v[4-i] )
			return i;
		
		if( i==4 && x>=v[0] )
			return 4;
		
		if( i==-16 && x<v[19] )
			return -16;
	}
	
	for( i=4; i>-16; i--)
	{
		if( x>=v[4-i] )return (last_index=i);
	}
	return (last_index=i);
}

static double DecimalValue(int n)
{
	//绝对值大于等于 1 的浮点数需要乘的系数，index 是整数部分的位数，index 最大为 4
	static double v1[] = {1e+9,1e+8,1e+7,1e+6
	};

	//绝对值小于 1 的浮点数需要乘的系数，index 是该浮点数的第一个非零数的小数点位数
	//index 最大为 16
	static double v2[] = {1e+9,1e+10,1e+11,1e+12,1e+13,1e+14,
		1e+15,1e+16,1e+17,1e+18,1e+19,1e+20,1e+21,1e+22,1e+23,
		1e+24,1e+25,1e+26
	};

	if( n>=0 )
	{
		return v1[n];
	}
	else
	{
		return v2[-n];
	}
}


// 压缩之后的格式:
// 标识字符串(16字节)+点数(4字节)+N个标志(3N个字节)+N个顶点（每个顶点为(3INT+BYTE)或者(3DBL+BYTE)）
// 无表示字符串则为一般格式

bool EncodePt3dex(const PT_3DEX *pts, int num, BYTE **ppBuf, int* bufSize)
{
	//点太少 则不压缩
	if( num<=16 )
	{
		goto _exit_false;
	}

	*bufSize = sizeof(PT_3DEX)*2*num;
	*ppBuf = new BYTE[*bufSize];

	if( *ppBuf==NULL )
	{
		goto _exit_false;
	}

	if( 1 )
	{
		BYTE *pBuf = *ppBuf;
		memcpy(pBuf,"Pt3dexCompress1",16);
		pBuf = pBuf + 16;
		
		ByteBuf_PutValue(pBuf,num);
		
		BYTE *pb = pBuf;
		BYTE *pXYZ = pBuf + num + num + num;

		ByteBuf_PutValue(pXYZ, pts[0]);
		
		const PT_3DEX *p1 = pts+1;
		PT_3DEX tpt;
		PT_3DEX *p2 = &tpt;
		
		//作差分
		double k;
		int digitx, digity, digitz;

		for( int i=1; i<num; i++,p1++,pb+=3)
		{
			p2->x = p1->x-(p1-1)->x;
			p2->y = p1->y-(p1-1)->y;
			p2->z = p1->z-(p1-1)->z;

			//为避免误差传播，每10个点，就重新记录一次精确坐标
			if( (i%10)==0 )
			{
				ByteBuf_PutValue(pXYZ,p1->x);
				ByteBuf_PutValue(pXYZ,p1->y);
				ByteBuf_PutValue(pXYZ,p1->z);
				ByteBuf_PutValue(pXYZ,(BYTE)p1->pencode);
				ByteBuf_PutValue(pXYZ,(BYTE)p1->type);
				ByteBuf_PutValue(pXYZ,p1->wid);
				pb[0] = pb[1] = pb[2] = 0;

				continue;
			}
			
			digitx = DecimalNum(p2->x);
			digity = DecimalNum(p2->y);			
			digitz = DecimalNum(p2->z);

			if( digitx>=4 )
			{
				ByteBuf_PutValue(pXYZ,p1->x);
			}
			else
			{
				k = DecimalValue(digitx);
				ByteBuf_PutValue(pXYZ,(int)(p2->x*k));
			}

			if( digity>=4 )
			{
				ByteBuf_PutValue(pXYZ,p1->y);
			}
			else
			{
				k = DecimalValue(digity);
				ByteBuf_PutValue(pXYZ,(int)(p2->y*k));
			}

			if( digitz>=4 )
			{
				ByteBuf_PutValue(pXYZ,p1->z);
			}
			//小于 1e-8 的 z，就忽略
			else if( digitz<=-8 );
			else
			{
				k = DecimalValue(digitz);
				ByteBuf_PutValue(pXYZ,(int)(p2->z*k));
			}

			ByteBuf_PutValue(pXYZ,(BYTE)p1->pencode);
			ByteBuf_PutValue(pXYZ,(BYTE)p1->type);
			
			pb[0] = 4-digitx;
			pb[1] = 4-digity;

			if( digitz<=-8 )
			{
				pb[2] = 0x80;
			}
			else
			{
				pb[2] = 4-digitz;
			}

			// 节点线宽
			if (fabs(p1->wid) < 1e-8)
			{
				pb[2] |= 0x40;
			}
			else
			{
				ByteBuf_PutValue(pXYZ,p1->wid);
			}
		}

		*bufSize = pXYZ-*ppBuf;

		// 4 字节对齐
		*bufSize = (*bufSize + 3)&(~3);
		
		return true;
	}

_exit_false:

	*bufSize = num * sizeof(PT_3DEX);
	*ppBuf = new BYTE[*bufSize];
	if( *ppBuf )
	{
		memcpy(*ppBuf,pts,*bufSize);
	}
	else
	{
		*bufSize = 0;
	}

	return false;
}


bool DecodePt3dex(int version, const BYTE *pBuf, int bufSize, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	if( bufSize<16 )
		return false;

	if( version==CSQLiteAccess::version40 )
	{
		//4.0没有压缩格式
		struct PT_3DEX_old
		{
			double x,y,z;
			int pencode;
		};

		int npt = bufSize/sizeof(PT_3DEX_old);
		arrPts.SetSize(npt);

		PT_3DEX *pts = arrPts.GetData();
		PT_3DEX_old *pts0 = (PT_3DEX_old*)pBuf;
		for( int i=0; i<npt; i++)
		{
			pts[i].x = pts0[i].x;
			pts[i].y = pts0[i].y;
			pts[i].z = pts0[i].z;
			pts[i].pencode = pts0[i].pencode;
		}
		return true;
	}

	char buf[20] = {0};
	memcpy(buf,pBuf,16);
	int npt = 0;

	if( strcmp(buf,"Pt3dexCompress1")!=0 )
	{
		npt = bufSize/sizeof(PT_3DEX);
		arrPts.SetSize(npt);
		memcpy(arrPts.GetData(),pBuf,bufSize);	
		return true;
	}

	if( bufSize<=68 )
		return false;

	//点数
	pBuf = pBuf + 16;
	ByteBuf_GetValue(pBuf,npt);

	if( npt<=16 )
		return false;

	if( bufSize<=(20+npt*3) )
		return false;

	const BYTE *pBufPts = pBuf + npt + npt + npt;
	const BYTE *pb = pBuf;
	BYTE pencode, type;
	int value, digitx, digity, digitz;
	double k;

	arrPts.SetSize(npt);

	//起点
	PT_3DEX *p2 = arrPts.GetData();
	ByteBuf_GetValue(pBufPts,p2[0]);
	p2++;

	for( int i=1; i<npt; i++, pb+=3, p2++)
	{
		digitx = 4-pb[0];
		digity = 4-pb[1];
		digitz = 4-(int)(pb[2]&0x0f);

		if( digitx>=4 )
		{
			ByteBuf_GetValue(pBufPts,p2->x);
		}
		else
		{
			k = 1.0/DecimalValue(digitx);
			ByteBuf_GetValue(pBufPts,value); p2->x = value*k;

			p2->x += (p2-1)->x;
		}

		if( digity>=4 )
		{
			ByteBuf_GetValue(pBufPts,p2->y);
		}
		else
		{
			k = 1.0/DecimalValue(digity);
			ByteBuf_GetValue(pBufPts,value); p2->y = value*k;

			p2->y += (p2-1)->y;
		}

		if( pb[2]==0 || pb[2]==0x40)
		{
			ByteBuf_GetValue(pBufPts,p2->z);
		}
		else if( pb[2]&0x80 )
		{
			p2->z = (p2-1)->z;
		}
		else
		{
			k = 1.0/DecimalValue(digitz);
			ByteBuf_GetValue(pBufPts,value); p2->z = value*k;

			p2->z += (p2-1)->z;
		}

		ByteBuf_GetValue(pBufPts,pencode);
		p2->pencode = pencode;

		ByteBuf_GetValue(pBufPts,type);
		p2->type = type;

		if( pb[2]&0x40 )
		{
			p2->wid = 0;
		}
		else
		{
			ByteBuf_GetValue(pBufPts,p2->wid);
		}

	}

	return true;
}


static void BindPts(CppSQLite3Statement& stm, int index, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	BYTE *pBuf = NULL;
	int size = 0;
	if( !EncodePt3dex(arrPts.GetData(),arrPts.GetSize(),&pBuf,&size) )
	{
		stm.bind(index,(BYTE*)arrPts.GetData(),arrPts.GetSize()*sizeof(PT_3DEX));
		return;
	}

	if( size>0 )
	{
		stm.bind(index,pBuf,size);
	}
	if( pBuf )
	{
		delete[] pBuf;
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSQLiteAccess::CSQLiteAccess()
{
	m_nCurLayID = -1;
	m_nAccessMode = modeRead;
	m_bHaveCheckedVersion = FALSE;
	m_bFileVersionLatest = TRUE;
	CppSQLite3DB::m_nErrorMode = CppSQLite3DB::errModeFile;

	m_nWriteCount = 0;
	m_bBeginTraction = FALSE;
	m_nFileVersion = version41;
}

CSQLiteAccess::~CSQLiteAccess()
{
	if(m_db.isValid())
		Close();
}

CppSQLite3DB *CSQLiteAccess::GetSqliteDb()
{
	return &m_db;
}
CppSQLite3Query *CSQLiteAccess::GetSqliteFtrQuery()
{
	return &m_ftrQuery;
}
CppSQLite3Statement *CSQLiteAccess::GetSqliteStm()
{
	return &m_stm;
}

BOOL CSQLiteAccess::Attach(LPCTSTR fileName)
{
	if( fileName==NULL )
		return FALSE;
	if( _tcslen(fileName)<=0 )
		return FALSE;

	wchar_t path2[1024] = {0};
	char path3[1024] = {0};

#ifdef _UNICODE
	_tcscpy(path2,fileName);
#else
	MultiByteToWideChar(CP_ACP,0,fileName,_tcslen(fileName),path2,1024);
#endif
	WideCharToMultiByte(CP_UTF8,0,path2,_tcslen(fileName),path3,1024,NULL,NULL);
	m_strReadName = m_strWriteName = fileName;
	m_db.open(path3);

	m_bValid = TRUE;
	m_nAccessMode = modeModify;

	if( !m_db.isValid() )
		m_bValid = FALSE;
	else if( !m_db.tableExists("WorkSpace") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Point") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Line") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Surface") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Text") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("DLGVectorLayer") )
		m_bValid = FALSE;

	if (m_bValid)
	{
		m_bFileVersionLatest = IsFileVersionLatest();

		CString strVersion = GetFileVersion();

		if( strVersion.Find(_T("FeatureOne4.0"))>=0 )
			m_nFileVersion = version40;
		else if( strVersion.Find(_T("FeatureOne4.1"))>=0 )
			m_nFileVersion = version41;	
		else if( strVersion.IsEmpty() )
			m_nFileVersion = version40;

	}
	return m_bValid;
}

BOOL CSQLiteAccess::OpenNew(LPCTSTR fileName)
{
	if( fileName==NULL )
		return FALSE;
	if( _tcslen(fileName)<=0 )
		return FALSE;
	
	wchar_t path2[1024] = {0};
	char path3[1024] = {0};
#ifdef _UNICODE
	_tcscpy(path2,fileName);
#else
	MultiByteToWideChar(CP_ACP,0,fileName,_tcslen(fileName),path2,1024);
#endif
	WideCharToMultiByte(CP_UTF8,0,path2,_tcslen(fileName),path3,1024,NULL,NULL);
	m_strReadName = m_strWriteName = fileName;
	m_db.open(path3);
	
	m_bValid = m_db.isValid();
	m_nAccessMode = modeModify;
	
	return m_bValid;
}

BOOL CSQLiteAccess::IsValid()
{
	return m_bValid;
}


BOOL CSQLiteAccess::Close()
{
	//清理扩展属性表中的DEL项	
// 	CppSQLite3Query  ftrQuery;
// 	ftrQuery = m_db.execQuery("select name from sqlite_master where type='table';");
// 	if (!ftrQuery.eof())
// 	{	
// 		const char* strTable;
// 		char line[1024];
// 		do
// 		{
// 			strTable = ftrQuery.getStringField("name");
// 			if(strncmp(strTable,"AttrTable_",10)==0)
// 			{
// 				sprintf(line,"delete from %s where DELETED = 1;",strTable);
// 				m_db.execDML(line);
// 			}
// 			ftrQuery.nextRow();
// 		}
// 		while(!ftrQuery.eof());
// 	}
// 	ftrQuery.finalize();
	if (!m_db.isValid()) return FALSE;
	if( m_nAccessMode!=modeRead )
		ClearAttrTables();
	m_stm.finalize();
	m_layerQuery.finalize();
	m_ftrQuery.finalize();
	m_db.close();
		
	return TRUE;
}

void CSQLiteAccess::DeleteTables(CScheme *ps)
{
	if (m_db.tableExists("WorkSpace"))
		m_db.execDML("drop table WorkSpace;");
	if (m_db.tableExists("DLGVectorLayer"))
		m_db.execDML("drop table DLGVectorLayer;");
	if (m_db.tableExists("Point"))
		m_db.execDML("drop table Point;");
	if (m_db.tableExists("Line"))
		m_db.execDML("drop table Line;");
	if (m_db.tableExists("Surface"))
		m_db.execDML("drop table Surface;");
	if (m_db.tableExists("Text"))
		m_db.execDML("drop table Text;");
	if (m_db.tableExists("DataSettings"))
		m_db.execDML("drop table DataSettings;");
	if (m_db.tableExists("DataInfo"))
		m_db.execDML("drop table DataInfo;");

	int nLayerCnt = ps->GetLayerDefineCount();
	CSchemeLayerDefine *pLayerDef = NULL;	
	
	for(int i=0;i<nLayerCnt;i++)
	{
		pLayerDef = ps->GetLayerDefine(i);
		if(!pLayerDef)continue;

		CString ename = EncodeString((LPCSTR)pLayerDef->GetLayerName());

		CString tableName;
		tableName.Format("AttrTable_%s",ename);

		if (m_db.tableExists(tableName))
		{
			CString line;
			line.Format("drop table %s",tableName);
			m_db.execDML(line);			
		}

				
		
	}
}


BOOL CSQLiteAccess::CreateFileSys(long Scale,CScheme *ps)
{
	//清空文件内容
	CString fileName = m_db.getFileName();
	if( fileName.IsEmpty() )
		return FALSE;
	
	if (m_db.isValid())
	{
 		DeleteTables(ps);
		m_db.close();
	}
	wchar_t path2[1024] = {0};
	char path3[1024] = {0};
	MultiByteToWideChar( CP_UTF8,0,fileName,strlen(fileName),path2,1024);
	WideCharToMultiByte(CP_ACP,0,path2,strlen(fileName),path3,1024,NULL,NULL);
	::DeleteFile(path3);
	
	m_db.open(fileName);

	BatchUpdateBegin();
	CreateWorkspaceTable();
	CreateVectorLayerTable();
	CreatePointTable();
	CreateLineTable();
	CreateSurfaceTable();
	CreateTextTable();
	CreateDataSettingsTable();
	CreateObjectGroupTable();
	CreateFtrLayerGroupTable();
	CreateFilePreViewImage();
	CreateCellDefTable();

	if( ps!=NULL )
	{
		CreateLayerItems(ps);		
		CreateXAttributeTables(ps);		
	}
	//创建缺省层
	CreateDefaultLayer();
	BatchUpdateEnd();

	m_bValid = TRUE;

	return TRUE;
}

void CSQLiteAccess::CreateLayerItems(CScheme *ps)
{
	//m_db.execDML("create table DLGVectorLayer(ID INT PRIMARY KEY, Name VARCHAR, MapName VARCHAR, Color int, Visible int, Locked int);");
	char line[1024] = {0};
	char line0[256]={0};
	CString str;
	
	int nLayerCnt = ps->GetLayerDefineCount();
	CSchemeLayerDefine *pLayerDef = NULL;	
	
	int id=5;
	for(int i=0;i<nLayerCnt;i++)
	{
		pLayerDef = ps->GetLayerDefine(i);
		if(!pLayerDef || pLayerDef->GetGroupName().CompareNoCase(StrFromResID(IDS_SPECIALGROUP)) == 0)continue;
		sprintf(line,"insert into  DLGVectorLayer values(%d,'%s','','%s',%d,1,0,1,-1,1);",id,(LPCSTR)pLayerDef->GetLayerName(),(LPCSTR)pLayerDef->GetGroupName(),pLayerDef->GetColor());
		m_db.execDML(line);	

		CString ename = EncodeString((LPCSTR)pLayerDef->GetLayerName());

		str.Format("create table AttrTable_%s(FTRID char[32] PRIMARY KEY, DELETED INT",(LPCSTR)ename);
		int num;
		const XDefine* xDef = pLayerDef->GetXDefines(num);
		for (int j=0;j<num;j++)
		{			
			switch(xDef[j].valuetype)
			{
			case DP_CFT_COLOR:
			case DP_CFT_SMALLINT:				
			case DP_CFT_INTEGER:				
			case DP_CFT_BOOL:
				sprintf(line0,", %s_ INT",xDef[j].field);
				break;
			case DP_CFT_FLOAT:
			case DP_CFT_DOUBLE:
				sprintf(line0,", %s_ FLOAT",xDef[j].field);
			    break;
			case DP_CFT_VARCHAR:
			case DP_CFT_DATE:
				sprintf(line0,", %s_ VARCHAR",xDef[j].field);
			    break;
			default:
				sprintf(line0,", %s_ VARCHAR",xDef[j].field);
			    break;
			}
			str += line0;			
		}
		str += ");";
		if(num>0)
			m_db.execDML(str);
		id++;
	}	
}


void CSQLiteAccess::CreateXAttributeTables(CScheme *ps)
{
	
}


void CSQLiteAccess::CreateWorkspaceTable()
{
	m_db.execDML("create table WorkSpace(ID INT PRIMARY KEY, Scale INT, Bound BLOB, Zmin FLOAT, Zmax FLOAT);");
	m_db.execDML("insert into  WorkSpace values(0,2000,null,-1000.0,1000.0);");
	
}


void CSQLiteAccess::CreateVectorLayerTable()
{
	m_db.execDML("create table DLGVectorLayer(ID INT PRIMARY KEY, Name VARCHAR,\
		MapName VARCHAR, GroupName VARCHAR, Color INT, Visible INT, Locked INT,Inherent INT,\
		DisplayOrder INT, Symbolized INT);");
}


void CSQLiteAccess::CreatePointTable()
{
	m_db.execDML("create table Point(FTRID char[32] PRIMARY KEY, ClassType INT,\
		GeoClass INT, Color INT, SymbolizeFlag INT, LayerID INT, Angle FLOAT,\
		Length FLOAT, Width FLOAT, SymbolName char[32], Shape BLOB, FtrCode char[16],\
		Visible INT, Kx FLOAT, Ky FLOAT, CoverType INT,ExtendDis FLOAT,\
		FtrPurpose INT,DisplayOrder FLOAT,ObjectGroup BLOB, ModifyTime INT);");
}


void CSQLiteAccess::CreateLineTable()
{
	m_db.execDML("create table Line(FTRID char[32] PRIMARY KEY, ClassType INT,\
		GeoClass INT, Color INT, SymbolizeFlag INT, LayerID INT, SymbolName char[32],\
		Shape BLOB, Width FLOAT,DHeight FLOAT, FtrCode char[16], Visible INT,\
		LinetypeScale FLOAT, LinewidthScale FLOAT,FtrPurpose INT,LineWidth FLOAT,LinetypeXoff FLOAT,DisplayOrder FLOAT, ObjectGroup BLOB,\
		ModifyTime INT);");
}

void CSQLiteAccess::CreateSurfaceTable()
{
	m_db.execDML("create table Surface(FTRID char[32] PRIMARY KEY, ClassType INT,\
		GeoClass INT, Color INT, SymbolizeFlag INT, LayerID INT, SymbolName char[32],\
		Shape BLOB, FtrCode char[16], Visible INT, CellScale FLOAT, CellAngle FLOAT,\
		IntvScale FLOAT, XStartOff FLOAT, YStartOff FLOAT, FtrPurpose INT,LineWidth FLOAT,DisplayOrder FLOAT, ObjectGroup BLOB,\
		ModifyTime INT);");
}


void CSQLiteAccess::CreateTextTable()
{
	m_db.execDML("create table Text(FTRID char[32] PRIMARY KEY, ClassType INT, GeoClass INT,\
		Color INT, SymbolizeFlag INT, LayerID INT, SymbolName char[32], Content VARCHAR, Font char[32],\
	 CharWidthR FLOAT, CharHeight FLOAT, CharIntvR FLOAT, LineIntvR FLOAT, AlignType INT, TextAngle FLOAT, CharAngle FLOAT, \
		Incline INT, InclineAngle FLOAT, PlaceType INT, OtherFlag INT, Shape BLOB, FtrCode char[16], Visible INT, \
		CoverType INT, ExtendDis FLOAT ,FtrPurpose INT,DisplayOrder FLOAT, ObjectGroup BLOB,\
		ModifyTime INT);");
}

void CSQLiteAccess::CreateObjectGroupTable()
{
	m_db.execDML("create table ObjectGroup(ID INT PRIMARY KEY, Name char[16], Selectable INT);");
}

void CSQLiteAccess::CreateFtrLayerGroupTable()
{
	m_db.execDML("create table FtrLayerGroup(ID INT PRIMARY KEY, Name VARCHAR NOT NULL, Color INT, Visible INT, Symbolized INT);");
}

void CSQLiteAccess::ClearAttrTables()
{
	BatchUpdateBegin();
	//清理扩展属性表中的DEL项	
	CppSQLite3Query  ftrQuery;
	ftrQuery = m_db.execQuery("select name from sqlite_master where type='table';");
	if (!ftrQuery.eof())
	{	
		const char* strTable;
		char line[1024];
		do
		{
			strTable = ftrQuery.getStringField("name");
			if(strncmp(strTable,"AttrTable_",10)==0)
			{
				sprintf(line,"delete from %s where DELETED = 1;",strTable);
				m_db.execDML(line);
			}
			ftrQuery.nextRow();
		}
		while(!ftrQuery.eof());
	}
	ftrQuery.finalize();
	BatchUpdateEnd();
}

BOOL CSQLiteAccess::OpenRead(LPCTSTR filename)
{
	if( filename==NULL )
		return FALSE;
	if( _tcslen(filename)<=0 )
		return FALSE;
	
	wchar_t path2[1024] = {0};
	char path3[1024] = {0};

#ifdef _UNICODE
	_tcscpy(path2,filename);
#else
	MultiByteToWideChar(CP_ACP,0,filename,_tcslen(filename),path2,1024);
#endif
	WideCharToMultiByte(CP_UTF8,0,path2,_tcslen(filename),path3,1024,NULL,NULL);

	m_db.open(path3, SQLITE_OPEN_READONLY);

	m_nAccessMode = modeRead;
	
	m_bValid = TRUE;
	
	if( !m_db.isValid() )
		m_bValid = FALSE;
	else if( !m_db.tableExists("WorkSpace") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Point") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Line") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Surface") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("Text") )
		m_bValid = FALSE;
	else if( !m_db.tableExists("DLGVectorLayer") )
		m_bValid = FALSE;

	if (m_bValid)
	{
		m_bFileVersionLatest = IsFileVersionLatest();

		CString strVersion = GetFileVersion();

		if( strVersion.Find(_T("FeatureOne4.0"))>=0 )
			m_nFileVersion = version40;
		else if( strVersion.Find(_T("FeatureOne4.1"))>=0 )
			m_nFileVersion = version41;	
		else if( strVersion.IsEmpty() )
			m_nFileVersion = version40;

	}
	
	return m_bValid;
}


BOOL CSQLiteAccess::OpenWrite(LPCTSTR filename)
{
	m_strWriteName = filename;
	return Attach(filename);
}


BOOL CSQLiteAccess::CloseRead()
{
	if (m_db.isValid())
	{
		m_db.close();
	}
	return TRUE;
}


BOOL CSQLiteAccess::CloseWrite()
{
	if (m_db.isValid())
	{
		m_db.close();
	}
	return TRUE;
}


//读取数据源信息
BOOL CSQLiteAccess::ReadDataSourceInfo(CDataSourceEx *pDS)
{
	CppSQLite3Query query = m_db.execQuery("select * from WorkSpace;");

	long value = query.getIntField("Scale",2000);

	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();

	tab.AddValue(FIELDNAME_SCALE,&CVariantEx((_variant_t)value));

	int nLen = 0;
	const BYTE *pBuf = query.getBlobField("Bound",nLen);
	int nPt = nLen/sizeof(PT_3DEX);
	if( nPt>0 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.SetSize(nPt);
		memcpy(arrPts.GetData(),pBuf,nPt*sizeof(PT_3DEX));
		
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_BOUND,&var);
	}
	double zmin = query.getFloatField("Zmin",-1000.0);
	tab.AddValue(FIELDNAME_ZMIN,&CVariantEx((_variant_t)zmin));

	double zmax = query.getFloatField("Zmax",1000.0);
	tab.AddValue(FIELDNAME_ZMAX,&CVariantEx((_variant_t)zmax));

	tab.EndAddValueItem();

	pDS->ReadFrom(tab);

	return TRUE;
}

void CSQLiteAccess::ReadSpecialData(CDataSourceEx *pDS)
{
	if (!pDS) return;

	ObjectGroup *pGroup = GetFirstObjectGroup();
	while (pGroup)
	{
		pDS->AddObjectGroup(pGroup,TRUE);
		pGroup = GetNextObjectGroup();
	}
	
	FtrLayerGroup *pFtrLayerGroup = GetFirstFtrLayerGroup();
	while (pFtrLayerGroup)
	{
		pDS->AddFtrLayerGroup(pFtrLayerGroup,TRUE);
		pFtrLayerGroup = GetNextFtrLayerGroup();
	}
	
	CString name, value, type;
	ReadDataSettings(FIELDNAME_STATEDITTIME,name,value,type);
	if (!value.IsEmpty())
	{
		pDS->m_nTotalModifiedTime = _ttol(value);
	}
}

void CSQLiteAccess::WriteSpecialData(CDataSourceEx *pDS)
{
	int nCount = pDS->GetObjectGroupCount();
	
	ObjectGroup *objGroup;
	for(int i=0; i<nCount; i++)
	{
		objGroup = pDS->GetObjectGroup(i);
		if (!objGroup)  continue;
		
		SaveObjectGroup(objGroup);
	}

	nCount = pDS->GetFtrLayerGroupCount();
	
	FtrLayerGroup *layerGroup;
	for( i=0; i<nCount; i++)
	{
		layerGroup = pDS->GetFtrLayerGroup(i);
		if (!layerGroup)  continue;
		
		SaveFtrLayerGroup(layerGroup);
	}
}

//读取层
CFtrLayer *CSQLiteAccess::GetFirstFtrLayer(long &idx)
{
	m_layerQuery = m_db.execQuery("select * from DLGVectorLayer;");
	if( m_layerQuery.eof() )
		return NULL;
	
	CFtrLayer *pLayer = ReadFtrLayer();

	m_layerQuery.nextRow();

	return pLayer;
}


CFtrLayer *CSQLiteAccess::GetNextFtrLayer(long &idx)
{
	if( m_layerQuery.eof() )
		return NULL;
	
	CFtrLayer *pLayer = ReadFtrLayer();

	m_layerQuery.nextRow();
	
	return pLayer;
}

ObjectGroup* CSQLiteAccess::GetFirstObjectGroup()
{
	m_ftrQuery = m_db.execQuery("select * from ObjectGroup;");
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadObjectGroup();
}

ObjectGroup* CSQLiteAccess::GetNextObjectGroup()
{
	m_ftrQuery.nextRow();
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadObjectGroup();	
}

FtrLayerGroup* CSQLiteAccess::GetFirstFtrLayerGroup()
{
	m_ftrQuery = m_db.execQuery("select * from FtrLayerGroup;");
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadFtrLayerGroup();
}

FtrLayerGroup* CSQLiteAccess::GetNextFtrLayerGroup()
{
	m_ftrQuery.nextRow();
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadFtrLayerGroup();
}

CFtrLayer *CSQLiteAccess::GetFtrLayerByID(long id)
{
	CppSQLite3Query save = m_layerQuery;
	char line[1024];
	sprintf(line,"select * from DLGVectorLayer where ID=%d;",id);

	CFtrLayer *pLayer = NULL;
	m_layerQuery = m_db.execQuery(line);
	if( !m_layerQuery.eof() )
	{
		pLayer = ReadFtrLayer();
	}
	
	m_layerQuery = save;
	
	return pLayer;
}

CFtrLayer *CSQLiteAccess::GetFtrLayerByName(LPCTSTR layerName)
{
	CppSQLite3Query save = m_layerQuery;
	char line[1024];
	sprintf(line,"select * from DLGVectorLayer where Name='%s';",layerName);
	CFtrLayer *pLayer = NULL;
	m_layerQuery = m_db.execQuery(line);
	if( !m_layerQuery.eof() )
	{
		pLayer = ReadFtrLayer();
	}
	m_layerQuery = save;
	return pLayer;
}

ObjectGroup* CSQLiteAccess::ReadObjectGroup()
{
	ObjectGroup *pGroup = new ObjectGroup;
	if (!pGroup) return NULL;

	CValueTable tab;
	
	int nValue;
	const char* pstr;
	
	nValue = m_ftrQuery.getIntField("ID");
	pGroup->id = nValue;
	
	pstr = m_ftrQuery.getStringField("Name");
	strncpy(pGroup->name,pstr,15); 
	pGroup->name[15] = '\0';
	
	nValue = m_ftrQuery.getIntField("Selectable");
	pGroup->select = nValue;
	
	return pGroup;
}

FtrLayerGroup* CSQLiteAccess::ReadFtrLayerGroup()
{
	FtrLayerGroup *pGroup = new FtrLayerGroup;
	if (!pGroup) return NULL;
	
	CValueTable tab;
	
	int nValue;
	const char* pstr;
	
	nValue = m_ftrQuery.getIntField("ID");
	pGroup->id = nValue;

	pstr = m_ftrQuery.getStringField("Name");
	pGroup->Name = pstr;
	
	nValue = m_ftrQuery.getIntField("Color");
	pGroup->Color = nValue;

	nValue = m_ftrQuery.getIntField("Visible");
	pGroup->Visible = nValue;

	nValue = m_ftrQuery.getIntField("Symbolized");
	pGroup->Symbolized = nValue;
	
	return pGroup;
}

CFtrLayer *CSQLiteAccess::ReadFtrLayer()
{
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
//	char line[256];
	int nValue;
	const char* pstr;
	bool bValue;


	nValue = m_layerQuery.getIntField("ID");
	tab.AddValue(FIELDNAME_LAYERID,&(CVariantEx)(_variant_t)(long)nValue);

	pstr = m_layerQuery.getStringField("Name");
	tab.AddValue(FIELDNAME_LAYERNAME,&(CVariantEx)(_variant_t)pstr);
	
// 	pstr = m_layerQuery.getStringField("MapName");
// 	tab.AddValue(FIELDNAME_LAYMAPNAME,&(CVariantEx)(_variant_t)pstr);

	pstr = m_layerQuery.getStringField("GroupName");
	tab.AddValue(FIELDNAME_LAYGROUPNAME,&(CVariantEx)(_variant_t)pstr);

	nValue = m_layerQuery.getIntField("Color");
	tab.AddValue(FIELDNAME_LAYCOLOR,&(CVariantEx)(_variant_t)(long)nValue);
	
	bValue = (m_layerQuery.getIntField("Locked")!=0);
	tab.AddValue(FIELDNAME_LAYLOCKED,&(CVariantEx)(_variant_t)bValue);
	
	bValue = (m_layerQuery.getIntField("Visible")!=0);
	tab.AddValue(FIELDNAME_LAYVISIBLE,&(CVariantEx)(_variant_t)bValue);

	bValue = (m_layerQuery.getIntField("Inherent")!=0);
	tab.AddValue(FIELDNAME_LAYINHERENT,&(CVariantEx)(_variant_t)bValue);

	nValue = m_layerQuery.getIntField("DisplayOrder");
	tab.AddValue(FIELDNAME_LAYERDISPLAYORDER,&(CVariantEx)(_variant_t)(long)nValue);

	bValue = (m_layerQuery.getIntField("Symbolized")!=0);
	tab.AddValue(FIELDNAME_LAYERSYMBOLIZED,&(CVariantEx)(_variant_t)bValue);
	
	tab.EndAddValueItem();
	
	CFtrLayer *pLayer = new CFtrLayer();
	if( !pLayer )return NULL;

	pLayer->ReadFrom(tab);
	
	CString strTable;
	//通过层ID获取扩展属性表的名称
	strTable.Format("AttrTable_%s",(LPCSTR)EncodeString(pLayer->GetName()));
	
	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
		pLayer->SetHaveExtraAttr(FALSE);
	else
		pLayer->SetHaveExtraAttr(TRUE);

	return pLayer;
}


CFeature *CSQLiteAccess::GetFirstPoint()
{
	m_ftrQuery = m_db.execQuery("select * from Point;");
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadPoint();
}


CFeature *CSQLiteAccess::GetNextPoint()
{
	m_ftrQuery.nextRow();
	if( m_ftrQuery.eof() )
		return NULL;

	return ReadPoint();
}

CFeature *CSQLiteAccess::ReadPoint()
{
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
	int nValue;
	const char* pstr;
	double lfValue = 0;
	
	pstr = m_ftrQuery.getStringField("FTRID");
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)pstr);
	
	nValue = m_ftrQuery.getIntField("ClassType");
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("GeoClass");
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("Color");
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)nValue);

	nValue = m_ftrQuery.getIntField("SymbolizeFlag",256);
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)nValue);

	pstr = m_ftrQuery.getStringField("SymbolName");
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)pstr);
	
	nValue = m_ftrQuery.getIntField("LayerID");
	m_nCurLayID = nValue;
	
	lfValue = m_ftrQuery.getFloatField("Angle");
	tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&(CVariantEx)(_variant_t)lfValue);

	pstr = m_ftrQuery.getStringField("FtrCode");
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)pstr);

	nValue = m_ftrQuery.getIntField("Visible",1);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)nValue);

	lfValue = m_ftrQuery.getFloatField("Kx",1);
	tab.AddValue(FIELDNAME_GEOPOINT_KX,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("Ky",1);
	tab.AddValue(FIELDNAME_GEOPOINT_KY,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("CoverType",0);
	tab.AddValue(FIELDNAME_GEOPOINT_COVERTYPE,&(CVariantEx)(_variant_t)(long)nValue);

	lfValue = m_ftrQuery.getFloatField("ExtendDis",0);
	tab.AddValue(FIELDNAME_GEOPOINT_EXTENDDIS,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("FtrPurpose",1);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)nValue);

	lfValue = m_ftrQuery.getFloatField("Width");
	tab.AddValue(FIELDNAME_GEOPOINT_WIDTH,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("DisplayOrder",-1);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)lfValue);

	nValue = m_ftrQuery.getIntField("ModifyTime");
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)nValue);
	
	int nLen = 0;
	const BYTE *pBuf = m_ftrQuery.getBlobField("Shape",nLen);
	int nPt = nLen/sizeof(PT_3DEX);
	if( nPt>0 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		//arrPts.SetSize(nPt);
		//memcpy(arrPts.GetData(),pBuf,nPt*sizeof(PT_3DEX));
		DecodePt3dex(m_nFileVersion,pBuf,nLen,arrPts);
		
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}

	nLen = 0;
	pBuf = m_ftrQuery.getBlobField("ObjectGroup",nLen);
	int num = nLen/sizeof(UINT);
	if( num>0 )
	{		
		var.SetAsBlob(pBuf,nLen);		
		tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	}
	
	tab.EndAddValueItem();
	
	CFeature *pFt = (CFeature*)CreateObject(tab);
	
	return pFt;
}

void CSQLiteAccess::SavePoint(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	
	// 查找地物是否存在
	sprintf(line,"select FTRID from Point where FTRID='%s';",(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);

	// 插入新数据
	if( query.eof() )
	{
		sprintf(line,"insert into Point(FTRID,ClassType,GeoClass,Color,SymbolizeFlag,SymbolName,LayerID,Shape,Angle,FtrCode,Visible,Kx,Ky,CoverType,ExtendDis,FtrPurpose,Width,DisplayOrder,ObjectGroup,ModifyTime) values('%s',?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}
	// 更新数据
	else
	{
		sprintf(line,"update Point set ClassType=?, GeoClass=?, Color=?, SymbolizeFlag=?, SymbolName=?, LayerID=?, Shape=?, Angle=?, FtrCode=?, Visible=?, Kx=?, Ky=?, CoverType=?, ExtendDis=?, FtrPurpose=?, Width=?, DisplayOrder=?, ObjectGroup=?, ModifyTime=? where FTRID='%s';",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}
	
	// 填充数据项
	CValueTable tab;
	const CVariantEx *pvar;
	
	tab.BeginAddValueItem();
	pFtr->WriteTo(tab);
	tab.EndAddValueItem();
	
	if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		m_stm.bind(1,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(1);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		m_stm.bind(2,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(2);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		m_stm.bind(3,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(3);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		m_stm.bind(4,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(4);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		m_stm.bind(5,((LPCTSTR)(_bstr_t)(_variant_t)*pvar));
	}
	else
	{
		m_stm.bindNull(5);
	}
	
	m_stm.bind(6,m_nCurLayID);

	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		
		//m_stm.bind(7,(BYTE*)arrPts.GetData(),arrPts.GetSize()*sizeof(PT_3DEX));

		BindPts(m_stm,7,arrPts);
	}
	else
	{
		m_stm.bindNull(7);
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_ANGLE,pvar) )
	{
		m_stm.bind(8,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(8);
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		m_stm.bind(9,((LPCTSTR)(_bstr_t)(_variant_t)*pvar));
	}
	else
	{
		m_stm.bindNull(9);
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		m_stm.bind(10,(bool)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(10);
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_KX,pvar) )
	{
		m_stm.bind(11,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(11);
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_KY,pvar) )
	{
		m_stm.bind(12,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(12);
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_COVERTYPE,pvar) )
	{
		m_stm.bind(13,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(13);
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_EXTENDDIS,pvar) )
	{
		m_stm.bind(14,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(14);
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		m_stm.bind(15,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(15);
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_WIDTH,pvar) )
	{
		m_stm.bind(16,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(16);
	}

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		m_stm.bind(17,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(17);
	}

	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		m_stm.bind(18,pBuf,nLen);
	}
	else
	{
		m_stm.bindNull(18);
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		m_stm.bind(19,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(19);
	}
	
	// 完成操作
	m_stm.execDML();
	m_stm.finalize();	
	query.finalize();
	return;
}


CFeature *CSQLiteAccess::GetFirstLine()
{
	m_ftrQuery = m_db.execQuery("select * from Line;");
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadLine();
}


CFeature *CSQLiteAccess::GetNextLine()
{
	m_ftrQuery.nextRow();
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadLine();
}

CFeature *CSQLiteAccess::ReadLine()
{
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
	int nValue;
	const char* pstr;
	double lfValue = 0;
	
	pstr = m_ftrQuery.getStringField("FTRID");
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)pstr);
	
	nValue = m_ftrQuery.getIntField("ClassType");
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("GeoClass");
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("Color");
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)nValue);

	nValue = m_ftrQuery.getIntField("SymbolizeFlag",256);
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)nValue);

	pstr = m_ftrQuery.getStringField("SymbolName");
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)pstr);
	
	nValue = m_ftrQuery.getIntField("LayerID");
	m_nCurLayID = nValue;

	lfValue = m_ftrQuery.getFloatField("Width");
	tab.AddValue(FIELDNAME_GEOCURVE_WIDTH,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("DHeight");
	tab.AddValue(FIELDNAME_GEOCURVE_DHEIGHT,&(CVariantEx)(_variant_t)lfValue);

	pstr = m_ftrQuery.getStringField("FtrCode");
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)pstr);

	nValue = m_ftrQuery.getIntField("Visible",1);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)nValue);

	lfValue = m_ftrQuery.getFloatField("LinetypeScale",1);
	tab.AddValue(FIELDNAME_GEOCURVE_LINETYPESCALE,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("LinewidthScale",1);
	tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTHSCALE,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("FtrPurpose",1);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)nValue);

	lfValue = m_ftrQuery.getFloatField("LineWidth",0);
	tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTH,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("LinetypeXoff",0);
	tab.AddValue(FIELDNAME_GEOCURVE_LINETYPEXOFF,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("DisplayOrder",-1);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)lfValue);
		
	nValue = m_ftrQuery.getIntField("ModifyTime");
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)nValue);

	int nLen = 0;
	const BYTE *pBuf = m_ftrQuery.getBlobField("Shape",nLen);
	int nPt = nLen/sizeof(PT_3DEX);
	if( nPt>0 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		//arrPts.SetSize(nPt);
		//memcpy(arrPts.GetData(),pBuf,nPt*sizeof(PT_3DEX));
		DecodePt3dex(m_nFileVersion,pBuf,nLen,arrPts);
		if (!m_bFileVersionLatest)
		{
			int num = arrPts.GetSize();
			for (int i=0; i<num; i++)
			{
				arrPts[i].wid = 0;
			}
		}
		
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}

	nLen = 0;
	pBuf = m_ftrQuery.getBlobField("ObjectGroup",nLen);
	int num = nLen/sizeof(UINT);
	if( num>0 )
	{		
		var.SetAsBlob(pBuf,nLen);		
		tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	}
	
	tab.EndAddValueItem();
	
	CFeature *pFt = (CFeature*)CreateObject(tab);
	
	return pFt;
}



void CSQLiteAccess::SaveLine(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	
	// 查找地物是否存在
	sprintf(line,"select FTRID from Line where FTRID='%s';",(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	
	// 插入新数据
	if( query.eof() )
	{
		sprintf(line,"insert into Line(FTRID,ClassType,GeoClass,Color,SymbolizeFlag,SymbolName,LayerID,Shape,Width,DHeight,FtrCode,Visible,LinetypeScale,LinewidthScale,FtrPurpose,LineWidth,LinetypeXoff,DisplayOrder,ObjectGroup,ModifyTime) values('%s',?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}
	// 更新数据
	else
	{
		sprintf(line,"update Line set ClassType=?, GeoClass=?, Color=?, SymbolizeFlag=?, SymbolName=?, LayerID=?, Shape=?, Width=?, DHeight=?, FtrCode=?, Visible=?, LinetypeScale=?, LinewidthScale=?, FtrPurpose=?, LineWidth=?, LinetypeXoff=?, DisplayOrder=?, ObjectGroup=?, ModifyTime=? where FTRID='%s';",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}
	// 填充数据项
	CValueTable tab;
	const CVariantEx *pvar;
	
	tab.BeginAddValueItem();
	pFtr->WriteTo(tab);
	tab.EndAddValueItem();
	
	if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		m_stm.bind(1,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(1);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		m_stm.bind(2,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(2);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		m_stm.bind(3,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(3);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		m_stm.bind(4,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(4);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		m_stm.bind(5,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(5);
	}
	
	m_stm.bind(6,m_nCurLayID);
	
	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		
		//m_stm.bind(7,(BYTE*)arrPts.GetData(),arrPts.GetSize()*sizeof(PT_3DEX));
		BindPts(m_stm,7,arrPts);
	}
	else
	{
		m_stm.bindNull(7);
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_WIDTH,pvar) )
	{
		m_stm.bind(8,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(8);
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_DHEIGHT,pvar) )
	{
		m_stm.bind(9,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(9);
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		m_stm.bind(10,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(10);
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		m_stm.bind(11,(bool)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(11);
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINETYPESCALE,pvar) )
	{
		m_stm.bind(12,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(12);
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINEWIDTHSCALE,pvar) )
	{
		m_stm.bind(13,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(13);
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		m_stm.bind(14,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(14);
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINEWIDTH,pvar) )
	{
		m_stm.bind(15,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(15);
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINETYPEXOFF,pvar) )
	{
		m_stm.bind(16,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(16);
	}

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		m_stm.bind(17,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(17);
	}

	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		m_stm.bind(18,pBuf,nLen);
	}
	else
	{
		m_stm.bindNull(18);
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		m_stm.bind(19,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(19);
	}
	
	// 完成操作
	m_stm.execDML();
	m_stm.finalize();
	query.finalize();

	return;
}


CFeature *CSQLiteAccess::GetFirstSurface()
{
	m_ftrQuery = m_db.execQuery("select * from Surface;");
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadSurface();
}


CFeature *CSQLiteAccess::GetNextSurface()
{
	m_ftrQuery.nextRow();
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadSurface();
}

CFeature *CSQLiteAccess::ReadSurface()
{
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
	int nValue;
	const char* pstr;
	double lfValue = 0;
	
	pstr = m_ftrQuery.getStringField("FTRID");
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)pstr);
	
	nValue = m_ftrQuery.getIntField("ClassType");
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("GeoClass");
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("Color");
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)nValue);

	nValue = m_ftrQuery.getIntField("SymbolizeFlag",256);
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)nValue);

	pstr = m_ftrQuery.getStringField("SymbolName");
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)pstr);

	pstr = m_ftrQuery.getStringField("FtrCode");
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)pstr);

	nValue = m_ftrQuery.getIntField("Visible",1);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)nValue);

	lfValue = m_ftrQuery.getFloatField("CellAngle");
	tab.AddValue(FIELDNAME_GEOSURFACE_CELLANGLE,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("CellScale",1);
	tab.AddValue(FIELDNAME_GEOSURFACE_CELLSCALE,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("IntvScale",1);
	tab.AddValue(FIELDNAME_GEOSURFACE_INTVSCALE,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("XStartOff");
	tab.AddValue(FIELDNAME_GEOSURFACE_XSTARTOFF,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("YStartOff");
	tab.AddValue(FIELDNAME_GEOSURFACE_YSTARTOFF,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("FtrPurpose",1);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)nValue);

	lfValue = m_ftrQuery.getFloatField("LineWidth");
	tab.AddValue(FIELDNAME_GEOSURFACE_LINEWIDTH,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("DisplayOrder",-1);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)lfValue);
		
	nValue = m_ftrQuery.getIntField("ModifyTime");
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("LayerID");
	m_nCurLayID = nValue;
	
	int nLen = 0;
	const BYTE *pBuf = m_ftrQuery.getBlobField("Shape",nLen);
	int nPt = nLen/sizeof(PT_3DEX);
	if( nPt>0 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		//arrPts.SetSize(nPt);
		//memcpy(arrPts.GetData(),pBuf,nPt*sizeof(PT_3DEX));
		DecodePt3dex(m_nFileVersion,pBuf,nLen,arrPts);
		if (!m_bFileVersionLatest)
		{
			int num = arrPts.GetSize();
			for (int i=0; i<num; i++)
			{
				arrPts[i].wid = 0;
			}
		}
		
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}

	nLen = 0;
	pBuf = m_ftrQuery.getBlobField("ObjectGroup",nLen);
	int num = nLen/sizeof(UINT);
	if( num>0 )
	{		
		var.SetAsBlob(pBuf,nLen);		
		tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	}
	
	tab.EndAddValueItem();
	
	CFeature *pFt = (CFeature*)CreateObject(tab);
	
	return pFt;
}


void CSQLiteAccess::SaveSurface(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	
	// 查找地物是否存在
	sprintf(line,"select FTRID from Surface where FTRID='%s';",(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	
	// 插入新数据
	if( query.eof() )
	{
		sprintf(line,"insert into Surface(FTRID,ClassType,GeoClass,Color,SymbolizeFlag,SymbolName,LayerID,Shape,FtrCode,Visible,CellAngle,CellScale,IntvScale,XStartOff,YStartOff,FtrPurpose,LineWidth,DisplayOrder,ObjectGroup,ModifyTime) values('%s',?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}
	// 更新数据
	else
	{
		sprintf(line,"update Surface set ClassType=?, GeoClass=?, Color=?, SymbolizeFlag=?, SymbolName=?, LayerID=?, Shape=?, FtrCode=?,Visible=?, CellAngle=?, CellScale=?, IntvScale=?, XStartOff=?, YStartOff=?, FtrPurpose=?, LineWidth=?, DisplayOrder=?, ObjectGroup=?, ModifyTime=? where FTRID='%s';",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}

	// 填充数据项
	CValueTable tab;
	const CVariantEx *pvar;
	
	tab.BeginAddValueItem();
	pFtr->WriteTo(tab);
	tab.EndAddValueItem();
	
	if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		m_stm.bind(1,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(1);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		m_stm.bind(2,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(2);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		m_stm.bind(3,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(3);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		m_stm.bind(4,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(4);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		m_stm.bind(5,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(5);
	}
	
	m_stm.bind(6,m_nCurLayID);
	
	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		
		//m_stm.bind(7,(BYTE*)arrPts.GetData(),arrPts.GetSize()*sizeof(PT_3DEX));
		BindPts(m_stm,7,arrPts);
	}
	else
	{
		m_stm.bindNull(7);
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		m_stm.bind(8,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(8);
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		m_stm.bind(9,(bool)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(9);
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_CELLANGLE,pvar) )
	{
		m_stm.bind(10,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(10);
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_CELLSCALE,pvar) )
	{
		m_stm.bind(11,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(11);
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_INTVSCALE,pvar) )
	{
		m_stm.bind(12,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(12);
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_XSTARTOFF,pvar) )
	{
		m_stm.bind(13,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(13);
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_YSTARTOFF,pvar) )
	{
		m_stm.bind(14,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(14);
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		m_stm.bind(15,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(15);
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_LINEWIDTH,pvar) )
	{
		m_stm.bind(16,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(16);
	}

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		m_stm.bind(17,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(17);
	}
	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		m_stm.bind(18,pBuf,nLen);
	}
	else
	{
		m_stm.bindNull(18);
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		m_stm.bind(19,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(19);
	}
	
	// 完成操作
	m_stm.execDML();
	m_stm.finalize();	
	query.finalize();

	return;
}


CFeature *CSQLiteAccess::GetFirstText()
{
	m_ftrQuery = m_db.execQuery("select * from Text;");
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadText();
}


CFeature *CSQLiteAccess::GetNextText()
{
	m_ftrQuery.nextRow();
	if( m_ftrQuery.eof() )
		return NULL;
	
	return ReadText();
}

CFeature *CSQLiteAccess::ReadText()
{
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
	int nValue;
	const char* pstr;
	double lfValue = 0;
	
	pstr = m_ftrQuery.getStringField("FTRID");
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)pstr);
	
	nValue = m_ftrQuery.getIntField("ClassType");
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("GeoClass");
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)nValue);
	
	nValue = m_ftrQuery.getIntField("Color");
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)nValue);

	nValue = m_ftrQuery.getIntField("SymbolizeFlag",256);
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)nValue);

	pstr = m_ftrQuery.getStringField("SymbolName");
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)pstr);
	
	nValue = m_ftrQuery.getIntField("LayerID");
	m_nCurLayID = nValue;

	pstr = m_ftrQuery.getStringField("Content");
	tab.AddValue(FIELDNAME_GEOTEXT_CONTENT,&(CVariantEx)(_variant_t)(LPCTSTR)pstr);

	pstr = m_ftrQuery.getStringField("Font");
	tab.AddValue(FIELDNAME_GEOTEXT_FONT,&(CVariantEx)(_variant_t)(LPCTSTR)pstr);

	lfValue = m_ftrQuery.getFloatField("CharWidthR");
	tab.AddValue(FIELDNAME_GEOTEXT_CHARWIDTHS,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("CharHeight");
	tab.AddValue(FIELDNAME_GEOTEXT_CHARHEIGHT,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("CharIntvR");
	tab.AddValue(FIELDNAME_GEOTEXT_CHARINTVS,&(CVariantEx)(_variant_t)lfValue);

	lfValue = m_ftrQuery.getFloatField("LineIntvR");
	tab.AddValue(FIELDNAME_GEOTEXT_LINEINTVS,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("AlignType");
	tab.AddValue(FIELDNAME_GEOTEXT_ALIGNTYPE,&(CVariantEx)(_variant_t)(long)nValue);
	
	lfValue = m_ftrQuery.getFloatField("TextAngle",0);
	tab.AddValue(FIELDNAME_GEOTEXT_TEXTANGLE,&(CVariantEx)(_variant_t)lfValue);
	
	lfValue = m_ftrQuery.getFloatField("CharAngle",0);
	tab.AddValue(FIELDNAME_GEOTEXT_CHARANGLE,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("Incline");
	tab.AddValue(FIELDNAME_GEOTEXT_INCLINE,&(CVariantEx)(_variant_t)(long)nValue);

	lfValue = m_ftrQuery.getFloatField("InclineAngle");
	tab.AddValue(FIELDNAME_GEOTEXT_INCLINEANGLE,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("PlaceType");
	tab.AddValue(FIELDNAME_GEOTEXT_PLACETYPE,&(CVariantEx)(_variant_t)(long)nValue);

	nValue = m_ftrQuery.getIntField("OtherFlag");
	tab.AddValue(FIELDNAME_GEOTEXT_OTHERFLAG,&(CVariantEx)(_variant_t)(long)nValue);
	
	pstr = m_ftrQuery.getStringField("FtrCode");
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)pstr);

	nValue = m_ftrQuery.getIntField("Visible",1);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)nValue);

	nValue = m_ftrQuery.getIntField("CoverType",0);
	tab.AddValue(FIELDNAME_GEOTEXT_COVERTYPE,&(CVariantEx)(_variant_t)(long)nValue);

	lfValue = m_ftrQuery.getFloatField("ExtendDis",0);
	tab.AddValue(FIELDNAME_GEOTEXT_EXTENDDIS,&(CVariantEx)(_variant_t)lfValue);

	nValue = m_ftrQuery.getIntField("FtrPurpose",1);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)nValue);

	lfValue = m_ftrQuery.getFloatField("DisplayOrder",-1);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)lfValue);
		
	nValue = m_ftrQuery.getIntField("ModifyTime");
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)nValue);

	int nLen = 0;
	const BYTE *pBuf = m_ftrQuery.getBlobField("Shape",nLen);
	int nPt = nLen/sizeof(PT_3DEX);
	if( nPt>0 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		//arrPts.SetSize(nPt);
		//memcpy(arrPts.GetData(),pBuf,nPt*sizeof(PT_3DEX));
		DecodePt3dex(m_nFileVersion,pBuf,nLen,arrPts);
		
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}
	
	nLen = 0;
	pBuf = m_ftrQuery.getBlobField("ObjectGroup",nLen);
	int num = nLen/sizeof(UINT);
	if( num>0 )
	{		
		var.SetAsBlob(pBuf,nLen);		
		tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	}

	tab.EndAddValueItem();
	
	CFeature *pFt = (CFeature*)CreateObject(tab);
	
	return pFt;
}


void CSQLiteAccess::SaveText(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	
	// 查找地物是否存在
	sprintf(line,"select FTRID from Text where FTRID='%s';",(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	
	// 插入新数据
	if( query.eof() )
	{
		sprintf(line,"insert into Text(FTRID,ClassType,GeoClass,Color,SymbolizeFlag,SymbolName,LayerID,Shape,Content,Font,CharWidthR,CharHeight,CharIntvR,LineIntvR,AlignType,TextAngle,CharAngle,Incline,InclineAngle,PlaceType,OtherFlag,FtrCode,Visible,CoverType,ExtendDis,FtrPurpose,DisplayOrder,ObjectGroup,ModifyTime) values('%s',?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}
	// 更新数据
	else
	{
		sprintf(line,"update Text set ClassType=?, GeoClass=?, Color=?, SymbolizeFlag=?, SymbolName=?, LayerID=?, Shape=?, Content=?, Font=?, CharWidthR=?, CharHeight=?, CharIntvR=?, LineIntvR=?, AlignType=?, TextAngle=?, CharAngle=?, Incline=?, InclineAngle=?, PlaceType=?, OtherFlag=?, FtrCode=?, Visible=?, CoverType=?, ExtendDis=?, FtrPurpose=?, DisplayOrder=?, ObjectGroup=?, ModifyTime=? where FTRID='%s';",
			(LPCSTR)pFtr->GetID().ToString());
		
		m_stm = m_db.compileStatement(line);
	}
	
	// 填充数据项
	CValueTable tab;
	const CVariantEx *pvar;
	
	tab.BeginAddValueItem();
	pFtr->WriteTo(tab);
	tab.EndAddValueItem();
	
	if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		m_stm.bind(1,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(1);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		m_stm.bind(2,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(2);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		m_stm.bind(3,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(3);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		m_stm.bind(4,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(4);
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		m_stm.bind(5,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(5);
	}
	
	m_stm.bind(6,m_nCurLayID);
	
	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		
		//m_stm.bind(7,(BYTE*)arrPts.GetData(),arrPts.GetSize()*sizeof(PT_3DEX));
		BindPts(m_stm,7,arrPts);
	}
	else
	{
		m_stm.bindNull(7);
	}

	int idx = 8;
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CONTENT,pvar) )
	{
		m_stm.bind(idx++,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_FONT,pvar) )
	{
		m_stm.bind(idx++,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARWIDTHS,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARHEIGHT,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}	
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARINTVS,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_LINEINTVS,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_ALIGNTYPE,pvar) )
	{
		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_TEXTANGLE,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARANGLE,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINE,pvar) )
	{
		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINEANGLE,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_PLACETYPE,pvar) )
	{
		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_OTHERFLAG,pvar) )
	{
		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		m_stm.bind(idx++,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		m_stm.bind(idx++,(bool)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_COVERTYPE,pvar) )
	{
		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_EXTENDDIS,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		m_stm.bind(idx++,pBuf,nLen);
	}
	else
	{
		m_stm.bindNull(idx++);
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
		m_stm.bindNull(idx++);
	}

	// 完成操作
	m_stm.execDML();
	m_stm.finalize();
	query.finalize();

	return;
}

int CSQLiteAccess::GetFtrLayerCount()
{
	CppSQLite3Query query = m_db.execQuery("select count(*) from DLGVectorLayer;");
	return query.getIntField(0);
}

int CSQLiteAccess::GetFeatureCount()
{
	CppSQLite3Query query;
	int sum = 0;
	query = m_db.execQuery("select count(*) from Point;");
	sum += query.getIntField(0);
	query = m_db.execQuery("select count(*) from Line;");
	sum += query.getIntField(0);
	query = m_db.execQuery("select count(*) from Surface;");
	sum += query.getIntField(0);
	query = m_db.execQuery("select count(*) from Text;");
	sum += query.getIntField(0);

	return sum;
}

//读取地物
CFeature *CSQLiteAccess::GetFirstFeature(long &idx, CValueTable *exAttriTab)
{
	idx = 0;
	CFeature *pFtr = GetFirstPoint();
	if( pFtr!=NULL )return pFtr;
	idx = 1;
	
	pFtr = GetFirstLine();
	if( pFtr!=NULL )return pFtr;
	idx = 2;

	pFtr = GetFirstSurface();
	if( pFtr!=NULL )return pFtr;
	idx = 3;

	pFtr = GetFirstText();
	if( pFtr!=NULL )return pFtr;

	idx = -1;
	return NULL;
}


CFeature *CSQLiteAccess::GetNextFeature(long &idx, CValueTable *exAttriTab)
{
	CFeature *pFtr = NULL;

	if( idx<0 )return NULL;

	//上个是点数据
	if( idx==0 )
	{
		pFtr = GetNextPoint();
		if( pFtr!=NULL )return pFtr;
	}

	//上个是线数据
	if( idx==1 )
	{
		pFtr = GetNextLine();
		if( pFtr!=NULL )return pFtr;
	}
	else if( idx<1 )
	{
		idx = 1;
		pFtr = GetFirstLine();
		if( pFtr!=NULL )return pFtr;
	}

	//上个是面数据
	if( idx==2 )
	{
		pFtr = GetNextSurface();
		if( pFtr!=NULL )return pFtr;
	}
	else if( idx<2 )
	{
		idx = 2;
		pFtr = GetFirstSurface();
		if( pFtr!=NULL )return pFtr;
	}

	//上个是文本数据
	if( idx==3 )
	{
		pFtr = GetNextText();
		if( pFtr!=NULL )return pFtr;
	}
	else if( idx<3 )
	{
		idx = 3;
		pFtr = GetFirstText();
		if( pFtr!=NULL )return pFtr;
	}
	
	return NULL;
}


int CSQLiteAccess::GetCurFtrLayID()
{
	return m_nCurLayID;
}


int CSQLiteAccess::GetCurFtrGrpID()
{
	return 0;
}

BOOL CSQLiteAccess::ReadXAttribute(CFeature *pFtr, CValueTable& tab)
{
	if(!pFtr || !pFtr->GetGeometry())
		return FALSE;
	char line[1024] = {0};
	CppSQLite3Query query;
	//首先获取地物所在的层ID
	CString strTable;
	int nGeoCls = pFtr->GetGeometry()->GetClassType();
	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT || nGeoCls == CLS_GEOSURFACEPOINT)
		strTable = "Point";
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT )
		strTable = "Line";
	else if( nGeoCls==CLS_GEOSURFACE || nGeoCls==CLS_GEOMULTISURFACE)
		strTable = "Surface";
	else if( nGeoCls==CLS_GEOTEXT )
		strTable = "Text";	
	if (strTable.IsEmpty())
	{
		return FALSE;
	}
	sprintf(line,"select LayerID from %s where FTRID='%s';",(LPCSTR)strTable,pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField("LayerID");

	//转化为层名来对应
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);
	
	//通过层ID获取扩展属性表的名称
	strTable.Format("AttrTable_%s",(LPCSTR)ename);
	
	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
		return FALSE;
	query.finalize();
	//在扩展属性表中查找地物的对应项是否存在
	sprintf(line,"select * from %s where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	
	//
	if( query.eof() )
	{
		return FALSE;
	}
	// 读取扩展数据	
	CVariantEx var;
	//tab.BeginAddValueItem();
	
	long nValue;
	const char* pstr;
	double lfValue = 0;
	int nVauleType;
	char field[256];	

	for(int i = 2;i<query.numFields();i++)
	{
		memset(field,0x00,sizeof(field));

		nVauleType = query.fieldDataType(i);
		const char* pfield = query.fieldName(i);
		if (pfield != NULL)
		{
			strcpy(field, pfield);
			field[strlen(field)-1] = 0x00;
		}
		
		switch(nVauleType)
		{
		case SQLITE_INTEGER:
			nValue = query.getIntField(i);
			tab.AddValue(field,&(CVariantEx)(_variant_t)nValue);
			break;
		case SQLITE_FLOAT:
			lfValue = query.getFloatField(i);
			tab.AddValue(field,&(CVariantEx)(_variant_t)lfValue);
			break;
		case SQLITE_TEXT:
			pstr = query.getStringField(i);
			tab.AddValue(field,&(CVariantEx)(_variant_t)pstr);
		    break;	
		default:
		    break;
		}		
	}
	query.finalize();

	return TRUE;

}


//更新数据源信息
void CSQLiteAccess::WriteDataSourceInfo(CDataSourceEx *pDS)
{
	CppSQLite3Statement stm = m_db.compileStatement("update WorkSpace set scale=?, bound=?, zmin=?, zmax=?;");
	
	CValueTable tab;
	const CVariantEx *pvar;

	tab.BeginAddValueItem();
	pDS->WriteTo(tab);
	tab.EndAddValueItem();
	
	if( tab.GetValue(0,FIELDNAME_SCALE,pvar) )
	{
		stm.bind(1,(int)(long)(_variant_t)*pvar);
	}

	if( tab.GetValue(0,FIELDNAME_BOUND,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		if( arrPts.GetSize()==4 )
		{
			stm.bind(2,(const BYTE*)arrPts.GetData(),4*sizeof(PT_3DEX));
		}
	}
	if( tab.GetValue(0,FIELDNAME_ZMIN,pvar) )
	{
		stm.bind(3,(double)(_variant_t)*pvar);
	}
	if( tab.GetValue(0,FIELDNAME_ZMAX,pvar) )
	{
		stm.bind(4,(double)(_variant_t)*pvar);
	}

	stm.execDML();
	stm.finalize();
}



//更新层
BOOL CSQLiteAccess::SaveFtrLayer(CFtrLayer *pLayer)
{
	if (!pLayer)  return FALSE;
	char line[1024] = {0};

	// 查询层是否存在
	sprintf(line,"select ID from DLGVectorLayer where ID=%d;",pLayer->GetID());
	
	CppSQLite3Query query = m_db.execQuery(line);

	// 不存在，需要新建
	if( query.eof() )
	{
		sprintf(line,"insert into DLGVectorLayer ( ID, Name, MapName, GroupName, Color, Visible, Locked, Inherent, DisplayOrder, Symbolized) values ( %d, ?, ?, ?, ?, ?, ?, ?, ?, ? ) ;",
			pLayer->GetID());
	}
	else
	{
		sprintf(line,"update DLGVectorLayer set Name=?, MapName=?, GroupName=?, Color=?, Visible=?, Locked=?, Inherent=?, DisplayOrder=?, Symbolized=? where ID=%d;",pLayer->GetID());
	}

	CppSQLite3Statement stm = m_db.compileStatement(line);
	
	CValueTable tab;
	const CVariantEx *pvar;

	tab.BeginAddValueItem();
	pLayer->WriteTo(tab);
	tab.EndAddValueItem();
	
	if( tab.GetValue(0,FIELDNAME_LAYERNAME,pvar) )
	{
		stm.bind(1,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	
// 	if( tab.GetValue(0,FIELDNAME_LAYMAPNAME,pvar) )
// 	{
// 		stm.bind(2,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
// 	}

	if( tab.GetValue(0,FIELDNAME_LAYGROUPNAME,pvar) )
	{
		stm.bind(3,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}

	if( tab.GetValue(0,FIELDNAME_LAYCOLOR,pvar) )
	{
		stm.bind(4,(long)(_variant_t)*pvar);
	}

	if( tab.GetValue(0,FIELDNAME_LAYVISIBLE,pvar) )
	{
		stm.bind(5,(long)(_variant_t)*pvar);
	}

	if( tab.GetValue(0,FIELDNAME_LAYLOCKED,pvar) )
	{
		stm.bind(6,(long)(_variant_t)*pvar);
	}

	if( tab.GetValue(0,FIELDNAME_LAYINHERENT,pvar) )
	{
		stm.bind(7,(long)(_variant_t)*pvar);
	}

	if( tab.GetValue(0,FIELDNAME_LAYERDISPLAYORDER,pvar) )
	{
		stm.bind(8,(long)(_variant_t)*pvar);
	}

	if( tab.GetValue(0,FIELDNAME_LAYERSYMBOLIZED,pvar) )
	{
		stm.bind(9,(long)(_variant_t)*pvar);
	}
	
	stm.execDML();
	stm.finalize();

	TryCommit();

	return TRUE;
}


BOOL CSQLiteAccess::DelFtrLayer(CFtrLayer *pLayer)
{
	char line[1024] = {0};
	sprintf(line,"select * from DLGVectorLayer where ID=%d;",pLayer->GetID());

	CppSQLite3Query query = m_db.execQuery(line);
	if( query.eof() )
		return FALSE;

	sprintf(line,"delete from DLGVectorLayer where ID=%d;",pLayer->GetID());
	m_db.execDML(line);

	return TRUE;
}

BOOL CSQLiteAccess::SaveObjectGroup(ObjectGroup *ftr)
{
	char line[1024] = {0};
	
	// 查询层是否存在
	sprintf(line,"select ID from ObjectGroup where ID=%d;",ftr->id);
	
	CppSQLite3Query query = m_db.execQuery(line);
	
	// 不存在，需要新建
	if( query.eof() )
	{
		sprintf(line,"insert into ObjectGroup( ID, Name, Selectable) values ( %d, ?, ?) ;",
			ftr->id);
	}
	else
	{
		sprintf(line,"update ObjectGroup set Name=?, Selectable=? where ID=%d;",ftr->id);
	}
	
	CppSQLite3Statement stm = m_db.compileStatement(line);
	
	stm.bind(1,ftr->name);
	stm.bind(2,(long)(_variant_t)ftr->select);
	
	stm.execDML();
	stm.finalize();
	
	return TRUE;
}

BOOL CSQLiteAccess::DelObjectGroup(ObjectGroup *ftr)
{
	char line[1024] = {0};
	sprintf(line,"select * from ObjectGroup where ID=%d;",ftr->id);
	
	CppSQLite3Query query = m_db.execQuery(line);
	if( query.eof() )
		return FALSE;
	
	sprintf(line,"delete from ObjectGroup where ID=%d;",ftr->id);
	m_db.execDML(line);
	
	return TRUE;
}

BOOL CSQLiteAccess::SaveFtrLayerGroup(FtrLayerGroup *pGroup)
{
	char line[1024] = {0};
	
	// 查询层是否存在
	sprintf(line,"select ID from FtrLayerGroup where ID=%d;",pGroup->id);
	
	CppSQLite3Query query = m_db.execQuery(line);
	
	// 不存在，需要新建
	if( query.eof() )
	{
		sprintf(line,"insert into FtrLayerGroup(ID, Name, Color, Visible, Symbolized) values ( %d, ?, ?, ?, ?) ;",
			pGroup->id);
	}
	else
	{
		sprintf(line,"update FtrLayerGroup set Name=?, Color=?, Visible=?, Symbolized=? where ID=%d;",pGroup->id);
	}
	
	CppSQLite3Statement stm = m_db.compileStatement(line);
	
	stm.bind(1,(LPCSTR)pGroup->Name);
	stm.bind(2,pGroup->Color);
	stm.bind(3,(long)pGroup->Visible);
	stm.bind(4,(long)pGroup->Symbolized);
	
	stm.execDML();
	stm.finalize();
	
	return TRUE;
}

BOOL CSQLiteAccess::DelFtrLayerGroup(FtrLayerGroup *pGroup)
{
	char line[1024] = {0};
	sprintf(line,"select * from FtrLayerGroup where ID=%d;",pGroup->id);
	
	CppSQLite3Query query = m_db.execQuery(line);
	if( query.eof() )
		return FALSE;
	
	sprintf(line,"delete from FtrLayerGroup where ID=%d;",pGroup->id);
	m_db.execDML(line);
	
	return TRUE;
}

//更新地物
BOOL CSQLiteAccess::SetCurFtrLayID(int id)
{
	m_nCurLayID = id;
	return TRUE;
}


BOOL CSQLiteAccess::SetCurFtrGrpID(int id)
{
	return TRUE;
}




BOOL CSQLiteAccess::SaveFeature(CFeature *pFt, CValueTable *exAttriTab)
{
#ifdef TRIAL_VERSION
	return TRUE;
#else

	int nGeoCls = pFt->GetGeometry()->GetClassType();
	CGeometry *pGeo = pFt->GetGeometry();

	//有可能插件中会创建派生类
	if( nGeoCls==CLS_GEOPOINT || nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		SavePoint(pFt);
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT ||
			pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		SaveLine(pFt);
	else if( nGeoCls==CLS_GEOSURFACE ||nGeoCls==CLS_GEOMULTISURFACE ||  pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		SaveSurface(pFt);
	else if( nGeoCls==CLS_GEOTEXT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		SaveText(pFt);
	else
		return FALSE;

	TryCommit();

	return TRUE;

#endif
}

BOOL CSQLiteAccess::RestoreFeature(CFeature *pFt)
{
   return SaveFeature(pFt,NULL);
}

BOOL CSQLiteAccess::DelAllFeature()
{
	// 查找地物所在的数据表名
// 	char line[1024] = {0};
// 	CppSQLite3Query query;
// 	
// 	CString strTable;
// 	int nGeoCls = pFtr->GetGeometry()->GetClassType();
// 	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT)
// 		strTable = "Point";
// 	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT )
// 		strTable = "Line";
// 	else if( nGeoCls==CLS_GEOSURFACE|| nGeoCls==CLS_GEOMULTISURFACE)
// 		strTable = "Surface";
// 	else if( nGeoCls==CLS_GEOTEXT )
// 		strTable = "Text";
// 	
// 	if(strTable.IsEmpty()) return FALSE;
// 	// 查找地物是否存在
// //	sprintf(line,"delete from %s;","Point");
	BatchUpdateBegin();
	//首先清理地物表
	m_db.execDML("delete from 'Point'");
	m_db.execDML("delete from 'Line'");
	m_db.execDML("delete from 'Surface'");
	m_db.execDML("delete from 'Text'");
	//清理扩展属性表
	CppSQLite3Query  ftrQuery;
	ftrQuery = m_db.execQuery("select name from sqlite_master where type='table';");
	if (!ftrQuery.eof())
	{	
		const char* strTable;
		char line[1024];
		do
		{
			strTable = ftrQuery.getStringField("name");
			if(strncmp(strTable,"AttrTable_",10)==0)
			{
				sprintf(line,"delete from %s;",strTable);
				m_db.execDML(line);
			}
			ftrQuery.nextRow();
		}
		while(!ftrQuery.eof());
	}
	ftrQuery.finalize();
	BatchUpdateEnd();
	Compress();//释放空间
	
	return TRUE;
}

BOOL CSQLiteAccess::DelFeature(CFeature *pFtr)
{

#ifdef TRIAL_VERSION
	return TRUE;
#else

	// 查找地物所在的数据表名
	char line[1024] = {0};
	CppSQLite3Query query;
	
	CString strTable;
	int nGeoCls = pFtr->GetGeometry()->GetClassType();
	CGeometry *pGeo = pFtr->GetGeometry();

	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		strTable = "Point";
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT || 
		pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		strTable = "Line";
	else if( nGeoCls==CLS_GEOSURFACE|| nGeoCls==CLS_GEOMULTISURFACE || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		strTable = "Surface";
	else if( nGeoCls==CLS_GEOTEXT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
		strTable = "Text";
	
	if(strTable.IsEmpty()) return FALSE;
	// 查找地物是否存在
	sprintf(line,"delete from %s where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	int nRet = m_db.execDML(line);

	return (nRet>0);

#endif
}

BOOL CSQLiteAccess::DelXAttributes(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	//首先获取地物所在的层ID
	CString strTable;
	int nGeoCls = pFtr->GetGeometry()->GetClassType();
	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT || nGeoCls == CLS_GEOSURFACEPOINT)
		strTable = "Point";
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT )
		strTable = "Line";
	else if( nGeoCls==CLS_GEOSURFACE || nGeoCls==CLS_GEOMULTISURFACE)
		strTable = "Surface";
	else if( nGeoCls==CLS_GEOTEXT )
		strTable = "Text";	
	if(strTable.IsEmpty()) return FALSE;
	sprintf(line,"select LayerID from %s where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField(0);

	//转化为层名来对应
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);
	
	//通过层ID获取扩展属性表的名称
	strTable.Format("AttrTable_%s",(LPCSTR)ename);
	
	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
		return FALSE;
	
	//在扩展属性表中更新地物的对应项的DEL标记
	sprintf(line,"update %s set DELETED='%d' where FTRID='%s';",
			(LPCSTR)strTable,1,(LPCSTR)pFtr->GetID().ToString());
	
	m_db.execDML(line);	
	query.finalize();
	return TRUE;
}

BOOL CSQLiteAccess::RestoreXAttributes(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	//首先获取地物所在的层ID
	CString strTable;
	int nGeoCls = pFtr->GetGeometry()->GetClassType();
	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT  || nGeoCls == CLS_GEOSURFACEPOINT)
		strTable = "Point";
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT )
		strTable = "Line";
	else if( nGeoCls==CLS_GEOSURFACE || nGeoCls==CLS_GEOMULTISURFACE)
		strTable = "Surface";
	else if( nGeoCls==CLS_GEOTEXT )
		strTable = "Text";	
	
	if(strTable.IsEmpty()) return FALSE;
	sprintf(line,"select LayerID from %s where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField(0);

	//转化为层名来对应
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);
	
	//通过层ID获取扩展属性表的名称
	strTable.Format("AttrTable_%s",(LPCSTR)ename);
	
	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
		return FALSE;
	
	//在扩展属性表中更新地物的对应项的DEL标记
	sprintf(line,"update %s set DELETED='%d' where FTRID='%s';",
		(LPCSTR)strTable,0,(LPCSTR)pFtr->GetID().ToString());
	
	m_db.execDML(line);	
	query.finalize();
	return TRUE;
}

static BOOL  SqliteVariantToText(const CVariantEx& v, CString &text, int fieldType)
{
	_variant_t var = v.m_variant;
	switch(fieldType)
	{
	case SQLITE_TEXT:
		break;
	case SQLITE_INTEGER:
		if (var.vt==VT_R4||var.vt==VT_R8)
		{
			var.ChangeType(VT_I4);
		}
		else if( var.vt==VT_BSTR )
			var = (long)atol((LPCTSTR)(_bstr_t)var);
		break;
	case SQLITE_FLOAT:
		if (var.vt==VT_UI2||var.vt==VT_UI4||var.vt==VT_UINT||var.vt==VT_I2||var.vt==VT_I4||var.vt==VT_INT)
		{
			var.ChangeType(VT_R8);
		}
		else if( var.vt==VT_BSTR )
			var = (double)atof((LPCTSTR)(_bstr_t)var);  
		break;
	default:
	    break;
	}
	switch(var.vt) 
	{
	case VT_UI1:
		text.Format("%i",var.bVal);
		break;
	case VT_UI2:
		text.Format("%i",var.uiVal);
		break;
	case VT_UI4:
		text.Format("%i",var.ulVal);
		break;
	case VT_UINT:
		text.Format("%i",var.uintVal);
		break;
	case VT_I1:
		text.Format("%c",var.cVal);
		break;
	case VT_I2:
		text.Format("%i",var.iVal);
		break;	
	case VT_I4:
		text.Format("%i",(long)var);
		break;
	case VT_INT:
		text.Format("%i",(long)var);
		break;
	case VT_BOOL:
		text.Format("%i",(bool)var);
		break;
	case VT_R4:
		text.Format("%f",(float)var);
		break;
	case VT_R8:
		text.Format("%f",(double)var);
		break;
	case VT_BSTR:
		text.Format("%s",(LPCTSTR)(_bstr_t)var);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

extern void VariantToText(const CVariantEx& v, CString &text);

BOOL CSQLiteAccess::SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx)
{
//	if(tab.GetFieldCount()<=2) return FALSE;

	char line[1024] = {0};
	CString str,str0,str1;
	CppSQLite3Query query;
	//首先获取地物所在的层ID
	CString strTable;
	int nGeoCls = pFtr->GetGeometry()->GetClassType();
	if (nGeoCls==CLS_GEOPOINT || nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT || nGeoCls == CLS_GEOSURFACEPOINT)
		strTable = "Point";
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT )
		strTable = "Line";
	else if( nGeoCls==CLS_GEOSURFACE || nGeoCls==CLS_GEOMULTISURFACE)
		strTable = "Surface";
	else if( nGeoCls==CLS_GEOTEXT )
		strTable = "Text";	
	
	if (strTable.IsEmpty())
	{
		return FALSE;
	}

	sprintf(line,"select LayerID from %s where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField("LayerID");
	query.finalize();

	//转化为层名来对应
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);

	//通过层ID获取扩展属性表的名称
	strTable.Format("AttrTable_%s",(LPCSTR)ename);

	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
	{
		query.finalize();
		return FALSE;
	}

	//在扩展属性表中查找地物的对应项是否存在
	sprintf(line,"select * from %s where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	
	const CVariantEx *var;
	// 插入新数据
	if( query.eof() )
	{
		str.Format("insert into %s(FTRID,DELETED",(LPCSTR)strTable);
		str0.Format("values('%s',0",(LPCSTR)pFtr->GetID().ToString());
		for(int i = 2;i<query.numFields();i++)
		{	
			str+=",";
			str+=query.fieldName(i);

			char field[256];
			memset(field,0x00,sizeof(field));
			const char* pfield = query.fieldName(i);
			if (pfield != NULL)
			{
				strcpy(field, pfield);
				field[strlen(field)-1] = 0x00;
			}

			if (tab.GetValue(idx,(LPCTSTR)field,var))
			{
				int fieldType = var->GetType();
				if( VariantToTextA(*var,str1) )
				{
					str1 = ReplaceQuotationMark(str1);
					str0+=",";				
					
					if(fieldType==VT_BSTR)	
						str0+="'";
					str0+=str1;
					if(fieldType==VT_BSTR)		
						str0+="'";						
				}
				else
				{
					str0+=",NULL";
				}


// 				int fieldType = query.fieldDataType(i);
// 				SqliteVariantToText(*var,str1,fieldType);
// 				str1 = ReplaceQuotationMark(str1);
// 				str0+=",";
// 
// 				if(fieldType==SQLITE_TEXT)	
// 					str0+="'";
// 				str0+=str1;
// 				if(fieldType==SQLITE_TEXT)		
// 					str0+="'";			
			}
			else
			{
				str0+=",NULL";
			}
		}
		str+=") ";
		str0+=");";
		str+=str0;
		m_db.execDML(LPCSTR(str));
	}
	// 更新数据
	else
	{
		str.Format("update %s set DELETED=0",strTable);		
		for(int i = 2;i<query.numFields();i++)
		{	
			char field[256];
			memset(field,0x00,sizeof(field));
			const char* pfield = query.fieldName(i);
			if (pfield != NULL)
			{
				strcpy(field, pfield);
				field[strlen(field)-1] = 0x00;
			}

			if (tab.GetValue(idx,field,var))
			{		
				int fieldType = query.fieldDataType(i);
				if( SqliteVariantToText(*var,str1,fieldType) )
				{
					str+=",";
					str+=query.fieldName(i);
					str+="=";	

					str1 = ReplaceQuotationMark(str1);
					if (fieldType == SQLITE_TEXT || fieldType == SQLITE_NULL)
						str+="'";
					str+=str1;
					if (fieldType == SQLITE_TEXT || fieldType == SQLITE_NULL)
						str+="'";
				}
			}				
		}
		str+=" where FTRID='";
		str+=pFtr->GetID().ToString();
		str+="';";	
		m_db.execDML(LPCSTR(str));		
	}
	query.finalize();
    return TRUE;
}

void CSQLiteAccess::BatchUpdateBegin()
{
	//临时调高cache尺寸，在大批量修改地物时，速度更快
	m_db.execDML("PRAGMA cache_size=10000;");
	m_db.execDML("begin transaction;");

	m_nWriteCount = 0;
	m_bBeginTraction = TRUE;
}


void CSQLiteAccess::BatchUpdateEnd()
{
CDebugTime time1;

	m_layerQuery.finalize();
	m_ftrQuery.finalize();
	m_stm.finalize();

	m_db.execDML("commit transaction;");
//	m_db.execDML("PRAGMA cache_size=1000;");

	m_bBeginTraction = FALSE;

time1.PrintfTime("commit");
}


void CSQLiteAccess::TryCommit()
{
	//修改的地物数量太多，达到合适的数量就直接写入，速度更快
	if( m_bBeginTraction )
	{
		m_nWriteCount++;
		if( 0 && m_nWriteCount>=3000 )
		{
			m_db.execDML("commit transaction;");
			m_db.execDML("begin transaction;");
			m_nWriteCount = 0;
		}
	}
}



void CSQLiteAccess::CreateDefaultLayer()
{
	//再创建几个缺省的层
	CppSQLite3Query ftrQuery;
	CString str;
	str.Format("select * from DLGVectorLayer where Name = '%s';",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMEP));
	ftrQuery = m_db.execQuery(LPCSTR(str));
	if (ftrQuery.eof())
	{
		str.Format("insert into  DLGVectorLayer values(1,'%s','','',16777215,1,0,1,-1,1);",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMEP));
		m_db.execDML(LPCSTR(str));
	}
	str.Format("select * from DLGVectorLayer where Name = '%s';",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMEL));
	ftrQuery = m_db.execQuery(LPCSTR(str));
	if (ftrQuery.eof())
	{
		str.Format("insert into  DLGVectorLayer values(2,'%s','','',16777215,1,0,1,-1,1);",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMEL));
		m_db.execDML(LPCSTR(str));
	}
	str.Format("select * from DLGVectorLayer where Name = '%s';",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMES));
	ftrQuery = m_db.execQuery(LPCSTR(str));
	if (ftrQuery.eof())
	{
		str.Format("insert into  DLGVectorLayer values(3,'%s','','',16777215,1,0,1,-1,1);",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMES));
		m_db.execDML(LPCSTR(str));
	}
	str.Format("select * from DLGVectorLayer where Name = '%s';",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMET));
	ftrQuery = m_db.execQuery(LPCSTR(str));
	if (ftrQuery.eof())
	{
		str.Format("insert into  DLGVectorLayer values(4,'%s','','',16777215,1,0,1,-1,1);",(LPCSTR)StrFromResID(IDS_DEFLAYER_NAMET));
		m_db.execDML(LPCSTR(str));
	}
}

BOOL CSQLiteAccess::Compress()
{
	m_db.execDML("VACUUM");
	return TRUE;
}

BOOL CSQLiteAccess::Repair()
{
	CopySqliteFile((LPCTSTR)m_strReadName,m_strReadName+_T(".repair"));
	return TRUE;
}

void CSQLiteAccess::CreateDataSettingsTable()
{
	m_db.execDML("create table DataSettings(ItemField VARCHAR PRIMARY KEY, ItemName VARCHAR, ItemValue VARCHAR, ValueType char[16]);");

	char line[1024] = {0};
	sprintf(line,"insert into DataSettings values('FileVersion','FileVersion','%s','string');",SQLITE_FDB_EDITION);
	
	m_db.execDML(line);
}

void CSQLiteAccess::CreateFilePreViewImage()
{
	m_db.execDML("create table PreviewImage(ImageId VARCHAR, Image BLOB);"); 
}


void CSQLiteAccess::CreateCellDefTable()
{
	//名称，类型，数据内容
	m_db.execDML("create table CellDef(Name char[32] PRIMARY KEY, CellType INT, Data BLOB);"); 
}


void CSQLiteAccess::SaveFilePreViewImage(LPCTSTR name, BYTE *img, int len)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	
	// 查找地物是否存在
	sprintf(line,"select ImageId from PreviewImage where ImageId='%s';",name);
	query = m_db.execQuery(line);
	
	// 插入新数据
	if( query.eof() )
	{
		sprintf(line,"insert into PreviewImage(ImageId,Image) values('%s',?);",
			name);
		
		m_stm = m_db.compileStatement(line);
	}
	// 更新数据
	else
	{
		sprintf(line,"update PreviewImage set Image=? where ImageId='%s';",
			name);
		
		m_stm = m_db.compileStatement(line);
	}
	
	m_stm.bind(1,img,len);
	
	// 完成操作
	m_stm.execDML();
	m_stm.finalize();
	query.finalize();
	return;
}


BOOL CSQLiteAccess::ReadFilePreViewImage(LPCTSTR name, BYTE *&img, int &len)
{
	char line[1024] = {0};
	sprintf(line,"select Image from PreviewImage where ImageId='%s';",name);

	CppSQLite3Query query = m_db.execQuery(line);
	if( query.eof() )
		return FALSE;

	CVariantEx var;
		
	const BYTE *pBuf = query.getBlobField("Image",len);
	if( len>0 )
	{
		img = new BYTE[len];
		memcpy(img,pBuf,len);
	}
	
	return TRUE;
}

BOOL CSQLiteAccess::DelFilePreViewImage(LPCTSTR name)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	
	sprintf(line,"delete from PreviewImage where ImageId='%s';",name);
	int nRet = m_db.execDML(line);
	
	return (nRet>0);
}

void CSQLiteAccess::SaveDataSettings(LPCTSTR field, LPCTSTR name, LPCTSTR value, LPCTSTR type)
{
	if (field == NULL || _tcslen(field)<=0 || name == NULL || value == NULL || type == NULL) return;

	char line[1024] = {0};
	CppSQLite3Query query;
	
	// 查找地物是否存在
	sprintf(line,"select * from DataSettings where ItemField='%s';",field);
	query = m_db.execQuery(line);
	
	// 插入新数据
	if( query.eof() )
	{
		sprintf(line,"insert into DataSettings(ItemField,ItemName,ItemValue,ValueType) values('%s',?,?,?);",
			field);
		
		m_stm = m_db.compileStatement(line);
	}
	// 更新数据
	else
	{
		sprintf(line,"update DataSettings set ItemName=?, ItemValue=?, ValueType=? where ItemField='%s';",
			field);
		
		m_stm = m_db.compileStatement(line);
	}

	m_stm.bind(1,name);
	m_stm.bind(2,value);
	m_stm.bind(3,type);
	
	// 完成操作
	m_stm.execDML();
	m_stm.finalize();
	query.finalize();
	return;
}


BOOL CSQLiteAccess::ReadDataSettings(LPCTSTR field, CString &name, CString& value, CString &type)
{
	char line[1024] = {0};
	sprintf(line,"select * from DataSettings where ItemField='%s';",field);
	
	CppSQLite3Query query = m_db.execQuery(line);
	if( query.eof() )
		return FALSE;
	
	CVariantEx var;
	
	const char* pstr = query.getStringField("ItemName");
	if( pstr )
	{
		name = pstr;
	}

	pstr = query.getStringField("ItemValue");
	if( pstr )
	{
		value = pstr;
	}

	pstr = query.getStringField("ValueType");
	if( pstr )
	{
		type = pstr;
	}
	
	return TRUE;
}

BOOL CSQLiteAccess::DelDataSettings(LPCTSTR field)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	
	sprintf(line,"delete from DataSettings where ItemField='%s';",field);
	int nRet = m_db.execDML(line);
	
	return (nRet>0);
}

BOOL CSQLiteAccess::IsFileVersionLatest()
{
	if (m_bHaveCheckedVersion)
		return m_bFileVersionLatest;

	CString name,value,type;
	ReadDataSettings(_T("FileVersion"),name,value,type);

	if (value.CompareNoCase(SQLITE_FDB_EDITION) == 0)
	{
		m_bFileVersionLatest = TRUE;
	}
	else
	{
		m_bFileVersionLatest = FALSE;
	}	

	m_bHaveCheckedVersion = TRUE;
	return m_bFileVersionLatest;
}

CString CSQLiteAccess::GetFileVersion()
{
	CString name,value,type;
	ReadDataSettings(_T("FileVersion"),name,value,type);	
	return value;
}


BOOL CSQLiteAccess::ReadCellDef(CellDef& def)
{
	int nValue;
	const char* pstr;
	bool bValue;

	if( !def.m_pgr )
		return FALSE;
		
	pstr = m_ftrQuery.getStringField("Name");
	if( pstr )
	{
		strncpy(def.m_name,pstr,31); 
		def.m_name[32] = '\0';
	}

	int nLen = 0;
	const BYTE *pData = m_ftrQuery.getBlobField("Data",nLen);

	return def.m_pgr->ReadFromBlob(pData,nLen);
}

BOOL CSQLiteAccess::GetFirstCellDef(CellDef& def)
{
	m_ftrQuery = m_db.execQuery("select * from CellDef;");
	if( m_ftrQuery.eof() )
		return FALSE;

	return ReadCellDef(def);
}


BOOL CSQLiteAccess::GetNextCellDef(CellDef& def)
{
	m_ftrQuery.nextRow();
	if( m_ftrQuery.eof() )
		return FALSE;

	return ReadCellDef(def);
}

BOOL CSQLiteAccess::SaveCellDef(CellDef& def)
{
	char line[1024] = {0};

	if( !def.m_pgr )
		return FALSE;

	CString name2 = ReplaceQuotationMark(def.m_name);
	
	// 查询层是否存在
	sprintf(line,"select Name from CellDef where Name='%s';",(LPCTSTR)name2);
	
	CppSQLite3Query query = m_db.execQuery(line);
	
	// 不存在，需要新建
	if( query.eof() )
	{
		sprintf(line,"insert into CellDef(Name,CellType,Data) values('%s',0,?);",
			(LPCTSTR)name2);
	}
	else
	{
		sprintf(line,"update CellDef set Data=? where Name='%s';",(LPCTSTR)name2);
	}
	
	CppSQLite3Statement stm = m_db.compileStatement(line);

	int nLen = 0;
	def.m_pgr->WriteToBlob(NULL,nLen);

	BYTE *pData = new BYTE[nLen];
	def.m_pgr->WriteToBlob(pData,nLen);
	
	stm.bind(1,pData,nLen);
	
	stm.execDML();
	stm.finalize();

	delete[] pData;
	
	return TRUE;
}


BOOL CSQLiteAccess::DelCellDef(LPCTSTR name)
{
	if( name==NULL || _tcslen(name)==0 )
		return FALSE;

	char line[1024] = {0};
	
	CString name2 = ReplaceQuotationMark(name);

	sprintf(line,"delete from CellDef where Name='%s';",(LPCSTR)name2);
	int nRet = m_db.execDML(line);
	
	return (nRet>0);
}


BOOL CSQLiteAccess::DelAllCellDefs()
{
	int nRet = m_db.execDML("delete from CellDef;");
	
	return (nRet>0);
}


CString GetOptTableName(int nGeoCls)
{
	CString strTable;
	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT || nGeoCls == CLS_GEOSURFACEPOINT)
		strTable = "PointOptFields";
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT )
		strTable = "LineOptFields";
	else if( nGeoCls==CLS_GEOSURFACE || nGeoCls==CLS_GEOMULTISURFACE)
		strTable = "SurfaceOptFields";
	else if( nGeoCls==CLS_GEOTEXT )
		strTable = "TextOptFields";	
	
	return strTable;
}

//创建可选属性；可以创建多次，只会累加，不会删除；sqlite也不支持删除属性列；
BOOL CSQLiteAccess::CreateOptAttributes(int nGeoCls, CStringArray& fields, CStringArray& field_types)
{
	CString table_name = GetOptTableName(nGeoCls);

	CString line;
	
	if( !m_db.tableExists(table_name) )
	{
		//m_db.execDML("create table DLGVectorLayer(ID INT PRIMARY KEY, Name VARCHAR, MapName VARCHAR, Color int, Visible int, Locked int);");

		line = CString("create table ") + table_name + "(FTRID char[32] PRIMARY KEY, DELETED INT";
		for( int i=0; i<fields.GetSize(); i++)
		{
			line += ",";
			line += fields[i];
			line += ' ';
			line += field_types[i];
		}
		line += ");";

		m_db.execDML(line);
		return TRUE;
	}
	else
	{
		line.Format("select * from %s where 0", (LPCSTR)table_name);
		CppSQLite3Query query = m_db.execQuery(line);	

		CStringArray fields0;

		for(int i=0;i<query.numFields();i++)
		{
			fields0.Add(CString(query.fieldName(i)));
		}

		for( i=fields.GetSize()-1; i>=0; i--)
		{
			CString f = fields[i];
			for( int j=0; j<fields0.GetSize(); j++)
			{
				if( f.CompareNoCase(fields0[j])==0 )
					break;
			}
			if( j<fields0.GetSize() )
			{
				fields.RemoveAt(i);
				field_types.RemoveAt(i);
			}
		}
		
		CString line2;
		for( i=0; i<fields.GetSize(); i++)
		{
			line2.Format("ALTER TABLE %s ADD %s %s;",(LPCSTR)table_name,(LPCSTR)fields[i],(LPCSTR)field_types[i]);
			m_db.execDML(line2);
		}
		return TRUE;
	}
}


BOOL CSQLiteAccess::GetOptAttributeDef(int nGeoCls, CStringArray& fields, CStringArray& field_types)
{
	fields.RemoveAll();
	field_types.RemoveAll();

	char line[1024] = {0};
	CppSQLite3Query query;
	CString strTable = GetOptTableName(nGeoCls);
	
	if(strTable.IsEmpty()) return FALSE;

	if( !m_db.tableExists(strTable) )
		return FALSE;
	
	sprintf(line,"select * from %s where 0;",(LPCSTR)strTable);
	query = m_db.execQuery(line);
	
	// 读取扩展数据		
	long nValue;
	const char* pstr;
	double lfValue = 0;
	int nVauleType;
	char field[256];	
	
	for(int i = 2;i<query.numFields();i++)
	{
		memset(field,0,sizeof(field));
		
		nVauleType = query.fieldDataType(i);
		const char* pfield = query.fieldName(i);
		if (pfield != NULL)
		{
			strncpy(field, pfield,sizeof(field)-1);
			fields.Add(CString(field));
			field_types.Add(CString(query.fieldDeclType(i)));
		}
	}
	query.finalize();
	
	return TRUE;
}


BOOL CSQLiteAccess::ReadOptAttributes(CFeature *pFtr, CValueTable& tab)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	CString strTable = GetOptTableName(pFtr->GetGeometry()->GetClassType());
	
	if(strTable.IsEmpty()) return FALSE;

	if( !m_db.tableExists(strTable) )
		return FALSE;

	sprintf(line,"select * from %s where FTRID='%s';",(LPCSTR)strTable,pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	if(query.eof()) return FALSE;

	// 读取扩展数据		
	long nValue;
	const char* pstr;
	double lfValue = 0;
	int nVauleType;
	char field[256];	
	
	for(int i = 2;i<query.numFields();i++)
	{
		memset(field,0,sizeof(field));
		
		nVauleType = query.fieldDataType(i);
		const char* pfield = query.fieldName(i);
		if (pfield != NULL)
		{
			strncpy(field, pfield,sizeof(field)-1);
		}
		CString field2 = (LPCTSTR)field;
		
		switch(nVauleType)
		{
		case SQLITE_INTEGER:
			nValue = query.getIntField(i);
			tab.AddValue(field2,&(CVariantEx)(_variant_t)nValue);
			break;
		case SQLITE_FLOAT:
			lfValue = query.getFloatField(i);
			tab.AddValue(field2,&(CVariantEx)(_variant_t)lfValue);
			break;
		case SQLITE_TEXT:
			pstr = query.getStringField(i);
			tab.AddValue(field2,&(CVariantEx)(_variant_t)pstr);
			break;
		default:
			break;
		}		
	}
	query.finalize();
	
	return TRUE;
}


BOOL CSQLiteAccess::DelOptAttributes(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	CString strTable = GetOptTableName(pFtr->GetGeometry()->GetClassType());

	if(strTable.IsEmpty()) return FALSE;
	
	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
		return FALSE;
	
	//在扩展属性表中更新地物的对应项的DEL标记
	sprintf(line,"update %s set DELETED=1 where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	
	m_db.execDML(line);	
	query.finalize();
	return TRUE;
}


BOOL CSQLiteAccess::RestoreOptAttributes(CFeature *pFtr)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	CString strTable = GetOptTableName(pFtr->GetGeometry()->GetClassType());
	
	if(strTable.IsEmpty()) return FALSE;
	
	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
		return FALSE;
	
	//在扩展属性表中更新地物的对应项的DEL标记
	sprintf(line,"update %s set DELETED=0 where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	
	m_db.execDML(line);	
	query.finalize();
	return TRUE;
}


BOOL CSQLiteAccess::SaveOptAttributes(CFeature *pFtr, CValueTable& tab,int idx)
{
	char line[1024] = {0};
	CppSQLite3Query query;
	CString strTable = GetOptTableName(pFtr->GetGeometry()->GetClassType());
	
	if(strTable.IsEmpty()) return FALSE;
	
	//扩展属性表不存在，则返回 
	if(!m_db.tableExists(LPCSTR(strTable)))
		return FALSE;	

	//在扩展属性表中查找地物的对应项是否存在
	sprintf(line,"select * from %s where FTRID='%s';",(LPCSTR)strTable,(LPCSTR)pFtr->GetID().ToString());
	query = m_db.execQuery(line);
	
	CString str, str0, str1;
	const CVariantEx *var;
	// 插入新数据
	if( query.eof() )
	{
		str.Format("insert into %s(FTRID,DELETED",(LPCSTR)strTable);
		str0.Format("values('%s',0",(LPCSTR)pFtr->GetID().ToString());
		for(int i = 2;i<query.numFields();i++)
		{	
			str+=",";
			str+=query.fieldName(i);
			
			char field[256] = {0};
			const char* pfield = query.fieldName(i);
			if (pfield != NULL)
			{
				strncpy(field, pfield,sizeof(field)-1);
			}
			
			if (tab.GetValue(idx,(LPCTSTR)field,var))
			{
				int fieldType = var->GetType();
				if( VariantToTextA(*var,str1) )
				{
					str1 = ReplaceQuotationMark(str1);
					str0+=",";				
					
					if(fieldType==VT_BSTR)	
						str0+="'";
					str0+=str1;
					if(fieldType==VT_BSTR)		
						str0+="'";						
				}
				else
				{
					str0+=",NULL";
				}
			}
			else
			{
				str0+=",NULL";
			}
		}
		str+=") ";
		str0+=");";
		str+=str0;
		m_db.execDML(LPCSTR(str));
	}
	// 更新数据
	else
	{
		str.Format("update %s set DELETED=0",strTable);		
		for(int i = 2;i<query.numFields();i++)
		{	
			char field[256] = {0};
			const char* pfield = query.fieldName(i);
			if (pfield != NULL)
			{
				strncpy(field, pfield,sizeof(field)-1);
			}
			
			if (tab.GetValue(idx,field,var))
			{
				int fieldType = query.fieldDataType(i);
				if( SqliteVariantToText(*var,str1,fieldType) )
				{
					str+=",";
					str+=query.fieldName(i);
					str+="=";	

					str1 = ReplaceQuotationMark(str1);
					if(fieldType==SQLITE_TEXT)		
						str+="'";
					str+=str1;
					if(fieldType==SQLITE_TEXT)		
						str+="'";
				}				
			}				
		}
		str+=" where FTRID='";
		str+=pFtr->GetID().ToString();
		str+="';";	
		m_db.execDML(LPCSTR(str));		
	}
	query.finalize();
    return TRUE;

}


MyNameSpaceEnd