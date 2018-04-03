// Expression.cpp: implementation of the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Expression.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExpression::CExpression()
{
	m_pExpItem = NULL;
}

CExpression::~CExpression()
{

}

//比较一个结果是否匹配表达式
BOOL CExpression::Match(CValueTable &tab)const
{
	return TRUE;
}

//只匹配 tab 中存在的字段 
BOOL CExpression::MatchExistPart(CValueTable &tab)const
{
	return TRUE;
}

//将字符串转换成一个表达式
void CExpression::FromString(LPCTSTR str)
{

}

//表达式转换成字符串
CString CExpression::ToString()const
{
	return CString();
}

//转换成按照字母表排序的字符串
CString CExpression::ToABCString()const
{
	return CString();
}


BOOL CExpression::Compare(const CExpression *exp)const
{
	return TRUE;
}

//设置一个表达式
void CExpression::SetExpression(CValueTable &tab, CExpression::CompareOp op)
{
}

//增加一个嵌套子表达式
BOOL CExpression::Add(const CExpression* exp, CExpression::LogicOp op)
{
	return TRUE;
}


void CExpression::CopyFrom(const CExpression* exp)
{
	return;
}
	
MyNameSpaceEnd