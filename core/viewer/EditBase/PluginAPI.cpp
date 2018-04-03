#include <stdafx.h>
#include "PluginAPI.h"


MyNameSpaceBegin

API_SNAPITEM::API_SNAPITEM()
{
	pFtr =NULL;
	bInPreObj = 0;
	nSnapMode = 0;
}

BOOL API_SNAPITEM::IsValid()
{
	return ( pFtr || bInPreObj || (nSnapMode&ebAPI::Snap::modeGrid)!=0 );
}

// DocId   fdb_DocIDFromDoc(CEditor *pEdit)
// {
// 	return DocId(pEdit);
// }

//向视图发送消息
long fdb_sendMessageToView(ViewId viewId, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWnd *pWnd = (CWnd *)viewId;
	return pWnd->SendMessage(message,wParam,lParam);
}

CString  StrFromModuleResID(UINT id, HMODULE hResource)
{
	if (NULL==hResource)
	{
		return CString(_T(""));
	}
	TCHAR tmpstr[256];	
	::LoadString(hResource,id,tmpstr,sizeof(tmpstr)-1);
	CString str(tmpstr);
	return str;
}

pfdb_copyXAttributes EXPORT_EDITBASE fdb_copyXAttributes = NULL;

pfdb_getXAttributes EXPORT_EDITBASE fdb_getXAttributes = NULL;

pfdb_refreshViews EXPORT_EDITBASE fdb_refreshViews = NULL;

pfdb_newFtrPropertyObj EXPORT_EDITBASE fdb_newFtrPropertyObj = NULL;

pfdb_deleteFtrPropertyObj EXPORT_EDITBASE fdb_deleteFtrPropertyObj = NULL;

pfdb_refreshFtrPropertyPanel EXPORT_EDITBASE fdb_refreshFtrPropertyPanel = NULL;

pfdb_getCurView EXPORT_EDITBASE fdb_getCurView = NULL;

pfdb_startPluginCommand EXPORT_EDITBASE fdb_startPluginCommand = NULL;

pfdb_getCurDoc EXPORT_EDITBASE fdb_getCurDoc = NULL;

pfdb_getCurCommand EXPORT_EDITBASE fdb_getCurCommand = NULL;

pfdb_StartCmd EXPORT_EDITBASE fdb_StartCmd = NULL;

pfdb_getLayerOfFtr EXPORT_EDITBASE fdb_getLayerOfFtr = NULL;

pfdb_getColorOfLayer EXPORT_EDITBASE fdb_getColorOfLayer = NULL;

pfdb_getNameOfLayer EXPORT_EDITBASE fdb_getNameOfLayer = NULL; 

pfdb_getIdOfLayer EXPORT_EDITBASE fdb_getIdOfLayer = NULL;

pfdb_getCodeOfLayer EXPORT_EDITBASE fdb_getCodeOfLayer = NULL;

pfdb_addFtr EXPORT_EDITBASE  fdb_addFtr = NULL;

pfdb_addFtrWithProperties EXPORT_EDITBASE fdb_addFtrWithProperties = NULL;

pfdb_delFtr EXPORT_EDITBASE fdb_delFtr = NULL;

pfdb_getLayCntOfDoc EXPORT_EDITBASE fdb_getLayCntOfDoc = NULL;

pfdb_getLayerByIdx EXPORT_EDITBASE fdb_getLayerByIdx = NULL;

pfdb_getFtrCntOfLay EXPORT_EDITBASE fdb_getFtrCntOfLay = NULL;

pfdb_getFtrFromLay EXPORT_EDITBASE fdb_getFtrFromLay = NULL;

pfdb_updateStereoParam EXPORT_EDITBASE fdb_updateStereoParam = NULL;

pfdb_getScale EXPORT_EDITBASE  fdb_getScale = NULL;

pfdb_getWorkSpaceBound EXPORT_EDITBASE  fdb_getWorkSpaceBound = NULL;
// 
// pfdb_getModeOfLay EXPORT_EDITBASE  fdb_getModeOfLay = NULL;
// 
// pfdb_setModeOfLay EXPORT_EDITBASE  fdb_setModeOfLay = NULL;

pfdb_findUserIdx EXPORT_EDITBASE  fdb_findUserIdx = NULL;

pfdb_getSymbol EXPORT_EDITBASE fdb_getSymbol = NULL;

pfdb_progressBegin EXPORT_EDITBASE fdb_progressBegin = NULL;

pfdb_progressStep EXPORT_EDITBASE fdb_progressStep = NULL;

pfdb_progressEnd EXPORT_EDITBASE fdb_progressEnd = NULL;

pfdb_outputstring EXPORT_EDITBASE fdb_outputstring = NULL;

pfdb_excDML EXPORT_EDITBASE fdb_excDML = NULL;

pfdb_excQuery EXPORT_EDITBASE fdb_excQuery = NULL;

pfdb_modifyPropertyUI EXPORT_EDITBASE fdb_modifyPropertyUI = NULL;

pfdb_isAutoSetAnchor EXPORT_EDITBASE fdb_isAutoSetAnchor = NULL;

pfdb_setAnchorPoint EXPORT_EDITBASE fdb_setAnchorPoint = NULL;

pfdb_activeLayer EXPORT_EDITBASE fdb_activeLayer = NULL;

pfdb_setCurLayer EXPORT_EDITBASE fdb_setCurLayer = NULL;

pfdb_getCurLayer EXPORT_EDITBASE fdb_getCurLayer = NULL;

pfdb_createLayer EXPORT_EDITBASE fdb_createLayer = NULL;

pfdb_addLayer EXPORT_EDITBASE fdb_addLayer = NULL;

pfdb_setColorOfLayer EXPORT_EDITBASE fdb_setColorOfLayer = NULL;

pfdb_isLockedOfLayer EXPORT_EDITBASE fdb_isLockedOfLayer = NULL;

pfdb_enableLockedOfLayer EXPORT_EDITBASE fdb_enableLockedOfLayer = NULL;

pfdb_isVisibleOfLayer EXPORT_EDITBASE fdb_isVisibleOfLayer = NULL;

pfdb_enableVisibleOfLayer EXPORT_EDITBASE fdb_enableVisibleOfLayer = NULL;

pfdb_createDefaultFeature EXPORT_EDITBASE fdb_createDefaultFeature = NULL;

pfdb_getLocalLayer EXPORT_EDITBASE fdb_getLocalLayer = NULL;

pfdb_getLayer EXPORT_EDITBASE fdb_getLayer = NULL;

pfdb_annotToText EXPORT_EDITBASE fdb_annotToText = NULL;

pfdb_saveFtr EXPORT_EDITBASE fdb_saveFtr = NULL;

pfdb_beginAddFtr EXPORT_EDITBASE fdb_beginAddFtr = NULL;

pfdb_compileStatement EXPORT_EDITBASE fdb_compileStatement = NULL;

pfdb_bindField EXPORT_EDITBASE fdb_bindField = NULL;

pfdb_endAddFtr EXPORT_EDITBASE fdb_endAddFtr = NULL;

pfdb_beginReadFtrFromTab EXPORT_EDITBASE fdb_beginReadFtrFromTab = NULL;

pfdb_isEof EXPORT_EDITBASE fdb_isEof = NULL;

pfdb_getNumFields EXPORT_EDITBASE fdb_getNumFields = NULL;

pfdb_getStringField EXPORT_EDITBASE fdb_getStringField = NULL;

pfdb_getIntField EXPORT_EDITBASE fdb_getIntField = NULL;

pfdb_getFloatField EXPORT_EDITBASE fdb_getFloatField = NULL;

pfdb_getBlobField EXPORT_EDITBASE fdb_getBlobField = NULL;

pfdb_nextRow EXPORT_EDITBASE fdb_nextRow = NULL;

pfdb_endReadFtrFromTab EXPORT_EDITBASE fdb_endReadFtrFromTab = NULL;

pfdb_getRecordNum EXPORT_EDITBASE fdb_getRecordNum = NULL;

pfdb_readFtrXAttribute EXPORT_EDITBASE fdb_readFtrXAttribute = NULL;

pfdb_saveFtrXAttribute EXPORT_EDITBASE fdb_saveFtrXAttribute = NULL;

pfdb_deleteFtrXAttribute EXPORT_EDITBASE fdb_deleteFtrXAttribute = NULL;

pfdb_restoreFtrXAttribute EXPORT_EDITBASE fdb_restoreFtrXAttribute = NULL;

pfdb_deleteFtr EXPORT_EDITBASE fdb_deleteFtr = NULL;

pfdb_getSelection EXPORT_EDITBASE fdb_getSelection = NULL;

pfdb_updateSelection EXPORT_EDITBASE fdb_updateSelection = NULL;

pfdb_getSnap EXPORT_EDITBASE fdb_getSnap = NULL;

pfdb_getSnapResultNum EXPORT_EDITBASE fdb_getSnapResultNum = NULL;

pfdb_getSnapResult0 EXPORT_EDITBASE fdb_getSnapResult0 = NULL;

pprj_createPrj EXPORT_EDITBASE prj_createPrj = NULL;
pprj_createCoordCenter EXPORT_EDITBASE prj_createCoordCenter = NULL;
pprj_deletePrj EXPORT_EDITBASE prj_deletePrj = NULL;
pprj_deleteCoordCenter EXPORT_EDITBASE prj_deleteCoordCenter = NULL;
pprj_loadProject EXPORT_EDITBASE prj_loadProject = NULL;
pprj_getAllStereoIDs EXPORT_EDITBASE prj_getAllStereoIDs = NULL;
pprj_getStereoImages EXPORT_EDITBASE prj_getStereoImages = NULL;
pprj_initModel EXPORT_EDITBASE prj_initModel = NULL;
pprj_convertXYZ EXPORT_EDITBASE prj_convertXYZ = NULL;
pprj_IsGeoImage EXPORT_EDITBASE prj_IsGeoImage = NULL;
pprj_clear EXPORT_EDITBASE prj_clear = NULL;
pprj_initModel2 EXPORT_EDITBASE prj_initModel2 = NULL;
pprj_convertXYZ2 EXPORT_EDITBASE prj_convertXYZ2 = NULL;
pprj_getModelBound EXPORT_EDITBASE prj_getModelBound = NULL;

pdsm_create EXPORT_EDITBASE dsm_create = NULL;
pdsm_open EXPORT_EDITBASE dsm_open = NULL;
pdsm_getZ EXPORT_EDITBASE dsm_getZ = NULL;
pdsm_delete EXPORT_EDITBASE dsm_delete = NULL;
pdsm_getInfo EXPORT_EDITBASE dsm_getInfo = NULL;

MyNameSpaceEnd