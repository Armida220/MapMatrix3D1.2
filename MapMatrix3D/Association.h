// Association.h: interface for the CAssociation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSOCIATION_H__B011CF07_0875_41B6_A66F_E9245A93AD9F__INCLUDED_)
#define AFX_ASSOCIATION_H__B011CF07_0875_41B6_A66F_E9245A93AD9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"



// 关联关系类；定义两个Feature对象之间的关联关系，例如，注记对象通常会和等高线关联；
class CAssociation : public CPermanent
{
	DECLARE_DYNAMIC(CAssociation)
public:
	enum
	{
		updateNothing = 0,
		updateDeleted = 1,
		updateModified = 2
	};
	CAssociation(){};
	virtual ~CAssociation(){};

	//基本的函数
	virtual int GetClassType()=0;
	virtual BOOL WriteTo(CValueTable& tab)const=0;
	virtual BOOL ReadFrom(CValueTable& tab ,int idx = 0)=0;

	//创建关联对象
	virtual BOOL CreateAssociation(OUID id1, OUID id2)=0;

	//获得关联对象中的Feature对象，id1 是关联关系的主动对象，id2 是关联关系的被动对象；
	//也就是说，在当前关系中，id1 关联到 id2；
	//表现的结果是，id2变化的时候，id1需要更新，id1变化的时候，id2不需要更新；
	virtual BOOL GetAssocFeatures(OUID &id1, OUID &id2)const=0;

	//当关联的Feature对象被修改后，更新与之关联的Feature对象
	//pFtr1是关联关系的主动对象（更新后的），updateFlag是上述对象的修改类型，
	//pFtr2是关联关系的被动对象的副本；
	//实现者应该在根据pFtr1 和 updateFlag，修改pFtr2，并返回适当的修改类型；
	//当 updateFlag 或者返回值为 updateDeleted 时，关联对象随后也会被删除；
	virtual int UpdateAssocFeatures(CFeature *pFtr1, int updateFlag, CFeature *pFtr2)=0;
};

#endif // !defined(AFX_ASSOCIATION_H__B011CF07_0875_41B6_A66F_E9245A93AD9F__INCLUDED_)
