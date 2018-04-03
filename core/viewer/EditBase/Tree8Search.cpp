// Tree8Search.cpp: implementation of the CTree8Search class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tree8Search.h"
#include "SmartViewFunctions.h"
#include "Linearizer.h"
#include "SmartViewFunctions.h"
#include "GeoPoint.h"
#include "stdlib.h"
#include "search.h"
#include "Feature.h"
#include "symbolbase.h"

#define _FABS(x)			((x)>=0?(x):(-(x)))
#define Max_ObjofNode		512

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin



// e, ground coordinate system; e1, view coordinate system;
bool GIsIntersectEnvelope(const Envelope *e, const Envelope *e1, CCoordSys *pCS)
{
	if( pCS==NULL )
	{
		return e1->bIntersect(e,2);
	}

	PT_4D pts[8];
	pts[0].x = e->m_xl, pts[0].y = e->m_yl, pts[0].z = e->m_zl; pts[0].yr = 0;
	pts[1].x = e->m_xh, pts[1].y = e->m_yl, pts[1].z = e->m_zl; pts[1].yr = 0;
	pts[2].x = e->m_xh, pts[2].y = e->m_yh, pts[2].z = e->m_zl; pts[2].yr = 0;
	pts[3].x = e->m_xl, pts[3].y = e->m_yh, pts[3].z = e->m_zl; pts[3].yr = 0;
	pts[4].x = e->m_xl, pts[4].y = e->m_yl, pts[4].z = e->m_zh; pts[4].yr = 0;
	pts[5].x = e->m_xh, pts[5].y = e->m_yl, pts[5].z = e->m_zh; pts[5].yr = 0;
	pts[6].x = e->m_xh, pts[6].y = e->m_yh, pts[6].z = e->m_zh; pts[6].yr = 0;
	pts[7].x = e->m_xl, pts[7].y = e->m_yh, pts[7].z = e->m_zh; pts[7].yr = 0;

	pCS->GroundToClient(pts,8);
	
	Envelope e2;
	e2.CreateFromPts(pts,8,sizeof(PT_4D));
	
	return e2.bIntersect(e1,2);
	
}

// e, ground coordinate system; pt, view coordinate system;
bool GIsInEnvelope(const Envelope *e, const PT_3D *pt, CCoordSys *pCS)
{
	if( pCS==NULL )
	{
		return e->bPtIn(pt);
	}

	PT_4D pts[8];
	pts[0].x = e->m_xl, pts[0].y = e->m_yl, pts[0].z = e->m_zl; pts[0].yr = 0;
	pts[1].x = e->m_xh, pts[1].y = e->m_yl, pts[1].z = e->m_zl; pts[1].yr = 0;
	pts[2].x = e->m_xh, pts[2].y = e->m_yh, pts[2].z = e->m_zl; pts[2].yr = 0;
	pts[3].x = e->m_xl, pts[3].y = e->m_yh, pts[3].z = e->m_zl; pts[3].yr = 0;
	pts[4].x = e->m_xl, pts[4].y = e->m_yl, pts[4].z = e->m_zh; pts[4].yr = 0;
	pts[5].x = e->m_xh, pts[5].y = e->m_yl, pts[5].z = e->m_zh; pts[5].yr = 0;
	pts[6].x = e->m_xh, pts[6].y = e->m_yh, pts[6].z = e->m_zh; pts[6].yr = 0;
	pts[7].x = e->m_xl, pts[7].y = e->m_yh, pts[7].z = e->m_zh; pts[7].yr = 0;
	
	pCS->GroundToClient(pts,8);
	
	Envelope e2;
	e2.CreateFromPts(pts,8,sizeof(PT_4D));
	
	return e2.bPtIn(pt);
	
}

bool GIsPtOnLine(PT_2D pt, PT_2D *pts)
{
	double dis1 = GraphAPI::GGet2DDisOf2P(pt,pts[0]) + GraphAPI::GGet2DDisOf2P(pt,pts[1]);
	double dis2 = GraphAPI::GGet2DDisOf2P(pts[0],pts[1]);
	if (dis1-dis2 < 0.01) return TRUE;
	else return FALSE;
}
/*
double FindMinimumDistance(PT_3D pt, const SearchableUnit *pu, CCoordSys *pCS, PT_3D *pRet);
{
	if( !GIsInEnvelope(&GetEnvelopeOfSearchableUnit(pu),&pt,pCS) )
		return -1;
	
	bAdd = FALSE;
	
	if( IsSearchablePoint(pu) )
	{
		return 1;
	}
	else if( IsSearchableLine(pu) )
	{
		const CShapeLine::ShapeLineUnit *pLine = (const CShapeLine::ShapeLineUnit*)(((const SearchableLine*)pu)->p);
		const PT_3DEX *pts = pLine->pts;
		
		for( int k=0; k<pLine->nuse-1; k++,pts++)
		{
			pCS->GroundToClient(&pts[0],&pt1);
			pCS->GroundToClient(&pts[1],&pt2);
			if( e.bIntersect(&pt1,&pt2,2) )break;
		}
		
		pCS->GroundToClient(&pts[0],&pt1);
		if( k<pLine->nuse-1 || (pLine->nuse==1 && e.bPtIn(&pt1,2)) )
		{
			bAdd = TRUE;
		}
	}
	else if( IsSearchableObj(pu) )
	{
		bAdd = ((SearchableObj*)pu)->FindObj(sr,NULL);
	}
	
	if( bAdd )
	{
		m_foundNodeObjs.Add(buf[i]);
		
		long *ids = m_foundIds.GetData(), id = buf[i].grid;
		int j,js,je,nid = m_foundIds.GetSize();
		
		j = nid/2, js = 0, je = nid-1;
		while( j!=js )
		{
			if( id>ids[j] )
				js = j;
			else if( id<ids[j])
				je = j;
			else break;
			
			j = (js+je)/2;
		}
		
		if( j>=nid )
		{
			m_foundIds.Add(id);				
		}
		else if( id>ids[j] )
		{
			m_foundIds.InsertAt(j+1,id);
		}
		else if( id<ids[j] )
		{
			m_foundIds.InsertAt(j,id);
		}
	}

}

*/
#define RADIUS(e)	(_FABS(e.m_xh-e.m_xl)+_FABS(e.m_yh-e.m_yl)+_FABS(e.m_zh-e.m_zl))

Tree8Node::Tree8Node()
{
	memset(son,0,sizeof(son));
	parent = NULL;
	vsum = 0;
	objs.SetSize(0,Max_ObjofNode);
}


void FreeTree(Tree8Node *t)
{
	if( !t )return;
	t->objs.RemoveAll();
	
	if( t->son[0] )
	{
		for( int i=0; i<8; i++)
		{
			FreeTree(t->son[i]);			
		}
	}

	delete t;
}

//在当前节点下创建8个叶子节点，采用均匀的创建方法；
BOOL Tree8Node::CreateSon0()
{
	// s 是子节点的范围尺寸相对于父节点的比例系数，例如，0.5表示子节点的尺寸是父节点的一半；t = 1-s；
	float s = 0.6f;
	float t = 0.4f;

	for( int i=0; i<8; i++)
	{
		son[i] = new Tree8Node;
		if( !son[i] )return FALSE;

		son[i]->parent = this;

		switch(i) 
		{
		case 0:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = e0.m_xl+(e0.m_xh-e0.m_xl)*s;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = e0.m_yl+(e0.m_yh-e0.m_yl)*s;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = e0.m_zl+(e0.m_zh-e0.m_zl)*s;
			break;
		case 1:
			son[i]->e0.m_xl = e0.m_xl+(e0.m_xh-e0.m_xl)*t; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = e0.m_yl+(e0.m_yh-e0.m_yl)*s;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = e0.m_zl+(e0.m_zh-e0.m_zl)*s;
			break;
		case 2:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = e0.m_xl+(e0.m_xh-e0.m_xl)*s;
			son[i]->e0.m_yl = e0.m_yl+(e0.m_yh-e0.m_yl)*t; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = e0.m_zl+(e0.m_zh-e0.m_zl)*s;
			break;
		case 3:
			son[i]->e0.m_xl = e0.m_xl+(e0.m_xh-e0.m_xl)*t; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = e0.m_yl+(e0.m_yh-e0.m_yl)*t; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = e0.m_zl+(e0.m_zh-e0.m_zl)*s;
			break;
		case 4:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = e0.m_xl+(e0.m_xh-e0.m_xl)*s;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = e0.m_yl+(e0.m_yh-e0.m_yl)*s;
			son[i]->e0.m_zl = e0.m_zl+(e0.m_zh-e0.m_zl)*t; son[i]->e0.m_zh = e0.m_zh;
			break;
		case 5:
			son[i]->e0.m_xl = e0.m_xl+(e0.m_xh-e0.m_xl)*t; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = e0.m_yl+(e0.m_yh-e0.m_yl)*s;
			son[i]->e0.m_zl = e0.m_zl+(e0.m_zh-e0.m_zl)*t; son[i]->e0.m_zh = e0.m_zh;
			break;
		case 6:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = e0.m_xl+(e0.m_xh-e0.m_xl)*s;
			son[i]->e0.m_yl = e0.m_yl+(e0.m_yh-e0.m_yl)*t; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = e0.m_zl+(e0.m_zh-e0.m_zl)*t; son[i]->e0.m_zh = e0.m_zh;
			break;
		case 7:
			son[i]->e0.m_xl = e0.m_xl+(e0.m_xh-e0.m_xl)*t; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = e0.m_yl+(e0.m_yh-e0.m_yl)*t; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = e0.m_zl+(e0.m_zh-e0.m_zl)*t; son[i]->e0.m_zh = e0.m_zh;
			break;
		default:;
		}
	}	

	return TRUE;
}


//在当前节点下创建8个叶子节点，采用非均匀的创建方法；
BOOL Tree8Node::CreateSon()
{
	// s 是子节点的范围的重叠系数；总是应该小于 0.5
	float s = 0.1f;

	//首先获得当前节点的数据的外包范围
	Envelope e;
	Tree8NodeObj *p = objs.GetData();
	int nObj = objs.GetSize(), i;
	for( i=0; i<nObj; i++,p++)
	{
		e.Union(&GetEnvelopeOfSearchableUnit(p->pu),2);
	}

	//几乎完全重叠，就不再切割了
	if( e.Width()<1e-8 && e.Height()<1e-8 )
	{
		return FALSE;
	}
	
	//得到中心位置
	double cx = (e.m_xl + e.m_xh)*0.5, cy = (e.m_yl + e.m_yh)*0.5, cz = (e.m_zl + e.m_zh)*0.5;

	//计算重叠度
	double dx = e.Width()*s, dy = e.Height()*s, dz = e.Depth()*s;

	//同高程面，就不对z方向作切分
	if( dz<1e-9 )dz = e.Depth();
	
	for( i=0; i<8; i++)
	{
		son[i] = new Tree8Node;
		if( !son[i] )return FALSE;
		
		son[i]->parent = this;
		
		switch(i) 
		{
		case 0:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = cx + dx;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = cy + dy;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = cz + dz;
			break;
		case 1:
			son[i]->e0.m_xl = cx - dx; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = cy + dy;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = cz + dz;
			break;
		case 2:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = cx + dx;
			son[i]->e0.m_yl = cy - dy; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = cz + dz;
			break;
		case 3:
			son[i]->e0.m_xl = cx - dx; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = cy - dy; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = e0.m_zl; son[i]->e0.m_zh = cz + dz;
			break;
		case 4:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = cx + dx;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = cy + dy;
			son[i]->e0.m_zl = cz - dz; son[i]->e0.m_zh = e0.m_zh;
			break;
		case 5:
			son[i]->e0.m_xl = cx - dx; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = e0.m_yl; son[i]->e0.m_yh = cy + dy;
			son[i]->e0.m_zl = cz - dz; son[i]->e0.m_zh = e0.m_zh;
			break;
		case 6:
			son[i]->e0.m_xl = e0.m_xl; son[i]->e0.m_xh = cx + dx;
			son[i]->e0.m_yl = cy - dy; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = cz - dz; son[i]->e0.m_zh = e0.m_zh;
			break;
		case 7:
			son[i]->e0.m_xl = cx - dx; son[i]->e0.m_xh = e0.m_xh;
			son[i]->e0.m_yl = cy - dy; son[i]->e0.m_yh = e0.m_yh;
			son[i]->e0.m_zl = cz - dz; son[i]->e0.m_zh = e0.m_zh;
			break;
		default:;
		}
	}	
	
	return TRUE;
}


Tree8NodeObj* Tree8Node::AddObj(SearchableUnit *u, LONG_PTR i)
{
	int index = objs.Add(Tree8NodeObj(u, i));
	vsum += RADIUS(GetEnvelopeOfSearchableUnit(u));

	return objs.GetData()+index;
}


void Tree8Node::DelObj(LONG_PTR i)
{
	Tree8NodeObj *buf = (Tree8NodeObj*)objs.GetData();
	int nobj = objs.GetSize();
	for( int j=nobj-1; j>=0; j--)
	{
		if( buf[j].grid==i )
		{
			objs.RemoveAt(j);
			buf = (Tree8NodeObj*)objs.GetData();
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTree8Search::CTree8Search():
m_arrPIndexes(1024)
{
	m_pHead = NULL;
	m_nHit = 0;
	m_nUnitSum = 0;
	m_nObjSum = 0;
	m_nMaxNumOfResult = 0;
	m_bIsExceedMaxNumOfResult = 0;
	m_bHitSurfaceInside = FALSE;

	InitRegion(NULL);
}

CTree8Search::~CTree8Search()
{
	DelAll();
}

void CTree8Search::InitRegion(Envelope *e)
{
	if( m_pHead )
		DelAll();

	if( m_pHead==NULL )
	{
		m_pHead = new Tree8Node;
		if( !m_pHead )return;
		
		if( e==NULL )m_pHead->e0.CreateMaxEnvelope();
		else m_pHead->e0 = *e;
	}
}

Tree8Node* CTree8Search::InsertNodeObj(SearchableUnit *pu, LONG_PTR grid, Tree8NodeObj *& pNodeObj)
{
	Envelope eu = GetEnvelopeOfSearchableUnit(pu), et;
	if( eu.IsEmpty(2) )
		return NULL;

	//1. 找最合适的插入位置
	int i = 0, m;

	Tree8Node *t = m_pHead;
	while( 1 )
	{
		while( t )
		{
			if( !t->son[0] )break;
			
			//找完全包含当前对象的那个子节点
			m = -1;
			for( i=0; i<8; i++)
			{
				if( t->son[i]->e0.bEnvelopeIn(&eu,3) )
				{
					t = t->son[i];
					break;
				}
			}

			//所有子节点都不能放，只有放在自己的数据表里
			if( i>=8 )
			{
				pNodeObj = t->AddObj(pu,grid);
				return t;
			}
		}

		//必然是叶子节点
		ASSERT(t->son[0]==NULL);
		
		//2. 是否需要增加子节点
		int nsz = t->objs.GetSize();

		// 1) 对象数目很少，不需要增加子节点
		if( nsz<Max_ObjofNode )break;

		double a1 = RADIUS(t->e0);
		// 2) 对象普遍比较庞大，增加了子节点反而不能起到优化的作用
		if( t->vsum/(a1*nsz)>0.25 )
		{
			break;
		}

		//增加子节点
		if( !SplitNode(t) )
		{
			break;
		}
	}

	pNodeObj = t->AddObj(pu,grid);
	return t;
}


BOOL CTree8Search::SplitNode(Tree8Node *t)
{
	//如果已经有子节点，不能拆分
	if( t->son[0] )return FALSE;

	//按照8叉树的规则创建子节点
	if( !t->CreateSon() )return FALSE;

	int nsz = t->objs.GetSize(), ncut = 0, idx;
	Tree8NodeObj *buf = (Tree8NodeObj*)t->objs.GetData();

	//将所有数据放入到8个子节点中
	Envelope eu, et;
	long lastid = -1;
	for( int j=nsz-1; j>=0; j--)
	{
		eu = GetEnvelopeOfSearchableUnit(buf[j].pu);
		for( int i=0; i<8; i++)
		{
			if( t->son[i]->e0.bEnvelopeIn(&eu,3) )
			{
				t->son[i]->objs.Add(buf[j]);

				//体积估计值
				t->son[i]->vsum += RADIUS(eu);

				TreeIndex *pIndex = buf[j].pIndex;

				for( int k=pIndex->links.GetSize()-1; k>=0; k--)
				{
					if( pIndex->links[k]==t )
					{
						pIndex->links.RemoveAt(k);
						break;
					}
				}

				pIndex->links.Add(t->son[i]);

				//清除ID值
				buf[j].grid = -1;

				//拆分的数据项计数
				ncut++;
				break;
			}
		}
	}

	//数据不合适放入到子节点，子节点可以清除掉了
	if( ncut==0 )
	{
		for( int i=0; i<8; i++)
		{
			delete t->son[i];
			t->son[i] = 0;
		}

		return FALSE;
	}

	//有了子节点，体积参数（仅用于估计节点的对象分布密度）清零
	t->vsum = 0;

	//数据已经全部放入到子节点，当前节点不再保留这些数据
	if( nsz==ncut )
	{
		t->objs.RemoveAll();
		return TRUE;
	}

	//看看哪些数据没有放入到子节点，将之作为当前节点的新数据
	CArray<Tree8NodeObj,Tree8NodeObj> objs;
	objs.SetSize(nsz-ncut);
	ncut = 0;
	for( j=nsz-1; j>=0; j--)
	{
		if( buf[j].grid!=-1 )
			objs.SetAt(ncut++,buf[j]);
	}

	t->objs.RemoveAll();
	t->objs.Copy(objs);
	return TRUE;
}


void CTree8Search::AddObj(LONG_PTR grid, SearchableUnit *pu)
{
	if( !m_pHead || !pu )return;

	//将单元放入到树中
	Tree8Node *ptr=NULL;
	Tree8NodeObj *pNodeObj = NULL;

	ptr = InsertNodeObj(pu,grid,pNodeObj);
	if( ptr==NULL )
		return;

    TreeIndex *pIndex = InsertIndex(grid,ptr);

	pNodeObj->pIndex = pIndex;

	m_nUnitSum++;	
	m_nObjSum++;
}


void CTree8Search::DelObj(LONG_PTR grid)
{
	int insert_pos = -1;

 	if (!FindIndex(grid,&insert_pos))
 	{
		return;
	}

	TreeIndex *pidx = m_arrPIndexes.GetAt(insert_pos);

	for( int i=0; i<pidx->links.GetSize(); i++)
	{
		pidx->links[i]->DelObj(grid);
	}

	delete pidx;

	m_arrPIndexes.RemoveAt(insert_pos);

	m_nObjSum--;
}

void CTree8Search::DelAll()
{
	Envelope e;
	e.CreateMaxEnvelope();

	BOOL bInited = FALSE;
	if( m_pHead )
	{
		e = m_pHead->e0;
		bInited = TRUE;
	}

	FreeTree(m_pHead);

	for( int i=0; i<m_arrPIndexes.GetSize(); i++)
	{
		delete m_arrPIndexes[i];
	}

	m_arrPIndexes.RemoveAll();
	m_foundIds.RemoveAll();
	m_foundNodeObjs.RemoveAll();
	m_foundDis.RemoveAll();
	m_pHead = NULL;
	m_nHit = 0;
	m_nObjSum = 0;
	m_nUnitSum = 0;	

	if( bInited )
	{
		InitRegion(&e);
	}
}

const LONG_PTR* CTree8Search::GetFoundIds(int& num)const
{
	num = m_foundIds.GetSize();
	return m_foundIds.GetData();
}


const Tree8NodeObj *CTree8Search::GetFoundNodeObj(int& num)const
{
	num = m_foundNodeObjs.GetSize();
	return m_foundNodeObjs.GetData();
}

template<class Type>
static bool BinarySearch(Type a[],const Type& x,int n,int &pos) 
{ 
	
	int left=0; 
	
	int right=n-1; 
	
	while(left<=right)
	{ 		
		int middle=(left+right)/2; 
		
		if (x==a[middle]) 
		{
			pos = middle;
			return true; 
		}
		
		if (x>a[middle]) left=middle+1; 
		
		else right=middle-1; 
		
	} 
	pos = left;
	return false;	
} 

extern double FindMinimumDistance(PT_3D sch_pt, Envelope sch_evlp, const SearchableUnit *pu, CCoordSys *pCS, PT_3D *pRet, double& real_mindis);

const Tree8Node *CTree8Search::FindLeafNodeByPt(PT_3D pt,CCoordSys *pCS)
{
	if (!m_pHead)
	{
		return NULL;
	}
	PT_3D pt0;
	pCS->ClientToGround(&pt,&pt0);
	//1. 找最合适的插入位置
	int i = 0, m;
	
	Tree8Node *t = m_pHead;
	
	while( t )
	{
		if( !t->son[0] )break;
		
		//找完全包含当前对象的那个子节点		
		for( i=0; i<8; i++)
		{
			if( t->son[i]->e0.bPtIn(&pt0) )
			{
				t = t->son[i];
				break;
			}
		}			
	}
	return t;

}

void CTree8Search::SortIdsNodeObj( PT_3D pt ,Envelope e,CCoordSys *pCS,bool isSort)
{
	if (m_foundNodeObjs.GetSize()<=0)
	{
		return;
	}
	if (!isSort)
	{
		for (int i=0;i<m_foundNodeObjs.GetSize();i++)
		{
			LONG_PTR id = m_foundNodeObjs[i].grid;

			int pos = -1;
			if (!BinarySearch(m_foundIds.GetData(),id,m_foundIds.GetSize(),pos))
			{
				m_foundIds.InsertAt(pos,id);
			}
		}
	}
	else
	{
		//先对m_foundNodeObjs进行排序
		int num = m_foundNodeObjs.GetSize();
		
        CArray<double,double> arrDis, arrDis_real, arrDis2;
		arrDis.SetSize(num);
		arrDis_real.SetSize(num);
		PT_3D ret;
		for(int i=0;i<num;i++)
		{
			double real_dis = 0;
			arrDis.SetAt(i,FindMinimumDistance(pt,e,m_foundNodeObjs[i].pu,pCS,&ret,real_dis));
			arrDis_real.SetAt(i,real_dis);
		}
		int left,right,middle;
		double temp, temp1;
		Tree8NodeObj temp0;
		for ( i = 1; i < num; i++)
		{			
			left=0; right = i-1; 
			temp = arrDis[i]; temp1 = arrDis_real[i];
			temp0 = m_foundNodeObjs[i];
			
			while ( left <= right ) 
			{                    
				
				 middle = ( left + right )/2;         
				
				if ( temp < arrDis[middle] )
					
					right = middle - 1;                 
				
				else 
					left = middle + 1;
				
			}
			
			for ( int k = i-1; k >= left; k-- )
			{
				arrDis[k+1] = arrDis[k];
				arrDis_real[k+1] = arrDis_real[k];
			}
			m_foundNodeObjs.RemoveAt(i);
			m_foundNodeObjs.InsertAt(left,temp0);			

// 			memmove(&m_foundNodeObjs[left+1],&m_foundNodeObjs[left],(i-left)*sizeof(Tree8NodeObj));
// 			m_foundNodeObjs[left] = temp0;
		    arrDis[left] = temp;
			arrDis_real[left] = temp1;
		}

		//然后对m_foundIds进行排序插入
		//循环从m_foundNodeObjs中取出节点对象ID，用二分法往arr0中插入，若成功，则将节点ID添加进arr1，arr1为所求结果。
		CArray<LONG_PTR, LONG_PTR> arr0, arr1;
		arr0.Add(m_foundNodeObjs[0].grid);
		arr1.Add(m_foundNodeObjs[0].grid);
		arrDis2.Add(arrDis_real[0]);
		int pos;
		for (i=1;i<num;i++)
		{
			if(!BinarySearch(arr0.GetData(),m_foundNodeObjs[i].grid,arr0.GetSize(),pos))
			{
				arr1.Add(m_foundNodeObjs[i].grid);
				arr0.InsertAt(pos,m_foundNodeObjs[i].grid);
				arrDis2.Add(arrDis_real[i]);
			}
		}
		m_foundIds.Append(arr1);
		m_foundDis.Append(arrDis2);

	}

}

const double* CTree8Search::GetFoundDis(int& num)const
{
	num = m_foundDis.GetSize();
	return m_foundDis.GetData();
}

int CTree8Search::FindObj(const SearchRange *sr)
{
	m_nHit = 0;

	m_bIsExceedMaxNumOfResult = 0;
	m_foundIds.RemoveAll();
	m_foundNodeObjs.RemoveAll();
	m_foundDis.RemoveAll();

	Envelope e;
	CCoordSys *p = NULL;
	BOOL bEntireInclude = FALSE;

	if( sr->type==EBSR_RECT || sr->type==EBSR_RECT_SIMPLE )
	{
		// find objects in the rect
		const SearchRect *srr = (const SearchRect*)sr;
		e = srr->evlp;
		p = srr->pCS;
		bEntireInclude = srr->bEntireInclude;

		FindObj(sr,e,p,m_pHead,bEntireInclude);
		PT_3D pt;
		pt.x = (e.m_xl+e.m_xh)/2;
		pt.y = (e.m_yl+e.m_yh)/2;
		pt.z = 0;

		SortIdsNodeObj(pt,e,sr->pCS,FALSE);
	}
	else if( sr->type==EBSR_RADIUS )
	{
		// find the nearest object(radius)
		const SearchNearest *srr = (const SearchNearest*)sr;
		e.CreateFromPtAndRadius(srr->pt,srr->r);
		p = srr->pCS;
		bEntireInclude = FALSE;
//		if( !p )return 0;
		
		FindObj(sr,e,p,m_pHead,bEntireInclude);
		SortIdsNodeObj(srr->pt,e,sr->pCS,TRUE);
		
	}
	else if( sr->type==EBSR_NEARESTOBJ )
	{
		// find the nearest object
		const SearchNearestObj *srr = (const SearchNearestObj*)sr;
//		e.CreateFromPtAndRadius(srr->pt,srr->r);
		p = srr->pCS;
		PT_3D pt = srr->pt;
		FindObj(srr,pt,p,m_pHead);
		SortIdsNodeObj(pt,Envelope(),sr->pCS,TRUE);
		
	}
	else if( sr->type==EBSR_INTERSECTOBJ )
	{
		// find the intersect object
		const SearchIntersectObj *srr = (const SearchIntersectObj*)sr;
//		e.CreateFromPtAndRadius(srr->pt,srr->r);
		p = srr->pCS;
		PT_3D *pts = srr->pts;
		int num = srr->num;
		FindObj(srr,pts,num,p,m_pHead);
		SortIdsNodeObj(PT_3D(),Envelope(),sr->pCS,FALSE);
	}
	else if (sr->type == EBSR_POLYGON)
	{
		const SearchPolygon *srr = (const SearchPolygon*)sr;
		//		e.CreateFromPtAndRadius(srr->pt,srr->r);
		p = srr->pCS;
		PT_3D *pts = srr->pts;
		int num = srr->num;
		FindObj(srr,pts,num,p,m_pHead);
		SortIdsNodeObj(PT_3D(),Envelope(),sr->pCS,FALSE);
	}
	

	int nret = m_foundIds.GetSize();

	return nret;
}



static Envelope GetEnvelopeOfGraph2(const Graph *gr, float fDrawCellAngle, float fDrawCellKX, float fDrawCellKY)
{
	if( gr==NULL )return Envelope();
	if( IsGrPoint(gr) )
	{
		const GrPoint *grpt = (const GrPoint*)gr;
		static GrPoint last_pt;
		static Envelope last_evlp;
		if( last_pt.cell==grpt->cell && last_pt.angle==grpt->angle && 
			last_pt.kx==grpt->kx && last_pt.ky==grpt->ky )
		{
			double dx = grpt->pt.x-last_pt.pt.x; 
			double dy = grpt->pt.y-last_pt.pt.y;
			Envelope e2 = last_evlp;
			e2.m_xl += dx; e2.m_xh += dx;
			e2.m_yl += dy; e2.m_yh += dy;
			return e2;
		}
		CCellDefLib *pLib = GetCellDefLib();
		PT_3D pt = grpt->pt;

		if( pLib!=NULL && grpt->cell!=0 )
		{
			CellDef item = pLib->GetCellDef(grpt->cell-1);
			if( item.m_pgr!=NULL )
			{
				Envelope e0 = item.m_evlp;
				double fabxl = e0.m_xl<0?-e0.m_xl:e0.m_xl, fabxh = e0.m_xh<0?-e0.m_xh:e0.m_xh;
				double fabyl = e0.m_yl<0?-e0.m_yl:e0.m_yl, fabyh = e0.m_yh<0?-e0.m_yh:e0.m_yh;
				double maxx = fabxl>fabxh?fabxl:fabxh, maxy = fabyl>fabyh?fabyl:fabyh;
				
				double kx = grpt->kx*fDrawCellKX, ky = grpt->ky*fDrawCellKY;
				double r = maxx*kx + maxy*ky;
				
				float xoff, yoff;
				item.m_pgr->GetOrigin(xoff,yoff);
				
				e0.m_xl = xoff - r + pt.x; e0.m_xh = xoff + r + pt.x;
				e0.m_yl = yoff - r + pt.y; e0.m_yh = yoff + r + pt.y;
				e0.m_zl = pt.z - 0.5; e0.m_zh = pt.z + 0.5;

				last_pt = *grpt;
				last_evlp = e0;
				return e0;
			}
		}
		
		float sx = 0.5*grpt->kx*fDrawCellKX;
		float sy = 0.5*grpt->ky*fDrawCellKY;

		if( sx<0 )sx = -sx;
		if( sy<0 )sy = -sy;

		Envelope e;
		e.m_xl = pt.x-sx; e.m_xh = pt.x+sx;
		e.m_yl = pt.y-sy; e.m_yh = pt.y+sy;
		e.m_zl = pt.z-sx; e.m_zh = pt.z+sx;
		return e;
	}
	if( IsGrPointString(gr)||IsGrPointStringEx(gr) )
	{
		const GrPointString *grpts = (const GrPointString*)gr;
		GrPoint pt = grpts->GetPoint(-1);
		
		Envelope e1 = GetEnvelopeOfGraph2(&pt,fDrawCellAngle,fDrawCellKX, fDrawCellKY);
		
		Envelope e = ((GrPointString*)gr)->evlp;
		e.m_xl = e.m_xl+e1.m_xl; e.m_xh = e.m_xh+e1.m_xh;
		e.m_yl = e.m_yl+e1.m_yl; e.m_yh = e.m_yh+e1.m_yh;
		
		return e;
	}
	if( IsGrLineString(gr) )
	{
		return ((GrLineString*)gr)->evlp;
	}
	if( IsGrPolygon(gr) )
	{
		return ((GrPolygon*)gr)->evlp;
	}
	if( IsGrText(gr) )
	{
		GrText *grtxt = (GrText*)gr;
		PT_3D pt[4];
		pt[0].x = grtxt->x[0]+grtxt->pt.x;
		pt[0].y = grtxt->y[0]+grtxt->pt.y;
		pt[1].x = grtxt->x[1]+grtxt->pt.x;
		pt[1].y = grtxt->y[1]+grtxt->pt.y;
		pt[2].x = grtxt->x[2]+grtxt->pt.x;
		pt[2].y = grtxt->y[2]+grtxt->pt.y;
		pt[3].x = grtxt->x[3]+grtxt->pt.x;
		pt[3].y = grtxt->y[3]+grtxt->pt.y;
		pt[0].z = pt[1].z=pt[2].z=pt[3].z = grtxt->pt.z;
		Envelope e;
		e.CreateFromPts(pt,4,sizeof(PT_3D),3);
		e.m_zl = grtxt->zmin;
		e.m_zh = grtxt->zmax;

		return e;
	}

	return Envelope();
}



static int ExtractSonPolgon(const GrVertex *pts, int npt, CArray<int,int>& arrPtCounts)
{
	int i0 = 0;
	for(int i=0; i<npt; i++)
	{
		if(i!=0 && (pts[i].code&GRBUFFER_PTCODE_LINETO)==0 )
		{
			arrPtCounts.Add(i-i0);
			i0 = i;
		}
	}

	arrPtCounts.Add(i-i0);
	return arrPtCounts.GetSize();
}



BOOL GrBufferIsIntersect(Envelope e, const GrBuffer *pBuf, BOOL bHitSurfaceInside)
{
	BOOL bAdd = FALSE;
	const Graph *pgr = pBuf->HeadGraph();				
	
	Envelope e1;
	GrVertex ptret1, ptret2;
	for( ; pgr!=NULL; pgr = pgr->next)
	{
		e1 = GetEnvelopeOfGraph2(pgr,0,1,1);
		if( !e1.bIntersect(&e) )
			continue;

		const GrVertex *pts;
		if( IsGrPoint(pgr) )
		{
			GrPoint *pgr0 = (GrPoint*)(pgr);
			if( pgr0->cell==0 )
			{
				if(e.bPtIn(&(pgr0->pt),2))
				{
					bAdd = TRUE;
					break;
				}
			}		
			else if( e1.Width()<e.Width()*0.5 && e1.Height()<e.Height()*0.5 )
			{
				bAdd = TRUE;
				break;
			}
			else
			{
				CCellDefLib *pCellLib = GetCellDefLib();
				CellDef cell;

				cell = pCellLib->GetCellDef(pgr0->cell-1);

				GrBuffer buf;
				buf.AddBuffer(cell.m_pgr,0);
				buf.Zoom(pgr0->kx,pgr0->ky);
				buf.Rotate(pgr0->angle);
				buf.Move(pgr0->pt.x,pgr0->pt.y);

				bAdd = GrBufferIsIntersect(e,&buf,bHitSurfaceInside);
				if( bAdd )
				{
					break;
				}
			}
		}
		else if( IsGrPointString(pgr)||IsGrPointStringEx(pgr) )
		{
			GrPointString *pgr0 = (GrPointString*)(pgr);	
			pts = pgr0->ptlist.pts;

			if( pgr0->cell==0 )
			{
				for( int k=0; k<pgr0->ptlist.nuse;k++ )
				{
					if(e.bPtIn(&pts[k],2))
					{
						bAdd = TRUE;
						break;
					}
				}
			}
			else
			{
				GrPoint grpt;
				for( int k=0; k<pgr0->ptlist.nuse;k++ )
				{
					grpt = pgr0->GetPoint(k);
					e1 = GetEnvelopeOfGraph2(&grpt,0,1,1);
					if( e1.bIntersect(&e) )
					{
						if( e1.Width()<e.Width()*0.5 && e1.Height()<e.Height()*0.5 )
						{
							bAdd = TRUE;
							break;
						}
						else
						{
							CCellDefLib *pCellLib = GetCellDefLib();
							CellDef cell;
							
							cell = pCellLib->GetCellDef(grpt.cell-1);
							
							GrBuffer buf;
							buf.AddBuffer(cell.m_pgr,0);
							buf.Zoom(grpt.kx,grpt.ky);
							buf.Rotate(grpt.angle);
							buf.Move(grpt.pt.x,grpt.pt.y);
							
							bAdd = GrBufferIsIntersect(e,&buf,bHitSurfaceInside);
							if( bAdd )
							{
								break;
							}
						}
					}
				}
			}		
		}
		else if( IsGrLineString(pgr))
		{
			GrLineString *pgr0 = (GrLineString*)(pgr);	
			pts = pgr0->ptlist.pts;
			
			for( int k=0; k<pgr0->ptlist.nuse-1;k++ )
			{
				if(IsGrPtCodeMoveTo(&pts[k+1]))
					continue;

				//			pCS->GroundToClient(&pts[0],&pt1);
				//			pCS->GroundToClient(&pts[1],&pt2);
				
				if( min(pts[k].x,pts[k+1].x)>e.m_xh || max(pts[k].x,pts[k+1].x)<e.m_xl || 
					min(pts[k].y,pts[k+1].y)>e.m_yh || max(pts[k].y,pts[k+1].y)<e.m_yl )
					;
				else 
					if( e.bIntersect(&pts[k],&pts[k+1],2) )break;
			}
			if( k<pgr0->ptlist.nuse-1 || (pgr0->ptlist.nuse==1 && e.bPtIn(&pts[0],2)) )
			{
				bAdd = TRUE;
				break;
			}		
		}
		else if(IsGrPolygon(pgr) && !bHitSurfaceInside)
		{
			GrPolygon *pgr0 = (GrPolygon*)(pgr);	
			pts = pgr0->ptlist.pts;
			
			for( int k=0; k<pgr0->ptlist.nuse-1;k++ )
			{
				if(IsGrPtCodeMoveTo(&pts[k+1]))
					continue;

				//			pCS->GroundToClient(&pts[0],&pt1);
				//			pCS->GroundToClient(&pts[1],&pt2);
				
				if( min(pts[k].x,pts[k+1].x)>e.m_xh || max(pts[k].x,pts[k+1].x)<e.m_xl || 
					min(pts[k].y,pts[k+1].y)>e.m_yh || max(pts[k].y,pts[k+1].y)<e.m_yl )
					;
				else 
					if( e.bIntersect(&pts[k],&pts[k+1],2) )break;
			}
			if( k<pgr0->ptlist.nuse-1 || (pgr0->ptlist.nuse==1 && e.bPtIn(&pts[0],2)) )
			{
				bAdd = TRUE;
				break;
			}			
		}
		else if (IsGrPolygon(pgr) && bHitSurfaceInside )
		{
			GrPolygon *pgr0 = (GrPolygon*)(pgr);	
			GrVertex *pts0 = pgr0->ptlist.pts;
			CArray<int,int> arrPtCounts;
			ExtractSonPolgon(pts0,pgr0->ptlist.nuse,arrPtCounts);

			int pos = 0, nPtInRegion_Count = 0;
			for(int m=0; m<arrPtCounts.GetSize(); m++)
			{	
				int npt = arrPtCounts[m];
				pts = pts0 + pos;
				pos += npt;
				if( npt<2 )
					continue;

				for( int k=0; k<npt-1; k++ )
				{
					if( min(pts[k].x,pts[k+1].x)>e.m_xh || max(pts[k].x,pts[k+1].x)<e.m_xl || 
						min(pts[k].y,pts[k+1].y)>e.m_yh || max(pts[k].y,pts[k+1].y)<e.m_yl )
						;
					else 
						if( e.bIntersect(&pts[k],&pts[k+1],2) )break;
				}
				if( k<npt-1 || (npt==1 && e.bPtIn(&pts[0],2))||e.bIntersect(&pts[0],&pts[npt-1],2) )
				{
					bAdd = TRUE;
					break;
				}
				
				if(!bAdd )
				{
					if (e.bPtIn(&pts[0],2))
					{
						bAdd = TRUE;
						break;
					}
					GrVertex  pts2[4];
					pts2[0].x = e.m_xl;	pts2[0].y = e.m_yl;
					pts2[1].x = e.m_xh;	pts2[1].y = e.m_yl;
					pts2[2].x = e.m_xh;	pts2[2].y = e.m_yh;
					pts2[3].x = e.m_xl;	pts2[3].y = e.m_yh;

					for( k=0; k<4; k++)
					{
						if (GraphAPI::GIsPtInRegion(pts2[k],const_cast<GrVertex *>(pts),npt)==2)
						{
							nPtInRegion_Count++;
							break;
						}
					}
				}

				if( bAdd )
					break;				
			}

			if( bAdd )
				break;
			else if(nPtInRegion_Count>0 && (nPtInRegion_Count%2)!=0)
			{
				bAdd = TRUE;
				break;
			}
		}
		else if( IsGrText(pgr) )
		{
			GrText *pgr0 = (GrText*)(pgr);

			GrVertex  pts1[4];
			for( int k=0; k<4; k++)
			{
				pts1[k].x = pgr0->x[k] + pgr0->pt.x; pts1[k].y = pgr0->y[k] + pgr0->pt.y;
			}

			if (e.bPtIn(&pts1[0],2))
			{
				bAdd = TRUE;
				break;
			}

			GrVertex  pts2[4];
			pts2[0].x = e.m_xl;	pts2[0].y = e.m_yl;
			pts2[1].x = e.m_xh;	pts2[1].y = e.m_yl;
			pts2[2].x = e.m_xh;	pts2[2].y = e.m_yh;
			pts2[3].x = e.m_xl;	pts2[3].y = e.m_yh;

			for( k=0; k<4; k++)
			{
				if (GraphAPI::GIsPtInRegion(pts2[k],pts1,4)==2)
				{
					bAdd = TRUE;
					break;
				}
			}

			if( bAdd )
				break;
		}
		
	}

	return bAdd;
}

BOOL CTree8Search::Intersect(LONG_PTR id, Envelope e, CCoordSys *pCS)
{
	if( GIsIntersectEnvelope(&((CFeature*)id)->GetGeometry()->GetEnvelope(),&e,pCS) )
		return TRUE;

	int insert_pos = -1;

	if (!FindIndex(id,&insert_pos))
	{
		return FALSE;
	}

	TreeIndex *pidx = m_arrPIndexes[insert_pos];
	int nsz = pidx->links.GetSize();

	for( int i = 0; i<nsz; i++ )
	{
		Tree8Node *pNode = pidx->links[i];
		if( !GIsIntersectEnvelope(&pNode->e0,&e,pCS) )
			continue;

		Tree8NodeObj *buf = (Tree8NodeObj*)pNode->objs.GetData();
		int nobj = pNode->objs.GetSize();
		for( int j=nobj-1; j>=0; j--)
		{
			if( buf[j].grid==id )
			{
				if( GIsIntersectEnvelope(&GetEnvelopeOfSearchableUnit(buf[j].pu),&e,pCS) )
					return TRUE;
			}
		}
	}
	return FALSE;
}

void CTree8Search::FindObj(const SearchRange *sr, Envelope e, CCoordSys *pCS, Tree8Node *t, BOOL bEntireInclude)
{
	if( !t )return;
	if( !GIsIntersectEnvelope(&t->e0,&e,pCS) )
		return;

	BOOL bSimple = (sr->type==EBSR_RECT_SIMPLE);

	int nsz = t->objs.GetSize();
	Tree8NodeObj *buf = (Tree8NodeObj*)t->objs.GetData();
	SearchableUnit *pu = NULL;	
	PT_3D pt1,pt2;

	BOOL bAdd;

	CArray<double,double> arrDis;

	if( pCS==NULL || (pCS->IsNoRotated()&&pCS->GetCoordSysType()==COORDSYS_44MATRIX) )
	{
// 		double rm[16];
// 		pCS->GetRMatrix(rm);
		Envelope e0 = e;
		e0.TransformCltToGrd(pCS);

		for( int i=0; i<nsz; i++)
		{
			pu = buf[i].pu;
			if( m_pFilterFunc!=NULL && !(m_pFilter->*m_pFilterFunc)(buf[i].grid) )
				continue;

			CFeature* pFtr = (CFeature*)buf[i].grid;

			Envelope e1 = pFtr->GetGeometry()->GetEnvelope();
			if (bEntireInclude&&!e0.bEnvelopeIn(&e1))
			{
				continue;
			}
			m_nHit++;

			e1 = GetEnvelopeOfSearchableUnit(pu);

			if(!e0.bIntersect(&e1))
				continue;
			if(bEntireInclude&&!e0.bEnvelopeIn(&e1))
				continue;

// 			if( !GIsIntersectEnvelope(&GetEnvelopeOfSearchableUnit(pu),&e,pCS) )
// 				continue;
			
			bAdd = FALSE;
			if (bEntireInclude)
			{
				bAdd = TRUE;
			}
			else if( bSimple )
			{
				bAdd = TRUE;
			}
			else
			{
				if( IsSearchablePoint(pu) )
				{
					bAdd = TRUE;
				}
				else if( IsSearchableLine(pu) )
				{				
					const CShapeLine::ShapeLineUnit *pLine = (const CShapeLine::ShapeLineUnit*)(((const SearchableLine*)pu)->p);
					const PT_3DEX *pts = pLine->pts;
					
					for( int k=0; k<pLine->nuse-1; k++,pts++)
					{
						//			pCS->GroundToClient(&pts[0],&pt1);
						//			pCS->GroundToClient(&pts[1],&pt2);
						
						if( min(pts[0].x,pts[1].x)>e0.m_xh || max(pts[0].x,pts[1].x)<e0.m_xl || 
							min(pts[0].y,pts[1].y)>e0.m_yh || max(pts[0].y,pts[1].y)<e0.m_yl )
							;
						else 
							if( e0.bIntersect(&pts[0],&pts[1],2) )break;
					}
					
					//	pCS->GroundToClient(&pts[0],&pt1);
					if( k<pLine->nuse-1 || (pLine->nuse==1 && e0.bPtIn(&pts[0],2)) )
					{
						bAdd = TRUE;
					}
				}
				else if( IsSearchableObj(pu) )
				{
					bAdd = ((SearchableObj*)pu)->p->FindObj(sr,NULL);
				}
				else if ( IsSearchableGrBuf(pu))
				{
					GrBuffer *grbuf = (GrBuffer *)(((SearchableGrBuf*)pu)->p);
					bAdd = GrBufferIsIntersect(e0,grbuf,m_bHitSurfaceInside);
				}
			
			}
		
			if( bAdd )
			{
				if( m_pFilterFunc1!=NULL && !(m_pFilter1->*m_pFilterFunc1)(buf[i].grid) )
					continue;

				m_foundNodeObjs.Add(buf[i]);		
				if( (m_nMaxNumOfResult!=0 && m_foundNodeObjs.GetSize()>=m_nMaxNumOfResult) )
				{
					m_bIsExceedMaxNumOfResult = 1;
					break;	
				}
			}
		}
	}
	else
	{
		for( int i=0; i<nsz; i++)
		{
			pu = buf[i].pu;
			if( m_pFilterFunc!=NULL && !(m_pFilter->*m_pFilterFunc)(buf[i].grid) )
				continue;

			CFeature* pFtr = (CFeature*)buf[i].grid;
			Envelope e1 = pFtr->GetGeometry()->GetEnvelope();
			e1.TransformGrdToClt(pCS,1);
			if (bEntireInclude&&!e.bEnvelopeIn(&e1))
			{
				continue;
			}
			m_nHit++;
			e1 = GetEnvelopeOfSearchableUnit(pu);
			e1.TransformGrdToClt(pCS,1);
			if(bEntireInclude&&!e.bEnvelopeIn(&e1))
				continue;

			if( !e.bIntersect(&e1) )
				continue;
			
			bAdd = FALSE;
			if (bEntireInclude)
			{
				bAdd = TRUE;
			}
			else if( bSimple )
			{
				bAdd = TRUE;
			}
			else 
			{
				if( IsSearchablePoint(pu) )
				{
					bAdd = TRUE;
				}
				else if( IsSearchableLine(pu) )
				{
					const CShapeLine::ShapeLineUnit *pLine = (const CShapeLine::ShapeLineUnit*)(((const SearchableLine*)pu)->p);
					const PT_3DEX *pts = pLine->pts;
					
					for( int k=0; k<pLine->nuse-1; k++,pts++)
					{
						pCS->GroundToClient(&pts[0],&pt1);
						pCS->GroundToClient(&pts[1],&pt2);
						if( e.bIntersect(&pt1,&pt2,2) )break;
					}
					
					pCS->GroundToClient(&pts[0],&pt1);
					if( k<pLine->nuse-1 || (pLine->nuse==1 && e.bPtIn(&pt1,2)) )
					{
						bAdd = TRUE;
					}
				}
				else if( IsSearchableObj(pu) )
				{
					bAdd = ((SearchableObj*)pu)->p->FindObj(sr,NULL);
				}
				else if ( IsSearchableGrBuf(pu))
				{
					GrBuffer *grbuf = (GrBuffer *)(((SearchableGrBuf*)pu)->p);
					
					const Graph *pgr = grbuf->HeadGraph();				
					
					GrVertex ptret1, ptret2;
					while( pgr )
					{
						const GrVertex *pts;
						if( IsGrPoint(pgr) )
						{
							GrPoint *pgr0 = (GrPoint*)(pgr);
							pCS->GroundToClient(&(pgr0->pt),&pt1);
							if(e.bPtIn(&pt1,2))
							{
								bAdd = TRUE;
								break;
							}							
						}
						else if( IsGrPointString(pgr)||IsGrPointStringEx(pgr) )
						{
							GrPointString *pgr0 = (GrPointString*)(pgr);	
							pts = pgr0->ptlist.pts;
							
							for( int k=0; k<pgr0->ptlist.nuse;k++ )
							{
								if(e.bPtIn(&pts[k],2))
								{
									bAdd = TRUE;
									break;
								}
							}	
						}
						else if( IsGrLineString(pgr) )
						{
							GrLineString *pgr0 = (GrLineString*)(pgr);	
							pts = pgr0->ptlist.pts;
							
							for( int k=0; k<pgr0->ptlist.nuse-1;k++ )
							{
								if(IsGrPtCodeMoveTo(&pts[k+1]))
									continue;

								pCS->GroundToClient(&pts[k],&pt1);
								pCS->GroundToClient(&pts[k+1],&pt2);
								
								if( min(pt1.x,pt2.x)>e.m_xh || max(pt1.x,pt2.x)<e.m_xl || 
									min(pt1.y,pt2.y)>e.m_yh || max(pt1.y,pt2.y)<e.m_yl )
									;
								else 
									if( e.bIntersect(&pt1,&pt2,2) )break;
							}
							pCS->GroundToClient(&pts[0],&pt1);
							if( k<pgr0->ptlist.nuse-1 || (pgr0->ptlist.nuse==1 && e.bPtIn(&pt1,2)) )
							{
								bAdd = TRUE;
								break;
							}
						}
						else if( IsGrPolygon(pgr) )
						{
							GrPolygon *pgr0 = (GrPolygon*)(pgr);	
							pts = pgr0->ptlist.pts;
							
							for( int k=0; k<pgr0->ptlist.nuse-1;k++ )
							{
								if(IsGrPtCodeMoveTo(&pts[k+1]))
									continue;

								pCS->GroundToClient(&pts[k],&pt1);
								pCS->GroundToClient(&pts[k+1],&pt2);
								
								if( min(pt1.x,pt2.x)>e.m_xh || max(pt1.x,pt2.x)<e.m_xl || 
									min(pt1.y,pt2.y)>e.m_yh || max(pt1.y,pt2.y)<e.m_yl )
									;
								else 
									if( e.bIntersect(&pt1,&pt2,2) )break;
							}
							pCS->GroundToClient(&pts[0],&pt1);
							if( k<pgr0->ptlist.nuse-1 || (pgr0->ptlist.nuse==1 && e.bPtIn(&pt1,2)) )
							{
								bAdd = TRUE;
								break;
							}
						}						
						else if( IsGrText(pgr) )
						{
							GrText *pgr0 = (GrText*)(pgr);
							Envelope e1 = GetEnvelopeOfGraph2(pgr0,0,1,1);
							e1.TransformGrdToClt(pCS,1);
							if( e1.bIntersect(&e) )
							{
								bAdd = TRUE;
								break;
							}
						}
						
						pgr = pgr->next;
					}	
				}
			}
			if( bAdd )
			{
				if( m_pFilterFunc1!=NULL && !(m_pFilter1->*m_pFilterFunc1)(buf[i].grid) )
					continue;

				m_foundNodeObjs.Add(buf[i]);		
				if( (m_nMaxNumOfResult!=0 && m_foundNodeObjs.GetSize()>=m_nMaxNumOfResult) )
				{
					m_bIsExceedMaxNumOfResult = 1;
					break;				
				}
			}
		}
	}
	if( (m_nMaxNumOfResult!=0 && m_foundNodeObjs.GetSize()>=m_nMaxNumOfResult) )
	{
		m_bIsExceedMaxNumOfResult = 1;
		return;
	}

	if( t->son[0] )
	{
		for(int i=0; i<8; i++)
		{
			FindObj(sr,e,pCS,t->son[i],bEntireInclude);
		}
		
		return;
	}

	return;
}


void FindLeafNodeObj(CArray<Tree8NodeObj,Tree8NodeObj> &arrNodeObj,const SearchRange *sr, PT_3D pt, CCoordSys *pCS, Tree8Node *t)
{
	if( !t )return;
	Envelope e;
	if ( !GIsInEnvelope(&t->e0, &pt, pCS) ) return;
		
	int nsz = t->objs.GetSize();
	Tree8NodeObj *buf = (Tree8NodeObj*)t->objs.GetData();
	SearchableUnit *pu = NULL;

	for( int i=0; i<nsz; i++)
	{
		pu = buf[i].pu;
		if( !GIsInEnvelope(&GetEnvelopeOfSearchableUnit(pu),&pt,pCS) )
			continue;

		if( t->son[0] )
		{
			for(int j=0; j<8; j++)
			{
				FindLeafNodeObj(arrNodeObj,sr,pt,pCS,t->son[j]);
			}
		}
		else
		{
			arrNodeObj.Add(buf[i]);
		}
	}
		
}

void CTree8Search::FindObj(const SearchRange *sr, PT_3D pt, CCoordSys *pCS, Tree8Node *t)
{
	CArray<Tree8NodeObj,Tree8NodeObj> arrNodeObj;
	FindLeafNodeObj(arrNodeObj,sr,pt,pCS,t);

	CMap<Tree8NodeObj*,Tree8NodeObj*,double,double> map;

	PT_3D pt1,pt2;
	SearchableUnit *pu = NULL;
	for (int i=0; i<arrNodeObj.GetSize(); i++)
	{
		pu = arrNodeObj.GetAt(i).pu;
		
		if( IsSearchablePoint(pu) )
		{
			CGeoPoint *geoPt =(CGeoPoint*)((SearchablePoint*)pu)->p;
			PT_2D pt1 = geoPt->GetDataPoint(0);
			double dis = GraphAPI::GGet2DDisOf2P(PT_2D(pt.x,pt.y),pt1);
			map.SetAt(&arrNodeObj.GetAt(i),dis);	
			
		}
		else if( IsSearchableLine(pu) )
		{
			const CShapeLine::ShapeLineUnit *pLine = (const CShapeLine::ShapeLineUnit*)(((const SearchableLine*)pu)->p);
			const PT_3DEX *pts = pLine->pts;
			
			for( int k=0; k<pLine->nuse-1; k++,pts++)
			{
				pCS->GroundToClient(&pts[0],&pt1);
				pCS->GroundToClient(&pts[1],&pt2);
				
				double dis = GraphAPI::GGetNearestDisOfPtToLine(pt1.x,pt1.y,pt2.x,pt2.y,pt.x,pt.y,NULL,NULL,FALSE);
				map.SetAt(&arrNodeObj.GetAt(i),dis);	
			}
			
			pCS->GroundToClient(&pts[0],&pt1);
			if(pLine->nuse ==1 )
			{
				double dis = GraphAPI::GGet2DDisOf2P(PT_2D(pt.x,pt.y),PT_2D(pt1.x,pt1.y));
				map.SetAt(&arrNodeObj.GetAt(i),dis);	
			}
		}
		else if( IsSearchableObj(pu) )
		{
//			bAdd = ((SearchableObj*)pu)->FindObj(sr,NULL);
		}
		else if ( IsSearchableGrBuf(pu))
		{

		}		
		
	}	
	

	int index = 0;
	POSITION pos = map.GetStartPosition();
    Tree8NodeObj    *NodObj;
    double mDis;
	map.GetNextAssoc( pos, NodObj, mDis );
	
    while (pos != NULL)
    {
		Tree8NodeObj    *nKey;
		double dis;
		map.GetNextAssoc( pos, nKey, dis );
		
		if (dis < mDis)
		{
			NodObj = nKey;
		}
    }
   
	if (NodObj && NodObj->pu)
	{
		m_foundNodeObjs.Add(*NodObj);	
	}
	
	
}


//for EBSR_INTERSECTOBJ or EBSR_POLYGON
void CTree8Search::FindObj(const SearchRange *sr, PT_3D *pts, int num, CCoordSys *pCS, Tree8Node *t, int size)
{
	if(!t || num<2)return;

	Envelope e;
	e.CreateFromPts(pts, num);

	if( sr->type==EBSR_INTERSECTOBJ )
	{
		SearchIntersectObj *s = (SearchIntersectObj*)sr;
		if( !s->evlp.IsEmpty() )
			e.Intersect(&s->evlp);
	}
	
	if( !GIsIntersectEnvelope(&t->e0,&e,pCS) )
		return;
	
	int nsz = t->objs.GetSize();
	Tree8NodeObj *buf = (Tree8NodeObj*)t->objs.GetData();
	SearchableUnit *pu = NULL;	
	PT_3D pt0,pt1,pt2,pt3;
	
	BOOL bAdd;

	int nSearchType = sr->type;
	
	CArray<double,double> arrDis;
	
	for( int i=0; i<nsz; i++)
	{
		pu = buf[i].pu;
		if( m_pFilterFunc!=NULL && !(m_pFilter->*m_pFilterFunc)(buf[i].grid) )
				continue;
		m_nHit++;
		if( !GIsIntersectEnvelope(&GetEnvelopeOfSearchableUnit(pu),&e,pCS) )
			continue;
		
		bAdd = FALSE;
		
		if( IsSearchablePoint(pu) )
		{
			CGeoPoint *geoPt =(CGeoPoint*)((SearchablePoint*)pu)->p;
			pt0 = geoPt->GetDataPoint(0);
			pCS->GroundToClient(&pt0,&pt1);
			int r = GraphAPI::GIsPtInRegion(pt1,pts,num);

			if (nSearchType == EBSR_INTERSECTOBJ)
			{
				if (r == 0 || r == 1)
				{
					bAdd = TRUE;
				}
			}
			else if (nSearchType == EBSR_POLYGON)
			{
				if (r >= 0)
				{
					bAdd = TRUE;
				}
			}			
			
		}
		else if( IsSearchableLine(pu) )
		{
			const CShapeLine::ShapeLineUnit *pLine = (const CShapeLine::ShapeLineUnit*)(((const SearchableLine*)pu)->p);
			const PT_3DEX *shapepts = pLine->pts;
			
			PT_3D ret;

			if (nSearchType == EBSR_INTERSECTOBJ)
			{	
				for (int m=1; m<num; m++)
				{
					pt0 = pts[m-1];
					pt1 = pts[m];
					
					for (int j=1; j<pLine->nuse; j++)
					{
						pCS->GroundToClient(&shapepts[j-1],&pt2);
						pCS->GroundToClient(&shapepts[j],&pt3);
						
						if ( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,&ret.x,&ret.y,NULL) )
						{
							bAdd = TRUE;
							break;
						}
						
					}

					if (bAdd)
					{						
						break;
					}
				}
			}
			else if (nSearchType == EBSR_POLYGON)
			{	
				
				for(int j=0; j<pLine->nuse; j++)
				{
					pt0 = shapepts[j];
					pCS->GroundToClient(&pt0,&pt1);

					int r = GraphAPI::GIsPtInRegion(pt1,pts,num);
					if( r>=0 )
					{
						bAdd = TRUE;
						break;
					}
				}

				// 点不在区域内,看线段是否相交
				if (!bAdd)
				{
					for (int m=1; m<num; m++)
					{
						pt0 = pts[m-1];
						pt1 = pts[m];
						
						for (int j=1; j<pLine->nuse; j++)
						{
							pCS->GroundToClient(&shapepts[j-1],&pt2);
							pCS->GroundToClient(&shapepts[j],&pt3);
							
							if ( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,&ret.x,&ret.y,NULL) )
							{
								bAdd = TRUE;
								break;
							}
							
						}
						
						if (bAdd)
						{						
							break;
						}
					}
				}
				
			}	
			
		}
		else if( IsSearchableObj(pu) )
		{
			bAdd = ((SearchableObj*)pu)->p->FindObj(sr,NULL);
		}
		else if ( IsSearchableGrBuf(pu))
		{
			GrBuffer *grbuf = (GrBuffer *)(((SearchableGrBuf*)pu)->p);
			
			const Graph *pgr = grbuf->HeadGraph();				
			
			GrVertex ptret1, ptret2;
			while( pgr )
			{
				const GrVertex *ptsV;
				if( IsGrPoint(pgr) )
				{
					GrPoint *pgr0 = (GrPoint*)(pgr);
//  					if(pCS)pCS->GroundToClient(&(pgr0->pt),&pt1);
//  					else COPY_3DPT(pt1,(pgr0->pt));
// 
// 					if(e.bPtIn(&pt1,2))
// 					{
// 						bAdd = TRUE;
// 						break;
// 					}	
					pCS->GroundToClient(&pgr0->pt,&pt1);

					int r = GraphAPI::GIsPtInRegion(pt1,pts,num);
					
					if (nSearchType == EBSR_INTERSECTOBJ)
					{
						if (r == 0 || r == 1)
						{
							bAdd = TRUE;
						}
					}
					else if (nSearchType == EBSR_POLYGON)
					{
						if (r >= 0)
						{
							bAdd = TRUE;
						}
					}

					if (bAdd)
					{
						break;
					}												
				}
				else if( IsGrPointString(pgr)||IsGrPointStringEx(pgr) )
				{
					GrPointString *pgr0 = (GrPointString*)(pgr);	
					ptsV = pgr0->ptlist.pts;
					
					for( int k=0; k<pgr0->ptlist.nuse;k++ )
					{
						pCS->GroundToClient(&ptsV[k],&pt1);

						int r = GraphAPI::GIsPtInRegion(pt1,pts,num);
						
						if (nSearchType == EBSR_INTERSECTOBJ)
						{
							if (r == 0 || r == 1)
							{
								bAdd = TRUE;
							}
						}
						else if (nSearchType == EBSR_POLYGON)
						{
							if (r >= 0)
							{
								bAdd = TRUE;
							}
						}
						
						if (bAdd)
						{
							break;
						}
					}	
				}
				else if( IsGrLineString(pgr)||IsGrPolygon(pgr) )
				{
					GrLineString *pgr0 = (GrLineString*)(pgr);	
					ptsV = pgr0->ptlist.pts;
					
					PT_3D pt0, pt1, pt2, pt3, ret;
					
					if (nSearchType == EBSR_INTERSECTOBJ)
					{	
						for (int m=1; m<num; m++)
						{
							pt0 = pts[m-1];
							pt1 = pts[m];
							
							for (int j=1; j<pgr0->ptlist.nuse; j++)
							{
								pCS->GroundToClient(&ptsV[j-1],&pt2);
								pCS->GroundToClient(&ptsV[j],&pt3);
								
								if ( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,&ret.x,&ret.y,NULL) )
								{
									bAdd = TRUE;
									break;
								}
								
							}
							
							if (bAdd)
							{						
								break;
							}
						}
					}
					else if (nSearchType == EBSR_POLYGON)
					{	
						
						for(int j=0; j<pgr0->ptlist.nuse; j++)
						{
							pCS->GroundToClient(&ptsV[j],&pt1);
							int r = GraphAPI::GIsPtInRegion(pt1,pts,num);
							if( r>=0 )
							{
								bAdd = TRUE;
								break;
							}
						}
						
						// 点不在区域内,看线段是否相交
						if (!bAdd)
						{
							for (int m=1; m<num; m++)
							{
								pt0 = pts[m-1];
								pt1 = pts[m];
								
								for (int j=1; j<pgr0->ptlist.nuse; j++)
								{
									pCS->GroundToClient(&ptsV[j-1],&pt2);
									pCS->GroundToClient(&ptsV[j],&pt3);
									
									if ( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,&ret.x,&ret.y,NULL) )
									{
										bAdd = TRUE;
										break;
									}
									
								}
								
								if (bAdd)
								{						
									break;
								}
							}
						}
						
					}
					/*for( int k=0; k<pgr0->ptlist.nuse-1;k++ )
					{
						if (pCS)
						{
							pCS->GroundToClient(&pts[k],&pt1);
							pCS->GroundToClient(&pts[k+1],&pt2);
						}
						else
						{
							COPY_3DPT(pt1,pts[k]);
							COPY_3DPT(pt2,pts[k+1]);
						}
						if( min(pt1.x,pt2.x)>e.m_xh || max(pt1.x,pt2.x)<e.m_xl || 
							min(pt1.y,pt2.y)>e.m_yh || max(pt1.y,pt2.y)<e.m_yl )
							;
						else 
							if( e.bIntersect(&pt1,&pt2,2) )break;
					}
					if(pCS)pCS->GroundToClient(&pts[0],&pt1);
					else COPY_3DPT(pt1,pts[0]);
					if( k<pgr0->ptlist.nuse-1 || (pgr0->ptlist.nuse==1 && e.bPtIn(&pt1,2)) )
					{
						bAdd = TRUE;
						break;
					}*/
				}				
				else if( IsGrText(pgr) )
				{
					GrText *pgr0 = (GrText*)(pgr);
					Envelope e1 = GetEnvelopeOfGraph2(pgr0,0,1,1);
					if(pCS)e1.TransformGrdToClt(pCS,1);
					else ;
					if( e1.bIntersect(&e) )
					{
						bAdd = TRUE;
						break;
					}
				}
				
				pgr = pgr->next;
			}	
		}
		
		if( bAdd )
		{
			if( m_pFilterFunc1!=NULL && !(m_pFilter1->*m_pFilterFunc1)(buf[i].grid) )
					continue;

			m_foundNodeObjs.Add(buf[i]);		
		}
	}

	//pts++;

	if( t->son[0] )
	{
		for( i=0; i<8; i++)
		{
			FindObj(sr,pts,num,pCS,t->son[i]);
		}
		
		return;
	}
	
	return;
}

//从头找ID为grid
int CTree8Search::FindIndex(LONG_PTR grid, int *insert_pos)
{
	int findidx = -1, bnew = 0;
	if( m_arrPIndexes.GetSize()<=0 ){ findidx = 0; bnew = 1; }
	else
	{
		TreeIndex *p0, *p1, *p2;
		int i0 = 0, i1 = m_arrPIndexes.GetSize()-1, i2;
		while(findidx==-1) 
		{
			p0 = m_arrPIndexes.GetAt(i0);
			p1 = m_arrPIndexes.GetAt(i1);
			if( grid<=p0->grid )
			{
				bnew = (grid==p0->grid?0:1);
				findidx = i0;
				break;
			}
			else if( grid>=p1->grid )
			{
				bnew = (grid==p1->grid?0:1);
				findidx = (grid==p1->grid?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (grid==p0->grid?0:1);
				findidx = (grid==p0->grid?i0:(i0+1));
				break;
			}

			p2 = m_arrPIndexes.GetAt(i2);
			
			if( grid<p2->grid )
				i1 = i2;
			else if( grid>p2->grid )
				i0 = i2;
			else
			{
				findidx = i2; bnew = 0;
				break;
			}
		}
	}
	
	*insert_pos = findidx;
	return (bnew==0);
}


TreeIndex* CTree8Search::InsertIndex(LONG_PTR grid, Tree8Node* link)
{
	int insert_pos = -1;

	if( FindIndex(grid,&insert_pos) )
	{
		TreeIndex *pIndex = m_arrPIndexes[insert_pos];
		pIndex->links.Add(link);
		return pIndex;
	}
	else
	{
		TreeIndex *pNewIndex = new TreeIndex;
		
		pNewIndex->grid = grid;
		pNewIndex->links.Add(link);
		
		m_arrPIndexes.InsertAt(insert_pos,pNewIndex);
		return pNewIndex;
	}
}


//获得树的节点分布状态，以分析树在搜索性能
void GetTreeInfo(const Tree8Node *t,TreeInfo *info)
{
	if( !t )return;

	TreeInfo info2;

	int max = -1;
	for( int i=0; i<8; i++)
	{
		memset(&info2,0,sizeof(TreeInfo));
		GetTreeInfo(t->son[i],&info2);

		info->node_sum += info2.node_sum;
		info->leaf_node_sum += info2.leaf_node_sum;
		info->nleaf_node_sum += info2.nleaf_node_sum;
		info->nodeobj_sum += info2.nodeobj_sum;
		info->leaf_nodeobj_sum += info2.leaf_nodeobj_sum;
		info->nleaf_nodeobj_sum += info2.nleaf_nodeobj_sum;

		for( int j=0; j<sizeof(info->leaf_node_stat)/sizeof(info->leaf_node_stat[0]); j++)
		{
			info->leaf_node_stat[j] += info2.leaf_node_stat[j];
			info->nleaf_node_stat[j] += info2.nleaf_node_stat[j];
		}

		if( max<info2.depth )max = info2.depth;
	}

	info->depth = max+1;
	info->node_sum += 1;

	int nobj = t->objs.GetSize();
	info->nodeobj_sum += nobj;

	if( t->son[0] )
	{
		if( nobj==0 )info->nleaf_node_stat[0]++;
		else if( nobj>40 )info->nleaf_node_stat[5]++;
		else info->nleaf_node_stat[(nobj-1)/10+1]++;

		info->nleaf_node_sum += 1;
		info->nleaf_nodeobj_sum += nobj;
	}
	else
	{
		if( nobj==0 )info->leaf_node_stat[0]++;
		else if( nobj>40 )info->leaf_node_stat[5]++;
		else info->leaf_node_stat[(nobj-1)/10+1]++;

		info->leaf_node_sum += 1;
		info->leaf_nodeobj_sum += nobj;
	}

	return;
}

void CTree8Search::GetTreeInfo(TreeInfo *info)const
{
	memset(info,0,sizeof(TreeInfo));
	MyNameSpaceName::GetTreeInfo(m_pHead,info);
}

int CTree8Search::GetObjSum()const
{
	return m_nObjSum;
}

int CTree8Search::SetMaxNumOfResult(int num)
{
	int old = m_nMaxNumOfResult;
	m_nMaxNumOfResult = num;
	return old;
}

BOOL CTree8Search::IsExceedMaxNumOfResult()
{
	return m_bIsExceedMaxNumOfResult;
}


void CTree8Search::SetHitSurfaceInside(BOOL bEnable)
{
	m_bHitSurfaceInside = bEnable;
}

MyNameSpaceEnd