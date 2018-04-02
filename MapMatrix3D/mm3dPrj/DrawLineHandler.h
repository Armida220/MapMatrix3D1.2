#pragma once
#include <include/cef_v8.h>
/*
** brief ��������Ļص�
*/
class CDrawLineHandler
{
public:
	CDrawLineHandler();
	virtual ~CDrawLineHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CDrawLineHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

