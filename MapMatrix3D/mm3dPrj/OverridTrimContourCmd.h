#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideTrimContourCmd ��д�ĵȸ��߱༭����
*/
class COverridTrimContourCmd : public CTrimContourAcrossBankCommand
{
public:
	/**
	* @brief subPtClick ��д�ĵȸ��߱༭����, ֪ͨosgbviewˢ��
	*/
	void subPtClick(PT_3D &pt, int flag);
};

