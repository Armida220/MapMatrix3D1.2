// UVSCashe.cpp: implementation of the CUVSCashe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UVSCashe.h"
#include "UVSModify.h"
#include "ClientDataType.h"
#include "DataSourceEx.h"
#include "Scheme.h"
#include "GeoPoint.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "SmartViewFunctions.h"
#include "ExMessage.h"
#include "FBHelperFunc.h"
#include "Functions_temp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace uvscashe
{

CWinThread * CUVSCashe::execute_thread=NULL;
BOOL CUVSCashe::execute=TRUE;
BOOL CUVSCashe::executing=FALSE;
int CUVSCashe::max_stack=-1;
uvsclient::IUVSClient * CUVSCashe::puvs=NULL;
CMutex CUVSCashe::mutex_execute;
CMutex CUVSCashe::mutex_list;
CMutex CUVSCashe::mutex_puvs;
list< shared_ptr<BasicOperate> > CUVSCashe::operate_list;
int CUVSCashe::operate_count=0;

static void SetAfterOperateChange(int count)
{
	::PostMessage(AfxGetMainWnd()->m_hWnd, FCCM_UVS_OPERATECOUNT_CHANGE, count, 0);
}

BOOL CUVSCashe::ConnectUVSServer(LPCTSTR ip, int port)
{
	//创建一个用于缓存的连接
	puvs = uvsclient::IUVSClient::NEW();
	if (!puvs->Connect(ip, port))
	{
		puvs->Release();
		puvs = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL CUVSCashe::OpenDB(LPCTSTR db_name, LPCTSTR user_name, LPCTSTR password, CString & identity_id)
{
	if (!puvs) return FALSE;
	uvsclient::DatasourceConfig cfg;
	sprintf(cfg.datasource_name, "%s", db_name);
	sprintf(cfg.user_name, "%s", user_name);
	sprintf(cfg.password, "%s", password);
	sprintf(cfg.identity_id, "%s", (LPCTSTR)identity_id);
	cfg.connect_timeout = 5;
	if (!puvs->OpenDatasource(cfg))
	{
		identity_id = "";
		return FALSE;
	}

	identity_id = cfg.identity_id;
	puvs->FlushImmediately(false);
	puvs->SetTransInfo((LPCTSTR)identity_id);
	execute_thread = AfxBeginThread(ExecuteThreadFunc, NULL);
	return TRUE;
}

void CUVSCashe::ReleaseUVSServer()
{
	if (puvs)
	{
		puvs->CloseDatasource();
		puvs->Release();
		puvs = NULL;
	}
}

void CUVSCashe::SetCurrentWorkspace(uvsclient::WorkSpace& wk)
{
	mutex_puvs.Lock();
	if (puvs)
	{
		puvs->SetCurrentWorkspace(wk);
	}
	mutex_puvs.Unlock();
}

void CUVSCashe::StartExecute()
{
	mutex_execute.Lock();
	execute=TRUE;
	mutex_execute.Unlock();
	//
	while(!executing)
	{
		Sleep(100);
	}
}

void CUVSCashe::StopExecute()
{
	mutex_execute.Lock();
	execute=FALSE;
	mutex_execute.Unlock();
	//
	while(executing)
	{
		Sleep(100);
	}
}

void CUVSCashe::SetMaxStack(int _max_stack)
{
	mutex_list.Lock();
	max_stack=_max_stack;
	mutex_list.Unlock();
}

int CUVSCashe::GetMaxStack()
{
	int temp;
    mutex_list.Lock();
	temp = max_stack;
	mutex_list.Unlock();
	return temp;
}

UINT CUVSCashe::ExecuteThreadFunc(LPVOID pParam)
{
	while (TRUE)
	{
		mutex_execute.Lock();
		if (!execute)
		{
			executing = FALSE;
			Sleep(50);
			mutex_execute.Unlock();
			continue;
		}
		executing = TRUE;
		mutex_execute.Unlock();
		if (!puvs) break;
		//
		int current_count;
		mutex_list.Lock();
		current_count = operate_count;
		mutex_list.Unlock();
		//
		if (current_count > 0)
		{
			mutex_list.Lock();
			shared_ptr<BasicOperate> poperate = *operate_list.begin();
			OPERATETYPE current_type = poperate->operate_type;
			operate_list.erase(operate_list.begin());
			--operate_count;
			mutex_list.Unlock();
			//
			mutex_puvs.Lock();
			poperate->execute(puvs);
			mutex_puvs.Unlock();
			//
			if (!puvs->IsFlushImmediately() && (puvs->GetOperateWaiteCount() >= 100
				|| current_type == END_TRANSACTION || current_type == ADD_LAYER || current_type == DEL_LAYER))
			{
				mutex_puvs.Lock();
				puvs->Flush();
				mutex_puvs.Unlock();
			}
			//
			SetAfterOperateChange(current_count - 1);
		}
		else
		{
			mutex_puvs.Lock();
			bool bRet = puvs->Test(); //当没有操作时不停的向UVS发送存在消息
			mutex_puvs.Unlock();
			if (bRet == false)
			{
				GOutPut(StrFromLocalResID(IDS_UVS_DISCONNECT));
			}
			Sleep(200);
		}
	}
	return 1;
}

BOOL CUVSCashe::AddOperate(shared_ptr<BasicOperate> operate)
{
	if (max_stack < 0)
	{
		mutex_list.Lock();
		operate_list.push_back(operate);
		++operate_count;
		int temp_count = operate_count;
		mutex_list.Unlock();
		//
		SetAfterOperateChange(temp_count);
	}
	else
	{
		while (true)
		{
			mutex_list.Lock();
			if (operate_count < max_stack)
			{
				operate_list.push_back(operate);
				++operate_count;
				int temp_count = operate_count;
				mutex_list.Unlock();
				//
				SetAfterOperateChange(temp_count);
				//
				return TRUE;
			}
			else
			{
				mutex_list.Unlock();
				AfxMessageBox("FeatureOne is busy,please wait!");
				float sleep_time = max_stack / 4.0;
				if (sleep_time > 1)
					Sleep(sleep_time * 25);
				else
					Sleep(50);
			}
		}
	}

	return TRUE;
}


void CUVSCashe::WaiteExecuteAll()
{
	mutex_list.Lock();
	int temp_count = operate_count;
	mutex_list.Unlock();
	//
	while(temp_count)
	{
		Sleep(100);
		mutex_list.Lock();
		temp_count = operate_count;
		mutex_list.Unlock();
	}
}

///// OPStartTransaction //////////////////////////////////////////////////
BOOL OPStartTransaction::execute(uvsclient::IUVSClient *puvs)
{
	puvs->StartTransaction();
	return TRUE;
}

///// OPEndTransaction //////////////////////////////////////////////////
BOOL OPEndTransaction::execute(uvsclient::IUVSClient *puvs)
{
	BOOL res = puvs->EndTransaction();
	return res;
}

///// OPAddFeature //////////////////////////////////////////////////
OPAddFeature::~OPAddFeature()
{
}

BOOL OPAddFeature::execute(uvsclient::IUVSClient *puvs)
{
	CGeometry *pGeo = pFt->GetGeometry();
	if(pGeo==NULL)
		return FALSE;
	int nGeoCls = pGeo->GetClassType();

	//有可能插件中会创建派生类
	if( nGeoCls==CLS_GEOPOINT || nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		CValueTable tab;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)workspace_name);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)map_name);
	    tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)layer_id);
		tab.EndAddValueItem();

		uvsclient::PointFTR point_ftr;
		puvsmodify->pointftrFromValueTable(tab,point_ftr);
        bool successed=puvs->SavePointFTR(point_ftr);
		if(point_ftr.shape!=NULL)
		{
           delete [] point_ftr.shape;
		   point_ftr.shape = NULL;
		}

		return TRUE;
	}
	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL || nGeoCls==CLS_GEODEMPOINT ||
		pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		CValueTable tab;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)workspace_name);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)map_name);
  	    tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)layer_id);
		tab.EndAddValueItem();
		
		uvsclient::LineFTR line_ftr;
		puvsmodify->lineftrFromValueTable(tab,line_ftr);
	    bool successed=puvs->SaveLineFTR(line_ftr);
		if(!successed)
//			AfxMessageBox("错误的线数据");
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

		return TRUE;
	}
	else if( nGeoCls==CLS_GEOSURFACE ||nGeoCls==CLS_GEOMULTISURFACE ||  pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		CValueTable tab;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)workspace_name);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)map_name);
	    tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)layer_id);
		tab.EndAddValueItem();
		
		uvsclient::PolygonFTR polygon_ftr;
        puvsmodify->polygonftrFromValueTable(tab,polygon_ftr);
		bool successed=puvs->SavePolygonFTR(polygon_ftr);
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

		return TRUE;
	}
	else if( nGeoCls==CLS_GEOTEXT || pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		CValueTable tab;
		CVariantEx *pvar=NULL;
		
		tab.BeginAddValueItem();
		pFt->WriteTo(tab);
		tab.AddValue(FIELDNAME_FTRWORKSPACENAME,&(CVariantEx)(_variant_t)workspace_name);
		tab.AddValue(FIELDNAME_FTRMAPSHEETNAME,&(CVariantEx)(_variant_t)map_name);
	    tab.AddValue(FIELDNAME_FTRLAYERID,&(CVariantEx)(_variant_t)(long)layer_id);
		tab.EndAddValueItem();
		
		uvsclient::TextFTR text_ftr;
		puvsmodify->textftrFromValueTable(tab,text_ftr);
		bool successed=puvs->SaveTextFTR(text_ftr);

		return TRUE;
	}
	
	return FALSE;
}

///// OPDeleteFeature //////////////////////////////////////////////////
BOOL OPDeleteFeature::execute(uvsclient::IUVSClient *puvs)
{
	uvsclient::FeatureItem * pftr=new uvsclient::FeatureItem();
    if(pftr==NULL)
		return FALSE;

	CValueTable tab;
	tab.BeginAddValueItem();
	pFt->WriteTo(tab);
	tab.EndAddValueItem();
    const CVariantEx *pvar=NULL;
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
	
	CFtrLayer *pLayer = puvsmodify->GetFtrLayerByID(layer_id);
	if (pLayer)
	{
		sprintf(pftr->layer_id,"%s",pLayer->GetstrID());
	}
	else
	{
		delete pftr;
		pftr = NULL;
		return FALSE;
	}
    
    sprintf(pftr->map_name,(LPCSTR)map_name);

	bool deleted=puvs->DeleteFeature(pftr,1);

	delete pftr;
	pftr = NULL;

    return TRUE;
}

///// OPAddvectorLayer //////////////////////////////////////////////////
BOOL OPAddvectorLayer::execute(uvsclient::IUVSClient *puvs)
{
	CValueTable tab;
	
	tab.BeginAddValueItem();
	pLayer->WriteTo(tab);
	tab.AddValue(FIELDNAME_LAYMAPNAME,&(CVariantEx)(_variant_t)map_name);
	tab.AddValue(FIELDNAME_LAYWORKSPACENAME,&(CVariantEx)(_variant_t)workspace_name);
	tab.EndAddValueItem();

	uvsclient::VectorLayer vector_layer;
	vector_layer.id_in_map=pLayer->GetID();
	sprintf(vector_layer.uuid,"%s",pLayer->GetstrID());
	sprintf(vector_layer.attribute_name,"%s",pLayer->GetXAttributeName());
    puvsmodify->vectorlayerFromValueTable(tab,vector_layer);
    bool successed = puvs->SaveVectorlayer(vector_layer);
	pLayer->SetstrID(vector_layer.uuid);

	return TRUE;
}

///// OPDeletevectorLayer //////////////////////////////////////////////////
BOOL OPDeletevectorLayer::execute(uvsclient::IUVSClient *puvs)
{
	CString strid = pLayer->GetstrID();
	bool successed = puvs->DeleteVectorlayer(strid);
	return TRUE;
}

///// OPAddXAttribute //////////////////////////////////////////////////
BOOL OPAddXAttribute::execute(uvsclient::IUVSClient *puvs)
{
	if (pfields == NULL/* || pfields->get_count()!=ptab->GetFieldCount()*/)
	{
		return FALSE;
	}
	//
	uvsclient::AttributeItem * attri_item = new uvsclient::AttributeItem();
	sprintf(attri_item->fid, "%s", fid);
	sprintf(attri_item->layer_id, "%s", layer_id);
	attri_item->deleted = 0;
	attri_item->fields = uvsclient::StringList::NEW();
	attri_item->values = uvsclient::StringList::NEW();
	//
	const CVariantEx *var = NULL;
	for (int i = 0; i < pfields->get_count(); ++i)
	{
		CString field = pfields->get_item(i).field_name;
		CStringA str_value;

		if (ptab->GetValue(idx, field, var))
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

///// OPDeleteXAttribute //////////////////////////////////////////////////
BOOL OPDeleteXAttribute::execute(uvsclient::IUVSClient *puvs)
{
	return puvs->DeleteAttribute(fid, layer_id);
}

///////OPAddvectorgroup //////////////////////////////////////////////////
BOOL OPAddvectorgroup::execute(uvsclient::IUVSClient *puvs)
{
    uvsclient::VectorGroup group;
	group.id_in_map=pgroup->id;
	group.selectable=pgroup->select;
	sprintf(group.name,"%s",pgroup->name);
	sprintf(group.map_name,"%s",map_name);
	bool successed=puvs->SaveFTRGroup(group);
	if(!successed)
	{
		AfxMessageBox(puvs->GetLastError());
	}
	return successed;
}


////////OPDeletevectorgroup //////////////////////////////////////////////
BOOL OPDeletevectorgroup::execute(uvsclient::IUVSClient *puvs)
{
	bool successed=puvs->DeleteFTRGroup(pgroup->name);
	if(!successed)
	{
		AfxMessageBox(puvs->GetLastError());
	}
	return successed;
}


}