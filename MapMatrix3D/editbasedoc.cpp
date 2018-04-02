// EditBaseDoc.cpp : implementation of the CDlgDoc class
//

#include "stdafx.h"
#include <float.h>
#include "EditBase.h"
#include "EditBaseDoc.h"
#include "DlgDataSource.h"
#include "XmlAccess.h"
#include "SymbolLib.h"
#include "MainFrm.h"
#include "LayersMgrDlg.h"
#include "FtrsMgrDlg.h"
#include "Feature.h"
#include "SQLiteAccess.h"
#include "Association.h"
#include "DlgCellDefLinetypeView.h"
#include "DlgScheme.h"
#include "DlgDataTransform.h"
#include "ExMessage.h"
#include "DxfAccess.h"
#include "DlgImportCtrlPts.h"
#include "UIFFileDialogEx.h"
#include "GeoText.h"
#include "GeoParallel.h "
#include "RegDef.h"
#include "RegDef2.h"
#include "VectorView_EX.h "
#include "VectorView_new.h"
#include "ChildFrm.h "
#include "DlgOverlayGrid.h "
#include "ExMessage.h "
#include "DlgWorkSpaceBound.h "
#include  <io.h>
#include "DSM.h"
#include "DlgConvertPhoto.h"
#include "DlgCreateContours.h"
#include "DlgCreateContours1.h"

#include "DlgConditionSelectConfig.h"
#include "ConditionSelect.h"
#include "DlgRefDataFile.h "
#include "StereoView.h"
#include "string"
using std::string;
#include "VVTAccess.h "
#include "DlgImportVVT.h "
#include "DlgBuildDEM.h "
#include "DlgExportShp.h"
#include "..\export\shapefil.h"
#include "DlgImportShp.h"

#include "..\export\MapStarText.h"
#include "DlgExportMapStarText.h"

#include "DlgTestContourSettings.h"
#include "DlgExportModel.h"
#include "dxfconvert.h"
#include "textfile.h"
#include "Functions_temp.h "
#include "DlgOverlayBound.h "
#include "License.h"

#include "DlgCommand.h"

#include "DlgExtProperties.h"

#include "UndoAction.h"

#include "DlgImportFdb.h"
#include "UndoBar.h"
#include "StereoFrame.h"
#include "DlgImportEps.h "
#include "EpsAccess.h "
#include "DlgExportEps.h "
#include ".\E00\e00IO.h"
#include "PluginAccess.h "

#include "DlgLayerCodeCheck.h"
#include "DlgImportJB.h"
#include "DlgExportJB.h"
#include "Export\MilitaryVectorFile.h"
#include "DlgRefDataBound.h "
#include "DlgExportFdb.h "
#include "DlgCellOverlayGrid.h "
#include "CheckScheme.h "
#include "DlgFilterSelect.h"

#include "UIFBoolProp.h"
#include "..\CORE\viewer\EditBase\res\resource.h"
#include "DlgMapInfo.h"

#include "DlgExportE00.h"
#include "DlgImportE00.h"

#include "DlgImportMDB.h"
#include "MATH.h"
#include "DlgCheckSuspendPoints.h"
#include "DlgTip.h"

#include "DlgWheelSpeed.h"
#include "GlobalFunc.h"

#include "VectorView_EP.h"
#include "OptionsDlg.h"
#include "dlgsetupgroup.h"
#include "DlgDeleteSame.h"
#include "DlgSelectShift.h"
#include "UVSModify.h"

#include "ArcgisMdbCodeTab.h"
#include "DlgExportArcgisMdb.h"

#include "Global/GeoBuilderContour.h"
#include "Global/GridContourX.h"

#include "mm3dPrj/OsgbView.h"
#include "mm3dPrj/MsgSender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void VariantToText(const CVariantEx& v, CString &text);
extern void AlignToString(long& nAlign, CString& str, BOOL bOrder);
extern void InclinedTypeToString(long& type, CString& str, BOOL bOrder);
extern void CoverTypeToString(long& value, CString& str, BOOL bOrder);
extern void PlaceTypeToString(long& type, CString& str, BOOL bOrder);
extern BOOL convertStringToStrArray(LPCTSTR str, CStringArray &arr);
extern CString VarToString(_variant_t& var);

#define RPO(x,y,z)  CPermanent::RegisterPermanentObject(x,y,z)

extern BOOL g_isFileOnUVS;

int GetAccessType(CDataSourceEx *pDS)
{
	if (!pDS) return -1;
	CAccessModify *pAccess = pDS->GetAccessObject();
	if (!pAccess) return -1;
	return pAccess->GetAccessType();
}


CString FormatFileNameByExt(const char *oldName, const char *ext)
{
	if( oldName==NULL || strlen(oldName)<=0 )
		return CString(_T(""));
	
	if( ext==NULL || strlen(ext)<=0 )
		return CString(oldName);
	
	CString name = oldName;
	int pos = name.ReverseFind(_T('.'));
	if( pos<0 )
		name = name + _T(".") + ext;
	else
	{
		CString old_ext = name.Mid(pos+1);
		old_ext.Remove(_T(' '));
		
		if( old_ext.GetLength()<=0 )
			name = name + ext;
		else if( old_ext.CompareNoCase(ext)!=0 )
		{
			name = name + _T(".") + ext;
		}
	}
	return name;
}

void SetLanguage( HMODULE hModule );

extern DWORD_PTR gdwInnerCmdData[2];

CView *GetActiveView()
{
	if (!gpMainWnd) return NULL;
	CFrameWnd *pChild = gpMainWnd->MDIGetActive();
	if( !pChild )return NULL;
	
	CView *pView = pChild->GetActiveView();
	return pView;
}

CDlgDoc *GetActiveDlgDoc()
{
	CView *pView = GetActiveView();
	if( !pView )return NULL;

	CDocument *pDoc = pView->GetDocument();
	if( !pDoc || !pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
		return NULL;

	return (CDlgDoc*)pDoc;
}

IMPLEMENT_DYNAMIC(CSelChangedExchanger, CPermanentExchanger)

BOOL CSelChangedExchanger::GetProperties(CValueTable& tab)
{	
	if (m_arrPFtr.GetSize()>0)
	{
		for (int i=0;i<m_arrPFtr.GetSize();i++)
		{
			CPermanent *pFtr = m_arrPFtr[i];
			tab.BeginAddValueItem();
			pFtr->WriteTo(tab);	
			tab.EndAddValueItem();

			//属性窗口只能显示一个地物的属性，为了加快速度，只显示第一个地物的属性
			break;
		}	
		return TRUE;
	}		
	return FALSE;
}

void CSelChangedExchanger::OnModifyProperties(CValueTable &tab )
{	
	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	CUndoModifyProperties undo(m_pDlgDoc,"ModifyProperties");
	for (int i=0;i<m_arrPFtr.GetSize();i++)
	{
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));

		m_pDlgDoc->DeleteObject(FtrToHandle(CPFeature(m_arrPFtr[i])),FALSE);
	}

	if (m_arrPFtr.GetSize()>0)
	{
		undo.oldVT.CopyFrom(tab);
		for (int i=0;i<m_arrPFtr.GetSize();i++)
		{
			CPermanent *pFtr = m_arrPFtr[i];

			CValueTable tab2;
			tab2.BeginAddValueItem();
			pFtr->WriteTo(tab2);	
			tab2.EndAddValueItem();

			undo.oldVT.SetItemFromTab(i,tab2,0);
		}
	}

	undo.newVT.CopyFrom(tab);
	undo.Commit();

	//属性的修改应用于所有的地物
	CPermanentExchanger::OnModifyProperties(tab);

	for (i=0;i<m_arrPFtr.GetSize();i++)
	{
		m_pDlgDoc->RestoreObject(FtrToHandle(CPFeature(m_arrPFtr[i])));	
		//m_pDlgDoc->UpdateObject(FtrToHandle(CPFeature(m_arrPFtr[i])),FALSE);
	}	
	m_pDlgDoc->UpdateAllViews(NULL,hc_SelChanged,(CObject*)TRUE);
	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}


BOOL CSelChangedExchanger::GetXAttributes(CValueTable& tab, BOOL bAll)
{
	if (m_arrPFtr.GetSize() <= 0)
		return FALSE;

	CAttributesSource  *pDXS = m_pDlgDoc->GetDlgDataSource()->GetXAttributesSource();
	if(!pDXS) return FALSE;

	if (bAll)
	{
		for (int i = 0; i < m_arrPFtr.GetSize(); i++)
		{
			CPermanent *pFtr = m_arrPFtr[i];
			tab.BeginAddValueItem();
			pDXS->GetXAttributes((CFeature*)pFtr, tab);
			tab.EndAddValueItem();
		}
	}
	else
	{
		for (int i = 0; i < m_arrPFtr.GetSize(); i++)
		{
			CPermanent *pFtr = m_arrPFtr[i];
			tab.BeginAddValueItem();
			pDXS->GetXAttributes((CFeature*)pFtr, tab);
			tab.EndAddValueItem();

			//属性窗口只能显示一个地物的属性，为了加快速度，只显示第一个地物的属性
			break;
		}
		CFtrLayer *pLayer = m_pDlgDoc->GetDlgDataSource()->GetFtrLayerOfObject((CFeature*)m_arrPFtr[0]);
		if (!pLayer) return FALSE;
		CConfigLibManager *pConfig = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		if (pConfig)
		{
			CScheme *pScheme = pConfig->GetScheme(m_pDlgDoc->GetDlgDataSource()->GetScale());
			if (pScheme)
				pScheme->GetXAttributesAlias(pLayer->GetName(), tab);//获取别名
		}
	}
	
	return TRUE;	
}


void CSelChangedExchanger::OnModifyXAttributes(CValueTable& tab)
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CAttributesSource  *pDXS = pDS->GetXAttributesSource();
	if(!pDXS) return ;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	CUndoModifyProperties undo(m_pDlgDoc,"ModifyProperties");
	GetXAttributes(undo.XoldVT,TRUE);
	for (int i = 0; i < m_arrPFtr.GetSize(); i++)
	{
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));

		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]), FALSE);
	}

	undo.XnewVT.CopyFrom(tab);
	undo.Commit();

	for (i = 0; i < m_arrPFtr.GetSize(); i++)
	{
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(CPFeature(m_arrPFtr[i]));

		//选中地物有可能来自参考文件，这是pFtrLayer为NULL；
		if (pFtrLayer == NULL)
			continue;

		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
		pDXS->SetXAttributes((CFeature*)m_arrPFtr[i], tab, i);

		//m_pDlgDoc->UpdateObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
	}

	m_pDlgDoc->UpdateAllViews(NULL,hc_SelChanged,(CObject*)TRUE);
	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}


CPermanentExchanger * CSelChangedExchanger::Clone()
{

	return new CSelChangedExchanger(m_arrPFtr,m_pDlgDoc);
}


void CSelChangedExchanger::OnModifyLayer(LPCSTR str)
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CFtrLayer *pLayer = NULL,*pLayer0 = pDS->GetFtrLayer(str);

	if (!pLayer0) return;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	
	float wid = pScheme->GetLayerDefineLineWidth(pLayer0->GetName());

	CUndoBatchAction batchUndo(m_pDlgDoc,"ModifyLayerOfObj");
	
	for (int i=0;i<m_arrPFtr.GetSize();i++)
	{
		CUndoModifyLayerOfObj undo(m_pDlgDoc,"ModifyLayerOfObj");

		CPFeature pftr = (CPFeature)(m_arrPFtr[i]);
		undo.arrHandles.Add(FtrToHandle(pftr));
		
		undo.oldVT.BeginAddValueItem();
		pftr->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();

		m_pDlgDoc->DeleteObject(FtrToHandle(pftr),FALSE);

		CGeometry *pGeo = pftr->GetGeometry();

		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			((CGeoCurveBase*)pGeo)->m_fLineWidth = wid;
		}

		pLayer = pDS->GetFtrLayerOfObject(pftr);
		if (!pLayer)  continue;
		undo.oldLayerArr.Add(pLayer->GetID());
		pDS->SetFtrLayerOfObject(pftr,pLayer0->GetID());	
		undo.newLayerArr.Add(pLayer0->GetID()); 

		//改变层之后恢复缺省参数
		CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(pLayer0->GetName());
		if(pSchemeLayer)
		{
			CValueTable tab;
			pSchemeLayer->GetDefaultValues(tab);
			tab.DelField(FIELDNAME_FTRDISPLAYORDER);
			tab.DelField(FIELDNAME_GEOTEXT_CONTENT);
			tab.DelField(FIELDNAME_FTRMODIFYTIME);
			tab.DelField(FIELDNAME_GEOCLASS);
			tab.DelField(FIELDNAME_CLSTYPE);
			tab.DelField(FIELDNAME_GEOPOINT_ANGLE);
			tab.DelField(FIELDNAME_GEOCURVE_WIDTH);
			tab.DelField(FIELDNAME_GEOCURVE_DHEIGHT);
			tab.DelField(FIELDNAME_FTRPURPOSE);
			pftr->ReadFrom(tab);
		}

		pftr->GetGeometry()->SetSymbolName("");
		
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
		m_pDlgDoc->RestoreObject(FtrToHandle(pftr));

		batchUndo.AddAction(&undo);
	}
	
	batchUndo.Commit();

	m_pDlgDoc->RefreshView();
	m_pDlgDoc->UpdateView(0,hc_SelChanged,TRUE);
	
}

void CSelChangedExchanger::OnModifyLayerGroup(LPCSTR str)
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	FtrLayerGroup *pLayerGroup = pDS->GetFtrLayerGroupByName(str);
	
	if (!pLayerGroup) return;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	CUndoModifyLayer undo(m_pDlgDoc,"ModifyLayer");
	for (int i=0;i<m_arrPFtr.GetSize();i++)
	{
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(CPFeature(m_arrPFtr[i]));

		//选中地物有可能来自参考文件，这是pFtrLayer为NULL；
		if( pFtrLayer==NULL )
			continue;

		if (stricmp(pFtrLayer->GetGroupName(),str) == 0)
			continue;

		int size = undo.arrLayers.GetSize();
		for (int j=0; j<size; j++)
		{
			if (undo.arrLayers[j] == pFtrLayer)
			{
				break;
			}
		}

		if (j >= size)
		{
			pLayerGroup->AddFtrLayer(pFtrLayer);
			FtrLayerGroup *pFtrLayerGroup0 = pDS->GetFtrLayerGroupByName(pFtrLayer->GetGroupName());
			if (pFtrLayerGroup0)
			{
				pFtrLayerGroup0->DelFtrLayer(pFtrLayer);
			}
			undo.arrLayers.Add(pFtrLayer);
		}		
	}
	
	for (i=0;i<undo.arrLayers.GetSize();i++)
	{
		undo.arrOldVars.Add((_variant_t)(const char*)undo.arrLayers[i]->GetGroupName());
		m_pDlgDoc->ModifyLayer(undo.arrLayers[i],FIELDNAME_LAYGROUPNAME,(_variant_t)(const char*)str,TRUE);
	}

	undo.newVar = (_variant_t)(const char*)str;
	
	undo.Commit();
	
	m_pDlgDoc->RefreshView();
	m_pDlgDoc->UpdateView(0,hc_SelChanged,TRUE);
	
}

void CSelChangedExchanger::OnModifyLinetype(int code)
{	
	CUndoModifyProperties undo(m_pDlgDoc,StrFromResID(IDS_CMDNAME_CHGCODE));
	PT_3DEX pt0,pt1;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	for(int i = m_arrPFtr.GetSize()-1; i>=0; i--)
	{
		CGeometry *pObj = CPFeature(m_arrPFtr[i])->GetGeometry();
		
		//生成新的对象，这里需要指定新对象层码
		if( !pObj || pObj->GetDataPointSum()<=0 )continue;

		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
		
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));
		undo.oldVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		CArray<PT_3DEX,PT_3DEX> pts;
		pObj->GetShape(pts);
		int num0 = pts.GetSize();
		for (int j=0;j<num0;j++)
		{
			pt0 = pts.GetAt(j);
			pt0.pencode = code;
			pts.SetAt(j,pt0);
		}
		if ( !pObj->CreateShape(pts.GetData(),pts.GetSize()) ) continue;

		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
// 		if( !m_pDlgDoc->UpdateObject(FtrToHandle((CFeature*)m_arrPFtr[i])) )
// 		{			
// 			continue;
// 		}
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();

	}
	undo.Commit();

	m_pDlgDoc->RefreshView();
}

void CSelChangedExchanger::OnModifyNodeWid(int index, float wid)
{
	CUndoModifyProperties undo(m_pDlgDoc,StrFromResID(IDS_CMDNAME_CHGNODEWID));
	PT_3DEX pt0,pt1;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	for(int i = m_arrPFtr.GetSize()-1; i>=0; i--)
	{
		CGeometry *pObj = CPFeature(m_arrPFtr[i])->GetGeometry();
		
		if( !pObj)continue;

		CArray<PT_3DEX,PT_3DEX> pts;
		pObj->GetShape(pts);

		int num0 = pts.GetSize();
		if (index < 0 || index >= num0) continue;
		
		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
		
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));
		undo.oldVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		pts[index].wid = wid;
		
		if ( !pObj->CreateShape(pts.GetData(),pts.GetSize()) ) continue;
		
		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
	
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
	}
	undo.Commit();
	
	m_pDlgDoc->RefreshView();
}

void CSelChangedExchanger::OnModifyFtrWid(float wid)
{
	CUndoModifyProperties undo(m_pDlgDoc,StrFromResID(IDS_CMDNAME_CHGFTRWID));
	PT_3DEX pt0,pt1;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	for(int i = m_arrPFtr.GetSize()-1; i>=0; i--)
	{
		CGeometry *pObj = CPFeature(m_arrPFtr[i])->GetGeometry();
		
		if( !pObj || (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))))continue;
		
		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
		
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));
		undo.oldVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			((CGeoCurve*)pObj)->m_fLineWidth = wid;
		}
		else
		{
			((CGeoSurface*)pObj)->m_fLineWidth = wid;
		}
		
		
		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
		
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
	}
	undo.Commit();
	
	m_pDlgDoc->RefreshView();
}

void CSelChangedExchanger::OnModifyFtrColor(COLORREF col)
{
	CUndoModifyProperties undo(m_pDlgDoc,"ModifyProperties");
	PT_3DEX pt0,pt1;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	for(int i = m_arrPFtr.GetSize()-1; i>=0; i--)
	{
		CGeometry *pObj = CPFeature(m_arrPFtr[i])->GetGeometry();
		
		if( !pObj )continue;
		
		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
		
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));
		undo.oldVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		pObj->SetColor(col);
		
		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
		
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
	}
	undo.Commit();
	
	m_pDlgDoc->RefreshView();
}

void CSelChangedExchanger::OnModifyTransparency(long transparency)
{
	CUndoModifyProperties undo(m_pDlgDoc,"ModifyTransparency");
	
	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	for(int i = m_arrPFtr.GetSize()-1; i>=0; i--)
	{
		CFeature* pFtr = CPFeature(m_arrPFtr[i]);
		CGeoCurveBase* pObj = (CGeoCurveBase*)pFtr->GetGeometry();
		if( !pObj )continue;
		
		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
		
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));
		undo.oldVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		pObj->SetTransparency(transparency);
		
		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
		
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
	}
	undo.Commit();
	
	m_pDlgDoc->RefreshView();
}

void CSelChangedExchanger::OnModifyFillColor(BOOL bFill, COLORREF col)
{
	CUndoModifyProperties undo(m_pDlgDoc,"ModifyFillColor");
	
	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;

	int transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", 50);
	
	for(int i = m_arrPFtr.GetSize()-1; i>=0; i--)
	{
		CFeature* pFtr = CPFeature(m_arrPFtr[i]);
		CGeoCurveBase* pObj = (CGeoCurveBase*)pFtr->GetGeometry();
		if( !pObj )continue;

		BOOL bFillColor_old = pObj->IsFillColor();
		
		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
		
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));
		undo.oldVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		pObj->EnableFillColor(bFill, col);
		if(!bFillColor_old)
		{
			pObj->SetTransparency(transparency);
		}
		
		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
		
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
	}
	undo.Commit();
	
	m_pDlgDoc->RefreshView();
}

void CSelChangedExchanger::OnModifyNodeType(int index, short type)
{
	CUndoModifyProperties undo(m_pDlgDoc,StrFromResID(IDS_CMDNAME_CHGNODETYPE));
	PT_3DEX pt0,pt1;

	//非激活数据源，不允许修改
	if( !IsActiveDataSource() )
		return;
	
	for( int i = m_arrPFtr.GetSize()-1; i>=0; i--)
	{
		CGeometry *pObj = CPFeature(m_arrPFtr[i])->GetGeometry();
		
		if( !pObj)continue;
		
		CArray<PT_3DEX,PT_3DEX> pts;
		pObj->GetShape(pts);
		
		int num0 = pts.GetSize();
		if (index < 0 || index >= num0) continue;
		
		m_pDlgDoc->DeleteObject(FtrToHandle((CFeature*)m_arrPFtr[i]),FALSE);
		
		undo.arrHandles.Add(FtrToHandle(CPFeature(m_arrPFtr[i])));
		undo.oldVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		pts[index].type = type;
		
		if ( !pObj->CreateShape(pts.GetData(),pts.GetSize()) ) continue;
		
		m_pDlgDoc->RestoreObject(FtrToHandle((CFeature*)m_arrPFtr[i]));
		
		undo.newVT.BeginAddValueItem();
		CPFeature(m_arrPFtr[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
	}
	undo.Commit();
	
	m_pDlgDoc->RefreshView();
}


//是否为当前数据源
BOOL CSelChangedExchanger::IsActiveDataSource()
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();	

	for (int i=0;i<m_arrPFtr.GetSize();i++)
	{
		if( pDS->GetFtrLayerOfObject((CPFeature)m_arrPFtr[i])==NULL )
			return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDlgDoc

IMPLEMENT_DYNCREATE(CDlgDoc, CDocument)

BEGIN_MESSAGE_MAP(CDlgDoc, CDocument)
	//{{AFX_MSG_MAP(CDlgDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_COMMAND(ID_FILE_PROPERTY,OnFileProperty)
	ON_COMMAND(ID_EDIT_REDO, OnRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_FILE_OPENDXF, OnImportDxf)
	ON_COMMAND(ID_FILE_OPENDGN, OnImportDgn)
	ON_COMMAND(ID_FILE_SAVE_ASDGN, OnExportDgn)
#ifndef TRIAL_VERSION
	ON_COMMAND(ID_FILE_SAVE_ASXML, OnFileSaveAsxml)
#endif
	ON_COMMAND(ID_FILE_OPENXML, OnFileOpenxml)
	ON_COMMAND(ID_FILE_INPUT_CTRLPTS, OnImportCtrlpts)
	ON_COMMAND(ID_FILE_OUTPUT_CTRLPTS, OnExportCtrlpts)
	ON_COMMAND(ID_FILE_REFERRENCES, OnFileReferrences)
	ON_COMMAND(ID_TOOL_LAYERMANAGE, OnToolLayermanage)
	ON_COMMAND(ID_TOOL_SHOWSELECTLAYER, OnToolShowSelectLayer)
	ON_UPDATE_COMMAND_UI(ID_TOOL_SHOWSELECTLAYER, OnUpdateShowSelectLayer)
	ON_COMMAND(ID_TOOL_HIDESELECTLAYER, OnToolhideSelectLayer)
	ON_UPDATE_COMMAND_UI(ID_TOOL_HIDESELECTLAYER, OnUpdateHideSelectLayer)
	ON_COMMAND(ID_TOOLS_SCRIPTS,OnToolScripts)
	ON_COMMAND(ID_TOOL_FTRSMANAGE, OnToolFtrsmanage)
	ON_COMMAND(ID_TOOL_FTRSORGANIZE, OnToolFtrsorganize)
	ON_COMMAND(ID_TOOL_SCHEME, OnToolSchememanage)
	ON_COMMAND(ID_CELLDEF_VIEW,OnCellDefView)
	ON_COMMAND(ID_FILE_OPENDB, OnFileOpendb)
	ON_COMMAND(ID_FILE_OPENFDB, OnFileOpenfdb)
	ON_COMMAND(ID_FILE_OPENFDB_DB, OnFileOpenfdbDB)
#ifndef TRIAL_VERSION
	ON_COMMAND(ID_FILE_SAVE_ASDB, OnFileSaveAsdb)
#endif
	ON_COMMAND(ID_FILE_SAVE_ASDXF, OnExportdxf)
	ON_COMMAND(ID_WS_DATABOUND, OnWsDatabound)
//	ON_COMMAND(ID_TOOL_DATATRANSFORM, OnDataTransform)
	ON_COMMAND(ID_OPTIONUPDATE,OnUpdateOption)
    ON_COMMAND(ID_VIEW_XZ, OnViewXz)
	ON_COMMAND(ID_VIEW_ROAD, OnViewRoad)
	ON_COMMAND(ID_VIEW_SECTION,OnViewSection)
	ON_COMMAND(ID_OVERLAY_GRID, OnOverlayGrid)
	ON_COMMAND(ID_SHOWLAYERDIR, OnShowLayerDir)
	ON_UPDATE_COMMAND_UI(ID_SHOWLAYERDIR, OnUpdateShowLayerDir)
	ON_COMMAND(ID_WS_SETBOUND, OnWsSetbound)
	ON_COMMAND(ID_TOOL_RECOVERZ,OnRecoverZ)
	ON_COMMAND(ID_ANCHORPT_AUTOSET, OnAnchorptAutoset)
	ON_UPDATE_COMMAND_UI(ID_ANCHORPT_AUTOSET, OnUpdateAnchorptAutoset)
	ON_COMMAND(ID_WORK_INPUT_VVT, OnWorkInputVvt)
	ON_COMMAND(ID_TOOL_PHOTOTOGROUND,OnPhotoToGround)
	ON_COMMAND(ID_BUILDTIN,OnBuildTIN)
	ON_COMMAND(ID_DESTROYTIN,OnDestroyTIN)
	ON_COMMAND(ID_EXPORTTIN,OnExportTIN)
	ON_COMMAND(ID_FILE_SAVE_ASCAS, OnExportCas)
	ON_COMMAND(ID_CREATECONTOUR,OnCreateContours)
	ON_COMMAND(ID_CREATECONTOUR_FROMDEM,OnCreateContoursFromDEM)
	ON_COMMAND(ID_CONDITIONSELECT,OnConditionSelect)
	ON_COMMAND(ID_CONFIG_QUERY,OnConditionSelectConfig)
	ON_COMMAND_RANGE(ID_CONFIG_QUERY_BEGIN,ID_CONFIG_QUERY_END,OnQueryMenu)
	ON_COMMAND(ID_SAVETIMER, OnSaveTimer)
	ON_COMMAND(ID_LAYER_DEFAULT_COMMAND, OnSetDefaultCommand)
	ON_UPDATE_COMMAND_UI(ID_LAYER_DEFAULT_COMMAND, OnUpdateSetDefaultCommand)
	ON_CBN_SELENDOK(ID_DUMMY_VIEW_FTRCLASS, OnDummySelectFtrClass)
	ON_UPDATE_COMMAND_UI(ID_DUMMY_VIEW_FTRCLASS, OnUpdateDummySelectFtrClass)
	ON_CBN_SELENDOK(ID_DUMMY_VIEW_LAYER, OnDummySelectLayer)
	ON_UPDATE_COMMAND_UI(ID_DUMMY_VIEW_LAYER, OnUpdateDummySelectLayer)
	ON_CBN_SELENDOK(ID_DUMMY_VIEW_LAYERGROUP, OnDummySelectLayerGroup)
	ON_UPDATE_COMMAND_UI(ID_DUMMY_VIEW_LAYERGROUP, OnUpdateDummySelectLayerGroup)
	ON_CBN_SELENDOK(ID_DUMMY_VIEW_FTRWIDTH, OnDummySelectFtrWidth)
	ON_UPDATE_COMMAND_UI(ID_DUMMY_VIEW_FTRWIDTH, OnUpdateDummySelectFtrWidth)
	ON_CBN_SELENDOK(ID_DUMMY_VIEW_FTRSYM, OnDummySelectFtrSym)
	ON_UPDATE_COMMAND_UI(ID_DUMMY_VIEW_FTRSYM, OnUpdateDummySelectFtrSym)
	ON_COMMAND(ID_DUMMY_VIEW_FTRCOLOR,OnDummyModifyFtrColor)
	ON_COMMAND(ID_WORK_OUTPUT_SHP,OnExportShp)
	ON_COMMAND(ID_WORK_INPUT_SHP,OnImportShp)
	ON_COMMAND(ID_WORK_OUTPUT_MODEL,OnExportModel)
	ON_COMMAND(ID_WORK_OUTPUT_MAPSTARTEXT,OnExportMapStarText)
#ifndef TRIAL_VERSION
	ON_COMMAND(ID_WORK_OUTPUT_VVT,OnExportVVT)
#endif
	ON_COMMAND(ID_TESTCREATE_SETTINGS,OnTestCreateContourSettings)
	ON_COMMAND(ID_CREATEDEM, OnBuildDEM)
	ON_COMMAND(ID_OVERLAYDEM, OnOverlayDEM)
//	ON_COMMAND(ID_SAVEDEM, OnSaveDEM)
//	ON_UPDATE_COMMAND_UI(ID_SAVEDEM, OnUpdateSaveDEM)
//	ON_COMMAND(ID_REMOVEDEM, OnRemoveDEM)
	ON_COMMAND(ID_OVERLAY_BOUND, OnOverlayBound)
	ON_COMMAND(ID_REFERENCE_DATAFILE, OnReferenceDatafile)
	ON_COMMAND(ID_RESET_DATAFILE_BOUND,OnResetDatafileBound)
//	ON_COMMAND(ID_REFERENCE_DATAFILE_UPDATE,OnReferenceDataFileUpdate)
	ON_UPDATE_COMMAND_UI(ID_REFERENCE_DATAFILE, OnUpdateReferenceDatafile)
	ON_COMMAND(ID_ANCHORPT_RECORD, OnAnchorptRecord)
	ON_COMMAND(ID_ANCHORPT_JUMP, OnAnchorptJump)
	ON_COMMAND(ID_REFRESH_DRAWING, OnRefreshDrawing)
	ON_COMMAND(ID_STATE_ACCUBOX, OnToggleAccuBox)
	ON_UPDATE_COMMAND_UI(ID_STATE_ACCUBOX, OnUpdateToggleAccuBox)
	ON_COMMAND(ID_STATE_RIGHTANGMOD, OnToggleRightAng)
	ON_UPDATE_COMMAND_UI(ID_STATE_RIGHTANGMOD, OnUpdateToggleRightAng)
	ON_COMMAND(ID_STATE_AUTOCLOSED, OnDrawAutoClosed)
	ON_UPDATE_COMMAND_UI(ID_STATE_AUTOCLOSED, OnUpdateDrawAutoClosed)
	ON_COMMAND(ID_STATE_AUTORECTIFY, OnDrawAutoRectify)
	ON_UPDATE_COMMAND_UI(ID_STATE_AUTORECTIFY, OnUpdateOnDrawAutoRectify)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_LASTCOMMAND, OnExecLastCommand)
	ON_COMMAND(ID_ACCUBOX_LOCKX, OnAccuLockX)
	ON_COMMAND(ID_ACCUBOX_LOCKY, OnAccuLockY)
	ON_COMMAND(ID_ACCUBOX_LOCKZ, OnAccuLockZ)
	ON_COMMAND(ID_ACCUBOX_LOCKANGLE, OnAccuLockAngle)
	ON_COMMAND(ID_ACCUBOX_LOCKDISTANCE, OnAccuLockDistance)
	ON_COMMAND(ID_ACCUBOX_LOCKINDEX, OnAccuLockIndex)
	ON_COMMAND(ID_ACCUBOX_COORDSYS_BASE, OnAccuCoordSysBase)
	ON_COMMAND(ID_ACCUBOX_LOCKEXTENSION, OnAccuLockExtension)
	ON_COMMAND(ID_ACCUBOX_LOCKPARALLEL, OnAccuLockParallel)
	ON_COMMAND(ID_ACCUBOX_LOCKPERP, OnAccuLockPerp)
	ON_COMMAND(ID_ACCUBOX_UNLOCK_CURDIR, OnAccuUnlockCurDir)
	ON_COMMAND(ID_ACCUBOX_UNLOCK_ALLDIR, OnAccuUnlockAllDir)
	ON_COMMAND(ID_DEVICE_ONLYSTEREO,OnDeviceOnlyStereo)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_ONLYSTEREO,OnUpdateDeviceOnlyStereo)
	ON_COMMAND(ID_STEREO_3DMOUSE_DRIVECROSS,OnDeviceOnlyCursor)
	ON_UPDATE_COMMAND_UI(ID_STEREO_3DMOUSE_DRIVECROSS,OnUpdateDeviceOnlyCursor)
	ON_COMMAND(ID_DOCINFORELOAD,OnDocInfoReload)
	ON_COMMAND(ID_VIEW_SINGLEPHOTOSTEREO, OnSinglePhotoStereo)
	ON_COMMAND(ID_SHOWHIDE_SUSPNT, OnCheckSuspendPoints)
	ON_COMMAND(ID_SHOWHIDE_PSEUDONODE, OnCheckPseudoNode)
	ON_COMMAND(ID_MODIFY_DELSAME, OnDelAllSameObjs)
	ON_COMMAND(ID_NON_LOCAL_LAYERS_RENAME, OnNonLocalLayersRename)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_INDICATOR_SNAP,OnShowSnapStatus)
	ON_COMMAND_RANGE(ID_POPUP_SNAPBEGIN,ID_POPUP_SNAPEND,OnSnapMode)
	ON_COMMAND(FCCMD_LOADSTEREO, OnLoadStereo)
	ON_COMMAND(ID_RECENTCLASSUPDATE,OnUpdateRecentClass)
	ON_CBN_SELENDOK(ID_DUMMY_VIEW_RECENTCMDS, OnDummySelectRecentCmd)
	ON_UPDATE_COMMAND_UI(ID_DUMMY_VIEW_RECENTCMDS, OnUpdateDummySelectRecentCmd)
	ON_COMMAND(ID_FILE_OPENEPS, OnFileOpeneps)
	ON_COMMAND(ID_WORK_OUTPUT_EPS,OnExporteps)
	ON_COMMAND(ID_FILE_OPENE00, OnFileOpenE00)
	ON_COMMAND(ID_WORK_OUTPUT_E00, OnExportE00)
	ON_COMMAND(ID_SELECT_NEXT,OnSelectNext)
	ON_UPDATE_COMMAND_UI(ID_SELECT_NEXT,OnUpdateSelectNext)
	ON_COMMAND(ID_TOOL_LAYERSCHECK,OnLayersCheck)
	ON_COMMAND(ID_PRINT_MUTIIMGVIEW, OnMutiimgview)
	ON_COMMAND(ID_FILE_IMPORTJB,OnImportJB)
	ON_COMMAND(ID_FILE_EXPORTJB,OnExportJB)
	ON_COMMAND(ID_FILE_EXPORTASC,OnExportASC)
	ON_COMMAND(ID_SAVE_DEMPOINTSTODEM, OnSaveDEMPointstoDEM)
	ON_COMMAND(ID_REMOVEDEM, OnRemoveDem)
	ON_COMMAND(ID_DEL_SAMEPT_INLINE,OnDelOverlapPtInLines)
	ON_COMMAND(ID_DEL_PSEUDOPT, OnDelAllPseudoPoints)
	ON_COMMAND(ID_DEL_PSEUDOPT_DIR, OnDelDirPseudoPoints)
	ON_COMMAND(ID_DEL_PSEUDOPT_ONE, OnDelOnePseudoPoint)
	ON_COMMAND(ID_REMOVE_ALLSUSPEND, OnRemoveAllSuspend)
	ON_COMMAND(ID_REMOVE_ONESUSPEND, OnRemoveOneSuspend)
	ON_COMMAND(ID_STATISTICS, OnStatistics)
	ON_COMMAND(ID_STATISTICSCLOSED, OnStatisticsClosed)
	//
	ON_COMMAND(ID_REPAIR_ONE_PTZTEXT, OnRepairOnePtZText)
	ON_COMMAND(ID_REPAIR_ALL_PTZTEXT, OnRepairAllPtZText)
	//
#ifndef TRIAL_VERSION
	ON_COMMAND(ID_EXPORT_FDB, OnExportFdb)
#endif
	ON_COMMAND(ID_IMPORT_ARCGISMDB, OnImportArcgisMdb)
	ON_COMMAND(ID_EXPORT_ARCGISMDB, OnExportArcgisMdb)
	ON_COMMAND(ID_MAPDECORATE, OnMapDecorate)
	ON_COMMAND(ID_SELECT_FILTER, OnFilterSelect)
	ON_COMMAND(ID_SELECT_REVERSE, OnSelectReverse)
	ON_COMMAND(ID_SETMAPINFO, OnSetMapInfo)
	ON_COMMAND(ID_TOOL_CLEAR_CADBLOCKS, OnClearCADBlocks)
	ON_COMMAND(ID_IMPORT_CADSYMBOLS,OnImportCADSymbols)
	ON_COMMAND(ID_DELETE_CADSYMBOLS,OnDeleteCADSymbols)
	ON_COMMAND(ID_WHEELSPEED_SET,OnWheelSpeedSet)
	ON_COMMAND(ID_RESET_LAYER_CMDPARAMS,OnResetAllLayerCmdParams)
	ON_COMMAND(ID_SELECT_LOCATE,OnSelectLocate)
	ON_COMMAND(ID_SELECT_FTRLAYER,OnLayerLocate)
	ON_COMMAND(ID_COLOR_SET, OnDlgSetColor)
	ON_COMMAND(ID_FILE_OUTPUT_LAYERCHECK, OnDlgOutputLayerCheck)
	ON_COMMAND(ID_ANNOTIP, OnDlgAnnoTip)
	ON_COMMAND(ID_FILL_RAND_COLOR, OnFillRandColor)

	ON_COMMAND(ID_SWITCH_POINT,OnSwitchPoint)
	ON_COMMAND(ID_SWITCH_LINE,OnSwitchLine)
	ON_COMMAND(ID_SWITCH_SURFACE,OnSwitchSurface)
	ON_COMMAND(ID_SWITCH_TEXT,OnSwitchText)
	ON_COMMAND(ID_HIDE_FTR_NOTGIS,OnHideFtrsNotGIS)
	ON_COMMAND(ID_SELECT_LIST, OnSelectToChecklist)

	ON_UPDATE_COMMAND_UI(ID_SWITCH_POINT,OnUpdateSwitchPoint)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_LINE,OnUpdateSwitchLine)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_SURFACE,OnUpdateSwitchSurface)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_TEXT,OnUpdateSwitchText)
	ON_UPDATE_COMMAND_UI(ID_HIDE_FTR_NOTGIS,OnUpdateHideFtrsNotGIS)

	ON_COMMAND(ID_SET_OSGB_EXTENT, OnSetOsgbExtent)

	ON_COMMAND(ID_TOP_VIEW, &CDlgDoc::OnTopView)
	ON_COMMAND(ID_LEFT_VIEW, &CDlgDoc::OnLeftView)
	ON_COMMAND(ID_OPEN_OSGB, &CDlgDoc::OnOpenOsgb)
	ON_COMMAND(ID_REMOVE_OSGB, &CDlgDoc::OnRemoveOsgb)
	END_MESSAGE_MAP()


void RegisterPermanentObjects()
{
	RPO(CLS_USERLAYER,(LPPROC_PERCREATE)CFtrLayer::CreateObject,NULL);
	
	RPO(CLS_GEOMULTIPOINT,(LPPROC_PERCREATE)CGeoMultiPoint::CreateObject,StrFromResID(IDS_GEO_MULTIPOINT));
	RPO(CLS_GEOMULTISURFACE,(LPPROC_PERCREATE)CGeoMultiSurface::CreateObject,StrFromResID(IDS_GEO_MULTISURFACE));
	RPO(CLS_GEODEMPOINT,(LPPROC_PERCREATE)CGeoDemPoint::CreateObject,StrFromResID(IDS_GEO_DEMPOINT));

	RPO(CLS_GEODCURVE,(LPPROC_PERCREATE)CGeoDCurve::CreateObject,StrFromResID(IDS_GEO_DCURVE));

	RPO(CLS_GEODIRPOINT,(LPPROC_PERCREATE)CGeoDirPoint::CreateObject,StrFromResID(IDS_GEO_DIRPOINT));

	RPO(CLS_GEOTEXT,(LPPROC_PERCREATE)CGeoText::CreateObject,StrFromResID(IDS_GEO_TEXT));
	RPO(CLS_GEOPARALLEL,(LPPROC_PERCREATE)CGeoParallel::CreateObject,StrFromResID(IDS_GEO_PARALLEL));
	RPO(CLS_GEOSURFACE,(LPPROC_PERCREATE)CGeoSurface::CreateObject,StrFromResID(IDS_GEO_SURFACE));
	RPO(CLS_GEOPOINT,(LPPROC_PERCREATE)CGeoPoint::CreateObject,StrFromResID(IDS_GEO_POINT));
	RPO(CLS_GEOCURVE,(LPPROC_PERCREATE)CGeoCurve::CreateObject,StrFromResID(IDS_GEO_CURVE));
	RPO(CLS_DLGFEATURE,(LPPROC_PERCREATE)CFeature::CreateObject,NULL);
	RPO(CLS_FEATURE,(LPPROC_PERCREATE)CFeature::CreateObject,NULL);
	RPO(CLS_GEOSURFACEPOINT,(LPPROC_PERCREATE)CGeoSurfacePoint::CreateObject,StrFromResID(IDS_GEO_SURFACEPOINT));
}


BOOL CMYTinObj::GetTriangle(int i, MYPT_3D *ret_pts)
{
	if (i < 0 || i >= nTIN)
		return FALSE;

	if (tins == NULL || pts == NULL)
		return FALSE;

	MYTIN tin = tins[i];
	if (tin.i1<0 || tin.i1>npt ||
		tin.i2<0 || tin.i2>npt ||
		tin.i3<0 || tin.i3>npt)
	{
		return FALSE;
	}

	ret_pts[0] = pts[tin.i1];
	ret_pts[1] = pts[tin.i2];
	ret_pts[2] = pts[tin.i3];

	return TRUE;
}

static bool PtInTri(MYPT_3D *pt, double x, double y)
{
	// 算法为Cyrus-Beck算法
	double lfFlag[3];
	lfFlag[0] = (pt[1].x - pt[0].x)*(y - pt[0].y) - (x - pt[0].x)*(pt[1].y - pt[0].y);
	lfFlag[1] = (pt[2].x - pt[1].x)*(y - pt[1].y) - (x - pt[1].x)*(pt[2].y - pt[1].y);
	lfFlag[2] = (pt[0].x - pt[2].x)*(y - pt[2].y) - (x - pt[2].x)*(pt[0].y - pt[2].y);
	return ((lfFlag[0] >= 0 && lfFlag[1] >= 0 && lfFlag[2] >= 0) ||
		(lfFlag[0] <= 0 && lfFlag[1] <= 0 && lfFlag[2] <= 0));
}

BOOL  CMYTinObj::GetZFromTriangle(MYPT_3D * pt, double x, double y, double& z) //return value to z
{
	double v3x = pt[2].x - x;
	double v3y = pt[2].y - y;
	double v2x = pt[1].x - x;
	double v2y = pt[1].y - y;
	double v1x = pt[0].x - x;
	double v1y = pt[0].y - y;
	if (pt[0].z < -9998 || pt[1].z < -9998 || pt[2].z < -9998)
		return FALSE;
	if (PtInTri(pt, x, y))
	{
		double A = v3x*v1y - v3x*v2y + v2x*v3y - v1x*v3y + v1x*v2y - v1y*v2x;
		double B = v2y*v3x*pt[0].z - v3x*v1y*pt[1].z - v2x*v3y*pt[0].z + v1x*v3y*pt[1].z
			- v1x*v2y*pt[2].z + v1y*v2x*pt[2].z;
		z = -B / A;
		return TRUE;
	}
	else
		return FALSE;
}

void CMYTinObj::RefreshTriangleBounds()
{
	if (bounds)
		delete[] bounds;

	bounds = new float[nTIN * 4];
	float *p = bounds;

	MYPT_3D tri[3];

	double x0 = pts[0].x;
	double y0 = pts[0].y;

	for (int i = 0; i < nTIN; i++)
	{
		if (GetTriangle(i, tri))
		{
			Envelope e = CreateEnvelopeFromPts(tri, 3);
			p[0] = e.m_xl - x0;
			p[1] = e.m_yl - y0;
			p[2] = e.m_xh - x0;
			p[3] = e.m_yh - y0;
		}
		else
		{
			p[0] = 0;
			p[1] = 0;
			p[2] = 0;
			p[3] = 0;
		}
		p += 4;
	}
}


int CMYTinObj::FindTriangle(PT_3D pt)
{
	if (bounds == NULL)
		return -1;

	double x0 = pts[0].x;
	double y0 = pts[0].y;

	double x = pt.x - x0;
	double y = pt.y - y0;

	MYPT_3D tri[3];
	float *p = bounds;

	for (int i = 0; i < nTIN; i++)
	{
		if (x >= p[0] && x <= p[2] && y >= p[1] && y <= p[3] && p[2] > p[0])
		{
			if (GetTriangle(i, tri) && PtInTri(tri, pt.x, pt.y))
			{
				return i;
			}
		}

		p += 4;
	}

	return -1;
}


BOOL gbCreatingWithPath = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CDlgDoc construction/destruction

CDlgDoc::CDlgDoc()
{
	m_pCellDefDlg = NULL;

	m_pDataQuery = new CDlgDataQuery();

	m_bPlaceProperties = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_PLACEPROPERTIES,FALSE);
	m_bRCLKSwitch = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RCLKSWITCH,TRUE);
	m_bCADStyle = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_CADSTYLE,FALSE);
	m_bFocusColletion = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_FOCUSCOL,FALSE);
	m_bOverlayGrid = FALSE;
	m_bShowLayDir = FALSE;

	m_nAnchorState = 0;
	m_bAutoSetAnchor = AfxGetApp()->GetProfileInt("Config\\Document","AutoSetAnchor",FALSE);
	m_nRefAll = AfxGetApp()->GetProfileInt("Config\\Document","RefIsAll",0);
	m_fRefBoundExt = GetProfileDouble("Config\\Document","RefBoundExt",0.0);

	m_nBeginBatchUpdateOp = 0;
	m_pSelEx = NULL;

	m_bPromptSave = FALSE;
	m_nPromptRet = IDNO;

//	m_bModifyDEM = FALSE;

	m_nPlugItem = -1;

	m_bObjectGroup = AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_OBJECTGROUP,TRUE);

	m_nActiveData = -1;

	m_pFilterSelect = NULL;

	m_nLastSaveTime = GetTickCount();

	m_bLoadCmdParams = TRUE;

	m_pDlgGroup = NULL;
	m_SetColorDlg = NULL;
	m_OutputLayerCheckDlg = NULL;
	m_pDlgAnnotip = NULL;

	m_nShowLayerState = 0;

	m_bViewPoint = m_bViewCurve = m_bViewSurface = m_bViewText = TRUE;

	m_pDlgSelectShift = NULL;
	m_bHideFtrsNotGIS = FALSE;
}

CDlgDoc::~CDlgDoc()
{
	if(m_SetColorDlg)  delete m_SetColorDlg;
	if(m_OutputLayerCheckDlg)  delete m_OutputLayerCheckDlg;
	if(m_pDlgAnnotip) delete m_pDlgAnnotip;

	CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
	if (pMain)
	{
		CEditExProp *pEdit = pMain->m_EditExPropDlg;
		if (pEdit)
		{
			pEdit->ShowWindow(SW_HIDE);
		}
		CCheckExProp *pCheck = pMain->m_CheckExPropDlg;
		if (pCheck)
		{
			pCheck->ShowWindow(SW_HIDE);
		}
	}
}

CString  CDlgDoc::GetLastCmdName()
{
	return m_RecentCmd.LastCommandName();
}

BOOL CDlgDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CFileDialogEx dlg("New File",FALSE,".fdb",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"*.fdb|*.fdb|*.*|*.*||",NULL);
	if( dlg.DoModal()!=IDOK )
		return FALSE;


	// 设置对话框
	CDlgWorkSpaceBound dlgB;
	dlgB.m_lfX1 = 0;
	dlgB.m_lfY1 = 0;
	dlgB.m_lfX2 = 1000;
	dlgB.m_lfY2 = 0;
	dlgB.m_lfX3 = 1000;
	dlgB.m_lfY3 = 1000;
	dlgB.m_lfX4 = 0;
	dlgB.m_lfY4 = 1000;
	
	dlgB.m_nScale = 2000;
	if( dlgB.DoModal()!=IDOK ) return FALSE;

	CSQLiteAccess *pAccess = new CSQLiteAccess;
	if( !pAccess )
		return FALSE;

	::DeleteFile(dlg.GetPathName());

	pAccess->Attach(dlg.GetPathName());

	CDlgDataSource *pDataSource = new CDlgDataSource(m_pDataQuery);	
	if( !pDataSource )
	{
		delete pAccess;
		return FALSE;
	}
	pDataSource->SetAccessObject(pAccess);
	m_pDataQuery->m_pFilterDataSource = pDataSource;

	OnUpdateOption();

	//设置缺省图元库和线型库
	// 		CCellDefLib *pCellLib = GetCellDefLib();
	// 		CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(dlgB.m_nScale);
	if (config.GetScale() == 0)
	{
		CDlgScheme::CreateNewScale(dlgB.m_nScale);
	}
	SetCellDefLib(pCfgLibManager->GetCellDefLib(dlgB.m_nScale));
	SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(dlgB.m_nScale));
	CScheme *pScheme = pCfgLibManager->GetScheme(dlgB.m_nScale);
	DWORD scale = pCfgLibManager->GetScaleByScheme(pScheme);
	if (scale==0) return FALSE;	
	BeginBatchUpdate();
	pAccess->CreateFileSys(scale, pScheme);
	EndBatchUpdate();

	pDataSource->LoadAll(NULL);	
	pDataSource->ResetDisplayOrder();
	BeginBatchUpdate();
	pDataSource->SaveAllLayers();
	EndBatchUpdate();
	SetScale(dlgB.m_nScale);
	
//	CFtrLayer *pLayer = pDataSource->GetFtrLayerByIndex(0);
	CFtrLayer *pLayer = pDataSource->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
	if( pLayer )
	{
		pDataSource->SetCurFtrLayer(pLayer->GetID());
	}
	
	PT_3D pts[4];
	memset(pts,0,sizeof(pts));
	pts[1].x = dlgB.m_lfX1;
	pts[1].y = dlgB.m_lfY1;
	pts[2].x = dlgB.m_lfX2;
	pts[2].y = dlgB.m_lfY2;
	pts[3].x = dlgB.m_lfX3;
	pts[3].y = dlgB.m_lfY3;
	pts[0].x = dlgB.m_lfX4;
	pts[0].y = dlgB.m_lfY4;
	Envelope e;
	e.CreateFromPts(pts,4);

	m_arrData.Add(pDataSource);
	m_nActiveData = 0;
//	LoadLayerConfig(dlgB.m_nScale);

	m_snap.Init(m_pDataQuery);

	SetLibSourceID((DWORD_PTR)m_arrData[0]);

	UIRequest(UIREQ_TYPE_SHOWUIPARAM,0);
	SetBound(e);
	
	if( dlgB.m_bTmValid )
	{
		TMProjectionZone prj;
		TMDatum datum;

		dlgB.m_tm.GetZone(&prj);
		dlgB.m_tm.GetDatum(&datum);
		
		BeginBatchUpdate();
		pDataSource->SetProjection(prj);
		pDataSource->SetDatum(datum);
		EndBatchUpdate();
	}

//	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
	//ini ftrrecnet
	if (pScheme != NULL)
	{
		//USERIDX *pRecnet = pScheme->GetRecentIdx();
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(pScheme));
	}

	AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(this));
	AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(this));

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDlgDoc serialization

void CDlgDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDoc diagnostics

#ifdef _DEBUG
void CDlgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDlgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDlgDoc commands
int CDlgDoc::GetFtrLayerIDOfFtr(FTR_HANDLE handle)
{
	CFtrLayer *pLayer = GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(handle));
	if (pLayer)
	{
		return pLayer->GetID();
	}
	return -1;

}

CDataQueryEx* CDlgDoc::GetDataQuery()
{
	return m_pDataQuery;
}


CDlgDataSource* CDlgDoc::GetDlgDataSource(int idx)
{
	if( m_arrData.GetSize()<=0 )return NULL;
	if (idx==-1)
	{
		if(m_nActiveData>=m_arrData.GetSize()) return NULL;
		return (CDlgDataSource*)m_arrData.GetAt(m_nActiveData);
	}	
	if ( idx<0||idx>=m_arrData.GetSize() ) return NULL;
	return (CDlgDataSource*)m_arrData.GetAt(idx);
}

CDlgDataSource* CDlgDoc::GetDlgDataSourceOfFtr(CPFeature pFtr)
{
	LONG_PTR info = 0;
	if( m_pDataQuery->GetObjectInfo(pFtr,0,info) )
	{
		return (CDlgDataSource*)info;
	}

	return NULL;
}

int CDlgDoc::GetDlgDataSourceCount()
{
	return m_arrData.GetSize();
}

CDataSource *CDlgDoc::GetDataSource()
{
	return GetDlgDataSource();
}


CSymbol* CDlgDoc::GetSymbol(CFeature *pGeo)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS || !gpCfgLibMan )return NULL;


	return NULL;
}


void CDlgDoc::UpdateDrag(long updatecode, const GrBuffer *pBuf, UpdateViewType updateview)
{
/*	switch( updatecode )
	{
	case ud_ClearDrag:
		UpdateAllViews(NULL, hc_ClearDragLine);
		break;
	case ud_SetConstDrag:
		UpdateAllViews(NULL, hc_SetConstDragLine, (CObject*)pBuf);
		break;
	case ud_AddConstDrag:
		UpdateAllViews(NULL, hc_AddConstDragLine, (CObject*)pBuf);
		break;
	case ud_SetVariantDrag:
		UpdateAllViews(NULL, hc_SetVariantDragLine, (CObject*)pBuf);
		break;
	case ud_AddVariantDrag:
		UpdateAllViews(NULL, hc_AddVariantDragLine, (CObject*)pBuf);
		break;
	case ud_Attach_Accubox:
		UpdateAllViews(NULL, hc_Attach_Accubox, (CObject*)pBuf);
		break;
	case ud_Detach_Accubox:
		UpdateAllViews(NULL, hc_Detach_Accubox, (CObject*)pBuf);
		break;
	case ud_ModifyHeight:
		UpdateAllViews(NULL,hc_ModifyHeight,(CObject*)pBuf);
		break;
	}*/

	HINTCODE hintCode;
	switch( updatecode )
	{
	case ud_ClearDrag:
		hintCode = hc_ClearDragLine; 
		break;
	case ud_SetConstDrag:
		hintCode = hc_SetConstDragLine;
		break;
	case ud_AddConstDrag:
		hintCode = hc_AddConstDragLine;
		break;
	case ud_SetVariantDrag:
		hintCode = hc_SetVariantDragLine;
		break;
	case ud_AddVariantDrag:
		hintCode = hc_AddVariantDragLine;
		break;
	case ud_Attach_Accubox:
		hintCode = hc_Attach_Accubox;
		break;
	case ud_Detach_Accubox:
		hintCode = hc_Detach_Accubox;
		break;
	case ud_ModifyHeight:
		hintCode = hc_ModifyHeight;
		break;
	case ud_UpdateVariantDrag:
		hintCode = hc_UpdateVariantDrag;
		break;
	case ud_UpdateConstDrag:
		hintCode = hc_UpdateConstDrag;
		break;
	}
	
	
	if (updateview == uv_AllView) 
	{
		UpdateAllViews(NULL,hintCode,(CObject*)pBuf);
	}
	else
	{
		POSITION pos = GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView = GetNextView(pos);
			ASSERT_VALID(pView);
			
			if	(updateview == uv_VectorView && pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
			{
				((CVectorView_new*)pView)->OnUpdate(NULL, hintCode, (CObject*)pBuf);
			}
			else if (updateview == uv_StereoView && pView->IsKindOf(RUNTIME_CLASS(CStereoView))) 
			{
				((CStereoView*)pView)->OnUpdate(NULL, hintCode, (CObject*)pBuf);
			}
			else if (pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
			{
				((COsgbView*)pView)->OnUpdate(NULL, hintCode, (CObject*)pBuf);
			}
		}
		
		
	}	
}


void CDlgDoc::UpdateView(long viewid, long updateid, LPARAM param)
{
	UpdateAllViews(NULL,updateid,(CObject*)param);
	return;
}

BOOL CDlgDoc::GetTextParamsFromScheme(CString strLayerName, CValueTable &tab)
{
	CCommand *pCmd = GetCurrentCommand();

	if (!pCmd || !pCmd->IsKindOf(RUNTIME_CLASS(CDrawTextCommand))) return FALSE;

	CDlgDataSource *pDS = GetDlgDataSource();

	if (strLayerName.IsEmpty())
	{
		CFtrLayer *pLayer = pDS->GetCurFtrLayer();
		if (!pLayer) return FALSE;

		strLayerName = pLayer->GetName();
	}

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return FALSE;

	CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(strLayerName);

	if (!pSchemeLayer) return FALSE;

	for (int i=0; i<pSchemeLayer->GetSymbolCount(); i++)
	{
		CSymbol *pSym = pSchemeLayer->GetSymbol(i);

		if (pSym && pSym->GetType() == SYMTYPE_ANNOTATION && ((CAnnotation*)pSym)->m_nAnnoType == CAnnotation::LayerConfig)
		{
			CAnnotation *pAnno = (CAnnotation*)pSym;
			tab.BeginAddValueItem();

			tab.AddValue(PF_FONT,&(CVariantEx((_variant_t)pAnno->m_textSettings.tcFaceName)),StrFromResID(IDS_CMDPLANE_FONT));
			tab.AddValue(PF_CHARH,&(CVariantEx(pAnno->m_textSettings.fHeight*pDS->GetScale()/1000)),StrFromResID(IDS_CMDPLANE_CHARH));
			tab.AddValue(PF_ALIGNTYPE,&(CVariantEx((long)pAnno->m_textSettings.nAlignment)),StrFromResID(IDS_CMDPLANE_ALIGNTYPE));
			tab.AddValue(PF_SHRUG,&(CVariantEx((long)pAnno->m_textSettings.nInclineType)),StrFromResID(IDS_CMDPLANE_SHRUG));
			tab.AddValue(PF_CHARW,&(CVariantEx(pAnno->m_textSettings.fWidScale)),StrFromResID(IDS_CMDPLANE_CHARWS));
			tab.AddValue(PF_CHARI,&(CVariantEx(pAnno->m_textSettings.fCharIntervalScale)),StrFromResID(IDS_CMDPLANE_CHARIS));
			tab.AddValue(PF_LINEI,&(CVariantEx(pAnno->m_textSettings.fLineSpacingScale)),StrFromResID(IDS_CMDPLANE_LINEIS));
			tab.AddValue(PF_SHRUGA,&(CVariantEx(pAnno->m_textSettings.fInclineAngle)),StrFromResID(IDS_CMDPLANE_SHRUGA));
			tab.AddValue(PF_LAYOUT,&(CVariantEx((long)pAnno->m_nPlaceType)),StrFromResID(IDS_CMDPLANE_LAYOUT));

			tab.EndAddValueItem();

			return TRUE;
		}
	}

	return FALSE;



}

BOOL CDlgDoc::UIRequest(long reqtype, LONG_PTR param)
{
	if( reqtype==UIREQ_TYPE_OUTPUTMSG )
	{
		if( param )
		{
			gpMainWnd->m_wndOpSettingBar.OutputMsg((LPCTSTR)param);
			gpMainWnd->m_wndOutputBar.OutputMsg((LPCTSTR)param);
		}
	}
	else if( reqtype==UIREQ_TYPE_OUTPUTMSG_OLDLINE )
	{
		if( param )
		{
			gpMainWnd->m_wndOpSettingBar.OutputMsg((LPCTSTR)param);
			gpMainWnd->m_wndOutputBar.OutputMsg((LPCTSTR)param,TRUE);
		}
	}
	else if( reqtype==UIREQ_TYPE_LOADUIPARAM )
	{
		if( param )
		{
			CUIParam *p = (CUIParam*)param;
		//	gpMainWnd->LoadParams(p);
			gpMainWnd->m_wndOpSettingBar.LoadParams(p);
			gpMainWnd->m_wndOutputBar.LoadParams(p);
		}
	}
	else if( reqtype==UIREQ_TYPE_SHOWUIPARAM )
	{
		if( param==0 )
		{
			gpMainWnd->m_wndOpSettingBar.ClearParams();
			gpMainWnd->m_wndOutputBar.ClearParams();
		}
		else
		{
			CUIParam *p = (CUIParam*)param;
			gpMainWnd->m_wndOpSettingBar.ShowParams(p);
			gpMainWnd->m_wndOutputBar.ShowKeyParams(p);
		}
	}
	else if( reqtype==UIREQ_TYPE_LOADFILEPARAM )
	{
		CCommand *pCmd = GetCurrentCommand();
		int nCmdId = GetCurrentCommandId();
		CFtrLayer *pFtrLayer = GetDlgDataSource()->GetCurFtrLayer();
		//获得命令的参数信息
		CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(GetDlgDataSource()->GetScale());
		if (pPlaceLib && pCmd!=NULL && pFtrLayer!=NULL && nCmdId!=0 && m_bLoadCmdParams)
		{
			BOOL bLoadPlaceParas = FALSE;

			// 采集命令
			if (pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) && !((CDrawCommand*)pCmd)->IsReadCmdParas() )
			{
				for (int i=0;i<pPlaceLib->GetConfigCount();i++)
				{
					CPlaceConfig* pPlaceConfig = pPlaceLib->GetConfig(i);
					if(stricmp(pFtrLayer->GetName(),pPlaceConfig->m_strLayName) == 0)
					{
						if (pPlaceConfig->m_nCommandID != nCmdId)
							break;
						
						if(strlen(pPlaceConfig->m_strCmdParams)>1)
						{
							CMarkup xml;
							if(xml.SetDoc(pPlaceConfig->m_strCmdParams))
							{
								if (xml.FindElem("Data"))
								{
									xml.IntoElem();
									CValueTable tab;
									
									tab.BeginAddValueItem();
									Xml_ReadValueTable(xml,tab);
									tab.EndAddValueItem();
									
									pCmd->SetParams(tab,TRUE);
									
									bLoadPlaceParas = TRUE;
									
									xml.OutOfElem();
								}
							}
						}
						
						break;
						
					}
				}
				
				if (!bLoadPlaceParas && pCmd->IsKindOf(RUNTIME_CLASS(CDrawTextCommand)))
				{
					CValueTable tab;
					if (GetTextParamsFromScheme(pFtrLayer->GetName(),tab))
					{
						pCmd->SetParams(tab,TRUE);
						bLoadPlaceParas = TRUE;
					}
					
				}
			}			

			if (!bLoadPlaceParas)
			{
				CCmdXMLParams *pParam = pPlaceLib->GetCmdXMLParams(m_nCurCmdCreateID);
				if( pParam )
				{
					CMarkup xml;
					if( xml.SetDoc(pParam->m_strCmdParams) )
					{
						if( xml.FindElem("Data") )
						{
							xml.IntoElem();
							
							CValueTable tab;
							
							tab.BeginAddValueItem();
							Xml_ReadValueTable(xml,tab);
							tab.EndAddValueItem();
							
							pCmd->SetParams(tab,TRUE);
							
							xml.OutOfElem();
						}
						
					}
				}
			}
		
		}
	}		
	else if( reqtype==UIREQ_TYPE_SAVEFILEPARAM )
	{
		CCommand *pCmd = GetCurrentCommand();
		//存储命令的参数信息
		CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(GetDlgDataSource()->GetScale());
		if (pPlaceLib && pCmd!=NULL && m_nCurCmdCreateID!=0)
		{
			CValueTable tab;

			tab.BeginAddValueItem();
			pCmd->GetParams(tab);
			tab.EndAddValueItem();
			
			CMarkup xml;
			xml.AddElem("Data");
			xml.IntoElem();
			Xml_WriteValueTable(xml,tab);
			xml.OutOfElem();
			
			CString str = xml.GetDoc();
			
			pPlaceLib->AddCmdXMLParams(m_nCurCmdCreateID,str);

			pPlaceLib->Save();
		}
	}

	return CWorker::UIRequest(reqtype,param);
}


void CDlgDoc::KeyDown(UINT nChar, int flag)
{
	if(m_pDlgSelectShift!=NULL)
	{
		m_pDlgSelectShift->ShowWindow(SW_HIDE);
	}

	if( nChar==VK_ESCAPE )
	{
		if(m_bSelectorOpen) 
		{
			CSelection *pSel = GetSelection();
			int num = 0;
			if(pSel)
			{				
				pSel->GetSelectedObjs(num);				
			}
			if (num>0)
			{
				DeselectAll();
				OnSelectChanged();
			}
			UpdateDrag(ud_ClearDrag,NULL); 			
			CloseSelector();
		}

		if( m_nCurCmdCreateID==ID_MAPDECORATE )
			return;

//		StartDefaultSelect();
		int curState = 0;
 		CCommand *pCurCmd = GetCurrentCommand();
 		if( pCurCmd )
 		{
			curState = pCurCmd->GetState();
 			pCurCmd->Abort();
 		}
 		TryFinishCurProcedure();
		if(m_bCADStyle && curState==PROCSTATE_READY)
		{
			StartDefaultSelect();
		}
	}
	else if (nChar==VK_BACK&&GetKeyState(VK_SHIFT)<0)
	{
		CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgBack,NULL);
		if(pCurProc)
			pCurProc->BatchBack();

		if (m_accuBox.IsOpen())
		{
			CCommand *pCurCmd = GetCurrentCommand();
			if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) )
			{
				DrawingInfo info = ((CDrawCommand*)pCurCmd)->GetCurDrawingInfo();
				if( info.pFtr )
				{
					CArray<PT_3DEX,PT_3DEX> pts;
					pts.Copy(info.pts);
					int size = pts.GetSize();
					
					if (size >= 1)
					{
						m_accuBox.Reset();
						if( size>1 )
						{
							m_accuBox.Click(pts[size-2]);
						}
						
						m_accuBox.Click(pts[size-1]);
					}
					
				}
				
			}
		}
	}
	else if (nChar==VK_BACK && GetCurrentCommand()/* && GetCurrentCommand()->IsKindOf(RUNTIME_CLASS(CDrawCommand)) */)
	{
		CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgBack,NULL);
		if(pCurProc)
			pCurProc->Back();

		if (m_accuBox.IsOpen())
		{
			CCommand *pCurCmd = GetCurrentCommand();
			if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) )
			{
				DrawingInfo info = ((CDrawCommand*)pCurCmd)->GetCurDrawingInfo();
				if( info.pFtr )
				{
					CArray<PT_3DEX,PT_3DEX> pts;
					pts.Copy(info.pts);
					int size = pts.GetSize();
					
					if (size >= 1)
					{
						m_accuBox.Reset();
						if( size>1 )
						{
							m_accuBox.Click(pts[size-2]);
						}
						
						m_accuBox.Click(pts[size-1]);
					}
					
				}
				
			}
		}
		
	}
	// 跳到定位点
	else if (nChar == VK_HOME)
	{
		OnAnchorptJump();
	}
	else if ((nChar == 'A' || nChar == 'a') && GetKeyState(VK_SHIFT)<0 && GetKeyState(VK_CONTROL)<0)
	{
		m_bObjectGroup = !m_bObjectGroup;
		AfxGetApp()->WriteProfileInt(REGPATH_SELECT,REGITEM_OBJECTGROUP,m_bObjectGroup);
		OnUpdateOption();
		CString str;
		if (m_bObjectGroup)
		{
			str.Format(IDS_GROUP_OPEN);
		}
		else
		{
			str.Format(IDS_GROUP_CLOSE);
		}

		GOutPut(str);
	}
	else
	{
		CCommand *pCur = GetCurrentCommand();
		if (nChar == VK_DELETE && (pCur==NULL || (pCur!=NULL && !pCur->IsKindOf(RUNTIME_CLASS(CDlgEditVertexCommand)))))
		{
			CancelCurrentCommand();
			StartCommand(ID_MODIFY_DEL);
		}
		else if( pCur==NULL && (nChar == 'W' || nChar == 'w') )//选择状态下
		{
			if( GetKeyState(VK_SHIFT)>=0 && GetKeyState(VK_CONTROL)>=0 && DeleteCurVertexs() )
				return;
			if( GetKeyState(VK_SHIFT)<0 && DeleteCurSegments() )
				return;
			if( GetKeyState(VK_CONTROL)<0 && BreakByPt() )
				return;

			AfxGetMainWnd()->SendMessage(FCCM_KEYIN,nChar);
		}
		else
			AfxGetMainWnd()->SendMessage(FCCM_KEYIN,nChar);
	}
}

void CDlgDoc::RefreshView()
{
	UpdateAllViews(NULL, hc_Refresh);
}

void CDlgDoc::OnUpdateRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nCurUndoPos<m_arrUndos.GetSize()-1);
}

void CDlgDoc::OnUpdateUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nCurUndoPos>=0);
}

void CDlgDoc::OnRedo() 
{
	CancelCurrentCommand();

	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;
	CAccessModify *pAccess = pDS->GetAccessObject();
	if( pAccess )
	{
		BeginBatchUpdate();
	}

	Redo();

	if( pAccess )
	{
		EndBatchUpdate();
	}

	OnSelectChanged();
	StartCommand(m_nCurCmdCreateID);
	UpdateAllViews(NULL,hc_Refresh);
	
	gpMainWnd->m_wndFtrLayerView.Refresh();
}

void CDlgDoc::OnUndo() 
{
	CancelCurrentCommand();
	
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;
	CAccessModify *pAccess = pDS->GetAccessObject();
	if( pAccess )
	{
		BeginBatchUpdate();
	}
	
	Undo();
	
	if( pAccess )
	{
		EndBatchUpdate();
	}
	
	OnSelectChanged();
	StartCommand(m_nCurCmdCreateID);
	UpdateAllViews(NULL,hc_Refresh);
	
	gpMainWnd->m_wndFtrLayerView.Refresh();
}

BOOL CDlgDoc::StartCommand(int id)
{
	if( id!=m_nCurCmdCreateID )
	{
		CMDREG Reg ;
		CCommand *pLastCmd = NULL;
		for( int i=m_arrCmdReg.GetSize()-1; i>=0; i--)
		{
			Reg = m_arrCmdReg[i];
			if( Reg.id==id && Reg.lpProcCreate!=NULL )
			{
				pLastCmd = (Reg.lpProcCreate)();
				break;
			}
		}
		
		if( pLastCmd )
		{
			m_RecentCmd.AddCmdItem(id,(LPCTSTR)pLastCmd->Name());
			AfxGetMainWnd()->SendMessage(FCCM_ADD_RECENTCMD,0,(LPARAM)(&m_RecentCmd));
			delete pLastCmd;
		}
	}
	return 	CWorker::StartCommand(id);
}

BOOL CDlgDoc::AddFtrLayer(CFtrLayer *pLayer)
{
	if( !pLayer )return FALSE;

	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return FALSE;

	if( !pDS->AddFtrLayer(pLayer) )
		return FALSE;

	return TRUE;
}


BOOL CDlgDoc::DelFtrLayer(CFtrLayer *pLayer)
{
	if( !pLayer )return FALSE;
	
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return FALSE;

	pDS->DelFtrLayer(pLayer);	
	
	return TRUE;
}


BOOL CDlgDoc::UpdateFtrLayer(CFtrLayer *pLayer)
{
	if( DelFtrLayer(pLayer) && RestoreFtrLayer(pLayer) )
		return TRUE;

	return FALSE;
}


BOOL CDlgDoc::ModifyLayer(CFtrLayer *pLayer, LPCTSTR field, _variant_t& value, BOOL bUndo, BOOL bAllView)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if(!pDS) return FALSE;
    if (stricmp(field,FIELDNAME_LAYERNAME)==0)
    {	
		pLayer->SetName((LPCTSTR)(_bstr_t)value);
    }
	else if (stricmp(field,FIELDNAME_LAYGROUPNAME)==0)
    {	
		pLayer->SetGroupName((LPCTSTR)(_bstr_t)value);
    }
    else if (stricmp(field,FIELDNAME_LAYMAPNAME)==0)
    {	
		pLayer->SetMapName((LPCTSTR)(_bstr_t)value);
    }
	else if (stricmp(field,FIELDNAME_LAYLOCKED)==0)
    {		
		pLayer->EnableLocked((bool)value);
    }
	else if (stricmp(field,FIELDNAME_LAYVISIBLE)==0)
    {	
		if (bAllView)
		{
			pLayer->EnableVisible((bool)value);
		}
		
		UpdateDispyParam param;
		param.type = UpdateDispyParam::typeVISIBLE;
		param.handle =(LONG_PTR)pLayer;
		param.data.bVisible = (bool)value;
		
		UpdateAllViews(NULL,hc_UpdateLayerDisplay,(CObject*)&param);	
	}
	else if (stricmp(field,FIELDNAME_LAYCOLOR)==0)
	{	
		pLayer->SetColor((long)(value));
		UpdateDispyParam param;
		param.type = UpdateDispyParam::typeCOLOR;
		param.handle = (LONG_PTR)pLayer;
		param.data.lColor = (long)(value);

		UpdateAllViews(NULL,hc_UpdateLayerDisplay,(CObject*)&param);		
	}
	else if (stricmp(field,FIELDNAME_LAYERDISPLAYORDER)==0)
    {		
		pLayer->SetDisplayOrder((long)value);

		UpdateAllViews(NULL,hc_UpdateLayerDisplayOrder);
    }
	else if (stricmp(field,FIELDNAME_LAYERSYMBOLIZED)==0)
    {	
		pLayer->EnableSymbolized((bool)value);		
    }
	
	if( bUndo )
	{
		BOOL bCurLayer = pLayer==pDS->GetCurFtrLayer()?TRUE:FALSE;

		UpdateFtrLayer(pLayer);	
	
		if (bCurLayer)
		{
			pDS->SetCurFtrLayer(pLayer->GetID());
		}
		
	}

	return TRUE;
}

BOOL CDlgDoc::ModifyLayerGroup(FtrLayerGroup *pGroup, LPCTSTR field, _variant_t& value, BOOL bUndo, BOOL bAllView)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if(!pDS) return FALSE;
	if (pDS->GetAccessObject()->GetAccessType() == AccessType::UVSModify)
		return FALSE;

    if (stricmp(field,FIELDNAME_LAYGROUPNAME)==0)
	{
		pGroup->Name = (LPCTSTR)(_bstr_t)value;	
	}
	else if (stricmp(field,FIELDNAME_LAYVISIBLE)==0)
	{
		pGroup->Visible = (long)value;							
	}
	else if (stricmp(field,FIELDNAME_LAYCOLOR)==0)
	{
		pGroup->Color = (long)value;			
	}
	else if (stricmp(field,FIELDNAME_LAYERSYMBOLIZED)==0)
	{
		pGroup->Symbolized = (long)value;			
	}

	pDS->UpdateFtrLayerGroup(pGroup);
	
	CUndoModifyLayer undo(this,_T("ModifyLayerGroup"));
	undo.field.Format("%s",field);
	undo.newVar = value;

	for (int j=0; j<pGroup->ftrLayers.GetSize(); j++)
	{
		_variant_t data;
		CFtrLayer *pLayer = pGroup->ftrLayers[j];
		if (stricmp(field,FIELDNAME_LAYGROUPNAME)==0)
		{
			data = pGroup->Name;	
		}
		else if (stricmp(field,FIELDNAME_LAYVISIBLE)==0)
		{
			data = (bool)pLayer->IsVisible();							
		}
		else if (stricmp(field,FIELDNAME_LAYCOLOR)==0)
		{
			data = (long)pLayer->GetColor();			
		}
		else if (stricmp(field,FIELDNAME_LAYERSYMBOLIZED)==0)
		{
			data = (bool)pLayer->IsSymbolized();			
		}
		
		undo.arrLayers.Add(pLayer);
		undo.arrOldVars.Add(data);
		ModifyLayer(pLayer,field,value,bUndo);
		
	}
	
	if (bUndo)
	{
		undo.Commit();
	}

	return TRUE;
}

void CDlgDoc::CutObjsToLayer(CFtrLayer *pLayer1, CFtrLayer *pLayer2, BOOL bUndo)
{
	long lSum = 0;	
	CArray<CFeature*,CFeature*> arr;
	CArray<int,int> arrLayID;
	int i, nsz = pLayer1->GetObjectCount();
	for (int j=0;j<nsz;j++)
	{
		CFeature *pFtr = pLayer1->GetObject(j);
		if(!pFtr)continue;
		arrLayID.Add(pLayer1->GetID());
		arr.Add(pFtr);
		lSum++;
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lSum);
	CArray<PT_3DEX,PT_3DEX> pts;
	CFeature *pFtr0 = NULL;
	CFeature *pTempl = pLayer2->CreateDefaultFeature(GetDlgDataSource()->GetScale());
	if (!pTempl)
		return;
	
	CUndoFtrs undo(this,_T("Change_Layer"));	
	int nCls = pTempl->GetGeometry()->GetClassType();

	if (nCls == CLS_GEOTEXT)
	{
		for(i=0;i<arr.GetSize();i++)
		{
			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
			CFeature *pFtr = arr.GetAt(i);
			if( !pFtr )continue;
			int cls = pFtr->GetGeometry()->GetClassType();
			
			if(cls!=CLS_GEOTEXT)
			{
				continue;
			}
			else
			{
				pFtr0 = pTempl->Clone();
				if(!pFtr0)
					continue;
				pFtr->GetGeometry()->GetShape(pts);
				pFtr0->SetID(OUID());
				if(pFtr0->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize()))
				{
					AddObject(pFtr0,pLayer2->GetID());
					undo.AddNewFeature(FtrToHandle(pFtr0));
				}
				CAttributesSource *pXDS = GetDlgDataSource()->GetXAttributesSource();
				if(pXDS)pXDS->CopyXAttributes(pFtr,pFtr0);
				DeleteObject(FtrToHandle(pFtr),FALSE);	
				undo.AddOldFeature(FtrToHandle(pFtr));
			}				
		}
	}
	else if (nCls==CLS_GEOPOINT||nCls==CLS_GEODIRPOINT||nCls==CLS_GEOSURFACEPOINT)
	{
		for(i=0;i<arr.GetSize();i++)
		{
			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
			CFeature *pFtr = arr.GetAt(i);
			if( !pFtr )continue;
			int cls = pFtr->GetGeometry()->GetClassType();

			if(cls!=CLS_GEOPOINT && cls!=CLS_GEODIRPOINT && cls!=CLS_GEOSURFACEPOINT)
			{
				continue;
			}
			else
			{
				pFtr0 = pTempl->Clone();
				if(!pFtr0)
					continue;
				pFtr->GetGeometry()->GetShape(pts);
				pFtr0->SetID(OUID());
				if(pFtr0->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize()))
				{
					AddObject(pFtr0,pLayer2->GetID());
					undo.AddNewFeature(FtrToHandle(pFtr0));
				}
				CAttributesSource *pXDS = GetDlgDataSource()->GetXAttributesSource();
				if(pXDS)pXDS->CopyXAttributes(pFtr,pFtr0);
				DeleteObject(FtrToHandle(pFtr),FALSE);	
				undo.AddOldFeature(FtrToHandle(pFtr));
			}				
		}
	}
	else if (nCls==CLS_GEOCURVE||nCls==CLS_GEOPARALLEL||nCls==CLS_GEOSURFACE||nCls==CLS_GEOMULTISURFACE||nCls==CLS_GEOMULTIPOINT||nCls==CLS_GEODEMPOINT)
	{
		for(i=0;i<arr.GetSize();i++)
		{
			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
			CFeature *pFtr = arr.GetAt(i);
			if( !pFtr )continue;
			int cls = pFtr->GetGeometry()->GetClassType();
			if(cls==CLS_GEOPOINT || cls==CLS_GEODIRPOINT || nCls==CLS_GEOSURFACEPOINT)
			{
				continue;
			}
			else
			{
				pFtr0 = pTempl->Clone();
				if(!pFtr0)
					continue;
				pFtr->GetGeometry()->GetShape(pts);
				pFtr0->SetID(OUID());
				if(pFtr0->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize()))
				{
					AddObject(pFtr0,pLayer2->GetID());
					undo.AddNewFeature(FtrToHandle(pFtr0));
				}
				CAttributesSource *pXDS = GetDlgDataSource()->GetXAttributesSource();
				if(pXDS)pXDS->CopyXAttributes(pFtr,pFtr0);				
				DeleteObject(FtrToHandle(pFtr),FALSE);					
				undo.AddOldFeature(FtrToHandle(pFtr));
			}
		}
	}
	else
	{
		delete pTempl;
		pTempl = NULL;
		return;
	}

	if( bUndo )undo.Commit();
	delete pTempl;
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);

	RefreshView();
}


BOOL CDlgDoc::AddObject(CPFeature pFtr, int layid)
{
	if( !pFtr )return FALSE;

	if( !CheckObjectNum(m_pDataQuery->GetObjectCount()) )
	{
		CString strErr;
		strErr.Format(IDS_ERR_LIMITNUM,300);
		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)strErr);
		return FALSE;
	}

	pFtr->SetCurrentModifyTime();

	CDlgDataSource *pDS = GetDlgDataSource();	
	if( !pDS )return FALSE;
	BOOL ret;
	if (layid==-1)
	{
		ret = pDS->AddObject(pFtr);
	}
	else
	{
		ret = pDS->AddObject(pFtr,layid);
	}
	if (ret)
	{
		UpdateView(0, hc_AddObject, (LONG_PTR)(pFtr));
		//UpdateAllViews(0,hc_AddObject,(CObject*)(pFtr));
	}
	return ret;	
}

BOOL CDlgDoc::AddObjectWithProperties(CPFeature pFtr, int layid)
{
	BOOL bRes = AddObject(pFtr,layid);
	if( m_bPlaceProperties )
	{		
		CDlgExtProperties dlg;
		if (dlg.Init(this,pFtr))
		{
			dlg.DoModal();

 			DeleteObject(FtrToHandle(pFtr),FALSE);
 			RestoreObject(FtrToHandle(pFtr));

			//update
			//UpdateObject(FtrToHandle(pFtr),TRUE);
		}		
	}

	if(m_bFocusColletion)
	{
		((CMainFrame*)AfxGetMainWnd())->m_wndCollectionView.OnSetFocusEdit();
	}

	return bRes;
}

BOOL CDlgDoc::RestoreObject(FTR_HANDLE handle)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return FALSE;

	HandleToFtr(handle)->SetCurrentModifyTime();

	CDataQuery *pDQ = GetDataQuery();
	if( !pDQ )return FALSE;
	
	BOOL bRet = CWorker::RestoreObject(handle);

	return bRet;
}


BOOL CDlgDoc::DeleteObject(FTR_HANDLE handle, BOOL bUpdateSel)
{	
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return FALSE;

	CDataQuery *pDQ = GetDataQuery();
	if( !pDQ )return FALSE;

	BOOL bRet = CWorker::DeleteObject(handle,bUpdateSel);
	
	return bRet;
}

BOOL CDlgDoc::UpdateObject(FTR_HANDLE handle, BOOL bOnlyUpdateView)
{
	CDataSource *pDS = GetDataSource();
	CDataQuery  *pDQ = GetDataQuery();
	ASSERT(pDS);

	if( !bOnlyUpdateView )
	{
		if( !pDS->DeleteObject(HandleToFtr(handle)) )
			return FALSE;
	}

	UpdateView(0,hc_DelObject,(LPARAM)(HandleToFtr(handle)));
	if( !bOnlyUpdateView )
	{
		if( !pDS->RestoreObject(HandleToFtr(handle)) )
			return FALSE;
	}
	UpdateView(0,hc_AddObject,(LPARAM)(HandleToFtr(handle)));

	return TRUE;
}

BOOL CDlgDoc::UpdateObjectKeepOrder(FTR_HANDLE handle, BOOL bOnlyUpdateView)
{
	CDataSource *pDS = GetDataSource();
	CDataQuery  *pDQ = GetDataQuery();
	ASSERT(pDS);
	
	if( !bOnlyUpdateView )
	{
		if( !((CDlgDataSource*)pDS)->DeleteObjectKeepOrder(HandleToFtr(handle)) )
			return FALSE;
	}
	
	UpdateView(0,hc_DelObject,(LPARAM)(HandleToFtr(handle)));
	if( !bOnlyUpdateView )
	{
		if( !pDS->RestoreObject(HandleToFtr(handle)) )
			return FALSE;
	}
	UpdateView(0,hc_AddObject,(LPARAM)(HandleToFtr(handle)));
	
	return TRUE;
}

BOOL CDlgDoc::ClearObject(FTR_HANDLE handle, BOOL bUpdateSel )
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return FALSE;
	
	CDataQuery *pDQ = GetDataQuery();
	if( !pDQ )return FALSE;
	
	BOOL bRet = CWorker::ClearObject(handle,bUpdateSel);
	
	return bRet;
}


void CDlgDoc::OnImportDxf() 
{
	CDxfRead dxfRead(this,TRUE);

	CDlgDataSource *pDS = GetDlgDataSource();

	CreateCADOptAttributes();

	BeginBatchUpdate();
	BOOL bSuccess = dxfRead.OnImportMultiDxf(pDS);
	EndBatchUpdate();

	if(bSuccess)
	{
//		UpdateBound();
//		SetPathName(pathName);
		UpdateAllViews(NULL,hc_UpdateAllObjects);
	}
}


void CDlgDoc::OnExportdxf() 
{
	CDxfWrite dxfWrite;
	BOOL bret = dxfWrite.OnExportDxf();
	if(bret)
	{
		GOutPut(StrFromResID(IDS_EXPORT_OK));
	}
}


BOOL Create3DPointArray(PT_3D *pts, int num, VARIANT *pvar, int size, BOOL b3d)
{
	SAFEARRAYBOUND bound;
	bound.lLbound = 0;
	if (b3d)
	{
		bound.cElements = num*3;
	}
	else
	{
		bound.cElements = num*2;
	}
	
	SAFEARRAY *pArray = SafeArrayCreate(VT_R8,1,&bound);
	
	double HUGEP *pData = NULL;
	SafeArrayAccessData(pArray,(void HUGEP**)&pData);
	
	for( int i=0; i<num; i++)
	{
		PT_3D *p = (PT_3D*)((byte*)(pts)+size*i);
		
		if (b3d)
		{
			pData[i*3] = p->x;
			pData[i*3+1] = p->y;
			pData[i*3+2] = p->z;
		}
		else
		{
			pData[i*2] = p->x;
			pData[i*2+1] = p->y;

		}
	}
	
	SafeArrayUnaccessData(pArray);
	
	VariantInit(pvar);
	pvar->vt = VT_R8|VT_ARRAY;
	pvar->parray = pArray;
	
	return TRUE;
}


void ArrayClear(VARIANT *pvar)
{
	if( pvar->vt&VT_ARRAY )
	{
		SafeArrayDestroy(pvar->parray);
		pvar->parray = NULL;
		pvar->vt = VT_EMPTY;
	}
}

BOOL Get3DPointArray(CArray<PT_3D,PT_3D> &pts, VARIANT *pvar,BOOL b3d)
{
	pts.RemoveAll();

	SAFEARRAY *pArray = pvar->parray;
	
	double HUGEP *pData = NULL;
	SafeArrayAccessData(pArray,(void HUGEP**)&pData);
	
	int num = 0;
	if (b3d)
	{
		num = pArray->rgsabound[0].cElements/3;
	}
	else
	{
		num = pArray->rgsabound[0].cElements/2;
	}
	

	pts.SetSize(num);

	for (int i=0; i<num; i++)
	{
		if (b3d)
		{
			pts[i].x = pData[3*i];
			pts[i].y = pData[3*i+1];
			pts[i].z = pData[3*i+2];
		}
		else
		{
			pts[i].x = pData[2*i];
			pts[i].y = pData[2*i+1];
		}
		
	}
	
	SafeArrayUnaccessData(pArray);
	
	return TRUE;
}

void CDlgDoc::DxfToDem(LPCTSTR strDxf,LPCTSTR strDem,double dx,double dy)
{
/*    CDxfConvert convert; 
    convert.m_strInputFile=strDxf;
    convert.m_dXSpace=dx;
	convert.m_dYSpace=dy; 
	convert.m_strOutputFile=strDem;  
    convert.Convert(-999999,-999999 ,999999 ,999999);*/
	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos1;
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	CString cmd;
	cmd.Format("%s\\IO32\\DxfToDem.exe %s %s  %lf %lf", path, strDxf, strDem, dx, dy);
	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));
	if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo))
		return;
	while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
	{
	}
}

BOOL CDlgDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CheckLicense(54))
	{
		GOutPut(StrFromResID(IDS_ERR_INVALIDLICENSE));
		return FALSE;
	}

	if (g_isFileOnUVS)
	{
		return LoadDatabase(lpszPathName);
	}

	CString path(lpszPathName);
	if( path.Right(4).CompareNoCase(_T(".fdb"))!=0 )
	{
		return FALSE;
	}

	if( !gbCreatingWithPath && _access(lpszPathName,0)==0 )
	{
		if( !CDocument::OnOpenDocument(lpszPathName) )
			return FALSE;
	}
	else
	{
		CDlgWorkSpaceBound dlgB;
		dlgB.m_lfX1 = 0;
		dlgB.m_lfY1 = 0;
		dlgB.m_lfX2 = 1000;
		dlgB.m_lfY2 = 0;
		dlgB.m_lfX3 = 1000;
		dlgB.m_lfY3 = 1000;
		dlgB.m_lfX4 = 0;
		dlgB.m_lfY4 = 1000;
		dlgB.m_lfZmin = -1000.0;
		dlgB.m_lfZmax = 1000.0;

		dlgB.m_nScale = 2000;
		if( dlgB.DoModal()!=IDOK ) return FALSE;

		CSQLiteAccess  *pAccess = new CSQLiteAccess;
		if( !pAccess )
		{
			return FALSE;
		}
		//数据源关联临时文件
		int pos = path.ReverseFind(_T('.'));		
		if(pos == -1)
			return FALSE;
		//		::DeleteFile(path);
		//path.Insert(pos,_T("~tmp"));
		path += _T(".tmp");
		::DeleteFile(path);
			
		//pAccess->Attach(path); 
		if (!pAccess->OpenNew(path))
		{
			delete pAccess;
			AfxMessageBox(IDS_DOC_INVALIDPATH);
			return FALSE;
		}

		CConfigLibManager *pCfgLibManager = gpCfgLibMan;
		CScheme *pScheme = pCfgLibManager->GetScheme(dlgB.m_nScale);
		if (!pScheme)
		{
			GOutPut(StrFromResID(IDS_ERR_INVALIDCONFIGPATH));
			return FALSE;
		}
		DWORD scale = pCfgLibManager->GetScaleByScheme(pScheme);
		if (scale == 0) return FALSE;
			
		CDlgDataSource *pDataSource = new CDlgDataSource(m_pDataQuery);	
		if( !pDataSource )
		{
			delete pAccess;
			return FALSE;
		}
		pDataSource->SetScale(dlgB.m_nScale);
		pDataSource->SetAccessObject(pAccess);
		m_pDataQuery->m_pFilterDataSource = pDataSource;
		m_arrData.Add(pDataSource);
		m_nActiveData = 0;
		SetMapArea();	
		
		ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(dlgB.m_nScale);
		if (config.GetScale() == 0)
		{
			CDlgScheme::CreateNewScale(dlgB.m_nScale);
		}

		SetCellDefLib(pCfgLibManager->GetCellDefLib(dlgB.m_nScale));
		SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(dlgB.m_nScale));
		SetLibSourceID((DWORD_PTR)m_arrData[0]);
			
		pAccess->CreateFileSys(scale, pScheme);
			
		pDataSource->LoadAll(NULL);	
		pDataSource->ResetDisplayOrder();
		BeginBatchUpdate();
		pDataSource->SaveAllLayers();
		EndBatchUpdate();
		SetScale(dlgB.m_nScale);
			
		OnUpdateOption();
			
//			CFtrLayer *pLayer = pDataSource->GetFtrLayerByIndex(0);
		CFtrLayer *pLayer = pDataSource->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
		if( pLayer )
		{
			pDataSource->SetCurFtrLayer(pLayer->GetID());
		}
			
		PT_3D pts[4];
		memset(pts,0,sizeof(pts));
		pts[0].x = dlgB.m_lfX1;
		pts[0].y = dlgB.m_lfY1;
		pts[1].x = dlgB.m_lfX2;
		pts[1].y = dlgB.m_lfY2;
		pts[2].x = dlgB.m_lfX3;
		pts[2].y = dlgB.m_lfY3;
		pts[3].x = dlgB.m_lfX4;
		pts[3].y = dlgB.m_lfY4;
// 			Envelope e;
// 			e.CreateFromPts(pts,4);		
// 			e.m_zl = dlgB.m_lfZmin;
// 			e.m_zh = dlgB.m_lfZmax;

		m_snap.Init(m_pDataQuery);			
			
		UIRequest(UIREQ_TYPE_SHOWUIPARAM,0);
		SetBound(pts,dlgB.m_lfZmin,dlgB.m_lfZmax);

		if( dlgB.m_bTmValid )
		{
			TMProjectionZone prj;
			TMDatum datum;

			dlgB.m_tm.GetZone(&prj);
			dlgB.m_tm.GetDatum(&datum);
				
			BeginBatchUpdate();
			pDataSource->SetProjection(prj);
			pDataSource->SetDatum(datum);
			EndBatchUpdate();
		}
			
		pDataSource->SetModifiedFlag(FALSE);
		//拷贝原始文件
		::CopyFile(LPCTSTR(path),lpszPathName,FALSE);
		SetActualPathName(lpszPathName);
		pDataSource->SetName(lpszPathName);
		m_strTmpFile = path;
			
		CQueryMenu *pMenu = NULL;
		AfxGetMainWnd()->SendMessage(FCCM_GETQUERYMENU,0,(LPARAM)&pMenu);
		if( pMenu )
		{
			if( !pMenu->IsLoaded() )
				pMenu->Load();
		}
			
		//填充采集面板
		ConfigLibItem *pData = &pCfgLibManager->GetConfigLibItemByScale(pDataSource->GetScale());
		AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(0),LPARAM(pData));
		AfxGetMainWnd()->SendMessage(FCCM_REFRESHCHKSCHBAR,0,0);

		//ini ftrrecnet
		if (pScheme != NULL)
		{
			//USERIDX *pRecnet = pScheme->GetRecentIdx();
			AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(pScheme));
		}

		// 插件数据
		int nCnt;
		const PlugItem *pItem = ((CEditBaseApp*)AfxGetApp())->GetPlugins(nCnt);
		for (int i=nCnt-1;i>=0;i--)
		{
			if (pItem[i].bUsed)
			{
				pItem[i].pObj->LoadPluginData((DocId)(CEditor*)this);
			}
				
		}

		Post_InitDoc();

		AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(this));
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(this));

		m_RecentCmd.Load();
		AfxGetMainWnd()->SendMessage(FCCM_LOAD_RECENTCMD, WPARAM(0), LPARAM(&m_RecentCmd));

		return TRUE;
	}
	
	return LoadDatabase(lpszPathName);
}


#ifndef TRIAL_VERSION

void CDlgDoc::OnFileSaveAsxml() 
{
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("*.xml|*.xml|*.*|*.*|"),NULL);
	
	if( dlg.DoModal()!=IDOK )return;
	CXmlAccess xml;
	CString str = dlg.GetPathName();
	if (str.Right(4)!=_T(".xml"))
	{
		str+= ".xml";
	}
	
	xml.OpenWrite(str);

	CDlgDataSource *pDS = GetDlgDataSource();
	BeginBatchUpdate();
	pDS->SaveAll(&xml);
	EndBatchUpdate();

	xml.Close();
}

#endif

void CDlgDoc::OnFileOpenxml() 
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("*.xml|*.xml|*.*|*.*|"),NULL);
	
	if( dlg.DoModal()!=IDOK )return;
	CXmlAccess xml;
	xml.OpenRead(dlg.GetPathName());

	CDlgDataSource *pDS = GetDlgDataSource();
	
	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
	if (pCurLayer == NULL)
	{
		if(pDS->GetFtrLayerCount() > 0)
		{
//			pCurLayer = pDS->GetFtrLayerByIndex(0);
			pCurLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
			pDS->SetCurFtrLayer(pCurLayer->GetID());
		}
	}
	
	// 加载
	xml.BatchUpdateBegin();
	BeginBatchUpdate();	
	
	long idx;
	int lSum = xml.GetFtrLayerCount();
	lSum += xml.GetFeatureCount();
	
	GOutPut(StrFromResID(IDS_LOAD_FEATURES));
	GProgressStart(lSum);	
	
	CUIntArray layid;
	CMap<int, int&, int, int&> idpair;
	
	CFtrLayer *pLayer = xml.GetFirstFtrLayer(idx);
	while( pLayer )
	{
		GProgressStep();
		
//		pLayer->SetMapName(strMapName);
		int pid = pLayer->GetID(), cid;
		pLayer->SetID(0);
		pLayer->SetInherentFlag(FALSE);
		CFtrLayer *pExistLayer = pDS->GetFtrLayer(pLayer->GetName(),pLayer->GetMapName());
		if (pExistLayer == NULL)
		{
			if( !AddFtrLayer(pLayer) )
			{
				delete pLayer;
				pLayer = xml.GetNextFtrLayer(idx);
				continue;
			}
			else
			{
				cid = pLayer->GetID();
				layid.Add(pLayer->GetID());
				
//				if( pDS->GetCurFtrLayer()==NULL )
//					pDS->SetCurFtrLayer(pLayer->GetID());
			}
			
		}
		else
			cid = pExistLayer->GetID();
		idpair.SetAt(pid,cid);
		
		pLayer = xml.GetNextFtrLayer(idx);	
		
	}
	
	int id;
	CFeature *pFt = xml.GetFirstFeature(idx);
	
	while( pFt )
	{
		GProgressStep();
		
		int pid = xml.GetCurFtrLayID();
		if (idpair.Lookup(pid,id))
		{
			pDS->SetCurFtrLayer(id);
		}		
		
		if (AddObject(pFt));
		else
			delete pFt;
		
		pFt = xml.GetNextFeature(idx);
		
	}
	
	GProgressEnd();
	GOutPut(StrFromResID(IDS_LOAD_FINISH));
	
	xml.BatchUpdateEnd();
	xml.Close();
	
	// 加载的空层删掉
	for (int i=0; i<layid.GetSize(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(layid.GetAt(i));		
		if( !pLayer ) 	continue;
		
		if (pLayer->GetObjectCount() == 0)
		{
			pDS->DelFtrLayer(layid.GetAt(i));
		}
	}
	
//	pDS->SaveAll(NULL);
	
	EndBatchUpdate();
	
	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}

	//UpdateBound();	
}



void CDlgDoc::OnImportCtrlpts()
{
	CDlgImportCtrlPts dlg;

	CDlgDataSource *pDS = GetDlgDataSource();

	DWORD dScale = pDS->GetScale();
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	
	int nuse = 0;
	for(int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if (!pFtrLayer || !pFtrLayer->IsInherent()) continue;

		CString strLayerName = pFtrLayer->GetName();

		for(int j=0; j<pScheme->GetLayerDefineCount(); j++)
		{
			CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(j);
			if(strLayerName.CompareNoCase(pLayer->GetLayerName())==0 && pLayer->GetGeoClass()==CLS_GEOPOINT)
			{
				dlg.AddLayerName(strLayerName);
				nuse++;
				break;
			}
		}
	}

	if (nuse <= 0)
	{
		AfxMessageBox(IDS_DOC_NOPOINTLAYER);
		return;
	}
	
	if( dlg.DoModal()!=IDOK )return;
	CString fileName = dlg.m_strFilePath;
	
	
	CFtrLayer *pFtrLayer = pDS->GetFtrLayer(dlg.m_strLayer/*,dlg.m_strMapName*/);

	if (!pFtrLayer)
	{
		pFtrLayer = pDS->CreateFtrLayer(dlg.m_strLayer);
		if (!pFtrLayer) return;
		//pFtrLayer->SetMapName(dlg.m_strMapName);
		pDS->AddFtrLayer(pFtrLayer);
	}

	CString strFtrLayerName = dlg.m_strLayer;
	
	FILE *fp = fopen(fileName,"r");
	if (!fp)
	{
		AfxMessageBox(IDS_FILE_OPEN_ERR);
		return;
	}
	
	char line[1024],name[1024];
	PT_3DEX pt;
	CGeometry *pGeo;
	
	int nSum = 0;
	while (!feof(fp)) 
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		
		pt.z = 0;
		if (sscanf(line,"%s %lf %lf %lf",name,&pt.x,&pt.y,&pt.z) < 3)
			continue;
		
		nSum++;
	}
	fseek(fp,0,SEEK_SET);
	
	PT_3D pt3ds[4];
	pDS->GetBound(pt3ds,NULL,NULL);
	
	GProgressStart(nSum);
	
	pt.pencode = penLine;

	CArray<PT_3DEX,PT_3DEX> arrPts;
	CAttributesSource *pDSX = pDS->GetXAttributesSource();

	CUndoFtrs undo(this,"Import Ctrlpts");

	BeginBatchUpdate();

	while (!feof(fp)) 
	{

		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		
		pt.z = 0;
		if (sscanf(line,"%s %lf %lf %lf",name,&pt.x,&pt.y,&pt.z) < 3)
			continue;
		
		GProgressStep();

		if( dlg.m_bInMapBound && GraphAPI::GIsPtInRegion((PT_3D)pt,pt3ds,4)<0 )
			continue;
		
		CFeature *pFtr = pFtrLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOPOINT);
		if (!pFtr || !pFtr->GetGeometry())  continue;
		pFtr->GetGeometry()->CreateShape(&pt,1);
		arrPts.Add(pt);

		pFtr->SetCode(dlg.m_strMapName);

		pDS->AddObject(pFtr,pFtrLayer->GetID());

		undo.AddNewFeature(FTR_HANDLE(pFtr));

		CValueTable tab;
		tab.BeginAddValueItem();
		tab.AddValue(dlg.m_strField,&CVariantEx((_variant_t)name));
		tab.EndAddValueItem();

		pDSX->SetXAttributes(pFtr,tab);
	}

	undo.Commit();

	if( arrPts.GetSize()>0 && dlg.m_bCreateLine )
	{
		pFtrLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
		
		if (pFtrLayer)
		{
			CFeature *pFtr = pFtrLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOCURVE);
			if (pFtr && pFtr->GetGeometry())
			{
				pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
				pFtr->SetCode(dlg.m_strMapName);
				pDS->AddObject(pFtr,pFtrLayer->GetID());
			}
		}
	}
	
	EndBatchUpdate();

	fclose(fp);

	
	GProgressEnd();
	
	//UpdateBound();
	UpdateAllViews(NULL,hc_UpdateAllObjects);
}

void CDlgDoc::OnExportCtrlpts()
{
	CString filter,title;
	filter.LoadString(IDS_LOADTXT_FILTER2);
	
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, ".txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	FILE *fp = fopen(dlg.GetPathName(),"w");
	if( !fp )return;

	CDlgDataSource *pDS = GetDlgDataSource();

	CAttributesSource *pDSX = pDS->GetXAttributesSource();
	
	//获取实体对象总数以及点的数目
	CGeometry *pGeo = NULL;
	long lObjSum = 0, lPtSum = 0;
	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetEditableObjsCount();
		lObjSum += nObjs;
		nObjs = pLayer->GetObjectCount();
		for(int j=0; j<nObjs; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr) continue;
			pGeo = pFtr->GetGeometry();
			if( !pGeo )continue;
			
			lPtSum += pGeo->GetDataPointSum();
		}
		
//		pLayer->SetGetMode(oldmode);
	}
	
	fprintf(fp,"%10d\n",lPtSum);
	
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lObjSum);
	
	long number = 0;
	//遍历所有层
	for ( i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		
		for(int j=0; j<nObjs; j++)
		{
			
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr ) continue;
			pGeo = pFtr->GetGeometry();
//			pGeo = pLayer->GetObject(j)->GetGeometry();	
			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
			if( !pGeo )continue;

			CValueTable tab;
			tab.BeginAddValueItem();
			pDSX->GetXAttributes(pFtr,tab);
			tab.EndAddValueItem();
			
			CString strNum;
			const CVariantEx *var;
			if (tab.GetValue(0,_T("POINTNUM"),var))
			{
				VariantToText(*var,strNum);
			}
			
			int npt = pGeo->GetDataPointSum();	
			PT_3DEX expt;
			for( int k=0; k<npt; k++)
			{
				expt = pGeo->GetDataPoint(k);
				if (!strNum.IsEmpty())
				{
					fprintf(fp,"%s %.4f %.4f %.4f\n",strNum,expt.x,expt.y,expt.z);
					number++;
				}
				else
				{
					fprintf(fp,"%10d %.4f %.4f %.4f\n",number++,expt.x,expt.y,expt.z);
				}
			}
		}
		
	}
	
	fclose(fp);
	
	//进度条复位
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
}


void CDlgDoc::OnFileReferrences() 
{
// 	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,".fdb",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
// 		filter);
// 	if( dlg.DoModal()!=IDOK ) return;
// 
// 	CXmlAccess xml;
// 	xml.OpenRead(dlg.GetPathName());
// 	
// 	CDlgDataSource *pDS = new CDlgDataSource(m_pDataQuery);
// 
// 	pDS->LoadAll(&xml);
// 	BeginBatchUpdate();
// 	pDS->SaveAll(NULL);
// 	EndBatchUpdate();
// 	xml.Close();
// 
// 	m_arrData.Add(pDS);
// 
// 	UpdateBound();
}


void CDlgDoc::UpdateBound()
{
	int nsz = m_arrData.GetSize();
	if (nsz<=0)
	{
		return;
	}
	PT_3D pts[4];
	GetDlgDataSource()->GetBound(pts,NULL,NULL);
	UpdateView(0, hc_SetMapArea, (LPARAM)pts);
}

void CDlgDoc::OnToolLayermanage() 
{
	static CLayersMgrDlg dlg;

	CDlgDataSource *pDS = GetDlgDataSource();
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();

	dlg.SetDoc(this);
	dlg.DoModal();

	int nLayer = pDS->GetFtrLayerCount();
	if( nLayer>0 && pDS->GetCurFtrLayer()==NULL )
		pDS->SetCurFtrLayer(pDS->GetFtrLayerByIndex(0)->GetID());

	if( pCurLayer!=pDS->GetCurFtrLayer() )
	{
		pCurLayer = pDS->GetCurFtrLayer();		
				
		// 没有则新建一个缺省命令为打点
		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());

		//add to recent
		if (pScheme != NULL)
		{
			CSchemeLayerDefine *pLayerDef = pScheme->GetLayerDefine(pCurLayer->GetName());
			if (!pLayerDef)	return;
			int nMaxLayerNum  = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RECENTLAYER,40);
			pScheme->SetMaxRecentNum(nMaxLayerNum);
			pScheme->AddToRecent(pLayerDef->GetLayerCode(), pCurLayer->GetName());
			pScheme->Save();

			AfxGetMainWnd()->PostMessage(FCCM_INITFTRCLASS, WPARAM(1), LPARAM(pScheme));			
		}
		
		AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(4), LPARAM(pDS->GetCurFtrLayer()));
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(this));
	}
	gpMainWnd->m_wndFtrLayerView.Refresh();
}

void CDlgDoc::OnUpdateShowSelectLayer(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nShowLayerState==1);
}

void CDlgDoc::OnUpdateHideSelectLayer(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nShowLayerState==2);
}

void CDlgDoc::OnToolShowSelectLayer()
{
	if(m_nShowLayerState == 1)
	{
		m_nShowLayerState = 0;
		OnToolShowAllLayer();
		return;
	}
	else
	{
		m_nShowLayerState = 1;
	}
	CDlgDataSource *pDS = GetDlgDataSource();
	int selnum = 0;
	CSelection *pSelect = GetSelection();
	int num;
	const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(num);	
	
	CFtrLayerArray arr;
	int i=0, j=0, k=0;
	for (k=0; k<num; k++)
	{
		CFeature *pFtr = (CFeature*)ftrs[k];
		if (!pFtr) continue;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);

		for(i=0; i<arr.GetSize(); i++)
		{
			if(pLayer == arr[i])
			{
				break;
			}
		}

		if(i==arr.GetSize())
			arr.Add(pLayer);
	}

	int nLayer = pDS->GetFtrLayerCount();
	for(i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer) continue;

		for(k=0; k<arr.GetSize(); k++)
		{
			if(pLayer==arr[k])
				break;
		}

		if(k<arr.GetSize())
		{
			if(!pLayer->IsVisible())
				pLayer->EnableVisible(TRUE);
		}
		else if(k==arr.GetSize())
		{
			if(pLayer->IsVisible())
				pLayer->EnableVisible(FALSE);
		}
	}
	DeselectAll();
	UpdateAllViews(NULL,hc_UpdateAllObjects);
}
void CDlgDoc::OnToolhideSelectLayer()
{
	if(m_nShowLayerState == 2)
	{
		m_nShowLayerState = 0;
		OnToolShowAllLayer();
		return;
	}
	else
	{
		m_nShowLayerState = 2;
	}
	CDlgDataSource *pDS = GetDlgDataSource();
	int selnum = 0;
	CSelection *pSelect = GetSelection();
	int num;
	const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(num);	
	
	CFtrLayerArray arr;
	int i=0, j=0, k=0;
	for (k=0; k<num; k++)
	{
		CFeature *pFtr = (CFeature*)ftrs[k];
		if (!pFtr) continue;
		
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		
		for(i=0; i<arr.GetSize(); i++)
		{
			if(pLayer == arr[i])
			{
				break;
			}
		}
		
		if(i==arr.GetSize())
			arr.Add(pLayer);
	}
	
	int nLayer = pDS->GetFtrLayerCount();
	for(i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer) continue;
		
		for(k=0; k<arr.GetSize(); k++)
		{
			if(pLayer==arr[k])
				break;
		}
		
		if(k==arr.GetSize())
		{
			if(!pLayer->IsVisible())
				pLayer->EnableVisible(TRUE);
		}
		else if(k<arr.GetSize())
		{
			if(pLayer->IsVisible())
				pLayer->EnableVisible(FALSE);
		}
	}
	DeselectAll();
	UpdateAllViews(NULL,hc_UpdateAllObjects);
}
void CDlgDoc::OnToolShowAllLayer()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	int nLayer = pDS->GetFtrLayerCount();
	for(int i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer) continue;

		if(!pLayer->IsVisible())
			pLayer->EnableVisible(TRUE);
	}

	UpdateAllViews(NULL,hc_UpdateAllObjects);
}

//#include <Python.h>

void CDlgDoc::OnToolScripts()
{
// 	Py_Initialize();
// 	PyObject   *pName,*pModule; 
// 	
// 	pName   =   PyString_FromString( "FeatureCore");        
// 	
// 	pModule   =   PyImport_Import(pName); 
// 	//PyRun_SimpleString("execfile('UseMFC.py')");
// 	PyRun_SimpleString("execfile('Progress.py')");
// 	Py_Finalize();
}

void CDlgDoc::OnToolFtrsmanage() 
{
	static CFtrsMgrDlg dlg;
	dlg.DoModal();
	
	gpMainWnd->m_wndFtrLayerView.Refresh();
}

void CDlgDoc::OnToolFtrsorganize()
{
	if( m_pDlgGroup==NULL )
	{
		m_pDlgGroup = new CDlgSetupGroup;
		m_pDlgGroup->Init(this);
		m_pDlgGroup->Create(IDD_GROUP,AfxGetMainWnd());
		m_pDlgGroup->CenterWindow();
	}
	
	m_pDlgGroup->ShowWindow(TRUE);
}

void CDlgDoc::OnToolSchememanage()
{
	ConfigLibItem *pData = &gpCfgLibMan->GetConfigLibItemByScale(GetDlgDataSource()->GetScale());

	for( int m=0; m<GetDlgDataSourceCount(); m++)
	{
		GetDlgDataSource(m)->ClearLayerSymbolCache();
	}

	CDlgScheme dlg;
	dlg.InitMems(*pData,pData->GetScale());
	dlg.m_pDoc = this;
	dlg.DoModal();

	for( m=0; m<GetDlgDataSourceCount(); m++)
	{
		GetDlgDataSource(m)->LoadLayerSymbolCache();
	}

	if (dlg.IsModify())
	{
		//pData = &gpCfgLibMan->GetConfigLibItemByScale(GetDlgDataSource()->GetScale());
		//AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(0),LPARAM(pData));
		GetDlgDataSource()->UpdateFtrQuery();
		UpdateAllViews(NULL,hc_UpdateAllObjects);
	}
	

}

void CDlgDoc::OnCellDefView()
{
	if(m_pCellDefDlg)
	{
		delete m_pCellDefDlg;
		m_pCellDefDlg = NULL;
	}

	
	m_pCellDefDlg = new CDlgCellDefLinetypeView;
	if( !m_pCellDefDlg )return;
	
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	DWORD dScale = GetDlgDataSource()->GetScale();
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(dScale);
	m_pCellDefDlg->SetConfig(config);

//	CSwitchScale scale(config.scale);

// 	m_pCellDefDlg->SetCellDefLib( GetCellDefLib() );
// 	m_pCellDefDlg->SetLineTypeLib( GetBaseLineTypeLib() );
// 	m_pCellDefDlg->SetScale(dScale);
	
	m_pCellDefDlg->SetShowMode(TRUE,FALSE,0,"",TRUE);
	
	m_pCellDefDlg->Create(IDD_CELLDEF_LINETYPE,NULL);
	
	m_pCellDefDlg->ShowWindow(SW_SHOW);
}

void CDlgDoc::OnFileOpenfdb() 
{
	CDlgImportFdb dlg;
	dlg.SetTitle(StrFromResID(IDS_IMPORT_FDB));
	dlg.SetFilter(_T("*.fdb|*.fdb|*.*|*.*||"));
	if( dlg.DoModal()!=IDOK )return;

//	VM_START

	CString strMapName = dlg.GetMapName();
	
	//加密
	if( !CheckLicense(1) )
	{
		return;
	}

//	VM_END

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	
	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
	if (!pScheme)  return;

	CAttributesSource *pDSX = pDS->GetXAttributesSource();

	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
		
	CSQLiteAccess fdb;

	GOutPut(StrFromResID(IDS_LOAD_FEATURES));

	CUndoFtrs undo(this,"Import Fdb");

	for( int i=0; i<dlg.m_arrFileNames.GetSize(); i++)
	{	
		CString fileName = dlg.m_arrFileNames[i];

		BOOL bOpen = fdb.Attach(fileName);
		if( !bOpen )
			continue;	
		
		CString strVersion = fdb.GetFileVersion();
		BOOL bOldVersion = (strVersion.Find(_T("FeatureOne4.1"))<0);

		// 加载
		fdb.BatchUpdateBegin();
		BeginBatchUpdate();	
		
		long idx;
		int lSum = fdb.GetFtrLayerCount();
		lSum += fdb.GetFeatureCount();
		
		CString strMsg;
		strMsg.Format("%2d : %s",i+1,(LPCTSTR)fileName);
		GOutPut(strMsg);
		
		GProgressStart(lSum);	
		
		CUIntArray layid;
		CMap<int, int&, int, int&> idpair;
		
		CFtrLayer *pLayer = fdb.GetFirstFtrLayer(idx);
		while( pLayer )
		{
			GProgressStep();

			int pid = pLayer->GetID(), cid;
			pLayer->SetID(0);
			pLayer->SetInherentFlag(FALSE);

			CFtrLayer *pExistLayer = pDS->GetFtrLayer(pLayer->GetName(),pLayer->GetMapName());
			if (pExistLayer == NULL)
			{
				if( !AddFtrLayer(pLayer) )
				{
					delete pLayer;
					pLayer = fdb.GetNextFtrLayer(idx);
					continue;
				}
				else
				{
					cid = pLayer->GetID();
					layid.Add(pLayer->GetID());
					
					if( pDS->GetCurFtrLayer()==NULL )
						pDS->SetCurFtrLayer(pLayer->GetID());
				}
				
			}
			else
				cid = pExistLayer->GetID();
			idpair.SetAt(pid,cid);
			
			pLayer = fdb.GetNextFtrLayer(idx);	
			
		}

		CString mapName = strMapName;

		if( mapName.CompareNoCase("*")==0 )
		{
			CString fileTitle = fileName;
			int pos = fileName.ReverseFind('\\');
			if( pos>=0 )fileTitle = fileName.Mid(pos+1);
			
			mapName = fileTitle;
		}
		
		int id;
		CFeature *pFt = fdb.GetFirstFeature(idx);
		
		while( pFt )
		{
			GProgressStep();

			pFt->SetCode(mapName);
			
			int pid = fdb.GetCurFtrLayID();
			if (idpair.Lookup(pid,id))
			{
				pDS->SetCurFtrLayer(id);
			}

			//以前的版本的地物，自动给一个线宽；面对象自动补一个闭合点
			if (bOldVersion)
			{
				float wid = 0;
				CFtrLayer *pFtrLayer = pDS->GetCurFtrLayer();
				if (pFtrLayer)
				{
					wid = pScheme->GetLayerDefineLineWidth(pFtrLayer->GetName());
				}
				
				CGeometry *pGeo = pFt->GetGeometry();
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					((CGeoCurve*)pGeo)->m_fLineWidth = wid;
				}
				else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					((CGeoSurface*)pGeo)->m_fLineWidth = wid;
					if( pGeo->GetClassType()==CLS_GEOSURFACE )
					{
						CArray<PT_3DEX,PT_3DEX> arrPts;
						pGeo->GetShape(arrPts);
						if( arrPts.GetSize()>2 )
						{
							arrPts.Add(arrPts[0]);
							pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
						}
					}
				}
			}

			// 导入地物的扩展属性
			BOOL bSucceed = FALSE;
			CValueTable tab;
			tab.BeginAddValueItem();
			bSucceed = fdb.ReadXAttribute(pFt,tab);
			tab.EndAddValueItem();
			
			//ID为空时，会写入缺省扩展属性
			pFt->SetID(OUID());

			if (pDS->IsFeatureValid(pFt) && pDS->AddObject(pFt))
			{
				// 存在扩展属性才导入
				if (bSucceed && tab.GetFieldCount()>0 )
				{
					pDSX->SetXAttributes(pFt,tab);
				}
				
			}
			else
				delete pFt;
			
			undo.arrNewHandles.Add(FTR_HANDLE(pFt));

			pFt = fdb.GetNextFeature(idx);
			
		}
		
		GProgressEnd();		

		fdb.BatchUpdateEnd();
		fdb.Close();

		// 加载的空层删掉
		for (int i=0; i<layid.GetSize(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayer(layid.GetAt(i));		
			if( !pLayer ) 	continue;

			if (pLayer->GetObjectCount() == 0)
			{
				pDS->DelFtrLayer(layid.GetAt(i));
			}
		}

		EndBatchUpdate();
	}

	undo.Commit();

	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}

	GOutPut(StrFromResID(IDS_LOAD_FINISH));

	UpdateAllViews(NULL,hc_UpdateAllObjects);
		
	// 清除undo队列
	DeleteUndoItems();
	
}

//uvs
void CDlgDoc::OnFileOpenfdbDB() 
{
	CDlgImportFdb dlg;
	CString title = StrFromResID(IDS_IMPORT_FDB)+"(UVS)";
	dlg.SetTitle(title);
	dlg.m_bOnDB = TRUE;
	if( dlg.DoModal()!=IDOK )return;

//	VM_START

	CString strMapName = dlg.GetMapName();
	
	//加密
	if( !CheckLicense(1) )
	{
		return;
	}

//	VM_END

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	
	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
	if (!pScheme)  return;

	CAttributesSource *pDSX = pDS->GetXAttributesSource();

	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();

	GOutPut(StrFromResID(IDS_LOAD_FEATURES));

	CUndoFtrs undo(this,"Import Fdb on UVS");

	CString mapName = strMapName;
	if( mapName.CompareNoCase("*")==0 )
	{
		mapName = dlg.m_strPath;
	}

	int i,j;
	for( i=0; i<dlg.m_arrFileNames.GetSize(); i++)
	{	
		CString fileName = dlg.m_arrFileNames[i];

		CAccessModify* pAccess = new CUVSModify;
		if (!pAccess) continue;
		BOOL bOpen = pAccess->OpenRead(fileName);
		if( !bOpen )
			continue;

		CDataQueryEx *pDQ = new CDataQueryEx();
		CDlgDataSource *pTempDS = new CDlgDataSource(pDQ);
		if(!pTempDS)
			continue;
		
		pAccess->SetDataSource(pTempDS);

		pTempDS->SetAccessObject(pAccess);
		pAccess->ReadDataSourceInfo(pTempDS);
		//设置缺省图元库和线型库
/*		SetDataSourceScale(pTempDS->GetScale());
		CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(pTempDS->GetScale());
		SetCellDefLib(pCfgLibManager->GetCellDefLib(pTempDS->GetScale()));
		SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(pTempDS->GetScale()));*/
		
		CString strVersion = ((CUVSModify*)pAccess)->GetFileVersion();
		BOOL bOldVersion = (strVersion.Find(_T("FeatureOne4.1"))<0);

		// 加载
		pAccess->BatchUpdateBegin();
		pTempDS->LoadAll(NULL);
		pAccess->BatchUpdateEnd();
		BeginBatchUpdate();
		
		long idx;
		int lSum0 = pTempDS->GetFtrLayerCount();
		int lSum1 = 0;
		for(j=0; j<lSum0; j++)
		{
			CFtrLayer *pLayer = pTempDS->GetFtrLayerByIndex(j);
			if (!pLayer||!pLayer->IsVisible())  continue;
			lSum1 += pLayer->GetObjectCount();
		}
		
		CString strMsg;
		strMsg.Format("%2d : %s",i+1,(LPCTSTR)fileName);
		GOutPut(strMsg);
		
		GProgressStart(lSum0+lSum1);
		for(j=0; j<lSum0; j++)
		{
			CFtrLayer *pLayer = pTempDS->GetFtrLayerByIndex(j);
			if (!pLayer||!pLayer->IsVisible())  continue;

			//不加载空层
			int nObj = pLayer->GetObjectCount();
			if(nObj<=0) continue;

			CFtrLayer *pExistLayer = pDS->GetFtrLayer(pLayer->GetName(),pLayer->GetMapName());
			if (pExistLayer == NULL)
			{
				pExistLayer = new CFtrLayer();
				pExistLayer->SetName(pLayer->GetName());
				pExistLayer->SetMapName(pLayer->GetMapName());
				pExistLayer->SetID(0);
				if( !AddFtrLayer(pExistLayer) )
				{
					delete pExistLayer;
					continue;
				}
			}

			for(int k=0; k<nObj; k++)
			{
				CFeature *pFt = pLayer->GetObject(k);
				if(pFt==NULL) continue;
				if(dlg.m_bCheckOnlyLocal && strlen(pFt->GetCode())>0)
					continue;

				CFeature *pNew = pFt->Clone();
				if(!pNew) continue;
				pNew->SetCode(mapName);

				//以前的版本的地物，自动给一个线宽；面对象自动补一个闭合点
				if (bOldVersion)
				{
					float wid = 0;
					CFtrLayer *pFtrLayer = pDS->GetCurFtrLayer();
					if (pFtrLayer)
					{
						wid = pScheme->GetLayerDefineLineWidth(pFtrLayer->GetName());
					}
					
					CGeometry *pGeo = pNew->GetGeometry();
					if(!pGeo)
					{
						delete pNew;
						continue;
					}
					if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					{
						((CGeoCurve*)pGeo)->m_fLineWidth = wid;
					}
					else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{
						((CGeoSurface*)pGeo)->m_fLineWidth = wid;
						if( pGeo->GetClassType()==CLS_GEOSURFACE )
						{
							CArray<PT_3DEX,PT_3DEX> arrPts;
							pGeo->GetShape(arrPts);
							if( arrPts.GetSize()>2 )
							{
								arrPts.Add(arrPts[0]);
								pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
							}
						}
					}
				}

				// 导入地物的扩展属性
				BOOL bSucceed = FALSE;
				CValueTable tab;
				tab.BeginAddValueItem();
				bSucceed = pAccess->ReadXAttribute(pFt,tab);
				tab.EndAddValueItem();
				
				//ID为空时，会写入缺省扩展属性
				pNew->SetID(OUID());
				
				if (pDS->IsFeatureValid(pNew) && pDS->AddObject(pNew,pExistLayer->GetID()))
				{
					// 存在扩展属性才导入
					if (bSucceed && tab.GetFieldCount()>0 )
					{
						pDSX->SetXAttributes(pNew,tab);
					}
					
				}
				else
					delete pNew;
				
				undo.arrNewHandles.Add(FTR_HANDLE(pNew));
			}
		}

		pAccess->BatchUpdateEnd();
		pAccess->Close();

		if(pTempDS) delete pTempDS;
		if(pDQ) delete pDQ;
		
		GProgressEnd();

		EndBatchUpdate();
	}

	undo.Commit();

	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}

	GOutPut(StrFromResID(IDS_LOAD_FINISH));

	UpdateAllViews(NULL,hc_UpdateAllObjects);
		
	// 清除undo队列
	DeleteUndoItems();
	
}

void CDlgDoc::OnFileOpendb() 
{
	CDlgImportFdb dlg;
	dlg.SetTitle(StrFromResID(IDS_IMPORT_GDB));
	dlg.SetFilter(_T("*.gdb|*.gdb|*.*|*.*|"));
	if( dlg.DoModal()!=IDOK )return;
	
	CString mapName = dlg.GetMapName();
	if (mapName.CompareNoCase("*") == 0)
	{
		CString fileTitle = dlg.m_strPath;
		int pos = fileTitle.ReverseFind('\\');
		if (pos >= 0)fileTitle = fileTitle.Mid(pos + 1);

		mapName = fileTitle;
	}

#ifdef _WIN64
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	AfxGetApp()->WriteProfileInt("IO32\\ImportMMGDB_Settings", "nScale", pDS->GetScale());

	CString gdbPath = dlg.m_strPath;

	CString tmpfdbPath = GetPathName();
	tmpfdbPath = tmpfdbPath.Left(tmpfdbPath.GetLength() - 4);
	tmpfdbPath += "_gdb.fdb";

	GOutPut(StrFromResID(IDS_PROCESSING));
	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos1;
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	strcat(path, "\\bin\\VectorConvert.exe");

	CString cmd;
	cmd.Format("\"%s\" importMMGDB \"%s\" \"%s\"", path, gdbPath, tmpfdbPath);

	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	//stStartUpInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));
	if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo))
		return;
	while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
	{
	}
	GOutPut(StrFromResID(IDS_PROCESS_END));

	//导入tmpfdbPath
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme)  return;

	CAttributesSource *pDSX = pDS->GetXAttributesSource();

	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();

	CSQLiteAccess fdb;

	GOutPut(StrFromResID(IDS_LOAD_FEATURES));

	CUndoFtrs undo(this, "Import Gdb");

	{
		CString fileName = tmpfdbPath;

		BOOL bOpen = fdb.Attach(fileName);
		if (!bOpen) return;

		// 加载
		fdb.BatchUpdateBegin();
		BeginBatchUpdate();

		long idx;
		int lSum = fdb.GetFtrLayerCount();
		lSum += fdb.GetFeatureCount();

		GProgressStart(lSum);

		CUIntArray layid;
		CMap<int, int&, int, int&> idpair;

		CFtrLayer *pLayer = fdb.GetFirstFtrLayer(idx);
		while (pLayer)
		{
			GProgressStep();

			int pid = pLayer->GetID(), cid;
			pLayer->SetID(0);
			pLayer->SetInherentFlag(FALSE);

			CFtrLayer *pExistLayer = pDS->GetFtrLayer(pLayer->GetName(), pLayer->GetMapName());
			if (pExistLayer == NULL)
			{
				if (!AddFtrLayer(pLayer))
				{
					delete pLayer;
					pLayer = fdb.GetNextFtrLayer(idx);
					continue;
				}
				else
				{
					cid = pLayer->GetID();
					layid.Add(pLayer->GetID());

					if (pDS->GetCurFtrLayer() == NULL)
						pDS->SetCurFtrLayer(pLayer->GetID());
				}

			}
			else
				cid = pExistLayer->GetID();
			idpair.SetAt(pid, cid);

			pLayer = fdb.GetNextFtrLayer(idx);

		}

		int id;
		CFeature *pFt = fdb.GetFirstFeature(idx);

		while (pFt)
		{
			GProgressStep();

			pFt->SetCode(mapName);

			int pid = fdb.GetCurFtrLayID();
			if (idpair.Lookup(pid, id))
			{
				pDS->SetCurFtrLayer(id);
			}

			// 导入地物的扩展属性
			BOOL bSucceed = FALSE;
			CValueTable tab;
			tab.BeginAddValueItem();
			bSucceed = fdb.ReadXAttribute(pFt, tab);
			tab.EndAddValueItem();

			//ID为空时，会写入缺省扩展属性
			pFt->SetID(OUID());

			if (pDS->IsFeatureValid(pFt) && pDS->AddObject(pFt))
			{
				// 存在扩展属性才导入
				if (bSucceed && tab.GetFieldCount() > 0)
				{
					pDSX->SetXAttributes(pFt, tab);
				}

			}
			else
				delete pFt;

			undo.arrNewHandles.Add(FTR_HANDLE(pFt));

			pFt = fdb.GetNextFeature(idx);

		}

		GProgressEnd();

		fdb.BatchUpdateEnd();
		fdb.Close();

		// 加载的空层删掉
		for (int i = 0; i < layid.GetSize(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayer(layid.GetAt(i));
			if (!pLayer) 	continue;

			if (pLayer->GetObjectCount() == 0)
			{
				pDS->DelFtrLayer(layid.GetAt(i));
			}
		}

		EndBatchUpdate();
	}

	undo.Commit();

	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}

	GOutPut(StrFromResID(IDS_LOAD_FINISH));

	UpdateAllViews(NULL, hc_UpdateAllObjects);

	::DeleteFile(tmpfdbPath);

#else

	CDBAccess db;
	db.OpenRead(dlg.GetPathName());
	
	CDlgDataSource *pDS = GetDlgDataSource();

	db.SetRefDataSource(pDS);
	db.m_strMapName = strMapName;

	BeginBatchUpdate();

	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
	if (pCurLayer == NULL)
	{
		if(pDS->GetFtrLayerCount() > 0)
		{
//			pCurLayer = pDS->GetFtrLayerByIndex(0);
			pCurLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
			if (pCurLayer)
			{
//				pDS->SetCurFtrLayer(pCurLayer->GetID());
			}
			
		}
	}

	long idx;
	int lSum = db.GetFeatureCount();
	
	GOutPut(StrFromResID(IDS_LOAD_FEATURES));
	GProgressStart(lSum);	
	
	int id;
	CFeature *pFt = db.GetFirstFeature(idx);
	
	CUndoFtrs undo(this,"Import Gdb");

	while( pFt )
	{
		GProgressStep();
		
		int pid = db.GetCurFtrLayID();		
		
		if (pDS->IsFeatureValid(pFt) && AddObject(pFt,pid));
		else
			delete pFt;
		
		undo.AddNewFeature(FTR_HANDLE(pFt));

		pFt = db.GetNextFeature(idx);
		
	}
	
	undo.Commit();

	GProgressEnd();
	GOutPut(StrFromResID(IDS_LOAD_FINISH));
	
//	pDS->SaveAll(NULL);

	EndBatchUpdate();
	
	db.Close();

	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}
	
//	UpdateBound();
#endif
}

#ifndef TRIAL_VERSION

void CDlgDoc::OnFileSaveAsdb() 
{
	/*CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("*.mdb|*.mdb|*.*|*.*|"),NULL);
	
	if( dlg.DoModal()!=IDOK )return;
	CDBAccess db;
	db.OpenWrite(dlg.GetPathName());
	
	CDlgDataSource *pDS = GetDlgDataSource();

	pDS->SaveAll(&db);
	
	db.Close();*/
}

#endif

void CDlgDoc::OnCloseDocument() 
{
	//判断当前是否正在关闭文档
	HWND hWnd = ::GetActiveWindow();
	HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
	BOOL bClosing = FALSE;
	if (hWnd == hWnd0 || IsChild(hWnd0, hWnd))
		bClosing = TRUE;

	BOOL bModified = FALSE;
	CDlgDataSource *pDS = GetDlgDataSource();
	if( pDS )
	{
		if (pDS->m_bUVSLocalCopy)//uvs数据的本地副本
		{
			Save();
			SaveBakFiles(BakTypeLatest);
			::CopyFile(m_strTmpFile, GetPathName(), FALSE);
			pDS->SetModifiedFlag(FALSE);
			HWND hWnd = ((CMainFrame*)AfxGetMainWnd())->m_wndProjectView.m_hWnd;
			::SendMessage(hWnd,WM_COMMAND, ID_COMMIT_LOCAL, 0);
		}
		else if (GetAccessType(pDS) == UVSModify)//uvs数据
		{
// 			int ret = CUVSModify::WaiteExecuteAll();
// 			if (IDCANCEL == ret || IDYES == ret)
// 				return;
			if (bClosing && pDS->IsModified())
			{
				Save();
				SaveBakFiles(BakTypeLatest);

				// 保存编辑者，修改次数, 编辑时间
				CString type;
				char username[UNLEN];
				ULONG nSize = UNLEN;
				GetUserName(username, &nSize);
				pDS->SaveDataSettings(FIELDNAME_STATEDITOR, FIELDNAME_STATEDITOR, username, type);

				CString name, modifynum;
				pDS->GetDataSettings(FIELDNAME_STATEDITNUM, name, modifynum, type);
				int nModifyNum = 0;
				if(!modifynum.IsEmpty())
				{
					nModifyNum = atoi(modifynum);
				}
				nModifyNum++;
				modifynum.Format("%i", nModifyNum);
				pDS->SaveDataSettings(FIELDNAME_STATEDITNUM, FIELDNAME_STATEDITNUM, modifynum, type);

				CString time;
				time.Format("%d", pDS->GetTotalModifiedTime());
				pDS->SaveDataSettings(FIELDNAME_STATEDITTIME, FIELDNAME_STATEDITTIME, time, type);

				Destroy();
				DeleteUndoItems();
				m_bPromptSave = FALSE;

				AfxGetMainWnd()->SendMessage(FCCM_SHOW_ACCUDLG, 0);
				CDocument::OnCloseDocument();
				return;
			}
		}
		if(pDS->IsModified())		
			bModified = TRUE;
	}
	
	
	if( !m_bPromptSave )
	{
		m_nPromptRet = IDNO;
		if( bModified && bClosing )
		{
			m_nPromptRet = AfxMessageBox(IDS_SAVETIP, MB_YESNOCANCEL);
			if( m_nPromptRet==IDCANCEL )return;

// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
		else if( bClosing )
		{
			m_nPromptRet = IDYES;
		}
	}
	
	if (((pDS && pDS->IsSaved()) || bModified) && bClosing)
	{
		// 保存编辑者，修改次数, 编辑时间
		CString type;
		char username[UNLEN];
		ULONG nSize = UNLEN;
		GetUserName(username,&nSize);
		pDS->SaveDataSettings(FIELDNAME_STATEDITOR,FIELDNAME_STATEDITOR,username,type);
		
		CString name, modifynum;
		pDS->GetDataSettings(FIELDNAME_STATEDITNUM,name,modifynum,type);
		int nModifyNum = 0;
		if (!modifynum.IsEmpty())
		{
			nModifyNum = atoi(modifynum);
		}
		nModifyNum++;
		modifynum.Format("%i",nModifyNum);
		pDS->SaveDataSettings(FIELDNAME_STATEDITNUM,FIELDNAME_STATEDITNUM,modifynum,type);
		
		CString time;
		time.Format("%d",pDS->GetTotalModifiedTime());
		pDS->SaveDataSettings(FIELDNAME_STATEDITTIME,FIELDNAME_STATEDITTIME,time,type);
	}
	
	if( bModified )
	{
//		if( !bClosing || (bClosing&&m_nPromptRet==IDYES) )
		{
			Save();
			if (m_nPromptRet == IDYES)
			{
				SaveBakFiles(BakTypeLatest);
				::CopyFile(m_strTmpFile, GetPathName(), FALSE);
				//CopySqliteFile(m_strTmpFile,GetPathName());
			}
			else
			{
				SaveBakFiles(BakTypeLatest);
			}
			
			
		//	pDS->SetModifiedFlag(FALSE);
		}
	}
		
	if( bClosing )
	{
		Destroy();	
		::DeleteFile(m_strTmpFile);//shy
	}
	
	DeleteUndoItems();
	m_bPromptSave = FALSE;

	AfxGetMainWnd()->SendMessage(FCCM_SHOW_ACCUDLG,0);
	CDocument::OnCloseDocument();
}

extern void gRegisterChkCmd(CDlgDoc *pDoc);
void CDlgDoc::Post_InitDoc()
{
	UpdateSwitchStates();
	gRegisterChkCmd(this);	
}

void CDlgDoc::Destroy()
{	
//	AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(1), LPARAM(NULL));
//	AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(NULL));

	AfxGetMainWnd()->SendMessage(FCCM_CLOSEDOC,0,LPARAM(this));

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		pView->SendMessage(FCCM_VIEWCLEAR);
	}  
	
	if( m_pFilterSelect )
	{
		delete m_pFilterSelect;
		m_pFilterSelect = NULL;
	}
	
	if( m_pDlgGroup )
		delete m_pDlgGroup;
	m_pDlgGroup = NULL;

	if(m_pDlgSelectShift)
		delete m_pDlgSelectShift;
	m_pDlgSelectShift = NULL;

//	SetCurDrawingObj(DrawingInfo());
	
	EndBatchUpdate();
	
// 	m_strDemPath.Empty();
// 	m_dem.Close();
	
	// 其它
	int i;
	for( i=m_arrWaitCmdObj.GetSize()-1; i>=0; i--)
	{
		((CCommand*)m_arrWaitCmdObj[i])->Abort();
		delete (CCommand*)m_arrWaitCmdObj[i];
	}
	
	m_arrWaitCmdObj.RemoveAll();
 	
	m_arrCmdReg.RemoveAll();
	
	if( m_pCellDefDlg )delete m_pCellDefDlg;
	m_pCellDefDlg = NULL;

	if(m_pSelEx)
	{		
		delete m_pSelEx;
		m_pSelEx = NULL;
		AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	}
	CancelCurrentCommand();	

	CCellDefLib *pCLib = GetCellDefLib();
	CBaseLineTypeLib *pLLib = GetBaseLineTypeLib();
	
	for( i=m_arrData.GetSize()-1; i>=0; i--)
	{
		CDlgDataSource *pDS = (CDlgDataSource*)m_arrData.GetAt(i);

		if(pDS!= NULL)
		{
			if( pCLib )pCLib->DelAllFromSourceID((DWORD_PTR)pDS);
			if (pLLib)pLLib->DelAllFromSourceID((DWORD_PTR)pDS);

			if(m_nActiveData!=i)
			{
				CString temp = pDS->GetTempFilePath();

				delete pDS;
				DeleteFile(temp);
			}
			else 
				delete pDS;
		}
	}
	
	m_arrData.RemoveAll();	
	
	if( m_pDataQuery )
	{
		delete m_pDataQuery;
		m_pDataQuery = NULL;
	}	
}

CChkCmd *CDlgDoc::CreateChkCmd(const CString &checkCategory,const CString &checkName)
{
	for (int i=0;i<m_arrChkCmdRegItems.GetSize();i++)
	{
		if (m_arrChkCmdRegItems[i].checkCategory==checkCategory&&m_arrChkCmdRegItems[i].checkName==checkName)
		{
			return (*m_arrChkCmdRegItems[i].lpProc_Create)();
		}		
	}
	return NULL;
}

BOOL CDlgDoc::RegisterChkCmd(LPPROC_CHKCMDCREATE lpProc_Create)
{
	if( !lpProc_Create )return FALSE;	
	ChkCmdReg Regitem;
	CChkCmd *pChkCmd = (lpProc_Create)();
	if (pChkCmd)
	{
		Regitem.checkCategory = pChkCmd->GetCheckCategory();
		Regitem.checkName = pChkCmd->GetCheckName();
		Regitem.lpProc_Create = lpProc_Create;
		Regitem.reasons.RemoveAll();
		pChkCmd->GetAllReasons(Regitem.reasons);
		m_arrChkCmdRegItems.Add(Regitem);
		delete pChkCmd;
	}	
	return TRUE;
}

ChkCmdReg *CDlgDoc::GetChkCmdRegs(int &num)
{
	num = m_arrChkCmdRegItems.GetSize();
	return m_arrChkCmdRegItems.GetData();
}

void CDlgDoc::OnWsDatabound() 
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;

	CDataQuery *pDQ = GetDataQuery();
	if( !pDQ )return;

	Envelope e;

	CFeature *pFtr = NULL;
	const GrBuffer *pGr = NULL;
	POSITION pos = pDQ->GetFirstObjPos();
	if (pos == NULL) return;
	while( pos )
	{
		pFtr = pDQ->GetNextObjByPos(pos);
		if( pFtr )
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);

			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo )
			{
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);
				Envelope e1;
				e1.CreateFromPts(arrPts.GetData(),arrPts.GetSize(),sizeof(PT_3DEX),3);
				e.Union(&e1,3);
			}
		}
	}

	SetBound(e);
}


void CDlgDoc::SetBound(Envelope e)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;

	PT_3D pts[4];
	Envelope evlp = e;
	
	pts[0].x = evlp.m_xl; pts[0].y = evlp.m_yl; pts[0].z = 0; 
	pts[1].x = evlp.m_xh; pts[1].y = evlp.m_yl; pts[1].z = 0; 
	pts[2].x = evlp.m_xh; pts[2].y = evlp.m_yh; pts[2].z = 0; 
	pts[3].x = evlp.m_xl; pts[3].y = evlp.m_yh; pts[3].z = 0; 

	pDS->SetBound(pts,e.m_zl, e.m_zh);

	UpdateBound();
}

void CDlgDoc::SetBound(const PT_3D *pts, double zmin, double zmax)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;
	
	pDS->SetBound(pts,zmin,zmax);
	
	UpdateBound();
}


void CDlgDoc::SetScale(DWORD scale)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;
	
	pDS->SetScale(scale);
}


void CDlgDoc::BeginBatchUpdate()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;

	// 写入数据到磁盘
	CAccessModify *pAccess = pDS->GetAccessObject();
	if( !pAccess )return;

	//以前有过 BeginBatchUpdate，就先结束，再重新开始
	if( m_nBeginBatchUpdateOp>0 )
	{
		pAccess->BatchUpdateEnd();
		pAccess->BatchUpdateBegin();
	}
	else if( m_nBeginBatchUpdateOp==0 )
	{
		pAccess->BatchUpdateBegin();
	}
	
	m_nBeginBatchUpdateOp++;
}


void CDlgDoc::EndBatchUpdate()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return;
	
	// 写入数据到磁盘
	CAccessModify *pAccess = pDS->GetAccessObject();
	if( !pAccess )return;

	//结束前面的 BeginBatchUpdate
	if( m_nBeginBatchUpdateOp>0 )
	{
		pAccess->BatchUpdateEnd();
		m_nBeginBatchUpdateOp--;
	}

	//之前还有过 BeginBatchUpdate，再开始
	if( m_nBeginBatchUpdateOp>0 )
	{
		pAccess->BatchUpdateBegin();
	}
}



void CDlgDoc::ReplaceDemPoints(void *p, PT_3D *polys, int nPt)
{
	CDSM *pDem = (CDSM*)p;
	
	//生成范围
	Envelope e,e0;
	e.CreateFromPts(polys,nPt);
	e0 = e;
	e0.TransformGrdToClt(GetCoordWnd().m_pSearchCS,1);
	//查找对象
	m_pDataQuery->FindObjectInRect(e0,GetCoordWnd().m_pSearchCS);
	int nObj ;
	const CPFeature *ftr = m_pDataQuery->GetFoundHandles(nObj);
	
	if( nObj<=0 )return;
	
	CGeometry *pGeo = NULL, *pObj2;
	PT_3DEX expt;
	CPtrArray arr;
	
	//过滤对象
	for(int i=0; i<nObj; i++)
	{
		pGeo = ftr[i]->GetGeometry();
		if( !pGeo )continue;
		
		if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
		{
			continue;
		}
		
		int nPtSum = pGeo->GetDataPointSum();
		BOOL bModified = FALSE;
		for( int j=0; j<nPtSum; j++)
		{
			expt = pGeo->GetDataPoint(j);
			if( !e.bPtIn(&expt) )continue;
			if( GraphAPI::GIsPtInRegion(PT_3D(expt.x,expt.y,expt.z),polys,nPt)==2 )
			{
				expt.z = pDem->GetZ(expt.x,expt.y);
				if( expt.z>DemNoValues+1 )
				{
					bModified = TRUE;
//					DeleteObject(FtrToHandle(ftr[i]));
					pGeo->SetDataPoint(j,expt);
//					RestoreObject(FtrToHandle(ftr[i]));
				}
			}
		}

		if( bModified )
		{
			DeleteObject(FtrToHandle(ftr[i]),FALSE);
			RestoreObject(FtrToHandle(ftr[i]));
		}
	}
}


void ReplaceDem(CDSM *pDem1, CDSM *pDem2, PT_3D *polys, int nPt)
{
	//生成范围
	Envelope e;
	e.CreateFromPts(polys,nPt);
	
	DEMHEADERINFO dh;
	dh = pDem1->m_demInfo;
	
	RECT rect;
	rect.left	= ( e.m_xl - dh.lfStartX ) /dh.lfDx;
	rect.right	= ( e.m_xh - dh.lfStartX ) /dh.lfDx;
	rect.top	= ( e.m_yl - dh.lfStartY ) /dh.lfDy;
	rect.bottom	= ( e.m_yh - dh.lfStartY ) /dh.lfDy;
	rect.left	= max( 0,rect.left );
	rect.right	= min( dh.nColumn-1,rect.right );
	rect.top	= max( 0,rect.top );
	rect.bottom	= min( dh.nRow-1,rect.bottom );
	
	PT_3D pt3d;
	for( int i=rect.left; i<=rect.right; i++)
	{
		for( int j=rect.top; j<=rect.bottom; j++)
		{
			pt3d.x = i*dh.lfDx + dh.lfStartX;
			pt3d.y = j*dh.lfDy + dh.lfStartY;
			
			if( GraphAPI::GIsPtInRegion(pt3d,polys,nPt)==2 )
			{
				pt3d.z = pDem2->GetZ(pt3d.x,pt3d.y);
				if( pt3d.z>DemNoValues+1 )
				{
					pDem1->m_lpHeight[j*dh.nColumn+i] = pt3d.z;
				}
			}
		}
	}
}


void CDlgDoc::BuildPartDem(PT_3D *polys, int nPt, double dx, double dy)
{
	CPtrArray arrLayers;
	if(GetDlgDataSource()->GetFtrLayer(_T("DEMPoints"),NULL,&arrLayers)==NULL)
		return;
	CFtrLayer *pLayer = NULL;
	double demDx = 0;
	CFeature *pFtr = NULL;
	for (int i=0;i<arrLayers.GetSize();i++)
	{
		pLayer = (CFtrLayer*)arrLayers[i];
		if(pLayer->IsEmpty())
			continue;
		for (int j=0;j<pLayer->GetObjectCount();j++)
		{
			pFtr = pLayer->GetObject(j);
			if(pFtr==NULL)continue;
			if(!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)))
			{
				pFtr = NULL;
				continue;
			}
			if (((CGeoDemPoint*)(pFtr->GetGeometry()))->GetDataPointSum()>50&&((CGeoDemPoint*)(pFtr->GetGeometry()))->CalcuDemDot())
			{
				demDx = ((CGeoDemPoint*)(pFtr->GetGeometry()))->CalcuDemDot();
				break;
			}
			else
				pFtr = NULL;
		}
		if(pFtr!=NULL)
			break;
	}
	if (pFtr==NULL)
	{
		return;
	}
	CDSM dem2;

	//生成范围
	Envelope e,e0;
	e.CreateFromPts(polys,nPt);
	e0 = e;
	double w = e.m_xh-e.m_xl, h = e.m_yh-e.m_yl;
	e.m_xh += w*3; e.m_xl -= w*3;
	e.m_yh += h*3; e.m_yl -= h*3;
	e0.TransformGrdToClt(GetCoordWnd().m_pSearchCS,1);
	w = e0.m_xh-e0.m_xl;
	h = e0.m_yh-e0.m_yl;
	e0.m_xh += w*3; e0.m_xl -= w*3;
	e0.m_yh += h*3; e0.m_yl -= h*3;

	//查找对象
	m_pDataQuery->FindObjectInRect(e0,GetCoordWnd().m_pSearchCS);
	int nObj;
	const CPFeature *ftr = m_pDataQuery->GetFoundHandles(nObj);

	if( nObj<=0 )return;


	CGeometry *pGeo = NULL, *pObj2;
	CFeature *pFt = NULL;
	PT_3DEX expt;
	CPtrArray arr;

	//过滤对象
	for( i=0; i<nObj; i++)
	{
		pGeo = ftr[i]->GetGeometry();
		if( !pGeo )continue;

		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			continue;
		}
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
		{
			continue;
		}
		pFt = ftr[i]->Clone();
		if (!pFt)
		{
			continue;
		}
		pObj2 = pFt->GetGeometry();
		if( !pObj2 )continue;

		//找到第一个在框内的点
		int nPtSum = pObj2->GetDataPointSum();
		for( int j=0; j<nPtSum; j++)
		{
			expt = pObj2->GetDataPoint(j);
			if( e.bPtIn(&expt) )break;
		}

		if( j>=nPtSum )
		{
			delete pObj2;
			continue;
		}

		CArray<PT_3DEX,PT_3DEX> pts;
		pObj2->GetShape(pts);
		//删除框外的点
		for( j=j-1; j>=0; j--)
		{
		//	pObj2->GetPt(j,&expt);
			pts.RemoveAt(j);
		}

		//删除尾部出框的点
		nPtSum = pts.GetSize();
		for( j=nPtSum-1; j>=0; j--)
		{
			expt = pts.GetAt(j);
			if( e.bPtIn(&expt) )break;
			pts.RemoveAt(j);
		}
		pObj2->CreateShape(pts.GetData(),pts.GetSize());
		if( pObj2->IsKindOf(RUNTIME_CLASS(CGeoPoint)) && pObj2->GetDataPointSum()<=0 )
		{
			delete pFt;
			continue;
		}

		if( pObj2->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && pObj2->GetDataPointSum()<=1 )
		{
			delete pFt;
			continue;
		}

		if( pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && pObj2->GetDataPointSum()<=2 )
		{
			delete pFt;
			continue;
		}
		pFt->SetID(OUID());
		AddObject(pFt,GetFtrLayerIDOfFtr(FtrToHandle(ftr[i])));
		arr.Add(pFt);
	}

	if (arr.GetSize() <= 0)  return;

	//生成DEM
	CString dxfName = "c:\\##$$$@@@.dxf";
	CString demName = "c:\\##$$$@@@.dem";
	ObjectsToDxf(arr,dxfName);

	for( i=0; i<arr.GetSize(); i++)
	{
		ClearObject(FtrToHandle((CFeature*)(arr[i])));	
	}	

	DxfToDem(dxfName,demName,demDx,demDx);

	//替换DEM
	if( !dem2.Open(demName) )
		return;

//	ReplaceDem(&m_dem,&dem2,polys,nPt);
	ReplaceDemPoints(&dem2,polys,nPt);

/*	m_bModifyDEM = TRUE;*/

	::DeleteFile(dxfName);
	::DeleteFile(demName);

	UpdateAllViews(NULL,hc_Refresh);
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END));
}

void CDlgDoc::OnStartCommand(CCommand* pCmd)
{
	BeginBatchUpdate();
}


void CDlgDoc::OnEndCommand(CCommand* pCmd)
{
	EndBatchUpdate();
	gpMainWnd->m_wndFtrLayerView.Refresh();  
}


BOOL CDlgDoc::RestoreFtrLayer(CFtrLayer *pLayer)
{
	if( !pLayer )return FALSE;
	
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS )return FALSE;
	
	pDS->RestoreLayer(pLayer);	
	
	return TRUE;
}

void CDlgDoc::OnSelectChanged(BOOL bAlways)
{
	CWorker::OnSelectChanged(bAlways);

	int num;
	const FTR_HANDLE *pFtr = m_selection.GetSelectedObjs(num);
	if (/*!pFtr*/num<=0) 
	{
		if(m_pSelEx)
		{
			AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);

			CFtrLayer *pAcitveLayer = GetDlgDataSource()->GetCurFtrLayer();
			AfxGetMainWnd()->PostMessage(FCCM_INITLAYERTOOL, WPARAM(4), LPARAM(pAcitveLayer));			
			AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(this));

			delete m_pSelEx;
			m_pSelEx = NULL;				
		}
	}
	else
	{
		if( num==1 )
		{
			//得到特征码
// 			CDpDBVariant var;
// 			DpCustomFieldType type;
// 			pObj->GetBind()->GetAttrValue("FID",var,type);

			CFeature *pFirstFtr = HandleToFtr(pFtr[0]);
			if (!pFirstFtr) return;

			if( m_pSelEx && m_pSelEx->IsIncludedObject(pFirstFtr) )
			{
				AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSelEx);				
				AfxGetMainWnd()->PostMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(this));
				return;
			}

			CFtrLayer *pLayer = GetDlgDataSource()->GetFtrLayerOfObject(pFirstFtr);

			CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());

			//地物来自参考数据时，pLayer为NULL；
			if( pLayer && pScheme )
			{			
				CString strHg;
				__int64 code = -1;
				CString name = pLayer->GetName();
				if( pScheme->FindLayerIdx(FALSE,code,name) )
				{
					strHg.Format(IDS_DOC_SELINFO,name,code);
				}
				else
					strHg.Format(IDS_DOC_SELINFO2,name);
				
				PT_3DEX tmpPt;
				CGeometry *pGeo = pFirstFtr->GetGeometry();
				if (pGeo)
				{
					tmpPt = pGeo->GetDataPoint(0);
					//高程信息
					if( CModifyZCommand::CheckObjForContour(pGeo) )
					{
						int nz = floor(-log10(GraphAPI::g_lfZTolerance)+0.5);
						
						CString strFormat;
						strFormat.Format("%%.%df",nz);
						
						CString strZ;					
						strZ.Format((LPCTSTR)strFormat,tmpPt.z);

						CString str;
						str.Format(IDS_DOC_SELINFO3,(LPCTSTR)strZ);
						strHg += str;
					}			
					strHg += _T("\n");
				}

				//定位到当前检查结果项
				if( stricmp(pLayer->GetName(),StrFromResID(IDS_MARKLAYER_NAME))==0 && pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
				{
					((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.SendMessage(WM_COMMAND,ID_RESULT_LOCATEITEM,0);
				}
				
				AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)strHg);

				AfxGetMainWnd()->PostMessage(FCCM_INITLAYERTOOL, WPARAM(4), LPARAM(pLayer));
			}
			
		}
		else
		{
			CString strHg;
			strHg.Format(IDS_DOC_SELINFO4,num);
			AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)((LPCSTR)strHg));

			CFtrLayer *pAcitveLayer = GetDlgDataSource()->GetCurFtrLayer();
			AfxGetMainWnd()->PostMessage(FCCM_INITLAYERTOOL, WPARAM(4), LPARAM(pAcitveLayer));
		}

		CArray<CPermanent*,CPermanent*> arr;
		for (int i=0;i<num;i++)
		{
			arr.Add(HandleToFtr(pFtr[i]));
		}
		if(m_pSelEx) delete m_pSelEx;
		m_pSelEx = new CSelChangedExchanger(arr,this);
		AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSelEx);

		AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(this));
	}

	CEditExProp *pEditExPropDlg = ((CMainFrame*)AfxGetMainWnd())->m_EditExPropDlg;
	if (pEditExPropDlg && pEditExPropDlg->IsWindowVisible())
	{
		pEditExPropDlg->UpdateSelection();
	}
}
void CDlgDoc::RefreshSelProperties()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSelEx);
}


void CDlgDoc::SelectAll()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	pDS->GetFtrLayerCount();
	int i,j;
	CArray<FTR_HANDLE,FTR_HANDLE> arr;
	for ( i=0;i<pDS->GetFtrLayerCount();i++)
	{
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if(!pFtrLayer||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible()) continue;
		
		for (j=0;j<pFtrLayer->GetObjectCount();j++)
		{
			CFeature *pFtr = pFtrLayer->GetObject(j);
			if (!pFtr)  continue;
			arr.Add(FtrToHandle(pFtr));
		}		
	}
	m_selection.SelectAll(arr.GetData(),arr.GetSize());
}

void CDlgDoc::OnShowSnapStatus()
{
	CMenu menu;
	menu.LoadMenu (IDR_POPUP_SNAPSTATUS);
	
	CMenu* pSumMenu = menu.GetSubMenu(0);

	if( pSumMenu )
	{
		if( m_snap.bOpen() )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPOPEN,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPOPEN,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.Is2D() )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_2D,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_2D,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.Is2D() )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_3D,
			MF_BYCOMMAND|MF_UNCHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_3D,
			MF_BYCOMMAND|MF_CHECKED );

		pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_SNAPSELF,
			MF_BYCOMMAND|(m_snap.bSnapSelf()?MF_CHECKED:MF_UNCHECKED) );

		pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_SNAPBASE,
			MF_BYCOMMAND|(m_snap.bOnlyBase()?MF_CHECKED:MF_UNCHECKED) );

		if( m_snap.GetSnapMode()&CSnap::modeEndPoint )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_END,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_END,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modeKeyPoint )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_KEYPOINT,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_KEYPOINT,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modeNearPoint )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_NEARPOINT,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_NEARPOINT,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modeMidPoint )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_MIDPOINT,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_MIDPOINT,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modeCenterPoint )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_CENTERPOINT,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_CENTERPOINT,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modeIntersect )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_INTERSECT,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_INTERSECT,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modePerpPoint )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_PERPPOINT,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_PERPPOINT,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modeTangPoint )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_TANGPOINT,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_TANGPOINT,
			MF_BYCOMMAND|MF_UNCHECKED );

		if( m_snap.GetSnapMode()&CSnap::modePolar )
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_POLAR,
			MF_BYCOMMAND|MF_CHECKED );
		else
			pSumMenu->CheckMenuItem(ID_POPUP_SNAPMODE_POLAR,
			MF_BYCOMMAND|MF_UNCHECKED );

	}
	
	CPoint point;
	GetCursorPos(&point);

	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		
	if(!pPopupMenu->Create(AfxGetMainWnd(), point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
		return;
		
	((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu (pPopupMenu);

	return;
}

void CDlgDoc::OnSnapMode(UINT nID)
{
	switch( nID )
	{
	case ID_POPUP_SNAPMODE_3D:
	case ID_POPUP_SNAPMODE_2D: m_snap.Enable2D(!m_snap.Is2D()); break;
	case ID_POPUP_SNAPSETTING: 
		{
			COptionsDlg dlgOptions (StrFromResID(IDS_OPTIONS), NULL,4);
			dlgOptions.DoModal ();
		}
		break;
	case ID_POPUP_SNAPOPEN: m_snap.Enable(!m_snap.bOpen()); break;
	case ID_POPUP_SNAPMODE_SNAPSELF: m_snap.EnableSnapSelf(!m_snap.bSnapSelf()); break;
	case ID_POPUP_SNAPMODE_NEARPOINT: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modeNearPoint); break; 
	case ID_POPUP_SNAPMODE_KEYPOINT: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modeKeyPoint); break; 
	case ID_POPUP_SNAPMODE_MIDPOINT: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modeMidPoint); break; 
	case ID_POPUP_SNAPMODE_CENTERPOINT: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modeCenterPoint); break; 
	case ID_POPUP_SNAPMODE_INTERSECT: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modeIntersect); break; 
	case ID_POPUP_SNAPMODE_PERPPOINT: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modePerpPoint); break; 
	case ID_POPUP_SNAPMODE_TANGPOINT: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modeTangPoint); break;
	case ID_POPUP_SNAPMODE_END: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modeEndPoint); break;
	case ID_POPUP_SNAPMODE_SNAPBASE: m_snap.EnableOnlyBase(!m_snap.bOnlyBase()); break;
	case ID_POPUP_SNAPMODE_POLAR: m_snap.SetSnapMode(m_snap.GetSnapMode()^CSnap::modePolar); break;
	}

	m_snap.UpdateSettings(TRUE);
}

void CDlgDoc::OnViewXz() 
{ 
	PT_3D cur_pt3d;
	CView *pView = GetActiveView();
	if( pView && pView->IsKindOf(RUNTIME_CLASS(CBaseView)) )
	{
		cur_pt3d = ((CBaseView*)pView)->GetCoordWnd().m_ptGrd;
	}
	CWinApp *pApp = (CWinApp *)AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	
	CString str;
	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			pApp->GetNextDocTemplate(curTemplatePos);
		
		if (curTemplate->GetDocString(str,CDocTemplate::fileNewName))
		{
			if (str.CompareNoCase(_T("Side"))==0)
			{
				CFrameWnd* pFrame = curTemplate->CreateNewFrame(this,NULL);
				curTemplate->InitialUpdateFrame(pFrame, this, TRUE);
				
				if( pView )
				{
					UpdateAllViews(pView,hc_SetCrossPos,(CObject*)&cur_pt3d);
				}
				break;
			}
		}		
	}
}

void CDlgDoc::OnViewSection()
{	
	CWinApp *pApp = AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	
	CDocument *pDoc = NULL;
	CDocTemplate* curTemplate = NULL;
	while(curTemplatePos!=NULL)
	{
		curTemplate = pApp->GetNextDocTemplate(curTemplatePos);
		
		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		while(curDocPos!=NULL)
		{
			pDoc = curTemplate->GetNextDoc(curDocPos);
			if( pDoc==this )break;
		}
		if( pDoc==this )break;
	}
	
	if( !curTemplate || pDoc!=this )return;
	
	//找到当前的文档模板
	CCreateContext context;
	context.m_pCurrentFrame = NULL;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = RUNTIME_CLASS(CEPSectionView);
	context.m_pNewDocTemplate = curTemplate;
	
	CFrameWnd* pFrame = (CFrameWnd*)new CChildFrame;
	if( pFrame==NULL )
		return;
	
	// create new from resource
	if (!pFrame->LoadFrame(IDR_EDITBATYPE,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
		NULL, &context))
	{
		return;
	}
	
	curTemplate->InitialUpdateFrame(pFrame,this,TRUE);
	
	return;
}


void CDlgDoc::OnViewRoad()
{
	CWinApp *pApp = AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	
	CDocument *pDoc = NULL;
	CDocTemplate* curTemplate = NULL;
	while(curTemplatePos!=NULL)
	{
		curTemplate = pApp->GetNextDocTemplate(curTemplatePos);
		
		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		while(curDocPos!=NULL)
		{
			pDoc = curTemplate->GetNextDoc(curDocPos);
			if( pDoc==this )break;
		}
		if( pDoc==this )break;
	}
	
	if( !curTemplate || pDoc!=this )return;
	
	//找到当前的文档模板
	CCreateContext context;
	context.m_pCurrentFrame = NULL;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = RUNTIME_CLASS(CEPRoadView);
	context.m_pNewDocTemplate = curTemplate;
	
	CFrameWnd* pFrame = (CFrameWnd*)new CChildFrame;
	if( pFrame==NULL )
		return;
	
	// create new from resource
	if (!pFrame->LoadFrame(IDR_EDITBATYPE,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
		NULL, &context))
	{
		return;
	}
	
	curTemplate->InitialUpdateFrame(pFrame,this,TRUE);
	
	return;
}

void CDlgDoc::OnOverlayGrid() 
{
	static BOOL bFirstSet = TRUE;
	
	CDlgOverlayGrid dlg;
	GetDlgDataSource()->GetBound(dlg.m_ptRegion,NULL,NULL);
	Envelope evlp;
	evlp.CreateFromPts(dlg.m_ptRegion,4);
	if( bFirstSet )
	{
		dlg.m_bOverlay = m_bOverlayGrid;
		dlg.m_fWid = 10;
		dlg.m_fHei = 10;
		dlg.m_fXOrigin = evlp.m_xl;
		dlg.m_fYOrigin = evlp.m_yl;
		dlg.m_fXRange = evlp.m_xh-evlp.m_xl;
		dlg.m_fYRange = evlp.m_yh-evlp.m_yl;
		dlg.m_strZ = _T("0.0");
		dlg.m_bVectView= TRUE;
		dlg.m_bImgView = FALSE;
	}
	else
	{
		dlg.m_bOverlay = m_bOverlayGrid;
		dlg.m_fWid = GetProfileDouble(REGPATH_OVERLAYGRID,"Width",10);
		dlg.m_fHei = GetProfileDouble(REGPATH_OVERLAYGRID,"Height",10);
		dlg.m_fXOrigin = GetProfileDouble(REGPATH_OVERLAYGRID,"MinX",evlp.m_xl);
		dlg.m_fYOrigin = GetProfileDouble(REGPATH_OVERLAYGRID,"MinY",evlp.m_yl);
		dlg.m_fXRange = GetProfileDouble(REGPATH_OVERLAYGRID,"XRange",evlp.m_xh-evlp.m_xl);
		dlg.m_fYRange = GetProfileDouble(REGPATH_OVERLAYGRID,"YRange",evlp.m_yh-evlp.m_yl);
		dlg.m_strZ = AfxGetApp()->GetProfileString(REGPATH_OVERLAYGRID,"Z",_T("0.0"));
		dlg.m_bVectView= AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"ViewVect",FALSE);
		dlg.m_bImgView = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"ViewImg",FALSE);
	}
	
	dlg.m_bSnapGrid= ((m_snap.GetSnapMode()&CSnap::modeGrid)!=0);
	dlg.m_color = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"Color",RGB(128,128,128));
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	m_bOverlayGrid = dlg.m_bOverlay;
	WriteProfileDouble(REGPATH_OVERLAYGRID,"Width",dlg.m_fWid);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"Height",dlg.m_fHei);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"MinX",dlg.m_fXOrigin);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"MinY",dlg.m_fYOrigin);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"XRange",dlg.m_fXRange);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"YRange",dlg.m_fYRange);
	AfxGetApp()->WriteProfileString(REGPATH_OVERLAYGRID,"Z",dlg.m_strZ);
	AfxGetApp()->WriteProfileInt(REGPATH_OVERLAYGRID,"ViewVect",dlg.m_bVectView);
	AfxGetApp()->WriteProfileInt(REGPATH_OVERLAYGRID,"ViewImg",dlg.m_bImgView);
	AfxGetApp()->WriteProfileInt(REGPATH_OVERLAYGRID,"Color",dlg.m_color);
	
	DWORD dwScale = GetDlgDataSource()->GetScale();
	double lfRatio = 0.01*dwScale;//图上1cm对应实际lfRatio米
	if( dlg.m_bOverlay && dlg.m_bSnapGrid )
	{
		SNAP_GRID sg;
		sg.ox = dlg.m_fXOrigin;	sg.oy = dlg.m_fYOrigin;
		sg.dx = dlg.m_fWid*lfRatio;	sg.dy = dlg.m_fHei*lfRatio;
		sg.nx = fabs(dlg.m_fWid)<1e-10?0:ceil(dlg.m_fXRange/dlg.m_fWid);	
		sg.ny = fabs(dlg.m_fWid)<1e-10?0:ceil(dlg.m_fYRange/dlg.m_fWid);
		m_snap.SetSnapMode(m_snap.GetSnapMode()|CSnap::modeGrid);
		m_snap.SetGridParams(&sg);
	}
	else
		m_snap.SetSnapMode(m_snap.GetSnapMode()&(~CSnap::modeGrid));

	GRIDParam pm;
	pm.ox = dlg.m_fXOrigin;	pm.oy = dlg.m_fYOrigin;
	pm.dx = dlg.m_fWid*lfRatio;	pm.dy = dlg.m_fHei*lfRatio;
	pm.xr = dlg.m_fXRange;	pm.yr = dlg.m_fYRange; 
	strcpy(pm.strz,dlg.m_strZ);
	pm.bVisible = dlg.m_bOverlay;
	pm.bViewVect= dlg.m_bVectView;
	pm.bViewImg = dlg.m_bImgView;
	pm.color = dlg.m_color;
	
	UpdateAllViews(NULL,hc_UpdateGrid,(CObject*)&pm);
	bFirstSet = FALSE;
	
}

void CDlgDoc::OnShowLayerDir()
{
	if(m_bShowLayDir)
	{
		m_bShowLayDir = FALSE;
		UpdateAllViews(NULL,hc_DelGraph);
		return;
	}

	m_bShowLayDir = TRUE;

	CDlgDataSource* pDS = GetDlgDataSource();
	if( !pDS ) return;

	int num ;
	const FTR_HANDLE *handles = GetSelection()->GetSelectedObjs(num);
	
	CFtrLayerArray arr;
	int i,j,k;
	if(num>0)
	{
		for(i=0; i<num; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(handles[i]));
			if(!pLayer) continue;
			for(j=0; j<arr.GetSize(); j++)
			{
				if(pLayer == arr[j]) break;
			}
			if(j==arr.GetSize())
			{
				arr.Add(pLayer);
			}
		}
	}
	else
	{
		int nLay = pDS->GetFtrLayerCount();
		for(i=0; i<nLay; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if(pLayer && pLayer->IsVisible() && pLayer->GetObjectCount()>0)
			{
				arr.Add(pLayer);
			}
		}
	}

	double dirlen = 0.0;
	PT_4D pts[2];
	pts[0].x = 0;
	pts[0].y = 0;
	GetCoordWnd().m_pViewCS->GroundToClient(pts,1);
	pts[1] = pts[0];
	pts[1].x += 10;
	pts[1].y += 10;
	GetCoordWnd().m_pViewCS->ClientToGround(pts,2);
	dirlen = sqrt((pts[1].x-pts[0].x)*(pts[1].x-pts[0].x)+(pts[1].y-pts[0].y)*(pts[1].y-pts[0].y));//  关键在这儿实现坐标的转换
	if( dirlen<=0.0 )dirlen = 1.0;

	GrBuffer buf;

	for(i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr) continue;
			
			CGeometry *pObj = pFtr->GetGeometry();
			if( pObj && pObj->GetDataPointSum()>1 )
			{	
				buf.BeginLineString(pObj->GetColor(),0);
				int ptnum = pObj->GetDataPointSum();
				if( ptnum<2 )return;
				
				PT_3D pt;
				PT_3DEX expt1, expt2;
				double dx, dy, dz, len, slen=0;	
				for( k=0; k<ptnum-1; k++)
				{
					expt1 = pObj->GetDataPoint(k);
					expt2 = pObj->GetDataPoint(k+1);
					
					len = sqrt( (expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y)+
						(expt1.z-expt2.z)*(expt1.z-expt2.z) );
					
					slen += len;
					if( k != 0  && slen<dirlen*20 )continue;
					
					dx = (expt2.x-expt1.x)/len; dy = (expt2.y-expt1.y)/len; dz = (expt2.z-expt1.z)/len;
					
					//中心线起点
					pt.x = expt1.x+dx*(len*0.5-dirlen);
					pt.y = expt1.y+dy*(len*0.5-dirlen);
					pt.z = expt1.z+dz*(len*0.5-dirlen);
					buf.MoveTo(&pt);
					
					//中心线终点
					pt.x = expt1.x+dx*(len*0.5);
					pt.y = expt1.y+dy*(len*0.5);
					pt.z = expt1.z+dz*(len*0.5);
					buf.LineTo(&pt);
					
					//左箭头起点
					pt.x -= dy*dirlen*0.25;
					pt.y += dx*dirlen*0.25;
					buf.MoveTo(&pt);
					
					//箭头顶端
					pt.x = expt1.x+dx*(len*0.5+dirlen);
					pt.y = expt1.y+dy*(len*0.5+dirlen);
					pt.z = expt1.z+dz*(len*0.5+dirlen);
					buf.LineTo(&pt);
					
					//右箭头起点
					pt.x = expt1.x+dx*(len*0.5)+dy*dirlen*0.25;
					pt.y = expt1.y+dy*(len*0.5)-dx*dirlen*0.25;
					pt.z = expt1.z+dz*(len*0.5);
					buf.LineTo(&pt);
					
					slen = 0;
				}
				buf.End();
			}
		}
	}

	UpdateAllViews(NULL,hc_AddGraph,(CObject*)&buf);
}

void CDlgDoc::OnUpdateShowLayerDir(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowLayDir);
}

BOOL CDlgDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	SaveViewPreviewImage();

	UpdateFrameCounts();

	//当有其他视图时，首先关闭矢量视图的情况
	CFrameWnd *pFirstFrm = NULL;

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		CFrameWnd* pFrame2 = pView->GetParentFrame();

		if( pFrame2!=NULL && pFirstFrm==NULL )
		{
			pFirstFrm = pFrame2;
		}

		// assume frameless views are ok to close
		if( pFrame2!=NULL && pFrame2==pFrame && pFrame2==pFirstFrm )
		{
			BOOL bHaveOtherFrame = FALSE;
			pos = GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView = GetNextView(pos);
				CFrameWnd* pFrame2 = pView->GetParentFrame();
				if( pFrame2!=NULL && pFrame2!=pFrame )
				{
					bHaveOtherFrame = TRUE;
				}
			}

			if( bHaveOtherFrame )
			{
				BOOL bModified = FALSE;
				CDlgDataSource* pDS = GetDlgDataSource();
				if( pDS )
				{
					if(pDS->IsModified())
					{
						bModified = TRUE;
					}					
				}
				
				//判断当前是否正在关闭文档
				HWND hWnd = ::GetActiveWindow();
				HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
				BOOL bClosing = FALSE;
				if( hWnd==hWnd0 || IsChild(hWnd0,hWnd) )
					bClosing = TRUE;
				
				m_nPromptRet = IDNO;
				if( (bModified) && bClosing && !m_bPromptSave )
				{
					m_nPromptRet = AfxMessageBox(IDS_SAVETIP, MB_YESNOCANCEL);
					if( m_nPromptRet==IDCANCEL )return FALSE;

// 					if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 					{
// 						if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 						{
// 							OnSaveDEM();
// 						}
// 					}
				}
				else if( bClosing && !m_bPromptSave )
				{
					m_nPromptRet = AfxMessageBox(IDS_SAVETIP2, MB_YESNO);
					if( m_nPromptRet==IDNO )return FALSE;

// 					if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 					{
// 						if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 						{
// 							OnSaveDEM();
// 						}
// 					}
				}

				m_bPromptSave = TRUE;

				pos = GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = GetNextView(pos);
					CFrameWnd* pFrame2 = pView->GetParentFrame();
					if( pFrame2!=NULL && pFrame2!=pFrame )
					{
						pFrame2->SendMessage(WM_CLOSE);
					}
				}

				break;
				//pFrame->PostMessage(WM_CLOSE);			
				//return FALSE;
			}
		}
	}

	//当有其他视图时，首先关闭非矢量视图的情况
	pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		CFrameWnd* pFrame2 = pView->GetParentFrame();
		// assume frameless views are ok to close
		if (pFrame2 != NULL)
		{
			// assumes 1 document per frame
			if (pFrame2->m_nWindow > 0)
			{
				return TRUE;        // more than one frame refering to us
			}
		}
	}

	//当没有其他视图或者其他视图已经关闭时，再关闭矢量视图的情况
	BOOL bModified = FALSE;
	CDlgDataSource* pDS = GetDlgDataSource();				
	if( pDS )
	{
		if (pDS->m_bUVSLocalCopy)//uvs数据的本地副本
		{
			SaveBakFiles(BakTypeLatest);
			::CopyFile(m_strTmpFile, GetPathName(), FALSE);
			pDS->SetModifiedFlag(FALSE);
			HWND hWnd = ((CMainFrame*)AfxGetMainWnd())->m_wndProjectView.m_hWnd;
			::SendMessage(hWnd, WM_COMMAND, ID_COMMIT_LOCAL, 0);
		}
		if(pDS->IsModified())		
			bModified = TRUE;
	}
	
	//判断当前是否正在关闭文档
	HWND hWnd = ::GetActiveWindow();
	HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
	BOOL bClosing = FALSE;
	if( hWnd==hWnd0 || IsChild(hWnd0,hWnd) )
		bClosing = TRUE;

	if(bClosing && UVSModify == GetAccessType(pDS))//uvs
	{
		return TRUE;
	}
	
	if( !m_bPromptSave )
	{
		m_nPromptRet = IDNO;
		if( bModified && bClosing )
		{
			m_nPromptRet = AfxMessageBox(IDS_SAVETIP, MB_YESNOCANCEL);
			if( m_nPromptRet==IDCANCEL )return FALSE;

// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
			
			m_bPromptSave = TRUE;
		}
		else if( bClosing )
		{
			m_nPromptRet = AfxMessageBox(IDS_SAVETIP2, MB_YESNO);
			if( m_nPromptRet==IDNO )return FALSE;
			m_bPromptSave = TRUE;

// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
	}

	return SaveModified();
}
BOOL CheckStereoIsOpen(CString StereID,CView *pRetView,int nImageType =-1,BOOL bRealTimeEpipolar=FALSE);
BOOL CheckStereoIsOpen(CString StereID,CView *pRetView,int nImageType,BOOL bRealTimeEpipolar)
{
	if (StereID.IsEmpty())
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	CView *pView = NULL;
	POSITION DocTempPos=AfxGetApp()->GetFirstDocTemplatePosition();
	while(DocTempPos!=NULL)
	{
		CDocTemplate* curTemplate=AfxGetApp()->GetNextDocTemplate(DocTempPos);
		POSITION DocPos=curTemplate->GetFirstDocPosition();
		while(DocPos!=NULL)
		{
			CDocument *pDoc=curTemplate->GetNextDoc(DocPos);
			if (pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)))
			{
				POSITION ViewPos=pDoc->GetFirstViewPosition();
				while(ViewPos)
				{
					pView=pDoc->GetNextView(ViewPos);
					if(pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
					{
						CString strStereID = ((CStereoView*)pView)->GetStereID();

						if (StereID.Compare(strStereID) == 0)
						{	
							bRet = TRUE;
							pRetView = pView;
						
						}
					}		
				}
			}
		}		
	}	
	return bRet;
}

void CDlgDoc::OnLoadStereo()
{
	CWinApp *pApp = (CWinApp *)AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	
	BOOL bDoubleScreen	= pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, gdef_bDoubleScreen);
	
	CString pathName = _T("1.mdl");
	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			pApp->GetNextDocTemplate(curTemplatePos);
		CDocument *rpDocMatch;
		
		if( curTemplate->MatchDocType( pathName, rpDocMatch )==CDocTemplate::yesAttemptNative )
		{
			if( bDoubleScreen && !curTemplate->IsKindOf(RUNTIME_CLASS(CSingleDocTemplate)) )
			{
				continue;
			}
			
			if( !bDoubleScreen && curTemplate->IsKindOf(RUNTIME_CLASS(CSingleDocTemplate)) )
			{
				continue;
			}
			CoreObject *pCore = (CoreObject*)gdwInnerCmdData[1];
			int stereidx = gdwInnerCmdData[0]&0x0fffffff;
			if (stereidx >= pCore->iStereoNum) break;
			CString strStereID = pCore->stereo[stereidx].sp.stereoID;
			CView *pView1 = NULL;
			if( gdwInnerCmdData[0]&0x80000000 )//原始相对
			{
				if(CheckStereoIsOpen(strStereID,pView1,SCANNER))
				{
					AfxMessageBox(StrFromResID(IDS_STERRO_OPENED));
					return;
				}
			}
			else if ((gdwInnerCmdData[0]&0x40000000)!=0)
			{
				if(CheckStereoIsOpen(strStereID,pView1,EPIPOLAR,TRUE))
				{
					AfxMessageBox(StrFromResID(IDS_STERRO_OPENED));
					return;
				}
			}
			else
			{
				if(CheckStereoIsOpen(strStereID,pView1,EPIPOLAR))
				{
					AfxMessageBox(StrFromResID(IDS_STERRO_OPENED));
					return;
				}
			}
			
			CFrameWnd* pFrame = curTemplate->CreateNewFrame(this,NULL);
			
			POSITION pos = GetFirstViewPosition();
			CView *pView;
			while( pos )
			{
				pView = GetNextView(pos);
				if( pView->GetParentFrame()==pFrame )
				{
					SendMessage(pView->m_hWnd,WM_STEREOVIEW_LOADMDL,
					gdwInnerCmdData[0],gdwInnerCmdData[1]);
				}
			}
			
			curTemplate->InitialUpdateFrame(pFrame, this, TRUE);
			break;
		}	
	}

	/*
	// 纠正层列表被UIF序列化机制修改的问题
	AfxGetMainWnd()->SendMessage (FCCM_INITFTRCLASS,WPARAM(0),LPARAM(&(m_pDataSource->m_UserIdx)));
	AfxGetMainWnd()->SendMessage(FCCM_SETVIEWTYPE,WPARAM(0),LPARAM(0));*/
}

void CDlgDoc::OnDummySelectRecentCmd()
{
	CMFCToolBarComboBoxButton* pSrcCombo = NULL;
	
	pSrcCombo = GetCommandButton(ID_DUMMY_VIEW_RECENTCMDS);
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);
		int id = pSrcCombo->GetItemData();
//		LPCTSTR lpszSelItem = pSrcCombo->GetItem ();
//		CString strSelItem = _T("");
	
// 		if( lpszSelItem )
// 		{			
// 			sscanf(lpszSelItem,"%s",strSelItem.GetBuffer(strlen(lpszSelItem)+1));
// 			strSelItem.ReleaseBuffer();
// 		}
//		CRecentCmd::CmdItem item = m_RecentCmd.FindItem(lpszSelItem/*LPCTSTR(strSelItem)*/);
		if (id!=0)
		{
			AfxGetMainWnd()->SendMessage(WM_COMMAND,MAKEWPARAM(id,0),0);
		}
// 		if (AfxGetMainWnd->SendMessage() < 0)
// 			AfxMessageBox(IDS_DOC_NOTFOUND_FID);
	}
	else
	{
		AfxMessageBox(IDS_RESOURCE_ERR);
	}
}

void CDlgDoc::OnUpdateDummySelectRecentCmd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

extern int GeoTypeFrom(int src);
void CDlgDoc::OnFileOpeneps()
{
	CDlgImportEps dlg;
	if (dlg.DoModal() != IDOK)return;
	if (dlg.m_strPath.IsEmpty())
	{
		return;
	}

	CString mapName = dlg.m_strMapName;
	if (mapName.CompareNoCase("*") == 0)
	{
		CString fileTitle = dlg.m_strPath;
		int pos = fileTitle.ReverseFind('\\');
		if (pos >= 0)fileTitle = fileTitle.Mid(pos + 1);

		mapName = fileTitle;
	}

#ifdef _WIN64

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	AfxGetApp()->WriteProfileInt("IO32\\ImportEPS_Settings", "nScale", pDS->GetScale());

	CString epsPath = dlg.m_strPath;

	CString tmpfdbPath = GetPathName();
	tmpfdbPath = tmpfdbPath.Left(tmpfdbPath.GetLength() - 4);
	tmpfdbPath += "_eps.fdb";

	GOutPut(StrFromResID(IDS_PROCESSING));
	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos1;
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	strcat(path, "\\bin\\VectorConvert.exe");

	CString cmd;
	cmd.Format("\"%s\" importEPS \"%s\" \"%s\"", path, epsPath, tmpfdbPath);

	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	//stStartUpInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));
	if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo))
		return;
	while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
	{
	}
	GOutPut(StrFromResID(IDS_PROCESS_END));

	//导入tmpfdbPath
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme)  return;

	CAttributesSource *pDSX = pDS->GetXAttributesSource();

	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();

	CSQLiteAccess fdb;

	GOutPut(StrFromResID(IDS_LOAD_FEATURES));

	CUndoFtrs undo(this,"Import Eps");

	{
		CString fileName = tmpfdbPath;

		BOOL bOpen = fdb.Attach(fileName);
		if( !bOpen ) return;	

		// 加载
		fdb.BatchUpdateBegin();
		BeginBatchUpdate();	

		long idx;
		int lSum = fdb.GetFtrLayerCount();
		lSum += fdb.GetFeatureCount();

		GProgressStart(lSum);	

		CUIntArray layid;
		CMap<int, int&, int, int&> idpair;

		CFtrLayer *pLayer = fdb.GetFirstFtrLayer(idx);
		while( pLayer )
		{
			GProgressStep();

			int pid = pLayer->GetID(), cid;
			pLayer->SetID(0);
			pLayer->SetInherentFlag(FALSE);

			CFtrLayer *pExistLayer = pDS->GetFtrLayer(pLayer->GetName(),pLayer->GetMapName());
			if (pExistLayer == NULL)
			{
				if( !AddFtrLayer(pLayer) )
				{
					delete pLayer;
					pLayer = fdb.GetNextFtrLayer(idx);
					continue;
				}
				else
				{
					cid = pLayer->GetID();
					layid.Add(pLayer->GetID());

					if( pDS->GetCurFtrLayer()==NULL )
						pDS->SetCurFtrLayer(pLayer->GetID());
				}

			}
			else
				cid = pExistLayer->GetID();
			idpair.SetAt(pid,cid);

			pLayer = fdb.GetNextFtrLayer(idx);	

		}

		int id;
		CFeature *pFt = fdb.GetFirstFeature(idx);

		while( pFt )
		{
			GProgressStep();

			pFt->SetCode(mapName);

			int pid = fdb.GetCurFtrLayID();
			if (idpair.Lookup(pid,id))
			{
				pDS->SetCurFtrLayer(id);
			}

			// 导入地物的扩展属性
			BOOL bSucceed = FALSE;
			CValueTable tab;
			tab.BeginAddValueItem();
			bSucceed = fdb.ReadXAttribute(pFt, tab);
			tab.EndAddValueItem();

			//ID为空时，会写入缺省扩展属性
			pFt->SetID(OUID());

			if (pDS->IsFeatureValid(pFt) && pDS->AddObject(pFt))
			{
				// 存在扩展属性才导入
				if (bSucceed && tab.GetFieldCount() > 0)
				{
					pDSX->SetXAttributes(pFt, tab);
				}

			}
			else
				delete pFt;

			undo.arrNewHandles.Add(FTR_HANDLE(pFt));

			pFt = fdb.GetNextFeature(idx);

		}

		GProgressEnd();

		fdb.BatchUpdateEnd();
		fdb.Close();

		// 加载的空层删掉
		for (int i = 0; i < layid.GetSize(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayer(layid.GetAt(i));
			if (!pLayer) 	continue;

			if (pLayer->GetObjectCount() == 0)
			{
				pDS->DelFtrLayer(layid.GetAt(i));
			}
		}

		EndBatchUpdate();
	}

	undo.Commit();

	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}

	GOutPut(StrFromResID(IDS_LOAD_FINISH));

	UpdateAllViews(NULL, hc_UpdateAllObjects);

	::DeleteFile(tmpfdbPath);

#else

	CString strMapName = dlg.m_strMapName;
	CDlgDataSource *pDS = GetDlgDataSource();
	CEpsAccess db;
	db.SetTemplatePath(dlg.m_strRefTmp);
	if (!db.OpenRead(dlg.m_strPath) || !db.InitImport())
	{
		db.CloseRead();
		return;
	}
	
	LCodeListManager codeMap;
	codeMap.LoadFile(dlg.m_strLstFile);
	codeMap.SetDataSourse(&db,pDS);

	BeginBatchUpdate();
	CFtrLayer *pLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
	CArray<CFtrLayer*,CFtrLayer*> arrAddLayer;
	long idx;
	CString layerName;
	CUIntArray layid;
	CMap<long, long&, int, int&> idpair;//存储外来层的层码到层ID的映射
	idpair.InitHashTable(3601);
	db.OpenSmallLayerTable();

	{	
		int pid;
		idx = db.GetFirstFtrLayerCode();
		while( idx!=-1 )
		{	
			layerName = codeMap.ExternCodeToLocLayName(idx);
			if (layerName.IsEmpty())
			{
				int cls = GeoTypeFrom(db.GetCurType());
				switch(cls)
				{
				case CLS_GEOPOINT:			
					pid = pDS->GetDefaultPntFtrLayer()->GetID();
					break;
				case CLS_GEOCURVE:			
					pid = pDS->GetDefaultLineFtrLayer()->GetID();
					break;
				case CLS_GEOSURFACE:				
					pid = pDS->GetDefaultPolygonFtrLayer()->GetID();
					break;
				case CLS_GEOTEXT:
					pid = pDS->GetDefaultTxtFtrLayer()->GetID();
					break;
				default:
					pid = pDS->GetDefaultLineFtrLayer()->GetID();
					break;
				}
				idpair.SetAt(idx,pid);
				idx = db.GetNextFtrLayerCode();
				continue;
			}
			else
			{
				CFtrLayer *pFtrLayer = pDS->GetLocalFtrLayer(layerName);
				if (pFtrLayer == NULL)
				{				
					pFtrLayer = pDS->CreateFtrLayer(layerName);
					if (!pFtrLayer || !pDS->AddFtrLayer(pFtrLayer)) 
						continue;			
				}

				pid = pFtrLayer->GetID();
				idpair.SetAt(idx,pid);
				
				idx = db.GetNextFtrLayerCode();
			}
		}
	}
	db.CloseSmallLayerTable();
	int lSum = db.GetFeatureCount();
	GOutPut(StrFromResID(IDS_LOAD_FEATURES));
	GProgressStart(lSum);	
	const char* name = NULL;
	CValueTable tab;
	CValueTable *pTab = NULL;
	if (1)
	{
		pTab = &tab;
	}
	CFeature *pFt = db.GetFirstFeature(idx,pTab);
	while( pFt )
	{
		if (pFt->IsDeleted())
		{
			delete pFt;
			if(pTab)pTab->DelAll();
			pFt = db.GetNextFeature(idx,pTab);
			continue;
		}
		GProgressStep();
		int pid = -1;
		long code = db.GetCurCode();
		if (code==-1)
		{
			pid = pDS->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET))->GetID();
		}
		else if(!idpair.Lookup(code,pid))
		{
		}

		if( pid!=-1 && pDS->GetFtrLayer(pid)!=NULL )
			db.m_nParaOutMode = codeMap.GetParaOutMode(pDS->GetFtrLayer(pid)->GetName());

		pFt->SetCode(strMapName);

		if ( pDS->IsFeatureValid(pFt) && AddObject(pFt,pid))
		{
			if(pTab)
				pDS->GetXAttributesSource()->SetXAttributes(pFt,*pTab);
		}
		else
			delete pFt;	
		if(pTab)pTab->DelAll();
		pFt = db.GetNextFeature(idx,pTab);		
	}	
	if(pTab)pTab->DelAll();
	GProgressEnd();
	for (int i=arrAddLayer.GetSize()-1;i>=0;i--)
	{
		if (arrAddLayer[i]->IsEmpty())
		{
			DelFtrLayer(arrAddLayer[i]);
		}
	}
	arrAddLayer.RemoveAll();
	GOutPut(StrFromResID(IDS_LOAD_FINISH));
//	pDS->SaveAll(NULL);
	EndBatchUpdate();
	db.CloseRead();
	if (pLayer)
	{
		pDS->SetCurFtrLayer(pLayer->GetID());
	}
#endif
}
void CDlgDoc::OnExporteps()
{
	CDlgExportEps dlg;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

#ifdef _WIN64

	CString fdbPath = GetPathName();
	fdbPath = fdbPath.Left(fdbPath.GetLength() - 4);
	fdbPath += "_eps.fdb";
	SaveAs(fdbPath, TRUE);

	CString epsPath = dlg.m_strFilePath;
	if (PathFileExists(epsPath))
		DeleteFile(epsPath);

	GOutPut(StrFromResID(IDS_PROCESSING));
	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos1;
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	strcat(path, "\\bin\\VectorConvert.exe");

	CString cmd;
	cmd.Format("\"%s\" exportEPS \"%s\" \"%s\"", path, fdbPath, epsPath);

	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	//stStartUpInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));
	if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo))
		return;
	while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
	{
	}
	GOutPut(StrFromResID(IDS_PROCESS_END));

	::DeleteFile(fdbPath);

#else
	int nSum = 0;
	CDlgDataSource* pDS = GetDlgDataSource();
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer)continue;
		if( !pLayer->IsVisible() )continue;
		nSum += pLayer->GetObjectCount();
	}
	CString path = dlg.m_strFilePath;
	if(PathFileExists(path))
		DeleteFile(path);
	CValueTable tab;
	CEpsAccess epsDb;
	epsDb.SetDataSource(pDS);
	if (dlg.m_strTemplateFile.IsEmpty())
	{
	if(!epsDb.CreateFile(path)) return;
	}
	else
		if(!epsDb.CreateFile(path,dlg.m_strTemplateFile)) return;

	if (!epsDb.OpenWrite(path) || !epsDb.InitExport())
	{
		epsDb.CloseWrite();
		return;
	}
	
	LCodeListManager codeMap;
	codeMap.LoadFile(dlg.m_strCodeList);
	codeMap.SetDataSourse(&epsDb,pDS);	
		
	GProgressStart(nSum);
	epsDb.WriteDataSourceInfo(pDS);
	int nLay = pDS->GetFtrLayerCount();
	CFtrLayer *pLayer = NULL;
	CFeature* pFtr = NULL;
	epsDb.OpenSmallLayerTable();

	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	if (!pCfgLibManager)  return;
	CScheme *pScheme = pCfgLibManager->GetScheme(pDS->GetScale());
	if (!pScheme)  return;		

	for (i=0;i<nLay;i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer||pLayer->GetObjectCount()<=0)continue;
		if( !pLayer->IsVisible() )continue;

		LPCTSTR  name = /*pDS->GetLayerCodeOfFtrLayer(pLayer)*/pLayer->GetName();
		epsDb.m_strSchemeLayerName = name;
		if (StrFromResID(IDS_DEFLAYER_NAMET).CompareNoCase(name)==0)
		{
			epsDb.m_nTmpType = -1;
		}
		else
		{
			epsDb.m_lTmpCode = codeMap.LocLayNameToExternCode(name);

			if (epsDb.m_lTmpCode == -1)
			{
				CSchemeLayerDefine *pLayerDefine = pScheme->GetLayerDefine(name);
				if (pLayerDefine)
				{
					epsDb.m_lTmpCode = pLayerDefine->GetLayerCode();
				}
			}

			if (epsDb.m_lTmpCode!=-1)
			{
				epsDb.UpdateLayerName();
			}	
		}	

		epsDb.m_nParaOutMode = codeMap.GetParaOutMode(name);
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{ 
			GProgressStep();
			pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())continue;
			tab.DelAll();
			tab.BeginAddValueItem();
			pXDS->GetXAttributes(pFtr,tab);
			tab.EndAddValueItem();
			epsDb.SaveFeature(pFtr,&tab);
		}
	}
	GProgressEnd();
	epsDb.CloseSmallLayerTable();
	epsDb.CloseWrite();
#endif
}
void CDlgDoc::OnFileOpenE00()
{
	CDlgImportE00 dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	CStringArray strFiles;
	strFiles.Copy(dlg.m_arrFileNames);

	CListFile lstFile;
	if( !dlg.m_strLstFile.IsEmpty() )
		lstFile.Open(dlg.m_strLstFile);

	CDlgDataSource *pDS = GetDlgDataSource();

	DWORD nScale = pDS->GetScale();
	CValueTable tab;
	_variant_t var;
	CAttributesSource *pXDs = GetDlgDataSource()->GetXAttributesSource();
	FILE *archivo;
	int precision;
	char section[128], e00fname[80], linea[128];
	strArcInfo ArcInfo;

	BeginBatchUpdate();

	for (int i=0;i<strFiles.GetSize();i++)
	{
		CString strPathName = strFiles[i];
		if (strPathName.IsEmpty())continue;

		if (!(archivo = openE00File (LPCTSTR(strPathName), 0)))
			return;

		char strFileName[MAX_PATH] = {0};
		
		_splitpath((LPCTSTR)strPathName,NULL,NULL,strFileName,NULL);
		
		CString layname = lstFile.FindMatchItem(strFileName, TRUE, FALSE);
		if( layname )
		{
			__int64 code = _atoi64(layname);
			if( code!=0 )
			{
				pDS->FindLayerIdx(TRUE,code,layname);
			}
		}
		else
		{
			layname = strFileName;
		}

		if( !layname.IsEmpty() && layname!=strFileName )
			strncpy(strFileName,layname,sizeof(strFileName)-1);

		ArcInfo.arcs = NULL; ArcInfo.arcsCount = 0;
		ArcInfo.points = NULL; ArcInfo.pointsCount = 0;
		ArcInfo.polygons = NULL; ArcInfo.polygonsCount = 0;
		ArcInfo.texts = NULL; ArcInfo.textsCount = 0;
		shapePolygonObject polygon;
		shapeArcObject arco;
		int shape, arc, j, node, nparts, *starts;
		double *x, *y;
		fgets (linea, 128, archivo);
		while (!feof (archivo))
		{
			fgets (linea, 128, archivo);
			if (!strncmp (linea, "EOS", 3))
				break;		
			sscanf (linea, "%3s  %i", section, &precision);
			if (!strcmp (section, "ARC")) readARCSection (archivo, &ArcInfo, precision);
			else if (!strcmp (section, "CNT")) readCNTSection (archivo, &ArcInfo);
			else if (!strcmp (section, "LAB")) readLABSection (archivo, &ArcInfo, precision);
			else if (!strcmp (section, "LOG")) readLOGSection (archivo, &ArcInfo);
			else if (!strcmp (section, "PAL")) readPALSection (archivo, &ArcInfo,precision);
			else if (!strcmp (section, "PRJ")) readPRJSection (archivo, &ArcInfo);
			else if (!strcmp (section, "SIN")) readSINSection (archivo, &ArcInfo);
			else if (!strcmp (section, "TOL")) readTOLSection (archivo, &ArcInfo);
			else if (!strcmp (section, "TXT")) readTXTSection (archivo, &ArcInfo,precision);
			else if (!strcmp (section, "TX6")) readTX6Section (archivo, &ArcInfo,precision);
			else if (!strcmp (section, "TX7")) readTX7Section (archivo, &ArcInfo,precision);
			else if (!strcmp (section, "RXP")) readRXPSection (archivo, &ArcInfo);
			else if (!strcmp (section, "RPL")) readRPLSection (archivo, &ArcInfo);
			else if (!strcmp (section, "IFO"))
			{
				if (ArcInfo.polygons) 
				{
					CFtrLayer *pLayer = GetDlgDataSource()->GetFtrLayer(strFileName);
					if (!pLayer)
					{
						pLayer = new CFtrLayer;
						if(!pLayer) break;
						pLayer->SetName(strFileName);
						if(!AddFtrLayer(pLayer))
						{
							delete pLayer;
							break;
						}
					}
					vector<int> widths;
					E00FieldInfoArray arry;
					readFieldsInfo(archivo,arry,widths,precision,2);
					GProgressStart(ArcInfo.polygonsCount);
					CFeature* pFtr = NULL;
					for (int shape = 0; shape < ArcInfo.polygonsCount; shape++)
					{
						GProgressStep();
						x = NULL; y = NULL; nparts = 0; starts = NULL;
						polygon = ArcInfo.polygons[shape];
						for (arc = 0, node = 0; arc < polygon.elements; arc++)
						{
							if (polygon.arcs[arc] == 0)
							{
								starts = (int *)realloc(starts, (nparts + 1) * sizeof (int));
								starts[nparts] = node;
								nparts++;
							} 
							else if (polygon.arcs[arc] > 0 ) 
							{
								arco = findARC(&ArcInfo,polygon.arcs[arc]);
								x = (double*)realloc (x, (node + arco.elements) * sizeof (double));
								y = (double*)realloc (y, (node + arco.elements) * sizeof (double));
								for (j = 0; j < arco.elements; j++, node++)
								{
									x[node] = arco.x[j]; y[node] = arco.y[j];
								}
							}
							else 
							{
								arco = ArcInfo.arcs[- 1 - polygon.arcs[arc]];
								x = (double*)realloc (x, (node + arco.elements) * sizeof (double));
								y = (double*)realloc (y, (node + arco.elements) * sizeof (double));
								for (j = arco.elements; j > 0; j--, node++)
								{
									x[node] = arco.x[j - 1]; y[node] = arco.y[j - 1];
								}
							}
						}
						pFtr = pLayer->CreateDefaultFeature(nScale,CLS_GEOSURFACE);
						if(pFtr)
						{	
							CArray<PT_3DEX,PT_3DEX> arrPts;
							arrPts.SetSize(node);
							PT_3DEX* p = arrPts.GetData();
							for (int i=0;i<node;i++)
							{
								p[i].x = x[i];p[i].y = y[i];p[i].z = 0;
								p[i].pencode = penLine;
							}
							if(pFtr->GetGeometry()->CreateShape(p,node))
							{
								if(AddObject(pFtr,pLayer->GetID()))
								{
									;
								}
								else
								{
									delete pFtr;
									pFtr = NULL;							
								}
							}
							else
							{
								delete pFtr;
								pFtr = NULL;											
							}
						}					
						if(x)free(x);
						if(y)free(y);
						if (pFtr)
						{
							int ivalue;
							double lfvalue;
							string str;						
							tab.BeginAddValueItem();
							for (int i=0;i<arry.size();i++)
							{
								int type = arry[i].fieldType;
								if (type==E00Integer)
								{
									ivalue = readFieldValueAsInteger(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(long)(ivalue)));
								}
								else if (type==E00Double)
								{
									lfvalue = readFieldValueAsDouble(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(lfvalue)));
								}
								else
								{
									str = readFieldValueAsString(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(_bstr_t)(str.c_str())));
								}
							}
							fgets (linea, 128, archivo);
							tab.EndAddValueItem();						
							pXDs->SetXAttributes(pFtr,tab);
							tab.DelAll();
						}
					}
					GProgressEnd();
				}
				else if (ArcInfo.arcs)
				{
					CFtrLayer *pLayer = GetDlgDataSource()->GetFtrLayer(strFileName);
					if (!pLayer)
					{
						pLayer = new CFtrLayer;
						if(!pLayer) break;
						pLayer->SetName(strFileName);
						if(!AddFtrLayer(pLayer))
						{
							delete pLayer;
							break;
						}
					}
					GProgressStart(ArcInfo.arcsCount);
					CFeature* pFtr = NULL;
					for (int shape = 0; shape < ArcInfo.arcsCount; shape++)
					{	
						GProgressStep();
						pFtr = pLayer->CreateDefaultFeature(nScale,CLS_GEOCURVE);
						if(pFtr)
						{	
							node = ArcInfo.arcs[shape].elements;
							CArray<PT_3DEX,PT_3DEX> arrPts;
							arrPts.SetSize(node);
							PT_3DEX* p = arrPts.GetData();
							for (int i=0;i<node;i++)
							{
								p[i].x = ArcInfo.arcs[shape].x[i];p[i].y = ArcInfo.arcs[shape].y[i];p[i].z = 0;
								p[i].pencode = penLine;
							}
							if(pFtr->GetGeometry()->CreateShape(p,node))
							{
								if(AddObject(pFtr,pLayer->GetID()))
								{
									;
								}
								else
								{
									delete pFtr;
									pFtr = NULL;							
								}
							}
							else
							{
								delete pFtr;
								pFtr = NULL;											
							}
						}
						if (pFtr)
						{
							int ivalue;
							double lfvalue;
							string str;
							vector<int> widths;
							E00FieldInfoArray arry;
							readFieldsInfo(archivo,arry,widths,precision,1);
							tab.BeginAddValueItem();
							for (int i=0;i<arry.size();i++)
							{
								if (i==6)
								{
									int fdf=77;
								}
								int type = arry[i].fieldType;
								if (type==E00Integer)
								{
									ivalue = readFieldValueAsInteger(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(long)(ivalue)));
								}
								else if (type==E00Double)
								{
									lfvalue = readFieldValueAsDouble(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(lfvalue)));
								}
								else
								{
									str = readFieldValueAsString(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(_bstr_t)(str.c_str())));
								}
							}
							fgets (linea, 128, archivo);
							tab.EndAddValueItem();						
							pXDs->SetXAttributes(pFtr,tab);
							tab.DelAll();
						}
					}
					GProgressEnd();
				}
				else if (ArcInfo.points) 
				{
					CFtrLayer *pLayer = GetDlgDataSource()->GetFtrLayer(strFileName);
					if (!pLayer)
					{
						pLayer = new CFtrLayer;
						if(!pLayer) break;
						pLayer->SetName(strFileName);
						if(!AddFtrLayer(pLayer))
						{
							delete pLayer;
							break;
						}
					}
					GProgressStart(ArcInfo.pointsCount);
					CFeature* pFtr = NULL;
					for (int shape = 0; shape < ArcInfo.pointsCount; shape++)
					{	
						GProgressStep();
						pFtr = pLayer->CreateDefaultFeature(nScale,CLS_GEOPOINT);
						if(pFtr)
						{	
							PT_3DEX pt;
							pt.x = ArcInfo.points[shape].x;
							pt.y = ArcInfo.points[shape].y;
							pt.z = 0;
							pt.pencode = penLine;						
							if(pFtr->GetGeometry()->CreateShape(&pt,1))
							{
								if(AddObject(pFtr,pLayer->GetID()))
								{
									;
								}
								else
								{
									delete pFtr;
									pFtr = NULL;							
								}
							}
							else
							{
								delete pFtr;
								pFtr = NULL;											
							}
						}
						if (pFtr)
						{
							int ivalue;
							double lfvalue;
							string str;
							vector<int> widths;
							E00FieldInfoArray arry;
							readFieldsInfo(archivo,arry,widths,precision,0);
							tab.BeginAddValueItem();
							for (int i=0;i<arry.size();i++)
							{
								int type = arry[i].fieldType;
								if (type==E00Integer)
								{
									ivalue = readFieldValueAsInteger(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(long)(ivalue)));
								}
								else if (type==E00Double)
								{
									lfvalue = readFieldValueAsDouble(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(lfvalue)));
								}
								else
								{
									str = readFieldValueAsString(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(_bstr_t)(str.c_str())));
								}
							}
							fgets (linea, 128, archivo);
							tab.EndAddValueItem();						
							pXDs->SetXAttributes(pFtr,tab);
							tab.DelAll();
						}
					}
					GProgressEnd();
				}
				else if (ArcInfo.texts) 
				{
					CFtrLayer *pLayer = GetDlgDataSource()->GetFtrLayer(strFileName);
					if (!pLayer)
					{
						pLayer = new CFtrLayer;
						if(!pLayer) break;
						pLayer->SetName(strFileName);
						if(!AddFtrLayer(pLayer))
						{
							delete pLayer;
							break;
						}
					}
					GProgressStart(ArcInfo.textsCount);
					CFeature* pFtr = NULL;
					for (int shape = 0; shape < ArcInfo.textsCount; shape++)
					{	
						GProgressStep();
						pFtr = pLayer->CreateDefaultFeature(nScale,CLS_GEOTEXT);
						if(pFtr)
						{						
							PT_3DEX pt;
							pt.x = ArcInfo.texts[shape].x;
							pt.y = ArcInfo.texts[shape].y;
							pt.z = 0;
							pt.pencode = penLine;						
							if(pFtr->GetGeometry()->CreateShape(&pt,1))
							{
								TEXT_SETTINGS0 settings;
								settings.fHeight = ArcInfo.texts[shape].height;						
								((CGeoText*)(pFtr->GetGeometry()))->SetSettings(&settings);
								((CGeoText*)(pFtr->GetGeometry()))->SetText(ArcInfo.texts[shape].text);
								if(AddObject(pFtr,pLayer->GetID()))
								{
									;
								}
								else
								{
									delete pFtr;
									pFtr = NULL;							
								}
							}
							else
							{
								delete pFtr;
								pFtr = NULL;											
							}
						}
						if (pFtr)
						{
							int ivalue;
							double lfvalue;
							string str;
							vector<int> widths;
							E00FieldInfoArray arry;
							readFieldsInfo(archivo,arry,widths,precision,0);
							tab.BeginAddValueItem();
							for (int i=0;i<arry.size();i++)
							{
								int type = arry[i].fieldType;
								if (type==E00Integer)
								{
									ivalue = readFieldValueAsInteger(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(long)(ivalue)));
								}
								else if (type==E00Double)
								{
									lfvalue = readFieldValueAsDouble(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(lfvalue)));
								}
								else
								{
									str = readFieldValueAsString(archivo,widths[i]);
									tab.AddValue(arry[i].fieldName,&((CVariantEx)(_variant_t)(_bstr_t)(str.c_str())));
								}
							}
							fgets (linea, 128, archivo);
							tab.EndAddValueItem();						
							pXDs->SetXAttributes(pFtr,tab);
							tab.DelAll();
						}
					}
					GProgressEnd();
				}			
			}
		}	
		fclose (archivo);
		if (ArcInfo.points)
		{
			free(ArcInfo.points);
		}
		if(ArcInfo.arcs)
		{
			for (int i=0;i<ArcInfo.arcsCount;i++)
			{
				if(ArcInfo.arcs[i].x)free(ArcInfo.arcs[i].x);
				if(ArcInfo.arcs[i].y)free(ArcInfo.arcs[i].y);
			}
			free(ArcInfo.arcs);
		}
		if (ArcInfo.polygons)
		{
			for (int i=0;i<ArcInfo.polygonsCount;i++)
			{
				if(ArcInfo.polygons[i].arcs)free(ArcInfo.polygons[i].arcs);
			}
			free(ArcInfo.polygons);
		}
		if (ArcInfo.texts)
		{
			free(ArcInfo.texts);
		}	
	}

	EndBatchUpdate();
}
extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格
void CDlgDoc::OnExportE00()
{
	CDlgExportE00 dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	CString szPath = dlg.m_strPath;

	CListFile lstFile;
	if( !dlg.m_strLstFile.IsEmpty() )
	{
		lstFile.Open(dlg.m_strLstFile);
	}

	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());

	strArcInfo arcinfo;	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int nSum = 0;
	CDlgDataSource* pDS = GetDlgDataSource();
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer)continue;		
		if( !pLayer->IsVisible() )continue;
		nSum += pLayer->GetObjectCount();
	}

	int precision = 3;

	GProgressStart(nSum);
	for (i=0;i<pDS->GetFtrLayerCount();i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer)continue;		
		if( !pLayer->IsVisible() )continue;
		nSum = pLayer->GetObjectCount();
		if (nSum<=0)continue;	
		arcinfo.arcs = NULL; arcinfo.arcsCount = 0;	
		arcinfo.points = NULL; arcinfo.pointsCount = 0;
		arcinfo.polygons = NULL; arcinfo.polygonsCount = 0;
		arcinfo.texts = NULL; arcinfo.textsCount = 0;	
		int arcID=0,pointID=0,polygonID=0,textID=0;
		for (int j=0;j<pLayer->GetObjectCount();j++)
		{
			GProgressStep();
			CFeature* pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;

			CFeature *pFtr0 = pFtr;
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;
			
			//双线打散再导出
			CPFeature ftrs[2] = {pFtr,NULL};
			int nftr = 1;
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
			{
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
				{
					CGeometry *pGeo1 = NULL, *pGeo2 = NULL;
					((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						ftrs[0] = pFtr0->Clone();
						ftrs[1] = pFtr0->Clone();
						
						ftrs[0]->SetGeometry(pGeo1);
						ftrs[1]->SetGeometry(pGeo2);
						nftr = 2;
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
				else
				{
					CGeometry *pGeo1 = NULL, *pGeo2 = NULL;
					((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						ftrs[0] = pFtr0->Clone();
						ftrs[1] = pFtr0->Clone();
						
						ftrs[0]->SetGeometry(pGeo1);
						ftrs[1]->SetGeometry(pGeo2);
						nftr = 2;
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
			}

			for( int m=0; m<nftr; m++)
			{
				pFtr = ftrs[m];
				if (pFtr )
				{
					CGeometry *pObj = pFtr->GetGeometry();
					if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
					{
						arcinfo.points = (shapePointObject *)realloc (arcinfo.points, (arcinfo.pointsCount + 1) * sizeof (shapePointObject));
						arcinfo.points[arcinfo.pointsCount].id= ++pointID;
						PT_3DEX  pt = pObj->GetDataPoint(0);
						arcinfo.points[arcinfo.pointsCount].x = pt.x;
						arcinfo.points[arcinfo.pointsCount].y = pt.y;					
						arcinfo.pointsCount++;
					}
					else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					{					
						pObj->GetShape(arrPts);
						arcinfo.arcs = (shapeArcObject*)realloc (arcinfo.arcs, (arcinfo.arcsCount + 1) * sizeof (shapeArcObject));
						arcinfo.arcs[arcinfo.arcsCount].id = ++arcID;
						arcinfo.arcs[arcinfo.arcsCount].elements = arrPts.GetSize();
						arcinfo.arcs[arcinfo.arcsCount].x = (double*)malloc (arrPts.GetSize() * sizeof (double));
						arcinfo.arcs[arcinfo.arcsCount].y = (double*)malloc (arrPts.GetSize() * sizeof (double));
						arcinfo.arcs[arcinfo.arcsCount].isPartofPolygon = false;
						for (int k=0;k<arrPts.GetSize();k++)
						{
							arcinfo.arcs[arcinfo.arcsCount].x[k] = arrPts[k].x;
							arcinfo.arcs[arcinfo.arcsCount].y[k] = arrPts[k].y;
						}
						arcinfo.arcsCount++;
					}
					else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{
						pObj->GetShape(arrPts);
						arrPts.Add(arrPts[0]);
						Envelope e = pObj->GetEnvelope();
						arcinfo.arcs = (shapeArcObject*)realloc (arcinfo.arcs, (arcinfo.arcsCount + 1) * sizeof (shapeArcObject));
						arcinfo.arcs[arcinfo.arcsCount].id = ++arcID;
						arcinfo.arcs[arcinfo.arcsCount].elements = arrPts.GetSize();
						arcinfo.arcs[arcinfo.arcsCount].x = (double*)malloc (arrPts.GetSize() * sizeof (double));
						arcinfo.arcs[arcinfo.arcsCount].y = (double*)malloc (arrPts.GetSize() * sizeof (double));
						arcinfo.arcs[arcinfo.arcsCount].isPartofPolygon = true;
						for (int k=0;k<arrPts.GetSize();k++)
						{
							arcinfo.arcs[arcinfo.arcsCount].x[k] = arrPts[k].x;
							arcinfo.arcs[arcinfo.arcsCount].y[k] = arrPts[k].y;
						}
						arcinfo.arcsCount++;
						arcinfo.polygons = (shapePolygonObject*)realloc (arcinfo.polygons, (arcinfo.polygonsCount + 1) * sizeof (shapePolygonObject));
						arcinfo.polygons[arcinfo.polygonsCount].elements = 1;
						arcinfo.polygons[arcinfo.polygonsCount].arcs =(int*) malloc (1 * sizeof (int));
						arcinfo.polygons[arcinfo.polygonsCount].arcs[0] = arcID;
						arcinfo.polygons[arcinfo.polygonsCount].xmin = e.m_xl;
						arcinfo.polygons[arcinfo.polygonsCount].xmax = e.m_xh;
						arcinfo.polygons[arcinfo.polygonsCount].ymin = e.m_yl;
						arcinfo.polygons[arcinfo.polygonsCount].ymax = e.m_yh;
						arcinfo.polygonsCount++;
					}
					else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))
					{
						PT_3DEX pt = pObj->GetDataPoint(0);
						TEXT_SETTINGS0 settings;
						((CGeoText*)pObj)->GetSettings(&settings);
						arcinfo.texts = (shapeTextObject*)realloc (arcinfo.texts, (arcinfo.textsCount + 1) * sizeof (shapeTextObject));
						arcinfo.texts[arcinfo.textsCount].height = settings.fHeight;
						strcpy(arcinfo.texts[arcinfo.textsCount].text ,((CGeoText*)pObj)->GetText());
						arcinfo.texts[arcinfo.textsCount].x = pt.x;
						arcinfo.texts[arcinfo.textsCount].y = pt.y;
						arcinfo.textsCount++;
					}
				}
			}
			
			if( nftr==2 )
			{
				delete ftrs[0];
				delete ftrs[1];
			}
		}
		
		CString layname = pLayer->GetName();
		__int64 code = 0;
		char code_str[256] = {0};
		if( pDS->FindLayerIdx(FALSE,code,layname) )
		{
			sprintf(code_str,"%I64d", code);
			layname = code_str;
		}
		
		const char *matchname = lstFile.FindMatchItem(layname);
		if( matchname )
			layname = matchname;
		else
		{
			matchname = lstFile.FindMatchItem(pLayer->GetName());
			if( matchname )
				layname = matchname;
		}

		E00FieldInfoArray array;
		E00FieldInfo  field;
		if (strlen(pLayer->GetName())>45)
		{
			strncpy(arcinfo.attributeName,layname,45);
			arcinfo.attributeName[45] = 0;
		}
		else
			strcpy(arcinfo.attributeName,layname);

		CSchemeLayerDefine *pLD = pScheme->GetLayerDefine(pLayer->GetName());
		
		if (arcinfo.points)
		{		
			CString Path;
			
			if( pLD && (pLD->GetGeoClass()==CLS_GEOPOINT || pLD->GetGeoClass()==CLS_GEODIRPOINT || pLD->GetGeoClass()==CLS_GEOSURFACEPOINT) )
				Path = CString(szPath)+"\\"+arcinfo.attributeName+".e00";
			else
				Path = CString(szPath)+"\\"+arcinfo.attributeName+"_point.e00";

			FILE *file = openE00File(Path,1);
			if (file)
			{
				writeLABSection(file,&arcinfo,precision);
				beginWriteIFOSection(file,&arcinfo,precision);
				strcpy(field.fieldName,"XMIN");
				field.fieldType = E00Double;
				array.push_back(field);
				strcpy(field.fieldName,"XMIN");
				field.fieldType = E00Double;
				array.push_back(field);
				strcpy(field.fieldName,"XMAX");
				field.fieldType = E00Double;
				array.push_back(field);
				strcpy(field.fieldName,"YMAX");
				field.fieldType = E00Double;
				array.push_back(field);
				writeBNDIFOField(file,&array,&arcinfo,precision,0);			
				CArray<PT_2D,PT_2D> arrPts;
				for (int i=0;i<arcinfo.pointsCount;i++)
				{
					arrPts.Add(PT_2D(arcinfo.points[i].x,arcinfo.points[i].y));
				}
				Envelope e;
				e.CreateFromPts(arrPts.GetData(),arcinfo.pointsCount);			
				string str;
				sprintfDoubleValue(str,e.m_xl,precision);
				sprintfDoubleValue(str,e.m_yl,precision);
				sprintfDoubleValue(str,e.m_xh,precision);
				sprintfDoubleValue(str,e.m_yh,precision);
				writeIFOFieldValue(file,str);
				array.clear();
				str.erase();
				strcpy(field.fieldName,"AREA");
				field.fieldType = E00Double;
				array.push_back(field);
				strcpy(field.fieldName,"PERIMETER");
				field.fieldType = E00Double;
				array.push_back(field);				
				sprintf(field.fieldName,"%s#",arcinfo.attributeName);
				field.fieldType = E00Integer;
				array.push_back(field);
				sprintf(field.fieldName,"%s-ID",arcinfo.attributeName);		
				field.fieldType = E00Integer;
				array.push_back(field);
				writePATIFOField(file,&array,&arcinfo,precision,0);
				for (i=0;i<arcinfo.pointsCount;i++)
				{	
					sprintfDoubleBlank(str,precision);
					sprintfDoubleBlank(str,precision);
					sprintfIntegerValue(str,arcinfo.points[i].id);
					sprintfIntegerValue(str,arcinfo.points[i].id);
					writeIFOFieldValue(file,str);
					str.erase();
				}
				endWriteIFOSection(file,&arcinfo,precision,0);
				closeE00File(file);
			}
		}
		if (arcinfo.arcs)
		{			
			CString Path;
			
			if( pLD && (pLD->GetGeoClass()==CLS_GEOCURVE || pLD->GetGeoClass()==CLS_GEOPARALLEL || pLD->GetGeoClass()==CLS_GEODCURVE ) )
				Path = CString(szPath)+"\\"+arcinfo.attributeName+".e00";
			else
				Path = CString(szPath)+"\\"+arcinfo.attributeName+"_line.e00";

			FILE *file = openE00File(Path,1);
			if (file)
			{
				writeARCSection(file,&arcinfo,precision,0);
				beginWriteIFOSection(file,&arcinfo,precision);
				strcpy(field.fieldName,"XMIN");
				field.fieldType = E00Double;
				array.push_back(field);
				strcpy(field.fieldName,"XMIN");
				field.fieldType = E00Double;
				array.push_back(field);
				strcpy(field.fieldName,"XMAX");
				field.fieldType = E00Double;
				array.push_back(field);
				strcpy(field.fieldName,"YMAX");
				field.fieldType = E00Double;
				array.push_back(field);
				writeBNDIFOField(file,&array,&arcinfo,precision,0);			
				CArray<PT_2D,PT_2D> arrPts;
				for (int i=0;i<arcinfo.arcsCount;i++)
				{
					if(!arcinfo.arcs[i].isPartofPolygon)
					{
						for (int j=0;j<arcinfo.arcs[i].elements;j++)
						{
							arrPts.Add(PT_2D(arcinfo.arcs[i].x[j],arcinfo.arcs[i].y[j]));
						}					
					}
				}		
				Envelope e;
				e.CreateFromPts(arrPts.GetData(),arrPts.GetSize());			
				string str;
				sprintfDoubleValue(str,e.m_xl,precision);
				sprintfDoubleValue(str,e.m_yl,precision);
				sprintfDoubleValue(str,e.m_xh,precision);
				sprintfDoubleValue(str,e.m_yh,precision);
				writeIFOFieldValue(file,str);
				endWriteIFOSection(file,&arcinfo,precision,0);
				closeE00File(file);
			}
		}
		if (arcinfo.polygons)
		{			
			CString Path;
			
			if( pLD && (pLD->GetGeoClass()==CLS_GEOSURFACE) )
				Path = CString(szPath)+"\\"+arcinfo.attributeName+".e00";
			else
				Path = CString(szPath)+"\\"+arcinfo.attributeName+"_polygon.e00";

			FILE *file = openE00File(Path,1);
			if (file)
			{
				writeARCSection(file,&arcinfo,precision,1);
				writePALSection(file,&arcinfo,precision);
				beginWriteIFOSection(file,&arcinfo,precision);
				endWriteIFOSection(file,&arcinfo,precision,2);
				closeE00File(file);
			}
		}
		if (arcinfo.texts)
		{	
			CString Path;
			
			if( pLD && (pLD->GetGeoClass()==CLS_GEOTEXT) )
				Path = CString(szPath)+"\\"+arcinfo.attributeName+".e00";
			else
				Path = CString(szPath)+"\\"+arcinfo.attributeName+"_text.e00";

			FILE *file = openE00File(Path,1);
			if (file)
			{
				writeTX6Section(file,&arcinfo,precision);
				beginWriteIFOSection(file,&arcinfo,precision);
				endWriteIFOSection(file,&arcinfo,precision,3);
				closeE00File(file);
			}
		}
		
		if (arcinfo.points)
		{
			free(arcinfo.points);
		}
		if(arcinfo.arcs)
		{
			for (int i=0;i<arcinfo.arcsCount;i++)
			{
				if(arcinfo.arcs[i].x)free(arcinfo.arcs[i].x);
				if(arcinfo.arcs[i].y)free(arcinfo.arcs[i].y);
			}
			free(arcinfo.arcs);
		}
		if (arcinfo.polygons)
		{
			for (int i=0;i<arcinfo.polygonsCount;i++)
			{
				if(arcinfo.polygons[i].arcs)free(arcinfo.polygons[i].arcs);
			}
			free(arcinfo.polygons);
		}
		if (arcinfo.texts)
		{
			free(arcinfo.texts);
		}
	}
	GProgressEnd();
}

void CDlgDoc::OnUpdateRecentClass()
{
//	AfxGetMainWnd()->SendMessage (FCCM_INITFTRCLASS,WPARAM(0),LPARAM(&(GetDlgDataSource()->m_UserIdx)));
}

void CDlgDoc::OnWsSetbound() 
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if( !pDS)return;
	PT_3D pts[4];

	CDlgWorkSpaceBound dlg;
	if (UVSModify == GetAccessType(pDS))//uvs
	{
		dlg.m_bUVS = TRUE;
	}
	pDS->GetBound(pts,&(dlg.m_lfZmin),&(dlg.m_lfZmax));	
	
	dlg.m_lfX1 = pts[0].x;
	dlg.m_lfY1 = pts[0].y;
	dlg.m_lfX2 = pts[1].x;
	dlg.m_lfY2 = pts[1].y;
	dlg.m_lfX3 = pts[2].x;
	dlg.m_lfY3 = pts[2].y;
	dlg.m_lfX4 = pts[3].x;
	dlg.m_lfY4 = pts[3].y;
	
	dlg.m_nScale = pDS->GetScale();

	int nPreScale = pDS->GetScale();
	
	if( dlg.DoModal()==IDOK )
	{

		//CDpPoint pts[4];
		memset(pts,0,sizeof(pts));
		pts[0].x = dlg.m_lfX1;
		pts[0].y = dlg.m_lfY1;
		pts[1].x = dlg.m_lfX2;
		pts[1].y = dlg.m_lfY2;
		pts[2].x = dlg.m_lfX3;
		pts[2].y = dlg.m_lfY3;
		pts[3].x = dlg.m_lfX4;
		pts[3].y = dlg.m_lfY4;		
		
		pDS->SetBound(pts,dlg.m_lfZmin,dlg.m_lfZmax);
		pDS->SetScale(dlg.m_nScale);

		if( dlg.m_bTmValid )
		{
			TMProjectionZone prj;
			TMDatum datum;

			dlg.m_tm.GetZone(&prj);
			dlg.m_tm.GetDatum(&datum);
			
			BeginBatchUpdate();
			pDS->SetProjection(prj);
			pDS->SetDatum(datum);
			EndBatchUpdate();
		}

		UpdateAllViews(NULL,hc_SetMapArea,(CObject*)pts);
		SetModifiedFlag();

		ConfigLibItem config = gpCfgLibMan->GetConfigLibItemByScale(pDS->GetScale());

		// 与先前比例尺调用图元库不同则更新采集窗口和地物
		if (config.GetScale() != gpCfgLibMan->GetConfigLibItemByScale(nPreScale).GetScale())
		{
			SetCellDefLib(config.pCellDefLib);
			SetBaseLineTypeLib(config.pLinetypeLib);
			SetLibSourceID((DWORD_PTR)pDS);

			for( int m=0; m<GetDlgDataSourceCount(); m++)
			{
				GetDlgDataSource(m)->ClearLayerSymbolCache();
				GetDlgDataSource(m)->LoadLayerSymbolCache();
			}

			//ini ftrrecnet
			if (config.pScheme != NULL)
			{
				//USERIDX *pRecnet = config.pScheme->GetRecentIdx();
				AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(config.pScheme));
			}
			
			//填充采集面板
			AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(0),LPARAM(&config));

			UpdateAllViews(NULL,hc_Refresh);

			CancelCurrentCommand();
			StartCommand(0);

			AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(4), LPARAM(0));
		}
	}	
	
}

BOOL CDlgDoc::LoadDatabase(LPCTSTR lpszPathName, int cfgscale)
{
	char database[256]={0};
	
	_CheckLoadPath((void*)(LPCTSTR)lpszPathName,database,sizeof(database));

	CAccessModify *pAccess = NULL;
	if (g_isFileOnUVS)
	{
		pAccess= new CUVSModify;
		if( !pAccess )
		{
			return FALSE;
		}
		if(!pAccess->Attach(lpszPathName))
		{
			pAccess->Close();
			return FALSE;
		}
	}
	else
	{
		pAccess= new CSQLiteAccess;
		if( !pAccess )
		{
			return FALSE;
		}
		//数据源关联临时文件
		CString path(lpszPathName);
		int pos = path.ReverseFind(_T('.'));		
		if(pos == -1)
			return FALSE;
		path += _T(".tmp");
	//	path.Insert(pos,_T("~tmp"));
		if( _taccess(LPCTSTR(path),0)!=-1  )
		{		
			if( AfxMessageBox(IDS_DOC_FIND_TMPFILE,MB_YESNO)==IDYES )
			{
				::CopyFile(path,database,FALSE);
			}
			else
			{
				m_strPathName = lpszPathName;
				m_strTmpFile = path;			

				
				::CopyFile(lpszPathName,path,FALSE);
			}
		}
		else
		{
			::CopyFile(lpszPathName,path,FALSE);
		}
		m_strTmpFile = path;
		if( !pAccess->Attach(LPCTSTR(m_strTmpFile)))
		{
			delete pAccess;
			AfxMessageBox(StrFromResID(IDS_FILEFORMAT_ERROR));
			return FALSE;
		}

		if (!((CSQLiteAccess*)pAccess)->IsFileVersionLatest())
		{
			if( AfxMessageBox(IDS_ERR_OLDVERSION_FDB,MB_OKCANCEL)!=IDOK )
			{
				delete pAccess;
				::DeleteFile(m_strTmpFile);
				return FALSE;
			}
			else
			{
				delete pAccess;
				::DeleteFile(m_strTmpFile);

				return UpgradeFDB(lpszPathName);
			}
		}
	}
	
	CDlgDataSource *pDataSource = new CDlgDataSource(m_pDataQuery);	
	if( !pDataSource )
	{
		delete pAccess;
		return FALSE;
	}

	if (g_isFileOnUVS)//uvs
	{
		pAccess->SetDataSource(pDataSource);
	}
	
	pDataSource->SetAccessObject(pAccess);
	m_pDataQuery->m_pFilterDataSource = pDataSource;

	pAccess->ReadDataSourceInfo(pDataSource);
	CLinearizer::m_fPrecision = GetProfileDouble(REGPATH_SYMBOL,REGITEM_ARCTOLER,0.05)*0.001*pDataSource->GetScale();
	SetSymbolDrawScale(GetProfileDouble(REGPATH_SYMBOL,REGITEM_DRAWSCALE,1));
	SetSymbolAnnoScale(GetProfileDouble(REGPATH_SYMBOL,REGITEM_ANNOSCALE,1));
	SetDataSourceScale(pDataSource->GetScale());

	//设置缺省图元库和线型库
	CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(pDataSource->GetScale());
	
	SetCellDefLib(pCfgLibManager->GetCellDefLib(pDataSource->GetScale()));
	SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(pDataSource->GetScale()));
	SetLibSourceID((DWORD_PTR)pDataSource);

	CSymbolFlag sf1(CSymbolFlag::BreakCell,FALSE);
	CSymbolFlag sf2(CSymbolFlag::BreakLinetype,FALSE);

	GetUsedTextStyles()->Load();

	pAccess->BatchUpdateBegin();	
	LoadCellDefs(pDataSource);
	pDataSource->LoadAll(NULL);	
	pAccess->BatchUpdateEnd();

	OnUpdateOption();

	CFtrLayer *pLayer = pDataSource->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
	if( pLayer )
	{
		pDataSource->SetCurFtrLayer(pLayer->GetID());
	}

	m_arrData.Add(pDataSource);
	m_nActiveData = 0;
	SetMapArea();
	
	m_snap.Init(m_pDataQuery);

	SetActualPathName(lpszPathName);
	pDataSource->SetName(lpszPathName);
	pDataSource->SetModifiedFlag(FALSE);

	CQueryMenu *pMenu = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETQUERYMENU,0,(LPARAM)&pMenu);
	if( pMenu )
	{
		if( !pMenu->IsLoaded() )
			pMenu->Load();
	}

	//填充采集面板
	ConfigLibItem *pData = &pCfgLibManager->GetConfigLibItemByScale(pDataSource->GetScale());
	AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(0),LPARAM(pData));
	AfxGetMainWnd()->SendMessage(FCCM_REFRESHCHKSCHBAR,0,0);

	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
	//初始化最近采集列表
	if (pScheme != NULL)
	{
		//USERIDX *pRecnet = pScheme->GetRecentIdx();
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(pScheme));
	}

	//加载参考数据
	CStringArray arrRefPath;
	ReadRefInfo(arrRefPath);
	CString dsPath;
	int num = arrRefPath.GetSize();
	for(int i=0; i<num; i++)
	{
		dsPath = arrRefPath.GetAt(i);
		CDlgDataSource *pDS = NULL;
		if( ReadRefData(dsPath,pDS) )
		{
			m_arrData.Add(pDS);	
			BOOL bEnable = TRUE;
			pDS->EnableMonocolor(&bEnable,NULL);
		}
	}

	UpdateAllViews(NULL,hc_UpdateOverlayBound,NULL);

	if (!g_isFileOnUVS)//uvs
	{
		// 插件数据
		int nCnt;
		const PlugItem *pItem = ((CEditBaseApp*)AfxGetApp())->GetPlugins(nCnt);
		for ( i=nCnt-1;i>=0;i--)
		{
			if (pItem[i].bUsed)
			{
				pItem[i].pObj->LoadPluginData((DocId)(CEditor*)this);
			}
			
		}
	}

	Post_InitDoc();
	AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(this));
	AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(this));

	m_RecentCmd.Load();
	AfxGetMainWnd()->SendMessage(FCCM_LOAD_RECENTCMD, WPARAM(0), LPARAM(&m_RecentCmd));

	return TRUE;
}

BOOL CDlgDoc::LoadDatabase0(LPCTSTR lpszPathName, int cfgscale)
{
	if (g_isFileOnUVS)//uvs
	{
		return FALSE;
	}

	char database[256]={0};
	_CheckLoadPath((void*)(LPCTSTR)lpszPathName,database,sizeof(database));

	CString path(lpszPathName);
	CAccessPlugin *pAccessPlugin = NULL;
	BOOL bUseSqlite = TRUE;
	int nCnt;
	const PlugItem *pItem = ((CEditBaseApp*)AfxGetApp())->GetPlugins(nCnt);
	for (int i=nCnt-1;i>=0;i--)
	{
		if(pItem[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
		{
			CPlugDocs *p = ((CPlugDocs *)pItem[i].pObj);
			if(path.Right(4).CompareNoCase(p->GetExtString())==0)
			{
				m_nPlugItem = i;
				pAccessPlugin = p->GetAccessObj();
			}
		}
	}
	if(pAccessPlugin==NULL)return FALSE;
	CPluginAccess *pAccess = new CPluginAccess;
	if( !pAccess )
	{
		return FALSE;
	}
	
	int pos = path.ReverseFind(_T('.'));		
	if(pos == -1)
		return FALSE;
	path += _T(".tmp");
//	path.Insert(pos,_T("~tmp"));
	if( _taccess(LPCTSTR(path),0)!=-1  )
	{		
		if( AfxMessageBox(IDS_DOC_FIND_TMPFILE,MB_YESNO)==IDYES )
		{		
			::CopyFile(path,database,FALSE);		
		}
		else
		{
			m_strPathName = lpszPathName;
			m_strTmpFile = path;
			::CopyFile(lpszPathName,path,FALSE);
		}
	}
	else
	{
		::CopyFile(lpszPathName,path,FALSE);
	}
	m_strTmpFile = path;


	pAccess->Init(pAccessPlugin,pAccessPlugin->IsUseFdb());
	if( !pAccess->Attach(LPCTSTR(m_strTmpFile)))
	{
		delete pAccess;
		AfxMessageBox(StrFromResID(IDS_FILEFORMAT_ERROR));
		return FALSE;
	}
	CDlgDataSource *pDataSource = new CDlgDataSource(m_pDataQuery);	
	if( !pDataSource )
	{
		delete pAccess;
		return FALSE;
	}
	pDataSource->SetAccessObject(pAccess);
	m_pDataQuery->m_pFilterDataSource = pDataSource;
	pAccess->ReadDataSourceInfo(pDataSource);
	CLinearizer::m_fPrecision = GetProfileDouble(REGPATH_SYMBOL,REGITEM_ARCTOLER,0.05)*0.001*pDataSource->GetScale();
	SetSymbolDrawScale(GetProfileDouble(REGPATH_SYMBOL,REGITEM_DRAWSCALE,1));
	SetSymbolAnnoScale(GetProfileDouble(REGPATH_SYMBOL,REGITEM_ANNOSCALE,1));
	SetDataSourceScale(pDataSource->GetScale());

	CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(pDataSource->GetScale());
	SetCellDefLib(pCfgLibManager->GetCellDefLib(pDataSource->GetScale()));
	SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(pDataSource->GetScale()));
	SetLibSourceID((DWORD_PTR)pDataSource);

	pAccess->BatchUpdateBegin();
	pDataSource->LoadAll(NULL);
	pAccess->BatchUpdateEnd();

	OnUpdateOption();
	CFtrLayer *pLayer = pDataSource->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
	if( pLayer )
	{
		pDataSource->SetCurFtrLayer(pLayer->GetID());
	}
	m_arrData.Add(pDataSource);
	m_nActiveData = 0;
	SetMapArea();
	m_snap.Init(m_pDataQuery);
	SetActualPathName(lpszPathName);
	pDataSource->SetName(lpszPathName);
	pDataSource->SetModifiedFlag(FALSE);
	CQueryMenu *pMenu = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETQUERYMENU,0,(LPARAM)&pMenu);
	if( pMenu )
	{
		if( !pMenu->IsLoaded() )
			pMenu->Load();
	}
	ConfigLibItem *pData = &pCfgLibManager->GetConfigLibItemByScale(pDataSource->GetScale());
	AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(0),LPARAM(pData));
	AfxGetMainWnd()->SendMessage(FCCM_REFRESHCHKSCHBAR,0,0);

	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
	if (pScheme != NULL)
	{
		//USERIDX *pRecnet = pScheme->GetRecentIdx();
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(pScheme));
	}
	CStringArray arrRefPath;
	ReadRefInfo(arrRefPath);
	CString dsPath;
	int num = arrRefPath.GetSize();
	for(i=0; i<num; i++)
	{
		dsPath = arrRefPath.GetAt(i);
		CDlgDataSource *pDS = NULL;
		if( ReadRefData(dsPath,pDS) )
		{
			m_arrData.Add(pDS);	
			BOOL bEnable = TRUE;
			pDS->EnableMonocolor(&bEnable,NULL);
		}	
	}

	UpdateAllViews(NULL,hc_UpdateOverlayBound,NULL);

	Post_InitDoc();
	AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(this));
	AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(this));

	m_RecentCmd.Load();
	AfxGetMainWnd()->SendMessage(FCCM_LOAD_RECENTCMD, WPARAM(0), LPARAM(&m_RecentCmd));

	return TRUE;
}


BOOL CDlgDoc::LoadCellDefs(CDlgDataSource *pDS)
{
	CellDef def;

	/////uvs/////////
	CAccessModify *pAccess = pDS->GetAccessObject();
	if( pAccess==NULL )
		return FALSE;

	def.Create();
	if( pAccess->GetFirstCellDef(def) )
	{
		CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		CCellDefLib *pCellLib = pCfgLibManager->GetCellDefLib(pDS->GetScale());

		def.m_nSourceID = (DWORD_PTR)pDS;
		def.RefreshEnvelope();
		pCellLib->AddCellDef(def);

		def = CellDef();
		def.Create();
		while( pAccess->GetNextCellDef(def) )
		{
			def.m_nSourceID = (DWORD_PTR)pDS;
			def.RefreshEnvelope();
			pCellLib->AddCellDef(def);

			def = CellDef();
			def.Create();
		}

		def.Clear();
	}
	return TRUE;
}


BOOL CDlgDoc::SaveCellDefToFile(LPCTSTR name, GrBuffer2d *pBuf)
{
	if( pBuf==NULL || name==NULL || strlen(name)<=0 )
		return FALSE;

	if( pBuf->HeadGraph()==NULL )
		return FALSE;

	CDlgDataSource *pDS = GetDlgDataSource();
	/////uvs//////////
	CAccessModify *p = pDS->GetAccessObject();

	if( p )
	{
		pBuf->Zoom(1000.0/pDS->GetScale(),1000.0/pDS->GetScale());
		CellDef def;
		def.Create();

		def.m_nSourceID = (DWORD_PTR)pDS;
		strcpy(def.m_name,name);
		def.m_pgr->CopyFrom(pBuf);
		p->SaveCellDef(def);

		CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		CCellDefLib *pCellLib = pCfgLibManager->GetCellDefLib(pDS->GetScale());

		pCellLib->AddCellDef(def);

		return TRUE;
	}
	return FALSE;
}



void CDlgDoc::CreateCADOptAttributes()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	CAccessModify *p = pDS->GetAccessObject();
	if( p==NULL )
		return;
	
	CStringArray field_names, field_types;

	field_names.Add("CAD_SymbolName");  field_types.Add("CHAR[32]");
	p->CreateOptAttributes(CLS_GEOPOINT,field_names,field_types);

	field_names.RemoveAll();  field_types.RemoveAll();
	field_names.Add("CAD_SymbolName");  field_types.Add("CHAR[32]");
	field_names.Add("CAD_Thickness");  field_types.Add("FLOAT");
	p->CreateOptAttributes(CLS_GEOCURVE,field_names,field_types);
	p->CreateOptAttributes(CLS_GEOSURFACE,field_names,field_types);

	field_names.RemoveAll();  field_types.RemoveAll();
	field_names.Add("CAD_SymbolName");  field_types.Add("CHAR[32]");
	field_names.Add("CAD_Thickness");  field_types.Add("FLOAT");
	field_names.Add("CAD_TextStyle");  field_types.Add("CHAR[32]");
	field_names.Add("CAD_FontHeight");  field_types.Add("FLOAT");
	field_names.Add("CAD_FontWidthFactor");  field_types.Add("FLOAT");
	p->CreateOptAttributes(CLS_GEOTEXT,field_names,field_types);
}


HMENU CDlgDoc::CreatePluginMenu(const CArray<PlugItem,PlugItem> &arr)
{
	int i = 0, nSum=0;
	for (i = 0; i < arr.GetSize(); i++)
	{
		if (!arr[i].bUsed) continue;
		nSum++;
	}
	if (nSum == 0) return NULL;

	HMENU hmenu = ::CreateMenu();
	if (!hmenu)return NULL;
	for (i=0;i<arr.GetSize();i++)
	{
		if (!arr[i].bUsed) continue;
		HMENU hsubmenu = ::CreateMenu();
		CPlugBase *pPlug = arr[i].pObj;		
		int nsz = pPlug->GetItemCount();
		const UINT *pui = pPlug->GetItemMenuStrResIDs();
		for (int j=0;j<nsz;j++)
		{
			if(pui[j]==NULL_ID) continue;
			CString str;
			::LoadString(arr[i].hIns,pui[j],str.GetBuffer(256),256);
			str.ReleaseBuffer();
			::AppendMenu(hsubmenu,MF_STRING,arr[i].itemID[j],str);
		}
		::AppendMenu(hmenu,MF_POPUP|MF_STRING,(UINT_PTR)hsubmenu,pPlug->GetPlugName());
	}
	return hmenu;
}
HMENU CDlgDoc::GetDefaultMenu()
{
	CQueryMenu *pMenu = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETQUERYMENU,0,(LPARAM)&pMenu);
	if( !pMenu )return CDocument::GetDefaultMenu();
	pMenu->SetDoc(this);
	pMenu->UpdateWindow();
	if (((CEditBaseApp*)AfxGetApp())->IsSupportPlugs()&&((CEditBaseApp*)AfxGetApp())->m_arrPlugObjs.GetSize()>0)
	{
		HMENU hMenuPlug = CreatePluginMenu(((CEditBaseApp*)AfxGetApp())->m_arrPlugObjs);
		if(!hMenuPlug) return pMenu->m_hMenu;
		int num = ::GetMenuItemCount(pMenu->m_hMenu);
		CString str;
		CString strMenuItem = StrFromResID(IDS_MENU_PLUGIN);
		for (int i=0;i<num;i++)
		{
			::GetMenuString(pMenu->m_hMenu,i,str.GetBuffer(10),10, MF_BYPOSITION);
			str.ReleaseBuffer();
			if (str.CompareNoCase(strMenuItem)==0)
			{
				DeleteMenu(pMenu->m_hMenu,i,MF_BYPOSITION);
				break;
			}
		}
		::AppendMenu(pMenu->m_hMenu,MF_POPUP,(UINT_PTR )hMenuPlug,strMenuItem);
	}
	return pMenu->m_hMenu;

}

void CDlgDoc::OnDataTransform()
{
	CDlgDataTransform dlg;
	if( dlg.DoModal()!=IDOK )
		return;
	else
	{
		if(AfxMessageBox(IDS_DATATRANSFERTIP,MB_YESNO)==IDYES);
		else 
			return;
		
	}
	
	//获取实体对象总数以及点的数目
	CGeometry *pGeo = NULL;
	long lObjSum = 0, lPtSum = 0;
	for (int i=0; i<m_arrData.GetSize(); i++)
	{
		CDlgDataSource *pData = (CDlgDataSource*)m_arrData[i];		
		if( !pData )continue;
		for(int j=0; j<pData->GetFtrLayerCount(); j++)
		{
			CFtrLayer *pFtrLayer = pData->GetFtrLayerByIndex(j);
			if(pFtrLayer!=NULL && pFtrLayer->IsVisible())
			{
				int nObjs = pFtrLayer->GetObjectCount();
				lObjSum += nObjs;
			}
		}
		
	}

	BeginBatchUpdate();
	
	GProgressStart(lObjSum);
	
	//遍历所有层
	for (i=0; i<m_arrData.GetSize(); i++)
	{
		CDlgDataSource *pData = (CDlgDataSource*)m_arrData[i];		
		if( !pData )continue;
		for(int j=0; j<pData->GetFtrLayerCount(); j++)
		{
			CFtrLayer *pFtrLayer = pData->GetFtrLayerByIndex(j);
			if(pFtrLayer!=NULL && pFtrLayer->IsVisible())
			{
				int nObjs = pFtrLayer->GetObjectCount();
				
				for(int m=0; m<nObjs; m++)
				{
					CPFeature pFeature = pFtrLayer->GetObject(m);

					GProgressStep();

					if(pFeature == NULL)
						continue;

					DeleteObject(FtrToHandle(pFeature),FALSE);
					CArray<PT_3DEX,PT_3DEX> arr;
					pFeature->GetGeometry()->GetShape(arr);
					int npt = arr.GetSize();
					PT_3DEX expt;
					for( int k=0; k<npt; k++)
					{
						expt = arr[k];
						
						expt.x += dlg.m_lfDX;
						expt.y += dlg.m_lfDY;
						expt.z += dlg.m_lfDZ;
						
						expt.x *= dlg.m_lfKX;
						expt.y *= dlg.m_lfKY;
						expt.z *= dlg.m_lfKZ;
						
						arr[k] = expt;
					}
					
					pFeature->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());

					RestoreObject(FtrToHandle(pFeature));


				}
			}
		}
		
	}
	
	//进度条复位
	GProgressEnd();

	EndBatchUpdate();
	
	UpdateAllViews(NULL,hc_Refresh);
}


void CDlgDoc::OnRecoverZ()
{
	CString filter(StrFromResID(IDS_LOADDEM_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	CStringArray arr;
	POSITION  pos = dlg.GetStartPosition();  
	while (pos != NULL)  
	{  
		arr.Add(dlg.GetNextPathName(pos));  
	}
	if(arr.GetSize()<=0)return;

	CDlgDataSource *pDlgDataSource = GetDlgDataSource();
	if ( !pDlgDataSource )  return;

	Envelope e = pDlgDataSource->GetBound();

	CPtrArray arrDem;
	CArray<Envelope,Envelope> arrDemEnvelope;
	for (int i=0; i<arr.GetSize(); i++)
	{
		CDSM *pDem = new CDSM;
		if( !pDem || !pDem->Open(arr.GetAt(i)) ) continue;

		CRect bound = pDem->GetBound();
		Envelope e1(bound.left, bound.right, bound.top, bound.bottom);

		if (!e.bIntersect(&e1))
		{
			delete pDem;
			continue;
		}

		arrDemEnvelope.Add(e1);
		arrDem.Add(pDem);
	}

	if (arrDem.GetSize() < 1)
	{
		return;
	}

// 	CDSM dem;
// 	if( !dem.Open(dlg.GetPathName()) )
// 		return;
	
	int nlay, nobj, nptsum, lSum=0,j,k;
	
	
	nlay = pDlgDataSource->GetFtrLayerCount();
	for (i=0; i<nlay; i++ )
	{
		CFtrLayer *pLayer = pDlgDataSource->GetFtrLayerByIndex(i);
		if( !pLayer||pLayer->IsLocked()||!pLayer->IsVisible() )continue;
		lSum += pLayer->GetObjectCount();
	}
	
	GProgressStart(lSum);

	BeginBatchUpdate();
	
	//遍历对象
	PT_3DEX expt;
	nlay = pDlgDataSource->GetFtrLayerCount();
	for( i=0; i<nlay; i++ )
	{
		CFtrLayer *pLayer = pDlgDataSource->GetFtrLayerByIndex(i);
		if( pLayer&&pLayer->IsVisible()&&!pLayer->IsLocked() )
		{
			nobj = pLayer->GetObjectCount();
			for( j=0; j<nobj; j++)
			{
				GProgressStep();
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr) continue;
				CGeometry *pGeo = pFtr->GetGeometry();
				if( !pGeo )continue;

				Envelope e1 = pGeo->GetEnvelope();

				CDSM *pDem = NULL;
				for (k=0; k<arrDem.GetSize(); k++)
				{
					if (e1.bIntersect(&arrDemEnvelope[k]))
					{
						pDem = (CDSM*)arrDem[k];
						break;
					}
				}

				if (pDem == NULL) continue;
								
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);
				nptsum = arrPts.GetSize();
				BOOL bRecover = FALSE;
				for( k=0; k<nptsum; k++)
				{
					expt = arrPts[k];

					expt.z = pDem->GetZ(expt.x,expt.y);
					if( expt.z>DemNoValues+1)
					{
						bRecover = TRUE;
						arrPts[k] = expt;
					}					
				}

				if (bRecover)
				{
					DeleteObject(FtrToHandle(pFtr),FALSE);
					pGeo->CreateShape(arrPts.GetData(),nptsum);					
					RestoreObject(FtrToHandle(pFtr));
				}

				
			}
		}
	}

	for (i=0; i<arrDem.GetSize(); i++)
	{
		if (arrDem[i])
		{
			delete arrDem[i];
		}
		
	}
	arrDem.RemoveAll();
	
	OnSelectChanged();
	UpdateAllViews(NULL,hc_Refresh);

	EndBatchUpdate();
	
	GProgressEnd();
}

void CDlgDoc::OnPhotoToGround()
{
	CDlgConvertPhoto dlg;
	
	if( dlg.DoModal()!=IDOK )return;
	CCoordCenter convert;

	CDSM dsm;

	if( dlg.m_bIsFilePath )
	{
		if( !convert.Init(dlg.m_prj.GetCoreObject(),CString(""),dlg.m_strImg) )
		{
			AfxMessageBox(IDS_ERR_FAILOPENFILE);
			return;
		}

		if( !dsm.Open(dlg.m_strDEM) )
		{
			AfxMessageBox(IDS_ERR_FAILOPENFILE);
			return;
		}
		convert.SetDsm(&dsm);
	}
	else
	{
		if( !convert.Init(dlg.m_prj.GetCoreObject(),dlg.m_strDEM,CString(""),dlg.m_strImg) )
		{
			AfxMessageBox(IDS_ERR_FAILOPENFILE);
			return;
		}
	}
	
	int nlay, nobj, nptsum, lSum=0, i,j,k;
	
	//计数
	CDlgDataSource *pDlgDataSource = GetDlgDataSource();
	if ( !pDlgDataSource )
		return;
	nlay = pDlgDataSource->GetFtrLayerCount();
	for( i=0; i<nlay; i++ )
	{
		CFtrLayer *pLayer = pDlgDataSource->GetFtrLayerByIndex(i);
		if( !pLayer||pLayer->IsLocked()||!pLayer->IsVisible() )continue;
		lSum += pLayer->GetObjectCount();
	}
	
	GProgressStart(lSum);
	
	//遍历对象
	PT_3DEX expt;
	PT_3D pt3d;
	CArray<PT_3D,PT_3D> pt3ds;
	Coordinate coord1,coord2; 	
	
	nlay = pDlgDataSource->GetFtrLayerCount();
	for( i=0; i<nlay; i++ )
	{
		CFtrLayer *pLayer = pDlgDataSource->GetFtrLayerByIndex(i);
		if( pLayer&&pLayer->IsVisible()&&!pLayer->IsLocked() )
		{
			nobj = pLayer->GetObjectCount();
			for( j=0; j<nobj; j++)
			{
				GProgressStep();
//				CGeometry *pGeo = pLayer->GetObject(j)->GetGeometry();				
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr) continue;
				CGeometry *pGeo = pFtr->GetGeometry();
				if( !pGeo )continue;
				
				DeleteObject(FtrToHandle(pLayer->GetObject(j)),FALSE);
				
				nptsum = pGeo->GetCtrlPointSum();
				pt3ds.SetSize(nptsum);
				for( k=0; k<nptsum; k++)
				{
					pt3d = pGeo->GetCtrlPoint(k);
					pt3ds.SetAt(k,pt3d);
				}
				
				CArray<PT_3DEX,PT_3DEX> arr;
				pGeo->GetShape(arr);
				nptsum = arr.GetSize();
				for( k=0; k<nptsum; k++)
				{
					expt = arr[k];
					
					coord1.lx=expt.x; coord1.ly=expt.y; 
					coord1.iType=SCANNER; coord2.iType=PHOTO;
					
					if( convert.Convert(coord1,coord2) )
					{
						coord2.iType=PHOTO; coord1.iType=GROUND; 
						coord2.z = DemNoValues;
						convert.Photo2Ortho(coord2,coord1);
						expt.x=coord1.x; expt.y=coord1.y; expt.z=coord1.z;
					}
					
					arr[k] = expt;
				}

				pGeo->CreateShape(arr.GetData(),arr.GetSize());
				
				nptsum = pt3ds.GetSize();
				for( k=0; k<nptsum; k++)
				{
					pt3d = pt3ds.GetAt(k);
					
					coord1.lx=pt3d.x; coord1.ly=pt3d.y; 
					coord1.iType=SCANNER; coord2.iType=PHOTO;
					
					if( convert.Convert(coord1,coord2) )
					{
						coord2.iType=PHOTO; coord1.iType=GROUND; 
						coord2.z = DemNoValues;
						convert.Photo2Ortho(coord2,coord1);
						pt3d.x=coord1.x; pt3d.y=coord1.y; pt3d.z=coord1.z;
					}
					
					pGeo->SetCtrlPoint(k,pt3d);
				}
				
				RestoreObject(FtrToHandle(pLayer->GetObject(j,FILTERMODE_NULL)));
			}
		}
	}
	
	OnSelectChanged();
	UpdateAllViews(NULL,hc_Refresh);
	
	GProgressEnd();
}

void KickoffSameGeoPts(CArray<GeoPoint,GeoPoint>& arr)
{
	GeoPoint *pts = arr.GetData();
	if( !pts )return;
	
	GeoPoint *pt = pts+1;
	int pos=0, num = arr.GetSize();
	double tolerance = 1e-4;
	
	for( int i=1; i<num; i++,pt++)
	{
		if( fabs(pts[pos].x-pt->x)<tolerance && 
			fabs(pts[pos].y-pt->y)<tolerance && 
			fabs(pts[pos].z-pt->z)<tolerance )
			continue;
		
		if( i!=(pos+1) )pts[pos+1] = *pt;
		pos++;
	}
	
	pos++;
	
	if( num>pos )arr.RemoveAt(pos,num-pos);
}



void KickoffSameGeoPts(CArray<MYPT_3D,MYPT_3D>& arr)
{
	MYPT_3D *pts = arr.GetData();
	if( !pts )return;
	
	MYPT_3D *pt = pts+1;
	int pos=0, num = arr.GetSize();
	double tolerance = 1e-4;
	
	for( int i=1; i<num; i++,pt++)
	{
		if( fabs(pts[pos].x-pt->x)<tolerance && 
			fabs(pts[pos].y-pt->y)<tolerance && 
			fabs(pts[pos].z-pt->z)<tolerance && 
			pts[pos].id==pt->id )
			continue;
		
		if( i!=(pos+1) )
		{
			pts[pos+1] = *pt;
		}
		pos++;
	}
	
	pos++;
	
	if( num>pos )arr.RemoveAt(pos,num-pos);
}


void CDlgDoc::OnBuildTIN()
{
	UpdateAllViews(NULL,hc_DelGraph);
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_BUILDTIN_PRE));
	
	//获取实体对象的点线数据
	int nlay, nobj, npt, i, j, k;
	CGeometry *pGeo;
	CArray<MYPT_3D,MYPT_3D> arrAll, arrCur;
	PT_3DEX expt;
	MYPT_3D geopt;
	MYPT_3D  tri[3];
	CDlgDataSource *pDlgDataSource = GetDlgDataSource();
	if( pDlgDataSource )
	{
		k = 0;
		nlay = pDlgDataSource->GetFtrLayerCount();
		for( i=0; i<nlay; i++ )
		{
			CFtrLayer *pLayer = pDlgDataSource->GetFtrLayerByIndex(i);
			if( pLayer&&/*!pLayer->IsLocked()&&*/pLayer->IsVisible() )
			{
				nobj = pLayer->GetObjectCount();
				for( j=0; j<nobj; j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr) continue;
					pGeo = pFtr->GetGeometry();
					if( !pGeo )continue;
					
					k++;

					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
					{
						expt = pGeo->GetDataPoint(0);
						geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; geopt.type = 0;
						geopt.id = k;
						arrAll.Add(geopt);
					}
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
					{
						const CShapeLine *pShape = pGeo->GetShape();
						if (!pShape) continue;

						CArray<PT_3DEX,PT_3DEX> pts;
						pShape->GetKeyPts(pts);

						npt = pts.GetSize();
						arrCur.RemoveAll();
						for( int n=0; n<npt; n++)
						{
							expt = pts[n];
							geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; 
							geopt.type = 1;
							geopt.id = k;
							arrCur.Add(geopt);
						}
						
						KickoffSameGeoPts(arrCur);
						if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
					}
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
					{
						const CShapeLine *pShape = pGeo->GetShape();
						if (!pShape) continue;

						CArray<PT_3DEX,PT_3DEX> pts;
						pShape->GetKeyPts(pts);
						
						npt = pts.GetSize();
						arrCur.RemoveAll();
						for( int n=0; n<npt; n++)
						{
							expt = pts[n];
							if (expt.pencode == penMove && n != 0)
							{
								k++;
							}
							geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
							geopt.type = 1;
							geopt.id = k;
							arrCur.Add(geopt);
						}
						
						KickoffSameGeoPts(arrCur);
						if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
					}
				}
			}
		}
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_BUILDTIN_START));
	
	//构建三角网，并显示图形
	GrBuffer buf;
	COLORREF color = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_DRAGCOLOR,RGB(255,255,255));
	buf.SetAllColor(color);
	PT_3D pt3d;
	if( arrAll.GetSize()>0 )
	{
		m_tin.Clear();

		MYPT_3D **retPts = NULL;
		int nRetPt = 0;
		MyTriangle::createTIN(arrAll.GetData(),arrAll.GetSize(),&m_tin.tins,&m_tin.nTIN,&m_tin.pts,&m_tin.npt);

		buf.BeginLineString(color,0);

		for( i=0; i<m_tin.nTIN; i++)
		{
			if( !m_tin.GetTriangle(i,tri) )
				continue;

			pt3d.x = tri[0].x; 
			pt3d.y = tri[0].y; 
			pt3d.z = tri[0].z;			

			buf.MoveTo(&pt3d);
			for( j=1; j<3; j++)
			{
				pt3d.x = tri[j].x; pt3d.y = tri[j].y; pt3d.z = tri[j].z;
				buf.LineTo(&pt3d);
			}
			
			pt3d.x = tri[0].x; pt3d.y = tri[0].y; pt3d.z = tri[0].z;
			buf.LineTo(&pt3d);
		}

		buf.End(FALSE);
		
		buf.RefreshEnvelope();
	
		UpdateAllViews(NULL,hc_AddGraph,(CObject*)&buf);
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_BUILDTIN_END));
}

void CDlgDoc::OnDestroyTIN()
{
	m_tin.Clear();

	UpdateAllViews(NULL,hc_DelGraph);
}

void CDlgDoc::OnExportTIN()
{
	//计算过滤字符串
	CString filter;
	
	filter.LoadString(IDS_LOADDXF_FILTER2);
	CFileDialogEx dlg(StrFromResID(IDS_NEWFILE_TITLE),FALSE,".dxf",FormatFileNameByExt(GetPathName(),_T("dxf")),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	if( dlg.DoModal()!=IDOK )return;
	
	//创建dxf内存文件体
	CDxfDrawing dxfFile;
	dxfFile.Create();
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0, (LPARAM)(LPCTSTR)StrFromResID(IDS_EXPORTDXF_START));
	
	LAYER Layer;
	
	//配置线型
	OBJHANDLE hlt = 0;
	{
		LTYPE ltype;
		memset(&ltype,0,sizeof(ltype));
		strcpy(ltype.Name, "Continuous");
		strcpy(ltype.DescriptiveText, "Solid line");
		hlt = dxfFile.AddLinetype(&ltype);
	}
	
	//添加一个层
	{
		Layer.StandardFlags = 0;
		Layer.Color = 1;
		Layer.LineTypeObjhandle = hlt;
		Layer.LineWeight = 0;
		Layer.PlotFlag = FALSE;
		Layer.PlotStyleObjhandle = 0;
		strcpy(Layer.Name,"TIN");
		
		dxfFile.AddLayer(&Layer);
		dxfFile.SetLayer(Layer.Name);
	}
	
	//输出三角形信息
	MYPT_3D  tri[3];
	ENTVERTEX pts[3];
	for( int i=0; i<m_tin.nTIN; i++)
	{
		if( !m_tin.GetTriangle(i,tri) )
			continue;
		
		memset(pts,0,sizeof(pts));
		for( int j=0; j<3; j++)
		{
			pts[j].Point.x = tri[j].x; pts[j].Point.y = tri[j].y; pts[j].Point.z = tri[j].z;
		}
		
		dxfFile.PolyLine(pts,3,9);
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0, (LPARAM)(LPCTSTR)StrFromResID(IDS_DOC_EXPORT_WRITING));
	dxfFile.SaveDXFFile(dlg.GetPathName());
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0, (LPARAM)(LPCTSTR)StrFromResID(IDS_DOC_EXPORT_END));
}

void KickoffSameGeo2Pts(CArray<GeoPoint,GeoPoint>& arr)
{
	GeoPoint *pts = arr.GetData();
	if( !pts )return;
	
	GeoPoint *pt = pts+1;
	int pos=0, num = arr.GetSize();
	double tolerance = 1e-4;
	
	for( int i=1; i<num; i++,pt++)
	{
		if( fabs(pts[pos].x-pt->x)<tolerance && 
			fabs(pts[pos].y-pt->y)<tolerance && 
			fabs(pts[pos].z-pt->z)<tolerance )
			continue;
		
		if( i!=(pos+1) )pts[pos+1] = *pt;
		pos++;
	}
	
	pos++;
	
	if( num>pos )arr.RemoveAt(pos,num-pos);
}


void CDlgDoc::OnCreateContours()
{
	CDlgCreateContours dlg;

	if (dlg.DoModal() != IDOK)
		return;
	

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_CREATE_CONTOUR_PRE));
	
	//获取实体对象的点线数据
	int nlay, nobj, npt, i, j, k;
	CGeometry *pGeo;
	CArray<GeoPoint,GeoPoint> arrAll, arrCur;
	PT_3DEX expt;
	GeoPoint geopt;
	GeoPoint  tri[3];
	CDlgDataSource *pDlgDataSource = GetDlgDataSource();
	if( pDlgDataSource )
	{
		nlay = pDlgDataSource->GetFtrLayerCount();
		for( i=0; i<nlay; i++ )
		{
			CFtrLayer *pLayer = pDlgDataSource->GetFtrLayerByIndex(i);
			if( pLayer&&pLayer->IsVisible() )
			{
				nobj = pLayer->GetObjectCount();
				for( j=0; j<nobj; j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr) continue;
					pGeo = pFtr->GetGeometry();
					if( !pGeo )continue;
					
					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
					{
						expt = pGeo->GetDataPoint(0);
						geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; geopt.p = 10002;
						arrAll.Add(geopt);
					}
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
					{
						CArray<PT_3DEX,PT_3DEX> pts;
						pGeo->GetShape(pts);
						npt = pts.GetSize();
						arrCur.RemoveAll();
						for( k=0; k<npt; k++)
						{
							expt = pts.GetAt(k);
							geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; 
							geopt.p = (k==0?10002:10001);
							arrCur.Add(geopt);
						}
						
						KickoffSameGeo2Pts(arrCur);
						if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
					}
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
					{
						CArray<PT_3DEX,PT_3DEX> pts;
						pGeo->GetShape(pts);
						npt = pts.GetSize();
						arrCur.RemoveAll();
						for( k=0; k<npt; k++)
						{
							expt = pts.GetAt(k);
							geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
							geopt.p = (k==0?10002:1003);
							arrCur.Add(geopt);
						}
						
						KickoffSameGeo2Pts(arrCur);
						if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
					}
					else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)))
					{
						CArray<PT_3DEX,PT_3DEX> pts;
						pGeo->GetShape(pts);
						npt = pts.GetSize();
						arrCur.RemoveAll();
						geopt.p = 10002;
						for( k=0; k<npt; k++)
						{
							expt = pts.GetAt(k);
							geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;							
							arrCur.Add(geopt);
						}
						KickoffSameGeo2Pts(arrCur);
						if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
					}
				}
			}
		}
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_CREATE_CONTOUR_START));
	
	//构建等高线
// 	CFtrLayer *pLayer0 = pDlgDataSource->GetFtrLayerByNameOrLayerIdx(dlg.m_strIndexFCode);
// 	CFtrLayer *pLayer1 = pDlgDataSource->GetFtrLayerByNameOrLayerIdx(dlg.m_strCountFCode);
	CFtrLayer *pLayer0 = pDlgDataSource->GetFtrLayer(dlg.m_strIndexFCode);
	CFtrLayer *pLayer1 = pDlgDataSource->GetFtrLayer(dlg.m_strCountFCode);
	int nScale = GetDlgDataSource()->GetScale();
	CFeature *pFtr0 = (pLayer0==NULL?NULL:pLayer0->CreateDefaultFeature(nScale));
	CFeature *pFtr1 = (pLayer1==NULL?NULL:pLayer1->CreateDefaultFeature(nScale));

	if( arrAll.GetSize()>0 && pFtr0 && pFtr1 )
	{
		//CGeoBuilderContourX build;
		CGeoBuilderContour build;
		GeoLine *pLine;
		CArray<PT_3DEX,PT_3DEX> expt;
		

		CFeature *pFtr = NULL;
 		int lh0 = pLayer0->GetID();
 		int lh1 = pLayer1->GetID();

		Envelope e = CreateEnvelopeFromPts(arrAll.GetData(),arrAll.GetSize());
		float fInterval = sqrt(e.Width()*e.Height()/arrAll.GetSize());

		build.m_lfDemInterval = FLT_MAX;
	//	build.m_bLasMode = true;
	//	build.Init((GeoPoint*)arrAll.GetData(),arrAll.GetSize(),fInterval,fInterval,dlg.m_fInterval);
		bool bRet = build.Init((GeoPoint*)arrAll.GetData(),arrAll.GetSize(),dlg.m_fInterval);
		int nContours = build.GetContourNumber();
		
		GProgressStart(nContours);
		for( i=0; i<nContours; i++)
		{
			GProgressStep();

			pLine = NULL;
			if( !build.GetLine(i,pLine) || pLine==NULL )continue;
			if( pLine->iPtNum<=1 )continue;

			double z0 = (dlg.m_nInterNum+1)*dlg.m_fInterval*floor(pLine->gp[0].z/((dlg.m_nInterNum+1)*dlg.m_fInterval));

			BOOL bFlag = FALSE;
			//计曲线
			if( fabs(pLine->gp[0].z-z0)<1e-4 )
			{
				pFtr = pFtr0->Clone();
				bFlag = TRUE;
			}
			//首曲线
			else
				pFtr = pFtr1->Clone();

			if( !pFtr )continue;

			expt.RemoveAll();

			for( j=0; j<pLine->iPtNum; j++)
			{
				PT_3DEX pt;
				pt.x = pLine->gp[j].x; pt.y = pLine->gp[j].y; pt.z = pLine->gp[j].z;
				pt.pencode = penLine;
				expt.Add(pt);
			}
			
			pFtr->GetGeometry()->CreateShape(expt.GetData(),expt.GetSize());			
			

			AddObject(pFtr,bFlag?lh0:lh1);

		}

		GProgressEnd();
		
		UpdateAllViews(NULL,hc_Refresh);
	}

	//AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_CREATE_CONTOUR_END));
}


void CDlgDoc::OnCreateContoursFromDEM()
{
	CDlgCreateContours1 dlg;
	if( dlg.DoModal()!=IDOK )return;
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_CREATE_CONTOUR_PRE));
	
	CDSM demObj;
	if( !demObj.Open(dlg.m_strDEMFile) )
		return;
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_CREATE_CONTOUR_START));
	
	CDlgDataSource *pDlgDataSource = GetDlgDataSource();
	if (!pDlgDataSource)
		return;

	//构建等高线
	CFtrLayer *pLayer0 = pDlgDataSource->GetFtrLayer(dlg.m_strIndexFCode);
	CFtrLayer *pLayer1 = pDlgDataSource->GetFtrLayer(dlg.m_strCountFCode);
	int nScale = GetDlgDataSource()->GetScale();
	CFeature *pFtr0 = (pLayer0==NULL?NULL:pLayer0->CreateDefaultFeature(nScale));
	CFeature *pFtr1 = (pLayer1==NULL?NULL:pLayer1->CreateDefaultFeature(nScale));
	
	GridContour contour;
	DEMHEADERINFO dh = demObj.m_demInfo;
	
	if( pFtr0 && pFtr1 )
	{
		double *x = NULL;
		double *y = NULL;
		double *z = NULL;
		int iNum = 0;
		float fContourInterval = dlg.m_fInterval;
		contour.m_iIndexInterval = dlg.m_nInterNum;
		iNum = demObj.GetPoints(&x,&y,&z,0); 
		if(iNum>20)
		{
			if (contour.Init(x, y, z, iNum, dh.lfDx, dh.lfDy, fContourInterval)) //set contour interval here  
			{
				CGeoCurve *pLine = NULL;
				
				CFeature *pFtr = NULL;
				int lh0 = pLayer0->GetID();
				int lh1 = pLayer1->GetID();
				
// 				CDpDBVariant var;
// 				var = (long)dlg.m_nExCode;
				
				int nContours = contour.GetContourNumber(), i, j;
				
				GProgressStart(nContours);

				for( i=0; i<nContours; i++)
				{
					GProgressStep();

					CArray<PT_3DEX,PT_3DEX> expt;
					
					pLine = contour.GetLine(i);
					if (pLine == NULL)continue;

					CArray<PT_3DEX, PT_3DEX> arrPts;
					pLine->GetShape(arrPts);
					if (arrPts.GetSize()<2) continue;
					
					double z0 = (dlg.m_nInterNum + 1)*dlg.m_fInterval*floor(arrPts[0].z / ((dlg.m_nInterNum + 1)*dlg.m_fInterval));
					
					BOOL bFlag = FALSE;
					//计曲线
					if (fabs(arrPts[0].z - z0)<1e-4)
					{
						pFtr = pFtr0->Clone();
						bFlag = TRUE;
					}
					//首曲线
					else
						pFtr = pFtr1->Clone();
					
					if( !pFtr )continue;
					
					pFtr->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());

					
					AddObject(pFtr,bFlag?lh0:lh1);
				}
				
				GProgressEnd();
			}
		}
		
		UpdateAllViews(NULL,hc_Refresh);
	}

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_CREATE_CONTOUR_END));
}

//检查表达式括号是否匹配,匹配则返回TRUE
BOOL BlacketMatch(const string &operation)
{
	string stack;
	for(string::const_iterator i=operation.begin();i!=operation.end();i++)
	{
		if (*i == '(')
			stack += *i;
		else if (*i == ')')
		{
			if (stack.size()>0 && stack[stack.size()-1]=='(')
				stack.erase(stack.end()-1);
			else
				return FALSE;
			
		}
		
	}
	return stack.empty();
	
}

//计算后缀表达式的值
BOOL CalPostValue(const string &s)
{
	string stack;
	int m, n;
	for (string::const_iterator i=s.begin(); i!=s.end(); i++)
	{
		if (*i=='1'||*i=='0')
			stack += *i;
		else
		{
			if (*i == '&')
			{
				
				m = stack[stack.size()-1];
				stack.erase(stack.end()-1);
				n = stack[stack.size()-1];
				stack.erase(stack.end()-1);
				stack += (m&n);
			}
			else
			{
				m = stack[stack.size()-1];
				stack.erase(stack.end()-1);
				n = stack[stack.size()-1];
				stack.erase(stack.end()-1);
				stack += (m|n);
				
			}
		}
	}
	if (!stack.empty())
	{
		if(stack[0]=='0')
			return FALSE;
		else
			return TRUE;
	}
	return FALSE;
	
	
}

//中缀表达式转向后缀表达式
BOOL Middle2Post(const string &infix,string &postfix)
{
	if (infix.empty() || !BlacketMatch(infix) )
		return FALSE;

	string stack;
    int i,len;
    len = infix.size();
    for (i=0; i<len; i++)
    {
        switch(infix[i])
        {
        case '(':
			stack += '(';
			break;
			
        case '&':
        case '|':
			while (stack.size()>0 && stack[stack.size()-1]!='(')
			{
				postfix += stack[stack.size()-1];
				stack.erase(stack.end()-1);
			}
			stack += infix[i];
			break;	
        case')':
			while (stack[stack.size()-1] != '(')
			{
				postfix += stack[stack.size()-1];
				stack.erase(stack.end()-1);
			}
			stack.erase(stack.end()-1);
			break;
        default:
			postfix += infix[i];
			break;
			
		}
    }
    while (stack.size() > 0)
    {
		postfix += stack[stack.size()-1];
		stack.erase(stack.end()-1);
	}
	return TRUE;
	
}

void FillCompareInfo(CONDCOMP *pcc, int baseno, int& size)
{
	int i=0;
	CONDCOMP ccs[] = 
	{
		{ IDS_SELCOND_X, baseno+i++, (PFUNCOMP)CompX },
		{ IDS_SELCOND_Y, baseno+i++, (PFUNCOMP)CompY },
		{ IDS_SELCOND_Z, baseno+i++, (PFUNCOMP)CompZ },
		{ IDS_SELCOND_INTZ, baseno+i++, (PFUNCOMP)CompIZ },
		{ IDS_SELCOND_DZ, baseno+i++, (PFUNCOMP)CompDZ },
		{ IDS_SELCOND_DIS, baseno+i++, (PFUNCOMP)CompPerDis },
		{ IDS_SELCOND_ANG, baseno+i++, (PFUNCOMP)CompPerAngle },
		{ IDS_SELCOND_ENDDIS, baseno+i++, (PFUNCOMP)CompEndDis },
		{ IDS_SELCOND_LINETYPE, baseno+i++, (PFUNCOMP)CompLineType },
		{ IDS_SELCOND_LINEDIR, baseno+i++, (PFUNCOMP)CompClockwise },
		{ IDS_SELCOND_LEN, baseno+i++, (PFUNCOMP)CompLen },
		{ IDS_SELCOND_AREA, baseno+i++, (PFUNCOMP)CompArea },
		{ IDS_SELCOND_INTERSECTSELF, baseno+i++, (PFUNCOMP)CompIntersectSelf },
		{ IDS_SELCOND_SUSPENDPOINT, baseno+i++, (PFUNCOMP)CompSuspend },
		{ IDS_SELCOND_STREAMREVERSE, baseno+i++, (PFUNCOMP)StreamReverse },
		{ IDS_SELCOND_SAMEPOINT, baseno+i++, (PFUNCOMP)CompSamePoint },
		{ IDS_SELCOND_SAMELINE, baseno+i++, (PFUNCOMP)CompSameLine },
		{ IDS_SELCOND_LLINTERSECT, baseno+i++, (PFUNCOMP)CompIntersectLine }
	};

	if( size<=0 )
	{
		size = sizeof(ccs)/sizeof(CONDCOMP);
		return;
	}
	if( size>sizeof(ccs)/sizeof(CONDCOMP) )size = sizeof(ccs)/sizeof(CONDCOMP);
	if( pcc )
	{
		memcpy(pcc,ccs,size*sizeof(CONDCOMP));
	}
}

int GetFieldIndex(CStringArray& fields,CString field)
{
	for (int i=0; i<fields.GetSize(); i++)
	{
		if (fields[i].CompareNoCase(field) == 0)
		{
			return i;
		}
	}

	return -1;
}

void QueryCond(CDlgDataSource *pDataSource, CArray<CONDSEL,CONDSEL>& conds, int nbaseno,
			   CStringArray& fields, CStringArray& names, CCPResultList& result)
{
	

	int condnum = conds.GetSize();
	if( condnum<=0 )return;

	CString strExpression;
	
	for (int i=0; i<conds.GetSize(); i++)
	{
		if (i != 0)
		{
			if (conds[i].condtype[0])
			{
				strExpression += '&';
			}
			else
				strExpression += '|';
		}
		if (conds[i].condtype[1])
		{
			strExpression += '(';
		}
		
		CString str;
		str.Format("%d",i);
		strExpression += str;
		
		if (conds[i].condtype[2])
		{
			strExpression += ')';
		}
	}

	//检测条件表达式格式是否正确
	string infix,postfix;
	for (int k=0; k<strExpression.GetLength(); k++)
	{
		if (strExpression[k] != ' ')
		{
			infix += strExpression[k];				
		}
		
	}
	
	if (!Middle2Post(infix,postfix))
	{
		AfxMessageBox(StrFromResID(IDS_EXPRESSION_ERROR));
		return;
	}

	int fdnum = nbaseno;
	int nccs = -1;
	FillCompareInfo(NULL,fdnum,nccs);	
	if (nccs <= 0) return;

	CONDCOMP *ccs = new CONDCOMP[nccs];
	if (!ccs)  return;	

	FillCompareInfo(ccs,fdnum,nccs);

	for (i=0; i<condnum;i++)
	{
		CONDSEL item = conds.GetAt(i);
		result.attrNameList.Add(CString(item.field));
	}

	//获取实体对象总数
	long lSum = 0;
	if( pDataSource )
	{
		int nlaynum = pDataSource->GetFtrLayerCount();
		for( int i=0; i<nlaynum; i++ )
		{
			CFtrLayer *pLayer = pDataSource->GetFtrLayerByIndex(i);
			if( pLayer&&pLayer->IsVisible()  )
			{
				lSum += pLayer->GetObjectCount();
			}
		}
	}

	if( lSum<=0 )return;

	//获取当前活动视图
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	
	// Get the active MDI child window.
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	
	// or CMDIChildWnd *pChild = pFrame->MDIGetActive();
	
	// Get the active view attached to the active MDI child
	// window.
	CView *pView = pChild->GetActiveView();
	if(pView == NULL)
		return;

	CCoordSys *pCoordSys = NULL;
	if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)) || pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
		pCoordSys = ((CBaseView*)pView)->GetCoordWnd().m_pSearchCS;

	if (!pCoordSys)
		return;

	gSSP_forPoint.Clear();
	gSSP_forPoint.Init(pDataSource,pCoordSys);
//	gSSP_forPoint.m_dwFindMode = pDoc->m_selection.m_dwFindMode;
//	gSSP_forPoint.m_bWithSymbol= pDoc->m_selection.m_bWithSymbol;
	gSSP_forPoint.Prepare();

	gSSP_forLine.Clear();
	gSSP_forLine.Init(pDataSource,pCoordSys);
// 	gSSP_forLine.m_dwFindMode = pDoc->m_selection.m_dwFindMode;
// 	gSSP_forLine.m_bWithSymbol= pDoc->m_selection.m_bWithSymbol;
	gSSP_forLine.Prepare();

// 	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESSING));
// 	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lSum);

	GOutPut(StrFromResID(IDS_PROCESSING));
	GProgressStart(lSum);

	for (i=0; i<pDataSource->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDataSource->GetFtrLayerByIndex(i);
		if ( !pLayer||!pLayer->IsVisible() )
			continue;
		
		int nobj = pLayer->GetObjectCount();
		int nResultNum = 0;
		for (int j=0; j<nobj; j++)
		{
			GProgressStep();

			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;

			result.StartRecord();
			result.SetCurFtr(pFtr);

			//验证地物是否满足条件
// 			int *cal = new int[conds.GetSize()];
// 			if (!cal)  continue;
			CByteArray cal;
			cal.SetSize(conds.GetSize());

			for (int k=0; k<conds.GetSize(); k++)
			{
				CONDSEL cond = conds.GetAt(k);

				result.SetCurAttr(k);

				int fidx = GetFieldIndex(names,cond.field);

				if (fidx < 0)
				{
					cal[k] = '0';
				}
				else if (/*cond.*/fidx < fdnum)
				{
					cal[k] = CompField(pDataSource,pFtr,fields[fidx],cond.op,cond.value,&result)?'1':'0';
				}
				else if(/*cond.*/fidx>=fdnum && /*cond.*/fidx<fdnum+nccs)
				{
					cal[k] = (*(ccs[/*cond.*/fidx-fdnum].pfun))(pDataSource,pFtr,"",cond.op,cond.value,&result)?'1':'0';
				}
				else
					cal[k] = '0';
				
			}

			string fix = postfix;
			//将后缀表达式中的数字换成基本表达式的结果
			int nsum = 0;
			for (k=0; k<fix.size(); k++)
			{
				if (isdigit(fix[k]) && nsum<conds.GetSize())
				{
// 					char ch[2];
// 					sprintf(ch,"%d",cal[nsum]);
// 					fix[k] = ch[0];
					fix[k] = cal[nsum];
					nsum++;
				}
			}

// 			if (cal)
// 			{
// 				delete []cal;
// 			}
			
			if ( CalPostValue(fix) )
			{
				nResultNum++;
				result.FinishRecord();
				if (nResultNum%100 == 0)
				{
//					AfxGetMainWnd()->SendMessage(FCCM_SEARCHRESULT,2,(LPARAM)&result);
				}
			}
			else
				result.AbortRecord();	
			
		}

	}

	GProgressEnd();

//	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);

	gSSP_forPoint.Clear();
	gSSP_forLine.Clear();

	if (ccs)  delete []ccs;


}

void CDlgDoc::OnConditionSelect()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;

	CDlgConditionSelect dlg;

	CStringArray strField;
	// 层名
	strField.Add(FIELDNAME_LAYERNAME);
	dlg.m_arrFields.Add(StrFromResID(IDS_FIELDNAME_LAYERNAME));
	
	// 基本属性
	pDS->GetAllField(TRUE,strField,dlg.m_arrFields);	

	int fdnum = dlg.m_arrFields.GetSize();
	// 自定义属性
	int nccs = 50;
	CONDCOMP ccs[50];
	FillCompareInfo(ccs,fdnum,nccs);
	
	for (int i=0; i<nccs; i++)
	{
		dlg.m_arrFields.Add(StrFromResID(ccs[i].strid));
	}
	
	CCPResultList result;

	if ( dlg.DoModal()!=IDOK )
		return;	
	
	QueryCond(pDS,dlg.m_arrConds,fdnum,strField,dlg.m_arrFields,result);
	
	AfxGetMainWnd()->SendMessage(FCCM_SEARCHRESULT,2/*dlg.m_nApply*/,(LPARAM)&result);
	
	OnSelectChanged();
	GOutPut(StrFromResID(IDS_PROCESS_END));
	
}


void CDlgDoc::OnConditionSelectConfig()
{
	CQueryMenu *pMenu = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETQUERYMENU,0,(LPARAM)&pMenu);
	if( !pMenu )return;
	
	CDlgConditionSelectConfig dlg;
	dlg.m_cQueryMenu.CopyCondData(*pMenu);

	// 层名
	dlg.m_arrFields.Add(StrFromResID(IDS_FIELDNAME_LAYERNAME));

	// 基本属性
	CStringArray strField;	
	GetDlgDataSource()->GetAllField(TRUE,strField,dlg.m_arrFields);

	int fdnum = dlg.m_arrFields.GetSize();
	// 自定义属性
	int nccs = 50;
	CONDCOMP ccs[50];
	FillCompareInfo(ccs,fdnum,nccs);
	
	for (int i=0; i<nccs; i++)
	{
		dlg.m_arrFields.Add(StrFromResID(ccs[i].strid));
	}
	
	CCPResultList result;
	
	if( dlg.DoModal()==IDOK )
	{
		pMenu->CopyCondData(dlg.m_cQueryMenu);
		pMenu->Save();
		pMenu->SetDoc(this);
		pMenu->UpdateWindow();
		((CFrameWnd*)AfxGetMainWnd())->OnUpdateFrameMenu(NULL);		
	}
}

void CDlgDoc::OnSelectReverse()
{
	int num = 0;
	m_selection.GetSelectedObjs(num);

	if( num<=0 )
		return;

	CDlgDataSource *pDS = GetDlgDataSource();
	pDS->GetFtrLayerCount();
	int i,j;
	CArray<FTR_HANDLE,FTR_HANDLE> arr;
	for ( i=0;i<pDS->GetFtrLayerCount();i++)
	{
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if(!pFtrLayer||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible()) continue;
		
		for (j=0;j<pFtrLayer->GetObjectCount();j++)
		{
			CFeature *pFtr = pFtrLayer->GetObject(j);
			if (!pFtr)  continue;

			if( !m_selection.IsObjInSelection(FtrToHandle(pFtr)) )
				arr.Add(FtrToHandle(pFtr));
		}		
	}

	m_selection.DeselectAll();
	m_selection.SelectAll(arr.GetData(),arr.GetSize());

	OnSelectChanged();
}


void CDlgDoc::OnFilterSelect()
{
	if( m_pFilterSelect==NULL )
	{
		m_pFilterSelect = new CDlgFilterSelect();
		if( !m_pFilterSelect )
			return;

		m_pFilterSelect->m_pDoc = this;

		m_pFilterSelect->Create(IDD_FILTERSELECT,AfxGetMainWnd());
	}

	m_pFilterSelect->RefreshProps();
	m_pFilterSelect->ShowWindow(SW_SHOWNORMAL);
}

extern CString PencodeToText(int pencode);
extern CString PurposeToText( int nPurpose);

static int atol2(LPCTSTR str, int* is_ok)
{
	int value = 0;
	if( sscanf(str,"%d",&value)==1 )
	{
		if( is_ok )*is_ok = 1;
		return value;
	}
	if( is_ok )*is_ok = 0;
	return value;
}

static double atof2(LPCTSTR str, int* is_ok)
{
	double value = 0;
	if( sscanf(str,"%lf",&value)==1 )
	{
		if( is_ok )*is_ok = 1;
		return value;
	}
	if( is_ok )*is_ok = 0;
	return value;
}

static BOOL CompareValue(const CVariantEx *pvar, LPCTSTR value)
{
	_variant_t var = *pvar;

	char value2[256] = {0};
	strcpy(value2,value);

	value = value2;

	int is_ok1 = 0, is_ok2 = 0;

	//范围值
	if( value[0]=='[' )
	{
		char pos[256] = { 0 };
		strcpy_s(pos, strchr(value, ','));
		if( !pos )return FALSE;

		*pos = 0;
		char v1[256] = {0}, v2[256] = {0};
		strcpy(v1,value+1);
		strcpy(v2,pos+1);

		strcpy_s(pos, strchr(v2, ']') );
		if( !pos )return FALSE;
		*pos = 0;

		switch( var.vt )
		{
		case VT_UI1:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = (BYTE)var;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_UI2:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = var.uiVal;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_UI4:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = var.ulVal;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_UINT:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = var.uintVal;
					if( t3>=t1 && t3<=t2 )
						return TRUE;

				}
			}
			break;
		case VT_I1:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = var.cVal;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_I2:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = (short)var;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;	
		case VT_I4:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = (long)var;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_INT:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = var.intVal;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_BOOL:
			{
				int t1 = atol2(v1,&is_ok1), t2 = atol2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					int t3 = (int)(bool)var;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_R4:
			{
				float t1 = atof2(v1,&is_ok1), t2 = atof2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					float t3 = (float)var;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_R8:
			{
				double t1 = atof2(v1,&is_ok1), t2 = atof2(v2,&is_ok2);
				if( is_ok1 && is_ok2 )
				{
					double t3 = (double)var;
					if( t3>=t1 && t3<=t2 )
						return TRUE;
				}
			}
			break;
		case VT_BSTR:
			{
				char t3[256] = {0};
				strncpy(t3,(LPCTSTR)(_bstr_t)var,sizeof(t3)-1);

				if( stricmp(v1,t3)>=0 && stricmp(t3,v2)>=0 )
				{
					return TRUE;
				}
			}
			break;
		default:
			;
		}
	}
	//枚举值
	else
	{
		CStringArray arrValues;
		char *p0 = value2;
		char *pos = strchr(p0,',');

		while( pos!=NULL )
		{
			*pos = 0;
			arrValues.Add(p0);
			p0 = pos+1;
			pos = strchr(p0,',');
		}

		arrValues.Add(p0);
		for( int i=0; i<arrValues.GetSize(); i++)
		{
			CString v1 = arrValues[i];
			switch( var.vt )
			{
			case VT_UI1:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = (BYTE)var;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_UI2:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = var.uiVal;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_UI4:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = var.ulVal;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_UINT:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = var.uintVal;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_I1:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = var.cVal;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_I2:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = (short)var;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;	
			case VT_I4:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = (long)var;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_INT:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = var.intVal;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_BOOL:
				{
					int t1 = atol2(v1,&is_ok1);
					if( is_ok1 )
					{
						int t3 = (int)(bool)var;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_R4:
				{
					float t1 = atof2(v1,&is_ok1);
					if( is_ok1 )
					{
						float t3 = (float)var;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_R8:
				{
					double t1 = atof2(v1,&is_ok1);
					if( is_ok1 )
					{
						double t3 = (double)var;
						if( t3==t1 )
							return TRUE;
					}
				}
				break;
			case VT_BSTR:
				{
					char t3[256] = {0};
					strncpy(t3,(LPCTSTR)(_bstr_t)var,sizeof(t3)-1);

					if( stricmp(v1,t3)==0 )
					{
						return TRUE;
					}
				}
				break;
			default:
				;
			}
		}
	}

	return FALSE;
}

BOOL FilterFeature(CValueTable& tab, CDlgDataSource *pDS, CFtrLayer *pLayer, CStringArray& arrBasicFields, CFeature *pFtr)
{
	const CVariantEx *pvar;

	CString value0, value1;

	CGeometry *pGeo = pFtr->GetGeometry();

	// 类型
	if( tab.GetValue(0,FIELDNAME_GEOCLASS,pvar) )
	{
		long cls = pGeo->GetClassType();
		long cls0 = (long)(_variant_t)*pvar;
		long buf[] = {
			CLS_GEOPOINT, CLS_GEODIRPOINT, CLS_GEOCURVE, CLS_GEOPARALLEL, CLS_GEODCURVE,
			CLS_GEOSURFACE, CLS_GEOTEXT, CLS_GEOSURFACEPOINT
		};

		if( cls==CLS_GEOMULTISURFACE )
			cls = CLS_GEOSURFACE;
		if( cls==CLS_GEOMULTIPOINT )
			cls = CLS_GEOPOINT;
		if( cls==CLS_GEODEMPOINT )
			cls = CLS_GEOPOINT;

		for( int i=0; i<sizeof(buf)/sizeof(buf[0]); i++)
		{
			if( cls==buf[i] && (cls0&(1<<i))==0 )
			{
				return FALSE;
			}
		}
	}

	// color
	if( tab.GetValue(0,FIELDNAME_GEOCOLOR,pvar) )
	{
		long color = pGeo->GetColor();
		//bylayer
		if( color==-1 )
		{
			color = pLayer->GetColor();
		}
		color = (((DWORD)color)&0xffffff);
		value1.Format("%06X",color);

		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		if( !CUIFCheckListProp::StringFindItem(value0,value1) )
			return FALSE;
	}

	CArray<PT_3DEX,PT_3DEX> arrPts;	

	// 线型
	if( tab.GetValue(0,FIELDNAME_LINETYPE,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			pGeo->GetShape(arrPts);
			int flag[5] = { 0 };
			for (int i = 0; i < arrPts.GetSize(); i++)
			{
				int penCode = arrPts[i].pencode;
				if (pen3PArc == penCode)
					flag[0] = pen3PArc;
				else if (penSpline == penCode)
					flag[1] = penSpline;
				else if (penArc == penCode)
					flag[2] = penArc;
				else if (penStream == penCode)
					flag[3] = penStream;
				else if (penLine == penCode)
					flag[4] = penLine;
			}
			for (int j = 0; j < 5; j++)
			{
				if (flag[j]>0)
				{
					CString text = PencodeToText(flag[j]);

					value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
					if (CUIFCheckListProp::StringFindItem(value0, text))
						return TRUE;
				}
			}
			return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//全局线宽
	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINEWIDTH,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			float lw = 0;
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				lw = ((CGeoCurve*)pGeo)->m_fLineWidth;
			else
				lw = ((CGeoSurface*)pGeo)->m_fLineWidth;

			if( lw!=(float)(_variant_t)*pvar )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	//有向点方向
	if( tab.GetValue(0,FIELDNAME_GEOPOINT_ANGLE,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
		{
			double v = ((CGeoDirPoint*)pGeo)->GetDirection();

			v = v - floor(v/(360))*360;
			
			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
			double v0, v1;
			if( sscanf(value0,"%lf,%lf",&v0,&v1)==2 )
			{
				if( v>=v0 && v<=v1 );
				else
					return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	//平行线宽度
	if( tab.GetValue(0,FIELDNAME_GEOCURVE_WIDTH,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
		{
			double v = ((CGeoParallel*)pGeo)->GetWidth();

			v = fabs(v);
			
			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
			double v0, v1;
			if( sscanf(value0,"%lf,%lf",&v0,&v1)==2 )
			{
				if( v>=v0 && v<=v1 );
				else
					return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	//线型比例
	if( tab.GetValue(0,FIELDNAME_GEOCURVE_LINETYPESCALE,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		{
			float v = ((CGeoCurve*)pGeo)->m_fLinetypeScale;
			
			v = fabs(v);
			
			float v0 = (float)(_variant_t)*pvar;
			if( v0!=v )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//标示码
	if( tab.GetValue(0,FIELDNAME_FTRCODE,pvar) )
	{
		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		value1 = pFtr->GetCode();
		if( !CUIFCheckListProp::StringFindItem(value0,value1) )
			return FALSE;
	}

	//符号名
	if( tab.GetValue(0,FIELDNAME_SYMBOLNAME,pvar) )
	{
		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		value1 = pFtr->GetGeometry()->GetSymbolName();
		if( !CUIFCheckListProp::StringFindItem(value0,value1) )
			return FALSE;
	}

	//编组号
	if( tab.GetValue(0,FIELDNAME_FTRGROUPID,pvar) )
	{
		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		CUIntArray arrGroups;
		pFtr->GetObjectGroup(arrGroups);

		int nGroup = arrGroups.GetSize();
		if( nGroup<=0 )
			return FALSE;

		int err_group = 0;
		for( int i=0; i<nGroup; i++)
		{
			ObjectGroup *pog = pDS->GetObjectGroupByID(arrGroups[i]);
			if( !pog )
			{
				err_group++;
				continue;
			}

			if( !CUIFCheckListProp::StringFindItem(value0,pog->name) )
				return FALSE;
		}

		if( err_group>=nGroup )
			return FALSE;
	}

	//用途
	if( tab.GetValue(0,FIELDNAME_FTRPURPOSE,pvar) )
	{
		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		
		CString text = PurposeToText(pFtr->GetPurpose());
		
		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		if( !CUIFCheckListProp::StringFindItem(value0,text) )
			return FALSE;
	}

	//符号化标志
	if( tab.GetValue(0,FIELDNAME_SYMBOLIZEFLAG,pvar) )
	{
		long v = (long)(_variant_t)*pvar;
		
		if( pGeo->GetSymbolizeFlag()!=v )
			return FALSE;
	}

	//文字内容
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CONTENT,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			value1 = ((CGeoText*)pGeo)->GetText();
			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;

			if( !CUIFCheckListProp::StringFindItem(value0,value1,FALSE) )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
	//字体
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_FONT,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			TEXT_SETTINGS0 settings;
			((CGeoText*)pGeo)->GetSettings(&settings);
			value1 = settings.strFontName;
			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
			
			if( !CUIFCheckListProp::StringFindItem(value0,value1) )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//字体大小
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARHEIGHT,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			TEXT_SETTINGS0 settings;
			((CGeoText*)pGeo)->GetSettings(&settings);
			float hei = settings.fHeight;
			float hei0 = (float)(_variant_t)*pvar;
			
			if( hei0!=hei )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//对齐方式
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_ALIGNTYPE,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			TEXT_SETTINGS0 settings;
			((CGeoText*)pGeo)->GetSettings(&settings);

			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;			
			CString text;
			long lv = settings.nAlignment;
			AlignToString(lv,text,TRUE);
			
			if( !CUIFCheckListProp::StringFindItem(value0,text) )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//倾斜方式
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINE,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			TEXT_SETTINGS0 settings;
			((CGeoText*)pGeo)->GetSettings(&settings);
			
			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;			
			CString text;
			long lv = settings.nInclineType;
			InclinedTypeToString(lv,text,TRUE);
			
			if( !CUIFCheckListProp::StringFindItem(value0,text) )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//倾斜角度
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINEANGLE,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			TEXT_SETTINGS0 settings;
			((CGeoText*)pGeo)->GetSettings(&settings);
			
			float fv0 = (float)(_variant_t)*pvar;					
			
			if( settings.fInclineAngle!=fv0 )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//字宽倍率
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARWIDTHS,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			TEXT_SETTINGS0 settings;
			((CGeoText*)pGeo)->GetSettings(&settings);
			
			float fv0 = (float)(_variant_t)*pvar;					
			
			if( settings.fWidScale!=fv0 )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//等高地物
	if( tab.GetValue(0,FIELDNAME_EQUALZ,pvar) )
	{
		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;

		BOOL bEqualZ = CAutoLinkContourCommand::CheckObjForZ(pGeo);

		if( bEqualZ && value0.Compare(StrFromResID(IDS_YES))==0 );
		else if( !bEqualZ && value0.Compare(StrFromResID(IDS_YES))!=0 );
		else
			return FALSE;
	}

	//闭合
	if( tab.GetValue(0,FIELDNAME_CLOSED,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		{
			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
			BOOL bClosed = ((CGeoCurveBase*)pGeo)->IsClosed();

			if( bClosed && value0.Compare(StrFromResID(IDS_YES))==0 );
			else if( !bClosed && value0.Compare(StrFromResID(IDS_YES))!=0 );
			else
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//点列方向
	if( tab.GetValue(0,FIELDNAME_CLOCKWISE,pvar) )
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			if( arrPts.GetSize()==0 )
				pGeo->GetShape(arrPts);

			if( arrPts.GetSize()>2 )
			{
				value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;

				int ret = GraphAPI::GIsClockwise(arrPts.GetData(),arrPts.GetSize());

				if( ret==1 && value0.Compare(StrFromResID(IDS_COND_CLOCKWISE))==0 );
				else if( ret==0 && value0.Compare(StrFromResID(IDS_COND_CLOCKWISE))!=0 );
				else
					return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	//高程值
	if( tab.GetValue(0,FIELDNAME_HEIGHT,pvar) )
	{
		if( arrPts.GetSize()==0 )
			pGeo->GetShape(arrPts);

		double z0, z1;
		int npt = arrPts.GetSize(), init = 1;
		PT_3DEX *pts = arrPts.GetData();
		for(int i=0; i<npt; i++)
		{
			if( init )
			{
				z0 = z1 = pts[i].z;
				init = 0;
			}
			else
			{
				if( z0>pts[i].z )z0 = pts[i].z;
				if( z1<pts[i].z )z1 = pts[i].z;
			}
		}

		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		double v0, v1;
		if( sscanf(value0,"%lf,%lf",&v0,&v1)==2 )
		{
			if( z0>=v0 && z1<=v1 );
			else
				return FALSE;
		}
	}

	//节点数
	if( tab.GetValue(0,FIELDNAME_POINTNUM,pvar) )
	{
		if( arrPts.GetSize()==0 )
			pGeo->GetShape(arrPts);
		int npt = arrPts.GetSize();

		value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;

		int v0, v1;
		if( sscanf(value0,"%d,%d",&v0,&v1)==2 )
		{
			if( npt>=v0 && npt<=v1 );
			else
				return FALSE;
		}
	}

	//长度
	if( tab.GetValue(0,FIELDNAME_LENGTH,pvar) )
	{
		if( arrPts.GetSize()==0 )
			pGeo->GetShape(arrPts);
		
		double len = 0;
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				len = ((CGeoCurve*)pGeo)->GetLength();
			else
				len = ((CGeoSurface*)pGeo)->GetLength();

			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
			
			double v0, v1;
			if( sscanf(value0,"%lf,%lf",&v0,&v1)==2 )
			{
				if( len>=v0 && len<=v1 );
				else
					return FALSE;
			}
		}
		else
			return FALSE;
	}

	//面积
	if( tab.GetValue(0,FIELDNAME_AREA,pvar) )
	{
		if( arrPts.GetSize()==0 )
			pGeo->GetShape(arrPts);
		
		double v = 0;
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				v = ((CGeoCurve*)pGeo)->GetArea();
			else
				v = ((CGeoSurface*)pGeo)->GetArea();
			
			value0 = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
			
			double v0, v1;
			if( sscanf(value0,"%lf,%lf",&v0,&v1)==2 )
			{
				if( v>=v0 && v<=v1 );
				else
					return FALSE;
			}
		}
		else
			return FALSE;

	}

	//其他复杂条件
	if( tab.GetValue(0,FIELDNAME_OTHERCONDS,pvar) )
	{
		CondMenu *pMenu = (CondMenu*)(long)(_variant_t)*pvar;
		CArray<CONDSEL,CONDSEL> arrConds;
		arrConds.Copy(pMenu->arrConds);

		//基本属性
		CValueTable tab1;
		tab1.BeginAddValueItem();
		pFtr->WriteTo(tab1);
		tab1.EndAddValueItem();

		//扩展属性,在有需要时才读出
		CValueTable tab2, *pvt;
		BOOL bReadExtraFields = FALSE;

		const CVariantEx *pvar;

		//准备方案变量
		CConfigLibManager * pConfi = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		CScheme *pScheme = NULL;
		CSchemeLayerDefine *pLayDef = NULL;
		if(pConfi)
		{
			pScheme = pConfi->GetScheme(pDS->GetScale());
			if(pScheme)
			{
				pLayDef = pScheme->GetLayerDefine(pLayer->GetName());
			}
		}

		int nsz = arrBasicFields.GetSize();
		for( int i=0; i<arrConds.GetSize(); i++)
		{
			//该条件是否属于基本属性
			for( int j=0; j<nsz; j++)
			{
				if( arrBasicFields[j].CompareNoCase(arrConds[i].field)==0 )
					break;
			}

			//是基本属性
			if( j<nsz )
			{
				pvt = &tab1;
			}
			//扩展属性
			else
			{
				if( !bReadExtraFields )
				{
					tab2.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr, tab2);
					tab2.EndAddValueItem();

					bReadExtraFields = TRUE;					
				}
				pvt = &tab2;
			}

			CString strField = arrConds[i].field;
			if( bReadExtraFields && pLayDef!=NULL )
			{
				int idx = pLayDef->FindXDefineByName(strField);
				if( idx>=0 )strField = pLayDef->GetXDefine(idx).field;
			}

			if( pvt->GetValueByName(0,arrConds[i].field,pvar) )
			{
				if( stricmp(arrConds[i].field,StrFromResID(IDS_FIELDNAME_GEOCOVERTYPE))==0 )
				{
					CString strValue;
					long value = (long)(_variant_t)*pvar;
					CoverTypeToString(value,strValue,TRUE);
					if( strValue.CompareNoCase(arrConds[i].value)!=0 )
					{
						return FALSE;
					}
				}
				else if( stricmp(arrConds[i].field,StrFromResID(IDS_FIELDNAME_TEXT_PLACETYPE))==0 )
				{
					CString strValue;
					long value = (long)(_variant_t)*pvar;
					PlaceTypeToString(value,strValue,TRUE);
					if( strValue.CompareNoCase(arrConds[i].value)!=0 )
					{
						return FALSE;
					}
				}
				else if( !CompareValue(pvar,arrConds[i].value) )
					return FALSE;
			}
			else if( pvt->GetValue(0,arrConds[i].field,pvar) || pvt->GetValue(0,strField,pvar) )
			{
				if( !CompareValue(pvar,arrConds[i].value) )
					return FALSE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

extern BOOL CheckNameForLayerCode(CDlgDataSource *pDS, LPCTSTR name, LPCTSTR pstrCode);


void CDlgDoc::DoFilterSelect(CValueTable& tab)
{
	const CVariantEx *pvar;

	int nRange = 0;
	if( tab.GetValue(0,FIELDNAME_RANGE,pvar) )
	{
		nRange = (long)(_variant_t)*pvar;
	}

	BOOL bReverse = FALSE;
	if( tab.GetValue(0,FIELDNAME_REVERSE,pvar) )
	{
		bReverse = (long)(_variant_t)*pvar;
	}

	CArray<FTR_HANDLE,FTR_HANDLE> arrHandles;

	CDlgDataSource *pDS = GetDlgDataSource();
	CFtrLayer *pLayer;

	CStringArray basicFields;
	CStringArray basicNames;
	pDS->GetAllField(FALSE,basicFields,basicNames);

	CString layer_value;
	if( tab.GetValue(0,FIELDNAME_LAYERNAME,pvar) )
	{
		layer_value = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
	}

	//all map
	if( nRange==0 )
	{
		int nlay = pDS->GetFtrLayerCount();
		for( int i=0; i<nlay; i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if( !pLayer || pLayer->IsDeleted() || !pLayer->IsVisible() )
				continue;

			//层能过滤掉很多地物，所以放在FilterFeature之前处理
			if( !layer_value.IsEmpty() )
			{
				if( bReverse )
				{
					if( CheckNameForLayerCode(pDS,pLayer->GetName(),layer_value) )
						continue;
					else
						;
				}
				else
				{
					if( CheckNameForLayerCode(pDS,pLayer->GetName(),layer_value) )
						;
					else
						continue;
				}
			}

			int nObj = pLayer->GetObjectCount();
			for( int j=0; j<nObj; j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if( !pFtr )
					continue;

				if( bReverse )
				{
					if( FilterFeature(tab,pDS,pLayer,basicNames,pFtr) )
						continue;
				}
				else
				{
					if( !FilterFeature(tab,pDS,pLayer,basicNames,pFtr) )
						continue;
				}
				
				arrHandles.Add(FtrToHandle(pFtr));
			}
		}
	}
	// selection
	else
	{
		int num = 0;
		const FTR_HANDLE *ftrs = m_selection.GetSelectedObjs(num);
		if( num<=0 )return;

		for( int i=0; i<num; i++)
		{
			CFeature *pFtr = HandleToFtr(ftrs[i]);

			pLayer = pDS->GetFtrLayerOfObject(pFtr);
			if( !pLayer )continue;

			//层能过滤掉很多地物，所以放在FilterFeature之前处理
			if( !layer_value.IsEmpty() )
			{
				if( bReverse )
				{
					if( CheckNameForLayerCode(pDS,pLayer->GetName(),layer_value) )
						continue;
					else
						;
				}
				else
				{
					if( CheckNameForLayerCode(pDS,pLayer->GetName(),layer_value) )
						;
					else
						continue;
				}
			}

			if( bReverse )
			{
				if( FilterFeature(tab,pDS,pLayer,basicNames,pFtr) )
					continue;
			}
			else
			{
				if( !FilterFeature(tab,pDS,pLayer,basicNames,pFtr) )
					continue;
			}

			arrHandles.Add(FtrToHandle(pFtr));
		}
	}

	m_selection.DeselectAll();
	m_selection.SelectAll(arrHandles.GetData(),arrHandles.GetSize());
	
	OnSelectChanged();
}

void CDlgDoc::OnAnchorptAutoset() 
{
	m_bAutoSetAnchor = !m_bAutoSetAnchor;
	AfxGetApp()->WriteProfileInt("Config\\Document","AutoSetAnchor",m_bAutoSetAnchor);
	
}

void CDlgDoc::OnUpdateAnchorptAutoset(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAutoSetAnchor);
	
}


int FindColorOfVVT(COLORREF clr, BOOL bGo=TRUE)
{
	static int table[] = {
		128,128,128,
			0,0,255,
			0,255,128,
			128,255,255,
			255,0,128,
			128,0,128,
			128,128,0,
			255,128,64,
			192,192,192,
			0,128,255,
			0,255,0,
			0,255,255,
			255,0,0,
			255,0,255,
			255,255,0,
			240,240,240
	};
	
	if( bGo )
	{
		int r = GetRValue(clr), g = GetGValue(clr), b = GetBValue(clr), k = 0;
		long dis, min = -1;
		for( int i=0; i<sizeof(table)/sizeof(table[0]); i+=3 )
		{
			dis = (r-table[i])*(r-table[i]) + (g-table[i+1])*(g-table[i+1]) + (b-table[i+2])*(b-table[i+2]);
			if( min<0 || min>dis )
			{
				min = dis;
				k = i;
			}
		}
		return k/3;
	}
	else
	{
		if( clr>=0 && clr<sizeof(table)/(sizeof(int)*3) )
		{
			return RGB(table[clr*3],table[clr*3+1],table[clr*3+2]);
		}
		return RGB(255,255,255);
	}
}

int GetPenCode(int cd, BOOL bToVVT)
{
	int ret = 0;
	if (bToVVT)
	{
		switch (cd)
		{
		case penMove:
			ret = 10;
			break;
		case penLine:
			ret = 20;
			break;
		case penStream:
			ret = 40;
			break;
		case penArc:
		case pen3PArc:
			ret = 60;
			break;
		case penSpline:
			ret = 30;
			break;
		default:
			ret = 0;
			break;
		}
	}
	else
	{
		switch (cd)
		{
		case 10:
			ret = penMove;
			break;
		case 20:
			ret = penLine;
			break;
		case 40:
			ret = penStream;
			break;
		case 50:
		case 60:
			ret = penArc;
			break;
		case 30:
			ret = penSpline;
			break;
		default:
			ret = penNone;
			break;
		}
	}	

	return ret;
}

void CDlgDoc::OnWorkInputVvt() 
{
	CDlgImportVVT dlg;
	
	if( dlg.DoModal()!=IDOK )return;
	
	CString path = dlg.m_strFilePath;

	FILE *fp = fopen(path,"rt");
	if( !fp )
	{
		AfxMessageBox( IDS_FILE_OPEN_ERR );
		return;
	}

	char line[1024] = {0};
	fgets(line,sizeof(line),fp);

	if( stricmp(line,"FCode ObjNum PtSum Color {x,y,z,cd}\n")!=0 )
	{
		AfxMessageBox(IDS_INVALID_FILE);
		fclose(fp);
		return;
	}

	int pSum;
	int fcode;
	int objnum;
	int cbr;
	CString strLayName, strFtrID;


	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS)  return;
	CConfigLibManager * pConfi = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	if(!pConfi) return ;
	CScheme *pScheme = pConfi->GetScheme(GetDlgDataSource()->GetScale());
	if(!pScheme) return ;
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, 1000);
	BeginBatchUpdate();
	while( !feof(fp) ) 
	{
		if( fscanf(fp,"%d%d%d%d",&fcode,&objnum,&pSum,&cbr)!=4 )
			break;

		AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);

		long code = fcode;
		
		CFtrLayer *pLayer = NULL;

		if( fcode==8999 )
		{
			strLayName.LoadString(IDS_DEFLAYER_NAMET);
			pLayer = pDS->GetFtrLayer(strLayName/*,dlg.m_strMapName*/);
			if (pLayer == NULL)
			{				
				pLayer = pDS->CreateFtrLayer(strLayName);
				if (!pLayer)  continue;
				
				pDS->AddFtrLayer(pLayer);				
				
			}
			if (pLayer)
			{
				CFeature * pFtr = pLayer->CreateDefaultFeature(GetDlgDataSource()->GetScale());
				CGeoText *pGeo = (CGeoText *)(pFtr->GetGeometry());
			//	CGeoText *pGeo = new CGeoText;
				CArray<PT_3DEX,PT_3DEX> pts;
				PT_3DEX expt;
				for( int j=0;j<pSum;j++ )
				{
					fscanf( fp,"%lf %lf %lf %d",&expt.x,&expt.y,&expt.z,&expt.pencode );
					expt.pencode = GetPenCode(expt.pencode,FALSE);
					if( expt.pencode!=penLine && expt.pencode!=penArc && expt.pencode!=penSpline && expt.pencode!=penStream && expt.pencode!=pen3PArc )
						expt.pencode = penLine;
					
					pts.Add(expt);
				}			
				pGeo->CreateShape(pts.GetData(),pts.GetSize());
				char feature = getc(fp);
				while(feature=='\n') feature = getc(fp);
				if (feature=='~') 
				{
					int tmp,tmp1,tmp2,tmp3;
					fscanf(fp,"%d %d %d %d\n",&tmp,&tmp1,&tmp2,&tmp3);
					
					TEXT_SETTINGS0 tset;
					tset.nPlaceType = tmp;
					tset.nAlignment = tmp1;
					tset.nInclineType = tmp3;
					
					double f1,f2;
					fscanf(fp,"%lf %lf\n",&f1,&f2);
					tset.fInclineAngle = f1;
					
					fscanf(fp,"%lf %lf %lf %lf\n",&f1,&f1,&f1,&f2);
					tset.fHeight = f2;
					tset.fWidScale = 1.0;
					
					fscanf(fp,"%d \n",&tmp);
					pGeo->SetColor(FindColorOfVVT(tmp,FALSE));
					
					fscanf(fp,"%d %d",&tmp,&tmp1);
					fgets(line,sizeof(line),fp);
					fgets(line,sizeof(line),fp);
					if( tmp<sizeof(line) )
						line[tmp] = '\0';
					
					pGeo->SetText(line);
					pGeo->SetSettings(&tset);				
				}

				pFtr->SetCode(dlg.m_strMapName);

				if( pDS->IsFeatureValid(pFtr) )
				{
					AddObject(pFtr,pLayer->GetID());					
				}
				else
				{
					delete pFtr;
				}
			}			
		}
		else
		{
			int clsid = CLS_NULL;		
			strLayName.Format(_T("%ld"),code);
			if(pLayer = pDS->GetFtrLayer(strLayName/*,dlg.m_strMapName*/))
			{
				CFeature *pFtr = pLayer->CreateDefaultFeature(GetDlgDataSource()->GetScale(),clsid);
				CGeometry *pGeo = pFtr->GetGeometry();
				pGeo->SetColor(FindColorOfVVT(cbr,FALSE));
				CArray<PT_3DEX,PT_3DEX> pts;
				PT_3DEX expt;
				for( int j=0;j<pSum;j++ )
				{
					fscanf( fp,"%lf %lf %lf %d",&expt.x,&expt.y,&expt.z,&expt.pencode );
					expt.pencode = GetPenCode(expt.pencode,FALSE);
					if( expt.pencode!=penLine && expt.pencode!=penArc && expt.pencode!=penSpline && expt.pencode!=penStream && expt.pencode!=pen3PArc)
						expt.pencode = penLine;
					
					pts.Add(expt);
				}
				pGeo->CreateShape(pts.GetData(),pts.GetSize());
				pFtr->SetCode(dlg.m_strMapName);

				if( pDS->IsFeatureValid(pFtr) )
				{
					AddObject(pFtr,pLayer->GetID());					
				}
				else
				{
					delete pFtr;
				}						

			}
			else
			{
				if( pSum==1 )
				{
					strLayName.LoadString(IDS_DEFLAYER_NAMEP);
					clsid = CLS_GEOPOINT;
				}
				else
				{
					strLayName.LoadString(IDS_DEFLAYER_NAMEL);
					clsid = CLS_GEOCURVE;
				}
				pLayer = pDS->GetFtrLayer(strLayName/*,dlg.m_strMapName*/);
				if (pLayer == NULL)
				{	
					strLayName.Format(_T("%ld"),code);
					pLayer = pDS->CreateFtrLayer(strLayName);
					if (!pLayer)  continue;
					
					pDS->AddFtrLayer(pLayer);				
					
				}
				if(pLayer)
				{
					CFeature *pFtr = pLayer->CreateDefaultFeature(GetDlgDataSource()->GetScale(),clsid);
					CGeometry *pGeo = pFtr->GetGeometry();
					pGeo->SetColor(FindColorOfVVT(cbr,FALSE));
					CArray<PT_3DEX,PT_3DEX> pts;
					PT_3DEX expt;
					for( int j=0;j<pSum;j++ )
					{
						fscanf( fp,"%lf %lf %lf %d",&expt.x,&expt.y,&expt.z,&expt.pencode );
						expt.pencode = GetPenCode(expt.pencode,FALSE);
						if( expt.pencode!=penLine && expt.pencode!=penArc && expt.pencode!=penSpline && expt.pencode!=penStream && expt.pencode!=pen3PArc)
							expt.pencode = penLine;
						
						pts.Add(expt);
					}
					pGeo->CreateShape(pts.GetData(),pts.GetSize());
					pFtr->SetCode(dlg.m_strMapName);
					if( pDS->IsFeatureValid(pFtr) )
					{
						AddObject(pFtr,pLayer->GetID());					
					}
					else
					{
						delete pFtr;
					}
				}			
			}
		}

	}

	fclose(fp);
	EndBatchUpdate();

	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
	UpdateAllViews(NULL,hc_UpdateAllObjects);
}


#ifndef TRIAL_VERSION


void CDlgDoc::OnExportVVT()
{
	CString filter,title;
	filter.LoadString(IDS_LOADVVT_FILTER);
	
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, ".vvt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;

	CString path = dlg.GetPathName();
	
	FILE *fp = fopen(path,"w");
	FILE *fpEx = fopen(path.Left(path.GetLength()-4)+".ext","w");
	if( !fp || !fpEx )
	{
		if( fp )fclose(fp);
		if( fpEx )fclose(fpEx);
		return;
	}

	CDlgDataSource *pDS = GetDlgDataSource();

	CDataQuery *pDQ = GetDataQuery();
	if( !pDQ )return;

	////uvs///////////////////////////
	CAccessModify *pAccess = pDS->GetAccessObject();
	if (!pAccess) return;

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme)  return;

	//获取实体对象总数以及点的数目
	CGeometry *pGeo = NULL;
	long lObjSum = 0, lPtSum = 0;
	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		lObjSum += nObjs;
		nObjs = pLayer->GetObjectCount();
		for(int j=0; j<nObjs; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr) continue;
			pGeo = pFtr->GetGeometry();
			if( !pGeo )continue;
			
			lPtSum += pGeo->GetDataPointSum();
		}
	}
	
	fprintf( fp,"FCode ObjNum PtSum Color {x,y,z,cd}\n" );
    fprintf( fpEx,"IGS_XYZ_FILE_DATA_TEXT_FORMAT_EXTEND_FILE_Don't_edit_it\n");
    fprintf( fpEx,"FCode ObjNum PtSum Color {x,y,z,cd}\n" );
	
	GProgressStart(lObjSum);
	
	long number = 0;
//	POSITION pos = pDQ->GetFirstObjPos();
	CFeature *pFtr = NULL;
	long idx;
	pFtr = pAccess->GetFirstFeature(idx);
	while( pFtr )
	{
		GProgressStep();
//		pFtr = pDQ->GetNextObjByPos(pos);
		if(!pFtr->IsVisible() || pFtr->IsDeleted())
		{
			delete pFtr;
			pFtr = pAccess->GetNextFeature(idx);
			continue;
		}

// 		long info;
// 		if (!pDQ->GetObjectInfo(pFtr,1,info)) continue;
// 
// 		CFtrLayer *pLayer = (CFtrLayer*)info;

		int id = pAccess->GetCurFtrLayID();
		CFtrLayer *pLayer = pDS->GetFtrLayer(id);

		if (!pLayer)
		{
			delete pFtr;
			pFtr = pAccess->GetNextFeature(idx);
			continue;
		}

		__int64 code = 0;
		CString strName = pLayer->GetName();
		pScheme->FindLayerIdx(FALSE,code,strName);
		
		CGeometry *pObj = pFtr->GetGeometry();
		if (!pObj)
		{
			delete pFtr;
			pFtr = pAccess->GetNextFeature(idx);
			continue;
		}
		
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			code = 8999;
		
		//颜色			
		int color = pObj->GetColor();
		if (color == -1)
		{
			color = pLayer->GetColor();
		}
		
		color = FindColorOfVVT(color);
		
		//点数
		CArray<PT_3DEX,PT_3DEX> pts;
		pObj->GetShape(pts);
		
		int npt = pts.GetSize();
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || 
			pObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
			fprintf( fp,"%I64d %d %d %d\n",code,number,npt+1,color );
		else
			fprintf( fp,"%I64d %d %d %d\n",code,number,npt,color );
		
		PT_3DEX expt;
		for( int k=0; k<npt; k++)
		{
			expt = pts[k];
			fprintf(fp,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,
				k==0?0:GetPenCode(expt.pencode));
		}
		
		//平行线多一个点
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
		{
			expt = PT_3DEX(((CGeoParallel*)pObj)->GetCtrlPoint(0),70);
			fprintf(fp,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,expt.pencode);
		}
		else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
		{
			expt = PT_3DEX(((CGeoDirPoint*)pObj)->GetCtrlPoint(0),penLine);
			fprintf(fp,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,GetPenCode(expt.pencode));
		}
		
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
		{
			CGeometry *pObj1, *pObj2;
			if( ((CGeoParallel*)pObj)->Separate(pObj1,pObj2) )
			{
				if( pObj1 )
				{
					delete pObj1;
				}
				if( pObj2 )
				{
					CArray<PT_3DEX,PT_3DEX> pts1;
					pObj2->GetShape(pts1);
					npt = pts1.GetSize();
					fprintf( fpEx,"%I64d %d %d %d\n",code,number,npt,color );
					
					for( k=0; k<npt; k++)
					{
						expt = pts1[k];
						fprintf(fpEx,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,
							(k==0?0:GetPenCode(expt.pencode)));
					}
					delete pObj2;
				}
			}
		}
		else
		{
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
			{
				fprintf( fpEx,"%I64d %d %d %d\n",code,number,-1,color );
			}
			else
			{
				fprintf( fpEx,"%I64d %d %d %d\n",code,number,0,color );
			}
		}
		
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			CGeoText *pText = (CGeoText*)pObj;
			TEXT_SETTINGS0 tset;
			pText->GetSettings(&tset);
			
			fprintf(fp,"~\n");
			fprintf(fp,"%d %d %d %d\n",tset.nPlaceType,
				tset.nAlignment,
				0,
				tset.nInclineType );
			fprintf(fp,"%.3f %.3f\n",tset.fInclineAngle,0.0f);
			fprintf(fp,"%.3f %.3f %.3f %.3f\n",0.0f,
				0.0f,
				0.0f,
				tset.fHeight );
			fprintf(fp,"%d \n",color);
			
			CString text = pText->GetText();
			
			fprintf(fp,"%d %d\n",text.GetLength(),0);
			fprintf(fp,"%s\n",text);
		}
		
		number++;

		delete pFtr;
		pFtr = pAccess->GetNextFeature(idx);
		
	}

	//遍历所有层
/*	for (i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer || !pLayer->IsVisible() )continue;
		
		long code = 0;
		CString strName = pLayer->GetName();
		pScheme->FindLayerIdx(FALSE,code,strName);

		int nObjs = pLayer->GetObjectCount();

		for(int j=0; j<nObjs; j++)
		{
			GProgressStep();

			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr) continue;

			CGeometry *pObj = pFtr->GetGeometry();
			if (!pObj) continue;

			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
				code = 8999;

			//颜色			
			int color = pObj->GetColor();
			if (color == -1)
			{
				color = pLayer->GetColor();
			}

			color = FindColorOfVVT(color);
			
			//点数
			CArray<PT_3DEX,PT_3DEX> pts;
			pObj->GetShape(pts);

			int npt = pts.GetSize();
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || 
				pObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
				fprintf( fp,"%d %d %d %d\n",code,number,npt+1,color );
			else
				fprintf( fp,"%d %d %d %d\n",code,number,npt,color );

			PT_3DEX expt;
			for( int k=0; k<npt; k++)
			{
				expt = pts[k];
				fprintf(fp,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,
					k==0?0:GetPenCode(expt.pencode));
			}

			//平行线多一个点
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
			{
				expt = PT_3DEX(((CGeoParallel*)pObj)->GetCtrlPoint(0),70);
				fprintf(fp,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,expt.pencode);
			}
			else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
			{
				expt = PT_3DEX(((CGeoDirPoint*)pObj)->GetCtrlPoint(0),penLine);
				fprintf(fp,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,GetPenCode(expt.pencode));
			}

			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
			{
				CGeometry *pObj1, *pObj2;
				if( ((CGeoParallel*)pObj)->Separate(pObj1,pObj2) )
				{
					if( pObj1 )
					{
						delete pObj1;
					}
					if( pObj2 )
					{
						CArray<PT_3DEX,PT_3DEX> pts1;
						pObj2->GetShape(pts1);
						npt = pts1.GetSize();
						fprintf( fpEx,"%d %d %d %d\n",code,number,npt,color );
						
						for( k=0; k<npt; k++)
						{
							expt = pts1[k];
							fprintf(fpEx,"%.4f %.4f %.4f %d\n",expt.x,expt.y,expt.z,
								(k==0?0:GetPenCode(expt.pencode)));
						}
						delete pObj2;
					}
				}
			}
			else
			{
				if( pObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
				{
					fprintf( fpEx,"%d %d %d %d\n",code,number,-1,color );
				}
				else
				{
					fprintf( fpEx,"%d %d %d %d\n",code,number,0,color );
				}
			}

			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				CGeoText *pText = (CGeoText*)pObj;
				TEXT_SETTINGS0 tset;
				pText->GetSettings(&tset);

				fprintf(fp,"~\n");
				fprintf(fp,"%d %d %d %d\n",tset.nPlaceType,
					tset.nAlignment,
					0,
					tset.nInclineType );
				fprintf(fp,"%.3f %.3f\n",tset.fInclineAngle,0.0f);
				fprintf(fp,"%.3f %.3f %.3f %.3f\n",0.0f,
					0.0f,
					0.0f,
					tset.fHeight );
				fprintf(fp,"%d \n",color);

				CString text = pText->GetText();

				fprintf(fp,"%d %d\n",text.GetLength(),0);
				fprintf(fp,"%s\n",text);
			}

			number++;
		}
	}*/
	
	fclose(fp);
	fclose(fpEx);
	
	//进度条复位
	GProgressEnd();
}


#endif

void CDlgDoc::OnUpdateOption()
{
	CDrawingContext::m_bTextClear =  AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_TEXTCLEAR,FALSE);
	m_bPlaceProperties = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_PLACEPROPERTIES,FALSE);
	m_bRCLKSwitch = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RCLKSWITCH,m_bRCLKSwitch);
	m_bCADStyle = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_CADSTYLE,m_bCADStyle);
	m_bFocusColletion = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_FOCUSCOL,FALSE);

	GraphAPI::g_lfDisTolerance = GetProfileDouble(REGPATH_PRECISION,REGITEM_XYPRECISION,0.0001);
	GraphAPI::g_lfZTolerance = GetProfileDouble(REGPATH_PRECISION,REGITEM_ZPRECISION,0.0001);	

	m_bObjectGroup = AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_OBJECTGROUP,TRUE);

	UpdateGlobalSymbolizeParams();

	UpdateAllViews(NULL,hc_UpdateOption);
	CDlgDataSource *pDS = GetDlgDataSource();
	if( pDS )
	{
		BOOL bEnable = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_SYMCOLORTYPE,TRUE);
//		pDS->m_SymbolLib.EnableColorByObj(bEnable);
		CLinearizer::m_fPrecision = GetProfileDouble(REGPATH_SYMBOL,REGITEM_ARCTOLER,0.05)*0.001*pDS->GetScale();
		SetSymbolDrawScale(GetProfileDouble(REGPATH_SYMBOL,REGITEM_DRAWSCALE,1));
		SetSymbolAnnoScale(GetProfileDouble(REGPATH_SYMBOL,REGITEM_ANNOSCALE,1));
	}

	m_snap.UpdateSettings(FALSE);
	m_selection.UpdateSettings(FALSE);
}

void CDlgDoc::OnQueryMenu(UINT id)
{
	CQueryMenu *pMenu = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETQUERYMENU,0,(LPARAM)&pMenu);
	if( !pMenu )return;
	
	CArray<CONDSEL,CONDSEL> arrConds;
	pMenu->GetQuery(id,arrConds);
	
	// 基本属性
	CStringArray strField,strName;
	
	GetDlgDataSource()->GetAllField(TRUE,strField,strName);

	// 层名
	strField.Add(FIELDNAME_LAYERNAME);
	strName.Add(StrFromResID(IDS_LAYER_NAME));
	
	int fdnum = strField.GetSize();
	//添加扩展条件选项
	int nccs = 50;
	CONDCOMP ccs[50];
	FillCompareInfo(ccs,fdnum,nccs);
	
	for (int i=0; i<nccs; i++)
	{
		strName.Add(StrFromResID(ccs[i].strid));
	}
	
	CCPResultList result;
	
	QueryCond(GetDlgDataSource(),arrConds,fdnum,strField,strName,result);
	
	AfxGetMainWnd()->SendMessage(FCCM_SEARCHRESULT,2/*dlg.m_nApply*/,(LPARAM)&result);
	
	OnSelectChanged();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESS_END));
}

void CDlgDoc::OnSaveTimer()
{
	int bAutoSave = AfxGetApp()->GetProfileInt(REGPATH_SAVE,REGITEM_AUTOSAVE,TRUE);
	int nSaveTime = AfxGetApp()->GetProfileInt(REGPATH_SAVE,REGITEM_SAVETIME,60);
	if( !bAutoSave || nSaveTime<=0 || m_nLastSaveTime==0 )return;
	
	CDlgDataSource *pDataSource = GetDlgDataSource();

	if( GetTickCount()-m_nLastSaveTime>=nSaveTime*60000 )
	{	
		if (pDataSource && UVSModify == GetAccessType(pDataSource))
		{
			CTime time = CTime::GetCurrentTime();
			CString str = time.Format("%Y-%m-%d %H:%M:%S");
			GOutPut(str);

			//OnFileSave();

			SaveBakFiles(BakTypeUVS);

			//GOutPut(StrFromResID(IDS_DOC_AUTOSAVE_END));

			/*pDataSource->SetModifiedFlag(FALSE);*/

			m_nLastSaveTime = GetTickCount();
		}
		else if( pDataSource && pDataSource->IsModified() )
		{			
			//GOutPut(StrFromResID(IDS_DOC_AUTOSAVE));
			
			CTime time = CTime::GetCurrentTime();
			CString str = time.Format("%Y-%m-%d %H:%M:%S");
			GOutPut(str);
				
			//OnFileSave();

			SaveBakFiles(BakType12);
			
			//GOutPut(StrFromResID(IDS_DOC_AUTOSAVE_END));
			
			/*pDataSource->SetModifiedFlag(FALSE);*/

			m_nLastSaveTime = GetTickCount();
		}
	}
}


BOOL CDlgDoc::LoadLayerConfig(int nScale)
{
	int scale;
	if (nScale==0)
	{
		scale = GetDlgDataSource()->GetScale();
	}
	else
		scale = nScale;

	CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	if(!pCfgLibManager) return FALSE;
	CScheme *pScheme = pCfgLibManager->GetScheme(scale);
	if(!pScheme) return FALSE;
	int nLayerCnt = pScheme->GetLayerDefineCount();
	CSchemeLayerDefine *pLayerDef = NULL;

	BeginBatchUpdate();

	for(int i=0;i<nLayerCnt;i++)
	{
		pLayerDef = pScheme->GetLayerDefine(i);
		CFtrLayer *pLayer = new CFtrLayer;
		if (!pLayer)
		{
			continue;
		}
		CValueTable tab;
		_variant_t var;
		tab.BeginAddValueItem();
		
		var = (long)(pLayerDef->GetColor());
		tab.AddValue(FIELDNAME_LAYCOLOR,&CVariantEx(var));
		var = (_bstr_t)(LPCTSTR)pLayerDef->GetLayerName();
		tab.AddValue(FIELDNAME_LAYERNAME,&CVariantEx(var));
// 		var = (_bstr_t)(LPCTSTR)pLayerDef->GetLayerName();
// 		tab.AddValue(FIELDNAME_LAYERNAME,&CVariantEx(var));
		tab.EndAddValueItem();

		pLayer->ReadFrom(tab);
		GetDlgDataSource()->AddFtrLayer(pLayer);
	}

	EndBatchUpdate();

	return TRUE;
}

CView* CDlgDoc::GetCurActiveView()
{	
	BOOL bDoubleScreen	= AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, gdef_bDoubleScreen);
	
	CView *pView = NULL;
	if (!bDoubleScreen)
	{
		CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		
		// Get the active MDI child window.
		CMDIChildWnd *pChild = 
			(CMDIChildWnd *) pFrame->GetActiveFrame();
		
		// or CMDIChildWnd *pChild = pFrame->MDIGetActive();
		
		// Get the active view attached to the active MDI child
		// window.
		pView = pChild->GetActiveView();
	}
	else
	{
		POSITION DocTempPos=AfxGetApp()->GetFirstDocTemplatePosition();
		while(DocTempPos!=NULL)
		{
			CDocTemplate* curTemplate=AfxGetApp()->GetNextDocTemplate(DocTempPos);
			POSITION DocPos=curTemplate->GetFirstDocPosition();
			while(DocPos!=NULL)
			{
				CDocument *pDoc=curTemplate->GetNextDoc(DocPos);
				if (pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)))
				{
					POSITION ViewPos=pDoc->GetFirstViewPosition();
					while(ViewPos)
					{
						pView = pDoc->GetNextView(ViewPos);
						if(pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
						{
							CFrameWnd *pFrame=pView->GetParentFrame();
							if (pFrame->IsKindOf(RUNTIME_CLASS(CStereoFrame)))
							{
								goto end ;
							}
						}
						
					}
				}
			}
			
		}
		
	}
end:
	
	return pView;
}

void CDlgDoc::OnSetDefaultCommand()
{
	// 设置当前特征缺省命令
	CFtrLayer *pFtrLayer = GetDlgDataSource()->GetCurFtrLayer();
	
	long CmdID = GetCurrentCommandId();
	CCommand *pCmd = GetCurrentCommand();
	if( !pFtrLayer || !pCmd || CmdID==0)return;

	CPlaceConfigLib *pPlaceConfigLib = gpCfgLibMan->GetPlaceConfigLib(GetDlgDataSource()->GetScale());
	if (pPlaceConfigLib)
	{
		// CmdParams
		CValueTable tab;
		tab.BeginAddValueItem();
		pCmd->GetParams(tab);
		tab.EndAddValueItem();
		
		CMarkup xml;
		xml.AddElem("Data");
		xml.IntoElem();
		Xml_WriteValueTable(xml,tab);
		xml.OutOfElem();
		
		CString cmdParams = xml.GetDoc();	
		
		// ViewParams
		char viewParams[256]={0};
		UpdateAllViewsParams(1,(LPARAM)viewParams);
// 		CView *pView = GetCurActiveView();
// 		if (pView) pView->SendMessage(FCCM_UPDATESTEREOPARAM,1,(LPARAM)viewParams);


		CPlaceConfig place;
		strcpy(place.m_strLayName,pFtrLayer->GetName());
		place.m_nCommandID = CmdID;
		strcpy(place.m_strCmdParams,cmdParams);
		strcpy(place.m_strViewParams,viewParams);

//		pPlaceConfigLib->SetConfig(place.m_strLayName,place);
		CPlaceConfig* pConfig = pPlaceConfigLib->GetConfig(place.m_strLayName);
		if (pConfig)
		{
			*pConfig = place;
		}
		else
			pPlaceConfigLib->AddConfig(place);
		
		pPlaceConfigLib->Save();

	}
	
	if (pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
		// 如果是采集命令则自动设置采集特征
	{
		DrawingInfo Info;
		Info = ((CDrawCommand*)pCmd)->GetCurDrawingInfo();
		CFeature *pFtr = Info.GetDrawFtr();
		CValueTable tab;
		if (pFtr)
		{
			tab.BeginAddValueItem();
			pFtr->WriteTo(tab);
			tab.EndAddValueItem();
			tab.DelField(FIELDNAME_SHAPE);
			tab.DelField(FIELDNAME_FTRID);
			tab.DelField(FIELDNAME_GEOCLASS);
		}		

		CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
		if (pScheme)
		{
			CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(pFtrLayer->GetName());
			if (pLayer)
			{
				pLayer->SetDefaultValues(tab);//扩展属性是否可在此更改进方案（可以，但只能更改扩展属性的缺省值）
				pScheme->Save();
			}

		}
	}
}

void CDlgDoc::TrimDem(LPCTSTR demPath, PT_3D *polys, int nPt)
{
// 	if( m_strDemPath.IsEmpty() )
// 		return;
	CPtrArray arrLayers;
	if(GetDlgDataSource()->GetFtrLayer(_T("DEMPoints"),NULL,&arrLayers)==NULL)
		return;
	CFtrLayer *pLayer = NULL;
	double demDx = 0;
	CFeature *pFtr = NULL;
	Envelope eDempt;
	BOOL bFlag = FALSE;
	for (int i=0;i<arrLayers.GetSize();i++)
	{
		pLayer = (CFtrLayer*)arrLayers[i];
		if(pLayer->IsEmpty())
			continue;
		for (int j=0;j<pLayer->GetObjectCount();j++)
		{
			pFtr = pLayer->GetObject(j);
			if(pFtr==NULL)continue;
			if(!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)))
			{
				pFtr = NULL;
				continue;
			}
			eDempt.Union(&pFtr->GetGeometry()->GetEnvelope());
			if (bFlag==FALSE&&((CGeoDemPoint*)(pFtr->GetGeometry()))->GetDataPointSum()>50&&((CGeoDemPoint*)(pFtr->GetGeometry()))->CalcuDemDot())
			{
				bFlag = TRUE;
				demDx = ((CGeoDemPoint*)(pFtr->GetGeometry()))->CalcuDemDot();				
			}		
		}	
	}
	if (bFlag==FALSE)
	{
		return;
	}
	//生成范围
	Envelope e;
	e.CreateFromPts(polys,nPt);
	
	e.Intersect(&eDempt);

	CString str(demPath);
	if(str.Right(4).CompareNoCase(_T(".dem"))!=0)
		str+=_T(".dem");
	
	//内插DEM	
	DEMHEADERINFO demInfo;
	demInfo.lfStartX = ceil(e.m_xl/demDx)*demDx; 
	demInfo.lfStartY = ceil(e.m_yl/demDx)*demDx; 
	demInfo.lfDx = demDx; 
	demInfo.lfDy = demDx;
	demInfo.nRow = floor(e.m_yh/demDx)-ceil(e.m_yl/demDx) + 1;
	demInfo.nColumn = floor(e.m_xh/demDx)-ceil(e.m_xl/demDx) + 1;
	demInfo.nType = CDSM::DSM_NSDTF;
	demInfo.strFilePath = str;
	demInfo.nScale = 1000;
	demInfo.lfNoData = -99999;

	if( demInfo.nRow<=0 || demInfo.nColumn<=0 )
	{
		return;
	}
	
	float *pDEMZ = new float[demInfo.nRow*demInfo.nColumn];
	if( !pDEMZ )return;
	for (i=demInfo.nRow*demInfo.nColumn-1;i>=0;i--)
	{
		pDEMZ[i] = -99999;
	}

	PT_3D *ptsNew = new PT_3D[nPt];
	if( !ptsNew )
	{
		delete[] pDEMZ;
		return;
	}

	if( GraphAPI::GIsClockwise(polys,nPt) )
		GraphAPI::GGetParallelLine(polys,nPt,demDx,ptsNew);
	else
		GraphAPI::GGetParallelLine(polys,nPt,-demDx,ptsNew);

	PT_3D pt3d;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int nR,nC;
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_BUILDDEM_START));
	for (i=0;i<arrLayers.GetSize();i++)
	{
		pLayer = (CFtrLayer*)arrLayers[i];
		if(pLayer->IsEmpty())
			continue;
		for (int j=0;j<pLayer->GetObjectCount();j++)
		{
			pFtr = pLayer->GetObject(j);
			if(pFtr==NULL)continue;
			if(!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)))
			{
				pFtr = NULL;
				continue;
			}
			pFtr->GetGeometry()->GetShape(arrPts);
			int nSz = arrPts.GetSize();
			for (int k=0;k<nSz;k++)
			{
				COPY_3DPT(pt3d,arrPts[k]);
				nC = (pt3d.x - demInfo.lfStartX)/demInfo.lfDx;
				nR = (pt3d.y - demInfo.lfStartY)/demInfo.lfDy;

				if( nC>=0 && nC<demInfo.nColumn && nR>=0 && nR<demInfo.nRow )
				{
					int nRet = GraphAPI::GIsPtInRegion(pt3d,ptsNew,nPt);
					if( nRet==1 || nRet==2 )
					{
						pDEMZ[nR*demInfo.nColumn+nC] = pt3d.z;
					}
				}				
				
			}
					
		}	
	}
// 	
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_BUILDDEM_SAVE));
	
	//保存DEM
	CDSM dem;
	dem.Save(demInfo.strFilePath,demInfo.nType,&demInfo,pDEMZ);
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END));
	
	delete[] pDEMZ;
	delete[] ptsNew;
}

void CDlgDoc::SilenceToDxf(LPCTSTR strDxf)
{
	CDlgExportDxf dlg;
	{
		dlg.m_strDxfFile = GetPathName();
		int pos = dlg.m_strDxfFile.ReverseFind(_T('.'));
		if( pos>=0 )dlg.m_strDxfFile.Delete(pos,dlg.m_strDxfFile.GetLength()-pos);

		dlg.m_strDxfFile += _T("@@@.dxf");
	}

	CString pathName = dlg.m_strDxfFile;
	CDxfWrite dxfWrite;
	dxfWrite.SetAsExportBaselines();
	dxfWrite.SilenceExportDxf(this,pathName);
}

void CDlgDoc::LoadDemPoints(CDSM *pDem)
{
	DEMHEADERINFO dh;
	dh = pDem->m_demInfo;

	CFtrLayer *pLayer = NULL;
	CDlgDataSource *pDS = GetDlgDataSource();
	if(!pDS) return;
	if((pLayer = pDS->GetFtrLayer((LPCTSTR)"DEMPoints"))==NULL)
	{
		pLayer = new CFtrLayer;
		if( !pLayer )return;
		pLayer->SetName((LPCTSTR)"DEMPoints");
		if(!AddFtrLayer(pLayer)) return;
	}	
	
	//构建三角网，并显示图形
	PT_3DEX expt;
	expt.pencode = penLine;
	
	COLORREF clrs[] = { RGB(255,0,0),RGB(0,255,0) };
	
	double sx = dh.lfStartX, sy = dh.lfStartY;
	int i, j, sum = dh.nColumn*dh.nRow;

	BeginBatchUpdate();
	
	GProgressStart(sum);
	
	CArray<PT_3DEX,PT_3DEX> pts;
	CFeature *pFtr = NULL;
	{
		for( i=0; i<dh.nColumn; i++)
		{
			for( j=0; j<dh.nRow; j++)
			{
				GProgressStep();

				if( pFtr==NULL || pts.GetSize()>=1000 )
				{
					if( pFtr!=NULL )
					{
						pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
						AddObject(pFtr,pLayer->GetID());
						pts.RemoveAll();
						pFtr = NULL;
					}
					
// 					CGeoDataExchange *pGDX = NULL;
// 					pGDX = pLayer->CreateGDX(NULL,TRUE);
// 					if( pGDX )
// 					{
// 						CDpDBVariant var;
// 						var = (LPCTSTR)"DEMPoints";
// 						DpCustomFieldType dptype = DP_CFT_VARCHAR;
// 						pGDX->SetAttrValue("FID",var,dptype);
// 						
// 						var = (bool)true;
// 						dptype = DP_CFT_BOOL;
// 						pGDX->SetAttrValue("VISI",var,dptype);
// 						
// 					}
					
					//创建 Geometry 对象
					pFtr = pLayer->CreateDefaultFeature(GetDlgDataSource()->GetScale(),CLS_GEODEMPOINT);
					if( !pFtr )continue;					
				
					pFtr->GetGeometry()->SetColor(RGB(255,0,0));
				}
				expt.x = sx + i*dh.lfDx; 
				float fZ;
				if( (i%2)==0 )
				{
					expt.y = sy + j*dh.lfDy;
					pDem->GetZ( i,j,&fZ );expt.z = fZ;
				}
				else
				{
					expt.y = sy + (dh.nRow-1-j)*dh.lfDy;
					pDem->GetZ( i,(dh.nRow-1-j),&fZ );expt.z = fZ;
				}
				
				if( expt.z>-9997 )
				{
					pts.Add(expt);
				}
			}
		}
	}
	
	if( pFtr!=NULL && pts.GetSize()>0 )
	{
		pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
		AddObject(pFtr,pLayer->GetID());
	}
	
	GProgressEnd();

	EndBatchUpdate();
}


void CDlgDoc::OnUpdateSetDefaultCommand(CCmdUI* pCmdUI)
{
	int id = GetCurrentCommandId();
	if( id>=ID_ELE_START && id<=ID_ELE_END )
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		return;
	}
	
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(0);
}

void CDlgDoc::OnDummySelectFtrClass() 
{
	CMFCToolBarComboBoxButton* pSrcCombo = NULL;
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRCLASS, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); 
		pSrcCombo == NULL && posCombo != NULL;)
		{
			CMFCToolBarComboBoxButton* pCombo =
				DYNAMIC_DOWNCAST(CMFCToolBarComboBoxButton, listButtons.GetNext(posCombo));
			
			if (pCombo != NULL && 
				CMFCToolBar::IsLastCommandFromButton (pCombo))
			{
				pSrcCombo = pCombo;
			}
		}
	}
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);
		
		LPCTSTR lpszSelItem = pSrcCombo->GetItem ();
		CString strSelItem = _T("");
		int code;
		if( lpszSelItem )
		{
			
			sscanf(lpszSelItem,"%s (%d)",strSelItem.GetBuffer(strlen(lpszSelItem)+1),&code);
			strSelItem.ReleaseBuffer();
		}

		if (ActiveLayer(strSelItem) < 0)
			AfxMessageBox(IDS_DOC_NOTFOUND_FID);
	}
	else
	{
		AfxMessageBox(IDS_RESOURCE_ERR);
	}
}

void CDlgDoc::OnUpdateDummySelectFtrClass(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

void CDlgDoc::OnDummySelectLayer() 
{
	CUIFToolbarLayerComboBoxButton* pSrcCombo = NULL;
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_LAYER, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); 
		pSrcCombo == NULL && posCombo != NULL;)
		{
			CUIFToolbarLayerComboBoxButton* pCombo = 
				DYNAMIC_DOWNCAST (CUIFToolbarLayerComboBoxButton, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL && 
				CMFCToolBar::IsLastCommandFromButton (pCombo))
			{
				pSrcCombo = pCombo;
			}
		}
	}
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);
		
		if (m_pSelEx)
		{
			if (m_pSelEx->GetObjectCount() <= 0) return;

			CString lpszSelItem = pSrcCombo->GetItem ();
			
			int idx = lpszSelItem.Find('(');
			if (idx < 0) return;
			
			CString strLayerName = lpszSelItem.Left(idx);
			
			if(AfxMessageBox(StrFromResID(IDS_CMDTIP_CHANGELAYERNAME),MB_OKCANCEL)==IDCANCEL)
			{
				return;
			}

			m_pSelEx->OnModifyLayer(strLayerName);
		}
	}
	else
	{
		AfxMessageBox(IDS_RESOURCE_ERR);
	}
}

void CDlgDoc::OnUpdateDummySelectLayer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

void CDlgDoc::OnDummySelectLayerGroup() 
{
	return;
	CUIFToolbarLayerGroupComboBoxButton* pSrcCombo = NULL;
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_LAYERGROUP, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); 
		pSrcCombo == NULL && posCombo != NULL;)
		{
			CUIFToolbarLayerGroupComboBoxButton* pCombo = 
				DYNAMIC_DOWNCAST (CUIFToolbarLayerGroupComboBoxButton, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL && 
				CMFCToolBar::IsLastCommandFromButton (pCombo))
			{
				pSrcCombo = pCombo;
			}
		}
	}
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);
		
		if (m_pSelEx)
		{
			if (m_pSelEx->GetObjectCount() <= 0) return;
			
			CString strGroupName = pSrcCombo->GetItem ();
			
			if(AfxMessageBox(StrFromResID(IDS_CMDTIP_CHANGELAYERGROUPNAME),MB_OKCANCEL)==IDCANCEL)
			{
				return;
			}
			
			m_pSelEx->OnModifyLayerGroup(strGroupName);
		}
	}
	else
	{
		AfxMessageBox(IDS_RESOURCE_ERR);
	}
}

void CDlgDoc::OnUpdateDummySelectLayerGroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

void CDlgDoc::OnDummyModifyFtrColor()
{
	CUIFColorToolBarButton* pSrcCombo = NULL;
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRCOLOR, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); 
		pSrcCombo == NULL && posCombo != NULL;)
		{
			CUIFColorToolBarButton* pCombo = 
				DYNAMIC_DOWNCAST (CUIFColorToolBarButton, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL /*&& 
				CMFCToolBar::IsLastCommandFromButton (pCombo)*/)
			{
				pSrcCombo = pCombo;
			}
		}
	}
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);
		
		COLORREF col = pSrcCombo->GetColor();
		
		_variant_t var = (long)col;

		CValueTable tab;

		// 修改选择地物
		if (m_pSelEx)
		{
			for( int i=0; i<m_pSelEx->GetObjectCount(); i++)
			{
				tab.BeginAddValueItem();
				tab.AddValue(FIELDNAME_GEOCOLOR,&CVariantEx(var),StrFromLocalResID(IDS_FIELDNAME_COLOR));
				tab.EndAddValueItem();
			}
		}
		// 修改绘制地物
		else
		{
			tab.BeginAddValueItem();
			tab.AddValue(FIELDNAME_GEOCOLOR,&CVariantEx(var),StrFromLocalResID(IDS_FIELDNAME_COLOR));
			tab.EndAddValueItem();
		}

		AfxGetMainWnd()->SendMessage(FCCM_UPDATE_PROPERTY_UI,0,(LPARAM)(&tab));
		
	}
	else
	{
		AfxMessageBox(IDS_RESOURCE_ERR);
	}
}

void CDlgDoc::OnDummySelectFtrWidth() 
{
	CUIFToolbarFtrWidthComboBoxButton* pSrcCombo = NULL;
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRWIDTH, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); 
		pSrcCombo == NULL && posCombo != NULL;)
		{
			CUIFToolbarFtrWidthComboBoxButton* pCombo = 
				DYNAMIC_DOWNCAST (CUIFToolbarFtrWidthComboBoxButton, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL && 
				CMFCToolBar::IsLastCommandFromButton (pCombo))
			{
				pSrcCombo = pCombo;
			}
		}
	}
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);
		
		int iSel = pSrcCombo->GetCurSel();
		float wid = *(float*)pSrcCombo->GetItemData(iSel);
		
// 		if (m_pSelEx)
// 		{
// 			m_pSelEx->OnModifyFtrWid(wid);
// 		}

		_variant_t var = (float)wid;
		CValueTable tab;

		if (m_pSelEx)
		{
			for( int i=0; i<m_pSelEx->GetObjectCount(); i++)
			{
				tab.BeginAddValueItem();
				tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTH,&CVariantEx(var),StrFromLocalResID(IDS_FIELDNAME_GEOWIDTH));
				tab.EndAddValueItem();
			}
		}
		else
		{
			tab.BeginAddValueItem();
			tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTH,&CVariantEx(var),StrFromLocalResID(IDS_FIELDNAME_GEOWIDTH));
			tab.EndAddValueItem();
		}		

		AfxGetMainWnd()->SendMessage(FCCM_UPDATE_PROPERTY_UI,0,(LPARAM)(&tab));
		
	}
	else
	{
		AfxMessageBox(IDS_RESOURCE_ERR);
	}
}

void CDlgDoc::OnUpdateDummySelectFtrWidth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

void CDlgDoc::OnDummySelectFtrSym() 
{
	CUIFToolbarFtrSymComboBoxButton* pSrcCombo = NULL;
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRSYM, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); 
		pSrcCombo == NULL && posCombo != NULL;)
		{
			CUIFToolbarFtrSymComboBoxButton* pCombo = 
				DYNAMIC_DOWNCAST (CUIFToolbarFtrSymComboBoxButton, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL && 
				CMFCToolBar::IsLastCommandFromButton (pCombo))
			{
				pSrcCombo = pCombo;
			}
		}
	}
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);
		
		int iSel = pSrcCombo->GetCurSel();
		CString str = pSrcCombo->GetItem(iSel);
		if (str.CompareNoCase(_T("ByLayer")) == 0) 
		{
			str.Empty();
		}
		else	
		{
			str.Insert(0,"@");
		}		
		
		_variant_t var = (_variant_t)(const char*)(LPCTSTR)str;

		CValueTable tab;

		// 修改选择地物
		if (m_pSelEx)
		{
			for( int i=0; i<m_pSelEx->GetObjectCount(); i++)
			{
				tab.BeginAddValueItem();
				tab.AddValue(FIELDNAME_SYMBOLNAME,&CVariantEx(var),StrFromLocalResID(IDS_FIELDNAME_GEOSYMBOLNAME));
				tab.EndAddValueItem();
			}
		}
		// 修改绘制地物
		else
		{
			tab.BeginAddValueItem();
			tab.AddValue(FIELDNAME_SYMBOLNAME,&CVariantEx(var),StrFromLocalResID(IDS_FIELDNAME_GEOSYMBOLNAME));
			tab.EndAddValueItem();
		}

		AfxGetMainWnd()->SendMessage(FCCM_UPDATE_PROPERTY_UI,0,(LPARAM)(&tab));
		
	}
	else
	{
		AfxMessageBox(IDS_RESOURCE_ERR);
	}
}

void CDlgDoc::OnUpdateDummySelectFtrSym(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

void CDlgDoc::SetMapArea()
{
	PT_3D pt3ds[4];
	memset(pt3ds,0,sizeof(pt3ds));
	GetDlgDataSource()->GetBound(pt3ds,NULL,NULL);
	if( GraphAPI::GIsClockwise(pt3ds,4)!=-1 )
	{		
		UpdateAllViews(NULL,hc_SetMapArea,(CObject*)pt3ds);
	}
}

int CDlgDoc::ActiveLayer(LPCTSTR strLayerName, BOOL bSaveRecnet)
{	
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return 0;

	// 获取当前层
	CFtrLayer *pAcitveLayer = NULL;
	if (strLayerName == NULL)
	{
		CFtrLayer *pLayer = pDS->GetCurFtrLayer();
		if (!pLayer) return 0;
		strLayerName = pLayer->GetName();

		pAcitveLayer = pLayer;
	}
	else
	{
		CFtrLayer *pFtrLayer = pDS->GetFtrLayer(strLayerName);
		if (!pFtrLayer)  return 0;

		if (!pDS->SetCurFtrLayer(pFtrLayer->GetID()))
			return 0;

		pAcitveLayer = pFtrLayer;
	}

	// 隐藏的层提示
	if (pAcitveLayer && !pAcitveLayer->IsVisible())
	{
		CString tip;
		tip.Format(StrFromResID(IDS_TIP_LAYERVISIBLE),pAcitveLayer->GetName());

		if( AfxMessageBox(tip,MB_YESNO)==IDYES )
		{		
			CUndoModifyLayer undo(this,_T("ModifyLayer"));
			undo.arrLayers.Add(pAcitveLayer);
			undo.field = FIELDNAME_LAYVISIBLE;
			undo.arrOldVars.Add(bool(pAcitveLayer->IsVisible()));

			_variant_t var = (bool)true;
			ModifyLayer(pAcitveLayer,FIELDNAME_LAYVISIBLE,var,TRUE);

			undo.newVar = var;
			undo.Commit();	
			
		}
	}

	CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(pDS->GetScale());
	if (!pPlaceLib) return 0;
	
	CPlaceConfig *pConfig = pPlaceLib->GetConfig(strLayerName);
	// 没有则新建一个缺省命令为打点
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return -1;

	if (!pConfig)
	{
		int nGeoClass = CLS_GEOCURVE;
		CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(strLayerName);
		if (pLayer)
		{
			nGeoClass = pLayer->GetGeoClass();
		}

		CPlaceConfig config;		
		if (nGeoClass == CLS_GEOPOINT)
		{
			config.m_nCommandID = ID_ELEMENT_DOT_DOT;
		}
		else if (nGeoClass == CLS_GEODIRPOINT)
		{
			config.m_nCommandID = ID_ELEMENT_DOT_VECTORDOT;
		}
		else if (nGeoClass == CLS_GEOCURVE)
		{
			config.m_nCommandID = ID_ELEMENT_LINE_LINE;
		}
		else if (nGeoClass == CLS_GEODCURVE)
		{
			config.m_nCommandID = ID_ELEMENT_LINE_DLINE;
		}
		else if (nGeoClass == CLS_GEOPARALLEL)
		{
			config.m_nCommandID = ID_ELEMENT_LINE_PARALLEL;
		}
		else if (nGeoClass == CLS_GEOSURFACE)
		{
			config.m_nCommandID = ID_ELEMENT_FACE_FACE;
		}
		else if (nGeoClass == CLS_GEOTEXT)
		{
			config.m_nCommandID = ID_ELEMENT_TEXT;
		}

		strcpy(config.m_strLayName, strLayerName);
		pPlaceLib->AddConfig(config);
		pConfig = pPlaceLib->GetConfig(strLayerName);
	}

// 	CancelCurrentCommand();

// 	StartCommand(pConfig->m_nCommandID);

	AfxGetMainWnd()->SendMessage(WM_COMMAND,pConfig->m_nCommandID);
	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	tab.AddValue(XMLTAG_CMDPARAMS, &CVariantEx(pConfig->m_strCmdParams));
// 	tab.EndAddValueItem();
// 	
// 	SetCurCmdParams(tab);
	
	CCommand *pCmd = GetCurrentCommand();
	if (pCmd!=NULL && pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
		// 如果是采集命令则自动设置采集特征
	{
//		CFtrLayer *pFtrLayer = pDS->GetFtrLayer();
		
		//add to recnet
		if (pScheme != NULL && bSaveRecnet)
		{
			CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(strLayerName);
			if (!pLayer)	return -1;
			int nMaxLayerNum  = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RECENTLAYER,40);
			pScheme->SetMaxRecentNum(nMaxLayerNum);
			pScheme->AddToRecent(pLayer->GetLayerCode(), strLayerName);
			pScheme->Save();
			//USERIDX *pRecnet = pScheme->GetRecentIdx();
			AfxGetMainWnd()->PostMessage(FCCM_INITFTRCLASS, WPARAM(1), LPARAM(pScheme));	
			
		}

		AfxGetMainWnd()->PostMessage(FCCM_INITLAYERTOOL, WPARAM(1), LPARAM(this));			
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(this));
		
	}
	
	BOOL bReadViewPara = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_READVIEWPARA,TRUE);
	if (bReadViewPara)
	{
		// 视图参数
		UpdateAllViewsParams(0,(LPARAM)pConfig->m_strViewParams,FALSE);		
	}

	if(m_bFocusColletion)
	{
		((CMainFrame*)AfxGetMainWnd())->m_wndCollectionView.OnSetFocusEdit();
	}
	
	return 1;
}

BOOL CDlgDoc::UpdateAllViewsParams(WPARAM wParam, LPARAM lParam, BOOL send)
{
	BOOL bRes = FALSE;

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		if (pView) 
		{
			if (send)
			{
				bRes |= pView->SendMessage(FCCM_UPDATESTEREOPARAM, wParam, lParam);
			}
			else
			{
				bRes |= pView->PostMessage(FCCM_UPDATESTEREOPARAM, wParam, lParam);
			}
			
		}
	}

	return bRes;
}

void CDlgDoc::UpdateAllOtherViews(LPARAM lHint,CObject* pHint)
{
	CMultiDocTemplate *pDocTemplate = (CMultiDocTemplate*)GetDocTemplate();
	if (!pDocTemplate) return;

	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while (pos)
	{
		CDocument *pDoc = pDocTemplate->GetNextDoc(pos);
		if (pDoc && pDoc != this)
		{
			pDoc->UpdateAllViews(NULL,lHint,pHint);
		}
	}
}

BOOL DBFAddField(DBFHandle hDbf, const CValueTable &tab, int index)
{
	BOOL bRet = TRUE;

	CString field, name;
	int type;
	if (tab.GetField(index,field,type,name))
	{
		switch( type )
		{
		case VT_I2:
		case VT_I4:
		case VT_BOOL:
		case VT_UI1:
			DBFAddField(hDbf,field,FTInteger,8,0);
			break;
		case VT_R4:
		case VT_R8:
			DBFAddField(hDbf,field,FTDouble,16,6);
			break;
		case VT_BSTR:
			DBFAddField(hDbf,field,FTString,255,0);
			break;
		default:
			bRet = FALSE;
			break;
		}

	}

	return bRet;
}

BOOL DBFAddField(DBFHandle hDbf, const XDefine *xDefine)
{
	BOOL bRet = TRUE;

	switch( xDefine->valuetype )
	{
	case DP_CFT_SMALLINT:
	case DP_CFT_BYTE	:
	case DP_CFT_INTEGER	:
	case DP_CFT_COLOR	:
	case DP_CFT_BOOL	:
		DBFAddField(hDbf,xDefine->field,FTInteger,8,0);
		break;
	case DP_CFT_FLOAT	:
	case DP_CFT_DOUBLE	:
		DBFAddField(hDbf,xDefine->field,FTDouble,16,6);
		break;
	case DP_CFT_DATE	:
	case DP_CFT_VARCHAR	:
		DBFAddField(hDbf,xDefine->field,FTString,255,0);
		break;
	default:
		bRet = FALSE;
		break;
	}

	return bRet;
}

void DBFWriteAttribute(DBFHandle hDbf, const CVariantEx *pvar,
					   int iShape, int iAttr)
{
	switch( pvar->GetType() )
	{
	case VT_I2:
	case VT_I4:
	case VT_BOOL:
	case VT_UI1:
		DBFWriteIntegerAttribute(hDbf,iShape,iAttr,(long)(_variant_t)(*pvar));
		break;
	case VT_R4:
	case VT_R8:
		DBFWriteDoubleAttribute(hDbf,iShape,iAttr,(float)(_variant_t)(*pvar));
		break;
	case VT_BSTR:
		DBFWriteStringAttribute(hDbf,iShape,iAttr,(const char*)(_bstr_t)(_variant_t)(*pvar));
		break;
	default:
		;
	}
}

struct ObjCodeGroup
{
//	int code;
	char code[256];
	CFtrLayer *pLayer;
	CPtrArray *pArrObjs;
};

void CDlgDoc::OnExportShp()
{
	CDlgExportShp dlg;

	if( dlg.DoModal()!=IDOK )
		return;

	CString retPath;
	retPath = dlg.m_strPath;

	CListFile lstFile, lstFile2;
	if( (dlg.m_strLstFile.GetLength()>0 && !lstFile.Open(dlg.m_strLstFile)) || 
		(dlg.m_strLstFile2.GetLength()>0 && !lstFile2.Open(dlg.m_strLstFile2)) )
	{
		AfxMessageBox(IDS_DOC_READLIST_ERR);
	}
	
	CFtrLayer *pLayer = NULL;
	CFeature  *pFtr = NULL;
	
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;

	int i, j, k, nLayer = pDS->GetFtrLayerCount(), nObj;
	int lSum = 0;

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return;

	//计数
	//并将地物按照层码分类
	CArray<ObjCodeGroup,ObjCodeGroup> arrGrps;
	ObjCodeGroup item;

	for (i=0; i<nLayer; i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
		
		nObj = pLayer->GetObjectCount();
		if( nObj<=0 )continue;

		for( j=0; j<nObj; j++)
		{
			pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;
			
			CString name = pLayer->GetName();
			char codestr[256] = {0};
			
			BOOL bMatch = FALSE;
			if( !dlg.m_strLstFile.IsEmpty() )
			{
				const char *match = lstFile.FindMatchItem(name,TRUE,TRUE);
				if( match )
				{
					strcpy(codestr,match);
					bMatch = TRUE;
				}
			}
			
			if (!bMatch)
			{
				__int64 ncode = 0;
				if( !pScheme->FindLayerIdx(FALSE,ncode,name) )
				{
					strcpy(codestr,name);
					
					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
						name = StrFromResID(IDS_DEFLAYER_NAMEL);
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
						name = StrFromResID(IDS_DEFLAYER_NAMEP);
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
						name = StrFromResID(IDS_DEFLAYER_NAMEL);
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
						name = StrFromResID(IDS_DEFLAYER_NAMEL);
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
						name = StrFromResID(IDS_DEFLAYER_NAMES);
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
						name = StrFromResID(IDS_DEFLAYER_NAMET);
					else
						continue;
					
					if( !pScheme->FindLayerIdx(FALSE,ncode,name) )
						continue;
				}
				
				sprintf(codestr,"%I64d",ncode);
			}
			
			int nSize = arrGrps.GetSize();
			for( k=0; k<nSize; k++)
			{
				if( stricmp(arrGrps.GetAt(k).code,codestr)==0 )
					break;
			}
			if( k<nSize )
			{
				CPtrArray *pArrObjs = arrGrps.GetAt(k).pArrObjs;
				if( pArrObjs!=NULL )
				{
					pArrObjs->Add(pFtr);
					lSum++;
					
				}
			}
			else
			{
				strcpy(item.code,codestr);
				item.pLayer = pDS->GetFtrLayer(name);
				item.pArrObjs = new CPtrArray;
				if( item.pArrObjs!=NULL )
				{				
					item.pArrObjs->Add(pFtr);
					lSum++;
					arrGrps.Add(item);
				}
			}
		}

	}

	if( lSum<=0 )return;

	GProgressStart(lSum);

	//遍历图层以写入文件
	int nShapeType = 0;
	int nSize = arrGrps.GetSize();
	for (i=0; i<nSize; i++)
	{
		item = arrGrps.GetAt(i);
		pLayer = item.pLayer;
		if( !pLayer )continue;

		nObj = item.pArrObjs->GetSize();
		if( nObj<=0 )continue;

		CSchemeLayerDefine *schemelayer = pScheme->GetLayerDefine(pLayer->GetName());
		if (!schemelayer)
		{
			GProgressStep(nObj);
			continue;
		}

		int nGeoClass = schemelayer->GetGeoClass();
		if (nGeoClass == CLS_GEOPOINT || nGeoClass == CLS_GEODIRPOINT || nGeoClass == CLS_GEOSURFACEPOINT)
			nShapeType = SHPT_POINTZ;
		else if(nGeoClass == CLS_GEOSURFACE || nGeoClass == CLS_GEOMULTISURFACE)
			nShapeType = SHPT_POLYGONZ;
		else if(nGeoClass == CLS_GEOTEXT)
			nShapeType = SHPT_MULTIPOINTZ;
		else
			nShapeType = SHPT_ARCZ;

		CString name;
		name.Format("%s\\%s",retPath,item.code);

		//创建ShapeFile, PointZ 类型
		SHPHandle hShp	= SHPCreate((LPCTSTR)name,nShapeType);
		
		//创建DBF文件
		DBFHandle hDbf	= DBFCreate((LPCTSTR)name);
		if (hShp!=NULL && hDbf!=NULL )
		{
			//该字段是否被添加的标记数组
			CArray<int,int> arrHaveFieldFlags;

			//向DBF文件写入字段定义(基本属性和扩展属性)
			CValueTable tabBasicFields;
			schemelayer->GetBasicAttributeDefaultValues(tabBasicFields);
			int nBasicFieldsNum = tabBasicFields.GetFieldCount();

			int nField;
			const XDefine *pXDefine = schemelayer->GetXDefines(nField);
			arrHaveFieldFlags.SetSize(nField+nBasicFieldsNum);

			for( j=0; j<nBasicFieldsNum; j++)
			{
				CString field, name;
				int type;
				tabBasicFields.GetField(j,field,type,name);

				CString field1 = lstFile2.FindMatchItem(field);
				//对应字段名称如果为*，就被认为是不需要添加的字段
				if( field1.CompareNoCase("*")==0 || !dlg.m_bBaseAtt)
				{
					arrHaveFieldFlags.SetAt(j,0);
					continue;
				}
				
				int ret = DBFAddField(hDbf,tabBasicFields,j);
				arrHaveFieldFlags.SetAt(j,ret);
			}
			
			for( j=0; j<nField; j++)
			{
				XDefine define = *(pXDefine + j);

				CString field = define.field;
				CString field1 = lstFile2.FindMatchItem(define.field);
				//对应字段名称如果为*，就被认为是不需要添加的字段
				if( field1.CompareNoCase("*")==0  || !dlg.m_bExtAtt)
				{
					arrHaveFieldFlags.SetAt(nBasicFieldsNum+j,0);
					continue;
				}

				int ret = DBFAddField(hDbf,&define);
				arrHaveFieldFlags.SetAt(nBasicFieldsNum+j,ret);
			}

			int *pHaveFieldFlags = arrHaveFieldFlags.GetData();

			for( j=0; j<nObj; j++)
			{
				pFtr = (CFeature*)item.pArrObjs->GetAt(j);
				if( !pFtr )continue;

				CFeature *pFtr0 = pFtr;

				CGeometry *pGeo = pFtr->GetGeometry();
				if (!pGeo) continue;

				CPFeature ftrs[2] = {pFtr,NULL};
				int nftr = 1;
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				{
					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
					{
						CGeometry *pGeo1 = NULL, *pGeo2 = NULL;
						((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2);
						if( pGeo1 && pGeo2 )
						{
							ftrs[0] = pFtr0->Clone();
							ftrs[1] = pFtr0->Clone();

							ftrs[0]->SetGeometry(pGeo1);
							ftrs[1]->SetGeometry(pGeo2);
							nftr = 2;
						}
						else
						{
							if( pGeo1 )delete pGeo1;
							if( pGeo2 )delete pGeo2;
						}
					}
					else
					{
						CGeometry *pGeo1 = NULL, *pGeo2 = NULL;
						((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2);
						if( pGeo1 && pGeo2 )
						{
							ftrs[0] = pFtr0->Clone();
							ftrs[1] = pFtr0->Clone();
							
							ftrs[0]->SetGeometry(pGeo1);
							ftrs[1]->SetGeometry(pGeo2);
							nftr = 2;
						}
						else
						{
							if( pGeo1 )delete pGeo1;
							if( pGeo2 )delete pGeo2;
						}
					}
				}

				GProgressStep();

				for( int m=0; m<nftr; m++)
				{
					pFtr = ftrs[m];
					pGeo = pFtr->GetGeometry();

					CArray<PT_3DEX,PT_3DEX> pts;
					pGeo->GetShape(pts);

					int npt = pts.GetSize();

					if (npt < 1)  continue;

					int iShape = -1;
					
					//点类型
					if(nGeoClass == CLS_GEOPOINT || nGeoClass == CLS_GEODIRPOINT || nGeoClass==CLS_GEOSURFACEPOINT)
					{
						//写坐标数据
						PT_3DEX expt;
						expt = pts[0];

						SHPObject* pNewObj = SHPCreateSimpleObject(SHPT_POINTZ,1,&expt.x,&expt.y,&expt.z);
						if( pNewObj )
						{
							iShape = SHPWriteObject(hShp,-1,pNewObj);
							SHPDestroyObject(pNewObj);
						}
					}
					else if(nGeoClass == CLS_GEOTEXT || nGeoClass == CLS_GEOSURFACE)
					{
						//坐标数据
						PT_3DEX expt;

						double *x = new double[npt];
						double *y = new double[npt];
						double *z = new double[npt];
						if( x && y && z )
						{
							for( k=0; k<npt; k++)
							{
								expt = pts[k];
								
								x[k] = expt.x;
								y[k] = expt.y;
								z[k] = expt.z;
							}
							
							SHPObject* pNewObj = NULL;
							if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
							{
								CGeoMultiSurface *pGeoMSurface = (CGeoMultiSurface*)pGeo;
								if (!pGeoMSurface) continue;

								int nSurfaceNum = pGeoMSurface->GetSurfaceNum();
								int *panPartStart = new int[nSurfaceNum], *panPartType = new int[nSurfaceNum];
								if (panPartStart == NULL || panPartType == NULL) continue;
								panPartStart[0] = 0;
								panPartType[0] = SHPT_POLYGON;
								
								for( k=1; k<nSurfaceNum; k++)
								{
									CArray<PT_3DEX,PT_3DEX> arr;
									pGeoMSurface->GetSurface(k-1,arr);
									
									panPartStart[k] = arr.GetSize() + panPartStart[k-1];								
									panPartType[k] = SHPT_POLYGON;
									
								}
								
								pNewObj = SHPCreateObject(nShapeType,-1,pGeoMSurface->GetSurfaceNum(),panPartStart,panPartType,npt,x,y,z,NULL);
							
								delete []panPartStart;
								delete []panPartType;
							}
							else
							{
								pNewObj = SHPCreateSimpleObject(nShapeType,npt,x,y,z);
							}
							
							if( pNewObj )
							{
								iShape = SHPWriteObject(hShp,-1,pNewObj);
								SHPDestroyObject(pNewObj);
							}
						}
						if( x )delete[] x;
						if( y )delete[] y;
						if( z )delete[] z;
					}
					else if (nGeoClass == CLS_GEOMULTISURFACE)
					{
						CGeoMultiSurface *pGeoMSurface = (CGeoMultiSurface*)pGeo;
						if (!pGeoMSurface) continue;
						//坐标数据
						PT_3DEX expt;
						
						double *x = new double[npt];
						double *y = new double[npt];
						double *z = new double[npt];
						if( x && y && z )
						{
							for( k=0; k<npt; k++)
							{
								expt = pts[k];
								
								x[k] = expt.x;
								y[k] = expt.y;
								z[k] = expt.z;
							}

							int nSurfaceNum = pGeoMSurface->GetSurfaceNum();
							int *panPartStart = new int[nSurfaceNum], *panPartType = new int[nSurfaceNum];
							if (panPartStart == NULL || panPartType == NULL) continue;
							panPartStart[0] = 0;

							for( k=0; k<nSurfaceNum; k++)
							{
								CArray<PT_3DEX,PT_3DEX> arr;
								pGeoMSurface->GetSurface(k,arr);
								
								if (k >= 1)
								{
									panPartStart[k] = arr.GetSize() + panPartStart[k-1];
								}

								panPartType[k] = SHPT_POLYGON;
								
							}
							
							SHPObject* pNewObj = SHPCreateObject(nShapeType,-1,pGeoMSurface->GetSurfaceNum(),panPartStart,panPartType,npt,x,y,z,NULL);
							if( pNewObj )
							{
								iShape = SHPWriteObject(hShp,-1,pNewObj);
								SHPDestroyObject(pNewObj);
							}

							delete []panPartStart;
							delete []panPartType;
						}
						if( x )delete[] x;
						if( y )delete[] y;
						if( z )delete[] z;
					}
					//其他类型
					else
					{
						if( npt<2 )continue;

						//写坐标数据
						PT_3DEX expt;					

						double *x = new double[npt];
						double *y = new double[npt];
						double *z = new double[npt];
						if( x && y && z )
						{
							for( k=0; k<npt; k++)
							{
								expt = pts[k];
								
								x[k] = expt.x;
								y[k] = expt.y;
								z[k] = expt.z;
							}
							
							SHPObject* pNewObj = SHPCreateSimpleObject(SHPT_ARCZ,npt,x,y,z);
							if( pNewObj )
							{
								iShape = SHPWriteObject(hShp,-1,pNewObj);
								SHPDestroyObject(pNewObj);
							}
						}
						if( x )delete[] x;
						if( y )delete[] y;
						if( z )delete[] z;
					}

					if( iShape>=0 )
					{
						int iAttrIdx = 0;
						//写基本属性值
						CValueTable tab;
						tab.BeginAddValueItem();
						pFtr0->WriteTo(tab);
						tab.EndAddValueItem();
						tab.DelField(FIELDNAME_FTRID);
						tab.DelField(FIELDNAME_CLSTYPE);
						tab.DelField(FIELDNAME_SHAPE);
						for( k=0; k<nBasicFieldsNum; k++)
						{
							if( pHaveFieldFlags[k]==0 )
								continue;

							CString field, name;
							int type;
							tabBasicFields.GetField(k,field,type,name);
							
							const CVariantEx *pvar;
							if ( tab.GetValue(0,field,pvar) )
							{
								DBFWriteAttribute(hDbf,pvar,iShape,iAttrIdx);
							}						
							
							iAttrIdx++;
						}
						//写扩展属性值
						CAttributesSource  *pDXS = GetDlgDataSource()->GetXAttributesSource();
						if(!pDXS) continue;

						tab.DelAll();
						tab.BeginAddValueItem();
						pDXS->GetXAttributes(pFtr0,tab);
						tab.EndAddValueItem();

						for( k=0; k<nField; k++)
						{
							if( pHaveFieldFlags[k+nBasicFieldsNum]==0 )
								continue;
							
							XDefine define = *(pXDefine+k);

							const CVariantEx *pvar;
							if ( tab.GetValue(0,define.field,pvar) )
							{
								DBFWriteAttribute(hDbf,pvar,iShape,iAttrIdx);
							}						
							
							iAttrIdx++;
						}
					}
				}

				if( nftr==2 )
				{
					delete ftrs[0];
					delete ftrs[1];
				}
			}
		}

		if( hShp )SHPClose(hShp);
		if( hDbf )DBFClose(hDbf);

		delete item.pArrObjs;
	}

	GProgressEnd();

}

BOOL DBFReadAttribute(DBFHandle hDbf, int iShape, LPCTSTR field, const XDefine *pXdefine, CValueTable &tab)
// 					  LPCTSTR field, CDpCustomFieldInfo *pInfo, 
// 					  CDpDBVariant& var, DpCustomFieldType& type)
{
	//	type = pInfo->m_CustomFieldType;
	int index = DBFGetFieldIndex(hDbf,field);
	if( index<0 )return FALSE;
	
	CVariantEx var;
	
	switch( pXdefine->valuetype )
	{
	case DP_CFT_SMALLINT:
		var = (_variant_t)(short)DBFReadIntegerAttribute(hDbf,iShape,index);
		break;
	case DP_CFT_BYTE	:
		var = (_variant_t)(BYTE)DBFReadIntegerAttribute(hDbf,iShape,index);
		break;
	case DP_CFT_INTEGER	:
		var = (_variant_t)(long)DBFReadIntegerAttribute(hDbf,iShape,index);
		break;
	case DP_CFT_COLOR	:
		var = (_variant_t)(long)DBFReadIntegerAttribute(hDbf,iShape,index);
		break;
	case DP_CFT_BOOL	:
		var = (_variant_t)(bool)DBFReadIntegerAttribute(hDbf,iShape,index);
		break;
	case DP_CFT_FLOAT	:
	case DP_CFT_DOUBLE	:
		var = (_variant_t)DBFReadDoubleAttribute(hDbf,iShape,index);
		break;
	case DP_CFT_VARCHAR	:
	case DP_CFT_DATE	:
		{
			const char* value = DBFReadStringAttribute(hDbf,iShape,index);
			if(value!=NULL)
				var = (_variant_t)value;
			else
				return FALSE;
		}		
		break;
	default:
		return FALSE;
	}
	
	tab.AddValue(field,&var);
	
	return TRUE;
}

BOOL DBFReadAttribute(DBFHandle hDbf, int iShape, CString field, int type, CValueTable &tab)
{
	int index = DBFGetFieldIndex(hDbf,field);
	if( index<0 )return FALSE;
	
	CVariantEx var;

	switch( type )
	{
	case VT_I2:
	case VT_I4:
	case VT_BOOL:
	case VT_UI1:
		var = (_variant_t)(long)DBFReadIntegerAttribute(hDbf,iShape,index);
		break;
	case VT_R4:
	case VT_R8:
		var = (_variant_t)DBFReadDoubleAttribute(hDbf,iShape,index);
		break;
	case VT_BSTR:
		var = (_variant_t)(char*)DBFReadStringAttribute(hDbf,iShape,index);
		break;
	default:
		return FALSE;
		;
	}

	tab.AddValue(field,&var);
	
	return TRUE;
}


CFeature *CreateObjFromSHPType(CDlgDataSource *pDataSource, int clstype, LPCTSTR layName, CFtrLayer **ppLayer)
{
/*	int clstype = CLS_GEOCURVE;
	switch( type ) 
	{
	case SHPT_POINT:
	case SHPT_POINTZ:
		clstype = CLS_GEOPOINT;
		break;
	case SHPT_ARC:
	case SHPT_ARCZ:
		clstype = CLS_GEOCURVE;
		break;
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
		clstype = CLS_GEOSURFACE;
		break;
	default:;
	}
	
	char LayerName[256];
	
	switch(clstype)
	{
	case CLS_GEOPOINT:
	case CLS_GEOMULTIPOINT:
		strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMEP));
		break;
	case CLS_GEOCURVE:
		strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMEL));
		break;
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
		strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMES));
		break;
	case CLS_GEOTEXT:
		strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMET));
		break;
	}
*/
	*ppLayer = pDataSource->CreateFtrLayer(layName);
	
	CFeature *pFtr = NULL;
	if (*ppLayer)
	{
		pDataSource->AddFtrLayer(*ppLayer);
		pFtr = (*ppLayer)->CreateDefaultFeature(pDataSource->GetScale(),clstype);
	}
	//创建 Geometry 对象
// 	CGeometry *pGeo = pDataSource->CreateObjByFIDOrUserIdx(LayerName,clstype,FALSE);
// 	*ppLayer = pDataSource->GetLayerByFIDOrUserIdx(LayerName);
	
	return pFtr;
}

BOOL SHPObjectToFeature(SHPObject *pShp, CFeature *pFtr)
{
	CGeometry *pGeo = pFtr->GetGeometry();
	int nGeoClass = pGeo->GetClassType();
	PT_3DEX expt;
	expt.pencode = penLine;
	if( nGeoClass==CLS_GEOPOINT )
	{
		if( pShp->nVertices<1 )return FALSE;
		expt.x = pShp->padfX[0];
		expt.y = pShp->padfY[0];
		expt.z = pShp->padfZ[0];
		pGeo->CreateShape(&expt,1);
	}
	else if( nGeoClass==CLS_GEOCURVE )
	{
		if( pShp->nVertices<2 )return FALSE;
		CArray<PT_3DEX,PT_3DEX> pts;
		for( int i=0; i<pShp->nVertices; i++)
		{
			expt.x = pShp->padfX[i];
			expt.y = pShp->padfY[i];
			expt.z = pShp->padfZ[i];
			pts.Add(expt);
		}
		pGeo->CreateShape(pts.GetData(),pts.GetSize());

	}
	else if( nGeoClass==CLS_GEOSURFACE || nGeoClass==CLS_GEOMULTISURFACE )
	{
		if( pShp->nVertices<3 )return FALSE;
		if (pShp->nParts > 1)
		{
			int aa = 10;
		}
		CArray<PT_3DEX,PT_3DEX> pts;
		for (int i=0; i<pShp->nParts; i++)
		{
			int iStart, iEnd;
			iStart = pShp->panPartStart[i];
			iEnd = (i==pShp->nParts-1)?pShp->nVertices:pShp->panPartStart[i+1];
			for (int j=iStart; j<iEnd; j++)
			{
				expt.x = pShp->padfX[j];
				expt.y = pShp->padfY[j];
				expt.z = pShp->padfZ[j];
				expt.pencode = (j==iStart)?penMove:penLine;

				pts.Add(expt);
			}
		}
	
		pGeo->CreateShape(pts.GetData(),pts.GetSize());
	}
	
	return TRUE;
}

void CDlgDoc::OnImportShp()
{
	CDlgImportShp dlg;
	
	if( dlg.DoModal()!=IDOK )
		return;

	CListFile lstFile, lstFile2;
	if( (dlg.m_strLstFile.GetLength()>0 && !lstFile.Open(dlg.m_strLstFile)) || 
		(dlg.m_strLstFile2.GetLength()>0 && !lstFile2.Open(dlg.m_strLstFile2)) )
	{
		AfxMessageBox(IDS_DOC_READLIST_ERR);
	}

	int nFiles = dlg.m_arrFileNames.GetSize();
	int i,j,k;

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return;

	CUndoFtrs undo(this,"Import SHP");

	BeginBatchUpdate();
	for( i=0; i<nFiles; i++)
	{
		CString strName = dlg.m_arrFileNames.GetAt(i);
		CString strMsg;
		strMsg.Format("Importing SHP file: %s\n",strName);
		GOutPut(strMsg);

		strName = strName.Left(strName.GetLength()-4);

		int index = strName.ReverseFind('\\');
		CString code = strName.Right(strName.GetLength()-index-1);

		SHPHandle hShp = SHPOpen(strName,"rb");
		DBFHandle hDbf = DBFOpen(strName,"rb");

		if( hShp!=NULL && hDbf!=NULL )
		{
			int nEntities, nShapeType;
			SHPGetInfo(hShp,&nEntities,&nShapeType,NULL,NULL);

// 			int nRecord = DBFGetRecordCount(hDbf);
// 			if( nEntities>nRecord )
// 				nEntities = nRecord;

			GProgressStart(nEntities);

			SHPObject *pShp;
			for( j=0; j<nEntities; j++)
			{
				GProgressStep();

				CString strLayerName = lstFile.FindMatchItem(code,TRUE,FALSE);
				// 对照表找不到，缺省为层码
				if( strLayerName.IsEmpty() )
				{
					strLayerName = code;
					
					__int64 ncode = _atoi64(strLayerName);
					CString name;
					if( pScheme->FindLayerIdx(TRUE,ncode,name) )
					{
						strLayerName = name;
					}
				}
//				int index = DBFGetFieldIndex(hDbf,strField);
// 				if( index>=0 )
				{
					pShp = SHPReadObject(hShp,j);
					if( pShp!=NULL )
					{						
						CFtrLayer *pLayer = pDS->GetFtrLayer(strLayerName);

						int clstype = CLS_GEOCURVE;
						switch( pShp->nSHPType ) 
						{
						case SHPT_POINT:
						case SHPT_POINTZ:
							clstype = CLS_GEOPOINT;
							break;
						case SHPT_ARC:
						case SHPT_ARCZ:
							clstype = CLS_GEOCURVE;
							break;
						case SHPT_POLYGON:
						case SHPT_POLYGONZ:
							clstype = (pShp->nParts>1)?CLS_GEOMULTISURFACE:CLS_GEOSURFACE;
							break;
						default:
							clstype = CLS_GEOCURVE;
							break;
						}

						CFeature *pFtr;
						if (pLayer != NULL)
							pFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),clstype);
						else							
							pFtr = CreateObjFromSHPType(pDS,clstype,strLayerName,&pLayer);						

						if( pFtr!=NULL && pLayer!=NULL)
						{
							//读取属性
							BOOL bReadOK = FALSE;

							//读取坐标数据
							if( SHPObjectToFeature(pShp,pFtr) )
								bReadOK = TRUE;
							
							if( !bReadOK )
							{
								delete pFtr;
								continue;
							}

							// 基本属性
							CValueTable tab;
							tab.BeginAddValueItem();
							pFtr->WriteTo(tab);
							tab.EndAddValueItem();

							tab.DelField(FIELDNAME_FTRID);
							tab.DelField(FIELDNAME_CLSTYPE);
							tab.DelField(FIELDNAME_SHAPE);

							CValueTable tab1;
							tab1.BeginAddValueItem();
							for( k=0; k<tab.GetFieldCount(); k++)
							{
								CString field, name;
								int type;
								tab.GetField(k,field,type,name);

								CString strField = lstFile2.FindMatchItem(field,TRUE,TRUE);
								if( strField.IsEmpty() )strField = field;
								if( strField.CompareNoCase("*")==0 )
									continue;
								
								if( stricmp(field,"CLASSID")==0 || stricmp(field,"VISI")==0 ||
									stricmp(field,"LOCK")==0 || stricmp(field,"DELETED")==0 )
									continue;
								
								DBFReadAttribute(hDbf,j,strField,type,tab1);									
							}
							tab1.EndAddValueItem();

							pFtr->ReadFrom(tab1);

							if( !AddObject(pFtr,pLayer->GetID()))
							{
								delete pFtr;
								continue;
							}
							
							undo.AddNewFeature(FTR_HANDLE(pFtr));

							tab.DelAll();

							CSchemeLayerDefine *schemelayer = pScheme->GetLayerDefine(pLayer->GetName());

							if(schemelayer)
							{
								tab.BeginAddValueItem();
								int nField;
								const XDefine *pXDefine = schemelayer->GetXDefines(nField);
								for( k=0; k<nField; k++)
								{
									CString strField0 = pXDefine[k].field;
									CString strField = lstFile2.FindMatchItem(strField0,TRUE,TRUE);
									if( strField.IsEmpty() )strField = strField0;
									if( strField.CompareNoCase("*")==0 )
										continue;

									if( stricmp(strField0,"CLASSID")==0 || stricmp(strField0,"VISI")==0 ||
										stricmp(strField0,"LOCK")==0 || stricmp(strField0,"DELETED")==0 )
										continue;

									if(code.CompareNoCase("HYDNT_LN")==0 && k==3)
									{
										int a=1;
									}
 
									if ( DBFReadAttribute(hDbf,j,strField,&pXDefine[k],tab) )
									{
									}
									
								}
								tab.EndAddValueItem();

								CAttributesSource  *pDXS = GetDlgDataSource()->GetXAttributesSource();
								if(!pDXS) continue;
		
								pDXS->SetXAttributes(pFtr,tab);
							}
						}

						SHPDestroyObject(pShp);
					}
				}
			}
		}
		
		if( hShp )SHPClose(hShp);
		if( hDbf )DBFClose(hDbf);
	}

	undo.Commit();

	EndBatchUpdate();
	GProgressEnd();

	UpdateAllViews(NULL,hc_UpdateAllObjects);

}

CString MakeTexture(CDlgDoc *pDoc, PT_3D *pts, int num, PT_3D *rets, CImageRead& image, int imgNO, LPCTSTR path)
{
	//求外接矩形
	Envelope evlp;
	evlp.CreateFromPts(pts,num);
	
	//取整和归一化
	evlp.m_xl = floor(evlp.m_xl);  evlp.m_xh = ceil(evlp.m_xh);
	evlp.m_yl = floor(evlp.m_yl);  evlp.m_yh = ceil(evlp.m_yh);
	
	for( int i=0; i<num; i++)
	{
		rets[i].x = (pts[i].x-evlp.m_xl)/(evlp.m_xh-evlp.m_xl);
		rets[i].y = (pts[i].y-evlp.m_yl)/(evlp.m_yh-evlp.m_yl);
		rets[i].z = 0;
	}
	
	CSize size = image.GetImageSize();
	
	//截取影像
	CRect rect((int)(evlp.m_xl+0.5),(int)(evlp.m_yl+0.5),
		(int)(evlp.m_xh+0.5),(int)(evlp.m_yh+0.5));
	BITMAPINFO info;
	
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = rect.Width();
	info.bmiHeader.biHeight = rect.Height();
	info.bmiHeader.biSizeImage = ((rect.Width()*3+3)&(~3))*rect.Height();
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biXPelsPerMeter = 0;
	info.bmiHeader.biYPelsPerMeter = 0;
	info.bmiHeader.biClrUsed = 0;
	info.bmiHeader.biClrImportant = 0;
	
	BYTE *pBits = NULL;
	HBITMAP hBmp = CreateDIBSection( NULL,&info,DIB_RGB_COLORS,
		(void**)&pBits,0,0L );
	if( !hBmp || !pBits )return _T("");
	
	memset(pBits, 0, info.bmiHeader.biSizeImage);
	image.ReadRectBmp(rect,hBmp);
	
	CImageAdjust ia;
	pDoc->UpdateAllViews(NULL,hc_GetImageAdjust,(CObject*)&ia);
	
	AdjustBitmap(hBmp,CRect(0,0,rect.Width(),rect.Height()),&ia,FALSE,0);
	
	//写影像
	BITMAPFILEHEADER fileInfo;
	
	fileInfo.bfType = 0x4d42;
	fileInfo.bfSize = sizeof(fileInfo)+sizeof(info)+info.bmiHeader.biSizeImage-sizeof(info.bmiColors);
	fileInfo.bfReserved1 = 0;
	fileInfo.bfReserved2 = 0;
	fileInfo.bfOffBits = sizeof(fileInfo)+sizeof(info)-sizeof(info.bmiColors);
	
	char fileName[_MAX_PATH]={0};
	sprintf(fileName,"%sTexture%d.bmp",path,imgNO);
	
	FILE *fp = fopen(fileName,"wb");
	if( fp )
	{
		fwrite(&fileInfo,sizeof(fileInfo),1,fp);
		fwrite(&info ,sizeof(info )-sizeof(info.bmiColors),1,fp);
		fwrite(pBits,1,info.bmiHeader.biSizeImage,fp);
		fclose(fp);
	}
	
	::DeleteObject(hBmp);
	if( fp )
	{
		CString ret;
		ret.Format("images\\Texture%d.bmp",imgNO);
		return ret;
	}
	else return _T("");
}

static BOOL CheckImageCoordinates(CImageRead& image, PT_3D *pts, int num)
{
	CSize sz = image.GetImageSize();
	for( int i=0; i<num; i++)
	{
		if( pts[i].x<0 || pts[i].x>sz.cx-1 )
			return FALSE;
		if( pts[i].y<0 || pts[i].y>sz.cy-1 )
			return FALSE;
	}
	return TRUE;
}

typedef CArray<PT_3DEX,PT_3DEX> Pt3dArray;

static int CGeoHui_GetTopSurfaceNum(CGeometry *pObj)
{
	Pt3dArray pts;
	pObj->GetShape(pts);
	
	if( pts.GetSize()<=5 )
		return 0;
	
	int npt = pts.GetSize();
	
	int num = 1;
	for( int i=1; i<npt; i++)
	{
		if( pts[i].pencode==penMove )
			num++;
	}
	
	return num;
}


static void CGeoHui_GetTopSurfaceShape(CGeometry *pObj, int index, Pt3dArray& ptsRet)
{
	Pt3dArray pts;
	pObj->GetShape(pts);
	
	if( pts.GetSize()<=5 )
		return;
	
	int npt = pts.GetSize();
	
	int num = 0, start = 0;
	for( int i=1; i<npt; i++)
	{
		if( num==index )
		{
			ptsRet.Add(pts[start]);
			
			for( int j=start+1; j<npt; j++)
			{
				if( pts[j].pencode==penMove )
					break;
				
				ptsRet.Add(pts[j]);
			}
			break;
		}
		
		if( pts[i].pencode==penMove )
		{
			num++;
			start = i;
		}
	}
	
	return;
}


//检查当前三角形是否为人字形房屋的两头的三角形，是的话返回TRUE，否则返回FALSE
BOOL CGeoRen_CheckEndTriangle(PT_3DEX *pts, int npt, MYPT_3D *tri)
{
	int k1 = -1, k2 = npt-1;
	for( int i=0; i<npt; i++)
	{
		if( pts[i].pencode==penMove )
		{
			k1 = i;
			break;
		}
	}

	if( k1<=4 )
		return FALSE;

	int index[3] = {-1,-1,-1};

	PT_3DEX ptt;
	for( int j=0; j<3; j++)
	{
		ptt.x = tri[j].x;
		ptt.y = tri[j].y;
		ptt.z = tri[j].z;
		for( i=0; i<npt; i++)
		{
			if( GraphAPI::GGet2DDisOf2P(ptt,pts[i])<1e-6 )
			{
				index[j] = i;
				break;
			}
		}
		if( i>=npt )
			return FALSE;
	}

	//index 排序
	BOOL bChanged = FALSE;
	while( 1 )
	{
		bChanged = FALSE;
		for( j=0; j<2; j++)
		{
			if( index[j]>index[j+1] )
			{
				int t = index[j];
				index[j] = index[j+1];
				index[j+1] = t;

				bChanged = TRUE;
			}
		}
		if( !bChanged )
			break;
	}

	int nEdgePtNum = k2-k1+1;

	if( index[0]==0 && index[1]==(k1-2) && index[2]==k1 )
		return TRUE;

	if( index[0]==(nEdgePtNum-1) && index[1]==nEdgePtNum && index[2]==k2 )
		return TRUE;
	
	return FALSE;
}


//获取人字形房屋的屋脊线节点数
int CGeoRen_GetRidgePtNum(PT_3DEX *pts, int npt )
{
	int k1 = -1, k2 = npt-1;
	for( int i=0; i<npt; i++)
	{
		if( pts[i].pencode==penMove )
		{
			k1 = i;
			break;
		}
	}	
	
	int nEdgePtNum = k2-k1+1;
	return nEdgePtNum;
}



//获取人字形房屋的两头的三角形，成功的话返回TRUE，否则返回FALSE
BOOL CGeoRen_GetEndTriangle(PT_3DEX *pts, int npt, int pos, MYTIN& tin)
{
	int k1 = -1, k2 = npt-1;
	for( int i=0; i<npt; i++)
	{
		if( pts[i].pencode==penMove )
		{
			k1 = i;
			break;
		}
	}
	
	if( k1<=4 )
		return FALSE;
	
	int nEdgePtNum = k2-k1+1;
	if( nEdgePtNum<2 )
		return FALSE;

	PT_3DEX test[3];
	test[0] = pts[0];
	test[1] = pts[k1];
	test[2] = pts[k1+1];

	if( GraphAPI::GIsClockwise(test,3)==0 )
	{
		if( pos==0 )
		{
			tin.i1 = 0;
			tin.i2 = k1-2;
			tin.i3 = k1;
		}
		else
		{
			tin.i1 = nEdgePtNum-1;
			tin.i2 = k2;
			tin.i3 = nEdgePtNum;
		}
	}
	else
	{
		if( pos==0 )
		{
			tin.i1 = 0;
			tin.i2 = k1;
			tin.i3 = k1-2;
		}
		else
		{
			tin.i1 = nEdgePtNum-1;
			tin.i2 = nEdgePtNum;
			tin.i3 = k2;
		}
	}
		
	return TRUE;
}


//人字顶房屋的侧面处理流程可以等同于一般的柱状房屋，不过需要把节点顺序调整，使得与柱状体房屋类似；
//这个函数就是做这个事情
BOOL CGeoRen_GetCylinderIndex(PT_3DEX *pts, int npt, CArray<int,int>& indexs)
{
	int k1 = -1, k2 = npt-1;
	for( int i=0; i<npt; i++)
	{
		if( pts[i].pencode==penMove )
		{
			k1 = i;
			break;
		}
	}
	
	if( k1<=4 )
		return FALSE;

	int nEdgePtNum = k2-k1+1;
	for( i=0; i<nEdgePtNum; i++)
	{
		indexs.Add(i);
	}
	indexs.Add(npt-1);
	for( ; i<k1-1; i++)
	{
		indexs.Add(i);
	}
	indexs.Add(k1);
	indexs.Add(0);

	return TRUE;
}


#define  CLS_GEOJIAN		20
#define  CLS_GEOREN			21
#define  CLS_GEOREN2		22
#define  CLS_GEOTAI			23
#define  CLS_GEOHUI			24

void MakeRender(CDlgDoc *pDoc, CMarkup& xmlFile, CGeometry *pGeo, CCoordCenter& coord, 
				double z0, CImageRead& image1, CImageRead& image2, int nImg, LPCTSTR path, 
				BOOL bExportSideFace, BOOL bFixNULL, int& no)
{
#ifndef _NOT_USE_DPWPACK
	int i,j,k,num;

	//得到顶面每个点以及其投影点的象点坐标
	CArray<PT_3D,PT_3D> arrVertexsL, arrProjectsL, arrVertexsR, arrProjectsR, arrRets, arrKeys;
	CArray<PT_3DEX,PT_3DEX> pts;
	pGeo->GetShape(pts);
	num = pts.GetSize();

	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		pts.Add(pts[0]);
		num++;
	}

	arrVertexsL.SetSize(num);
	arrProjectsL.SetSize(num);
	arrVertexsR.SetSize(num);
	arrProjectsR.SetSize(num);
	arrRets.SetSize(num);
	arrKeys.SetSize(num);
	PT_3D *ptsVL = arrVertexsL.GetData();
	PT_3D *ptsPL = arrProjectsL.GetData();
	PT_3D *ptsVR = arrVertexsR.GetData();
	PT_3D *ptsPR = arrProjectsR.GetData();
	PT_3D *rets = arrRets.GetData();
	PT_3D *keys = arrKeys.GetData();
	memset(ptsVL,0,sizeof(PT_3D)*num);
	memset(ptsPL,0,sizeof(PT_3D)*num);
	memset(ptsVR,0,sizeof(PT_3D)*num);
	memset(ptsPR,0,sizeof(PT_3D)*num);

	int nBoundStop = -1;
	
	Coordinate coord1,coord2; 
	PT_3DEX expt;
	for( i=0; i<num; i++ )
	{
		expt = pts[i];
		
		//penMove之后不再是边界部分了
		if( pts[i].pencode==penMove && nBoundStop==-1 )
			nBoundStop = i;

		coord1.x=expt.x; coord1.y=expt.y; coord1.z=expt.z;  
		coord1.iType=GROUND; coord2.iType=SCANNER;
		if( !coord.Convert(coord1,coord2) )continue;
		
		ptsVL[i].x=coord2.lx;  ptsVL[i].y=coord2.ly;
		ptsVR[i].x=coord2.rx;  ptsVR[i].y=coord2.ry;

		coord1.x=expt.x; coord1.y=expt.y; coord1.z=z0;  
		coord1.iType=GROUND; coord2.iType=SCANNER;
		if( !coord.Convert(coord1,coord2) )continue;
		ptsPL[i].x=coord2.lx;  ptsPL[i].y=coord2.ly;
		ptsPR[i].x=coord2.rx;  ptsPR[i].y=coord2.ry;
	}

	if( nBoundStop<0 )
		nBoundStop = num;

	BOOL bLeft = FALSE, bOutArea = FALSE;
	if( nImg==0 )bLeft = TRUE;
	else if( nImg==1 )bLeft = FALSE;
	else
	{
		//点是否超出了影像范围
		BOOL bOutArea1 = !CheckImageCoordinates(image1,ptsVL,num);
		BOOL bOutArea2 = !CheckImageCoordinates(image2,ptsVR,num);
		if( bOutArea1 && bOutArea2 )bOutArea = TRUE;
		else if( bOutArea1 && !bOutArea2 )bLeft = FALSE;
		else if( !bOutArea1 && bOutArea2 )bLeft = TRUE;

		//如果都没有超出，就检查看哪个面积最大
		else bLeft = GraphAPI::GGetPolygonArea(ptsVL,nBoundStop)>GraphAPI::GGetPolygonArea(ptsVR,nBoundStop);
	}

	//挖出顶面的纹理(存储为影像文件)并计算纹理坐标
	CString strImg = _T("NULL");
	if( bOutArea )
		memset(rets,0,sizeof(PT_3D)*num);
	else
		strImg = MakeTexture(pDoc,bLeft?ptsVL:ptsVR,num,rets,(bLeft?image1:image2),no, path);

	//三角剖分
	CArray<MYPT_3D,MYPT_3D> arrGeoPt;
	arrGeoPt.SetSize(num);
	MYPT_3D *geopts = arrGeoPt.GetData();
	int line_id = 0;
	for( i=0; i<num; i++ )
	{
		//尖顶点或者屋顶脊线构成独立的特征线，参与三角剖分
		if( i==0 || pts[i].pencode==penMove )
		{
			line_id++;
		}
		geopts[i].id = line_id;

		if( pts[i].pencode==penMove && i==(num-1) )
			geopts[i].type = 0;
		else
			geopts[i].type = 1;

		expt = pts[i];
		COPY_3DPT(keys[i],expt);
		COPY_3DPT(geopts[i],expt);
	}

	//人字形房屋，屋脊线可能与房边缘相交，为了确保三角网构建正确，我们将两头的房屋边线的连线段去掉
	if( pGeo->GetClassType()==CLS_GEOREN || pGeo->GetClassType()==CLS_GEOREN2 )
	{		
		int nRidgePtNum = CGeoRen_GetRidgePtNum(pts.GetData(),num); 
		if( nRidgePtNum>=2 && (num==(nRidgePtNum*3+1)) )
		{
			for( i=0; i<nRidgePtNum; i++ )
			{
				geopts[i].id = 1;				
				geopts[i].type = 1;
			}
			for( ; i<nRidgePtNum+nRidgePtNum; i++ )
			{
				geopts[i].id = 2;				
				geopts[i].type = 1;
			}
			i++;
			for( ; i<num; i++ )
			{
				geopts[i].id = 3;				
				geopts[i].type = 1;
			}
		}		
	}

	KickoffSameGeoPts(arrGeoPt);
	geopts = arrGeoPt.GetData();

	if (arrGeoPt.GetSize() < 3) return;

	MYTIN *tins = NULL;
	int nTin = 0;
	
	MYPT_3D *retPts = NULL;
	int nRetPt = 0;
	MyTriangle::createTIN(geopts,arrGeoPt.GetSize(),&tins,&nTin,&retPts,&nRetPt);

	//准备
	xmlFile.AddElem("Render");
	xmlFile.IntoElem();
	xmlFile.AddElem("SurfaceList");
	xmlFile.IntoElem();
	CString strValue;
	strValue.Format("Surface%d",no++);
	xmlFile.AddElem("Surface");
	xmlFile.AddAttrib("name",(LPCTSTR)strValue);
	xmlFile.IntoElem();

	///输出纹理
	xmlFile.AddElem("Texture",(LPCTSTR)strImg);
	xmlFile.AddElem("RenderVertexs");
	xmlFile.IntoElem();
	for( i=0; i<num; i++ )
	{
		strValue.Format("%.4f %.4f %.4f %.4f %.4f",
			geopts[i].x,geopts[i].y,geopts[i].z,rets[i].x,rets[i].y);
		xmlFile.AddElem("RV",(LPCTSTR)strValue);
	}
	xmlFile.OutOfElem(); //RenderVertexs

	//输出三角形数据
	MYPT_3D  tri[3];

	xmlFile.AddElem("Triangles");
	xmlFile.IntoElem();
	for( i=0; i<nTin+2; i++)
	{
		MYTIN tin;
		
		if( i<nTin )
		{
			tin = tins[i];
			if( tin.i1<0 || tin.i1>nRetPt ||
				tin.i2<0 || tin.i2>nRetPt ||
				tin.i3<0 || tin.i3>nRetPt )
			{
				continue;
			}

			tin.i1 = retPts[tin.i1].iold;
			tin.i2 = retPts[tin.i2].iold;
			tin.i3 = retPts[tin.i3].iold;

			tri[0] = geopts[tin.i1];
			tri[1] = geopts[tin.i2];
			tri[2] = geopts[tin.i3];

			//判断三角形是否在外边
			PT_3D cpt;
			cpt.x = (tri[0].x+tri[1].x+tri[2].x)/3; cpt.y = (tri[0].y+tri[1].y+tri[2].y)/3; cpt.z = 0;
			if( GraphAPI::GIsPtInRegion(cpt,keys,nBoundStop)!=2 )
				continue;
			
			//人字顶的两头的三角形不需要
			if( pGeo->GetClassType()==CLS_GEOREN )
			{
				if( CGeoRen_CheckEndTriangle(pts.GetData(),pts.GetSize(),tri) )
					continue;
			}
			
			if( pGeo->GetClassType()==CLS_GEOREN2 )
			{
				if( CGeoRen_CheckEndTriangle(pts.GetData(),pts.GetSize(),tri) )
					continue;
			}
		}
		else
		{
			//斜人字顶的两头三角形 手工制作
			if( pGeo->GetClassType()==CLS_GEOREN2 )
			{
				if( !CGeoRen_GetEndTriangle(pts.GetData(),pts.GetSize(),i-nTin,tin) )
					continue;

				COPY_3DPT(tri[0],pts[tin.i1]);
				COPY_3DPT(tri[1],pts[tin.i2]);
				COPY_3DPT(tri[2],pts[tin.i3]);
			}
			else
			{
				continue;
			}
		}


		//寻找当前点序号
		int idx[3] = {-1};
		for( j=0; j<3; j++ )
		{
			double dis, min = -1;
			for( k=0; k<num; k++ )
			{
				dis = fabs(tri[j].x-geopts[k].x)+fabs(tri[j].y-geopts[k].y);
				if( min<0 || min>dis )
				{
					min = dis; 
					idx[j] = k;
				}
			}
		}

		//调整顺序，使最小的序号在前
		int idxm = 0;
		k = num+2;
		for( j=0; j<3; j++ )
		{
			if( k>idx[j] )
			{
				k = idx[j];
				idxm = j;
			}
		}

		strValue.Format("%d %d %d",idx[idxm],idx[(idxm+1)%3],idx[(idxm+2)%3]);
		xmlFile.AddElem("Triangle",strValue);
	}
	xmlFile.OutOfElem(); //Triangles
	xmlFile.OutOfElem(); //Surface

	delete[] tins;
	delete[] retPts;

	//依次对侧面如下处理
	int bTopDir = GraphAPI::GIsClockwise(bLeft?ptsVL:ptsVR,nBoundStop), bSideDir;
	PT_3D pts3L[4], pts3R[4], *pts3, rets3[4];

	CArray<int,int> indexs;
	if( pGeo->GetClassType()==CLS_GEOREN )
	{
		CGeoRen_GetCylinderIndex(pts.GetData(),pts.GetSize(),indexs);
	}
	if( indexs.GetSize()<=1 )
	{
		indexs.RemoveAll();
		for( i=0; i<nBoundStop; i++ )
		{
			indexs.Add(i);
		}
	}
	int nSideNum = indexs.GetSize()-1;

	CArray<int,int> arrNULL;
	CArray<float,float> arrTexValue;
	CStringArray arrTexPath;

	arrNULL.SetSize(nSideNum*2);
	arrTexValue.SetSize(nSideNum*8);
	arrTexPath.SetSize(nSideNum);
	int *pn = arrNULL.GetData();
	float *pftv = arrTexValue.GetData();

	for( int i5=0; i5<nSideNum; i5++ )
	{
		i = indexs[i5];
		int i1 = indexs[i5+1];

		//1)利用顶面边和它的投影做Z型三角剖分
		pts3L[0] = ptsVL[i];  pts3L[1] = ptsPL[i];
		pts3L[2] = ptsPL[i1];  pts3L[3] = ptsVL[i1];

		pts3R[0] = ptsVR[i];  pts3R[1] = ptsPR[i];
		pts3R[2] = ptsPR[i1];  pts3R[3] = ptsVR[i1];

		bOutArea = FALSE;
		bLeft = FALSE;

		if( nImg==0 )bLeft = TRUE;
		else if( nImg==1 )bLeft = FALSE;
		else 
		{
			//点是否超出了影像范围
			BOOL bOutArea1 = !CheckImageCoordinates(image1,pts3L,4);
			BOOL bOutArea2 = !CheckImageCoordinates(image2,pts3R,4);
			int bD1 = GraphAPI::GIsClockwise(pts3L,4);
			int bD2 = GraphAPI::GIsClockwise(pts3R,4);

			//都超出了，就没得找了
			if( bOutArea1 && bOutArea2 )bOutArea = TRUE;

			//左片超出，就取右片
			else if( bOutArea1 && !bOutArea2 )
			{
				//如果顶面方向和该片方向相同，就可取，否则，也是不可取
				if( (bTopDir==0 && bD2==0) || (bTopDir==1 && bD2==1) )
					bLeft = FALSE;
				else
					bOutArea = TRUE;
			}
			//右片超出，就取左片
			else if( !bOutArea1 && bOutArea2 )
			{
				//如果顶面方向和该片方向相同，就可取，否则，也是不可取
				if( (bTopDir==1 && bD1==1) || (bTopDir==1 && bD1==1) )
					bLeft = TRUE;
				else
					bOutArea = TRUE;
			}
			//如果都没有超出，就检查看哪个面积最大
			else
			{
				//跟右边方向相同，就取右片
				if( (bTopDir==0 && bD1==1 && bD2==0) || (bTopDir==1 && bD1==0 && bD2==1) )
					bLeft = FALSE;
				//跟左边方向相同，就取左片
				else if( (bTopDir==0 && bD2==1 && bD1==0) || (bTopDir==1 && bD2==0 && bD1==1) )
					bLeft = TRUE;
				//跟两片方向相同，就取面积大的
				else if( (bTopDir==0 && bD1==0 && bD2==0) || (bTopDir==1 && bD1==1 && bD2==1) )
					bLeft = GraphAPI::GGetPolygonArea(pts3L,4)>GraphAPI::GGetPolygonArea(pts3R,4);
				else
					bOutArea = TRUE;
			}

			/*
			int bD1 = GIsClockwise(pts3L,4);
			int bD2 = GIsClockwise(pts3R,4);
			if( bD1!=bD2 )bLeft = bD1;
			else 
			bLeft = GGetPolygonArea(pts3L,4)>GGetPolygonArea(pts3R,4);*/
		}

		pts3 = (bLeft?pts3L:pts3R);

		//理论上来说，投影到影像上时，顶面的点绕向和侧面的点绕向应该是相反的
		bSideDir = GraphAPI::GIsClockwise(pts3,4);
		
		if( !bExportSideFace || bTopDir<0 /*|| bSideDir<0 || bTopDir==bSideDir*/ || bOutArea )
		{
			pn[i5*2] = 0;
			pn[i5*2+1] = no;
			strImg = _T("NULL");
			memset(rets3,0,sizeof(rets3));
		}
		
		//2)挖出纹理并计算纹理坐标
		else 
		{
			pn[i5*2] = 1;
			pn[i5*2+1] = no;
			strImg = MakeTexture(pDoc,pts3,4,rets3,bLeft?image1:image2,no, path);
		}

		arrTexPath.SetAt(i5,strImg);

		//输出纹理
		CString strValue;
		strValue.Format("Surface%d",no++);
		xmlFile.AddElem("Surface");
		xmlFile.AddAttrib("name",(LPCTSTR)strValue);
		xmlFile.IntoElem();
		xmlFile.AddElem("Texture",(LPCTSTR)strImg);

		//大地坐标
		pts3[0].x = geopts[i].x;  pts3[0].y = geopts[i].y;  pts3[0].z = geopts[i].z;  
		pts3[1].x = geopts[i].x;  pts3[1].y = geopts[i].y;  pts3[1].z = z0;  
		pts3[2].x = geopts[i1].x;  pts3[2].y = geopts[i1].y;  pts3[2].z = z0; 
		pts3[3].x = geopts[i1].x;  pts3[3].y = geopts[i1].y;  pts3[3].z = geopts[i1].z;  
		xmlFile.AddElem("RenderVertexs");
		xmlFile.IntoElem();
		for( j=0; j<4; j++)
		{
			strValue.Format("%.4f %.4f %.4f %.4f %.4f",pts3[j].x,pts3[j].y,pts3[j].z,
				rets3[j].x,rets3[j].y);
			xmlFile.AddElem("RV",(LPCTSTR)strValue);

			pftv[i5*8+j*2] = rets3[j].x; pftv[i5*8+j*2+1] = rets3[j].y;
		}
		xmlFile.OutOfElem(); //RenderVertexs

		//输出三角形数据
		xmlFile.AddElem("Triangles");
		xmlFile.IntoElem();

		// 侧面按逆时针导出
		if (bTopDir == 0)
		{
			xmlFile.AddElem("Triangle","0 3 2");
 			xmlFile.AddElem("Triangle","2 1 0");
		}
		else
		{
 			xmlFile.AddElem("Triangle","0 1 2");
 			xmlFile.AddElem("Triangle","2 3 0");
		}

		xmlFile.OutOfElem(); //Triangles
		xmlFile.OutOfElem(); //Surface
	}

	xmlFile.SavePos();

	//用合适的纹理填补空纹理
	if( bFixNULL )
	{
		for( i5=0; i5<nSideNum; i5++)
		{
			i = indexs[i5];
			int i1 = indexs[i5+1];

			if( pn[i5*2] )continue;
			CString strValue;
			strValue.Format("Surface%d",pn[i5*2+1]);
			if( xmlFile.FindElem("Surface",TRUE) )
			{
				BOOL bFind = FALSE;
				do 
				{
					if( xmlFile.GetAttrib("name")==strValue )bFind = TRUE;
				} while( !bFind && xmlFile.FindElem("Surface") );
				
				if( !bFind )continue;

				//找合适的纹理
				float min = -1;
				k = -1;
				for( int j5=0; j5<nSideNum; j5++ )
				{
					j = indexs[j5];
					int j1 = indexs[j5+1];
					//该纹理必须非空
					if( pn[j*2]==0 )continue;

					//形状最接近
					float d1 = sqrt((keys[i1].x-keys[i].x)*(keys[i1].x-keys[i].x) +	(keys[i1].y-keys[i].y)*(keys[i1].y-keys[i].y));
					float d2 = sqrt((keys[j1].x-keys[j].x)*(keys[j1].x-keys[j].x) +	(keys[j1].y-keys[j].y)*(keys[j1].y-keys[j].y));
					float d3 = fabs((keys[i].z-z0));
					float d4 = fabs((keys[j].z-z0));

					float s1 = (d2==0.0)?0xffff:(d1/d2), s2 = (d4==0.0)?0xffff:(d3/d4);
					if( s1==0.0 )s1 = 0xffff;
					else if( s1<1.0 )s1 = 1.0/s1;
					if( s2==0.0 )s2 = 0xffff;
					else if( s2<1.0 )s2 = 1.0/s2;

					s1 = s1*s2;

					if( k<0 || min>s1 )
					{
						min = s1;
						k = j5;
					}
				}

				if( k>=0 )
				{
					xmlFile.IntoElem();
					if( xmlFile.FindElem("Texture",TRUE) )
					{
						xmlFile.SetData(arrTexPath.GetAt(k));
					}
					if( xmlFile.FindElem("RenderVertexs",TRUE) )
					{
						xmlFile.RemoveElem();

						int i0 = i;
						pts3[0].x = geopts[i0].x;  pts3[0].y = geopts[i0].y;  pts3[0].z = geopts[i0].z;  
						pts3[1].x = geopts[i0].x;  pts3[1].y = geopts[i0].y;  pts3[1].z = z0; 
						pts3[2].x = geopts[i1].x;  pts3[2].y = geopts[i1].y;  pts3[2].z = z0;  
						pts3[3].x = geopts[i1].x;  pts3[3].y = geopts[i1].y;  pts3[3].z = geopts[i1].z;  
						
						xmlFile.AddElem("RenderVertexs");
						xmlFile.IntoElem();
						for( j=0; j<4; j++)
						{
							strValue.Format("%.4f %.4f %.4f %.4f %.4f",pts3[j].x,pts3[j].y,pts3[j].z,
								pftv[k*8+j*2],pftv[k*8+j*2+1]);
							xmlFile.AddElem("RV",(LPCTSTR)strValue);
						}
						xmlFile.OutOfElem(); //RenderVertexs
					}

					xmlFile.OutOfElem(); //Surface
				}
			}
		}
	}

	xmlFile.RestorePos();


	xmlFile.OutOfElem(); //SurfaceList
	xmlFile.OutOfElem(); //Render
#endif
}

//从正射影像提取纹理
void MakeRender_fromOrtho(CDlgDoc *pDoc, CMarkup& xmlFile, CGeometry *pGeo, CCoordSys& cs,
				double z0, CImageRead& image1, LPCTSTR path, int& no)
{
#ifndef _NOT_USE_DPWPACK
	int i,j,k,num;

	//得到顶面每个点以及其投影点的象点坐标
	CArray<PT_3D,PT_3D> arrVertexsL, arrRets, arrKeys;
	CArray<PT_3DEX,PT_3DEX> pts;
	pGeo->GetShape(pts);
	num = pts.GetSize();

	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		pts.Add(pts[0]);
		num++;
	}

	arrVertexsL.SetSize(num);
	arrRets.SetSize(num);
	arrKeys.SetSize(num);
	PT_3D *ptsVL = arrVertexsL.GetData();
	PT_3D *rets = arrRets.GetData();
	PT_3D *keys = arrKeys.GetData();
	memset(ptsVL,0,sizeof(PT_3D)*num);
	int nBoundStop = -1;
	
	Coordinate coord1,coord2; 
	PT_3DEX expt;
	for( i=0; i<num; i++ )
	{
		expt = pts[i];
		
		//penMove之后不再是边界部分了
		if( pts[i].pencode==penMove && nBoundStop==-1 )
			nBoundStop = i;

		cs.GroundToClient(&expt,ptsVL+i);
	}

	if( nBoundStop<0 )
		nBoundStop = num;

	BOOL bLeft = TRUE, bOutArea = FALSE;

	bOutArea = !CheckImageCoordinates(image1,ptsVL,num);

	//挖出顶面的纹理(存储为影像文件)并计算纹理坐标
	CString strImg = _T("NULL");
	if( bOutArea )
		memset(rets,0,sizeof(PT_3D)*num);
	else
		strImg = MakeTexture(pDoc,ptsVL,num,rets,image1,no, path);

	//三角剖分
	CArray<MYPT_3D,MYPT_3D> arrGeoPt;
	arrGeoPt.SetSize(num);
	MYPT_3D *geopts = arrGeoPt.GetData();
	int line_id = 0;
	for( i=0; i<num; i++ )
	{
		//尖顶点或者屋顶脊线构成独立的特征线，参与三角剖分
		if( i==0 || pts[i].pencode==penMove )
		{
			line_id++;
		}
		geopts[i].id = line_id;

		if( pts[i].pencode==penMove && i==(num-1) )
			geopts[i].type = 0;
		else
			geopts[i].type = 1;

		expt = pts[i];
		COPY_3DPT(keys[i],expt);
		COPY_3DPT(geopts[i],expt);
	}

	//人字形房屋，屋脊线可能与房边缘相交，为了确保三角网构建正确，我们将两头的房屋边线的连线段去掉
	if( pGeo->GetClassType()==CLS_GEOREN || pGeo->GetClassType()==CLS_GEOREN2 )
	{		
		int nRidgePtNum = CGeoRen_GetRidgePtNum(pts.GetData(),num); 
		if( nRidgePtNum>=2 && (num==(nRidgePtNum*3+1)) )
		{
			for( i=0; i<nRidgePtNum; i++ )
			{
				geopts[i].id = 1;				
				geopts[i].type = 1;
			}
			for( ; i<nRidgePtNum+nRidgePtNum; i++ )
			{
				geopts[i].id = 2;				
				geopts[i].type = 1;
			}
			i++;
			for( ; i<num; i++ )
			{
				geopts[i].id = 3;				
				geopts[i].type = 1;
			}
		}		
	}

	KickoffSameGeoPts(arrGeoPt);
	geopts = arrGeoPt.GetData();

	if (arrGeoPt.GetSize() < 3) return;

	MYTIN *tins = NULL;
	int nTin = 0;
	
	MYPT_3D *retPts = NULL;
	int nRetPt = 0;
	MyTriangle::createTIN(geopts,arrGeoPt.GetSize(),&tins,&nTin,&retPts,&nRetPt);

	//准备
	xmlFile.AddElem("Render");
	xmlFile.IntoElem();
	xmlFile.AddElem("SurfaceList");
	xmlFile.IntoElem();
	CString strValue;
	strValue.Format("Surface%d",no++);
	xmlFile.AddElem("Surface");
	xmlFile.AddAttrib("name",(LPCTSTR)strValue);
	xmlFile.IntoElem();

	///输出纹理
	xmlFile.AddElem("Texture",(LPCTSTR)strImg);
	xmlFile.AddElem("RenderVertexs");
	xmlFile.IntoElem();
	for( i=0; i<num; i++ )
	{
		strValue.Format("%.4f %.4f %.4f %.4f %.4f",
			geopts[i].x,geopts[i].y,geopts[i].z,rets[i].x,rets[i].y);
		xmlFile.AddElem("RV",(LPCTSTR)strValue);
	}
	xmlFile.OutOfElem(); //RenderVertexs

	//输出三角形数据
	MYPT_3D  tri[3];

	xmlFile.AddElem("Triangles");
	xmlFile.IntoElem();
	for( i=0; i<nTin+2; i++)
	{
		MYTIN tin;
		
		if( i<nTin )
		{
			tin = tins[i];
			if( tin.i1<0 || tin.i1>nRetPt ||
				tin.i2<0 || tin.i2>nRetPt ||
				tin.i3<0 || tin.i3>nRetPt )
			{
				continue;
			}

			tin.i1 = retPts[tin.i1].iold;
			tin.i2 = retPts[tin.i2].iold;
			tin.i3 = retPts[tin.i3].iold;

			tri[0] = geopts[tin.i1];
			tri[1] = geopts[tin.i2];
			tri[2] = geopts[tin.i3];

			//判断三角形是否在外边
			PT_3D cpt;
			cpt.x = (tri[0].x+tri[1].x+tri[2].x)/3; cpt.y = (tri[0].y+tri[1].y+tri[2].y)/3; cpt.z = 0;
			if( GraphAPI::GIsPtInRegion(cpt,keys,nBoundStop)!=2 )
				continue;
			
			//人字顶的两头的三角形不需要
			if( pGeo->GetClassType()==CLS_GEOREN )
			{
				if( CGeoRen_CheckEndTriangle(pts.GetData(),pts.GetSize(),tri) )
					continue;
			}
			
			if( pGeo->GetClassType()==CLS_GEOREN2 )
			{
				if( CGeoRen_CheckEndTriangle(pts.GetData(),pts.GetSize(),tri) )
					continue;
			}
		}
		else
		{
			//斜人字顶的两头三角形 手工制作
			if( pGeo->GetClassType()==CLS_GEOREN2 )
			{
				if( !CGeoRen_GetEndTriangle(pts.GetData(),pts.GetSize(),i-nTin,tin) )
					continue;

				COPY_3DPT(tri[0],pts[tin.i1]);
				COPY_3DPT(tri[1],pts[tin.i2]);
				COPY_3DPT(tri[2],pts[tin.i3]);
			}
			else
			{
				continue;
			}
		}


		//寻找当前点序号
		int idx[3] = {-1};
		for( j=0; j<3; j++ )
		{
			double dis, min = -1;
			for( k=0; k<num; k++ )
			{
				dis = fabs(tri[j].x-geopts[k].x)+fabs(tri[j].y-geopts[k].y);
				if( min<0 || min>dis )
				{
					min = dis; 
					idx[j] = k;
				}
			}
		}

		//调整顺序，使最小的序号在前
		int idxm = 0;
		k = num+2;
		for( j=0; j<3; j++ )
		{
			if( k>idx[j] )
			{
				k = idx[j];
				idxm = j;
			}
		}

		strValue.Format("%d %d %d",idx[idxm],idx[(idxm+1)%3],idx[(idxm+2)%3]);
		xmlFile.AddElem("Triangle",strValue);
	}
	xmlFile.OutOfElem(); //Triangles
	xmlFile.OutOfElem(); //Surface

	delete[] tins;
	delete[] retPts;

	//依次对侧面如下处理
	int bTopDir = GraphAPI::GIsClockwise(ptsVL,nBoundStop), bSideDir;
	PT_3D pts3[4], rets3[4];

	CArray<int,int> indexs;
	if( pGeo->GetClassType()==CLS_GEOREN )
	{
		CGeoRen_GetCylinderIndex(pts.GetData(),pts.GetSize(),indexs);
	}
	if( indexs.GetSize()<=1 )
	{
		indexs.RemoveAll();
		for( i=0; i<nBoundStop; i++ )
		{
			indexs.Add(i);
		}
	}
	int nSideNum = indexs.GetSize()-1;

	for( int i5=0; i5<nSideNum; i5++ )
	{
		i = indexs[i5];
		int i1 = indexs[i5+1];

		//1)利用顶面边和它的投影做Z型三角剖分
		
		strImg = _T("NULL");
		memset(rets3,0,sizeof(rets3));

		//输出纹理
		CString strValue;
		strValue.Format("Surface%d",no++);
		xmlFile.AddElem("Surface");
		xmlFile.AddAttrib("name",(LPCTSTR)strValue);
		xmlFile.IntoElem();
		xmlFile.AddElem("Texture",(LPCTSTR)strImg);

		//大地坐标
		pts3[0].x = geopts[i].x;  pts3[0].y = geopts[i].y;  pts3[0].z = geopts[i].z;  
		pts3[1].x = geopts[i].x;  pts3[1].y = geopts[i].y;  pts3[1].z = z0;  
		pts3[2].x = geopts[i1].x;  pts3[2].y = geopts[i1].y;  pts3[2].z = z0; 
		pts3[3].x = geopts[i1].x;  pts3[3].y = geopts[i1].y;  pts3[3].z = geopts[i1].z;  
		xmlFile.AddElem("RenderVertexs");
		xmlFile.IntoElem();
		for( j=0; j<4; j++)
		{
			strValue.Format("%.4f %.4f %.4f %.4f %.4f",pts3[j].x,pts3[j].y,pts3[j].z,
				rets3[j].x,rets3[j].y);
			xmlFile.AddElem("RV",(LPCTSTR)strValue);
		}
		xmlFile.OutOfElem(); //RenderVertexs

		//输出三角形数据
		xmlFile.AddElem("Triangles");
		xmlFile.IntoElem();

		// 侧面按逆时针导出
		if (bTopDir == 0)
		{
			xmlFile.AddElem("Triangle","0 3 2");
 			xmlFile.AddElem("Triangle","2 1 0");
		}
		else
		{
 			xmlFile.AddElem("Triangle","0 1 2");
 			xmlFile.AddElem("Triangle","2 3 0");
		}

		xmlFile.OutOfElem(); //Triangles
		xmlFile.OutOfElem(); //Surface
	}


	xmlFile.OutOfElem(); //SurfaceList
	xmlFile.OutOfElem(); //Render
#endif
}

int GetStereoImagePath(CoreObject &core, LPCTSTR stereID, CString& str1, CString& str2)
{
	//得到影像路径
	CString imgFile;
	int i,j;
	for( i=0; i<core.iStereoNum; i++)
	{
		if( core.stereo[i].sp.stereoID.CompareNoCase(stereID)==0 )
		{
			str1 = core.stereo[i].imageID[0];
			str2 = core.stereo[i].imageID[1];
			break;
		}
	}
	if( i<core.iStereoNum )
	{
		BOOL bFind1 = FALSE, bFind2 = FALSE;
		for( i=0; i<core.iStripNum; i++)
		{
			for( j=0; j<core.strip[i].iImageNum; j++)
			{
				if( !bFind1 && str1.CompareNoCase(core.strip[i].image[j].strImageID )==0 )
				{
					str1 = core.strip[i].image[j].ie.strFileName;
					bFind1 = TRUE;
				}
				if( !bFind2 && str2.CompareNoCase(core.strip[i].image[j].strImageID )==0 )
				{
					str2 = core.strip[i].image[j].ie.strFileName;
					bFind2 = TRUE;
				}
				
				if( bFind1 && bFind2 )break;
			}
			
			if( bFind1 && bFind2 )break;
		}
		
		int ret = 0;
		if( bFind1 )ret += 1;
		if( bFind2 )ret += 2;
		return ret;
	}
	
	return 0;
}

static void ChangeClockwise(Pt3dArray& arr)
{
	PT_3DEX *pts = arr.GetData();
	int npt = arr.GetSize();
	
	BOOL bHavePenMove = FALSE;
	int nBoundStop = -1;
	for( int i=0; i<npt; i++)
	{
		if( pts[i].pencode==penMove )
		{
			nBoundStop = i;
			bHavePenMove = TRUE;
			break;
		}
	}
	
	if( nBoundStop<0 )
		nBoundStop = npt;
	
	if( GraphAPI::GIsClockwise(pts,nBoundStop)==1 )
	{
		PT_3DEX expt;
		for( i=0; i<nBoundStop/2; i++)
		{
			expt = pts[i];
			pts[i] = pts[nBoundStop-1-i];
			pts[nBoundStop-1-i] = expt;
		}
		
		if( bHavePenMove )
		{
			pts[0].pencode = penLine;
			pts[nBoundStop].pencode = penMove;
		}
	}
}


static int gnCvtBaseType = PHOTO;

int TestPoint(PT_3DEX *pt, Envelope e, CPtrArray& arr, CArray<QUADRANGLE,QUADRANGLE> &arrModelBounds, CUIntArray *arrsStereoIndexs)
{
	double minArea = -1;
	int num = arr.GetSize(), find = -1;
	CUIntArray arrSameAreaIndex;
	for( int i=0; i<num; i++)
	{
		CCoordCenter *pCenter = (CCoordCenter*)arr[i];
		if( !pCenter )continue;

		PT_3D pt3ds[4];
		memset(pt3ds,0,sizeof(pt3ds));
		
		QUADRANGLE quad = arrModelBounds[i];

		/*RECT rect = pCenter->GetModelBound(&quad);*/
		
		for( int j=0; j<4; j++)
		{
			pt3ds[j].x = quad.lfX[j];
			pt3ds[j].y = quad.lfY[j];
			pt3ds[j].z = 0;
		}

		Envelope e0;
		e0.CreateFromPts(pt3ds,4,sizeof(PT_3D),3);
		
		//Envelope e0 = arrModelBounds[i];
		e0.Intersect(&e,2);

		double area = e0.Width()*e0.Width() + e0.Height()*e0.Height();
		if (minArea < 0 || area > minArea || fabs(area-minArea) < 1e-6)
		{
			minArea = area;
			find = i;

			if (!(fabs(area-minArea) < 1e-6))
			{
				arrSameAreaIndex.RemoveAll();
			}

			arrSameAreaIndex.Add(i);
		}
	}

	int areaSize = arrSameAreaIndex.GetSize();
	if (areaSize < 2)
	{
		return find;
	}

	double dis,min=-1;
	num = areaSize, find = -1;
	Coordinate coord1, coord2;
	COPY_3DPT(coord1,(*pt));
	coord1.iType = GROUND, coord2.iType = gnCvtBaseType;
	for( i=0; i<num; i++)
	{
		int index = arrSameAreaIndex[i];
		CCoordCenter *pCenter = (CCoordCenter*)arr[index];
		if( !pCenter )continue;
			
		if( !pCenter->Convert(coord1,coord2) )continue;

		PT_3D pt3ds[4];
		memset(pt3ds,0,sizeof(pt3ds));

		QUADRANGLE quad = arrModelBounds[index];		
		for( int j=0; j<4; j++)
		{
			pt3ds[j].x = quad.lfX[j];
			pt3ds[j].y = quad.lfY[j];
			pt3ds[j].z = 0;
		}

		double fWidth = GraphAPI::GGet2DDisOf2P(pt3ds[0],pt3ds[1]), fHeight = GraphAPI::GGet2DDisOf2P(pt3ds[1],pt3ds[2]);


		dis = coord2.lx*coord2.lx/(fWidth*fWidth)+coord2.ly*coord2.ly/(fHeight*fHeight);
		if( min<0 || dis<min )
		{
			min = dis;
			find = index;
		}
	}

	// 检测是否有效
	BOOL bValid = FALSE, bFirst = TRUE;
	int i0 = find;
	while (!bValid && i0 < num)
	{
		CCoordCenter *pCenter = (CCoordCenter*)arr[i0];
		if( !pCenter ) return -1;
		
		PT_3D expt = *pt;
		
		Coordinate coord1,coord2;
		coord1.x=expt.x; coord1.y=expt.y; coord1.z=expt.z;  
		coord1.iType=GROUND; coord2.iType=SCANNER;
		if( !pCenter->Convert(coord1,coord2) ) return -1;
		
		PT_3D ptVL;
		ptVL.x=coord2.lx;  ptVL.y=coord2.ly;
		
		CoreObject core = pCenter->coreobj;
		
		int stereoIndex = i0;
		if (arrsStereoIndexs)
		{
			stereoIndex = arrsStereoIndexs->GetAt(i0);
		}

		//影像路径
		CString imgFile1, imgFile2;			
		GetStereoImagePath(core,core.stereo[stereoIndex].sp.stereoID,imgFile1,imgFile2);
		
		CImageRead image1, image2;
		image1.Load(imgFile1);
		image2.Load(imgFile2);
		
		if ( CheckImageCoordinates(image1,&ptVL,1) || CheckImageCoordinates(image2,&ptVL,1) )
		{
			find = i0;
			bValid = TRUE;
		}
		else
		{
			if (bFirst)
			{
				i0 = 0;
				bFirst = FALSE;
			}
			else
			{
				i0++;
				if (i0 == find)
				{
					i0++;
				}
			}
		}
		
		
	} 
	
	
	return find;
}

void LoadMatrix(LPCTSTR path, CCoordSys& cs)
{
	CGeoBuilderPrj prj;
	GeoHeader head;
	if( prj.IsGeoImage(path,head) )
	{
		double m[9], m2[9];
		
		matrix_toIdentity(m,3);
		m[2] = head.lfStartX; m[5] = head.lfStartY;
		m[0] = head.lfDx*head.lfScale*0.001*cos(head.lfKapa); 
		m[1] = head.lfDy*head.lfScale*0.001*sin(head.lfKapa); 
		m[3] =-head.lfDx*head.lfScale*0.001*sin(head.lfKapa); 
		m[4] = head.lfDy*head.lfScale*0.001*cos(head.lfKapa);
		
		matrix_reverse(m,3,m2);
		cs.Create33Matrix(m2);
		
		CString name = path;
		name = name.Left(name.GetLength()-3) + "tfw";
		FILE *fp = fopen(name,"rt");
		if( fp )
		{
			// CGeoBuilderPrj 并不支持带有旋转角的参数，这里对此情况特别处理；
			// 将基于左上角像素的参数转为基于左下角像素的参数
			double v[6];
			if( fscanf(fp,"%lf%lf%lf%lf%lf%lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5])==6 )
			{
				matrix_toIdentity(m,3);
				m[0] = v[0];
				m[1] = v[1];
				m[3] = -v[2];
				m[4] = -v[3];
				m[2] = v[4] + v[1]*head.iRow;
				m[5] = v[5] + v[3]*head.iRow;
				
				matrix_reverse(m,3,m2);
				cs.Create33Matrix(m2);
			}
			fclose(fp);
		}
	}
}

void CDlgDoc::OnExportModel()
{
	CDlgExportModel dlg;
	int nlay, nobj, nsel, nptsum, lSum=0, i,j,k;
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;

	nlay = pDS->GetFtrLayerCount();

	for( i=0; i<nlay; i++ )
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer) continue;
		if( !pLayer->IsVisible() )continue;

		int num = pLayer->GetObjectCount();
		if (num > 0)
		{
			dlg.m_strInitList.Add(pLayer->GetName());
			lSum += num;
			continue;
		}
	}

	if( dlg.DoModal()!=IDOK )return;

	//初始化DEM
	BOOL bDemValid = FALSE;
	CArray<CDSM*,CDSM*> arrDem;
	for ( i=0; i<dlg.m_arrDEMFileNames.GetSize(); i++)
	{
		CDSM *dem = new CDSM;
		if (dem == NULL) return;
		if (dem->Open(dlg.m_arrDEMFileNames[i]))
		{
			arrDem.Add(dem);
			bDemValid = TRUE;
		}
	}
// 	CDSM dem;
// 	BOOL bDemValid = TRUE;
// 	if( !dem.Open(dlg.m_strDEMFile) )
// 		bDemValid = FALSE;

	//初始化坐标换算器
	CCoordCenter coord, *pcoord = NULL;
	CPtrArray arrCoords;
	CUIntArray arrPrjIndexs, arrsStereoIndexs;

	CArray<QUADRANGLE,QUADRANGLE> arrModelBounds;
//	CoreObject core = dlg.m_prj.GetCoreObject();

	CArray<CImageRead*,CImageRead*> arrImages;
	CImageRead *image1 = NULL, *image2 = NULL;
	CCoordSys cs;
	if( dlg.m_bTexture )
	{
		//从正射影像提取纹理
		if( dlg.m_nTextureSource==1 && !dlg.m_strOrthoFile.IsEmpty() )
		{
			image1 = new CImageRead;
			image1->Load(dlg.m_strOrthoFile);

			LoadMatrix(dlg.m_strOrthoFile,cs);

			arrImages.InsertAt(0,image1);			
		}
		else if( dlg.m_strStereo.GetLength()>0 )
		{
			CoreObject core;
			if (dlg.m_nCurPrj >= 0 && dlg.m_nCurPrj < dlg.m_arrPrjs.GetSize())
			{
				core = dlg.m_arrPrjs[dlg.m_nCurPrj]->GetCoreObject();
			}

			if( !coord.Init(core,dlg.m_strStereo) )
				return;

			//影像路径
			CString imgFile1, imgFile2;			
			GetStereoImagePath(core,dlg.m_strStereo,imgFile1,imgFile2);

			{
				image1 = new CImageRead;
				image1->Load(imgFile1);
				arrImages.InsertAt(0,image1);
			}
			
			{
				image2 = new CImageRead;
				image2->Load(imgFile2);
				arrImages.InsertAt(0,image2);
			}
			
		}
		else
		{
			for ( i=0; i<dlg.m_arrPrjs.GetSize(); i++)
			{
				CoreObject core = dlg.m_arrPrjs[i]->GetCoreObject();
				CPtrArray coords;
				coords.SetSize(core.iStereoNum);
				for( j=0; j<core.iStereoNum; j++)
				{	
					CString imgFile1, imgFile2;			
					GetStereoImagePath(core,core.stereo[j].sp.stereoID,imgFile1,imgFile2);
					if (_taccess(LPCTSTR(imgFile1),0)==-1 || _taccess(LPCTSTR(imgFile2),0)==-1)
					{
						coords.SetAt(j,0);
						arrModelBounds.Add(QUADRANGLE());
					}
					else
					{
						pcoord = new CCoordCenter;
						if( pcoord )
						{
							pcoord->Init(core,core.stereo[j].sp.stereoID);
							coords.SetAt(j,pcoord);

							QUADRANGLE quad;
							pcoord->GetModelBound(&quad);

							/*PT_3D pt3ds[4];
							memset(pt3ds,0,sizeof(pt3ds));
							for( int j=0; j<4; j++)
							{
								pt3ds[j].x = quad.lfX[j];
								pt3ds[j].y = quad.lfY[j];
								pt3ds[j].z = 0;
							}
							
							Envelope e;
							e.CreateFromPts(pt3ds,4,sizeof(PT_3D),3);*/
							arrModelBounds.Add(quad);
						}
						else
						{
							coords.SetAt(j,0);
							arrModelBounds.Add(QUADRANGLE());
						}
					}
					
					arrPrjIndexs.Add(i);
					arrsStereoIndexs.Add(j);
				}	
				
				arrCoords.Append(coords);
				
			}

			pcoord = NULL;

// 			arrCoords.SetSize(core.iStereoNum);
// 			for( i=0; i<core.iStereoNum; i++)
// 			{				
// 				pcoord = new CCoordCenter;
// 				if( pcoord )
// 				{
// 					pcoord->Init(core,core.stereo[i].sp.stereoID);
// 					arrCoords.SetAt(i,pcoord);
// 				}
// 				else
// 					arrCoords.SetAt(i,0);
// 			}
// 
// 			pcoord = NULL;
		}
	}

	//路径
	CString path = dlg.m_strMdlFile;
	int pos = path.ReverseFind('\\');
	if( pos>=0 )path.Delete(pos+1,path.GetLength()-pos-1);
	path += _T("images\\");
	::CreateDirectory(path,NULL);

//	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lSum);
	GProgressStart(lSum);

	nsel = dlg.m_strSelList.GetSize();	

	//导出文件
	//文件头
	CMarkup xmlFile;
	xmlFile.AddElem("BodyList");
	xmlFile.IntoElem();

	//遍历对象
	CFeature *pFtr;
	PT_3DEX expt;
	char strVertex[256];
	CString strFID;
	DpCustomFieldType type;
	int no = 0;

	for( i=0; i<nlay; i++ )
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if( pLayer )
		{
			if( !pLayer->IsVisible() )continue;

			nobj = pLayer->GetObjectCount();
			for( j=0; j<nobj; j++)
			{
//				AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, lSum);
				GProgressStep();
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;

				CGeometry *pGeo = pFtr->GetGeometry();
				if (!pGeo) continue; 

				//只导出面和闭合线
				if( pGeo->GetClassType()>=CLS_GEOJIAN && pGeo->GetClassType()<=CLS_GEOHUI )
					;
				else
				if( !(pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface))||
					(pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))&&((CGeoCurve*)pGeo)->IsClosed())) )
					continue;

				//判断对象是否在需要导出的特征码表中
				strFID = pLayer->GetName();
				for( k=0; k<nsel; k++ )
				{
					if( strFID.CompareNoCase(dlg.m_strSelList[k])==0 )
						break;
				}
				if( k>=nsel )continue;

				Envelope e = pGeo->GetEnvelope();

				//如果有标签属性，就以它作为名称
// 				pGeo->GetBind()->GetAttrValue("LABEL",var,type);
// 				if( type==DP_CFT_VARCHAR && strFID.CollateNoCase((LPCTSTR)var)==0 )
// 					strFID = (LPCTSTR)var;

				CValueTable tab;
				tab.BeginAddValueItem();
				pFtr->WriteTo(tab);
				tab.EndAddValueItem();

				const CVariantEx* pval; 
				if ( tab.GetValue(0,"LABEL",pval) )
				{
					strFID = (const char*)(_bstr_t)(_variant_t)pval;
				}				

				//通过DEM计算对象的基点
				double z0 = 0, z1 = 0;
				float fZ;
				CArray<PT_3DEX,PT_3DEX> arr;
				pGeo->GetShape(arr);
				nptsum = arr.GetSize();
				if( bDemValid )
				{
					for( k=0; k<nptsum; k++)
					{
						expt = arr[k];
						for (int l=0; l<arrDem.GetSize(); l++)
						{
							fZ = arrDem[l]->GetZ(expt.x,expt.y);
							if (fZ > DemNoValues+1)
							{
								break;
							}
						}
						//fZ = dem.GetZ(expt.x,expt.y);
						z1 = fZ;
						if( k==0 || z0>z1 )z0 = z1;
					}

				}

				ChangeClockwise(arr);

				if (arrImages.GetSize() > 5)
				{
					for (int m=arrImages.GetSize()-1; m>=3; m--)
					{
						delete (CImageRead*)arrImages[m];
						arrImages.RemoveAt(m);
					}
				}

				//回字形屋顶
				if( pGeo->GetClassType()==CLS_GEOHUI )
				{
					//导出对象
					xmlFile.AddElem("Body");
					xmlFile.AddAttrib("name",strFID);
					xmlFile.AddAttrib("value",j);
					
					unsigned char* szUUID;
					GUID id = pFtr->GetID().ToGUID();
					UuidToString(&id,&szUUID);
					xmlFile.AddAttrib("uuid",(char*)szUUID);
					RpcStringFree(&szUUID);
					
					xmlFile.IntoElem();

					//计算第一个顶面的平均高程，作为其他子面的基准面高程
					double za = 0;
					int nSubObj = CGeoHui_GetTopSurfaceNum(pGeo);

					if( nSubObj>0 )
					{
						Pt3dArray ptsRet;
						CGeoHui_GetTopSurfaceShape(pGeo,0,ptsRet);
						if( ptsRet.GetSize()>=4 )
						{
							for( int m=0; m<ptsRet.GetSize(); m++)
							{
								za += ptsRet[m].z;
							}
							za /= ptsRet.GetSize();
						}
					}

					for( int k=0; k<nSubObj; k++)
					{
						Pt3dArray ptsRet;
						CGeoHui_GetTopSurfaceShape(pGeo,k,ptsRet);

						if( k>0 )
							z0 = za;

						if( ptsRet.GetSize()>=4 )
						{
							nptsum = ptsRet.GetSize();
							for( int m=0; m<nptsum; m++)
							{
								ptsRet[m].pencode = penLine;
							}

							CGeoCurve *pCurve = new CGeoCurve();
							pCurve->CreateShape(ptsRet.GetData(),ptsRet.GetSize());

							xmlFile.AddElem("SubBody");
							xmlFile.IntoElem();
							xmlFile.AddElem("Inhesion");
							xmlFile.IntoElem();
							xmlFile.AddElem("BodyType","DLGFeatures");
							
							//基点
							xmlFile.AddElem("KeyEle");
							xmlFile.IntoElem();
							xmlFile.AddElem("EleType","Datum");
							xmlFile.AddElem("ConnectType","Projection");
							sprintf(strVertex,"%.6f",z0);
							xmlFile.AddElem("Value",strVertex);
							xmlFile.OutOfElem();
							
							//对象（目前只是面）
							xmlFile.AddElem("KeyEle");
							xmlFile.IntoElem();
							xmlFile.AddElem("EleType","Surface");
							xmlFile.AddElem("ConnectType","Projection");
							xmlFile.AddElem("Vertexs");
							xmlFile.IntoElem();
							for( m=0; m<nptsum-1; m++)
							{
								expt = ptsRet[m];
								sprintf(strVertex,"%.6f %.6f %.6f %d",expt.x,expt.y,expt.z,expt.pencode);
								xmlFile.AddElem("V",strVertex);
							}
							xmlFile.OutOfElem(); //Vertexs
							xmlFile.OutOfElem(); //KeyEle
							
							xmlFile.OutOfElem(); //Inhesion
							
							if( !dlg.m_bTexture )
							{
								delete pCurve;
								xmlFile.OutOfElem(); //SubBody
								continue;
							}
							
							//输出纹理
							//从正射影像提取纹理
							if( dlg.m_nTextureSource==1 && !dlg.m_strOrthoFile.IsEmpty() )
							{
								MakeRender_fromOrtho(this,xmlFile,pCurve,cs,z0,*image1,path,no);
							}
							else if( dlg.m_strStereo.GetLength()>0 )
								MakeRender(this,xmlFile,pCurve,coord,z0,*image1,*image2,dlg.m_nLeftImg,path,dlg.m_bExportSideFace,dlg.m_bFixHide,no);
							else
							{
								//找到合适的模型					
								PT_3D pt3d1, pt3d2;
								expt = arr[0];
								COPY_3DPT(pt3d1,expt);
								pCurve->GetCenter(&pt3d1,&pt3d2);
								COPY_3DPT(expt,pt3d2);
								int idx = TestPoint(&expt,e,arrCoords,arrModelBounds,&arrsStereoIndexs);
								
								if( idx>=0 && idx<arrCoords.GetSize() )
								{
									pcoord = (CCoordCenter*)arrCoords.GetAt(idx);
									CoreObject core = dlg.m_arrPrjs[arrPrjIndexs[idx]]->GetCoreObject();
									
// 									image1.Close();
// 									image2.Close();
									
									//影像路径
									CString imgFile1, imgFile2;			
									GetStereoImagePath(core,core.stereo[arrsStereoIndexs[idx]/*idx*/].sp.stereoID,imgFile1,imgFile2);
									
									image1 = image2 = NULL;
									for (int m=0; m<arrImages.GetSize(); m++)
									{
										LPCTSTR imgFileName = arrImages[m]->GetImageName();
										if (stricmp(imgFileName,imgFile1) == 0)
										{
											image1 = arrImages[m];
										}
										else if (stricmp(imgFileName,imgFile2) == 0)
										{
											image2 = arrImages[m];
										}
									}
									
									if (!image1)
									{
										image1 = new CImageRead;
										image1->Load(imgFile1);
										arrImages.InsertAt(0,image1);
									}
									
									if (!image2)
									{
										image2 = new CImageRead;
										image2->Load(imgFile2);
										arrImages.InsertAt(0,image2);
									}
									
									MakeRender(this,xmlFile,pCurve,*pcoord,z0,*image1,*image2,dlg.m_nLeftImg,path,dlg.m_bExportSideFace,dlg.m_bFixHide,no);
								}
								else
								{
									//image1.Close();
									//image2.Close();
									CImageRead image11, image22;
									MakeRender(this,xmlFile,pCurve,*pcoord,z0,image11,image22,dlg.m_nLeftImg,path,dlg.m_bExportSideFace,dlg.m_bFixHide,no);
								}
							}

							delete pCurve;
							
							xmlFile.OutOfElem(); //SubBody
						}
					}

					xmlFile.OutOfElem(); //Body

				}
				else
				{
					//导出对象
					xmlFile.AddElem("Body");
					xmlFile.AddAttrib("name",strFID);
					xmlFile.AddAttrib("value",j);
					
					unsigned char* szUUID;
					GUID id = pFtr->GetID().ToGUID();
					UuidToString(&id,&szUUID);
					xmlFile.AddAttrib("uuid",(char*)szUUID);
					RpcStringFree(&szUUID);
					
					xmlFile.IntoElem();
					xmlFile.AddElem("SubBody");
					xmlFile.IntoElem();
					xmlFile.AddElem("Inhesion");
					xmlFile.IntoElem();
					xmlFile.AddElem("BodyType","DLGFeatures");
					
					//基点
					xmlFile.AddElem("KeyEle");
					xmlFile.IntoElem();
					xmlFile.AddElem("EleType","Datum");
					xmlFile.AddElem("ConnectType","Projection");
					sprintf(strVertex,"%.6f",z0);
					xmlFile.AddElem("Value",strVertex);
					xmlFile.OutOfElem();
					
					//对象（目前只是面）
					xmlFile.AddElem("KeyEle");
					xmlFile.IntoElem();
					xmlFile.AddElem("EleType","Surface");
					xmlFile.AddElem("ConnectType","Projection");
					xmlFile.AddElem("Vertexs");
					xmlFile.IntoElem();

					if( pGeo->GetClassType()>=CLS_GEOJIAN && pGeo->GetClassType()<=CLS_GEOHUI )
					{
						for( k=0; k<nptsum; k++)
						{							
							if( k<(nptsum-1) && arr[k+1].pencode==penMove )
								continue;

							expt = arr[k];
							sprintf(strVertex,"%.6f %.6f %.6f %d",expt.x,expt.y,expt.z,expt.pencode);
							xmlFile.AddElem("V",strVertex);
						}
					}
					else
					{
						for( k=0; k<nptsum-1; k++)
						{
							expt = arr[k];
							sprintf(strVertex,"%.6f %.6f %.6f %d",expt.x,expt.y,expt.z,expt.pencode);
							xmlFile.AddElem("V",strVertex);
						}
					}

					xmlFile.OutOfElem(); //Vertexs
					xmlFile.OutOfElem(); //KeyEle
					
					xmlFile.OutOfElem(); //Inhesion
					
					if( !dlg.m_bTexture )
					{
						xmlFile.OutOfElem(); //SubBody
						xmlFile.OutOfElem(); //Body
						continue;
					}
					
					//输出纹理
					//从正射影像提取纹理
					if( dlg.m_nTextureSource==1 && !dlg.m_strOrthoFile.IsEmpty() )
					{
						MakeRender_fromOrtho(this,xmlFile,pGeo,cs,z0,*image1,path,no);
					}
					else if( dlg.m_strStereo.GetLength()>0 )
						MakeRender(this,xmlFile,pGeo,coord,z0,*image1,*image2,dlg.m_nLeftImg,path,dlg.m_bExportSideFace,dlg.m_bFixHide,no);
					else
					{
						//找到合适的模型					
						PT_3D pt3d1, pt3d2;
						expt = arr[0];
						COPY_3DPT(pt3d1,expt);
						pGeo->GetCenter(&pt3d1,&pt3d2);
						COPY_3DPT(expt,pt3d2);
						int idx = TestPoint(&expt,e,arrCoords,arrModelBounds,&arrsStereoIndexs);
						
						if( idx>=0 && idx<arrCoords.GetSize() )
						{
							pcoord = (CCoordCenter*)arrCoords.GetAt(idx);
							CoreObject core = dlg.m_arrPrjs[arrPrjIndexs[idx]]->GetCoreObject();
							
							//image1.Close();
							//image2.Close();
							
							//影像路径
							CString imgFile1, imgFile2;			
							GetStereoImagePath(core,core.stereo[arrsStereoIndexs[idx]/*idx*/].sp.stereoID,imgFile1,imgFile2);
							
							image1 = image2 = NULL;
							for (int m=0; m<arrImages.GetSize(); m++)
							{
								LPCTSTR imgFileName = arrImages[m]->GetImageName();
								if (stricmp(imgFileName,imgFile1) == 0)
								{
									image1 = arrImages[m];
								}
								else if (stricmp(imgFileName,imgFile2) == 0)
								{
									image2 = arrImages[m];
								}
							}
							
							if (!image1)
							{
								image1 = new CImageRead;
								image1->Load(imgFile1);
								arrImages.InsertAt(0,image1);
							}
							
							if (!image2)
							{
								image2 = new CImageRead;
								image2->Load(imgFile2);
								arrImages.InsertAt(0,image2);
							}
							
							MakeRender(this,xmlFile,pGeo,*pcoord,z0,*image1,*image2,dlg.m_nLeftImg,path,dlg.m_bExportSideFace,dlg.m_bFixHide,no);
						}
						else
						{
							//image1.Close();
							//image2.Close();
							CImageRead image11, image22;
							MakeRender(this,xmlFile,pGeo,*pcoord,z0,image11,image22,dlg.m_nLeftImg,path,dlg.m_bExportSideFace,dlg.m_bFixHide,no);
						}
					}
					
					xmlFile.OutOfElem(); //SubBody
					xmlFile.OutOfElem(); //Body
				}

			}
		}
	}

	xmlFile.OutOfElem(); //BodyList

	if( arrCoords.GetSize()>0 )
	{
		for( i=0; i<arrCoords.GetSize(); i++)
		{
			if( arrCoords.GetAt(i)!=NULL )
				delete (CCoordCenter*)arrCoords.GetAt(i);
		}
	}

	if( arrDem.GetSize()>0 )
	{
		for( i=0; i<arrDem.GetSize(); i++)
		{
			if( arrDem.GetAt(i)!=NULL )
				delete (CDSM*)arrDem.GetAt(i);
		}
	}

	if( arrImages.GetSize()>0 )
	{
		for( i=0; i<arrImages.GetSize(); i++)
		{
			if( arrImages.GetAt(i)!=NULL )
				delete (CImageRead*)arrImages.GetAt(i);
		}

		arrImages.RemoveAll();
	}

	GProgressEnd();
	GOutPut(StrFromResID(IDS_DOC_EXPORT_WRITING));
// 	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
// 	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_DOC_EXPORT_WRITING) );

	//写文件
#ifdef _ENCODE_UNICODE
	//转换为Unicode格式
	CString strDoc = xmlFile.GetDoc();
	CTextFileWrite wfile(dlg.m_strMdlFile,CTextFileBase::UNI16_LE);
	if (wfile.IsOpen()==FALSE)
	{
		CString strMsg;
		strMsg.Format(IDS_DOC_WRITEFILE_ERR,dlg.m_strMdlFile);
		AfxMessageBox(strMsg);
	}
	else
	{
		wfile.SetCodePage(0);
		wfile<<strDoc;
	}
#else
	xmlFile.Save(dlg.m_strMdlFile);
#endif

	GOutPut(StrFromResID(IDS_DOC_EXPORT_END));
//	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_DOC_EXPORT_END) );
}

void CDlgDoc::OnExportMapStarText()
{
	CDlgExportMapStarText dlg;
	if( dlg.DoModal()!=IDOK )
		return;
	
	CMapStarWrite wr;
	wr.Open(dlg.m_strDir,GetPathName());
	wr.SetPointListFile(dlg.m_strLstFile);
	wr.SetCurveListFile(dlg.m_strLstFile);
	wr.m_lfContourInterval = dlg.m_fContourInterval;
	
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;

	//获取实体对象总数以及点的数目
	CFeature *pFtr = NULL;
	long lObjSum = 0, lPtSum = 0;
	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
//		DWORD oldmode = pLayer->GetGetMode();
//		pLayer->SetGetMode(GETMODE_INC_LOCKED|oldmode);
		
		int nObjs = pLayer->GetObjectCount();
		lObjSum += nObjs;
		
		for(int j=0; j<nObjs; j++)
		{
			pFtr = pLayer->GetObject(j);	
			if( !pFtr || !pFtr->IsVisible())continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo)continue;
			
			lPtSum += pGeo->GetDataPointSum();
		}
		
//		pLayer->SetGetMode(oldmode);
	}
	
	GProgressStart(lObjSum);
//	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lObjSum);
	
	PT_3D bounds[4];
//	GetWorkSpaceBound(bounds);
	pDS->GetBound(bounds,NULL,NULL);
	wr.Bound(bounds);
	
	long number = 0;
	//遍历所有层
	for ( i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
//		DWORD oldmode = pLayer->GetGetMode();
//		pLayer->SetGetMode(GETMODE_INC_LOCKED|oldmode);
		
		if(!pLayer|| !pLayer->IsVisible() )continue;
		int nObjs = pLayer->GetObjectCount();
		
		for(int j=0; j<nObjs; j++)
		{
			pFtr = pLayer->GetObject(j);
			GProgressStep();
//			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, lObjSum);
			if( !pFtr )continue;
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo)continue;

			wr.Object(pFtr,pLayer,pDS);
		}
		
//		pLayer->SetGetMode(oldmode);
	}
	
	wr.Close();
	
	//进度条复位
	GProgressEnd();
//	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
}

void CDlgDoc::ObjectsToDxf(CPtrArray& arrFtrs, LPCTSTR strDxf)
{
	CString pathName = strDxf;
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS)
	{
		return;
	}
	
	CDxfWrite dxfWrite;
	dxfWrite.SetAsExportBaselines();
	dxfWrite.OpenWrite(strDxf);
	dxfWrite.SetDlgDataSource(pDS);
	int ObjSum = arrFtrs.GetSize();
	
	GProgressStart(ObjSum);

	CPFeature pFtr = NULL;
	CFtrLayer* pFtrLayer = NULL;
	for (int i=0;i<ObjSum;i++)
	{
		GProgressStep();

		pFtr = (CPFeature)arrFtrs[i];
		pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
		dxfWrite.SaveFtrLayer(pFtrLayer);
		dxfWrite.SaveFeature(pFtr);			
	}
	dxfWrite.CloseWrite();		
	
	GProgressEnd();
}

void CDlgDoc::ObjectsToDxf2(CPtrArray& arrFtrs, LPCTSTR strDxf)
{
	CDxfWrite dxfWrite;
	dxfWrite.SetDlgDataSource(GetDlgDataSource());
	dxfWrite.SetAsExportBaselines();
	CPtrArray arrLayer;
	dxfWrite.ObjectsToDxf(arrFtrs,arrLayer,strDxf);	
}

void CDlgDoc::OnTestCreateContourSettings()
{
	CDlgTestContourSettings dlg;
	
	CString strRegPath = "Config\\TestCreateContour";
	
	dlg.m_fInterval = GetProfileDouble(strRegPath,"Interval",5.0);
	dlg.m_strLayers = AfxGetApp()->GetProfileString(strRegPath,"Layers",dlg.m_strLayers);
	
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	
	int nlay = pDS->GetFtrLayerCount();

	int lSum = 0;
	
	for(int i=0; i<nlay; i++ )
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer||pLayer->IsLocked()||!pLayer->IsVisible()) continue;
		
		int num = pLayer->GetEditableObjsCount();
		if (num > 0)
		{
			dlg.m_strInitList.Add(pLayer->GetName());
			lSum += num;
			continue;
		}
	}

	if( dlg.DoModal()!=IDOK )
		return;
	
	WriteProfileDouble(strRegPath,"Interval",dlg.m_fInterval);
	AfxGetApp()->WriteProfileString(strRegPath,"Layers",dlg.m_strLayers);
}

void GetFileWriteTime(LPCTSTR filename, FILETIME *pTime)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	memset(&attr,0,sizeof(attr));
	GetFileAttributesEx(filename,GetFileExInfoStandard,&attr);

	*pTime = attr.ftLastWriteTime;
}

BOOL CDlgDoc::ReadTempRefData(CString strPathName, CDlgDataSource *&pDS)
{
	char database[256]={0};

	_CheckLoadPath((void*)(LPCTSTR)strPathName,database,sizeof(database));
	
	if(strPathName.Right(4).CompareNoCase(_T(".tmp")) == 0)
	{
		CSQLiteAccess  *pAccess = new CSQLiteAccess;
		if (!pAccess) return FALSE;

		pAccess->Attach(strPathName);
		
		pDS = new CDlgDataSource(m_pDataQuery);	
		if( !pDS ) 
		{
			delete pAccess;
			return FALSE;
		}
		int pos = strPathName.ReverseFind(_T('.'));
		pDS->SetName(strPathName.Left(pos));
		pDS->SetAccessObject(pAccess);

		pAccess->ReadDataSourceInfo(pDS);

		// 
		Envelope evlp = GetDlgDataSource(0)->GetBound();
		evlp.m_xl -= m_fRefBoundExt; evlp.m_yl -= m_fRefBoundExt;
		evlp.m_xh += m_fRefBoundExt; evlp.m_yh += m_fRefBoundExt;
		
		//加载范围内数据
		pAccess->BatchUpdateBegin();		
		
		long idx;
		int lSum = pAccess->GetFtrLayerCount();
		lSum += pAccess->GetFeatureCount();
		
		GOutPut(StrFromResID(IDS_LOAD_FEATURES));
		GProgressStart(lSum);	
		
		CFtrLayer *pLayer = pAccess->GetFirstFtrLayer(idx);
		while( pLayer )
		{
			GProgressStep();
			
			if( !pDS->AddFtrLayer(pLayer) )
				delete pLayer;
			else
			{
//				if( pDS->GetCurFtrLayer()==NULL )
//					pDS->SetCurFtrLayer(pLayer->GetID());
			}
			
			pLayer = pAccess->GetNextFtrLayer(idx);	
			
		}
		
		int id;
		CFeature *pFt = pAccess->GetFirstFeature(idx);
		
		while( pFt )
		{
			GProgressStep();
			
			id = pAccess->GetCurFtrLayID();

			Envelope e = pFt->GetGeometry()->GetEnvelope();

			if (m_nRefAll==1&&!evlp.bIntersect(&e))
			{
				pFt = pAccess->GetNextFeature(idx);
				continue;
			}
			
			if( pDS->SetCurFtrLayer(id) && pDS->AddObject(pFt,id,TRUE) );
			else
				delete pFt;
			
			pFt = pAccess->GetNextFeature(idx);
			
		}
		
		GProgressEnd();
		GOutPut(StrFromResID(IDS_LOAD_FINISH));		
	
		pAccess->BatchUpdateEnd();

// 		GetFileWriteTime(strPathName,&pDS->writetime);
		
		//断开与引用数据文件的联系
//		pDS->SetAccessObject(NULL);		
		
		pLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
		if( pLayer )
		{
			pDS->SetCurFtrLayer(pLayer->GetID());
		}
		return TRUE;
	}	
	return FALSE;
}
BOOL CDlgDoc::ReadRefData(CString strPathName, CDlgDataSource *&pDS)
{
	char database[256]={0};
	_CheckLoadPath((void*)(LPCTSTR)strPathName,database,sizeof(database));

	if(strPathName.Right(4).CompareNoCase(_T(".fdb")) == 0)
	{
		CSQLiteAccess  *pAccess = new CSQLiteAccess;
		if (!pAccess) return FALSE;

		//数据源关联临时文件
		CString path(strPathName);
		if( _taccess(LPCTSTR(path),0)!=-1  )
		{
			;
		}
		else
		{
			CString temp;
			temp.Format(IDS_REFDATA_NOT_EXIST,path);
			AfxMessageBox(temp);
			return FALSE;
		}
		int pos = path.ReverseFind(_T('.'));		
		if(pos == -1)
			return FALSE;
		path += _T(".tmp");
		//	path.Insert(pos,_T("~tmp"));
		if( _taccess(LPCTSTR(path),0)!=-1  )
		{		
			if( AfxMessageBox(IDS_DOC_FIND_TMPFILE,MB_YESNO)==IDYES )
			{		
				::CopyFile(path,database,FALSE);		
			}
			else
			{
				m_strPathName = strPathName;
				m_strTmpFile = path;	
				::CopyFile(strPathName,path,FALSE);
			}
		}
		else
		{
			::CopyFile(strPathName,path,FALSE);
		}
		CString strTmpFile = path;
		if( !pAccess->Attach(LPCTSTR(strTmpFile)))
		{
			delete pAccess;
			AfxMessageBox(StrFromResID(IDS_FILEFORMAT_ERROR));
			return FALSE;
		}
		if (!((CSQLiteAccess*)pAccess)->IsFileVersionLatest())
		{
			AfxMessageBox(IDS_STRING_NEWDOC);
			delete pAccess;
			::DeleteFile(strTmpFile);
			return FALSE;
		}
		pDS = new CDlgDataSource(m_pDataQuery);	
		if( !pDS ) 
		{
			delete pAccess;
			return FALSE;
		}
		pDS->SetName(strPathName);
		pDS->SetAccessObject(pAccess);
		pAccess->ReadDataSourceInfo(pDS);

		Envelope evlp = GetDlgDataSource(0)->GetBound();
		evlp.m_xl -= m_fRefBoundExt; evlp.m_yl -= m_fRefBoundExt;
		evlp.m_xh += m_fRefBoundExt; evlp.m_yh += m_fRefBoundExt;
		pAccess->BatchUpdateBegin();		
		long idx;
		int lSum = pAccess->GetFtrLayerCount();
		lSum += pAccess->GetFeatureCount();
		GOutPut(StrFromResID(IDS_LOAD_FEATURES));
		GProgressStart(lSum);	
		CFtrLayer *pLayer = pAccess->GetFirstFtrLayer(idx);
		while( pLayer )
		{
			GProgressStep();
			if( !pDS->AddFtrLayer(pLayer) )
				delete pLayer;
			else
			{
//				if( pDS->GetCurFtrLayer()==NULL )
//					pDS->SetCurFtrLayer(pLayer->GetID());
			}
			pLayer = pAccess->GetNextFtrLayer(idx);	
		}

		pDS->LoadLayerSymbolCache();

		int id;
		CFeature *pFt = pAccess->GetFirstFeature(idx);
		while( pFt )
		{
			GProgressStep();
			id = pAccess->GetCurFtrLayID();
			Envelope e = pFt->GetGeometry()->GetEnvelope();
			if (m_nRefAll==1&&!evlp.bIntersect(&e))
			{
				pFt = pAccess->GetNextFeature(idx);
				continue;
			}
			if( pDS->SetCurFtrLayer(id) && pDS->AddObject(pFt,id,TRUE) );
			else
				delete pFt;
			pFt = pAccess->GetNextFeature(idx);
		}
		GProgressEnd();
		GOutPut(StrFromResID(IDS_LOAD_FINISH));		
		pAccess->BatchUpdateEnd();

		pLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
		if( pLayer )
		{
			pDS->SetCurFtrLayer(pLayer->GetID());
		}
		pDS->SetModifiedFlag(FALSE);
		return TRUE;		
	}
	else// if (UVSModify == GetAccessType(GetDlgDataSource()))//uvs
	{
		CUVSModify *pAccess = new CUVSModify();
		if (!pAccess) return FALSE;

		//参考网络数据时，先用只读的方式打开
		if(!pAccess->OpenRead(strPathName))
		{
			pAccess->Close();
			CString temp;
			temp.Format(IDS_REFDATA_NOT_EXIST,strPathName);
			AfxMessageBox(temp);
			return FALSE;
		}

		pDS = new CDlgDataSource(m_pDataQuery);	
		if( !pDS ) 
		{
			delete pAccess;
			return FALSE;
		}
		pAccess->SetDataSource(pDS);
		pDS->SetName(strPathName);
		pDS->SetAccessObject(pAccess);
		pAccess->ReadDataSourceInfo(pDS);

		Envelope evlp = GetDlgDataSource(0)->GetBound();
		evlp.m_xl -= m_fRefBoundExt; evlp.m_yl -= m_fRefBoundExt;
		evlp.m_xh += m_fRefBoundExt; evlp.m_yh += m_fRefBoundExt;
		pAccess->BatchUpdateBegin();		
		long idx;
		int lSum = pAccess->GetFtrLayerCount();
		lSum += pAccess->GetFeatureCount();
		GOutPut(StrFromResID(IDS_LOAD_FEATURES));
		GProgressStart(lSum);	
		CFtrLayer *pLayer = pAccess->GetFirstFtrLayer(idx);
		while( pLayer )
		{
			GProgressStep();
			if( !pDS->AddFtrLayer(pLayer) )
				delete pLayer;
			else
			{
//				if( pDS->GetCurFtrLayer()==NULL )
//					pDS->SetCurFtrLayer(pLayer->GetID());
			}
			pLayer = pAccess->GetNextFtrLayer(idx);	
		}

		pDS->LoadLayerSymbolCache();

		int id;
		CFeature *pFt = pAccess->GetFirstFeature(idx);
		while( pFt )
		{
			GProgressStep();
			id = pAccess->GetCurFtrLayID();
			Envelope e = pFt->GetGeometry()->GetEnvelope();
			if (m_nRefAll==1&&!evlp.bIntersect(&e))
			{
				pFt = pAccess->GetNextFeature(idx);
				continue;
			}
			if( pDS->SetCurFtrLayer(id) && pDS->AddObject(pFt,id,TRUE) );
			else
				delete pFt;
			pFt = pAccess->GetNextFeature(idx);
		}
		GProgressEnd();
		GOutPut(StrFromResID(IDS_LOAD_FINISH));		
		pAccess->BatchUpdateEnd();

		pLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
		if( pLayer )
		{
			pDS->SetCurFtrLayer(pLayer->GetID());
		}
		pDS->SetModifiedFlag(FALSE);
		return TRUE;
	}

	return FALSE;
}

void CDlgDoc::ReadRefInfo(CStringArray &arrDSName)
{
	arrDSName.RemoveAll();
	CString infofile = m_strPathName + _T(".ref");
	
	FILE *fp = fopen(infofile,"r");
	if( !fp )return;
	
	char line[1024];
	CString text;
	while( !feof(fp) )
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line),fp);
		text = line;
		text.Remove(_T('\n'));
		text.Remove(_T('\r'));
		if( text.GetLength()>=2 )
		{
			arrDSName.Add(text);			
		}
	}
	fclose(fp);
}


void CDlgDoc::SaveRefInfo()
{
	if( GetDlgDataSource()==NULL )return;

	if (AccessType::UVSModify == GetDlgDataSource()->GetAccessObject()->GetAccessType())
		return;
	
	CString infofile = m_strPathName + _T(".ref");
	
	if( GetDlgDataSourceCount()<=1 )
	{
		::DeleteFile(infofile);
		return;
	}
	
	FILE *fp = fopen(infofile,"w");
	if( !fp )return;
	CDlgDataSource *pRefDS = NULL;
	int nCount = 0;
	for( int i=1; i<GetDlgDataSourceCount(); i++)
	{
		pRefDS  = GetDlgDataSource(i);
		if (AccessType::UVSModify == pRefDS->GetAccessObject()->GetAccessType())
			continue;
		fprintf(fp, "%s\n",(LPCTSTR)pRefDS->GetName());
		nCount++;
	}
	fclose(fp);

	if (nCount <= 0)
	{
		::DeleteFile(infofile);
	}
}
namespace
{
	struct DSDisplayChange 
	{
		CDlgDataSource *pDS;
		BOOL bShowBoundOld;
		BOOL bShowBoundNew;
		BOOL bEnableMonoOld;
		BOOL bEnableMonoNew;
		COLORREF clrMonoOld;
		COLORREF clrMonoNew;
	};
}

void CDlgDoc::UpdateRefDatafile(CArray<dataParam,dataParam&>& data_params)
{
	CString path;
	
	float old_boundext = m_fRefBoundExt;
	int nRef = m_arrData.GetSize();


	WriteProfileDouble("Config\\Document","RefBoundExt",m_fRefBoundExt);
	
	BOOL bAddOrDel = FALSE;
	//	m_snap.Init(m_pDataSource);
	
	CDlgDataSource *pRefDS = NULL;
	CDlgDataSource *pDelActiveDS = NULL;		
	
	dataParam itemParam;
	CDlgDataSource *pActiveDS = GetDlgDataSource(m_nActiveData);
// 	if(UVSModify==GetAccessType(pActiveDS))
// 	{
// 		int ret = CUVSModify::WaiteExecuteAll();
// 		if(IDCANCEL == ret || IDYES == ret)
// 			return;
// 	}
	CString strOldActiveDS = GetDlgDataSource(m_nActiveData)->GetName();
	CString strNewActiveDS = _T("");
	BOOL bDelActiveData = FALSE;
	int nData = data_params.GetSize();
	CArray<DSDisplayChange*,DSDisplayChange*> arrDisChange;
	for(int i=0; i<nData; i++)
	{
		itemParam = data_params[i];
		if (itemParam.strDataState.CompareNoCase(_T("Add"))==0)
		{
			if (itemParam.bIsActive)
			{
				strNewActiveDS = itemParam.strDataName;
			}
			CDlgDataSource *pNewDS = NULL;
			if(ReadRefData(itemParam.strDataName,pNewDS))
			{
				m_arrData.Add(pNewDS);
				bAddOrDel = TRUE;
				pNewDS->EnableShowBound(itemParam.bShowBound);
				pNewDS->EnableMonocolor(&(itemParam.bEnableMono),&(itemParam.clrMono));
			}
		}
		else
		{
			pRefDS = GetDlgDataSource(i);
			if (!pRefDS)
			{
				continue;
			}
			if (itemParam.strDataState.CompareNoCase(_T("Del"))==0)
			{
				if (strOldActiveDS==itemParam.strDataName)
				{
					pDelActiveDS = pRefDS;						
					m_arrData.SetAt(i,NULL);
					bDelActiveData = TRUE;
				}
				else
				{
					if (itemParam.bIsActive)
					{
						strNewActiveDS =_T("");
					}
					CString temp = pRefDS->GetTempFilePath();
					delete pRefDS;
					::DeleteFile(temp);
					m_arrData.SetAt(i,NULL);
				}
				bAddOrDel = TRUE;
			}
			else
			{
				if (itemParam.bIsActive)
				{
					strNewActiveDS = itemParam.strDataName;
				}
				DSDisplayChange *pTemp = new DSDisplayChange;
				arrDisChange.Add(pTemp);
				pTemp->pDS = pRefDS;
				pTemp->bShowBoundOld = pRefDS->GetShowBound();
				pRefDS->GetMonoColor(&(pTemp->bEnableMonoOld),&(pTemp->clrMonoOld));
				pRefDS->EnableShowBound(itemParam.bShowBound);
				pRefDS->EnableMonocolor(&(itemParam.bEnableMono),&(itemParam.clrMono));
				pTemp->bShowBoundNew = pRefDS->GetShowBound();
				pRefDS->GetMonoColor(&(pTemp->bEnableMonoNew),&(pTemp->clrMonoNew));
			}
		}
	}
	for (i=m_arrData.GetSize()-1;i>=0;i--)
	{
		if (m_arrData[i]==NULL)
		{
			m_arrData.RemoveAt(i);
		}
	}
	if (strNewActiveDS.IsEmpty())
	{
		if (bDelActiveData)
		{
			SaveModified(pDelActiveDS);
			CString temp = pDelActiveDS->GetTempFilePath();
			delete pDelActiveDS;
			::DeleteFile(temp);
			ActiveDataSource(m_arrData[0]);
		}
		else
		{
			for (int i=0;i<m_arrData.GetSize();i++)//无需改变激活数据源(m_nActiveData可能会更改)
			{
				if (pActiveDS==m_arrData[i])
				{
					m_nActiveData = i;
					break;
				}
			}
			//				m_snap.AddSource(ref.pData);
		}
	}
	else
	{
		if (strNewActiveDS==strOldActiveDS)
		{
			for (int i=0;i<m_arrData.GetSize();i++)//无需改变激活数据源(m_nActiveData可能会更改)
			{
				if (pActiveDS==m_arrData[i])
				{
					m_nActiveData = i;
					break;
				}
			}
		}
		else
		{
			//激活指定数据源/*(可能因为版本问题而激活失败）*/
			//若失败，则要么保持原激活数据源不变，要么激活主数据源（因为原激活数据源要被删除）
			if (bDelActiveData)
			{
				//原激活数据源的保存(它的删除操作也延迟到这儿进行）
				SaveModified(pDelActiveDS);
				CString temp = pDelActiveDS->GetTempFilePath();
				delete pDelActiveDS;
				::DeleteFile(temp);
				ActiveDataSource(strNewActiveDS);
			}
			else
			{
				SaveModified(strOldActiveDS);					
				ActiveDataSource(strNewActiveDS);
			}
			// 		else
			// 		{
			// 			if( ref.pData )
			// 				m_snap.AddSource(ref.pData);
			// 		}
		}
	}
	if(bAddOrDel)
	{
		SaveRefInfo();
		UpdateAllViews(NULL,hc_UpdateAllObjects);
	}
	else
	{
		BOOL bShowBoundFlag = FALSE;
		for (int i=0;i<arrDisChange.GetSize();i++)
		{
			if(arrDisChange[i]->bShowBoundNew!=arrDisChange[i]->bShowBoundOld)
				bShowBoundFlag = TRUE;
			if(arrDisChange[i]->bEnableMonoOld==arrDisChange[i]->bEnableMonoNew)
			{
				if (arrDisChange[i]->bEnableMonoOld)
				{
					if (arrDisChange[i]->clrMonoNew!=arrDisChange[i]->clrMonoOld)
					{
						UpdateAllViews(NULL,hc_UpdateDataColor,(CObject*)arrDisChange[i]->pDS);
						bShowBoundFlag = TRUE;
					}
				}					
			}
			else
			{
				UpdateAllViews(NULL,hc_UpdateDataColor,(CObject*)arrDisChange[i]->pDS);
				bShowBoundFlag = TRUE;
			}
		}
		if (bShowBoundFlag)
		{
			UpdateAllViews(NULL,hc_UpdateOverlayBound);
		}
	}
	for ( i=0;i<arrDisChange.GetSize();i++)
	{
		delete arrDisChange[i];
	}
	arrDisChange.RemoveAll();
}

void CDlgDoc::OnReferenceDatafile() 
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS)
	{
		return;
	}
	CDlgRefDataFile dlg;
	
	if(dlg.DoModal()==IDOK)
	{
		UpdateRefDatafile(dlg.m_arrDataParams);

		UpdateView(0,hc_UpdateRefFileText,NULL);
		//
		gpMainWnd->m_wndFtrLayerView.Refresh();
	}
}

void CDlgDoc::OnResetDatafileBound()
{
	CDlgRefDataBound dlg;
	dlg.m_nOption = m_nRefAll;
	dlg.m_fBoundExt = m_fRefBoundExt;
	if (dlg.DoModal()==IDOK)
	{
		m_nRefAll = dlg.m_nOption;
		m_fRefBoundExt = dlg.m_fBoundExt;
		AfxGetApp()->WriteProfileInt("Config\\Document","RefIsAll",m_nRefAll);
		WriteProfileDouble("Config\\Document","RefBoundExt",m_fRefBoundExt);
		BOOL bEnable;
		COLORREF clr;
		BOOL bShowBound;
		int num = GetDlgDataSourceCount();
		for( int i=1; i<num; i++)
		{
			CDlgDataSource *pDS = GetDlgDataSource(i);

			if(UVSModify == GetAccessType(pDS))
			{
				if(i==m_nActiveData)
				{
					CString path = pDS->GetName();
					BOOL bModify = pDS->IsModified();
					pDS->GetMonoColor(&bEnable, &clr);
					bShowBound = pDS->GetShowBound();
					delete pDS;
					if(ReadTempRefData(path, pDS))
					{
						pDS->EnableMonocolor(&bEnable, &clr);
						pDS->EnableShowBound(bShowBound);
						m_arrData.SetAt(i, pDS);
						m_pDataQuery->m_pFilterDataSource = pDS;
					}
					else
						m_arrData.RemoveAt(i);
					pDS->SetModifiedFlag(bModify);
				}
				else
				{
					CString path = pDS->GetName();
					pDS->GetMonoColor(&bEnable, &clr);
					bShowBound = pDS->GetShowBound();
					delete pDS;
					if(ReadRefData(path, pDS))
					{
						pDS->EnableMonocolor(&bEnable, &clr);
						pDS->EnableShowBound(bShowBound);
						m_arrData.SetAt(i, pDS);
					}
					else
						m_arrData.RemoveAt(i);
				}
			}
			else if (pDS != NULL)
			{
				//比较时间是否一致
				//	FILETIME time2;
				//	CString path = pDS->GetName();
				//	GetFileWriteTime(path,&time2);
				//	if( CTime(pDS->writetime)!=CTime(time2) )
				{
					if (i==m_nActiveData)
					{
						BOOL bModify = pDS->IsModified();
						CString path = pDS->GetName()/*GetAccessObject()->GetReadName()*/;
						path+=_T(".tmp");
						pDS->GetMonoColor(&bEnable,&clr);
						bShowBound = pDS->GetShowBound();
						delete pDS;
						if (ReadTempRefData(path,pDS))
						{
							pDS->EnableMonocolor(&bEnable,&clr);
							pDS->EnableShowBound(bShowBound);
							m_arrData.SetAt(i,pDS);
							m_pDataQuery->m_pFilterDataSource = pDS;
						}
						else
							m_arrData.RemoveAt(i);
						pDS->SetModifiedFlag(bModify);
					}
					else
					{
						CString path = pDS->GetName();
						CString tempPath = pDS->GetTempFilePath();
						pDS->GetMonoColor(&bEnable,&clr);
						bShowBound = pDS->GetShowBound();
						delete pDS;
						DeleteFile(tempPath);
						if (ReadRefData(path,pDS))
						{
							pDS->EnableMonocolor(&bEnable,&clr);
							pDS->EnableShowBound(bShowBound);
							m_arrData.SetAt(i,pDS);				
						}
						else
							m_arrData.RemoveAt(i);
					}					
				}
			}
			else
			{
				m_arrData.RemoveAt(i);
			}
		}
		DeleteUndoItems();
		UpdateView(0,hc_UpdateRefFileText,NULL);
		UpdateAllViews(NULL, hc_UpdateAllObjects);
		UpdateAllViews(NULL,hc_UpdateOverlayBound,NULL);
	}
	
}

void CDlgDoc::OnUpdateReferenceDatafile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CDlgDoc::OnAnchorptRecord() 
{
	m_nAnchorState = 1;
}

void CDlgDoc::OnAnchorptJump() 
{
	if( m_nAnchorState==2 )
	{
		UpdateAllViews(NULL,hc_SetAnchorPoint,(CObject*)&m_ptAnchor);
	}
}

void CDlgDoc::OnRefreshDrawing() 
{	
	for( int i=0; i<m_arrData.GetSize(); i++)
	{
		m_arrData[i]->ClearLayerSymbolCache();
		m_arrData[i]->LoadLayerSymbolCache();
	}

	UpdateAllViews(NULL,hc_UpdateAllObjects);
}

void ChangeDem(LPCTSTR src, LPCTSTR des, int nFormat)
{
	CDSM dem;
	dem.Open(src);
	float *p = dem.m_lpHeight;
	DEMHEADERINFO dh;
	dh = dem.m_demInfo;
	
	DEMHEADERINFO demInfo;
	demInfo.lfStartX = dh.lfStartX; demInfo.lfStartY = dh.lfStartY;
	demInfo.lfDx = dh.lfDx; demInfo.lfDy = dh.lfDy;
	demInfo.nRow = dh.nRow;
	demInfo.nColumn = dh.nColumn;
	demInfo.nType = nFormat;
	demInfo.strFilePath = des;
	demInfo.nScale = 1000;
	demInfo.lfNoData=-99999;
	dem.Save(des,nFormat,&demInfo,p);
}

void CDlgDoc::OnBuildDEM() 
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS)
	{
		return;
	}
	CDlgBuildDEM dlg;
	if( dlg.DoModal()!=IDOK )return;

	if( dlg.m_fDX<=0||dlg.m_strDEMPath.IsEmpty() )
		return;

	{
		CString strDxfFile = GetPathName();
		int pos = strDxfFile.ReverseFind(_T('.'));
		if( pos>=0 )strDxfFile.Delete(pos,strDxfFile.GetLength()-pos);

		strDxfFile += _T("@@@.dxf");

		SilenceToDxf(strDxfFile);

		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_BUILDDEM_SAVE));
		//		DxfToDem(strDxfFile,dlg.m_strDEMPath,dlg.m_fDX,dlg.m_fDX);
		{
			DxfToDem(strDxfFile,"C:\\@@@.dem",dlg.m_fDX,dlg.m_fDX);
			ChangeDem("C:\\@@@.dem",dlg.m_strDEMPath,dlg.m_nFormat);
			::DeleteFile("C:\\@@@.dem");
		}
		::DeleteFile(strDxfFile);
		
		CDSM dem;
		//clear old info
		dem.Open("####$$$$");

		dem.Open(dlg.m_strDEMPath);
		LoadDemPoints(&dem);
		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END));

//		m_strDemPath = dlg.m_strDEMPath;

		UpdateAllViews(NULL,hc_Refresh);

		return;
	}
}

void CDlgDoc::OnOverlayDEM() 
{
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		StrFromResID(IDS_LOADDEM_FILTER));
	
	if( dlg.DoModal()!=IDOK )return;
	
	CDSM dem;
	//clear old info
	dem.Open("####$$$$");
	
	//load the new file
	if( !dem.Open(dlg.GetPathName()) )
		return;
	
	LoadDemPoints(&dem);
	RefreshView();
}

void CDlgDoc::OnOverlayBound() 
{
	CDlgOverlayBound dlg;
	PT_3D region[4];
	GetDlgDataSource()->GetBound(region,NULL,NULL);
	
	dlg.m_lfX1 = region[1].x;
	dlg.m_lfY1 = region[1].y;
	dlg.m_lfX2 = region[2].x;
	dlg.m_lfY2 = region[2].y;
	dlg.m_lfX3 = region[3].x;
	dlg.m_lfY3 = region[3].y;
	dlg.m_lfX4 = region[0].x;
	dlg.m_lfY4 = region[0].y;
	dlg.m_bImgView = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYBOUND,"ViewImg",FALSE);
	
	dlg.m_lfZ1 = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z1",dlg.m_lfZ1);
	dlg.m_lfZ2 = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z2",dlg.m_lfZ2);
	dlg.m_lfZ3 = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z3",dlg.m_lfZ3);
	dlg.m_lfZ4 = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z4",dlg.m_lfZ4);
	
	dlg.m_color = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYBOUND,"Color",RGB(255,0,0));
	if( dlg.DoModal()!=IDOK )
		return;
	
	WriteProfileDouble(REGPATH_OVERLAYBOUND,"Z1",dlg.m_lfZ1);
	WriteProfileDouble(REGPATH_OVERLAYBOUND,"Z2",dlg.m_lfZ2);
	WriteProfileDouble(REGPATH_OVERLAYBOUND,"Z3",dlg.m_lfZ3);
	WriteProfileDouble(REGPATH_OVERLAYBOUND,"Z4",dlg.m_lfZ4);
	AfxGetApp()->WriteProfileInt(REGPATH_OVERLAYBOUND,"ViewImg",dlg.m_bImgView);
	AfxGetApp()->WriteProfileInt(REGPATH_OVERLAYBOUND,"Color",dlg.m_color);
	
	UpdateAllViews(NULL,hc_UpdateOverlayBound);
}

void CDlgDoc::OnToggleAccuBox() 
{
	if( !m_accuBox.IsOpen() )
	{
		m_accuBox.Open();
		//SetCurDrawingObj(m_curDrawingInfo);
		CCommand *pCurCommand = GetCurrentCommand();
		if( pCurCommand )
		{
			if( pCurCommand->GetState()==PROCSTATE_PROCESSING )
			{
				DrawingInfo curDrawingInfo = pCurCommand->GetCurDrawingInfo();
				int num = curDrawingInfo.pts.GetSize();
				if( num == 1 && curDrawingInfo.pFtr != NULL )
				{
					PT_3DEX pt = curDrawingInfo.pts.GetAt(num-1);
					m_accuBox.Click(pt);
				}
				else if( num > 1 && curDrawingInfo.pFtr != NULL)
				{
					PT_3DEX pt1 = curDrawingInfo.pts.GetAt(num-2);
					PT_3DEX pt2 = curDrawingInfo.pts.GetAt(num-1);
					m_accuBox.Click(pt1);
					m_accuBox.Click(pt2);
				}
			}
		}


	}
	else 
	{
		m_accuBox.Close();
	}
	
	HWND hFocus = ::GetFocus();
	AfxGetMainWnd()->SendMessage(FCCM_SHOW_ACCUDLG,m_accuBox.IsOpen());
	::SetFocus(hFocus);
	
	UpdateAllViews(NULL,hc_Update_Accubox);
}

void CDlgDoc::OnUpdateToggleAccuBox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_accuBox.IsOpen());
}

void CDlgDoc::OnToggleRightAng()
{
	CCommand *pCmd = GetCurrentCommand();
	if (pCmd && pCmd->IsKindOf(RUNTIME_CLASS(CDlgDrawCurveCommand_HuNan)))
	{
		CValueTable tab0;
		tab0.BeginAddValueItem();
		tab0.AddValue("RightAngle",&CVariantEx(_variant_t(BYTE('x'))));
		tab0.EndAddValueItem();

		pCmd->SetParams(tab0);
		return;
	}
	
	m_accuBox.ClearDir();
	if( m_accuBox.m_nRightAnglePlace!=CAccuBox::rightangleIM )
	{
		m_accuBox.SetRightAngleMode(CAccuBox::rightangleIM);
		CCommand *pCurCmd = GetCurrentCommand();
		if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) )
		{
			DrawingInfo info = ((CDrawCommand*)pCurCmd)->GetCurDrawingInfo();
			if( !info.pFtr )return;
			
			CArray<PT_3DEX,PT_3DEX> pts;
			pts.Copy(info.pts);
			int size = pts.GetSize();
			//按照直角方式采集
			if (size == 1)
			{
				m_accuBox.Click(pts[0]);
			}
			else if( size>=2 )
			{
				m_accuBox.Click(pts[size-2]);
				m_accuBox.Click(pts[size-1]);
			}
		}
	}
	else
	{
		m_accuBox.SetRightAngleMode(CAccuBox::rightangleNone);
	}				

}

void CDlgDoc::OnUpdateToggleRightAng(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_accuBox.m_nRightAnglePlace==CAccuBox::rightangleIM);
}

#include "UIParam2.h"
void CDlgDoc::OnDrawAutoClosed()
{				
	CCommand *pCmd = GetCurrentCommand();
	if (pCmd && pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		pCmd->GetParams(tab);
		tab.EndAddValueItem();

		const CVariantEx *varex;
		if (tab.GetValue(0,PF_CLOSED,varex))
		{
			bool bClosed = (bool)(_variant_t)*varex;
			_variant_t var = (_variant_t)!bClosed;

			CValueTable tab0;
			tab0.BeginAddValueItem();
			tab0.AddValue(PF_CLOSED,&CVariantEx(var));
			tab0.EndAddValueItem();

			pCmd->SetParams(tab0);
		}
	}
}

void CDlgDoc::OnUpdateDrawAutoClosed(CCmdUI* pCmdUI) 
{
	CCommand *pCmd = GetCurrentCommand();
	if (pCmd && pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		pCmd->GetParams(tab);
		tab.EndAddValueItem();

		const CVariantEx *varex;
		if (tab.GetValue(0,PF_CLOSED,varex))
		{
			bool bClosed = (bool)(_variant_t)*varex;

			pCmdUI->Enable(TRUE);
			pCmdUI->SetCheck(bClosed);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	
}

void CDlgDoc::OnDrawAutoRectify()
{				
	CCommand *pCmd = GetCurrentCommand();
	if (pCmd && pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		pCmd->GetParams(tab);
		tab.EndAddValueItem();

		const CVariantEx *varex;
		if (tab.GetValue(0,PF_RECTIFY,varex))
		{
			bool bRectify = (bool)(_variant_t)*varex;
			_variant_t var = (_variant_t)!bRectify;
			
			CValueTable tab0;
			tab0.BeginAddValueItem();
			tab0.AddValue(PF_RECTIFY,&CVariantEx(var));
			tab0.EndAddValueItem();
			
			pCmd->SetParams(tab0);
		}
	}
}

void CDlgDoc::OnUpdateOnDrawAutoRectify(CCmdUI* pCmdUI) 
{
	BOOL bCheckd = FALSE;
	CCommand *pCmd = GetCurrentCommand();
	if (pCmd && pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		pCmd->GetParams(tab);
		tab.EndAddValueItem();

		const CVariantEx *varex;
		if (tab.GetValue(0,PF_RECTIFY,varex))
		{
			bool bRectify = (bool)(_variant_t)*varex;
			pCmdUI->Enable(TRUE);
			pCmdUI->SetCheck(bRectify);		
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CDlgDoc::LButtonDblClk(PT_3D pt, int flag)
{
	CWorker::LButtonDblClk(pt,flag);

	if( !(m_bSelectorOpen&&m_bSelectDrag) && CanSetAnchor())
	{
		SetAnchorPoint(pt);
	}
}

void CDlgDoc::LButtonDown(PT_3D &pt, int flag)
{
	BOOL bOld = m_pDataQuery->SetSymFixed(TRUE);

	CWorker::LButtonDown(pt,flag);

	m_pDataQuery->SetSymFixed(bOld);
	
	if (!(m_bSelectorOpen&&m_bSelectDrag) && CanSetAnchor())
	{
		SetAnchorPoint(pt);
	}

	if(m_selection.m_arrCanSelObjs.GetSize()>1)
	{
		if(m_pDlgSelectShift==NULL)
		{
			m_pDlgSelectShift = new CDlgSelectShift();
			m_pDlgSelectShift->Create(CDlgSelectShift::IDD,NULL);	
		}

		if(m_pDlgSelectShift!=NULL)
		{
			if(!m_pDlgSelectShift->IsWindowVisible())
			{
				CRect rcClient;
				GetWindowRect(GetActiveView()->GetSafeHwnd(),rcClient);
				
				CRect rcDlg;
				m_pDlgSelectShift->GetWindowRect(&rcDlg);
				
				rcDlg.OffsetRect(rcClient.left-rcDlg.left,rcClient.top-rcDlg.top);
				m_pDlgSelectShift->MoveWindow(&rcDlg);	

				m_pDlgSelectShift->m_pDoc = this;
				m_pDlgSelectShift->m_arrFtrs.RemoveAll();
				for(int i=0; i<m_selection.m_arrCanSelObjs.GetSize(); i++)
				{
					m_pDlgSelectShift->m_arrFtrs.Add(HandleToFtr(m_selection.m_arrCanSelObjs[i]));
				}
				m_pDlgSelectShift->UpdateList();
				m_pDlgSelectShift->ShowWindow(SW_SHOW);

				CView *pView = GetActiveView();
				if(pView)pView->SetFocus();

			}
			else
				m_pDlgSelectShift->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if(m_pDlgSelectShift!=NULL)
		{
			m_pDlgSelectShift->ShowWindow(SW_HIDE);
		}		
	}
}

void CDlgDoc::RButtonDown(PT_3D pt, int flag)
{
	if( m_bDisableCmdInput )return;

	if(m_pDlgSelectShift!=NULL)
	{
		m_pDlgSelectShift->ShowWindow(SW_HIDE);
	}
	
	if( m_bSelectorOpen )
	{
		if( m_bSelectDrag )
		{
			CWorker::RButtonDown(pt,flag);
			return;
		}
	}

	BOOL bRCLKSwitch = m_bRCLKSwitch;
	if(m_bCADStyle)
		bRCLKSwitch = FALSE;
	
	CCommand *pCurCommand = GetCurrentCommand();
	if( pCurCommand )
	{
		if( m_bCADStyle && pCurCommand->GetState()==PROCSTATE_READY && m_nCurCmdCreateID!=ID_MAPDECORATE )
		{
			AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_SELECT,0);
		}
		else if( bRCLKSwitch && pCurCommand->GetState()==PROCSTATE_READY && m_nCurCmdCreateID!=ID_MAPDECORATE )
		{
			int id = GetCurrentCommandId();
			if( id>=ID_ELE_START && id<=ID_ELE_END )
				AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_SELECT,0);
			else
				ActiveLayer(NULL,FALSE);
		}
		else if(pCurCommand->GetState()==PROCSTATE_READY && m_nCurCmdCreateID!=ID_MAPDECORATE)
		{
			AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_SELECT,0);
		}
		else
		{
			CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgPtReset,NULL);
			if( pCurProc )
			{
				m_bCmdProcessing = TRUE;
				pCurProc->PtReset(pt);
				m_bCmdProcessing = FALSE;
				
				TryFinishCurProcedure();
			}
		}
	}
	else
		ActiveLayer(NULL,FALSE);

}


//是否显示右键菜单
BOOL CDlgDoc::CanShowRBDMenu()
{
	if( m_bRCLKSwitch )
		return FALSE;

	CCommand *pCurCommand = GetCurrentCommand();
	if(pCurCommand==NULL)
		return m_bCADStyle;

	return FALSE;
}


void CDlgDoc::OnSelectState(PT_3D pt, int flag)
{
	if (m_bObjectGroup)
	{
		int selnum = 0;
		CSelection *pSelect = GetSelection();
		int num;
		const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(num);	
		
		for (int k=0; k<num; k++)
		{
			CFeature *pFtr = (CFeature*)ftrs[k];
			if (!pFtr) continue;
			
			CFtrArray arr;
			selnum += GetDlgDataSource()->GetAssociatesWithObject(pFtr,arr);
			if (arr.GetSize() > 1)
			{
				for (int i=0; i<arr.GetSize(); i++)
				{
					pSelect->SelectObj(FTR_HANDLE(arr[i]));
				}
			}
			
		}	
		
		if (selnum > num)
		{
			OnSelectChanged();
		}
	}
}

void CDlgDoc::Save()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if (pDS && UVSModify == GetAccessType(pDS))//uvs
	{
		CString strbaklatest = m_strPathName;

		int pos1 = strbaklatest.ReverseFind(_T('\\'));
		if (pos1 == -1) return;
		strbaklatest.Insert(pos1, _T("\\fdb_baks"));

		// 备份文件夹
		pos1 = strbaklatest.ReverseFind(_T('\\'));
		CString strBaks = strbaklatest.Left(pos1);
		if (!PathFileExists(strBaks))
		{
			CreateDirectory(strBaks, NULL);
		}
		if (pDS->IsModified() || !PathFileExists(strbaklatest))
			SaveAs(strbaklatest);
		return;
	}

	if( m_nBeginBatchUpdateOp )
	{
		EndBatchUpdate();
		BeginBatchUpdate();
	}
}

#include "propertyfilebaseinfopage.h"
#include "propertysummarypage.h"
#include "propertystatinfopage.h"
#include "propertyuserdefinepage.h"
void CDlgDoc::OnFileProperty()
{
	CPropertySheet sheet(StrFromResID(IDS_ATTRIBUTE));
	CPropertyFileBaseInfoPage pageBase(this);
	sheet.AddPage(&pageBase);
	CPropertySummaryPage pageSummary(this);
	sheet.AddPage(&pageSummary);
	CPropertyStatInfoPage pageStatInfo(this);
	sheet.AddPage(&pageStatInfo);
	CPropertyUserDefinePage pageUserDefined(this);
	sheet.AddPage(&pageUserDefined);

	BeginBatchUpdate();

	sheet.DoModal();

	EndBatchUpdate();
// 	SHELLEXECUTEINFO ShellInfo;
// 	
// 	memset(&ShellInfo, 0, sizeof(ShellInfo));
// 	ShellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
// 	ShellInfo.hwnd = NULL;
// 	ShellInfo.lpVerb = _T("properties");
// 	ShellInfo.lpFile = m_strPathName; 
// 	ShellInfo.nShow = SW_SHOWNORMAL;
// 	ShellInfo.fMask = SEE_MASK_INVOKEIDLIST|SEE_MASK_FLAG_NO_UI;
// 	
//      BOOL bResult = ShellExecuteEx(&ShellInfo);
}

void CDlgDoc::OnFileSave() 
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if(!pDS) return;
	if(UVSModify == GetAccessType(pDS) && !CUVSModify::CanExport())//uvs
		return;

	Save();
	
	if(!pDS->IsModified())
		return;

	if( !pDS->GetAccessObject() )
		return;

	SaveBakFiles(BakTypeLatest);
	
	/////uvs/////////////////
	CAccessModify *pAcess = pDS->GetAccessObject();
	if(pAcess)
		pAcess->ClearAttrTables();
	
	// 保存编辑者，编辑时间
	CString type;
	char username[UNLEN];
	ULONG nSize = UNLEN;
	GetUserName(username,&nSize);
	pDS->SaveDataSettings(FIELDNAME_STATEDITOR,FIELDNAME_STATEDITOR,username,type);
	CString time;
	time.Format("%d",pDS->GetTotalModifiedTime());
	pDS->SaveDataSettings(FIELDNAME_STATEDITTIME,FIELDNAME_STATEDITTIME,time,type);

	::CopyFile((LPCTSTR)m_strTmpFile,(LPCTSTR)m_strPathName,FALSE);
	//CopySqliteFile((LPCTSTR)m_strTmpFile,(LPCTSTR)m_strPathName);
	pDS->SetModifiedFlag(FALSE);
	pDS->SetSavedFlag(TRUE);

	GOutPut(StrFromResID(IDS_OK_SAVEFILE));
}


BOOL CDlgDoc::ActiveDataSource(CString strDataName)
{
	int i;
	for (i=0;i<m_arrData.GetSize();i++)
	{
		if (strDataName.CompareNoCase(m_arrData[i]->GetName())==0)
		{
			m_nActiveData = i;
			break;
		}
	}
	if (i>=m_arrData.GetSize())
	{
		return FALSE;
	}
	SetActualPathName(strDataName);
	m_strTmpFile = m_arrData[i]->GetTempFilePath();
//	m_strTmpFile+=_T(".tmp");
	m_pDataQuery->m_pFilterDataSource = m_arrData[i];
	UpdateBound();
	return TRUE;
}
BOOL CDlgDoc::ActiveDataSource(CDataSourceEx *pDS)
{
	return ActiveDataSource(pDS->GetName());
}
BOOL CDlgDoc::SaveModified(CString strDataName)
{
	if (UVSModify == GetAccessType(GetDlgDataSource()))//uvs
	{
		return TRUE;
	}

	if (strDataName.IsEmpty())
	{
		return SaveModified(); 
	}
	int i;
	for ( i=0;i<m_arrData.GetSize();i++)
	{
		if (strDataName.CompareNoCase(m_arrData[i]->GetName())==0)
		{
			break;
		}
	}
	if (i>=m_arrData.GetSize())
	{
		return FALSE;
	}
	return SaveModified(m_arrData[i]);
}
BOOL CDlgDoc::SaveModified(CDataSourceEx *pDS)
{
	if(pDS==NULL)
	{
		return SaveModified();
	}

	if(UVSModify == GetAccessType(pDS))//uvs
	{
		return TRUE;
	}

	
	BOOL bModified = FALSE;
	if(pDS->IsModified())
		bModified = TRUE;	
	int nPromptRet;	
	if( bModified)
	{
		nPromptRet = AfxMessageBox(IDS_SAVETIP, MB_YESNOCANCEL);
		if( nPromptRet==IDCANCEL )return FALSE;
		
// 		if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 		{
// 			if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 			{
// 				OnSaveDEM();
// 			}
// 		}
		if (nPromptRet == IDYES)
		{
			SaveBakFiles(BakTypeLatest);
			::CopyFile(m_strTmpFile,GetPathName(),FALSE);
			//CopySqliteFile(m_strTmpFile,GetPathName());
		}
		else
		{
			SaveBakFiles(BakTypeLatest);
		}
	}	
	return TRUE;
}

BOOL CDlgDoc::SaveModified() 
{
	if (UVSModify == GetAccessType(GetDlgDataSource()))//uvs
	{
		return TRUE;
	}

	BOOL bModified = FALSE;
	CDlgDataSource *pDS = GetDlgDataSource();
	if( pDS )
	{
		if(pDS->IsModified())
			bModified = TRUE;
	}
	
	//判断当前是否正在关闭文档
	HWND hWnd = ::GetActiveWindow();
	HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
	BOOL bClosing = FALSE;
	if( hWnd==hWnd0 || IsChild(hWnd0,hWnd) )
		bClosing = TRUE;
	
	if( !m_bPromptSave )
	{
		m_nPromptRet = IDNO;
		if( (IsModified()||bModified) && bClosing )
		{
			m_nPromptRet = AfxMessageBox(IDS_SAVETIP, MB_YESNOCANCEL);
			if( m_nPromptRet==IDCANCEL )return FALSE;

// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
		else if( bClosing )
		{
			m_nPromptRet = AfxMessageBox(IDS_SAVETIP2, MB_YESNO);
			if( m_nPromptRet==IDNO )return FALSE;

// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
	}
	if( bModified )
	{
		Save();
	}

	if (((pDS && pDS->IsSaved()) || bModified) && bClosing)
	{
		// 保存编辑者，修改次数, 编辑时间
		CString type;
		char username[UNLEN];
		ULONG nSize = UNLEN;
		GetUserName(username,&nSize);
		pDS->SaveDataSettings(FIELDNAME_STATEDITOR,FIELDNAME_STATEDITOR,username,type);
		
		CString name, modifynum;
		pDS->GetDataSettings(FIELDNAME_STATEDITNUM,name,modifynum,type);
		int nModifyNum = 0;
		if (!modifynum.IsEmpty())
		{
			nModifyNum = atoi(modifynum);
		}
		nModifyNum++;
		modifynum.Format("%i",nModifyNum);
		pDS->SaveDataSettings(FIELDNAME_STATEDITNUM,FIELDNAME_STATEDITNUM,modifynum,type);
		
		CString time;
		time.Format("%d",pDS->GetTotalModifiedTime());
		pDS->SaveDataSettings(FIELDNAME_STATEDITTIME,FIELDNAME_STATEDITTIME,time,type);
	}

	if( bModified )
	{
//		if( !bClosing || (bClosing&&m_nPromptRet==IDYES) )
		{
			if (m_nPromptRet == IDYES)
			{
				SaveBakFiles(BakTypeLatest);
				::CopyFile(m_strTmpFile, GetPathName(), FALSE);
				//CopySqliteFile(m_strTmpFile,GetPathName());
			}
			else
			{
				SaveBakFiles(BakTypeLatest);
			}
		}
	}
	
	if( bClosing )
	{	
		Destroy();
		::DeleteFile(m_strTmpFile);//shy
	}
	
	m_bPromptSave = FALSE;
	
	return TRUE;
}

void CDlgDoc::SaveViewPreviewImage()
{
	return;
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
		{
			HBITMAP hBitmap = ((CVectorView_new*)pView)->GetViewBitmap(32,32);
			CDlgDataSource *pDS = GetDlgDataSource();
			if (pDS && (pDS->IsModified() || pDS->IsSaved()))
			{
				pDS->AddPreviewImage(_T("32*32"),hBitmap);
				::DeleteObject(hBitmap);
			}
			break;
		}
	}   

}


BOOL CDlgDoc::SaveAs(LPCTSTR fileName, BOOL bfastcopy)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS)return FALSE;

	if(UVSModify == GetAccessType(pDS) && !CUVSModify::CanExport())
		return FALSE;

	if (bfastcopy && SQLiteAccess == GetAccessType(pDS))
	{
		EndBatchUpdate();
		BeginBatchUpdate();
		return ::CopyFile((LPCTSTR)m_strTmpFile,fileName,FALSE);
	}

	if(::PathFileExists(fileName))
	{
		if(::DeleteFile(fileName)==FALSE)
			return FALSE;
	}

	CDataQueryEx *pDQ = new CDataQueryEx();
	CDlgDataSource *pDataSource = new CDlgDataSource(pDQ);	
	if( !pDataSource )
	{	
		return FALSE;
	}
	CSQLiteAccess *pSqlAccess = new CSQLiteAccess;
	if(!pSqlAccess) 
	{
		delete pDataSource;
		return FALSE;
	}
	pSqlAccess->Attach(fileName);

	pDataSource->SetAccessObject(pSqlAccess);
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	CScheme *pScheme = pCfgLibManager->GetScheme(pDS->GetScale());
	DWORD scale = pCfgLibManager->GetScaleByScheme(pScheme);
	if (scale==0)
	{
		delete pDataSource;
		return FALSE;
	}
	pSqlAccess->BatchUpdateBegin();
	pSqlAccess->CreateFileSys(scale, pScheme);
	pSqlAccess->BatchUpdateEnd();
	
	pDataSource->LoadAll(NULL,FALSE);		
	pSqlAccess->BatchUpdateBegin();
	pDataSource->ResetDisplayOrder();
	pDataSource->SaveAllLayers();
	pSqlAccess->BatchUpdateEnd();

	//写入工作区信息
	pDataSource->SetScale(pDS->GetScale());
	PT_3D pts[4];
	double zmin,zmax;
	pDS->GetBound(pts,&zmin,&zmax);
	pDataSource->SetBound(pts,zmin,zmax);

	GOutPut(StrFromResID(IDS_TIPS_CREATE_BAKEFILE));
	
	CFtrLayer *pLayer = NULL,*pLayer0 = NULL;
	CFeature *pFtr = NULL,*pFtr0 = NULL;
	CValueTable tab;
	pSqlAccess->BatchUpdateBegin();
	if(1)
	{
		int nSum = 0;		
		int nLay = pDS->GetFtrLayerCount();
		for (int i=0;i<nLay;i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer)continue;			
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
				if (pFtr)
				{					
					nSum++;
				}
			}
		}
		GProgressStart(nSum);
		for (i=0;i<nLay;i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if( !pLayer )continue;

			pLayer0 = pDataSource->GetFtrLayer(pLayer->GetName());
			if (!pLayer0)
			{
				pLayer0 = new CFtrLayer;
				if(!pLayer0)continue;
				tab.DelAll();
				tab.BeginAddValueItem();
				pLayer->WriteTo(tab);
				tab.EndAddValueItem();
				pLayer0->ReadFrom(tab);
				pLayer0->SetID(0);
				pDataSource->AddFtrLayer(pLayer0);
			}
			int nObj = pLayer->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
				if (pFtr)
				{
					GProgressStep();
					pFtr0 = pFtr->Clone();
					pFtr0->SetID(OUID());
					if(!pDataSource->AddObject(pFtr0,pLayer0->GetID()))
					{
						delete pFtr0;
						continue;
					}
					tab.DelAll();
					tab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab);
					tab.EndAddValueItem();
					pDataSource->GetXAttributesSource()->SetXAttributes(pFtr0,tab);
				}
			}
		}
		GProgressEnd();
	}
	pSqlAccess->BatchUpdateEnd();
	delete pDataSource;

	GOutPut(StrFromResID(IDS_OK_CREATE_BAKEFILE));

	return TRUE;
}


void CDlgDoc::SaveBakFiles(int nBakType)
{
	static bool bSaving = false;
	if (bSaving) return;
	bSaving = true;

	CString strFileName = m_strPathName;

	if (nBakType==BakType12)
	{
		CString strbak1,strbak2;
		strbak1 = strFileName;
		
		int pos1 = strbak1.ReverseFind(_T('\\'));
		if (pos1 == -1) 
		{
			bSaving = false;
			return;
		}
		strbak1.Insert(pos1,_T("\\fdb_baks"));
		
		// 备份文件夹
		pos1 = strbak1.ReverseFind(_T('\\'));
		CString strBaks = strbak1.Left(pos1);
		if (!PathFileExists(strBaks))
		{
			CreateDirectory(strBaks,NULL);
		}
		
		int pos = strbak1.ReverseFind(_T('.'));
		if(pos==-1)
		{
			bSaving = false;
			return;
		}
		strbak1.Insert(pos,_T(".bak1"));
		
		if( _access(LPCTSTR(strbak1),0)==-1 )//不存在
		{
			if( !SaveAs(strbak1, FALSE) )
			{
				AfxMessageBox(IDS_ERROR_CREATE_BAKEFILE);
			}
		}
		else
		{
			//临时文件比备份文件新，才覆盖
			FILETIME time1,time2;
			GetFileWriteTime(m_strTmpFile,&time1);
			GetFileWriteTime(strbak1,&time2);
			if( CompareFileTime(&time1,&time2)>0 )
			{
				strbak2 = strFileName;
				
				int pos1 = strbak2.ReverseFind(_T('\\'));
				if (pos1 == -1)
				{
					bSaving = false;
					return;
				}
				strbak2.Insert(pos1,_T("\\fdb_baks"));
				
				int pos = strbak2.ReverseFind(_T('.'));
				if(pos==-1)
				{
					bSaving = false;
					return;
				}

				CString strbak3 = strbak2;
				strbak3.Insert(pos, _T(".bak3"));
				strbak2.Insert(pos,_T(".bak2"));
				
				::MoveFile((LPCTSTR)strbak2, (LPCTSTR)strbak3);
				::MoveFile((LPCTSTR)strbak1,(LPCTSTR)strbak2);
				//::CopyFile((LPCTSTR)m_strTmpFile,(LPCTSTR)strbak1,FALSE);
				if( !SaveAs(strbak1, FALSE) )
				{
					::MoveFile((LPCTSTR)strbak3, (LPCTSTR)strbak2);
					AfxMessageBox(IDS_ERROR_CREATE_BAKEFILE);
				}
				else
				{
					::DeleteFile((LPCTSTR)strbak3);
				}
			}
		}
	}
	else if (nBakType==BakTypeLatest)
	{
		CString strbaklatest = strFileName;
		
		int pos1 = strbaklatest.ReverseFind(_T('\\'));
		if (pos1 == -1)
		{
			bSaving = false;
			return;
		}
		strbaklatest.Insert(pos1,_T("\\fdb_baks"));
		
		// 备份文件夹
		pos1 = strbaklatest.ReverseFind(_T('\\'));
		CString strBaks = strbaklatest.Left(pos1);
		if (!PathFileExists(strBaks))
		{
			CreateDirectory(strBaks,NULL);
		}
		
		int pos = strbaklatest.ReverseFind(_T('.'));
		if(pos==-1)
		{
			bSaving = false;
			return;
		}
		strbaklatest.Insert(pos,_T(".baklatest"));

		//临时文件比备份文件新，才覆盖
		FILETIME time1,time2;
		GetFileWriteTime(m_strTmpFile,&time1);
		GetFileWriteTime(strbaklatest,&time2);
		if( CompareFileTime(&time1,&time2)>0 )
		{
			//::CopyFile((LPCTSTR)m_strTmpFile,(LPCTSTR)strbaklatest,FALSE);
			//CopySqliteFile((LPCTSTR)m_strTmpFile,(LPCTSTR)strbaklatest);
			if( !SaveAs(strbaklatest,TRUE) )
			{
				AfxMessageBox(IDS_ERROR_CREATE_BAKEFILE);
			}
		}
	}
	else if (nBakType == BakTypeUVS)
	{
		CString strbak1, strbak2;
		strbak1 = strFileName;

		int pos1 = strbak1.ReverseFind(_T('\\'));
		if (pos1 == -1)
		{
			bSaving = false;
			return;
		}
		strbak1.Insert(pos1, _T("\\fdb_baks"));

		// 备份文件夹
		pos1 = strbak1.ReverseFind(_T('\\'));
		CString strBaks = strbak1.Left(pos1);
		if (!PathFileExists(strBaks))
		{
			CreateDirectory(strBaks, NULL);
		}

		int pos = strbak1.ReverseFind(_T('.'));
		if (pos == -1)
		{
			bSaving = false;
			return;
		}
		strbak1.Insert(pos, _T(".bak1"));

		if (_access(LPCTSTR(strbak1), 0) == -1)//不存在
		{
			if (!SaveAs(strbak1, FALSE))
			{
				AfxMessageBox(IDS_ERROR_CREATE_BAKEFILE);
			}
		}
		else
		{
			strbak2 = strFileName;

			int pos1 = strbak2.ReverseFind(_T('\\'));
			if (pos1 == -1)
			{
				bSaving = false;
				return;
			}
			strbak2.Insert(pos1, _T("\\fdb_baks"));

			int pos = strbak2.ReverseFind(_T('.'));
			if (pos == -1)
			{
				bSaving = false;
				return;
			}

			CString strbak3 = strbak2;
			strbak3.Insert(pos, _T(".bak3"));
			strbak2.Insert(pos, _T(".bak2"));

			::MoveFile((LPCTSTR)strbak2, (LPCTSTR)strbak3);
			::MoveFile((LPCTSTR)strbak1, (LPCTSTR)strbak2);
			//::CopyFile((LPCTSTR)m_strTmpFile,(LPCTSTR)strbak1,FALSE);
			if (!SaveAs(strbak1, FALSE))
			{
				::MoveFile((LPCTSTR)strbak3, (LPCTSTR)strbak2);
				AfxMessageBox(IDS_ERROR_CREATE_BAKEFILE);
			}
			else
			{
				::DeleteFile((LPCTSTR)strbak3);
			}
		}
	}
	bSaving = false;
}

void CDlgDoc::OnFileSaveAs() 
{
	CDocTemplate* pTemplate = GetDocTemplate();
	ASSERT(pTemplate != NULL);
	
	ASSERT(m_strPathName);
	
	CString newName = m_strPathName;
	
	if( !AfxGetApp()->DoPromptFileName(newName,AFX_IDS_SAVEFILECOPY,
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate) )
		return;
	
	//OnFileSave();
	Save();
	//CopyFile((LPCSTR)m_strTmpFile,(LPCSTR)newName,FALSE);
	if( !SaveAs(newName) )
	{
		AfxMessageBox(IDS_ERROR_CREATE_BAKEFILE);
	}
	//CopySqliteFile((LPCSTR)m_strTmpFile,(LPCSTR)newName);
//	ClearGarbage(newName);	


	


}

void CDlgDoc::OnExecLastCommand() 
{
	m_RecentCmd.ActiveLastRecentCmd();
}

void CDlgDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	
}

void CDlgDoc::SetActualPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	CString title = lpszPathName;
	if (0!=title.Right(4).CompareNoCase(".fdb"))//uvs
	{
		CString xmlPath = ((CMainFrame*)AfxGetMainWnd())->m_wndProjectView.GetCurPrjPath();
		if (!xmlPath.IsEmpty())
		{
			int pos = xmlPath.ReverseFind('\\');
			if (pos > 0)
			{
				//添加此不存在的路径，方便导出一些文件时生成默认路径
				m_strPathName = xmlPath.Left(pos) + "\\DLG\\" + lpszPathName + ".uvs.fdb";
			}
		}
		else
		{
			m_strPathName = lpszPathName;
		}
		ASSERT(!m_strPathName.IsEmpty());
		m_bEmbedded = FALSE;
		ASSERT_VALID(this);
		SetTitle(lpszPathName);
	}
	else
	{
		CDocument::SetPathName(lpszPathName, bAddToMRU);
	}
// 	// store the path fully qualified
// 	TCHAR szFullPath[_MAX_PATH];
// 	AfxFullPath(szFullPath, lpszPathName);
// 	m_strPathName = szFullPath;
// 	ASSERT(!m_strPathName.IsEmpty());       // must be set to something
// 	m_bEmbedded = FALSE;
// 	ASSERT_VALID(this);
// 	
// 	// set the document title based on path name
// 	TCHAR szTitle[_MAX_FNAME];
// 	if (AfxGetFileTitle(szFullPath, szTitle, _MAX_FNAME) == 0)
// 		SetTitle(szTitle);
// 	
// 	// add it to the file MRU list
// 	if (bAddToMRU)
// 		AfxGetApp()->AddToRecentFileList(m_strPathName);
// 	
// 	ASSERT_VALID(this);
}

void CDlgDoc::DoUndo(CUndoButton* pUndoBtn)
{
	if (pUndoBtn == NULL)
	{
		ASSERT (FALSE);
		return;
	}
	
	switch (pUndoBtn->m_nID) 
	{
	case ID_EDIT_UNDO:
		{
			int nstep = pUndoBtn->GetSelNum();

			CancelCurrentCommand();
			
			CDlgDataSource *pDS = GetDlgDataSource();
			if( !pDS )return;
			CAccessModify *pAccess = pDS->GetAccessObject();
			if( pAccess )
			{
				BeginBatchUpdate();
			}
			
			Undo(nstep);
			
			if( pAccess )
			{
				EndBatchUpdate();
			}
			
			OnSelectChanged();
			StartCommand(m_nCurCmdCreateID);
			UpdateAllViews(NULL,hc_Refresh);
		}
		break;
		
	case ID_EDIT_REDO:
		{
			int nstep = pUndoBtn->GetSelNum();

			CancelCurrentCommand();
			
			CDlgDataSource *pDS = GetDlgDataSource();
			if( !pDS )return;
			CAccessModify *pAccess = pDS->GetAccessObject();
			if( pAccess )
			{
				BeginBatchUpdate();
			}
			
			Redo(nstep);
			
			if( pAccess )
			{
				EndBatchUpdate();
			}
			
			OnSelectChanged();
			StartCommand(m_nCurCmdCreateID);
			UpdateAllViews(NULL,hc_Refresh);
		}
		break;
		
	default:
		ASSERT(FALSE);
	}
}

void CDlgDoc::GetUndoActions(CStringList& lstActions) const
{
	GetUndoRedoActions(lstActions, TRUE);
}

void CDlgDoc::GetRedoActions(CStringList& lstActions) const
{
	GetUndoRedoActions(lstActions, FALSE);
}

void CDlgDoc::OnAccuLockX()
{
	m_accuBox.LockX(!m_accuBox.IsLockX());
}

void CDlgDoc::OnAccuLockY()
{
	m_accuBox.LockY(!m_accuBox.IsLockY());
}

void CDlgDoc::OnAccuLockZ()
{
//	m_accuBox.LockZ(!m_accuBox.IsLockZ());
}

void CDlgDoc::OnAccuLockAngle()
{
	m_accuBox.LockAngle(!m_accuBox.IsLockAngle());
}

void CDlgDoc::OnAccuLockDistance()
{
	m_accuBox.LockDistance(!m_accuBox.IsLockDistance());
}

void CDlgDoc::OnAccuLockIndex()
{
	m_accuBox.LockIndex(!m_accuBox.IsLockIndex());
}

void CDlgDoc::OnAccuCoordSysBase()
{
	m_accuBox.SetCoordSysBase(m_accuBox.GetCoordSysBase()==CAccuBox::sysRelative?CAccuBox::sysBase:CAccuBox::sysRelative);
	UpdateAllViews(NULL,hc_Update_Accubox);
}

void CDlgDoc::OnAccuLockExtension()
{
	m_accuBox.LockDir(ACCU_DIR::extension,TRUE);
}

void CDlgDoc::OnAccuLockParallel()
{
	m_accuBox.LockDir(ACCU_DIR::parallel,TRUE);
}

void CDlgDoc::OnAccuLockPerp()
{
	m_accuBox.LockDir(ACCU_DIR::perpendicular,TRUE);
}

void CDlgDoc::OnAccuUnlockCurDir()
{
	m_accuBox.LockDir(ACCU_DIR::invalid,TRUE);
}

void CDlgDoc::OnAccuUnlockAllDir()
{
	m_accuBox.ClearDir();
}



void CDlgDoc::OnDeviceOnlyStereo()
{
	BOOL bDriveOnlyStereo = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYSTEREO,TRUE);
	bDriveOnlyStereo = !bDriveOnlyStereo;
	
	AfxGetApp()->WriteProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYSTEREO,bDriveOnlyStereo);
	
	UpdateAllViews(NULL,hc_UpdateOption);
}


void CDlgDoc::OnUpdateDeviceOnlyStereo(CCmdUI* pCmdUI)
{
	BOOL bDriveOnlyStereo = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYSTEREO,TRUE);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bDriveOnlyStereo);
}

void CDlgDoc::OnDeviceOnlyCursor()
{
	BOOL bDriveOnlyCursor = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
	bDriveOnlyCursor = !bDriveOnlyCursor;
	
	AfxGetApp()->WriteProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,bDriveOnlyCursor);
	
	UpdateAllViews(NULL,hc_UpdateOption);
}


void CDlgDoc::OnUpdateDeviceOnlyCursor(CCmdUI* pCmdUI)
{
	BOOL bDriveOnlyCursor = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bDriveOnlyCursor);
}

IMPLEMENT_DYNCREATE(CDlgCellDoc, CDlgDoc)

BEGIN_MESSAGE_MAP(CDlgCellDoc, CDlgDoc)
//{{AFX_MSG_MAP(CDlgCellDoc)
	ON_COMMAND(ID_MODIFY_MOVETOCENTER, OnMovetoCenter)
	ON_COMMAND(ID_OVERLAY_GRID, OnOverlayGrid)
//}}AFX_MSG_MAP

END_MESSAGE_MAP()

CDlgCellDoc::CDlgCellDoc()
{
	m_pCellEditDef = NULL;
	m_bClosing = FALSE;
}

BOOL CDlgCellDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	return TRUE;
}

void CDlgCellDoc::InitDoc(int nscale)
{
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(nscale);
	
	if (config.GetScale() == 0)
	{
		CDlgScheme::CreateNewScale(nscale);
	}

	//毫米作为单位（1000的比例尺，绘制倍率就是1倍，这样视图的米单位等于毫米单位）
	SetScale(1000);
	
	CScheme *pScheme = pCfgLibManager->GetScheme(nscale);
	
	//ini ftrrecnet
	if (pScheme != NULL)
	{
		//USERIDX *pRecnet = pScheme->GetRecentIdx();
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(pScheme));
	}

	//填充采集面板
	AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(0),LPARAM(&config));
	AfxGetMainWnd()->SendMessage(FCCM_REFRESHCHKSCHBAR,0,0);

	AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(this));
	AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(this));
}

BOOL CDlgCellDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	// 设置对话框
	CDlgWorkSpaceBound dlgB;
	dlgB.m_lfX1 = -5;
	dlgB.m_lfY1 = -5;
	dlgB.m_lfX2 = 5;
	dlgB.m_lfY2 = -5;
	dlgB.m_lfX3 = 5;
	dlgB.m_lfY3 = 5;
	dlgB.m_lfX4 = -5;
	dlgB.m_lfY4 = 5;
	
	if (m_pCellEditDef)
	{
		dlgB.m_nScale = m_pCellEditDef->GetConfig()->GetScale();
	}
	else
		dlgB.m_nScale = gpCfgLibMan->GetConfigLibItem(0).GetScale();

	
	CDlgDataSource *pDataSource = new CDlgDataSource(m_pDataQuery);	
	if( !pDataSource )
	{
		return FALSE;
	}	

	SetLibSourceID((DWORD_PTR)pDataSource);
	
	CFtrLayer *pCellLayer = pDataSource->CreateFtrLayer("CellLayer");
	if (!pCellLayer)  return FALSE;
	
	pDataSource->AddFtrLayer(pCellLayer);
	pDataSource->SetCurFtrLayer(pCellLayer->GetID());
	
	m_pDataQuery->m_pFilterDataSource = pDataSource;
	
	PT_3D pts[4];
	memset(pts,0,sizeof(pts));
	pts[1].x = dlgB.m_lfX1;
	pts[1].y = dlgB.m_lfY1;
	pts[2].x = dlgB.m_lfX2;
	pts[2].y = dlgB.m_lfY2;
	pts[3].x = dlgB.m_lfX3;
	pts[3].y = dlgB.m_lfY3;
	pts[0].x = dlgB.m_lfX4;
	pts[0].y = dlgB.m_lfY4;
	Envelope e;
	e.CreateFromPts(pts,4);
	
	m_arrData.Add(pDataSource);
	m_nActiveData = 0;	
	
	//毫米作为单位（1000的比例尺，绘制倍率就是1倍，这样视图的米单位等于毫米单位）
	SetScale(1000);
	
	m_snap.Init(m_pDataQuery);
	
	
	UIRequest(UIREQ_TYPE_SHOWUIPARAM,0);
	SetBound(e);

	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	if (pCfgLibManager->GetConfigLibItemByScale(dlgB.m_nScale).GetScale() == 0)
	{
		CDlgScheme::CreateNewScale(dlgB.m_nScale);
	}
	// 	SetCellDefLib(pCfgLibManager->GetCellDefLib(dlgB.m_nScale));
	// 	SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(dlgB.m_nScale));

	CScheme *pScheme = pCfgLibManager->GetScheme(dlgB.m_nScale);
	
	//ini ftrrecnet
	if (pScheme != NULL)
	{
		//USERIDX *pRecnet = pScheme->GetRecentIdx();
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(pScheme));
	}

	// 显示格网
	CDlgOverlayGrid dlg;
	GetDlgDataSource()->GetBound(dlg.m_ptRegion,NULL,NULL);
	Envelope evlp;
	evlp.CreateFromPts(dlg.m_ptRegion,4);
	
	m_bOverlayGrid = dlg.m_bOverlay = TRUE;
	dlg.m_bSnapGrid = TRUE;
	dlg.m_fWid = 1;
	dlg.m_fHei = 1;
	dlg.m_fXOrigin = evlp.m_xl;
	dlg.m_fYOrigin = evlp.m_yl;
	dlg.m_fXRange = evlp.m_xh-evlp.m_xl;
	dlg.m_fYRange = evlp.m_yh-evlp.m_yl;
	dlg.m_strZ = _T("0.0");
	dlg.m_bVectView= TRUE;
	dlg.m_bImgView = FALSE;
	
	dlg.m_color = RGB(128,128,128);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"Width",dlg.m_fWid);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"Height",dlg.m_fHei);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"MinX",dlg.m_fXOrigin);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"MinY",dlg.m_fYOrigin);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"XRange",dlg.m_fXRange);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"YRange",dlg.m_fYRange);
	AfxGetApp()->WriteProfileString(REGPATH_CELLOVERLAYGRID,"Z",dlg.m_strZ);
	AfxGetApp()->WriteProfileInt(REGPATH_CELLOVERLAYGRID,"ViewVect",dlg.m_bVectView);
	AfxGetApp()->WriteProfileInt(REGPATH_CELLOVERLAYGRID,"ViewImg",dlg.m_bImgView);
	AfxGetApp()->WriteProfileInt(REGPATH_CELLOVERLAYGRID,"Color",dlg.m_color);
	
	OnUpdateOption();
	
//	if( dlg.m_bOverlay && dlg.m_bSnapGrid )
	{
		SNAP_GRID sg;
		sg.ox = dlg.m_fXOrigin;	sg.oy = dlg.m_fYOrigin;
		sg.dx = dlg.m_fWid;	sg.dy = dlg.m_fHei;
		sg.nx = fabs(dlg.m_fWid)<1e-10?0:ceil(dlg.m_fXRange/dlg.m_fWid);	
		sg.ny = fabs(dlg.m_fWid)<1e-10?0:ceil(dlg.m_fYRange/dlg.m_fWid);
		m_snap.SetSnapMode(m_snap.GetSnapMode()|CSnap::modeGrid);
		m_snap.SetGridParams(&sg);
		m_snap.Enable(TRUE);
	}

	AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(this));
	AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(this));
	
	GetSelection()->DeselectAll();
	OnSelectChanged();

	return TRUE;	
	
}

BOOL CDlgCellDoc::ClearAllObject()
{
	CancelCurrentCommand();
	DeleteUndoItems();

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return FALSE;
	CFtrLayer *pFtrLay = pDS->GetCurFtrLayer();
	if (!pFtrLay)  return FALSE;
	m_selection.DeselectAll();
//	SelectAll();
	for (int i=pFtrLay->GetObjectCount()-1; i>=0; i--)
	{
 		CFeature *pFtr = pFtrLay->GetObject(i,TRUE);
// 		pFtrLay->DeleteObject(pFtr);
		ClearObject(FtrToHandle(pFtr));
		
	}

	UpdateAllViews(NULL,hc_DelAllObjects);
	GRIDParam pm;
	UpdateAllViews(NULL,hc_UpdateGrid,(CObject*)&pm);

	return TRUE;
}

void CDlgCellDoc::OnCloseDocument() 
{
	
// 	if(m_pSelEx)
// 	{		
// 		delete m_pSelEx;
// 		m_pSelEx = NULL;
// 		AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
// 	}
// 	CancelCurrentCommand();
	
//	DeleteUndoItems();
	
	
// 	for( int i=m_arrData.GetSize()-1; i>=0; i--)
// 	{
// 		//		m_arrData.GetAt(i)->SaveAll(NULL);
// 		if(m_arrData.GetAt(i) != NULL)
// 			delete m_arrData.GetAt(i);
// 	}
// 	
// 	m_arrData.RemoveAll();	
// 	
// 	if( m_pDataQuery )
// 	{
// 		delete m_pDataQuery;
// 		m_pDataQuery = NULL;
// 	}
	
	
	BOOL bModified = FALSE;
	CDlgDataSource *pDS = GetDlgDataSource();
	if( pDS )
	{
		if(pDS->IsModified())		
			bModified = TRUE;
	}
	
	//判断当前是否正在关闭文档
//	HWND hWnd = ::GetActiveWindow();
//	HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
	BOOL bClosing = FALSE;
//	if( hWnd==hWnd0 || IsChild(hWnd0,hWnd) )
		m_bClosing = bClosing = TRUE;

	// 保存修改图元	
	if (!m_pCellEditDef->IsClosing())
	{
		m_pCellEditDef->SendMessage(WM_CLOSE);
	}

	DeleteUndoItems();
	
	if( !m_bPromptSave )
	{
		m_nPromptRet = IDNO;
		if( bModified && bClosing )
		{
// 			m_nPromptRet = AfxMessageBox(IDS_SAVECELLEDIT, MB_YESNOCANCEL);
// 			if( m_nPromptRet==IDCANCEL )return;
			
// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
		else if( bClosing )
		{
			m_nPromptRet = IDYES;
		}
	}
	
	if( bClosing )
	{
		ClearAllObject();
		Destroy();	
	}
	
	if( bModified )
	{
		if( !bClosing || (bClosing&&m_nPromptRet==IDYES) )
		{			
			
//			SaveBakFiles();
//			::CopyFile(m_strTmpFile,GetPathName(),FALSE);
			
		}
	}
	
	if( bClosing )
	{
//		::DeleteFile(m_strTmpFile);//shy
	}
	
	m_bPromptSave = FALSE;
	CDocument::OnCloseDocument();
}

BOOL CDlgCellDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	UpdateFrameCounts();

	//当有其他视图时，首先关闭矢量视图的情况
	CFrameWnd *pFirstFrm = NULL;

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		CFrameWnd* pFrame2 = pView->GetParentFrame();

		if( pFrame2!=NULL && pFirstFrm==NULL )
		{
			pFirstFrm = pFrame2;
		}

		// assume frameless views are ok to close
		if( pFrame2!=NULL && pFrame2==pFrame && pFrame2==pFirstFrm )
		{
			BOOL bHaveOtherFrame = FALSE;
			pos = GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView = GetNextView(pos);
				CFrameWnd* pFrame2 = pView->GetParentFrame();
				if( pFrame2!=NULL && pFrame2!=pFrame )
				{
					bHaveOtherFrame = TRUE;
				}
			}

			if( bHaveOtherFrame )
			{
				BOOL bModified = FALSE;
				CDlgDataSource* pDS = GetDlgDataSource();
				if( pDS )
				{
					if(pDS->IsModified())
					{
						bModified = TRUE;
					}					
				}
				
				//判断当前是否正在关闭文档
				HWND hWnd = ::GetActiveWindow();
				HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
				BOOL bClosing = FALSE;
				if( hWnd==hWnd0 || IsChild(hWnd0,hWnd) )
					bClosing = TRUE;
				
				m_nPromptRet = IDNO;
				if( (bModified) && bClosing && !m_bPromptSave )
				{
					m_nPromptRet = AfxMessageBox(IDS_SAVECELLEDIT, MB_YESNOCANCEL);
					if( m_nPromptRet==IDCANCEL )return FALSE;

// 					if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 					{
// 						if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 						{
// 							OnSaveDEM();
// 						}
// 					}
				}
				else if( bClosing && !m_bPromptSave )
				{
					m_nPromptRet = AfxMessageBox(IDS_SAVETIP2, MB_YESNO);
					if( m_nPromptRet==IDNO )return FALSE;

// 					if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 					{
// 						if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 						{
// 							OnSaveDEM();
// 						}
// 					}
				}

				m_bPromptSave = TRUE;

				pos = GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = GetNextView(pos);
					CFrameWnd* pFrame2 = pView->GetParentFrame();
					if( pFrame2!=NULL && pFrame2!=pFrame )
					{
						pFrame2->PostMessage(WM_CLOSE);
					}
				}

				pFrame->PostMessage(WM_CLOSE);			
				return FALSE;
			}
		}
	}

	//当有其他视图时，首先关闭非矢量视图的情况
	pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		CFrameWnd* pFrame2 = pView->GetParentFrame();
		// assume frameless views are ok to close
		if (pFrame2 != NULL)
		{
			// assumes 1 document per frame
			if (pFrame2->m_nWindow > 0)
			{
				return TRUE;        // more than one frame refering to us
			}
		}
	}

	//当没有其他视图或者其他视图已经关闭时，再关闭矢量视图的情况
/*	BOOL bModified = FALSE;
	CDlgDataSource* pDS = GetDlgDataSource();				
	if( pDS )
	{
		if(pDS->IsModified())		
			bModified = TRUE;
	}
	
	//判断当前是否正在关闭文档
	HWND hWnd = ::GetActiveWindow();
	HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
	BOOL bClosing = FALSE;
	if( hWnd==hWnd0 || IsChild(hWnd0,hWnd) )
		bClosing = TRUE;
	
	if( !m_bPromptSave )
	{
		m_nPromptRet = IDNO;
		if( bModified && bClosing )
		{
// 			m_nPromptRet = AfxMessageBox(IDS_SAVECELLEDIT, MB_YESNOCANCEL);
// 			if( m_nPromptRet==IDCANCEL )return FALSE;

// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
			
			m_bPromptSave = TRUE;
		}
		else if( bClosing )
		{
// 			m_nPromptRet = AfxMessageBox(IDS_SAVETIP2, MB_YESNO);
// 			if( m_nPromptRet==IDNO )return FALSE;
			m_bPromptSave = TRUE;

// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
	}
*/
	return TRUE;//SaveModified();
}

void CDlgCellDoc::SetDlgCellEditDef(CDlgCellDefLinetypeView *pCell)
{
	m_pCellEditDef = pCell;
	UpdateCell();
}

BOOL CDlgCellDoc::SaveModified() 
{
	BOOL bModified = FALSE;
	CDlgDataSource *pDS = GetDlgDataSource();
	if( pDS )
	{
		if(pDS->IsModified())
			bModified = TRUE;
	}
	
	//判断当前是否正在关闭文档
	HWND hWnd = ::GetActiveWindow();
	HWND hWnd0 = AfxGetMainWnd()->GetSafeHwnd();
	BOOL bClosing = FALSE;
	if( hWnd==hWnd0 || IsChild(hWnd0,hWnd) )
		bClosing = TRUE;
	
	if( !m_bPromptSave )
	{
		m_nPromptRet = IDNO;
		if( (IsModified()||bModified) && bClosing )
		{
			m_nPromptRet = AfxMessageBox(IDS_SAVECELLEDIT, MB_YESNOCANCEL);
			if( m_nPromptRet==IDCANCEL )return FALSE;
			
// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
		else if( bClosing )
		{
			m_nPromptRet = AfxMessageBox(IDS_SAVETIP2, MB_YESNO);
			if( m_nPromptRet==IDNO )return FALSE;
			
// 			if( (!m_strDemPath.IsEmpty()) && m_bModifyDEM )
// 			{
// 				if( AfxMessageBox(IDS_REMOVEDEM_TIP,MB_YESNO)==IDYES )
// 				{
// 					OnSaveDEM();
// 				}
// 			}
		}
	}
	
	if( bClosing )
	{
		Destroy();	
	}
	
	if( bModified )
	{
		if( !bClosing || (bClosing&&m_nPromptRet==IDYES) )
		{
// 			if (m_pCellEditDef)
// 			{
// 				m_pCellEditDef->SaveEditCell();
// 			}
// 			SaveBakFiles();
// 			::CopyFile(m_strTmpFile,GetPathName(),FALSE);
		}
	}
	
	if( bClosing )
	{
//		::DeleteFile(m_strTmpFile);//shy
	}
	
	m_bPromptSave = FALSE;

// 	if (m_pCellEditDef)
// 	{
// 		m_pCellEditDef->SendMessage(WM_CLOSE);
// 	}
	
	return TRUE;
}

BOOL CDlgCellDoc::UpdateCell()
{
	ClearAllObject();

	StartDefaultSelect();

	CellDef def = m_pCellEditDef->GetConfig()->pCellDefLib->GetCellDef(m_pCellEditDef->GetName());
	if (def.m_pgr == NULL) return FALSE;

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS)  return FALSE;

	CFtrLayer *pFtrLay = pDS->GetCurFtrLayer();
	if (!pFtrLay)  return FALSE;

	Graph2d *pGr = def.m_pgr->HeadGraph();
	float xoff , yoff;
	def.m_pgr->GetOrigin(xoff,yoff);
	while(pGr)
	{
		if(pGr->type == GRAPH_TYPE_POINT)
		{
			GrPoint2d *pPoint = (GrPoint2d*)pGr;
			CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOPOINT);
			if (!pFtr) continue;
			CGeoPoint *pGeo = (CGeoPoint*)pFtr->GetGeometry();
			if (!pGeo) continue;
			pGeo->CreateShape(&PT_3DEX(pPoint->pt.x+xoff,pPoint->pt.y+yoff,0,penLine),1);
			pGeo->SetColor(pPoint->color);
			pGeo->EnableFlated(pPoint->bFlat);
			pGeo->EnableGrdWid(pPoint->bGrdSize);
			pGeo->m_fKx = pPoint->kx;
			pGeo->m_fKy = pPoint->ky;
			
			AddObject(pFtr);
			
		}
		else if(pGr->type == GRAPH_TYPE_LINESTRING)
		{
			GrLineString2d *pLine = (GrLineString2d*)pGr;
			GrVertexList2d ptlist = pLine->ptlist;
			GrVertex2d *ptNodes = ptlist.pts;
			int ptnum = ptlist.nuse;

			CArray<PT_3DEX,PT_3DEX> arrpts;
			arrpts.SetSize(ptnum);

			int nMoveTo = 0;
			for (int i=0; i<ptnum; i++)
			{
				COPY_2DPT(arrpts[i],ptNodes[i]);
				arrpts[i].x += xoff;
				arrpts[i].y += yoff;
				arrpts[i].pencode = penLine;
				if (IsGrPtCodeMoveTo(ptNodes+i))
				{
					if (i>=1 && IsGrPtCodeLineTo(ptNodes+i-1) && GraphAPI::GGet3DDisOf2P(arrpts[i-1],arrpts[i])>GraphAPI::GetDisTolerance())
					{
						CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
						if (!pFtr) continue;
						CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
						if (!pGeo) continue;
						pGeo->CreateShape(arrpts.GetData()+nMoveTo,i-nMoveTo);
						pGeo->SetColor(pLine->color);
						if( pLine->bGrdWid )
							pGeo->m_fLineWidth = pLine->width;
						
						AddObject(pFtr);

						nMoveTo = i;
					}
										
				}
				else
				{
					if (i == ptnum-1)
					{
						CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
						if (!pFtr) continue;
						CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
						if (!pGeo) continue;
						pGeo->CreateShape(arrpts.GetData()+nMoveTo,i-nMoveTo+1);
						pGeo->SetColor(pLine->color);
						if( pLine->bGrdWid )
							pGeo->m_fLineWidth = pLine->width;
						
						AddObject(pFtr);
					}
				}


			}

	
		}
		else if(pGr->type == GRAPH_TYPE_POLYGON)
		{
			GrPolygon2d *pPolygon = (GrPolygon2d*)pGr;
			GrVertexList2d ptlist = pPolygon->ptlist;
			GrVertex2d *ptNodes = ptlist.pts;
			int ptnum = ptlist.nuse;

			CArray<PT_3DEX,PT_3DEX> arrpts;
			arrpts.SetSize(ptnum);
			
			int nMoveTo = 0;
			for (int i=0; i<ptnum; i++)
			{
				COPY_2DPT(arrpts[i],ptNodes[i]);
				arrpts[i].x += xoff;
				arrpts[i].y += yoff;
				arrpts[i].pencode = penLine;
				if (IsGrPtCodeMoveTo(ptNodes+i))
				{
					if (i>=1 && IsGrPtCodeLineTo(ptNodes+i-1) && GraphAPI::GGet3DDisOf2P(arrpts[i-1],arrpts[i])>GraphAPI::GetDisTolerance())
					{
						CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOSURFACE);
						if (!pFtr) continue;
						CGeoSurface *pGeo = (CGeoSurface*)pFtr->GetGeometry();
						if (!pGeo) continue;
						pGeo->CreateShape(arrpts.GetData()+nMoveTo,i-nMoveTo);
						pGeo->SetColor(pPolygon->color);						
						
						// 颜色填充
						if (pPolygon->filltype == POLYGON_FILLTYPE_COLOR)
						{
							pGeo->EnableFillColor(TRUE,pPolygon->color);
						}

						AddObject(pFtr);
						
						nMoveTo = i;
					}
					
				}
				else
				{
					if (i == ptnum-1)
					{
						CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOSURFACE);
						if (!pFtr) continue;
						CGeoSurface *pGeo = (CGeoSurface*)pFtr->GetGeometry();
						if (!pGeo) continue;
						pGeo->CreateShape(arrpts.GetData()+nMoveTo,i-nMoveTo+1);
						pGeo->SetColor(pPolygon->color);
						// 颜色填充
						if (pPolygon->filltype == POLYGON_FILLTYPE_COLOR)
						{
							pGeo->EnableFillColor(TRUE,pPolygon->color);
						}
						
						AddObject(pFtr);
					}
				}
				
				
			}
			
			
		}
		else if(pGr->type == GRAPH_TYPE_TEXT)
		{
			GrText *pText = (GrText*)pGr;

			CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOTEXT);
			if (!pFtr) continue;
			CGeoText *pGeo = (CGeoText*)pFtr->GetGeometry();
			if (!pGeo) continue;
			pGeo->CreateShape(&PT_3DEX(pText->pt.x+xoff,pText->pt.y+yoff,0,penLine),1);
			pGeo->SetColor(pText->color);
			pGeo->SetText(pText->text);

			TEXT_SETTINGS0 settings;
			settings.fHeight = pText->settings.fHeight;
			settings.fWidScale = pText->settings.fWidScale;
			settings.fCharIntervalScale = pText->settings.fCharIntervalScale;
			settings.fLineSpacingScale = pText->settings.fLineSpacingScale;
			settings.fTextAngle = pText->settings.fTextAngle;
			settings.fCharAngle = pText->settings.fCharAngle;			
			settings.nAlignment = pText->settings.nAlignment;
			settings.nInclineType = pText->settings.nInclineType;
			settings.fInclineAngle = pText->settings.fInclineAngle;		
			_tcscpy(settings.strFontName,pText->settings.tcFaceName);		
			pGeo->SetSettings(&settings);
				
			AddObject(pFtr);
			
		}
		
		pGr = pGr->next;
	}	

	pDS->SetModifiedFlag(FALSE);

	UpdateAllViews(NULL,hc_Refresh);

	return TRUE;
}

void CDlgCellDoc::OnMovetoCenter()
{

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	CFtrLayer *pFtrLay = pDS->GetCurFtrLayer();
	if (!pFtrLay)  return;
	CArray<PT_3D,PT_3D> arr;
	PT_3D cen;
	for (int i=pFtrLay->GetObjectCount()-1; i>=0; i--)
	{
		CFeature *pFtr = pFtrLay->GetObject(i);
		if (!pFtr) continue;
		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo) continue;
		
		PT_3DEX ptex0 = pGeo->GetDataPoint(0);
		pGeo->GetCenter(&ptex0,&cen);
		arr.Add(cen);		
	}

	cen.x = cen.y = cen.z = 0;
	int num = arr.GetSize();
	for (i=0; i<num; i++)
	{
		cen.x += arr[i].x;
		cen.y += arr[i].y;
		cen.z += arr[i].z;
		
	}
	cen.x /= num;
	cen.y /= num;
	cen.z /= num;

	// 离原点近就不要移了
	if (cen.x <= GraphAPI::GetDisTolerance() && cen.y <= GraphAPI::GetDisTolerance() && cen.z <= GraphAPI::GetDisTolerance())
	{
		return;
	}

	CUndoFtrs undo(this,"MoveToCenter");
	for (i=pFtrLay->GetObjectCount()-1; i>=0; i--)
	{
		CFeature *pFtr = pFtrLay->GetObject(i);
		if (!pFtr) continue;
		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo) continue;
		
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);
		for (int j=0; j<pts.GetSize(); j++)
		{
			pts[j].x -= cen.x;
			pts[j].y -= cen.y;
			pts[j].z -= cen.z;

		}
		CFeature *pNewFtr = pFtr->Clone();
		pNewFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());

		
		undo.arrOldHandles.Add(FtrToHandle(pFtr));
		//删除原来的对象
		DeleteObject(FtrToHandle(pFtr));
		//	pFtr1->SetID(OUID());
		if( !AddObject(pNewFtr) )
		{
			delete pNewFtr;
			continue;
		}			
		undo.arrNewHandles.Add(FtrToHandle(pNewFtr));	

		
	}

	undo.Commit();

	
}
void CDlgCellDoc::OnOverlayGrid() 
{
	CDlgCellOverlayGrid dlg;
	GetDlgDataSource()->GetBound(dlg.m_ptRegion,NULL,NULL);
	Envelope evlp;
	evlp.CreateFromPts(dlg.m_ptRegion,4);
	{
		dlg.m_bOverlay = m_bOverlayGrid;
		dlg.m_fWid = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"Width",1);
		dlg.m_fHei = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"Height",1);
		dlg.m_fXOrigin = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"MinX",evlp.m_xl);
		dlg.m_fYOrigin = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"MinY",evlp.m_yl);
		dlg.m_fXRange = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"XRange",evlp.m_xh-evlp.m_xl);
		dlg.m_fYRange = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"YRange",evlp.m_yh-evlp.m_yl);
		dlg.m_strZ = AfxGetApp()->GetProfileString(REGPATH_CELLOVERLAYGRID,"Z",_T("0.0"));
		dlg.m_bVectView= AfxGetApp()->GetProfileInt(REGPATH_CELLOVERLAYGRID,"ViewVect",FALSE);
		dlg.m_bImgView = AfxGetApp()->GetProfileInt(REGPATH_CELLOVERLAYGRID,"ViewImg",FALSE);
	}
	dlg.m_bSnapGrid= ((m_snap.GetSnapMode()&CSnap::modeGrid)!=0);
	dlg.m_color = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"Color",RGB(128,128,128));
	if( dlg.DoModal()!=IDOK )
		return;
	m_bOverlayGrid = dlg.m_bOverlay;
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"Width",dlg.m_fWid);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"Height",dlg.m_fHei);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"MinX",dlg.m_fXOrigin);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"MinY",dlg.m_fYOrigin);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"XRange",dlg.m_fXRange);
	WriteProfileDouble(REGPATH_CELLOVERLAYGRID,"YRange",dlg.m_fYRange);
	AfxGetApp()->WriteProfileString(REGPATH_CELLOVERLAYGRID,"Z",dlg.m_strZ);
	AfxGetApp()->WriteProfileInt(REGPATH_CELLOVERLAYGRID,"ViewVect",dlg.m_bVectView);
	AfxGetApp()->WriteProfileInt(REGPATH_CELLOVERLAYGRID,"ViewImg",dlg.m_bImgView);
	AfxGetApp()->WriteProfileInt(REGPATH_CELLOVERLAYGRID,"Color",dlg.m_color);
	if( dlg.m_bOverlay && dlg.m_bSnapGrid )
	{
		SNAP_GRID sg;
		sg.ox = dlg.m_fXOrigin;	sg.oy = dlg.m_fYOrigin;
		sg.dx = dlg.m_fWid;	sg.dy = dlg.m_fHei;
		sg.nx = fabs(dlg.m_fWid)<1e-10?0:ceil(dlg.m_fXRange/dlg.m_fWid);	
		sg.ny = fabs(dlg.m_fWid)<1e-10?0:ceil(dlg.m_fYRange/dlg.m_fWid);
		m_snap.SetSnapMode(m_snap.GetSnapMode()|CSnap::modeGrid);
		m_snap.SetGridParams(&sg);
	}
	else
		m_snap.SetSnapMode(m_snap.GetSnapMode()&(~CSnap::modeGrid));
	DWORD dwScale = GetDlgDataSource()->GetScale();
	double lfRatio = 1/*0.001*dwScale*/;//图上1cm对应实际lfRatio米
	GRIDParam pm;
	pm.ox = dlg.m_fXOrigin;	pm.oy = dlg.m_fYOrigin;
	pm.dx = dlg.m_fWid*lfRatio;	pm.dy = dlg.m_fHei*lfRatio;
	pm.xr = dlg.m_fXRange;	pm.yr = dlg.m_fYRange; 
	strcpy(pm.strz,dlg.m_strZ);
	pm.bVisible = dlg.m_bOverlay;
	pm.bViewVect= dlg.m_bVectView;
	pm.bViewImg = dlg.m_bImgView;
	pm.color = dlg.m_color;
	UpdateAllViews(NULL,hc_UpdateGrid,(CObject*)&pm);
}


void CDlgDoc::SetDataQueryType(BOOL bSymbolized)
{
	m_pDataQuery->m_bUseSymQuery = bSymbolized;
}


void CDlgDoc::OnDocInfoReload()
{
	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
	if (pScheme != NULL)
	{
		//USERIDX *pRecnet = pScheme->GetRecentIdx();
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRCLASS, WPARAM(0), LPARAM(pScheme));
	}

	AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(this));
	AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(this));

	//填充采集面板
	AfxGetMainWnd()->SendMessage(FCCM_LOAD_RECENTCMD, WPARAM(0), LPARAM(&m_RecentCmd));
}


void CDlgDoc::OnSelectNext()
{
	CDataQuery *pDQ = GetDataQuery();
	if( !pDQ )return;

	int num = m_selection.m_arrCanSelObjs.GetSize();
	if( num<=1 )return;

	for( int i=0; i<num; i++)
	{
		if( m_selection.IsObjInSelection(m_selection.m_arrCanSelObjs[i]) )
		{
			break;
		}
	}

	m_selection.DeselectAll();
	m_selection.SelectObj(m_selection.m_arrCanSelObjs[(i+1)%num]);
	OnSelectChanged(TRUE);

	UpdateAllViews(NULL,hc_Refresh);
}

void CDlgDoc::OnUpdateSelectNext(CCmdUI* pCmdUI)
{
	int num1 = 0, num2 = 0;
	m_selection.GetSelectedObjs(num1);

	num2 = m_selection.m_arrCanSelObjs.GetSize();

	pCmdUI->Enable(num1==1&&num2>1);
}


void CDlgDoc::OnSinglePhotoStereo()
{
	CWinApp *pApp = (CWinApp *)AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	
	BOOL bDoubleScreen	= pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, gdef_bDoubleScreen);
	
	CString pathName = _T("1.st2");
	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			pApp->GetNextDocTemplate(curTemplatePos);
		CDocument *rpDocMatch;
		
		if( curTemplate->MatchDocType( pathName, rpDocMatch )==CDocTemplate::yesAttemptNative )
		{
			if( bDoubleScreen && !curTemplate->IsKindOf(RUNTIME_CLASS(CSingleDocTemplate)) )
			{
				continue;
			}
			
			if( !bDoubleScreen && curTemplate->IsKindOf(RUNTIME_CLASS(CSingleDocTemplate)) )
			{
				continue;
			}
			
			CFrameWnd* pFrame = curTemplate->CreateNewFrame(this,NULL);
			
			POSITION pos = GetFirstViewPosition();
			CView *pView;
			while( pos )
			{
				pView = GetNextView(pos);
				if( pView->GetParentFrame()==pFrame )
					SendMessage(pView->m_hWnd,WM_STEREOVIEW_LOADMDL,
					0,0);
			}
			
			curTemplate->InitialUpdateFrame(pFrame, this, TRUE);
			break;
		}	
	}
}


void CDlgDoc::OnLayersCheck()
{
	CDlgLayerCodeCheck dlg;
	dlg.m_pDoc = this;

	dlg.DoModal();
}
void CDlgDoc::OnMutiimgview() 
{
	CWinApp *pApp = (CWinApp *)AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	
	CString str;
	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			pApp->GetNextDocTemplate(curTemplatePos);
		
		if (curTemplate->GetDocString(str,CDocTemplate::fileNewName))
		{
			if (str.CompareNoCase(_T("GL"))==0)
			{
				CFrameWnd* pFrame = curTemplate->CreateNewFrame(this,NULL);
				curTemplate->InitialUpdateFrame(pFrame, this, TRUE);
				break;
			}
		}		
	}
}




//去掉大数
double CutBigNumber(double x)
{
	long n = floor(x);
	return ((x-n)+(n%1000000));
}


static CFtrLayer* FindLayer(CDlgDataSource *pDS, LPCTSTR name, int geotype)
{
	CScheme *psch = gpCfgLibMan->GetScheme(pDS->GetScale());
	if( !psch )return NULL;

	__int64 code = _atoi64(name);
	if( code>0 )
	{
		int nLayers = psch->GetLayerDefineCount();
		for( int i=0; i<nLayers; i++)
		{
			CSchemeLayerDefine *pd = psch->GetLayerDefine(i);
			if( pd->GetLayerCode()==code )
			{
				return pDS->GetFtrLayer(pd->GetLayerName());
			}
		}

		int type = 0;
		if( geotype==CLS_GEOPOINT )type = 1;
		else if( geotype==CLS_GEOCURVE )type = 2;
		else if( geotype==CLS_GEOSURFACE )type = 3;
		else if( geotype==CLS_GEOTEXT )type = 4;

		code = code*10 + type;

		for( i=0; i<nLayers; i++)
		{
			CSchemeLayerDefine *pd = psch->GetLayerDefine(i);
			if( pd->GetLayerCode()==code )
			{
				return pDS->GetLocalFtrLayer(pd->GetLayerName());
			}
		}

		return NULL;
	}
	else
	{
		return pDS->GetFtrLayer(name);
	}
}

void CDlgDoc::OnImportJB()
{
	CDlgImportJB dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	CMilitaryVectorFile file;
	if( !file.Open(dlg.m_strFilePath) )
	{
		AfxMessageBox(StrFromResID(IDS_ERR_FAILOPENFILE));
		return;
	}

	const CVariantEx *p = NULL;

	//获取图廓左下角点
	double gxoff = 0, gyoff = 0;
	if( file.m_metaData.m_mapValues.GetValue(0,JBFIELD_MAPLBX,p) )
		gxoff = CutBigNumber((double)(_variant_t)*p) - 1000;
	if( file.m_metaData.m_mapValues.GetValue(0,JBFIELD_MAPLBY,p) )
		gyoff = (double)(_variant_t)*p - 1000;
	
	//层对照表
	CListFile lstLay;
	lstLay.Open(dlg.m_strLstFile);

	//遍历地物
	CValueTable tab;
	CArray<PT_3DEX,PT_3DEX> arrPts;

// 	CDpDBVariant varExcode;
// 	varExcode = (long)dlg.m_nExCode;

	DpCustomFieldType type;

	int i, lSum = 0;
	for( i=0; i<file.GetGroupCount(); i++)
	{
		LPCTSTR grpID = file.GetGroupID(i);
		lSum += file.GetObjectNumOfGroup(grpID);
	}

	CUndoFtrs undo(this,"Import JunBiao");

	GProgressStart(lSum);

	CFtrLayer *pOldLayer = NULL, *pLayer = NULL;
	CString oldLayName;

	CDlgDataSource *pDS = GetDlgDataSource();

	CAttributesSource *pXS = pDS->GetXAttributesSource(); 

	BeginBatchUpdate();

	for( i=0; i<file.GetGroupCount(); i++)
	{
		LPCTSTR grpID = file.GetGroupID(i);
		int nobject = file.GetObjectNumOfGroup(grpID);

		for( int j=0; j<nobject; j++)
		{
			GProgressStep();

			file.GetObjectData(grpID,j,tab);

			//获取地物类型
			tab.GetValue(0,JBFIELD_GEOTYPE,p);
			int geotype = (long)(_variant_t)*p;

			if( geotype==JBGEOTYPE_POINT )
				geotype = CLS_GEOPOINT;
			else if( geotype==JBGEOTYPE_LINE )
				geotype = CLS_GEOCURVE;
			else if( geotype==JBGEOTYPE_AREA )
				geotype = CLS_GEOSURFACE;
			else if( geotype==JBGEOTYPE_ANNOT )
				geotype = CLS_GEOTEXT;
			else if( geotype==JBGEOTYPE_COMPLICATED )
				continue;

			//获取层码
			tab.GetValue(0,JBFIELD_CODE,p);
			CString laycode = (LPCTSTR)(_bstr_t)(_variant_t)*p;
			CString layname = laycode;
			if( laycode==oldLayName )
			{
				pLayer = pOldLayer;
			}
			else
			{
				LPCTSTR match = lstLay.FindMatchItem(laycode);
				if( match )
				{
					layname = match;
					pLayer = pDS->GetFtrLayer(layname);
				}
				else
				{
					pLayer = FindLayer(pDS,layname,geotype);
				}
				
			}
			
			//创建地物
			CFeature *pFtr = NULL;
			
			if( pLayer!=NULL )
			{
				pFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),geotype);
			}

			if( pFtr==NULL )
			{
				char LayerName[128] = {0};
				switch(geotype)
				{
				case CLS_GEOPOINT:
					strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMEP));
					break;
				case CLS_GEOCURVE:
					strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMEL));
					break;
				case CLS_GEOSURFACE:
					strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMES));
					break;
				case CLS_GEOTEXT:
					strcpy(LayerName,StrFromResID(IDS_DEFLAYER_NAMET));
					break;
				}
				
				pLayer = pDS->GetFtrLayer(LayerName);
				if( !pLayer )continue;

				pFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),geotype);
			}

			if( pFtr==NULL || pLayer==NULL )continue;

			CScheme *psch = gpCfgLibMan->GetScheme(pDS->GetScale());
			CSchemeLayerDefine *pd = NULL;
			if( psch )pd = psch->GetLayerDefine(pLayer->GetName());
			
			CGeometry *pObj = pFtr->GetGeometry();

			//读取坐标点
			if( tab.GetValue(0,JBFIELD_SHAPE,p) )
			{
				arrPts.RemoveAll();
				p->GetShape(arrPts);

				if( arrPts.GetSize()<=0 )continue;

				PT_3DEX expt;
				if( geotype==CLS_GEOPOINT || geotype==CLS_GEOCURVE )
				{
					for( int k=0; k<arrPts.GetSize(); k++)
					{
						expt = arrPts.GetAt(k);
						expt.x += gxoff; expt.y += gyoff;
						arrPts.SetAt(k,expt);
					}					
				}
				else if( geotype==CLS_GEOSURFACE )
				{
					for( int k=1; k<arrPts.GetSize(); k++)
					{
						expt = arrPts.GetAt(k);
						expt.x += gxoff; expt.y += gyoff;
						arrPts.SetAt(k,expt);
					}

					arrPts.RemoveAt(0);
				}

				pObj->CreateShape(arrPts.GetData(),arrPts.GetSize());

				pFtr->SetCode(dlg.m_strMapName);

				pDS->AddObject(pFtr,pLayer->GetID());

				undo.AddNewFeature(FTR_HANDLE(pFtr));
				
				if( pd!=NULL )
				{
					CValueTable tab2;
					tab2.BeginAddValueItem();				
					
					//设置属性
					for(int n=0; n<tab.GetFieldCount(); n++)
					{
						int nftype;
						CString field,name;
						tab.GetField(n,field,nftype,name);
						
						int nField = pd->FindXDefineByName(field);
						if( nField<0 )continue;
						field = pd->GetXDefine(nField).field;
						
						tab.GetValue(0,n,p);
						tab2.AddValue(field,&CVariantEx(*p));
					}
					
					tab2.EndAddValueItem();
					pXS->SetXAttributes(pFtr,tab2);
				}

			}
			else
			{
				delete pFtr;
			}
		}
	}

	undo.Commit();

	EndBatchUpdate();

	GProgressEnd();
	
	UpdateAllViews(NULL,hc_UpdateAllObjects);
}


extern void SortBoundPoints(PT_3D pts[4]);


void CDlgDoc::OnExportJB()
{
	CDlgExportJB dlg;

	dlg.m_pDoc = this;

	dlg.m_dat.BeginAddValueItem();
	dlg.m_dat.AddValue("文件名",&CVariantEx((_variant_t)"Test"));
	dlg.m_dat.AddValue("数据密级",&CVariantEx((_variant_t)"B"));
	dlg.m_dat.AddValue("数据等级",&CVariantEx((_variant_t)"B"));
	dlg.m_dat.AddValue("遵循的标准号",&CVariantEx((_variant_t)"GJB,2.0"));
	dlg.m_dat.AddValue("系统数据区",&CVariantEx((_variant_t)"2009XXXXXXXXXXXXXXXXXXXXXXXXXXX"));
	dlg.m_dat.EndAddValueItem();

	if( dlg.DoModal()!=IDOK )
		return;
	
	CDlgDataSource *pDS = GetDlgDataSource();
	
	CAttributesSource *pXS = pDS->GetXAttributesSource(); 
	

	//准备
	int i, lSum = 0;
	for( i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		lSum += pLayer->GetObjectCount();
	}

	GProgressStart(lSum);

	const CVariantEx *p = NULL;
	CMilitaryVectorFile file;

	file.PrepareSave();

	//设置元数据
	file.m_metaData.SetValuesFromDoc(this);

	CString dataPath = m_strPathName;
	dataPath += ".met";
	file.m_metaData.LoadValues(dataPath);
	file.m_metaData.RefreshBigNumber();
	file.m_metaData.SetMapValue(JBFIELD_MAPNAME,"XXXX");
	file.m_metaData.SetMapValue(JBFIELD_MAPNUMBER,"YYYY");

	//获取图廓左下角点
	double gxoff = 0, gyoff = 0;
	PT_3D bounds[4];
	pDS->GetBound(bounds,NULL,NULL);
	SortBoundPoints(bounds);
	gxoff = bounds[0].x; gyoff = bounds[0].y;
	if( gxoff!=0 )
	{
		gxoff -= 1000;  gyoff -= 1000;
	}

	if( file.m_metaData.m_mapValues.GetValue(0,JBFIELD_MAPLBX,p) )
		gxoff = CutBigNumber((double)(_variant_t)*p) - 1000;
	if( file.m_metaData.m_mapValues.GetValue(0,JBFIELD_MAPLBY,p) )
		gyoff = (double)(_variant_t)*p - 1000;

	//设置文件头
	dlg.m_dat.GetValue(0,"数据密级",p);
	strncpy(file.m_fileHead.secret,(LPCTSTR)(_bstr_t)*p,sizeof(file.m_fileHead.secret)-1);
	dlg.m_dat.GetValue(0,"数据等级",p);
	strncpy(file.m_fileHead.level,(LPCTSTR)(_bstr_t)*p,sizeof(file.m_fileHead.level)-1);
	dlg.m_dat.GetValue(0,"遵循的标准号",p);
	strncpy(file.m_fileHead.standard,(LPCTSTR)(_bstr_t)*p,sizeof(file.m_fileHead.standard)-1);
	dlg.m_dat.GetValue(0,"系统数据区",p);
	strncpy(file.m_fileHead.system,(LPCTSTR)(_bstr_t)*p,sizeof(file.m_fileHead.system)-1);

	//向 CMilitaryVectorFile 中添加对象
	int nIndex = 0;
	for( i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObject = pLayer->GetObjectCount();
		CValueTable tab;

		CScheme *psch = gpCfgLibMan->GetScheme(pDS->GetScale());
		CSchemeLayerDefine *pd = NULL;
		if( psch )pd = psch->GetLayerDefine(pLayer->GetName());

		//遍历地物
		for( int j=0; j<nObject; j++)
		{
			GProgressStep();

			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;

			CGeometry *pObj = pFtr->GetGeometry();
			if( !pObj )continue;

			CFeature *pFtr0 = pFtr;			
			CGeometry *pGeo = pObj;
			
			CPFeature ftrs[2] = {pFtr,NULL};
			int nftr = 1;
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
			{
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
				{
					CGeometry *pGeo1 = NULL, *pGeo2 = NULL;
					((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						ftrs[0] = pFtr0->Clone();
						ftrs[1] = pFtr0->Clone();
						
						ftrs[0]->SetGeometry(pGeo1);
						ftrs[1]->SetGeometry(pGeo2);
						nftr = 2;
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
				else
				{
					CGeometry *pGeo1 = NULL, *pGeo2 = NULL;
					((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						ftrs[0] = pFtr0->Clone();
						ftrs[1] = pFtr0->Clone();
						
						ftrs[0]->SetGeometry(pGeo1);
						ftrs[1]->SetGeometry(pGeo2);
						nftr = 2;
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
			}

			for( int m=0; m<nftr; m++)
			{
				pFtr = ftrs[m];
				pObj = pFtr->GetGeometry();

				//获取编码
				__int64 code = 0;
				if( pd )code = pd->GetLayerCode();

				_variant_t var2;
				
				tab.DelAll();
				tab.BeginAddValueItem();

				//设置类型
				int geotype = 0;
				if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
					geotype = JBGEOTYPE_POINT;
				else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
					geotype = JBGEOTYPE_LINE;
				else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
					geotype = JBGEOTYPE_AREA;

				pFtr->WriteTo(tab);

				tab.DelField(FIELDNAME_SHAPE);

				CVariantEx var3;
				var3 = (_variant_t)(long)geotype;
				tab.AddValue(JBFIELD_GEOTYPE,&var3);

				nIndex++;
				var3 = (_variant_t)(long)nIndex;
				tab.AddValue(JBFIELD_INDEX,&var3);
				
				if( code>=1e+7 )code = code/100;
				if( code>=1e+6 )code = code/10;

				//编码
				CString strcode;
				strcode.Format("%I64d", code);
				var2 = (LPCTSTR)strcode;
				tab.AddValue(JBFIELD_CODE,&CVariantEx(var2));

				//写坐标
				CArray<PT_3DEX,PT_3DEX> arrPts;
				int nPt = pObj->GetDataPointSum();
				PT_3DEX expt;

				double lfElevation = 0.0;//高程
				
				if( geotype==JBGEOTYPE_AREA )
				{
					pObj->GetCenter(NULL,&expt);
					expt.pencode = penLine;
					expt.x -= gxoff; expt.y -= gyoff;
					arrPts.Add(expt);
				}

				for( int k=0; k<nPt; k++)
				{
					expt = pObj->GetDataPoint(k);
					if (k == 0) lfElevation = expt.z;
					expt.x -= gxoff; expt.y -= gyoff;
					arrPts.Add(expt);
				}
				
				var3.SetAsShape(arrPts);
				tab.AddValue(JBFIELD_SHAPE,&var3);
				
				//写属性
				if( pd && pXS )
				{
					CValueTable tab2;

					tab2.BeginAddValueItem();
					pXS->GetXAttributes(pFtr,tab2);
					tab2.EndAddValueItem();

					for( int n=0; n<tab2.GetFieldCount(); n++)
					{
						const CVariantEx *pv = NULL;
						if( tab2.GetValue(0,n,pv) )
						{
							//字段名转换成别名
							CString field, name;
							int type = 0;
							tab2.GetField(n,field,type,name);
							int nField = pd->FindXDefine(field);
							if( nField>=0 )
							{
								name = pd->GetXDefine(nField).name;
							}
							else
								name = field;
							tab.AddValue(name,(CVariantEx*)pv);
						}
					}
				}

				//高程
				{
					const CVariantEx *pv = NULL;
					if (tab.GetValue(0, "高程", pv))
					{
						double val = (double)(_variant_t)*pv;
						if (fabs(val) < 1e-6)
						{
							_variant_t var;
							var = (double)lfElevation;
							tab.SetValue(0, "高程", &CVariantEx(var));
						}
					}
					else
					{
						_variant_t var;
						var = (double)lfElevation;
						tab.AddValue("高程", &CVariantEx(var));
					}
				}

				if( code<110000 )
				{
					if( geotype==JBGEOTYPE_POINT )
						code = 200201;
					if( geotype==JBGEOTYPE_LINE )
						code = 200108;
					if( geotype==JBGEOTYPE_AREA )
						code = 210310;
					if( geotype==JBGEOTYPE_ANNOT )
						code = 280302;
				}

				file.AddObject(file.GetGroupID((code-100000)/10000-1),tab);
			}

			if( nftr==2 )
			{
				delete ftrs[0];
				delete ftrs[1];
			}
		}
	}

	//保存
	//获得文件名
	dlg.m_dat.GetValue(0,"文件名",p);
	file.Save(dlg.m_strFilePath,(LPCTSTR)(_bstr_t)*p,TRUE);

	GProgressEnd();
}

#include "DlgExportASC.h"
void CDlgDoc::OnExportASC()
{
	CDlgExportASC dlg;
	GetModuleFileName(NULL,dlg.m_strLstFile.GetBuffer(300),299);
	dlg.m_strLstFile.ReleaseBuffer();
	int nstrpos = dlg.m_strLstFile.ReverseFind('\\');
	if( nstrpos )dlg.m_strLstFile = dlg.m_strLstFile.Left(nstrpos);
	nstrpos = dlg.m_strLstFile.ReverseFind('\\');
	if( nstrpos )dlg.m_strLstFile = dlg.m_strLstFile.Left(nstrpos);
	char Path[_MAX_PATH];
	strncpy(Path,GetConfigPath(),_MAX_PATH-1);	
	dlg.m_strLstFile = CString(Path)+_T("arcCodeList.txt");
	if( dlg.DoModal()!=IDOK )
		return;
	CString path = dlg.m_strPath;
	FILE *fp = fopen(path,"wt");
	if( !fp )return;
	CGeometry *pGeo = NULL;
	long lObjSum = 0;

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;

	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer || !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		lObjSum += nObjs;
	}

	PT_3D boundpts[4];
	pDS->GetBound(boundpts,NULL,NULL);
	fprintf( fp,"MAPBOX 9610 5 -1 0 0.00000\n");
	for( i=0; i<4; i++)
	{
		fprintf( fp,"%.6f %.6f %.6f\n",boundpts[i].x,boundpts[i].y,0.0);
	}
	fprintf( fp,"%.6f %.6f %.6f\n",boundpts[0].x,boundpts[0].y,0.0);

	GProgressStart(lObjSum);

	CMultiListFile lstFile;
	lstFile.Open(dlg.m_strLstFile);
	for ( i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer || !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		for(int j=0; j<nObjs; j++)
		{
			GProgressStep();

			CFeature *pFtr = pLayer->GetObject(j);	
			if( !pFtr || !pFtr->IsVisible()) continue;

			__int64 code = 0;
			CString name0 = pLayer->GetName();
			CString name = "0";
			CString symbol = "0";
			
			if( pDS->FindLayerIdx(FALSE,code,name0) )
			{
				name.Format("%I64d",code);
				symbol = lstFile.FindTextItem((LPCTSTR)name,0,6,"0");
				name = lstFile.FindTextItem((LPCTSTR)name,0,7,"0");
			}

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;

			int npt = pGeo->GetDataPointSum();

			CGeometry *pObj2 = NULL;
			CGeometry *p = pGeo;
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				fprintf(fp,"POINT %s %d -1 %s 0.000000\n",(LPCTSTR)name,npt,(LPCTSTR)symbol);
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface))  )
			{
				pObj2 = pGeo->Linearize();
				if( pObj2 )
				{
					npt = pObj2->GetDataPointSum();
					p = pObj2;
				}
				fprintf(fp,"LINE %s %d -1 %s 0.000000\n",(LPCTSTR)name,npt,(LPCTSTR)symbol);
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				npt = 1;
				fprintf(fp,"TEXT %s %d -1 %s 0.000000\n",(LPCTSTR)name,npt,(LPCTSTR)symbol);
			}

			CArray<PT_3DEX,PT_3DEX> arr;
			p->GetShape(arr);

			PT_3DEX expt;
			for( int k=0; k<npt; k++)
			{
				expt = arr[k];
				fprintf(fp,"%.6f %.6f %.6f\n",expt.x,expt.y,expt.z);
			}
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				fprintf(fp,"CELL %s %d -1 %s 0.000000\n",(LPCTSTR)name,npt,(LPCTSTR)symbol);
				fprintf(fp,"%.6f %.6f %.6f\n",expt.x,expt.y,expt.z);
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				CGeoText *pText = (CGeoText*)pGeo;
				fprintf(fp,"%s\n",(LPCTSTR)pText->GetText());
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
			{
				CGeometry *pObj3, *pObj4;
				if( ((CGeoParallel*)pGeo)->Separate(pObj3,pObj4) )
				{
					if( pObj3 )
					{
						delete pObj3;
					}
					if( pObj4 )
					{
						CArray<PT_3DEX,PT_3DEX> arr1;
						pObj4->GetShape(arr1);

						fprintf(fp,"LINE %s %d -1 %s 0.000000\n",(LPCTSTR)name,npt,(LPCTSTR)symbol);						
						for( k=0; k<npt; k++)
						{
							expt = arr1[k];
							fprintf(fp,"%.6f %.6f %.6f\n",expt.x,expt.y,expt.z);
						}
						delete pObj4;
					}
				}
			}

			CPtrArray arrAnns;
			CConfigLibManager *pCLM = gpCfgLibMan;
			pCLM->GetSymbol(pDS,pFtr,arrAnns);
			
			for(int i=0; i<arrAnns.GetSize(); i++)
			{
				CSymbol *pSymbol = (CSymbol*)arrAnns.GetAt(i);
				if (pSymbol && pSymbol->GetType()==SYMTYPE_ANNOTATION )
				{
					CAnnotation *pAnnot = (CAnnotation*)arrAnns.GetAt(i);
					
					if (pAnnot->m_nAnnoType != CAnnotation::Attribute) continue;
					
					CPtrArray parr;
					CValueTable tab;
					pDS->GetAllAttribute(pFtr,tab);
					if( pAnnot->ExtractGeoText(pFtr,parr,tab,pDS->GetSymbolDrawScale()) )
					{
						for (int j=0; j<parr.GetSize(); j++)
						{
							CGeoText *pText = (CGeoText*)parr[j];
							fprintf(fp,"TEXT %s 1 -1 0 0.000000\n",(LPCTSTR)name);
							fprintf(fp,"%.6f %.6f %.6f\n",expt.x,expt.y,expt.z);
							fprintf(fp,"%s\n",(LPCTSTR)pText->GetText());
							delete pText;					
							
						}				
						
					}
				}
			}
			
			if( pObj2!=NULL )delete pObj2;
		}
	}
	fclose(fp);
	
	GProgressEnd();
}

#include "ValueTable.h"
#include "DlgExportCas.h"
extern void VariantToText(const CVariantEx& v, CString &text);
void CDlgDoc::OnExportCas()
{
	CDlgExportCas dlg;
	if( dlg.DoModal()!=IDOK )
		return;
	CString path = dlg.m_strPath;
	FILE *fp = fopen(path,"wt");
	if( !fp )return;
	CGeometry *pGeo = NULL;
	long lObjSum = 0;

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;

	CAttributesSource  *pDXS = pDS->GetXAttributesSource();
	if (!pDXS) return;

	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer || !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		lObjSum += nObjs;
	}

	PT_3D boundpts[4];
	pDS->GetBound(boundpts,NULL,NULL);
	fprintf( fp,"CASS7\n");

	fprintf( fp,"%.4f,%.4f\n",boundpts[0].x,boundpts[0].y);
	fprintf( fp,"%.4f,%.4f\n",boundpts[2].x,boundpts[2].y);

	GProgressStart(lObjSum);

	CMultiListFile lstFile;
	lstFile.Open(dlg.m_strLstFile);
	for ( i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer || !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		if (nObjs <= 0) continue;

		CString strLayerName = pLayer->GetName();

		__int64 code = 0;
		
		// 对照表格式：FeatureOneCode   cassCode  cassLayerName   
		CString cassCode = "0";
		CString casNname = "0";
		
		if( pDS->FindLayerIdx(FALSE,code,strLayerName) )
		{
			CString strCode;
			strCode.Format("%I64d",code);
			cassCode = lstFile.FindTextItem((LPCTSTR)strCode,0,1,strCode);
			casNname = lstFile.FindTextItem((LPCTSTR)strCode,0,2,strCode);
		}

		fprintf( fp,"[%s]\n",casNname);

		for(int j=0; j<nObjs; j++)
		{
			GProgressStep();

			CFeature *pFtr = pLayer->GetObject(j);	
			if( !pFtr || !pFtr->IsVisible()) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;

			int npt = pGeo->GetDataPointSum();
			if (npt < 1) continue;

			CArray<PT_3DEX,PT_3DEX> arr;
			pGeo->GetShape(arr);

			double z_avg = 0;
			if( CAutoLinkContourCommand::CheckObjForZ(pGeo) )
			{
				z_avg = arr[0].z;
			}

			// 点状地物(POINT)
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				fprintf(fp,"POINT\n");

				double ang = 0;
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)))
				{
					ang = ((CGeoDirPoint*)pGeo)->GetDirection()/180*PI; 
				}
				if (0 == StrFromResID(IDS_CULVERT_NOTPROPORT).Compare(pLayer->GetName()))
				{
					ang+=PI;
				}

				fprintf(fp,"%s,%.4f\n",cassCode,ang);
				fprintf(fp,"%.4f,%.4f,%.4f\n",arr[0].x,arr[0].y,arr[0].z);

				//写扩展属性值								
				CValueTable tab;
				tab.BeginAddValueItem();
				pDXS->GetXAttributes(pFtr,tab);
				tab.EndAddValueItem();
				int nField = tab.GetFieldCount();
				for( int k=0; k<nField; k++)
				{		
					CString field,name;
					int type;
					if (tab.GetField(k,field,type,name))
					{
						const CVariantEx* value;
						if (tab.GetValue(0,field,value))
						{	
							CString strValue;
							VariantToText(*value,strValue);
							fprintf(fp,"%s,%s:\n",field,strValue);
						}
					}
				}

			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			{
				if (npt < 2) continue;
				
				// 对于平行线类地物应放在ASSIST层生成骨架线，如桥
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
				{
/*
					CGeoParallel *pParGeo = (CGeoParallel*)pGeo;

					CArray<PT_3DEX,PT_3DEX> ptspar;
					pParGeo->GetParallelShape(ptspar);
					
					for (int i=ptspar.GetSize()-1; i>=0; i--)
					{
						arr.Add(ptspar.GetAt(i));
					}

					int pencode = arr[1].pencode;
					
					CString strSpline = _T("N");
					if (pencode == penSpline)
					{
						strSpline = _T("F");
					}
					
					fprintf(fp,"PLINE\n");
					fprintf(fp,"%s,%.3f,%s,%.3f\n",cassCode,z_avg,strSpline,0.0);
					
					for (int k=0; k<arr.GetSize(); k++)
					{
						fprintf(fp,"%.3f,%.3f\n",arr[k].x,arr[k].y);
					}

					fprintf(fp,"C\n");
*/
					// 坡底
					int pencode = arr[1].pencode;
					
					CString strSpline = _T("N");
					if (pencode == penSpline)
					{
						strSpline = _T("F");
					}
					
					CString cassCode1 = cassCode;
					fprintf(fp,"PLINE\n");
					fprintf(fp,"%s,%.4f,%s,%.4f\n",cassCode1,z_avg,strSpline,0.0);
					
					for (int k=0; k<arr.GetSize(); k++)
					{
						fprintf(fp,"%.4f,%.4f\n",arr[k].x,arr[k].y);
					}
					
					fprintf(fp,"E\n");					
					
					// 坡顶
					CArray<PT_3DEX,PT_3DEX> arr1;
					((CGeoParallel*)pGeo)->GetParallelShape(arr1);

					if( arr1.GetSize()>1 )
					{
						fprintf(fp,"e\n");						
	
						pencode = arr1[1].pencode;
						
						strSpline = _T("N");
						if (pencode == penSpline)
						{
							strSpline = _T("F");
						}
						
						CString cassCode2 = cassCode;
						//fprintf(fp,"PLINE\n");
						fprintf(fp,"%s,%.4f,%s,%.4f\n",cassCode2,z_avg,strSpline,0.0);
						
						for (k=0; k<arr1.GetSize(); k++)
						{
							fprintf(fp,"%.4f,%.4f\n",arr1[k].x,arr1[k].y);
						}
						
						fprintf(fp,"E\n");						
					}
				}
				// 对于双线类地物，如自然斜坡，应同时导出坡底(编码+1)和坡顶(编码+2)
				else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
				{					
					// 坡顶
					CArray<PT_3DEX,PT_3DEX> arr1;
					((CGeoDCurve*)pGeo)->GetAssistShape(arr1);

					// 坡底
					int pencode = arr[1].pencode;
					
					CString strSpline = _T("N");
					if (pencode == penSpline)
					{
						strSpline = _T("F");
					}
					
					CString cassCode1 = cassCode;
					cassCode1.SetAt(cassCode1.GetLength()-1,'1');
					fprintf(fp,"PLINE\n");
					fprintf(fp,"%s,%.4f,%s,%.4f\n",cassCode1,z_avg,strSpline,0.0);
					
					for (int k=0; k<arr.GetSize(); k++)
					{
						fprintf(fp,"%.4f,%.4f\n",arr[k].x,arr[k].y);
					}

					if( arr1.GetSize()>1 )
					{					
						fprintf(fp,"E\n");
						
						fprintf(fp,"e\n");

						pencode = arr1[1].pencode;
						
						strSpline = _T("N");
						if (pencode == penSpline)
						{
							strSpline = _T("F");
						}
						
						CString cassCode2 = cassCode;
						cassCode2.SetAt(cassCode2.GetLength()-1,'2');
						//fprintf(fp,"PLINE\n");
						fprintf(fp,"%s,%.4f,%s,%.4f\n",cassCode2,z_avg,strSpline,0.0);
						
						for (k=0; k<arr1.GetSize(); k++)
						{
							fprintf(fp,"%.4f,%.4f\n",arr1[k].x,arr1[k].y);
						}
						
						fprintf(fp,"E\n");
					}
					else
					{						
						fprintf(fp,"e\n");
					}
				}
				else
				{
					int pencode = arr[1].pencode;
					
					CGeometry *pGeo2 = NULL;
					CString strSpline = _T("N");
					if (pencode == penSpline)
					{
						strSpline = _T("F");
					}
					else 
					{
						CGeometry *pGeo2 = pGeo->Linearize();
						if (pGeo2)
						{
							pGeo2->GetShape(arr);
							npt = arr.GetSize();
						}
					}

					fprintf(fp,"PLINE\n");
					fprintf(fp,"%s,%.4f,%s,%.4f\n",cassCode,z_avg,strSpline,0.0);
					
					for (int k=0; k<npt; k++)
					{
						fprintf(fp,"%.4f,%.4f\n",arr[k].x,arr[k].y);
					}
					
					if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pGeo)->IsClosed())
					{
						fprintf(fp,"C\n");
					}
					else				
					{
						fprintf(fp,"E\n");
					}
					
					if(pGeo2)
						delete pGeo2;
				}
				
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				if (npt < 2) continue;
				
				int pencode = arr[1].pencode;
				
				CGeometry *pGeo2 = NULL;
				CString strSpline = _T("N");
				if (pencode == penSpline)
				{
					strSpline = _T("F");
				}
				else 
				{
					CGeometry *pGeo2 = pGeo->Linearize();
					if (pGeo2)
					{
						pGeo2->GetShape(arr);
						npt = arr.GetSize();
					}
				}				
				
				fprintf(fp,"PLINE\n");
				fprintf(fp,"%s,%.4f,%s,%.4f\n",cassCode,z_avg,strSpline,0.0);
				
				for (int k=0; k<npt; k++)
				{
					fprintf(fp,"%.4f,%.4f\n",arr[k].x,arr[k].y);
				}
				
				fprintf(fp,"C\n");
				
				if(pGeo2)
					delete pGeo2;
			}
			// 文字(TEXT)
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				fprintf(fp,"TEXT\n");
				
				CGeoText *pText = (CGeoText*)pGeo;

				TEXT_SETTINGS0 setting;
				pText->GetSettings(&setting);

				fprintf(fp,"%s,%.4f,%.4f\n",cassCode,setting.fHeight,setting.fInclineAngle/180*PI);
				fprintf(fp,"%s\n",pText->GetText());
				fprintf(fp,"%.4f,%.4f,%.4f\n",arr[0].x,arr[0].y,arr[0].z);

			}


			fprintf(fp,"e\n");

			fprintf(fp,"nil\n");
		}
	}

	fprintf(fp,"END\n");


	fclose(fp);
	
	GProgressEnd();
}

void CDlgDoc::OnSaveDEMPointstoDEM()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	
	CFtrLayer *pLayer = pDS->GetFtrLayer("DEMPoints");
	if( !pLayer || !pLayer->IsVisible() )
	{
		AfxMessageBox(IDS_ERR_NOTFOUNDDEMPOINTS);
		return;
	}

	long lObjSum = pLayer->GetObjectCount();
	if( lObjSum==0 )
	{
		AfxMessageBox(IDS_ERR_NOTFOUNDDEMPOINTS);
		return;
	}

	CDSM dem;
	{
		CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			_T("All Files(*.*)|*.*||"));
		
		if( dlg.DoModal()!=IDOK )return;
		if (!PathFileExists(dlg.GetPathName()))
		{
			AfxMessageBox(StrFromResID(IDS_INVALID_FILE));
			return;
		}
		//clear old info
		dem.Open("####$$$$");
		
		//load the new file
		if( !dem.Open(dlg.GetPathName()) )
			return;		
		
	}
	
	
	DEMHEADERINFO dh;
	dh = dem.m_demInfo;

	GProgressStart(lObjSum);
	
	for (int i=0; i<lObjSum; i++)
	{
		CFeature *pFtr = pLayer->GetObject(i);	

		GProgressStep();
		if( !pFtr || !pFtr->IsVisible()) continue;

		CGeometry *pGeo = pFtr->GetGeometry();

		if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)) )
			continue;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo->GetShape(arrPts);

		PT_3DEX expt;
		int nx, ny;
		for( int j=0; j<arrPts.GetSize(); j++)
		{
			expt = arrPts[j];

			nx	= ( expt.x - dh.lfStartX ) /dh.lfDx;
			ny	= ( expt.y - dh.lfStartY ) /dh.lfDy;
			
			if( fabs(expt.x-dh.lfStartX-nx*dh.lfDx)>1e-4 )continue;
			if( fabs(expt.y-dh.lfStartY-ny*dh.lfDy)>1e-4 )continue;

			dem.SetZ(nx,ny,expt.z);
		}
		
	}

	dem.Save();	

	GProgressEnd();
}


void CDlgDoc::OnRemoveDem()
{
	CUndoFtrs undo(this,StrFromResID(IDS_CMDNAME_DEL));
	BeginBatchUpdate();
	int nSum = 0;
	for (int i=0; i<GetDlgDataSourceCount(); i++)
	{
		CDlgDataSource *pDS = GetDlgDataSource(i);
		if (!pDS)  continue;
		
		for (int j=0; j<pDS->GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(j);
			if( !pLayer )continue;
			
			for( int k=0; k<pLayer->GetObjectCount(); k++)
			{
				CFeature *pFtr = pLayer->GetObject(k,FILTERMODE_DELETED);
				if( !pFtr )continue;

				if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
				{
					DeleteObject(FtrToHandle(pFtr));
					undo.AddOldFeature(FtrToHandle(pFtr));
				}
			}
		}
	}
	EndBatchUpdate();

	undo.Commit();
}

void CDlgDoc::ReCreateAllFtrs()
{
	int nSum = 0;
	for (int i=0; i<GetDlgDataSourceCount(); i++)
	{
		CDlgDataSource *pDS = GetDlgDataSource(i);
		if (!pDS)  continue;
		
		for (int j=0; j<pDS->GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(j);
			if( !pLayer || !pLayer->IsVisible() )continue;
			
			nSum += pLayer->GetObjectCount();
		}
	}

	GOutPut(StrFromResID(IDS_TIP_RESET_QUERYENGINE));
	GProgressStart(nSum);

	for (i=0; i<GetDlgDataSourceCount(); i++)
	{
		CDlgDataSource *pDS = GetDlgDataSource(i);
		if (!pDS)  continue;
		
		for (int j=0; j<pDS->GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(j);
			if( !pLayer || !pLayer->IsVisible() )continue;
				
			int nObjs = pLayer->GetObjectCount();
			for(int k=0; k<nObjs; k++)
			{	
				GProgressStep();

				CFeature *pFtr = pLayer->GetObject(k);	
				if (!pFtr) continue;

				CGeometry *pGeo = pFtr->GetGeometry();
				if (pGeo)
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(arrPts);
					
					pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
					
				}

				pDS->UpdateFtrQuery(pFtr);
			}
		}
	}

	GProgressEnd();
	
	UpdateAllViews(NULL,hc_UpdateAllObjects);
}

void CDlgDoc::UpdateSymDrawScale()
{
	GOutPut(StrFromResID(IDS_TIP_RESET_QUERYENGINE));

	double fDrawScale = GetProfileDouble(REGPATH_SYMBOL,REGITEM_DRAWSCALE,1);
	double fAnnoScale = GetProfileDouble(REGPATH_SYMBOL,REGITEM_ANNOSCALE,1);
	for (int i=0; i<GetDlgDataSourceCount(); i++)
	{
		CDlgDataSource *pDS = GetDlgDataSource(i);
		if (!pDS)  continue;
		
		pDS->SetDrawScaleByUser(fDrawScale);

		pDS->UpdateFtrQuery();
	}

	SetSymbolDrawScale(fDrawScale);
	SetSymbolAnnoScale(fAnnoScale);
	
	UpdateAllViews(NULL,hc_UpdateAllObjects);

}


static BOOL Get2EndLineOverlapLine0(double x0,double y0,double z0,double x1,double y1,double z1,double x2,double y2,double z2, double x3, double y3,double z3,double *startx, double *starty, double *startz,double *endx, double *endy,double *endz, double *t0,double *t1)
{
	double xmin0 = min(x0,x1);
	double xmax0 = max(x0,x1);
	double xmin1 = min(x2,x3);
	double xmax1 = max(x2,x3);
	if (xmin0>xmax1||xmax0<xmin1) return FALSE;
	double ymin0 = min(y0,y1);
	double ymax0 = max(y0,y1);
	double ymin1 = min(y2,y3);
	double ymax1 = max(y2,y3);
	if (ymin0>ymax1||ymax0<ymin1) return FALSE;
	//将线段2的两点投影到线段1上，垂距超限差那么就不重叠
	double toler = GraphAPI::g_lfDisTolerance;
	double retx,rety,retz;
	GraphAPI::GGetPerpendicular3D(x0,y0,z0, x1,y1,z1, x2,y2,z2, &retx,&rety,&retz);
	if( fabs(x2-retx)>toler || fabs(y2-rety)>toler || fabs(z2-retz)>toler)
		return FALSE;
	GraphAPI::GGetPerpendicular3D(x0,y0,z0, x1,y1,z1, x3,y3,z3, &retx,&rety,&retz);
	if( fabs(x3-retx)>toler || fabs(y3-rety)>toler || fabs(z3-retz)>toler)
		return FALSE;
	
	double temp0,temp1;
	if( fabs(x0-x1)<1e-6 )
	{
		temp0 = (y2-y0)/(y1-y0);
		temp1 = (y3-y0)/(y1-y0);
	}
	else
	{
		temp0 = (x2-x0)/(x1-x0);
		temp1 = (x3-x0)/(x1-x0);
	}
	if (temp1>temp0)
	{
		if (temp0<0)
		{
			if (t0) *t0 = 0.0;
			if(startx)*startx = x0;
			if(starty)*starty = y0;
			if(startz)*startz = z0;
		}
		else
		{
			if (t0) *t0 = temp0;
			if(startx)*startx = x2;
			if(starty)*starty = y2;
			if(startz)*startz = z2;
		}
		if (temp1>1)
		{
			if (t1) *t1 = 1.0;
			if(endx)*endx = x1;
			if(endy)*endy = y1;
			if(endz)*endz = z1;
		}
		else
		{
			if (t1) *t1 = temp1;
			if(endx)*endx = x3;
			if(endy)*endy = y3;
			if(endz)*endz = z3;
		}			
	}
	else
	{
		if (temp1<0)
		{
			if (t0) *t0 = 0.0;
			if(startx)*startx = x0;
			if(starty)*starty = y0;
			if(startz)*startz = z0;
		}
		else
		{
			if (t0) *t0 = temp1;
			if(startx)*startx = x3;
			if(starty)*starty = y3;
			if(startz)*startz = z3;
		}
		if (temp0>1)
		{
			if (t1) *t1 = 1.0;
			if(endx)*endx = x1;
			if(endy)*endy = y1;
			if(endz)*endz = z1;
		}
		else
		{
			if (t1) *t1 = temp0;
			if(endx)*endx = x2;
			if(endy)*endy = y2;
			if(endz)*endz = z2;
		}			
	}
	return TRUE;	
}

static void addOnverlapSectionToArray(CArray<OverlapSection,OverlapSection> &arry,const OverlapSection &item)
{
	BOOL bOut0=FALSE,bOut1=FALSE;
	int nStart=-1,nEnd=-1;
	for (int i=0;i<arry.GetSize();i++)
	{
		if(nStart==-1)
		{
			if ((item.idx0+item.t0) < (arry[i].idx0+arry[i].t0))
			{
				bOut0 = TRUE;
				nStart = i;
			}
			else if ((item.idx0+item.t0) < (arry[i].idx1+arry[i].t1))
			{
				bOut0 = FALSE;
				nStart = i;
			}
		}			
		if(nStart!=-1&&nEnd==-1)
		{
			if((item.idx1+item.t1) < (arry[i].idx0+arry[i].t0))
			{
				bOut1 = TRUE;
				nEnd = i;
				break;
			}
			else if((item.idx1+item.t1) < (arry[i].idx1+arry[i].t1))
			{
				bOut1 = FALSE;
				nEnd = i;
				break;
			}
		}
	}
	if (nStart==-1)
	{
		arry.Add(item);
	}
	else if (nStart!=-1&&nEnd==-1)
	{
		if (bOut0)
		{
			arry.RemoveAt(nStart,arry.GetSize()-nStart);
			arry.Add(item);
		}
		else
		{
			OverlapSection item0 = item;
			item0.t0 = arry[nStart].t0;
			item0.x0 = arry[nStart].x0;
			item0.y0 = arry[nStart].y0;
			item0.z0 = arry[nStart].z0;
			arry.RemoveAt(nStart,arry.GetSize()-nStart);
			arry.Add(item0);
		}
	}
	else
	{
		if (bOut0&&bOut1)
		{
			if (nStart==nEnd)
			{
				arry.InsertAt(nStart,item);
			}
			else
			{
				arry.RemoveAt(nStart,nEnd-nStart);
				arry.InsertAt(nStart,item);
			}
		}
		else if (bOut0&&!bOut1)
		{
			if (nStart==nEnd)
			{
				arry.ElementAt(nStart).t0 = item.t0;
				arry.ElementAt(nStart).x0 = item.x0;
				arry.ElementAt(nStart).y0 = item.y0;
				arry.ElementAt(nStart).z0 = item.z0;
			}
			else
			{
				arry.RemoveAt(nStart,nEnd-nStart);
				arry.ElementAt(nStart).t0 = item.t0;
				arry.ElementAt(nStart).x0 = item.x0;
				arry.ElementAt(nStart).y0 = item.y0;
				arry.ElementAt(nStart).z0 = item.z0;
			}
		}
		else if (!bOut0&&bOut1)
		{
			if(nEnd-nStart==1)
			{
				arry.ElementAt(nStart).t1 = item.t1;
				arry.ElementAt(nStart).x1 = item.x1;
				arry.ElementAt(nStart).y1 = item.y1;
				arry.ElementAt(nStart).z1 = item.z1;
			}
			else
			{
				arry.RemoveAt(nStart+1,nEnd-nStart-1);
				arry.ElementAt(nStart).t1 = item.t1;
				arry.ElementAt(nStart).x1 = item.x1;
				arry.ElementAt(nStart).y1 = item.y1;
				arry.ElementAt(nStart).z1 = item.z1;
			}
		}
		else
		{
			if (nStart==nEnd)
			{
				;
			}
			else
			{
				arry.ElementAt(nEnd).t0 = arry.ElementAt(nStart).t0;
				arry.ElementAt(nEnd).x0 = arry.ElementAt(nStart).x0;
				arry.ElementAt(nEnd).y0 = arry.ElementAt(nStart).y0;
				arry.ElementAt(nEnd).z0 = arry.ElementAt(nStart).z0;
				arry.RemoveAt(nStart,nEnd-nStart);
			}
		}
	}
}

static void addOverlapSectionToItemArray(CArray<OverlapLineItem,OverlapLineItem> &arrFtrs, CFeature *pFtr, const OverlapSection &item)
{
	int nsz = arrFtrs.GetSize();
	for (int i=0; i<nsz; i++)
	{
		if (arrFtrs[i].pFtr == pFtr)
		{
			addOnverlapSectionToArray(arrFtrs[i].arry,item);
			break;
		}
	}

	if (i >= nsz)
	{
		OverlapLineItem ftrItem;
		ftrItem.pFtr = pFtr;
		ftrItem.arry.Add(item);
		arrFtrs.Add(ftrItem);
	}
}


void CDlgDoc::FindOverlapLines(CPFeature pFtr, CArray<OverlapLineItem,OverlapLineItem>& arrItemFtrs, BOOL bAutoSelectTarget, BOOL bCheckZ)
{
	CDlgDataSource* pDS = GetDlgDataSource();
	CDataQuery *pDQ = GetDataQuery();

	if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return;

	if( pFtr->GetGeometry()->GetDataPointSum()<2 )
		return;

	CArray<PT_3DEX,PT_3DEX> arrPts0,arrPts1;
	PT_3DEX pt0,pt1,pt2,pt3;
	Envelope e0,e1;
	OverlapSection section,section0;

	CArray<OverlapSection,OverlapSection> arrFtrSections,arrlineSections,arrlineSections0;

	pDS->SaveAllAppFlags();

	pFtr->GetGeometry()->GetShape(arrPts0);
	int len1 = GraphAPI::GGetAllLen2D(arrPts0.GetData(), arrPts0.GetSize());
	for (int i=0;i<arrPts0.GetSize()-1;i++)
	{
		arrlineSections.RemoveAll();
		pt0 = arrPts0[i];
		pt1 = arrPts0[i+1];
		if(!bCheckZ)
		{
			pt0.z=0; pt1.z=0;
		}

		//找到 pt1 所落在的地物
		CFtrArray arrFinds1, arrFinds2;

		if( i==0 )
		{
			e1.CreateFromPtAndRadius(pt0,GraphAPI::g_lfDisTolerance);
			if( pDQ->FindObjectInRect(e1,NULL,FALSE,FALSE)>1 )
			{
				int nFind = 0;
				const CPFeature *ftrs = pDQ->GetFoundHandles(nFind);
				arrFinds1.SetSize(nFind);
				memcpy(arrFinds1.GetData(),ftrs,sizeof(CPFeature)*nFind);
				arrFinds2.Append(arrFinds1);
			}
		}

		e1.CreateFromPtAndRadius(pt1,GraphAPI::g_lfDisTolerance);
		if( pDQ->FindObjectInRect(e1,NULL,FALSE,FALSE)>1 )
		{
			int nFind = 0;
			const CPFeature *ftrs = pDQ->GetFoundHandles(nFind);
			arrFinds1.SetSize(nFind);
			memcpy(arrFinds1.GetData(),ftrs,sizeof(CPFeature)*nFind);
			arrFinds2.Append(arrFinds1);
		}

		if( arrFinds2.GetSize()<=0 )
			continue;

		//去除重复
		int nFind = arrFinds2.GetSize();
		for( int m=0; m<arrFinds2.GetSize(); m++)
		{
			CPFeature ftr = arrFinds2[m];
			if( ftr->GetAppFlag()!=0 )
			{
				arrFinds2[m] = NULL;
			}
			else if( !ftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			{
				arrFinds2[m] = NULL;
			}
			else
			{
				arrFinds2[m]->SetAppFlag(1);
			}
		}
		for( m=arrFinds2.GetSize()-1; m>=0; m--)
		{
			if( arrFinds2[m]==NULL )
			{
				arrFinds2.RemoveAt(m);
			}
			else
			{
				arrFinds2[m]->SetAppFlag(0);
			}
		}

		//逐个计算重叠信息
		nFind = arrFinds2.GetSize();
		for (m=0;m<nFind;m++)
		{
			CPFeature pFtr0 = arrFinds2[m];
			if( pFtr==pFtr0 )
				continue;

			pFtr0->GetGeometry()->GetShape(arrPts1);
			int len2 = GraphAPI::GGetAllLen2D(arrPts1.GetData(), arrPts1.GetSize());
			for (int n=0;n<arrPts1.GetSize()-1;n++)
			{
				arrlineSections0.RemoveAll();
				pt2 = arrPts1[n];
				pt3 = arrPts1[n+1];
				if(!bCheckZ)
				{
					pt2.z=0; pt3.z=0;
				}
				
				OverlapLineItem item;
				if (bAutoSelectTarget && len1>len2)
				{
					if (Get2EndLineOverlapLine0(pt2.x, pt2.y, pt2.z, pt3.x, pt3.y, pt3.z, pt0.x, pt0.y, pt0.z, pt1.x, pt1.y, pt1.z, &section.x0, &section.y0, &section.z0, &section.x1, &section.y1, &section.z1, &section.t0, &section.t1))
					{
						item.pFtr = pFtr0;
						section.idx0 = section.idx1 = n;

						addOverlapSectionToItemArray(arrItemFtrs, pFtr0, section);
					}
				}
				else if (Get2EndLineOverlapLine0(pt0.x, pt0.y, pt0.z, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, pt3.x, pt3.y, pt3.z, &section.x0, &section.y0, &section.z0, &section.x1, &section.y1, &section.z1, &section.t0, &section.t1))
				{
					item.pFtr = pFtr;
					section.idx0 = section.idx1 = i;

					addOverlapSectionToItemArray(arrItemFtrs, pFtr, section);

					if (fabs(section.t0) < 1e-6&&fabs(section.t1 - 1.0) < 1e-6)
					{
						goto CurrentLineEnd;
					}
				}
			}
		}
CurrentLineEnd:	
		;
	}

	pDS->RestoreAllAppFlags();

	return;
}



void CDlgDoc::ProcessOverlapLineItems(CArray<OverlapLineItem,OverlapLineItem>& arrItemFtrs, CUndoFtrs& undo)
{
	CDlgDataSource* pDS = GetDlgDataSource();

	CArray<PT_3DEX,PT_3DEX> arrPts0, arrPts;

	//处理结果
	for (int k=0; k<arrItemFtrs.GetSize(); k++)
	{
		OverlapLineItem ftrItem = arrItemFtrs[k];
		
		CPFeature pFtr = ftrItem.pFtr;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		
		pFtr->GetGeometry()->GetShape(arrPts0);
		
		CArray<OverlapSection,OverlapSection> arrFtrSections;
		
		arrFtrSections.Copy(ftrItem.arry);

		OverlapSection section, section0;
		
		arrPts.RemoveAll();

		if (arrFtrSections.GetSize()>0)
		{
			section = arrFtrSections.GetAt(0);
			if(section.idx0==0&&fabs(section.t0)<1e-6)
			{
			}
			else
			{
				for (int i=0;i<=section.idx0;i++)
				{
					arrPts.Add(arrPts0[i]);
				}

				if (fabs(section.t0) > 1e-6)
				{
					arrPts.Add(PT_3DEX(section.x0,section.y0,section.z0,penLine));
				}
				
				CFeature *Temp = pFtr->Clone();
				if (Temp)
				{
					Temp->SetID(OUID());
					BOOL bCreateSucceed = Temp->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
					if(bCreateSucceed && AddObject(Temp,pLayer->GetID()))
					{
						undo.AddNewFeature(FtrToHandle(Temp));
						GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr,Temp);
					}
					else 
					{
						delete Temp;
					}
				}
			}
			
			for (int i=0;i<arrFtrSections.GetSize()-1;i++)
			{
				arrPts.RemoveAll();
				section = arrFtrSections.GetAt(i);
				section0 = arrFtrSections.GetAt(i+1);
				if (fabs(1-section.t1) > 1e-6)
				{
					arrPts.Add(PT_3DEX(section.x1,section.y1,section.z1,penLine));
				}
				
				for (int m=section.idx1+1;m<=section0.idx0;m++)
				{
					arrPts.Add(arrPts0[m]);
				}

				if (fabs(section0.t0) > 1e-6)
				{
					arrPts.Add(PT_3DEX(section0.x0,section0.y0,section0.z0,penLine));
				}
				int nPt = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(), arrPts.GetSize());
				arrPts.SetSize(nPt);
				
				CFeature *Temp = pFtr->Clone();
				if (Temp)
				{
					Temp->SetID(OUID());
					BOOL bCreateSucceed = Temp->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
					if(bCreateSucceed && AddObject(Temp,pLayer->GetID()))
					{
						undo.AddNewFeature(FtrToHandle(Temp));
						GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr,Temp);
					}
					else
					{
						delete Temp;
					}
				}
			}
			
			section = arrFtrSections.GetAt(arrFtrSections.GetSize()-1);
			if (section.idx1==arrPts0.GetSize()-2&&fabs(section.t1-1)<1e-6)
			{
				;
			}
			else
			{
				arrPts.RemoveAll();
				if (fabs(1-section.t1) > 1e-6)
				{
					arrPts.Add(PT_3DEX(section.x1,section.y1,section.z1,penLine));
				}
				
				for (int i=section.idx1+1;i<arrPts0.GetSize();i++)
				{
					arrPts.Add(arrPts0[i]);
				}
				CFeature *Temp = pFtr->Clone();
				if (Temp)
				{
					Temp->SetID(OUID());
					BOOL bCreateSucceed = Temp->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
					if(bCreateSucceed && AddObject(Temp,pLayer->GetID()))
					{
						undo.AddNewFeature(FtrToHandle(Temp));
						GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr,Temp);
					}
					else
					{
						delete Temp;
					}
				}
			}
			DeleteObject(FtrToHandle(pFtr));
			undo.AddOldFeature(FtrToHandle(pFtr));
		}
		
	}
}



void CDlgDoc::OnDelOverlapLines()
{
	CDlgDataSource* pDS = GetDlgDataSource();
	int nLayerCnt = pDS->GetFtrLayerCount();
	int nSum = 0;

	CPFeature pFtr;
	CFtrLayer *pLayer;
	for (int i=0;i<nLayerCnt;i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (pLayer==NULL||!pLayer->IsVisible()||pLayer->IsLocked())continue;
		int nObj = pLayer->GetObjectCount();		
		for (int j=0;j<nObj;j++)
		{
			pFtr = pLayer->GetObject(j);
			if (pFtr==NULL||!pFtr->IsVisible()||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))continue;
			nSum++;
		}
	}
	CArray<OverlapLineItem,OverlapLineItem> arrItemFtrs1, arrItemFtrs2;
	CFtrArray arrFtrs;
	
	CUndoFtrs undo(this,_T("Del_OverlapLines"));
	GProgressStart(nSum);
	for (i=0;i<nLayerCnt;i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (pLayer==NULL||!pLayer->IsVisible()||pLayer->IsLocked())continue;
		int nObj = pLayer->GetObjectCount();
		arrFtrs.RemoveAll();
		for (int j=0;j<nObj;j++)
		{
			pFtr = pLayer->GetObject(j);
			if (pFtr==NULL||!pFtr->IsVisible()||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))continue;
			arrFtrs.Add(pFtr);
		}
		int nFtrs = arrFtrs.GetSize();

		for (int k=0;k<nFtrs;k++)
		{
			GProgressStep();

			FindOverlapLines(arrFtrs[k],arrItemFtrs1,TRUE,FALSE);

			arrItemFtrs2.Append(arrItemFtrs1);
		}	
	}

	BeginBatchUpdate();

	ProcessOverlapLineItems(arrItemFtrs2,undo);
	undo.Commit();
	EndBatchUpdate();

	GProgressEnd();
	RefreshView();
}


void CDlgDoc::OnDelAllSameObjs()
{
	CDlgDeleteSame dlg;
	dlg.m_pDS = GetDlgDataSource();
	if(IDOK==dlg.DoModal())
	{
		CDelAllSameCommand cmd;
		cmd.Init(this);
		cmd.m_strLayers = dlg.m_strLayer;
		cmd.m_bCheckLayName = !dlg.m_bCrossLayer;
		cmd.m_nMode = dlg.m_bSameShape;
		cmd.Start();
	}
}


void CDlgDoc::OnDelOverlapPtInLines()
{
	CDlgDataSource* pDS = GetDlgDataSource();
	int nSum = pDS->GetEditableObjCount();
	CFtrLayer *pLayer = NULL;
	CFeature* pFtr = NULL,*pFtr0;
	double lfSamePtsInlineToler	= GetProfileDouble(REGPATH_QT,REGITEM_QT_SAMEPTINLINETOLER,0.1);
	lfSamePtsInlineToler = pDS->GetScale()*lfSamePtsInlineToler/1000;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	CUndoFtrs undo(this,_T("DelOverlapPtInLines"));
	GProgressStart(nSum);
	BeginBatchUpdate();
	int nNum=0;
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (pLayer==NULL||pLayer->IsLocked()||!pLayer->IsVisible())continue;		
		for (int j=0;j<pLayer->GetObjectCount();j++)
		{
			pFtr = pLayer->GetObject(j);
			if(pFtr==NULL)
				continue;
			GProgressStep();
			//if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))continue;
			if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))continue;
			pFtr->GetGeometry()->GetShape(arrPts);
			//CTempTolerance temp(lfSamePtsInlineToler);
			int nTemp = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());
			if(arrPts.GetSize()==nTemp)continue;
			arrPts.SetSize(nTemp);
			arrPts.FreeExtra();
			pFtr0 = pFtr->Clone();
			if(!pFtr0)continue;
			pFtr0->SetID(OUID());
			if(pFtr0->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize())&&AddObject(pFtr0,pLayer->GetID()))
			{
				nNum++;
				undo.AddNewFeature(FtrToHandle(pFtr0));
				GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr,pFtr0);
				if(DeleteObject(FtrToHandle(pFtr)))
					undo.AddOldFeature(FtrToHandle(pFtr));
			}
		}
	}
	undo.Commit();
	EndBatchUpdate();
	GProgressEnd();
	CString output;
	output.Format(StrFromResID(IDS_MODIFID_OBJSUM),nNum);
	AfxMessageBox(output);
	RefreshView();
}


namespace
{
	struct LinkOrderItem 
	{
		CFeature *pFtr;
		BOOL bFirstPt;
	};
}



void CDlgDoc::FindPseudoPoints(LPCTSTR layNames, double lfToler, BOOL bCheckZ, BOOL bCrossLay, CArray<PseudoPtItem,PseudoPtItem>& pts)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	CDataQuery *pDQ = GetDataQuery();

	//通过每个地物的端点找端点重叠的其他地物，结果存在arrRet中
	CArray<PseudoPtItem,PseudoPtItem> arrRet;

	BOOL bCheckLayName = (layNames!=NULL && strlen(layNames)>0);

	pDS->SaveAllAppFlags();

	const double c_minToler = lfToler;
	Envelope e;		
	CFeature *pFtr = NULL;	
	int nLayNum = pDS->GetFtrLayerCount();
	int nObj = 0;
	for (int i=0;i<nLayNum;i++)
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
		if (pLayer==NULL||pLayer->IsLocked()||!pLayer->IsVisible())continue;

		if( StrFromResID(IDS_MARKLAYER_NAME).CompareNoCase(pLayer->GetName())==0 )
			continue;

		if( bCheckLayName && !CheckNameForLayerCode(pDS,pLayer->GetName(),layNames) )
		{
			continue;
		}

		nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			pFtr = pLayer->GetObject(j);
			if(pFtr==NULL||!pFtr->IsVisible()||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))continue;

			if( ((CGeoCurveBase*)(pFtr->GetGeometry()))->IsClosed() )
			{
				continue;
			}

			CArray<PT_3DEX,PT_3DEX> arrPts;
			pFtr->GetGeometry()->GetShape(arrPts);
			if( arrPts.GetSize()<2 )
				continue;

			PT_3DEX pts1[2];

			pts1[0] = arrPts[0];
			pts1[1] = arrPts[arrPts.GetSize()-1];

			CArray<PseudoPtItem,PseudoPtItem> arrStart, arrEnd;

			for( int k=0; k<2; k++)
			{				
				//AppFlag的0位为1，表示起点已经计算过，1位为1，表示尾点已经计算过
				if( pFtr->GetAppFlagBit(k) )
					continue;

				e.CreateFromPtAndRadius(pts1[k],c_minToler);
				pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);

				int nfind;
				const CPFeature *ftrs = pDQ->GetFoundHandles(nfind);
				if( nfind<2 )
					continue;

				for( int m=0; m<nfind; m++)
				{
					if( ftrs[m]==pFtr )
						continue;	
					
					CGeometry *pGeo = ftrs[m]->GetGeometry();

					if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
						continue;

					//闭合地物跳过
					if( ((CGeoCurveBase*)pGeo)->IsClosed() )
						continue;

					if( !bCrossLay && stricmp(pLayer->GetName(),pDS->GetFtrLayerOfObject(ftrs[m])->GetName())!=0 )
						continue;

					CArray<PT_3DEX,PT_3DEX> arrPts2;
					pGeo->GetShape(arrPts2);
					if( arrPts2.GetSize()<2 )
						continue;

					PT_3DEX pts2[2];
					pts2[0] = arrPts2[0];
					pts2[1] = arrPts2[arrPts2.GetSize()-1];

					PseudoPtItem item;
					item.idx1 = -1;
					if( !bCheckZ )
					{
						if( fabs(pts1[k].x-pts2[0].x)<c_minToler && fabs(pts1[k].y-pts2[0].y)<c_minToler )
						{
							item.idx1 = 0;
						}
						else if( fabs(pts1[k].x-pts2[1].x)<c_minToler && fabs(pts1[k].y-pts2[1].y)<c_minToler )
						{
							item.idx1 = arrPts2.GetSize()-1;
						}
					}
					else
					{
						if( fabs(pts1[k].x-pts2[0].x)<c_minToler && fabs(pts1[k].y-pts2[0].y)<c_minToler && fabs(pts1[k].z-pts2[0].z)<c_minToler )
						{
							item.idx1 = 0;
						}
						else if( fabs(pts1[k].x-pts2[1].x)<c_minToler && fabs(pts1[k].y-pts2[1].y)<c_minToler && fabs(pts1[k].z-pts2[0].z)<c_minToler )
						{
							item.idx1 = arrPts2.GetSize()-1;
						}
					}

					if( item.idx1>=0 )
					{
						pFtr->SetAppFlagBit(k,1);
						
						if( item.idx1==0 )
							ftrs[m]->SetAppFlagBit(0,1);
						else
							ftrs[m]->SetAppFlagBit(1,1);

						if(k==0)
						{
							item.pt = pts1[0];
							item.pFtr0 = ftrs[m];
							item.pFtr1 = pFtr;
							item.idx0 = item.idx1;
							item.idx1 = 0;
							arrStart.Add(item);
						}
						else
						{
							item.pt = pts1[1];
							item.pFtr0 = pFtr;
							item.pFtr1 = ftrs[m];
							item.idx0 = arrPts.GetSize()-1;
							//item.idx1 = item.idx1;
							arrEnd.Add(item);
						}	
					}
				}
			}

			//重叠点太多，不算伪节点
			if( arrStart.GetSize()==1 )
				arrRet.Append(arrStart);

			if( arrEnd.GetSize()==1 )
				arrRet.Append(arrEnd);
		}			
	}

	pts.Copy(arrRet);

	pDS->RestoreAllAppFlags();
}


void CDlgDoc::FindPseudoPointOne(PT_3D pt, LPCTSTR layNames, double lfToler, BOOL bCheckZ, BOOL bCrossLay, CArray<PseudoPtItem,PseudoPtItem>& arrRet)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	CDataQuery *pDQ = GetDataQuery();
	
	CFtrLayerArray layers0, layers1;
	pDS->GetFtrLayersByNameOrCode(layNames,layers1);
	
	if( CString(layNames).IsEmpty() )
	{
		pDS->SaveAllQueryFlags(TRUE,TRUE);
	}
	else if( layers1.GetSize()<=0 )
	{
		return;
	}
	else
	{
		pDS->SaveAllQueryFlags(TRUE,FALSE);
		for( int i=0; i<layers1.GetSize(); i++)
		{
			layers1[i]->SetAllowQuery(TRUE);
		}
	}
	
	//通过每个地物的端点找端点重叠的其他地物，结果存在arrRet中
	
	const double c_minToler = lfToler;
	Envelope e;		
	
	CArray<PseudoPtItem,PseudoPtItem> arrStart;

	e.CreateFromPtAndRadius(pt,c_minToler);
	pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);

	pDS->RestoreAllQueryFlags();
	
	int nfind, i;
	const CPFeature *ftrs = pDQ->GetFoundHandles(nfind);
	if( nfind<2 )
		return;
	
	int index = -1;
	for( i=0; i<nfind; i++)
	{
		int id = GetFtrLayerIDOfFtr(FtrToHandle(ftrs[i]));
		if(!bCrossLay)
		{
			for(int j=i+1; j<nfind; j++)
			{
				int id1 = GetFtrLayerIDOfFtr(FtrToHandle(ftrs[j]));
				if(id==id1) break;
			}
			if(j>=nfind) continue;
		}

		CGeometry *pGeo = ftrs[i]->GetGeometry();
		if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			continue;
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo->GetShape(arrPts);
		
		if( arrPts.GetSize()<2 )
			continue;
		
		int npt = arrPts.GetSize();
		if( GraphAPI::GGet2DDisOf2P(pt,(PT_3D)arrPts[0])<c_minToler )
		{
			index = 0;
			break;
		}
		
		if( GraphAPI::GGet2DDisOf2P(pt,(PT_3D)arrPts[npt-1])<c_minToler )
		{
			index = npt-1;
			break;
		}
	}
	
	if( i>=nfind )
		return;
	
	for( int m=0; m<nfind; m++)
	{					
		if( m==i )
			continue;
		
		CGeometry *pGeo = ftrs[m]->GetGeometry();
		
		if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			continue;
		
		//闭合地物跳过
		if( ((CGeoCurveBase*)pGeo)->IsClosed() )
			continue;
		
		if( !bCrossLay && 
			stricmp(pDS->GetFtrLayerOfObject(ftrs[i])->GetName(),pDS->GetFtrLayerOfObject(ftrs[m])->GetName())!=0 )
			continue;
		
		CArray<PT_3DEX,PT_3DEX> arrPts2;
		pGeo->GetShape(arrPts2);
		if( arrPts2.GetSize()<2 )
			continue;
		
		PT_3DEX pts2[2];
		pts2[0] = arrPts2[0];
		pts2[1] = arrPts2[arrPts2.GetSize()-1];
		
		PseudoPtItem item;
		item.idx1 = -1;
		if( !bCheckZ )
		{
			if( fabs(pt.x-pts2[0].x)<c_minToler && fabs(pt.y-pts2[0].y)<c_minToler )
			{
				item.idx1 = 0;
			}
			else if( fabs(pt.x-pts2[1].x)<c_minToler && fabs(pt.y-pts2[1].y)<c_minToler )
			{
				item.idx1 = arrPts2.GetSize()-1;
			}
		}
		else
		{
			if( fabs(pt.x-pts2[0].x)<c_minToler && fabs(pt.y-pts2[0].y)<c_minToler && fabs(pt.z-pts2[0].z)<c_minToler )
			{
				item.idx1 = 0;
			}
			else if( fabs(pt.x-pts2[1].x)<c_minToler && fabs(pt.y-pts2[1].y)<c_minToler && fabs(pt.z-pts2[1].z)<c_minToler )
			{
				item.idx1 = arrPts2.GetSize()-1;
			}
		}
		
		if( item.idx1>=0 )
		{
			if(index==0)
			{
				item.pt = pt;
				item.pFtr0 = ftrs[m];
				item.pFtr1 = ftrs[i];
				item.idx0 = item.idx1;
				item.idx1 = 0;
			}
			else
			{
				item.pt = pt;
				item.pFtr0 = ftrs[i];
				item.pFtr1 = ftrs[m];
				item.idx0 = index;
				//item.idx1 = item.idx1;
			}
			arrStart.Add(item);
		}
	}
	
	//重叠点太多，不算伪节点
	if( arrStart.GetSize()==1 )
		arrRet.Append(arrStart);
}


void CDlgDoc::OnCheckPseudoNode()
{
	CArray<PseudoPtItem,PseudoPtItem> arrRet;

	CDlgCheckPseudoPoints dlg;
	dlg.m_pDS = GetDlgDataSource();

	if( dlg.DoModal()!=IDOK )
		return;

	double fToler = GraphAPI::g_lfDisTolerance;
	BOOL bCheckZ = dlg.m_bCheckZ;
	BOOL bCrossLay = dlg.m_bCrossLay;
	FindPseudoPoints(dlg.m_strLayNames,fToler,bCheckZ,bCrossLay,arrRet);

	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,1,0);

	CChkResManger *pChkRes = &GetChkResMgr();

	CString chkName = StrFromResID(IDS_PSEUDONODE);
	CString chkReason = StrFromResID(IDS_PSEUDONODE);

	GrBuffer pseBuf;
	for (int j= 0;j<arrRet.GetSize();j++)
	{
		pChkRes->BeginResItem(chkName);
		pChkRes->AddAssociatedFtr(arrRet[j].pFtr0);
		pChkRes->AddAssociatedFtr(arrRet[j].pFtr1);
		pChkRes->SetAssociatedPos(arrRet[j].pt);
		pChkRes->SetReason(chkReason);
		pChkRes->EndResItem();
	}

	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);

}


void DeletePseudoPoints(CDlgDoc *pDoc, CArray<PseudoPtItem,PseudoPtItem>& arrRet, BOOL bProgress)
{
	PseudoPtItem start;
	PseudoPtItem *pos = arrRet.GetData();
	PseudoPtItem *pos0 = pos;
	int nSz = arrRet.GetSize();
	CArray<LinkOrderItem,LinkOrderItem> linkOrder;
	LinkOrderItem link;
	CFeature *pStart;
	CArray<PT_3DEX,PT_3DEX> arrPts,arrPts0;

	CUndoFtrs undo(pDoc,_T("DelPseudopoint"));
	pDoc->BeginBatchUpdate();
	int nRet = arrRet.GetSize();
	if( bProgress )GProgressStart(nRet);

	BeginCheck41License

	while (arrRet.GetSize()>0)
	{
		if( bProgress )GProgressStep();

		start = arrRet[0];
		linkOrder.RemoveAll();
		link.pFtr = start.pFtr0;
		if (start.idx0==0)
		{
			link.bFirstPt = FALSE;
		}
		else
			link.bFirstPt = TRUE;
		linkOrder.Add(link);
		link.pFtr = start.pFtr1;
		if (start.idx1==0)
		{
			link.bFirstPt = TRUE;
		}
		else
			link.bFirstPt = FALSE;
		linkOrder.Add(link);
		arrRet.RemoveAt(0);
		if(arrRet.GetSize()>0)
		{
			//从伪节点的第一个关联线开始，往前连接其他线
			pStart = start.pFtr0;
			while (1)
			{
				int i;
				for (i=0;i<arrRet.GetSize();i++)
				{
					if (pStart==arrRet[i].pFtr0)
					{
						if(arrRet[i].pFtr1 == start.pFtr1)
							break;

						pStart = link.pFtr = arrRet[i].pFtr1;
						if (arrRet[i].idx1==0)
						{
							link.bFirstPt = FALSE;
						}
						else
							link.bFirstPt = TRUE;
						linkOrder.InsertAt(0,link);												
						break;
					}
					if (pStart==arrRet[i].pFtr1)
					{						
						if(arrRet[i].pFtr0 == start.pFtr1)
							break;

						pStart = link.pFtr = arrRet[i].pFtr0;
						if (arrRet[i].idx0==0)
						{
							link.bFirstPt = FALSE;
						}
						else
							link.bFirstPt = TRUE;
						linkOrder.InsertAt(0,link);												
						break;
					}					
				}
				if (i<arrRet.GetSize())
				{
					arrRet.RemoveAt(i);
				}
				else
					break;
				if (arrRet.GetSize()==0)
				{
					break;
				}
			}
		}
		if(arrRet.GetSize()>0)
		{
			//伪节点的第二个关联线开始，往后连接其他线
			pStart = start.pFtr1;
			while (1)
			{
				int i;
				for (i=0;i<arrRet.GetSize();i++)
				{
					if (pStart==arrRet[i].pFtr0)
					{
						if(arrRet[i].pFtr1 == start.pFtr0)
							break;
						pStart = link.pFtr = arrRet[i].pFtr1;
						if (arrRet[i].idx1==0)
						{
							link.bFirstPt = TRUE;
						}
						else
							link.bFirstPt = FALSE;
						linkOrder.Add(link);												
						break;
					}
					if (pStart==arrRet[i].pFtr1)
					{
						if(arrRet[i].pFtr0 == start.pFtr0)
							break;
						pStart = link.pFtr = arrRet[i].pFtr0;
						if (arrRet[i].idx0==0)
						{
							link.bFirstPt = TRUE;
						}
						else
							link.bFirstPt = FALSE;
						linkOrder.Add(link);												
						break;
					}					
				}
				if (i<arrRet.GetSize())
				{
					arrRet.RemoveAt(i);
				}
				else
					break;
				if (arrRet.GetSize()==0)
				{
					break;
				}
			}
		}

		//连接处理		
		CFeature *pNew = linkOrder[0].pFtr->Clone();
		if(!pNew)continue;
		pNew->SetID(OUID());
		int nLayID = pDoc->GetFtrLayerIDOfFtr(FtrToHandle(linkOrder[0].pFtr));
		arrPts.RemoveAll();
		for (int i=0;i<linkOrder.GetSize();i++)
		{
			linkOrder[i].pFtr->GetGeometry()->GetShape(arrPts0);
			if( linkOrder[i].bFirstPt )
			{
				if(i!=0)arrPts0.RemoveAt(0);
				arrPts.Append(arrPts0);
			}
			else
			{
				int temp = 2;
				if(i==0)temp = 1;
				for (int j=arrPts0.GetSize()-temp;j>=0;j--)
				{
					arrPts.Add(arrPts0[j]);
				}
			}
			pDoc->DeleteObject(FtrToHandle(linkOrder[i].pFtr));
			undo.AddOldFeature(FtrToHandle(linkOrder[i].pFtr));
		}
		if(pNew->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize())&&pDoc->AddObject(pNew,nLayID))
		{
			undo.AddNewFeature(FtrToHandle(pNew));
		}
		else
			delete pNew;
	}

	EndCheck41License

	pDoc->EndBatchUpdate();
	undo.Commit();

	if( bProgress )GProgressEnd();

	pDoc->RefreshView();	
}


//从检查结果中提取伪节点数据
BOOL CDlgDoc::RestorePseudoPtItemFromPt(PT_3D pt, PseudoPtItem& item, BOOL bCheckZ)
{
	BOOL bCrossLay = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTCROSSLAY,TRUE);
	CString strLayNames  = AfxGetApp()->GetProfileString(REGPATH_QT,REGITEM_QT_PSEPTLAYS,NULL);
	CArray<PseudoPtItem,PseudoPtItem> arrRet;

	FindPseudoPointOne(pt,strLayNames,GraphAPI::g_lfDisTolerance,bCheckZ,bCrossLay,arrRet);

	if( arrRet.GetSize()!=1 )
		return FALSE;

	item = arrRet[0];

	return TRUE;
}


void CDlgDoc::OnDelOnePseudoPoint()
{
	BOOL bCheckZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTZ,TRUE);

	ChkResultItem *pItem = ((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.GetCurItem();
	if( pItem==NULL )
	{
		AfxMessageBox(IDS_CHK_NOPSEUDOPOINTS);
		return;
	}
	
	CString chkName = StrFromResID(IDS_PSEUDONODE);
	if( pItem->reason.CompareNoCase(chkName)!=0 || pItem->state!=0 || pItem->arrFtrs.GetSize()<=0 )
	{
		AfxMessageBox(IDS_CHK_NOPSEUDOPOINTS);
		return;
	}

	CDataQuery *pDQ = GetDataQuery();
	for( int i=0; i<pItem->arrFtrs.GetSize(); i++)
	{
		if( !pDQ->IsManagedObj(pItem->arrFtrs[i]) )
			break;
	}

	if( i<pItem->arrFtrs.GetSize() )
	{
		AfxMessageBox(IDS_CHK_NOPSEUDOPOINTS);
		return;
	}

	CArray<PseudoPtItem,PseudoPtItem> arrRets;
	PseudoPtItem item;
	if( !RestorePseudoPtItemFromPt(pItem->pos,item,bCheckZ) )
	{
		AfxMessageBox(IDS_CHK_NOPSEUDOPOINTS);
		return;
	}

	((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.SetItemState(pItem,2);
	
	arrRets.Add(item);

	DeletePseudoPoints(this,arrRets,FALSE);
}


void CDlgDoc::OnDelAllPseudoPoints()
{
	BOOL bCheckZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTZ,TRUE);

	CArray<PseudoPtItem,PseudoPtItem> arrRets;
	CChkResManger *pChkRes = &GetChkResMgr();		
	
	CString chkName = StrFromResID(IDS_PSEUDONODE);
	CArray<CFeature*,CFeature*> arrFtrs;
	CDataQuery *pDQ = GetDataQuery();
	
	for( int i=0; i<pChkRes->GetChkResCount(); i++ )
	{
		ChkResultItem *pItem = pChkRes->GetChkResByIdx(i);
		if( pItem->reason.CompareNoCase(chkName)!=0 )
			continue;
		
		if( pItem->state!=0 )
			continue;

		for( int j=0; j<pItem->arrFtrs.GetSize(); j++)
		{
			if( !pDQ->IsManagedObj(pItem->arrFtrs[j]) )
				break;
		}
		
		if( j<pItem->arrFtrs.GetSize() )
		{
			continue;
		}

		PseudoPtItem item;
		if( !RestorePseudoPtItemFromPt(pItem->pos,item,bCheckZ) )
			continue;

		((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.SetItemState(pItem,2);
		
		arrRets.Add(item);
	}
	
	if( arrRets.GetSize()<=0 )
	{
		AfxMessageBox(IDS_CHK_NOPSEUDOPOINTS);
		return;
	}
	
	DeletePseudoPoints(this,arrRets,TRUE);
}

void CDlgDoc::OnDelDirPseudoPoints()
{
	BOOL bCheckZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTZ,TRUE);
	
	CArray<PseudoPtItem,PseudoPtItem> arrRets;
	CChkResManger *pChkRes = &GetChkResMgr();		
	
	CString chkName = StrFromResID(IDS_PSEUDONODE);
	CArray<CFeature*,CFeature*> arrFtrs;
	CDataQuery *pDQ = GetDataQuery();
	
	for( int i=0; i<pChkRes->GetChkResCount(); i++ )
	{
		ChkResultItem *pItem = pChkRes->GetChkResByIdx(i);
		if( pItem->reason.CompareNoCase(chkName)!=0 )
			continue;
		
		if( pItem->state!=0 )
			continue;
		
		for( int j=0; j<pItem->arrFtrs.GetSize(); j++)
		{
			if( !pDQ->IsManagedObj(pItem->arrFtrs[j]) )
				break;
		}
		
		if( j<pItem->arrFtrs.GetSize() )
		{
			continue;
		}

		//方向一致性
		if(j!=2) continue;
		CGeometry *pGeo1 = pItem->arrFtrs[0]->GetGeometry();
		CGeometry *pGeo2 = pItem->arrFtrs[1]->GetGeometry();
		if(!pGeo1 || !pGeo2) continue;
		CArray<PT_3DEX, PT_3DEX> pts1, pts2;
		pGeo1->GetShape(pts1);
		pGeo2->GetShape(pts2);
		int npt1=pts1.GetSize();
		int npt2=pts2.GetSize();
		if(npt1<2 || npt2<2) continue;
		if(!GraphAPI::GIsEqual2DPoint(&pts1[npt1-1], &pts2[0]))
			continue;
		
		PseudoPtItem item;
		if( !RestorePseudoPtItemFromPt(pItem->pos,item,bCheckZ) )
			continue;
		
		((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.SetItemState(pItem,2);
		
		arrRets.Add(item);
	}
	
	if( arrRets.GetSize()<=0 )
	{
		AfxMessageBox(IDS_CHK_NOPSEUDOPOINTS);
		return;
	}
	
	DeletePseudoPoints(this,arrRets,TRUE);
}

extern double GetDistanceFromPointToCurve(CArray<PT_3DEX,PT_3DEX>& pts, PT_3D& pt, PT_3D *retpt=NULL);
void CDlgDoc::OnCheckSuspendPoints()
{
	CDlgDataSource *pDS = GetDlgDataSource();

	CDlgCheckSuspendPoints dlg;
	dlg.m_pDS = pDS;
	if( dlg.DoModal()!=IDOK )
		return;

	BOOL bZ = dlg.m_bCheckZ;
	BOOL bVep = dlg.m_bCheckVEP;
	BOOL bCrossLay = dlg.m_bCrossLay;
	BOOL bBound = dlg.m_bCheckBound;
	double lfSusPntRadius = GraphAPI::g_lfDisTolerance;
	double lfRange = dlg.m_lfRange;
	BOOL bCheckRange = dlg.m_bCheckRange;

	int nSusMode = 0;
		
	GOutPut(StrFromResID(IDS_TIP_START));
	
	Envelope e;		
	CFeature *pFtr = NULL;	
	int nLayNum = pDS->GetFtrLayerCount();
	int nObj, nCount = pDS->GetEditableObjCount();

	CFtrArray arrRetFtrs;
	CArray<PT_3D,PT_3D> arrRetPts;

	CDataQuery* pQuery = GetDataQuery();

	BOOL bCheckLayName = (dlg.m_strLayNames.GetLength()>0);

	pDS->SaveAllQueryFlags();
	pDS->SetFtrLayerQueryFlag(StrFromResID(IDS_MARKLAYER_NAME),FALSE);

	GOutPut(StrFromResID(IDS_TIP_SEARCHSUS));
	GProgressStart(nCount);

	int i,j,k;
	for (i=0;i<nLayNum;i++)
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
		if (pLayer==NULL||pLayer->IsLocked()||!pLayer->IsVisible())continue;
		if( StrFromResID(IDS_MARKLAYER_NAME).CompareNoCase(pLayer->GetName())==0 )
			continue;

		if( bCheckLayName && !CheckNameForLayerCode(pDS,pLayer->GetName(),dlg.m_strLayNames) )
			continue;

		nObj = pLayer->GetObjectCount();
		for (j=0;j<nObj;j++)
		{
			GProgressStep();

			pFtr = pLayer->GetObject(j);
			if( pFtr==NULL ||!pFtr->IsVisible() )
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;
			if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
				continue;

			CArray<PT_3DEX,PT_3DEX> arrPts, pts;
			if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
				((CGeoParallel*)pGeo)->GetAllShape(arrPts);
			else
				pGeo->GetShape(arrPts);
			int nSz = arrPts.GetSize();

			for(k=0; k<nSz; k++)
			{
				if(k==0)
					pts.Add(arrPts[0]);
				else if(k==nSz-1)
					pts.Add(arrPts[k]);
				else if(arrPts[k+1].pencode==penMove)
				{
					pts.Add(arrPts[k]);
					pts.Add(arrPts[k+1]);
				}
			}
			int npt = pts.GetSize();

			if( npt==2 && GraphAPI::GGet2DDisOf2P(pts[0],pts[1])<lfSusPntRadius )
			{
				if( bZ )
				{
					//
					if( fabs(pts[0].z-pts[1].z)<GraphAPI::g_lfZTolerance )
						continue;
					//
					arrRetFtrs.Add(pFtr);
					arrRetPts.Add(pts[0]);
				}
				continue;
			}

			for( k=0; k<npt; k++)
			{
				e.CreateFromPtAndRadius(pts[k],lfSusPntRadius);
				if( pQuery->FindObjectInRect(e,NULL,FALSE,FALSE)>1 )
				{
					int nNum = 0;
					const CPFeature *ftr = pQuery->GetFoundHandles(nNum);
					for (int m=0;m<nNum;m++)
					{
						if( ftr[m]==pFtr ) continue;
						if( !ftr[m]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
							continue;

						if( !bCrossLay )
						{
							if( stricmp(pDS->GetFtrLayerOfObject(ftr[m])->GetName(),pDS->GetFtrLayerOfObject(pFtr)->GetName())!=0 )
							{
								continue;
							}
						}

						CGeometry *pGeo2 = ftr[m]->GetGeometry();
						CArray<PT_3DEX,PT_3DEX> arrPts2;
						pGeo2->GetShape(arrPts2);

						PT_KEYCTRL keypt = pGeo2->FindNearestKeyCtrlPt(pts[k],lfSusPntRadius,NULL,1);

						PT_3DEX expt(0,0,0,penNone);

						//端点和顶点
						if( nSusMode==0 )
						{
							if( keypt.index>=0 )
								expt = arrPts2[keypt.index];
						}
						else
						{
							if( keypt.index==0 || keypt.index==(arrPts2.GetSize()-1) )
								expt = arrPts2[keypt.index];
						}

						//虚顶点不作为悬挂点
						if( bVep && expt.type==penNone )
						{
							if( pGeo2->FindNearestBasePt(pts[k],e,NULL,&expt,NULL) )
							{
								expt.pencode = penLine;
							}
						}

						if( expt.pencode!=penNone )
						{
							if( bZ )
							{
								if(fabs(pts[k].z-expt.z)<GraphAPI::g_lfZTolerance)
								{
									break;
								}
							}
							else
							{
								break;
							}
						}

						//平行线的辅助线单独处理
						if(pGeo2->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
						{
							CGeometry *pObj1=NULL;
							CGeometry *pObj2=NULL;
							if( ((CGeoParallel*)pGeo2)->Separate(pObj1,pObj2) )
							{
								if(pObj1) delete pObj1;
								if(pObj2)
								{
									pGeo2->GetShape(arrPts2);
									
									PT_KEYCTRL keypt = pObj2->FindNearestKeyCtrlPt(pts[k],lfSusPntRadius,NULL,1);
									
									PT_3DEX expt(0,0,0,penNone);
									
									//端点和顶点
									if( nSusMode==0 )
									{
										if( keypt.index>=0 )
											expt = arrPts2[keypt.index];
									}
									else
									{
										if( keypt.index==0 || keypt.index==(arrPts2.GetSize()-1) )
											expt = arrPts2[keypt.index];
									}
									
									//虚顶点不作为悬挂点
									if( bVep && expt.type==penNone )
									{
										if( pObj2->FindNearestBasePt(pts[k],e,NULL,&expt,NULL) )
										{
											expt.pencode = penLine;
										}
									}
									
									if( expt.pencode!=penNone )
									{
										if( bZ )
										{
											if(fabs(pts[k].z-expt.z)<GraphAPI::g_lfZTolerance)
											{
												delete pObj2;
												break;
											}
										}
										else
										{
											delete pObj2;
											break;
										}
									}
									delete pObj2;
								}
							}
						}
						//咬合到图廓层不作为悬挂
						if(bBound)
						{
							if(pDS->GetFtrLayerOfObject(ftr[m]) == pDS->GetFtrLayer(dlg.m_strBoundLayer) )
							{
								break;
							}
						}
					}

					if( m<nNum )
					{
						continue;
					}
				}

				if(bCheckRange)
				{
					e.CreateFromPtAndRadius(pts[k],lfRange);
					double mindis = 10e9;
					if(pQuery->FindObjectInRect(e,NULL,FALSE,FALSE)>1 )
					{
						int nNum = 0;
						const CPFeature *ftr = pQuery->GetFoundHandles(nNum);
						for (int m=0;m<nNum;m++)
						{
							if( ftr[m]==pFtr ) continue;
							if( !ftr[m]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
								continue;
								
							CGeometry *pGeo2 = ftr[m]->GetGeometry();
							CArray<PT_3DEX,PT_3DEX> arrPts2;
							pGeo2->GetShape(arrPts2);
							double dis = GetDistanceFromPointToCurve(arrPts2,pts[k]);
							if(dis<mindis)
								mindis = dis;
							if(mindis<lfRange)
								break;
						}
					}
					if(mindis>lfRange)
					{
						continue;
					}
				}
				arrRetFtrs.Add(pFtr);
				arrRetPts.Add(pts[k]);
			}	
		}
	}

	GProgressEnd();
	
	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,1,0);

	pDS->RestoreAllQueryFlags();
	
	CChkResManger *pChkRes = &GetChkResMgr();		
	
	CString chkName = StrFromResID(IDS_SELCOND_SUSPENDPOINT);
	CString chkReason = StrFromResID(IDS_SELCOND_SUSPENDPOINT);
	
	for (j=0;j<arrRetFtrs.GetSize();j++)
	{
		pChkRes->BeginResItem(chkName);
		pChkRes->AddAssociatedFtr(arrRetFtrs[j]);
		pChkRes->SetAssociatedPos(arrRetPts[j]);
		pChkRes->SetReason(chkReason);
		pChkRes->EndResItem();
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);
	GOutPut(StrFromResID(IDS_TIP_END));
}


int CDlgDoc::KillSuspendOne(CFeature *pFtr, PT_3D pt, double lfSusToler, double lfTolerZ, BOOL bCrossLay, BOOL bModifyZ, BOOL bVep, BOOL bInsertPoint, CUndoFtrs& undo, CFtrArray *pArrReplacedFtrs, BOOL bAllowVSuspend)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	CDataQuery* pDQ = GetDataQuery();
	CCoordSys *pCS = GetCoordWnd().m_pSearchCS;
	CArray<PT_3DEX,PT_3DEX> arrPts, arrPtsKey;
	PT_3DEX targetPt,tempPt;
	PT_3D line[2];

	if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return 1;

	CString strLayName;
	if(!bCrossLay)strLayName = pDS->GetFtrLayerOfObject(pFtr)->GetName();
	((CGeoCurveBase*)pFtr->GetGeometry())->GetShape()->GetPts(arrPts);
	pFtr->GetGeometry()->GetShape(arrPtsKey);

	int npt = arrPts.GetSize();

	if( arrPts.GetSize()<2 )
		return 1;

	CArray<CPFeature,CPFeature> arrFoundHandles;

	double c_minToler = GraphAPI::g_lfDisTolerance;
	int cur_idx = 0;

	int bProcessed = 3;

	BeginCheck41License

	//处理地物闭合的情况
	if( GraphAPI::GGet2DDisOf2P(arrPts[0],arrPts[npt-1])<lfSusToler )
	{
		//正确闭合，无需处理
		if( GraphAPI::GGet3DDisOf2P(arrPts[0],arrPts[npt-1])<c_minToler )
			return 2;

		arrPts[npt-1].x = arrPts[0].x;
		arrPts[npt-1].y = arrPts[0].y;

		if( fabs(arrPts[0].z-arrPts[npt-1].z)<lfTolerZ )
		{
			arrPts[npt-1].z = arrPts[0].z;
		}

		CFeature *pNew = pFtr->Clone();
		if (pNew)
		{
			pNew->SetID(OUID());
			if(pNew->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize())&&AddObject(pNew,GetFtrLayerIDOfFtr(FtrToHandle(pFtr))))
			{								
				undo.AddNewFeature(FtrToHandle(pNew));
				GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr,pNew);
				if(DeleteObject(FtrToHandle(pFtr)))
				{
					if( pArrReplacedFtrs )
					{
						pArrReplacedFtrs->Add(pFtr);
						pArrReplacedFtrs->Add(pNew);
					}
					undo.AddOldFeature(FtrToHandle(pFtr));	
				}
			}
		}

		return 0;
	}

	{
		int i = 0;
		if( GraphAPI::GGet2DDisOf2P((PT_3D)arrPts[0],pt)<1e-6 )
			i = 0;
		else
			i = 1;

		//首点
		if( i==0 )
		{
			targetPt = arrPts[0];
			double dis = GraphAPI::GGet2DDisOf2P(targetPt,arrPts[1]);
			double k =  (dis<1e-6?0:(lfSusToler/dis));
			line[0].x = targetPt.x+(arrPts[1].x-targetPt.x)*k;
			line[0].y = targetPt.y+(arrPts[1].y-targetPt.y)*k;
			line[1].x = targetPt.x+targetPt.x-line[0].x;
			line[1].y = targetPt.y+targetPt.y-line[0].y;
			cur_idx = 0;
		}
		//尾点
		else
		{
			targetPt = arrPts[npt-1];
			double dis = GraphAPI::GGet2DDisOf2P(targetPt,arrPts[npt-2]);
			double k =  (dis<1e-6?0:(lfSusToler/dis));
			line[0].x = targetPt.x+(arrPts[npt-2].x-targetPt.x)*k;
			line[0].y = targetPt.y+(arrPts[npt-2].y-targetPt.y)*k;
			line[1].x = targetPt.x+targetPt.x-line[0].x;
			line[1].y = targetPt.y+targetPt.y-line[0].y;
			cur_idx = arrPtsKey.GetSize()-1;
		}

		Envelope e;
		e.CreateFromPtAndRadius(targetPt,lfSusToler);

		BOOL bProcess = FALSE;
		if( pDQ->FindObjectInRect(e,NULL,FALSE,FALSE)>1 )
		{
			int nNum = 0;
			const CPFeature *ftr = pDQ->GetFoundHandles(nNum);
			arrFoundHandles.SetSize(nNum);
			memcpy(arrFoundHandles.GetData(),ftr,nNum*sizeof(CPFeature));
			ftr = arrFoundHandles.GetData();
			if (nNum<=0)
				return 3;

			//判断是不是悬点：第一步，是否有顶点重合
			int j=0;
// 			for (j=0;j<nNum;j++)
// 			{	
// 				if( pFtr==ftr[j] )
// 					continue;
// 
// 				if (!ftr[j]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
// 				{
// 					continue;
// 				}
// 				if(!bCrossLay&&strLayName.CompareNoCase(pDS->GetFtrLayerOfObject(ftr[j])->GetName())!=0)
// 					continue;
// 
// 				CArray<PT_3DEX,PT_3DEX> arrPts2;
// 				ftr[j]->GetGeometry()->GetShape(arrPts2);
// 
// 				if( arrPts2.GetSize()<=1 )
// 					continue;
// 
// 				for( int k=0; k<arrPts2.GetSize(); k++)
// 				{
// 					if( (bModifyZ&&GraphAPI::GIsEqual3DPoint(&(arrPts2[k]),&targetPt)) ||
// 						(!bModifyZ&&GraphAPI::GIsEqual2DPoint(&(arrPts2[k]),&targetPt)) )
// 						break;
// 				}
// 				if( k<arrPts2.GetSize() )
// 					break;
// 			}
// 			if (j<nNum)return 2;

			//判断是不是悬点：第二步，是否有虚顶点重合
// 			if (bVep)
// 			{
// 				Envelope e2;
// 				e2.CreateFromPtAndRadius(targetPt,c_minToler);
// 
// 				int nNum0;
// 				if( pDQ->FindObjectInRect(e2,NULL,FALSE,FALSE)>1 )
// 				{
// 					const CPFeature *ftr0 = pDQ->GetFoundHandles(nNum0);
// 					PT_3D RetPt;
// 
// 					for (int j0=0;j0<nNum0;j0++)
// 					{
// 						if (ftr0[j0]!=pFtr&&ftr0[j0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
// 						{
// 							if( bModifyZ )
// 							{
// 								if( ftr0[j0]->GetGeometry()->FindNearestBasePt(targetPt,e2,NULL,&RetPt,NULL))
// 								{
// 									if( fabs(targetPt.z-RetPt.z)<GraphAPI::g_lfZTolerance )
// 										break;
// 								}
// 							}						
// 						}
// 					}
// 
// 					//不是悬挂点
// 					if( j0<nNum0 )
// 					{
// 						return 2;
// 					}
// 				}
// 			}

			//是悬挂点，首先尝试按照延伸或者缩进来处理
			PT_3DEX ret;
			ret.pencode = penLine;
			double mint = -1, mindis = -1;
			CFeature *pFtr0 = NULL;
			int idx = -1;
			CArray<PT_3DEX,PT_3DEX> arrPts1;

			for (j=0;j<nNum;j++)
			{
				if(ftr[j]==pFtr)
					continue;

				if (!ftr[j]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}
				if(!bCrossLay&&strLayName.CompareNoCase(pDS->GetFtrLayerOfObject(ftr[j])->GetName())!=0)
					continue;
				CArray<PT_3DEX,PT_3DEX> arrPts0;
				((CGeoCurveBase*)ftr[j]->GetGeometry())->GetShape()->GetPts(arrPts0);
				int nSz = arrPts0.GetSize();
				int start = 0, end = nSz-1;

				for (int k=start;k<end;k++)
				{
					double xt,yt;
					double t0;
					if(GraphAPI::GGetLineIntersectLineSeg(line[0].x,line[0].y,line[1].x,line[1].y,arrPts0[k].x,arrPts0[k].y,arrPts0[k+1].x,arrPts0[k+1].y,&xt,&yt,&t0))
					{
						if (idx<0||fabs(t0-0.5)<fabs(mint-0.5))
						{
							idx = k;
							mint = t0;
							pFtr0 = ftr[j];								
							ret.x = xt;
							ret.y = yt;
							double tem = arrPts0[k+1].x-arrPts0[k].x;
							if(fabs(tem)<c_minToler)
								ret.z = arrPts0[k].z+(arrPts0[k+1].z-arrPts0[k].z)*(yt-arrPts0[k].y)/(arrPts0[k+1].y-arrPts0[k].y);
							else
								ret.z = arrPts0[k].z+(arrPts0[k+1].z-arrPts0[k].z)*(xt-arrPts0[k].x)/tem;
						}
					}					
				}
			}
			//可以按照延伸或者缩进来处理
			if( mint>=0.0 && mint<=1.0 && pFtr0!=NULL )
			{
				goto modify_obj;
			}

			//尝试按照垂直距离处理
			ret = PT_3DEX();
			ret.pencode = penLine;
			pFtr0 = NULL;
			idx = -1;

			if(!bAllowVSuspend)
			{
				return 3;
			}

			for (j=0;j<nNum;j++)
			{
				if(ftr[j]==pFtr)
					continue;

				if (!ftr[j]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}
				if(!bCrossLay&&strLayName.CompareNoCase(pDS->GetFtrLayerOfObject(ftr[j])->GetName())!=0)
					continue;

				Envelope sch_evlp;
				sch_evlp.CreateFromPtAndRadius(targetPt,lfSusToler);

				CGeoCurveBase *pGeo = (CGeoCurveBase*)ftr[j]->GetGeometry();
				
				if( pGeo->FindNearestBasePt(targetPt,sch_evlp,NULL,&ret,NULL) )
				{
					if( fabs(targetPt.z-ret.z)>=lfTolerZ )
						ret.z = targetPt.z;

					idx = 1;
					pFtr0 = ftr[j];
				}
			}

			//可以按照垂直距离处理
modify_obj:			
			
			if( idx>=0 )
			{
				int state = 0;
				idx = CLinearizer::FindKeyPosOfBaseLine(ret,((CGeoCurveBase*)pFtr0->GetGeometry())->GetShape(),NULL,&state);

				pFtr0->GetGeometry()->GetShape(arrPts1);
			}

			if( idx>=0 && idx<arrPts1.GetSize() )
			{
				BOOL bModifiedOther = FALSE;

				if (GraphAPI::GGet2DDisOf2P(arrPts1[idx],ret)<lfSusToler)
				{
					tempPt = arrPts1[idx];
					tempPt.pencode = arrPtsKey[cur_idx].pencode;								
					if(!bModifyZ)
					{
						tempPt.z = targetPt.z;
					}
					arrPtsKey.SetAt(cur_idx,tempPt);
				}
				else if ( idx<arrPts1.GetSize()-1 && GraphAPI::GGet2DDisOf2P(arrPts1[idx+1],ret)<lfSusToler)
				{
					tempPt = arrPts1[idx+1];
					tempPt.pencode = arrPts[cur_idx].pencode;								
					if(!bModifyZ)
					{
						tempPt.z = targetPt.z;
					}
					arrPtsKey.SetAt(cur_idx,tempPt);
				}
				else
				{
					tempPt = ret;
					tempPt.pencode = arrPtsKey[cur_idx].pencode;								
					if(!bModifyZ)
					{
						tempPt.z = targetPt.z;
					}
					arrPtsKey.SetAt(cur_idx,tempPt);
					arrPts1.InsertAt(idx+1,ret);

					bModifiedOther = TRUE;
				}
						
				if( bModifiedOther && bInsertPoint )
				{
					CFeature *pNew = pFtr0->Clone();
					if (pNew)
					{
						pNew->SetID(OUID());
						if(pNew->GetGeometry()->CreateShape(arrPts1.GetData(),arrPts1.GetSize())&&AddObject(pNew,GetFtrLayerIDOfFtr(FtrToHandle(pFtr0))))
						{								
							undo.AddNewFeature(FtrToHandle(pNew));
							GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr0,pNew);
							if(DeleteObject(FtrToHandle(pFtr0)))
							{
								if( pArrReplacedFtrs )
								{
									pArrReplacedFtrs->Add(pFtr0);
									pArrReplacedFtrs->Add(pNew);
								}
								undo.AddOldFeature(FtrToHandle(pFtr0));	
							}
						}
					}
				}

				CFeature *pNew = pFtr->Clone();
				if (pNew)
				{
					pNew->SetID(OUID());
					if(pNew->GetGeometry()->CreateShape(arrPtsKey.GetData(),arrPtsKey.GetSize())&&AddObject(pNew,GetFtrLayerIDOfFtr(FtrToHandle(pFtr))))
					{								
						undo.AddNewFeature(FtrToHandle(pNew));
						GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(pFtr,pNew);
						if(DeleteObject(FtrToHandle(pFtr)))
						{
							if( pArrReplacedFtrs )
							{
								pArrReplacedFtrs->Add(pFtr);
								pArrReplacedFtrs->Add(pNew);
							}
							undo.AddOldFeature(FtrToHandle(pFtr));	
						}
					}
				}

				bProcessed = 0;
			}			
		}
	}

	EndCheck41License

	return bProcessed;
}


void CDlgDoc::OnRemoveAllSuspend() 
{	
	CChkResManger *pChkRes = &GetChkResMgr();		
	
	CString chkName = StrFromResID(IDS_SELCOND_SUSPENDPOINT);
	CArray<CFeature*,CFeature*> arrFtrs;
	CArray<PT_3D,PT_3D> arrPts;
	CPtrArray arrPItems;
	
	CDataQuery *pDQ = GetDataQuery();

	for( int i=0; i<pChkRes->GetChkResCount(); i++ )
	{
		ChkResultItem *pItem = pChkRes->GetChkResByIdx(i);
		if( pItem->reason.CompareNoCase(chkName)!=0 )
			continue;

		if( pItem->state!=0 )
			continue;

		if( pItem->arrFtrs.GetSize()!=1 )
			continue;

		if( !pDQ->IsManagedObj(pItem->arrFtrs[0]) )
		{
			continue;
		}
		if( pItem->arrFtrs[0]->IsDeleted() )
			continue;

		arrFtrs.Append(pItem->arrFtrs);
		arrPts.Add(pItem->pos);
		arrPItems.Add(pItem);
	}

	if( arrFtrs.GetSize()<=0 )
	{
		AfxMessageBox(IDS_CHK_NOSUSPENDPOINTS);
		return;
	}

	CDlgProcessSuspendPoints dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	CDlgDataSource *pDS = GetDlgDataSource();

	double lfSusToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTRADIUS,0.2);
	BOOL bCrossLay = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCROSSLAY,0);
	BOOL bModifyZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTZ,1);
	BOOL bVep = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTVEP,1);
	double lfSusZToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTZTOLER,0.2);
	BOOL bInsertPoint = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_INSERTPOINT,FALSE);
	BOOL bAllowVSuspend = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_ALLOWVSUSPEND,TRUE);

	pDS->SaveAllAppFlags();
	pDS->SaveAllQueryFlags();
	pDS->SetFtrLayerQueryFlag(StrFromResID(IDS_MARKLAYER_NAME),FALSE);

	CUndoFtrs undo(this,"Remove Suspended Points");

	CFtrArray replaceFtrs, replaceFtrs_all;
	GProgressStart(arrFtrs.GetSize());
	BeginBatchUpdate();
	for( i=0; i<arrFtrs.GetSize(); i++ )
	{
		GProgressStep();
		CFeature *pFtr = arrFtrs.GetAt(i);

		while( pFtr->GetAppFlag()!=0 )
		{
			pFtr = replaceFtrs_all[pFtr->GetAppFlag()-1];
		}

		int bRet = KillSuspendOne(pFtr,arrPts[i],lfSusToler,lfSusZToler,bCrossLay,bModifyZ,bVep,bInsertPoint,undo,&replaceFtrs, bAllowVSuspend);

		if( bRet==0 || bRet==2 )
		{
			ChkResultItem *pItem = (ChkResultItem*)arrPItems[i];
			((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.SetItemState(pItem,2);

			for(int j=0; j<replaceFtrs.GetSize(); j+=2)
			{
				CFeature *pFtr1 = replaceFtrs[j];
				CFeature *pFtr2 = replaceFtrs[j+1];

				pFtr1->SetAppFlag(1+replaceFtrs_all.Add(pFtr2));
			}
		}

		replaceFtrs.RemoveAll();
	}
	GProgressEnd();

	undo.DelInvalidUndoInfo();
	undo.Commit();

	EndBatchUpdate();

	pDS->RestoreAllQueryFlags();
	pDS->RestoreAllAppFlags();
	RefreshView();	
}


void CDlgDoc::OnRemoveOneSuspend()
{
	ChkResultItem *pItem = ((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.GetCurItem();
	if( pItem==NULL )
		return;

	CString chkName = StrFromResID(IDS_SELCOND_SUSPENDPOINT);
	if( pItem->state!=0 || pItem->arrFtrs.GetSize()!=1 || pItem->reason.CompareNoCase(chkName)!=0 )
		return;
	
	CDlgDataSource *pDS = GetDlgDataSource();
	
	double fScale = pDS->GetScale()*0.001;
	double lfSusToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTRADIUS,0.2);
	lfSusToler *= fScale;
	BOOL bCrossLay = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCROSSLAY,0);
	BOOL bModifyZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTZ,0);
	BOOL bVep = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTVEP,1);
	BOOL bInsertPoint = AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_INSERTPOINT,FALSE);
	double lfSusZToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTZTOLER,0.2);
	
	CUndoFtrs undo(this,"Remove Suspended Points");

	pDS->SaveAllQueryFlags();
	pDS->SetFtrLayerQueryFlag(StrFromResID(IDS_MARKLAYER_NAME),FALSE);
	
	CFtrArray replaceFtrs;
	BeginBatchUpdate();

	CFeature *pFtr = pItem->arrFtrs[0];		
		
	int bRet = KillSuspendOne(pFtr,pItem->pos,lfSusToler,lfSusZToler,bCrossLay,bModifyZ,bVep,bInsertPoint,undo,&replaceFtrs, FALSE);		
		
	undo.DelInvalidUndoInfo();
	undo.Commit();

	if( bRet==0 || bRet==2 )
	{
		((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.SetItemState(pItem,2);
	}

	EndBatchUpdate();
	pDS->RestoreAllQueryFlags();

	RefreshView();	

}



void CDlgDoc::OnStatistics()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	int nLayCount = pDS->GetFtrLayerCount();
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = NULL;
	int nPoint = 0,nLine = 0, nArea = 0, nText = 0;
	for (int i=0;i<nLayCount;i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			pFtr = pLayer->GetObject(j);
			if (!pFtr)continue;
			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			{
				nPoint++;
			}
			else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				nLine++;
			}
			else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				nArea++;
			}
			else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				nText++;
			}
		}
	}
	CString strText;
	strText.Format(_T("%s %d"),StrFromResID(IDS_POINT_COUNT),nPoint);
	GOutPut(strText);
	strText.Format(_T("%s %d"),StrFromResID(IDS_LINE_COUNT),nLine);
	GOutPut(strText);
	strText.Format(_T("%s %d"),StrFromResID(IDS_AREA_COUNT),nArea);
	GOutPut(strText);
	strText.Format(_T("%s %d"),StrFromResID(IDS_TEXT_COUNT),nText);
	GOutPut(strText);
}

void CDlgDoc::OnStatisticsClosed()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	int nLayCount = pDS->GetFtrLayerCount();
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = NULL;
	int nClosedFtr = 0;
	double area = 0.0;
	for (int i=0;i<nLayCount;i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			pFtr = pLayer->GetObject(j);
			if (!pFtr)continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo)continue;
			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				CArray<PT_3DEX, PT_3DEX> arrPts;
				
				pGeo->GetShape(arrPts);
				int num = arrPts.GetSize();
				if(num<4) continue;
				if (GraphAPI::GIsEqual2DPoint(&arrPts[0],&arrPts[num-1]))
				{
					nClosedFtr++;
					double v = ((CGeoCurveBase*)pGeo)->GetArea();
					area += v;
				}
			}
			else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				nClosedFtr++;
				double v = ((CGeoCurveBase*)pGeo)->GetArea();
				area += v;
			}
		}
	}
	CString strText;
	strText.Format(_T("\r\n%s %d"),StrFromResID(IDS_CLOSEDFEA_COUNT),nClosedFtr);
	GOutPut(strText);
	strText.Format(_T("%s %lf\r\n"),StrFromResID(IDS_CLOSEDFEA_AREA),area);
	GOutPut(strText);
}



BOOL CreateFdbFromCurSchemAndTemplate(LPCTSTR newFdb, LPCTSTR templFdb)
{
	CString strPath = newFdb;

	CSQLiteAccess *pSqlAccess_old = new CSQLiteAccess;
	pSqlAccess_old->Attach(templFdb);

	CDataSourceEx *pDS = new CDataSourceEx();
	pSqlAccess_old->ReadDataSourceInfo(pDS);

	CSQLiteAccess *pSqlAccess = new CSQLiteAccess;
	pSqlAccess->Attach(strPath);

	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	CScheme *pScheme = pCfgLibManager->GetScheme(pDS->GetScale());
	DWORD scale = pCfgLibManager->GetScaleByScheme(pScheme);
	if (scale==0)
	{
		delete pDS;
		delete pSqlAccess_old;
		delete pSqlAccess;
		return FALSE;
	}

	pSqlAccess->BatchUpdateBegin();
	pSqlAccess->CreateFileSys(scale, pScheme);
	pSqlAccess->WriteDataSourceInfo(pDS);
	pSqlAccess->BatchUpdateEnd();

	delete pDS;
	delete pSqlAccess_old;
	delete pSqlAccess;

	return TRUE;
}
	

#ifndef TRIAL_VERSION


void CDlgDoc::OnExportFdb()
{
	CDlgExportFdb dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	CString strPath = dlg.m_strFilePath;

	if( strPath.Right(4).CompareNoCase(_T(".fdb"))!=0 )
	{
		strPath += _T(".fdb");
	}
	else
	{
		::PathRenameExtension(strPath.GetBuffer(_MAX_PATH),_T(".fdb"));
		strPath.ReleaseBuffer();
	}

	int  nOption = dlg.m_nOption;
	if(::PathFileExists(strPath))
	{
		if(::DeleteFile(strPath)==FALSE)
			return;
	}
	CDlgDataSource *pDS = GetDlgDataSource();	
	if(!pDS)return;
	CDataQueryEx *pDQ = new CDataQueryEx();
	CDlgDataSource *pDataSource = new CDlgDataSource(pDQ);	
	if( !pDataSource )
	{	
		return ;
	}
	CSQLiteAccess *pSqlAccess = new CSQLiteAccess;
	if(!pSqlAccess) 
	{
		delete pDataSource;
		return;
	}
	pSqlAccess->Attach(strPath);

	pDataSource->SetAccessObject(pSqlAccess);
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	CScheme *pScheme = pCfgLibManager->GetScheme(pDS->GetScale());
	DWORD scale = pCfgLibManager->GetScaleByScheme(pScheme);
	if (scale==0)
	{
		delete pDataSource;
		return ;
	}
	pSqlAccess->BatchUpdateBegin();
	pSqlAccess->CreateFileSys(scale, pScheme);
	pSqlAccess->BatchUpdateEnd();
	
	pDataSource->LoadAll(NULL);	
	pDataSource->ResetDisplayOrder();
	pSqlAccess->BatchUpdateBegin();
	pDataSource->SaveAllLayers();
	pSqlAccess->BatchUpdateEnd();
	//写入工作区信息
	pDataSource->SetScale(pDS->GetScale());
	PT_3D pts[4];
	double zmin,zmax;
	pDS->GetBound(pts,&zmin,&zmax);
	pDataSource->SetBound(pts,zmin,zmax);

	
	CFtrLayer *pLayer = NULL,*pLayer0 = NULL;
	CFeature *pFtr = NULL,*pFtr0 = NULL;
	CValueTable tab;
	pSqlAccess->BatchUpdateBegin();
	if (nOption==0)
	{
		int nSum = 0;		
		int nLay = pDS->GetFtrLayerCount();
		for (int i=0;i<nLay;i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())continue;			
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if (pFtr)
				{					
					nSum++;
				}
			}
		}
		GProgressStart(nSum);
		for (i=0;i<nLay;i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())continue;
			pLayer0 = pDataSource->GetFtrLayer(pLayer->GetName());
			if (pLayer0)
			{
				;
			}
			else
			{
				pLayer0 = new CFtrLayer;
				if(!pLayer0)continue;
				tab.DelAll();
				tab.BeginAddValueItem();
				pLayer->WriteTo(tab);
				tab.EndAddValueItem();
				pLayer0->ReadFrom(tab);
				pLayer0->SetID(0);
				pDataSource->AddFtrLayer(pLayer0);
			}
			int nObj = pLayer->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if (pFtr)
				{
					GProgressStep();
					pFtr0 = pFtr->Clone();
					pFtr0->SetID(OUID());
					if(!pDataSource->AddObject(pFtr0,pLayer0->GetID()))
					{
						delete pFtr0;
						continue;
					}
					tab.DelAll();
					tab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab);
					tab.EndAddValueItem();
					pDataSource->GetXAttributesSource()->SetXAttributes(pFtr0,tab);
				}
			}
		}
		GProgressEnd();
	}
	else
	{
		int nObj;
		const FTR_HANDLE *handles = GetSelection()->GetSelectedObjs(nObj);
		GProgressStart(nObj);
		for (int i=0;i<nObj;i++)
		{	
			pFtr = HandleToFtr(handles[i]);			
			if (pFtr)
			{
				pLayer = pDS->GetFtrLayerOfObject(pFtr);
				pLayer0 = pDataSource->GetFtrLayer(pLayer->GetName());
				if (pLayer0)
				{
					;
				}
				else
				{
					pLayer0 = new CFtrLayer;
					if(!pLayer0)continue;
					tab.DelAll();
					tab.BeginAddValueItem();
					pLayer->WriteTo(tab);
					tab.EndAddValueItem();
					pLayer0->ReadFrom(tab);
					pLayer0->SetID(0);
					pDataSource->AddFtrLayer(pLayer0);
				}				
				GProgressStep();
				pFtr0 = pFtr->Clone();
				pFtr0->SetID(OUID());
				if(!pDataSource->AddObject(pFtr0,pLayer0->GetID()))
				{
					delete pFtr0;
					continue;
				}			
				tab.DelAll();
				tab.BeginAddValueItem();
				pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab);
				tab.EndAddValueItem();
				pDataSource->GetXAttributesSource()->SetXAttributes(pFtr0,tab);
			}			
		}
		GProgressEnd();
	}
	pSqlAccess->BatchUpdateEnd();
	delete pDataSource;
}

#endif

void CDlgDoc::ExecuteCheck(LPCTSTR pGroupName,LPCTSTR pItem, BOOL bForSelection)
{
	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,1,0);

	CCheckScheme& refCheckScheme = GetCheckScheme();
	CChkResManger& refChkResMgr = GetChkResMgr();
	CCheckSchemeViewBar& refCheckSchemeViewBar = ((CMainFrame*)AfxGetMainWnd())->m_wndCheckSchemeViewBar;
	if (pGroupName)
	{
		int nNum = 0;
		CCheckGroup* pGroup = refCheckScheme.GetCheckGroup(nNum);
		if(nNum>0)
		{
			for (int i=0;i<nNum;i++)
			{
				if(pGroup[i].m_groupName==pGroupName)
					break;
			}
			if (i<nNum)
			{
				CArray<CCheckItem,CCheckItem&> &itemList = pGroup[i].m_itemList;
				if (pItem!=NULL)
				{
					GOutPut(StrFromResID(IDS_CHK_BEGIN));
					for (int j=0;j<itemList.GetSize();j++)
					{
						if( refCheckSchemeViewBar.IsItemSelected(pGroupName, itemList[j].m_itemName) )
						{
							CArray<CCheckTask,CCheckTask&> & refChkTaskList = itemList[j].m_taskList;
							for (int k=0;k<refChkTaskList.GetSize();k++)
							{
								CChkCmd *pCmd = CreateChkCmd(refChkTaskList[k].m_checkCategory,refChkTaskList[k].m_checkName);
								if (pCmd)
								{
									CString strTemp(pCmd->GetCheckCategory());
									strTemp+=_T("->");
									strTemp+=pCmd->GetCheckName();
									strTemp+=_T("...");
									GOutPut(strTemp);
									pCmd->Init(this);
									pCmd->SetChkResMgr(&refChkResMgr);
									CMarkup xml;
									xml.SetDoc(refChkTaskList[k].m_checkParams);
									xml.FindElem(XMLTAG_CHKTASKPARAM);
									xml.IntoElem();
									CValueTable tab;				
									tab.BeginAddValueItem();
									Xml_ReadValueTable(xml,tab);
									tab.EndAddValueItem();
									xml.OutOfElem();
									pCmd->SetParams(tab);
									refChkResMgr.SetCurChkCmd(pCmd);
									pCmd->Execute(bForSelection);
									refChkResMgr.SetCurChkCmd(NULL);
									delete pCmd;
								}
							}
						}
					}
					GOutPut(StrFromResID(IDS_CHK_END));
					AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);
				}
				else
				{
					GOutPut(StrFromResID(IDS_CHK_BEGIN));
					for (int j=0;j<itemList.GetSize();j++)
					{
						CArray<CCheckTask,CCheckTask&> & refChkTaskList = itemList[j].m_taskList;
						for (int k=0;k<refChkTaskList.GetSize();k++)
						{
							CChkCmd *pCmd = CreateChkCmd(refChkTaskList[k].m_checkCategory,refChkTaskList[k].m_checkName);
							if (pCmd)
							{
								CString strTemp(pCmd->GetCheckCategory());
								strTemp+=_T("->");
								strTemp+=pCmd->GetCheckName();
								strTemp+=_T("...");
								GOutPut(strTemp);
								pCmd->Init(this);
								pCmd->SetChkResMgr(&refChkResMgr);
								CMarkup xml;
								xml.SetDoc(refChkTaskList[k].m_checkParams);
								xml.FindElem(XMLTAG_CHKTASKPARAM);
								xml.IntoElem();
								CValueTable tab;				
								tab.BeginAddValueItem();
								Xml_ReadValueTable(xml,tab);
								tab.EndAddValueItem();
								xml.OutOfElem();
								pCmd->SetParams(tab);
								refChkResMgr.SetCurChkCmd(pCmd);
								pCmd->Execute(bForSelection);
								refChkResMgr.SetCurChkCmd(NULL);
								delete pCmd;
							}
						}
					}
					GOutPut(StrFromResID(IDS_CHK_END));
					AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);
				}
			}
		}
	}
}

void CDlgDoc::OnExportArcgisMdb()
{
	CDlgExportArcgisMdb dlg;
	dlg.DoModal();
}

BOOL CDlgDoc::ImportFDB(LPCTSTR path, CFtrLayer *pLayer, PT_3D move_pt, PT_3D rotate_pt, double angle)
{
	CString strPathName = path;

	if( !pLayer )
		return FALSE;

	{
		CSQLiteAccess fdb;
		BOOL bOpen = fdb.Attach(strPathName);
		if( !bOpen )
		{
			CString temp;
			temp.Format(IDS_REFDATA_NOT_EXIST,path);
			AfxMessageBox(temp);
			return FALSE;
		}

		// 清除undo队列
		DeleteUndoItems();

		CDlgDataSource *pDS = GetDlgDataSource();
		if (!pDS) 
			return FALSE;

		// 记录当前层，导完后得置
		CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
		pDS->SetCurFtrLayer(pLayer->GetID());
		
		// 加载
		fdb.BatchUpdateBegin();
		BeginBatchUpdate();	
		
		long idx;
		int lSum = fdb.GetFeatureCount();
		
		GOutPut(StrFromResID(IDS_LOAD_FEATURES));
		GProgressStart(lSum);			
		
		CFeature *pFt = fdb.GetFirstFeature(idx);

		double m1[16], m2[16], m3[16];
		Matrix44FromMove(move_pt.x,move_pt.y,move_pt.z,m1);

		if( angle!=0.0 )
			Matrix44FromRotate(&rotate_pt,angle,m2);

		matrix_multiply(m1,m2,4,m3);
		
		while( pFt )
		{
			GProgressStep();
			
			pFt->GetGeometry()->Transform(m3);

			AddObject(pFt);
			
			pFt = fdb.GetNextFeature(idx);
			
		}

		EndBatchUpdate();
		
		GProgressEnd();
		GOutPut(StrFromResID(IDS_LOAD_FINISH));

		fdb.BatchUpdateEnd();
		fdb.Close();

		// 重置当前层
		if (pCurLayer)
		{
			pDS->SetCurFtrLayer(pCurLayer->GetID());
		}

		UpdateAllViews(NULL,hc_UpdateAllObjects);
	}

	return TRUE;
}


BOOL CDlgDoc::ImportFDB(LPCTSTR path)
{
//	VM_START
		
	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return FALSE;
	
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme)return FALSE;

	CString strPathName = path;
	
	//加密
	if( !CheckLicense(1) )
	{
		return FALSE;
	}

//	VM_END

	CAttributesSource *pDSX = pDS->GetXAttributesSource();

	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
		
	CSQLiteAccess fdb;

	GOutPut(StrFromResID(IDS_LOAD_FEATURES));

	CUndoFtrs undo(this,"Import Fdb");

	{	

		BOOL bOpen = fdb.Attach(strPathName);
		if( !bOpen )
			return FALSE;	
		
		CString strVersion = fdb.GetFileVersion();
		BOOL bOldVersion = (strVersion.Find(_T("FeatureOne4.1"))<0);

		// 加载
		fdb.BatchUpdateBegin();
		BeginBatchUpdate();	
		
		long idx;
		int lSum = fdb.GetFtrLayerCount();
		lSum += fdb.GetFeatureCount();		
		
		GProgressStart(lSum);	
		
		CUIntArray layid;
		CMap<int, int&, int, int&> idpair;
		
		CFtrLayer *pLayer = fdb.GetFirstFtrLayer(idx);
		while( pLayer )
		{
			GProgressStep();

			int pid = pLayer->GetID(), cid;
			pLayer->SetID(0);
			pLayer->SetInherentFlag(FALSE);

			CFtrLayer *pExistLayer = pDS->GetFtrLayer(pLayer->GetName(),pLayer->GetMapName());
			if (pExistLayer == NULL)
			{
				if( !AddFtrLayer(pLayer) )
				{
					delete pLayer;
					pLayer = fdb.GetNextFtrLayer(idx);
					continue;
				}
				else
				{
					cid = pLayer->GetID();
					layid.Add(pLayer->GetID());
					
					if( pDS->GetCurFtrLayer()==NULL )
						pDS->SetCurFtrLayer(pLayer->GetID());
				}
				
			}
			else
				cid = pExistLayer->GetID();
			idpair.SetAt(pid,cid);
			
			pLayer = fdb.GetNextFtrLayer(idx);	
			
		}
		
		int id;
		CFeature *pFt = fdb.GetFirstFeature(idx);
		
		while( pFt )
		{
			GProgressStep();
			
			int pid = fdb.GetCurFtrLayID();
			if (idpair.Lookup(pid,id))
			{
				pDS->SetCurFtrLayer(id);
			}

			//以前的版本的地物，自动给一个线宽；面对象自动补一个闭合点
			if (bOldVersion)
			{
				float wid = 0;
				CFtrLayer *pFtrLayer = pDS->GetCurFtrLayer();
				if (pFtrLayer)
				{
					wid = pScheme->GetLayerDefineLineWidth(pFtrLayer->GetName());
				}
				
				CGeometry *pGeo = pFt->GetGeometry();
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					((CGeoCurve*)pGeo)->m_fLineWidth = wid;
				}
				else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					((CGeoSurface*)pGeo)->m_fLineWidth = wid;
					if( pGeo->GetClassType()==CLS_GEOSURFACE )
					{
						CArray<PT_3DEX,PT_3DEX> arrPts;
						pGeo->GetShape(arrPts);
						if( arrPts.GetSize()>2 )
						{
							arrPts.Add(arrPts[0]);
							pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
						}
					}
				}
			}

			// 导入地物的扩展属性
			BOOL bSucceed = FALSE;
			CValueTable tab;
			tab.BeginAddValueItem();
			bSucceed = fdb.ReadXAttribute(pFt,tab);
			tab.EndAddValueItem();	
			
			//ID为空时，会写入缺省扩展属性
			pFt->SetID(OUID());

			if (pDS->IsFeatureValid(pFt) && pDS->AddObject(pFt))
			{
				// 存在扩展属性才导入
				if (bSucceed && tab.GetFieldCount()>0 )
				{
					pDSX->SetXAttributes(pFt,tab);
				}
				
			}
			else
				delete pFt;
			
			undo.arrNewHandles.Add(FTR_HANDLE(pFt));

			pFt = fdb.GetNextFeature(idx);
			
		}
		
		GProgressEnd();		

		fdb.BatchUpdateEnd();
		fdb.Close();

		// 加载的空层删掉
		for (int i=0; i<layid.GetSize(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayer(layid.GetAt(i));		
			if( !pLayer ) 	continue;

			if (pLayer->GetObjectCount() == 0)
			{
				pDS->DelFtrLayer(layid.GetAt(i));
			}
		}

		EndBatchUpdate();
	}

	undo.Commit();

	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}

	GOutPut(StrFromResID(IDS_LOAD_FINISH));

	UpdateAllViews(NULL,hc_UpdateAllObjects);

	return TRUE;
		
}

static BOOL GetFDBWorkspace(LPCTSTR path, long& scale, PT_3D *ptsBound, double& zmin, double& zmax)
{	
	CString strPathName = path;
		
	CSQLiteAccess  *pAccess = new CSQLiteAccess;
	if( !pAccess )
		return FALSE;

	if( !pAccess->Attach(strPathName) )
	{
		delete pAccess;
		return FALSE;
	}
	
	CDlgDataSource *pDataSource = new CDlgDataSource(NULL);	
	if( !pDataSource )
	{
		delete pAccess;
		return FALSE;
	}
	
	pAccess->ReadDataSourceInfo(pDataSource);

	zmin = -1000,zmax = 1000;
	pDataSource->GetBound(ptsBound,&zmin,&zmax);
	
	scale = pDataSource->GetScale();
	
	delete pAccess;
	delete pDataSource;	
		
	return TRUE;
}


BOOL CDlgDoc::UpgradeFDB(LPCTSTR path)
{
	//备份源文件
	CString filePath = path, bakPath;
	if( filePath.Right(4).CompareNoCase(".fdb")==0 )
	{
		bakPath = filePath.Left(filePath.GetLength()-4) + ".oldver.fdb";
	}
	else
	{
		bakPath = filePath + ".oldver.fdb";
	}

	int pos1 = bakPath.ReverseFind(_T('\\'));
	if (pos1 == -1) return FALSE;
	bakPath.Insert(pos1,_T("\\fdb_baks"));
	
	// 备份文件夹
	pos1 = bakPath.ReverseFind(_T('\\'));
	CString strBaks = bakPath.Left(pos1);
	if (!PathFileExists(strBaks))
	{
		CreateDirectory(strBaks,NULL);
	}

	CString strMsg;
	strMsg.Format(IDS_BAKING_OLDVER_FDB,(LPCTSTR)bakPath);

	GOutPut(strMsg);

	if( !::CopyFile(path,bakPath,FALSE) )
		return FALSE;

	//新建一个 FDB 文件
	CSQLiteAccess *pAccess = new CSQLiteAccess;
	if( !pAccess )
		return FALSE;
	
	::DeleteFile(path);
	
	pAccess->Attach(path);
	
	CDlgDataSource *pDataSource = new CDlgDataSource(m_pDataQuery);	
	if( !pDataSource )
	{
		delete pAccess;
		return FALSE;
	}
	pDataSource->SetAccessObject(pAccess);

	//读取工作区信息
	long nScale = 2000;
	PT_3D ptsBound[4];
	ptsBound[0].x = 0, ptsBound[0].y = 0;
	ptsBound[1].x = 1000, ptsBound[1].y = 0;
	ptsBound[0].x = 1000, ptsBound[0].y = 1000;
	ptsBound[0].x = 0, ptsBound[0].y = 1000;
	double zmin = -1000, zmax = 1000;
	GetFDBWorkspace(bakPath,nScale,ptsBound,zmin,zmax);
	
	//创建方案图层
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(nScale);
	if (config.GetScale() == 0)
	{
		CDlgScheme::CreateNewScale(nScale);
	}
	CScheme *pScheme = pCfgLibManager->GetScheme(nScale);
	
	pAccess->CreateFileSys(nScale, pScheme);
	
	pDataSource->LoadAll(NULL,FALSE);	
	pDataSource->ResetDisplayOrder();
	
	//设置工作区信息
	pDataSource->SetBound(ptsBound,zmin,zmax);
	pDataSource->SetScale(nScale);

	//保存并退出
	pAccess->BatchUpdateBegin();
	pDataSource->SaveAll(NULL);
	pAccess->BatchUpdateEnd();
	
	delete pDataSource;

	//重新打开文件
	if( !LoadDatabase(path) )
	{
		return FALSE;
	}

	//导入数据
	if( !ImportFDB(bakPath) )
	{
		return FALSE;
	}

	//保存	
	::CopyFile((LPCTSTR)m_strTmpFile,(LPCTSTR)m_strPathName,FALSE);

	GetDlgDataSource()->SetModifiedFlag(FALSE);
	GetDlgDataSource()->SetSavedFlag(TRUE);

	return TRUE;
}

BOOL CDlgDoc::ImportXML(LPCTSTR path, CFtrLayer *pLayer, PT_3D move_pt, PT_3D rotate_pt, double angle, CFtrArray* arr)
{
	CString strPathName = path;
	
	if( !pLayer )
		return FALSE;
	
	{
		CXmlAccess xml;
		BOOL bOpen = xml.OpenRead(strPathName);
		if( !bOpen )
		{
			CString temp;
			temp.Format(IDS_REFDATA_NOT_EXIST,path);
			//AfxMessageBox(temp);
			return FALSE;
		}
		
		// 清除undo队列
		DeleteUndoItems();
		
		CDlgDataSource *pDS = GetDlgDataSource();
		if (!pDS) 
			return FALSE;
		
		// 记录当前层，导完后得置
		CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
		if( pLayer )pDS->SetCurFtrLayer(pLayer->GetID());
		
		// 加载
		xml.BatchUpdateBegin();
		BeginBatchUpdate();	
		
		long idx;
		int lSum = xml.GetFeatureCount();
		
		GOutPut(StrFromResID(IDS_LOAD_FEATURES));
		GProgressStart(lSum);			
		
		CFeature *pFt = xml.GetFirstFeature(idx);
		
		double m1[16], m2[16], m3[16];
		Matrix44FromMove(move_pt.x,move_pt.y,move_pt.z,m1);
		
		if( angle!=0.0 )
			Matrix44FromRotate(&rotate_pt,angle,m2);
		else
			matrix_toIdentity(m2,4);
		
		matrix_multiply(m2,m1,4,m3);

		long ilay = 0;
		CFtrLayer *pl = xml.GetFirstFtrLayer(ilay);
		CArray<CFtrLayer*,CFtrLayer*> arrPLayers;
		while( pl )
		{
			arrPLayers.Add(pl);
			pl = xml.GetNextFtrLayer(ilay);
		}
		
		while( pFt )
		{
			GProgressStep();
			
			pFt->GetGeometry()->Transform(m3);

			if( pLayer )
			{
				int layID = xml.GetCurFtrLayID();
				for( int i=0; i<arrPLayers.GetSize(); i++)
				{
					if( layID==arrPLayers[i]->GetID() )
						break;
				}
				if( i<arrPLayers.GetSize() )
				{
					pFt->GetGeometry()->SetSymbolName(arrPLayers[i]->GetName());

					if( pFt->GetGeometry()->GetColor()==COLOUR_BYLAYER )
					{
						pFt->GetGeometry()->SetColor(arrPLayers[i]->GetColor());
					}
				}
			}
			
			AddObject(pFt);
			if( arr )
			{
				arr->Add(pFt);
			}
			
			pFt = xml.GetNextFeature(idx);
			
		}
		
		EndBatchUpdate();

		for( int i=0; i<arrPLayers.GetSize(); i++)
		{
			delete arrPLayers[i];
		}

		arrPLayers.RemoveAll();
		
		GProgressEnd();
		GOutPut(StrFromResID(IDS_LOAD_FINISH));
		
		xml.BatchUpdateEnd();
		xml.Close();
		
		// 重置当前层
		if (pCurLayer)
		{
			pDS->SetCurFtrLayer(pCurLayer->GetID());
		}
		
		UpdateAllViews(NULL,hc_UpdateAllObjects);
	}
	
	return TRUE;
}

void CDlgDoc::OnImportArcgisMdb()
{
	BeginCheck41License

	CDlgImportMDB dlg;
	if (IDOK != dlg.DoModal())
		return;

	CString mapName = dlg.m_strMapName;
	if (mapName.CompareNoCase("*") == 0)
	{
		CString fileTitle = dlg.m_strFilePath;
		int pos = fileTitle.ReverseFind('\\');
		if (pos >= 0)fileTitle = fileTitle.Mid(pos + 1);

		mapName = fileTitle;
	}

	CDlgDataSource *pDS = GetDlgDataSource();
	if (!pDS) return;
	AfxGetApp()->WriteProfileInt("IO32\\ImportMDB_Settings", "nScale", pDS->GetScale());

	CString mdbGdbPath = dlg.m_strFilePath;

	CString tmpfdbPath = GetPathName();
	tmpfdbPath = tmpfdbPath.Left(tmpfdbPath.GetLength() - 4);
	tmpfdbPath += "_mdb.fdb";

	GOutPut(StrFromResID(IDS_PROCESSING));
	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos1;
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	strcat(path, "\\bin\\VectorConvert.exe");

	CString cmd;
	cmd.Format("\"%s\" importMDB \"%s\" \"%s\"", path, mdbGdbPath, tmpfdbPath);

	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	//stStartUpInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));
	if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo))
		return;
	while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
	{
	}
	GOutPut(StrFromResID(IDS_PROCESS_END));

	//导入tmpfdbPath
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme)  return;

	CAttributesSource *pDSX = pDS->GetXAttributesSource();

	// 记录当前层，导完后得置
	CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();

	CSQLiteAccess fdb;

	GOutPut(StrFromResID(IDS_LOAD_FEATURES));

	CUndoFtrs undo(this, "Import Mdb");

	{
		CString fileName = tmpfdbPath;

		BOOL bOpen = fdb.Attach(fileName);
		if (!bOpen) return;

		// 加载
		fdb.BatchUpdateBegin();
		BeginBatchUpdate();

		long idx;
		int lSum = fdb.GetFtrLayerCount();
		lSum += fdb.GetFeatureCount();

		GProgressStart(lSum);

		CUIntArray layid;
		CMap<int, int&, int, int&> idpair;

		CFtrLayer *pLayer = fdb.GetFirstFtrLayer(idx);
		while (pLayer)
		{
			GProgressStep();

			int pid = pLayer->GetID(), cid;
			pLayer->SetID(0);
			pLayer->SetInherentFlag(FALSE);

			CFtrLayer *pExistLayer = pDS->GetFtrLayer(pLayer->GetName(), pLayer->GetMapName());
			if (pExistLayer == NULL)
			{
				if (!AddFtrLayer(pLayer))
				{
					delete pLayer;
					pLayer = fdb.GetNextFtrLayer(idx);
					continue;
				}
				else
				{
					cid = pLayer->GetID();
					layid.Add(pLayer->GetID());

					if (pDS->GetCurFtrLayer() == NULL)
						pDS->SetCurFtrLayer(pLayer->GetID());
				}

			}
			else
				cid = pExistLayer->GetID();
			idpair.SetAt(pid, cid);

			pLayer = fdb.GetNextFtrLayer(idx);

		}

		int id;
		CFeature *pFt = fdb.GetFirstFeature(idx);

		while (pFt)
		{
			GProgressStep();

			pFt->SetCode(mapName);

			int pid = fdb.GetCurFtrLayID();
			if (idpair.Lookup(pid, id))
			{
				pDS->SetCurFtrLayer(id);
			}

			// 导入地物的扩展属性
			BOOL bSucceed = FALSE;
			CValueTable tab;
			tab.BeginAddValueItem();
			bSucceed = fdb.ReadXAttribute(pFt, tab);
			tab.EndAddValueItem();

			//ID为空时，会写入缺省扩展属性
			pFt->SetID(OUID());

			if (pDS->IsFeatureValid(pFt) && pDS->AddObject(pFt))
			{
				// 存在扩展属性才导入
				if (bSucceed && tab.GetFieldCount() > 0)
				{
					pDSX->SetXAttributes(pFt, tab);
				}

			}
			else
				delete pFt;

			undo.arrNewHandles.Add(FTR_HANDLE(pFt));

			pFt = fdb.GetNextFeature(idx);

		}

		GProgressEnd();

		fdb.BatchUpdateEnd();
		fdb.Close();

		// 加载的空层删掉
		for (int i = 0; i < layid.GetSize(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayer(layid.GetAt(i));
			if (!pLayer) 	continue;

			if (pLayer->GetObjectCount() == 0)
			{
				pDS->DelFtrLayer(layid.GetAt(i));
			}
		}

		EndBatchUpdate();
	}

	undo.Commit();

	// 重置当前层
	if (pCurLayer)
	{
		pDS->SetCurFtrLayer(pCurLayer->GetID());
	}

	GOutPut(StrFromResID(IDS_LOAD_FINISH));

	UpdateAllViews(NULL, hc_UpdateAllObjects);

	::DeleteFile(tmpfdbPath);

	EndCheck41License
}

void CDlgDoc::OnMapDecorate()
{
	CancelCurrentCommand();
	StartCommand(ID_MAPDECORATE);
}


void CDlgDoc::OnSetMapInfo()
{
	CDlgMapInfo dlg;

	TMProjectionZone prj = GetDlgDataSource()->GetProjection();
	TMDatum datum = GetDlgDataSource()->GetDatum();

	strcpy( prj.tmName,datum.tmName );

	dlg.m_tm0.SetDatum(&datum);
	dlg.m_tm0.SetZone(&prj);

	if( dlg.DoModal()!=IDOK )
		return;

	dlg.m_tm0.GetZone(&prj);
	dlg.m_tm0.GetDatum(&datum);

	BeginBatchUpdate();

	GetDlgDataSource()->SetProjection(prj);
	GetDlgDataSource()->SetDatum(datum);

	EndBatchUpdate();
}


void CDlgDoc::CreateFigure(double xoff, double yoff)
{
	CDlgDataSource *pDS = GetDlgDataSource();

	CXmlAccess xml;
	CString str = MapDecorate::CMapDecorator::GetFigurePath(pDS->GetScale());
	
	xml.OpenWrite(str);	

	CAccessWrite *pAccess = &xml;
	
	pAccess->WriteDataSourceInfo(pDS);
	
	pAccess->WriteSpecialData(pDS);
	
	int i, j;
	int nlay = pDS->GetFtrLayerCount();
	
	CFtrLayer *pLayer;
	for( i=0; i<nlay; i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer)  continue;
		
		pAccess->SaveFtrLayer(pLayer);
	}

	double m[16];
	Matrix44FromMove(xoff,yoff,0,m);

	CString strTableLayer = StrFromResID(IDS_LEGEND_TABLELINES);
	
	CFeature *pFt;
	for( i=0; i<nlay; i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		
		if( !pLayer )continue;
		if( strTableLayer.CompareNoCase(pLayer->GetName())==0 )
			continue;
		
		int nobj = pLayer->GetObjectCount();
		for( j=0; j<nobj; j++)
		{
			pFt = pLayer->GetObject(j);
			if( !pFt )continue;

			CFeature *pFtr2 = pFt->Clone();
			pFtr2->GetGeometry()->Transform(m);
			
			pAccess->SetCurFtrLayID(pLayer->GetID());
			pAccess->SaveFeature(pFtr2);

			delete pFtr2;
		}
		
	}
	
	xml.Close();
}


void CDlgDoc::OnClearCADBlocks()
{
	CellDef def;

	CDlgDataSource *pDS = GetDlgDataSource();
	
	/////uvs/////////////////////////////////
	CAccessModify *pAccess = pDS->GetAccessObject();
	if( pAccess==NULL )
		return;
	
	CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	CCellDefLib *pCellLib = pCfgLibManager->GetCellDefLib(pDS->GetScale());

	CStringArray names;

	CDataQuery *pDQ = GetDataQuery();

	for( int i=pCellLib->GetCellDefCount()-1; i>=0; i--)
	{
		def = pCellLib->GetCellDef(i);
		if (def.m_nSourceID == (DWORD_PTR)pDS)
		{
			CString symname = CString("@") + def.m_name;

			BOOL bFindObj = FALSE;
			POSITION pos = pDQ->GetFirstObjPos();
			while( pos )
			{
				CFeature *pFtr = pDQ->GetNextObjByPos(pos);
				if( !pFtr )continue;
				if( pFtr->IsDeleted() )continue;

				if( symname.CompareNoCase(pFtr->GetGeometry()->GetSymbolName())==0 )
				{
					bFindObj = TRUE;
					break;
				}
			}
			if( !bFindObj )
			{
				names.Add(CString(def.m_name));
			}
		}
	}

	CDlgTip dlg;
	dlg.SetTitle(StrFromResID(IDS_TIP_CLEAR_CADBLOCKS));	

	for( i=0; i<names.GetSize(); i++ )
	{
		pAccess->DelCellDef(names[i]);
		pCellLib->DelCellDef(names[i]);

		dlg.AddTip(names[i]);
	}

	dlg.DoModal();
	return;
}

extern BOOL DlgSelectMultiFiles(HWND hOwner, LPCTSTR filter, LPCTSTR defExt, CStringArray& fileName, CString *all_name);
void CDlgDoc::OnImportCADSymbols()
{
	CString strFilter = StrFromResID(IDS_CADSYM_FILTER);
	strFilter.Replace('|','\0');
	
	CStringArray arrFileNames;
	CString strDxfFile;
	if( !DlgSelectMultiFiles(AfxGetMainWnd()->GetSafeHwnd(),
		strFilter,
		NULL,arrFileNames,&strDxfFile) )
		return;

	CConfigLibManager *pConfig = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	if(!pConfig) return;
	CScheme *pScheme = pConfig->GetScheme(GetDlgDataSource()->GetScale());
	if(!pScheme)return;

	int nOK = 0;
	for( int i=0; i<arrFileNames.GetSize(); i++)
	{
		if( pScheme->ImportCADSymbols(arrFileNames[i]) )
			nOK++;
	}

	pScheme->SaveCADSymbols();

	if( nOK>0 )
	{
		AfxMessageBox(IDS_IMPORTCADSYMBOLS_OK);
	}
	else
	{
		AfxMessageBox(IDS_IMPORTCADSYMBOLS_FAIL);
	}
}


void CDlgDoc::OnDeleteCADSymbols()
{
	CConfigLibManager *pConfig = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	if(!pConfig) return;
	CScheme *pScheme = pConfig->GetScheme(GetDlgDataSource()->GetScale());
	if(!pScheme)return;
	
	pScheme->DeleteCADSymbols();
}

void CDlgDoc::StopWork()
{
	Destroy();
}

void CDlgDoc::OnWheelSpeedSet()
{
	CDlgWheelSpeed dlg;
	dlg.DoModal();
	
	UpdateAllViews(NULL,hc_UpdateOption);
}

/*
#include "DlgImportDGN.h"
int ConvertAlignmentAndDgnJustification(int type, BOOL fdb2Dgn)
{
	if (fdb2Dgn)
	{
		int nJustification;
		switch(type)
		{
		case TAH_LEFT:
		case TAV_TOP:
		case TAH_LEFT|TAV_TOP:
			nJustification = dgnTextJustificationLeftTop;
			break;
		case TAV_MID:
		case TAH_LEFT|TAV_MID:
			nJustification = dgnTextJustificationLeftCenter;
			break;
		case TAV_BOTTOM:
		case TAH_LEFT|TAV_BOTTOM:		
			nJustification = dgnTextJustificationLeftBottom;		
			break;	
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			nJustification = dgnTextJustificationCenterTop;			
			break;
		case TAH_MID|TAV_MID:
			nJustification = dgnTextJustificationCenterCenter;		
			break;
		case TAH_MID|TAV_BOTTOM:
			nJustification = dgnTextJustificationCenterBottom;			
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			nJustification = dgnTextJustificationRightTop;			
			break;	
		case TAH_RIGHT|TAV_MID:	
			nJustification = dgnTextJustificationRightCenter;			
			break;		
		case TAH_RIGHT|TAV_BOTTOM:
			nJustification = dgnTextJustificationRightBottom;
			break;
		default:
			nJustification = dgnTextJustificationLeftTop;			
			break;
		}
		
		return nJustification;
	}
	else
	{
		int nAlign;
		switch(type)
		{
		case dgnTextJustificationLeftTop:
			nAlign = TAH_LEFT|TAV_TOP;
			break;
		case dgnTextJustificationLeftCenter:
			nAlign = TAH_LEFT|TAH_MID;			
			break;
		case dgnTextJustificationLeftBottom:
			nAlign = TAH_LEFT|TAV_BOTTOM;			
			break;
		case dgnTextJustificationCenterTop:
			nAlign = TAH_MID|TAV_TOP;			
			break;
		case dgnTextJustificationCenterCenter:
			nAlign = TAH_MID|TAV_MID;			
			break;
		case dgnTextJustificationCenterBottom:
			nAlign = TAH_MID|TAV_BOTTOM;			
			break;
		case dgnTextJustificationRightTop:
			nAlign = TAH_RIGHT|TAV_TOP;			
			break;
		case dgnTextJustificationRightCenter:
			nAlign = TAH_MID|TAV_MID;			
			break;
		case dgnTextJustificationRightBottom:
			nAlign = TAH_RIGHT|TAV_BOTTOM;			
			break;
		default:
			nAlign = TAH_LEFT;			
			break;
		}
		
		return nAlign;
	}
	
}
*/
void CDlgDoc::OnImportDgn() 
{
	/*
	CDlgImportDGN dlg;
	if( dlg.DoModal()!=IDOK )return;
	CString fileName = dlg.m_strPath;

	CDGNAccess dgn;
	BOOL ret = dgn.Open(fileName);
	if (!ret) return;

	CListFile lstFile;
	lstFile.Open(dlg.m_strLstFile);

	CDlgDataSource *pDS = GetDlgDataSource();

	int numele = 0;
	ret = dgn.FindFirstElement();
	while (ret)
	{
		numele++;
		DGNENTDATA EntityData;
		dgn.GetCurElementData(&EntityData);

		// Level
		CString strLayerName = EntityData.LevelName;

		DGNLevel Level;
		dgn.GetLevelData(strLayerName,&Level);

		const char* listLayerName = NULL;
		if (!dlg.m_strLstFile.IsEmpty())
		{
			listLayerName = lstFile.FindMatchItem(strLayerName,TRUE,FALSE);
			if (listLayerName != NULL)
			{
				strLayerName = listLayerName;
			}
		}

		CFtrLayer *pFtrLay = pDS->GetFtrLayer(strLayerName);
		
		if (pFtrLay == NULL)
		{				
			pFtrLay = pDS->CreateFtrLayer(strLayerName);
			if (!pFtrLay)  continue;
			pFtrLay->SetColor(Level.Color);
			pDS->AddFtrLayer(pFtrLay);			
		}
		
		if (!pDS->SetCurFtrLayer(pFtrLay->GetID()))
			continue;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		int nGeoCls = CLS_GEOCURVE;

		switch (EntityData.EntityType)
		{
		case DGNENTTYPE_LINE:
			{
				PDGNENTLINE pl = (PDGNENTLINE)EntityData.GeoData;
				PT_3DEX pt;
				pt.pencode = penLine;
				
				COPY_3DPT(pt,pl->Point0);
				arrPts.Add(pt);						
				
				if (GraphAPI::GGet3DDisOf2P(pl->Point0,pl->Point1) < 1e-4)
				{
					nGeoCls = CLS_GEOPOINT;
				}
				else
				{
					COPY_3DPT(pt,pl->Point1);
					arrPts.Add(pt);
					nGeoCls = CLS_GEOCURVE;
				}
			}
			break;
		case DGNENTTYPE_ARC:
			{
				PDGNENTARC pa = (PDGNENTARC)EntityData.GeoData;
				PT_3DEX pt;
				pt.pencode = penArc;
				pt.x = pa->Point0.x + pa->PrimaryRadius * cos(pa->StartAngle);
				pt.y = pa->Point0.y + pa->PrimaryRadius * sin(pa->StartAngle);
				pt.z = pa->Point0.z;
				arrPts.Add(pt);
				
				if (pa->StartAngle < pa->SweepAngle)
				{
					pt.x = pa->Point0.x + pa->PrimaryRadius * cos((pa->StartAngle+pa->SweepAngle)/2);
					pt.y = pa->Point0.y + pa->PrimaryRadius * sin((pa->StartAngle+pa->SweepAngle)/2);
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
				}
				else
				{
					pt.x = pa->Point0.x + pa->PrimaryRadius * cos((pa->StartAngle+pa->SweepAngle)/2+PI);
					pt.y = pa->Point0.y + pa->PrimaryRadius * sin((pa->StartAngle+pa->SweepAngle)/2+PI);
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
					
				}
				
				pt.x = pa->Point0.x + pa->PrimaryRadius * cos(pa->SweepAngle);
				pt.y = pa->Point0.y + pa->PrimaryRadius * sin(pa->SweepAngle);
				pt.z = pa->Point0.z;
				arrPts.Add(pt);
			}
			break;
		case DGNENTTYPE_ELLIPSE:
			{
				PDGNENTCIRCLE pa = (PDGNENTCIRCLE)EntityData.GeoData;
				PT_3DEX pt;
				pt.pencode = penArc;
				pt.x = pa->Point0.x + pa->PrimaryRadius;
				pt.y = pa->Point0.y;
				pt.z = pa->Point0.z;
				arrPts.Add(pt);
				
				pt.x = pa->Point0.x;
				pt.y = pa->Point0.y + pa->PrimaryRadius;
				pt.z = pa->Point0.z;
				arrPts.Add(pt);
				
				pt.x = pa->Point0.x - pa->PrimaryRadius;
				pt.y = pa->Point0.y;
				pt.z = pa->Point0.z;
				arrPts.Add(pt);
				
				pt.x = pa->Point0.x + pa->PrimaryRadius;
				pt.y = pa->Point0.y;
				pt.z = pa->Point0.z;
				arrPts.Add(pt);
			}
			break;
		case DGNENTTYPE_SHAPE:
			{
				PDGNENTPOLYLINE pl = (PDGNENTPOLYLINE)EntityData.GeoData;
				for( int i=0; i<pl->nVertex; i++)
				{
					PT_3DEX pt;
					COPY_3DPT(pt,pl->pVertex[i]);
					pt.pencode = penLine;
					arrPts.Add(pt);
				}
			}
			break;
		case DGNENTTYPE_CURVE:
			{
				PDGNENTPOLYLINE pl = (PDGNENTPOLYLINE)EntityData.GeoData;
				for( int i=0; i<pl->nVertex; i++)
				{
					PT_3DEX pt;
					COPY_3DPT(pt,pl->pVertex[i]);
					pt.pencode = penSpline;
					arrPts.Add(pt);
				}
			}
			break;
		case DGNENTTYPE_POLYLINE:
			{
				PDGNENTPOLYLINE pl = (PDGNENTPOLYLINE)EntityData.GeoData;
				for( int i=0; i<pl->nVertex; i++)
				{
					PT_3DEX pt;
					COPY_3DPT(pt,pl->pVertex[i]);
					pt.pencode = penLine;
					arrPts.Add(pt);
				}
			}
			break;
		case DGNENTTYPE_TEXT:
			{
				PDGNENTTEXT pt = (PDGNENTTEXT)EntityData.GeoData;
				PT_3DEX expt;
				COPY_3DPT(expt,pt->Point0);
				expt.pencode = penLine;
				arrPts.Add(expt);

				nGeoCls = CLS_GEOTEXT;
			}
			break;
		case DGNENTTYPE_CELL:
			{
				PDGNENTCELL pp = (PDGNENTCELL)EntityData.GeoData;
				PT_3DEX pt;
				COPY_3DPT(pt,pp->Point0);
				pt.pencode = penLine;
				arrPts.Add(pt);	

				nGeoCls = CLS_GEOPOINT;
			}
			break;
		}

		CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(), nGeoCls);
		
		if (!pFtr || !pFtr->GetGeometry())   continue;

		BOOL bResult = pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		
		if (pFtr->GetGeometry()->GetDataPointSum() < 1)
		{
			delete pFtr;
			pFtr = NULL;
			continue;
		}
		
		if (nGeoCls == CLS_GEOTEXT)
		{
			if (EntityData.EntityType == DGNENTTYPE_TEXT)
			{
				PDGNENTTEXT pt = (PDGNENTTEXT)EntityData.GeoData;
				CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
				pText->SetText(pt->strText);
				TEXT_SETTINGS0 setting;
				setting.fHeight = pt->TextStyle.Height;
				setting.nAlignment = ConvertAlignmentAndDgnJustification(pt->TextStyle.JustificationType,FALSE);
				strncpy(setting.strFontName,pt->TextStyle.strFontName,LF_FACESIZE);	
				
				pText->SetSettings(&setting);
			}		
			
		}
		
		
		pFtr->GetGeometry()->SetColor(EntityData.Color);
		
		//pFtr->SetCode(strMapName);
		
		if (!pDS->AddObject(pFtr))
		{
			delete pFtr;
			pFtr = NULL;
			continue;
		}
		

		ret = dgn.FindNextElement();
	}

	dgn.Close();*/
}
/*
#include "DlgExportDGN.h"

static void SaveDGNLineString(CDGNAccess *pAccess, CGeometry *pGeoObj)
{
	if (!pGeoObj)  return;	
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeoObj->GetShape(arrPts);

	int size = arrPts.GetSize();
	if (size < 2) return;

	if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		arrPts.Add(arrPts[size-1]);

	DGNPoint *pVertex = new DGNPoint[size];
	if (!pVertex) return;

	for (int i=0; i<size; i++)
	{
		COPY_3DPT(pVertex[i],arrPts[i]);
	}

	pAccess->PolyLine(pVertex,size);

	delete []pVertex;
}
*/
void CDlgDoc::OnExportDgn() 
{
/*	CDlgExportDGN dlg;
	if (dlg.DoModal() != IDOK) return;

	CString fileName = dlg.m_strPath;

	CDGNAccess dgn;
	BOOL ret = dgn.Open(fileName);
	if (!ret) return;
	
	CListFile lstFile;
	lstFile.Open(dlg.m_strLstFile);
	
	CDlgDataSource *pDS = GetDlgDataSource();

	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	if (!pCfgLibManager)  return;
	if (!pDS) return ;
	CScheme *pScheme = pCfgLibManager->GetScheme(pDS->GetScale());
	if (!pScheme)  return;


	int lObjSum = 0;
	for(int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer||!pLayer->IsVisible())  continue;
		lObjSum += pLayer->GetObjectCount();
	}
	
	GProgressStart(lObjSum);
	
	for(i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		
		if (!pLayer ||!pLayer->IsVisible()|| pLayer->GetEditableObjsCount() <= 0)
		{
			if (pLayer)
			{
				GProgressStep(pLayer->GetObjectCount());
			}
			
			continue;
		}

		CString strLayerName = pLayer->GetName();

		//导出符号: 带符号地物导出为块
		CSchemeLayerDefine *pLayerDefine = pScheme->GetLayerDefine(strLayerName);

		LPCTSTR strMatchName = lstFile.FindMatchItem(strLayerName);
		if(strMatchName != NULL)
			strLayerName = strMatchName;

		DGNLevel Level;
		strncpy(Level.LevelName,strLayerName,LF_FACESIZE);
		Level.Color = pLayer->GetColor();
		dgn.AddLevel(&Level);

		for(int j=0; j<pLayer->GetObjectCount(); j++)
		{
			GProgressStep();
			
			CFeature *pFt = pLayer->GetObject(j);
			if (!pFt || !pFt->IsVisible())  continue;
			
			CGeometry *pGeoObj = pFt->GetGeometry();
			//点
			if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeoObj->GetShape(arrPts);
					
					if(arrPts.GetSize() > 0)
					{
						if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
						{
							double offlen = CGeoDirPoint::GetDefaultDirLen() * GetSymbolDrawScale();
							
							double ang = ((CGeoDirPoint*)pGeoObj)->GetDirection()*PI/180;
							double cosa= cos(ang), sina= sin(ang);
							PT_3DEX pt = arrPts[0], pt1; 
							pt1.x = pt.x+cosa*offlen; pt1.y = pt.y+sina*offlen; pt1.z = pt.z;
							
							dgn.Line(pt.x,pt.y,pt.z,pt1.x,pt1.y,pt1.z);
						}
						else
						{							
							double ang = 0;
							if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)))
							{
								ang = ((CGeoDirPoint*)pGeoObj)->GetDirection();
							}

							dgn.Line(arrPts[0].x,arrPts[0].y,arrPts[0].z,arrPts[0].x,arrPts[0].y,arrPts[0].z);
							
						}
						
						
					}
				}
				
			}
			//线
			else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			{
				if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
				{
					BOOL bExportSepOrCenline = FALSE;
					
					
					CGeometry *pCur1, *pCur2;
					if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
					{
						((CGeoParallel*)pGeoObj)->Separate(pCur1, pCur2);
					}
					else if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
					{
						((CGeoDCurve*)pGeoObj)->Separate(pCur1, pCur2);
					}
					
					if (pCur1 && pCur2)
					{
						CGeoCurve *plineCurve1 = (CGeoCurve*)(pCur1)->Linearize();
						CGeoCurve *plineCurve2 = (CGeoCurve*)(pCur2)->Linearize();
						SaveDGNLineString(&dgn,plineCurve1);
						SaveDGNLineString(&dgn,plineCurve2);
						
						if (plineCurve1)
						{
							delete plineCurve1;
							plineCurve1 = NULL;
						}
						
						if (plineCurve2)
						{
							delete plineCurve2;
							plineCurve2 = NULL;
						}
					}
					
					if (pCur1)
					{
						delete pCur1;
						pCur1 = NULL;
					}
					
					if (pCur2)
					{
						delete pCur2;
						pCur2 = NULL;
					}
					
					
				}
				else
				{
					CGeoCurve *pCurve = (CGeoCurve*)(pGeoObj)->Linearize();
					SaveDGNLineString(&dgn,pCurve);
					
					if (pCurve)
					{
						delete pCurve;
						pCurve = NULL;
					}
				}
			}			
			else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
			{
				//if(m_dlgDxfExport.m_bSurface)
				{
					CGeoSurface *pSur = new CGeoSurface;
					if(!pSur) return;
					CArray<PT_3DEX,PT_3DEX> arrPts;
					//if(m_dlgDxfExport.m_bLinearize)
					{
						CGeoMultiSurface *pSurface = (CGeoMultiSurface*)(pGeoObj)->Linearize();
						if(!pSurface)
						{
							delete pSur;
							return;
						}
						for (int i=pSurface->GetSurfaceNum()-1;i>=0;i--)
						{
							arrPts.RemoveAll();
							pSurface->GetSurface(i,arrPts);
							if(pSur->CreateShape(arrPts.GetData(),arrPts.GetSize()))
								SaveDGNLineString(&dgn,pSur);
						}
						delete pSurface;			
					}
					delete pSur;
				}
			}
			//面
			else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				//if(m_dlgDxfExport.m_bSurface || bDem)
				{
					//if(m_dlgDxfExport.m_bLinearize || bDem)
					{
						CGeoSurface *pSurface = (CGeoSurface*)(pGeoObj)->Linearize();
						SaveDGNLineString(&dgn,pSurface);
						
						if (pSurface)
						{
							delete pSurface;
							pSurface = NULL;
						}
						
					}
					
					
				}
			}
			//文本
			else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				//if(m_dlgDxfExport.m_bText)
				{
					CGeoText *pText = (CGeoText*)pGeoObj;
					
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeoObj->GetShape(arrPts);
					if(arrPts.GetSize() > 0)
					{
						TEXT_SETTINGS0 setting;
						pText->GetSettings(&setting);
						
						DGNTextJustification justification = (DGNTextJustification)ConvertAlignmentAndDgnJustification(setting.nAlignment,TRUE);
						dgn.Text(pText->GetText(),arrPts[0].x,arrPts[0].y,arrPts[0].z,setting.fHeight,setting.fHeight,justification);
						
					}
				}
				
			}
			// Lidar 点
			else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
			{
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeoObj->GetShape(arrPts);
				for( int t=0; t<arrPts.GetSize(); t++)
				{
					PT_3DEX pt = arrPts[t];
					dgn.Line(pt.x,pt.y,pt.z,pt.x,pt.y,pt.z);
				}
			}

			long color = pGeoObj->GetColor();
			if (color == FTRCOLOR_BYLAYER)
			{
				color = pLayer->GetColor();
			}
			dgn.SetElementColor(color);
			dgn.SetElementLevel(strLayerName);
		}
	}

	GProgressEnd();*/
}

void CDlgDoc::OnResetAllLayerCmdParams()
{
	CScheme *pScheme = gpCfgLibMan->GetScheme(GetDlgDataSource()->GetScale());
	if (!pScheme) return;
	CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(GetDlgDataSource()->GetScale());
	if( !pPlaceLib )
		return;

	m_bLoadCmdParams = FALSE;

	int nCount = pScheme->GetLayerDefineCount();

	GProgressStart(nCount);
	for( int i=0; i<nCount; i++)
	{
		GProgressStep();

		CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(i);
		int nGeoClass = pLayer->GetGeoClass();

		int nCommandID = 0;
		if (nGeoClass == CLS_GEOPOINT)
		{
			nCommandID = ID_ELEMENT_DOT_DOT;
		}
		else if (nGeoClass == CLS_GEODIRPOINT)
		{
			nCommandID = ID_ELEMENT_DOT_VECTORDOT;
		}
		else if (nGeoClass == CLS_GEOCURVE)
		{
			nCommandID = ID_ELEMENT_LINE_LINE;
		}
		else if (nGeoClass == CLS_GEODCURVE)
		{
			nCommandID = ID_ELEMENT_LINE_DLINE;
		}
		else if (nGeoClass == CLS_GEOPARALLEL)
		{
			nCommandID = ID_ELEMENT_LINE_PARALLEL;
		}
		else if (nGeoClass == CLS_GEOSURFACE)
		{
			nCommandID = ID_ELEMENT_FACE_FACE;
		}
		else if (nGeoClass == CLS_GEOTEXT)
		{
			nCommandID = ID_ELEMENT_TEXT;
		}

		if( nCommandID==0 )
			continue;

		CancelCurrentCommand();
		StartCommand(nCommandID);

		CCommand *pCmd = GetCurrentCommand();
		//存储命令的参数信息
		if ( pCmd!=NULL && m_nCurCmdCreateID!=0)
		{
			CValueTable tab;
			
			tab.BeginAddValueItem();
			pCmd->GetParams(tab);
			tab.EndAddValueItem();
			
			CMarkup xml;
			xml.AddElem("Data");
			xml.IntoElem();
			Xml_WriteValueTable(xml,tab);
			xml.OutOfElem();

			CString str = xml.GetDoc();

			CPlaceConfig place;
			strncpy(place.m_strLayName,pLayer->GetLayerName(),sizeof(place.m_strLayName)-1);
			place.m_nCommandID = m_nCurCmdCreateID;
			strncpy(place.m_strCmdParams,str,sizeof(place.m_strCmdParams)-1);
			
			CPlaceConfig* pConfig = pPlaceLib->GetConfig(pLayer->GetLayerName());
			if (pConfig)
			{
				*pConfig = place;
			}
			else
				pPlaceLib->AddConfig(place);
		}

	}

	pPlaceLib->Save();

	GProgressEnd();

	m_bLoadCmdParams = TRUE;
}


void CDlgDoc::OnSelectLocate()
{
	CMainFrame *pMainFrame = ((CMainFrame*)AfxGetMainWnd());
	CDlgNextLast *pDlg = &pMainFrame->m_dlgSelectLocate;

	BOOL bFirst = FALSE;
	if( pDlg->GetSafeHwnd()==NULL )
	{
		pDlg->Create(IDD_NEXT_LAST);
		bFirst = TRUE;
	}

	pDlg->Init(this);
	pDlg->ShowWindow(SW_SHOW);
	
	if( bFirst )
	{
		CRect rcClient;
		GetWindowRect(pMainFrame->m_hWndMDIClient,rcClient);
		//pMainFrame->ScreenToClient(&rcClient);
		
		CRect rcDlg;
		pDlg->GetWindowRect(&rcDlg);
		//pMainFrame->ScreenToClient(&rcDlg);
		
		rcDlg.OffsetRect(rcClient.left-rcDlg.left,rcClient.top-rcDlg.top);
		
		pDlg->MoveWindow(&rcDlg);

		pDlg->OnButtonNext();
	}
}

//2015.3.6
void CDlgDoc::OnLayerLocate()
{
	CMainFrame *pMainFrame = ((CMainFrame*)AfxGetMainWnd());
	CDlgLocateFtrs *pDlg = &pMainFrame->m_dlfFtrsLocate;
	
	BOOL bFirst = FALSE;
	if( pDlg->GetSafeHwnd()==NULL )
	{
		pDlg->Create(IDD_LOCATE_FTRS);
		bFirst = TRUE;
	}
	
	pDlg->Init(this);
	pDlg->ShowWindow(SW_SHOW);
	
	if( bFirst )
	{
		CRect rcClient;
		GetWindowRect(pMainFrame->m_hWndMDIClient,rcClient);
		//pMainFrame->ScreenToClient(&rcClient);
		
		CRect rcDlg;
		pDlg->GetWindowRect(&rcDlg);
		//pMainFrame->ScreenToClient(&rcDlg);
		
		rcDlg.OffsetRect(rcClient.left-rcDlg.left,rcClient.top-rcDlg.top);
		
		pDlg->MoveWindow(&rcDlg);
	}
}


//2013.09.23
#include "DlgRepairPTZText.h"

void CDlgDoc::OnRepairOnePtZText()
{
	BeginCheck41License

	CDlgRepairPTZText dlg;
	if( IDOK != dlg.DoModal() )
		return;
	int sel_opt = dlg.m_sel; //0表示将注记值赋给高程点，1表示与之相反

	ChkResultItem *pItem = ((CMainFrame*)AfxGetMainWnd())->m_wndCheckResultBar.GetCurItem();
	if( pItem==NULL )
		return;

	CString chkName = StrFromResID( IDS_CHK_CMD_Z_ANNO_CONSISTENCY );
	if( pItem->state!=0 || pItem->arrFtrs.GetSize()!=1 || pItem->chkCmd.CompareNoCase(chkName)!=0 )
		return;
	
	CUndoFtrs undo(this,"Repair Point Annotation");	
	CFeature *pFtr = pItem->arrFtrs[0];	//高程点
	DoRepairAnno(pFtr, undo, sel_opt);
	undo.DelInvalidUndoInfo();
	undo.Commit();		  
	RefreshView();

	EndCheck41License
}

void CDlgDoc::OnRepairAllPtZText()
{
	BeginCheck41License
		
	CDlgRepairPTZText dlg;
	if( IDOK != dlg.DoModal() )
		return;
	int sel_opt = dlg.m_sel;

	CChkResManger *pChkRes = &GetChkResMgr();		
	
	CString chkName = StrFromResID(IDS_CHK_CMD_Z_ANNO_CONSISTENCY);
	CArray<CFeature*,CFeature*> arrFtrs;

	for( int i=0; i<pChkRes->GetChkResCount(); i++ )
	{
		ChkResultItem *pItem = pChkRes->GetChkResByIdx(i);
		if( pItem->chkCmd.CompareNoCase(chkName)!=0 )
			continue;

		if( pItem->state!=0 )
			continue;

		if( pItem->arrFtrs.GetSize()!=1 )
			continue;

		if( pItem->arrFtrs[0]->IsDeleted() )
			continue;

		arrFtrs.Append(pItem->arrFtrs);
	}

	if( arrFtrs.GetSize()<=0 )
		return;


	CUndoFtrs undo(this,"Repair Point Annotation");	

	GProgressStart(arrFtrs.GetSize());
	for( i=0; i<arrFtrs.GetSize(); i++ )
	{
		GProgressStep();
		CFeature* pFtr = arrFtrs.GetAt(i);
		DoRepairAnno(pFtr, undo, sel_opt);
	}
	GProgressEnd();

	undo.DelInvalidUndoInfo();
	undo.Commit();
	RefreshView();	

	EndCheck41License
}

void CDlgDoc::DoRepairAnno(CFeature* pFtr,CUndoFtrs& undo,int flag)
{
	if(!pFtr)
		return;

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pFtr->GetGeometry()->GetShape( arrPts );
	if(arrPts.GetSize() < 1)
		return;
	PT_3DEX pt = arrPts.GetAt(0);

	//搜索最近的注记
	CDataQuery* pDQ = GetDataQuery();
	struct SearchNearest sr;
	sr.pt = pt;
	sr.r = 4;
	(pDQ->m_arrFoundHandles).RemoveAll();
	int num = pDQ->FindObject( &sr );
	if( num <= 0 )
		return;

	CFeature* pFtrFound = NULL;
	double disMin = -1;
	for(int i = 0; i < (pDQ->m_arrFoundHandles).GetSize(); i++)
	{
		CFeature* pFtr = (pDQ->m_arrFoundHandles).GetAt(i);
		if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			continue;

		CArray<PT_3DEX,PT_3DEX> pts;
		pFtr->GetGeometry()->GetShape( pts );
		PT_3DEX pt0 = pts.GetAt(0);

		double dis = GraphAPI::GGet2DDisOf2P(pt, pt0);
		if( -1 == disMin || dis < disMin )
		{
			disMin = dis;
			pFtrFound = pFtr;
		}
	}

	if(!pFtrFound)		//如果注记不存在，则不作处理
		return;

	//修改
	if( pFtrFound->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		CFeature* pNewText = pFtrFound->Clone();
		CString strText = ((CGeoText*)pNewText->GetGeometry())->GetText();			
		
		if( 0 == flag )
		{//将注记值赋给高程点
			CFeature* pNew = pFtr->Clone();
			PT_3DEX ptn = pt;
			ptn.z = atof(strText);
			pNew->GetGeometry()->SetDataPoint(0, ptn);

			if(AddObject(pNew,GetFtrLayerIDOfFtr(FtrToHandle(pFtr))))
			{								
				undo.AddNewFeature(FtrToHandle(pNew));
			}
			if(DeleteObject(FtrToHandle(pFtr)))
			{
				undo.AddOldFeature(FtrToHandle(pFtr));	
			}
		}
		else
		{
			int nLenText = strText.GetLength();
			int nPrecision = nLenText-strText.ReverseFind('.')-1;
			CString strPrecision;
			strPrecision.Format("%d",nPrecision);
			double lfZValue = pt.z;
			CString strZValue;
			sprintf(strZValue.GetBuffer(nLenText),"%."+strPrecision+"f",lfZValue);

			((CGeoText*)pNewText->GetGeometry())->SetText( strZValue );

			if(AddObject(pNewText,GetFtrLayerIDOfFtr(FtrToHandle(pFtrFound))))
			{								
				undo.AddNewFeature(FtrToHandle(pNewText));
			}
			if(DeleteObject(FtrToHandle(pFtrFound)))
			{
				undo.AddOldFeature(FtrToHandle(pFtrFound));	
			}
		}
	}

}

void CDlgDoc::OnDlgOutputLayerCheck()
{
	if(m_OutputLayerCheckDlg)  { delete m_OutputLayerCheckDlg; }
	m_OutputLayerCheckDlg = new CDlgOutputLayerCheck();
	m_OutputLayerCheckDlg->m_pDoc = this;
	m_OutputLayerCheckDlg->Create(IDD_FILE_OUTPUT_LAYERCHECK, AfxGetMainWnd());
	m_OutputLayerCheckDlg->ShowWindow(SW_SHOW);
}

void CDlgDoc::OnDlgSetColor()
{
	if(m_SetColorDlg)  { delete m_SetColorDlg; }
	m_SetColorDlg = new CDlgSetColor();
	m_SetColorDlg->Create(IDD_SETCOLOR_BYFIELD, AfxGetMainWnd());
	m_SetColorDlg->ShowWindow(SW_SHOW);
	m_SetColorDlg->Init(this);
}


void CDlgDoc::MakeCheckPtSample()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		ASSERT_VALID(pView);
		
		if (pView->IsKindOf(RUNTIME_CLASS(CStereoView))) 
		{
			((CStereoView*)pView)->OnMakeCheckPtSample();
		}
	}
}

void CDlgDoc::OnDlgAnnoTip()
{
	if(m_pDlgAnnotip)
	{
		m_pDlgAnnotip->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDlgAnnotip = new CDlgAnnoTip();
		m_pDlgAnnotip->Create(IDD_ANNOTIP, NULL);
		m_pDlgAnnotip->ShowWindow(SW_SHOW);
	}
}

void CDlgDoc::OnFillRandColor()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	if(!pDS) return;
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if( !pScheme ) return;
	srand((unsigned)time(NULL));

	int nlayer = pDS->GetFtrLayerCount(), i, j, nSum=0;
	for(i=0; i<nlayer; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer || !pLayer->IsVisible()) continue;
		nSum += pLayer->GetValidObjsCount();
	}

	int transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", 50);
	
	CUndoModifyProperties undo(this,"FillRandColor");
	GProgressStart(nSum);
	for(i=0; i<nlayer; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer|| !pLayer->IsVisible() || pLayer->GetValidObjsCount()<=0)
			continue;

		CSchemeLayerDefine *pDefLayer = pScheme->GetLayerDefine( pLayer->GetName() );
		if( pDefLayer)
		{
			int nSym = pDefLayer->GetSymbolCount();
			for(j=0; j<nSym; j++)
			{
				int type = pDefLayer->GetSymbol(j)->GetType();
				if(type==SYMTYPE_CELLHATCH || type==SYMTYPE_COLORHATCH)
				{
					break;
				}
			}
			if(j<nSym) continue;//有填充符号的就不再填充颜色
		}
		
		COLORREF color = RGB(rand()%256, rand()%256, rand()%256);

		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr) continue;

			CGeometry *pObj = pFtr->GetGeometry();
			if(!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;

			CGeoCurveBase *pCB = (CGeoCurveBase*)pObj;
			if(!pCB->IsClosed() || pCB->IsFillColor())
				continue;
			
			undo.arrHandles.Add(FtrToHandle(pFtr));

			CValueTable tab2;
			tab2.BeginAddValueItem();
			pFtr->WriteTo(tab2);	
			tab2.EndAddValueItem();
			tab2.DelField(FIELDNAME_SHAPE);
			tab2.DelField(FIELDNAME_SHAPE);
			tab2.DelField(FIELDNAME_GEOCURVE_CLOSED);
			tab2.DelField(FIELDNAME_FTRDELETED);
			tab2.DelField(FIELDNAME_FTRVISIBLE);
			tab2.DelField(FIELDNAME_CLSTYPE);
			
			undo.oldVT.AddItemFromTab(tab2);
			
			pCB->EnableFillColor(TRUE, color);
			pCB->SetTransparency(transparency);
			
			CString symName = pObj->GetSymbolName();
			
			if (tab2.SetValue(0,FIELDNAME_SYMBOLNAME,&CVariantEx((_variant_t)(const char*)symName)))
			{
				undo.newVT.AddItemFromTab(tab2);
			}
		}
	}
	GProgressEnd();

	undo.Commit();
	UpdateAllViews(NULL, hc_UpdateAllObjects);
}


void CDlgDoc::SetFtrsVisibleBySwitchStates(const CRuntimeClass* pGeoClass, BOOL bVisible)
{
	CDlgDataSource *pDS = GetDlgDataSource();
	
	//获取实体对象总数以及点的数目
	CGeometry *pGeo = NULL;
	long lObjSum = 0;
	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		for(int j=0; j<nObjs; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
			if (!pFtr) continue;
			pGeo = pFtr->GetGeometry();
			if( !pGeo )continue;

			if( !pGeo->IsKindOf(pGeoClass) )
				continue;
			
			lObjSum++;
		}
	}	
	
//	BeginBatchUpdate();

	GProgressStart(lObjSum);
	
	for (i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		for(int j=0; j<nObjs; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
			if (!pFtr) continue;
			pGeo = pFtr->GetGeometry();
			if( !pGeo )continue;

			if( !pGeo->IsKindOf(pGeoClass) )
				continue;
			
			GProgressStep();

//			DeleteObject(FtrToHandle(pFtr),FALSE);

			pFtr->EnableVisible(bVisible);

//			RestoreObject(FtrToHandle(pFtr));
		}
	}

//	EndBatchUpdate();	

	GProgressEnd();

	UpdateAllViews(NULL, hc_UpdateAllObjects_Visible);
}

void CDlgDoc::OnSwitchPoint()
{
	m_bViewPoint = !m_bViewPoint;

	SetFtrsVisibleBySwitchStates(RUNTIME_CLASS(CGeoPoint),m_bViewPoint);
}


void CDlgDoc::OnSwitchLine()
{
	m_bViewCurve = !m_bViewCurve;
	SetFtrsVisibleBySwitchStates(RUNTIME_CLASS(CGeoCurve),m_bViewCurve);
}

void CDlgDoc::OnSwitchSurface()
{
	m_bViewSurface = !m_bViewSurface;

	SetFtrsVisibleBySwitchStates(RUNTIME_CLASS(CGeoSurface),m_bViewSurface);
}

void CDlgDoc::OnSwitchText()
{
	m_bViewText = !m_bViewText;

	SetFtrsVisibleBySwitchStates(RUNTIME_CLASS(CGeoText),m_bViewText);
}

void CDlgDoc::OnUpdateSwitchPoint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bViewPoint);
}

void CDlgDoc::OnUpdateSwitchLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bViewCurve);

}

void CDlgDoc::OnUpdateSwitchSurface(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bViewSurface);

}

void CDlgDoc::OnUpdateSwitchText(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bViewText);

}


void CDlgDoc::GetFtrsNotGIS(CFtrArray& ftrs)
{
	CDlgDataSource* pDS = GetDlgDataSource();	
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());

	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = NULL;

	ftrs.RemoveAll();
	
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||pLayer->IsLocked()||!pLayer->IsVisible()||pLayer->IsEmpty())
			continue;
		int nSize = pLayer->GetObjectCount();
		if (nSize<=0)
			continue;

		CFeature* pTfeature = NULL;		
		for( int n=0; n<nSize; n++)
		{
			pTfeature = pLayer->GetObject(n,FILTERMODE_DELETED);
			if (pTfeature)break;
		}
		if( !pTfeature )
			continue;
		
		int nClass = pTfeature->GetGeometry()->GetClassType();
		BOOL IsDBLayerNameEmpty = TRUE;

		//使用层方案配置中的入库几何体类型
		if(pScheme)
		{
			CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(pLayer->GetName());
			if(pDef)
			{
				nClass = pDef->GetDbGeoClass();
				CString DBLayerName = pDef->GetDBLayerName();
				IsDBLayerNameEmpty = DBLayerName.IsEmpty();
			}
		}

		int nEsriClass = CLS_NULL;

		switch(nClass)
		{
		case CLS_GEOPOINT:
		case CLS_GEODIRPOINT:
		case CLS_GEOSURFACEPOINT:
			nEsriClass = CLS_GEOPOINT;
			break;
		case CLS_GEOCURVE:
		case CLS_GEODCURVE:
		case CLS_GEOPARALLEL:
			nEsriClass = CLS_GEOCURVE;
			break;
		case CLS_GEOMULTISURFACE:
		case CLS_GEOSURFACE:
			nEsriClass = CLS_GEOSURFACE;
			break;
		default:
			break;
		}

		for (int j=0;j<pLayer->GetObjectCount();j++)
		{
			pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
			if(!pFtr)continue;

			if(IsDBLayerNameEmpty)
			{
				ftrs.Add(pFtr);
			}

			GProgressStep();
			int nCls = pFtr->GetGeometry()->GetClassType();

			BOOL bSuc = FALSE;
			switch(nEsriClass)
			{
			case CLS_GEOPOINT:
				if(!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
				{
					ftrs.Add(pFtr);
				}
				break;
			case CLS_GEOCURVE:
				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					ftrs.Add(pFtr);
				}
				break;
			case CLS_GEOSURFACE:
				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					ftrs.Add(pFtr);
				}
				break;
			default:	
				ftrs.Add(pFtr);
			}			
		}
	}	
}


void CDlgDoc::HideFtrsNotGIS(BOOL bHide)
{
	CFtrArray ftrs;
	GetFtrsNotGIS(ftrs);

	if(bHide)
	{
		for (int j=0;j<ftrs.GetSize();j++)
		{
			ftrs[j]->EnableVisible(FALSE);
		}		
	}
	else
	{
		for (int j=0;j<ftrs.GetSize();j++)
		{
			CGeometry *pGeo = ftrs[j]->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				ftrs[j]->EnableVisible(m_bViewPoint);
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			{
				ftrs[j]->EnableVisible(m_bViewCurve);
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				ftrs[j]->EnableVisible(m_bViewSurface);
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				ftrs[j]->EnableVisible(m_bViewText);
			}
			else
			{
				ftrs[j]->EnableVisible(TRUE);
			}
		}
	}

	UpdateAllViews(NULL, hc_UpdateAllObjects);
}


//隐藏不入库的数据，该功能与 OnExportArcgisMDB 强相关
void CDlgDoc::OnHideFtrsNotGIS()
{
	m_bHideFtrsNotGIS = !m_bHideFtrsNotGIS;
	HideFtrsNotGIS(m_bHideFtrsNotGIS);
}


void CDlgDoc::OnUpdateHideFtrsNotGIS(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bHideFtrsNotGIS);
}

//曾进翀2017/8/6
void CDlgDoc::OnSetOsgbExtent()
{
	COsgbView* pOsgbView = getOsgbView();

	if (pOsgbView == nullptr)
	{
		return;
	}
	
	Envelope e = pOsgbView->getOsgbExtent();
	SetBound(e);

	// 重置视图位置
	pOsgbView->SendMessage(WM_COMMAND, ID_OSGBVIEW_HOME, 0);
}


void CDlgDoc::OnTopView()
{
	// TODO: Add your command handler code here
	COsgbView* pOsgbView = getOsgbView();

	if (pOsgbView == nullptr)
	{
		return;
	}

	pOsgbView->SendMessage(WM_TOP_VIEW, 0, 0);
}

void CDlgDoc::OnLeftView()
{
	// TODO: Add your command handler code here
	COsgbView* pOsgbView = getOsgbView();

	if (pOsgbView == nullptr)
	{
		return;
	}

	pOsgbView->SendMessage(WM_LEFT_VIEW, 0, 0);
}

COsgbView* CDlgDoc::getOsgbView()
{
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			COsgbView* pOsgbView = static_cast<COsgbView*>(pView);

			if (pOsgbView == nullptr)
			{
				continue;
			}

			return pOsgbView;
		}
	}

	return nullptr;
}


void CDlgDoc::UpdateSwitchStates()
{
	CDlgDataSource *pDS = GetDlgDataSource();
	
	//获取实体对象总数以及点的数目
	CGeometry *pGeo = NULL;
	int nPointNum0 = 0, nLineNum0 = 0, nSurfaceNum0 = 0, nTextNum0 = 0;
	int nPointNum1 = 0, nLineNum1 = 0, nSurfaceNum1 = 0, nTextNum1 = 0;
	
	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		if( !pLayer->IsVisible() )continue;
		
		int nObjs = pLayer->GetObjectCount();
		for(int j=0; j<nObjs; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr) continue;
			pGeo = pFtr->GetGeometry();
			if( !pGeo )continue;

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				nPointNum0++;
				if( pFtr->IsVisible() )
					nPointNum1++;
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			{
				nLineNum0++;
				if( pFtr->IsVisible() )
					nLineNum1++;
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				nSurfaceNum0++;
				if( pFtr->IsVisible() )
					nSurfaceNum1++;
			}
			else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				nTextNum0++;
				if( pFtr->IsVisible() )
					nTextNum1++;
			}
		}
	}

	m_bViewPoint = m_bViewCurve = m_bViewSurface = m_bViewText = TRUE;
	
	if(nPointNum1<nPointNum0/2)
		m_bViewPoint = FALSE;

	if(nLineNum1<nLineNum0/2)
		m_bViewCurve = FALSE;

	if(nSurfaceNum1<nSurfaceNum0/2)
		m_bViewSurface = FALSE;

	if(nTextNum1<nTextNum0/2)
		m_bViewText = FALSE;
}




void CDlgDoc::UpdateFlagOfHideFtrsNotGIS()
{
	CFtrArray ftrs;
	GetFtrsNotGIS(ftrs);

	int count = 0;

	for (int j=0;j<ftrs.GetSize();j++)
	{
		if(ftrs[j]->IsVisible())
			count++;
	}

	if(count<ftrs.GetSize()/2)
	{
		m_bHideFtrsNotGIS = TRUE;
	}
	else
	{
		m_bHideFtrsNotGIS = FALSE;
	}
}

BOOL CDlgDoc::DeleteCurVertexs()
{
	CSelection *pSel = GetSelection();
	if(!pSel) return FALSE;
	int nObj;
	const FTR_HANDLE *ftrs = pSel->GetSelectedObjs(nObj);
	if(!ftrs || nObj<=0) return FALSE;

	CView *pView = GetActiveView();
	if(!pView || !pView->IsKindOf(RUNTIME_CLASS(CBaseView)))
		return FALSE;
	PT_3D pt = ((CBaseView*)pView)->GetCurPoint();
	
	CUndoBatchAction batchUndo(this,"DeleteVertexs");
	CFeature *pFtr = HandleToFtr(ftrs[0]);
	CGeometry *pGeo = pFtr->GetGeometry();
	if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		return FALSE;
		
	CArray<PT_3DEX,PT_3DEX> pts;
	pGeo->GetShape(pts);
	for(int i=0; i<pts.GetSize(); i++)
	{
		PT_3D pt1 = pts[i];
		if(!GraphAPI::GIsEqual2DPoint(&pt, &pt1))
			continue;
		if( CDeleteVertexCommand::DeleteVertex(this,pFtr,i,batchUndo,"DeleteVertexs") )
		{
			batchUndo.Commit();
			DeselectAll();
			OnSelectChanged(TRUE);	
			UpdateDrag(ud_ClearDrag);
			GOutPut(StrFromResID(IDS_CMDPLANE_DELVERTEX));
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return FALSE;
}

BOOL CDlgDoc::DeleteCurSegments()
{
	CSelection *pSel = GetSelection();
	if(!pSel) return FALSE;
	int nObj;
	pSel->GetSelectedObjs(nObj);
	if(nObj<=0) return FALSE;
	
	CView *pView = GetActiveView();
	if(!pView || !pView->IsKindOf(RUNTIME_CLASS(CBaseView)))
		return FALSE;

	PT_3D pt = ((CBaseView*)pView)->GetCurPoint();
	Envelope e;
	e.CreateFromPtAndRadius(pt, 0.01);
	
	m_pDataQuery->FindObjectInRect(e,NULL,FALSE,FALSE);
	
	int num, i, j, k, sum=0;
	const CPFeature *ftrs = m_pDataQuery->GetFoundHandles(num);
	if(num<=0) return FALSE;
	
	CUndoFtrs undo(this,"DeleteSegments");
	for(i=0; i<num; i++)
	{
		if(!pSel->IsObjInSelection(FtrToHandle(ftrs[i])))
			continue;
		CGeometry *pGeo = ftrs[i]->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			continue;
		
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);
		for(j=0; j<pts.GetSize()-1; j++)
		{
			PT_3D pt0 = pts[j];
			PT_3D pt1 = pts[j+1];
			if(GraphAPI::GIsEqual2DPoint(&pt, &pt1))
			{
				continue;
			}
			if(GraphAPI::GIsPtInLine(pt0, pt1, pt))
			{
				CGeometry *objs[3]={NULL};
				((CGeoCurveBase*)pGeo)->GetBreak(pt0,pt1,objs[0],objs[1],objs[2],TRUE);
				if( objs[1] )
				{
					delete objs[1];
					objs[1] = NULL;
				}
				
				for( k=0; k<3; k++)
				{
					CGeometry *pObj = objs[k];
					if( pObj )
					{
						CFeature *pFtr0 = ftrs[i]->Clone();
						pFtr0->SetID(OUID());
						if (pFtr0)
						{
							pFtr0->SetGeometry(pObj);
						}
						AddObject(pFtr0,GetFtrLayerIDOfFtr(FtrToHandle(ftrs[i])));
						undo.AddNewFeature(FtrToHandle(pFtr0));
						GETXDS(this)->CopyXAttributes(ftrs[i], pFtr0);
					}
				}
				
				undo.AddOldFeature(FtrToHandle(ftrs[i]));
				DeleteObject(FtrToHandle(ftrs[i]), FALSE);
				sum++;
			}
		}
	}

	if(sum>0)
	{
		undo.Commit();
		DeselectAll();
		OnSelectChanged(TRUE);	
		UpdateDrag(ud_ClearDrag);
		GOutPut(StrFromResID(IDS_DELSEGMENT));
		return TRUE;
	}

	return FALSE;
}

BOOL CDlgDoc::BreakByPt()
{
	CSelection *pSel = GetSelection();
	if(!pSel) return FALSE;
	int nObj;
	pSel->GetSelectedObjs(nObj);
	if(nObj<=0) return FALSE;
	
	CView *pView = GetActiveView();
	if(!pView || !pView->IsKindOf(RUNTIME_CLASS(CBaseView)))
		return FALSE;

	PT_3D pt = ((CBaseView*)pView)->GetCurPoint();
	Envelope e;
	e.CreateFromPtAndRadius(pt, 0.01);
	
	m_pDataQuery->FindObjectInRect(e,NULL,FALSE,FALSE);
	
	int num, i, j, sum=0;
	const CPFeature *ftrs = m_pDataQuery->GetFoundHandles(num);
	if(num<=0) return FALSE;
	
	CUndoFtrs undo(this,"Break");
	for(i=0; i<num; i++)
	{
		if(!pSel->IsObjInSelection(FtrToHandle(ftrs[i])))
			continue;
		CGeometry *pGeo = ftrs[i]->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			continue;

		CGeometry *objs[3]={NULL};
		((CGeoCurveBase*)pGeo)->GetBreak(pt,pt,objs[0],objs[1],objs[2],TRUE);
		if( objs[1] )
		{
			delete objs[1];
			objs[1] = NULL;
		}

		for( j=0; j<3; j++)
		{
			CGeometry *pObj = objs[j];
			if( pObj )
			{
				CFeature *pFtr0 = ftrs[i]->Clone();
				pFtr0->SetID(OUID());
				if (pFtr0)
				{
					pFtr0->SetGeometry(pObj);
				}
				AddObject(pFtr0,GetFtrLayerIDOfFtr(FtrToHandle(ftrs[i])));
				undo.AddNewFeature(FtrToHandle(pFtr0));
				GETXDS(this)->CopyXAttributes(ftrs[i], pFtr0);
			}
		}

		undo.AddOldFeature(FtrToHandle(ftrs[i]));
		DeleteObject(FtrToHandle(ftrs[i]), FALSE);
		sum++;
	}

	if(sum>0)
	{
		undo.Commit();
		DeselectAll();
		OnSelectChanged(TRUE);	
		UpdateDrag(ud_ClearDrag);
		GOutPut(StrFromResID(IDS_CMDNAME_BREAK));
		return TRUE;
	}

	return FALSE;
}

CFtrsHighliteDisplay::CFtrsHighliteDisplay()
{
	m_pDoc = NULL;
}


CFtrsHighliteDisplay::~CFtrsHighliteDisplay()
{
}


void CFtrsHighliteDisplay::SetDoc(CDlgDoc *pDoc)
{
	m_pDoc = pDoc;
}


void CFtrsHighliteDisplay::Add(CFeature *pFtr)
{
	if(pFtr==NULL)
		return;

	m_arrFtrs.Add(pFtr);
}


void CFtrsHighliteDisplay::Highlite()
{
	GrBuffer allBuf;
	for(int i=0; i<m_arrFtrs.GetSize(); i++)
	{
		CFeature *pFtr = m_arrFtrs[i];

		GrBuffer buf;
		pFtr->Draw(&buf);

		allBuf.AddBuffer(&buf);
	}

	allBuf.SetAllLineWidth(FALSE,2.0);
	allBuf.SetAllColor(RGB(255,0,0));

	m_pDoc->UpdateAllViews(NULL,hc_SetConstDragLine,(CObject*)&allBuf);
}


void CFtrsHighliteDisplay::ClearHighlite()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(pDoc)
		pDoc->UpdateAllViews(NULL,hc_SetConstDragLine,NULL);
}

void CDlgDoc::OnNonLocalLayersRename()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return;

	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (!pDS) return;

	DWORD scale = pDS->GetScale();
	CScheme *pScheme = gpCfgLibMan->GetScheme(scale);
	if (!pScheme) return;

	int i, j, nSum = 0;
	for (i = 0; i < pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer || !pLayer->IsVisible()) continue;
		nSum += pLayer->GetObjectCount();
	}

	CUndoBatchAction batchUndo(pDoc, "NonLocalLayersRename");
	pDoc->BeginBatchUpdate();
	GProgressStart(nSum);
	for (i = 0; i < pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer || !pLayer->IsVisible()) continue;

		int nObj = pLayer->GetObjectCount();
		if (nObj <= 0) continue;

		CString strLayerName = pLayer->GetName();
		CString strDBName;

		CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(strLayerName);
		if (pDef) strDBName = pDef->GetDBLayerName();
		if (strDBName.IsEmpty()) continue;

		CFtrLayer *pLayer1 = pDS->GetFtrLayer(strDBName);
		if (!pLayer1)
		{
			pLayer1 = new CFtrLayer();
			pLayer1->SetName(strDBName);
			pDoc->AddFtrLayer(pLayer1);
		}

		CFtrArray arr;

		for (j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;
			arr.Add(pFtr);
		}

		for (j = 0; j < nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = arr[j];
			CString strcode = pFtr->GetCode();
			if (!strcode.IsEmpty())
			{
				CUndoModifyLayerOfObj undo(pDoc, "ModifyLayerOfObj");

				undo.oldLayerArr.Add(pLayer->GetID());
				pDS->SetFtrLayerOfObject(pFtr, pLayer1->GetID());
				undo.newLayerArr.Add(pLayer1->GetID());

				batchUndo.AddAction(&undo);
			}
		}
	}
	GProgressEnd();
	batchUndo.Commit();
	pDoc->EndBatchUpdate();

	gpMainWnd->m_wndFtrLayerView.Refresh();
}

void CDlgDoc::OnSelectToChecklist()
{
	CSelection *pSelect = GetSelection();
	int num;
	const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(num);
	if (num < 0) return;

	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT, 1, 0);

	CChkResManger *pChkRes = &GetChkResMgr();
	CString chkName("selected");
	CString chkReason("selected");

	for (int i = 0; i < num; i++)
	{
		CFeature *pFtr = (CFeature*)ftrs[i];
		if (!pFtr) continue;
		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pFtr) continue;

		pChkRes->BeginResItem(chkName);
		pChkRes->AddAssociatedFtr(pFtr);
		pChkRes->SetAssociatedPos(pGeo->GetDataPoint(0));
		pChkRes->SetReason(chkReason);
		pChkRes->EndResItem();
	}

	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT, 0, 0);
}


void CDlgDoc::OnOpenOsgb()
{
	// TODO: Add your command handler code here
	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("3D model Files (*.osgb, *.osg, *.obj)|*.osgb;*.osg;*.obj;*.osgt|All Files (*.*)|*.*||"), NULL);

	if (dialog.DoModal() == IDOK)
	{
		string fileName = dialog.GetPathName();
		COsgbView* pView = getOsgbView();

		if (pView != nullptr)
		{
			pView->addScene(fileName);
		}
	}
}


void CDlgDoc::OnRemoveOsgb()
{
	// TODO: Add your command handler code here
	COsgbView* pView = getOsgbView();

	if (pView != nullptr)
	{
		pView->removeAllScene();
	}
}
