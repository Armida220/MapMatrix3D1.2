// UndoAction.h: interface for the CUndoAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOACTION_H__F7508733_C169_47C4_AF3C_616FD9EA4D42__INCLUDED_)
#define AFX_UNDOACTION_H__F7508733_C169_47C4_AF3C_616FD9EA4D42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "Editor.h"


#define DECLARE_CLONE(class_name) \
public: \
	virtual CUndoAction* Clone(){ \
	CUndoAction *pNewObj = new class_name(m_pEditor,m_strName); \
	if( pNewObj ){ pNewObj->CopyFrom(this); } \
	return pNewObj; \
}\

	

MyNameSpaceBegin




class EXPORT_EDITBASE CUndoAction  
{
public:
	CUndoAction();
	CUndoAction(CEditor *p, LPCTSTR name);
	virtual ~CUndoAction();

	virtual void Undo(){}
	virtual void Redo(){}
	virtual void CopyFrom(const CUndoAction *pObj);

	virtual void Commit();
	virtual void DestoryNewObjs(){}
	virtual void DestoryOldObjs(){}

	DECLARE_CLONE(CUndoAction)

	CEditor *m_pEditor;
	CString m_strName;
};

class EXPORT_EDITBASE CUndoBatchAction : public CUndoAction  
{
public:
	CUndoBatchAction();
	CUndoBatchAction(CEditor *p, LPCTSTR name);
	virtual ~CUndoBatchAction();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();
	
	DECLARE_CLONE(CUndoBatchAction)

	void AddAction(CUndoAction* pAction);
		
	CArray<CUndoAction*,CUndoAction*> arrActions;
};


class EXPORT_EDITBASE CUndoFtrs : public CUndoAction
{
public:
	CUndoFtrs();
	CUndoFtrs(CEditor *p, LPCTSTR name);
	virtual ~CUndoFtrs();

	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	void DelInvalidUndoInfo();
	virtual void Commit();
	virtual void DestoryNewObjs();
	virtual void DestoryOldObjs();

	DECLARE_CLONE(CUndoFtrs)

	void AddNewFeature(FTR_HANDLE handle);
	void AddOldFeature(FTR_HANDLE handle);

	CArray<FTR_HANDLE,FTR_HANDLE> arrOldHandles;
	CArray<FTR_HANDLE,FTR_HANDLE> arrNewHandles;
};

class EXPORT_EDITBASE CUndoVertex : public CUndoAction
{
public:
	CUndoVertex();
	CUndoVertex(CEditor *p, LPCTSTR name);
	virtual ~CUndoVertex();

	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();

	DECLARE_CLONE(CUndoVertex)
	
	FTR_HANDLE handle;
	int nPtType, nPtIdx;
	PT_3DEX ptOld, ptNew;
	BOOL bRepeat;
};


class EXPORT_EDITBASE CUndoShape : public CUndoAction
{
public:
	CUndoShape();
	CUndoShape(CEditor *p, LPCTSTR name);
	virtual ~CUndoShape();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();
	
	DECLARE_CLONE(CUndoShape)
		
	FTR_HANDLE handle;
	CArray<PT_3DEX,PT_3DEX> arrPtsOld, arrPtsNew;
};


class EXPORT_EDITBASE CUndoTransform : public CUndoAction
{
public:
	CUndoTransform();
	CUndoTransform(CEditor *p, LPCTSTR name);
	virtual ~CUndoTransform();

	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();

	DECLARE_CLONE(CUndoTransform)
	
	double matrix[16];
	CArray<FTR_HANDLE,FTR_HANDLE> arrHandles;
};



class EXPORT_EDITBASE CUndoNewDelLayer : public CUndoAction
{
public:
	CUndoNewDelLayer();
	CUndoNewDelLayer(CEditor *p, LPCTSTR name);
	virtual ~CUndoNewDelLayer();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();
	
	DECLARE_CLONE(CUndoNewDelLayer)
	
	bool isDel;
	CArray<CFtrLayer*,CFtrLayer*> arrObjs;
};



class EXPORT_EDITBASE CUndoModifyLayer : public CUndoAction
{
public:
	CUndoModifyLayer();
	CUndoModifyLayer(CEditor *p, LPCTSTR name);
	virtual ~CUndoModifyLayer();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();
	
	DECLARE_CLONE(CUndoModifyLayer)

	CString field;	
	CArray<CFtrLayer*,CFtrLayer*> arrLayers;
	_variant_t newVar;
	CArray<_variant_t,_variant_t> arrOldVars;
};

class EXPORT_EDITBASE CUndoModifyLayer0 : public CUndoAction
{
public:
	CUndoModifyLayer0();
	CUndoModifyLayer0(CEditor *p, LPCTSTR name);
	virtual ~CUndoModifyLayer0();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();
	
	DECLARE_CLONE(CUndoModifyLayer0)
		
	CString field;	
	CArray<CFtrLayer*,CFtrLayer*> arrLayers;
	CArray<_variant_t,_variant_t> arrNewVars;
	CArray<_variant_t,_variant_t> arrOldVars;
};


class EXPORT_EDITBASE CUndoModifyProperties : public CUndoAction
{
public:
	CUndoModifyProperties();
	CUndoModifyProperties(CEditor *p, LPCTSTR name);
	virtual ~CUndoModifyProperties();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();

	void SetModifyProperties(FTR_HANDLE handle, CValueTable &oldtab,CValueTable &newtab,BOOL isBaseAttr );

	void SetModifyProp(CFeature *pFtr, LPCTSTR fieldName, CVariantEx *pOldValue, CVariantEx *pNewValue);

	//只保留部分字段，fieldList存放多个字段名，空格隔开
	void JustKeepBasicFields(CString& fieldList);
	
	DECLARE_CLONE(CUndoModifyProperties)

	CArray<FTR_HANDLE,FTR_HANDLE> arrHandles;
	CValueTable oldVT,newVT;
	CValueTable XoldVT,XnewVT;
};

class EXPORT_EDITBASE CUndoModifyLayerOfObj: public CUndoAction
{
public:
	CUndoModifyLayerOfObj();
	CUndoModifyLayerOfObj(CEditor *p, LPCTSTR name);
	virtual ~CUndoModifyLayerOfObj();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();

	void ModifyLayerOfObj(FTR_HANDLE handle, int oldlayer, int newlayer);
	
	DECLARE_CLONE(CUndoModifyLayerOfObj)

	CArray<FTR_HANDLE,FTR_HANDLE> arrHandles;
//	CStringArray oldLayerArr,newLayerArr;
	CUIntArray oldLayerArr, newLayerArr;
	CValueTable oldVT,newVT;
};


MyNameSpaceEnd

#endif // !defined(AFX_UNDOACTION_H__F7508733_C169_47C4_AF3C_616FD9EA4D42__INCLUDED_)
