#pragma once
#include <include/cef_v8.h>

/*
** brief ��Բ�ص���
*/
class CDrawCircleHandler
{
public:
	CDrawCircleHandler();
	virtual ~CDrawCircleHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CDrawCircleHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

