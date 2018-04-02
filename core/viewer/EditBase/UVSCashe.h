// UVSCashe.h: interface for the CUVSCashe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UVSCASHE_H__6EACD73D_E475_4FFD_AD93_65E6BCB5814F__INCLUDED_)
#define AFX_UVSCASHE_H__6EACD73D_E475_4FFD_AD93_65E6BCB5814F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Access.h"
#include <AFXMT.H>
#include <list>
#include <STRING>
using std::list;
using std::string;
#include <memory>
#include "uvsclient.h"

namespace uvsclient
{
	struct IUVSClient;
	struct StringList;
}

class CUVSModify;
//
namespace uvscashe
{

enum OPERATETYPE
{
	START_TRANSACTION,
    END_TRANSACTION,
    ADD_FEATURE,
	DEL_FEATURE,
	ADD_LAYER,
	DEL_LAYER,
	ADD_XATTRIBUTE,
	DEL_XATTRIBUTE,
	ADD_FTRGROUP,
	DEL_FTRGROUP
};
//
class BasicOperate
{
public:
	virtual BOOL execute(uvsclient::IUVSClient *puvs) = 0;
	virtual ~BasicOperate() {}
	OPERATETYPE operate_type;
protected:
	CUVSModify * puvsmodify;	
};

//开始一个事务；
class OPStartTransaction : public BasicOperate
{
public:
	OPStartTransaction(CUVSModify * pm)
	{
		puvsmodify=pm;
		operate_type = START_TRANSACTION;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
};

//结束一个事务；
class OPEndTransaction : public BasicOperate
{
public:
	OPEndTransaction(CUVSModify * pm)
	{
		puvsmodify=pm;
		operate_type = END_TRANSACTION;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
};

//添加地物；
class OPAddFeature : public BasicOperate
{
public:
	OPAddFeature(std::shared_ptr<CFeature> & pftr,CUVSModify * pm,int _layer_id,LPCSTR _map_name,LPCSTR _workspace_name)
	{
		pFt=pftr;
		puvsmodify=pm;
		layer_id=_layer_id;
		map_name=_map_name;
		workspace_name=_workspace_name;
		operate_type = ADD_FEATURE;
	}
	~OPAddFeature();
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	std::shared_ptr<CFeature> pFt;
	int layer_id;
	CStringA map_name;
	CStringA workspace_name;
};

//删除地物；
class OPDeleteFeature : public BasicOperate
{
public:
	OPDeleteFeature(std::shared_ptr<CFeature> & pftr,CUVSModify * pm,int _layer_id,LPCSTR _map_name,LPCSTR _workspace_name)
	{
		pFt=pftr;
		puvsmodify=pm;
		layer_id=_layer_id;
		map_name = _map_name;
		operate_type = DEL_FEATURE;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	std::shared_ptr<CFeature> pFt;
	int layer_id;
	CStringA map_name;
};

//添加扩展属性；
class OPAddXAttribute : public BasicOperate
{
public:
	OPAddXAttribute(LPCSTR _fid, LPCSTR _layer_id, int _deleted, uvsclient::attributefielddefineSet* _pfields,
		            std::shared_ptr<CValueTable> & _tab,CUVSModify * _pm, int _idx)
	{
		fid = _fid;
		layer_id = _layer_id;
		deleted = _deleted;
		pfields = _pfields;
		ptab = _tab;
		puvsmodify = _pm;
		idx=_idx;
		operate_type = ADD_XATTRIBUTE;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	CStringA fid;
	CStringA layer_id;
	int deleted;
	uvsclient::attributefielddefineSet* pfields;
	std::shared_ptr<CValueTable> ptab;
	int idx;
};

//删除扩展属性；
class OPDeleteXAttribute : public BasicOperate
{
public:
	OPDeleteXAttribute(LPCSTR _fid, LPCSTR _layer_id, CUVSModify * pm)
	{
		fid = _fid;
		layer_id = _layer_id;
		puvsmodify=pm;
		operate_type = DEL_XATTRIBUTE;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	CStringA fid;
	CStringA layer_id;
};

//添加图层；
class OPAddvectorLayer : public BasicOperate
{
public:
	OPAddvectorLayer(std::shared_ptr<CFtrLayer> & layer, CUVSModify * pm, LPCSTR _workspace_name, LPCSTR _map_name)
	{
		pLayer=layer;
		puvsmodify=pm;
		workspace_name=_workspace_name;
		map_name=_map_name;
		operate_type = ADD_LAYER;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	std::shared_ptr<CFtrLayer> pLayer;
	CStringA workspace_name;
	CStringA map_name;
};

//删除图层；
class OPDeletevectorLayer : public BasicOperate
{
public:
	OPDeletevectorLayer(std::shared_ptr<CFtrLayer> & layer,CUVSModify * pm)
	{
		pLayer=layer;
		puvsmodify=pm;
		operate_type = DEL_LAYER;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	std::shared_ptr<CFtrLayer> pLayer;
};

//添加分组；
class OPAddvectorgroup : public BasicOperate
{
public:
	OPAddvectorgroup(std::shared_ptr<ObjectGroup> & group,CUVSModify * pm,LPCSTR _map_name)
	{
        pgroup=group;
		puvsmodify=pm;
		map_name=_map_name;
		operate_type = ADD_FTRGROUP;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	std::shared_ptr<ObjectGroup> pgroup;
	CStringA map_name;
};

//删除分组；
class OPDeletevectorgroup : public BasicOperate
{
public:
	OPDeletevectorgroup(std::shared_ptr<ObjectGroup> & group,CUVSModify * pm)
	{
		pgroup=group;
		puvsmodify=pm;
		operate_type = DEL_FTRGROUP;
	}
	BOOL execute(uvsclient::IUVSClient *puvs);
private:
	std::shared_ptr<ObjectGroup> pgroup;
};


class CUVSCashe  
{
public:
	static BOOL ConnectUVSServer(LPCTSTR ip, int port);
	static BOOL OpenDB(LPCTSTR db_name, LPCTSTR user_name, LPCTSTR password,CString & identity_id);
	static void ReleaseUVSServer();
	static void SetCurrentWorkspace(uvsclient::WorkSpace& wk);
	static BOOL AddOperate(std::shared_ptr<BasicOperate> operate);
	static void StopExecute();
	static void StartExecute();
	static void WaiteExecuteAll();
	static void SetMaxStack(int _max_stack);
	static int  GetMaxStack();

private:
	static CWinThread * execute_thread;
	static UINT ExecuteThreadFunc(LPVOID pParam);
    static BOOL execute;
	static BOOL executing;
    static int max_stack;
	static std::list< std::shared_ptr<BasicOperate> > operate_list;
	static uvsclient::IUVSClient * puvs;
	static CMutex mutex_execute;
	static CMutex mutex_list;
	static CMutex mutex_puvs;
	static int operate_count;
};

}

#endif // !defined(AFX_UVSCASHE_H__6EACD73D_E475_4FFD_AD93_65E6BCB5814F__INCLUDED_)
