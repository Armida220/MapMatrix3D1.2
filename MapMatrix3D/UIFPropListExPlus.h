// UIFPropListExPlus.h: interface for the CUIFPropListExPlus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFPROPLISTEXPLUS_H__84692DA9_F670_492F_A653_586E5E8C7114__INCLUDED_)
#define AFX_UIFPROPLISTEXPLUS_H__84692DA9_F670_492F_A653_586E5E8C7114__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIFPropEx.h"
#include "UIEXTENSION\UIFPropListEx.h"
#include "UIParam.h"

class CUIFPropListExPlus : public CUIFPropListEx  
{
public:
	CUIFPropListExPlus();
	virtual ~CUIFPropListExPlus();
	void LoadParams(const CUIParam *param);
	void ShowParams(const CUIParam *param);
	void ClearParams();
	void GetCmdParams(CValueTable &tab);
	const CUIParam *GetOriginalParams();
protected:
	CUIFProp *CreateProp(const CUIParam::ParamItem& item);
	void SetPropValue(CUIFProp *pProp, const CUIParam::ParamItem& item);
private:
	CUIParam *m_pLoadParam;
};

#endif // !defined(AFX_UIFPROPLISTEXPLUS_H__84692DA9_F670_492F_A653_586E5E8C7114__INCLUDED_)
