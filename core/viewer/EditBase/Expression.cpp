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

//�Ƚ�һ������Ƿ�ƥ����ʽ
BOOL CExpression::Match(CValueTable &tab)const
{
	return TRUE;
}

//ֻƥ�� tab �д��ڵ��ֶ� 
BOOL CExpression::MatchExistPart(CValueTable &tab)const
{
	return TRUE;
}

//���ַ���ת����һ�����ʽ
void CExpression::FromString(LPCTSTR str)
{

}

//���ʽת�����ַ���
CString CExpression::ToString()const
{
	return CString();
}

//ת���ɰ�����ĸ��������ַ���
CString CExpression::ToABCString()const
{
	return CString();
}


BOOL CExpression::Compare(const CExpression *exp)const
{
	return TRUE;
}

//����һ�����ʽ
void CExpression::SetExpression(CValueTable &tab, CExpression::CompareOp op)
{
}

//����һ��Ƕ���ӱ��ʽ
BOOL CExpression::Add(const CExpression* exp, CExpression::LogicOp op)
{
	return TRUE;
}


void CExpression::CopyFrom(const CExpression* exp)
{
	return;
}
	
MyNameSpaceEnd