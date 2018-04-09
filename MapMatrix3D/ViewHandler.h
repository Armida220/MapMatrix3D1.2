#pragma once
#include <include/cef_v8.h>

/*
** brief ��ͼ�ص���
*/
class CViewHandler
{
public:
	CViewHandler();
	virtual ~CViewHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CViewHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

