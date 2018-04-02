// MilitaryVectorFile.cpp: implementation of the CMilitaryVectorFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MilitaryVectorFile.h"
#include "math.h"
#include "SmartViewFunctions.h"

#define FMT_CHAR		"字符型"
#define FMT_SHORT		"短整型"
#define FMT_LONG		"长整型"
#define FMT_FLOAT		"浮点型"
#define FMT_DOUBLE		"双精度"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


char *groupIDList[] = {
	"CLKZD","GNY","JMD","LDJT","GX","SYLD","HDDM","JS","SW","LDDM","JJ","ZB",
	"DCYS","ZHSB","HSQYJX","HKYS","ZJ","JSQY",
};

char *groupNameList[] = {
	"测量控制点","工农业社会文化","居民地","陆地交通运输","管线与桓栅","水域/陆地","海底地貌及底质",
	"礁石沉船障碍物","水文","陆地地貌及土质","境界与政区","植被",
	"地磁要素","助航设备及航道","海上区域界线","航空要素","注记","军事区域",
};


extern int ReadLineFromString(char *line, int *pos, char *ret);
extern int ReadStringFromString(char *line, int *pos, char *ret);



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMilitaryVectorFile::CMilitaryVectorFile()
{
	m_b2D = TRUE;
	memset(&m_fileHead,0,sizeof(m_fileHead));
}

CMilitaryVectorFile::~CMilitaryVectorFile()
{
	Clear();
}


BOOL CMilitaryVectorFile::Open(LPCTSTR path)
{
	CFileFind finder;
	
	// build a string with wildcards
	CString strWildcard(path);
	strWildcard += _T("\\*.Azb");
	
	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);
	
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		
		if (finder.IsDots())
			continue;
		
		// if it's a directory, recursively search it
		
		if (finder.IsDirectory())
			continue;

		break;
	}

	m_strMapName = finder.GetFileTitle();
	m_strPath = path;

	LoadFormats();

	{
		//读取文件头文件
		CString strFileName = m_strPath + "\\" + m_strMapName + ".AMS";
		FILE *fp = fopen(strFileName,"rt");
		if( fp )
		{
			fscanf(fp,"%s",m_fileHead.fileName);
			fscanf(fp,"%s",m_fileHead.secret);
			fscanf(fp,"%s",m_fileHead.level);
			fscanf(fp,"%s",m_fileHead.standard);
			fscanf(fp,"%s",m_fileHead.system);
			fclose(fp);
		}
	}

	//元数据
	ReadYSJ();

	const CVariantEx *p = NULL;
	if( m_metaData.m_mapValues.GetValue(0,"坐标维数",p) )
	{
		m_b2D = (((long)(_variant_t)*p)==2);
	}
	//区域描述数据
	ReadQYMSSJ();
	//要素层描述数据
	ReadYSCMSSJ();
	//测量控制点
	ReadCLKZD();
	//工农业社会文化设施
	ReadGNY();
	//居民地及附属设施
	ReadJMD();
	//陆地交通
	ReadLDJT();
	//管线
	ReadGX();
	//水域/陆地
	ReadSYLD();
	//海底地貌及底质
	ReadHDDM();
	//礁石、沉船、障碍物
	ReadJS();
	//水文
	ReadSW();
	//陆地地貌及土质
	ReadLDDM();
	//境界与政区
	ReadJJ();
	//植被
	ReadZB();
	//地磁要素
	ReadDC();
	//助航设备及航道
	ReadZHSB();
	//海上区域界线
	ReadHSQYJX();
	//航空要素
	ReadHKYS();
	//军事区域
	ReadJSQY();
	//读取注记
	ReadZJ();

	return TRUE;
}


void CMilitaryVectorFile::GetObjectCounts(CValueTable& tab, int *nPoint, int *nLine, int *nArea, int *nOther, int *nAnnot)
{
	int nObject = tab.GetItemCount();
	const CVariantEx *pVar;
	for( int i=0; i<nObject; i++)
	{
		if( !tab.GetValue(i,JBFIELD_GEOTYPE,pVar) )continue;
		int geotype = (long)(_variant_t)(*pVar);
		
		if( nPoint!=NULL && geotype==JBGEOTYPE_POINT )(*nPoint)++;
		else if( nLine!=NULL && geotype==JBGEOTYPE_LINE )(*nLine)++;
		else if( nArea!=NULL && geotype==JBGEOTYPE_AREA )(*nArea)++;
		else if( nOther!=NULL && geotype==JBGEOTYPE_COMPLICATED )(*nOther)++;
		else if( nAnnot!=NULL && geotype==JBGEOTYPE_ANNOT )(*nAnnot)++;
	}
}

BOOL CMilitaryVectorFile::WriteGroup(int type, LPCTSTR group)
{
	GroupItem *pData = NULL;
	
	CString strFileName; 
	FILE *fp;
	int i;

	for( i=0; i<m_arrGroups.GetSize(); i++)
	{
		pData = m_arrGroups.GetAt(i);
		if( pData->m_strID.CompareNoCase(group)==0 )
			break;
	}
	if( !pData)return FALSE;
	
	//写描述文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "MS";
	fp = fopen(strFileName,"wt");
	if( !fp )return FALSE;

	//--文件头
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "MS"));
	fprintf(fp," %s\n",m_fileHead.secret);
	fprintf(fp," %s\n",m_fileHead.level);
	fprintf(fp,"%s\n",m_fileHead.standard);
	fprintf(fp,"%s\n",m_fileHead.system);
	fprintf(fp,"%20s\n",(LPCTSTR)pData->m_strName);
	
	//--文件名
	fprintf(fp,"%8d\n",4);
	fprintf(fp,"%15s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "SX"));
	fprintf(fp,"%15s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "ZB"));
	fprintf(fp,"%15s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "MS"));
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "TP"));

	//--地物数目
	int nPoint = 0, nLine = 0, nArea = 0, nOther = 0;
	int nObject = pData->m_ObjData.GetItemCount();
	const CVariantEx *pVar;
	GetObjectCounts(pData->m_ObjData,&nPoint,&nLine,&nArea,&nOther,NULL);

	fprintf(fp,"%10d\n%10d\n%10d\n%10d\n",nPoint,nLine,nArea,nOther);
	
	//其他	
	fprintf(fp,"N\nN\nN\nN\n");
	fprintf(fp,"NULL\n-32767\nNULL\nNULL\nNULL\n0\nNULL\n");
	fclose(fp);	
	
	//写属性文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "SX";
	fp = fopen(strFileName,"wt");
	if( !fp )return FALSE;
	
	//--文件头
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "SX"));
	fprintf(fp," %s\n",m_fileHead.secret);
	fprintf(fp," %s\n",m_fileHead.level);
	fprintf(fp,"%c?????????????????\n",(char)type);
	fprintf(fp,"%s\n",m_fileHead.standard);
	fprintf(fp,"%s\n",m_fileHead.system);

	CJBFormatData *pFmt = GetFormat(CString(group)+"_SX");
	
	for( i=JBGEOTYPE_POINT; i<JBGEOTYPE_COMPLICATED; i++)
	{
		//--要素数目
		if( i==JBGEOTYPE_POINT )
			fprintf(fp,"P%10d\n",nPoint);
		else if( i==JBGEOTYPE_LINE )
			fprintf(fp,"L%10d\n",nLine);
		else if( i==JBGEOTYPE_AREA )
			fprintf(fp,"A%10d\n",nArea);
		else if( i==JBGEOTYPE_COMPLICATED )
			fprintf(fp,"F%10d\n",nOther);
		
		//--要素属性记录
		for( int j=0; j<nObject; j++)
		{
			if( !pData->m_ObjData.GetValue(j,JBFIELD_GEOTYPE,pVar) )continue;
			int geotype = (long)(_variant_t)(*pVar);
			if( i!=geotype )continue;

			CString text;
			if(pFmt) pFmt->SaveValues(j,pData->m_ObjData,text);

			fprintf(fp,"%s\n",(LPCTSTR)text);

		}
	}
	
	fclose(fp);
	
	//写坐标文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "ZB";
	fp = fopen(strFileName,"wt");
	if( !fp )return FALSE;

	//--文件头
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "ZB"));
	fprintf(fp," %s\n",m_fileHead.secret);
	fprintf(fp," %s\n",m_fileHead.level);
	fprintf(fp,"%c?????????????????\n",(char)type);
	fprintf(fp,"%s\n",m_fileHead.standard);
	fprintf(fp,"%s\n",m_fileHead.system);

	WriteCoordinates(fp,pData->m_ObjData);
	fclose(fp);

	//写拓扑数据
	//--文件头
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "TP";
	fp = fopen(strFileName,"wt");
	if( !fp )return FALSE;
	
	//--文件头
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "TP"));
	fprintf(fp," %s\n",m_fileHead.secret);
	fprintf(fp," %s\n",m_fileHead.level);
	fprintf(fp,"%c?????????????????\n",(char)type);
	fprintf(fp,"%s\n",m_fileHead.standard);
	fprintf(fp,"%s\n",m_fileHead.system);

	for( i=JBGEOTYPE_POINT; i<JBGEOTYPE_COMPLICATED; i++)
	{
		//--要素数目
		if( i==JBGEOTYPE_POINT )
			fprintf(fp,"P%10d\n",nPoint);
		else if( i==JBGEOTYPE_LINE )
			fprintf(fp,"L%10d\n",nLine);
		else if( i==JBGEOTYPE_AREA )
			fprintf(fp,"A%10d\n",nArea);
		else if( i==JBGEOTYPE_COMPLICATED )
			continue;
		
		//--要素拓扑记录
		for( int j=0; j<nObject; j++)
		{
			if( !pData->m_ObjData.GetValue(j,JBFIELD_GEOTYPE,pVar) )continue;
			int geotype = (long)(_variant_t)(*pVar);
			if( i!=geotype )continue;

			pData->m_ObjData.GetValue(j,JBFIELD_INDEX,pVar);
			int index = (long)(_variant_t)(*pVar);

			if( pData->m_arrTopoData.GetAt(j)==NULL )
			{
				fprintf(fp,"%10d 0\n",index);
			}
			else
			{
				TopoDataItem *pTopo = pData->m_arrTopoData.GetAt(j);

				if( pTopo->type==JBTOPO_NODE )
				{
					TopoPoint *pi = (TopoPoint*)pTopo;

					if( pi->arcList.GetSize()>0 )
					{
						fprintf(fp,"%10d 1 %10d\n",index,pi->arcList[0]);
					}
					else
					{
						fprintf(fp,"%10d %10d\n",index,0);
					}
				}
				else if( pTopo->type==JBTOPO_LINE )
				{
					TopoLine *pi = (TopoLine*)pTopo;
					fprintf(fp,"%10d %10d %10d %10d %10d\n",index,pi->nPt0,pi->nPt1,pi->nArea0,pi->nArea1);
				}
				else if( pTopo->type==JBTOPO_AREA )
				{
					TopoArea *pi = (TopoArea*)pTopo;
					if( pi->arcList.GetSize()>0 )
					{
						fprintf(fp,"%10d %10d %10d %10d\n",index,1,pi->arcList.GetAt(0),0);
					}
					else
					{
						fprintf(fp,"%10d %10d %10d\n",index,0,0);
					}
				}
			}
			
		}
	}
	
	fclose(fp);	
	
	return TRUE;
}


BOOL CMilitaryVectorFile::Save(LPCTSTR path, LPCTSTR mapName, BOOL b2D)
{
	m_strPath = path;
	m_strMapName = mapName;
	m_b2D = b2D;

	//元数据
	WriteYSJ();
	
	//区域描述数据
	WriteQYMSSJ();
	//要素层描述数据
	WriteYSCMSSJ();
	//测量控制点
	WriteCLKZD();
	//工农业社会文化设施
	WriteGNY();
	//居民地及附属设施
	WriteJMD();
	//陆地交通
	WriteLDJT();
	//管线
	WriteGX();
	//水域/陆地
	WriteSYLD();
	//海底地貌及底质
	WriteHDDM();
	//礁石、沉船、障碍物
	WriteJS();
	//水文
	WriteSW();
	//陆地地貌及土质
	WriteLDDM();
	//境界与政区
	WriteJJ();
	//植被
	WriteZB();
	//地磁要素
	WriteDC();
	//助航设备及航道
	WriteZHSB();
	//海上区域界线
	WriteHSQYJX();
	//航空要素
	WriteHKYS();
	//军事区域
	WriteJSQY();
	//读取注记
	WriteZJ();
	
	return TRUE;
}


void CMilitaryVectorFile::Clear()
{
	for( int i=0; i<m_arrGroups.GetSize(); i++)
	{
		GroupItem *pData = m_arrGroups.GetAt(i);
		for( int j=0; j<pData->m_arrTopoData.GetSize(); j++)
		{
			delete pData->m_arrTopoData.GetAt(j);
		}
		delete pData;
	}

	m_arrGroups.RemoveAll();

	for( i=0; i<m_arrPFormat.GetSize(); i++)
	{
		delete m_arrPFormat.GetAt(i);
	}
	m_arrPFormat.RemoveAll();

	m_metaData.Clear();
}


int  CMilitaryVectorFile::GetGroupCount()
{
	return sizeof(groupIDList)/sizeof(groupIDList[0]);
}


LPCTSTR CMilitaryVectorFile::GetGroupID(int idx)
{
	int nsz = sizeof(groupIDList)/sizeof(groupIDList[0]);
	if( idx>=0 && idx<nsz )
		return groupIDList[idx];

	return NULL;
}

// read
LPCTSTR CMilitaryVectorFile::GetNameOfGroup(LPCTSTR group)
{
	for( int i=0; i<m_arrGroups.GetSize(); i++)
	{
		if( m_arrGroups.GetAt(i)->m_strID.CompareNoCase(group)==0 )
		{
			return (LPCTSTR)m_arrGroups.GetAt(i)->m_strName;
		}
	}
	return NULL;
}


int  CMilitaryVectorFile::GetObjectNumOfGroup(LPCTSTR group)
{
	for( int i=0; i<m_arrGroups.GetSize(); i++)
	{
		if( m_arrGroups.GetAt(i)->m_strID.CompareNoCase(group)==0 )
		{
			return m_arrGroups.GetAt(i)->m_ObjData.GetItemCount();
		}
	}
	return 0;
}


BOOL CMilitaryVectorFile::GetObjectData(LPCTSTR group, int idx, CValueTable& tab)
{
	for( int i=0; i<m_arrGroups.GetSize(); i++)
	{
		GroupItem *pGp = m_arrGroups.GetAt(i);

		if( pGp->m_strID.CompareNoCase(group)==0 )
		{
			tab.DelAll();
			tab.AddItemFromTab(pGp->m_ObjData,idx);
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CMilitaryVectorFile::GetObjectTopoData(LPCTSTR group, int i)
{
	return TRUE;
}


BOOL CMilitaryVectorFile::CreateGroup(int type, LPCTSTR group)
{
	GroupItem *pData = new GroupItem;
	pData->m_nType = type;
	pData->m_strID = group;

	int nsz = sizeof(groupIDList)/sizeof(groupIDList[0]);
	for( int i=0; i<nsz; i++)
	{
		if( stricmp(group,groupIDList[i])==0 )
		{
			pData->m_strName = groupNameList[i];
			break;
		}
	}

	if( i<nsz )
	{
		m_arrGroups.Add(pData);
	}
	else
	{
		delete pData;
	}

	return TRUE;
}


void CMilitaryVectorFile::PrepareSave()
{
	LoadFormats();
	CreateDefaultGroups();
}

// write
BOOL CMilitaryVectorFile::CreateDefaultGroups()
{
	/*
	"CLKZD","GNY","JMD","LDJT","GX","SYLD","HDDM","JS","SW","LDDM","JJ","ZB",
		"DCYS","ZHSB","HSQYJX","HKYS","ZJ","JSQY",
	*/
	CreateGroup('A',"CLKZD");
	CreateGroup('B',"GNY");
	CreateGroup('C',"JMD");
	CreateGroup('D',"LDJT");
	CreateGroup('E',"GX");
	CreateGroup('F',"SYLD");
	CreateGroup('G',"HDDM");
	CreateGroup('H',"JS");
	CreateGroup('I',"SW");
	CreateGroup('J',"LDDM");
	CreateGroup('K',"JJ");
	CreateGroup('L',"ZB");
	CreateGroup('M',"DCYS");
	CreateGroup('N',"ZHSB");
	CreateGroup('O',"HSQYJX");
	CreateGroup('P',"HKYS");	
	CreateGroup('Q',"JSQY");
	CreateGroup('R',"ZJ");

	return TRUE;
}

int GIsClockwise3(PT_3DEX *pts, int num)
{
	if( num<=2 )return -1;
	
	if( fabs(pts[0].x-pts[num-1].x)<1e-10 && 
		fabs(pts[0].y-pts[num-1].y)<1e-10 )
		num--;
	
	if( num<=2 )return -1;
	
	double angs = 0, curang = 0, oldang = 0, delta;
	int i1,i2;
	for( int i=0; i<num; i++ )
	{
		i1 = (i+1)%num;
		i2 = (i+2)%num;
		if( fabs(pts[i1].x-pts[i2].x)<1e-10 && 
			fabs(pts[i1].y-pts[i2].y)<1e-10 )
			continue;
		
		oldang = curang;
		if( i==0 )oldang = GraphAPI::GGetAngle(pts[i].x,pts[i].y,pts[i1].x,pts[i1].y);
		curang = GraphAPI::GGetAngle(pts[i1].x,pts[i1].y,pts[i2].x,pts[i2].y);
		
		delta = curang-oldang;
		if( delta>PI )delta -= 2*PI;
		else if( delta<-PI )delta += 2*PI;
		else if( (delta>PI-1e-10&&delta<PI+1e-10) || (delta>-PI-1e-10&&delta<-PI+1e-10) )
			delta = 0;
		
		angs += delta;
	}
	
	if( angs>PI )return 0;
	else if( angs<-PI )return 1;
	return -1;
}

BOOL CMilitaryVectorFile::AddObject(LPCTSTR group, CValueTable& tab)
{
	if( group==NULL )
		return FALSE;
	
	for( int i=0; i<m_arrGroups.GetSize(); i++)
	{
		GroupItem *pGrp = m_arrGroups.GetAt(i);
		if( pGrp->m_strID.CompareNoCase(group)==0 )
		{
			CValueTable tab1, tab2;
			CreateDefaultObject(group,tab2);
			tab1.CopyFrom(tab2);

			//设置图形特征
			int geotype = 0;
			const CVariantEx *p = NULL;
			if( tab.GetValue(0,JBFIELD_GEOTYPE,p) )
			{
				geotype = (long)(_variant_t)*p;
			}
			
			int *pPointNum = &pGrp->m_nPoint;
			int *pLineNum = &pGrp->m_nLine;
			int *pAreaNum = &pGrp->m_nArea;
			int *pNum = NULL;

			if( geotype==JBGEOTYPE_POINT )
			{
				tab2.SetValue(0,JBFIELD_GRAPH,&CVariantEx((_variant_t)"PG"));
				pNum = pPointNum;
			}
			else if( geotype==JBGEOTYPE_LINE )
			{
				tab2.SetValue(0,JBFIELD_GRAPH,&CVariantEx((_variant_t)"LS"));
				pNum = pLineNum;
			}
			else if( geotype==JBGEOTYPE_AREA )
			{
				tab2.SetValue(0,JBFIELD_GRAPH,&CVariantEx((_variant_t)"AA"));
				pNum = pAreaNum;
			}
			else
				return FALSE;

			for( int j=0; j<tab2.GetFieldCount(); j++)
			{
				CString field,name;
				int type;
				tab2.GetField(j,field,type,name);

				if( tab.GetValue(0,field,p) )
				{
					tab2.SetValue(0,field,(CVariantEx*)p);
				}
			}

			//设置要素编号
			long index0 = pNum[0]+1;
			tab2.SetValue(0,JBFIELD_INDEX,&CVariantEx((_variant_t)index0));

			pGrp->m_ObjData.AddItemFromTab(tab2);
			pNum[0]++;

			//如果是面，需要将边界点作为弧段(线)存一份
			if( geotype==JBGEOTYPE_AREA )
			{
				//增加拓扑信息
				TopoArea *pTopoArea = new TopoArea;
				TopoLine *pTopoLine = new TopoLine;
				TopoPoint *pTopoPoint = new TopoPoint;

				//存弧段
				CValueTable tab3;
				tab3.CopyFrom(tab1);

				tab3.SetValue(0,JBFIELD_GRAPH,&CVariantEx((_variant_t)"LS"));
				tab3.SetValue(0,JBFIELD_GEOTYPE,&CVariantEx((_variant_t)(long)JBGEOTYPE_LINE));

				if( tab.GetValue(0,JBFIELD_SHAPE,p) )
				{
					tab3.SetValue(0,JBFIELD_SHAPE,(CVariantEx*)p);
				}

				long index = pLineNum[0]+1;
				tab3.SetValue(0,JBFIELD_INDEX,&CVariantEx((_variant_t)index));

				pGrp->m_ObjData.AddItemFromTab(tab3);
				pLineNum[0]++;

				pTopoArea->arcList.Add(index);
				pTopoPoint->arcList.Add(index);

				//存弧段的首尾节点
				CValueTable tab4;
				tab4.CopyFrom(tab1);
				
				tab4.SetValue(0,JBFIELD_GRAPH,&CVariantEx((_variant_t)"PN"));
				tab4.SetValue(0,JBFIELD_GEOTYPE,&CVariantEx((_variant_t)(long)JBGEOTYPE_POINT));

				index = pPointNum[0]+1;
				tab4.SetValue(0,JBFIELD_INDEX,&CVariantEx((_variant_t)index));

				int bclockwise = 0;
				if( tab.GetValue(0,JBFIELD_SHAPE,p) )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts, arrPts2;
					p->GetShape(arrPts);
					if( arrPts.GetSize()>0 )
					{
						bclockwise = GIsClockwise3(arrPts.GetData(),arrPts.GetSize());
						
						arrPts2.Add(arrPts.GetAt(0));
						CVariantEx varex;
						varex.SetAsShape(arrPts2);

						tab4.SetValue(0,JBFIELD_SHAPE,&varex);
					}
				}
				
				pGrp->m_ObjData.AddItemFromTab(tab4);
				pPointNum[0]++;

				pTopoLine->nPt0 = index; pTopoLine->nPt1 = index;

				if( bclockwise )
				{
					pTopoLine->nArea1 = index0;
					pTopoLine->nArea0 = 0;
				}
				else
				{
					pTopoLine->nArea0 = index0;
					pTopoLine->nArea1 = 0;
				}

				pGrp->m_arrTopoData.Add(pTopoArea);
				pGrp->m_arrTopoData.Add(pTopoLine);
				pGrp->m_arrTopoData.Add(pTopoPoint);
			}
			else if( geotype==JBGEOTYPE_LINE )
			{
				//增加拓扑信息
				TopoLine *pTopoLine = new TopoLine;
				TopoPoint *pTopoPoint0 = new TopoPoint;
				TopoPoint *pTopoPoint1 = new TopoPoint;
				
				//存弧段的首节点
				CValueTable tab4;
				tab4.CopyFrom(tab1);
				
				tab4.SetValue(0,JBFIELD_GRAPH,&CVariantEx((_variant_t)"PN"));
				tab4.SetValue(0,JBFIELD_GEOTYPE,&CVariantEx((_variant_t)(long)JBGEOTYPE_POINT));
				
				long index1 = pPointNum[0]+1;
				tab4.SetValue(0,JBFIELD_INDEX,&CVariantEx((_variant_t)index1));
				
				if( tab.GetValue(0,JBFIELD_SHAPE,p) )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts, arrPts2;
					p->GetShape(arrPts);
					if( arrPts.GetSize()>0 )
					{
						arrPts2.Add(arrPts.GetAt(0));
						CVariantEx varex;
						varex.SetAsShape(arrPts2);
						
						tab4.SetValue(0,JBFIELD_SHAPE,&varex);
					}
				}
				
				pGrp->m_ObjData.AddItemFromTab(tab4);
				pPointNum[0]++;

				//存弧段的尾节点

				long index2 = pPointNum[0]+1;
				tab4.SetValue(0,JBFIELD_INDEX,&CVariantEx((_variant_t)index2));
				if( tab.GetValue(0,JBFIELD_SHAPE,p) )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts, arrPts2;
					p->GetShape(arrPts);
					if( arrPts.GetSize()>0 )
					{
						arrPts2.Add(arrPts.GetAt(arrPts.GetSize()-1));
						CVariantEx varex;
						varex.SetAsShape(arrPts2);
						
						tab4.SetValue(0,JBFIELD_SHAPE,&varex);
					}
				}
				
				pGrp->m_ObjData.AddItemFromTab(tab4);
				pPointNum[0]++;
				
				pTopoLine->nPt0 = index1; pTopoLine->nPt1 = index2;
				
				pTopoPoint0->arcList.Add(index0);
				pTopoPoint1->arcList.Add(index0);
				
				pGrp->m_arrTopoData.Add(pTopoLine);
				pGrp->m_arrTopoData.Add(pTopoPoint0);
				pGrp->m_arrTopoData.Add(pTopoPoint1);
			}
			else
			{
				pGrp->m_arrTopoData.Add(NULL);
			}

			return TRUE;
		}
	}
	return FALSE;
}


BOOL CMilitaryVectorFile::LoadFormats()
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

	m_metaData.LoadDefines(text);
	
	while( 1 )
	{
		CJBFormatData *p = new CJBFormatData;
		if( p->LoadDefines(text) )
		{
			m_arrPFormat.Add(p);
		}
		else
		{
			delete p;
			break;
		}
	}

	return TRUE;
}


//元数据
BOOL CMilitaryVectorFile::ReadYSJ()
{
	CString strFileName = m_strPath + "\\" + m_strMapName + ".SMS";

	return m_metaData.LoadValues(strFileName);
}


BOOL CMilitaryVectorFile::WriteYSJ()
{
	CString strFileName = m_strPath + "\\" + m_strMapName + ".SMS";
	
	return m_metaData.SaveValues(strFileName);
}


//区域描述数据
BOOL CMilitaryVectorFile::ReadQYMSSJ()
{
	return TRUE;
}


BOOL CMilitaryVectorFile::WriteQYMSSJ()
{
	return TRUE;
}


//要素层描述数据
BOOL CMilitaryVectorFile::ReadYSCMSSJ()
{
	return TRUE;
}


BOOL CMilitaryVectorFile::WriteYSCMSSJ()
{
	return TRUE;
}


void CMilitaryVectorFile::ReadCoordinates(FILE *fp, CValueTable& tab)
{
	char word[256];
	int i, num, index = 0;
	PT_3DEX expt1, expt2;
	expt1.pencode = penLine;
	expt1.pencode = penLine;

	CArray<PT_3DEX,PT_3DEX> arrPts;

	while( !feof(fp) )
	{
		//--读取类型
		int geotype = 0, nobject = 0;
		
		fscanf(fp,"%s%d",word,&nobject);
		if( stricmp(word,"p")==0 )
			geotype = JBGEOTYPE_POINT;
		else if( stricmp(word,"l")==0 )
			geotype = JBGEOTYPE_LINE;
		else if( stricmp(word,"a")==0 )
			geotype = JBGEOTYPE_AREA;
		else 
			break;
		
		//--读取地物坐标		
		if( m_b2D )
		{
			if( geotype==JBGEOTYPE_POINT )
			{
				for( i=0; i<nobject; i++)
				{
					fscanf(fp,"%d%lf%lf%lf%lf",&num,&expt1.x,&expt1.y,&expt2.x,&expt2.y);

					arrPts.RemoveAll();

					expt1.z = expt2.z = 0;
					arrPts.Add(expt1);
					if( expt2.x!=0.0 )
						arrPts.Add(expt2);

					CVariantEx var;
					var.SetAsShape(arrPts);
					tab.SetValue(index++,JBFIELD_SHAPE,&var);
				}
			}
			else if( geotype==JBGEOTYPE_LINE )
			{
				for( i=0; i<nobject; i++)
				{
					int nPt = 0;
					fscanf(fp,"%d%d",&num,&nPt);

					arrPts.RemoveAll();

					for( int j=0; j<nPt; j++)
					{
						fscanf(fp,"%lf%lf",&expt1.x,&expt1.y);
						expt1.z = 0;

						arrPts.Add(expt1);
					}
					
					CVariantEx var;
					var.SetAsShape(arrPts);
					tab.SetValue(index++,JBFIELD_SHAPE,&var);
				}
			}
			else if( geotype==JBGEOTYPE_AREA )
			{
				for( i=0; i<nobject; i++)
				{
					arrPts.RemoveAll();

					int nArea = 0;
					fscanf(fp,"%d%lf%lf%d",&num,&expt1.x,&expt1.y,&nArea);

					expt1.z = 0;
					expt1.pencode = penLine;
					arrPts.Add(expt1);
					
					for( int j=0; j<nArea; j++)
					{
						int nPt = 0;
						fscanf(fp,"%d",&nPt);

						for( int k=0; k<nPt; k++)
						{
							fscanf(fp,"%lf%lf",&expt1.x,&expt1.y);
							expt1.z = 0;
							if( k==0 )expt1.pencode = penMove;
							else expt1.pencode = penLine;
							arrPts.Add(expt1);
						}
					}
					
					CVariantEx var;
					var.SetAsShape(arrPts);
					tab.SetValue(index++,JBFIELD_SHAPE,&var);
				}
			}
		}
		else
		{
			if( geotype==JBGEOTYPE_POINT )
			{
				for( i=0; i<nobject; i++)
				{
					fscanf(fp,"%d%lf%lf%lf%lf%lf%lf",&num,&expt1.x,&expt1.y,&expt1.z,&expt2.x,&expt2.y,&expt2.z);

					arrPts.RemoveAll();
					
					arrPts.Add(expt1);
					if( expt2.x!=0.0 )
						arrPts.Add(expt2);
					
					CVariantEx var;
					var.SetAsShape(arrPts);
					tab.SetValue(index++,JBFIELD_SHAPE,&var);
				}
			}
			else if( geotype==JBGEOTYPE_LINE )
			{
				for( i=0; i<nobject; i++)
				{
					arrPts.RemoveAll();

					int nPt = 0;
					fscanf(fp,"%d%d",&num,&nPt);
					
					for( int j=0; j<nPt; j++)
					{
						fscanf(fp,"%lf%lf%lf",&expt1.x,&expt1.y,&expt1.z);
						
						arrPts.Add(expt1);
					}
					
					CVariantEx var;
					var.SetAsShape(arrPts);
					tab.SetValue(index++,JBFIELD_SHAPE,&var);
				}
			}
			else if( geotype==JBGEOTYPE_AREA )
			{
				for( i=0; i<nobject; i++)
				{
					arrPts.RemoveAll();

					int nArea = 0;
					fscanf(fp,"%d%lf%lf%d",&num,&expt1.x,&expt1.y,&nArea);
					
					expt1.z = 0;
					expt1.pencode = penLine;
					arrPts.Add(expt1);
					
					for( int j=0; j<nArea; j++)
					{
						int nPt = 0;
						fscanf(fp,"%d",&nPt);
						
						for( int k=0; k<nPt; k++)
						{
							fscanf(fp,"%lf%lf%lf",&expt1.x,&expt1.y,&expt1.z);
							if( k==0 )expt1.pencode = penMove;
							else expt1.pencode = penLine;
							arrPts.Add(expt1);
						}
					}
					
					CVariantEx var;
					var.SetAsShape(arrPts);
					tab.SetValue(index++,JBFIELD_SHAPE,&var);
				}
			}
		}
	}
}


void CMilitaryVectorFile::ReadAnnotCoordinates(FILE *fp, CValueTable& tab)
{
	char word[256];
	int i, num;
	PT_3DEX expt1, expt2;
	expt1.pencode = penLine;
	expt1.pencode = penLine;

	CArray<PT_3DEX,PT_3DEX> arrPts;

	while( !feof(fp) )
	{
		//--读取类型
		int geotype = 0, nobject = 0;
		
		fscanf(fp,"%s%d",word,&nobject);
		
		//--读取地物坐标		
		if( m_b2D )
		{
			for( i=0; i<nobject; i++)
			{
				arrPts.RemoveAll();

				int nPt = 0;
				fscanf(fp,"%d%lf%lf%lf%lf%d",&num,&expt1.x,&expt1.y,&expt2.x,&expt2.y,&nPt);

				expt1.z = expt2.z = 0;
				expt1.pencode = expt2.pencode = penLine;

				arrPts.Add(expt1);
				arrPts.Add(expt2);

				for( int j=0; j<nPt; j++)
				{
					fscanf(fp,"%lf%lf",&expt1.x,&expt1.y);
					expt1.z = 0;
					expt1.pencode = penLine;

					arrPts.Add(expt1);
				}
				
				CVariantEx var;
				var.SetAsShape(arrPts);
				tab.SetValue(i,JBFIELD_SHAPE,&var);
			}
		}
		else
		{
			for( i=0; i<nobject; i++)
			{
				arrPts.RemoveAll();

				int nPt = 0;
				fscanf(fp,"%d%lf%lf%lf%lf%d",&num,&expt1.x,&expt1.y,&expt2.x,&expt2.y,&nPt);
				
				expt1.z = expt2.z = 0;
				expt1.pencode = expt2.pencode = penLine;
				
				arrPts.Add(expt1);
				arrPts.Add(expt2);
				
				for( int j=0; j<nPt; j++)
				{
					fscanf(fp,"%lf%lf%lf",&expt1.x,&expt1.y,&expt1.z);
					expt1.pencode = penLine;
					
					arrPts.Add(expt1);
				}
				
				CVariantEx var;
				var.SetAsShape(arrPts);
				tab.SetValue(i,JBFIELD_SHAPE,&var);
			}
		}
	}
}


void CMilitaryVectorFile::WriteCoordinates(FILE *fp, CValueTable& tab)
{
	int nPoint = 0, nLine = 0, nArea = 0, nOther = 0, nAnnot = 0;
	int nObject = tab.GetItemCount(), index = 0;
	const CVariantEx *pVar;
	GetObjectCounts(tab,&nPoint,&nLine,&nArea,&nOther,&nAnnot);

	CArray<PT_3DEX,PT_3DEX> arrPts;

	//点状要素坐标
	fprintf(fp,"P%10d\n",nPoint);
	for( int i=0; i<nObject; i++)
	{
		if( !tab.GetValue(i,JBFIELD_GEOTYPE,pVar) )continue;
		int geotype = (long)(_variant_t)(*pVar);
		if( JBGEOTYPE_POINT!=geotype )continue;

		if( !tab.GetValue(i,JBFIELD_INDEX,pVar) )continue;
		index = (long)(_variant_t)(*pVar);

		if( 1/*m_b2D*/ )
		{
			tab.GetValue(i,JBFIELD_SHAPE,pVar);
			pVar->GetShape(arrPts);
			if( arrPts.GetSize()<=0 )continue;

			if( arrPts.GetSize()==1 )
			{
				if( m_b2D )
					fprintf(fp,"%10d %15.6f %15.6f %15.6f %15.6f\n",index,arrPts[0].x,arrPts[0].y,0,0);
				else
					fprintf(fp,"%10d %15.6f %15.6f %8.2f %15.6f %15.6f %8.2f\n",index,arrPts[0].x,arrPts[0].y,arrPts[0].z,0,0,0);
			}
			else
			{
				if( m_b2D )
					fprintf(fp,"%10d %15.6f %15.6f %15.6f %15.6f\n",index,arrPts[0].x,arrPts[0].y,arrPts[1].x,arrPts[1].y);
				else
					fprintf(fp,"%10d %15.6f %15.6f %8.2f %15.6f %15.6f %8.2f\n",index,arrPts[0].x,arrPts[0].y,arrPts[0].z,arrPts[1].x,arrPts[1].y,arrPts[1].z);
			}
		}
	}

	index = 0;
	//线状要素坐标
	fprintf(fp,"L%10d\n",nLine);
	for( i=0; i<nObject; i++)
	{
		if( !tab.GetValue(i,JBFIELD_GEOTYPE,pVar) )continue;
		int geotype = (long)(_variant_t)(*pVar);
		if( JBGEOTYPE_LINE!=geotype )continue;

		if( !tab.GetValue(i,JBFIELD_INDEX,pVar) )continue;
		index = (long)(_variant_t)(*pVar);
		
		if( 1/*m_b2D*/ )
		{
			tab.GetValue(i,JBFIELD_SHAPE,pVar);
			pVar->GetShape(arrPts);
			int nPt = arrPts.GetSize();

			if( nPt<=1 )continue;
			
			fprintf(fp,"%10d%10d\n",index,nPt);
			for( int j=0; j<arrPts.GetSize(); j++)
			{
				if( m_b2D )
					fprintf(fp,"%15.6f %15.6f ",arrPts[j].x,arrPts[j].y);
				else
					fprintf(fp,"%15.6f %15.6f %8.2f ",arrPts[j].x,arrPts[j].y,arrPts[j].z);

				if( ((j+1)%6)==0 )
					fprintf(fp,"\n");
			}
			if( (j%6)!=0 )fprintf(fp,"\n");
		}
	}

	index = 0;
	//面状要素坐标
	fprintf(fp,"A%10d\n",nArea);
	for( i=0; i<nObject; i++)
	{
		if( !tab.GetValue(i,JBFIELD_GEOTYPE,pVar) )continue;
		int geotype = (long)(_variant_t)(*pVar);
		if( JBGEOTYPE_AREA!=geotype )continue;

		if( !tab.GetValue(i,JBFIELD_INDEX,pVar) )continue;
		index = (long)(_variant_t)(*pVar);
		
		if( 1/*m_b2D*/ )
		{
			tab.GetValue(i,JBFIELD_SHAPE,pVar);
			pVar->GetShape(arrPts);
			int nPt = arrPts.GetSize();
			
			if( nPt<=1 )continue;
			PT_3DEX *pts = arrPts.GetData();

			int nAreas = 1, nAreaPt = 1; 
			PT_3DEX cpt = pts[0];
			CArray<int,int> arrNums;

			//计算子面的数目和坐标点数
			for( int j=2; j<nPt; j++)
			{
				if( pts[j].pencode==penMove )
				{
					nAreas++;
					nAreaPt++;
					arrNums.Add(nAreaPt);
					nAreaPt = 0;
				}
				else
				{
					nAreaPt++;
				}
			}
			arrNums.Add(nAreaPt);
			
			int t = 1;
			//输出各个面
			fprintf(fp,"%10d %15.6f %15.6f %10d\n",index,cpt.x,cpt.y,nAreas);
			for( j=0; j<nAreas; j++)
			{
				fprintf(fp,"%10d\n",arrNums[j]);
				
				for( int k=0; k<arrNums[j]; k++)
				{
					if( m_b2D )
						fprintf(fp,"%15.6f %15.6f ",pts[t].x,pts[t].y);
					else
						fprintf(fp,"%15.6f %15.6f %8.2f ",pts[t].x,pts[t].y,pts[t].z);

					t++;
					if( ((k+1)%6)==0 )
						fprintf(fp,"\n");
				}
				if( (k%6)!=0 )fprintf(fp,"\n");
			}
		}
	}

	fclose(fp);
}


void CMilitaryVectorFile::WriteAnnotCoordinates(FILE *fp, CValueTable& tab)
{
	int nPoint = 0, nLine = 0, nArea = 0, nOther = 0, nAnnot = 0;
	int nObject = tab.GetItemCount(), index = 0;
	const CVariantEx *pVar;
	GetObjectCounts(tab,&nPoint,&nLine,&nArea,&nOther,&nAnnot);

	CArray<PT_3DEX,PT_3DEX> arrPts;

	//点状要素坐标
	fprintf(fp,"N%10d\n",nAnnot);
	for( int i=0; i<nObject; i++)
	{
		if( !tab.GetValue(i,JBFIELD_GEOTYPE,pVar) )continue;
		int geotype = (long)(_variant_t)(*pVar);
		if( JBGEOTYPE_ANNOT!=geotype )continue;

		if( !tab.GetValue(i,"编号",pVar) )continue;
		index = (long)(_variant_t)(*pVar);

		if( m_b2D )
		{
			tab.GetValue(i,JBFIELD_SHAPE,pVar);
			pVar->GetShape(arrPts);
			if( arrPts.GetSize()<=0 )continue;

			int nPt = arrPts.GetSize();
			PT_3DEX *pts = arrPts.GetData();

			if( nPt==1 )
			{
				if( m_b2D )
					fprintf(fp,"%10d %15.6f %15.6f %15.6f %15.6f %8d %15.6f %15.6f\n",
					index,pts[0].x,pts[0].y,0,0,1,pts[0].x,pts[0].y);
				else
					fprintf(fp,"%10d %15.6f %15.6f %15.6f %15.6f %8d %15.6f %15.6f %8.2f\n",
					index,pts[0].x,pts[0].y,0,0,1,pts[0].x,pts[0].y,pts[0].z);
			}
			else if( nPt>1 )
			{
				if( m_b2D )
				{
					fprintf(fp,"%10d%15.6f%15.6f%15.6f%15.6f%8d%15.6f%15.6f ",
						index,pts[0].x,pts[0].y,0,0,nPt,pts[0].x,pts[0].y);
				}
				else
				{
					fprintf(fp,"%10d %15.6f %15.6f %15.6f %15.6f %8d %15.6f %15.6f %8.2f ",
						index,pts[0].x,pts[0].y,0,0,nPt,pts[0].x,pts[0].y,pts[0].z);
				}

				for( int j=1; j<nPt; j++)
				{
					if( m_b2D )
						fprintf(fp,"%15.6f %15.6f ",pts[j].x,pts[j].y);
					else
						fprintf(fp,"%15.6f %15.6f %8.2f ",pts[j].x,pts[j].y,pts[j].z);
				}
				fprintf(fp,"\n");
			}
		}
	}
}




void CMilitaryVectorFile::CreateDefaultObject(LPCTSTR group, CValueTable& tab)
{
	CString fmt = group;
	fmt += "_SX";

	CJBFormatData *p = GetFormat(fmt);
	if( !p )return;

	tab.BeginAddValueItem();

	_variant_t var;
	var = (long)0;
	tab.AddValue(JBFIELD_SHAPE,&CVariantEx(var),(LPCTSTR)NULL);
	tab.AddValue(JBFIELD_GEOTYPE,&CVariantEx(var),(LPCTSTR)NULL);

	for( int i=0; i<p->m_FmtItems.GetSize(); i++)
	{
		JBFormatDefineItem item = p->m_FmtItems.GetAt(i);
		item.CreateDefaultValue(var);

		tab.AddValue(p->m_FmtItems.GetAt(i).name,&CVariantEx(var),(LPCTSTR)NULL);
	}

	tab.EndAddValueItem();

	return;
}

CJBFormatData *CMilitaryVectorFile::GetFormat(LPCTSTR name)
{
	for( int i=0; i<m_arrPFormat.GetSize(); i++)
	{
		if( m_arrPFormat.GetAt(i)->m_strFmtName.CompareNoCase(name)==0 )
		{
			return m_arrPFormat.GetAt(i);
		}
	}
	return NULL;
}


struct ReadGroupIndexStruct
{
	int type;
	int index;
	int isdel;
};

void SetToDel(CArray<ReadGroupIndexStruct,ReadGroupIndexStruct>& arr, int type, int index, int isdel)
{
	ReadGroupIndexStruct *buf = arr.GetData();
	int nSize = arr.GetSize();
	for( int i=0; i<nSize; i++)
	{
		if( buf[i].type==type && buf[i].index==index )
		{
			buf[i].isdel = isdel;
			break;
		}
	}
}


BOOL CMilitaryVectorFile::ReadGroup(int type, LPCTSTR group)
{
	GroupItem *pData;
	
	CString strFileName; 
	FILE *fp;
	char line[1024], word[256];
	int i, pos;
	
	//读取描述文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "MS";
	fp = fopen(strFileName,"rt");
	if( !fp )return FALSE;
	
	//--跳过文件头记录
	for(i=0;i<5;i++)fgets(line,sizeof(line),fp);
	fgets(line,sizeof(line),fp);
	
	//--读取层名
	pos = 0;
	ReadStringFromString(line,&pos,word);
	
	pData = new GroupItem;
	pData->m_nType = type;
	pData->m_strID = group;
	pData->m_strName = word;
	fclose(fp);
	
	//读取属性文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "SX";
	fp = fopen(strFileName,"rt");
	if( !fp )return FALSE;
	
	//--跳过文件头记录
	for(i=0;i<6;i++)fgets(line,sizeof(line),fp);
	
	while( !feof(fp) )
	{
		//--读取类型
		int geotype = 0, nobject = 0;
		
		fscanf(fp,"%s%d",word,&nobject);
		
		if( stricmp(word,"p")==0 )
			geotype = JBGEOTYPE_POINT;
		else if( stricmp(word,"l")==0 )
			geotype = JBGEOTYPE_LINE;
		else if( stricmp(word,"a")==0 )
			geotype = JBGEOTYPE_AREA;
		else if( stricmp(word,"f")==0 )
			geotype = JBGEOTYPE_COMPLICATED;
		else
			break;

		fgets(line,sizeof(line),fp);

		CJBFormatData *pFmt = GetFormat(CString(group)+"_SX");
		
		//--读取地物属性
		for( i=0; i<nobject; i++)
		{
			fgets(line,sizeof(line),fp);
			pos = 0;
			ReadStringFromString(line,&pos,word);
			if( strlen(word)<=0 )continue;

			int index = pData->m_ObjData.GetItemCount();
			CreateDefaultObject(group,pData->m_ObjData);
			pData->m_ObjData.SetValue(index,JBFIELD_GEOTYPE,&CVariantEx((_variant_t)(long)geotype));
			if( pFmt )
			{
				pFmt->LoadValues(index,CString(line),pData->m_ObjData);
			}
		}
	}

	fclose(fp);
	
	//读取坐标文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "ZB";
	fp = fopen(strFileName,"rt");
	if( fp )
	{
		//--跳过文件头记录
		for(i=0;i<6;i++)fgets(line,sizeof(line),fp);
		
		ReadCoordinates(fp,pData->m_ObjData);
		fclose(fp);
	}

	//清理因为存在拓扑数据导致的重复数据
	int nSize = pData->m_ObjData.GetItemCount();

	CArray<ReadGroupIndexStruct,ReadGroupIndexStruct> arrIndexs;
	arrIndexs.SetSize(nSize);
	ReadGroupIndexStruct item;
	const CVariantEx *p;

	int nPointIdx = 0, nLineIdx = 0, nAreaIdx = 0;

	for( i=0; i<nSize; i++ )
	{
		pData->m_ObjData.GetValue(i,JBFIELD_GEOTYPE,p);
		item.type = (long)(_variant_t)*p;

		pData->m_ObjData.GetValue(i,JBFIELD_INDEX,p);
		item.index = (long)(_variant_t)*p;
		item.isdel = 0;

		if( item.type==JBGEOTYPE_POINT )
		{
			if( pData->m_ObjData.GetValue(i,JBFIELD_GRAPH,p) )
			{
				if( stricmp((LPCTSTR)(_bstr_t)(_variant_t)*p,"PN")==0 )
				{
					item.isdel = 1;
				}
			}
		}

		arrIndexs.SetAt(i,item);
	}

	//读取拓扑信息
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "TP";
	fp = fopen(strFileName,"rt");
	if( fp )
	{
		//--跳过文件头记录
		for(i=0;i<6;i++)fgets(line,sizeof(line),fp);
		int nIndex = 0, v1, v2, v3, v4, v5;
		
		//--读取类型
		int geotype = 0, nobject = 0;

		while( !feof(fp) )
		{
		
			fscanf(fp,"%s%d",word,&nobject);
			
			if( stricmp(word,"p")==0 )
				geotype = JBGEOTYPE_POINT;
			else if( stricmp(word,"l")==0 )
				geotype = JBGEOTYPE_LINE;
			else if( stricmp(word,"a")==0 )
				geotype = JBGEOTYPE_AREA;
			else if( stricmp(word,"f")==0 )
				geotype = JBGEOTYPE_COMPLICATED;
			else
				break;
			
			fgets(line,sizeof(line),fp);
			
			//--读取地物属性
			for( i=0; i<nobject; i++)
			{
				fgets(line,sizeof(line),fp);
				pos = 0;
				if( geotype==JBGEOTYPE_POINT )
				{
					if( sscanf(line,"%d%d",&v1,&v2)==2 )
					{
						if( v2!=0 )
						{
							SetToDel(arrIndexs,JBGEOTYPE_POINT,v1,1);
						}
					}
				}
				else if( geotype==JBGEOTYPE_LINE )
				{
					if( sscanf(line,"%d%d%d%d",&v1,&v2,&v3,&v4,&v5)==5 )
					{
						if( v2!=0 )
							SetToDel(arrIndexs,JBGEOTYPE_POINT,v2,1);

						if( v3!=0 )
							SetToDel(arrIndexs,JBGEOTYPE_POINT,v3,1);

						if( v4!=0 )
							SetToDel(arrIndexs,JBGEOTYPE_AREA,v4,1);

						if( v5!=0 )
							SetToDel(arrIndexs,JBGEOTYPE_AREA,v5,1);
					}
				}
			}
		}

		fclose(fp);
	}

	for( i=nSize-1; i>=0; i-- )
	{
		if( arrIndexs[i].isdel )
		{
			pData->m_ObjData.DelValueItem(i);
		}
	}
	
	m_arrGroups.Add(pData);
	
	return TRUE;
}

//测量控制点
BOOL CMilitaryVectorFile::ReadCLKZD()
{
	return ReadGroup('A',"CLKZD");
}


BOOL CMilitaryVectorFile::WriteCLKZD()
{
	return WriteGroup('A',"CLKZD");
}


//工农业社会文化设施
BOOL CMilitaryVectorFile::ReadGNY()
{
	return ReadGroup('B',"GNY");
}


BOOL CMilitaryVectorFile::WriteGNY()
{
	return WriteGroup('B',"GNY");
}


//居民地及附属设施
BOOL CMilitaryVectorFile::ReadJMD()
{
	return ReadGroup('C',"JMD");
}


BOOL CMilitaryVectorFile::WriteJMD()
{
	return WriteGroup('C',"JMD");
}


//陆地交通
BOOL CMilitaryVectorFile::ReadLDJT()
{
	return ReadGroup('D',"LDJT");
}


BOOL CMilitaryVectorFile::WriteLDJT()
{
	return WriteGroup('D',"LDJT");
}


//管线
BOOL CMilitaryVectorFile::ReadGX()
{
	return ReadGroup('E',"GX");
}


BOOL CMilitaryVectorFile::WriteGX()
{
	return WriteGroup('E',"GX");
}


//水域/陆地
BOOL CMilitaryVectorFile::ReadSYLD()
{
	return ReadGroup('F',"SYLD");
}


BOOL CMilitaryVectorFile::WriteSYLD()
{
	return WriteGroup('F',"SYLD");
}


//海底地貌及底质
BOOL CMilitaryVectorFile::ReadHDDM()
{
	return ReadGroup('G',"HDDM");
}


BOOL CMilitaryVectorFile::WriteHDDM()
{
	return WriteGroup('G',"HDDM");
}


//礁石、沉船、障碍物
BOOL CMilitaryVectorFile::ReadJS()
{
	return ReadGroup('H',"JS");
}


BOOL CMilitaryVectorFile::WriteJS()
{
	return WriteGroup('H',"JS");
}


//水文
BOOL CMilitaryVectorFile::ReadSW()
{
	return ReadGroup('I',"SW");
}


BOOL CMilitaryVectorFile::WriteSW()
{
	return WriteGroup('I',"SW");
}


//陆地地貌及土质
BOOL CMilitaryVectorFile::ReadLDDM()
{
	return ReadGroup('J',"LDDM");
}


BOOL CMilitaryVectorFile::WriteLDDM()
{
	return WriteGroup('J',"LDDM");
}


//境界与政区
BOOL CMilitaryVectorFile::ReadJJ()
{
	return ReadGroup('K',"JJ");
}


BOOL CMilitaryVectorFile::WriteJJ()
{
	return WriteGroup('K',"JJ");
}


//植被
BOOL CMilitaryVectorFile::ReadZB()
{
	return ReadGroup('L',"ZB");
}


BOOL CMilitaryVectorFile::WriteZB()
{
	return WriteGroup('L',"ZB");
}


//地磁要素
BOOL CMilitaryVectorFile::ReadDC()
{
	return ReadGroup('M',"DC");
}


BOOL CMilitaryVectorFile::WriteDC()
{
	return WriteGroup('M',"DC");
}


//助航设备及航道
BOOL CMilitaryVectorFile::ReadZHSB()
{
	return ReadGroup('N',"ZHSB");
}


BOOL CMilitaryVectorFile::WriteZHSB()
{
	return WriteGroup('N',"ZHSB");
}


//海上区域界线
BOOL CMilitaryVectorFile::ReadHSQYJX()
{
	return ReadGroup('O',"HSQYJX");
}


BOOL CMilitaryVectorFile::WriteHSQYJX()
{
	return WriteGroup('O',"HSQYJX");
}


//航空要素
BOOL CMilitaryVectorFile::ReadHKYS()
{
	return ReadGroup('P',"HKYS");
}


BOOL CMilitaryVectorFile::WriteHKYS()
{
	return WriteGroup('P',"HKYS");
}


//军事区域
BOOL CMilitaryVectorFile::ReadJSQY()
{
	return ReadGroup('Q',"JSQY");
}


BOOL CMilitaryVectorFile::WriteJSQY()
{
	return WriteGroup('Q',"JSQY");
}


BOOL CMilitaryVectorFile::ReadZJ()
{
	GroupItem *pData;
	
	CString strFileName; 
	FILE *fp;
	char line[1024], word[256], type = 'R';
	int i, pos;
	CString group = "ZJ";
	
	//读取描述文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + ".MS";
	fp = fopen(strFileName,"rt");
	if( !fp )return FALSE;
	
	//--跳过文件头记录
	for(i=0;i<5;i++)fgets(line,sizeof(line),fp);
	fgets(line,sizeof(line),fp);
	
	//--读取层名
	pos = 0;
	ReadStringFromString(line,&pos,word);
	
	pData = new GroupItem;
	pData->m_strID = group;
	pData->m_strName = word;
	fclose(fp);
		
	//读取属性文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "SX";
	fp = fopen(strFileName,"rt");
	if( !fp )return FALSE;
	
	//--跳过文件头记录
	for(i=0;i<6;i++)fgets(line,sizeof(line),fp);
	
	while( !feof(fp) )
	{
		//--读取类型
		int geotype = 0, nobject = 0;
		
		fscanf(fp,"%s%d",word,&nobject);

		CJBFormatData *pFmt = GetFormat(CString(group)+"_SX");
		
		//--读取地物属性
		for( i=0; i<nobject; i++)
		{
			fgets(line,sizeof(line),fp);
			CreateDefaultObject(group,pData->m_ObjData);
			if( pFmt )
			{
				pFmt->LoadValues(i,CString(line),pData->m_ObjData);
			}
		}
	}
	
	//读取坐标文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "ZB";
	fp = fopen(strFileName,"rt");
	if( fp )
	{
		//--跳过文件头记录
		for(i=0;i<6;i++)fgets(line,sizeof(line),fp);
		
		ReadAnnotCoordinates(fp,pData->m_ObjData);
		fclose(fp);
	}
	
	m_arrGroups.Add(pData);
	
	return TRUE;
}


BOOL CMilitaryVectorFile::WriteZJ()
{
	GroupItem *pData = NULL;
	
	CString strFileName; 
	FILE *fp;
	char type = 'R';
	int i;

	CString group = "ZJ";

	for( i=0; i<m_arrGroups.GetSize(); i++)
	{
		pData = m_arrGroups.GetAt(i);
		if( pData->m_strID.CompareNoCase(group)==0 )
			break;
	}
	if( !pData)return FALSE;
	
	//写描述文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "MS";
	fp = fopen(strFileName,"wt");
	if( !fp )return FALSE;

	//--文件头
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "MS"));
	fprintf(fp," %s\n",m_fileHead.secret);
	fprintf(fp," %s\n",m_fileHead.level);
	fprintf(fp,"%s\n",m_fileHead.standard);
	fprintf(fp,"%s\n",m_fileHead.system);
	fprintf(fp,"%s\n",(LPCTSTR)pData->m_strName);
	
	//--文件名
	fprintf(fp,"3\n");
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "SX"));
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "ZB"));
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "MS"));
	//fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "TP"));

	//--地物数目
	int nPoint = 0, nLine = 0, nArea = 0, nOther = 0, nAnnot = 0;
	int nObject = pData->m_ObjData.GetItemCount();
	const CVariantEx *pVar;
	GetObjectCounts(pData->m_ObjData,&nPoint,&nLine,&nArea,&nOther,&nAnnot);

	fprintf(fp,"%d\n%d\n%d\n%d\n",nPoint,nLine,nArea,nOther);
	
	//其他	
	fprintf(fp,"N\nN\nN\nN\n");
	fprintf(fp,"NULL\n-32767\nNULL\nNULL\nNULL\n0\nNULL\n");
	fclose(fp);	
	
	//写属性文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "SX";
	fp = fopen(strFileName,"wt");
	if( !fp )return FALSE;
	
	//--文件头
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "SX"));
	fprintf(fp," %s\n",m_fileHead.secret);
	fprintf(fp," %s\n",m_fileHead.level);
	fprintf(fp,"%c?????????????????\n",(char)type);
	fprintf(fp,"%s\n",m_fileHead.standard);
	fprintf(fp,"%s\n",m_fileHead.system);

	CJBFormatData *pFmt = GetFormat(CString(group)+"_SX");

	//--要素数目
	fprintf(fp,"N%10d\n",nAnnot);
	
	//--要素属性记录
	for( int j=0; j<nObject; j++)
	{
		if( !pData->m_ObjData.GetValue(j,JBFIELD_GEOTYPE,pVar) )continue;
		int geotype = (long)(_variant_t)(*pVar);
		if( JBGEOTYPE_ANNOT!=geotype )continue;

		CString text;
		if(pFmt) pFmt->SaveValues(j,pData->m_ObjData,text);

		fprintf(fp,"%s\n",(LPCTSTR)text);

	}
	
	fclose(fp);
	
	//写坐标文件
	strFileName = m_strPath + "\\" + m_strMapName + "." + (char)type + "ZB";
	fp = fopen(strFileName,"wt");
	if( !fp )return FALSE;

	//--文件头
	fprintf(fp,"%s\n",(LPCTSTR)CString(m_strMapName + "." + (char)type + "ZB"));
	fprintf(fp," %s\n",m_fileHead.secret);
	fprintf(fp," %s\n",m_fileHead.level);
	fprintf(fp,"%c?????????????????\n",(char)type);
	fprintf(fp,"%s\n",m_fileHead.standard);
	fprintf(fp,"%s\n",m_fileHead.system);

	WriteAnnotCoordinates(fp,pData->m_ObjData);
	fclose(fp);

	return TRUE;
}

