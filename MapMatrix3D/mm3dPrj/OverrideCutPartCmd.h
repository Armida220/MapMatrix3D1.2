#pragma once
#include "DlgCommand.h"


/**
* @brief COverrideCutPartCmd ��д�޼�����,��������޼���
* @brief ����osgbview
*/
class COverrideCutPartCmd : public CCutPartCommand
{
public:
	/**
	* @brief subPtClick �����滻ԭ�����޼������ptClick
	* @brief ����������osgbview���͸�����Ϣ
	*/
	void subPtClick(PT_3D &pt, int flag);
};

