#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideEditCmd ��д�༭�ڵ����ʹ�ñ༭����
* @brief �л��������ڷֱ���
*/
class COverrideEditVertCmd : public CDlgEditVertexCommand
{
public:
	/**
	* @brief subPtClick ��ԭ����ptClick����ٳ���, ���滻
	*/
	void subPtClick(PT_3D &pt, int flag);

	/**
	* @brief subHitTest ��ԭ����hitTest����ٳ���, ���滻
	*/
	BOOL subHitTest(PT_3D pt, int state)const;

	/**
	* @brief subPtMove ��ԭ����ptMove����ٳ���, ���滻
	*/
	void subPtMove(PT_3D &pt);

protected:
	/**
	* @brief subParentPtClick �滻�����PtClick���룬������ʸ����ͼ�ĳ߶�
	*/
	void subParentPtClick(PT_3D &pt, int flag);
	
	/**
	* @brief subParentHitTest �滻�����HitTest���룬������ʸ����ͼ�ĳ߶�
	*/
	BOOL subParentHitTest(PT_3D pt, int state)const;

	/**
	* @brief subParentPtMove �滻�����PtMove���룬������ʸ����ͼ�ĳ߶�
	*/
	void subParentPtMove(PT_3D &pt);
};

