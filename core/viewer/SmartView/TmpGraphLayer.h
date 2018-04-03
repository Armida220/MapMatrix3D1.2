// TmpGraphLayer.h: interface for the CTmpGraphLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMPGRAPHLAYER_H__08FAD719_C9BB_4945_AFEF_307BB74082D3__INCLUDED_)
#define AFX_TMPGRAPHLAYER_H__08FAD719_C9BB_4945_AFEF_307BB74082D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RealtimeDrawingLayer.h"
#include "FArray.hpp"

MyNameSpaceBegin

struct GrElement;

//临时图形的图层
class EXPORT_SMARTVIEW CTmpGraphLayer : public CRealtimeDrawingLayer  
{
public:
	enum DataType
	{
		typeNone = 0,
		typeGrBuffer2d = 1,
		typeGrBuffer = 2
	};

	enum EraseType
	{
		eraseNone = 0,
		eraseXOR = 1,
		eraseBMP = 2
	};

	CTmpGraphLayer(int nDataType);
	virtual ~CTmpGraphLayer();
	
	void SetDataType(int nDataType);
	int GetDataType(){
	return m_nType;
	}
	void *OpenObj(LONG_PTR objnum);
	void DelObj(LONG_PTR objnum);
	void DelAllObjs();

	virtual BOOL CanErase();
	virtual void Erase();
	
	//绘制
	virtual void Draw();
	void Reset();

	void SetMarkParams(BOOL bEnable, COLORREF color, float wid);
	BOOL IsEnableMark();
	
	void SetObjMark(LONG_PTR objnum, BOOL bMark);
	BOOL IsObjMark(LONG_PTR objnum);
	
protected:
	//if objnum found, return TRUE, and insert_idx is its index;
	//if not found, return FALSE, and insert_idx is its suit pos inserted at, 
	//that's next pos to the pos this new obj wound be inserted at.
	BOOL FindObj(LONG_PTR objnum, int& insert_idx);
	
	virtual void *CreateObject();
	virtual void ReleaseObject(void *pBuf);
	
	void DrawBuffers();

	void Save();
	
protected:
	int m_nType;
	CFArray<GrElement*> m_arrPEles;

	BOOL m_bEnableMark;
	COLORREF m_clrMark;
	float m_fMarkWid;

private:
	BOOL m_bValidXOR;

public:
	//擦除类型
	int m_nEraseType;

	//绘制时，是否作坐标系换算
	BOOL m_bUseCoordSys;
};


class EXPORT_SMARTVIEW CVariantDragLayer : public CRealtimeDrawingLayer  
{
public:
	enum EraseType
	{
		eraseNone = 0,
		eraseXOR = 1,
		eraseBMP = 2
	};

	CVariantDragLayer();
	virtual ~CVariantDragLayer();

	const GrBuffer2d* GetBuffer2d()const{
		return m_pDrawBuf2d;
	}
	const GrBuffer* GetBuffer()const{
		return m_pDrawBuf;
	}
	void SetBuffer(GrBuffer2d *pBuf);
	void AddBuffer(GrBuffer2d *pBuf);

	void SetBuffer(GrBuffer *pBuf);
	void AddBuffer(GrBuffer *pBuf);

	virtual BOOL CanErase();
	virtual void Erase();
	
	//绘制
	virtual void Draw();

	BOOL IsBufferChanged()const{
		return m_bBufferChanged;
	}

	void Reset();

public:
	//擦除类型
	int m_nEraseType;
	
	//绘制时，是否作坐标系换算
	BOOL m_bUseCoordSys;

	//是GrBuffer还是GrBuffer2D;
	BOOL m_bUseGrBuffer;

private:
	GrBuffer2d *m_pDrawBuf2d;
	GrBuffer2d *m_pEraseBuf2d;

	GrBuffer *m_pDrawBuf;
	GrBuffer *m_pEraseBuf;

	BOOL m_bBufferChanged;
};

class EXPORT_SMARTVIEW CConstDragLayer : public CTmpGraphLayer  
{
public:
	CConstDragLayer(int nDataType):CTmpGraphLayer(nDataType){
	}
	virtual ~CConstDragLayer(){
	}
};

class EXPORT_SMARTVIEW CAddConstDragLayer : public CTmpGraphLayer  
{
public:
	CAddConstDragLayer(int nDataType):CTmpGraphLayer(nDataType){
		m_nEraseType = eraseNone;
	}
	virtual ~CAddConstDragLayer(){
	}

	virtual BOOL CanErase(){
		return TRUE;
	}
	virtual void Erase(){
	}
};


MyNameSpaceEnd

#endif // !defined(AFX_TMPGRAPHLAYER_H__08FAD719_C9BB_4945_AFEF_307BB74082D3__INCLUDED_)
