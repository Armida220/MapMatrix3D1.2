// Graph2dSearch.h: interface for the CGraph2dSearch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPH2DSEARCH_H__EB02BC63_A7F3_4C70_9E95_B025EF934C51__INCLUDED_)
#define AFX_GRAPH2DSEARCH_H__EB02BC63_A7F3_4C70_9E95_B025EF934C51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ObjDataBlockSize	50

#include "FArray.hpp"

MyNameSpaceBegin

struct Graph2d;
class CCoordSys;

class CGraph2dSearch  
{
public:
	struct ObjData
	{
		ObjData(){
			id = 0;
			nused = 0; next = NULL;
			xl = xh = yl = yh = 0;
		}
		LONG_PTR id;
		int nused;
		Graph2d *list[ObjDataBlockSize];
		float xl,xh,yl,yh;
		ObjData *next;
	};
public:
	CGraph2dSearch();
	virtual ~CGraph2dSearch();

	void AddObj(LONG_PTR id, CArray<Graph2d*, Graph2d*>& arr);
	void DelObj(LONG_PTR id);
	void DelAll();

	void MarkObjsInRect(double xl, double xh, double yl, double yh, CCoordSys *pCS);
	void ClearMarks();
	int  GetObjSum(){
		return m_arrObjs.GetSize();
	}

	float m_fDrawCellKX, m_fDrawCellKY, m_fDrawCellAngle;

private:
	CGraph2dSearch::ObjData *CreateObj(LONG_PTR id, CArray<Graph2d*, Graph2d*>& arr);
	BOOL FindObj(LONG_PTR id, int& insert_idx);
	void FreeObj(ObjData *p);

	CFArray<ObjData*> m_arrObjs;
	CArray<ObjData*,ObjData*> m_arrFoundObjs;
};


MyNameSpaceEnd

#endif // !defined(AFX_GRAPH2DSEARCH_H__EB02BC63_A7F3_4C70_9E95_B025EF934C51__INCLUDED_)
