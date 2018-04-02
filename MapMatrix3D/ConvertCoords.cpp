// onvertCoords.cpp: implementation of the ConvertCoords class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DlgDataSource.h"
#include "EditBase.h"
#include "ConvertCoords.h"
#include "CadLib.h"
#include "SQLiteAccess.h"
#include "DxfAccess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


class CConvertDXFAssist
{
public:
	CConvertDXFAssist(){
		m_nConvertType = 0;
		m_tm1 = m_tm2 = NULL;
	}
	~CConvertDXFAssist(){}

	void SetParams(double *m)
	{
		m_nConvertType = 0;
		memcpy(m_matrix,m,sizeof(m_matrix));
	}

	void SetParams(CTM *tm1, CTM *tm2)
	{
		m_nConvertType = 1;
		m_tm1 = tm1;
		m_tm2 = tm2;
	}

	bool ChangeData(ENTITYHEADER &EntityHeader, LPVOID pEntityData);
	void ChangePoint(REALPOINT &pt);

	int m_nConvertType;

	double m_matrix[16];
	CTM *m_tm1, *m_tm2;
};


class CConvertDWGAssist
{
public:
	CConvertDWGAssist(){
		m_nConvertType = 0;
		m_tm1 = m_tm2 = NULL;
	}
	~CConvertDWGAssist(){
	}
	
	void SetParams(double *m)
	{
		m_nConvertType = 0;
		memcpy(m_matrix,m,sizeof(m_matrix));

		char path[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, _MAX_FNAME);
		char *pos;
		if ((pos = strrchr(path, '\\')))*pos = '\0';
		if ((pos = strrchr(path, '\\')))*pos = '\0';
		strcat(pos, "\\History\\dwgparam.txt");

		FILE *fp = fopen(path, "wb");
		if (fp)
		{
			fwrite((const char*)m_matrix, sizeof(m_matrix), 1, fp);
			fclose(fp);
		}
	}
	
	void SetParams(CTM *tm1, CTM *tm2)
	{
		m_nConvertType = 1;
		m_tm1 = tm1;
		m_tm2 = tm2;

		char path[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, _MAX_FNAME);
		char *pos;
		if ((pos = strrchr(path, '\\')))*pos = '\0';
		if ((pos = strrchr(path, '\\')))*pos = '\0';
		strcat(pos, "\\History\\dwgparam.txt");

		FILE *fp = fopen(path, "wb");
		if (fp)
		{
			int nsize = sizeof(CTM);
			fwrite((const char*)m_tm1, nsize, 1, fp);
			fwrite((const char*)m_tm2, nsize, 1, fp);
			fclose(fp);
		}
	}
	
	BOOL Convert(CString dwgPath)
	{
		char path[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, _MAX_FNAME);
		char *pos;
		if ((pos = strrchr(path, '\\')))*pos = '\0';
		if ((pos = strrchr(path, '\\')))*pos = '\0';
		strcat(path, "\\bin\\VectorConvert.exe");

		CString cmd;
		cmd.Format("\"%s\" ConvertDWG \"%s\"", path, dwgPath);
		WinExec(cmd, SW_SHOW);

		return TRUE;
	}

protected:
	int m_nConvertType;
	double m_matrix[16];
	CTM *m_tm1, *m_tm2;
};

bool CConvertDXFAssist::ChangeData(ENTITYHEADER &EntityHeader, LPVOID pEntityData)
{
	unsigned short type = EntityHeader.EntityType;

	if (type == ENT_LINE)
	{
		PENTLINE pl = (PENTLINE)pEntityData;

		ChangePoint(pl->Point0);
		ChangePoint(pl->Point1);
	}
	else if (type == ENT_ARC)
	{
		PENTARC pl = (PENTARC)pEntityData;

		ChangePoint(pl->Point0);
	}
	else if (type == ENT_CIRCLE)
	{
		PENTCIRCLE pl = (PENTCIRCLE)pEntityData;

		ChangePoint(pl->Point0);
	}
	else if (type == ENT_DIMENSION)
	{
		PENTDIMENSION pl = (PENTDIMENSION)pEntityData;

		ChangePoint(pl->DefPoint3);
		ChangePoint(pl->DefPoint4);
		ChangePoint(pl->DimLineDefPoint);
	}
	else if (type == ENT_INSERT)
	{
		PENTINSERT pl = (PENTINSERT)pEntityData;

		ChangePoint(pl->Point0);
	}
	else if (type == ENT_POINT)
	{
		PENTPOINT pl = (PENTPOINT)pEntityData;

		ChangePoint(pl->Point0);
	}
	else if (type == ENT_SOLID)
	{
		PENTSOLID pl = (PENTSOLID)pEntityData;

		ChangePoint(pl->Point0);
		ChangePoint(pl->Point1);
		ChangePoint(pl->Point2);
		ChangePoint(pl->Point3);
	}
	else if (type == ENT_TEXT)
	{
		PENTTEXT pl = (PENTTEXT)pEntityData;

		ChangePoint(pl->Point0);
		ChangePoint(pl->TextData.SecondAlignmentPoint);
	}
	else if (type == ENT_VERTEX)
	{
		PENTVERTEX pl = (PENTVERTEX)pEntityData;

		ChangePoint(pl->Point);
	}
	else if (type == ENT_POLYLINE)
	{
		PENTPOLYLINE pl = (PENTPOLYLINE)pEntityData;

		// 多边形要处理每个点

		for (int i = 0; i < pl->nVertex; ++i)
		{
			ChangePoint(pl->pVertex[i].Point);
		}
	}
	else if (type == ENT_ELLIPSE)
	{
		PENTELLIPSE pl = (PENTELLIPSE)pEntityData;

		ChangePoint(pl->CenterPoint);
		ChangePoint(pl->MajorAxisEndPoint);
	}
	else
	{
		// 其他不包含点的
	}

	return true;
}


void CConvertDXFAssist::ChangePoint(REALPOINT &pt)
{
	double gx = pt.x;
	double gy = pt.y;
	double gz = pt.z;

	if (m_nConvertType == 0)
	{
		pt.x = gx*m_matrix[0] + gy*m_matrix[1] + gz*m_matrix[2] + m_matrix[3];
		pt.y = gx*m_matrix[4] + gy*m_matrix[5] + gz*m_matrix[6] + m_matrix[7];
		pt.z = gx*m_matrix[8] + gy*m_matrix[9] + gz*m_matrix[10] + m_matrix[11];
	}
	else
	{
		PT_3D tpt;
		m_tm1->ConvertToGeodetic(pt.x, pt.y, &gy, &gx);
		m_tm2->MolodenskyShift(*m_tm1, gy, gx, pt.z, &tpt.y, &tpt.x, &tpt.z);
		m_tm2->ConvertFromGeodetic(tpt.y, tpt.x, &pt.x, &pt.y);
		pt.z = tpt.z;
	}
}

BOOL ConvertFtrs(CFtrArray& arr, double *m, CUndoTransform* undo, BOOL bProgress)
{
	CGeometry *pGeo;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	PT_3DEX pt0, pt1;
	int nsz = arr.GetSize();

	if( bProgress )
		GProgressStart(nsz);

	if( undo )
		memcpy(undo->matrix,m,sizeof(undo->matrix));

	for( int i=0; i<nsz; i++)
	{
		if( bProgress )
			GProgressStep();

		if( undo )
			undo->arrHandles.Add(FtrToHandle(arr[i]));

		pGeo = arr[i]->GetGeometry();
		pGeo->GetShape(arrPts);

		int npt = arrPts.GetSize();
		for( int j=0; j<npt; j++)
		{
			pt0 = arrPts[j]; 

			pt1.x = pt0.x*m[0] + pt0.y*m[1] + pt0.z*m[2] + m[3];
			pt1.y = pt0.x*m[4] + pt0.y*m[5] + pt0.z*m[6] + m[7];
			pt1.z = pt0.x*m[8] + pt0.y*m[9] + pt0.z*m[10] + m[11];
			pt1.pencode = pt0.pencode;
			pt1.wid = pt0.wid;

			arrPts[j] = pt1;
		}

		pGeo->CreateShape(arrPts.GetData(), arrPts.GetSize());
	}

	if( bProgress )
		GProgressEnd();

	return TRUE;
}

BOOL ConvertFtrs(CFtrArray& arr, CTM *pTm1, CTM *pTm2, BOOL bProgress)
{
	CGeometry *pGeo;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	PT_3DEX pt0, pt1, tpt;
	int nsz = arr.GetSize();

	if( bProgress )
		GProgressStart(nsz);

	for( int i=0; i<nsz; i++)
	{
		if( bProgress )
			GProgressStep();

		pGeo = arr[i]->GetGeometry();
		pGeo->GetShape(arrPts);
		
		int npt = arrPts.GetSize();
		for( int j=0; j<npt; j++)
		{
			pt0 = arrPts[j]; 

			pTm1->ConvertToGeodetic(pt0.x,pt0.y,&pt1.y,&pt1.x);
			pTm2->MolodenskyShift(*pTm1,pt1.y,pt1.x,pt0.z,&tpt.y,&tpt.x,&tpt.z);
			pTm2->ConvertFromGeodetic(tpt.y,tpt.x,&pt0.x,&pt0.y);
			pt0.z = tpt.z;
						
			arrPts[j] = pt0;
		}
		
		pGeo->CreateShape(arrPts.GetData(), arrPts.GetSize());
	}

	if( bProgress )
		GProgressEnd();
	
	return TRUE;
}


BOOL ConvertDS(CDlgDataSource *pDS, double *m, CUndoTransform* undo, BOOL bProgress, CFtrArray* arr)
{
	CFtrArray arrFtrs;
	int nlay = pDS->GetFtrLayerCount();

	for (int i=0; i<nlay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer || !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		for( int j=0; j<nObjs; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;

			arrFtrs.Add(pFtr);
		}
	}

	if( arr )arr->Copy(arrFtrs);

	return ConvertFtrs(arrFtrs,m,undo,bProgress);
}


BOOL ConvertDS(CDlgDataSource *pDS, CTM *pTm1, CTM *pTm2, BOOL bProgress, CFtrArray* arr)
{
	CFtrArray arrFtrs;
	int nlay = pDS->GetFtrLayerCount();
	
	for (int i=0; i<nlay; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer || !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		for( int j=0; j<nObjs; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;
			
			arrFtrs.Add(pFtr);
		}
	}

	if( arr )arr->Copy(arrFtrs);
	
	return ConvertFtrs(arrFtrs,pTm1,pTm2,bProgress);	
}


BOOL ConvertVectFile(LPCTSTR filePath0, LPCTSTR filePath1, double *m, BOOL bProgress)
{
	CString path = filePath0;
	CString ext = path.Right(3);

	if( ext.CompareNoCase("fdb")==0 )
	{
		if( !::CopyFile(filePath0,filePath1,FALSE) )
			return FALSE;

		// -------------------------------- read all Features
		CSQLiteAccess *pAccess0 = new CSQLiteAccess;
		if( !pAccess0 )
		{
			return FALSE;
		}
		
		if( !pAccess0->Attach(filePath1) )
		{
			CString str = filePath1;
			str = str+_T("\n")+StrFromResID(IDS_FILE_OPEN_ERR);
			AfxMessageBox(str);
			delete pAccess0;
			return FALSE;
		}
		
		CDlgDataSource *pDS0 = new CDlgDataSource(NULL);	
		if( !pDS0 )
		{			
			CString str = filePath1;
			str = str+_T("\n")+StrFromResID(IDS_INVALID_FILE);
			AfxMessageBox(str);
			delete pAccess0;
			return FALSE;
		}
		
		pDS0->SetAccessObject(pAccess0);
		pAccess0->ReadDataSourceInfo(pDS0);
		pAccess0->BatchUpdateBegin();
		
		pDS0->LoadAll(NULL,bProgress);

		pAccess0->BatchUpdateEnd();

		// -------------------------------- convert

		ConvertDS(pDS0,m,NULL,bProgress,NULL);

		// -------------------------------- save
		pAccess0->BatchUpdateBegin();
		pDS0->SaveAll(NULL);
		pAccess0->BatchUpdateEnd();

		delete pDS0;

		return TRUE;
	}
	else if( ext.CompareNoCase("dxf")==0 )
	{
		CDxfDrawing dxfFile;
		dxfFile.Create();
		if(!dxfFile.LoadDXFFile(filePath0, NULL)) return FALSE;

		CConvertDXFAssist a;
		a.SetParams(m);

		int lSum = 0;
		if( bProgress )
		{
			if( dxfFile.FindEntity(FIND_FIRST,NULL)>0 )
			{
				do
				{
					lSum++;
				}
				while( dxfFile.FindEntity( FIND_NEXT,NULL )>0 );
			}
			GProgressStart(lSum);
		}		
		// 开始读DXF文件
		
		if( dxfFile.FindEntity(FIND_FIRST,NULL)>0 )
		{
			do			
			{
				GProgressStep();

				ENTITYHEADER	EntityHeader;
				char			EntityData[4096];
				
				dxfFile.GetCurEntityHeader(&EntityHeader);
				dxfFile.GetCurEntityData(EntityData);
				
				// 修改EntityData, 然后重新写入
				
				a.ChangeData(EntityHeader, EntityData);
				dxfFile.ChangeEntity(&EntityHeader, EntityData);
			} 
			while(dxfFile.FindEntity(FIND_NEXT, NULL) > 0);
		}

		if( bProgress )
		{
			GProgressEnd();
		}

		dxfFile.SaveDXFFile(filePath1, NULL);

		return TRUE;
	}
	else if( ext.CompareNoCase("dwg")==0 )
	{
		if( !::CopyFile(filePath0,filePath1,FALSE) )
			return FALSE;

		CConvertDWGAssist a;
		a.SetParams(m);
		a.Convert(filePath1);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}



BOOL ConvertVectFile(LPCTSTR filePath0, LPCTSTR filePath1, CTM *pTm1, CTM *pTm2, BOOL bProgress)
{
	CString path = filePath0;
	CString ext = path.Right(3);
	
	if( ext.CompareNoCase("fdb")==0 )
	{
		if( !::CopyFile(filePath0,filePath1,FALSE) )
			return FALSE;
		
		// -------------------------------- read all Features
		CSQLiteAccess *pAccess0 = new CSQLiteAccess;
		if( !pAccess0 )
		{
			return FALSE;
		}
		
		if( !pAccess0->Attach(filePath1) )
		{
			CString str = filePath1;
			str = str+_T("\n")+StrFromResID(IDS_FILE_OPEN_ERR);
			AfxMessageBox(str);
			delete pAccess0;
			return FALSE;
		}
		
		CDlgDataSource *pDS0 = new CDlgDataSource(NULL);	
		if( !pDS0 )
		{			
			CString str = filePath1;
			str = str+_T("\n")+StrFromResID(IDS_INVALID_FILE);
			AfxMessageBox(str);
			delete pAccess0;
			return FALSE;
		}
		
		pDS0->SetAccessObject(pAccess0);
		pAccess0->ReadDataSourceInfo(pDS0);
		pAccess0->BatchUpdateBegin();
		
		pDS0->LoadAll(NULL,bProgress);
		
		pAccess0->BatchUpdateEnd();
		
		// -------------------------------- convert
		
		ConvertDS(pDS0,pTm1,pTm2,bProgress,NULL);
		
		// -------------------------------- save
		pAccess0->BatchUpdateBegin();
		pDS0->SaveAll(NULL);
		pAccess0->BatchUpdateEnd();
		
		delete pDS0;
		
		return TRUE;
	}
	else if( ext.CompareNoCase("dxf")==0 )
	{
		CDxfDrawing dxfFile;
		dxfFile.Create();
		if(!dxfFile.LoadDXFFile(filePath0, NULL)) return FALSE;
		
		CConvertDXFAssist a;
		a.SetParams(pTm1,pTm2);
		
		// 开始读DXF文件
		
		while(dxfFile.FindEntity(FIND_NEXT, NULL) > 0)
		{
			ENTITYHEADER	EntityHeader;
			char			EntityData[4096];
			
			dxfFile.GetCurEntityHeader(&EntityHeader);
			dxfFile.GetCurEntityData(EntityData);
			
			// 修改EntityData, 然后重新写入
			
			a.ChangeData(EntityHeader, EntityData);
			dxfFile.ChangeEntity(&EntityHeader, EntityData);
		} 
		dxfFile.SaveDXFFile(filePath1, NULL);
		
		return TRUE;
	}
	else if( ext.CompareNoCase("dwg")==0 )
	{
		if (!::CopyFile(filePath0, filePath1, FALSE))
			return FALSE;

		CConvertDWGAssist a;
		a.SetParams(pTm1,pTm2);
		a.Convert(filePath1);
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL ReadCtrlPts(LPCTSTR filePath, CArray<PT_3D,PT_3D>& pts)
{
	FILE *fp = fopen(filePath,"r");
	if (!fp)
	{
		AfxMessageBox(IDS_FILE_OPEN_ERR);
		return FALSE;
	}
	
	char line[1024],name[1024];
	PT_3D pt;
	
	while (!feof(fp)) 
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		
		pt.z = 0;
		if (sscanf(line,"%s %lf %lf %lf",name,&pt.x,&pt.y,&pt.z)<3 )
			continue;
		
		pts.Add(pt);
	}

	fclose(fp);

	return TRUE;
}