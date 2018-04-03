#pragma once
#include "CoordCenter.h"


/*
*	��д����ת����֧�ֶ�����Ӱ�����ǰ������ķ�ʽ
*	
*/
class COsgCoordCenter : public CCoordCenter
{
public:
	COsgCoordCenter();
	virtual ~COsgCoordCenter();

	/**
	* @brief Init ����ϵ��ʼ��	
	* @param core ���̵Ĳ����������ɹ�����Ϣ
	* @param lImg ����ǰ������ĵ�һ��Ӱ��
	* @param rImg ����ǰ������ĵڶ���Ӱ��
	*/
	virtual bool InitByTwoImg(CoreObject& core, CString lImg, CString rImg);

protected:
	/**
	* @brief SwitchToTwoImg ת��������Ӱ���ģʽ
	* @param lImg ����ǰ������ĵ�һ��Ӱ��
	* @param rImg ����ǰ������ĵڶ���Ӱ��
	*/
	virtual bool SwitchToTwoImg(CString lImg, CString rImg);

};

