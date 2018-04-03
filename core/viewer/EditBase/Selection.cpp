// EBSelection.cpp: implementation of the CSelection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "Selection.h"
#include "SmartViewFunctions.h"
#include "regdef.h"
#include "RegDef2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

SelNodeList::SelNodeList()
{
	head = tail = NULL;
}

SelNodeList::~SelNodeList()
{
	delAll();
}

void SelNodeList::delAll()
{
	SelNode *p1 = head, *p2;
	while( p1!=NULL )
	{
		p2 = p1;
		p1 = p1->next;
		delete p2;			
	}

	head = tail = NULL;
}

void SelNodeList::del(SelNode *nod)
{
	if( head==nod )
	{
		if( tail==head )
		{
			head = NULL;
			tail = NULL;
			
			delete nod;
		}
		else
		{
			head = nod->next;
			head->prev = NULL;
			
			delete nod;
		}
	}
	else if( tail==nod )
	{
		tail = nod->prev;
		tail->next = NULL;
		
		delete nod;
	}
	else
	{
		SelNode *prev = nod->prev;
		SelNode *next = nod->next;
		prev->next = next;
		next->prev = prev;
		
		delete nod;
	}
}


void SelNodeList::add(SelNode *nod)
{
	if( head==NULL )
		head = nod;
	if( tail==NULL )
		tail = nod;
	else
	{
		tail->next = nod;
		nod->next = NULL;
		nod->prev = tail;
		tail = nod;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSelection::CSelection():
m_arrSortedSelObjs(1024)
{
	m_bGetSelChanged = FALSE;
	m_lfSelRadius = gdef_nSelectSize ;
	UpdateSettings(FALSE);
}

CSelection::~CSelection()
{

}

double CSelection::GetSelectRadius()const
{
	return m_lfSelRadius;
}

void CSelection::SetSelectRadius(double r)
{
	m_lfSelRadius = r;
}

void CSelection::SelectObj(FTR_HANDLE handle)
{
	if (handle == 0) return;

	int i;
 	if(FindObj(handle,i))return;
// 	m_arrSelObjs.Add(handle);
	SelNode *nod = new SelNode;
	if( !nod )return;	
	nod->handle = handle;
	m_arrSortedSelObjs.InsertAt(i,nod);	
	m_arrSelObjList.add(nod);
	m_bGetSelChanged = TRUE;

	SELCHG_STATE s(handle,SELCHG_TOBESEL);
	m_arrSelChgObjs.Add(s);
}


void CSelection::SelectObj(const FTR_HANDLE *handles, int num)
{
	CArray<FTR_HANDLE,FTR_HANDLE> arr;
	arr.SetSize(num);
	memcpy(arr.GetData(),handles,sizeof(FTR_HANDLE)*num);
	for (int i=0;i<num;i++)
	{
		if (arr[i] == 0) continue;

		int insert_pos;
		if(!FindObj(arr[i],insert_pos))
		{
			SelNode *nod = new SelNode;
			if( !nod )return;	
			nod->handle = arr[i];
			m_arrSortedSelObjs.InsertAt(insert_pos,nod);	
			m_arrSelObjList.add(nod);
			m_bGetSelChanged = TRUE;
//			m_arrSelObjs.Add(arr[i]);
			m_arrSelChgObjs.Add(SELCHG_STATE(arr[i],SELCHG_TOBESEL));
		}
	}	
}

BOOL CSelection::FindObj(FTR_HANDLE handle, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrSortedSelObjs.GetSize()<=0 )
	{ 
		findidx = 0;
		bnew = 1; 
	}
	else
	{
		int i0 = 0, i1 = m_arrSortedSelObjs.GetSize()-1, i2;
		while (findidx == -1) 
		{
			if( handle<=m_arrSortedSelObjs.GetAt(i0)->handle )
			{
				bnew = (handle==m_arrSortedSelObjs.GetAt(i0)->handle?0:1);
				findidx = i0;
				break;
			}
			else if( handle>=m_arrSortedSelObjs.GetAt(i1)->handle )
			{
				bnew = (handle==m_arrSortedSelObjs.GetAt(i1)->handle?0:1);
				findidx = (handle==m_arrSortedSelObjs.GetAt(i1)->handle?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (handle==m_arrSortedSelObjs.GetAt(i0)->handle?0:1);
				findidx = (handle==m_arrSortedSelObjs.GetAt(i0)->handle?i0:(i0+1));
				break;
			}
			
			if( handle<m_arrSortedSelObjs.GetAt(i2)->handle )
				i1 = i2;
			else if( handle>m_arrSortedSelObjs.GetAt(i2)->handle )
				i0 = i2;
			else
			{
				findidx = i2; bnew = 0;
				break;
			}
		}
	}
	
	insert_idx = findidx;
	return (bnew==0);
}

void CSelection::SelectAll(const FTR_HANDLE *handles, int num)
{
	DeselectAll();
	CArray<FTR_HANDLE,FTR_HANDLE> arr;
	arr.SetSize(num);
	memcpy(arr.GetData(),handles,sizeof(FTR_HANDLE)*num);
//	m_arrSelObjs.Append(arr);
	SELCHG_STATE s;
	s.tobesel= SELCHG_TOBESEL;
	for (int i=0;i<num;i++)
	{	
		if (arr[i] == 0) continue;

		int insert_pos;
		if (!FindObj(arr[i],insert_pos))
		{
			SelNode *nod = new SelNode;
			if( !nod )return;	
			nod->handle = arr[i];
			m_arrSortedSelObjs.InsertAt(insert_pos,nod);	
			m_arrSelObjList.add(nod);
		}

		s.handle = arr[i];
		m_arrSelChgObjs.Add(s);
	}

	m_bGetSelChanged = TRUE;
}

void CSelection::DeselectObj(FTR_HANDLE handle)
{
	int i;
	if (FindObj(handle,i))
	{
		SELCHG_STATE s(handle,SELCHG_TOBEUNSEL);
		m_arrSelChgObjs.Add(s);

//		m_arrSelObjs.RemoveAt(i);
		SelNode *nod = m_arrSortedSelObjs.GetAt(i);	
		m_arrSortedSelObjs.RemoveAt(i);
		m_arrSelObjList.del(nod);
		m_bGetSelChanged = TRUE;
	}
// 	for( int i=m_arrSelObjs.GetSize()-1; i>=0; i--)
// 	{
// 		if( m_arrSelObjs[i]==handle )
// 		{
// 			SELCHG_STATE s(handle,SELCHG_TOBEUNSEL);
// 			m_arrSelObjs.RemoveAt(i);
// 			m_arrSelChgObjs.Add(s);			
// 			break;
// 		}
// 	}
}


void CSelection::DeselectAll()
{
	int size = m_arrSortedSelObjs.GetSize();
	SELCHG_STATE s;
	for( int i=0; i<size; i++)
	{
		SelNode *nod = m_arrSortedSelObjs.GetAt(i);
		if (!nod) continue;
		s.handle = nod->handle;
		s.tobesel= SELCHG_TOBEUNSEL;
		m_arrSelChgObjs.Add(s);
	}
	
	m_arrSelObjList.delAll();
	m_arrSortedSelObjs.RemoveAll();

	m_bGetSelChanged = TRUE;

/*
	SELCHG_STATE s;
	for( int i=m_arrSelObjs.GetSize()-1; i>=0; i--)
	{
		s.handle = m_arrSelObjs[i];
		s.tobesel= SELCHG_TOBEUNSEL;
		m_arrSelChgObjs.Add(s);
	}
	if( m_arrSelObjs.GetSize()>0 )
	{
		m_arrSelObjs.RemoveAll();
//		m_bSelChanged = TRUE;
	}
*/
}

BOOL CSelection::IsObjInSelection(FTR_HANDLE handle)
{
	int insert_pos;
	return FindObj(handle,insert_pos);
// 	int num = m_arrSelObjs.GetSize();
// 	const FTR_HANDLE *handles = m_arrSelObjs.GetData();
// 	for( int i=0; i<num; i++)
// 	{
// 		if( handles[i]==handle )
// 			return TRUE;
// 	}
// 	return FALSE;
}


FTR_HANDLE CSelection::GetLastSelectedObj()const
{
// 	if( m_arrSelObjs.GetSize()>0 )
// 		return m_arrSelObjs.GetAt(0);
	if (m_arrSelObjList.tail)
	{
		return m_arrSelObjList.tail->handle;
	}
	return NULL_HANDLE;
}

const FTR_HANDLE *CSelection::GetSelectedObjs(int &num)
{
	if (m_bGetSelChanged)
	{
		m_arrSelObjs.RemoveAll();
		SelNode *nod = m_arrSelObjList.tail;
		while(nod)
		{
			m_arrSelObjs.Add(nod->handle);
			nod = nod->prev;
		}

		m_bGetSelChanged = FALSE;
	}

 	num = m_arrSelObjs.GetSize();
 	return m_arrSelObjs.GetData();
}

int CSelection::GetSelChgObjs(SELCHG_STATE *states)const
{
	if( states )
	{
		memcpy(states,m_arrSelChgObjs.GetData(),sizeof(SELCHG_STATE)*m_arrSelChgObjs.GetSize());
	}
	return m_arrSelChgObjs.GetSize();
	
}



void CSelection::DeselectAllBut(FTR_HANDLE handle)
{
	
	for( int i=m_arrSortedSelObjs.GetSize()-1; i>=0; i--)
	{
		if( m_arrSortedSelObjs[i]->handle!=handle )
		{
			SELCHG_STATE s(m_arrSortedSelObjs[i]->handle,SELCHG_TOBEUNSEL);
			m_arrSelChgObjs.Add(s);

			SelNode *nod = m_arrSortedSelObjs.GetAt(i);	
			m_arrSortedSelObjs.RemoveAt(i);
			m_arrSelObjList.del(nod);
			m_bGetSelChanged = TRUE;
//			m_arrSelObjs.RemoveAt(i);
//			m_bSelChanged = TRUE;
		}
	}
}

void CSelection::EraseChangeFlag()
{
//	m_bSelChanged = FALSE;
	m_arrSelChgObjs.RemoveAll();
}

void CSelection::UpdateSettings(BOOL bSave)
{
	CWinApp *pApp = AfxGetApp();
	if (!pApp) return;

	if( !bSave )
	{
		m_lfSelRadius = pApp->GetProfileInt(REGPATH_SELECT, REGITEM_SELECTSIZE, (int)m_lfSelRadius);
	}
	else
	{
		m_lfSelRadius = pApp->WriteProfileInt(REGPATH_SELECT, REGITEM_SELECTSIZE, (int)m_lfSelRadius);
	}

}

FTR_HANDLE CSelection::GetNearestInCanSelObjs(PT_3D pt, CCoordSys *pCS)
{
	if( m_arrCanSelObjs.GetSize()<=0 )
		return NULL;

	if( m_arrCanSelObjs.GetSize()==1 )
		return m_arrCanSelObjs[0];

	Envelope sch_evlp;
	sch_evlp.CreateFromPtAndRadius(pt,1e+10);

	PT_3D ret;
	double dis, mindis;
	int k = -1;

	for( int i=0; i<m_arrCanSelObjs.GetSize(); i++)
	{
		CGeometry *pGeo = HandleToFtr(m_arrCanSelObjs[i])->GetGeometry();

		pGeo->FindNearestBasePt(pt,sch_evlp,pCS,&ret,&dis);

		if( i==0 || dis<mindis )
		{
			mindis = dis;
			k = i;
		}
	}

	return m_arrCanSelObjs[k];
}

void CSelection::RemoveOneFromCanSelObjs(FTR_HANDLE hFtr)
{
	for( int i=m_arrCanSelObjs.GetSize()-1; i>=0; i--)
	{
		if( hFtr==m_arrCanSelObjs[i] )
		{
			m_arrCanSelObjs.RemoveAt(i);
		}
	}
}


MyNameSpaceEnd


