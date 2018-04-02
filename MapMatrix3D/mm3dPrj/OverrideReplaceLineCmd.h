#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideReplaceLineCmd ��д�����޲�����
*/
class COverrideReplaceLineCmd : public CReplaceLinesCommand
{
public:
	/**
	* @brief subPtClick �������д�������ڶ�ά��ͼ�����ű�
	*/
	void subPtClick(PT_3D &pt, int flag);

	/**
	* @brief subPtMove ����ƶ���д�������ڶ�ά��ͼ�����ű�
	*/
	void subPtMove(PT_3D &pt);

	/**
	* @brief DrawPointTip �滻����tip
	*/
	void subDrawPointTip(CGeometry *pObj, PT_3D pt, GrBuffer *buf);

};

