// UVSModify.cpp: implementation of the CUVSModify class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UVSModify.h"
#include "UVSCashe.h"
#include "uvsclient.h"
#include "ClientDataType.h"
#include "DataSourceEx.h"
#include "Scheme.h"
#include "GeoPoint.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "SmartViewFunctions.h"
#include "SymbolLib.h"

using uvscashe::CUVSCashe;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
uvsclient::IUVSClient * CUVSModify::puvs = NULL;
BOOL CUVSModify::update_immediately = TRUE;
uvsclient::WorkSpace * CUVSModify::current_workspace=NULL;
BOOL CUVSModify::can_export_fdb = TRUE;
//
CUVSModify::CUVSModify():
current_mapsheet(NULL),record_set(NULL)
{
   pds=NULL;
   fetch_size=500;
}

CUVSModify::~CUVSModify()
{
	vector_layer.clear();
	vector_group.clear();
	record_set = NULL;
	for(map<CStringA,uvsclient::attributefielddefineSet*>::iterator itr = layer_xattribute.begin();itr!=layer_xattribute.end();++itr)
	{
		if(itr->second!=NULL)
		{
			itr->second->release();
			itr->second = NULL;
		}
	}
	layer_xattribute.clear();
	for(list<CFeature*>::iterator itr1 = feature_list.begin();itr1!=feature_list.end();++itr1)
	{
		if(*itr1!=NULL)
		{
			delete *itr1;
			*itr1 = NULL;
		}
	}
    feature_list.clear();
	Close();
}

BOOL  CUVSModify::Compress()
{
	return FALSE;
}

BOOL  CUVSModify::Repair()
{
	return FALSE;
}

BOOL CUVSModify::ConnectUVSServer(LPCTSTR ip, int port)
{
	if (puvs != NULL)
		return TRUE;

	puvs = uvsclient::IUVSClient::NEW();
	if (!puvs->Connect(ip, port))
	{
		puvs->Release();
		puvs = NULL;
		return FALSE;
	}

	if (!CUVSCashe::ConnectUVSServer(ip, port))
	{
		puvs->CloseDatasource();
		puvs->Release();
		puvs = NULL;
		return FALSE;
	}

	CUVSModify::can_export_fdb = puvs->CanExport();

	return TRUE;
}

BOOL CUVSModify::OpenDB(LPCTSTR db_name, LPCTSTR user_name, LPCTSTR password,
		                         CString & identity_id,CString & identity_id2)
{
	if(!puvs) return FALSE;
	//
	uvsclient::DatasourceConfig cfg;
	sprintf(cfg.datasource_name, "%s", db_name);
	sprintf(cfg.user_name, "%s", user_name);
	sprintf(cfg.password, "%s", password);
	sprintf(cfg.identity_id, "%s", (LPCTSTR)identity_id);
	cfg.connect_timeout = 5;
	if (!puvs->OpenDatasource(cfg))
	{
		identity_id = "";
		AfxMessageBox(puvs->GetLastError());
		return FALSE;
	}
	
	identity_id = cfg.identity_id;
	puvs->FlushImmediately(true);
	//
    if(!CUVSCashe::OpenDB(db_name, user_name, password, identity_id2) )
	{
		ReleaseUVSServer();
		return FALSE;
	}
	return TRUE;
}

void CUVSModify::UpdateImmediately(BOOL _update_immediately)
{
	if(!_update_immediately)
	{
		CUVSCashe::StartExecute();
		update_immediately=FALSE;
	}
	else
	{
		CUVSCashe::WaiteExecuteAll();
		CUVSCashe::StopExecute();
		update_immediately=TRUE;
	}
}

BOOL CUVSModify::is_updateimmediately()
{
	return update_immediately;
}

BOOL CUVSModify::CanExport()
{
	return can_export_fdb;
}

void CUVSModify::SetDelayCount(int delay_count)
{
	CUVSCashe::SetMaxStack(delay_count);
}

int CUVSModify::GetDelayCount()
{
	return CUVSCashe::GetMaxStack();
}

void CUVSModify::ReleaseUVSServer()
{
	if (current_workspace)
	{
		delete current_workspace;
		current_workspace = NULL;
	}

	if(puvs!=NULL)
	{
		puvs->CloseDatasource();
		puvs->Release();
		puvs=NULL;
	}
	//
	CUVSCashe::ReleaseUVSServer();
}

BOOL CUVSModify::IsDBConnected()
{
   if(puvs==NULL || current_workspace==NULL)
   {
	   return FALSE;
   }
   return TRUE;
}

void CUVSModify::EnumerateDBList(CStringArray& db_list)
{
	if (puvs == NULL)
	{
		return;
	}
	//
	uvsclient::StringList *dbs = uvsclient::StringList::NEW();
	puvs->GetDataSourceList(dbs);
	for (int i = 0; i < dbs->get_count(); ++i)
	{
		db_list.Add(dbs->get_item(i));
	}
	dbs->release();
}

void CUVSModify::EnumerateWorkspace(CStringArray& workspace_list)
{
   if(puvs==NULL)
   {
	   return;
   }
   //
   uvsclient::workspaceSet * workspaces=uvsclient::workspaceSet::NEW();
   puvs->GetWorkspace(workspaces);
   for(int i=0;i<workspaces->get_count();++i)
   {
	   workspace_list.Add(workspaces->get_item(i).name);
   }
   workspaces->release();
}

void CUVSModify::EnumerateMapsheet(CStringArray& mapsheet_list, CArray<double, double>& bounds)
{
	if(puvs==NULL)
	{
		return;
	}
	//
	uvsclient::mapsheetSet * mapsheets=uvsclient::mapsheetSet::NEW();
	puvs->GetMapsheet(mapsheets);
	for(int i=0;i<mapsheets->get_count();++i)
	{
		mapsheet_list.Add(mapsheets->get_item(i).name);
		for (int j = 0; j < 12; j++)
		{
			bounds.Add(mapsheets->get_item(i).bound[j]);
		}
	}
	mapsheets->release();
}

BOOL CUVSModify::AddMapsheet(LPCTSTR name, double lbx, double lby, double rbx, double rby,
	double rtx, double rty, double ltx, double lty,float zmin, float zmax)
{
    if(puvs==NULL || current_workspace==NULL)
	{
		return FALSE;
	}
	//
	uvsclient::MapSheet * map_sheet=new uvsclient::MapSheet();
	sprintf(map_sheet->name,"%s",name);
	sprintf(map_sheet->workspace_name,"%s",current_workspace->name);
	map_sheet->bound[0]=lbx; map_sheet->bound[1]=lby; map_sheet->bound[2]=0;
    map_sheet->bound[3]=rbx; map_sheet->bound[4]=rby; map_sheet->bound[5]=0;
	map_sheet->bound[6]=rtx; map_sheet->bound[7]=rty; map_sheet->bound[8]=0;
	map_sheet->bound[9]=ltx; map_sheet->bound[10]=lty; map_sheet->bound[11]=0;
	map_sheet->zmax=zmax;
	map_sheet->zmin=zmin;
	map_sheet->scale=current_workspace->scale;
	if(puvs->IsMapsheetExist(map_sheet->name))
	{
		CString error_inf = CString(map_sheet->name) + CString(" exist");
		AfxMessageBox(error_inf);
		return FALSE;
	}
	BOOL successed = puvs->SaveMapsheet(*map_sheet);
	delete map_sheet;
	map_sheet = NULL;
	return successed;
}

BOOL CUVSModify::SetCurrentWorkspace(LPCTSTR workspace_name)
{
   if(puvs==NULL/* || current_workspace!=NULL*/)
   {
	   return FALSE;
   }
   //
   uvsclient::workspaceSet * workspaces=uvsclient::workspaceSet::NEW();
   puvs->GetWorkspace(workspaces);
   CString the_workspace(workspace_name);
   for(int i=0;i<workspaces->get_count();++i)
   {
	   CString temp(workspaces->get_item(i).name);
	   if(temp==the_workspace)
	   {
		   uvsclient::WorkSpace &temp_workspace=workspaces->get_item(i);
		   puvs->SetCurrentWorkspace(temp_workspace);
		   CUVSCashe::SetCurrentWorkspace(temp_workspace);
		   current_workspace=new uvsclient::WorkSpace();
		   *current_workspace=temp_workspace;
		   workspaces->release();
		   return TRUE;
	   }
   }
   workspaces->release();
   return FALSE;
}

CString CUVSModify::GetCurrentWorkspace()
{
	if(puvs || current_workspace)
	{
		return CString(current_workspace->name);
	}

	return _T("");
}

BOOL CUVSModify::Attach(LPCTSTR fileName)
{
	if(puvs==NULL || current_workspace==NULL)
	{
		return FALSE;
	}
	//
	uvsclient::mapsheetSet * mapsheets=uvsclient::mapsheetSet::NEW();
	puvs->GetMapsheet(mapsheets);
	CString the_mapsheet(fileName);
	for(int j=0;j<mapsheets->get_count();++j)
	{
		uvsclient::MapSheet & temp_mapsheet=mapsheets->get_item(j);
		CString temp(temp_mapsheet.name);
		if(the_mapsheet==temp)
		{
			current_mapsheet=new uvsclient::MapSheet();
			*current_mapsheet=temp_mapsheet;
			mapsheets->release();
			return TRUE;
		}
	} 
    mapsheets->release();
	return FALSE;
}

BOOL CUVSModify::Close()
{
	CUVSModify::ReleaseUVSServer();
	return TRUE;
}

BOOL CUVSModify::CreateFileSys(long Scale, CScheme *ps)
{
	return FALSE;
}

BOOL CUVSModify::IsValid()
{
	return FALSE;
}

void CUVSModify::ClearAttrTables()
{
	return;
}

BOOL CUVSModify::OpenNew(LPCTSTR fileName)
{
	return FALSE;
}

BOOL CUVSModify::OpenRead(LPCTSTR filename)
{
	return FALSE;
}

BOOL CUVSModify::OpenWrite(LPCTSTR filename)
{
	return FALSE;
}

BOOL CUVSModify::CloseRead()
{
	return FALSE;
}

BOOL CUVSModify::CloseWrite()
{
	return FALSE;
}

void CUVSModify::ReadSpecialData(CDataSourceEx *pDS)
{
	if (!pDS) return;

	ObjectGroup *pGroup = GetFirstObjectGroup();
	while (pGroup)
	{
		pDS->AddObjectGroup(pGroup, TRUE);
		pGroup = GetNextObjectGroup();
	}

	CScheme * pScheme = GetConfigLibManager()->GetScheme(pDS->GetScale());
	if (!pScheme) return;
	int nLayer = pScheme->GetLayerDefineCount();
	for (int i = 0; i < nLayer; i++)
	{
		CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(i);
		if (!pDef) continue;
		CString group = pDef->GetGroupName();

		if (pDS->GetFtrLayerGroupByName(group))
			continue;

		FtrLayerGroup *pFtrLayerGroup = new FtrLayerGroup;
		pFtrLayerGroup->id = i;
		pFtrLayerGroup->Name = group;
		pDS->AddFtrLayerGroup(pFtrLayerGroup, TRUE);
	}
}

void CUVSModify::WriteSpecialData(CDataSourceEx *pDS)
{
	if (!pDS) return;

	ObjectGroup *objGroup=NULL;
	int nCount = pDS->GetObjectGroupCount();
	for (int i = 0; i < nCount; i++)
	{
		objGroup = pDS->GetObjectGroup(i);
		if (!objGroup)  continue;

		SaveObjectGroup(objGroup);
	}
}

void CUVSModify::ReadObjGroup()
{
	if(puvs==NULL || current_mapsheet==NULL)
	{
		return;
	}
	//
	uvsclient::vectorgroupSet * groups=uvsclient::vectorgroupSet::NEW();
	puvs->GetVectorGroup(current_mapsheet->name,groups);
	for(int i=0;i<groups->get_count();++i)
	{
		uvsclient::VectorGroup & temp=groups->get_item(i);
        vector_group.push_back(temp);
	}
	groups->release();
}

ObjectGroup* CUVSModify::GetFirstObjectGroup()
{
	ReadObjGroup();
	//
	if(vector_group.size()==0)
	{
		return NULL;
	}
	//
	read_group_index=0;
	uvsclient::VectorGroup & group=vector_group[read_group_index];
    //
	ObjectGroup *pGroup = new ObjectGroup;
	if (!pGroup) return NULL;
	
	CValueTable tab;
		

	pGroup->id = group.id_in_map;
	strncpy(pGroup->name,group.name,15); 
	pGroup->name[15] = '\0';
	pGroup->select = group.selectable;
	
	++read_group_index;
	return pGroup;
}

ObjectGroup* CUVSModify::GetNextObjectGroup()
{
	if(read_group_index>=vector_group.size())
	{
		vector_group.clear();
		read_group_index=0;
		return NULL;
	}
	//
	uvsclient::VectorGroup & group=vector_group[read_group_index];
    //
	ObjectGroup *pGroup = new ObjectGroup;
	if (!pGroup) return NULL;
	
	CValueTable tab;
		
	
	pGroup->id = group.id_in_map;
	strncpy(pGroup->name,group.name,15); 
	pGroup->name[15] = '\0';
	pGroup->select = group.selectable;
	
	++read_group_index;
	return pGroup;
}

FtrLayerGroup* CUVSModify::GetFirstFtrLayerGroup()
{
	return NULL;
}

FtrLayerGroup* CUVSModify::GetNextFtrLayerGroup()
{
	return NULL;
}

//读取数据源信息
BOOL CUVSModify::ReadDataSourceInfo(CDataSourceEx *pDS)
{
	if(current_mapsheet==NULL)
	{
		return FALSE;
	}
	//
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
	tab.AddValue(FIELDNAME_SCALE,&(CVariantEx)(_variant_t)(long)current_workspace->scale);
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	for(int i=0;i<uvsclient::bound_length/3;++i)
	{
       PT_3DEX temp;
	   temp.x=current_mapsheet->bound[i*3+0];
	   temp.y=current_mapsheet->bound[i*3+1];
	   temp.z=current_mapsheet->bound[i*3+2];
	   temp.pencode=1;
	   arrPts.Add(temp);
	}
	
	var.SetAsShape(arrPts);
	
	tab.AddValue(FIELDNAME_BOUND,&var);

	tab.AddValue(FIELDNAME_ZMIN,&(CVariantEx)(_variant_t)current_mapsheet->zmin);
	
	tab.AddValue(FIELDNAME_ZMAX,&(CVariantEx)(_variant_t)current_mapsheet->zmax);
	
	tab.EndAddValueItem();
	
	pDS->ReadFrom(tab);
	
	return TRUE;
}

void CUVSModify::valuetableFromVectorLayer(CValueTable & tab,uvsclient::VectorLayer & temp_layer)
{
	tab.BeginAddValueItem();
	tab.AddValue(FIELDNAME_LAYERID,&(CVariantEx)(_variant_t)(long)temp_layer.id_in_map);
	tab.AddValue(FIELDNAME_LAYERNAME,&(CVariantEx)(_variant_t)temp_layer.name);	
	tab.AddValue(FIELDNAME_LAYGROUPNAME,&(CVariantEx)(_variant_t)"");
	tab.AddValue(FIELDNAME_LAYCOLOR,&(CVariantEx)(_variant_t)(long)temp_layer.color);
	tab.AddValue(FIELDNAME_LAYLOCKED,&(CVariantEx)(_variant_t)(long)temp_layer.locked);
	tab.AddValue(FIELDNAME_LAYVISIBLE,&(CVariantEx)(_variant_t)(long)temp_layer.visible);
	tab.AddValue(FIELDNAME_LAYINHERENT,&(CVariantEx)(_variant_t)(long)temp_layer.inherent);
	tab.AddValue(FIELDNAME_LAYERDISPLAYORDER,&(CVariantEx)(_variant_t)(long)temp_layer.display_order);
	tab.AddValue(FIELDNAME_LAYERSYMBOLIZED,&(CVariantEx)(_variant_t)(long)temp_layer.symbolized);
	tab.EndAddValueItem();
}

//读取层
CFtrLayer * CUVSModify::GetFirstFtrLayer(long &idx)
{
	ReadFtrLayer();
	//
	read_layer_index=0;
	//
	return GetNextFtrLayer(idx);
}


CFtrLayer * CUVSModify::GetNextFtrLayer(long &idx)
{
	if(read_layer_index==vector_layer.size())
	{
		vector_layer.clear();
		//设置组名
		CConfigLibManager *pCfg = GetConfigLibManager();
		if (pCfg && pds)
		{
			CScheme *pScheme = pCfg->GetScheme(pds->GetScale());
			if (pScheme)
			{
				int nLay = GetFtrLayerCount();
				for (int i = 0; i < nLay; i++)
				{
					CFtrLayer *pLayer = pds->GetFtrLayer(i);
					if (!pLayer) continue;
					CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(pLayer->GetName());
					if (pDef) pLayer->SetGroupName(pDef->GetGroupName());
				}
			}
		}
		//
		return NULL;
	}
	//
	uvsclient::VectorLayer & temp_layer=vector_layer[read_layer_index];
	//
	CValueTable tab;
	valuetableFromVectorLayer(tab,temp_layer);
	
	CFtrLayer *pLayer = new CFtrLayer();
	if( !pLayer )return NULL;
	
	pLayer->ReadFrom(tab);
	///uvs////////////////// set strID //////////////////////
    pLayer->SetstrID(temp_layer.uuid);
	pLayer->SetXAttributeName(temp_layer.attribute_name);
	//
	if(puvs->IsLayerHasAttribute(temp_layer.uuid))
	{
		///uvs////////
		pLayer->SetHaveExtraAttr(TRUE);
	}
	else
	{
		pLayer->SetHaveExtraAttr(FALSE);
	}
    //
	++read_layer_index;
	
	return pLayer;
}

void CUVSModify::ReadFtrLayer()
{
    if(puvs==NULL || current_mapsheet==NULL)
	{
		return;
	}
	//
	uvsclient::vectorlayerSet * layers=uvsclient::vectorlayerSet::NEW();
	puvs->vectorlayerInMapsheet(current_mapsheet->name,layers);
    for(int i=0;i<layers->get_count();++i)
	{
		uvsclient::VectorLayer & temp=layers->get_item(i);
		vector_layer.push_back(temp);
	}
	layers->release();
}

int CUVSModify::GetFtrLayerCount()
{
	if(puvs==NULL)
	{
		return -1;
	}
    
	return puvs->layercountInMapsheet(current_mapsheet->name);
}

CFtrLayer * CUVSModify::GetFtrLayerByID(long id)
{
	if(pds!=NULL)
        return pds->GetFtrLayer(id);
	else
		return NULL;
}

CFtrLayer * CUVSModify::GetFtrLayerByName(LPCTSTR layerName)
{
	if(pds!=NULL)
	    return pds->GetFtrLayer(layerName);
	else
		return NULL;
}

void CUVSModify::valuetableFromPointFTR(CValueTable & tab,uvsclient::PointFTR & point_ftr)
{
	tab.BeginAddValueItem();
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)point_ftr.fid);
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)point_ftr.class_type);
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)point_ftr.geo_class);	
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)point_ftr.color);
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)point_ftr.symbolize_flag);
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)point_ftr.symbol_name);
	m_nCurLayID = pds->GetFtrLayerBystrID(point_ftr.layer_id)->GetID();
	
	tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&(CVariantEx)(_variant_t)point_ftr.angle);
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)point_ftr.code);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)point_ftr.visible);
	tab.AddValue(FIELDNAME_GEOPOINT_KX,&(CVariantEx)(_variant_t)point_ftr.kx);
	tab.AddValue(FIELDNAME_GEOPOINT_KY,&(CVariantEx)(_variant_t)point_ftr.ky);
	tab.AddValue(FIELDNAME_GEOPOINT_COVERTYPE,&(CVariantEx)(_variant_t)(long)point_ftr.covertype);
	tab.AddValue(FIELDNAME_GEOPOINT_EXTENDDIS,&(CVariantEx)(_variant_t)point_ftr.extenddis);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)point_ftr.purpose);
	tab.AddValue(FIELDNAME_GEOPOINT_WIDTH,&(CVariantEx)(_variant_t)point_ftr.width);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)point_ftr.display_order);
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)point_ftr.modify_time);
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int shape_size=point_ftr.shape_size;
	for(int i=0;i<shape_size;++i)
	{
		PT_3DEX temp_shape;
		temp_shape.x=point_ftr.shape[i].x;
		temp_shape.y=point_ftr.shape[i].y;
		temp_shape.z=point_ftr.shape[i].z;
		temp_shape.wid=point_ftr.shape[i].wid;
		temp_shape.pencode=point_ftr.shape[i].pencode;
		temp_shape.type=point_ftr.shape[i].type;
		arrPts.Add(temp_shape);
	}
	CVariantEx var;
	var.SetAsShape(arrPts);
	tab.AddValue(FIELDNAME_SHAPE,&var);	

	string str_group=point_ftr.group_name;
	unsigned char * group_blob=toHexArray(str_group);
	var.SetAsBlob(group_blob,str_group.length()/2);
	tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	delete [] group_blob;
	tab.EndAddValueItem();
}

void CUVSModify::valuetableFromLineFTR(CValueTable & tab,uvsclient::LineFTR & line_ftr)
{
    tab.BeginAddValueItem();
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)line_ftr.fid);
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)line_ftr.class_type);
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)line_ftr.geo_class);

	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)line_ftr.color);
	//tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)16777215);

	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)line_ftr.symbolize_flag);
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)line_ftr.symbol_name);
	
	//nValue = m_ftrQuery.getIntField("LayerID");
	m_nCurLayID = pds->GetFtrLayerBystrID(line_ftr.layer_id)->GetID();
	
	tab.AddValue(FIELDNAME_GEOCURVE_WIDTH,&(CVariantEx)(_variant_t)line_ftr.width);

	tab.AddValue(FIELDNAME_GEOCURVE_DHEIGHT,&(CVariantEx)(_variant_t)line_ftr.dheight);
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)line_ftr.code);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)line_ftr.visible);
	tab.AddValue(FIELDNAME_GEOCURVE_LINETYPESCALE,&(CVariantEx)(_variant_t)line_ftr.line_type_scale);
	tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTHSCALE,&(CVariantEx)(_variant_t)line_ftr.line_width_scale);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)line_ftr.purpose);
	//
	tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTH,&(CVariantEx)(_variant_t)(double)line_ftr.line_width);
	//
	tab.AddValue(FIELDNAME_GEOCURVE_LINETYPEXOFF,&(CVariantEx)(_variant_t)line_ftr.line_type_xoff);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)line_ftr.display_order);
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)line_ftr.modify_time);
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int shape_size=line_ftr.shape_size;
	for(int i=0;i<shape_size;++i)
	{
		PT_3DEX temp_shape;
		temp_shape.x=line_ftr.shape[i].x;
		temp_shape.y=line_ftr.shape[i].y;
		temp_shape.z=line_ftr.shape[i].z;
		temp_shape.wid=line_ftr.shape[i].wid;
		temp_shape.pencode=line_ftr.shape[i].pencode;
		temp_shape.type=line_ftr.shape[i].type;
		arrPts.Add(temp_shape);
	}
	CVariantEx var;
	var.SetAsShape(arrPts);
	tab.AddValue(FIELDNAME_SHAPE,&var);	
	
	string str_group=line_ftr.group_name;
	unsigned char * group_blob=toHexArray(str_group);
	var.SetAsBlob(group_blob,str_group.length()/2);
	tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	delete [] group_blob;
	tab.EndAddValueItem();
}

void CUVSModify::valuetableFromPolygonFTR(CValueTable & tab,uvsclient::PolygonFTR & polygon_ftr)
{
	tab.BeginAddValueItem();
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)polygon_ftr.fid);
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)polygon_ftr.class_type);
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)polygon_ftr.geo_class);
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)polygon_ftr.color);
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)polygon_ftr.symbolize_flag);
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)polygon_ftr.symbol_name);
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)polygon_ftr.code);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)polygon_ftr.visible);
	tab.AddValue(FIELDNAME_GEOSURFACE_CELLANGLE,&(CVariantEx)(_variant_t)polygon_ftr.cell_angle);
	tab.AddValue(FIELDNAME_GEOSURFACE_CELLSCALE,&(CVariantEx)(_variant_t)polygon_ftr.cell_scale);
	tab.AddValue(FIELDNAME_GEOSURFACE_INTVSCALE,&(CVariantEx)(_variant_t)polygon_ftr.intv_scale);
	tab.AddValue(FIELDNAME_GEOSURFACE_XSTARTOFF,&(CVariantEx)(_variant_t)polygon_ftr.xstartoff);
	tab.AddValue(FIELDNAME_GEOSURFACE_YSTARTOFF,&(CVariantEx)(_variant_t)polygon_ftr.ystartoff);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)polygon_ftr.purpose);
	tab.AddValue(FIELDNAME_GEOSURFACE_LINEWIDTH,&(CVariantEx)(_variant_t)polygon_ftr.line_width);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)polygon_ftr.display_order);
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)polygon_ftr.modify_time);
	
	m_nCurLayID = pds->GetFtrLayerBystrID(polygon_ftr.layer_id)->GetID();
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int shape_size=polygon_ftr.shape_size;
	for(int i=0;i<shape_size;++i)
	{
		PT_3DEX temp_shape;
		temp_shape.x=polygon_ftr.shape[i].x;
		temp_shape.y=polygon_ftr.shape[i].y;
		temp_shape.z=polygon_ftr.shape[i].z;
		temp_shape.wid=polygon_ftr.shape[i].wid;
		temp_shape.pencode=polygon_ftr.shape[i].pencode;
		temp_shape.type=polygon_ftr.shape[i].type;
		arrPts.Add(temp_shape);
	}		
	CVariantEx var;
	var.SetAsShape(arrPts);
	tab.AddValue(FIELDNAME_SHAPE,&var);		

	string str_group=polygon_ftr.group_name;
	unsigned char * group_blob=toHexArray(str_group);
	var.SetAsBlob(group_blob,str_group.length()/2);
	tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	delete [] group_blob;
	tab.EndAddValueItem();
}

void CUVSModify::valuetableFromTextFTR(CValueTable & tab,uvsclient::TextFTR & text_ftr)
{
	tab.BeginAddValueItem();
	tab.AddValue(FIELDNAME_FTRID,&(CVariantEx)(_variant_t)text_ftr.fid);
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)text_ftr.class_type);
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)text_ftr.geo_class);
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)(long)text_ftr.color);
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&(CVariantEx)(_variant_t)(long)text_ftr.symbolize_flag);
	tab.AddValue(FIELDNAME_SYMBOLNAME,&(CVariantEx)(_variant_t)text_ftr.symbol_name);
	
	//nValue = m_ftrQuery.getIntField("LayerID");
	m_nCurLayID = pds->GetFtrLayerBystrID(text_ftr.layer_id)->GetID();
	
	tab.AddValue(FIELDNAME_GEOTEXT_CONTENT,&(CVariantEx)(_variant_t)(LPCTSTR)text_ftr.content);
	tab.AddValue(FIELDNAME_GEOTEXT_FONT,&(CVariantEx)(_variant_t)(LPCTSTR)text_ftr.font);
	tab.AddValue(FIELDNAME_GEOTEXT_CHARWIDTHS,&(CVariantEx)(_variant_t)text_ftr.char_width);
	tab.AddValue(FIELDNAME_GEOTEXT_CHARHEIGHT,&(CVariantEx)(_variant_t)text_ftr.char_height);
	tab.AddValue(FIELDNAME_GEOTEXT_CHARINTVS,&(CVariantEx)(_variant_t)text_ftr.char_intvr);
	tab.AddValue(FIELDNAME_GEOTEXT_LINEINTVS,&(CVariantEx)(_variant_t)text_ftr.line_intvr);
	tab.AddValue(FIELDNAME_GEOTEXT_ALIGNTYPE,&(CVariantEx)(_variant_t)(long)text_ftr.align_type);
	tab.AddValue(FIELDNAME_GEOTEXT_TEXTANGLE,&(CVariantEx)(_variant_t)text_ftr.text_angle);
	tab.AddValue(FIELDNAME_GEOTEXT_CHARANGLE,&(CVariantEx)(_variant_t)text_ftr.char_angle);
	tab.AddValue(FIELDNAME_GEOTEXT_INCLINE,&(CVariantEx)(_variant_t)(long)text_ftr.incline);
	tab.AddValue(FIELDNAME_GEOTEXT_INCLINEANGLE,&(CVariantEx)(_variant_t)text_ftr.incline_angle);
	tab.AddValue(FIELDNAME_GEOTEXT_PLACETYPE,&(CVariantEx)(_variant_t)(long)text_ftr.place_type);
	tab.AddValue(FIELDNAME_GEOTEXT_OTHERFLAG,&(CVariantEx)(_variant_t)(long)text_ftr.other_flag);
	tab.AddValue(FIELDNAME_FTRCODE,&(CVariantEx)(_variant_t)text_ftr.code);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&(CVariantEx)(_variant_t)(bool)text_ftr.visible);
	tab.AddValue(FIELDNAME_GEOTEXT_COVERTYPE,&(CVariantEx)(_variant_t)(long)text_ftr.cover_type);
	tab.AddValue(FIELDNAME_GEOTEXT_EXTENDDIS,&(CVariantEx)(_variant_t)text_ftr.extenddis);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&(CVariantEx)(_variant_t)(long)text_ftr.purpose);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&(CVariantEx)(_variant_t)(double)text_ftr.display_order);
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&(CVariantEx)(_variant_t)(long)text_ftr.modify_time);
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int shape_size = text_ftr.shape_size;
	for (int i = 0; i < shape_size; ++i)
	{
		PT_3DEX temp_shape;
		temp_shape.x = text_ftr.shape[i].x;
		temp_shape.y = text_ftr.shape[i].y;
		temp_shape.z = text_ftr.shape[i].z;
		temp_shape.wid = text_ftr.shape[i].wid;
		temp_shape.pencode = text_ftr.shape[i].pencode;
		temp_shape.type = text_ftr.shape[i].type;
		arrPts.Add(temp_shape);
	}
	CVariantEx var;
	var.SetAsShape(arrPts);
	tab.AddValue(FIELDNAME_SHAPE, &var);

	string str_group=text_ftr.group_name;
	unsigned char * group_blob=toHexArray(str_group);
	var.SetAsBlob(group_blob,str_group.length()/2);
	tab.AddValue(FIELDNAME_FTRGROUPID,&var);
	delete [] group_blob;
	tab.EndAddValueItem();
}


//读取地物
CFeature * CUVSModify::GetFirstFeature(long &idx, CValueTable *exAttriTab)
{
	if(puvs==NULL)
	{
		return NULL;
	}
    point_loaded=false;
	line_loaded=false;
	polygon_loaded=false;
	text_loaded=false;
	feature_list.clear();
	if(record_set!=NULL)
	{
		record_set->finish();
		record_set=NULL;
	}
	//
	return GetNextFeature(idx, exAttriTab);
}

CFeature * CUVSModify::GetNextFeature(long &idx, CValueTable *exAttriTab)
{
READ:
	if(feature_list.size()>0)
	{
		CFeature *temp=*feature_list.begin();
		feature_list.pop_front();
		m_nCurLayID=temp->GetLayerID();
		return temp;
	}
	//
	int fetch_count=0;
	if(!point_loaded)
	{
		if (record_set == NULL)
		{
			record_set = puvs->pointftrsInMapsheet(current_mapsheet->name);
		}
		//
       	uvsclient::pointftrSet * pointftrs=uvsclient::pointftrSet::NEW();
F1:		fetch_count = record_set->fetch(fetch_size,pointftrs);
		if(fetch_count>0)
		{
			for(int i=0;i<fetch_count;++i)
			{
				uvsclient::PointFTR & point_ftr=pointftrs->get_item(i);
				if(pds->GetFtrLayerBystrID(point_ftr.layer_id)==NULL)
				{
					//CString msg;
					//msg.Format(IDS_LAYER_INFO_LOSED, point_ftr.layer_id);
					//AfxMessageBox(msg);
					continue;
				}
				//
				CValueTable tab;
				valuetableFromPointFTR(tab,point_ftr);
				CFeature *pFt = (CFeature*)CreateObject(tab);
				pFt->SetLayerID(m_nCurLayID);
				if(pFt!=NULL)
				{
					feature_list.push_back(pFt);
				}
			}
			if(feature_list.size()==0)
			{
				pointftrs->clear();
				goto F1;
			}
			goto READ;
		}
		else
		{
			if(record_set->hitend())
			{
				record_set->finish();
				record_set=NULL;
				point_loaded=true;
			}
			else
			{
				point_loaded=true;
				line_loaded=true;
				polygon_loaded=true;
				text_loaded=true;
				return NULL;
			}
		}
	}
	//
	if(!line_loaded)
	{
		if(record_set==NULL)
		{
			record_set = puvs->lineftrsInMapsheet(current_mapsheet->name);
		}
		//
        uvsclient::lineftrSet * lineftrs=uvsclient::lineftrSet::NEW();
F2:		fetch_count = record_set->fetch(fetch_size,lineftrs);
		if(fetch_count>0)
		{
			for(int i=0;i<fetch_count;++i)
			{
				uvsclient::LineFTR & line_ftr=lineftrs->get_item(i);
				if(pds->GetFtrLayerBystrID(line_ftr.layer_id)==NULL)
				{
					//CString msg;
					//msg.Format(IDS_LAYER_INFO_LOSED, line_ftr.layer_id);
					//AfxMessageBox(msg);
					continue;
				}
				//
				CValueTable tab;
				valuetableFromLineFTR(tab,line_ftr);
				CFeature *pFt = (CFeature*)CreateObject(tab);
				pFt->SetLayerID(m_nCurLayID);
				if(pFt!=NULL)
				{
					feature_list.push_back(pFt);
				}
			}
			//
			if(feature_list.size()==0)
			{
				lineftrs->clear();
				goto F2;
			}
			goto READ;
		}
		else
		{
			if(record_set->hitend())
			{
				record_set->finish();
				record_set=NULL;
				line_loaded=true;
			}
			else
			{
				line_loaded=true;
				polygon_loaded=true;
				text_loaded=true;
				return NULL;
			}
		}
	}
	//
	if(!polygon_loaded)
	{
		if(record_set==NULL)
		{
			record_set = puvs->polygonftrsInMapsheet(current_mapsheet->name);
		}
		//
		uvsclient::polygonftrSet * polygonftrs=uvsclient::polygonftrSet::NEW();
F3:		fetch_count = record_set->fetch(fetch_size,polygonftrs);
		if(fetch_count>0)
		{
			for(int i=0;i<fetch_count;++i)
			{
				uvsclient::PolygonFTR & polygon_ftr=polygonftrs->get_item(i);
				if(pds->GetFtrLayerBystrID(polygon_ftr.layer_id)==NULL)
				{
					//CString msg;
					//msg.Format(IDS_LAYER_INFO_LOSED, polygon_ftr.layer_id);
					//AfxMessageBox(msg);
					continue;
				}
				CValueTable tab;
				valuetableFromPolygonFTR(tab,polygon_ftr);
				CFeature *pFt = (CFeature*)CreateObject(tab);
				pFt->SetLayerID(m_nCurLayID);
				if(pFt!=NULL)
				{
					feature_list.push_back(pFt);
				}
			}
			if(feature_list.size()==0)
			{
				polygonftrs->clear();
				goto F3;
			}
		    goto READ;
		}
		else
		{
			if(record_set->hitend())
			{
				record_set->finish();
				record_set=NULL;
				polygon_loaded=true;
			}
			else
			{
				polygon_loaded=true;
				text_loaded=true;
				return NULL;
			}
		}
	}
	//
	if(!text_loaded)
	{
		if(record_set==NULL)
		{
			record_set = puvs->textftrsInMapsheet(current_mapsheet->name);
		}
		//
       	uvsclient::textftrSet * textftrs=uvsclient::textftrSet::NEW();
F4:		fetch_count = record_set->fetch(fetch_size,textftrs);
		if(fetch_count>0)
		{
			for(int i=0;i<fetch_count;++i)
			{
				uvsclient::TextFTR & text_ftr=textftrs->get_item(i);
				if(pds->GetFtrLayerBystrID(text_ftr.layer_id)==NULL)
				{
					//CString msg;
					//msg.Format(IDS_LAYER_INFO_LOSED, text_ftr.layer_id);
					//AfxMessageBox(msg);
					continue;
				}
				//
				CValueTable tab;
				valuetableFromTextFTR(tab,text_ftr);
				CFeature *pFt = (CFeature*)CreateObject(tab);
				pFt->SetLayerID(m_nCurLayID);
				if(pFt!=NULL)
				{
					feature_list.push_back(pFt);
				}
			}
			if(feature_list.size()==0)
			{
				textftrs->clear();
				goto F4;
			}
			goto READ;
		}
		else
		{
			if(record_set->hitend())
			{
				record_set->finish();
				record_set=NULL;
				text_loaded=true;
			}
			else
			{
				text_loaded=true;
				return NULL;
			}
		}
	}

	if (record_set)//已结束
	{
		record_set->finish();
		record_set = NULL;
	}
	return NULL;
}

int CUVSModify::GetFeatureCount()
{
	if(puvs==NULL)
	{
		return -1;
	}
    
	return puvs->featurecountInMapsheet(current_mapsheet->name);
}

int CUVSModify::GetCurFtrLayID()
{
	return m_nCurLayID;
}

int CUVSModify::GetCurFtrGrpID()
{
	return 0;
}

//读取扩展属性信息
BOOL CUVSModify::ReadXAttribute(CFeature *pFtr, CValueTable& tab)
{
	if (!pFtr) return FALSE;

	//获取地物所在图层
	CFtrLayer *pLayer = GetFtrLayerByID(pFtr->GetLayerID());
	if (!pLayer) return FALSE;
	CString fid = pFtr->GetID().ToString();
	CString layer_id = pLayer->GetstrID();
	CString attribute_name = pLayer->GetXAttributeName();
	//
	map<CStringA, uvsclient::attributefielddefineSet*>::iterator itr = layer_xattribute.find(layer_id);
	if(itr==layer_xattribute.end() || itr->second==NULL)
	{
		uvsclient::attributefielddefineSet * fields=uvsclient::attributefielddefineSet::NEW();
		puvs->GetAttributeDefinition((LPCSTR)attribute_name,fields);
		if(fields->get_count()>0)
		{
			mutex_layer_xattribute.Lock();
			layer_xattribute[layer_id]=fields;
			mutex_layer_xattribute.Unlock();
		}
		else
			return FALSE;
	}
    //
	uvsclient::StringList * values=uvsclient::StringList::NEW();
	puvs->GetAttribute(fid,layer_id, layer_xattribute[layer_id], values);
	if(values->get_count()==0)
	{
		uvsclient::attributefielddefineSet * field_defines=layer_xattribute[layer_id];
		tab.BeginAddValueItem();
		for(int i=0;i<field_defines->get_count();++i)
		{
			const uvsclient::AttributeFieldDefine& field=field_defines->get_item(i);
			CString fieldname = ConvertCharToTChar(field.field_name);
			if(CString(field.type)=="int")
			{
				tab.AddValue(fieldname, &(CVariantEx)(_variant_t)(long)atoi(field.default_value));
			}
			else if(CString(field.type)=="float")
			{
				tab.AddValue(fieldname, &(CVariantEx)(_variant_t)atof(field.default_value));
			}
			else
			{
				tab.AddValue(fieldname, &(CVariantEx)(_variant_t)field.default_value);
			}
		}
		tab.EndAddValueItem();
		//
		values->release();
		return TRUE;
	}
	if(values->get_count()!=layer_xattribute[layer_id]->get_count())
	{
		return FALSE;
	}
	//
	tab.BeginAddValueItem();
	for(int i = 0;i<values->get_count();++i)
	{	
		const uvsclient::AttributeFieldDefine& field=layer_xattribute[layer_id]->get_item(i);
		CString fieldname = ConvertCharToTChar(field.field_name);
		if(CString(field.type)=="int")
		{
			tab.AddValue(fieldname, &(CVariantEx)(_variant_t)(long)atoi(values->get_item(i)));
		}
		else if(CString(field.type)=="float")
		{
			tab.AddValue(fieldname, &(CVariantEx)(_variant_t)atof(values->get_item(i)));
		}
		else
		{
			tab.AddValue(fieldname, &(CVariantEx)(_variant_t)values->get_item(i));
		}
	}
	tab.EndAddValueItem();
	values->release();
    
	return TRUE;
}

//更新数据源信息
void CUVSModify::WriteDataSourceInfo(CDataSourceEx *pDS)
{
	if (current_mapsheet == NULL || pDS==NULL)
	{
		return;
	}

	PT_3D pts[4];
	double zmin, zmax;
	pDS->GetBound(pts, &zmin, &zmax);
	//
	uvsclient::MapSheet * map_sheet = new uvsclient::MapSheet();
	sprintf(map_sheet->name, "%s", current_mapsheet->name);
	sprintf(map_sheet->workspace_name, "%s", current_workspace->name);
	for (int i = 0; i < 4; ++i)
	{
		map_sheet->bound[i * 3] = pts[i].x;
		map_sheet->bound[i * 3 + 1] = pts[i].y;
		map_sheet->bound[i * 3 + 2] = pts[i].z;
	}
	map_sheet->zmax = zmax;
	map_sheet->zmin = zmin;
	map_sheet->scale = current_workspace->scale;
	//
	BOOL successed = puvs->SaveMapsheet(*map_sheet);
	if (successed)
	{
		for (i = 0; i < 4; ++i)
		{
			current_mapsheet->bound[i * 3] = pts[i].x;
			current_mapsheet->bound[i * 3 + 1] = pts[i].y;
			current_mapsheet->bound[i * 3 + 2] = pts[i].z;
		}
		current_mapsheet->zmax = zmax;
		current_mapsheet->zmin = zmin;
	}
	delete map_sheet;
	map_sheet = NULL;
}

void CUVSModify::vectorlayerFromValueTable(CValueTable & tab,uvsclient::VectorLayer & vector_layer)
{
	const CVariantEx *pvar=NULL;
	//
    if( tab.GetValue(0,FIELDNAME_LAYERNAME,pvar) )
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		sprintf(vector_layer.name,str);
	}
	
	if( tab.GetValue(0,FIELDNAME_LAYGROUPNAME,pvar) )
	{
		//stm.bind(3,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	
	if( tab.GetValue(0,FIELDNAME_LAYCOLOR,pvar) )
	{
		vector_layer.color=(long)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,FIELDNAME_LAYVISIBLE,pvar) )
	{
		vector_layer.visible=(long)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,FIELDNAME_LAYLOCKED,pvar) )
	{
		vector_layer.locked=(long)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,FIELDNAME_LAYINHERENT,pvar) )
	{
		vector_layer.inherent=(long)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,FIELDNAME_LAYERDISPLAYORDER,pvar) )
	{
		vector_layer.display_order=(long)(_variant_t)*pvar;
	}
	
	if( tab.GetValue(0,FIELDNAME_LAYERSYMBOLIZED,pvar) )
	{
		vector_layer.symbolized=(long)(_variant_t)*pvar;
	}
	
	if(tab.GetValue(0,FIELDNAME_LAYMAPNAME,pvar))
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
        sprintf(vector_layer.map_name,str);
	}
    
	if(tab.GetValue(0,FIELDNAME_LAYWORKSPACENAME,pvar))
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		sprintf(vector_layer.workspace_name,str);
	}
}

//更新层
BOOL CUVSModify::SaveFtrLayer(CFtrLayer *pLayer)
{
    if(CString(pLayer->GetstrID()).GetLength()<5)
	{
			GUID   guid; 
			CString   szGUID; 
	L0:	if (S_OK   ==   ::CoCreateGuid(&guid)) 
		{ 
			szGUID.Format( _T("%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X")
				,   guid.Data1 
				,   guid.Data2 
				,   guid.Data3 
				,   guid.Data4[0],   guid.Data4[1] 
				,   guid.Data4[2],   guid.Data4[3],   guid.Data4[4],   guid.Data4[5] 
				,   guid.Data4[6],   guid.Data4[7] 
				); 
			pLayer->SetstrID(szGUID);
		} 
		else
		{
			goto L0;
		}
	}
	//
	//延迟保存；
	if(!update_immediately)
	{
		shared_ptr<CFtrLayer> pl(new CFtrLayer());
		pl->CopyFrom(pLayer);
		return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>
			(new uvscashe::OPAddvectorLayer(pl,this,current_workspace->name,current_mapsheet->name)));	
	}
	//
	CValueTable tab;
	
	tab.BeginAddValueItem();
	pLayer->WriteTo(tab);
	tab.AddValue(FIELDNAME_LAYWORKSPACENAME,&(CVariantEx)(_variant_t)current_workspace->name);
	tab.AddValue(FIELDNAME_LAYMAPNAME,&(CVariantEx)(_variant_t)current_mapsheet->name);
	tab.EndAddValueItem();
	
	uvsclient::VectorLayer vector_layer;
	vector_layer.id_in_map=pLayer->GetID();
	sprintf(vector_layer.uuid,"%s",pLayer->GetstrID());
	sprintf(vector_layer.attribute_name,"%s",pLayer->GetXAttributeName());
    vectorlayerFromValueTable(tab,vector_layer);
    bool successed = puvs->SaveVectorlayer(vector_layer);
    if(!successed)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CUVSModify::DelFtrLayer(CFtrLayer *pLayer)
{
	//延迟保存；
	if(!update_immediately)
	{
		shared_ptr<CFtrLayer> pl(new CFtrLayer());
		pl->CopyFrom(pLayer);
		return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>(new uvscashe::OPDeletevectorLayer(pl,this)));	
	}
	//
	bool successed = puvs->DeleteVectorlayer(pLayer->GetstrID());
	if(successed)
	{
		return TRUE;
	}
	AfxMessageBox(ConvertCharToTChar(puvs->GetLastError()));
	return FALSE;
}

//更新编组
BOOL CUVSModify::SaveObjectGroup(ObjectGroup *ftr)
{
	//延迟保存；
	if(!update_immediately)
	{
		shared_ptr<ObjectGroup> pg(new ObjectGroup());
		pg->id=ftr->id;
		sprintf(pg->name,"%s",ftr->name);
		pg->select=ftr->select;
		return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>(new uvscashe::OPAddvectorgroup(pg,this,current_mapsheet->name)));	
	}
	//
	uvsclient::VectorGroup group;
	group.id_in_map=ftr->id;
	group.selectable=ftr->select;
	sprintf(group.name,"%s",ftr->name);
	sprintf(group.map_name,"%s",current_mapsheet->name);
	bool successed=puvs->SaveFTRGroup(group);
	if(!successed)
	{
		AfxMessageBox(ConvertCharToTChar(puvs->GetLastError()));
	}
	return successed;
}

BOOL CUVSModify::DelObjectGroup(ObjectGroup *ftr)
{
	//延迟保存；
	if(!update_immediately)
	{
		shared_ptr<ObjectGroup> pg(new ObjectGroup());
		pg->id=ftr->id;
		sprintf(pg->name,"%s",ftr->name);
		pg->select=ftr->select;
		return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>(new uvscashe::OPDeletevectorgroup(pg,this)));	
	}
	//
	bool successed=puvs->DeleteFTRGroup(ftr->name);
	if(!successed)
	{
		AfxMessageBox(ConvertCharToTChar(puvs->GetLastError()));
	}
	return successed;
}

//更新层组
BOOL CUVSModify::SaveFtrLayerGroup(FtrLayerGroup *pGroup)
{
	return FALSE;
}

BOOL CUVSModify::DelFtrLayerGroup(FtrLayerGroup *pGroup)
{
	return FALSE;
}

BOOL CUVSModify::DelAllFeature()
{
	return FALSE;
}

//更新地物
BOOL CUVSModify::SetCurFtrLayID(int id)
{
	m_nCurLayID=id;
	return TRUE;
}

BOOL CUVSModify::SetCurFtrGrpID(int id)
{
	return FALSE;
}


void CUVSModify::pointftrFromValueTable(CValueTable & tab,uvsclient::PointFTR & point_ftr)
{
	const CVariantEx *pvar=NULL;

	if(tab.GetValue(0,FIELDNAME_FTRWORKSPACENAME,pvar))
	{
        sprintf(point_ftr.workspace_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}

	if(tab.GetValue(0,FIELDNAME_FTRMAPSHEETNAME,pvar))
	{
       sprintf(point_ftr.map_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}
	
	if(tab.GetValue(0,FIELDNAME_FTRLAYERID,pvar))
	{
		int temp=(long)(_variant_t)*pvar;
		CFtrLayer *pLayer = GetFtrLayerByID(temp);
		if (pLayer)
		{
			sprintf(point_ftr.layer_id, "%s", pLayer->GetstrID());
		}
	}

	if(tab.GetValue(0,FIELDNAME_FTRID,pvar))
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		sprintf(point_ftr.fid,str);
	}
	else
	{
		return;
	}

	if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		point_ftr.class_type=(long)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.class_type=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		point_ftr.geo_class=(long)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.geo_class=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		point_ftr.color=(long)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.color=0;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		point_ftr.symbolize_flag=(long)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.symbolize_flag=0;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		sprintf(point_ftr.symbol_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(point_ftr.symbol_name,"%s","");
	}
	
	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		
		point_ftr.shape_size=arrPts.GetSize();
		point_ftr.shape=new uvsclient::Vec_3DPTEX[point_ftr.shape_size];
		if(point_ftr.shape==NULL)
			return;
        for(int i=0;i<point_ftr.shape_size;++i)
		{
			point_ftr.shape[i].x=arrPts[i].x;
			point_ftr.shape[i].y=arrPts[i].y;
			point_ftr.shape[i].z=arrPts[i].z;
			point_ftr.shape[i].wid=arrPts[i].wid;
			point_ftr.shape[i].pencode=arrPts[i].pencode;
			point_ftr.shape[i].type=arrPts[i].type;
		}
	}
	else
	{
		point_ftr.shape_size=0;
		point_ftr.shape=NULL;
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_ANGLE,pvar) )
	{
		point_ftr.angle=(double)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.angle = 0.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		sprintf(point_ftr.code,str);
	}
	else
	{
	    sprintf(point_ftr.code,"");
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		point_ftr.visible=(bool)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.visible = 1;
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_KX,pvar) )
	{
		point_ftr.kx=(double)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.kx = 0.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_KY,pvar) )
	{
		point_ftr.ky=(double)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.ky = 0.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_COVERTYPE,pvar) )
	{
		point_ftr.covertype=(long)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.covertype=0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_EXTENDDIS,pvar) )
	{
		point_ftr.extenddis=(double)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.extenddis=0.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		point_ftr.purpose=(long)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.purpose=0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOPOINT_WIDTH,pvar) )
	{
		point_ftr.width=(double)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.width=0.0;
	}

	point_ftr.length=0.0;

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		point_ftr.display_order=(double)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.display_order=-1;
	}

	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		if(pBuf==NULL)
		{
           sprintf(point_ftr.group_name,"");
		}
		else
		{
			string str_blob=toHexString(pBuf,nLen);
            sprintf(point_ftr.group_name,"%s",str_blob.c_str());
		}
		
	}
	else
	{
		sprintf(point_ftr.group_name,"%s","");
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		point_ftr.modify_time=(long)(_variant_t)*pvar;
	}
	else
	{
		point_ftr.modify_time=0;
	}
}

void CUVSModify::lineftrFromValueTable(CValueTable & tab,uvsclient::LineFTR & line_ftr)
{
	const CVariantEx *pvar = NULL;

	if(tab.GetValue(0,FIELDNAME_FTRWORKSPACENAME,pvar))
	{
        sprintf(line_ftr.workspace_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}
	
	if(tab.GetValue(0,FIELDNAME_FTRMAPSHEETNAME,pvar))
	{
		sprintf(line_ftr.map_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}
	
	if(tab.GetValue(0,FIELDNAME_FTRLAYERID,pvar))
	{
		int temp=(long)(_variant_t)*pvar;
		CFtrLayer *pLayer = GetFtrLayerByID(temp);
		if (pLayer)
		{
			sprintf(line_ftr.layer_id, "%s", pLayer->GetstrID());
		}
	}

	if(tab.GetValue(0,FIELDNAME_FTRID,pvar))
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		sprintf(line_ftr.fid,str);
	}
	else
	{
		return;
	}

	if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		line_ftr.class_type=(long)(_variant_t)*pvar;
	}
	else
	{
	    line_ftr.class_type=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		line_ftr.geo_class=(long)(_variant_t)*pvar;
	}
	else
	{
	    line_ftr.geo_class=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		line_ftr.color=(long)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.color=0;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		line_ftr.symbolize_flag=(long)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.symbolize_flag = 256;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		sprintf(line_ftr.symbol_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(line_ftr.symbol_name,"%s","");
	}
	
	
	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		line_ftr.shape_size=arrPts.GetSize();
		//if(line_ftr.shape_size == 0)
			//AfxMessageBox("错误的线数据");
		line_ftr.shape=new uvsclient::Vec_3DPTEX[line_ftr.shape_size];
		if(line_ftr.shape==NULL)
			return;
		for(int i=0;i<line_ftr.shape_size;++i)
		{
			line_ftr.shape[i].x=arrPts[i].x;
			line_ftr.shape[i].y=arrPts[i].y;
			line_ftr.shape[i].z=arrPts[i].z;
			line_ftr.shape[i].wid=arrPts[i].wid;
			line_ftr.shape[i].pencode=arrPts[i].pencode;
			line_ftr.shape[i].type=arrPts[i].type;
		}
	}
	else
	{
		line_ftr.shape_size=0;
		line_ftr.shape=NULL;
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_WIDTH,pvar) )
	{
		line_ftr.width=(double)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.width=0.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_DHEIGHT,pvar) )
	{
		line_ftr.dheight=(double)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.dheight=0.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		sprintf(line_ftr.code,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(line_ftr.code,"%s","");
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		line_ftr.visible=(bool)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.visible=1;
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINETYPESCALE,pvar) )
	{
		line_ftr.line_type_scale=(double)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.line_type_scale = 1.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINEWIDTHSCALE,pvar) )
	{
		line_ftr.line_width_scale=(double)(_variant_t)*pvar;
	}
	else
	{
	    line_ftr.line_width_scale = 1.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		line_ftr.purpose=(long)(_variant_t)*pvar;
	}
	else
	{
	    line_ftr.purpose=0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINEWIDTH,pvar) )
	{
		line_ftr.line_width=(double)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.line_width=0.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINETYPEXOFF,pvar) )
	{
		line_ftr.line_type_xoff=(double)(_variant_t)*pvar;
	}
	else
	{
	    line_ftr.line_type_xoff = 0.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		line_ftr.display_order=(double)(_variant_t)*pvar;
	}
	else
	{
	    line_ftr.display_order = -1;
	}

	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		if(pBuf==NULL)
		{
	        sprintf(line_ftr.group_name,"%s","");
		}
		else
		{
			string str_blob=toHexString(pBuf,nLen);
		    sprintf(line_ftr.group_name,"%s",str_blob.c_str());
		}
	}
	else
	{
		sprintf(line_ftr.group_name,"%s","");
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		line_ftr.modify_time=(long)(_variant_t)*pvar;
	}
	else
	{
		line_ftr.modify_time=0;
	}
    //
	line_ftr.geo=NULL;
	line_ftr.geo_size=0;
}

void CUVSModify::polygonftrFromValueTable(CValueTable & tab,uvsclient::PolygonFTR & polygon_ftr)
{
	const CVariantEx *pvar = NULL;
	
	if(tab.GetValue(0,FIELDNAME_FTRWORKSPACENAME,pvar))
	{
        sprintf(polygon_ftr.workspace_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}
	
	if(tab.GetValue(0,FIELDNAME_FTRMAPSHEETNAME,pvar))
	{
		sprintf(polygon_ftr.map_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}
	
	if(tab.GetValue(0,FIELDNAME_FTRLAYERID,pvar))
	{
		int temp=(long)(_variant_t)*pvar;
		CFtrLayer *pLayer = GetFtrLayerByID(temp);
		if (pLayer)
		{
			sprintf(polygon_ftr.layer_id, "%s", pLayer->GetstrID());
		}
	}

	if(tab.GetValue(0,FIELDNAME_FTRID,pvar))
	{
		sprintf(polygon_ftr.fid,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}

    if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		polygon_ftr.class_type=(long)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.class_type=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		polygon_ftr.geo_class=(long)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.geo_class=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		polygon_ftr.color=(long)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.color=0;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		polygon_ftr.symbolize_flag=(long)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.symbolize_flag=0;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		sprintf(polygon_ftr.symbol_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(polygon_ftr.symbol_name,"%s","");
	}
	
	
	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		polygon_ftr.shape_size=arrPts.GetSize();
		polygon_ftr.shape=new uvsclient::Vec_3DPTEX[polygon_ftr.shape_size];
		if(polygon_ftr.shape==NULL)
			return;

		if (arrPts[0].pencode == penMove)
		{
			arrPts[0].pencode = penNone;
		}
		for(int i=0;i<polygon_ftr.shape_size;++i)
		{
			polygon_ftr.shape[i].x=arrPts[i].x;
			polygon_ftr.shape[i].y=arrPts[i].y;
			polygon_ftr.shape[i].z=arrPts[i].z;
			polygon_ftr.shape[i].wid=arrPts[i].wid;
			polygon_ftr.shape[i].pencode=arrPts[i].pencode;
			polygon_ftr.shape[i].type=arrPts[i].type;
		}
	}
	else
	{
		polygon_ftr.shape=NULL;
		polygon_ftr.shape_size=0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		sprintf(polygon_ftr.code,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(polygon_ftr.code,"%s","");
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		polygon_ftr.visible=(bool)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.visible=1;
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_CELLANGLE,pvar) )
	{
		polygon_ftr.cell_angle=(double)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.cell_angle = 0.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_CELLSCALE,pvar) )
	{
		polygon_ftr.cell_scale=(double)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.cell_scale = 1.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_INTVSCALE,pvar) )
	{
		polygon_ftr.intv_scale=(double)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.intv_scale = 1.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_XSTARTOFF,pvar) )
	{
		polygon_ftr.xstartoff=(double)(_variant_t)*pvar;
	}
	else
	{
	     polygon_ftr.xstartoff = 0.0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_YSTARTOFF,pvar) )
	{
		polygon_ftr.ystartoff=(double)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.ystartoff = 0.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		polygon_ftr.purpose=(long)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.purpose=0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOSURFACE_LINEWIDTH,pvar) )
	{
		polygon_ftr.line_width=(long)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.line_width=0.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		polygon_ftr.display_order=(double)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.display_order = -1;
	}
	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		if(pBuf==NULL)
		{
            sprintf(polygon_ftr.group_name,"%s","");
		}
		else
		{
			string str_blob=toHexString(pBuf,nLen);
	     	sprintf(polygon_ftr.group_name,"%s",str_blob.c_str());
		}
	}
	else
	{
		sprintf(polygon_ftr.group_name,"%s","");
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		polygon_ftr.modify_time=(long)(_variant_t)*pvar;
	}
	else
	{
		polygon_ftr.modify_time=0;
	}
	//
	polygon_ftr.geo=NULL;
	polygon_ftr.geo_size=0;
}

void CUVSModify::textftrFromValueTable(CValueTable & tab,uvsclient::TextFTR & text_ftr)
{
	const CVariantEx *pvar = NULL;
	
	if(tab.GetValue(0,FIELDNAME_FTRWORKSPACENAME,pvar))
	{
        sprintf(text_ftr.workspace_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}
	
	if(tab.GetValue(0,FIELDNAME_FTRMAPSHEETNAME,pvar))
	{
		sprintf(text_ftr.map_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}

	if(tab.GetValue(0,FIELDNAME_FTRLAYERID,pvar))
	{
		int temp=(long)(_variant_t)*pvar;
		CFtrLayer *pLayer = GetFtrLayerByID(temp);
		if (pLayer)
		{
			sprintf(text_ftr.layer_id, "%s", pLayer->GetstrID());
		}
	}

	if(tab.GetValue(0,FIELDNAME_FTRID,pvar))
	{
		sprintf(text_ftr.fid,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		return;
	}

    if( tab.GetValue(0,FIELDNAME_CLSTYPE,pvar) )
	{
		text_ftr.class_type=(long)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.class_type=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		text_ftr.geo_class=(long)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.geo_class=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		text_ftr.color=(long)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.color=0;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		text_ftr.symbolize_flag=(long)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.symbolize_flag=0;
	}

	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		sprintf(text_ftr.symbol_name,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(text_ftr.symbol_name,"%s","");
	}
	
	if( tab.GetValue(0,FIELDNAME_SHAPE,pvar) )
	{
		CArray<PT_3DEX, PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		text_ftr.shape_size = arrPts.GetSize();
		text_ftr.shape = new uvsclient::Vec_3DPTEX[text_ftr.shape_size];
		if (text_ftr.shape == NULL)
			return;

		for (int i = 0; i < text_ftr.shape_size; ++i)
		{
			text_ftr.shape[i].x = arrPts[i].x;
			text_ftr.shape[i].y = arrPts[i].y;
			text_ftr.shape[i].z = arrPts[i].z;
			text_ftr.shape[i].wid = arrPts[i].wid;
			text_ftr.shape[i].pencode = arrPts[i].pencode;
			text_ftr.shape[i].type = arrPts[i].type;
		}
	}
	else
	{
		return;
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CONTENT,pvar) )
	{
		sprintf(text_ftr.content,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(text_ftr.content,"%s","");
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_FONT,pvar) )
	{
		sprintf(text_ftr.font,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
	    sprintf(text_ftr.font,"%s","");
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARWIDTHS,pvar) )
	{
		text_ftr.char_width=(double)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.char_width=0.0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARHEIGHT,pvar) )
	{
		text_ftr.char_height=(double)(_variant_t)*pvar;
	}	
	else
	{
		text_ftr.char_height = 0.0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARINTVS,pvar) )
	{
		text_ftr.char_intvr=(double)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.char_intvr = 0.0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_LINEINTVS,pvar) )
	{
		text_ftr.line_intvr=(double)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.line_intvr = 0.0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_ALIGNTYPE,pvar) )
	{
		text_ftr.align_type=(long)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.align_type=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_TEXTANGLE,pvar) )
	{
		text_ftr.text_angle=(double)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.text_angle = 0.0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARANGLE,pvar) )
	{
		text_ftr.char_angle=(double)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.char_angle = 0.0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINE,pvar) )
	{
		text_ftr.incline=(long)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.incline=0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINEANGLE,pvar) )
	{
		text_ftr.incline_angle=(double)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.incline_angle = 0.0;
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_PLACETYPE,pvar) )
	{
		text_ftr.place_type=(long)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.place_type=0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_OTHERFLAG,pvar) )
	{
		text_ftr.other_flag=(long)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.other_flag=0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		sprintf(text_ftr.code,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		sprintf(text_ftr.code,"%s","");
	}

	if( tab.GetValue(0,FIELDNAME_FTRVISIBLE,pvar) )
	{
		text_ftr.visible=(bool)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.visible=1;
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_COVERTYPE,pvar) )
	{
		text_ftr.cover_type=(long)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.cover_type=0;
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_EXTENDDIS,pvar) )
	{
		text_ftr.extenddis=(double)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.extenddis = 1.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		text_ftr.purpose=(long)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.purpose=0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRDISPLAYORDER,pvar) )
	{
		text_ftr.display_order=(double)(_variant_t)*pvar;
	}
	else
	{
		text_ftr.display_order = 1.0;
	}

	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{		
		int nLen;
		const BYTE *pBuf = pvar->GetBlob(nLen);
		if(pBuf==NULL)
		{
            sprintf(text_ftr.group_name,"%s","");
		}
		else
		{
			string str_blob=toHexString(pBuf,nLen);
		    sprintf(text_ftr.group_name,"%s",str_blob.c_str());
		}
	}
	else
	{
	    sprintf(text_ftr.group_name,"%s","");
	}
	
	if( tab.GetValue(0,FIELDNAME_FTRMODIFYTIME,pvar) )
	{
		text_ftr.modify_time=(long)(_variant_t)*pvar;
	}
	else
	{
	    text_ftr.modify_time=0;
	}
}

BOOL CUVSModify::SaveFeature(CFeature *pFt, CValueTable *exAttriTab)
{
	//延迟保存；
	if(!update_immediately)
	{
		shared_ptr<CFeature> pftr(new CFeature());
		pftr->CopyFrom(pFt);
		return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>
			(new uvscashe::OPAddFeature(pftr,this,m_nCurLayID,current_mapsheet->name,current_workspace->name)));
	}
	//
	CGeometry *pGeo = pFt->GetGeometry();
	if (!pGeo)
		return FALSE;
	int nGeoCls = pGeo->GetClassType();
	
	//有可能插件中会创建派生类
	if (nGeoCls == CLS_GEOPOINT || nGeoCls == CLS_GEODIRPOINT || nGeoCls == CLS_GEOSURFACEPOINT || nGeoCls == CLS_GEOMULTIPOINT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
	{
		CValueTable tab;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)current_mapsheet->name);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)current_workspace->name);
		tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)m_nCurLayID);
		tab.EndAddValueItem();

		uvsclient::PointFTR point_ftr;
		pointftrFromValueTable(tab,point_ftr);
        bool successed=puvs->SavePointFTR(point_ftr);
		if(point_ftr.shape!=NULL)
		{
           delete [] point_ftr.shape;
		   point_ftr.shape = NULL;
		}

		if(!successed)
		{
			return FALSE;
		}

		return TRUE;
	}
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT ||
		pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		CValueTable tab;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)current_mapsheet->name);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)current_workspace->name);
		tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)m_nCurLayID);
		tab.EndAddValueItem();
		
		uvsclient::LineFTR line_ftr;
		lineftrFromValueTable(tab,line_ftr);
		bool successed = successed=puvs->SaveLineFTR(line_ftr);
		if(line_ftr.shape!=NULL)
		{
           delete [] line_ftr.shape;
		   line_ftr.shape = NULL;
		}
		if(line_ftr.geo!=NULL)
		{
			delete [] line_ftr.geo;
			line_ftr.geo = NULL;
		}

		if(!successed)
		{

			//AfxMessageBox(puvs->GetLastError());

			return FALSE;
		}

		return TRUE;
	}
	else if( nGeoCls==CLS_GEOSURFACE ||nGeoCls==CLS_GEOMULTISURFACE ||  pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		CValueTable tab;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)current_mapsheet->name);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)current_workspace->name);
		tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)m_nCurLayID);
		tab.EndAddValueItem();
		
		uvsclient::PolygonFTR polygon_ftr;
        polygonftrFromValueTable(tab,polygon_ftr);
		bool successed = puvs->SavePolygonFTR(polygon_ftr);
		if(polygon_ftr.shape!=NULL)
		{
			delete [] polygon_ftr.shape;
			polygon_ftr.shape = NULL;
		}
		if(polygon_ftr.geo!=NULL)
		{
			delete [] polygon_ftr.geo;
			polygon_ftr.geo = NULL;
		}
		
		if(!successed)
		{
			//AfxMessageBox(puvs->GetLastError());
			return FALSE;
		}
		return TRUE;
	}
	else if( nGeoCls==CLS_GEOTEXT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		CValueTable tab;
		CVariantEx *pvar=NULL;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)current_mapsheet->name);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)current_workspace->name);
		tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)m_nCurLayID);
		tab.EndAddValueItem();
		
		uvsclient::TextFTR text_ftr;
		textftrFromValueTable(tab,text_ftr);
		bool successed = puvs->SaveTextFTR(text_ftr);
		if(!successed)
		{
			//AfxMessageBox(puvs->GetLastError());
			return FALSE;
		}

		return TRUE;
	}
	else
		return FALSE;
	
	return TRUE;
}

BOOL CUVSModify::RestoreFeature(CFeature *pFt)
{
	return TRUE;
}

BOOL CUVSModify::DelFeature(CFeature *pFt)
{
	//延迟保存；
	if(!update_immediately)
	{
		shared_ptr<CFeature> pftr(new CFeature());
		pftr->CopyFrom(pFt);
        return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>
			(new uvscashe::OPDeleteFeature(pftr,this,m_nCurLayID,current_mapsheet->name,current_workspace->name)));
	}
	//
	uvsclient::FeatureItem * pftr=new uvsclient::FeatureItem();
	if(pftr==NULL)
		return FALSE;
    
	CValueTable tab;
	tab.BeginAddValueItem();
	pFt->WriteTo(tab);
	tab.EndAddValueItem();
	const CVariantEx *pvar = NULL;
	if(tab.GetValue(0,FIELDNAME_FTRID,pvar))
	{
		sprintf(pftr->fid,"%s",(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		delete pftr;
		pftr = NULL;
		return FALSE;
	}
    if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		pftr->geo_class=(long)(_variant_t)*pvar;
	}
	else
	{
		delete pftr;
		pftr = NULL;
		return FALSE;
	}
    sprintf(pftr->map_name,current_mapsheet->name);
	CFtrLayer *pLayer = GetFtrLayerByID(pFt->GetLayerID());
	if (pLayer)
	{
		sprintf(pftr->layer_id, "%s", pLayer->GetstrID());
	}
	else
	{
		delete pftr;
		pftr = NULL;
	    return FALSE;
	}

	bool deleted=puvs->DeleteFeature(pftr,1);
	if(!deleted)
	{
	   AfxMessageBox(ConvertCharToTChar(puvs->GetLastError()));
	   delete pftr;
	   pftr = NULL;
	   return FALSE;
	}

	delete pftr;
	pftr = NULL;
    return TRUE;
}

BOOL CUVSModify::DelXAttributes(CFeature *pFtr)
{
	if (!pFtr) return FALSE;

	CString fid = pFtr->GetID().ToString();
	CFtrLayer *pLayer = GetFtrLayerByID(pFtr->GetLayerID());
	if (!pLayer) return FALSE;
	CString layer_id = pLayer->GetstrID();

	//延迟保存；
	if (!update_immediately)
	{
		return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>
			(new uvscashe::OPDeleteXAttribute(fid, layer_id, this)));
	}

	return puvs->DeleteAttribute(fid, layer_id); //只是让数据库里面的deleted字段设为1
}

BOOL CUVSModify::RestoreXAttributes(CFeature *pFtr)
{
	CValueTable tab;
	return SaveXAttribute(pFtr,tab,0);//空的tab，只是让数据库里面的deleted字段设为0
}

//更新扩展属性信
BOOL CUVSModify::SaveXAttribute(CFeature *pFtr, CValueTable& tab, int idx)
{
	if (!pFtr || tab.GetFieldCount()<=0)
		return FALSE;
	CGeometry *pGeo = pFtr->GetGeometry();
	if (!pGeo) return FALSE;

	CString fid = pFtr->GetID().ToString();
	CFtrLayer *pLayer = GetFtrLayerByID(pFtr->GetLayerID());
	if (!pLayer) return FALSE;
	CString layer_id = pLayer->GetstrID();
	CString attribute_name = pLayer->GetXAttributeName();

	//获取UVS属性定义
	map<CStringA, uvsclient::attributefielddefineSet*>::iterator itr = layer_xattribute.find(layer_id);
	uvsclient::attributefielddefineSet * pfields = NULL;
	if (itr == layer_xattribute.end() || itr->second == NULL)
	{
		uvsclient::attributefielddefineSet * pfields1 = uvsclient::attributefielddefineSet::NEW();
		puvs->GetAttributeDefinition((LPCSTR)attribute_name, pfields1);
		if (pfields1->get_count() > 0)
		{
			mutex_layer_xattribute.Lock();
			layer_xattribute[layer_id] = pfields1;
			mutex_layer_xattribute.Unlock();
			pfields = pfields1;
		}
	}
	else
	{
		pfields = itr->second;
	}

	if (pfields == NULL)
		return FALSE;

	//延迟保存；
	if (!update_immediately)
	{
		shared_ptr<CValueTable> ptab(new CValueTable());
		ptab->CopyFrom(tab);
		return CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>
			(new uvscashe::OPAddXAttribute(fid, layer_id, 0, pfields, ptab, this, idx)));
	}

	//
	uvsclient::AttributeItem * attri_item = new uvsclient::AttributeItem();
	sprintf(attri_item->fid, "%s", fid);
	sprintf(attri_item->layer_id, "%s", layer_id);
	attri_item->deleted = 0;
	attri_item->fields = uvsclient::StringList::NEW();
	attri_item->values = uvsclient::StringList::NEW();

	const CVariantEx *var = NULL;
	for (int i = 0; i<pfields->get_count(); ++i)
	{
		CString field = pfields->get_item(i).field_name;
		CStringA str_value;
		if(tab.GetValue(idx, field, var))
		{
			attri_item->fields->add_item(field);
			VariantToTextA(*var, str_value);
			attri_item->values->add_item(str_value);
		}
	}
	//
	BOOL successed = puvs->SaveAttribute(*attri_item);
	attri_item->fields->release();
	attri_item->values->release();
	delete attri_item;
	attri_item = NULL;
	//
	return successed;
}

//更新可选属性（可有可无的属性）
//创建可选属性；可以创建多次，只会累加，不会删除；sqlite也不支持删除属性列；
BOOL CUVSModify::CreateOptAttributes(int nGeoCls, CStringArray& fields, CStringArray& field_types)
{
	return FALSE;
}

BOOL CUVSModify::DelOptAttributes(CFeature *pFtr)
{
	return FALSE;
}

BOOL CUVSModify::RestoreOptAttributes(CFeature *pFtr)
{
	return FALSE;
}

BOOL CUVSModify::SaveOptAttributes(CFeature *pFtr, CValueTable& tab,int idx)
{
	return FALSE;
}

BOOL CUVSModify::ReadOptAttributes(CFeature *pFtr, CValueTable& tab)
{
	return FALSE;
}

BOOL CUVSModify::GetOptAttributeDef(int nGeoCls, CStringArray& fields, CStringArray& field_types)
{
	return FALSE;
}

void CUVSModify::BatchUpdateBegin()
{
	//延迟保存；
	if(!update_immediately)
	{
	    CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>(new uvscashe::OPStartTransaction(this)));
		return ;
	}
    puvs->StartTransaction();
}

void CUVSModify::BatchUpdateEnd()
{
	//延迟保存；
	if(!update_immediately)
	{
		CUVSCashe::AddOperate(shared_ptr<uvscashe::BasicOperate>(new uvscashe::OPEndTransaction(this)));
		return ;
	}
    puvs->EndTransaction();
}

void CUVSModify::SaveDataSettings(LPCTSTR field, LPCTSTR name, LPCTSTR value, LPCTSTR type)
{

}

BOOL CUVSModify::DelDataSettings(LPCTSTR field)
{
	return FALSE;
}

BOOL CUVSModify::ReadDataSettings(LPCTSTR field, CString &name, CString& value, CString &type)
{
	return FALSE;
}

BOOL CUVSModify::ReadFilePreViewImage(LPCTSTR name, BYTE *&img, int &len)
{
	return FALSE;
}

BOOL CUVSModify::DelFilePreViewImage(LPCTSTR name)
{
	return FALSE;
}

void CUVSModify::SaveFilePreViewImage(LPCTSTR name, BYTE *img, int len)
{
	
}

//读写存储在FDB中的单元
BOOL CUVSModify::GetFirstCellDef(CellDef& def)
{
	return FALSE;
}

BOOL CUVSModify::GetNextCellDef(CellDef& def)
{
	return FALSE;
}

BOOL CUVSModify::SaveCellDef(CellDef& def)
{
	return FALSE;
}

BOOL CUVSModify::DelCellDef(LPCTSTR name)
{
	return FALSE;
}

BOOL CUVSModify::DelAllCellDefs()
{
	return FALSE;
}

BOOL CUVSModify::IsFileVersionLatest()
{
	return TRUE;
}

CString CUVSModify::GetFileVersion()
{
	return "FeatureOne4.1_0001";
}

CString CUVSModify::GetWorkspaceScale()
{
	if (current_workspace)
	{
		CString text;
		text.Format("%s:%d", current_workspace->description, current_workspace->scale);
		return text;
	}
	else
	{
		return CString();
	}
}


bool CUVSModify::IsMapsheetExist(LPCTSTR map_name)
{
	if (puvs)
	{
		return puvs->IsMapsheetExist(map_name);
	}

	return false;
}
