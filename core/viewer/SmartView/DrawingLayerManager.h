// DrawingLayerManager.h: interface for the CDrawingLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWINGLAYERMANAGER_H__1D96B493_D8CD_478F_9F2E_2376AB13EE32__INCLUDED_)
#define AFX_DRAWINGLAYERMANAGER_H__1D96B493_D8CD_478F_9F2E_2376AB13EE32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingLayer.h"

MyNameSpaceBegin

class EXPORT_SMARTVIEW CDrawingLayerManager  
{
public:
	CDrawingLayerManager();
	virtual ~CDrawingLayerManager();

	void InsertDrawingLayer(CDrawingLayer *pLayer, int idx=-1);
	void InsertBeforeDrawingLayer(CDrawingLayer *pLayer, CDrawingLayer *pLayer0);
	void RemoveDrawingLayer(CDrawingLayer *pLayer);
	int GetDrawingLayerCount();
	CDrawingLayer *GetDrawingLayer(int idx);
	int FindDrawingLayer(CDrawingLayer *pLayer);

	Envelope GetDataBound();

	void RemoveAll();
	void BGLoad();
	void PlanBGLoad();
	void OnChangeCoordSys(BOOL bJustScrollScreen);

	BOOL IsIncludeRasterLayer();
	BOOL IsIncludeVectorLayer();

	void SaveForceDrawFlags(CArray<BOOL,BOOL>& arrFlags);
	void SetForceDrawFlags(CArray<BOOL,BOOL>& arrFlags);
	void SetForceDrawFlags(BOOL bForceDraw);

protected:
	CArray<CDrawingLayer*,CDrawingLayer*> m_arrLayers;
};

MyNameSpaceEnd

#endif // !defined(AFX_DRAWINGLAYERMANAGER_H__1D96B493_D8CD_478F_9F2E_2376AB13EE32__INCLUDED_)
