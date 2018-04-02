// Expression.h: interface for the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPRESSION_H__253B6D3B_B034_4A89_95DD_FD88D0F07AEA__INCLUDED_)
#define AFX_EXPRESSION_H__253B6D3B_B034_4A89_95DD_FD88D0F07AEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Valuetable.h"

MyNameSpaceBegin

//���ʽ��
//֧���߼��������롱���򡱣����ȼ��ǡ��롱>���򡱣�
//֧�ֱȽϲ��������ڡ��������ڡ������ڡ���С�ڡ������ڵ��ڡ���С�ڵ��ڡ���
//֧�֡�()��
//�ַ�����ʽ����"[INT]LayerID=2110&&([STRING]����=�ؼ�||[STRING]����=1��)"
class CExpression
{
public:
	enum CompareOp
	{
		Equal=1,
		NotEqual=2,
		Greater=3,
		Less=4,
		GreaterEqual=5,
		LessEqual=6
	};
	enum LogicOp
	{
		And=1,
		Or=2
	};
protected:
	//�򵥱��ʽֻ����һ�������������ţ����߼�������ֻ��һ���Ƚϲ�����
	//Ƕ���ӱ��ʽ�ɶ���򵥱��ʽ���ɣ����Ա�����ŵ�Ƕ�ף�Ҳ���Ա�ﲻ�������ŵĶ������ı��ʽ��
	//һ�����ʽ����Ҫôȫ�����ӱ��ʽ(SubExpression)���ɣ�Ҫô������Ǽ򵥱��ʽ��
	//Ҳ����˵ CExpression::m_pExpItem �� CExpression::m_arrSubExps ��Ȼ��һ��Ϊ�գ�����ȫ�գ�
	struct SubExpression
	{
		CExpression *exp;
		LogicOp op;
	};
	struct ExpressionItem
	{
		char field[32];
		_variant_t value;
		CompareOp op;
	};

public:
	CExpression();
	virtual ~CExpression();
	
	//�Ƚ�һ������Ƿ�ƥ����ʽ
	BOOL Match(CValueTable &tab)const;

	//ֻƥ�� tab �д��ڵ��ֶ� 
	BOOL MatchExistPart(CValueTable &tab)const;

	//�Ƚ��������ʽ�Ƿ���ͬ
	BOOL Compare(const CExpression *exp)const;
	
	//���ַ���ת����һ�����ʽ
	void FromString(LPCTSTR str);
	
	//���ʽת�����ַ���
	CString ToString()const;
	
	//����һ�����ʽ
	void SetExpression(CValueTable &tab, CExpression::CompareOp op);
	
	//����һ��Ƕ���ӱ��ʽ
	BOOL Add(const CExpression* exp, CExpression::LogicOp op);

	void CopyFrom(const CExpression* exp);

protected:

	//ת���ɰ�����ĸ��������ַ���
	CString ToABCString()const;
	
protected:
	//�򵥱�ʾʽ
	ExpressionItem *m_pExpItem;
	//Ƕ���ӱ��ʽ
	CArray<SubExpression,SubExpression> m_arrSubExps;
};

MyNameSpaceEnd

#endif // !defined(AFX_EXPRESSION_H__253B6D3B_B034_4A89_95DD_FD88D0F07AEA__INCLUDED_)
