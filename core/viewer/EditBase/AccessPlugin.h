// AccessPlugin.h: interface for the CAccessPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCESSPLUGIN_H__C78DD449_AEBD_4A9B_9BD3_FFBD9B589C34__INCLUDED_)
#define AFX_ACCESSPLUGIN_H__C78DD449_AEBD_4A9B_9BD3_FFBD9B589C34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "PluginAPI.h"

struct XDef
{
	void operator=(XDef def)
	{
		strcpy(field,def.field);
		strcpy(name,def.name);
		strcpy(defvalue,def.defvalue);
		valuetype = def.valuetype;
		valuelen = def.valuelen;
	}
	
	//�ֶ������洢�õ�����
	char field[32];
	
	//��������ʾ���û��������ƣ�
	char name[32];
	
	//ȱʡֵ
	char defvalue[64];
	
	//ֵ����
	int  valuetype;
	
	//ֵ����
	int  valuelen;
};

struct LayerDef 
{
	LayerDef(){}
	LayerDef(const LayerDef &lay){
		memcpy(this,&lay,sizeof(LayerDef));
	}
	LayerDef& operator=(const LayerDef&lay ){
		if(this==&lay)return *this;
		memcpy(this,&lay,sizeof(LayerDef));
		return *this;
	}
	CString m_strGroupName;
	__int64 m_nCode;
	CString m_strLayName;
	CStringArray m_strSupportClsName;
	int m_nGeoClass;
	long m_nColor;
	XDef *m_pXDefines;
	int m_nNum;
};

class EXPORT_EDITBASE CAccessPlugin  
{
public:
	CAccessPlugin();
	virtual ~CAccessPlugin();

	virtual BOOL IsUseFdb(){return TRUE;}
	virtual void SetFdbAccessObj(AccessObj accessObj){}
	virtual BOOL  Compress(){return TRUE;};
	virtual BOOL Attach(LPCTSTR fileName)=0;
	virtual BOOL Close()=0;
	
	virtual BOOL CreateFileSys(const LayerDef* laydef, int num)=0;
	virtual BOOL IsValid()=0;
	
	virtual void ClearAttrTables()=0;
	
// 	virtual BOOL OpenRead(LPCTSTR filename)=0;
// 	virtual BOOL OpenWrite(LPCTSTR filename)=0;
// 	virtual BOOL CloseRead()=0;
// 	virtual BOOL CloseWrite()=0;
// 	
	//��ȡ����Դ��Ϣ
	virtual BOOL ReadDataSourceInfo(DataId dataId)=0;
	
	//��ȡ��
	virtual LayId GetFirstFtrLayer(long &idx)=0;
	virtual LayId GetNextFtrLayer(long &idx)=0;
	virtual int GetFtrLayerCount()=0;
	
	//��ȡ����
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL)=0;
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL)=0;
	virtual int GetFeatureCount()=0;
	virtual int GetCurFtrLayID()=0;
	virtual int GetCurFtrGrpID()=0;
	
	//��ȡ��չ������Ϣ
	virtual BOOL ReadXAttribute(CFeature *pFtr, CValueTable& tab)=0;
	
	//��������Դ��Ϣ
	virtual void WriteDataSourceInfo(DataId dataId)=0;
	
	//���²�
	virtual BOOL SaveFtrLayer(LayId pLayer)=0;
	virtual BOOL DelFtrLayer(LayId pLayer)=0;
	
	//���µ���
	virtual BOOL SetCurFtrLayID(int id)=0;
	virtual BOOL SetCurFtrGrpID(int id)=0;
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL)=0;
	virtual BOOL DelFeature(CFeature *pFt)=0;
	virtual BOOL DelXAttributes(CFeature *pFtr)=0;
	virtual BOOL RestoreXAttributes(CFeature *pFtr)=0;
	//������չ������Ϣ
	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0)=0;
	
	virtual void BatchUpdateBegin()=0;
	virtual void BatchUpdateEnd()=0;

};

#endif // !defined(AFX_ACCESSPLUGIN_H__C78DD449_AEBD_4A9B_9BD3_FFBD9B589C34__INCLUDED_)
