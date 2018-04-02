// CheckResultViewBar.cpp: implementation of the CCheckResultViewBar class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "EditBase.h"

#include "CheckResultViewBar.h"
#include "DlgDataSource.h "
#include "editbasedoc.h "
#include "ExMessage.h "
#include "Feature.h "
#include "DlgCheckList.h"

#include "DlgCommand.h"
#include "DlgMarkIconsSetting.h"
#include "CheckScheme.h"

#include "Markup.h"
#include "dxfAccess.h"
#include "DlgSelectShift.h"

#define			XMLTAG_SETTINGS				_T("Settings")
#define			XMLTAG_ICONS				_T("Icons")
#define			XMLTAG_ICONNAME_MAP			_T("IconNameMap")
#define			XMLTAG_ICONNAME_MAPITEM		_T("IconNameMapItem")
#define			XMLTAG_CHKCMD				_T("ChkCmd")
#define			XMLTAG_REASON				_T("Reason")
#define			XMLTAG_ICONNAME				_T("IconName")
#define			XMLTAG_ICONSIZE				_T("IconSize")
#define			XMLTAG_COLOR				_T("Color")
#define   MESSAGE_CLICKICON   WM_USER+0x1001

namespace
{
	
	CDocument *GetCurDocument()
	{
		CWnd *pWnd = AfxGetMainWnd();
		if( pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
		{
			CMDIFrameWnd *pMain = (CMDIFrameWnd*)pWnd;
			CMDIChildWnd *pChild= pMain->MDIGetActive();
			if( pChild )
			{
				CView *pView = pChild->GetActiveView();
				if( pView )
					return pView->GetDocument();
			}
		}
		else if( pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
		{
			CFrameWnd *pMain = (CFrameWnd*)pWnd;
			return pMain->GetActiveDocument();
		}
		return NULL;
	}
	
	
	
	CFeature* MarkErrorPt(CDlgDoc *pDoc, ChkResultItem *pItem, double lfMarkWid )
	{
		CFtrLayer *pLayer = pDoc->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_MARKLAYER_NAME));
		if( !pLayer )
		{
			pLayer = pDoc->GetDlgDataSource()->CreateFtrLayer(StrFromResID(IDS_MARKLAYER_NAME));
			
			if(!pDoc->AddFtrLayer(pLayer)) return NULL;
		}			
		pLayer->EnableLocked(TRUE);
		CFeature *pFtr = pLayer->CreateDefaultFeature(pDoc->GetDlgDataSource()->GetScale(),CLS_GEODCURVE);
		if( !pFtr )
		{
			return NULL;
		}
		
		CGeometry *pObj = pFtr->GetGeometry();
		
		if (!pObj)
		{
			return NULL;
		}
		
		pObj->SetColor(RGB(255,0,0));		
		
		//	pFtr->SetFID(StrFromResID(IDS_DEFLAYER_ERROR));
		
		CArray<PT_3DEX,PT_3DEX> pts;
		PT_3D pt = pItem->pos;
		PT_3DEX expt;
		COPY_3DPT(expt,pt);
		
		expt.x = pt.x-lfMarkWid;  expt.y = pt.y-lfMarkWid; expt.pencode = penMove;
		pts.Add(expt);
		expt.x = pt.x+lfMarkWid;  expt.y = pt.y+lfMarkWid; expt.pencode = penLine;
		pts.Add(expt);
		
		//移动中心点
		// 	expt.x = pt.x; expt.y = pt.y; expt.pencode = penLine;
		// 	pts.Add(expt);
		
		expt.x = pt.x-lfMarkWid;  expt.y = pt.y+lfMarkWid; expt.pencode = penMove;
		pts.Add(expt);
		expt.x = pt.x+lfMarkWid;  expt.y = pt.y-lfMarkWid; expt.pencode = penLine;
		pts.Add(expt);
		
		pObj->CreateShape(pts.GetData(),pts.GetSize());
		
		pDoc->AddObject(pFtr,pLayer->GetID());

		pItem->pFtrMark = pFtr;
		
		return pFtr;
	}
	
}


CString CChkResSettings::GetIcon(LPCTSTR cmd, LPCTSTR reason)
{
	for( int i=0; i<m_arrChkIcons.GetSize(); i++)
	{
		ChkIcon item = m_arrChkIcons[i];
		if( stricmp(cmd,item.cmd)==0 && CompareReason(item.reason,reason) )
		{
			return item.icon;
		}
	}

	return CString();
}



COLORREF CChkResSettings::GetColor(LPCTSTR cmd, LPCTSTR reason)
{
	for( int i=0; i<m_arrChkIcons.GetSize(); i++)
	{
		ChkIcon item = m_arrChkIcons[i];
		if( stricmp(cmd,item.cmd)==0 && CompareReason(item.reason,reason) )
		{
			return item.color;
		}
	}
	
	return RGB(255,0,0);
}


void CChkResSettings::SetIcon(LPCTSTR cmd, LPCTSTR reason, LPCTSTR icon)
{
	for( int i=0; i<m_arrChkIcons.GetSize(); i++)
	{
		ChkIcon item = m_arrChkIcons[i];
		if( stricmp(cmd,item.cmd)==0 && CompareReason(item.reason,reason) )
		{
			m_arrChkIcons[i].icon = icon;
			break;
		}
	}
	
	if( i>=m_arrChkIcons.GetSize() )
	{
		ChkIcon item;
		item.cmd = cmd;
		item.reason = reason;
		item.icon = icon;
		m_arrChkIcons.Add(item);
	}
}


void CChkResSettings::SetColor(LPCTSTR cmd, LPCTSTR reason, COLORREF color)
{
	for( int i=0; i<m_arrChkIcons.GetSize(); i++)
	{
		ChkIcon item = m_arrChkIcons[i];
		if( stricmp(cmd,item.cmd)==0 && CompareReason(item.reason,reason) )
		{
			m_arrChkIcons[i].color = color;
			break;
		}
	}	
}

void ClearNumber(wstring& s)
{
	wstring s2;
	for( int i=0; i<s.size(); i++)
	{
		int c = s[i];
		if( (c>=L'0' && c<=L'9') || (c==L'-') || (c==L'.') )
		{

		}
		else
		{
			s2 += c;
		}
	}
	s = s2;
}

BOOL CChkResSettings::CompareReason(LPCTSTR reason0, LPCTSTR reason1)
{
	if( stricmp(reason0,reason1)==0 )
		return TRUE;

	wstring s0, s1;

	ConvertCharToWstring(reason0,s0,CP_ACP);
	ConvertCharToWstring(reason1,s1,CP_ACP);

	//清除里面的数字
	ClearNumber(s0);
	ClearNumber(s1);

	if( wcsicmp(s0.data(),s1.data())==0 )
		return TRUE;

	return FALSE;
}


CChkResManger::CChkResManger()
{
	m_pCurResultItem = NULL;	
}

CChkResManger::~CChkResManger()
{
	Clear();
}

void CChkResManger::SetCurChkCmd(const CChkCmd* pCmd)
{
	m_pCurCmd = pCmd;
}


BOOL CChkResManger::BeginResItem(LPCTSTR name)
{
	if (m_pCurResultItem)
	{
		EndResItem();
	}
	m_pCurResultItem = new ChkResultItem;
	if(!m_pCurResultItem)return FALSE;

	if( m_pCurCmd!=NULL && name==NULL )
		m_pCurResultItem->chkCmd = m_pCurCmd->GetCheckName();
	else
		m_pCurResultItem->chkCmd = name;

	return TRUE;
}

void CChkResManger::AddAssociatedFtr(CFeature* pFtr)
{
	if (m_pCurResultItem)
	{
		m_pCurResultItem->arrFtrs.Add(pFtr);
	}
}

void CChkResManger::SetAssociatedPos(PT_3D pos)
{
	if (m_pCurResultItem)
	{
		m_pCurResultItem->pos = pos;
	}
}

void CChkResManger::SetReason(LPCTSTR reason)
{
	if (m_pCurResultItem)
	{
		m_pCurResultItem->reason = reason;
	}
}

BOOL CChkResManger::EndResItem()
{
	if (m_pCurResultItem)
	{
		if (m_pCurResultItem->reason.IsEmpty())
		{
			delete m_pCurResultItem;
			m_pCurResultItem = NULL;
			return FALSE;
		}
		else
		{
			m_pCurResultItem->ntime = time(NULL);
			m_arrChkRes.Add(m_pCurResultItem);
			m_pCurResultItem = NULL;
			return TRUE;
		}
		
	}
	return FALSE;
}

void CChkResManger::Clear()
{
	for (int i=0;i<m_arrChkRes.GetSize();++i)
	{
		delete m_arrChkRes[i];
	}
	m_arrChkRes.RemoveAll();
	if (m_pCurResultItem!=NULL)
	{
		delete m_pCurResultItem;
		m_pCurResultItem = NULL;
	}
}

int CChkResManger::GetChkResCount()const
{
	return m_arrChkRes.GetSize();
}

ChkResultItem* CChkResManger::GetChkResByIdx(int idx)
{
	if (idx<0||idx>=m_arrChkRes.GetSize())
		return NULL;
	return m_arrChkRes.GetAt(idx);
}


CString CChkResManger::GetSavePath()
{
	//保存历史记录
	TCHAR module[_MAX_PATH]={0};	
	GetModuleFileName(NULL,module,_MAX_PATH);
	CString dir = module;
	int pos = dir.ReverseFind(_T('\\'));
	if( pos>0 )
	{
		dir = dir.Left(pos);
		pos = dir.ReverseFind(_T('\\'));		
	}
	
	if( pos>0 )
	{
		dir = dir.Left(pos+1) + _T("Config");
	}
	else
	{
		dir = _T("Config");
	}
	
	dir += "\\CheckResult.dat";

	return dir;
}


void CChkResManger::Load()
{	
	CDocument *pDoc = GetCurDocument();
	if (!pDoc) return;

	CDlgDataSource *pDS = NULL;
	CDataQuery *pDQ = NULL;
	
	if( pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
	{
		CDlgDoc *pd = NULL;
		pd = (CDlgDoc*)pDoc;
		pDS = pd->GetDlgDataSource();
		pDQ = pd->GetDataQuery();
	}
	
	CString path = GetSavePath();
	
	FILE *fp = fopen(path,"rb");
	if( !fp )
		return;

	Clear();

	char docPath[500] = {0};
	fread(docPath,1,sizeof(docPath),fp);
	if( stricmp(docPath,pDoc->GetPathName())!=0 )
	{
		fclose(fp);
		return;
	}
	
	char buf[128];
	int i, nsz = 0, szObj = 0;
	fread(&szObj,sizeof(int),1,fp);

	//数据结构已经发生变化
	if( szObj!=sizeof(ChkResultItem) )
	{
		fclose(fp);
		DeleteFile(path);
		return;
	}

	fread(&nsz,sizeof(int),1,fp);
	if( nsz<=0 )
	{
		fclose(fp);
		return;
	}

	for( i=0; i<nsz; i++)
	{
		ChkResultItem *pItem = new ChkResultItem();
		
		memset(buf,0,sizeof(buf));
		fread(buf,sizeof(buf),1,fp);
		pItem->chkCmd = buf;
		
		memset(buf,0,sizeof(buf));
		fread(buf,sizeof(buf),1,fp);
		pItem->reason = buf;
		
		fread(&pItem->pos,sizeof(pItem->pos),1,fp);
		fread(&pItem->state,sizeof(pItem->state),1,fp);

		OUID id2;
		fread(&id2,sizeof(OUID),1,fp);

		CPFeature pFtr = pDS->GetFeatureByID(id2);
		if( pFtr )
			pItem->pFtrMark = pFtr;
		
		int nobj = 0;
		fread(&nobj,sizeof(int),1,fp);
		
		for( int j=0; j<nobj; j++)
		{
			PT_3DEX expt;
			OUID id;			
			fread(&id,sizeof(OUID),1,fp);
			fread(&expt,sizeof(expt),1,fp);

			if( pDS && pDQ )
			{
				pFtr = pDS->GetFeatureByID(id);
				if( pFtr )
					pItem->arrFtrs.Add(pFtr);
			}
		}

		m_arrChkRes.Add(pItem);
	}
}

void CChkResManger::Load(CString path, CArray<ChkResultItem*, ChkResultItem*>& arrChkRes_Compare)
{
	CDocument *pDoc = GetCurDocument();
	if (!pDoc) return;

	CDlgDataSource *pDS = NULL;
	CDataQuery *pDQ = NULL;

	if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)))
	{
		CDlgDoc *pd = NULL;
		pd = (CDlgDoc*)pDoc;
		pDS = pd->GetDlgDataSource();
		pDQ = pd->GetDataQuery();
	}

	FILE *fp = fopen(path, "rb");
	if (!fp) return;

	char docPath[500] = { 0 };
	fread(docPath, 1, sizeof(docPath), fp);
	if (stricmp(docPath, pDoc->GetPathName()) != 0)
	{
		fclose(fp);
		return;
	}

	char buf[128];
	int i, nsz = 0, szObj = 0;
	fread(&szObj, sizeof(int), 1, fp);

	//数据结构已经发生变化
	if (szObj != sizeof(ChkResultItem))
	{
		fclose(fp);
		DeleteFile(path);
		return;
	}

	fread(&nsz, sizeof(int), 1, fp);
	if (nsz <= 0)
	{
		fclose(fp);
		return;
	}

	for (i = 0; i<nsz; i++)
	{
		ChkResultItem *pItem = new ChkResultItem();

		memset(buf, 0, sizeof(buf));
		fread(buf, sizeof(buf), 1, fp);
		pItem->chkCmd = buf;

		memset(buf, 0, sizeof(buf));
		fread(buf, sizeof(buf), 1, fp);
		pItem->reason = buf;

		fread(&pItem->pos, sizeof(pItem->pos), 1, fp);
		fread(&pItem->state, sizeof(pItem->state), 1, fp);

		OUID id2;
		fread(&id2, sizeof(OUID), 1, fp);

		CPFeature pFtr = pDS->GetFeatureByID(id2);
		if (pFtr)
			pItem->pFtrMark = pFtr;

		int nobj = 0;
		fread(&nobj, sizeof(int), 1, fp);

		for (int j = 0; j<nobj; j++)
		{
			PT_3DEX expt;
			OUID id;
			fread(&id, sizeof(OUID), 1, fp);
			fread(&expt, sizeof(expt), 1, fp);

			if (pDS && pDQ)
			{
				pFtr = pDS->GetFeatureByID(id);
				if (pFtr)
					pItem->arrFtrs.Add(pFtr);
			}
		}

		arrChkRes_Compare.Add(pItem);
	}
}


//有可能CChkResManger中的Feature已经被删除了，所以Save之前应保证对应的文档没有关闭
void CChkResManger::Save(CDocument *pDoc, LPCTSTR savepath)
{
	CString path = savepath?savepath:GetSavePath();
	if( !pDoc )return;

	if( !pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
		return;

	CDlgDoc *pDD = (CDlgDoc*)pDoc;

	CDataQuery *pDQ = pDD->GetDataQuery();

	FILE *fp = fopen(path, "wb");
	if (!fp) return;

	char docPath[500] = {0};
	strcpy(docPath,pDoc->GetPathName());
	fwrite(docPath,1,sizeof(docPath),fp);

	int szObj = sizeof(ChkResultItem);
	fwrite(&szObj,sizeof(int),1,fp);

	char buf[128];
	int nsz = m_arrChkRes.GetSize();
	fwrite(&nsz,sizeof(int),1,fp);
	for( int i=0; i<nsz; i++)
	{
		ChkResultItem *pItem = m_arrChkRes[i];

		memset(buf,0,sizeof(buf));
		strcpy(buf,pItem->chkCmd);
		fwrite(buf,sizeof(buf),1,fp);

		memset(buf,0,sizeof(buf));
		strcpy(buf,pItem->reason);
		fwrite(buf,sizeof(buf),1,fp);

		fwrite(&pItem->pos,sizeof(pItem->pos),1,fp);
		fwrite(&pItem->state,sizeof(pItem->state),1,fp);

		if( pDQ->IsManagedObj(pItem->pFtrMark) )
		{
			PT_3DEX expt = pItem->pFtrMark->GetGeometry()->GetDataPoint(0);
			
			fwrite(&pItem->pFtrMark->GetID(),sizeof(OUID),1,fp);
		}
		else
		{
			fwrite(&OUID(),sizeof(OUID),1,fp);
		}

		int nobj = pItem->arrFtrs.GetSize();
		fwrite(&nobj,sizeof(int),1,fp);

		for( int j=0; j<nobj; j++)
		{
			CPFeature pFtr = pItem->arrFtrs[j];

			if( pDQ->IsManagedObj(pFtr) )
			{
				PT_3DEX expt = pFtr->GetGeometry()->GetDataPoint(0);
				
				fwrite(&pFtr->GetID(),sizeof(OUID),1,fp);
				fwrite(&expt,sizeof(expt),1,fp);
			}
			else
			{
				fwrite(&OUID(),sizeof(OUID),1,fp);
				fwrite(&PT_3DEX(),sizeof(PT_3DEX),1,fp);
			}
		}
	}

	fclose(fp);
}

BOOL CChkResManger::MarkItem(CDlgDoc *pDoc, ChkResultItem *pItem, int icon_index, COLORREF color, float icon_size)
{
	CChkResSettings *pCS = CChkResSettings::Obj();
	CStringArray *pIconNames = pCS->GetResultIconNames();

	PT_3D pt = pItem->pos;
	CString result_reason = pItem->reason;

	int i = icon_index;

	if( i<0 || i>=pIconNames->GetSize() )
	{
		MarkErrorPt(pDoc,pItem,pDoc->GetDlgDataSource()->GetSymbolDrawScale()*icon_size*0.5);
		return TRUE;
	}
	
	CCellDefLib *pCellLib = GetCellDefLib();
	if( !pCellLib )
	{
		MarkErrorPt(pDoc,pItem,pDoc->GetDlgDataSource()->GetSymbolDrawScale()*icon_size*0.5);
		return TRUE;
	}
	
	CellDef def = pCellLib->GetCellDef(pIconNames->GetAt(i));
	if( def.m_pgr==NULL )
	{
		MarkErrorPt(pDoc,pItem,pDoc->GetDlgDataSource()->GetSymbolDrawScale()*icon_size*0.5);
		return TRUE;
	}
	
	CFtrLayer *pLayer = pDoc->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_MARKLAYER_NAME));
	if( !pLayer )
	{
		pLayer = pDoc->GetDlgDataSource()->CreateFtrLayer(StrFromResID(IDS_MARKLAYER_NAME));
		
		if(!pDoc->AddFtrLayer(pLayer)) return FALSE;
		
	}
	pLayer->EnableLocked(TRUE);
	int scale = pDoc->GetDlgDataSource()->GetScale();	
	
	CFeature *pFtr = pLayer->CreateDefaultFeature(scale,CLS_GEOPOINT);
	if( !pFtr )
	{
		return FALSE;
	}
	
	CGeometry *pObj = pFtr->GetGeometry();
	
	if (!pObj)
	{
		return FALSE;
	}
	
	pObj->SetColor(color);	
	
	CArray<PT_3DEX,PT_3DEX> pts;
	PT_3DEX expt;
	COPY_3DPT(expt,pt);
	
	expt.pencode = penLine;
	
	pts.Add(expt);	
	
	pObj->CreateShape(pts.GetData(),pts.GetSize());
	
	double size1 = max(def.m_evlp.Width(),def.m_evlp.Height());
	double size2 = icon_size;
	
	CGeoPoint *pPoint = (CGeoPoint*)pObj;
	pPoint->SetSymbolName(CString("@")+def.m_name);
	pPoint->m_fKx = pPoint->m_fKy = size2/size1;
	pPoint->m_fWidth = 0;
	
	pDoc->AddObject(pFtr,pLayer->GetID());
	
	pItem->pFtrMark = pFtr;
	
	return TRUE;
}



BOOL CChkResManger::DeleteMark(CDlgDoc *pDoc, ChkResultItem *pItem)
{
	if( pDoc && pItem->pFtrMark && pDoc->GetDataQuery()->IsManagedObj(pItem->pFtrMark) )
	{
		pDoc->DeleteObject(FtrToHandle(pItem->pFtrMark));
		pItem->pFtrMark = NULL;
		return TRUE;
	}
	return FALSE;
}


BOOL CChkResManger::DeleteAllMarks(CDlgDoc *pDoc)
{
	if( pDoc )
	{
		pDoc->BeginBatchUpdate();
		for( int i=0; i<m_arrChkRes.GetSize(); i++)
		{
			ChkResultItem *pItem = m_arrChkRes[i];
			if( pItem->pFtrMark && pDoc->GetDataQuery()->IsManagedObj(pItem->pFtrMark) )
			{
				pDoc->DeleteObject(FtrToHandle(pItem->pFtrMark));
				pItem->pFtrMark = NULL;
			}
		}
		pDoc->EndBatchUpdate();

		return TRUE;
	}

	return FALSE;
}


CChkResManger& GetChkResMgr()
{
	static CChkResManger s_ChkResMgr;
	return s_ChkResMgr;
}


void CChkResManger::CompareLastResult()
{
	CDocument *pDoc = GetCurDocument();
	if (!pDoc) return;
	
	CDlgDataSource *pDS = NULL;
	CDataQuery *pDQ = NULL;
	
	if( pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
	{
		CDlgDoc *pd = NULL;
		pd = (CDlgDoc*)pDoc;
		pDS = pd->GetDlgDataSource();
		pDQ = pd->GetDataQuery();
	}
	
	CString path = GetSavePath();
	
	FILE *fp = fopen(path,"rb");
	if( !fp ) return;
	
	char docPath[500] = {0};
	fread(docPath,1,sizeof(docPath),fp);
	if( stricmp(docPath,pDoc->GetPathName())!=0 )
	{
		fclose(fp);
		return;
	}
	
	char buf[128];
	int i, nsz = 0, szObj = 0;
	fread(&szObj,sizeof(int),1,fp);
	
	//数据结构已经发生变化
	if( szObj!=sizeof(ChkResultItem) )
	{
		fclose(fp);
		DeleteFile(path);
		return;
	}
	
	fread(&nsz,sizeof(int),1,fp);

	CArray<ChkResultItem*,ChkResultItem*> arrLastChkRes;
	
	for( i=0; i<nsz; i++)
	{
		ChkResultItem *pItem = new ChkResultItem();
		
		memset(buf,0,sizeof(buf));
		fread(buf,sizeof(buf),1,fp);
		pItem->chkCmd = buf;
		
		memset(buf,0,sizeof(buf));
		fread(buf,sizeof(buf),1,fp);
		pItem->reason = buf;
		
		fread(&pItem->pos,sizeof(pItem->pos),1,fp);
		fread(&pItem->state,sizeof(pItem->state),1,fp);
		
		OUID id2;
		fread(&id2,sizeof(OUID),1,fp);
		
		CPFeature pFtr = pDS->GetFeatureByID(id2);
		if( pFtr )
			pItem->pFtrMark = pFtr;
		
		int nobj = 0;
		fread(&nobj,sizeof(int),1,fp);
		
		for( int j=0; j<nobj; j++)
		{
			PT_3DEX expt;
			OUID id;			
			fread(&id,sizeof(OUID),1,fp);
			fread(&expt,sizeof(expt),1,fp);
			
			if( pDS && pDQ )
			{
				pFtr = pDS->GetFeatureByID(id);
				if( pFtr )
					pItem->arrFtrs.Add(pFtr);
			}
		}
		
		arrLastChkRes.Add(pItem);
	}

	fclose(fp);
	fp = NULL;

	CDlgDoc *pd = (CDlgDoc*)pDoc;
	if( pd )
	{
		pd->BeginBatchUpdate();
		for( i=0; i<m_arrChkRes.GetSize(); i++)
		{
			ChkResultItem *pItem1 = m_arrChkRes[i];
			for(int j=0; j<arrLastChkRes.GetSize(); j++)
			{
				ChkResultItem *pItem2 = arrLastChkRes[j];
				if(pItem2->state==1)
				{
					if(*pItem1 == *pItem2)
					{
						pItem1->state = pItem2->state;
					}
				}
			}
		}
		pd->EndBatchUpdate();
	}

	for( i=0; i<arrLastChkRes.GetSize(); i++)
	{
		delete arrLastChkRes[i];
	}
}


CChkResSettings::CChkResSettings()
{
	m_bLoad = FALSE;
	m_fResultIconSize = 2.0f;
}

CChkResSettings::~CChkResSettings()
{

}


CString CChkResSettings::GetPath()
{
	//保存历史记录
	TCHAR module[_MAX_PATH]={0};	
	GetModuleFileName(NULL,module,_MAX_PATH);
	CString dir = module;
	int pos = dir.ReverseFind(_T('\\'));
	if( pos>0 )
	{
		dir = dir.Left(pos);
		pos = dir.ReverseFind(_T('\\'));		
	}
	
	if( pos>0 )
	{
		dir = dir.Left(pos+1) + _T("Config");
	}
	else
	{
		dir = _T("Config");
	}
	
	dir += "\\CheckResultSettings.xml";
	
	return dir;
}


BOOL CChkResSettings::Load()
{
	if( m_path.IsEmpty() )
		m_path = GetPath();

	CMarkup xmlFile;	
	if(!xmlFile.Load(m_path))
		return FALSE;

	m_arrResultIconNames.RemoveAll();
	
	if( xmlFile.FindElem(XMLTAG_SETTINGS,TRUE) )
	{		
		xmlFile.IntoElem();	
		
		if( xmlFile.FindElem(XMLTAG_ICONSIZE,TRUE) )
		{
			m_fResultIconSize = atof(xmlFile.GetData());
		}
		
		if( xmlFile.FindElem(XMLTAG_ICONS,TRUE) )
		{
			xmlFile.IntoElem();
			
			BOOL bFromTop = TRUE;
			while ( xmlFile.FindElem(XMLTAG_ICONNAME,bFromTop) )
			{
				bFromTop = FALSE;

				m_arrResultIconNames.Add(xmlFile.GetData());
			}
			
			xmlFile.OutOfElem();
		}

		if( xmlFile.FindElem(XMLTAG_ICONNAME_MAP,TRUE) )
		{
			xmlFile.IntoElem();
			
			BOOL bFromTop = TRUE;
			while ( xmlFile.FindElem(XMLTAG_ICONNAME_MAPITEM,bFromTop) )
			{
				bFromTop = FALSE;
				xmlFile.IntoElem();

				ChkIcon item;
				int num = 0;
				if( xmlFile.FindElem(XMLTAG_CHKCMD) )
				{
					item.cmd = xmlFile.GetData();
					num++;
				}
				if( xmlFile.FindElem(XMLTAG_REASON) )
				{
					item.reason = xmlFile.GetData();
					num++;
				}
				if( xmlFile.FindElem(XMLTAG_ICONNAME) )
				{
					item.icon = xmlFile.GetData();
					num++;
				}
				if( xmlFile.FindElem(XMLTAG_COLOR) )
				{
					item.color = atol(xmlFile.GetData());
					num++;
				}		
				
				if( num==4 )m_arrChkIcons.Add(item);
				
				xmlFile.OutOfElem();
			}
			
			xmlFile.OutOfElem();
		}
		
		xmlFile.OutOfElem();
	}
	else
	{
		return FALSE;
	}
	
	m_bLoad = TRUE;
	return TRUE;
}


BOOL CChkResSettings::Save()
{
	if( m_path.IsEmpty() )
		m_path = GetPath();

	CMarkup xmlFile;
	
	xmlFile.AddElem(XMLTAG_SETTINGS);
	xmlFile.IntoElem();

	CString strData;
	strData.Format("%.6f",(double)m_fResultIconSize);
	xmlFile.AddElem(XMLTAG_ICONSIZE,strData);
	
	xmlFile.AddElem(XMLTAG_ICONS);
	xmlFile.IntoElem();
	for( int i=0; i<m_arrResultIconNames.GetSize(); i++)
	{
		xmlFile.AddElem(XMLTAG_ICONNAME,(LPCTSTR)m_arrResultIconNames[i]);
	}
	xmlFile.OutOfElem();

	xmlFile.AddElem(XMLTAG_ICONNAME_MAP);
	xmlFile.IntoElem();
	CString strValue;
	for( i=0; i<m_arrChkIcons.GetSize(); i++)
	{
		xmlFile.AddElem(XMLTAG_ICONNAME_MAPITEM);
		xmlFile.IntoElem();
	
		ChkIcon item = m_arrChkIcons[i];
		xmlFile.AddElem(XMLTAG_CHKCMD,(LPCTSTR)item.cmd);
		xmlFile.AddElem(XMLTAG_REASON,(LPCTSTR)item.reason);
		xmlFile.AddElem(XMLTAG_ICONNAME,(LPCTSTR)item.icon);

		strValue.Format("%d",(int)item.color);
		xmlFile.AddElem(XMLTAG_COLOR,(LPCTSTR)strValue);

		xmlFile.OutOfElem();
	}
	xmlFile.OutOfElem();

	xmlFile.OutOfElem();
		
	xmlFile.Save(m_path);
	return TRUE;

}


void CChkResSettings::Copy(CChkResSettings *p)
{
	m_arrResultIconNames.Copy(p->m_arrResultIconNames);
	
	m_fResultIconSize = p->m_fResultIconSize;
	m_arrChkIcons.Copy(p->m_arrChkIcons);
	
	m_bLoad = p->m_bLoad;
	m_path = p->m_path;
}

CChkResSettings* CChkResSettings::Obj()
{
	static CChkResSettings s_Obj;

	if( !s_Obj.IsLoaded() )
	{
		s_Obj.Load();
	}

	return &s_Obj;
}


BEGIN_MESSAGE_MAP(CListCtrl0, CListCtrl)
//{{AFX_MSG_MAP(CListEx)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, CustDraw)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CListCtrl0::CustDraw(NMHDR *pNotifyStruct, LRESULT *result)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW) pNotifyStruct; // cast our generic structure to bigger/specialized strc
	
	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*result  = CDRF_NOTIFYITEMDRAW;	// request notifications for individual items		
		break;		
	case CDDS_ITEMPREPAINT:
		*result = CDRF_NOTIFYSUBITEMDRAW|CDRF_NOTIFYPOSTPAINT;	
		break;
	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
		{		
			if (lplvcd->iSubItem==0)
			{
				*result = /*CDRF_NOTIFYPOSTPAINT|*/CDRF_SKIPDEFAULT;
			}
			else 
				*result = CDRF_DODEFAULT;
		}
		break;
	case CDDS_ITEMPOSTPAINT:
		{
			LVITEM rItem;
			ZeroMemory ( &rItem, sizeof(LVITEM) );				
			rItem.mask  = LVIF_IMAGE;				
			rItem.iItem = lplvcd->nmcd.dwItemSpec;
			GetItem ( &rItem );
			CDC*  pDC = CDC::FromHandle ( lplvcd->nmcd.hdc );
			CRect rect;	
			GetItemRect(lplvcd->nmcd.dwItemSpec,&rect,LVIR_ICON);		
			
			CImageList* pImgList = GetImageList( LVSIL_SMALL );
			if (pImgList != NULL){
				pImgList->Draw ( pDC, rItem.iImage, rect.TopLeft(),
						ILD_TRANSPARENT );
			}
		}
		break;
	default:
		*result = CDRF_DODEFAULT;
	}
	
}

void CListCtrl0::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	SubItemHitTest(&lvhti);
	int state = GetItemState(lvhti.iItem, LVIS_SELECTED);
	if (lvhti.iSubItem==0 && state )
	{
		CMainFrame *pMainFrame = ((CMainFrame*)AfxGetMainWnd());
		pMainFrame->m_wndCheckResultBar.SendMessage(MESSAGE_CLICKICON,lvhti.iItem,lvhti.iSubItem);
		return;
	}
	CListCtrl::OnLButtonDown(nFlags, point);
}


BEGIN_MESSAGE_MAP(CCheckResultViewBar, CDockablePane)
//	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(LVN_COLUMNCLICK, ID_LIST_CHKRES, OnColumnclickList)
	ON_NOTIFY(NM_CLICK, ID_LIST_CHKRES, OnClickList)
	ON_COMMAND(ID_RESULT_CLEAR, OnResultClear)
	ON_COMMAND(ID_RESULT_MARKALL, OnResultMarkall)
	ON_COMMAND(ID_RESULT_MARKPT, OnResultMarkpt)
	ON_COMMAND(ID_RESULT_SELALL, OnResultSelall)
	ON_COMMAND(ID_RESULT_SELOBJ, OnResultSelobj)
	ON_NOTIFY(LVN_ITEMCHANGED, ID_LIST_CHKRES, OnItemChangedList)
	ON_COMMAND(ID_RESULT_FILTER, OnResultFilter)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnNeedTipText)
	ON_COMMAND(ID_RESULT_LOAD, OnResultLoad)
	ON_COMMAND(ID_RESULT_ITERATE, OnResultIterate)
	ON_UPDATE_COMMAND_UI(ID_RESULT_ITERATE,OnUpdateResultIterate)
	ON_COMMAND(ID_RESULT_REFRESH, OnRefreshResultStates)
	ON_COMMAND(ID_RESULT_MARKSETTINGS, OnMarkSettings)
	ON_COMMAND(ID_RESULT_LOCATEITEM,OnLocateItem)
	ON_COMMAND(ID_RESULT_EXPORT,OnExport)
	ON_COMMAND(ID_CHKRESAVERET, OnSaveRet)
	ON_COMMAND(ID_LOADALLCHKRES, OnLoadRets)
	ON_MESSAGE(MESSAGE_CLICKICON, OnClickIcon)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCheckResultViewBar::CCheckResultViewBar()
{
	m_pChkResManger = &GetChkResMgr();
	m_pDoc = NULL;

	m_bInitLoad = FALSE;
	m_bResultIterate = FALSE;
	m_bStopLocateResultPt = FALSE;
}

CCheckResultViewBar::~CCheckResultViewBar()
{

}


void CCheckResultViewBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	{	
		CMenu menu;
		menu.LoadMenu(IDR_MENU_CHK_RES);
		menu.GetSubMenu(0)->CheckMenuItem(ID_RESULT_ITERATE,(m_bResultIterate?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this);

		return;
	}
	CDockablePane::OnContextMenu( pWnd,  point);	
}

int CCheckResultViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_HIDE_INPLACE, IDR_TOOLBAR_CHK);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() &
		~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_CHK, 0, 0, TRUE);
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetRouteCommandsViaFrame (FALSE);
	m_wndToolBar.SetWindowText("CheckResult");

	CRect dumy;
	dumy.SetRectEmpty();

	m_listCtrl.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_SHOWSELALWAYS,
		dumy, this, ID_LIST_CHKRES);

// 	m_listCtrl.SetExtendedStyle
// 		(m_listCtrl.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	ListView_SetExtendedListViewStyleEx(m_listCtrl.m_hWnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);	
	ListView_SetExtendedListViewStyleEx(m_listCtrl.m_hWnd, LVS_EX_GRIDLINES , LVS_EX_GRIDLINES );
	
//	ListView_SetExtendedListViewStyleEx(m_listCtrl.m_hWnd, LVS_EX_SUBITEMIMAGES , LVS_EX_SUBITEMIMAGES );
	//ListView_SetExtendedListViewStyleEx(m_list.m_hWnd, LVS_EX_TRACKSELECT, LVS_EX_TRACKSELECT );
//	ListView_SetExtendedListViewStyleEx(m_listCtrl.m_hWnd, LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES );
	m_imageList.Create(IDB_CHK_MARK,16,0,RGB(255,255,255));
	
	m_listCtrl.InsertColumn(0,StrFromResID(IDS_CHK_MARK),LVCFMT_LEFT,37);
	m_listCtrl.InsertColumn(1,StrFromResID(IDS_CHK_IDX),LVCFMT_LEFT,43);
	m_listCtrl.InsertColumn(2,StrFromResID(IDS_CHK_CMDNAME),LVCFMT_LEFT,100);
	m_listCtrl.InsertColumn(3,StrFromResID(IDS_CHK_REASON),LVCFMT_LEFT,100);
	m_listCtrl.InsertColumn(4,StrFromResID(IDS_CHK_OBJINFO),LVCFMT_LEFT,150);
	m_listCtrl.SetImageList(&m_imageList,LVSIL_SMALL);	

	m_ToolTip.Create (this);
	m_ToolTip.Activate (TRUE);
	m_ToolTip.SetMaxTipWidth(300);


	AdjustLayout();
	return 0;	
}


void CCheckResultViewBar::InitLoad()
{
	m_pDoc = GetCurDocument();
	if (!m_pDoc) return;

	m_pChkResManger->Load();
	m_bInitLoad = TRUE;

	CreateFilter();

	FillChkResItem();
	UpdateToolTips2();
}

void CCheckResultViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);	
	AdjustLayout ();
}


CString CCheckResultViewBar::MakeObjInfo(const ChkResultItem *pItem, BOOL bForTips)
{
	if( !m_pDoc )return CString();
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return CString();
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;

	CString ret, cur;

	if( bForTips )
	{
		ret = pItem->reason;		
		ret += "\n";
	}
	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	for( int i=0; i<pItem->arrFtrs.GetSize(); i++)
	{
		if( bForTips )
		{
			ret += "  ";
		}

		if( pItem->arrFtrs.GetSize()>1 )
		{
			cur.Format("%d: ",i+1);
			ret += cur;
		}

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pItem->arrFtrs[i]);
		cur.Format("%s (%I64d), ",pLayer->GetName(), pDS->GetLayerCodeOfFtrLayer(pLayer));

		ret += cur;

		PT_3DEX tmpPt;
		CGeometry *pGeo = pItem->arrFtrs[i]->GetGeometry();
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
				
				CString str = StrFromResID(IDS_DOC_SELINFO3);
				cur.Format(str,(LPCTSTR)strZ);
				ret += cur;
			}
		}

		if( bForTips )
			ret += "\n";
		else
			ret += "  ";
	}

	return ret;
}

void CCheckResultViewBar::FillChkResItem()
{
	m_listCtrl.DeleteAllItems();
	int nCnt = m_pChkResManger->GetChkResCount();
	int nCol = m_listCtrl.GetHeaderCtrl()->GetItemCount(), nItem = 0;
	for (int i=0;i<nCnt;i++)
	{
		const ChkResultItem* pItem = m_pChkResManger->GetChkResByIdx(i);
		
		for( int j=0; j<m_arrFilterFlags.GetSize(); j++ )
		{
			if( m_arrFilterFlags[j]==0 && pItem->reason.CompareNoCase(m_arrFilterNames[j])==0 )
				break;
		}
		if( j>=m_arrFilterFlags.GetSize() )
		{
			m_listCtrl.InsertItem(nItem,_T(""),pItem->state );
			CString strTemp;

			strTemp.Format(_T("%d"),nItem+1);
			m_listCtrl.SetItemText(nItem,1,strTemp);
			strTemp = pItem->chkCmd;
			m_listCtrl.SetItemText(nItem,2,strTemp);
			strTemp = pItem->reason;
			m_listCtrl.SetItemText(nItem,3,strTemp);
			strTemp = MakeObjInfo(pItem,FALSE);
			m_listCtrl.SetItemText(nItem,4,strTemp);

			m_listCtrl.SetItemData(nItem,(DWORD_PTR)pItem);

			nItem++;
		}	
	}
	m_listCtrl.Invalidate(FALSE);
}


void CCheckResultViewBar::LocateCurFtrs()
{
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	PT_3D pt0;
	ChkResultItem *pItem = NULL;
	int nSelItem = -1;
	if( !(pItem=GetSelPos(&pt0,&nSelItem)) )
		return;	
	
	KillTimer(4);
	m_nTimerState = 0;
	
	{
		m_gr.DeleteAll();
		
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		CDataQuery *pDQ = pDoc->GetDataQuery();
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		for( int i=0; i<pItem->arrFtrs.GetSize(); i++)
		{
			if( !pDQ->IsManagedObj(pItem->arrFtrs[i]) )
				continue;
			
			pItem->arrFtrs[i]->GetGeometry()->GetShape(arrPts);
			
			m_gr.BeginPointString(RGB(0,255,0),6,6,FALSE,0,0,TRUE);
			for( int j=0; j<arrPts.GetSize(); j++)
			{
				m_gr.PointString(&arrPts[j]);
			}
			m_gr.End();
		}
		
		pDoc->UpdateAllViews(NULL,hc_SetConstDragLine,(CObject*)&m_gr);
		pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
		
		CString strMsg;
		strMsg.Format("%s<%3d>: %s",(LPCTSTR)pItem->chkCmd,nSelItem,pItem->reason);
		GOutPut(strMsg);
		
		if( 0 )
		{		
			PT_3D pt1;		
			double markwid = 10*(pDoc->GetDlgDataSource())->GetScale()*0.001;
			m_gr.BeginLineString(RGB(255,255,0),0,FALSE);
			pt1.x = pt0.x-markwid; pt1.y = pt0.y-markwid; pt1.z = pt0.z;
			m_gr.MoveTo(&pt1);
			pt1.x = pt0.x+markwid; pt1.y = pt0.y+markwid; pt1.z = pt0.z;
			m_gr.LineTo(&pt1);
			
			pt1.x = pt0.x-markwid; pt1.y = pt0.y+markwid; pt1.z = pt0.z;
			m_gr.MoveTo(&pt1);
			pt1.x = pt0.x+markwid; pt1.y = pt0.y-markwid; pt1.z = pt0.z;
			m_gr.LineTo(&pt1);
			m_gr.End();
			pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
			
			SetTimer(4,500,NULL);
		}
	}
}

void CCheckResultViewBar::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if( m_bStopLocateResultPt )
	{
		*pResult = 0;
		return;
	}

	if( m_listCtrl.GetSelectedCount()>1)
		return;

	LocateCurFtrs();

	//避免文档指针m_pDaoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
 	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	CArray<CFeature*,CFeature*> arr;
	GetSelFtrs(arr);
	if( arr.GetSize()<=0 )return;
	
	CDataQuery *pDQ = pDoc->GetDataQuery();
	
	pDoc->DeselectAll();
	for(int i=arr.GetSize()-1;i>=0;i--)
	{
		if( !pDQ->IsManagedObj(arr[i]) || arr[i]->IsDeleted() )
		{
			arr.RemoveAt(i);
		}
	}
	
	if(arr.GetSize()>1)
	{
		if(pDoc->m_pDlgSelectShift==NULL)
		{
			pDoc->m_pDlgSelectShift = new CDlgSelectShift();
			pDoc->m_pDlgSelectShift->Create(CDlgSelectShift::IDD,NULL);	
		}
		pDoc->m_pDlgSelectShift->ShowWindow(SW_HIDE);
		if(pDoc->m_pDlgSelectShift!=NULL)
		{
			if(!pDoc->m_pDlgSelectShift->IsWindowVisible())
			{
				CRect rcClient;
				::GetWindowRect(GetActiveView()->GetSafeHwnd(),rcClient);
				
				CRect rcDlg;
				pDoc->m_pDlgSelectShift->GetWindowRect(&rcDlg);
				
				rcDlg.OffsetRect(rcClient.left-rcDlg.left,rcClient.top-rcDlg.top);
				pDoc->m_pDlgSelectShift->MoveWindow(&rcDlg);	
				
				pDoc->m_pDlgSelectShift->m_pDoc = pDoc;
				pDoc->m_pDlgSelectShift->m_arrFtrs.RemoveAll();
				for(int i=0; i<arr.GetSize(); i++)
				{
					pDoc->m_pDlgSelectShift->m_arrFtrs.Add(arr[i]);
				}
				pDoc->m_pDlgSelectShift->UpdateList();
				pDoc->m_pDlgSelectShift->ShowWindow(SW_SHOW);
				
				CView *pView = GetActiveView();
				if(pView)pView->SetFocus();
				
			}
		}
	}
	
	if(arr.GetSize()>=1)
	{
		pDoc->SelectObj(FtrToHandle(arr[0]),FALSE);	
	}
	pDoc->OnSelectChanged();

	*pResult = 0;
}


int CCheckResultViewBar::GetIconIndex(ChkResultItem *pItem)
{	
	CChkResSettings *pSettings = CChkResSettings::Obj();

	CString icon = pSettings->GetIcon(pItem->chkCmd,pItem->reason);
	if( icon.IsEmpty() )
	{
		if( pSettings->GetResultIconNames()->GetSize()>0 )
			return 0;
		else
			return -1;
	}

	for( int i=0; i<pSettings->GetResultIconNames()->GetSize(); i++)
	{
		if( icon.CompareNoCase(pSettings->GetResultIconNames()->GetAt(i))==0 )
		{
			return i;
		}
	}

	return -1;
}


COLORREF CCheckResultViewBar::GetIconColor(ChkResultItem *pItem)
{	
	CChkResSettings *pSettings = CChkResSettings::Obj();
	
	COLORREF color = pSettings->GetColor(pItem->chkCmd,pItem->reason);
	
	return color;
}

void CCheckResultViewBar::OnClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	LVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = pNMListView->ptAction;
	m_listCtrl.HitTest(&HitTestInfo);
	LVITEM item;
	item.mask = LVIF_IMAGE;
	item.iItem = HitTestInfo.iItem;
	item.iSubItem = 0/*HitTestInfo.iSubItem*/;
	if (HitTestInfo.flags==LVHT_ONITEMICON)//点图标
	{	
		ChkResultItem *pItem = (ChkResultItem*)m_listCtrl.GetItemData(item.iItem);

		if( pItem )
		{
			pItem->state = (pItem->state==0?1:0);
			
			CDocument *pDoc0 = GetCurDocument();
			if( pDoc0 && pDoc0->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
			{
				CDlgDoc *pDoc = (CDlgDoc*)pDoc0;
				CDataQuery *pDQ = pDoc->GetDataQuery();
				
				CPFeature pMark = pItem->pFtrMark;
			
				if( pItem->state==0 )
				{
					float szIcon = CChkResSettings::Obj()->GetResultIconSize();
					
					m_pChkResManger->MarkItem(pDoc,pItem,GetIconIndex(pItem),GetIconColor(pItem),szIcon);	
				}
				else if( pItem->state==1 )
				{
					m_pChkResManger->DeleteMark(pDoc,pItem);
				}

				pDoc->UpdateAllViews(NULL,hc_Refresh);
			}
		}

		m_listCtrl.GetItem(&item);
	/*	if(m_listCtrl.GetItemState(item.iItem, LVIS_SELECTED) == LVIS_SELECTED)*/
		{
			if( item.iImage==0 )
				item.iImage = 1;
			else
				item.iImage = 0;
			m_listCtrl.SetItem(&item);
		}
	}
	*pResult = 0;
}

void CCheckResultViewBar::ClearAll()
{
	m_pChkResManger->Clear();
	m_pDoc = NULL;

	m_listCtrl.DeleteAllItems();
	m_gr.DeleteAll();
	m_nTimerState = 0;
	
}

void CCheckResultViewBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	m_listCtrl.GetWindowRect (rect);
	ScreenToClient (rect);
	
	rect.InflateRect (1, 1);
	dc.Draw3dRect (rect, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}


BOOL CCheckResultViewBar::OnEraseBkgnd( CDC* pDC )
{
	CRect rectClient;
	GetClientRect (rectClient);	
	
	::FillRect(pDC->GetSafeHdc(),rectClient,GetSysColorBrush(COLOR_3DFACE));
	return TRUE;	
}

void CCheckResultViewBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rcWnd;
	m_listCtrl.GetWindowRect(rcWnd);
	ScreenToClient(&rcWnd);
	
	CRect rectClient;
	GetClientRect (rectClient);
	
	int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;	
	m_wndToolBar.SetWindowPos (NULL, rectClient.left, rectClient.top, 
		rectClient.Width (), cyTlb,
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_listCtrl.SetWindowPos(NULL,rectClient.left,rectClient.top+cyTlb,rectClient.Width(),rectClient.Height()-cyTlb,SWP_NOACTIVATE | SWP_NOZORDER);

	UpdateToolTips2();
}


LONG CCheckResultViewBar::OnSetResult(WPARAM wParam,LPARAM lParam)
{	
	m_pDoc = GetCurDocument();
	if (!m_pDoc) return 0;

	m_pChkResManger->CompareLastResult();

	CreateFilter();
	FillChkResItem();

	UpdateToolTips2();

	m_bResultIterate = TRUE;

	OnResultMarkall();

	m_pChkResManger->Save(m_pDoc);

  	return 0;
}

void CCheckResultViewBar::OnResultClear() 
{
	if( m_pChkResManger->GetChkResCount()>0 && m_pDoc!=NULL )
	{
		m_pChkResManger->Save(m_pDoc);
		if( m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
		{
			m_pChkResManger->DeleteAllMarks((CDlgDoc*)m_pDoc);
		}
	}

	ClearAll();
	m_bResultIterate = FALSE;
}


void CCheckResultViewBar::OnResultMarkall() 
{
//	避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;	
	
	int num = m_pChkResManger->GetChkResCount();
	ChkResultItem* pItem = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESSING));
	GProgressStart(num);

	if( m_arrFilterNames.GetSize()<=0 )
	{
		CreateFilter();
	}

	float szIcon = CChkResSettings::Obj()->GetResultIconSize();

	pDoc->BeginBatchUpdate();
	for( int i=0; i<num; i++)
	{
		pItem = m_pChkResManger->GetChkResByIdx(i);
	
		//if( pItem->arrFtrs.GetSize()<=0 )continue;	
		
		GProgressStep();		
		
		if( pItem->state==0 )
			m_pChkResManger->MarkItem(pDoc,pItem,GetIconIndex(pItem),GetIconColor(pItem),szIcon);	
			
	}
	pDoc->EndBatchUpdate();

	GProgressEnd();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESS_END));
	
	pDoc->UpdateAllViews(NULL,hc_Refresh);
}


void CCheckResultViewBar::GetSelFtrs(CArray<CFeature*,CFeature*> &arr)
{
	CListCtrl0 * pmyListCtrl = &m_listCtrl;
	
	UINT i, uSelectedCount = pmyListCtrl->GetSelectedCount();
	int  nItem = -1;
	
	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
		for (i=0;i < uSelectedCount;i++)
		{
			nItem = pmyListCtrl->GetNextItem(nItem, LVNI_SELECTED);
			ASSERT(nItem != -1);
			pmyListCtrl->Update(nItem); 
		}
	}
	else
		return ;	
	
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	ChkResultItem* pItem = (ChkResultItem*)m_listCtrl.GetItemData(nItem);
	
	arr.Copy(pItem->arrFtrs);	
}

ChkResultItem* CCheckResultViewBar::GetSelPos(PT_3D *pt, int *pIndex)
{
	CListCtrl0 * pmyListCtrl = &m_listCtrl;
	
	UINT i, uSelectedCount = pmyListCtrl->GetSelectedCount();
	int  nItem = -1;
	ASSERT(uSelectedCount==1);
	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
		for (i=0;i < uSelectedCount;i++)
		{
			nItem = pmyListCtrl->GetNextItem(nItem, LVNI_SELECTED);
			ASSERT(nItem != -1);
			pmyListCtrl->Update(nItem); 
		}
	}
	else
	{
		return NULL;	
	}
	
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;	
	ChkResultItem* pItem = (ChkResultItem*)m_listCtrl.GetItemData(nItem);
	if( pt )*pt = pItem->pos;
	if( pIndex )*pIndex = nItem;

	return  pItem;
}



void CCheckResultViewBar::OnResultMarkpt() 
{
	//	避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	PT_3D pt0;
	ChkResultItem* pItem = GetSelPos(&pt0);
	if(!pItem)return;	

	if( m_arrFilterNames.GetSize()<=0 )
	{
		CreateFilter();
	}
	
	float szIcon = CChkResSettings::Obj()->GetResultIconSize();

	m_pChkResManger->MarkItem(pDoc,pItem,GetIconIndex(pItem),GetIconColor(pItem),szIcon);	

	pDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CCheckResultViewBar::OnResultSelall() 
{
	//避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;	
	CDataQuery *pDQ = pDoc->GetDataQuery();

	int num = m_pChkResManger->GetChkResCount();
	const ChkResultItem* pItem = NULL;

	pDoc->DeselectAll();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESSING));
	GProgressStart(num);
	for( int i=0; i<num; i++)
	{
		pItem = m_pChkResManger->GetChkResByIdx(i);
		
		if( pItem->arrFtrs.GetSize()<=0 )continue;	
		
		GProgressStep();
		for (int j=0;j<pItem->arrFtrs.GetSize();j++)
		{
			if( !pDQ->IsManagedObj(pItem->arrFtrs[j]) )
				continue;

			//若已被删除，则不再选中
			if( (pItem->arrFtrs[j])->IsDeleted() )
				continue;
			//

			pDoc->SelectObj(FtrToHandle(pItem->arrFtrs[j]),FALSE);
		}
		
	}
	GProgressEnd();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESS_END));
	pDoc->OnSelectChanged();
}

void CCheckResultViewBar::OnResultSelobj() 
{
	//避免文档指针m_pDaoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
 	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
 	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	CArray<CFeature*,CFeature*> arr;
	GetSelFtrs(arr);
	if( arr.GetSize()<=0 )return;

	CDataQuery *pDQ = pDoc->GetDataQuery();

	pDoc->DeselectAll();
	for(int i=0;i<arr.GetSize();++i)
	{
		if( !pDQ->IsManagedObj(arr[i]) )
			continue;

		//
		if( arr[i]->IsDeleted() )
			continue;
		//

		pDoc->SelectObj(FtrToHandle(arr[i]),FALSE);
	}
	pDoc->OnSelectChanged();

	LocateCurFtrs();
}


void CCheckResultViewBar::OnResultFilter()
{
	CDlgCheckList dlg;

	if( m_arrFilterNames.GetSize()<=0 )
	{
		CreateFilter();
	}

	dlg.m_arrStrList.Copy(m_arrFilterNames);
	dlg.m_arrFlags.Copy(m_arrFilterFlags);
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	m_arrFilterNames.Copy(dlg.m_arrStrList);
	m_arrFilterFlags.Copy(dlg.m_arrFlags);

	FillChkResItem();
}


void CCheckResultViewBar::CreateFilter()
{
	CStringArray arrNames;
	CArray<int,int> arrFlags;
	
	int num = m_pChkResManger->GetChkResCount();
	const ChkResultItem* pItem = NULL;
	
	for( int i=0; i<num; i++)
	{
		pItem = m_pChkResManger->GetChkResByIdx(i);
		
		if( pItem->arrFtrs.GetSize()<=0 )continue;	
		
		for( int j=0; j<arrNames.GetSize(); j++)
		{
			if( arrNames[j].CompareNoCase(pItem->reason)==0 )
			{
				break;
			}
		}
		if( j>=arrNames.GetSize() )
		{
			arrNames.Add(pItem->reason);
			arrFlags.Add(1);
		}
	}

	m_arrFilterNames.Copy(arrNames);
	m_arrFilterFlags.Copy(arrFlags);

}


void CCheckResultViewBar::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==4 )
	{
		UpdateDrawing((m_nTimerState&1)==0);
		m_nTimerState++;
		
		if( m_nTimerState>=6 )KillTimer(4);
	}
	else
		CDockablePane::OnTimer(nIDEvent);
}


void CCheckResultViewBar::UpdateDrawing(BOOL bDraw)
{
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	if( bDraw )
	{
		pDoc->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)&m_gr);
		pDoc->UpdateAllViews(NULL,hc_UpdateVariantDrag);
	}
	else
	{
		pDoc->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)NULL);
		pDoc->UpdateAllViews(NULL,hc_UpdateVariantDrag);
	}
}


void CCheckResultViewBar::SelectLastResult()
{
	CListCtrl0 * pmyListCtrl = &m_listCtrl;
	
	if( pmyListCtrl->GetItemCount()<=0 )
		return;
	
	UINT i, uSelectedCount = pmyListCtrl->GetSelectedCount();
	int  nItem = -1;
	if (uSelectedCount > 0)
	{
		nItem = pmyListCtrl->GetNextItem(nItem, LVNI_SELECTED);
		nItem = (nItem+pmyListCtrl->GetItemCount()-1)%pmyListCtrl->GetItemCount();
	}	
	else
	{
		nItem = pmyListCtrl->GetItemCount()-1;
	}

	for( int j=0; j<pmyListCtrl->GetItemCount(); j++)
	{		
		ChkResultItem *pItem = (ChkResultItem*)pmyListCtrl->GetItemData(nItem);
		if( pItem && pItem->state==1 )
		{
			nItem = (nItem+pmyListCtrl->GetItemCount()-1)%pmyListCtrl->GetItemCount();
		}
	}
	
	int index = -1;
	for( i=0; i<uSelectedCount; i++)
	{
		index = pmyListCtrl->GetNextItem(index, LVNI_SELECTED);
		pmyListCtrl->SetItemState(index, 0, LVIS_SELECTED);
	}
	
	pmyListCtrl->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	pmyListCtrl->EnsureVisible(nItem,FALSE);
	pmyListCtrl->Update(nItem); 
}


void CCheckResultViewBar::SelectNextResult()
{
	CListCtrl0 * pmyListCtrl = &m_listCtrl;

	if( pmyListCtrl->GetItemCount()<=0 )
		return;
	
	UINT i, uSelectedCount = pmyListCtrl->GetSelectedCount();
	int  nItem = -1;

	if (uSelectedCount > 0)
	{
		nItem = pmyListCtrl->GetNextItem(nItem, LVNI_SELECTED);
		nItem = (nItem+1)%pmyListCtrl->GetItemCount();
	}
	else
		nItem = 0;

	for( int j=0; j<pmyListCtrl->GetItemCount(); j++)
	{		
		ChkResultItem *pItem = (ChkResultItem*)pmyListCtrl->GetItemData(nItem);
		if( pItem && pItem->state==1 )
		{
			nItem = (nItem+1)%pmyListCtrl->GetItemCount();
		}
	}

	int index = -1;
	for( i=0; i<uSelectedCount; i++)
	{
		index = pmyListCtrl->GetNextItem(index, LVNI_SELECTED);
		pmyListCtrl->SetItemState(index, 0, LVIS_SELECTED);
	}

	pmyListCtrl->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	pmyListCtrl->EnsureVisible(nItem,FALSE);
	pmyListCtrl->Update(nItem); 
}


void CCheckResultViewBar::DeleteOrRestoreCurResult()
{
	//OnLocateItem();

	//删除或者恢复当前问题标记
	PT_3D pt;
	int index = -1;
	ChkResultItem *pItem = GetSelPos(&pt,&index);
	if( !pItem )
	{
		return;
	}

	CDocument *pDoc0 = GetCurDocument();
	if( !pDoc0 || !pDoc0->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
		return;

	CDlgDoc *pDoc = (CDlgDoc*)pDoc0;

	if( pItem->state==0 )
	{
		m_pChkResManger->DeleteMark(pDoc,pItem);
		
		pItem->state = 1;
		
		//将当前项目的问题状态设置为OK
		LVITEM item;
		item.mask = LVIF_IMAGE;
		item.iItem = index;
		item.iSubItem = 0;		
		
		m_listCtrl.GetItem(&item);
		
		item.iImage = 1;
		m_listCtrl.SetItem(&item);
	}
	else
	{
		m_pChkResManger->MarkItem(pDoc,pItem,GetIconIndex(pItem),GetIconColor(pItem),CChkResSettings::Obj()->GetResultIconSize());
		
		pItem->state = 0;
		
		//将当前项目的问题状态设置为OK
		LVITEM item;
		item.mask = LVIF_IMAGE;
		item.iItem = index;
		item.iSubItem = 0;		
		
		m_listCtrl.GetItem(&item);
		
		item.iImage = 0;
		m_listCtrl.SetItem(&item);
	}

	m_pChkResManger->Save(pDoc);
}


void CCheckResultViewBar::OnDestroy()
{
	if (m_ToolTip.GetSafeHwnd () != NULL)
	{
		m_ToolTip.DestroyWindow ();
	}

	CDockablePane::OnDestroy();
}


BOOL CCheckResultViewBar::PreTranslateMessage(MSG* pMsg) 
{
   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_MOUSEMOVE:
		if (m_ToolTip.GetSafeHwnd () != NULL)
		{
			m_ToolTip.RelayEvent(pMsg);
		}
	}

	if( pMsg->message==WM_LBUTTONDOWN )
	{
		int a=1;
	}
	
	return CDockablePane::PreTranslateMessage(pMsg);
}


BOOL CCheckResultViewBar::OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult)
{
	static CString strTipText;

	LPNMTTDISPINFO	pTTDispInfo	= (LPNMTTDISPINFO) pNMH;	

	pTTDispInfo->lpszText = NULL;
	memset(pTTDispInfo->szText,0,sizeof(pTTDispInfo->szText));

	if (m_ToolTip.GetSafeHwnd () == NULL)
	{
		return FALSE;
	}
	
	CPoint point;
	::GetCursorPos (&point);
	m_listCtrl.ScreenToClient (&point);

	int nItem = m_listCtrl.HitTest(point,NULL);
	if( nItem<0 )
		return FALSE;

	ChkResultItem *pItem = (ChkResultItem*)m_listCtrl.GetItemData(nItem);
	if( !pItem )
		return FALSE;

	strTipText = MakeObjInfo(pItem,TRUE);	
	
	pTTDispInfo->lpszText = const_cast<LPTSTR> ((LPCTSTR) strTipText);
	
	return TRUE;
}


void CCheckResultViewBar::UpdateToolTips2()
{
	return;
	int num = m_ToolTip.GetToolCount();

	for( int i=0; i<num; i++)
		m_ToolTip.DelTool(this,i+1);

	CRect rcList;
	m_listCtrl.GetWindowRect(rcList);
	ScreenToClient(&rcList);

	CRect rcItem, rcListClient;
	m_listCtrl.GetClientRect(&rcListClient);

	int nCount = 1;

	for( i=0; i<m_listCtrl.GetItemCount(); i++)
	{
		m_listCtrl.GetItemRect(i,&rcItem,LVIR_BOUNDS);

		CRect rci = rcItem&rcListClient;

		if( !rci.IsRectEmpty() )
		{
			rcItem.OffsetRect(rcList.TopLeft());
			m_ToolTip.AddTool(this,LPSTR_TEXTCALLBACK,&rcItem,nCount++);
		}
	}
}


void CCheckResultViewBar::OnResultLoad()
{
	if( m_pChkResManger )
	{
		m_pDoc = GetCurDocument();
		CreateFilter();

		if( m_pChkResManger->GetChkResCount()<=0 )
		{
			m_pChkResManger->Clear();			
			m_pChkResManger->Load();
			if( m_pDoc && m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
			{
				m_pChkResManger->DeleteAllMarks((CDlgDoc*)m_pDoc);
			}
		}
		else
		{
			if( m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
			{
				m_pChkResManger->DeleteAllMarks((CDlgDoc*)m_pDoc);
			}
			m_pChkResManger->Clear();			
			m_pChkResManger->Load();
		}

		FillChkResItem();
		UpdateToolTips2();

		OnResultMarkall();

		m_bResultIterate = TRUE;
	}
}

void CCheckResultViewBar::OnLoadRets()
{
	if (m_pChkResManger->GetChkResCount() == 0)
	{
		return;
	}
	TCHAR module[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, module, _MAX_PATH);
	CString path = module;
	int pos = path.ReverseFind('\\');
	if (pos>0)path = path.Left(pos);
	pos = path.ReverseFind('\\');
	if (pos>0)path = path.Left(pos);
	path = path + "\\History\\";

	CString filter = "检查文件(*.dat)|*.dat||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, filter);
	dlg.m_ofn.lpstrInitialDir = path;

	if (dlg.DoModal() != IDOK)
		return;
	CString filename = dlg.GetPathName();

	m_pDoc = GetCurDocument();
	if (!m_pDoc) return;
	CArray<ChkResultItem*, ChkResultItem*> arrChkRes_Compare;
	m_pChkResManger->Load(filename, arrChkRes_Compare);

	int count = m_pChkResManger->GetChkResCount();
	int count_Compare = arrChkRes_Compare.GetSize();
	GProgressStart(count);
	for (int i = 0; i < count; i++)
	{
		GProgressStep();
		ChkResultItem *pItem = m_pChkResManger->GetChkResByIdx(i);
		for (int h = 0; h < count_Compare; h++)
		{
			ChkResultItem *pTemp = arrChkRes_Compare.GetAt(h);
			if (pTemp->state != 0
				&& pItem->chkCmd == pTemp->chkCmd
				&& GraphAPI::GIsEqual3DPoint(&pItem->pos, &pTemp->pos)
				&& pItem->reason == pTemp->reason)
			{
				pItem->state = pTemp->state;
				m_pChkResManger->DeleteMark((CDlgDoc*)m_pDoc, pItem);
			}
		}
	}
	GProgressEnd();

	FillChkResItem();

	for (int h = 0; h < count_Compare; h++)
	{
		delete arrChkRes_Compare.GetAt(h);
		arrChkRes_Compare.GetAt(h) = NULL;
	}
}

void CCheckResultViewBar::OnSaveRet()
{
	if (m_listCtrl.GetItemCount() == 0)
	{
		return;
	}
	if ( !m_pDoc || !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
		return;

	CDlgDoc *pDoc = (CDlgDoc *)m_pDoc;
	CDlgDataSource *pDS = ((CDlgDoc *)m_pDoc)->GetDlgDataSource();
	if (!pDS) return;
	ChkResultItem *pItem0 = m_pChkResManger->GetChkResByIdx(0);
	if (pItem0 == NULL)
		return;

	TCHAR module[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, module, _MAX_PATH);
	CString path = module;
	int pos = path.ReverseFind('\\');
	if (pos>0)path = path.Left(pos);
	pos = path.ReverseFind('\\');
	if (pos>0)path = path.Left(pos);
	path = path + "\\History\\";

	CString filename = ExtractFileNameExt(pDS->GetName());
	filename = filename + "-" + pItem0->chkCmd + ".dat";
	CString filter;
	filter = "检查文件(*.dat)|*.dat||";
	CFileDialog dlg(FALSE, NULL, filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, filter, this, sizeof(OPENFILENAME));
	dlg.m_ofn.lpstrInitialDir = path;
	if (dlg.DoModal() == IDOK)
	{
		CString savepath = dlg.GetPathName();
		m_pChkResManger->Save(m_pDoc, (LPCTSTR)savepath);
	}
}

void CCheckResultViewBar::OnResultIterate()
{
	m_bResultIterate = !m_bResultIterate;
}


void CCheckResultViewBar::OnUpdateResultIterate(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bResultIterate);
}


void CCheckResultViewBar::RefreshResultStates()
{
	if( !m_pDoc )
		m_pDoc = GetCurDocument();

	if( !m_pDoc )
		return;

	CDocument *pDoc = m_pDoc;

	CDlgDataSource *pDS = NULL;
	CDataQueryEx *pDQ = NULL;
	
	if( pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
	{
		CDlgDoc *pd = NULL;
		pd = (CDlgDoc*)pDoc;
		pDS = pd->GetDlgDataSource();
		pDQ = pd->GetDataQuery();
	}

	if( pDS && pDQ )
	{
		ChkResultItem *pCurSelItem = GetSelPos(NULL,NULL);

		CArray<ChkResultItem*,ChkResultItem*> arrPItems;

		//首先检查目标地物位置是否发生了变化
		BOOL bChanged = FALSE;
		for( int i=0; i<m_pChkResManger->GetChkResCount(); i++)
		{
			ChkResultItem *pItem = (ChkResultItem*)m_pChkResManger->GetChkResByIdx(i);

			if(pItem->chkCmd==StrFromResID(IDS_CMDNAME_CURVESNAP))
				continue;

			if( pItem->state==0 )
			{
				for( int j=0; j<pItem->arrFtrs.GetSize(); j++)
				{
					Envelope e;
					e.CreateFromPtAndRadius(pItem->pos,1e-4);
					pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);
					int nFound = 0;
					const CPFeature *ftrs = pDQ->GetFoundHandles(nFound);
					
					for( int k=0; k<nFound; k++)
					{
						if( pItem->arrFtrs[j]==ftrs[k] )
							break;
					}
					
					if( k>=nFound )
					{
						pItem->state = 2;
						bChanged = TRUE;

						arrPItems.Add(pItem);
						break;
					}					
				}
			}
		}

		//其次检查目标地物位置的错误标记是否删除了
		pDQ->SetFilterIncludeLocked(TRUE);
		CFtrLayer *pLayer = pDS->GetFtrLayer(StrFromResID(IDS_MARKLAYER_NAME));
		if( !bChanged && pLayer!=NULL && pLayer->GetValidObjsCount()>0 )
		{
			for( i=0; i<m_pChkResManger->GetChkResCount(); i++)
			{
				ChkResultItem *pItem = (ChkResultItem*)m_pChkResManger->GetChkResByIdx(i);
				
				if( pItem->state==0 )
				{
					for( int j=0; j<pItem->arrFtrs.GetSize(); j++)
					{
						Envelope e;
						e.CreateFromPtAndRadius(pItem->pos,1e-6);
						pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);
						int nFound = 0;
						const CPFeature *ftrs = pDQ->GetFoundHandles(nFound);
						
						for( int k=0; k<nFound; k++)
						{
							if( pItem->arrFtrs[j]==ftrs[k] )
								break;
						}
						
						//当前地物没有修改
						if( k<nFound )
						{
							//错误标记被修改了
							for( k=0; k<nFound; k++)
							{
								if( pDS->GetFtrLayerOfObject(ftrs[k])==pLayer )
									break;
							}
							if( k>=nFound )
							{
								pItem->state = 2;
								bChanged = TRUE;

								arrPItems.Add(pItem);
								break;
							}
						}					
					}
				}
			}
		}
		pDQ->SetFilterIncludeLocked(FALSE);

		//再次检查修改时间是否变化
		if( !bChanged )
		{
			for( i=0; i<m_pChkResManger->GetChkResCount(); i++)
			{
				ChkResultItem *pItem = (ChkResultItem*)m_pChkResManger->GetChkResByIdx(i);
				
				if( pItem->state==0 )
				{
					for( int j=0; j<pItem->arrFtrs.GetSize(); j++)
					{
						if( pItem->arrFtrs[j]->GetModifyTime()>pItem->ntime )
						{
							pItem->state = 2;
							bChanged = TRUE;
							
							arrPItems.Add(pItem);
							break;
						}				
					}
				}
			}
		}

		if( bChanged )
		{
			FillChkResItem();

			for( i=0; i<arrPItems.GetSize(); i++)
			{
				m_pChkResManger->DeleteMark((CDlgDoc*)pDoc,arrPItems[i]);
			}

			if( pCurSelItem )
			{
				m_bStopLocateResultPt = TRUE;
				SelectItem(pCurSelItem,TRUE);
				
				m_bStopLocateResultPt = FALSE;
			}
		}
	}

}

void CCheckResultViewBar::OnRefreshResultStates()
{
	RefreshResultStates();

	if( !m_pDoc )
		m_pDoc = GetCurDocument();
	
	if( !m_pDoc )
		return;
	
	CDocument *pDoc = m_pDoc;
		
	if( pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
	{
		m_pChkResManger->Save((CDlgDoc*)pDoc);
	}
	
}

void CCheckResultViewBar::OnCloseDoc(CDocument *pDoc)
{
	if( pDoc==m_pDoc )
	{
		m_pChkResManger->Save(pDoc);

		m_pChkResManger->Clear();

		ClearAll();
	}
}


void CCheckResultViewBar::OnMarkSettings()
{
	CDlgMarkIconsSetting dlg;

	CChkResSettings *pCS = CChkResSettings::Obj();

	CDocument *pDoc = GetCurDocument();
	if (!pDoc) return;

	if( !pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;

	dlg.SetDoc((CDlgDoc*)pDoc,pCS);
	if( dlg.DoModal()!=IDOK )
		return;

	pCS->Save();
}


void CCheckResultViewBar::OnLocateItem()
{
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;

	CSelection *pSel = pDoc->GetSelection();
	CPFeature pFtr = HandleToFtr(pSel->GetLastSelectedObj());
	if( pFtr && m_pChkResManger )
	{
		for( int i=0; i<m_pChkResManger->GetChkResCount(); i++)
		{
			ChkResultItem *pItem = (ChkResultItem*)m_pChkResManger->GetChkResByIdx(i);
			
			for( int j=0; j<pItem->arrFtrs.GetSize(); j++)
			{
				if( pFtr==pItem->arrFtrs[j] || pFtr==pItem->pFtrMark )
				{
					m_bStopLocateResultPt = TRUE;
					SelectItem(pItem,TRUE);

					m_bStopLocateResultPt = FALSE;

					return;
				}
			}
		}
	}
}


void CCheckResultViewBar::SelectItem(int index, BOOL bScrollCtrl)
{
	for( int k=0; k<m_listCtrl.GetItemCount(); k++)
	{
		if( k==index )
		{
			m_listCtrl.SetItemState(k,LVIS_SELECTED,LVIS_SELECTED );
			if( bScrollCtrl )m_listCtrl.EnsureVisible(k,FALSE);
		}
		else
		{
			m_listCtrl.SetItemState(k,0,LVIS_SELECTED );
		}
	}
}


void CCheckResultViewBar::SelectItem(const ChkResultItem *pItem, BOOL bScrollCtrl)
{
	for( int k=0; k<m_listCtrl.GetItemCount(); k++)
	{
		if( (DWORD_PTR)pItem==m_listCtrl.GetItemData(k) )
		{
			m_listCtrl.SetItemState(k,LVIS_SELECTED,LVIS_SELECTED );
			if( bScrollCtrl )m_listCtrl.EnsureVisible(k,FALSE);
		}
		else
		{
			m_listCtrl.SetItemState(k,0,LVIS_SELECTED );
		}
	}
}

ChkResultItem *CCheckResultViewBar::GetCurItem()
{
	return GetSelPos(NULL,NULL);
}



void CCheckResultViewBar::SetItemState(ChkResultItem *pItem, int state)
{
	if( state<0 || state>2 )
		return;
	
	CDocument *pDoc0 = GetCurDocument();
	if( !pDoc0 || !pDoc0->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
		return;
	
	CDlgDoc *pDoc = (CDlgDoc*)pDoc0;
	
	int index = 0;
	
	if( pItem==NULL )
		return;

	m_bStopLocateResultPt = TRUE;
	
	pItem->state = state;
	
	for( int j=0; j<m_listCtrl.GetItemCount(); j++)
	{
		if( m_listCtrl.GetItemData(j)==(DWORD_PTR)pItem )
		{
			//将当前项目的问题状态设置为OK
			LVITEM item;
			item.mask = LVIF_IMAGE;
			item.iItem = j;
			item.iSubItem = 0;	
			
			m_listCtrl.GetItem(&item);
			
			item.iImage = state;
			m_listCtrl.SetItem(&item);
		}
	}
	
	if( state!=0 )
		m_pChkResManger->DeleteMark(pDoc,pItem);
	else
		m_pChkResManger->MarkItem(pDoc,pItem,GetIconIndex(pItem),GetIconColor(pItem),CChkResSettings::Obj()->GetResultIconSize());

	m_bStopLocateResultPt = FALSE;
	
}

void CCheckResultViewBar::SetAllStates(int state)
{
	if( state<0 || state>2 )
		return;

	CDocument *pDoc0 = GetCurDocument();
	if( !pDoc0 || !pDoc0->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
		return;

	m_bStopLocateResultPt = TRUE;
	
	CDlgDoc *pDoc = (CDlgDoc*)pDoc0;

	int nCount = m_pChkResManger->GetChkResCount();
	for( int i=0; i<nCount; i++)
	{
		ChkResultItem *pItem = m_pChkResManger->GetChkResByIdx(i);
		if( !pItem )
		{
			continue;
		}

		pItem->state = state;

		for( int j=0; j<m_listCtrl.GetItemCount(); j++)
		{
			if (m_listCtrl.GetItemData(j) == (DWORD_PTR)pItem)
			{
				//将当前项目的问题状态设置为OK
				LVITEM item;
				item.mask = LVIF_IMAGE;
				item.iItem = j;
				item.iSubItem = 0;	

				m_listCtrl.GetItem(&item);

				item.iImage = state;
				m_listCtrl.SetItem(&item);
			}
		}

		if( state!=0 )
			m_pChkResManger->DeleteMark(pDoc,pItem);
		else
			m_pChkResManger->MarkItem(pDoc,pItem,GetIconIndex(pItem),GetIconColor(pItem),CChkResSettings::Obj()->GetResultIconSize());
		
	}

	m_bStopLocateResultPt = FALSE;
}

static int CALLBACK MyCompareProc(LPARAM  lParam1, LPARAM  lParam2, LPARAM  lParamSort)  
{
	CListCtrl0* pListCtrl = (CListCtrl0*)lParamSort; 
	int nItem1,nItem2;
	LVFINDINFO FindInfo;
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=lParam1;
	nItem1=pListCtrl->FindItem(&FindInfo,-1);
	FindInfo.lParam=lParam2;
	nItem2=pListCtrl->FindItem(&FindInfo,-1);
	if((nItem1==-1)||(nItem2==-1))
	{
		return 0;
	}
 	CString    strItem1  = pListCtrl->GetItemText(nItem1, pListCtrl->m_nSortCol);  
 	CString    strItem2  = pListCtrl->GetItemText(nItem2, pListCtrl->m_nSortCol); 

	int n1=0,n2=0;
	if(strItem1.IsEmpty() && strItem2.IsEmpty())
	{
		ChkResultItem *pItem1 = (ChkResultItem*)pListCtrl->GetItemData(nItem1);
		n1 = pItem1->state;
		
		ChkResultItem *pItem2 = (ChkResultItem*)pListCtrl->GetItemData(nItem2);
		n2 = pItem2->state;
	}
	else
	{
		n1=atoi((LPCTSTR)strItem1);
		n2=atoi((LPCTSTR)strItem2);
	}

	int rval = 0;
	if(n1==0 && n2==0)
	{
		rval = strItem1.Compare(strItem2);
	}
	else
	{
		if(n1>n2) rval = 1;
		else rval = -1;
	}
	if(!pListCtrl->m_bAscending)
	{
		rval = -rval;
	}
	return rval;
}

void CCheckResultViewBar::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMListView->iSubItem==m_listCtrl.m_nSortCol)
	{
		m_listCtrl.m_bAscending = !m_listCtrl.m_bAscending;
	}
	else
	{
		m_listCtrl.m_bAscending = TRUE;
		m_listCtrl.m_nSortCol = pNMListView->iSubItem;
	}

	m_listCtrl.SortItems(MyCompareProc, (LPARAM)&m_listCtrl);       
	*pResult = 0;
}


CFeature* MarkErrorPt(ChkResultItem *pItem, double lfMarkWid )
{	
	CFeature *pFtr = new CFeature();	
	CGeometry *pObj = new CGeoCurve();

	pFtr->SetGeometry(pObj);
	
	pObj->SetColor(RGB(255,0,0));	
	
	CArray<PT_3DEX,PT_3DEX> pts;
	PT_3D pt = pItem->pos;
	PT_3DEX expt;
	COPY_3DPT(expt,pt);
	
	expt.x = pt.x-lfMarkWid;  expt.y = pt.y-lfMarkWid; expt.pencode = penMove;
	pts.Add(expt);
	expt.x = pt.x+lfMarkWid;  expt.y = pt.y+lfMarkWid; expt.pencode = penLine;
	pts.Add(expt);
	
	//移动中心点
	// 	expt.x = pt.x; expt.y = pt.y; expt.pencode = penLine;
	// 	pts.Add(expt);
	
	expt.x = pt.x-lfMarkWid;  expt.y = pt.y+lfMarkWid; expt.pencode = penMove;
	pts.Add(expt);
	expt.x = pt.x+lfMarkWid;  expt.y = pt.y-lfMarkWid; expt.pencode = penLine;
	pts.Add(expt);
	
	pObj->CreateShape(pts.GetData(),pts.GetSize());
	
	return pFtr;
}

void CCheckResultViewBar::OnExport()
{
	CString title = StrFromResID(IDS_NEWFILE_TITLE);
	CString filter = "*.dxf|*.dxf|*.dwg|*.dwg|*.csv|*.csv||";

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	dlg.m_pOFN->lpstrTitle = title;
	if( dlg.DoModal()!=IDOK )return;

	CString filePath = dlg.GetPathName();
	CString ext;
	DWORD nIndex = dlg.m_pOFN->nFilterIndex;
	if (nIndex == 1)
		ext = ".dxf";
	else if (nIndex == 2)
		ext = ".dwg";
	else if (nIndex == 3)
		ext = ".csv";
	else
		return;

	if(filePath.Right(4).CompareNoCase(ext)!=0)
	{
		filePath += ext;
	}

	CDlgDoc *pDoc = GetActiveDlgDoc();
	CDataQueryEx *pDQ = (pDoc==NULL?NULL:pDoc->GetDataQuery());
	double scale = 1.0;
	
	if(pDoc!=NULL)
	{
		scale = pDoc->GetDlgDataSource()->GetSymbolDrawScale();
	}
	
	if(ext.CompareNoCase(".dxf")==0 || ext.CompareNoCase(".dwg")==0)
	{
		if(pDoc==NULL)
			return;

		float icon_size = CChkResSettings::Obj()->GetResultIconSize();
		
		int nsz = m_pChkResManger->GetChkResCount();
		CPtrArray ftrs;

		for( int i=0; i<nsz; i++)
		{
			ChkResultItem *pItem = m_pChkResManger->GetChkResByIdx(i);				

			CFeature *pFtr1 = MarkErrorPt(pItem,scale*icon_size*0.5);
			ftrs.Add(pFtr1);

			CFeature *pFtr2 = new CFeature();
			CGeoText *pText = new CGeoText();
			pFtr2->SetGeometry(pText);

			pText->SetText(pItem->reason + "(" + pItem->chkCmd + ")");

			TEXT_SETTINGS0 settings;
			pText->GetSettings(&settings);

			settings.fHeight = 2.0;
			settings.nAlignment = TAH_LEFT|TAV_MID;
			pText->SetSettings(&settings);

			PT_3DEX expt;
			expt.pencode = penLine;
			expt.x = pItem->pos.x + 1.0*scale; expt.y = pItem->pos.y; expt.z = pItem->pos.z;
			pText->CreateShape(&expt,1);
			ftrs.Add(pFtr2);
		}

		if(ftrs.GetSize()>0)
		{
			CDxfWrite dxfWrite;
			dxfWrite.SetDlgDataSource(pDoc->GetDlgDataSource());
			dxfWrite.SetAsExportBaselines();
			CPtrArray arrLayer;
			
			if(ext.CompareNoCase(".dwg")==0)
				dxfWrite.ObjectsToDwg(ftrs,arrLayer,filePath);
			else
				dxfWrite.ObjectsToDxf(ftrs,arrLayer,filePath);

			AfxMessageBox("OK!");
		}
	}
	else if(ext.CompareNoCase(".csv")==0)
	{
		FILE *fp = fopen(filePath,"wt");
		if(!fp)
			return;
		
		int nsz = m_pChkResManger->GetChkResCount();

		fprintf(fp,"index,name,reason,x,y,z,state\n");	

		for( int i=0; i<nsz; i++)
		{
			ChkResultItem *pItem = m_pChkResManger->GetChkResByIdx(i);

			fprintf(fp,"%d,",i+1);			

			fprintf(fp,(LPCTSTR)pItem->chkCmd);
			fprintf(fp,",");

			fprintf(fp,(LPCTSTR)pItem->reason);
			fprintf(fp,",");

			fprintf(fp,"%.4f,%.4f,%.4f,",pItem->pos.x,pItem->pos.y,pItem->pos.z);
			fprintf(fp,"%d\n",pItem->state);
		}
		
		fclose(fp);
		AfxMessageBox("OK!");
	}
}

LRESULT CCheckResultViewBar::OnClickIcon(WPARAM wParam, LPARAM lParam)
{
	int state = 0;
	ChkResultItem *pCurItem = (ChkResultItem*)m_listCtrl.GetItemData(wParam);
	if(pCurItem)
	{
		state = pCurItem->state==0 ? 1 : 0;
	}

	LVITEM item;
	item.mask = LVIF_IMAGE;
	item.iSubItem = 0;

	POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		int iItem = m_listCtrl.GetNextSelectedItem(pos);
		ChkResultItem *pItem = (ChkResultItem*)m_listCtrl.GetItemData(iItem);
		if( pItem )
		{
			if(pItem->state==state) continue;

			pItem->state = state;
			
			CDocument *pDoc0 = GetCurDocument();
			if( pDoc0 && pDoc0->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
			{
				CDlgDoc *pDoc = (CDlgDoc*)pDoc0;
				CDataQuery *pDQ = pDoc->GetDataQuery();
				
				CPFeature pMark = pItem->pFtrMark;
				
				if( pItem->state==0 )
				{
					float szIcon = CChkResSettings::Obj()->GetResultIconSize();
					
					m_pChkResManger->MarkItem(pDoc,pItem,GetIconIndex(pItem),GetIconColor(pItem),szIcon);	
				}
				else if( pItem->state==1 )
				{
					m_pChkResManger->DeleteMark(pDoc,pItem);
				}
				
				pDoc->UpdateAllViews(NULL,hc_Refresh);
			}
		}
		
		item.iItem = iItem;
		item.iImage = state;
		m_listCtrl.SetItem(&item);
	}
	return 0;
}