// CADLinFile.cpp: implementation of the CCADLinFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Symbol.h"
#include "GrBuffer.h"
#include "SmartViewFunctions.h"
#include "CADSymFile.h"
#include "Linearizer.h"
#include "GrTrim.h"
#include <stack>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin


//删除头尾的空格
static void RemoveEndSpace(char *str)
{
	int len = strlen(str);
	if( len<=0 )
		return;

	char *p0 = str;
	while( *p0==' ' || *p0=='\t' || *p0=='\r' || *p0=='\n' )p0++;
	
	char *p1 = str+strlen(str)-1;
	while( *p1==' ' || *p1=='\t' || *p1=='\r' || *p1=='\n' )p1--;

	if( p1<p0 )
	{
		str[0] = 0;
		return;
	}

	memmove(str,p0,p1-p0+1);
	str[p1-p0+1] = 0;
}


static void RemoveSpace(char *str)
{	
	char *p0 = str;
	int pos = 0;
	while( *p0!=0 )
	{
		if( *p0==' ' || *p0=='\t' || *p0=='\r' || *p0=='\n' )
		{
		}
		else 
		{
			if( p0!=(str+pos) )
				str[pos] = *p0;

			pos++;
		}
		p0++;
	}
	str[pos] = 0;
}


static BOOL ReadOneData(char *data, char *&pPos, char *pMax)
{
	data[0] = 0;

	char buf[256];
	int i = 0;
	while( *pPos!=0 && *pPos!=',' && pPos<pMax && i<sizeof(buf) )
	{
		if( *pPos!='(' && *pPos!=')' )
		{
			buf[i++] = *pPos;
		}
		pPos++;
	}

	buf[i] = 0;

	if( pPos<pMax )pPos++;
	else
		return FALSE;

	char *p0 = buf;
	while( *p0==' ' || *p0=='\t' || *p0=='\r' || *p0=='\n' )p0++;

	char *p1 = buf+i-1;
	while( *p1==' ' || *p1=='\t' || *p1=='\r' || *p1=='\n' )p1--;

	if( p1<p0 )
	{
		data[0] = 0;
		return FALSE;
	}

	memcpy(data,p0,p1-p0+1);
	data[p1-p0+1] = 0;

	return TRUE;
}

static char NextValidChar(const char *p, char *&p2)
{
	while( *p==' ' || *p=='\t' || *p=='\r' || *p=='\n'  )p++;

	p2 = (char*)p;

	return *p;
}

static void GetTextContent(const char *p, char *buf)
{
	const char *p0 = p;
	while( *p0!='"' && *p0!=0 )p0++;
	if( *p0==0 )
		return;
	
	p0++;
	int i = 0;
	while( *p0!='"' && *p0!=0 )
	{
		buf[i++] = *p0++;
	}
	buf[i] = 0;
}

static BOOL GetShapeParam(const char *p, char& field, float& value)
{
	char buf[256] = {0};
	strncpy(buf,p,sizeof(buf)-1);
	RemoveSpace(buf);

	if( sscanf(p,"%c=%f",&field,&value)==2 )
		return TRUE;
	else
		return FALSE;
}


static int ReadInt(const char *p)
{
	char *p2;
	int sign = 0;
	if( p[0]=='+' || p[0]=='-' )
	{
		if( p[0]=='-' )
			sign = 0x80;
		p++;
	}

	if( p[0]=='0' )
	{		
		return (sign|strtol(p+1,&p2,16));
	}
	else
	{
		int n = strtol(p,&p2,10);
		return (sign==0?n:-n);
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCADLinFile::CCADLinFile()
{
	m_nLTNum = 0;
	m_nLTMemLen = 0;
	m_pLTDefs = NULL;
}

CCADLinFile::~CCADLinFile()
{
	if( m_pLTDefs )delete[] m_pLTDefs;
}


BOOL CCADLinFile::Open(const char *fileName)
{
	m_nLTNum = 0;

	FILE *fp = fopen(fileName,"r");
	if( !fp )return FALSE;

	CAD_LTDEF def;
	CAD_LT_SEGMENT seg;

	memset(&seg,0,sizeof(seg));

	char line[1024] = {0};
	int type = 0; //当前类型
	while( !feof(fp) ) 
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		if( strlen(line)<=0 )continue;

		RemoveEndSpace(line);

		//注释
		if( line[0]==';' )
			continue;

		char *pMax = line+strlen(line)+1;

		//线型的定义
		if( line[0]=='*' )
		{
			type = 1;

			//读取线型名称
			char *p = line+1;
			ReadOneData(def.name,p,pMax);

			ReadOneData(def.descr,p,pMax);			
		}
		//线型的数据
		else if( type==1 )
		{
			char *p = line;
			char data[256];

			ReadOneData(data,p,pMax);

			//对齐标志
			if( data[0]!='A' )continue;

			int complex = 0;
			char *pos2;

			while( ReadOneData(data,p,pMax) )
			{
				int len = strlen(data);
				if( len<=0 )
					continue;

				//插入的对象
				if( data[0]=='[' )
				{				
					char *p2 = 0;
					//插入了文字对象
					if( NextValidChar(data+1,p2)=='"')
					{
						complex = 2;

						seg.InitText();
						//文字内容
						GetTextContent(data+1,seg.data.text.content);

						if( !ReadOneData(data,p,pMax) )
							break;

						len = strlen(data);

						//文字风格
						if( data[len-1]==']' )
						{
							data[len-1] = 0;
							strncpy(seg.data.text.style,data,sizeof(seg.data.text.style)-1);
							data[len-1] = ']';
						}
						else
							strncpy(seg.data.text.style,data,sizeof(seg.data.text.style)-1);
					}
					//插入了形对象
					else
					{
						complex = 1;

						seg.InitShape();
						//形对象名
						strncpy(seg.data.shape.shape_name,p2,sizeof(seg.data.shape.shape_name)-1);
						if( !ReadOneData(data,p,pMax) )
							break;

						len = strlen(data);

						//形文件名
						if( data[len-1]==']' )
						{
							data[len-1] = 0;
							strncpy(seg.data.shape.shapefile_name,data,sizeof(seg.data.shape.shapefile_name)-1);
							data[len-1] = ']';
						}
						else
							strncpy(seg.data.shape.shapefile_name,data,sizeof(seg.data.shape.shapefile_name)-1);
					}
				}
				//读取复杂线型
				else if( complex )
				{
					char field = 0;
					float value = 0;

					if( GetShapeParam(data,field,value) )
					{
						if( complex==1 )
						{
							switch(field)
							{
							case 'r':
							case 'R':
								seg.data.shape.relative_ang = value;
								break;
							case 'a':
							case 'A':
								seg.data.shape.absolute_ang = value;
								break;
							case 's':
							case 'S':
								seg.data.shape.scale = value;
								break;
							case 'x':
							case 'X':
								seg.data.shape.dx = value;
								break;
							case 'y':
							case 'Y':
								seg.data.shape.dy = value;
								break;
							}
						}
						else
						{
							switch(field)
							{
							case 'r':
							case 'R':
								seg.data.text.relative_ang = value;
								break;
							case 'a':
							case 'A':
								seg.data.text.absolute_ang = value;
								break;
							case 's':
							case 'S':
								seg.data.text.scale = value;
								break;
							case 'x':
							case 'X':
								seg.data.text.dx = value;
								break;
							case 'y':
							case 'Y':
								seg.data.text.dy = value;
								break;
							}
						}
					}
				}
				else
				{
					//读取简单线型
					seg.InitDash();
					seg.data.dash_len = strtod(data,&pos2);
					if( pos2!=data )
					{
						if( def.segnum<sizeof(def.segs)/sizeof(def.segs[0]) )
							def.segs[def.segnum++] = seg;

					}
				}
				
				if( data[len-1]==']' )
				{
					complex = 0;

					def.segs[def.segnum++] = seg;
				}
			}

			if( def.segnum>0 )
			{
				AddItem(&def);
				memset(&def,0,sizeof(def));
			}
		}
	}

	fclose(fp);

	return TRUE;
}


int CCADLinFile::GetCount()
{
	return m_nLTNum;
}


const CAD_LTDEF *CCADLinFile::GetItem(int idx)
{
	if( idx>=0 && idx<m_nLTNum )
		return m_pLTDefs+idx;
	else
		return NULL;
}



const CAD_LTDEF *CCADLinFile::GetItem(const char *name)
{
	if( name==NULL )
		return NULL;

	for( int i=0; i<m_nLTNum; i++)
	{
		if( stricmp(m_pLTDefs[i].name,name)==0 )
			return m_pLTDefs+i;
	}
	
	return NULL;
}


void CCADLinFile::GetShapeFileNames(CStringArray& arrNames)
{
	for( int i=0; i<m_nLTNum; i++)
	{
		for( int j=0; j<m_pLTDefs[i].segnum; j++)
		{
			if( m_pLTDefs[i].segs[j].type==1 )
			{
				if( strlen(m_pLTDefs[i].segs[j].data.shape.shapefile_name)>0 )
				{
					CString name = m_pLTDefs[i].segs[j].data.shape.shapefile_name;
					for( int k=0; k<arrNames.GetSize(); k++)
					{
						if( name.CompareNoCase(arrNames[k])==0 )
							break;
					}
					if( k>=arrNames.GetSize() )
					{
						arrNames.Add(name);
					}
				}
			}
		}
	}
}

void CCADLinFile::AddItem(CAD_LTDEF *def)
{
	for( int i=0; i<m_nLTNum; i++)
	{
		if( stricmp(m_pLTDefs[i].name,def->name)==0 )
			return;
	}
	if( m_nLTNum>=m_nLTMemLen )
	{
		int newlen = 0;
		if( m_nLTMemLen<128 )
			newlen = 128;
		else
			newlen = m_nLTMemLen*2;

		CAD_LTDEF *pNew = new CAD_LTDEF[newlen];

		if( !pNew )return;
		if( m_pLTDefs )
		{
			memcpy(pNew, m_pLTDefs, sizeof(CAD_LTDEF)*m_nLTNum);
			delete[] m_pLTDefs;
		}

		m_pLTDefs = pNew;
		m_nLTMemLen  = newlen;
	}

	memcpy(m_pLTDefs+m_nLTNum, def, sizeof(CAD_LTDEF));
	m_nLTNum++;
}


BOOL CCADLinFile::OpenBin(const char *fileName)
{	
	FILE *fp = fopen(fileName,"rb");
	if( !fp )
		return FALSE;
	
	int num = 0;
	fread(&num,sizeof(int),1,fp);
	if( num<=0 || num>=10000 )
	{
		fclose(fp);
		return FALSE;
	}
	
	CAD_LTDEF *pLTDefs = new CAD_LTDEF[num];
	int nSize = fread(pLTDefs,sizeof(CAD_LTDEF),num,fp);
	
	fclose(fp);
	
	if( nSize==num )
	{
		for( int i=0; i<num; i++)
		{
			AddItem(pLTDefs+i);
		}
		delete[] pLTDefs;
		return TRUE;
	}
	else
	{
		delete[] pLTDefs;
		return FALSE;
	}
}


BOOL CCADLinFile::SaveBin(const char *fileName)
{
	if( m_pLTDefs==NULL || m_nLTNum<=0 )
		return FALSE;
	
	FILE *fp = fopen(fileName,"wb");
	if( !fp )
		return FALSE;
	
	fwrite(&m_nLTNum,sizeof(int),1,fp);
	fwrite(m_pLTDefs,sizeof(CAD_LTDEF),m_nLTNum,fp);
	fclose(fp);
	
	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


static BOOL ReadBytes(void *pDes, char *&pSrc, int size, char *pMax)
{
	if( pMax-pSrc<size )
		return FALSE;

	memcpy(pDes,pSrc,size);
	pSrc += size;

	return TRUE;
}

CCADShxFile::CCADShxFile()
{
	m_nShapeNum = 0;
	m_nShapeMemLen = 0;
	m_pShapeDefs = NULL;
}

CCADShxFile::~CCADShxFile()
{
	if( m_pShapeDefs )delete[] m_pShapeDefs;
}


static CString GetFileName(LPCTSTR name)
{
	CString strPath = name;
	int pos = strPath.ReverseFind('\\');
	if( pos>=0 )
	{
		strPath = strPath.Mid(pos+1);		
	}
	return strPath;
}


BOOL CCADShxFile::Open(const char *fileName)
{
	CString text = fileName;
	if( text.Right(3).CompareNoCase("shp")==0 )
		return OpenSHP(fileName);
	
	return OpenSHX(fileName);
}

BOOL CCADShxFile::OpenSHX(const char *fileName)
{
	//读取内容
	FILE *fp = fopen(fileName,"rb");
	if( !fp )return FALSE;

	fseek(fp,0,SEEK_END);

	long nFileLen = ftell(fp);
	fseek(fp,0,SEEK_SET);

	char *pFile = new char[nFileLen+1];
	if( pFile )
	{
		fread(pFile,1,nFileLen,fp);
		pFile[nFileLen] = 0;
	}
	fclose(fp);

	char *pMax = pFile + nFileLen;
	char *pos = pFile;

	//读取版本
	char version[64]={0};
	if( !ReadBytes(version,pos,24,pMax) )
	{
		delete[] pFile;
		return FALSE;
	}

	if( strncmp(version,"AutoCAD-86 shapes 1.",20)!=0 )
	{
		delete[] pFile;
		return FALSE;
	}

	//读取编码范围
	short no_range[2];
	ReadBytes(no_range,pos,sizeof(no_range),pMax);

	//读取形定义数目
	short shapenum = 0;
	ReadBytes(&shapenum,pos,sizeof(shapenum),pMax);
	if( shapenum<=0 )
	{
		delete[] pFile;
		return FALSE;
	}

	CAD_SHPDEF *pDefs = new CAD_SHPDEF[shapenum];
	if( !pDefs )
	{
		delete[] pFile;
		return FALSE;
	}

	memset(pDefs,0,sizeof(CAD_SHPDEF)*shapenum);

	short index[2];

	CString strName = GetFileName(fileName);

	//读取形定义的编号和长度
	for( int i=0; i<shapenum; i++)
	{
		if( !ReadBytes(index,pos,sizeof(index),pMax) )
		{
			delete[] pFile;
			delete[] pDefs;
			return FALSE;
		}

		strncpy(pDefs[i].filename,strName,sizeof(pDefs[i].filename)-1);

		pDefs[i].no = index[0];
		pDefs[i].bytes = index[1];
	}

	//读取形定义内容
	char buf[1024];
	for( i=0; i<shapenum; i++)
	{
		memset(buf,0,sizeof(buf));
		if( !ReadBytes(buf,pos,pDefs[i].bytes,pMax) )
		{
			delete[] pFile;
			delete[] pDefs;
			return FALSE;
		}

		strcpy(pDefs[i].name,buf);

		int len0 = strlen(buf)+1;
		pDefs[i].datalen = pDefs[i].bytes-len0;

		memcpy(pDefs[i].data,buf+len0,pDefs[i].bytes-len0);

		AddItem(pDefs+i);
	}

	delete[] pDefs;

	m_strPath = fileName;

	//完成
	delete[] pFile;
	return TRUE;
}



BOOL CCADShxFile::OpenSHP(const char *fileName)
{	
	FILE *fp = fopen(fileName,"r");
	if( !fp )return FALSE;

	CString strName = GetFileName(fileName);

	CAD_SHPDEF def;
	char line[1024] = {0};
	int index = 0;
	while( !feof(fp) ) 
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);

		int nLineChar = strlen(line);
		if( nLineChar<=0 )continue;

		char *p = line;
		char data[256];

		char *pMax = line + nLineChar+1;

		if( !ReadOneData(data,p,pMax) )
			continue;

		if( data[0]==';' )
			continue;

		//形定义头部
		if( data[0]=='*' )
		{
			def.no = atoi(data+1);

			ReadOneData(data,p,pMax);

			def.datalen = atoi(data);

			ReadOneData(data,p,pMax);
			strcpy(def.name,data);

			index = 0;
		}
		//形定义内容
		else
		{
			def.data[index++] = ReadInt(data);
			for( ; index<def.datalen; index++)
			{
				if( !ReadOneData(data,p,pMax) )
					break;

				def.data[index] = ReadInt(data);
				strncpy(def.data_str[index],data,7);
			}

			if( index>=def.datalen || def.data[index-1]==0  )
			{
				strncpy(def.filename,strName,sizeof(def.filename)-1);
				AddItem(&def);
			}
		}
	}

	fclose(fp);

	m_strPath = fileName;
	
	return TRUE;
}


int CCADShxFile::GetCount()
{
	return m_nShapeNum;
}


const CAD_SHPDEF *CCADShxFile::GetItem(int idx)
{
	if( idx>=0 && idx<m_nShapeNum )
		return m_pShapeDefs+idx;
	else
		return NULL;
}



const CAD_SHPDEF *CCADShxFile::GetItem(const char *filename, const char *name)
{
	if( name==NULL )
		return NULL;

	for( int i=0; i<m_nShapeNum; i++)
	{
		if( filename!=NULL )
		{
			if( stricmp(m_pShapeDefs[i].filename,filename)!=0 )
				continue;
		}
		if( stricmp(m_pShapeDefs[i].name,name)==0 )
			return m_pShapeDefs+i;
	}

	return NULL;
}


void CCADShxFile::AddItem(CAD_SHPDEF *def)
{
	for( int i=0; i<m_nShapeNum; i++)
	{
		if( stricmp(m_pShapeDefs[i].filename,def->filename)==0 &&
			stricmp(m_pShapeDefs[i].name,def->name)==0 )
			return;
	}
	if( m_nShapeNum>=m_nShapeMemLen )
	{
		int newlen = 0;
		if( m_nShapeMemLen<128 )
			newlen = 128;
		else
			newlen = m_nShapeMemLen*2;

		CAD_SHPDEF *pNew = new CAD_SHPDEF[newlen];

		if( !pNew )return;
		if( m_pShapeDefs )
		{
			memcpy(pNew, m_pShapeDefs, sizeof(CAD_SHPDEF)*m_nShapeNum);
			delete[] m_pShapeDefs;
		}

		m_pShapeDefs = pNew;
		m_nShapeMemLen  = newlen;
	}

	memcpy(m_pShapeDefs+m_nShapeNum, def, sizeof(CAD_SHPDEF));
	m_nShapeNum++;
}

BOOL CCADShxFile::OpenBin(const char *fileName)
{
	FILE *fp = fopen(fileName,"rb");
	if( !fp )
		return FALSE;

	int num = 0;
	fread(&num,sizeof(int),1,fp);
	if( num<=0 || num>=10000 )
	{
		fclose(fp);
		return FALSE;
	}

	CAD_SHPDEF *pShapeDefs = new CAD_SHPDEF[num];
	int nSize = fread(pShapeDefs,sizeof(CAD_SHPDEF),num,fp);

	fclose(fp);

	if( nSize==num )
	{
		for( int i=0; i<num; i++)
		{
			AddItem(pShapeDefs+i);
		}
		delete[] pShapeDefs;
		return TRUE;
	}
	else
	{
		delete[] m_pShapeDefs;
		m_pShapeDefs = NULL;
		return FALSE;
	}
}


BOOL CCADShxFile::SaveBin(const char *fileName)
{
	if( m_pShapeDefs==NULL || m_nShapeNum<=0 )
		return FALSE;

	FILE *fp = fopen(fileName,"wb");
	if( !fp )
		return FALSE;

	fwrite(&m_nShapeNum,sizeof(int),1,fp);
	fwrite(m_pShapeDefs,sizeof(CAD_SHPDEF),m_nShapeNum,fp);
	fclose(fp);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCADShxFileManager::CCADShxFileManager()
{
}


CCADShxFileManager::~CCADShxFileManager()
{
	for( int i=0; i<m_arrPFiles.GetSize(); i++)
	{
		delete m_arrPFiles[i];
	}
}



void CCADShxFileManager::SetPath(LPCTSTR path)
{
	m_strPath = path;
	if( m_strPath.Right(1)!='\\' )
	{
		m_strPath += '\\';
	}
}


const CAD_SHPDEF *CCADShxFileManager::GetShape(const char *file_name, const char *shp_name)
{
	for( int i=0; i<m_arrPFiles.GetSize(); i++)
	{
		CString path = m_arrPFiles[i]->m_strPath;
		int pos = path.ReverseFind('\\');
		if( pos>=0 )
			path = path.Mid(pos+1);

		if( path.CompareNoCase(file_name)==0 )
		{
			return m_arrPFiles[i]->GetItem(NULL,shp_name);
		}
	}

	CCADShxFile *pNewFile = new CCADShxFile();
	if( pNewFile->Open(m_strPath+file_name) )
	{
		m_arrPFiles.Add(pNewFile);
		return pNewFile->GetItem(NULL,shp_name);
	}

	return NULL;
}


CCADShxFileManager *CCADShxFileManager::GetObject()
{
	static CCADShxFileManager m;
	
	return &m;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCADPatFile::CCADPatFile()
{
	m_nHatchNum = 0;
	m_nHatchMemLen = 0;
	m_pHatchDefs = NULL;
}

CCADPatFile::~CCADPatFile()
{
	if( m_pHatchDefs )delete[] m_pHatchDefs;
}


BOOL CCADPatFile::Open(const char *fileName)
{
	FILE *fp = fopen(fileName,"r");
	if( !fp )return FALSE;

	CAD_HATCHDEF def;
	char line[1024] = {0};
	int type = 0; //当前类型
	while( !feof(fp) ) 
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		if( strlen(line)<=0 )continue;

		RemoveEndSpace(line);

		//注释
		if( line[0]==';' )
			continue;

		char data[256], *p = line, *pMax = line+strlen(line)+1;

		//定义
		if( line[0]=='*' )
		{
			//存储上次的hatch
			if( type==1 && def.segnum>0 )
			{
				AddItem(&def);
				memset(&def,0,sizeof(def));
			}

			type = 1;

			p = line+1;

			//读取名称
			ReadOneData(def.name,p,pMax);

			//读取描述
			ReadOneData(def.descr,p,pMax);
			
		}
		//数据
		else if( type==1 )
		{
			//读取Hatch seg的参数
			CAD_HATCHDEF::HATCH_SEG *pseg = def.segs + def.segnum;
			if( sscanf(line,"%lf,%lf,%lf,%lf,%lf",&pseg->ang, &pseg->xo, &pseg->yo, &pseg->dx, &pseg->dy)!=5 )
				continue;

			//跳过上述已读参数
			p = line;
			for( int i=0; i<5; i++)
			{
				ReadOneData(data,p,pMax);
			}

			//读取线型部分
			char *pos2;
			double seg = 0;

			while( ReadOneData(data,p,pMax) )
			{
				int len = strlen(data);
				if( len<=0 )
					continue;

				//读取简单线型
				seg = strtod(data,&pos2);
				if( pos2!=data )
				{
					if( pseg->segnum<sizeof(pseg->segs)/sizeof(pseg->segs[0]) )
						pseg->segs[pseg->segnum++] = seg;
					
				}
			}

			def.segnum++;
		}
	}

	//存储上次的hatch
	if( type==1 && def.segnum>0 )
	{
		AddItem(&def);
		memset(&def,0,sizeof(def));
	}

	fclose(fp);

	return TRUE;
}


int CCADPatFile::GetCount()
{
	return m_nHatchNum;
}


const CAD_HATCHDEF *CCADPatFile::GetItem(int idx)
{
	if( idx>=0 && idx<m_nHatchNum )
		return m_pHatchDefs+idx;
	else
		return NULL;
}

const CAD_HATCHDEF *CCADPatFile::GetItem(const char *name)
{
	if( name==NULL )
		return NULL;
	
	for( int i=0; i<m_nHatchNum; i++)
	{
		if( stricmp(m_pHatchDefs[i].name,name)==0 )
			return m_pHatchDefs+i;
	}
	
	return NULL;
}


void CCADPatFile::AddItem(CAD_HATCHDEF *def)
{
	for( int i=0; i<m_nHatchNum; i++)
	{
		if( stricmp(m_pHatchDefs[i].name,def->name)==0 )
			return;
	}
	if( m_nHatchNum>=m_nHatchMemLen )
	{
		int newlen = 0;
		if( m_nHatchMemLen<128 )
			newlen = 128;
		else
			newlen = m_nHatchMemLen*2;

		CAD_HATCHDEF *pNew = new CAD_HATCHDEF[newlen];
		if( !pNew )return;

		if( m_pHatchDefs )
		{
			memcpy(pNew, m_pHatchDefs, sizeof(CAD_HATCHDEF)*m_nHatchNum);
			delete[] m_pHatchDefs;
		}

		m_pHatchDefs = pNew;
		m_nHatchMemLen  = newlen;
	}

	memcpy(m_pHatchDefs+m_nHatchNum, def, sizeof(CAD_HATCHDEF));
	m_nHatchNum++;
}


BOOL CCADPatFile::OpenBin(const char *fileName)
{	
	FILE *fp = fopen(fileName,"rb");
	if( !fp )
		return FALSE;
	
	int num = 0;
	fread(&num,sizeof(int),1,fp);
	if( num<=0 || num>=10000 )
	{
		fclose(fp);
		return FALSE;
	}
	
	CAD_HATCHDEF *pHatchDefs = new CAD_HATCHDEF[num];
	int nSize = fread(pHatchDefs,sizeof(CAD_HATCHDEF),num,fp);
	
	fclose(fp);
	
	if( nSize==num )
	{
		for(int i=0; i<num; i++)
		{
			AddItem(pHatchDefs+i);
		}
		delete[] pHatchDefs;
		return TRUE;
	}
	else
	{
		delete[] m_pHatchDefs;
		m_pHatchDefs = NULL;
		return FALSE;
	}
}


BOOL CCADPatFile::SaveBin(const char *fileName)
{
	if( m_pHatchDefs==NULL || m_nHatchNum<=0 )
		return FALSE;
	
	FILE *fp = fopen(fileName,"wb");
	if( !fp )
		return FALSE;
	
	fwrite(&m_nHatchNum,sizeof(int),1,fp);
	fwrite(m_pHatchDefs,sizeof(CAD_HATCHDEF),m_nHatchNum,fp);
	fclose(fp);
	
	return TRUE;
}


void ShpToGrBuffer(const CAD_SHPDEF *pDef, GrBuffer *pBuf)
{
	pBuf->BeginLineString(RGB(255,255,255),0);

	double dxy[32] = { 
		1,0,		//0
		1,0.5,		//1
		1,1,		//2
		0.5,1,		//3
		0,1,		//4
		-0.5,1,		//5
		-1,1,		//6
		-1,0.5,		//7
		-1,0,		//8
		-1,-0.5,	//9
		-1,-1,		//A
		-0.5,-1,	//B
		0,-1,		//C
		0.5,-1,		//D
		1,-1,		//E
		1,-0.5,		//F
		};

	PT_3D pt(0,0,0);
	pBuf->MoveTo(&pt);

	int draw_on = 1;
	int scale_on = 0;
	int subshape_on = 0;
	int offset_on = 0;
	int offsets_on = 0;
	int octant_arc_on = 0;
	int fractional_arc_on = 0;
	int bulge_spec_arc_on = 0;
	int bulge_spec_arcs_on = 0;
	int dual_orientation_text_flag_on = 0;

	//特殊编码后面跟着的参数数目
	int spec_param_num = 0;
	int spec_param[20];

	double k = 1.0;

	stack<PT_3D> stk;

	for( int i=0; i<pDef->datalen; i++)
	{
		BYTE b = pDef->data[i];

		//记录参数
		if( spec_param_num>0 )
		{
			spec_param[spec_param_num-1] = b;
			spec_param_num--;
		}

		if( scale_on )
		{
			if( spec_param_num==0 )
			{
				if( scale_on==1 )
					k /= spec_param[0];
				else
					k *= spec_param[0];

				scale_on = 0;
			}

			continue;
		}

		//子形，太复杂，暂不处理
		if( subshape_on )
		{
			if( spec_param_num==0 )
				subshape_on = 0;

			continue;
		}

		if( offset_on || offsets_on )
		{
			if( spec_param_num==0 )
			{
				pt.x += k * (int)(char)(BYTE)spec_param[1];
				pt.y += k * (int)(char)(BYTE)spec_param[0];

				if( draw_on )
					pBuf->LineTo(&pt);
				else
					pBuf->MoveTo(&pt);

				if( offset_on )
					offset_on = 0;
				else
				{
					//全零时结束
					if( spec_param[0]==0 && spec_param[1]==0 )
						offsets_on = 0;
					else
						spec_param_num = 2;
				}
			}				

			continue;
		}

		//绘制八分圆弧
		if( octant_arc_on )
		{
			if( spec_param_num==0 )
			{
				double r = k * spec_param[1];
				BYTE b2 = spec_param[0];

				int start = ((b2>>4)&7);
				int num = (b2&0x0f);
				bool bclockwise = (b2>=128);

				if( num==0 )
					num = 8;

				if( bclockwise )
					num = -num;

				//绘制八分圆弧
				CLinearizer lz;
				CShapeLine sl;
				GrBuffer buf;
				lz.SetShapeBuf(&sl);
				lz.Arc(&PT_3D(),r,start*PI/4,(start+num)*PI/4,0,bclockwise);

				//将圆弧衔接到当前点上
				int npt = sl.GetPtsCount();
				PT_3D pt0 = sl.GetPt(0);
				PT_3D pt1 = sl.GetPt(npt-1);

				if( draw_on )
				{
					buf.BeginLineString(RGB(255,255,255),0);
					sl.ToGrBuffer(&buf);
					buf.End();
					buf.Move(pt.x-pt0.x,pt.y-pt0.y,pt.z-pt0.z);

					pBuf->AddBuffer(&buf);
				}

				//更新当前点
				pt.x = pt.x + pt1.x-pt0.x;
				pt.y = pt.y + pt1.y-pt0.y;
				pt.z = pt.z + pt1.z-pt0.z;

				pBuf->MoveTo(&pt);

				octant_arc_on = 0;
			}

			continue;				
		}

		//非对齐的八分圆弧
		if( fractional_arc_on )
		{
			if( spec_param_num==0 )
			{
				double r = k * (spec_param[1]+(spec_param[2]<<8));
				BYTE b2 = spec_param[0];
				bool bclockwise = (b2>=128);
				
				int start_octant = ((b2>>4)&7);
				int end_octant = (b2&0x0f);

				int start_ang = spec_param[4]*45/256 + start_octant*45;
				int end_ang = spec_param[3]*45/256 + end_octant*45;
				
				//绘制八分圆弧
				CLinearizer lz;
				CShapeLine sl;
				GrBuffer buf;
				lz.SetShapeBuf(&sl);
				lz.Arc(&PT_3D(),r,start_ang*PI/180,end_ang*PI/180,0,bclockwise);
				
				//将圆弧衔接到当前点上
				int npt = sl.GetPtsCount();
				PT_3D pt0 = sl.GetPt(0);
				PT_3D pt1 = sl.GetPt(npt-1);
				
				if( draw_on )
				{
					buf.BeginLineString(RGB(255,255,255),0);
					sl.ToGrBuffer(&buf);
					buf.End();
					buf.Move(pt.x-pt0.x,pt.y-pt0.y,pt.z-pt0.z);
					
					pBuf->AddBuffer(&buf);
				}
				
				//更新当前点
				pt.x = pt1.x + pt.x-pt0.x;
				pt.y = pt1.y + pt.y-pt0.y;
				pt.z = pt1.z + pt.z-pt0.z;

				pBuf->MoveTo(&pt);

				fractional_arc_on = 0;
			}
			continue;
		}

		//用弦高比来计算的圆弧
		if( bulge_spec_arc_on || bulge_spec_arcs_on )
		{
			int isover = 0;
			if( bulge_spec_arcs_on && spec_param_num==1 )
			{
				//前两个数全零时结束
				if( spec_param[2]==0 && spec_param[1]==0 )
				{
					bulge_spec_arcs_on = 0;
					spec_param_num = 0;
					isover = 1;
				}
			}

			if( spec_param_num==0 && !isover )
			{
				//前两个参数为弦的终点的dx、dy，第3个参数为(254*高D/弦H, D = 2d)
				int dx = (int)(char)(BYTE)spec_param[2];
				int dy = (int)(char)(BYTE)spec_param[1];
				double d = k * sqrt(dx*dx+dy*dy)*0.5;
				double H = abs((char)spec_param[0])*d/127;
				double r = (H*H + d*d)/(2*H);
				bool bclockwise = (((BYTE)spec_param[0])>=128);
				
				PT_3D center = pt;

				if( bclockwise )
				{
					center.x += dx*0.5 + 0.5*dy*(r-H)/d;
					center.y += dy*0.5 - 0.5*dx*(r-H)/d;
				}
				else
				{
					center.x += dx*0.5 - 0.5*dy*(r-H)/d;
					center.y += dy*0.5 + 0.5*dx*(r-H)/d;
				}
				
				double ang1 = GraphAPI::GGetAngle(center.x,center.y,pt.x,pt.y);
				double ang2 = GraphAPI::GGetAngle(center.x,center.y,pt.x+dx,pt.y+dy);
				
				//绘制圆弧
				if( draw_on )
				{
					CLinearizer lz;
					CShapeLine sl;
					GrBuffer buf;
					lz.SetShapeBuf(&sl);
					lz.Arc(&center,r,ang1,ang2,0,bclockwise);
					
					buf.BeginLineString(RGB(255,255,255),0);
					sl.ToGrBuffer(&buf);
					buf.End();
					
					pBuf->AddBuffer(&buf);
				}
				
				//更新当前点
				pt.x += dx;
				pt.y += dy;

				pBuf->MoveTo(&pt);
				
				if( bulge_spec_arc_on )
					bulge_spec_arc_on = 0;
				else
				{
					spec_param_num = 3;
				}
			}
			continue;
		}

		if( dual_orientation_text_flag_on )
		{
			//无任何处理
		}

		//特殊编码
		if( b<=15 )
		{
			//0 退出码
			if( b==0 )
				break;

			//1，2 落笔，抬笔
			if( b==1 )
			{
				draw_on = 1;
				spec_param_num = 0;
			}
			else if( b==2 )
			{
				draw_on = 0;
				spec_param_num = 0;
			}
			//3，4，尺寸的倍数控制
			else if( b==3 )
			{
				scale_on = 1;
				spec_param_num = 1;
			}
			else if( b==4 )
			{
				scale_on = 2;
				spec_param_num = 1;
			}
			//5，6，当前位置的堆栈操作
			else if( b==5 )
			{
				stk.push(pt);
			}
			else if( b==6 )
			{
				pt = stk.top();
				stk.pop();

				pBuf->MoveTo(&pt);
			}
			//7，子图形
			else if( b==7 )
			{
				subshape_on = 1;
				spec_param_num = 1;
			}
			//8，9 坐标偏移
			else if( b==8 )
			{
				offset_on = 1;
				spec_param_num = 2;
			}
			else if( b==9 )
			{
				offsets_on = 1;
				spec_param_num = 2;
			}
			//10，八分圆弧
			else if( b==10 )
			{
				octant_arc_on = 1;
				spec_param_num = 2;
			}
			//11，不用对齐的八分圆弧
			else if( b==11 )
			{
				fractional_arc_on = 1;
				spec_param_num = 5;
			}
			//12，13，弦高比控制的圆弧
			else if( b==12 )
			{
				bulge_spec_arc_on = 1;
				spec_param_num = 3;
			}
			else if( b==13 )
			{
				bulge_spec_arcs_on = 1;
				spec_param_num = 0;
			}
			//双向文字中垂直文字的控制位（仅当双向文字为垂直状态时，该标志的后一代码起作用，否则后一代码跳过）
			else if( b==14)
			{
				dual_orientation_text_flag_on = 1;
				spec_param_num = 0;
			}
		}
		//直线编码
		else
		{
			int len = (b>>4);
			int dir = (b&0x0f);

			pt.x += len * k * dxy[dir+dir];
			pt.y += len * k * dxy[dir+dir+1];

			if( draw_on && k<1e-4 )
			{
				pBuf->End();
				pBuf->Point(RGB(255,255,255),&pt,0,0);
				pBuf->BeginLineString(RGB(255,255,255),0);
				pBuf->MoveTo(&pt);
			}
			else
			{
				if( draw_on )
					pBuf->LineTo(&pt);
				else
					pBuf->MoveTo(&pt);
			}
		}
	}

	pBuf->End();
}

namespace{

class CLinetypePtHelper
{
public:
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CArray<double,double> m_arrLens;
	int m_nCurIndex;
	double m_lfRatio;
	BOOL m_bOutside;

	CLinetypePtHelper()
	{
		m_nCurIndex = 0;
		m_lfRatio = 0;
		m_bOutside = FALSE;
	}

	~CLinetypePtHelper()
	{

	}

	BOOL IsOutside()
	{
		return m_bOutside;
	}

	void SetPts(CArray<PT_3DEX,PT_3DEX>& arrPts)
	{
		m_nCurIndex = 0;
		m_lfRatio = 0;
		m_bOutside = FALSE;

		m_arrPts.Copy(arrPts);
		int npt = arrPts.GetSize();

		m_arrLens.SetSize(npt);
		for( int i=0; i<npt-1; i++)
		{
			m_arrLens[i] = GraphAPI::GGet2DDisOf2P(arrPts[i+1],arrPts[i]);
		}

		m_arrLens[npt-1] = 0;
	}

	void ResetPos()
	{
		m_nCurIndex = 0;
		m_lfRatio = 0;
		m_bOutside = FALSE;
	}

	void MoveDis(double dis)
	{
		if( dis==0 )
			return;

		m_bOutside = FALSE;
		if( dis>0 )
		{
			dis += m_arrLens[m_nCurIndex]*m_lfRatio;

			while( dis>=m_arrLens[m_nCurIndex] && m_nCurIndex<m_arrPts.GetSize()-1 )
			{
				dis -= m_arrLens[m_nCurIndex];
				m_nCurIndex++;
			}

			if( m_nCurIndex==(m_arrPts.GetSize()-1) )
			{
				if( dis>0 )
					m_bOutside = TRUE;

				m_lfRatio = 0;
			}
			else
				m_lfRatio = dis/m_arrLens[m_nCurIndex];
		}
		else
		{
			dis += m_arrLens[m_nCurIndex]*m_lfRatio;
			
			while( dis<0 && m_nCurIndex>0 )
			{
				dis += m_arrLens[m_nCurIndex-1];
				m_nCurIndex--;
			}
			
			if( m_nCurIndex==0 && dis<0 )
			{
				m_bOutside = TRUE;
				m_lfRatio = 0;
			}
			else
				m_lfRatio = dis/m_arrLens[m_nCurIndex];
		}
	}

	int GetCurIndex()
	{
		return m_nCurIndex;
	}

	PT_3D GetCurPt()
	{
		PT_3D pt;

		if( m_nCurIndex>=0 && m_nCurIndex<m_arrPts.GetSize()-1 )
		{
			PT_3D pt0 = m_arrPts[m_nCurIndex];
			PT_3D pt1 = m_arrPts[m_nCurIndex+1];
			pt.x = pt0.x + (pt1.x-pt0.x)*m_lfRatio;
			pt.y = pt0.y + (pt1.y-pt0.y)*m_lfRatio;
			pt.z = pt0.z + (pt1.z-pt0.z)*m_lfRatio;
		}
		return pt;
	}

	double GetCurAngle()
	{
		PT_3DEX *pts = m_arrPts.GetData();
		int idx = m_nCurIndex;
		if( idx>=0 && idx<m_arrPts.GetSize()-1 )
		{
			while( idx<m_arrPts.GetSize()-1 && fabs(pts[idx].x-pts[idx+1].x)<1e-6 && fabs(pts[idx].y-pts[idx+1].y)<1e-6 )
				idx++;

		}
		
		if( idx==(m_arrPts.GetSize()-1) )
		{
			while( idx>0 && fabs(pts[idx].x-pts[idx-1].x)<1e-6 && fabs(pts[idx].y-pts[idx-1].y)<1e-6 )
				idx--;

		}

		if( idx==m_nCurIndex )
			idx = m_nCurIndex+1;

		if( idx>m_nCurIndex )
			return GraphAPI::GGetAngle(pts[m_nCurIndex].x,pts[m_nCurIndex].y,pts[idx].x,pts[idx].y);
		else
			return GraphAPI::GGetAngle(pts[idx].x,pts[idx].y,pts[m_nCurIndex].x,pts[m_nCurIndex].y);		

		return 0;
	}

	PT_3D GetPtByDX(double dx)
	{
		if( dx==0 )
			return GetCurPt();

		int save_index = m_nCurIndex;
		double save_ratio = m_lfRatio;

		MoveDis(dx);

		PT_3D pt = GetCurPt();

		m_nCurIndex = save_index;
		m_lfRatio = save_ratio;

		return pt;
	}

	PT_3D GetPtByDY(double dy)
	{
		if( dy==0 )
			return GetCurPt();

		PT_3DEX *pts = m_arrPts.GetData();
		int idx = m_nCurIndex;
		if( idx>=0 && idx<m_arrPts.GetSize()-1 )
		{
			while( idx<m_arrPts.GetSize()-1 && fabs(pts[idx].x-pts[idx+1].x)<1e-6 && fabs(pts[idx].y-pts[idx+1].y)<1e-6 )
				idx++;
		}
		if( idx==(m_arrPts.GetSize()-1) )
		{
			while( idx>0 && fabs(pts[idx].x-pts[idx-1].x)<1e-6 && fabs(pts[idx].y-pts[idx-1].y)<1e-6 )
				idx--;
		}

		if( idx==m_nCurIndex )
			idx = m_nCurIndex+1;

		PT_3D pt = GetCurPt();

		if( fabs(pts[idx].x-pts[m_nCurIndex].x)<1e-6 && fabs(pts[idx].y-pts[m_nCurIndex].y)<1e-6 )
		{
			pt.y += dy;
			return pt;
		}

		double dx0 = pts[idx].x-pts[m_nCurIndex].x;
		double dy0 = pts[idx].y-pts[m_nCurIndex].y;
		if( idx<m_nCurIndex )			
		{
			dx0 = -dx0; dy0 = -dy0;
		}

		double dis = sqrt(dx0*dx0+dy0*dy0);

		PT_3D pt2;
		pt2.x = pt.x - dy*dy0/dis;
		pt2.y = pt.y + dy*dx0/dis;

		pt2.z = pt.z;
		return pt2;
	}

	PT_3D GetPtByDXY(double dx, double dy)
	{
		int save_index = m_nCurIndex;
		double save_ratio = m_lfRatio;
		
		MoveDis(dx);
		
		PT_3D pt = GetPtByDY(dy);
		
		m_nCurIndex = save_index;
		m_lfRatio = save_ratio;
		
		return pt;
	}

};

}




void LinToGrBuffer(const CAD_LTDEF *pDef, CCADSymbolLib *pCadSymLib, CArray<PT_3DEX,PT_3DEX>& arrPts, float line_width, float sym_scale, GrBuffer *pBuf)
{
	PT_3DEX *pts = arrPts.GetData();

	int nSum = arrPts.GetSize();

	if (!pts || nSum<=1 || !pBuf)  return;
	
	// 隐藏线
	int i, j;
	float alllen = 0, hidelen = 0;
	for (i=0; i<pDef->segnum; i++)
	{
		alllen += fabs(pDef->segs[i].GetLen());
	}

	// 实线
	if (pDef->segnum <= 1 || alllen<0.01 )
	{
		float wid = line_width;

		PT_3DEX *pRet = arrPts.GetData();
		int npt = arrPts.GetSize();

		for (i=0; i<npt; i++)
		{
			if (i==0||(pRet[i].pencode==penMove))
			{	
				if (i != 0)
				{
					pBuf->End();
				}
				pBuf->BeginLineString(RGB(255,255,255),wid,TRUE,0,1,FALSE);
				pBuf->MoveTo(&pRet[i]);
				
			}				
			else
				pBuf->LineTo(&pRet[i]);
			
		}

		pBuf->End();

		return;
	}

	CCADShxFile *pShxFile = pCadSymLib->GetShx();
	
	CLinetypePtHelper help;
	help.SetPts(arrPts);

	GrBuffer buf_shapes;

	pBuf->BeginLineString(RGB(255,255,255),line_width,TRUE,0,1,FALSE);
	pBuf->MoveTo(&pts[0]);

	CArray<PT_3D,PT_3D> arrPoints;

	float dis = 0;
	i = 0, j = 0;
	while( !help.IsOutside() )
	{
		int old_index = help.GetCurIndex();

		dis = pDef->segs[j].GetLen()*sym_scale;
		help.MoveDis(fabs(dis));

		PT_3D pt3d = help.GetCurPt();

		if( pDef->segs[j].type==0 )
		{
			if( dis>0 ) 
			{
				int new_index = help.GetCurIndex();
				for( i=old_index+1; i<=new_index; i++)
				{
					pBuf->LineTo(&arrPts[i]);
				}

				if( help.m_lfRatio!=0 )
					pBuf->LineTo(&pt3d);
			}
			else if( dis<0 )
				pBuf->MoveTo(&pt3d);
			else if( dis==0 )
			{
				arrPoints.Add(pt3d);
			}
		}
		else if( pDef->segs[j].type==1 )
		{
			const CAD_SHPDEF *pShpDef = pShxFile->GetItem(pDef->segs[j].data.shape.shapefile_name,pDef->segs[j].data.shape.shape_name);
			if( pShpDef )
			{
				GrBuffer buf;
				ShpToGrBuffer(pShpDef,&buf);
				
				double ang = pDef->segs[j].data.shape.relative_ang*PI/180 + help.GetCurAngle();

				PT_3D pt3d2 = help.GetPtByDXY(pDef->segs[j].data.shape.dx*sym_scale,pDef->segs[j].data.shape.dy*sym_scale);
				
				buf.Rotate(ang);
				buf.Zoom(pDef->segs[j].data.shape.scale*sym_scale,pDef->segs[j].data.shape.scale*sym_scale);
				buf.Move(pt3d2.x,pt3d2.y,pt3d.z);
				buf.SetAllLineWidth(TRUE,line_width);

				buf_shapes.AddBuffer(&buf);
			}
		}

		j = (j+1)%pDef->segnum;
	}

	pBuf->End();

	pBuf->BeginPointString(RGB(255,255,255),0,0);

	for( i=0; i<arrPoints.GetSize(); i++)
	{
		pBuf->PointString(&arrPoints[i]);
	}

	pBuf->End();

	pBuf->AddBuffer(&buf_shapes);

}


void LinToSymbol(const CAD_LTDEF *pDef, CPtrArray& dashLTs, CPtrArray& cellLTs)
{
	CDashLinetype dlt;
	CCellLinetype clt;

	double len1 = 0, len2 = 0;
	int bHaveCell = 0;

	for( int i=0; i<pDef->segnum; i++)
	{
		CAD_LT_SEGMENT seg = pDef->segs[i];
		if( seg.type==0 )
		{
			if( seg.data.dash_len>=0 )
				len1 += seg.data.dash_len;
			else
				len2 += seg.data.dash_len;
		}
		else if( seg.type==1 )
		{
			bHaveCell = 1;
		}
	}

	//虚线线型
	if( fabs(len1)>1e-4 && fabs(len2)>1e-4 )
	{
		CArray<float,float> arrLens;
		float len = 0;
		for( i=0; i<pDef->segnum; i++)
		{
			CAD_LT_SEGMENT seg = pDef->segs[i];
			if( seg.type==0 )
			{
				if( fabs(seg.data.dash_len)<1e-4 )
				{
				}
				else if( len==0 )
				{
					len += seg.data.dash_len;
				}
				else
				{
					if( len*seg.data.dash_len>0 )
						len += seg.data.dash_len;
					else
					{
						arrLens.Add(len);
						len = seg.data.dash_len;
					}
				}

			}
		}

		if( len!=0 )
			arrLens.Add(len);
		
		if( arrLens.GetSize()>=2 && arrLens.GetSize()<=8 )
		{
			CBaseLineTypeLib *pLTLib = GetBaseLineTypeLib();
			BaseLineType lt;
			strcpy(lt.m_name,CString("lin_")+pDef->name);
			lt.m_nNum = arrLens.GetSize();
			memcpy(lt.m_fLens,arrLens.GetData(),lt.m_nNum*sizeof(float));

			pLTLib->AddBaseLineType(lt);

			CDashLinetype *pSym1 = new CDashLinetype();
			pSym1->m_strBaseLinetypeName = lt.m_name;

			dashLTs.Add(pSym1);
		}
	}

	if( bHaveCell )
	{
		float len = 0;
		for( i=0; i<pDef->segnum; i++)
		{
			CAD_LT_SEGMENT seg = pDef->segs[i];
			if( seg.type==0 )
			{
				if( fabs(seg.data.dash_len)>1e-4 )
				{
					len += fabs(seg.data.dash_len);
				}
			}
			else if( seg.type==1 )
			{
				CCellLinetype *pSym2 = new CCellLinetype();
				pSym2->m_strCellDefName = CString("shx_") + seg.data.shape.shape_name;
				pSym2->m_fCycle = len1 - len2;
				pSym2->m_fkx = pSym2->m_fky = seg.data.shape.scale;
				pSym2->m_fBaseXOffset = len + seg.data.shape.dx;
				pSym2->m_fBaseYOffset = seg.data.shape.dy;
				pSym2->m_fAngle = seg.data.shape.relative_ang;

				cellLTs.Add(pSym2);
			}
		}
	}
}

//旋转的平面坐标系，(m_lfXO,m_lfyO)为原点，m_lfAngle为x轴
class CGen2DCoordSys
{
public:
	double m_lfXO, m_lfYO;
	double m_lfAngle;
	double m_lfCosA, m_lfSinA;
	
	CGen2DCoordSys()
	{
		m_lfXO = m_lfYO = 0;
		m_lfAngle = 0;
	}
	
	~CGen2DCoordSys()
	{
		
	}	
	
	void SetCoordAxis(double xo, double yo, double angle)
	{
		m_lfXO = xo;  m_lfYO = yo;
		m_lfCosA = cos(angle);  m_lfSinA = sin(angle);
		m_lfAngle = angle;
	}
	
	
	void LocalToGround(double& x, double &y)
	{
		double newx, newy;

		newx = x * m_lfCosA - y * m_lfSinA;
		newy = x * m_lfSinA + y * m_lfCosA;

		x = newx + m_lfXO;
		y = newx + m_lfYO;
	}	
	
	void GroundToLocal(double& x, double &y)
	{
		double newx = x-m_lfXO, newy = y-m_lfYO;

		x = newx * m_lfCosA + newy * m_lfSinA;
		y = -newx * m_lfSinA + newy * m_lfCosA;
	}
	
};


extern BOOL SolvePlaneEquation2(PT_3D *pts, int num, double* a,double* b,double* c);


void PatToGrBuffer(const CAD_HATCHDEF *pDef, CArray<PT_3DEX,PT_3DEX>& arrPts, float line_wid, float sym_scale, double xoff, double yoff,GrBuffer* buf)
{
	CAD_HATCHDEF def2;
	memcpy(&def2,pDef,sizeof(def2));

	double gscale = sym_scale;

	double dy_max = 0;
	double cz = 0;

	if( arrPts.GetSize()<=0 )
		return;

	//初始化裁剪器
	GrBuffer curbuf;
	for( int i=0; i<def2.segnum; i++)
	{
		if( dy_max<fabs(def2.segs[i].dy) )
			dy_max = fabs(def2.segs[i].dy);
	}

	CArray<PT_3D,PT_3D> pts_3d;
	int npt = arrPts.GetSize();
	pts_3d.SetSize(npt);
	for( i=0; i<npt; i++)
	{
		pts_3d[i] = arrPts[i];
		cz += pts_3d[i].z;
	}

	cz = cz/npt;

	double A, B, C;
	BOOL bSolved = SolvePlaneEquation2(pts_3d.GetData(),pts_3d.GetSize(),&A,&B,&C);

	CGrTrim	grTrim;
	
	grTrim.InitTrimPolygon(pts_3d.GetData(),pts_3d.GetSize(),dy_max);
	
	//计算外包矩形
	Envelope e;
	e.CreateFromPts(pts_3d.GetData(),pts_3d.GetSize());

	PT_3D pt_corners[4];

	pt_corners[0].x = e.m_xl; pt_corners[0].y = e.m_yl;
	pt_corners[1].x = e.m_xh; pt_corners[1].y = e.m_yl;
	pt_corners[2].x = e.m_xh; pt_corners[2].y = e.m_yh;
	pt_corners[3].x = e.m_xl; pt_corners[3].y = e.m_yh;

	PT_3D pt_corners2[4];

	GrBuffer buf_all;

	for( i=0; i<def2.segnum; i++)
	{
		//计算步进长度
		double dash_len = 0;
		for( int j=0; j<def2.segs[i].segnum; j++)
		{
			if( def2.segs[i].segs[j]<0 )
			{
				dash_len -= def2.segs[i].segs[j];
			}
			else
			{
				dash_len += def2.segs[i].segs[j];
			}
		}

		double dx = def2.segs[i].dx;
		double dy = def2.segs[i].dy;

		if( dy==0 )
			continue;

		dash_len *= gscale;
		dx *= gscale;
		dy *= gscale;

		CGen2DCoordSys cs;
		cs.SetCoordAxis(def2.segs[i].xo+xoff,def2.segs[i].yo+yoff,def2.segs[i].ang*PI/180);

		//将外包的角点pt_corners转到填充的局部旋转坐标系中
		memcpy(pt_corners2,pt_corners,sizeof(pt_corners2));
		for( j=0; j<4; j++)
		{
			cs.GroundToLocal(pt_corners2[j].x,pt_corners2[j].y);
		}

		//计算新的外包，作为后续遍历的范围
		Envelope e2;
		e2.CreateFromPts(pt_corners2,4);

		//计算一条线（该线在局部坐标系中是平行于X轴的线）的GrBuffer
		curbuf.DeleteAll();
		curbuf.BeginLineString(RGB(255,255,255),line_wid);

		PT_3D pt(-2*dash_len,0,0);
		curbuf.MoveTo(&pt);

		CArray<PT_3D,PT_3D> arrPoints;

		double wid = (dash_len==0?e2.Width():((ceil(e2.m_xh/dash_len)-floor(e2.m_xl/dash_len))*dash_len));
		wid += (2*dash_len);

		while( pt.x<wid )
		{
			//实线
			if( def2.segs[i].segnum==0 )
			{
				pt.x += wid;
				curbuf.LineTo(&pt);
			}
			else
			{
				for( j=0; j<def2.segs[i].segnum; j++)
				{
					if( fabs(def2.segs[i].segs[j])<1e-4 )
					{
						//打个点（先记录点坐标，稍后一起加入到buf中）
						if( (j>0 && def2.segs[i].segs[j-1]<0) || (j<def2.segs[i].segnum-1 && def2.segs[i].segs[j+1]<0 ) )
						{
							arrPoints.Add(pt);
						}
					}
					else if( def2.segs[i].segs[j]<0 )
					{
						pt.x -= def2.segs[i].segs[j]*gscale;
						curbuf.MoveTo(&pt);
					}
					else if( def2.segs[i].segs[j]>0 )
					{
						pt.x += def2.segs[i].segs[j]*gscale;
						curbuf.LineTo(&pt);
					}
				}
			}
		}
		
		curbuf.End();

		//逐行画线
		CArray<PT_3D,PT_3D> arrPoints2;

		if( dy<0 )
		{
			dy = -dy;
			dx = -dx;
		}

		double y = floor(e2.m_yl/dy)*dy;
		double ddx = floor(e2.m_yl/dy)*dx;
		for( ; y<e2.m_yh; y+=dy, ddx+=dx )
		{
			double x;
			if( dash_len==0 )
			{
				x = e2.m_xl;
			}
			else
			{
				x = ddx;
				
				x = x - floor((x-e2.m_xl)/dash_len + 0.5)*dash_len;
			}

			GrBuffer buf2;
			buf2.CopyFrom(&curbuf);
			buf2.Move(x,y);

			for( int k=0; k<arrPoints.GetSize(); k++)
			{
				PT_3D tpt = arrPoints[k];
				tpt.x += x;
				tpt.y += y;
				arrPoints2.Add(tpt);
			}

			buf2.Rotate(cs.m_lfAngle);
			buf2.Move(cs.m_lfXO,cs.m_lfYO);

			buf_all.AddBuffer(&buf2);
		}

		//将点串放入到Buf中
		GrBuffer buf2;
		buf2.BeginPointString(RGB(255,255,255),0,0);
		for( int k=0; k<arrPoints2.GetSize(); k++)
		{
			buf2.PointString(&arrPoints2[k]);
		}
		buf2.End();

		buf2.Rotate(cs.m_lfAngle);
		buf2.Move(cs.m_lfXO,cs.m_lfYO);
		
		buf_all.AddBuffer(&buf2);

		//对高程值进行内插
		BOOL bIntZ = (bSolved && fabs(C) > GraphAPI::GetZTolerance());
		int pts_num = buf_all.GetVertexPts(NULL);		

		GrVertex *pV = new GrVertex[pts_num];
		PT_3D *pP = new PT_3D[pts_num];
		buf_all.GetVertexPts(pV);
		for( int m=0; m<pts_num; m++)
		{
			pP[m] = pV[m];

			if( bIntZ )
			{
				pP[m].z = A*pP[m].x+B*pP[m].y+C;
			}
			else
			{
				pP[m].z = cz;
			}
		}
		buf_all.SetAllPts(pP);
		delete[] pV;
		delete[] pP;
	}
	
	//裁剪
	GrBuffer ret;
	grTrim.Trim(&buf_all,ret);
	
	buf->AddBuffer(&ret);
}


CCADSymbolLib::CCADSymbolLib()
{
	m_pLinFile = NULL;
	m_pPatFile = NULL;
	m_pShxFile = NULL;
}


CCADSymbolLib::~CCADSymbolLib()
{
	Clear();
}


void CCADSymbolLib::Clear()
{
	if( m_pLinFile )
		delete m_pLinFile;
	m_pLinFile = NULL;

	if( m_pPatFile )
		delete m_pPatFile;
	m_pPatFile = NULL;

	if( m_pShxFile )
		delete m_pShxFile;
	m_pShxFile = NULL;
}

void CCADSymbolLib::Create()
{
	if( !m_pLinFile )
		m_pLinFile = new CCADLinFile();
	if( !m_pPatFile )
		m_pPatFile = new CCADPatFile();
	if( !m_pShxFile )
		m_pShxFile = new CCADShxFile();

}

BOOL CCADSymbolLib::Load(LPCTSTR path)
{
	if( path==NULL )
		return FALSE;
	CString strPath = path;
	
	Clear();
	Create();

	m_pShxFile->OpenBin(strPath+"cadshx.dat");
	m_pLinFile->OpenBin(strPath+"cadlin.dat");
	m_pPatFile->OpenBin(strPath+"cadpat.dat");

	return FALSE;
}


BOOL CCADSymbolLib::Import(LPCTSTR path)
{
	if( path==NULL )
		return FALSE;

	Create();

	CString strPath = path;
	CString ext = strPath.Right(4);
	if( ext.CompareNoCase(".lin")==0 )
	{
		if( !m_pLinFile->Open(strPath) )
			return FALSE;

		CStringArray arrFiles;
		m_pLinFile->GetShapeFileNames(arrFiles);

		int pos = strPath.ReverseFind('\\');
		if( pos>=0 )
			strPath = strPath.Left(pos+1);

		for( int i=0; i<arrFiles.GetSize(); i++)
		{
			m_pShxFile->Open(strPath+arrFiles[i]);
		}
		return TRUE;
	}
	else if( ext.CompareNoCase(".pat")==0 )
	{
		return m_pPatFile->Open(strPath);
	}
	else
		return FALSE;
}


BOOL CCADSymbolLib::Save(LPCTSTR path)
{
	if( path==NULL )
		return FALSE;
	CString strPath = path;	

	if( m_pShxFile )
		m_pShxFile->SaveBin(strPath+"cadshx.dat");
	if( m_pLinFile )
		m_pLinFile->SaveBin(strPath+"cadlin.dat");
	if( m_pPatFile )
		m_pPatFile->SaveBin(strPath+"cadpat.dat");

	return TRUE;
}

void DeleteCADSymbolFile(LPCTSTR path)
{
	CString strPath = path;	

	::DeleteFile(strPath+"cadshx.dat");
	::DeleteFile(strPath+"cadlin.dat");
	::DeleteFile(strPath+"cadpat.dat");
}

CCADLinFile *CCADSymbolLib::GetLin()
{
	Create();
	return m_pLinFile;
}


CCADShxFile *CCADSymbolLib::GetShx()
{
	Create();
	return m_pShxFile;
}


CCADPatFile *CCADSymbolLib::GetPat()
{
	Create();
	return m_pPatFile;
}

MyNameSpaceEnd