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
		// ���Ϊ0��ʹ��ʱ�������Χ
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

//��������õ�����API

//������չ����
typedef BOOL  (*pfdb_copyXAttributes)(DocId docId, FtrId srcFtrId, FtrId desFtrId);

// ��ȡ��չ����
typedef BOOL  (*pfdb_getXAttributes)(DocId docId, FtrId ftr, CValueTable& tab);

//��ͼ�ĸ���
typedef void  (*pfdb_refreshViews)(DocId docId, HINTCODE intCode, long data);

// ��ȡ�������Ծ��
typedef FtrPropertyObj  (*pfdb_newFtrPropertyObj)(FtrId ftrId);

// �ͷŵ������Ծ��
typedef void (*pfdb_deleteFtrPropertyObj)(FtrPropertyObj propertyObj);

// ���µ������Դ���
typedef void  (*pfdb_refreshFtrPropertyPanel)(FtrPropertyObj propertyObj);

//��õ�ǰ������ͼ
typedef ViewId  (*pfdb_getCurView)(DocId docId);

// ������������
typedef BOOL (*pfdb_startPluginCommand)(LPCTSTR strPluginName, DocId docId, int id);

//��õ�ǰ�ĵ�
typedef DocId  (*pfdb_getCurDoc)();

// ��ȡ��ǰ���� 
typedef CommandObj  (*pfdb_getCurCommand)(DocId docId);

//����ָ������(�������ƻ��߿���ַ���)
typedef BOOL(*pfdb_StartCmd)(LPCTSTR cmd);

//�õ��������ڵĲ�
typedef LayId  (*pfdb_getLayerOfFtr)(DocId docId, FtrId ftrId);

//�õ������ɫ
typedef long  (*pfdb_getColorOfLayer)(LayId layId);

//�޸Ĳ����ɫ
typedef void  (*pfdb_setColorOfLayer)(LayId layId, long color);

//�Ƿ�����
typedef BOOL  (*pfdb_isLockedOfLayer)(LayId layId);

//�޸�����״̬
typedef void  (*pfdb_enableLockedOfLayer)(LayId layId, BOOL bLocked);

//�Ƿ�ɼ�
typedef BOOL  (*pfdb_isVisibleOfLayer)(LayId layId);

//�޸���ʾ״̬
typedef void  (*pfdb_enableVisibleOfLayer)(LayId layId, BOOL bLocked);

//�õ�����
typedef LPCTSTR  (*pfdb_getNameOfLayer)(LayId layId);

//�õ���ID
typedef int (*pfdb_getIdOfLayer)(LayId layId);

//�õ�����
typedef __int64 (*pfdb_getCodeOfLayer)(DocId docId, LayId layId);

//���ӵ��ﵽ�ĵ���
typedef BOOL  (*pfdb_addFtr)(DocId docId, FtrId ftrId, LayId layId);

typedef BOOL  (*pfdb_addFtrWithProperties)(DocId docId, FtrId ftrId, int layId);

//ɾ������
typedef BOOL  (*pfdb_delFtr)(DocId docId, FtrId ftrId, BOOL bUpdateSel);

//�õ�ĳ���ĵ��У���ĸ���
typedef int  (*pfdb_getLayCntOfDoc)(DocId docId);

//�õ��ĵ��е�idx����
typedef LayId (*pfdb_getLayerByIdx)(DocId docId, int idx);

//�õ����е���ĸ���
typedef int  (*pfdb_getFtrCntOfLay)(LayId lay);

//�õ���lay�е�idx������
typedef FtrId  (*pfdb_getFtrFromLay)(LayId lay, int idx, BOOL bIncDeleted);

//������ͼ����
typedef long  (*pfdb_updateStereoParam)(ViewId viewId, LayId lay);

//�õ�������
typedef long (*pfdb_getScale)(DocId docId);

//�õ��߽緶Χ
typedef void (*pfdb_getWorkSpaceBound)(DocId docId, PT_3D pts[4]);
// 
// //�õ����mode
// typedef DWORD (*pfdb_getModeOfLay)(LayId layId);
// 
// //���ò��mode
// typedef void (*pfdb_setModeOfLay)(LayId layId, DWORD mode);

//������������໥ת��
typedef BOOL (*pfdb_findUserIdx)(DocId docId, BOOL bFindStr, __int64 &code, CString& name);

//�õ�����ķ���
typedef BOOL (*pfdb_getSymbol)(DocId docId, FtrId ftrId, CPtrArray &arrSymbol);

//��ʼ������
typedef void (*pfdb_progressBegin)(int nSum);

//����������
typedef void (*pfdb_progressStep)(int nStep);

//����������
typedef void (*pfdb_progressEnd)();

//�������
typedef void (*pfdb_outputstring)(LPCTSTR text, BOOL bNewLine);

//ִ��SQL��䣬�ǲ�ѯ���
typedef BOOL (*pfdb_excDML)(AccessObj obj, char sql[1024]);

//ִ��SQL��ѯ��䣬���ؽ���洢��retTable��
typedef BOOL (*pfdb_excQuery)(AccessObj obj, char sql[1024], CValueTable &retTable);

//�޸�����������ʾ����Ҫ����Command������
typedef void (*pfdb_modifyPropertyUI)(WPARAM wParam, FtrId ftrId, void *&pSel);

//�ж��Ƿ��¼�˶�λ��
typedef BOOL (*pfdb_isAutoSetAnchor)(DocId docId);

//���ö�λ��
typedef void (*pfdb_setAnchorPoint)(DocId docId, PT_3D pt);

//�������ΪstrLayer�Ĺ̶���
typedef int (*pfdb_activeLayer)(DocId docId, LPCTSTR strLayer, BOOL bSaveRecent);//������Ҫ

//���õ�ǰ��
typedef void (*pfdb_setCurLayer)(DocId docId, LayId lay);

//��ȡ��ǰ��
typedef LayId (*pfdb_getCurLayer)(DocId docId);

//������
typedef LayId (*pfdb_createLayer)(DocId docId, LPCTSTR layName);

//��Ӳ�
typedef BOOL (*pfdb_addLayer)(DocId docId, LayId lay);

//����ĳ���ȱʡ����
typedef FtrId (*pfdb_createDefaultFeature)(LayId lay, DocId docId, int nCls);

//�õ�����ΪlayName�Ĺ̶���
typedef LayId (*pfdb_getLocalLayer)(DocId docId, LPCTSTR layName);

//�õ�����ΪlayName�Ĳ�
typedef LayId (*pfdb_getLayer)(DocId docId, LPCTSTR layName);

//�־û����Ŀǰûʵ�֣�������ӿ������
typedef void  (*pfdb_saveFtr)(AccessObj accessObj, FtrId ftrId, LPCTSTR table);

//����ע��ת��Ϊ�ı�����
typedef BOOL (*pfdb_annotToText)(DocId docId, FtrId ftrId, LayId layId, FtrId tmp, LPCTSTR name);

//��ʼ�����ݿ��ļ�����ӵ���
typedef BOOL (*pfdb_beginAddFtr)(AccessObj accessObj, FtrId ftrId, LPCTSTR table, BOOL &bAdd);

//����ִ�����
typedef void (*pfdb_compileStatement)(AccessObj accessObj, char line[1024]);

//��ĳ�򵽱���
typedef void (*pfdb_bindField)(AccessObj accessObj, int field, const CVariantEx *pvar, valueType type);

//������ӵ���
typedef void (*pfdb_endAddFtr)(AccessObj accessObj);

//��ʼ���־û�
typedef FtrQuery (*pfdb_beginReadFtrFromTab)(AccessObj accessObj, LPCTSTR table);

//ʱ���ȡ�����ݿ��ĩβ
typedef BOOL (*pfdb_isEof)(FtrQuery query);

//�õ���ѯ������ֶ���
typedef int  (*pfdb_getNumFields)(FtrQuery query);

//�õ���ǰ��¼���ַ�������ַ�ֵ
typedef const char*  (*pfdb_getStringField)(FtrQuery query, int idx, const char* szNullValue);

//�õ���ǰ��¼�������������ֵ
typedef int (*pfdb_getIntField)(FtrQuery query, int idx, int nNullValue);

//�õ���ǰ��¼�ĸ�������ĸ���ֵ
typedef double (*pfdb_getFloatField)(FtrQuery query, int idx, double lfNullValue);

//�õ���ǰ��¼�Ŀ����ֵ���׵�ַ
typedef const unsigned char* (*pfdb_getBlobField)(FtrQuery query, int idx, int &nLen);

//��������һ����¼
typedef int (*pfdb_nextRow)(FtrQuery query);

//��������ķ��־û�
typedef void (*pfdb_endReadFtrFromTab)(FtrQuery query);

//�õ����ݿ���ĳ����ļ�¼������
typedef int (*pfdb_getRecordNum)(AccessObj obj, LPCTSTR tableName);

//��ȡobjTable���е���ftrId����չ����
typedef BOOL (*pfdb_readFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab);

//����objTable���е���ftrId����չ����
typedef BOOL (*pfdb_saveFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable, CValueTable &tab, int idx);

//ɾ��objTable���е���ftrId��Ӧ����չ����
typedef BOOL (*pfdb_deleteFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

//�ָ������Ӧ����չ����
typedef BOOL (*pfdb_restoreFtrXAttribute)(AccessObj obj, FtrId ftrId, LPCTSTR objTable);

//ɾ��objTable���е���ftrId
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

//��ȡ�洢��hResource����е���Դ�ַ���
CString EXPORT_EDITBASE StrFromModuleResID(UINT id, HMODULE hResource);

//FtrIdת��ΪCFeature *
EXPORT_EDITBASE inline CFeature* fdb_FtrFromFtrID(FtrId ftrId){ return (CFeature*)(ftrId); }

////CFeature *ת��ΪFtrId
inline FtrId  EXPORT_EDITBASE fdb_FtrIDFromFtr(CFeature * pFtr){return FtrId(pFtr);}

//CEditor *ת��ΪDocId
inline DocId  EXPORT_EDITBASE fdb_DocIDFromDoc(CEditor *pEdit){
	return DocId(pEdit);
}

//DocIdת��Ϊ CEditor *
EXPORT_EDITBASE inline CEditor* fdb_DocFromDocID(DocId docId){ return (CEditor*)docId; }

//����ͼ������Ϣ
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