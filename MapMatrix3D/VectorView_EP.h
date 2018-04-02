// VectorView_EP.h: interface for the CVectorView_EP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORVIEW_EP_H__7CF63D89_FD77_475E_94F1_57A6663C6BB1__INCLUDED_)
#define AFX_VECTORVIEW_EP_H__7CF63D89_FD77_475E_94F1_57A6663C6BB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VectorView_new.h"

#define EPREGION_FTRCODE		_T("RegionFtr")

class CEPSectionView;
class CEPSectionViewCenterPoint0
{
public:
	CEPSectionViewCenterPoint0(CEPSectionView *p);
	~CEPSectionViewCenterPoint0();
	void Save();
	void Restore();
	
private:
	CEPSectionView *pView;
	PT_3D ptCenter;
};

//断面视图
class CEPSectionView : public CVectorView_new  
{
	friend class CEPSectionViewCenterPoint0;
public:
	DECLARE_DYNCREATE(CEPSectionView)
	CEPSectionView();
	virtual ~CEPSectionView();


	virtual void GroundToImage(PT_3D *pt0, PT_4D *pt1);
	virtual void ImageToGround(PT_4D *pt0, PT_3D *pt1);
	//virtual void ImageToClient(PT_4D *pt0, PT_4D *pt1);

	virtual void GroundToVectorLay(PT_3D *pt0, PT_4D *pt1);

	//virtual void ClientToImage(PT_4D *pt0, PT_4D *pt1);

	virtual BOOL InitBmpLayer();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
// 	virtual void AddObjtoVectLay(long handle, const GrBuffer *pBuf, BOOL bGround=TRUE, CVectorLayer *pLL=NULL, CVectorLayer *pLR=NULL);
// 	void ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer *outL, GrBuffer *outR);


//	virtual BOOL LoadDocData();

protected:
	//{{AFX_MSG(CEPSectionView)
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	double m_lfGScaleX, m_lfGScaleZ;

	CArray<PT_3D,PT_3D> m_arrRoadPts;
	double   m_fRoadWid;
};

//线路视图
class CEPRoadView : public CVectorView_new  
{
public:
	DECLARE_DYNCREATE(CEPRoadView)
	CEPRoadView();
	virtual ~CEPRoadView();
	
protected:
	virtual void GroundToImage(PT_3D *pt0, PT_4D *pt1);
	virtual void ImageToGround(PT_4D *pt0, PT_3D *pt1);
	
	virtual void GroundToVectorLay(PT_3D *pt0, PT_4D *pt1);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	virtual void DrawBkgnd(GrBuffer *buf);
	
protected:
	//{{AFX_MSG(CEPRoadView)
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CArray<double,double> m_arrRoadDis;
	CArray<PT_3D,PT_3D> m_arrRoadPts;
	double   m_fRoadWid;
	
};

#endif // !defined(AFX_VECTORVIEW_EP_H__7CF63D89_FD77_475E_94F1_57A6663C6BB1__INCLUDED_)
