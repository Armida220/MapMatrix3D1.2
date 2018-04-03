// DlgDataSource.h: interface for the CDataSourceEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASOURCEEX_H__1112B7A5_D365_448E_BAA1_75B2E967C0E6__INCLUDED_)
#define AFX_DATASOURCEEX_H__1112B7A5_D365_448E_BAA1_75B2E967C0E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataSource.h"
#include "FtrLayer.h"
#include <map>

#define FIELDNAME_SCALE		_T("Scale")
#define FIELDNAME_BOUND		_T("Bound")
#define FIELDNAME_ZMIN      _T("Zmin")
#define FIELDNAME_ZMAX      _T("Zmax")

MyNameSpaceBegin

struct EXPORT_EDITBASE UserMap
{
	UserMap(){
		memset(name,0,sizeof(name));
		memset(bounds,0,sizeof(bounds));
	}
	TCHAR name[64];
	PT_3D bounds[4];
};

typedef CArray<UserMap,UserMap> CUserMapArray;

class CFtrLayer;
class CDataSourceEx;
class CAccessRead;
class CAccessWrite;
class CAccessModify;
class CSetMaxNumResultsOfSearch;


class EXPORT_EDITBASE CDataQueryEx : public CDataQuery
{
	friend CDataSourceEx;
	friend CSetMaxNumResultsOfSearch;
public:
	struct FilterFeatureItem
	{
		FilterFeatureItem(CSearcherFilter *p1=NULL, PFilterFunc p2=NULL){
			m_pFilter = p1;
			m_pFilterFunc = p2;
		}
		CSearcherFilter *m_pFilter;
		PFilterFunc m_pFilterFunc;
	};
	CDataQueryEx();
	virtual ~CDataQueryEx();

	BOOL SetFilterIncludeLocked(BOOL bLocked);
	BOOL SetFilterIncludeInvisible(BOOL bVisible);
	BOOL SetFilterIncludeRefData(BOOL bRef);
	BOOL SetSymFixed(BOOL bEnable);
	BOOL FilterFeature(LONG_PTR id);
	int FindNearestObjectByObjNum(PT_3D sch_pt, int nObjNum, CCoordSys *pCS, BOOL forSymbolized=TRUE, double* pFindRadius=NULL, double *pLimitRadius=NULL);
	FilterFeatureItem SetFilterFeature(FilterFeatureItem item);//�û����ƵĹ��˺���
	//���½ӿڶ������ػ���
	//manage searchers
	virtual void AddSearcher(CSearcher *ps);
	virtual void RemoveSearcher(CSearcher *ps=NULL);
	virtual void SetActiveSearcher(CSearcher *ps);
	virtual void RefillObjects(CSearcher* ps);
	
	// Check
	virtual BOOL IsManagedObj(CPFeature pObj)const;
	
	// Iterator
	virtual POSITION GetFirstObjPos()const;	
	virtual CFeature* GetNextObjByPos(POSITION &pos)const;
	virtual int GetObjectCount()const;
	
	// Search objects by searchers
	virtual int FindObject(const SearchRange *sr);
	virtual BOOL IsObjInFoundObjs(CPFeature pObj)const;
	virtual const CPFeature *GetFoundHandles(int &num)const;

	void AddSearchableUnit_Sym(CPFeature pFt, SearchableUnit **pus, int nUnit);

	// sch_evlpΪ�ͻ����꣬�򵥲��ң���ͬʱ�������Ż��Ľ������Ҫ�� CRealtimeVectLayer ʹ��
	int FindObjectInRect_SimpleAndSym(Envelope sch_evlp, CCoordSys *pCS, BOOL bEntirelyInc=FALSE, BOOL forSymbolized=TRUE);

	BOOL IsExceedMaxNumOfResult();

protected:	
	// add searchable objects to searchers
	virtual void AddSearchableUnit(CPFeature pFt, SearchableUnit **pus, int nUnit);
	virtual void DelSearchableUnit(CPFeature pFt, BOOL bDelInfo);
	virtual void ClearAllSearchableUnits();

	virtual BOOL SetObjectInfo(CPFeature pFt, int idx, LONG_PTR info);

	void InitQuery();

public:
	virtual BOOL GetObjectInfo(CPFeature pFt, int idx, LONG_PTR &info)const;
	
	//get searchers
	virtual CSearcher *GetActiveSearcher();

public:
	CDataSourceEx *m_pFilterDataSource;
	BOOL m_bUseSymQuery;
	CDataQuery* m_pBaseQuery;
	CDataQuery* m_pSymQuery;
	
	// ������
	CTree8Search m_searchBase;
	CTree8Search m_searchSym;

private:
	BOOL m_bIncludeLocked;
	BOOL m_bIncludeInvisible;
	BOOL m_bIncludeRefData;

	//�����߷��ŵ����⣻
	BOOL m_bSymFixed;
};


class EXPORT_EDITBASE CSetMaxNumResultsOfSearch
{
public:
	CSetMaxNumResultsOfSearch(CDataQueryEx *p, int num);
	~CSetMaxNumResultsOfSearch();

	CDataQueryEx *m_pDQ;
	int m_nSaveNum;
};



class CDataSourceEx;
//��չ��������Դ��
class EXPORT_EDITBASE CAttributesSource
{
	friend class CDataQueryEx;
	friend class CDataSourceEx;
public:
	CAttributesSource();
	virtual ~CAttributesSource();

	BOOL GetXAttributes(CFeature *pFt, CValueTable& tab);
	BOOL SetXAttributes(CFeature *pFt, CValueTable& tab,int idx=0);

	//�˽ӿڵ���ʱ����Ҫ�������δɾ��
	BOOL CopyXAttributes(CFeature* pSrc, CFeature* pDes);
	BOOL SetAccessObject(CAccessModify *pAccess){
		m_pAccess = pAccess;
		return TRUE;
	}
	void SetScale(DWORD scale);
	CAccessModify* GetAccessObject(){
		return m_pAccess;
	}

protected:
	BOOL DelXAttributes(CFeature *pFt);
	BOOL RestoreXAttributes(CFeature *pFt);

protected:
	//��д���󣬿����ڶ�ȡ����д������
	CAccessModify *m_pAccess;
	int m_nScale;
	CDataSourceEx *m_pDS;
};

struct EXPORT_EDITBASE ObjectGroup
{
	ObjectGroup()
	{
		id = 0;
		select = true;
	}
	ObjectGroup(const ObjectGroup &group)
	{
		*this = group;
	}

	ObjectGroup& operator=(const ObjectGroup &group)
	{
		id = group.id;
		strncpy(name,group.name,16);
		select = group.select;
		ftrs.Copy(group.ftrs);
		return *this;
	}
	bool ClearObject(CFeature *pFtr);
	bool AddObject(CFeature *pFtr);
	unsigned int id;
	char name[16];
	bool select;
	CFtrArray ftrs;
};

struct EXPORT_EDITBASE FtrLayerGroup
{
	FtrLayerGroup()
	{
		id = 0;
		Name.Empty();
		Color = RGB(255,255,255);
		Visible = true;
		Symbolized = true;
	}

	FtrLayerGroup(const FtrLayerGroup &group)
	{
		*this = group;
	}
	
	FtrLayerGroup& operator=(const FtrLayerGroup &group)
	{
		id = group.id;
		Name = group.Name;
		Color = group.Color;
		Visible = group.Visible;
		Symbolized = group.Symbolized;
		ftrLayers.Copy(group.ftrLayers);
		return *this;
	}

	bool DelFtrLayer(CFtrLayer *pFtrLayer);
	bool AddFtrLayer(CFtrLayer *pFtrLayer);

	unsigned int id;
	CString Name;
	long	Color;
	bool	Visible;
	bool    Symbolized;

	CFtrLayerArray ftrLayers;
};

class EXPORT_EDITBASE CDataSourceEx : public CDataSource
{
	friend class CSQLiteAccess;
public:	
	struct FtrAppFlag
	{
		CFeature *pFtr;
		int flag;
	};
	struct FtrLayerQueryFlag
	{
		CFtrLayer *pLayer;
		int flag;
	};
	typedef CArray<FtrAppFlag,FtrAppFlag> AppFlagArray;
	typedef CArray<FtrLayerQueryFlag,FtrLayerQueryFlag> QueryFlagArray;

	CDataSourceEx();
	CDataSourceEx(CDataQueryEx *pDQ);
	virtual ~CDataSourceEx();
	BOOL ClearAll(BOOL delete_org_data = false);

	static CDataSourceEx *CreateObj(CDataQueryEx *pDQ);
	static void DestroyObj(CDataSourceEx *pObj);

	void SetModifiedFlag(BOOL bModified);
	BOOL IsModified();

	DWORD GetTotalModifiedTime();

	void SetSavedFlag(BOOL bSaved);
	BOOL IsSaved();
	
	BOOL AddPreviewImage(LPCTSTR name, HBITMAP hBit);
	BOOL DelPreviewImage(LPCTSTR name);

	BOOL SaveDataSettings(LPCTSTR field, LPCTSTR name, LPCTSTR value, LPCTSTR type);
	BOOL DelDataSettings(LPCTSTR field);
	BOOL GetDataSettings(LPCTSTR field, CString &name, CString &value, CString &type);

	int GetDataSettingsInt(LPCTSTR field, int def_val);
	float GetDataSettingsFloat(LPCTSTR field, float def_val);
	double GetDataSettingsDouble(LPCTSTR field, double def_val);
	CString GetDataSettingsString(LPCTSTR field, LPCTSTR def_val);
	void SetDataSettingsInt(LPCTSTR field, int val, LPCTSTR name=NULL);
	void SetDataSettingsFloat(LPCTSTR field, float val, LPCTSTR name=NULL);
	void SetDataSettingsDouble(LPCTSTR field, double val, LPCTSTR name=NULL);
	void SetDataSettingsString(LPCTSTR field, LPCTSTR val, LPCTSTR name=NULL);

	BOOL AddObjectGroup(ObjectGroup *group, BOOL bForLoad=FALSE);
	BOOL DelObjectGroup(ObjectGroup *group);
	BOOL UpdateObjectGroup(ObjectGroup *group);
	int GetMaxObjectGroupID();
	
	int GetObjectGroupCount() const;
	ObjectGroup* GetObjectGroup(int i);
	ObjectGroup* GetObjectGroupByID(UINT id);
	ObjectGroup* CreateNewObjectGroup();
	
	int GetAssociatesWithObject(CFeature *pFtr, CFtrArray &arr);

	BOOL AddFtrLayerGroup(FtrLayerGroup *pGroup, BOOL bForLoad=FALSE);
	BOOL DelFtrLayerGroup(FtrLayerGroup *pGroup);
	BOOL UpdateFtrLayerGroup(FtrLayerGroup *pGroup);
	int GetMaxFtrLayerGroupID();

	int GetFtrLayerGroupCount() const;
	FtrLayerGroup* GetFtrLayerGroup(int i);
	FtrLayerGroup* GetFtrLayerGroupByName(CString Name);

	void LoadLayerSymbolCache();
	void ClearLayerSymbolCache();
	int GetMaxLayerId();
	int GetMaxDisplayOrder();
	//strFieldΪ�洢ʱ������,strNameΪ����, bIncludeExtraFields �Ƿ�����չ����
	void GetAllField(BOOL bIncludeExtraFields, CStringArray &strField,CStringArray &strName);

	virtual BOOL AddObject(CFeature *pFt);
	virtual BOOL DeleteObject(CPFeature pFt);
	BOOL DeleteObjectKeepOrder(CPFeature pFt);
	virtual BOOL RestoreObject(CPFeature pFt);
	virtual BOOL ClearObject(CPFeature pFt);
	void SetName(LPCTSTR dsName)
	{
		m_strName = dsName;
	}
	CString GetName()
	{
		return m_strName;
	}
	BOOL AddObject(CFeature *pFt, int layid, BOOL bForLoad=FALSE);
	BOOL AddObject(CFeature *pFt, LPCTSTR layid, BOOL bForLoad = FALSE);
	BOOL DrawFeature(CFeature *pFtr, GrBuffer *pBuf, BOOL bText=TRUE, float angle=0.0f, LPCTSTR strLayerNameSpecified=NULL);

	//���ַ��Ż���ͼԪ��䡢������ͼԪ�ȷ��ţ������ֲ����Ż����������͡�ͼԪ���͵ȣ�
	BOOL DrawFeature_SymbolizePart(CFeature *pFtr, GrBuffer *pBuf, BOOL bText=TRUE, float angle=0.0f, LPCTSTR strLayerNameSpecified=NULL);

	BOOL ExplodeSymbols(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs, LPCTSTR strLayerNameSpecified=NULL);
	BOOL ExplodeSymbols(CFeature *pFtr, CPtrArray& arrPSyms, CFtrLayer *pLayer, CFtrArray& arrPFtrs);
	BOOL ExplodeSymbolsFinally(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs, LPCTSTR strLayerNameSpecified=NULL);	
	BOOL ExplodeSymbolsFinally(CFeature *pFtr, CPtrArray& arrPSyms, CFtrLayer *pLayer, CFtrArray& arrPFtrs);
	BOOL ExplodeText(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs);

	//��ɢһ����ţ���ͼԪ�������������ȡΪ����ʹ���û����Զ����ǵ�������
	BOOL ExplodeSymbolsExceptSurface(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs, LPCTSTR strLayerNameSpecified=NULL);
	
	int GetUserMapCount();
	UserMap GetUserMap(int i);
	UserMap GetUserMap(LPCTSTR name);

	CFtrLayer* CreateFtrLayer(LPCTSTR layname);
	int GetFtrLayerCount();
	int GetCurFtrLayerID();

	int GetValidObjCount();
	int GetEditableObjCount();

	CFtrLayer* GetFtrLayerByIndex(int i);
	CFtrLayer* GetFtrLayer(int layid);
	CFtrLayer* GetFtrLayerBystrID(LPCTSTR str_id);
	void GetAllFtrLayersByDisplayOrder(CFtrLayerArray& arr);

	CFtrLayer* GetFtrLayer(LPCTSTR strLayerName, LPCTSTR mapname=NULL, CPtrArray *arr=NULL);
	CFtrLayer* GetLocalFtrLayer(LPCTSTR strLayerName);
	CFtrLayer* GetCurFtrLayer();
	CFtrLayer* GetOrCreateFtrLayer(LPCTSTR strLayerName);
	
	CFtrLayer* GetDefaultTxtFtrLayer();
	CFtrLayer* GetDefaultPntFtrLayer();
	CFtrLayer* GetDefaultLineFtrLayer();
	CFtrLayer* GetDefaultPolygonFtrLayer();
	CFtrLayer* GetFtrLayerOfObject(CPFeature pFtr);

	void GetFtrLayersByNameOrCode(LPCTSTR strLayerName, CFtrLayerArray& arr);
	void GetFtrLayersByNameOrCode_editable(LPCTSTR strLayerName, CFtrLayerArray& arr);

	//�ƺ�û��������ӿ�
	BOOL RestoreFtrLayerOfObject(CPFeature pFtr,int layid);
	BOOL SetFtrLayerOfObject(CPFeature pFtr,int layid);
//	BOOL SetCurFtrLayer(LPCTSTR layname, LPCTSTR mapname=NULL);
	BOOL SetCurFtrLayer(int layid);
	
	BOOL SetDefaultXAttributes(CFeature *pFtr);

	CFeature* CreateObjByNameOrLayerIdx(LPCTSTR name);
	BOOL AddFtrLayer(CFtrLayer* pLayer, BOOL bForLoad=FALSE);
	void DelFtrLayer(CFtrLayer* pLayer);
//	void DelFtrLayer(LPCTSTR layname, LPCTSTR mapname=NULL);
	void DelFtrLayer(int layid);
	void RestoreLayer(CFtrLayer* pLayer);
	void SaveAllLayers();
	void SaveLayer(CFtrLayer *pLayer);
	void ResetDisplayOrder();
	void SaveFeature(CFeature *pFtr);

	CFeature *GetFeatureByID(OUID id);

	Envelope GetBound()const{
		Envelope evlp;
		evlp.CreateFromPts(m_bounds,4);
		evlp.m_zl = m_lfZmin;
		evlp.m_zh = m_lfZmax;
		return evlp;
	}
	void GetBound(PT_3D *pts, double *zmin, double *zmax)const{
		if(pts)
			memcpy(pts,m_bounds,sizeof(m_bounds));
		if(zmin) *zmin = m_lfZmin;
		if(zmax) *zmax = m_lfZmax;
	}
	void SetBound(const PT_3D *pts, double zmin, double zmax);

	DWORD GetScale()const{
		return m_nScale;
	}
	void SetScale(DWORD scale);

	void SetDrawScaleByUser(double fDrawScale=1);
	double GetDrawScaleByUser();
	double GetSymbolDrawScale();
	double GetAnnotationScale();

	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab);

	BOOL SetAccessObject(CAccessModify *pAccess);
	CAccessModify* GetAccessObject(){
		return m_pAccess;
	}
	virtual BOOL SaveAll(CAccessWrite *pAccess);
	virtual BOOL LoadAll(CAccessRead *pAccess, BOOL bShowProgress=TRUE);

	BOOL IsFeatureValid(CFeature *pFtr);

	// manage eXtensional attributes of features
	CAttributesSource *GetXAttributesSource(){
		return m_pAttrSource;
	}

	BOOL GetAllAttribute(CFeature *pFtr, CValueTable &tab);

	// ���Ʋ���ʾ˳��
	void DisplayTop(CFtrLayer *pLayer);
	void DisplayBottom(CFtrLayer *pLayer);
	void DisplayInsert(CFtrLayer *pInserted, CFtrLayer *pLayer);

	CString GetFtrLayerNameByNameOrLayerIdx(LPCTSTR name);

	__int64 GetFtrLayerCode(LPCTSTR name);

	void EnableMonocolor(BOOL *bEnable, COLORREF *clr = NULL){
		if(bEnable)m_bMonoMode = *bEnable;
		if(clr)m_monoColor = *clr;
	}
	void GetMonoColor(BOOL *bEnable,COLORREF *clr){
		if(bEnable)*bEnable = m_bMonoMode;
		if(clr)*clr = m_monoColor;
	}
	void EnableShowBound(BOOL bEnable){
		m_bShowBound = bEnable;
	}
	BOOL GetShowBound(){
		return m_bShowBound;
	}
	BOOL UpdateFtrQuery(CPFeature pFtr=NULL);
	CString GetTempFilePath();

	double GetFtrsMaxMinDisplayOrder(BOOL bMax);
	void GetAllFtrsByDisplayOrder(CFtrArray &arr);
	BOOL DisplayTop(CFeature **pSrcFtrs, int nSrcNum, CFeature **pRefFtrs=NULL, int nRefNum=0);
	BOOL DisplayBottom(CFeature **pSrcFtrs, int nSrcNum, CFeature **pRefFtrs=NULL, int nRefNum=0);

	//����ͻָ�����feature��AppFlag
	void SaveAllAppFlags(BOOL bClearZero=TRUE);
	void RestoreAllAppFlags();
	void SetAllAppFlags(int flag);

	//����ͻָ�����ftrlayer��AllowQuery
	void SaveAllQueryFlags(BOOL bClear=TRUE, BOOL bQueryFlag=TRUE);
	void RestoreAllQueryFlags();
	void SetFtrLayerQueryFlag(LPCTSTR layname, BOOL bAllowQuery);

protected:
	void AddSymGrBufToQuery(CPFeature pFtr);
	void UpdateModifiedTime();
	BOOL FindOrderIndex(double order, int& insert_idx);

	virtual void CreateSearchableUnit(CFeature *pObj, CPtrArray& arrPUnits);
	
protected:
	BOOL m_bModified;
	BOOL m_bHaveSaved;
	DWORD			m_nStartModifiedTime;
	DWORD			m_nTotalModifiedTime;
	//����Դ��
	CString m_strName;

	// ����
	CArray<ObjectGroup*,ObjectGroup*> m_arrFtrGroup;
	// ������
	CArray<FtrLayerGroup*,FtrLayerGroup*> m_arrFtrLayerGroup;

	CFtrLayerArray m_arrPFtrLayer;
	CUserMapArray m_arrUserMap;
	int m_nCurLayer;
	CUIntArray m_arrUsedFtrlayer;

	CFtrLayerArray m_arrPFtrLayerByDisplayOrder;
	BOOL m_bDisplayOrderModified;

	//����Դ��Χ
	PT_3D m_bounds[4];
	double m_lfZmin,m_lfZmax;

	//������
	DWORD m_nScale;

	CAttributesSource *m_pAttrSource;

	//��д���󣬿����ڶ�ȡ����д������
	CAccessModify *m_pAccess;

	// ��ʾ����
	double m_fDrawScale;

	BOOL m_bSymbolizedSelect;
	BOOL m_bShowBound;
	BOOL m_bMonoMode;
	COLORREF m_monoColor;

	// ��������ʾ˳������
	CFtrArray m_arrAllFtrsByOrderID; 

	CArray<AppFlagArray*,AppFlagArray*> m_arrAppFlagsBuf;
	CArray<QueryFlagArray*,QueryFlagArray*> m_arrQueryFlagsBuf;

public:
	BOOL FindLayerIdx(BOOL byCode, __int64 &code, CString& name);
	__int64 GetLayerCodeOfFtrLayer(CFtrLayer *pFtrLayer);
	//FILETIME writetime;
	BOOL m_bUVSLocalCopy;
};


MyNameSpaceEnd

#endif // !defined(AFX_DATASOURCEEX_H__1112B7A5_D365_448E_BAA1_75B2E967C0E6__INCLUDED_)
