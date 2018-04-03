#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_EDITBASE_PLUGINAPI_INCLUDED
#define _INC_EDITBASE_PLUGINAPI_INCLUDED
#ifndef EXPORT_EDITBASE
#define EXPORT_EDITBASE
#endif

#include "EditDef.h "

#include "Selection.h"


MyNameSpaceBegin

typedef LONG_PTR DocId;
typedef LONG_PTR DataId;
typedef LONG_PTR LayId;
typedef LONG_PTR FtrId;
typedef LONG_PTR ViewId;
typedef LONG_PTR CommandObj;
typedef LONG_PTR AccessObj;
typedef LONG_PTR Statement;
typedef LONG_PTR FtrQuery;
typedef LONG_PTR FtrPropertyObj;
typedef LONG_PTR SnapId;
typedef LONG_PTR MMProjectId;
typedef LONG_PTR CoordCenterId;
typedef LONG_PTR DSMId;


enum valueType
{
	sql_STR = 0,
	sql_BLOB = 1,
	sql_INT = 2,
	sql_DBL = 3,
	sql_NULL
};


struct PtXYZ
{
	double x,y,z;
	double lx,ly,rx,ry;
	int type;
};

struct EXPORT_EDITBASE MyGeoHeader
{
	double lfStartX;
	double lfStartY;
	double lfKapa;
	double lfDx;
	double lfDy;
	double lfK;
	double lfZ0;
	int iColumn;
	int iRow;
	int iColor;
	bool bIsReady;
	float fContourInterval;
	double lfScale;
    MyGeoHeader()
	{
		// 宽高为0，使用时计算最大范围
		iColumn= iRow=0;
		lfDx=lfDy=10;
		lfStartX=0;
		lfStartY=0;
		lfKapa=0;
		fContourInterval=5;
	}
};


enum PtXYZ_Type
{
	typeGROUND = 0,
	typeSCANNER,
	typePHOTO,
	typeEPIPOLAR,
	typeMODEL,
	typeLOCKZ,
	typeORTHO
};


namespace ebAPI
{
	namespace Snap
	{
		enum 
		{
			modeNearPoint=0x0001,
			modeKeyPoint=0x0002,
			modeMidPoint=0x0004,
			modeIntersect=0x0008,
			modePerpPoint=0x0010,
			modeCenterPoint=0x0020,
			modeTangPoint=0x0040,
			modeEndPoint=0x0080,
			modeGrid=0x10000000
		};
	}
}

struct EXPORT_EDITBASE API_SNAPITEM
{
	API_SNAPITEM();
	BOOL IsValid();
	CFeature * pFtr;
	int bInPreObj;
	int nSnapMode;
	PT_3D pt;
};

class CDataSource;
class CEditor;

//供插件调用的若干API

//拷贝扩展属性
typedef BOOL  (*pfdb_copyXAttributes)(DocId docId, FtrId srcFtrId, FtrId desFtrId);

// 获取扩展属性
typedef BOOL  (*pfdb_getXAttributes)(DocId docId, FtrId ftr, CValueTable& tab);

//视图的更新
typedef void  (*pfdb_refreshViews)(DocId docId, HINTCODE intCode, long data);

// 获取地物属性句柄
typedef FtrPropertyObj  (*pfdb_newFtrPropertyObj)(FtrId ftrId);

// 释放地物属性句柄
typedef void (*pfdb_deleteFtrPropertyObj)(FtrPropertyObj propertyObj);

// 更新地物属性窗口
typedef void  (*pfdb_refreshFtrPropertyPanel)(FtrPropertyObj propertyObj);

//获得当前激活视图
typedef ViewId  (*pfdb_getCurView)(DocId docId);

// 激活插件命令项
typedef BOOL (*pfdb_startPluginCommand)(LPCTSTR strPluginName, DocId docId, int id);

//获得当前文档
typedef DocId  (*pfdb_getCurDoc)();

// 获取当前命令 
typedef CommandObj  (*pfdb_getCurCommand)(DocId docId);

//启动指定命令(命令名称或者快捷字符串)
typedef BOOL(*pfdb_StartCmd)(LPCTSTR cmd);

//得到地物所在的层
typedef LayId  (*pfdb_getLayerOfFtr)(DocId docId, FtrId ftrId);

//得到层的颜色
typedef long  (*pfdb_getColorOfLayer)(LayId layId);

//修改层的颜色
typedef void  (*pfdb_setColorOfLayer)(LayId layId, long color);

//是否锁定
typedef BOOL  (*pfdb_isLockedOfLayer)(LayId layId);

//修改锁定状态
typedef void  (*pfdb_enableLockedOfLayer)(LayId layId, BOOL bLocked);

//是否可见
typedef BOOL  (*pfdb_isVisibleOfLayer)(LayId layId);

//修改显示状态
typedef void  (*pfdb_enableVisibleOfLayer)(LayId layId, BOOL bLocked);

//得到层名
typedef LPCTSTR  (*pfdb_getNameOfLayer)(LayId layId);

//得到层ID
typedef int (*pfdb_getIdOfLayer)(LayId layId);

//得到层码
typedef __int64 (*pfdb_getCodeOfLayer)(DocId docId, LayId layId);

//增加地物到文档中
typedef BOOL  (*pfdb_addFtr)(DocId docId, FtrId ftrId, LayId layId);

typedef BOOL  (*pfdb_addFtrWithProperties)(DocId docId, FtrId ftrId, int layId);

//删除地物
typedef BOOL  (*pfdb_delFtr)(DocId docId, FtrId ftrId, BOOL bUpdateSel);

//得到某个文档中，层的个数
typedef int  (*pfdb_getLayCntOfDoc)(DocId docId);

//得到文档中第idx个层
typedef LayId (*pfdb_getLayerByIdx)(DocId docId, int idx);

//得到层中地物的个数
typedef int  (*pfdb_getFtrCntOfLay)(LayId lay);

//得到层lay中地idx个地物
typedef FtrId  (*pfdb_getFtrFromLay)(LayId lay, int idx, BOOL bIncDeleted);

//更新视图参数
typedef long  (*pfdb_updateStereoParam)(ViewId viewId, LayId lay);

//得到比例尺
typedef long (*pfdb_getScale)(DocId docId);

//得到边界范围
typedef void (*pfdb_getWorkSpaceBound)(DocId docId, PT_3D pts[4]);
// 
// //得到层的mode
// typedef DWORD (*pfdb_getModeOfLay)(LayId layId);
// 
// //设置层的mode
// typedef void (*pfdb_setModeOfLay)(LayId layId, DWORD mode);

//层码与层名的相互转换
typedef BOOL (*pfdb_findUserIdx)(DocId docId, BOOL bFindStr, __int64 &code, CString& name);

//得到地物的符号
typedef BOOL (*pfdb_getSymbol)(DocId docId, FtrId ftrId, CPtrArray &arrSymbol);

//开始进度条
typedef void (*pfdb_progressBegin)(int nSum);

//进度条步进
typedef void (*pfdb_progressStep)(int nStep);

//进度条结束
typedef void (*pfdb_progressEnd)();

//输出窗口
typedef void (*pfdb_outputstring)(LPCTSTR text, BOOL bNewLine);

//执行SQL语句，非查询语句
typedef BOOL (*pfdb_excDML)(AccessObj obj, char sql[1024]);

//执行SQL查询语句，返回结果存储在retTable中
typedef BOOL (*pfdb_excQuery)(AccessObj obj, char sql[1024], CValueTable &retTable);

//修改属性面板的显示，主要用在Command命令中
typedef void (*pfdb_modifyPropertyUI)(WPARAM wParam, FtrId ftrId, void *&pSel);

//判断是否记录了定位点
typedef BOOL (*pfdb_isAutoSetAnchor)(DocId docId);

//设置定位点
typedef void (*pfdb_setAnchorPoint)(DocId docId, PT_3D pt);

//激活层名为strLayer的固定层
typedef int (*pfdb_activeLayer)(DocId docId, LPCTSTR strLayer, BOOL bSaveRecent);//或许不需要

//设置当前层
typedef void (*pfdb_setCurLayer)(DocId docId, LayId lay);

//获取当前层
typedef LayId (*pfdb_getCurLayer)(DocId docId);

//创建层
typedef LayId (*pfdb_createLayer)(DocId docId, LPCTSTR layName);

//添加层
typedef BOOL (*pfdb_addLayer)(DocId docId, LayId lay);

//创建某层的缺省地物
typedef FtrId (*pfdb_createDefaultFeature)(LayId lay, DocId docId, int nCls);

//得到层名为layName的固定层
typedef LayId (*pfdb_getLocalLayer)(DocId docId, LPCTSTR layName);

//得到层名为layName的层
typedef LayId (*pfdb_getLayer)(DocId docId, LPCTSTR layName);

//持久化地物（目前没实现，被下面接口替代）
typedef void  (*pfdb_saveFtr)(AccessObj accessObj, FtrId ftrId, LPCTSTR table);

//地物注记转化为文本地物
typedef BOOL (*pfdb_annotToText)(DocId docId, FtrId ftrId, LayId layId, FtrId tmp, LPCTSTR name);

//开始往数据库文件中添加地物
typedef BOOL (*pfdb_beginAddFtr)(AccessObj accessObj, FtrId ftrId, LPCTSTR table, BOOL &bAdd);

//编译执行语句
typedef void (*pfdb_compileStatement)(AccessObj accessObj, char line[1024]);

//绑定某域到变量
typedef void (*pfdb_bindField)(AccessObj accessObj, int field, const CVariantEx *pvar, valueType type);

//结束添加地物
typedef void (*pfdb_endAddFtr)(AccessObj accessObj);

//开始反持久化
typedef FtrQuery (*pfdb_beginReadFtrFromTab)(AccessObj accessObj, LPCTSTR table);

//时候读取到数据库表末尾
typedef BOOL (*pfdb_isEof)(FtrQuery query);

//得到查询结果的字段数
typedef int  (*pfdb_getNumFields)(FtrQuery query);

//得到当前记录的字符串域的字符值
typedef const char*  (*pfdb_getStringField)(FtrQuery query, int idx, const char* szNullValue);

//得到当前记录的整型域的整型值
typedef int (*pfdb_getIntField)(FtrQuery query, int idx, int nNullValue);

//得到当前记录的浮点型域的浮点值
typedef double (*pfdb_getFloatField)(FtrQuery query, int idx, double lfNullValue);

//得到当前记录的块域的值的首地址
typedef const unsigned char* (*pfdb_getBlobField)(FtrQuery query, int idx, int &nLen);

//步进到下一条记录
typedef int (*pfdb_nextRow)(FtrQuery query);

//结束地物的反持久化
typedef void (*pfdb_endReadFtrFromTab)(FtrQuery query);

//得到数据库中某个表的记录的条数
typedef int (*pfdb_getRecordNum)(AccessObj obj, LPCTSTR tableName);

//读取objTable表中地物ftrId的扩展属性
typedef BOOL (*pfdb_readFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab);

//保存objTable表中地物ftrId的扩展属性
typedef BOOL (*pfdb_saveFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab, int idx);

//删除objTable表中地物ftrId对应的扩展属性
typedef BOOL (*pfdb_deleteFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

//恢复地物对应的扩展属性
typedef BOOL (*pfdb_restoreFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

//删除objTable表中地物ftrId
typedef BOOL (*pfdb_deleteFtr)(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

typedef CSelection * (*pfdb_getSelection)(DocId docId);

typedef void (*pfdb_updateSelection)(DocId docId);

typedef SnapId (*pfdb_getSnap)(DocId docId);

typedef int (*pfdb_getSnapResultNum)(SnapId snap, API_SNAPITEM *pItems);

typedef BOOL (*pfdb_getSnapResult0)(SnapId snap, API_SNAPITEM *pItem);

typedef MMProjectId (*pprj_createPrj)();
typedef CoordCenterId (*pprj_createCoordCenter)();

typedef void (*pprj_deletePrj)(MMProjectId prj);
typedef void (*pprj_deleteCoordCenter)(MMProjectId cc);

typedef BOOL (*pprj_loadProject)(MMProjectId prj, LPCTSTR xmlPath);
typedef void (*pprj_getAllStereoIDs)(MMProjectId prj, CStringArray& arr);
typedef BOOL (*pprj_getStereoImages)(MMProjectId prj, LPCTSTR stereoID,  int type, CString& path1, CString& path2);
typedef void (*pprj_initModel)(MMProjectId prj, LPCTSTR stereoID);	
typedef BOOL (*pprj_convertXYZ)(MMProjectId prj, PtXYZ pt1, PtXYZ& pt2);
typedef BOOL (*pprj_IsGeoImage)(MMProjectId prj, LPCTSTR path, MyGeoHeader& head);
typedef void (*pprj_clear)(MMProjectId prj);
typedef void (*pprj_initModel2)(CoordCenterId cc, MMProjectId prj, LPCTSTR stereoID);
typedef BOOL (*pprj_convertXYZ2)(CoordCenterId cc, PtXYZ pt1, PtXYZ& pt2);
typedef void (*pprj_getModelBound)(CoordCenterId cc, double x[4], double y[4], double z[4]);

typedef DSMId (*pdsm_create)();
typedef BOOL (*pdsm_open)(DSMId dsm, LPCTSTR filePath);
typedef float (*pdsm_getZ)(DSMId dsm, double x, double y);
typedef void (*pdsm_delete)(DSMId dsm);
typedef BOOL (*pdsm_getInfo)(DSMId dsm, double* x0, double* y0, double *dx, double *dy, int *nrow, int *ncol, float **zBuf);

//获取存储在hResource句柄中的资源字符串
CString EXPORT_EDITBASE StrFromModuleResID(UINT id, HMODULE hResource);

//FtrId转化为CFeature *
EXPORT_EDITBASE inline CFeature* fdb_FtrFromFtrID(FtrId ftrId){ return (CFeature*)(ftrId); }

////CFeature *转化为FtrId
inline FtrId  EXPORT_EDITBASE fdb_FtrIDFromFtr(CFeature * pFtr){return FtrId(pFtr);}

//CEditor *转化为DocId
inline DocId  EXPORT_EDITBASE fdb_DocIDFromDoc(CEditor *pEdit){
	return DocId(pEdit);
}

//DocId转化为 CEditor *
EXPORT_EDITBASE inline CEditor* fdb_DocFromDocID(DocId docId){ return (CEditor*)docId; }

//向视图发送消息
long  EXPORT_EDITBASE fdb_sendMessageToView(ViewId viewId, UINT message, WPARAM wParam, LPARAM lParam);

extern pfdb_copyXAttributes EXPORT_EDITBASE fdb_copyXAttributes;

extern pfdb_getXAttributes EXPORT_EDITBASE fdb_getXAttributes;

extern pfdb_refreshViews EXPORT_EDITBASE fdb_refreshViews;

extern pfdb_newFtrPropertyObj EXPORT_EDITBASE fdb_newFtrPropertyObj;

extern pfdb_deleteFtrPropertyObj EXPORT_EDITBASE fdb_deleteFtrPropertyObj;

extern pfdb_refreshFtrPropertyPanel EXPORT_EDITBASE fdb_refreshFtrPropertyPanel;

extern pfdb_getCurView EXPORT_EDITBASE fdb_getCurView;

extern pfdb_startPluginCommand EXPORT_EDITBASE fdb_startPluginCommand; 

extern pfdb_getCurDoc EXPORT_EDITBASE fdb_getCurDoc;

extern pfdb_getCurCommand EXPORT_EDITBASE fdb_getCurCommand;

extern pfdb_StartCmd EXPORT_EDITBASE fdb_StartCmd;

extern pfdb_getLayerOfFtr EXPORT_EDITBASE fdb_getLayerOfFtr;

extern pfdb_getColorOfLayer EXPORT_EDITBASE fdb_getColorOfLayer;

extern pfdb_getNameOfLayer EXPORT_EDITBASE fdb_getNameOfLayer; 

extern pfdb_getIdOfLayer EXPORT_EDITBASE fdb_getIdOfLayer; 

extern pfdb_getCodeOfLayer EXPORT_EDITBASE fdb_getCodeOfLayer; 

extern pfdb_addFtr EXPORT_EDITBASE  fdb_addFtr;

extern pfdb_addFtrWithProperties EXPORT_EDITBASE fdb_addFtrWithProperties;

extern pfdb_delFtr EXPORT_EDITBASE fdb_delFtr;

extern pfdb_getLayCntOfDoc EXPORT_EDITBASE fdb_getLayCntOfDoc;

extern pfdb_getLayerByIdx EXPORT_EDITBASE fdb_getLayerByIdx;

extern pfdb_getFtrCntOfLay EXPORT_EDITBASE fdb_getFtrCntOfLay;

extern pfdb_getFtrFromLay EXPORT_EDITBASE fdb_getFtrFromLay;

extern pfdb_updateStereoParam EXPORT_EDITBASE fdb_updateStereoParam;

extern pfdb_getScale EXPORT_EDITBASE  fdb_getScale;

extern pfdb_getWorkSpaceBound EXPORT_EDITBASE  fdb_getWorkSpaceBound;

extern pfdb_findUserIdx EXPORT_EDITBASE  fdb_findUserIdx;

extern pfdb_getSymbol EXPORT_EDITBASE fdb_getSymbol;
// extern pfdb_getModeOfLay EXPORT_EDITBASE  fdb_getModeOfLay;
// 
// extern pfdb_setModeOfLay EXPORT_EDITBASE  fdb_setModeOfLay;

extern pfdb_progressBegin EXPORT_EDITBASE fdb_progressBegin;

extern pfdb_progressStep EXPORT_EDITBASE fdb_progressStep;

extern pfdb_progressEnd EXPORT_EDITBASE fdb_progressEnd;

extern pfdb_outputstring EXPORT_EDITBASE fdb_outputstring;

extern pfdb_excDML EXPORT_EDITBASE fdb_excDML;

extern pfdb_excQuery EXPORT_EDITBASE fdb_excQuery;

extern pfdb_modifyPropertyUI EXPORT_EDITBASE fdb_modifyPropertyUI;

extern pfdb_isAutoSetAnchor EXPORT_EDITBASE fdb_isAutoSetAnchor;

extern pfdb_setAnchorPoint EXPORT_EDITBASE fdb_setAnchorPoint;

extern pfdb_activeLayer EXPORT_EDITBASE fdb_activeLayer;

extern pfdb_setCurLayer EXPORT_EDITBASE fdb_setCurLayer;

extern pfdb_getCurLayer EXPORT_EDITBASE fdb_getCurLayer;

extern pfdb_createLayer EXPORT_EDITBASE fdb_createLayer;

extern pfdb_addLayer EXPORT_EDITBASE fdb_addLayer;

extern pfdb_setColorOfLayer EXPORT_EDITBASE fdb_setColorOfLayer;

extern pfdb_isLockedOfLayer EXPORT_EDITBASE fdb_isLockedOfLayer;

extern pfdb_enableLockedOfLayer EXPORT_EDITBASE fdb_enableLockedOfLayer;

extern pfdb_isVisibleOfLayer EXPORT_EDITBASE fdb_isVisibleOfLayer;

extern pfdb_enableVisibleOfLayer EXPORT_EDITBASE fdb_enableVisibleOfLayer;

extern pfdb_createDefaultFeature EXPORT_EDITBASE fdb_createDefaultFeature;

extern pfdb_getLocalLayer EXPORT_EDITBASE fdb_getLocalLayer;

extern pfdb_getLayer EXPORT_EDITBASE fdb_getLayer;

extern pfdb_annotToText EXPORT_EDITBASE fdb_annotToText;

extern pfdb_saveFtr EXPORT_EDITBASE fdb_saveFtr;

extern pfdb_beginAddFtr EXPORT_EDITBASE fdb_beginAddFtr;

extern pfdb_compileStatement EXPORT_EDITBASE fdb_compileStatement;

extern pfdb_bindField EXPORT_EDITBASE fdb_bindField;

extern pfdb_endAddFtr EXPORT_EDITBASE fdb_endAddFtr;

extern pfdb_beginReadFtrFromTab EXPORT_EDITBASE fdb_beginReadFtrFromTab;

extern pfdb_isEof EXPORT_EDITBASE fdb_isEof;

extern pfdb_getNumFields EXPORT_EDITBASE fdb_getNumFields;

extern pfdb_getStringField EXPORT_EDITBASE fdb_getStringField;

extern pfdb_getIntField EXPORT_EDITBASE fdb_getIntField;

extern pfdb_getFloatField EXPORT_EDITBASE fdb_getFloatField;

extern pfdb_getBlobField EXPORT_EDITBASE fdb_getBlobField;

extern pfdb_nextRow EXPORT_EDITBASE fdb_nextRow;

extern pfdb_endReadFtrFromTab EXPORT_EDITBASE fdb_endReadFtrFromTab;

extern pfdb_getRecordNum EXPORT_EDITBASE fdb_getRecordNum;

extern pfdb_readFtrXAttribute EXPORT_EDITBASE fdb_readFtrXAttribute;

extern pfdb_saveFtrXAttribute EXPORT_EDITBASE fdb_saveFtrXAttribute;

extern pfdb_deleteFtrXAttribute EXPORT_EDITBASE fdb_deleteFtrXAttribute;

extern pfdb_restoreFtrXAttribute EXPORT_EDITBASE fdb_restoreFtrXAttribute;

extern pfdb_deleteFtr EXPORT_EDITBASE fdb_deleteFtr;

extern pfdb_getSelection EXPORT_EDITBASE fdb_getSelection;

extern pfdb_updateSelection EXPORT_EDITBASE fdb_updateSelection;

extern pfdb_getSnap EXPORT_EDITBASE fdb_getSnap;

extern pfdb_getSnapResultNum EXPORT_EDITBASE fdb_getSnapResultNum;

extern pfdb_getSnapResult0 EXPORT_EDITBASE fdb_getSnapResult0;


extern pprj_createPrj EXPORT_EDITBASE prj_createPrj;
extern pprj_createCoordCenter EXPORT_EDITBASE prj_createCoordCenter;
extern pprj_deletePrj EXPORT_EDITBASE prj_deletePrj;
extern pprj_deleteCoordCenter EXPORT_EDITBASE prj_deleteCoordCenter;
extern pprj_loadProject EXPORT_EDITBASE prj_loadProject;
extern pprj_getAllStereoIDs EXPORT_EDITBASE prj_getAllStereoIDs;
extern pprj_getStereoImages EXPORT_EDITBASE prj_getStereoImages;
extern pprj_initModel EXPORT_EDITBASE prj_initModel;
extern pprj_convertXYZ EXPORT_EDITBASE prj_convertXYZ;
extern pprj_IsGeoImage EXPORT_EDITBASE prj_IsGeoImage;
extern pprj_clear EXPORT_EDITBASE prj_clear;
extern pprj_initModel2 EXPORT_EDITBASE prj_initModel2;
extern pprj_convertXYZ2 EXPORT_EDITBASE prj_convertXYZ2;
extern pprj_getModelBound EXPORT_EDITBASE prj_getModelBound;

extern pdsm_create EXPORT_EDITBASE dsm_create;
extern pdsm_open EXPORT_EDITBASE dsm_open;
extern pdsm_getZ EXPORT_EDITBASE dsm_getZ;
extern pdsm_delete EXPORT_EDITBASE dsm_delete;
extern pdsm_getInfo EXPORT_EDITBASE dsm_getInfo;

MyNameSpaceEnd
#endif