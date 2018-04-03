// Association.h: interface for the CAssociation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSOCIATION_H__B011CF07_0875_41B6_A66F_E9245A93AD9F__INCLUDED_)
#define AFX_ASSOCIATION_H__B011CF07_0875_41B6_A66F_E9245A93AD9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"



// ������ϵ�ࣻ��������Feature����֮��Ĺ�����ϵ�����磬ע�Ƕ���ͨ����͵ȸ��߹�����
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

	//�����ĺ���
	virtual int GetClassType()=0;
	virtual BOOL WriteTo(CValueTable& tab)const=0;
	virtual BOOL ReadFrom(CValueTable& tab ,int idx = 0)=0;

	//������������
	virtual BOOL CreateAssociation(OUID id1, OUID id2)=0;

	//��ù��������е�Feature����id1 �ǹ�����ϵ����������id2 �ǹ�����ϵ�ı�������
	//Ҳ����˵���ڵ�ǰ��ϵ�У�id1 ������ id2��
	//���ֵĽ���ǣ�id2�仯��ʱ��id1��Ҫ���£�id1�仯��ʱ��id2����Ҫ���£�
	virtual BOOL GetAssocFeatures(OUID &id1, OUID &id2)const=0;

	//��������Feature�����޸ĺ󣬸�����֮������Feature����
	//pFtr1�ǹ�����ϵ���������󣨸��º�ģ���updateFlag������������޸����ͣ�
	//pFtr2�ǹ�����ϵ�ı�������ĸ�����
	//ʵ����Ӧ���ڸ���pFtr1 �� updateFlag���޸�pFtr2���������ʵ����޸����ͣ�
	//�� updateFlag ���߷���ֵΪ updateDeleted ʱ�������������Ҳ�ᱻɾ����
	virtual int UpdateAssocFeatures(CFeature *pFtr1, int updateFlag, CFeature *pFtr2)=0;
};

#endif // !defined(AFX_ASSOCIATION_H__B011CF07_0875_41B6_A66F_E9245A93AD9F__INCLUDED_)
