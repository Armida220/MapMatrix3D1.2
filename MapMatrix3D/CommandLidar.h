// CommandLidar.h: interface for the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDLIDAR_H_INCLUDED_)
#define AFX_COMMANDLIDAR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Command.h"
#include "GeoCurve.h"
#include "EditDef.h"

class CDlgDoc;
typedef CArray<FTR_HANDLE,FTR_HANDLE> CIDArray;

class CEditLidarCommand : public CCommand
{

	DECLARE_DYNCREATE(CEditLidarCommand)
public:
	enum
	{
		modifyNone   = 0,
		modifyDelObj = 1,
		modifyDelPt  = 2,
		modifyMovePt = 3
	};
	struct LidarSelection
	{
		LidarSelection()
		{
			m_nSelFlag = SELSTAT_NONESEL;
			keyPt.id = 0;
			keyPt.nPt = -1;
		}
		void Clear()
		{
			m_nSelFlag = SELSTAT_NONESEL;
			boundPts.RemoveAll();
			keyPt.id = 0;
			keyPt.nPt = -1;
		}
		int m_nSelFlag;
		CArray<PT_3D,PT_3D> boundPts;
		struct KeyPoint
		{
			FTR_HANDLE id;
			int nPt;
		}keyPt;
	}m_lidarSelection;

	CEditLidarCommand();
	virtual ~CEditLidarCommand();
	void Start();
	void Abort();
	void Finish();
	virtual CString AccelStr(){
		return _T("EditLidar");
	}
	
	
// 	void UnDo();
// 	void ReDo();
	
	virtual void PtReset(PT_3D &pt);
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual CString Name();
	static BOOL CanGetSelObjs(int flag, BOOL bMultiSel=TRUE, BOOL bOutputTip=TRUE);
	
protected:
	int FindLidarObj(CDlgDoc *pDoc, CIDArray& ids);
	void AimAllSelObjs();
	void EditStepOne();
//	const char *GetTipString(int op);
	
protected:
	int   m_nStep;
	int	  m_nModifyWay;
	int	  m_nKeyPt;
	PT_3DEX m_oldPt, m_newPt;
	FTR_HANDLE m_objID;
	
//	CArray<FTR_HANDLE,FTR_HANDLE> m_idsOld;
//	CArray<FTR_HANDLE,FTR_HANDLE> m_idsNew;

	PT_3D	m_ptSelDragStart;
	BOOL	m_bSelectDrag;
};


class CDeleteLidarCommand : public CEditLidarCommand  
{
	DECLARE_DYNCREATE(CDeleteLidarCommand)
public:
	CDeleteLidarCommand();
	virtual ~CDeleteLidarCommand();
	static CCommand* Create();
	
	
//	void KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void PtClick(PT_3D &pt, int flag);
	void Start();
	virtual CString Name();
	virtual CString AccelStr(){
		return _T("DelLidarPoint");
	}
	
protected:
//	const char * GetTipString(int op);
};

#define PF_KEEPOLD		"KeepOld"
#define PF_MOUSEDEFINE	"MouseDefine"
#define PF_XOFFSET		"XOffset"
#define PF_YOFFSET		"YOffset"
#define PF_ZOFFSET		"ZOffset"

class CMoveLidarCommand : public CEditLidarCommand  
{
	DECLARE_DYNCREATE(CMoveLidarCommand)
public:
	static CCommand* Create();
	CMoveLidarCommand();
	virtual ~CMoveLidarCommand();
	virtual CString AccelStr(){
		return _T("MoveLidar");
	}
	
	void PtMove(PT_3D &pt);
	void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	virtual CString Name();
//	void KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
// 	void InitOpSettingBar(int type, CUIFPropListEx *pList);
// 	void OnPropertyChanged(CUIFProp *pProp);
// 	virtual const char *GetTipString(int op);

	BOOL MoveObject(CGeoMultiPoint *pObj, Envelope e, double dx, double dy, double dz, CGeoMultiPoint *&pNewObj);
	
protected:
	CPtrArray m_ptrObjs;
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
	BOOL  m_bKeepOld;
	BOOL  m_bMouseDefine;
	
	double m_lfDeltaX, m_lfDeltaY, m_lfDeltaZ;
};

#define PF_ZLIDAR  _T("ZLidar")
#define PF_ZLIDARSELMODE  _T("ZLidarSelMode")
class CModifyZLidarCommand : public CEditLidarCommand
{
	DECLARE_DYNCREATE(CModifyZLidarCommand)
	enum
	{
		modeNormal = 0,
		modePolygon = 1,
		modeRefObj = 2
	};
public:
	CModifyZLidarCommand();
	virtual ~CModifyZLidarCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("ModifyZLidar");
	}

	virtual void PtReset(PT_3D &pt);
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	void Start();
	void Abort();
	CProcedure *GetActiveSonProc(int nMsgType);
	static BOOL CheckObjForContour(CGeometry *pObj);
	
	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	virtual void Finish();
protected:
	CDrawCurveProcedure *m_pDrawProc;
	CFeature *m_pFtr;
	int m_nSelMode;
	double m_lfResultZ;
};

#define  PF_MODE		_T("Mode")
#define  PF_GRIDSIZE	_T("GridSize")
class CInterpolateLidarCommand : public CCommand
{
	DECLARE_DYNCREATE(CInterpolateLidarCommand)
	enum
	{
		modeAddPt = 0,
		modeModifyPt = 1
	};
public:
	CInterpolateLidarCommand();
	virtual ~CInterpolateLidarCommand();
	virtual CString Name();
	static CCommand* Create(){
		return new CInterpolateLidarCommand;
	}
	virtual CString AccelStr(){
		return _T("InterpolateLidar");
	}

//	void Back();
//	void KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	virtual void Back();
	virtual int  GetState();
	void PtReset(PT_3D &pt);
	void PtMove(PT_3D &pt);
	void PtClick(PT_3D &pt, int flag);
	void Start();
	void Abort();
	CProcedure *GetActiveSonProc(int nMsgType);
	
// 	void ReDo(); 
// 	void UnDo();
	


	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
// 	void InitOpSettingBar(int type, CUIFPropListEx *pList);
// 	void OnPropertyChanged(CUIFProp *pProp);
// 	virtual const char *GetTipString(int op);	
// 	void UpdateSettings(BOOL bSave);
	virtual void Finish();
	
private:
	
protected:
	int	  m_nStep;
	int   m_nMode;//modeAddPt, 绘制，modeModifyPt, 选取

	//内插点间距
	float m_fGridSize;

	CDrawCurveProcedure *m_pDrawProc;
	CFeature *m_pFtr;
	
	CArray<FTR_HANDLE,FTR_HANDLE> m_idsOld;
	CArray<FTR_HANDLE,FTR_HANDLE> m_idsNew;
};



#endif