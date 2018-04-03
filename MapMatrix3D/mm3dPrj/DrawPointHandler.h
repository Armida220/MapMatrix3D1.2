#pragma once
#include <string>
#include <include/cef_v8.h>

/*
** brief ��������Ļص�
*/
class CDrawPointHandler
{
public:
	CDrawPointHandler();
	virtual ~CDrawPointHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CDrawPointHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

