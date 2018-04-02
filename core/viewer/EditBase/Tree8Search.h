// Tree8Search.h: interface for the CTree8Search class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREE8SEARCH_H__59728E33_61F6_47A4_8B26_B81E7546DD3D__INCLUDED_)
#define AFX_TREE8SEARCH_H__59728E33_61F6_47A4_8B26_B81E7546DD3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SearchStruct.h"
#include "CoordSys.h"
#include "FArray.hpp"

MyNameSpaceBegin

struct Tree8Node;

struct TreeIndex
{
	TreeIndex(){
		grid = 0;
	}
	LONG_PTR grid;

	//对象插入后，各个SearchableUnit所在的树节点的指针保存在links中，便于快速删除
	CArray<Tree8Node*,Tree8Node*> links; //
};

struct Tree8NodeObj
{
	Tree8NodeObj(SearchableUnit *u = NULL, LONG_PTR i = 0){
		pu = u; grid = i;
		pIndex = NULL;
	}
	SearchableUnit *pu;
	LONG_PTR grid;

	//附加指针，指向索引，这是为了SplideNode时能快速更新 TreeIndex::links 
	TreeIndex *pIndex;
};

struct Tree8Node
{
	Tree8Node();
	Envelope e0; // initial envelope
	Tree8Node *parent, *son[8];
	CArray<Tree8NodeObj,Tree8NodeObj> objs;

	//当前节点中的对象的体积和估计值，用于分析当前节点下对象的分布密度
	double vsum;
	
	Tree8NodeObj* AddObj(SearchableUnit *u, LONG_PTR i);
	void DelObj(LONG_PTR i);
	BOOL CreateSon0();
	BOOL CreateSon();
};

struct TreeInfo
{
	int depth;  //树深度	
	int node_sum; //节点数目
	int leaf_node_sum, nleaf_node_sum; //叶子和非叶子节点数目
	int nodeobj_sum; //节点包含的对象的数目
	int leaf_nodeobj_sum,nleaf_nodeobj_sum; //叶子和非叶子节点的对象数目
	int leaf_node_stat[6]; //叶子节点的对象数目的统计表
	int nleaf_node_stat[6]; //非叶子节点的对象数目的统计表
};


//采用8叉树搜索对象

class EXPORT_EDITBASE CTree8Search : public CSearcher
{
public:
	CTree8Search();
	virtual ~CTree8Search();

	virtual int GetType(){
		return EBSE_TREE8;
	}
	
	// initialize
	void InitRegion(Envelope* e=NULL);
	Envelope GetRegion(){
		return m_pHead->e0;
	}
	const Tree8Node *FindLeafNodeByPt(PT_3D pt,CCoordSys *pCS);
	// add/delete objects
	virtual void AddObj(LONG_PTR grid, SearchableUnit *pb);
	virtual void DelObj(LONG_PTR grid);
	virtual void DelAll();

	// search
	virtual int  FindObj(const SearchRange* sr);
	virtual BOOL Intersect(LONG_PTR id, Envelope e, CCoordSys *pCS);

	// get results of search
	virtual const LONG_PTR* GetFoundIds(int& num)const;
	const Tree8NodeObj *GetFoundNodeObj(int& num)const;
	virtual const double* GetFoundDis(int& num)const;

	// information about the tree
	void GetTreeInfo(TreeInfo *info)const;	
	virtual int  GetObjSum()const;

	int SetMaxNumOfResult(int num);

	BOOL IsExceedMaxNumOfResult();

	virtual void SetHitSurfaceInside(BOOL bEnable);

protected:
	Tree8Node* InsertNodeObj(SearchableUnit *pu, LONG_PTR grid, Tree8NodeObj *& pNodeObj);
	BOOL SplitNode(Tree8Node *t);

	void SortIdsNodeObj(PT_3D pt,Envelope e,CCoordSys *pCS,bool isSort);
	// search sub-procedure
	void FindObj(const SearchRange *sr, Envelope e, CCoordSys *pCS, Tree8Node *t, BOOL bEntireInclude);
	void FindObj(const SearchRange *sr, PT_3D pt, CCoordSys *pCS, Tree8Node *t);
	//for EBSR_INTERSECTOBJ or EBSR_POLYGON
	void FindObj(const SearchRange *sr, PT_3D *pts, int num, CCoordSys *pCS, Tree8Node *t, int size=sizeof(PT_3D));

	// manage indexes between grids and tree nodes
	int  FindIndex(LONG_PTR grid, int *insert_pos);
	TreeIndex* InsertIndex(LONG_PTR grid, Tree8Node* link);
	
protected:
	// the pointer to the root node of the tree
	Tree8Node *m_pHead;

	// record found results
	CArray<LONG_PTR, LONG_PTR> m_foundIds;
	CArray<Tree8NodeObj,Tree8NodeObj> m_foundNodeObjs;
	CArray<double,double> m_foundDis;
	
	// only for deleting faster an object
	//CFArray<TreeIndex> m_arrIndexes;
	CFArray<TreeIndex*> m_arrPIndexes;
	
	// statistics of the tree
	int m_nHit, m_nUnitSum, m_nObjSum;

	int m_nMaxNumOfResult, m_bIsExceedMaxNumOfResult;

	BOOL m_bHitSurfaceInside;
};


BOOL EXPORT_EDITBASE GrBufferIsIntersect(Envelope e, const GrBuffer *pBuf, BOOL bHitSurfaceInside);

bool EXPORT_EDITBASE GIsIntersectEnvelope(const Envelope *e, const Envelope *e1, CCoordSys *pCS);

MyNameSpaceEnd

#endif // !defined(AFX_TREE8SEARCH_H__59728E33_61F6_47A4_8B26_B81E7546DD3D__INCLUDED_)
