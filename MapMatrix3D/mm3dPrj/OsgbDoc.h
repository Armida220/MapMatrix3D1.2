#pragma once
#include "editbasedoc.h"

// COsgbDoc document

/**
* @brief COsgbDoc ������б��Ӱ��ͼ����дdlgdoc��
*/
class COsgbDoc : public CDlgDoc
{
public:
	COsgbDoc();
	virtual ~COsgbDoc();

	/**
	* @brief LButtonDown ��дLButtonDown, ��Ҫ��Ϊ�˽����osgbview ���е��ʱ���������vecview��ߵķֱ���
	*/
	void subLButtonDown(PT_3D &pt, int flag);

	/**
	* @brief workerMouseMv ����osgbview�ƶ�ʱ�򣬽��в����ڷֱ��ʵĶ��߲�׽
	*/
	void subParentMouseMv(PT_3D &pt, int flag);

	/**
	* @brief MouseMove ����osgbview�ƶ�ʱ�򣬰�worker��MouseMove�滻�ɸ�д�ģ���������ֱ��ʵ�����
	*/
	void subMouseMove(PT_3D &pt, int flag);

	/**
	* @brief TestStateCommand ��д����༭����Ͳ��붥�㣬��������ֱ��ʵ�����
	*/
	void TestStateCommand(PT_3D pt, int state);

public:

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnOpenOsgb();

	afx_msg void OnRemoveOsgb();


		
};
