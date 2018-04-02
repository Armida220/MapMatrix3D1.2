// EBWorker.h: interface for the CWorker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBWORKER_H__9D98DCF2_0F4B_4651_9706_22A1B723AF50__INCLUDED_)
#define AFX_EBWORKER_H__9D98DCF2_0F4B_4651_9706_22A1B723AF50__INCLUDED_

#include "DataSource.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Command.h"
#include "Editor.h"
#include "Snap.h"

MyNameSpaceBegin

class CCommand;

struct CMDREG
{
	int id;
	LPPROC_CMDCREATE lpProcCreate;
	TCHAR strAccel[64];
	TCHAR strName[64];
};

//CWorker 定义为工作者/操作员，它继承CEditor的能力，并可以管理命令

class EXPORT_EDITBASE CWorker : public CEditor
{
public:
	CWorker();
	virtual ~CWorker();

	// manage commands
	virtual void CancelCurrentCommand();
	virtual BOOL StartCommand(int id);
	void StartDefaultSelect();
	virtual void SetCurCmdParams(CValueTable& tab);
	void StartPolygonSelect();
	void StartRectSelect();
	
	BOOL RegisterCommand(int id, LPPROC_CMDCREATE lpProc);
	BOOL RegisterSelectStateCommand(LPPROC_CMDCREATE lpProc);
	const CMDREG *GetCmdRegs(int &num);
	
	int GetCurrentCommandId();
	CCommand * GetCurrentCommand();

	// manage input
	virtual void LButtonDown(PT_3D &pt, int flag);
	virtual void LButtonDblClk(PT_3D pt, int flag);
	virtual void RButtonDown(PT_3D pt, int flag);
	virtual void MouseMove(PT_3D &pt, int flag);
	virtual void KeyDown(UINT nChar, int flag);

	virtual BOOL UIRequest(long reqtype, LONG_PTR param);
	virtual void OnSelectState(PT_3D pt, int flag);
	void TryFinishCurProcedure();
	BOOL IsDefaultSelect();

	virtual void CloseAll();

	virtual BOOL SendCmdKeyin(LPCTSTR text);

protected:
	CCommand* CreateCommand(int id);

	void TestStateCommand(PT_3D pt, int rflag);

	CProcedure* GetAllLevelProcedures(int nMsgType, CPtrArray* arr);

	// called before starting the command
	virtual void OnStartCommand(CCommand* pCmd);

	// called after ending the command
	virtual void OnEndCommand(CCommand* pCmd);

protected:
	CArray<CMDREG,CMDREG> m_arrCmdReg;
	
	//等待中的命令队列
	CArray<CCommand*,CCommand*> m_arrWaitCmdObj;

	CArray<LPPROC_CMDCREATE,LPPROC_CMDCREATE> m_arrCreateProc;

	int	m_nLastCmdCreateID;
	int	m_nCurCmdCreateID;

	BOOL m_bDisableCmdInput;

	//是否命令正在执行，此标志用以禁止命令在执行的过程中，启动命令、结束当前命令等操作
	BOOL m_bCmdProcessing;

	CSnap m_snap;

};

MyNameSpaceEnd

#endif // !defined(AFX_EBWORKER_H__9D98DCF2_0F4B_4651_9706_22A1B723AF50__INCLUDED_)
