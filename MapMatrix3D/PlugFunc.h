// PlugFunc.h: interface for the PlugFunc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGFUNC_H__B6C4E6BE_4D4C_45CF_B858_09505E4B41DC__INCLUDED_)
#define AFX_PLUGFUNC_H__B6C4E6BE_4D4C_45CF_B858_09505E4B41DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PluginAPI.h "

namespace plugins
{
void initPluginApi();
//拷贝扩展属性
BOOL  _fdb_copyXAttributes(DocId docId, FtrId srcFtrId, FtrId desFtrId);

//获取扩展属性
BOOL  _fdb_getXAttributes(DocId docId, FtrId ftr, CValueTable& tab);

//视图的更新
void  _fdb_refreshViews(DocId docId, HINTCODE intCode, long data);

// 获取地物属性句柄
FtrPropertyObj _fdb_newFtrPropertyObj(FtrId ftrId);

// 释放地物属性句柄
void _fdb_deleteFtrPropertyObj(FtrPropertyObj propertyObj);

// 更新地物属性窗口
void  _fdb_refreshFtrPropertyPanel(FtrPropertyObj propertyObj);

//获得当前激活视图
ViewId  _fdb_getCurView(DocId docId);

// 激活插件命令项
BOOL _fdb_startPluginCommand(LPCTSTR strPluginName, DocId docId, int id);

//获得当前文档
DocId  _fdb_getCurDoc();

// 获取当前命令
CommandObj  _fdb_getCurCommand(DocId docId);

//启动指定命令
BOOL _fdb_StartCmd(LPCTSTR cmd);

//得到地物所在的层
LayId  _fdb_getLayerOfFtr(DocId docId, FtrId ftrId);

//得到层的颜色
long  _fdb_getColorOfLayer(LayId layId);

//得到层名
LPCTSTR  _fdb_getNameOfLayer(LayId layId);

//得到层Id
int _fdb_getIdOfLayer(LayId layId);

//增加地物到文档中
BOOL  _fdb_addFtr(DocId docId, FtrId ftrId, LayId layId);

BOOL  _fdb_addFtrWithProperties(DocId docId, FtrId ftrId, int layId);

//删除地物
BOOL  _fdb_delFtr(DocId docId, FtrId ftrId, BOOL bUpdateSel = TRUE);

//得到某个文档中，层的个数
int  _fdb_getLayCntOfDoc(DocId docId);

//得到文档中第idx个层
LayId _fdb_getLayerByIdx(DocId docId, int idx);

//得到层中地物的个数
int  _fdb_getFtrCntOfLay(LayId lay);

//得到层lay中地idx个地物
FtrId  _fdb_getFtrFromLay(LayId lay, int idx, BOOL bIncDeleted = FALSE);

//更新视图参数
long  _fdb_updateStereoParam(ViewId viewId, LayId lay);

long _fdb_getScale(DocId docId);

void _fdb_getWorkSpaceBound(DocId docId, PT_3D pts[4]);

BOOL _fdb_findUserIdx(DocId docId, BOOL bFindStr, __int64 &code, CString& name);

BOOL _fdb_getSymbol(DocId docId, FtrId ftrId, CPtrArray &arrSymbol);

__int64 _fdb_getCodeOfLayer(DocId docId, LayId layId);

void _fdb_progressBegin(int nSum);

void _fdb_progressStep(int nStep = 1);

void _fdb_progressEnd();

void _fdb_outputstring(LPCTSTR text, BOOL bNewLine);

BOOL _fdb_excDMl(AccessObj obj, char sql[1024]);

BOOL _fdb_excQuery(AccessObj obj, char sql[1024], CValueTable &retTable);

void _fdb_modifyPropertyUI(WPARAM wParam,FtrId ftrId, void *&pSel);

BOOL _fdb_isAutoSetAnchor(DocId docId);

void _fdb_setAnchorPoint(DocId docId, PT_3D pt);

int _fdb_activeLayer(DocId docId, LPCTSTR strLayer, BOOL bSaveRecent);

void _fdb_setCurLayer(DocId docId, LayId lay);

LayId _fdb_getCurLayer(DocId docId);

LayId _fdb_createLayer(DocId docId, LPCTSTR layName);

BOOL  _fdb_addLayer(DocId docId, LayId layId);

void  _fdb_setColorOfLayer(LayId layId, long color);

BOOL  _fdb_isLockedOfLayer(LayId layId);

void  _fdb_enableLockedOfLayer(LayId layId, BOOL bLocked);

BOOL  _fdb_isVisibleOfLayer(LayId layId);

void  _fdb_enableVisibleOfLayer(LayId layId, BOOL bVisible);

FtrId _fdb_createDefaultFeature(LayId lay, DocId docId, int nCls);

LayId _fdb_getLocalLayer(DocId docId, LPCTSTR layName);

LayId _fdb_getLayer(DocId docId, LPCTSTR layName);

BOOL _fdb_annotToText(DocId docId, FtrId ftrId, LayId layId, FtrId tmp, LPCTSTR name);

void  _fdb_saveFtr(AccessObj accessObj, FtrId ftrId, LPCTSTR table);

BOOL _fdb_beginAddFtr(AccessObj accessObj, FtrId ftrId, LPCTSTR table, BOOL &bAdd);

void _fdb_compileStatement(AccessObj accessObj, char line[1024]);

void _fdb_bindField(AccessObj accessObj, int field, const CVariantEx *pvar, valueType type );

void _fdb_endAddFtr(AccessObj accessObj);

FtrQuery _fdb_beginReadFtrFromTab(AccessObj accessObj, LPCTSTR table);

BOOL _fdb_isEof(FtrQuery query);

int  _fdb_getNumFields(FtrQuery query);

const char*  _fdb_getStringField(FtrQuery query, int idx, const char* szNullValue);

int _fdb_getIntField(FtrQuery query, int idx, int nNullValue);

double _fdb_getFloatField(FtrQuery query, int idx, double lfNullValue);

const unsigned char* _fdb_getBlobField(FtrQuery query, int idx, int &nLen);

int _fdb_nextRow(FtrQuery query);

void _fdb_endReadFtrFromTab(FtrQuery query);

int _fdb_getRecordNum(AccessObj obj, LPCTSTR tableName);

BOOL _fdb_readFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab);

BOOL _fdb_saveFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab, int idx);

BOOL _fdb_deleteFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

BOOL _fdb_restoreFtrXAttribute(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

BOOL _fdb_deleteFtr(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

CSelection *_fdb_getSelection(DocId docId);

void _fdb_updateSelection(DocId docId);

SnapId _fdb_getSnap(DocId docId);

int _fdb_getSnapResultNum(SnapId snap, API_SNAPITEM *pItems);

BOOL _fdb_getSnapResult0(SnapId snap, API_SNAPITEM *pItem);

MMProjectId _prj_createPrj();
CoordCenterId _prj_createCoordCenter();

void _prj_deletePrj(MMProjectId prj);
void _prj_deleteCoordCenter(MMProjectId cc);
BOOL _prj_loadProject(MMProjectId prj, LPCTSTR xmlPath);
void _prj_getAllStereoIDs(MMProjectId prj, CStringArray& arr);
BOOL _prj_getStereoImages(MMProjectId prj, LPCTSTR stereoID,  int type, CString& path1, CString& path2);
void _prj_initModel(MMProjectId prj, LPCTSTR stereoID);	
BOOL _prj_convertXYZ(MMProjectId prj, PtXYZ pt1, PtXYZ& pt2);
BOOL _prj_IsGeoImage(MMProjectId prj, LPCTSTR path, MyGeoHeader& head);
void _prj_clear(MMProjectId prj);
void _prj_initModel2(CoordCenterId cc, MMProjectId prj, LPCTSTR stereoID);
BOOL _prj_convertXYZ2(CoordCenterId cc, PtXYZ pt1, PtXYZ& pt2);
void _prj_getModelBound(CoordCenterId cc, double x[4], double y[4], double z[4]);


DSMId _dsm_create();
BOOL _dsm_open(DSMId dsm, LPCTSTR filePath);
float _dsm_getZ(DSMId dsm, double x, double y);
void _dsm_delete(DSMId dsm);
BOOL _dsm_getInfo(DSMId dsm, double* x0, double* y0, double *dx, double *dy, int *nrow, int *ncol, float **zBuf);

}
#endif // !defined(AFX_PLUGFUNC_H__B6C4E6BE_4D4C_45CF_B858_09505E4B41DC__INCLUDED_)
