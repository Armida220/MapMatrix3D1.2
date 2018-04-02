// PlugFunc.cpp: implementation of the PlugFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "PlugFunc.h"
#include "editbasedoc.h "
#include "SymbolLib.h "
#include "DlgDataSource.h "
#include "ExMessage.h "
#include "SQLiteAccess.h "
#include "GeoText.h "
#include "GeoBuilderPrj2.h"
#include "DSM.h"
#include "CoordCenter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern CString DecodeString(const char *p);

extern void VariantToText(const CVariantEx& v, CString &text);

namespace plugins
{
void initPluginApi()
{
	fdb_copyXAttributes = _fdb_copyXAttributes;

	fdb_getXAttributes = _fdb_getXAttributes;
	
	fdb_refreshViews = _fdb_refreshViews;
	
	fdb_newFtrPropertyObj = _fdb_newFtrPropertyObj;

	fdb_deleteFtrPropertyObj = _fdb_deleteFtrPropertyObj;

	fdb_refreshFtrPropertyPanel = _fdb_refreshFtrPropertyPanel;

	fdb_getCurView = _fdb_getCurView;
	
	fdb_startPluginCommand = _fdb_startPluginCommand;

	fdb_getCurDoc = _fdb_getCurDoc;

	fdb_getCurCommand = _fdb_getCurCommand;

	fdb_StartCmd = _fdb_StartCmd;
	
	fdb_getLayerOfFtr = _fdb_getLayerOfFtr;
	
	fdb_getColorOfLayer = _fdb_getColorOfLayer;
	
	fdb_getNameOfLayer = _fdb_getNameOfLayer; 
	
	fdb_getIdOfLayer = _fdb_getIdOfLayer;

	fdb_addFtr = _fdb_addFtr;
	
	fdb_addFtrWithProperties = _fdb_addFtrWithProperties;
	
	fdb_delFtr = _fdb_delFtr;
	
	fdb_getLayCntOfDoc = _fdb_getLayCntOfDoc;
	
	fdb_getLayerByIdx = _fdb_getLayerByIdx;
	
	fdb_getFtrCntOfLay = _fdb_getFtrCntOfLay;
	
	fdb_getFtrFromLay = _fdb_getFtrFromLay;
	
	fdb_updateStereoParam = _fdb_updateStereoParam;	

	fdb_getScale = _fdb_getScale;
	
	fdb_getWorkSpaceBound = _fdb_getWorkSpaceBound;
	
	fdb_findUserIdx = _fdb_findUserIdx;
	
	fdb_getSymbol = _fdb_getSymbol;

	fdb_getCodeOfLayer = _fdb_getCodeOfLayer;

	fdb_progressBegin = _fdb_progressBegin;

	fdb_progressStep = _fdb_progressStep;

	fdb_progressEnd = _fdb_progressEnd;

	fdb_outputstring = _fdb_outputstring;

	fdb_excDML = _fdb_excDMl;
	
	fdb_excQuery = _fdb_excQuery;

	fdb_modifyPropertyUI = _fdb_modifyPropertyUI;

	fdb_isAutoSetAnchor =  _fdb_isAutoSetAnchor;
	
	fdb_setAnchorPoint = _fdb_setAnchorPoint;

	fdb_activeLayer = _fdb_activeLayer;

	fdb_setCurLayer = _fdb_setCurLayer;

	fdb_getCurLayer =  _fdb_getCurLayer;

	fdb_createLayer = _fdb_createLayer;

	fdb_addLayer = _fdb_addLayer;

	fdb_setColorOfLayer = _fdb_setColorOfLayer;

	fdb_isLockedOfLayer = _fdb_isLockedOfLayer;

	fdb_enableLockedOfLayer = _fdb_enableLockedOfLayer;

	fdb_isVisibleOfLayer = _fdb_isVisibleOfLayer;
	
	fdb_enableVisibleOfLayer = _fdb_enableVisibleOfLayer;

	fdb_createDefaultFeature = _fdb_createDefaultFeature;

	fdb_getLocalLayer =  _fdb_getLocalLayer;

	fdb_getLayer =  _fdb_getLayer;

	fdb_annotToText = _fdb_annotToText;

	fdb_saveFtr = _fdb_saveFtr;

	fdb_beginAddFtr = _fdb_beginAddFtr;	
	
	fdb_compileStatement = _fdb_compileStatement;
	
	fdb_bindField = _fdb_bindField;
	
	fdb_endAddFtr = _fdb_endAddFtr;

	fdb_endReadFtrFromTab =  _fdb_endReadFtrFromTab;

	fdb_isEof = _fdb_isEof;
	
	fdb_getNumFields =  _fdb_getNumFields;
	
	fdb_getStringField =  _fdb_getStringField;
	
	fdb_getIntField = _fdb_getIntField;
	
	fdb_getFloatField = _fdb_getFloatField;
	
	fdb_getBlobField = _fdb_getBlobField;
	
    fdb_nextRow = _fdb_nextRow;
	
    fdb_beginReadFtrFromTab = _fdb_beginReadFtrFromTab;

	fdb_getRecordNum = _fdb_getRecordNum;

	fdb_readFtrXAttribute = _fdb_readFtrXAttribute;

	fdb_saveFtrXAttribute = _fdb_saveFtrXAttribute;

	fdb_deleteFtrXAttribute = _fdb_deleteFtrXAttribute;

	fdb_restoreFtrXAttribute = _fdb_restoreFtrXAttribute;

	fdb_deleteFtr = _fdb_deleteFtr;

	fdb_getSelection = _fdb_getSelection;

	fdb_updateSelection = _fdb_updateSelection;

	fdb_getSnap = _fdb_getSnap;
	
	fdb_getSnapResultNum = _fdb_getSnapResultNum;
	
	fdb_getSnapResult0 = _fdb_getSnapResult0;

	prj_createPrj = _prj_createPrj;
	prj_createCoordCenter = _prj_createCoordCenter;
	prj_deletePrj = _prj_deletePrj;
	prj_deleteCoordCenter = _prj_deleteCoordCenter;
	prj_loadProject = _prj_loadProject;
	prj_getAllStereoIDs = _prj_getAllStereoIDs;
	prj_getStereoImages = _prj_getStereoImages;
	prj_initModel = _prj_initModel;
	prj_convertXYZ = _prj_convertXYZ;
	prj_IsGeoImage = _prj_IsGeoImage;
	prj_clear = _prj_clear;
	prj_initModel2 = _prj_initModel2;
	prj_convertXYZ2 = _prj_convertXYZ2;
	prj_getModelBound = _prj_getModelBound;

	dsm_create = _dsm_create;
	dsm_open = _dsm_open;
	dsm_getZ = _dsm_getZ;
	dsm_delete = _dsm_delete;
	dsm_getInfo = _dsm_getInfo;

}

//拷贝扩展属性
BOOL  _fdb_copyXAttributes(DocId docId, FtrId srcFtrId, FtrId desFtrId)
{
	if (docId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	CFeature *pFtr0 = (CFeature *)srcFtrId;
	CFeature *pFtr1 = (CFeature *)desFtrId;
	return pDoc->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr0,pFtr1);
}

//获取扩展属性
BOOL  _fdb_getXAttributes(DocId docId, FtrId ftr, CValueTable& tab)
{
	if (docId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	CFeature *pFtr = (CFeature *)ftr;
	return pDoc->GetDlgDataSource()->GetXAttributesSource()->GetXAttributes(pFtr,tab);
}

//视图的更新
void  _fdb_refreshViews(DocId docId, HINTCODE intCode, long data)
{
	if (docId==0)return;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	pDoc->UpdateAllViews(NULL, intCode, (CObject*)data);
}

// 获取地物属性句柄
FtrPropertyObj _fdb_newFtrPropertyObj(FtrId ftrId)
{
	CFeature *pFtr = (CFeature*)ftrId;
	if (pFtr == NULL) return 0;

	CPermanentExchanger *pObj = new CPermanentExchanger(pFtr);
	return (FtrPropertyObj)pObj;
}

// 释放地物属性句柄
void _fdb_deleteFtrPropertyObj(FtrPropertyObj propertyObj)
{
	CPermanentExchanger *pObj = (CPermanentExchanger*)propertyObj;
	if (pObj)
	{
		delete pObj;
		pObj = NULL;
	}
}

// 更新地物属性窗口
void  _fdb_refreshFtrPropertyPanel(FtrPropertyObj propertyObj)
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)propertyObj);
}

//获得当前激活视图
ViewId  _fdb_getCurView(DocId docId)
{
	if (docId==0)return 0;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return (ViewId)pDoc->GetCurActiveView();
}

// 激活插件命令项
BOOL _fdb_startPluginCommand(LPCTSTR strPluginName, DocId docId, int id)
{
	if (strPluginName == NULL || docId == 0) return FALSE;

	int nDocCommand = -1;

	int nsz = 0;
	const PlugItem *pItem = ((CEditBaseApp*)AfxGetApp())->GetPlugins(nsz);
	for (int i=0;i<nsz;i++)
	{
		CString name = pItem[i].pObj->GetPlugName();
		if (stricmp(name,strPluginName) == 0)
		{
			for (int j=0; j<pItem[i].pObj->GetItemCount(); j++)
			{
				if (pItem[i].itemPlugID[j] == id)
				{
					nDocCommand = pItem[i].itemID[j];
					break;
				}
			}

			if (nDocCommand != -1)
			{
				break;
			}			
		}
	}

	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	if (pDoc)
	{
		return pDoc->StartCommand(nDocCommand);
	}

	return FALSE;

}

//获得当前文档
DocId  _fdb_getCurDoc()
{
	return (DocId)(CEditor*)GetActiveDlgDoc();
}

// 获取当前命令
CommandObj  _fdb_getCurCommand(DocId docId)
{
	CDlgDoc *pDoc = (CDlgDoc*)(CEditor*)docId;
	if (pDoc)
	{
		return (CommandObj)pDoc->GetCurrentCommand();
	}

	return 0;
}

//启动指定命令
BOOL _fdb_StartCmd(LPCTSTR cmd)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return FALSE;

	int nReg = 0;
	const CMDREG *pRegs = pDoc->GetCmdRegs(nReg);
	for (int i = 0; i<nReg; i++)
	{
		CString name = pRegs[i].strName;
		if (0 == name.CompareNoCase(cmd))
		{
			pDoc->StartCommand(pRegs[i].id);
			return TRUE;
		}
		CString accel = pRegs[i].strAccel;
		if (0 == accel.CompareNoCase(cmd))
		{
			pDoc->StartCommand(pRegs[i].id);
			return TRUE;
		}
	}

	return FALSE;
}

//得到地物所在的层
LayId  _fdb_getLayerOfFtr(DocId docId, FtrId ftrId)
{
	if (docId==0)return 0;	
	CDlgDoc *pDoc = (CDlgDoc*)(CEditor *)docId;
	CFeature *pFtr = (CFeature *)ftrId;
	return (LayId) pDoc->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
}

//得到层的颜色
long  _fdb_getColorOfLayer(LayId layId)
{
	if (layId==0)return -1;	
	CFtrLayer *pLay = (CFtrLayer *)layId;
	return (long)pLay->GetColor();
	
}

//得到层名
LPCTSTR  _fdb_getNameOfLayer(LayId layId)
{
	if (layId==0)return NULL;	
	CFtrLayer *pLay = (CFtrLayer *)layId;
	return pLay->GetName();
}

//得到层Id
int _fdb_getIdOfLayer(LayId layId)
{
	if (layId==0)return NULL;	
	CFtrLayer *pLay = (CFtrLayer *)layId;
	return pLay->GetID();
}

//增加地物到文档中
BOOL  _fdb_addFtr(DocId docId, FtrId ftrId, LayId layId)
{
	if (docId==0||layId==0||ftrId==0)return FALSE;
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	CFtrLayer *pLay = (CFtrLayer *)layId;
	CFeature *pFtr = (CFeature *)ftrId;
	return pDoc->AddObject(pFtr,pLay->GetID());
}

BOOL  _fdb_addFtrWithProperties(DocId docId, FtrId ftrId, int layId)
{
	if (docId==0||layId==0||ftrId==0)return FALSE;
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	/*CFtrLayer *pLay = (CFtrLayer *)layId;*/
	CFeature *pFtr = (CFeature *)ftrId;
	return pDoc->AddObjectWithProperties(pFtr,layId);
}

//删除地物
BOOL  _fdb_delFtr(DocId docId, FtrId ftrId, BOOL bUpdateSel)
{
	if (docId==0||ftrId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	FTR_HANDLE handle = (FTR_HANDLE)ftrId;
	return pDoc->DeleteObject(handle, bUpdateSel);
}

//得到某个文档中，层的个数
int  _fdb_getLayCntOfDoc(DocId docId)
{
	if (docId==0)return -1;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return pDoc->GetDlgDataSource()->GetFtrLayerCount();
}

//得到文档中第idx个层
LayId _fdb_getLayerByIdx(DocId docId, int idx)
{
	if (docId==0)return -1;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return (LayId)pDoc->GetDlgDataSource()->GetFtrLayerByIndex(idx);
}

//得到层中地物的个数
int  _fdb_getFtrCntOfLay(LayId lay)
{
	if (lay==0)return -1;
	CFtrLayer *pLay = (CFtrLayer*)lay;
	return pLay->GetObjectCount();
}

//得到层lay中地idx个地物
FtrId  _fdb_getFtrFromLay(LayId lay, int idx, BOOL bIncDeleted)
{
	if (lay==0)return -1;
	CFtrLayer *pLay = (CFtrLayer*)lay;

	if( bIncDeleted )
		return (FtrId)pLay->GetObject(idx, 0);
	else
		return (FtrId)pLay->GetObject(idx);
	
}

//更新视图参数
long  _fdb_updateStereoParam(ViewId viewId, LayId lay)
{
	CView *pView = (CView *)viewId;
	CFtrLayer *pLay = (CFtrLayer *)lay;
	CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(((CDlgDoc*)pView->GetDocument())->GetDlgDataSource()->GetScale());
	if (pPlaceLib)
	{
		CPlaceConfig *pConfig = pPlaceLib->GetConfig(pLay->GetName());
		//CView* pView = GetActiveDlgDoc()->GetCurActiveView();
		if (pConfig/* && pView*/)
		{
			return GetActiveDlgDoc()->UpdateAllViewsParams(0,(LPARAM)pConfig->m_strViewParams);
			//return pView->SendMessage(FCCM_UPDATESTEREOPARAM, 0, (LPARAM)pConfig->m_strViewParams);
		}
	}
	return -1;
}

long _fdb_getScale(DocId docId)
{
	if (docId==0)return -1;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return pDoc->GetDlgDataSource()->GetScale();
}

void _fdb_getWorkSpaceBound(DocId docId, PT_3D pts[4])
{
	if (docId==0)return;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	pDoc->GetDlgDataSource()->GetBound(pts,NULL,NULL);
}

BOOL _fdb_findUserIdx(DocId docId, BOOL bFindStr, __int64 &code, CString& name)
{
	if (docId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;

	static CString str;
	str = name;
	BOOL bret = pDoc->GetDlgDataSource()->FindLayerIdx(bFindStr, code, str);
	name = str;
	return bret;
}

BOOL _fdb_getSymbol(DocId docId, FtrId ftrId, CPtrArray &arrSymbol)
{
	if (docId==0||ftrId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;	
	CFeature *pFtr = (CFeature *)ftrId;
	CConfigLibManager *pCLM = gpCfgLibMan;
	return pCLM->GetSymbol(pDoc->GetDlgDataSource(),pFtr,arrSymbol);
}

__int64 _fdb_getCodeOfLayer(DocId docId, LayId layId)
{
	if (docId==0||layId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;	
	CFtrLayer *pLay = (CFtrLayer *)layId;
    return 	pDoc->GetDlgDataSource()->GetLayerCodeOfFtrLayer(pLay);
}

void _fdb_progressBegin(int nSum)
{
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, nSum);
}

void _fdb_progressStep(int nStep)
{
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, nStep);
}

void _fdb_progressEnd()
{
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
}

void _fdb_outputstring(LPCTSTR text, BOOL bNewLine)
{
	GOutPut(text,bNewLine);
}

BOOL _fdb_excDMl(AccessObj obj, char sql[1024])
{
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	pDb->execDML(sql);
	return TRUE;
}

BOOL _fdb_excQuery(AccessObj obj, char sql[1024], CValueTable &retTable)
{
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	CppSQLite3Query query = pDb->execQuery(sql);
	if(query.eof())return FALSE;
	int nVauleType;
	int nValue;
	double lfValue;
	const char* pStr;
	int nLen = 0;
	const BYTE *pBuf = NULL;
	int nPt = 0;
	while(!query.eof())
	{
		retTable.BeginBatchAddValueItem();
		for(int i = 0;i<query.numFields();i++)
		{
			nVauleType = query.fieldDataType(i);
			switch(nVauleType)
			{
			case SQLITE_INTEGER:
				nValue = query.getIntField(i);
				retTable.AddValue(query.fieldName(i),&(CVariantEx)(_variant_t)(long)nValue);
				break;
			case SQLITE_FLOAT:
				lfValue = query.getFloatField(i);
				retTable.AddValue(query.fieldName(i),&(CVariantEx)(_variant_t)lfValue);
				break;
			case SQLITE_TEXT:
				pStr = query.getStringField(i);
				retTable.AddValue(query.fieldName(i),&(CVariantEx)(_variant_t)pStr);
				break;	
			case SQLITE_BLOB:
				nLen = 0;
				pBuf = query.getBlobField(i,nLen);
				nPt = nLen/sizeof(PT_3DEX);
				if( nPt>0 )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					arrPts.SetSize(nPt);
					memcpy(arrPts.GetData(),pBuf,nPt*sizeof(PT_3DEX));
					CVariantEx var;
					var.SetAsShape(arrPts);				
					retTable.AddValue(query.fieldName(i),&var);
				}
				break;			
			default:
				retTable.AddValue(query.fieldName(i),&(CVariantEx)(_variant_t()));
				break;
			}		
		}
		retTable.EndAddValueItem();
		query.nextRow();
	}
	
	query.finalize();
	return TRUE;
}

void _fdb_modifyPropertyUI(WPARAM wParam, FtrId ftrId, void *&pSel)
{
	if (ftrId==0)
	{
		if (pSel)
		{
			delete (CPermanentExchanger*)pSel;
			pSel = NULL;
		}
		AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);

	}
	else
	{
		pSel = new CPermanentExchanger((CFeature*)ftrId);
		AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)pSel);
	}	
}

BOOL _fdb_isAutoSetAnchor(DocId docId)
{
	if (docId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return pDoc->IsAutoSetAnchor();
}

void _fdb_setAnchorPoint(DocId docId, PT_3D pt)
{
	if (docId==0)return; 	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	pDoc->SetAnchorPoint(pt);
}

int _fdb_activeLayer(DocId docId, LPCTSTR strLayer, BOOL bSaveRecent)
{
	if (docId==0)return 0;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return pDoc->ActiveLayer(strLayer, bSaveRecent);
}

void _fdb_setCurLayer(DocId docId, LayId lay)
{
	if (docId==0||lay==0)return;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	CFtrLayer *pLay = (CFtrLayer *)lay;
	pDoc->GetDlgDataSource()->SetCurFtrLayer(pLay->GetID());
}

LayId _fdb_getCurLayer(DocId docId)
{
	if (docId==0)return 0;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return (LayId)(pDoc->GetDlgDataSource()->GetCurFtrLayer());
}

LayId _fdb_createLayer(DocId docId, LPCTSTR layName)
{
	if (docId==0 || layName==NULL)return 0;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return (LayId)(pDoc->GetDlgDataSource()->CreateFtrLayer(layName));
}

BOOL  _fdb_addLayer(DocId docId, LayId layId)
{
	if (docId==0 || layId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return (LayId)(pDoc->GetDlgDataSource()->AddFtrLayer((CFtrLayer*)layId));
}

void  _fdb_setColorOfLayer(LayId layId, long color)
{
	if (layId==0)return;	
	((CFtrLayer*)layId)->SetColor(color);
}

BOOL  _fdb_isLockedOfLayer(LayId layId)
{
	if (layId==0)return FALSE;	
	return ((CFtrLayer*)layId)->IsLocked();
}

void  _fdb_enableLockedOfLayer(LayId layId, BOOL bLocked)
{
	if (layId==0)return;	
	((CFtrLayer*)layId)->EnableLocked(bLocked);
}

BOOL  _fdb_isVisibleOfLayer(LayId layId)
{
	if (layId==0)return FALSE;	
	return ((CFtrLayer*)layId)->IsVisible();
}

void  _fdb_enableVisibleOfLayer(LayId layId, BOOL bVisible)
{
	if (layId==0)return;	
	((CFtrLayer*)layId)->EnableVisible(bVisible);
}

FtrId _fdb_createDefaultFeature(LayId lay, DocId docId, int nCls)
{
	if (docId==0||lay==0)return 0;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	CFtrLayer *pLay = (CFtrLayer*)lay;
	return (FtrId)pLay->CreateDefaultFeature(pDoc->GetDlgDataSource()->GetScale(),nCls);
}

LayId _fdb_getLocalLayer(DocId docId, LPCTSTR layName)
{
	if (docId==0)return 0;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return (LayId)pDoc->GetDlgDataSource()->GetLocalFtrLayer(layName);
}

LayId _fdb_getLayer(DocId docId, LPCTSTR layName)
{
	if (docId==0)return 0;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	return (LayId)pDoc->GetDlgDataSource()->GetFtrLayer(layName);
}

BOOL _fdb_annotToText(DocId docId, FtrId ftrId, LayId layId, FtrId tmp, LPCTSTR name)
{
	if (docId==0||ftrId==0||tmp==0||layId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	CFeature *pFtr = (CFeature*)ftrId;
	CFeature *pTemp = (CFeature*)tmp;
	CFtrLayer *pLayer = (CFtrLayer*)layId;
	CPtrArray arrAnnots;
	CConfigLibManager *pCLM = gpCfgLibMan;
	pCLM->GetSymbol(pDoc->GetDlgDataSource(),pFtr,arrAnnots);
	CUndoFtrs undo(pDoc,name);
	for(int i=0; i<arrAnnots.GetSize(); i++)
	{
		CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
		if (pSymbol && pSymbol->GetType()==SYMTYPE_ANNOTATION )
		{
			CAnnotation *pAnnot = (CAnnotation*)pSymbol;							
			CPtrArray parr;
			CValueTable tab;
			pDoc->GetDlgDataSource()->GetAllAttribute(pFtr,tab);
			if( pAnnot->ExtractGeoText(pFtr,parr,tab) )
			{
				for (int j=0; j<parr.GetSize(); j++)
				{
					CGeoText *pText = (CGeoText*)parr[j];
					pText->SetColor(pFtr->GetGeometry()->GetColor());
					CFeature *pNewFtr = pTemp->Clone();
					pNewFtr->SetGeometry(pText);									
					pDoc->AddObject(pNewFtr,pLayer->GetID());									
					undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
				}
			}
		}					
	}
	undo.Commit();

	return TRUE;
}

void  _fdb_saveFtr(AccessObj accessObj, FtrId ftrId, LPCTSTR table)
{

}

BOOL _fdb_beginAddFtr(AccessObj accessObj, FtrId ftrId, LPCTSTR table, BOOL &bAdd)
{
	if (accessObj==0||ftrId==0||table==NULL) return FALSE;
	
	CSQLiteAccess *pAccess = (CSQLiteAccess *)accessObj;
	CFeature *pFtr = (CFeature*)ftrId;
	char line[1024] = {0};
	CppSQLite3Query query;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	// 查找地物是否存在
	sprintf(line,"select FTRID from %s where FTRID='%s';",table, pFtr->GetID().ToString());
	query = pDb->execQuery(line);
	if (query.eof())
	{
		bAdd = TRUE;
	}
	else
		bAdd = FALSE;
	query.finalize();
	return TRUE; 
}

void _fdb_compileStatement(AccessObj accessObj, char line[1024])
{
	if (accessObj==0) return;	
	CSQLiteAccess *pAccess = (CSQLiteAccess *)accessObj;
	CppSQLite3Statement *p = pAccess->GetSqliteStm();
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	*p = pDb->compileStatement(line);

}

void _fdb_bindField(AccessObj accessObj, int field, const CVariantEx *pvar, valueType type)
{
	if (accessObj==0) return;	
	CSQLiteAccess *pAccess = (CSQLiteAccess *)accessObj;
	CppSQLite3Statement *p = pAccess->GetSqliteStm();
	if (type==sql_NULL)
	{
		p->bindNull(field);
	}
	else if (type==sql_INT)
	{
		p->bind(field,(long)(_variant_t)*pvar );
	}
	else if (type==sql_DBL)
	{
		p->bind(field, (double)(_variant_t)*pvar);
	}
	else if (type==sql_STR)
	{
		p->bind(field,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else if (type==sql_BLOB)
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		p->bind(field,(BYTE*)arrPts.GetData(),arrPts.GetSize()*sizeof(PT_3DEX));
	}

}

void _fdb_endAddFtr(AccessObj accessObj)
{
	if (accessObj==0) return;	
	CSQLiteAccess *pAccess = (CSQLiteAccess *)accessObj;
	CppSQLite3Statement *p = pAccess->GetSqliteStm();
	// 完成操作
	p->execDML();
	p->finalize();
}

FtrQuery _fdb_beginReadFtrFromTab(AccessObj accessObj, LPCTSTR table)
{
	if (accessObj==0||table==NULL) return 0;
	
	CSQLiteAccess *pAccess = (CSQLiteAccess *)accessObj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	char line[1024];
	sprintf(line, "select * from %s;", table);
//	CppSQLite3Query query = pDb->execQuery(line);
	CppSQLite3Query *p = pAccess->GetSqliteFtrQuery();
	*p = pDb->execQuery(line);
	return (FtrQuery)p;
}

BOOL _fdb_isEof(FtrQuery query)
{
	if (query==0) return TRUE;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	if (p->eof())
	{
		return TRUE;
	}
	return FALSE;
}

int  _fdb_getNumFields(FtrQuery query)
{
	if (query==0) return 0;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	return p->numFields();
}

const char*  _fdb_getStringField(FtrQuery query, int idx, const char* szNullValue)
{
	if (query==0) return szNullValue;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	return p->getStringField(idx,szNullValue);
}

int _fdb_getIntField(FtrQuery query, int idx, int nNullValue)
{
	if (query==0) return nNullValue;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	return p->getIntField(idx,nNullValue);
}

double _fdb_getFloatField(FtrQuery query, int idx, double lfNullValue)
{
	if (query==0) return lfNullValue;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	return p->getFloatField(idx,lfNullValue);
}

const unsigned char* _fdb_getBlobField(FtrQuery query, int idx, int &nLen)
{
	if (query==0) return NULL;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	return p->getBlobField(idx,nLen);
}

int _fdb_nextRow(FtrQuery query)
{
	if (query==0) return -1;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	if (p->eof())
	{
		return -1;
	}
	p->nextRow();
	return 1;
}

void _fdb_endReadFtrFromTab(FtrQuery query)
{
	if (query==0) return;
	CppSQLite3Query *p = (CppSQLite3Query *)query;
	p->finalize();
}

int _fdb_getRecordNum(AccessObj obj, LPCTSTR tableName)
{
	if (obj==0||tableName==NULL) return 0;
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	char line[1024];
	sprintf(line, "select count(*) from %s;", tableName);
	CppSQLite3Query query = pDb->execQuery(line);
	return query.getIntField(0);
}

BOOL _fdb_readFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab)
{
	if(obj==0||ftrId==0||objTable==NULL) return FALSE;
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	CFeature *pFtr = (CFeature*)ftrId;

	char line[1024] = {0};
	CppSQLite3Query query;

	//首先获取地物所在的层ID
	sprintf(line,"select LayerID from %s where FTRID='%s';",objTable,pFtr->GetID().ToString());
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField("LayerID");
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);
	
	//通过层ID获取扩展属性表的名称
	CString strTable;
	strTable.Format("AttrTable_%s",(LPCTSTR)ename);
	
	//扩展属性表不存在，则返回 
	if(!pDb->tableExists(LPCTSTR(strTable)))
		return FALSE;
	query.finalize();
	//在扩展属性表中查找地物的对应项是否存在
	sprintf(line,"select * from %s where FTRID='%s';",strTable,pFtr->GetID().ToString());
	query = pDb->execQuery(line);
	
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
	for(int i = 2;i<query.numFields();i++)
	{
		nVauleType = query.fieldDataType(i);
		switch(nVauleType)
		{
		case SQLITE_INTEGER:
			nValue = query.getIntField(i);
			tab.AddValue(query.fieldName(i),&(CVariantEx)(_variant_t)nValue);
			break;
		case SQLITE_FLOAT:
			lfValue = query.getFloatField(i);
			tab.AddValue(query.fieldName(i),&(CVariantEx)(_variant_t)lfValue);
			break;
		case SQLITE_TEXT:
			pstr = query.getStringField(i);
			tab.AddValue(query.fieldName(i),&(CVariantEx)(_variant_t)pstr);
			break;	
		default:
			break;
		}		
	}
	query.finalize();

	return TRUE;
}

BOOL _fdb_saveFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab, int idx)
{
	if(obj==0||ftrId==0||objTable==NULL) return FALSE;
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	CFeature *pFtr = (CFeature*)ftrId;

	char line[1024] = {0};
	CString str,str0,str1;
	CppSQLite3Query query;
	//首先获取地物所在的层ID
	
	sprintf(line,"select LayerID from %s where FTRID='%s';",objTable,pFtr->GetID().ToString());
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField("LayerID");
	query.finalize();
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);
	//通过层ID获取扩展属性表的名称
	CString strTable;
	strTable.Format("AttrTable_%s",(LPCTSTR)ename);
	
	//扩展属性表不存在，则返回 
	if(!pDb->tableExists(LPCTSTR(strTable)))
		return FALSE;
	
	//在扩展属性表中查找地物的对应项是否存在
	sprintf(line,"select * from %s where FTRID='%s';",strTable,pFtr->GetID().ToString());
	query = pDb->execQuery(line);
	
	const CVariantEx *var;
	// 插入新数据
	if( query.eof() )
	{
		str.Format("insert into %s(FTRID,DELETED",strTable);
		str0.Format("values('%s',0",pFtr->GetID().ToString());
		for(int i = 2;i<query.numFields();i++)
		{	
			str+=",";
			str+=query.fieldName(i);
			if (tab.GetValue(idx,query.fieldName(i),var))
			{
				VariantToText(*var,str1);
				str0+=",";
				
				int fieldType = var->GetType();
				if(fieldType==VT_BSTR)		
				str0+="'";
				str0+=str1;
				if(fieldType==VT_BSTR)		
				str0+="'";			
			}
			else
			{
				str0+",NULL";
			}
		}
		str+=") ";
		str0+=");";
		str+=str0;
		pDb->execDML(LPCTSTR(str));
	}
	// 更新数据
	else
	{
		str.Format("update %s set DELETED=0",strTable);		
		for(int i = 2;i<query.numFields();i++)
		{	
			if (tab.GetValue(idx,query.fieldName(i),var))
			{
				str+=",";
				str+=query.fieldName(i);
				str+="=";
				
				VariantToText(*var,str1);
				if(query.fieldDataType(i)==SQLITE_TEXT)		
				str+="'";
				str+=str1;
				if(query.fieldDataType(i)==SQLITE_TEXT)		
				str+="'";					
			}				
		}
		str+=" where FTRID='";
		str+=pFtr->GetID().ToString();
		str+="';";	
		pDb->execDML(LPCTSTR(str));		
	}
	query.finalize();
    return TRUE;

}

BOOL _fdb_deleteFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable)
{
	if(obj==0||ftrId==0||objTable==NULL) return FALSE;
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	CFeature *pFtr = (CFeature*)ftrId;

	char line[1024] = {0};
	CppSQLite3Query query;
	//首先获取地物所在的层ID
	
	sprintf(line,"select LayerID from %s where FTRID='%s';",objTable,pFtr->GetID().ToString());
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField(0);
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);
	
	//通过层ID获取扩展属性表的名称
	CString strTable;
	strTable.Format("AttrTable_%s",(LPCTSTR)ename);
	
	//扩展属性表不存在，则返回 
	if(!pDb->tableExists(LPCTSTR(strTable)))
		return FALSE;
	
	//在扩展属性表中更新地物的对应项的DEL标记
	sprintf(line,"update %s set DELETED='%d' where FTRID='%s';",
		strTable,1,pFtr->GetID().ToString());
	
	pDb->execDML(line);	
	query.finalize();
	return TRUE;
}

BOOL _fdb_restoreFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable)
{
	if(obj==0||ftrId==0||objTable==NULL) return FALSE;
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	CFeature *pFtr = (CFeature*)ftrId;

	char line[1024] = {0};
	CppSQLite3Query query;
	//首先获取地物所在的层ID

	sprintf(line,"select LayerID from %s where FTRID='%s';",objTable,pFtr->GetID().ToString());
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	int LayerID = query.getIntField(0);
	sprintf(line,"select Name from DLGVectorLayer where ID=%d;",LayerID);
	query = pDb->execQuery(line);
	if(query.eof()) return FALSE;
	const char *layname = query.getStringField(0);
	CString ename = EncodeString(layname);
	
	//通过层ID获取扩展属性表的名称
	CString strTable;
	strTable.Format("AttrTable_%s",(LPCTSTR)ename);
	
	//扩展属性表不存在，则返回 
	if(!pDb->tableExists(LPCTSTR(strTable)))
		return FALSE;
	
	//在扩展属性表中更新地物的对应项的DEL标记
	sprintf(line,"update %s set DELETED='%d' where FTRID='%s';",
		strTable,0,pFtr->GetID().ToString());
	
	pDb->execDML(line);	
	query.finalize();
	return TRUE;
}

BOOL _fdb_deleteFtr(AccessObj obj, FtrId ftrId, LPCTSTR objTable)
{
	if(obj==0||ftrId==0||objTable==NULL) return FALSE;
	CSQLiteAccess *pAccess = (CSQLiteAccess *)obj;
	CppSQLite3DB *pDb = pAccess->GetSqliteDb();
	CFeature *pFtr = (CFeature*)ftrId;

	char line[1024] = {0};
	
	// 查找地物是否存在
	sprintf(line,"delete from %s where FTRID='%s';",objTable,pFtr->GetID().ToString());
	int nRet = pDb->execDML(line);
	
	return (nRet>0);
}

CSelection *_fdb_getSelection(DocId docId)
{
	if (docId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;

	return pDoc->GetSelection();
}

void _fdb_updateSelection(DocId docId)
{
	if (docId==0)return;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;
	
	pDoc->OnSelectChanged();
}

SnapId _fdb_getSnap(DocId docId)
{
	if (docId==0)return FALSE;	
	CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)docId;

	return (SnapId)&pDoc->m_snap;	
}


int _fdb_getSnapResultNum(SnapId snap, API_SNAPITEM *pItems)
{
	if( snap==0 )
		return 0;

	CSnap *pSnap = (CSnap*)snap;
	return pSnap->GetSnapResult((SNAPITEM*)pItems);
}



BOOL _fdb_getSnapResult0(SnapId snap, API_SNAPITEM *pItem)
{
	if( snap==0 )
		return FALSE;

	CSnap *pSnap = (CSnap*)snap;

	SNAPITEM item = pSnap->GetFirstSnapResult();
	memcpy(pItem,&item,sizeof(API_SNAPITEM));

	return TRUE;
}



class CMMProject  
{
public:
	CMMProject();
	virtual ~CMMProject();
	
	BOOL LoadProject(LPCTSTR xmlPath);
	void GetAllStereoID(CStringArray& arr);
	// type=0,原始像对，type=1,核线，type=2，实时核线
	BOOL GetStereoImages(LPCTSTR stereoID,  int type, CString& path1, CString& path2);
	void InitModel(LPCTSTR stereoID);	
	BOOL ConvertXYZ(PtXYZ pt1, PtXYZ& pt2);
	void Clear();
	
	CGeoBuilderPrj *m_pProj;
	CCoordCenter *m_pCvt;
};


class CMMCoordCenter
{
public:
	CMMCoordCenter();
	virtual ~CMMCoordCenter();
	
	void InitModel(CMMProject &prj, LPCTSTR stereoID);
	BOOL ConvertXYZ(PtXYZ pt1, PtXYZ& pt2);
	
	CCoordCenter *m_pCvt;
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMMProject::CMMProject()
{
	m_pProj = NULL;
	m_pCvt = NULL;
}

CMMProject::~CMMProject()
{
	Clear();
}


void CMMProject::Clear()
{
	if( m_pCvt )
		delete m_pCvt;

	m_pCvt = NULL;
	
	if( m_pProj )
		delete m_pProj;

	m_pProj = NULL;
}

void CMMProject::GetAllStereoID(CStringArray& arr)
{
	arr.RemoveAll();

	CoreObject *p = &m_pProj->core;
	for( int i=0; i<p->iStereoNum; i++)
	{
		arr.Add(p->stereo[i].sp.stereoID);
	}
}


BOOL CMMProject::GetStereoImages(LPCTSTR stereoID, int type, CString& path1, CString& path2)
{
	if (NULL == m_pProj)
	{
		return FALSE;
	}
	CoreObject *pCore = &m_pProj->core;
	if (NULL == pCore)
	{
		return FALSE;
	}
	int stereidx = -1;
	{
		for( int i=0; i<pCore->iStereoNum; i++)
		{
			if( pCore->stereo[i].sp.stereoID.CompareNoCase(stereoID)==0 )
			{
				stereidx = i;
				break;
			}
		}
	}

	if( stereidx<0 )
		return FALSE;

	path1.Empty();
	path2.Empty();

	//原始像对
	if( type==0 )
	{
		CString strId1 = pCore->stereo[stereidx].imageID[0];
		CString strId2 = pCore->stereo[stereidx].imageID[1];
		BOOL bFind1=FALSE, bFind2=FALSE;
		for( int i=0; i<pCore->iStripNum; i++)
		{
			for( int j=0; j<pCore->strip[i].iImageNum; j++)
			{
				if( !bFind1 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId1)==0 )
				{
					path1 = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						path1 = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						path1 = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						path1 = pCore->strip[i].image[j].ie.strFileName+_T(".ei");

					bFind1 = TRUE;
				}

				if( !bFind2 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId2)==0 )
				{
					path2 = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						path2 = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						path2 = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						path2 = pCore->strip[i].image[j].ie.strFileName+_T(".ei");

					bFind2 = TRUE;
				}
			}
		}		
	}
	//实时核线
	else if( type==2 )
	{
		CString strId1 = pCore->stereo[stereidx].imageID[0];
		CString strId2 = pCore->stereo[stereidx].imageID[1];
		BOOL bFind1=FALSE, bFind2=FALSE;
		for( int i=0; i<pCore->iStripNum; i++)
		{
			for( int j=0; j<pCore->strip[i].iImageNum; j++)
			{
				if( !bFind1 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId1)==0 )
				{
					path1 = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						path1 = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						path1 = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						path1 = pCore->strip[i].image[j].ie.strFileName+_T(".ei");
					
					bFind1 = TRUE;
				}
				
				if( !bFind2 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId2)==0 )
				{
					path2 = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						path2 = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						path2 = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						path2 = pCore->strip[i].image[j].ie.strFileName+_T(".ei");
					
					bFind2 = TRUE;
				}
			}
		}		
		
	}
	//核线相对
	else if( type==1 )
	{
		path1 = pCore->stereo[stereidx].se.strImage[0];
		path2 = pCore->stereo[stereidx].se.strImage[1];
	}

	if( path1.IsEmpty() || path2.IsEmpty() )
		return FALSE;

	return TRUE;
}


BOOL CMMProject::LoadProject(LPCTSTR xmlPath)
{
	Clear();

	m_pProj = new CGeoBuilderPrj();
	if( !m_pProj->LoadProject(CString(xmlPath)) )
		return FALSE;

	return TRUE;
}


void CMMProject::InitModel(LPCTSTR stereoID)
{
	if( !m_pProj )
		return;

	if( m_pCvt )
		delete m_pCvt;
	m_pCvt = new CCoordCenter();
	m_pCvt->Init(&m_pProj->core,CString(stereoID));

	return;
}



BOOL CMMProject::ConvertXYZ(PtXYZ pt1, PtXYZ& pt2)
{
	if( !m_pCvt )
		return FALSE;

	Coordinate c1, c2;
	c1.x = pt1.x;
	c1.y = pt1.y;
	c1.z = pt1.z;
	c1.lx = pt1.lx;
	c1.ly = pt1.ly;
	c1.rx = pt1.rx;
	c1.ry = pt1.ry;
	c1.iType = pt1.type;
	c2.iType = pt2.type;

	if( !m_pCvt->Convert(c1,c2) )
		return FALSE;

	pt2.x = c2.x;
	pt2.y = c2.y;
	pt2.z = c2.z;
	pt2.lx = c2.lx;
	pt2.ly = c2.ly;
	pt2.rx = c2.rx;
	pt2.ry = c2.ry;

	return TRUE;
}



CMMCoordCenter::CMMCoordCenter()
{
	m_pCvt = NULL;
}




CMMCoordCenter::~CMMCoordCenter()
{
	if( !m_pCvt	)
		delete m_pCvt;
}


BOOL CMMCoordCenter::ConvertXYZ(PtXYZ pt1, PtXYZ& pt2)
{
	if( !m_pCvt )
		return FALSE;
	
	Coordinate c1, c2;
	c1.x = pt1.x;
	c1.y = pt1.y;
	c1.z = pt1.z;
	c1.lx = pt1.lx;
	c1.ly = pt1.ly;
	c1.rx = pt1.rx;
	c1.ry = pt1.ry;
	c1.iType = pt1.type;
	c2.iType = pt2.type;
	
	if( !m_pCvt->Convert(c1,c2) )
		return FALSE;
	
	pt2.x = c2.x;
	pt2.y = c2.y;
	pt2.z = c2.z;
	pt2.lx = c2.lx;
	pt2.ly = c2.ly;
	pt2.rx = c2.rx;
	pt2.ry = c2.ry;
	
	return TRUE;
}

void CMMCoordCenter::InitModel(CMMProject &prj, LPCTSTR stereoID)
{
	if( !prj.m_pProj )
		return;

	m_pCvt = new CCoordCenter();

	m_pCvt->Init(&prj.m_pProj->core,CString(stereoID));
	
	return;
}

MMProjectId _prj_createPrj()
{
	return (MMProjectId)(new CMMProject());
}


CoordCenterId _prj_createCoordCenter()
{
	return (CoordCenterId)(new CMMCoordCenter());
}



void _prj_deletePrj(MMProjectId prj)
{
	if( !prj )
		return;

	delete (CMMProject*)prj;
}


void _prj_deleteCoordCenter(MMProjectId cc)
{
	if( !cc )
		return;
	
	delete (CMMCoordCenter*)cc;
}



BOOL _prj_loadProject(MMProjectId prj, LPCTSTR xmlPath)
{
	if( !prj )
		return FALSE;	

	return ((CMMProject*)prj)->LoadProject(xmlPath);
}


void _prj_getAllStereoIDs(MMProjectId prj, CStringArray& arr)
{
	if( !prj )
		return;	
	
	((CMMProject*)prj)->GetAllStereoID(arr);	
}


BOOL _prj_getStereoImages(MMProjectId prj, LPCTSTR stereoID,  int type, CString& path1, CString& path2)
{
	if( !prj )
		return FALSE;	
	
	return ((CMMProject*)prj)->GetStereoImages(stereoID,type,path1,path2);	
}


void _prj_initModel(MMProjectId prj, LPCTSTR stereoID)
{
	if( !prj )
		return;	
	
	((CMMProject*)prj)->InitModel(stereoID);		
}

	
BOOL _prj_convertXYZ(MMProjectId prj, PtXYZ pt1, PtXYZ& pt2)
{
	if( !prj )
		return FALSE;	
	
	return ((CMMProject*)prj)->ConvertXYZ(pt1,pt2);	
}

BOOL _prj_IsGeoImage(MMProjectId prj, LPCTSTR path, MyGeoHeader& head)
{
	if( !prj )
		return FALSE;
	
	if( ((CMMProject*)prj)->m_pProj )	
	{	
		return ((CMMProject*)prj)->m_pProj->IsGeoImage(path,*(GeoHeader*)&head);
	}
	else
	{
		((CMMProject*)prj)->m_pProj = new CGeoBuilderPrj();
		if (!((CMMProject*)prj)->m_pProj)
		{
		return FALSE;
		}
		return ((CMMProject*)prj)->m_pProj->IsGeoImage(path,*(GeoHeader*)&head);
	}
}

void _prj_clear(MMProjectId prj)
{
	if( !prj )
		return;	

	((CMMProject*)prj)->Clear();
}


void _prj_initModel2(CoordCenterId cc, MMProjectId prj, LPCTSTR stereoID)
{
	if( !cc )
		return;	
	
	((CMMCoordCenter*)cc)->InitModel(*((CMMProject*)prj),stereoID);
}


BOOL _prj_convertXYZ2(CoordCenterId cc, PtXYZ pt1, PtXYZ& pt2)
{
	if( !cc )
		return FALSE;	
	
	return ((CMMCoordCenter*)cc)->ConvertXYZ(pt1,pt2);		
}

void _prj_getModelBound(CoordCenterId cc, double x[4], double y[4], double z[4])
{
	if( !cc )
		return;	
	
	if( !((CMMCoordCenter*)cc)->m_pCvt )
		return;

	QUADRANGLE qa;
	((CMMCoordCenter*)cc)->m_pCvt->GetModelBound(&qa);

	memcpy(x,qa.lfX,sizeof(qa.lfX));
	memcpy(y,qa.lfY,sizeof(qa.lfY));
	memcpy(z,qa.lfZ,sizeof(qa.lfZ));
}

DSMId _dsm_create()
{
	return (DSMId)(new CDSM());
}


BOOL _dsm_open(DSMId dsm, LPCTSTR filePath)
{
	if( dsm )
	{
		return ((CDSM*)dsm)->Open(filePath);
	}
	return FALSE;
}


float _dsm_getZ(DSMId dsm, double x, double y)
{
	if( dsm )
	{
		return ((CDSM*)dsm)->GetZ(x,y);
	}
	return DemNoValues;
}

void _dsm_delete(DSMId dsm)
{
	if( dsm )
	{
		delete (CDSM*)dsm;
	}
}

BOOL _dsm_getInfo(DSMId dsm, double* x0, double* y0, double *dx, double *dy, int *nrow, int *ncol, float **zBuf)
{
	if( dsm )
	{
		CDSM *pDsm = (CDSM*)dsm;
		if( x0 )*x0 = pDsm->m_demInfo.lfStartX;
		if( y0 )*y0 = pDsm->m_demInfo.lfStartY;
		if( dx )*dx = pDsm->m_demInfo.lfDx;
		if( dy )*dy = pDsm->m_demInfo.lfDy;
		if( nrow )*nrow = pDsm->m_demInfo.nRow;
		if( ncol )*ncol = pDsm->m_demInfo.nColumn;
		if( zBuf )*zBuf = pDsm->m_lpHeight;

		return TRUE;
	}
	return FALSE;
}

}

