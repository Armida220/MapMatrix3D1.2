#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideInsertVertCmd ��д����ڵ����ʹ�ò���ڵ�����
* @brief �л���������ʸ����ͼ�ֱ���
*/
class COverrideInsertVertCmd : public CDlgInsertVertexCommand
{
public:
	/**
	* @brief subHitTest ��д����л��¼���������������
	* @brief ʸ����ͼ�ֱ���
	*/
	BOOL subHitTest(PT_3D pt, int state)const;
};

