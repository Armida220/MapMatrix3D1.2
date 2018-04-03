// ConvertCoords.h: interface for the ConvertCoords class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTCOORDS_H__8A5C86A0_52A1_49F5_929A_25708C0EAF80__INCLUDED_)
#define AFX_CONVERTCOORDS_H__8A5C86A0_52A1_49F5_929A_25708C0EAF80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tm.h"
#include "Feature.h"
#include "UndoAction.h"


class CDlgDataSource;

// m  «4*4æÿ’Û
BOOL ConvertFtrs(CFtrArray& arr, double *m, CUndoTransform* undo, BOOL bProgress);
BOOL ConvertDS(CDlgDataSource *pDS, double *m, CUndoTransform* undo, BOOL bProgress, CFtrArray* arr);


BOOL ConvertFtrs(CFtrArray& arr, CTM *pTm1, CTM *pTm2, BOOL bProgress);
BOOL ConvertDS(CDlgDataSource *pDS, CTM *pTm1, CTM *pTm2, BOOL bProgress, CFtrArray* arr);

BOOL ConvertVectFile(LPCTSTR filePath0, LPCTSTR filePath1, double *m, BOOL bProgress);
BOOL ConvertVectFile(LPCTSTR filePath0, LPCTSTR filePath1, CTM *pTm1, CTM *pTm2, BOOL bProgress);


BOOL ReadCtrlPts(LPCTSTR filePath, CArray<PT_3D,PT_3D>& pts);


#endif // !defined(AFX_CONVERTCOORDS_H__8A5C86A0_52A1_49F5_929A_25708C0EAF80__INCLUDED_)
