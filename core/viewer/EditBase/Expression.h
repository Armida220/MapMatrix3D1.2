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

//表达式类
//支持逻辑操作“与”“或”，优先级是“与”>“或”；
//支持比较操作“等于”“不等于”“大于”“小于”“大于等于”“小于等于”；
//支持“()”
//字符串格式类似"[INT]LayerID=2110&&([STRING]级别=特级||[STRING]级别=1级)"
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
	//简单表达式只含有一个条件，无括号，无逻辑操作，只有一个比较操作；
	//嵌套子表达式由多个简单表达式构成，可以表达括号的嵌套，也可以表达不含有括号的多条件的表达式；
	//一个表达式对象要么全部由子表达式(SubExpression)构成，要么本身就是简单表达式；
	//也就是说 CExpression::m_pExpItem 和 CExpression::m_arrSubExps 必然有一个为空，或者全空；
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
	
	//比较一个结果是否匹配表达式
	BOOL Match(CValueTable &tab)const;

	//只匹配 tab 中存在的字段 
	BOOL MatchExistPart(CValueTable &tab)const;

	//比较两个表达式是否相同
	BOOL Compare(const CExpression *exp)const;
	
	//将字符串转换成一个表达式
	void FromString(LPCTSTR str);
	
	//表达式转换成字符串
	CString ToString()const;
	
	//设置一个表达式
	void SetExpression(CValueTable &tab, CExpression::CompareOp op);
	
	//增加一个嵌套子表达式
	BOOL Add(const CExpression* exp, CExpression::LogicOp op);

	void CopyFrom(const CExpression* exp);

protected:

	//转换成按照字母表排序的字符串
	CString ToABCString()const;
	
protected:
	//简单表示式
	ExpressionItem *m_pExpItem;
	//嵌套子表达式
	CArray<SubExpression,SubExpression> m_arrSubExps;
};

MyNameSpaceEnd

#endif // !defined(AFX_EXPRESSION_H__253B6D3B_B034_4A89_95DD_FD88D0F07AEA__INCLUDED_)
